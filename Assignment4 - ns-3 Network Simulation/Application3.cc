/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// ============================================================ //
// BASED ON tcp-bulk-send.cc                                    //
// ============================================================ //

// Network topology
//
//       n0 ----------- n1
//            1 Mbps
//            10 ms
//
// - Flow from n0 to n1 using BulkSendApplication.
//


// ============================================================ //
// NOTE: You can check the "magic" number of a pcap file with   //
//       the following command:                                 //
//                                                              //
//                    od -N4 -tx1 filename.pcap                 //
//                                                              //
// ============================================================ //

#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Application3");   // $export NS_LOG=Application3 to debug

static bool firstCwnd = true;
static bool firstSshThr = true;
static Ptr<OutputStreamWrapper> cWndStream;
static Ptr<OutputStreamWrapper> ssThreshStream;
static uint32_t cWndValue;
static uint32_t ssThreshValue;

static void
CwndTracer (uint32_t oldval, uint32_t newval)
{
  if (firstCwnd)
    {
      *cWndStream->GetStream () << "0.0 " << oldval << std::endl;
      firstCwnd = false;
    }
  *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  cWndValue = newval;

  if (!firstSshThr)
    {
      *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << ssThreshValue << std::endl;
    }
}

static void
TraceCwnd (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer));
}


int
main (int argc, char *argv[])
{

  bool tracing = true;
  bool nanosec = false;
  bool pcap = true;
  uint32_t maxBytes = 0;
  std::string transport_prot = "TcpWestwood";

//
// Allow the user to override any of the defaults at
// run-time, via command-line arguments
//
  CommandLine cmd;
  cmd.AddValue ("tracing", "Flag to enable tracing", tracing);
  cmd.AddValue ("nanosec",  "Flag to use nanosecond timestamps for pcap as default", nanosec);
  cmd.AddValue ("maxBytes",
                "Total number of bytes for application to send", maxBytes);
  cmd.AddValue ("prot", "Transport protocol to use: TcpNewReno, TcpHybla, TcpWestwood, TcpScalable, TcpVegas ", transport_prot);
  cmd.Parse (argc, argv);

  if (transport_prot.compare ("TcpNewReno") == 0)
    {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId ()));
    }
    else if (transport_prot.compare ("TcpHybla") == 0)
    {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpHybla::GetTypeId ()));
    }
    else if (transport_prot.compare ("TcpWestwood") == 0)
    {
    // the default protocol type in ns3::TcpWestwood is WESTWOOD
    // for WESTWOODPLUS, add Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
      Config::SetDefault ("ns3::TcpWestwood::FilterType", EnumValue (TcpWestwood::TUSTIN));
    }
    else if (transport_prot.compare ("TcpScalable") == 0)
    {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpScalable::GetTypeId ()));
    }
    else if (transport_prot.compare ("TcpVegas") == 0)
    {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpVegas::GetTypeId ()));
    }
    else
    {
      NS_LOG_DEBUG ("Invalid TCP version");
      exit (1);
    }

//
// If requested via the --nanosec cmdline flag, generate nanosecond timestamp for pcap traces
//
  if (nanosec)
    {
      Config::SetDefault ("ns3::PcapFileWrapper::NanosecMode",   BooleanValue (true));
    }

//
// Explicitly create the nodes required by the topology (shown above).
//
  NS_LOG_INFO ("Create nodes.");
  NodeContainer nodes;
  nodes.Create (2);

  NS_LOG_INFO ("Create channels.");

//
// Explicitly create the point-to-point link required by the topology (shown above).
//
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("10ms"));
  pointToPoint.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("10KB"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

//
// Install the internet stack on the nodes
//
  InternetStackHelper internet;
  internet.Install (nodes);

//
// We've got the "hardware" in place.  Now we need to add IP addresses.
//
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (devices);

  NS_LOG_INFO ("Create Applications.");

//
// Create a BulkSendApplication and install it on node 0
//
  uint16_t port = 9;  // well-known echo port number


  BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (i.GetAddress (1), port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer sourceApps = source.Install (nodes.Get (0));
  sourceApps.Start (Seconds (0.0));
  sourceApps.Stop (MilliSeconds (1800.0));

//
// Create a PacketSinkApplication and install it on node 1
//
  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = sink.Install (nodes.Get (1));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (MilliSeconds (1800.0));

//
// Create CBR Agents
//
  uint32_t packetSize = 512;
  uint16_t cbrPort = 8000, n_cbr = 5;

  ApplicationContainer cbr;
  ApplicationContainer cbrSinkApps[n_cbr];

  OnOffHelper onOff ("ns3::UdpSocketFactory", InetSocketAddress (i.GetAddress(1), cbrPort));
  onOff.SetConstantRate (DataRate ("300Kbps"), packetSize);

  PacketSinkHelper cbrSink ("ns3::UdpSocketFactory",
                            InetSocketAddress (Ipv4Address::GetAny (), cbrPort));
  for (uint16_t cbrIndex = 1; cbrIndex <= n_cbr; cbrIndex++)
    {
      onOff.SetAttribute ("Remote",
                          AddressValue (InetSocketAddress (i.GetAddress(1), cbrPort+cbrIndex)));

      cbr = onOff.Install (nodes.Get (0));
    
      // Start CBR
      cbr.Start (MilliSeconds (200.0 * cbrIndex));
      cbr.Stop (MilliSeconds (1800.0));
    
      cbrSink.SetAttribute ("Local",
                          AddressValue (InetSocketAddress (i.GetAddress(1), cbrPort+cbrIndex)));
      cbrSinkApps[cbrIndex-1] = cbrSink.Install (nodes.Get (1));
      cbrSinkApps[cbrIndex-1].Start (MilliSeconds (200.0 * cbrIndex));
      cbrSinkApps[cbrIndex-1].Stop (MilliSeconds (1800.0));
    }

//
// Set up tracing if enabled
//
  if (tracing)
    {
      std::ofstream ascii;
      Ptr<OutputStreamWrapper> ascii_wrap;
      ascii.open ((transport_prot + "-ascii").c_str ());
      ascii_wrap = new OutputStreamWrapper ((transport_prot + "-ascii").c_str (),
                                            std::ios::out);
      internet.EnableAsciiIpv4All (ascii_wrap);

      Simulator::Schedule (Seconds (0.00001), &TraceCwnd, transport_prot + "-cwnd.data");
    }

  if (pcap)
    {
      pointToPoint.EnablePcapAll (transport_prot, false);  // void EnablePcapAll (std::string prefix, bool promiscuous = false);
    }

// Flow monitor
  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();


//
// Now, do the actual simulation.
//
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (MilliSeconds (1800.0));

  Simulator::Run ();

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();
  std::cout << std::endl << "Flow monitor output:" << std::endl;
  std::cout << "Tx Packets:   " << stats[1].txPackets << std::endl;
  std::cout << "Tx Bytes:     " << stats[1].txBytes << std::endl;
  std::cout << "Offered Load: " << stats[1].txBytes * 8.0 / (stats[1].timeLastTxPacket.GetSeconds () - stats[1].timeFirstTxPacket.GetSeconds ()) / 1000000 << " Mbps" << std::endl;
  std::cout << "Rx Packets:   " << stats[1].rxPackets << std::endl;
  std::cout << "Rx Bytes:     " << stats[1].rxBytes<< std::endl;
  std::cout << "Throughput:   " << stats[1].rxBytes * 8.0 / (stats[1].timeLastRxPacket.GetSeconds () - stats[1].timeFirstRxPacket.GetSeconds ()) / 1000000 << " Mbps" << std::endl;
  std::cout << "Mean delay:   " << stats[1].delaySum.GetSeconds () / stats[1].rxPackets << std::endl;
  std::cout << "Mean jitter:  " << stats[1].jitterSum.GetSeconds () / (stats[1].rxPackets - 1) << std::endl;

  flowMonitor->SerializeToXmlFile(transport_prot + ".flowMonitor", true, true);

  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  Ptr<PacketSink> sinkptr = DynamicCast<PacketSink> (sinkApps.Get (0));
  std::cout << "Total Bytes Received on FTP Channel: " << sinkptr->GetTotalRx () << std::endl;
  for (uint16_t cbrIndex = 1; cbrIndex <= n_cbr; cbrIndex++)
    {
      sinkptr = DynamicCast<PacketSink> (cbrSinkApps[cbrIndex-1].Get (0));
      std::cout << "Total Bytes Received on CBR" << cbrIndex << " Channel: " << sinkptr->GetTotalRx () << std::endl;
    }
}
