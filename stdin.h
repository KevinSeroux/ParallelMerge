#ifndef STDIN__H
#define STDIN__H

#include <stdbool.h>

bool allocIntFromStdin(int* pSize, int** pInt);
void freeInt(int integers[]);

#endif

