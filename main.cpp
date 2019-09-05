#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bits/stdc++.h>

#define MAX_INT 2147483647

std::string instance_name;
int dimension, vehicles, capacity, totalCost;
int *demand;
int **adjacency, **routes;

void parseFile(std::string file_path);
void traceRoute();
int getCost();
//void deallocate();

int main(int argc, char** argv) {
    std::string file_path(argv[1]);
    parseFile(file_path);
    traceRoute();
    std::cout << "INSTANCE: " << instance_name << "TOTAL COST " << getCost() << std::endl;
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
    totalCost = 0;
    for(int i = 0; i < dimension; i++) {
        adjacency[i] = new int[dimension];
    }
    file >> aux; // VEHICLES:
    file >> vehicles;

    routes = new int*[vehicles+1];
    routes[vehicles] = new int[vehicles];
    
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
    file.close();
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
    std::cout << std::endl;
#endif
   
}

void traceRoute() {
    std::vector<int> visited;
    
    for(int v = 0; v < vehicles; v++) {
        int vcapacity = 0, currentClient = 0;
        
        std::vector<int> vroute;
        vroute.push_back(0);

        while(vcapacity < capacity) {
        
            int smallest = MAX_INT, i;
            for(i = 1; i < dimension; i++) {
                if( i != currentClient 
                    && adjacency[currentClient][i] < smallest 
                    && (demand[i]+vcapacity) < capacity 
                    && std::find(visited.begin(), visited.end(), i) == visited.end() ) {
                    
                        smallest = i;
                }
            }     

            if(smallest != MAX_INT) {
                visited.push_back(smallest);
                vroute.push_back(smallest);
                vcapacity += demand[smallest];
                currentClient = smallest;
            } else{
                vroute.push_back(0);
                break;
            }
        }

        routes[v] = new int[vroute.size()];
        routes[vehicles][v] = vroute.size();
        std::copy(vroute.begin(),vroute.end(), routes[v]);
        vroute.erase(vroute.begin(), vroute.end());

    }
#ifdef DEBUG
    for(int i = 0; i < vehicles; i++){
        std::cout << "Route " << i+1 << ": ";
        for(int j = 0; j < routes[vehicles][i]; j++){
            std::cout << routes[i][j] << " | ";
        }
        std::cout << std::endl;
    }
#endif
}

int getCost() {
    int sum = 0;
    for(int i = 0; i < vehicles; i++) {
        for(int j = 0; j < routes[vehicles][i]-1; j++) {
            sum += adjacency[ routes[i][j] ][ routes[i][j+1] ];
        }
    }
    return sum;
}

/*void deallocate() {
    delete [] demand;
    for(int i = 0; i < dimension; i++) {
        delete [] adjacency[i];
        delete [] routes[i];
    }
}*/
