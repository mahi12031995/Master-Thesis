// ----------------------------------------------------------------------------
/// \file		 filter_Ciirf.h
/// \brief		 module for N. order recursive in float.
///				 see DF 5.2.2 and 5.5.8
/// \license             For educational purposes only. No Warranty. See License.txt
/// \author		 Wolfgang Schulter
/// \date		 25.06.2016 ws:  initial version for M3 (Due) as class
// ----------------------------------------------------------------------------

#ifndef __FILTER_CIIRF_H__
#define __FILTER_CIIRF_H__  	  	// only once ..

#define N_IIR_FLT  33			// 33 since .27

#include "filter_init.h"                // initial coefficients
#include "dsp_macros.h"  	  // commons for DSP macros
#include <string.h>

// -------------------------------------------------------
class CIIR_flt {
public:
	void init() {
		memset(ybuf, 0, sizeof(ybuf));
	};
    // ----------------------------------------------------------------------------
    CIIR_flt() {
      nybuf = 0;
      Nb = MIN(N_IIR_FLT, (sizeof(init_b_iir)/sizeof(double)));
      for (int i=0; i < N_IIR_FLT; i++) {
        if (i < Nb)
          b[i] = init_b_iir[i];
        else
          b[i] = 0;
      }
      Na = MIN(N_IIR_FLT, (sizeof(init_a_iir)/sizeof(double)));
      for (int i=0; i < N_IIR_FLT; i++) {
        if (i < Na)
          a[i] = init_a_iir[i];
        else
          a[i] = 0;
      }
	  init();
    };
    // ----------------------------------------------------------------------------
    float tic(Cxbuf *cx) {             // y = iir(x)
      int16_t i, ind; 	// , n = *n_;  // cyclic index
      float ynrec = 0, yrec = 0, y;
      // ---- recursive part ---------------------------
      for (i = 1; i < Na; i++) {	
        ind = nybuf - i;
        if (ind < 0) ind += N_IIR_FLT;
        if (a[i]) yrec += ybuf[ind] * a[i];
      }

      // ---- non-recursive part -----------------------
      for (i = 0; i < Nb; i++) {	// ---- non-recursive part
        if (b[i]) ynrec += cx->getx(i) * b[i]; 
      }
      y = (ynrec - yrec); 
      save_ybuf(y);					// save back y[n]
    
      return y;		// return y[n]
    };
    // ---------------------------------------------------
    void save_ybuf(float y) {
      ybuf[nybuf] = y;				// save back y[n]
      nybuf++; if (nybuf >= N_IIR_FLT) nybuf = 0;
    };
    // ----------------------------------------------------------------------------
    uint8_t Na, Nb;             //!< actual filter lengths, must be <= N_IIR_INT
    float b[N_IIR_FLT];		//!< IIR_flt: b[] numerator coeffs
    float a[N_IIR_FLT];		//!< IIR_flt: a[] denominator coeffs
    float ybuf[N_IIR_FLT];	//!< since v.19: separate y-buffer
    int16_t nybuf;              //! cyclic buffer index for output signal
};

extern CIIR_flt ciirf;

#endif  // __FILTER_CIIRF_H__

