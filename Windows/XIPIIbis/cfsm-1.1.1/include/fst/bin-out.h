/* $Id: bin-out.h,v 1.17 2002/06/11 01:59:39 lk Exp $ */
/* Copyright (c) 1990-93 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  BIN-OUT.H
 **
 **	 Todd Yampol, Xerox PARC, February 1990
 **
 **  Binary output routines for fsms.
 **
 ******************************************************/

#ifndef C_FSM_BIN_OUT

#define C_FSM_BIN_OUT

#include "header.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STANDARD_ATTRIBUTE_COUNT 7
extern char *STANDARD_ATTRIBUTES[STANDARD_ATTRIBUTE_COUNT];
extern char *STANDARD_VALUES[STANDARD_ATTRIBUTE_COUNT];

void FsmFstExport edit_standard_attributes(int clear_p);
void FsmFstExport add_standard_attributes(NETptr net);

typedef void *VOIDptr;
extern VOIDptr (*transition_table_from_net)(NETptr net);
extern void (*transition_table_to_file)(FILE *stream, NETptr net, VOIDptr ntt);

int FsmFstExport write_arc_block(FILE *stream, unsigned long *byte_countp,
				 NETptr net, unsigned long *arc_count,
				 unsigned long *state_count, unsigned long
				 *file_lengthp, int old_format_p);    
    
#ifndef WORLD
int FsmFstExport write_virtual_net(FILE *stream, unsigned long *byte_countp,
                                    NETptr net, unsigned long *arc_count,
                                    unsigned long *state_count,
				   unsigned long *file_lengthp,
				   int old_format_p);
#endif

int FsmFstExport write_arcs_and_states(FILE *stream, unsigned long
				       *byte_countp, NETptr net,
				       unsigned long *arc_count,
				       unsigned long *state_count,
				       unsigned long *file_lengthp,
				       int old_format_p);
    
int FsmFstExport network_to_stream(NETptr net, FILE *stream, unsigned long *byte_count, int old_format_p);

void FsmFstExport real_networks_to_file (NVptr nets, FILE *stream, int net_count, int old_format_p);

/* below:  X = nets (NET_VECTOR), Y = stream */
#define network_to_file(X,Y) real_networks_to_file((X), (Y), 1, 0)
#define networks_to_file(X,Y) real_networks_to_file((X), (Y), 0, 0)

void optimize_labels(NETptr net, ALPHABETptr *optimized_map);

int compare_label_freq (const void *a, const void *b);
int compare_label_freq_symbols (const void *a, const void *b);
int compare_label_freq_symbols_precede_tags (const void *a, const void *b);

void tally_labels(NETptr net);
void print_tally(FILE * ofile);
TALLYptr make_symbol_tally_export(int size);
TALLYptr make_tally_sigma(NETptr net, 
		      int * sigma_tally_count,
		      id_type * max_sigma_id);
void sort_this_tally(TALLYptr tally_sigma, id_type max_sigma_id,
			    int comp_fn (const void *a, const void *b));
void print_this_tally(FILE * ofile,
		      TALLYptr tally_this, 
		      int this_tally_count,
		      id_type max_this_tally_id,
		      int comp_fn (const void *a, const void *b));

int write_nets(NVptr nets, char *filename);
int FsmFstExport write_net(NETptr net, char *filename);

int display_write_status_bar(int on_off);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_BIN_OUT */

