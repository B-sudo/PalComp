#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<cuda_runtime.h>

#define THREAD_NUM 256
#define MATRIX_SIZE 1000

int blocks_num = (MATRIX_SIZE + THREAD_NUM - 1) / THREAD_NUM;

void generateMatrix(float *a, float *b)		//a for matrix b for vector
{
	int i;
	int size = MATRIX_SIZE * MATRIX_SIZE;
	for (i = 0; i < size; i++)
		a[i] = i;
	for (i = 0; i < MATRIX_SIZE; i++)
		b[i] = i * i;
}

__global__ static void CUDAkernal(const float *a, const float *b, float *c, int n)
{
	int i;
	const int tid = threadIdx.x;
	const int bid = blockIdx.x;
	const int idx = bid * THREAD_NUM + tid;
	if (idx < n)
	{
		float t = 0;
		for (i = 0; i < n; i++)
			t += a[idx*n+i]*b[i];
		c[idx] = t;
	}
}

int main()
{
	int i;
	float *a, *b, *c;
	float *cuda_a, *cuda_b, *cuda_c;
	int n = MATRIX_SIZE;
	//alloc
	a = (float*)malloc(sizeof(float)*n*n);
	b = (float*)malloc(sizeof(float)*n);
	c = (float*)malloc(sizeof(float)*n);

	cudaMalloc((void**)&cuda_a, sizeof(float)*n*n);
	cudaMalloc((void**)&cuda_b, sizeof(float)*n);
	cudaMalloc((void**)&cuda_c, sizeof(float)*n);

	generateMatrix(a, b);

	//Copy
	cudaMemcpy(cuda_a, a, sizeof(float)*n*n, cudaMemcpyHostToDevice);
	cudaMemcpy(cuda_b, b, sizeof(float)*n, cudaMemcpyHostToDevice);

	CUDAkernal<<<blocks_num, THREAD_NUM, 0>>>(cuda_a, cuda_b, cuda_c, n);
	
	cudaMemcpy(c, cuda_c, sizeof(float)*n, cudaMemcpyDeviceToHost);

	cudaFree(cuda_a);
	cudaFree(cuda_b);
	cudaFree(cuda_c);
}

