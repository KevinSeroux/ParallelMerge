#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <assert.h>
#include <omp.h>
#include <unistd.h>

void print(int size, int const integers[])
{
	for(int i = 0; i < size; i++)
		printf("%d ", integers[i]);

	printf("\n");
}

int init(int **in, int **out) {
	int size;
	fscanf(stdin, "%d", &size);

	int *tmp = (int *) malloc(sizeof(int)*2*size);
	if (!tmp) return 0;

	int i=0;
	while (fscanf(stdin, "%d", &tmp[i]) == 1) i++;
		*in = tmp;

	*out = (int *) malloc(sizeof(int)*2*size);
	if (!out) return 0;

	return size;
}

void destroy(int *in, int *out)
{
	free(in);
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

void start_merge(
	int const in[],
	int idxStartArr1,
	int idxEndArr1,
	int idxStartArr2,
	int idxEndArr2,
	int out[],
	int idxStartOut
)
{
	omp_set_nested(1);

#pragma omp parallel
	{
#pragma omp single nowait
		{
			merge(
				in,
				idxStartArr1,
				idxEndArr1,
				idxStartArr2,
				idxEndArr2,
				out,
#ifdef GRAPH
				idxStartOut,
				-1,
				0
#else
				idxStartOut
#endif
			);
		}
	}
}

int main()
{
	int size;
	int *in, *out;
	size = init(&in, &out);
	if (!size) {
		perror("Init failed");
		return EXIT_FAILURE;
	}

#ifdef GRAPH
	fprintf(stderr, "digraph g {\n");
	fprintf(stderr, "\tnode [colorscheme=rdylgn11 style=filled]\n");
#endif

	double begin = omp_get_wtime();
	start_merge(in, 0, size - 1, size, 2 * size - 1, out, 0);
	double end = omp_get_wtime();
	double duration = end - begin;

 	print(size * 2, out);

 #ifdef GRAPH
 	fprintf(stderr, "}\n//");
 #endif

 	fprintf(stderr, "Time: %lf seconds\n", duration);

	destroy(in, out);
	return EXIT_SUCCESS;
}
