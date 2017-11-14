#!/bin/bash
gcc -Wall -Wextra -g -fopenmp -lm d2p.c stdin.c -o d2p
gcc -Wall -Wextra -g -fopenmp -lm -DGRAPH d2p.c stdin.c -o d2p_graph

gcc -Wall -Wextra -g -lm d2s.c stdin.c -o d2s
gcc -Wall -Wextra -g -lm d2s.c -DGRAPH stdin.c -o d2s_graph

