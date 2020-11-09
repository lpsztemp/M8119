#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <thread>
#include "common.h"
#include "experiment.h"

using std::size_t;

#include <vector>
unsigned cpp_accumulate_false_sharing(const unsigned* V, size_t count)
{
	unsigned full_sum = 0;
	unsigned T = get_num_threads();
	std::vector<std::thread> threads;
	std::vector<unsigned> partial_sums(T, 0);
	for (unsigned t = 0; t < T; t++)
		threads.emplace_back([t, T, V, count, &partial_sums]()
	{
		for (size_t i = t; i < count; i += T)
			partial_sums[t] += V[i];
	});
	for (auto& thread:threads)
		thread.join();
	for (int i = 0; i < T; ++i)
		full_sum += partial_sums[i];
	return full_sum;
}

#include <memory>
constexpr std::size_t CACHE_LINE = std::hardware_destructive_interference_size;
struct aligned_unsigned
{
	alignas(CACHE_LINE) unsigned value;
};

unsigned cpp_accumulate_aligned(const unsigned* V, size_t count)
{
	unsigned full_sum = 0;
	std::vector<std::thread> threads;
	unsigned T = get_num_threads();
	std::vector<aligned_unsigned> partial_sums = std::vector<aligned_unsigned>(T);
	for (unsigned t = 0; t < T; t++)
		threads.emplace_back([t, T, V, count, &partial_sums]()
	{
		for (size_t i = t; i < count; i += T)
			partial_sums[t].value += V[i];
	});
	for (auto& thread:threads)
		thread.join();
	for (int i = 0; i < T; ++i)
		full_sum += partial_sums[i].value;
	return full_sum;
}

#include <atomic>
unsigned cpp_accumulate_atomic(const unsigned* V, size_t count)
{
	std::atomic<unsigned> sum = std::atomic<unsigned>{0};
	unsigned T = get_num_threads();
	std::vector<std::thread> threads;
	for (unsigned t = 0; t < T; ++t)
		threads.emplace_back([t, T, V, count,  &sum]()
	{
		unsigned local_sum = 0;
		for (size_t i = t; i < count; i += T)
			local_sum += V[i];
		sum += local_sum;
	});
	for (auto& thread:threads)
		thread.join();
	return sum;
} //lock-free

#include <mutex>
unsigned cpp_accumulate_critical(const unsigned* V, size_t count)
{
	unsigned sum = 0;
	unsigned T = get_num_threads();
	std::vector<std::thread> threads;
	std::mutex mtx;
	for (unsigned t = 0; t < T; ++t)
		threads.emplace_back([t, T, V, count, &sum, &mtx]()
	{
		unsigned local_sum = 0;
		for (size_t i = t; i < count; i += T)
			local_sum += V[i];
		{
			std::scoped_lock lock(mtx);
			//std::lock lock(mtx);
			//mtx.lock();
			sum += local_sum;
			//mtx.unlock();
		}
	});
	for (auto& thread:threads)
		thread.join();
	return sum;
}

#include <numeric>
#include <execution>
unsigned cpp_accumulate_reduction(const unsigned* V, size_t count) //C++17
{
	return std::reduce(std::execution::parallel_policy{}, &V[0], &V[count]/*, how-to-reduce*/);
}

/*
#include <barrier>
using std::barrier;
*/
#include <condition_variable>
class barrier
{
	std::mutex mtx;
	std::condition_variable cv;
	const unsigned init_count;
	unsigned count;
	bool current_barrier_id = false;
public:
	explicit barrier(unsigned T):init_count(T), count(T) {}
	void arrive_and_wait()
	{
		std::unique_lock lock{mtx};
		if (--count == 0)
		{
			count = init_count;
			current_barrier_id = !current_barrier_id;
			cv.notify_all();
		}else
		{
			bool my_barrier_id = current_barrier_id;
			do
			{
				cv.wait(lock);
			}while (my_barrier_id == current_barrier_id);
		}
	}
};

unsigned cpp_accumulate_reduction_manual(const unsigned* V, size_t count)
{
	unsigned full_sum = 0;
	std::vector<std::thread> threads;
	unsigned T = get_num_threads();
	std::vector<aligned_unsigned> partial_sums = std::vector<aligned_unsigned>(T);
	barrier bar{T};
	
	for (unsigned t = 0; t < T; t++)
		threads.emplace_back([t, T, V, count, &bar, &partial_sums]()
	{
		for (size_t i = t; i < count; i += T)
			partial_sums[t].value += V[i];
		std::size_t s = 1;
		while (s < T)
		{
			bar.arrive_and_wait();
			if (t % (s * 2) == 0 && t + s < T)
				partial_sums[t].value += partial_sums[t + s].value;
			s *= 2;
		}
	});
	for (auto& thread:threads)
		thread.join();
	return partial_sums[0].value;
}
