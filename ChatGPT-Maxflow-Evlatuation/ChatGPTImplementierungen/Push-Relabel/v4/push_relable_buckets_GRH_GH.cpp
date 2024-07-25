#include <iostream>
#include <vector>
#include <deque>
#include <climits>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <queue>

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
    Graph(int n) 
        : adj(n), height(n), excess(n), bucket(n * 2), active(n, false), 
          max_height(0), relabel_count(0), height_count(n * 2, 0) {}

    void addEdge(int u, int v, long long capacity) {
        adj[u].emplace_back(v, capacity, adj[v].size());
        adj[v].emplace_back(u, 0, adj[u].size() - 1);
    }

    long long maxFlow(int s, int t) {
        int n = adj.size();
        initialize(s);

        while (max_height >= 0) {
            if (relabel_count >=  n) {
                globalRelabel(t);
                relabel_count = 0;
            }

            if (!bucket[max_height].empty()) {
                int u = bucket[max_height].front();
                bucket[max_height].pop_front();
                active[u] = false;

                bool pushed = false;
                for (Edge &edge : adj[u]) {
                    if (edge.capacity > 0 && height[u] == height[edge.to] + 1) {
                        push(u, edge);
                        if (edge.to != s && edge.to != t && !active[edge.to]) {
                            addActive(edge.to);
                        }
                        pushed = true;
                    }
                }

                if (!pushed) {
                    relabel(u);
                }

                if (excess[u] > 0) {
                    addActive(u);
                }

                ++relabel_count;
            } else {
                max_height--;
            }
        }

        return excess[t];
    }

private:
    vector<vector<Edge>> adj;
    vector<int> height;
    vector<long long> excess;
    vector<deque<int>> bucket;
    vector<bool> active;
    int max_height;
    int relabel_count;
    vector<int> height_count;

    void initialize(int s) {
        int n = adj.size();
        height[s] = n;
        active.assign(n, false);
        max_height = 0;
        fill(height_count.begin(), height_count.end(), 0);
        height_count[0] = n - 1;
        height_count[n] = 1;

        for (Edge &edge : adj[s]) {
            long long capacity = edge.capacity;
            if (capacity > 0) {
                edge.capacity = 0;
                adj[edge.to][edge.reverse_index].capacity += capacity;
                excess[edge.to] += capacity;
                excess[s] -= capacity;
                height[edge.to] = 1;
                addActive(edge.to);
            }
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
        int min_height = INT_MAX;
        for (const Edge &edge : adj[u]) {
            if (edge.capacity > 0) {
                min_height = min(min_height, height[edge.to]);
            }
        }
        if (min_height < INT_MAX) {
            int old_height = height[u];
            height[u] = min_height + 1;
            max_height = max(max_height, height[u]);
            height_count[old_height]--;
            height_count[height[u]]++;
            if (height_count[old_height] == 0 && old_height < max_height) {
                gapHeuristic(old_height);
            }
        }
    }

    void addActive(int u) {
        active[u] = true;
        bucket[height[u]].push_back(u);
        max_height = max(max_height, height[u]);
    }

    void globalRelabel(int t) {
        int n = adj.size();
        fill(height.begin(), height.end(), n);
        fill(active.begin(), active.end(), false);
        for (auto &b : bucket) {
            b.clear();
        }
        fill(height_count.begin(), height_count.end(), 0);

        queue<int> q;
        height[t] = 0;
        q.push(t);
        height_count[0] = 1;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (Edge &edge : adj[u]) {
                if (adj[edge.to][edge.reverse_index].capacity > 0 && height[edge.to] == n) {
                    height[edge.to] = height[u] + 1;
                    q.push(edge.to);
                    height_count[height[edge.to]]++;
                    if (excess[edge.to] > 0 && edge.to != t) {
                        addActive(edge.to);
                    }
                }
            }
        }

        max_height = *max_element(height.begin(), height.end());
    }

    void gapHeuristic(int gap_height) {
        int n = adj.size();
        for (int i = 0; i < n; ++i) {
            if (height[i] > gap_height) {
                height_count[height[i]]--;
                height[i] = n;
                height_count[n]++;
                if (active[i]) {
                    auto &bucket_list = bucket[height[i]];
                    bucket_list.erase(remove(bucket_list.begin(), bucket_list.end(), i), bucket_list.end());
                    addActive(i);
                }
            }
        }
        max_height = gap_height - 1;
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

