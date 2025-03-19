/* $Id: lookup.c,v 1.4 2009/03/18 16:35:28 roux Exp $ */

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
   filename   : lookup.c
   Date       : 10/01/1999
   Purpose    : LOOK-UP OF WORDS IN DICTIONARIES encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/

/*************************************************************
 *
 *  FILE  "lookup.c"
 *
 *      LOOK-UP OF WORDS IN DICTIONARIES
 *
 *  Andre Kempe, RANK XEROX, Grenoble, France
 *  30/01/96 - ...
 *  Tamas Gaal, RANK XEROX, Grenoble, France
 *  04-05/96
 *
 *************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "fst/types.h"
#include "fst/struct.h"
#include "fst/modify.h"
#include "fst/bin-in.h"
#include "fst/look.h"
#include "fst/label.h"
#include "fst/twol-net.h"
#include "fst/pars-tbl.h"
#include "fst/fsm-version.h"
#include "fst/context.h"


#include "lktypes.h"
#include "ems.h"
#include "lookup.h"
#include "networks.h"
#include "readscript.h"
#include "versiontok.h"

#if defined _WINDOWS
#define strcasecmp _stricmp
#endif

/* GLOBAL Variables */
ALPHABETptr LOOKUP_CHAR_TABLE;
uint16 *LOOKUP_CHAR_VECT;

FILE *LOOKUP_INPUT_FILE;

struct _LkEvaluate EVALUATE;


/*===========================================================
  
  ===========================================================*/

int DAEMON_WITH_FFLUSH = FALSE;

void evaluate_apply_functions ()
{
  float x;  long n1, n2;  char m;

  if (! LOOKUP_FLAGS.evaluate_check)
    return;       /* evaluation not required */

  switch (EVALUATE.phase)
    {
    case EVALU_end:
      return;    /* evaluation already finished */

    case EVALU_begin:
      EVALUATE.phase = EVALU_without_check;
      EVALUATE.num_mapped = 0;
      EVALUATE.timeA = clock ();
      LOOKUP_FLAGS.apply_with_check = FALSE;
      break;

    case EVALU_without_check:
      if (++EVALUATE.num_mapped <= LOOKUP_FLAGS.evaluate_num)
	break;

      EVALUATE.phase = EVALU_with_check;
      EVALUATE.num_mapped = 0;
      EVALUATE.timeA = clock () - EVALUATE.timeA;
      EVALUATE.timeB = clock ();
      LOOKUP_FLAGS.apply_with_check = TRUE;
      break;

    case EVALU_with_check:
      if (++EVALUATE.num_mapped <= LOOKUP_FLAGS.evaluate_num)
	break;

      EVALUATE.phase = EVALU_end;
      EVALUATE.timeB = clock () - EVALUATE.timeB;

      if (EVALUATE.timeB < EVALUATE.timeA)
	{                      /* function with check is better */
	  LOOKUP_FLAGS.apply_with_check = TRUE;
	  m = 'B';
	  n1 = EVALUATE.timeA;
	  n2 = EVALUATE.timeB;
	}
      else
	{                     /* function with check is not better */
	  LOOKUP_FLAGS.apply_with_check = FALSE;
	  m = 'A';
	  n1 = EVALUATE.timeB;
	  n2 = EVALUATE.timeA;
	}

      x = n1;  x /= n2;

      if (LOOKUP_FLAGS.show_comments)
	work_message ("Two methods of lookup on a net vector \
have been evaluated,\n\
on %li words each:\n\
(A) simple lookup\n\
(B) lookup with checking intermediate results immediately\n\
     on the input side of the following net\n\
Method (%c) was %.3f (i.e. %li/%li) times faster\n\
and therefore selected.\n\n",
 LOOKUP_FLAGS.evaluate_num, m, x, n1, n2);

      break;
    }
}

int LOOKUP_ALGORITHM = 0;

int lookup_word(char* input,void* parseur) {
    int parse_error;

    if (LOOKUP_FLAGS.evaluate_check)
        EVALUATE.phase = EVALU_begin;  /* init the evaluation */

    strcpy (LOOKUP_INPUT_WORD, input);
    parse_error=convert_string_to_id_seq (input, LOOKUP_ID_SEQUENCE);
    if (! parse_error) {            
        switch (LOOKUP_ALGORITHM) {                
        case 0:
            lookup_input_and_write_result_andre (parseur);
            break;
        case 1:
            lookup_input_and_write_result_tamas(parseur);
            break;
        case 2:
            lookup_input_and_write_result_word_to_number ();
            break;
        case 3: 
            lookup_input_and_write_result_compose_apply ();
            break;
		case 4:
			look_input_and_write_result_claude (parseur);
			break;
        default:
            return -1;
        }	         
    }
    return -1;
}

void lookup_input (FILE *out_stream,void* parseur) {
    
    char *line;
    int len = LOOKUP_FLAGS.len_inp_line;
    FILE* in_stream = LOOKUP_INPUT_FILE;
    int parse_error=0;    
    
    line = (char *) mem_alloc (LOOKUP_FLAGS.len_inp_line + 1);
    
    if (LOOKUP_FLAGS.evaluate_check)
        EVALUATE.phase = EVALU_begin;  /* init the evaluation */
    
    
    while (universal_read_line(&line, len, in_stream) == NO_ERROR) {    
        if (line_is_empty (line))
            continue;   /* ignore empty lines */
        
        if (line_contains_tags (line))   /* lookup already made */
            write_tagged_line (line);
        else  {                  /* lookup has to be made */        
            if (LOOKUP_FLAGS.evaluate_check)
                evaluate_apply_functions ();  /* make the evaluation */
            
            strcpy (LOOKUP_INPUT_WORD, line);
            parse_error=convert_string_to_id_seq (line, LOOKUP_ID_SEQUENCE);
            /*                                        */
            /* There are 4 algorithms - AK, TG, LK and word_to_number - */
            if (! parse_error) {            
                switch (LOOKUP_ALGORITHM) {                
                case 0:
                    lookup_input_and_write_result_andre (parseur);
                    break;
                case 1:
                    lookup_input_and_write_result_tamas(parseur);
                    break;
                case 2:
                    lookup_input_and_write_result_word_to_number ();
                    break;
                case 3: 
                    lookup_input_and_write_result_compose_apply ();
                    break;
                default:
                    fprintf(stderr, "lookup: no such algorithm: %d\n", LOOKUP_ALGORITHM);
                    exit(1);
                }	 
            }
            else {            
                NUM_WORDS_NOT_FOUND++;
                fprintf(stdout, "%s\t%s\t%s\n", LOOKUP_INPUT_WORD, LOOKUP_INPUT_WORD,
                    LOOKUP_UNKNOWN_WORD_TAG);
            }
        }
        
        fprintf (LOOKUP_OUTPUT_FILE, "\n");  /* end of line block */
        if (DAEMON_WITH_FFLUSH)
            fflush (out_stream);
    }
    
    free (line);
}

/*===========================================================
AUXILIARY FUNCTIONS FOR DEBUGGING
===========================================================*/

void LKTEST_get_label_name(LABELptr label, char *lab_name)
{
    FAT_STR fstring = LABEL_atom(label);
    
    fat_to_thin (fstring, lab_name);
}

int LKTEST_get_label(id_type id, char *lab_name)
{
    TUPLEptr tuple;  LABELptr label;
    int16 arity;
    uint16 id_count = LIM_id_count(LABEL_MAP);
    id_type upper_id, lower_id;
    char *pt;
    
    if (id>=id_count)
    { strcpy(lab_name, "WRONG_ID"); return(FALSE);}
    
    label = id_to_label(id);
    arity = LABEL_arity(label);
    
    switch(arity)
    {
    case 1:
        LKTEST_get_label_name(label, lab_name);
        break;
    case 2:
        tuple = LABEL_tuple(label);
        upper_id = TUPLE_labels(tuple)[UPPER];
        lower_id = TUPLE_labels(tuple)[LOWER];
        
        LKTEST_get_label(upper_id, lab_name);
        
        pt = &( lab_name[ strlen(lab_name) ]); *pt++=':';
        LKTEST_get_label(lower_id, pt);
        break;
    default:
        strcpy(lab_name, "WRONG_ARITY");
        return(FALSE);
    }
    
    return(TRUE);
}

void LKTEST_print_label (id_type id)
{
    char label[100];
    
    if (LKTEST_get_label (id, label))
        fprintf (stderr, "ID %hi  LABEL <%s>\n", id, label);
    else
        fprintf (stderr, "  *** ERROR ***  %s\n", label);
}

void TEST_print_id_seq (id_type *seq, int len)
{
    char label[100];  int i;
    
    for (i=0; i< len; i++)
        fprintf (stderr, "%hi ", seq [i]);
    
    fprintf (stderr, "  ");
    
    for (i=0; i< len; i++)
    {
        if (LKTEST_get_label (seq [i], label))
            fprintf (stderr, "%s ", label);
        else
            fprintf (stderr, "*ERROR* ");
    }
    
    fprintf (stderr, "\n");
}

