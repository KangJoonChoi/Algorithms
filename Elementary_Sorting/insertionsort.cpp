#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#define NUM 10000
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void insertionSort(int* A, int n);

int main()
{
	clock_t start = clock();
	int count = 0;
	int arr[NUM];

	ifstream inputfile;
	inputfile.open("input.txt");

	while (count<=10000)
	{
		inputfile >> arr[count];
		count++;
	}

	insertionSort(arr, NUM);

	inputfile.close();

	FILE* fout = fopen("outputinsertionsort.txt", "w");

	for (int a = 0; a < NUM; a++)
	{
		fprintf(fout, "%d ", arr[a]);
	}

	fclose(fout);
	clock_t end = clock();
	printf("Time: %lf\n", (double)(end - start) / CLOCKS_PER_SEC);

}

void insertionSort(int* A, int n)
{
	int i, j, key;
	for (i = 1; i < n; i++) {
		key = A[i];
		for (j = i - 1; j >= 0 && A[j] > key; j--)
			A[j + 1] = A[j];
		A[j + 1] = key;
	}
}

