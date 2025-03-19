/* $Id: bimach.c,v 1.4 2005/03/04 15:45:21 grondeau Exp $ */
/*************************************************************
 *
 *  FILE  "bimach.c"
 *
 *  Algorithms for factorizing different types of FSTs
 *
 *  March 1999 - March 2000
 *  Andre Kempe, MLTT, XRCE, Xerox Corporation
 *
 *************************************************************/


/*  IN A NETVECTOR, FACTORS ARE ALWAYS ARRANGED LIKE [ FA1, FA2 ]
    INDEPENDENTLY OF THE INPUT SIDE.
    IF NECESSARY, THE VECTOR SHOULD BE TURNED OUTSIDE OF THE ALGORITHM,
    E.G., IN THE FILE  iy-fs-ops.c .
    */


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "fst/types.h"
#include "fst/label.h"
#include "fst/context.h"
#include "fst/struct.h"
#include "fst/modify.h"
#include "fst/calculus.h"
#include "fst/heap.h"
#include "fst/copy-fsm.h"
#include "fst/error.h"
#include "fst/pars-tbl.h"
#include "fst/error.h"
#include "fst/mark.h"

#include "fst/bimach.h"
#include "fst/cfsm.h"
#include "fst/utf8_fatstr_util.h"

/************************************************************
 *
 * BASIC FUNCTIONS  for
 *
 *   - memory handling
 *   - net maintenance
 *   - alphabets and labels
 *   - symbol transduction tables
 *   - comment printing
 *   - label reduction
 *   - states and arcs
 *   - state vectors
 *   - state lists (state sets)
 *   - state list vectors
 *   - binary tree of states
 *   - arc chains
 */

/*===========================================================
  basic functions for memory handling
  */

static void clean_fct2_arc_vector (FCT2_ARCVECptr vec)
{
  vec->len = 0;

  vec->sub.len = 0;
  vec->sub.vector = NULL;

  vec->sub.flags.amb = FALSE;
  vec->sub.flags.beg = FALSE;
  vec->sub.flags.end = FALSE;
  vec->sub.flags.fork = FALSE;
  vec->sub.flags.other = FALSE;

  vec->sub.index.mid = 0;
}

static FCT2_ARCVECptr make_fct2_arc_vector ()
{
  FCT2_ARCVECptr vec;

  if (! (vec = (FCT2_ARCVECptr) malloc (sizeof (FCT2_ARCVECtype))))
    not_enough_memory ("make_fct2_arc_vector");

  vec->max = FCT2_ARC_VECTOR_INIT_LENGTH;

  if (! (vec->vector = (FCT2_ARCptr)
	 malloc (vec->max * sizeof (FCT2_ARCtype))))
    not_enough_memory ("make_fct2_arc_vector");

  clean_fct2_arc_vector (vec);
  vec->sub.index.beg = 0;

  return (vec);
}

static void enhance_fct2_arc_vector (FCT2_ARCVECptr vec)
{
  vec->max *= FCT2_ARC_VECTOR_REALLOC;

  if (! (vec->vector = (FCT2_ARCptr)
	 realloc (vec->vector, vec->max * sizeof (FCT2_ARCtype))))
    not_enough_memory ("enhance_fct2_arc_vector");
}

static void free_fct2_arc_vector (FCT2_ARCVECptr vec)
{
  free (vec->vector);
  free (vec);
}

/*-----------------------------------------------------------*/
static ARC_PATHptr alloc_arc_path (long max)
{
  ARC_PATHptr path;

  if (! (path = (ARC_PATHptr) malloc (sizeof (ARC_PATHtype))))
    not_enough_memory ("alloc_arc_path");

  path->max = max;
  path->len = 0;

  if (! (path->arcs = (ARCptr *) malloc (max * sizeof (ARCptr))))
    not_enough_memory ("alloc_arc_path");

  return (path);
}

static void enhance_arc_path (ARC_PATHptr path, long max)
{
  path->max = max;

  if (! (path->arcs = (ARCptr *) realloc (path->arcs, max * sizeof (ARCptr))))
    not_enough_memory ("enhance_arc_path");
}

static void free_arc_path (ARC_PATHptr path)
{
  free (path->arcs);
  free (path);
}

/*-----------------------------------------------------------*/
static LOOP_VECTORptr alloc_loop_vector (long num_loops, long num_lpstates)
{
  LOOP_VECTORptr vec;

  if (! (vec = (LOOP_VECTORptr) malloc (sizeof (LOOP_VECTORtype))))
    not_enough_memory ("alloc_loop_vector");

  vec->num_loops = num_loops;

  if (! (vec->loops = (ARC_LOOPptr)
	 malloc (num_loops * sizeof (ARC_LOOPtype))))
    not_enough_memory ("alloc_loop_vector");

  vec->path_buffer = NULL;

  vec->num_lpstates = num_lpstates;

  if (! (vec->lpstates = (STATE_LOOPptr)
	 malloc (num_lpstates * sizeof (STATE_LOOPtype))))
    not_enough_memory ("alloc_loop_vector");

  return (vec);
}

static void free_loop_vector (LOOP_VECTORptr vec)
{
  free (vec->loops);
  free (vec->lpstates);

  if (vec->path_buffer)
    free_arc_path (vec->path_buffer);

  free (vec);
}

/*-----------------------------------------------------------*/
static ALPHTRANSptr alloc_alphabet_transduction (int max)
{
  ALPHTRANSptr table;

  if (! (table = (ALPHTRANSptr) malloc (sizeof (ALPHTRANStype))))
    not_enough_memory ("alloc_alphabet_transduction");

  table->max = max;
  table->len = 0;

  if (! (table->symbols = (SYMTRANSptr) malloc (max * sizeof (SYMTRANStype))))
    not_enough_memory ("alloc_alphabet_transduction");

  return (table);
}

static void enhance_alphabet_transduction (ALPHTRANSptr table, int max)
{
  table->max = max;

  if (! (table->symbols = (SYMTRANSptr)
	 realloc (table->symbols, max * sizeof (SYMTRANStype))))
    not_enough_memory ("enhance_alphabet_transduction");
}

static void free_alphabet_transduction (ALPHTRANSptr table)
{
  free (table->symbols);
  free (table);
}

/*===========================================================
  basic functions for net maintenance
  */

static int check_number_of_nets (NVptr nv, int num_nets)
{
  if (NV_len (nv) != num_nets)
    {
      fprintf (stderr, "The algorithm expects %d rather than %hd input nets.",
	       num_nets, NV_len (nv));
      return (FALSE);
    }

  return (TRUE);
}

static void reset_client_cells (NETptr net)
{
  STATEptr state;

  for (state = NET_states (net);  state;  state = next_state (state))
    STATE_client_cell (state) = NULL;
}

static NETptr force_minimize (NETptr net, int copy_p, int compact_p)
{
  NET_deterministic (net) = FALSE;
  NET_pruned (net) = FALSE;
  NET_minimized (net) = FALSE;
  NET_epsilon_free (net) = FALSE;
  NET_loop_free (net) = FALSE;

  net = min_fsm (net, copy_p);

  if (compact_p)
    compact_sigma(net);

  return (net);
}

void set_bimachine_flags (NETptr left, NETptr right, int side_p)
{
  /* Sets the flags of both FSTs of a bimachine. */

  NET_left_bimachine (left) = TRUE;
  NET_right_bimachine (right) = TRUE;

  if (side_p == UPPER)
    {
      NET_upper_seq (left) = TRUE;
      NET_upper_seq (right) = TRUE;
    }
  else
    {
      NET_lower_seq (left) = TRUE;
      NET_lower_seq (right) = TRUE;
    }
}

static NETptr attach_boundaries
  (NETptr net, id_type left_bound, id_type right_bound)
{
  /* Concatenates the net with the boundary symbol. */

  NVtype nv;  NETptr nets [3];

  if (left_bound != EPSILON)
    sigma_add_to (NET_sigma (net), left_bound);

  if (right_bound != EPSILON)
    sigma_add_to (NET_sigma (net), right_bound);

  nv.len = 3;
  nv.nets = nets;

  nets [0] = symbol_fsm (left_bound);
  nets [1] = net;
  nets [2] = symbol_fsm (right_bound);
  net = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

  return (net);
}

/*===========================================================
  basic functions for alphabets and labels
  */

static int num_symbols_in_alph (ALPHABETptr alph)
{
  int i, num;

  if (ALPH_type (alph) == LABEL_VECTOR)
    num = ALPH_len (alph);
  else
    for (i = num = 0;  i < ALPH_len (alph);  i ++)
      if (ALPH_item (alph, i))
	num ++;

  return (num);
}

static id_type make_diacritic (char *pattern, int index)
{
  /* Returns the ID of a diacritic with the given name pattern
     and index. */

  char name [400];  id_type id;  int error = NO_ERROR;

  sprintf (name, pattern, index);
  error = FST_intern_literal (&id, name);

  if (error != NO_ERROR)
    handle_error ("Too many labels", "make_diacritic", error);

  return (id);
}

static id_type make_dir_pair_label (id_type in_id, id_type out_id, int side_p)
{
  /* Returns a directed pair label. */

  id_type pair_id;  int error = NO_ERROR;

  if (in_id == out_id)
    pair_id = in_id;
  else if (side_p == UPPER)
    error = make_pair_label (&pair_id, in_id, out_id);
  else
    error = make_pair_label (&pair_id, out_id, in_id);

  if (error != NO_ERROR)
    handle_error ("Too many labels", "make_dir_pair_label", error);

  return (pair_id);
}

static void assign_match_tables
  (id_type **in_match, id_type **out_match, int side_p)
{
  update_match_tables ();

  if (in_match)
    *in_match = (side_p == UPPER) ? UPPER_MATCH : LOWER_MATCH;

  if (out_match)
    *out_match = (side_p == UPPER) ? LOWER_MATCH : UPPER_MATCH;
}

static int split_dir_pair_label
  (id_type pair_id, id_type *in_id, id_type *out_id, int side_p)
{
  /* Splits a directed pair ID. Returns TRUE if it is a real pair
     and FALSE if it is a single symbol. */

  int is_pair = FALSE;

  if (in_id)
    {
      *in_id = (side_p == UPPER) ? upper_id (pair_id) : lower_id (pair_id);
      is_pair = (*in_id != pair_id);
    }

  if (out_id)
    {
      *out_id = (side_p == UPPER) ? lower_id (pair_id) : upper_id (pair_id);
      is_pair = (*out_id != pair_id);
    }

  return (is_pair);
}

static NETptr alphabet_to_net (ALPHABETptr alph)
{
  /* Returns a net that is the union of all strings consisting
     of one symbol from the alphabet. */

  NETptr net;  id_type i;

  net = null_fsm ();

  if (ALPH_type (alph) == LABEL_VECTOR)
    for (i = 0;  i < ALPH_len (alph);  i ++)
      net = net_union (net, symbol_fsm (ALPH_item (alph, i)));

  else
    for (i = 0;  i < ALPH_len (alph);  i ++)
      if (ALPH_item (alph, i))
	net = net_union (net, symbol_fsm (i));

  net = force_minimize (net, DONT_COPY, TRUE);
  return (net);
}

static int increasing_ids_comp_fn (id_type *a, id_type *b)
{
  return (*a - *b);
}

static void sort_alph_on_increasing_ids (ALPHABETptr alph)
{
  if (ALPH_type (alph) != LABEL_VECTOR)
    prog_error ("sort_alph_on_increasing_ids", "Bad alphabet type");

  if (ALPH_len (alph) < 2)
    return;

  qsort (ALPH_items (alph), ALPH_len (alph), sizeof (id_type),
	 (int (*)(const void *, const void *)) increasing_ids_comp_fn); 
}

/*-----------------------------------------------------------*/
static int contains_forbidden_symbol (ALPHABETptr sigma, id_type id)
{
  LABELptr label;  
  char name [200];

  label = id_to_label (id);

  if (LABEL_arity (label) != 1)
    prog_error ("contains_forbidden_symbol",
		"ID with arity %hi", LABEL_arity (label));

  if (sigma_member (sigma, id))
    {
      FST_fat_to_thin (LABEL_atom (label), name);
      error_buffer ("put", "Input net contains symbol '%s'", name);
      return (TRUE);
    }

  return (FALSE);
}

static int verify_fst_before_factorization (NETptr net)
{
  ALPHABETptr sigma = NET_sigma (net);  int error_p = FALSE;

  if (contains_forbidden_symbol (sigma, ALTCHAIN_SYMBOL))
    {
      error_buffer ("put", "Net contains ALTCHAIN_SYMBOL");
      error_p = TRUE;
    }

  if (contains_forbidden_symbol (sigma, BOUNDARY_SYMBOL))
    {
      error_buffer ("put", "Net contains BOUNDARY_SYMBOL");
      error_p = TRUE;
    }
  /*
  if (contains_forbidden_symbol (sigma, DET_EPSILON))
    {
      error_buffer ("put", "Net contains DET_EPSILON");
      error_p = TRUE;
    }
    */
  if (NET_reduced_labelset (net))
    {
      error_buffer ("put", "Net has reduced label set");
      error_p = TRUE;
    }

  return (! error_p);
}

static int verify_bmfst_before_unfactorization (NETptr net)
{
  ALPHABETptr sigma = NET_sigma (net);  int error_p = FALSE;

  if (contains_forbidden_symbol (sigma, ALTCHAIN_SYMBOL))
    {
      error_buffer ("put", "Net contains ALTCHAIN_SYMBOL");
      error_p = TRUE;
    }

  if (contains_forbidden_symbol (sigma, BOUNDARY_SYMBOL))
    {
      error_buffer ("put", "Net contains BOUNDARY_SYMBOL");
      error_p = TRUE;
    }

  if (NET_reduced_labelset (net))
    {
      error_buffer ("put", "Net has reduced label set");
      error_p = TRUE;
    }

  return (! error_p);
}

static int num_items_in_bin_alph (ALPHABETptr alph)
{
  int i, num;

  if (! alph)
    num = -1;
  else
    for (num = 0, i=0;  i< ALPH_len (alph);  i++)
      if (ALPH_item (alph, i))
	num ++;

  return (num);
}

static ALPHABETptr oneside_bin_alph (ALPHABETptr alph, int side_p)
{
  ALPHABETptr oneside;  
  uint16 i;
  id_type *match, max_id, id;

  if (ALPH_type (alph) != LABEL_VECTOR)
    prog_error ("oneside_bin_alph",
		"argument alphabet must be of type LABEL_VECTOR");

  assign_match_tables (&match, NULL, side_p);
  oneside = make_alph (LIM_id_count (LABEL_MAP), BINARY_VECTOR);

  for (i=0, max_id = 0;  i< ALPH_len (alph);  i++)
    {
      id = match [ALPH_item (alph, i)];
      ALPH_item (oneside, id) = TRUE;
      max_id = max_of (max_id, id);
    }

  ALPH_len (oneside) = max_id + 1;

  return (oneside);
}

static int has_constant_transduction (id_type inlab, ALPHABETptr labels, int side_p)
{
  /* Returns TRUE if inlab has always the same output label. */

  id_type *in_match;  int i, num;

  assign_match_tables (&in_match, NULL, side_p);

  for (num = 0, i=0;  i< ALPH_len (labels);  i++)
    if (in_match [ALPH_item (labels, i)] == inlab)
      {
	num ++;

	if (num > 1)
	  return (FALSE);  /* more than one output label */
      }

  return (TRUE);
}

/*-----------------------------------------------------------*/
static NETptr replace_label
  (NETptr net, id_type old, id_type new, int min_p, int compact_p)
{
  /* Replaces all labels in the net that have "old" ID by "new" ID.
     Does not look at component symbols of pairs but rather handles
     pairs as as whole. */

  STATEptr state;  ARCptr arc;

  for (state = NET_states (net);  state;  state = next_state (state))
    for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
      if (ARC_label (arc) == old)
	ARC_label (arc) = new;

  update_net_labels_and_sigma (net);

  if (min_p == DO_MINIMIZE)
    net = force_minimize (net, DONT_COPY, compact_p);

  return (net);
}

static NETptr replace_symbol
  (NETptr net, id_type old, id_type new, int min_p, int compact_p)
{
  STATEptr state;  ARCptr arc;
  id_type uid, lid;  int error, change_p;

  update_match_tables ();

  for (state = NET_states (net);  state;  state = next_state (state))
    for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
      {
	uid = UPPER_MATCH [ARC_label (arc)];
	lid = LOWER_MATCH [ARC_label (arc)];
	change_p = FALSE;

	if (uid == old)
	  {
	    uid = new;
	    change_p = TRUE;
	  }

	if (lid == old)
	  {
	    lid = new;
	    change_p = TRUE;
	  }

	if (change_p)
	  {
	    if (uid == lid)
	      ARC_label (arc) = uid;
	    else
	      {
		error = make_pair_label (&(ARC_label (arc)), uid, lid);

		if (error != NO_ERROR)
		  handle_error ("Too many labels", "replace_symbol", error);
	      }
	  }
      }

  update_net_labels_and_sigma (net);

  if (min_p == DO_MINIMIZE)
    net = force_minimize (net, DONT_COPY, compact_p);

  return (net);
}

static NETptr replace_oneside_symbol
  (NETptr net, id_type old, id_type new, int side_p, int min_p, int compact_p)
{
  /* Changes all old symbols to new symbols on the specified side
     of the FST (below called input_side). Remakes the alphabets. */

  STATEptr state;  ARCptr arc;  id_type *in_match, *out_match;

  assign_match_tables (&in_match, &out_match, side_p);

  for (state = NET_states (net);  state;  state = next_state (state))
    for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
      if (in_match [ARC_label (arc)] == old)
	ARC_label (arc) =
	  make_dir_pair_label (new, out_match [ARC_label (arc)], side_p);

  update_net_labels_and_sigma (net);

  if (min_p == DO_MINIMIZE)
    net = force_minimize (net, DONT_COPY, compact_p);

  return (net);
}

/*-----------------------------------------------------------*/
static void free_alph_if_is (ALPHABETptr *alph)
{
  if (*alph)
    {
      free_alph (*alph);
      *alph = NULL;
    }
}

static void derive_alphabets (NETptr net, ALPHABETptr add_sigma)
{
  /* Creates label and sigma alphabet, and adds all items from
     "add_sigma" (if this is not NULL) to the sigma alphabet.
     Sets arity. */

  ALPHABETptr labels, sigma;  
  uint16 i;

  free_alph_if_is (&(NET_labels (net)));
  free_alph_if_is (&(NET_sigma (net)));

  NET_labels (net) = labels = update_label_alphabet (net);
  NET_sigma (net) = sigma = label_sigma (net);

  if (add_sigma)
    {
      adjust_alph_size (sigma, ALPH_len (add_sigma));
      ALPH_len (sigma) = max_of (ALPH_len (sigma), ALPH_len (add_sigma));

      add_binary_alph
	(ALPH_items (sigma), ALPH_items (add_sigma), ALPH_len (add_sigma));
    }

  NET_arc_label_arity (net) = 1;
  for (i=0;  i< ALPH_len (labels);  i++)
    if (LABEL_arity (id_to_label (ALPH_item (labels, i))) == 2)
      {
	NET_arc_label_arity (net) = 2;
	break;
      }
}

static ALPHABETptr one_side_alph (ALPHABETptr labels, int side_p)
{
  /* Returns a (LABEL_VECTOR) alphabet with symbols that occur
     on the specified side, except EPSILON. */

  ALPHABETptr alph;  
  uint16 i;
  id_type lab, *input_match;

  alph = make_alph (LIM_id_count(LABEL_MAP) + 1, BINARY_VECTOR);
  assign_match_tables (&input_match, NULL, side_p);

  for (i=0;  i< ALPH_len (labels);  i++)
    {
      lab = input_match [ALPH_item (labels, i)];

      if (lab != EPSILON)
	ALPH_item (alph, lab) = TRUE;
    }

  alph = binary_to_label (alph);
  return (alph);
}

static int change_epsilon (id_type *new_id, id_type id, id_type new_eps)
{
  /* Match tables must be updated before.
     Replaces EPSILON by "new_eps". */

  int error= NO_ERROR;
  id_type uid, lid;

  uid = UPPER_MATCH [id];
  lid = LOWER_MATCH [id];

  if (uid == EPSILON)
    uid = new_eps;

  if (lid == EPSILON)
    lid = new_eps;

  if (uid == lid)
    *new_id = uid;
  else
    error = make_pair_label (new_id, uid, lid);

  return (error);
}

static int contains_symbol_on_side (NETptr fst, id_type sym, int side_p)
{
  /* Returns TRUE if "sym" is found on the specified side of the FST. */

  ALPHABETptr labels;  
  id_type *match;  
  int i;

  labels = NET_labels (fst);
  assign_match_tables (&match, NULL, side_p);

  for (i=0;  i< ALPH_len (labels);  i++)
    if (match [ALPH_item (labels, i)] == sym)
      return (TRUE);

  return (FALSE);
}

#if NOT_USED
static void change_side_det_epsilons (NETptr fst, int side_p)
{
  /* Changes all DET_EPSILONs to ordinary EPSILONs on the specified side
     of the FST (below called input_side). Remakes the alphabets. */

  STATEptr state;  ARCptr arc;  id_type *in_match, *out_match;

  assign_match_tables (&in_match, &out_match, side_p);

  for (state = NET_states (fst);  state;  state = next_state (state))
    for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
      if (in_match [ARC_label (arc)] == DET_EPSILON)
	ARC_label (arc) =
	  make_dir_pair_label (EPSILON, out_match [ARC_label (arc)], side_p);

  update_net_labels_and_sigma (fst);
}
#endif /* NOT_USED */

static ALPHABETptr reverse_alphabet (ALPHABETptr alph)
{
  ALPHABETptr rev;  
  int i, max_id = -1;

  for (i=0;  i< ALPH_len (alph);  i++)
    if (max_id < ALPH_item (alph, i))
      max_id = ALPH_item (alph, i);

  rev = make_alph (max_id + 1, LABEL_VECTOR);

  for (i=0;  i< ALPH_len (rev);  i++)
    ALPH_item (rev, i) = 0;

  for (i=0;  i< ALPH_len (alph);  i++)
    ALPH_item (rev, ALPH_item (alph, i)) = i;

  return (rev);
}

/*===========================================================
  basic functions for symbol transduction tables
  */

static int alphabet_comp_fn (ALPHABETptr a1, ALPHABETptr a2)
{
  /* Compares two sorted alphabets:
     1. on increasing length, and 2. on increasing IDs. */

  int i, cmp;

  if ((cmp = ALPH_len (a1) - ALPH_len (a2)))
    return (cmp);

  for (i = 0;  i < ALPH_len (a1);  i ++)
    if ((cmp = ALPH_item (a1, i) - ALPH_item (a2, i)))
      return (cmp);

  return (0);
}

static void insert_symbol_set_into_table
  (ALPHTRANSptr table, ALPHABETptr outsyms, id_type insym, int position)
{
  if (position > table->len)
    prog_error ("insert_symbol_set_into_table", "wrong position");

  if (table->len == table->max)
    enhance_alphabet_transduction
      (table, table->max * FCT2_TRANSDUCTION_REALLOC);

  if (position < table->len)
    memmove (table->symbols + (position + 1), table->symbols + position,
	     (table->len - position) * sizeof (SYMTRANStype));

  table->symbols [position].in_symbol = insym;
  table->symbols [position].out_symbols = copy_alphabet (outsyms);

  table->len ++;
}

static SYMTRANSptr find_output_symbolset_in_table
  (ALPHTRANSptr table, ALPHABETptr outsyms)
{
  /* Returns a pointer to the table entry with the same set of output
     symbols. If such a set does no exist yet, it is created.
     NOTE: "outsyms" will be modified but not consumed. */

  id_type insym;  int i, cmp;

  sort_alph (outsyms);

  for (i = 0, cmp = 1;  i < table->len;  i ++)
    if ((cmp = alphabet_comp_fn (table->symbols [i].out_symbols, outsyms))
	>= 0)  /* the set is either found now, or it does not exist yet */
      break;

  if (cmp)  /* the set does not exist yet */
    {
      insym = make_diacritic (FCT2_SHORT_AMB_DIACRITIC, table->len);
      insert_symbol_set_into_table (table, outsyms, insym, i);
    }

  return (table->symbols + i);
}

/*===========================================================
  basic functions for comment printing
  */

static void num_to_string (char *str, char *format, long num)
{
  /* Format, e.g.:  "%li state%s"
     where %li will be "num" and %s will be "" or "s" (plural). */

  sprintf (str, format, num, (num == 1) ? "" : "s");
}

static char *temp_net_info (NETptr net, int side_p)
{
  static char info [100];
  ALPHABETptr alph, in_syms, out_syms;
  long num_sigma, num_labels, num_insym, num_outsym;

  if (! net)
    *info = '\0';
  else
    {
      num_sigma = num_items_in_bin_alph (NET_sigma (net));

      if ((alph = NET_labels (net)))
	{
	  num_labels = ALPH_len (alph);

	  in_syms = oneside_bin_alph (alph, side_p);
	  num_insym = num_items_in_bin_alph (in_syms);
	  free_alph (in_syms);

	  out_syms = oneside_bin_alph (alph, opposite_side (side_p));
	  num_outsym = num_items_in_bin_alph (out_syms);
	  free_alph (out_syms);
	}
      else
	num_labels = num_insym = num_outsym = -1;

      num_to_string (info, "%li state%s", NET_num_states (net));
      num_to_string (info + strlen (info), ", %li arc%s", NET_num_arcs (net));

      if (num_labels >= 0)
	num_to_string (info + strlen (info), ", %li label%s", num_labels);
      if (num_sigma >= 0)
	num_to_string (info + strlen (info), ", %li symbol%s", num_sigma);

      if (num_insym >= 0)
	sprintf (info + strlen (info), " (%li in, %li out)",
		 num_insym, num_outsym);
    }

  return (info);
}

static void print_dot_for_n_states (NETptr det, long index, int verbose_p)
{
  static long states_per_dot = -1;

  if ((! verbose_p) || (states_per_dot == 0))
    return;

  if (det)    /* initialize */
    {
      states_per_dot = NET_num_states (det) / FCT2_TOTAL_DOTS;

      if (states_per_dot < FCT2_MIN_STATES_PER_DOT)
	{
	  states_per_dot = 0;
	  fprintf (stderr, "...");
	  fflush (stderr);
	}
    }
  else    /* print */
    {
      if (states_per_dot < 0)
	prog_error ("print_dot_for_n_states", "not initialized");

      if (index % states_per_dot == 0)
	{
	  fprintf (stderr, ".");
	  fflush (stderr);
	}
    }
}

#if NOT_USED
/*===========================================================
  basic functions for label reduction
  */

static ALPHABETptr define_singleton_ids (NETptr net, int side_p)
{
  /* Returns an alphabet with all labels that must become singleton
     classes in label reduction.
     NOTE: Ambiguity alignment must be done before !!!

     FIRST INTUITION (doesn't work, and I don't know why. AK) :
     - pair labels with DET_EPSILON as input symbol, and
     - pair labels that correspond with different alternative
       transductions of the same input symbol.

     SECOND INTUITION
     (seems to work, but is diff from result without reduction. AK) :
     - pair labels with DET_EPSILON as input symbol, and
     - pair labels that correspond with different alternative
       transductions of the same input symbol, and
     - all non-identity pairs.

     THIRD INTUITION (seems to work, and no strange effects. AK) :
     - pair labels with DET_EPSILON on either side, and
     - pair labels that correspond with different alternative
       transductions of the same input symbol, and
     - all non-identity pairs.
     */

  ALPHABETptr singletons, labels;
  uint16 i;
  id_type *in_match, *out_match, pairlab, inlab, outlab;

  assign_match_tables (&in_match, &out_match, side_p);
  singletons = make_alph (LIM_id_count (LABEL_MAP), BINARY_VECTOR);
  labels = NET_labels (net);

  for (i=0;  i< ALPH_len (labels);  i++)
    {
      pairlab = ALPH_item (labels, i);
      inlab = in_match [pairlab];
      outlab = out_match [pairlab];

      if ((inlab == DET_EPSILON) || (outlab == DET_EPSILON)
	  || (! has_constant_transduction (inlab, labels, side_p))
	  || (inlab != pairlab))
	ALPH_item (singletons, pairlab) = TRUE;
    }

  singletons = binary_to_label (singletons);
  return (singletons);
}

static ALPH_VECTORptr eqv_classes_for_factorization (NETptr net, int side_p)
{
  /* Returns ID equivalence classes for label set reduction before
     factorization, or NULL if all classes are singletons and no
     reduction can be done. */

  ALPH_VECTORptr eqv_classes;  ALPHABETptr singletons;

  eqv_classes = net_eqv_classes (net);
  singletons = define_singleton_ids (net, side_p);

  inforce_singleton_classes (eqv_classes, singletons);
  free_alph (singletons);

  if (av_len (eqv_classes) == ALPH_len (NET_labels (net)))
    {
      free_alph_vector (eqv_classes);
      eqv_classes = NULL;
    }

  return (eqv_classes);
}
#endif /* NOT_USED */

/*===========================================================
  basic functions for states and arcs
  */

static void turn_all_arcs (NETptr net)
{
  /* Turns all arcs. Uses client cells. */

  STATEptr state, dest;  ARCptr arc, nextarc;

  for (state = NET_states (net);  state;  state = next_state (state))
    STATE_client_cell (state) = NULL;

  for (state = NET_states (net);  state;  state = next_state (state))
    for (arc = STATE_arc_set (state);  arc;  arc = nextarc)
      {
	nextarc = next_arc (arc);
	dest = ARC_destination (arc);

	next_arc (arc) = STATE_client_cell (dest);
	STATE_client_cell (dest) = arc;
	ARC_destination (arc) = state;
      }

  for (state = NET_states (net);  state;  state = next_state (state))
    {
      STATE_arc_set (state) = STATE_client_cell (state);
      STATE_client_cell (state) = NULL;
    }
}

/*-----------------------------------------------------------*/
static ARCptr insert_sort_arc
  (STATEptr source, id_type id, STATEptr dest, NETptr net)
{
  ARCptr arc, newarc, prev;

  for (prev = NULL, arc = STATE_arc_set (source);
       arc;
       prev = arc, arc = next_arc (arc))
    {
      if (ARC_label (arc) > id)
	break;  /* not in list */

      if ((ARC_label (arc) == id) && (ARC_destination (arc) == dest))
	return (arc);  /* already in list */
    }

  newarc = make_arc (id, dest, net);

  if (prev)
    next_arc (prev) = newarc;
  else
    STATE_arc_set (source) = newarc;

  next_arc (newarc) = arc;
  STATE_deterministic (source) = FALSE;

  return (newarc);
}

/*-----------------------------------------------------------*/
static void **save_state_client_cells (NETptr net)
{
  void **vec;  STATEptr state;  long i;

  if (! (vec = (void **) malloc (NET_num_states (net) * sizeof (void *))))
    not_enough_memory ("save_state_client_cells");

  for (state = NET_states (net), i=0;
       (state) && (i< NET_num_states (net));
       state = next_state (state), i++)
    vec [i] = STATE_client_cell (state);

  if ((state) || (i< NET_num_states (net)))
    prog_error ("save_state_client_cells", "wrong state number");

  return (vec);
}

static void restore_state_client_cells (void **vec, NETptr net)
{
  STATEptr state;  long i;

  for (state = NET_states (net), i=0;
       (state) && (i< NET_num_states (net));
       state = next_state (state), i++)
    STATE_client_cell (state) = vec [i];

  if ((state) || (i< NET_num_states (net)))
    prog_error ("restore_state_client_cells", "wrong state number");
}

/*-----------------------------------------------------------*/
static void set_state_numbers (NETptr net)
{
  /* Writes a (long int) number on the client cell of every state.
     The start state gets number 0. */

  STATEptr state;  long num = 1;

  for (state = NET_states (net);  state;  state = next_state (state))
    {
      if (state == NET_start_state (net))
	STATE_client_cell (state) = (void *) 0;
      else
	STATE_client_cell (state) = (void *) (num ++);
    }
}

/*-----------------------------------------------------------*/
static int increasing_upper_id_and_dest_addr_comp_fn
  (const void *a1, const void *a2)
{
  /* Sorts labels numerically based on
     1. the upper id of the label without any exception, and
     2. the destinatyion state address. */

  int cmp;

  if ((cmp = upper_id (ARC_label (* (ARCptr *) a1))
       - upper_id (ARC_label (* (ARCptr *) a2))))
    return (cmp);

  if (ARC_destination (* (ARCptr *) a1) < ARC_destination (* (ARCptr *) a2))
    return (-1);
  if (ARC_destination (* (ARCptr *) a1) > ARC_destination (* (ARCptr *) a2))
    return (1);

  return (0);
}

static int increasing_lower_id_and_dest_addr_comp_fn
  (const void *a1, const void *a2)
{
  /* Sorts labels numerically based on
     1. the lower id of the label without any exception, and
     2. the destinatyion state address. */

  int cmp;

  if ((cmp = lower_id (ARC_label (* (ARCptr *) a1))
       - lower_id (ARC_label (* (ARCptr *) a2))))
    return (cmp);

  if (ARC_destination (* (ARCptr *) a1) < ARC_destination (* (ARCptr *) a2))
    return (-1);
  if (ARC_destination (* (ARCptr *) a1) > ARC_destination (* (ARCptr *) a2))
    return (1);

  return (0);
}

/*===========================================================
  basic functions for state vectors
  */

static STATEVECptr alloc_state_vector (long max)
{
  STATEVECptr vec;

  if (! (vec = (STATEVECptr) malloc (sizeof (STATEVECtype))))
    not_enough_memory ("alloc_state_vector");

  vec->max = max;
  vec->len = 0;

  if (! (vec->vec = (STATEptr *) malloc (max * sizeof (STATEptr))))
    not_enough_memory ("alloc_state_vector");

  return (vec);
}

static void free_state_vector (STATEVECptr vec)
{
  free (vec->vec);
  free (vec);
}


/*===========================================================
  basic functions for state lists (state sets)
  */

static void free_state_list (STATELISTptr list, HEAPptr heap)
{
  STATELISTptr next;

  for (  ;  list;  list = next)
    {
      next = list->next;
      free_cell (list, heap);
    }
}

static STATELISTptr add_state_to_sorted_list
  (STATEptr state, STATELISTptr list, HEAPptr heap)
{
  STATELISTptr prev, cur, new;

  /* see if state is already in sorted list */

  for (prev = NULL, cur = list;  cur;  prev = cur, cur = cur->next)
    {
      if (cur->state == state)
	return (list);    /* nothing to do */
      else if (cur->state > state)
	break;
    }

  /* add state */

  new = (STATELISTptr) cell_alloc (heap);
  new->state = state;
  new->next = cur;

  if (prev)
    prev->next = new;
  else
    list = new;

  return (list);
}

static STATEptr take_state_from_list (STATELISTptr *list, HEAPptr heap)
{
  STATEptr state;  STATELISTptr tail;

  state = (*list)->state;
  tail = (*list)->next;

  free_cell (*list, heap);
  *list = tail;

  return (state);
}

static STATELISTptr list_initial_state (NETptr fst, HEAPptr heap)
{
  /* Returns a list with the initial state only. */

  STATELISTptr list;

  list = add_state_to_sorted_list (NET_start_state (fst), NULL, heap);
  return (list);
}

static STATELISTptr list_final_states (NETptr fst, HEAPptr heap)
{
  /* Returns a sorted list with all final states. */

  STATELISTptr list = NULL;  STATEptr state;

  for (state = NET_states (fst);  state;  state = next_state (state))
    if (STATE_final (state))
      list = add_state_to_sorted_list (state, list, heap);

  return (list);
}

static int contains_initial_state (STATELISTptr list, STATEptr init)
{
  /* Returns TRUE if the list contains the "init" state. */

  for (  ;  list;  list = list->next)
    if (list->state == init)
      return (TRUE);

  return (FALSE);
}

static int contains_final_states (STATELISTptr list)
{
  /* Returns TRUE if the list contains final states. */

  for (  ;  list;  list = list->next)
    if (STATE_final (list->state))
      return (TRUE);

  return (FALSE);
}

static int compare_state_lists (STATELISTptr a, STATELISTptr b)
{
  /* Returns 0 if lists are equivalent, -1 if a<b, and 1 if a>b. */

  for (  ;  (a) && (b);  a = a->next, b = b->next)
    {
      if (a->state < b->state)
	return (-1);
      if (a->state > b->state)
	return (1);
    }

  if (a)    /* a not finished, b finished --> a>b */
    return (1);
  if (b)    /* a finished, b not finished --> a<b */
    return (-1);

  return (0);    /* both a and b finished --> a==b */
}


/*===========================================================
  basic functions for state list vectors
  */

static SLVECptr alloc_state_list_vector (ALPHABETptr in_alph)
{
  SLVECptr vec;

  if (! (vec = (SLVECptr) malloc (sizeof (SLVECtype))))
    not_enough_memory ("alloc_state_list_vector");

  vec->id2idx = reverse_alphabet (in_alph);
  vec->max = ALPH_len (vec->id2idx);
  vec->len = vec->max;

  if (! (vec->vec = (STATELISTptr *) malloc (vec->max * sizeof (STATELISTptr))))
    not_enough_memory ("alloc_state_list_vector");

  return (vec);
}

static void free_state_list_vector (SLVECptr vec)
{
  free_alph (vec->id2idx);
  free (vec->vec);
  free (vec);
}

static void clean_state_list_vector (SLVECptr vec)
{
  memset (vec->vec, 0, vec->len * sizeof (STATELISTptr));
}


/*===========================================================
  basic functions for binary tree of states
  */

static SBTREEptr *find_bintree_state_with_same_list
  (SBTREEptr *bintree, STATELISTptr ref_list)
{
  /* Returns a pointer to an existing bintree cell with a state
     that has the required list, or to the place where such a
     cell should be attached. */

  STATELISTptr state_list;

  while (*bintree)
    {
      state_list = (STATELISTptr) STATE_client_cell ((*bintree)->state);
      switch (compare_state_lists (state_list, ref_list))
	{
	case 0:    /* state_list == ref_list */
	  return (bintree);
	  break;
	case -1:   /* state_list < ref_list */
	  bintree = &((*bintree)->next);
	  break;
	case 1:    /* state_list > ref_list */
	  bintree = &((*bintree)->prev);
	  break;
	}
    }

  return (bintree);    /* needed for the compiler */
}

static STATEptr make_bintree_state
  (SBTREEptr *subtree, STATELISTptr statelist, NETptr net, HEAPptr heap,
   STATEptr orig_startstate, int reverse_p)
{
  SBTREEptr cell;  STATEptr state;

  state = make_state (NULL, net);
  STATE_client_cell (state) = statelist;

  if (reverse_p)
    STATE_final (state) =
      contains_initial_state (statelist, orig_startstate);
  else
    STATE_final (state) = contains_final_states (statelist);

  cell = (SBTREEptr) cell_alloc (heap);
  cell->state = state;
  cell->prev = NULL;
  cell->next = NULL;

  *subtree = cell;

  return (state);
}

/*===========================================================
  basic functions for arc chains
  */

static ARCCHVECptr alloc_arc_chain_vector ()
{
  ARCCHVECptr vec;

  vec = (ARCCHVECptr) malloc (sizeof (ARCCHVECtype));
  vec->max = ARC_CHAIN_VECTOR_LENGTH;
  vec->len = 0;
  vec->vec = (ARCCHAINptr) malloc (vec->max * sizeof (ARCCHAINtype));
  vec->id_cell_heap = init_heap
    (sizeof (IDLISTtype), ARC_CHAIN_VECTOR_HEAP_BLOCK, "ID list cells");

  return (vec);
}

static void increase_arc_chain_vector (ARCCHVECptr vec)
{
  vec->max *= ARC_CHAIN_VECTOR_REALLOC;
  vec->vec = (ARCCHAINptr) realloc (vec->vec, vec->max * sizeof (ARCCHAINtype));
}

static void clean_arc_chain_vector (ARCCHVECptr vec)
{
  vec->len = 0;
  reset_heap (vec->id_cell_heap);
}

static void free_arc_chain_vector (ARCCHVECptr vec)
{
  free_heap (vec->id_cell_heap);
  free (vec->vec);
  free (vec);
}


/************************************************************
 *
 * MAKE DETERMINISTIC SIMPLE NETWORK FROM THE INPUT SIDE OF AN FST.
 *
 *   All states of this net are linked with sets (lists) of states
 *   in the FST.
 */

static int iterate_add_states_to_all_lists
  (ARCptr arc, SLVECptr dest_vector, HEAPptr heap, ARCptr *arc_vec, id_type *in_match)
{
  /* Adds to all list of the dest_vector, states that can be reached from the
     arc set starting with "arc" by matching any label.
     Epsilon arcs are traversed only before real symbols, never after.
     Returns FALSE if recursion limit exceded, and TRUE otherwise. */

  STATELISTptr *list;  int depth;  id_type arc_lab;

  depth = 0;

  /* iterate */

  while (arc)
    {
      /* ---- check arc label */
      if ((arc_lab = in_match [ARC_label (arc)]) == EPSILON)
	{
	  if (depth >= BIMACH_EPSILON_RECURSE_LIMIT)
	    {
	      error_buffer
		("put", "Ambiguous input FST. Epsilon recursion limit (%d) exeeded.",
		 BIMACH_EPSILON_RECURSE_LIMIT);
	      return (FALSE);
	    }

	  arc_vec [depth ++] = next_arc (arc);
	  arc = STATE_arc_set (ARC_destination (arc));
	}
      else    /* not EPSILON */
	{
	  list = dest_vector->vec + ALPH_item (dest_vector->id2idx, arc_lab);
	  *list = add_state_to_sorted_list (ARC_destination (arc), *list, heap);
	  arc = next_arc (arc);
	}

      /* ---- handle end of arc list */
      while ((! arc) && (depth > 0))
	arc = arc_vec [-- depth];
    }

  return (TRUE);
}

static int list_all_reachable_states
  (STATELISTptr source, SLVECptr dest_vector, HEAPptr heap,
   ARCptr *arc_vec, id_type *in_match)
{
  /* Fills the dest_vector with sorted lists of destination states
     that can be reached from any of the states in the source_list.
     This is done for ALL input symbols.
     Epsilon arcs are traversed only before real symbols, never after.
     Returns FALSE if recursion limit exceded, and TRUE otherwise. */

  for (  ;  source;  source = source->next)
    if (! iterate_add_states_to_all_lists
	(STATE_arc_set (source->state), dest_vector, heap, arc_vec, in_match))
      return (FALSE);

  return (TRUE);
}

static NETptr make_determ_input_net
  (NETptr fst, HEAPptr statelist_heap, id_type *in_match,
   int side_p, int reverse_p)
{
  /* Returns a deterministic simple net corresponding to the input side
     of the FST. Every state of this hyper net will be linked to the
     corresponding set of states in the original FST.
     If reverse_p==TRUE then the hyper net of the reversed FST is built.
     Epsilon arcs are traversed only before real symbols, never after.
     Returns NULL on failure (epsilon recursion limit exceded). */

  HEAPptr bintree_heap;  SBTREEptr *bintree;  SLVECptr dest_vector;
  STATELISTptr source_list, dest_list, newstates;
  NETptr hyper_net;
  STATEptr source_state, dest_state;  
  ARCptr arc, *arc_vec;
  ALPHABETptr input_alph;  
  int i;  
  id_type inlab;

  /* prepare */

  bintree_heap = init_heap
    (sizeof (SBTREEtype), STATE_BINTREE_BLOCK_SIZE, "Bin.tree cells");

  input_alph = one_side_alph (NET_labels (fst), side_p);
  dest_vector = alloc_state_list_vector (input_alph);

  reset_client_cells (fst);
  hyper_net = make_network ();

  if (! (arc_vec = (ARCptr *) malloc
	 (BIMACH_EPSILON_RECURSE_LIMIT * sizeof (ARCptr))))
    not_enough_memory ("make_determ_input_net");

  /* handle initial state */

  if (reverse_p)
    {
      turn_all_arcs (fst);
      source_list = list_final_states (fst, statelist_heap);
    }
  else
    source_list = list_initial_state (fst, statelist_heap);

  bintree = (SBTREEptr *) (&(STATE_client_cell (source_list->state)));

  NET_start_state (hyper_net) = source_state = make_bintree_state
    (bintree, source_list, hyper_net, bintree_heap, NET_start_state (fst), reverse_p);

  newstates = add_state_to_sorted_list (source_state, NULL, statelist_heap);

  /* traverse FST */

  while (newstates)    /* some states have not been handled yet */
    {
      source_state = take_state_from_list (&newstates, statelist_heap);
      STATE_deterministic (source_state) = FALSE;
      source_list = STATE_client_cell (source_state);

      clean_state_list_vector (dest_vector);

      if (! list_all_reachable_states
	  (source_list, dest_vector, statelist_heap, arc_vec, in_match))
	return (NULL);    /* too deep epsilon recursion */

      for (i=0;  i< ALPH_len (input_alph);  i++)
	{
	  inlab = ALPH_item (input_alph, i);
	  dest_list = dest_vector->vec [ALPH_item (dest_vector->id2idx, inlab)];

	  if (! dest_list)    /* no states reachable */
	    continue;

	  /* ---- consider list of reachable states */
	  bintree = (SBTREEptr *) (&(STATE_client_cell (dest_list->state)));
	  bintree = find_bintree_state_with_same_list (bintree, dest_list);

	  if (*bintree)    /* state with this list exists already */
	    {
	      dest_state = (*bintree)->state;
	      free_state_list (dest_list, statelist_heap);
	    }
	  else             /* state with this list does not exist yet */
	    {
	      dest_state = make_bintree_state
		(bintree, dest_list, hyper_net, bintree_heap,
		 NET_start_state (fst), reverse_p);

	      newstates =
		add_state_to_sorted_list (dest_state, newstates, statelist_heap);
	    }

	  arc = make_arc (inlab, dest_state, hyper_net);
	  push_arc (arc, source_state);
	}
    }

  /* finish */

  free_alph (input_alph);
  free_state_list_vector (dest_vector);
  free (arc_vec);
  free_heap (bintree_heap);

  if (reverse_p)
    turn_all_arcs (fst);

  derive_alphabets (hyper_net, NET_sigma (fst));
  NET_loop_free (hyper_net) = FALSE;

  reset_client_cells (fst);

  return (hyper_net);
}


/************************************************************
 *
 * UNFOLD THE FST LOCALLY.
 *
 *   Duplicate branches of an FST F, so that if a deterministic
 *   automaton D, that corresponds to the input side of F, is
 *   created and all states of D are linked to sets of states
 *   of F then every state of F is in exactly one set.
 *   With a general FST this is not the case.
 */


/************************************************************
 *
 * ALIGN AMBIGUITY BY PUSHING IT TO EQUAL DISTANCE
 *
 */

static NETptr make_state_pairs
  (NETptr hyper_net, HEAPptr statelist_heap, HEAPptr pair_heap)
{
  /* Replaces all state lists in the hyper net by pair lists. */

  NETptr twin;  STATEptr hstate;
  STATELISTptr state_list, state_cell, state_next;
  PRSTLISTptr pair_list, pair_cell, pair_prev;

  twin = make_network ();

  for (hstate = NET_states (hyper_net);  hstate;  hstate = next_state (hstate))
    {
      state_list = (STATELISTptr) STATE_client_cell (hstate);
      pair_list = pair_prev = NULL;

      for (state_cell = state_list;  state_cell;  state_cell = state_next)
	{
	  pair_cell = (PRSTLISTptr) cell_alloc (pair_heap);

	  pair_cell->state1 = state_cell->state;
	  pair_cell->state2 = make_state (NULL, twin);
	  pair_cell->next = NULL;

	  /* ---- handle new twin state */

	  STATE_deterministic (pair_cell->state2) = FALSE;
	  STATE_final (pair_cell->state2) = STATE_final (pair_cell->state1);

	  if (hstate == NET_start_state (hyper_net))
	    {
	      if (state_list->next)    /* more than one state in set */
		prog_error ("make_state_pairs", "#1");

	      NET_start_state (twin) = pair_cell->state2;
	    }

	  /* ---- connect new twin state */

	  if (pair_prev)
	    pair_prev->next = pair_cell;
	  else
	    pair_list = pair_cell;

	  state_next = state_cell->next;
	  pair_prev = pair_cell;

	  free_cell (state_cell, statelist_heap);
	}

      STATE_client_cell (hstate) = (void *) pair_list;
    }

  return (twin);
}

/*-----------------------------------------------------------*/
static void save_arc_chain
  (ARCCHVECptr acvec, STATEptr source, STATEptr dest,
   PRSTLISTptr dest_list, id_type *id_seq, int len)
{
  /* Writes an arc chain into the vector. */

  PRSTLISTptr dest_cell;
  ARCCHAINptr chain;  IDLISTptr cell, prev_cell;  int i;

  /* search for destination state */

  for (dest_cell = dest_list;  dest_cell;  dest_cell = dest_cell->next)
    if (dest_cell->state1 == dest)
      break;

  if (! dest_cell)
    prog_error ("save_arc_chain", "#1");

  dest = dest_cell->state2;    /* set to twin destination */

  /* save arc chain */

  if (acvec->len == acvec->max)
    increase_arc_chain_vector (acvec);

  chain = acvec->vec + (acvec->len ++);

  chain->source = source;
  chain->dest = dest;
  chain->len = len;

  for (prev_cell = NULL, i=0;  i< len;  prev_cell = cell, i++)
    {
      cell = (IDLISTptr) cell_alloc (acvec->id_cell_heap);

      cell->id = id_seq [i];
      cell->next = NULL;

      if (prev_cell)
	prev_cell->next = cell;
      else
	chain->chain = cell;
    }
}

static void recurse_collect_arc_chains
  (ARCCHVECptr acvec, STATEptr source, PRSTLISTptr dest_list, ARCptr arc,
   id_type inlab, id_type *in_match, id_type *id_seq, int id_pos)
{
  /* Recursion depth (and id_pos) overflow is not checked; this has been done
     in  iterate_add_state_to_list()  already. */

  id_seq [id_pos] = ARC_label (arc);  /* save arc ID */

  if (in_match [ARC_label (arc)] == inlab)
    save_arc_chain
      (acvec, source, ARC_destination (arc), dest_list, id_seq, id_pos + 1);

  else if (in_match [ARC_label (arc)] == EPSILON)
    for (arc = STATE_arc_set (ARC_destination (arc));  arc;  arc = next_arc (arc))
      recurse_collect_arc_chains
	(acvec, source, dest_list, arc, inlab, in_match, id_seq, id_pos + 1);
}

/*-----------------------------------------------------------*/
static void insert_twin_arc_chain
  (NETptr twin_net, STATEptr source, STATEptr dest, id_type *idseq, int seqlen)
{
  STATEptr state;  ARCptr arc;

  /* 1st see what already exists */

  while (seqlen > 0)
    {
      for (arc = STATE_arc_set (source);  arc;  arc = next_arc (arc))
	if (ARC_label (arc) == *idseq)    /* matching arc found */
	  break;

      if (! arc)    /* no matching arc found */
	break;

      source = ARC_destination (arc);
      idseq ++;
      seqlen --;
    }

  /* then add what does not exist yet */

  while (seqlen > 0)
    {
      if (seqlen == 1)    /* last ID */
	state = dest;
      else
	state = make_state (NULL, twin_net);

      arc = make_arc (*idseq, state, twin_net);
      push_arc (arc, source);
      STATE_deterministic (source) = FALSE;

      source = state;
      idseq ++;
      seqlen --;
    }
}

static void copy_arc_chains_to_twin_net
  (NETptr twin_net, ARCCHVECptr acvec, id_type *id_seq)
{
  /* Copies all arc chains, that belong to ONE hyper net state,
     to the twin net. */

  IDLISTptr cell;  
  STATEptr source, dest;  
  int i, j, maxlen, difflen;
  id_type new_id;

  if (! acvec->len)
    return;

  /* find maximal chain length */

  for (maxlen = 0, i=0;  i< acvec->len;  i++)
    if (maxlen < acvec->vec [i].len)
      maxlen = acvec->vec [i].len;

  /* copy every chain to twin net */

  for (i=0;  i< acvec->len;  i++)
    {
      source = acvec->vec [i].source;    /* twin source */
      dest = acvec->vec [i].dest;      /* twin destination */

      if ((difflen = maxlen - acvec->vec [i].len) < 0)
	prog_error ("copy_arc_chains_to_twin_net", "#1");

      for (j=0;  j< difflen;  )
	id_seq [j++] = DET_EPSILON;

      for (cell = acvec->vec [i].chain;  cell;  cell = cell->next)
	{
	   change_epsilon (&new_id, cell->id, DET_EPSILON);
	   id_seq [j++] = new_id;
	}

      if (j != maxlen)
	prog_error ("copy_arc_chains_to_twin_net", "#2");

      insert_twin_arc_chain (twin_net, source, dest, id_seq, maxlen);
    }
}

static void make_twin_arc_chains
  (NETptr twin_net, PRSTLISTptr source_list, PRSTLISTptr dest_list,
   id_type inlab, ARCCHVECptr acvec, id_type *in_match, id_type *id_seq)
{
  /* Collects recursively all arc chains for ONE hyper net state and
     ONE input label, and copies them to the twin net.
     Recursion depth (and id_pos) overflow is not checked;
     this has been done in  iterate_add_state_to_list()  already. */

  PRSTLISTptr source;  ARCptr arc;

  clean_arc_chain_vector (acvec);

  for (source = source_list;  source;  source = source->next)
    for (arc = STATE_arc_set (source->state1);  arc;  arc = next_arc (arc))
      recurse_collect_arc_chains
	(acvec, source->state2, dest_list, arc, inlab, in_match, id_seq, 0);

  copy_arc_chains_to_twin_net (twin_net, acvec, id_seq);
}

/*-----------------------------------------------------------*/
NETptr align_ambiguity (NETptr fst, int side_p, int copy_p, int compact_p)
{
  /* Aligns (pushes) locally all ambiguous arcs to equal distance from the
     current hyper state. Replaces EPSILON by "DET_EPSILON".
     Returns the modified FST, or NULL on failure. */

  ARCCHVECptr acvec;
  HEAPptr state_heap, pair_heap;
  NETptr hyper_net, twin_net;  STATEptr state;  ARCptr arc;
  id_type *in_match, *id_seq;  

  if (copy_p == DO_COPY)
    fst = copy_fsm (fst);

  /* prepare heaps */

  state_heap = init_heap
    (sizeof (STATELISTtype), STATE_LIST_BLOCK_SIZE, "State list cells");

  pair_heap = init_heap
    (sizeof (PRSTLISTtype), PAIR_STATE_LIST_BLOCK_SIZE, "Pair state cells");

  /* make hyper net and twin states */

  fst = attach_boundaries (fst, BOUNDARY_SYMBOL, BOUNDARY_SYMBOL);

  assign_match_tables (&in_match, NULL, side_p);
  hyper_net = make_determ_input_net (fst, state_heap, in_match, side_p, FALSE);

  if (! hyper_net)        /* too deep epsilon recursion */
    return (NULL);

  twin_net = make_state_pairs (hyper_net, state_heap, pair_heap);
  free_heap (state_heap);

  /* build arc chains in the twin net */

  acvec = alloc_arc_chain_vector ();

  id_seq = (id_type *) malloc
    ((BIMACH_EPSILON_RECURSE_LIMIT + 2) * sizeof (id_type));

  assign_match_tables (&in_match, NULL, side_p);

  for (state = NET_states (hyper_net);  state;  state = next_state (state))
    for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
      make_twin_arc_chains
	(twin_net, (PRSTLISTptr) STATE_client_cell (state),
	 (PRSTLISTptr) STATE_client_cell (ARC_destination (arc)),
	 ARC_label (arc), acvec, in_match, id_seq);

  /* finish */

  twin_net = replace_label
    (twin_net, BOUNDARY_SYMBOL, EPSILON, DO_MINIMIZE, compact_p);

  free_heap (pair_heap);
  free_network (fst);
  free_network (hyper_net);

  free_arc_chain_vector (acvec);
  free (id_seq);

  return (twin_net);
}


/************************************************************
 *
 * CONVERT FST WITH PUSHED AMBIGUITY BACK TO ORDINARY FST
 *
 */

NETptr unalign_ambiguity (NETptr fst, int copy_p, int compact_p)
{
  /* Converts an FST with pushed ambiguity back to ordinary FST.
     Returns NULL if the argument FST contains forbidden symbols. */

  int error_p = FALSE;

  /* check argument FST */

  if (contains_forbidden_symbol (NET_sigma (fst), EPSILON))
    error_p = TRUE;

  if (contains_forbidden_symbol (NET_sigma (fst), ALTCHAIN_SYMBOL))
    error_p = TRUE;

  if (error_p)
    return (NULL);

  /* convert argument FST */

  if (copy_p == DO_COPY)
    fst = copy_fsm (fst);

  fst = replace_symbol (fst, DET_EPSILON, EPSILON, DO_MINIMIZE, compact_p);
  return (fst);
}


/************************************************************
 *
 * MAKE ORIGINAL BIMACHINE AUTOMATON
 *
 */

static NETptr bimachine_automaton
  (NETptr fst, HEAPptr state_heap, int side_p, int reverse_p)
{
  NETptr autom;  
  id_type *in_match;

  assign_match_tables (&in_match, NULL, side_p);
  autom = make_determ_input_net (fst, state_heap, in_match, side_p, reverse_p);

  if (! autom)
    prog_error ("bimachine_automaton", "#1");
  /* epsilon recursion limit exceded
     -- should have been remarked in ambiguity alignment already */

  return (autom);
}


/************************************************************
 *
 * MAKE SEQUENTIAL FSTs FROM BIMACHINE
 *
 */

static int arc_list_compare (ARCptr *a, ARCptr *b)
{
  static id_type *in_match = NULL;

  /* CASE: assign input match */

  if (! b)
    {
      in_match = (id_type *) a;
      return (0);
    }

  /* CASE: compare items */

  if (in_match [ARC_label (*a)] < in_match [ARC_label (*b)])
    return (1);
  else if (in_match [ARC_label (*a)] > in_match [ARC_label (*b)])
    return (-1);

  return (0);
}

void sort_bimachine_fst_arcs (NETptr net, int side_p, int eps_1st_p)
{
  /* Sorts the arcs of every state on increasing ID.
     If eps_1st_p==TRUE then DET_EPSILON arcs are put in the
     first position. Otherwise they are handled as all other arcs,
     and placed accoding to their ID.
     If net==NULL then the static variables are reinitialized.

     This function is automatically called when the two sequential FSTs
     are derived from the bimachine. So you do not need to call it again!
     However, if you perform calculus opperations on these FSTs (while
     maintaining their sequentiality) then this may change the order of
     arcs. So you should sort the arcs again by calling this function. */

  static ARCptr *arcvec = NULL;  static long max = 0;
  STATEptr state;  ARCptr arc, eps_arc;  long i, len;
  id_type *in_match;

  /* CASE: re-initialization */

  if (! net)
    {
      if (arcvec)
	free (arcvec);

      arcvec = NULL;
      max = 0;
      return;
    }

  /* CASE: sorting of arcs */

  assign_match_tables (&in_match, NULL, side_p);

  for (state = NET_states (net);  state;  state = next_state (state))
    {
      len = num_arcs (state);

      if (max < len)    /* add memory if required */
	{
	  if (arcvec)
	    {
	      if (! (arcvec = (ARCptr *)
		     realloc (arcvec, len * sizeof (ARCptr))))
		not_enough_memory ("sort_bimachine_fst_arcs");
	    }
	  else
	    {
	      if (! (arcvec = (ARCptr *) malloc (len * sizeof (ARCptr))))
		not_enough_memory ("sort_bimachine_fst_arcs");
	    }

	  max = len;
	}

      for (i=0, arc = STATE_arc_set (state);  arc;  i++, arc = next_arc (arc))
	arcvec [i] = arc;

      if (i != len)
	prog_error ("sort_bimachine_fst_arcs", "#1");

      arc_list_compare ((ARCptr *) in_match, NULL);  /* assign input match */
      qsort (arcvec, len, sizeof (ARCptr), (int (*)(const void *, const void *)) arc_list_compare);
      arc_list_compare (NULL, NULL);        /* clear input match */

      STATE_arc_set (state) = NULL;
      eps_arc = NULL;

      for (i=0;  i< len;  i++)
	{
	  arc = arcvec [i];

	  if ((eps_1st_p)
	      && (in_match [ARC_label (arc)] == DET_EPSILON))
	    {
	      if (eps_arc)    /* other DET_EPSILON found before */
		prog_error ("sort_bimachine_fst_arcs", "#2");

	      eps_arc = arc;
	    }
	  else
	    {
	      next_arc (arc) = STATE_arc_set (state);
	      STATE_arc_set (state) = arc;
	    }
	}

      if (eps_arc)    /* DET_EPSILON found */
	{
	  next_arc (eps_arc) = STATE_arc_set (state);
	  STATE_arc_set (state) = eps_arc;
	}
    }
}

/*-----------------------------------------------------------*/
static void add_new_arc_only
  (STATEptr source, STATEptr dest, id_type id, NETptr net)
{
  /* Adds an arc with "id" leading form "source" to "dest" if there
     is no such arc already. Does not check for determinism. */

  ARCptr arc;

  for (arc = STATE_arc_set (source);  arc;  arc = next_arc (arc))
    if ((ARC_label (arc) == id) && (ARC_destination (arc) == dest))
      return;

  arc = make_arc (id, dest, net);
  push_arc (arc, source);
}

static void correct_twin_arc_destinations
  (STATEVECptr Avec, STATEVECptr Tvec, NETptr Tnet)
{
  /* Corrects the destination pointers of some twin arcs in T.
     Most of them currently point to states in A.
     Note: State client cells are used. */

  STATEptr state, dest;  ARCptr arc;  long i;

  if (Avec->len != Tvec->len)
    prog_error ("correct_twin_arc_destinations", "#1");

  for (i = 0;  i < Avec->len;  i ++)
    STATE_client_cell (Avec->vec [i]) = Tvec->vec [i];

  for (state = NET_states (Tnet);  state;  state = next_state (state))
    for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
      if ((dest = (STATEptr) STATE_client_cell (ARC_destination (arc))))
	ARC_destination (arc) = dest;

  for (i = 0;  i < Avec->len;  i ++)
    {
      STATE_client_cell (Avec->vec [i]) = NULL;
      STATE_client_cell (Tvec->vec [i]) = NULL;
    }
}

/*-----------------------------------------------------------*/
static void enhance_one_transducer
  (STATEptr A_state, STATEptr T_state, NETptr T,
   id_type inlab, id_type T_id, id_type T_id_2nd)
{
  STATEptr aux_state;  ARCptr arc;  int num_arcs = 0;

  for (arc = STATE_arc_set (A_state);  arc;  arc = next_arc (arc))
    if (ARC_label (arc) == inlab)  /* arc found */
      {
	if (T_id_2nd != EPSILON)  /* make an arc chain */
	  {
	    aux_state = make_state (NULL, T);
	    STATE_deterministic (aux_state) = FALSE;
	    /* STATE_client_cell (aux_state) = NULL; */

	    add_new_arc_only (T_state, aux_state, T_id, T);  /* 1st arc */
	    num_arcs ++;

	    T_state = aux_state;
	    T_id = T_id_2nd;
	  }

	/* NOTE: We sould use the destination state of T, but currently
	   we cannot access it easily. Therefore we use here the destination
	   state of A (which is wrong) and will correct it later by
	   correct_twin_arc_destinations(). */

	add_new_arc_only (T_state, ARC_destination (arc), T_id, T);
	num_arcs ++;
      }

  if (((T_id_2nd == EPSILON) && (num_arcs != 1)) ||
      ((T_id_2nd != EPSILON) && (num_arcs != 2)))
    prog_error ("enhance_one_transducer", "#1");
}

static id_type make_intermediate_bm_label (id_type in_id, uint16 index)
{
  LABELptr label;  id_type mid_id;  char in_name [400], mid_name [400];

  label = id_to_label (in_id);

  if (LABEL_arity (label) != 1)
    prog_error ("make_intermediate_bm_label", "labels arity %hd rather than 1",
		LABEL_arity (label));

  FST_fat_to_thin (LABEL_atom (label), in_name);
  sprintf (mid_name, BM_DIACRITIC_PATTERN, in_name, index);

  if (FST_intern_literal (&mid_id, mid_name) != NO_ERROR)
    handle_error ("too many labels", "make_intermediate_bm_label", mid_id);

  return (mid_id);
}

static void enhance_transducers
  (DELTAMATRIXptr matrix, id_type inlab, int side_p)
{
  /* Adds arcs to both sequential transducers, T1 and T2, derived from
     the bimachine. Note: The work is done for ONE input label only. */

  DLDENSEptr item, end;
  STATEVECptr A1vec, A2vec, T1vec, T2vec;
  int a1, a2;
  uint16 *label_index, index;
  id_type pair_id, in_id, mid_id, a1_id, a2_id, a1_id_2nd, a2_id_2nd,
    *in_match, *out_match;

  assign_match_tables (&in_match, &out_match, side_p);

  label_index = matrix->label_index;

  A1vec = matrix->state_vector.A1;
  A2vec = matrix->state_vector.A2;
  T1vec = matrix->state_vector.T1;
  T2vec = matrix->state_vector.T2;

  item = matrix->dense_matrix.items;
  end = item + matrix->dense_matrix.len;

  for (  ;  item < end;  item ++)
    {
      a1 = item->A1_num;
      a2 = item->A2_num;
      pair_id = item->pair_label;
      index = label_index [a1];

      in_id = in_match [pair_id];

      if (! index)    /* no label factorization required */
	{
	  a1_id = in_id;
	  a2_id = pair_id;
	  a1_id_2nd = a2_id_2nd = EPSILON;
	}
      else if (in_id == OTHER)    /* indirect factorization required */
	{
	  mid_id = make_diacritic (INDIR_FACTORIZE_DIACR_PATTERN, index);

	  a1_id = in_id;
	  a1_id_2nd = make_dir_pair_label (DET_EPSILON, mid_id, side_p);

	  a2_id = make_dir_pair_label (mid_id, EPSILON, side_p);
	  a2_id_2nd = pair_id;
	}
      else    /* direct factorization */
	{
	  mid_id = make_intermediate_bm_label (in_id, index);
	  a1_id = make_dir_pair_label (in_id, mid_id, side_p);
	  a2_id = make_dir_pair_label (mid_id, out_match [pair_id], side_p);
	  a1_id_2nd = a2_id_2nd = EPSILON;
	}

      enhance_one_transducer
	(A1vec->vec [a1], T1vec->vec [a1], matrix->net.T1, inlab, a1_id, a1_id_2nd);

      enhance_one_transducer
	(A2vec->vec [a2], T2vec->vec [a2], matrix->net.T2, inlab, a2_id, a2_id_2nd);
    }
}


/************************************************************
 *
 * MAKE BIMACHINE DELTA MATRIX
 *
 */

static STATEVECptr link_net_to_state_vector (NETptr net)
{
  /* Returns a state vector with every vec[i] pointing to a state
     of the net, and vec[0] points to the start state. */

  STATEVECptr vec;  STATEptr state;  long i, num = 0;

  vec = alloc_state_vector (NET_num_states (net));
  vec->len = vec->max;

  vec->vec [num++] = NET_start_state (net);

  for (state = NET_states (net), i=0;
       (state) && (i< vec->len);
       state = next_state (state), i++)
    if (state != NET_start_state (net))
      vec->vec [num++] = state;

  if ((state) || (num != vec->len))
    prog_error ("link_net_to_state_vector", "#1");

  return (vec);
}

static STATEVECptr link_twin_net_to_state_vector
  (STATEVECptr vec, NETptr *twin_net)
{
  /* Assigns to *twin_net a new network with as many states as vec[]
     is long, but witout any arcs.
     Returns a new state vector with every twin_vec[i] pointing to
     a state of the new twin_net, and twin_vec[0] points to the start
     state. Finality of the states of twin_vec[i] depends on the states
     of vec[i]. */

  STATEVECptr twin_vec;  STATEptr state;  long i;

  *twin_net = make_network ();
  NET_deterministic (*twin_net) = FALSE;
  NET_loop_free (*twin_net) = FALSE;
  NET_epsilon_free (*twin_net) = FALSE;

  twin_vec = alloc_state_vector (vec->len);
  twin_vec->len = twin_vec->max;

  for (i=0;  i< vec->len;  i++)
    {
      state = make_state (NULL, *twin_net);
      STATE_deterministic (state) = FALSE;
      STATE_final (state) = STATE_final (vec->vec [i]);

      twin_vec->vec [i] = state;
    }

  NET_start_state (*twin_net) = twin_vec->vec [0];
  return (twin_vec);
}

static void attach_state_number_sets (NETptr fst, HEAPptr heap)
{
  STATEptr state;  NUM2SETptr state_set;

  for (state = NET_states (fst);  state;  state = next_state (state))
    {
      state_set = (NUM2SETptr) cell_alloc (heap);
      STATE_client_cell (state) = (void *) state_set;

      state_set->A1_states = NULL;
      state_set->A2_states = NULL;
    }
}

static void attach_state_number_lists
  (STATEVECptr A_vector, HEAPptr heap, int A1_p)
{
  STATELISTptr state_cell;  NUMLISTptr num_cell, *num_list;
  NUM2SETptr state_set;  STATEptr A_state, orig_state;  int i;

  for (i=0;  i< A_vector->len;  i++)
    {
      A_state = A_vector->vec [i];

      for (state_cell = (STATELISTptr) STATE_client_cell (A_state);
	   state_cell;
	   state_cell = state_cell->next)
	{
	  orig_state = state_cell->state;
	  state_set = (NUM2SETptr) STATE_client_cell (orig_state);

	  if (A1_p)
	    num_list = &(state_set->A1_states);
	  else
	    num_list = &(state_set->A2_states);

	  num_cell = (NUMLISTptr) cell_alloc (heap);
	  num_cell->next = *num_list;
	  *num_list = num_cell;

	  num_cell->number = i;
	}
    }
}

/*-----------------------------------------------------------*/
static void clean_delta_matrix (DELTAMATRIXptr matrix)
{
  matrix->dense_matrix.len = 0;
  matrix->dense_matrix.sorted_p = FALSE;

  memset (matrix->label_index, 0,
	  matrix->state_vector.A1->len * sizeof (uint16));
}

static DELTAMATRIXptr create_delta_matrix (NETptr fst, NETptr A1, NETptr A2)
{
  DELTAMATRIXptr matrix;  long len1, len2;

  if (! (matrix = (DELTAMATRIXptr) malloc (sizeof (DELTAMATRIXtype))))
    not_enough_memory ("alloc_delta_matrix");

  /* networks */

  matrix->net.fst = fst;
  matrix->net.A1 = A1;
  matrix->net.A2 = A2;

  /* state vectors */

  matrix->state_vector.A1 = link_net_to_state_vector (A1);
  matrix->state_vector.A2 = link_net_to_state_vector (A2);

  matrix->state_vector.T1 = link_twin_net_to_state_vector
    (matrix->state_vector.A1, &(matrix->net.T1));
  matrix->state_vector.T2 = link_twin_net_to_state_vector
    (matrix->state_vector.A2, &(matrix->net.T2));

  /* dense matrix */

  len1 = matrix->state_vector.A1->len;
  len2 = matrix->state_vector.A2->len;

  matrix->dense_matrix.max = len1 + len2;

  if (! (matrix->dense_matrix.items = (DLDENSEptr)
	 malloc (matrix->dense_matrix.max * sizeof (DLDENSEtype))))
    not_enough_memory ("alloc_delta_matrix");

  /* index vector */

  if (! (matrix->label_index = (uint16 *) malloc (len1 * sizeof (uint16))))
    not_enough_memory ("alloc_delta_matrix");

  /* heaps */

  matrix->heap.number_list = init_heap
    (sizeof (NUMLISTtype), NUMBER_LIST_HEAP_BLOCK, "Number list cells");

  matrix->heap.num2_set = init_heap
    (sizeof (NUM2SETtype), TWO_NUMBER_SETS_HEAP_BLOCK, "Two number sets");

  /* attach state number lists to original FST */

  attach_state_number_sets (fst, matrix->heap.num2_set);

  attach_state_number_lists
    (matrix->state_vector.A1, matrix->heap.number_list, TRUE);

  attach_state_number_lists
    (matrix->state_vector.A2, matrix->heap.number_list, FALSE);

  /* finish */

  clean_delta_matrix (matrix);
  return (matrix);
}

static void free_delta_heaps (DELTAMATRIXptr matrix)
{
  if (matrix->heap.number_list)
    {
      free_heap (matrix->heap.number_list);
      matrix->heap.number_list = NULL;
    }

  if (matrix->heap.num2_set)
    {
      free_heap (matrix->heap.num2_set);
      matrix->heap.num2_set = NULL;
    }
}

static void free_delta_matrix (DELTAMATRIXptr matrix)
{
  /* do not touch networks */

  free_state_vector (matrix->state_vector.A1);
  free_state_vector (matrix->state_vector.A2);
  free_state_vector (matrix->state_vector.T1);
  free_state_vector (matrix->state_vector.T2);

  free (matrix->dense_matrix.items);
  free (matrix->label_index);
  free_delta_heaps (matrix);

  free (matrix);
}

static void enhance_delta_matrix (DELTAMATRIXptr matrix, long newmax)
{
  matrix->dense_matrix.max = newmax;

  if (! (matrix->dense_matrix.items = (DLDENSEptr)
	 realloc (matrix->dense_matrix.items, newmax * sizeof (DLDENSEtype))))
    not_enough_memory ("enhance_delta_matrix");
}

/*-----------------------------------------------------------*/
static int sort_delta_A2_A1_cmp_fn (DLDENSEptr a, DLDENSEptr b)
{
  int cmp;

  if ((cmp = a->A2_num - b->A2_num))
    return (cmp);
  if ((cmp = a->A1_num - b->A1_num))
    return (cmp);

  return (0);
}

static int sort_dense_delta_matrix (DELTAMATRIXptr matrix)
{
  /* Sorts the dense representation of the delta matrix. Removes repetitions.
     Returns FALSE if there are contradictory IDs. */

  DLDENSEptr prev, cur, to_item, end;

  /* sort all items */

  qsort (matrix->dense_matrix.items, matrix->dense_matrix.len, sizeof (DLDENSEtype),
	 (int (*)(const void *, const void *)) sort_delta_A2_A1_cmp_fn);

  /* remove repetitions */

  cur = to_item = matrix->dense_matrix.items;
  end = cur + matrix->dense_matrix.len;
  prev = NULL;

  for (  ;  cur < end;  prev = cur, cur ++)
    {
      if ((prev) && (prev->A1_num == cur->A1_num)    /* same coordinates */
	  && (prev->A2_num == cur->A2_num))
	{
	  if (prev->pair_label != cur->pair_label)  /* contradictory IDs */
	    return (FALSE);
	}
      else       /* different coordinates */
	{
	  if (to_item < cur)    /* copy required */
	    memcpy (to_item, cur, sizeof (DLDENSEtype));

	  to_item ++;
	}
    }

  matrix->dense_matrix.len = to_item - matrix->dense_matrix.items;
  matrix->dense_matrix.sorted_p = TRUE;

  return (TRUE);
}

/*-----------------------------------------------------------*/
static int arc_label_to_dense_delta_matrix
  (DELTAMATRIXptr matrix, int A1_num, int A2_num, id_type arc_label)
{
  /* Inserts one arc label into the dense representation of the delta matrix.
     Returns FALSE if there are contradictory IDs which is remarked only
     when sorting the matrix. */

  DLDENSEptr item;  long newmax;

  if (matrix->dense_matrix.len == matrix->dense_matrix.max)
    {
      if (! sort_dense_delta_matrix (matrix))
	return (FALSE);

      newmax = matrix->dense_matrix.len * BM_DELTA_DENSE_REALLOC_FACTOR;

      if (newmax > matrix->dense_matrix.max)
	enhance_delta_matrix (matrix, newmax);
    }

  item = matrix->dense_matrix.items + (matrix->dense_matrix.len ++);
  item->A1_num = A1_num;
  item->A2_num = A2_num;
  item->pair_label = arc_label;

  matrix->dense_matrix.sorted_p = FALSE;

  return (TRUE);
}

static void fill_delta_matrix
  (DELTAMATRIXptr matrix, id_type inlab, int *error_p, int side_p)
{
  /* Fills delta matrix with IDs of arcs with "inlab" on the input side.
     Sets *error_p=TRUE if contradictory IDs occur which means that the
     original FST is not functional.
     Note: The work is done for ONE input label only. */

  STATEptr source, dest;
  ARCptr arc;  
  NUMLISTptr A1_state, A2_state;
  id_type label, *in_match;

  assign_match_tables (&in_match, NULL, side_p);
  *error_p = FALSE;

  /* ---- see all arcs of original FST with given input label */
  for (source = NET_states (matrix->net.fst);  source;  source = next_state (source))
    for (arc = STATE_arc_set (source);  arc;  arc = next_arc (arc))
      if (in_match [ARC_label (arc)] == inlab)
	{
	  dest = ARC_destination (arc);
	  label = ARC_label (arc);

	  /* ---- see all related states in A1 and A2 */
	  for (A1_state = ((NUM2SETptr) STATE_client_cell (source))->A1_states;
	       A1_state;
	       A1_state = A1_state->next)
	    for (A2_state = ((NUM2SETptr) STATE_client_cell (dest))->A2_states;
		 A2_state;
		 A2_state = A2_state->next)

	      if (! arc_label_to_dense_delta_matrix
		  (matrix, A1_state->number, A2_state->number, label))
		{
		  *error_p = TRUE;
		  return;
		}
	}

  if (! sort_dense_delta_matrix (matrix))
    *error_p = TRUE;
}

/*-----------------------------------------------------------*/
static void define_delta_label_indices (DELTAMATRIXptr matrix)
{
  /* Defines label indices. Distinctions (by different index) are
     only made where necessary.
     Note: The work is done for ONE input label only. */

  DLDENSEptr item1, item2, item_end;
  uint16 *label_index;  int next_index, next_index_used_p;

  item1 = matrix->dense_matrix.items;
  item_end = item1 + matrix->dense_matrix.len;
  label_index = matrix->label_index;

  next_index = 1;        /* next un-used label index */
  next_index_used_p = FALSE;

  for (  ;  item1 < item_end;  item1 ++)
    {
      if (next_index_used_p)    /* current value of next_index already used */
	{
	  next_index ++;    /* new value for next_index */
	  next_index_used_p = FALSE;

	  if (next_index >= MAX_LABEL)
	    prog_error ("define_delta_label_indices", "ID overflow (max %d)", MAX_LABEL);
	}

      for (item2 = item1 + 1;
	   (item2 < item_end) && (item1->A2_num == item2->A2_num);
	   item2 ++)
	if ((item1->pair_label != item2->pair_label)  /* different labels */
	    && (label_index [item1->A1_num] == label_index [item2->A1_num])) /*equal idx*/
	  {
	    label_index [item2->A1_num] = next_index;  /* change index */
	    next_index_used_p = TRUE;
	  }
    }
}


/*-----------------------------------------------------------*/
#if NOT_USED
static void DEBUG_set_delta_label_indices (DELTAMATRIXptr matrix, uint16 idx1)
{
  /* TEMPORARY DEBUG FUNCTION:
     Sets label indices to [idx1+0] [idx1+1] [idx1+2] [idx1+3] .....

     ALLOWS TESTING THE INDEX HANDLING BY ROCHE AND SCHABES.
     */

  uint16 *label_index;  long i, len1;

  label_index = matrix->label_index;
  len1 = matrix->state_vector.A1->len;

  for (i = 0;  i < len1;  i ++)
    label_index [i] = idx1 ++;
}
#endif /* NOT_USED */

#if NOT_USED
static void DEBUG_print_total_sigma (NETptr T1, NETptr T2)
{
  ALPHABETptr sigma;  long num;

  sigma = sigma_union_dont_copy (copy_alphabet (NET_sigma (T1)),
				 copy_alphabet (NET_sigma (T2)));

  num = num_items_in_bin_alph (sigma);
  free_alph (sigma);

  print_comment (TRUE, "\tJoint Sigma (T1+T2) :\t%li symbols\n", num);
}
#endif /* NOT_USED */

/************************************************************
 *
 * PRINCIPAL FACTORIZATION FUNCTIONS
 *
 */

static DELTAMATRIXptr bimachine_to_fsts
  (NETptr fst, NETptr A1, NETptr A2,
   int side_p, int compact_p, int verbose_p, int *error_p)
{
  DELTAMATRIXptr delta;  ALPHABETptr in_alph, excl_sym;
  id_type inlab;  long a;

  *error_p = FALSE;

  in_alph = one_side_alph (NET_labels (fst), side_p);
  excl_sym = excluded_symbols (fst);

  delta = create_delta_matrix (fst, A1, A2);

  /* iterate through input alphabet */

  for (a=0;  a< ALPH_len (in_alph);  a++)
    {
      inlab = ALPH_item (in_alph, a);
      print_comment (verbose_p, ".");

      clean_delta_matrix (delta);
      fill_delta_matrix (delta, inlab, error_p, side_p);

      if (*error_p)
	return (delta);

      if (! has_constant_transduction (inlab, NET_labels (fst), side_p))
	define_delta_label_indices (delta);

      enhance_transducers (delta, inlab, side_p);
    }

  print_comment (verbose_p, " ");

  /* finalize sequential T1 and T2 */

  clear_client_cells (delta->net.T1);
  clear_client_cells (delta->net.T2);

  correct_twin_arc_destinations
    (delta->state_vector.A1, delta->state_vector.T1, delta->net.T1);
  correct_twin_arc_destinations
    (delta->state_vector.A2, delta->state_vector.T2, delta->net.T2);

  derive_alphabets (delta->net.T1, excl_sym);
  derive_alphabets (delta->net.T2, excl_sym);

  delta->net.T1 = force_minimize (delta->net.T1, DONT_COPY, compact_p);
  delta->net.T2 = force_minimize (delta->net.T2, DONT_COPY, compact_p);

  /* change output DET_EPSILON of second FST into EPSILON */

  if (contains_symbol_on_side
      (delta->net.T2, DET_EPSILON, opposite_side (side_p)))
    delta->net.T2 = replace_oneside_symbol
      (delta->net.T2, DET_EPSILON, EPSILON, opposite_side (side_p), DO_MINIMIZE, TRUE);

  /* finish */

  reset_client_cells (fst);
  free_delta_heaps (delta);

  free_alph (in_alph);
  free_alph (excl_sym);

  return (delta);
}

NVptr factorize_bm_fsts
  (NETptr fst, int side_p, int copy_p, int compact_p, int alphreduce_p,
   int verbose_p)
{
  /* Factorizes a functional FST into a left-sequential and a
     right-sequential FST. Builds intermediately a bimachine.
     Returns the two sequential FSTs, or NULL on failure.

     ARGUMENTS:
       fst ......... input FST, functional
       side_p ...... if UPPER than functionality goes from upper to
                       lower side
       copy_p ...... if DO_COPY than the input FST is copied
       compact_p ... if TRUE than the sigma alphabets of both resulting
                       FSTs are compacted
       verbose_p ... if TRUE than all steps of conversion are commented

     THE CALLER MUST ENSURE THAT THE INPUT FST IS FUNCTIONAL !!!
     This function is able to detect some forms of ambiguity in the
     input FST, in which case NULL is returned, but most probably
     it cannot detect all forms of ambiguity. In the later case the
     result is wrong. */

  DELTAMATRIXptr delta;  HEAPptr state_list_heap;
  NVptr bm;  NETptr A1, A2, T1, T2;
  int error_p;

  /* prepare */

  if (! verify_fst_before_factorization (fst))
    return (NULL);

  fst = min_fsm (fst, copy_p);
  error_buffer ("clear", NULL);

  print_comment
    (verbose_p, "Input transducer:\n  %s\n", temp_net_info (fst, side_p));

  /* align ambiguity */

  if (contains_symbol_on_side (fst, EPSILON, side_p))  /* input EPSILON */
    {
      print_comment (verbose_p, "Aligning ambiguity ... ");

      if (contains_forbidden_symbol (NET_sigma (fst), DET_EPSILON))
	{
	  error_buffer
	    ("put", "FST contains both EPSILON and DET_EPSILON on the input side");
	  return (NULL);
	}

      if (! (fst = align_ambiguity (fst, side_p, DONT_COPY, compact_p)))
	{                /* original FST infinitely ambiguous */
	  error_buffer ("put", "FST contains infinite ambiguity");
	  return (NULL);
	}

      print_comment (verbose_p, "done:\n  %s\n", temp_net_info (fst, side_p));
    }
  else if (contains_symbol_on_side (fst, EPSILON, opposite_side (side_p)))
    fst = replace_oneside_symbol
      (fst, EPSILON, DET_EPSILON, opposite_side (side_p), DO_MINIMIZE, TRUE);

  /* convert FST to bimachine */

  state_list_heap = init_heap
    (sizeof (STATELISTtype), STATE_LIST_BLOCK_SIZE, "State list cells");

  print_comment (verbose_p, "Building 1st automaton ... ");
  A1 = bimachine_automaton (fst, state_list_heap, side_p, FALSE);
  print_comment (verbose_p, "done:\n  %s\n", temp_net_info (A1, side_p));

  print_comment (verbose_p, "Building 2nd automaton ... ");
  A2 = bimachine_automaton (fst, state_list_heap, side_p, TRUE);
  print_comment (verbose_p, "done:\n  %s\n", temp_net_info (A2, side_p));

  /* convert bimachine to two sequential FSTs */

  print_comment (verbose_p, "Building two sequential transducers ");
  delta = bimachine_to_fsts
    (fst, A1, A2, side_p, compact_p, verbose_p, &error_p);

  T1 = delta->net.T1;
  T2 = delta->net.T2;
  delta->net.T1 = delta->net.T2 = NULL;

  print_comment (verbose_p, "done:\n");
  print_comment (verbose_p, "  %s\n", temp_net_info (T1, side_p));
  print_comment (verbose_p, "  %s\n", temp_net_info (T2, side_p));

  bm = make_nv (2);

  /* reduced the intemediate alphabet */

  if (alphreduce_p)
    {
      print_comment (verbose_p, "Reducing intermediate alphabet ... ");
      NV_net (bm, 0) = T1;
      NV_net (bm, 1) = T2;

      bm = reduce_intermediate_alphabet_in_pair
	(bm, NULL, side_p, DONT_COPY, TRUE, FALSE);

      T1 = NV_net (bm, 0);
      T2 = NV_net (bm, 1);

      print_comment (verbose_p, "done:\n");
      print_comment (verbose_p, "  %s\n", temp_net_info (T1, side_p));
      print_comment (verbose_p, "  %s\n", temp_net_info (T2, side_p));
    }

  /* free memory */

  free_heap (state_list_heap);
  free_delta_matrix (delta);

  /* move property list of original fst to T1 (left fst) */

  NET_properties(T1) = NET_properties(fst);
  NET_properties(fst) = NULL;

  free_network (fst);
  free_network (A1);
  free_network (A2);

  if (error_p)  /* original FST ambiguous */
    {
      free_network (T1);
      free_network (T2);
      free_nv_only (bm);
      error_buffer ("put", "Network is ambiguous. It cannot be factorized.");
      return (NULL);
    }

  /* finish */

  set_bimachine_flags (T1, T2, side_p);

  sort_bimachine_fst_arcs (T1, side_p, TRUE);
  sort_bimachine_fst_arcs (T2, side_p, FALSE);

  sort_bimachine_fst_arcs (NULL, 0, FALSE);  /* cleans stat.vars */

  /* T1 and T2 are stored in canonical order [T1 T2] that is
     use with all factorizations and un-factorizations.
     To reflect the direction of application, [T1 T2] should
     be reversed [ by nv_reserse() ] outside.  */

  NV_net (bm, 0) = T1;
  NV_net (bm, 1) = T2;

  return (bm);
}


/************************************************************
 *
 * CONVERT TWO FACTORIZED FSTs (BIMACHINE) BACK TO ONE FUNCTIONAL FST
 *
 */

NETptr unfactorize_bm_fsts
  (NETptr left, NETptr right, int side_p, int copy_p, int verbose_p)
{
  NETptr fst;  ALPHABETptr diff_sigma, diff_sig1, diff_sig2;
  PROPptr props;

  print_comment (verbose_p, "Input transducers:\n");
  print_comment (verbose_p, "  %s\n", temp_net_info (left, side_p));
  print_comment (verbose_p, "  %s\n", temp_net_info (right, side_p));

  if (! verify_bimachine_flags (left, right, side_p))
    {
      error_buffer ("put", "Not a bimachine or incorrectly marked");
      return (NULL);
    }

  if ((! verify_bmfst_before_unfactorization (left))
      || (! verify_bmfst_before_unfactorization (right)))
    return (NULL);

  if (copy_p == DO_COPY)
    {
      left = copy_fsm (left);
      right = copy_fsm (right);
    }

  /* compute sigma difference */

  diff_sig1 = excluded_symbols (left);
  diff_sig2 = excluded_symbols (right);
  diff_sigma = sigma_union (diff_sig1, diff_sig2);

  free_alph (diff_sig1);
  free_alph (diff_sig2);

  /* compose FSTs */

  print_comment (verbose_p, "Composing transducers ... ");
  right = reverse_fsm (right, DONT_COPY, DO_MINIMIZE);

  props = NET_properties (left);  /* preserve properties of left FST */
  NET_properties (left) = NULL;

  if (side_p == UPPER)
    fst = compose (left, right);
  else
    fst = compose (right, left);

  NET_properties(fst) = props;
  print_comment (verbose_p, "done:\n  %s\n", temp_net_info (fst, side_p));

  /* modify arcs */

  print_comment (verbose_p, "Removing pseudo epsilon symbols ... ");
  fst = replace_symbol (fst, DET_EPSILON, EPSILON, DONT_MINIMIZE, FALSE);

  /* modify alphabets */

  derive_alphabets (fst, diff_sigma);
  free_alph (diff_sigma);

  /* finish */

  fst = force_minimize (fst, DONT_COPY, TRUE);
  print_comment (verbose_p, "done:\n  %s\n", temp_net_info (fst, side_p));

  return (fst);
}


/************************************************************
 *
 * PRINCIPLE FACTORIZATION FUNCTIONS FOR SHORT AMBIGUITY
 *
 */

static void disambiguate_arc_list
  (NETptr fst, STATEptr state, ALPHTRANSptr table, ALPHABETptr arc_labels,
   id_type *in_match, id_type *out_match, int side_p,
   int arc_comp_fn (const void *, const void *))
{
  /* Replaces the old arc list of the state by a new one with diacritics
     for short ambiguities. The original transductions are stored in
     "table". */

  SYMTRANSptr out_symset;  id_type new_label;
  ARCptr arc, arc2, nextarc, newarc, newlist = NULL;

  /* create new arc list */

  for (arc = STATE_arc_set (state);  arc;  arc = arc2)
    {
      ALPH_len (arc_labels) = 0;

      for (arc2 = next_arc (arc);  arc2;  arc2 = next_arc (arc2))
	{
	  if (! arc_comp_fn ((void *) &arc, (void *) &arc2))
	    ALPH_item (arc_labels, ALPH_len (arc_labels) ++) =
	      out_match [ARC_label (arc2)];
	  else
	    break;
	}

      if (ALPH_len (arc_labels) > 0)    /* ambiguous transduction */
	{
	  ALPH_item (arc_labels, ALPH_len (arc_labels) ++) =
	    out_match [ARC_label (arc)];

	  out_symset = find_output_symbolset_in_table (table, arc_labels);
	  new_label = make_dir_pair_label (in_match [ARC_label (arc)],
					   out_symset->in_symbol, side_p);
	}
      else
	new_label = ARC_label (arc);

      newarc = make_arc (new_label, ARC_destination (arc), fst);
      next_arc (newarc) = newlist;
      newlist = newarc;
    }

  /* free old arc list */

  for (arc = STATE_arc_set (state);  arc;  arc = nextarc)
    {
      nextarc = next_arc (arc);
      free_arc (arc);
      NET_num_arcs (fst) --;
    }

  STATE_arc_set (state) = newlist;
}

static NETptr build_factor1_without_short_ambiguity
  (NETptr fst, ALPHTRANSptr out_symbols, int side_p, int compact_p)
{
  /* NOTE: "fst" is consumed. No argument is consumed or modified. */

  NETptr fa1;  ALPHABETptr arc_labels;  STATEptr state;
  id_type *in_match, *out_match;
  int (*arc_comp_fn)(const void *, const void *);

  arc_comp_fn = (side_p == UPPER) ?
    increasing_upper_id_and_dest_addr_comp_fn :
    increasing_lower_id_and_dest_addr_comp_fn ;

  reorder_arcs (fst, arc_comp_fn);
  assign_match_tables (&in_match, &out_match, side_p);

  arc_labels = make_alph (ALPH_len (NET_labels (fst)), LABEL_VECTOR);

  for (state = NET_states (fst);  state;  state = next_state (state))
    disambiguate_arc_list (fst, state, out_symbols, arc_labels,
			   in_match, out_match, side_p, arc_comp_fn);

  free_alph (arc_labels);

  update_net_labels_and_sigma (fst);
  fa1 = force_minimize (fst, DONT_COPY, compact_p);

  return (fa1);
}

/*-----------------------------------------------------------*/
static NETptr build_factor2_with_short_ambiguity
  (NETptr fa1, ALPHTRANSptr table, int side_p, int compact_p)
{
  /* NOTE: No argument is consumed or modified. */

  NETptr fa2, transduce, diacritics, out1;
  ALPHABETptr fa2_labels, dia_symbols, out1_symbols, out2_symset;
  id_type in_id, out_id, pair_id;  int i, j;

  fa2_labels = make_alph (LIM_id_count (LABEL_MAP), BINARY_VECTOR);
  dia_symbols = make_alph (LIM_id_count (LABEL_MAP), BINARY_VECTOR);

  /* collect diacritics and all labels from the transduction table */

  for (i = 0;  i < table->len;  i ++)
    {
      in_id = table->symbols [i].in_symbol;
      sigma_add_to (dia_symbols, in_id);

      out2_symset = table->symbols [i].out_symbols;

      for (j = 0;  j < ALPH_len (out2_symset);  j ++)
	{
	  out_id = ALPH_item (out2_symset, j);
	  pair_id = make_dir_pair_label (in_id, out_id, side_p);
	  sigma_add_to (fa2_labels, pair_id);
	}
    }

  /* build auxiliary nets */

  transduce = alphabet_to_net (fa2_labels);
  free_alph (fa2_labels);

  diacritics = alphabet_to_net (dia_symbols);
  free_alph (dia_symbols);

  out1_symbols = side_sigma (fa1, opposite_side (side_p));
  out1 = alphabet_to_net (out1_symbols);
  free_alph (out1_symbols);

  /* build factor 2 with short ambiguity:
     fa2 = [ out1 .o. [ transduce | ? - diacritics ] ]*  */

  fa2 = net_union (transduce, minus (symbol_fsm (OTHER), diacritics));

  if (side_p == UPPER)
    fa2 = compose (out1, fa2);
  else
    fa2 = compose (fa2, out1);

  fa2 = kleene_star (fa2);
  fa2 = force_minimize (fa2, DONT_COPY, compact_p);

  return (fa2);
}

/*-----------------------------------------------------------*/
NVptr factorize_shortamb_fst
  (NETptr fst, int side_p, int copy_p, int compact_p, int verbose_p)
{
  NVptr factors;  NETptr fa1, fa2;  ALPHTRANSptr out_symbols;

  fst = min_fsm (fst, copy_p);
  out_symbols = alloc_alphabet_transduction (FCT2_TRANSDUCTION_INIT_LENGTH);

  print_comment
    (verbose_p, "Input transducer:\n  %s\n", temp_net_info (fst, side_p));

  /* build factors */

  print_comment (verbose_p, "Building factors:\n");

  fa1 = build_factor1_without_short_ambiguity (fst, out_symbols, side_p, compact_p);
  fa2 = build_factor2_with_short_ambiguity (fa1, out_symbols, side_p, compact_p);

  print_comment (verbose_p, "  %s\n", temp_net_info (fa1, side_p));
  print_comment (verbose_p, "  %s\n", temp_net_info (fa2, side_p));

  /* finish */

  free_alphabet_transduction (out_symbols);

  factors = make_nv (2);
  NV_net (factors, 0) = fa1;
  NV_net (factors, 1) = fa2;

  return (factors);
}

/************************************************************
 *
 * PRINCIPLE FUNCTIONS FOR REDUCING INTERMADIATE SYMBOLS
 *
 */

static ALPHTRANSptr make_onesided_initial_equivalence_class
  (NETptr fst, ALPHABETptr symbols, int side_p)
{
  /* Returns one equivalence class that contains all relevant symbols
     wrt. all arguments. */

  ALPHTRANSptr eqv;  ALPHABETptr use_symbols, sigma;

  /* determine relevant symbols */

  if (! symbols)
    use_symbols = side_sigma (fst, side_p);
  else
    {
      symbols = copy_alphabet (symbols);
      sigma = side_sigma (fst, side_p);

      use_symbols = sigma_intersect (symbols, sigma);

      free_alph (symbols);
      free_alph (sigma);
    }

  binary_to_label (use_symbols);

  /* make 1st equivalence class containing all relevant symbols */

  if (! ALPH_len (use_symbols))    /* no symbols to be reduced */
    eqv = NULL;
  else
    {
      sort_alph_on_increasing_ids (use_symbols);

      eqv = alloc_alphabet_transduction (FCT2_TRANSDUCTION_INIT_LENGTH);
      insert_symbol_set_into_table (eqv, use_symbols, ID_NO_SYMBOL, 0);
    }

  free_alph (use_symbols);
  return (eqv);
}

/*-----------------------------------------------------------*/
static void extract_new_eqv_class
  (ALPHABETptr old_class, ALPHABETptr arc_labels, ALPHABETptr new_class)
{
  /* Creates new_class as the intersection of old_class and arc_labels.
     All new_class items are removed form old_class.
     If old_class becomes empty, new_class is considered to be old_class.
     NOTE: "arc_labels" is not modofied. */

  id_type *in_old, *in_arcs, *to_old, *to_new, *end_old, *end_arcs;

  /* CASE: old_class[1], arc_labels[1] */

  if ((ALPH_len (old_class) == 1) && (ALPH_len (arc_labels) == 1))
    {
      ALPH_len (new_class) = 0;
      return;    /* it does not matter whether the content is equal !!! */
    }

  /* CASE: default */

  in_old = to_old = ALPH_items (old_class);
  end_old = in_old + ALPH_len (old_class);

  in_arcs = ALPH_items (arc_labels);
  end_arcs = in_arcs + ALPH_len (arc_labels);

  to_new = ALPH_items (new_class);

  while ((in_old < end_old) && (in_arcs < end_arcs))
    {
      if (*in_old < *in_arcs)    /* ID in old_class but not in arc_labels */
	*to_old ++ = *in_old ++;    /* keep ID in old_class but change position */

      else if (*in_old > *in_arcs)    /* ID not in old_class but in arc_labels */
	in_arcs ++;                 /* ignore ID */

      else    /*  *in_old == *in_arcs,  ID in old_class and in arc_labels */
	{
	  *to_new ++ = *in_old ++;    /* move ID to new_class */
	  in_arcs ++;
	}
    }

  ALPH_len (new_class) = to_new - ALPH_items (new_class);

  if (ALPH_len (new_class) > ALPH_max (new_class))
    prog_error ("extract_new_eqv_class", "alphabet overflow");

  /* handle old_class */

  if (in_old < end_old)  /* more ID still in old_class */
    memmove (to_old, in_old, (end_old - in_old) * sizeof (id_type));

  ALPH_len (old_class) -= ALPH_len (new_class);

  if (! ALPH_len (old_class))    /* old_class is empty */
    {
      memmove (ALPH_items (old_class), ALPH_items (new_class),
	       ALPH_len (new_class) * sizeof (id_type));

      ALPH_len (old_class) = ALPH_len (new_class);
      ALPH_len (new_class) = 0;
    }
}

static void partition_onesided_eqv_classes
  (ALPHTRANSptr eqv, ALPHABETptr arc_labels, ALPHABETptr new_class,
   ARCptr arc, ARCptr arc2, id_type *in_match)
{
  ALPHABETptr old_class;  int i, len;

  /* collect all input symbols with same output symbol and same destination */

  ALPH_len (arc_labels) = 0;

  for (  ;  arc != arc2;  arc = next_arc (arc))
    ALPH_item (arc_labels, ALPH_len (arc_labels)++) = in_match [ARC_label (arc)];

  sort_alph_on_increasing_ids (arc_labels);

  /* partition existing equivalence classes */

  len = eqv->len;  /* eqv->len will change during loop; therefore we save it */

  for (i = 0;  i < len;  i ++)
    {
      old_class = eqv->symbols [i].out_symbols;
      extract_new_eqv_class (old_class, arc_labels, new_class);

      if (ALPH_len (new_class))    /* new class exists */
	insert_symbol_set_into_table (eqv, new_class, ID_NO_SYMBOL, eqv->len);
    }
}

static ALPHTRANSptr search_onesided_equivalence_classes
  (NETptr fst, ALPHABETptr symbols, int side_p)
{
  /* Returns equivalence classes, or NULL if there are only sigleton
     classes. In this cases no reduction is possible.
     NOTE: None of the arguments are modified. */

  ALPHTRANSptr eqv;  ALPHABETptr arc_labels, new_class;
  STATEptr state;  ARCptr arc, arc2;
  id_type *in_match;  int num_symbols;
  int (*arc_comp_fn)(const void *, const void *);

  /* put all relevant symbols into initial equivalence class */

  if (! (eqv = make_onesided_initial_equivalence_class (fst, symbols, side_p)))
    return (NULL);    /* no symbols to be reduced */

  num_symbols = num_symbols_in_alph (eqv->symbols->out_symbols);

  /* prepare the FST */

  arc_labels = make_alph (ALPH_len (NET_labels (fst)), LABEL_VECTOR);
  new_class = make_alph (ALPH_len (NET_labels (fst)), LABEL_VECTOR);

  arc_comp_fn = (side_p == UPPER) ?
    increasing_lower_id_and_dest_addr_comp_fn :
    increasing_upper_id_and_dest_addr_comp_fn ;

  reorder_arcs (fst, arc_comp_fn);
  assign_match_tables (&in_match, NULL, side_p);

  /* partition iteratively the existing equivalence classes */

  for (state = NET_states (fst);  state;  state = next_state (state))
    for (arc = STATE_arc_set (state);  arc;  arc = arc2)
      for (arc2 = next_arc (arc);  ;  arc2 = next_arc (arc2))
	if ((! arc2) || (arc_comp_fn ((void *) &arc, (void *) &arc2)))
	  {                                       /* end of arc subset reached */
	    partition_onesided_eqv_classes
	      (eqv, arc_labels, new_class, arc, arc2, in_match);

	    if (eqv->len == num_symbols)  /* maximal partitioning reached */
	      {
		free_alph (arc_labels);
		free_alphabet_transduction (eqv); /* only singleton classes */
		return (NULL);
	      }

	    break;    /* go to next arc subset */
	  }

  /* finish */

  free_alph (arc_labels);
  free_alph (new_class);

  return (eqv);
}

/*-----------------------------------------------------------*/
static void note_symbol_recoding
  (ALPHABETptr recode, id_type id, id_type represent)
{
  int i;

  if (id >= ALPH_max (recode))
    prog_error ("note_symbol_recoding", "Recoding alphabet too short");

  if (id >= ALPH_len (recode))    /* length must ne extended */
    {
      for (i = ALPH_len (recode);  i < id;  i ++)
	ALPH_item (recode, i) = ID_NO_SYMBOL;

      ALPH_len (recode) = id + 1;
    }

  ALPH_item (recode, id) = represent;
}

static ALPHABETptr build_symbol_recode_alph (ALPHTRANSptr eqv)
{
  /* Returns and alphabet for recoding all symbols in "eqv" (equivalence
     classes) by one representative of each class.
     Symbols that will not be recoded get here the value ID_NO_SYMBOL. */

  ALPHABETptr recode, class;  id_type represent;  int i, j;

  recode = make_alph (LIM_id_count (LABEL_MAP), BINARY_VECTOR);
  ALPH_len (recode) = 0;

  for (i = 0;  i < eqv->len;  i ++)
    {
      class = eqv->symbols [i].out_symbols;

      if (num_symbols_in_alph (class) > 1)    /* recoding is meaningful */
	{
	  class = copy_alphabet (class);
	  sort_alph (class);

	  represent = ALPH_item (class, 0);

	  for (j = 1;  j < ALPH_len (class);  j ++)
	    note_symbol_recoding (recode, ALPH_item (class, j), represent);

	  free_alph (class);
	}
    }

  return (recode);
}

/*-----------------------------------------------------------*/
static NETptr recode_fst_oneside
  (NETptr fst, ALPHABETptr recode, int side_p, int compact_p)
{
  /* Recodes symbols on the given side of the fst. */

  STATEptr state;  ARCptr arc;
  id_type *side_match, *oppos_match, side_id, reco_id;

  assign_match_tables (&side_match, &oppos_match, side_p);

  for (state = NET_states (fst);  state;  state = next_state (state))
    for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
      {
	side_id = side_match [ARC_label (arc)];

	if (ALPH_len (recode) > side_id)
	  if ((reco_id = ALPH_item (recode, side_id)) != ID_NO_SYMBOL)
	    {
	      ARC_label (arc) = make_dir_pair_label
		(reco_id, oppos_match [ARC_label (arc)], side_p);

	      STATE_deterministic (state) = FALSE;
	    }
      }

  update_net_labels_and_sigma (fst);
  fst = force_minimize (fst, DONT_COPY, compact_p);

  return (fst);
}

/*-----------------------------------------------------------*/
NVptr reduce_intermediate_alphabet_in_pair
  (NVptr nv, ALPHABETptr midsyms, int side_p, int copy_p, int compact_p,
   int verbose_p)
{
  /* Reduces the number of intermediate symbols "midsyms", between the
     two FSTs of "nv". "midsyms" contains all symbols that are allowed
     to be replaced. If "midsyms" is NULL, it is set to the input
     alphabet of the second net.
     First, all intermediate symbols are grouped into equivalence classes
     that cause the same final output symbol. Then, all intermediate
     symbols are replaced (in both FSTs) by the representative of their
     class.
     NOTE: the argument "nv" is modified, "midsyms" is preserved. */

  NETptr fa1, fa2;  ALPHTRANSptr eqv;  ALPHABETptr recode;

  fa1 = min_fsm (NV_net (nv, 0), copy_p);  /* order [ fa1, fa2 ] indepd. of side_p */
  fa2 = min_fsm (NV_net (nv, 1), copy_p);

  print_comment (verbose_p, "Input transducers:\n");
  print_comment (verbose_p, "  %s\n", temp_net_info (fa1, side_p));
  print_comment (verbose_p, "  %s\n", temp_net_info (fa2, side_p));

  /* define equivalence classes and recoding */

  print_comment (verbose_p, "Defining equivalence classes ... ");
  eqv = search_onesided_equivalence_classes (fa2, midsyms, side_p);
  print_comment (verbose_p, "done\n");

  if (! eqv)
    {
      print_comment (verbose_p, "No reduction possible\n");
      return (nv);
    }

  recode = build_symbol_recode_alph (eqv);

  /* recode factors */

  print_comment (verbose_p, "Recoding factors ... ");
  fa1 = recode_fst_oneside (fa1, recode, opposite_side (side_p), compact_p);
  fa2 = recode_fst_oneside (fa2, recode, side_p, compact_p);
  print_comment (verbose_p, "done\n");

  /* finish */

  free_alphabet_transduction (eqv);
  free_alph (recode);

  print_comment (verbose_p, "Output transducers:\n");
  print_comment (verbose_p, "  %s\n", temp_net_info (fa1, side_p));
  print_comment (verbose_p, "  %s\n", temp_net_info (fa2, side_p));

  if (copy_p == DO_COPY)
    nv = make_nv (2);

  NV_net (nv, 0) = fa1;
  NV_net (nv, 1) = fa2;

  return (nv);
}

NVptr reduce_intermediate_alphabet_in_nv
  (NVptr nv, ALPHABETptr midsyms, int side_p, int copy_p, int compact_p,
   int verbose_p)
{
  /* Reduces the number of intermediate symbols "midsyms", between all
     FSTs of "nv". The FSTs are handled pair-wise.
     "midsyms" contains all symbols that are allowed to be replaced.
     If "midsyms" is NULL, all intermediate symbols are can be replaced.
     First, all intermediate symbols are grouped into equivalence classes
     that cause the same final output symbol. Then, all intermediate
     symbols are replaced (in both FSTs) by the representative of their
     class.
     NOTE: the argument "nv" is modified, "midsyms" is preserved. */

  NVptr nv2;  int i;

  if (copy_p == DO_COPY)
    nv = copy_nv_and_nets (nv);

  nv2 = make_nv (2);

  for (i = NV_len (nv) - 2;  i >= 0;  i --)
    {
      NV_net (nv2, 0) = NV_net (nv, i);
      NV_net (nv2, 1) = NV_net (nv, i + 1);

      nv2 = reduce_intermediate_alphabet_in_pair
	(nv2, midsyms, side_p, DONT_COPY, compact_p, verbose_p);

      NV_net (nv, i) = NV_net (nv2, 0);
      NV_net (nv, i + 1) = NV_net (nv2, 1);
    }

  free_nv_only (nv2);
  return (nv);
}

/************************************************************
 *
 * UNFOLD THE FST LOCALLY.
 *
 *   Duplicate branches of an FST F, so that if a deterministic
 *   automaton D, that corresponds to the input side of F, is
 *   created and all states of D are linked to sets of states
 *   of F then every state of F is in exactly one set.
 *   With a general FST this is not the case.
 */

static void copy_state_numbers_to_twin_net (NETptr hyper_net)
{
  STATEptr hstate;  PRSTLISTptr pair;

  for (hstate = NET_states (hyper_net);  hstate;  hstate = next_state (hstate))
    for (pair = (PRSTLISTptr) STATE_client_cell (hstate);
	 pair;  pair = pair->next)
      STATE_client_cell (pair->state2) = STATE_client_cell (pair->state1);
}

static void unfold_arcs_to_twin_state
  (STATEptr hyper_source, STATEptr orig_source, STATEptr twin_source,
   NETptr twin_net, id_type *in_match)
{
  PRSTLISTptr dest_pair;
  STATEptr hyper_dest, orig_dest, twin_dest;
  ARCptr hyper_arc, orig_arc;

  for (orig_arc = STATE_arc_set (orig_source);
       orig_arc;
       orig_arc = next_arc (orig_arc))
    {
      for (hyper_arc = STATE_arc_set (hyper_source);
	   hyper_arc;
	   hyper_arc = next_arc (hyper_arc))
	if (ARC_label (hyper_arc) == in_match [ARC_label (orig_arc)])
	  break;    /* corresponding hyper_arc found */

      if (! hyper_arc)
	prog_error ("unfold_arcs_to_twin_state", "#1");

      hyper_dest = ARC_destination (hyper_arc);
      orig_dest = ARC_destination (orig_arc);

      for (dest_pair = (PRSTLISTptr) STATE_client_cell (hyper_dest);
	   dest_pair;
	   dest_pair = dest_pair->next)
	if (dest_pair->state1 == orig_dest)
	  break;    /* corresponding dest_pair found */

      if (! dest_pair)
	prog_error ("unfold_arcs_to_twin_state", "#2");

      twin_dest = dest_pair->state2;
      insert_sort_arc (twin_source, ARC_label (orig_arc), twin_dest, twin_net);
    }
}

static void unfold_arcs_to_twin_net
  (NETptr hyper_net, NETptr twin_net, STATEptr orig_start, id_type *in_match)
{
  /* Copies arcs from original net to twin_net.
     Sets start_state of twin_net. */

  PRSTLISTptr pair;  STATEptr hyper_state;

  for (hyper_state = NET_states (hyper_net);
       hyper_state;
       hyper_state = next_state (hyper_state))
    for (pair = (PRSTLISTptr) STATE_client_cell (hyper_state);
	 pair;
	 pair = pair->next)
      {
	if (pair->state1 == orig_start)
	  NET_start_state (twin_net) = pair->state2;

	unfold_arcs_to_twin_state
	  (hyper_state, pair->state1, pair->state2, twin_net, in_match);
      }
}

static NETptr unfold_locally (NETptr orig_net, int side_p, int reverse_p)
{
  /* Duplicate branches of orig_net, so that if a left-deterministic
     input net D is created and all states of D are linked to
     sets of states in orig_net, every state of orig_net is in exactly
     one set. With a general FST this is not the case.
     If (reverse_p==TRUE) then D is right-deterministic.
     The argument orig_net is modified. */

  HEAPptr state_heap, pair_heap;
  NETptr hyper_net, twin_net;  id_type *in_match;
  void **client_cells;

  /* prepare */

  assign_match_tables (&in_match, NULL, side_p);

  state_heap = init_heap
    (sizeof (STATELISTtype), STATE_LIST_BLOCK_SIZE, "State list cells");

  pair_heap = init_heap
    (sizeof (PRSTLISTtype), PAIR_STATE_LIST_BLOCK_SIZE, "Pair state cells");

  /* make hyper net */

  client_cells = save_state_client_cells (orig_net);

  hyper_net = make_determ_input_net
    (orig_net, state_heap, in_match, side_p, reverse_p);  /* +++ uses CCells */

  if (! hyper_net)        /* too deep epsilon recursion */
    return (NULL);

  /* make enhanced twin_net */

  if (reverse_p)
    turn_all_arcs (orig_net);  /* +++ uses CCells */

  twin_net = make_state_pairs (hyper_net, state_heap, pair_heap);
  free_heap (state_heap);

  restore_state_client_cells (client_cells, orig_net);
  free (client_cells);

  copy_state_numbers_to_twin_net (hyper_net);

  unfold_arcs_to_twin_net
    (hyper_net, twin_net, NET_start_state (orig_net), in_match);

  if (reverse_p)
    {
      client_cells = save_state_client_cells (twin_net);
      turn_all_arcs (twin_net);  /* +++ uses CCells */

      restore_state_client_cells (client_cells, twin_net);
      free (client_cells);
    }

  NET_labels (twin_net) = NET_labels (orig_net);
  NET_labels (orig_net) = NULL;

  NET_sigma (twin_net) = NET_sigma (orig_net);
  NET_sigma (orig_net) = NULL;

  /* free and return */

  free_heap (pair_heap);
  free_network (orig_net);
  free_network (hyper_net);

  return (twin_net);
}



/************************************************************
 *
 * SPLIT FST INTO A FUNCTIONAL AND A SMALL NON-FUNCTIONAL FST
 *
 */

static void attach_state_info (NETptr fst, HEAPptr state_info_heap)
{
  /* Attaches to (the client cell of) every state in the original fst,
     a state info cell. The state number (previously stored in the
     client cell) is copied to this state info cell. */

  STATEptr state;  FCT2_STATEINFOptr info;

  for (state = NET_states (fst);  state;  state = next_state (state))
    {
      info = (FCT2_STATEINFOptr) cell_alloc (state_info_heap);

      info->state_num = (long) STATE_client_cell (state);
      info->det_right = NULL;
      info->fctfst_state = NULL;
      info->ambfst_state = NULL;

      STATE_client_cell (state) = (void *) info;
    }
}

static void set_and_check_hyper_dependecies (NETptr hyper_net, NETptr orig_net)
{
  STATELISTptr statelist;  FCT2_STATEINFOptr info;
  STATEptr hyper_state, orig_state;

  /* clean hyper dependecies */

  for (orig_state = NET_states (orig_net);
       orig_state;  orig_state = next_state (orig_state))
    {
      info = (FCT2_STATEINFOptr) STATE_client_cell (orig_state);
      info->det_right = NULL;
    }

  /* set and check hyper dependecies */

  for (hyper_state = NET_states (hyper_net);
       hyper_state;  hyper_state = next_state (hyper_state))
    for (statelist = (STATELISTptr) STATE_client_cell (hyper_state);
	 statelist;  statelist = statelist->next)
      {
	orig_state = statelist->state;
	info = (FCT2_STATEINFOptr) STATE_client_cell (orig_state);

	if (! info->det_right)
	  info->det_right = hyper_state;
	else if (info->det_right != hyper_state)
	  prog_error ("set_and_check_hyper_dependecies", "FST not unfolded");
      }
}

static void make_state_triplets (NETptr fst, NETptr fct_fst, NETptr amb_fst)
{
  STATEptr state;  FCT2_STATEINFOptr info;

  for (state = NET_states (fst);  state;  state = next_state (state))
    {
      info = (FCT2_STATEINFOptr) STATE_client_cell (state);

      info->fctfst_state = make_state (NULL, fct_fst);
      info->ambfst_state = make_state (NULL, amb_fst);

      STATE_final (info->fctfst_state) = STATE_final (state);
    }

  info = (FCT2_STATEINFOptr) STATE_client_cell (NET_start_state (fst));
  NET_start_state (fct_fst) = info->fctfst_state;
  NET_start_state (amb_fst) = info->ambfst_state;

  STATE_final (NET_start_state (amb_fst)) = TRUE;
}

/*-----------------------------------------------------------*/
static void copy_arcs_to_arc_vector
  (FCT2_ARCVECptr arcvec, STATEptr hyper_source,
   id_type *in_match, id_type *out_match)
{
  STATELISTptr statelist;
  FCT2_STATEINFOptr source_info, dest_info;  FCT2_ARCptr arcinfo;
  STATEptr orig_source, orig_dest;  ARCptr orig_arc;

  for (statelist = (STATELISTptr) STATE_client_cell (hyper_source);
       statelist;
       statelist = statelist->next)
    {
      orig_source = statelist->state;
      source_info = (FCT2_STATEINFOptr) STATE_client_cell (orig_source);

      for (orig_arc = STATE_arc_set (orig_source);
	   orig_arc;
	   orig_arc = next_arc (orig_arc))
	{
	  orig_dest = ARC_destination (orig_arc);
	  dest_info = (FCT2_STATEINFOptr) STATE_client_cell (orig_dest);

	  if (arcvec->len == arcvec->max)
	    enhance_fct2_arc_vector (arcvec);

	  arcinfo = arcvec->vector + (arcvec->len ++);

	  arcinfo->source = orig_source;
	  arcinfo->dest = orig_dest;
	  arcinfo->source_num = source_info->state_num;
	  arcinfo->dest_num = dest_info->state_num;
	  arcinfo->hyper_dest = dest_info->det_right;

	  arcinfo->pair_id = ARC_label (orig_arc);
	  arcinfo->in_id = in_match [ARC_label (orig_arc)];
	  arcinfo->out_id = out_match [ARC_label (orig_arc)];
	}
    }
}

static int cmp_factorization_arc_info (FCT2_ARCptr a, FCT2_ARCptr b)
{
  int cmp;

  if ((cmp = b->in_id - a->in_id))
    return (cmp);

  if ((cmp = b->hyper_dest - a->hyper_dest))
    return (cmp);

  if ((cmp = b->source_num - a->source_num))
    return (cmp);

  if ((cmp = b->dest_num - a->dest_num))
    return (cmp);

  if ((cmp = b->pair_id - a->pair_id))
    return (cmp);

  return (0);
}

static int partition_arc_vector (FCT2_ARCVECptr vec)
{
  /* Set "vec->sub.vector" and "vec->sub.len" so that all arc in this
     partition have the same "in_id" and "hyper_dest".
     Returns FALSE when there are no more partitions left over. */

  FCT2_ARCptr beg, cur, end;

  if (! vec->len)    /* no arcs collected into vector */
    return (FALSE);

  if (! vec->sub.vector)
    vec->sub.vector = vec->vector;
  else
    vec->sub.vector += vec->sub.len;

  beg = vec->sub.vector;
  end = vec->vector + vec->len;

  if (beg == end)    /* no more partitions left over */
    return (FALSE);

  for (cur = beg + 1;  cur < end;  cur ++)
    if ((cur->in_id != beg->in_id)
	|| (cur->hyper_dest != beg->hyper_dest))
      break;

  vec->sub.len = cur - beg;

  return (TRUE);    /* partition found */
}

/*-----------------------------------------------------------*/
static int is_fork_in_partition (FCT2_ARCVECptr vec)
{
  /* There is a fork if at least one arc of the partition has the
     same input label and source number as another arc in the
     arc vector that is not member of the partition (i.e. it has
     a different hyper destination. */

  FCT2_ARCptr arc1, arc2, end1, end2;

  if (vec->len == vec->sub.len)  /* only 1 partition */
    return (FALSE);

  /* initialize */

  arc1 = vec->sub.vector;
  end1 = arc1 + vec->sub.len;

  arc2 = vec->vector;
  end2 = arc2 + vec->len;

  /* search for fork */

  for (  ;  arc1 < end1;  arc1 ++)    /* traverse partition */
    for (  ;  arc2 < end2;  arc2 ++)    /* traverse whole vector */
      if ((arc1->in_id == arc2->in_id) &&          /* same input id */
	  (arc1->source_num == arc2->source_num) &&  /* same source */
	  (arc1->hyper_dest != arc2->hyper_dest))  /* diff hyper dest */
	return (TRUE);

  return (FALSE);
}

static void set_partition_flags (FCT2_ARCVECptr vec)
{
  /* Sets partition flags. */

  FCT2_ARCptr beg, cur, end;
  short same_pair_id, same_source_num, same_dest_num, other_p;

  /* initialize */

  same_pair_id = same_source_num = same_dest_num = TRUE;
  other_p = FALSE;

  beg = vec->sub.vector;
  end = beg + vec->sub.len;

  /* traverse partition */

  for (cur = beg + 1;  cur < end;  cur ++)
    {
      if (cur->pair_id != beg->pair_id)
	same_pair_id = FALSE;

      if (cur->source_num != beg->source_num)
	same_source_num = FALSE;

      if (cur->dest_num != beg->dest_num)
	same_dest_num = FALSE;

      if ((cur->in_id == OTHER) && (cur->out_id == OTHER))
	other_p = TRUE;
    }

  /* set flags */

  if ((same_pair_id) && (same_source_num) && (same_dest_num))
    {
      vec->sub.flags.amb = FALSE;
      vec->sub.flags.beg = FALSE;
      vec->sub.flags.end = FALSE;
    }
  else
    {
      vec->sub.flags.amb = TRUE;
      vec->sub.flags.beg = (same_source_num);
      vec->sub.flags.end = (same_dest_num);
    }

  vec->sub.flags.other = other_p;
  vec->sub.flags.fork = is_fork_in_partition (vec);
}

/*-----------------------------------------------------------*/
static id_type define_middle_id_for_partition
  (FCT2_ARCVECptr arcvec, ALPHABETptr *amb_alph)
{
  FCT2_ARCptr arcinfo;  id_type mid_id;

  arcinfo = arcvec->sub.vector;

  if (arcinfo->pair_id == BOUNDARY_SYMBOL)
    {
      if ((arcvec->sub.flags.beg) ||
	  ((arcvec->sub.flags.amb) && (! arcvec->sub.flags.end)))
	prog_error ("define_middle_id_for_partition", "#1");

      mid_id = BOUNDARY_SYMBOL;
    }
  else if (! arcvec->sub.flags.amb)  /* partition outside amb area */
    {
      mid_id = arcinfo->out_id;
    }
  else        /* partition inside amb area */
    {
      if (arcvec->sub.flags.beg)    /* ambiguity begins with partition */
	{
	  mid_id = make_diacritic (FCT2_AMB_BEGIN, arcvec->sub.index.beg ++);
	  sigma_add_to (*amb_alph, mid_id);
	}
      else if (arcvec->sub.flags.fork)  /* fork in partition */
	{
	  mid_id = make_diacritic (FCT2_AMB_MIDDLE, arcvec->sub.index.mid ++);
	  sigma_add_to (*amb_alph, mid_id);
	}
      else
	{
	  mid_id = arcinfo->in_id;
	}
    }

  return (mid_id);
}

/*-----------------------------------------------------------*/
static void copy_arc_to_factor
  (NETptr factor, FCT2_ARCVECptr arcvec, FCT2_ARCptr arcinfo,
   id_type mid_id, int side_p, int fctfst_p)
{
  /* Copies an arc to a factor fst. */

  FCT2_STATEINFOptr source_info, dest_info;
  STATEptr source, dest, midstate;  id_type pair1, pair2;

  /* initialize */

  source_info = (FCT2_STATEINFOptr) STATE_client_cell (arcinfo->source);
  dest_info = (FCT2_STATEINFOptr) STATE_client_cell (arcinfo->dest);

  if (fctfst_p)
    {
      source = source_info->fctfst_state;
      dest = dest_info->fctfst_state;
    }
  else
    {
      source = source_info->ambfst_state;
      dest = dest_info->ambfst_state;
    }

  /* make pair IDs for different cases */

  if (! arcvec->sub.flags.amb)  /* non-amb area */
    {
      if (fctfst_p)
	pair1 = arcinfo->pair_id;
      else
	pair1 = arcinfo->out_id;

      pair2 = EPSILON;
    }
  else if (! arcvec->sub.flags.other)  /* amb area without ? or ?:? */
    {
      if (fctfst_p)
	pair1 = make_dir_pair_label (arcinfo->in_id, mid_id, side_p);
      else
	pair1 = make_dir_pair_label (mid_id, arcinfo->out_id, side_p);

      pair2 = EPSILON;
    }
  else        /* amb area with ? or ?:? */
    {
      if (fctfst_p)
	{
	  pair1 = make_dir_pair_label (EPSILON, mid_id, side_p);
	  pair2 = arcinfo->in_id;
	}
      else
	{
	  pair1 = make_dir_pair_label (mid_id, EPSILON, side_p);
	  pair2 = arcinfo->pair_id;
	}
    }

  /* make arcs */

  if (pair2 == EPSILON)    /* only 1 arc */
    {
      insert_sort_arc (source, pair1, dest, factor);
    }
  else        /* chain of 2 arcs */
    {
      midstate = make_state (NULL, factor);
      insert_sort_arc (source, pair1, midstate, factor);
      insert_sort_arc (midstate, pair2, dest, factor);
    }
}

static void copy_partition_to_factors
  (NETptr fctfst, NETptr ambfst, FCT2_ARCVECptr arcvec,
   id_type mid_id, int side_p)
{
  FCT2_STATEINFOptr source_info, dest_info;  FCT2_ARCptr cur, end;

  cur = arcvec->sub.vector;
  end = cur + arcvec->sub.len;

  for (  ;  cur < end;  cur ++)
    {
      /* ---- copy to fct fst */
      copy_arc_to_factor (fctfst, arcvec, cur, mid_id, side_p, TRUE);

      if (arcvec->sub.flags.amb)    /* ambiguity exists in partition */
	{
	  /* ---- copy to amb fst */
	  copy_arc_to_factor (ambfst, arcvec, cur, mid_id, side_p, FALSE);

	  source_info = (FCT2_STATEINFOptr) STATE_client_cell (cur->source);
	  dest_info = (FCT2_STATEINFOptr) STATE_client_cell (cur->dest);

	  if (arcvec->sub.flags.beg)  /* ambiguity begins with partition */
	    insert_sort_arc (NET_start_state (ambfst), BOUNDARY_SYMBOL,
			     source_info->ambfst_state, ambfst);

	  if (arcvec->sub.flags.end)  /* ambiguity ends with partition */
	    insert_sort_arc (dest_info->ambfst_state, BOUNDARY_SYMBOL,
			     NET_start_state (ambfst), ambfst);
	}
      else        /* no ambiguity in partition */
	{
	  insert_sort_arc (NET_start_state (ambfst), cur->out_id,
			   NET_start_state (ambfst), ambfst);

	}
    }
}

/*-----------------------------------------------------------*/
static void copy_arcs_to_factors
  (NETptr fst, NETptr det_left, NETptr fctfst, NETptr ambfst,
   ALPHABETptr *amb_alph, int side_p, int verbose_p)
{
  FCT2_ARCVECptr arcvec;  STATEptr hyper_source;
  id_type *in_match, *out_match, mid_id;  long idx_left;

  /* prepare */

  *amb_alph = make_alph (LIM_id_count (LABEL_MAP), BINARY_VECTOR);
  arcvec = make_fct2_arc_vector ();

  assign_match_tables (&in_match, &out_match, side_p);

  /* copy arcs */

  print_dot_for_n_states (det_left, 0, verbose_p);  /* initialize */

  for (hyper_source = NET_states (det_left), idx_left = 0;
       hyper_source;
       hyper_source = next_state (hyper_source), idx_left ++)
    {
      print_dot_for_n_states (NULL, idx_left, verbose_p);

      clean_fct2_arc_vector (arcvec);
      copy_arcs_to_arc_vector (arcvec, hyper_source, in_match, out_match);

      qsort (arcvec->vector, arcvec->len, sizeof (FCT2_ARCtype),
	     (int (*)(const void *, const void *)) cmp_factorization_arc_info);

      while (partition_arc_vector (arcvec))
	{
	  set_partition_flags (arcvec);
	  mid_id = define_middle_id_for_partition (arcvec, amb_alph);
	  copy_partition_to_factors (fctfst, ambfst, arcvec, mid_id, side_p);
	}
    }

  /* finish */

  binary_to_label (*amb_alph);
  free_fct2_arc_vector (arcvec);
}

/*-----------------------------------------------------------*/
static NETptr prefinalize_factor
  (NETptr fst, NETptr factor, int side_p, int compact_p, int fctfst_p)
{
  /* Removes BOUNDARY_SYMBOL, undates alphabets, and minimizes the net.
     Does not remove DET_EPSILON which is still needed for p_seq test
     and factor compaction. */

  NET_labels (factor) = copy_alphabet (NET_labels (fst));
  NET_sigma (factor) = copy_alphabet (NET_sigma (fst));

  if (! fctfst_p)
    insert_sort_arc
      (NET_start_state (factor), OTHER, NET_start_state (factor), factor);

  factor = replace_label
    (factor, BOUNDARY_SYMBOL, EPSILON, DO_MINIMIZE, compact_p);

  return (factor);
}

/*-----------------------------------------------------------*/
static NVptr copy_fst_to_factors
  (NETptr fst, ALPHABETptr *amb_alph, int side_p, int compact_p, int verbose_p)
{
  HEAPptr state_list_heap, state_info_heap;
  NETptr det_left, det_right, fct_fst, amb_fst;  NVptr factors;
  id_type *in_match;  void **client_cells;

  /* prepare */

  assign_match_tables (&in_match, NULL, side_p);

  state_list_heap = init_heap
    (sizeof (STATELISTtype), STATE_LIST_BLOCK_SIZE, "State list cells");

  state_info_heap = init_heap
    (sizeof (FCT2_STATEINFOtype), FCT2_STATE_INFO_BLOCK_SIZE,
     "State info (fct2)");

  /* make hyper nets */

  client_cells = save_state_client_cells (fst);

  det_left = make_determ_input_net
    (fst, state_list_heap, in_match, side_p, FALSE);  /* +++ uses CCells */

  det_right = make_determ_input_net
    (fst, state_list_heap, in_match, side_p, TRUE);  /* +++ uses CCells */

  restore_state_client_cells (client_cells, fst);
  free (client_cells);

  attach_state_info (fst, state_info_heap);
  set_and_check_hyper_dependecies (det_left, fst);
  set_and_check_hyper_dependecies (det_right, fst);

  /* make factors: fct_fst and amb_fst */

  fct_fst = make_network ();
  amb_fst = make_network ();

  make_state_triplets (fst, fct_fst, amb_fst);
  copy_arcs_to_factors
    (fst, det_left, fct_fst, amb_fst, amb_alph, side_p, verbose_p);

  fct_fst = prefinalize_factor (fst, fct_fst, side_p, compact_p, TRUE);
  amb_fst = prefinalize_factor (fst, amb_fst, side_p, compact_p, FALSE);

  factors = make_nv (2);
  NV_net (factors, 0) = fct_fst;
  NV_net (factors, 1) = amb_fst;

  /* free and return */

  free_heap (state_list_heap);
  free_heap (state_info_heap);

  return (factors);
}


/************************************************************
 *
 * FUNCTIONS FOR FACTOR COMPRESSION
 *
 */

static void compress_factors
  (NVptr factors, ALPHABETptr amb_alph, int side_p, int compact_p)
{
  NETptr fctfst, ambfst;
  /*  ALPH_VECTORptr eqv_classes;  ALPHABETptr singletons; */

  fctfst = NV_net (factors, 0);
  ambfst = NV_net (factors, 1);

  /* TO BE IMPLEMENTED ---- RELATION IS MORE COMLICATED THAN EQV_CLASSES */

}

/************************************************************
 *
 * FUNCTIONS FOR CHECKING THE RESULT
 *
 */

static int same_input_symbols
  (STATELISTptr statelist, ALPHABETptr insym, ALPHABETptr insym2,
   id_type *in_match)
{
  STATEptr state;  ARCptr arc;  int i, final_p, count;  id_type sym;

  if (! statelist->next)  /* only 1 state in set */
    return (TRUE);

  /* make the test */

  memset (ALPH_items (insym), 0, ALPH_len (insym) * sizeof (id_type));

  for (count = 0;  statelist;  count++, statelist = statelist->next)
    {
      state = statelist->state;
      memset (ALPH_items (insym2), 0, ALPH_len (insym2) * sizeof (id_type));

      /* ---- check finality */
      if (! count)
	final_p = STATE_final (state);    /* initialize */
      else if (final_p != STATE_final (state))
	return (FALSE);    /* different finality */

      /* ---- count outgoing input symbols */
      for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
	{
	  sym = in_match [ARC_label (arc)];

	  if (! ALPH_item (insym2, sym))  /* not yet found with current state */
	    {
	      ALPH_item (insym2, sym) = TRUE;  /* found with current state */
	      ALPH_item (insym, sym) ++;       /* add to state symbol count */
	    }
	}
    }

  /* check outgoing input symbols */

  for (i=0;  i< ALPH_len (insym);  i++)
    if ((ALPH_item (insym, i) > 0) && (ALPH_item (insym, i) != count))
      return (FALSE);    /* some states do not have this symbol */

  return (TRUE);
}

int check_fail_safety (NETptr fst, int side_p)
{
  /* Returns TRUE if every fst state that is member of the same set
     associated with a (deterministic input) hyper net state,
     has exactly the same outgoing input symbols and finality. */

  HEAPptr state_heap;  NETptr hyper_net;  STATEptr hstate;
  ALPHABETptr insym, insym2;  id_type *in_match;
  int fail_safe_p;

  /* prepare */

  fail_safe_p = TRUE;
  assign_match_tables (&in_match, NULL, side_p);

  state_heap = init_heap
    (sizeof (STATELISTtype), STATE_LIST_BLOCK_SIZE, "State list cells");

  insym = make_alph (LIM_id_count (LABEL_MAP), BINARY_VECTOR);
  insym2 = make_alph (LIM_id_count (LABEL_MAP), BINARY_VECTOR);

  /* make hyper net */

  hyper_net = make_determ_input_net
    (fst, state_heap, in_match, side_p, FALSE);

  if (! hyper_net)        /* too deep epsilon recursion */
    {
      fprintf (stderr, "too deep epsilon recursion");
      fail_safe_p = FALSE;
    }

  /* make the test */

  if (fail_safe_p)
    for (hstate = NET_states (hyper_net);  hstate;  hstate = next_state (hstate))
      if (! same_input_symbols ((STATELISTptr) STATE_client_cell (hstate),
				insym, insym2, in_match))
	{
	  fail_safe_p = FALSE;
	  break;
	}

  /* finish */

  free_network (hyper_net);
  free_heap (state_heap);
  free_alph (insym);
  free_alph (insym2);

  return (fail_safe_p);
}

/************************************************************
 *
 * PRINCIPLE FACTORIZATION FUNCTIONS (FOR FINITELY AMBIGUOUS FSTs)
 *
 */

static void finalize_finamb_factors
  (NVptr factors, ALPHABETptr amb_alph, int side_p, int compact_p)
{
  NETptr fctfst, ambfst;

  compress_factors (factors, amb_alph, side_p, compact_p);

  fctfst = NV_net (factors, 0);
  ambfst = NV_net (factors, 1);

  if (! check_fail_safety (ambfst, side_p))
    prog_error ("check_fail_safety", "test failed");

  fctfst = replace_symbol (fctfst, DET_EPSILON, EPSILON, DO_MINIMIZE, compact_p);
  ambfst = replace_symbol (ambfst, DET_EPSILON, EPSILON, DO_MINIMIZE, compact_p);

  if ((sigma_member (NET_sigma (fctfst), DET_EPSILON)) ||
      (sigma_member (NET_sigma (fctfst), BOUNDARY_SYMBOL)) ||
      (sigma_member (NET_sigma (ambfst), DET_EPSILON)) ||
      (sigma_member (NET_sigma (ambfst), BOUNDARY_SYMBOL)))
    prog_error ("finalize_factors", "auxiliary symbols not removed");

  NV_net (factors, 0) = fctfst;
  NV_net (factors, 1) = ambfst;
}

NVptr factorize_finamb_fst
  (NETptr fst, int side_p, int copy_p, int compact_p, int alphreduce_p,
   int verbose_p)
{
  NVptr factors;  ALPHABETptr amb_alph;

  /* prepare */

  fst = min_fsm (fst, copy_p);

  print_comment
    (verbose_p, "Input transducer:\n  %s\n", temp_net_info (fst, side_p));

  /* align ambiguity */

  if (contains_symbol_on_side (fst, EPSILON, side_p))  /* input EPSILON */
    {
      print_comment (verbose_p, "Aligning ambiguity ... ");

      if (contains_forbidden_symbol (NET_sigma (fst), DET_EPSILON))
	{
	  error_buffer
	    ("put", "FST contains both EPSILON and DET_EPSILON on the input side");
	  return (NULL);
	}

      if (! (fst = align_ambiguity (fst, side_p, DONT_COPY, compact_p)))
	{                /* original FST infinitely ambiguous */
	  error_buffer ("put", "FST contains infinite ambiguity");
	  return (NULL);
	}

      print_comment (verbose_p, "done:\n  %s\n", temp_net_info (fst, side_p));
    }
  else if (contains_symbol_on_side (fst, EPSILON, opposite_side (side_p)))
    fst = replace_oneside_symbol
      (fst, EPSILON, DET_EPSILON, opposite_side (side_p), DO_MINIMIZE, compact_p);

  /* ---> MAKE LABEL REDUCTION AND UN-REDUCTION !!! */

  /* unfold locally left and right side */

  print_comment (verbose_p, "Unfolding left side ... ");
  fst = attach_boundaries (fst, BOUNDARY_SYMBOL, BOUNDARY_SYMBOL);
  set_state_numbers (fst);
  fst = unfold_locally (fst, side_p, FALSE);
  print_comment (verbose_p, "done:\n  %s\n", temp_net_info (fst, side_p));

  print_comment (verbose_p, "Unfolding right side ... ");
  fst = unfold_locally (fst, side_p, TRUE);
  print_comment (verbose_p, "done:\n  %s\n", temp_net_info (fst, side_p));

  /* build two FSTs */

  print_comment (verbose_p, "Building two factors ");

  factors = copy_fst_to_factors (fst, &amb_alph, side_p, compact_p, verbose_p);
  free_network (fst);

  finalize_finamb_factors (factors, amb_alph, side_p, compact_p);
  free_alph (amb_alph);

  print_comment (verbose_p, " done:\n");
  print_comment (verbose_p, "  %s\n", temp_net_info (NV_net (factors, 0), side_p));
  print_comment (verbose_p, "  %s\n", temp_net_info (NV_net (factors, 1), side_p));

  /* reduced the intemediate alphabet */

  if (alphreduce_p)
    {
      print_comment (verbose_p, "Reducing intermediate alphabet ... ");

      factors = reduce_intermediate_alphabet_in_pair
	(factors, NULL, side_p, DONT_COPY, TRUE, FALSE);

      print_comment (verbose_p, "done:\n");
      print_comment (verbose_p, "  %s\n", temp_net_info (NV_net (factors, 0), side_p));
      print_comment (verbose_p, "  %s\n", temp_net_info (NV_net (factors, 1), side_p));
    }

  /* finish */

  return (factors);
}


/************************************************************
 *
 * PRINCIPLE UN-FACTORIZATION FUNCTION (FOR ANY AMBIGUOUS FSTs)
 *
 */

NETptr unfactorize_amb_fsts
  (NETptr fa1, NETptr fa2, int side_p, int copy_p, int verbose_p)
{
  NETptr fst;

  print_comment (verbose_p, "Input transducers:\n");
  print_comment (verbose_p, "  %s\n", temp_net_info (fa1, side_p));
  print_comment (verbose_p, "  %s\n", temp_net_info (fa2, side_p));

  /* compose FSTs */

  print_comment (verbose_p, "Composing transducers ... ");

  if (side_p == UPPER)
    fst = compose (fa1, fa2);
  else
    fst = compose (fa2, fa1);

  fst = force_minimize (fst, DONT_COPY, TRUE);
  print_comment (verbose_p, "done:\n  %s\n", temp_net_info (fst, side_p));

  return (fst);
}


/************************************************************
 *
 * FUNCTIONS FOR EPSILON LOOPS
 *
 */

static void copy_epsilon_loop (ARC_PATHptr path, ARC_PATHptr loops)
{
  /* Copies (appends) the current epsilon loop from "path". */

  long i;

  while (loops->len + path->len + 1 > loops->max)
    enhance_arc_path (loops, loops->max * FCT2_ARC_PATH_REALLOC);

  for (i = 0;  i < path->len;  i ++)
    loops->arcs [loops->len ++] = path->arcs [i];

  loops->arcs [loops->len ++] = NULL;
}

static void follow_epsilon_loops
  (STATEptr lpstate, ARC_PATHptr path, ARC_PATHptr loops, id_type *in_match)
{
  /* Follows all epsilon paths starting at "lpstate", and saves them
     if they are looping on "lpstate". */

  STATEptr dest;  ARCptr arc, arc_ahead;

  arc = STATE_arc_set (lpstate);
  STATE_visit_mark (lpstate) = IN_PROCESS;
  path->len = 1;

  /* traverse the FST */

  while (arc)
    {
      dest = ARC_destination (arc);

      if (in_match [ARC_label (arc)] == EPSILON)    /* follow current arc */
	{
	  /* ---- save current arc to path buffer */
	  if (path->len >= path->max)
	    enhance_arc_path (path, path->max * FCT2_ARC_PATH_REALLOC);

	  path->arcs [path->len - 1] = arc;

	  /* ---- check for loop and for path continuation */
	  if (dest == lpstate)    /* loop on "lpstate" */
	    {
	      copy_epsilon_loop (path, loops);
	      arc_ahead = NULL;    /* no path continuation */
	    }
	  else if (STATE_visit_mark (dest) == IN_PROCESS)
	    {                                      /* loop not on "lpstate" */
	      arc_ahead = NULL;    /* no path continuation */
	    }
	  else
	    arc_ahead = STATE_arc_set (dest);
	}
      else        /* current arc not epsilon ---> do not follow */
	arc_ahead = NULL;

      /* ---- go to next arc */
      if (arc_ahead)    /* first arc of destination state */
	{
	  STATE_visit_mark (dest) = IN_PROCESS;
	  arc = arc_ahead;
	  path->len ++;
	}
      else if (arc->next)    /* next arc of same state */
	{
	  arc = arc->next;
	}
      else        /* next arc of any previous state */
	{
	  arc = NULL;
	  while ((! arc) && (path->len > 1))
	    {
	      path->len --;
	      arc = path->arcs [path->len - 1];
	      STATE_visit_mark (ARC_destination (arc)) = NOT_VISITED;
	      arc = arc->next;
	    }
	}
    }

  STATE_visit_mark (lpstate) = NOT_VISITED;
}

static ARC_PATHptr search_epsilon_loops (NETptr fst, int side_p)
{
  /* Returns an (ARC_PATHptr) buffer with all input epsilon loops
     of the FST, or NULL if there are no epsilon loops. */

  ARC_PATHptr path, loops;  STATEptr state;  id_type *in_match;

  /* prepare */

  assign_match_tables (&in_match, NULL, side_p);

  path = alloc_arc_path (FCT2_ARC_PATH_INIT_LENGTH);
  loops = alloc_arc_path (FCT2_ARC_PATH_INIT_LENGTH);

  NET_visit_marks_dirty (fst) = TRUE;
  prepare_visit_marks (fst, NOT_VISITED);

  /* traverse the FST */

  for (state = NET_states (fst);  state;  state = next_state (state))
    follow_epsilon_loops (state, path, loops, in_match);

  /* finish */

  free_arc_path (path);

  if (! loops->len)    /* no epsilon loops found */
    {
      free_arc_path (loops);
      loops = NULL;
    }

  return (loops);
}

/*-----------------------------------------------------------*/
static void initialize_epsilon_loop_vector
  (NETptr fst, ARC_PATHptr loops, LOOP_VECTORptr loop_vector,
   long *num_loops, long *num_lpstates)
{
  /* Initialize (LOOP_VECTORptr) loop_vector. If loop_vector is NULL
     then only the number of loops and of loop states is counted. */

  ARC_LOOPptr cur_loop;  STATEptr state;  ARCptr last_arc;
  long i, num;  int beg_p;

  clear_client_cells (fst);
  *num_loops = *num_lpstates = 0;

  if (loop_vector)
    loop_vector->path_buffer = loops;

  if (! loops->len)
    return;

  /* count and/or organize loops */

  for (i = 0, num = 0, beg_p = TRUE;  i < loops->len;  i ++)
    {
      if (beg_p)    /* beginning of a loop */
	{
	  if (loop_vector)
	    {
	      cur_loop = loop_vector->loops + num;

	      cur_loop->len = 0;
	      cur_loop->arcs = loops->arcs + i;
	      cur_loop->state = NULL;
	      cur_loop->next = NULL;
	    }

	  num ++;
	  beg_p = FALSE;
	}

      if (! loops->arcs [i])    /* end marker */
	{
	  beg_p = TRUE;

	  if ((! i) || (! (last_arc = loops->arcs [i-1])))
	    prog_error ("initialize_epsilon_loop_vector", "#1");

	  STATE_client_cell (ARC_destination (last_arc)) = (STATEptr) NULL + 1;
	}
    }

  *num_loops = num;

  /* count loop states */

  for (state = NET_states (fst), num = 0;  state;  state = next_state (state))
    if (STATE_client_cell (state))
      num ++;

  *num_lpstates = num;
}

static void organize_epsilon_loop_vector (NETptr fst, LOOP_VECTORptr vector)
{
  /* Make final organization of the epsilon loop vector.
     Does not attach loops to state_client_cells !!! */

  ARC_LOOPptr cur_loop;  STATE_LOOPptr cur_lpstate;
  STATEptr state;  long i, j;

  clear_client_cells (fst);

  for (i = 0;  i < vector->num_loops;  i ++)
    {
      cur_loop = vector->loops + i;

      for (j = 0;  ;  j ++)
	if (! cur_loop->arcs [j])
	  break;

      cur_loop->len = j;
      cur_loop->state = ARC_destination (cur_loop->arcs [j-1]);

      cur_loop->next = (ARC_LOOPptr) STATE_client_cell (cur_loop->state);
      STATE_client_cell (cur_loop->state) = (void *) cur_loop;
    }

  for (state = NET_states (fst), i = 0;  state;  state = next_state (state))
    if (STATE_client_cell (state))
      {
	cur_lpstate = vector->lpstates + (i++);

	cur_lpstate->state = state;
	cur_lpstate->loops = (ARC_LOOPptr) STATE_client_cell (state);
      }

  clear_client_cells (fst);
}

static void define_epsilon_diacritics (LOOP_VECTORptr vector)
{
  /* Defines a diacritic for every state with epsilon loops.
     writes the diacric into the vector. */

  int i;

  for (i = 0;  i < vector->num_lpstates;  i ++)
    vector->lpstates [i].diacritic = make_diacritic (FCT2_EPS_DIACRITIC, i);
}

/*-----------------------------------------------------------*/
static LOOP_VECTORptr build_epsilon_loop_vector (NETptr fst, int side_p)
{
  /* Returns a (LOOP_VECTORptr) vector with all epsilon loops of the FST
     or NULL if there are no epsilon loops. */

  LOOP_VECTORptr eps_vector;  ARC_PATHptr eps_loops;
  long num_loops, num_lpstates;

  if (! (eps_loops = search_epsilon_loops (fst, side_p)))
    return (NULL);

  initialize_epsilon_loop_vector
    (fst, eps_loops, NULL, &num_loops, &num_lpstates);

  eps_vector = alloc_loop_vector (num_loops, num_lpstates);

  initialize_epsilon_loop_vector
    (fst, eps_loops, eps_vector, &num_loops, &num_lpstates);

  organize_epsilon_loop_vector (fst, eps_vector);
  define_epsilon_diacritics (eps_vector);

  return (eps_vector);
}


/************************************************************
 *
 * FUNCTIONS FOR BUILDING FACTOR 2 WITHOUT INFINITE AMBIGUITY
 *
 */

static int is_destloop_in_sourceloop
  (ARC_LOOPptr src_loop, ARC_LOOPptr dst_loop, ARCptr arc,
   id_type *in_match, id_type *out_match)
{
  /* Returns TRUE if the destination loop results from rotating the
     source loop over the arc. */

  ARCptr *src_arc,  *dst_arc;  int i, last;

  if (src_loop->len != dst_loop->len)
    return (FALSE);

  src_arc = src_loop->arcs;
  dst_arc = dst_loop->arcs;
  last = src_loop->len - 1;

  if ((src_arc [0] != arc) || (dst_arc [last] != arc))
    return (FALSE);

  for (i = 0;  i < last;  i ++)
    if (dst_arc [i] != src_arc [i+1])
      return (FALSE);

  return (TRUE);
}

static int decide_arc_redirection
  (STATE_LOOPptr source, STATE_LOOPptr dest, ARCptr arc,
   id_type *in_match, id_type *out_match)
{
  /* Returns TRUE if the arc must be redirected.
     Arcs must not be redirected if they have EPSILON on the input side
     and if all their destination state loops also occur on their source
     states in rotated form. */

  ARC_LOOPptr src_loop, dst_loop;

  if (! dest)
    return (FALSE);  /* do not redirect */

  if ((! source) || (in_match [ARC_label (arc)] != EPSILON))
    return (TRUE);    /* redirect */

  for (dst_loop = dest->loops;  dst_loop;  dst_loop = dst_loop->next)
    {
      for (src_loop = source->loops;  src_loop;  src_loop = src_loop->next)
	if (is_destloop_in_sourceloop
	    (src_loop, dst_loop, arc, in_match, out_match))
	  break;    /* equivalent source loop found */

      if (! src_loop)    /* no equivalent source loop found */
	return (TRUE);    /* redirect */
    }

  /* every dest loop has an equivalent source loop --> do not redirect */
  return (FALSE);
}

static void redirect_arcs
  (NETptr fa1, LOOP_VECTORptr eps_vector, int side_p)
{
  /* Creates an auxiliary state for p[i] every state q[i] with epsilon
     loops, and redirects all incomming arcs of p[i] to q[i] if they
     meet certain conditions. Connects every p[i] with q[i] by an arc
     labeled with EPSILON:DIACRITIC. */

  STATE_LOOPptr lpstate, source, dest;  STATEptr state;  ARCptr arc;
  id_type pair_id, *in_match, *out_match;  int i;

  assign_match_tables (&in_match, &out_match, side_p);
  clear_client_cells (fa1);

  /* make auxiliary states (without arcs) */

  for (i = 0;  i < eps_vector->num_lpstates;  i ++)
    {
      lpstate = eps_vector->lpstates + i;

      lpstate->auxstate = make_state (NULL, fa1);
      STATE_client_cell (lpstate->state) = (void *) lpstate;
    }

  /* redirect incomming arcs */

  for (state = NET_states (fa1);  state;  state = next_state (state))
    {
      source = (STATE_LOOPptr) STATE_client_cell (state);

      for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
	if ((dest = (STATE_LOOPptr) STATE_client_cell (ARC_destination (arc))))
	  if (decide_arc_redirection (source, dest, arc, in_match, out_match))
	    ARC_destination (arc) = dest->auxstate;
    }

  /* make auxiliary arcs */

  for (i = 0;  i < eps_vector->num_lpstates;  i ++)
    {
      lpstate = eps_vector->lpstates + i;
      pair_id = make_dir_pair_label (EPSILON, lpstate->diacritic, side_p);
      arc = make_arc (pair_id, lpstate->state, fa1);
      push_arc (arc, lpstate->auxstate);
    }
}

/*-----------------------------------------------------------*/
static ALPHABETptr overwrite_loops_with_diacritics
  (LOOP_VECTORptr eps_vector, int side_p)
{
  /* Overwrites every input EPSILON in a loop with a diacritic.
     We cannot use the match tables here because we permanently
     create new IDs.
     Returns an alphabet (type BINARY_VECTOR) with all diacritics. */

  ARC_LOOPptr loop;  ARCptr arc;  id_type in_id, out_id;  ALPHABETptr diacr;
  int i, j, index = 0;

  diacr = make_alph (LIM_id_count (LABEL_MAP), BINARY_VECTOR);

  for (i = 0;  i < eps_vector->num_lpstates;  i ++)
    for (loop = eps_vector->lpstates [i].loops;  loop;  loop = loop->next)
      for (j = 0;  j < loop->len;  j ++)
	{
	  arc = loop->arcs [j];
	  split_dir_pair_label (ARC_label (arc), &in_id, &out_id, side_p);

	  if (in_id == EPSILON)    /* not yet overwritten */
	    {
	      in_id = make_diacritic (FCT2_TMPEPS_DIACRITIC, index ++);
	      sigma_add_to (diacr, in_id);
	      ARC_label (arc) = make_dir_pair_label (in_id, out_id, side_p);
	    }
	}

  return (diacr);
}

static NETptr make_loop_constraints (LOOP_VECTORptr eps_vector, int side_p)
{
  /* Returns a network that encodes the loop constraints:
     No full looping paths (overwritten with diacritics)
     are permitted. */

  ARC_LOOPptr loop;  NETptr net, path, constr;  ARCptr arc;
  id_type *in_match;  int i, j;

  assign_match_tables (&in_match, NULL, side_p);
  constr = null_fsm ();

  for (i = 0;  i < eps_vector->num_lpstates;  i ++)
    for (loop = eps_vector->lpstates [i].loops;  loop;  loop = loop->next)
      {
	path = epsilon_fsm ();

	for (j = 0;  j < loop->len;  j ++)
	  {
	    arc = loop->arcs [j];
	    net = symbol_fsm (in_match [ARC_label (arc)]);
	    path = concat (path, net);
	  }

	constr = net_union (constr, path);
      }

  constr = negate (contains (constr));
  constr = force_minimize (constr, DONT_COPY, TRUE);

  return (constr);
}

static void overwrite_diacritics_with_epsilon
  (NETptr fa1, ALPHABETptr diacr, int side_p)
{
  STATEptr state;  ARCptr arc;  id_type *in_match, *out_match;

  assign_match_tables (&in_match, &out_match, side_p);

  for (state = NET_states (fa1);  state;  state = next_state (state))
    for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
      if (sigma_member (diacr, in_match [ARC_label (arc)]))
	ARC_label (arc) =
	  make_dir_pair_label (EPSILON, out_match [ARC_label (arc)], side_p);
}

/*-----------------------------------------------------------*/
static NETptr build_infamb_factor1
  (NETptr fst, LOOP_VECTORptr eps_vector, int side_p, int compact_p)
{
  /* Builds factor 1 where every set of epsilon loops is replaced
     by a single arc labeled with EPSILON:DIACRITIC.
     The argument "fst" is consumed, it becomes factor 1.
     All other arguments are neither consumed nor modified.
     Note: factor 2 must be build before factor 1 !!!  */

  NETptr constr;  ALPHABETptr diacr;

  redirect_arcs (fst, eps_vector, side_p);
  diacr = overwrite_loops_with_diacritics (eps_vector, side_p);
  update_net_labels_and_sigma (fst);

  constr = make_loop_constraints (eps_vector, side_p);

  if (side_p == UPPER)
    fst = compose (constr, fst);
  else
    fst = compose (fst, constr);

  overwrite_diacritics_with_epsilon (fst, diacr, side_p);

  /* finish */

  free_alph (diacr);
  fst = replace_label (fst, BOUNDARY_SYMBOL, EPSILON, DO_MINIMIZE, compact_p);

  return (fst);
}

/************************************************************
 *
 * FUNCTIONS FOR BUILDING FACTOR 2 WITH INFINITE AMBIGUITY
 *
 */

static NETptr attach_twin_net (NETptr fst)
{
  /* Copies the original FST and attaches every original state
     to its twin state. */

  NETptr twin_fst;  STATEptr state, twin_state, twin_dest;
  ARCptr arc, twin_arc;

  twin_fst = make_network ();
  NET_client_cell (fst) = (void *) twin_fst;

  twin_fst->flags = fst->flags;
  NET_arc_label_arity (twin_fst) = NET_arc_label_arity (fst);
  
  NET_sigma (twin_fst) = copy_alphabet (NET_sigma (fst));
  NET_labels (twin_fst) = copy_alphabet (NET_labels (fst));

  /* copy states */

  clear_client_cells (fst);

  for (state = NET_states (fst);  state;  state = next_state (state))
    {
      twin_state = make_state (NULL, twin_fst);
      STATE_final (twin_state) = STATE_final (state);
      STATE_deterministic (twin_state) = STATE_deterministic (state);

      STATE_client_cell (state) = (void *) twin_state;
    }

  NET_start_state (twin_fst) =
    (STATEptr) STATE_client_cell (NET_start_state (fst));

  /* copy arcs */

  for (state = NET_states (fst);  state;  state = next_state (state))
    {
      twin_state = (STATEptr) STATE_client_cell (state);

      for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
	{
	  twin_dest = (STATEptr) STATE_client_cell (ARC_destination (arc));

	  twin_arc = make_arc (ARC_label (arc), twin_dest, twin_fst);
	  push_arc (twin_arc, twin_state);
	}
    }

  return (twin_fst);
}

/*-----------------------------------------------------------*/
static void insert_diacritics_into_infamb_factor2
  (NETptr fa2, LOOP_VECTORptr vector, int side_p)
{
  STATEptr state, init_state, fin_state;  ARCptr arc;
  id_type diacr, pair_id;  int i;

  /* make a new final state */

  init_state = NET_start_state (fa2);
  fin_state = make_state (NULL, fa2);
  STATE_final (fin_state) = TRUE;

  /* insert diacritic arcs */

  for (i = 0;  i < vector->num_lpstates;  i ++)
    {
      diacr = vector->lpstates [i].diacritic;
      state = (STATEptr) STATE_client_cell (vector->lpstates [i].state);

      pair_id = make_dir_pair_label (diacr, EPSILON, side_p);

      arc = make_arc (pair_id, state, fa2);
      push_arc (arc, init_state);

      arc = make_arc (pair_id, fin_state, fa2);
      push_arc (arc, state);
    }

  update_net_labels_and_sigma (fa2);
}

static NETptr make_infamb_constrains (LOOP_VECTORptr eps_vector, int side_p)
{
  /* Returns a net that constrains diacritics in factor 2.
     [ D1 0:D1 | D2 0:D2 | .... | Dn 0:Dn ] for side_p==UPPER  */

  NVptr constr;  NVtype nv;  NETptr nets [2], result;
  id_type diacr, pair_id;  int i;

  nv.len = 2;
  nv.nets = nets;
  constr = make_nv (eps_vector->num_lpstates);

  for (i = 0;  i < eps_vector->num_lpstates;  i ++)
    {
      diacr = eps_vector->lpstates [i].diacritic;
      pair_id = make_dir_pair_label (EPSILON, diacr, side_p);

      nets [0] = symbol_fsm (diacr);
      nets [1] = symbol_fsm (pair_id);
      NV_net (constr, i) = concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE);
    }

  result = union_fsm (constr, DONT_COPY, DO_MINIMIZE);
  free_nv_only (constr);

  return (result);
}

#if NOT_USED
static NETptr other_than_infamb_diacritics (LOOP_VECTORptr eps_vector)
{
  /* Returns a net that accepts a sinle symbol other than a diacritic.
     \[ D1 | D2 | .... ]  */

  NETptr net;  int i;  id_type diacr;

  net = symbol_fsm (OTHER);

  for (i = 0;  i < eps_vector->num_lpstates;  i ++)
    {
      diacr = eps_vector->lpstates [i].diacritic;
      sigma_add_to (NET_sigma (net), diacr);
    }

  return (net);
}
#endif /* NOT_USED */

static ALPHABETptr diacritics_to_alphabet (LOOP_VECTORptr eps_vector)
{
  ALPHABETptr alph;  int i;  id_type diacr;

  alph = make_alph (LIM_id_count (LABEL_MAP), BINARY_VECTOR);

  for (i = 0;  i < eps_vector->num_lpstates;  i ++)
    {
      diacr = eps_vector->lpstates [i].diacritic;
      sigma_add_to (alph, diacr);
    }

  binary_to_label (alph);
  return (alph);
}

/*-----------------------------------------------------------*/
static NETptr prepare_infamb_factor2
  (NETptr fst, LOOP_VECTORptr eps_vector, int side_p)
{
  /* Builds part of factor 2 that maps a string consisting
     of one single diacritic to a set of epsilon loops.
     Arguments are neither consumed nor modified.
     Note: factor 2 must be prepared before factor 1 is build !!! */

  NETptr twin, constr, dia2loops;

  /*  DIA_2_LOOPS = [ D1 0:D1 | D2 0:D2 | ....] .o. TWIN  */

  twin = attach_twin_net (fst);
  insert_diacritics_into_infamb_factor2 (twin, eps_vector, side_p);
  constr = make_infamb_constrains (eps_vector, side_p);

  if (side_p == UPPER)
    dia2loops = compose (constr, twin);
  else
    dia2loops = compose (twin, constr);

  sigma_remove_from (NET_sigma (dia2loops), BOUNDARY_SYMBOL);
  dia2loops = force_minimize (dia2loops, DONT_COPY, FALSE);

  return (dia2loops);
}


/*-----------------------------------------------------------*/
static NETptr build_infamb_factor2
  (NETptr fa1, NETptr dia2loops, LOOP_VECTORptr eps_vector, int side_p,
   int compact_p)
{
  /* Builds factor 2 that maps a diacritic to a set of epsilon loops,
     and a non-diacritic to itself.
     Arguments are neither consumed nor modified.
     Note: factor 2 must be built after factor 1 !!! */

  NETptr fa2, out1, dia;  ALPHABETptr alph;

  /* make union of output symbols of factor 1 */

  alph = side_sigma (fa1, opposite_side (side_p));
  out1 = alphabet_to_net (alph);
  free_alph (alph);

  /* make union of diacritics of factor 2 */

  alph = diacritics_to_alphabet (eps_vector);
  dia = alphabet_to_net (alph);
  free_alph (alph);

  /*  FA2 = [ OUT1 .o. [ DIA_2_LOOPS | ? - DIA ] ]*  */

  fa2 = net_union (copy_fsm (dia2loops), minus (symbol_fsm (OTHER), dia));

  if (side_p == UPPER)
    fa2 = compose (out1, fa2);
  else
    fa2 = compose (fa2, out1);

  fa2 = kleene_star (fa2);
  fa2 = force_minimize (fa2, DONT_COPY, compact_p);

  return (fa2);
}


/************************************************************
 *
 * PRINCIPLE FACTORIZATION FUNCTIONS (FOR INFINITELY AMBIGUOUS FSTs)
 *
 */

NVptr factorize_infamb_fst
  (NETptr fst, int side_p, int copy_p, int compact_p, int alphreduce_p,
   int verbose_p)
{
  /* Extracts infinite ambiguity (input epsilon loops) from an FST.
     CASE 1: epsilon loops exist --> returns two FST:
       factor 1 without eps loops, factor 2 with only eps loops.
     CASE 2: no epsilon loops --> returns only the original (minimized) FST.
     */

  LOOP_VECTORptr eps_vector;  NVptr factors;  NETptr fa1, fa2, dia2loops;

  /* prepare */

  fst = min_fsm (fst, copy_p);
  factors = make_nv (2);

  print_comment
    (verbose_p, "Input transducer:\n  %s\n", temp_net_info (fst, side_p));

  fst = attach_boundaries (fst, BOUNDARY_SYMBOL, BOUNDARY_SYMBOL);

  /* search for epsilon loops */

  print_comment (verbose_p, "Searching for epsilon loops ... ");
  eps_vector = build_epsilon_loop_vector (fst, side_p);

  if (eps_vector)
    print_comment (verbose_p, "%li found\n", eps_vector->num_loops);
  else
    {
      print_comment (verbose_p, "nothing found\n");
      fst = replace_label (fst, BOUNDARY_SYMBOL, EPSILON, DO_MINIMIZE, compact_p);

      NV_net (factors, 0) = fst;
      NV_net (factors, 1) = sigma_star_fsm ();

      return (factors);
    }

  /* factorize */

  print_comment (verbose_p, "Building factors:\n");

  dia2loops = prepare_infamb_factor2 (fst, eps_vector, side_p);
  fa1 = build_infamb_factor1 (fst, eps_vector, side_p, compact_p);
  fa2 = build_infamb_factor2 (fa1, dia2loops, eps_vector, side_p, compact_p);

  NV_net (factors, 0) = fa1;
  NV_net (factors, 1) = fa2;

  print_comment (verbose_p, "  %s\n", temp_net_info (fa1, side_p));
  print_comment (verbose_p, "  %s\n", temp_net_info (fa2, side_p));

  /* reduced the intemediate alphabet */

  if (alphreduce_p)
    {
      print_comment (verbose_p, "Reducing intermediate alphabet ... ");

      factors = reduce_intermediate_alphabet_in_pair
	(factors, NULL, side_p, DONT_COPY, TRUE, FALSE);

      print_comment (verbose_p, "done:\n");

      print_comment (verbose_p, "  %s\n", temp_net_info (NV_net (factors, 0), side_p));
      print_comment (verbose_p, "  %s\n", temp_net_info (NV_net (factors, 1), side_p));
    }

  /* finish */

  free_loop_vector (eps_vector);
  free_network (dia2loops);

  return (factors);
}


/************************************************************
 *
 * FACTORIZATION OF FINITELY AMBIGUOUS FSTs
 *   BY MEANS OF S-COVERING
 *   (Schutzenberger, Sakarovitch)
 *
 */

static int fSc_correct_1st_other (STATEptr state, id_type other2, int side_p)
{
  ARCptr arc;

  for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
    if (ARC_label (arc) == OTHER)
      {
	ARC_label (arc) = make_dir_pair_label (OTHER, other2, side_p);
	fSc_split_arc (arc) = TRUE;

	return (TRUE);
      }

  return (FALSE);
}

static void fSc_set_out_symbol_indices
  (NETptr net, ALPHABETptr alph, int side_p, int final_state_p)
{
  STATEptr state;  ARCptr arc;
  id_type insym, outsym, other2;  uint16 index;

  if (FST_intern_literal (&other2, fSc_INDIR_FACTO) != NO_ERROR)
    handle_error ("Too many labels", "fSc_set_out_symbol_indices", 1);

  if (final_state_p)    /* clean alphabet only once */
    memset (ALPH_items (alph), 0, ALPH_len (alph) * sizeof (id_type));

  for (state = NET_states (net);  state;  state = next_state (state))
    if (((final_state_p) && (STATE_final (state)))     ||
	((! final_state_p) && (! STATE_final (state)))  )
      {
	if (! final_state_p)    /* clean alphabet with every state */
	  memset (ALPH_items (alph), 0, ALPH_len (alph) * sizeof (id_type));
      
	for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
	  {
	    insym = ARC_label (arc);
	    index = ALPH_item (alph, insym) ++;

	    if (index)    /* more than 1 arc with same label --> destinguish */
	      {
		if (insym == OTHER)    /* indirect factorization required */
		  {
		    outsym = other2;
		    fSc_split_arc (arc) = TRUE;
		  }
		else    /* direct factorization */
		  {
		    outsym = insym;
		    fSc_split_arc (arc) = FALSE;
		  }

		outsym = make_intermediate_bm_label (outsym, index);
		ARC_label (arc) = make_dir_pair_label (insym, outsym, side_p);
	      }
	    else
	      fSc_split_arc (arc) = FALSE;
	  }

	if ((! final_state_p) && (ALPH_item (alph, OTHER) > 1))
	  fSc_correct_1st_other (state, other2, side_p);
      }

  if ((final_state_p) && (ALPH_item (alph, OTHER) > 1))
    for (state = NET_states (net);  state;  state = next_state (state))
      if (STATE_final (state))
	if (fSc_correct_1st_other (state, other2, side_p))
	  break;
}

static void fSc_make_T1_labels (NETptr net, int side_p)
{
  ALPHABETptr alph;
  void **client_cells;

  alph = make_alph (LIM_id_count (LABEL_MAP), BINARY_VECTOR);

  client_cells = save_state_client_cells (net);
  turn_all_arcs (net);

  /* make output symbols with indices */

  fSc_set_out_symbol_indices (net, alph, side_p, TRUE);
  fSc_set_out_symbol_indices (net, alph, side_p, FALSE);

  /* finish */

  turn_all_arcs (net);
  restore_state_client_cells (client_cells, net);
  free (client_cells);

  free_alph (alph);
}

/*-----------------------------------------------------------*/
static ARCptr fSc_get_T1_arc (ARCptr T_arc, STATEptr T1_source, id_type *in_match)
{
  ARCptr T1_arc;  id_type T_insym;

  T_insym = in_match [ARC_label (T_arc)];

  for (T1_arc = STATE_arc_set (T1_source);  T1_arc;  T1_arc = next_arc (T1_arc))
    if (in_match [ARC_label (T1_arc)] == T_insym)
      return (T1_arc);

  prog_error ("fSc_get_T1_arc", "#1");
  return (NULL);
}

static STATEptr fSc_get_T2_dest (STATEptr T_dest, STATEptr T1_dest)
{
  PRSTLISTptr pair;

  for (pair = (PRSTLISTptr) STATE_client_cell (T1_dest);
       pair;
       pair = pair->next)
    if (pair->state1 == T_dest)
      return (pair->state2);

  prog_error ("fSc_get_T2_dest", "#1");
  return (NULL);
}

static id_type fSc_make_T2_label
  (ARCptr T_arc, ARCptr T1_arc, id_type *out_match, id_type other2, int side_p)
{
  id_type T2_sym, T2_insym, T2_outsym;

  if (ARC_label (T_arc) == OTHER)  /* identity OTHER */
    {
      if (fSc_split_arc (T1_arc))    /* identity OTHER to be split */
	{
	  T2_insym = out_match [ARC_label (T1_arc)];
	  T2_outsym = other2;
	}
      else        /* identity OTHER to be not split */
	{
	  T2_insym = OTHER;
	  T2_outsym = ID_NO_SYMBOL;
	}
    }
  else        /* non-identity OTHER or ordinary symbol */
    {
      T2_insym = out_match [ARC_label (T1_arc)];
      T2_outsym = out_match [ARC_label (T_arc)];
    }

  if (T2_outsym == ID_NO_SYMBOL)
    T2_sym = T2_insym;
  else
    T2_sym = make_dir_pair_label (T2_insym, T2_outsym, side_p);

  return (T2_sym);
}

static void fSc_make_T2_arcs
  (NETptr T, NETptr T1, NETptr T2, id_type *in_match, id_type *out_match,
   int side_p)
{
  /* T...original FST, T1...FST from DetInputAutomaton, T2...S-covering of T */

  PRSTLISTptr pair;
  STATEptr T_source, T1_source, T2_source, T2_dest;
  ARCptr T_arc, T1_arc, T2_arc;  id_type T2_label, other2;

  if (FST_intern_literal (&other2, fSc_INDIR_FACTO) != NO_ERROR)
    handle_error ("Too many labels", "fSc_set_out_symbol_indices", 1);

  for (T1_source = NET_states (T1);
       T1_source;
       T1_source = next_state (T1_source))
    for (pair = (PRSTLISTptr) STATE_client_cell (T1_source);
	 pair;
	 pair = pair->next)
      {
	T_source = pair->state1;
	T2_source = pair->state2;

	for (T_arc = STATE_arc_set (T_source);  T_arc;  T_arc = next_arc (T_arc))
	  {
	    T1_arc = fSc_get_T1_arc (T_arc, T1_source, in_match);

	    T2_label = fSc_make_T2_label (T_arc, T1_arc, out_match, other2, side_p);
	    T2_dest = fSc_get_T2_dest
	      (ARC_destination (T_arc), ARC_destination (T1_arc));

	    T2_arc = insert_sort_arc (T2_source, T2_label, T2_dest, T2);
	    fSc_split_arc (T2_arc) = fSc_split_arc (T1_arc);
	  }
      }
}

/*-----------------------------------------------------------*/
static void fSc_arc_splitting
  (NETptr fst, int T1_p, id_type *in_match, id_type *out_match, int side_p)
{
  STATEptr state, state2;  ARCptr arc, arc2;
  id_type orig_sym, orig_insym, orig_outsym, dia_sym, dia_insym, dia_outsym,
    other2, other_other;

  if (FST_intern_literal (&other2, fSc_INDIR_FACTO) != NO_ERROR)
    handle_error ("Too many labels", "fSc_set_out_symbol_indices", 1);

  if (make_pair_label (&other_other, OTHER, OTHER) != NO_ERROR)
    handle_error ("Too many labels", "fSc_set_out_symbol_indices", 1);

  for (state = NET_states (fst);  state;  state = next_state (state))
    for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
      if (fSc_split_arc (arc))
	{
	  /* make new labels */

	  if (T1_p)
	    {
	      orig_insym = OTHER;
	      orig_outsym = ID_NO_SYMBOL;
	      dia_insym = DET_EPSILON;
	      dia_outsym = out_match [ARC_label (arc)];

	      if (in_match [ARC_label (arc)] != OTHER)
		prog_error ("fSc_arc_splitting", "#1");
	    }
	  else  /* T2 */
	    {
	      orig_insym = OTHER;
	      orig_outsym = out_match [ARC_label (arc)];
	      dia_insym = in_match [ARC_label (arc)];
	      dia_outsym = EPSILON;

	      if (orig_outsym == other2)    /* identity OTHER match */
		orig_outsym = ID_NO_SYMBOL;
	    }

	  if (orig_outsym == ID_NO_SYMBOL)
	    orig_sym = orig_insym;
	  else if (orig_outsym == OTHER)  /* non-identity OTHER match */
	    orig_sym = other_other;
	  else
	    orig_sym = make_dir_pair_label (orig_insym, orig_outsym, side_p);

	  dia_sym = make_dir_pair_label (dia_insym, dia_outsym, side_p);

	  /* split arc */

	  arc2 = make_arc (dia_sym, ARC_destination (arc), fst);
	  state2 = make_state (arc2, fst);

	  ARC_label (arc) = orig_sym;
	  ARC_destination (arc) = state2;
	}
}

static void fSc_remake_alphabets (NETptr T, NETptr Tn)
{
  ALPHABETptr add_symbols;

  if (sigma_member (NET_sigma (T), OTHER))
    add_symbols = NET_sigma (T);
  else
    add_symbols = NULL;

  derive_alphabets (Tn, add_symbols);
}

/*-----------------------------------------------------------*/
static NVptr fSc_factorize
  (NETptr fst, int side_p, int compact_p, int verbose_p)
{
  HEAPptr state_heap, pair_heap;
  NVptr factors;  NETptr T1, T2;  id_type *in_match, *out_match;

  state_heap = init_heap
    (sizeof (STATELISTtype), STATE_LIST_BLOCK_SIZE, "State list cells");

  pair_heap = init_heap
    (sizeof (PRSTLISTtype), PAIR_STATE_LIST_BLOCK_SIZE, "Pair state cells");

  /* make T1 from the Determinized of the Input Automaton of the FST */

  assign_match_tables (&in_match, NULL, side_p);
  T1 = make_determ_input_net (fst, state_heap, in_match, side_p, FALSE);

  if (! T1)
    prog_error ("facto_with_S_covering", "#1");

  fSc_make_T1_labels (T1, side_p);

  /* make T2 from the S-covering of the FST */

  T2 = make_state_pairs (T1, state_heap, pair_heap);
  free_heap (state_heap);

  assign_match_tables (&in_match, &out_match, side_p);
  fSc_make_T2_arcs (fst, T1, T2, in_match, out_match, side_p);
  free_heap (pair_heap);

  /* split arcs */

  assign_match_tables (&in_match, &out_match, side_p);
  fSc_arc_splitting (T1, TRUE, in_match, out_match, side_p);
  fSc_arc_splitting (T2, FALSE, in_match, out_match, side_p);

  /* make new alphabets */

  fSc_remake_alphabets (fst, T1);
  fSc_remake_alphabets (fst, T2);

  /* finish */

  T2 = reverse_fsm (T2, DONT_COPY, DONT_MINIMIZE);
  T1 = force_minimize (T1, DONT_COPY, compact_p);
  T2 = force_minimize (T2, DONT_COPY, compact_p);

  factors = make_nv (2);
  NV_net (factors, 0) = T1;
  NV_net (factors, 1) = T2;

  return (factors);
}

NVptr factorize_with_S_covering
  (NETptr fst, int side_p, int copy_p, int compact_p, int alphreduce_p,
   int verbose_p)
{
  /* Factorizes a finitely ambiguous FST using its S-covering.
     Returns a net vector with the two factors. */

  NVptr factors;

  /* prepare */

  fst = min_fsm (fst, copy_p);

  print_comment
    (verbose_p, "Input transducer:\n  %s\n", temp_net_info (fst, side_p));

  /* align ambiguity */

  if (contains_symbol_on_side (fst, EPSILON, side_p))  /* input EPSILON */
    {
      print_comment (verbose_p, "Aligning ambiguity ... ");

      if (contains_forbidden_symbol (NET_sigma (fst), DET_EPSILON))
	{
	  error_buffer
	    ("put", "FST contains both EPSILON and DET_EPSILON on the input side");
	  return (NULL);
	}

      if (! (fst = align_ambiguity (fst, side_p, DONT_COPY, compact_p)))
	{                /* original FST infinitely ambiguous */
	  error_buffer ("put", "FST contains infinite ambiguity");
	  return (NULL);
	}

      print_comment (verbose_p, "done:\n  %s\n", temp_net_info (fst, side_p));
    }
  else if (contains_symbol_on_side (fst, EPSILON, opposite_side (side_p)))
    fst = replace_oneside_symbol
      (fst, EPSILON, DET_EPSILON, opposite_side (side_p), DONT_MINIMIZE, FALSE);

  /*  */

  print_comment (verbose_p, "Factorizing ... ");
  factors = fSc_factorize (fst, side_p, compact_p, verbose_p);
  free_network (fst);

  print_comment (verbose_p, "done:\n");
  print_comment (verbose_p, "  %s\n", temp_net_info (NV_net (factors, 0), side_p));
  print_comment (verbose_p, "  %s\n", temp_net_info (NV_net (factors, 1), side_p));

  /* reduced the intemediate alphabet */

  if (alphreduce_p)
    {
      print_comment (verbose_p, "Reducing intermediate alphabet ... ");

      factors = reduce_intermediate_alphabet_in_pair
	(factors, NULL, side_p, DONT_COPY, TRUE, FALSE);

      print_comment (verbose_p, "done:\n");
      print_comment (verbose_p, "  %s\n", temp_net_info (NV_net (factors, 0), side_p));
      print_comment (verbose_p, "  %s\n", temp_net_info (NV_net (factors, 1), side_p));
    }

  /* finish */

  fst = NV_net (factors, 1);

  if (contains_symbol_on_side (fst, DET_EPSILON, opposite_side (side_p)))
    NV_net (factors, 1) = replace_oneside_symbol
      (fst, DET_EPSILON, EPSILON, opposite_side (side_p), DONT_MINIMIZE, FALSE);

  return (factors);
}


/************************************************************
 *
 * COMPLETE (UN-)FACTORIZATION OF ARBITRARY FSTSs
 *
 */

NVptr factorize_arbitrary_fst
  (NETptr fst, int side_p, int copy_p, int compact_p, int alphreduce_p,
   int verbose_p)
{
  /* Factorizes an arbitrary FST into four FSTs.
     Equivalent to the successive application of three factorizations:
     extraction of infinite ambiguity, extraction of finite ambiguity,
     and bimachine factorization.
     The resulting FSTs contain the infinitely ambiguous part,
     the finitely ambiguous part, the left-sequential part, and
     the right-sequential part of the original FST. */

  NVptr nv;
  NETptr infamb, shortamb, finamb, unamb, rightseq, leftseq;

  /* extract infinite ambiguity */

  print_comment (verbose_p, "----  EXTRACTION OF INFINITE AMBIGUITY  ----\n");
  nv = factorize_infamb_fst (fst, side_p, copy_p, compact_p, alphreduce_p, verbose_p);

  finamb = NV_net (nv, 0);
  infamb = NV_net (nv, 1);
  free_nv_only (nv);

  /* extract short ambiguity */

  print_comment (verbose_p, "----  EXTRACTION OF SHORT AMBIGUITY  ----\n");
  nv = factorize_shortamb_fst (finamb, side_p, DONT_COPY, compact_p, verbose_p);

  finamb = NV_net (nv, 0);
  shortamb = NV_net (nv, 1);
  free_nv_only (nv);

  /* extract finite ambiguity */

  print_comment (verbose_p, "----  EXTRACTION OF FINITE AMBIGUITY  ----\n");
  nv = factorize_finamb_fst (finamb, side_p, DONT_COPY, compact_p, alphreduce_p,
			     verbose_p);
  unamb = NV_net (nv, 0);
  finamb = NV_net (nv, 1);
  free_nv_only (nv);

  if (side_p == UPPER)
    finamb = compose (finamb, shortamb);
  else
    finamb = compose (shortamb, finamb);

  /* bimachine factorization */

  print_comment (verbose_p, "----  BIMACHINE FACTORIZATION  ----\n");
  nv = factorize_bm_fsts (unamb, side_p, DONT_COPY, compact_p, alphreduce_p,
			  verbose_p);
  leftseq = NV_net (nv, 0);
  rightseq = NV_net (nv, 1);
  free_nv_only (nv);

  /* finish */

  nv = make_nv (4);
  NV_net (nv, 0) = leftseq;
  NV_net (nv, 1) = rightseq;
  NV_net (nv, 2) = finamb;
  NV_net (nv, 3) = infamb;

  return (nv);
}

NETptr unfactorize_arbitrary_fst
  (NVptr nv, int side_p, int copy_p, int compact_p, int verbose_p)
{
  NETptr infamb, finamb, unamb, rightseq, leftseq;

  if (! check_number_of_nets (nv, 4))
    return (NULL);

  /* prepare */

  if (copy_p == DO_COPY)
    nv = copy_nv_and_nets (nv);

  leftseq = NV_net (nv, 0);
  rightseq = NV_net (nv, 1);
  finamb = NV_net (nv, 2);
  infamb = NV_net (nv, 3);

  free_nv_only (nv);

  /* unfactorize */

  print_comment (verbose_p, "Input transducers:\n");
  print_comment (verbose_p, "  Lseq:  %s\n", temp_net_info (leftseq, side_p));
  print_comment (verbose_p, "  Rseq:  %s\n", temp_net_info (rightseq, side_p));
  print_comment (verbose_p, "  Famb:  %s\n", temp_net_info (finamb, side_p));
  print_comment (verbose_p, "  Iamb:  %s\n", temp_net_info (infamb, side_p));

  unamb = unfactorize_bm_fsts (leftseq, rightseq, side_p, DONT_COPY, FALSE);
  finamb = unfactorize_amb_fsts (unamb, finamb, side_p, DONT_COPY, FALSE);
  infamb = unfactorize_amb_fsts (finamb, infamb, side_p, DONT_COPY, FALSE);

  /* finish */

  print_comment (verbose_p, "Output transducers:\n");
  print_comment (verbose_p, "  %s\n", temp_net_info (infamb, side_p));

  return (infamb);
}

/************************************************************
 *
 * END
 *
 */


#ifdef DEBUG

#include "fst/prolog.h"
#include "fst/debug.h"

void TEST_print_prolog (NETptr net, char *filename, int sort_states_and_arcs)
{
  FILE *stream;

  if (! (stream = fopen (filename, "w")))
    {
      fprintf (stderr, "File %s cannot be opened\n", filename);
      return;
    }

  network_to_prolog_file (net, stream, sort_states_and_arcs);
  fclose (stream);
}

static void TEST_get_label_name(LABELptr label, char *lab_name)
{
  FAT_STR fstring = LABEL_atom(label);
     
  FST_fat_to_thin (fstring, lab_name);
}

static int TEST_get_label (id_type id, char *lab_name)
{
  TUPLEptr tuple;  LABELptr label;
  id_type upper_id, lower_id;
  int16 arity, id_count = LIM_id_count(LABEL_MAP);
  char *pt;

  if (id>=id_count)
    { strcpy(lab_name, "WRONG_ID"); return(FALSE);}

  label = id_to_label(id);
  arity = LABEL_arity(label);

  switch(arity)
    {
    case 1:
      TEST_get_label_name(label, lab_name);
      break;
    case 2:
      tuple = LABEL_tuple(label);
      upper_id = TUPLE_labels(tuple)[UPPER];
      lower_id = TUPLE_labels(tuple)[LOWER];

      TEST_get_label(upper_id, lab_name);

      pt = &( lab_name[ strlen(lab_name) ]); *pt++=':';
      TEST_get_label(lower_id, pt);
      break;
    default:
      strcpy(lab_name, "WRONG_ARITY");
      return(FALSE);
    }

  return(TRUE);
}

void TEST_print_state_client_cells (NETptr net)
{
  void **vec;  long i, len;

  vec = save_state_client_cells (net);
  len = NET_num_states (net);
  fprintf (stderr, "Length %li: ", len);

  for (i=0;  i< len;  i++)
    fprintf (stderr, " %#lx", vec [i]);

  fprintf (stderr, "\n");
  fflush (stderr);
  free (vec);
}

void TEST_print_arcinfo (FCT2_ARCptr arcinfo)
{
  char label [200];

  if (! TEST_get_label (arcinfo->pair_id, label))
    strcpy (label, "-:-");

  fprintf (stderr, "%s\tS: %li %#lx\tD: %li %#lx\tHD: %#lx\n",
	   label, arcinfo->source_num, arcinfo->source,
	   arcinfo->dest_num, arcinfo->dest, arcinfo->hyper_dest);

  fflush (stderr);
}

void TEST_print_partition (FCT2_ARCVECptr arcvec)
{
  int i;

  for (i=0;  i< arcvec->sub.len;  i++)
    {
      fprintf (stderr, "[%d]  ", i);
      TEST_print_arcinfo (arcvec->sub.vector + i);
    }
}

void TEST_print_arcpath (ARC_PATHptr path)
{
  ARCptr *arcPt;  char label [200];  int i;

  for (i = 0;  i < path->len;  i ++)
    {
      arcPt = path->arcs + i;

      if (*arcPt)
	TEST_get_label (ARC_label (*arcPt), label);
      else
	strcpy (label, "<NULL>");

      fprintf (stderr, "  %s", label);
    }
  fprintf (stderr, "\n");
  fflush (stderr);
}

void TEST_print_loopstate (STATE_LOOPptr lpstate)
{
  ARC_LOOPptr loops;    ARCptr *arc;  char label [200];

  fprintf (stderr, "State  %#lx\n", lpstate->state);

  for (loops = lpstate->loops;  loops;  loops = loops->next)
    {
      fprintf (stderr, "  Loop [%li]", loops->len);

      for (arc = loops->arcs;  *arc;  arc ++)
	{
	  TEST_get_label (ARC_label (*arc), label);
	  fprintf (stderr, "  %s", label);
	}
      fprintf (stderr, "\n");
    }
}

void TEST_print_transduction (ALPHTRANSptr tr)
{
  char label [200];  int i;

  fprintf (stderr, "Length %d\n", tr->len);

  for (i = 0;  i < tr->len;  i ++)
    {
      if (! TEST_get_label (tr->symbols [i].in_symbol, label))
	strcpy (label, "--");

      fprintf (stderr, "%d:\t[%s]\t", i, label);
      TEST_print_alph (tr->symbols [i].out_symbols);
    }
}

void TEST_print_recoding (ALPHABETptr recode)
{
  char orig [200], reco [200];  id_type i;

  fprintf (stderr, "Length %hd\n", ALPH_len (recode));

  for (i = 0;  i < ALPH_len (recode);  i ++)
    {
      if (! TEST_get_label (i, orig))
	strcpy (orig, "--");

      if (! TEST_get_label (ALPH_item (recode, i), reco))
	strcpy (reco, "--");

      fprintf (stderr, "%s\t%s\n", orig, reco);
    }
}

/*-----------------------------------------------------------*/
static int TESTaux_state_sequential
  (STATEptr state, ARCptr *arcset, int setlen, id_type *in_match)
{
  ARCptr arc;  id_type in_sym;

  memset (arcset, 0, setlen * sizeof (ARCptr));

  for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
    {
      in_sym = in_match [ARC_label (arc)];

      if (arcset [in_sym])  /* previous arc with same input symbol exists */
	{
	  if ((ARC_label (arc) != ARC_label (arcset [in_sym])) ||
	      (ARC_destination (arc) != ARC_destination (arcset [in_sym])))
	    return (FALSE);
	}
      else
	arcset [in_sym] = arc;
    }

  return (TRUE);
}

int TEST_state_sequential (STATEptr state, int side_p)
{
  ARCptr *arcset;  int setlen, result;  id_type *in_match;

  assign_match_tables (&in_match, NULL, side_p);
  setlen = LIM_id_count (LABEL_MAP);
  arcset = (ARCptr *) malloc (setlen * sizeof (ARCptr));

  result = TESTaux_state_sequential (state, arcset, setlen, in_match);

  free (arcset);
  return (result);
}

STATEptr TEST_fst_sequential (NETptr fst, int side_p)
{
  STATEptr state;  ARCptr *arcset;  int setlen;  id_type *in_match;

  assign_match_tables (&in_match, NULL, side_p);
  setlen = LIM_id_count (LABEL_MAP);
  arcset = (ARCptr *) malloc (setlen * sizeof (ARCptr));

  for (state = NET_states (fst);  state;  state = next_state (state))
    if (! TESTaux_state_sequential (state, arcset, setlen, in_match))
      break;    /* non-sequential state found */

  free (arcset);
  return (state);
}

#endif  /* DEBUG */


