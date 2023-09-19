#include <stdio.h>

void main()
{
	int matriz1[10][10];
	int matriz2[10][10];
	int matrizR[10][10];

	int numb = 1;

	int resp;

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			matriz1[i][j] = numb;
			matriz2[j][i] = numb;

			numb++;
		}
	}

	for (int k = 0; k < 10; k++)
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				resp += matriz1[i][j] * matriz2[j][i];
			}

			matrizR[i][k] = resp;
			resp = 0;
		}
	}

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			printf("%d ", matrizR[i][j]);
		}
		printf("\n");
	}
    return 0;
}