/* $Id: virtual-base.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */ 
/* Used to be called virtual-fsm-base.c. Name shortened to stay within
   the 15 character limit on filenames that affect the 'ar' command
   in some systems such as Solaris where 'ar -xv libfsmbase.a' doesn't
   work on longer filenames. */

/* Copyright (c) 2001 by the Xerox Corporation.  All rights reserved */ 

/****************************************************** 
** 
**  virtual-base.c
** 
**  Caroline Privault, XRCE, January 2002
** 
**  Low level functions (from J. Maxwell: moved from 
**  file "virtual-fsm.c") that are needed in the BASE 
**  library, in both WORLD and NON-WORLD mode.
**  
******************************************************/ 

#include <assert.h>

#include "fst/types.h"
#include "fst/virtual-base.h"

/* The whole file content is necessary when SLIM_FST is defined. */ 

int dead_state_p(STATEptr state) {
  /* RMK: Because virtual_info overlays the arcset field, virtual states 
     will not be considered dead. */
  return !(STATE_final(state) || STATE_arc_set(state));
}

ARCptr virtual_arc_set(STATEptr vstate) {
  /* RMK: Rewriting any algorithm (lookup, lookdown...) that fetches 
     a state's arcset field so that it calls VARCSET instead will make 
     that algorithm also work on virtual machines. */
  VirtualInfo *vinfo;
  ARCptr arc_set;
  int final;
  if (!STATE_is_virtual(vstate)) return STATE_arc_set(vstate);
  vinfo = STATE_virtual_info(vstate);
  assert(vinfo);
#if 0
  /* ------------------------------------------------ */
  /* Verify that the state is in the virtual network. */
  /* ------------------------------------------------ */
  assert(state_in_network(vstate, vinfo->network));
#endif
  arc_set = vinfo->methods->arc_set_fn(vstate, vinfo);
  final = vinfo->methods->final_p_fn(vstate, vinfo);
  STATE_arc_set(vstate) = arc_set;
  STATE_final(vstate) = final;
  STATE_is_virtual(vstate) = FALSE;

  if (dead_state_p(vstate)) {
    /* RMK: A really dead state--keep track for pruning heuristics. */
    vinfo->n_dead_states++;
  }
  return arc_set;
}


int virtual_final_p(STATEptr vstate) {
  /* RMK: Rewriting any algorithm (lookup, lookdown...) that fetches a 
     state's final field so that it calls virtual_final_p instead will make 
     that algorithm also work on virtual machines.   Note that is OK 
     to fetch final directly if you are sure that virtual_arc_set or 
     virtual_final_p were previously called. */
  virtual_arc_set(vstate);
  return STATE_final(vstate);
}


