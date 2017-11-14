#include "stdin.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <assert.h>
#include <omp.h>
#include <unistd.h>

//#define GRAPH

void print(int size, int const integers[])
{
	for(int i = 0; i < size; i++)
		printf("%d ", integers[i]);

	printf("\n");
}

bool init(int *pSize, int **pIn, int **pOut)
{
	//omp_set_num_threads(8);

	int size;
	int* in = 0;
	if(!allocIntFromStdin(&size, &in))
	{
		perror("Cannot allocate input array");
		return false;
	}

	int* out = malloc(sizeof(int) * 2 * size);
	if(out == NULL)
	{
		perror("Cannot allocate final array");
		return false;
	}

	*pSize = size;
	*pIn = in;
	*pOut = out;
	return true;
}

void destroy(int *in, int *out)
{
	freeInt(in);
	free(out);
}

void swap(int* a, int* b)
{
	int backup = *a;
	*a = *b;
	*b = backup;
}

int max(int a, int b)
{
	return (a > b ? a : b);
}

int binarySearch(
	int element, int const in[],
	int idxFrom, int idxTo
)
{
	int low = idxFrom;
	int high = max(idxFrom, idxTo + 1);

	while(low < high)
	{
		int mid = (low + high) / 2;

		if(element <= in[mid])
			high = mid;
		else
			low = mid + 1;
	}

	return high;
}

#ifdef GRAPH
int nextNode = 0;

void merge(
	int const in[],
	int idxStartArr1,
	int idxEndArr1,
	int idxStartArr2,
	int idxEndArr2,
	int out[],
	int idxStartOut,
	int nodeParent,
	int node
)
#else
void merge(
	int const in[],
	int idxStartArr1,
	int idxEndArr1,
	int idxStartArr2,
	int idxEndArr2,
	int out[],
	int idxStartOut
)
#endif
{
	int sizeArr1 = idxEndArr1 - idxStartArr1 + 1;
	int sizeArr2 = idxEndArr2 - idxStartArr2 + 1;

#ifdef GRAPH
	fprintf(stderr, "\t%d [fillcolor=%d];\n",
		node, omp_get_thread_num() + 1);
	if(nodeParent >= 0)
		fprintf(stderr, "\t%d -> %d;\n", nodeParent, node);
#endif

	if(sizeArr1 < sizeArr2)
	{
		swap(&sizeArr1, &sizeArr2);
		swap(&idxStartArr1, &idxStartArr2);
		swap(&idxEndArr1, &idxEndArr2);
	}

	if(sizeArr1 > 0)
	{
		int idxPivotArr1 = (idxStartArr1 + idxEndArr1) / 2;
		int idxPivotArr2 = binarySearch(
			in[idxPivotArr1], in, idxStartArr2, idxEndArr2
		);
		int idxPivotOut =
			idxStartOut +
			(idxPivotArr1 - idxStartArr1) +
			(idxPivotArr2 - idxStartArr2);

		out[idxPivotOut] = in[idxPivotArr1];

#ifdef GRAPH
		int myNextNode1, myNextNode2;
#pragma omp critical
		{
			myNextNode1 = ++nextNode;
			myNextNode2 = ++nextNode;
		}
#endif

		// Parallel version
#pragma omp parallel
		{
#pragma omp single nowait
			{
#pragma omp task
				{
#ifdef GRAPH
					merge(
						in,
						idxStartArr1, idxPivotArr1 - 1,
						idxStartArr2, idxPivotArr2 - 1,
						out, idxStartOut,
						node, myNextNode1
					);
#else
					merge(
						in,
						idxStartArr1, idxPivotArr1 - 1,
						idxStartArr2, idxPivotArr2 - 1,
						out, idxStartOut
					);
#endif
				}
#pragma omp task
				{
#ifdef GRAPH
					merge(
						in,
						idxPivotArr1 + 1, idxEndArr1,
						idxPivotArr2, idxEndArr2,
						out, idxPivotOut + 1,
						node, myNextNode2
					);
#else
					merge(
						in,
						idxPivotArr1 + 1, idxEndArr1,
						idxPivotArr2, idxEndArr2,
						out, idxPivotOut + 1
					);
#endif
				}
			}
		}
	}

	// Debugging purpose: sleep to make appear all the threads in htop
	//sleep(1);
}

int main()
{
	int size;
	int *in, *out;
	if(!init(&size, &in, &out))
	{
		perror("Init failed");
		return EXIT_FAILURE;
	}
	print(size, in);
	print(size, in + size);

#ifdef GRAPH
	fprintf(stderr, "digraph g {\n");
	fprintf(stderr, "\tnode [colorscheme=rdylgn11 style=filled]\n");
#endif

	clock_t begin = clock();
#ifdef GRAPH
	merge(in, 0, size - 1, size, 2 * size - 1, out, 0, -1, 0);
#else
	merge(in, 0, size - 1, size, 2 * size - 1, out, 0);
#endif
	clock_t end = clock();
	clock_t duration = (end - begin) / (CLOCKS_PER_SEC / 1000);

	print(size * 2, out);

#ifdef GRAPH
	fprintf(stderr, "}\n");
#endif

	fprintf(stderr, "//Time: %ld ms\n", duration);

	destroy(in, out);
	return EXIT_SUCCESS;
}
