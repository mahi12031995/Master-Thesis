// ----------------------------------------------------------------------------
/// \file		 filter_Cfir.h
/// \brief		 class for 1. order digital filters in int.
///				 see DF 5.4
/// \license             For educational purposes only. No Warranty. See License.txt
/// \author		 Wolfgang Schulter
/// \date		 16.06.2016 ws:  classes

#ifndef __FILTER_CFIR_H__
#define __FILTER_CFIR_H__  	  	// only once ..

#include "filter_init.h"                // initial coefficients
#include "dsp_macros.h"  	// commons for DSP macros

// ----------------------------------
// FIR Filter: y[n] = b0*x[n] + b1*x[n-1] + ... b{M-1}*x[n-(M-1)]
// ----------------------------------
#define LDSFIR	13
#define SFIR	(1 << LDSFIR)
#define RFIR	(1 << (LDSFIR-1))

#define	NBUF_FIR	256	// maximum number of FIRi coeff's

// -------------------------------------------------------
// Initialization
// -------------------------------------------------------
const int16_t init_M = sizeof(init_b_fir)/sizeof(double);
// -------------------------------------------------------

// ----------------------------------
// FIR Filter: (floating-point)
// y[n] = b0*x[n] + b1*x[n-1] + ... + b{M-1}*x[n-(M-1)]
// ----------------------------------
class CFIR_flt {
  public:
    // ----------------------------------
    CFIR_flt() {
      // _zero_xbuf();
      M = MIN(init_M, NBUF_FIR);
      for (int i=0; i < M; i++) {
        b[i] = init_b_fir[i];
      }
    };
    // ----------------------------------
    float tic(Cxbuf *cx) 	// FIRi: integer (fixed point) realization
    {
      float y = 0;
      for (int i = 0; i < M; i++) { 
        if (b[i]) y += cx->getx(i) * b[i];    // = x[n-i] * Sb[i]
      }
      return y;
    };
    // ----------------------------------
    float b[NBUF_FIR];	        // non-rec. coeffs b[]
    uint8_t M;		        // number of taps (filter order is M-1)
    // ----------------------------------
};

// ----------------------------------
// FIR Filter: (integer fixed-point)
// y[n] = (Sb0*x[n] + Sb1*x[n-1] + ... + SbN*x[n-(M-1)]) / S
//      = (Sb0*x[n] + Sb1*x[n-1] + ... + SbN*x[n-(M-1)]) >> ldS
// ----------------------------------
class CFIR_int {
  public:
    // ----------------------------------
    CFIR_int() {
      // _zero_xbuf();
      M = MIN(init_M, NBUF_FIR);
      for (int i=0; i < M; i++) {
        Sb[i] = ROUND(SFIR*init_b_fir[i]);
      }
    };
    // ----------------------------------
    int16_t tic(Cxbuf *cx) 	// FIRi: integer (fixed point) realization
    {
#ifdef USE_M3_DSP_FEATURES
      int64_t y = 0;
      for (int i = 0; i < M; i++) {
        y = MADD64(y, (int32_t)cx->getx(i), Sb[i]);  // Multiply Accumulate
      }
#else
      int32_t y = 0;
      for (int i = 0; i < M; i++) {       // Due exec-time with Butterworth LP6: 42 us
        y += (int32_t)cx->getx(i) * Sb[i];    // = x[n-i] * Sb[i]
      }
#endif
      y = ROUNDOFF(y, RFIR) >> LDSFIR; //
      return y; 
    };
    // ----------------------------------
    int16_t Sb[NBUF_FIR];	// non-rec. coeffs Sb[]
    uint8_t M;		        // number of taps (filter order is M-1)
    // ----------------------------------
};

#endif // __FILTER_CFIR_H__

