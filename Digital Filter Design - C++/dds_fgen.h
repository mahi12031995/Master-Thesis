/// \file	cfgen.h
/// \brief	A Frequency Generator Class (DDS method, DF 8.1)
/// \author	Wolfgang Schulter
/// \license    For educational purposes only. No Warranty. See License.txt
// 28.02.2016 Wolfgang Schulter
// ----------------------------------------------------------------------------

#ifndef __DDS_FGEN_H__
#define __DDS_FGEN_H__  		// only once ..

#include <Arduino.h>

// ----------------------------------------------------------------------
// must be defined before ..
#define USE_32BIT_CPU		 // assuming 32-bit CPU, e.g.: SAM3
//#define  FS_HZ   5000          // Basic FTS periodic frequency in Hz
// ----------------------------------------------------------------------
enum FGEN_MODE {FGEN_MODE_OFF = 0, 	FGEN_MODE_DC, 	FGEN_MODE_IMP, 	FGEN_MODE_IMP2,
                FGEN_MODE_SIN = 4,	FGEN_MODE_SAW,	FGEN_MODE_TRI, 	FGEN_MODE_REC,
                FGEN_MODE_RNDN = 8,	FGEN_MODE_SYN,	FGEN_MODE_ARB,	FGEN_MODE_RND,
                FGEN_MODE_NRZ = 12,	FGEN_NUM_MODES
               };  //

extern const char *fgen_mode_str[FGEN_NUM_MODES];

// fgen_mode_str[] implemented in fgen.c
#define INIT_FGEN_MODE_STRINGS \
  {\
    "OFF",				"DC ",			"IMP",			"IM2",\
    "SIN",				"SAW",			"TRI",			"REC",\
    "RNDN",				"SYN",			"ARB", 			"RND",\
    "NRZ"\
  }

#define PARAM_SNF    6
#define PARAM_NF     ((int32_t)1 << PARAM_SNF)

#ifdef USE_32BIT_CPU
#define PARAM_SNPHI 28  // ld(Nphi)
#else
#define PARAM_SNPHI 20  // ld(Nphi)
#endif
#define PARAM_NPHI      ((int32_t)1 << PARAM_SNPHI)
#define PARAM_NPHI_1_4	((int32_t)1 << (PARAM_SNPHI-2))	// NPHI/4
#define PARAM_NPHI_1_2	((int32_t)1 << (PARAM_SNPHI-1))	// NPHI/2
#define PARAM_NPHI_3_4	((int32_t)3 << (PARAM_SNPHI-2))	// NPHI*3/4
#define PARAM_SNH  14

class Cdds_fgen
{
  public:
    Cdds_fgen(int16_t _fs_hz);
    // ~Cfgen() {};
    const char *get_mode_str() {
      if ((mode <=0) || (mode >= FGEN_NUM_MODES))
        return fgen_mode_str[0];
      else
        return fgen_mode_str[mode];
    };
    void set_sampling_freq_hz(int16_t _fs_hz);
    void set_freq_hz(int16_t _freq_hz);
    int32_t dphi, phi, phiq4;
    int32_t frq, frq_1;
    int16_t mode, Ts_us;
#ifdef USE_32BIT_CPU
    int32_t Ts_ns;					// with 32-bit versions
#endif
    int16_t amp, out, out2, offset, duty;
    int16_t _sin, _cos;
    int16_t add_rndn_ampl;	// add noise amplitude
    int8_t full_rot;
    int16_t tic();
    char *mode_str = "";
    // void set_frq_hz(float f);
};

extern Cdds_fgen fgen;		// the DDS frequency generator

// ----------------------------------------------------------------------------
#define 	NARB_MAX	50	// since 1.01: arbitrary buffer, max elements
extern int16_t fgen_arb[NARB_MAX+1];

// realizes a sine function table.
// see d:\common/MATLAB/sintab.m
#define PARAM_SNH     	14
#define PARAM_NH  		(1L << PARAM_SNH)

//! #DEF: Anzahl Sinus-Tabellenwerte für eine Vollperiode
#define PARAM_SNSIN     10
#define PARAM_NSIN  	(1L << PARAM_SNSIN)

//! #DEF: Anzahl Sinus-Tabellenwerte für eine Viertelperiode
#define PARAM_NTAB      (PARAM_NSIN / 4)

extern const int16_t Sintab[PARAM_NTAB + 1];

int16_t GetSinTab(int16_t p, int16_t *p_cos);
int16_t GetCosTab(int16_t p, int16_t *p_sin);

#endif // __DDS_FGEN_H__


