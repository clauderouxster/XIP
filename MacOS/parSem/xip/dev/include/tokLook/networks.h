/* $Id: networks.h,v 1.5 2007/02/13 13:10:34 roux Exp $ */
/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.60
   filename   : network.h
   Date       : 21/01/2002
   Purpose    : Encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/
/*************************************************************
 *
 *  FILE  "networks.h"
 *
 *      FINITE STATE NETWORK FUNCTIONS FOR
 *      LOOK-UP OF WORDS IN DICTIONARIES
 *
 *  Andre Kempe, RANK XEROX, Grenoble, France
 *  Tamas Gaal,  RANK XEROX, Grenoble, France - alternative lookup algorithm
 *  30/01/96 - ...
 *
 *************************************************************/

#ifndef _LookupNetworks_h
#define _LookupNetworks_h

#include "lookup_windefs.h"

#ifdef __cplusplus
extern "C" {
#endif

void open_network_files ();
void close_network_files ();

NETptr read_first_nets_only (FILE *file);
void FsmLookupExport get_networks (void*);
void FsmLookupExport get_strategies ();
void FsmLookupExport free_all_networks ();
void FsmLookupExport vectorize_all_networks ();
void FsmLookupExport make_range_map_for_every_net (int input_side, int lookup_algorithm);
void FsmLookupExport recode_eps_range_map (NETptr net, int input_side);

void write_buffer_to_output (char *bf_end);
int append_label_to_buffer(id_type id, char *bf_end,char **new_bf_end,int sep_p);
void write_result_net_to_output (STATEptr state, char *bf_end, int sep_p);
void write_id_seq_to_output (id_type *seq);

PARSE_TBL make_parse_table_from_nv (NVptr nv, int side);
void FsmLookupExport make_id_vector_or_parse_table (int side);
void FsmLookupExport select_match_tables (int input_side);

int parse_string_with_parse_table (char *string, id_type *path);
void parse_string_with_id_table (char *string, id_type *path);
int FsmLookupExport convert_string_to_id_seq (char *string, id_type *path);

#define CHK_chknet(N)  *((NETptr *)(&NET_client_cell(N)))
#define CHK_start_state(N)  \
  (CHK_chknet(N) ? NET_start_state (CHK_chknet(N)) : NULL)

#define CHK_num_arcs_in_state(S)  *((long *)(&STATE_client_cell(S)))

long CHK_vectorize_check_net (NETptr net);
NETptr CHK_make_check_net (NETptr fst, int fst_idx, int input_side);
void FsmLookupExport CHK_make_all_check_nets (NVptr nv);
void FsmLookupExport CHK_free_all_check_nets (NVptr nv);
STATEptr CHK_map_id_on_check_net (id_type id, STATEptr state, NETptr net);

int length_of_sequence (id_type *seq);
void add_id_seq_to_net (id_type *seq, NETptr net);
void add_id_seq_to_result (id_type *seq,void* parseur);

void apply_id_seq_to_nv_checked
  (id_type *inseq, id_type *outseq, NVptr nv, int net_idx,
   STATEptr state, STATEptr chk_state,void*);
void apply_id_seq_to_nv_vstate_checked
  (id_type *inseq, id_type *outseq, NVptr nv, int net_idx,
   STATEptr state, STATEptr chk_state,void*);
void apply_id_seq_to_nv_nstate_checked
  (id_type *inseq, id_type *outseq, NVptr nv, int net_idx,
   STATEptr state, STATEptr chk_state,void*);

void FsmLookupExport apply_id_seq_to_nv_simple
  (id_type *inseq, id_type *outseq, NVptr nv, int net_idx, STATEptr state,void*);
void apply_id_seq_to_nv_vstate_simple
  (id_type *inseq, id_type *outseq, NVptr nv, int net_idx, STATEptr state,void*);
void apply_id_seq_to_nv_nstate_simple
  (id_type *inseq, id_type *outseq, NVptr nv, int net_idx, STATEptr state,void*);

NETptr apply_string_to_nv (NVptr nv, int input_side, int minimize_p);
void FsmLookupExport lookup_input_and_write_result_andre (void* parseur);

/* ----------- Alternative lookup algorithm -- Tamas Gaal */


extern HEAPptr FsmLookupExport LK_CONFIG_HEAP ;
extern HEAPptr FsmLookupExport LK_OUT_CELL_HEAP ;

void FsmLookupExport init_alt_alg_memory();
void FsmLookupExport free_alt_alg_memory();
void FsmLookupExport alt_alg_set_buffer_and_function(int lk_xelda_buffering, 
                                     id_type *lk_xelda_buffer, 
                                     void (* LK_TX_Write_Buf) (id_type *));
void FsmLookupExport allocate_alt_alg_memory(int lk_xelda_buffering, 
			     id_type *lk_xelda_buffer, 
			     void (* LK_TX_Write_Buf) (id_type *start));
void FsmLookupExport reclaim_alt_alg_memory(void);

/* Strategy control */
typedef struct Str_Control * STR_CONTROLptr;

extern STR_CONTROLptr FsmLookupExport STRATS_CONTROL ; 
extern unsigned int FsmLookupExport STR_NO;
void FsmLookupExport init_strats_contr ();
STR_CONTROLptr FsmLookupExport init_strats_contr_memory (int num_strategies);
void FsmLookupExport init_strat_control_strategy (int strategy, NVptr nv, int max_len_inp_line);
void FsmLookupExport free_strats_control( );
void FsmLookupExport reset_strat_control( int16 s, NVptr nv );

/* unsigned int FsmLookupExport set_nondet_limit (unsigned int length_of_longest_allowed_deterministic_chain); */

void FsmLookupExport lookup_input_and_write_result_tamas (void* parseur);
void FsmLookupExport lookup_input_and_write_result_word_to_number();
int FsmLookupExport apply_input_seq_to_nv_tamas ( NVptr nv,void* parseur) ;


/* ----------- Alternative compose-apply algorithm of Lauri Karttunen */

/* Initialization step required for the compose-apply algorithm: */
APPLYptr FsmLookupExport *init_compose_apply_for_lookup();
    
/* Top-level function for looking up a word with the Depth-First
   compose-apply algorithm:
   It supports standard nets, bimachines, labelSet reduced and Karttunen
   compacted networks; it does NOT support vectorized nets. */
void FsmLookupExport lookup_input_and_write_result_compose_apply();
/* ----------- */
    
lower_sigma_netptr FsmLookupExport make_lower_sigmas_all_networks ();
void free_lower_sigmas_nets (lower_sigma_netptr nets, int n);
void FsmLookupExport free_lower_sigmas_all_networks ();

#define LK_OUT_CHAIN_DEPTH_DEFAULT 8
extern FsmLookupExport unsigned int LK_OUT_CHAIN_DEPTH;

#ifdef __cplusplus
}
#endif

#endif /* _LookupNetworks_h */
