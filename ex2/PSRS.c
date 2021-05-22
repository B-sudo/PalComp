#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<assert.h>
#include"mpi.h"
#define N 27

int *L, *R;

void Merge(int *a, int p, int q, int r)
{
	int i, j, k;
	int n1 = q - p + 1;
	int n2 = r - q;
	L = (int *)malloc((n1 + 1)*sizeof(int));
	R = (int *)malloc((n2 + 1)*sizeof(int));

	for (i = 0; i < n1; i++)
		L[i] = a[p + i];
	L[i] = 65536;
	for (j = 0; j < n2; j++)
		R[j] = a[q + j + 1];
	R[j] = 65536;
	i = 0;
	j = 0;
	for (k = p; k <= r; k++)
	{
		if (L[i] <= R[j])
			a[k] = L[i++];
		else
			a[k] = R[j++];
	}
}

void MergeSort(int *a, int p, int r)
{
	int q;
	if (p < r)
	{
		q = (p + r)/2;
		MergeSort(a, p, q);
		MergeSort(a, q + 1, r);
		Merge(a, p, q, r);
	}
}

void main(int argc, char * argv[])
{
	int i, k=0, id, start, end, P, step, left=0, right=0, TotalRecv=0;
	int * array, * sam, * Sample, * keyelem, * fina;
        int * posl, * posr, * count,* recv, * finabias;
	int * FinalArrayLength, * FinalArrayBias, * FinalArray;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &P);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	//record the start and end index of the array
	start = id * (N/P);
	end = (id + 1) * (N/P) - 1;
	array = (int *)malloc(sizeof(int) * (end - start + 1));
	for (i = 0; i < end - start + 1; i++)
		array[i] = (rand() % 400)*(id+1) % 97;
	//Sort locally
	MergeSort(array, 0, end - start);

	//Regular Sample
	sam = (int *)malloc(sizeof(int) * P);
	step = (end - start + 1)/P;
	for (i = 0; i < end - start + 1; i += step)
		sam[i/step] = array[i];
	
	if (id == 0)
		Sample = (int *)malloc(sizeof(int) * P * P);
	
	//gather
	MPI_Gather(sam, P, MPI_INT, Sample, P, MPI_INT, 0, MPI_COMM_WORLD);

	keyelem = (int *)malloc(sizeof(int) * (P - 1));
	if (id == 0)
	{
		MergeSort(Sample, 0, P * P - 1);
		for (i = P; i < P * P;i += P)
		       keyelem[i/P - 1] = Sample[i];	
	}
	
	//broadcast
	MPI_Bcast(keyelem, P - 1, MPI_INT, 0, MPI_COMM_WORLD);

	//partion segment
	posl = (int *)malloc(sizeof(int) * P);
	posr = (int *)malloc(sizeof(int) * P);
	memset(posl, -1, P);
	memset(posr, -1, P);
	i = 0;
	while (i <= end - start)
	{
		if (array[i] > keyelem[k])
		{
			k++;
			if (left > right)
				posr[right++] = i - 1;
			else
			{
				left++;
				right++;
			}
			if (k == P - 1)
			{
				posl[left] = i;
				posr[right] = end - start;
				break;
			}
		}
		else if (left == right)
			posl[left++] = i++;
		else
			i++;
	}
	if (left > right)
		posr[right++] = end - start;
	
	//global exchange
	count = (int *)malloc(sizeof(int) * P);
	recv = (int *)malloc(sizeof(int) * P);
	for (i = 0; i < P; i++)
	{
		if (posr[i] != -1 && posl[i] != -1)
			count[i] = posr[i] - posl[i] + 1;
		else
			count[i] = 0;
	}
	MPI_Alltoall(count, 1, MPI_INT, recv, 1, MPI_INT, MPI_COMM_WORLD);

	finabias = (int *)malloc(sizeof(int) * P);
	finabias[0] = 0;
	for (i = 1; i < P; i++)
		finabias[i] = finabias[i-1] + recv[i-1];
	for (i = 0; i < P; i++)
		TotalRecv += recv[i];

	fina = (int *)malloc(sizeof(int) * TotalRecv);

	MPI_Alltoallv(array, count, posl, MPI_INT, fina, recv, finabias, MPI_INT, MPI_COMM_WORLD);

	MergeSort(fina, 0, TotalRecv - 1);

	//fanal merge
	if (id != 0)
	{
		MPI_Send(&TotalRecv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); 
		MPI_Send(fina, TotalRecv, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	else if (id == 0)
	{	
		FinalArrayLength = (int *)malloc(sizeof(int) * P);
		FinalArrayBias = (int *)malloc(sizeof(int) * P);
		FinalArray = (int *)malloc(sizeof(int) * N);

		FinalArrayLength[0] = TotalRecv;
		for (i = 1; i < P; i++)
			MPI_Recv(FinalArrayLength + i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
		FinalArrayBias[0] = 0;
		for (i = 1; i < P; i++)
			FinalArrayBias[i] = FinalArrayBias[i-1] + FinalArrayLength[i-1];
		
		for (i = 0; i < TotalRecv; i++)
			FinalArray[i] = fina[i];
		for (i = 1; i < P; i++)
			MPI_Recv(FinalArray + FinalArrayBias[i], FinalArrayLength[i], MPI_INT, i, 0, MPI_COMM_WORLD, &status);
		printf("FINAL RESULT:\n");
		for (i = 0; i < N; i++)
			printf("%d\t", FinalArray[i]);
		printf("\n");
	}
	MPI_Finalize();
}
