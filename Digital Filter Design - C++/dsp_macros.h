/// \file		 dsp_macros.h
/// \brief		 some DSP macros.
/// \license             For educational purposes only. No Warranty. See License.txt
/// \author		 Wolfgang Schulter
/// \date		 30.08.2013 ws:  initial version
// ----------------------------------------------------------------------------

#ifndef __DSP_MACROS_H__
#define __DSP_MACROS_H__  	  	// only once ..

#ifndef WIN32
#define USE_M3_DSP_FEATURES		// Multiply Accumulate
#endif

#ifndef MIN
#define MIN(A,B) (((A)<(B)) ? (A) : (B))
#endif
#ifndef MAX
#define MAX(A,B) (((A)>(B)) ? (A) : (B))
#endif
#ifndef ABS
#define ABS(A) (((A)<(0)) ? -(A) : (A))
#endif
#ifndef LIMIT
#define LIMIT(X, LO, HI)  ((X) >= (LO) ? ( MIN(X,HI) ) : (LO))
#endif
#ifndef ROUND  // MATLAB round to closest integer
#define ROUND(A) (((A)<(0)) ? ((int)((A)-0.5)) : ((int)((A)+0.5)))
#endif
#ifndef ROUNDOFF    // add/sub round offset to closest integer multiple of S, R=S/2
#define ROUNDOFF(A,R) (((A)<(0)) ? ((A)-(R)) : ((A)+(R)))
#endif
// the modulo 2^exp macro
#define MOD2(EXP, X) ( ((1 << (EXP))-1) & (X) )

// -----------------------------------------------------------------------------------
// Multiply 32-bit, round and right-shift, equiv. to (int) ((double)X*Y)/(2^RSH) + 0.5
// -----------------------------------------------------------------------------------
#define MUL32_RND_RSH(X,Y,RSH) ( ( ((int)X)*(Y) + (1<<((RSH)-1)) ) >> (RSH) )
// -----------------------------------------------------------------------------------

#ifdef USE_M3_DSP_FEATURES
// ----------------------------------
// http://mbed.org/forum/mbed/topic/1549/
// ----------------------------------
static __inline__ int MULSHIFT32(int x, int y)
{
  int zlow;
  __asm__ volatile ("smull %0,%1,%2,%3" : "=&r" (zlow), "=r" (y) : "r" (x), "1" (y) : "cc");
  return y;
}
// ----------------------------------
typedef union _U64
{
  int64_t w64;
  struct {
    uint32_t lo32;
    int32_t hi32;
  } r;
} Union64;

// ----------------------------------
// for FIR,M=250: 66 us, with gcc only: 100 us !
// ----------------------------------
static __inline int64_t MADD64(int64_t sum64, int x, int y)
{
  Union64 u;
  u.w64 = sum64;
  __asm__ volatile ("smlal %0,%1,%2,%3" : "+&r" (u.r.lo32), "+&r" (u.r.hi32) : "r" (x), "r" (y) : "cc");
  return u.w64;
}
#endif // USE_M3_DSP_FEATURES

#ifdef USE_M4_DSP_FEATURES
// ----------------------------------
// http://mbed.org/forum/mbed/topic/1549/
// ----------------------------------
static __inline int64_t MADD_FLOAT(float sum, float x, float y)
{
  Union64 u;
  u.w64 = sum64;
  // __asm__ volatile ("smlal %0,%1,%2,%3" : "+&r" (u.r.lo32), "+&r" (u.r.hi32) : "r" (x), "r" (y) : "cc");
  return u.w64;
}
#endif

#endif // __DSP_MACROS_H__


