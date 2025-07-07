/* $Id: com-rep.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1997 by the Xerox Corporation.  All rights reserved */

/**********************************************************************
*								      *
*                      COMPILE_AND_REPLACE_SELF                       *
*								      *
*                         Lauri Karttunen                             *
*                        December 19, 1997                            *
*                   Xerox Research Centre Europe                      *
*                                                                     *
*     The main function, compile_replace_self() compiles              *
*     regular expressions in a network and replaces the original      *
*     path with crossproduct of the uncompiled side of the path       *
*     and the result of the compilation. For example the result       *
*     of applying compile_replace_self to an Arabic lexical form      *
*     such as                                                         *
*               ^[ k t b .m>. C V C V C V .<m. a* ^]                  *
*     yields a transducer that maps the lexical form to the surface   *
*     form                                                            *
*                  k a t a b a                                        *
*     and vice versa.                                                 *
*                                                                     *
***********************************************************************/

#include <stddef.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "fst/calculus.h"
#include "fst/cons.h"
#include "fst/state-mr.h"
#include "fst/regex-in.h"
#include "fst/fsm_io.h"
#include "fst/flags.h"
#include "fst/define.h"
#include "fst/mark.h"
#include "fst/utf8_fatstr_util.h"


static int re_arc_p(ARCptr arc, id_type re_id, int direction)
{
  if (direction == UPPER)
    return(upper_id(ARC_label(arc)) == re_id);
  else
    return(lower_id(ARC_label(arc)) == re_id);
}

static int write_to_buff(id_type id, char **buffp, int *len, int count,
			 int retokenize_p)
{
  FAT_STR fs;
  int width;

  if (id == EPSILON)
    return(0);

  fs = id_to_atom_name(id);
  width = fat_strlen(fs);

  if (!retokenize_p && count > 0)
    width++;

  if ((count + width) >= *len)
    {
      *buffp = (char *) realloc(*buffp, (size_t) (2 * (*len)));
      *len = (2 * (*len));
    }
    
  if (count > 0)
    {
      if (retokenize_p)
	FST_fat_to_thin(fs, (*buffp + count));
      else
	{
	  *(*buffp + count) = ' ';
	  FST_fat_to_thin(fs, (*buffp + count + 1));
	}
    }
  else
    FST_fat_to_thin(fs, *buffp);
  
  return(width);
}

void static make_re_string(STACKptr stack, int direction, char **buffp,
			   int *len, int retokenize_p, int has_flags_p,
			   int *flags_p)
{
  /* Applies WRITE_TO_BUFF to all items in the stack */

  int loc, lim;
  STACK_BLOCKptr block;
  void **cells;
  ARCptr arc;
  id_type id;
  int count = 0;

  block = STACK_top(stack);

  while(STACK_BLOCK_previous(block))
    block = STACK_BLOCK_previous(block);
  
  while (block)
    {
      /* Ignore the first ^[ marker */
      if (count == 0)
	loc = 1;
      else
	loc = 0;

      if (block == STACK_top(stack))
	lim = STACK_cur_loc(stack);
      else
	lim = STACK_BLOCK_SIZE;

      cells = block->block;
      while (loc < lim)
	{
	  arc = (ARCptr) cells[loc];
	  loc++;

	  if (direction == UPPER)
	    id = upper_id(ARC_label(arc));
	  else
	    id = lower_id(ARC_label(arc));

	  /* Ignore flag diacritics */
	  /* Ignore the final ^] marker */

	  if (has_flags_p && flag_diacritic_p(id))
	    *flags_p = TRUE;
	  else if (loc < lim || block != STACK_top(stack))
	    count += write_to_buff(id, buffp, len, count, retokenize_p);
	}

      block = STACK_BLOCK_next(block);
    }
}

static NETptr make_path_net(STACKptr stack, int side, int flags_p)
{
  ARCptr arc, new_arc;
  NETptr net = epsilon_fsm();
  STACK_BLOCKptr top = STACK_top(stack);
  int loc = STACK_cur_loc(stack);
  int depth = STACK_depth(stack);
  STATEptr last_state = NET_start_state(net);
  ALPHABETptr sig = NET_sigma(net);
  ALPHABETptr labs = NET_labels(net);
  id_type id;

  /* Ignore the first and the last arc. */

  pop(stack);

  while ((arc = pop(stack)) && peek(stack))
    {
      switch(side) {
      case UPPER:
	if (flags_p)
	  id = upper_id(ARC_label(arc));
	else
	  id = lower_id(ARC_label(arc));
	break;
      case LOWER:
	if (flags_p)
	  id = lower_id(ARC_label(arc));
	else
	  id = upper_id(ARC_label(arc));
	break;
      case BOTH_SIDES:
	id = ARC_label(arc);
	break;
      default:
	assert(0);
	break;
      }

      if (flags_p)
	{
	  if (flag_diacritic_p(id))
	    {

	      switch(side) {
	      case UPPER:
		make_fstlabel(&id, id, EPSILON);
		break;
	      case LOWER:
		make_fstlabel(&id, EPSILON, id);
		break;
	      default:
		break;
	      }
	      new_arc = make_arc(id, last_state, net);
	    }
	  else
	    new_arc = NULL;
	}
      else if (id != EPSILON)
	new_arc = make_arc(id, last_state, net);
      else
	new_arc = NULL;


      if (new_arc)
	{
	  last_state = make_state(new_arc, net);

	  label_to_sigma(sig, id);
	  sigma_add_to(labs, id);
	}
	
    }

  NET_start_state(net) = last_state;

  binary_to_label(labs);

  STACK_top(stack) = top;
  STACK_cur_loc(stack) = loc;
  STACK_depth(stack) = depth;

  set_net_arity(net);

  return(net);
}


static void merge_tail(STATEptr state, NETptr net, STATEptr end_state)
{

  ARCptr arc, new_arc;
  STATEptr dest;

  if (STATE_visit_mark(state) == 0)
    {
      STATE_visit_mark(state) = 1;

      for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	{

	  dest = ARC_destination(arc);

	  if (STATE_final(dest))
	    {
	      /* Non empty final states must be kept. */

	      if (STATE_arc_set(dest))
		{
		  new_arc = copy_arc(arc, net);
		  ARC_destination(new_arc) = end_state;
		  push_arc(new_arc, state);
		  merge_tail(dest, net, end_state);
		}
	      else
		ARC_destination(arc) = end_state;
	    }
	  else
	    merge_tail(dest, net, end_state);
	}

      if (STATE_final(state))
	STATE_final(state) = FALSE;
    }
}


static void merge_prefix(STATEptr state, STATEptr old_state, NETptr net,
			 STATEptr end_state)
{
  /* For every arc in STATE look for a matching arc in OLD_STATE.
     If a match is found, reclaim the arc and continue prefix merge
     in the destination states of the matching arcs. If no match
     is found, collect the arc and start tail merge. At the
     end, update the arc set of the old state with the collected
     arcs.
     */

  ARCptr old_arc, arc, next, new_old_arc_set = STATE_arc_set(old_state);
  STATEptr dest;

  for (arc = STATE_arc_set(state); arc; arc = next)
    {
      next = next_arc(arc);
      dest = ARC_destination(arc);

      /* If the destination of the old arc has the visit mark
	 set to 2, it has more than one incoming arc. In
	 that case we have to break the merge in order not
	 to give rise to unwanted paths. */

      for (old_arc = STATE_arc_set(old_state);
	   old_arc;
	   old_arc = next_arc(old_arc))
	if (ARC_label(arc) == ARC_label(old_arc))
	  {
	    if (STATE_visit_mark(ARC_destination(old_arc)) & 2)
	      {
		old_arc = NULL;
		break;
	      }

	    /* Continue merge */
	    merge_prefix(dest, ARC_destination(old_arc), net, end_state);

	    /* Reclaim arc */

	    free_arc(arc);
	    NET_num_arcs(net)--;

	    break;
	  }

      if (!old_arc)
	{
	  /* No match. Move arc to the old_state.
	     Dont subtrack the arc from net's arc count
	     because it will be kept in the result.
	     Only the arcs that are freed get subtracted
	     from the net's arc count. */
	      
	  next_arc(arc) = new_old_arc_set;
	  new_old_arc_set = arc;
	  
	  if (STATE_final(dest) && !STATE_arc_set(dest))
	    ARC_destination(arc) = end_state;
	  else
	    merge_tail(dest, net, end_state);
	}
    }

  STATE_arc_set(state) = NULL;
  STATE_arc_set(old_state) = new_old_arc_set;
}


/*
static int check_num_arcs(NETptr net)
{
  STATEptr state;
  ARCptr arc;
  long count=0;

  for (state = NET_states(net); state; state = next_state(state))
    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
      count++;

  if (count != NET_num_arcs(net))
    {
      fprintf(stdout, "Expected: %ld, Counted: %ld\n", NET_num_arcs(net),
	      count);
      NET_num_arcs(net) = count;
      return(1);
    }
  else
    return(0);
}
*/

static void merge_comp_net(NETptr comp_net, NETptr net, STACKptr re_stack,
			   STATEptr start_state, STATEptr end_state)
{
  /* Link comp_net's tail to the terminating state of the regular
     expression. Push the start state of comp_net onto the stack
     in the client cell of the start state. The initial link
     is made in copy_compile_replace() when all the re paths
     starting at start_state are complete.

     The states and arcs of comp_net are assigned to the host
     network and the rest of the comp_net structure is reclaimed.

     Returning to signal that the compiled net is not a null
     fsm.
     */

  STATEptr state, next, prev;

  merge_prefix(NET_start_state(comp_net), start_state, comp_net, end_state);

  for (prev = NULL, state = NET_states(comp_net); state; state = next)
    {
      next = next_state(state);

      if (!(STATE_arc_set(state)))
	{
	  if (prev)
	    next_state(prev) = next;
	  else
	    NET_states(comp_net) = next;
	  
	  free_state(state);
	  NET_num_states(comp_net)--;
	}
      else
	{
	  STATE_visit_mark(state) = 0;
	  STATE_final(state) = FALSE;
	  prev = state;
	}
    }


  if (!prev)
    prev = NET_states(comp_net);

  if (prev)
    {
      /* Splice into net the remaining states of comp_net. */

      next_state(prev) = NET_states(net);
      NET_states(net) = NET_states(comp_net);
    }

  NET_num_states(net) += NET_num_states(comp_net);
  NET_num_arcs(net) += NET_num_arcs(comp_net);

  NET_states(comp_net) = NULL;
  NET_num_states(comp_net) = 0;
  NET_num_arcs(comp_net) = 0;

  NET_start_state(comp_net) = NULL;

  label_augment(NET_labels(net), NET_labels(comp_net));
  sigma_augment(NET_sigma(net), NET_sigma(comp_net));

  free_network(comp_net);
}

static void print_arc_label(void *item)
{
  ARCptr arc = (ARCptr) item;

  print_label(ARC_label(arc), stdout, FALSE);
}

static void print_re_stack(STACKptr re_stack)
{
   map_stack(re_stack, print_arc_label);
  fsm_fprintf(stdout, "\nDepth: %d\n", STACK_depth(re_stack));
}


static id_type get_link_arc_label(id_type label, NETptr net, int side)
{
  id_type sym_id, lab_id;

  switch (side) {
  case UPPER:
    sym_id = lower_id(label);
    make_fstlabel(&lab_id, EPSILON, sym_id);
    break;
  case LOWER:
    sym_id = upper_id(label);
    make_fstlabel(&lab_id, sym_id, EPSILON);
    break;
  case BOTH_SIDES:
    lab_id = label;
    sigma_add_to(NET_sigma(net), upper_id(label));
    sym_id = lower_id(label);
    break;
  default:
    assert(0);
    break;
  }

  sigma_add_to(NET_sigma(net), sym_id);
  sigma_add_to(NET_labels(net), lab_id);

  return(lab_id);
}



static NETptr add_edges(NETptr net, id_type first_lab, id_type last_lab,
			int side)
{
  /* If the begin-re and end-re symbols, ^[ and ^], had something other than
     EPSILON on the other side, we need to preserve them, having replaced
     begin-re and end-re symbols by epsilons.
     */

  id_type lab_id;
  ARCptr arc;

  if (first_lab == EPSILON && last_lab == EPSILON)
    return(net);

  label_to_binary(NET_labels(net));

  lab_id = get_link_arc_label(first_lab, net, side);

  if (lab_id != EPSILON)
    {
      arc = make_arc(lab_id, NET_start_state(net), net);

      NET_start_state(net) = make_state(arc, net);
    }

  lab_id = get_link_arc_label(last_lab, net, side);

  if (lab_id != EPSILON)
    {
      /* Create a new final state for the net (but don't make it final yet).
	 Add an new arc from every current final state to the
	 new state making them nonfinal.
	 */

      STATEptr state, final = make_state(NULL, net);

      for (state = NET_states(net); state; state = next_state(state))
	if (STATE_final(state))
	  {
	    add_arc(make_arc(lab_id, final, net), state, net);
	    STATE_final(state) = FALSE;
	  }

      STATE_final(final) = TRUE;
    }

  binary_to_label(NET_labels(net));

  return(net);
}

static STATEptr get_client_cell(STATEptr state, NETptr net)
{
  STATEptr client_state = (STATEptr) STATE_client_cell(state);

  if (!client_state)
    {
      /* Copies the re-entrancy bit from the original state. */

      client_state = (void*) make_state(NULL, net);
      STATE_final(client_state) = STATE_final(state);
      STATE_client_cell(state) = client_state;
      STATE_visit_mark(client_state) = (STATE_visit_mark(state) & 2);
    }
  
  return(client_state);
}

static void recover_copy(STACKptr re_stack, STATEptr begin, STATEptr end,
			 NETptr net, int side, id_type first_lab,
			 id_type last_lab)
{
  /* RE stack contains a path that does not end with END_ID
     although it starts with START_ID. We copy it without
     calling the regular expression compiler.

     */

  NETptr path_net;
  STATEptr start_state, end_state;
    
  start_state = get_client_cell(begin, net);
  end_state = get_client_cell(end, net);

  /* print_re_stack(re_stack); */

  path_net = make_path_net(re_stack, BOTH_SIDES, FALSE);
  /*  print_words(path_net, 72, 0, 1, FALSE, FALSE, stdout); */
  add_edges(path_net, first_lab, last_lab, BOTH_SIDES);

  merge_comp_net(path_net, net, re_stack, start_state, end_state);
}

static int compile_replace(STACKptr re_stack, STATEptr begin, STATEptr end,
			   NETptr net, int side, id_type first_lab,
			   id_type last_lab, char **string_buf,
			   int *buf_len, int retokenize_p,
			   ALPHABETptr upper_other, ALPHABETptr lower_other,
			   CONSptr **upper_array, id_type lower_max,
			   HEAPptr cons_heap, int has_flags_p)
{
  /* The inner core of the compile-replace algorithm. RE_STACK contains
     the path (in reverse order) of a regular expression. START_ARC
     is the first arc of that expression (on the bottom of RE_STACK.
     START_ID and END_ID are the symbol ids marking the beginning
     and the end of the expression ("^[" and "^]"). SIDE
     is the UPPER (= 0) or LOWER (= 1) indicating on what side of the
     path the compiled network is replaced.
     STRING_BUF is a pointer to a pointer to string, BUF_LEN is
     the length of the buffer (adjusted if necessary). NET is the
     network under construction.

     The routine consists of four steps: (1) make the PATH_NET
     network concatenating the labels on the stack; (2) make
     a string representing the expression to be compiled;
     (3) make RE_NET by compiling the string; 
     (4) build the crossproduct of the RE_NET and the lower or
     upper side of PATH_NET depending on whether the SIDE is
     UPPER or LOWER. On the lower side we compute

        [ [PATH_NET.l] .x. RE_NET ]

     On the UPPER side, the construction is

        [ RE_NET .x. [PATH_NET.u]]

     */

  NETptr path_net, re_net, cross_net;
  /*  STATEptr start_state = ARC_destination(start_arc); */
  STATEptr start_state, end_state;
  NVtype nv;
  NETptr nets[2];
  int return_val;
  int flags_p = FALSE;

  
  nv.len = 2;
  nv.nets = nets;

  /* print_re_stack(re_stack); */

  start_state = get_client_cell(begin, net);
  end_state = get_client_cell(end, net);

  path_net = make_path_net(re_stack, side, FALSE);
  /*  print_words(path_net, 72, 0, 1, FALSE, FALSE, stdout); */

  make_re_string(re_stack, side, string_buf, buf_len, retokenize_p,
		 has_flags_p, &flags_p);
  
  /* fsm_fprintf(stdout, "%s\n", *string_buf); */

  /* If the lower side of the path is an empty string, we treat
     it as a pair of empty brackets, i.e. we assign epsilon_fsm()
     to comp_net. Calling RE2N_net_from_string() would yield something
     different in that case.
     */

  if (!strcmp(*string_buf, ""))
    re_net = epsilon_fsm();
  else if (!(re_net = RE2N_net_from_regex_string (*string_buf)))
    {
      fsm_fprintf(stdout, "%s\n", *string_buf);
      re_net = null_fsm();
    }

  if (side == UPPER)
    cross_net = crossproduct_main(re_net, path_net, upper_other, lower_other,
				  upper_array, lower_max,
				  cons_heap, DO_MINIMIZE);
  else
    cross_net = crossproduct_main(path_net, re_net, upper_other, lower_other,
				  upper_array, lower_max,
				  cons_heap, DO_MINIMIZE);

  free_network(re_net);
  free_network(path_net);

  if (null_fsm_p(cross_net))
    {
      return_val = 1;
      free_network(cross_net);
    }
  else
    {
      if (flags_p)
	{
	  path_net = make_path_net(re_stack, side, TRUE);
	  cross_net = concat(cross_net, path_net);
	}
      return_val = 0;
      add_edges(cross_net, first_lab, last_lab, side);
      merge_comp_net(cross_net, net, re_stack, start_state, end_state);
    }
  return(return_val);
}

static void recursive_compile_replace(NETptr new_net, ALPHABETptr sig,
				      ALPHABETptr labs, STATEptr start_state,
				      id_type start_id, id_type end_id,
				      int side, int retokenize_p,
				      char **str_buf, int *buf_len,
				      long int *count, long int *error_count,
				      ALPHABETptr upper_other,
				      ALPHABETptr lower_other,
				      CONSptr **upper_array,
				      id_type lower_max,
				      HEAPptr cons_heap,
				      int has_flags_p)
{       
  /* The outer loop of compile_replace. The overall strategy is
     that of copy_fsm(): the arcs and states of OLD_NET are
     duplicated into NEW_NET. In general, each state of OLD_NET
     contains in its client cell the corresponding state of
     NEW_NET. However, in the course of recursive decent into
     OLD_NET we watch for the beginning of a regular expression
     marker (presumed to be a left bracket). When the START_ID
     is encountered, the arcs are pushed into RE_STACK and
     the new states are not placed, except for the initial
     state into the client cell of the corresponding old
     cell. 
     When the regular expression is complete, we call
     compile_replace() to do the real work. Because there may
     be several regular expressions with the same starting
     symbol, the results are pushed onto a stack held in
     the client cell of the new state corresponding to
     the start state of the RE in OLD_NET.
     If the regular expression path leads to a final state
     without a terminating END_ID marke, the path is copied.
     Thus we allow the START_ID and END_ID markers to occur
     in the net freely.
     If the RETOKENIZE_P flag is TRUE, the regular expression
     path should contain spaces as token boundaries. If the
     RETOKENIZE_P is FALSE, the original token boundaries
     are preserved.
     This function needs to recursively call itself when a regular
     expression substring is followed by something else, possibly
     containing another regular expression substring. Need new
     stacks and  new root_state, end_state variables, to allow any
     number of regular expression substrings to occur along the same
     path.

     Paths with unbalanced ^[ and ^] markers are treated in the same
     manner as paths with no instances of ^[ or ^].

     If a path contains two ^[ markers without an intervening ^],
     the second ^[ is included in the regular expression string
     started at the first marker and a warning message is
     printed. The compilation of such a string will fail.
  */

  STACKptr state_stack, arc_stack, re_stack;
  ARCptr arc, new_arc;
  STATEptr state, dest, new_state, new_dest, root_state = NULL;
  STATEptr end_state = NULL;
  int re_p, compile_p, recover_p, error_p;
  id_type first_lab = 0, last_lab = 0;

  state_stack = create_stack("States to process");
  arc_stack = create_stack("Arcs to process");
  re_stack = create_stack("Regular expression path");

  new_state = get_client_cell(start_state, new_net);
  
  push((void *) start_state, state_stack);
  push((void *) STATE_arc_set(start_state), arc_stack);

  compile_p = FALSE;
  recover_p = FALSE;
  error_p = FALSE;

  while ((state = pop(state_stack)))
    {

      /* States with incoming arcs have visit mark set to 2.
	 States along a regular expression path have the
	 visit mark set to 1 or 3. We mask here for the
	 least significant bit. */

      re_p = (STATE_visit_mark(state) & 1);

      if (re_p)
	{
	  pop(re_stack);
	  if (state == root_state)
	    {
	      /* Continue in copy mode. Masking with 2 clears the re bit. */
	      re_p = FALSE;
	      STATE_visit_mark(state) = STATE_visit_mark(state) & 2;
	      root_state = NULL;
	    }
	}

      arc = pop(arc_stack);

      /* States in a regular-expression region have
	 empty client cells. */

      while (arc)
	{
	  push((void *) next_arc(arc), arc_stack);

	  dest = ARC_destination(arc);
	  new_dest = STATE_client_cell(dest);

	  if (re_arc_p(arc, start_id, side))
	    {
	      if (re_p)
		error_p = TRUE;
	      else
		{
		  first_lab = ARC_label(arc);
		  re_p = TRUE;
		  root_state = state;
		}
	    }
	  else if (re_p && re_arc_p(arc, end_id, side))
	    {
	      last_lab = ARC_label(arc);
	      end_state = dest;

	      re_p = FALSE;
	      compile_p = TRUE;
	    }

	  if (re_p || compile_p)
	    {
	      push((void *) arc, re_stack);
	      STATE_visit_mark(state) = (STATE_visit_mark(state) | 1);
	      if (error_p)
		{
		  fsm_fprintf(stdout, "*** Warning: nested ^[ ignored.\n");
		  print_re_stack(re_stack);
		  error_p = FALSE;
		}

	      if (STATE_final(dest) && !compile_p)
		{
		  /* A path containing ^[ that does not contain
		     a matching ^]. In the previous version this
		     was considered an error. Now we copy the
		     path without calling the regular expression
		     compiler. In effect the path is treated as if
		     there was no instance of ^[.
		  */
		  recover_p = TRUE;
		  last_lab = ARC_label(arc);
		  re_p = FALSE;
		  end_state = dest;
		}
	    }
	  else
	    {
	      new_arc = make_arc(ARC_label(arc),
				 get_client_cell(dest, new_net), new_net);
	      new_state = get_client_cell(state, new_net);
	      add_arc(new_arc, new_state, new_net);
	      label_to_sigma(sig, ARC_label(new_arc));
	      sigma_add_to(labs, ARC_label(new_arc));
	    }

	  if (new_dest && !re_p)
	    {
	      /* The destination state of the arc has already been
		 processed. If we are in a copy mode, we don't have
		 to go there a second time, but we are looking
		 for the end of a regular expression substring
		 the state has to be processed again. */

	      arc = pop(arc_stack);
	    }
	  else
	    {
	      /* The destination state of the arc has not yet
		 been visited. If we are in the copy mode, the
		 new destination state is pushed to the client
		 cell of the corresponding original state.
	      */

	      /* Depth-first mode: start processing the destination
		 state of the current arc. */
	      
	      push((void *) state, state_stack);
	      state = dest;
	      arc = STATE_arc_set(state);
	    }

	  if (compile_p)
	    {
	      (*count)++;
	      if (compile_replace(re_stack, root_state, end_state,
				  new_net, side,
				  first_lab, last_lab, str_buf, buf_len,
				  retokenize_p, upper_other, lower_other,
				  upper_array, lower_max, cons_heap,
				  has_flags_p))
		{
		  fsm_fprintf(stdout, "*** Input # %ld: %s",
			      *count, *str_buf);
		  fsm_fprintf(stdout, "    No result.\n");
			    
		  (*error_count)++;
		}

	      compile_p = FALSE ;

	      if (new_dest)
		pop(re_stack);
	      else if (STATE_arc_set(end_state) &&
		       (!STATE_client_cell(end_state)))
		{
		  /* Regular expression string followed by something
		     else. Need to recurse instead of continuing
		     directly. */
		     
		  recursive_compile_replace(new_net, sig, labs, end_state,
					    start_id, end_id, side,
					    retokenize_p, str_buf,
					    buf_len, count, error_count,
					    upper_other, lower_other,
					    upper_array, lower_max,
					    cons_heap, has_flags_p);

		  /* Set arc to NULL. We have already processed
		     the state's arc set in the recursive call above. */

		  arc = NULL;
		}
	    }
	  else if (recover_p)
	    {
	      /* The re_stack contains a path that starts with ^[
		 but does not lead to ^]. We just copy it.
		 */
	      recover_copy(re_stack, root_state, end_state, new_net, side,
			   first_lab, last_lab);

	      recover_p = FALSE;
	      re_p = TRUE;

	      if (new_dest)
		pop(re_stack);

	    }
	}

      /* Masking the visit mark with 2 clears the last bit
	 that marks a regular expression path. Clearing is
	 is necessary because we might come back to the state
	 another path that does not contain the initial ^[.
      */

      STATE_visit_mark(state) = (STATE_visit_mark(state) & 2);
    }

  /* If re_stack is not empty at this point, there is a bug in
     the algorithm. */

  if (STACK_cur_loc(re_stack) != 0)
    handle_warning("RE_STACK is not empty!", "RECURSIVE_COMPILE_REPLACE",
		   STACK_cur_loc(re_stack));

  free_stack(re_stack);
  free_stack(state_stack);
  free_stack(arc_stack);
}

static int limiter_absent(NETptr net, id_type start_id, id_type end_id,
			       int side)
{
  ALPHABETptr labs = binary_to_label(NET_labels(net));
  uint16 len = ALPH_len(labs);
  int i, start_p = FALSE, end_p = FALSE;

  if (side == UPPER)
    for (i = 0; i < len; i++)
      {
	if (upper_id(ALPH_item(labs, i)) == start_id)
	  start_p = TRUE;
	else if (upper_id(ALPH_item(labs, i)) == end_id)
	  end_p = TRUE;
      }
  else
    {
      for (i = 0; i < len; i++)
	if (lower_id(ALPH_item(labs, i)) == start_id)
	  start_p = TRUE;
	else if (lower_id(ALPH_item(labs, i)) == end_id)
	  end_p = TRUE;
    }

  return(!(start_p && end_p));
}

static NETptr try_to_fix(NETptr net, int side)
{
  NVtype nv;
  NETptr nets[2];
  NETptr fix;
  id_type id1, id2, id3;


  FST_intern_literal(&id1, "^");
  FST_intern_literal(&id2, "[");
  FST_intern_literal(&id3, "]");

  if (sigma_member(NET_sigma(net), id1) &&
      sigma_member(NET_sigma(net), id2) &&
      sigma_member(NET_sigma(net), id3))
    {
      nv.nets = nets;
      nv.len = 2;

      fix = RE2N_net_from_regex_string("%^ %[ -> %^%[, %^ %] -> %^%]");

      if (side == UPPER)
	{
	  nets[0] = invert_fst(fix, DONT_COPY);
	  nets[1] = net;
	}
      else
	{
	  nets[0] = net;
	  nets[1] = fix;
	}
      net = compose_fst(&nv, DO_RECLAIM, DO_MINIMIZE, DONT_CLEANUP, PAIRWISE,
			DONT_OBEY);
    }
  return(net);
}

static NETptr com_rep_abort(NETptr net, int minimize_p, int reclaim_p,
			    char *start_re, char *end_re)
{

  fsm_fprintf(stdout, "No occurrences of '%s' and '%s'.\n",
	      start_re, end_re);
  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);

  if (reclaim_p == DO_RECLAIM)
    return(net);
  else
    return(copy_fsm(net));
}

static void mark_reentrancy(NETptr net)
{
  /* Set visit mark of original states to 2 if the state has more
     than one incoming arc. */

  STATEptr state, dest;
  ARCptr arc;

  clear_state_marks_and_client_cells(net);
  
  for (state = NET_states(net); state; state = next_state(state))
    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
      {
	dest = ARC_destination(arc);

	if (STATE_visit_mark(dest) < 2)
	  (STATE_visit_mark(dest))++;
      }

  /* Reset the visit mark back to 0 for states that only have
     one incoming arc. */

  for (state = NET_states(net); state; state = next_state(state))
    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
      {
	dest = ARC_destination(arc);

	if (STATE_visit_mark(dest) < 2)
	  STATE_visit_mark(dest) = 0;
      }
}



NETptr compile_and_replace_self(NETptr net, int side, int reclaim_p,
				int minimize_p, int retokenize_p)
{
  /* Every regular expression on the given side of NET is
     extracted and compiled. The compiled expression is
     replaced by the result of the compilation "in situ".
     This function is primarily intended for the compilation of the
     Arabic morphological analyzer but it may have other uses.

     For example, compiling-replacing the lower side of the
     relation

       [ w r i t e .x. "^[" k t b ".m>." C V C V C V ".<m." a "*" "^]" ]

     yiels the result:
     
       [ w r i t e .x. k a t a b a ] 

     because the regular expression on the lower side of the
     original relation compiles into "kataba".
  */

  char *start_re = "^[", *end_re = "^]";
  id_type start_id, end_id;
  NETptr result, def_net;
  ALPHABETptr sig, labs;
  int buf_len = 80, i;
  long int count = 0, error_count = 0;
  char *str_buf ;
  ALPHABETptr upper_other, lower_other, aux_sigma;
  id_type upper_max, lower_max, id;
  CONSptr **upper_array;
  HEAPptr cons_heap;
  int has_flags_p = has_flag_diacritics_side_p(net, side);
  FAT_STR name, fs;
  FAT_CHAR chars[2];

  FST_intern_literal(&start_id, start_re);
  FST_intern_literal(&end_id, end_re);

  /* This will be used later to peel off the components of a
     multicharacter symbol to be interned as such. */

  name = (FAT_STR) chars;
  CHARNUM(name+1) = CHARSET(name+1) = '\0';

  if (limiter_absent(net, start_id, end_id, side))
    {
      if (retokenize_p)
	{
	  net = try_to_fix(net, side);
	
	  if (limiter_absent(net, start_id, end_id, side))
	    return(com_rep_abort(net, minimize_p, reclaim_p, start_re,
				 end_re));
	}
      else
	return(com_rep_abort(net, minimize_p, reclaim_p, start_re, end_re));
    }

  /* Preliminary steps of crossproduct */

  scan_labels(net, net, &upper_max, NULL, NULL, &upper_other);
  scan_labels(net, net, &lower_max, NULL, NULL, &lower_other); 
  binary_to_label(upper_other);
  binary_to_label(lower_other);
  aux_sigma = side_sigma(net, side);

  /* If the alphabet of the net contains defined symbols,
     we need to take into account symbols in the associated
     networks. If the label is an undefined multicharacter
     symbol, we need to include its single character components
     because the label might be, for example, "[u*i]" that gets
     compiled into the u*i language by the regular expression
     compiler. This would give an error in the crossproduct
     compilation if neither "u" nor "i" occurs in the net.
     -- A bug found by Shlomo Yona. */

  sig = label_to_binary(NET_sigma(net));

  for (i = 0; i < ALPH_len(sig); i++)
    {
      if (ALPH_item(sig, i))
	{
	  if ((def_net = get_def_uncopied(i)))
	  {
	    scan_labels(def_net, net, &upper_max, NULL, NULL, &upper_other);
	    scan_labels(def_net, net, &lower_max, NULL, NULL, &lower_other); 
	  }
          else
            {
              if (sigma_member(aux_sigma, i) && (fs = id_to_atom_name(i)) &&
                  !(CHARNUM(fs+1) == '\0' && CHARSET(fs+1) == '\0'))
                {
                  /* Intern the components of an undefined multicharacter
                     symbols because it may be split up in parsing
                     the regular expression. We need to know this in
                     order to allocate arrays big enough to accommodate
                     them. This is a fix to a bug flushed out by Shlomo
                     Yona. 2004-02-05 LK.
                  */

                  while(CHARNUM(fs) || CHARSET(fs))
                    {
                      CHARNUM(name) = CHARNUM(fs);
                      CHARSET(name) = CHARSET(fs++);

                      intern_atomic_label(&id, name);
                      if (id  > upper_max)
                        upper_max = id;
                      if (id  > lower_max)
                        lower_max = id;
                    }
                }
            }
	}
    }

  upper_max++; lower_max++;
  upper_array = (CONSptr **)calloc((size_t) (upper_max),
				   (size_t) sizeof(CONSptr *));
  str_buf = (char *) calloc((size_t) buf_len, sizeof(char));
  cons_heap = make_cons_heap();

  if (!(upper_array && str_buf && cons_heap))
    {
      fsm_fprintf(stderr, "*** Aborting COMPILE_AND_REPLACE_SELF().\n\
    Not enough memory for this operation.\n");

      return(net);
    }

  mark_reentrancy(net);

  result = null_fsm();
  

  NET_minimized(result) = FALSE;
  NET_deterministic(result) = FALSE;
  NET_epsilon_free(result) = FALSE;
  NET_pruned(result) = FALSE;
  NET_loop_free(result) = FALSE;

  sig = NET_sigma(result);
  labs = NET_labels(result);

  STATE_final(NET_start_state(result)) =  STATE_final(NET_start_state(net));
  STATE_client_cell(NET_start_state(net)) = NET_start_state(result);
		    
  recursive_compile_replace(result, sig, labs, NET_start_state(net),
			    start_id, end_id, side, retokenize_p, &str_buf,
			    &buf_len, &count, &error_count,
			    upper_other, lower_other, upper_array,
			    lower_max, cons_heap, has_flags_p);

  binary_to_label(labs);

  free(str_buf);
  free_upper_array(upper_array, NET_labels(net));
  free_alph(upper_other);
  free_alph(lower_other);
  free_alph(aux_sigma);

  prune_fsm(result, DONT_COPY);
  label_check(result);

  if (reclaim_p == DO_RECLAIM)
    free_network(net);

  if (minimize_p == DO_MINIMIZE)
    {
      min_fsm(result, DONT_COPY);
      compact_sigma(result);
    }

  fsm_fprintf(stdout, "%ld regular expressions compiled successfully. ",
	      count);
  if (error_count == 0)
    fsm_fprintf(stdout, "No errors.\n");
  else
    fsm_fprintf(stdout, "%ld error%s\n", error_count,
		((error_count == 1) ? "" : "s"));

  free_heap(cons_heap);

  return(result);
}


NETptr extract_compile_replace(NETptr net, int side, int reclaim_p,
			       int minimize_p, int retokenize_p)
{
  /* A version of compile-replace for large jobs such as Ken
     Beesley's Arabic lexicon. It exctracs from NET all the
     valid regular expression strings excluding all the other
     material and calls the regular compile_and_replace_self()
     on the extracted subnetwork. Once the work is done,
     the result is composed with the original network. On
     large networks this three-step procedure is in general
     faster than calling compile_and_replace_self() directly.
     Another benefit is that it tends to yield a smaller
     result, both in terms arcs and the size of the alphabet.
  */

  char *start_re = "^[", *end_re = "^]";
  id_type start_id, end_id;
  NVtype nv;
  NETptr nets[3];
  NETptr result, delete_rule, require_marker;

  FST_intern_literal(&start_id, start_re);
  FST_intern_literal(&end_id, end_re);

  if (limiter_absent(net, start_id, end_id, side))
    {
      if (retokenize_p)
	{
	  net = try_to_fix(net, side);
	
	  if (limiter_absent(net, start_id, end_id, side))
	    return(com_rep_abort(net, minimize_p, reclaim_p, start_re,
				 end_re));
	}
      else
	return(com_rep_abort(net, minimize_p, reclaim_p, start_re, end_re));
    }

  /* Delete everything that precedes ^[ or follows ^] */

  delete_rule = 
    RE2N_net_from_regex_string("\\[%^%[ | %^%]] -> 0 || _ $%^%[ , $%^%] _ ");

  /* Require an initial ^[ */

  require_marker = RE2N_net_from_regex_string("$%^%[");

  nv.nets = nets;
  nv.len = 3;

  switch (side) {
  case UPPER:
    nets[0] = copy_fsm(require_marker);
    nets[1] = invert_fst(delete_rule, DONT_COPY);
    nets[2] = copy_fsm(net);
    break;
  case LOWER:
    nets[0] = copy_fsm(net);
    nets[1] = delete_rule;
    nets[2] = copy_fsm(require_marker);
    break;
  default:
    assert(0);
    break;
  }
 
  /* Extract regular expression substrings. */

  fsm_fprintf(stdout, " *** Extracting regular expression substrings...\n");

  result = compose_fst(&nv, DO_RECLAIM, DO_MINIMIZE, DONT_CLEANUP,
		       SEQUENTIALLY, FLAGS_NOT_SPECIAL);

  switch (side) {
  case UPPER:
    result = upper_fsm(result, DONT_COPY, DO_MINIMIZE);
    break;
  case LOWER:
    result = lower_fsm(result, DONT_COPY, DO_MINIMIZE);
    break;
  default:
    assert(0);
    break;
  }

  /* Call the main routine. */

  fsm_fprintf(stdout, " *** Calling compile-replace...\n");

  result =  compile_and_replace_self(result, side, DO_RECLAIM,
				     minimize_p, retokenize_p);

    /* Splice the result between ~$"^[" and ~$"^]" */

  nets[0] = RE2N_net_from_regex_string("~$%^%[");
  nets[1] = result;
  nets[2] = RE2N_net_from_regex_string("~$%^%]");

  result = concat_fsm(&nv, DONT_COPY, DONT_MINIMIZE);

  /* Union with ~$"^[" */

  nv.len = 2;
  nets[0] = result;
  nets[1] = negate_fsm(require_marker, DONT_COPY, DONT_MINIMIZE);

  result = union_fsm(&nv, DONT_COPY, DO_MINIMIZE);

  switch (side) {
  case UPPER:
    nets[0] = result;
    nets[1] = net;
    break;
  case LOWER:
    nets[0] = net;
    nets[1] = result;
    break;
  default:
    assert(0);
    break;
  }
  fsm_fprintf(stdout, " *** Composing the final result...\n");

  result = compose_fst(&nv, reclaim_p, minimize_p, DONT_CLEANUP,
		       PAIRWISE, FLAGS_NOT_SPECIAL);
  return(result);
}
