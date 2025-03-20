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

/*  Function prototypes for LEXICON.Y  */
#define STRING_BLOCK_SIZE 16000
void reset_string_mem(void);
void free_string_mem(void);
char *new_str(char *str);
FAT_STR make_sublex_name(char *str);

void begin_syms(void);
void end_alph(void);
void no_alph(void);

void begin_lexicons(void);
void end_lexicons(void);
NETptr link_and_cleanup(int quit_on_fail_p);
void begin_lexicon(FAT_STR name);
void end_lexicon(void);
void add_simple_entry(char *word, FAT_STR class);
void add_pair_entry(char *word1, char *word2, FAT_STR class);
void add_regex_entry(NETptr net, FAT_STR class);

char *rm_escapes(char *str);
char *rm_mult_escapes(char *str);
void info_warning(void);
void lexicon_error_cleanup(void);


NETptr lexc_regex(char *regex_str);

void lex_add_prop(FAT_STR attr, OBJECTptr value);
