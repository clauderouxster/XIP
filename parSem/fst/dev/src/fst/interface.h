/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: interface.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

#ifndef C_FSM_INTERFACE
#define C_FSM_INTERFACE

#include "fst/types.h"

#define IY_IDIOM_ADD_SPACE	interface_parameters.idiom.add_space
#define IY_INTERACTIVE		interface_parameters.parameters.interactive
#define IY_LAST_ERRORS		interface_parameters.regex.lex_errors
#define IY_LEX_ERRORS		interface_parameters.regex.lex_max_errors
#define IY_INORDER		interface_parameters.optimisation.in_order
#define IY_READ_LEFT		interface_parameters.alphabet.read_left
#define IY_PRINT_LEFT		interface_parameters.alphabet.print_left
#define IY_PRINT_PAIRS		interface_parameters.alphabet.print_pairs
#define IY_PRINT_SIGMA		interface_parameters.io.print_sigma_also
#define IY_PRINT_SPACE		interface_parameters.io.more_space
#define IY_MAX_STATE_VISITS     interface_parameters.io.max_state_visits
#define IY_RECURSIVE_DEFINE     interface_parameters.general.recursive_define
#define IY_RECURSIVE_APPLY      interface_parameters.general.recursive_apply
#define IY_COMP_WORDS		interface_parameters.lexicon.compress_words
#define IY_SORT_ARCS		interface_parameters.general.sort_arcs
#define IY_STACK_NUMBER		interface_parameters.general.stack
#define IY_VERBOSE		interface_parameters.general.verbose
#define IY_COMPLETION		interface_parameters.general.completion /*AK*/
#define IY_MARK_VERSION         interface_parameters.io.mark_version
#define IY_NAME_NETS		interface_parameters.general.namenets
#define IY_MINIMIZE_P		interface_parameters.general.minimal
#define IY_OBEY_FLAGS		interface_parameters.io.obey_flags
#define IY_PARSE_MAXRESULTS	interface_parameters.parsing.maxresult
#define IY_PARSE_THRESHOLD	interface_parameters.parsing.maxerrors
#define IY_PARSE_MAXHEURS	interface_parameters.parsing.maxheurs
#define IY_PARSE_SPACE		interface_parameters.parsing.maxspace
#define IY_RETOKENIZE_P         interface_parameters.io.retokenize
#define IY_SHOW_FLAGS		interface_parameters.io.show_flags
#define IY_COUNT_PATTERNS       interface_parameters.io.count_patterns
#define IY_DELETE_PATTERNS      interface_parameters.io.delete_patterns
#define IY_EXTRACT_PATTERNS     interface_parameters.io.extract_patterns
#define IY_MARK_PATTERNS        interface_parameters.io.mark_patterns
#define IY_UNICODE		interface_parameters.alphabet.unicode
#define IY_QUIT_ON_FAIL		interface_parameters.general.quit_on_fail
#define IY_QUOTE_SPECIAL        interface_parameters.general.show_escape
#define IY_COMPOSE_FLAG_AS_SPECIAL        interface_parameters.general.compose_flag_as_special
#define IY_NEED_SEPARATORS      interface_parameters.general.need_separators
#define IY_ASSERT	        interface_parameters.general.assert
#define IY_LICENSE_TYPE         interface_parameters.io.license_type
#define IY_CHAR_ENCODING        interface_parameters.io.character_encoding
#define IY_USE_MMAP             interface_parameters.io.use_memory_map
#define OUTPUT_SIDE (IY_PRINT_PAIRS ? BOTH_SIDES:(IY_PRINT_LEFT ? UPPER:LOWER))
#define INPUT_SIDE (IY_READ_LEFT ? UPPER : LOWER)

#define	WIN_LEN			78

struct STRIntPar
{
  struct regex    /* This component is obsolete. Delete? */
    {
      int lex_errors;
      int lex_max_errors;
    } regex;
  struct idiom    /* This component is obsolete. Delete? */
    {
      int add_space;
    } idiom;
  struct alphabet
    {
      int print_pairs;
      int print_left;
      int read_left;
      int unicode;
    } alphabet;
  struct general
    {
      int sort_arcs;
      int verbose;
      int completion;  /*AK*/
      int stack;
      int namenets;
      int minimal;
      int quit_on_fail;
      int assert;  /* TG */
      int show_escape;
      int sq_final_arcs; /* TG */
      int sq_intern_arcs; /* TG */
      int recursive_define;
      int recursive_apply; /* LK */
      int compose_flag_as_special; /* LK */
      int need_separators;  /* ... in apply_patterns()  2001-04-30 LK */
    } general;
  struct optimisation
    {
      int in_order;
    } optimisation;
  struct parsing      /* This component is obsolete. Delete? */
    {
      int maxresult;
      int maxerrors;
      int maxheurs;
      int maxspace;
    } parsing;
  struct io
    {
      int print_sigma_also;
      int more_space;
      int obey_flags;
      int mark_version;
      int retokenize;
      int show_flags;         /* LK 2002-03-25 */
      int max_state_visits;   /* LK 2001-11-30 */
      int count_patterns;     /* LK 2002-04-16 */
      int delete_patterns;    /* LK 2002-04-15 */
      int extract_patterns;   /* LK 2002-04-11 */
      int mark_patterns;      /* LK 2002-04-11 */
      int license_type;       /* LK 2002-05-16 */
      int character_encoding; /* LK 2002-10-03 */
      int use_memory_map;     /* CP 2002-11-26 */
    } io;
  struct parameters  /* This component is obsolete. Delete?  */
    {
      int interactive;
    } parameters;
  struct lexicon     /* This component is obsolete. Delete? */
    {
      int compress_words;
    } lexicon;
};

int FstCalcExport command_parse(void *);

void FstCalcExport yacc_restart(void);
void FstCalcExport execute_commands(FST_CNTXT *);
void FstCalcExport execute_string(char *, FST_CNTXT *);
void FstCalcExport iy_source(ifile *, FST_CNTXT * );

extern char *interface_text;
extern int interface_leng;
extern int FstCalcExport user_license_type;

#endif /* C_FSM_INTERFACE */
