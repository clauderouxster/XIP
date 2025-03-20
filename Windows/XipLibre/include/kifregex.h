/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 10.00
   filename   : kifregex.h
   Date       : 09/09/2010
   Purpose    : Regex Library
   Programmer : Claude ROUX
   Reviewer   : 
*/


#ifndef i_kifregex
#define i_kifregex


#if defined(WIN32) || defined(MAVERICK) 
#include <regex>
#else
#include <boost/regex.hpp>
#endif

#ifdef WIN32 
using namespace std::tr1;
#else
#ifndef MAVERICK
using namespace boost;
#endif
#endif

#endif







