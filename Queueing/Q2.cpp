#include <iostream>
#include <random>
#include <cmath>
#include<time.h>
#define inf 9999
using namespace std;

double exponential(double time) //exponential distribution function
{
 double temp=0;
 while ((temp==0) || (temp==1))
 {
 temp = ((double) rand()/RAND_MAX); //random values between  0 and 1
 }
 return((-time)*log(temp));// formula for exponential Random Variable
}


void caseA(double a_rate,double d_rate,int total_time){   //case A , 3 queue ,3 server

double curr_time=0,arrival_time=0,departure_time[3]={inf,inf,inf},notserved[3]={0,0,0},served=0;
//curr_time for current time
//departure_time for departure of passenger
//arrival time =arrival of passenger
//notserved = notserved passenger in individual queue of server
//served are total number of served passenger till now
//customer_system_time =total time spent by customer in system
//wt= waiting time of all customer in system
//last_event_time = time of last event happened (arrival or departure)

double customer_system_time=0,last_event_time=0,wt=0;
int ser_num;
do{ //run until curr_time is less than total simulation time
if(arrival_time < departure_time[0] && arrival_time< departure_time[1] && arrival_time< departure_time[2]) //arrival event
    {
        //cout<<"\nfirst";
    curr_time=arrival_time;
    wt+=notserved[0]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[0]); //update waiting time and system time
    wt+=notserved[1]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[1]);
    wt+=notserved[2]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[2]);
    arrival_time+=exponential(1/a_rate);   //next arrival time
    ser_num=rand()%3; // randomly passenger join the one of the three queue
    notserved[ser_num]+=1;   //increament current number of passenger
    if(notserved[ser_num]==1){
        departure_time[ser_num]=curr_time+exponential(1/(d_rate));  //reduce service time from infinity to a finit time, once passenger join empty queue
                                                                    // if not clear (check the below "else" part
    }
}
else if( departure_time[0]< departure_time[1] && departure_time[0] < departure_time[2])
{
    //departure from first server
    curr_time=departure_time[0];
    //wt+=notserved[0]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[0]); //update waiting time and system time
    wt+=notserved[1]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[1]);
    wt+=notserved[2]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[2]);
    served+=1;
    notserved[0]-=1;  //updating served and notserved passenger number
    wt+=notserved[0]*(curr_time-last_event_time);
    if(notserved[0]>0){
        departure_time[0]+= exponential(1/(d_rate));
    }
    else{
        departure_time[0]=inf;  //if no passenger in inspector queue , then wait till infinity
        // once passenger come time reduce from infinity.
    }

}
else if(departure_time[1] < departure_time[2])
{
    //departure from second server
    curr_time=departure_time[1];
    wt+=notserved[0]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[0]);
    //wt+=notserved[1]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[1]);
    wt+=notserved[2]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[2]);
    served+=1;
    notserved[1]-=1;
    wt+=notserved[1]*(curr_time-last_event_time);
    if(notserved[1]>0){
        departure_time[1]+= exponential(1/(d_rate));
    }
    else{
        departure_time[1]=inf;
    }

}
else
{
   // departure from third server
    curr_time=departure_time[2];
    wt+=notserved[0]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[0]);
    wt+=notserved[1]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[1]);
   // wt+=notserved[2]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[2]);
    served+=1;
    notserved[2]-=1; //updating different values accordingly
    wt+=notserved[2]*(curr_time-last_event_time);
    if(notserved[2]>0){
        departure_time[2]+= exponential(1/(d_rate));
    }
    else{
        departure_time[2]=inf;
    }

}
last_event_time=curr_time; //last_event_time update

}while(curr_time<total_time);
//cout<<"\n"<<customer_system_time;
//cout<<"\n"<<wt;
cout<<"\nAverage number of passengers getting inspected:"<<served/total_time;//total here total number of  people get serviced in 100 unit of time
cout<<"\nAverage response time(total) for passengers in getting inspected:"<<customer_system_time/served;
cout<<"\nAverage waiting time for passengers in getting inspected:"<<wt/served;
cout<<"\nAverage number of passengers waiting in queue before each officer during Inspection:"<<wt/total_time;
cout<<"\nAverage number of passengers in the system:"<<customer_system_time/total_time;

}





void caseB(double a_rate,double d_rate,int total_time){
double curr_time=0,arrival_time=0,departure_time[3]={inf,inf,inf},notserved=0,served=0;
double customer_system_time=0,last_event_time=0,wt=0;
//curr_time for current time
//departure_time for departure of passenger
//arrival time =arrival of passenger
//notserved = notserved passenger in individual queue of server
//served are total number of served passenger till now
//customer_system_time =total time spent by customer in system
//wt= waiting time of all customer in system
//last_event_time = time of last event happened (arrival or departure)

do{
if(arrival_time < departure_time[0] && arrival_time< departure_time[1] && arrival_time< departure_time[2])
    {
        //cout<<"\nfirst";
    curr_time=arrival_time;
    wt+=notserved*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved);
    arrival_time+=exponential(1/a_rate);
    notserved+=1;
    if(notserved==1){
        //ser_num=rand()%3;
        departure_time[0]=curr_time+exponential(1/(d_rate));
        departure_time[1]=curr_time+exponential(1/(d_rate));
        departure_time[2]=curr_time+exponential(1/(d_rate));
    }
}
else if( departure_time[0]< departure_time[1] && departure_time[0] < departure_time[2])
{
    //cout<<"\nsecond";
    curr_time=departure_time[0];
    customer_system_time+=((departure_time[0]-last_event_time)*notserved);
    served+=1;
    notserved-=1;
    wt+=notserved*(curr_time-last_event_time);
    if(notserved>0){
        departure_time[0]+= exponential(1/(d_rate));
    }
    else{
        departure_time[0]=inf;
        departure_time[1]=inf;
        departure_time[2]=inf; //if no passenger in the queue than all inspector wait till infinity
                                //once passenger arrive ,inspector once again start serving
    }

}
else if(departure_time[1] < departure_time[2])
{
   // cout<<"\nthird";
    curr_time=departure_time[1];
    customer_system_time+=((departure_time[1]-last_event_time)*notserved);
    served+=1;
    notserved-=1;
    wt+=notserved*(curr_time-last_event_time);
    if(notserved>0){
        departure_time[1]+= exponential(1/(d_rate));
    }
    else{
        departure_time[0]=inf;
        departure_time[1]=inf;
        departure_time[2]=inf;
    }

}
else
{
   // cout<<"\nfinal";
    curr_time=departure_time[2];
    customer_system_time+=((departure_time[2]-last_event_time)*notserved);
    served+=1;
    notserved-=1;
    wt+=notserved*(curr_time-last_event_time);
    if(notserved>0){
        departure_time[2]+= exponential(1/(d_rate));
    }
    else{
        departure_time[0]=inf;
        departure_time[1]=inf;
        departure_time[2]=inf;
    }

}
last_event_time=curr_time;

}while(curr_time<total_time);
cout<<"\nAverage number of passengers getting inspected:"<<served/total_time;//total here total number of  people get serviced in 100 unit of time
cout<<"\nAverage response time(total) for passengers in getting inspected:"<<customer_system_time/served;
cout<<"\nAverage waiting time for passengers in getting inspected:"<<wt/served;
cout<<"\nAverage number of passengers waiting in queue before each officer during Inspection:"<<wt/total_time;
cout<<"\nAverage number of passengers in the system:"<<customer_system_time/total_time;


}






void caseC(double a_rate,double d_rate,int total_time,int chair){ //case C

double curr_time=0,arrival_time=0,departure_time[3]={inf,inf,inf},notserved[3]={0,0,0},served=0;
double customer_system_time=0,last_event_time=0,wt=0;
int ser_num;
//curr_time for current time
//departure_time for departure of passenger
//arrival time =arrival of passenger
//notserved = notserved passenger in individual queue of server
//served are total number of served passenger till now
//customer_system_time =total time spent by customer in system
//wt= waiting time of all customer in system
//last_event_time = time of last event happened (arrival or departure)

do{
if(arrival_time < departure_time[0] && arrival_time< departure_time[1] && arrival_time< departure_time[2])
    {
    curr_time=arrival_time;
    wt+=notserved[0]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[0]);
    wt+=notserved[1]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[1]);
    wt+=notserved[2]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[2]);
    arrival_time+=exponential(1/a_rate);
    ser_num=rand()%3;
    if(notserved[ser_num]<chair){ notserved[ser_num]+=1;}
    if(notserved[ser_num]==1){
        departure_time[ser_num]=curr_time+exponential(1/(d_rate));
    }
}
else if( departure_time[0]< departure_time[1] && departure_time[0] < departure_time[2])
{
    curr_time=departure_time[0];
    customer_system_time+=((arrival_time-last_event_time)*notserved[0]);
    wt+=notserved[1]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[1]);
    wt+=notserved[2]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[2]);
    served+=1;
    notserved[0]-=1;
    wt+=notserved[0]*(curr_time-last_event_time);
    if(notserved[0]>0){
        departure_time[0]+= exponential(1/(d_rate));
    }
    else{
        departure_time[0]=inf;
    }

}
else if(departure_time[1] < departure_time[2])
{
    curr_time=departure_time[1];
    wt+=notserved[0]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[0]);
    customer_system_time+=((arrival_time-last_event_time)*notserved[1]);
    wt+=notserved[2]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[2]);
    served+=1;
    notserved[1]-=1;
    wt+=notserved[1]*(curr_time-last_event_time);
    if(notserved[1]>0){
        departure_time[1]+= exponential(1/(d_rate));
    }
    else{
        departure_time[1]=inf;
    }

}
else
{
    curr_time=departure_time[2];
    wt+=notserved[0]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[0]);
    wt+=notserved[1]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[1]);
   // wt+=notserved[2]*(curr_time-last_event_time);
    customer_system_time+=((arrival_time-last_event_time)*notserved[2]);
    served+=1;
    notserved[2]-=1;
    wt+=notserved[2]*(curr_time-last_event_time);
    if(notserved[2]>0){
        departure_time[2]+= exponential(1/(d_rate));
    }
    else{
        departure_time[2]=inf;
    }

}
last_event_time=curr_time;  //last event var to curr time

}while(curr_time<total_time);
cout<<"\nAverage number of passengers getting inspected:"<<served/total_time;//total here total number of  people get serviced in 100 unit of time
cout<<"\nAverage response time(total) for passengers in getting inspected:"<<customer_system_time/served;
cout<<"\nAverage waiting time for passengers in getting inspected:"<<wt/served;
cout<<"\nAverage number of passengers waiting in queue before each officer during Inspection:"<<wt/total_time;
cout<<"\nAverage number of passengers in the system:"<<customer_system_time/total_time;

}


int main()
{
 int choice=1,chairs=10;
 double lambda,mu,it,ch;  //arrival rate and service rate
 cout<<"\nOUTPUT will be in same units as Input.";
 cout<<"\nMore the Time for Simulation, better the Result.";

 //it=24;
 while(choice!=0){
        cout<<"\nEnter choice\n0.Exit\n1. Case A \n2.Case B\n3.Case C \n4.Comparision (All cases) ";
        cin>>ch;

        if(!(ch>0 && ch<5)){
    cout<<"\n Invalid Input";
    continue;
}
choice=int(ch);
cout<<"\n Enter time for Simulation( prefer About 100 unit):";
 cin>>it;
cout<<"\nEnter mean arrival rate Lambda :";
cin>>lambda;
cout<<"\nEnter mean service rate of Mu:";
cin>>mu;
if(3*mu< lambda || mu<0 || lambda< 0 || it<0 ){
    choice=5;
}


        switch (choice)
        {
        case 1:
            caseA(lambda,mu,it);
            break;
        case 2:
            caseB(lambda,mu,it);
                break;
        case 3:
            caseC(lambda,mu,it,chairs);
                break;
        case 4:
            cout<<"\n\n*****    CASE A ****";
            caseA(lambda,mu,it);
            cout<<"\n\n*****    CASE B ****";
            caseB(lambda,mu,it);
            cout<<"\n\n*****    CASE C ****";
            caseC(lambda,mu,it,chairs);
                break;
        case 0:
                choice=0;
                break;
        case 5:
                cout<<"\n Invalid value of lambda or mu or time";
                break;
        default:
            cout<<"\n Invalid choice";
            break;
        }
 }
return 0;
 }


