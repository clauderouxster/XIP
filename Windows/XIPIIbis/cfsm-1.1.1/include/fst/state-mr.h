/* $Id: state-mr.h,v 1.2 1997/11/28 17:28:04 lk Exp $ */
/* Copyright (c) 1997 by the Xerox Corporation.  All rights reserved */

 /******************************************************
 **
 **  STATE-MR.H
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

#ifndef C_FSM_STATE_MR

#define C_FSM_STATE_MR

#ifdef __cplusplus
extern "C" {
#endif

typedef struct STATE_MANAGER {
  struct NETWORK  *new_net;
  struct HEAP     *btree_heap;
  struct HEAP     *cons_heap;
  struct STACK    *new_states;
  int index_on_s2;
} STATE_MR, *STATE_MRptr;

#define STATE_MR_net(X)          (X)->new_net
#define STATE_MR_btree_heap(X)   (X)->btree_heap
#define STATE_MR_cons_heap(X)    (X)->cons_heap
#define STATE_MR_new_states(X)   (X)->new_states
#define STATE_MR_index_on_s2(X)  (X)->index_on_s2

STATE_MRptr init_state_manager(NETptr old1, NETptr old2, NETptr new, NETptr inext_net);
void free_state_manager(STATE_MRptr state_manager);

STATEptr find_pair_state(STATEptr s1, STATEptr s2, STATE_MRptr state_mr);
void pair_states (STATEptr s1, STATEptr s2, STATE_MRptr state_mr);
STATEptr next_new_state(STATE_MRptr state_mr);
void reclaim_cell(CONSptr cons, STATE_MRptr state_mr);

#ifdef __cplusplus
}
#endif

#endif
