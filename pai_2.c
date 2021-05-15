#include<omp.h>
#include<stdio.h>
static long num_steps=100000;
#define NUM_THREADS 2
void main()
{
	int i;
	double x,pi,sum[NUM_THREADS];
	double step=1.0/num_steps;
	omp_set_num_threads(NUM_THREADS);
	#pragma omp parallel private(i,x) shared(sum)
	{
		int id=omp_get_thread_num();
		sum[id]=0;
		#pragma omp for
		for(i=0;i<num_steps;i++)
		{
			x=(i+0.5)*step;
			sum[id]+=4.0/(1.0+x*x);
		}
	}
	for(i=0,pi=0;i<NUM_THREADS;i++)
		pi+=sum[i]*step;
	printf("PI : %lf", pi);
}
