/* $Id: text-io.h,v 1.4 2003/01/10 08:26:17 jmcoursi Exp $ */
/* Copyright (c) 1991-94 Xerox Corporation. All rights reserved. */

/********************************************************************************
 **
 **  text-io.h
 **
 **	 Todd Yampol, Xerox PARC, September 1991
 **
 **  Function prototypes and global variables for text-io.c
 **
 ********************************************************************************/

#ifndef C_FSM_TEXT_IO

#define C_FSM_TEXT_IO

#ifdef __cplusplus
extern "C" {
#endif

void FsmFstExport add_seq_to_net (id_type *seq, NETptr net, STATEptr final_state,
                                  STATEptr **start_destinations, int *sd_len, 
                                  int limit);

NETptr FsmFstExport cleanup_seq_list_fsm (NETptr net,
                                          STATEptr *start_destinations,
                                          int sd_len,
                                          STATEptr final_state);

NETptr seqs_to_net(id_type *(*seq_form)(void));

void network_to_text(NETptr net, FILE *stream, void print_path(void)); /* YM, 2/9/96 */

NETptr word_file_to_fsm(FILE *stream);
NETptr utf8_word_file_to_fsm(FILE *stream);
void fsm_to_word_file(NETptr net, FILE *stream);
void utf8_fsm_to_word_file(NETptr net, FILE *stream);

NETptr network_from_text_file(FILE *stream);
void network_to_text_file(NETptr net, FILE *stream);
void utf8_network_to_text_file(NETptr net, FILE *stream);

#define QUOTED_AS_SINGLE "0?"
#define QUOTED_ANYWHERE "%! "

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_TEXT_IO */

