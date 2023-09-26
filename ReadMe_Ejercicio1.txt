Definición de matrices:
una matris es una estructura de datos donde se divide en columnas y filas,
cada una corespondiendo a x, y en coordenadas. Un ejemplo de esto seria:
| a11 a12 |
| a21 a22 |

Multiplicación de matrices
La multiplicacion de matrices es cuando se multiplican las filas de una matriz,
por las columnas de otra matriz. 

las filas de las matrices se pueden interpretar como i, y las columnas como j

Proceso de multiplicacion
La forma de multiplicar una matriz es la siguiente. Primero asegurate de que la
antidad de columnas de matriz A sea igual que la cantidad de filas de matriz B.

Por ejemplo: | a11 a12 |	| b11 b12 b13 |
	     | a21 a22 |	| b21 b22 b23 |
	     | a31 a32 |
Si es posible porque es 3x2 y 2x3, por lo que la cantidad de columnas de matriz A 
es igual a la cantidad de filas en matriz B.

por el otro lado: | a11 a12 |	| b11 b12 b13 |
	          | a21 a22 |	| b21 b22 b23 |
	     	  | a31 a32 |	| b31 b32 b33 |
No es posible porque hay mas filas en matriz B que columnas en matriz A, 3x2, 3x3.

Despues se hace lo siguiente:
Se multiplican los valores de la fila de a y las columnas de b, despues de multiplicar
cada uno de los valores que corresponden se suman todos los resultados. La formula
siendo

cij = (a1j * b i1) + (a2j * b i2) + (a3j * b i3) + ... + (anj * b in)

de esta forma se obtiene el valor correspondiente de la cordenada de la matriz C.

Aplicaciones de la multiplicación de matrices:

Código

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

Análisis de complejidad temporal
1. Inicio para dar datos a las matrices 1 y 2.
	Hay bucle de dos for lo cual se califica que O(N^2).

2. Multiplicación:
	Se tienen un ytiple bucle de for lo cual lo haria un O(N^3).

3. Impresión de la Matriz Resultante:
	Es un bucle de dos for, po lo cual es O(N^2).