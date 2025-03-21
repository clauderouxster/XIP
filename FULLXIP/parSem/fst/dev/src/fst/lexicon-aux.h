/* Copyright (c) 1992 by the Xerox Corporation.  All rights reserved */

#define _H_gram

/******************************************************
 **
 **  lexicon-aux.h
 **
 **  Function prototypes for parsing Two-Level Lexicons.
 **  Todd Yampol, Xerox PARC, 12/92.
 **
 ******************************************************/
#include "fst/fst_windefs.h"

/*  Function prototypes for LEXICON.Y  */
#define STRING_BLOCK_SIZE 16000
void FstCalcExport reset_string_mem(void);
void FstCalcExport free_string_mem(void);
char FstCalcExport *new_str(char *str);
FAT_STR FstCalcExport make_sublex_name(char *str);

void FstCalcExport begin_syms(void);
void FstCalcExport end_alph(void);
void FstCalcExport no_alph(void);

void FstCalcExport begin_lexicons(void);
void FstCalcExport end_lexicons(void);
NETptr FstCalcExport link_and_cleanup(int quit_on_fail_p);
void FstCalcExport begin_lexicon(FAT_STR name);
void FstCalcExport end_lexicon(void);
void FstCalcExport add_simple_entry(char *word, FAT_STR class);
void FstCalcExport add_pair_entry(char *word1, char *word2, FAT_STR class);
void FstCalcExport add_regex_entry(NETptr net, FAT_STR class);

char FstCalcExport *rm_escapes(char *str);
char FstCalcExport *rm_mult_escapes(char *str);
void FstCalcExport info_warning(void);
void FstCalcExport lexicon_error_cleanup(void);


NETptr FstCalcExport lexc_regex(char *regex_str);

void FstCalcExport lex_add_prop(FAT_STR attr, OBJECTptr value);

void FstCalcExport parse_lexicon();
