/* $Id: machine.h,v 1.3 1998/10/24 09:48:30 lk Exp $ */
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

#include "struct.h"


byte next_byte(FILE *stream, unsigned long *byte_countp);
unsigned int next_two_bytes(FILE *stream, unsigned long *byte_countp);
unsigned long next_three_bytes(FILE *stream, unsigned long *byte_countp);
unsigned long next_four_bytes(FILE *stream, unsigned long *byte_countp);

void write_byte(byte b, FILE *stream, unsigned long *byte_countp);
void write_two_bytes(unsigned int b, FILE *stream, unsigned long *byte_countp);
void write_three_bytes(unsigned long b,  FILE *stream, unsigned long *byte_countp);
void write_four_bytes(unsigned long b, FILE *stream, unsigned long *byte_countp);


#ifdef __cplusplus
}
#endif

#endif /* _FsmMachine_h */

