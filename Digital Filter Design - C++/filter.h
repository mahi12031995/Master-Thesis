// ----------------------------------------------------------------------------
/// \file		 filter.h
/// \brief		 realizes a digital filter.
/// \license             For educational purposes only. No Warranty. See License.txt
/// \author		 Wolfgang Schulter
/// \date		 24.02.2017 ws:  simpler Arduino version
/// \date		 31.05.2015 ws:  non-FreeRTOS version
/// \date		 03.03.2012 ws:  FreeRTOS version
/// \date		 26.12.2011 ws:  initial version

#ifndef __FILTER_H__
#define __FILTER_H__  	  	  // only once ..

#include <Arduino.h>
#include "dsp_macros.h"  	  // commons for DSP macros

// ----------------------------------------------------------------------------
// x-buffer class, buffer can be large enough for the longest filter type
// ----------------------------------------------------------------------------
#define N_XBUF    256
class Cxbuf {
public:
  Cxbuf() {
    ixbuf = N_XBUF-1;
    // memset(xbuf, 0, N_XBUF * sizeof(int16_t));
  };
  void tic(short x) {
    ixbuf++; if (ixbuf >= N_XBUF) ixbuf = 0;
    xbuf[ixbuf] = x;
  };
  __inline int16_t getx(int16_t ind_back) {
    int16_t ind = ixbuf - ind_back;    // xbuf[ (ixbuf - ind_back) % N_X_BUF ]
    if (ind < 0) ind += N_XBUF;
    return xbuf[ind];
  }
  int16_t xbuf[N_XBUF];	     //!< x-buffer
  int16_t ixbuf;            // points to the actual element
};

// ----------------------------------
#include "app.h"          // FS_HZ
#include "filter_Ciirf.h"	// IIR filter (float) class
#include "filter_Ciiri.h"	// IIR filter (int) class
#include "filter_Cfir.h"	// FIR filter (int) and (float) class
#include "filter_Ciir2.h"	// IIR2 and SOS filter (int and float) classes
#include "filter_CIQdet.h"	// IQ detector class

class Cfilter {
public:  
  Cfilter() {                   // constructor
    mode = 1;                   // initial mode, can be changed by 'fm' command
    input_mode = 0;             // initial input-mode, can be changed by 'fi' command
    // new in 0.71:
    direct_form = 1;      // can be: 1 for direct form 1,or: 2 for direct form 2
    quant_mode = 0;       // quantization modes: 0:round, 1:floor (binary cut), 2:fix (abs cut)
  }
  // --------------------------------------------------
  void init() {                 // init buffers
    ciirf.init();
    ciiri.init();
    csosf.init();
    csosi.init();
  };
  // --------------------------------------------------
  int16_t tic(int16_t x) {      // called every Ts sampling period, x is input 
    cx.tic(x);                  // store x[n]] into cyclic buffer
    yf = 0;                     // float output, default
    if (mode == 0)              // calc my-filter function for one cycle
      y = my_filter(x);         // own filter
    else if (mode == 1) {        
      yf = ciirf.tic(&cx);      // IIR (float)
      y = ROUND(yf);
    } else if (mode == 2)
      y = ciiri.tic(&cx, direct_form, quant_mode);       // IIR (int)
    else if (mode == 3) {
      yf = cfirf.tic(&cx);      // FIR (float)
      y = ROUND(yf);
    } else if (mode == 4)
      y = cfiri.tic(&cx);       // FIR (int)
    else if (mode == 5) {
      yf = csosf.tic(x);    // SOS (cascaded IIR2) (float)
      y = ROUND(yf);
    } else if (mode == 6)
      y = csosi.tic(x);         // SOS (cascaded IIR2) (int)
    // ---------------------------------------------------------------
    if (use_iq) ciq.tic(y);     // IQ phase detector
    // ---------------------------------------------------------------
    return y;
  }
  // --------------------------------------------------
  Cxbuf cx;                     // x-buffer object
  CIIR_flt ciirf;               // filters ...
  CIIR_int ciiri;               // IIR
  CFIR_flt cfirf;               // FIR
  CFIR_int cfiri;               // FIR
  CSOSf csosf;                  // SOS cascades
  CSOSi csosi;
  CIQdet ciq;                   // IQ (phase) detector
  bool use_iq;                  // 1:use IQ  0:don't use
  int16_t y;                    // integer output (all implementations)
  float yf;                     // float output (with float implementations)
  uint8_t mode;
  uint8_t input_mode;
#define FILTER_NUM_MODES 7
  const char mode_name[FILTER_NUM_MODES][16] = {  // filter mode names[]
    "My Filter",   "IIR (float)",  "IIR(int)",  "FIR (float)",  "FIR(int)", 
    "SOS(float)",  "SOS (int)"
  };
  uint8_t direct_form;      // can be: 1 for direct form 1,or: 2 for direct for 2
  uint8_t quant_mode;       // quantization modes: 0:round, 1:binary cut, 2:abs cut
  
  // --------------------------------------------------
  // TODO: try programming your own filter (my_filter)
  // --------------------------------------------------
  int16_t my_filter(int16_t x)   // mode 0: my_filter
  {  
    // ---- your code follows here ----
    int16_t y = x;               // ws: trivial example: pass thru
    return y;
    // ------ end of my_filter() ------
  }  
  
  int16_t my_filter__(int16_t xin)   // mode 0: my_filter, conv example
  {  
    // ---- your code follows here ----
    //float g[5] = {1,3,0,-3,-1};   // --- conv example, SP2 (2.3)
    float g[5] = {1,1,0,0,0};   // --- conv basic example, SP2 (2.3)
    static int16_t x[5];  // static is important here!
    float y = 0;	// 
	
    // --- store actual x[n] input to a buffer, including a history of 4 samples
    x[4] = x[3];	// x[n-4]
    x[3] = x[2];	// x[n-3]
    x[2] = x[1];	// x[n-2]
    x[1] = x[0];	// x[n-1]
    x[0] = xin;		// x[n]
	
    // --- convolution equation SP2 (2.3), y[n] =
    y = x[0]*g[0] + x[1]*g[1] + x[2]*g[2] + x[3]*g[3] + x[4]*g[4];
    return y;
    // ------ end of my_filter() ------
  }  
  float my_filter_param = 0.05;  // any value, can be set by 'fp' command
};

extern Cfilter filter;          // the filter object

#endif // __FILTER_H__
