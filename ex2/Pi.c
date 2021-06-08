#include<stdio.h>
#include<time.h>
#include"mpi.h"

static long num_steps = 100000;
double step;
void main(int argc, char * argv[])
{
	int i, id, num;
	struct timespec start, end;
	double pi=0.0, x, sum;
	
	clock_gettime(CLOCK_MONOTONIC, &start);
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &num);
	MPI_Status status;
	step = 1.0/(double)num_steps;
	if (id != 0)
	{
		for (i = id; i <= num_steps; i += (num - 1))
		{
			x = (i - 0.5) * step;
			sum += 4.00/(1.0 + x * x);
		}
		MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
	else
	{
		for (i = 1; i < num; i++)
		{
			MPI_Recv(&sum, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
			pi += sum * step;
		}
		clock_gettime(CLOCK_MONOTONIC, &end);
		printf("Pi is %f\nTime is %lfs\n", pi, (end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)/1000000000.0);
	}
	MPI_Finalize();
}
