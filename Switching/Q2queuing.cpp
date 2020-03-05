#include<bits/stdc++.h>
using namespace std;
#define f first
#define s second
#define ll long long
#define mp make_pair
#define MAX 1000006
#define mod 1000000007
#define pb push_back
#define INF 1e18
#define pii pair<int,int>
using namespace std::chrono; 

queue<high_resolution_clock::time_point> inputBuffer[3];
vector<double> results;
vector<double> waitingTime;
high_resolution_clock::time_point startTime;
int numPassengers;

mutex mtx, mtx_2;             // mutex for critical section
condition_variable cv, cv_2; // condition variable for critical section
int sem_val=1, sem_val_2=1;

int findWaiting(){
	int sum=0;
	for(int i=0;i<3;i++){
		sum+=inputBuffer[i].size();
	}
	return sum;
}

double average(vector<double> v){
	double sum=0.0;
	for(int i=0;i<v.size();i++)
		sum+=v[i];
	return sum/(double)v.size();
}

void arrival(double lambda, int idx){
	unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);
	poisson_distribution<int> poisson(lambda);
	uniform_int_distribution<int> uniform(0,2);
	exponential_distribution<double> exponential(lambda);

	while(1){
		double num = (double)exponential(generator);
		duration<double> period(num);
		this_thread::sleep_for(period);

		high_resolution_clock::time_point start = high_resolution_clock::now();

		int queue_index = uniform(generator);
		if(idx == 1)
			queue_index = 0;
		if((idx == 2) && (inputBuffer[queue_index].size() == 10))
			continue;
		//cout<<"Packet arrived at "<<queue_index<<endl;
		inputBuffer[queue_index].push(start);
	}
}

void service(double mu, int index, int idx){
	if(idx == 1)
		index = 0;
	unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);
	exponential_distribution<double> exponential(mu);
	 
	while(1){
		unique_lock<mutex> lck(mtx);
		unique_lock<mutex> lck_2(mtx_2);
		if(!inputBuffer[index].empty()){
			//cout<<"Packet picked for service at "<<index<<endl;
			if(idx == 1){
				if(--sem_val_2 < 0)
					cv_2.wait(lck_2);
			}
			high_resolution_clock::time_point start = inputBuffer[index].front();
			inputBuffer[index].pop();
			if(idx == 1){
				if(++sem_val_2 <= 0)
					cv_2.notify_one();
			}
			high_resolution_clock::time_point stop = high_resolution_clock::now();
			auto waitTime = duration_cast<milliseconds>(stop - start);

			double num = exponential(generator);
			chrono::duration<double> period (num);
			this_thread::sleep_for(period);

			if(--sem_val < 0)
				cv.wait(lck);
			waitingTime.pb(waitTime.count());
			stop = high_resolution_clock::now();
			auto duration1 = duration_cast<milliseconds>(stop - start);
			results.pb(duration1.count());
			numPassengers++;
			auto duration2 = duration_cast<seconds>(stop - startTime);
			cout << "\033[2J\033[1;1H";
			cout<<"Average service rate (pps)               = "<<(double)numPassengers/(double)(duration2.count())<<endl;
			cout<<"Average response time (ms)               = "<<average(results)<<endl;
			cout<<"Average waiting time (ms)                = "<<average(waitingTime)<<endl;
			cout<<"Number of passengers in queue            = "<<findWaiting()<<endl;
			if(++sem_val <= 0)
				cv.notify_one();
		}
	}
}

int main()
{
    //freopen ("input1.in","r",stdin);
    //freopen ("output22.txt","w",stdout);
    //ios_base::sync_with_stdio(false);
    //cin.tie(NULL);

    double lambda;
    double mu;
    int numServers = 3;
    thread th[4];
    string s;
    int idx;

    cout<<"Enter Lambda (pps): ";
    cin>>lambda;
    if(lambda <= 0.0){
    	cout<<"INVALID Lambda\n";
    	exit(0);
    }
    cout<<"Enter Mu (pps): ";
    cin>>mu;
    if((mu <= 0.0)||(lambda >= ((double)(numServers)*mu))){
    	cout<<"INVALID Mu\n";
    	exit(0);
    }
    cout<<"Enter Case (A/B/C): ";
    cin>>s;
    if((s=="A")||(s=="a"))
    	idx = 0;
    else if((s=="B")||(s=="b"))
    	idx = 1;
    else if((s=="C")||(s=="c"))
    	idx = 2;
    else{
    	cout<<"INVALID Case\n";
    	exit(0);
    }

    startTime = high_resolution_clock::now();
    th[0] = thread(arrival, lambda, idx);
    for(int i=0;i<numServers;i++)
		th[i+1] = thread(service, mu, i, idx);
	
	for(int i=0;i<numServers+1;i++)
		th[i].join();

    return 0;
}