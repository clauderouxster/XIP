/* $Id: bin-in.h,v 1.4 2005/03/04 15:45:21 grondeau Exp $ */
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

#include "fst/id_type.h"
#include "fst/header.h"
#include "fst/vectorize.h"
#include "fst/flags.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Set up of the global restore_functions array. */
void FstBaseExport init_restore_functions();

/* Control of the use of mmap() for compacted net files. */
extern int FstBaseExport MEMORY_MAP;
    
extern void FstBaseExport (*transition_table_from_stream)(FILE *stream, NETptr net);

long FstBaseExport get_number(FILE *stream, unsigned long *byte_countp);

NETptr FstBaseExport get_arcs_and_states(char *filename, fst_byte* cur_byte,
                                        long int num_states, long int num_arcs,
                                        NETptr net, FILE *stream,
                                        unsigned long *byte_countp, int file_type);
    
NETptr FstBaseExport get_arc_block(char *filename, fst_byte* cur_byte,
                                  long int num_states, long int num_arcs,
                                  NETptr net, FILE *stream,
                                  unsigned long *byte_countp, int file_type);
    
NETptr FstBaseExport network_from_stream (FILE *stream, int file_type);
    
NVptr FstBaseExport networks_from_file(FILE *stream);

NETptr FstBaseExport read_Kaplan_compressed_network(char *filename, fst_byte* cur_byte,
                                                   long int num_states, long int num_arcs,
                                                   NETptr net, FILE *stream,
                                                   unsigned long *byte_countp, int file_type);
NVptr FstBaseExport read_nets(char *filename);
NVptr FstBaseExport standard_nets_from_stream(FILE *stream);
NETptr FstBaseExport read_first_net(char *filename);

#define network_from_file(X) networks_from_file((X))

int FstBaseExport display_read_status_bar(int on_off);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_BIN_IN */


