/* $ Id: virtual-fsm.c,v 1.175 2001/02/16 22:30:19 maxwell Exp $
 * 
 * Copyright (C) 1994-2004, Xerox Corporation. All rights reserved.
 * 
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#if defined(SOLARIS) || defined(_SOLARIS)
#include <alloca.h>
#endif
#include "fst/types.h"
#include "fst/modify.h"
#include "fst/btree.h"
#include "fst/cons.h"
#include "fst/label.h"
#include "fst/mark.h"
#include "fst/calculus.h"
#include "fst/pars-tbl.h"
#include "fst/bin-in.h"       /* For get_number in the virtual net restoration */
#include "fst/virtual-fsm.h"
#include "fst/virtual-base.h" /* for low-level function such as dead_state_p() */
#include "fst/machine.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 


/* The licensable version (i.e. WORLD version) does not include this code:
   skip the whole file. */
#ifndef WORLD

void init_virtual_fsm_restore_function()
{
    /* Loading of virtual networks: */
    restore_functions[TypeRestoreVirtual]= get_virtual_net;
}

/* ------------------------------------------------------------------------ */
/* DESIGN NOTES by John Maxwell                                             */
/* ------------------------------------------------------------------------ */

/* Each virtual state stores state data in the arc set and network data
   in the client cell.  We can use the arc set of a virtual state because
   a virtual state doesn't have any arcs until it is realized, and a
   real state doesn't need the state data.  The network data has methods
   for converting the state data into arcs and a final bit.  The state
   data can be a single state, a pair of states, or a list of states
   depending on the virtual operation being simulated. */

/* The n-ary operations like compose and concat are implemented here as
   binary operations.  This simplifies the procedure calls and the data
   structures.  N-ary versions of these operations could be implemented
   by using lists of states if this was important.  But apart from
   simplicity, converting n-ary operations into binary operations has
   the advantage that it caches the results for right-end combinations 
   of operands, which may make it quicker to use them when they combine 
   with new states in the left-ward operands. */

/* Another possible optimization is to convert small virtual machines
   into real machines.  For instance, the virtual concatenation of a
   number of small machines could be realized as a single real machine
   that was small. */

/* ------------------------------------------------------------------------ */
/* PROTOTYPES FOR INTERNAL FUNCTIONS                                        */
/* ------------------------------------------------------------------------ */

NETptr virtual_network(NETptr op1, NETptr op2, VMethods *methods,
		       void *extra_info, void *added_sigma);
void vcommon_sigma(NETptr vnetwork, ALPHABETptr common_sigma);
void vtorfsm1(STATEptr vstate, NETptr net);
int small_real_network_p(NETptr network, int max_states, int max_arcs);
STATEptr vget_state(StateData *state_data, VirtualInfo *vinfo);
ARCptr veps_arc(STATEptr to_vstate, VirtualInfo *vinfo);
StatePair *create_state_pair(STATEptr state1, STATEptr state2);
ARCptr copy_arc_set(STATEptr state, NETptr net);
ARCptr vcopy_arc_set(STATEptr state, VirtualInfo *vinfo);
STATEptr vget_pair_state(StatePair *state_pair, int ignore_epsilons, 
			 VirtualInfo *vinfo);
STATEptr vget_single_state(STATEptr data_state, VirtualInfo *vinfo);
STATEptr vget_list_state(CONSptr states, VirtualInfo *vinfo);
int vget_pair_final_p(STATEptr state, VirtualInfo *vinfo);
ARCptr append_arc_list(ARCptr list1, ARCptr list2);
ARCptr vcopy_arc(ARCptr arc, VirtualInfo *vinfo);
ARCptr vmake_arc(id_type label, StateData *state_data, VirtualInfo *vinfo);
int veps_remove_final_p(STATEptr state, VirtualInfo *vinfo);
NETptr vlookup(char *string, NETptr fst, int no_print);
NETptr vlookdown(char *string, NETptr fst, int no_print);
void flush_realized_arcs(STATEptr state, VirtualInfo *vinfo);
int state_in_network(STATEptr state, NETptr net);

#define DONT_RECURSE 0
#define DO_RECURSE 1
void flush_virtual_arcs(NETptr vnetwork, int recurse);
void flush_virtual_other_arcs(NETptr vnetwork, int recurse);
void flush_virtual_states(NETptr vnetwork, int recurse);
BTREEptr btree_insert_address(void *item, BTREEptr tree, HEAPptr bt_heap);

/* ------------------------------------------------------------------------ */
/* FUNCTIONS FOR MANIPULATING THE STATE DATA                                */
/* ------------------------------------------------------------------------ */

STATEptr get_data_state(STATEptr state) {
  if (STATE_is_virtual(state)) {
    VirtualInfo *vinfo = STATE_virtual_info(state);
    assert(vinfo->methods->data_type == DATA_STATE);
  }
  return (STATEptr)STATE_state_data(state);
}

StatePair *get_state_pair(STATEptr state) {
  if (STATE_is_virtual(state)) {
    VirtualInfo *vinfo = STATE_virtual_info(state);
    assert(vinfo->methods->data_type == STATE_PAIR);
  }
  return (StatePair *)STATE_state_data(state);
}

CONSptr get_state_list(STATEptr state) {
  if (STATE_is_virtual(state)) {
    VirtualInfo *vinfo = STATE_virtual_info(state);
    assert(vinfo->methods->data_type == STATE_LIST);
  }
  return (CONSptr)STATE_state_data(state);
}

StatePair *create_state_pair(STATEptr state1, STATEptr state2) {
  StatePair *pair;
  pair = (StatePair *)calloc(sizeof(StatePair), 1);
  pair->state1 = state1;
  pair->state2 = state2;
  return pair;
}

STATEptr vget_state(StateData *state_data, VirtualInfo *vinfo) {
  /* ---------------------------------------------------------- */
  /* Create a state for state_data.  The default depends on the */
  /* data type.  State are created without their outgoing arcs  */
  /* or their final bits; the final_p_fn and arc_set_fn will be */
  /* called if needed to produce them.                          */
  /* ---------------------------------------------------------- */
  if (vinfo->methods->data_type == DATA_STATE) {
    return vget_single_state(state_data, vinfo);
  } else if (vinfo->methods->data_type == STATE_PAIR) {
    return vget_pair_state(state_data, 0, vinfo);
  } else {
    assert(vinfo->methods->data_type == STATE_LIST);
    return vget_list_state(state_data, vinfo);
  }
}

STATEptr vget_pair_state(StatePair *state_pair, int ignore_epsilons, 
			 VirtualInfo *vinfo) {
  /* --------------------------------------- */
  /* Returns a state given a pair of states. */
  /* Creates a btree of btrees.              */
  /* --------------------------------------- */
  BTREEptr node1;
  BTREEptr node2;
  STATEptr new_state;
  STATEptr state1, state2;
  state1 = state_pair->state1;
  state2 = state_pair->state2;
  if (state1) assert(state_in_network(state1, vinfo->op1));
  if (state2) assert(state_in_network(state2, vinfo->op2));
  if (ignore_epsilons) {
    /* ----------------------------------------------------------- */
    /* Swap the states so that we get a different resulting state. */
    /* ----------------------------------------------------------- */
    STATEptr temp = state1;
    state1 = state2;
    state2 = temp;
  }
  node1 = btree_insert_address(state1, vinfo->btree, vinfo->btree_heap);
  if (!vinfo->btree) vinfo->btree = node1;
  node2 = (BTREEptr)node1->value;
  node2 = btree_insert_address(state2, node2, vinfo->btree_heap);
  if (!node1->value) node1->value = node2;
  if (node2->value) {
    free(state_pair);
    return (STATEptr)node2->value;
  }
  /* ------------------------------------------------------------- */
  /* Create a new state, initialize it, and add it to the network. */
  /* ------------------------------------------------------------- */
  new_state = make_state(NULL, vinfo->network);
  STATE_deterministic(new_state) = FALSE;
  node2->value = new_state;
  STATE_is_virtual(new_state) = TRUE;
  STATE_virtual_info(new_state) = vinfo;
  STATE_state_data(new_state) = state_pair;
  new_state->unused = ignore_epsilons;
  return new_state;
}

STATEptr vget_single_state(STATEptr data_state, VirtualInfo *vinfo) {
  BTREEptr node;
  STATEptr new_state;
  assert(state_in_network(data_state, vinfo->op1));
  node = btree_insert_address(data_state, vinfo->btree, 
			      vinfo->btree_heap);
  if (!vinfo->btree) vinfo->btree = node;
  if (node->value) return (STATEptr)node->value;
  new_state = make_state(NULL, vinfo->network);
  STATE_deterministic(new_state) = STATE_deterministic(data_state);
  node->value = new_state;
  STATE_is_virtual(new_state) = TRUE;
  STATE_virtual_info(new_state) = vinfo;
  STATE_state_data(new_state) = data_state;
  return new_state;
}

void sort_list(CONSptr list) {
  CONSptr tail;
  STATEptr first, second;
  if (!list || !cdr(list)) return;
  tail = cdr(list);
  sort_list(tail);
  first = (STATEptr)car(list);
  second = (STATEptr)car(tail);
  if ((int)first < (int)second) return;
  car(list) = second;
  car(tail) = first;
  sort_list(tail);
}

int state_in_network(STATEptr state, NETptr network) {
  STATEptr s;
  return 1;
  for (s = NET_states(network); s; s = next_state(s)) {
    if (s == state) return 1;
  }
  return 0;
}

int equal_lists(CONSptr l1, CONSptr l2) {
  while (1) {
    if (l1 == l2) return 1;
    if (l1 && !l2) return 0;
    if (l2 && !l1) return 0;
    if (car(l1) != car(l2)) return 0;
    l1 = cdr(l1);
    l2 = cdr(l2);
  }
  assert(0);
  return 0;
}

STATEptr vget_list_state(CONSptr states, VirtualInfo *vinfo) {
  CONSptr tail;
  STATEptr state, new_state;
  BTREEptr node;
  BTREEptr *root;
  HEAPptr heap = vinfo->btree_heap;
  
  int length = 0;
  /* ---------------------------------------------- */
  /* Sort the states so that the list is canonical. */
  /* ---------------------------------------------- */
  sort_list(states);
  /* ------------------------------ */
  /* Index on the number of states. */
  /* ------------------------------ */
  for (tail = states; tail; tail = cdr(tail)) length++;
  root = (BTREEptr *)&vinfo->btree;
  node = btree_insert_address((void *)length, *root, heap);
  if (!*root) *root = node;
  /* ------------------------------------- */
  /* Then index on each state iteratively. */
  /* ------------------------------------- */
  for (tail = states; tail; tail = cdr(tail)) {
    state = car(tail);
    assert(state_in_network(state, vinfo->op1));
    root = (BTREEptr *)&node->value;
    node = btree_insert_address(state, *root, heap);
    if (!*root) *root = node;
  }
  /* ----------------------------------------- */
  /* Return the state at the end if it exists. */
  /* Otherwise, create a new state.            */
  /* ----------------------------------------- */
  if (node->value) {
    new_state = node->value;
    while (states) {
      tail = cdr(states);
      free(states);
      states = tail;
    }
    return new_state;
  }
  new_state = make_state(NULL, vinfo->network);
  STATE_deterministic(new_state) = FALSE;
  node->value = new_state;
  STATE_is_virtual(new_state) = TRUE;
  STATE_virtual_info(new_state) = vinfo;
  STATE_state_data(new_state) = states;
  return new_state;
}



int sigma_count = 0;
ALPHABETptr sigma_array[100];
int sigma_free[100];

/* ------------------------------------------------------------------------ */
/* GENERAL FUNCTIONS FOR CREATING VIRTUAL NETWORKS                          */
/* ------------------------------------------------------------------------ */

int virtual_network_id = 1;

/* Need this declaration here. Definition later. */

static VMethods *get_virtual_methods(int operation_id);

static VirtualInfo *make_virtual_info(NETptr vnet, NETptr op1, NETptr op2,
				      VMethods *methods, void *extra_info)
{
  VirtualInfo *vinfo;

  vinfo = (VirtualInfo *)calloc(sizeof(VirtualInfo), 1);
  vinfo->op1 = op1;
  vinfo->op1_reclaimable = FALSE;
  vinfo->op2 = op2;
  vinfo->op2_reclaimable = FALSE;
  vinfo->methods = methods;
  vinfo->extra_info = extra_info;
  vinfo->network = vnet;
  vinfo->btree_heap = make_btree_heap();
  vinfo->id = virtual_network_id++;

  return(vinfo);
}

static STATEptr make_virtual_start_state(NETptr op1, NETptr op2,
					 VMethods *methods,
					 VirtualInfo *vinfo)
{

  /* ------------------------------ */
  /* Create a virtual start state.  */
  /* ------------------------------ */
  StateData *state_data;
  STATEptr start1, start2;

  start1 = NET_start_state(op1);
  if (methods->data_type == DATA_STATE) {
    assert(!op2);
    state_data = start1;
  } else if (methods->data_type == STATE_PAIR) {
    assert(op2);
    start2 = NET_start_state(op2);
    state_data = create_state_pair(start1, start2);
  } else {
    CONSptr cons;
    assert(methods->data_type == STATE_LIST);
    assert(!op2);
    cons = (CONSptr)calloc(sizeof(CONStype), 1);
    car(cons) = start1;
    state_data = cons;
  }
 
  return(vget_state(state_data, vinfo));
}

NETptr virtual_network(NETptr op1, NETptr op2, VMethods *methods,
		       void *extra_info, void *added_sigma) {
  /* --------------------------------------------------------------------- */
  /* Primitive function for creating and initializing a virtual network.   */
  /* Insures that all operands have a shared idea of the sigma.            */
  /* Some functions add their own symbol (e.g. OTHER in the case of        */
  /* negation), which they declare through added_sigma.                    */
  /* NOTE: op1 and op2 are marked as non-reclaimable by default. If that   */
  /* is not the case, the caller should reset the flags by calling         */
  /* make_virtual_operands_reclaimable() on the resulting virtual net.     */
  /* If the operands are marked as reclaimable, they will be freed.        */
  /* when the virtual network itself is reclaimed.                         */
  /* --------------------------------------------------------------------- */
  VirtualInfo *vinfo;
  NETptr vnetwork;
  ALPHABETptr common_sigma;
  STATEptr start;
  
  vnetwork = make_network();
  NET_is_virtual(vnetwork) = (fst_bit)TRUE;
  NET_names_matter(vnetwork) = TRUE;
  assert(op1 != vnetwork);
  assert(op2 != vnetwork);
  
  vinfo = make_virtual_info(vnetwork, op1, op2, methods, extra_info);
  NET_virtual_info(vnetwork) = vinfo;

  if (!op1) {
    assert(!op2);
    start = make_state(NULL, vnetwork);
    STATE_deterministic(start) = FALSE;
    STATE_is_virtual(start) = TRUE;
    STATE_virtual_info(start) = vinfo;
    NET_start_state(vnetwork) = start;
    NET_sigma(vnetwork) = make_alph(100, BINARY_VECTOR);
    return vnetwork;
  }

  start = make_virtual_start_state(op1, op2, methods, vinfo);

  NET_start_state(vnetwork) = start;

  /* --------------------------------------------- */
  /* Make all of the networks have the same sigma. */
  /* --------------------------------------------- */
  if (op2) {
    common_sigma = sigma_union(NET_sigma(op1), NET_sigma(op2));
    vcommon_sigma(op1, common_sigma);
    vcommon_sigma(op2, common_sigma);
  } else {
    common_sigma = copy_alphabet(NET_sigma(op1));
  }
  NET_sigma(vnetwork) = common_sigma;
  NET_labels(vnetwork) = make_alph(0, LABEL_VECTOR);
  
#if 0
  if (vinfo->methods->initialize_fn) {
    vinfo->methods->initialize_fn(vnetwork, vinfo);
  }
#endif
  return vnetwork;
}


void vcommon_sigma(NETptr vnetwork, ALPHABETptr common_sigma) {
  /* ----------------------------------------------------------- */
  /* Ensures that vnetwork and all its operands recursively have */
  /* common_sigma.                                               */
  /* ----------------------------------------------------------- */
  ALPHABETptr sigma, new_sigma;
  if (!vnetwork) return;
  sigma = NET_sigma(vnetwork);
  if (sigma) {
    label_to_binary(sigma);
    new_sigma = sigma_difference(common_sigma, sigma);
  } else {
    if (!common_sigma) return;
    new_sigma = copy_alphabet(common_sigma);
  }
  if (!new_sigma) return;
#if 0
  /* --------------------------- */
  /* Remove tags from new_sigma. */
  /* --------------------------- */
  {int i, sigma_len;
  new_sigma = binary_to_label(new_sigma);
  sigma_len = ALPH_len(new_sigma);
  for (i = 0; i < sigma_len; i++) {
    if (!id_tag_p(ALPH_item(new_sigma, i))) continue;
    new_sigma = sigma_remove_from(new_sigma, ALPH_item(new_sigma, i));
  }
#endif
  if (ALPH_len(new_sigma) == 0) {
    free_alph(new_sigma);
    return;
  }
  /* ------------------------------------------------------------ */
  /* new_sigma is the set of symbols in common_sigma that are new */
  /* to this vnetwork and, by the invariant established by the    */
  /* recursion, all of its subcomponents.  We recursively update  */
  /* everything.                                                  */
  /* ------------------------------------------------------------ */
  if (NET_is_virtual(vnetwork)) {
    VirtualInfo *vinfo = NET_virtual_info(vnetwork);
    /* ------------------------------------------------------- */
    /* Flush virtual arcs in case there is an OTHER that needs */
    /* to be updated.  Then recurse.                           */
    /* ------------------------------------------------------- */
    flush_virtual_other_arcs(vnetwork, DONT_RECURSE);
    if (vinfo->methods->expand_other_fn) {
      vinfo->methods->expand_other_fn(new_sigma, vnetwork, vinfo);
    } else {
      vcommon_sigma(vinfo->op1, common_sigma);
      vcommon_sigma(vinfo->op2, common_sigma);
    }
  } else {
    if (NET_closed_sigma(vnetwork)) {
      /* RMK: We don't expect to find OTHER in a machine with a closed 
	 alphabet.  If we do, someone was lying, but we try to make it 
	 good. */
      substitute_symbol(OTHER, new_sigma, vnetwork, DONT_COPY, FALSE);
    } else {
      sigma_add_to(new_sigma, OTHER);
      substitute_symbol(OTHER, new_sigma, vnetwork, DONT_COPY, FALSE);
    }
  }
  free_alph(new_sigma);
  if (NET_sigma(vnetwork)) {
    free_alph(NET_sigma(vnetwork));
  }
  NET_sigma(vnetwork) = copy_alphabet(common_sigma);
  if (NET_upper_parse_table(vnetwork)) {
    free_parse_table(NET_upper_parse_table(vnetwork));
    NET_upper_parse_table(vnetwork) = NULL;
  }
  if (NET_lower_parse_table(vnetwork)) {
    free_parse_table(NET_lower_parse_table(vnetwork));
    NET_lower_parse_table(vnetwork) = NULL;
  }
}

ARCptr veps_arc(STATEptr to_vstate, VirtualInfo *vinfo) {
  /* RMK: Creates a (virtual) epsilon arc to to_vstate, but relies on 
     caller to actually install.  That way, we wouldn't be 
     second-guessing the behavior of virtual_arc_set -- in some cases it 
     might not want to cache the arcset in the state. */
  if (dead_state_p(to_vstate)) return NULL;
  if (NET_epsilon_free(vinfo->network)) {
    /* RMK: If the virtual machine is marked as epsilon-free, we preserve 
       this property by adding a copy of the TOVSTATE's arcset (and hence 
       realizing it) instead of merely creating an epsilon arc.  This is 
       safe because in this situation the finality of a state is 
       independent of its arcset, hence known in advance.  If the caller 
       wants to suppress this behavior, then it should not set the 
       epsilonfree bit. */

    /* LK: The resulting network is not pruned unless the
       TO_VSTATE happens to have incoming arcs from its
       own network. We don't know this, so the pruned flag
       needs to be set to FALSE. */
    NET_pruned(vinfo->network) = FALSE;
    return copy_arc_set(to_vstate, vinfo->network);
  } else {
    return make_arc(EPSILON, to_vstate, vinfo->network);
  }
}

ARCptr copy_arc_set(STATEptr state, NETptr net) {
  /* ---------------------------------------- */
  /* Copy state's arcset in the same machine. */
  /* ---------------------------------------- */
  ARCptr arc, new_arc;
  ARCptr arc_list = NULL;
  for (arc = virtual_arc_set(state); arc; arc = next_arc(arc)) {
    new_arc = make_arc(ARC_label(arc), ARC_destination(arc), net);
    new_arc->next = arc_list;
    arc_list = new_arc;
  }
  return arc_list;
}

ARCptr vcopy_arc_set(STATEptr state, VirtualInfo *vinfo) {
  /* ------------------------------------------------------ */
  /* Copies state's arcset up into vinfo's virtual machine. */
  /* ------------------------------------------------------ */
  ARCptr arc, new_arc;
  ARCptr arc_list = NULL;
  STATEptr data_state = get_data_state(state);
  for (arc = virtual_arc_set(data_state); arc; arc = next_arc(arc)) {
    new_arc = vcopy_arc(arc, vinfo);
    if (!new_arc) continue;
    new_arc->next = arc_list;
    arc_list = new_arc;
  }
  return arc_list;
}

int vget_pair_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  StatePair *state_pair = get_state_pair(vstate);
  return virtual_final_p(state_pair->state1) && 
    virtual_final_p(state_pair->state2);
}

ARCptr append_arc_list(ARCptr list1, ARCptr list2) {
  ARCptr tail;
  if (!list2) return list1;
  if (!list1) return list2;
  tail = list1;
  while (tail->next) {tail = tail->next;}
  tail->next = list2;
  return list1;
}

ARCptr vcopy_arc(ARCptr arc, VirtualInfo *vinfo) {
  /* ----------------------------- */
  /* Builds a virtual copy of arc. */
  /* ----------------------------- */
  STATEptr dest = NULL;
  if (dead_state_p(ARC_destination(arc))) return NULL;
  assert(vinfo->methods->data_type == DATA_STATE);
  dest = vget_state(ARC_destination(arc), vinfo);
  if (!dest) return NULL;
  return make_arc(ARC_label(arc), dest, vinfo->network);
}

ARCptr vmake_arc(id_type label, StateData *state_data, VirtualInfo *vinfo) {
  /* ------------------------------------------------ */
  /* Builds an arc for label to the state_data state. */
  /* ------------------------------------------------ */
  STATEptr dstate;
  dstate = vget_state(state_data, vinfo);
  if (0 && dead_state_p(dstate)) return NULL;
  return make_arc(label, dstate, vinfo->network);
}



/* ------------------------------------------------------------------------ */
/* GENERAL UTILITY FUNCTIONS THAT OPERATE ON VIRTUAL FSMS                   */
/* ------------------------------------------------------------------------ */

static void clear_real_operand_marks(NETptr net)
{
  VirtualInfo *vinfo;

  if (!net)
    return;

  if (NET_is_virtual(net))
    {
      vinfo = NET_virtual_info(net);
      clear_real_operand_marks(vinfo->op1);
      clear_real_operand_marks(vinfo->op2);
    }
  else
    clear_state_marks(net);
}

static void mark_accessible_states(STATEptr state)
{
  ARCptr arc;

  if (STATE_is_virtual(state) || STATE_visit_mark(state) == IN_PROCESS)
    return;

  STATE_visit_mark(state) = IN_PROCESS;

  for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
    mark_accessible_states(ARC_destination(arc));

}

static void move_visited_states(NETptr old_net, NETptr new_net)
{
  /* Move the states in old_net that have been visited into
     the new_net. Fix the arc_count as well. */

  ARCptr arc;
  STATEptr state, next;
  STATEptr prev = NULL;

  /* If we are on the top level already there is nothing to do. */

  if (old_net == new_net)
    return;

  for (state = NET_states(old_net); state; state = next)
    {
      next = next_state(state);

      if (STATE_visit_mark(state) == IN_PROCESS)
	{
	  /* Make sure the client cell is empty. Otherwise
	     free_virtual_network_storage() will try
	     to free a STATE_PAIR or a STATE_LIST. */

	  STATE_client_cell(state) = NULL;
	  STATE_visit_mark(state) = NOT_VISITED;

	  next_state(state) = NET_states(new_net);

	  if (prev)
	    next_state(prev) = next;
	  else
	    NET_states(old_net) = next;

	  NET_states(new_net) = state;
	  NET_num_states(new_net)++;
	  NET_num_states(old_net)--;

	  /* Add the states arcs to the arc count of new_net. */

	  for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	    {
	      NET_num_arcs(new_net)++;
	      NET_num_arcs(old_net)--;
	    }
	}
      else
	prev = state;
    }
}


static void collect_states_from_op(NETptr vnetwork, NETptr op)
{
  if (!op)
    return;

  if (NET_is_virtual(op))
    {
      VirtualInfo *vinfo = NET_virtual_info(op);

      collect_states_from_op(vnetwork, vinfo->op1);
      collect_states_from_op(vnetwork, vinfo->op2);

      /* The "success state" of a negated network is not
	 on the state list of its real operand but on the state
	 list of the virtual network itself. In this special case
	 we also need to scan the states of the virtual net.
      */

      if (vinfo->methods->operation_id == Virtual_Negation)
	move_visited_states(op, vnetwork);
    }
  else
    move_visited_states(op, vnetwork);
}



static void collect_states_from_real_operands(NETptr vnetwork)
{
  /* Set all the state visit marks in vnetwork and its
     operand networks to NOT_VISITED. */

  clear_state_marks(vnetwork);
  clear_real_operand_marks(vnetwork);

  /* Mark states that are accessible from the virtual net. */

  mark_accessible_states(NET_start_state(vnetwork));

  NET_visit_marks_dirty(vnetwork) = TRUE;

  collect_states_from_op(vnetwork, vnetwork);

  clear_state_marks(vnetwork);

  NET_visit_marks_dirty(vnetwork) = FALSE;
}




NETptr virtual_to_real_fsm(NETptr vnetwork, int minimize_p) {
  /* RMK: Causes all the arcs and states of vnetwork to be realized.  
     Depends on the fact that vget_state pushes states on the statelist 
     as well as in whatever hashing structure it is using. */ 
  if (NET_is_virtual(vnetwork)) {
    STATEptr state;
    for (state = NET_states(vnetwork); state; state = next_state(state)) {
      vtorfsm1(state, vnetwork);
    }

    /* The arcs of vnetwork may access states that are on the
       state list of some operand network but not yet in vnetwork
       itself. At this point we need to fetch them and make them
       part of the vnetwork.  2001-06-11 LK */

    collect_states_from_real_operands(vnetwork);

    free_virtual_network_storage(vnetwork);
    NET_is_virtual(vnetwork) = FALSE;
    NET_virtual_info(vnetwork) = NULL;

    update_label_alphabet(vnetwork);

    prune_fsm(vnetwork, DONT_COPY);
   
 if (minimize_p)
   {
     min_fsm(vnetwork, DONT_COPY);
     compact_sigma(vnetwork);
   }
  }
  return(vnetwork);
}

void vtorfsm1(STATEptr state, NETptr net) {
  ARCptr arc;
  VirtualInfo *vinfo = NET_virtual_info(net);
  if (STATE_is_virtual(state)) {
    /* RMK: The following will stop the recursion, since the state 
       is real before its destinations are explored. */
    assert(STATE_virtual_info(state) == vinfo);
    STATE_arc_set(state) = virtual_arc_set(state);
    STATE_is_virtual(state) = FALSE;
    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc)) {
      vtorfsm1(ARC_destination(arc), net);
    }
  }
}

int virtual_network_p(NETptr net) {
  return NET_is_virtual(net);
}

static void inherit_arity(NETptr vnet, NETptr op1, NETptr op2)
{
  if (NET_arc_label_arity(op1) == 2 || (op2 && NET_arc_label_arity(op2) == 2))
    NET_arc_label_arity(vnet) = 2;
}

static void inherit_epsilon_free(NETptr vnet, NETptr op1, NETptr op2)
{
  assert(op1);

  NET_epsilon_free(vnet) =
    (NET_epsilon_free(op1) && (!op2 || NET_epsilon_free(op2)));
}


static void inherit_closed_sigma(NETptr vnet, NETptr op1, NETptr op2)
{
  assert(op1);

  NET_closed_sigma(vnet) =
    (NET_closed_sigma(op1) && (!op2 || NET_closed_sigma(op2)));
}

static void inherit_deterministic(NETptr vnet, NETptr op1, NETptr op2)
{
  NET_deterministic(vnet) =
    (NET_deterministic(op1) && (!op2 || NET_deterministic(op2)));
}

static void inherit_loop_free(NETptr vnet, NETptr op1, NETptr op2)
{
  assert(op1);

  NET_loop_free(vnet) =
    (NET_loop_free(op1) && (!op2 || NET_loop_free(op2)));
}

static void inherit_minimized(NETptr vnet, NETptr op)
{
  NET_minimized(vnet) = NET_minimized(op);
}

static void inherit_pruned(NETptr vnet, NETptr op1, NETptr op2)
{
  NET_pruned(vnet) = (NET_pruned(op1) && (!op2 || NET_pruned(op2)));
}


static void fetch_max_label(id_type *max, ARCptr arc_list)
{
  ARCptr arc;

  for (arc = arc_list; arc; arc = next_arc(arc))
    if (ARC_label(arc) > *max)
      *max = ARC_label(arc);
}

static void index_arcs_by_label(ARCptr arcs, ARCptr *index, int side) {
  /* ----------------------------------------------------------- */
  /* This index arcs in index.  The arcs remain linked together, */
  /* but are reordered so that arcs with the same label are      */
  /* together in the list.                                       */
  /* ----------------------------------------------------------- */
  ARCptr arc, next_arc, prior = NULL;
  for (arc = arcs; arc; arc = next_arc) {
    id_type label, label2;
    label = ARC_label(arc);
    if (tuple_id_p(label)) {
      if (side == UPPER) label = id_to_upper_id(label);
      if (side == LOWER) label = id_to_lower_id(label);
    }
    next_arc = next_arc(arc);
    if (index[label]) {
      label2 = ARC_label(prior);
      if (tuple_id_p(label2)) {
	if (side == UPPER) label2 = id_to_upper_id(label2);
	if (side == LOWER) label2 = id_to_lower_id(label2);
      }
      if (label == label2) {
	/* ------------------------------- */
	/* arc is already in index[label]. */
	/* ------------------------------- */
	prior = arc;
	continue;
      }
      /* -------------------------------------- */
      /* Remove arc from its place in the list. */
      /* -------------------------------------- */
      next_arc(prior) = next_arc;
      /* ------------------------------------------------- */
      /* Add arc just after the first arc in index[label]. */
      /* ------------------------------------------------- */
      next_arc(arc) = next_arc(index[label]);
      next_arc(index[label]) = arc;
    } else {
      /* ------------------------ */
      /* Index arc and set prior. */
      /* ------------------------ */
      index[label] = arc;
      prior = arc;
    }
  }
}

void set_reclaimable(NETptr vnet, int reclaim_p)
{
  /* Marks all the existing operands of VNET reclaimable depending
     if reclaima_p is DO_RECLAIM. */

  VirtualInfo *vinfo;
  assert(vnet && NET_is_virtual(vnet));
 
  if (reclaim_p == DONT_RECLAIM)
    return;

  assert(reclaim_p == DO_RECLAIM);

  vinfo = NET_virtual_info(vnet);
  vinfo->op1_reclaimable = TRUE;
  /* Don't mark the second operand as reclaimable unless there is one. */
  if (vinfo->op2)
    vinfo->op2_reclaimable = TRUE;
}

int small_real_network_p(NETptr network, int max_states, int max_arcs) {
  /* RMK: True if network is a real network with no more than max_states
     and no more than max_arcs.  If one of the bounds is 0, that 
     parameter is not tested.  This can be used to decide whether 
     virtual operations should be realized on small real operands. */
  assert(0);
  return 0;
}

NETptr vlookup(char *string, NETptr fst, int no_print) {
  assert(0);
  return NULL;
}

NETptr vlookdown(char *string, NETptr fst, int no_print) {
  assert(0);
  return NULL;
}


/* ------------------------------------------------------------------------ */
/* Virtual Concatentation                                                   */
/* ------------------------------------------------------------------------ */


int vconcat_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  StatePair *state_pair = get_state_pair(vstate);
  /* ------------------------------------------------------ */
  /* A state is final if it is final in the second machine. */
  /* We are in the second machine if there is no state1.    */
  /* ------------------------------------------------------ */
  if (!state_pair->state1) { 
    return virtual_final_p(state_pair->state2);
  }
  if (NET_epsilon_free(vinfo->network) &&
      virtual_final_p(state_pair->state1) &&
      virtual_final_p(state_pair->state2)) {
    return 1;
  }
  return 0;
}

ARCptr vconcat_arc_set(STATEptr vstate, VirtualInfo *vinfo) {
  ARCptr arc, new_arc, arc_list;
  StatePair *state_pair, *dest_state_pair;
  STATEptr dest;
  arc_list = NULL;
  state_pair = get_state_pair(vstate);
  /* --------------------------------------------------------- */
  /* virtual_arc_set produces the arcs in the current machine. */
  /* We copy them up to the concat machine.                    */
  /* --------------------------------------------------------- */
  assert(vinfo->methods->data_type == STATE_PAIR);
  if (!state_pair->state1) {
    /* ------------------------------------ */
    /* Copy the arcs from the second state. */
    /* ------------------------------------ */
    for (arc = virtual_arc_set(state_pair->state2); arc; 
	 arc = next_arc(arc)) {
      if (dead_state_p(ARC_destination(arc))) continue;
      dest_state_pair = create_state_pair(NULL, ARC_destination(arc));
      new_arc = vmake_arc(ARC_label(arc), dest_state_pair, vinfo);
      if (!new_arc) continue;
      new_arc->next = arc_list;
      arc_list = new_arc;
    }
    STATE_deterministic(vstate) = STATE_deterministic(state_pair->state2);
    return arc_list;
  }
  /* ----------------------------------- */
  /* Copy the arcs from the first state. */
  /* ----------------------------------- */
  for (arc = virtual_arc_set(state_pair->state1); arc; 
       arc = next_arc(arc)) {
    dest_state_pair = create_state_pair(ARC_destination(arc), 
					state_pair->state2);
    new_arc = vmake_arc(ARC_label(arc), dest_state_pair, vinfo);
    if (!new_arc) continue;
    new_arc->next = arc_list;
    arc_list = new_arc;
  }
  STATE_deterministic(vstate) = STATE_deterministic(state_pair->state1);

  if (!virtual_final_p(state_pair->state1)) return arc_list;

  /* If state1 is a non-empty final state, appending state2 arcs
     may make it non-deterministic. */

  if (arc_list)
    STATE_deterministic(vstate) = FALSE;

  /* ----------------------------------------------------- */
  /* If state2 is a real state and if op2 is reclaimable   */
  /* we copy its arc list without creating a "useless"     */
  /* a virtual state. This means that the states           */
  /* of the operand network are accessed from the host     */
  /* net without being on its state list. This may be      */
  /* a dangerous move as the foreign states have to be     */
  /* collected from the operands in virtual_to_real_fsm(). */
  /* 2001-06-13 LK                                         */
  /* ----------------------------------------------------- */

  if (!STATE_is_virtual(state_pair->state2) && vinfo->op2_reclaimable)
    arc_list = append_arc_list(arc_list, copy_arc_set(state_pair->state2,
						      vinfo->network));
  else
    {
      /* -------------------------------------------------- */
      /* Concatenate the start state of the second machine. */
      /* veps_arc creates an EPSILON arc to dest unless the */
      /* epsilon_free bit is set, in which case it copies   */
      /* the arcs in dest.                                  */
      /* -------------------------------------------------- */
      dest_state_pair = create_state_pair(NULL, state_pair->state2);
      dest = vget_pair_state(dest_state_pair, 0, vinfo);
      arc_list = append_arc_list(arc_list, veps_arc(dest, vinfo));
    }
  return arc_list;
}

NETptr virtual_concat_fsm(NETptr op1, NETptr op2, int reclaim_p) {
  /* ----------------------------------------------------------------- */
  /* Derived from VIRTUALCONCATFSM.                                    */
  /* The state data of a virtual state contains the state in the first */
  /* machine plus the start state of the second machine. We can        */
  /* concatenate a machine with itself without pre-copying, because    */
  /* the remaining operands distinguish which occurrence we're in.     */
  /* If reclaim_p is DO_RECLAIM, the operands will be freed when the   */
  /* resulting virtual net is freed.                                   */
  /* ----------------------------------------------------------------- */
  NETptr new;
  VMethods *methods =  get_virtual_methods(Virtual_Concat);

  assert(op1 && op2);

  new = virtual_network(op1, op2, methods, NULL, NULL);
  /* RMK: If we left epsilon_free as 0, then veps_arcs would never copy 
     arcs.  This may be a bad idea if one machine has lots of final 
     states and the next one's startsate has lots of arcs. */
  STATE_deterministic(NET_start_state(new)) =
    STATE_deterministic(NET_start_state(op1));

  inherit_arity(new, op1, op2);
  inherit_closed_sigma(new, op1, op2);
  inherit_epsilon_free(new, op1, op2);
  inherit_pruned(new, op1, op2);

  set_reclaimable(new, reclaim_p);
  return new;
}

/* ------------------------------------------------------------------------ */
/* Virtual Intersection                                                     */
/* With contributions from Franck Guingne and Florent Nicart                */
/* ------------------------------------------------------------------------ */

ARCptr vintersect_arc_set(STATEptr vstate, VirtualInfo *vinfo) {
  StatePair *pair = get_state_pair(vstate);
  ARCptr *index;
  int index_size;
  id_type id;
  ARCptr arc_list1, arc_list2;
  ARCptr new_arc, new_arcs = NULL;
  STATEptr dest1, dest2, dest;
  id_type max_label = 0;
  /* ------------------------------ */
  /* Fetch the component arc lists. */
  /* ------------------------------ */
  arc_list1 = arc_list2 = NULL;
  if (pair->state1) arc_list1 = virtual_arc_set(pair->state1);
  if (pair->state2) arc_list2 = virtual_arc_set(pair->state2);
  if (!arc_list1 && !arc_list2) return NULL;
  /* ---------------------------------------------------- */
  /* Index the arcs by their labels.                      */
  /* Note: index_arcs_by_labels only rearranges the arcs. */
  /* ---------------------------------------------------- */
  fetch_max_label(&max_label, arc_list1);
  fetch_max_label(&max_label, arc_list2);
  index_size = sizeof(ARCptr) * (max_label + 1);
  index = alloca(index_size);
  memset(index, 0, index_size);
  index_arcs_by_label(arc_list2, index, BOTH_SIDES);

  while (arc_list1) {
    id=ARC_label(arc_list1);
    dest1 = ARC_destination(arc_list1);

    for (arc_list2=index[id]; arc_list2 && (ARC_label(arc_list2) == id);
	 arc_list2=next_arc(arc_list2)) {

      dest2 = ARC_destination(arc_list2);
      pair = create_state_pair(dest1, dest2);
      dest = vget_state(pair, vinfo);
      new_arc = make_arc(id, dest, vinfo->network);
      new_arc->next = new_arcs;
      new_arcs = new_arc;

    }
	
    /* go to the next arc in the first list : */
    arc_list1 = next_arc(arc_list1);
  }
  return new_arcs;
}

int vintersect_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  StatePair *pair = get_state_pair(vstate);
  assert(pair->state1 && pair->state2);
   
  return (virtual_final_p(pair->state1) && virtual_final_p(pair->state2));
}



NETptr virtual_intersect_fsm(NETptr op1, NETptr op2, int reclaim_p) {
  NETptr new;
  VMethods *methods =  get_virtual_methods(Virtual_Intersect);

  assert(op1 && op2);

  new = virtual_network(op1, op2, methods, NULL, NULL);

  NET_deterministic(new) = NET_deterministic(op1) && NET_deterministic(op2);

  STATE_deterministic(NET_start_state(new)) =
    STATE_deterministic(NET_start_state(op1)) && 
    STATE_deterministic(NET_start_state(op2));

  inherit_pruned(new, op1, op2);
  inherit_epsilon_free(new, op1, op2);
  inherit_closed_sigma(new, op1, op2);
  inherit_deterministic(new, op1, op2);
  set_reclaimable(new, reclaim_p);
  return new;
}


NETptr old_virtual_intersect_fsm(NETptr op1, NETptr op2, int reclaim_p) {
  /* ------------------------------------------------------------- */
  /* A brute force definition of intersection in terms of union    */
  /* and negation. Can be replaced by better_virtual_intersect_fsm */
  /* below when the arc set and finality functions above have      */
  /* been defined.                                                 */
  /* ------------------------------------------------------------- */
  
  /* Closing the sigma has the effect that negation is done
     with respect to the actual alphabet of the two operands
     without introducing a redundant OTHER. Closing the sigma
     does not affect the outcome. It just makes the operation
     a little less inefficient. */

  if (!NET_is_virtual(op1))
    close_sigma(op1, NET_sigma(op2), DONT_COPY, DONT_MINIMIZE);

  if (!NET_is_virtual(op2))
    close_sigma(op2, NET_sigma(op1), DONT_COPY, DONT_MINIMIZE);

  return 
    virtual_negate_fsm(virtual_union_fsm(virtual_negate_fsm(op1, reclaim_p),
					 virtual_negate_fsm(op2, reclaim_p),
					 DO_RECLAIM), DO_RECLAIM);
}



/* ------------------------------------------------------------------------ */
/* Virtual Minus                                                            */
/* With contributions from Franck Guingne and Florent Nicart                */
/* ------------------------------------------------------------------------ */

ARCptr vminus_arc_set(STATEptr vstate, VirtualInfo *vinfo) {
  StatePair *pair = get_state_pair(vstate);
  ARCptr *index;
  int index_size;
  int id;
  ARCptr arc_list1, arc_list2;
  ARCptr new_arc, new_arcs = NULL;
  STATEptr dest1, dest2, dest;
  id_type max_label = 0;
  /* ------------------------------ */
  /* Fetch the component arc lists. */
  /* ------------------------------ */
  arc_list1 = arc_list2 = NULL;

  if (!pair->state1) return NULL;

  arc_list1 = virtual_arc_set(pair->state1);
  
  if (pair->state2) {
    arc_list2 = virtual_arc_set(pair->state2);
    /* ---------------------------------------------------- */
    /* Index the arcs by their labels.                      */
    /* Note: index_arcs_by_labels only rearranges the arcs. */
    /* ---------------------------------------------------- */

    fetch_max_label(&max_label, arc_list1);
    fetch_max_label(&max_label, arc_list2);

    index_size = sizeof(ARCptr) * (max_label + 1);
    index = alloca(index_size);
    memset(index, 0, index_size);
    index_arcs_by_label(arc_list2, index, BOTH_SIDES);
    /* ------------------------- */
    /* Merge the lists together. */
    /* ------------------------- */
  
    while (arc_list1) {
      id=ARC_label(arc_list1);
      dest1 = ARC_destination(arc_list1);

      if (index[id]) {
	for (arc_list2=index[id]; arc_list2 && (ARC_label(arc_list2) == id);
	     arc_list2=next_arc(arc_list2)) {
	  dest2 = ARC_destination(arc_list2);
	  pair = create_state_pair(dest1, dest2);
	  dest = vget_state(pair, vinfo);
	  new_arc = make_arc(id, dest, vinfo->network);
	  new_arc->next = new_arcs;
	  new_arcs = new_arc;
	}
      }
      else {
	pair = create_state_pair(dest1, NULL);
	dest = vget_state(pair, vinfo);
	new_arc = make_arc(id, dest, vinfo->network);
	new_arc->next = new_arcs;
	new_arcs = new_arc;		
      }
	
      /* go to the next arc in the first list : */
      arc_list1 = next_arc(arc_list1);
    }

  } else {
    /* When there is no more states in the net to substract */
    while (arc_list1) {
      id=ARC_label(arc_list1);
      dest1 = ARC_destination(arc_list1);

      /* If the first operand is reclaimable, we can point
	 to the real destination state. */

      if (vinfo->op1_reclaimable)
	dest = dest1;
      else
	{
	  pair = create_state_pair(dest1, NULL);
	  dest = vget_state(pair, vinfo);
	}

      new_arc = make_arc(id, dest, vinfo->network);
      new_arc->next = new_arcs;
      new_arcs = new_arc;
			
      /* go to the next arc in the first list : */
      arc_list1 = next_arc(arc_list1);
    }
  
  }
  return new_arcs;
}

int vminus_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  StatePair *pair = get_state_pair(vstate);

  if (virtual_final_p(pair->state1))
    return(!(pair->state2 && virtual_final_p(pair->state2)));
  else
    return(FALSE);
}

NETptr old_virtual_minus_fsm(NETptr op1, NETptr op2, int reclaim_p) {
  /* ------------------------------------------------------------- */
  /* A brute force definition of MINUS in terms of negation and    */ 
  /* intersection. To be replaced by the better virtual_minus()    */
  /* below when the arc set and finality functions above have      */
  /* been defined.                                                 */
  /* ------------------------------------------------------------- */

  /* Closing the sigma has the effect that negation is done
     with respect to the actual alphabet of the two operands
     without introducing a superfluous OTHER. Closing the sigma
     does not affect the outcome. It just makes the operation a
     little less inefficient. */

  if (!NET_is_virtual(op1))
    close_sigma(op1, NET_sigma(op2), DONT_COPY, DONT_MINIMIZE);

  if (!NET_is_virtual(op2))
    close_sigma(op2, NET_sigma(op1), DONT_COPY, DONT_MINIMIZE);

  return virtual_intersect_fsm(op1, virtual_negate_fsm(op2, reclaim_p),
			       reclaim_p);
}

NETptr virtual_minus_fsm(NETptr op1, NETptr op2, int reclaim_p) {
  NETptr new;
  VMethods *methods =  get_virtual_methods(Virtual_Minus);

  assert(op1 && op2);

  if (!NET_deterministic(op2))
    op2 = virtual_determinize(op2, reclaim_p);

  new = virtual_network(op1, op2, methods, NULL, NULL);

  STATE_deterministic(NET_start_state(new)) =
    STATE_deterministic(NET_start_state(op1));

  inherit_epsilon_free(new, op1, op2);
  inherit_closed_sigma(new, op1, op2);
  inherit_deterministic(new, op1, NULL);

  set_reclaimable(new, reclaim_p);
  return new;
}

/* ------------------------------------------------------------------------ */
/* Virtual Option                                                           */
/* ------------------------------------------------------------------------ */

int voption_final_p(STATEptr vstate, VirtualInfo *vinfo)
{
  if (vstate == NET_start_state(vinfo->network))
    return TRUE;
  else
    return virtual_final_p(vstate);
}

ARCptr voption_arc_set(STATEptr vstate, VirtualInfo *vinfo)
{

  if (STATE_is_virtual(vstate))
    {
      if (vstate == NET_start_state(vinfo->network))
	return(copy_arc_set(get_data_state(vstate), vinfo->network));
      else
	return(virtual_arc_set(vstate));
    }

  return(STATE_arc_set(vstate));
}


NETptr virtual_option_fsm(NETptr op, int reclaim_p) {
  /* The original version of this function made a virtual copy
     of its operand, realized its start state and flipped the
     finality of the realized start state to TRUE. This trick
     is incompatible with the policy that virtual networks
     are saved without any realized states and arcs. Here
     we create virtual state with no arcs. The voption_arc_set
     function above copies the arcs of the start state
     of the operand network. If the operand is real rather
     than virtual, all of its arcs and states are shared
     with the virtual host net.
    */

  NETptr new;
  VMethods *methods = get_virtual_methods(Virtual_Option);

  if (!NET_is_virtual(op))
    {
      /* If the real start state is final, we return the
	 net or its copy depending on whether reclaim_p
	 is DO_RECLAIM (= don't copy) or DONT_RECLAIM (= do copy).
      */

      if (STATE_final(NET_start_state(op)))
	{
	  if (reclaim_p == DO_RECLAIM)
	    return(op);
	  else
	    return(virtual_copy_fsm(op, reclaim_p));
	}
    }

  new = virtual_network(op, NULL, methods, NULL, NULL);

  inherit_arity(new, op, NULL);
  inherit_epsilon_free(new, op, NULL);
  inherit_closed_sigma(new, op, NULL);
  inherit_deterministic(new, op, NULL);
  inherit_loop_free(new, op, NULL);
  inherit_pruned(new, op, NULL);

  set_reclaimable(new, reclaim_p);

  return new;
}

/* ------------------------------------------------------------------------ */
/* Virtual Negate                                                           */
/* ------------------------------------------------------------------------ */

int vnegate_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  STATEptr success = (STATEptr)vinfo->extra_info;
  if (vstate == success) return 1;
  /* ------------------------------------------- */
  /* Negate the final bit of the embedded state. */
  /* ------------------------------------------- */
  return !virtual_final_p(get_data_state(vstate));
}

static ARCptr completion_arcs(STATEptr state, VirtualInfo *vinfo)
{
  /* For each symbol for which there is no arc in the
     state, an arc is created pointing to the final
     sink state of the complement network.
  */

  NETptr vnetwork = vinfo->network;
  ALPHABETptr sigma = binary_to_label(NET_sigma(vnetwork));
  STATEptr success = (STATEptr)vinfo->extra_info;
  ARCptr arc, arc_set, varc, new_arc_set = NULL;
  int i;
  id_type sym;
  STATEptr data_state;

  /* ---------------------------------------- */
  /* Complete the arc set of state.           */
  /* An integer array would make this linear. */
  /* ---------------------------------------- */
  if (state == success) {
    /* --------------------------------------------------- */
    /* The success state doesn't have a data state, so we  */
    /* initialize things so that the success state will be */
    /* computed correctly.                                 */
    /* --------------------------------------------------- */
    data_state = state;
    arc_set = NULL;
  } else {
    data_state = get_data_state(state);
    arc_set = virtual_arc_set(data_state);
  }

  STATE_deterministic(state) = STATE_deterministic(data_state);

  for (i = 0; i < ALPH_len(sigma); i++) {
    sym = ALPH_item(sigma, i);
    varc = NULL;
    for (arc = arc_set; arc; arc = next_arc(arc)) {
      if (sym != ARC_label(arc)) continue;
      if (!dead_state_p(ARC_destination(arc))) {
	varc = vmake_arc(sym, ARC_destination(arc), vinfo);
      }
      break;
    }
    if (!varc) {
      varc = make_arc(sym, success, vnetwork);
    }
    varc->next = new_arc_set;
    new_arc_set = varc;
  }
  
  /* Restore sigma to standard form. */
  label_to_binary(sigma);

  return new_arc_set;
}


ARCptr vnegate_arc_set(STATEptr vstate, VirtualInfo *vinfo) {
  NETptr vnetwork = vinfo->network;
  STATEptr success = (STATEptr)vinfo->extra_info;

  if (!success) {
    /* ------------------------------------ */
    /* Create the image of a failure state. */
    /* The image is a final state.          */
    /* ------------------------------------ */
    success = make_state(NULL, vnetwork);
    STATE_final(success) = TRUE;
    vinfo->extra_info = success;
    STATE_arc_set(success) = completion_arcs(success, vinfo);
  }

  return(completion_arcs(vstate, vinfo));
}

NETptr virtual_negate_fsm(NETptr operand, int reclaim_p) {
  NETptr new;
  /* ----------------------------------------------------- */
  /* Derived from VIRTUALNEGATEFSM.                        */
  /* Creates a virtual complement of operand.              */
  /* This could probably benefit from on-the-fly pruning.  */
  /* The negation of a failure state is in extra_info.     */
  /* ----------------------------------------------------- */
  VMethods *methods = get_virtual_methods(Virtual_Negation);

  if (!NET_deterministic(operand)) {
    operand = virtual_determinize(operand, DO_RECLAIM);
  }
  if (!NET_closed_sigma(operand)) {
    sigma_add_to(NET_sigma(operand), OTHER);
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* Should the OTHER be propagated down with vcommon_sigma? */
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  }

  new = virtual_network(operand, NULL, methods, NULL, NULL);

  inherit_arity(new, operand, NULL);
  inherit_closed_sigma(new, operand, NULL);

  NET_deterministic(new) = TRUE;
  NET_epsilon_free(new) = TRUE;

  set_reclaimable(new, reclaim_p);
  return new;
}

/* ------------------------------------------------------------------------ */
/* Virtual Union                                                            */
/* ------------------------------------------------------------------------ */

int vunion_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  StatePair *pair = get_state_pair(vstate);
  if (pair->state1 && virtual_final_p(pair->state1)) return 1;
  if (pair->state2 && virtual_final_p(pair->state2)) return 1;
  return 0;
}

static void push_to_arc_list(ARCptr new_arc, ARCptr *arc_list)
{
  new_arc->next = *arc_list;
  *arc_list = new_arc;
}

ARCptr make_union_arc(id_type id, STATEptr dest, VirtualInfo *vinfo)
{
  /* The original version of this function created a virtual
     union state just in case the destination state was
     virtual. This was a bug. We need to always do this.
     Bug fixed with help from John Maxwell.  2002-07-02 LK
  */
  StatePair *pair;
  ARCptr new_arc;

  /*
  if (STATE_is_virtual(dest))
    {
      pair = create_state_pair(dest, NULL);
      dest = vget_state(pair, vinfo);
    }
  */

  pair = create_state_pair(dest, NULL);
  dest = vget_state(pair, vinfo);

  new_arc = make_arc(id, dest, vinfo->network);

  return(new_arc);
}

ARCptr vunion_arc_set(STATEptr vstate, VirtualInfo *vinfo) {
  StatePair *pair = get_state_pair(vstate);
  ARCptr *index1, *index2;
  int index_size, i;
  ARCptr arc_list1, arc_list2;
  ARCptr new_arc, new_arcs = NULL;
  STATEptr dest1, dest2;
  id_type max_label = 0;
  /* ------------------------------ */
  /* Fetch the component arc lists. */
  /* ------------------------------ */
  arc_list1 = arc_list2 = NULL;
  if (pair->state1) arc_list1 = virtual_arc_set(pair->state1);
  if (pair->state2) arc_list2 = virtual_arc_set(pair->state2);
  if (!arc_list1 && !arc_list2) return NULL;
  /* ---------------------------------------------------- */
  /* Index the arcs by their labels.                      */
  /* Note: index_arcs_by_labels only rearranges the arcs. */
  /* ---------------------------------------------------- */

  fetch_max_label(&max_label, arc_list1);
  fetch_max_label(&max_label, arc_list2);

  index_size = sizeof(ARCptr) * (max_label + 1);
  index1 = alloca(index_size);
  index2 = alloca(index_size);
  memset(index1, 0, index_size);
  memset(index2, 0, index_size);
  index_arcs_by_label(arc_list1, index1, BOTH_SIDES);
  index_arcs_by_label(arc_list2, index2, BOTH_SIDES);
  /* ------------------------- */
  /* Merge the lists together. */
  /* ------------------------- */
  for (i = max_label; i >= 0; i--) {
    arc_list1 = index1[i];
    arc_list2 = index2[i];
    while (arc_list1 || arc_list2) {
      dest1 = NULL;
      dest2 = NULL;
      if (arc_list1 && ARC_label(arc_list1) == i) {
	dest1 = ARC_destination(arc_list1);
	arc_list1 = next_arc(arc_list1);
      }
      if (arc_list2 && ARC_label(arc_list2) == i) {
	dest2 = ARC_destination(arc_list2);
	arc_list2 = next_arc(arc_list2);
      }
      if (!dest1 && !dest2) break;

      /* Two solutions here. The first one keeps the result
	 deterministic at the cost of always creating a new virtual
	 state. */

      if (dest1 && dest2)
      
	{
	  STATEptr dest;

	  pair = create_state_pair(dest1, dest2);
	  dest = vget_state(pair, vinfo);
	  new_arc = make_arc(i, dest, vinfo->network);
	}

      /* This second solution copies both arcs with their original
	 destinations. The result is not deterministic.
      */
      /*
      if (dest1 && dest2)
	{
	  new_arc = make_union_arc(i, dest1, vinfo);
	  push_to_arc_list(new_arc, &new_arcs);
	  new_arc = make_union_arc(i, dest2, vinfo);
	}
      */
      /* This is the common part. */

      else if (dest1)
	new_arc = make_union_arc(i, dest1, vinfo);
      else
	new_arc = make_union_arc(i, dest2, vinfo);

      push_to_arc_list(new_arc, &new_arcs);
    }
  }

  return new_arcs;
}


NETptr virtual_union_fsm(NETptr op1, NETptr op2, int reclaim_p) {
  /* ----------------------------------------------------------------- */
  /* Derived from VIRTUALUNIONFSM.                                     */
  /* If reclaim_p is DO_RECLAIM, the operands will be freed when the   */
  /* resulting virtual net is freed.                                   */
  /* ----------------------------------------------------------------- */
  NETptr new;
  VMethods *methods = get_virtual_methods(Virtual_Union);

  assert(op1 && op2);
  new = virtual_network(op1, op2, methods, NULL, NULL);

  inherit_arity(new, op1, op2);
  inherit_closed_sigma(new, op1, op2);
  inherit_epsilon_free(new, op1, op2);
  inherit_loop_free(new, op1, op2);
  inherit_pruned(new, op1, op2);

  set_reclaimable(new, reclaim_p);
  return new;
}

/* ------------------------------------------------------------------------ */
/* Virtual Kleene Star                                                      */
/* ------------------------------------------------------------------------ */

ARCptr vzero_plus_arc_set(STATEptr state, VirtualInfo *vinfo) {
  STATEptr start;
  STATEptr data_state = get_data_state(state);
  ARCptr copy = vcopy_arc_set(state, vinfo);
  ARCptr epsarc;
  if (!virtual_final_p(data_state)) return copy;
  /* ------------------------------------------------------------------ */
  /* If the state is final, add an epsilon arc back to the start state. */
  /* If there is no start state, then we are constructing it, and we    */
  /* don't need an epsilon arc to itself.                               */
  /* ------------------------------------------------------------------ */
  start = NET_start_state(vinfo->network);
  assert(start);
  if (state == start) return copy;
#if 0
  NET_epsilon_free(vinfo->network) = FALSE;
  epsarc = make_arc(EPSILON, start, vinfo->network);
#else
  epsarc = veps_arc(start, vinfo);
#endif
  return append_arc_list(copy, epsarc);
}

int data_state_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  STATEptr data_state = get_data_state(vstate);
  return virtual_final_p(data_state);
}

int vcopy_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  STATEptr data_state = get_data_state(vstate);
  return virtual_final_p(data_state);
}

int zero_plus_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  STATEptr data_state = get_data_state(vstate);
  /* ------------------------------------------------------- */
  /* The start state of a zero plus machine is always final. */
  /* If we are using epsilons to transition back to the      */
  /* start state, then nothing else if final.  Otherwise,    */
  /* we use the final bit of the underlying machine.         */
  /* ------------------------------------------------------- */
  if (vstate == NET_start_state(vinfo->network)) return TRUE;
  if (!NET_epsilon_free(vinfo->network)) return FALSE;
  return virtual_final_p(data_state);
}

NETptr virtual_zero_plus(NETptr op, int reclaim_p) {
  /* ----------------------------------------------------------------- */
  /* Derived from VIRTUALZEROPLUS.                                     */
  /* If reclaim_p is DO_RECLAIM, the operands will be freed when the   */
  /* resulting virtual net is freed.                                   */
  /* ----------------------------------------------------------------- */
  /* RMK: Creates a virtual machine for the Kleene * of the virtual
     operand.  The virtual start state is final as are the states 
     corresponding to final operand states. veps_arc produces either 
     an epsilon arc back to the startstate, or a copy of the start 
     states arcset, depending on whether we've decided to preserve 
     epsilon-freeness.  Setting the epsilon_free flag is heuristic to 
     preserve epsilon freeness in the result.  May be a bad idea if 
     the startstate has lots of arcs.
 */
  NETptr new;
  VMethods *methods = get_virtual_methods(Virtual_Zero_Plus);

  new = virtual_network(op, NULL, methods, NULL, NULL);

  inherit_epsilon_free(new, op, NULL);
  inherit_pruned(new, op, NULL);
  inherit_deterministic(new, op, NULL);
  inherit_closed_sigma(new, op, NULL);
  inherit_arity(new, op, NULL);

  set_reclaimable(new, reclaim_p);
  return new;
}

NETptr virtual_one_plus(NETptr op, int reclaim_p) {
  /* Make sure the operand is reclaimed only once. */
  return virtual_concat_fsm(op, virtual_zero_plus(op, DONT_RECLAIM),
			    reclaim_p);
}

/* ------------------------------------------------------------------------ */
/* Virtual Repeat                                                           */
/* ------------------------------------------------------------------------ */

NETptr virtual_rpt_fsm(NETptr operand, int min, int max, int reclaim_p) {
  /* ------------------------------------------------------------ */
  /* Derived from VIRTUALRPTFSM.                                  */
  /* ------------------------------------------------------------ */
  assert(0);
  return NULL;
}

/* ------------------------------------------------------------------------ */
/* Virtual Shuffle                                                          */
/* ------------------------------------------------------------------------ */

NETptr virtual_shuffle_fsm(NETptr op1, NETptr op2, int reclaim_p) {
  /* ------------------------------------------------------------ */
  /* Derived from VIRTUALSHUFFLEFSM.                              */
  /* ------------------------------------------------------------ */
  assert(0);
  return NULL;
}

/* ------------------------------------------------------------------------ */
/* Virtual Ignore                                                           */
/* ------------------------------------------------------------------------ */

NETptr virtual_ignore_fsm(NETptr target, NETptr fluff, int reclaim_p) {
  /* ------------------------------------------------------------ */
  /* Derived from VIRTUALIGNOREFSM.                               */
  /* ------------------------------------------------------------ */
  assert(0);
  return NULL;
}

/* ------------------------------------------------------------------------ */
/* Virtual Sigma But                                                        */
/* ------------------------------------------------------------------------ */

NETptr virtual_sigma_but_fsm(NETptr operand, int reclaim_p) {
  /* ------------------------------------------------------------ */
  /* Derived from VIRTUALSIGMABUTFSM.                             */
  /* ------------------------------------------------------------ */
  assert(0);
  return NULL;
}


/* ------------------------------------------------------------------------ */
/* Virtual Determinize                                                      */
/* ------------------------------------------------------------------------ */


int eps_closure_final_p(STATEptr state) {
  ARCptr arc;
  STATEptr dest;
  int final;
  if (virtual_final_p(state)) return 1;
  for (arc = virtual_arc_set(state); arc; 
       arc = next_arc(arc)) {
    if (ARC_label(arc) != EPSILON) continue;
    dest = ARC_destination(arc);
    if (STATE_visit_mark(dest)) continue;
    STATE_visit_mark(dest) = 1;
    final = eps_closure_final_p(dest);
    STATE_visit_mark(dest) = 0;
    if (final) return 1;
  }
  return 0;
}

int veps_remove_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  /* ----------------------------------------------------- */
  /* If one of the states in the epsilon closure is final, */
  /* then the epsilon-removed state is final.  This is     */
  /* used by virtual_eps_remove_fsm and                    */
  /* virtual_determinize.  Exploring the epsilon closure   */
  /* may cause a lot of arcs to be created.                */
  /* ----------------------------------------------------- */
  CONSptr tail, states;
  STATEptr state;
  states = get_state_list(vstate);
  for (tail = states; tail; tail = cdr(tail)) {
    state = (STATEptr)car(tail);
    if (eps_closure_final_p(state)) return 1;
  }
  return 0;
}

int vdeterminize_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  /* ----------------------------------------------------------- */
  /* If any of the states are final, then the determinized state */
  /* is final.  This procedure is only called if the network     */
  /* is known to be epsilon-free.                                */
  /* ----------------------------------------------------------- */
  CONSptr tail, states;
  STATEptr state;
  NETptr net = vinfo->op1;
  states = get_state_list(vstate);

  if (NET_epsilon_free(net))
    {
      for (tail = states; tail; tail = cdr(tail)) {
	state = (STATEptr)car(tail);
	if (virtual_final_p(state)) return 1;
      }
      return 0;
    }
  else
    return(veps_remove_final_p(vstate, vinfo));
}

int list_member(void *item, CONSptr list) {
  CONSptr tail;
  for (tail = list; tail; tail = cdr(tail)) {
    if (car(tail) == item) return 1;
  }
  return 0;
}

void vdeterminize_add_arcs(STATEptr state, ARCptr *arcs, NETptr net, int len) {
  /* ----------------------------------------------------- */
  /* Add the arcs in the epsilon closure of state to arcs. */
  /* ----------------------------------------------------- */
  ARCptr arc;
  CONSptr dest_states, cons;
  for (arc = virtual_arc_set(state); arc; 
       arc = next_arc(arc)) {
    id_type label = ARC_label(arc);
    STATEptr dest = ARC_destination(arc);
    assert(label < len);
    if (label == EPSILON) {
      /* ---------------------------------------------- */
      /* We use the visit mark to avoid infinite loops. */
      /* ---------------------------------------------- */
      if (STATE_visit_mark(dest)) continue;
      STATE_visit_mark(dest) = 1;
      vdeterminize_add_arcs(dest, arcs, net, len);
      STATE_visit_mark(dest) = 0;
      continue;
    }
    if (!arcs[label]) {
      arcs[label] = make_arc(label, NULL, net);
    }
    dest_states = (CONSptr)ARC_destination(arcs[label]);
    /* ------------------------------------------- */
    /* Avoid duplicating states in the state list. */
    /* ------------------------------------------- */
    if (list_member(dest, dest_states)) continue;
    cons = (CONSptr)calloc(sizeof(CONStype), 1);
    car(cons) = dest;
    cdr(cons) = dest_states;
    ARC_destination(arcs[label]) = (STATEptr)cons;
  }
}
 

ARCptr vdeterminize_arc_set(STATEptr vstate, VirtualInfo *vinfo) {
  /* RMK: arc_set function for determinize.    This is the simple 
     quadratic algorithm.  We could use the linear version of the 
     standard determinizer if the arcs throughout a virtual machine 
     are first recoded as integers, which could be done by 
     vcommon_sigma.  Then it might be useful to allocate a single 
     array and cache it in the extra_info. */
  CONSptr tail, dest_states, states;
  STATEptr state;
  ARCptr *arcs;
  ARCptr arc_list;
  int i, len;
  /* -------------------------------------------------------- */
  /* Merge the arcs from all of the states into an arc array. */
  /* -------------------------------------------------------- */
  states = get_state_list(vstate);
  len = LIM_id_max(LABEL_MAP) + 1;
  arcs = alloca(sizeof(ARCptr) * len);
  for (i = 0; i < len; i++) {
    arcs[i] = NULL;
  }
  for (tail = states; tail; tail = cdr(tail)) {
    state = (STATEptr)car(tail);
    vdeterminize_add_arcs(state, arcs, vinfo->network, len);
  }
  /* --------------------------------------- */
  /* Extract an arc list from the arc array. */
  /* --------------------------------------- */
  arc_list = NULL;
  for (i = 0; i < len; i++) {
    if (!arcs[i]) continue;
    dest_states = (CONSptr)ARC_destination(arcs[i]);
    ARC_destination(arcs[i]) = vget_list_state(dest_states, vinfo);
    arcs[i]->next = arc_list;
    arc_list = arcs[i];
  }
  STATE_deterministic(vstate) = TRUE;
  return arc_list; 
}

NETptr virtual_determinize(NETptr operand, int reclaim_p) {
  /* ------------------------------------------------------------ */
  /* Derived from VIRTUALDETERMINIZE.                             */
  /* ------------------------------------------------------------ */
  NETptr new;
  VMethods *methods = get_virtual_methods(Virtual_Determinize);

  if (NET_deterministic(operand)) return operand;
  new = virtual_network(operand, NULL, methods, NULL, NULL);

  inherit_closed_sigma(new, operand, NULL);
  inherit_arity(new, operand, NULL);
  inherit_loop_free(new, operand, NULL);
  NET_deterministic(new) = TRUE;
  NET_epsilon_free(new) = TRUE;

  set_reclaimable(new, reclaim_p);
  return new;
}

/* ----------------------------------------------------------------------- */
/* Virtual Epsilon Removal                                                 */
/* ----------------------------------------------------------------------- */

NETptr virtual_eps_remove(NETptr operand, int reclaim_p) {
  /* --------------------------------------------------------------- */
  /* Derived from VIRTUALEPSREMOVE.                                  */
  /* --------------------------------------------------------------- */
  assert(0);
  return NULL;
}

/* ------------------------------------------------------------------------ */
/* Virtual Copy                                                             */
/* ------------------------------------------------------------------------ */


int state_has_arc(STATEptr state, id_type label, STATEptr dest) {
  ARCptr arc;
  for (arc = virtual_arc_set(state); arc; arc = next_arc(arc)) {
    if (ARC_label(arc) != label) continue;
    if (ARC_destination(arc) != dest) continue;
    return 1;
  }
  return 0;
}

NETptr virtual_copy_fsm(NETptr operand, int reclaim_p) {
  /* --------------------------------------------------------------- */
  /* Derived from VIRTUALCOPYFSM.                                    */
  /* --------------------------------------------------------------- */
  NETptr new, op1, op2;
  VMethods *methods = NULL;

  assert(operand);

  if (0 && NET_is_virtual(operand)) {
    /* Copy methods and operands from the original into a
       reclaimable method structure.
    */

    VirtualInfo *old_vinfo = NET_virtual_info(operand);
    VMethods *old_methods = old_vinfo->methods;

    op1 = old_vinfo->op1;
    op2 = old_vinfo->op2;

    methods = (VMethods *)calloc(sizeof(VMethods), 1);
    memcpy((void*) methods, (void*) old_methods, sizeof(VMethods));
    methods->reclaimable = TRUE;
  }
  else {
    methods = get_virtual_methods(Virtual_Copy);
    op1 = operand;
    op2 = NULL;
  }
  
  new = virtual_network(op1, op2, methods, NULL, NULL);

  inherit_arity(new, operand, NULL);
  inherit_pruned(new, operand, NULL);
  inherit_deterministic(new, operand, NULL);
  inherit_minimized(new, operand);
  inherit_epsilon_free(new, operand, NULL);
  inherit_closed_sigma(new, operand, NULL);

  set_reclaimable(new, reclaim_p);

  return new;
}

/* ------------------------------------------------------------------------ */
/* Virtual Invert FST                                                       */
/* ------------------------------------------------------------------------ */

ARCptr vinvert_arc_set(STATEptr vstate, VirtualInfo *vinfo) {
  assert(0);
  return NULL;
}

int vinvert_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  assert(0);
  return 0;
}


NETptr virtual_invert_fst(NETptr operand, int reclaim_p) {
  /* --------------------------------------------------------------- */
  /* Derived from VIRTUALINVERTFST.                                  */
  /* --------------------------------------------------------------- */
  assert(0);
  return NULL;
}

/* ------------------------------------------------------------------------ */
/* Virtual Lower of FST                                                     */
/* ------------------------------------------------------------------------ */

ARCptr vlower_arc_set(STATEptr vstate, VirtualInfo *vinfo) {
  ARCptr arc, new_arc, new_arc_set = NULL;
  id_type label;
  for (arc = virtual_arc_set(get_data_state(vstate)); 
       arc; arc = next_arc(arc)) {
    if (dead_state_p(ARC_destination(arc))) continue;
    label = ARC_label(arc);
    if (tuple_id_p(label)) {
      label = id_to_lower_id(label);
    }
    assert(vinfo->methods->data_type == DATA_STATE);
    new_arc = vmake_arc(label, ARC_destination(arc), vinfo);
    if (!new_arc) continue;
    new_arc->next = new_arc_set;
    new_arc_set = new_arc;
  }
  return new_arc_set;
}

NETptr virtual_lower_fsm(NETptr operand, int reclaim_p) {
  /* --------------------------------------------------------- */
  /* Derived from VIRTUALLOWERFSM.                             */
  /* --------------------------------------------------------- */
  NETptr new;
  VMethods *methods = get_virtual_methods(Virtual_Lower);
  new = virtual_network(operand, NULL, methods, NULL, NULL);

  inherit_pruned(new, operand, NULL);
  inherit_closed_sigma(new, operand, NULL);
  inherit_loop_free(new, operand, NULL);

  set_reclaimable(new, reclaim_p);
  return new;
}

/* ------------------------------------------------------------------------ */
/* Virtual Upper of FST                                                     */
/* ------------------------------------------------------------------------ */


ARCptr vupper_arc_set(STATEptr vstate, VirtualInfo *vinfo) {
  ARCptr arc, new_arc, new_arc_set = NULL;
  id_type label;
  for (arc = virtual_arc_set(get_data_state(vstate)); 
       arc; arc = next_arc(arc)) {
    if (dead_state_p(ARC_destination(arc))) continue;
    label = ARC_label(arc);
    if (tuple_id_p(label)) {
      label = id_to_upper_id(label);
    }
    assert(vinfo->methods->data_type == DATA_STATE);
    new_arc = vmake_arc(label, ARC_destination(arc), vinfo);
    if (!new_arc) continue;
    new_arc->next = new_arc_set;
    new_arc_set = new_arc;
  }
  return new_arc_set;
}

NETptr virtual_upper_fsm(NETptr operand, int reclaim_p) {
  /* --------------------------------------------------------- */
  /* Derived from VIRTUALUPPERFSM.                             */
  /* --------------------------------------------------------- */
  NETptr new;
  VMethods *methods = get_virtual_methods(Virtual_Upper);
  new = virtual_network(operand, NULL, methods, NULL, NULL);
  NET_pruned(new) = NET_pruned(operand);
  set_reclaimable(new, reclaim_p);
  return new;
}

/* ------------------------------------------------------------------------ */
/* Virtual Crossproduct FST                                                 */
/* ------------------------------------------------------------------------ */

ARCptr vcrossproduct_arc_set(STATEptr vstate, VirtualInfo *vinfo) {
  assert(0);
  return NULL;
}

int vcrossproduct_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  assert(0);
  return 0;
}


NETptr virtual_crossproduct_fst(NETptr operand, int reclaim_p) {
  /* ----------------------------------------------------------- */
  /* Derived from VIRTUALMAPPINGFST.                             */
  /* ----------------------------------------------------------- */
  assert(0);
  return NULL;
}

/* ------------------------------------------------------------------------ */
/* Virtual Priority Union (FST)                                             */
/* ------------------------------------------------------------------------ */

ARCptr vpriority_union_arc_set(STATEptr vstate, VirtualInfo *vinfo) {
  assert(0);
  return NULL;
}

int vpriority_union_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  assert(0);
  return 0;
}


NETptr virtual_priority_union(NETptr op1, NETptr op2,int side,int reclaim_p) {
  /* --------------------------------------------------------- */
  /* Derived from VIRTUALPRIORITYUNIONFST.                     */
  /* --------------------------------------------------------- */

  /* ------------------------------------------------------------- */
  /* A brute force definition of PRIORITY_UNION. To be replaced by */
  /* a better version when when the arc set and finality functions */
  /* Lower priority union of A and B is defined as the union of A  */
  /* With the composition of the negation of the lower side of A   */
  /* with B. In other words it contains all the mappings from A    */
  /* and all the mappings from B for lower-side words not found in */
  /* in A.                                                         */
  /* ------------------------------------------------------------- */

  /* Internally constructed virtual nets are always reclaimable
     but the resulting virtual network is reclaimable only if
     reclaim_p == DO_RECLAIM. Note that we need to be careful
     here not to reclaim OP1 twice. It is essential that
     virtual_upper_fsm() and virtual_lower_fsm() declare that
     OP1 is not reclaimable because OP1 is also used as the
     operand of the virtual union later on.
  */

  NETptr net;

  if (side == UPPER) {
    NETptr negated_upper1 =
      virtual_negate_fsm(virtual_upper_fsm(op1, DONT_RECLAIM), DO_RECLAIM);
    op2 = virtual_compose_fst(negated_upper1, op2, DO_RECLAIM);
  } else {
    NETptr negated_lower1 =
      virtual_negate_fsm(virtual_lower_fsm(op1, DONT_RECLAIM), DO_RECLAIM);
    assert(side == LOWER);
    op2 = virtual_compose_fst(op2, negated_lower1, DO_RECLAIM);
  }
  net = virtual_union_fsm(op1, op2, reclaim_p);
  set_reclaimable(net, reclaim_p);
  return net;
}

/* ------------------------------------------------------------------------ */
/* Virtual Compose FST                                                      */
/* ------------------------------------------------------------------------ */


#define SPREAD_LAB(arc, up_var, low_var, both_var) {   \
  both_var = ARC_label(arc);                 \
  if (tuple_id_p(both_var)) {                \
    low_var = id_to_lower_id(both_var);      \
    up_var = id_to_upper_id(both_var);       \
  } else {                                   \
    up_var = low_var = both_var;             \
  }                                          \
}

ARCptr vcompose_arc(id_type up_lab, id_type low_lab, 
		    STATEptr up_dest, STATEptr low_dest,
		    int ignore_upper_eps,
		    ARCptr next, VirtualInfo *vinfo,
		    ARCptr *index, int index_size) {
  StateData *state_data = create_state_pair(up_dest, low_dest);
  STATEptr dest = vget_pair_state(state_data, ignore_upper_eps, vinfo);
  id_type label;
  ARCptr arc, temp;
  assert(vinfo->methods->data_type == STATE_PAIR);
  make_fstlabel(&label, up_lab, low_lab);
  if (NET_labels(vinfo->network)) {
    sigma_add_to(NET_labels(vinfo->network), label);
  }
  if (label >= index_size) {
    /* ------------------------------ */
    /* We've created too many labels. */
    /* Don't eliminate duplicates.    */
    /* ------------------------------ */
    arc = make_arc(label, dest, vinfo->network);
    arc->next = next;
    return arc;
  }
  /* ------------------------------ */
  /* See if the arc already exists. */
  /* ------------------------------ */
  assert(label < index_size);
  for (temp = index[label]; temp; temp = temp->next) {
    if (ARC_label(temp) != label) break;
    if (ARC_destination(temp) != dest) continue;
    return next;
  }
  arc = make_arc(label, dest, vinfo->network);
  if (index[label]) {
    /* ------------------------------- */
    /* Add the arc after index[label]. */
    /* ------------------------------- */
    next_arc(arc) = next_arc(index[label]);
    next_arc(index[label]) = arc;
    return next;
  }
  /* ------------------------------------- */
  /* Put the arc at the front of the list. */
  /* ------------------------------------- */
  arc->next = next;
  index[label] = arc;
  return arc;
}

ARCptr vcompose_arc_set(STATEptr state, VirtualInfo *vinfo) {
  STATEptr up_state, low_state;
  ARCptr up_arcs, low_arcs;
  ARCptr up_arc, low_arc;
  ARCptr new_arcs = NULL;
  ARCptr *index, *index2;
  int index_size, index2_size;
  id_type uofl, lofl, llab;
  id_type uofu, lofu, ulab;
  int up_has_non_eps = 0;
  int ignore_upper_eps = state->unused;
  StatePair *state_pair = get_state_pair(state);
  STATEptr final = (STATEptr)vinfo->extra_info;
  up_state = state_pair->state1;
  low_state = state_pair->state2;
  low_arcs = virtual_arc_set(low_state);
  if (!low_arcs && !virtual_final_p(low_state)) {
    /* ------------------------------------------------------ */
    /* Nothing to do if low_state is a failure state, perhaps */
    /* not pruned from a lower operand.  Don't even have to   */
    /* create the upper arcs.                                 */
    /* ------------------------------------------------------ */
    return NULL;
  }
  /* ------------------------------------------ */
  /* Index the lower arcs for efficient access. */
  /* NB: This may reorder the arcs somewhat.    */
  /* ------------------------------------------ */
  assert(!NET_shared_arc_lists(vinfo->op2));
  index_size = sizeof(ARCptr) * (LIM_id_count(LABEL_MAP) + 1); 
  index = alloca(index_size);
  memset(index, 0, index_size);
  index_arcs_by_label(low_arcs, index, UPPER);
  /* ------------------------------------------------------------- */
  /* vcompose_arc indexes the output arcs to check for duplicates. */
  /* ------------------------------------------------------------- */
  index2_size = 2 * index_size;
  index2 = alloca(index2_size);
  memset(index2, 0, index2_size);
  index2_size = index2_size/sizeof(ARCptr);
  up_arcs = virtual_arc_set(up_state);
  /* --------------------------------------------------------------- */
  /* First deal with all the upper arcs, traversing upper A:eps      */
  /* without regard to lower arcs.  In the same loop, also determine */
  /* whether up_state has any A:B transitions, for use below.        */
  /* --------------------------------------------------------------- */
  for (up_arc = up_arcs; up_arc; up_arc = next_arc(up_arc)) {
    SPREAD_LAB(up_arc, uofu, lofu, ulab);
    if (lofu == EPSILON) {
      if (!ignore_upper_eps) {
	new_arcs = vcompose_arc(uofu, EPSILON, 
				ARC_destination(up_arc), low_state, 0,
				new_arcs, vinfo, index2, index2_size);
      }
    } else {
      up_has_non_eps = 1;
      /* ----------------------------------------------------------- */
      /* Upper A:X.  Don't build A:B if A:eps and eps:B are being    */ 
      /* built. That would be a different strategy, where we might   */
      /* have both A:B, A:eps and eps:B outgoing transitions instead */
      /* of just the sequence A:eps eps:B and epsB going to a state  */
      /* where upper epsilons are ignored.                           */
      /* ----------------------------------------------------------- */
      for (low_arc = index[lofu]; low_arc; 
	   low_arc = next_arc(low_arc)) {
	SPREAD_LAB(low_arc, uofl, lofl, llab);
	if (lofu != uofl) break;
	/* ----------------------------------------------------------- */
	/* Non-epsilons match. (A:eps and eps:B are not encountered    */
	/* here).  However, we have to worry about OTHER labels.  We   */
	/* have to worry about the difference between OTHER and OTHER: */
	/* OTHER, to decide when we should output which one.  (Note:   */
	/* OTHER in both machines stands for mutually unknown symbols, */
	/* because the alphabets have been synchronized.               */
	/* ----------------------------------------------------------- */
	if ((ulab == OTHER && lofl == OTHER) ||
	    (llab == OTHER && uofu == OTHER)) {
	  /* -------------------------------------------------------- */
	  /* Upper or lower is OTHER, so inner match is OTHER.  If    */
	  /* opposite outer is OTHER, we have either OTHER and OTHER, */
	  /* OTHER and OTHER:OTHER, or OTHER:OTHER and OTHER.  In all */
	  /* such cases we need a matching OTHER.  We don't use       */
	  /* vcompose_arc because OTHER:OTHER means something         */
	  /* different from just plain OTHER.                         */
	  /* -------------------------------------------------------- */
	  ARCptr arc;
	  STATEptr dest;
	  StateData *state_data =
	    create_state_pair(ARC_destination(up_arc),
			      ARC_destination(low_arc));
	  dest = vget_pair_state(state_data, 0, vinfo);
	  arc = make_arc(OTHER, dest, vinfo->network);
	  arc->next = new_arcs;
	  new_arcs = arc;
	}
	if (!(ulab == OTHER && llab == OTHER)) {
	  /* ------------------------------ */
	  /* Build A:B but not OTHER:OTHER. */
	  /* ------------------------------ */
	  new_arcs = vcompose_arc(uofu, lofl,
				  ARC_destination(up_arc),
				  ARC_destination(low_arc), 0,
				  new_arcs, vinfo, index2, index2_size);
	}
      }
    }
  }
  if (up_has_non_eps) {
    /* ------------------------------------------------------------------ */
    /* If up_state had only A:eps transitions, then they were traversed   */
    /* above to reach low_state again.  So we would have nothing more     */
    /* to do here.  So we need only worry about the future of low_state's */
    /* eps:B transitions if there are some non_eps arcs that we could     */
    /* reach at up_state if we were to traverse out eps:B.  We make our   */
    /* eps:B transition go to a state with a pseudo upper that is free    */
    /* of A:eps transitions.                                              */
    /* ------------------------------------------------------------------ */
    for (low_arc = index[EPSILON]; low_arc; 
	 low_arc = next_arc(low_arc)) {
      SPREAD_LAB(low_arc, uofl, lofl, llab);
      if (uofl != EPSILON) break;
      /* -------------------------------------------------------------- */
      /* We want to ignore the upper epsilons in the constructed state, */
      /* so we pass ignore_upper_eps as 1.                              */
      /* -------------------------------------------------------------- */
      new_arcs = vcompose_arc(EPSILON, lofl,
			      up_state, ARC_destination(low_arc), 1,
			      new_arcs, vinfo, index2, index2_size);
    }    
  } else if (virtual_final_p(up_state)) {
    for (low_arc = index[EPSILON]; low_arc; 
	 low_arc = next_arc(low_arc)) {
      SPREAD_LAB(low_arc, uofl, lofl, llab);
      if (uofl != EPSILON) break;
      if (!final)
	{
	  final = make_state(NULL, vinfo->network);
	  STATE_final(final) = TRUE;
	  vinfo->extra_info = (void *) final;
	}
      new_arcs = vcompose_arc(EPSILON, lofl, final, ARC_destination(low_arc),
			      0, new_arcs, vinfo, index2, index2_size);
    }
  }
  update_match_tables();

  return new_arcs;
}

NETptr virtual_compose_fst(NETptr op1, NETptr op2, int reclaim_p) {
  /* ----------------------------------------------------- */
  /* Derived from VIRTUALCOMPOSEFST.                       */
  /* ----------------------------------------------------- */
  NETptr new;
  VMethods *methods = get_virtual_methods(Virtual_Compose);

  new = virtual_network(op1, op2, methods, NULL, NULL);

  set_reclaimable(new, reclaim_p);
  return new;
}

/* ------------------------------------------------------------------------ */
/* Virtual Uncompact                                                        */
/* ------------------------------------------------------------------------ */

NETptr virtual_uncompact_fsm(NETptr operand, int reclaim_p) {
  /* ----------------------------------------------------- */
  /* Derived from VIRTUALUNCOMPACTFSM.                     */
  /* ----------------------------------------------------- */
  assert(0);
  return NULL;
}

/* ------------------------------------------------------------------------ */
/* Virtual Prune                                                            */
/* ------------------------------------------------------------------------ */

void virtual_prune_fsm(NETptr operand, int reclaim_p) {
  /* ----------------------------------------------------- */
  /* Derived from VIRTUALPRUNEFSM.                         */
  /* ----------------------------------------------------- */
  assert(0);
}

/* ------------------------------------------------------------------------ */
/* Virtual Tags Simulator                                                   */
/* ------------------------------------------------------------------------ */

int vtags_final_p(STATEptr vstate, VirtualInfo *vinfo) {
  STATEptr end_state = vinfo->extra_info;
  /* ---------------------------- */
  /* Only the end state is final. */
  /* ---------------------------- */
  return vstate == end_state;
}

ARCptr vtags_arc_set(STATEptr state, VirtualInfo *vinfo) {
  NETptr vnetwork = vinfo->network;
  ALPHABETptr sigma;
  ARCptr arc, arcs;
  id_type id;
  int i, sigma_len;
  STATEptr end_state;
  if (state != NET_start_state(vnetwork)) return NULL;
  /* --------------------------------------------------------- */
  /* The start state has an arc to the end state for each tag. */
  /* This state is recomputed whenever the sigma is updated    */
  /* because of the OTHER.                                     */
  /* --------------------------------------------------------- */
  end_state = (STATEptr)vinfo->extra_info;
  arcs = make_arc(OTHER, end_state, vnetwork);
  sigma = NET_sigma(vnetwork);
  if (!sigma) return arcs;
  binary_to_label(sigma);
  sigma_len = ALPH_len(sigma);
  for (i = 0; i < sigma_len; i++) {
    id = ALPH_item(sigma, i);
    if (!id_tag_p(id)) continue;
    if (id == EPSILON) continue;
    if (id == OTHER) continue;
    arc = make_arc(id, end_state, vnetwork);
    arc->next = arcs;
    arcs = arc;
  }
  /* Restore sigma to standard format. */
  label_to_binary(sigma);
  return arcs;
}

NETptr virtual_tags_fsm() {
  /* --------------------------------------------------- */
  /* This produces a machine with a list of current tags */
  /* (i.e. multi-character symbols).  It gets updated    */
  /* automatically whenever the sigma changes.           */
  /* --------------------------------------------------- */
  STATEptr end_state;
  NETptr vnetwork;
  VirtualInfo *vinfo;
  VMethods *methods = get_virtual_methods(Virtual_Tags);
  vnetwork = virtual_network(NULL, NULL, methods, NULL, NULL);
  NET_epsilon_free(vnetwork) = TRUE;
  sigma_add_to(NET_sigma(vnetwork), OTHER);
  vinfo = NET_virtual_info(vnetwork);
  end_state = make_state(NULL, vnetwork);
  STATE_deterministic(end_state) = FALSE;
  STATE_is_virtual(end_state) = TRUE;
  STATE_virtual_info(end_state) = vinfo;
  vinfo->extra_info = end_state;
  return vnetwork;
}

ARCptr vnon_tags_arc_set(STATEptr state, VirtualInfo *vinfo) {
  NETptr vnetwork = vinfo->network;
  ALPHABETptr sigma;
  ARCptr arc, arcs;
  id_type id, LB;
  int i, sigma_len;
  STATEptr end_state;
  if (state != NET_start_state(vnetwork)) return NULL;
  /* --------------------------------------------------------- */
  /* The start state has an arc to the end state for each tag. */
  /* This state is recomputed whenever the sigma is updated    */
  /* because of the OTHER.                                     */
  /* --------------------------------------------------------- */
  end_state = (STATEptr)vinfo->extra_info;
  arcs = make_arc(OTHER, end_state, vnetwork);
  sigma = NET_sigma(vnetwork);
  if (!sigma) return arcs;
  binary_to_label(sigma);
  sigma_len = ALPH_len(sigma);
  intern_literal(&LB, "@");
  for (i = 0; i < sigma_len; i++) {
    id = ALPH_item(sigma, i);
    if (id_tag_p(id)) continue;
    if (id == EPSILON) continue;
    if (id == OTHER) continue;
    if (id == LB) continue;
    arc = make_arc(id, end_state, vnetwork);
    arc->next = arcs;
    arcs = arc;
  }
  /* Restore sigma to standard format. */
  label_to_binary(sigma);
  return arcs;
}

NETptr virtual_non_tags_fsm(int zero_plus) {
  /* ----------------------------------------------- */
  /* This produces a machine with a list of current  */
  /* non-tags.  It gets updated  automatically       */
  /* whenever the sigma changes.                     */
  /* ----------------------------------------------- */
  STATEptr end_state;
  NETptr vnetwork;
  VirtualInfo *vinfo;
  VMethods *methods = get_virtual_methods(Virtual_Non_Tags);
  vnetwork = virtual_network(NULL, NULL, methods, NULL, NULL);
  NET_epsilon_free(vnetwork) = TRUE;
  sigma_add_to(NET_sigma(vnetwork), OTHER);
  vinfo = NET_virtual_info(vnetwork);
  if (zero_plus) {
    end_state = NET_start_state(vnetwork);
  } else {
    end_state = make_state(NULL, vnetwork);
    STATE_deterministic(end_state) = FALSE;
    STATE_virtual_info(end_state) = vinfo;
  }
  STATE_is_virtual(end_state) = TRUE;
  vinfo->extra_info = end_state;
  return vnetwork;
}

static VMethods *make_vmethods(char *op, int op_id, StateDataType data_type,
			       FinalPFN final_p_fn, ArcSetFN arc_set_fn)
{
  VMethods *methods = (VMethods *)calloc(sizeof(VMethods), 1);

  if (!methods)
    not_enough_memory("MAKE_VMETHODS");

  methods->operation = op;
  methods->operation_id = op_id;
  methods->data_type = data_type;
  methods->final_p_fn = final_p_fn;
  methods->arc_set_fn = arc_set_fn;
  methods->expand_other_fn = NULL;    /* Not in use now/yet? */
  methods->reclaimable = FALSE;       /* For static structures */
  return(methods);
}

static VMethods *get_virtual_methods(int operation_id)
{
  static VMethods *vconcat_methods = NULL;
  static VMethods *vintersect_methods = NULL;
  static VMethods *vminus_methods = NULL;
  static VMethods *vnegate_methods = NULL;
  static VMethods *vunion_methods = NULL;
  static VMethods *vzero_plus_methods = NULL;
  static VMethods *vdeterminize_methods = NULL;
  static VMethods *vcopy_methods = NULL;
  static VMethods *vinvert_methods = NULL;
  static VMethods *vlower_methods = NULL;
  static VMethods *voption_methods = NULL;
  static VMethods *vupper_methods = NULL;
  static VMethods *vcompose_methods = NULL;
  static VMethods *vcrossproduct_methods = NULL;
  static VMethods *vpriority_union_methods = NULL;
  static VMethods *vtags_methods = NULL;
  static VMethods *vnon_tags_methods = NULL;

  switch(operation_id)
    {
    case Virtual_Concat:
      if (!vconcat_methods) {
	vconcat_methods = make_vmethods("concat", operation_id,	STATE_PAIR,
					vconcat_final_p, vconcat_arc_set);
      }
      return(vconcat_methods);
      break;
    case Virtual_Intersect:
      if (!vintersect_methods)
	{
	vintersect_methods =
	  make_vmethods("intersect", operation_id, STATE_PAIR,
			vintersect_final_p, vintersect_arc_set);
	}
      return(vintersect_methods);
      break;
    case Virtual_Option:
      if (!voption_methods)
	voption_methods = make_vmethods("option", operation_id, DATA_STATE,
					voption_final_p, voption_arc_set);
      return(voption_methods);
      break;
    case Virtual_Minus:
      if (!vminus_methods)
	{
	vminus_methods = make_vmethods("minus", operation_id, STATE_PAIR,
				       vminus_final_p, vminus_arc_set);
	}
      return(vminus_methods);
      break;
    case Virtual_Negation:
      if (!vnegate_methods) {
	vnegate_methods = make_vmethods("negate", operation_id, DATA_STATE,
					vnegate_final_p, vnegate_arc_set);
      }
      return(vnegate_methods);
      break;
    case Virtual_Union:
      if (!vunion_methods) {
	vunion_methods = make_vmethods("union", operation_id, STATE_PAIR,
				       vunion_final_p, vunion_arc_set);
      }
      return(vunion_methods);
      break;
    case Virtual_One_Plus:
      assert(0);
      break;
    case Virtual_Zero_Plus:
      if (!vzero_plus_methods) {
	vzero_plus_methods =
	  make_vmethods("zero_plus", operation_id, DATA_STATE,
			zero_plus_final_p, vzero_plus_arc_set);
      }
      return(vzero_plus_methods);
      break;
    case Virtual_Determinize:
      if (!vdeterminize_methods) {
	vdeterminize_methods =
	  make_vmethods("determinize", operation_id, STATE_LIST,
                        vdeterminize_final_p, vdeterminize_arc_set);
      }
      return(vdeterminize_methods);
      break;
    case Virtual_Eps_Remove:
      break;
    case Virtual_Copy:
      if (!vcopy_methods) {
	vcopy_methods = make_vmethods("copy", operation_id, DATA_STATE,
				      vcopy_final_p, vcopy_arc_set);
      }
      return(vcopy_methods);
      break;
    case Virtual_Invert:
      if (!vinvert_methods) {
	vinvert_methods = make_vmethods("invert", operation_id, DATA_STATE,
					 vinvert_final_p, vinvert_arc_set);
      }
      break;
    case Virtual_Lower:
      if (!vlower_methods) {
	vlower_methods = make_vmethods("lower_net", operation_id, DATA_STATE,
				       data_state_final_p, vlower_arc_set);
      }
      return(vlower_methods);
      break;
    case Virtual_Upper:
      if (!vupper_methods) {
	vupper_methods = make_vmethods("upper_net", operation_id, DATA_STATE,
				       data_state_final_p, vupper_arc_set);
      }
      return(vupper_methods);
      break;
    case Virtual_Priority_Union:
      if (!vpriority_union_methods) {
	vpriority_union_methods =
	  make_vmethods("priority_union", operation_id, STATE_PAIR,
			vpriority_union_final_p, vpriority_union_arc_set);
      }
      break;
    case Virtual_Tags:
      if (!vtags_methods) {
	vtags_methods = make_vmethods("tags", operation_id, DATA_STATE,
				      vtags_final_p, vtags_arc_set);
      }
      return(vtags_methods);
      break;
    case Virtual_Non_Tags:
      if (!vnon_tags_methods) {
	vnon_tags_methods =
	  make_vmethods("non_tags", operation_id, DATA_STATE,
			vtags_final_p, vnon_tags_arc_set);
      }
      return(vnon_tags_methods);
      break;
    case Virtual_Compose:
      if (!vcompose_methods) {
	vcompose_methods = make_vmethods("compose", operation_id, STATE_PAIR,
					 vget_pair_final_p, vcompose_arc_set);
      }
      return(vcompose_methods);
      break;
    case Virtual_CrossProduct:
      if (!vcrossproduct_methods)
	{
	vcrossproduct_methods =
	  make_vmethods("crossproduct", operation_id, STATE_PAIR,
			vcrossproduct_final_p, vcrossproduct_arc_set);
	}
      return(vcrossproduct_methods);
      break;
    default:
      fprintf(stderr, "Uninplemented virtual operation.");
      fprintf(stderr, "Operation id: %d\n", operation_id);
      assert(0);
      break;
    }
  return(NULL);
}


void restore_virtual_network(NETptr vnet, NETptr op1, NETptr op2, int op_id)
{
  /* Called from network_from_stream() in bin-in.c to restore
     a virtual network from a file. When a virtual network
     is saved, only the id of the operation, the number of
     operands, and the operands themselves are actually saved
     into the file.  When the file is read back in, we need
     to create an initial state for it and reconsitute
     the data structures for virtual info and methods.
  */

  VirtualInfo *vinfo;
  STATEptr start;
  VMethods *methods = get_virtual_methods(op_id);

  NET_names_matter(vnet) = TRUE;

  vinfo = make_virtual_info(vnet, op1, op2, methods, NULL);
  
  /* Operand networks loaded from a file are reclaimable. */

  if (op1)
    vinfo->op1_reclaimable = TRUE;
  else
    vinfo->op1_reclaimable = FALSE;

  if (op2)
    vinfo->op2_reclaimable = TRUE;
  else
    vinfo->op2_reclaimable = FALSE;

  start = make_virtual_start_state(op1, op2, methods, vinfo);

  /* ---------------------------------------------------------------- */
  /* If not epsilonfree, we have to  worry about the epsilon closure. */
  /* ---------------------------------------------------------------- */

  if (op_id == Virtual_Determinize)
    {
      if (NET_epsilon_free(op1)) {
	methods->final_p_fn = vdeterminize_final_p;
      } else {
	methods->final_p_fn = veps_remove_final_p;
      }
    }


  if (op_id == Virtual_Negation && !(NET_closed_sigma(vnet)))
    {
      sigma_add_to(NET_sigma(vnet), OTHER);
      sigma_add_to(NET_sigma(op1), OTHER);
    }

  NET_virtual_info(vnet) = vinfo;
  NET_start_state(vnet) = start;
}


NETptr get_virtual_net(char *filename, fst_byte* cur_byte,
                       long int num_states, long int num_arcs,
                       NETptr net, FILE *stream,
                       unsigned long *byte_countp, int file_type)
{
  NETptr op1 = NULL;
  NETptr op2 = NULL;

  int op_id = (int) get_number(stream, byte_countp);
  unsigned long num_nets = get_number(stream, byte_countp);

  assert(num_nets == 1 || num_nets == 2);

  op1 = network_from_stream(stream, file_type);
  
  if (num_nets == 2)
    op2 = network_from_stream(stream, file_type);

  restore_virtual_network(net, op1, op2, op_id);

  /* The next byte contains the byte count of the virtual net. */

  *cur_byte = next_byte(stream, byte_countp);

  return(net);
}


/* ------------------------------------------------------------------------ */
/* Flushing Virtual Data                                                    */
/* ------------------------------------------------------------------------ */

void free_virtual_network(NETptr net) {
  if (NET_is_virtual(net)) {
    free_virtual_network_storage(net);
  }
  free_network(net);
}

void flush_realized_arcs(STATEptr state, VirtualInfo *vinfo) {
  ARCptr arc, next;
  if (STATE_is_virtual(state)) return;
  assert(STATE_type_bit(state));
  assert(!STATE_vector_p(state));
  arc = STATE_arc_set(state);
  while (arc) {
    assert(!ARC_type_bit(arc));
    next = next_arc(arc);
    free_arc(arc);
    arc = next;
  }
  STATE_arc_set(state) = NULL;
  STATE_is_virtual(state) = TRUE;
  STATE_virtual_info(state) = vinfo;
}

void flush_virtual_arcs(NETptr vnetwork, int recurse) {
  VirtualInfo *vinfo;
  STATEptr state;
  if (!vnetwork) return;
  if (!virtual_network_p(vnetwork)) return;
  vinfo = NET_virtual_info(vnetwork);
  for (state = NET_states(vnetwork); state; state = next_state(state)) {
    if (!STATE_state_data(state)) continue;
    flush_realized_arcs(state, vinfo);
  }
  if (!recurse) return;
  flush_virtual_arcs(vinfo->op1, recurse);
  flush_virtual_arcs(vinfo->op2, recurse);
}

int has_other_arc(STATEptr state) {
  ARCptr arc;
  id_type label;
  assert(!STATE_is_virtual(state));
  for (arc = STATE_arc_set(state); arc; arc = next_arc(arc)) {
    label = ARC_label(arc);
    if (label == OTHER) return 1;
    if (!tuple_id_p(label)) continue;
    if (id_to_upper_id(label) == OTHER) return 1;
    if (id_to_lower_id(label) == OTHER) return 1;
  }
  return 0;
}

void flush_virtual_other_arcs(NETptr vnetwork, int recurse) {
  VirtualInfo *vinfo;
  STATEptr state;
  if (!vnetwork) return;
  if (!virtual_network_p(vnetwork)) return;
  vinfo = NET_virtual_info(vnetwork);
  for (state = NET_states(vnetwork); state; state = next_state(state)) {
    if (STATE_is_virtual(state)) continue;
    if (!has_other_arc(state)) continue;
    flush_realized_arcs(state, vinfo);
  }
  if (!recurse) return;
  flush_virtual_other_arcs(vinfo->op1, recurse);
  flush_virtual_other_arcs(vinfo->op2, recurse);
}

#if 0
/* ------------------------------------------------------------------------ */
/* More Efficient Version of btree_insert.                                  */
/* ------------------------------------------------------------------------ */

static unsigned char bit_mask[8] = {128,64,32,16,8,4,2,1};

int test_bit(void *address, int bit_no) {
  unsigned char *bit_array = (unsigned char *)&address;
  return ((bit_array[ENDIAN_BYTE_OFFSET(bit_no/8)] & bit_mask[bit_no%8]) != 0);
}

static int first_bit_difference(void *addr1, void *addr2) {
  int i, j, bit1, bit2, result;
  unsigned char char1, char2;
  for (i = 0; i < sizeof(void *); i++) {
    char1 = ((unsigned char *)&addr1)[ENDIAN_BYTE_OFFSET(i)];
    char2 = ((unsigned char *)&addr2)[ENDIAN_BYTE_OFFSET(i)];
    if (char1 != char2) break;
  }
  for (j = 0; j < 8; j++) {
    bit1 = char1 & bit_mask[j];
    bit2 = char2 & bit_mask[j];
    if (bit1 != bit2) break;
  }
  result = i*8 + j;
  if (j != 8) {
#if 0
    assert(test_bit(addr1, result) != test_bit(addr2, result));
#endif
    if (bit1) {
      assert(addr1 > addr2);
    } else {
      assert(addr2 > addr1);
    }
  }
  return result;
}

BTREEptr btree_insert_address(void *item, BTREEptr tree, HEAPptr bt_heap) {
  /* -------------------------------------------------- */
  /* Inserts ITEM in TREE if it is not already present. */
  /* Inserts ITEM based on binary-branching address.    */
  /* Returns subtree containing ITEM as its root.       */
  /* -------------------------------------------------- */
  int node_diff, item_diff;
  BTREEptr left, right, result;
  if (!tree) {
    /* --------------------- */
    /* Initialize the btree. */
    /* --------------------- */
    tree = cell_alloc(bt_heap);
    BT_node(tree) = item;
    return tree;
  }
  /* ---------------------------- */
  /* Find a place to insert ITEM. */
  /* ---------------------------- */
  while (BT_left(tree)) {
    node_diff = first_bit_difference(BT_node(tree), BT_node(BT_left(tree)));
    item_diff = first_bit_difference(BT_node(tree), item);
    if (item_diff >= node_diff) {
      /* ---------------------------------------------------------- */
      /* ITEM disagrees with the tree at or below the current bit   */
      /* position.  Therefore, we need to branch here based on the  */
      /* value of that bit in item.                                 */
      /* ---------------------------------------------------------- */
      if (test_bit(item, node_diff)) {
	assert(item > BT_node(BT_left(tree)));
	tree = BT_right(tree);
      } else {
	assert(item < BT_node(BT_right(tree)));
	tree = BT_left(tree);
      }
    } else {
      /* ------------------------------------------------------------ */
      /* ITEM disagrees with the tree earlier than the tree disagrees */
      /* with its left node.  Therefore, we need to insert a branch.  */
      /* ------------------------------------------------------------ */
      break;
    }
  }
  if (item == BT_node(tree)) return tree;
  /* ------------------------------------------------------------ */
  /* Insert a branch at tree with the lesser address to the left. */
  /* We insert item on one branch, and copy tree to the other.    */
  /* ------------------------------------------------------------ */
  left = cell_alloc(bt_heap);
  right = cell_alloc(bt_heap);
  if (item > BT_node(tree)) {
    result = right;
    *left = *tree;
  } else {
    result = left;
    *right = *tree;
  }
  BT_node(result) = item;
  /* -------------------------------------------------------- */
  /* Replace TREE after left and right have been initialized. */
  /* -------------------------------------------------------- */
  BT_left(tree) = left;
  BT_right(tree) = right;
  BT_node(tree) = BT_node(right);
  BT_value(tree) = NULL;
  assert((int)left->node < (int)right->node);
  /* --------------------------- */
  /* Return RESULT as our value. */
  /* --------------------------- */
  return result;
}

#else

static int state_compare_fn(void *s1, void *s2) {
  if (s1 == s2) return 0;
  if ((int)s1 > (int)s2) return 1;
  return -1;
}

BTREEptr btree_insert_address(void *item, BTREEptr tree, HEAPptr bt_heap) {
  return btree_insert(item, tree, bt_heap, state_compare_fn);
}

#endif
  
#endif /* WORLD */
     
#endif /* SLIM_FST */
