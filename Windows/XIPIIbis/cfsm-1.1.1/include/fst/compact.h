/* $Id: compact.h,v 1.9 2003/01/21 17:54:55 privault Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  COMPACT.H
 **
 **  Lauri Karttunen, Xerox PARC, October 1992
 **
 **  Conversion between standard and compact network formats.
 **
 ******************************************************/

#ifndef C_FSM_COMPACT
#define C_FSM_COMPACT

#ifdef __cplusplus
extern "C" {
#endif
 
void init_cs_mem(void);
void free_cs(CSptr cs);
void free_cs_mem(void);
void clear_cs_mem(void);
/* Sorts arcs by increasing lower label values. */
void sort_arcs_lower_label(NETptr net);
unsigned long int collect_state_statistics(NETptr net, int speed_option,
					   int *addr_size);
unsigned char FsmFstExport *decode_var_label(unsigned char *loc);
unsigned char *two_byte_label(unsigned char *loc);

unsigned char *decode_var_addr(unsigned char *loc);
unsigned char *skip_four_bytes(unsigned char *loc);
unsigned char *skip_var_data(unsigned char *loc);
unsigned char *four_byte_data(STATEptr state, unsigned char *loc);
unsigned char *decode_client_data(STATEptr state);
unsigned char *one_byte_addr(unsigned char *loc);
unsigned char *two_byte_addr(unsigned char *loc);
unsigned char *three_byte_addr(unsigned char *loc);
unsigned char *four_byte_addr(unsigned char *loc);
unsigned char *four_byte_absolute_addr(unsigned char *loc);

void abs_to_rel_block(NETptr net);

void FsmFstExport set_decoding_fns(char first_byte);
CSptr get_dest_state(void);
NETptr compact_net(NETptr net, int speed_option);
NETptr uncompact_net(NETptr net);

#define include_next_mask 128	/* Bit 7 */

#define more_to_come_p(X)		((X) & include_next_mask)

#define final_dest_bit 6	/* Special bits in the first label byte */
#define final_dest_mask 64	/* Bit 6 */

#define last_bit 5
#define last_mask 32	/* Bit 5 */

#define first_label_size 5	/* First five bits of the label start at 4 */
#define first_label_mask 31	/* 11111 in binary */
	
#define first_addr_size 7 	/* Address bits start at 6 */
#define first_addr_mask 127	/* 1111111 in binary */

#define next_data_size 7	/* Additional data in chunks of seven bits. */
#define next_data_mask 127	/* 1111111 in binary */

/* Tests if some net arcs are labeled with ALTCHAIN symbols:
   helps to know if a net went previously through 
   do_optimize_arcs(), but does not garantee that it is
   optimized.*/
int FsmFstExport possibly_already_optimized_p(NETptr net);
    
NETptr optimize_arc_chains (NETptr net, ALPHABETptr dont_reorder_labels, 
			    int dont_reorder_mark, int ignore_state_mark,
			    int reorder_p);
void FsmFstExport do_optimize_arcs(NETptr net);
void do_unoptimize_arcs(NETptr net); 

void sort_arcs_by_arc_indegree(NETptr net, ALPHABETptr initialchars, 
                               ALPHABETptr finalchars);
#ifndef SMALL_FSM
ARCptr make_big_arc (id_type label, STATEptr dest, NETptr net); 
void make_arcs_big (NETptr net); /* transforms ARCs into BIG_ARCs */
void make_arcs_small (NETptr net); /* transforms BIG_ARCs into ARCs */
#endif /* SMALL_FSM */

/* Functions for (UN)SHARE_ARCS */

#ifndef SMALL_FSM

void SHARE_warning (char *fct_name, char *message);
void SHARE_clear_deletemarks (NETptr net);
int SHARE_delete_altchain_arcs (NETptr net, int *altchain_remains);
void SHARE_delete_marked_states (NETptr net);
NETptr FsmFstExport SHARE_make_shared_arclists (NETptr net, int copy_p);

/*
void SHARE_clear_counts_and_marks (NETptr net);
void SHARE_count_shared_arcs (NETptr net);
void SHARE_mark_shared_arcs (NETptr net);
void SHARE_clear_arc_sources (NETptr net);
void SHARE_reinsert_altchain_arcs (NETptr net);
*/
NETptr SHARE_make_unshared_arclists (NETptr net, int copy_p);

#endif /* SMALL_FSM */

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_COMPACT */



