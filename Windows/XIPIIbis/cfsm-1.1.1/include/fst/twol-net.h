/* $Id: twol-net.h,v 1.3 2002/04/24 15:44:05 jmcoursi Exp $ */
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

ALPHABETptr FsmFstExport decompress_alphabet(ALPHABETptr alph, int copy_p);
void FsmFstExport decompress_net(NETptr net);
void standardize_sigma(NETptr net);
void FsmFstExport convert_twol_nets(NVptr nets, int quiet_p);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_TWOL_NET */

