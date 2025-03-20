/* $Id: ambclass.c,v 1.4 2005/03/04 15:45:21 grondeau Exp $
 * 
 * Copyright (C) 1994-2004, Xerox Corporation. All rights reserved.
 * 
 * Created by Pasi Tapanainen as "confuse.h". Rebaptized and modified by
 * Lauri Karttunen. Nov. 12. 1997.
 * 
 */

#include "fst/fsm-package.h"
#include "fst/fsm_io.h"
#include "fst/ambclass.h"
#include "fst/iy-stack.h"
#include "fst/mark.h"


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#define CONF	DUMMY_LABEL


static int epsilon_diacritic_p(id_type id)
{
  /* Returns TRUE if id is EPSILON, ALTCHAIN_SYMBOL, or a FLAG DIACTRITIC. */

  return(id == EPSILON || id == ALTCHAIN_SYMBOL || flag_diacritic_p(id));
}


static int check_paths(STATEptr state)
{
  /* Returns zero (= OK) if all the paths starting at STATE contains
     only epsilon or flag diacritics on the upper side.

     Interpretation of VISIT_MARK:
         0 : check already done
	 1 : check in progress
	 2 : unprocessed state
  */

  ARCptr arc;
  id_type id;

  if (STATE_visit_mark(state) == 2)
    {
      STATE_visit_mark(state) = 1;

      for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	{
	  id = upper_id(ARC_label(arc));

	  if (!epsilon_diacritic_p(id) || check_paths(ARC_destination(arc)))
	    return(1);
	}

      STATE_visit_mark(state) = 0;
    }
  return(0);
}

static int tag_label_p(id_type label_id)
{
  /* Returns TRUE iff the upper side of LABEL_ID is neither an
     epsilon diacritic nor OTHER and has a multicharacter string
     as its printname. */

  id_type id = upper_id(label_id);

  FAT_STR name = LABEL_atom(id_to_label(id));

  return (!(epsilon_diacritic_p(id) || id == OTHER || fat_strlen(name) == 1));
}


static int check_format(NETptr net)
{
  /* Returns zero (= OK) if the network contains a single empty
     final states; 1 (= not OK but fixable) if the arcs leaving
     a final states only have epsilons on the upper side;
     otherwise 2 (not OK and not fixable);
  */

  ARCptr arc;
  STATEptr state = NET_states(net);
  int check = 0;

  /* Set all visit marks to 2. They will be cleared if the check
     yields 0 or 1.
     */

  prepare_visit_marks(net, 2);

  for (state = NET_states(net); state; state=next_state(state))
    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
      if (tag_label_p(ARC_label(arc)) && STATE_arc_set(ARC_destination(arc)))
	{
	  check = 1;

	  /* tag arc pointing to a nonempty final state */

	  if (check_paths(ARC_destination(arc)))
	    return(2);
	}

  NET_visit_marks_dirty(net) = FALSE;
  return(check);
}

static void copy_subnet(ARCptr final_arc, STATEptr state, STATEptr final_state,
			NETptr net, STACKptr stack)
{
  /* If the client cell of STATE is empty, push a copy of the
     state into that slot. Only work on states whose client cell
     is empty, otherwise the state has already been copied or
     is in the process of being copied. If the state is final,
     make it nonfinal and add a final arc pointing to the
     new, empty final state.
     */

   ARCptr arc, new_arc;
   STATEptr new_state;

   if (!(STATE_client_cell(state)))
     {
       push((void *) state, stack);

       new_state = copy_state(state, net);
       STATE_client_cell(state) = (void *) new_state;

       for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	 copy_subnet(final_arc, ARC_destination(arc), final_state, net, stack);

       if (STATE_final(new_state))
	 {
	   /* Keep the label but change the destination of the
	      tag arc in making the copy. */

	   STATE_final(new_state) = FALSE;
	   new_arc = copy_arc(final_arc, net);
	   ARC_destination(new_arc) = final_state;
	   add_arc(new_arc, new_state, net);
	   STATE_deterministic(new_state) = FALSE;
	 }
     }
}

static void reroute_arcs(void *item)
{

  /* Go through the states on the stack. Reroute the arcs of the
     copied states from the original destination to the copy of
     the original destination if there is one.
     */

  ARCptr arc;
  STATEptr old_dest, new_state = STATE_client_cell((STATEptr) item);

  for (arc = STATE_arc_set(new_state); arc; arc = next_arc(arc))
    {
      old_dest = ARC_destination(arc);
      if (STATE_client_cell(old_dest))
	ARC_destination(arc) = (STATEptr) STATE_client_cell(old_dest);
      else if (!(STATE_final(old_dest) && !STATE_arc_set(old_dest)))
	handle_error("Expecting an empty final state", "REROUTE_ARCS", 0);
    }
}


static NETptr fix_source_net(NETptr net)
{
  /* Look for an arc pointing to a nonempty final state
     that does not have an epsilon on the upper side.
     Make a copy of the subnet starting at its destination
     making the inital state of the copy nonfinal.
     Reroute the current arc to the initial state of the
     copied subnet concatenate to the end of the subnet
     the network consisting of the original arc, now
     pointing to a new empty final state. */

  STATEptr state, state2, dest, new_final;
  ARCptr arc, next;
  STACKptr state_stack = create_stack("States to copy");
  id_type id;

  clear_client_cells(net);

  new_final = make_state(NULL, net);
  STATE_final(new_final) = TRUE;

  for (state = NET_states(net); state ; state = next_state(state))
    {
      for (arc = STATE_arc_set(state); arc; arc = next)
	{
	  next = next_arc(arc);
	  dest = ARC_destination(arc);
	  id = ARC_label(arc);
	  
	  if (tag_label_p(id) && STATE_arc_set(dest))
	    {
	      /* Tag arc pointing to a nonempty state. Make a
		 copy of the subnet using the client cells of
		 the original states to keep track of what
	         is a copy of what and to prevent looping. */

	      copy_subnet(arc, dest, new_final, net, state_stack);

	      /* Reroute the current arc to the copy and make
		 it an epsilon arc. */

      	      ARC_destination(arc) = (STATEptr) STATE_client_cell(dest);
      	      ARC_label(arc) = EPSILON;
	      STATE_deterministic(state) = FALSE;
	      
	      /* Reroute the copied arcs from the the original destinations
		 to the copies of the original destinations. */

	      map_stack(state_stack, reroute_arcs);

	      /* Reset the stack and clear client cells in use
		 to prepare for another round of copying. */

	      while ((state2 = (STATEptr) pop(state_stack)))
		STATE_client_cell(state2) = NULL;
	    }
	}
    }

  free_stack(state_stack);

  NET_deterministic(net) = FALSE;
  NET_epsilon_free(net) = FALSE;
  NET_pruned(net) = FALSE;
  NET_minimized(net) = FALSE;

  min_fsm(net, DONT_COPY);

  return(net);
}

static void split_pair_arc(id_type up_sym, id_type lo_sym, ARCptr arc, NETptr net,
			   ALPHABETptr labs)
{
  /* Replacing 'X:Y' pair with 'Y X' sequence */

  ARCptr new_arc;
  STATEptr new_state;

  new_arc = make_arc(up_sym, ARC_destination(arc), net);
  new_state = make_state(new_arc, net);
  ARC_destination(arc) = new_state;
  ARC_label(arc) = lo_sym;
  sigma_add_to(labs, up_sym);
  sigma_add_to(labs, lo_sym);
}


static void smash_lemmas(NETptr net)
{
  /* Replaces all symbol pairs by the lower-side symbol except for
     symbol pairs with a tag on the the upper side. */

  STATEptr state;
  ARCptr arc;
  ALPHABETptr old_labs, new_labs, new_sigma, special_sigma, lab_sigma;
  id_type id, up_sym, lo_sym, max_id;
  int i;

  /* Make sure sigma and label alphabets are in the expected format. */

  old_labs = binary_to_label(NET_labels(net));
  label_to_binary(sigma(net));

  /* First check if there is any work to do. */

  for (i = 0; i < ALPH_len(old_labs); i++)
    {
      id = ALPH_item(old_labs, i);
      if (tuple_id_p(id) && !(tag_label_p(id) && lower_id(id) == EPSILON))
	break;
    }

  /* If we did not break the previous loop, all arc labels are
     either single symbols or symbol pairs in which the upper
     side is a POS tag with an EPSILON on the lower side.
     */

  if (i == ALPH_len(old_labs))
    return;

  /* Prepare to compute a new label alphabet. */

  max_id = max_label_component(old_labs, BOTH_SIDES);
  new_labs = make_alph(max_id + 1, BINARY_VECTOR);

  /* Smash the upper side of the network except for tag symbols */

  for (state = NET_states(net); state; state = next_state(state))
    {
      STATE_deterministic(state) = FALSE;

      for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	{
	  id = ARC_label(arc);

	  if (tag_label_p(id))
	    {
	      /* If the lower side is not EPSILON, need to
		 preserve both symbols. */
	      if (lower_id(id) == EPSILON)
		sigma_add_to(new_labs, id);
	      else
		split_pair_arc(upper_id(id), lower_id(id), arc, net, new_labs);
	    }
	  else if (tuple_id_p(ARC_label(arc)))
	    {
	      up_sym = upper_id(id);
	      lo_sym = lower_id(id);

	      /* If the upper side is a flag diacritic and the
		 lower side is not an epsilon, need to preserve
		 both symbols */

	      if (flag_diacritic_p(up_sym) && lo_sym != EPSILON)
		split_pair_arc(up_sym, lo_sym, arc, net, new_labs);
	      else
		{
		  ARC_label(arc) = lo_sym;
		  sigma_add_to(new_labs, lo_sym);
		}
	    }
	  else
	    sigma_add_to(new_labs, id);
	}
    }

  /* See if there are symbols in the sigma that are not in the original
     label alphabet. */

  lab_sigma = make_alph(max_id + 1, BINARY_VECTOR);
  sigma_augment(lab_sigma, old_labs);
  special_sigma = sigma_difference(NET_sigma(net), lab_sigma);
  free_alph(lab_sigma);

  /* Switch to the new label alphabet */

  free_alph(old_labs);
  NET_labels(net) = binary_to_label(new_labs);

  /* Compute the new sigma. Add the elements in the original sigma that
     were not in the actual label alphabet. */

  max_id = max_label_component(new_labs, BOTH_SIDES);
  new_sigma = make_alph(max_id + 1, BINARY_VECTOR);
  sigma_augment(new_sigma, new_labs);
  free_alph(NET_sigma(net));
  NET_sigma(net) = sigma_union_dont_copy(new_sigma, special_sigma);
  free_alph(special_sigma);

  /* Determinize and minimize the result. */

  NET_deterministic(net) = FALSE;
  NET_epsilon_free(net) = FALSE;
  NET_minimized(net) = FALSE;

  min_fsm(net, DONT_COPY);
}


NETptr ambiguity_net(NETptr net, int copy_p, int minimize_p)
{
  /* Assumes that NET is a lexical transducer that maps each surface
     form to a single tag. Returns a transducer that maps each surface
     form to a sequence of tags that encodes the alternative analyses
     in the original transducer.

     The main part of this routine was written by Pasi Tapanainen. It
     comes from the function 'iy_confuse()' in confuse.c, one of the
     sources of the old FSC application.

     The original function assumes that the net is in other respects
     a simple fsm (no fstpairs) except for the POS tags that are
     encoded as fstpairs with EPSILON on the bottom side. Because
     this is an unnecessary limiting assumption, I added a "prolog"
     that preprocesses the incoming network, if possible, to match
     the requirements of Pasi's code. Nonfinal POS tags are moved
     to the end if they are only followed by epsilon arcs on the
     upper side. If the input is a lemmatizing transducer, the
     upper side symbols, except for the POS tags, are smashed.

     We assume here that any multicharacter symbol that is not
     EPSILON, OTHER, ALTCHAIN_SYMBOL, or a flagdiacritic is a
     POS tag. Each path must have one and only one POS tag.
  */

  STATEptr state;
  ARCptr *arc, conf, conf2;

  if (copy_p == DO_COPY)
    net = copy_fsm(net);

  /* Preprocessing */

  switch (check_format(net))
    {
    case 2:
      /* Nonfinal tags. Cannot repare. */
      fsm_fprintf(stderr, "Network not in correct format. Cannot proceed.\n");
      free_network(net);
      return(null_fsm());
      break;
    case 1:
      /* Nonfinal tags with epsilon tails on the upper side. Can be
	 fixed by moving the tags over the epsilons. */
      fix_source_net(net);
      break;
    default:
      break;
    }

  /* If the net contains symbol pairs in which the upper side is
     not a POS tag, that is, if the input is a lemmatizing
     transducer, we smash the upper side except for the tags */

  smash_lemmas(net);

  /* Beginning of Pasi's original code. */


  state = NET_states(net);
  while (state)
    {
      conf = NULL;
      arc = &STATE_arc_set(state);
      while (*arc)
	{
	  if (STATE_final(ARC_destination(*arc)))
	    {
	      conf2 = *arc;
	      *arc = next_arc(*arc);
	      next_arc(conf2) = conf;
	      conf = conf2;
	      if (STATE_arc_set(ARC_destination(conf)))
		{
		  fputs("Network not in correct format (",stderr);
		  print_label(ARC_label(conf),stderr, TRUE);
		  fputs(" ",stderr);
		  print_label(ARC_label(STATE_arc_set(ARC_destination(conf))),
			      stderr, TRUE);
		  fputs(")\n",stderr);
		  free_network(net);
		  return null_fsm();
		}
	    }
	  else
	    {
	      arc = &next_arc(*arc);
	    }
	}
      if (conf)
	{
	  add_new_arc(net, state, CONF, make_state(conf,net));
	}
      state = next_state(state);
    }
  NET_pruned(net) = FALSE;
  NET_minimized(net) = FALSE;
  NET_labels(net) = sigma_add_to(NET_labels(net), CONF);
  NET_sigma(net) = sigma_add_to(NET_sigma(net), CONF);
  net = min_fsm(net, DONT_COPY);
  reorder_arcs(net, default_arc_comp_fn);
  NET_labels(net) = sigma_remove_from(NET_labels(net), CONF);
  NET_sigma(net) = sigma_remove_from(NET_sigma(net), CONF);

  state = NET_states(net);
  while (state)
    {
      ARCptr arc, class, class2;
      for (arc = STATE_arc_set(state); arc && (ARC_label(arc) != CONF);
	   arc = next_arc(arc)) ;
      if (arc)
	{
	  if (next_arc(STATE_arc_set(ARC_destination(arc))))
	    {
	      class = STATE_arc_set(ARC_destination(arc));
	      while (next_arc(class))
		{
		  class2 = class;
		  class = next_arc(class);
		  next_arc(class2) = NULL;
		  ARC_destination(class2) = make_state(class,net);
		}
	      STATE_final(ARC_destination(class)) = 1;
	    }
	  ARC_label(arc) = ARC_label(STATE_arc_set(ARC_destination(arc)));
	  ARC_destination(arc) =ARC_destination(STATE_arc_set(ARC_destination(arc)));
	}
      state = next_state(state);
    }

  NET_pruned(net) = FALSE;
  NET_minimized(net) = FALSE;
  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);

  return(net);
}

static void merge_arcs(STATEptr state, STATEptr from)
{
  ARCptr arc;
  arc = STATE_arc_set(from);
  if (arc)
    {
      while (next_arc(arc)) arc = next_arc(arc);
      next_arc(arc) = STATE_arc_set(state);
      STATE_arc_set(state) = STATE_arc_set(from);
      STATE_arc_set(from) = NULL;
    }
}

NETptr iy_stringify(NETptr net, int copyp, int minp)
{
  STATEptr state, staterem;
  ARCptr arc, arcrem;

  net = min_fsm(net, copyp);
  state = NET_start_state(net);
  while (state)
    {
      arc = STATE_arc_set(state);
      if (arc)
	{
	  state = ARC_destination(arc);
	  arc = next_arc(arc);
	  while (arc)
	    {
	      if (state != ARC_destination(arc))
		{
		  merge_arcs(state,ARC_destination(arc));
		  ARC_destination(arc) = state;
		}
	      arc = next_arc(arc);
	    }
	}
      else break;
    }
  NET_pruned(net) = FALSE;
  net = prune_fsm(net, DONT_COPY);
  reorder_arcs(net, default_arc_comp_fn);

  staterem = make_state(NULL, net);
  state = NET_start_state(net);
  while (state)
    {
      STATE_final(state) = FALSE;
      arc = STATE_arc_set(state);
      if (arc)
	{
	  if (next_arc(arc))
	    {
	      if (ARC_label(arc) == ARC_label(next_arc(arc)))
		{
		  arcrem = next_arc(arc);
		  next_arc(arc) = next_arc(next_arc(arc));
		  next_arc(arcrem) = STATE_arc_set(staterem);
		  STATE_arc_set(staterem) = arcrem;
		}
	      else
		{
		  state = ARC_destination(arc) = make_state(next_arc(arc), net);
		  next_arc(arc) = NULL;
		}
	    }
	  else state = ARC_destination(arc);
	}
      else break;
    }
  STATE_final(state) = TRUE;

  NET_pruned(net) = FALSE;
  return min_fsm(net, DONT_COPY);
}


#endif /* SLIM_FST */
