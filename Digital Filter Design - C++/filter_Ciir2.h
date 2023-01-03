// ----------------------------------------------------------------------------
/// \file		 filter_Ciir2.h
/// \brief		 class for 2. order digital filters (SOS, biquad) in int and float.
/// \license             For educational purposes only. No Warranty. See License.txt
/// \author		 Wolfgang Schulter
/// \date		 26.02.2017 ws:  initial version

#ifndef __FILTER_CIIR2_H__
#define __FILTER_CIIR2_H__  	  	// only once ..

#include "filter_init.h"                // initial coefficients
#include "dsp_macros.h"  	        // commons for DSP macros

#define NUM_SOS	16			// 16 since .27, 8 since v.11, renamed to NUM_SOS in v.24, was (NUM_MSTAGE)

// -------------------------------------------------------
// Initialization
// -------------------------------------------------------
const int16_t init_Nsos = sizeof(init_sos)/6/sizeof(double);
// -------------------------------------------------------

// ----------------------------------
// IIR Filter 2.order with integer arithmetic
//            default scaling factor SIIR2
// ----------------------------------
#define LDSIIR2	        20
#define SIIR2	        (1 << LDSIIR2)
#define RIIR2	        (1 << (LDSIIR2-1))

// ----------------------------------
// IIR2 Filter: (integer fixed-point)
// y[n] = (Sb0*x[n] + Sb1*x[n-1] + Sb2*x[n-2] - Sa1*y[n-1] - Sa2*y[n-2]) / S
//      = (Sb0*x[n] + Sb1*x[n-1] + Sb2*x[n-2] - Sa1*y[n-1] - Sa2*y[n-2]) >> ldS
// ----------------------------------
class CIIR2i {
  public:
	void init() {
		y_1 = y_2 = 0;
	};
    CIIR2i() {
      x_1 = x_2 = 0;
      y_1 = y_2 = 0;
    };
    int16_t tic(int16_t x) {	// IIR2: integer (fixed point) realization
      int64_t y = -(y_1 >> (LDSIIR2 - 5)) * Sa[1];
      y -= (y_2 >> (LDSIIR2 - 5)) * Sa[2];
      y = (y >> 5);
      y += (int64_t) x * Sb[0];
      y += (int64_t)x_1 * Sb[1];
      y += (int64_t)x_2 * Sb[2];
      y_2 = y_1; y_1 = y;
      x_2 = x_1; x_1 = x;
      return (y + RIIR2) >> LDSIIR2; 
      //return ROUNDOFF(y, RIIR2) >> LDSIIR2;
    };
    int32_t Sb[3];	// non-rec. coeffs Sb[]
    int32_t Sa[3];	// rec. coeff Sa[], Sa[0] = scaling factor
    int16_t x_1, x_2;	// previous inputs
    int64_t y_1, y_2;	// (previous) outputs
};

// ----------------------------------
// IIR2 Filter: (floating-point)
// y[n] = (b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2])
// ----------------------------------
class CIIR2f {
  public:
	void init() {
		y_1 = y_2 = 0;
	};
    CIIR2f() {
      x_1 = x_2 = 0;
      y_1 = y_2 = 0;
    };
    // ----------------------------------
    short tic(float x) 	{  // IIR2: integer (fixed point) realization
      float y = x * b[0];
      y += x_1 * b[1];
      y += x_2 * b[2];
      y -= y_1 * a[1];
      y -= y_2 * a[2];
      // y += 0.5; //
      y_2 = y_1; y_1 = y;
      x_2 = x_1; x_1 = x;
      return y;     	// fixed point: quite efficient !
    };
    float b[3];	      // non-rec. coeffs Sb[]
    float a[3];	      // rec. coeff Sa[], Sa[0] = scaling factor
    float x_1, x_2;   // previous inputs
    float y_1, y_2;   // (previous) outputs
};

// ----------------------------------
class CSOSi {
  public:
	void init() {
		for (int i=0; i < NUM_SOS; i++) iir2i[i].init();
	};
    CSOSi() {
      Nsos = MIN(NUM_SOS, init_Nsos);
      for (int i = 0,i6 = 0; i < Nsos; i++, i6+=6) {
        iir2i[i].Sb[0] = ROUND(SIIR2*init_sos[i6]);
        iir2i[i].Sb[1] = ROUND(SIIR2*init_sos[i6+1]);
        iir2i[i].Sb[2] = ROUND(SIIR2*init_sos[i6+2]);
        iir2i[i].Sa[0] = ROUND(SIIR2*init_sos[i6+3]);
        iir2i[i].Sa[1] = ROUND(SIIR2*init_sos[i6+4]);
        iir2i[i].Sa[2] = ROUND(SIIR2*init_sos[i6+5]);
      }
	  init();
    }
    short tic(int16_t x) 	{       // SOS: integer (fixed point) realization
      short y = x;
      for (int i = 0; i < Nsos; i++) {
        y = iir2i[i].tic(y);
      }
      return y;
    }
    uint8_t Nsos;		// since .27, default to 8 (as before)
    CIIR2i iir2i[NUM_SOS];	// array of SOS
};

// ----------------------------------
class CSOSf {
  public:
	void init() {
		for (int i=0; i < NUM_SOS; i++) iir2f[i].init();
	};
    CSOSf() {
      Nsos = MIN(NUM_SOS, init_Nsos);
      for (int i = 0,i6 = 0; i < Nsos; i++, i6+=6) {
        iir2f[i].b[0] = init_sos[i6];
        iir2f[i].b[1] = init_sos[i6+1];
        iir2f[i].b[2] = init_sos[i6+2];
        iir2f[i].a[0] = init_sos[i6+3];
        iir2f[i].a[1] = init_sos[i6+4];
        iir2f[i].a[2] = init_sos[i6+5];
      }
	  init();
    }
    float tic(int16_t x) 	{       // SOS: floating point realization
      float y = x;              // cascade of Nsos sections
      for (int i = 0; i < Nsos; i++) {
        y = iir2f[i].tic(y);    
      }
      return y;
    }
    uint8_t Nsos;		// since .27, default to 8 (as before)
    CIIR2f iir2f[NUM_SOS];	// array of SOS
};

#endif // __FILTER_CIIR2_H__

