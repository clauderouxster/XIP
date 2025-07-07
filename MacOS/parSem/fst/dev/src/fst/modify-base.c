/* $Id: modify-base.c,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  MODIFY-BASE.C
 **
 **  Todd Yampol, Xerox PARC, April 1990
 **  Lauri Karttunen, Xerox PARC, February 1991
 **
 **  Support for the finite-state calculus
 **
 ******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "fst/modify-base.h"
#include "fst/copy-fsm.h"
#include "fst/dbl-link.h"
#include "fst/mark.h"
#include "fst/pars-tbl.h"
#include "fst/arc-optimization.h"
#include "fst/fsm_io.h"

#ifndef SLIM_FST
void invert_arcs(NETptr net, int mark)
     /***********
      *
      * Inverts all arcs in net, switching source and destination.
      *
      ***********/
{
  STATEptr cur_state;
  ARCptr cur_arc, temp, prev;
  
  for (cur_state = NET_states(net);
       cur_state != NULL;
       cur_state = next_state(cur_state))
    {
      prev = NULL;
      for (cur_arc = STATE_arc_set(cur_state); cur_arc != NULL;)
        {
          temp = next_arc(cur_arc);
          if (ARC_visit_mark(cur_arc) != mark)
            {
              if (ARC_destination(cur_arc) == cur_state)
                /* Arcs looping back to their source
                 * don't need to be reversed.
                 */
                {
                  ARC_visit_mark(cur_arc) = mark;
                  prev = cur_arc;
                }
              else
                {
                  if (prev == NULL)
                    STATE_arc_set(cur_state) = next_arc(cur_arc);
                  else
                    next_arc(prev) = next_arc(cur_arc);
                  
                  next_arc(cur_arc) = STATE_arc_set(ARC_destination(cur_arc));
                  STATE_arc_set(ARC_destination(cur_arc)) = cur_arc;
                  ARC_destination(cur_arc) = cur_state;
                  ARC_visit_mark(cur_arc) = mark;
                }
            }
          else
            prev = cur_arc;
          
          cur_arc = temp;
        }
    }
}

int state_mark_zero_p(STATEptr state)
     /* keep_pred for delete_states */
{
  if (STATE_visit_mark(state) == 0)
    return (1);
  else
    return (0);
}

int state_mark_one_p(STATEptr state)
     /* keep_pred for delete_states */
{
  if (STATE_visit_mark((STATEptr) state) == 1)
    return (1);
  else
    return (0);
}

NETptr make_connected(NETptr net, int inverted_p)
     /*****
      *
      * MAKE_CONNECTED removes states from NET that
      * are not connected to its start state, or,
      * if INVERTED_P is TRUE, have no paths to final states.
      * Arcs must be inverted back in an 
      * additional external cleanup operation.
      * If INVERTED_P is TRUE, it means that arcs are 
      * inverted, and connectivity to final states is desired for pruning.
      *
      *****/
{
  STATEptr cur_state;
  STACKptr stack;
  
  prepare_state_marks(net, 1);
  
  stack = create_stack("States to connect");
  
  if (inverted_p == TRUE)
    {
      for (cur_state = NET_states(net);
           cur_state != NULL;
           cur_state = next_state(cur_state))
        if (STATE_final(cur_state)) 
          mark_reachable_states(cur_state, 0, stack);
      
      if (STATE_visit_mark(NET_start_state(net)) == 1)
        /* net is a null fsm */
        {
          STATE_visit_mark(NET_start_state(net)) = 0;
          free_arc_set(NET_start_state(net), net);
          NET_loop_free(net) = TRUE;
          ALPH_len(NET_labels(net)) = 0;
        }
    }
  else
    mark_reachable_states(NET_start_state(net), 0, stack);
  
  free_stack(stack);
  
  NET_states(net) = delete_states(net, state_mark_zero_p);
  
  if (inverted_p == TRUE)
    {
      NET_completed(net) = FALSE;
      NET_pruned(net) = TRUE;
    }
  
  NET_visit_marks_dirty(net) = FALSE;
  
  return(net);
}

void label_check (NETptr net)
{
  /* Updates the label set and the arity of NET. If NET_labels is previously set,
     reuses the old data structure, If the NET_labels field is NULL, constructs
     the correct label alphabet for NET.
     */
  
  id_type id, old_max, new_max = 0;
  int16 arity = 1;
  STATEptr state;
  ARCptr arc;
  ALPHABETptr labels = NET_labels(net);
  id_type *item;
  int count = 0;
  uint16 max_size = LIM_id_count(LABEL_MAP);
  static int16 *labs = NULL;
  static uint16 size = 0;
  
  if (size < max_size)
    {
      if (size == 0)
        {
          /* labs is a cache that does not get freed once it is allocated,
             so I don't call the standard alph functions here %%TSY */
          labs = (int16 *) calloc((size_t) max_size, (size_t) sizeof(int16));
          size = max_size;
        }
      else
        {
          /* labs is a cache that does not get freed once it is allocated,
             so I don't call the standard alph functions here %%TSY */
          labs = (int16 *) realloc(labs,
				   (size_t) max_size * (size_t) sizeof(int16));
          while (size < max_size)
            labs[size++] = 0;
        }
    }
  
  if (labels != NULL)
    {
      binary_to_label(labels);
      old_max = max_label_item(ALPH_items(labels), ALPH_len(labels));
    }
  else
    old_max = max_size;
  
  NET_arc_label_arity(net) = 1;
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
      {
        id = ARC_label(arc);
        
        if ((int16)LABEL_arity(id_to_label(id)) > arity)
          arity = LABEL_arity(id_to_label(id));
        
        if (id > old_max)
	  {
	    handle_warning("Unaccounted label", "LABEL_CHECK", id);
	    print_label(id, stdout, DO_ESCAPE);
	    fputc('\n', stdout);
	  }
        
        /* Be careful not to count EPSILON as belonging to
           the label alphabet of the net. Ancient bug fixed
           94-11-1. L.K.
           */

        if (id != EPSILON)
	  {
	    if (id > new_max)
	      {
		new_max = id;
		count++;
		labs[id] = 1;
	      }
	    else if (labs[id] == 0)
	      {
		labs[id] = 1;
		count++;
	      }
	  }
      }
  
  if (labels == NULL)
    {
      labels = make_alph(count, LABEL_VECTOR);
      NET_labels(net) = labels;
    }
  else if (count > ALPH_max(labels))
    {
      handle_warning("Unaccounted labels", "LABEL_CHECK", count);
      adjust_alph_size(labels, count);
    }
  
  ALPH_len(labels) = count;
  
  item = ALPH_items(labels);
  
  for (id = 0; id <= new_max; id++)
    if (labs[id])
      {
        *item++ = id;
        labs[id] = 0;
      }
  
  NET_arc_label_arity(net) = arity;
}
/***********************************
 *
 *  Label type conversions
 *
 ************************************/
/* commented out by CP-10/02/99 */
/********************************/

void renumber_net(NETptr net)
     /******
      *
      * Creates a dense set of labels for a single net,
      * renumbers arc labels in net accordingly.
      * Calling "renumber_arc_labels(net, ALPH_items(NET_labels(net)))"
      * reverts net to the original numbering.
      *
      ******/
{
  ALPHABETptr rev_labels;
  
  binary_to_label(NET_labels(net));
  rev_labels = reverse_map(NET_labels(net));
  renumber_arc_labels(net, rev_labels);
  free_alph(rev_labels);
} 

int TEMP_OTHER;                 

ALPHABETptr encoding_key, decoding_key;
/* this must be preserved to restore nets */

ALPHABETptr renumber_nets(NVptr nets)
     /*****
      *
      * Finds a single dense set of labels for NETS.
      * Renumbers arc labels in all nets accordingly.
      * 
      * Returns the key for restoring the nets to their original state.
      * renumber_arc_labels(net, key) will restore each net to its original
      * numbering. If the key is large, be sure to release it after all
      * the nets have been restored.
      *
      *****/
{
  int i;
  
  decoding_key = copy_alphabet(NET_labels(NV_net(nets, 0)));
  
  for (i = 1; i < NV_len(nets); i++)
    sigma_union_dont_copy(decoding_key, NET_labels(NV_net(nets,i)));
  
  decoding_key = binary_to_label(decoding_key);
  encoding_key = reverse_map(decoding_key);
  
  for (i = 0; i < NV_len(nets); i++)
    renumber_arc_labels(NV_net(nets,i), encoding_key);
  
  return (decoding_key);
}

/******************
 *                *
 *  WORD_INDICES  *
 *                *
 ******************/

static void unmark_path(VECTORptr vector)
{
  ARCptr arc;
    
  do {
    subtract_from_vector((void *)(&arc), vector);
    if (arc)
      {
    ARC_visit_mark(arc) = 0;
    STATE_visit_mark(ARC_destination(arc)) = 0;
      }
  } while(arc);
}

static int add_to_count(NETptr net, VECTORptr arc_vector, ARCptr arc, STATEptr state)
{
    /* Sub-routine of index_paths():
       - Retrieve the state from which "arc" is going out;
       - Update the count in its STATE_client_cell;
       - Move to the next arc by appending next_arc(arc) to arc_vector.
       (NB: supports labelSet reduced networks).
    */
    
  ARCptr prev_arc;
  STATEptr prev_state;
  unsigned long old_count, new_count;

  vector_element_at((void *)(&prev_arc), VECTOR_pos(arc_vector)-2, arc_vector);
  prev_state = ARC_destination(prev_arc);

  /* Count update: */
  old_count = (long int) STATE_client_cell(prev_state);

  if (NET_reduced_labelset(net))
  {
      int class_cardinal= 0;
      MATCH_TABLEptr match_table= NULL;

      if (NET_upmatch_table(net) == NULL)
      { /* Build one of the 2 match tables, (the side -upper or lower-
           doesn't matter since what we need is only the "eqv_classes"
           array: */
          get_match_table(net, LOWER);
      }

      match_table= NET_upmatch_table(net);
      class_cardinal= RANGE_len(MATCH_TABLE_class(match_table)+ ARC_label(arc));
      new_count = old_count + class_cardinal * ((unsigned long) STATE_client_cell(state));
  }
  else
  { /* Standard case: */
      new_count = old_count + (unsigned long) STATE_client_cell(state);

  }

  if (new_count < old_count)
    return(1);
  else
    STATE_client_cell(prev_state) = (void *) new_count;

  VECTOR_pos(arc_vector)--;
  ARC_visit_mark(arc) = 0;

  append_to_vector(next_arc(arc), arc_vector);

  return(0);
}  /* add_to_count */

static int index_paths(NETptr net)
{
  /* Recursive core of INDEX_NET. We implement this iteratively in order
     to avoid running out of stack space on very long paths.
     Returns 0 on success, 1 if the number of paths is greater than
     ULONG_MAX, and 2 if the net turns out to be cyclic.
     (NB: supports labelSet reduced networks).
  */
  
  ARCptr arc;
  ARCtype dummy_arc;
  STATEptr state;
  VECTORptr arc_vector = make_vector(50);
  int error = 0;
  
  /* Set up a dummy arc pointing to the start state */

  ARC_visit_mark(&dummy_arc) = 1;
  ARC_label(&dummy_arc) = DUMMY_LABEL;
  ARC_destination(&dummy_arc) = NET_start_state(net);
  next_arc(&dummy_arc) = NULL;
    
  /* Start the depth-first process. If no error occurs, the process
     will terminate when we rebound back to the dummy arc. */

  append_to_vector(&dummy_arc, arc_vector);
    
  do {
    vector_element_at((void *)(&arc), VECTOR_pos(arc_vector)-1, arc_vector);
        
    if (arc)
      {
    state = ARC_destination(arc);
            
    if (ARC_visit_mark(arc) == 0)
      {
        /* Arc processed. Add the count at the destination to
           the count of the state where the arc originates.
        */
                     
        STATE_visit_mark(state) = 0;

            /* - Retrieve the state from which "arc" is going out;
               - Update the count in its STATE_client_cell;
               - Move to the next arc by appending next_arc(arc) to arc_vector.
            */
        if (add_to_count(net, arc_vector, arc, state))
          error = 1;
      }
    else
      {
        /* Clear the mark to indicate that the arc has
           been processed. */

        ARC_visit_mark(arc) = 0;
                    
        switch(STATE_visit_mark(state))
          {
          case 0: /* Nothing to do */

                  /* - Retrieve the state from which "arc" is going out;
                     - Update the count in its STATE_client_cell;
                     - Move to next arc by appending next_arc(arc) to arc_vector.
                  */ 
                  if (add_to_count(net, arc_vector, arc, state))
                      error = 1;
        break;
          case 1: /* Loop dected */
        error = 2;
        break;
          case 2:  /* Continue path */
        STATE_visit_mark(state) = 1;

        if (STATE_final(state))
          STATE_client_cell(state) = (void *) 1;

        append_to_vector(STATE_arc_set(state), arc_vector);
        break;
          default:
        prog_error("index_paths", "Not a legal visit_mark %d\n",
               STATE_visit_mark(state));
        break;
          }
      }
      } /*  if (arc) */
    else
      VECTOR_pos(arc_vector)--;
  } while (!error && VECTOR_pos(arc_vector) > 1);

  unmark_path(arc_vector);
  free_vector(arc_vector);
  return(error);
}

static void prepare_to_index_net(NETptr net)
{
  /* Sets state visit marks to 2 and arc visit marks to 1
     to indicate that they need to be processed. State client
     are cleared. When the index is finished, the client
     cells contain the number of valid paths originating
     at the state. The state and arc visit marks are
     set to 0.
  */
  STATEptr state;
  ARCptr arc;

  for (state = NET_states(net); state; state = next_state(state))
    {
      STATE_client_cell(state) = NULL;
      STATE_visit_mark(state) = 2;

      for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
    ARC_visit_mark(arc) = 1;
    }

  NET_visit_marks_dirty(net) = TRUE;

}


int index_net (NETptr net)
{
  /* Replaces the client cell of each state with the number of words
     reachable from the state. Returns 0 if the operation succeeds, 1
     if it fails because the there are more than ULONG_MAX paths, 2 if
     the net turns out to be cyclic. Sets the NET_loop_free flag
     correctly. Turns NET_names_matter to TRUE to indicate that state
     client cells contain useful information.

     Returns 0 on success, 1 if the number of paths is greater than
     ULONG_MAX, 2 if the net turns out to be cyclic, and 3
     if the net is a virtual net.

     (NB: supports labelSet reduced networks. INDEX_NET is usually
     called before running path_to_numbers_cmp() that does the path to
     numbers mapping and that also supports labelset reduced
     networks).  */
  
  int not_countable;
  
  if (NET_is_virtual(net))
    return(3);

  prepare_to_index_net(net);
  
  not_countable = index_paths(net);

  switch (not_countable)
    {
    case 2:    /* Loop was found. Clean up. */
      clear_state_marks_and_client_cells(net);

      if (NET_loop_free(net))
    {
      handle_warning("Net incorrectly marked as loopfree",
             "INDEX_NET", NET_loop_free(net));
      NET_loop_free(net) = FALSE;
    }
      break;
    case 1:   /* Overflow. More than ULONG_MAX paths:
         names don't matter - Must be explicit!  (TG) */
      /* NET_loop_free(net) = TRUE;
     No! We don't know for sure. 1999-08-09 LK*/
      NET_names_matter(net) = FALSE; 
      break;
    default:  /* Count successful */
      NET_loop_free(net) = TRUE;
      NET_names_matter(net) = TRUE;
      break;
    }

  NET_visit_marks_dirty(net) = FALSE;

  return(not_countable);
}


/***************
 *             *
 *   PRUNING   *
 *             *
 ***************/

NETptr prune_fsm (NETptr net, int copy_p)
     /*********
      *
      * TOP LEVEL FUNCTION FOR PRUNING
      *
      * Removes paths leading to non-final states
      * and unreachable states from NET.
      * Non-destructive is COPY_P is DO_COPY, destructive otherwise.
      *
      *********/
{
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  if (NET_pruned(net) == FALSE)
    {
      long arc_count = NET_num_arcs(net); 
      
      /* what about saving sigma??  (see lisp code) */
      prepare_arc_marks(net, 0);
      invert_arcs(net, 1);
      
      /* Remove non-finable states */
      make_connected(net, TRUE);
      invert_arcs(net, 0);
      
      /* other states might also be disconnected */
      make_connected(net, FALSE); 
      
      /* pruned machine with no arcs (NULLFSM or EPSILONFSM)
         has an unspecified alphabet */
      if (STATE_arc_set(NET_start_state(net)) == NULL)
	{
          if (ALPH_len(NET_labels(net)) != 0)
	    ALPH_len(NET_labels(net)) = 0;
        }
      else if (NET_num_arcs(net) < arc_count)
        /* Some arcs were lost, need to make a new inventory of labels. */
        label_check(net);
    }
  
  return (net);
}

/***********************
 *                     *
 *   EPSILON REMOVAL   *
 *                     *
 ***********************/

#define epsilon_arc_p(X) (ARC_label((X)) == EPSILON)

static void mark_epsilon_closure(STATEptr state, NETptr net)
     /****
      *
      * Recursively walks through and marks the states in the epsilon closure
      * of STATE, preparing to copy the arcs at those states back to S.  It
      * builds chains of (copies) of those arcs in their destination states'
      * client cells to make it relatively quick to filter duplicate arcs.
      *
      *****/
{
  ARCptr arc, arc2, new_arc;
  STATEptr destination;
  id_type label;
  int dup_found;
#ifdef FULL_FSM
  void *user_pointer;
#endif

  /* Turn on the first bit so that we do not loop. */

  STATE_visit_mark(state) = STATE_visit_mark(state) | 1;

  for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
    {
      destination = ARC_destination(arc);
      label = ARC_label(arc);
#ifdef FULL_FSM
      user_pointer = ARC_user_pointer(arc);
#endif
      if (epsilon_arc_p(arc))
        {
          if (!(STATE_visit_mark(destination) & 1))
            mark_epsilon_closure(destination, net);
        }
      else
        {
          dup_found = FALSE;
          for (arc2 = (ARCptr) STATE_client_cell(destination);
               arc2 != NULL;
               arc2 = next_arc(arc2))
            {
              if (ARC_label(arc2) == label
#ifdef FULL_FSM
                  && ARC_user_pointer(arc2) == user_pointer
#endif
                  )
                {
                  dup_found = TRUE;
                  break;
                }
            }
          if (!dup_found)
            {
              new_arc = make_arc(label, destination, net);
              next_arc(new_arc) = (ARCptr) STATE_client_cell(destination);
              STATE_client_cell(destination) = (void *) new_arc;
            }
        }
    }
}

static ARCptr LAST;

static ARCptr collect_arcs(STATEptr state, STATEptr start_state)
     /*****
      *
      * Recursively walks through the epsilon closure of START_STATE,
      * unmarking the states, collecting the arcs, and clearing
      * the client_cells.
      * START_STATE is passed down to be marked as final when there is a final
      * state in the epsilon closure.  Returns the first arc in the chain.
      * The last arc in the chain is bound to the global variable LAST.
      *
      *****/
{
  ARCptr first_arc, last_arc, arc, arc2, first;
  STATEptr destination;

  if (STATE_visit_mark(state) & 1)
    {
        /* Turn off the low bit but keep the second bit if the state
         has one so that we do not lose the information about whether
         the state has incoming non-epsilon arcs. */

      STATE_visit_mark(state) = STATE_visit_mark(state) & 2;

      if (STATE_final(state) == TRUE)
        STATE_final(start_state) = TRUE;

      first_arc = NULL;
      last_arc = NULL;

      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
        {
          if (epsilon_arc_p(arc))
            {
              first = collect_arcs(ARC_destination(arc), start_state);
              if (first != NULL)
                {
                  if (last_arc != NULL)
                    next_arc(last_arc) = first;
                  else
                    first_arc = first;

                  last_arc = LAST;
                }
            }
          else
            {
              destination = ARC_destination(arc);
              if (STATE_client_cell(destination) != NULL)
                {
                  if (last_arc != NULL)
                    next_arc(last_arc) =
                      (ARCptr) STATE_client_cell(destination);
                  else
                    first_arc = (ARCptr) STATE_client_cell(destination);

                  for (arc2 = (ARCptr) STATE_client_cell(destination);
                       arc2 != NULL;
                       arc2 = next_arc(arc2))
                    last_arc = arc2;

                  STATE_client_cell(destination) = NULL;
                }
            }
        }
      LAST = last_arc;
      return (first_arc);
    }
  else
    {
      LAST = NULL;
      return (NULL);
    }
}

NETptr epsilon_remove (NETptr net, int copy_p)
     /*****
      *
      * TOP LEVEL FUNCTION FOR EPSILON REMOVAL
      *
      * EPSILON-REMOVE removes epsilons from NETWORK or from its copy
      * if COPY-P is DO_COPY.
      *
      *****/
{
    int flg, epsilon_arc_found;
    STATEptr state;
    ARCptr arc, next, new_arcs;

    if (copy_p == DO_COPY)
        net = copy_fsm(net);

    if (!NET_epsilon_free(net))
    {
        epsilon_arc_found = FALSE;
        prepare_visit_marks(net, 0);

        /* Leave states that have only incoming epsilon arcs
           unmarked, other states get the second bit of the
           visit mark turned on. The start state gets a mark
           regardless of what kind of incoming arcs it has, if any.
           */

        for (state = NET_states(net); state; state = next_state(state))
            for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
                if (epsilon_arc_p(arc))
                    epsilon_arc_found = TRUE;
                else
                    STATE_visit_mark(ARC_destination(arc)) = 2;
    
        NET_visit_marks_dirty(net) = TRUE;

        if (!(epsilon_arc_found))
        {
            NET_epsilon_free(net) = TRUE;
            return(net);
        }

        clear_client_cells(net);
        /* should names (client cells) be saved?  see lisp code */

        flg = FALSE;
        STATE_visit_mark(NET_start_state(net)) = 2;

        /* We don't care about states that only have incoming epsilon
           arcs since they eventually get eliminated.
           */

        for (state = NET_states(net); state; state = next_state(state))
            if (STATE_visit_mark(state) & 2)
            {
                epsilon_arc_found = FALSE;
                for (arc = STATE_arc_set(state);
                     arc != NULL;
                     arc = next_arc(arc))
                {
                    if (epsilon_arc_p(arc))
                    {
                        epsilon_arc_found = TRUE;
                        break;
                    }
                }
                if (epsilon_arc_found == TRUE)
                {
                    mark_epsilon_closure(state, net);
                    new_arcs = collect_arcs(state, state);
                    for (arc = STATE_arc_set(state); arc != NULL; arc = next)
                    {
                        next = next_arc(arc);
                        free_arc(arc);
                        NET_num_arcs(net)--;
                    }
                    STATE_arc_set(state) = new_arcs;
                    STATE_deterministic(state) = FALSE;
                    flg = TRUE;
                }
            }

        /* what about restoring state names (client cells) ?? See Lisp code */

        if (flg == TRUE)
            /* states entered only by epsilon arcs are no longer accessible */
            make_connected(net, FALSE);

        NET_deterministic(net) = FALSE;
        NET_epsilon_free(net) = TRUE;
    }

    return (net);
}

/***********************
 *                     *
 *   DETERMINIZATION   *
 *                     *
 ***********************/
STACKptr STP = NULL;
CONSptr NEWSTATES = NULL;
CONSptr *list_dest_array = NULL;
CONSptr *pending = NULL;
STATEptr *atom_dest_array = NULL;
NETptr det_net;
ARCptr new_arcs, last_arc;

static CONSptr add_state_in_order (STATEptr state, CONSptr state_list)
     /*****
       
       DETERMINIZE-ADDSTATEINORDER splices STATE into STATE_LIST just before
       the first  state ordered after it, and returns the modified STATE_LIST
       
       In C, the ordering is by absolute address (increasing). 
       
       *****/
{
  CONSptr prev, rest, cur;
  
  /* search for insertion point */
  for (prev = NULL, cur = state_list;
       cur != NULL; 
       prev = cur, cur = cdr(cur))
    {
      if (car(cur) == state)
        /* state is already there, so quit */
        return (state_list);
      
      if ((STATEptr) car(cur) > state)
        /* found the insertion point */
        break;
    }
  
  if (prev == NULL)
    /* here, state is first in list */
    state_list = cons(state, state_list);  
  else
    {
      /* else, splice in after prev */
      rest = cons(state, cdr(prev));
      rplacd(prev, rest);
    }   
  
  return (state_list);
}

static CONSptr ordered_union (CONSptr new, CONSptr prior)
{       
  for (; new != NULL; new = cdr(new))
    prior = add_state_in_order(car(new), prior);
  
  return (prior);
}

typedef struct INDEX {
  int len;
  CONSptr comp_states;
  struct INDEX *next;
} INDEXtype, *INDEXptr;

#define INDEX_len(X) (X)->len
#define INDEX_comp_states(X) (X)->comp_states
#define INDEX_next(X) (X)->next
  
  /** For better memory management, INDEXtype now has its own heap.  
   *** %%%TSY 12/7/92 
   **/
  HEAPptr INDEXheap = NULL;

static int identical_or_empty(STATEptr state, CONSptr state_list)
{
  /* Returns TRUE if (1) STATE is a member of STATE_LIST and
     (2) the other states are (a) empty and (b) non-final when
     STATE itself is non-final.
 */

  CONSptr tail;
  int all_non_final_p = TRUE;
  int member_p = FALSE;
  STATEptr dest;

  for (tail = state_list; tail; tail = cdr(tail))
    {
      dest = (STATEptr) car(tail);

      if (dest == state)
	member_p = TRUE;
      else
	{
	  if (STATE_arc_set(dest))
	    return(FALSE);
	  else if (STATE_final(dest))
	    all_non_final_p = FALSE;
	}
    }

  return(member_p && (STATE_final(state) || all_non_final_p));
}

static STATEptr determinize_get_state (CONSptr state_list)
     /*****
       
       ;;; DETERMINIZE-GETSTATE Searches index kept in the client cell of first
       ;;; STATE-LIST  state (an original state) for a state
       ;;; with STATE-LIST.  Secondary index is by the number of states in
       ;;; STATE-LIST. This means that we  don't have to worry about
       ;;; mismatched lengths in the loop below.  Adds a newstate
       ;;; named STATE-LIST to the index if one isn't found.  Assumes that
       ;;; names of original states are initially NULL. 
       
       *****/
{
  CONSptr comp_tail, comp_states = NULL;
  INDEXptr index, ind, prev;
  int len;
  STATEptr head, state;
  
  /* set len to number of states in name_tail */
  len = list_len(state_list);
  
  head = (STATEptr) car(state_list);
  index = (INDEXptr) STATE_client_cell(head);
  
  for (prev = NULL, ind = index;
       ind != NULL;
       prev = ind, ind = INDEX_next(ind))
    {
      if (len == INDEX_len(ind))
        {
          comp_states = INDEX_comp_states(ind);
          /* found set of lists of length len */
          break;
        }
      else if (len > INDEX_len(ind))
        {
          comp_states = NULL;
          break;
          /* failed to find a match */
        }
    }
  
  if (comp_states == NULL)      /* first instance of length len */
    /* create a new composite state and an index */
    {
      ind = (INDEXptr) cell_alloc(INDEXheap);
      
      if (!ind)
        not_enough_memory("DETERMINIZE_GET_STATE");
      
      INDEX_len(ind) = len;
      if (prev == NULL) /* new index is first in list */
        {
          INDEX_next(ind) = index;
          STATE_client_cell(head) = ind;
        }
      else
        {
          INDEX_next(ind) = INDEX_next(prev);
          INDEX_next(prev) = ind;
        }
      
    }
  else
    {
      /*****
        ;; If comp_states contains a composite state matching
        ;; STATE-LIST return it;  otherwise, create a new
        ;; composite state, push it on the index. 
        *****/
      for (comp_tail = comp_states;
           comp_tail != NULL;
           comp_tail = cdr(comp_tail))
        {
          CONSptr match_tail, state_tail;
          
          /* if the state list in the index cell of a composite
             state matches STATE_LIST we don't need a new composite state.
             */
          
          state = (STATEptr) car(comp_tail);
          
          for (match_tail = STATE_client_cell(state), state_tail = state_list;
               car(match_tail) == car(state_tail);
               match_tail = cdr(match_tail), state_tail = cdr(state_tail))
            /* only test for one because the lists are of the same length */
            if (cdr(match_tail) == NULL)
              /* consumed both lists, so found a match */
              {
                  free_list(state_list);
                  return(state);
              }
        }
    }
 
  /* No match was found. Make a new undeterminized composite state except
     when the state being deteriminized is itself a member of the state list,
     i.e. has a looping arc, and the other states are empty and non-final
     if STATE is non-final.In this special case, we do not need a new
     composite state. The arcs leading to non-empty states can be discarded.
  */

  if (identical_or_empty(state, state_list))
    return(state);
  else
    {
      state = make_state(NULL, det_net);
      STATE_deterministic(state) = FALSE;
      STATE_client_cell(state) = state_list;
      STATE_visit_mark(state) = 2;
      NEWSTATES = cons(state, NEWSTATES);
      INDEX_comp_states(ind) = cons(state, comp_states);
      return (state);
    }
}

static STATEptr determinize_doarcs(STATEptr old_state, int copy_arc_p)
{
  ARCptr arc, next, new_arc;
  STATEptr arc_dest, atom_label_dest;
  CONSptr list_label_dest;
  id_type arc_label;
  
  for (arc = STATE_arc_set(old_state); arc != NULL; arc = next)
    {
      arc_label = ARC_label(arc);
      arc_dest = ARC_destination(arc);
      next = next_arc(arc);
      
      list_label_dest = list_dest_array[arc_label];
      atom_label_dest = atom_dest_array[arc_label];
      
      /******
        ;; add the arc's destination to the collection of destinations
        ;; for the arc label
        ******/
      if (list_label_dest == NULL && atom_label_dest == NULL)
        {
          /******
            ;; first instance of label in STATE
            ;; Create new arc for this label, preserving user pointer
            ;; Leave the destination field empty until all the arcs
            ;; have been processed
            ******/
          if (copy_arc_p == TRUE)
            {
              new_arc = make_arc(arc_label, NULL, det_net);
#ifdef FULL_FSM
              ARC_user_pointer(new_arc) = ARC_user_pointer(arc);
#endif
              next_arc(new_arc) = new_arcs;
            }
          else
            new_arc = arc;
          
          if (last_arc != NULL)
            next_arc(last_arc) = new_arc;
          else
            new_arcs = new_arc;
          
          last_arc = new_arc;
          atom_dest_array[arc_label] = arc_dest;
        }
      else if (atom_dest_array[arc_label] == arc_dest)
        {
          /* duplicate of a previous arc, subtract from count
             and free, unless we are just collecting copies.
             */
          if (copy_arc_p == FALSE)
            {
              NET_num_arcs(det_net)--;
              free_arc(arc);
            }
        }
      else
        {
          /* label seen before, subtract from count and free
             unless we are just collecting copies.
             */
          if (copy_arc_p == FALSE)
            {
              NET_num_arcs(det_net)--;
              free_arc(arc);
            }
          
          if (list_label_dest == NULL)
            /* second instance of label in STATE */
            {
              STATEptr first_state;
              
              first_state = atom_dest_array[arc_label];
              
              switch (STATE_visit_mark(first_state))
                {
                case (2):               
                  /*****
                    ;; new composite state, get the
                    ;; corresponding original states from
                    ;; the state client cell.
                    *****/
                  list_label_dest =
                    copy_list((CONSptr) STATE_client_cell(first_state));
                  break;
                default:                /* original state */
                  list_label_dest = cons(first_state, NULL);
                  break;
                }
              atom_dest_array[arc_label] = NULL;
            }
          
          switch (STATE_visit_mark(arc_dest))
            {
            case (2):
              list_dest_array[arc_label] =
                ordered_union(STATE_client_cell(arc_dest), list_label_dest);
              break;
            default:
              list_dest_array[arc_label] =
                add_state_in_order(arc_dest, list_label_dest);
              break;
            }
        }
    }
  return(old_state);
}

static STATEptr determinize_fsm_aux(STATEptr state, int depth)
     /*****
       States with VISIT_MARK not 2 are original nondeterministic states
       that are being kept.  New composite-states are created with
       VISIT-MARK=2, their STATE_client_cell contains a list of
       the correspoding original states.
       *****/
{
  ARCptr arc;
  
  depth--;
  if (depth == 0)
    {
      push(state, STP);  /* defer recursion */
      return (state);
    }
  
  if (STATE_deterministic(state))
    {
      /*****
        VM=1 means original unprocessed state, have to process
        destinations.  0 means original but processed, 2 means composite
        already processed  cause already deterministic.
        Only process if VM=1.
        *****/
      if (STATE_visit_mark(state) == 1)
        {
          STATE_visit_mark(state) = 0;
          for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
            determinize_fsm_aux(ARC_destination(arc), depth);
        }
      return (state);
    }
  
  /*****
    ;; VM=0 means original non-deterministic state in process
    ;; VM=1 means original non-deterministic unprocessed state
    ;; VM=2 means an unprocessed composite state
    *****/
  if (STATE_visit_mark(state) != 0)
    {
      new_arcs = NULL;
      last_arc = NULL;
      
      if (STATE_visit_mark(state) == 2)
        {
          /*****
            ;; state is a new composite state
            ;; collect arcs from the corresponding original
            ;; states.
            *****/
          CONSptr old_states ;
          STATEptr old_state;
          
          for (old_states = (CONSptr) STATE_client_cell(state);
               old_states != NULL;
               old_states = cdr(old_states))
            {
              old_state = (STATEptr) car(old_states);
              /*****
                ;; a composite state is final if at
                ;; least one of the states it represents is final
                *****/
              if (STATE_final(old_state))
                STATE_final(state) = TRUE;
              determinize_doarcs(old_state, TRUE);
            }
        }
      else              /* VM=1 */
        {
          STATE_visit_mark(state) = 0;
          /* Nothing to do unless there are at least two arcs */
          if (STATE_arc_set(state) == NULL)
            {
              STATE_deterministic(state) = TRUE;
              return(state);
            }
          else if (next_arc(STATE_arc_set(state)) == NULL)
            {
              STATE_deterministic(state) = TRUE;
              determinize_fsm_aux(ARC_destination(STATE_arc_set(state)),
                                  depth);
              return(state);    
            }
          else
            determinize_doarcs(state, FALSE);
        }
      
      /*  Clear the next-arc field of LAST_ARC to make sure it doesn't point
          to anything -- at least one new arc must have been picked up
          in DETERMINIZE-DOARCS
          */
      if (last_arc != NULL)
        next_arc(last_arc) = NULL;
      
      /*****
        ;; convert the cells in the destination array to states and
        ;; clear the array, add destination state to all the new arcs
        ;; created for the state
        *****/
      for (arc = new_arcs; arc != NULL; arc = next_arc(arc))
        {
          id_type label;
          CONSptr label_dest;
          
          label = ARC_label(arc);
          label_dest = list_dest_array[label];
          if (label_dest != NULL)
            {
              ARC_destination(arc) = determinize_get_state(label_dest);
              list_dest_array[label] = NULL;
            }
          else
            {
              ARC_destination(arc) = atom_dest_array[label];
              atom_dest_array[label] = NULL;
            }
        }
      /*****
        ;; replace the old arc set of the state with the new one and mark
        ;; the state as deterministic.
        ;; adjust the arc count appropriately
        *****/
      
      STATE_arc_set(state) = new_arcs;
      STATE_deterministic(state) = TRUE;
      /*****
        ;; Process new destination states now when the array is clear
        *****/
      for (arc = new_arcs; arc != NULL; arc = next_arc(arc))
        determinize_fsm_aux(ARC_destination(arc), depth);
    }
  return (state);
}

static int db_keep_pred(STATEptr state)
     /*****
      *
      * Passed as an argument to delete_states()
      * in determinize_basic(), below.
      *
      *****/
{
  if (STATE_visit_mark(state) == 0)
    {
      STATE_client_cell(state) = NULL;
      return (TRUE);
    }
  else return (FALSE);
}

int determinize_basic (NETptr net)
     /*****
       ;;
       ;; Assumes that the arc labels are small integers drawn from a
       ;; dense set and that the network is epsilon free
       ;;
       *****/
{
  STATEptr state;
  CONSptr temp;


  if (NET_optimized(net))
  {
      NET_deterministic(net)= FALSE;
#ifdef DEBUG
      handle_warning("Can't determinize an optimized network.\n",
		     "determinize_basic", 0);
#endif
      return(ERROR);
  }
  
  det_net = net;
  prepare_visit_marks(net, 1);
  
  /*****
    ;; Original states have visitmark of 1 initially, 0 if they've
    ;; been kept in result.  New composite states are created with
    ;; VISITMARK=2 
    *****/
  
  NET_sorted_states(net) = FALSE;
  
  /*****
    ;; Prepare to build index in client cell.  Don't bother saving
    ;; names, since they become meaningless as state identities
    ;; change 
    *****/
  clear_client_cells(net);
  
  STP = create_stack("States to determinize");
  INDEXheap = init_heap(sizeof(INDEXtype), 16384/sizeof(INDEXtype),
			"Determinizer indices");
  NEWSTATES = NULL;
  atom_dest_array = (STATEptr *) calloc((size_t) ALPH_len(NET_labels(net)),
                                        (size_t) sizeof(STATEptr));
  list_dest_array = (CONSptr *) calloc((size_t) ALPH_len(NET_labels(net)),
                                       (size_t) sizeof(CONSptr));
  
  /* N.B. Metrowerks compiler returns a NULL pointer for calloc(0)
     whereas gcc returns a pointer to real memory if it can.
     Have
     */

  if (!(atom_dest_array && list_dest_array) && ALPH_len(NET_labels(net)) > 0)
    not_enough_memory("DETERMINIZE");
  
  /*****
    ;; STP contains states to process. States are processed
    ;; recursive down  to a fixed depth, then pushed on STP.
    ;; This is to avoid stack  overflows on machines with long
    ;; linear chains. New composite states are pushed to NEWSTATES
    ;; when they are created.
    *****/
  
  determinize_fsm_aux (NET_start_state(net), 20);
  
  for (state = pop(STP); state != NULL; state = pop(STP))
    {
      determinize_fsm_aux(state, 20);
    } 
  
  for (temp = NEWSTATES; temp != NULL; temp = cdr(temp))
    {
      state = car(temp);
      STATE_visit_mark(state) = 0;
      free_list((CONSptr) STATE_client_cell(state));
    }
  /*****
    ;; Now flush states that were not kept.
    *****/
  NET_states(net) = delete_states(net, db_keep_pred);
  NET_visit_marks_dirty(net) = FALSE;
  NET_deterministic(net) = TRUE;
  
  free((void *) atom_dest_array);
  atom_dest_array = NULL;
  free((void *)list_dest_array);
  list_dest_array = NULL;
  free_stack(STP);
  STP = NULL;
  free_heap(INDEXheap);
  INDEXheap = NULL;
  return (NO_ERROR);
} /* determinize_basic */

NETptr determinize_fsm(NETptr net, int copy_p)
     /***** TOP LEVELFUNCTION FOR DETERMINIZER
       DETERMINIZE determinizes n.  COPY-P=DONT_COPY means smash,
       DO_COPY means guarantee a copy even if there are no changes,
       COPY_IF_ALTERED means guarantee a copy only when there is
       work to be done.
       *****/
     
{
    
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  if (!(NET_deterministic(net) && NET_epsilon_free(net)))
    {

        if (NET_optimized(net) || possibly_already_optimized_p(net))
        {
            handle_warning("Can't determinize an optimized network.\n",
		     "determinize_fsm", 0);
            fsm_fprintf(stdout, "Un-optimizing ...\n");

            do_unoptimize_arcs(net);

            fsm_fprintf(stdout, "Determimizing ...\n");
        }
        
      if (copy_p == COPY_IF_ALTERED)
        net = copy_fsm(net);
      
      epsilon_remove(net, FALSE);
      
      /* If the number of arcs is less than two, the net
	 is deterministic by definition.
	 */

      if (NET_num_arcs(net) < 2)
	{
	   NET_deterministic(net) = TRUE;
	   STATE_deterministic(NET_start_state(net)) = TRUE;
	   /* Added the necessary line above to mark the start state
	      as well. 1998-11-14 LK */
	 }
      else
	{
	  init_cons_mem();
	  renumber_net(net);
      
	  determinize_basic(net);
      
	  renumber_arc_labels(net, NET_labels(net));
	  free_cons_mem();
	}

      /* If the net's arcs were previously sorted, this arc ordering is lost. */
      NET_sorted_arcs(net) = 0;      
    }
  return (net);
} /* determinize_fsm */

/********************
 *                  *
 *   MINIMIZATION   *
 *                  *
 ********************/
static void update_pending(CONSptr *pending, DBLptr newclass)
     /*****
      *
      * UPDATE_PENDING puts NEWCLASS on the pending list for a symbol if
      * it has an arc with that symbol.  NEWCLASS is always chosen so that
      * it is the smaller of the split classes.  Therefore we don't have to
      * do any testing to see which class to use.
      *
      *****/
{
  DBLptr temp;
  ARCptr cur_arc;
  CONSptr p;
  
  for (temp = newclass; temp != NULL; temp = DBL_cdr(temp))
    for (cur_arc = STATE_arc_set(DBL_state(temp)); 
         cur_arc != NULL; 
         cur_arc = next_arc(cur_arc))
      {
        p = pending[ARC_label(cur_arc)];
        
        if (p == NULL || car(p) != newclass)
          pending[ARC_label(cur_arc)] = cons(newclass, p);
      }
}

static void split_class (DBLptr class, CONSptr *pending)
     /*******
      *
      * Part of MIN_FSM:  Splits an equivalence class
      * into marked and unmarked states 
      *
      *******/
{
  DBLptr prior, statelst, temp;
  DBLptr newclass = NULL, listhead = NULL,listtail = NULL;
  STATEptr state;
  
  prior = class;
  
  /*
   *  Move all unmarked states to NEWCLASSSTATES.  Reuse the CONS cells.
   *  There is guaranteed to be at least one marked state, although
   *  there may not be any unmarked states.
   */
  
  for (statelst = DBL_cdr(class); ; statelst = DBL_cdr(statelst))
    {
      if (statelst == NULL)
        {
          /* clean up and quit the loop */
          if (listhead != NULL)
            {
              DBL_cdr(prior) = NULL;
              DBL_cdr(listtail) = newclass;
              DBL_back_link(listhead) = NULL;
              
              if (newclass != NULL)
                DBL_back_link(newclass) = listtail;                     
              
              newclass = listhead;
              listhead = NULL;
            }
          break;
        }
      
      state = DBL_state(statelst);
      switch (STATE_visit_mark(state))
        {
        case (NOT_VISITED):     /* move onto NEWCLASS */
          if (listhead == NULL)
            listhead = statelst;
          listtail = statelst;
          break;
        default:
          STATE_visit_mark(state) = NOT_VISITED;
          if (listhead != NULL)
            {
              DBL_cdr(prior) = statelst;
              DBL_back_link(statelst) = prior;                  
              DBL_cdr(listtail) = newclass;
              if (newclass != NULL)
                DBL_back_link(newclass) = listtail;             
              DBL_back_link(listhead) = NULL;                           
              newclass = listhead;
              listhead = NULL;
            }
          prior = statelst;
          break;
        }       
    }
  
  /*
   *  Do the first state last because it represents the class and
   *  has to stay at the head of the list.  (There are lots of
   * references to the first item in a class.)
   */
  
  switch(STATE_visit_mark(DBL_state(class)))
    {
    case (NOT_VISITED):
      statelst = DBL_cdr(class);
      DBL_cdr(class) = newclass;
      if (newclass != NULL)
        DBL_back_link(newclass) = class;                                
      
      newclass = statelst;
      DBL_back_link(statelst) = NULL;
      break;
    default:
      STATE_visit_mark(DBL_state(class)) = NOT_VISITED;
      break;
    }
  
  /* If there are any new states, create a new class with the
   * smaller number of states.  Then change all of the states to
   * point to that class and update the pending list.
   */
  
  if (newclass != NULL)
    {
      for (temp = newclass; temp != NULL; temp = DBL_cdr(temp))
        DBL_class(temp) = newclass;     
      
      update_pending(pending, newclass);
    }
}

static void process_class(DBLptr class, id_type symbol, CONSptr *pending,
                          CONSptr free_class_list)
     /********
      *
      * Part of MIN_FSM:  Take a class/symbol and look for classes
      * that are split by it.
      *
      ********/
{
  DBLptr link, destclass, temp;
  DBLptr backlink = NULL, nextlink = NULL, newclass = NULL;
  CONSptr equivclasses, lst;
  STATEptr state, dest, tempstate;
  ARCptr arc;
  int circular = FALSE;
  
  equivclasses = free_class_list;
  
  /* 
    Mark all of the states that point to this equivalence class.
    Keep track of the equivalence classes that we have touched. 
    */  
  
  for (temp = class; temp != NULL; temp = DBL_cdr(temp))
    {
      state = DBL_state(temp);
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
        {
          if (symbol == ARC_label(arc))
            {
              dest = ARC_destination(arc);
              link = (DBLptr) STATE_client_cell(dest);
              destclass = DBL_class(link);
              if (destclass == class)
                {
                  STATE_visit_mark(dest) = 1;
                  /* process class later */
                  circular = TRUE;
                }
              else
                {
                  newclass = DBL_back_link(destclass);
                  
                  /* First, remove LINK from its current class */
                  if (link != destclass)                
                    {
                      backlink = DBL_back_link(link);
                      nextlink = DBL_cdr(link);
                      DBL_cdr(backlink) = nextlink;
                      if (nextlink != NULL)
                        DBL_back_link(nextlink) = backlink;
                      /* This one!!! */
                    }
                  else
                    /* LINK represents the class.
                     * Swap the data with the next link,
                     * and use it instead */
                    {
                      link = DBL_cdr(destclass);
                      if (link != NULL) 
                        {
                          /* remove LINK from its class */
                          nextlink = DBL_cdr(link);
                          if (nextlink != NULL)
                            DBL_back_link(nextlink) = destclass;
                          
                          DBL_cdr(destclass) = nextlink;
                          
                          /* Swap the states between the old link
                             and the new link */
                          tempstate = DBL_state(link);
                          DBL_state(destclass) = tempstate;
                          STATE_client_cell(tempstate) = destclass;
                          DBL_state(link) = dest;
                          STATE_client_cell(dest) = link;
                        }
                      else
                        /* The link is the only element in the class,
                           leave it there. */
                        {
                          if (newclass != NULL)
                            {
                              /* We don't want to create a new class, because 
                               * it would produce an empty old class.  Move
                               * all of the links on the new class back
                               * to the old class. 
                               * Eliminates the new class. 
                               */
                              DBL_back_link(destclass) = NULL;          
                              DBL_cdr(destclass) = newclass;
                              DBL_back_link(newclass) = destclass;      
                              while (newclass != NULL)
                                {
                                  DBL_class(newclass) = destclass;
                                  newclass = DBL_cdr(newclass);
                                } 
                            }
                        }
                    }
                  
                  /* Second, add LINK to the new class */
                  if (link != NULL)                     
                    {
                      if (newclass == NULL)
                        {
                          /* create a new class */
                          newclass = link;
                          DBL_cdr(link) = NULL;
                          DBL_back_link(newclass) = destclass;  
                          DBL_back_link(destclass) = newclass;  
                          DBL_class(link) = newclass;
                          
                          rplaca(free_class_list, link);
                          if (cdr(free_class_list) == NULL)
                            rplacd(free_class_list, cons(NULL, NULL));
                          
                          free_class_list = cdr(free_class_list);
                        }
                      else
                        {
                          dbl_splice(newclass, link, DBL_cdr(newclass));
                          DBL_class(link) = newclass;
                        }
                    }
                }
            }
        }
    }
  
  /* Put the classes on PENDING */      
  for (lst = equivclasses; lst != NULL; lst = cdr(lst))
    {
      link = car(lst);
      if (link == NULL)
        break;
      else
        {
          if (link == DBL_class(link))
            {
              temp = DBL_back_link(link);
              if (temp)
              	DBL_back_link(temp) = NULL;                       
              DBL_back_link(link) = NULL;                       
              update_pending(pending, link);
            }
        }
      rplaca(lst, NULL);
    }
  
  if (circular == TRUE)
    split_class(class, pending);
}

static int extract_net_keep_pred(STATEptr state)
{
  if (state == DBL_state(DBL_class((DBLptr) STATE_client_cell(state))))
    {
      STATE_client_cell(state) = NULL;
      return (TRUE);
    }
  else
    return (FALSE);
}

static NETptr extract_network(NETptr net)
     /*****
      *
      * Part of MIN_FSM:  Destructively extracts
      * the new minimized network from NET.
      *
      *****/
{
  STATEptr state;
  ARCptr arc;
  
  NET_start_state(net) = 
    DBL_state(DBL_class((DBLptr) STATE_client_cell(NET_start_state(net))));
  
  /* change pointers to representative states */
  for (state = NET_states(net); state != NULL; state = next_state(state))
    for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
      ARC_destination(arc) = 
        DBL_state(DBL_class((DBLptr) STATE_client_cell(ARC_destination(arc))));
  
  /* remove unnecessary states from the network */
  NET_states(net) = delete_states(net, extract_net_keep_pred);
  
  return (net);
}

static DBLptr min_fsm_init (NETptr net, CONSptr *pending)
     /**********
      *
      * Creates initial final and non-final partitions.
      *
      **********/
{
  STATEptr cur_state;   
  DBLptr finals = NULL, nonfinals = NULL, link;
  long size_difference = 0;
  
  for (cur_state = NET_states(net);
       cur_state != NULL;
       cur_state = next_state(cur_state))
    {           
      link = make_dbl(cur_state);
      if (STATE_final(cur_state) == TRUE)
        {
          if (finals == NULL)
            finals = link;
          else
            dbl_splice(finals, link, DBL_cdr(finals));
          
          DBL_class(link) = finals;
          size_difference++;
        }
      else
        {
          if (nonfinals == NULL)
            nonfinals = link;
          else
            dbl_splice(nonfinals, link, DBL_cdr(nonfinals));
          
          DBL_class(link) = nonfinals;
          size_difference--;
        }
      STATE_client_cell(cur_state) = link;
    }
  
  if (nonfinals == NULL || size_difference < 0)
    {
      update_pending(pending, finals);
      return (nonfinals);
    }
  else
    {
      update_pending(pending, nonfinals);
      return (finals);
    }
}

static int min_fsm_aux(NETptr net)
     /*****
      *
      *  Destructively minimizes NET using Hopcroft's algorithm.
      *
      *****/
{
  DBLptr final_class, class2;
  CONSptr classes, free_class_list;
  int cont, n, error;
  
  if (NET_deterministic(net) == FALSE)
  {
    error = determinize_basic(net);
    if (error != NO_ERROR)
    {
        NET_minimized(net)= FALSE;
        return (ERROR);
    }
  }
  
  init_dbl_mem(NET_num_states(net));
/*  NET_names_smashed(net) = TRUE;        */
  prepare_arc_marks(net, 0);
  if (!NET_pruned(net))
    make_connected(net, FALSE);
  
  /* Invert the arcs so that ARCSET contains delta inverse. */
  invert_arcs(net, DONE);
  
  /* Throw away dead paths */
  if (!NET_pruned(net))
    make_connected(net, TRUE);
  
  /* If only the start state remains, there is nothing further to do */
  if (next_state(NET_states(net)) != NULL)
    {
      free_class_list = cons(NULL, NULL);
      pending = (CONSptr *) calloc((size_t) ALPH_len(NET_labels(net)),
                                   sizeof(CONSptr));
      
      if (!pending)
        not_enough_memory("MINIMIZE");
      
      cont = TRUE;
      
      /*        
       * Split the states into final and non-final equivalence
       * classes.  This can't be done until after the arcs have
       * been inverted. 
       */
      final_class = min_fsm_init(net, pending);
      
      /* Process the list of pending actions until there are none left */
      prepare_visit_marks(net, 0);
      while (cont == TRUE)
        {
          cont = FALSE;                 
          for (n = 0; n < ALPH_len(NET_labels(net)); n++)
            {
              classes = pending[n];
              pending[n] = NULL;
              while (classes != NULL)
                {
                  class2 = car(classes);
                  classes = freeing_CDR(classes);
                  process_class(class2, (uint16)n, pending, free_class_list);
                  cont = TRUE;
                }
            }   
          
          /*
           * Don't process FINALCLASS until everything else
           * has been processed. (FINALCLASS is a huge class
           * at the beginning, but by the end it has  been
           * whittled down to a reasonable size.)
           */
          if (final_class != NULL && cont == FALSE)
            {
              update_pending(pending, final_class);
              final_class = NULL;
              cont = TRUE;
            }
        }       
      /*
       * Restore the arcs to their original direction and
       * extract the  minimized fsm.
       */
      
      invert_arcs(net, NOT_VISITED);
      net = extract_network(net);
      
      /*
       * Minimal machine with no arcs.  NULLFSM or
       * EPSILONFSM has  an unspecified alphabet.
       */
      if (STATE_arc_set(NET_start_state(net)) == NULL)
        ALPH_len(NET_labels(net)) = 0;
    }
  
  free_dbl_mem();

  if (pending)
    {
      free((void *) pending);
      pending = NULL;
    }

  clear_client_cells(net);

  return (NO_ERROR);
} /* min_fsm_aux */

void prune_sigma(NETptr net)
{
  /* Sets the sigma alphabet of the net to the structural
     sigma if the original sigma contains OTHER but OTHER
     does not actually appear in the label set of the net.
     */

  ALPHABETptr sig = label_to_binary(NET_sigma(net));
  ALPHABETptr labs = binary_to_label(NET_labels(net));
  int16  j;
  id_type id;
  int i, lab_len = ALPH_len(labs);
  LABELptr label;

  if (!binary_sigma_member(sig, OTHER))
    return;

  /* Scan the label alphabet for OTHER. */

  for (i = 0; i < lab_len; i++)
    {
      id = ALPH_item(labs, i);
      if (id == OTHER)
	return;
      else
	{
	  label = id_to_label(id);
	  if (LABEL_arity(label) > 1)
	    for (j = 0; j < LABEL_arity(label); j++)
	      if (TUPLE_labels(LABEL_tuple(label))[j] == OTHER)
		return;
	}
    }
  
  /* The sigma alphabet contains OTHER but there are no arcs
     that have OTHER in their label. We reduce SIGMA to the
     structural sigma of the net.
     */

  free_alph(NET_sigma(net));
  NET_sigma(net) = label_sigma(net);
}

NETptr min_fsm(NETptr net, int copy_p)
     /*****
      *
      * TOP LEVEL FUNCTION FOR MINIMIZER
      *
      *  Destructive unless copy_p = DO_COPY
      *
      *****/
{
    int error= NO_ERROR;
    STATEptr state;

    if (NET_optimized(net) || possibly_already_optimized_p(net))
    {
        NET_minimized(net)= FALSE;
#ifdef DEBUG
        handle_warning("Can't minimize an optimized network.\n",
		     "min_fsm", 0);
#endif
        return(net);
    }
    
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  if (!NET_minimized(net))
    {
        
      /* Prune here, to make sure that label set is correctly
         specified.*/
      
      prune_fsm(net, DONT_COPY);
      
      epsilon_remove(net, DONT_COPY);
      
      /* If the number of arcs is less than two, the net is
	 already deterministic and minimal, although it might not
	 yet be marked as such. */

      if (NET_num_arcs(net) < 2)
        {
          NET_deterministic(net) = TRUE;
          /*  STATE_deterministic(NET_start_state(net)) = TRUE;
              Added the necessary line above to mark the start state
             as well. 1998-11-14 LK */
          /* 2003-2-19 All states must be set as deterministic as well TG */
          for (state = NET_states(net); state != NULL; state = next_state(state))
            {
              STATE_deterministic(state) = TRUE;
            }
        }
      else
	{
	  init_cons_mem();
      

          /* renumber only when net hasn't been renumbered by the caller */
          renumber_net(net);
          
	  error = min_fsm_aux(net);
      
	  renumber_arc_labels(net, NET_labels(net));
      
	  free_cons_mem();
	}

      NET_minimized(net) = TRUE;

      /* If the net's arcs were previously sorted, this arc ordering is lost. */
      NET_sorted_arcs(net) = 0;
      
    }
  else if (!NET_pruned(net))
    /*
     * Might be marked minimal but still have a failure 
     * state in its state set, plus arcs that lead only there.
     */
    net = prune_fsm(net, FALSE);
  
  prune_sigma(net);

  return (net);
} /* min_fsm */

#if 0
/*******************
 *
 * Cleanup
 *
 *******************/

/* Utility for replacing pairs of X:EPSILON EPSILON:Y and
   EPSILON:Y X:EPSILON arcs with single X:Y arcs.
   */

static id_type get_label_pair(id_type id1, id_type id2, int id1_side)
{
  /* Returns the label id for the composite label consisting of one
     side of id1 and the opposing side of id2. The previous version
     of this function used a very memory expensive scheme for performing
     this simple task. In order to avoid calling intern_tuple_label()
     twice with the same pair of arguments, the id of the composite label
     was cached into an array whose size was ((max_id1+1)*(max_id2+1)).
     Large temporary arrays can lead to memory fragmentation on Solaris
     machine. For that reason, we abandoned a similar efficiency hack
     previous used in compose_fst(). Now get_label_pair() is as simple
     as it can be. The loss of efficiency, if any, is negligable.
     */

  id_type comp_id;

  if (id1_side == UPPER)
    make_fstlabel(&comp_id, upper_id(id1), lower_id(id2));
  else
    make_fstlabel(&comp_id, upper_id(id2), lower_id(id1));

  return(comp_id);
}

static int eps_label_merge(NETptr net, STATEptr *empty_final, STATEptr state,
                           ARCptr arc1, int arc1_side)

{
    /* Inner loop of CLEANUP_FST.  ARC1 is an arc with EPSILON on one
       side of the label.  The label is X:EPSILON if ARC1_SIDE is UPPER,
       EPSILON:Y if LOWER. If the destination of ARC1 contains only
       arcs with EPSILON on the opposite side and the destination
       state is not final, ARC1 relabeled with a new composite label,
       removing the epsilons, and rerouted to the new destination of ARC2
       and copies of it are inserted for all the other similar labels.
       However, if the destination of ARC1 is final, the original arc must
       be kept as well.
       Returns 1 if ARC1 was rerouted, 0 otherwise.
       */
    STATEptr dest1 = ARC_destination(arc1);
    ARCptr arc2 = STATE_arc_set(dest1);
    LABELptr label2;
    id_type comp_label;
    int final_p = STATE_final(dest1);

    /* Check if all the arcs in the destination state of ARC1
       have a label with an EPSILON on the opposite side.
       */

    for (arc2 = STATE_arc_set(dest1); arc2; arc2 = next_arc(arc2))
    {
        label2 = id_to_label(ARC_label(arc2));
        if (LABEL_arity(label2) != 2 ||
            get_field(label2, arc1_side) != EPSILON)
            return(0);
    }

    /* There is work to be done.

       If the destination of ARC1 is final, reroute it to an empty
       final state with its original label. If the net does not yet
       have any empty final state yet, create it. Then add arcs with
       new composite labels to all ARC2 destinations.
       If the detstination of ARC1 is not final, reroute it to the
       destination of the first ARC2 in the destination of ARC1
       with a new composite label. Then add new arcs to the state for
       the other ARC2s, if any.
       */

    if (final_p)
    {
        if (!(*empty_final))
        {
            *empty_final = make_state(NULL, net);
            STATE_final(*empty_final) = TRUE;
        }

        ARC_destination(arc1) = *empty_final;
    }

    for (arc2 = STATE_arc_set(dest1); arc2; arc2 = next_arc(arc2))
    {
        comp_label = get_label_pair(ARC_label(arc1), ARC_label(arc2),
                                    arc1_side);

        if (!final_p && arc2 == STATE_arc_set(dest1))
        {
            ARC_label(arc1) = comp_label;
            ARC_destination(arc1) = ARC_destination(arc2);
        }
        else
            push_arc(make_arc(comp_label, ARC_destination(arc2), net),
                     state);
    }
    return(1);
}

NETptr cleanup_fst(NETptr net, int copy_p, int minimize_p)
{
  /* Replaces pairs of X:EPSILON EPSILON:Y and pairs of EPSILON:Y X:EPSILON
     arcs with single X:Y arcs.
     */
  STATEptr state, empty_final = NULL;
  ARCptr arc;
  LABELptr label;

  int state_changed_p = FALSE, net_changed_p = FALSE;
  
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  /* If the net is a simple automaton, cleanup does nothing. */

  if (NET_arc_label_arity(net) == 1)
     {
         if (minimize_p)
             min_fsm(net, DONT_COPY);
         return(net);
     }

  /* The result of cleanup seems to be in general better when the
     states are ordered in an accessibility order from the start
     state. Don't leave the state names in client cells. Someone
     may depend on them being clean.
   */

  rename_states(net);
  reorder_states(net, state_num_less_p);
  clear_client_cells(net);

  init_cons_mem();
  prepare_arc_marks(net, NOT_VISITED);
  
  /* Check if the network has an empty final state to use in
     eps_label_merge.
     */

  for (state = NET_states(net); state != NULL; state = next_state(state))
      if (STATE_final(state) && !(STATE_arc_set(state)))
      {
          empty_final = state;
          break;
      }

  for (state = NET_states(net); state != NULL; state = next_state(state))
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
      {
          label = id_to_label(ARC_label(arc));
          if (LABEL_arity(label) == 2)
        {
              if (get_field(label, LOWER) == EPSILON)
        state_changed_p = eps_label_merge(net, &empty_final, state,
                          arc, UPPER);
              else if (get_field(label, UPPER) == EPSILON)
        state_changed_p = eps_label_merge(net, &empty_final, state,
                          arc, LOWER);
        }

          if (state_changed_p)
          {
              STATE_deterministic(state) = FALSE;
              net_changed_p = TRUE;
          }
      }
      
  free_cons_mem();
  
  if (net_changed_p)
    {
      /* Although the parse tables, if any, are still valid,
         we have to release them because the associated MATCH_TABLE
         in the look routine has to be changed to accommodate the
         new label set. The only sure way to make it happen is to
         reclaim the parse tables and set the pointers to NULL.
         */

      free_parse_table(NET_upper_parse_table(net));
      NET_upper_parse_table(net) = NULL;
      free_parse_table(NET_lower_parse_table(net));
      NET_lower_parse_table(net) = NULL;

      update_label_alphabet(net);
      NET_pruned(net) = FALSE;
      NET_deterministic(net) = FALSE;
      NET_minimized(net) = FALSE;
    }
  
  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);

  return(net);
}

void sever_states(NETptr net)
     /****
      *
      * Replaces join_nets().   This function releases the connection
      * beetween NET and it's state (& arc) memory.  This ensures that
      * states & arcs are not randomly nuked when NET gets freed.  This 
      * is crucial if those structures have been spliced into some other 
      * net.  If sever_states() is not called, then other nets containing
      * structures from NET will be corrupted.
      *
      ****/
{
  NET_states(net) = NULL;
  NET_start_state(net) = NULL;
}

#endif /* 0 */

void recover_temp_memory(void)
{
  /* Called from run_commands() in fst.c when the user aborts with
     a control-C. As this may happen any time, it is not clear that
     we can always recover all memory that has been allocated.
     However, we try to make the best of the situation.
     Some of the maintenance functions, such as determinization and
     minimization use static variables to point to allocated memory.
     This function is located here because the headers of these
     variables STP, etc are declared above. It is rather
     awful to do things this way but there is easy fix to past
     bad programming practices.
  */

  fsm_fprintf(stdout, "Resuming. Operands of any stack-");
  fsm_fprintf(stdout, "modifying operation will be lost.\n");

  /* determinize_basic() variables */

  if (atom_dest_array)
    free((void *) atom_dest_array);

  atom_dest_array = NULL;

  if (list_dest_array)
    free((void *)list_dest_array);

  list_dest_array = NULL;

  if (STP)
    free_stack(STP);

  STP = NULL;

  if (INDEXheap)
    free_heap(INDEXheap);

  INDEXheap = NULL;

  free_cons_mem();  /* determinization, minimization
               Frees CONS_HEAP if it is non-NULL
               and resets the variable to NULL */

  /* min_fsm_aux() variables */

  if (pending)
    free((void *) pending);

  pending = NULL;

  free_dbl_mem();  /* Frees DBLheap if it is non-NULL
              and resets the variable to NULL */
}

#if 0

/*********************************
 * 
 *  extra function for NET_VECTOR
 *
 *********************************/
NVptr copy_nv_and_nets(NVptr old)
     /*****
      *
      * Copies both the NV wrapper and the networks it
      * points to.
      *
      *****/
{
  NVptr new;
  int i;
  
  new = make_nv(NV_len(old));
  
  for (i = 0; i < NV_len(new); i++)
    NV_net(new, i) = copy_fsm(NV_net(old, i));
  
  return (new);
}





void rmap_net(NETptr net, int pre_state_fn(STATEptr), int arc_fn(ARCptr), 
              int post_state_fn(STATEptr), STACKptr stack)
/***********
 *
 * Recursively maps over the states of the network as long as PRE_STATE_FN,
 * returns TRUE applying ARC_FN to every arc of the state and POST_STATE_FN
 * to the state after all its arcs have been processed.
 * If STACK is null, a local stack is created and released
 * at the end.  STACK contains the chain of arcs leading to CUR_STATE.
 *
 ************/
{       
  ARCptr cur_arc = NULL;
  STATEptr cur_state;
  int free_stack_p = TRUE;
  
  if (stack == NULL)
    stack = create_stack("States to rmap");
  else
    free_stack_p = FALSE;
  
  cur_state = NET_start_state(net);
  
  for (;;)
    {
      /* If PRE_STATE_FN returns FALSE, stop recursion,
         otherwise start processing the arcs. */
      
      if (pre_state_fn(cur_state))
        cur_arc = STATE_arc_set(cur_state);
      
      while (cur_arc == NULL)
        {
          /* No arcs. Backtrack to a previously
             processed arc that has an unprocessed sister
             applying POST_STATE_FN to the destination
             states of the arcs that are removed from the stack.
             */
          if (STACK_depth(stack) > 0)
            {
              cur_arc = (ARCptr) pop(stack);
              /* The destination state has been fully processed.
                 apply POST_STATE_FN to it.
                 */
              post_state_fn(ARC_destination(cur_arc));
              cur_arc = next_arc(cur_arc);
            }
          else
            break;
        }               
      if (cur_arc == NULL)
        /* We are done. */
        break;
      else
        {
          /* Apply ARC_FN to the current arc and recurse
             on its destination state. */
          
          arc_fn(cur_arc);
          
          push(cur_arc, stack);
          
          cur_state = ARC_destination(cur_arc);
          cur_arc = NULL;
        }
    }
  post_state_fn(NET_start_state(net));
  if (free_stack_p)
    free_stack(stack);
}


/*********************************
 *
 *  LOCAL EPSILON PUSHING
 *
 *  This "cleaning function" pushes all epsilons on both sides of
 *  a transducer as far as possible.
 *  First all "untouchable" states are detected and paired with a twin
 *  state in the new network.
 *  Then all sections of the old net - delimited by "untouchable" states -
 *  are copied to the new net. These section copies are cleaned separately
 *  before being incorporated into the new net.
 *  In this way identical paths that are only destinguised by different
 *  epsilon locations, become visible to the determinization algorithm.
 *  Finaly the new net is determinized and minimized.
 *
 *  As implemented here, this function is very simple.
 *  It can probably also be done in the MORE GENERAL FRAMEWORK OF
 *  SEQUENTIALIZATION, and may later be replaced by that.
 *
 *  AK, 26/10/98
 *
 **********************************/


static void LEP_mark_untouchable_states (NETptr net)
{
  /* Marks all "untouchable" states by LEP_STATE_count (state) = 2.
     These states are:
     - initial state
     - final states
     - states with two or more incomming arcs
     - source and destination states of arcs labeled with (identity) OTHER,
         i.e.  ?  but not ?:?  ?:a  a:?
     */

  STATEptr state, dest;  ARCptr arc;

  /* set all state counters to zero */

  for (state = NET_states (net);  state;  state = next_state (state))
    LEP_STATE_count (state) = 0;

  /* mark all "untouchable" states by state counter value 2 */

  LEP_STATE_count (NET_start_state (net)) = 2;   /* initial state */

  for (state = NET_states (net);  state;  state = next_state (state))
    {
      if (STATE_final (state))    /* final state */
    LEP_STATE_count (state) = 2;

      for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
    {
      dest = ARC_destination (arc);

      if (LEP_STATE_count (dest) < 2)
        LEP_STATE_count (dest) ++;   /* count incomming arcs up to 2 */

      if (ARC_label (arc) == OTHER)
        {
          LEP_STATE_count (state) = 2;  /* source and ... */
          LEP_STATE_count (dest) = 2;   /* ... destination of OTHER */
        }
    }
    }
}

static NETptr LEP_make_twin_states (NETptr oldnet)
{
  /* Pairs "untouchable" states with twin states. */

  NETptr newnet;  STATEptr state, twin;

  newnet = null_fsm ();

  /* handle initial state */

  state = NET_start_state (oldnet);
  LEP_STATE_twin (state) = twin = NET_start_state (newnet);
  LEP_STATE_twin (twin) = state;
  STATE_final (twin) = (STATE_final (state));

  /* handle all other states */

  for (state = NET_states (oldnet);  state;  state = next_state (state))
    if (state != NET_start_state (oldnet))
      {
    if (LEP_STATE_count (state) == 2)  /* "untouchable" state */
      {
        LEP_STATE_twin (state) = twin = make_state (NULL, newnet);
        LEP_STATE_twin (twin) = state;
        STATE_final (twin) = (STATE_final (state));
      }
    else                    /* ordinary state */
      LEP_STATE_twin (state) = NULL;
      }

  return (newnet);
}

/*-----------------------------------------------------------*/
static void LEP_add_id_to_seq (ALPHABETptr alph, id_type id)
{
  if (id == EPSILON)
    return;   /* no not add EPSILON */

  if (ALPH_len (alph) >= ALPH_max (alph))
    adjust_alph_size (alph, ALPH_len (alph) + 1);  /* adjust size */

  ALPH_item (alph, ALPH_len (alph)) = id;
  ALPH_len (alph) ++;
}

static void LEP_copy_section
  (NETptr newnet, STATEptr end, STATEptr beg,
   ALPHABETptr upper, ALPHABETptr lower, uint16 upstart, uint16 lowstart)
{
  ARCptr arc;  
  id_type uid, lid, pair_id;

  beg = LEP_STATE_twin (beg);
  end = LEP_STATE_twin (end);
  arc = NULL;

  if ((! beg) || (! end))
    handle_error ("Programming error", "LEP_copy_section", 1);

  for (;;)
    {
      if (upstart < ALPH_len (upper))    /* get upper ID */
    uid = ALPH_item (upper, upstart++);
      else
    uid = EPSILON;

      if (lowstart < ALPH_len (lower))   /* get lower ID */
    lid = ALPH_item (lower, lowstart++);
      else
    lid = EPSILON;

      if ((uid == EPSILON) && (lid == EPSILON)) /* all IDs finished */
    {
      if (! arc)       /* section must have at least one arc */
        handle_error ("Programming error", "LEP_copy_section", 1);

      ARC_destination (arc) = end;
      return;
    }

      if (arc)  /* previous arc exists */
    ARC_destination (arc) = beg = make_state (NULL, newnet);

      if (uid == lid)
    arc = make_arc (uid, NULL, newnet);
      else
    {
      make_pair_label(&pair_id, uid, lid);
      arc = make_arc (pair_id, NULL, newnet);
    }

      add_arc (arc, beg, newnet);
    }
}

static void LEP_eps_push_in_tail
  (NETptr newnet, STATEptr state, STATEptr start,
   ALPHABETptr upper, ALPHABETptr lower, uint16 upstart, uint16 lowstart)
{
  /* Visits recursively the FSTs tail starting at "state".
     Creates a section copy whenever an "untouchable" state is reached. */

  ARCptr arc;  
  uint16 uplen, lowlen;

  /* state is "untouchable" and different from section begin */

  if ((LEP_STATE_twin (state)) &&
      ((ALPH_len (upper) > upstart) || (ALPH_len (lower) > lowstart)))
    {
      LEP_copy_section
    (newnet, state, start, upper, lower, upstart, lowstart);

      start = state;
      upstart = ALPH_len (upper);
      lowstart = ALPH_len (lower);
    }

  /* state either IN_PROCESS or DONE --> return */

  if ((STATE_visit_mark (state) == IN_PROCESS) ||
      (STATE_visit_mark (state) == DONE))
    {
      if (! LEP_STATE_twin (state))  /* must be "untouchable" */
    handle_error ("Programming error", "LEP_eps_push_in_tail", 1);

      return;
    }

  /* visit recursively destinations of all arcs */

  uplen = ALPH_len (upper);
  lowlen = ALPH_len (lower);

  STATE_visit_mark (state) = IN_PROCESS;

  for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
    {
      LEP_add_id_to_seq (upper, UPPER_MATCH [ARC_label (arc)]);
      LEP_add_id_to_seq (lower, LOWER_MATCH [ARC_label (arc)]);

      LEP_eps_push_in_tail (newnet, ARC_destination (arc), start,
                upper, lower, upstart, lowstart);

      ALPH_len (upper) = uplen;
      ALPH_len (lower) = lowlen;
    }

  STATE_visit_mark (state) = DONE;
}

/*-----------------------------------------------------------*/
static void LEP_update_newnet_alphabets (NETptr oldnet, NETptr newnet)
{
  /* collect labels and sigma from arcs in newnet */

  if (NET_sigma (newnet))
    free_alph (NET_sigma (newnet));

  NET_labels (newnet) = update_label_alphabet (newnet);
  NET_sigma (newnet) = label_sigma (newnet);

  /* add sigma items from oldnet */

  adjust_alph_size (NET_sigma (newnet), ALPH_len (NET_sigma (oldnet)));
  add_binary_alph (ALPH_items (NET_sigma (newnet)),
           ALPH_items (NET_sigma (oldnet)),
           ALPH_len (NET_sigma (oldnet)));
}

static void LEP_update_newnet_properties (NETptr oldnet, NETptr newnet)
{
  NET_deterministic (newnet) = FALSE;
  NET_pruned (newnet) = FALSE;
  NET_completed (newnet) = FALSE;
  NET_epsilon_free (newnet) = FALSE;
  NET_sorted_states (newnet) = FALSE;
  NET_loop_free (newnet) = (NET_loop_free (oldnet));
  NET_visit_marks_dirty (newnet) = TRUE;
  NET_names_matter (newnet) = FALSE;
  NET_shared_arc_lists (newnet) = FALSE;
  NET_has_arc_user_pointer (newnet) = FALSE;
  NET_closed_sigma (newnet) = (NET_closed_sigma (oldnet));
  NET_start_state_final (newnet) = (NET_start_state_final (oldnet));
  NET_tabular (newnet) = FALSE;
  NET_twol_net (newnet) = (NET_twol_net (oldnet));
  NET_compacted (newnet) = FALSE;
  NET_upper_seq (newnet) = FALSE;
  NET_lower_seq (newnet) = FALSE;
  NET_mark (newnet) = FALSE;
  NET_u_flag_diacr (newnet) = (NET_u_flag_diacr (oldnet));
  NET_l_flag_diacr (newnet) = (NET_l_flag_diacr (oldnet));
}

/*-----------------------------------------------------------*/
NETptr LEP_local_eps_push_fst (NETptr oldnet, int reclaim_p, int minimize_p)
{
  /* TOP LEVEL FUNCTION for Local Epsilon Pushing. */

  NETptr newnet;  STATEptr state;  ALPHABETptr upper, lower;
  char error_str [100];

  /* do not handle simple FSMs and nets with ALTCHAIN arcs */

  switch (NET_arc_label_arity (oldnet))
    {
    case 1:   /* simple FSM */
      return (oldnet);
    case 2:   /* FST */
      break;
    default:
      sprintf (error_str, "Wrong arc label arity %hi in argument network",
           NET_arc_label_arity (oldnet));
      handle_warning (error_str, "LEP_local_eps_push_fst", 1);
      return (oldnet);
    }

  if (sigma_member (NET_sigma (oldnet), ALTCHAIN_SYMBOL))
    return (oldnet);

  /* mark "untouchable" states, and pair them with twin states */

  update_match_tables ();

  LEP_mark_untouchable_states (oldnet);
  newnet = LEP_make_twin_states (oldnet);

  /* visit recursively old net, and copy "cleaned" sections */

  for (state = NET_states (oldnet);  state;  state = next_state (state))
    STATE_visit_mark (state) = NOT_VISITED;

  upper = make_alph (LEP_SEQ_LENGTH_INIT, LABEL_VECTOR);
  lower = make_alph (LEP_SEQ_LENGTH_INIT, LABEL_VECTOR);

  ALPH_len (upper) = ALPH_len (lower) = 0;

  LEP_eps_push_in_tail
    (newnet, NET_start_state (oldnet), NET_start_state (oldnet),
     upper, lower, 0, 0);

  /* finish */

  LEP_update_newnet_alphabets (oldnet, newnet);
  LEP_update_newnet_properties (oldnet, newnet);

  free_alph (upper);
  free_alph (lower);

  if (reclaim_p == DO_RECLAIM)
    free_network (oldnet);

  if (minimize_p == DO_MINIMIZE)
    newnet = min_fsm (newnet, DONT_COPY);

  return (newnet);
}


/* ********** Path Addition Functions ************** */

static STATEptr find_anchor_state(NETptr fst, id_type** path, id_type* path_end, int reverse)
{
    /* This is a subroutine of the add_path function:
       it returns the state that can be reached with the longest match on PATH,
       (duplicating some of the traversed states if they have more than one
       incoming arc).
       If reverse is TRUE, the FST has been reversed and we are looking
       for a tail_anchor_state, processing path from right to left. */
    
    STATEptr state, copied_state= NULL;
    ARCptr arc, matching_arc;
    id_type* input;
    int matching_arc_found= FALSE;
    int anchor_state_found= FALSE;
    
    state= NET_start_state(fst);
    input= *path;

    while( (input != path_end) && !anchor_state_found)
     {
         matching_arc_found= FALSE;
         arc = STATE_arc_set(state);
         while (arc) 
         {
             if ( ARC_label(arc) == *input )
             { /* A matching arc has been found */
                    
                 matching_arc_found= TRUE;
                 matching_arc= arc;
                 break;
             }
             arc = next_arc(arc);
         }

         if (!matching_arc_found)
         { /* No matching are found from state:
              state becomes the anchor_state. */
             anchor_state_found= TRUE;
         }
         else
         {
             if( reverse && STATE_final(ARC_destination(matching_arc)) )
             {
                 /* The FST is being processed in the reverse direction,
                    and as the destination state of the matching arc is
                    final, it is not a candidate tail_anchor_state :
                    terminate by returning the current state. */
                 
                 anchor_state_found= TRUE;
                 break;
             } /* if reverse */
                                 
             if ((int) STATE_client_cell(ARC_destination(matching_arc)) != 1)
             {
                 /* There are several arcs coming to the destination state:
                    thus we can't use it to build the path, and we must
                    rather build a copy of this destination state. */

                 /* Copy the state AND its arcs */
                 copied_state=
                     copy_state(ARC_destination(matching_arc), fst);
                    
                 /* Copy also the state client_cell */
                 STATE_client_cell(copied_state) =
                     STATE_client_cell(ARC_destination(matching_arc));
                    
                 /* Update the client cell of each state that is directly
                    pointed from copied_state */
                 for (arc=STATE_arc_set(copied_state); arc; arc = next_arc(arc))
                 {
                     STATE_client_cell(ARC_destination(arc)) = (void*)
                       (  (int) STATE_client_cell(ARC_destination(arc)) + 1);
                 } /* for arc */
                    
                 /* Disconnect the matching arc: change its destination to
                    make the arcpoint to the copied state. */

                 /* Decrement the client cell of the current destination
                    state before changing the destination of matching_arc.*/
                 STATE_client_cell(ARC_destination(matching_arc)) = (void*)
                    ( (int) STATE_client_cell(ARC_destination(matching_arc)) -1 );
                 ARC_destination(matching_arc)= copied_state;
                        
                 /* Increment the client cell of the copied state. */
                 STATE_client_cell(copied_state) = (void*)
                    ( (int) STATE_client_cell(copied_state) + 1 );

             } /* if STATE_client_cell != 1 */
                
             /* Move to the destination state */
             state = ARC_destination(matching_arc);
                
             /* Go ahead with path. */
             if (reverse)
                 input--;
             else
                 input++;
                
         } /* matching arc found */            
     } /* while (*input != ID_EOS) && !anchor_state_found */
    
    *path = input; /* path contains the remaining part of the id sequence */
    return(state); /* state is the anchor_state */
    
} /* find_anchor_state */


NETptr add_path(NETptr fst, id_type* path)
{
    /* Adds the path represented by the string of id-s PATH to the FST, in
       such a way that:
       - FST remains deterministic,
       - It does not create unexpected over-generation,
       - states and arcs of the already existing paths are reused as much as
         possible.
       NB: PATH may contain new symbols that are in LABEL_MAP and not yet in
       the sigma and labels alphabets of FST; however, it can't introduce
       new multi-char symbols, since PATH has been parsed according to the
       current parse table of FST.
    */
    STATEptr state, new_state, anchor_state, start_state;
    STATEptr final_state= NULL;
    ARCptr arc, new_arc;
    id_type* input, *remaining_path, *path_end;
    
#ifdef DEBUG
    if (fsm_modify_debug !=0)
    {
        fsm_printf("The parsed path is: ");
        input=path;
        while (*input != ID_EOS)
        {
            fsm_printf("%d ", *input++);
        }
        fsm_printf("\n-----------\n");
    }
#endif
    
    /* Compute the address of the last id in PATH */
    input= path;
    while (*input != ID_EOS)
        input++;
    path_end= input;

    /* Test if the net is vectorized, compacted or labelSetreduced
       and reject it if so.
       The function cannot handle compacted and labelset reduced nets
       but it can be modified to handle vectorized nets, ... to be implemented later */
    if (NET_compacted(fst) || NET_reduced_labelset(fst) || has_arc_vectors_p(fst) )
    {
        fsm_printf("The add_path operation currently does not handle\n");
        fsm_printf("compacted or vectorized or labelset reduced transducers.\n");
    }
    else
    {
        /* CHECK IF PATH DOES NOT ALREADY BELONG TO THE FST
           -------------------------------------------------------------------- */
        state= NET_start_state(fst);
        input=path;

        while (*input != ID_EOS)
        {
            arc = STATE_arc_set(state);
            while (arc) 
            {
                if ( ARC_label(arc) == *input )
                { /* A matching arc has been found */
                    break;
                }
                arc = next_arc(arc);
            }
            if (arc)
            {
                state= ARC_destination(arc);
                input++;
            }
            else
                break;   
        } /* while *input */

        if ( (*input == ID_EOS) && STATE_final(state) )
        {
            fsm_printf("Path is already in the network ---> exit.\n");
            return(fst); 
        }
        
        /* SWEEP THE WHOLE NET AND SET THE STATE CLIENT_CELLS
           -------------------------------------------------------------------- */
        
        /* Clean the state client cells */
        clear_client_cells(fst);

        /* Set each state client_cell to the number of its incoming arcs */
        /* and find by the way the address of the final state if there is
           one having no outgoing arcs. */
        
         for (state = NET_states(fst); state != NULL; state = next_state(state))
         {
             for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
             {
                 STATE_client_cell(ARC_destination(arc))= (void*)
                    ( (int) STATE_client_cell(ARC_destination(arc)) + 1);
             } /* for arc */
             
             if ( STATE_final(state) && (!final_state) && (STATE_arc_set(state)== NULL) )
             {
                 /* Current state is final and has no outgoing arcs: it can be used
                    as "final_state" to terminate the new path to be added. */
                 final_state= state;
             }
         } /* for state */

         
         /* CHECK IF THERE IS A LOOP ON THE INITIAL STATE
           -------------------------------------------------------------------- */
         if ( (int) STATE_client_cell(NET_start_state(fst)) >0)
         {
             /* There is a loop on the start state: consequently, 
                the start state must be duplicated, and its
                duplicate becomes the start state. */
#ifdef DEBUG
             if (fsm_modify_debug !=0)
             {
                 fsm_printf("\nLoop found on the start state... \n");
             }
#endif
             /* copy the start state AND all its arcs */
             start_state= copy_state(NET_start_state(fst), fst);
             
             /* set its client_cell to 0. */
             STATE_client_cell(start_state)= 0;
                     
             /* Update the client cell of each state that is directly
                pointed from the new start_state */
             for (arc=STATE_arc_set(start_state); arc; arc = next_arc(arc))
             {
                 STATE_client_cell(ARC_destination(arc)) = (void*)
                    ( (int) STATE_client_cell(ARC_destination(arc)) + 1);
             } /* for arc */

             /* start_state becomes the start state of fst */
             NET_start_state(fst)= start_state;
         }
             
                               
         /* FIND THE ANCHOR STATE IN THE FST
            -------------------------------------------------------------------- */
         input= path;
         anchor_state= find_anchor_state(fst, &input, path_end,
                                         FALSE /* reverse */);
#ifdef DEBUG
         if (fsm_modify_debug !=0)
         {
             if ( anchor_state == NET_start_state(fst) )
             {
                 fsm_printf("\nAnchor state found on the start state \n");
             }
         }
#endif
        
        /* BUILD THE PATH
           -------------------------------------------------------------------- */ 
        if (*input == ID_EOS)
        { 
            /* Note that at this step, anchor_state cannot be final, otherwise
               PATH would have been detected from the very beginning as a path
               already belonging to the net.
               Path is rather the beginning sub-section of a path already in
               fst and in this special case, adding PATH just requires to make
               the anchor_state final. */  
            STATE_final(anchor_state)= TRUE;
        }
        else
        {
            /* Build the subpath with states and 2-sided (or 1-sided) arcs, and 
               make it terminate on a new final state or on the candidate final_state
               if any has been detected during the first fst sweep. */
            
            /* The path is built in reverse order: from the end to the
               beginning. */
            remaining_path=input;
            while (*input != ID_EOS)
                input++;
            
            /* Start by localizing a final state for the new subpath. */
            if (final_state && (final_state != anchor_state) )
            { /* We can use the final state selected from the FST */
                new_state= final_state;
            }
            else
            { /* We haven't found in FST a final state having no outgoing arcs, 
                 or the one we had recorded as final_state is also the anchor_state
                 so that it has now an outgoing arc.
                 Consequently, a new final state must be allocated. */
                new_state= make_state(NULL, fst);
                STATE_final(new_state)= TRUE;
            }
            
            input--; /* input points to the last id pair in PATH */
            
            while (input != remaining_path)
            {
                new_arc=  make_arc (*input, new_state, fst);
                new_state= make_state (new_arc, fst);
                input--;
            } /* while */
            
            /* Branch the sub-path on the anchor state */
             new_arc=  make_arc (*input, new_state, fst);
             next_arc(new_arc)= STATE_arc_set(anchor_state);
             STATE_arc_set(anchor_state)= new_arc;
        } /* else *input == */      

        
        /* The parse tables DONT need to be updated because if the new path
           has introduced new (single-char only) symbols, they have been
           automatically interned in the upper parse table or in the lower
           one, during the parsing step, when calling the function
           pair_symbol_from_string(). */

        /* Free label alphabet and make a new one by looking at
           the arcs in the network */
        update_label_alphabet(fst);

        /* Free sigma alphabet and make a new one by looking at
           the label alphabet */
        free_alph(NET_sigma(fst));
        NET_sigma(fst) = label_sigma(fst);
             
        /* Prepare the net for pruning if requested */
        NET_pruned(fst)= FALSE;
        
        /* Prepare the net for minimizing if requested */
        NET_minimized(fst)= FALSE;

        /* If the net's arcs were previously sorted, this arc ordering is lost. */
        NET_sorted_arcs(fst) = 0;
        
    } /* else (NET_compacted(fst) ... */
    
    return(fst);
    
} /* add_path version1 */



NETptr add_path_version2(NETptr fst, id_type* path)
{
    /* This is a variant of add_path that processes the FST
       from left to right AND then from right to left, trying
       to find the best anchor state from the start state, and
       the best tail_anchor_state from the final state; building the
       intermediate sub-path from anchor_state to tail_anchor_state.
     */
    STATEptr state, new_state, anchor_state, start_state;
    STATEptr final_state= NULL;
    STATEptr tail_anchor_state= NULL;
    ARCptr arc, new_arc;
    id_type* input, *path_end;

    
#ifdef DEBUG
    if (fsm_modify_debug !=0)
        {
            fsm_printf("The parsed path is: ");
            input=path;
            while (*input != ID_EOS)
            {
                fsm_printf("%d ", *input++);
            }
            fsm_printf("\n-----------\n");
        }
#endif

    input= path;
    while (*input != ID_EOS)
        input++;
    path_end= input;

    /* Test if the net is vectorized, compacted or labelSetreduced
       and reject it if so.
       The function cannot handle compacted and labelset reduced nets
       but it can be modified to handle vectorized nets, ... to be
       implemented later. */
    if (NET_compacted(fst) || NET_reduced_labelset(fst) || has_arc_vectors_p(fst) )
    {
        fsm_printf("The add_path operation currently does not handle\n");
        fsm_printf("compacted or vectorized or labelset reduced transducers.\n");
    }
    else
    {
        /* CHECK IF PATH DOES NOT ALREADY BELONG TO THE FST
           -------------------------------------------------------------------- */
        state= NET_start_state(fst);
        input=path;

        while (*input != ID_EOS)
        {
            arc = STATE_arc_set(state);
            while (arc) 
            {
                if ( ARC_label(arc) == *input )
                { /* A matching arc has been found */
                    break;
                }
                arc = next_arc(arc);
            }
            if (arc)
            {
                state= ARC_destination(arc);
                input++;
            }
            else
                break;   
        } /* while *input */

        if ( (*input == ID_EOS) && STATE_final(state) )
        {
            fsm_printf("Path is already in the network ---> exit.\n");
            return(fst); 
        }
        
        /* SWEEP THE WHOLE NET AND SET THE STATE CLIENT_CELLS
           -------------------------------------------------------------------- */
        
        /* Clean the state client cells */
        clear_client_cells(fst);

        /* Set each state client_cell to the number of its incoming arcs */
        /* and find by the way the address of the final state if there is
           one having no outgoing arcs. */
        
         for (state = NET_states(fst); state != NULL; state = next_state(state))
         {
             for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
             {
                 STATE_client_cell(ARC_destination(arc))= (void*)
                   (  (int) STATE_client_cell(ARC_destination(arc)) + 1);
             } /* for arc */
             
             if ( STATE_final(state) && !final_state && (STATE_arc_set(state)== NULL) )
             {
                 /* Current state is final and has no outgoing arcs: it can be used as
                    "final_state" to terminate the new path to be added. */
                 final_state= state;
             }
         } /* for state */

         
         /* CHECK IF THERE IS A LOOP ON THE INITIAL STATE
           -------------------------------------------------------------------- */
         if ( (int) STATE_client_cell(NET_start_state(fst)) >0)
         {
             /* There is a loop on the start state: consequently, 
                the start state must be duplicated, and its
                duplicate becomes the start state. */
#ifdef DEBUG
    if (fsm_modify_debug !=0)
    {
             fsm_printf("\nLoop found on the start state... \n");
    }
#endif
             /* copy the start state AND all its arcs */
             start_state= copy_state(NET_start_state(fst), fst);
             
             /* set its client_cell to 0. */
             STATE_client_cell(start_state)= 0;
                     
             /* Update the client cell of each state that is directly
                pointed from the new start_state */
             for (arc=STATE_arc_set(start_state); arc; arc = next_arc(arc))
             {
                 STATE_client_cell(ARC_destination(arc)) = (void*)
                     ( (int) STATE_client_cell(ARC_destination(arc)) + 1);
             } /* for arc */

             /* start_state becomes the start state of fst */
             NET_start_state(fst)= start_state;
         }
             

         start_state = NET_start_state(fst);
         
         /* FIND THE ANCHOR STATE IN THE FST
            -------------------------------------------------------------------- */
         input= path;
         anchor_state= find_anchor_state(fst, &input, path_end, FALSE /*=reverse*/);
#ifdef DEBUG
         if (fsm_modify_debug !=0)
         {     
             if ( anchor_state == NET_start_state(fst) )
             {
                 fsm_printf("\nAnchor state found on the start state \n");
             }
         }
#endif
                       
       
        if (*input == ID_EOS)
        { 
            /* Note that at this step, anchor_state cannot be final, otherwise
               PATH would have been detected from the very beginning as a path
               already belonging to the net.
               Path is rather the beginning sub-section of a path already in
               fst and in this special case, adding PATH just requires to make
               the anchor_state final. */  
            STATE_final(anchor_state)= TRUE;
        }
        else
        {
             /* FIND THE TAIL ANCHOR STATE IN THE FST
                -------------------------------------------------------------------- */ 
             id_type* remaining_path_left_to_right= NULL;
             id_type* remaining_path_right_to_left= NULL;

             remaining_path_left_to_right=input;
             while (*input != ID_EOS)
                 input++;
             remaining_path_right_to_left= --input;
            
            /* reverse the net and try to find the best tail_anchor_state
               from the final state. */
            
            if (!final_state)
            {
                tail_anchor_state= make_state (NULL, fst);
                STATE_final(tail_anchor_state)= TRUE;
            }
            else
            {
                if (remaining_path_left_to_right ==remaining_path_right_to_left)
                {
                    tail_anchor_state= final_state;
#ifdef DEBUG
                     if (fsm_modify_debug !=0)
                     {  
                         fsm_printf("Only one symbol left in path ! \n");
                     }
#endif
                }
                else
                {
                    /* reverse the net */
                    prepare_arc_marks(fst, 0);
                    invert_arcs(fst, 1);
                    NET_start_state(fst)=final_state;

                    /* Clean the state client cells */
                    clear_client_cells(fst);

                    /* Set each state client_cell to the number of
                       its incoming arcs */    
                    for (state = NET_states(fst); state != NULL;
                         state = next_state(state))
                    {
                        for (arc= STATE_arc_set(state); arc != NULL;
                             arc = next_arc(arc))
                        {
                            STATE_client_cell(ARC_destination(arc))= (void*)
                               ( (int) STATE_client_cell(ARC_destination(arc)) + 1);
                        } /* for arc */
                    } /* for state */
                    
                    /* Find the tail_anchor state */
                    tail_anchor_state= find_anchor_state(fst,
                                                         &remaining_path_right_to_left,
                                                         remaining_path_left_to_right,
                                                         TRUE /*reverse */);

#ifdef DEBUG
                    if (fsm_modify_debug !=0)
                    {  
                        if (tail_anchor_state != NET_start_state(fst) )
                        {
                        fsm_printf("Real tail_anchor_state found after %d! \n",*remaining_path_right_to_left );

                        }
                    }
#endif
                    
                    /* Revert the network once again */
                    prepare_arc_marks(fst, 0);

                    invert_arcs(fst, 1);
                    NET_start_state(fst)= start_state;
                    
                } /*remaining_path_left_to_right !=remaining_path_right_to_left*/

            } /* else final_state */


            /* BUILD THE PATH
               -------------------------------------------------------------------- */ 
            /* Build a "bridge path" from anchor_state to tail_anchor_state */
            new_state= tail_anchor_state;
            while (remaining_path_left_to_right < remaining_path_right_to_left)
            {
                new_arc=  make_arc (*remaining_path_right_to_left,
                                    new_state, fst);
                new_state= make_state (new_arc, fst);
                remaining_path_right_to_left--;
            } /* while */
           
            /* Branch the last arc between the 2 anchor states */
             new_arc=  make_arc (*remaining_path_left_to_right, new_state, fst);
             next_arc(new_arc)= STATE_arc_set(anchor_state);
             STATE_arc_set(anchor_state)= new_arc;
        } /* else *input == */

        
        /* The parse tables DONT need to be updated because if the new path
           has introduced new (single-char only) symbols, they have been
           automatically interned in the upper parse table or in the lower
           one, during the parsing step, when calling the function
           pair_symbol_from_string(). */

        /* Free label alphabet and make a new one by looking at
           the arcs in the network */
        update_label_alphabet(fst);

        /* Free sigma alphabet and make a new one by looking at
           the label alphabet */
        free_alph(NET_sigma(fst));
        NET_sigma(fst) = label_sigma(fst);
             
        /* Prepare the net for pruning if requested */
        NET_pruned(fst)= FALSE;
        
        /* Prepare the net for minimizing if requested */
        NET_minimized(fst)= FALSE;
        
    } /* else (NET_compacted(fst) ... */
    
    return(fst);
    
} /* add_path: version2 */



NETptr add_word(NETptr fst, id_type* word, id_type* model)
{
    /* This function aims at enriching an FST with all the inflected
       forms of a WORD following a MODEL word whose inflected forms
       are already part of the FST. */
    /* NOT YET IMPLEMENTED ... */
    fsm_printf("add-words is not yet implemented ...\n");
    
    fsm_printf("word:\n");
    while (*word != ID_EOS)
    {
         fsm_printf("%d ", *word++);
    }
    fsm_printf("\nmodel: \n");
    
    while (*model != ID_EOS)
    {
         fsm_printf("%d ", *model++);
    }
    fsm_printf("\n ---------- \n");
    
    return(fst);
} /* add_word */
#endif /* 0 */
#endif /* SLIM_FST */

/******************************************/
