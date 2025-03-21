/* $Id: vectorize.h,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
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

int FstBaseExport arc_vector_size (NETptr net);
void FstBaseExport make_vector_state (STATEptr state, NETptr net, int vector_size);
void FstBaseExport make_vector_state_from_reduced (STATEptr state, 
				     NETptr net,
				     int vector_size, 
				     ALPH_VECTORptr reduced_to_unreduced_map);
void FstBaseExport unmake_vector_state (STATEptr state, NETptr net, int vector_size);
void FstBaseExport vectorize_net (NETptr net);
int FstBaseExport vectorize_dense_states_of_net (NETptr net, int min_vectorize);
void FstBaseExport unvectorize_net (NETptr net);
void FstBaseExport vectorize_nets (NVptr nets);
void FstBaseExport unvectorize_nets (NVptr nets);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_VECTORIZE */
