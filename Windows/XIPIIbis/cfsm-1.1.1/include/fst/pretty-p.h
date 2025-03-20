/* $Id: pretty-p.h,v 1.16 2002/12/13 09:28:18 privault Exp $ */
/* Copyright (c) 1990-93 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  PRETTY-P.H
 **
  **	 Todd Yampol, Xerox PARC, January 1990
 **  Lauri Karttunen, Xerox PARC, April 1991
 **  Lauri Karttunen, RXRC, December 1993
 **
 **  Ascii OUTPUT functions for simple FSM data types
 **
 ******************************************************/

#ifndef C_FSM_PRETTY_P

#define C_FSM_PRETTY_P

#include "fst_windefs.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "fst_windefs.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int FsmFstExport state_num_less_p(const void *s1, const void *s2);
NETptr FsmFstExport reorder_states (NETptr net, int sort_fn(const void *, const void *));

void FsmFstExport print_alphabet_s(ALPHABETptr alph, int rm, int indent, int sigma_p,
		      FILE *stream);
void FsmFstExport print_sigma_s(NETptr net, int rm, int indent, FILE *stream);
void print_uncompacted_net(NETptr net, int rm, FILE *stream);

void FsmFstExport print_time(long start, long end);

void FsmFstExport reset_out_buffer(void);
void FsmFstExport exact_buffer_out_s(int rm, int indent, FILE *stream);
void FsmFstExport buffer_out_s(int rm, int indent, FILE *stream);
void FsmFstExport fresh_line_s(int rm, int indent, FILE *stream);
void FsmFstExport string_array_out_s(int len, char *array[], int rm, int indent,
			FILE *stream);
void FsmFstExport string_out(char *str);
void FsmFstExport fat_string_out (FAT_STR fs);
void fat_string_out_esc(FAT_STR fs, char *esc);
void net_name_out(NETptr net);
void FsmFstExport char_out(char c);
void fat_char_out (FAT_STR fc);
void fat_char_out_esc (FAT_STR fc , char *esc_codes);
void symbol_out(FAT_STR name);
void FsmFstExport spaces_out(int n);
void FsmFstExport word_out_s(char *str, int rm, int indent, FILE *stream);
void byte_block_out(BYTE_BLOCKptr byte_block, int rm, int indent, FILE *stream);
void FsmFstExport int_out (long i);
void hex_out (long i);
void FsmFstExport label_out(id_type id, int escape_p);
void FsmFstExport one_side_out(id_type id, int side, int skip_epsilon_p, int escape_p);
void FsmFstExport alph_out_s(ALPHABETptr alph, int rm, int indent, FILE *stream);
void state_label_out(long int name, int final_p);
void object_out_s(OBJECTptr obj, int rm, int indent, FILE *stream);
void sequence_out_s(SEQUENCEptr s, int rm, int indent, FILE *stream);
void print_compacted_net(NETptr net, int rm, FILE *stream);
void print_state_label_s(STATEptr state, FILE *stream);
void print_labels_s(NETptr net, int rm, int indent, FILE *stream);
void print_flags_s(NETptr net, int rm, int indent, FILE *stream);

void print_object_s(OBJECTptr obj, int rm, int indent, FILE *stream);
void FsmFstExport print_alph_vector_s(ALPH_VECTORptr alph_vector, int rm, 
			 int indent, FILE *stream);
void print_props_s(NETptr net, int rm, int indent, FILE *stream);

#define buffer_out(x,y)		buffer_out_s(x,y,stdout)
#define fresh_line(x,y)		fresh_line_s(x,y,stdout)
#define word_out(x,y,z)		word_out_s(x,y,z,stdout)
#define string_array_out(w,x,y,z)   string_array_out_s(w,x,y,z,stdout)
#define sigma_out(x,y,z)	sigma_out_s(x,y,z,stdout)
#define alph_out(x,y,z)		alph_out_s(x,y,z,stdout)
#define print_alphabet(w,x,y,z) print_alphabet_s(w,x,y,z,stdout)
#define print_labels(x,y,z)	print_labels_s(x,y,z,stdout)
#define print_sigma(x,y,z)	print_sigma_s(x,y,z,stdout)
#define print_alph_vector(x,y,z)   print_alph_vector_s(x,y,z,stdout)
#define print_arc(x,y,z)	print_arc_s(x,y,z,stdout)
#define print_arcs(x,y,z)	print_arcs_s(x,y,z,stdout)
#define print_state(x)		print_state_s(x,stdout)
#define print_states(x)		print_states_s(x,stdout)
#define print_state_label(x)	print_state_label_s(x,stdout)
#define print_flags(x,y,z)	print_flags_s(x,y,z,stdout)
#define object_out(x,y,z)	object_out_s(x,y,z,stdout)
#define print_object(x,y,z)	print_object_s(x,y,z,stdout)
#define sequence_out(x,y,z)	sequence_out_s(x,y,z,stdout)
#define print_props(x,y,z)	print_props_s(x,y,z,stdout)
							
#define upper_out(X, Y, Z)	one_side_out((X), UPPER, (Y), (Z));
#define lower_out(X, Y, Z)	one_side_out((X), LOWER, (Y), (Z));

#endif /* SLIM_FST */

#define OUTPUT_BUFFER_SIZE 10000

extern char output_buffer[];
extern int FsmFstExport line_pos;
extern int FsmFstExport cur_pos;

extern int IY_UNICODE;

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_PRETTY_P */
