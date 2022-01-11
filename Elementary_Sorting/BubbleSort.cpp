#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#define NUM 10000
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void bubbleSort(int* A, int n);

int main()
{
	clock_t start = clock();
	int count = 0;

	int arr[NUM];

	ifstream inputfile;
	inputfile.open("input.txt");

	while (count <= 10000)
	{
		inputfile >> arr[count];
		count++;
	}

	bubbleSort(arr, NUM);
	inputfile.close();

	FILE* fout = fopen("outputbubblesort.txt", "w");

	for (int a = 0; a < NUM; a++)
	{
		fprintf(fout, "%d ", arr[a]);
	}

	fclose(fout);
	clock_t end = clock();
	printf("Time: %lf\n", (double)(end - start) / CLOCKS_PER_SEC);
}

void bubbleSort(int* A, int n)
{
	int i, j, temp;
	for (i = n - 1; i > 0; i--)
		for(j = 0; j<i; j++)
			if (A[j] > A[j + 1]) {
				temp = A[j];
				A[j] = A[j + 1];
				A[j + 1] = temp;
			}
}

