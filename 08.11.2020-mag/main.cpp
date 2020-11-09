#include "cpp.h"
#include "omp.h"
#include "experiment.h"
#include <cstdio>

unsigned accumuate_sequential(const unsigned* V, size_t count)
{
	unsigned i, S = 0;
	for (i = 0; i < count; ++i)
		S += V[i];
	return S;
}

int main(int argc, char** argv)
{
	size_t element_count = 10000000u;
	unsigned* V = generate_vector(element_count);
	using std::printf;

	printf("Control: %u\n", accumuate_sequential(V, element_count));
	printf("==False sharing==\n");
	run_experiments_for(V, element_count, omp_accumulate_false_sharing);
	printf("==Alignment==\n");
	run_experiments_for(V, element_count, omp_accumulate_aligned);
	printf("==Atomic==\n");
	run_experiments_for(V, element_count, omp_accumulate_atomic);
	printf("==Critical==\n");
	run_experiments_for(V, element_count, omp_accumulate_critical);
	printf("==Reduction (static)==\n");
	run_experiments_for(V, element_count, omp_accumulate_reduction_static);
	printf("==Reduction (dynamic)==\n");
	run_experiments_for(V, element_count, omp_accumulate_reduction_dynamic);

	printf("==False sharing==\n");
	run_experiments_for(V, element_count, cpp_accumulate_false_sharing);
	printf("==Alignment==\n");
	run_experiments_for(V, element_count, cpp_accumulate_aligned);
	printf("==Atomic==\n");
	run_experiments_for(V, element_count, cpp_accumulate_atomic);
	printf("==Critical==\n");
	run_experiments_for(V, element_count, cpp_accumulate_critical);
	printf("==Reduction (auto)==\n");
	run_experiments_for(V, element_count, cpp_accumulate_reduction);
	printf("==Reduction (manual)==\n");
	run_experiments_for(V, element_count, cpp_accumulate_reduction_manual);
	return 0;
}