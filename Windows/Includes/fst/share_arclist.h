/* $Id: share_arclist.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  SHARE_ARCLIST.H
 **
 **  Lauri Karttunen, Xerox PARC, October 1992
 **
 **
 ******************************************************/

#ifndef C_FSM_SHARE_ARCLIST
#define C_FSM_SHARE_ARCLIST

#include "fst/types.h"

#ifdef __cplusplus
extern "C" {
#endif
 
/* Functions for (UN)SHARE_ARCS */

#ifndef SMALL_FSM

void FstBaseExport SHARE_warning (char *fct_name, char *message);
void FstBaseExport SHARE_clear_deletemarks (NETptr net);
int FstBaseExport SHARE_delete_altchain_arcs (NETptr net, int *altchain_remains);
void FstBaseExport SHARE_delete_marked_states (NETptr net);
NETptr FstBaseExport SHARE_make_shared_arclists (NETptr net, int copy_p);

/*
void SHARE_clear_counts_and_marks (NETptr net);
void SHARE_count_shared_arcs (NETptr net);
void SHARE_mark_shared_arcs (NETptr net);
void SHARE_clear_arc_sources (NETptr net);
void SHARE_reinsert_altchain_arcs (NETptr net);
*/
NETptr FstBaseExport SHARE_make_unshared_arclists (NETptr net, int copy_p);

#endif /* SMALL_FSM */

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_SHARE_ARCLIST */
