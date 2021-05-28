#include<omp.h>
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<assert.h>
#define N 27
#define P 3

void InsertSort(int *a, int start, int end)
{
	assert(end >= start);
	int i, j, hook;
	for (i = start + 1; i <= end; i++)
	{
		hook = a[i];
		for (j = i - 1; j >= start; j--)
		{
			if (hook < a[j])
			{
				a[j+1] = a[j];
				if (j == start)
				{
					a[j] = hook;
					break;
				}
			}
			else
			{
				a[j+1] = hook;
				break;
			}
		}
	}
}
void main()
{
	int a[N], sam[P*P], keyelem[P-1] ,count[P]={0}, fina[N];
	int posl[P][P]={-1}, posr[P][P]={-1};
	int i;
	int start, end;
	struct timespec tstart, tend;
	for (i = 0; i < N; i++)
		a[i] = rand() % 100;
	clock_gettime(CLOCK_MONOTONIC, &tstart);
	omp_set_num_threads(P);	
	#pragma omp parallel private(start, end) shared(a, sam, keyelem, count, posl, posr, fina)
	{
		int id=omp_get_thread_num();
		int j, k=0, step, base;
		int left=0, right=0;
		start = id * (N/P);
		end = (id + 1) * (N/P) - 1;
		InsertSort(a, start, end);
		step = (end - start + 1)/P;
		for (j = start; j < end; j += step)
			sam[id*P + (j - start)/step] = a[j];
		#pragma omp barrier
		#pragma omp master
		{
			InsertSort(sam, 0, P*P-1);
			for (j = P; j < P * P; j += P)
				keyelem[j/P - 1] = sam[j];
		}
		#pragma omp barrier
		j = start;
		while (j <= end)
		{
			if (a[j] > keyelem[k])
			{
				k++;
				if (left > right)
					posr[id][right++] = j - 1;
				else
				{
					left++;
					right++;
				}
				if (k == P - 1)
				{
					posl[id][left] = j;
					posr[id][right] = end;
					break;  
				}
			}
			else if (left == right)
				posl[id][left++] = j++;	
			else
				j++;
		}
		if (left > right)
			posr[id][right++] = end;
		#pragma omp barrier
		for (j = 0; j < P; j++)
		{
			assert(posl[j][id] <= posr[j][id]);
			count[id] += posr[j][id] - posl[j][id] + 1;
		}
		#pragma omp barrier
		#pragma omp master
		{
			for (j = 1; j < P; j++)
				count[j] += count[j - 1];
		}
		#pragma omp barrier
		if (id == 0)
			base = 0;
		else 
			base = count[id - 1];
		k = base; 
		for (j = 0; j < P; j++)
		{
			if (posl[j][id] != -1 && posr[j][id] != -1)
			{
				memcpy(&fina[k], &a[posl[j][id]], sizeof(int) * (posr[j][id] - posl[j][id] + 1));
				k += posr[j][id] - posl[j][id] + 1;
			}
		}
		InsertSort(fina, base, k - 1);
	}
	
	clock_gettime(CLOCK_MONOTONIC, &tend);

	for (i = 0; i < N; i++)
		printf("%d\t", fina[i]);
	printf("\nTime: %lfs\n", (tend.tv_sec - tstart.tv_sec) + (tend.tv_nsec - tstart.tv_nsec)/1000000000.0);

}
