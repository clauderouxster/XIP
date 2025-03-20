/* $Id: bin-in.h,v 1.17 2002/11/27 10:13:56 privault Exp $ */
/* Copyright (c) 1990-93 by the Xerox Corporation.  All rights reserved */


/******************************************************
 **
 **  bin-in.h
 **
 **	 Binary input functions for networks
 **  Lauri Karttunen
 **  December 3, 1993
 **  RXRC
 **
 ******************************************************/
 
 
#ifndef C_FSM_BIN_IN

#define C_FSM_BIN_IN

#include "id_type.h"
#include "header.h"
#include "vectorize.h"
#include "flags.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Control of the use of mmap() for compacted net files. */
extern int FsmFstExport MEMORY_MAP;
    
extern void (*transition_table_from_stream)(FILE *stream, NETptr net);

long FsmFstExport get_number(FILE *stream, unsigned long *byte_countp);

NETptr FsmFstExport get_arcs_and_states(char *filename, byte* cur_byte,
                                        long int num_states, long int num_arcs,
                                        NETptr net, FILE *stream,
                                        unsigned long *byte_countp, int file_type);
    
NETptr FsmFstExport get_arc_block(char *filename, byte* cur_byte,
                                  long int num_states, long int num_arcs,
                                  NETptr net, FILE *stream,
                                  unsigned long *byte_countp, int file_type);
    
NETptr FsmFstExport network_from_stream (FILE *stream, int file_type);
    
NVptr FsmFstExport networks_from_file(FILE *stream);

NETptr FsmFstExport read_Kaplan_compressed_network(char *filename, byte* cur_byte,
                                                   long int num_states, long int num_arcs,
                                                   NETptr net, FILE *stream,
                                                   unsigned long *byte_countp, int file_type);
NVptr FsmFstExport read_nets(char *filename);
NVptr standard_nets_from_stream(FILE *stream);
NETptr FsmFstExport read_first_net(char *filename);

#define network_from_file(X) networks_from_file((X))

int FsmFstExport display_read_status_bar(int on_off);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_BIN_IN */


/***************************************************************
 *
 *  THIS IS ONLY TEMPORARILY HERE AND WILL BE PUT
 *  INTO  regex-in.c  AND  regex-in.h
 */

NETptr FsmFstExport net_from_atfile (char *filename, int filetype, int fatstr_p);


