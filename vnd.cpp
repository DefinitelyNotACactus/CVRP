#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <chrono>
#include <cmath>

#ifndef __APPLE__
#include <bits/stdc++.h>
#endif

#define MAX_INT 2147483647
#define distance(x1, y1, x2, y2) sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))

std::string instance_name;
int dimension, vehicles, actual_vehicles, capacity, totalCost;
int *demand;
int **adjacency;
vector<route> routes;

struct client {
    int index;
    int demand;
    
    bool operator <(const client &c) const { // Comparar clientes pela demanda
        return demand < c.demand;
    }
} *clients;

struct point {
    int index;
    int x;
    int y;
};

struct route {
    vector<int> clients;
    int demand;
    int cost;
};

#ifdef DEBUG_PARSER
void debugParser() {
    for(int i = 0; i < dimension; i++) {
        std::cout << clients[i].demand << std::endl;
    }
    for(int i = 0; i < dimension; i++) {
        for(int j = 0; j < dimension; j++) {
            std::cout << adjacency[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
#endif

#ifdef DEBUG_SORT
void debugSort(){
    for(int i = 0; i < dimension; i++) {
        std::cout << clients[i].index << " " << clients[i].demand << std::endl;
    }
}
#endif

//Declaracao das funcoes
void parseFile(std::string file_path, bool cup);
void traceRoute();
void greedyBestRoute(int route);
void showRoutes();
void showRoutesCup();
void sort();
int routeCost(int* route, int routeSize);
int getCost();
void VND();
bool nbhdL1(int* route, int routeSize);
bool nbhdL2(int* routeA, int* routeB, int routeASize, int routeBSize, int indexA, int indexB);
bool nbhdL3(int* routeA, int* routeB, int RouteASize, int routeBSize, int indexA, int indexB);
void deallocate();
void buildSolution(int seed, std::string input, bool cup, int iterations);
int getOptimal();

int main(int argc, char** argv) {
    int seed, c, iterations;
    if(argc < 4) {
        std::cout << "Please use ./<executable name> <input file> <cvrp-cup (0 if cup)> <iterations> <seed (optional)>" << std::endl;
        exit(-1);
    }
    std::string file_path(argv[1]);
    c = atoi(argv[2]);
    iterations = atoi(argv[3]);
    if(argc > 4) { // Caso queiramos passar a seed como argumento
        seed = atoi(argv[4]);
    } else {
        seed = 0;
    }
    buildSolution(seed, file_path, (c == 0), iterations);
    return 0;
}

void buildSolution(int seed, std::string input, bool cup, int iterations) {
    bool s = (seed == 0);
    int current_seed = seed, best_seed, heuristic_cost, current_cost, best_cost = MAX_INT;
    std::chrono::duration<float> sum;
    if(!s) {
        srand(current_seed);
    }
    for(int i = 0; i < iterations; i++) {
        auto start = std::chrono::steady_clock::now();
        if(s) {
            current_seed = time(nullptr);
            srand(current_seed);
        }
        parseFile(input, cup);
        traceRoute();
        if(!cup && !i) {
            heuristic_cost = getCost();
        }
        VND();
        current_cost = getCost();
        if(cup) {
            std::cout << "INSTANCE: " << instance_name << std::endl;
            std::cout << "SEED: " << current_seed << std::endl;
            std::cout << "ROUTES: " << std::endl;
            showRoutesCup();
            std::cout << "COST: " << current_cost << std::endl;
        } else {
            #ifdef SHOW_ITERATION
            std::cout << "SEED: " << current_seed << std::endl;
            std::cout << "INSTANCE: " << instance_name << " TOTAL COST (SIMULATED ANEALLING): " << current_cost << std::endl;
            #endif
            #ifdef DEBUG_ROUTE
            showRoutes();
            #endif
        }
        if(current_cost < best_cost) {
            best_cost = current_cost;
            best_seed = current_seed;
        }
        deallocate();
        auto end = std::chrono::steady_clock::now();
        if(!i) {
            sum = (end - start);
        } else {
            sum += (end - start);
        }
    }
    
    if(iterations >= 1) {
        int opt = getOptimal();
        double gap;
        std::cout << "INSTANCE: " << instance_name << std::endl;
        std::cout << "OPTIMAL COST: " << opt << std::endl;
        std::cout << "TOTAL COST (HEURISTIC): " << heuristic_cost << std::endl;
        if(!cup) {
            gap = ((heuristic_cost - opt) / (double) opt) * 100;
            std::cout << "GAP (HEURISTIC): " << gap << std::endl;
        }
        #ifdef DEBUG_SEED
        std::cout << "BEST SEED: " << best_seed << std::endl;
        #endif
        std::cout << "BEST COST: " << best_cost << std::endl;
        std::cout << "AVERAGE TIME: " << sum.count() / iterations << " secs (Iterations = " << iterations << ")" << std::endl;
        if(!cup) {
            gap = ((best_cost - opt) / (double) opt) * 100;
            std::cout << "GAP (SIMULATED ANNEALING): " << gap << std::endl;
        }
    }
}

// Funcao hardcoded para retornar o valor das solucoes otimas
int getOptimal() {
    if(instance_name.compare("P-n16-k8") == 0) {
        return 450;
    } else if(instance_name.compare("P-n19-k2") == 0) {
        return 212;
    } else if(instance_name.compare("P-n20-k2") == 0) {
        return 216;
    } else if(instance_name.compare("P-n23-k8") == 0) {
        return 529;
    } else if(instance_name.compare("P-n55-k7") == 0) {
        return 510;
    } else if(instance_name.compare("P-n51-k10") == 0) {
        return 696;
    } else if(instance_name.compare("P-n50-k10") == 0) {
        return 741;
    } else if(instance_name.compare("P-n45-k5") == 0) {
        return 568;
    }
    return -1;
}

// Funcao para fazer o parsing do arquivo de entrada
void parseFile(std::string file_path, bool cup) {
    std::ifstream file(file_path);
    std::string aux;
    if(!file.is_open()) {
        std::cout << "Error while opening file: " << file_path << std::endl;
        exit(1);
    }
    file >> aux; // NAME:
    file >> instance_name;
    file >> aux; // DIMENSION:
    file >> dimension;
    // Aloca os vetores de demanda e adjacencia
    clients = new client[dimension];
    demand = new int[dimension];
    adjacency = new int*[dimension];
    totalCost = 0;
    for(int i = 0; i < dimension; i++) {
        adjacency[i] = new int[dimension];
    }
    file >> aux; // VEHICLES:
    file >> vehicles;
    
    for(int i = 0; i < vehicles; i++) {
        routes.push_back(route());
    }
    
    file >> aux; // CAPACITY:
    file >> capacity;
    file >> aux; // DEMAND_SECTION:
    for(int i = 0; i < dimension; i++) {
        file >> clients[i].index; // Index
        file >> clients[i].demand;
        demand[i] = clients[i].demand;
    }
    
    file >> aux; // NODE_COORD_SECTION / EDGE_WEIGHT_SECTION
    if(cup) {
        struct point *points = new point[dimension];
        for(int i = 0; i < dimension; i++) {
            file >> points[i].index >> points[i].x >> points[i].y;
        }
        for(int i = 0; i < dimension; i++) {
            for(int j = i; j < dimension; j++) {
                int d = distance(points[i].x, points[i].y, points[j].x, points[j].y);
                adjacency[i][j] = d;
                adjacency[j][i] = d;
            }
        }
        delete[] points;
    } else {
        for(int i = 0; i < dimension; i++) {
            for(int j = 0; j < dimension; j++) {
                file >> adjacency[i][j];
            }
        }
    }
    file.close();
#ifdef DEBUG_PARSER
    debugParser();
#endif
}


// Funcao para criar as rotas validas iniciais
void traceRoute() {
    int* visited = new int[dimension];
    for(int i = 0; i < dimension; i++) {
        visited[i] = 0;
    }
    std::sort(clients, clients + dimension); // Faz o sort dos clientes em ordem decrescente de demanda
    actual_vehicles = 0;
    for(int v = 0; v < vehicles; v++ ) {
        int vcapacity = 0;
        std::vector<int> vroute;
        routes[v].clients.push_back(0);
        
        for(int i = 1; i < dimension ; i++) {
            if(clients[i].demand <= capacity - vcapacity
               && !visited[clients[i].index]) {
                routes[v].clients.push_back(clients[i].index);
                visited[clients[i].index] = 1;
                vcapacity += clients[i].demand;
            }
        }
        routes[v].clients.push_back(0);
        
        if(routes[v].clients.size() > 2) {
            actual_vehicles++;
        }
        routes[v].demand = vcapacity;
    }
    
    for(int r = 0; r < vehicles; r++) {
        greedyBestRoute(r);
    }
    
#ifdef DEBUG_ROUTE
    showRoutes();
#endif
}

// Insercao mais barata
void greedyBestRoute(int route) {
    // Subrota inicial usando o primeiro do vetor de candidatos
    s.push_back(candidates.at(0));
    candidates.erase(candidates.begin());
    s.push_back(1);
    // Insercao na subrota
    int pi, pj, pk;
    double skij;
    while(candidates.size() > 0) {
        pi = 0, pj = 1, pk = 0;
        for(int i = 0; i < s.size() - 1; i++) {
            for(int j = 1; j < s.size(); j++) {
                skij = DBL_MAX;
                for(int k = 0; k < candidates.size(); k++) {
                    if(matrizAdj[s.at(i)][candidates.at(k)] + matrizAdj[candidates.at(0)][s.at(j)] - matrizAdj[s.at(i)][s.at(j)] < skij) {
                        skij = matrizAdj[s.at(i)][candidates.at(k)] + matrizAdj[candidates.at(0)][s.at(j)] - matrizAdj[s.at(i)][s.at(j)];
                        pi = i;
                        pj = j;
                        pk = k;
                    }
                }
            }
        }
        s.insert(s.begin() + pi, candidates.at(pk));
        candidates.erase(candidates.begin() + pk);
    }
    for(int i = 0; i < routes[vehicles][route]-2; i++) {
        int smallest = i+1;
        for(int j = i+2; j < routes[vehicles][route]-1; j++) {
            if(adjacency[ routes[route][i] ][ routes[route][j] ] <= adjacency[ routes[route][i] ][ routes[route][smallest] ]) {
                smallest = j;
            }
        }
        int aux = routes[route][i+1];
        routes[route][i+1] = routes[route][smallest];
        routes[route][smallest] = aux;
    }
}

//imprime as rotas e suas demandas
void showRoutes(){
    int *points = new int[dimension];
    
    std::cout << "Instance " << instance_name << " Capacity " << capacity << std::endl;
    for(int i = 0; i < vehicles; i++){
        std::cout << "Route " << i+1 << ": ";
        for(int j = 0; j < routes[vehicles][i]; j++){
            std::cout << routes[i][j] << " | ";
            points[ routes[i][j] ]++;
        }
        std::cout << std::endl;
        std::cout << "Demand " << routes[i].demand << std::endl;
    }
    
    for(int i = 0; i < dimension; i++){
        if(!points[i])
            std::cout << "Missing " << i << std::endl;
    }
    
    delete[] points;
}

// Imprime as rotas no formato da cvrp cup
void showRoutesCup() {
    std::cout << routes[0][0];
    for(int j = 1; j < routes[vehicles][0]; j++){
        std::cout << ", " << routes[0][j];
    }
    for(int i = 1; i < actual_vehicles; i++){
        std::cout << "; " << routes[i][0];
        for(int j = 1; j < routes[vehicles][i]; j++){
            std::cout << ", " << routes[i][j];
        }
    }
    std::cout << std::endl;
}

// Funcao que calcula o preco de uma rota
int routeCost(int* route, int routeSize) {
    int sum = 0 , i;
    for(i = 0; i < routeSize -1; i++) {
        sum+= adjacency[ route[i] ][ route[i+1] ];
    }
#ifdef DEBUG_COST
    std::cout << "Route " << i << " cost: " << sum << std::endl;
#endif
    return sum;
}

// Funcao que calcula o preco de todas as rotas de uma solucao
int getCost() {
    int sum = 0, i;
    for(i = 0; i < vehicles; i++) {
        sum += routeCost(routes[i], routes[vehicles][i]);
    }
    return sum;
}

//Algoritmo VND
void VND(){
    int i = 0, j = 0,k = 0;
    bool improvemment = true;
    while(improvemment){
        k = 1;
        improvemment = false;
        while(k <= 3){
            int formerCost = getCost();
            i = j = 0;
            switch (k) {
                case 1:
                    for(int i = 0; i < vehicles; i++) {
                        //nbhdL1(routes[i],routes[vehicles][i]);
                    }
                    break;
                case 2:
                    do {
                        i = rand()%vehicles;
                        j = rand()%vehicles;
                    }while(i == j);
                    for(int l = 0; l < vehicles ; i = (i+1)%vehicles , j = (j+1)%vehicles, l++) {
                        //nbhdL2(routes[i], routes[j],routes[vehicles][i], routes[vehicles][j], i, j);
                    }
                    break;
                case 3:
                    do {
                        i = rand()%vehicles;
                        j = rand()%vehicles;
                    }while(i == j);
                    for(int l = 0; l < vehicles ; i = (i+1)%vehicles , j = (j+1)%vehicles, l++) {
                        //nbhdL3(routes[i], routes[j],routes[vehicles][i], routes[vehicles][j], i, j);
                    }
                    break;
            }
#ifdef DEBUG_VND
            std::cout << "VND new cost " << getCost() << " k = " << k << std::endl;
#ifdef DEBUG_ROUTE
            showRoutes();
#endif
#endif
            if(getCost() < formerCost) {
                improvemment = true;
                k = 1;
            } else {
                k++;
            }
        }
    }
}

// Funcao que desaloca todos os recursos utilizados durante esse programa
void deallocate() {
    delete[] demand;
    delete[] clients;
    for(int i = 0; i < dimension; i++) {
        delete[] adjacency[i];
    }
    for(int i = 0; i < vehicles+1; i++) {
        delete[] routes[i];
    }
    delete[] adjacency;
    delete[] routes;
}
