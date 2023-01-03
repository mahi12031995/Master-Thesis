// ----------------------------------------------------------------------------
/// \file			ccomm.h
/// \author			Wolfgang Schulter 
/// \copyright		(c) Wolfgang Schulter.
/// \date			05.04.2020 ws:  initial version
// ----------------------------------------------------------------------------

#ifndef __CCOMM_H__
#define __CCOMM_H__

#define NRXBUF 		1024	//128  RTSim: for very long lines

extern char rxbuf[NRXBUF];
extern uint8_t nrxbuf;

void comm_analyze();
void comm_tic();

#endif // __CCOMM_H__
