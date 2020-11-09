#pragma once
#ifdef __cplusplus
#define NS_STD std::
#define EXTERN_C extern "C"
#define verify(bool_expr) if (!(bool_expr)) {std::cerr << "Error in " << __FILE__ << " (" << __LINE__ << ")\n"; std::abort();}
#else
#define NS_STD
#define EXTERN_C
#define verify(bool_expr) if (!(bool_expr)) {fprintf(stderr, "Error in %s (%d)\n", __FILE__, __LINE__); abort();}
#endif