#include<bits/stdc++.h>
using namespace std;
#define f first
#define s second
#define ll long long
#define mp make_pair
#define MAX 100
#define mod 1000000007
#define pb push_back
#define INF 1e18
#define pii pair<int,int>

// Declare parameters used to define the problem
int N = 8;
int B = 4;
double kFrac = 0.6;
int K = 5;
double p = 0.5;
string queue_type = "INQ";
int T = 10000;
string outputFile = "output";

// Declare global variables used
vector<queue<pii> > inputBuffer;
vector<queue<int> > outputBuffer;
int linkUsage;
double KDropProb = 0.0;
int KDropNum = 0;
vector<int> packetDelay;
vector<int> a;
vector<int> g;
bool verbose = 0;
int dropped = 0;
int transmitted = 0;

// Initialize all parameters and variables
void initialize(){
	inputBuffer = vector<queue<pii> >(N, queue<pii>());
	outputBuffer = vector<queue<int> >(N, queue<int>());
	a = vector<int>(N, 0);
	g = vector<int>(N, 0);
	linkUsage = 0;
	KDropProb = 0.0;
	KDropNum = 0;
	packetDelay.clear();
	dropped = 0;
	transmitted = 0;
}

// PHASE 1: Generate Traffic for a single time slot on every input port 
void generateTraffic(int t){
	for(int i=0;i<N;i++){
		bool generated = (rand() % 100000) < (100000 * p);	// This ensures packet generation with probability p
		if(generated){
			if(inputBuffer[i].size()<B)
				inputBuffer[i].push(mp(rand() % N, t));
			else
				dropped++;
		}
	}
	if(verbose)
		cout<<"Time "<<t<<": Packet Generation Completed\n";
}

// Send packet from iPort input port to oPort output port
void allocate(int iPort, int oPort){
	if(outputBuffer[oPort].size()<B)
		outputBuffer[oPort].push(inputBuffer[iPort].front().s);
	else
		dropped++;
	inputBuffer[iPort].pop();
	linkUsage++;
	if(verbose)
		cout<<iPort<<" - "<<oPort<<endl;
}

// PHASE 2: Scheduling Algorithm (INQ/KOUQ/iSLIP) for a single time slot
void schedule(){
	vector<vector<int> > v = vector<vector<int> >(N, vector<int>());
	for(int i=0;i<N;i++){
		if(!inputBuffer[i].empty())
			v[inputBuffer[i].front().f].pb(i);
	}
	int count = 0;
	for(int i=0;i<N;i++)
		if(v[i].size() > K)
			count++;
	if(verbose){
		cout<<"O/p Ports: I/P requests\n";
		for(int i=0;i<N;i++){
			cout<<i<<": ";
			for(int j=0;j<v[i].size();j++)
				cout<<v[i][j]<<" ";
			cout<<endl;
		}
		cout<<endl;
	}
	KDropProb += (double)count/(double)N;
	KDropNum++;
	if(queue_type == "INQ"){
		for(int i=0;i<N;i++){
			if(v[i].size()){
				int iport = rand() % v[i].size();
				allocate(v[i][iport], i);
			}
		}
	}
	if(queue_type == "KOUQ"){
		for(int i=0;i<N;i++){
			int counter = K;
			while((counter--) && (v[i].size())){
				int iport = rand() % v[i].size();
				allocate(v[i][iport], i);
				v[i].erase(v[i].begin()+iport);
			}
			for(int j=0;j<v[i].size();j++){
				dropped++;
				inputBuffer[v[i][j]].pop();
			}
		}
	}
	if(queue_type == "iSLIP"){
		vector<bool> isusedIPort = vector<bool>(N, 0);
		vector<bool> isusedOPort = vector<bool>(N, 0);
		vector<vector<int> > offered = vector<vector<int> >(N, vector<int>());			//SIZE
		while(1){
			int num = 0;
			if(verbose){
				cout<<"GRANT PHASE\n";
				cout<<"O/P Port    I/P Port\n";
			}
			for(int i=0;i<N;i++){
				if(!isusedOPort[i]){
					int iport = -1;
					for(int j=0;j<v[i].size();j++){
						if((!isusedIPort[v[i][j]]) && (v[i][j]>=g[i])){
							iport = v[i][j];
							break;
						}
					}
					if(iport == -1){
						for(int j=0;j<v[i].size();j++){
							if(!isusedIPort[v[i][j]]){
								iport = v[i][j];
								break;
							}
						}
					}
					if(iport >= 0){
						if(verbose)
							cout<<i<<"            "<<iport<<endl;
						offered[iport].pb(i);
					}
				}
			}
			if(verbose){
				cout<<"ACCEPT PHASE\n";
				cout<<"I/P Port    O/P Port\n";
			}
			for(int i=0;i<N;i++){
				if(!isusedIPort[i]){
					int oport = -1;
					for(int j=0;j<offered[i].size();j++){
						if((!isusedOPort[offered[i][j]]) && (offered[i][j]>=a[i])){
							oport = offered[i][j];
							break;
						}
					}
					if(oport == -1){
						for(int j=0;j<offered[i].size();j++){
							if(!isusedOPort[offered[i][j]]){
								oport = offered[i][j];
								break;
							}
						}
					}
					if(oport >= 0){
						allocate(i, oport);
						a[i] = (oport + 1)%N;
						g[oport] = (i + 1)%N;
						num++;
						isusedIPort[i] = 1;
						isusedOPort[oport] = 1;
					}
				}
			}
			if(!num)
				break;
		}
	}
}

//PHASE 3: Packet Transmission from output ports in a single time slot
void transmit(int t){
	for(int i=0;i<N;i++){
		if(!outputBuffer[i].empty()){
			packetDelay.pb(t - outputBuffer[i].front());
			outputBuffer[i].pop();
			transmitted++;
		}
	}
	if(verbose)
		cout<<"Time "<<t<<": Transmission Completed\n";
}

// Print Results and store in file
void printResult(){
	double avgPacketDelay = 0.0;
	double avgIOLinkUtilisation = 0.0;
	double avgOLinkUtilisation = 0.0;
	double avgKDropProb = 0.0;
	double avgDropped = 0.0;
	if(packetDelay.size())
		avgPacketDelay = (double)accumulate(packetDelay.begin(), packetDelay.end(), 0)/(double)packetDelay.size();
	if((T)&&(N)){
		avgIOLinkUtilisation = (double)linkUsage/(double)T;
		avgIOLinkUtilisation = avgIOLinkUtilisation/((double)N*(double)N);
	}
	if(N){
		avgOLinkUtilisation = (double)transmitted/(double)T;
		avgOLinkUtilisation = avgOLinkUtilisation/(double)N;
		avgDropped = (double)dropped/(double)T;
		avgDropped = avgDropped/(2.0*(double)N);
	}
	if(KDropNum)
		avgKDropProb = (double)KDropProb/(double)KDropNum;
	if(verbose){
		cout<<"Average packet delay: "<<avgPacketDelay<<endl;
		cout<<"Average input-output link utilisation: "<<avgIOLinkUtilisation<<endl;
		cout<<"Average KOUQ drop probability: "<<avgKDropProb<<endl;
	}

    double stdPacketDelay = 0.0;
    if(packetDelay.size()){
	    for (int i = 0; i < packetDelay.size(); i++)  
	        stdPacketDelay += (double)(((double)packetDelay[i] - avgPacketDelay) * ((double)packetDelay[i] - avgPacketDelay)); 
	    stdPacketDelay = sqrt(stdPacketDelay/(double)packetDelay.size());
	}

    fstream file;
    file.open(outputFile+".tsv", ios::out | ios::app);
    file<<N<<"\t"<<p<<"\t"<<queue_type<<"\t"<<avgPacketDelay<<"\t"<<stdPacketDelay;
    file<<"\t"<<avgIOLinkUtilisation<<"\t"<<avgOLinkUtilisation<<"\t"<<avgDropped<<"\t"<<avgKDropProb<<"\n";
    file.close();
}

// Run Single Test
// The global parameters should be already set accordingly
void runTest(){
	initialize();
	for(int i=0;i<T;i++){
		if(i%3 == 0)
			generateTraffic(i);
		else if(i%3 == 1)
			schedule();
		else
			transmit(i);
	}
	printResult();
}

// Run all test cases from N = 4, 5, ..., 100
void runAll(){
	outputFile = "K"+to_string(kFrac)+"_p"+to_string(p)+"_B"+to_string(B)+"_"+queue_type;
	fstream file;
	file.open(outputFile+".tsv", ios::out);
	file<<"N\tp\tqueue\tavgPacketDelay\tstdPacketDelay\tavgIOLinkUtilisation\tavgOLinkUtilisation\tavgDropped\tKOUQDropProbability\n";
	file.close();
	for(N=4;N<=MAX;N++){
		cout<<round((double)N*100.0/(double)MAX)<<"%\r";
		runTest();
	}
}

int main(int argc, char** argv)
{
	for(int i=1;i<argc;i++){				// Read commandline arguments and assign to global parameters
		if(strcmp(argv[i], "-N") == 0)
			N = stoi(argv[i+1]);
		if(strcmp(argv[i], "-B") == 0)
			B = stoi(argv[i+1]);
		if(strcmp(argv[i], "-p") == 0)
			p = stod(argv[i+1]);
		if(strcmp(argv[i], "-queue") == 0)
			queue_type = argv[i+1];
		if(strcmp(argv[i], "-K") == 0)
			kFrac = stod(argv[i+1]);
		if(strcmp(argv[i], "-out") == 0)
			outputFile = argv[i+1];
		if(strcmp(argv[i], "-T") == 0)
			T = stoi(argv[i+1]);
	}
	K = round(kFrac*(double)N);

	//verbose = 1;					//UNCOMMENT THIS LINE TO DISPLAY STEP BY STEP DETAILS ON COMMANDLINE

	//srand ( time(NULL) );			//UNCOMMENT THIS LINE TO GENERATE A DIFFERENT RANDOM NUMBER EVERYTIME DURING TRAFFIC GENERATION

	runTest();

	//runAll();			//COMMENT THE ABOVE LINE (runTest();) AND UNCOMMENT THIS LINE TO RUN TESTS FOR N = 4, 5, ..., 100

    return 0;
}