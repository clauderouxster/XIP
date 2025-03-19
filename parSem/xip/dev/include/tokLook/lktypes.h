/* $Id: lktypes.h,v 1.2 2005/06/03 14:22:12 grondeau Exp $ */
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
   filename   : lktypes.h
   Date       : 21/01/2002
   Purpose    : Encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/

/*************************************************************
 *
 *  FILE  "lktypes.h"
 *
 *      DEFINITION OF TYPES, VARIABLES, STRUCTURES, ETC. FOR
 *      LOOK-UP OF WORDS IN DICTIONARIES
 *
 *  Andre Kempe, RANK XEROX, Grenoble, France
 *  30/01/96 - ...
 *
 *************************************************************/

#ifndef LKTYPES_H
#define LKTYPES_H

#include "lookup_windefs.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "fst/types.h"
#include "fst/apply.h"   /* For using the compose-apply algorithm */

#ifdef __cplusplus
extern "C" {
#endif

/* use the following definition if you compile for SunOS */

#if defined(SUNOS)
#define memmove(A,B,N)  bcopy(B,A,N)
#endif


#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif

#ifndef min_of
#define min_of(X,Y)  (X<Y)?X:Y
#define max_of(X,Y)  (X>Y)?X:Y
#endif

#define PROGRAM_NAME    "LOOKUP"

#define L_LINE              2000
#define L_SEPARATOR           30
#define MAX_TOKENS          1000
#define REALLOC_FACTOR         2.0

#define SCRIPT_SEPARATORS  " \t\n"
#define INPUT_SEPARATORS   "\t\n"
#define OUTPUT_SEPARATOR   "\t"
/* #define OUTPUT_SEPARATOR   '\t' */

#define LOOKUP_UNKNOWN_SYMBOL '?'
#define LOOKUP_UNKNOWN_WORD_TAG "+?"

/*===================  Default script definitions  ====================*/

#define OPT_LANGUAGE     "-l"
#define OPT_SCRIPT       "-f"

enum lang_p {
  lg_BASE,
  lg_ENGLISH,
  lg_FRENCH,
  lg_GERMAN,
  lg_DUTCH,
  lg_SPANISH,
  lg_PORTUGUESE,
  lg_ITALIAN
};


#define LANG_LOOK_ENGLISH     "english"
#define LANG_LOOK_FRENCH      "french"
#define LANG_LOOK_GERMAN      "german"
#define LANG_LOOK_DUTCH       "dutch"
#define LANG_LOOK_SPANISH     "spanish"
#define LANG_LOOK_PORTUGUESE  "portuguese"
#define LANG_LOOK_ITALIAN     "italian"

#define ENV_BASE         "LOOKUP_SCRIPT_BASE"
#define ENV_ENGLISH      "LOOKUP_SCRIPT_ENGLISH"
#define ENV_FRENCH       "LOOKUP_SCRIPT_FRENCH"
#define ENV_GERMAN       "LOOKUP_SCRIPT_GERMAN"
#define ENV_DUTCH        "LOOKUP_SCRIPT_DUTCH"
#define ENV_SPANISH      "LOOKUP_SCRIPT_SPANISH"
#define ENV_PORTUGUESE   "LOOKUP_SCRIPT_PORTUGUESE"
#define ENV_ITALIAN      "LOOKUP_SCRIPT_ITALIAN"

#ifndef LSCR_BASE
#define LSCR_BASE        "/opt/Mltt/tagging/"
#endif
#ifndef LSCR_ENGLISH
#define LSCR_ENGLISH     "english/rxrc/lookup.script"
#endif
#ifndef LSCR_FRENCH
#define LSCR_FRENCH      "french/rxrc/lookup.script"
#endif
#ifndef LSCR_GERMAN
#define LSCR_GERMAN      "german/rxrc/lookup.script"
#endif
#ifndef LSCR_DUTCH
#define LSCR_DUTCH       "dutch/rxrc/lookup.script"
#endif
#ifndef LSCR_SPANISH
#define LSCR_SPANISH     "spanish/rxrc/lookup.script"
#endif
#ifndef LSCR_PORTUGUESE
#define LSCR_PORTUGUESE  "portuguese/rxrc/lookup.script"
#endif
#ifndef LSCR_ITALIAN
#define LSCR_ITALIAN     "italian/rxrc/lookup.script"
#endif

/*===================  Default symbols  ====================*/

#define LEXICON_IDX         0
#define NORMALIZER_IDX      1
#define GUESSER_IDX         2

#define LEXICON_SYMBOL     "l"
#define NORMALIZER_SYMBOL  "n"
#define GUESSER_SYMBOL     "g"

/*===================  Current state  ====================*/

extern int FsmLookupExport READING_SCRIPT;
extern int FsmLookupExport READING_INPUT;
extern long FsmLookupExport CUR_LINE_NUM;

/*===================  Temporary data  ====================*/

typedef struct _tempBuffer {
	int max;
	int size;
	char** buffer;
} tempBuffer;

typedef struct _tempIntBuffer {
	int i;
	int size;
	int max;
	int* buffer;
} tempIntBuffer;

 
struct _LkTmpFiles {
  tempBuffer *symbols;
  tempBuffer *netfile_names;
  tempIntBuffer *strategies;
};
extern struct _LkTmpFiles FsmLookupExport TEMP_FILES;

struct _LkTmpData{
  long num_nets;
  long len_symbol_byffer;
  long num_strategies;
  long max_len_strategy;
};
extern struct _LkTmpData FsmLookupExport TEMP_DATA;

/*===================  Networks  ====================*/

struct _LkNetFile {
  long num;
  FILE **file;
};
extern struct _LkNetFile FsmLookupExport NET_FILES;

extern NVptr FsmLookupExport LEXNETS;            /* transducers: lexicon, guesser, etc. */
extern NVVptr FsmLookupExport STRATEGIES;        /* contains a NV for every strategy */

/*===================  Look-up  ====================*/

/* Global variable required for using the compose-apply algorithm:
   it is an array of apply_context addresses, one for each cascade of the
   strategy list. It is allocated and initialized by the function
   init_compose_apply_for_lookup(). */
extern APPLYptr FsmLookupExport *STRATEGIES_APPLY_CONTEXT;  

/* Result of a word lookup stored in a network-like format: */    
extern NETptr FsmLookupExport LOOKUP_RESULT;   

/* Input/output buffers:
   --------------------- */   
extern char FsmLookupExport *LOOKUP_INPUT_WORD;       /* input word */
extern char FsmLookupExport *LOOKUP_OUTPUT_BUFFER;    /* output buffer */
extern id_type FsmLookupExport *LOOKUP_ID_SEQUENCE;  /* input, output and itermediate ID sequences */
extern long FsmLookupExport ID_SEQUENCE_LENGTH;    /* lenght of ALL  ID sequences */

extern id_type FsmLookupExport *NORM_ID_SEQUENCE;  /*  intermediate ID sequences (from normalizing fst-s) tg */
extern ALPHABETptr FsmLookupExport LOOKUP_ID_TABLE;  /* char to ID conversion table */
extern ALPHABETptr FsmLookupExport LOOKUP_CHAR_TABLE; /* ID to char conversion table */
extern uint16 FsmLookupExport *LOOKUP_CHAR_VECT; /* ID to char conversion vector */

extern PARSE_TBL FsmLookupExport LOOKUP_PARSE_TABLE;

extern id_type FsmLookupExport *LOOKUP_INPUT_MATCH;  /* match table of the input side */
extern id_type FsmLookupExport *LOOKUP_OUTPUT_MATCH; /* match table of the output side */

extern FILE FsmLookupExport *LOOKUP_INPUT_FILE;
extern FILE FsmLookupExport *LOOKUP_OUTPUT_FILE;

extern void FsmLookupExport (*LOOKUP_print_result_fn)(id_type *,void*);

#define LOOKUP_CMD_LINE_FLAGS "-flags"
#define ENV_LOOKUP_FLAGS      "LOOKUP_FLAGS"
#define DEF_LOOKUP_FLAGS      "cKv29"

#define FLAG_SHOW_COMMENTS      'c'
#define FLAG_MAKE_RESULT_NET    'n'
#define FLAG_APPLY_WITH_CHECK   'k'
#define FLAG_CHECKNET_FROM_FILE 'K'
#define FLAG_VECTORIZE_NETS     'v'
#define FLAG_EVALUATE_CHECK     'e'
#define FLAG_DONT_COPY_INPUT    'x'

#define FLAG_MULTI_CHAR_SYMBOLS 'm'
#define FLAG_MCHAR_INPUT_SIDE   'i'
#define FLAG_MCHAR_UPPER_SIDE   'u'
#define FLAG_MCHAR_LOWER_SIDE   'l'
#define FLAG_MCHAR_BOTH_SIDES   'b'

#define FLAG_SURF_LEMMA_SEP     'L'
#define FLAG_LEMMA_TAG_SEP      'T'

#define FLAG_INPUT_LENGTH       'I'

struct _LkFlags {
  char *flag_string;

  fst_bit show_comments : 1;
  fst_bit make_result_net : 1;
  fst_bit apply_with_check : 1;
  fst_bit checknet_from_file : 1;
  fst_bit vectorize_nets : 1;
  fst_bit evaluate_check : 1;
  fst_bit multi_char_symbols : 1;  /* input with multi-char symbols allowed */
  fst_bit dont_copy_input : 1;   /* input will not be copied to the output */

  long check_net_min;     /* a check net for fst with with >= n states */
  long vectorize_min;     /* vectorization for states with >= n arcs */
  long evaluate_num;
     /* number of words for evaluation of each apply_to_nv_... function */
  char multi_char_side;     /* side of multi-char symbols */
  char surf_lemma_sep [L_SEPARATOR];
  char lemma_tag_sep [L_SEPARATOR];

  long len_inp_line;          /* max. length of an input line */
};
extern struct _LkFlags FsmLookupExport LOOKUP_FLAGS;

extern long FsmLookupExport *SUCCESS_COUNT;

extern time_t FsmLookupExport LOOKUP_SECONDS;
extern long FsmLookupExport NUM_WORDS_TOTAL;
extern long FsmLookupExport NUM_WORDS_NOT_FOUND;

enum EVALU_PHASE \
  {EVALU_begin, EVALU_without_check, EVALU_with_check, EVALU_end};

struct _LkEvaluate {
  int phase;
  int num_mapped;    /* number of mapped words in current phase */
  long timeA;        /* cycles for mapping without check */
  long timeB;        /* cycles for mapping with check */
};
extern struct _LkEvaluate FsmLookupExport EVALUATE;

/* Alternate algorithm types Tamas Gaal April 1996 */

typedef struct lower_sigs_net {
  ALPHABETptr low_sig;
  NETptr net;
} lower_sigma_net, *lower_sigma_netptr;

extern lower_sigma_netptr FsmLookupExport LOWER_SIGMAS_LEXNETS;

/*===================  Symbols  ====================*/

#define HASH_LEN_FACTOR   1.4
#define NO_INDEX         -1

typedef struct HASH_TYPE *HASHptr;
typedef struct HASH_TYPE {
  char **item;
  HASHptr next;           /* next hash cell with same address */
}
HASHtype;

struct TAG_HASH_TYPE {
  long num_symbols;
  char **symbol;
  HASHptr hash_list;
  
  long bf_len;
  char *buffer;

  long hash_tab_len;
  HASHptr *hash_tab;
};
extern struct TAG_HASH_TYPE FsmLookupExport SYMBOL;

#ifdef __cplusplus
}
#endif

#endif /* LKTYPES_H */
