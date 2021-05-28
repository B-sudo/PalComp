#include<stdio.h>
#include<stdlib.h>

#define MATRIX_SIZE 10

void generateMatrix(float *a, float *b)
{
	int i;
	int SIZE = MATRIX_SIZE * MATRIX_SIZE;
	for (i = 0; i < SIZE; i++)
	{
		a[i] = i;
		b[i] = i * i;
	}
}

int main()
{
	int i, j, row, column;
	float t;
	float *a, *b, *c;
	int n = MATRIX_SIZE;
	int size = MATRIX_SIZE * MATRIX_SIZE;

	a = (float*)malloc(sizeof(float)*n*n);
	b = (float*)malloc(sizeof(float)*n*n);
	c = (float*)malloc(sizeof(float)*n*n);

	generateMatrix(a, b);

	for (i = 0; i < size; i++)
	{
		t = 0;
		row = i / n;
		column = i % n;
		for (j = 0; j < n; j++)
			t += a[row*n+j]*b[j*n+column];
		c[i] = t;
	}

	printf("Result:\n");
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
			printf("%lf\t", c[i*n+j]);
		printf("\n");
	}
}
