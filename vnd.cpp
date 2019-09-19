#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>

#ifndef __APPLE__
#include <bits/stdc++.h>
#endif

#define MAX_INT 2147483647

std::string instance_name;
int dimension, vehicles, capacity, totalCost;
int *demand, *route_demand;
int **adjacency, **routes;

struct client {
    int index;
    int demand;
} *clients;

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
void parseFile(std::string file_path);
void traceRoute();
void greedyBestRoute(int route);
void showRoutes();
void sort();
int routeCost(int* route, int routeSize);
int getCost();
void VND();
bool nbhdL1(int* route, int routeSize);
bool nbhdL2(int* routeA, int* routeB, int routeASize, int routeBSize, int indexA, int indexB);
bool nbhdL3(int* routeA, int* routeB, int RouteASize, int routeBSize, int indexA, int indexB);
void deallocate();


int main(int argc, char** argv) {
    int seed = time(nullptr);
    if(argc > 2) { // Caso queiramos passar a seed como argumento
        seed = atoi(argv[2]);
    }
    srand(seed);
    std::cout << "Seed  = " << seed << std::endl;
    std::string file_path(argv[1]);
    parseFile(file_path);
    traceRoute();
    std::cout << "INSTANCE: " << instance_name << " TOTAL COST: " << getCost() << std::endl;
    VND();
    std::cout << "INSTANCE: " << instance_name << " TOTAL COST (VND): " << getCost() << std::endl;
    showRoutes();
    deallocate();
    return 0;
}


// Funcao para fazer o parsing do arquivo de entrada
void parseFile(std::string file_path) {
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
    file >> aux; // EDGE_WEIGHT_SECTION
    for(int i = 0; i < dimension; i++) {
        for(int j = 0; j < dimension; j++) {
            file >> adjacency[i][j];
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
            std::cout << routes[i][j] << " | ";
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
                        nbhdL1(routes[i],routes[vehicles][i]);
                    }
                    break;
                case 2:
                    do {
                        i = rand()%vehicles;
                        j = rand()%vehicles;
                    }while(i == j);
                    for(int l = 0; l < vehicles ; i = (i+1)%vehicles , j = (j+1)%vehicles, l++) {
                        nbhdL2(routes[i], routes[j],routes[vehicles][i], routes[vehicles][j], i, j);
                    }
                    break;
                case 3:
                    do {
                        i = rand()%vehicles;
                        j = rand()%vehicles;
                    }while(i == j);
                    for(int l = 0; l < vehicles ; i = (i+1)%vehicles , j = (j+1)%vehicles, l++) {
                        nbhdL3(routes[i], routes[j],routes[vehicles][i], routes[vehicles][j], i, j);
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

// Funcao que cria um vizinho do nivel N1 aleatorio e calcula se esse vizinho
// tem um custo menor que a antiga solucao
bool nbhdL1(int* route, int routeSize) {
#ifdef DEBUG_VND
    std::cout << "Called nbhdL1" << std::endl;
#endif
    int formerCost = routeCost(route, routeSize);
    int i = (rand()%(routeSize - 3)) + 1, aux;
    aux = route[i];
    route[i] = route[i+1];
    route[i+1] = aux;
    
    if( routeCost(route, routeSize) < formerCost ) {
#ifdef DEBUG_VND
        std::cout << "New cost " << routeCost(route, routeSize) << std::endl;
        std::cout << "Old cost " << formerCost << std::endl;
#endif
        return true;
    } else {
        aux = route[i];
        route[i] = route[i+1];
        route[i+1] = aux;
        return false;
    }
    
}

// Funcao que cria um par vizinhos do nivel N2 aleatorio e calcula se esses vizinhos
// tem custos menores que as antigas solucoes
bool nbhdL2(int* routeA, int* routeB, int routeASize, int routeBSize, int indexA, int indexB) {
#ifdef DEBUG_VND
    std::cout << "Called nbhdL2" << std::endl;
#endif
    int costA = routeCost(routeA, routeASize), costB = routeCost(routeB, routeBSize);
    int *exchange = new int[routeASize], matches = 0;
    for(int i = 0; i < routeASize; i++) {
        exchange[i] = 0;
    }
    for(int i = 1; i < routeASize - 1; i++) {
        for(int j = 1; j < routeBSize - 1; j++) {
            if( demand[routeB[j]] <= (demand[routeA[i]] + capacity - route_demand[indexA])
               && demand[routeA[i]] <= (demand[routeB[j]] + capacity - route_demand[indexB]) ) {
                exchange[i] = j;
                matches++;
                continue;
            }
        }
    }
    int r = 0;
    if(matches) {
        do {
            r = rand()% routeASize;
        } while(exchange[r] == 0 );
    } else {
        return false;
    }
    
#ifdef DEBUG_VND
    std::cout << "Old Demand A= " << route_demand[indexA] << " Old Demand B= " << route_demand[indexB] << std::endl;
#endif
    int aux = routeA[r], aux2 = route_demand[indexA];
    routeA[r] = routeB[exchange[r]];
    routeB[exchange[r]] = aux;
    
    route_demand[indexA] = route_demand[indexA] - demand[routeB[exchange[r]]] + demand[routeA[r]];
    route_demand[indexB] = route_demand[indexB] - demand[routeA[r]] + demand[routeB[exchange[r]]];
    
    if(route_demand[indexA] <= capacity && route_demand[indexB] <= capacity && routeCost(routeA, routeASize) + routeCost(routeB, routeBSize) <= costA + costB ) {
        nbhdL1(routes[indexA],routes[vehicles][indexA]);
        nbhdL1(routes[indexB],routes[vehicles][indexB]);
#ifdef DEBUG_VND
        std::cout << "New Demand A= " << route_demand[indexA] << " New Demand B= " << route_demand[indexB] << std::endl;
        std::cout << "Old cost A= " << costA << " Old cost B= " << costB << std::endl;
        std::cout << "New cost A= " << routeCost(routeA, routeASize) << " New cost B= " << routeCost(routeB, routeBSize) << std::endl;
#endif
        return true;
    } else {
        aux = routeA[r];
        aux2 = route_demand[indexA];
        routeA[r] = routeB[exchange[r]];
        routeB[exchange[r]] = aux;
        
        route_demand[indexA] = route_demand[indexA] - demand[routeB[exchange[r]]] + demand[routeA[r]];
        route_demand[indexB] = route_demand[indexB] - demand[routeA[r]] + demand[routeB[exchange[r]]];
        return false;
    }
}

// Troca 2 clientes de uma rota A por 2 de uma rota B (ou vice-versa)
bool nbhdL3(int* routeA, int* routeB, int routeASize, int routeBSize, int indexA, int indexB) {
#ifdef DEBUG_VND
    std::cout << "Called nbhdL3" << std::endl;
#endif
    int costA = routeCost(routeA, routeASize), costB = routeCost(routeB, routeBSize);
    int target1A = 0, target2A = 0, target1B = 0, target2B;
    while(target1A == target2A) {
        target1A = (rand()%(routeASize -2)) +1;
        target2A = (rand()%(routeASize -2)) +1;
        target1B = (rand()%(routeBSize -2)) +1;
    }
    int dA = demand[routeA[target1A]] + demand[routeA[target2A]], dB = demand[routeB[target1B]];
    // Procura um B para trocar
    int i;
    for(i = 1; i < routeBSize - 1; i++) {
        if(demand[routeB[i]]+dB >= dA && (route_demand[indexA] - dA + dB + demand[routeB[i]]) <= capacity && i != target1B) {
            target2B = i;
            break;
        }
    }
    if(i == routeBSize - 1) {
        return false;
    }
#ifdef DEBUG_VND
    std::cout << "Old Demand A= " << route_demand[indexA] << " Old Demand B= " << route_demand[indexB] << std::endl;
#endif
    int aux = routeA[target1A], aux2 = route_demand[indexA];
    routeA[target1A] = routeB[target1B];
    routeB[target1B] = aux;
    aux = routeA[target2A];
    routeA[target2A] = routeB[target2B];
    routeB[target2B] = aux;
    
    route_demand[indexA] = route_demand[indexA] - demand[routeB[target1B]] - demand[routeB[target2B]] + demand[routeA[target1A]] + demand[routeA[target2A]];
    route_demand[indexB] = route_demand[indexB] - demand[routeA[target1A]] - demand[routeA[target2A]] + demand[routeB[target1B]] + demand[routeB[target2B]];
    
    if(route_demand[indexA] <= capacity && route_demand[indexB] <= capacity && routeCost(routeA, routeASize) + routeCost(routeB, routeBSize) <= costA + costB ) {
        nbhdL1(routes[indexA],routes[vehicles][indexA]);
        nbhdL1(routes[indexB],routes[vehicles][indexB]);
#ifdef DEBUG_VND
        std::cout << "New Demand A= " << route_demand[indexA] << " New Demand B= " << route_demand[indexB] << std::endl;
        std::cout << "Old cost A= " << costA << " Old cost B= " << costB << std::endl;
        std::cout << "New cost A= " << routeCost(routeA, routeASize) << " New cost B= " << routeCost(routeB, routeBSize) << std::endl;
#endif
        return true;
    } else {
        aux = routeA[target1A];
        aux2 = route_demand[indexA];
        routeA[target1A] = routeB[target1B];
        routeB[target1B] = aux;
        aux = routeA[target2A];
        routeA[target2A] = routeB[target2B];
        routeB[target2B] = aux;
        
        route_demand[indexA] = route_demand[indexA] - demand[routeB[target1B]] - demand[routeB[target2B]] + demand[routeA[target1A]] + demand[routeA[target2A]];
        route_demand[indexB] = route_demand[indexB] - demand[routeA[target1A]] - demand[routeA[target2A]] + demand[routeB[target1B]] + demand[routeB[target2B]];
        return false;
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
