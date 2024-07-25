#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <climits>
#include <chrono>
#include <stack>
#include <memory>

using namespace std;
using namespace std::chrono;

struct Edge {
    int to, rev;
    long long cap, flow;
};

class EdgePool {
public:
    EdgePool() : pool_size(1000), pool_position(0), current_pool_index(-1) {}

    Edge* allocate(size_t size) {
        if (current_pool_index == -1 || pool_position + size > pool_size) {
            expand_pool(size);
        }
        Edge* ptr = pools[current_pool_index].get() + pool_position;
        pool_position += size;
        return ptr;
    }

private:
    void expand_pool(size_t size) {
        pool_size = max(size, pool_size * 2);
        current_pool_index++;
        pool_position = 0;
        pools.push_back(make_unique<Edge[]>(pool_size));
    }

    vector<unique_ptr<Edge[]>> pools;
    size_t pool_size;
    size_t pool_position;
    int current_pool_index;
};

class Dinic {
public:
    Dinic(int n, int m) : n(n), adj(n), level(n), ptr(n), edge_pool() {
        for (auto &vec : adj) {
            vec.reserve(m / n); // Reserve space based on average number of edges per node
        }
    }

    void add_edge(int u, int v, long long cap) {
        Edge* e1 = edge_pool.allocate(1);
        Edge* e2 = edge_pool.allocate(1);
        *e1 = {v, static_cast<int>(adj[v].size()), cap, 0};
        *e2 = {u, static_cast<int>(adj[u].size()), 0, 0};
        adj[u].push_back(*e1);
        adj[v].push_back(*e2);
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
    EdgePool edge_pool;

    bool bfs(int s, int t) {
        queue<int> q;
        q.push(s);
        fill(level.begin(), level.end(), -1);
        level[s] = 0;
        while (!q.empty() && level[t] == -1) {
            int u = q.front();
            q.pop();
            for (const auto& e : adj[u]) {
                if (level[e.to] == -1 && e.flow < e.cap) {
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }
        return level[t] != -1;
    }

    long long dfs(int u, int t, long long flow) {
        if (flow == 0) return 0;
        if (u == t) return flow;

        for (int& i = ptr[u]; i < adj[u].size(); ++i) {
            Edge& e = adj[u][i];
            if (level[e.to] == level[u] + 1 && e.flow < e.cap) {
                long long pushed = dfs(e.to, t, min(flow, e.cap - e.flow));
                if (pushed > 0) {
                    e.flow += pushed;
                    adj[e.to][e.rev].flow -= pushed;
                    return pushed;
                }
            }
        }

        // Only mark the node as non-productive if all outgoing edges are fully utilized
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
            dinic.add_edge(u - 1, v - 1, cap); // Convert to 0-indexed
        }
    }

    infile.close();

    int source = 0; // 1 in DIMACS format
    int sink = 1;   // 2 in DIMACS format

    auto start = high_resolution_clock::now();
    long long maxFlow = dinic.max_flow(source, sink);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<chrono::duration<double>>(stop - start);

    cout << "Maximaler Fluss: " << maxFlow << endl;
    cout << "Berechnungsdauer: " << duration.count() << " Sekunden" << endl;

    return 0;
}

