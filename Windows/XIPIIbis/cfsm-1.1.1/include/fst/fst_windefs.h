/* $Id: fst_windefs.h,v 1.4 2002/12/17 16:55:01 grayrat Exp $ */
/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */


/******************************************************
 **
 **  FST_WINDEFS.H
 **
 **  XRCE, Decenmber 1999
 **
 **  Definitions for windows DLL BUILD
 **
 ******************************************************/

#ifndef _FsmFstWinDefs_hh
#define _FsmFstWinDefs_hh

#if defined _WINDOWS || defined UNDER_CE
/* Be sure WIN32 is defined */
#if defined(_WIN32) && !defined(WIN32)
#define WIN32 
#endif

/* FSMFST DLL: _FSMFSTDLL and _FSMFSTBUILDDLL FLAGS */
#ifdef _FSMFSTDLL
#  if !defined(__LARGE__) && !defined(_WIN32)
#    error   Must use large or flat memory model when compiling or using the FsmFst DLL!
#  endif
#  ifdef _FSMFSTBUILDDLL
     /* Compiling the fst DLL. */
#    ifndef _MSC_VER
#      define FsmFstExport _export	                
#    else /* Microsoft: */
#      define FsmFstExport __declspec(dllexport)
#    endif
#  else
     /* Using the fst DLL. */
#    if defined(__WIN16__)
#      define FsmFstExport far
#    elif defined(_WIN32)
#      ifndef _MSC_VER
#        define FsmFstExport _import
#      else
#        define FsmFstExport __declspec(dllimport)
#      endif  
#    endif
#  endif
#else
   /* Neither compiling, nor using the fst DLL. */
#  define FsmFstExport
#endif

/* Windows CE Specific definitions */
#ifdef UNDER_CE
#include "fst_wincedefs.h"
#endif

#else /* !defined _WINDOWS && !defined UNDER_CE */
   /* Not windows plateform. */
#  define FsmFstExport
#endif /* _WINDOWS || defined UNDER_CE */ 
#endif /* _FsmFstWinDefs_hh */
