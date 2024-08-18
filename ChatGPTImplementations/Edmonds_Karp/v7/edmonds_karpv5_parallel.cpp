#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <climits>
#include <cstring>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <omp.h>

using namespace std;
using namespace std::chrono;

class Graph {
    int V;
    vector<unordered_map<int, long long>> adj;

public:
    Graph(int V) : V(V), adj(V) {}

    void addEdge(int u, int v, long long cap) {
        adj[u][v] = cap;
        (void)adj[v];
    }

    long long bfs(int s, int t, vector<int>& parent, long long scale) {
        fill(parent.begin(), parent.end(), -1);
        parent[s] = s;

        priority_queue<pair<long long, int>, vector<pair<long long, int>>, less<pair<long long, int>>> pq;
        pq.push({LLONG_MAX, s});

        while (!pq.empty()) {
            int cur = pq.top().second;
            long long flow = pq.top().first;
            pq.pop();

            for (const auto& next : adj[cur]) {
                if (parent[next.first] == -1 && next.second >= scale) {
                    parent[next.first] = cur;
                    long long new_flow = min(flow, next.second);
                    if (next.first == t) return new_flow;
                    pq.push({new_flow, next.first});
                }
            }
        }

        return 0;
    }

    long long edmondsKarp(int s, int t) {
        long long flow = 0;
        vector<int> parent(V);

        long long max_capacity = 0;
        #pragma omp parallel for reduction(max:max_capacity)
        for (int u = 0; u < V; ++u) {
            for (const auto& edge : adj[u]) {
                max_capacity = max(max_capacity, edge.second);
            }
        }

        long long scale = 1;
        while (scale <= max_capacity) {
            scale <<= 1;
        }

        while (scale >= 1) {
            long long new_flow;
            do {
                new_flow = bfs(s, t, parent, scale);
                if (new_flow) {
                    flow += new_flow;
                    int cur = t;
                    #pragma omp parallel
                    {
                        #pragma omp single nowait
                        {
                            while (cur != s) {
                                int prev = parent[cur];
                                #pragma omp task firstprivate(prev, cur)
                                {
                                    #pragma omp atomic
                                    adj[prev][cur] -= new_flow;
                                    #pragma omp atomic
                                    adj[cur][prev] += new_flow;
                                }
                                cur = prev;
                            }
                        }
                    }
                }
            } while (new_flow);
            scale >>= 1;
        }

        return flow;
    }

    static Graph readDIMACS(istream& in, int& source, int& sink) {
        string line;
        int V = 0, E = 0;
        Graph g(0);

        while (getline(in, line)) {
            if (line[0] == 'c') continue;

            istringstream iss(line);
            char type;
            iss >> type;

            if (type == 'p') {
                string temp;
                iss >> temp >> V >> E;
                g = Graph(V);
            } else if (type == 'a') {
                int u, v;
                long long cap;
                iss >> u >> v >> cap;
                g.addEdge(u - 1, v - 1, cap);
            }
        }

        source = 0;
        sink = 1;
        return g;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Fehler beim Öffnen der Datei" << endl;
        return 1;
    }

    int source = 0, sink = 1;
    Graph g = Graph::readDIMACS(infile, source, sink);

    auto start = high_resolution_clock::now();

    long long max_flow = g.edmondsKarp(source, sink);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<chrono::duration<double>>(stop - start);

    cout << "Maximaler Fluss: " << max_flow << endl;
    cout << fixed << setprecision(6);
    cout << "Berechnungsdauer: " << duration.count() << " Sekunden" << endl;

    return 0;
}

