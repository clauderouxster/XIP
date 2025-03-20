/* $Id: vectorize.h,v 1.5 2002/04/24 12:23:30 jmcoursi Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved.
   Copyright protection claimed includes all forms and matters of
   copyrightable material and information now allowed by statutory or
   judicial law or hereinafter granted, including without limitation,
   material generated from the software programs which are displayed 
   on the screen such as icons, screen display looks, etc.
   */

/*******************************************************
 **
 **  vectorize.h
 **
 **	 Todd Yampol, Xerox PARC, January 1993.
 **
 **  Headers & definitions for vectorize.c.
 ** 
 ********************************************************/

#ifndef C_FSM_VECTORIZE

#define C_FSM_VECTORIZE

#ifdef __cplusplus
extern "C" {
#endif

int FsmFstExport arc_vector_size (NETptr net);
void FsmFstExport make_vector_state (STATEptr state, NETptr net, int vector_size);
void make_vector_state_from_reduced (STATEptr state, 
				     NETptr net,
				     int vector_size, 
				     ALPH_VECTORptr reduced_to_unreduced_map);
void unmake_vector_state (STATEptr state, NETptr net, int vector_size);
void FsmFstExport vectorize_net (NETptr net);
int FsmFstExport vectorize_dense_states_of_net (NETptr net, int min_vectorize);
void unvectorize_net (NETptr net);
void FsmFstExport vectorize_nets (NVptr nets);
void unvectorize_nets (NVptr nets);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_VECTORIZE */
