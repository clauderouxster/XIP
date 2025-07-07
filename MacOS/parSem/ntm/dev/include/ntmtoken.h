/* $Id: ntmtoken.h,v 1.1 2005/02/11 17:40:00 grondeau Exp $ */ 

#ifndef NTM_TOKEN_H
#define NTM_TOKEN_H

#include "ntm_windefs.h"
#include "utf8_fatstr_util.h"
#include "ntmtypes.h"


/* entry structure */
typedef struct NtmEntry {
    id_type *normform; /* the normalized wordform */
    id_type *lemma;    /* the lemma of this entry */
    id_type *features; /* the (morpho.) features of this entry */
} NtmEntry;


/* token structure */
typedef struct NtmToken {
    long int initpos;        /* initial pos of this token in inputBuffer : char offset */
    short int inputLength;    /* length of this token in characters (ISO, UTF etc.) */
    short int inputByteLength;    /* length of this token in BYTES */
    long int initBytepos;        /* initial pos in input stream : BYTE offset */
   
    unsigned char strategy; /* number of the strategy used */
    ntmbit entryNb : 8;        /* number of lexical entries for this token */
    ntmbit weight : 6;
    ntmbit ignore : 1;
    ntmbit tambig : 1;
    struct NtmEntry * entrySeq;   /* sequence of lexical entries for this token */
    struct NtmToken * longerToken; /* ptr to longer token with same initpos*/
    struct NtmToken * shorterToken; /* ptr to shorter token with same initpos*/
    struct NtmToken * previousToken; /* ptr to previous token */
    struct NtmToken * nextToken; /* ptr to next following token */
} NtmToken;



/* methods for tokens */

extern int FsmNTMExport ntmIsEofToken(NtmToken *token);


/* adds an entry to the token */
extern void addEntryToToken( NtmToken *token, NtmEntry *e);
extern NtmToken *getLastToken();




#endif /* NTM_TOKEN_H */
