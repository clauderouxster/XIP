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
   filename   : parsbin.h
   Date       : 10/01/1999
   Purpose    : Principal header of XIP
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef parsbin_h
#define parsbin_h

//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//#ifndef DBG_NEW
//	#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//	#define new DBG_NEW
//#endif
//#endif 

#define XIPBUFFERSIZE 1024
#define XIPBUFFERMAX 1023

#ifdef UNIX
#include <unistd.h>
#endif

#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#if (defined(JMAX_DLL) || defined(XIPSTLAPI) )

#define modeOuverture ios::in|ios::binary

#ifdef AUCUNDOMAINE
#include <ostream.h>
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#else
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>


using namespace std;
#endif

#else

#ifdef XIPSTDC
#define modeOuverture ios::in|ios::binary
#else
#define modeOuverture ios::in|ios::nocreate|ios::binary
#endif

#include <fstream.h>
#include <iostream.h>

#ifdef UNIX
#include <strstream.h>
#else
#include <strstrea.h>
#endif

#endif


#include "defun.h"

#ifdef XIPSTLAPI
#include "xipstlres.h" 
#else
#include "xipresult.h"
#endif

#include "kif.h"
#include "variable.h"

#include "divers.h"
#include "avlcat.h"
#include "avlvoc.h"
#include "noeud.h"
#include "lire.h"

#include "compatible.h"
#include "vecteur.h"
#include "avlreg.h"
#include "avltrait.h"
#include "pls.h"
#include "lliste.h"
#include "mathemat.h"
#include "avlfonc.h"
#include "indexation.h"
#include "affiche.h"
#include "automate.h"

#include "xmlxip.h"

#include "ontologies.h"

char RecreeChainesFrancais(char* nom);
char RecreeChainesAnglais(char* nom);

void insere_erreur(char* Message);
char lespace(unsigned char);

#ifndef WIN32
#define strcmpi strcasecmp
#define sprintf_s snprintf
#define strcpy_s(a,b,c) strncpy(a,c,b)
#define strcat_s(a,b,c) strncat(a,c,b)
#endif


void LibereParseur(unsigned int iParseur);
GlobalParseur* SelectParseur(unsigned int iParseur);
void DetruitEcran();
void InitialisationSignaux();

#ifdef XIPPYTHON

#define Py_UNICODE_WIDE

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif
#endif

#endif

