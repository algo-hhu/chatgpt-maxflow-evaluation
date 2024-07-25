#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <algorithm>

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

    bool bidirectionalBFS(int s, int t, vector<int>& parentS, vector<int>& parentT, int& meetNode, long long& flow) {
        fill(parentS.begin(), parentS.end(), -1);
        fill(parentT.begin(), parentT.end(), -1);
        parentS[s] = s;
        parentT[t] = t;

        priority_queue<pair<long long, int>, vector<pair<long long, int>>, less<pair<long long, int>>> pqS;
        priority_queue<pair<long long, int>, vector<pair<long long, int>>, less<pair<long long, int>>> pqT;
        pqS.push({LLONG_MAX, s});
        pqT.push({LLONG_MAX, t});

        unordered_map<int, long long> visitedFromS, visitedFromT;
        visitedFromS[s] = LLONG_MAX;
        visitedFromT[t] = LLONG_MAX;

        while (!pqS.empty() && !pqT.empty()) {
            if (!pqS.empty()) {
                int curS = pqS.top().second;
                long long flowS = pqS.top().first;
                pqS.pop();

                for (const auto& next : adj[curS]) {
                    int nextNode = next.first;
                    long long capacity = next.second;

                    if (parentS[nextNode] == -1 && capacity > 0) {
                        parentS[nextNode] = curS;
                        long long newFlow = min(flowS, capacity);
                        if (visitedFromT.count(nextNode)) {
                            meetNode = nextNode;
                            flow = min(newFlow, visitedFromT[nextNode]);
                            return true;
                        }
                        pqS.push({newFlow, nextNode});
                        visitedFromS[nextNode] = newFlow;
                    }
                }
            }

            if (!pqT.empty()) {
                int curT = pqT.top().second;
                long long flowT = pqT.top().first;
                pqT.pop();

                for (const auto& next : adj[curT]) {
                    int nextNode = next.first;
                    long long capacity = next.second;

                    for (const auto& rev : adj[nextNode]) {
                        if (rev.first == curT && rev.second > 0) {
                            if (parentT[nextNode] == -1 && rev.second > 0) {
                                parentT[nextNode] = curT;
                                long long newFlow = min(flowT, rev.second);
                                if (visitedFromS.count(nextNode)) {
                                    meetNode = nextNode;
                                    flow = min(newFlow, visitedFromS[nextNode]);
                                    return true;
                                }
                                pqT.push({newFlow, nextNode});
                                visitedFromT[nextNode] = newFlow;
                            }
                        }
                    }
                }
            }
        }

        return false;
    }

    long long edmondsKarp(int s, int t) {
        long long flow = 0;
        vector<int> parentS(V), parentT(V);
        int meetNode;
        long long new_flow;

        while (bidirectionalBFS(s, t, parentS, parentT, meetNode, new_flow)) {
            flow += new_flow;
            int cur = meetNode;

            vector<int> path;
            path.push_back(cur + 1);

            while (cur != s) {
                int prev = parentS[cur];
                path.push_back(prev + 1);
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

            cur = meetNode;
            while (cur != t) {
                int prev = parentT[cur];
                path.push_back(prev + 1);
                for (auto& edge : adj[prev]) {
                    if (edge.first == cur) {
                        edge.second += new_flow;
                        break;
                    }
                }
                for (auto& edge : adj[cur]) {
                    if (edge.first == prev) {
                        edge.second -= new_flow;
                        break;
                    }
                }
                cur = prev;
            }

            reverse(path.begin(), path.end());
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

