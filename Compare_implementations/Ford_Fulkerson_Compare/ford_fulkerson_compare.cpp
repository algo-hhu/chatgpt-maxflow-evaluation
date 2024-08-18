#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <functional>
#include <sstream>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define fst first
#define snd second
#define all(c) ((c).begin()), ((c).end())

const int INF = 1 << 30;
struct graph {
    typedef long long flow_type;
    struct edge {
        int src, dst;
        flow_type capacity, flow;
        size_t rev;
    };
    int n;
    vector<vector<edge>> adj;
    graph(int n) : n(n), adj(n) { }
    void add_edge(int src, int dst, flow_type capacity) {
        adj[src].push_back({src, dst, capacity, 0, adj[dst].size()});
        adj[dst].push_back({dst, src, 0, 0, adj[src].size() - 1});
    }
    int max_flow(int s, int t) {
        vector<bool> visited(n);
        function<flow_type(int, flow_type)> augment = [&](int u, flow_type cur) {
            if (u == t) return cur;
            visited[u] = true;
            for (auto &e : adj[u]) {
                if (!visited[e.dst] && e.capacity > e.flow) {
                    flow_type f = augment(e.dst, min(e.capacity - e.flow, cur));
                    if (f > 0) {
                        e.flow += f;
                        adj[e.dst][e.rev].flow -= f;
                        return f;
                    }
                }
            }
            return flow_type(0);
        };
        for (int u = 0; u < n; ++u)
            for (auto &e : adj[u]) e.flow = 0;

        flow_type flow = 0;
        while (1) {
            fill(all(visited), false);
            flow_type f = augment(s, INF);
            if (f == 0) break;
            flow += f;
        }
        return flow;
    }
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " input.max" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Error opening file: " << argv[1] << endl;
        return 1;
    }

    string line;
    int n = 0, m = 0;
    graph g(0); // Initialer leerer Graph

    while (getline(infile, line)) {
        if (line[0] == 'c') {
            // Kommentarzeile, überspringen
            continue;
        } else if (line[0] == 'p') {
            // Problemzeile
            stringstream ss(line);
            string tmp;
            ss >> tmp >> tmp >> n >> m;
            g = graph(n); // Graph mit n Knoten erstellen
        } else if (line[0] == 'a') {
            // Kantenzeile
            stringstream ss(line);
            string tmp;
            int u, v, capacity;
            ss >> tmp >> u >> v >> capacity;
            g.add_edge(u - 1, v - 1, capacity);  // 1-basiert in 0-basiert umwandeln
        }
    }

    int s = 0; // Quelle hat Index 1, wird zu 0-basiertem Index 0
    int t = 1; // Senke hat Index 2, wird zu 0-basiertem Index 1

    auto start = high_resolution_clock::now();
    int maxFlow = g.max_flow(s, t);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<chrono::duration<double>>(stop - start);

    cout << "Maximaler Fluss: " << maxFlow << endl;
    cout << "Berechnungsdauer: " << duration.count() << " Sekunden" << endl;

    return 0;
}

