#include "experiment.h"
#include <omp.h>
#include <chrono>
#include <memory>
#include <thread>
#include <iostream>
#include <cstdio>

static unsigned thread_num = 1;

EXTERN_C void set_num_threads(unsigned T)
{
	thread_num = T;
	omp_set_num_threads((int) T);
}

EXTERN_C unsigned get_num_threads()
{
	return thread_num;
}

EXTERN_C experiment_result* run_experiments(const unsigned* V, size_t count, unsigned (*accumulate) (const unsigned*, size_t))
{
	unsigned P = (unsigned) std::thread::hardware_concurrency();
	experiment_result* results = (experiment_result*) std::malloc(P * sizeof(experiment_result));
	verify(results);
	for (unsigned i = 0; i < P; ++i)
	{
		auto tm_start = std::chrono::steady_clock::now();
		set_num_threads(i + 1);
		results[i].result = accumulate(V, count);
		results[i].time_ms = (double) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - tm_start).count();
		results[i].speedup = results[0].time_ms / results[i].time_ms;
	}
	return results;
}

EXTERN_C unsigned* generate_vector(size_t count)
{
	unsigned* V = (unsigned*) std::malloc(count * sizeof(unsigned));
	verify(V);
	for (size_t i = 0; i < count; ++i)
		V[i] = i + 1;
	return V;
}

EXTERN_C void print_experiment_results(const experiment_result* results)
{
	unsigned P = (unsigned) std::thread::hardware_concurrency();
	std::printf("%10s\t%10s\t%10s\n", "Result", "Time", "Speedup");
	for (unsigned i = 0; i < P; ++i)
		std::printf("%10u\t%10lgms\t%10lg\n", results[i].result, results[i].time_ms, results[i].speedup);
}

EXTERN_C void run_experiments_for(const unsigned* V, size_t count, unsigned (*accumulate) (const unsigned*, size_t))
{
	experiment_result* results = run_experiments(V, count, accumulate);
	print_experiment_results(results);
	std::free(results);
}