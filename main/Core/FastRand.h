#ifndef RAND_SSE_H
#define RAND_SSE_H

#include "emmintrin.h"

#define RAND_SSE_COMPATABILITY
//define this if you wish to return values similar to the standard rand();

void srand_sse( unsigned int seed );
short rand_sse();
void rand_sse( unsigned int* );

#endif
