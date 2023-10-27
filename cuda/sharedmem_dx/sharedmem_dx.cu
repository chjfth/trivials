#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "libcode.h"

__global__ void mykernel() 
{
	int idx = threadIdx.x;
	printf("gpu_sbin[%d] = %d\n", idx, gpu_sbin[idx]);
}

void myPrintGpuInfo()
{
	int count = 0;
	cudaError_t cuerr = cudaGetDeviceCount( &count );
	if(count==0)
	{
		printf("No NVIDIA GPU found. This program cannot continue.\n");
		exit(2);
	}

	int cudaver = 0;
	cudaRuntimeGetVersion(&cudaver);
	cudaDeviceProp  prop = {};
	cudaGetDeviceProperties( &prop, 0 );
	printf("GPU: %s, CUDA ver: %d\n", prop.name, cudaver);
}


int main(int argc, char* argv[])
{
	int threads = BIN256;
	if(argc>1)
		threads = strtoul(argv[1], nullptr, 0);

	printf("Hello, sharedmem_dx!\n");

	myPrintGpuInfo();

	mykernel<<<1, threads>>>();

	cudaError_t kerr = cudaDeviceSynchronize();
	if(kerr)
		printf("CUDA kernel call error(%d): %s\n", kerr, cudaGetErrorName(kerr));
	else
		printf("CUDA kernel call OK.\n");

	printf("Done. (press any key)\n");
	_getch();
}
