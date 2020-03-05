#include<bits/stdc++.h>
using namespace std;
#define f first
#define s second
#define ll long long
#define mp make_pair
#define MAX 1000006
#define mod 1000000007
#define pb push_back
#define INF 1e9
#define pii pair<int,int>

struct connection{
    int source;
    int dest;
    double bmin;
    double bmax;
    double bave;
    double bequiv;
};

struct topology{
    int node1;
    int node2;
    int propDelay;
    double capacity;
    int bequivSum;
    int bmaxSum;
    int dist;
};

int V;
vector<connection> conList;
vector<topology> topList;
map<pii, int> edgeToTop;

void readConnectionData(string filename){
    fstream file;
    string word;
    vector<int> vfile;
    file.open(filename.c_str());

    while (file >> word)
        vfile.pb(atoi(word.c_str()));
    int i=1;
    while(i<vfile.size()){
        connection temp;
        temp.source = vfile[i];
        temp.dest = vfile[i+1];
        temp.bmin = vfile[i+2];
        temp.bmax = vfile[i+3];
        temp.bave = vfile[i+4];
        temp.bequiv = min(temp.bmax, temp.bave+0.25*(temp.bmax - temp.bmin));
        conList.pb(temp);
        i=i+5;
    }
}

void readTopologyData(string filename){
    fstream file;
    string word;
    vector<int> vfile;
    file.open(filename.c_str());

    while (file >> word)
        vfile.pb(atoi(word.c_str()));
    int i=2;
    V = vfile[0];
    while(i<vfile.size()){
        topology temp;
        temp.node1 = vfile[i];
        temp.node2 = vfile[i+1];
        temp.propDelay = vfile[i+2];
        temp.dist = temp.propDelay;
        temp.capacity = vfile[i+3];
        temp.bequivSum = 0;
        temp.bmaxSum = 0;
        topList.pb(temp);
        edgeToTop[mp(temp.node1, temp.node2)] = topList.size()-1;
        edgeToTop[mp(temp.node2, temp.node1)] = topList.size()-1;
        i=i+5;
    }
}

int distance(topology top, bool flag){
    if(flag)
        return top.dist;
    return 1;
}

vector<int> findRoute1(vector<vector<int> > dist, int i, int j, vector<vector<int> > graph, bool flag, map<int, bool> &my){
    vector<int> v;
    int k=i;
    while(k!=j){
        int n=graph[k].size();
        for(int l=0;l<n;l++){
            int x = topList[graph[k][l]].node1;
            if(x == k)
                x = topList[graph[k][l]].node2;
            if(dist[k][j]==distance(topList[graph[k][l]], flag)+dist[x][j]){
                my[graph[k][l]] = 1;
                v.pb(graph[k][l]);
                k=x;
                break;
            }
        }
    }
    return v;
}

void printDist(vector<vector<int> > dist){
    for(int i=0;i<V;i++){
        for(int j=0;j<V;j++)
            cout<<setw(4)<<dist[i][j];
        cout<<endl;
    }
}

vector<vector<vector<vector<int> > > > findShortestPaths(bool flag){

    vector<vector<int> > dist(V, vector<int>(V, INF));
    vector<vector<int> > graph(V, vector<int>());
    vector<vector<vector<vector<int> > > > shortest(3, vector<vector<vector<int> > >(V, vector<vector<int> >(V, vector<int>())));
    for(int i=0;i<topList.size();i++){
        dist[topList[i].node1][topList[i].node2] = min(distance(topList[i], flag), dist[topList[i].node1][topList[i].node2]);
        dist[topList[i].node2][topList[i].node1] = min(distance(topList[i], flag), dist[topList[i].node2][topList[i].node1]);
        graph[topList[i].node1].pb(i);
        graph[topList[i].node2].pb(i);
    }

    for(int i=0;i<V;i++)
        dist[i][i] = 0;

    for (int i=0;i<V;i++){
        for (int j=0;j<V;j++){
            for (int k=0;k<V;k++){
                if (dist[j][i] + dist[i][k] < dist[j][k])
                    dist[j][k] = dist[j][i] + dist[i][k];
            }
        }
    }

    //printDist(dist);

    for(int i=0;i<V;i++){
        for(int j=0;j<V;j++){
            if(i==j)
                continue;
            map<int, bool> my;
            shortest[0][i][j] = findRoute1(dist, i, j, graph, flag, my);
            int minm=INF;
            int v1=0, v2=0;
            bool isSecond=0;
            int idx=0;
            for(int l=0;l<topList.size();l++){
                if((!my[l])&&(topList[l].node1!=topList[l].node2)){
                    if(minm > (distance(topList[l], flag)+dist[i][topList[l].node1]+dist[topList[l].node2][j])){
                        isSecond = 1;
                        minm = distance(topList[l], flag)+dist[i][topList[l].node1]+dist[topList[l].node2][j];
                        v1 = topList[l].node1;
                        v2 = topList[l].node2;
                        idx = l;
                    }
                    if(minm > (distance(topList[l], flag)+dist[i][topList[l].node2]+dist[topList[l].node1][j])){
                        isSecond = 1;
                        minm = distance(topList[l], flag)+dist[i][topList[l].node2]+dist[topList[l].node1][j];
                        v1 = topList[l].node2;
                        v2 = topList[l].node1;
                        idx = l;
                    }
                }
            }
            if(isSecond){
                vector<int> vec1 = findRoute1(dist, i, v1, graph, flag, my);
                vector<int> vec2 = findRoute1(dist, v2, j, graph, flag, my);
                vec1.pb(idx);
                vec1.insert(vec1.end(), vec2.begin(), vec2.end());
                shortest[1][i][j] = vec1;
            }
        }
    }
    return shortest;
}

vector<int> findRoute2(vector<int> cost, int i, int j, vector<vector<int> > graph, bool flag){
    vector<int> v;
    int k=j;
    while(k!=i){
        int n=graph[k].size();
        for(int l=0;l<n;l++){
            int x = topList[graph[k][l]].node1;
            if(x == k)
                x = topList[graph[k][l]].node2;
            if(cost[k]==cost[x]+distance(topList[graph[k][l]], flag)){
                v.pb(graph[k][l]);
                k=x;
                break;
            }
        }
    }
    return v;
}

vector<vector<vector<int> > > findLinkDisjointShortestPaths(bool flag, vector<vector<vector<int> > > sPath){

    vector<vector<vector<int> > > shortest(V, vector<vector<int> >(V, vector<int>()));

    vector<vector<int> > oldGraph(V, vector<int>());
    for(int i=0;i<topList.size();i++){
        oldGraph[topList[i].node1].pb(i);
        oldGraph[topList[i].node2].pb(i);
    }

    vector<vector<int> > graph(V, vector<int>());
    for(int i=0;i<V;i++){
        for(int j=0;j<V;j++){
            if(!sPath[i][j].size())
                continue;
            vector<int> cost(V, INF);
            graph = oldGraph;
            vector<int> route = sPath[i][j];
            for(int k=0;k<route.size();k++){
                topology temp = topList[route[k]];
                graph[temp.node1].erase(remove(graph[temp.node1].begin(), graph[temp.node1].end(), route[k]), graph[temp.node1].end());
                graph[temp.node2].erase(remove(graph[temp.node2].begin(), graph[temp.node2].end(), route[k]), graph[temp.node2].end());
            }

            priority_queue< pii, vector< pii >, greater< pii > > pq;
            cost[i] = 0;
            pq.push(mp(0, i));
            while(!pq.empty())
            {
                int current = pq.top().s;
                int w_cur = pq.top().f;
                pq.pop();

                if(cost[current] < w_cur) continue;

                for(int j=0;j<graph[current].size();j++)
                {
                    int x = topList[graph[current][j]].node1;
                    if(current == x)
                        x = topList[graph[current][j]].node2;
                    if(cost[x] > cost[current] + distance(topList[graph[current][j]], flag)){
                        cost[x] = cost[current] + distance(topList[graph[current][j]], flag);
                        pq.push(mp(cost[x], x));
                    }
                }
            }
            shortest[j][i] = findRoute2(cost, i, j, graph, flag);
        }
    }

    return shortest;
}

void printPath(vector<vector<vector<vector<int> > > > shortest){
    for(int i=0;i<V;i++){
        for(int j=0;j<V;j++){
            cout<<"Path 1 from "<<i<<" to "<<j<<":\n";
            int l=i;
            cout<<i<<"->";
            for(int k=0;k<shortest[0][i][j].size();k++){
                int x = topList[shortest[0][i][j][k]].node1;
                if(x == l)
                    x = topList[shortest[0][i][j][k]].node2;
                l = x;
                cout<<x<<"->";
            }
            cout<<endl;
            cout<<"Path 2 from "<<i<<" to "<<j<<":\n";
            l=i;
            cout<<i<<"->";
            for(int k=0;k<shortest[1][i][j].size();k++){
                int x = topList[shortest[1][i][j][k]].node1;
                if(x == l)
                    x = topList[shortest[1][i][j][k]].node2;
                l = x;
                cout<<x<<"->";
            }
            cout<<endl;
            cout<<"Path 3 from "<<i<<" to "<<j<<":\n";
            l=i;
            cout<<i<<"->";
            for(int k=0;k<shortest[2][i][j].size();k++){
                int x = topList[shortest[2][i][j][k]].node1;
                if(x == l)
                    x = topList[shortest[2][i][j][k]].node2;
                l = x;
                cout<<x<<"->";
            }
            cout<<endl;
        }
    }
}

bool checkRoute(vector<int> route, connection con, bool p){
    if(p==0){
        for(int i=0;i<route.size();i++){
            if(con.bequiv > topList[route[i]].capacity - topList[route[i]].bequivSum)
                return 0;
        }
        for(int i=0;i<route.size();i++)
            topList[route[i]].bequivSum += con.bequiv;
        return 1;
    }
    for(int i=0;i<route.size();i++){
        if(con.bmax > topList[route[i]].capacity - topList[route[i]].bmaxSum)
            return 0;
    }
    for(int i=0;i<route.size();i++)
        topList[route[i]].bmaxSum += con.bmax;
    return 1;
}

vector<int> setupOptimistic(vector<vector<vector<vector<int> > > > shortest){
    vector<int> v;
    for(int i=0;i<conList.size();i++){
        vector<int> route1 = shortest[0][conList[i].source][conList[i].dest];
        vector<int> route2 = shortest[2][conList[i].source][conList[i].dest];

        if(checkRoute(route1, conList[i], 0))
            v.pb(0);
        else if(checkRoute(route2, conList[i], 0))
            v.pb(2);
        else
            v.pb(-1);
    }
    return v;
}

vector<int> setupPessimistic(vector<vector<vector<vector<int> > > > shortest){
    vector<int> v;
    for(int i=0;i<conList.size();i++){
        vector<int> route1 = shortest[0][conList[i].source][conList[i].dest];
        vector<int> route2 = shortest[1][conList[i].source][conList[i].dest];

        if(checkRoute(route1, conList[i], 1))
            v.pb(0);
        else if(checkRoute(route2, conList[i], 1))
            v.pb(1);
        else
            v.pb(-1);
    }
    return v;
}

vector<int> setupConnections(bool p, vector<vector<vector<vector<int> > > > shortest){
    if(p==0)
        return setupOptimistic(shortest);
    return setupPessimistic(shortest);
}

void writeRoutingTable(string filename, vector<vector<vector<vector<int> > > > shortest){
    fstream f;
    for(int i=0;i<V;i++){
        string name = filename+"_"+to_string(i)+".csv";
        f.open(name.c_str(), ios::out);
        f.close();
    }
    map<pair<int,pair<pii,pii> >, bool> isEntry;
    for(int i=0;i<V;i++){
        for(int j=0;j<V;j++){
            vector<int> route = shortest[0][i][j];
            int l=i;
            for(int k=0;k<route.size();k++){
                int x = topList[route[k]].node1;
                if(x == l)
                    x = topList[route[k]].node2;
                if(!isEntry[mp(l,mp(mp(x,route[k]),mp(topList[route[k]].dist,topList[route[k]].capacity)))]){
                    string name = filename+"_"+to_string(l)+".csv";
                    f.open(name.c_str(), ios::out | ios::app);
                    f<<x<<","<<route[k]<<","<<topList[route[k]].dist<<","<<topList[route[k]].capacity<<"\n";
                    f.close();
                    isEntry[mp(l,mp(mp(x,route[k]),mp(topList[route[k]].dist,topList[route[k]].capacity)))] = 1;
                }
                l = x;
            }
        }
    }
}

void writeForwardingTable(string filename, vector<int> v, vector<vector<vector<vector<int> > > > shortest){
    fstream f;
    for(int i=0;i<V;i++){
        string name = filename+"_"+to_string(i)+".csv";
        f.open(name.c_str(), ios::out);
        f.close();
    }
    int port = 7000;
    for(int i=0;i<v.size();i++){
        if(v[i] == -1)
            continue;
        vector<int> route = shortest[v[i]][conList[i].source][conList[i].dest];
        int k=conList[i].source;
        for(int j=0;j<route.size();j++){
            int x = topList[route[j]].node1;
            if(x == k)
                x = topList[route[j]].node2;
            string name = filename+"_"+to_string(k)+".csv";
            f.open(name.c_str(), ios::out | ios::app);
                f<<k<<","<<port<<","<<i<<","<<port+1<<","<<i<<"\n";
            f.close();
            k = x;
        }
    }
}

void writePathsFile(string filename, vector<int> v, vector<vector<vector<vector<int> > > > shortest){
    int total = v.size();
    int admitted = 0;
    fstream f;
    f.open(filename+".txt", ios::out);
    f.close();
    f.open(filename+".csv", ios::out);
    f.close();
    f.open(filename+".csv", ios::out | ios::app);
    for(int i=0;i<v.size();i++){
        if(v[i] == -1)
            continue;
        admitted++;
        f<<i<<","<<conList[i].source<<","<<conList[i].dest<<",";

        vector<int> route = shortest[v[i]][conList[i].source][conList[i].dest];
        int k=conList[i].source;    
        for(int j=0;j<route.size();j++){
            int x = topList[route[j]].node1;
            if(x == k)
                x = topList[route[j]].node2;
            f<<k<<"-";
            k = x;
        }
        f<<k<<","<<i<<"\n";
    }
    f.close();
    f.open(filename+".txt", ios::out | ios::app);
    f<<total<<","<<admitted<<"\n";
    f.close();
}

int main(int argc, char** argv)
{
    //freopen ("input.in","r",stdin);
    //freopen ("output.txt","w",stdout);
    //ios_base::sync_with_stdio(false);
    //cin.tie(NULL);

    string outputDir = "output/";
    vector<vector<vector<vector<int> > > > shortest;

    if(strcmp(argv[12], "hop") && strcmp(argv[12], "dist")){
        cout<<"Incorrect flag value "<<argv[12]<<"\n";
        exit(0);
    }
    if(strcmp(argv[14], "0") && strcmp(argv[14], "1")){
        cout<<"Incorrect p value "<<argv[14]<<"\n";
        exit(0);
    }

    readTopologyData(argv[2]);
    readConnectionData(argv[4]);
    bool flag = (bool)strcmp(argv[12], "hop");
    shortest = findShortestPaths(flag);
    shortest[2] = findLinkDisjointShortestPaths(flag, shortest[0]);
    //printPath(shortest);
    vector<int> v = setupConnections((bool)stoi(argv[14]), shortest);
    writeRoutingTable(outputDir+argv[6], shortest);
    writeForwardingTable(outputDir+argv[8], v, shortest);
    writePathsFile(outputDir+argv[10], v, shortest);

    return 0;
}