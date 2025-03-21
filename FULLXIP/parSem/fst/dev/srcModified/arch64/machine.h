/* $Id: machine.h,v 1.1 2008/03/11 16:52:42 grondeau Exp $ */
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

#ifdef ARCH64
#define ALONG	unsigned int
#else
#define ALONG	unsigned long
#endif

fst_byte FstBaseExport next_byte(FILE *stream, ALONG *byte_countp);
unsigned int next_two_bytes(FILE *stream, ALONG *byte_countp);
ALONG next_three_bytes(FILE *stream, ALONG *byte_countp);
ALONG next_four_bytes(FILE *stream, ALONG *byte_countp);

void write_byte(fst_byte b, FILE *stream, ALONG *byte_countp);
void write_two_bytes(unsigned int b, FILE *stream, ALONG *byte_countp);
void write_three_bytes(ALONG b,  FILE *stream, ALONG *byte_countp);
void write_four_bytes(ALONG b, FILE *stream, ALONG *byte_countp);

#ifdef __cplusplus
}
#endif

#endif /* _FsmMachine_h */

