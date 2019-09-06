#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#ifndef __APPLE__
#include <bits/stdc++.h>
#endif

#define MAX_INT 2147483647

std::string instance_name;
int dimension, vehicles, capacity, totalCost;
int *demand;
int **adjacency, **routes;

struct client {
    int index;
    int demand;
} *clients;

//Declaracao das funcoes
void parseFile(std::string file_path);
void traceRoute();
void sort();
int routeCost(int* route, int routeSize);
int getCost();
void VND();
bool nbhdL1(int* route, int routeSize);
bool nbhdL2(int* route, int routeSize);
bool nbhdL3(int* routeA, int* routeB, int routeASize, int routeBSize);
bool nbhdL4(int* routeA, int* routeB, int RouteASize, int routeBSize);
//void deallocate();


int main(int argc, char** argv) {
    std::string file_path(argv[1]);
    parseFile(file_path);
    traceRoute();
    std::cout << "INSTANCE: " << instance_name << " TOTAL COST: " << getCost() << std::endl;
    VND();
    std::cout << "INSTANCE: " << instance_name << " TOTAL COST (VND): " << getCost() << std::endl;
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
    clients = new client[dimension];
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
        file >> clients[i].index; // Index
        file >> clients[i].demand;
    }
    file >> aux; // EDGE_WEIGHT_SECTION
    for(int i = 0; i < dimension; i++) {
        for(int j = 0; j < dimension; j++) {
            file >> adjacency[i][j];
        }
    }
    file.close();
#ifdef DEBUG_PARSER
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
        demand[v] = vcapacity;
        routes[v] = new int[vroute.size()];
        routes[vehicles][v] = vroute.size();
        std::copy(vroute.begin(),vroute.end(), routes[v]);
        vroute.erase(vroute.begin(), vroute.end());
    }

    for(int r = 0; r < vehicles; r++) {
        for(int i = 0; i < routes[vehicles][r]-2; i++) {
            int smallest = i+1;
            for(int j = i+2; j < routes[vehicles][r]-1; j++) {
                if(adjacency[ routes[r][i] ][ routes[r][j] ] <= adjacency[ routes[r][i] ][ routes[r][smallest] ]) {
                    smallest = j;
                }
            }
            int aux = routes[r][i+1];
            routes[r][i+1] = routes[r][smallest];
            routes[r][smallest] = aux;
        }
    }
    #ifdef DEBUG_ROUTE
        int *points = new int[dimension];

        std::cout << "Instance " << instance_name << " Capacity " << capacity << std::endl;
        for(int i = 0; i < vehicles; i++){
            std::cout << "Route " << i+1 << ": ";
            for(int j = 0; j < routes[vehicles][i]; j++){
                std::cout << routes[i][j] << " | ";
                points[ routes[i][j] ]++;
            }
            std::cout << std::endl;
            std::cout << "Demand " << demand[i] << std::endl;
        }

        for(int i = 0; i < dimension; i++){
            if(!points[i])
            std::cout << "Missing " << i << std::endl;
        }

            delete[] points;
    #endif
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
        for(int i = 0; i < dimension; i++) {
            std::cout << clients[i].index << " " << clients[i].demand << std::endl;
        }
    #endif
}

// Funcao que calcula o prece de uma rota
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

// Funcao que calcula o preco de todas as rotas de uam solucao
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
        std::cout << "newTry" << std::endl;
        while(k <= 4){
            bool result = false;
            switch (k) {
            case 1:
                i = rand() % vehicles;
                result = nbhdL2(routes[i],routes[vehicles][i]);
                break;
            case 2:
                i = rand() % vehicles;
                result = nbhdL2(routes[i],routes[vehicles][i]);
                break;
            case 3:
                i = rand() % vehicles;
                j = rand() % vehicles;
                result = nbhdL3(routes[i], routes[j],routes[vehicles][i], routes[vehicles][i]);
                break;
            case 4:
                i = rand() % vehicles;
                j = rand() % vehicles;
                result = nbhdL4(routes[i], routes[j],routes[vehicles][i], routes[vehicles][i]);
                break;
            }
            if(result)
                k = 1;
            else
                k++;
        }

        
    }
}
 
 // Funcao que cria um vizinho do nivel N1 aleatorio e calcula se esse vizinho
 // tem um custo menor que a antiga solucao
bool nbhdL1(int* route, int routeSize) {
    int formerCost = routeCost(route, routeSize);
    int i = (rand()%(routeSize - 1)) + 1, aux;
    aux = route[i];
    route[i] = route[i+1];
    route[i+1] = aux;
 
    if( routeCost(route, routeSize) < formerCost ) {
        return true;
    } else {
        aux = route[i];
        route[i] = route[i+1];
        route[i+1] = aux;
        return false;
    }
   
}
 
 // Funcao que cria um vizinho do nivel N2 aleatorio e calcula se esse vizinho
 // tem um custo menor que a antiga solucao
bool nbhdL2(int* route, int routeSize) {
    int formerCost = routeCost(route, routeSize);
    int i = 0, j = 0, aux;
    while(i == j) {
        i = (rand()%(routeSize -1)) +1;
        j = (rand()%(routeSize -1)) +1;
    }
   
    aux = route[i];
    route[i] = route[j];
    route[j] = aux;    
 
    if( routeCost(route, routeSize) < formerCost ) {
        return true;
    } else {
        aux = route[i];
        route[i] = route[i+1];
        route[i+1] = aux;
        return false;
    }
}
 
 // Funcao que cria um par vizinhos do nivel N3 aleatorio e calcula se esses vizinhos
 // tem custos menores que as antigas solucoes
bool nbhdL3(int* routeA, int* routeB, int routeASize, int routeBSize) {
    int costA = routeCost(routeA, routeASize), costB = routeCost(routeB, routeBSize), biggest, aux, i, j;;
    int exchange[routeASize];
    #ifndef __APPLE__
    for(i = 0; i < routeASize; i++) {
        exchange[i] = 0;
    }
    #endif
    for(i = 1; i < routeASize-1; i++) {
        biggest = -1;
        for(j = 1; j < routeBSize-1; j++) {
            if(routeB[j] < routeA[i] + (capacity - demand[j]) && routeB[j] > routeB[biggest]) {
                biggest = j;
            }
        }
        exchange[i] = biggest;
    }
   
    do {
        biggest = rand()% routeASize;
    } while(exchange[biggest] == 0);
   
    aux = routeA[i];
    routeA[i] = routeB[exchange[biggest]];
    routeB[exchange[biggest]] = aux;
 
    if( routeCost(routeA, routeASize) + routeCost(routeB, routeBSize) < costA + costB ) {
        return true;
    } else {
        aux = routeA[i];
        routeA[i] = routeB[exchange[biggest]];
        routeB[exchange[biggest]] = aux;
        return false;
    }
 
}
 
 // Funcao que cria um par vizinhos do nivel N4 aleatorio e calcula se esses vizinhos
 // tem custos menores que as antigas solucoes
bool nbhdL4(int* routeA, int* routeB, int RouteASize, int routeBSize) {
    int iab, iae, ibb, ibe;
 
    return true;
}

// Funcao que desaloca todos os recursos utilizados durante esse programa
/*void deallocate() {
    delete [] demand;
    for(int i = 0; i < dimension; i++) {
        delete [] adjacency[i];
        delete [] routes[i];
    }
}*/

