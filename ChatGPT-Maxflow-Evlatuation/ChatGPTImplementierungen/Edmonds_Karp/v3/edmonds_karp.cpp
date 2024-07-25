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

using namespace std;
using namespace std::chrono;

class RingBufferQueue {
    vector<int> buffer;
    int head;
    int tail;
    int capacity;
    int size;

public:
    RingBufferQueue(int capacity) : buffer(capacity), head(0), tail(0), capacity(capacity), size(0) {}

    bool isEmpty() const {
        return size == 0;
    }

    bool isFull() const {
        return size == capacity;
    }

    void enqueue(int value) {
        if (isFull()) {
            throw overflow_error("Queue is full");
        }
        buffer[tail] = value;
        tail = (tail + 1) % capacity;
        size++;
    }

    int dequeue() {
        if (isEmpty()) {
            throw underflow_error("Queue is empty");
        }
        int value = buffer[head];
        head = (head + 1) % capacity;
        size--;
        return value;
    }
};

class Graph {
    int V; 
    vector<unordered_map<int, long long>> adj; 

public:
    Graph(int V) : V(V), adj(V) {}

    void addEdge(int u, int v, long long cap) {
        adj[u][v] = cap; 
        (void)adj[v]; 
    }

    long long bfs(int s, int t, vector<int>& parent) {
        fill(parent.begin(), parent.end(), -1); // Setze alle Elternknoten auf -1
        parent[s] = s;
        RingBufferQueue q(V); 
        q.enqueue(s);

        vector<long long> flow(V, 0);
        flow[s] = LLONG_MAX;

        while (!q.isEmpty()) {
            int cur = q.dequeue();

            for (const auto& next : adj[cur]) {
                if (parent[next.first] == -1 && next.first != s && next.second > 0) { 
                    parent[next.first] = cur;
                    flow[next.first] = min(flow[cur], next.second);
                    if (next.first == t) return flow[t]; 
                    q.enqueue(next.first);
                }
            }
        }

        return 0; 
    }

    long long edmondsKarp(int s, int t) {
        long long flow = 0;
        vector<int> parent(V);
        long long new_flow;

        while ((new_flow = bfs(s, t, parent)) != 0) {
            flow += new_flow;
            int cur = t;

            while (cur != s) {
                int prev = parent[cur];
                adj[prev][cur] -= new_flow;
                adj[cur][prev] += new_flow;
                cur = prev;
            }
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

