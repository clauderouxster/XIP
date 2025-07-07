/* $Id: share_arclist.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1992 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  COMPACT.C
 **
 **  Lauri Karttunen, Xerox PARC, October 1992
 **
 **  Conversion between standard and compact network formats.
 **
 ******************************************************/

#include <stddef.h>
#include <stdlib.h> 
#include <stdio.h>
#include <limits.h>
/* #include <memory.h> */

#if defined _WINDOWS
#include <windows.h>
#elif !defined UNDER_CE
#include <sys/mman.h>
#endif

#ifdef MAC_OS
#include <cstring>
#else
#include <string.h>
#endif /* MAC_OS */

#include "fst/syst-dep.h"
#include "fst/share_arclist.h"
#include "fst/error.h"
#include "fst/struct.h"
#include "fst/modify-base.h"
#include "fst/copy-fsm.h"
#include "fst/arc-optimization.h"
#include "fst/fsm_io.h"
#include "fst/vectorize.h"

/*
#include "fst/dbl-link.h"
#include "fst/btree.h"
#include "fst/net-io.h"
#include "fst/bin-in.h"
#include "fst/bin-out.h"
#include "fst/rename.h"
#include "fst/cmp-net.h"
#include "fst/pretty-p.h"
*/


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 


/******************************************************
 **
 **  (UN)SHARE_ARCS
 **
 **  Andre Kempe, RXRC Grenoble, November 1995
 **
 **  The algorithm works on the output of optimize_arcs.
 **  Replaces ALTCHAIN arcs by shared arc lists.
 **
 **  Needs the ARC_user_pointer and the ARC_userflag1.
 **  If compiled with normal FSM instead of FULL_FSM, arcs will
 **  be converted to BIGARCS that have the ARC_user_pointer.
 **  Cannot be compiled with SMALL_FSM.
 **
 ******************************************************/
#ifndef SMALL_FSM

/* needed for SHARE_make_shared_arclists() */
#define STATE_delete_mark(X)     (X)->unused

/* needed for SHARE_make_unshared_arclists() */
#define ARC_share_mark(X)        ARC_userflag1(X)
#define ARC_share_count(X)       ((long)ARC_user_pointer(X))
#define ARC_source(X)            ((STATEptr)ARC_user_pointer(X))


void SHARE_warning (char *fct_name, char *message)
{
  fsm_fprintf (stderr, "WARNING: in function '%s'\n    %s\n",
	   fct_name, message);
}

void SHARE_clear_deletemarks (NETptr net)
{
  STATEptr state;

  for (state = NET_states (net); state; state = next_state (state))
    STATE_delete_mark (state) = FALSE;
}

int SHARE_delete_altchain_arcs (NETptr net, int *altchain_remains)
{
  STATEptr state;  ARCptr previous, arc;
  int arc_vect_size = arc_vector_size(net);
  STATEptr *dest_array;
  int i;

  /* delete ALTCHAIN arcs, and mark states to be deleted */

  for (state = NET_states (net); state; state = next_state (state))
    if (!STATE_vector_p(state))
      {
	for (previous = NULL, arc = STATE_arc_set (state);
	     arc;
	     previous = arc, arc = next_arc (arc))
	  if (ARC_label (arc) == ALTCHAIN_SYMBOL)
	    {
	      if (next_arc (arc))   /* arc must be the last one in the list */
		{
		  SHARE_warning ("SHARE_delete_altchain_arcs()",
				 "ALTCHAIN arc is not the last one in the arc list. Initial (now probably corrupted) net will be returned.");
		  return (FALSE);
		}

	      /* An ALTCHAIN arc that points to a vectorized state
		 has to be kept. */

	      if (!STATE_vector_p(ARC_destination(arc)))
		{
		  if (previous)
		    next_arc (previous) =
		      STATE_arc_set (ARC_destination (arc));
		  else
		    STATE_arc_set (state) =
		      STATE_arc_set (ARC_destination (arc));

		  STATE_delete_mark (ARC_destination (arc)) = TRUE;
		  free_arc (arc);
		  NET_num_arcs (net)--;
		}
	      else
		{
		  STATE_delete_mark(ARC_destination(arc)) = FALSE;
		  *altchain_remains = TRUE;
		}
	    }
      }

  /* reset STATE_delete_mark if state can be reached from remaining arcs
     or if it is accessed by an arc vector.
  */


  for (state = NET_states (net); state; state = next_state (state))
    if (!STATE_vector_p(state))
      {
	for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
	  STATE_delete_mark (ARC_destination (arc)) = FALSE;
      }
    else
      {
	dest_array = AV_dest_array(STATE_arc_vector(state));
	for (i = 0; i < arc_vect_size ; i++)
	  {
	    if(dest_array[i])
	      STATE_delete_mark(dest_array[i]) = FALSE;
	  }
      }

  return (TRUE);
}

void SHARE_delete_marked_states (NETptr net)
{
  STATEptr state, state2, state3;

  for (state = NULL, state2 = NET_states (net); state2; )
    {
      if (STATE_delete_mark (state2))
	{
	  state3 = next_state (state2);

	  if (state)
	    next_state (state) = state3;
	  else
	    NET_states (net) = state3;

	  free_state (state2);
	  NET_num_states (net)--;
	  state2 = state3;
	}
      else
	{
	  state = state2;
	  state2 = next_state (state2);
	}
    }
}

NETptr SHARE_make_shared_arclists (NETptr net, int copy_p)
{
  NETptr net2;
  int altchain_remains = FALSE;

  if (NET_shared_arc_lists (net))
    return (net);    /* arc lists are already shared */

  if (! (ALPH_type(NET_sigma(net)) == BINARY_VECTOR))
      label_to_binary(NET_sigma(net));
  if (! binary_sigma_member(NET_sigma(net), ALTCHAIN_SYMBOL))
      return (net); /* No altchain-s so no need for arc list sharing. tg */

  if (copy_p == DO_COPY)
    net2 = copy_fsm (net);
  else
    net2 = net;

  SHARE_clear_deletemarks (net2);

  if (! SHARE_delete_altchain_arcs (net2, &altchain_remains))
    return (net);  /* Here we do not delete net2 (may-be different
		      from net), because net2 could be corrupted, and
		      its deletion could give a crash. */

  SHARE_delete_marked_states (net2);

  if (!altchain_remains)
    {
      /* Altchain arcs that point to vectorized states must be kept. */
      sigma_remove_from (NET_labels (net2), ALTCHAIN_SYMBOL);
      sigma_remove_from (NET_sigma (net2), ALTCHAIN_SYMBOL);
    }

  NET_shared_arc_lists (net2) = TRUE;

  /* If the net's arcs were previously sorted, this arc ordering is lost. */
  NET_sorted_arcs(net2) = 0;
  
  return (net2);
}
/*-----------------------------------------------*/

#ifndef TRUE /* Obsolete stuff */

void SHARE_clear_counts_and_marks (NETptr net)
{
  STATEptr state;  ARCptr arc;

  for (state = NET_states (net); state; state = next_state (state))
    for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
      {
    ARC_share_count (arc) = 0;
    ARC_share_mark (arc) = FALSE;
      }
}

void SHARE_count_shared_arcs (NETptr net)
{
  /* Counts for every arc, to how many arc lists it belongs, i.e.
     how often it is shared. */

  STATEptr state;  ARCptr arc;

  for (state = NET_states (net); state; state = next_state (state))
    for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
      ARC_share_count (arc)++;
}

void SHARE_mark_shared_arcs (NETptr net)
{
  /* Marks every arc that is shared by more states than the preceding
     arc in the same list. */

  STATEptr state;  ARCptr arc;  long num;

  for (state = NET_states (net); state; state = next_state (state))
    for (num = 1, arc = STATE_arc_set (state); arc; arc = next_arc (arc))
      if (num < ARC_share_count (arc))
    {              /* shared by more states than the preceding arc */
      ARC_share_mark (arc) = TRUE;
      num = ARC_share_count (arc);
    }
}

void SHARE_clear_arc_sources (NETptr net)
{
  STATEptr state;  ARCptr arc;

  for (state = NET_states (net); state; state = next_state (state))
    for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
      ARC_source (arc) = NULL;
}

void SHARE_reinsert_altchain_arcs (NETptr net)
{
  /* Reinserts ALTCHAIN arcs and additionnal states. */

  STATEptr state;  ARCptr arc, previous, altchain;

  for (state = NET_states (net); state; state = next_state (state))
    for (previous = NULL, arc = STATE_arc_set (state);
     arc;
     previous = arc, arc = next_arc (arc))
      if (ARC_share_mark (arc))
    {
      if (! ARC_source (arc))   /* arc has still no source state */
        {
          ARC_source (arc) = make_state (arc, net);
          STATE_deterministic (ARC_source (arc)) = TRUE;
        }

      altchain = make_arc (ALTCHAIN_SYMBOL, ARC_source (arc), net);

      if (previous)
        next_arc (previous) = altchain;
      else
        STATE_arc_set (state) = altchain;
    }
}

NETptr old_SHARE_make_unshared_arclists (NETptr net, int copy_p)
{
  /* THIS FUNCTION IS NO MORE USED SINCE make_arcs_big()
     AND make_arcs_small() DO NOT WORK WITH SHARED ARC LISTS
     BUT ARE NEEDED IF WE DO NOT HAVE FULL_FSM. */

  NETptr net2;  long num_arcs;

  if (copy_p == DO_COPY)
    net2 = copy_fsm (net);
  else
    net2 = net;

#ifndef FULL_FSM
  make_arcs_big (net2);   /* normal ARCS --> BIGARCS */
#endif /* FULL_FSM */

  SHARE_clear_counts_and_marks (net2);
  SHARE_count_shared_arcs (net2);
  SHARE_mark_shared_arcs (net2);

  SHARE_clear_arc_sources (net2);

  num_arcs = NET_num_arcs (net);
  SHARE_reinsert_altchain_arcs (net2);

  if (num_arcs < NET_num_arcs (net))  /* ALTCHAIN arcs were created */
    {
      sigma_add_to (NET_labels (net), ALTCHAIN_SYMBOL);
      sigma_add_to (NET_sigma (net), ALTCHAIN_SYMBOL);
    }

#ifndef FULL_FSM
  make_arcs_small (net2);   /* BIGARCS --> normal ARCS */
#endif /* FULL_FSM */
  return (net2);
}

#endif /* 0 */

#ifndef WORLD

NETptr SHARE_make_unshared_arclists (NETptr net, int copy_p)
{
  /* THIS IS AN ALTERNATIVE SOLUTION FOR THIS FUNCTION (see above). */

  NETptr net2;

  if (! NET_shared_arc_lists (net))
    return (net);    /* arc lists are already unshared */

  net2 = copy_fsm (net);      /* make the original network */
  NET_shared_arc_lists (net2) = FALSE;

  NET_optimized(net2)= FALSE;

  do_optimize_arcs (net2);

  if (copy_p == DONT_COPY)
    free_network (net);

  return (net2);
}
#endif /* WORLD */

#endif /* SMALL_FSM */

#endif /* SLIM_FST */

