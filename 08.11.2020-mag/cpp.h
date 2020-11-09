#pragma once
#include <cstdlib>

unsigned cpp_accumulate_false_sharing(const unsigned* V, std::size_t count);
unsigned cpp_accumulate_aligned(const unsigned* V, std::size_t count);
unsigned cpp_accumulate_atomic(const unsigned* V, std::size_t count);
unsigned cpp_accumulate_critical(const unsigned* V, std::size_t count);
unsigned cpp_accumulate_reduction(const unsigned* V, std::size_t count);
unsigned cpp_accumulate_reduction_manual(const unsigned* V, std::size_t count);
