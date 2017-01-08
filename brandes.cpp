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
queue<int> nodes_to_process;

MAPIV graph;
vector<int> vertices;
vector<int> keys;
mutex mut;
mutex tasks;

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
    MAPII sigma; //number of the shortest paths from s to w

    for (int w : vertices)
        dist[w] = -1;

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
            lock_guard<mutex> lock(mut); // only one thread can modify the bc map
            BC[w] += delta[w];
        }
    }

}

void multi_node_processing(MAPID& BC) {
    while(true) {
        int top;
        {
            lock_guard<mutex> lock(tasks);
            if (nodes_to_process.empty())
                break;

            top = nodes_to_process.front();
            nodes_to_process.pop();
        }
        node_processing(top, BC);

    }
}

MAPID betweenness(unsigned thread_number, vector<thread>& threads) {
    MAPID BC;

    for (int v : vertices)
        nodes_to_process.push(v);

    for (unsigned i = 0; i < thread_number; i++)
        threads[i] = thread{[&BC]{multi_node_processing(BC);}};

    for (unsigned i = 0; i < thread_number; i++)
        threads[i].join();

    return BC;
}

void print_results(unsigned thread_number, ofstream& output) {
    vector<thread> threads(thread_number);
    MAPID BC = betweenness(thread_number, threads);

    for (auto node : keys)
        output << node << " " << BC[node] << endl;

}

void take_input(ifstream& input) {
    int a, b;
    set<int> vertSet; // set of vertices

    while (input >> a >> b) {
        graph[a].push_back(b);
        if (keys.empty() || keys.back() != a)
            keys.push_back(a);

        vertSet.insert(a);
        vertSet.insert(b);
    }

    vertices.assign(vertSet.begin(), vertSet.end());
}

int main(int argc, char* argv[]) {

    unsigned thread_number = unsigned(stoi(argv[1]));
    ifstream input;
    ofstream output;
    input.open(argv[2]);
    output.open(argv[3]);

    take_input(input);

    print_results(thread_number, output);
		
    input.close();
    output.close();
    return 0;
}
