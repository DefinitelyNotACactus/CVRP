# CVRP
Capacitated Vehicle Routing Problem

## Compilação
```
$ g++ simannealing.cpp -o <nome executavel> -std=c++11
```

## Execução
```
$ ./<nome executavel> <arquivo entrada> <cvrp-cup (0 se for)> <num iteracoes> <seed (opcional)>
```

## Comparações 
| Instância | Solução ótima | Custo (Heurística) | Gap (Heurística) | Melhor Custo (Simulated Annealing) | Gap (Simulated Annealing) | Tempo médio de execução (segundos) |
|-----------|---------------|--------------------|------------------|------------------------------------|---------------------------|------------------------------------|
| P-n19-k2  | 212           | 289                | 36.3208          | 249                                | 17.4528                   | 0.405141                           |
| P-n20-k2  | 216           | 296                | 37.037           | 250                                | 15.7407                   | 0.439668                           |
| P-n23-k8  | 529           | 689                | 30.2457          | 615                                | 16.2571                   | 0.335632                           |
| P-n45-k5  | 568           | 1057               | 86.0915          | 547                                | -3.69718                  | 0.88859                            |
| P-n50-k10 | 741           | 1290               | 74.0891          | 711                                | -4.04858                  | 0.66249                            |
| P-n51-k10 | 696           | 1464               | 110.345          | 813                                | 16.8103                   | 0.645988                           |
| P-n55-k7  | 510           | 1074               | 110.588          | 607                                | 19.0196                   | 0.840223                           |
