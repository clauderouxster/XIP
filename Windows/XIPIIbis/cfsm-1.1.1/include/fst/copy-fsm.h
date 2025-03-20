/* $Id: copy-fsm.h,v 1.5 2002/11/29 08:32:54 privault Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */


/******************************************************
 **
 **  COPY-FSM.H
 **
 **  Lauri Karttunen, Xerox PARC, March 1991
 **  Copying fsm structures
 **
 ******************************************************/

#ifndef C_FSM_COPY_FSM

#define C_FSM_COPY_FSM

#ifdef __cplusplus
extern "C" {
#endif

OBJECTptr copy_object(OBJECTptr obj);
void copy_properties(NETptr source, NETptr target);
ARCptr FsmFstExport copy_arc (ARCptr arc, NETptr net);
STATEptr copy_state (STATEptr state, NETptr net);
NETptr FsmFstExport copy_fsm(NETptr old_net);
NVptr copy_net_vector (NVptr old_vec);

NETptr state_prefix_net(NETptr net, STATEptr state);
NETptr state_suffix_net(NETptr net, STATEptr state);
NETptr state_to_state_subnet(NETptr net, STATEptr start, STATEptr end);

#ifdef __cplusplus
}
#endif

#endif
