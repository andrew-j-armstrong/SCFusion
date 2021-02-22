#include "stdafx.h"
#include "FastRand.h"

__declspec( align(16) ) static __m128i cur_seed;

short rand_sse()
{
	unsigned int result[4];
	rand_sse(result);
	return (short)*result;
}

void srand_sse( unsigned int seed )
{
	cur_seed = _mm_set_epi32( seed, seed+1, seed, seed+1 );
}

void rand_sse( unsigned int* result )
{
	__declspec( align(16) ) __m128i cur_seed_split;
	__declspec( align(16) ) __m128i multiplier;
	__declspec( align(16) ) __m128i adder;
	__declspec( align(16) ) __m128i mod_mask;
	__declspec( align(16) ) __m128i sra_mask;
#ifdef RAND_SSE_COMPATABILITY
	__declspec( align(16) ) __m128i sseresult;
#endif

	__declspec( align(16) ) static const unsigned int mult[4] ={ 214013, 17405, 214013, 69069 };
	__declspec( align(16) ) static const unsigned int gadd[4] ={ 2531011, 10395331, 13737667, 1 };
	__declspec( align(16) ) static const unsigned int mask[4] ={ 0xFFFFFFFF, 0, 0xFFFFFFFF, 0 };
	__declspec( align(16) ) static const unsigned int masklo[4] ={ 0x00007FFF, 0x00007FFF, 0x00007FFF, 0x00007FFF };

	adder = _mm_load_si128( (__m128i*) gadd);
	multiplier = _mm_load_si128( (__m128i*) mult);
	mod_mask = _mm_load_si128( (__m128i*) mask);
	sra_mask = _mm_load_si128( (__m128i*) masklo);
	cur_seed_split = _mm_shuffle_epi32( cur_seed, _MM_SHUFFLE( 2, 3, 0, 1 ) );
	cur_seed = _mm_mul_epu32( cur_seed, multiplier );
	multiplier = _mm_shuffle_epi32( multiplier, _MM_SHUFFLE( 2, 3, 0, 1 ) );
	cur_seed_split = _mm_mul_epu32( cur_seed_split, multiplier );
	cur_seed = _mm_and_si128( cur_seed, mod_mask);
	cur_seed_split = _mm_and_si128( cur_seed_split, mod_mask );
	cur_seed_split = _mm_shuffle_epi32( cur_seed_split, _MM_SHUFFLE( 2, 3, 0, 1 ) );
	cur_seed = _mm_or_si128( cur_seed, cur_seed_split );
	cur_seed = _mm_add_epi32( cur_seed, adder);

#ifdef RAND_SSE_COMPATABILITY // Add the lines below if you wish to reduce your results to 16-bit vals...
	sseresult = _mm_srai_epi32( cur_seed, 16);
	sseresult = _mm_and_si128( sseresult, sra_mask );
	_mm_storeu_si128( (__m128i*) result, sseresult );
	return;
#endif
	_mm_storeu_si128( (__m128i*) result, cur_seed);
	return;
}

