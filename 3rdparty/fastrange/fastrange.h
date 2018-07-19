/**
* Fair maps to intervals without division.
* Reference : http://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
*
* (c) Daniel Lemire
* Apache License 2.0
*/
#ifndef INCLUDE_FASTRANGE_H
#define INCLUDE_FASTRANGE_H
#include <iso646.h> // mostly for Microsoft compilers
#include <stdint.h> // part of Visual Studio 2010 and better
#include <stddef.h> // for size_t in C
#include <limits.h> // for size_t in C

/**
* Given a value "word", produces an integer in [0,p) without division.
* The function is as fair as possible in the sense that if you iterate
* through all possible values of "word", then you will generate all
* possible outputs as uniformly as possible.
*/
static inline uint32_t fastrange32(uint32_t word, uint32_t p) {
	return (uint32_t)(((uint64_t)word * (uint64_t)p) >> 32);
}

#if defined(_MSC_VER) && defined (_WIN64)
#include <intrin.h>// should be part of all recent Visual Studio
#pragma intrinsic(_umul128)
#endif // defined(_MSC_VER) && defined (_WIN64)


/**
* Given a value "word", produces an integer in [0,p) without division.
* The function is as fair as possible in the sense that if you iterate
* through all possible values of "word", then you will generate all
* possible outputs as uniformly as possible.
*/
static inline uint64_t fastrange64(uint64_t word, uint64_t p) {
#if defined(__SIZEOF_INT128__) && !defined(_MSC_VER)// then we know we have a 128-bit int
	return (uint64_t)(((__uint128_t)word * (__uint128_t)p) >> 64);
#elif defined(_MSC_VER) && defined(_WIN64) && !defined(__clang__)	// Disable this on Clang_c2 and use fallback
	// supported in Visual Studio 2005 and better
	uint64_t highProduct;
	_umul128(word, p, &highProduct); // ignore output
	return highProduct;
	unsigned __int64 _umul128(
		unsigned __int64 Multiplier,
		unsigned __int64 Multiplicand,
		unsigned __int64 *HighProduct
	);
#else
	return word % p; // fallback
#endif // __SIZEOF_INT128__
}


#ifndef UINT32_MAX
#define UINT32_MAX  (0xffffffff)
#endif // UINT32_MAX

/**
* Given a value "word", produces an integer in [0,p) without division.
* The function is as fair as possible in the sense that if you iterate
* through all possible values of "word", then you will generate all
* possible outputs as uniformly as possible.
*/
static inline size_t fastrangesize(size_t word, size_t p) {
#if (SIZE_MAX == UINT32_MAX)
	return (size_t)fastrange32(word, p);
#else // assume 64-bit
	return (size_t)fastrange64(word, p);
#endif // SIZE_MAX == UINT32_MAX
}

/**
* Given a value "word", produces an integer in [0,p) without division.
* The function is as fair as possible in the sense that if you iterate
* through all possible values of "word", then you will generate all
* possible outputs as uniformly as possible.
*/
static inline int fastrangeint(int word, int p) {
#if (SIZE_MAX == UINT32_MAX)
	return (int)fastrange32(word, p);
#else // assume 64-bit
	return (int)fastrange64(word, p);
#endif // (SIZE_MAX == UINT32_MAX)
}

#endif// INCLUDE_FASTRANGE_H
