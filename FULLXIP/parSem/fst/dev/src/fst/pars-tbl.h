/* $Id: pars-tbl.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1993 by the Xerox Corporation.  All rights reserved */


/******************************************************
 **
 **  PARS-TBL.H
 **
 **  Lauri Karttunen, RXRC 1993
 **
 **  Functions relocated from ASCII-IO.H
 **
 ******************************************************/

#ifndef C_FSM_PARS_TBL

#define C_FSM_PARS_TBL

#include "fst/fst_windefs.h"
#include "fst/types.h"

#ifdef __cplusplus
extern "C" {
#endif
 
#define WORD_STRING_SIZE 10000 /* Long enough? Why do we limit this? -- PT */
#define myisspace(X) ((X) == ' ' || (X) == '\t' || (X) == '\n')

#ifndef _WIN32
#ifndef __svr4__
#ifndef MAC_OS
#include <unistd.h>
#ifndef fpos_t
#define fpos_t off_t
#endif
#endif
#endif
#endif /* YM, 2/9/96 */

/*  moved to types.h   98-01-04 L.K.

enum zero_to_epsilon_p {DONT_ZERO_TO_EPS, DO_ZERO_TO_EPS};
enum ignore_whitespace_p {DONT_IGNORE_WHITES, DO_IGNORE_WHITES};
*/

    
int FstBaseExport verify_bimachine_flags (NETptr left, NETptr right, int side_p);
PARSE_TBL create_bimachine_parse_table (NETptr left, NETptr right, int side_p);
PARSE_TBL get_bimachine_parse_table (NETptr left, NETptr right, int side_p);
PARSE_TBL get_tokenizer_parse_table (NETptr left, ALPHABETptr lower_sig);
PARSE_TBL FstBaseExport make_parse_table(NETptr net, ALPHABETptr alphp, int side);
PARSE_TBL FstBaseExport get_parse_table(NETptr net, int side);
void FstBaseExport add_symbol_to_parse_table(PARSE_TBL parse_table, int id);
PARSE_TBL FstBaseExport create_parse_table(NETptr net, ALPHABETptr alphp, int side);
CH_NODEptr FstBaseExport make_ch_node(id_type id);
void FstBaseExport update_match_tables(void);

void FstBaseExport set_range(RANGEptr range, id_type id, int count);
RANGEptr FstBaseExport make_range_map(NETptr net, int side);
MATCH_TABLEptr FstBaseExport get_match_table(NETptr net, int side);

int FstBaseExport parse_string (char *word, PARSE_TBL parse_table, int zero_to_eps_p,
		  int ignore_white_space_p, int escape_p, int report_error_p);
int FstBaseExport utf8_parse_string (char *word, PARSE_TBL parse_table, int zero_to_eps_p,
		  int ignore_white_space_p, int escape_p, int report_error_p);
void report_parse_error(char *word, int pos);
int FstBaseExport symbol_from_string (id_type *id, char *input, char **remain,
			PARSE_TBL parse_table, int zero_to_eps_p,
			int ignore_white_space_p, int escape_p);
int FstBaseExport utf8_symbol_from_string (id_type *id, char *input, char **remain,
			PARSE_TBL parse_table, int zero_to_eps_p,
			int ignore_white_space_p, int escape_p);
int FstBaseExport pair_symbol_from_string (id_type *id, char *input, char **remain,
                             PARSE_TBL upper_parse_table, PARSE_TBL lower_parse_table,
                             int zero_to_eps_p, int ignore_white_space_p, int escape_p);
int symbol_from_stream (id_type *id, FILE *stream, PARSE_TBL parse_table,
	int zero_to_eps_p, int ignore_white_space_p, int escape_p);
	
int FstBaseExport next_symbol (id_type *id, FILE *stream, PARSE_TBL parse_table,
	int zero_to_eps_p, int ignore_white_space_p, int escape_p);

id_type upper_side_id(id_type id);
id_type lower_side_id(id_type id);

ALPHABETptr FstBaseExport make_id_vect ();
ALPHABETptr FstBaseExport make_outchar_vect (ALPHABETptr id_vector);

extern id_type FstBaseExport INPUT_SEQ[WORD_STRING_SIZE +1];

extern id_type FstBaseExport *UPPER_MATCH; /* UPPER_MATCH[i] the upper_id of the i'th label */
extern id_type FstBaseExport *LOWER_MATCH; /* LOWER_MATCH[i] the lower_id of the i'th label */
extern id_type FstBaseExport *MATCH_TABLE; /* Alternates between UPPER_MATCH and LOWER_MATCH */

#ifdef DEBUG
void TEST_print_range_map (NETptr nn, RANGEptr rm);
#endif

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_PARS_TBL */
