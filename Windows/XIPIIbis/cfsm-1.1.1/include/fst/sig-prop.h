/* $Id: sig-prop.h,v 1.2 2002/04/24 15:44:02 jmcoursi Exp $ */
/******************************************************
 **
 **  SIG-PROP.H
 **
 **  Lauri Karttunen, RXRC 1993
 **  
 **  Functions for storing and retrieving alphabets
 **  as network properties. Used to be in SIGMA-IO.C.
 **
 ******************************************************/

#ifndef C_FSM_SIG_PROP

#define C_FSM_SIG_PROP

#ifdef __cplusplus
extern "C" {
#endif

void recover_sigma(NETptr net);
ALPHABETptr FsmFstExport prop_val_to_alph (OBJECTptr val, int type);
OBJECTptr FsmFstExport alph_to_prop_val(ALPHABETptr alph);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_SIG_PROP */
