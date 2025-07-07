/* $Id: copy-fsm.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "fst/struct.h"
#include "fst/modify-base.h"
#include "fst/copy-fsm.h"
#include "fst/fsm_io.h"
#include "fst/mark.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

/******************************************************
 **
 **  COPY-FSM.C
 **
 **  Lauri Karttunen, Xerox PARC, March 1991
 **  Copying fsm structures
 **
 ******************************************************/


ARCptr copy_arc (ARCptr arc, NETptr net)
     /*****
      *
      * Returns a copy of ARC.
      *
      *****/
{
  ARCptr new_arc;
  
  new_arc = make_arc(ARC_label(arc), ARC_destination(arc), net);
#ifdef FULL_FSM
  ARC_user_pointer(new_arc) = ARC_user_pointer(arc);
#endif
  return (new_arc);
  
}

STATEptr copy_state(STATEptr state, NETptr net)
{
  /*  Adds a new state to NET with the same properties and arcs as STATE */
  
  ARCptr arc;
  STATEptr new_state = make_state(NULL, net);
  
  STATE_final(new_state) = STATE_final(state);
  STATE_deterministic(new_state) = STATE_deterministic(state);
  
  for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
    add_arc(copy_arc(arc, net), new_state, net);
  
  return(new_state);
}

static void copy_states(NETptr old_net, NETptr new_net)
{       
  STACKptr stack;
  ARCptr old_arc = NULL, new_arc = NULL;
  STATEptr old_source, old_dest, new_source, new_dest;
  
  stack = create_stack("States to copy");
  
  old_source = NET_start_state(old_net);        
  new_source = make_state(NULL, new_net);
  
  STATE_client_cell(old_source)   = new_source;
  STATE_final(new_source)         = STATE_final(old_source);
  STATE_deterministic(new_source) = STATE_deterministic(old_source);
  
  NET_start_state(new_net) = new_source;
  old_arc = STATE_arc_set(old_source);
  
  for(;;)
    {           
      if (old_arc == NULL)
        {
          if (STACK_depth(stack) > 0)
            {
              old_source = pop(stack);
              new_source = STATE_client_cell(old_source);
              old_arc = pop(stack);
            }
          else
            break;
        }               
      
      new_arc = make_arc(ARC_label(old_arc), NULL, new_net);
      add_arc(new_arc, new_source, new_net);
      
      new_dest = STATE_client_cell(ARC_destination(old_arc));
      if (new_dest == NULL)
        {
          old_dest = ARC_destination(old_arc);
          new_dest = make_state(NULL, new_net);
          
          STATE_client_cell(old_dest) = new_dest;
          STATE_final(new_dest) = STATE_final(old_dest);
          STATE_deterministic(new_dest) = STATE_deterministic(old_dest);
          
          ARC_destination(new_arc) = new_dest;
          
          if (next_arc(old_arc) != NULL)
            {
              push(next_arc(old_arc), stack);
              push(old_source, stack);
            }
          
          old_source = old_dest;
          new_source = new_dest;
          old_arc = STATE_arc_set(old_source);
        }
      else
        {
          ARC_destination(new_arc) = new_dest;
          
          if (next_arc(old_arc) != NULL)
            {
              push(next_arc(old_arc), stack);
              push(old_source, stack);
            }
          old_arc = NULL;  /* forces a pop on next cycle */
        }
    }
  
  free_stack(stack);
}

static void allocate_sequence_data(SEQUENCEptr seq)
{
  switch (SEQ_type(seq))
    /* allocate memory for sequence */
    {
    case (Object):
      /* type -1 */
      SEQ_objects(seq) = (OBJECTptr *) calloc ((size_t) SEQ_len(seq),
					       (size_t) sizeof(OBJECTptr));
      if (!SEQ_objects(seq))
	not_enough_memory("ALLOCATE_SEQUENCE_DATA");
      break;
    case (Int):
      /* type 0 */
      SEQ_ints(seq) = (long int *) calloc((size_t) SEQ_len(seq),
					  (size_t) sizeof(long int));
      if (!SEQ_ints(seq))
	not_enough_memory("ALLOCATE_SEQUENCE_DATA");
      break;
    case (Character):
      /* type 1 */
      SEQ_characters(seq) = (FAT_CHAR *) calloc((size_t) SEQ_len(seq),
						(size_t) sizeof(FAT_CHAR));
      if (!SEQ_characters(seq))
	not_enough_memory("ALLOCATE_SEQUENCE_DATA");
      break;
    case (String):
      /* type 2 */
      SEQ_strings(seq) = (FAT_STR *) calloc((size_t) SEQ_len(seq),
					    (size_t) sizeof(FAT_STR));
      if (!SEQ_strings(seq))
	not_enough_memory("ALLOCATE_SEQUENCE_DATA");
      break;
    case (Symbol):
      /* type 3 */
      SEQ_symbols(seq) = (IO_SYMBOLptr)	calloc((size_t) SEQ_len(seq),
					       (size_t) sizeof(IO_SYMBOLtype));
      if (!SEQ_symbols(seq))
	not_enough_memory("ALLOCATE_SEQUENCE_DATA");
      break;
    case (Symbol_package):
      /* type -3 */
      SEQ_symbol_package(seq) =           
	(IO_SYMBOL_PACKAGEptr) calloc((size_t) 1,
				      (size_t) sizeof (IO_SYMBOL_PACKAGEtype));
      if (!SEQ_symbol_package(seq))
	not_enough_memory("ALLOCATE_SEQUENCE_DATA");

      SYM_PACK_sym_names(SEQ_symbol_package(seq)) = 
        (FAT_STR *) calloc ((size_t) SEQ_len(seq), (size_t) sizeof(FAT_STR));

      if (SYM_PACK_sym_names(SEQ_symbol_package(seq)) == NULL)
        not_enough_memory("ALLOCATE_SEQUENCE_DATA");
      break;
    case (Array):
      /* type 4 */
      SEQ_array(seq) = (SEQUENCEptr *) calloc((size_t) SEQ_len(seq),
					      (size_t) sizeof(SEQUENCEptr));
      if (SEQ_array(seq) == NULL)
        not_enough_memory("ALLOCATE_SEQUENCE_DATA");
      break;
    case (List):
      /* type 5 */
      SEQ_list(seq) = (SEQUENCEptr *) calloc((size_t) SEQ_len(seq),
					     (size_t) sizeof(SEQUENCEptr));
      if (SEQ_list(seq) == NULL)
        not_enough_memory("ALLOCATE_SEQUENCE_DATA");
      break;
    case (Byte_block):
      /* type 5 */
      SEQ_byte_block(seq) =
	(BYTE_BLOCKptr *) calloc((size_t) SEQ_len(seq),
				 (size_t) sizeof(BYTE_BLOCKptr));
      if (SEQ_byte_block(seq) == NULL)
        not_enough_memory("GET_SEQUENCE");
      break;
    default: /* Error */
      prog_error("allocate_sequence_data", "Unrecognized data type: %d\n",
		 SEQ_type(seq));
      break;
    }
}

static BYTE_BLOCKptr copy_byte_block(BYTE_BLOCKptr block)
{
  BYTE_BLOCKptr new_block;
  size_t len = BYTE_BLOCK_len(block);

  new_block = (BYTE_BLOCKptr) malloc((size_t) sizeof(BYTE_BLOCKtype));

  if (!new_block)
    not_enough_memory("COPY_BYTE_BLOCK");

  BYTE_BLOCK_len(new_block) =  len;
  BYTE_BLOCK_block(new_block) = (void **) malloc((size_t)len*sizeof(void *));

  if (!(BYTE_BLOCK_block(new_block)))
    not_enough_memory("COPY_BYTE_BLOCK");

  memcpy(BYTE_BLOCK_block(new_block), BYTE_BLOCK_block(block),
	 BYTE_BLOCK_len(block));

  return(new_block);
}

static IO_SYMBOLptr copy_io_symbol(IO_SYMBOLptr io_sym)
{
  IO_SYMBOLptr new_sym;

  new_sym = (IO_SYMBOLptr) malloc((size_t) sizeof(IO_SYMBOLtype));

  if (!new_sym)
    not_enough_memory("COPY_IO_SYMBOL");

  IO_SYM_name(new_sym) = copy_fat_string(IO_SYM_name(io_sym));
  
  return(new_sym);
}


static SEQUENCEptr copy_sequence(SEQUENCEptr seq)
{
  SEQUENCEptr new_seq;
  int i;

  new_seq = (SEQUENCEptr) calloc((size_t) 1, (size_t) sizeof(SEQUENCEtype));
 
  if (!new_seq)
    not_enough_memory("COPY_SEQUENCE");

  SEQ_len(new_seq) = SEQ_len(seq);
  SEQ_type(new_seq) = SEQ_type(seq);

  allocate_sequence_data(new_seq);


  for (i = 0; i < SEQ_len(seq); i++) /* fill sequence with data */
    {
      switch(SEQ_type(seq))
        {
        case (Object):
          /* type -1 */
          SEQ_objects(new_seq)[i] = copy_object(SEQ_objects(seq)[i]);
          break;
        case (Int):
          /* type 0 */
          SEQ_ints(new_seq)[i] = SEQ_ints(seq)[i];
          break;
        case (Character):
          /* type 1 */
	  fat_charcpy(SEQ_characters(new_seq)[i], SEQ_characters(seq)[i]);
          break;
        case (String):
          /* type 2 */
	  SEQ_strings(new_seq)[i] = copy_fat_string(SEQ_strings(seq)[i]);
          break;
        case (Symbol_package):
          /* type -3 */
	  SYM_PACK_sym(SEQ_symbol_package(new_seq), i) =
	    copy_fat_string(SYM_PACK_sym(SEQ_symbol_package(seq), i));
          break;
        case (Symbol):
          /* type 3 */
	    IO_SYM_name(SEQ_symbols(new_seq) + i) =
	    copy_fat_string(IO_SYM_name(SEQ_symbols(seq) + i));
          break;
        case (Array):
          /* type 4 */
          SEQ_array(new_seq)[i] = copy_sequence(SEQ_array(seq)[i]);
          break;
        case (List):
          /* type 5 */
          SEQ_list(new_seq)[i] = copy_sequence(SEQ_list(seq)[i]);
          break;
        case (Byte_block):
          /* type 5 */
          SEQ_byte_block(new_seq)[i] = copy_byte_block(SEQ_byte_block(seq)[i]);
	  break;
        }
    }

  return(new_seq);
}


OBJECTptr copy_object(OBJECTptr obj)
{       
  OBJECTptr new_obj = NULL;

  if (obj != NULL)
    {
      new_obj = make_object(OBJ_type(obj));

      switch (OBJ_type(obj))
        {
	case(Int):
	  OBJ_integer(new_obj) = OBJ_integer(obj);
	  break;
	case(Character):
	  fat_charcpy(OBJ_character(new_obj), OBJ_character(obj));
	  break;
	case(String):
	  OBJ_str(new_obj) = copy_fat_string(OBJ_str(obj));
          break;
	case(Symbol):
	  OBJ_symbol(new_obj) = copy_io_symbol(OBJ_symbol(obj));
	  break;
	case(Symbol_package):
	  prog_error("copy_object",
		     "Cannot copy a symbol package outside of a sequence.\n");
	  break;
	case(Array):
	  OBJ_array(new_obj) = copy_sequence(OBJ_array(obj));
          break;
	case(List):
	  OBJ_list(new_obj) = copy_sequence(OBJ_list(obj));
          break;
	case(Byte_block):
	  OBJ_byte_block(new_obj) = copy_byte_block(OBJ_byte_block(obj));
	  break;
        }
    }

  return(new_obj);
}

void copy_properties(NETptr source, NETptr target)
{

  PROPptr prop, next;
  FAT_STR attr;
  OBJECTptr val;

  prop = NET_properties(source);

  while (prop != NULL)
    {
      next = next_prop(prop);      
      attr = copy_fat_string(PROP_attr(prop));
      val  = copy_object(PROP_val(prop));
      add_prop(target, attr, val);
      prop = next;
    }
}



NETptr copy_fsm(NETptr old_net)
{
  NETptr new_net;
  
  if (NET_tabular(old_net) || NET_compacted(old_net))
    handle_error("Tabular and compacted nets cannot be copied.",
                 "COPY_FSM", 0);

  clear_client_cells(old_net);
  
  new_net = make_network();
  
  copy_states(old_net, new_net);
  
  NET_pruned(new_net) = NET_pruned(old_net);
  NET_visit_marks_dirty(new_net) = FALSE;
  NET_deterministic(new_net) = NET_deterministic(old_net);
  NET_completed(new_net) = NET_completed(old_net);
  NET_minimized(new_net) = NET_minimized(old_net);
  NET_epsilon_free(new_net) = NET_epsilon_free(old_net);
  NET_sorted_states(new_net) = NET_sorted_states(old_net);
  NET_loop_free(new_net) = NET_loop_free(old_net);
  NET_names_matter(new_net) = NET_names_matter(old_net);
  NET_shared_arc_lists(new_net) = FALSE;
  NET_has_arc_user_pointer(new_net) = NET_has_arc_user_pointer(old_net);
  NET_closed_sigma(new_net) = NET_closed_sigma(old_net);
  NET_arc_label_arity(new_net) = NET_arc_label_arity(old_net);
  NET_mark(new_net) = NET_mark(old_net);
  NET_upper_seq(new_net) = NET_upper_seq(old_net);
  NET_lower_seq(new_net) = NET_lower_seq(old_net);
  NET_u_flag_diacr(new_net) =   NET_u_flag_diacr(old_net);
  NET_l_flag_diacr(new_net) =   NET_l_flag_diacr(old_net);
  NET_left_bimachine(new_net) = NET_left_bimachine(old_net);
  NET_right_bimachine(new_net) = NET_right_bimachine(old_net);
  NET_reduced_labelset(new_net) =   NET_reduced_labelset(old_net);
  NET_optimized(new_net) =   NET_optimized(old_net);

  new_net->flags.unused = old_net->flags.unused;
  /*AK.12/03/97: unused 13 bits; copying them cannot do any harm */

  if (NET_pruned(old_net) &&
      NET_num_states(new_net) != NET_num_states(old_net))
    {
      fsm_fprintf(stderr, "*** Programmer alert!\n");
      fsm_fprintf(stderr, "    State count in COPY_FSM is %ld, should be %ld\n",
              (long) NET_num_states(old_net), (long) NET_num_states(new_net));
    }
  
  if (NET_pruned(old_net) && NET_num_arcs(new_net) != NET_num_arcs(old_net)
      && !NET_shared_arc_lists(old_net))
    {
      fsm_fprintf(stderr, "**** Programmer alert!\n");
      fsm_fprintf(stderr, "     Arc count in COPY_FSM is %ld, should be %ld\n",
              (long) NET_num_arcs(old_net), (long) NET_num_arcs(new_net));
    }
  
  binary_to_label(NET_labels(old_net));
  NET_labels(new_net) = copy_alphabet(NET_labels(old_net));
  NET_sigma(new_net) = copy_alphabet(NET_sigma(old_net));
  
  /* If the old net has a reduced labelset, we need to copy the
     SIGMA and EQV-VECTOR properties. What about other properties? */
  
  copy_properties(old_net, new_net);

  return(new_net);
}

NVptr copy_net_vector (NVptr old_vec)
{
  NVptr new_vec;  NETptr net;  int i;

  new_vec = make_nv (NV_len (old_vec));

  for (i=0; i< NV_len (old_vec); i++)
    {
      net = NV_net (old_vec, i);

      if (net)
	NV_net (new_vec, i) = copy_fsm (net);
      else
	NV_net (new_vec, i) = NULL;
    }

  return (new_vec);
}


/* ------------------------------------------------------------------ */
/*                                                                    */
/*           Copying selected parts of a network.                     */
/*                                                                    */
/* ------------------------------------------------------------------ */

/* The code in this section is a quick port of the correspoding functions
   in the old Common Lisp finite-state package. Recode into C at
   the request of John Maxwell for the benefit of XLE. The original
   functions are state_prefix_net() and state_suffix_net(). The
   state_to_state_subnet() function was added specifically for
   XLE generation. 2001-02-21 LK
*/


static int state_in_network_p(STATEptr state, NETptr net)
{
  /* Check that STATE belongs to NET. */

  STATEptr s;
  for (s = NET_states(net); s; s = next_state(s))
    if (s == state)
      break;

  return(s != NULL);
}


static NETptr copy_marked_states(NETptr net, STATEptr start_state,
				 STATEptr final_state)
{
  /* The working core of state_prefix_net() and state_suffix_net().
     The states of NET that should be copied have IN_PROCESS as their
     visit mark. Returns the network consisting of the copied
     states.

     If START_STATE is not NULL, its copy is marked ast the start
     state of the subnetwork and we are in the process of creating
     a suffix network. If FINAL_STATE is not NULL, its copy will be
     final, all other states of the subnetwork will be non-final
     and the result is a prefix network.

     The copied states are temporarily stored into the client cell
     of the corresponding original state.
  */

  STATEptr state, dest, new_state;
  ARCptr arc;
  id_type id;
  ALPHABETptr sigma, labels;
  NETptr subnet = make_network();

  if (!start_state)
    start_state = NET_start_state(net);

  for (state = NET_states(net); state; state = next_state(state))
    if (STATE_visit_mark(state) == IN_PROCESS)
      {
	new_state = copy_state(state, subnet);

	/* Note: the copied arcs still point to the original destination. */

	if (final_state)
	  {
	    if (state == final_state)
	      {
		/* Mark the final state of the prefix network.
		   It will not have any arcs.
		*/
		
		STATE_final(new_state) = TRUE;
		STATE_deterministic(new_state) = TRUE;
	      }
	    else
	      STATE_final(new_state) = FALSE;
	  }

	STATE_client_cell(state) = new_state;
      }
   
  /* Reroute the copied arcs to their destination in the subnet.
     Collect the label and sigma alphabet of the subnet.
     Set the arity and epsilon_free flag as appropriate.
  */

  sigma = make_alph(0, BINARY_VECTOR);
  labels = make_alph(0, BINARY_VECTOR);

  NET_epsilon_free(subnet) = TRUE;

  for (state = NET_states(net); state; state = next_state(state))
    if (STATE_visit_mark(state) == IN_PROCESS)
      {
	STATE_visit_mark(state) = NOT_VISITED;
	new_state = (STATEptr) STATE_client_cell(state);

	for (arc = STATE_arc_set(new_state); arc; arc = next_arc(arc))
	  {
	    dest = (STATEptr) STATE_client_cell(ARC_destination(arc));

	    ARC_destination(arc) = dest;

	    if (final_state)
	      {
		/* We are making a prefix network. Arcs will have to
		   be inverted. We set the mark here to IN_PROCESS so that
		   it will be reset to NOT_VISITED when the arc is inverted.
		*/
		
		ARC_visit_mark(arc) = IN_PROCESS;
	      }

	    id = ARC_label(arc);

	    if (id == EPSILON)
	      NET_epsilon_free(subnet) = FALSE;
	    else
	      sigma_add_to(labels, id);

	    if (tuple_id_p(id))
	      {
		sigma_add_to(sigma, upper_id(id));
		sigma_add_to(sigma, lower_id(id));
		NET_arc_label_arity(subnet) = 2;
	      }
	    else
	      sigma_add_to(sigma, id);
	  }
      }

  /* Assign start state, sigma, and label alphabets to the result.
    Copy the flags that must still be valid. */

  NET_start_state(subnet) = (STATEptr) STATE_client_cell(start_state);
  NET_labels(subnet) = binary_to_label(labels);
  NET_sigma(subnet) = sigma;
  NET_deterministic(subnet) = NET_deterministic(net);
  NET_pruned(subnet) = NET_pruned(net);
  NET_loop_free(subnet) = NET_loop_free(net);

  /* Clean the client cells of the original net. */

  for (state = NET_states(net); state; state = next_state(state))
    STATE_client_cell(state) = NULL;

  return(subnet);
}

NETptr state_suffix_net(NETptr net, STATEptr state)
{
  /* Returns the network consisting of all paths leading from
     STATE to a final state.

     WARNING: Any information stored in the client cells or
     in the state visit marks in the original network will be
     lost. Arc visit marks are not affected.

     The state client cells of NET are used temporarily to
     store a copy of a state that is being copied into the
     suffix net. The state visit marks are first reset to
     NOT_VISITED and used by the algorithm.

     At the end, the state client cells of the original network
     are cleared to NULL. All state visit marks are left at NOT_VISITED.
     The result is deterministic if the original net has this
     property but minimality is not preserved.
  */
  
  NETptr suffix_net;

  if (!state_in_network_p(state, net))
    {
      fsm_fprintf(stderr, "Start state does not belong to the net\n");
      return(null_fsm());
    }

  /* Need to clear state marks because they are used to
     mark the prefix region and the state client cells because
     the copy of the state is stored there. We also need
     No reason to touch arc visit marks. */

   clear_state_marks_and_client_cells(net);

   /* Mark states that are on a path leading from STATE */

   mark_reachable_states(state, IN_PROCESS, NULL);

   suffix_net = copy_marked_states(net, state, NULL);
   
   return(suffix_net);
}

NETptr state_prefix_net(NETptr net, STATEptr state)
{

  /* Returns the network consisting of all paths leading to STATE.

     WARNING: Any information stored in the client cells or
     in the visit marks in the original network will be lost.

     The state client cells of NET are used temporarily to
     store a copy of a state that is being copied into the
     prefix net. The visit marks of arcs and states are first
     reset to 0 and used by the algorithm.

     At the end, the state client cells of the original network
     are cleared to NULL. All visit marks are left at 0.
     The result is deterministic if the original net has this
     property but minimality is not preserverd.
  */
  
  NETptr prefix_net;

  if (!state_in_network_p(state, net))
    {
      fsm_fprintf(stderr, "End state does not belong to the net\n");
      return(null_fsm());
    }

  /* Need to clear state marks because they are used to
     mark the prefix region and the state client cells because
     the copy of the state is stored there. We also need
     to clear arc visit marks to indicate that the arc has
     been inverted. */

   clear_all_visit_marks_and_state_client_cells(net);

   /* Mark states that are on a path leading to STATE */

   invert_arcs(net, IN_PROCESS);
   NET_visit_marks_dirty(net) = TRUE;
   mark_reachable_states(state, IN_PROCESS, NULL);
   
   /* Start state must be reachable. */

   if (STATE_visit_mark(NET_start_state(net)) == NOT_VISITED)
     {
       fsm_fprintf(stderr, "End state is not reachable.\n");
       invert_arcs(net, NOT_VISITED);
       return(null_fsm());
     }

   prefix_net = copy_marked_states(net, NULL, state);

  invert_arcs(net, NOT_VISITED);
  NET_visit_marks_dirty(net) = FALSE;

  invert_arcs(prefix_net, NOT_VISITED);
  NET_visit_marks_dirty(prefix_net) = FALSE;

  return(prefix_net);
}



NETptr state_to_state_subnet(NETptr net, STATEptr start, STATEptr end)
{
  /* Returns the network consisting of all paths leading from START
     to END in NET.

     WARNING: Any information stored in the client cells or
     in the visit marks in the original network will be lost.

     The state client cells of NET are used temporarily to
     store a copy of a state that is being copied into the
     subnet. The visit marks of arcs and states are first
     reset to 0 and used by the algorithm.

     At the end, the state client cells of the original network
     are cleared to NULL. All visit marks are left at 0.
     The result is deterministic if the original net has this
     property but minimality is not preserved.
  */
  
  STATEptr state, real_start_state;
  NETptr subnet;
  int start_ok = FALSE;
  int end_ok = FALSE;

  /* Check that START and END belong to NET. */

  for (state = NET_states(net); state; state = next_state(state))
    {
      if (state == start)
	start_ok = TRUE;
      else if (state == end)
	end_ok = TRUE;

      if (start_ok && end_ok)
	break;
    }

  if (state == NULL)
    {
      if (start_ok)
	fsm_fprintf(stderr, "End state does not belong to the net\n");
      else if (end_ok)
	fsm_fprintf(stderr, "Start state does not belong to the net\n");
      else
	fsm_fprintf(stderr, "States do not belong to the net\n");

      return(null_fsm());
    }

  real_start_state = NET_start_state(net);

  NET_start_state(net) = start;
  subnet = state_prefix_net(net, end);

  NET_start_state(net) = real_start_state;

  return(subnet);
}

#endif /* SLIM_FST */
