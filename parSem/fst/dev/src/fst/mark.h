/* $Id: mark.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990-94 Xerox Corporation. All rights reserved. */
/***************************
 *
 * MARK.H
 *
 *
 *
 ***************************/

#ifndef C_FSM_MARK

#define C_FSM_MARK

#include "fst/struct.h"

#ifdef __cplusplus
extern "C" {
#endif

void FstBaseExport prepare_arc_marks(NETptr net, int mark);
void FstBaseExport prepare_state_marks(NETptr net, int mark);
NVptr FstBaseExport remove_duplicates(NVptr nv);

#define clear_arc_marks(X)	prepare_arc_marks((X), NOT_VISITED)
#define clear_state_marks(X) prepare_state_marks((X), NOT_VISITED)

void FstBaseExport clear_all_visit_marks_and_state_client_cells(NETptr net);
void FstBaseExport clear_client_cells(NETptr net);
void FstBaseExport clear_state_marks_and_client_cells(NETptr net);
void FstBaseExport prepare_visit_marks(NETptr net, int mark);
void FstBaseExport mark_reachable_states(STATEptr state, int mark, STACKptr stack);
    
#ifdef __cplusplus
}
#endif

#endif /* C_FSM_MARK */

