#ifdef COMPILATION // -*-indent-tabs-mode:t;c-basic-offset:4;tab-width:4;-*-
sudo cpupower frequency-set --governor performance; cmake -DCMAKE_CUDA_COMPILER=/usr/local/cuda-11.1/bin/nvcc -DCMAKE_BUILD_TYPE=Release .. && make $0.x && ctest ; exit
#endif // © Alfredo A. Correa 2021

#include <benchmark/benchmark.h>

#include <thrust/system/cuda/memory.h>
#include <thrust/system/cuda/execution_policy.h>

#include <multi/array.hpp>

#include<execution>
#include<numeric>

namespace multi = boost::multi;

template<class T, class X, class Y>
Y&& axpy_cpu_transform(T alpha, X const& x, Y&& y){
	assert( size(x) == size(y) );
	std::transform( begin(x), end(x), begin(y), begin(y), [alpha](auto const& a, auto const& b){return a + alpha*b;} );
	return std::forward<Y>(y);
}

static void BM_axpy_cpu_transform(benchmark::State& state){

	using T = double;
	using alloc = std::allocator<double>;
	multi::array<T, 2, alloc> const X({1<<27, 2}, 1.);
	multi::array<T, 2, alloc>       Y(extensions(X), 1.);

	auto&& x = (~X)[0];
	auto&& y = (~Y)[0];

	for(auto _ : state){
		axpy_cpu_transform(2.0, x, y);
		benchmark::DoNotOptimize(base(y)); benchmark::ClobberMemory();
	}

	state.SetBytesProcessed(state.iterations()*size(x)*2.*sizeof(T));
	state.SetItemsProcessed(state.iterations()*size(x)             );
}
BENCHMARK(BM_axpy_cpu_transform);

template<class T> __global__ void axpy_gpu_legacy_kernel(int n, T alpha, T const* x, int x_stride, T* y, int y_stride){
	int i = blockIdx.x*blockDim.x + threadIdx.x;
	if(i < n){*(y + i*y_stride) = alpha* *(x + i*x_stride) + *(y + i*y_stride);}
}

template<class T, class X, class Y>
Y&& axpy_gpu_legacy(T alpha, X const& x, Y&& y){
	assert( size(x) == size(y) );
	axpy_gpu_legacy_kernel<<<(size(x) + 255) / 256, 256>>>(size(x), alpha, raw_pointer_cast(base(x)), stride(x), raw_pointer_cast(base(y)), stride(y));
	return std::forward<Y>(y);
}

static void BM_axpy_gpu_legacy_kernel(benchmark::State& state){

	using T = double;
	using alloc = thrust::system::cuda::allocator<double>;
	multi::array<T, 2, alloc> const X({1<<27, 2}, 1.);
	multi::array<T, 2, alloc>       Y(extensions(X), 1.);

	auto&& x = (~X)[0];
	auto&& y = (~Y)[0];

	for(auto _ : state){
		axpy_gpu_legacy(2.0, x, y);
		cudaDeviceSynchronize(); benchmark::DoNotOptimize(y.base()); benchmark::ClobberMemory();
	}

	state.SetBytesProcessed(state.iterations()*size(x)*2.*sizeof(T));
	state.SetItemsProcessed(state.iterations()*size(x)             );
}
BENCHMARK(BM_axpy_gpu_legacy_kernel);


template<class T, class ItX, class ItY> __global__ void axpy_gpu_multi_kernel(int n, T alpha, ItX x, ItY y){
	int i = blockIdx.x*blockDim.x + threadIdx.x;
	if(i < n){y[i] = alpha*x[i] + y[i];}
}

template<class T, class X, class Y>
Y&& axpy_gpu_multi_kernel(T alpha, X const& x, Y&& y){
	assert( size(x) == size(y) );
	axpy_gpu_multi_kernel<<<(size(x) + 255) / 256, 256>>>(size(x), alpha, begin(x), begin(y));
	return std::forward<Y>(y);
}

static void BM_axpy_gpu_multi_kernel(benchmark::State& state){

	using T = double;
	using alloc = thrust::system::cuda::allocator<double>;
	multi::array<T, 2, alloc> const X({1<<27, 2}, 1.);
	multi::array<T, 2, alloc>       Y(extensions(X), 1.);

	auto&& x = (~X)[0];
	auto&& y = (~Y)[0];

	for(auto _ : state){
		axpy_gpu_multi_kernel(2.0, x, y);
		cudaDeviceSynchronize(); benchmark::DoNotOptimize(base(y)); benchmark::ClobberMemory();
	}

	state.SetBytesProcessed(state.iterations()*size(x)*2.*sizeof(T));
	state.SetItemsProcessed(state.iterations()*size(x)             );
}
BENCHMARK(BM_axpy_gpu_multi_kernel);

template<class T, class X, class Y>
Y&& axpy_gpu_multi_thrust(T alpha, X const& x, Y&& y){
	thrust::transform(thrust::cuda::par, begin(x), end(x), begin(y), begin(y), [alpha] __device__ (auto a, auto b){return a + alpha*b;} );
	return std::forward<Y>(y);
}

static void BM_axpy_gpu_multi_thrust(benchmark::State& state){

	using T = double;
	using alloc = thrust::system::cuda::allocator<double>;
	multi::array<T, 2, alloc> const X({1<<27, 2}, 1.);
	multi::array<T, 2, alloc>       Y(extensions(X), 1.);

	auto&& x = (~X)[0];
	auto&& y = (~Y)[0];

	for(auto _ : state){
		axpy_gpu_multi_thrust(2.0, x, y);
		cudaDeviceSynchronize(); benchmark::DoNotOptimize(base(y)); benchmark::ClobberMemory();
	}

	state.SetBytesProcessed(state.iterations()*size(x)*2.*sizeof(T));
	state.SetItemsProcessed(state.iterations()*size(x)             );
}
BENCHMARK(BM_axpy_gpu_multi_thrust);

BENCHMARK_MAIN();

