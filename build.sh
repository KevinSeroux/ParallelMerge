#!/bin/bash
gcc -std=c99 -Wall -Wextra -fopenmp d2p.c -o d2p
gcc -std=c99 -Wall -Wextra -fopenmp -DGRAPH d2p.c -o d2p_graph

gcc -std=c99 -Wall -Wextra -fopenmp d2s.c -o d2s
gcc -std=c99 -Wall -Wextra -fopenmp -DGRAPH d2s.c -o d2s_graph
