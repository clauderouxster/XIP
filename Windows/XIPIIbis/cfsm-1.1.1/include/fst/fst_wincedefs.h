/* $Id: fst_wincedefs.h,v 1.3 2002/12/17 16:55:00 grayrat Exp $ */
/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */


/******************************************************
 **
 **  FST_WINCEDEFS.H
 **
 **  XRCE, February 2002
 **
 **  Definitions for windows CE DLL BUILD
 **
 **  Defines Windows CE missing functions or typedefs
 **
 **  ** !!! ONLY INCLUDE IF UNDER WINDOWS CE !!! ***
 **
 ******************************************************/

#ifndef _FsmFstWinCEDefs_hh
#define _FsmFstWinCEDefs_hh

/* We must include stdlib.h before defining qsort macro: 
 * qsort is defined in stdlib.h, so defining macro before 
 * inclusion makes an error at compilation
 */
#include <stdlib.h>

/* Function prototype type def for qsort parameter */
#ifndef QSORT_MACRO_DEFINED
#define QSORT_MACRO_DEFINED
typedef int __cdecl wce_qsort_func_proto(const void*, const void*);
#define qsort(A, B, C, D) qsort(A, B, C, (wce_qsort_func_proto*)D)
#endif

#include "celib.h"

#endif /* _FsmFstWinCEDefs_hh */