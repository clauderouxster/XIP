/* $Id: tokenize_windefs.h,v 1.1 2005/06/03 12:11:51 grondeau Exp $ */
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
   filename   : tokenize_windefs.h
   Date       : 21/01/2002
   Purpose    : Encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/

/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */


/******************************************************
 **
 **  TOKENIZE_WINDEFS.H
 **
 **  XRCE, Decenmber 1999
 **
 **  Definitions for windows DLL BUILD
 **
 ******************************************************/

#ifndef _FsmTokenizeWinDefs_hh
#define _FsmTokenizeWinDefs_hh

#ifdef _WINDOWS
/* Be sure WIN32 is defined */
#if defined(_WIN32) && !defined(WIN32)
#define WIN32 
#endif

/* FSMTOKENIZE DLL: _FSMTOKENIZEDLL and _FSMTOKENIZEBUILDDLL FLAGS */
#ifdef _FSMTOKENIZEDLL
#  if !defined(__LARGE__) && !defined(_WIN32)
#    error   Must use large or flat memory model when compiling or using the FsmTokenize DLL!
#  endif
#  ifdef _FSMTOKENIZEBUILDDLL
     /* Compiling the tokenize DLL. */
#    ifndef _MSC_VER
#      define FsmTokenizeExport _export	                
#    else /* Microsoft: */
//#error GOOD EXPORT
#      define FsmTokenizeExport __declspec(dllexport)
#    endif
#  else
     /* Using the tokenize DLL. */
#    if defined(__WIN16__)
#      define FsmTokenizeExport far
#    elif defined(_WIN32)
#      ifndef _MSC_VER
#        define FsmTokenizeExport _import
#      else
//#error GOOD import
#        define FsmTokenizeExport __declspec(dllimport)
#      endif  
#    endif
#  endif
#else
   /* Neither compiling, nor using the tokenize DLL. */
#  define FsmTokenizeExport
#endif

#else
   /* Not windows plateform. */
#  define FsmTokenizeExport
#endif /* _WINDOWS */ 
#endif /* _FsmTokenizeWinDefs_hh */
