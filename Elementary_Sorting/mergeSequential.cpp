#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#define NUM 10000
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void mergeSort(int* A, int n);

int main()
{
	clock_t start = clock();
	int count = 0;
	int arr[NUM];

	ifstream inputfile;
	inputfile.open("input.txt");

	while (count <= NUM)
	{
		inputfile >> arr[count];
		count++;
	}

	mergeSort(arr, NUM);

	inputfile.close();

	FILE* fout = fopen("outputSequentialMerge.txt", "w");

	for (int a = NUM-1; a >= 0; a--)
	{
		fprintf(fout, "%d ", arr[a]);
	}

	fclose(fout);
	clock_t end = clock();
	printf("Time: %lf\n", (double)(end - start) / CLOCKS_PER_SEC);
}
 
void merge(int* A, int* B, int* C, int sizeA, int sizeB);
void mergeSort(int* A, int n)
{
	int j, i;
	
	for (i = 1; i <2*( (n / 2) + (n % 2) + 1); i=2*i)
	{
		for (j = 0; j < n; j = j + 2*i) 
		{
			int limit = j + 2*i;
			if (( limit > n)) //마지막부분
			{
				int partial = n - j; //partial 개 남아있음
				int rest;
				if (partial >= i) { rest = partial - i;}//2번째배열 할당수판별
				else { rest = 0;}
				if (i < partial) //2번째 배열을 만들만큼 큰경우 아닐경우 그대로 두면 된다
				{
					int* arr1 = (int*)malloc(sizeof(int) * i);
					int* arr2 = (int*)malloc(sizeof(int) * rest);
					int* arr3 = (int*)malloc(sizeof(int) * (i + rest));
					if (rest > 0) //2개의 배열에 나눠 할당
					{
						for (int k = 0; k < i; k++)
						{
							arr1[k] = A[j + k];
						}
						for (int l = 0; l < rest; l++)
						{
							arr2[l] = A[j + i + l];
						}
					}
					merge(arr1, arr2, arr3, i, rest);
					for (int q = 0; q < (i + rest); q++) //기존 배열에 합산
					{
						A[j + q] = arr3[q];
					}
					free(arr1);
					free(arr2);
					free(arr3);
				}
			}//마지막부분
			else //일반적인 경우
			{
				int* arr1 = (int*)malloc(sizeof(int) * i);
				int* arr2 = (int*)malloc(sizeof(int) * i);
				int* arr3 = (int*)malloc(sizeof(int) * (i + i));
				for (int k = 0; k < i; k++)
				{
					arr1[k] = A[j + k];
					arr2[k] = A[j + k + i];
				}
				merge(arr1, arr2, arr3, i, i);
				for (int q = 0; q < (i + i); q++)
				{
					A[j + q] = arr3[q];
				}
				free(arr1);
				free(arr2);
				free(arr3);
			}
		}
	}

}

int compare(int a, int b);
void merge(int* A, int* B, int* C, int sizeA, int sizeB)
{
	int i, j, k;
	i = j = k = 0;
	while ((i < sizeA) && (j < sizeB))
	{
		switch (compare(A[i], B[j]))
		{
		case 1: C[k] = A[i]; i++; k++; break;

		case 0: C[k] = A[i]; i++; k++;
			C[k] = B[j]; j++; k++; break;

		case 2: C[k] = B[j]; j++; k++; break;
		}
	}
	while (i < sizeA)
	{
		C[k] = A[i];
		k++;
		i++;
	}
	while (j < sizeB)
	{
		C[k] = B[j];
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
