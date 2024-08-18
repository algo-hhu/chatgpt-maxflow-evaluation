#include <iostream> 
#include <iomanip> 
#include <chrono>
#include "Dinics.h" 
#include "DinicsStats.h" 
#include "utils.h" 

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <DIMACS file path>" << std::endl;
        return 1;
    }

    std::string file_path = argv[1];
    std::cout << "Reading DIMACS file from: " << file_path << std::endl;
    auto [graph, s, t] = importDIMACS(file_path);

    auto start = std::chrono::high_resolution_clock::now();
    Dinics::Dinics4Skip alg(graph);
    auto max_flow = alg.maxFlow(s, t);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;

    std::cout << "Maximaler Fluss: " << std::fixed << std::setprecision(0) << max_flow << std::endl; 
    std::cout << std::fixed << std::setprecision(6) << "Berechnungsdauer: " << duration.count() << " Sekunden" << std::endl;

    return 0;
}

