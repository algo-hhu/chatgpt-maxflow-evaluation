#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <climits>
#include <chrono>
#include <tbb/tbb.h>
#include <mutex>

using namespace std;
using namespace tbb;
using namespace std::chrono;

struct Edge {
    int to, rev;
    long long cap, flow;
};

class Dinic {
public:
    Dinic(int n, int m) : n(n), adj(n), level(n), ptr(n) {
        for (auto &vec : adj) {
            vec.reserve(m / n); 
        }
    }

    void add_edge(int u, int v, long long cap) {
        adj[u].push_back({v, static_cast<int>(adj[v].size()), cap, 0});
        adj[v].push_back({u, static_cast<int>(adj[u].size() - 1), 0, 0});
    }

    long long max_flow(int s, int t) {
        long long flow = 0;
        while (bfs(s, t)) {
            fill(ptr.begin(), ptr.end(), 0);
            long long pushed;
            while ((pushed = dfs(s, t, LLONG_MAX)) != 0) {
                flow += pushed;
            }
        }
        return flow;
    }

private:
    int n;
    vector<vector<Edge>> adj;
    vector<int> level, ptr;
    std::mutex q_mtx; // Mutex to protect queue operations

    bool bfs(int s, int t) {
        concurrent_queue<int> q;
        q.push(s);
        fill(level.begin(), level.end(), -1);
        level[s] = 0;
        bool found_sink = false;

        while (!q.empty() && !found_sink) {
            size_t queue_size = q.unsafe_size();
            vector<int> nodes(queue_size);
            for (size_t i = 0; i < queue_size; ++i) {
                q.try_pop(nodes[i]);
            }

            parallel_for_each(nodes.begin(), nodes.end(), [&](int u) {
                for (const auto& e : adj[u]) {
                    if (level[e.to] == -1 && e.flow < e.cap) {
                        {
                            lock_guard<std::mutex> lock(q_mtx);
                            level[e.to] = level[u] + 1;
                            q.push(e.to);
                        }
                        if (e.to == t) {
                            found_sink = true;
                        }
                    }
                }
            });
        }

        return level[t] != -1;
    }

    long long dfs(int u, int t, long long flow) {
        if (flow == 0) return 0;
        if (u == t) return flow;

        for (; ptr[u] < adj[u].size(); ++ptr[u]) {
            Edge& e = adj[u][ptr[u]];
            if (level[e.to] == level[u] + 1 && e.flow < e.cap) {
                long long pushed = dfs(e.to, t, min(flow, e.cap - e.flow));
                if (pushed > 0) {
                    e.flow += pushed;
                    adj[e.to][e.rev].flow -= pushed;
                    return pushed;
                }
            }
        }

        
        bool productive = false;
        for (const auto& e : adj[u]) {
            if (level[e.to] == level[u] + 1 && e.flow < e.cap) {
                productive = true;
                break;
            }
        }

        if (!productive) {
            level[u] = -1;
        }

        return 0;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Could not open file " << argv[1] << endl;
        return 1;
    }

    int n, m;
    infile >> ws;
    string line;
    while (getline(infile, line)) {
        if (line[0] == 'p') {
            if (sscanf(line.c_str(), "p max %d %d", &n, &m) != 2) {
                cerr << "Error reading problem line." << endl;
                return 1;
            }
            break;
        }
    }

    Dinic dinic(n, m);
    while (getline(infile, line)) {
        if (line[0] == 'a') {
            int u, v;
            long long cap;
            if (sscanf(line.c_str(), "a %d %d %lld", &u, &v, &cap) != 3) {
                cerr << "Error reading edge line." << endl;
                return 1;
            }
            dinic.add_edge(u - 1, v - 1, cap); 
        }
    }

    infile.close();

    int source = 0; 
    int sink = 1;   

    auto start = high_resolution_clock::now();
    long long maxFlow = dinic.max_flow(source, sink);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<chrono::duration<double>>(stop - start);

    cout << "Maximaler Fluss: " << maxFlow << endl;
    cout << "Berechnungsdauer: " << duration.count() << " Sekunden" << endl;

    return 0;
}

