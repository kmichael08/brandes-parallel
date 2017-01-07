#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stack>
#include <queue>
#include <set>

using namespace std;
typedef unordered_map <int, int> MAPII;
typedef unordered_map <int, vector<int>> MAPIV;
typedef unordered_map <int, float> MAPIF;

MAPIV graph;
vector<int> vertices;

void process_neighbour(const int w, const int v, MAPII& dist, queue<int>& Q, MAPII& sigma, MAPIV& P) {
    if (dist[w] < 0) { // w not visited yet
        Q.push(w); // w on the queue
        dist[w] = dist[v] + 1; // set the distance (bfs way)
    }

    if (dist[w] == dist[v] + 1) { // if w is the 'next' (one farther)
        sigma[w] += sigma[v];  // all shortest for w are the shortest for v (adding wv edge)
        P[w].push_back(v); // w is the predeccessor for w
    }
}

void node_processing(int node, MAPIF& BC) {
    stack<int> S;
    MAPIV P; // predecessors of w on all the shortest paths from s
    MAPII dist;  // distance from w to s
    MAPIF delta; // // betweenness value for w in paths from s
    MAPII sigma; //number of the shortesh paths from s to w

    for (int w : vertices) {
        sigma[w] = 0;
        delta[w] = 0;
        dist[w] = -1;
    }

    sigma[node] = 1;
    dist[node] = 0;
    queue<int> Q;
    Q.push(node);

    while (!Q.empty()) {
        int v = Q.front();
        Q.pop();
        S.push(v);

        for (int neighbour : graph[v])
            process_neighbour(neighbour, v, dist, Q, sigma, P);
    }

    while (!S.empty()) {
        int w = S.top();
        S.pop();

        for (int pred : P[w])
            delta[pred] += (sigma[pred] / sigma[w]) * (1 + delta[w]);

        if(w != node)
            BC[w] += delta[w];
    }

}

// TODO parallelize nodeBetweenness calls
vector<pair<int, float> > betweenness() {
    vector<pair<int, float> > result;
    MAPIF BC;

    for (int node : vertices)
        node_processing(node, BC);

    for (auto entry : BC)
        result.push_back(entry);

    sort(result.begin(), result.end());
    return result;
}

void print_results() {
    vector<pair<int, float> > result = betweenness();

    for (auto res : result) {
        if (graph[res.first].size()) // print only nodes with out degree > 0
            cout << res.first << " " << res.second << endl;
    }

}

void take_input(ifstream& input) {
    int a, b;
    set<int> vertSet;

    while (input >> a >> b) {
        graph[a].push_back(b);
        vertSet.insert(a);
        vertSet.insert(b);
    }

    vertices.assign(vertSet.begin(), vertSet.end());
    
}

int main(int argc, char* argv[]) {

    int threadNumber = stoi(argv[1]);
    ifstream input;
    ofstream output;
    input.open(argv[2]);
    output.open(argv[3]);

    // debug for parameters
    for (int i = 0; i < argc; i++)
        cout << argv[i] << endl;

    take_input(input);

    // just debug
    for (auto it = graph.begin(); it!= graph.end(); it++)
    {
        cout << (*it).first << "  ";
        for (auto inner = (*it).second.begin(); inner != (*it).second.end();
    inner++)
        cout << (*inner) << " ";
        cout << endl;
    }

    print_results();

    input.close();
    output.close();
    return 0;
}
