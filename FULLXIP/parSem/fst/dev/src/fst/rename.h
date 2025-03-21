/* $Id: rename.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
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

int FstBaseExport default_arc_comp_fn (const void *a1, const void *a2);
int FstBaseExport down_left_bm_arc_comp_fn (const void *a1, const void *a2);
int FstBaseExport up_left_bm_arc_comp_fn (const void *a1, const void *a2);
int FstBaseExport down_right_bm_arc_comp_fn (const void *a1, const void *a2);
int FstBaseExport up_right_bm_arc_comp_fn (const void *a1, const void *a2);

NETptr FstBaseExport rename_states (NETptr net);
NETptr FstBaseExport top_sort_net(NETptr net);
NETptr FstBaseExport reorder_arcs (NETptr net, int arc_comp_fn(const void *,const void *));

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_RENAME */
