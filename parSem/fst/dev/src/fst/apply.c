/* $Id: apply.c,v 1.6 2006/06/09 12:54:59 grondeau Exp $ */
/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "fst/apply.h"
#include "fst/str-buf.h"
#include "fst/lab-vect.h"
#include "fst/vector.h"
#include "fst/error.h"
#include "fst/struct.h"
#include "fst/rename.h"
#include "fst/share_arclist.h"
#include "fst/pars-tbl.h"
#include "fst/fsm_io.h"
#include "fst/flags.h"
#include "fst/apply.h"
#include "fst/id_type.h"
#include "fst/compact-defs.h"
#include "fst/net-traversal.h" /* Net traversal call-back routines */
#include "fst/vectorize.h"
#include "fst/virtual-base.h"
#include "fst/types.h"
#include "fst/utf8_fatstr_util.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/******************************************************
 **
 **  APPLY.C
 **
 **  Lauri Karttunen, XRCE, February 1999
 **
 **  New lookup routines
 **
 ******************************************************/

/* Determines the maximum size of the pattern for right-oriented
   constraints (RC and ~RC) in apply_patterns(). */

#define MAX_CONTEXT_LENGTH 64
#define LAB_RING_INITIAL_SIZE 100

#define io_sym_at(X, Y) \
        lab_vector_element_at(IO_SEQ_io_vector(X), Y)
#define other_than_sym_at(X, Y) \
	lab_vector_element_at(IO_SEQ_other_than_vector(X), Y)
#endif /* SLIM_FST */

#define EPSILON_MATCH(X) (X == EPSILON || X == ALTCHAIN_SYMBOL)


#define NO_INPUT(X, Y) (EPSILON_MATCH(X) || \
		        (flag_diacritic_p(X) && APPLY_obey_flags_p(Y)))

/*
 #define OTHER_MATCH(X, Y, Z) (X != ID_NO_SYMBOL && Y == OTHER && \
			      !binary_sigma_member(Z, X))
*/


id_type FstBaseExport *INPUT_MATCH;  /* match table of the input side */
id_type FstBaseExport *OUTPUT_MATCH; /* match table of the output side */
/*
static int other_than_test(id_type sym_id, APPLYptr cntxt)
{
  id_type id;
  lab_vector_element_at(&id, APPLY_in_pos(cntxt), APPLY_ot_vect(cntxt));
  return (id != sym_id);
}
*/

static int other_than_test(id_type sym_id, APPLYptr cntxt)
{
  IO_SEQptr in_seq = APPLY_in_seq(cntxt);
  int pos = APPLY_in_pos(cntxt);
  LAB_VECTORptr other_than_vect = IO_SEQ_other_than_vector(in_seq);
  id_type other_id;
  VECTORptr sig_vect;
  int i;
  ALPHABETptr sig;

  other_id = LAB_VECTOR_array(other_than_vect)[pos];
  if (sym_id == other_id)
    return(FALSE);

  vector_element_at((void**)&sig_vect, pos, IO_SEQ_prev_sigma_vector(in_seq));

  if (sig_vect)
    for (i = 0; i < VECTOR_pos(sig_vect); i++)
      {
	vector_element_at((void **) &sig, i, sig_vect);
	if (binary_sigma_member(sig, sym_id))
	  return(FALSE);
      }

  return(TRUE);
}


static int OTHER_MATCH(id_type in_sym, id_type arc_sym, APPLYptr cntxt)
{
    if (arc_sym != OTHER)
        return(FALSE);
    else if (in_sym == ID_NO_SYMBOL)
        return(FALSE);
    else
        return(!binary_sigma_member(APPLY_sigma(cntxt), in_sym));
}

static int ANY_MATCH(id_type in_sym, id_type arc_input_sym, APPLYptr cntxt)
{	
  /* If the input symbol is OTHER, it matches any arc_sym that
     is not in the sigma of the previous network that was applied
     and, if we are at the second stage of compose-apply, is
     different from the original input symbol. Turn this to a macro?
     */

  if (in_sym != OTHER)
    return(FALSE);
  else if (!APPLY_prev_sigma(cntxt))
    return(TRUE);
  else
    return(other_than_test(arc_input_sym, cntxt));
}


IO_SEQptr make_io_sequence(int length)
{
  IO_SEQptr io_seq;

  check_for_negative_or_zero(length, "MAKE_IO_SEQUENCE");
  
  io_seq =  (IO_SEQptr) malloc((size_t) sizeof(IO_SEQtype));

  if (!io_seq)
    not_enough_memory("MAKE_IO_SEQUENCE");

  IO_SEQ_io_pos(io_seq) = -1;
  IO_SEQ_io_vector(io_seq) = make_lab_vector(length);
  IO_SEQ_other_than_vector(io_seq) = make_lab_vector(length);
  IO_SEQ_other_than_index(io_seq) = make_vector(length);
  IO_SEQ_prev_sigma_vector(io_seq) = make_vector(length);

  return(io_seq);
}


 void free_io_sequence(IO_SEQptr io_seq)
{
  if (io_seq)
    {
      VECTORptr prev_sigma_vector = IO_SEQ_prev_sigma_vector(io_seq);
      int i;

      reclaim_lab_vector(IO_SEQ_io_vector(io_seq));
      reclaim_lab_vector(IO_SEQ_other_than_vector(io_seq));
      free_vector(IO_SEQ_other_than_index(io_seq));

      for (i = 0; i < VECTOR_length(prev_sigma_vector); i++)
      {
        VECTORptr sig_vect= (VECTORptr) VECTOR_array(prev_sigma_vector)[i];
        free_vector(sig_vect);
      }

      free_vector(prev_sigma_vector);
      free((void *) io_seq);
    }
}


void reset_io_sequence(IO_SEQptr io_seq)
{
  int i;
  void **index_array = VECTOR_array(IO_SEQ_other_than_index(io_seq));
  void **prev_sigma_array = VECTOR_array(IO_SEQ_prev_sigma_vector(io_seq));
  id_type *other_than_array =
    LAB_VECTOR_array(IO_SEQ_other_than_vector(io_seq));
  VECTORptr sig_vect;

  for (i = 0; i < VECTOR_pos(IO_SEQ_other_than_index(io_seq)); i++)
    {
      other_than_array[(int)index_array[i]] = 0;
      sig_vect = prev_sigma_array[(int)index_array[i]];
      if (sig_vect)
      {
          /* Free the vector "sig_vect" otherwise it is likely to be
             smashed out by further calls to vector_element_at()
             because the "pos" of prev_sigma_vector will be reset
             to 0 (below).
             Freeing the vector "sig_vect" is safer than resetting it.*/
	free_vector(sig_vect);
        prev_sigma_array[(int)index_array[i]]= NULL;
      }
    }

  LAB_VECTOR_pos(IO_SEQ_io_vector(io_seq)) = 0;
  LAB_VECTOR_pos(IO_SEQ_other_than_vector(io_seq)) = 0;
  VECTOR_pos(IO_SEQ_other_than_index(io_seq)) = 0;
  VECTOR_pos(IO_SEQ_prev_sigma_vector(io_seq)) = 0;
  IO_SEQ_io_pos(io_seq) = -1;
}

void init_io_sequence(IO_SEQptr *io_seq)
{
  if (*io_seq)
    reset_io_sequence(*io_seq);
  else
    *io_seq = make_io_sequence(10);
}

/************************* IO_SEQUENCE_TABLE ********************/


IO_SEQ_TABLEptr make_io_sequence_table(int table_length, int seq_length)
{
  IO_SEQ_TABLEptr table;
  IO_SEQptr *array;
  int i;

  check_for_negative_or_zero(table_length, "MAKE_IO_SEQ_TABLE");
  check_for_negative_or_zero(seq_length, "MAKE_IO_SEQ_TABLE");

  table = (IO_SEQ_TABLEptr) malloc((size_t) sizeof(IO_SEQ_TABLEtype));

  if (!table)
    not_enough_memory("MAKE_IO_SEQ_TABLE");

  array =
    (IO_SEQptr *) malloc((size_t) (table_length * sizeof(IO_SEQptr)));

  if (!array)
    not_enough_memory("MAKE_IO_SEQ_TABLE");

  IO_SEQ_TABLE_array(table) = array;
  IO_SEQ_TABLE_length(table) = table_length;
  IO_SEQ_TABLE_seq_length(table) = seq_length;
  IO_SEQ_TABLE_pos(table) = 0;

  for (i = 0; i < table_length; i++)
    array[i] = make_io_sequence(seq_length);

  return(table);
}

void free_io_sequence_table(IO_SEQ_TABLEptr io_table)
{
  int i, len;
  IO_SEQptr *array;

  if (io_table)
    {
      len = IO_SEQ_TABLE_length(io_table);
      array = IO_SEQ_TABLE_array(io_table);

      for (i = 0; i < len; i++)
	free_io_sequence(array[i]);

      free(array);
      free(io_table);
    }
}

void reset_io_sequence_table(IO_SEQ_TABLEptr table)
{
  int i, pos = IO_SEQ_TABLE_pos(table);
  IO_SEQptr *array = IO_SEQ_TABLE_array(table);

  for (i = 0; i < pos; i++)
  {
      if (array[i] != NULL)
          reset_io_sequence(array[i]);
#ifdef DEBUG
      else
          fsm_fprintf(stderr, "NULL IO_SEQptr detected in RESET_IO_SEQUENCE_TABLE\n");
#endif

  }

  IO_SEQ_TABLE_pos(table) = 0;
}

void init_io_sequence_table(IO_SEQ_TABLEptr *table)
{
  if (*table)
    reset_io_sequence_table(*table);
  else
    *table = make_io_sequence_table(5, 20);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

int io_sequence_at(IO_SEQptr *io_seq, int pos, IO_SEQ_TABLEptr table)
{

  if (pos >= IO_SEQ_TABLE_pos(table))
    {
      *io_seq = NULL;
      return(1);
    }

  *io_seq = IO_SEQ_TABLE_array(table)[pos];

  return(0);

}
#endif /* SLIM_FST */

void assure_io_sequence_table_space(int len, IO_SEQ_TABLEptr table)
{
  int new_pos = IO_SEQ_TABLE_pos(table) + len;
  int cur_length = IO_SEQ_TABLE_length(table);
  int i;

  if (new_pos > cur_length)
    {
      int new_length = cur_length;
      int seq_length = IO_SEQ_TABLE_seq_length(table);
      IO_SEQptr *array = IO_SEQ_TABLE_array(table);

      do {
        new_length = 2 * new_length;
      } while (new_pos > new_length);

      array = (IO_SEQptr *)
        realloc(array, (size_t) (new_length * sizeof(IO_SEQptr)));

      if (!array)
        not_enough_memory("ASSURE_IO_SEQ_TABLE_SPACE");

      for (i = cur_length; i < new_length; i++)
        array[i] = make_io_sequence(seq_length);

      IO_SEQ_TABLE_length(table) = new_length;
      IO_SEQ_TABLE_array(table) = array;
    }
  else if (IO_SEQ_pos(IO_SEQ_TABLE_array(table)[IO_SEQ_TABLE_pos(table)]) != 0)
    reset_io_sequence(IO_SEQ_TABLE_array(table)[IO_SEQ_TABLE_pos(table)]);
}


void print_io_seq_table(IO_SEQ_TABLEptr table, FILE *stream, int escape_p)
{

  int i;
  IO_SEQptr *array = IO_SEQ_TABLE_array(table);
  
  for (i = 0; i < IO_SEQ_TABLE_pos(table); i++)
    {
      print_lab_vector(IO_SEQ_io_vector(array[i]), stream, escape_p);
      fsm_fprintf(stream, "\n");
    }

}

/*********************************************************************/


static int read_from_string(id_type *in_sym, void * cntxt)
{
    int error_code= NO_ERROR;
    
    /* Retrieves the next symbol from the input or NO_SYMBOL_ID if
       there is no more input. */
    error_code= symbol_from_string(in_sym, APPLY_remainder((APPLYptr) cntxt),
                                   &APPLY_remainder((APPLYptr) cntxt),
                                   APPLY_parse_table((APPLYptr) cntxt),
                                   FALSE, FALSE, FALSE);
    return (error_code);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/* Must be aligned with the prototype of APPLY_next_symbol_fn() : */
static int read_char_from_stream(id_type *id, void *cntxt)
{
  /* Interns the next character from the stream and sets ID
     to the appropriate value. This function can be used
     instead of symbol_from_stream() by applications that
     do not need a parse table because it is known that
     the input consists of single character
     symbols */

  int char_code = fsm_fgetc(APPLY_in_stream((APPLYptr) cntxt));

  if (char_code == EOF)
    *id = ID_NO_SYMBOL;
  else
    intern_char(id, (char) char_code);
  
  return(NO_ERROR);
}

/* Must be aligned with the prototype of APPLY_next_symbol_fn() : */
static int read_from_stream(id_type *in_sym, void * cntxt)
{
    /* Retrieves the next symbol from the stream or ID_NO_SYMBOL if
       there is no more input. */
    int error= NO_ERROR;
    
    error= next_symbol(in_sym, APPLY_in_stream((APPLYptr) cntxt),
                       APPLY_parse_table((APPLYptr) cntxt),
                       FALSE, FALSE, FALSE);
    return(error);
}

static void symbol_name_to_buffer(id_type id, STRING_BUFFERptr out_buf)
{
  FST_fat_string_to_buf(id_to_atom_name(id), out_buf);
}
#endif /* SLIM_FST */

static void init_vector(VECTORptr *vector)
{
  if (*vector == NULL)
    *vector = make_vector(80);
  else
    VECTOR_pos(*vector) = 0;
}


static void init_id_vector(LAB_VECTORptr *lab_vector)
{
  if (*lab_vector == NULL)
    *lab_vector = make_lab_vector(80);
  else
    reset_lab_vector(*lab_vector);
}


static void init_lab_vector_table(LAB_VECTOR_TABLEptr *table)
{
  if (*table == NULL)
    *table = make_lab_vector_table(5, 10);
  else
    reset_lab_vector_table(*table);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

static void init_input_ring(LAB_RINGptr *lab_ring)
{
  if (*lab_ring == NULL)
    *lab_ring = make_lab_ring(LAB_RING_INITIAL_SIZE, MAX_CONTEXT_LENGTH);
  else
    reset_lab_ring(*lab_ring);
}
#endif /* SLIM_FST */

static void assure_input_sym_in_vect(APPLYptr cntxt)
{
    assure_lab_vector_space(INPUT_LINE_MAX_LEN, APPLY_in_vect(cntxt));
}

APPLYptr init_apply_context(APPLYptr cntxt)
{
    /* Creates a context object if called with a NULL parameter */

  if (cntxt == NULL)
    {
      cntxt = (APPLYptr) malloc (sizeof(APPLYtype));
      
      if (cntxt == NULL)
	not_enough_memory("INITIALIZE_APPLY_CONTEXT");

      memset (cntxt, 0, sizeof(APPLYtype));
    }

  /* Create or reset input/output vectors. */

  init_vector(&APPLY_arc_vector(cntxt));
  init_vector(&APPLY_state_vector(cntxt));
  init_id_vector(&APPLY_in_vector(cntxt));   /* Not in use anymore, replaced by APPLY_in_seq(cntxt) */
  init_id_vector(&APPLY_mid_vector(cntxt));
  init_id_vector(&APPLY_out_vector(cntxt));

#ifndef SLIM_FST
  init_input_ring(&APPLY_input_ring(cntxt));
#endif /* SLIM_FST */

  /* Initial size: 5 vectors of length 10. */

  init_lab_vector_table(&APPLY_in_table(cntxt));
  init_lab_vector_table(&APPLY_out_table(cntxt));

  /* Create or reset vectors than keep track of input symbols
     that yield a non-identity OTHER on the output side
     at the first stage of compose-apply.
     */

  init_id_vector(&APPLY_other_than_vector(cntxt));

  /* Create or reset vectors that keeps an input/output vector
     aligned with a vector that contains the symbols
     that yielded a non-identity OTHER at the previous
     stage of compose-apply.
     */

  /* Reset of the buffer containing the intermediate output sequence: */
  init_io_sequence(&APPLY_out_seq(cntxt));
  
  /* Reset of the buffer containing the intermediate input sequence: */
  init_io_sequence(&APPLY_in_seq(cntxt));

  /* Assure enough space in APPLY_in_vect:
     (reallocating dynamically is time consuming). */
  assure_input_sym_in_vect(cntxt); 
  
  /* Reset of the IO_EQ_TABLE containing the set of the different input
     sequences of this context (used by the non-recursive version only) */
  init_io_sequence_table(&APPLY_input_table(cntxt));
  
  /* Reset of the IO_EQ_TABLE containing the set of the different output
     sequences of the context:
     this table is supposed to be filled only for the last context in the
     apply_context_vector; if the last set of ouput strings has rather
     been stored in a solution_tree, then the" output_table" doesn't
     contain the solution strings but it's however NOT empty: it stores
     information for interpreting the meaning of OTHER in case it appears
     as ouput symbol. */
  
  init_io_sequence_table(&APPLY_output_table(cntxt));
  
  /* NOTE: the solution_tree - if any - is reset in the function
     reinitialize_for_apply(), but even when reset, the context's field
     APPLY_solution_tree is not NULL: it still contains the root state
     of an empty tree. */
	       
  /* Create or reset output buffer. */

  if (!APPLY_out_buffer(cntxt))
    APPLY_out_buffer(cntxt) = make_string_buffer(80);
  else
    initialize_string_buffer(APPLY_out_buffer(cntxt));

  /* Choose input/output functions. */

  if (APPLY_side(cntxt) == UPPER)
    {
      APPLY_in_fn(cntxt) = upper_id;
      APPLY_out_fn(cntxt) = lower_id;
    }
  else
    {
      APPLY_in_fn(cntxt) = lower_id;
      APPLY_out_fn(cntxt) = upper_id;
    }

  /* No previous net in the input. OTHER will be interpreted as ANY. */

  APPLY_prev_sigma(cntxt) = NULL;
  APPLY_out_count(cntxt) = 0;
  
  return(cntxt);
}


static int free_solution_tree(STATEptr root_state);

void free_apply_context(APPLYptr cntxt)
{
  if (cntxt)
    {
      /* DON'T free the cascade of networks in APPLY_net_vector(cntxt)
         because they are not copied to the context and hence they may
         be pointed from somewhere else. */
/*       free_nv_and_nets(APPLY_net_vector(cntxt)); */
        
      /* Create or reset input/output vectors. */
      free_vector(APPLY_arc_vector(cntxt));
      free_vector(APPLY_state_vector(cntxt));
      reclaim_lab_vector(APPLY_mid_vector(cntxt));
      reclaim_lab_vector(APPLY_out_vector(cntxt));
      reclaim_lab_vector(APPLY_in_vector(cntxt)); /* Not in use anymore,
                                                     replaced by
                                                     APPLY_in_seq(cntxt) */

      /* Initial size: 5 vectors of length 10. */

      reclaim_lab_vector_table(APPLY_in_table(cntxt));
      reclaim_lab_vector_table(APPLY_out_table(cntxt));

      /* Create or reset vectors than keep track of input symbols
	 that yield a non-identity OTHER on the output side
	 at the first stage of compose-apply.
	 */

      reclaim_lab_vector(APPLY_other_than_vector(cntxt));

      /* Create or reset vectors that keeps an input/output vector
	 aligned with a vector that contains the symbols
	 that yielded a non-identity OTHER at the previous
	 stage of compose-apply.
	 */

      free_io_sequence(APPLY_out_seq(cntxt));
      free_io_sequence(APPLY_in_seq(cntxt));
      free_io_sequence_table(APPLY_input_table(cntxt));
      free_io_sequence_table(APPLY_output_table(cntxt));
	       
      /* Create or reset output buffer. */

      free_string_buffer(APPLY_out_buffer(cntxt));

      /* free the solution_tree if any */
      if (APPLY_solution_tree(cntxt) != NULL)
          free_solution_tree(APPLY_solution_tree(cntxt));

      /* Free the lab_ring if any: */
      if (APPLY_input_ring(cntxt) != NULL)
          reclaim_lab_ring(APPLY_input_ring(cntxt));

      if (APPLY_flag_vector(cntxt) != NULL)
        reclaim_lab_vector(APPLY_flag_vector(cntxt));

      /* Free the block of LOCATIONptr-s if any: */
      if (APPLY_location_heap(cntxt) != NULL)
      {
          free(APPLY_location_heap(cntxt));
          APPLY_location_heap(cntxt)= NULL;
      }

      free(cntxt);
    }
}

/* Must be aligned with the prototype of APPLY_next_symbol_fn() : */
int symbol_from_in_vector(id_type *in_sym, void *apply_context)
{
  LAB_VECTORptr in_vector = APPLY_in_vect((APPLYptr) apply_context);
  int pos = APPLY_in_pos((APPLYptr) apply_context);

  if (pos < LAB_VECTOR_pos(in_vector))
    lab_vector_element_at(in_sym, pos, in_vector);
  else
    *in_sym = ID_NO_SYMBOL;

  return(NO_ERROR);
}

/* Set up of the global variables INPUT_MATCH and OUTPUT_MATCH at the
   initialization of the vectorized-apply, depending on the
   value of the input_side.  (Imported from lookup/networks.c) */
void select_match_tables_for_apply (int input_side)
{
  /* Select the match tables: */

    update_match_tables ();

  if (input_side == UPPER)
    {
      INPUT_MATCH = UPPER_MATCH;
      OUTPUT_MATCH = LOWER_MATCH;
    }
  else
    {
      INPUT_MATCH = LOWER_MATCH;
      OUTPUT_MATCH = UPPER_MATCH;
    }

  INPUT_MATCH [ALTCHAIN_SYMBOL] = EPSILON;
  OUTPUT_MATCH [ALTCHAIN_SYMBOL] = EPSILON;
} /* select_match_tables_for_apply */


void init_apply_net(APPLYptr cntxt)
{
    
  /* If the net1 has a reduced labelset, pick up a match table. */

  if (NET_reduced_labelset(APPLY_net1(cntxt)))
    {
      APPLY_match_table(cntxt) =
	get_match_table(APPLY_net1(cntxt), APPLY_side(cntxt));
    }
    
  /* Check if we need to check flag diacritics and set up
     the appropriate data structures if necessary. */

  if (((NET_u_flag_diacr(APPLY_net1(cntxt)) && APPLY_side(cntxt) == UPPER) ||
       (NET_l_flag_diacr(APPLY_net1(cntxt)) && APPLY_side(cntxt) == LOWER))
      && APPLY_obey_flags_p(cntxt))
    {
      APPLY_flags_p(cntxt) = TRUE;

      if (!(NET_flag_register(APPLY_net1(cntxt))))
	NET_flag_register(APPLY_net1(cntxt)) = make_flag_register();

      APPLY_flag_register(cntxt) = NET_flag_register(APPLY_net1(cntxt));

      if (!APPLY_flag_vector(cntxt))
	APPLY_flag_vector(cntxt) = make_lab_vector(80);
    }
  else
    APPLY_flags_p(cntxt) = FALSE;

  /* Check if the net is vectorized and allocate a location heap 
     if necessary, (this heap is required for the kernel algorithm
     apply_vectorized_network()). */
    if (NET_arc_vector_heap(APPLY_net1(cntxt)) != NULL)
    {
        if (cntxt->location_heap == NULL)
        {  
            cntxt->location_heap= (LOCATIONtype *) malloc(1000*sizeof(LOCATIONtype));
        }
    }
} /* init_apply_net */


static void apply_get_parse_table(APPLYptr apply_context)
{
 /* Get the appropriate parse table */
    
#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 
  if (APPLY_net2(apply_context))
    {
      APPLY_parse_table(apply_context) =
	get_bimachine_parse_table(APPLY_net1(apply_context),
				  APPLY_net2(apply_context),
				  APPLY_side(apply_context));
    }
  else
#endif /* SLIM_FST */
   
      APPLY_parse_table(apply_context) =
      get_parse_table(APPLY_net1(apply_context), APPLY_side(apply_context));
}

void init_apply_to_string(char *input, APPLYptr apply_context)
{

  /* Initializes APPLY_CONTEXT to parse the input. */

  APPLY_input(apply_context) = input;
  APPLY_remainder(apply_context) = input;
  
  APPLY_next_symbol_fn(apply_context) = read_from_string;
  APPLY_in_pos(apply_context) = 0;

  apply_get_parse_table(apply_context);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 
void init_apply_to_stream(FILE *in_stream, APPLYptr apply_context)
{

  /* Initializes APPLY_CONTEXT to parse the input. Uses either
     read_from_stream() or read_char_from_stream(). The
     latter is appropriate when it is known that there the
     input contains no multicharacter symbols and we are not
     in UTF-8 mode.  */

  APPLY_in_stream(apply_context) = in_stream;
  
  apply_get_parse_table(apply_context);

  if (PARSE_TBL_MULTICHAR(APPLY_parse_table(apply_context)))
    APPLY_next_symbol_fn(apply_context) = read_from_stream;
  else
    APPLY_next_symbol_fn(apply_context) = read_char_from_stream;

  APPLY_in_pos(apply_context) = 0;
  

 }
#endif /* SLIM_FST */

/* Input symbol function for standard, compacted, LabelSet reduced,
   bimachine nets. */

/* Must be aligned with the prototype of APPLY_next_symbol_fn() : */
static int fetch_input_symbol(id_type *in_sym, APPLYptr cntxt)
{
  /* Returns a previously read or a new input symbol depending
     on APPLY_in_pos(cntxt). A new symbol is appended to in_vect.
     Used by apply_standard_network(), apply_mixed_network(),
     apply_reduced_network(), and apply_bimachine().
  */
    int error= NO_ERROR;

  if (APPLY_in_pos(cntxt) >= LAB_VECTOR_pos(APPLY_in_vect(cntxt)))
    {
      error= APPLY_next_symbol_fn(cntxt)(in_sym, cntxt);
      append_to_lab_vector(*in_sym, APPLY_in_vect(cntxt));
    }
  else
    lab_vector_element_at(in_sym, APPLY_in_pos(cntxt), APPLY_in_vect(cntxt));
  
  return(error);
}


/* Input symbol function for vectorized nets.
   (does not allocate space on demand in the input lab_vector).
   Must be aligned with the prototype of APPLY_next_symbol_fn() : */
static int fetch_input_symbol_vect(id_type *in_sym, APPLYptr cntxt)
{
  /* Returns a previously read or a new input symbol depending
     on APPLY_in_pos(cntxt). A new symbol is appended to in_vect.
     Used by apply_vectorized_network() only. */

    int error= NO_ERROR;

    int pos= LAB_VECTOR_pos(APPLY_in_vect(cntxt));

    if (APPLY_in_pos(cntxt) >= LAB_VECTOR_pos(APPLY_in_vect(cntxt)))
    {
        error= APPLY_next_symbol_fn(cntxt)(in_sym, cntxt);
        
        if (pos == LAB_VECTOR_length(APPLY_in_vect(cntxt))) {
            /* grow in_vect */
            assure_lab_vector_space(1, APPLY_in_vect(cntxt));
        }
        /* append to vector: */
        LAB_VECTOR_array(APPLY_in_vect(cntxt))[pos]= *in_sym;
        LAB_VECTOR_pos(APPLY_in_vect(cntxt))= pos+1;
    }
  else
    *in_sym= LAB_VECTOR_array(APPLY_in_vect(cntxt))[APPLY_in_pos(cntxt)];

    return(error);

} /* fetch_input_symbol_vect */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

static void fetch_input_at_pos(id_type *in_sym, int pos, APPLYptr cntxt)
{
  /* Returns a previously read or a new input symbol depending
     at a given position. A new symbol is appended to in_vect.
     Used for implementing a lookahead condition in apply_patterns.
  */

  LAB_RINGptr input_ring = APPLY_input_ring(cntxt);
  int start_pos = LAB_RING_start_pos(input_ring);
  int cur_pos = LAB_RING_cur_pos(input_ring);
  int size = LAB_RING_size(input_ring);

  int addr = (start_pos + pos) % LAB_RING_size(input_ring);

  if (addr < 0)
    {
      addr = size + addr;
      
      if (addr < cur_pos)
	*in_sym = ID_NO_SYMBOL;
    }

  if (addr == cur_pos)
    {
      APPLY_next_symbol_fn(cntxt)(in_sym, cntxt);
      append_to_lab_ring(*in_sym, input_ring);
    }
  else
    lab_ring_element_at(in_sym, addr, input_ring);

}

static void label_component_to_buffer(id_type sym, STRING_BUFFERptr buffer)
{
  if (sym == EPSILON)
    {
     append_char_to_buffer('0', buffer);
    }
  else if (sym == OTHER)
    {
      append_char_to_buffer('?', buffer);
    }
  else
    {
      symbol_name_to_buffer(sym, buffer);
    }
}

static void symbol_pair_to_buffer(id_type u, id_type l,  STRING_BUFFERptr buffer)
{
  append_char_to_buffer('<', buffer);
  label_component_to_buffer(u, buffer);
  append_char_to_buffer(':', buffer);
  label_component_to_buffer(l, buffer);
  append_char_to_buffer('>', buffer);
}
#endif /* SLIM_FST */


static void record_sigma_in_out_seq(IO_SEQptr out_seq, ALPHABETptr sig,
				    APPLYptr cntxt)
{
  VECTORptr prev_sigma_vector = IO_SEQ_prev_sigma_vector(out_seq);
  VECTORptr sig_vect;
  int pos = LAB_VECTOR_pos(IO_SEQ_io_vector(out_seq));

  int k = (pos - VECTOR_length(prev_sigma_vector)) + 1;
  

  if (k > 0)
    assure_vector_space(k, prev_sigma_vector);

  vector_element_at((void **) &sig_vect, pos, prev_sigma_vector);

  if (!sig_vect)
    {
      int old_pos = VECTOR_pos(prev_sigma_vector);
      int len;

      if (APPLY_recursive_p(cntxt))
	len = APPVEC_len((APPVECptr) APPLY_hyper_unit(cntxt));
      else
	len = NV_len(APPLY_net_vector(cntxt));
	
      sig_vect = make_vector(len);
      set_vector_element_at((void*) sig_vect, pos, prev_sigma_vector);
      
      if (old_pos <= pos)
	VECTOR_pos(prev_sigma_vector) = pos + 1;
    }

  /* Append the sigma sig to sig_vect: */
  append_to_vector((void *) sig, sig_vect);
}


static void copy_prev_sigmas(int in_pos, IO_SEQptr in_seq, IO_SEQptr out_seq,
			     APPLYptr cntxt)
{
  VECTORptr prev_sigma_vector = IO_SEQ_prev_sigma_vector(in_seq);
  VECTORptr sig_vect;
  ALPHABETptr sig;
  int i;

  vector_element_at((void**)&sig_vect, in_pos, prev_sigma_vector);

  if (sig_vect)
    for (i = 0; i < VECTOR_pos(sig_vect); i++)
      {
	vector_element_at((void **)&sig, i, sig_vect);
	record_sigma_in_out_seq(out_seq, sig, cntxt);
      }
}



static void record_restrictions(id_type in_sym, id_type arc_label,
				IO_SEQptr out_seq, APPLYptr cntxt)
{
  int out_pos = LAB_VECTOR_pos(IO_SEQ_io_vector(out_seq));
  LAB_VECTORptr other_than_vector = IO_SEQ_other_than_vector(out_seq);
  VECTORptr other_than_loc_vector = IO_SEQ_other_than_index(out_seq);
  id_type other_id, match_id;

  if (in_sym == OTHER)
    {
      IO_SEQptr in_seq = APPLY_in_seq(cntxt);
      int in_pos = IO_SEQ_io_pos(in_seq);

      /* Need to distinquish between an OTHER and OTHER:OTHER.
	 In the case of an identity OTHER, all the restrictions
	 associated with the input OTHER are inherited by the
	 output OTHER.
      */
 
      if (arc_label == OTHER)
	{
	  LAB_VECTORptr prev_other_than_vector =
	    IO_SEQ_other_than_vector(in_seq);
	  
	  lab_vector_element_at(&other_id, in_pos, prev_other_than_vector);
	  copy_prev_sigmas(in_pos, in_seq, out_seq, cntxt);
	}
      else
	{
	  match_id = APPLY_in_fn(cntxt)(arc_label);

	  if (match_id != OTHER)
	    other_id = match_id;
	  else
	    other_id = EPSILON;   /* No restriction */
	}
    }
  else
    other_id = in_sym;

  set_lab_vector_element_at(other_id, out_pos, other_than_vector);
  record_sigma_in_out_seq(out_seq, APPLY_sigma(cntxt), cntxt);
  append_to_vector((void *) out_pos, other_than_loc_vector);
}



static void out_sym_to_lab_vector(id_type out_sym, id_type in_sym, 
				  id_type arc_label, IO_SEQptr out_seq,
				  APPLYptr cntxt)
{
  int output_side =  APPLY_out_side(cntxt);
  int obey_flags_p =  APPLY_obey_flags_p(cntxt);
  int side =  APPLY_side(cntxt);
  LAB_VECTORptr out_vector = IO_SEQ_io_vector(out_seq);


  /* OUT_SYM is the output side of ARC_LABEL, IN_SYM is the input
     symbol that matched the input side of ARC_LABEL. If ARC_LABEL
     is OTHER, we store the input symbol.
     */

  if (output_side == BOTH_SIDES && 
      (in_sym != out_sym || tuple_id_p(arc_label)))
    {
      if (arc_label == OTHER)
	append_to_lab_vector(in_sym, out_vector);
      else if (side == UPPER)
	append_to_lab_vector(arc_label, out_vector);
      else
	append_to_lab_vector(arc_label, out_vector);
    }
  else
    {
      if (!(EPSILON_MATCH(out_sym) || 
	    ( flag_diacritic_p(out_sym) && obey_flags_p)))
	{
	  if (out_sym == OTHER)
	    record_restrictions(in_sym, arc_label, out_seq, cntxt);

	  if (arc_label == OTHER)
	    append_to_lab_vector(in_sym, out_vector);
	  else
	    append_to_lab_vector(out_sym, out_vector);
	}
    }
}

static void out_sym_to_out_seq(id_type out_sym, id_type in_sym,
			       id_type arc_label, APPLYptr cntxt)
{
  out_sym_to_lab_vector(out_sym, in_sym, arc_label, APPLY_out_seq(cntxt),
			cntxt);
}

static void out_sym_to_out_table(id_type out_sym, id_type in_sym,
				 id_type arc_label, APPLYptr cntxt)
{
  IO_SEQ_TABLEptr output_table = APPLY_output_table(cntxt);
  int pos = IO_SEQ_TABLE_pos(output_table);
  IO_SEQptr out_seq = IO_SEQ_TABLE_array(output_table)[pos];

  out_sym_to_lab_vector(out_sym, in_sym, arc_label, out_seq, cntxt);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

static void out_sym_to_out_stream(id_type out_sym, id_type in_sym, 
				 id_type arc_label, APPLYptr cntxt)
{
  int output_side =  APPLY_out_side(cntxt);
  int obey_flags_p =  APPLY_obey_flags_p(cntxt);
  int show_flags_p = APPLY_show_flags_p(cntxt);
  FILE *out_stream = APPLY_out_stream(cntxt);

  /* OUT_SYM is the output side of ARC_LABEL, IN_SYM is the input
     symbol that matched the input side of ARC_LABEL. If ARC_LABEL
     is OTHER, we print the input symbol. If OUT_SYM is a one-sided
     OTHER, we display it as '?'. If output_side is BOTH_SIDES,
     we display a upper:lower symbol pair, except in the case when
     ARC_LABEL is the identity OTHER.
  */

  if (output_side == BOTH_SIDES)
    {
      if (APPLY_print_space_p(cntxt))
	fputc(' ', out_stream);

      if (arc_label == OTHER || (out_sym == in_sym && out_sym != OTHER))
	print_label(in_sym, out_stream, FALSE);
      else if (in_sym != out_sym)
	print_label(arc_label, out_stream, FALSE);
      else
	print_label(out_sym, out_stream, FALSE);


      if (arc_label == OTHER)
	print_label(in_sym, out_stream, FALSE);
      else
	print_label(out_sym, out_stream, FALSE);
    }
  else
    {
      if (!(EPSILON_MATCH(out_sym) || 
	    (flag_diacritic_p(out_sym) && obey_flags_p && !show_flags_p)))
	{

	  if (APPLY_print_space_p(cntxt))
	    fputc(' ', out_stream);

	  if (arc_label == OTHER)
	    print_label(in_sym, out_stream, FALSE);
	  else if (out_sym == OTHER)
	    fprintf(out_stream, "?");
	  else
	    print_label(out_sym, out_stream, FALSE);
	}
    }
}

static void out_sym_to_buffer(id_type out_sym, id_type in_sym, 
                              id_type arc_label, APPLYptr cntxt)
{
  STRING_BUFFERptr out_buf =  APPLY_out_buffer(cntxt);
  int output_side =  APPLY_out_side(cntxt);
  int obey_flags_p =  APPLY_obey_flags_p(cntxt);
  int show_flags_p = APPLY_show_flags_p(cntxt);

  /* OUT_SYM is the output side of ARC_LABEL, IN_SYM is the input
     symbol that matched the input side of ARC_LABEL. If ARC_LABEL
     is OTHER, we print the input symbol. If OUT_SYM is a one-sided
     OTHER, we display it as '?'. If output_side is BOTH_SIDES,
     we display a upper:lower symbol pair, except in the case when
     ARC_LABEL is the identity OTHER.
     */

  if (output_side == BOTH_SIDES)
    {
      if (APPLY_print_space_p(cntxt) && STRING_BUFFER_pos(out_buf) > 0)
	append_char_to_buffer(' ', out_buf);

      if (arc_label == OTHER || (out_sym == in_sym && out_sym != OTHER))
	symbol_name_to_buffer(in_sym, out_buf);
      else if (APPLY_side(cntxt) == UPPER)	
	symbol_pair_to_buffer(in_sym, out_sym, out_buf);
      else
	symbol_pair_to_buffer(out_sym, in_sym, out_buf);
    }
  else
    {
      if (!(EPSILON_MATCH(out_sym) || 
	    (flag_diacritic_p(out_sym) && obey_flags_p && !show_flags_p)))
	{
	  if (APPLY_print_space_p(cntxt) && STRING_BUFFER_pos(out_buf) > 0)
	    append_char_to_buffer(' ', out_buf);

	  if (arc_label == OTHER)
	    symbol_name_to_buffer(in_sym, out_buf);
	  else if (out_sym == OTHER)
	    append_char_to_buffer('?', out_buf);
	  else
	    symbol_name_to_buffer(out_sym, out_buf);
	}
    }
}
#endif /* SLIM_FST */


static void standard_network_output(APPLYptr cntxt, void (*output_fn)(id_type, id_type, id_type, APPLYptr))
{
  /* Output routine for apply_standard_network(). */

  id_type (*in_fn)(id_type) = APPLY_in_fn(cntxt);
  id_type (*out_fn)(id_type) = APPLY_out_fn(cntxt);
  id_type in_sym, arc_label, arc_sym;
  ARCptr arc;
  VECT_ENUMtype arc_enumerator;
  LAB_ENUMtype input_enumerator;
  IO_SEQptr in_seq = APPLY_in_seq(cntxt);

  arc_enumerator.vector = APPLY_arc_vector(cntxt);
  arc_enumerator.pos = 0;

  input_enumerator.lab_vector = IO_SEQ_io_vector(in_seq);
  input_enumerator.pos = 0;
  IO_SEQ_io_pos(in_seq) = -1;

  while (!next_vector_element((void **)&arc, &arc_enumerator))
    {
      arc_label = ARC_label(arc);
      arc_sym = in_fn(ARC_label(arc));
      
      if (EPSILON_MATCH(arc_sym))
	in_sym = EPSILON;
      else if (flag_diacritic_p(arc_sym))
	{
	  if (APPLY_obey_flags_p(cntxt) &&
	      APPLY_out_side(cntxt) != BOTH_SIDES)
	    in_sym = EPSILON;
	  else
	    in_sym = arc_sym;
	}
      else
	{
	  next_lab_vector_element(&in_sym, &input_enumerator);
	  IO_SEQ_io_pos(in_seq)++;
	}
	  
      output_fn(out_fn(arc_label), in_sym, arc_label, cntxt);
    }
} /* standard_network_output */


/* Sub-routine to be called by store_in_out_seq(): on the model of
   standard_network_output(). It reads back the array of LOCATIONptr-s
   contained in APPLY_arc_vector. */
static void vectorized_network_output(APPLYptr cntxt, void (*output_fn)(id_type, id_type, id_type, APPLYptr))
{
  /* Output routine for apply_vectorized_network(). */

  id_type in_sym, arc_label, arc_sym;
  LOCATIONptr location;
  VECT_ENUMtype loc_enumerator;
  LAB_ENUMtype input_enumerator;
  IO_SEQptr in_seq = APPLY_in_seq(cntxt);

  loc_enumerator.vector = APPLY_arc_vector(cntxt);
  loc_enumerator.pos = 0;

  input_enumerator.lab_vector = IO_SEQ_io_vector(in_seq);
  input_enumerator.pos = 0;
  IO_SEQ_io_pos(in_seq) = -1;

  while (!next_vector_element((void **)&location, &loc_enumerator))
    {
        arc_label = LOCATION_id(location);
        
        arc_sym = INPUT_MATCH[arc_label];
      
        if (EPSILON_MATCH(arc_sym))
            in_sym = EPSILON;
        else if (flag_diacritic_p(arc_sym))
	{
            if (APPLY_obey_flags_p(cntxt) &&
                APPLY_out_side(cntxt) != BOTH_SIDES)
                in_sym = EPSILON;
            else
                in_sym = arc_sym;
	}
        else
	{
            next_lab_vector_element(&in_sym, &input_enumerator);
            IO_SEQ_io_pos(in_seq)++;
	}
	  
        output_fn(OUTPUT_MATCH[arc_label], in_sym, arc_label, cntxt);
    }
} /* vectorized_network_output */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
static int not_an_html_tag_p(id_type id)
{
  /* Returns true if the name of id is not of the form <XYZ>. */

  FAT_STR first, last;
  int len;
  first = id_to_atom_name(id);
  len = fat_strlen(first);

  last = (first+(len-1));

  return(!(len > 2 && CHARNUM(first) == '<' && CHARNUM(last) == '>'));

}


static void simple_network_output(APPLYptr cntxt)
{
  /* 
     Output routine for apply_patterns(). The main simplification is
     that we assume that the the pattern network does not contain
     one-sided OTHERs, only identity OTHERs. Another difference
     with respec to standard_network_output() is that this functions
     assumes that the path is stored as a sequence of label IDs,
     not arcs as in standard_network_output(). Since the pattern network
     may contain vectorized states, it is not possible to store the
     path as sequence of arcs. Maybe standard_network_output()
     should be changed to store the path as a label sequence.
   */

  id_type (*out_fn)(id_type) = APPLY_out_fn(cntxt);
  STRING_BUFFERptr out_buf =  APPLY_out_buffer(cntxt);
  LAB_ENUMtype output_enumerator;
  id_type arc_label, out_id;

  output_enumerator.lab_vector = APPLY_out_vector(cntxt);
  output_enumerator.pos = 0;

  /* If the interface variable 'mark-patterns' is off, we don't
     print tags. */

  while (!next_lab_vector_element(&arc_label, &output_enumerator))
    {
      out_id = out_fn(arc_label);

      if (!(out_id == EPSILON || out_id == ALTCHAIN_SYMBOL))
	{
	  if (APPLY_mark_patterns_p(cntxt))
	    symbol_name_to_buffer(out_id, out_buf);
	  else if (not_an_html_tag_p(out_id))
	    symbol_name_to_buffer(out_id, out_buf);
	}
    }

  /* If we are not marking patterns in any other way, print a newline. */
  if (!APPLY_mark_patterns_p(cntxt))
    append_char_to_buffer('\n', out_buf);
}
#endif /* SLIM_FST */

/* Output routine to be called in apply_mixed_network():
   this output routine is dedicated to Karttunen compacted nets. It converts
   the solution produced as an arc_vector into a sequence of label id-s. As
   the format of the net is compacted, we can't use the already existing
   function standard_network_output().
*/
static void compact_network_output(APPLYptr cntxt, void (*output_fn)(id_type, id_type, id_type, APPLYptr))
{
  /* Output routine for apply_mixed_network() dedicated to
     KARTTUNEN COMPACTED nets. */

  NETptr net= APPLY_net1(cntxt);
  id_type *RECODE_KEY = ALPH_items(NET_recode_key(net));
  id_type in_sym, arc_label, arc_sym;
  void* arc_location;
  VECT_ENUMtype arc_enumerator;
  LAB_ENUMtype input_enumerator;
  IO_SEQptr in_seq = APPLY_in_seq(cntxt);

  arc_enumerator.vector = APPLY_arc_vector(cntxt);
  arc_enumerator.pos = 0;

  input_enumerator.lab_vector = IO_SEQ_io_vector(in_seq);
  input_enumerator.pos = 0;
  IO_SEQ_io_pos(in_seq) = -1;

  while (!next_vector_element(&arc_location, &arc_enumerator))
    {
        unsigned char *loc= (unsigned char*) arc_location;
        unsigned char byte = *loc++;
 
        /* Get label id. */
        arc_label = byte & first_label_mask;
  
        while (more_to_come_p(byte))
        {
            byte = *loc++;
            arc_label = (arc_label << 7) | (byte & next_data_mask);
        }

        arc_label= RECODE_KEY[arc_label];
        arc_sym = INPUT_MATCH[arc_label];

      
      if (EPSILON_MATCH(arc_sym))
	in_sym = EPSILON;
      else if (flag_diacritic_p(arc_sym))
	{
	  if (APPLY_obey_flags_p(cntxt))
	    in_sym = EPSILON;
	  else
	    in_sym = arc_sym;
	}
      else
	{
	  next_lab_vector_element(&in_sym, &input_enumerator);
	  IO_SEQ_io_pos(in_seq)++;
	}
	  
      output_fn(OUTPUT_MATCH[arc_label], in_sym, arc_label, cntxt);
    }
} /* compact_network_output() */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */


static void reduced_net_output(APPLYptr cntxt, void (*output_fn)(id_type, id_type, id_type, APPLYptr))
{
  /* Output routine for apply_reduced_network(). */

  int i, j;
  id_type arc_label, in_sym, arc_sym;
  id_type (*in_fn)(id_type) = APPLY_in_fn(cntxt);
  id_type (*out_fn)(id_type) = APPLY_out_fn(cntxt);
  LAB_VECTORptr in_vector = APPLY_in_vect(cntxt);
  LAB_VECTORptr out_vector = APPLY_out_vector(cntxt);

  for (i = 0, j = 0; i < LAB_VECTOR_pos(out_vector); i++)
    {
      lab_vector_element_at(&arc_label, i, out_vector);
      arc_sym = in_fn(arc_label);

      if (EPSILON_MATCH(arc_sym) || flag_diacritic_p(arc_sym))
	in_sym = EPSILON;
      else
	{
	  lab_vector_element_at(&in_sym, j, in_vector);
	  j++;
	}
      output_fn(out_fn(arc_label), in_sym, arc_label, cntxt);
    }
}

static void bimachine_output(APPLYptr cntxt, void (*output_fn)(id_type, id_type, id_type, APPLYptr))
{
  /* Output routine for apply_bimachine().
   Note: using APPLY_in_vect(cntxt) because in_vector may not be
   in sync with out_vector -- need to know where the EPSILONs are
   placed in the input sequence in order to correctly interpret OTHER.
  */

  LAB_VECTORptr in_vect = APPLY_in_vect(cntxt);
  LAB_VECTORptr out_vector = APPLY_out_vector(cntxt);

  id_type in_sym, out_sym;
  int32 i, j= 0;

  for (i = LAB_VECTOR_pos(out_vector) - 1; i >= 0; i--, j++)
    {
      lab_vector_element_at(&in_sym, j, in_vect);
      lab_vector_element_at(&out_sym, i, out_vector);

      output_fn(out_sym, in_sym, in_sym, cntxt);
    }
}
#endif /* SLIM_FST */

void store_in_out_seq(void *cntxt)
{
#ifndef SLIM_FST    
  if (NET_left_bimachine(APPLY_net1((APPLYptr) cntxt)) &&
      APPLY_net2((APPLYptr) cntxt) &&
      NET_right_bimachine(APPLY_net2((APPLYptr) cntxt)))
    bimachine_output((APPLYptr) cntxt, out_sym_to_out_seq);
  else if (NET_reduced_labelset(APPLY_net1((APPLYptr) cntxt)))
    reduced_net_output((APPLYptr) cntxt, out_sym_to_out_seq);
  else if (NET_compacted(APPLY_net1((APPLYptr) cntxt)))
    compact_network_output((APPLYptr) cntxt, out_sym_to_out_seq);
  else if (has_arc_vectors_p(APPLY_net1((APPLYptr) cntxt)))
    vectorized_network_output((APPLYptr) cntxt, out_sym_to_out_seq);
  else
    standard_network_output((APPLYptr) cntxt, out_sym_to_out_seq);
#else
  if (NET_compacted(APPLY_net1((APPLYptr) cntxt)))
    compact_network_output((APPLYptr) cntxt, out_sym_to_out_seq);
  else if (has_arc_vectors_p(APPLY_net1((APPLYptr) cntxt)))
    vectorized_network_output((APPLYptr) cntxt, out_sym_to_out_seq);
  else
    standard_network_output((APPLYptr) cntxt, out_sym_to_out_seq);
#endif
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void write_to_buffer(void *cntxt)
{
  if (NET_left_bimachine(APPLY_net1((APPLYptr) cntxt)) &&
      APPLY_net2((APPLYptr) cntxt) &&
      NET_right_bimachine(APPLY_net2((APPLYptr) cntxt)))
    bimachine_output((APPLYptr) cntxt, out_sym_to_buffer);
  else if (NET_reduced_labelset(APPLY_net1((APPLYptr) cntxt)))
    reduced_net_output((APPLYptr) cntxt, out_sym_to_buffer);
  else if (NET_compacted(APPLY_net1((APPLYptr) cntxt)))
    compact_network_output((APPLYptr) cntxt, out_sym_to_buffer);
  else if (has_arc_vectors_p(APPLY_net1((APPLYptr) cntxt)))
      vectorized_network_output((APPLYptr) cntxt, out_sym_to_buffer);
  else
    standard_network_output((APPLYptr) cntxt, out_sym_to_buffer);

  append_char_to_buffer('\n', APPLY_out_buffer((APPLYptr) cntxt));
}

void write_to_out_stream(void *cntxt)
{
  if (NET_left_bimachine(APPLY_net1((APPLYptr) cntxt)) &&
      APPLY_net2((APPLYptr) cntxt) &&
      NET_right_bimachine(APPLY_net2((APPLYptr) cntxt)))
    bimachine_output((APPLYptr) cntxt, out_sym_to_out_stream);
  else if (NET_reduced_labelset(APPLY_net1((APPLYptr) cntxt)))
    reduced_net_output((APPLYptr) cntxt, out_sym_to_out_stream);
  else if (NET_compacted(APPLY_net1((APPLYptr) cntxt)))
    compact_network_output((APPLYptr) cntxt, out_sym_to_out_stream);
  else
    standard_network_output((APPLYptr) cntxt, out_sym_to_out_stream);
}
#endif /* SLIM_FST */

void store_in_table(void *cntxt)
{
/*
  IO_SEQ_TABLEptr table;   --- for debugging
  int pos;
  IO_SEQptr out_seq;
*/
  assure_io_sequence_table_space(1, APPLY_output_table((APPLYptr) cntxt));
  
#ifndef SLIM_FST    
  if (NET_left_bimachine(APPLY_net1((APPLYptr) cntxt)) &&
      APPLY_net2((APPLYptr) cntxt) &&
      NET_right_bimachine(APPLY_net2((APPLYptr) cntxt)))
    bimachine_output((APPLYptr) cntxt, out_sym_to_out_table);
  else if (NET_reduced_labelset(APPLY_net1((APPLYptr) cntxt)))
    reduced_net_output((APPLYptr) cntxt, out_sym_to_out_table);
  else if (NET_compacted(APPLY_net1((APPLYptr) cntxt)))
#else
  if (NET_compacted(APPLY_net1((APPLYptr) cntxt)))
#endif 
    compact_network_output((APPLYptr) cntxt, out_sym_to_out_table);
  else if (has_arc_vectors_p(APPLY_net1((APPLYptr) cntxt)))
    vectorized_network_output((APPLYptr) cntxt, out_sym_to_out_table);
  else
    standard_network_output((APPLYptr) cntxt, out_sym_to_out_table);

/*
  table = APPLY_output_table((APPLYptr) cntxt);
  pos = IO_SEQ_TABLE_pos(table);
  out_seq = IO_SEQ_TABLE_array(table)[pos];
  print_lab_vector(IO_SEQ_io_vector(out_seq), stdout, FALSE);
  fsm_fprintf(stdout, "\n");
  print_lab_vector(IO_SEQ_other_than_vector(out_seq), stdout, FALSE);
  fsm_fprintf(stdout, "\n");
  fsm_fprintf(stdout, "\n");
*/
  IO_SEQ_TABLE_pos(APPLY_output_table((APPLYptr) cntxt))++;
}
/* Sub-routine called by the function standard_network_output_to_tree(),
   (and also by its "sisters" dedicated to the "bimachine", "reduced" 
   and "compacted" mode).
   It is used when the output of compose-apply is saved as a solution 
   tree rather than in a table.      */
static void add_to_solution_tree(id_type id, STATEptr *current_state)
{
    /* Find if in the arc_set of CURRENT_STATE, there's an arc labeled
       with ID. If so, set CURRENT_STATE to the destination state of
       this arc; otherwise, add to the arc_set a new arc labeled with
       ID, and set CURRENT_STATE to the destination of the new arc. */
    ARCptr arc_set, cur_arc;
    
     /* Find a match from the current_state. */
    arc_set= STATE_arc_set(*current_state);
      
    for (cur_arc= arc_set; cur_arc !=NULL; cur_arc= next_arc(cur_arc))
    {
        if (ARC_label(cur_arc) == id)
            break;
    }
      
    if (cur_arc != NULL)
    {
        /* match found =>
           move to destination state and wait for next
           arc_label_to_store.  */
        *current_state= ARC_destination(cur_arc);
    }
    else
    {
        /* No match found:
           a new arc labeled with arc_label_to_store must be added to
           current_state: */
        STATEptr new_state= alloc_state();
        ARCptr new_arc= arc_alloc(id, new_state);
        next_arc(new_arc)= arc_set;
        STATE_arc_set(*current_state)= new_arc;
          
        /* Move to new_state: */
        *current_state= new_state;       
    }  
} /* add_to_solution_tree */


/* Sub-routine called by the function standard_network_output_to_tree(),
   (and also by its "sisters" dedicated to the "bimachine", "reduced" and
   "compacted" modes).
   It is used when the output of compose-apply is saved as a solution tree
   rather than in a table.*/
static void out_sym_from_arc_label(id_type *arc_label_to_store, id_type out_sym,
                                   id_type in_sym, id_type arc_label, IO_SEQptr out_seq, APPLYptr cntxt)
{
  int output_side =  APPLY_out_side(cntxt);
  int obey_flags_p =  APPLY_obey_flags_p(cntxt);
  int side =  APPLY_side(cntxt);

  /* OUT_SYM is the output side of ARC_LABEL, IN_SYM is the input
     symbol that matched the input side of ARC_LABEL. If ARC_LABEL
     is OTHER, we store the input symbol.
     ARC_LABEL_TO_STORE is the id to be added to the solution tree.
     */

  if ( (output_side != BOTH_SIDES)
      || ((in_sym == out_sym) && !tuple_id_p(arc_label)) )
  {
      if (!(EPSILON_MATCH(out_sym) || 
            ( flag_diacritic_p(out_sym) && obey_flags_p)))
      {
          if (out_sym == OTHER)
              record_restrictions(in_sym, arc_label, out_seq, cntxt);

          if (arc_label == OTHER)
              *arc_label_to_store= in_sym;
          else
              *arc_label_to_store= out_sym;
      }
  }
  else
  { /* both sides cas: */
      if ((side == UPPER) || (side == LOWER))
          *arc_label_to_store= arc_label;
      else if (arc_label == OTHER)
          *arc_label_to_store= in_sym;
  }
} /* out_sym_from_arc_label */


/*
  Converts the result of the compose-apply into a solution_tree:
  the result is initially returned as an arc vector. It is converted into an
  id-path and added to the solution_tree pointed by the last apply_context
  in the cascade.
  - returns FALSE if the candidate solution was already there in the tree;
  - returns TRUE if the candidate solution is a newly added one.
  This function handles "standard" networks only, (i.e. non optimized nets).
  For other formats see the functions bimachine_output_to_tree(),
  reduced_net_output_to_tree() and compact_network_output_to_tree().
 */
static int standard_network_output_to_tree(APPLYptr cntxt)
{
    /* Output routine for the store_in_a_tree() function in STANDARD mode.*/

    STATEptr current_state= APPLY_solution_tree(cntxt);
    
    ARCptr arc;
    IO_SEQ_TABLEptr output_table = APPLY_output_table(cntxt);
    int pos = IO_SEQ_TABLE_pos(output_table);
    IO_SEQptr out_seq = IO_SEQ_TABLE_array(output_table)[pos];
    LAB_VECTORptr out_vector = IO_SEQ_io_vector(out_seq);
        
    id_type (*in_fn)(id_type) = APPLY_in_fn(cntxt);
    id_type (*out_fn)(id_type) = APPLY_out_fn(cntxt);
    id_type in_sym, arc_label, arc_sym, out_sym, arc_label_to_store;
    VECT_ENUMtype arc_enumerator;
    LAB_ENUMtype input_enumerator;
    IO_SEQptr in_seq = APPLY_in_seq(cntxt);
  
    arc_enumerator.vector = APPLY_arc_vector(cntxt);
    arc_enumerator.pos = 0;
    
    input_enumerator.lab_vector = IO_SEQ_io_vector(in_seq);
    input_enumerator.pos = 0;
    IO_SEQ_io_pos(in_seq) = -1;
  
    while (!next_vector_element((void **)&arc, &arc_enumerator))
    {
      arc_label = ARC_label(arc);
      arc_sym = in_fn(ARC_label(arc));
      
      if (EPSILON_MATCH(arc_sym))
	in_sym = EPSILON;
      else if (flag_diacritic_p(arc_sym))
	{
	  if (APPLY_obey_flags_p(cntxt))
	    in_sym = EPSILON;
	  else
	    in_sym = arc_sym;
	}
      else
	{
	  next_lab_vector_element(&in_sym, &input_enumerator);
	  IO_SEQ_io_pos(in_seq)++;
	}

      /* Finding the output symbol to be written in the solution tree: */

      out_sym= out_fn(arc_label);
      arc_label_to_store= ID_NO_SYMBOL;
      out_sym_from_arc_label(&arc_label_to_store, out_sym, in_sym,
                             arc_label, out_seq, cntxt);
      
      if (arc_label_to_store != ID_NO_SYMBOL)
      {
          /* Writing the output symbol to the solution tree: */
          add_to_solution_tree(arc_label_to_store, &current_state);
          
          /* Writing the output symbol to the output_table: */
          append_to_lab_vector(arc_label_to_store, out_vector);
      } /* if (arc_label_to_store != ID_NO_SYMBOL) */
      
    } /* while */

    /* Make the (new) arc chain be a branch of the solution_tree:
       it may however already be a branch of the tree; in this case the
       solution tree is unchanged.*/
    if (STATE_final(current_state) == TRUE)
    {
        /* The current solution was already there ! 
           By returning FALSE, the current written out_seq will be over-written
           next time, as IO_SEQ_TABLE_pos(output_table) won't be incremented.*/
        return(FALSE);
    }
    else
    {
        /* Add definitely the new solution by making the current state
           final: */
        STATE_final(current_state)= TRUE;
        return(TRUE);
    }
  
} /* standard_network_output_to_tree() */



/*
  Converts the result of the compose-apply into a solution_tree:
  the result is initially returned as an arc vector. It is converted into 
  an id-path and added to the solution_tree pointed by the last
  apply_context in the cascade.
  - returns FALSE if the candidate solution was already there in the tree;
  - returns TRUE if the candidate solution is a newly added one.
  This function handles bimachine networks only. For other net formats see
  the functions standard_network_output_to_tree(),
  reduced_net_output_to_tree() and compact_network_output_to_tree().  */
static int bimachine_output_to_tree(APPLYptr cntxt)
{

/* Output routine for storing the result of apply_bimachine() as a tree.
   Note: using APPLY_in_vect(cntxt) because in_vector may not be
   in sync with out_vector -- need to know where the EPSILONs are
   placed in the input sequence in order to correctly interpret OTHER.
  */

    STATEptr current_state= APPLY_solution_tree(cntxt);
    
    IO_SEQ_TABLEptr output_table = APPLY_output_table(cntxt);
    int pos = IO_SEQ_TABLE_pos(output_table);
    IO_SEQptr out_seq = IO_SEQ_TABLE_array(output_table)[pos];
    LAB_VECTORptr sym_vector = IO_SEQ_io_vector(out_seq);
    
  LAB_VECTORptr in_vect = APPLY_in_vect(cntxt);
  LAB_VECTORptr out_vector = APPLY_out_vector(cntxt);

  id_type in_sym, out_sym, arc_label_to_store;
  int32 i, j= 0;

  for (i = LAB_VECTOR_pos(out_vector) - 1; i >= 0; i--, j++)
    {
      lab_vector_element_at(&in_sym, j, in_vect);
      lab_vector_element_at(&out_sym, i, out_vector);

/*       output_fn(out_sym, in_sym, in_sym, cntxt); */
      arc_label_to_store= ID_NO_SYMBOL;
      out_sym_from_arc_label(&arc_label_to_store, out_sym, in_sym, in_sym,
                             out_seq, cntxt);
      
      if (arc_label_to_store != ID_NO_SYMBOL)
      {
          /* Writing the output symbol to the solution tree: */
          add_to_solution_tree(arc_label_to_store, &current_state);

          /* Writing the output symbol to the output_table: */
          append_to_lab_vector(arc_label_to_store, sym_vector);
      } /* if (arc_label_to_store != ID_NO_SYMBOL) */
      
    } /* for */

    /* Make the (new) arc chain be a branch of the solution_tree:
       it may however already be a branch of the tree; in this case the
       solution tree is unchanged.*/
    if (STATE_final(current_state) == TRUE)
    {
        /* The current solution was already there ! 
           By returning FALSE, the current written out_seq will be over-written
           next time, as IO_SEQ_TABLE_pos(output_table) won't be incremented.*/
        return(FALSE);
    }
    else
    {
        /* Add definitely the new solution by making the current state
           final: */
        STATE_final(current_state)= TRUE;
        return(TRUE);
    }
    
} /* bimachine_output_to_tree() */



/*
  Converts the result of the compose-apply into a solution_tree:
  the result is initially returned as an arc vector. It is converted into 
  an id-path and added to the solution_tree pointed by the last
  apply_context in the cascade.
  - returns FALSE if the candidate solution was already there in the tree;
  - returns TRUE if the candidate solution is a newly added one.
  This function handles labelSet reduced networks only. For other net
  formats see functions standard_network_output_to_tree(),
  bimachine_output_to_tree() and compact_network_output_to_tree(). */
static int reduced_net_output_to_tree(APPLYptr cntxt)
{
    /* Output routine for the store_in_a_tree() function in LABELSET
       REDUCED mode. */

    STATEptr current_state= APPLY_solution_tree(cntxt);
    
    IO_SEQ_TABLEptr output_table = APPLY_output_table(cntxt);
    int pos = IO_SEQ_TABLE_pos(output_table);
    IO_SEQptr out_seq = IO_SEQ_TABLE_array(output_table)[pos];
    LAB_VECTORptr sym_vector = IO_SEQ_io_vector(out_seq);
    
    int i, j;
    id_type arc_label, in_sym, arc_sym, out_sym, arc_label_to_store;
    id_type (*in_fn)(id_type) = APPLY_in_fn(cntxt);
    id_type (*out_fn)(id_type) = APPLY_out_fn(cntxt);
    LAB_VECTORptr in_vector = APPLY_in_vect(cntxt);
    LAB_VECTORptr out_vector = APPLY_out_vector(cntxt);

    for (i = 0, j = 0; i < LAB_VECTOR_pos(out_vector); i++)
    {
        lab_vector_element_at(&arc_label, i, out_vector);
        arc_sym = in_fn(arc_label);

        if (EPSILON_MATCH(arc_sym) || flag_diacritic_p(arc_sym))
            in_sym = EPSILON;
        else
	{
            lab_vector_element_at(&in_sym, j, in_vector);
            j++;
	}

        /* Finding the output symbol to be written in the solution tree: */

        out_sym= out_fn(arc_label);
        arc_label_to_store= ID_NO_SYMBOL;
        out_sym_from_arc_label(&arc_label_to_store, out_sym, in_sym,
                               arc_label, out_seq, cntxt);
      
        if (arc_label_to_store != ID_NO_SYMBOL)
        {
            /* Writing the output symbol to the solution tree: */
            add_to_solution_tree(arc_label_to_store, &current_state);

            /* Writing the output symbol to the output_table: */
            append_to_lab_vector(arc_label_to_store, sym_vector);
        } /* if (arc_label_to_store != ID_NO_SYMBOL) */
      
    } /* for */

    /* Make the (new) arc chain be a branch of the solution_tree:
       it may however already be a branch of the tree; in this case the
       solution tree is unchanged.*/
    if (STATE_final(current_state) == TRUE)
    {
        /* The current solution was already there !
           By returning FALSE, the current written out_seq will be over-written
           next time, as IO_SEQ_TABLE_pos(output_table) won't be incremented.*/
        return(FALSE);
    }
    else
    {
        /* Add definitely the new solution by making the current state
           final: */
        STATE_final(current_state)= TRUE;
        return(TRUE);
    }
  
} /* reduced_net_output_to_tree() */


/*
  Converts the result of the compose-apply into a solution_tree:
  the result is initially returned as an arc vector. It is converted into an
  id-path and added to the solution_tree pointed by the last apply_context
  in the cascade.
  - returns FALSE if the candidate solution was already there in the tree;
  - returns TRUE if the candidate solution is a newly added one.
  This function handles Karttunen compacted networks only. For other net
  formats see the functions standard_network_output_to_tree(),
  bimachine_output_to_tree() and reduced_net_output_to_tree().
 */
static int compact_network_output_to_tree(APPLYptr cntxt)
{
    /* Output routine for the store_in_a_tree() function in COMPACT mode. */

    STATEptr current_state= APPLY_solution_tree(cntxt);
    
    IO_SEQ_TABLEptr output_table = APPLY_output_table(cntxt);
    int pos = IO_SEQ_TABLE_pos(output_table);
    IO_SEQptr out_seq = IO_SEQ_TABLE_array(output_table)[pos];
    LAB_VECTORptr out_vector = IO_SEQ_io_vector(out_seq);

    NETptr net= APPLY_net1(cntxt);
    id_type *RECODE_KEY = ALPH_items(NET_recode_key(net));
    id_type (*in_fn)(id_type) = APPLY_in_fn(cntxt);
    id_type (*out_fn)(id_type) = APPLY_out_fn(cntxt);
    id_type in_sym, arc_label, arc_sym, out_sym, arc_label_to_store;
    void* arc_location;
    VECT_ENUMtype arc_enumerator;
    LAB_ENUMtype input_enumerator;
    IO_SEQptr in_seq = APPLY_in_seq(cntxt);

    arc_enumerator.vector = APPLY_arc_vector(cntxt);
    arc_enumerator.pos = 0;

    input_enumerator.lab_vector = IO_SEQ_io_vector(in_seq);
    input_enumerator.pos = 0;
    IO_SEQ_io_pos(in_seq) = -1;

    
    while (!next_vector_element(&arc_location, &arc_enumerator))
    {
        unsigned char *loc= (unsigned char*) arc_location;
        unsigned char byte = *loc++;
 
        /* Get label id. */
  
        arc_label = byte & first_label_mask;
  
        while (more_to_come_p(byte))
        {
            byte = *loc++;
            arc_label = (arc_label << 7) | (byte & next_data_mask);
        }

        arc_label= RECODE_KEY[arc_label];
        arc_sym = in_fn(arc_label);

      
      if (EPSILON_MATCH(arc_sym))
	in_sym = EPSILON;
      else if (flag_diacritic_p(arc_sym))
	{
	  if (APPLY_obey_flags_p(cntxt))
	    in_sym = EPSILON;
	  else
	    in_sym = arc_sym;
	}
      else
	{
	  next_lab_vector_element(&in_sym, &input_enumerator);
	  IO_SEQ_io_pos(in_seq)++;
	}

      /* Finding the output symbol to be written in the solution tree: */
      out_sym= out_fn(arc_label);
      
      arc_label_to_store= ID_NO_SYMBOL;
      out_sym_from_arc_label(&arc_label_to_store, out_sym, in_sym,
                             arc_label, out_seq, cntxt);
      
      if (arc_label_to_store != ID_NO_SYMBOL)
      {
          /* Writing the output symbol to the solution tree: */
          add_to_solution_tree(arc_label_to_store, &current_state);

          /* Writing the output symbol to the output_table: */
          append_to_lab_vector(arc_label_to_store, out_vector);
      } 
      
    } /* while */

    /* Make the (new) arc chain be a branch of the solution_tree:
       it may however already be a branch of the tree; in this case the
       solution tree is unchanged.*/
    if (STATE_final(current_state) == TRUE)
    {
        /* The current solution was already there !
           By returning FALSE, the current written out_seq will be over-written
           next time, as IO_SEQ_TABLE_pos(output_table) won't be incremented.*/
        return(FALSE);
    }
    else
    {
        /* Add definitely the new solution by making the current state
           final: */
        STATE_final(current_state)= TRUE;
        return(TRUE);
    }
    
} /* compact_network_output_to_tree() */


/* Converts the result of the compose-apply into a solution_tree:
   the result is initially returned as an arc vector. It is converted
   into an id-path and added to the solution_tree pointed by the last
   apply_context in the cascade.
   - returns FALSE if the candidate solution was already there in the tree;
   - returns TRUE if the candidate solution is a newly added one.
   This function handles vectorized or standard networks only. For
   other net formats see the functions compact_network_output_to_tree(),
   bimachine_output_to_tree() and reduced_net_output_to_tree(). */
static int vectorized_network_output_to_tree(APPLYptr cntxt)
{
    /* Output routine for the store_in_a_tree() function in VECTORIZED
       mode. */

    STATEptr current_state= APPLY_solution_tree(cntxt);
    
    IO_SEQ_TABLEptr output_table = APPLY_output_table(cntxt);
    int pos = IO_SEQ_TABLE_pos(output_table);
    IO_SEQptr out_seq = IO_SEQ_TABLE_array(output_table)[pos];
    LAB_VECTORptr out_vector = IO_SEQ_io_vector(out_seq);
    int out_pos = LAB_VECTOR_pos(out_vector);

    id_type in_sym, arc_label, arc_sym, out_sym, arc_label_to_store;
    LOCATIONptr location;
    VECT_ENUMtype loc_enumerator;
    LAB_ENUMtype input_enumerator;
    IO_SEQptr in_seq = APPLY_in_seq(cntxt);

    loc_enumerator.vector = APPLY_arc_vector(cntxt);
    loc_enumerator.pos = 0;

    input_enumerator.lab_vector = IO_SEQ_io_vector(in_seq);
    input_enumerator.pos = 0;
    IO_SEQ_io_pos(in_seq) = -1;

    while (!next_vector_element((void**) &location, &loc_enumerator))
    {
        arc_label = LOCATION_id(location);
        
        arc_sym = INPUT_MATCH[arc_label];

      if (EPSILON_MATCH(arc_sym))
	in_sym = EPSILON;
      else if (flag_diacritic_p(arc_sym))
	{
	  if (APPLY_obey_flags_p(cntxt))
	    in_sym = EPSILON;
	  else
	    in_sym = arc_sym;
	}
      else
	{
	  next_lab_vector_element(&in_sym, &input_enumerator);
	  IO_SEQ_io_pos(in_seq)++;
	}

      /* Finding the output symbol to be written in the solution tree: */
      out_sym= OUTPUT_MATCH[arc_label];
      
      arc_label_to_store= ID_NO_SYMBOL;
      out_sym_from_arc_label(&arc_label_to_store, out_sym, in_sym,
                             arc_label, out_seq, cntxt);
      
      if (arc_label_to_store != ID_NO_SYMBOL)
      {
          /* Writing the output symbol to the solution tree: */
          add_to_solution_tree(arc_label_to_store, &current_state);

          /* Writing the output symbol to the output_table: */
          if (out_pos >= LAB_VECTOR_length(out_vector))
          {
              append_to_lab_vector(arc_label_to_store, out_vector);
              out_pos= LAB_VECTOR_pos(out_vector);
          }
          else
          {
              LAB_VECTOR_array(out_vector)[out_pos]= arc_label_to_store;
              LAB_VECTOR_pos(out_vector)= ++out_pos;
          }
      } 
      
    } /* while */

    /* Make the (new) arc chain be a branch of the solution_tree:
       it may however already be a branch of the tree; in this case the
       solution tree is unchanged.*/
    if (STATE_final(current_state) == TRUE)
    {
        /* The current solution was already there !
         By returning FALSE, the current written out_seq will be over-written
         next time, as IO_SEQ_TABLE_pos(output_table) won't be incremented. */
        return(FALSE);
    }
    else
    {
        /* Add definitely the new solution by making the current state
           final: */
        STATE_final(current_state)= TRUE;
        return(TRUE);
    }
    
} /* vectorized_network_output_to_tree() */


/* Alternative to function store_in_table() for filtering the identical
   outputs of the LAST apply_context structure in the net cascade.
   This function is to be assigned to the output_fn field of the last
   context in the cascade. */
void store_in_a_tree(void *cntxt)
{
    /*
      Instead of storing systematically the output string of the last
      net in the cascade in an io_seq_table, we store it at the same
      time in a tree, always checking if it is not already in the tree
      before adding it.
      The solution tree is a kind of network that we always keep
      deterministic so that checking if a path is already there is quick
      and easy.
      If a candidate output string is added to the tree, we keep its
      recording in the io_seq_table by incrementing the IO_SEQ_TABLE_pos
      of the APPLY_output_table; otherwise we don't increment the pos
      so that this recording will be overwritten by the next output 
      string if any, (or just omitted at the printing step).

      Make sure when calling this function that the context "cntxt"
      passed as a parameter IS the LAST context in the cascade;
      otherwise the result is not guaranteed !!!     */      

    int new_solution_added= FALSE;
    
    /* We still need the output_table structure to store the information for
       interpreting symbol OTHER in case it'd part of the output:           */
    assure_io_sequence_table_space(1, APPLY_output_table((APPLYptr) cntxt));
  
  if (APPLY_solution_tree((APPLYptr) cntxt) == NULL)
  {
      /* The root of the solution tree has not yet been built:
         allocate a new state. */
      APPLY_solution_tree((APPLYptr) cntxt)= alloc_state();
  } 

  if (NET_left_bimachine(APPLY_net1((APPLYptr) cntxt)) &&
      APPLY_net2((APPLYptr) cntxt) &&
      NET_right_bimachine(APPLY_net2((APPLYptr) cntxt)))
    new_solution_added= bimachine_output_to_tree((APPLYptr) cntxt);
  else if (NET_reduced_labelset(APPLY_net1((APPLYptr) cntxt)))
    new_solution_added= reduced_net_output_to_tree((APPLYptr) cntxt);
  else if (NET_compacted(APPLY_net1((APPLYptr) cntxt)))
    new_solution_added= compact_network_output_to_tree((APPLYptr) cntxt);
  else if (has_arc_vectors_p(APPLY_net1((APPLYptr) cntxt)))
    new_solution_added= vectorized_network_output_to_tree((APPLYptr) cntxt);
  else
    new_solution_added= standard_network_output_to_tree((APPLYptr) cntxt);

  if (new_solution_added)
      /* Keep the recoding in the output_table by incrementing the pos. */
      IO_SEQ_TABLE_pos(APPLY_output_table((APPLYptr) cntxt))++;
  
} /* store_in_a_tree */


/* Cleaning of the solution tree:
   don't call this function directly; call rather reset_solution_tree() .
*/
static int free_solution_tree(STATEptr root_state)
{
    /* Frees all the tree starting at root, including the root state ! */
    
    ARCptr next_arc, arc;

    if (STATE_arc_set(root_state) == NULL)
    {
        /* The current state is a leaf:
           delete and backtrack. */
        free_state(root_state);
        return 0;
    }
    else
    {
        for (arc= STATE_arc_set(root_state); arc!= NULL; arc= next_arc)
        {
            free_solution_tree(ARC_destination(arc));
            next_arc= next_arc(arc);
            free_arc(arc);
        }
        /* The arc set of root_state is now empty; we can free root_state
           and backtrack: */
        free_state(root_state);
        return 0;
    } /* else */    
} /* free_solution_tree */



/* Reset of the solution_tree:
   It frees all the solution tree except its root state. This function 
   is to be called between 2 runs of the compose-apply with the same
   apply_context_vector; otherwise, the output strings coming from 
   different string lookups will be unexpectedly accumulated in the same
   tree...   */
void reset_solution_tree(APPLYptr apply_context)
{
    /* Frees the whole solution tree except its root state. */
    
    ARCptr arc= NULL;
    ARCptr next_arc= NULL;
    STATEptr root_state;

    root_state= APPLY_solution_tree(apply_context);
    
    for (arc= STATE_arc_set(root_state); arc!= NULL; arc= next_arc)
    {
        free_solution_tree(ARC_destination(arc));
        next_arc= next_arc(arc);
        free_arc(arc);
    }
    
    STATE_arc_set(root_state)= NULL;
} /* reset_solution_tree */


/* Printing of the solution tree:
   It is to be used in place of the function print_io_seq_table() when the
   output of the compose-apply algorithm has been stored as a tree instead of
   the usual table. It prints recursively all the paths (and sub-paths!)
   contained in the tree.
 */
int print_solution_tree(STATEptr solution_tree, LAB_VECTORptr buffer, FILE *stream, int escape_p)
{
    /* Prints to the stream each of the paths (or sub-paths!) contained in the
       solution tree. */
    ARCptr arc;
    
    if (STATE_final(solution_tree))
    {
        /* Path found: print the content of the buffer. */
        int pos;
        for (pos=0; pos < LAB_VECTOR_pos(buffer); pos++)
        {
            print_label(LAB_VECTOR_array(buffer)[pos], stream, escape_p);
        }

        fsm_fprintf(stream, "\n");
        return(0);
    }
    else
    {
        if (STATE_arc_set(solution_tree)== NULL)
        {
            fsm_fprintf(stderr, "Error in solution tree: non final leaf state detected... ");
            return(1);
        }
        else
        {
            for (arc= STATE_arc_set(solution_tree); arc!= NULL; arc= next_arc(arc))
            {
                int current_position= LAB_VECTOR_pos(buffer);
        
                append_to_lab_vector(ARC_label(arc), buffer);
                print_solution_tree(ARC_destination(arc), buffer, stream, escape_p);
                LAB_VECTOR_pos(buffer)= current_position;
            }
            return(0);
        }
    } /* else */
} /* print_solution_tree */

/*************          APPLY_STANDARD_NETWORK          ***************/

/* Sub-routine for the function apply_standard_network():
 works for standard nets only => use the other sub-routnes for other net
 formats. */
static int has_epsilon_loop(VECTORptr arc_stack, id_type (*in_fn)(id_type),
			    STATEptr start_state)
{
  int pos = VECTOR_pos(arc_stack) - 1;
  STATEptr dest;
  ARCptr arc;
  id_type id;

  if (pos < 0)
    return(FALSE);

  vector_element_at((void *) &arc, pos--, arc_stack);
  dest = ARC_destination(arc);
  id = in_fn(ARC_label(arc));
  
  if (dest == start_state && pos < 0 &&
      !STATE_final(start_state) &&
      (EPSILON_MATCH(id) ||
       flag_diacritic_p(id)))
    return(TRUE);

  while (pos > 0)
    {
      vector_element_at((void *) &arc, pos--, arc_stack);
      id = in_fn(ARC_label(arc));
      if (!(EPSILON_MATCH(id) || flag_diacritic_p(id)))
	return(FALSE);
	
      if (ARC_destination(arc) == dest)
	return(TRUE);
    }
  return(FALSE);
} /* has_epsilon_loop */


int apply_standard_network(APPLYptr apply_context)
{
  /* Applies the network in a depth-first manner to symbols of the
     input string.
     */
      
  id_type (*in_fn)(id_type) = APPLY_in_fn(apply_context);
  ALPHABETptr flag_register = APPLY_flag_register(apply_context);
  VECTORptr arc_vector = APPLY_arc_vector(apply_context);
  LAB_VECTORptr in_vector = APPLY_in_vect(apply_context);
  LAB_VECTORptr flag_vector = APPLY_flag_vector(apply_context);
  int flags_p = APPLY_flags_p(apply_context);
  STATEptr state, start_state = NET_start_state(APPLY_net1(apply_context));

  ARCptr arc;
  id_type in_sym, arc_sym, flag_attr = 0;
  int result_count = 0;
  int epsilon_loop_p = FALSE;
  int parse_error= NO_ERROR; /* error code for parsing the input string */

  /* print_io_seq(APPLY_in_seq(apply_context), stdout); */
  
  /* Make sure that sigma is in the expected format. */
  label_to_binary(APPLY_sigma(apply_context));

  state = start_state;

  while (state)
    {
      do {
          parse_error= fetch_input_symbol(&in_sym, apply_context);
	 
          /* if (flag_diacritic_p(in_sym))
             ...
             Should we allow flag diacritics in the input?
             If so, what should we do here?  */
      } while (in_sym != ID_NO_SYMBOL && flag_diacritic_p(in_sym));

      if (parse_error == ERROR)
          break;
              
      if (in_sym == ID_NO_SYMBOL && virtual_final_p(state))
	{
	  APPLY_output_fn(apply_context)(apply_context);
	  result_count++;
	}

      /* If state is an ordinary state, virtual_arc_set(state)
	 is equivalent to STATE_arc_set(state). If the state
	 is an ordinary state, virtual_arc_set(state) computes
	 a real arcset for it. L.K. 2001-02-02.
	 arc = STATE_arc_set(state);
      */

      arc = virtual_arc_set(state);

      state = NULL;

      do {
	while(arc) {

	  arc_sym = in_fn(ARC_label(arc));

	  flag_attr = flag_diacritic_attrib(arc_sym);

	  if (flags_p)
	    {
	      /* Save current value or just increment the
		 current position if there is no value to save. */

	      if (flag_attr)
		append_to_lab_vector(flag_value(flag_register, flag_attr),
				     flag_vector);
	      else
		increment_lab_vector(flag_vector);
	    }
  
	  if (EPSILON_MATCH(arc_sym) ||
	      (flag_attr &&
	       (!flags_p ||
		flag_action(flag_register, flag_diacritic(arc_sym)))))
	    {
	      /* Epsilon match or flag diacritic. Avoid
		 epsilon loops. */
	      /*
		This condition is an error. Need to append regardless
		of whether this is an EPSILON arc or not. Otherwise
		backtracking will not work correctly.  2000-06-19 LK.
	      if (!EPSILON_MATCH(ARC_label(arc)))
		*/
	      append_to_vector((void*) arc, arc_vector);

	      if (has_epsilon_loop(arc_vector, in_fn, start_state))
		{
		  epsilon_loop_p = TRUE;
		  arc = next_arc(arc);
		  decrement_vector(arc_vector);

		  /* If an epsilon loop has been detected,
		     there is no reason to discard the
		     current input symbol. Don't understand
		     why this clause is here. In any case,
		     it seems to be the source of a bug
		     reported by Ron. The bug goes back a
		     long way, to January 2000 at least.
		     2001-09-26  LK

		  if (APPLY_next_symbol_fn(apply_context)
		      != symbol_from_in_vector)
		    decrement_lab_vector(in_vect);
		    */

		}
	      else
		{
		  /* Flag diacritic in the input! */

		  if (flag_attr && arc_sym == in_sym)
		    APPLY_in_pos(apply_context)++;

		  state = ARC_destination(arc);
		  break;
		}
	    }
	  else if ((arc_sym == in_sym && !flag_attr)
		    || OTHER_MATCH(in_sym, arc_sym, apply_context)
		    ||(!flag_attr &&
		       ANY_MATCH(in_sym, arc_sym, apply_context)))
	    {
	      /* Matching a real symbol. */

	      append_to_vector((void *) arc, arc_vector);
	      state = ARC_destination(arc);
	      APPLY_in_pos(apply_context)++;
	      break;
	    }
	  else
	    {
	      /* No match. Continue with the next arc. If the
		 arc was a flag diacritic, discard the saved
		 value. */
	  
	      if (flags_p)
		decrement_lab_vector(flag_vector);

	      arc = next_arc(arc);
	    }
	}
    
	if (state)
	  break;

	/* Back up the arc vector until we find an arc that
	   has a next arc. Decrease input position at every
	   step unless the input side of the label is epsilon
	   or a flag diacritic not matching itself. */
	  
	while(!subtract_from_vector((void*) &arc, arc_vector))
	  {
	    arc_sym = in_fn(ARC_label(arc));
	    flag_attr = flag_diacritic_attrib(arc_sym);

	    if (flags_p)
	      {
		/* Restore the old value of the flag */

		if (flag_attr)
		  subtract_from_lab_vector(&flag_value(flag_register,
						       flag_attr),
					   flag_vector);
		else
		  decrement_lab_vector(flag_vector);
	      }

	    if (flag_attr)
	      {
		/* A flag symbol that matches itself is treated like an
		   ordinary input symbol except that we also check for
		   the constraints. */
		    
		if (APPLY_in_pos(apply_context) > 0 &&
		    arc_sym ==
		    LAB_VECTOR_array(in_vector)[APPLY_in_pos(apply_context)-1])
		  APPLY_in_pos(apply_context)--;
	      }
	    else if (!EPSILON_MATCH(arc_sym))
	      APPLY_in_pos(apply_context)--;
		
	    arc = next_arc(arc);
	  
	    if (arc)
	      {
		/* Found the arc, now get the symbol at the current
		   depth. */
		lab_vector_element_at(&in_sym, APPLY_in_pos(apply_context),
				      in_vector);
		break;
	      }
	  }
      } while(arc);
    }

  if (epsilon_loop_p)
    fsm_fprintf(stdout, "Network has an epsilon loop on the input side.\n");

  return(result_count);
} /* apply_standard_network */


/************* MIXED VERSION OF APPLY_STANDARD_NETWORK ***************/


/* Sub-routine for the apply_mixed_network():
   it can handle Karttunen compacted networks as well as standard nets.
   To this end it makes call to some of the traversal call-back functions
   for accessing the net's arc and state locations. */
static int has_epsilon_loop_mixed(NETptr net, VECTORptr arc_stack,
				  void *start_state_location,
                                  id_type (*in_fn)(id_type),
                                  void* (*destination_from_arc)(NETptr, void **),
                                  id_type (*label_from_arc_location)(NETptr, void **, int *, int *))
{
  int pos = VECTOR_pos(arc_stack) - 1;
  void* current_dest_location; /* destination of the candidate epsilon arc */
  void* current_arc_location;   /* address of the current epsilon arc */
  void* preceding_dest_location; /* destination of the preceding arc in arc_stack. */
  void* preceding_arc_location; /* preceding arc in arc_stack. */
  id_type preceding_id, preceding_label;
  id_type current_id, current_label;
  int final, current_final, last_arc;

  /* If the arc_stack is empty, just return. */
  if (pos < 0)
    return(FALSE);

  /* Pop the current arc_location: */
  vector_element_at(&current_arc_location, pos--, arc_stack);
  
  /* Read the label first:
   (don't remove this step and do it before reading the arc destination
   because this step is absolutely necessary for moving forward in
   compacted nets.*/
  current_label= label_from_arc_location(net, &current_arc_location,
                                         &current_final, &last_arc);
  current_id = in_fn(current_label);

  /* Get the current arc destination: */
  current_dest_location= destination_from_arc(net, &current_arc_location);

  /* Now check for complex (indirect) epsilon-loops by backtracking in
     the arc_stack:
     Note that the first preceding arc destination is ALSO the origin state
     of the candidate epsilon arc. We use it to first discard simple direct
     epsilon-loops: i.e. origin == destination.
     If there's no preceding arc (pos<0) it means that we are still on
     the start state of the network: then we check that the current eps
     transition is not looping on "start_state_location".*/ 
  while (pos >= 0)
  {
      /* Pop the preceding arc in arc_stack: */
      vector_element_at(&preceding_arc_location, pos--, arc_stack);
          
      /* Read the label first:
         (don't remove this step and do it before reading the arc destination
         because this step is absolutely necessary for moving forward in
         compacted nets.*/
      preceding_label=label_from_arc_location(net, &preceding_arc_location,
                                              &final, &last_arc);
        
      preceding_id = in_fn(preceding_label);

      /* Get the arc destination */
      preceding_dest_location= destination_from_arc(net, &preceding_arc_location);

      if (preceding_dest_location == current_dest_location)
          return(TRUE); /* Complex or suimple epsilon loop detected. */
      else if (!(EPSILON_MATCH(preceding_id) || flag_diacritic_p(preceding_id)))
          /* The preceding arc is not an epsilon arc: we can stop here
             as we know that any possible loop would contain at least this
             non-epsilon symbol. */
          return(FALSE);
  } /* while */

  /* A this stage all the arc_stack has been checked and the test proved
     that it is entirely made of epsilon arcs. Thus you need to check
     that the candidate eps transition is not looping on the
     "start_state_location". */
  if ((current_dest_location == start_state_location)/*&& !current_final*/)
  {
      /* Complex epsilon loop on the start state detected. */
      return(TRUE);
  }
  else
      return(FALSE);
} /* has_epsilon_loop_mixed */


/* New version of the original apply_standard_network():
   it is built on the same model but it can handle "standard" networks as
   well as Karttunen compacted networks. To this end it uses traversal
   call-back routines for accessing to the net's state and arc locations,
   and to retrieve the label ids.
   The apply_context fields storing pointers to the appropriate traversal
   routines must be initialized with the functions declared in the file
   "net-traversal.c". Currently, this intialization is made in the function
   apply_network(). */
int apply_mixed_network(APPLYptr apply_context)
{
  /* Applies the network in a depth-first manner to symbols of the
     input string. */
    
 /* Retrieve the traversal call-back routines:
    ------------------------------------------ */
    id_type (*in_fn)(id_type) = APPLY_in_fn(apply_context);
      
    void* (*start_state_function)(NETptr, void**, int*)=  APPLY_start_state_fn(apply_context);
    
    id_type (*label_from_arc_location)(NETptr, void**, int*, int*)= APPLY_label_from_arc_fn(apply_context);
    
    void (*next_arc_location)(NETptr, void**, int)= APPLY_next_arc_fn(apply_context);
    
    void* (*destination_from_arc)(NETptr, void **)= APPLY_destination_fn(apply_context);

  
  ALPHABETptr flag_register = APPLY_flag_register(apply_context);

  /* Get the arc vector: (already allocated in the apply_context)
     it registers the path and allows backtracking. */
  VECTORptr arc_location_vector = APPLY_arc_vector(apply_context);

  
  LAB_VECTORptr in_vector = APPLY_in_vect(apply_context);
  LAB_VECTORptr flag_vector = APPLY_flag_vector(apply_context);
  int flags_p = APPLY_flags_p(apply_context);
  
  id_type in_sym, arc_sym, flag_attr = 0;
  int result_count = 0;
  
  /* Boolean variables: */
  int epsilon_loops_detected_p = FALSE;
  int continuation_found = TRUE;

  /* New variables:
     they are not in the original apply_standard_network() function but had to
     be introduced for the mixed version: */
  id_type arc_label;
  void *start_state_location;
  void*  state_location= NULL;
  int final= FALSE;
  int last_arc= FALSE;
  void *arc_location, *saved_arc_location;
  NETptr net= APPLY_net1(apply_context);
  int parse_error= NO_ERROR; /* error code for parsing the input string */
  /* End of new variables. */
  
  /* state = start_state; In original apply_standard_network() */
  state_location = start_state_function(net, &arc_location, &final);  
  start_state_location = state_location;

  while (continuation_found)
    {
        /* Look for the next symbol in the input string: */
        do {
            parse_error= fetch_input_symbol(&in_sym, apply_context);
        } while (in_sym != ID_NO_SYMBOL && flag_diacritic_p(in_sym));

        if (parse_error == ERROR)
            break;
            
        if (in_sym == ID_NO_SYMBOL && final)
	{
            APPLY_output_fn(apply_context)(apply_context);
            result_count++;
	}

       /*  arc = STATE_arc_set(state); :In original standard version. */

        /* Prepare the search for a possible new destination state: */
        continuation_found= FALSE;
      
        do {
            while (arc_location != NULL)
            {

                /* append_to_vector(arc_location, arc_location_vector); */
                /* Save the current arc location in case we need to append it
                   to the arc_location_vector. */
                saved_arc_location= arc_location; 

                arc_label= label_from_arc_location(net, &arc_location, &final, &last_arc); 

                if (arc_label == ID_NO_SYMBOL)
                {
                    /* No arc at the "arc_location":
                       the state must be empty: we must backtrack to the preceding state. */
/*                 decrement_vector(arc_location_vector);  */
                    break;
                }
            
                arc_sym =  INPUT_MATCH[arc_label];

                flag_attr = flag_diacritic_attrib(arc_sym);

                if (flags_p)
                {
                    /* Save current value or just increment the
                       current position if there is no value to save. */

                    if (flag_attr)
                        append_to_lab_vector(flag_value(flag_register, flag_attr),
                                         flag_vector);
                    else
                        increment_lab_vector(flag_vector);
                }

                /* Test for epsilon matches: */
                /* ------------------------------------------- */
                if (EPSILON_MATCH(arc_sym) ||
                    (flag_attr &&
                     (!flags_p ||
                      flag_action(flag_register, flag_diacritic(arc_sym)))))
                {
                    /* Epsilon match or flag diacritic. Avoid
                       epsilon loops. */

                    /*
                      This condition is an error. Need to append regardless
                      of whether this is an EPSILON arc or not. Otherwise
                      backtracking will not work correctly.  2000-06-19 LK.
                      if (!EPSILON_MATCH(ARC_label(arc)))
                    */

                    /* Add the current arc location to the stack: */
                    append_to_vector(saved_arc_location, arc_location_vector);

                    if (has_epsilon_loop_mixed(net, arc_location_vector,
                                               start_state_location,
                                               in_fn,
                                               destination_from_arc,
                                               label_from_arc_location) )
                    {
                        /* An epsilon loop has been detected: */
                        epsilon_loops_detected_p = TRUE;

                        /* Skip the arc by decrementing the stack and jumping
                           to the next one: */
                        next_arc_location(net, &arc_location, last_arc); 
                        decrement_vector(arc_location_vector);
                        
                            /* If an epsilon loop has been detected,
                               there is no reason to discard the
                               current input symbol. Don't understand
                               why this clause is here. In any case,
                               it seems to be the source of a bug
                               reported by Ron. The bug goes back a
                               long way, to January 2000 at least.
                               2001-09-26  LK

                               if (APPLY_next_symbol_fn(apply_context)
                               != symbol_from_in_vector)
                               decrement_lab_vector(in_vector);
			    */
                        }
                        else
                        { /* Flag diacritic in the input! */
                        
                            if (flag_attr && arc_sym == in_sym)
                                APPLY_in_pos(apply_context)++;
                 
                            state_location = destination_from_arc(net, &arc_location);
                            continuation_found= TRUE;
                            break;
                        }
                }
                /* Test for  "real" matches: */
                /* ------------------------- */
                else if ((arc_sym == in_sym && !flag_attr)
                     || OTHER_MATCH(in_sym, arc_sym, apply_context)
                     ||(!flag_attr &&
                        ANY_MATCH(in_sym, arc_sym, apply_context)))
                {
                    /* Matching a real symbol. */
                    /* Push the arc onto the stack: */
                    append_to_vector(saved_arc_location, arc_location_vector);

                    state_location = destination_from_arc(net, &arc_location);
                    continuation_found= TRUE;
                    APPLY_in_pos(apply_context)++;
                    break;
                }
                else
                {
                    /* No match. Continue with the next arc. If the
                       arc was a flag diacritic, discard the saved
                       value. */
/*                 decrement_vector(arc_location_vector); */
                    if (flags_p)
                        decrement_lab_vector(flag_vector);

                    /* arc = next_arc(arc); :In original standard version. */
                    next_arc_location(net, &arc_location, last_arc);   
                }
            } /* while(arc_location != NULL) */
    
            if (continuation_found)
                break;

	/* Back up the arc vector until we find an arc that
	   has a next arc. Decrease input position at every
	   step unless the input side of the label is epsilon
	   or a flag diacritic not matching itself. */
	  
	while(!subtract_from_vector(&arc_location, arc_location_vector))
	  {
              arc_label= label_from_arc_location(net, &arc_location, &final, &last_arc); 

              arc_sym = in_fn(arc_label);
              flag_attr = flag_diacritic_attrib(arc_sym);

              if (flags_p)
	      {
                  /* Restore the old value of the flag */

                  if (flag_attr)
                      subtract_from_lab_vector(&flag_value(flag_register,
                                                           flag_attr),
                                               flag_vector);
                  else
                      decrement_lab_vector(flag_vector);
	      }

              if (flag_attr)
	      {
		/* A flag symbol that matches itself is treated like an
		   ordinary input symbol except that we also check for
		   the constraints. */
		    
		if (APPLY_in_pos(apply_context) > 0 &&
		    arc_sym ==
		    LAB_VECTOR_array(in_vector)[APPLY_in_pos(apply_context)-1])
		  APPLY_in_pos(apply_context)--;
	      }
	    else if (!EPSILON_MATCH(arc_sym))
	      APPLY_in_pos(apply_context)--;

            /* arc = next_arc(arc); :In original standard version. */
            next_arc_location(net, &arc_location, last_arc); 
	  
	    if (arc_location)
	      {
		/* Found the arc, now get the symbol at the current
		   depth. */
		lab_vector_element_at(&in_sym, APPLY_in_pos(apply_context),
				      in_vector);
		break;
	      }
	  } /* end of backtracking loop */
      } while(arc_location != NULL);
    } /* while (continuation_found) */

  if (epsilon_loops_detected_p)
    fsm_fprintf(stdout, "Network has an epsilon loop on the input side.\n");

  return(result_count);
} /* apply_mixed_network */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 
/********** VECTORIZED VERSION OF APPLY_STANDARD_NETWORK ************/

/* Set up or reset a location structure from a state object:
   (called also to re-initialize and re-use already existing
   location objects). */
static void set_location(LOCATIONptr location, STATEptr state)
{
    if (location == NULL)
    {
        fsm_fprintf(stderr, "No location to set up.\n");
    }
    else
    {
        LOCATION_eps_pos(location)= -1;
        LOCATION_match_pos(location)= -1;
        LOCATION_state(location)= state;
        LOCATION_arc(location)= NULL;
    }
}

/* Sub-routine for the apply_vectorized_network():
   it can handle vectorized networks as well as standard nets. */
static int has_epsilon_loop_vect(LOCATIONptr loc_heap, int heap_pos,
				  void *start_state_location)
{
  int pos = heap_pos - 1;
  
  STATEptr origin_state= NULL;

  /* destination of the candidate epsilon arc */
  STATEptr current_dest_state;

  /* destination of the preceding arc in loc_stack. */
  STATEptr preceding_dest_state; 
  LOCATIONptr preceding_location; /* preceding location in loc_stack. */
  LOCATIONptr current_location;   /* preceding location in loc_stack. */
  id_type preceding_id, preceding_label;
  id_type current_id, current_label;

  /* If the loc_stack is empty, just return. */
  if (pos < 0)
    return(FALSE);

  /* Pop the current arc_location: */
  current_location= &loc_heap[pos--];
  
  /* Read the label first: */
  current_label= LOCATION_id(current_location);

  current_id = INPUT_MATCH[current_label];

  origin_state= LOCATION_state(current_location);
  
  /* Get the current arc destination: */
  if (STATE_vector_p(origin_state))
  {
      /* Vectorized state: */
      current_dest_state= AV_dest(STATE_arc_vector(origin_state),
                                  LOCATION_id(current_location));
  }
  else
  {
      /* Standard state: */
      current_dest_state= ARC_destination(LOCATION_arc(current_location));
  }

  /* Now check for complex (indirect) epsilon-loops by backtracking in
     the loc_stack:
     Note that the first preceding arc destination is ALSO the origin 
     state of the candidate epsilon arc. We use it to first discard
     simple direct epsilon-loops: i.e. origin == destination.
     If there's no preceding arc (pos<0) it means that we are still on
     the start state of the network: then we check that the current eps
     transition is not looping on "start_state_location".*/ 
  while (pos >= 0)
  {
      /* Pop the preceding arc in loc_stack: */
      preceding_location= &loc_heap[pos--];
          
      /* Read the label first:  */
      preceding_label=LOCATION_id(preceding_location);
        
      preceding_id = INPUT_MATCH[preceding_label];

      origin_state= LOCATION_state(preceding_location);

      /* Get the arc destination */
      if (STATE_vector_p(origin_state))
      {
          /* Vectorized state: */
          preceding_dest_state= AV_dest(STATE_arc_vector(origin_state),
                                  LOCATION_id(preceding_location));
      }
      else
      {
          /* Standard state: */
          preceding_dest_state= ARC_destination(LOCATION_arc(preceding_location));
      }
      
      if (preceding_dest_state == current_dest_state)
          return(TRUE); /* Complex or suimple epsilon loop detected. */
      else if (!(EPSILON_MATCH(preceding_id) || flag_diacritic_p(preceding_id)))
          /* The preceding arc is not an epsilon arc: we can stop here
             as we know that any possible loop would contain at least this
             non-epsilon symbol. */
          return(FALSE);
  } /* while */

  /* A this stage all the loc_stack has been checked and the test proved
     that it is entirely made of epsilon arcs. Thus you need to check
     that the candidate eps transition is not looping on the
     "start_state_location". */
  if ((current_dest_state == start_state_location))
  {
      /* Complex epsilon loop on the start state detected. */
      return(TRUE);
  }
  else
      return(FALSE);
} /* has_epsilon_loop_vect */


/* Special version of the apply_standard_network() for vectorized nets:
   it is built on the same model but it can handle "vectorized" networks,
   as well as standard nets.
   It doesn't work for other formats, (Karttunen compacted networks,
   labelset reduced, ...)
   Currently, it is called from function apply_network().  */
int apply_vectorized_network(APPLYptr apply_context)
{
    /* Applies the network in a depth-first manner to symbols of the
       input string. */

    /* Retrieve data from the apply_context:
       ------------------------------------- */
    ALPHABETptr flag_register = APPLY_flag_register(apply_context);
    LAB_VECTORptr in_vector = APPLY_in_vect(apply_context);
    LAB_VECTORptr flag_vector = APPLY_flag_vector(apply_context);
    int flags_p = APPLY_flags_p(apply_context);
    STATEptr start_state = NET_start_state(APPLY_net1(apply_context));
    NETptr net= APPLY_net1(apply_context);

    /* Local variables: */
    VECTORptr location_vector= NULL;/* location stack */
    LOCATIONptr location= NULL;     /* the current location structure  */
    LOCATIONptr new_loc= NULL;      /* the new location structure to use */
    ARCptr arc;                     /* the current arc if standard     */
    id_type in_sym, arc_sym, flag_attr = 0;
    int result_count = 0;           /* number of output strings found  */
    int epsilon_loop_p = FALSE;     /* TRUE if an eps-loop is detected */
    int continuation_found = TRUE;  /* TRUE if a new location is found */
    int backtracked_loc= FALSE;     /* TRUE if a backtracked loc was found */
    int parse_error= NO_ERROR; /* error code for parsing the input string */

    /* For vectorized states: */
    RANGEptr range_map= NULL;   /* maps each id to labels it belongs to. */
    id_type *items [2];         /* label arrays extracted from range_map */
    int range_len [2];          /* lengths of label arrays "items"       */
    AVptr arc_array= NULL;      /* arc_vector of current state/location  */
    STATEptr current_state= NULL; /* state of the current location       */
    STATEptr dest_state= NULL;  /* destination state of the current match */

    /* Location block variables: */
    LOCATIONtype *heap= NULL;/* address of the LOCATIONptr block. */
    int heap_pos= 0;         /* next available position in LOCATIONptr block. */
    int i= 0;                /* index for heap sweeping */

    /* Retrieve the heap from the APPLY_CONTEXT:
       it must have been already allocated by init_apply_net() 
       otherwise return.*/
    heap= apply_context->location_heap;
    if (heap == NULL)
    {
        not_enough_memory ("apply_vectorized_network");
        return 0;
    }

    /* Use the arc_vector as a location array to store each solution. */
    location_vector= APPLY_arc_vector(apply_context);

    /* Make sure that sigma is in the expected format: */
    label_to_binary(APPLY_sigma(apply_context));

    /* Set the range_map depending on the input side:
       (using the range maps speeds up the access to the dest_array
       of the vectorized states). */
    if (APPLY_side(apply_context) == LOWER)
        range_map= NET_uprange_map(net);
    else
        range_map= NET_downrange_map(net);

    /* Prepare the ranges for possible epsilon transitions:
        - the net labels having epsilon on the input side are stored in
          items[0],
        - the net labels having the current in_sym on the input side (or
          OTHER if the in_sym is not in the net sigma), are stored in
          items[1]
          => items[1] is set each time a new in_sym is get... */
    range_len[0]= RANGE_len(range_map + EPSILON);
    items[0]= RANGE_items(range_map + EPSILON);

    /* Build the first location with the start_state: */
    /* Get the location from the heap. */
    location= &(heap[0]);
    heap_pos++;
    /* Set up that location: */
    set_location(location, start_state);
    LOCATION_arc(location)= STATE_arc_set(start_state);
    LOCATION_id(location)= EPSILON;

    while (continuation_found)
    {
        /* Get a new input symbol: */
        do {
            parse_error= fetch_input_symbol_vect(&in_sym, apply_context);
        } while (in_sym != ID_NO_SYMBOL && flag_diacritic_p(in_sym));

        /* test if the parsing failed: */
        if (parse_error == ERROR)
          break;
              
        /* Test if a complete match has been found : */
        if ((in_sym == ID_NO_SYMBOL)
            && STATE_final(LOCATION_state(location)))
	{
            /* Complete match found: */
            /* Fill in the location_vector with the current path
               taken from location_heap: */
            
            /* reset the location_vector: */
            VECTOR_pos(location_vector)= 0;
            assure_vector_space(heap_pos-1, location_vector);

            for (i= 0; i< heap_pos-1; i++)
            {
                /* append to vector: */
                VECTOR_array(location_vector)[i]= (void *) &heap[i];
            }
            VECTOR_pos(location_vector)= heap_pos-1;

            /* Record that solution: */
            APPLY_output_fn(apply_context)(apply_context);
            result_count++;
	}

        /* Prepare the search for a possible new destination state: */
        continuation_found= FALSE;

        do
        {
            /* Retrieve the current state from the current location: */
            current_state= LOCATION_state(location);

            if (!STATE_vector_p(current_state))
            {
                /* The current state is NON vectorized: */

                while(TRUE)
                {
                    arc= LOCATION_arc(location);
                  
                    if (arc == NULL)
                    {
                        /* No more arc to explore from this state =>
                           abandon the current location and backtrack 
                           afterwards for finding a new location from 
                           which we can continue the search. */
                        break;
                    }
                    else
                    {
                        /* Examine the current arc transition: */
                        arc_sym = INPUT_MATCH[ARC_label(arc)];

                        flag_attr = flag_diacritic_attrib(arc_sym);

                        if (flags_p)
                        {
                            /* Save current value or just increment the
                               current pos if there is no value to save.
                            */

                            if (flag_attr)
                                append_to_lab_vector(flag_value(flag_register,
                                                              flag_attr),
                                                   flag_vector);
                            else
                                increment_lab_vector(flag_vector);
                        }

                        /* Test for a "real" match first: */
                        if ((arc_sym == in_sym) && !flag_attr) 
                        {
                            /* Matching a real symbol. */
                            continuation_found= TRUE;

                            dest_state = ARC_destination(arc);

                            /* Prepare the moving to dest_state by
                               setting a new location:          */
                            new_loc= &(heap[heap_pos]);
                            heap_pos++;
                            set_location(new_loc, dest_state);
                          
                            if (!STATE_vector_p(dest_state))
                                LOCATION_arc(new_loc)= STATE_arc_set(dest_state);
                          
                            LOCATION_id(location)= ARC_label(arc);

                            /* Move to the dest_state:  */
                            location= new_loc;

                            APPLY_in_pos(apply_context)++;
                            break;
                        }
                        else if ((arc_sym == OTHER)
                                 && (in_sym != ID_NO_SYMBOL)
                                 && !binary_sigma_member(APPLY_sigma(apply_context), in_sym) )
                        {
                            /* Matching with current arc symbol OTHER: */
                            continuation_found= TRUE;

                            dest_state = ARC_destination(arc);

                            /* Prepare the moving to dest_state by
                               setting a new location:          */
                            new_loc= &(heap[heap_pos]);
                            heap_pos++;
                            set_location(new_loc, dest_state);
                          
                            if (!STATE_vector_p(dest_state))
                                LOCATION_arc(new_loc)= STATE_arc_set(dest_state);
                          
                            LOCATION_id(location)= ARC_label(arc);

                            /* Move to the dest_state:  */
                            location= new_loc;

                            APPLY_in_pos(apply_context)++;
                            break;
                        }
                        else if/*(!flag_attr && ANY_MATCH(in_sym,arc_sym,apply_context))*/
                           ( !flag_attr
                            && ((in_sym == OTHER)
                                &&
                                (!APPLY_prev_sigma(apply_context)
                                 || other_than_test(arc_sym, apply_context))) )
                        {
                            /* Matching a OTHER as input symbol. */
                            continuation_found= TRUE;

                            dest_state = ARC_destination(arc);

                            /* Prepare the moving to dest_state by
                               setting a new location:          */
                            new_loc= &(heap[heap_pos]);
                            heap_pos++;
                            set_location(new_loc, dest_state);
                          
                            if (!STATE_vector_p(dest_state))
                                LOCATION_arc(new_loc)= STATE_arc_set(dest_state);
                          
                            LOCATION_id(location)= ARC_label(arc);

                            /* Move to the dest_state:  */
                            location= new_loc;

                            APPLY_in_pos(apply_context)++;
                            break;
                        }
                        /* Test for an epsilon match: */
                        else if (EPSILON_MATCH(arc_sym))
                        {
                            /* Epsilon match ( no flag diacritic):
                               Test to avoid epsilon loops. */

                            /* Push the traversed location in the stack:
                               (and remove it later if the transition
                               happened to create a simple or complex
                               epsilon loop with respect to the path
                               already traversed until now). */
                            LOCATION_id(location)= ARC_label(arc);
                          
                            if (has_epsilon_loop_vect(heap, heap_pos,
                                                      start_state))
                            {
                                /* Epsilon loop found ! */
                                epsilon_loop_p = TRUE;

                                /* Move to the next arc: */
                                arc = next_arc(arc);
                                
                                LOCATION_arc(location)= arc;
                          
                                /* If an epsilon loop has been detected,
                                   there is no reason to discard the
                                   current input symbol.Don't understand
                                   why this clause is here. In any case,
                                   it seems to be the source of a bug
                                   reported by Ron. The bug goes back a
                                   long way, to January 2000 at least.
                                   2001-09-26  LK

                                   if (APPLY_next_symbol_fn(apply_context)
                                   != symbol_from_in_vector)
                                   decrement_lab_vector(in_vect); */
                            }
                            else
                            {
                                /* Epsilon match found with no loop: */
                                continuation_found= TRUE;

                                dest_state = ARC_destination(arc);

                                /* Prepare the moving to dest_state by
                                   setting a new location:          */
                                new_loc= &(heap[heap_pos]);
                                heap_pos++;
                                set_location(new_loc, dest_state);

                                LOCATION_arc(new_loc)= STATE_arc_set(dest_state);

                                LOCATION_id(location)= ARC_label(arc);
                              
                                /* Move to the dest_state:  */
                                location= new_loc;
                          
                                break;
                            }
                        } /* else if (EPSILON_MATCH(arc_sym) */
                        else if (flag_attr &&
                                 (!flags_p ||
                                  flag_action(flag_register, flag_diacritic(arc_sym))))
                        {
                            /* flag diacritic match detected:
                               Test to avoid epsilon loops. */

                            /* Push the traversed location in the stack:
                               (and remove it later if the transition
                               happened to create a simple or complex
                               epsilon loop with respect to the path
                               already traversed until now). */
                            LOCATION_id(location)= ARC_label(arc);
                          
                            if (has_epsilon_loop_vect(heap, heap_pos,
                                                      start_state))
                            {
                                /* Epsilon loop found ! */
                                epsilon_loop_p = TRUE;

                                /* Move to the next arc: */
                                arc = next_arc(arc);

                                LOCATION_arc(location)= arc;
                          
                                /* If an epsilon loop has been detected,
                                   there is no reason to discard the
                                   current input symbol.Don't understand
                                   why this clause is here. In any case,
                                   it seems to be the source of a bug
                                   reported by Ron. The bug goes back a
                                   long way, to January 2000 at least.
                                   2001-09-26  LK

                                   if (APPLY_next_symbol_fn(apply_context)
                                   != symbol_from_in_vector)
                                   decrement_lab_vector(in_vect); */
                            }
                            else
                            {
                                /* Epsilon match found: */
                                continuation_found= TRUE;

                                /* Flag diacritic in the input! */
                                if (arc_sym == in_sym)
                                    APPLY_in_pos(apply_context)++;

                                dest_state = ARC_destination(arc);

                                /* Prepare the moving to dest_state by
                                   setting a new location:          */
                                new_loc= &(heap[heap_pos]);
                                heap_pos++;
                                set_location(new_loc, dest_state);

                                LOCATION_arc(new_loc)= STATE_arc_set(dest_state);

                                LOCATION_id(location)= ARC_label(arc);
                              
                                /* Move to the dest_state:  */
                                location= new_loc;
                          
                                break;
                            }
                        } /* else if (flag_attr */
                        else
                        {
                          /* No match. Continue with the next arc.
                             If the arc was a flag diacritic, discard
                             the saved value. */
	  
                            if (flags_p)
                                decrement_lab_vector(flag_vector);

                            LOCATION_arc(location)= next_arc(arc);
                        } /* no match */
                    } /* if arc != NULL */
                } /* while(TRUE) */
            } /* if non vectorized */
            else
            {
                /* The current state IS vectorized:
                 find an entry in the destarray or epsilon array. */

                int epsilon_pos;
                int match_pos;
                  
                arc_array= STATE_arc_vector(current_state);

                /* Get the last visited indexes in the current location: */
                epsilon_pos= LOCATION_eps_pos(location);
                match_pos= LOCATION_match_pos(location);

                /* Check if the input symbol is in the net sigma:
                   if so, retrieve the set of two-sided symbols of
                   the net that have the input symbol on one of their
                   side. */
                if (binary_sigma_member(NET_sigma (net), in_sym))
                {
                    range_len[1]= RANGE_len(range_map + in_sym);
                    items[1]= RANGE_items(range_map + in_sym);
                }
                else  
                {
                    /* The input symbol is not part of the net's sigma:
                       => the input label can only be accepted by a
                       symbol having OTHER on one of its sides. Retrieve
                       this set of symbols. */
                    range_len[1]= RANGE_len(range_map + OTHER);
                    items[1]= RANGE_items(range_map + OTHER);
                }

                /* Search first for an epsilon match in items[0]: 
                   start the search from the last visited position if any. */
                epsilon_pos++;

                while (epsilon_pos < range_len[0])
                {    
                    dest_state= AV_dest(arc_array,
                                        items[0][epsilon_pos]);

                    if (dest_state != NULL)
                    {
                        /* Epsilon match found: */

                        LOCATION_id(location)= items[0][epsilon_pos];

                        /* Test if that transition would create an
                           epsilon loop: */
                        if (has_epsilon_loop_vect(heap, heap_pos,
                                                  start_state))
                        {
                            /* Epsilon loop detected ! */
                            epsilon_loop_p = TRUE;

                            /* Try next entry in the epsilon vector: */
                            epsilon_pos++;
                            continue;
                        }
                        else
                        {
                            /* Epsilon match found: */
                            continuation_found= TRUE;

                            /* Set up a new location before moving to
                               dest_state:  */
                            new_loc= &(heap[heap_pos]);
                            heap_pos++;
                            set_location(new_loc, dest_state);

                            if (!STATE_vector_p(dest_state))
                                LOCATION_arc(new_loc)= STATE_arc_set(dest_state);

                            LOCATION_id(location)= items[0][epsilon_pos];

                            /* Update the position in the current
                               location for future possible backtracking: */
                            LOCATION_eps_pos(location)= epsilon_pos;
                              
                            /* Note that the location is already
                               pushed on the stack ! */

                            /* Move to the dest_state:  */
                            location= new_loc;

                            arc_sym= INPUT_MATCH[items[0][epsilon_pos]];

                            flag_attr = flag_diacritic_attrib(arc_sym);

                            if (flags_p)
                            {
                                /* Save current value or just increment
                                   the current position if there is no
                                   value to save. */

                                if (flag_attr)
                                    append_to_lab_vector(flag_value(flag_register, flag_attr),
                                                         flag_vector);
                                else
                                    increment_lab_vector(flag_vector);
                            }
                              
                            if (flag_attr && arc_sym == in_sym)
                                APPLY_in_pos(apply_context)++;
                            
                            break;
                        }
                    } /* if (dest_state */
                      
                    epsilon_pos++;

                } /* while epsilon_pos */

                if (continuation_found)
                    break;
                  else
                      /* Update the position in the current location for
                         future possible backtracking: */
                      LOCATION_eps_pos(location)= epsilon_pos;

                /* Search for an OTHER match or classical match in
                   items[1]: 
                   Discard this search if the input string is already
                   exhausted. */
                if (in_sym != ID_EOS)
                {
                    /* Start the search from the last position if any:*/
                    match_pos++;
                    while (match_pos < range_len[1])
                    {
                        dest_state= AV_dest(arc_array,
                                            items[1][match_pos]);

                        if (dest_state != NULL)
                        {
                            /* match found: */
                            continuation_found= TRUE;

                            arc_sym = INPUT_MATCH[items[1][match_pos]];

                            flag_attr = flag_diacritic_attrib(arc_sym);

                            if (flags_p)
                            {
                                /* Save current value or just increment 
                                   the current position if no value to
                                   save: */
                                if (flag_attr)
                                    append_to_lab_vector(flag_value(flag_register,
                                                                    flag_attr),
                                                         flag_vector);
                                else
                                    increment_lab_vector(flag_vector);
                            }
                            
                            /* Prepare the moving to dest_state by
                               setting a new location:          */
                            new_loc= &(heap[heap_pos]);
                            heap_pos++;
                            set_location(new_loc, dest_state);
                          
                            LOCATION_id(location)= items[1][match_pos];

                            if (!STATE_vector_p(dest_state))
                                LOCATION_arc(new_loc)= STATE_arc_set(dest_state);

                            /* Update the position in the current
                               location for future possible backtracking:
                            */
                            LOCATION_match_pos(location)= match_pos;
                          
                            /* Push the traversed location in the stack */
                            /* Move to the dest_state:  */
                            location= new_loc;
                              
                            APPLY_in_pos(apply_context)++;

                            break;
                        }
                        match_pos++;

                    } /* while match_pos */
                }  /* if in_sym != ID_EOS */
                  
            } /* else vectorized */
    
          if (continuation_found)
              break;
          else
              /* Abandon that location because it's a deadlock */
              location= NULL;          

          /* Back up the location block until we find a location that
             has a continuation. Decrease input position at every
             step unless the input side of the label is epsilon
             or a flag diacritic not matching itself. */
          backtracked_loc= FALSE;
          heap_pos--;
          while(heap_pos > 0)
          {
              location= &heap[heap_pos-1];
              arc_sym = INPUT_MATCH[LOCATION_id(location)];
              flag_attr = flag_diacritic_attrib(arc_sym);
              
              if (flags_p)
              {
                  /* Restore the old value of the flag */
                  if (flag_attr)
                      subtract_from_lab_vector(&flag_value(flag_register,
                                                           flag_attr),
                                               flag_vector);
                  else
                      decrement_lab_vector(flag_vector);
              }

              if (flag_attr)
              {
                  /* A flag symbol that matches itself is treated like an
                     ordinary input symbol except that we also check for
                     the constraints. */
		    
                  if (APPLY_in_pos(apply_context) > 0 &&
                      arc_sym ==
                      LAB_VECTOR_array(in_vector)[APPLY_in_pos(apply_context)-1])
                      APPLY_in_pos(apply_context)--;
              }
              else if (!EPSILON_MATCH(arc_sym))
                  APPLY_in_pos(apply_context)--;
                  
              /* Check if the location corresponds to a non-vectorized
                 state: if so, move to the next arc */
              if (!STATE_vector_p(LOCATION_state(location)))
              {
                  ARCptr arc_loc;
                      
                  arc_loc = LOCATION_arc(location);
                  if ((arc_loc != NULL) && (next_arc(arc_loc) != NULL))
                  {
                      LOCATION_arc(location)= next_arc(arc_loc);
                      backtracked_loc= TRUE;
                  }
              }
              else
              { /* Just take the location as it is: it contains the last
                   positions in the range arrays to re-start the search
                   from. */
                  backtracked_loc= TRUE;
              }
	  
              if (backtracked_loc)
              {
                  /* Found the location, now retrieve the corresponding
                     input symbol at the current depth. */
                  lab_vector_element_at(&in_sym,APPLY_in_pos(apply_context),
                                        in_vector);
                  break;
              }
              else
                  heap_pos--;

          } /* end of backtracking loop */

      } while(location != NULL);
    } /* while (continuation_found) */

  if (epsilon_loop_p)
      fsm_fprintf(stdout, "Network has an epsilon loop on the input side.\n");

  return(result_count);
  
} /* apply_vectorized_network */


static int has_epsilon_loops_general(APPLYptr cntxt)
{
  /* The state vector contains a trail of successive
     states that have been entered through an epsilon
     transition on the input side. If the last state
     of the vector is identical to one of the previous
     states, we have an epsilon loop. */

  VECTORptr state_vector = APPLY_state_vector(cntxt);
  int pos = VECTOR_pos(state_vector);

  if (pos > 1)
    {
      STATEptr s1, s2;

      vector_element_at((void **) &s1, --pos, state_vector);

      while (pos > 0)
	{
	  vector_element_at((void **) &s2, --pos, state_vector);

	  if (s1 == s2)
	    return(TRUE);
	}
    }

  return(FALSE);
}


static int new_look(STATEptr state, int out_pos, APPLYptr cntxt);

static int match_epsilons(id_type id, id_type input_id,  ARCptr arc,
			  int out_pos, APPLYptr cntxt)
{
	  
  id_type attr, old_val;
  int count;

  attr = flag_diacritic_attrib(input_id);
	  
  if (APPLY_flags_p(cntxt) && attr)
    {
      old_val = flag_value(APPLY_flag_register(cntxt), attr);

      if (!flag_action(APPLY_flag_register(cntxt), flag_diacritic(id)))
	return(0);
    }
  else
    attr = 0;

  append_to_vector((void *) ARC_destination(arc), APPLY_state_vector(cntxt));

  if (has_epsilon_loops_general(cntxt))
    fsm_fprintf(stdout,
		"Network has an epsilon loop on the input side.\n");

  else if (id == ALTCHAIN_SYMBOL || id == EPSILON || attr)
    count = new_look(ARC_destination(arc), out_pos, cntxt);
  else
    {
      set_lab_vector_element_at(id, out_pos, APPLY_out_vector(cntxt));
      count = new_look(ARC_destination(arc), out_pos+1, cntxt);
    }
	  	
  if (attr)
    flag_value(APPLY_flag_register(cntxt), attr) = old_val;

  return(count);
}

static int match_ordinary_sym(id_type id, ARCptr arc, int out_pos,
			      APPLYptr cntxt)
{
  int count;

  APPLY_in_pos(cntxt)++;

  /* No epsilon loop is possible. */

  VECTOR_pos(APPLY_state_vector(cntxt)) = 0;

  set_lab_vector_element_at(id, out_pos, APPLY_out_vector(cntxt));
  count = new_look(ARC_destination(arc), out_pos+1, cntxt);

  APPLY_in_pos(cntxt)--;

  return(count);
}



static int new_look(STATEptr state, int out_pos, APPLYptr cntxt)
{

  ARCptr arc;
  id_type in_sym, id, input_id;
  int count = 0;
  RANGEptr range, eps_range;
  RANGE_VECTORptr *table = MATCH_TABLE_table(APPLY_match_table(cntxt));
  int path_depth;
  int i;

  /* Remember the trail of epsilon arcs leading to the state. */

  path_depth = VECTOR_pos(APPLY_state_vector(cntxt));

  do {
    fetch_input_symbol(&in_sym, cntxt);
/*	 
	if (flag_diacritic_p(in_sym))
	  ...
	  Should we allow flag diacritics in the input?
	  If so, what should we do here?
*/
  } while (in_sym != ID_NO_SYMBOL && flag_diacritic_p(in_sym));

  if (in_sym == ID_NO_SYMBOL && STATE_final(state))
    {
      APPLY_output_fn(cntxt)(cntxt);
      count++;
    }

  for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
    {
      eps_range = RANGE_VECTOR_vector(table[ARC_label(arc)]);
	  
      if (in_sym != ID_NO_SYMBOL)
	{
	  if (sigma_member(NET_sigma(APPLY_net1(cntxt)), in_sym) &&
	      in_sym < MATCH_TABLE_len(*table))
	    range = eps_range + in_sym;
	  else
	    range = eps_range + OTHER;

	  if (RANGE_len(range) == 0 && in_sym == OTHER)
	    {
	      id_type arc_sym = ARC_label(arc);

	      /* Iterate through all the symbols in the arc_symbol's
		 equivalence class. */

	      range = MATCH_TABLE_class(APPLY_match_table(cntxt)) + arc_sym;

	      for (i = 0; i < RANGE_len(range); i++)
		{
		  id = RANGE_item(range, i);
		  input_id = APPLY_in_fn(cntxt)(id);

		  if (EPSILON_MATCH(input_id) ||
		      (flag_diacritic_p(input_id) &&
		       APPLY_obey_flags_p(cntxt)))
		    count += match_epsilons(id, input_id, arc, out_pos, cntxt);
		  else if (ANY_MATCH(in_sym, input_id, cntxt))
		    count += match_ordinary_sym(id, arc, out_pos, cntxt);
		}
	    }
	  else
	    for (i = 0; i < RANGE_len(range); i++)
	      {
		id = RANGE_item(range, i);
    	   	input_id = APPLY_in_fn(cntxt)(id);

		/* If the input symbol is OTHER we must check whether
		   the input side of ID qualifies as a match. The
		   check fails if there is a previous sigma and
		   if the input side of ID is present there and
		   is not OTHER. Note that OTHER will always match OTHER.
		*/
		if (in_sym == OTHER && APPLY_prev_sigma(cntxt) &&
		    input_id != OTHER &&
		    binary_sigma_member(APPLY_prev_sigma(cntxt),
					APPLY_in_fn(cntxt)(id)))
		  continue;
		else
		  count += match_ordinary_sym(id, arc, out_pos, cntxt);
	      }
	}
      /* Look for epsilon matches. The vector position needs
	 to be reset to the position as it may have been
	 reset in the recursion.  */
	
      for (i = 0; i < RANGE_len(eps_range); i++)
	{
	  id = RANGE_item(eps_range, i);
	  input_id = APPLY_in_fn(cntxt)(id);
	  
	  VECTOR_pos(APPLY_state_vector(cntxt)) = path_depth;
	
	  count += match_epsilons(id, input_id, arc, out_pos, cntxt);
	}
    	

    }

  return(count);
}


int apply_reduced_network(APPLYptr cntxt)
{
  /* Applies the network in a depth-first manner to symbols of the
     input string.
     the output buffer separated by newline characters. The input side
     must be UPPER or LOWER. We use STATE_VECTOR to keep track
     of the path in order to be able to check for epsilon loops. */

  STATEptr start_state = NET_start_state(APPLY_net1(cntxt));

  append_to_vector((void *) start_state, APPLY_state_vector(cntxt));

  return(new_look(start_state, 0, cntxt));
}


/******************* BEGIN OF APPLY_PATTERNS ***********************/


static void right_shift_buffer_by_n(STRING_BUFFERptr buffer, int n)
{
  /* Shifts the contents of the string buffer to the right
     by n characters.

     Append first n characters to the end of the buffer to
     make sure that the memmove operation does not overflow
     the allocated space. append_char_to_buffer() calls
     realloc if necessary.
  */

  char *string; 
  int i;

  for (i = 0; i < n; i++)
    append_char_to_buffer('\0', buffer);

  string = STRING_BUFFER_string(buffer);
  memmove(string+n, string, STRING_BUFFER_pos(buffer));
}


static id_type left_context_marker(void)
{
  /* Returns the ID of the left context maker */

  static id_type lc_marker = 0;

  if (!lc_marker)
    FST_intern_literal(&lc_marker, "*LEFT_CONTEXT*");

  return(lc_marker);
}

static id_type non_left_context_marker(void)
{
  /* Returns the ID of the non_left context maker */

  static id_type non_lc_marker = 0;

  if (!non_lc_marker)
    FST_intern_literal(&non_lc_marker, "*NON_LEFT_CONTEXT*");

  return (non_lc_marker);
}

static id_type right_context_marker(void)
{
  /* Returns the ID of the right context maker */

  static id_type rc_marker = 0;

  if (!rc_marker)
    FST_intern_literal(&rc_marker, "*RIGHT_CONTEXT*");

  return (rc_marker);
}

static id_type non_right_context_marker(void)
{
  /* Returns the ID of the non_right context maker */

  static id_type non_rc_marker = 0;

  if (!non_rc_marker)
    FST_intern_literal(&non_rc_marker, "*NON_RIGHT_CONTEXT*");

  return (non_rc_marker);
}

static id_type and_check_mark(void)
{
  /* Returns the ID of the conjunctive check mark. */

  static id_type check_mark = 0;

  if (!check_mark)
    FST_intern_literal(&check_mark, "*AND_CHECK*");
  
  return(check_mark);
}

static id_type or_check_mark(void)
{
  /* Returns the ID of the disjunctive check mark. */

  static id_type check_mark = 0;

  if (!check_mark)
    FST_intern_literal(&check_mark, "*OR_CHECK*");
  
  return(check_mark);
}

static int lc_marker_p(id_type id)
{
  /* Returns TRUE iff id is the left context marker. */

  return (id == left_context_marker());
}

static int non_lc_marker_p(id_type id)
{
  /* Returns TRUE iff id is the non_left context marker. */

  return (id == non_left_context_marker());
}

static int rc_marker_p(id_type id)
{
  /* Returns TRUE iff id is the right context marker. */

  return (id == right_context_marker());
}

static int non_rc_marker_p(id_type id)
{
  /* Returns TRUE iff id is the non_right context marker. */

  return (id == non_right_context_marker());
}

static id_type and_check_mark_p(id_type id)
{
  /* Returns TRUE iff id is the conjunctive check mark. */

  return(id == and_check_mark());
}

static id_type or_check_mark_p(id_type id)
{
  /* Returns TRUE iff id is the disjunctive context marker. */

  return(id == or_check_mark());
}

static int context_marker_p(id_type id)
{
  /* Returns TRUE iff id is some context marker. */

  return(lc_marker_p(id) || rc_marker_p(id) ||
	 non_lc_marker_p(id) || non_rc_marker_p(id));

}

static int check_mark_p(id_type id)
{
  /* Returns TRUE iff id is a check mark. */

  return(and_check_mark_p(id) || or_check_mark_p(id));
}


static int closing_html_tag_p(FAT_STR name, int len)
{
  /* Returns true if tag name is not of the form </XYZ>. */

  FAT_STR second = (name+1);
  FAT_STR last = (name+(len-1));

  return(len > 3 && CHARNUM(name) == '<' && CHARNUM(second) == '/' &&
	 CHARNUM(last) == '>');

}

static void begin_html_tag_to_buffer(FAT_STR tag_name, int tag_len,
				     STRING_BUFFERptr out_buffer)
{				     

  /* Copies a tag known to be of </XYZ>, we copy it into the buffer
     without the slash, that is as <XYZ> but without the '\0'
     marker at the end. We restore whatever character was
     originally in that position. */
  
  char c;

  assert(tag_len < STRING_BUFFER_length(out_buffer));

  c = STRING_BUFFER_string(out_buffer)[tag_len-1];

  append_char_to_buffer('<', out_buffer);

  FST_fat_string_to_buf(tag_name+2, out_buffer);

  STRING_BUFFER_string(out_buffer)[tag_len-1] = c;

}



void merge_to_buffer(void *apply_context)
{
  /* Stores the output into the buffer. If the output ends with
     an HTML end tag, the corresponding begin tag is inserted
     to mark the beginning of the pattern. If the
     buffer already contains an output string, only the
     beginning and end tags are added. The assumption here
     is that everything else would be the same. For example
     if the input "abc" would produce two outputs, 
     <person>abc</person> and <company>abc</company>, we produce
     <company><person>abc/person></company> instead.
     */

  static int longest_path = 0;
  static LAB_VECTORptr tags = NULL;
  STRING_BUFFERptr out_buffer = APPLY_out_buffer((APPLYptr) apply_context);
  LAB_VECTORptr out_vector = APPLY_out_vector((APPLYptr) apply_context);
  int pos = LAB_VECTOR_pos(out_vector);
  id_type in_id, out_id, last_id;
  int path_length = pos;
  int tag_len, saved_buffer_pos;
  FAT_STR tag_name;
  LABELptr label;
  int i;
  
  /* If the interface variable 'delete-patterns' is ON we don't
     print anything. */

  if (!tags)
    tags = make_lab_vector(5);


  /* Any ALTCHAIN arcs on the path must be subtracted from
     the path length to get the true length */

  for (i = 0; i < LAB_VECTOR_pos(out_vector) ; i++)
    {
      lab_vector_element_at(&out_id, i, out_vector);
      if (out_id == ALTCHAIN_SYMBOL)
	path_length--;
    }


  /* If there is nothing to print we can stop here. */

  if (pos > 0)
    lab_vector_element_at(&last_id, pos-1, out_vector);
  else
    return;

  in_id = APPLY_in_fn((APPLYptr) apply_context)(last_id);
  out_id = APPLY_out_fn((APPLYptr) apply_context)(last_id);
  
  /* If the interface variable 'mark-patterns' is off, we don't
     print tags even if we have them. */

  tag_name = id_to_atom_name(out_id);
  tag_len = fat_strlen(tag_name);
  
  if (closing_html_tag_p(tag_name, tag_len) &&
      APPLY_mark_patterns_p((APPLYptr) apply_context))
    {
      /* Wee have a valid HTML tag. */

      long int count;
      label = id_to_label(out_id);
      count= (long int) LABEL_data(label);
      LABEL_data(label)= (void*) (count+1);
      APPLY_out_count((APPLYptr) apply_context)++;

      if (APPLY_delete_patterns_p((APPLYptr) apply_context))
	return;

      if (STRING_BUFFER_pos(out_buffer) > 0)
	{
	  /* We already have found another pattern with the same
	     starting location. Is the new one longer or of the
	     same length or shorter?
	     */

	  if (path_length > longest_path)
	    {

	      longest_path = path_length;

	      /* We write over the previous shorter match. */

	      STRING_BUFFER_pos(out_buffer) = 0;

	      /* Forget the previous tag(s) and remember this one. */

	      reset_lab_vector(tags);
	    }
	  else if (path_length == longest_path)
	    {
	      /* If we already have tagged the string with this
		  tag, we don't have to do it again. */

	      if (lab_vector_contains(last_id, tags))
		return;

	      /* We modify the previous result with an initial
		 HTML tag in the beginning and a final HTML tag
		 at the end. The length of the initial tag
		 is one less than the length of the final tag
		 because the slash is not present. Save the
		 buffer position and make it temporarily 0
		 in order to print the initial html tag in
		 the proper place.
	      */
	      
	      right_shift_buffer_by_n(out_buffer, tag_len-1);
	      saved_buffer_pos = STRING_BUFFER_pos(out_buffer);
	      STRING_BUFFER_pos(out_buffer) = 0;
	    }
	  else
	    {
	      /* This should not happen but, just in case, we
		 discard the shorter match. */

	      return;
	    }
	}
      else
	{
	  /* First match for a pattern with this starting location. */

	  longest_path = path_length ;

	  reset_lab_vector(tags);

	}

      /* Remember the tag. */

      append_to_lab_vector(last_id, tags);

      /* Copy an opening HTML tag into the buffer. */

      begin_html_tag_to_buffer(tag_name, tag_len, out_buffer);

      /* If this is the first match for the string, we print the
	 whole path. Otherwise we wrap the tags around what is
	 already there. */

      if (LAB_VECTOR_pos(tags) == 1)
	simple_network_output((APPLYptr) apply_context);
      else
	{
	  /* We will just print a new closing tag. But
	     first we need to restore the real buffer position
	     temporarily set to 0 to get the initial tag
	  */
	  
	  STRING_BUFFER_pos(out_buffer) = saved_buffer_pos;
	  out_id = APPLY_out_fn((APPLYptr) apply_context)(last_id);
	  symbol_name_to_buffer(out_id, out_buffer);
	}
    }
  else
    {
      if (!APPLY_delete_patterns_p((APPLYptr) apply_context))
	{
	  /* We are not printing tags and a shorter match
	     has already been found, we will print over
	     the previous output. */

	  longest_path = 0;
	  STRING_BUFFER_pos(out_buffer)  = 0;
	  
	  simple_network_output((APPLYptr) apply_context);
	}
    }
}

static int ok_to_start_with(id_type in_sym, APPLYptr cntxt)
{
  /* Returns TRUE if no separators are required. Otherwise, returns
     TRUE if we are at the beginning of the input or the input
     symbol IN_SYM is a whitespace symbol or other terminator.
  */

  FAT_STR sym_name;

   if (!(APPLY_need_separators_p(cntxt)))
     return(TRUE);

   if (in_sym == ID_NO_SYMBOL)
     return(TRUE);

   sym_name = id_to_atom_name(in_sym);

   /* Multi_character symbols are alphanumeric. */

   if (CHARNUM(sym_name+1) == '\0')
     return (!isalnum((int) CHARNUM(sym_name))) ;
   else
     return(FALSE);
}

static int ok_to_end_with(id_type in_sym, APPLYptr cntxt)
{
  /* Returns TRUE if no separators are required. Otherwise, returns
     TRUE if we are at the end of the input or the input
     symbol at POS is a whitespace symbol or other terminator.
  */

  FAT_STR sym_name;

   if (!(APPLY_need_separators_p(cntxt)) ||
       in_sym == ID_NO_SYMBOL)
     return(TRUE);

   sym_name = id_to_atom_name(in_sym);

   /* Multi_character symbols are alphanumeric. */

   if (CHARNUM(sym_name+1) == '\0')
     return (!isalnum((int) CHARNUM(sym_name))) ;
   else
     return(FALSE);
}

/* Need these declarations here because of recursive definitions */

static void longest_paths(STATEptr state, int *end_pos, APPLYptr cntxt);
static int check_context(STATEptr state, int pos, int diff, int len,
			 APPLYptr cntxt);
static int check_left_context(STATEptr state, id_type in_sym,
			       APPLYptr cntxt);
static int check_right_context(STATEptr state, id_type in_sym,
			       APPLYptr cntxt);
static int check_non_left_context(STATEptr state, id_type in_sym, 
				  APPLYptr cntxt);
static int check_non_right_context(STATEptr state, id_type in_sym,
				   APPLYptr cntxt);

static int check_contexts(STATEptr state, id_type marker, id_type in_sym,
			   int *end_pos, APPLYptr cntxt);


static int check_context_by_arc_vector(id_type in_sym, STATEptr state,
				       int pos, int diff, int len,
				       APPLYptr cntxt)
{

  /* Pursues the context check in a vectorized state. */

  int i;
  AVptr arc_vector = STATE_arc_vector(state);
  ALPHABETptr sigma = NET_sigma(APPLY_net1(cntxt));
  NETptr net = APPLY_net1(cntxt);
  RANGEptr range, range_map;
  STATEptr dest;
  id_type arc_label;
  
  /* We assume here that the network is deterministic and all
     labels are identity labels. */

  if (APPLY_side(cntxt) == LOWER)
    range_map = NET_uprange_map(net);
  else
    range_map = NET_downrange_map(net);

  /* BOUNDARY_SYMBOL matches ID_NO_SYMBOL. That is
     the very beginning or the very end of the input. */

  if (in_sym == ID_NO_SYMBOL)
    {
      range = range_map + BOUNDARY_SYMBOL;

      /* No need to go the destination if it exists. */

      if (RANGE_len(range) > 1)
	return(TRUE);
      else
	return(FALSE);
    }


  /* If the text symbol is not in the transducer's alphabet,
     treat it as OTHER.
  */

  if (!binary_sigma_member(sigma, in_sym))
    range = range_map + OTHER;
  else
    range = range_map + in_sym;

  for (i = 0; i < RANGE_len(range); i++)
    {
      arc_label = RANGE_item(range, i);
      if ((dest = AV_dest(arc_vector, arc_label)))
	{
	  if (check_context(dest, pos+diff, diff, len+1, cntxt))
	    return(TRUE);
	}
    }


  return(FALSE);
}


static int check_context_by_arc_list(id_type in_sym, STATEptr state, int pos,
				     int diff, int len, APPLYptr cntxt)
{
  /* Pursues the context check in an ordinary state. */

  id_type arc_sym;
  ARCptr arc;
  STATEptr dest;

  /* We assume here that the network is deterministic and all
     labels are identity labels. ALTCHAIN symbols are in general
     removed from the net by sharing common tails. However,
     ALTCHAIN arcs that have a vectorized state as the
     destination cannot be removed and must be taken into
     account.  */

  for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
    {
      arc_sym = ARC_label(arc);
      dest = ARC_destination(arc);

      /* BOUNDARY_SYMBOL matches ID_NO_SYMBOL. That is
	 the very beginning or the very end of the input. */

      if (arc_sym == BOUNDARY_SYMBOL && in_sym == ID_NO_SYMBOL)
	return(TRUE);

      if (arc_sym == in_sym || OTHER_MATCH(in_sym, arc_sym, cntxt))
	return(check_context(dest, pos+diff, diff, len+1, cntxt));
      else if (arc_sym == ALTCHAIN_SYMBOL)
	return(check_context(dest, pos, diff, len+1, cntxt));

    }

  return(FALSE);
}

static int check_context(STATEptr state, int pos, int diff, int len, 
			 APPLYptr cntxt)
{
  /* The recursive core of all the context checking routined
     Branches depending on whether the sate is normal or a
     vector state. POS is the position we are with respect to
     the current start location in the input ring. DIFF is
     either 1 or -1 depending on whether we are interested in
     the left or the right context. LEN is a counter that
     is checked here to make sure that we are not exceeding
     the limit of MAX_CONTEXT_LENGTH.
  */

  id_type in_sym;

  if (STATE_final(state))
    return(TRUE);

  if (len >= MAX_CONTEXT_LENGTH)
    {
      id_type id, tag;
      LAB_VECTORptr out_vector = APPLY_out_vector(cntxt);
      int pos = LAB_VECTOR_pos(out_vector);

      lab_vector_element_at(&id, pos-1, out_vector);

      if (APPLY_out_side(cntxt) == UPPER)
	tag = upper_id(id);
      else
	tag = lower_id(id);

      fprintf(stderr, "\n*** Maximum context length (%d) exceeded while checking the ",
	      MAX_CONTEXT_LENGTH);
      fprintf(stderr, "%s context\n    for the ", 
	      (diff < 0) ? "left" : "right");
      print_label(tag, stderr, FALSE);
      fprintf(stderr, " tag. The string under consideration is:\n    ");

      /* Don't print the tag again. */

      LAB_VECTOR_pos(out_vector)--;
      print_lab_vector(out_vector, stderr, FALSE);
      LAB_VECTOR_pos(out_vector)++;

      fprintf(stderr, "\n");

      return(FALSE);
    }

  /* We can check left context, right context, non-left context
     and non-right context independently and in any order.
  */
     
  fetch_input_at_pos(&in_sym, pos, cntxt);


  if (STATE_vector_p(state))
    return(check_context_by_arc_vector(in_sym, state, pos, diff, len, cntxt));
  else
    return(check_context_by_arc_list(in_sym, state, pos, diff, len, cntxt));
}

static int skip_check(id_type in_sym, APPLYptr cntxt)
{
  /* Determines whether any check is necessary. */

  /* There is no point in checking context in any of the following
     three situations:
     (1) the path is empty.
     (2) we are not at a good ending location.
  */

  return(APPLY_in_pos(cntxt) == 0 ||
	 !ok_to_end_with(in_sym, cntxt));
}


static int check_left_context(STATEptr state, id_type in_sym,
			       APPLYptr cntxt)
{
  /* Returns FALSE if no check is necessary or if the
     check of the left context fails. Othewise TRUE. */

  /* First check if we have to check the context .*/

  if (skip_check(in_sym, cntxt))
    return(FALSE);

  /* The checking of the left context begins at the letter preceding
     the current start location in the input ring. If no input was
     previously processed, we will see ID_NO_SYMBOL in that
     position. */

  return (check_context(state, -1, -1, 0, cntxt));
}

static int check_non_left_context(STATEptr state, id_type in_sym,
				   APPLYptr cntxt)
{
  /* Returns FALSE if no check is necessary or if the
     check of the left context succeeds. Othewise TRUE. */

  /* The checking of the left context begins at the letter preceding
     the current start location in the input ring. If no input was
     previously processed, we will see ID_NO_SYMBOL in that
     position. */

  /* First check if we have to check the context .*/

  if (skip_check(in_sym, cntxt))
    return(FALSE);

  /* If the check for the left context fails, we have succeeded. */

  return(!(check_context(state, -1, -1, 0, cntxt)));
}

static int check_right_context(STATEptr state, id_type in_sym,
				APPLYptr cntxt)
{
  /* Returns FALSE if no check is necessary or if the
     check of the right context fails. Othewise TRUE. */

  int pos = APPLY_in_pos(cntxt);

  /* First check if we have to check the context .*/

  if (skip_check(in_sym, cntxt))
    return(FALSE);

  /* The checking of the right context begins at the location following
     our current position in the input ring. We will read in more
     characters into the ring but we keep our input position
     unchanged. We assume that we don't need to consider more than
     MAX_CONTEXT_LENGTH symbols. */

  return(check_context(state, pos, 1, 0, cntxt));
}


static int check_non_right_context(STATEptr state, id_type in_sym,
				   APPLYptr cntxt)
{
  /* Returns FALSE if no check is necessary or if the
     check of the right context fails. Othewise TRUE. */

  int pos = APPLY_in_pos(cntxt);

  /* The checking of the right context begins at the location following
     our current position in the input ring. We will read in more
     characters into the ring but we keep our input position
     unchanged. We assume that we don't need to consider more than
     MAX_CONTEXT_LENGTH symbols. */

  /* First check if we have to check the context .*/

  if (skip_check(in_sym, cntxt))
    return(FALSE);

  return(!(check_context(state, pos, 1, 0, cntxt)));
}

static int check_contexts_by_arc_vector(STATEptr state, id_type marker,
					id_type in_sym,	APPLYptr cntxt)
{
  /* Entry point for context checking for vector states. */

  int i, j, result;
  AVptr arc_vector = STATE_arc_vector(state);
  NETptr net = APPLY_net1(cntxt);
  RANGEptr range[4], range_map;
  ALPHABETptr eps_labs;
  STATEptr dest;
  int test_done = FALSE;

  int or_test = or_check_mark_p(marker);

  if (APPLY_side(cntxt) == LOWER)
    {
      range_map = NET_uprange_map(net);
      eps_labs = AV_l_eps(arc_vector);
    }
  else
    {
      range_map = NET_downrange_map(net);
      eps_labs = AV_u_eps(arc_vector);
    }

  range[0] = range_map + left_context_marker();
  range[1] = range_map + right_context_marker();
  range[2] = range_map + non_left_context_marker();
  range[3] = range_map + non_right_context_marker();
  
  for (i = 0; i < 4; i++)
    {
      for (j = 0; j < RANGE_len(range[i]); j++)
	{
	  if ((dest = AV_dest(arc_vector, RANGE_item(range[i],j))))
	    {
	      test_done = TRUE;

	      switch (i)
		{
		case 0:
		  result = check_left_context(dest, in_sym, cntxt);
		  break;
		case 1:
		  result = check_right_context(dest, in_sym, cntxt);
		  break;
		case 2:
		  result = check_non_left_context(dest, in_sym, cntxt);
		  break;
		case 3:
		  result = check_non_right_context(dest, in_sym, cntxt);
		  break;
		default:
		  assert(0);
		  break;
		}
	      
	      if (or_test)
		{
		  if (result)
		    return(TRUE);
		}
	      else if (!result)
		return (FALSE);
	    }
	}
    }

  if (!test_done)
    {
      fprintf(stderr, "Warning: a ");
      if (or_test)
	print_label(or_check_mark(), stderr, FALSE);
      else
	print_label(and_check_mark(), stderr, FALSE);
      fprintf(stderr, " found but no check followed.\n");
    }

  return(TRUE);
}



static int check_contexts_by_arc_list(STATEptr state, id_type marker,
				      id_type in_sym, APPLYptr cntxt)
{
  /* Entry point for context checking for ordinary states. */

  ARCptr arc;
  STATEptr dest;
  id_type arc_label;
  int result;
  int or_test = or_check_mark_p(marker);

  for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
    {
      dest = ARC_destination(arc);
      arc_label = ARC_label(arc);

      /* Arc label must be one of the four check markers or
	 a check_mark. */

      if (lc_marker_p(arc_label))
	result = check_left_context(dest, in_sym, cntxt);
      else if (non_lc_marker_p(arc_label))
	result = check_non_left_context(dest, in_sym, cntxt);
      else if (rc_marker_p(arc_label))
        result = check_right_context(dest, in_sym, cntxt);
      else if (non_rc_marker_p(arc_label))
	result = check_non_right_context(dest, in_sym, cntxt);
      /*
      else if (and_check_mark_p(arc_label) || or_check_mark_p(arc_label))
	result = check_contexts(dest, arc_label, in_sym, end_pos, cntxt);
      */
      else
	{
	  fprintf(stderr, "Expecting a context marker. Found '");
	  print_label(arc_label, stderr, FALSE);
	  fprintf(stderr, "' instead.\n");
	  return(FALSE);
	}

      if (or_test)
	{
	  if (result)
	    return(TRUE);
	}
      else if (!result)
	return(FALSE);
    }
  return(TRUE);
}

static int check_contexts(STATEptr state, id_type marker, id_type in_sym,
			   int *end_pos, APPLYptr cntxt)
{
  /* The common entry point for checking multiple context constraints.
     Forks depending on what kind of state we have. Marker is either
     for a conjunctive or a disjunctive test.  However, if we have
     already output for a longer match, the tests can be skipped.
     If that is not the case we produce output on succes.
  */

  int result = TRUE;

  if (APPLY_in_pos(cntxt) < *end_pos)
    return(result);

  if (!STATE_final(state))
    {
      if (STATE_vector_p(state))
	result = check_contexts_by_arc_vector(state, marker, in_sym, cntxt);
      else
	result = check_contexts_by_arc_list(state, marker, in_sym, cntxt);
    }
    
  if (result)
    {
      APPLY_output_fn(cntxt)(cntxt);
      *end_pos = APPLY_in_pos(cntxt);
    }

  return(result);
}


static int check_one_context(STATEptr state, id_type marker, id_type in_sym,
			      int *end_pos, APPLYptr cntxt)
{
  /* The marker argument is known to be a context marker. We
     branch here into the appropriate check function. However,
     if we already have succeeded on a longer path, there is
     no point to continue here. If this is not the case and
     the check succeeds, we put the result into the out buffer.
  */

  int result = TRUE;

  if (APPLY_in_pos(cntxt) < *end_pos)
    return(result);

  if (!STATE_final(state))
    {
      /* Arc label must be one of the four check markers. */

      if (lc_marker_p(marker))
	result = check_left_context(state, in_sym, cntxt);
      else if (non_lc_marker_p(marker))
	result = check_non_left_context(state, in_sym, cntxt);
      else if (rc_marker_p(marker))
        result = check_right_context(state, in_sym, cntxt);
      else if (non_rc_marker_p(marker))
	result = check_non_right_context(state, in_sym, cntxt);
    }
    
  if (result)
    {
      APPLY_output_fn(cntxt)(cntxt);
      *end_pos = APPLY_in_pos(cntxt);
    }

  return(result);
}

static int check_range(AVptr arc_vector, RANGEptr range_map, id_type id, 
		       int (*check_fn)(STATEptr, id_type, id_type, int *,
				       APPLYptr),
		       id_type in_sym, int *end_pos, APPLYptr cntxt)
{
  /* Called from forward_by_arc_vector(). Id is known to be a context
     marker. If there is an outgoing transition, the check_fn is called
     to do the check and the result is return. Otherwise we return FALSE.
  */

  RANGEptr range;
  int i;
  STATEptr dest;

  /* If the symbol is not in the network's alphabet, it
     has no entry in the range map. */

  if (!binary_sigma_member(NET_sigma(APPLY_net1(cntxt)), id))
    return(FALSE);

  range = range_map + id;

  for (i = 0; i < RANGE_len(range); i++)
    if ((dest = AV_dest(arc_vector, RANGE_item(range, i))))
      return(check_fn(dest, RANGE_item(range,i), in_sym, end_pos, cntxt));
  return(FALSE);
}


static void forward_by_arc_vector(id_type in_sym, STATEptr state,
				  int *end_pos, APPLYptr cntxt)
{
  int i;
  AVptr arc_vector = STATE_arc_vector(state);
  ALPHABETptr sigma = NET_sigma(APPLY_net1(cntxt));
  NETptr net = APPLY_net1(cntxt);
  LAB_VECTORptr out_vector = APPLY_out_vector(cntxt);
  RANGEptr range, range_map;
  ALPHABETptr eps_labs;
  STATEptr dest;
  id_type arc_label;
  
  /* Advancing in a vectorized state. See a longer comment
     on the strategy and the assumptions in the comments
     to forward_by_arc_list. */


  if (APPLY_side(cntxt) == LOWER)
    {
      range_map = NET_uprange_map(net);
      eps_labs = AV_l_eps(arc_vector);
    }
  else
    {
      range_map = NET_downrange_map(net);
      eps_labs = AV_u_eps(arc_vector);
    }

  /* If we are at the end of the input, only look for
     epsilon matches */

  if (in_sym != ID_NO_SYMBOL)
    {
      /* If the text symbol is not in the transducer's alphabet,
	 treat it as OTHER.
      */

      if (!binary_sigma_member(sigma, in_sym))
	range = range_map + OTHER;
      else
	range = range_map + in_sym;

      for (i = 0; i < RANGE_len(range); i++)
	{
	  arc_label = RANGE_item(range, i);
	  if ((dest = AV_dest(arc_vector, arc_label)))
	    {
	      
	      /* Match moving forward. */

	      /* If we are matching OTHER, use the input symbol
		 itself. We assume that the net contains no
		 one-sided OTHERs and no input OTHERs.
	      */

	      if (RANGE_id(range) == OTHER)
		arc_label = in_sym;

	      /* The input position counter keeps track of where
		 we are with respect to the input that is
		 being process. 0 means the first letter
		 of the input at the start position in the
		 input ring. We do not move the input
		 counter when contexts are being checked.
	      */
		 
	      APPLY_in_pos(cntxt)++;
	      append_to_lab_vector(arc_label, out_vector);
	      longest_paths(dest, end_pos, cntxt);
	      decrement_lab_vector(out_vector);
	      APPLY_in_pos(cntxt)--;
	    }
	}
    }


  if (eps_labs)
    {
      /* Process arcs that have an EPSILON on the input side.
	 Staying in the current position.
      */

      for (i = 0; i < ALPH_len(eps_labs); i++)
	{
	  arc_label = ALPH_item(eps_labs, i);
	  dest = AV_dest(arc_vector, arc_label);

	  append_to_lab_vector(arc_label, out_vector);
	  longest_paths(dest, end_pos, cntxt);
	  decrement_lab_vector(out_vector);
	}
    }

  /* Look for a context or a check mark */


  if (check_range(arc_vector, range_map, left_context_marker(),
		       check_one_context, in_sym, end_pos, cntxt)   ||
      check_range(arc_vector, range_map, right_context_marker(),
		  check_one_context, in_sym, end_pos, cntxt)        ||
      check_range(arc_vector, range_map, non_left_context_marker(),
		  check_one_context, in_sym, end_pos, cntxt)        ||
      check_range(arc_vector, range_map, non_right_context_marker(),
		  check_one_context, in_sym, end_pos, cntxt)        ||
      check_range(arc_vector, range_map, and_check_mark(),
		  check_contexts, in_sym, end_pos, cntxt)           ||
      check_range(arc_vector, range_map, or_check_mark(),
		  check_contexts, in_sym, end_pos, cntxt))
    return;
}

static void forward_by_arc_list(id_type in_sym, STATEptr state,
				int *end_pos, APPLYptr cntxt)
{
  /* Scand forward in an ordinary state trying to find a
     match for the current input symbol. If we get an epsilon
     match, the output side of the label contains a tag and
     we continue in the destination state with the same
     input. If we succeed matching, we call longest_paths()
     again with the destination state of the arc. In
     traversing forward we push the labels along the way
     into cntxt->out_vector. cntxt->in_pos tracks the
     number of input symbols along the current path.
     
     If we encounter a context marker, and_check or, or_check
     marker, we call the appropriate checking function.
     
     If we succeed in checking a context, we don't need
     to look at the remaining arcs on the assumption that
     context markers occur at the end of an entry. If the
     state has an outgoing context marker or check mark,
     it has no ordinary outgoing arcs that could lead to
     a longer match.

     If there are more than one context conditions, it
     is sufficient to succeed once. If the conditions are
     conjunctive, they should be grouped under an and_check.
  */

  ARCptr arc;
  STATEptr dest;
  id_type arc_label, arc_sym;
  int match_p;
  LAB_VECTORptr out_vector = APPLY_out_vector(cntxt);
  id_type (*in_fn)(id_type) = APPLY_in_fn(cntxt);

  for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
    {
      dest = ARC_destination(arc);
      arc_label = ARC_label(arc);
      arc_sym = in_fn(arc_label);
      match_p = 0;
      
      if (check_mark_p(arc_label))
	{
	  if (check_contexts(dest, arc_label, in_sym, end_pos, cntxt))
	    return;
	}
      else if (context_marker_p(arc_label))
	{
	  if (check_one_context(dest, arc_label, in_sym, end_pos, cntxt))
	    return;
	}
      else if (EPSILON_MATCH(arc_sym))
	match_p = 1;
      else if (arc_sym == in_sym || OTHER_MATCH(in_sym, arc_sym, cntxt))
	{
	  APPLY_in_pos(cntxt)++;
	  match_p = 2;
	  
	  /* We assume that pattern network contains no one-sided
	     OTHERs and that there are no OTHERs in the input. */

	  if (arc_sym == OTHER)
	    arc_label = in_sym;
	}

      if (match_p)
	{
	  append_to_lab_vector(arc_label, out_vector);

	  longest_paths(dest, end_pos, cntxt);
	  decrement_lab_vector(out_vector);

	  /* If the match was a non-EPSILON match, we reset
	     the position counter. */

	  if (match_p == 2)
	    APPLY_in_pos(cntxt)--;

	}
    }
}



static void longest_paths(STATEptr state, int *end_pos, APPLYptr cntxt)
{
  /* The recursive core of apply_patterns(). Tries to match
     a pattern forward from the current location branching
     into forward_by_arc_vector() or forward_by_arclist()
     depending on the type of the state. If the search
     succeeds input is produced unless we already have
     found a longer match. */

  id_type in_sym;

  fetch_input_at_pos(&in_sym, APPLY_in_pos(cntxt), cntxt);

  if (STATE_vector_p(state))
    forward_by_arc_vector(in_sym, state, end_pos, cntxt);
  else
    forward_by_arc_list(in_sym, state, end_pos, cntxt);

  /* Output is produced iff
     (1) we are at a final state,
     (2) no longer match has been found, and
     (3) some non-empty string has been found, and
     (4) the match is followed by a separator or
     we don't require separators.
     (5) the delete-patterns flag is off.
  */

  if (STATE_final(state) && !(*end_pos > APPLY_in_pos(cntxt)) &&
      APPLY_in_pos(cntxt) > 0 && ok_to_end_with(in_sym, cntxt))
    {
      APPLY_output_fn(cntxt)(cntxt);
      *end_pos = APPLY_in_pos(cntxt);
    }
}

void reset_pattern_match_counts(APPLYptr apply_context)
{
  /* If we are counting the number of matches for each tag,
     the count is kept in the label data field of each
     tag symbol. To initiate the count we set the count
     to 0.
  */

  NETptr net = APPLY_net1(apply_context);
  ALPHABETptr labs = binary_to_label(NET_labels(net));
  FAT_STR name;
  LABELptr label;
  id_type out_id;
  int i;

  for (i = 0; i < ALPH_len(labs); i++)
    {
      if (APPLY_out_side(apply_context) == UPPER)
	out_id = upper_id(ALPH_item(labs, i));
      else
	out_id = lower_id(ALPH_item(labs, i));

      name = id_to_atom_name(out_id);

      if (closing_html_tag_p(name, fat_strlen(name)))
	{
	  label = id_to_label(out_id);
	  LABEL_data(label) = 0;
	}
    }
}

static void print_n_copies(char *str, int n, FILE *stream)
{
  int i;

  for (i = 0; i < n; i++)
    fsm_fprintf(stream, str);
}

void print_pattern_match_counts(APPLYptr apply_context, FILE *stream)
{
  /* The count is kept in the label data field of each
     tag symbol.
  */

  NETptr net = APPLY_net1(apply_context);
  ALPHABETptr labs = binary_to_label(NET_labels(net));
  FAT_STR name;
  LABELptr label;
  id_type out_id;
  int i, len, pad, line_len, max_len=0;

  sort_alph(labs);

  /* find the longest tag name */

  for (i = 0; i < ALPH_len(labs); i++)
    {
      if (APPLY_out_side(apply_context) == UPPER)
	out_id = upper_id(ALPH_item(labs, i));
      else
	out_id = lower_id(ALPH_item(labs, i));

      name = id_to_atom_name(out_id);
      len = fat_strlen(name);
      if (max_len < len)
	max_len = len;
    }

  /* Pad to the length of the longest tag */
  fsm_fprintf(stream, "Pattern");
  pad = max_len - strlen("Pattern");
  print_n_copies(" ", pad, stream);
  fsm_fprintf(stream, "\t# of matches\n");
  line_len = strlen("Pattern") + pad + 5 + strlen("# of matches");
  print_n_copies("-", line_len, stream);
  fsm_fprintf(stream, "\n");

  for (i = 0; i < ALPH_len(labs); i++)
    {
      if (APPLY_out_side(apply_context) == UPPER)
	out_id = upper_id(ALPH_item(labs, i));
      else
	out_id = lower_id(ALPH_item(labs, i));

      name = id_to_atom_name(out_id);
      len = fat_strlen(name);

      if (closing_html_tag_p(name, len))
	{
	  label = id_to_label(out_id);
	  FST_fprint_fat_string(stream, name);
	  print_n_copies(" ", (max_len - len), stream);
	  fsm_fprintf(stream, "\t%7d\n", (int) LABEL_data(label));
	}
    }
  print_n_copies("-", line_len, stream);
  fsm_fprintf(stream, "\n\t\tTotal:\t%7d\n", APPLY_out_count(apply_context));
}

int apply_patterns(APPLYptr cntxt)
{
  /* Applies the network in a depth-first manner. Produces
     output whenever the input path leads to a terminal state
     but only on the condition that there is no longer
     path leading to a terminal state for the same input.
     Continues the application until there is no more input.

     The input network, which may be circular, is assumed to
     consist of paths that end with a symbol pair of the
     type "</Tag>":0 or 0:"</Tag>" where EPSILON is on the input
     side and the tag on the output side. A matching initial
     output marker, "<Tag>" is provided automatically to mark
     the beginning of input. For example, it might contain
     a path compiled from the regular expression

         {Sarah Lee} "</company>":0 

     Unlike the more sophisticated apply routines,
     apply_patterns() does not no one-sided OTHER labels.
     It handles only the identity OTHER. Another limitation
     is that flag diacritics are not handled.

     If the network has the property VECTORIZE-N: K
     on its property list all states with K states or
     more will be vectorized.

     Any unrecognized sequence of input symbols is mapped
     to itself. Consequently, the apply_patterns() routine
     never fails on any input even if the pattern network
     does not have the universal language on its input
     side.

     If the same input string can be tagged in more than
     one way, the alternatives are wrapped around the
     input in the order they are found, for example,
     the input
     
       I know Sarah Lee.

     would produce the output

       I know <company><person>Sarah Lee</person></company>;

     if the pattern network recognizes for "Sarah Lee"
     as a company name and as a person name. This
     could arise in several ways. For example, the
     network might contain a set of simple person
     names such as "Sarah" and "Lee" and a pattern
     such as 

     [[PersonName [" " PersonName]*] "</person>:0]

     that allows for multi-word names.

     If the pattern network contains any other type of
     ambiguity, for example, mapping "abc" to both
     "<tag1>abc</tag2>" and "<tag2>xyw</tag2>", all but
     one of the possible results get discarded.

     A pattern entry may be associated with contextual
     conditions. For example, to suppress the company
     interpretation of Sarah Lee in the presense of
     a Title such as Mrs., Miss, etc. The company
     entry for Sarah Lee could be written as

         {Sarah Lee} "</company>":0
	 "*NON_LEFT_CONTEXT*" [[{Mrs }|{Miss }]].r

     Note that the specification of the disallowed
     left context is reversed: [[{Mrs }|{Miss }]].r
     The result is that "Sarah Lee" will be tagged as
     a company only if it is not preceded by a tit.e.

     A positive left context can be used to tag an
     otherwise uninterpretable item. For example, if
     we have defined Title in the appropriate way and
     CapSeq as a sequence of capitalized words separated
     by white space, then the entry

     CapSeq "</person>":0 "*LEFT_CONTEXT*" [Title " "].r

     has the effect of mapping "Mr. Bill Gates"
     into "Mr. <person>Bill Gates</person>.

     The right context may also be used to select or to
     disallow interpretations. For example, a suffix such
     as "Jr." usually indicates a name. The entry for
     this pattern could be

     CapSeq "</person>":0 "*RIGHT_CONTEXT* { Jr.}

     Finally, the special symbol "*NON_RIGHT_CONTEXT*
     disallows the tag if it is followed by the
     specified right context.

     Multiple conditions can be associated with the
     same pattern by union them together and by
     concatenating either "*AND_CHECK*" or "*OR_CHECK*
     to the front. In the former case all the conditions
     have to be satisfied; in the latter case at least
     one must be satisfied. For example, to indicate that
     neither "Miss Sara Lee" nor "Sarah Lee Jr" should
     be tagged as a company one can append to it both
     checks

     [ *AND_CHECK* [*NON_LEFT_CONTEXT* ...] |
                   [*NON_RIGHT_CONTEXT* ..]   ]

     Multiple conditions can be written in any order.
  */

  int end_pos = 0;
  STRING_BUFFERptr buffer = APPLY_out_buffer(cntxt);
  FILE *stream = APPLY_out_stream(cntxt);
  LAB_RINGptr input_ring = APPLY_input_ring(cntxt);
  LAB_VECTORptr out_vector = APPLY_out_vector(cntxt);
  id_type start_sym = ID_NO_SYMBOL;
  NETptr net = APPLY_net1(cntxt);
  OBJECTptr vect_prop;
  int print_non_match = !APPLY_extract_patterns_p(cntxt);

  reset_lab_ring(input_ring);

  vect_prop = get_value(net, VECTORIZE_N_ATTR);

  /* Sharing tails of arclist is not currently compatible with
     vectorization. This could be changed. */

  if (vect_prop)
    {
      if (!has_arc_vectors_p(net))
	{
	  vectorize_dense_states_of_net(net, OBJ_integer(vect_prop));
	  make_range_map(net, APPLY_side(cntxt) == LOWER ? LOWER : UPPER);

	  if (sigma_member(NET_sigma(net), ALTCHAIN_SYMBOL))
	    SHARE_make_shared_arclists(net, DONT_COPY);
	}
    }
  else if (sigma_member(NET_sigma(net), ALTCHAIN_SYMBOL))
    SHARE_make_shared_arclists(net, DONT_COPY);

  do {

    /* If separators are required, we check here that the current
       start symbol qualifies. */

    APPLY_in_pos(cntxt) = 0;
    /*
    fprintf(stdout, "Start location = %d\n", LAB_RING_start_pos(input_ring));
    */
    if (ok_to_start_with(start_sym, cntxt))
      longest_paths(NET_start_state(net), &end_pos, cntxt);

    /* The next match starts from where we ended up. */

    fetch_input_at_pos(&start_sym, end_pos, cntxt);

    if (end_pos == 0)
      {
	/* Failed to find anything. Accept the first input symbol as such
	   unless we are at the end of the file. */

	if (start_sym == ID_NO_SYMBOL)
	  {
	    /* Flush the remaining input. */

	    LAB_VECTOR_pos(out_vector) = count_symbols_in_ring(input_ring)-1;
	    APPLY_output_fn(cntxt)(cntxt);
	    print_string_buffer(buffer, stream);
	    initialize_string_buffer(buffer);

	    break;
	  }

	if (print_non_match)
	  symbol_name_to_buffer(start_sym, buffer);

	end_pos = 1;
      }
    else if (!print_non_match)
      {
	/* If we are extracting the match patterns, print a linefeed */

	fsm_fprintf(stream, "\n");

      }

    if (STRING_BUFFER_pos(buffer) > 0)
      {
	print_string_buffer(buffer, stream);

	initialize_string_buffer(buffer);
      }

    /* Move look-ahead symbols to the beginning of the input buffer */
    /*
    left_shift_lab_vector(lab_vector, end_pos);
    */

    if (start_sym == ID_NO_SYMBOL)
      reset_lab_ring(input_ring);
    else
      turn_lab_ring(input_ring, end_pos);
		    

    APPLY_in_pos(cntxt) = 0;
    end_pos = 0;
  } while (TRUE);

  /* Flush out any remaining content. */

  reset_lab_vector(APPLY_in_vector(cntxt));

  /*  fputc('\n', stream);  -- No reason for this. 2002-09024 LK */

  return(0);
}

/******************* END OF APPLY_PATTERNS ***********************/


static int apply_right_to_left(APPLYptr apply_context)
{
  ARCptr right_arc;
  id_type cur_sym, arc_sym;
  int pos;
  LAB_VECTORptr mid_vector = APPLY_mid_vector(apply_context);
  LAB_VECTORptr out_vector = APPLY_out_vector(apply_context);
  id_type (*in_fn)(id_type) = APPLY_in_fn(apply_context);
  id_type (*out_fn)(id_type) = APPLY_out_fn(apply_context);
  ALPHABETptr prev_sig,
    sig = label_to_binary(NET_sigma(APPLY_net2(apply_context)));
  int altchains_p = binary_sigma_member(sig, ALTCHAIN_SYMBOL);

  /* Need to update prev_sigma and sigma. */
  prev_sig = APPLY_prev_sigma(apply_context);
  APPLY_prev_sigma(apply_context) = APPLY_sigma(apply_context);
  APPLY_sigma(apply_context) = sig;

  /* The arcs of the right bimachine are ordered in the ascending
     order of symbol id's on the input side. Consequently, the
     while loop below quits when ARC_SYM becomes larger than CUR_SYM.
     The remaining arcs do not need to be examined. However, if
     ALTCHAIN_SYMBOL is present in the sigma, the arcs have been
     ordered by indegree and the test does not apply. */

  pos = lab_vector_size(mid_vector) - 1;
  lab_vector_element_at(&cur_sym, pos--, mid_vector);
  right_arc = STATE_arc_set(NET_start_state(APPLY_net2(apply_context)));

  reset_lab_vector(out_vector);

  while(right_arc) {
    arc_sym = in_fn(ARC_label(right_arc));

    if (arc_sym == cur_sym || OTHER_MATCH(cur_sym, arc_sym, apply_context))
      {
	if (ARC_label(right_arc) == OTHER)
	  append_to_lab_vector(cur_sym, out_vector);
	else
	  append_to_lab_vector(out_fn(ARC_label(right_arc)), out_vector);

	if (STATE_final(ARC_destination(right_arc)) && pos < 0)
	  {
	    APPLY_output_fn(apply_context)(apply_context);

	    /* restore prev_sigma and sigma. AK */
	    APPLY_sigma(apply_context) = APPLY_prev_sigma(apply_context);
	    APPLY_prev_sigma(apply_context) = prev_sig;

	    return(0);
	  }

	lab_vector_element_at(&cur_sym, pos--, mid_vector);
	right_arc = STATE_arc_set(ARC_destination(right_arc));
	
      }
    else if (arc_sym == ALTCHAIN_SYMBOL)
      right_arc = STATE_arc_set(ARC_destination(right_arc));
    else if (altchains_p || arc_sym < cur_sym)
      right_arc = next_arc(right_arc);
    else
      right_arc = NULL;
  }
  
  /* restore prev_sigma and sigma. AK */
  APPLY_sigma(apply_context) = APPLY_prev_sigma(apply_context);
  APPLY_prev_sigma(apply_context) = prev_sig;

  return(1);
}

static void check_bimachine_arc_order(NETptr left, NETptr right)
{
  if (!NET_sorted_arcs(left))
    {
      if (NET_upper_seq(left))
	reorder_arcs(left, down_left_bm_arc_comp_fn);
      else
	reorder_arcs(left, up_left_bm_arc_comp_fn);
				
      NET_sorted_arcs(left) = TRUE;
    }
 	
  if (!NET_sorted_arcs(right))
    {
      if (NET_upper_seq(right))
	reorder_arcs(right, down_right_bm_arc_comp_fn);
      else
	reorder_arcs(right, up_right_bm_arc_comp_fn);
				
      NET_sorted_arcs(right) = TRUE;
    }
}


static int wrong_apply_direction(NETptr net, int side)
{
  /* Returns 1 on an attempt to use a bimachine in a wrong direction.
     */

  if (side == UPPER)
    {
      if (NET_left_bimachine(net) && !NET_upper_seq(net))
	{
	  fsm_fprintf(stderr, "This is an upwards bimachine. ");
	  fsm_fprintf(stderr, "It cannot be applied downwards.\n");
	  return(1);
	}
    }
  else
    {
      if (NET_left_bimachine(net) && !NET_lower_seq(net))
	{
	  fsm_fprintf(stderr, "This is a downwards bimachine. ");
	  fsm_fprintf(stderr, "It cannot be applied upwards.\n");
	  return(1);
	}
    }

  return(0);
} /* wrong_apply_direction */


int apply_bimachine(APPLYptr apply_context)
{
  /* Applies the network in a depth-first manner to symbols parsed
     from the input string. The resulting strings are stored in
     the output buffer separated by newline characters. The input side
     must be UPPER or LOWER. */
      
  NETptr left = APPLY_net1(apply_context);
  NETptr right = APPLY_net2(apply_context);
  id_type (*in_fn)(id_type) = APPLY_in_fn(apply_context);
  id_type (*out_fn)(id_type) = APPLY_out_fn(apply_context);
  LAB_VECTORptr mid_vector = APPLY_mid_vector(apply_context);
  int side = APPLY_side(apply_context);

  STATEptr state;
  ALPHABETptr left_sig = label_to_binary(NET_sigma(left));
  id_type in_sym, arc_sym;
  ARCptr cur_arc, eps_arc;
  int altchains_p = binary_sigma_member(left_sig, ALTCHAIN_SYMBOL);
  int parse_error= NO_ERROR; /* error code for parsing the input string */
  
  if (! verify_bimachine_flags (left, right, side))
    {
      /* Returns with an error message on an attempt to apply a
	 bimachine in the wrong direction. */
      
      if (! wrong_apply_direction(left, side))
	return(1);
    }

  check_bimachine_arc_order(left, right);
	
  reset_lab_vector(mid_vector);

  state = NET_start_state(left);

  while (state)
    {
      do {
          parse_error= fetch_input_symbol(&in_sym, apply_context);
	 
        /* if (flag_diacritic_p(in_sym))
	  ...
	  Should we allow flag diacritics in the input?
	  If so, what should we do here?   */
      } while (in_sym != ID_NO_SYMBOL && flag_diacritic_p(in_sym));

      /* Test if the parsing failed: */
      if (parse_error == ERROR)
          break;
        
      if (in_sym == ID_NO_SYMBOL && STATE_final(state))
	return(apply_right_to_left(apply_context));

      cur_arc = STATE_arc_set(state);

      if (cur_arc && in_fn(ARC_label(cur_arc)) == DET_EPSILON)
	{
	  eps_arc = cur_arc;
	  cur_arc = next_arc(eps_arc);
	}
      else
	eps_arc = NULL;

      state = NULL;

      while(cur_arc) {

	arc_sym = in_fn(ARC_label(cur_arc));

	if (arc_sym == in_sym || OTHER_MATCH(in_sym, arc_sym, apply_context) ||
	    ANY_MATCH(in_sym, arc_sym, apply_context))
	  {
/*
	    if (APPLY_next_symbol_fn(apply_context) != symbol_from_in_vector)
	      append_to_lab_vector(in_sym, in_vect);
*/
	    if (ARC_label(cur_arc) == OTHER)
	      append_to_lab_vector(in_sym, mid_vector);
	    else
	      append_to_lab_vector(out_fn(ARC_label(cur_arc)), mid_vector);
	  
	    APPLY_in_pos(apply_context)++;

	    state = ARC_destination(cur_arc);
	    break;
	  }
	else if (arc_sym == ALTCHAIN_SYMBOL)
	  cur_arc = STATE_arc_set(ARC_destination(cur_arc));
	else
	  {
	    /* No match. Continue with the next arc unless we know
	       already that the remaining arcs will not match.
	       The arcs of a left bimachine are sorted in an
	       ascending input label order except for initial
	       DET_EPSILON arcs. However, if ALTCHAIN_SYMBOL
	       is present in the sigma, the arcs have beeen reordered
	       by indegree and the test is not applicable.
	    */
	  
	    if (altchains_p || arc_sym < in_sym || in_sym == ID_NO_SYMBOL)
	      cur_arc = next_arc(cur_arc);
	    else
	      cur_arc = NULL;
	  }
      }

      if (!state && eps_arc)
	{
	  /* No more arcs. Use the epsilon arc */

/*
	  if (APPLY_next_symbol_fn(apply_context) != symbol_from_in_vector)
	    append_to_lab_vector(in_sym, in_vect);
*/
	  append_to_lab_vector(out_fn(ARC_label(eps_arc)), mid_vector);
	  state = ARC_destination(eps_arc);
	}
    }
  return(1);
}
#endif /* SLIM_FST */

/* Part of the compose-apply algorithm:
   It supports standard nets, bimachines, labelSet reduced and Karttunen
   compacted networks.
*/
int apply_network(APPLYptr apply_context)
{
  int error_p = FALSE;

  NETptr net1 = APPLY_net1(apply_context);
  NETptr net2 = APPLY_net2(apply_context);

#ifndef SLIM_FST
  if (net2 && NET_right_bimachine(net2))
    {
      error_p = apply_bimachine(apply_context);
      APPLY_out_count(apply_context) = (error_p) ? 0 : 1;
    }
  else if (NET_reduced_labelset(net1))
    APPLY_out_count(apply_context) = apply_reduced_network(apply_context);
  else if (NET_arc_vector_heap(net1) != NULL)
  {
      /* Support of vectorized nets: */
      APPLY_out_count(apply_context)= apply_vectorized_network(apply_context);
  }
  else
#endif /* SLIM_FST */
  {
      if (NET_compacted(net1))
      {
          /* Init of the net traversal call-back functions:
             ---------------------------------------------- */
          APPLY_start_state_fn(apply_context)= start_state_compacted;
          APPLY_label_from_arc_fn(apply_context)= label_from_arc_location_comp;
          APPLY_next_arc_fn(apply_context)= next_arc_location_comp;
          APPLY_destination_fn(apply_context)= destination_from_arc_comp;
      }
      else
      {   /* Init of the net traversal call-back functions:
             ---------------------------------------------- */
          APPLY_start_state_fn(apply_context)= start_state_standard;
          APPLY_label_from_arc_fn(apply_context)= label_from_arc_location_stand;
          APPLY_next_arc_fn(apply_context)= next_arc_location_stand;
          APPLY_destination_fn(apply_context)= destination_from_arc_stand;
      } /* Not compacted */
      
      /* Using apply_mixed_network on virtual nets works but is
	 suboptimal because the virtual net gets fully expanded
	 immediately. This happens because the routine checks
	 the finality of all arc destinations, even when there
	 is no match. Asking for the finality of a virtual state
	 in general requires making it real.
      */
      if (NET_is_virtual(APPLY_net1(apply_context)) &&
	  APPLY_net2(apply_context) == NULL)
	APPLY_out_count(apply_context)= apply_standard_network(apply_context);
      else
#ifndef WORLD
	APPLY_out_count(apply_context) = apply_mixed_network(apply_context);
#else
      if (NET_compacted(net1))
      {
          /* Unsupported format in xfst */
          fsm_fprintf(stdout, "Unsupported format: can't process this network...\n");
      }
      else
      {
	APPLY_out_count(apply_context) = apply_standard_network(apply_context);
      }
#endif /* WORLD */
  }

  return(error_p);
} /* apply_network */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

static void switch_n1_and_n2(NETptr *n1, NETptr *n2, int error_p)
{
  NETptr net = *n2;
			
  *n2 = *n1;
  *n1 = net;
			
  if (error_p)
    {
      fsm_fprintf(stdout, "The two halves of an upwards bimachine ");
      fsm_fprintf(stdout, "should be on the stack\n");
      fsm_fprintf(stdout, "in the opposite order: RIGHT, LEFT.\n");
    }
}

int choose_nets_to_apply(APPLYptr apply_context, int inverse_p)
{
  /* If the top of the stack contains a bimachine, NET1 is set to
     the left half of the machine and NET2 to the right half of the
     machine. In the case of a downward bimachine, they are on the
     stack in the order LEFT, RIGHT. In the case of an upward
     bimachine the order on the stack is RIGHT, LEFT. If there is
     no bimachine, NET1 is set to the top network on the stack and
     NET2 becomes NULL.
  */
	
  NETptr n1, n2 = NULL;
  NVptr nv = APPLY_net_vector(apply_context);
  int nv_pos = APPLY_nv_pos(apply_context);
	
  if (nv_pos < 0 || nv_pos > NV_len(APPLY_net_vector(apply_context)))
    prog_error("choose_nets_to_apply",
	       "Net vector length %d, attempting to access position %d",
	       NV_len(APPLY_net_vector(apply_context)), nv_pos);
	
  n1 = NV_net(nv, nv_pos);
	
  if (inverse_p)
    {
      if (nv_pos > 0)
	n2 = NV_net(nv, nv_pos - 1);
    }
  else if (nv_pos + 1 < NV_len(nv))
    n2 = NV_net(nv, nv_pos + 1);
		
  if (n2)
    {
      if (NET_left_bimachine(n1))
	{
	  if (!NET_right_bimachine(n2))
	    n2 = NULL;
		
	}
      else if (NET_right_bimachine(n1))
	{
	  /* If inverse_p is TRUE, the nets should have been in
	     the opposite order. Otherwise the order is as expected
	     but we need to switch.  */

	  if (NET_left_bimachine(n2))
	    switch_n1_and_n2(&n1, &n2, inverse_p);
	  else
	    n2 = NULL;
	}
      else
	n2 = NULL;
    }
	
  /* In applying a net for the first time, make sure the flag bits
     are set correctly. */

  if (APPLY_net1(apply_context) != n1)
    check_for_flags(n1);

  APPLY_net1(apply_context) = n1;
  APPLY_net2(apply_context) = n2;
  APPLY_sigma(apply_context) = label_to_binary(NET_sigma(n1));

  if (n2)
    {
      if (wrong_apply_direction(n1, APPLY_side(apply_context)))
	return(1);
				
      if (inverse_p)
	APPLY_nv_pos(apply_context) -= 2;
      else
	APPLY_nv_pos(apply_context) += 2;
    }
  else
    if (inverse_p)
      APPLY_nv_pos(apply_context) -= 1;
    else
      APPLY_nv_pos(apply_context) += 1;
			
  return(0);
}
 

static void switch_input_and_output(APPLYptr apply_context)
{
  /* First save the current in_table, other_than_table, and
     other_than_loc_table in a temporary variable. */

  IO_SEQ_TABLEptr next_output_table = APPLY_input_table(apply_context);

  /* Reset table */

  reset_io_sequence_table(next_output_table);

  /* Make the current out_table to be the new in_table
     make the previous in_table to be the new out_table,
     make the next_other_than_table to be the new other_than_table,
     and the the next_other_than_loc_table the new other_than_loc_table. */

  APPLY_input_table(apply_context) = APPLY_output_table(apply_context);
  APPLY_output_table(apply_context) = next_output_table;
}
#endif /* SLIM_FST */

static void reinitialize_for_apply(APPLYptr apply_context)
{
  LAB_VECTOR_pos(APPLY_out_vector(apply_context)) = 0;
  LAB_VECTOR_pos(APPLY_mid_vector(apply_context)) = 0;
  VECTOR_pos(APPLY_arc_vector(apply_context)) = 0;
  VECTOR_pos(APPLY_state_vector(apply_context)) = 0;

  /* Position in input sequence: */
  APPLY_in_pos(apply_context) = 0;

  /* Number solution found: */
  APPLY_out_count(apply_context) = 0;

  /* Reset the solution_tree (if any) of the context. */
  if (apply_context->solution_tree != NULL)
  {
      /* Reset the whole tree, but keep the same pointer to the root
         state saved in APPLY_solution_tree: */
      reset_solution_tree(apply_context);
  }
      reset_io_sequence_table(APPLY_output_table(apply_context));
  
}

#ifndef SLIM_FST


static int apply_to_inputs(APPLYptr apply_context)
{
  int i;
  int error = FALSE;

  IO_SEQ_TABLEptr input_table;
  IO_SEQptr *array;

  switch_input_and_output(apply_context);
  
  input_table = APPLY_input_table(apply_context);
  array = IO_SEQ_TABLE_array(input_table);

  for (i = 0; i < IO_SEQ_TABLE_pos(input_table) && !error; i++)
    {
      APPLY_in_seq(apply_context) = array[i];
      APPLY_in_pos(apply_context) = 0;

      error = apply_network(apply_context);

      reset_io_sequence(APPLY_in_seq(apply_context));
    }

  return(error);
}


int compose_apply_networks(char *input, APPLYptr apply_context)
{
  int input_side = APPLY_side(apply_context);
  int final_out_side = APPLY_out_side(apply_context);
  int inverse_p = (input_side == LOWER);

  int len = NV_len(APPLY_net_vector(apply_context));
  
  int error;

  /* Apply the stack top down or bottom up depending on the input side. */
	
  if (inverse_p)
    APPLY_nv_pos(apply_context) = NV_len(APPLY_net_vector(apply_context)) - 1;
  else
    APPLY_nv_pos(apply_context) = 0;

  choose_nets_to_apply(apply_context, inverse_p);

  select_match_tables_for_apply(input_side);

  init_apply_context(apply_context);
  init_apply_net(apply_context);
  init_apply_to_string(input, apply_context);

  APPLY_output_fn(apply_context) = store_in_table;
	
  /* Only the last apply can be done in the BOTH_SIDES mode. */

  if (final_out_side == BOTH_SIDES)
    APPLY_out_side(apply_context) = LOWER;

  error = apply_network(apply_context);
  				
  if (error)
    return(error);

  /* If the input came from a string or a stream, the first
     io_sequence of the input table was used to store the
     symbols. Need to mark the table as used and switch
     the next_symbol_fn.
     */

  if (APPLY_next_symbol_fn(apply_context) != symbol_from_in_vector)
    {
      IO_SEQ_TABLE_pos(APPLY_input_table(apply_context))++;
      APPLY_next_symbol_fn(apply_context) = symbol_from_in_vector;
    }

  while (IO_SEQ_TABLE_pos(APPLY_output_table(apply_context)) > 0
	 && !error &&
	 APPLY_nv_pos(apply_context) < len &&
	 APPLY_nv_pos(apply_context) >= 0)
      {
	/* Remember last sigma to interpret OTHER correctly. */

	APPLY_prev_sigma(apply_context) = APPLY_sigma(apply_context);

	choose_nets_to_apply(apply_context, inverse_p);
	init_apply_net(apply_context);
	  
	error = apply_to_inputs(apply_context);
      }

  reset_io_sequence_table(APPLY_input_table(apply_context));

  return(error);
}

#if NOT_USED
static int valid_apply_context_vector(APPVECptr appvec, NVptr nv, int side_p)
{
  /* Verifies that the APPLY_CONTEXT_VECTOR matches the net vector.
     Exits if error. */

  APPLYptr *context = appvec->context;
  NETptr net, left, right;
  int i;

  if (side_p == LOWER) /*   if (side_p == UPPER) */
    nv_reverse (nv);  /* reverse again in the end */

  /* check context for every net or bimachine */

  for (i=0;  i< NV_len (nv);  )
    {
      net = left = right = NULL;

      /* get net or bimachine from net vector */

      if (NET_left_bimachine (NV_net (nv, i)))
	{
	  left = NV_net (nv, i++);

	  if ((i< NV_len (nv)) && (NET_right_bimachine (NV_net (nv, i))))
	    right = NV_net (nv, i++);
	  else
	    exit_with_error
	      ("net vector contains bimachine with only left net");
	}
      else if (NET_right_bimachine (NV_net (nv, i)))
	{
	  right = NV_net (nv, i++);

	  if ((i< NV_len (nv)) && (NET_left_bimachine (NV_net (nv, i))))
	    left = NV_net (nv, i++);
	  else
	    exit_with_error
	      ("net vector contains bimachine with only right net");
	}
      else    /* separate network */
	net = NV_net (nv, i++);

      if (left)
	{
	  if (!(APPLY_net1(*context) == left &&
	       APPLY_net2(*context) == right))
	    return(FALSE);
	}
      else if (!(APPLY_net1(*context) == net))
	return(FALSE);
      
      context++;
    }

  if (side_p == LOWER) /*   if (side_p == UPPER) */
    nv_reverse (nv);

  return (TRUE);
}
#endif /* NOT_USED */
#endif /* SLIM_FST */

void initialize_hyper_unit(APPLYptr apply_context, void (*final_output_fn)(void*))
{

/* Creates an APPLY_CONTEXT_VECTOR structure:
   it contains a vector of APPLY_CONTEXT structures, 1 per net in the cascade
   to be applied.
   The  APPLY_CONTEXT_VECTOR is stored in the APPLY_hyper_unit field of
   apply_context. */
    
  APPVECptr appvec = APPLY_hyper_unit(apply_context);
  NVptr nv = APPLY_net_vector(apply_context);
  int input_side = APPLY_side(apply_context);

  /* CP: Cancelled this part because the checking criterion seems unsafe. */
  /* if (appvec) */
  /*     { */
  /*       Check to make sure that the appvec is still up-to-date.  */ 
  /*       if (valid_apply_context_vector(appvec, nv, input_side)) */
  /* 	{ */
  /* 	  Appvec is still valid */ 

  /* 	  IO_SEQ_TABLEptr output_table = */
  /* 	    APPLY_output_table(APPVEC_last_context(appvec)); */
	  
  /* 	  reset_io_sequence_table(output_table); */
          
  /* 	  reset_solution_tree(APPVEC_last_context(appvec)); */ /* CP */ 

  /* 	  return; */
  /* 	} */
  /*       else */
  /* 	free_apply_context_vector(appvec); */
  /*     } */

  if (appvec)
      free_apply_context_vector(appvec);
  
  /* Need a new appvec. */

  appvec = create_apply_context_vector(nv, final_output_fn, input_side, apply_context);
  APPLY_hyper_unit(apply_context) = appvec;
}


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 


int recursive_compose_apply_networks(char *input, APPLYptr apply_context)
{

  int error = FALSE;
  APPVECptr appvec;
  APPLYptr first_context;

  initialize_hyper_unit(apply_context,  store_in_table /* store_in_a_tree */);
  appvec = APPLY_hyper_unit(apply_context);
  first_context = APPVEC_first_context(appvec);

  select_match_tables_for_apply(apply_context->side);
  init_apply_context(first_context);
  init_apply_net(first_context);
  init_apply_to_string(input, first_context);

  apply_netvector(appvec);
  
  return(error);
}
#endif /* SLIM_FST */

/******************************************************
 *
 * RECURSIVE DEPTH-FIRST APPLY FUNCTION FOR NET VECTORS
 *
 *   Andre Kempe, XRCE, May 1999
 */

static void recurse_depth_1st_apply (void* context);

APPVECptr create_apply_context_vector
  (NVptr nv, void (*final_output_fn)(void*), int side_p, APPLYptr toplevel_apply_context)
{
  /* Creates and returns an APPLY_CONTEXT_VECTOR for the given
     net vector.
     The nets in the vector must be in the order of application
     (regardless of side_p).
     The left and right part of a bimachine can occur in eighth order.
     Exits if error. */

  APPVECptr appvec;  APPLYptr context;
  NETptr net, left, right;  int i;

  if (side_p == LOWER) /*  if (side_p == UPPER) */
    nv_reverse (nv);  /* reverse again in the end */

  if (! (appvec = (APPVECptr) malloc (sizeof (APPVECtype))))
    not_enough_memory ("create_apply_context_vector");

  if (! (appvec->context = (APPLYptr *) malloc
	 (NV_len (nv) * sizeof (APPLYptr))))
    not_enough_memory ("create_apply_context_vector");

  appvec->len = 0;
  appvec->cur = 0;
  appvec->out_count = 0;
  
  /* make separate context for every net or bimachine */

  for (i=0;  i< NV_len (nv);  )
    {
      net = left = right = NULL;

      /* get net or bimachine from net vector */

      if (NET_left_bimachine (NV_net (nv, i)))
	{
	  left = NV_net (nv, i++);

	  if ((i< NV_len (nv)) && (NET_right_bimachine (NV_net (nv, i))))
	    right = NV_net (nv, i++);
	  else
	    exit_with_error
	      ("net vector contains bimachine with only left net");
	}
      else if (NET_right_bimachine (NV_net (nv, i)))
	{
	  right = NV_net (nv, i++);

	  if ((i< NV_len (nv)) && (NET_left_bimachine (NV_net (nv, i))))
	    left = NV_net (nv, i++);
	  else
	    exit_with_error
	      ("net vector contains bimachine with only right net");
	}
      else    /* separate network */
	net = NV_net (nv, i++);

      /* put net or bimachine (pointers) to NEW apply_context */
  
      context = init_apply_context (NULL);
      appvec->context [appvec->len ++] = context;
      APPLY_hyper_unit (context) = (void *) appvec;

      APPLY_side (context) = side_p;
      APPLY_out_side (context) = opposite_side (side_p);
      context->solution_tree= NULL; /* The solution tree will be stored in
                                       the last context ONLY: its root
                                       state will be allocated only when
                                       the function store_in_a_tree() is
                                       called. */
      /* we need to re-initialize after having set the sides and the
       solution_tree: */
      init_apply_context (context);
      /* propagate the obey_flags flag of the toplevel apply_context */
      APPLY_obey_flags_p(context) = APPLY_obey_flags_p(toplevel_apply_context);
      context->recursive_p = TRUE;
   
#ifndef SLIM_FST        
      if (left)
	{
	  if (! verify_bimachine_flags (left, right, side_p))
	    exit_with_error
	      ("net vector contains bimachine with bad flags");

	  APPLY_net1 (context) = left;
	  APPLY_net2 (context) = right;
	}
      else
#endif
	{
	  APPLY_net1 (context) = net;
	  APPLY_net2 (context) = NULL;

          if (NET_arc_vector_heap(net) != NULL)
          {
              /* Vectorized net detected:
              create the NET_uprange_map (or NET_downrange_map
              depending on the apply side). This is a key step to
              speed up the access to the destination array of the
              vectorized states: RANGE_items(range_map + in_sym)
              contains the list of labels having "in_sym" on the 
              apply input side. */
              make_range_map (net, APPLY_side(context));
          }
        }

      APPLY_sigma (context) = NET_sigma (APPLY_net1 (context));
      init_apply_net(context);

      if (appvec->len > 1)    /* not 1st context */
	APPLY_prev_sigma (context) =
	  APPLY_sigma (appvec->context [appvec->len - 2]);
      else
	APPLY_prev_sigma (context) = NULL;

      /* set input and output functions of NEW context */

      if (appvec->len == 1)    /* 1st context */
	APPLY_next_symbol_fn (context) = NULL;  /* must be set by user */
      else
	APPLY_next_symbol_fn (context) = symbol_from_in_vector;

      if (i >= NV_len (nv))    /* last context */
	APPLY_output_fn (context) = final_output_fn; 
      else
	APPLY_output_fn (context) = recurse_depth_1st_apply;
    }

  if (side_p == LOWER) /*   if (side_p == UPPER) */
    nv_reverse (nv);

  return (appvec);
}

void free_apply_context_vector (APPVECptr appvec)
{
  int i;

  for (i=0;  i< appvec->len;  i++)
  {
      /* All the contexts in the vector store in their hyper_unit field
         the address of the vector itself:
         reset it to NULL in order to avoid memory leak suspicions, (the
         vector itself -i.e. the container- will be released after). */
      APPVEC_context(appvec, i)->hyper_unit= NULL;
      free_apply_context(APPVEC_context(appvec, i));
  }

  free (appvec->context);
  free (appvec);
}
void reset_apply_context_vector (APPVECptr appvec)
{
  int i, vector_length= appvec->len;

  for (i=0;  i< vector_length;  i++)
    {
      LAB_VECTOR_pos (APPLY_in_vect(appvec->context [i])) = 0;
      reinitialize_for_apply (appvec->context [i]);
    }
}

/*-----------------------------------------------------------*/
static void recurse_depth_1st_apply (void* context)
{
  APPVECptr appvec;  IO_SEQptr in_seq, out_seq;
  /*  LAB_VECTORptr out_vector, in_vector;*/
  
  appvec = (APPVECptr) APPLY_hyper_unit ((APPLYptr) context);

  /* handle current recursion level [n] */

  if (APPVEC_cur (appvec) >= 0)  /* output exists on level [n] */
    {
      store_in_out_seq ((APPLYptr) context);
      out_seq = APPLY_out_seq ((APPLYptr) context);
    }
  else
    out_seq = NULL;

  /* go to next recursion level [n+1] */

  APPVEC_cur (appvec) ++;
  context = (void*) APPVEC_context (appvec, APPVEC_cur (appvec));

  /* Initialize! */

  APPLY_in_pos((APPLYptr) context) = 0;

  if (APPVEC_cur (appvec) >= APPVEC_len (appvec))  /* recursion error */
    prog_error ("recurse_depth_1st_apply",
		"recursion level %d with context vector of length %d",
		APPVEC_cur (appvec), APPVEC_len (appvec));

  if (out_seq)  /* point input[n+1] onto previous output[n] */
    {
      in_seq = APPLY_in_seq ((APPLYptr) context);  /* save original input vector[n] */
      APPLY_in_seq ((APPLYptr) context) = out_seq;
    }

  apply_network (context);  /* apply recursion level n+1 */

  /* Reset input sequence */

  reset_io_sequence(APPLY_in_seq((APPLYptr) context));

  if (APPVEC_cur (appvec) == APPVEC_len (appvec) - 1)  /* last level */
    APPVEC_out_count (appvec) += APPLY_out_count ((APPLYptr) context); /*count results*/

  if (out_seq)  /* all except 1st apply context */
    {                    /* set input[n+1] back to original vector */
      APPLY_in_seq ((APPLYptr) context) = in_seq;
      /*reinitialize_for_apply (context);*/ /* re-initialize current level n+1 */
    }

  APPVEC_cur (appvec) --;  /* return to initial recursion level n */
}


int apply_netvector (APPVECptr appvec)
{
  APPVEC_out_count (appvec) = 0;
  APPVEC_cur (appvec) = -1;        /* before 1st level */

  recurse_depth_1st_apply ((void*) APPVEC_first_context (appvec));

  return (APPVEC_out_count (appvec));
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

/*-----------------------------------------------------------*/
char *apply_netvector_to_string (APPVECptr appvec, char *str)
{
  char *result;

  reset_apply_context_vector (appvec);
  initialize_string_buffer
    (APPLY_out_buffer (APPVEC_last_context (appvec)));
  init_apply_to_string (str, APPVEC_first_context (appvec));

  apply_netvector (appvec);

  result = STRING_BUFFER_string
    (APPLY_out_buffer (APPVEC_last_context (appvec)));

  return (result);
}


/******************************************************
 *
 * SIMPLE APPLY FUNCTIONS FOR BIMACHINES
 *
 *   These functions don't use automatically growing buffers
 *   for strings or ID sequences. If you prefer such buffers,
 *   use the function  apply_bimachine()
 *
 *   Andre Kempe, XRCE, April 1999
 */

int apply_bimach_to_sequence
  (NETptr left, NETptr right, id_type *in_seq, id_type *out_seq,
   id_type *in_match, id_type *out_match)
{
  /* Applies the two sequential FSTs resulting from factorization to the
     input sequence of IDs ("in_seq"). Writes the result to "out_seq".
     Returns TRUE if "in_seq" is accepted and FALSE otherwise.

     BEFORE CALLING THIS FUNCTION YOU MUST (and it will not be verified):
     - update match tables
     - set pointers "in_match" and "out_match" onto UPPER_MATCH and
         LOWER_MATCH, according to the apply direction (down or up)
     - ensure that "in_seq" and "out_seq" are allocated large enough
     - ensure that "left" is a left-sequential and "right" a right-
         sequential FST
	 */

  STATEptr state;  ARCptr arc;  id_type insym, *in, *out, *out_end;

  /* apply 1st FST left to right */

  *out_seq = ID_EOS;  /* sequence end */

  for (state = NET_start_state (left), in = in_seq, out = out_seq+1;
       (*in != ID_EOS) || (! STATE_final (state));
       state = ARC_destination (arc))
    {
      if (*in != ID_EOS)
	{
	  insym = match_input_symbol (*in, NET_sigma (left));

	  for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
	    if ((in_match [ARC_label (arc)] >= insym)
		&& (in_match [ARC_label (arc)] != DET_EPSILON))
	      break;
	}
      else
	arc = NULL;  /* no more arc needed with real symbol */

      if ((arc) && (in_match [ARC_label (arc)] == insym))  /* arc found */
	{
	  *out ++ = match_output_symbol
	    (*in, out_match [ARC_label (arc)], ARC_label (arc));
	  in ++;
	}
      else  /* try 1st arc again : DET_EPSILON */
	{
	  arc = STATE_arc_set (state);
	  if ((arc) && (in_match [ARC_label (arc)] == DET_EPSILON))
	    *out ++ = out_match [ARC_label (arc)];
	  else  /* no arc found --> sequence not accepted */
	    return (FALSE);
	}
    }

  /* apply 2nd FST right to left */

  *out = ID_EOS;  /* sequence end */
  out_end = out --;

  for (state = NET_start_state (right), in = out;
       *in != ID_EOS;
       state = ARC_destination (arc))
    {
      insym = match_input_symbol (*in, NET_sigma (right));

      for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
	if (in_match [ARC_label (arc)] >= insym)
	  break;

      if ((arc) && (in_match [ARC_label (arc)] == insym))  /* arc found */
	{
	  *out = match_output_symbol
	    (*in, out_match [ARC_label (arc)], ARC_label (arc));

	  in --;
	  if (*out != EPSILON)
	    out --;
	}
      else
	return (FALSE);
    }

  if (! STATE_final (state))  /* sequence not accepted */
    return (FALSE);

  memmove (out_seq, out + 1, (size_t) ((char *)out_end - (char *)out));
  return (TRUE);
}

int apply_bimach_to_string
  (NETptr left, NETptr right, char *in_str, char *out_str, int side_p)
{
  /* Applies the two sequential FSTs resulting from factorization to
     the input string "in_str", and writes the result to "out_str".
     Returns TRUE if "in_str" is accepted and FALSE otherwise.

     Does not use automatically growing string buffers. If you prefer
     such buffers, use the function  apply_bimachine_to_string()
     instead. */

  PARSE_TBL parse_table;
  id_type *in_match, *out_match, *seq, out_seq [1000];

  if (! verify_bimachine_flags (left, right, side_p))
    {
      error_buffer ("put", "Not a bimachine or incorrectly marked");
      return (FALSE);
    }

  /* parse string --> ID sequence */

  if (! (parse_table = get_bimachine_parse_table (left, right, side_p)))
    {
      error_buffer ("put", "Parse table cannot be created");
      return (FALSE);
    }

  if (FST_parse_string (in_str, parse_table, FALSE, FALSE, FALSE, FALSE))
    return (FALSE);     /* not parsable */

  /* apply bimachine to ID sequence */

  update_match_tables();
  if (side_p == UPPER)
    {
      in_match = UPPER_MATCH;
      out_match = LOWER_MATCH;
    }
  else
    {
      in_match = LOWER_MATCH;
      out_match = UPPER_MATCH;
    }

  if (! apply_bimach_to_sequence
      (left, right, INPUT_SEQ, out_seq, in_match, out_match))
    return (FALSE);     /* not accepted */

  for (seq = out_seq;  *seq != ID_EOS;  seq ++)
    {
      FST_fat_to_thin (LABEL_atom (id_to_label (*seq)), out_str);
      out_str += strlen (out_str);
    }

  return (TRUE);
}

#endif /* SLIM_FST */
