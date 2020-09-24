#include "Kernels.cuh"
#include <iostream>
#include <cuda.h>
#include <device_launch_parameters.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#ifdef __CUDACC__
#define KERNEL_ARGS2(grid, block) <<< grid, block >>>
#define KERNEL_ARGS3(grid, block, sh_mem) <<< grid, block, sh_mem >>>
#define KERNEL_ARGS4(grid, block, sh_mem, stream) <<< grid, block, sh_mem, stream >>>
#else
#define KERNEL_ARGS2(grid, block)
#define KERNEL_ARGS3(grid, block, sh_mem)
#define KERNEL_ARGS4(grid, block, sh_mem, stream)
#endif

#define CU_SAFE_CALL(call)\
		{\
			cudaError_t err = call;\
		    if( cudaSuccess != err)\
			{\
		        std::cerr <<"Cuda driver error " << cudaGetErrorString(err) <<" in file " << __FILE__" in line " << __LINE__ << std::endl;\
		        __debugbreak();\
		    }\
		}

__constant__
unsigned lookup[256];

__device__ __inline__
double Map(double input, double fromA, double fromB, double toA, double toB)
{
	return (input - fromA) * (toB - toA) / (fromB - fromA) + toA;
}

void SetLookup(const std::array<Color, 256>& palette)
{
	CU_SAFE_CALL(cudaMemcpyToSymbol(lookup, &*palette.begin(), 256 * sizeof(unsigned int)));
}

void InitCuda(const mb::Buffer& buffer)
{
	CU_SAFE_CALL(cudaGLRegisterBufferObject(buffer.GetId()));
}

__global__
void CalculateCuda(unsigned int* pixels, Bounds bounds, ScreenDim dim, int iterations)
{
	const int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx >= dim.width * dim.height) { return; }

	const int h = idx / dim.width;
	const int w = idx % dim.width;

	const double real = Map(w, 0, dim.width, bounds.left, bounds.right);
	const double imag = Map(h, 0, dim.height, bounds.down, bounds.up);

	const Complex num = { real, imag };

	Complex current = { 0,0 };
	unsigned int result = 0;

	for (int i = 0; i < iterations; ++i)
	{
		const Complex temp = current;
		current.real = temp.real * temp.real - temp.imag * temp.imag + num.real;
		current.imag = 2 * temp.real * temp.imag + num.imag;
		if (current.real * current.real + current.imag * current.imag > 4)
		{
			result = i;
			break;
		}
	}

	result = Map(result, 0, iterations, 0, 255);

	//result = result > 255 ? 100 : result;

	pixels[idx] = lookup[result];
}

void Calculate(unsigned int* pixels, Bounds bounds, ScreenDim dim, int iterations, const mb::Buffer& buffer)
{
	const int tpb = 256;
	const int blocks = dim.width * dim.height / tpb + 1;

	CU_SAFE_CALL(cudaGLMapBufferObject(reinterpret_cast<void**>(&pixels), buffer.GetId()));
	CalculateCuda KERNEL_ARGS2(blocks, tpb) (pixels, bounds, dim, iterations);
	CU_SAFE_CALL(cudaGLUnmapBufferObject(buffer.GetId()));
}
