/* $Id: prop-ed.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1994  by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  PROP-ED.H
 **
 **  Lauri Karttunen, RXRC 1994
 **  
 **  Functions for setting and adding network properties
 **  from files.
 **
 ******************************************************/
 
#ifndef C_FSM_PROP_ED
#define C_FSM_PROP_ED

#ifdef __cplusplus
extern "C" {
#endif

void FstCalcExport print_prop_names(NETptr net);
void FstCalcExport PE_menu(void);
int FstCalcExport PE_check_range(NETptr net, int num);
void FstCalcExport PE_add_prop (NETptr net);
void FstCalcExport PE_delete_prop (NETptr net);
void FstCalcExport PE_modify_prop (NETptr net);
void FstCalcExport PE_print_prop (NETptr net);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_PROP_ED */
