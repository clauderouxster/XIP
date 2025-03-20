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


#define modeOuverture ios::in|ios::binary
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>


using namespace std;
#define uchar unsigned char
#define STR(x) (char*)x.c_str()
#define USTR(x) (unsigned char*)x.c_str()

#endif
