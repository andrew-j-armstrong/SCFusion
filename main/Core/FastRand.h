#ifndef RAND_SSE_H
#define RAND_SSE_H

#include "emmintrin.h"

#define RAND_SSE_COMPATABILITY
//define this if you wish to return values similar to the standard rand();

// The honest upper bound of rand_sse()'s output (15 bits). The GA code used
// to say RAND_MAX here, which happened to be the same number on MSVC and a
// spectacularly different number on glibc — turning the roulette wheel of
// evolution into a roulette wheel of nothing.
#define RAND_SSE_MAX 0x7FFF

void srand_sse( unsigned int seed );
short rand_sse();
void rand_sse( unsigned int* );

#endif
