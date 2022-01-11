#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#define NUM 10000
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void selectionSort(int* A, int n);

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

	selectionSort(arr, NUM);

	inputfile.close();

	FILE* fout = fopen("outputselectionsort.txt", "w");

	for (int a = 0; a < NUM; a++)
	{
		fprintf(fout, "%d ", arr[a]);
	}

	fclose(fout);
	clock_t end = clock();
	printf("Time: %lf\n", (double)(end - start) / CLOCKS_PER_SEC);
}

void selectionSort(int* A, int n)
{
	int max, i, temp;
	while (n > 0) 
	{
		max = 0;
		for (i = 1; i < n; i++)
			if (A[i] > A[max])
				max = i;

		temp = A[n - 1];
		A[n - 1] = A[max];
		A[max] = temp;
		n--;
	}
}

