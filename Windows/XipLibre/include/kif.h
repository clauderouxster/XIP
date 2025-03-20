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
   filename   : kif.h
   Date       : 09/09/2010
   Purpose    : Implementation of the KiF language
   Programmer : Claude ROUX
   Reviewer   : 
*/


#ifndef i_kif
#define i_kif

#if defined(WIN32) && defined(KIF_DLL)
#define Exported __declspec(dllexport)
#define Imported __declspec(dllimport)
#else
#define Exported
#define Imported
#endif

#define NON 0
#define OUI 1
#define modeOuverture ios::in|ios::binary
#define uchar unsigned char
#define STR(x) (char*)x.c_str()
#define USTR(x) (uchar*)x.c_str()
#define WSTR(x) (wchar_t*)x.c_str()
#define TailleChaine 512
#ifdef WIN32
#define SEP '\\'
#define Endl "\r\n"
#define ANTISEP '/'
#else
#define sprintf_s snprintf
#define strcpy_s(a,b,c) strncpy(a,c,b)
#define SEP '/'
#define Endl "\n"
#define ANTISEP '\\'
#include <sys/time.h>
#endif

#include "kifn.h"
#include "kifbase.h"
#include "automate.h"
#include "math.h"

double convertfloat(char* s);
Exported void KiFSetdebug(bool d);
Exported string KifExecute(int ikif,string name,vector<string>& parameters,ostringstream* os,bool debugkif);
Exported string Whoami_kif();

Exported void KifRecreatekifGlobals(KifCode* kifcodemain);
Exported void KifReloadLibraries(KifCode* kifcode);
Exported KifCode* Brandnewkifcode(KifCode* kifcodemain);
Exported bool KifRecordLibraries(string name,KifExternalModule);
Exported bool KifRecordMainLibraries(string name,KifExternalModule);
Exported bool KifRecordTerminateModule(string name,KifExternalTerminateModule module);

#endif

