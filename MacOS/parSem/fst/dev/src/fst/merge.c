/* $Id: merge.c,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990-97 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  MERGE.C
 **
 **  Lauri Karttunen, XRCE, November 1997
 **
 **  A variant of intersection: specialization of one finite-state
 **  language by another one.
 **
 ******************************************************/

#include <stddef.h>
#include <stdlib.h> 
#include <stdio.h>
#include <assert.h>

#include "fst/calculus.h"
#include "fst/btree.h"
#include "fst/fsm_io.h"
#include "fst/define.h"
#include "fst/state-mr.h"
#include "fst/mark.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

static int set_member(id_type id1, id_type id2)
{
  ALPHABETptr set;

  if ((set = (ALPHABETptr) LABEL_data(id_to_label(id1))))
    return(binary_sigma_member(set, id2));
  else
    return(FALSE);
}


static void merge_states(NETptr net, STATE_MRptr state_mr, int n2_cyclic_p)
{
  CONSptr cell;
  STATEptr state, dest, s1, s2;
  ARCptr arc, a1, a2;
  int match_p;
  ALPHABETptr sig = NET_sigma(net);
  ALPHABETptr labs = NET_labels(net);
  ALPHABETptr repl_alph = copy_alphabet(NET_labels(net));

  while((state = next_new_state(state_mr)))
    {
      cell = (CONSptr) STATE_client_cell(state);
      s1 = (STATEptr) car(cell);
      s2 = (STATEptr) cdr(cell);
      
      reclaim_cell(cell, state_mr);
      STATE_client_cell(state) = NULL;

      /* All states on the stack are initially marked as final to prevent
	 them from being pruned if the algorithm does intermittent
	 pruning. (Not the case here.)
	 */

      STATE_final(state) = (STATE_final(s1) && STATE_final(s2));

      for (a1 = STATE_arc_set(s1); a1; a1 = next_arc(a1))
	{
	  match_p = FALSE;

	  for (a2 = STATE_arc_set(s2); a2; a2 = next_arc(a2))
	    if (set_member(ARC_label(a1), ARC_label(a2)))
	      {
		match_p = TRUE;
		dest = find_pair_state(ARC_destination(a1),
				      ARC_destination(a2), state_mr);

		arc = make_arc(ARC_label(a2), dest, net);
		next_arc(arc) = STATE_arc_set(state);
		STATE_arc_set(state) = arc;
		sigma_add_to(sig, ARC_label(arc));
		sigma_add_to(labs, ARC_label(arc));
		sigma_add_to(repl_alph, ARC_label(a1));
	      }

	  /* Don't allow mismatch if the S2 has been successfully
	     visited before.
	     */

	  if (!match_p &&
	      !(n2_cyclic_p && sigma_member(repl_alph, ARC_label(a1))))
	    {
	      dest = find_pair_state(ARC_destination(a1), s2, state_mr);
	      arc = make_arc(ARC_label(a1), dest, net);
	      next_arc(arc) = STATE_arc_set(state);
	      STATE_arc_set(state) = arc;
	      sigma_add_to(sig, ARC_label(arc));
	      sigma_add_to(labs, ARC_label(arc));
	    }
	}
    }
  free_alph(repl_alph);
}

/*
static ALPHABETptr cache_set(ALPHABETptr labs)
{
  int i;
  ALPHABETptr set;

  binary_to_label(labs);

  set = make_alph(max_label_item(ALPH_items(labs), ALPH_len(labs)) + 1,
		  BINARY_VECTOR);

  for (i = 0; i < ALPH_len(labs); i++)
    ALPH_item(set, ALPH_item(labs, i)) = 1;

  return(set);
}
*/


static int set_net_p(NETptr net)
{
  /* Returns true if the net represents an alternation of symbols */

  return (NET_arc_label_arity(net) == 1 &&NET_num_states(net) == 2 &&
	  !STATE_final(NET_start_state(net)));
}

static void prepare_set_cache(NETptr net)
{

  ALPHABETptr labs = binary_to_label(NET_labels(net));
  id_type lab_id;
  int i, len = ALPH_len(labs);
  NETptr def;

  for (i = 0; i < len; i++)
    {
      lab_id = ALPH_item(labs, i);

      if (!LABEL_data(id_to_label(i)) &&
	  (def = get_def_uncopied(lab_id)) && set_net_p(def))
	define_label_set(lab_id, def);
    }
}

#if NOT_USED
static void free_set_cache(NETptr net)
{

  ALPHABETptr labs = NET_labels(net);
  int i;

  for (i = 0; i < ALPH_len(labs); i++)
    undefine_label_set(ALPH_item(labs, i));
}
#endif /* NOT_USED */

NETptr merge_fsm(NETptr n1, NETptr n2, int reclaim_p, int minimize_p)
{

  NETptr net;
  STATE_MRptr state_mr;
  int len1, len2;
  int n2_cyclic_p;

  if (!(NET_arc_label_arity(n1) == 1 && NET_arc_label_arity(n2) == 1))
    {
      handle_warning("The merge operation cannot be applied to a transducer",
		     "MERGE_FSM", 0);
      if (reclaim_p)
	{
	  free_network(n1);
	  free_network(n2);
	}
      return(null_fsm());
    }

  /* If any symbols in N1 have set definitions, cash them on the
     data slot of the label in the form of a binary sigma.
     */

  prepare_set_cache(n1);

  align_sigmas(n1, n2);

  /* Nets don't have to be deterministic but they must be epsilonfree.
     pair_state index will be kept in the state client cells of n1.
     */

  epsilon_remove(n1, DONT_COPY);
  epsilon_remove(n2, DONT_COPY);

  net = make_network();
  state_mr = init_state_manager(n1, n2, net, n1);

  len1 = max_label_item(ALPH_items(binary_to_label(NET_labels(n1))),
			ALPH_len(NET_labels(n1)));
  len2 = max_label_item(ALPH_items(binary_to_label(NET_labels(n2))),
			ALPH_len(NET_labels(n2)));

  NET_labels(net) = make_alph(len2 > len1 ? len2+1 : len1+1, BINARY_VECTOR);
  
  len1 = ALPH_len(label_to_binary(NET_sigma(n1)));
  len2 = ALPH_len(label_to_binary(NET_sigma(n2)));

  NET_sigma(net) = make_alph(len2 > len1 ? len2+1 : len1+1, BINARY_VECTOR);

  NET_start_state(net) = find_pair_state(NET_start_state(n1),
					NET_start_state(n2), state_mr);

  n2_cyclic_p = (index_net(n2) == 2);
  prepare_visit_marks(n2, 0);

  merge_states(net, state_mr, n2_cyclic_p);

  /* release memory */

  free_state_manager(state_mr);

  /*  free_set_cache(n1); */
  
  if (reclaim_p == DO_RECLAIM)
    {
      free_network(n1);
      free_network(n2);
    }

  /* Convert to standard type */
  binary_to_label(NET_labels(net));

  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);

  return(net);
}


NETptr merge(NETptr net1, NETptr net2)
{
  return(merge_fsm(net1, net2, DO_RECLAIM, DO_MINIMIZE));
}

#endif /* SLIM_FST */
