/* $Id: lookup_windefs.h,v 1.1 2005/06/03 12:11:51 grondeau Exp $ */
/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.60
   filename   : lookup_windefs.h
   Date       : 21/01/2002
   Purpose    : Encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/
/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */


/******************************************************
 **
 **  LOOKUP_WINDEFS.H
 **
 **  XRCE, Decenmber 1999
 **
 **  Definitions for windows DLL BUILD
 **
 ******************************************************/

#ifndef _FsmLookupWinDefs_hh
#define _FsmLookupWinDefs_hh

#ifdef _WINDOWS
/* Be sure WIN32 is defined */
#if defined(_WIN32) && !defined(WIN32)
#define WIN32 
#endif

/* FSMLOOKUP DLL: _FSMLOOKUPDLL and _FSMLOOKUPBUILDDLL FLAGS */
#ifdef _FSMLOOKUPDLL
#  if !defined(__LARGE__) && !defined(_WIN32)
#    error   Must use large or flat memory model when compiling or using the FsmLookup DLL!
#  endif
#  ifdef _FSMLOOKUPBUILDDLL
     /* Compiling the lookup DLL. */
#    ifndef _MSC_VER
#      define FsmLookupExport _export	                
#    else /* Microsoft: */
//#error GOOD EXPORT
#      define FsmLookupExport __declspec(dllexport)
#    endif
#  else
     /* Using the lookup DLL. */
#    if defined(__WIN16__)
#      define FsmLookupExport far
#    elif defined(_WIN32)
#      ifndef _MSC_VER
#        define FsmLookupExport _import
#      else
//#error GOOD import
#        define FsmLookupExport __declspec(dllimport)
#      endif  
#    endif
#  endif
#else
   /* Neither compiling, nor using the lookup DLL. */
#  define FsmLookupExport
#endif

#else
   /* Not windows plateform. */
#  define FsmLookupExport
#endif /* _WINDOWS */ 

#endif /* _FsmLookupWinDefs_hh */
