/* $Id: look.h,v 1.10 2002/07/17 22:26:02 lk Exp $ */
/* Copyright (c) 1990-94 Xerox Corporation. All rights reserved. */

/******************************************************
 **
 **  LOOK.H
 **
 **  Lauri Karttunen, RXRC, December 1993
 **
 **  LOOKUP and LOOKDOWN words in a transducer
 **
 ******************************************************/
 
#ifndef C_FSM_LOOK

#define C_FSM_LOOK

#include "fst_windefs.h"
#include "pars-tbl.h"


#ifdef __cplusplus
extern "C" {
#endif

extern int FsmFstExport OBEY_FLAGS_P;
extern id_type FsmFstExport OUTPUT_SEQ[WORD_STRING_SIZE + 1];
extern int FsmFstExport OUTPUT_SIDE;
extern int FsmFstExport RESULT_COUNT;
extern ALPHABETptr FsmFstExport SIGMA;
extern ALPHABETptr FsmFstExport FLAG_REGISTER;


    
void print_result(id_type *tail);
int FsmFstExport do_compact_look(NETptr net, void result_fn(id_type *));
int FsmFstExport look(id_type *path, STATEptr state, id_type *result, void result_fn(id_type *), int ignore_finality);
int FsmFstExport apply_to_string(char *word, PARSE_TBL parse_table, NETptr net, 
		    int input_side, int output_side, int obey_flags_p);
NETptr map_seq_to_net(id_type *input, NETptr net, int input_side,
                      int output_side, int minimize_p);
NETptr map_string_to_net(char *word, NETptr net, int input_side,
                         int output_side, int minimize_p);
int longest_prefix_seq(id_type *path, NETptr net, int input_side);
int FsmFstExport longest_prefix_string(char *word, NETptr net, int input_side);

int accepts_path (id_type *path, STATEptr state, int input_side, 
                  ALPHABETptr sigma);
int accepts_string (char *strString, NETptr pNet);

int LKP01_length_of_sequence (id_type *seq);
void LKP01_add_id_seq_to_net (id_type *seq, NETptr net);
void LKP01_add_id_seq_to_result (id_type *seq);
void LKP01_apply_seq_to_nv
  (id_type *inseq, id_type *outseq, NVptr nv, int net_idx, STATEptr state);
NETptr LKP01_apply_sequence_to_net_vector
  (NVptr nv, id_type *seq, int input_side, int minimize_p,
   void (*LKP_res_fn)(id_type *res));

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_LOOK */


