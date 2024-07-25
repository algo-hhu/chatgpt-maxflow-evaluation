#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <chrono> 
#include <fstream> 
#include <sstream> 
#include <iomanip> 

using namespace std;
using namespace std::chrono;

class Graph {
    int V; 
    vector<vector<pair<int, long long>>> adj; 

public:
    Graph(int V) : V(V), adj(V) {}

    void addEdge(int u, int v, long long cap) {
        adj[u].emplace_back(v, cap); 
        adj[v].emplace_back(u, 0);   
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
                int next_node = next.first;
                long long capacity = next.second;

                if (parent[next_node] == -1 && capacity >= scale) { 
                    parent[next_node] = cur;
                    long long new_flow = min(flow, capacity);
                    if (next_node == t) return new_flow; 
                    pq.push({new_flow, next_node});
                }
            }
        }

        return 0; 
    }

    long long edmondsKarp(int s, int t) {
        long long flow = 0;
        vector<int> parent(V);

        
        long long max_capacity = 0;
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
            while (long long new_flow = bfs(s, t, parent, scale)) {
                flow += new_flow;
                int cur = t;

                while (cur != s) {
                    int prev = parent[cur];

                    
                    for (auto& edge : adj[prev]) {
                        if (edge.first == cur) {
                            edge.second -= new_flow;
                            break;
                        }
                    }
                    for (auto& edge : adj[cur]) {
                        if (edge.first == prev) {
                            edge.second += new_flow;
                            break;
                        }
                    }
                    cur = prev;
                }
            }
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

