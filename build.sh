#!/bin/bash
gcc -std=c99 -Wall -Wextra -g -fopenmp -lm d2p.c stdin.c -o d2p
gcc -std=c99 -Wall -Wextra -g -fopenmp -lm -DGRAPH d2p.c stdin.c -o d2p_graph

gcc -std=c99 -Wall -Wextra -g -fopenmp -lm d2s.c stdin.c -o d2s
gcc -std=c99 -Wall -Wextra -g -fopenmp -lm d2s.c -DGRAPH stdin.c -o d2s_graph
