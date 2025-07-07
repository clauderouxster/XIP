/* $Id: look.c,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1994 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "fst/error.h"
#include "fst/struct.h"
#include "fst/cmp-net.h"

#include "fst/prompt.h"
#include "fst/pretty-p.h"
#include "fst/pars-tbl.h"
#include "fst/look.h"
#include "fst/modify.h"
#include "fst/fsm_io.h"
#include "fst/flags.h"
#include "fst/utf8_fatstr_util.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

/******************************************************
 **
 **  LOOK.C
 **
 **  Lauri Karttunen, RXRC, December 1993
 **
 **  LOOKUP and LOOKDOWN words in a transducer
 **
 ******************************************************/

static int OUTPUT_MAX_LEN;
static NETptr OUTPUT_NET = NULL;
/*static ALPHABETptr SIGMA;*/
ALPHABETptr SIGMA;
static void (*RESULT_FN)(id_type *result);
ALPHABETptr FLAG_REGISTER;

id_type OUTPUT_SEQ[WORD_STRING_SIZE + 1];
int OUTPUT_SIDE;
int RESULT_COUNT;
int TEST_P = FALSE;  /* Obscure variable. Declared extern in tabular.c */
int OBEY_FLAGS_P;

static id_type *OUTPUT_LIM = OUTPUT_SEQ + WORD_STRING_SIZE;
static id_type label_id/*, path_id*/;

void print_result(id_type *tail)
{
  id_type label, *head;

  for (head = OUTPUT_SEQ; head != tail; head++)
    {
      label = *head;
      
      if (OUTPUT_SIDE == UPPER)
	label =  upper_id(label);
      else if (OUTPUT_SIDE == LOWER)
	label = lower_id(label);
      
      if (!(label == EPSILON || label == ALTCHAIN_SYMBOL ||
	    (flag_diacritic_p(label) && OBEY_FLAGS_P)) ||
	  OUTPUT_SIDE == BOTH_SIDES)
      {
	label_out(label, DONT_ESCAPE) ;
      }
    }
    fresh_line(72,0);
  
  RESULT_COUNT++;
}


static void collect_result(id_type *tail)
{
  id_type label, *head;
  STATEptr dest, state = NET_start_state(OUTPUT_NET);
  ARCptr arc;

  for (head = OUTPUT_SEQ; head != tail; head++)
    {
      label = *head;
      
      if (OUTPUT_SIDE == UPPER)
	label =  upper_id(label);
      else if (OUTPUT_SIDE == LOWER)
	label = lower_id(label);
      
      if (!(label == EPSILON || label == ALTCHAIN_SYMBOL))
        {
          for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
            if (ARC_label(arc) == label)
              {
                state = ARC_destination(arc);
                break;
              }

          if (!arc)
            {
              dest = make_state(NULL, OUTPUT_NET);
              arc =  make_arc(label, dest, OUTPUT_NET);
              push_arc(arc, state);
              state = dest;
            }
        }
    }
  STATE_final(state) = TRUE;
}

static void ignore_result(id_type *tail)
{
  /* Do nothing. */
}

static void update_max_prefix_len(id_type *tail)
{
  int len = tail - INPUT_SEQ;

  if (len > OUTPUT_MAX_LEN)
    OUTPUT_MAX_LEN = len;
}


int look(id_type *path, STATEptr state, id_type *result, void result_fn(id_type *), int ignore_finality)
     /*****
      * Recursively follows PATH through a net starting at STATE.
      * matching label ids on the path against the lower side or the upper
      * side of arc labels depending on MATCH_TABLE. Builds list of arcs
      * in RESULT.  The 'arc' variable is here redundant ('*result' would
      * just as well), but it speeds up the loop by nearly 20%.
      *****/
{
  ARCptr arc;
  id_type id, old_val, attr;
  
  if (result == OUTPUT_LIM)
    {
      fsm_fprintf(stderr, "\n*** Recursion exceeds limit (%d). Aborting...\n",
	      WORD_STRING_SIZE);
      return(1);
    }

  if (*path == ID_EOS)		/* when the path is consumed */
    {
      /* check if current state is a hit */
      
      if (STATE_final(state) || ignore_finality)
	{
          result_fn(result);
          update_max_prefix_len(path);
        }
      /* path exhausted, only look for epsilon arcs and flag diacritics */
      
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
	{
	  id = MATCH_TABLE[ARC_label(arc)]; 
	  if ( id == EPSILON || id == ALTCHAIN_SYMBOL)
	    {
	      *result = ARC_label(arc);
	      if (look(path, ARC_destination(arc), result+1, result_fn,
		       ignore_finality))
		return(1);
	    }
	  else if ((attr = flag_diacritic_attrib(id)))
	    {
	      /* Save the value currently in the register, if any,
		 and try to unify it with the diacritic. If the
		 unification succeeds, we need to restore the
		 old value on exit from the recursive call.
		 */

	      old_val = flag_value(FLAG_REGISTER, attr);

	      if (!OBEY_FLAGS_P ||
		  flag_action(FLAG_REGISTER, flag_diacritic(id)))
		{
		  *result = ARC_label(arc);
		  if (look(path, ARC_destination(arc), result+1, result_fn,
			   ignore_finality))
		    {
		      flag_value(FLAG_REGISTER, attr) = old_val;
		      return(1);
		    }
		  else
		    flag_value(FLAG_REGISTER, attr) = old_val;
		}
	    }
	}
    }
  else
    {
      if (STATE_final(state) || ignore_finality)
        update_max_prefix_len(path);
  
        /* look for epsilon arcs and arcs that match the next symbol */
    
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
        {
          label_id = MATCH_TABLE[ARC_label(arc)];
          if (label_id == EPSILON || label_id == ALTCHAIN_SYMBOL)
            {
              *result = ARC_label(arc);
              if(look(path, ARC_destination(arc), result + 1, result_fn,
		      ignore_finality))
                return(1);
            }
          else if (*path == label_id)
	    {
              *result = ARC_label(arc);
              if(look(path + 1, ARC_destination(arc), result + 1, result_fn,
		      ignore_finality))
                return(1);
	     }
	  else if ((attr = flag_diacritic_attrib(label_id)))
	    {
	      /* Save the value currently in the register, if any,
		 and try to unify it with the diacritic. If the
		 unification succeeds, we need to restore the
		 old value on exit from the recursive call.
		 */

	      old_val = flag_value(FLAG_REGISTER, attr);

	      if (!OBEY_FLAGS_P ||
		  flag_action(FLAG_REGISTER, flag_diacritic(label_id)))
		{
		  *result = ARC_label(arc);
		  if (look(path, ARC_destination(arc), result+1, result_fn,
			   ignore_finality))
		    {
		      flag_value(FLAG_REGISTER, attr) = old_val;
		      return(1);
		    }
		  else
		    flag_value(FLAG_REGISTER, attr) = old_val;
		}
	    }
           else
             if (label_id == OTHER && !binary_sigma_member(SIGMA, *path))
               {
                  /* If the arc label is an fstpair with OTHER only on one
                     side, we take it to transduce the unknown symbol.
                     Otherwise we keep the unknown symbol, i.e. transduce
                     it back to itself.
                     */

                  if (ARC_label(arc) == OTHER)
                    *result = *path;
                  else
                    *result = ARC_label(arc);

                  if (look(path + 1, ARC_destination(arc), result + 1,
                           result_fn, ignore_finality))
                    return(1);
                }   
        }
    }
  return(0);
}

#ifndef LEXC_APPLICATION
static int compact_look(id_type *path, unsigned char *loc, id_type *result)
{
    id_type attr, old_value;
    
    if (result == OUTPUT_LIM)
    {
      fsm_fprintf(stderr, "\n*** Recursion exceeds limit (%d). Aborting...\n",
	      WORD_STRING_SIZE);
      return(1);
    }
  
  if (SKIP_DATA_P)		/* Skip client data, if any. */
    loc = SKIP_DATA(loc);
  
  if (DEST_FINAL_P)
    update_max_prefix_len(path);

  if (*path == ID_EOS && DEST_FINAL_P) /* Print result when appropriate. */
  {
      RESULT_FN(result);
  }
  
  loc = decode_var_label(loc);	/* Get the first label. */
  
  if (LABEL_ID >= LABEL_LIMIT)
    return(0);			/* Empty state. No arcs. */
  
  do {
    LABEL_ID = RECODE_KEY[LABEL_ID];
    
    if (MATCH_TABLE[LABEL_ID] == EPSILON ||
	MATCH_TABLE[LABEL_ID] == ALTCHAIN_SYMBOL)
      {
	*result = LABEL_ID;	/* Matching epsilon */
	loc = DECODE_ADDR(loc);
	if (LAST_ARC_P)
	  return(compact_look(path, DEST_ADDR, result+1));
	else if (compact_look(path, DEST_ADDR, result+1))
	  return(1);
      }
    else if (*path == MATCH_TABLE[LABEL_ID])
      {
	*result = LABEL_ID;	/* Matching a lexical symbol. */
	loc = DECODE_ADDR(loc);
	if (LAST_ARC_P)
	  return(compact_look(path+1, DEST_ADDR, result+1));
	else if (compact_look(path+1, DEST_ADDR, result+1))
	  return(1);
      }
    else if ( (attr = flag_diacritic_attrib(LABEL_ID)) )
      {
          /* Save the value currently in the register, if any,
             and try to unify it with the diacritic. If the
             unification succeeds, we need to restore the
             old value on exit from the recursive call.
          */

          old_value= flag_value(FLAG_REGISTER, attr);

          if (!OBEY_FLAGS_P ||
              flag_action(FLAG_REGISTER, flag_diacritic(LABEL_ID)))
          {
              *result = LABEL_ID;
              loc = DECODE_ADDR(loc);
        
              if (LAST_ARC_P)
              {
                  int returned_code= compact_look(path, DEST_ADDR, result+1);
                  flag_value(FLAG_REGISTER, attr) = old_value;
                  return(returned_code);
              }
              else if (compact_look(path, DEST_ADDR, result+1))
              {
                  flag_value(FLAG_REGISTER, attr) = old_value;
                  return(1);
              }
              else
                  flag_value(FLAG_REGISTER, attr) = old_value;
          }
          else if (LAST_ARC_P)
              return(0);		/* No more arcs. */              
      } /* flag diacritics */
    else if (MATCH_TABLE[LABEL_ID] == OTHER && *path != 0 &&
	     *path != ID_EOS &&
             !binary_sigma_member(SIGMA, *path))
      {
          /* If the arc label is an fstpair with OTHER only on one
             side, we take it to transduce the unknown symbol.
             Otherwise we keep the unknown symbol, i.e. transduce
             it back to itself.
             */
        if (LABEL_ID == OTHER)
          *result = *path;
        else
          *result = LABEL_ID;

        loc = DECODE_ADDR(loc);
	if (LAST_ARC_P)
	  return(compact_look(path+1, DEST_ADDR, result+1));
	else if (compact_look(path+1, DEST_ADDR, result+1))
	  return(1);
      }   
    else if (LAST_ARC_P)
      return(0);		/* No more arcs. */
    else
      /* Skip over the bytes encoding the address of the arc */
      if (SKIP_BYTES == 0)
	while(more_to_come_p(*loc++)) ;
      else
	loc = loc + SKIP_BYTES;
    
    loc = decode_var_label(loc); /* Get the next label. */
    
  } while (TRUE);
}
#endif

#ifdef MAC_OS
static int fast_look(id_type *path, CAptr ca, id_type *result, int final_p)
{
  
  if (result == OUTPUT_LIM)
    {
      fsm_fprintf(stderr, "\n*** Recursion exceeds limit (%d). Aborting...\n",
	      WORD_STRING_SIZE);
      return(1);
    }
  
  if (final_p)
    update_max_prefix_len(path);

  if (*path == ID_EOS && final_p)
    RESULT_FN(result);	/* Print result. */
  
  LABEL_ID = CA_label(ca) >> 2;
  
  if (LABEL_ID >= LABEL_LIMIT)
    return(0);			/* State has no arcs. */
  
  do {
    
    if (MATCH_TABLE[LABEL_ID] == EPSILON ||
	MATCH_TABLE[LABEL_ID] == ALTCHAIN_SYMBOL)
      {
	*result = LABEL_ID;     
	if (fast_look(path, CA_next(ca), result+1, CA_label(ca) & 1))
	  return(1);
      }
    else if (*path == MATCH_TABLE[LABEL_ID])
      {
	*result = LABEL_ID;
	if (fast_look(path+1, CA_next(ca), result+1, CA_label(ca) & 1))
	  return(1);
      }
    else if (MATCH_TABLE[LABEL_ID] == OTHER && *path != 0 &&
	     *path != ID_EOS &&
             !binary_sigma_member(SIGMA, *path))
      {
        /* If the arc label is an fstpair with OTHER only on one
           side, we take it to transduce the unknown symbol.
           Otherwise we keep the unknown symbol, i.e. transduce
           it back to itself.
           */
        if (LABEL_ID == OTHER)
          *result = *path;
        else
          *result = LABEL_ID;

	if (fast_look(path+1, CA_next(ca), result+1, CA_label(ca) & 1))
	  return(1);
      }   

    if (CA_label(ca) & 2)
      return(0);
    
    ca = ca + 1;
    
    LABEL_ID = CA_label(ca) >> 2;
    
  } while(TRUE);
}
#else
inline
  static unsigned short *get_addr(unsigned short *loc)
{
  /* LOC is guaranteed to be an even number. If it is a multiple
     of 4, just coerce it to a pointer of the desired type, otherwise
     compose a long integer from two short integers and coerce that
     to a pointer of the right type.
     */
  if ((unsigned long) loc % 4)
    return((unsigned short *)((((unsigned long)  *loc) << 16) | *(loc+1)));
  else
    return((unsigned short *) *(unsigned long *) loc);
}

static int fast_look(id_type *path, unsigned short *label, id_type *result,
		     int final_p)
{
  if (result == OUTPUT_LIM)
    {
      fsm_fprintf(stderr, "\n*** Recursion exceeds limit (%d). Aborting...\n",
	      WORD_STRING_SIZE);
      return(1);
    }
  
  if (*path == ID_EOS && final_p)
    RESULT_FN(result);
  
  LABEL_ID = *label >> 2;
  
  if (LABEL_ID >= LABEL_LIMIT)
    return(0);
  
  do {
    
    if (MATCH_TABLE[LABEL_ID] == EPSILON ||
	MATCH_TABLE[LABEL_ID] == ALTCHAIN_SYMBOL)
      {
	*result = LABEL_ID;
	if (fast_look(path, get_addr(label + 1), result+1, *label & 1))
	  return(1);
      }
    else if (*path == MATCH_TABLE[LABEL_ID])
      {
	*result = LABEL_ID;
	if (fast_look(path+1, get_addr(label + 1), result+1, *label & 1))
	  return(1);
      }
    else if (MATCH_TABLE[LABEL_ID] == OTHER && *path != 0 &&
	     *path != ID_EOS &&
             !binary_sigma_member(SIGMA, *path))
      {
        /* If the arc label is an fstpair with OTHER only on one
           side, we take it to transduce the unknown symbol.
           Otherwise we keep the unknown symbol, i.e. transduce
           it back to itself.
           */
        if (LABEL_ID == OTHER)
          *result = *path;
        else
          *result = LABEL_ID;

	if (fast_look(path+1, get_addr(label + 1), result+1, *label & 1))
	  return(1);
      }   

    if (*label & 2)
      return(0);
    
    label = label + 3;
    LABEL_ID = *label >> 2;
    
  } while(TRUE);
  
}
#endif

static int fastest_look(id_type *path, MAptr ma, id_type *result, int final_p)
{
  if (result == OUTPUT_LIM)
    {
      fsm_fprintf(stderr, "\n*** Recursion exceeds limit (%d). Aborting...\n",
	      WORD_STRING_SIZE);
      return(1);
    }
  
  if (final_p)
    update_max_prefix_len(path);

  if (*path == ID_EOS && final_p)
    RESULT_FN(result);
  
  LABEL_ID = MA_label(ma);
  
  if (LABEL_ID >= LABEL_LIMIT)
    return(0);
  
  do {
    
    if (MATCH_TABLE[LABEL_ID] == EPSILON ||
	MATCH_TABLE[LABEL_ID] == ALTCHAIN_SYMBOL)
      {
	*result = LABEL_ID;     
	if (fastest_look(path, MA_next(ma), result+1, MA_dest_final(ma)))
	  return(1);
      }
    else if (*path == MATCH_TABLE[LABEL_ID])
      {
	*result = LABEL_ID;
	if (fastest_look(path+1, MA_next(ma), result+1, MA_dest_final(ma)))
	  return(1);
      }
    else if (MATCH_TABLE[LABEL_ID] == OTHER && *path != 0 &&
	     *path != ID_EOS &&
             !binary_sigma_member(SIGMA, *path))
      {
        /* If the arc label is an fstpair with OTHER only on one
           side, we take it to transduce the unknown symbol.
           Otherwise we keep the unknown symbol, i.e. transduce
           it back to itself.
           */
        if (LABEL_ID == OTHER)
          *result = *path;
        else
          *result = LABEL_ID;

	if (fastest_look(path+1, MA_next(ma), result+1, MA_dest_final(ma)))
	  return(1);
      }   
    
    if (MA_last_arc(ma))
      return(0);
    
    ma = ma + 1;
    LABEL_ID = MA_label(ma);
    
  } while(TRUE);
}

#ifndef LEXC_APPLICATION
static void init_compact_lookup(NETptr net, void result_fn(id_type *))
{
  ARC_BLOCK = NET_arc_block(net);
  set_decoding_fns(*ARC_BLOCK);
  START_LOC = NET_start_loc(net);
  RECODE_KEY = ALPH_items(NET_recode_key(net));
  LABEL_LIMIT = ALPH_len(NET_recode_key(net));
  SKIP_DATA_P = NET_names_matter(net);
  RESULT_FN = result_fn;
}

/* static int do_compact_look(NETptr net, void result_fn(int16 *)) */
int do_compact_look(NETptr net, void result_fn(id_type *))
{
  DEST_FINAL_P = NET_start_state_final(net);
  
  init_compact_lookup(net, result_fn);
  
  if (*ARC_BLOCK == 100)
    return(fastest_look(INPUT_SEQ, (MAptr) START_LOC, OUTPUT_SEQ,
			DEST_FINAL_P));
#ifdef MAC_OS\
  /* Presupposes that pointers don't need alignment */
  else if (*ARC_BLOCK == 99)
    return(fast_look(INPUT_SEQ, (CAptr) START_LOC, OUTPUT_SEQ, DEST_FINAL_P));
#else
  /* For Unix machines that require pointer alingment. */
  else if (*ARC_BLOCK == 99)
    return(fast_look(INPUT_SEQ, (unsigned short *) START_LOC, OUTPUT_SEQ,
		     DEST_FINAL_P));
#endif
  else
    return(compact_look(INPUT_SEQ, START_LOC, OUTPUT_SEQ));
}
#endif


int apply_to_string(char *word, PARSE_TBL parse_table, NETptr net, 
		    int input_side, int output_side, int obey_flags_p)
{
  int parse_error;

  OBEY_FLAGS_P = obey_flags_p;

  if (input_side == UPPER)
    MATCH_TABLE = UPPER_MATCH;
  else
    MATCH_TABLE = LOWER_MATCH;

  OUTPUT_SIDE = output_side;

  parse_error = FST_parse_string(word, parse_table, DONT_ZERO_TO_EPS,
				  DONT_IGNORE_WHITES, DONT_ESCAPE,
                                  DO_REPORT_ERROR);
  if ( parse_error == ERROR)
     return(-1);

  RESULT_COUNT = 0;
  SIGMA = label_to_binary(NET_sigma(net));

  if (!(FLAG_REGISTER = NET_flag_register(net)))
    FLAG_REGISTER = NET_flag_register(net) = make_flag_register();

#ifndef LEXC_APPLICATION
  if (NET_compacted(net))
    do_compact_look(net, print_result);
  else
#endif
    look(INPUT_SEQ, NET_start_state(net), OUTPUT_SEQ, print_result, FALSE);
  return(RESULT_COUNT);
}


NETptr map_seq_to_net(id_type *input, NETptr net, int input_side,
                      int output_side, int minimize_p)
{
    /* PATH is a sequence of label id's terminating with ID_EOS.
       Constructs a network from the matching paths in NET. If
       OUTPUT_SIDE is BOTH_SIDES, the result is equivalent
       to the composition of the input word with the network,
       if OUTPUT_SIDE is UPPER or LOWER, the result is
       equivalent to composition and extraction. Returns
       NULL if word cannot be parsed with the present parse
       table, a null fsm, if the word is not found in the
       net. The caller is responsible for releasing the
       returned network.
       */

   SIGMA = label_to_binary(NET_sigma(net));

   OUTPUT_NET = null_fsm();
   OUTPUT_SIDE = output_side;
   
  if (!(FLAG_REGISTER = NET_flag_register(net)))
    FLAG_REGISTER = NET_flag_register(net) = make_flag_register();
    
#ifndef LEXC_APPLICATION
  if (NET_compacted(net))
    do_compact_look(net, collect_result);
  else
#endif
    look(input, NET_start_state(net), OUTPUT_SEQ, collect_result, FALSE);

   /* make new label and sigma alphabet */

   update_label_alphabet (OUTPUT_NET);
   free_alph (NET_sigma(OUTPUT_NET));
   NET_sigma(OUTPUT_NET) = label_sigma(OUTPUT_NET);

   NET_minimized(OUTPUT_NET) = FALSE;
   if (minimize_p == DO_MINIMIZE)
     return(min_fsm(OUTPUT_NET, DONT_COPY));
   else
     return(OUTPUT_NET);
}

NETptr map_string_to_net(char *word, NETptr net, int input_side,
                         int output_side, int minimize_p)
{
    /* Parses WORD using the appropriate parse table for NET.
       Matches the resulting PATH against the network. If
       OUTPUT_SIDE is BOTH_SIDES, the result is equivalent
       to the composition of the input word with the network,
       if OUTPUT_SIDE is UPPER or LOWER, the result is
       equivalent to composition and extraction. Returns
       NULL if word cannot be parsed with the present parse
       table, a null fsm, if the word is not found in the
       net. The caller is responsible for releasing the
       returned network.
       */

    PARSE_TBL parse_table;
    int parse_error;

    if (input_side == UPPER)
      parse_table = NET_upper_parse_table(net);
    else
      parse_table = NET_lower_parse_table(net);
      
    if (!parse_table)
      parse_table = make_parse_table(net, NULL, input_side);

  if (input_side == UPPER)
    MATCH_TABLE = UPPER_MATCH;
  else
    MATCH_TABLE = LOWER_MATCH;

  if ((parse_error = FST_parse_string(word, parse_table, DONT_ZERO_TO_EPS,
				  DONT_IGNORE_WHITES, DONT_ESCAPE,
                                  DONT_REPORT_ERROR)))
      return(NULL);

  return(map_seq_to_net(INPUT_SEQ, net, input_side, output_side,
                        minimize_p));
}

int longest_prefix_seq(id_type *input, NETptr net, int input_side)

{
    /* INPUT is a sequence of label id's terminating with ID_EOS.
       Returns the length of the longest subpath of PATH in NET
       that leads to a final state.
       */

   SIGMA = label_to_binary(NET_sigma(net));
   OUTPUT_MAX_LEN = -1 ;

#ifndef LEXC_APPLICATION
  if (NET_compacted(net))
    do_compact_look(net, ignore_result);
  else
#endif
    look(input, NET_start_state(net), OUTPUT_SEQ, ignore_result, FALSE);
   return(OUTPUT_MAX_LEN);
}

int longest_prefix_string(char *word, NETptr net, int input_side)
{
    /* Parses WORD using the appropriate parse table for NET.
       Finds the length of the longest substring of WORD that
       is accepted by the NET. Returns
       -1 when no substring matches any word in the net
        0 when no nonnull substring matches any word in the
          net.
        n the length of the longest 
       */

    PARSE_TBL parse_table;
    int parse_error;

    if (input_side == UPPER)
      parse_table = NET_upper_parse_table(net);
    else
      parse_table = NET_lower_parse_table(net);
      
    if (!parse_table)
      parse_table = make_parse_table(net, NULL, input_side);

  if (input_side == UPPER)
    MATCH_TABLE = UPPER_MATCH;
  else
    MATCH_TABLE = LOWER_MATCH;

  parse_error = FST_parse_string(word, parse_table, DONT_ZERO_TO_EPS,
                             DONT_IGNORE_WHITES, DONT_ESCAPE,
                             DONT_REPORT_ERROR);

  return(longest_prefix_seq(INPUT_SEQ, net, input_side));
}
/*------------------------------------------------------------*/

int accepts_path
  (id_type *path, STATEptr state, int input_side, ALPHABETptr sigma)
{
     /*****
      * Recursively follows path through a net, matching 
      * label ids on the path against the lower side or the upper
      * side of arc labels depending on MATCH_TABLE.  Returns
      * 1 if the path is accepted, 0 if not.
      *****/
    /* this code has been lifted from c-fsm's look.c,
     * from look().
     */
  id_type *match_table;
  id_type label_id;
  ARCptr arc;
 
  if (input_side == UPPER)
    match_table = UPPER_MATCH;
  else
    match_table = LOWER_MATCH;
 
  if (*path == ID_EOS)  {             /* when the path is consumed */
      /* check if current state is a hit */
 
      if (STATE_final(state)) {
          return TRUE;
        }
      /* path exhausted, only look for epsilon arcs. */
 
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
        if (match_table[ARC_label(arc)] == EPSILON ||
            match_table[ARC_label(arc)] == ALTCHAIN_SYMBOL) {
            if (accepts_path(path, ARC_destination(arc), input_side, sigma)) {
              return(TRUE);
              }
          }
      }
  else {
        /* look for epsilon arcs and arcs that match the next symbol */
 
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc)) {
          label_id = match_table[ARC_label(arc)];
          if (label_id == EPSILON || label_id == ALTCHAIN_SYMBOL) {
              if(accepts_path(path, ARC_destination(arc), input_side, sigma))
                return(TRUE);
            }
          else if (*path == label_id) {
              if(accepts_path(path+1, ARC_destination(arc), input_side, sigma))
                return(TRUE);
             }
           else {
             if (label_id == OTHER && !binary_sigma_member(sigma, *path)) {
                   if (accepts_path(path+1, ARC_destination(arc),
				    input_side, sigma))
                    return(TRUE);
                }
            }
        }
    }
  return(FALSE);
}

int accepts_string (char *strString, NETptr pNet)
{
    /* is the string accepted by the network? */
    /* this code has been lifted from c-fsm's look.c,
     * mostly from apply_to_string().
     */
  PARSE_TBL parse_table = NET_upper_parse_table(pNet);
  int parse_error;
  if (parse_table == NULL) {
    parse_table = make_parse_table(pNet, NULL, UPPER);
  }
  parse_error = FST_parse_string(strString, parse_table, DONT_ZERO_TO_EPS,
			     DONT_IGNORE_WHITES, DONT_ESCAPE,
			     DONT_REPORT_ERROR);
  if (parse_error != NO_ERROR) {
    return(FALSE);
  }
  label_to_binary(NET_sigma(pNet));
  if (accepts_path(INPUT_SEQ, NET_start_state(pNet), UPPER,
		   NET_sigma(pNet)) == 1) {
    return TRUE;
  }
  return FALSE;
}

/****************************************
 *
 *  LOOKUP / LOOKDOWN IN A SET (VECTOR) OF TRANSDUCERS
 *
 *   (Since in the future, other algorithms that serve the same purpose
 *    might be implemented, I will prefix this one with 'LKP01_'.)
 *
 *  Andre Kempe, 21/03/96
 *
 **************************/

/* If a set of transducers cannot be composed because of the size of
   the final or intermediate networks, then the following functions
   can be used to apply a sequence of label IDs to this set of transducers
   which is equivalent to applying it to the non-computable composition.
   */

void (*LKP01_result_fn)(id_type *res);
NETptr LKP01_result_net;
id_type *LKP01_INPUT_MATCH;
id_type *LKP01_OUTPUT_MATCH;

#define LKP01_UNKNOWN_SYMBOL  '?'


int LKP01_length_of_sequence (id_type *seq)
{
  /* Returns the forward length of an ID sequence. */

  id_type *end;

  for (end = seq; *end != ID_EOS; end++) ;
  return (end - seq);
}

void LKP01_add_id_seq_to_net (id_type *seq, NETptr net)
{
  /* Adds a new path corresponding to the ID sequence to the net.
     Does no miminization or determinization !!! */

  STATEptr prev_state, state = NET_start_state (net);
  ARCptr arc;

  for ( ; *seq != ID_EOS; seq++)
    {
      prev_state = state;
      state = make_state (NULL, net);

      arc = make_arc (*seq, state, net);

      if (prev_state == NET_start_state (net))
	{
	  push_arc (arc, prev_state);
	  STATE_deterministic (prev_state) = FALSE;
	}
      else
	STATE_arc_set (prev_state) = arc;
    }

  STATE_final (state) = TRUE;
}

void LKP01_add_id_seq_to_result (id_type *seq)
{
  LKP01_add_id_seq_to_net (seq, LKP01_result_net);
}

void LKP01_apply_seq_to_nv
  (id_type *inseq, id_type *outseq, NVptr nv, int net_idx, STATEptr state)
{
  /* Applies the input sequence recursively to all arcs of one network,
     and to all networks of the net vector.
     It is the simple version of this function.
     The sigmas of all networks are supposed to be of type
     BINARY_VECTOR !!!*/

  ARCptr arc;  
  id_type inlab;  
  int xin, xout;

  /* case: sequence end */

  if (*inseq == ID_EOS)
    {
      if (STATE_final (state))   /* input sequence is accepted */
	{
	  *outseq = ID_EOS;   /* finish the output sequence */

	  /* If the current net is the last one in the nv, we generate
	     an ouput, i.e. we add a new path to the LKP01_result_net.
	     The output sequence begins immediately after the input sequence,
	     i.e. at inseq+1. */

	  if (net_idx == NV_len (nv) - 1)
	    LKP01_result_fn (inseq+1);

	  /* If the current net is not the last one in the nv, we have to
	     re-enter recursion with:
	     - outseq (i.e. inseq+1) as new inseq,
	     - the space after outseq (i.e. outseq+1) as new outseq,
	     - the next network and its start state */

	  else
	    LKP01_apply_seq_to_nv
	      (inseq+1, outseq+1, nv, net_idx+1,
	       NET_start_state (NV_net (nv, net_idx+1)));
	}
      else   /* state is not final; continue with EPSILON only */
	{
	  for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
	    if (LKP01_INPUT_MATCH [ARC_label (arc)] == EPSILON)
	      {
		if ((*outseq = LKP01_OUTPUT_MATCH [ARC_label (arc)]) == OTHER)
		  xout = 1;       /* one output symbol will be generated */
		else if (*outseq == EPSILON)
		  xout = 0;       /* no output will be generated */
		else  /* outlab == normal symbol */
		  xout = 1;       /* one output symbol will be generated */

		/* continue mapping recursively */

		LKP01_apply_seq_to_nv
		  (inseq, outseq + xout, nv, net_idx, ARC_destination (arc));
	      }
	}

      return;
    }

  /* case: not sequence end; try to map the current input symbol */

  for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
    {
      /* check whether the input ID is accepted by the arc */

      if ((inlab = LKP01_INPUT_MATCH [ARC_label (arc)]) == OTHER)
	{
	  if ((! binary_sigma_member (NET_sigma (NV_net (nv,net_idx)),*inseq))
	      || (*inseq == OTHER))
	    xin = 1;   /* input accepted and one symbol consumed */
	  else
	    continue;  /* input not accepted by OTHER */
	}
      else if (inlab == EPSILON)
	{
	  if (ARC_destination (arc) != state)  /* avoid EPSILON cycles */
	    xin = 0;   /* input accepted, but no symbol consumed */
	  else
	    continue;  /* input not accepted on EPSILON cycle */
	}
      else  /* inlab == normal symbol */
	{
	  if ((inlab == *inseq) || (*inseq == OTHER))
	    xin = 1;   /* input accepted and one symbol consumed */
	  else
	    continue;  /* input not accepted */
	}

      /* handle output ID */

      if ((*outseq = LKP01_OUTPUT_MATCH [ARC_label (arc)]) == OTHER)
	{
	  xout = 1;       /* one output symbol will be generated */
	  if (inlab == EPSILON)
	    *outseq = LKP01_UNKNOWN_SYMBOL;  /* a default symbol */
	  else
	    *outseq = *inseq;
	}
      else if (*outseq == EPSILON)
	xout = 0;       /* no output will be generated */
      else  /* outlab == normal symbol */
	xout = 1;       /* one output symbol will be generated */

      /* continue mapping recursively */

      LKP01_apply_seq_to_nv
	(inseq + xin, outseq + xout, nv, net_idx, ARC_destination (arc));
    }
}

NETptr LKP01_apply_sequence_to_net_vector
  (NVptr nv, id_type *seq, int input_side, int minimize_p,
   void (*LKP_res_fn)(id_type *res))
{
  /* Applies the ID sequence 'seq' (terminated by ID_EOS) sequentially to all
     transducers in the net vector 'nv'.
     Two types of results are possible:

     (1) If 'LKP_res_fn' is not NULL, then it will be called for every
         result with the corresponding ID sequence as argument.
	 In this case the user must write his own result function.
	 If there was no result at all, NULL will be returned.
	 Otherwise the return value is NULL+1.

     (2) If 'LKP_res_fn' is NULL, then the result will be returned as
         a network which can be minimized if minimize_p==DO_MINIMIZE.
	 If there was no result at all, NULL will be returned.

     All intermediate and the final ID sequence will be written into
     'seq[]' after the end of the input sequence.
     IT IS THE USER'S RESPONSIBILITY to provide enough space in this
     buffer.
     */

  id_type *outseq;

  /* select match tables */

  if (! UPPER_MATCH)
    update_match_tables ();

  if (input_side == UPPER)
    {
      LKP01_INPUT_MATCH = UPPER_MATCH;
      LKP01_OUTPUT_MATCH = LOWER_MATCH;
    }
  else
    {
      LKP01_INPUT_MATCH = LOWER_MATCH;
      LKP01_OUTPUT_MATCH = UPPER_MATCH;
    }

  LKP01_INPUT_MATCH [ALTCHAIN_SYMBOL] = EPSILON;
  LKP01_OUTPUT_MATCH [ALTCHAIN_SYMBOL] = EPSILON;

  /* CASE: call user's result function and do not make a resulting network */
  if ((LKP01_result_fn = LKP_res_fn))
    LKP01_result_net = NULL;   /* results will set it to NULL+1 */

  /* CASE: call standard result function and make a resulting network */
  else
    {
      LKP01_result_fn = LKP01_add_id_seq_to_result;
      LKP01_result_net = null_fsm ();  /* results will add pathes */
    }

  /* call the recursive lookup */

  outseq = seq + (LKP01_length_of_sequence (seq) + 1);

  LKP01_apply_seq_to_nv
    (seq, outseq, nv, 0, NET_start_state (NV_net (nv, 0)));

  /* handle result */

         /* user's result function took already care about the result */
  if (LKP_res_fn)
    return (LKP01_result_net);  /* is NULL+1 or NULL */

         /* pathes might have been added to 'LKP01_result_net' */
  if (minimize_p == DO_MINIMIZE)
    {
      NET_minimized (LKP01_result_net) = FALSE;
      NET_deterministic (LKP01_result_net) = FALSE;
      determinize_fsm (LKP01_result_net, DONT_COPY);
      min_fsm (LKP01_result_net, DONT_COPY);
    }

         /* check whether there is a result */
  if ((! NET_num_arcs (LKP01_result_net))
      && (! STATE_final (NET_start_state (LKP01_result_net))))
    {
      free_network (LKP01_result_net);    /* null_fsm */
      return (NULL);        /* no result */
    }

  return (LKP01_result_net);
}


/****************************************
 *
 *  TEST FUNCTIONS
 *
 **************************/

#endif /* SLIM_FST */
