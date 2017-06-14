#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define MAXLINESIZE 100

__device__ int dNUM;
__device__ int dBLOCK_N;
__device__ int dTHREAD_N;
__device__ float dx1;
__device__ float dy1;
__device__ float dx2;
__device__ float dy2;
__device__ float dx3;
__device__ float dy3;

// NUmber of threads = 96, Number of SM = 2, Number of cores per SM = 48 
__global__ 
void calculate_triliteration(float *dda, float *ddb, float *ddc, float *dx, float *dy){
	extern __shared__ float temp[];
	float *tempX = temp;
	float *tempY = (float *)&tempX[dTHREAD_N];
	
	int blockId = blockIdx.x;
	int threadId = threadIdx.x;
	
	int idx = blockId * dTHREAD_N + threadId;
	int totalThreads = dTHREAD_N * dBLOCK_N;
	
	float dx1dx2 = pow(dx1, 2) - pow(dx2, 2);
	float dy1dy2 = pow(dy1, 2) - pow(dy2, 2);
	
	float dx1dx3 = pow(dx1, 2) - pow(dx3, 2);
	float dy1dy3 = pow(dy1, 2) - pow(dy3, 2);
	
	float dy2dy1 = 2 * (dy2 - dy1);
	float dy3dy1 = 2 * (dy3 - dy1);
	
	float dx2dx1 = 2 * (dx2 - dx1);
	float dx3dx1 = 2 * (dx3 - dx1);
	
	float denX = dx2dx1 * dy3dy1 - dx3dx1 * dy2dy1; 	
	
	
	 
	for(int i = idx; i < dNUM; i += totalThreads){
		if(denX == 0){
			if(i % 4 == 0){
				dx[i/4] = 0;
				dy[i/4] = 0;		
			}
			return;
		}
		float ddaddb = pow(dda[i], 2) - pow(ddb[i], 2);
		float ddaddc = pow(dda[i], 2) - pow(ddc[i], 2);
		
		float numX = (ddaddb - dx1dx2 - dy1dy2) * dy3dy1 - (ddaddc - dx1dx3 - dy1dy3) * dy2dy1;
		float numY = (ddaddc - dx1dx3 - dy1dy3) * dx2dx1 - (ddaddb - dx1dx2 - dy1dy2) * dx3dx1;
		tempX[threadId] = numX / denX;
		tempY[threadId] = numY / denX;
/*		printf("The output is => %f %f\n", tempX[threadId], tempY[threadId]);*/
		__syncthreads();
		if(i % 4 == 0){
			dx[i/4] = (tempX[threadId] + tempX[threadId + 1] + tempX[threadId + 2] + tempX[threadId + 3] ) / 4;
			dy[i/4] = (tempY[threadId] + tempY[threadId + 1] + tempY[threadId + 2] + tempY[threadId + 3] ) / 4;				
		}
		__syncthreads();
		
	}
}


int main(int args, char ** argv){
	clock_t begin = clock();
	if(args != 6){
		printf("Invalid Arguments\nUsage: ./triliteration <NUM> <BLOCK_N> <THREAD_N> <INPUT_FILE> <CHECK_FILE>\n");	
		return -1;
	}
	int NUM = pow(2, atoi(argv[1]));
	int BLOCK_N = atoi(argv[2]);
	int THREAD_N = atoi(argv[3]);
	char *INPUT_FILE = argv[4];
	char *CHECK_FILE = argv[5];
	printf("\n\tInput Size : %d\n", NUM);
	printf("\tBlock_N  : %d\n", BLOCK_N);
	printf("\tTHREAD_N : %d\n", THREAD_N);
	
	FILE * input_fd, * check_fd;
	input_fd = fopen(INPUT_FILE, "r");
	char line[MAXLINESIZE];
	int line_count = 0;
	float da[NUM],db[NUM],dc[NUM], x[NUM / 4], y[NUM / 4];
	float x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
	
	if(fgets(line, MAXLINESIZE, input_fd) != NULL){
		sscanf(line, "%f %f %f %f %f %f\n", &x1, &y1, &x2, &y2, &x3, &y3);	
	}
	/*x1 = 0.0;
	y1 = 0.0;
	x2 = 20000.0;
	y2 = 20000.0;
	x3 = 40000.0;
	y3 = 0.0;*/
	printf("\nThe Guard points:\n\t%f, %f \n\t%f, %f\n\t%f, %f\n", x1, y1, x2, y2, x3, y3);
	
	while ( fgets(line, MAXLINESIZE, input_fd) != NULL && line_count < NUM) {
		da[line_count] = 0.0;
		db[line_count] = 0.0;
		dc[line_count] = 0.0;
		/*printf("The line is %s\n",line );*/
		sscanf(line, "%f %f %f\n", &da[line_count], &db[line_count], &dc[line_count]);
		/*printf("the values are => %f %f %f\n", da[line_count], db[line_count], dc[line_count]);*/
		//da[line_count] = 3.0; db[line_count] = 3.0; dc[line_count] = 3.0;
		line_count ++;
	}
		
	float * dda, * ddb, * ddc;
	float * dx, * dy;
	int _floatSize = NUM * sizeof(float);
	cudaMalloc( (void**)&dda, _floatSize);
	cudaMalloc( (void**)&ddb, _floatSize);
	cudaMalloc( (void**)&ddc, _floatSize);
	cudaMalloc( (void**)&dx, _floatSize / 4);
	cudaMalloc( (void**)&dy, _floatSize / 4);
	
	cudaMemcpyToSymbol(dNUM, &NUM, sizeof(int));
	cudaMemcpyToSymbol(dBLOCK_N, &BLOCK_N, sizeof(int));
	cudaMemcpyToSymbol(dTHREAD_N, &THREAD_N, sizeof(int));
	
	cudaMemcpy( dda, da, _floatSize, cudaMemcpyHostToDevice ); 
	cudaMemcpy( ddb, db, _floatSize, cudaMemcpyHostToDevice );	
	cudaMemcpy( ddc, dc, _floatSize, cudaMemcpyHostToDevice );
	
	cudaMemcpyToSymbol( dx1, &x1, sizeof(float));
	cudaMemcpyToSymbol( dy1, &y1, sizeof(float));
	cudaMemcpyToSymbol( dx2, &x2, sizeof(float));
	cudaMemcpyToSymbol( dy2, &y2, sizeof(float));
	cudaMemcpyToSymbol( dx3, &x3, sizeof(float));
	cudaMemcpyToSymbol( dy3, &y3, sizeof(float));
	
	calculate_triliteration<<<BLOCK_N, THREAD_N, 2 * THREAD_N * sizeof(float)>>>(dda, ddb, ddc, dx, dy);
	cudaMemcpy( x, dx, _floatSize / 4, cudaMemcpyDeviceToHost);
	cudaMemcpy( y, dy, _floatSize / 4, cudaMemcpyDeviceToHost);
	printf("Legend : \n\t. => difference less than 0.1,\n\tX => difference greater than 0.1 \n");
	check_fd = fopen(CHECK_FILE, "r");
	float avgX = 0.0, avgY= 0.0;
	//printf("Original X and Y\t Calculated X and Y\n");
	for(int i = 0; i < NUM / 4; i ++){
		fgets(line, MAXLINESIZE, check_fd);
		sscanf(line, "%f %f", &avgX, &avgY);
		if(abs(x[i] - avgX) <= 0.1 && abs(y[i] - avgY) <= 0.1){
			printf(".");	
		}else{
			printf("X");		
		}
		//printf("%f, %f\t%f, %f\n", x[i], y[i], avgX, avgY);
	
	}
	cudaFree(dda);
	cudaFree(ddb);
	cudaFree(ddc);
	cudaFree(dx);
	cudaFree(dy);
	
	
	fclose(input_fd);
	fclose(check_fd);
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("\nExecution time is %f\n", time_spent);
	return 1;
}