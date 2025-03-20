/* $Id: modify.c,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  MODIFY.C
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

#ifdef _WIN32 /* YM, 2/23/96 to have the prototype of isatty */
#include <io.h>
#else
#ifdef MAC_OS
#include <unix.h>
#endif /* MAC_OS */
#endif  /* WIN32 */


#include "fst/modify.h"
#include "fst/fsm_io.h"
#include "fst/pars-tbl.h"
#include "fst/label.h"
#include "fst/pretty-p.h"
#include "fst/eqv-labs.h"
#include "fst/vector.h"
#include "fst/flags.h"
#include "fst/bin-in.h"          /* For init_restore_functions() */
#include "fst/bin-out.h"         /* For init_save_functions() */
#include "fst/virtual-fsm.h"     /* For init_restore_functions() */
#include "fst/compact.h"         /* For compactions in determinize_fsm() 2002-3-26 TG */
#include "fst/mark.h"
#include "fst/struct.h"

/* #include "fst/calculus.h" */

/* STRUCT.C by default sets FSM_MODE to STANDARD_MODE (defined
   in TYPES.H.  The other alternative is TWOL_MODE.  In TWOL_MODE,
   fstpairs are treated as if they were atomic symbols and SIGMA
   is composed of labels rather than atomic symbols.
   */

/* For selecting the messages to be printed in debug mode */
#ifdef DEBUG
int fsm_modify_debug=0;
#endif


/*****************************************
 *
 *  Switch between basic network functions
 *  (such as load, save, etc)
 *
 ******************************************/

/* Default saving function:
   its signature is aligned with the save_func prototype. */
static int save_unsupported_network_format(FILE *stream, unsigned long *byte_countp,
                                           NETptr net, unsigned long *arc_count,
                                           unsigned long *state_count,
                                           unsigned long *file_lengthp,
                                           int old_format_p)
{
    fsm_fprintf(stderr, "Unsupported file format: ");
    fsm_fprintf(stderr, "cannot write network to file.\n");
    return ERROR;
}

#ifdef WORLD
void init_save_functions()
{
#ifndef SLIM_FST
    /* Saving of Karttunen compacted networks:
       this format is not supported by the licensable version of the lib. */
    save_functions[TypeSaveCompact]= save_unsupported_network_format;

    /* Saving of virtual networks:
       this format is not supported by the licensable version of the lib. */
    save_functions[TypeSaveVirtual]= save_unsupported_network_format;

    /* Saving of Kaplan compressed networks:
       it is not possible to save a net in this format neither in the
       licensable version, nor in the non-licensable version of the lib. */
    save_functions[TypeSaveCompress]= save_unsupported_network_format;

    /* Saving of standard networks: */
    save_functions[TypeSaveStandard]= write_arcs_and_states;
#endif /* SLIM_FST */
} /* init_save_functions */
#else
void init_save_functions()
{
#ifndef SLIM_FST
    /* Saving of compacted networks: */
    save_functions[TypeSaveCompact]= write_arc_block;

    /* Saving of virtual networks: */
    save_functions[TypeSaveVirtual]= write_virtual_net;

    /* Saving of Kaplan compressed networks:
       it is not possible to save a net in this format neither in the
       licensable version, nor in the non-licensable version of the lib. */
    save_functions[TypeSaveCompress]= save_unsupported_network_format;

    /* Saving of standard networks: */
    save_functions[TypeSaveStandard]= write_arcs_and_states;
    
#endif /* SLIM_FST */
} /* init_save_functions */
#endif /* WORLD */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/*********************************
 *
 *  Longest path
 *
 **********************************/

static int symbol_length(id_type id, int print_epsilon_p)
{
  if (id == EPSILON || id == ALTCHAIN_SYMBOL)
    {
      if (print_epsilon_p)
	return(1);		/* Epsilon printed as 0 */
      else
	return(0);		/* Epsilon not printed. */
    }
  else if (id == OTHER)
    return(1);			/* Printed as '?'. */
  else if (flag_diacritic_p(id))
    {
      if (print_epsilon_p)
	return(fat_strlen(id_to_atom_name(id)));
      else
	return(0);
    }
  else
    return(fat_strlen(id_to_atom_name(id)));
}

static int label_length(id_type id, int side, int print_epsilon_p)
{
  if (side == UPPER)
    return(symbol_length(upper_id(id), print_epsilon_p));
  else if (side == LOWER)
    return(symbol_length(lower_id(id), print_epsilon_p));
  else if (side == BOTH_SIDES)
    {
      id_type upper = upper_id(id), lower = lower_id(id);
       
      if (upper == lower)
	return(symbol_length(upper, print_epsilon_p));
      else
	return(symbol_length(upper, print_epsilon_p) + 1
	       + symbol_length(lower, print_epsilon_p));
    }
  else
    {
      handle_error("Bad argument", "LABEL_LENGTH", side);
      return(-1);
    }
}

static int MAX_DEPTH ;

static int longest_string_from_state(STATEptr state, int side, int level,
				     int obey_flags_p)
{
  /* Returns the longest substring of symbols on SIDE ignoring epsilons. */

  int longest_string = 0;
  
  if (level > MAX_DEPTH)
    MAX_DEPTH = level;
    
  if ((int) STATE_client_cell(state) > 0)
    longest_string = (int) STATE_client_cell(state);
  else if (STATE_client_cell(state) == NULL)
    {
      int len;
      ARCptr a;
      
      STATE_client_cell(state) = (void *) -1; /* state is being processed */
      
      for (a = STATE_arc_set(state); a != NULL; a = next_arc(a))
	{
	  if ( ( (int) STATE_client_cell(ARC_destination(a)) == -1) &&
	       (! STATE_final(ARC_destination(a))))
	    continue;

	  len = longest_string_from_state(ARC_destination(a), side, level+1,
					  obey_flags_p)
	    + label_length(ARC_label(a), side, !obey_flags_p);
	  
	  if (len > longest_string)
	    longest_string = len ;
	}
      STATE_client_cell(state) = (void *) longest_string;
    }

  return(longest_string);
}


static int longest_string_from_state_with_mark(STATEptr state, int side, int level,
					       int obey_flags_p)
{
  /* Returns the longest substring of symbols on SIDE ignoring epsilons. */

  ARCptr last_marked_arc;
  int longest_string = 0;
  
  if (level > MAX_DEPTH)
    MAX_DEPTH = level;
    
  if ((int) STATE_client_cell(state) > 0)
    longest_string = (int) STATE_client_cell(state);
  else if (STATE_client_cell(state) == NULL)
    {
      int len;
      ARCptr a;
      
      STATE_client_cell(state) = (void *) -1; /* state is being processed */
      
      for (a = STATE_arc_set(state), last_marked_arc = NULL; a != NULL; a = next_arc(a))
	{

	  if ( ( (int) STATE_client_cell(ARC_destination(a)) == -1) &&
	       (! STATE_final(ARC_destination(a))))
	    continue;

	  len = longest_string_from_state_with_mark(ARC_destination(a), side, level+1,
						    obey_flags_p)
	    + label_length(ARC_label(a), side, !obey_flags_p);
	  
	  if (len > longest_string)
	    {
	      longest_string = len ;
	      ARC_visit_mark(a) = 1;
	      if (last_marked_arc)
		ARC_visit_mark(last_marked_arc) = 0;
	      last_marked_arc = a;
	    }
	}
      STATE_client_cell(state) = (void *) longest_string;
    }

  return(longest_string);
}


int longest_minimal_string(NETptr net, int side, int obey_flags_p)
     /*****
      *
      * Returns the length of the longest "minimal" string in NET, that is, the
      * longest string from the start state to a final state that doesn't loop.
      * This function sums the lengths of the arc labels on the path.
      *
      * N.B. Epsilons are ignored.
      *
      *****/
{	
  int longest_string; 
  
  clear_client_cells(net);
  MAX_DEPTH = 0;
  longest_string = longest_string_from_state(NET_start_state(net), side, 1,
					     obey_flags_p);
  clear_client_cells(net);

  return (longest_string);
}

static void reset_visit_marks_to_zero(NETptr t1)
{
  ARCptr arc1;
  STATEptr st;
  for ( st =  NET_states(t1); st; st = next_state (st) )
    {
      STATE_visit_mark(st) = 0;
      for (arc1 = STATE_arc_set(st); arc1 ; arc1 = next_arc (arc1))
	{
	  ARC_visit_mark(arc1) = 0;
	}
    }
}

static void print_labels_of_marked_arcs(STATEptr st, int space_p, int written,
					id_type (* iside_id) (id_type id), int * arcs_traversed, FILE *ffi) 
     /* supposes a single, i.e. deterministic, marked path */
{
  ARCptr arc;

  for (arc = STATE_arc_set(st); arc; arc = next_arc(arc))
    {
      if (ARC_visit_mark(arc) == 1)
	{
	  if (space_p && written)
	    fsm_fprintf (ffi, " ");
	  if (!(iside_id(ARC_label(arc)) == EPSILON ||
		iside_id(ARC_label(arc)) == ALTCHAIN_SYMBOL))
	    {
	      print_label (iside_id(ARC_label(arc)), ffi, DO_ESCAPE);
	      written=TRUE;
	    }
	  else
	    written = FALSE;
	  ARC_visit_mark(arc) = 0;
	  (*arcs_traversed)++;
	  print_labels_of_marked_arcs(ARC_destination(arc), space_p, written, iside_id, arcs_traversed, ffi);
	  break;
	 }
    }
}

int print_longest_minimal_string(NETptr net, int side, int obey_flags_p, int escape_p, FILE *ffi)
     /*****
      *
      * Returns the length of and prints the longest "minimal" string in NET, that
      * is, the longest string from the start state to a final state that doesn't loop.
      * This function sums the lengths of the arc labels on the path.
      *
      * N.B. Epsilons are ignored.
      *
      *****/
{	
  int longest_string;
  int arcs_traversed=0;
  id_type (* iside_id) (id_type id);

  clear_client_cells(net); 
  reset_visit_marks_to_zero(net);
  MAX_DEPTH = 0;
 if (side == UPPER)
   iside_id = upper_id;
 else
   iside_id = lower_id;

  longest_string = longest_string_from_state_with_mark(NET_start_state(net), side,
						       1, obey_flags_p);
  print_labels_of_marked_arcs(NET_start_state(net), escape_p, FALSE, iside_id, &arcs_traversed, ffi);
  /** fputc('\n', stdout);**/
  fsm_fprintf(ffi, "\n%d arcs traversed, in %d maximal depth.\n", arcs_traversed, MAX_DEPTH-1);

  clear_client_cells(net);
  reset_visit_marks_to_zero(net);
  
  return (longest_string);
}

/*********************************
 *
 *  Simple predicates
 *
 **********************************/

int null_fsm_p (NETptr net)
{
  /* NULLFSMP returns TRUE if NET accepts no strings at all */
  
  STATEptr state;
  
  prune_fsm(net, DONT_COPY);
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    if (STATE_final(state))
      return(FALSE);
  
  return(TRUE);
}

int deterministic_arc_set_p (ARCptr arc)
     /****
      *
      * Returns TRUE (1) if the arc set headed by arc contains
      * no duplicate labels.  FALSE (0) otherwise.
      *
      *****/
{
    id_type *id_array = (id_type *) calloc((size_t)  LIM_id_count(LABEL_MAP), 
                                   (size_t) sizeof(id_type));

    while (arc != NULL)
    {
        if (id_array[ARC_label(arc)])
        {
            free((void *) id_array);
            return(FALSE);
        }
        else
            id_array[ARC_label(arc)] = 1;

        arc = next_arc(arc);
    }

    free(id_array);
    return(TRUE);
}

int disjoint_arcs_p (STATEptr s1, STATEptr s2)
     /*****
      *
      * Returns TRUE (1) is s1 and s2 have no arcs with identical labels,
      * FALSE (0) otherwise.
      *
      *****/
{
    ARCptr arc;
  
    id_type *id_array = (id_type *) calloc((size_t)  LIM_id_count(LABEL_MAP), 
                                   (size_t) sizeof(id_type));

    for (arc = STATE_arc_set(s1); arc != NULL; arc = next_arc(arc))
        id_array[ARC_label(arc)] = 1 ;

    for (arc = STATE_arc_set(s2); arc != NULL; arc = next_arc(arc))
        if (id_array[ARC_label(arc)])
        {
            free((void *) id_array);
            return (FALSE); 
        }

    free((void *) id_array);
    return (TRUE);
}

int has_arc_p(STATEptr state, id_type id)
{
  /* Returns TRUE if STATE contains an arc with ID as its label.
     Otherwise FALSE.
     */

  ARCptr arc;

  for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
    if (ARC_label(arc) == id)
      return(TRUE);

  return(FALSE);
}


int start_loops_p (NETptr net)
     /******
      *
      * Returns TRUE (1) if any arc in NET points to the start state of NET.
      * Returns FALSE (0) otherwise.
      *
      ******/
{
    ARCptr arc;
    STATEptr start, state;
  
    start = NET_start_state(net);
    for (state = NET_states(net); state != NULL; state = next_state(state))
        for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
            if (ARC_destination(arc) == start)
            {
                if (NET_loop_free(net))
                {
                    /* Should never happen! */
                    handle_warning("Net incorrectly marked as loopfree",
                                   "START_LOOPS_P", NET_loop_free(net));
                    NET_loop_free(net) = FALSE;
                }
                return (TRUE);  
            }
  
    return (FALSE);
}

int sigma_star_fsm_p(NETptr net)
{
  /* Returns TRUE if NET is equivalent to sigma_star_fsm().
     Otherwise FALSE.
     */

  STATEptr state;
  ALPHABETptr sig = label_to_binary(sigma(net));
  int i, len = ALPH_len(sig);

  /* OTHER must be in the alphabet. */

  if (binary_sigma_member(sig, OTHER))
    {
      epsilon_remove(net, DONT_COPY);
  
      /* Every state must be final. */

      for (state = NET_states(net); state; state = next_state(state))
        if (!(STATE_final(state)))
          return(FALSE);

      /* The net must be deterministic for the next test. */

      determinize_fsm(net, DONT_COPY);

      /* Every symbol in the alphabet must have an arc in every state. */

      for (i = OTHER; i < len; i++)
	if (ALPH_item(sig, i))
	  for (state = NET_states(net); state; state = next_state(state))
	    if (!(has_arc_p(state, i)))
	      return(FALSE);

      return(TRUE);
  }
  else
    return(FALSE);
}

static int final_state_in_eps_closure_p(STATEptr state)
{
  /* Returns TRUE if the state is final and has no arcs. If STATE
     has arcs, then the return value is TRUE if every arc is an
     EPSILON arc leading to a final state.
     */

  ARCptr arc;
  int final_p = STATE_final(state);
  
  if (STATE_visit_mark(state))
    return(TRUE);

  STATE_visit_mark(state) = 1;

  for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
    if (ARC_label(arc) == EPSILON)
      {
	if (!(final_p = final_state_in_eps_closure_p(ARC_destination(arc))))
	  break;
      }
    else
      {
	final_p = FALSE;
	break;
      }
  
  STATE_visit_mark(state) = 0;

  return(final_p);
}


int epsilon_fsm_p(NETptr net)
{
  /* Returns TRUE if NET is equivalent to epsilon_fsm().
     Otherwise FALSE.
     */
  STATEptr start_state = NET_start_state(net);
  
  /* Must prune first to through out dead paths, if any. */

  prune_fsm(net, DONT_COPY);

  clear_state_marks_and_client_cells(net);

  return(final_state_in_eps_closure_p(start_state));
}


static int sublang_process_states(STATEptr state1, STATEptr state2, 
                                  STACKptr still_to_do, STATEptr *dest_array)
     /* Support function for sublanguage_p() */
{
  CONSptr cur;
  ARCptr a1, a2;
  STATEptr a2_dest;
  int previously_matched;
  
  /* If STATE2 has already been paired with STATE1, 
     nothing further needs to be done here.
     */
  
  push ((void *) state1, still_to_do);
  push ((void *) state2, still_to_do);
  
  while (STACK_depth(still_to_do) > 0)
    {
      state2 = (STATEptr) pop(still_to_do);
      state1 = (STATEptr) pop(still_to_do);
      previously_matched = FALSE;
      
      if (STATE_client_cell(state1) != NULL)
        {
          switch (STATE_visit_mark(state1))
            {
              case(1):  /* client cell contains a single state */
                if ((STATEptr) STATE_client_cell(state1) == state2)
                  previously_matched = TRUE;
              break;
              case(2):  /* client cell contains a list of states */
                for (cur = (CONSptr) STATE_client_cell(state1);
                     cur != NULL;
                     cur = cdr(cur))
                  if ((STATEptr) car(cur) == state2)
                    {
                      previously_matched = TRUE;
                      break; /* for loop */
                    }
              break;
            }
        }
      
      if (!previously_matched)
        {
          if (!(STATE_final(state1)) || STATE_final(state2))
            {
              /* If we make it this far, then state1 & state2 haven't been
                 compared before. */
              /* STATE2 must be final if STATE1 is final */
              /* Link STATE2 to STATE1.  If we come back to STATE1 on
                 a different path, we know that STATE1 and STATE2 are
                 already being compared.  STATE1 visit mark encodes the
                 status of it's client cell: 0=empty, 1=single state, 2=list
                 of states.  We defer using cons cells until a second
                 matching state is found.
                 */
              switch(STATE_visit_mark(state1))
                {
                  case(0): /* client cell is empty --> just insert state */
                    STATE_client_cell(state1) = (void *) state2;
                  STATE_visit_mark(state1) = 1;
                  break;
                  case(1):
                    /* client cell contains a single state, coerce to list */
                    STATE_client_cell(state1) = 
                      cons((void *) state2,
                           cons(STATE_client_cell(state1), NULL));
                  STATE_visit_mark(state1) = 2;
                  break;
                case (2):  /* client cell contains a list */
                  STATE_client_cell(state1) =
                    cons((void *) state2, STATE_client_cell(state1));
                  break;
                }
              
              /* Place destinations of matching arcs on the still_to_do stack.
                 dest_array holds the destination of every arc leaving from
                 state2, indexed by its label.  This array is used to find
                 matching arcs from state 1 in linear rather than quadratic
                 time.  Both nets are deterministic at this point, so there
                 is only one destination per label.
                 */
              
              /* first pass:  load dest_array with arcs from state2 */
              for (a2 = STATE_arc_set(state2); a2 != NULL; a2 = next_arc(a2))
                dest_array[ARC_label(a2)] = ARC_destination(a2);
              
              /* second pass:
                 find matches, push destinations onto still_to_do */
              
              for (a1 = STATE_arc_set(state1); a1 != NULL; a1 = next_arc(a1))
                {
                  a2_dest = dest_array[ARC_label(a1)];
                  if (a2_dest != NULL)
                    {   
                      /* found a match -- push destinations onto still_to_do */
                      push ((void *) ARC_destination(a1), still_to_do);
                      push ((void *) a2_dest, still_to_do);
                    }
                  else
                    /* failed to find a match --> predicate fails */
                    /* Third pass is unnecessary because dest_array
                       won't get used anymore */
                    return(FALSE);
                }
              
              /* third pass:  clear dest_array */
              for (a2 = STATE_arc_set(state2); a2 != NULL; a2 = next_arc(a2))
                dest_array[ARC_label(a2)] = NULL;               
            }
          else 
            return (FALSE);  /* STATE2 fails to match STATE1 in finality */
        }
    }
  
  return (TRUE);
}

int sublanguage_p (NETptr net1, NETptr net2)
     /******
      *
      * Returns TRUE (1) if the language accepted by NET1 is a subset of
      * the language of NET2.  Almost the same algorithm as equiv_fsm_p(), 
      * except that a state in NET1 need not have a unique corresponding
      * state in NET2, and the matching states in NET2 may be final even
      * if the state they are paired with is non-final.
      *
      * A negative result is generally meaningless for transducers that have
      * arcs with EPSILON on the upper or lower side.
      * 
      * If one or the other network contains OTHER, we need to align sigmas
      * first.
      *
      ******/
{
  ALPHABETptr labels1, labels2;
  STACKptr still_to_do = create_stack("Sublanguage state pairs");
  STATEptr *dest_array;
  int value, i;
  
  if (net1 == net2)
    return (TRUE);
  
  min_fsm(net1, DONT_COPY);
  min_fsm(net2, DONT_COPY);
  
  align_sigmas(net1, net2);
  
  /* First check the label sets. */
  
  labels1 = binary_to_label(NET_labels(net1));
  labels2 = label_to_binary(copy_alphabet(NET_labels(net2)));
  
  for (i = 0; i < ALPH_len(NET_labels(net1)); i++)
    if (!binary_sigma_member(labels2, ALPH_item(labels1, i)))
      {
        free_alph(labels2);
        
        /* A label in NET1 is not present in NET2 */
        return(FALSE);
      }
  
  /* Must check states and arcs. */
  
  free_alph(labels2);
  init_cons_mem();
  clear_state_marks_and_client_cells(net1);
  
  /* Use client_cells of NET1 states to store the equivalent state(s) in NET2.
     STATE_visit_mark indicates whether client_cell is a state or a list
     of states.
     visit_mark:  0 = empty, 1 = single state, 2 = list of states.
     Clean the client_cells & visit_marks when done.
     */
  
  dest_array = (STATEptr *) calloc((size_t)  LIM_id_count(LABEL_MAP), 
                                   (size_t) sizeof(STATEptr));
  if (!dest_array)
    not_enough_memory("SUBLANGUAGE_P");
  
  value = sublang_process_states(NET_start_state(net1), NET_start_state(net2), 
                                 still_to_do, dest_array);
  
  free((void *) dest_array);
  free_stack(still_to_do);
  free_cons_mem();
  
  clear_state_marks_and_client_cells(net1);
  
  return(value);        
}

static int equiv_process_states(STATEptr state1, STATEptr state2,
                                STACKptr still_to_do, STATEptr *dest_array)
     /* Support function for equiv_fsm_p() */
{
  ARCptr a1, a2;
  STATEptr a1_dest;
  
  push ((void *) state1, still_to_do);
  push ((void *) state2, still_to_do);
  
  while (STACK_depth(still_to_do) > 0)
    {
      state2 = (STATEptr) pop(still_to_do);
      state1 = (STATEptr) pop(still_to_do);
      
      if (STATE_client_cell(state1) != NULL)
        {
          if ((STATEptr) STATE_client_cell(state1) != state2)
            return(FALSE); /* S1 already paired with another state --> FAIL */
        } /* if states have already been matched, nothing more to do. */
      else if (STATE_final(state1) == STATE_final(state2) &&
               num_arcs(state1) == num_arcs(state2))
        {
          /* STATE1 and STATE2 must match in finality and arc count. */
          /* Link STATE2 to STATE1.  If we come back to STATE1 on a different 
             path, we know that STATE1 and STATE2 are already being compared.
             */
          
          STATE_client_cell(state1) = (void *) state2;
          
          /* Place destinations of matching arcs on the still_to_do stack.
             dest_array holds the destination of every arc leaving from state1,
             indexed by its label.  This array is used to find matching arcs 
             from state 1 in linear rather than quadratic time.  Both nets are 
             deterministic at this point, so there is only one destination
             per label.
             */
          
          /* first pass:  load dest_array with arcs from state1 */
          for (a1 = STATE_arc_set(state1); a1 != NULL; a1 = next_arc(a1))
            dest_array[ARC_label(a1)] = ARC_destination(a1);
          
          /* second pass:  find matches & push destinations onto still_to_do */
          /* Both machines are deterministic, and both states have the same
             number of arcs.  So, only a single match is possible for each
             arc. Also, if this loop finishes, then every arc has been matched.
             */
          for (a2 = STATE_arc_set(state2); a2 != NULL; a2 = next_arc(a2))
            {
              a1_dest = dest_array[ARC_label(a2)];
              if (a1_dest != NULL)
                {
                  /* found a match -- push destinations onto still_to_do */
                  push ((void *) a1_dest, still_to_do);
                  push ((void *) ARC_destination(a2), still_to_do);
                }
              else
                /* failed to find a match -- predicate fails */
                return(FALSE);
            }
          
          /* third pass:  clear dest_array */
          for (a1 = STATE_arc_set(state1); a1 != NULL; a1 = next_arc(a1))
            dest_array[ARC_label(a1)] = NULL;
        }
      else 
        /* STATE2 fails to match STATE1 in finality or arc count. --> FAIL */
        return (FALSE);
    }
  return (TRUE);
}

int equiv_fsm_p (NETptr net1, NETptr net2)
     /******
      *
      * Returns TRUE (1) if the two nets accept the same language.
      * A negative result is generally meaningless for transducers that have
      * arcs with EPSILON on the upper or lower side. 
      * 
      * If one or the other network contains OTHER, we need to copy the
      * net and close_sigma on the copy.
      *
      ******/
{
  if (net1 == net2)
    return (TRUE);
  
  min_fsm(net1, DONT_COPY);
  min_fsm(net2, DONT_COPY);
  
  /* If the state counts are different or if the arc counts are different
     and OTHER is not in the alphabet of either net, the languages
     are different.
     */
  if (NET_num_states(net1) != NET_num_states(net2) ||
      (NET_num_arcs(net1) != NET_num_arcs(net2) &&
       !(sigma_member(NET_sigma(net1), OTHER) ||
         sigma_member(NET_sigma(net2), OTHER))))
    return(FALSE);
  else
    {
      int value;
      
      align_sigmas(net1, net2);
      
      /* Check the labels sets first.  If they are of different lengths
         or otherwise distinct, the languages are different.
         */
      
      if (ALPH_len(NET_labels(net1)) != ALPH_len(NET_labels(net2)))
        value = FALSE;
      else
        {
          ALPHABETptr labels1 =
            label_to_binary(copy_alphabet(NET_labels(net1)));
          ALPHABETptr labels2 =
            label_to_binary(copy_alphabet(NET_labels(net2)));
          
          if (memcmp((void *) ALPH_items(labels1),
                     (void *) ALPH_items(labels2),
                     (size_t) ALPH_len(labels1) * sizeof(id_type)) == 0)
            value = TRUE;
          else
            value = FALSE;
          
          free_alph(labels1);
          free_alph(labels2);
        }
      
      if (value == TRUE && ALPH_len(NET_labels(net1)) > 0)
        {
          /* Must check states and arcs. */
          
          STACKptr still_to_do = create_stack("Equivalence state pairs");
          STATEptr *dest_array =
            (STATEptr *) calloc((size_t)  LIM_id_count(LABEL_MAP), 
                                (size_t) sizeof(STATEptr));
          
          if (!dest_array)
            not_enough_memory("EQUIV_FSM_P");
          
          clear_state_marks_and_client_cells(net1);
          
          /* Use client_cells of NET1 states to store the equivalent state
             in NET2. Clean the client_cells & visit_marks when done.
             */
          
          value = equiv_process_states(NET_start_state(net1),
                                       NET_start_state(net2), 
                                       still_to_do, dest_array);
          
          free((void *) dest_array);
          free_stack(still_to_do);
          clear_state_marks_and_client_cells(net1);
        }
      
      return(value);
    }
}


static int epsilon_loop_state_p(STATEptr state, int side)
{
  /* Returns TRUE if there is an loop of arcs leading back to STATE
     such that the only symbols on SIDE are epsilons. Do not
     call directly. The calling function must first set all state
     visit marks to 2.
     */

  switch (STATE_visit_mark(state))
    {
    case 0:
      /* State already examined. No loop found. */
      return(FALSE);
      break;
    case 1:
      /* State seen earlier. Loop found! */
      return(TRUE);
      break;
    case 2:
      {
	/* Unprocessed state. Start processing it. */

	ARCptr arc;

	STATE_visit_mark(state) = 1;

	for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	  switch (LABEL_arity(id_to_label(ARC_label(arc))))
	    {
	    case 1:
	      break;
	    case 2:
	      if (side == UPPER)
		{
		  if (id_to_upper_id(ARC_label(arc)) == EPSILON &&
		      epsilon_loop_state_p(ARC_destination(arc), side))
		    return(TRUE);
		}
	      else
		{
		  if (id_to_lower_id(ARC_label(arc)) == EPSILON &&
		      epsilon_loop_state_p(ARC_destination(arc), side))
		    return(TRUE);
		}
	      break;
	    default:
	      handle_error("Bad arity", "EPSILON_LOOP_STATE_P",
			   LABEL_arity(id_to_label(ARC_label(arc))));
	      break;
	    }
      }
      /* No loop found. Mark state as processed. */
      STATE_visit_mark(state) = 0;
      return(FALSE);
      break;
    default:
      handle_error("Shouldn't happen", "EPSILON_LOOP_STATE_P",
		   STATE_visit_mark(state));
      return(FALSE);
      break;
    }
}



int linearbounded_p(NETptr net, int side)
{
  /* A transducer is linearbounded with respect to SIDE, if it has no
     onesided epsilon loops on that side.
     */

    /* This function cannot handle Karttunen compacted nets: */
    if (NET_compacted(net))
    {
#ifdef DEBUG
        handle_warning("Can't check loops on a compacted network.\n",
                       "linearbounded_p", 0);
#endif
        return(FALSE);
    }

  if (NET_loop_free(net))
    return(TRUE);
  else
    {
      STATEptr state;
  
      prepare_visit_marks(net, 2);
      
      for (state = NET_states(net); state; state = next_state(state))
	if (epsilon_loop_state_p(state, side))
	  {
	    while(state)
	      {
		STATE_visit_mark(state) = 0;
		state = next_state(state);
	      }
	    NET_visit_marks_dirty(net) = FALSE;
	    return(FALSE);
	  }
      NET_visit_marks_dirty(net) = FALSE;
      return(TRUE);
    }
}


/***************************
 *                         *
 *  Alphabet Manipulation  *
 *                         *
 ***************************/

/** void set_net_arity(NETptr net)
  Displaced to struct.c 2000-4-21 TG **/


/* static ALPHABETptr alph_cpy(ALPHABETptr target, ALPHABETptr source)
 * {
 */
  /* Arguments must be of the same type.  If the length of
     SOURCE is larger than the length of TARGET, an error is signalled.
     Sets the length indicator of TARGET to that of SOURCE, although
     the actual length of the target's item vector may be larger.
     The calling function needs to keep track of it.
     */
/*  
 *   int alph_len = ALPH_len(source);
 *   
 *   if (ALPH_type(target) != ALPH_type(source))
 *     handle_error("Mixed alphabet types", "ALPH_CPY", ALPH_type(source));
 *   
 *   if (alph_len > ALPH_len(target))
 *     handle_error("Unexpected length", "ALPH_COPY", alph_len);
 *   
 *   if (alph_len > 0)
 *     memcpy((void *) ALPH_items(target),
 *            (void *) ALPH_items(source),
 *            (size_t) alph_len * (size_t) sizeof(int16));
 *   
 *   ALPH_len(target) = alph_len;
 *   
 *   return(target);
 * }
 */

int invert_tuple(id_type *inv_id, LABELptr label)
{
  /* Retrieves an id of tuple with the opposite order or elements.
     That is, returns the id of the tuple <Y,X> for the label <X,Y>.
     If the inverse has not yet been determined, a new tuple is interned.
     */
  
  TUPLEptr tuple = LABEL_tuple(label);
  
  if (TUPLE_inverse(tuple) != ID_NO_SYMBOL)
    {
      *inv_id= TUPLE_inverse(tuple) ;
      return(NO_ERROR);
    }
  else
    {
      /* Inverse has not been determined yet */
      int error;
      int16 i, j;
      id_type id = LABEL_id(label), *labels = TUPLE_labels(tuple);
      int16 arity = LABEL_arity(label);
      static id_type *inv_labels;
      static int16 max_arity = 0;
      
      if (arity > max_arity)
        {
          if (max_arity > 0)
            free((void *) inv_labels);
          
          inv_labels = (id_type *) malloc((size_t) arity * sizeof(id_type));
          if (!inv_labels)
            not_enough_memory("INVERT_TUPLE");
          max_arity = arity;
        }
      
      for (i = 0, j = arity - 1; j >= 0 ; i++, j--)
        inv_labels[i] = labels[j];
      
       error= intern_tuple_label(inv_id, inv_labels, arity);
       if(error != NO_ERROR)
	 {
	   *inv_id= ID_NO_SYMBOL;
	   return(error);
	 }
       else
	 {
	 /* Make the inverse relation symmetric */
	   TUPLE_inverse(tuple) = *inv_id;
	   TUPLE_inverse(LABEL_tuple(id_to_label(*inv_id))) = id;
	   return(NO_ERROR);
	 }
    }
}

int label_member(ALPHABETptr labels, id_type item)
{
  /* Returns 1 if ITEM is a member of LABELS, 0 otherwise. */
  
  id_type *items;
  int i, len;

  if (labels == NULL)
    return(FALSE);
  
  if (ALPH_type(labels) == BINARY_VECTOR)
    return(binary_sigma_member(labels, item));
  
  len = ALPH_len(labels);
  items = ALPH_items(labels);
  
  for (i = 0; i < len; i++)
    if (*items++ == item)
      return(TRUE);
  
  return(FALSE);
}


static void adjust_bin_alph_len(ALPHABETptr alph)
{
  /* Correctly sets the length of a binary alphabet. */

  int len = ALPH_len(label_to_binary(alph));

  while (len >= 0)
    {
      if (ALPH_item(alph, len))
	break;
      len--;
    }

  if (len == 0)
    ALPH_len(alph) = 0;
  else
    ALPH_len(alph) = len + 1;
}

ALPHABETptr subtract_alph(ALPHABETptr a1, ALPHABETptr a2)
{
  /* Subtracts from A1 the symbols in A2 and returns the
     modified A1 in the original format.
     */
  
  int label_p;
  
  if (ALPH_type(a1) == LABEL_VECTOR)
    {
      label_to_binary(a1);
      label_p = TRUE;
    }
  else
    label_p = FALSE;
  
  if (ALPH_type(a2) == BINARY_VECTOR)
    {
      int len;
      
      if (ALPH_len(a1) <= ALPH_len(a2))
        len = ALPH_len(a1);
      else
        len = ALPH_len(a2);
      
      subtract_binary_alph(ALPH_items(a1), ALPH_items(a2), len);

      /* Adjust the length of A1 if it could have become shorter. */

      if (len == ALPH_len(a1))
	adjust_bin_alph_len(a1);
    }
  else
    {
      id_type  id; 
      int i, len1 = ALPH_len(a1), len2 = ALPH_len(a2);
      id_type *items1 = ALPH_items(a1), *items2 = ALPH_items(a2);
      
      for (i = 0; i < len2; i++)
        {
          id = *items2++;
          if (id < len1)
            items1[id] = 0;

	  /* Adjust the length of A1 if it could have become shorter. */

	  if (len2 >= len1)
	    adjust_bin_alph_len(a1);
        }
    }
  if (label_p)
    binary_to_label(a1);
  return(a1);
}

int sigma_disjoint_p(ALPHABETptr s1, ALPHABETptr s2)
     /*****
      *
      * Returns FALSE if S1 and S2 contain a common element,
      * otherwise TRUE.  Preserves S1 and S2.
      *
      *****/
{
  int i, lim, len1 = ALPH_len(s1), len2 = ALPH_len(s2);
  
  if (ALPH_type(s1) == BINARY_VECTOR)
    { 
      id_type *items1 = ALPH_items(s1), *items2 = ALPH_items(s2);

      if (ALPH_type(s2) == BINARY_VECTOR)
        {
          /* Both S1 and S2 are in binary form. */
          
          if (len1 <= len2)
            lim = len1;
          else
            lim = len2;
          
          for (i = 0; i < lim; i++, items1++, items2++)
            if (*items1 == 1 && *items2 == 1)
              return (FALSE);
        }
      else
        /* S1 is in binary form, S2 in label form. */
        for (i = 0; i < len2; i++)
          {
            id_type id2 = *items2++;
            
            if (id2 < len1 && items1[id2] == 1)
              return (FALSE);
          }

      return(TRUE);
    }

  /* if S1 is in label form, S2 in binary form,
     switch the arguments and return the result
     computed by the code  above. */

  if (ALPH_type(s2) == BINARY_VECTOR)
    return(sigma_disjoint_p(s2, s1));

  /* Both S1 and S2 are in label form. Convert the smaller
     one temporarily to binary form and return the answer computed
     by the upper part of this same function. */
      
  if (len1 <= len2)
    {
      label_to_binary(s1);
      i = sigma_disjoint_p(s1, s2);
      binary_to_label(s1);
    }
  else
    {
      label_to_binary(s2);
      i = sigma_disjoint_p(s2, s1);
      binary_to_label(s2);
    }

  return(i);
}


void binary_labels_add_to(ALPHABETptr labels, id_type id)
     /*****
      *
      * Assumes LABELS is a binary_vector alphabet.
      * Adds ID to LABELS (expanding if necessary) without
      * any sort of error checking.  Be careful!
      * Don't use for SIGMA, because this doesn't watch out
      * for EPSILON.
      *
      *****/
{
  if (id >= ALPH_len(labels))
    {
      uint16 max_len = LIM_id_count(LABEL_MAP);
      
      if (id >= max_len)
        handle_error("Unexpected label", "BINARY_LABELS_ADD_TO", id);
      
      adjust_alph_size(labels, id+1);
      
      if (!ALPH_items(labels))
        not_enough_memory("BINARY_LABELS_ADD_TO");
    }
  ALPH_item(labels, id) = 1;
}
#if 0
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

#endif /* 0 */

/**************
 *            *
 *  SYMSUBST  *
 *            *
 **************/

static NETptr sym_or_labsubst_remove (id_type old_sym, NETptr net,
				      int no_connect_p, int label_subst_p)
     /*******
      *
      * Remove all transitions involving old_sym from net.
      * Do not call this function directly.
      * SYMBOL_OR_LABEL_SUBST() is the top level function
      * for symbol substitution.
      *
      *******/
{
  int removed_p, remove_this_arc;
  id_type id;
  int i;
  LABELptr lab;
  STATEptr state;
  ARCptr arc, next_arc, prev_arc, first_arc;
  ALPHABETptr labs = NET_labels(net);
  
  binary_to_label(labs);

  if (label_subst_p)
    {
      /* Remove OLD_SYM from sigma unless some label has it
	 as its component.
	 */

      for (i = 0; i < ALPH_len(labs); i++)
	{
	  id = ALPH_item(labs, i);

	if (LABEL_arity(id_to_label(id)) == 2 &&
	    (upper_id(id) == old_sym || lower_id(id) == old_sym))
	  break;
	 }
 
      /* If we broke the loop, OLD_SYM remains in sigma. */

      if (i == ALPH_len(labs))
	sigma_remove_from(sigma(net), old_sym);
      label_remove_from(NET_labels(net), old_sym);
    }
  else
  {
      sigma_remove_from(sigma(net), old_sym);

      /* The next block of code becam obsolete with the addition
      of the call to update_label_alphabet() at the end, which
      is necessary anyway.
      */
  
/*
         Remove all symbols from the label alphabet that
	 have OLD_SYM as a component (except in TWOL_MODE).
	 


      for (i = 0; i < ALPH_len(labs); i++)
      {
	  id = ALPH_item(labs, i);

	  if (id == old_sym ||
	      (FSM_MODE != TWOL_MODE &&
	       LABEL_arity(id_to_label(id)) > 1 &&
	       (upper_id(id) == old_sym || lower_id(id) == old_sym)))
          {
	       Move all remaining labels forward, if any.

	      for (j = i + 1; j < ALPH_len(labs); j++)
                  ALPH_item(labs, j-1) = ALPH_item(labs, j);

	      ALPH_len(labs)--;
	    
	       May have to reprocess this location.

	      i--;
          }
      }
*/

  }

  removed_p = FALSE;
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      prev_arc = NULL;
      first_arc = NULL;
      
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc)
        {               
          remove_this_arc = FALSE;
          id = ARC_label(arc);
          next_arc = next_arc(arc);
          
          if (id == old_sym)
            remove_this_arc = TRUE;
          else if (!(label_subst_p || FSM_MODE == TWOL_MODE))
            {
              lab = id_to_label(id);
              
              if (LABEL_arity(lab) > 1)
                /* if label is a tuple */
                for (i = 0; i < (int)LABEL_arity(lab); i++)
                  if (TUPLE_labels(LABEL_tuple(lab))[i] == old_sym)
                    {
                      remove_this_arc = TRUE;
                      break;
                    }
            }
          
          if (remove_this_arc == TRUE)
            {
              /* remove arc from chain */
              removed_p = TRUE;
              
              if (prev_arc != NULL)
                next_arc(prev_arc) = next_arc(arc);
              
              /* if there is no previous arc, then ARC is the first arc
                 thus FIRST_ARC has not been set, so the arc will not appear
                 in the final chain headed by FIRST_ARC
                 */
              NET_num_arcs(net)--;
              free_arc(arc);
            }
          else  /* keep arc */
            {
              prev_arc = arc;
              if (first_arc == NULL)
                first_arc = arc;
            }
        }
      STATE_arc_set(state) = first_arc;
    }
  
  /*
   *    ;;; Different patterns of deletion could cause states to become 
   *    ;;; unreachable from the start or folded together or final states
   *    ;;; to become unreachable.  Prune and minimize may not necessarily
   *    ;;; notice disconnected states, so throw them out here.
   */
  if (removed_p == TRUE)
    {
      if (old_sym == EPSILON)
        NET_epsilon_free(net) = TRUE;
      
      if (no_connect_p == FALSE)
        make_connected(net, FALSE);
      
      update_label_alphabet(net);
      NET_minimized(net) = FALSE;
      NET_pruned(net) = FALSE;
    }
  
  return (net);
}

static ALPHABETptr scratch_tup = NULL;

static ALPHABETptr generate_subst(ALPHABETptr subst, id_type old_sym,
                                  ALPHABETptr new_syms, int depth,
				  int other_special_p)
     /******
      * SUPPORT FUNCTION FOR SYMSUBST_MODIFY
      *
      *
      * Recursively generates all the n-tuples created when substituting 
      * NEW_SYMS for OLD_SYM, given the frame SCRATCH_TUP.  SCRATCH_TUP is
      * the tuple that serves as the basis for the generation, with the
      * exception that each instance of OLD_SYM is replaced by (-1).
      *
      * For instance, given the tuple x:y:z,
      * let's substitute each item in the list 
      * (a b c) (NEW_SYMS) for x (OLD_SYM).
      * SCRATCH_TUP initially contains (ID_NO_SYMBOL):y:z.
      * This function recursively creates the tuples
      * a:y:z, b:y:z, and c:y:z, and 
      * places the IDs of these three tuples in SUBST.
      * A later function uses this vector
      * as a recipie for replacing arcs labeled by the tuple x:y:z in a net.
      *
      * If OTHER_SPECIAL_P is TRUE, the symbol OTHER in OTHER:OTHER
      * is preserved. This is required for the correct expansion of
      * OTHER in concatenation, union, etc.
      *

      *
      * Returns SUBST, a list of the id's of the substitution tuples.
      *
      ******/
{
  int i, j, match;
  id_type id, cur_cell, cur_new_sym;
  
  /* if scratch_tup is full, build a tuple, install into subst */
  if (depth == ALPH_len(scratch_tup)) 
    {
      /* increase the length of SUBST */
      adjust_alph_size(subst, ALPH_len(subst) + 1);
      if (!ALPH_items(subst))
        not_enough_memory("SYMSUBST");
      
      /* scan for X:X:X..., but not OTHER:OTHER:OTHER... */
      /* X:X:X gets compressed to just X */
      if (!(other_special_p && ALPH_item(scratch_tup, 0) == OTHER))
        {
          match = TRUE;
          for (i = 1; i < ALPH_len(scratch_tup); i++)
            {
              if (ALPH_item(scratch_tup, i) != ALPH_item(scratch_tup, 0))
                {
                  match = FALSE;
                  break;
                }
            }
          
          if (match == TRUE)
            /* X:X:X... case, so compress to single X */
            {                           
              ALPH_item(subst, ALPH_len(subst) - 1) =
                ALPH_item(scratch_tup, 0);
              return (subst);
            }
          /* else, drop through to next case */
        }
      
      /* intern tuple using the scratch tuple */
      intern_tuple_label(&id, ALPH_items(scratch_tup), ALPH_len(scratch_tup));
      ALPH_item(subst, ALPH_len(subst) - 1) = id;
        
      return (subst);
    }
  else  /* continue building the tuple */
    {
      cur_cell = ALPH_item(scratch_tup, depth);
      
      if (cur_cell != ID_NO_SYMBOL)      /* cell is not a substitution cell */
        return (generate_subst(subst, old_sym, new_syms, depth + 1,
			       other_special_p));
      else
        {
          for (i = 0; i < ALPH_len(new_syms); i++)
            {
              cur_new_sym = ALPH_item(new_syms, i);
              
              /* no matches are allowed when substituting
                 for OTHER, except for OTHER:OTHER
                 */
              if (other_special_p && old_sym == OTHER && cur_new_sym != OTHER)
                {
                  match = FALSE;
                  for (j = 0; j < ALPH_len(scratch_tup); j++)
                    {
                      if (ALPH_item(scratch_tup, j) == cur_new_sym)
                        {
                          match = TRUE;
                          break;
                        }
                    }
                  
                  if (match == FALSE)   
                    {
                      ALPH_item(scratch_tup, depth) = cur_new_sym;
                      subst = generate_subst(subst, old_sym, new_syms,
                                             depth + 1, other_special_p);
                    }
                }
              else
                {
                  ALPH_item(scratch_tup, depth) = cur_new_sym;
                  subst = generate_subst(subst, old_sym, new_syms, depth + 1,
					 other_special_p);
                }
              ALPH_item(scratch_tup, depth) = ID_NO_SYMBOL;               
            }
          return (subst);
        }
    }
}

static void prepare_scratch_tuple(LABELptr l, id_type old_sym)
     /*****
      * SUPPORT FUNCTION FOR SYMSUBST_MODIFY
      *
      *
      * Initializes the global value SCRATCH_TUP to the
      * specifications described in the generate_subst() function (above).
      *
      *****/
{
  int i;
  
  if (scratch_tup == NULL)
    /* 0: alph type doesn't matter here */
    scratch_tup = make_alph(LABEL_arity(l), 0);
  else
    adjust_alph_size(scratch_tup, LABEL_arity(l));
  
  if (!ALPH_items(scratch_tup))
    not_enough_memory("SYMSUBST_MODIFY");
  
  for (i = 0; i < ALPH_len(scratch_tup); i++)
    if (TUPLE_labels(LABEL_tuple(l))[i] != old_sym)
      ALPH_item(scratch_tup, i) = TUPLE_labels(LABEL_tuple(l))[i];
    else
      ALPH_item(scratch_tup, i) = ID_NO_SYMBOL;
}

static ALPHABETptr build_subst(id_type id, id_type old_sym, ALPHABETptr new_syms,
			       int other_special_p)

     /******
      * SUPPORT FUNCTION FOR SYMSUBST_MODIFY
      *
      * Builds an entry in the substitution table for the label
      * with number ID.  If ID label matchs OLD_SYM, then the
      * substitution is direct.  If ID is a tuple containing
      * OLD_SYM, then generate_subst() is called to generate
      * all of the possible substitutions for the label.
      * If the label does not contain or match OLD_SYM, NULL is returned.
      *
      ******/
{
  LABELptr l;
  int match, i;
  ALPHABETptr subst;
  
  if (id == old_sym)
    /* OLD_SYM maps to NEW_SYMS directly */
    return (new_syms);
  else if (FSM_MODE != TWOL_MODE)
    {
      l = id_to_label(id);
      
      if (LABEL_arity(l) > 1)
        /* unpack tuple, search for match */
        {
          match = FALSE;
          for (i = 0; i < (int)LABEL_arity(l); i++)
            if (TUPLE_labels(LABEL_tuple(l))[i] == old_sym)
              {
                match = TRUE;
                break;
              }
          
          if (match == TRUE)
            {   
              prepare_scratch_tuple(l, old_sym);
              
              subst = make_alph(0, 0);
              
              if (!subst)
                not_enough_memory("SYMSUBST_MODIFY");
              
              generate_subst(subst, old_sym, new_syms, 0, other_special_p);
              
              /* Free scratch_tup always so that it does not show up
                 as a stray unreclaimed alphabet in the storage
                 listing.
                 */
              free_alph(scratch_tup);
              scratch_tup = NULL;

              return(subst);
            }
          else
            /* Label does not contain old_sym,
               no substitution in this case
               */
            return (NULL);
        }
      else
        /* label is atomic, does not match old_sym.
           no substitution in this case
           */
        return (NULL);
    }
  else
    return(NULL);
}

static int subst_table_len;
static ALPHABETptr *build_subst_table(id_type old_sym, ALPHABETptr new_syms,
				      NETptr net, int other_special_p)
     /*****
      * SUPPORT FUNCTION FOR SYMSUBST_MODIFY
      *
      * Builds a substitution table which contains the IDs of the 
      * replacement(s) label(s) for each label in NET containing OLD_SYM.
      *
      *****/
{
  ALPHABETptr *subst_table;
  int i;
  
  /* NET_labels(net) are BINARY_VECTOR form at this point */
  subst_table_len = LIM_id_count(LABEL_MAP);
  subst_table = (ALPHABETptr *) 
    calloc((size_t) subst_table_len, (size_t) sizeof (ALPHABETptr));
  
  if (!subst_table)
    not_enough_memory("SYMSUBST");
  
  for (i = 0; i < ALPH_len(NET_labels(net)); i++)
    if (binary_sigma_member(NET_labels(net), i))
      subst_table[i] = build_subst(i, old_sym, new_syms, other_special_p);
  
  /* Just in case EPSILON label is not in NET_labels(net). */
  
  if (old_sym == EPSILON && !binary_sigma_member(NET_labels(net), EPSILON))
    subst_table[EPSILON] = build_subst(EPSILON, old_sym, new_syms,
				       other_special_p);
  
  return (subst_table);
}

static void free_subst_table(ALPHABETptr *subst_table, int len,
                             ALPHABETptr new_syms)
     /*****
      * SUPPORT FUNCTION FOR SYMSUBST (_MODIFY)
      * 
      * Releases memory occupied by the substitution table.
      *
      ******/
{
  int i;
  
  for (i = 0; i < len; i++)
    if (subst_table[i] != NULL && subst_table[i] != new_syms)
      {
        free_alph(subst_table[i]);
        subst_table[i] = NULL;
      }
  free((void *) subst_table);
}

static int keep_in_sigma_p(id_type old_id, ALPHABETptr labs, id_type ignore_id)
{
  /* Returns TRUE if OLD_ID appears as a component of some label
     in labs, ignoring IGNORE_ID */

  uint16 i, len = ALPH_len(binary_to_label(labs));
  id_type id;

  for (i = 0; i < len; i++)
    {
      id = ALPH_item(labs, i);
      
      if (id != ignore_id)
	{
	  if (LABEL_arity(id_to_label(id)) == 2)
	    {
	      if (upper_id(id) == old_id || lower_id(id) == old_id)
		break;
	    }
	  else if (id == old_id)
	    break;
	}
    }
 
  /* If we broke early a match was found */

  return (i < len);
}

static NETptr labelsubst_modify(id_type old_lab, ALPHABETptr new_labs, NETptr net)
     /*****
      * SUPPORT FUNCTION FOR SYMBOL_OR_LABEL_SUBST
      *
      * Every arc in NET with a label containing OLD_LAB is replaced
      *  by one or more arcs containing an element of NEW_LABS instead of
      *  OLD_LAB.
      *
      *  Returns the altered network.
      *
      ******/
{
  ALPHABETptr sig = NET_sigma(net), labs = NET_labels(net);
  int not_deterministic_if_changed_p = FALSE;
  int epsilon_label_p, network_changed_p = FALSE, state_changed;
  id_type id, upper, lower;
  int i, len;
  int new_disjoint_p, new_labs_copy_p = FALSE;
  STATEptr state;
  ARCptr a, next;
  
  /* If NEW_LABS and SIG are the same, we must copy NEW_LABS. */

  if (sig == new_labs)
    {
      new_labs = copy_alphabet(new_labs);
      new_labs_copy_p = TRUE;
    }

  label_to_binary(labs);

  len = ALPH_len(new_labs);

  new_disjoint_p = sigma_disjoint_p (NET_labels(net), new_labs);
  
  epsilon_label_p = sigma_member(new_labs, EPSILON);
  
  if (epsilon_label_p || !new_disjoint_p)
    not_deterministic_if_changed_p = TRUE;  
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      state_changed = FALSE;

      for (a = STATE_arc_set(state); a != NULL; a = next)
        {
          id = ARC_label(a);
          next = next_arc(a);
          
	  if (id == old_lab)
	    {
	      /* Reuse the old arc for the first substitution */

	      ARC_label(a) = ALPH_item(new_labs, 0);

	      for (i = 1; i < len; i++)
		{
#ifdef FULL_FSM
		  add_new_arc(net, state, ALPH_item(new_labs, i),
			      ARC_destination(a), ARC_user_pointer(a));
#else
		  add_new_arc(net, state, ALPH_item(new_labs, i),
			      ARC_destination(a));
#endif                      
		}

	      state_changed = TRUE;
	    }
	}

      if (state_changed == TRUE)
        {
          if (not_deterministic_if_changed_p == TRUE)
            STATE_deterministic(state) = FALSE;
          network_changed_p = TRUE;     
        }
    }
  
  if (network_changed_p == TRUE)
    {
      /* Fix network flags */

      NET_completed(net) = FALSE;
      if (old_lab == EPSILON)
        NET_epsilon_free(net) = TRUE;   
      
      if (not_deterministic_if_changed_p == TRUE)
        {
          if (epsilon_label_p == TRUE)
            NET_epsilon_free(net) = FALSE;
          NET_deterministic(net) = FALSE;
          NET_minimized(net) = FALSE;
        }

      /* Remove the old_lab from the label alphabet */

      label_remove_from(labs, old_lab);

      /* Remove its components from sigma, if they don't occur
         in other labels. */

      if (FSM_MODE == TWOL_MODE)
	sigma_remove_from(sig, old_lab);
      else if (LABEL_arity(id_to_label(old_lab)) == 1)
	{
	  /* Remove OLD_LAB from sigma unless some other label has it
	     as its component.
	  */

	  if (!(keep_in_sigma_p(old_lab, labs, old_lab)))
	    sigma_remove_from(sigma(net), old_lab);
	}
      else
	{
	  /* OLD_LAB is a symbol pair. Remove its components
	     from SIGMA unless they appear as components of some
	     other label. */

	  if (!(keep_in_sigma_p(upper_id(old_lab), labs, old_lab)))
	    sigma_remove_from(NET_sigma(net), upper_id(old_lab));

	  if (!(keep_in_sigma_p(lower_id(old_lab), labs, old_lab)))
	    sigma_remove_from(NET_sigma(net), lower_id(old_lab));
	}

      /* Add the new labels to the label alphabet and their components
	 to the sigma.
      */

      for (i = 0; i < len; i++)
	{
	  id = ALPH_item(new_labs, i);
      
	  upper = upper_id(id);
	  lower = lower_id(id);

	  sigma_add_to(sig, upper);
	  sigma_add_to(sig, lower);

	  sigma_add_to(labs, id);
	}


    }
  
  /* If NEW_LABS was copied, reclaim the copy. */

  if (new_labs_copy_p)
    free_alph(new_labs);

  return (net);
}

static NETptr symsubst_modify(id_type old_sym, ALPHABETptr new_syms, NETptr net,
			      int other_special_p)
     /*****
      * SUPPORT FUNCTION FOR SYMBOL_OR_LABEL_SUBST
      *
      * Every arc in NET with a label containing OLD_SYM is replaced
      *  by one or more arcs containing an element of NEW_SYMS instead of
      *  OLD_SYM.  The strategy is to first build a table of substitution 
      *  labels for each label in NET containing OLD_SYM.  Then, 
      *  we make one sweep through all the arcs in NET, looking up the
      *  appropriate substition on the table.  
      *
      *  Returns the altered network.
      *
      ******/
{
  ALPHABETptr *subst_table, subst, sig = NET_sigma(net);
  int not_deterministic_if_changed_p = FALSE;
  int epsilon_label_p, network_changed_p = FALSE, state_changed;
  int i, new_disjoint_p, new_syms_copy_p = FALSE;
  id_type id;
  STATEptr state;
  ARCptr a, next, prev;
  
  /* If NEW_SYMS and SIG are the same, we must copy NEW_SYMS. */

  if (sig == new_syms)
    {
      new_syms = copy_alphabet(new_syms);
      new_syms_copy_p = TRUE;
    }

  label_to_binary(NET_labels(net));
  
  /* Exclude the OLD_SYM when calculating whether
     NEW_SYMS is distinct from the old sigma.
     */
  
  sigma_remove_from(sig, old_sym);
  new_disjoint_p = sigma_disjoint_p (sig, new_syms);
  sigma_union_dont_copy(sig, new_syms);
  
  epsilon_label_p = binary_sigma_member(sig, EPSILON);
  
  /* EPSILON does not belong to SIGMA */
  if (epsilon_label_p)
    sigma_remove_from(sig, EPSILON);
  
  if (epsilon_label_p || !new_disjoint_p)
    not_deterministic_if_changed_p = TRUE;  
  
  subst_table = build_subst_table(old_sym, new_syms, net, other_special_p);
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      state_changed = FALSE;
      for (a = STATE_arc_set(state); a != NULL; a = next)
        {
          id = ARC_label(a);
          next = next_arc(a);
          
          if (subst_table[id])
            /* if NULL, there is no change for this label */
            {
              subst = subst_table[id];
              
              sigma_remove_from(NET_labels(net), ARC_label(a));
              
              if (ALPH_len(subst) > 0)
                {
                  ARC_label(a) = ALPH_item(subst, 0);
                  /* reuse the current arc */
                  sigma_add_to(NET_labels(net),  ALPH_item(subst, 0));
                  
                  for (i = 1; i < ALPH_len(subst); i++)
                    {
#ifdef FULL_FSM
                      add_new_arc(net, state, ALPH_item(subst, i),
                                  ARC_destination(a), ARC_user_pointer(a));
#else
                      add_new_arc(net, state, ALPH_item(subst, i),
                                  ARC_destination(a));
#endif                      
                      sigma_add_to(NET_labels(net), ALPH_item(subst, i));
                    }
                }
              else      /* zero length substitution --> delete arc */
                {
                  if (STATE_arc_set(state) == a)
                    STATE_arc_set(state) = next_arc(a);
                  else
                    {
                      prev = STATE_arc_set(state);
                      
                      while (next_arc(prev) != a)
			prev = next_arc(prev);
                  
                      next_arc(prev) = next;
                    }
                  NET_num_arcs(net)--;
                  free_arc(a);
		  NET_pruned(net) = FALSE;
                }
              state_changed = TRUE;
            }
        }
      
      if (state_changed == TRUE)
        {
          if (not_deterministic_if_changed_p == TRUE)
            STATE_deterministic(state) = FALSE;
          network_changed_p = TRUE;     
        }
    }
  
  free_subst_table(subst_table, subst_table_len, new_syms);
  
  if (network_changed_p == TRUE)
    {
      NET_completed(net) = FALSE;
      if (old_sym == EPSILON)
        NET_epsilon_free(net) = TRUE;

      /* If OLD_SYM is a flag diacritic, the flag register (if any)
	 of the net is now invalid. */

      if (flag_diacritic_p(old_sym))
	{
	  free_alph(NET_flag_register(net));
	  NET_flag_register(net) = NULL;
	}
      
      if (not_deterministic_if_changed_p == TRUE)
        {
          if (epsilon_label_p == TRUE)
            NET_epsilon_free(net) = FALSE;
          NET_deterministic(net) = FALSE;
          NET_minimized(net) = FALSE;
        }
    }
  
  /* If NEW_SYMS was copied, reclaim the copy. */

  if (new_syms_copy_p)
    free_alph(new_syms);

  return (net);
}

int occurs_as_label_component_p(id_type id, ALPHABETptr labs)
{
  /* Returns TRUE iff id occurs in LABS  as a component of another label. */

  uint16 i;
  id_type lab_id;
  int restore_binary_p = (ALPH_type(labs) == BINARY_VECTOR);
  int result;

  /* An fstpair cannot be a component of another label! */

  if (LABEL_arity(id_to_label(id)) == 2)
    return(FALSE);

  binary_to_label(labs);

  for (i = 0; i < ALPH_len(labs); i++)
    {
      lab_id = ALPH_item(labs, i);

      if (LABEL_arity(id_to_label(lab_id)) == 2 &&
	  (upper_id(lab_id) == id || lower_id(lab_id) == id))
	break;
    }

  /* If we broke the loop, a match was found */

  result = (i < ALPH_len(labs));

  if (restore_binary_p)
    label_to_binary(labs);

  return(result);
}




static NETptr symbol_or_label_subst(id_type old_sym, ALPHABETptr new_syms,
				    NETptr net, int copy_p,
				    int no_connect_p, int label_subst_p,
				    int other_special_p)
{
  /*****
       SYMBOL_OR_LABEL_SUBST substitutes the alphabet for all
       occurrences of OLD_SYM in NET.  If NEWSYMS = NULL, removes any
       transitions involving OLD_SYM. If LABEL_SUBST_P is TRUE,
       OLD_SYM may be either a simple symbol or a tuple, if LABEL_SUBST_P
       is FALSE, OLD_SYM must be a simple symbol (unless we are in
       TWOL_MODE). In label substitution mode, an arc label is changed
       if it matches OLD_SYM, in symbol substitution mode arc labels
       change if one of the components matches OLD_SYM.
       If OTHER_SPECIAL_P is TRUE, no substitution is allowed for
       OTHER:OTHER. OTHER:OTHER must be preserved in the expansion
       of other. If OTHER_SPECIAL_P is FALSE, OTHER:OTHER has no
       special status. For example in substituting X for OTHER,
       OTHER:OTHER is replaced by X.
       *****/

  /* coerce the various label vectors into the correct format */

  NET_sigma(net) = label_to_binary(sigma(net));
  binary_to_label(new_syms);

  if (!(label_subst_p || FSM_MODE == TWOL_MODE))
    {
      /* Make sure OLD_SYM and NEW_SYMS are not fstpairs */
      
      int i, len, error_p = FALSE;
      
      if (LABEL_arity(id_to_label(old_sym)) > 1)
        {
          fsm_fprintf(stderr, "\nCannot substitute for an fstpair.\n");
          error_p = TRUE;
        }
      
      if (new_syms != NULL)
        {
          len = ALPH_len(new_syms);
          
          for (i = 0; i < len; i++)
            if (LABEL_arity(id_to_label(ALPH_item(new_syms, i))) > 1)
              {
                fsm_fprintf(stderr, "\nCannot substitute an fstpair.\n");
                error_p = TRUE;
                break;
              }
          if (error_p)
            return(net);
        }
    }

  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  if (sigma_member(NET_sigma(net), old_sym) ||
      sigma_member(NET_labels(net), old_sym) ||
      (old_sym == EPSILON &&
       occurs_as_label_component_p(old_sym, NET_labels(net))))
    {
      if (new_syms == NULL || ALPH_len(new_syms) == 0)
        /* remove all transitions involving old_sym */
	sym_or_labsubst_remove(old_sym, net, no_connect_p, label_subst_p);
      else
	/* real case */
	if (ALPH_len(new_syms) > 1 || old_sym != ALPH_item(new_syms, 0))
	  {
	    if (label_subst_p)
	      labelsubst_modify(old_sym, new_syms, net);
	    else
	      symsubst_modify(old_sym, new_syms, net, other_special_p);
	  }
     }
  /* restore the label vector */
  
  binary_to_label(NET_labels(net));


  /* Old parse tables may not be valide anymore. This could be done
     more carefully because we know what has changed.
     */

  free_net_parse_tables(net);
  
  check_for_flags(net);

  return (net);
}


NETptr symsubst(id_type old_sym, ALPHABETptr new_syms, NETptr net,
                int copy_p, int no_connect_p)
{
  /* The original version of this function assumed that OLD_SYM is a
     simple symbol, not an fstpair. The new version of the function,
     now called SYMBOL_OR_LABEL_SUBST works on both symbols and labels
     It allows OTHER to be treated like an ordinary symbol.
     Here we call it with LABEL_SUBST_P == FALSE, OTHER_SPECIAL_P == TRUE
     to make it behave like the old SYMSUBST
  */

  return(symbol_or_label_subst(old_sym, new_syms, net, copy_p,
			       no_connect_p, FALSE, TRUE));
}


NETptr substitute_symbol(id_type old_sym, ALPHABETptr new_syms, NETptr net,
			 int copy_p, int no_connect_p)
{

  /* Replacement for the old symsubst with a better name with
     no special treatment for OTHER. If OLD_SYM is OTHER,
     OTHER:OTHER becomes a single symbol.
     */

  return(symbol_or_label_subst(old_sym, new_syms, net, copy_p,
			       no_connect_p, FALSE, FALSE));
  
}


NETptr substitute_symbol_alph(ALPHABETptr old_syms, ALPHABETptr new_syms,
			     NETptr net, int copy_p, int no_connect_p)
{
  int i;

  binary_to_label(old_syms);

  for (i = 0; i < ALPH_len(old_syms); i++)
    symbol_or_label_subst(ALPH_item(old_syms, i), new_syms, net,
			  copy_p, no_connect_p, FALSE, FALSE);
  return(net);
}

NETptr substitute_label(id_type old_sym, ALPHABETptr new_syms, NETptr net,
			int copy_p, int no_connect_p)
{
  return(symbol_or_label_subst(old_sym, new_syms, net, copy_p,
			       no_connect_p, TRUE, FALSE));
}

NETptr substitute_label_alph(ALPHABETptr old_syms, ALPHABETptr new_syms,
			     NETptr net, int copy_p, int no_connect_p)
{
  int i;

  binary_to_label(old_syms);

  for (i = 0; i < ALPH_len(old_syms); i++)
    symbol_or_label_subst(ALPH_item(old_syms, i), new_syms, net,
			  copy_p, no_connect_p, TRUE, FALSE);
  return(net);
}


/***************************
 *
 *   Interactive SYMSUBST
 *
 ***************************/
 
static int label_name_from_stream(char *name, int lim, FILE *stream)
{
  /* Reads at most LIM characters from LINE to make up a symbol
     name. The only special characters for this function are the
     whitespace characters ' ', '\t', and '\n' that can
     terminate a symbol name and '%' that acts as an escape
     character in front of a whitespace character. Returns 1
     if the last character processed was a linebreak that is
     not part of a symbol name of an END-OF-FILE. Otherwise
     returns 0. Note that % is removed only when a white
     space character follows. %0, %%, etc. pass through
     unchanged. The value returned is designed to be converted
     to a symbol id by the function INTERN_SYMBOL that removes
     the escapes protecting %, 0, and ? when they are used
     without the special meaning.
     */
     
  int in, n = 0;
  char *sym = name, prev, ch = '\0';

 /* Eliminate initial whitespace */

  do {
  in = fsm_fgetc(stream);
  } while ((char) in == ' '|| (char) in == '\t');
  
  if (in == '\n')
    {
      *sym = '\0';
      return(TRUE);
    }

  prev = (char) in;
  in = fsm_fgetc(stream);
  
  if (!(in == EOF))
    do {
      /* Collect characters until an unquoted whitespace character has
	   been encountered.
	   */
	  
	  ch = (char) in;
	  
      if (prev == '%' && (ch == ' ' || ch == '\t' || ch == '\n'))
        {
          /* Escape before a delimiter. Do not break. */
          
          prev = ch;
          
          in = fsm_fgetc(stream);
          ch = (char) in;
        }
      
      if (n < lim)
        sym[n++] = prev;
      else
        break;
      
      if (ch == ' ' || ch == '\t' || ch == '\n')
        break;
      
      prev = ch;
      in = fsm_fgetc(stream); 
     } while (!(in == EOF));
   else
     if (n < lim)
       sym[n++] = prev;


   sym[n] = '\0';

    if (n < lim)
      return((ch == '\n' || in == EOF));
    else
      handle_error("String too long", "LABEL_NAME_FROM_STREAM", n);
  return(FALSE);
}


static ALPHABETptr sym_line_to_alph(FILE *stream)
{
  /* Support for STRING_SYMSUBST.  Converts a string of symbols to
     a binary alphabet for SYMSUBST.
     */
  
  int eol_p/*, type = -1*/ ;
  char name[80];
  ALPHABETptr alph;
  id_type id;
  
  alph = make_alph(LIM_id_count(LABEL_MAP), BINARY_VECTOR);
  
  do  {
    
    eol_p = label_name_from_stream(name, 80, stream);
    
    if (*name == '\0')
      break;
      
    intern_symbol(&id, name);
    
    /* SIGMA_ADD_TO does not add EPSILON to sigma but here we
       need to do that. */
    
    if (id == EPSILON)
      ALPH_item(alph, id) = 1;
    else
      sigma_add_to(alph, id);
  }  while (!eol_p);
  
  binary_to_label(alph);
  return (alph);
}


static NVptr stream_symsubst(char *old_sym, id_type label_id, FILE *stream,
					NVptr nets, int minimize_p)
{
  /* Converts the string OLD_SYM to a symbol ID and reads a line
     of NEW_SYMS from STREAM converting them to a binary alphabet
     to use as argument in calling SYMSUBST on NETS.
     The type and package of the symbols are deduced from NETS;
     failing that, they default to Symbol and KEYWORD.
     */
  
  int i;
  NETptr net;
  int net_count = 0;
  long int arc_count, pos_tally, arc_tally = 0;
  
  if (label_id != ID_NO_SYMBOL)
    {
      ALPHABETptr labels = sym_line_to_alph(stream);
      
      if ((net_count = NV_len(nets)) == 0)
	{
	  fsm_fprintf(stderr, "No nets. Symbol substitute aborted.\n");
	  return(nets);
	}

      for (i = 0, net = NV_net(nets, 0);
           i < net_count;
           i++, net = NV_net(nets, i))
        {
          arc_count = NET_num_arcs(net);
          symsubst(label_id, labels, net, DONT_COPY, FALSE);
          if (minimize_p == DO_MINIMIZE)
            min_fsm(net, DONT_COPY);
          arc_tally += NET_num_arcs(net) - arc_count;
        }
      
      if (*old_sym == '%' && *(old_sym + 1) != '\0')
	old_sym++;

      if (ALPH_len(labels) == 0)
        fsm_fprintf(stderr, "'%s' deleted.\n", old_sym);
      else
        fsm_fprintf(stderr, "'%s' replaced.\n", old_sym);
      
      if (arc_tally < 0)
	pos_tally = 0 - arc_tally;
      else
	pos_tally = arc_tally;
          
      fsm_fprintf(stderr, "%ld", pos_tally);
      if (pos_tally == 1)
	fsm_fprintf(stderr, " arc");
      else
	fsm_fprintf(stderr, " arcs");
          
      if (arc_tally < 0)
	fsm_fprintf(stderr, " removed");
      else
	fsm_fprintf(stderr, " added");
      
      if (net_count > 1)
        fsm_fprintf(stderr, " in %d nets.\n", net_count);
      else
	fsm_fprintf(stderr, ".\n");
      
      free_alph(labels);
    }
  else
    {
      if (old_sym[0] == '%' && old_sym[1] != '\0')
        old_sym++;
      fsm_fprintf(stderr, "No changes. Symbol '%s' does not exist.\n", old_sym);
    }
  
  return(nets);
}

#if NOT_USED
static void maybe_remove_escape(char *str)
{
  /* Remove escape symbol unless it is protecting ? or 0. */

  if (*str == '%' && *(str + 1) != '\0' &&
      *(str +1) != '?' && *(str + 1) != '0')
    {
      char *tail = str;
      while (*tail)
	{
	  *tail = *(tail + 1);
	  tail++;
	}
    }
}


static void check_for_escapes(char *name)
{
  /* Check for redundant escapes and remove them. */

  while (*name)
    {
      maybe_remove_escape(name);
      name++;
    }

}

static void parse_label_name(char *name, char **upper, char **lower)
{
  if (!(strchr(name, ':')))
    {
      /* An atomic symbol. */
      *upper = name;
      *lower = name;
      return;
    }
  else
    {
      /* Probably an fstpair. Must check. */

      char *rest = name, *tail;

      *upper = NULL;
      *lower = NULL;

      while (*rest)
	{
	  if (*rest == '%')
	    maybe_remove_escape(rest);
	  else if (*rest == ':')
	    {
	      /* Found a breakpoint. */

	      if (rest != name)
		{
		  /* Upper symbol is defined. */

		  *rest = '\0';
		  *upper = name;
		}

		
	      if (*(rest + 1))
		{
		  /* Lower symbol is defined. */

		  *lower = rest;
		    
		  /* Check for illegal occurrences of colon.*/

		  tail = rest;

		  while (*tail)
		    {
		      if (*tail == '%' && *(tail + 1))
			tail++;
		      else if (*tail == ':')
			{
			  *rest = ':';
			  fsm_fprintf(stderr, "*** Warning: '%s' cannot ", name);
			  fsm_fprintf(stderr, "be unambiguously interpreted.\n");
			  *rest = '\0';
			  fsm_fprintf(stderr, "  Upper symbol = '%s', ", *upper);
			  fsm_fprintf(stderr, "lower symbol = '%s'\n", *lower);
			  break;
			}
		      tail++;
		    }
		  /* Remove redundant escapes, if any. */
		  check_for_escapes(*lower);
		  
		}
	      return;
	    }
	  rest++;
	}

      /* No breakpoint found. It is a single symbol, after all */

      *upper = *lower = name;
    }
}
#endif /* NOT_USED */

int interactive_substitute(FILE *in_stream, NVptr nets, int minimize_p)
{
  /* Interactive version of SYMSUBST. Gets a symbol from STREAM that is
     to be replaced in all nets in NETS by a list of symbols given on the
     the same or the following line following line (or lines, if one of the
     replacement symbols is a linefeed. Special characters that have to
     be prefixed by the escape character % are (1) whitespace characters
     (\s, \t, \n), EPSILON (0) and OTHER (?).
     */
  
  static char name[80];
  int count = global_alph_size(), error= NO_ERROR;
  id_type label_id;
  
  fsm_fprintf(stdout, "Old symbols: ");
  if (isatty(fileno(in_stream)))
    fflush(in_stream);
  
  label_name_from_stream(name, 80, in_stream);
  
  if (*name == '\0')
    fsm_printf("No changes.\n");
  else
    {
      error= intern_symbol(&label_id, name);
      if (error!= NO_ERROR)
	return(error);

      if (label_id <= count)
        {
          fsm_fprintf(stdout, "New symbols: ");
          stream_symsubst(name, label_id, in_stream, nets, minimize_p);
        }
      else
        {
          char *sym = name;
          
          if (name[0] == '%' && name[1] != '\0')
            sym++;
          fsm_fprintf(stdout, "Symbol '%s' does not exist.\n", sym);
        }
    }

return(error);
}

/********************
 *
 *   Splicing
 *
 ********************/


static int has_arcs_p(STATEptr state)
     /* keep_pred for delete_states */
{
  if (STATE_arc_set(state) == NULL)
    return (0);
  else
    return (1);
}

static void splice_arcs (STATEptr state, ARCptr arcs, NETptr net,
                         int copy_p, int *label_vector)
{
  /* Adds ARCS or copies of them to STATE in NET.  Uses LABEL_VECTOR to keep
     track of labels if STATE is deterministic. Also keeps track of epsilon
     arcs and loops. LABEL_VECTOR should be an array or 0's long enough for
     the largest label id.
     */
  
  if (arcs == NULL)
    return;
  else
    {
      ARCptr arc, state_arcs = STATE_arc_set(state);
      ARCptr first_arc = NULL, last_arc = NULL, next_arc;
      int det_p = STATE_deterministic(state);
      id_type id;
      
      /* If the state is deterministic, set label vector. */
      if (det_p)
        for (arc = state_arcs; arc != NULL; arc = next_arc(arc))
          label_vector[ARC_label(arc)] = 1;
      
      /* Find the last arc of the set and look out for epsilons and loops. */
      for (arc = arcs; arc != NULL; arc = next_arc(arc))
        {
          id = ARC_label(arc);
          
          if (state == ARC_destination(arc))
            NET_loop_free(net) = FALSE;
          
          if (id == EPSILON)
            {
              NET_epsilon_free(net) = FALSE;
              STATE_deterministic(state) = FALSE;
            }
          
          if (det_p && label_vector[id] == 1)
            STATE_deterministic(state) = FALSE;
          else
            label_vector[id] = 1;
          
          if (copy_p == DO_COPY)
            next_arc = copy_arc(arc, net);
          else
            next_arc = arc;
          
          if (first_arc == NULL)
            first_arc = next_arc;
          else
            next_arc(last_arc) = next_arc;
          
          last_arc = next_arc;
        }
      
      /* Push the new arcs to the beginning
         of STATE's arc set. */
      
      next_arc(last_arc) = state_arcs;
      STATE_arc_set(state) = first_arc;
      
      /* If we used label_vector, clear it for the next round. */
      if (det_p)
        for (arc = first_arc; arc != NULL; arc = next_arc(arc))
          label_vector[ARC_label(arc)] = 0;
      
      /* Update NET properties. */
      if (STATE_deterministic(state) == FALSE)
        {
          NET_deterministic(net) = FALSE;
          NET_minimized(net) = FALSE;
        }
    }
}

static int find_label(id_type label, ARCptr arcs)
{
  /* Returns TRUE if LABEL occurs in ARCS, otherwise FALSE */
  
  ARCptr arc;
  
  for (arc = arcs; arc != NULL; arc = next_arc(arc))
    if (ARC_label(arc) == label)
      return(TRUE);
  
  return(FALSE);
}

NETptr splice_fsm (NETptr target, STATEptr from_state, STATEptr to_state,
                   NETptr insert, int expand_other_p, id_type beware_label)
{
  
  /* SPLICE_FSM splices INSERT from FROM_STATE to TO_STATE in TARGET.
     Physically  modifies both TARGET and INSERT, so caller must  copy
     if desired.  If FROM_STATE and TO_STATE are distinct and
     TO_STATE contains an arc with BEWARE_LABEL, instead of copying
     TO_STATE arcs to FROM_STATE or to the final states of INSERT,
     an EPSILON arc to TO_STATE is introduced.  This is important
     in calls from NETSUBST.
     To run the toutine with no BEWARE_LABEL, set the last parameter 
     to ID_NO_SYMBOL.
     */
  
  prune_fsm(insert, DONT_COPY);
  
  if (expand_other_p == TRUE)
    align_sigmas(target, insert);
  
  if (!(null_fsm_p(insert)))
    {
      STATEptr insert_start = NET_start_state(insert);
      int *label_vector = (int *)
        calloc((size_t) LIM_id_count(LABEL_MAP), (size_t) sizeof(int));
      
      if (STATE_arc_set(insert_start))
        {
          
          STATEptr state, last_state;
          ARCptr arc, next_arc, insert_start_arcs = NULL, last_arc = NULL;
          int empty_final = FALSE, keep_start_state_p = start_loops_p(insert);
          int insert_start_final_p = STATE_final(insert_start);
          int add_epsilon_p = -1;
          
          
          /* If the start state of INSERT is final and TO_STATE is not
             FROM_STATE, push copies of TO_STATE arcs to the beginning
             of FROM_STATE's arc_set when BEWARE_LABEL does not occur
             in TO_STATE.  If it does, add an epsilon arc to TO_STATE
             instead.  (Important for NETSUBST.)
             */
          
          if (from_state != to_state && insert_start_final_p)
            {
              if (beware_label != ID_NO_SYMBOL)
                add_epsilon_p =
                  find_label(beware_label, STATE_arc_set(to_state));
              else
                add_epsilon_p = FALSE;
              
              if (add_epsilon_p)
                {
                  push_arc(make_arc(EPSILON, to_state, target), from_state);
                  STATE_deterministic(from_state) = FALSE;
                }
              else
                {
                  if (STATE_final(to_state))
                    STATE_final(from_state) = TRUE;
                  
                  splice_arcs(from_state, STATE_arc_set(to_state), target,
                              DO_COPY, label_vector);
                }
            }
          
          /* Make INSERT_START_ARCS chain that contains the arcs of
             INSERT_START.
             If KEEP_START_STATE_P is TRUE, the chain is made of copies of
             the original arcs.  These arcs will be spliced to the arc set of
             FROM_STATE If the destination of an arc is a non-empty final
             state,  reroute the copy to TO_STATE because the original will
             eventually  be rerouted there. Don't add the arcs to FROM_STATE
             until INSERT has been processed.
             */
          
          if (keep_start_state_p)
            for (arc = STATE_arc_set(insert_start);
                 arc != NULL;
                 arc = next_arc(arc))
              {
                STATEptr dest = ARC_destination(arc);
                
                if (STATE_final(dest) && STATE_arc_set(dest) == NULL)
                  dest = to_state;
                
                next_arc = make_arc(ARC_label(arc), dest, target);
                
                if (insert_start_arcs == NULL)
                  insert_start_arcs = next_arc;
                else
                  next_arc(last_arc) = next_arc;
                
                last_arc = next_arc;
              }
          else
            insert_start_arcs = STATE_arc_set(insert_start);
          
          /* Find the final states of INSERT.
             
             If a final state contains no arcs, take a note of it,
             and reroute arcs leading to it to TO_STATE instead.
             
             If a final state contains arcs and is not a start state
             that is about to be eliminated, make it have the same
             finality as TO_STATE. Copy the arcs of TO_STATE
             to the state when TO_STATE doesn't contain an arc with
             BEWARE_LABEL; if it does, add an epsilon arc to TO_STATE
             If FROM_STATE and TO_STATE are identical, add the arcs from
             the start state of INSERT unless that is the state itself.
             */         
          
          for (state = NET_states(insert);
               state != NULL;
               state = next_state(state))
            if (STATE_final(state))
	      {
		if (STATE_arc_set(state) == NULL)
		  {
		    /* Reroute arcs leading to an empty final state to TO_STATE
		       instead. Do it here rather than later so that we don't
		       confuse original INSERT arcs to arcs that are being
		       copied from TO_STATE.
		       */
		    STATEptr s;
                  
		    for (s = NET_states(insert); s != NULL; s = next_state(s))
		      for (arc = STATE_arc_set(s);
			   arc != NULL;
			   arc = next_arc(arc))
			if (ARC_destination(arc) == state)
			  ARC_destination(arc) = to_state;
                  
		    empty_final = TRUE;
		  }               
		else if (state != insert_start || keep_start_state_p)
		  {
		    /* If we haven't already checked TO_STATE arcs for
		       BEWARE_LABEL, do it now.
		       */
                  
		    if (!(add_epsilon_p == TRUE || add_epsilon_p == FALSE))
		      {
			if (beware_label != ID_NO_SYMBOL)
			  add_epsilon_p =
			    find_label(beware_label, STATE_arc_set(to_state));
			else
			  add_epsilon_p = FALSE;
		      }
                  
		    if (add_epsilon_p)
		      {
			push_arc(make_arc(EPSILON, to_state, insert), state);
			STATE_deterministic(state) = FALSE;
		      }
		    else
		      {
			STATE_final(state) = STATE_final(to_state);
                      
			splice_arcs(state, STATE_arc_set(to_state), insert,
				    DO_COPY, label_vector);
		      }
		    if (state != insert_start && from_state == to_state)
		      splice_arcs(state, insert_start_arcs, insert,
				  DO_COPY, label_vector);
		  }
	      }
          
          /* If we did not find an empty final state, TO_STATE may
             have become unreachable because no arc in INSERT was
             rerouted to it.
             */
          
          if (empty_final)
            delete_states(insert, has_arcs_p);
          else if (from_state != to_state)
            NET_pruned(target) = FALSE;
          
          if (!keep_start_state_p)
            {
              /* Delete the state but adding the arcs to the arc
                 count of TARGET.
                 */
              
              NET_states(insert) = 
                delete_state_from_list(insert_start, NET_states(insert));
              NET_num_states(insert)--;
              /* Don't change the arc count of INSERT because the
                 arcs of the deleted start state are going to be
                 be kept.
                 */
            }
          
          /* Append the arcs from the start state of INSERT (originals or
             copies depending on whether that state stays) to FROM_STATE.
             */
          
          splice_arcs(from_state, insert_start_arcs, target,
                      DONT_COPY, label_vector);
          
          
          /* TARGET is not deterministic or epsilon free unless
             INSERT has these properties and no epsilon arcs
             were added.
             */
          
          if (add_epsilon_p == TRUE || NET_deterministic(insert) == FALSE)
            NET_deterministic(target) = FALSE;
          
          if (add_epsilon_p == TRUE || NET_epsilon_free(insert) == FALSE)
            NET_epsilon_free(target) = FALSE;
          
	  /* If FROM_STATE or TO_STATE are the same or if INSERT is not
	     loopfree, TARGET is not loopfree. */

	  if (from_state == to_state || !NET_loop_free(insert))
	    NET_loop_free(target) = FALSE;

          /* Merge the contents of insert with target and free the rest */
          
          NET_num_states(target) += NET_num_states(insert);
          NET_num_arcs(target) += NET_num_arcs(insert);
          
          last_state = NET_states(insert);
          if (last_state != NULL)
            {
              while (next_state(last_state) != NULL)
                last_state = next_state(last_state);
              
              /* New states are spliced in to the front of the TARGET's state
                 set so that the do not interfere when SPLICE_FSM is called
                 iteratively on the TARGET's state set (as in IGNORE_FSM).
                 */
              next_state(last_state) = NET_states(target);
              NET_states(target) = NET_states(insert);
            }
          sever_states(insert);
        }
      else if (from_state != to_state)
        {
          splice_arcs(from_state, STATE_arc_set(to_state), target,
                      DO_COPY, label_vector);
          /* TO_STATE may have become unreachable. */
          NET_pruned(target) = FALSE;
        }
      label_augment(NET_labels(target), NET_labels(insert));
      
      free((void *) label_vector);
      NET_minimized(target) = FALSE;
    }
  else if (from_state != to_state)
    {
      /* Since no link to TO_STATE was created, it may now
         be a disconnected state.
         */
      NET_pruned(target) = FALSE;
      NET_minimized(target) = FALSE;
    }
  
  /* Augment the SIGMA field even in the case that no arcs are present
     to propagate the intensional sigma of insert.  (This requires
     more thought.)
     */
  
  sigma_augment(sigma(target), sigma(insert));

  /* If INSERT was a transducer, so is TARGET now. */

  if (NET_arc_label_arity(insert) > NET_arc_label_arity(target))
    NET_arc_label_arity(target) = NET_arc_label_arity(insert);

  free_network(insert);

  check_for_flags(target);
  return(target);
}

void align_sigmas(NETptr net1, NETptr net2)
{
  ALPHABETptr sigma1 = label_to_binary(sigma(net1)),
  sigma2 = label_to_binary(sigma(net2));
  
  if (binary_sigma_member(sigma1, OTHER))
    {
      if (binary_sigma_member(sigma2, OTHER))
	{ /* both nets contain OTHER */
	  ALPHABETptr other1 = sigma_difference(sigma2, sigma1);
	  ALPHABETptr other2 = sigma_difference(sigma1, sigma2);
	  expand_other(net1, other1, DONT_COPY);
	  expand_other(net2, other2, DONT_COPY);
	  free_alph(other1);
	  free_alph(other2);
	}
      else
	{ /* only net1 contains OTHER */
	  ALPHABETptr other1 = sigma_difference(sigma2, sigma1);
	  expand_other(net1, other1, DONT_COPY);
	  free_alph(other1);
	}
    }
  else if (binary_sigma_member(sigma2, OTHER))
    { /* only net2 contains OTHER */
      ALPHABETptr other2 = sigma_difference(sigma1, sigma2);
      expand_other(net2, other2, DONT_COPY);
      free_alph(other2);
    }
}

void align_multiple_sigmas(NVptr nv)
{
  /* Computes the joint sigma for all the networks in the net vector
     and uniformly expands OTHER with respect to the joint alphabet
     in all networks that contain it.
     */

  int n, other_p = FALSE, len = NV_len(nv);
  int sig_len = 0;
  NETptr net;
  ALPHABETptr sig;

  if (len < 2)
    return;
  
  /* Find the largest sigma. Check if we have OTHER. */

  for (n = 0; n < len; n++)
    {
      net = NV_net(nv, n);
      sig = label_to_binary(sigma(net));
      if (sig_len < ALPH_len(sig))
	sig_len = ALPH_len(sig);
      if (binary_sigma_member(sig, OTHER))
	other_p = TRUE;
    }
  
  if (!other_p)
    return;

  /* Collect the sigma */
  sig = make_alph(sig_len, BINARY_VECTOR);
  for (n = 0; n < len; n++)
    {
      net = NV_net(nv, n);
      add_binary_alph(ALPH_items(sig), ALPH_items(NET_sigma(net)),
		      ALPH_len(NET_sigma(net)));
    }

  /* Expand OTHER in all nets. */

  for (n = 0; n < len; n++)
    expand_other(NV_net(nv, n), sig, DONT_COPY);
  
  free_alph(sig);
}

NETptr expand_tuple_other(NETptr net, int copy_p)
{
  /* Duplicates every arc that has a tuple label containing
     OTHER on one side. The added arc is labeled with the
     symbol from the opposite side. Called by functions
     such as symbol_fsm() and sigma_fsm() that construct
     a network from symbol ids. If the net's sigma does not
     contain OTHER, there can not be any such arcs.
     */

  STATEptr state;
  ARCptr arc;
  ALPHABETptr labels;

  if (copy_p == DO_COPY)
    net = copy_fsm(net);

  if (!(sigma_member(NET_sigma(net), OTHER)))
    return(net);

  labels = label_to_binary(NET_labels(net));

  for (state = NET_states(net); state; state=next_state(state))
    for (arc = STATE_arc_set(state); arc; arc=next_arc(arc))
      {
	id_type id = ARC_label(arc);

	if (tuple_id_p(id))
	  {
	    if (id_to_upper_id(id) == OTHER && id_to_lower_id(id) != EPSILON)
	      {
		add_arc(make_arc(id_to_lower_id(id), ARC_destination(arc), net),
			state, net);
		sigma_add_to(labels, id_to_lower_id(id));
	      }
	    else if (id_to_lower_id(id) == OTHER &&
		     id_to_upper_id(id) != EPSILON)
	      {
		add_arc(make_arc(id_to_upper_id(id), ARC_destination(arc), net),
			state, net);
		sigma_add_to(labels, id_to_upper_id(id));
	      }
	  }
      }
  binary_to_label(NET_labels(net));
  return(net);
}

NETptr expand_other (NETptr net, ALPHABETptr other_sigma, int copy_p)
     /*****
       Duplicates every arc in NET that has OTHER as its label
       by adding a new arc for every symbol in OTHER_SIGMA that is
       not already in the NET's sigma alphabet. Note that the
       OTHER arcs themselves are preserved. If that is not desirable,
       use close_sigma().

       Returns the modified network.

       If COPY_P is DO_COPY, modifies a copy of NET.  If COPY_P is
       DONT_COPY, this function is destructive to NET.
       *****/
{
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  if (sigma_member(sigma(net), OTHER))
    {
      ALPHABETptr new_symbols;
      
      new_symbols = sigma_difference(other_sigma, NET_sigma(net));

      /* BOUNDARY_SYMBOL is never in the domain of OTHER! 
       */
      sigma_remove_from(new_symbols, BOUNDARY_SYMBOL);

      if (ALPH_len(new_symbols) > 0)
        {
          sigma_add_to(new_symbols, OTHER);
	  symbol_or_label_subst(OTHER, new_symbols, net, DONT_COPY,
				FALSE, FALSE, TRUE);
        }
      free_alph(new_symbols);
    }
  
  return (net);
}



ALPHABETptr expand_other_in_alph(ALPHABETptr alph, ALPHABETptr other_sigma, int copy_p)
     /*****
       Returns ALPH or its copy, expanded with labels in which OTHER is
       replaced by symbols that are included in OTHER_SIGMA but do not
       appear in ALPH.

       Returns the modified alphabet.

       If COPY_P is DO_COPY, modifies a copy of ALPH.  If COPY_P is
       DONT_COPY, this function is destructive to ALPH.
       *****/
{
  ALPHABETptr syms;
  int binary_p = FALSE;
	
  if (copy_p == DO_COPY)
    alph = copy_alphabet(alph);
  
  if (ALPH_type(alph) == BINARY_VECTOR)
    {
      binary_p = TRUE;
      binary_to_label(alph);
    }
		
  if (other_in_alph_p(alph))
    {
      ALPHABETptr aux, new_symbols;
      int i, j, k;
      id_type id, new_id;
      
      syms = label_components(alph);
      new_symbols = sigma_difference(other_sigma, syms);
      
      aux = copy_alphabet(alph);
      
      label_to_binary(alph);
      
      /* BOUNDARY_SYMBOL is never in the domain of OTHER! 
       */
      sigma_remove_from(new_symbols, BOUNDARY_SYMBOL);

      binary_to_label(new_symbols);
	  
      if (ALPH_len(new_symbols) > 0)
        {
	  for (i = 0; i < ALPH_len(aux); i++)
	    {
	      id = ALPH_item(aux, i);
		  	 
	      if (id == OTHER)
		{
		  for (j = 0; j < ALPH_len(new_symbols); j++)
		    sigma_add_to(alph, ALPH_item(new_symbols, j));
		  	 
		}
	      else if (upper_id(id) == OTHER)
		{
		  if (lower_id(id) == OTHER)
		    {
		      /* ?:? This is a tricky case because OTHER:OTHER
			 expansion must not produce an identity pair. */
		  	 				
		      for (j = 0; j < ALPH_len(new_symbols); j++)
			{
			  if (ALPH_item(new_symbols, j) != OTHER)
			    {
			      make_fstlabel(&new_id, OTHER,
					      ALPH_item(new_symbols, j));
			      sigma_add_to(alph, new_id);

			      make_fstlabel(&new_id, ALPH_item(new_symbols, j),
					      OTHER);
			      sigma_add_to(alph, new_id);
			    }

			  for (k = 0; k < ALPH_len(new_symbols); k++)
			    if (j != k)
			      {
				make_fstlabel(&new_id, ALPH_item(new_symbols, j), ALPH_item(new_symbols, k));
				sigma_add_to(alph, new_id);
			      }
			}
		    }
		  else
		    for (j = 0; j < ALPH_len(new_symbols); j++)
		      {
			make_fstlabel(&new_id, ALPH_item(new_symbols, j),
					lower_id(id));
			sigma_add_to(alph, new_id);
		      }
		}
	      else if (lower_id(id) == OTHER)
		{
		  for (j = 0; j < ALPH_len(new_symbols); j++)
		    {
		      make_fstlabel(&new_id, upper_id(id),
					      ALPH_item(new_symbols, j));
		      sigma_add_to(alph, new_id);
		    }
		}
	    }
        }
      free_alph(aux);
      free_alph(syms);
      free_alph(new_symbols);
    }
  
  if (!binary_p)
    binary_to_label(alph);
  	
  return (alph);
}

NETptr close_sigma(NETptr net, ALPHABETptr new_symbols,
		     int copy_p, int minimize_p)
     /*
      *
      * CLOSE_SIGMA closes the alphabet of NET by replacing OTHER transitions
      * in NET  with symbols in NEW_SYMBOLS that are not already in the
      * alphabet of NET. OTHER remains only if it is included in NEW_SYMBOLS.
      * If so, a warning is issued on the assumption that the purpose of
      * a calling CLOSE_SIGMA is to remove all accurrences of OTHER.
      *
      */
{
  if (copy_p)
    net = copy_fsm(net);
  
  if (sigma_member(sigma(net), OTHER))
    {
      if (new_symbols != NULL)
        {
          ALPHABETptr sig_diff;
          
          sig_diff = sigma_difference(new_symbols, NET_sigma(net)); 
          symsubst(OTHER, sig_diff, net, DONT_COPY, FALSE);
          free_alph(sig_diff);
        }
      else
        symsubst(OTHER, NULL, net, DONT_COPY, FALSE);
      
      ALPH_item(NET_sigma(net), OTHER) = FALSE;
    }
  
  if (!(sigma_member(sigma(net), OTHER)))
    NET_closed_sigma(net) = TRUE;
  else
    handle_warning("SIGMA not closed", "CLOSE_SIGMA", 1);
  
  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);
  
  return(net);
}

static int one_sided_other_p(ALPHABETptr labels)
{
  /* Return TRUE or FALSE depending on whether labels contains OTHER
     as a component of a symbol pair.
     */
  int i;
  id_type id;
  LABELptr lab;
  
  for (i = 0; i < ALPH_len(labels); i++)
    {
      id = ALPH_item(labels, i);
      lab = id_to_label(id);
      if (LABEL_arity(lab) > 1 &&
	  (id_to_upper_id(id) == OTHER ||
	    id_to_lower_id(id) == OTHER))
	return(TRUE);
    }
  return(FALSE);
}


void compact_sigma(NETptr net)
{
  /* If OTHER is in the SIGMA of the network but not in the label
     alphabet, it can be eliminated entirely. If OTHER occurs in
     the net, we can eliminate transitions for single symbols that
     (1) are in the same equivalence class with OTHER and
     (2) do not occur as a componenent of any fstpair label.
     and (3) OTHER does not occur as component of a symbol pair.
     *** Condition (3) added by LK 1998-12-21.
     */

  prune_fsm(net, DONT_COPY);
  prune_sigma(net);

  if (sigma_member(NET_sigma(net), OTHER))
    {
      /* Having called prune_sigma(), we know that OTHER
	 occurs as an arc label somewhere.
	 */

      ALPH_VECTORptr eqv_partition;
      ALPHABETptr sig = NET_sigma(net), labels = NET_labels(net);

      /* Make sure alphabets are in the expected format. */

      label_to_binary(sig);
      binary_to_label(labels);

      /* Cannot reduce sigma if the net contains symbol pairs
	 with OTHER as one component. */
      
      if (one_sided_other_p(labels))
	return;
	  
      eqv_partition = net_eqv_classes(net);

      /* The equivalence class headed by OTHER should be the
	 first one.
	 */

      if (av_len(eqv_partition) > 0)
	{
	  ALPHABETptr class = av_alph(eqv_partition, 0);
	  LABELptr label;
	  int16 k;
	  id_type id;
	  int i, j;
	  STATEptr state = NET_states(net);
	  ARCptr arc, prev, next;

	  if (!(class && ALPH_len(class) > 0 && ALPH_item(class, 0) == OTHER))
	    {
	      /* This could occur if OTHER were present only as a component
		 of a symbol pair but we have already excluded all cases
		 of one-sided OTHERs. Thus OTHER should be present as a
		 single label. */

	      handle_warning("No equivalence class for OTHER found.",
			     "COMPACT_SIGMA", 0); 

	      free_alph_vector(eqv_partition);
	      return;
	    }

	  label_to_binary(class);

	  /* Remove OTHER itself, all fstpair labels, and all single
	     labels that occur as a component of some fstpair label
	     in the network from the list.
	     */

	  ALPH_item(class, OTHER) = 0;

	  for (i = 1; i < ALPH_len(class); i++)
	    if (ALPH_item(class, i))
	      {
		label = id_to_label(i);
		if (LABEL_arity(label) > 1)
		  ALPH_item(class, i) = 0;
		else
		  {
		    /* An atomic symbol. Must check if it appears
		       as a component of some other label.
		       */

		    for (j = 0; j < ALPH_len(labels); j++)
		      {
			id = ALPH_item(labels, j);
			k = LABEL_arity(id_to_label(id));

			if (k > 1 && (upper_id(id) == i || lower_id(id) == i))
			  {
			    /* Symbol i and its transitions must be kept. */
			    ALPH_item(class, i) = 0;
			    break;
			  }
		      }
		  }
	      }

	  binary_to_label(class);
	  
	  /* Is there any work to be done? */
	  
	  if (ALPH_len(class) == 0)
	    {
	      /* All candicate symbols for deletion were eliminated. */

	      free_alph_vector(eqv_partition);
	      return;
	    }

	  /* Eliminate arcs for the remaining symbols. */

	  while (state)
	    {
	      prev = NULL;
	      arc = STATE_arc_set(state);
	      
	      while(arc)
		{
		  next = next_arc(arc);

		  for (i = 0; i < ALPH_len(class); i++)
		    if (ARC_label(arc) == ALPH_item(class, i))
		      break;

		  if (i < ALPH_len(class))
		    {
		      /* Arc label is in the set of redundant labels. */

		      if (prev)
			next_arc(prev) = next;
		      else
			STATE_arc_set(state) = next;

		      free_arc(arc);
		      NET_num_arcs(net)--;
		    }
		  else
		    prev = arc;

		  arc = next;
		}
	      state = next_state(state);
	    }
	  /* Update sigma and label alphabets. */

	  label_to_binary(labels);

	  for (i = 0; i < ALPH_len(class); i++)
	    {
	      id = ALPH_item(class, i);
	      ALPH_item(sig, id) = 0;
	      ALPH_item(labels, id) = 0;
	    }
	    
	  binary_to_label(labels);

	  /* Done. Release eqv partitition. */

	  free_alph_vector(eqv_partition);
	}
    }
  else
    {
      /* Eliminate any superfluous symbols in SIGMA. */
      free_alph(NET_sigma(net));
      NET_sigma(net) = label_sigma(net);
    }
}

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

#if 0
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

#endif /* 0 */

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

#endif /* SLIM_FST */
/******************************************/
