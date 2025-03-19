/* $Id: arc-optimization.c,v 1.4 2005/03/04 15:45:21 grondeau Exp $ */
/* Copyright (c) 1992 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  ARC-OPTIMIZATION.C
 **
 **  Lauri Karttunen, Xerox PARC, October 1992
 **
 **
 ******************************************************/

#include <stddef.h>
#include <stdlib.h> 
#include <stdio.h>
#include <limits.h>
#include <string.h>

#if defined _WINDOWS
#include <windows.h>
#elif !defined UNDER_CE
#include <sys/mman.h>
#endif

#include "fst/arc-optimization.h"
#include "fst/struct.h"
#include "fst/label.h"
#include "fst/fsm_io.h"
#include "fst/modify-base.h"
#include "fst/rename.h"
#include "fst/net-io.h"


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 


static int label_occurs_p(id_type id, NETptr net)
{
  STATEptr state;
  ARCptr arc;
  
  for (state = NET_states(net); state; state = next_state(state))
    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
      if (ARC_label(arc) == id)
	return(TRUE);

  return(FALSE);
}

#ifndef WORLD

/*
<html>
<title>Arc Optimization</title>
<body>
<h1>Arc Optimization</h1>
<p>
The <tt>optimize_arc_chains</tt> procedure arranges for common arc list
tails to be shared.  It finds the common arc list tails, creates new
states for them, and inserts epsilon arcs in the original states to
point to the new states.  It also reorders arcs in arc lists so that
more arcs will be in the common part of the tail.  The parameters to
<tt>optimize_arc_chains</tt> are: <tt>net</tt> (a finite state
machine), <tt>dont_reorder_labels</tt> (a list of labels that
shouldn't be reordered), <tt>dont_reorder_mark</tt> (a mark that
indicates that a state's arcs should not be reordered), and
<tt>ignore_state_mark</tt> (a mark that indicates that a state's arcs
should not be processed in any way).

<p>
There are four steps in <tt>optimize_arc_chains</tt>:

<ol>
<li>Mark all of the arcs that shouldn't be reordered (done in
    <tt>arc_chains_indexarcs</tt>),
<li>Index all of the arc lists by their tails except those whose state
    has the <tt>ignore_state_mark</tt> (<tt>arc_chains_indexarcs</tt>),
<li>Make small changes to the index arc order to make more arc tails
    collapse together (<tt>arc_chains_optimize_index</tt>), and
<li>Convert the indices back to arc lists, inserting epsilon arcs where
    arc tails are shared (<tt>arc_chains_convert_index</tt>).
</ol>

<p>
The main procedures that implement these steps are described below:

<h2>Procedure <tt>arc_chains_indexarcs</tt></h2>
<p>
This procedure marks all of the arcs of a state that shouldn't be
reordered and then indexes the arc list by their tail.  The root of the
index is stored in a list in the client cell of the destination state
of the last arc.  Then the arcs are indexed from last to first, with
each arc stored in a list in the arc user pointer of the arc that was just
indexed.  Finally, the state is stored at the end of the arc user pointer
list: in the arc user pointer of the last arc indexed (which is the first
arc of the arc list).

<h2>Procedure <tt>arc_chains_convert_index</tt></h2>
<p>
Building the indexes as described above automatically collapses tails
together.  When we reverse the process, we insert a state whenever an
arc's arc user pointer has more than one entity in it and add epsilon arcs
to the new state.

<h2>Procedure <tt>arc_chains_optimize_index</tt></h2>
<p>
While the arcs are still in their indexed form, it is possible to make
local changes the structure so that more arcs get collapsed together.
For instance, if we have a state with arcs a, b, and c (in that order),
and another state with arcs a and c (in that order), and both a and both c
arcs lead to the same state, then arc c can be shared.  But if the arcs in
the first state were in the order b, a, and c instead of a, b, and c, then
both the b and c arcs can be shared, instead of just the c arc.

<p>
<tt>arc_chains_optimize_index</tt> scans the arcs in an arc user pointer field,
looking for daughter arcs hat have the same label and destination as a
sister arc.  If either the arc or its daughter has the <tt>ARC_userflag1</tt>
bit set, then no swap is made, since that bit indicates that the arc
cannot be reordered.  If the daughter has any sisters, then no swap is
made, since all of the daughters would have to have matching sisters,
and this is too expensive to check.  There is also a restriction that
the daughter only have one daughter as well, although this restriction
might be unnecessary.  If an appropriate daughter is found, then the
mother gets the daughter's children and becomes a daughter of the
matching sister.  One complication in this is that if you are looking
at the arcs in the client cell of a state, then the sisters that are
checked are those in the client cell of the daughter's destination,
since this is where the daughter would be stored if it had been
swapped with the final arc before building the index.

<h2>Conclusion</h2>

The net effect of <tt>optimize_arc_chains</tt> is that 1) arcs can be
reordered, 2) new states may be created with epsilon arcs to those new
states.  The resulting machine is deterministic if the original
machine was deterministic, since there will be at most one arc with
each label in the epsilon closure of a state.
</body>
</html>
*/

static void add_altchain_symbol_to_unreduce_key(ALPHABETptr unreduce_key)
{
  if (!unreduce_key)
    return;
  
  /* To make sure there is space */

  sigma_add_to(unreduce_key, ALTCHAIN_SYMBOL);

  ALPH_item(unreduce_key, ALTCHAIN_SYMBOL) = ALTCHAIN_SYMBOL;

}

static void add_altchain_symbol_to_range_map(RANGEptr range_map)
{
  if (!range_map)
    return;

  if (RANGE_items(range_map))
    RANGE_items(range_map) =
      (id_type *) realloc(RANGE_items(range_map),
			(size_t) (RANGE_len(range_map) + 1) * sizeof(id_type));
  else
    RANGE_items(range_map) = malloc((size_t) sizeof(id_type));

  RANGE_item(range_map, RANGE_len(range_map)) = ALTCHAIN_SYMBOL;
  RANGE_len(range_map)++;
}


static void reclaim_match_tables(NETptr net)
{

  /* We could update the tables but it is simpler just to have them
     rebuilt, if the apply routines need them. */

  free_match_table(NET_upmatch_table(net));
  NET_upmatch_table(net) = NULL;

  free_match_table(NET_downmatch_table(net));
  NET_downmatch_table(net) = NULL;

}


static void add_altchain_symbol_class(NETptr net)
{
  /* Adds ALTCHAIN_SYMBOL as its own equivalence class.
     We need to modify the previous values of SIGMA_ATTR
     (should be renamed to LABELS_ATTR) and EQV_VECTOR_ATTR.
     by adding ALTCHAIN_SYMBOL to the end of the labels
     list and the number identifying a new equivalence class
     to the end of the eqv_vector array. If the net has
     a range map, ALTCHAIN_SYMBOL must be added to the
     set of epsilon transitions. If the net thas an
     unreduce key, add the information that ALTCHAIN_SYMBOL
     is the head of its singleton class. If the net has
     a match tables, we reclaim them.
  */

  OBJECTptr labels = get_value(net, SIGMA_ATTR);
  OBJECTptr map = get_value(net, EQV_VECTOR_ATTR);
  SEQUENCEptr seq;
  OBJECTptr *labs, *new_labs, new_obj;
  int seq_len;
  int i, max = 0;
  long int *key, *new_key;
  IO_SYMBOLptr sym;
  LABELptr label = id_to_label(ALTCHAIN_SYMBOL);

  if (!(labels && map))
    {
      prog_error("add_altchain_symbol", "Cannot find equivalence classes");
      return;
    }

  seq = OBJ_array(map);
  seq_len = SEQ_len(seq);
  key = SEQ_ints(seq);

  /* Find the number for the last equivalence classes */
  for (i = 0; i < seq_len; i++)
    if (key[i] > max)
      max = key[i];
     
  /* Allocate a new vector */
  new_key = (long *) malloc((size_t) ((seq_len + 1) * sizeof(long int)));
     
  /* Copy the contents of the old eqv vector */
     
  for (i = 0; i < seq_len; i++)
    new_key[i] = key[i];
       
  /* Install the new eqv vector and free the old one */
     
  new_key[seq_len] = max+1;
  SEQ_ints(seq) = new_key;
  SEQ_len(seq)++;
  free(key);
     
  /* Allocate a new label pointer list */
     
  seq = OBJ_list(labels);
  labs = SEQ_objects(seq);
  new_labs =
    (OBJECTptr *) malloc((size_t) ((seq_len + 1) * sizeof(OBJECTtype)));
     
  /* Copy the contents of the previous one */
     
  for (i = 0; i < seq_len; i++)
    new_labs[i] = labs[i];
     
  /* Install the new list and free the previous one. */
     
  SEQ_objects(seq) = new_labs;
  SEQ_len(seq)++;
  free(labs);
     
  /* Make a new label object and have it be pointed by the
     last object pointer. */
     
  new_obj = (OBJECTptr)  calloc((size_t) 1, (size_t) sizeof(OBJECTtype));
  new_labs[seq_len] = new_obj;
     
  /* Copy into the new object the name of ALTCHAIN_SYMBOL */
     
  OBJ_type(new_obj) = Symbol;
  sym = (IO_SYMBOLptr) malloc((size_t) sizeof(IO_SYMBOLtype));
  OBJ_symbol(new_obj) = sym;
                 
  IO_SYM_name(sym) = copy_fat_string(LABEL_atom(label));

  /* Add ALTCHAIN_SYMBOL to the range maps and unreduce_key, if any */

  add_altchain_symbol_to_range_map(NET_downrange_map(net));
  add_altchain_symbol_to_range_map(NET_uprange_map(net));
  add_altchain_symbol_to_unreduce_key(NET_unreduce_key(net));

  /* Reclaim match tables. */

  reclaim_match_tables(net);
}

#ifdef FULL_FSM

/******************************************************
 **
 **  SORT ARCS BY ARC INDEGREE
 **
 **  Todd Yampol, Xerox PARC, February 1993
 **
 **  Near-optimal arc sort for arc_chains.
 **
 ******************************************************/

ALPHABETptr BINARY_INITS = NULL, 
  BINARY_FINALS = NULL, 
  LABEL_INITS = NULL, 
  LABEL_FINALS = NULL;

void indegree_prepare_arcs(NETptr net)
     /****
      *
      * Clears arc visit marks & user pointers.
      *
      ****/
{
  STATEptr s;
  ARCptr a;
  
  for (s = NET_states(net); s != NULL; s = next_state(s))
    {
      for (a = STATE_arc_set(s); a != NULL; a = next_arc(a))
	{
	  ARC_user_pointer(a) = NULL;
	  ARC_visit_mark(a) = 0;
	}
    }
}

static void mark_arc_indegree(NETptr net)
     /*****
      *
      * Mark the arc indegree of each arc on the arc itself.
      * Arc indegree is the number of arcs with the same label
      * that point to a particular state.
      *
      *****/
{
  STATEptr state;
  ARCptr arc, tail, a;
  int label;
  long arccount;
  
  if (NET_has_arc_user_pointer(net))
    fsm_printf("Warning:  User pointers already in use.\n");
  
  indegree_prepare_arcs(net);
  invert_arcs(net, 1);
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
	{
	  if (!ARC_user_pointer(arc))
	    /* arc's label hasn't been counted yet. */
	    {
	      tail = arc;
	      label = ARC_label(arc);
	      
	      /* first pass:  count occurences of LABEL in STATE.
		 Start from tail since it is the first occurence.
		 */
	      for (a = tail, arccount = 0; a != NULL; a = next_arc(a))
		{
		  if (ARC_label(a) == label)
		    arccount++;
		}
	      
	      /* Second pass:  mark arccount on each appropriate arc */
	      for (a = tail; a != NULL; a = next_arc(a))
		{
		  if (ARC_label(a) == label)
		    ARC_user_pointer(a) = (void *) arccount;
		}
	    }
	}
    }
  
  invert_arcs(net, 0);
}


static int compare_alph_pos(register int lab1, register int lab2,
			    ALPHABETptr alph)
     /****
      *
      * Returns (-1) if lab1 precedes lab2 in alph.
      * Returns (1) otherwise.
      *
      ****/
{
  register int pos;
  
  for (pos = 0; pos < ALPH_len(alph); pos++)
    {
      if (ALPH_item(alph, pos) == lab1)
	return (-1);
      else if (ALPH_item(alph, pos) == lab2)
	return (1);
    }
  
  handle_error("Both labs should be in ALPH.  Neither found.",
	       "compare_alph_pos", 0);
  return(0);
}

static int arc_indegree_cmpfn (register const void *a1,
			       register const void *a2)
{
  /* Returns -1 if a1 precedes a2, 0 if they are the same, otherwise 1.
     
     Lower indegree precedes higher.  If indegree is the same,
     uses alphabetical order.  Initial labels precede all others.
     Final labels follow all others.  If lab1 & lab2 are both
     initial or both final, their relative order in the set
     determines the result. 
     */
  register int lab1 = ARC_label(*(ARCptr *) a1), 
  lab2 = ARC_label(*(ARCptr *) a2),
  indegree1, indegree2, 
  case_num = 0;
  
  if (BINARY_INITS)
    {
      if (binary_sigma_member(BINARY_INITS, lab1))  case_num += 1;
      if (binary_sigma_member(BINARY_INITS, lab2))  case_num += 2;
    }
  
  if (BINARY_FINALS)
    {
      if (binary_sigma_member(BINARY_FINALS, lab1)) case_num += 4;	
      if (binary_sigma_member(BINARY_FINALS, lab2)) case_num += 8;
    }
  
  switch (case_num)
    {
    case (0):  /* NORMAL CASE:  neither lab1 nor lab2 is initial or final */
      indegree1 = (int) ARC_user_pointer(*(ARCptr *) a1);
      indegree2 = (int) ARC_user_pointer(*(ARCptr *) a2);
      if (indegree1 == indegree2)
	return (alph_order(ARC_label(*(ARCptr *) a1),
			   ARC_label(*(ARCptr *) a2)));
      else
	return (indegree1 - indegree2);
      break;
    case (1):	/* only lab1 is inital */
    case (8):	/* only lab2 is final */
    case (9):	/* lab1 is initial, lab2 is final */
      return (-1); /* lab1 precedes lab2 */
      break;
    case (2):	/* only lab2 is initial */
    case (4):	/* only lab1 is final */
    case (6):	/* lab1 is final & lab2 is initial */
      return (1);  /* lab2 precedes lab1 */
      break;
    case (3):	/* both lab1 & lab2 are initial */
      return (compare_alph_pos(lab1, lab2, LABEL_INITS));
      break;
    case (12):  /* both lab1 & lab2 are final */
      return (compare_alph_pos(lab1, lab2, LABEL_FINALS));
      break;
    default:
      handle_error("impossible value", "arc_indegree_cmpfn", case_num);
      break;
    }
  return(0);
}

void sort_arcs_by_arc_indegree(NETptr net, ALPHABETptr initialchars, 
                               ALPHABETptr finalchars)
     /*****
      *
      * Sorts arc by arc indegree, but guarantees that labels within
      * INITIALCHARS come first and in the specified order, and that
      * FINALCHARS come last and in that order, modulo universal constrtaints
      * on special labels.
      *
      * In Interlisp, SIGMA is reordered.
      * In C, the order of SIGMA has no special
      * meaning.
      *
      *****/
{
/*
  fsm_printf("Sorting arcs by indegree...\n");
*/
  LABEL_INITS  = initialchars;
  LABEL_FINALS = finalchars;
  BINARY_INITS  = label_to_binary(copy_alphabet(LABEL_INITS));
  BINARY_FINALS = label_to_binary(copy_alphabet(LABEL_FINALS));
  
  /*	sort_arcs_fixedchars(net, initialchars, finalchars);
   */
  mark_arc_indegree(net);
  reorder_arcs(net, arc_indegree_cmpfn);
  indegree_prepare_arcs(net); /* clean up */
  
  free_alph(BINARY_INITS);
  free_alph(BINARY_FINALS);
  BINARY_INITS = NULL;
  BINARY_FINALS = NULL;
  LABEL_INITS = NULL;
  LABEL_FINALS = NULL;
}



/******************************************************
 **
 **  OPTIMIZE_ARC_CHAINS (alias ALTCHAINS)
 **
 **  Todd Yampol, Xerox PARC, February 1993
 **
 **  The algorithm optimizes an FSM for linear encoding.
 **  Reduces reduces arc count, but increases state count.
 **
 **  Must be compiled with FULL_FSM data structures
 **  because it needs ARCUSERPOINTER cells.
 **
 ******************************************************/

static void arc_chains_prepare_net(NETptr net)
     /***********
      *
      * Clears STATE: client_cell, ARC: user_pointer and userflag1
      * Check for double occurrence of ALTCHAIN_SYMBOL. The net
      * could already contain occurrences of this special symbol.
      *
      ***********/
{
  STATEptr state;
  ARCptr arc;
  int altchain_p;
  
  for (state = NET_states(net);
       state != NULL;
       state = next_state(state))
    {
      STATE_client_cell(state) = NULL;
      STATE_type_bit(state) = 1;

      altchain_p = FALSE;

      for (arc = STATE_arc_set(state);
	   arc != NULL;
	   arc = next_arc(arc))
	{
	  ARC_user_pointer(arc) = NULL;
	  ARC_userflag1(arc) = 0;
	  ARC_type_bit(arc) = 0;

	  if (ARC_label(arc) == ALTCHAIN_SYMBOL)
	    if (altchain_p)
	      {
		STATE_deterministic(state) = FALSE;
		NET_deterministic(net) = FALSE;
	      }
	    else
	      altchain_p = TRUE;
	}
    }
} /*  arc_chains_prepare_net */


static ARCptr reverse_arc_chain(ARCptr head)
{
  ARCptr next, prev = NULL;
  
  if (head)
    {
      while (next_arc(head))
	{
	  next = next_arc(head);
	  next_arc(head) = prev;
	  prev = head;
	  head = next;
	}		
      next_arc(head) = prev;
    }
  
  return (head);
}

ARCptr fetch_lastarc(ARCptr a)
     /* Returns the last arc on the chain headed by A.
	Note that the chain may end with a STATE, in which
	case the last REAL ARC is returned.
	*/
{
  while (next_arc(a) && ARC_P(next_arc(a)))
    a = next_arc(a);
  
  return (a);
}


static void arc_chains_addstate (ARCptr root, STATEptr newstate, NETptr net)
{
  void *rootindex = ARC_user_pointer(root);
  ARCptr lastarc = NULL;
  STATEptr oldstate;
  
  if (!rootindex) /* rootindex is null */
    {
      /* Index is empty.  Store state directly in ARC_user_pointer field. */
      ARC_user_pointer(root) = (void *) newstate;
      return;
    }
  else if (ARC_P(rootindex))	
    lastarc = fetch_lastarc(rootindex);
  
  /* Otherwise, rootindex is a STATE */
  if (lastarc && !next_arc(lastarc))
    {
      /* This is the first state.  Store it in the next_arc of the last arc. */
      next_arc(lastarc) = (void *) newstate;	
      return;				
    }
  else if (lastarc)
    oldstate = (STATEptr) next_arc(lastarc);
  else
    oldstate = (STATEptr) rootindex;
  
  /* Two states should only end up at the same place if one is final and
     the other is not.
     */
  if (STATE_final(oldstate) && !STATE_final(newstate))
    {
      /* Swap oldstate & newstate.
	 Make the non-final one be the representative */
      
      STATEptr temp;
      
      /* Must swap pointers as well
	 This is handled by XPOINTERS in Interlisp.
	 No need for them here. 
	 */	
      if (lastarc)
	next_arc(lastarc) = (ARCptr) newstate;
      else
	ARC_user_pointer(root) = newstate;
      
      temp = oldstate;
      oldstate = newstate;
      newstate = temp;
      
    }
  
  /* Make newstate have an EPSILON transition to OLDSTATE. */
  STATE_arc_set(newstate) = make_arc(ALTCHAIN_SYMBOL, oldstate, net);
}

static void arc_chains_indexarcs(STATEptr arcstate,
				ALPHABETptr dont_reorder_labels, 
                                int dont_reorder_mark, int reorder_p,
				NETptr net)
     /****
      *
      * Reverse the arcs in arcstate and put them in a tree stored in the
      * client_cell of INDEXSTATE, the destination of the first arc in
      * arcstate, after reversal.
      *
      ****/
{
  ARCptr a, arcset, indexarc;
  STATEptr indexstate, dest;
  void *arcindex, *root;
  int label, break_exit = FALSE;
  
  /* First mark the arcs that shouldn't be reordered. */
  if (reorder_p == FALSE ||
      dont_reorder_mark == STATE_visit_mark(arcstate))
    {
      /* mark all arcs as non-reorderable */
      for (a = STATE_arc_set(arcstate); a != NULL; a = next_arc(a))
	ARC_userflag1(a) = 1; /* 1 = don't reorder */
    }
  else if (dont_reorder_labels)
    {
      /* mark only arcs whose label is in dont_reorder_labels */
      for (a = STATE_arc_set(arcstate); a != NULL; a = next_arc(a))
	{
	  if (binary_sigma_member(dont_reorder_labels, ARC_label(a)))
	    ARC_userflag1(a) = 1; /* 1 = don't reorder */
	}
    }
  
  arcset = reverse_arc_chain(STATE_arc_set(arcstate));
  STATE_arc_set(arcstate) = NULL; /* sever to prevent circularity */
  
  /* put the arcs in a tree rooted in the client_cell of INDEXSTATE */
  indexstate = ARC_destination(arcset);
  root = indexstate;
  arcindex = STATE_client_cell((STATEptr) root);
  
  break_exit = FALSE;
  while (arcset != NULL)
    {
      a = arcset;
      arcset = next_arc(arcset);
      
      for (indexarc = arcindex, label = ARC_label(a),
	   dest = ARC_destination(a);
	   ARC_P(indexarc); 
	   indexarc = next_arc(indexarc))
	{
	  if (label == ARC_label(indexarc) &&  /* look for an existing arc. */
	      dest == ARC_destination(indexarc))
	    {	
	      /* this arc will get shared, so we can free it now */
	      free_arc(a);
	      NET_num_arcs(net)--;
	      
	      /* make the arc that was found the new root */
	      a = indexarc;
	      break_exit = TRUE;
	      break;
	    }
	}
      if (break_exit)
	break_exit = FALSE;
      else /* finally... */
	{
	  next_arc(a) = arcindex;
	  
	  if (STATE_P(root))
	    STATE_client_cell((STATEptr) root) = (void *) a;
	  else
	    ARC_user_pointer((ARCptr) root) = (void *) a;
	}
      
      /* the last arc we inserted becomes the root for the next arc. */
      root = a;
      arcindex = ARC_user_pointer((ARCptr) root);
    }
  
  /* Finally, store the state in the last NEXTARC cell of the next arc index */
  arc_chains_addstate(root, arcstate, net);
}

/* need prototype to allow recursion. */
long arc_chains_optimize_index(void *root, NETptr net);

static long arc_chains_merge_indices(void *root, void *auxindex, NETptr net)
     /* Recursively merge AUXINDEX into the index stored in ROOT. */
{
  long reorderings = 0;	
  ARCptr indexarc;
  int reoptimize = FALSE, break_exit;
  void *rootindex;
  ARCptr a, next_a;
  int label;
  STATEptr dest;
  
  rootindex = ARC_user_pointer((ARCptr) root);
  
  while (auxindex)
    {
      if (ARC_P(auxindex))
	{
	  indexarc = auxindex;
	  auxindex = next_arc((ARCptr) auxindex);
	  
	  for (a = rootindex, 
	       label = ARC_label(indexarc), 
	       dest = ARC_destination(indexarc),
	       break_exit = FALSE;
	       ARC_P(a);
	       a = next_a)
	    {
	      next_a = next_arc(a);
	      
	      if (label == ARC_label(a) && dest == ARC_destination(a))
		{
		  reorderings +=
		    arc_chains_merge_indices(a, ARC_user_pointer(indexarc),
					     net);
		  /* INDEXARC gets shared, so we can free it now */
		  free_arc(indexarc);
		  NET_num_arcs(net)--;
		  break_exit = TRUE;
		  break;
		}
	    }
	  /* FINALLY... */
	  if (break_exit)
	    break_exit = FALSE;
	  else
	    {
	      next_arc(indexarc) = ARC_user_pointer((ARCptr) root);
	      ARC_user_pointer((ARCptr) root) = indexarc;
	      reoptimize = TRUE;
	    }
	}
      else
	{
	  /* the last item of AUXINDEX might be a state */
	  arc_chains_addstate(root, auxindex, net);
	  return(reorderings);
	}
    }
  
  if (reoptimize == TRUE)
    {
      reorderings += arc_chains_optimize_index(root, net);
    }
  return (reorderings);
}

long arc_chains_optimize_index(void *root, NETptr net)
     /*****
      *
      * Look for arcs that have a single sub-arc which has a correspondent at
      * arc's level.  Putting sub-arc before arc will save space.
      *
      *****/
{
  long reorderings = 0;
  void *arcindex, *subarcindex = NULL, *subsubarcindex = NULL;
  ARCptr arc, nextarc = NULL, subarc = NULL, 
  prior = NULL, swaparc, a;
  STATEptr dest = NULL;
  int label;

  if (STATE_P(root))
    arcindex = STATE_client_cell((STATEptr) root);
  else
    arcindex = ARC_user_pointer((ARCptr) root);
  
  if (ARC_P(arcindex))
    arc = arcindex;
  else
    arc = NULL;
  
  while (arc)
    {
      /* Save NEXTARC because we smash the field below.
	 Ignore the final state.
	 */
      if (ARC_P(next_arc(arc)))
	nextarc = next_arc(arc);
      else
	nextarc = NULL;
      
      /* Determine whether or not to swap ARC with its sub-arc */
      swaparc = NULL;

      /* only swap arcs that are marked as re-orderable */
      if (!ARC_userflag1(arc))
	{
	  subarcindex = ARC_user_pointer(arc);
	  
	  /* only swap arc if it has exactly one sub-arc */
	  if (ARC_P(subarcindex) && !next_arc((ARCptr) subarcindex))
	    {
	      subarc = subarcindex;
	      subsubarcindex = ARC_user_pointer(subarc);

	      if (!(ARC_userflag1(subarc) ||
		    (ARC_P(subsubarcindex) &&
		     next_arc((ARCptr)subsubarcindex))))
		{
		  /* don't reorder if any of the original states were marked
		     with visit_mark, as indicated by ARC_userflag1.
		     */
		  
		  if (STATE_P(root))
		    a = STATE_client_cell(ARC_destination(subarc));
		  else
		    a = ARC_user_pointer((ARCptr) root);
		  
		  for (; ARC_P(a); a = next_arc(a))
		    {
		      label = ARC_label(subarc);
		      dest = ARC_destination(subarc);
		      if (label == ARC_label(a) &&
			  dest == ARC_destination(a))
			{
			  swaparc = a;
			  break;
			}						
		    }
		}    
	    }
	}
      
      if (!swaparc) /* recursively consider the sub-arcs */
	{
	  reorderings += arc_chains_optimize_index(arc, net);
	  prior = arc;
	} 
      else /* swap the arcs */
	{
	  /* remove ARC from the list */
	  if (prior)
	    next_arc(prior) = next_arc(arc);
	  else if (STATE_P(root))
	    STATE_client_cell((STATEptr) root) = next_arc(arc);
	  else
	    ARC_user_pointer((ARCptr) root) = next_arc(arc);
	  
	  /* merge ARC into the sub-arcs of SUBARC's twin. */
	  next_arc(arc) = NULL;
	  ARC_user_pointer(arc) = ARC_user_pointer(subarc);
	  
	  /* SUBARC gets shared, so we can free it now */
	  free_arc(subarc);
	  NET_num_arcs(net)--;
	  
	  reorderings += arc_chains_merge_indices(swaparc, arc, net);
	  /* NOTE:  arc_chains_merge_indices() will reoptimize the index
	     in case adding this index opens up new opportunities for swapping.
	     */
	  reorderings++;
	}
      arc = nextarc;
    }
  
  return (reorderings);
}

static void arc_chains_convert_index(void *arcindex, void *prior_arc_or_state,
				    NETptr net)
     /*****
      *
      * Recursively convert the arc index into arc sets, adding epsilon arcs 
      * where multple states share the arc.  The client must be sure to clear
      * the pointer to ARCINDEX when done.  NET is passed so states can be
      * added.
      *
      *****/
{
  void *arc_or_state = NULL, *next_arc_or_state = NULL, *next_arcindex = NULL;
  ARCptr lastarc = NULL, newarc = NULL;
  
  while (arcindex)
    {
      arc_or_state = arcindex;
      if (ARC_P(arcindex)) /* only advance index if it is an arc */
	arcindex = next_arc((ARCptr) arcindex);
      else
	arcindex = NULL;
      
      if (STATE_P(prior_arc_or_state))
	newarc = make_arc(ALTCHAIN_SYMBOL, prior_arc_or_state, net);
      else
	newarc = prior_arc_or_state;
      
      if (ARC_P(arc_or_state))
	{
	  /* Add newarc to charin of arcs */
	  next_arc((ARCptr) arc_or_state) = newarc; 
	  next_arcindex = ARC_user_pointer((ARCptr) arc_or_state);
	  ARC_user_pointer((ARCptr) arc_or_state) = NULL;
	  
	  if (ARC_P(next_arcindex) && next_arc((ARCptr) next_arcindex))
	    {
	      /* If there are multiple possible pointers to this arc,
		 create an epsilon state, using an existing state if possible.
		 */
	      lastarc = fetch_lastarc(next_arcindex);
	      if (STATE_P(next_arc(lastarc)) &&
		  !STATE_final((STATEptr) next_arc(lastarc)))
		{
		  /* if we have a non-final state, we can use it as
		     representative */
		  next_arc_or_state = next_arc(lastarc);
		  
		  /* remove state from chain so it won't be processed in
		     recursive call */
		  next_arc(lastarc) = NULL;
		  STATE_arc_set((STATEptr) next_arc_or_state) = arc_or_state;
		  
		}
	      else
		{
		  next_arc_or_state = make_state(arc_or_state, net);
		  STATE_deterministic((STATEptr) next_arc_or_state) = TRUE;
		}	
	    }
	  else
	    {
	      next_arc_or_state = arc_or_state;		
	    }
	  
	  if (arcindex)
	    {
	      /* recurse on next_arcindex & next_arc_or_state */
	      arc_chains_convert_index(next_arcindex, next_arc_or_state, net);
	    }
	  else
	    {
	      /* Simulate a tail-recursive call to avoid stack overflow */
	      arcindex = next_arcindex;
	      prior_arc_or_state = next_arc_or_state;
	      next_arc_or_state = NULL;
	      /* clear local variables */
	      next_arcindex = NULL;
	      lastarc = NULL;
	    }
	}
      else
	{
	  /* The last element may be the state that this 
	     sequence of arcs came from.  Add newarc to it. */
	  STATE_arc_set((STATEptr) arc_or_state) = newarc;
	}
    }
}

#define percent_message(input, output, name) \
  fsm_printf("%s: %+2.1f%% (%+ld)\n", name, \
	 (float) 100 * (output - input) / input, output - input)	


NETptr optimize_arc_chains (NETptr net, ALPHABETptr dont_reorder_labels, 
			    int dont_reorder_mark, int ignore_state_mark,
			    int reorder_p)
     /****
      *
      * Originally called ALTCHAINS. Renamed as not to baffle the user.
      *
      * Based on RK & JM's Interlisp code.  Their comments are
      * maintained to the two versions can be easily aligned.
      *
      * Reconfigures NET to optimize for linear encoding.
      *
      * Don't reorder arcs with labels in the dont_reorder_labels.
      *       NOTE: If reorder_p is FALSE, dont_reorder_labels is ignored.
      * Don't reorder arcs of states marked with dont_reorder_mark.
      * Don't process states marked with ignore_state_mark.
      * Don't reorder anything if reorder_p is FALSE.
      *
      ***/
{
  STATEptr s;
  long reorder_count = 0;
  long input_arcs, input_states;
  long output_arcs, output_states;
  int already_optimized_p = label_occurs_p(ALTCHAIN_SYMBOL, net);

  fsm_printf("OPTIMIZER INPUT SIZE:  ");
  print_net_size(net, stdout);
  input_arcs = NET_num_arcs(net);
  input_states = NET_num_states(net);
  
  if (!NET_minimized(net))
    fsm_printf("Warning:  Input net is not marked as minimized.\n");
  
  arc_chains_prepare_net(net); /* clears marks & user_pointers & such */
  label_to_binary(dont_reorder_labels);
  /* convert for unit time membership test */
  
  for (s = NET_states(net); s != NULL; s = next_state(s))
    if (STATE_arc_set(s) && STATE_visit_mark(s) != ignore_state_mark)
      arc_chains_indexarcs(s, dont_reorder_labels, dont_reorder_mark, 
			  reorder_p, net);
  
  if (reorder_p == TRUE)
    {
      for (s = NET_states(net);
	   s != NULL;
	   s = next_state(s))
	{
	  if (STATE_client_cell(s))
	    reorder_count += arc_chains_optimize_index(s, net);
	}
    }
  fsm_printf("%ld arcs reordered.\n", reorder_count);
  
  for (s = NET_states(net); s != NULL; s = next_state(s))
    {
      if (STATE_client_cell(s))
	{
	  arc_chains_convert_index(STATE_client_cell(s), NULL, net);
	  STATE_client_cell(s) = NULL;
	}
    }
  
  arc_chains_prepare_net(net); /* clean up */
  
  /* If the net already had altchain symbols, it may now be
     nondeterministic. In that case, it is also nonminimized.
     */

  if (!NET_deterministic(net))
    NET_minimized(net) = FALSE;

  output_arcs = NET_num_arcs(net);
  output_states = NET_num_states(net);

  if (input_arcs != output_arcs || input_states != output_states
      || label_occurs_p(ALTCHAIN_SYMBOL, net))
    {
      sigma_add_to(NET_sigma(net), ALTCHAIN_SYMBOL);
      sigma_add_to(NET_labels(net), ALTCHAIN_SYMBOL);
      binary_to_label(NET_labels(net));

      if (NET_reduced_labelset(net) && !already_optimized_p)
	add_altchain_symbol_class(net);
    }

  fsm_printf("OPTIMIZER OUTPUT SIZE: ");
  print_net_size(net, stdout);
  percent_message(input_arcs, output_arcs,     "  Arcs");
  percent_message(input_states, output_states, "States");
  
  return(net);
}

void do_optimize_arcs(NETptr net)
{
  int reorder_p = TRUE;
  /*
    ALPHABETptr initials = NULL, finals = NULL;
  */
  ALPHABETptr dont_reorder;


  if (NET_optimized(net) || possibly_already_optimized_p(net))
  {
      fsm_fprintf(stderr, "Nothing to do: ");
      fsm_fprintf(stderr, "the network has already been optimized.\n");
      return;
  }
  
  /* Don't allow DET_EPSILON to be reordered. It must
     stay in the first position. */

  if (NET_left_bimachine(net))
    {
      dont_reorder = make_alph(1, LABEL_VECTOR);
      ALPH_item(dont_reorder, 0) = DET_EPSILON;
      ALPH_len(dont_reorder) = 1;
    }
  else
    dont_reorder = NULL;
  
  /*
    reorder_p = yes_or_no("Allow arc reordering?", 'y');
  */

  if (reorder_p)
    sort_arcs_by_arc_indegree(net, dont_reorder);
  
  optimize_arc_chains(net, dont_reorder -1, -1,
		      reorder_p);

  /* If the net's arcs were previously sorted, this arc ordering is lost. */
  NET_sorted_arcs(net) = 0;

/* Check that some work could really be done: */
  if (label_occurs_p(ALTCHAIN_SYMBOL, net))
  {
        NET_optimized(net)= TRUE;
  }
  else
  {
        NET_optimized(net)= FALSE;
  }

  free_alph(dont_order);
}

#else
#ifndef SMALL_FSM

/* This version runs also if the compilation is NOT done with
 * FULL_FSM on, the only constraint being that we don't have it 
 * available with SMALL_FSM; 
 * to achieve this we use the BIG_ARC objects to replace 
 * the ARCs of the network we are dealing with (see types.h),
 * then we basically follow the same algorithm used in the FULL_FSM
 * version and then we copy back again the content of BIG_ARCs into
 * normal arcs.
 */

#define ARC_user_pointer(X) ((BIG_ARCptr)X)->user_pointer
#define free_big_arc(X)     free_cell((void *) (X), BIG_ARC_HEAP)

ARCptr make_big_arc (id_type label, STATEptr dest, NETptr net)
{
    ARCptr arc;
    BIG_ARCptr big_arc;

    arc = make_arc (label, dest, net);
    big_arc = (BIG_ARCptr) cell_alloc (BIG_ARC_HEAP);
    memcpy ((void*)big_arc, (void*)arc, sizeof(ARCtype));
    /* IMPORTANT!!! after copying the flags of ARC, set BIG_ARC_FLAG to 1 */
    ARC_big_arc_flag(big_arc) = 1;
    next_arc(big_arc) = NULL;
    ARC_user_pointer(big_arc) = NULL;
    free_arc(arc);

    return (ARCptr) big_arc;
}

void make_arcs_big(NETptr net)
{
    STATEptr state;
    ARCptr arc;
    BIG_ARCptr big_arc, first, prev = NULL;
    long arcs;

    for (state = NET_states(net); state != NULL; state = next_state(state))
    {
        first = NULL;
        arcs = 0;
        for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
        {
            big_arc = (BIG_ARCptr) cell_alloc (BIG_ARC_HEAP);
            memcpy ((void*)big_arc, (void*)arc, sizeof(ARCtype));
            /* IMPORTANT!!! after copying the flags of ARC, 
             * set BIG_ARC_FLAG to 1 
             */
            ARC_big_arc_flag(big_arc) = 1;
            next_arc(big_arc) = NULL;
            if (!first)
                first = prev = big_arc;
            else
            {
                next_arc(prev) = big_arc;
                prev = big_arc;
            }
            arcs++;
        }
        free_arc_set (state, net);
        NET_num_arcs(net) += arcs;
        STATE_arc_set(state) = (ARCptr) first;
    }
    return;
}

void make_arcs_small (NETptr net)
{
    STATEptr state;
    ARCptr arc, first, prev = NULL;
    BIG_ARCptr big_arc;

    for (state = NET_states(net); state != NULL; state = next_state(state))
    {
        first = NULL;
        for (big_arc = (BIG_ARCptr) STATE_arc_set(state); big_arc != NULL; big_arc = next_arc(big_arc))
        {
            arc = (ARCptr) cell_alloc (ARC_HEAP);
            memcpy ((void*)arc, (void*)big_arc, sizeof(ARCtype));
            /* IMPORTANT!!! After copying the flags of BIG_ARC, 
             * set BIG_ARC_FLAG to 0
             */
            ARC_big_arc_flag(arc) = 0;
            next_arc(arc) = NULL;
            if (!first)
                first = prev = arc;
            else
            {
                next_arc(prev) = arc;
                prev = arc;
            }
        }
        STATE_arc_set(state) = first;
    }
    return;
}

/******************************************************
 **
 **  SORT ARCS BY ARC INDEGREE
 **
 **  Todd Yampol, Xerox PARC, February 1993
 **
 **  Near-optimal arc sort for arc_chains.
 **
 ******************************************************/

ALPHABETptr BINARY_INITS = NULL, 
  BINARY_FINALS = NULL, 
  LABEL_INITS = NULL, 
  LABEL_FINALS = NULL;

static void indegree_prepare_arcs(NETptr net)
     /****
      *
      * Clears arc visit marks & user pointers.
      *
      ****/
{
  STATEptr s;
  ARCptr a;
  
  for (s = NET_states(net); s != NULL; s = next_state(s))
    {
      for (a = STATE_arc_set(s); a != NULL; a = next_arc(a))
	  ARC_visit_mark(a) = 0;
    }
}

static void mark_arc_indegree(NETptr net)
     /*****
      *
      * Mark the arc indegree of each arc on the arc itself.
      * Arc indegree is the number of arcs with the same label
      * that point to a particular state.
      *
      *****/
{
  STATEptr state;
  ARCptr arc, tail, a;
  int label;
  long arccount;
  
  if (NET_has_arc_user_pointer(net))
    fsm_printf("Warning:  User pointers already in use.\n");
  
  indegree_prepare_arcs(net);
  invert_arcs(net, 1);
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
	{
	  if (!ARC_user_pointer(arc))
	    /* arc's label hasn't been counted yet. */
	    {
	      tail = arc;
	      label = ARC_label(arc);
	      
	      /* first pass:  count occurences of LABEL in STATE.
		 Start from tail since it is the first occurence.
		 */
	      for (a = tail, arccount = 0; a != NULL; a = next_arc(a))
		{
		  if (ARC_label(a) == label)
		    arccount++;
		}
	      
	      /* Second pass:  mark arccount on each appropriate arc */
	      for (a = tail; a != NULL; a = next_arc(a))
		{
		  if (ARC_label(a) == label)
		    ARC_user_pointer(a) = (void *) arccount;
		}
	    }
	}
    }
  
  invert_arcs(net, 0);
}


static int compare_alph_pos(register int lab1, register int lab2,
			    ALPHABETptr alph)
     /****
      *
      * Returns (-1) if lab1 precedes lab2 in alph.
      * Returns (1) otherwise.
      *
      ****/
{
  register int pos;
  
  for (pos = 0; pos < ALPH_len(alph); pos++)
    {
      if (ALPH_item(alph, pos) == lab1)
	return (-1);
      else if (ALPH_item(alph, pos) == lab2)
	return (1);
    }
  
  handle_error("Both labs should be in ALPH.  Neither found.",
	       "compare_alph_pos", 0);
  return(0);
}

static int arc_indegree_cmpfn (register const void *a1,
			       register const void *a2)
{
  /* Returns -1 if a1 precedes a2, 0 if they are the same, otherwise 1.
     
     Lower indegree precedes higher.  If indegree is the same,
     uses alphabetical order.  Initial labels precede all others.
     Final labels follow all others.  If lab1 & lab2 are both
     initial or both final, their relative order in the set
     determines the result. 
     */
  register int lab1 = ARC_label(*(ARCptr *) a1), 
  lab2 = ARC_label(*(ARCptr *) a2),
  indegree1, indegree2, 
  case_num = 0;
  
  if (BINARY_INITS)
    {
      if (binary_sigma_member(BINARY_INITS, lab1))  case_num += 1;
      if (binary_sigma_member(BINARY_INITS, lab2))  case_num += 2;
    }
  
  if (BINARY_FINALS)
    {
      if (binary_sigma_member(BINARY_FINALS, lab1)) case_num += 4;	
      if (binary_sigma_member(BINARY_FINALS, lab2)) case_num += 8;
    }
  
  switch (case_num)
    {
    case (0):  /* NORMAL CASE:  neither lab1 nor lab2 is initial or final */
      indegree1 = (int) ARC_user_pointer(*(ARCptr *) a1);
      indegree2 = (int) ARC_user_pointer(*(ARCptr *) a2);
      if (indegree1 == indegree2)
	return (alph_order(ARC_label(*(ARCptr *) a1),
			   ARC_label(*(ARCptr *) a2)));
      else
	return (indegree1 - indegree2);
      break;
    case (1):	/* only lab1 is inital */
    case (8):	/* only lab2 is final */
    case (9):	/* lab1 is initial, lab2 is final */
      return (-1); /* lab1 precedes lab2 */
      break;
    case (2):	/* only lab2 is initial */
    case (4):	/* only lab1 is final */
    case (6):	/* lab1 is final & lab2 is initial */
      return (1);  /* lab2 precedes lab1 */
      break;
    case (3):	/* both lab1 & lab2 are initial */
      return (compare_alph_pos(lab1, lab2, LABEL_INITS));
      break;
    case (12):  /* both lab1 & lab2 are final */
      return (compare_alph_pos(lab1, lab2, LABEL_FINALS));
      break;
    default:
      handle_error("impossible value", "arc_indegree_cmpfn", case_num);
      break;
    }
  return(0);
}

void sort_arcs_by_arc_indegree(NETptr net, ALPHABETptr initialchars, 
                               ALPHABETptr finalchars)
     /*****
      *
      * Sorts arc by arc indegree, but guarantees that labels within
      * INITIALCHARS come first and in the specified order, and that
      * FINALCHARS come last and in that order, modulo universal constrtaints
      * on special labels.
      *
      * In Interlisp, SIGMA is reordered.
      * In C, the order of SIGMA has no special
      * meaning.
      *
      *****/
{
/*
  fsm_printf("Sorting arcs by indegree...\n");
*/
  LABEL_INITS  = initialchars;
  LABEL_FINALS = finalchars;
  BINARY_INITS  = label_to_binary(copy_alphabet(LABEL_INITS));
  BINARY_FINALS = label_to_binary(copy_alphabet(LABEL_FINALS));
  
  /*	sort_arcs_fixedchars(net, initialchars, finalchars);
   */
  mark_arc_indegree(net);
  reorder_arcs(net, arc_indegree_cmpfn);
  indegree_prepare_arcs(net); /* clean up */
  
  free_alph(BINARY_INITS);
  free_alph(BINARY_FINALS);
  BINARY_INITS = NULL;
  BINARY_FINALS = NULL;
  LABEL_INITS = NULL;
  LABEL_FINALS = NULL;
}



/******************************************************
 **
 **  OPTIMIZE_ARC_CHAINS (alias ALTCHAINS)
 **
 **  Todd Yampol, Xerox PARC, February 1993
 **
 **  The algorithm optimizes an FSM for linear encoding.
 **  Reduces reduces arc count, but increases state count.
 **
 ******************************************************/

static void arc_chains_prepare_net(NETptr net)
     /***********
      *
      * Clears STATE: client_cell, ARC: user_pointer and userflag1
      * Check for double occurrence of ALTCHAIN_SYMBOL. The net
      * could already contain occurrences of this special symbol.
      *
      ***********/
{
  STATEptr state;
  ARCptr arc;
  int altchain_p;
  
  for (state = NET_states(net);
       state != NULL;
       state = next_state(state))
    {
      STATE_client_cell(state) = NULL;
      STATE_type_bit(state) = 1;

      altchain_p = FALSE;

      for (arc = STATE_arc_set(state);
	   arc != NULL;
	   arc = next_arc(arc))
	{
	  ARC_user_pointer(arc) = NULL;
	  ARC_userflag1(arc) = 0;
	  ARC_type_bit(arc) = 0;

	  if (ARC_label(arc) == ALTCHAIN_SYMBOL)
	    {
	      if (altchain_p)
		{
		  STATE_deterministic(state) = FALSE;
		  NET_deterministic(net) = FALSE;
		}
	      else
		altchain_p = TRUE;
	    }
	}
    }
}

static ARCptr reverse_arc_chain(ARCptr head)
{
  ARCptr next, prev = NULL;
  
  if (head)
    {
      while (next_arc(head))
	{
	  next = next_arc(head);
	  next_arc(head) = prev;
	  prev = head;
	  head = next;
	}		
      next_arc(head) = prev;
    }
  
  return (head);
}

static ARCptr fetch_lastarc(ARCptr a)
     /* Returns the last arc on the chain headed by A.
	Note that the chain may end with a STATE, in which
	case the last REAL ARC is returned.
	*/
{
  while (next_arc(a) && ARC_P(next_arc(a)))
    a = next_arc(a);
  
  return (a);
}


static void arc_chains_addstate (ARCptr root, STATEptr newstate, NETptr net)
{
  void *rootindex = ARC_user_pointer(root);
  ARCptr lastarc = NULL;
  STATEptr oldstate;
  
  if (!rootindex) /* rootindex is null */
    {
      /* Index is empty.  Store state directly in ARC_user_pointer field. */
      ARC_user_pointer(root) = (void *) newstate;
      return;
    }
  else if (ARC_P(rootindex))	
    lastarc = fetch_lastarc(rootindex);
  
  /* Otherwise, rootindex is a STATE */
  if (lastarc && !next_arc(lastarc))
    {
      /* This is the first state.  Store it in the next_arc of the last arc. */
      next_arc(lastarc) = (void *) newstate;	
      return;				
    }
  else if (lastarc)
    oldstate = (STATEptr) next_arc(lastarc);
  else
    oldstate = (STATEptr) rootindex;
  
  /* Two states should only end up at the same place if one is final and
     the other is not.
     */
  if (STATE_final(oldstate) && !STATE_final(newstate))
    {
      /* Swap oldstate & newstate.
	 Make the non-final one be the representative */
      
      STATEptr temp;
      
      /* Must swap pointers as well
	 This is handled by XPOINTERS in Interlisp.
	 No need for them here. 
	 */	
      if (lastarc)
	next_arc(lastarc) = (ARCptr) newstate;
      else
	ARC_user_pointer(root) = newstate;
      
      temp = oldstate;
      oldstate = newstate;
      newstate = temp;
      
    }
  
  /* Make newstate have an EPSILON transition to OLDSTATE. */
  STATE_arc_set(newstate) = make_big_arc(ALTCHAIN_SYMBOL, oldstate, net);
}

static void arc_chains_indexarcs(STATEptr arcstate,
				ALPHABETptr dont_reorder_labels, 
                                int dont_reorder_mark, int reorder_p,
				NETptr net)
     /****
      *
      * Reverse the arcs in arcstate and put them in a tree stored in the
      * client_cell of INDEXSTATE, the destination of the first arc in
      * arcstate, after reversal.
      *
      ****/
{
  ARCptr a, arcset, indexarc;
  STATEptr indexstate, dest;
  void *arcindex, *root;
  int label, break_exit = FALSE;
  
  /* First mark the arcs that shouldn't be reordered. */
  if (reorder_p == FALSE ||
      dont_reorder_mark == STATE_visit_mark(arcstate))
    {
      /* mark all arcs as non-reorderable */
      for (a = STATE_arc_set(arcstate); a != NULL; a = next_arc(a))
	ARC_userflag1(a) = 1; /* 1 = don't reorder */
    }
  else if (dont_reorder_labels)
    {
      /* mark only arcs whose label is in dont_reorder_labels */
      for (a = STATE_arc_set(arcstate); a != NULL; a = next_arc(a))
	{
	  if (binary_sigma_member(dont_reorder_labels, ARC_label(a)))
	    ARC_userflag1(a) = 1; /* 1 = don't reorder */
	}
    }
  
  arcset = reverse_arc_chain(STATE_arc_set(arcstate));
  STATE_arc_set(arcstate) = NULL; /* sever to prevent circularity */
  
  /* put the arcs in a tree rooted in the client_cell of INDEXSTATE */
  indexstate = ARC_destination(arcset);
  root = indexstate;
  arcindex = STATE_client_cell((STATEptr) root);
  
  break_exit = FALSE;
  while (arcset != NULL)
    {
      a = arcset;
      arcset = next_arc(arcset);
      
      for (indexarc = arcindex, label = ARC_label(a),
	   dest = ARC_destination(a);
	   ARC_P(indexarc); 
	   indexarc = next_arc(indexarc))
	{
	  if (label == ARC_label(indexarc) &&  /* look for an existing arc. */
	      dest == ARC_destination(indexarc))
	    {	
	      /* this arc will get shared, so we can free it now */
	      free_big_arc(a);
	      NET_num_arcs(net)--;
	      
	      /* make the arc that was found the new root */
	      a = indexarc;
	      break_exit = TRUE;
	      break;
	    }
	}
      if (break_exit)
	break_exit = FALSE;
      else /* finally... */
	{
	  next_arc(a) = arcindex;
	  
	  if (STATE_P(root))
	    STATE_client_cell((STATEptr) root) = (void *) a;
	  else
	    ARC_user_pointer((ARCptr) root) = (void *) a;
	}
      
      /* the last arc we inserted becomes the root for the next arc. */
      root = a;
      arcindex = ARC_user_pointer((ARCptr) root);
    }
  
  /* Finally, store the state in the last NEXTARC cell of the next arc index */
  arc_chains_addstate(root, arcstate, net);
}

/* need prototype to allow recursion. */
long arc_chains_optimize_index(void *root, NETptr net);

static long arc_chains_merge_indices(void *root, void *auxindex, NETptr net)
     /* Recursively merge AUXINDEX into the index stored in ROOT. */
{
  long reorderings = 0;	
  ARCptr indexarc;
  int reoptimize = FALSE, break_exit;
  void *rootindex;
  ARCptr a, next_a;
  int label;
  STATEptr dest;
  
  rootindex = ARC_user_pointer((ARCptr) root);
  
  while (auxindex)
    {
      if (ARC_P(auxindex))
	{
	  indexarc = auxindex;
	  auxindex = next_arc((ARCptr) auxindex);
	  
	  for (a = rootindex, 
	       label = ARC_label(indexarc), 
	       dest = ARC_destination(indexarc),
	       break_exit = FALSE;
	       ARC_P(a);
	       a = next_a)
	    {
	      next_a = next_arc(a);
	      
	      if (label == ARC_label(a) && dest == ARC_destination(a))
		{
		  reorderings +=
		    arc_chains_merge_indices(a, ARC_user_pointer(indexarc),
					     net);
		  /* INDEXARC gets shared, so we can free it now */
		  free_big_arc(indexarc);
		  NET_num_arcs(net)--;
		  break_exit = TRUE;
		  break;
		}
	    }
	  /* FINALLY... */
	  if (break_exit)
	    break_exit = FALSE;
	  else
	    {
	      next_arc(indexarc) = ARC_user_pointer((ARCptr) root);
	      ARC_user_pointer((ARCptr) root) = indexarc;
	      reoptimize = TRUE;
	    }
	}
      else
	{
	  /* the last item of AUXINDEX might be a state */
	  arc_chains_addstate(root, auxindex, net);
	  return(reorderings);
	}
    }
  
  if (reoptimize == TRUE)
    {
      reorderings += arc_chains_optimize_index(root, net);
    }
  return (reorderings);
}

long arc_chains_optimize_index(void *root, NETptr net)
     /*****
      *
      * Look for arcs that have a single sub-arc which has a correspondent at
      * arc's level.  Putting sub-arc before arc will save space.
      *
      *****/
{
  long reorderings = 0;
  void *arcindex, *subarcindex = NULL, *subsubarcindex = NULL;
  ARCptr arc, nextarc = NULL, subarc = NULL, 
  prior = NULL, swaparc, a;
  STATEptr dest = NULL;
  int label;
  
  if (STATE_P(root))
    arcindex = STATE_client_cell((STATEptr) root);
  else
    arcindex = ARC_user_pointer((ARCptr) root);
  
  if (ARC_P(arcindex))
    arc = arcindex;
  else
    arc = NULL;
  
  while (arc)
    {
      /* Save NEXTARC because we smash the field below.
	 Ignore the final state.
	 */
      if (ARC_P(next_arc(arc)))
	nextarc = next_arc(arc);
      else
	nextarc = NULL;
      
      /* Determine whether or not to swap ARC with its sub-arc */
      swaparc = NULL;
      
      /* only swap arcs that are marked as re-orderable */
      if (!ARC_userflag1(arc))
	{
	  subarcindex = ARC_user_pointer(arc);
	  
	  /* only swap arc if it has exactly one sub-arc */
	  if (ARC_P(subarcindex) && !next_arc((ARCptr) subarcindex))
	    {
	      subarc = subarcindex;
	      subsubarcindex = ARC_user_pointer(subarc);
	      
	      if (!(ARC_userflag1(subarc) ||
		    (ARC_P(subsubarcindex) &&
		     next_arc((ARCptr)subsubarcindex))))
		{
		  /* don't reorder if any of the original states were marked
		     with visit_mark, as indicated by ARC_userflag1.
		     */
		  
		  if (STATE_P(root))
		    a = STATE_client_cell(ARC_destination(subarc));
		  else
		    a = ARC_user_pointer((ARCptr) root);
		  
		  for (; ARC_P(a); a = next_arc(a))
		    {
		      label = ARC_label(subarc);
		      dest = ARC_destination(subarc);
		      if (label == ARC_label(a) &&
			  dest == ARC_destination(a))
			{
			  swaparc = a;
			  break;
			}						
		    }
		}    
	    }
	}
      
      if (!swaparc) /* recursively consider the sub-arcs */
	{
	  reorderings += arc_chains_optimize_index(arc, net);
	  prior = arc;
	} 
      else /* swap the arcs */
	{
	  /* remove ARC from the list */
	  if (prior)
	    next_arc(prior) = next_arc(arc);
	  else if (STATE_P(root))
	    STATE_client_cell((STATEptr) root) = next_arc(arc);
	  else
	    ARC_user_pointer((ARCptr) root) = next_arc(arc);
	  
	  /* merge ARC into the sub-arcs of SUBARC's twin. */
	  next_arc(arc) = NULL;
	  ARC_user_pointer(arc) = ARC_user_pointer(subarc);
	  
	  /* SUBARC gets shared, so we can free it now */
	  free_big_arc(subarc);
	  NET_num_arcs(net)--;
	  
	  reorderings += arc_chains_merge_indices(swaparc, arc, net);
	  /* NOTE:  arc_chains_merge_indices() will reoptimize the index
	     in case adding this index opens up new opportunities for swapping.
	     */
	  reorderings++;
	}
      arc = nextarc;
    }
  
  return (reorderings);
}

static void arc_chains_convert_index(void *arcindex, void *prior_arc_or_state,
				    NETptr net)
     /*****
      *
      * Recursively convert the arc index into arc sets, adding epsilon arcs 
      * where multple states share the arc.  The client must be sure to clear
      * the pointer to ARCINDEX when done.  NET is passed so states can be
      * added.
      *
      *****/
{
  void *arc_or_state = NULL, *next_arc_or_state = NULL, *next_arcindex = NULL;
  ARCptr lastarc = NULL, newarc = NULL;
  
  while (arcindex)
    {
      arc_or_state = arcindex;
      if (ARC_P(arcindex)) /* only advance index if it is an arc */
	arcindex = next_arc((ARCptr) arcindex);
      else
	arcindex = NULL;
      
      if (STATE_P(prior_arc_or_state))
	newarc = make_big_arc(ALTCHAIN_SYMBOL, prior_arc_or_state, net);
      else
	newarc = prior_arc_or_state;
      
      if (ARC_P(arc_or_state))
	{
	  /* Add newarc to charin of arcs */
	  next_arc((ARCptr) arc_or_state) = newarc; 
	  next_arcindex = ARC_user_pointer((ARCptr) arc_or_state);
	  ARC_user_pointer((ARCptr) arc_or_state) = NULL;
	  
	  if (ARC_P(next_arcindex) && next_arc((ARCptr) next_arcindex))
	    {
	      /* If there are multiple possible pointers to this arc,
		 create an epsilon state, using an existing state if possible.
		 */
	      lastarc = fetch_lastarc(next_arcindex);
	      if (STATE_P(next_arc(lastarc)) &&
		  !STATE_final((STATEptr) next_arc(lastarc)))
		{
		  /* if we have a non-final state, we can use it as
		     representative */
		  next_arc_or_state = next_arc(lastarc);
		  
		  /* remove state from chain so it won't be processed in
		     recursive call */
		  next_arc(lastarc) = NULL;
		  STATE_arc_set((STATEptr) next_arc_or_state) = arc_or_state;
		  
		}
	      else
		{
		  next_arc_or_state = make_state(arc_or_state, net);
		  STATE_deterministic((STATEptr) next_arc_or_state) = TRUE;
		}	
	    }
	  else
	    {
	      next_arc_or_state = arc_or_state;		
	    }
	  
	  if (arcindex)
	    {
	      /* recurse on next_arcindex & next_arc_or_state */
	      arc_chains_convert_index(next_arcindex, next_arc_or_state, net);
	    }
	  else
	    {
	      /* Simulate a tail-recursive call to avoid stack overflow */
	      arcindex = next_arcindex;
	      prior_arc_or_state = next_arc_or_state;
	      next_arc_or_state = NULL;
	      /* clear local variables */
	      next_arcindex = NULL;
	      lastarc = NULL;
	    }
	}
      else
	{
	  /* The last element may be the state that this 
	     sequence of arcs came from.  Add newarc to it. */
	  STATE_arc_set((STATEptr) arc_or_state) = newarc;
	}
    }
}

#define percent_message(input, output, name) \
  fsm_printf("%s: %+2.1f%% (%+ld)\n", name, \
	 (float) 100 * (output - input) / input, output - input)	


NETptr optimize_arc_chains (NETptr net, ALPHABETptr dont_reorder_labels, 
			    int dont_reorder_mark, int ignore_state_mark,
			    int reorder_p)
     /****
      *
      * Originally called ALTCHAINS. Renamed as not to baffle the user.
      *
      * Based on RK & JM's Interlisp code.  Their comments are
      * maintained to the two versions can be easily aligned.
      *
      * Reconfigures NET to optimize for linear encoding.
      *
      * Don't reorder arcs with labels in the dont_reorder_labels.
      *       NOTE: If reorder_p is FALSE, dont_reorder_labels is ignored.
      * Don't reorder arcs of states marked with dont_reorder_mark.
      * Don't process states marked with ignore_state_mark.
      * Don't reorder anything if reorder_p is FALSE.
      *
      ***/
{
  STATEptr s;
  long reorder_count = 0;
  long input_arcs, input_states;
  long output_arcs, output_states;
  int already_optimized_p = label_occurs_p(ALTCHAIN_SYMBOL, net);

  fsm_printf("OPTIMIZER INPUT SIZE:  ");
  print_net_size(net, stdout);
  input_arcs = NET_num_arcs(net);
  input_states = NET_num_states(net);
  
  if (!NET_minimized(net))
    fsm_printf("Warning:  Input net is not marked as minimized.\n");
  
  arc_chains_prepare_net(net); /* clears marks & user_pointers & such */
  label_to_binary(dont_reorder_labels);
  /* convert for unit time membership test */
  
  for (s = NET_states(net); s != NULL; s = next_state(s))
    if (STATE_arc_set(s) && STATE_visit_mark(s) != ignore_state_mark)
      arc_chains_indexarcs(s, dont_reorder_labels, dont_reorder_mark, 
			  reorder_p, net);
  
  if (reorder_p == TRUE)
    {
      for (s = NET_states(net);
	   s != NULL;
	   s = next_state(s))
	{
	  if (STATE_client_cell(s))
	    reorder_count += arc_chains_optimize_index(s, net);
	}
    }
  fsm_printf("%ld arcs reordered.\n", reorder_count);
  
  for (s = NET_states(net); s != NULL; s = next_state(s))
    {
      if (STATE_client_cell(s))
	{
	  arc_chains_convert_index(STATE_client_cell(s), NULL, net);
	  STATE_client_cell(s) = NULL;
	}
    }
  
  arc_chains_prepare_net(net); /* clean up */
  
  /* If the net already had altchain symbols, it may now be
     nondeterministic. In that case, it is also nonminimized.
     */

  if (!NET_deterministic(net))
    NET_minimized(net) = FALSE;

  output_arcs = NET_num_arcs(net);
  output_states = NET_num_states(net);

  if (input_arcs != output_arcs || input_states != output_states
      || label_occurs_p(ALTCHAIN_SYMBOL, net))
    {
      sigma_add_to(NET_sigma(net), ALTCHAIN_SYMBOL);
      sigma_add_to(NET_labels(net), ALTCHAIN_SYMBOL);
      binary_to_label(NET_labels(net));

      if (NET_reduced_labelset(net) && !already_optimized_p)
	add_altchain_symbol_class(net);
    }

  fsm_printf("OPTIMIZER OUTPUT SIZE: ");
  print_net_size(net, stdout);
  percent_message(input_arcs, output_arcs,     "  Arcs");
  percent_message(input_states, output_states, "States");
  
  return(net);
}

void do_optimize_arcs(NETptr net)
{
  int reorder_p = TRUE;
  ALPHABETptr dont_reorder;


  if (NET_optimized(net) || possibly_already_optimized_p(net))
  {
      fsm_fprintf(stderr, "Nothing to do: ");
      fsm_fprintf(stderr, "the network has already been optimized.\n");
      return;
  }
  
  /* Dont allow DET_EPSILON arcs to be reordered. They must
     stay in the first position. */

  if (NET_left_bimachine(net))
    {
      dont_reorder = make_alph(1, LABEL_VECTOR);
      ALPH_item(dont_reorder, 0) = DET_EPSILON;
      ALPH_len(dont_reorder) = 1;
    }
  else
    dont_reorder = NULL;


  if (!BIG_ARC_HEAP)
    BIG_ARC_HEAP = init_heap(sizeof(BIG_ARCtype), 32768/sizeof(BIG_ARCtype),
			     "Big_Arcs");
  /* reorder_p =  yes_or_no("Allow arc reordering?", 'y'); */
  make_arcs_big (net);
  /*
    if (reorder_p)
    sort_arcs_by_arc_indegree(net, initials, finals);
  */

  if (reorder_p)
    sort_arcs_by_arc_indegree(net, dont_reorder, NULL);
    
  optimize_arc_chains(net, dont_reorder, -1, -1, reorder_p);

  make_arcs_small (net);

  
/* Check that some work could really be done: */
  if (label_occurs_p(ALTCHAIN_SYMBOL, net))
  {
        NET_optimized(net)= TRUE;
  }
  else
  {
        NET_optimized(net)= FALSE;
  }
  
  free_heap (BIG_ARC_HEAP);
  BIG_ARC_HEAP = NULL;

  free_alph(dont_reorder);

}
#endif /* SMALL_FSM */
#endif /* FULL_FSM */
#endif /* WORLD */
/* Keep the arc_UNoptimization available in Xfst-WORLD version
   for functions in calculus.c that detect optimized nets, and
   unoptimize them on-the fly.*/

/* Tests if some net arcs are labeled with ALTCHAIN symbols:
   helps to know if a net went previously through the
   do_optimize_arcs() function, but does not garantee that
   the net is optimized.*/
int possibly_already_optimized_p(NETptr net)
{
   return (label_occurs_p(ALTCHAIN_SYMBOL, net));
}

/******************************************************
 **
 **  UNOPTIMIZE_ARCS
 **
 **  Andre Kempe, RXRC Grenoble, November 1995
 **
 ******************************************************/

void do_unoptimize_arcs(NETptr net)
{
  /* Reverse function to 'do_optimize_arcs()'. Removes ALTCHAIN arcs. */

  STATEptr state;  ARCptr arc;

  if (!label_occurs_p(ALTCHAIN_SYMBOL, net))
    {
      fsm_fprintf(stderr, "Nothing to do:  ");
      fsm_fprintf(stderr, "The network has not been optimized.\n");
      return;
    }

  if (NET_shared_arc_lists (net))
    return;    /* arc lists must not be shared */

  for (state = NET_states (net); state; state = next_state (state))
    for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
      if (ARC_label (arc) == ALTCHAIN_SYMBOL)
	ARC_label (arc) = EPSILON;

  sigma_remove_from (NET_labels (net), ALTCHAIN_SYMBOL);
  sigma_remove_from (NET_sigma (net), ALTCHAIN_SYMBOL);

  sigma_add_to (NET_labels (net), EPSILON);
  sigma_add_to (NET_sigma (net), EPSILON);

  NET_optimized(net)= FALSE;
  NET_minimized (net) = FALSE;
  NET_pruned(net) = FALSE;
  NET_deterministic (net) = FALSE;
  NET_epsilon_free (net) = FALSE;

  min_fsm (net, DONT_COPY);
  prune_fsm (net, DONT_COPY);
  determinize_fsm (net, DONT_COPY);
}

#endif /* SLIM_FST */

