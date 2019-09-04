#include <iostream>
#include <fstream>

std::string instance_name;
int dimension, vehicles, capacity;

int *demand;
int **adjacency;

void parseFile(std::string file_path);

int main(int argc, char** argv) {
    std::string file_path("./instancias_teste/P-n19-k2.txt");
    parseFile(file_path);
    delete [] demand;
    for(int i = 0; i < dimension; i++) {
        delete [] adjacency[i];
    }
    return 0;
}

// Funcao para fazer o parsing do arquivo de entrada
void parseFile(std::string file_path) {
    std::ifstream file(file_path);
    std::string aux;
    
    file >> aux; // NAME:
    file >> instance_name;
    file >> aux; // DIMENSION:
    file >> dimension;
    // Aloca os vetores de demanda e adjacencia
    demand = new int[dimension];
    adjacency = new int*[dimension];
    for(int i = 0; i < dimension; i++) {
        adjacency[i] = new int[dimension];
    }
    file >> aux; // VEHICLES:
    file >> vehicles;
    file >> aux; // CAPACITY:
    file >> capacity;
    file >> aux; // DEMAND_SECTION:
    for(int i = 0; i < dimension; i++) {
        file >> aux; // Index
        file >> demand[i];
    }
    file >> aux; // EDGE_WEIGHT_SECTION
    for(int i = 0; i < dimension; i++) {
        for(int j = 0; j < dimension; j++) {
            file >> adjacency[i][j];
        }
    }
#ifdef DEBUG
    for(int i = 0; i < dimension; i++) {
        std::cout << demand[i] << std::endl;
    }
    for(int i = 0; i < dimension; i++) {
        for(int j = 0; j < dimension; j++) {
            std::cout << adjacency[i][j] << " ";
        }
        std::cout << std::endl;
    }
#endif
}
