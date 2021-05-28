#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<cuda_runtime.h>
#define THREAD_NUM 256
#define MATRIX_SIZE 1000
int blocks_num = (MATRIX_SIZE * MATRIX_SIZE + THREAD_NUM - 1) / THREAD_NUM;

void generateMatrix(float * a, float *b)
{
	int i;
	int SIZE = MATRIX_SIZE * MATRIX_SIZE;
	for (i = 0; i < SIZE; i ++)
	{
		a[i] = i;
		b[i] = i * i;
	}
}

__global__ static void CUDAkernal(const float *a ,const float *b, float *c, int n)
{
	//block threadID
	const int tid = threadIdx.x;
	//blockID
	const int bid = blockIdx.x;
	//threadID
	const int idx = bid * THREAD_NUM + tid;
	const int row = idx / n;
	const int column = idx % n;
	if (row < n && column < n)
	{ 
		float t = 0;
		for (i = 0; i < n; i++)
		{ 
			t += a[row * n + i] * b[i * n + column];
		}
	}
}

int main()
{
	float *a, *b, *c, *d;
	float *cuda_a, * cuda_b, * cuda_c;
	int n = MATRIX_SIZE;

	//CPU alloc
	a = (float*)malloc(sizeof(float) * n * n);
	b = (float*)malloc(sizeof(float) * n * n);
	c = (float*)malloc(sizeof(float) * n * n);
	d = (float*)malloc(sizeof(float) * n * n);

	//GPU alloc
	cudaMalloc((void**)&cuda_a, sizeof(float) * n * n);
	cudaMalloc((void**)&cuda_b, sizeof(float) * n * n);
	cudaMalloc((void**)&cuda_c, sizeof(flaot) * n * n);

	generateMatrix(a, b);

	//Copy
	cudaMemcpy(cuda_a, a, sizeof(float)*n*n, cudaMemcpyHostToDevice);
	cudaMemcpy(cuda_b, b, sizeof(float)*n*n, cudaMemcpyHostToDevice);

	CUDAkernal << <blocks_num, THREAD_NUM, 0>> >(cuda_a, cuda_b, cuda_c, n);

	//Copy
	cudaMemcpy(c, cuda_c, sizeof(float)*n*n, cudaMemcpyDeviceToHost);

	cudaFree(cuda_a);
	cudaFree(cuda_b);
	cudaFree(cuda_c);
}
