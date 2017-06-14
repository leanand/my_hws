#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define MAXLINESIZE 35

__device__ int dNUM;
__device__ int dBLOCK_N;
__constant__ int dTHREAD_N;

// NUmber of threads = 96, Number of SM = 2, Number of cores per SM = 48 
__global__ 
void calculate_triliteration(float *dda, float *ddb, float *ddc, float *dx0, float *dy0){
	extern __shared__ float temp[];
	int blockId = blockIdx.x;
	int threadId = threadIdx.x;
	int idx = blockId * dTHREAD_N + threadId;
	int totalThreads = dTHREAD_N * dBLOCK_N;
	for(int i = idx; i < dNUM; i += totalThreads){
		//printf("%d\n", i);
		temp[threadId] = dda[i] + ddb[i] + ddc[i];
		__syncthreads();
		if(i % 4 == 0){
			dx0[i/4] = temp[threadId] + temp[threadId + 1] + temp[threadId + 2] + temp[threadId + 3] ;				
		}		
		__syncthreads();
		//dy0[idx] = idx;
	}
}


int main(int args, char ** argv){
	if(args != 5){
		printf("Invalid input.....\n");	
		return -1;
	}
	int NUM = pow(2, atoi(argv[1]));
	int BLOCK_N = atoi(argv[2]);
	int THREAD_N = atoi(argv[3]);
	char *INPUT_FILE = argv[4];
	printf("\n\tInput Size : %d\n", NUM);
	printf("\tBlock_N  : %d\n", BLOCK_N);
	printf("\tTHREAD_N : %d\n", THREAD_N);
	
	FILE * input_fd;
	input_fd = fopen(INPUT_FILE, "r");
	char line[MAXLINESIZE];
	int line_count = 0;
	float da[NUM],db[NUM],dc[NUM], x0[NUM / 4], y0[NUM/ 4];
	while ( fgets(line, MAXLINESIZE, input_fd) != NULL && line_count < NUM) {
		da[line_count] = 0.0;
		db[line_count] = 0.0;
		dc[line_count] = 0.0;
		sscanf(line, "%f %f %f\n", &da[line_count], &db[line_count], &dc[line_count]);
		da[line_count] = 1.0; db[line_count] = 1.0; dc[line_count] = 1.0;
		//printf("da = %f, db = %f, dc = %f\n", da[line_count], db[line_count], dc[line_count]);
		line_count ++;
	}
		
	float * dda, * ddb, * ddc;
	float * dx0, * dy0;
	int _floatSize = NUM * sizeof(float);
	cudaMalloc( (void**)&dda, _floatSize);
	cudaMalloc( (void**)&ddb, _floatSize);
	cudaMalloc( (void**)&ddc, _floatSize);
	cudaMalloc( (void**)&dx0, _floatSize / 4);
	cudaMalloc( (void**)&dy0, _floatSize / 4);
	cudaMemcpyToSymbol(dNUM, &NUM, sizeof(int));
	cudaMemcpyToSymbol(dBLOCK_N, &BLOCK_N, sizeof(int));
	cudaMemcpyToSymbol(dTHREAD_N, &THREAD_N, sizeof(int));
	
	cudaMemcpy( dda, da, _floatSize, cudaMemcpyHostToDevice ); 
	cudaMemcpy( ddb, db, _floatSize, cudaMemcpyHostToDevice );	
	cudaMemcpy( ddc, dc, _floatSize, cudaMemcpyHostToDevice );
	calculate_triliteration<<<BLOCK_N, THREAD_N, THREAD_N * sizeof(float)>>>(dda, ddb, ddc, dx0, dy0);
	cudaMemcpy( x0, dx0, _floatSize / 4, cudaMemcpyDeviceToHost);
	//cudaMemcpy( y0, dy0, _floatSize, cudaMemcpyDeviceToHost);
	for(int i = 0; i < NUM / 4; i ++){
		printf("%f\n", x0[i]);	
	}
	cudaFree(dda);
	cudaFree(ddb);
	cudaFree(ddc);
	cudaFree(dx0);
	cudaFree(dy0);
	
	
	fclose(input_fd);
	
	return 1;
}