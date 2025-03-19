/* $Id: arc-optimization.h,v 1.4 2005/03/04 15:45:21 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  ARC-OPTIMZATON.H
 **
 **  Lauri Karttunen, Xerox PARC, October 1992
 **
 **
 ******************************************************/

#ifndef C_FSM_ARC_OPTIMZATION
#define C_FSM_ARC_OPTIMZATION

#include "fst/types.h"
#ifdef __cplusplus
extern "C" {
#endif
 
/* Tests if some net arcs are labeled with ALTCHAIN symbols:
   helps to know if a net went previously through 
   do_optimize_arcs(), but does not garantee that it is
   optimized.*/
int FstBaseExport possibly_already_optimized_p(NETptr net);
    
NETptr FstBaseExport optimize_arc_chains (NETptr net, ALPHABETptr dont_reorder_labels, 
			    int dont_reorder_mark, int ignore_state_mark,
			    int reorder_p);
void FstBaseExport do_optimize_arcs(NETptr net);
void FstBaseExport do_unoptimize_arcs(NETptr net); 
void FstBaseExport sort_arcs_by_arc_indegree(NETptr net, ALPHABETptr initialchars, 
                               ALPHABETptr finalchars);

#ifndef SMALL_FSM
ARCptr FstBaseExport make_big_arc (id_type label, STATEptr dest, NETptr net); 
void FstBaseExport make_arcs_big (NETptr net); /* transforms ARCs into BIG_ARCs */
void FstBaseExport make_arcs_small (NETptr net); /* transforms BIG_ARCs into ARCs */
#endif /* SMALL_FSM */

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_ARC_OPTIMZATION */



