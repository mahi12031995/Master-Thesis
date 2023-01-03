// ----------------------------------------------------------------------------
/// \file		 filter_Ciiri.h
/// \brief		 module for N. order recursive in int.
///				 see DF 5.2.2 and 5.5.8
/// \license             For educational purposes only. No Warranty. See License.txt
/// \author		 Wolfgang Schulter
/// \date		 25.06.2016 ws:  initial version for M3 (Due) as class
// ----------------------------------------------------------------------------

#ifndef __FILTER_CIIRI_H__
#define __FILTER_CIIRI_H__  	  	// only once ..

#include "filter_init.h"                // initial coefficients
#include "dsp_macros.h"  		// commons for DSP macros

// ----------------------------------
// IIR Filter: IIR2, IIR with integer arithmetic
//             scaling factor PARAM_NIIR
// ----------------------------------
#define LDSIIR		20                      // ldS = ld(S) = log_2(S)
#define LDS2IIR		(2*LDSIIR)              // ldS2 = ld(S^2) = 2*log_2(S)
#define SIIR		  (1 << LDSIIR)           // scaling factor S = 2^ldS
#define RIIR	    (1 << (LDSIIR-1))       // = S/2
#define LDY         8						// direct form 1: scaling of y * a values, 
											// 32 bit storage: 32 > LDSIIR + LDY

#define N_IIR_INT  33		// 33 since .27, 12 since 4.0.8, 16 since 4.0.11

#if (N_XBUF < N_IIR_INT)
#error "N_XBUF too small !"
#endif

#ifndef UINT32_MAX
#define UINT32_MAX	0xffffffff
#endif

class CIIR_int {
public:
	void init() {
		memset(ybuf, 0, sizeof(ybuf));
	};
    // ----------------------------------------------------------------------------
	// saturate (to limit +- 2^limit_ld) (3 quantization modes) & quantize 
	// see [Lacroix] Bild 5.2
    // ----------------------------------------------------------------------------
	int64_t qssat(int64_t x, unsigned limit_ld, unsigned lds) {
		int64_t limit = (limit_ld) ? (((int64_t)1) << limit_ld): 0;	// <-- +-limit ~ 2^limit_ld before scaling, or 0
		int64_t value0_5 = 0; //((__int64)1 << (lds-1));			// <-- 0.5 before scaling
		// (1) quantization mode 
		if (quant_mode == 0) {  // round (as common at 0.5)
			if (x >= 0) {
				x += value0_5;		// round up at >= 0.5
			} else {
				x -= value0_5;		// round down at <= -0.5
			}
			// (2) saturation
			if (limit) {
				if (x > limit) {
					x = limit;
					if (nqssat_limits < UINT32_MAX) nqssat_limits++;
				} else if (x < (-limit)) {
					x = -limit;
					if (nqssat_limits < UINT32_MAX) nqssat_limits++;
				}
			}
			// (3) final scaling
			x = x >> lds;			
		} else if (quant_mode == 1) {	// binary cut
			if (x >= 0) {
				// (2) saturation
				if (x > limit) {
					x = limit;
					if (nqssat_limits < UINT32_MAX) nqssat_limits++;
				}
			} else {
				// (2) saturation
				if (limit) {
					if (x < (-limit)) {
						x = -limit;
						if (nqssat_limits < UINT32_MAX) nqssat_limits++;
					}
				}
			}
			x = x >> lds;			// (3) final scaling
		} else if (quant_mode == 2) {	// absolute cut
			if (x >= 0) {
				// (2) saturation
				if (limit) {
					if (x > limit) {
						x = limit;
						if (nqssat_limits < UINT32_MAX) nqssat_limits++;
					}
				}
				// (3) scaling with abs cut, here positive
				x = x >> lds;
			} else {
				// (2) saturation
				if (limit) {
					if (x < (-limit)) {
						x = -limit;
						if (nqssat_limits < UINT32_MAX) nqssat_limits++;
					}
				}
				// (3) scaling with abs cut, here negative
				x = -((-x) >> lds);
			}
		}
		return x;
	}
    // ----------------------------------------------------------------------------
    CIIR_int() {        // initialization
      nybuf = 0;
	  direct_form = 1;	// direct form 1 tends to more limit cycles than 2

      Nb = MIN(N_IIR_INT, (sizeof(init_b_iir)/sizeof(double)));
      for (int i=0; i < N_IIR_INT; i++) {
        if (i < Nb)
          Sb[i] = ROUND(SIIR*init_b_iir[i]);
        else
          Sb[i] = 0;
      }
      Na = MIN(N_IIR_INT, (sizeof(init_a_iir)/sizeof(double)));
      for (int i=0; i < N_IIR_INT; i++) {
        if (i < Na)
          Sa[i] = ROUND(SIIR*init_a_iir[i]);
        else
          Sa[i] = 0;
      }
	  init();
    };
    // ----------------------------------------------------------------------------
    int16_t tic(Cxbuf *cx, uint8_t _direct_form, uint8_t _quant_mode) {  // y = iir(x)
      int16_t i, ind; 	// , n = *n_;	// cyclic index
      int64_t ynrec = 0, yrec = 0, acc;
      int32_t *pSa = Sa+1;				// start at a[1]
      int32_t *pSb = Sb;
  	  direct_form = _direct_form;
  	  quant_mode = _quant_mode;
  
  	  if (direct_form == 1) {			// --- direct form 1 implementation
  
  		  // ---- recursive part ---------------------------
  		  yrec = 0;
  		  for (i = 1; i < Na; i++) {	
    			ind = nybuf - i;
    			if (ind < 0) ind += N_IIR_INT;
#ifdef USE_M3_DSP_FEATURES
  			  if (Sa[i]) yrec = MADD64(yrec, ybuf[ind], Sa[i]); // Multiply Accumulate
#else
	  		  if (Sa[i]) yrec += (int64_t)ybuf[ind] * Sa[i];
#endif
		    }
  		  // yrec = yrec >> LDSIIR;	// scal a*y --> ldM + ldX - 10
  		  yrec = ROUNDOFF(yrec, RIIR) >> LDSIIR;
	      
  		  // ---- non-recursive part -----------------------
  		  ynrec = 0;
  		  for (i = 0; i < Nb; i++) {	// ---- non-recursive part
  		  //ind = cx->ixbuf - i;
  		  //if (ind < 0) ind += N_XBUF;
#ifdef USE_M3_DSP_FEATURES
  			  if (Sb[i]) ynrec = MADD64(ynrec, cx->getx(i), Sb[i]); // Multiply Accumulate
#else
	  		  if (Sb[i]) ynrec += (int64_t)cx->getx(i) * Sb[i]; 
#endif
		    }
  		  // ynrec = ynrec >> (LDSIIR - LDY);	// scal a*y --> ldM - LDY
  		  ynrec = ROUNDOFF(ynrec, 1<<(LDSIIR-LDY-1)) >> (LDSIIR - LDY);
  	      
  		  acc = (ynrec - yrec); 
  		  save_ybuf((int32_t)acc);		   // save back y[n] << 5
	      
  		  // return (y >> LDY);		// return y[n]
  		  return (ROUNDOFF(acc, 1<<(LDY-1)) >> LDY);
  
	    } else if (direct_form == 2)  {		// --- direct form 2 implementation

  		  // ---- recursive part ---------------------------
  		  yrec = (((int64_t)cx->getx(0)) << LDS2IIR);
  		  for (i = 1; i < Na; i++) {	
  			ind = nwbuf - i;
  			if (ind < 0) ind += N_IIR_INT;
  		    if (Sa[i]) yrec -= Swbuf[ind] * Sa[i];
  		  }
  		  acc = qssat(yrec, LDS2IIR+16, LDSIIR);
  		  //acc = yrec >> LDSIIR;
  		  //acc = ;		// limit to (16 + LDSIIR) bits
  	      Swbuf[nwbuf] = acc;
  
  		  // ---- non-recursive part -----------------------
  		  ynrec = 0;
  		  for (i = 0; i < Nb; i++) {	// ---- non-recursive part
  			ind = nwbuf - i;
  			if (ind < 0) ind += N_IIR_INT;
  			if (Sb[i]) ynrec += Swbuf[ind] * Sb[i]; 
  		  }
  		  save_wbuf(acc); 
  	      
  		  return qssat(ynrec, LDS2IIR+16, LDS2IIR);	// saturate, round/floor/fix (quantizer mode 0/1/2), scale (rsh)
  	  }	// USE_DIRECT_FORM == 2
    };

    // ----    for direct form 1     ---------------------
    void save_ybuf(int32_t y) {		// 
      ybuf[nybuf] = (int32_t)y;		// save back y[n] << 
      nybuf++; if (nybuf >= N_IIR_INT) nybuf = 0;
    };
    // ---------------------------------------------------
    int16_t nybuf;                  //! cyclic buffer index for output signal
    int32_t ybuf[N_IIR_INT];		//!< y-buffer

    // ----    for direct form 2     ---------------------
    void save_wbuf(int64_t Sw) {
      Swbuf[nwbuf] = Sw;			// save back Sw[n]
      nwbuf++; if (nwbuf >= N_IIR_INT) nwbuf = 0;
    };
    // ---------------------------------------------------
    int16_t nwbuf;                  //! cyclic buffer index for output signal
    int64_t Swbuf[N_IIR_INT];		//!< w-buffer, double word length

    // ----    for direct form 1 and 2   -----------------
    uint8_t Na, Nb;                 //!< actual filter lengths, must be <= N_IIR_INT
    int32_t Sb[N_IIR_INT];			//!< IIR_int: b[] numerator coeffs scaled to S
    int32_t Sa[N_IIR_INT];	        //!< IIR_int: a[] denominator coeffs scaled to S
	uint32_t nqssat_limits;			//!< number of limitations in qssat

    uint8_t direct_form;			//!< can be: 1 for direct form 1,or: 2 for direct for 2
	uint8_t quant_mode;				//!< quantization modes: 0:round, 1:binary cut, 2:abs cut
};


extern CIIR_int ciiri;

#endif  // __FILTER_CIIRi_H__
