#include<omp.h>
#include<stdio.h>
static long num_steps=100000;
#define NUM_THREADS 2
void main()
{
	int i;
	double x,pi=0,sum;
	double step=1.0/num_steps;
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
	printf("PI :%lf",pi);

}
