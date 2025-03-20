#ifndef NTM_TOKEN_H
#define NTM_TOKEN_H

#include "ntmtypes.h"


/* entry structure */
typedef struct Entry {
    id_type *normform; /* the normalized wordform */
    id_type *lemma;    /* the lemma of this entry */
    id_type *features; /* the (morpho.) features of this entry */
} Entry;


/* token structure */
typedef struct Token {
    int initpos;        /* initial pos of this token in inputBuffer */
    int inputLength;    /* length of this token */
    unsigned char strategy; /* number of the strategy used */
    ntmbit entryNb : 8;        /* number of lexical entries for this token */
    ntmbit weight : 6;
    ntmbit ignore : 1;
    ntmbit tambig : 1;
    struct Entry * entrySeq;   /* sequence of lexical entries for this token */
    struct Token * longerToken; /* ptr to longer token with same initpos*/
    struct Token * shorterToken; /* ptr to shorter token with same initpos*/
    struct Token * previousToken; /* ptr to previous token */
    struct Token * nextToken; /* ptr to next following token */
} Token;



/* methods for tokens */

extern int ntmIsEofToken(Token *token);


/* adds an entry to the token */
extern void addEntryToToken( Token *token, Entry *e);
extern Token *getLastToken();




#endif /* NTM_TOKEN_H */
