#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <math.h>

#ifndef __APPLE__
#include <bits/stdc++.h>
#endif

#define MAX_INT 2147483647
#define distance(x1, y1, x2, y2) sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))

std::string instance_name;
int dimension, vehicles, actual_vehicles, capacity, totalCost;
int *demand, *route_demand;
int **adjacency, **routes;

struct client {
    int index;
    int demand;
} *clients;

struct point {
    int index;
    int x;
    int y;
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
void simulatedAnelling(double alpha, int iterMax, double tempIni);
void decideMovement(double temp);
void deallocate();

int main(int argc, char** argv) {
    int seed = time(nullptr);
    bool cup = false;
    if(argc > 2) { // Caso queiramos passar a seed como argumento
        seed = atoi(argv[2]);
    }
    if(argc > 3) {
        cup = true;
    }
    srand(seed);
    std::string file_path(argv[1]);
    parseFile(file_path, cup);
    traceRoute();
    if(!cup) {
        std::cout << "INSTANCE: " << instance_name << " TOTAL COST: " << getCost() << std::endl;
    }
    simulatedAnelling(0.95, 1000, 1500);
    if(cup) {
        std::cout << "INSTANCE: " << instance_name << std::endl;
        std::cout << "SEED: " << seed << std::endl;
        std::cout << "ROUTES: " << std::endl;
        showRoutesCup();
        std::cout << "COST: " << getCost() << std::endl;
    } else {
        std::cout << "SEED: " << seed << std::endl;
        std::cout << "INSTANCE: " << instance_name << " TOTAL COST (SIMULATED ANELLING): " << getCost() << std::endl;
        #ifdef DEBUG_ROUTE
        showRoutes();
        #endif
    }
    deallocate();
    return 0;
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
    
    routes = new int*[vehicles + 1];
    routes[vehicles] = new int[vehicles];
    route_demand = new int[vehicles];
    
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
    sort(); // Faz o sort dos clientes em ordem decrescente de demanda
    actual_vehicles = 0;
    for(int v = 0; v < vehicles; v++ ) {
        int vcapacity = 0;
        std::vector<int> vroute;
        vroute.push_back(0);
        
        for(int i = 1; i < dimension ; i++) {
            if(clients[i].demand <= capacity - vcapacity
               && !visited[clients[i].index]) {
                vroute.push_back(clients[i].index);
                visited[clients[i].index] = 1;
                vcapacity += clients[i].demand;
            }
        }
        vroute.push_back(0);
        if(vroute.size() > 2) {
            actual_vehicles++;
        }
        route_demand[v] = vcapacity;
        routes[v] = new int[vroute.size()];
        routes[vehicles][v] = vroute.size();
        std::copy(vroute.begin(),vroute.end(), routes[v]);
        vroute.erase(vroute.begin(), vroute.end());
    }
    
    for(int r = 0; r < vehicles; r++) {
        greedyBestRoute(r);
    }
    
#ifdef DEBUG_ROUTE
    showRoutes();
#endif
}

// Funcao gulosa para melhorar a rota
void greedyBestRoute(int route) {
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
            std::cout << routes[i][j] << " ";
            points[ routes[i][j] ]++;
        }
        std::cout << std::endl;
        std::cout << "Demand " << route_demand[i] << std::endl;
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

// Funcao de apoio para traceRoute que ordena os clientes da maneira desejada
void sort() {
    struct client aux;
    for(int i = 1, j; i < dimension; i++) {
        j = i - 1;
        aux.demand = clients[i].demand;
        aux.index = clients[i].index;
        while(j > 0 && clients[j].demand < aux.demand) {
            clients[j+1].index = clients[j].index;
            clients[j+1].demand = clients[j].demand;
            j--;
        }clients[j+1].demand = aux.demand;
        clients[j+1].demand = aux.demand;
        clients[j+1].index = aux.index;
    }
#ifdef DEBUG_SORT
    debugSort();
#endif
}

// Funcao que calcula o preco de uma rota
int routeCost(int* route, int routeSize) {
    int sum = 0 , i;
    for(i = 0; i < routeSize - 1; i++) {
        sum += adjacency[ route[i] ][ route[i+1] ];
    }
#ifdef DEBUG_COST
    std::cout << " cost: " << sum << std::endl;
#endif
    return sum;
}

// Funcao que calcula o preco de todas as rotas de uma solucao
int getCost() {
    int sum = 0, i;
    for(i = 0; i < vehicles; i++) {
        #ifdef DEBUG_COST
        std::cout << "Route " << i ;
        #endif
        sum += routeCost(routes[i], routes[vehicles][i]);
    }
    return sum;
}

//metaheuristica simulated anelling
void simulatedAnelling(double alpha, int iterMax, double tempIni){
    double temp = tempIni;
    while(temp > 1) {
        for(int i = 0; i < iterMax; i++) {
            decideMovement(temp);
        }
        temp = alpha * temp;
    }
}

//parte interna do simulated anelling
void decideMovement(double temp) {
    //preco antigo
    int cost = getCost();
    
    //calcular nova solucao
    int indexA, indexB, i, j , matches = 0;
    
    indexA = rand() % actual_vehicles;
    indexB = rand() % actual_vehicles;
    
    if(indexA == indexB) {
        do{
            i = (rand()%(routes[vehicles][indexA] - 2)) +1;
            j = (rand()%(routes[vehicles][indexB] - 2)) +1;
        } while(i == j);
        int aux = routes[indexA][i];
        routes[indexA][i] = routes[indexB][j];
        routes[indexB][j] = aux;
    } else {
        int *exchange = new int[routes[vehicles][indexA]];
        for(int k = 0; k < routes[vehicles][indexA]; k++) {
            exchange[k] = 0;
        }
        for(int k = 1; k < routes[vehicles][indexA] - 1; k++) {
            for(int l = 1; l < routes[vehicles][indexB] - 1; l++) {
                if( demand[routes[indexB][l]] <= (demand[routes[indexA][k]] + capacity - route_demand[indexA])
                   && demand[routes[indexA][k]] <= (demand[routes[indexB][l]] + capacity - route_demand[indexB]) ) {
                    exchange[k] = l;
                    matches++;
                    continue;
                }
            }
        }
        if(matches) {
            do {
                i = rand()% routes[vehicles][indexA];
            } while(exchange[i] == 0);
            j = exchange[i];
            delete[] exchange;
            int aux = routes[indexA][i];
            routes[indexA][i] = routes[indexB][j];
            routes[indexB][j] = aux;
            route_demand[indexA] = route_demand[indexA] - demand[routes[indexB][j]] + demand[routes[indexA][i]];
            route_demand[indexB] = route_demand[indexB] - demand[routes[indexA][i]] + demand[routes[indexB][j]];
        }
    }
    
    //novo preco
    int newCost = getCost();
    //decisao de troca
    if(newCost - cost >= 0) {
        double u = ((double) rand() / (RAND_MAX));
        if(u >= exp(-(newCost - cost)/temp)) {
            //reverte movimento
            if(indexA == indexB) {
                int aux = routes[indexA][i];
                routes[indexA][i] = routes[indexB][j];
                routes[indexB][j] = aux;
            } else if(matches) {
                int aux = routes[indexA][i];
                routes[indexA][i] = routes[indexB][j];
                routes[indexB][j] = aux;
                route_demand[indexA] = route_demand[indexA] - demand[routes[indexB][j]] + demand[routes[indexA][i]];
                route_demand[indexB] = route_demand[indexB] - demand[routes[indexA][i]] + demand[routes[indexB][j]];
            }
        }
    }
}

// Funcao que desaloca todos os recursos utilizados durante esse programa
void deallocate() {
    delete[] demand;
    delete[] route_demand;
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