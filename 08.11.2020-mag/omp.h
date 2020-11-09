#pragma once
#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif
#include "common.h"

EXTERN_C unsigned omp_accumulate_false_sharing(const unsigned* V, NS_STD size_t count);
EXTERN_C unsigned omp_accumulate_aligned(const unsigned* V, NS_STD size_t count);
EXTERN_C unsigned omp_accumulate_atomic(const unsigned* V, NS_STD size_t count);
EXTERN_C unsigned omp_accumulate_critical(const unsigned* V, NS_STD size_t count);
EXTERN_C unsigned omp_accumulate_reduction_static(const unsigned* V, NS_STD size_t count);
EXTERN_C unsigned omp_accumulate_reduction_dynamic(const unsigned* V, NS_STD size_t count);
