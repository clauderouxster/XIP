/* $Id: machine.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1994 by the Xerox Corporation.  All rights reserved */

/*********************************************************************
 *                                                                   *
 *  MACHINE.H
 *                                                                   *
 *********************************************************************/

#ifndef _FsmMachine_h
#define _FsmMachine_h

#ifdef __cplusplus
extern "C" {
#endif

#include "fst/struct.h"


fst_byte FstBaseExport next_byte(FILE *stream, unsigned long *byte_countp);
unsigned int next_two_bytes(FILE *stream, unsigned long *byte_countp);
unsigned long next_three_bytes(FILE *stream, unsigned long *byte_countp);
unsigned long next_four_bytes(FILE *stream, unsigned long *byte_countp);

void write_byte(fst_byte b, FILE *stream, unsigned long *byte_countp);
void write_two_bytes(unsigned int b, FILE *stream, unsigned long *byte_countp);
void write_three_bytes(unsigned long b,  FILE *stream, unsigned long *byte_countp);
void write_four_bytes(unsigned long b, FILE *stream, unsigned long *byte_countp);

#ifdef __cplusplus
}
#endif

#endif /* _FsmMachine_h */

