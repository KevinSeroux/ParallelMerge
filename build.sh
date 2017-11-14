#!/bin/bash
gcc -Wall -Wextra -g -fopenmp -lm d2p.c stdin.c -o d2p
gcc -Wall -Wextra -g -lm d2s.c stdin.c -o d2s
