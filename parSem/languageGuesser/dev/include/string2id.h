/*******************************************************************
|***
|***         File: languages.h
|***       Author: Bruno Maximilian Schulze
|***      Purpose: prototypes for string hashing
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

#ifndef _STRHASH_
#define _STRHASH_

typedef struct dict_t {
    unsigned long   maxsize;
    unsigned long   filled;
    char          **strings;
    unsigned long  *ids;
    char          **backp;
} dict_t;


typedef enum i_t { no_insert, insert_copy, insert_alloc } insert_t;

dict_t *
make_dict(unsigned long maxsize, const int backp);

unsigned long
string2id(const char * string, dict_t * dictionary, const insert_t insert);

char *
id2string(const unsigned long id,  const dict_t * dictionary);

#endif
