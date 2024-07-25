#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

struct Edge {
    int to;
    long long capacity;
    int reverse_index;

    Edge(int to, long long capacity, int reverse_index) 
        : to(to), capacity(capacity), reverse_index(reverse_index) {}
};

class Graph {
public:
    Graph(int n) : adj(n), height(n), excess(n) {}

    void addEdge(int u, int v, long long capacity) {
        adj[u].emplace_back(v, capacity, adj[v].size());
        adj[v].emplace_back(u, 0, adj[u].size() - 1);
    }

    long long maxFlow(int s, int t) {
        int n = adj.size();
        initialize(s);

        vector<int> active_nodes;
        for (int i = 0; i < n; ++i) {
            if (i != s && i != t && excess[i] > 0) {
                active_nodes.push_back(i);
            }
        }

        while (!active_nodes.empty()) {
            int u = active_nodes.back();
            active_nodes.pop_back();

            bool pushed = false;
            for (Edge &edge : adj[u]) {
                if (edge.capacity > 0 && height[u] == height[edge.to] + 1) {
                    push(u, edge);
                    if (edge.to != s && edge.to != t && excess[edge.to] > 0 && find(active_nodes.begin(), active_nodes.end(), edge.to) == active_nodes.end()) {
                        active_nodes.push_back(edge.to);
                    }
                    pushed = true;
                }
            }

            if (!pushed) {
                relabel(u);
            }

            if (excess[u] > 0 && find(active_nodes.begin(), active_nodes.end(), u) == active_nodes.end()) {
                active_nodes.push_back(u);
            }
        }

        return excess[t];
    }

private:
    vector<vector<Edge>> adj;
    vector<long long> height;
    vector<long long> excess;

    void initialize(int s) {
        int n = adj.size();
        height[s] = n;
        for (Edge &edge : adj[s]) {
            long long capacity = edge.capacity;
            edge.capacity = 0;
            adj[edge.to][edge.reverse_index].capacity += capacity;
            excess[edge.to] += capacity;
            excess[s] -= capacity;
        }
    }

    void push(int u, Edge &edge) {
        long long delta = min(excess[u], edge.capacity);
        edge.capacity -= delta;
        adj[edge.to][edge.reverse_index].capacity += delta;
        excess[u] -= delta;
        excess[edge.to] += delta;
    }

    void relabel(int u) {
        long long min_height = LLONG_MAX;
        for (const Edge &edge : adj[u]) {
            if (edge.capacity > 0) {
                min_height = min(min_height, height[edge.to]);
            }
        }
        if (min_height < LLONG_MAX) {
            height[u] = min_height + 1;
        }
    }
};

void parseDIMACSInput(Graph &g, int &source, int &sink, istream &input) {
    string line;
    int num_nodes = 0, num_edges = 0;

    while (getline(input, line)) {
        if (line.empty() || line[0] == 'c') {
            continue;
        }

        istringstream iss(line);
        char type;
        iss >> type;

        if (type == 'p') {
            string problem_type;
            iss >> problem_type >> num_nodes >> num_edges;
            g = Graph(num_nodes);
            source = 0;
            sink = 1;
        } else if (type == 'a') {
            int u, v;
            long long capacity;
            iss >> u >> v >> capacity;
            g.addEdge(u - 1, v - 1, capacity);
        }
    }
}

int main(int argc, char *argv[]) {
    int source, sink;

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " input_file" << endl;
        return 1;
    }

    ifstream input_file(argv[1]);
    if (!input_file.is_open()) {
        cerr << "Error opening file: " << argv[1] << endl;
        return 1;
    }

    Graph g(0); 

    
    parseDIMACSInput(g, source, sink, input_file);

    auto start = high_resolution_clock::now(); 

    long long max_flow = g.maxFlow(source, sink);

    auto end = high_resolution_clock::now(); 
    duration<double> duration = end - start;

    cout << "Maximaler Fluss: " << max_flow << endl;
    cout << fixed << setprecision(6) << "Berechnungsdauer: " << duration.count() << " Sekunden" << endl;

    return 0;
}

