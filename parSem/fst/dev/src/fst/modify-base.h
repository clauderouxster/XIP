/* $Id: modify-base.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990-94 Xerox Corporation. All rights reserved. */
/***************************
 *
 * MODIFY.H
 *
 * Function prototypes for functions that maintain
 * and modify fsms. This used to be called BASICS.H.
 *
 *
 ***************************/

#ifndef C_FSM_MODIFY_BASE

#define C_FSM_MODIFY_BASE

#include "fst/struct.h"

#ifdef __cplusplus
extern "C" {
#endif

void FstBaseExport invert_arcs(NETptr net, int mark);
int FstBaseExport state_mark_zero_p(STATEptr state);
int FstBaseExport state_mark_one_p(STATEptr state);
NETptr FstBaseExport make_connected(NETptr net, int inverted_p);
void FstBaseExport label_check (NETptr net);
ALPHABETptr renumber_nets(NVptr nets);
void FstBaseExport renumber_net(NETptr net);
NETptr FstBaseExport prune_fsm (NETptr net, int copy_p);
NETptr FstBaseExport epsilon_remove (NETptr net, int copy_p);
NETptr FstBaseExport determinize_fsm(NETptr net, int copy_p);
int FstBaseExport determinize_basic (NETptr net);
void FstBaseExport prune_sigma(NETptr net);
NETptr FstBaseExport min_fsm(NETptr net, int copy_p);
int FstBaseExport index_net (NETptr net);

FstBaseExport void recover_temp_memory(void);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_MODIFY_BASE */

