/*****************************************************************\
***                                                             ***
***         File: string2id.c                                   ***
***       Author: Andreas Eisele(Univ. Stuttgart), B.M. Schulze ***
***      Purpose: Map strings to integers                       ***
***      Created: Sun Jan 29 13:54:34 1995                      ***
***     Modified: Wed Apr 10 10:04:31 1996 (schulze)            ***
***    Copyright: Xerox Corporation				***
***                                                             ***
\*****************************************************************/
/* Copyright (c) 1996-97 by the Xerox Corporation.  All rights reserved.
   Copyright protection claimed includes all forms and matters of
   copyrightable material and information now allowed by statutory or
   judicial law or hereinafter granted, including without limitation,
   material generated from the software programs which are displayed 
   on the screen such as icons, screen display looks, etc.
   */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <string2id.h>

#define min(lhs, rhs) (((int)lhs) < ((int)rhs) ? (lhs) : (rhs))

/* the hashing function is based on an Perl5's hashing function*/
static const char coeff[] = {
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1};

static unsigned long
hashfunc( const char *str ) {

    register const char    *s = (const char *)str;
    register int            i = 0;
    register unsigned long  hash = 0;
    register int	    len = min( strlen(str), 127 );

    for ( ; i < len; s++, i++,  hash *= 5) 
	hash += *s * coeff[i];

    return hash;
}

/* end of hashing function */


static unsigned long
primep(const unsigned long x) {
    
    register unsigned long i;
    
    for (i = 1 + (unsigned long)sqrt((double)x); i > 1; i--)
	if ((x/i) * i == x) return(0);
    return(1);
}


static unsigned long
find_prime(const unsigned long x) {

  register unsigned long i = x;

  for ( ; i > 0; i--)
    if(primep(i)) return(i);
  
  assert(0);   /* should never be reached */
}


dict_t *
make_dict(unsigned long maxsize, const int backp) {

    register dict_t * res;
    register unsigned long i;

    if (maxsize == 0) return((dict_t *)NULL);

    maxsize = find_prime(maxsize);

    res          = (dict_t *)malloc(sizeof(dict_t));
    res->maxsize = maxsize;
    res->filled  = 0;
    res->backp   = (char **)NULL;
    
    res->strings = (char **)malloc(sizeof(char*)*maxsize);
    res->ids     = (unsigned long *)malloc(sizeof(unsigned long)*maxsize);

    if (backp)
	res->backp = (char **)malloc(sizeof(char*)*maxsize);

    if (!res->ids || !res->strings || (backp && !res->backp)){
	fprintf(stderr,"make_dict: allocation failed\n");
	exit(-1);
    }

    for (i = 0; i < maxsize; i++) {
	res->strings[i] = (char *)NULL;
	res->ids[i]     = ~0;
    }

    if (backp)
	for (i = 0; i < maxsize; i++) 
	    res->backp[i] = (char *)NULL;

    return(res);
}


unsigned long
string2id(const char * string, dict_t * dictionary, const insert_t insert) {

    register unsigned long  hash;
    register char          *s;

    /* treat two special cases ... */
    if (dictionary->maxsize == 0)
        return(-1);
    if (dictionary->maxsize == 1)
        return(0);

    /* get the hash code */
    hash = hashfunc(string) % dictionary->maxsize;
    
    /* use open address method to find next free slot */
    while ((s = dictionary->strings[hash]) && strcmp(string, s))
	if (++hash == dictionary->maxsize) hash = 0;
    
    /* now insert new element */
    if ( (insert != no_insert) && !dictionary->strings[hash]) {
	(dictionary->strings)[hash] = (insert == insert_alloc)? (char *)strdup(string) : (char *)string;
	if (dictionary->backp)
	    dictionary->backp[dictionary->filled] = dictionary->strings[hash];
	dictionary->ids[hash] = dictionary->filled++;
    }

    return(dictionary->ids[hash]);
}


char *
id2string(const unsigned long id, const dict_t * dictionary) {

    if (!dictionary) return((char *)NULL);
    if (!dictionary->backp)
	return("dictionary has no mapping from ids\n");
    else return((dictionary->backp)[id]);
}
