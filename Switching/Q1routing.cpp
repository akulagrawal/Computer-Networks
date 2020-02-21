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
        return top.propDelay;
    return 1;
}

vector<int> findRoute1(vector<vector<int> > dist, int i, int j, vector<vector<pii> > graph, map<pair<int, pii>, bool> &my){
    vector<int> v;
    int k=i;
    v.pb(i);
    while(k!=j){
        int n=graph[k].size();
        for(int l=0;l<n;l++){
            int x=graph[k][l].f;
            if(dist[k][j]==dist[k][x]+dist[x][j]){
                pair<int, pii> temp = mp(graph[k][l].s, mp(k,x));
                my[temp] = 1;
                temp.s = mp(x,k);
                my[temp] = 1;
                v.pb(x);
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
    vector<vector<pii> > graph(V, vector<pii>());
    vector<vector<vector<vector<int> > > > shortest(3, vector<vector<vector<int> > >(V, vector<vector<int> >(V, vector<int>())));
    for(int i=0;i<topList.size();i++){
        dist[topList[i].node1][topList[i].node2] = min(distance(topList[i], flag), dist[topList[i].node1][topList[i].node2]);
        dist[topList[i].node2][topList[i].node1] = min(distance(topList[i], flag), dist[topList[i].node1][topList[i].node1]);
        graph[topList[i].node1].pb(mp(topList[i].node2, distance(topList[i], flag)));
        graph[topList[i].node2].pb(mp(topList[i].node1, distance(topList[i], flag)));
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
            map<pair<int, pii>, bool> my;
            shortest[0][i][j] = findRoute1(dist, i, j, graph, my);
            int minm=INF;
            int v1=0, v2=0;
            bool isSecond=0;
            for(int l=0;l<topList.size();l++){
                pair<int, pii> temp1 = mp(distance(topList[l], flag), mp(topList[l].node1, topList[l].node2));
                pair<int, pii> temp2 = mp(distance(topList[l], flag), mp(topList[l].node2, topList[l].node1));
                if((!my[temp1])&&(!my[temp2])&&(topList[l].node1!=topList[l].node2)){
                    if(minm > (temp1.f+dist[i][temp1.s.f]+dist[temp1.s.s][j])){
                        isSecond = 1;
                        minm = temp1.f+dist[i][temp1.s.f]+dist[temp1.s.s][j];
                        v1 = temp1.s.f;
                        v2 = temp1.s.s;
                    }
                    if(minm > (temp2.f+dist[i][temp2.s.f]+dist[temp2.s.s][j])){
                        isSecond = 1;
                        minm = temp2.f+dist[i][temp2.s.f]+dist[temp2.s.s][j];
                        v1 = temp2.s.f;
                        v2 = temp2.s.s;
                    }
                }
            }
            if(isSecond){
                vector<int> vec1 = findRoute1(dist, i, v1, graph, my);
                vector<int> vec2 = findRoute1(dist, v2, j, graph, my);
                vec1.insert(vec1.end(), vec2.begin(), vec2.end());
                shortest[1][i][j] = vec1;
            }
        }
    }
    return shortest;
}

vector<int> findRoute2(vector<int> cost, int i, int j, vector<vector<int> > graph, vector<vector<int> > dist){
    vector<int> v;
    int k=j;
    v.pb(j);
    while(k!=i){
        int n=graph[k].size();
        for(int l=0;l<n;l++){
            int x=graph[k][l];
            if(cost[k]==cost[x]+dist[x][k]){
                v.pb(x);
                k=x;
                break;
            }
        }
    }
    return v;
}

vector<vector<vector<int> > > findLinkDisjointShortestPaths(bool flag, vector<vector<vector<int> > > sPath){

    vector<vector<vector<int> > > shortest(V, vector<vector<int> >(V, vector<int>()));

    vector<vector<int> > oldDist(V, vector<int>(V, INF));
    vector<vector<int> > oldGraph(V, vector<int>());
    for(int i=0;i<topList.size();i++){
        oldDist[topList[i].node1][topList[i].node2] = min(distance(topList[i], flag), oldDist[topList[i].node1][topList[i].node2]);
        oldDist[topList[i].node2][topList[i].node1] = min(distance(topList[i], flag), oldDist[topList[i].node1][topList[i].node1]);
        oldGraph[topList[i].node1].pb(topList[i].node2);
        oldGraph[topList[i].node2].pb(topList[i].node1);
    }

    for(int i=0;i<V;i++)
        oldDist[i][i] = 0;

    vector<vector<int> > dist(V, vector<int>(V, INF));
    vector<vector<int> > graph(V, vector<int>());
    for(int i=0;i<V;i++){
        for(int j=0;j<V;j++){
            if(!sPath[i][j].size())
                continue;
            vector<int> cost(V, INF);
            dist = oldDist;
            graph = oldGraph;
            vector<int> route = sPath[i][j];
            dist[i][route[0]] = INF;
            dist[route[0]][i] = INF;
            for(int k=1;k<route.size();k++){
                dist[route[k-1]][route[k]] = INF;
                dist[route[k]][route[k-1]] = INF;
                graph[route[k]].erase(remove(graph[route[k]].begin(), graph[route[k]].end(), route[k-1]), graph[route[k]].end());
                graph[route[k-1]].erase(remove(graph[route[k-1]].begin(), graph[route[k-1]].end(), route[k]), graph[route[k-1]].end());
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
                    if(cost[graph[current][j]] > cost[current] + dist[current][graph[current][j]]){
                        cost[graph[current][j]] = cost[current] + dist[current][graph[current][j]];
                        pq.push(mp(cost[graph[current][j]], graph[current][j]));
                    }
                }
            }
            shortest[j][i] = findRoute2(cost, i, j, graph, dist);
        }
    }

    return shortest;
}

void printPath(vector<vector<vector<vector<int> > > > shortest){
    for(int i=0;i<V;i++){
        for(int j=0;j<V;j++){
            cout<<"Path 1 from "<<i<<" to "<<j<<":\n";
            for(int k=0;k<shortest[0][i][j].size();k++)
                cout<<shortest[0][i][j][k]<<"->";
            cout<<endl;
            cout<<"Path 2 from "<<i<<" to "<<j<<":\n";
            for(int k=0;k<shortest[1][i][j].size();k++)
                cout<<shortest[1][i][j][k]<<"->";
            cout<<endl;
            cout<<"Path 3 from "<<i<<" to "<<j<<":\n";
            for(int k=0;k<shortest[2][i][j].size();k++)
                cout<<shortest[2][i][j][k]<<"->";
            cout<<endl;
        }
    }
}

bool checkRoute(vector<int> route, connection con, bool p){
    if(p==0){
        for(int i=1;i<route.size();i++){
            topology temp = topList[edgeToTop[mp(route[i-1], route[i])]];
            if(con.bequiv > temp.capacity - temp.bequivSum)
                return 0;
        }
        for(int i=1;i<route.size();i++)
            topList[edgeToTop[mp(route[i-1], route[i])]].bequivSum += con.bequiv;
        return 1;
    }
    for(int i=1;i<route.size();i++){
        topology temp = topList[edgeToTop[mp(route[i-1], route[i])]];
        if(con.bmax > temp.capacity - temp.bmaxSum)
            return 0;
    }
    for(int i=1;i<route.size();i++)
        topList[edgeToTop[mp(route[i-1], route[i])]].bmaxSum += con.bmax;
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

vector<int> writeRoutingTable(vector<vector<vector<vector<int> > > > shortest){
    fstream f;
    f.open("routingtable.csv", ios::out);
    for(int i=0;i<V;i++){
        f.open("routingtable_"+to_string(i)+".csv", ios::out);
        f.close();
    }
    for(int i=0;i<V;i++){
        for(int j=0;j<V;j++){
            vector<int> route = shortest[0][i][j];
            for(int k=0;k<route.size();k++){
                f.open("routingtable_"+to_string(route[k])+".csv", ios::out, ios::app);
                f<<route[k+1]<<","<<edgeToTop(mp(route[k], route[k+1]))<<","<<
            }
        }
    }
}

int main()
{
    //freopen ("input.in","r",stdin);
    freopen ("output.txt","w",stdout);
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    vector<vector<vector<vector<int> > > > shortest;

    readConnectionData("ARPANET_100.doc");
    readTopologyData("top24.doc");
    shortest = findShortestPaths(1);
    shortest[2] = findLinkDisjointShortestPaths(1, shortest[0]);
    printPath(shortest);
    vector<int> v = setupConnections(0, shortest);
    for(int i=0;i<v.size();i++)
        cout<<v[i]<<" ";

    return 0;
}