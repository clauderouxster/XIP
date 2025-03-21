/*******************************************************************
|***
|***         File: languages.h
|***       Author: Bruno Maximilian Schulze (RXRC Grenoble)
|***      Purpose: language definition file
|***      Created:
|***     Modified:
|***    Copyright: Xerox Corporation
|***
\*******************************************************************/
/* Copyright (c) 1996-97 by the Xerox Corporation.  All rights reserved.
   Copyright protection claimed includes all forms and matters of
   copyrightable material and information now allowed by statutory or
   judicial law or hereinafter granted, including without limitation,
   material generated from the software programs which are displayed 
   on the screen such as icons, screen display looks, etc.
   */

#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_


#include "string2id.h"

/* this array contains the language data. for each language there must  *
 * be given two short forms, a full name and some pointers as described *
 *  below.                                                              *
 *                                                                      *
 * NOTE 1: the order of the languages does not imply any ranking.       *
 *                                                                      *
 * NOTE 2: do not change the last line                                  *
 *                                                                      *
 * NOTE 3: use ISO 639 and ISO 639_2 for the letter codes of languages. */
typedef struct lg_t {
    char    *iso639;      /* ISO 639 version 1, two-letter codes.               */
    char    *iso639_2;    /* ISO 639 version 2, three-letter codes.             */
    char    *full;        /* full language name.                                */
    int      used;        /* do you want to use this language?                  */
    dict_t  *dict;        /* language specific data structure used for hashing. */
    double  *hash;	  /* language specific hash table.                      */
    dict_t  *wdict;       /* short word dict.                                   */
    double  *whash;       /* short word hashtable.                              */
    int    (*init)(int);  /* initialization routine.                            */
} lang_tbl;


extern lang_tbl  languages[];
extern const int       _num_langs;


typedef struct lghash_t {
    char   *trig;
    double  prob;
} langhash_t;


typedef struct whash_t {
    char   *word;
    double  prob;
} wordhash_t;


#endif
