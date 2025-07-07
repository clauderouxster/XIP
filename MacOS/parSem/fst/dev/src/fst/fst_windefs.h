/* $Id: fst_windefs.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
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
#  if defined(_WIN32) && !defined(WIN32)
#    define WIN32 
#  endif
/* FSTBASE DLL: _FSTBASEDLL and _FSTBASEBUILDDLL FLAGS */
#  ifdef _FSTBASEDLL
#    if !defined(__LARGE__) && !defined(_WIN32)
#      error   Must use large or flat memory model when compiling or using the FsmFst DLL!
#    endif
#    ifdef _FSTBASEBUILDDLL
       /* Compiling the fstbase DLL. */
#      if defined(_MSC_VER) || defined(__MINGW32__)
#        define FstBaseExport __declspec(dllexport)
#      else /* unknown */
#        define FstBaseExport _export	                
#      endif
#    else
       /* Using the fstbase DLL. */
#      if defined(__WIN16__)
#        define FstBaseExport far
#      elif defined(_WIN32)
#        if defined(_MSC_VER) || defined(__MINGW32__)
#          define FstBaseExport __declspec(dllimport)
#        else /* unknown */
#          define FstBaseExport _import                   
#        endif
#      endif
#    endif
#  else
     /* Neither compiling, nor using the fstbase DLL. */
#    define FstBaseExport
#  endif
/* FSTCALC DLL: _FSTCALCDLL and _FSTCALCBUILDDLL FLAGS */
#  ifdef _FSTCALCDLL
#    if !defined(__LARGE__) && !defined(_WIN32)
#      error   Must use large or flat memory model when compiling or using the FsmFst DLL!
#    endif
#    ifdef _FSTCALCBUILDDLL
       /* Compiling the fstcalc DLL. */
#      if defined(_MSC_VER) || defined(__MINGW32__)
#        define FstCalcExport __declspec(dllexport)
#      else /* unknown */
#        define FstCalcExport _export                   
#      endif
#    else
       /* Using the fstbase DLL. */
#      if defined(__WIN16__)
#        define FstCalcExport far
#      elif defined(_WIN32)
#        if defined(_MSC_VER) || defined(__MINGW32__)
#          define FstCalcExport __declspec(dllimport)
#        else /* unknown */
#          define FstCalcExport _import                   
#        endif
#      endif
#    endif
#  else
     /* Neither compiling, nor using the fstcalc DLL. */
#    define FstCalcExport
#  endif
#else /* !defined _WINDOWS && !defined UNDER_CE */
   /* Not windows plateform. */
#  define FstBaseExport
#  define FstCalcExport
#endif /* _WINDOWS || defined UNDER_CE */ 

/* Windows CE Specific definitions */
#ifdef UNDER_CE
#include "fst/fst_wincedefs.h"
#endif

#if defined(_WIN32) && ! defined(__GNUC__) 
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define alloca _alloca
#define isatty _isatty
#define chdir _chdir
#endif

#endif /* _FsmFstWinDefs_hh */
