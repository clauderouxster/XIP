/* $Id: haskell.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1997 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  HASKELL.H
 **
 **  Lauri Karttunen, Xerox PARC, October 1992
 **
 **  Functions for converting finite-state networks
 **  to Prolog clauses.
 **  
 **
 ******************************************************/
 
#ifndef C_FSM_HASKELL
#define C_FSM_HASKELL

#ifdef __cplusplus
extern "C" {
#endif

void networks_to_haskell_file(NVptr nets, FILE *stream, int sort_states_and_arcs);
void network_to_haskell_file(NETptr net, FILE *stream, int sort_states_and_arcs);

NVptr networks_from_haskell_file(FILE *stream);

void hs_net(void);
void hs_sym(int sym_id);
STATEptr hs_state(int state_id);
void hs_start(STATEptr state);
void hs_arc(STATEptr orig, id_type label_id, STATEptr dest);
void hs_final(STATEptr state);

/******************************************************
  Example:
  
  xfst[0]: read regex			[Compile a regular expression.]
  [a:x 0:y (b:z)]*;
  3 states, 4 arcs. Circular.

  xfst[1]: apply down ab		[Check it out.]
  xyz

  xfst[1]: write haskell		[Write it as a Haskell structure.]

  ([a, b, x, y, z], -- sigma
   [0, 1, 2], -- states
   [0, 2], -- final states
   0, -- initial state,
   [(0, (a, x), 1), (1, (eps, y) 2), (2, (a, x), 1), (2, (b, z) 0)] -- arcs
   )
	
 ******************************************************/
#ifdef __cplusplus
}
#endif

#endif /* C_FSM_HASKELL */
