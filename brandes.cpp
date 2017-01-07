#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stack>
#include <queue>
#include <set>
#include <thread>
#include <mutex>

using namespace std;
typedef unordered_map <int, int> MAPII;
typedef unordered_map <int, vector<int>> MAPIV;
typedef unordered_map <int, double> MAPID;

MAPIV graph;
vector<int> vertices;
vector<int> keys;
mutex mut;

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

void node_processing(int node, MAPID& BC) {
    stack<int> S;
    MAPIV P; // predecessors of w on all the shortest paths from s
    MAPII dist;  // distance from w to s
    MAPID delta; // // betweenness value for w in paths from s
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
            delta[pred] += (double(sigma[pred]) / double(sigma[w])) * (1 + delta[w]);

        if(w != node) {
            mut.lock();
            BC[w] += delta[w];
            mut.unlock();
        }
    }

}

void multi_node_processing(vector<int>& node_vec, MAPID& BC) {
    for (auto node : node_vec)
        node_processing(node, BC);
}

MAPID betweenness(int thread_number, vector<thread>& threads) {
    MAPID BC;

    vector<vector<int> > tasks(thread_number);

    int th = 0;
    for (int node : vertices) {
        tasks[th].push_back(node);
        th = (th + 1) % thread_number;
    }


    for (int i = 0; i < thread_number; i++)
        threads[i] = thread{[i, &tasks, &BC]{multi_node_processing(tasks[i], BC);}};

    for (int i = 0; i < thread_number; i++)
        threads[i].join();

    return BC;
}

void print_results(int thread_number, ofstream& output) {
    vector<thread> threads(thread_number);
    MAPID BC = betweenness(thread_number, threads);

    sort (keys.begin(), keys.end());

    for (auto node : keys)
        output << node << " " << BC[node] << endl;

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

    for (auto entry : graph)
        keys.push_back(entry.first);

}

int main(int argc, char* argv[]) {

    int thread_number = stoi(argv[1]);
    ifstream input;
    ofstream output;
    input.open(argv[2]);
    output.open(argv[3]);

    // debug for parameters
    for (int i = 0; i < argc; i++)
        cout << argv[i] << endl;

    take_input(input);

    /**
    // just debug
    for (auto it = graph.begin(); it!= graph.end(); it++)
    {
        cout << (*it).first << "  ";
        for (auto inner = (*it).second.begin(); inner != (*it).second.end();
    inner++)
        cout << (*inner) << " ";
        cout << endl;
    }
    */

    print_results(thread_number, output);

    input.close();
    output.close();
    return 0;
}
