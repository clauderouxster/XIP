/* $Id: twol-net.h,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 1993 Xerox Corporation. All rights reserved. */

/*************************************************
 *
 *  TWOL-NET.H
 * 
 *  Lauri Karttunen, RXRC
 *  December 1993
 *
 *************************************************/

#ifndef C_FSM_TWOL_NET

#define C_FSM_TWOL_NET

#ifdef __cplusplus
extern "C" {
#endif

ALPHABETptr FstBaseExport decompress_alphabet(ALPHABETptr alph, int copy_p);
void FstBaseExport decompress_net(NETptr net);
void standardize_sigma(NETptr net);
void FstBaseExport convert_twol_nets(NVptr nets, int quiet_p);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_TWOL_NET */

