#include "stdin.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

bool readStdin(char* pFile[])
{
	if(fseek(stdin, 0, SEEK_END) != 0)
	{
		perror("Cannot reach EOF");
		return false;
	}

	long availableBytes = ftell(stdin);
	if(availableBytes == -1L)
	{
		perror("Cannot get the count of available bytes");
		return false;
	}

	if(fseek(stdin, 0, SEEK_SET) != 0)
	{
		perror("Cannot restore BOF");
		return false;
	}

	char* file = malloc(availableBytes);
	if(file == NULL)
	{
		perror("Cannot allocate spaces to parse the file");
		return false;
	}
	*pFile = file;

	long bytesRead = fread(file, 1, availableBytes, stdin);
	if(bytesRead != availableBytes)
	{
		perror("Mismatch between read and available bytes from file");
		return false;
	}

	return true;
}

bool readInt(int* out, char* file, char** pFrom)
{
	char* endptr = 0;
	char* from = *pFrom;
	if(from == 0)
		from = file;

	int integer = (int)strtol(from, &endptr, 10);
	// If endptr not changed, there is an issue
	if(endptr == 0)
	{
		perror("Cannot parse one integer");
		return false;
	}

	int countCharsToSkip = 1; // Skip the whitespace/EOL
	if(integer == 0)
		countCharsToSkip++;
	else if(integer < 0)
	{
		countCharsToSkip++; // Skip the "-"
		countCharsToSkip += 1 + log10(abs(integer));
	}
	else
		countCharsToSkip += 1 + log10(integer);
	// Remove first countChars characters in the buffer
	from += countCharsToSkip;

	*out = integer;
	*pFrom = from;
	return true;
}

bool fillIntArray(int intArray[], int size, char* file, char** from)
{
	for(int i = 0; i < size; i++)
	{
		int val;

		if(readInt(&val, file, from))
			intArray[i] = val;
		else
		{
			perror("No input file or bad formatted file");
			return false;
		}
	}

	return true;
}

bool allocIntFromStdin(int* pSize, int** pInt)
{
	char* file;
	if(!readStdin(&file))
	{
		perror("Cannot read the file");
		return false;
	}

	char* from = 0;
	int size;
	if(!readInt(&size, file, &from))
	{
		perror("Cannot read size\n");
		return false;
	}
	*pSize = size;

	int* integers = malloc(2 * sizeof(int) * size);
	if(integers == NULL)
	{
		perror("Cannot allocate array\n");
		return false;
	}
	*pInt = integers;

	if(!fillIntArray(integers, size, file, &from))
		return false;

	if(!fillIntArray(integers + size, size, file, &from))
		return false;

	free(file);

	return true;
}

void freeInt(int* integers)
{
	if(integers != 0)
		free(integers);
}

