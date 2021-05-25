#include<omp.h>
#include<stdio.h>
#include<time.h>
static long num_steps=100000;
#define NUM_THREADS 2
void main()
{
	int i;
	double x,pi=0,sum;
	double step=1.0/num_steps;
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
	omp_set_num_threads(NUM_THREADS);
	#pragma omp parallel private(i,x,sum)
	{
		int id=omp_get_thread_num();
		for(i=id,sum=0;i<num_steps;i=i+NUM_THREADS)
		{
			x=(i+0.5)*step;
			sum+=4.0/(1.0+x*x);
		}
		#pragma omp critical
		pi+=sum*step;
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	printf("PI :%lf\nTime: %lfs\n",pi, (end.tv_sec-start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1000000000.0);

}
