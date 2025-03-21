/* $Id: pretty-p.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
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

#include "fst/fst_windefs.h"
#include "fst/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "fst/fst_windefs.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int FstBaseExport state_num_less_p(const void *s1, const void *s2);
NETptr FstBaseExport reorder_states (NETptr net, int sort_fn(const void *, const void *));

void FstBaseExport print_alphabet_s(ALPHABETptr alph, int rm, int indent, int sigma_p,
		      FILE *stream);
void FstBaseExport print_sigma_s(NETptr net, int rm, int indent, FILE *stream);

void FstBaseExport print_time(long start, long end);

void FstBaseExport reset_out_buffer(void);
void FstBaseExport exact_buffer_out_s(int rm, int indent, FILE *stream);
void FstBaseExport buffer_out_s(int rm, int indent, FILE *stream);
void FstBaseExport fresh_line_s(int rm, int indent, FILE *stream);
void FstBaseExport string_array_out_s(int len, char *array[], int rm, int indent,
			FILE *stream);
void FstBaseExport string_out(char *str);
void FstBaseExport fat_string_out (FAT_STR fs);
void FstBaseExport fat_string_out_esc(FAT_STR fs, char *esc);
void FstBaseExport net_name_out(NETptr net);
void FstBaseExport char_out(char c);
void FstBaseExport fat_char_out (FAT_STR fc);
void FstBaseExport utf8_fat_string_out (FAT_STR fs);
void FstBaseExport symbol_out(FAT_STR name);
void FstBaseExport spaces_out(int n);
void FstBaseExport word_out_s(char *str, int rm, int indent, FILE *stream);
void FstBaseExport byte_block_out(BYTE_BLOCKptr byte_block, int rm, int indent, FILE *stream);
void FstBaseExport int_out (long i);
void FstBaseExport hex_out (long i);
void FstBaseExport label_out(id_type id, int escape_p);
void FstBaseExport one_side_out(id_type id, int side, int skip_epsilon_p, int escape_p);
void FstBaseExport alph_out_s(ALPHABETptr alph, int rm, int indent, FILE *stream);
void FstBaseExport state_label_out(long int name, int final_p);
void FstBaseExport object_out_s(OBJECTptr obj, int rm, int indent, FILE *stream);
void FstBaseExport sequence_out_s(SEQUENCEptr s, int rm, int indent, FILE *stream);
void FstBaseExport print_state_label_s(STATEptr state, FILE *stream);
void FstBaseExport print_labels_s(NETptr net, int rm, int indent, FILE *stream);
void FstBaseExport print_flags_s(NETptr net, int rm, int indent, FILE *stream);

void FstBaseExport print_object_s(OBJECTptr obj, int rm, int indent, FILE *stream);
void FstBaseExport print_alph_vector_s(ALPH_VECTORptr alph_vector, int rm, 
			 int indent, FILE *stream);
void FstBaseExport print_props_s(NETptr net, int rm, int indent, FILE *stream);

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
extern int FstBaseExport line_pos;
extern int FstBaseExport cur_pos;

extern int IY_UNICODE;

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_PRETTY_P */
