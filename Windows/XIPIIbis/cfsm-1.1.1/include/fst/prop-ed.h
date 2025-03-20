/* $Id: prop-ed.h,v 1.1 1996/12/11 17:49:45 gaal Exp $ */
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

void print_prop_names(NETptr net);
void PE_menu(void);
int PE_check_range(NETptr net, int num);
void PE_add_prop (NETptr net);
void PE_delete_prop (NETptr net);
void PE_modify_prop (NETptr net);
void PE_print_prop (NETptr net);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_PROP_ED */
