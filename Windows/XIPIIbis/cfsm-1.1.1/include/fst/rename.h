/* $Id: rename.h,v 1.6 2002/04/11 14:51:32 privault Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  RENAME.H
 **
 **	 Todd Yampol, Xerox PARC, January 1990
 **  Lauri Karttunen, Xerox PARC, April 1991, RXRC January 1994
 **
 **
 ******************************************************/

#ifndef C_FSM_RENAME
#define C_FSM_RENAME

#ifdef __cplusplus
extern "C" {
#endif

int FsmFstExport default_arc_comp_fn (const void *a1, const void *a2);
int down_left_bm_arc_comp_fn (const void *a1, const void *a2);
int up_left_bm_arc_comp_fn (const void *a1, const void *a2);
int down_right_bm_arc_comp_fn (const void *a1, const void *a2);
int up_right_bm_arc_comp_fn (const void *a1, const void *a2);

NETptr FsmFstExport rename_states (NETptr net);
NETptr top_sort_net(NETptr net);
NETptr FsmFstExport reorder_arcs (NETptr net, int (*arc_comp_fn)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_RENAME */
