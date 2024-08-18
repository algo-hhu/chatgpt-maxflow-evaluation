#include <iostream>
#include <vector>
#include <cstring>
#include <climits>
#include <ctime>
#include <algorithm>

using namespace std;

struct Edge {
    int from, to, capacity, flow;
};

bool dfs(vector<vector<int>>& adj, vector<Edge>& edges, vector<bool>& visited, vector<int>& parent, int current, int sink) {
    if (current == sink) return true;

    visited[current] = true;
    
    sort(adj[current].begin(), adj[current].end(), [&](int a, int b) {
        return (edges[a].capacity - edges[a].flow) > (edges[b].capacity - edges[b].flow);
    });

    for (int edge_index : adj[current]) {
        Edge& edge = edges[edge_index];
        if (!visited[edge.to] && edge.capacity > edge.flow) {
            parent[edge.to] = edge_index;
            if (dfs(adj, edges, visited, parent, edge.to, sink))
                return true;
        }
    }
    return false;
}

int ford_fulkerson(vector<vector<int>>& adj, vector<Edge>& edges, int source, int sink) {
    int max_flow = 0;
    vector<int> parent(adj.size());
    vector<bool> visited(adj.size());

    while (true) {
        fill(visited.begin(), visited.end(), false);
        fill(parent.begin(), parent.end(), -1);
        if (!dfs(adj, edges, visited, parent, source, sink)) break;

        int flow = INT_MAX;
        for (int v = sink; v != source; v = edges[parent[v]].from) {
            flow = min(flow, edges[parent[v]].capacity - edges[parent[v]].flow);
        }
        for (int v = sink; v != source; v = edges[parent[v]].from) {
            edges[parent[v]].flow += flow;
            edges[parent[v] ^ 1].flow -= flow;
        }
        max_flow += flow;
    }
    return max_flow;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " input.max" << endl;
        return 1;
    }

    if (!freopen(argv[1], "r", stdin)) {
        cerr << "Error: Unable to open input file " << argv[1] << endl;
        return 1;
    }

    int num_nodes, num_edges;
    vector<Edge> edges;
    vector<vector<int>> adj;

    string line;
    while (getline(cin, line)) {
        if (line[0] == 'c') {
            continue; 
        } else if (line[0] == 'p') {
            sscanf(line.c_str(), "p max %d %d", &num_nodes, &num_edges);
            adj.resize(num_nodes + 1);
        } else if (line[0] == 'a') {
            int from, to, capacity;
            sscanf(line.c_str(), "a %d %d %d", &from, &to, &capacity);
            edges.push_back({from, to, capacity, 0});
            edges.push_back({to, from, 0, 0}); 
            adj[from].push_back(edges.size() - 2);
            adj[to].push_back(edges.size() - 1);
        }
    }

    int source = 1, sink = 2;

    clock_t start = clock();
    int max_flow = ford_fulkerson(adj, edges, source, sink);
    clock_t end = clock();

    double duration = double(end - start) / CLOCKS_PER_SEC;

    cout << "Maximaler Fluss: " << max_flow << endl;
    cout << "Berechnungsdauer: " << duration << " Sekunden" << endl;

    return 0;
}

