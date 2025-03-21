/* $Id: bin-out.h,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
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

#include "fst/header.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STANDARD_ATTRIBUTE_COUNT 7
extern char *STANDARD_ATTRIBUTES[STANDARD_ATTRIBUTE_COUNT];
extern char *STANDARD_VALUES[STANDARD_ATTRIBUTE_COUNT];

void FstBaseExport edit_standard_attributes(int clear_p);
void FstBaseExport add_standard_attributes(NETptr net);

typedef void *VOIDptr;
extern VOIDptr FstBaseExport (*transition_table_from_net)(NETptr net);
extern void FstBaseExport (*transition_table_to_file)(FILE *stream, NETptr net, VOIDptr ntt);

int FstBaseExport write_arc_block(FILE *stream, unsigned long *byte_countp,
				 NETptr net, unsigned long *arc_count,
				 unsigned long *state_count, unsigned long
				 *file_lengthp, int old_format_p);    
    
#ifndef WORLD
int FstBaseExport write_virtual_net(FILE *stream, unsigned long *byte_countp,
                                    NETptr net, unsigned long *arc_count,
                                    unsigned long *state_count,
				   unsigned long *file_lengthp,
				   int old_format_p);
#endif

int FstBaseExport write_arcs_and_states(FILE *stream, unsigned long
				       *byte_countp, NETptr net,
				       unsigned long *arc_count,
				       unsigned long *state_count,
				       unsigned long *file_lengthp,
				       int old_format_p);
    
int FstBaseExport network_to_stream(NETptr net, FILE *stream, unsigned long *byte_count, int old_format_p);

void FstBaseExport real_networks_to_file (NVptr nets, FILE *stream, int net_count, int old_format_p);

/* below:  X = nets (NET_VECTOR), Y = stream */
#define network_to_file(X,Y) real_networks_to_file((X), (Y), 1, 0)
#define networks_to_file(X,Y) real_networks_to_file((X), (Y), 0, 0)

void FstBaseExport optimize_labels(NETptr net, ALPHABETptr *optimized_map);

int FstBaseExport compare_label_freq (const void *a, const void *b);
int FstBaseExport compare_label_freq_symbols (const void *a, const void *b);
int FstBaseExport compare_label_freq_symbols_precede_tags (const void *a, const void *b);

extern TALLYptr FstBaseExport SYMBOL_TALLY;

void FstBaseExport tally_labels(NETptr net);
void FstBaseExport print_tally(FILE * ofile);
TALLYptr FstBaseExport make_symbol_tally_export(int size);
TALLYptr FstBaseExport make_tally_sigma(NETptr net, 
		      int * sigma_tally_count,
		      id_type * max_sigma_id);
void FstBaseExport sort_this_tally(TALLYptr tally_sigma, id_type max_sigma_id,
			    int comp_fn (const void *a, const void *b));
void FstBaseExport print_this_tally(FILE * ofile,
		      TALLYptr tally_this, 
		      int this_tally_count,
		      id_type max_this_tally_id,
		      int comp_fn (const void *a, const void *b));

int FstBaseExport write_nets(NVptr nets, char *filename);
int FstBaseExport write_net(NETptr net, char *filename);

int FstBaseExport display_write_status_bar(int on_off);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_BIN_OUT */

