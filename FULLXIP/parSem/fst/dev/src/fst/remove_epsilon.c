/* $Id: remove_epsilon.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1998 by the Xerox Corporation.  All rights reserved */

#include "fst/calculus.h"
#include "fst/subseq.h"
#include "fst/cfsm.h"
#include "fst/label.h"
#include "fst/struct.h"
#include "fst/types.h"
#include "fst/modify.h"
#include "fst/remove_epsilon.h"
#include "fst/mark.h"

#include "fst/net-io.h"
/* #include "auxilary_debug_functions.c" */

/******************************************************
 **
 **  remove_epsilon.c
 **
 **      Frederic Tendeau, INRIA-XRCE, July 1998
 **  computes transitive closure of upperside-epsilon transitions
 **  and replaces all upperside-epsilon transitions by transitions
 **  no epsilon in the upperside
 **
 ******************************************************/

static void remove_epsilon_arcs(NETptr net, STATEptr s);
static void apply_epsilon_closure(NETptr net, STATEptr s);
static int add_arcs (NETptr net, STATEptr state, OUT_CELLptr outputs, ARCptr first_arc);

void remove_epsilon (NETptr net)
{
  STATEptr state;

  allocate_seq_memory(NULL, 0);
  sq_net_epsilon_closures(net, upper_id, lower_id);

  /* all existing arcs are marked by 0
     all arcs created by the epsilon closure will be marked by 1 */
  prepare_visit_marks(net, 0);

  state = NET_states(net);
  
  while (state)
    {
      remove_epsilon_arcs(net, state);
      apply_epsilon_closure(net, state);
      state = next_state(state);
    }
  reclaim_seq_memory();
}

static void remove_epsilon_arcs(NETptr net, STATEptr state)
{
  ARCptr cur_arc, nxt_arc;

  cur_arc = STATE_arc_set(state);

  while (cur_arc)
    {
      if (upper_id(ARC_label(cur_arc)) == EPSILON)
	{
	  nxt_arc = next_arc(cur_arc);
	  cfsm_remove_arc((cfsm_arc) cur_arc, (cfsm_state) state, (cfsm_fsm) net);
	  cur_arc = nxt_arc;
	}
      else cur_arc = next_arc(cur_arc);
    }
}

static void apply_epsilon_closure(NETptr net, STATEptr state)
{
  Q2_LISTptr client = (Q2_LISTptr) STATE_client_cell(state);
  ARCptr arc;

  while (client)
    { 
      if (QL_st(client) && QL_w(client))
	{
	  arc = STATE_arc_set(QL_st(client));
	  while (arc)
	    {
	      if (upper_id(ARC_label(arc)) != EPSILON && ARC_visit_mark(arc) == 0)
		/* Only arcs with non-epsilon upperside AND that existed before the
		   application of the epsilon closure on the network are considered.
		   Otherwise the closure could be partly recomputed, giving rise to
		   spurious ambiguities */
		add_arcs(net, state, QL_w(client), arc);
	      arc = next_arc(arc);
	    }
	}
      client = next_cell(client);
    }
}

static void add_marked_arc(NETptr net, STATEptr from_state, id_type label, STATEptr to_state)
/* arcs created by epsilon closure are distinguished (visit_mark=1)
   from initial ones (visit_mark=0) */
{
  ARCptr arc = make_arc(label, to_state, net);

  ARC_visit_mark(arc) = 1; 
  add_arc(arc, from_state, net);
}

static int add_arcs (NETptr net, STATEptr state, OUT_CELLptr outputs, ARCptr first_arc)
{
  int error= NO_ERROR;
  id_type label_id;
  STATEptr s;

  if (lower_id(OC_id(outputs)) == EPSILON)
    label_id = ARC_label(first_arc);
  else
    {
      error = make_fstlabel(&label_id, upper_id(ARC_label(first_arc)), lower_id(OC_id(outputs)));
      if (error != NO_ERROR)
	return(error);

      if (OC_next(outputs) || lower_id(ARC_label(first_arc)) != EPSILON)
	  while (outputs)
	    {
	      s = make_state(NULL, net);
	      add_marked_arc(net, state, label_id, s);
	      state = s;
	      outputs = OC_next(outputs);
	      if (outputs)
		{
		error = make_fstlabel(&label_id, EPSILON, lower_id(OC_id(outputs)));
		if (error != NO_ERROR)
		  return(error);
		}
	      else
		{
		error = make_fstlabel(&label_id, EPSILON, lower_id(ARC_label(first_arc)));
		if (error != NO_ERROR)
		  return(error);
		}
	    }
    }
  add_marked_arc(net, state, label_id, ARC_destination(first_arc));
  return(error);
}


