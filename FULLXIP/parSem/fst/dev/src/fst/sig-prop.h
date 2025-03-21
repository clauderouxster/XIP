/* $Id: sig-prop.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
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
ALPHABETptr FstBaseExport prop_val_to_alph (OBJECTptr val, int type);
OBJECTptr FstBaseExport alph_to_prop_val(ALPHABETptr alph);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_SIG_PROP */
