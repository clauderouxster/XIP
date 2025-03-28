/* (c) 2000 by the Xerox Corporation.  All rights reserved */
#ifndef VIRTUAL_FSM_H
#define VIRTUAL_FSM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Virtual networks are like regular networks except that they construct
   states lazily, as needed.  This is done by associating information
   with each state that tells how to get the arcs and final bit of the
   state.  The state gets realized when virtual_arc_set or virtual_final_p
   gets called. Virtual networks are initialized with a start state.
   All of the rest of the states are constructed on demand. */

/* ------------------------------------------------------------------------ */
/* THE VIRTUAL CALCULUS                                                     */
/* ------------------------------------------------------------------------ */

NETptr virtual_concat_fsm(NETptr op1, NETptr op2, int reclaim_p);
NETptr virtual_intersect_fsm(NETptr op1, NETptr op2, int reclaim_p);
NETptr virtual_option_fsm(NETptr op, int reclaim_p);
NETptr virtual_minus_fsm(NETptr n1, NETptr n2, int reclaim_p);
NETptr virtual_negate_fsm(NETptr op, int reclaim_p);
NETptr virtual_union_fsm(NETptr op1, NETptr op2, int reclaim_p);
NETptr virtual_zero_plus(NETptr op, int reclaim_p);
NETptr virtual_one_plus(NETptr op, int reclaim_p);
NETptr virtual_determinize(NETptr op, int reclaim_p);
NETptr virtual_eps_remove(NETptr op, int reclaim_p);
NETptr virtual_copy_fsm(NETptr op, int reclaim_p);
NETptr virtual_invert_fst(NETptr op, int reclaim_p);
NETptr virtual_lower_fsm(NETptr op, int reclaim_p);
NETptr virtual_upper_fsm(NETptr op, int reclaim_p);
NETptr virtual_priority_union(NETptr op1, NETptr op2, int reclaim_p);
NETptr virtual_compose_fst(NETptr op1, NETptr op2, int reclaim_p);

/* ------------------------------------------------------- */
/* virtual_tags_fsm produces a machine consisting of the   */
/* current tags.  virtual_non_tags_fsm has all of the non- */
/* tags. These get updated automatically whenever          */
/* the sigma is changed.                                   */
/* ------------------------------------------------------- */
NETptr virtual_tags_fsm();
NETptr virtual_non_tags_fsm(int zero_plus);

/* ------------------------------------------------------------------- */
/* UTILITY OPERATIONS                                                  */
/* ------------------------------------------------------------------- */

NETptr virtual_to_real_fsm(NETptr vnetwork, int minimize_p);
int virtual_network_p(NETptr n);
void free_virtual_network(NETptr net);
void set_virtual_net_operand_reclaimability(NETptr vnet, int reclaim_p);

/* ------------------------------------------------------------------- */
/* Restoring a virtual network from a file                             */
/* ------------------------------------------------------------------- */

void restore_virtual_network(NETptr vnet, NETptr op1, NETptr op2, int op_id);

/* ---------------------------------------------------- */
/* The following can be used to access a virtual state. */
/* ---------------------------------------------------- */
ARCptr virtual_arc_set(STATEptr vstate);
int virtual_final_p(STATEptr vstate);

/*
  The following is used to enable explicit access to the bytes
  of a pointer in an endian independent way.
  always use pointer[ENDIAN_BYT_OFFSET(i)] instead of 
  pointer[i] to get ith byte in pointer.
  This assumes only Linux is little endian
 */


#ifndef REDHAT60_LINUX
#define ENDIAN_BYTE_OFFSET(b) (b)
#else
#define ENDIAN_BYTE_OFFSET(b) ((sizeof(void *) - 1) - b)
#endif

#ifdef __cplusplus
}
#endif

#endif
