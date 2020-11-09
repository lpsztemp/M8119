#pragma once
#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif
#include "common.h"

typedef struct experiment_result_
{
	unsigned result;
	double time_ms;
	double speedup;
}experiment_result;

EXTERN_C void set_num_threads(unsigned T);
EXTERN_C unsigned get_num_threads();

EXTERN_C experiment_result* run_experiments(const unsigned* V, NS_STD size_t count, unsigned (*accumulate) (const unsigned*, NS_STD size_t));

EXTERN_C unsigned* generate_vector(NS_STD size_t count);
EXTERN_C void print_experiment_results(const experiment_result* results);
EXTERN_C void run_experiments_for(const unsigned* V, NS_STD size_t count, unsigned (*accumulate) (const unsigned*, NS_STD size_t));
