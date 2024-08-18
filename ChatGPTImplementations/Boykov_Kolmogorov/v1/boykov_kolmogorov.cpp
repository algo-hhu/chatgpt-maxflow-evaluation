#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <chrono>
#include <fstream>
#include <algorithm> 

using namespace std;
using namespace std::chrono;

const long long INF = std::numeric_limits<long long>::max();

struct Edge {
    int to;
    long long capacity;
    int reverseIndex;
};

class Graph {
public:
    Graph(int nodes) : adj(nodes) {}

    void addEdge(int from, int to, long long capacity) {
        adj[from].push_back({to, capacity, (int)adj[to].size()});
        adj[to].push_back({from, 0, (int)adj[from].size() - 1});
    }

    std::vector<Edge>& operator[](int node) {
        return adj[node];
    }

    int size() const {
        return adj.size();
    }

private:
    std::vector<std::vector<Edge>> adj;
};

enum TreeType { NONE = 0, S_TREE = -2, T_TREE = -1 };

struct Node {
    int sParent;
    int tParent;
    long long sParentCapacity;
    long long tParentCapacity;
    TreeType tree;
};

bool bfs(Graph& graph, vector<int>& sVisited, vector<int>& tVisited, vector<Node>& nodes, int source, int sink, int& meetingNode) {
    queue<int> sQueue, tQueue;
    fill(sVisited.begin(), sVisited.end(), NONE);
    fill(tVisited.begin(), tVisited.end(), NONE);

    sQueue.push(source);
    tQueue.push(sink);
    sVisited[source] = S_TREE;
    tVisited[sink] = T_TREE;
    nodes[source].tree = S_TREE;
    nodes[sink].tree = T_TREE;

    while (!sQueue.empty() || !tQueue.empty()) {
        if (!sQueue.empty()) {
            int u = sQueue.front();
            sQueue.pop();
            for (const Edge& edge : graph[u]) {
                if (sVisited[edge.to] == NONE && edge.capacity > 0) {
                    sVisited[edge.to] = S_TREE;
                    if (nodes[edge.to].tree == NONE) {
                        nodes[edge.to].sParent = u;
                        nodes[edge.to].sParentCapacity = edge.capacity;
                        nodes[edge.to].tree = S_TREE;
                        cout << "S-tree: Node " << u + 1 << " -> Node " << edge.to + 1 << " with capacity " << edge.capacity << endl;
                        cout << "Node " << edge.to + 1 << " assigned S-tree parent " << u + 1 << " with capacity " << edge.capacity << endl;
                    }
                    sQueue.push(edge.to);
                    if (tVisited[edge.to] == T_TREE) {
                        meetingNode = edge.to;
                        cout << "Path found via S-tree: Node " << edge.to + 1 << endl;
                        return true;
                    }
                }
            }
        }

        if (!tQueue.empty()) {
            int u = tQueue.front();
            tQueue.pop();
            for (const Edge& edge : graph[u]) {
                if (tVisited[edge.to] == NONE && graph[edge.to][edge.reverseIndex].capacity > 0) {
                    tVisited[edge.to] = T_TREE;
                    if (nodes[edge.to].tree == NONE) {
                        nodes[edge.to].tParent = u;
                        nodes[edge.to].tParentCapacity = graph[edge.to][edge.reverseIndex].capacity;
                        nodes[edge.to].tree = T_TREE;
                        cout << "T-tree: Node " << u + 1 << " -> Node " << edge.to + 1 << " with reverse capacity " << graph[edge.to][edge.reverseIndex].capacity << endl;
                        cout << "Node " << edge.to + 1 << " assigned T-tree parent " << u + 1 << " with reverse capacity " << graph[edge.to][edge.reverseIndex].capacity << endl;
                    }
                    tQueue.push(edge.to);
                    if (sVisited[edge.to] == S_TREE) {
                        meetingNode = edge.to;
                        cout << "Path found via T-tree: Node " << edge.to + 1 << endl;
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

long long augment(Graph& graph, vector<Node>& nodes, int source, int sink, int meetingNode) {
    long long flow = INF;
    vector<int> path;

    cout << "Augmenting path: ";

    // Pfad von der Quelle bis zum Treffpunkt
    for (int u = meetingNode; u != source; u = nodes[u].sParent) {
        path.push_back(u);
        cout << u + 1 << " <- ";
        flow = min(flow, nodes[u].sParentCapacity);
    }
    path.push_back(source);
    reverse(path.begin(), path.end());

    // Pfad von der Senke bis zum Treffpunkt
    for (int u = meetingNode; u != sink; u = nodes[u].tParent) {
        path.push_back(u);
        cout << u + 1 << " <- ";
        flow = min(flow, nodes[u].tParentCapacity);
    }
    path.push_back(sink);

    cout << " with flow " << flow << endl;

    if (flow == 0) {
        cout << "Error: Found augmenting path with zero flow." << endl;
        return 0;
    }

    for (size_t i = 0; i < path.size() - 1; ++i) {
        int u = path[i];
        int v = path[i + 1];
        for (Edge& edge : graph[u]) {
            if (edge.to == v) {
                edge.capacity -= flow;
                graph[v][edge.reverseIndex].capacity += flow;
                cout << "Updated capacity from Node " << u + 1 << " to Node " << v + 1 << ": " << edge.capacity << endl;
                cout << "Updated reverse capacity from Node " << v + 1 << " to Node " << u + 1 << ": " << graph[v][edge.reverseIndex].capacity << endl;
                break;
            }
        }
    }

    return flow;
}

long long boykovKolmogorovMaxFlow(Graph& graph, int source, int sink) {
    long long maxFlow = 0;
    vector<Node> nodes(graph.size());
    vector<int> sVisited(graph.size(), NONE);
    vector<int> tVisited(graph.size(), NONE);
    int meetingNode = -1;

    while (bfs(graph, sVisited, tVisited, nodes, source, sink, meetingNode)) {
        // Ausgabe des Zustands der Elternknoten und Kapazitäten nach der BFS
        cout << "State of nodes after BFS:" << endl;
        for (int i = 0; i < nodes.size(); ++i) {
            cout << "Node " << i + 1 << ": S-Parent = " << nodes[i].sParent + 1 << ", S-Parent Capacity = " << nodes[i].sParentCapacity << ", T-Parent = " << nodes[i].tParent + 1 << ", T-Parent Capacity = " << nodes[i].tParentCapacity << ", Tree = " << (nodes[i].tree == S_TREE ? "S_TREE" : (nodes[i].tree == T_TREE ? "T_TREE" : "NONE")) << endl;
        }

        long long flow = augment(graph, nodes, source, sink, meetingNode);
        if (flow == 0) break;
        cout << "Augmented flow: " << flow << endl;
        maxFlow += flow;

        // Zurücksetzen der Elterneinträge für die nächste Iteration
        for (Node& node : nodes) {
            node.sParent = -1;
            node.tParent = -1;
            node.sParentCapacity = 0;
            node.tParentCapacity = 0;
            node.tree = NONE;
        }
        sVisited.assign(graph.size(), NONE);
        tVisited.assign(graph.size(), NONE);
    }

    return maxFlow;
}

void readDIMACS(ifstream& file, Graph& graph, int& nodes, int& edges) {
    file >> ws;
    while (file.peek() == 'c' || file.peek() == 'n')
        file.ignore(numeric_limits<streamsize>::max(), '\n'); 

    string p, max;
    file >> p >> max >> nodes >> edges;

    graph = Graph(nodes);

    int u, v;
    long long capacity;
    for (int i = 0; i < edges; ++i) {
        file >> ws;
        while (file.peek() == 'c' || file.peek() == 'n')
            file.ignore(numeric_limits<streamsize>::max(), '\n'); 
        file >> p >> u >> v >> capacity;
        graph.addEdge(u - 1, v - 1, capacity); 
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " input.dimacs" << endl;
        return 1;
    }

    ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open file " << argv[1] << endl;
        return 1;
    }

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int nodes, edges;

    Graph graph(0);
    readDIMACS(inputFile, graph, nodes, edges);

    int source = 0; 
    int sink = 1;   

    auto start = high_resolution_clock::now();
    long long maxFlow = boykovKolmogorovMaxFlow(graph, source, sink);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);
    double seconds = duration.count() / 1e6;

    cout << "Maximaler Fluss: " << maxFlow << endl;
    cout << "Berechnungsdauer: " << seconds << " Sekunden" << endl;

    return 0;
}

