/* $Id: versiontok.h,v 1.1 2005/06/03 12:11:51 grondeau Exp $ */
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
   filename   : versiontok.h
   Date       : 21/01/2002
   Purpose    : Encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/

/* Copyright (c) 1990-96 by the Xerox Corporation.  All rights reserved.
   Copyright protection claimed includes all forms and matters of
   copyrightable material and information now allowed by statutory or
   judicial law or hereinafter granted, including without limitation,
   material generated from the software programs which are displayed 
   on the screen such as icons, screen display looks, etc.
   */
/*
 * Description: Version and Revision defintions for LOOKUP
 */

#ifndef TOKEN_LOOKUP_VERSION_H
#define TOKEN_LOOKUP_VERSION_H

#define LOOKUP_VERSION      2
#define LOOKUP_REVISION     3
#define LOOKUP_UPDATE_LEVEL 6
#define LOOKUP_Version      ((LOOKUP_VERSION * 100 + LOOKUP_REVISION) * 100 + LOOKUP_UPDATE_LEVEL)
#define LOOKUP_VERSION_STRING "2.3.6"

#define TOKENIZE_VERSION      2
#define TOKENIZE_REVISION     3
#define TOKENIZE_UPDATE_LEVEL 3
#define TOKENIZE_Version      ((TOKENIZE_VERSION * 100 + TOKENIZE_REVISION) * 100 + TOKENIZE_UPDATE_LEVEL)
#define TOKENIZE_VERSION_STRING "2.3.3"

#define  NONDET_LIMIT_DEFAULT 1000;


#endif /* TOKEN_LOOKUP_VERSION_H */
