## Instructions
- Use ns3-dev version
- Place `script.cc` in `ns-3-dev/scratch/` directory.
- Place `run_script.sh` and `plot.py` in `ns-3-dev/` directory.
- Enter `ns-3-dev/` directory and run `$bash run_script.sh`.
- Enter `ns-3-dev/` directory and run `$python3 plot.py` to view plots.
- Plots and data files are generated in `ns-3-dev/Output/` directory.

## Problem Statement
**Application #3** in **CS 349_Assignment 4_March-April 2019.pdf**:

Create a topology of two nodes N0 and N1 connected by a link of bandwidth 1 Mbps and link delay 10 ms. Use a
drop-tail queue at the link. Set the queue size according to bandwidth-delay product. Create a TCP agent (type
of the agent specified below) and FTP traffic at N0 destined for N1. Create 5 CBR traffic agents of rate 300 Kbps
each at N0 destined for N1. Make appropriate assumptions wherever necessary. The timing of the flows are as
follows:
- FTP starts at 0 sec and continues till the end of simulation.
- CBR1 starts at 200 ms and continues till end.
- CBR2 starts at 400 ms and continues till end.
- CBR3 starts at 600 ms and stops at 1200 ms.
- CBR4 starts at 800 ms and stops at 1400 ms.
- CBR5 starts at 1000 ms and stops at 1600 ms.
- Simulation runs for 1800 ms.
```
1. Plot graph(s) of TCP congestion window w.r.t. time for following 5 TCP congestion control algorithm
    implementations, and describe the TCP congestion control algorithms’ behaviour.
- Case 1: use TCP New Reno
- Case 2: use TCP Hybla
- Case 3: use TCP Westwood
- Case 4: use TCP Scalable
- Case 5: use TCP Vegas
````
```
2. Draw a graph showing cumulative TCP packets dropped w.r.t. time comparing above 5 TCP congestion
    control algorithm implementations.
```
```
3. Draw a graph showing cumulative bytes transferred w.r.t. time comparing above 5 TCP congestion
    control algorithm implementations.
```

## Observations
Cumulative Bytes Transferred vs Time:
![Cumulative Bytes Transferred](https://github.com/akulagrawal/Computer-Networks/blob/master/Assignment4%20-%20ns-3%20Network%20Simulation/Plots/Bytes%20Transferred.png)
Congestion Window Size vs Time:
![Congestion Window Size](https://github.com/akulagrawal/Computer-Networks/blob/master/Assignment4%20-%20ns-3%20Network%20Simulation/Plots/CWND%20Comparision.png)
Cumulative Packets dropped vs Time:
![Cumulative Packets dropped](https://github.com/akulagrawal/Computer-Networks/blob/master/Assignment4%20-%20ns-3%20Network%20Simulation/Plots/Packets%20dropped.png)
