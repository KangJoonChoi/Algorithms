#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define infinite 9999 // 만약 9999 이상의 값을 사용할 경우 
#define NIL 7777 ///NIL 값을 임의의 값으로 지정

void floyd_warshall(int*** distancematrix, int*** predecessormatrix, int value);
int main()
{
	FILE* input = fopen("input.txt", "r"); //파일을 열어준다. 
	if (input == NULL)
	{
		printf("File Open Error");
	}
	FILE* distoutput = fopen("floyd_warshall_distance.txt", "w");
	FILE* predecoutput = fopen("floyd_warshall_predecessor.txt", "w");
	char string[10] = { 0 }; //두자리수 이상 받고싶으면 string수정
	fscanf(input, "%s", string);
	int value = 0;
	for (int i = 0; i <= strlen(string) - 1; i++)
	{
		value += string[i] - 48;
		value *= 10;
	}
	value /= 10; //row 수

	int*** distmatrix;
	int*** predmatrix;
	distmatrix = (int***)malloc(sizeof(int**) * (value+1)); //동적할당
	predmatrix = (int***)malloc(sizeof(int**) * (value+1));
	for (int j = 0; j <= value; j++)
	{
		distmatrix[j] = (int**)malloc(sizeof(int*) * value);
		predmatrix[j] = (int**)malloc(sizeof(int*) * value);
		for (int k = 0; k <= value; k++)
		{
			distmatrix[j][k] = (int*)malloc(sizeof(int) * value);
			predmatrix[j][k] = (int*)malloc(sizeof(int) * value);
		}
	}// 여기까지 동적할당
	for(int i = 0; i < value; i++) //초기값 입력
	{
		for (int j = 0; j < value; j++)
		{
			char string[10] = { 0 };
			fscanf(input, "%s", string);
			int number = 0;
			if (string[0] == '-')//음수 weight값 입력 받으면
			{
				for (int i = 1; i <= strlen(string) - 1; i++)
				{
					number += string[i] - 48;
					number *= 10;
				}
				number /= 10;
				number = number * (-1);
			}
			else
			{//양수 weight값 입력받으면
				for (int i = 0; i <= strlen(string) - 1; i++)
				{
					number += string[i] - 48;
					number *= 10;
				}
				number /= 10;
			}
			

			if (((number == 0) && (i == j)) || (number != 0)) //자기 자신에게 가는 weight
			{
				distmatrix[0][i][j] = number;
			}
			else//같지 않은데 0이 입력될경우 infinite값 넣어준다
			{
				distmatrix[0][i][j] = infinite;
			}

			if ((distmatrix[0][i][j] == infinite) || (distmatrix[0][i][j] == 0)) //NIL값 입력 혹은 자기 자신으로 가는 경우 
			{
				predmatrix[0][i][j] = NIL; //NIL 값을 받고
			}
			else
			{
				predmatrix[0][i][j] = i+1; // 아닌경우 (i가 0부터 시작해서 i+1을 해준다)
			}
		}
	}

	floyd_warshall(distmatrix, predmatrix, value);
	for (int i = 0; i < value; i++)
	{
		for (int j = 0; j < value; j++)
		{
			fprintf(distoutput, "%d     ", distmatrix[value][i][j]);
			if (predmatrix[value][i][j] == NIL)
			{
				fprintf(predecoutput, "N     ");
			}
			else
			{
				fprintf(predecoutput, "%d     ", predmatrix[value][i][j]);
			}
			
		}
		fprintf(distoutput, "\n");
		fprintf(predecoutput, "\n");
	}
	fclose(input);
	fclose(distoutput);
	fclose(predecoutput);
}

void floyd_warshall(int*** distancematrix, int*** predecessormatrix, int value)
{
	int n = value;
	for (int k = 1; k <= n; k++)
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (distancematrix[k - 1][i][j] <= (distancematrix[k - 1][i][k - 1] + distancematrix[k - 1][k - 1][j])) //이전값이 작다면
				{
					distancematrix[k][i][j] = distancematrix[k - 1][i][j]; //이전 값 그대로 유지
					predecessormatrix[k][i][j] = predecessormatrix[k - 1][i][j];
				}
				else
				{
					distancematrix[k][i][j] = distancematrix[k - 1][i][k - 1] + distancematrix[k - 1][k - 1][j]; //새로운 vertex거친 값이 작다면 distance 저장
					if (distancematrix[k][i][j] < 7000)
					{
						predecessormatrix[k][i][j] = predecessormatrix[k - 1][k-1][j]; //새로운 값 k, j 저장(k-1 vertex 를 거쳐서 가는데, 여기에서 해당하는 값으
							//predeessormatrix에서는 k-1이 특정 경로로 가기 위해 제일 마지막으로 지나쳐야 할 vertex의 정보가 존재한다.
					}
					else
					{
						predecessormatrix[k][i][j] = predecessormatrix[k - 1][i][j]; // 혹시모를 INFINITE 값이 작아졌다고 입력되는 상황 방지 
					}
					
				}
			}
		}
	}
}
