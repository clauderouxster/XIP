/* $Id: net-io.h,v 1.16 2002/04/24 15:44:00 jmcoursi Exp $ */
/* Copyright (c) 1990-93 by the Xerox Corporation.  All rights reserved */


/******************************************************
 **
 **  NET-IO.H
 **
 **	 Todd Yampol, Xerox PARC, January 1990
 **  Lauri Karttunen, RXRC, December 1993
 **  
 **  Display functions for networks
 **
 **  with macros for function calls that use STDOUT
 **  as the output stream without specifying it
 **  explicitly.
 **
 ******************************************************/

#ifndef C_FSM_NET_IO

#define C_FSM_NET_IO

#ifdef __cplusplus
extern "C" {
#endif

int FsmFstExport print_path_fn(char *buf, FILE *stream);
int print_paths(NETptr net, int max_visits, int max_paths, int side,
		char *separator, unsigned long *word_count, FILE *stream,
		int escape_p, int obey_flags_p, int show_flags_p,
		int print_fn(char *buf, FILE *stream));
void FsmFstExport print_nth_word(NETptr net, int n, int max_visits, int rm, int indent, 
                    int space_p, FILE *stream);
void print_words(NETptr net, int rm, int indent, int max_visits,
		   int count_p, int obey_flags_p, FILE *stream);
void print_compact_words(NETptr net, int rm, int indent, int max_visits, 
			 int count_p, FILE *stream);
void FsmFstExport print_net_size(NETptr net, FILE *stream);
void print_compacted_net(NETptr net, int rm, FILE *stream);

void print_states_s(NETptr net, int rm, FILE *stream);
void print_uncompacted_net(NETptr net, int rm, FILE *stream);
void FsmFstExport print_net_s(NETptr net, int rm, FILE *stream);
void FsmFstExport print_net(NETptr net, FILE *stream);

void FsmFstExport random_words_s(NETptr net, int num, int side, int obey_flags_p,
		    int show_flags_p, FILE *stream, char * separator,
		    int escape_p, int print_fn(char *buf, FILE *stream));

/* #define random_words(X, Y)  random_words_s(X, Y, LOWER, TRUE, stdout) */

char *number_to_word(unsigned long num, NETptr net, int side, int escape_p);
int default_symbol_id_cmp_fn(id_type id1, id_type id2);
long int FsmFstExport *path_to_numbers(id_type *path, NETptr net, id_type *match_table);
long int FsmFstExport *path_to_numbers_cmp(id_type *path, NETptr net, id_type* match_table,
                              int symbol_id_cmp_fn(id_type, id_type));
long int FsmFstExport *word_to_numbers(char *word, NETptr net, int side);
long int *path_to_locations(id_type *path, NETptr index_net, int side);
long int *word_to_locations(char *word, NETptr index_net, int side);
int FsmFstExport path_index_count(void);

void inspect_net (NETptr net, int rm, int output_side);

#define MAX_TALLY_ID_LENGTH  256

TALLYptr tally_paths(NETptr net, int max_visits, int max_paths, int side,
		     unsigned long * word_count,
		     int * tally_count,
		     id_type * max_tally_id,
		     int escape_p, int obey_flags_p);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_NET_IO */
