// ----------------------------------------------------------------------------
/// \file     vars.h
/// \brief     allows read|write access by identifier in ccomm module.
/// \license     For educational purposes only. No Warranty. See License.txt
/// \author    Wolfgang Schulter
/// \date    12.04.2020 ws:  initial version (RTSim ~ Arduino) version

#ifndef __VARS_H__  // only once
#define __VARS_H__

typedef struct _tsvar {
  char name[20];    // a variable's ID (short form), optional with offset of xx elements: var+x
  char type;      // element type b|c = u|int8_t, S|s = u|int16_t, I|i = u|int32_t, f = float
  void* addr;     // void * address of the variable 
  int nele;     // number of successive elements stored at addr
} TSVAR;

TSVAR *get_var(char *var, int *noff);

#endif // __VARS_H__
