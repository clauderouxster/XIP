/* $Id: calc-aux.h,v 1.6 2002/04/24 12:23:19 jmcoursi Exp $ */
/* Copyright (c) 1997 by the Xerox Corporation.  All rights reserved */

/***********************************************************
 *
 *			    CALC-AUX.H
 *
 *	 Auxiliary function definitions (displaced from re-aux.h)
 *
 *
 ***********************************************************/
#ifndef C_FSM_CALC_AUX_H

#define C_FSM_CALC_AUX_H

#ifdef __cplusplus
extern "C" {
#endif

int FsmFstExport nat_number(char *str);

NETptr FsmFstExport exactly(NETptr net, int n);
NETptr lab_exactly(id_type label_id, int n);

NETptr FsmFstExport between(NETptr net, int m, int n);
NETptr lab_between(id_type label_id, int m, int n);

NETptr FsmFstExport less_than(NETptr net, int n);
NETptr lab_less_than(id_type label_id, int n);

NETptr FsmFstExport more_than(NETptr net, int n);
NETptr lab_more_than(id_type label_id, int n);

NETptr FsmFstExport product(NETptr upper, NETptr lower);
NETptr concat_lab(NETptr net1, id_type id);
NETptr kleene_star_lab(id_type id);
NETptr kleene_plus_lab(id_type id);

NETptr FsmFstExport symbol_to_fsm(id_type label_id);
NETptr label_to_net(id_type label_id);
NETptr ignore_lab(NETptr net1, id_type id);

NETptr contains_lab(id_type id);
NETptr read_or_take(char *str);
NETptr FsmFstExport read_file(char *name);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_CALC_AUX_H */
