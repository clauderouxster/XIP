/* $Id: define.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1997 by the Xerox Corporation.  All rights reserved */

/***********************************************************
 *
 *		       DEFINE.C
 *
 *	Defining symbols as names of networks.
 *      These functions used to be in RE-AUX.C
 *
 ***********************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "fst/struct.h"
#include "fst/modify-base.h"
#include "fst/define.h"
#include "fst/copy-fsm.h"
#include "fst/fsm_io.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

HASH_TABLEptr DEF_TABLE = NULL;

uint32 def_id_hashfn(void *id)
{
  return((uint32) id);
}

int def_id_comparefn(void *id1, void *id2)
{
  if (id1 < id2)
    return(-1);
  else if (id1 == id2)
    return(0);
  else
    return(1);
}


void init_def_table(void)
{
  DEF_TABLE = make_hash_table(DEF_BUCKETS, "Definitions");
}

NETptr get_def_uncopied(id_type label_id)
{
  void *value;

  if (label_id == EPSILON)
  {
    /*
      fsm_fprintf(stdout, "[GET_DEF_UNCOPIED] Label id is EPSILON: can't get network from DEF_TABLE !");
      Dropping this useless warning message.
      */
      return (NULL);
  }
  
  if (DEF_TABLE == NULL)
    init_def_table();
  
  if (get_hash(DEF_TABLE, (void *) ((int) label_id), &value, 
	       def_id_hashfn, def_id_comparefn) == 0 &&
      value != NULL)
    return((NETptr) value);
  else
    return(NULL);
}

static int present_p(id_type label_id, ALPHABETptr labs)
{
  /* Returns TRUE if LABEL_ID occurs on one side of a pair label */

  uint16 i; 
  id_type id;
  int present_p = FALSE;

  binary_to_label(labs);

  for (i = 0; i < ALPH_len(labs); i++)
    {
      id = ALPH_item(labs,i);
      if (LABEL_arity(id_to_label(id)) == 2 &&
	  (upper_id(id) == label_id || lower_id(id) == label_id))
	{
	  fsm_fprintf(stdout, "*** Warning: Defined symbol '");
	  print_label(label_id, stdout, DONT_ESCAPE);
	  fsm_fprintf(stdout, "' occurs as a component in '");
	  print_label(id, stdout, DONT_ESCAPE);
	  fsm_fprintf(stdout, "'.\n");
	  present_p = TRUE;
	}
    }
  return(present_p);
}


static NETptr eliminate_self_reference(id_type label_id, NETptr net)
{
  /* If LABEL_ID is being defined as an alias for NET, we need
     to eliminate any occurrences of it in NET. In cases such
     as 'define S [a S]', 'define S [S a]', or define E [E "+" E]'
     where LABEL_ID occurs at the left or the right edge of
     a regular expression, the self-reference in the definition
     is irrelevant because there is an equivalent regular expression
     that does not involve self-reference. That's because left-
     and right-linear grammars, S -> a S, for example. describe
     regular languages. In the case of definitions such as
     'define S [a S b]' the language described is not regular
     because 'S' occurs in the middle of the definition. That
     is, CF rules such as S -> a S b are stricly context-free
     languages. In such cases, eliminating the self-reference
     by the method used here yields a poor approximation of
     the CF language.
     */

  STATEptr state, state2, dest, start_state = NET_start_state(net);
  ARCptr arc, prev, next;
  int found_p = FALSE;

  if (sigma_member(NET_labels(net), label_id))
    {
      for (state = NET_states(net); state; state=next_state(state))
	{
	  prev = NULL;

	  for (arc = STATE_arc_set(state); arc; arc =next)
	    {
	      next = next_arc(arc);

	      if (ARC_label(arc) == label_id)
		{
		  found_p = TRUE;
		  dest = ARC_destination(arc);
		  
		  /* If we are at the start state, eliminate the arc.
		     Otherwise, relabel it with EPSILON are reroute
		     it to the start sate.
		     */

		  if (state == start_state)
		    {
		      if (prev)
			next_arc(prev) = next;
		      else
			STATE_arc_set(state) = next;

		      free_arc(arc);
		      NET_num_arcs(net)--;
		    }
		  else
		    {
		      ARC_destination(arc) = start_state;
		      ARC_label(arc) = EPSILON;
		      NET_epsilon_free(net) = FALSE;
		      STATE_deterministic(state) = FALSE;
		    }
		
		  /* Add an epsilon arc from all the final states of the
		     network to the destination state of the arc except
		     that no epsilon arc is needed to point from the
		     destination state to itself when the destination is
		     final.
		     */

		  for (state2 = NET_states(net);
		       state2;
		       state2=next_state(state2))
		    if (STATE_final(state2) && state2 != dest)
		      {
			add_arc(make_arc(EPSILON, dest, net), state2, net);
			NET_epsilon_free(net) = FALSE;
			STATE_deterministic(state2) = FALSE;
		      }
		}
	      else
		prev = arc;
	    }
	}
    }
  else
    present_p(label_id, NET_labels(net));

  if (found_p)
    {
      NET_deterministic(net) = FALSE;
      NET_minimized(net) = FALSE;
      NET_loop_free(net) = FALSE;
      NET_pruned(net) = FALSE;

      sigma_remove_from(NET_labels(net), label_id);

      if (!(present_p(label_id, NET_labels(net))))
	sigma_remove_from(NET_sigma(net), label_id);


      min_fsm(net, DONT_COPY);
    }

  return(net);
}



int define_label(id_type label_id, NETptr net, int recursive_p)
{
  /* Sets the definition of LABEL_ID to NET in
     DEF_TABLE and frees the previous value if any.
     If RECURSIVE_P is not NULL any instances of the symbol being
     defined in the net are taken to be self-referential. The
     effect is that 'define A [a A | b];' is equivalent to
     'define A [a* b]'. If RECURSIVE_P in NULL, no action is taken
     to deal with self reference. In that case, 'define A [a A | b];'
     defines A as the language [a A | b].
     */
  
  NETptr old_net = get_def_uncopied(label_id);
  
  if (old_net != NULL)
    free_network(old_net);

  if (net && recursive_p)
    net = eliminate_self_reference(label_id, net);
  
  return(put_hash(DEF_TABLE, (void *) ((int) label_id), (void *) net,
		  def_id_hashfn, def_id_comparefn));
}

int label_set_net_p(NETptr net)
{
  /*    min_fsm(net, DONT_COPY); */

    if (NET_num_states(net) != 2 && NET_minimized(net))
        return(FALSE);
    else
    {
        STATEptr state;

        for (state = NET_states(net); state; state = next_state(state))
            if (STATE_final(state) && !(STATE_arc_set(state)))
                return(TRUE);
    }

    return(FALSE);
}

/* compares a label with another, also (if it is there) via the 
 * alphabet vector that may be underlying some labels
 * because of define_label_set()
 */
int indirect_cmp_label_fn (id_type lab, id_type candidate_lab)
{
    LABELptr lab_ptr;
    ALPHABETptr alph_ptr;

   /* first check if there is direct match */
    if (lab == candidate_lab)
        return (TRUE);
        
		
   /* Now check if lab is one of the labels defined as "special" */
    if (SPECIAL_MATCH(lab, candidate_lab) == TRUE)
	return (TRUE);
	
   /* At last check if lab is a cover symbol and if so match
    * candidate_lab against the alphabet vector attached to lab
    */
    lab_ptr = id_to_label(lab);
    if (!LABEL_data(lab_ptr))
        return (FALSE);

    alph_ptr = (ALPHABETptr)LABEL_data(lab_ptr);

    if (ALPH_type(alph_ptr) != BINARY_VECTOR)
        return(FALSE);

    if (candidate_lab >= ALPH_len(alph_ptr))
        return (FALSE);

    return (ALPH_item(alph_ptr, candidate_lab));
}

int define_label_set(id_type label_id, NETptr net)
{
  /* Sets the definition of LABEL_ID to the copy of the NET's sigma,
     freeing the previous label set value if any. The network remains
     unchanged.
     */
  
    ALPHABETptr alph = label_set_value(label_id);
    int error_p = 0;
  
  if (alph != NULL)
    free_alph(alph);
  
  if (!label_set_net_p(net))
  {
      fsm_fprintf(stdout, "*** Incorrect 'define_label_set'. Too many states..\n");
      error_p = -1;
  }

  alph = copy_alphabet(sigma(net));
  LABEL_data(id_to_label(label_id)) = (void *) alph;

  return(error_p);
}

int undefine_label_set(id_type label_id)
{
  /* Resets the data field of the label to NULL and frees the
     value. Returns 0 if the label was defined, 1 otherwise.
     */

    ALPHABETptr alph = LABEL_data(id_to_label(label_id));

    if (!alph)
        return(1);

    LABEL_data(id_to_label(label_id)) = NULL;
    free_alph(alph);
    return(0);
}


ALPHABETptr label_set_value(id_type label_id)
{
    return((ALPHABETptr) LABEL_data(id_to_label(label_id)));
}


NETptr def_value(id_type label_id)
{
  /* Returns a copy of the value of LABEL_ID in DEF_TABLE if
     it is defined, otherwise NULL.
     */
  
  NETptr net;
  
  if (DEF_TABLE == NULL)
  {
    return(NULL);
  }
  
  if ((net = get_def_uncopied(label_id)))
    return(copy_fsm(net));
  else
    return(NULL);
} /* def_value */


int def_value_p(id_type label_id)
{
  /* Returns TRUE if LABEL_ID has a definition in DEF_TABLE,
     otherwise FALSE.
     */
  
  if (get_def_uncopied(label_id))
    return(TRUE);
  else
    return(FALSE);
}

int label_set_value_p(id_type label_id)
{
  /* Returns TRUE if LABEL_ID has a definition in its data fiels
     otherwise FALSE.
     */
  
  if (LABEL_data(id_to_label(label_id)))
    return(TRUE);
  else
    return(FALSE);
}


NETptr reset_label(id_type label_id)
{
  /* Looks for the definition of LABEL_ID in DEF_TABLE and removes it
     from the table.  Returns the network, if a definition was found,
     otherwise NULL.

     Also eliminates the alphabet in the label data field, if one
     is there.
     */

  NETptr net = get_def_uncopied(label_id);

  if (net != NULL)
    {
      del_hash(DEF_TABLE, (void *) ((int) label_id), def_id_hashfn,
               def_id_comparefn);
      return(net);
    }
  else
    return(NULL);
}

int undefine_label(id_type label_id)
{
    /* Looks for the definition of LABEL_ID DEF_TABLE and frees it along
       with the cell that contained it.  Returns 0 if label was defined,
       -1 if it didn't.
       */
    NETptr net = reset_label(label_id);
    int error_p = -1;
  
    if (label_set_value_p(label_id))
    {
        free_alph((ALPHABETptr) LABEL_data(id_to_label(label_id)));
        LABEL_data(id_to_label(label_id)) = NULL;
        error_p = 0;
    }

    if (net != NULL)
    {
        free_network(net);
        error_p = 0;
    }

    return(error_p);
}


#endif /* SLIM_FST */
