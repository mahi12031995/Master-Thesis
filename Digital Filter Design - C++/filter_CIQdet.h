// ----------------------------------------------------------------------------
/// \file		 filter_CIQdet.h
/// \brief		 class for IQ quadrature detection.
///				 see DF 5.4.7.1
/// \license             For educational purposes only. No Warranty. See License.txt
/// \author		 Wolfgang Schulter
/// \date		 26.06.2016 ws:  classes

#ifndef __FILTER_CIQDET_H__
#define __FILTER_CIQDET_H__  	  // only once ..

#include "dds_fgen.h"		  // the DDS Function generator class

// ----------------------------------
// Al_LP Filter 1.order:  with integer arithmetic
// y[n] = y[n-1] + Salpha*(x[n] + x[n-1] - 2*y[n-1]) / S
//      = y[n-1] + Salpha*(x[n] + x[n-1] - 2*y[n-1]) >> ldS
// ----------------------------------
#define PARAM_SNAL1		14
#define PARAM_NAL1		(1L << PARAM_SNAL1)
#define PARAM_RNAL1		((1L << (PARAM_SNAL1-1))-1)

#define TS_US        ((1 << 20)/FS_HZ)    // approx sampling perion in us

// ----------------------------------
// Al_LPi Filter: (integer fixed-point)
// ----------------------------------
class CAl_LPi {
    void _zero() {
      x_1 = 0; y_1 = 0;
    }
  public:
    void set_scale(uint8_t _ldS) {
      ldS = _ldS;
      S = (int16_t)1 << ldS;
    }
    CAl_LPi(int16_t _Salpha = (PARAM_NAL1 >> 1)) {
      _zero();
      set_scale(PARAM_SNAL1);	// default scale
      Salpha = _Salpha;
    };
    // -----------------------------------------
    // Al-Lowpass alpha(fg) = 2*fg*Ts  <- Patent
    // -----------------------------------------
    int16_t set_alpha_fg_Hz(int16_t fg_Hz) {
      // alpha setting:
      // fg / fs = 0.5*alpha --> alpha = 2*fg*(Ts_us/1E6) ~ fg*Ts >> 21 !!!! was 19: Bug
      // short alpha = (short)(((int)fg*Ts_us) >> (21 - PARAM_SNALPHA));  // <-- ws: Bugfix
      Salpha = ((int32_t)fg_Hz*TS_US) >> (19 - ldS);  // <-- ws: Bugfix
      //extern unsigned int Ts_ns;	//!< sampling period in ~ns (since 4.0.11)
      //short alpha = (short) (((int) fg * Ts_ns) >> (31 - PARAM_SNALPHA)); // <-- Ts_ns since v.11
      return Salpha;
    }
    short tic(int16_t x) 		// IIR1: integer (fixed point) realization
    {
      int32_t y = y_1;
      y += ((int32_t)(x + x_1 - (y_1 >> (ldS - 1)))) * Salpha;
      // y += y_1;
      y_1 = y;
      y = (int16_t) ((y + (S >> 1)) >> ldS); //
      // y_1 = y;
      x_1 = x;
      return y;     	// fixed point: quite efficient !
    };
    int16_t Salpha;		// control coeff Salpha
    int16_t x_1;		// previous input
    int32_t y_1;		// (previous) output
    int16_t S;			// scaling factor
    uint8_t ldS;		// log2(S)
};

// ----------------------------------
// IQ Detector (integer fixed-point)
// ----------------------------------
class CIQdet {
  public:
    // ----------------------------------
    CIQdet() {
      // _zero_xbuf();
    };
    // ----------------------------------
    short tic(int16_t x) {	    // IQ detector (fixed point) realization
      int16_t ycos_, ysin_;
      int16_t iq_cos, iq_sin;
      
      int32_t fgen_iq_phi = fgen.phi;    // already calculated !
      
      // y = ycos + j*ysin: center frequency complex harmonic generation
      iq_sin = GetSinTab(fgen_iq_phi >> (PARAM_SNPHI - PARAM_SNSIN), &iq_cos); // very little runtime
      
      // multiplication with 2*exp(j*20*fgen_par.f0*t)
      //xcos_1 = xcos;
      //xsin_1 = xsin;
      xcos = ((int32_t) x * iq_cos + (PARAM_NH >> 2)) >> (PARAM_SNH-1);
      xsin = ((int32_t) x * iq_sin + (PARAM_NH >> 2)) >> (PARAM_SNH-1);

      // alpha setting: fg / fs = 0.5*alpha --> alpha = 2*fg*(Ts_us/1E6) ~ fg*Ts >> 19
      int16_t fg = MAX(fgen.frq >> (PARAM_SNF + 4), 1); // ~ 0.03 f0, int freq. since v.10
      als1.Salpha = als2.Salpha = alc1.Salpha = alc2.set_alpha_fg_Hz(fg);
      
      // final lowpass filtering
      int16_t ycos = alc1.tic(xcos);
      ycos2 = alc2.tic(ycos);
      
      ysin = als1.tic(xsin);
      ysin2 = als2.tic(ysin);
    }
    // ----------------------------------
    // static Cfgen fgen_iq;
    // typedef struct iq_struct {
    short xcos;		// = input x * cos
    short xsin;		// = input x * sin
    CAl_LPi alc1,alc2;	// alpha (Al-LP)
    CAl_LPi als1,als2;	// alpha (Al-LP)
    //short xcos_1;	// xcos[n-1]
    //short xsin_1;	//
    short ycos;		// Al-LP out (short. in-phase)
    short ysin;		// Al-LP out (short. q-phase)
    //int ycos_1;
    //int ysin_1;
    short ycos2;	// Al-LP(2) out (short. in-phase)
    short ysin2;	// Al-LP(2) out (short. q-phase)
    //int ycos2_1;
    //int ysin2_1;
    // } IQSTRUCT;
    // ----------------------------------
};

extern CIQdet ciq;      // the object

#endif // __FILTER_CIQDET_H__

