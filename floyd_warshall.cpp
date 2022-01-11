#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define infinite 9999 // ���� 9999 �̻��� ���� ����� ��� 
#define NIL 7777 ///NIL ���� ������ ������ ����

void floyd_warshall(int*** distancematrix, int*** predecessormatrix, int value);
int main()
{
	FILE* input = fopen("input.txt", "r"); //������ �����ش�. 
	if (input == NULL)
	{
		printf("File Open Error");
	}
	FILE* distoutput = fopen("floyd_warshall_distance.txt", "w");
	FILE* predecoutput = fopen("floyd_warshall_predecessor.txt", "w");
	char string[10] = { 0 }; //���ڸ��� �̻� �ް������ string����
	fscanf(input, "%s", string);
	int value = 0;
	for (int i = 0; i <= strlen(string) - 1; i++)
	{
		value += string[i] - 48;
		value *= 10;
	}
	value /= 10; //row ��

	int*** distmatrix;
	int*** predmatrix;
	distmatrix = (int***)malloc(sizeof(int**) * (value+1)); //�����Ҵ�
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
	}// ������� �����Ҵ�
	for(int i = 0; i < value; i++) //�ʱⰪ �Է�
	{
		for (int j = 0; j < value; j++)
		{
			char string[10] = { 0 };
			fscanf(input, "%s", string);
			int number = 0;
			if (string[0] == '-')//���� weight�� �Է� ������
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
			{//��� weight�� �Է¹�����
				for (int i = 0; i <= strlen(string) - 1; i++)
				{
					number += string[i] - 48;
					number *= 10;
				}
				number /= 10;
			}
			

			if (((number == 0) && (i == j)) || (number != 0)) //�ڱ� �ڽſ��� ���� weight
			{
				distmatrix[0][i][j] = number;
			}
			else//���� ������ 0�� �Էµɰ�� infinite�� �־��ش�
			{
				distmatrix[0][i][j] = infinite;
			}

			if ((distmatrix[0][i][j] == infinite) || (distmatrix[0][i][j] == 0)) //NIL�� �Է� Ȥ�� �ڱ� �ڽ����� ���� ��� 
			{
				predmatrix[0][i][j] = NIL; //NIL ���� �ް�
			}
			else
			{
				predmatrix[0][i][j] = i+1; // �ƴѰ�� (i�� 0���� �����ؼ� i+1�� ���ش�)
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
				if (distancematrix[k - 1][i][j] <= (distancematrix[k - 1][i][k - 1] + distancematrix[k - 1][k - 1][j])) //�������� �۴ٸ�
				{
					distancematrix[k][i][j] = distancematrix[k - 1][i][j]; //���� �� �״�� ����
					predecessormatrix[k][i][j] = predecessormatrix[k - 1][i][j];
				}
				else
				{
					distancematrix[k][i][j] = distancematrix[k - 1][i][k - 1] + distancematrix[k - 1][k - 1][j]; //���ο� vertex��ģ ���� �۴ٸ� distance ����
					if (distancematrix[k][i][j] < 7000)
					{
						predecessormatrix[k][i][j] = predecessormatrix[k - 1][k-1][j]; //���ο� �� k, j ����(k-1 vertex �� ���ļ� ���µ�, ���⿡�� �ش��ϴ� ����
							//predeessormatrix������ k-1�� Ư�� ��η� ���� ���� ���� ���������� �����ľ� �� vertex�� ������ �����Ѵ�.
					}
					else
					{
						predecessormatrix[k][i][j] = predecessormatrix[k - 1][i][j]; // Ȥ�ø� INFINITE ���� �۾����ٰ� �ԷµǴ� ��Ȳ ���� 
					}
					
				}
			}
		}
	}
}