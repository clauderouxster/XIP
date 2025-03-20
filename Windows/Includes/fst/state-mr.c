/* $Id: state-mr.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1997 by the Xerox Corporation.  All rights reserved */

 /******************************************************
 **
 **  STATE-MR.C
 **
 **	 Lauri Karttunen, XRCE 1997
 **
 **  Manages resources for routines that create new states
 **  that represent two original states, for example,
 **  intersection and composition. In the old c-fsm code
 **  new states were created with the function get_pair_state().
 **  We keep the same function name. The new version takes
 **  Three arguments: OLD_STATE1, OLD_STATE2, STATE_MANAGER.
 **  The state manager allocates states, cons cells and
 **  binary tree nodes, as needed without using global
 **  variables.
 **
 ******************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "fst/struct.h"
#include "fst/modify.h"
#include "fst/state-mr.h"
#include "fst/btree.h"
#include "fst/mark.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

STATE_MRptr init_state_manager(NETptr old1, NETptr old2, NETptr new,
			       NETptr index_net)
{
  /* Sets up storage for creating states in new that correspond
     to pairs of states drawn from old1 and old2. The btree
     index to what old1 state is paired with what old2 states
     is stored in the client cells of old1 states unless index_on_s2
     flag is non-NULL. Whichever net is chosen, the client cells
     must be clean.
     */

 STATE_MRptr state_mr;

  state_mr = (STATE_MRptr) malloc((size_t) sizeof(STATE_MR));

  if (!state_mr)
    not_enough_memory("INIT_STATE_MANAGER");

  STATE_MR_btree_heap(state_mr) = make_btree_heap();
  STATE_MR_cons_heap(state_mr) = make_cons_heap();
  STATE_MR_new_states(state_mr) = create_stack("New pair states");
  STATE_MR_net(state_mr) = new;

  if ((STATE_MR_index_on_s2(state_mr) = (old2 == index_net)))
    clear_client_cells(old2);    
   else if (index_net == old1)
    clear_client_cells(old1);
  else
    handle_error("INDEX_NET is not OLD1 or OLD2", "INIT_STATE_MANAGER", 0);

  return(state_mr);
}

void free_state_manager(STATE_MRptr state_mr)
{
  free_heap(STATE_MR_btree_heap(state_mr));
  free_heap(STATE_MR_cons_heap(state_mr));
  free_stack(STATE_MR_new_states(state_mr));
  free((void *) state_mr);
}

static int state_compare_fn(void *s1, void *s2)
{
  if (s1 == s2)
    return (0);
  else if (s1 < s2)
    return (1);
  else
    return (-1);
}

STATEptr find_pair_state(STATEptr s1, STATEptr s2, STATE_MRptr state_mr)
{
  /*****
       *
       * FIND_PAIR_STATE gets new state for machines whose states are
       * drawn from the cross-product of states from 2 other machines, e.g., 
       * INTERSECT_FSM and MINUS_FSM. Caller must initialize STATE_MR.
       *
       * S2_INDEX_P is FALSE, we look for a binary tree in the client
       * cell of S1, and search for S2 using its address as the key.
       * If S2_INDEX_P is TRUE, the binary tree starts in the client cell
       * of S2 and the key is the address of S1.
      
       * The value cell correspdonding to the pair S1, S2 is a state in
       * NEW_NET.  When the state is created, we store the pair S1, S2
       * as a cons cell to the client cell of the new state for further
       * processing.  Note that the order of S1 and S2 in the cons cell
       * is fixed regardless of where the binary search tree is located.
       *
       * NEW_NET is the network being created by the calling function.
       * In this implementation, a state must be created within a network.
       * (the memory that the states occupy is maintained locally for each
       * network.)
       *
       *****/

  STATEptr new_state;
  BTREEptr node;
  
  if (STATE_MR_index_on_s2(state_mr))
    {
      node = btree_insert((void *) s1, (BTREEptr) STATE_client_cell(s2),
			  STATE_MR_btree_heap(state_mr), state_compare_fn);
      
      if (STATE_client_cell(s2) == NULL)
        
        /* NODE is the first one in INDEX_STATE */
        STATE_client_cell(s2) = (void *) node;
    }
  else
    {
      node = btree_insert((void *) s2, (BTREEptr) STATE_client_cell(s1),
			  STATE_MR_btree_heap(state_mr), state_compare_fn);
      
      if (STATE_client_cell(s1) == NULL)
        STATE_client_cell(s1) = (void *) node;
    }
  
  if (BT_value(node) == (void *) NULL) /* node has just been created */
    {
      /*
       * No node for s2 was found, so a new one was created by bt_insert().
       * Create a new state with FINAL bit set to TRUE, so that
       * an intermediate prune works right even for states still
       * on the stack.  After processing of a state is completed, caller
       * must set FINAL bit properly.
       */
      
      new_state = make_state(NULL, STATE_MR_net(state_mr));
      STATE_client_cell(new_state) = 
	cons_cell(s1, (CONSptr) s2, STATE_MR_cons_heap(state_mr));
      /* makes a dotted pair */
      STATE_final(new_state) = TRUE;
      
      push(new_state, STATE_MR_new_states(state_mr));
      BT_value(node) = (void *) new_state;
    }
  
  return (BT_value(node));
}


STATEptr next_new_state(STATE_MRptr state_mr)
{
  return((STATEptr) pop(STATE_MR_new_states(state_mr)));
}

void reclaim_cell(CONSptr cons, STATE_MRptr state_mr)
{
  free_cell((void *) cons, STATE_MR_cons_heap(state_mr));
}

void pair_states (STATEptr s1, STATEptr s2, STATE_MRptr state_mr)
     /*****
      *
      * PAIRED_STATES uses the client cell of S1 to check whether
      * S1 and S2 have been paired.  We try to locate s2 using its address
      * as a key in a binary tree of S1.  If we do not find a node for S2
      * we create it and push the pair <S1, S2> to PAIR_STACK. If S2 node
      * is found, no further action is taken.  Caller must initialize
      * STATE_MR.
      *****/
{
  BTREEptr node = btree_insert((void *) s2, (BTREEptr) STATE_client_cell(s1),
			       STATE_MR_btree_heap(state_mr), state_compare_fn);
  
  if (BT_value(node) == (void *) NULL)
    {
      /* NULL value means that node has just been created */
      
      BT_value(node) = (void *) 1;
      
      if (STATE_client_cell(s1) == NULL)
        /* NODE is the first one in s1 */
        STATE_client_cell(s1) = (void *) node;
      
      push(cons_cell((void *) s1, (CONSptr) s2, STATE_MR_cons_heap(state_mr)),
	   STATE_MR_new_states(state_mr));
    }
}

#endif /* SLIM_FST */
