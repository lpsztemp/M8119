#include <omp.h>
#include "omp.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>

unsigned omp_accumulate_false_sharing(const unsigned* V, size_t count)
{
	unsigned full_sum = 0, *partial_sums;
	unsigned T;
	#pragma omp parallel shared(T, full_sum)
	{
		size_t t = (size_t) omp_get_thread_num();
		#pragma omp single
		{
			T = omp_get_num_threads();
			partial_sums = (unsigned*) calloc(T, sizeof(unsigned));
			verify(partial_sums);
		}
		for (size_t i = t; i < count; i += T)
			partial_sums[t] += V[i];
	}
	for (int i = 0; i < T; ++i)
		full_sum += partial_sums[i];
	free(partial_sums);
	return full_sum;
}

#ifndef _MSC_VER
#include <stdalign.h>
#define _aligned_free(x) free((x))
#else //_MSC_VER
#define alignas(val) __declspec(align(val))
#define aligned_alloc(alignment, size) _aligned_malloc((size), (alignment))
#endif //_MSC_VER

#define CACHE_LINE 64
struct aligned_unsigned
{
	alignas(CACHE_LINE) unsigned value;
};

unsigned omp_accumulate_aligned(const unsigned* V, size_t count)
{
	unsigned full_sum = 0;
	struct aligned_unsigned* partial_sums;
	unsigned T;
	#pragma omp parallel shared(T)
	{
		size_t t = (size_t) omp_get_thread_num();
		#pragma omp single
		{
			T = omp_get_num_threads();
			partial_sums = (struct aligned_unsigned*) aligned_alloc(CACHE_LINE, T * sizeof(struct aligned_unsigned));
			verify(partial_sums);
		}
		partial_sums[t].value = 0;
		for (size_t i = t; i < count; i += T)
			partial_sums[t].value += V[i];
	}
	for (int i = 0; i < T; ++i)
		full_sum += partial_sums[i].value;
	_aligned_free(partial_sums);
	return full_sum;
}

unsigned omp_accumulate_atomic(const unsigned* V, size_t count)
{
	unsigned sum = 0;
	int i;
	#pragma omp for
	for (i = 0; i < count; ++i)
		#pragma omp atomic
			sum += V[i];
	return sum;
}

unsigned omp_accumulate_critical(const unsigned* V, size_t count)
{
	unsigned sum = 0;
	#pragma omp parallel
	{
		unsigned local_sum = 0;
		for (size_t i = (size_t) omp_get_thread_num(); i < count; i += (size_t) omp_get_num_threads())
			local_sum += V[i];
		#pragma omp critical
		{
			sum += local_sum;
		}
	}
	return sum;
}

unsigned omp_accumulate_reduction_static(const unsigned* V, size_t count)
{
	unsigned S = 0;
	int i;
	#pragma omp parallel for reduction(+:S) schedule(static)
	for (i = 0; i < count; ++i)
		S += V[i];
	return S;
}

unsigned omp_accumulate_reduction_dynamic(const unsigned* V, size_t count)
{
	unsigned S = 0;
	int i;
	#pragma omp parallel for reduction(+:S) schedule(dynamic)
	for (i = 0; i < count; ++i)
		S += V[i];
	return S;
}
