#include<stdio.h>
#include<stdlib.h>

#define MATRIX_SIZE 10

void generateMatrix(float *a, float *b)
{
	int i;
	int size = MATRIX_SIZE * MATRIX_SIZE;
	for (i = 0; i < size; i++)
		a[i] = i;
	for (i = 0; i < MATRIX_SIZE; i++)
		b[i] = i * i;
}

int main()
{
	int i, idx;
	float *a, *b, *c;
	float t;
	int n = MATRIX_SIZE;

	a = (float*)malloc(sizeof(float)*n*n);
	b = (float*)malloc(sizeof(float)*n);
	c = (float*)malloc(sizeof(float)*n);

	generateMatrix(a, b);

	for (idx = 0; idx < n; idx++)
	{
		t = 0;
		for (i = 0; i < n; i++)
			t += a[idx*n+i]*b[i];
		c[idx] = t;
	}
	printf("Result:\n");
	for(i = 0; i < n; i++)
		printf("%lf\t", c[i]);
	printf("\n");
}	
