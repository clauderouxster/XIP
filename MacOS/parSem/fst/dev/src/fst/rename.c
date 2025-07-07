/* $Id: rename.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "fst/error.h"
#include "fst/struct.h"
#include "fst/rename.h"
#include "fst/mark.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

/******************************************************
 **
 **  RENAME.C
 **
 **	 Todd Yampol, Xerox PARC, January 1990
 **  Lauri Karttunen, Xerox PARC, April 1991
 **
 **  This used to be part of ASCII-IO.C
 **
 ******************************************************/


/****************************************
 *                                      *
 *   assigning integer names to states  *
 *                                      *
 ****************************************/


NETptr rename_states (NETptr net)
{
  /*****
   *
   * Assign a unique state numbers to client cells.  Assign numbers
   * in the order of reachability from the start state
   *
   *****/

  STATEptr state = NET_start_state(net);
  STACKptr arc_stack = create_stack("States to rename");
  unsigned long name = 0;
  ARCptr arc;
  
  /* Cannot count on client cells to avoid circularity because
     the integer 0 and the NULL pointer are not distinct, unlike
     0 and NIL in Lisp.  Marks get set back to 0.
     */
     
  prepare_visit_marks(net, 1);
  
  push((void *) STATE_arc_set(state), arc_stack);
  STATE_visit_mark(state) = 0;
  STATE_client_cell(state) = (void *) name++;
  
  while ((arc = (ARCptr) pop(arc_stack)))
    {
      if (next_arc(arc))
        push((void *) next_arc(arc), arc_stack);
        
      state = ARC_destination(arc);
      if (STATE_visit_mark(state) == 1)
        {
          STATE_visit_mark(state) = 0;
	  STATE_client_cell(state) = (void *) name++;
	  if (STATE_arc_set(state))
	    push((void *) STATE_arc_set(state), arc_stack);
        }
    }
    
  free_stack(arc_stack);
  NET_visit_marks_dirty(net) = FALSE;
/*  NET_names_smashed(net) = FALSE; */
  
  return(net);
}

NETptr top_sort_net(NETptr net)
{
  /*****
   *
   * Another method of assigning unique integer names to client cells.
   * Assign numbers in the order of minimum distance from the start state.
   * 'top' here stands for 'topological'.
   *
   *****/

  STATEptr state, dest, start_state = NET_start_state(net);
  
  unsigned long count = 1;
  ARCptr arc;
  CONSptr cur, next, last = NULL;
  
  init_cons_mem();

  clear_client_cells(net);
  
  /* We'll change the number of the start state to 0 when
     everything else has been done.
     */

  STATE_client_cell(start_state) = (void *) -1;

  next = cons((void *) start_state, NULL);

  while ((cur = next))
    {
      next = NULL;
      
      while (cur)
	{
	  state = (STATEptr) car(cur);
	  cur = freeing_CDR(cur);
	  if (!(STATE_is_virtual(state)))
	    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	      {
		dest = ARC_destination(arc);

		if (!(STATE_client_cell(dest)))
		  {
		    STATE_client_cell(dest) = (void *) count++;
	
		    /* Keep the original order of the destination states.
		       by appending the new state to the tail of the
		       list. */

		    if (next)
		      {
			cdr(last) = cons((void *) dest, NULL);
			last = cdr(last);
		      }
		    else
		      next = last = cons((void *) dest, NULL);
		}
	    }
	}
    }

  STATE_client_cell(start_state) = (void *) 0;

  free_cons_mem();

  /* Check state count if the net is marked as pruned and is
     not a virtual net. A virtual net may have arcs pointing
     to states that are not on its state list. In that case
     they will be marked on the first sort and retain the
     marks on subsequent passes. */

  if (!NET_is_virtual(net))
    {
      if (NET_pruned(net))
	{
	  if (NET_num_states(net) != (long)count)
	    handle_warning("Incorrect state count", "TOP_SORT_NET",
			   count - NET_num_states(net));
	}
      else if (NET_num_states(net) < (long)count)
	handle_warning("Incorrect state count", "TOP_SORT_NET",
		       count - NET_num_states(net));
    }
  return(net);
}

/****************************************
 *                                      *
 *   reordering states and arcs         *
 *                                      *
 ****************************************/

int default_arc_comp_fn (const void *a1, const void *a2)
{
  return(default_label_order(ARC_label (* (ARCptr *) a1),
                             ARC_label (* (ARCptr *) a2)));
} 

static int left_bm_label_order(id_type id1, id_type id2)
{
  /* DET_EPSILON arc must come first, OTHER must predede
     all labels except for the DET_EPSILON arc. ALTCHAIN_SYMBOL
     is always last. Other labels  are sorted numerically based
     on the upper id of the label. */

  if (id1 == id2)
    return(0);
  else if (id1 == DET_EPSILON || id1 == EPSILON)
    return(-1);
  else if (id2 == DET_EPSILON || id2 == EPSILON)
    return(1);
  else if (id1 == OTHER)
    return(-1);
  else if (id2 == OTHER)
    return(1);
  else if (id1 == ALTCHAIN_SYMBOL)
    return(1);
  else if (id2 == ALTCHAIN_SYMBOL)
  	return(-1);
  else if (id1 < id2)
    return(-1);
  else
    return(1);
}

static int downwards_left_bm_label_order(id_type id1, id_type id2)
{
   return(left_bm_label_order(upper_id(id1), upper_id(id2)));
}

int down_left_bm_arc_comp_fn (const void *a1, const void *a2)
{
  /* Function for sorting the arcs in a left bimachine that is
     to be applied downwards. */

  return(downwards_left_bm_label_order(ARC_label (* (ARCptr *) a1),
				       ARC_label (* (ARCptr *) a2)));
}


static int right_bm_label_order(id_type id1, id_type id2)
{
  /* ALTCHAIN_SYMBOL is always last. Other labels  are sorted
  numerically based on the input side of the label. */

  if (id1 == id2)
    return(0);
  else if (id1 == ALTCHAIN_SYMBOL)
    return(1);
  else if (id2 == ALTCHAIN_SYMBOL)
  	return(-1);
  else if (id1 < id2)
    return(-1);
  else
    return(1);
}


static int downwards_right_bm_label_order(id_type id1, id_type id2)
{
    /* Labels are sorted numerically based on the upper id of
       the label except that ALTCHAIN_SYMBOL is placed last. */

	return(right_bm_label_order(upper_id(id1), upper_id(id2)));
	
}

int down_right_bm_arc_comp_fn (const void *a1, const void *a2)
{
  /* Function for sorting the arcs in a left bimachine that is
     to be applied downwards. */

  return(downwards_right_bm_label_order(ARC_label (* (ARCptr *) a1),
					 ARC_label (* (ARCptr *) a2)));
}

static int upwards_left_bm_label_order(id_type id1, id_type id2)
{
  return(left_bm_label_order(lower_id(id1), lower_id(id2)));
}

int up_left_bm_arc_comp_fn (const void *a1, const void *a2)
{
  /* Function for sorting the arcs in a left bimachine that is
     to be applied upwards. */

  return(upwards_left_bm_label_order(ARC_label (* (ARCptr *) a1),
				     ARC_label (* (ARCptr *) a2)));
}

static int upwards_right_bm_label_order(id_type id1, id_type id2)
{

  /* Labels are sorted numerically based on the lower id of the 
     label except that ALTCHAIN_SYMBOL is always last. */
  
  return(right_bm_label_order(lower_id(id1), lower_id(id2)));
}

int up_right_bm_arc_comp_fn (const void *a1, const void *a2)
{
  /* Function for sorting the arcs in a right bimachine that is
     to be applied upwards. */

  return(upwards_right_bm_label_order(ARC_label (* (ARCptr *) a1),
				       ARC_label (* (ARCptr *) a2)));
}


/* Sort the arc set of each state according to a given comparison
   routine 'arc_comp_fn'. */
NETptr reorder_arcs (NETptr net, int arc_comp_fn(const void *,const void *))
{
  /* Create a temporary arc vector for each state, sort it as determined
   * by the arc_comp_fn, and relink the arcs in the sorted order.
   * If NET is a part of a bimachine, we check that it is being sorted
   * by the proper arc_comp_fn and set the NET_sorted_arcs flag
   * to indicate that the arcs have been sorted.
   * For other types of networks we set the flag ON to prevent the possible
   * disruption in the arc ordering that could be caused by some further 
   * operations, such as calculus, optimization, etc.
   * Be aware that some features such as the path_to_number mapping are 
   * highly dependent on the arc ordering.
   *
   * Smashes the STATE_client_cell of the start_state and sets the flag
   * NET_names_matter(net) to FALSE, (it is to force reindexation later
   * in case of possible path_to_number mapping).
   */
  
  STATEptr state;
  ARCptr arc;
  int i, j, max_arcs = ALPH_len(NET_labels(net));
  ARCptr *arc_vector;

  if (NET_shared_arc_lists(net))
    prog_error("reorder_arcs",
	       "Cannot reorder arcs when arcs are shared!\n");

  arc_vector = (ARCptr *) calloc((size_t) max_arcs, (size_t) sizeof(ARCptr));

  if (arc_vector == NULL )
    {
      if (NET_num_arcs(net) > 0)
      	handle_warning("Too little space to reorder", "REORDER_ARCS", 0);
      return(net);
    }
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
  {
      /* Don't process vectorized states: */
      if (!STATE_vector_p(state))
      {
          /* Don't bother unless STATE has at least two arcs */
    
          if (STATE_arc_set(state) != NULL &&
              next_arc(STATE_arc_set(state)) != NULL)
          {
              for (i = 0, arc = STATE_arc_set(state);
                   arc != NULL;
                   i++, arc = next_arc(arc))
              {
                  if (i == max_arcs)
                  {
                      max_arcs = (int) (max_arcs * 1.5);
                      /* YM, 2/23/96 add parenthesis to max_arcs * 1.5 */
                      arc_vector =
                          (ARCptr *) realloc((ARCptr *) arc_vector,
                                             (size_t) max_arcs * sizeof(ARCptr));
                      if (arc_vector == NULL)
                      {
                          handle_warning("Too little space to reorder",
                                         "REORDER_ARCS", 0);
                          return(net);
                      }
                  }
                  arc_vector[i] = arc;
              }
              QSORT((void *)arc_vector, (size_t) i,
                    (size_t) sizeof(ARCptr), arc_comp_fn);
              i--;
	
              for (j = 0; j < i; j++)
                  next_arc(arc_vector[j]) = arc_vector[j+1];
	
              next_arc(arc_vector[i]) = NULL;
	
              STATE_arc_set(state) = arc_vector[0];
          } 
      } /* if (!STATE_vector_p(current_state)) */
  } /* for */
  
  free((void *) arc_vector);

  /* We reset the client cell of the start state to NULL and reset
     the two flags to indicate that, if the net was indexed before
     it needs to be reindex for word-number mapping.
     */

  STATE_client_cell(NET_start_state(net)) = NULL;
  NET_names_matter(net) = FALSE;
/*  NET_names_smashed(net) = TRUE;  -- obsolete */

  /* If the network is a bimachine, it is important to know that the
     arcs have been sorted by the proper function. */

  if (NET_left_bimachine(net))
    {
      if ((NET_upper_seq(net) && arc_comp_fn == down_left_bm_arc_comp_fn) ||
	  (NET_lower_seq(net) && arc_comp_fn == up_left_bm_arc_comp_fn))
	NET_sorted_arcs(net) = TRUE;
      else
	NET_sorted_arcs(net) = FALSE;
    }
  else if (NET_right_bimachine(net))
    {
      if ((NET_upper_seq(net) && arc_comp_fn == down_right_bm_arc_comp_fn) ||
	  (NET_lower_seq(net) && arc_comp_fn == up_right_bm_arc_comp_fn))
	NET_sorted_arcs(net) = TRUE;
      else
	NET_sorted_arcs(net) = FALSE;
    }
  else
    {
        NET_sorted_arcs(net) = TRUE; /* CP */
    }

  return(net);
} /* reorder_arcs */

#endif /* SLIM_FST */
