/* $Id: ntm_windefs.h,v 1.1 2005/02/11 17:40:00 grondeau Exp $ */
/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */


/******************************************************
 **
 **  NTM_WINDEFS.H
 **
 **  XRCE, Decenmber 1999
 **
 **  Definitions for windows DLL BUILD
 **
 ******************************************************/

#ifndef _FsmNTMWinDefs_hh
#define _FsmNTMWinDefs_hh

#ifdef _WINDOWS
/* Be sure WIN32 is defined */
#if defined(_WIN32) && !defined(WIN32)
#define WIN32 
#endif

/* FSMNTM DLL: _FSMNTMDLL and _FSMNTMBUILDDLL FLAGS */
#ifdef _FSMNTMDLL
#  if !defined(__LARGE__) && !defined(_WIN32)
#    error   Must use large or flat memory model when compiling or using the FsmNTM DLL!
#  endif
#  ifdef _FSMNTMBUILDDLL
     /* Compiling the NTM DLL. */
#    ifndef _MSC_VER
#      define FsmNTMExport _export	                
#    else /* Microsoft: */
//#error GOOD EXPORT
#      define FsmNTMExport __declspec(dllexport)
#    endif
#  else
     /* Using the NTM DLL. */
#    if defined(__WIN16__)
#      define FsmNTMExport far
#    elif defined(_WIN32)
#      ifndef _MSC_VER
#        define FsmNTMExport _import
#      else
//#error GOOD import
#        define FsmNTMExport __declspec(dllimport)
#      endif  
#    endif
#  endif
#else
   /* Neither compiling, nor using the NTM DLL. */
#  define FsmNTMExport
#endif

#else
   /* Not windows plateform. */
#  define FsmNTMExport
#endif /* _WINDOWS */ 
#endif /* _FsmNTMWinDefs_hh */
