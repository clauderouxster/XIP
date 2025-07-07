/* $Id: syst-dep.h,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

#ifndef C_FSM_SYST_DEP
#define  C_FSM_SYST_DEP

#include "fst/fst_windefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/****
 *
 *	syst-dep.h
 *
 ****/
 

/* File creation dates are encoded using Common Lisp time standard.
   Time 1 is 12:00:01 AM January 1, 1900.  On Macintosh, native
   time starts four years later; in Unix, the beginning of time
   is Jan 17, 1970.  How is it in the IBM world?
   */
     
#ifdef MAC_OS
#define CL_TIME_OFFSET  (unsigned long) 4*365*24*60*60
#define int16	int
#define uint16	unsigned int
#define int32	long int
#define uint32	unsigned long int
#else
#define CL_TIME_OFFSET  (unsigned long) 70 * 365*24*60*60 + 17*24*60*60
#define int16	short 
#define uint16	unsigned short 
#define int32	int
#define uint32	unsigned int
#endif

#define cast_to_void(X)   ((void *) (int) (X))

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_SYST_DEP */
