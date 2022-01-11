#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#define NUM 10000
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void mergeSort(int* A, int l, int r);

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

	mergeSort(arr, 0, 9999);

	inputfile.close();

	FILE* fout = fopen("outputRecursiveMerge.txt", "w");

	for (int a = NUM - 1; a >= 0; a--)
	{
		fprintf(fout, "%d ", arr[a]);
	}

	fclose(fout);
	clock_t end = clock();
	printf("Time: %lf\n", (double)(end - start) / CLOCKS_PER_SEC);
}

void merge(int* A, int* l, int* C, int o, int r);
void mergeSort(int* A, int l, int r)
{
	int c = (l + r) / 2;
	if (l < r)
	{
		mergeSort(A, l, c);
		mergeSort(A, c + 1, r);
		int* arr1 = (int*)malloc(sizeof(int) * (c - l + 1));
		int* arr2 = (int*)malloc(sizeof(int) * (r - c));
		int* arr3 = (int*)malloc(sizeof(int) * (r -l + 1));
		int k = 0;
		int b = 0;
		int v = 0;
		while (k < (c - l + 1))
		{
			arr1[k] = A[l + k];
			k++;
		}
		while (b < (r - c))
		{
			arr2[b] = A[c + 1 + b];
			b++;
		}
		merge(arr1, arr2, arr3, c-l+1,r-c);
		while (v < (r - l + 1))
		{
			A[l + v] = arr3[v];
			v++;
		}
		free(arr1);
		free(arr2);
		free(arr3);
	}
}

int compare(int a, int b);
void merge(int* A, int* l, int* C, int o, int r)
{
	int i, j, k;
	i = j = k = 0;
	while ((i < o) && (j < r))
	{
		switch (compare(A[i], l[j]))
		{
		case 1: C[k] = A[i]; i++; k++; break;

		case 0: C[k] = A[i]; i++; k++;
			C[k] = l[j]; j++; k++; break;

		case 2: C[k] = l[j]; j++; k++; break;
		}
	}
	while (i < o)
	{
		C[k] = A[i];
		k++;
		i++;
	}
	while (j < r)
	{
		C[k] = l[j];
		k++;
		j++;
	}
}

int compare(int a, int b)
{
	if (a > b) return 1;
	else if (a == b) return 0;
	else if (a < b) return 2;
}
