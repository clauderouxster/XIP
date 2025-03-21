/* $Id: ems.c,v 1.13 2015/11/20 15:07:39 roux Exp $ */

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
   filename   : ems.c
   Date       : 10/01/1999
   Purpose    : Error Handling, Memory Handling, String Handling (FST library) Encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/


/*************************************************************
 *
 *  FILE  "ems.c"
 *
 *      ERROR HANDLING,
 *      MEMORY HANDLING and
 *      STRING HANDLING for
 *      LOOK-UP OF WORDS IN DICTIONARIES
 *
 *  Andre Kempe, RANK XEROX, Grenoble, France
 *  30/01/96 - ...
 *
 *************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "fst/types.h"
#include "fst/struct.h"
#include "fst/modify.h"
#include "fst/bin-in.h"
#include "fst/look.h"
#include "fst/label.h"
#include "fst/twol-net.h"
#include "fst/pars-tbl.h"

#include "lktypes.h"
#include "ems.h"
#include "lookup.h"
#include "networks.h"
#include "readscript.h"


void *mem_alloc (long size)
{
/* Allocates the memory, or prints an error message and exits if failed.
    Fills the memory with 0. */
    
    void *mem;
    
    if (! (mem = (void *) malloc (size))) {
        printf("%s","memory allocation failed");
        exit(-1);
    }
    
    memset (mem, 0, size);
    return (mem);
}

void *mem_realloc (void *old, long size)
{
/* Allocates the memory, or prints an error message and exits if failed.
    Fills the memory with 0. */
    
    void *nouveau;
    
    if (! (nouveau = (void *) realloc (old, size))) {
        printf("%s","memory reallocation failed");
        exit(-1);
    }
    
    
    return (nouveau);
}
/*===========================================================
  ERROR HANDLING AND MESSAGES
  ===========================================================*/
void write_id_seq_to_buffer (id_type *seq);

void error_message (char *msg, ...)
{
  va_list ap;

  fprintf (stderr, "ERROR in '%s'", PROGRAM_NAME);

  if (READING_SCRIPT)
    fprintf (stderr, " at script line %li", CUR_LINE_NUM);
  if (READING_INPUT)
    fprintf (stderr, " at input line %li", CUR_LINE_NUM);

  fprintf (stderr, " :\n");

  va_start (ap, msg);
  vfprintf (stderr, msg, ap);

  fprintf (stderr, "\n");
  exit (1);
}

void work_message (char *msg, ...)
{
  va_list ap;

  va_start (ap, msg);
  vfprintf (stderr, msg, ap);

  fflush (stderr);
}

void print_success_count ()
{
  float rate;  int i, words_per_sec;  long num;
  char *s = (NUM_WORDS_TOTAL == 1 ? "" : "s");

  if (LOOKUP_SECONDS < 1)
    words_per_sec = NUM_WORDS_TOTAL;
  else
    words_per_sec = NUM_WORDS_TOTAL / LOOKUP_SECONDS;

  work_message ("\n%s STATISTICS (success with different strategies):\n",
		PROGRAM_NAME);

  for (i=0; i< NVV_len (STRATEGIES); i++)
    {
      num = SUCCESS_COUNT [i];
      rate = num * 100;  rate /= NUM_WORDS_TOTAL;
      work_message ("strategy %d:\t%li times \t(%.2f %%)\n", i, num, rate);
    }

/* Bug in MSVC++ 
  num = NUM_WORDS_NOT_FOUND;
*/
  rate = NUM_WORDS_NOT_FOUND * 100; 
  rate /= NUM_WORDS_TOTAL;
  work_message ("not found:\t%li times \t(%.2f %%)\n\n", NUM_WORDS_NOT_FOUND, rate);

  work_message ("corpus size:\t%li word%s\n", NUM_WORDS_TOTAL, s);
  work_message ("execution time:\t%li sec\n", LOOKUP_SECONDS);
  work_message ("speed:\t\t%d word%s/sec\n", words_per_sec, s);
}

/*===========================================================
  SET LOOKUP FLAGS
  ===========================================================*/

static void take_separator_from_flags
  (char *flag_str, char flag, char *deft, char *sep)
{
  /* Copies a separator contained in flag_str and delimited by
     flag....flag, to sep[].
     Overwrites the separator in flag_str with blanks.
     If 'flag' is not in 'flag_str', deft[] will be copied to sep[]. */

  char *pt1, *pt2;

  if ((pt1 = strchr (flag_str, flag)))  /* 1st occurence found */
    {
      if (! (pt2 = strchr (pt1 + 1, flag)))  /* 2nd occurence missed */
	error_message
	  ("in LOOKUP_FLAGS: wrong specification of flag '%c'", flag);
      else
	pt1++;  /* exclude the flag from the separator */
    }
  else    /* flag not found at all */
    {
      pt1 = deft;
      pt2 = deft + strlen (deft);
    }

  if (pt2 - pt1 > L_SEPARATOR - 1)  /* separator too long */
    error_message ("flag '%c' specifies a too long separator", flag);

  memmove (sep, pt1, (pt2 - pt1) * sizeof (char));
  sep [pt2 - pt1] = '\0';

  if (pt1 != deft)  /* separator is not the default */
    memset (pt1 - 1, ' ', ((pt2 - pt1) + 2) * sizeof (char));
                                    /* overwrite with blanks */
}

void set_lookup_flags(int argn, char **argv)
{
  char flags [L_LINE], *pt;  int i;

  memset (&LOOKUP_FLAGS, 0, sizeof (LOOKUP_FLAGS));

  /* 1st, try to get the flags from argv[] */

  for (i=0, pt=NULL; i< argn-1; i++)
    if (! strcmp (argv[i], LOOKUP_CMD_LINE_FLAGS))
      {
	pt = argv[i+1];   /* flag string in command line */
	break;
      }

  if (! pt)                             /* no flags in command line */
    if (! (pt = getenv (ENV_LOOKUP_FLAGS)))  /* no flags in environment */
      pt = DEF_LOOKUP_FLAGS;            /* default flags */

  LOOKUP_FLAGS.flag_string = pt;   /* original flag string */
  strcpy (flags, pt);            /* copy of the flag string */

  /* 1st: set all flags that should be consumed */

  take_separator_from_flags (flags, FLAG_SURF_LEMMA_SEP, OUTPUT_SEPARATOR,
			     LOOKUP_FLAGS.surf_lemma_sep);

  take_separator_from_flags (flags, FLAG_LEMMA_TAG_SEP, OUTPUT_SEPARATOR,
			     LOOKUP_FLAGS.lemma_tag_sep);

  if ((pt = strchr (flags, FLAG_MULTI_CHAR_SYMBOLS)))
    {
      LOOKUP_FLAGS.multi_char_symbols = TRUE;
      LOOKUP_FLAGS.multi_char_side = pt [1];
      pt [0] = ' ';
      pt [1] = ' ';
    }

  /* 2nd: set all other flags */

  if (strchr (flags, FLAG_SHOW_COMMENTS))
    LOOKUP_FLAGS.show_comments = TRUE;

  if (strchr (flags, FLAG_MAKE_RESULT_NET))
    LOOKUP_FLAGS.make_result_net = TRUE;

  if (strchr (flags, FLAG_CHECKNET_FROM_FILE))
    {
      LOOKUP_FLAGS.apply_with_check = TRUE;
      LOOKUP_FLAGS.checknet_from_file = TRUE;
    }
  else if ((pt = strchr (flags, FLAG_APPLY_WITH_CHECK)))
    {
      LOOKUP_FLAGS.apply_with_check = TRUE;
      LOOKUP_FLAGS.checknet_from_file = FALSE;
      sscanf (pt+1, "%li", &(LOOKUP_FLAGS.check_net_min));
    }

  if ((pt = strchr (flags, FLAG_VECTORIZE_NETS)))
    {
      LOOKUP_FLAGS.vectorize_nets = TRUE;
      sscanf (pt+1, "%li", &(LOOKUP_FLAGS.vectorize_min));
    }

  if ((pt = strchr (flags, FLAG_EVALUATE_CHECK)))
    {
      LOOKUP_FLAGS.evaluate_check = TRUE;
      sscanf (pt+1, "%li", &(LOOKUP_FLAGS.evaluate_num));
    }

  if (strchr (flags, FLAG_DONT_COPY_INPUT))
    LOOKUP_FLAGS.dont_copy_input = TRUE;


  if ((pt = strchr (flags, FLAG_INPUT_LENGTH)))
    sscanf (pt+1, "%li", &(LOOKUP_FLAGS.len_inp_line));
  else
    LOOKUP_FLAGS.len_inp_line = L_LINE;
}

/*-----------------------------------------------------------*/
void alloc_symbol_table (long num_sym, long bf_len)
{
  long hlen;

  SYMBOL.num_symbols = num_sym;
  SYMBOL.symbol = (char **) mem_alloc (num_sym * sizeof (char *));
  SYMBOL.hash_list = (HASHptr) mem_alloc (num_sym * sizeof (HASHtype));

  SYMBOL.bf_len = bf_len;
  SYMBOL.buffer = (char *) mem_alloc (bf_len * sizeof (char));

  SYMBOL.hash_tab_len = hlen = num_sym * HASH_LEN_FACTOR;
/*
  SYMBOL.hash_tab = (HASHptr *) mem_alloc (num_sym * sizeof (HASHptr));
  This allocates too little. Must be hlen instead of num_sym LK. 17.2.96
*/
  SYMBOL.hash_tab = (HASHptr *) mem_alloc (hlen * sizeof (HASHptr)); 
}

void free_symbol_table ()
{
  free (SYMBOL.symbol);
  free (SYMBOL.hash_list);
  free (SYMBOL.buffer);
  free (SYMBOL.hash_tab);
}

void alloc_net_file_vector (long num)
{
  NET_FILES.num = num;
  NET_FILES.file = (FILE **) mem_alloc (num * sizeof (FILE *));
}

void free_net_file_vector ()
{
  free (NET_FILES.file);
}

void alloc_success_count()
{
	SUCCESS_COUNT = (long *)mem_alloc(NVV_len(STRATEGIES) * sizeof (long));
	NUM_WORDS_TOTAL = 0;
	NUM_WORDS_NOT_FOUND = 0;
}

void set_success_count(long* sc) {
	SUCCESS_COUNT = sc;
}

long* get_success_count() {
	return SUCCESS_COUNT;
}

void free_success_count()
{
	if (SUCCESS_COUNT != NULL)
		free(SUCCESS_COUNT);
	SUCCESS_COUNT = NULL;
}

void alloc_in_out_buffers()
{
	if (LOOKUP_INPUT_WORD == NULL)
		LOOKUP_INPUT_WORD = (char *)mem_alloc(LOOKUP_FLAGS.len_inp_line + 1);

	if (LOOKUP_OUTPUT_BUFFER == NULL)
		LOOKUP_OUTPUT_BUFFER = (char *)mem_alloc(LOOKUP_FLAGS.len_inp_line + 1);

	ID_SEQUENCE_LENGTH = (LOOKUP_FLAGS.len_inp_line + 1) * (NV_len(LEXNETS) + 1);

	if (LOOKUP_ID_SEQUENCE == NULL)
		LOOKUP_ID_SEQUENCE =
		(id_type *)mem_alloc(ID_SEQUENCE_LENGTH * sizeof (id_type));
}

void free_in_out_buffers()
{
	if (LOOKUP_INPUT_WORD != NULL)
		free(LOOKUP_INPUT_WORD);
	if (LOOKUP_OUTPUT_BUFFER != NULL)
		free(LOOKUP_OUTPUT_BUFFER);
	if (LOOKUP_ID_SEQUENCE != NULL)
		free(LOOKUP_ID_SEQUENCE);

	LOOKUP_ID_SEQUENCE = NULL;
	LOOKUP_OUTPUT_BUFFER = NULL;
	LOOKUP_INPUT_WORD = NULL;
}

/*===========================================================
  STRING HANDLING
  ===========================================================*/

char* lookup_read_line (char *line, int len, FILE* file)
{
  CUR_LINE_NUM++;

  if (! fgets (line, len-1, file))
    return (NULL);

  len = strlen (line);

  if (line [len - 1] == '\n')
    line [len - 1] = '\0';

  return (line);
}

int tokenize_line (char *line, char **token, char *sep)
{
  /* Cuts a line into tokens where-ever any 'sep' character is found.
     Returns the number of tokens. */

  char *pt;  int toknum;

  for (pt = line, toknum = 0; ; )
    {
      while ((*pt) && (strchr (sep, *pt)))  *pt++='\0';
      if (! *pt)  return (toknum);

      token [toknum++] = pt;
      while ((*pt) && (! strchr (sep, *pt)))  pt++;
    }

  return toknum;
}

/*===========================================================
  CHECK IF LINE IS EMPTY
  ===========================================================*/

int line_is_empty (char *line)
{
  for ( ; *line; line++)
    if (! strchr (INPUT_SEPARATORS, *line))
      return (FALSE);   /* something that is not a separator */

  return (TRUE);
}

/*===========================================================
  HANDLE INPUT LINES THAT ALREADY HAVE TAGS
  ===========================================================*/

int line_contains_tags (char *line)
{
  for ( ; *line; line++)
    if (strchr (INPUT_SEPARATORS, *line))
      return (TRUE);

  return (FALSE);
}

void write_tagged_line (char *line)
{
  /* Writed a input line that is already tagged to the output. Changes
     the token separators from INPUT_SEPARATORS[] to OUTPUT_SEPARATOR. */

  char *token [MAX_TOKENS];  int i, num;

  strcpy (LOOKUP_OUTPUT_BUFFER, line);
  if (! (num = tokenize_line (LOOKUP_OUTPUT_BUFFER, token, INPUT_SEPARATORS)))
    return;  /* ignore empty input lines */

  if (num % 2 == 0)   /* even number of tokens */
    error_message ("pre-tagged input line with %d tokens.\n\
the number of tokens cannot be even here !!!\n\
LINE:\t%s", num, line);

  for (i=1; i< num; i+=2)
    {
      fprintf (LOOKUP_OUTPUT_FILE, "%s%s%s%s%s\n",
	       token [0], OUTPUT_SEPARATOR,
	       token [i], OUTPUT_SEPARATOR, token [i+1]);
    }
}

/*===========================================================
  SELECT RESULT FUNCTION
  ===========================================================*/

void select_result_function ()
{
  if (LOOKUP_FLAGS.make_result_net)
    LOOKUP_print_result_fn = add_id_seq_to_result;
  else
    LOOKUP_print_result_fn = write_id_seq_to_buffer;
}

