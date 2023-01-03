// ----------------------------------------------------------------------------
/// \file     vars.cpp
/// \brief     allows read|write access by identifier in ccomm module.
/// \license     For educational purposes only. No Warranty. See License.txt
/// \author    Wolfgang Schulter
/// \date    12.04.2020 ws:  initial version (RTSim ~ Arduino) version

#include "vars.h"   // variable identifiers for r|w access

#define NVARS 36  // var id {name, element type, first element addr*, size(elements) }

#include <string.h>
#include <stdlib.h>
#include "filter.h"
#include "dds_fgen.h" // the DDS Function module

// -------------------------------------------------------------------
TSVAR tsvar[NVARS] = {
  // ---------------------------------------------------------------
  // r|w firf.M <val>
  {"firf.N",  'c',  &filter.cfirf.M,  sizeof(filter.cfirf.M)}, 

  // ---------------------------------------------------------------
  // r|w firf.b <val_0> ... <val_M-1>
  {"firf.b",  'f',  filter.cfirf.b,   sizeof(filter.cfirf.b)/sizeof(float)},

  // ---------------------------------------------------------------
  // r|w firi.M <val>
  {"firi.N",  'c',  &filter.cfiri.M,  sizeof(filter.cfiri.M)}, 

  // ---------------------------------------------------------------
  // r|w firi.Sb <val_0> ... <val_M-1>
  {"firi.Sb", 's',  filter.cfiri.Sb,  sizeof(filter.cfiri.Sb)/sizeof(short)},

  // ---------------------------------------------------------------
  // r|w iirf.Na <val>
  {"iirf.Na", 'c',  &filter.ciirf.Na, sizeof(filter.ciirf.Na)}, 
  // r|w iirf.Nb <val>
  {"iirf.Nb", 'c',  &filter.ciirf.Nb, sizeof(filter.ciirf.Nb)}, 

  // ---------------------------------------------------------------
  // r|w iirf.a <val_0> ... <val_Na-1>
  {"iirf.a",  'f',  filter.ciirf.a,   sizeof(filter.ciirf.a)/sizeof(float)},
  // r|w iirf.b <val_0> ... <val_Nb-1>
  {"iirf.b",  'f',  filter.ciirf.b,   sizeof(filter.ciirf.b)/sizeof(float)},

  // ---------------------------------------------------------------
  // r|w iiri.Na <val>
  {"iiri.Na", 'c',  &filter.ciiri.Na, sizeof(filter.ciiri.Na)}, 
  // r|w iiri.Nb <val>
  {"iiri.Nb", 'c',  &filter.ciiri.Nb, sizeof(filter.ciiri.Nb)}, 

  // ---------------------------------------------------------------
  // r|w iiri.Sa <val_0> ... <val_Na-1>
  {"iiri.Sa", 'i',  filter.ciiri.Sa,  sizeof(filter.ciiri.Sa)/sizeof(int)},
  // r|w iiri.Sb <val_0> ... <val_Nb-1>
  {"iiri.Sb", 'i',  filter.ciiri.Sb,  sizeof(filter.ciiri.Sb)/sizeof(int)},

  // ---------------------------------------------------------------
  // r|w sosf.N <val>
  {"sosf.N",  'c',  &filter.csosf.Nsos, sizeof(filter.csosf.Nsos)}, 

  // ---------------------------------------------------------------
  // r|w sosf0..9 <val_0> ... <val_5>
  {"sosf0", 'f',  &filter.csosf.iir2f[0], sizeof(filter.csosf.iir2f)/sizeof(float)},
  {"sosf1", 'f',  &filter.csosf.iir2f[1], sizeof(filter.csosf.iir2f)/sizeof(float)},
  {"sosf2", 'f',  &filter.csosf.iir2f[2], sizeof(filter.csosf.iir2f)/sizeof(float)},
  {"sosf3", 'f',  &filter.csosf.iir2f[3], sizeof(filter.csosf.iir2f)/sizeof(float)},
  {"sosf4", 'f',  &filter.csosf.iir2f[4], sizeof(filter.csosf.iir2f)/sizeof(float)},
  {"sosf5", 'f',  &filter.csosf.iir2f[5], sizeof(filter.csosf.iir2f)/sizeof(float)},
  {"sosf6", 'f',  &filter.csosf.iir2f[6], sizeof(filter.csosf.iir2f)/sizeof(float)},
  {"sosf7", 'f',  &filter.csosf.iir2f[7], sizeof(filter.csosf.iir2f)/sizeof(float)},
  {"sosf8", 'f',  &filter.csosf.iir2f[8], sizeof(filter.csosf.iir2f)/sizeof(float)},
  {"sosf9", 'f',  &filter.csosf.iir2f[9], sizeof(filter.csosf.iir2f)/sizeof(float)},

  // ---------------------------------------------------------------
  // r|w sosi.N <val>
  {"sosi.N",  'c',  &filter.csosi.Nsos, sizeof(filter.csosi.Nsos)}, 

  // ---------------------------------------------------------------
  // r|w sosi0..9 <val_0> ... <val_5>
  {"sosi0", 'i',  &filter.csosi.iir2i[0], sizeof(filter.csosi.iir2i)/sizeof(int)},
  {"sosi1", 'i',  &filter.csosi.iir2i[1], sizeof(filter.csosi.iir2i)/sizeof(int)},
  {"sosi2", 'i',  &filter.csosi.iir2i[2], sizeof(filter.csosi.iir2i)/sizeof(int)},
  {"sosi3", 'i',  &filter.csosi.iir2i[3], sizeof(filter.csosi.iir2i)/sizeof(int)},
  {"sosi4", 'i',  &filter.csosi.iir2i[4], sizeof(filter.csosi.iir2i)/sizeof(int)},
  {"sosi5", 'i',  &filter.csosi.iir2i[5], sizeof(filter.csosi.iir2i)/sizeof(int)},
  {"sosi6", 'i',  &filter.csosi.iir2i[6], sizeof(filter.csosi.iir2i)/sizeof(int)},
  {"sosi7", 'i',  &filter.csosi.iir2i[7], sizeof(filter.csosi.iir2i)/sizeof(int)},
  {"sosi8", 'i',  &filter.csosi.iir2i[8], sizeof(filter.csosi.iir2i)/sizeof(int)},
  {"sosi9", 'i',  &filter.csosi.iir2i[9], sizeof(filter.csosi.iir2i)/sizeof(int)},

  // ---------------------------------------------------------------
  // r|w garb.N <val>
  {"garb.N",  's',  &fgen_arb[0], sizeof(short)}, 

  // ---------------------------------------------------------------
  // r|w garb <val>
  {"garb.Sb", 's',  &fgen_arb[1], sizeof(fgen_arb)/sizeof(short)-1},
  
};

// -------------------------------------------------------------
TSVAR *get_var(char *var, int *noff) {
  int ind = -1;
  int off = 0;
  char *pc;

  if (!var) return NULL;
  pc = strchr(var,'+');
  if (pc)
    off = atoi(pc+1);
  
  for (int i=0; i < NVARS; i++) {
    if (strnicmp(var, tsvar[i].name, strlen(tsvar[i].name))==0) {
      ind = i;
      break;
    }
  }
  if (ind < 0) return NULL;
  if (noff) *noff = off;
  return &tsvar[ind];
}
