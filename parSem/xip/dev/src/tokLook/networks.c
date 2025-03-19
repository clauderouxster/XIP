/* $Id: networks.c,v 1.66 2017/10/16 08:53:29 roux Exp $ */

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
filename   : network.c
Date       : 10/01/1999
Purpose    : FINITE STATE NETWORK FUNCTIONS FOR LOOK-UP OF WORDS IN DICTIONARIES encapsulation
Programmer : Claude ROUX
Reviewer   : 
*/

/************************************************************
*
*  FILE  "networks.c"
*
*      FINITE STATE NETWORK FUNCTIONS FOR
*      LOOK-UP OF WORDS IN DICTIONARIES
*
*  Andre Kempe, RANK XEROX, Grenoble, France - basic lookup algorithm
*  Tamas Gaal,  RANK XEROX, Grenoble, France - alternative lookup algorithm
*  30/01/96 - ...
*
*************************************************************/


#include <stdio.h>
/*#include <stdarg.h> */
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "fst/fsm-version.h"
#include "fst/types.h"
#include "fst/struct.h"
#include "fst/calculus.h"
#include "fst/vectorize.h"
#include "fst/modify.h"
#include "fst/heap.h"
#include "fst/flags.h"
#include "fst/bin-in.h"
#include "fst/look.h"
#include "fst/label.h"
#include "fst/twol-net.h"
#include "fst/pars-tbl.h"
#include "fst/fsm_io.h"
#include "fst/pretty-p.h"
#include "fst/prompt.h"
#include "fst/compact.h"
/* #include "fat-str.h" */


#include "lktypes.h"
#include "ems.h"
#include "lookup.h"
#include "networks.h"
#include "readscript.h"
#include "calculus.h"

NETptr net_minus(NETptr net1, NETptr net2) {
	return minus(net1, net2);
}

NETptr net_negate(NETptr net1) {
	return negate(net1);
}

char monlocalbuf[2040];

/* GLOBAL Variables */
int READING_SCRIPT;
int READING_INPUT;
long CUR_LINE_NUM;
struct _LkTmpFiles TEMP_FILES;
struct _LkTmpData TEMP_DATA;
struct _LkNetFile NET_FILES;
NVptr LEXNETS;
NVVptr STRATEGIES;
APPLYptr *STRATEGIES_APPLY_CONTEXT;
NETptr LOOKUP_RESULT;
char *LOOKUP_INPUT_WORD;
char *LOOKUP_OUTPUT_BUFFER;
id_type *LOOKUP_ID_SEQUENCE;
long ID_SEQUENCE_LENGTH;

/* id_type *NORM_ID_SEQUENCE; */
ALPHABETptr LOOKUP_ID_TABLE;

PARSE_TBL LOOKUP_PARSE_TABLE;

id_type *LOOKUP_INPUT_MATCH;
id_type *LOOKUP_OUTPUT_MATCH;

FILE *LOOKUP_OUTPUT_FILE;

void (*LOOKUP_print_result_fn)(id_type *,void * );

struct _LkFlags LOOKUP_FLAGS;
long *SUCCESS_COUNT;
time_t LOOKUP_SECONDS;
long NUM_WORDS_TOTAL;
long NUM_WORDS_NOT_FOUND;

struct lower_sigs_net * LOWER_SIGMAS_LEXNETS;
struct TAG_HASH_TYPE SYMBOL;

#ifdef DEBUG
#define cell_alloc(h) FstDebug_cell_alloc(h, __FILE__, __LINE__)
#define free_cell(cell, heap) FstDebug_free_cell(cell, heap, __FILE__, __LINE__)
#endif

#if (FSM_VERSION < 5)
/* not available in FSM 4 */
#define NET_lower_seq(x) 0
#endif

void insert_token_char(char c,void* parseur,char final);
void insert_token_id(id_type id,void* parseur);
void insert_token(char* buffer,void* parseur);
void add_token(char* buffer,void* parseur);
extern char buffer_erreur[1000];
void message_erreur(void* parseur,char* message);


/*---------------------------- EDIT DISTANCE ---------------------------------*/
char EditDistanceAction(void* parseur,char action);
void EditDistancePop(void* parseur);
char AddId(void* parseur,short id);
void RemoveId(void* parseur);
char CheckEditDistance(void* parseur);
void insert_all_id_sequence(void* parseur,char);
char CheckEditDistanceAction(void* parseur,char action);
void AddLastEditDistanceAction(void* parseur,char ret,short* pile);
char RemoveLastEditDistanceAction(void* parseur);
void AfficheContenu(void* parseur,unsigned short* inseq,char* buff);

void AddEditDistanceAction(void* parseur,char action);
char RemoveEditDistanceAction(void* parseur,char action);

short* CleanEditDistance(void* parseur);

#define action_change	2
#define action_delete	4
#define action_insert	8
#define action_switch	16
#define action_last		32
#define action_prefix	64
#define action_prefix_detected	128
/*---------------------------- EDIT DISTANCE ---------------------------------*/

static int symbol_length (id_type id)
{
	/* In UTF8-mode a single symbol may have a multicharacter
	representation in the input. */

	int len;
	FAT_STR name;

	if (id == EPSILON || id == ALTCHAIN_SYMBOL || id == OTHER)
		return (0);
	else
		name = id_to_atom_name(id);

	len = fat_strlen(name);

	if (len == 1 && IS_UTF8_MODE)
	{
		if (CHARSET(name) != 0 || CHARNUM(name) > 127)
			len = 2;
	}

	return(len);
}


/*======================================================================
new stuff - 04.96 Tamas Gaal - Alternative lookup algorithm functions  
- config-s (see also the tokenizer - tokenize - )
- on-the-fly analysis according to strategies
- output by a simplified tree
======================================================================*/

static char rcs_id_lookup[] = "$Id: networks.c,v 1.66 2017/10/16 08:53:29 roux Exp $";
/* --- Debug help ----------------------------------------------------*/

#ifdef DEBUG
#define SEQ_MAX_PRINT_STATES 50

void debug_print_arc(ARCptr narc, int arc_number)
{ 
	if (!narc)  
	{
		fprintf (stderr,"\t arc %d=%d=0x%x.", arc_number, (int) narc, (int) narc);  
		fflush(stderr); 
		return;
	}

	fprintf (stderr,"\t arc %d=%d=0x%x. Flag=%d, Vm=%d, label=%d=", arc_number, (int) narc, (int) narc,
		ARC_userflag2(narc), ARC_visit_mark(narc), ARC_label(narc));
	print_label(ARC_label(narc), stderr, DO_ESCAPE);
	fprintf (stderr,"  destination=%d=0x%x\n",  (int) ARC_destination(narc), (int) ARC_destination(narc)); 
	fflush(stderr); 
}

void debug_print_state(STATEptr nstate, int state_number, int with_arcs)
{
	ARCptr narc;
	int arc_i;
	if (! nstate)  
	{
		fprintf (stderr, " %d  state =%d=0x%x.", state_number, (int) nstate, (int) nstate); 
		fflush(stderr); return;
	}

	if (STATE_final(nstate))
		fprintf (stderr, " %d Fstate", state_number);
	else
		fprintf (stderr, " %d  state", state_number);
	fprintf (stderr," =%d=0x%x. Flag=%d, Vm=%d, Client_cell=%d=0x%x\n",
		(int) nstate, (int) nstate,
		STATE_userflag2(nstate),
		STATE_visit_mark(nstate),
		(int) STATE_client_cell(nstate), (int) STATE_client_cell(nstate));
	if (with_arcs)
		for (narc = STATE_arc_set(nstate), arc_i=1 ; narc ; narc = next_arc(narc), arc_i++)
		{
			debug_print_arc(narc, arc_i);
		}
		fflush(stderr);
}

void debug_print_net(NETptr net, int with_arcs)
{
	int i;
	STATEptr nstate;

	if (!net)
	{ 
		fprintf (stderr, "\n\
						 SEQ Debug: No net to print.\n");
		return;
	}

	if  (NET_num_states(net) > SEQ_MAX_PRINT_STATES)
	{
		fprintf (stderr,"Number of states:  %ld",  NET_num_states(net));
		if (! yes_or_no(" Debug print net: print all these states ?",'n'))
			return;
	}

	fprintf (stderr,"\n*** Net Start state==%d=0x%x.",  (int) NET_start_state(net),
		(int) NET_start_state(net));

	if (NET_start_state(net))
		fprintf (stderr," Flag=%d.\n", STATE_userflag2( NET_start_state(net)));
	else
		fprintf (stderr,"\n");

	for (nstate = NET_states(net), i=1 ; nstate ; nstate = next_state(nstate), i++)
		debug_print_state(nstate, i, with_arcs);
	fflush(stderr);
}
#endif /* DEBUG */

#ifdef LKDEBUG_ADD
inline
	void debug_printinfo_branch_out(char *where, id_type fst_id, id_type up, id_type to, int indent)
{
	register int i;		      
	for (i=0; i < indent; i++)
		fprintf(stdout, " ");
	/* ** Test, debug */
	fprintf(stdout, where);
	/*   fprintf(stdout, "p-t-t Arc label-fast-eps: "); */
	print_label(fst_id, stdout, DO_ESCAPE);
	fprintf(stdout, " up: ");
	print_label(up, stdout, DO_ESCAPE);
	fprintf(stdout, " output_cell->id:    ");
	print_label(to, stdout, DO_ESCAPE);
	fprintf(stdout, "\n");
	fflush(stdout);
	return;
}
inline
	void debug_printinfo_branch(char *where, id_type fst_id, id_type up, int indent)
{
	register int i;		      
	for (i=0; i < indent; i++)
		fprintf(stdout, " ");
	/* ** Test, debug */
	fprintf(stdout, where);
	print_label(fst_id, stdout, DO_ESCAPE);
	fprintf(stdout, " up: ");
	print_label(up, stdout, DO_ESCAPE);
	fprintf(stdout, "\n");
	fflush(stdout);
	return;
}
#endif

typedef id_type OUT_ID, *OUT_ID_VECT;

/* OUT_CELL is to build output trees. It is used when building
alternate predictions depending on the input, the tokenizing
transducer used, and the possible previous set of predictions.
*/

typedef struct OUT_CELL {
	OUT_ID outid;
	uint16 refcount; /* new 99-4-30 previously unsigned int */
	unsigned int symbol_cnt; /* New; signals actual related real input symbol */
	struct OUT_CELL  *link;
} OUT_CELLtype, *OUT_CELLptr;

#define OC_id(X)           (X)->outid
#define OC_l(X)            (X)->link
#define OC_sc(X)           (X)->symbol_cnt

/* In CONFIGURATION : Applied population control: cascade_ancestor and
symb_cnt_ancestor are unique mark-s allocated at sequencing points of a token. A
next fst in the chain inherits cascade_ancestor from its antecedent fst in the
cascade and symb_cnt_ancestor from its antecedent at the previous input symbol in
the same fst (the input symbol is what its cascading antecedent provides). It gets
a unique own_mark then passes it on as cascade_ancestor to the upper composing fst
(if any) and symb_cnt_ancestor to the next config (at the next symbol) and so on.

At the next symbol (and at all further ones) the *_ancestor marks will have to
match complying to common origin in both directions (cascade-wise and
symbol-wise). So there is vertical and horizontal inheritance. At a given
normalizer fst, the cascade_ancestor/symb_cnt_ancestor pairs are given to possibly
multiple offsprings (config-s) so that they bear the same ancestor mark-s. When
passing one config upwards (to the next fst in the chain still remaining at the
actual symbol) only upper-fst-config-s with this identical parent will possibly
multiply.  The number of offsprings can be much bigger at each step but only
reproductive branches will survive a next (real) symbol.
*/

typedef struct CONFIGURATION {
	struct STATE *fst_state;
	unsigned int symbol_cnt; /* signals actual related symbol order */
	unsigned int cascade_ancestor; /* mark of the ancestor in a prev fst,
								   same symbol */
	unsigned int symb_cnt_ancestor; /* mark of the ancestor at the previous
									symbol at the same fst */
	unsigned int own_mark;  /* self mark to pass on to offsprings */
	union {
		struct OUT_CELL  *cell;
		struct STATE *state;
	} output;
	ALPHABETptr flag_diacritic_register;
	struct CONFIGURATION *alt_config;
} CONFIG, *CONFIGptr;

#define CONFIG_fst_state(X)	(X)->fst_state
#define CONFIG_output_cell(X)	(X)->output.cell
#define CONFIG_output_state(X)	(X)->output.state
#define CONFIG_next(X)		(X)->alt_config
#define CONFIG_count(X)         (X)->symbol_cnt
#define CONFIG_c_papa(X)        (X)->cascade_ancestor
#define CONFIG_s_papa(X)        (X)->symb_cnt_ancestor
#define CONFIG_own(X)           (X)->own_mark
#define CONFIG_flag_d_reg(X)    (X)->flag_diacritic_register
/* redefinition of mark fields for tail token handling */
#define CONFIG_tail_c_grandpa(X)   (X)->own_mark
#define CONFIG_tail_own(X)         (X)->symb_cnt_ancestor

HEAPptr LK_CONFIG_HEAP = NULL;
HEAPptr LK_OUT_CELL_HEAP  = NULL;

id_type *LK_XELDA_BUFFER = NULL;
id_type *lk_xelda_b_p = NULL;
void (*lk_xelda_print) (id_type *start);
int XELDA_STYLE_PRINT = FALSE;

/* ** TUNING! Tune heap sizes ! */

void lk_xelda_test_print(id_type *start)
{
	id_type *bp;
	assert (LK_XELDA_BUFFER && lk_xelda_b_p && (!((lk_xelda_b_p - (( id_type *) LK_XELDA_BUFFER)) < 0)));

	for (bp = start; *bp != ID_EOS; bp++)
		print_label(*bp, LOOKUP_OUTPUT_FILE, DONT_ESCAPE);
	fputc('\n',LOOKUP_OUTPUT_FILE);  /* output token separator */
}

void init_alt_alg_memory()
{
	/* 2400 */
	LK_CONFIG_HEAP = init_heap(sizeof(CONFIG),  16384/sizeof(CONFIG),
		"Lookup configs");

	/*  2400/sizeof(OUT_CELLtype) */
	LK_OUT_CELL_HEAP = init_heap(sizeof(OUT_CELLtype), 16384/sizeof(OUT_CELLtype),
		"Lookup out_cells");
}

void alt_alg_set_buffer_and_function(int lk_xelda_buffering, 
	id_type *lk_xelda_buffer, 
	void (* LK_TX_Write_Buf) (id_type *)) {

		if (lk_xelda_buffering) {

			XELDA_STYLE_PRINT = TRUE;
			if (lk_xelda_buffer)
				LK_XELDA_BUFFER = lk_xelda_buffer;
			else
				LK_XELDA_BUFFER = (id_type *) LOOKUP_OUTPUT_BUFFER;
			if (LK_TX_Write_Buf)
				lk_xelda_print = LK_TX_Write_Buf;
			else
				lk_xelda_print = lk_xelda_test_print;
		}
		else
			lk_xelda_print = NULL;    
}

void allocate_alt_alg_memory(int lk_xelda_buffering, 
	id_type *lk_xelda_buffer, 
	void (* LK_TX_Write_Buf) (id_type *))
{
	init_alt_alg_memory();
	alt_alg_set_buffer_and_function(lk_xelda_buffering, lk_xelda_buffer, LK_TX_Write_Buf);
}

void free_strats_control( );

void free_alt_alg_memory(void)
{
	free_heap(LK_CONFIG_HEAP);
	LK_CONFIG_HEAP = NULL;

	free_heap(LK_OUT_CELL_HEAP);
	LK_OUT_CELL_HEAP = NULL;
}

void reclaim_alt_alg_memory(void)
{
	free_alt_alg_memory();
	free_strats_control( ); 
}

static void free_out_cell(OUT_CELLptr out)
{
	if (out)
#ifdef LKDEBUG_HEAP
		/* IF DEBUGGING set BREAKPOINT in heap.c at a marked line */
		free_cell_debug((void *) out, LK_OUT_CELL_HEAP);
#else
		free_cell((void *) out, LK_OUT_CELL_HEAP);
#endif
}

id_type *LOOKUP_ID_SEQUENCEptr; /* pointer to the actual id  in the input
								label_id vector*/

/* -----------------------------------------------------------------------*/
/* Control structures and variables for a token line                      */

typedef struct Net_Control{
	NETptr net;
	ALPHABETptr lsigma; /* lower sigma */
	RANGEptr range_of_OTHER; /** New 2000-2-3: range for OTHER for input OTHER-s */
	ALPHABETptr * other_exception_map; /** New 2000-2-10: other sigma constraint
									   alphabets */
	CONFIGptr prev_symbol_result; /* temporary result at a "sequence
								  point" in the respective fst, that 
								  is, when having finished at an input
								  symbol for all the fst-s in the
								  cascade. */
	CONFIGptr temp_result; /* temporary and intermediate result during
						   searching for possible matches in
						   the respective fst in the cascade */
	id_type *norm_id_sequence; /* id-s from previous cascade fst */
} NET_CONTROL, * NET_CONTROLptr;

NET_CONTROLptr STRAT_CONTROL; /* strategy control vector for this actual strategy. */
unsigned int NET_NO;          /* number of fst-s in this actual strategy. */
unsigned int SYMBOL_COUNT;    /* for this actual token */
unsigned int ANCESTOR_MARK;   /* for this actual token */

void* get_strat_control() {
	return (void*)STRATS_CONTROL;
}

void set_strat_control(void* sc,int nb) {
	STRATS_CONTROL=(STR_CONTROLptr)sc;
	STR_NO = nb;
}

int get_str_no() {
	return STR_NO;
}

struct Str_Control{
	int netno; 
	NVptr nv;
	NET_CONTROLptr nets_ctr;
} STR_CONTROL;

/* -----------------------------------------------------------------------*/

#ifdef DEBUG
/* Debugging help to print a chain of config-s with all its strings. */

void debug_print_out_cells(OUT_CELLptr oc)
{
	if (!oc)
		return;
	debug_print_out_cells(OC_l(oc));
	print_label(OC_id(oc), stderr,  DO_ESCAPE);
}

void debug_print_out_cells_with_cnt(OUT_CELLptr oc)
{
	if (!oc)
		return;
	debug_print_out_cells_with_cnt(OC_l(oc));
	print_label(OC_id(oc), stderr,  DO_ESCAPE);
	fprintf (stderr,"=%d(%d)", OC_id(oc), OC_sc(oc));
}

void debug_print_config(CONFIGptr cf, int with_count)
{
	if (!cf)
		return;
	fprintf (stderr,"Config=%d=0x%x ",  (int) cf, (int) cf);
	if (CONFIG_fst_state(cf) && STATE_final(CONFIG_fst_state(cf)))
		fprintf (stderr,"F");
	fprintf (stderr,"state=%d=0x%x, output_cell=%d=0x%x, next=%d=0x%x, count=%d=0x%x,\n\t \
					c_papa=%d=0x%x, s_papa=%d=0x%x, own=%d=0x%x, flag_d_reg=%d=0x%x, \
					tail_c_grandpa=%d=0x%x, _tail_own=%d=0x%x.\n",  
					(int) CONFIG_fst_state(cf),   (int) CONFIG_fst_state(cf),
					(int) CONFIG_output_cell(cf), (int) CONFIG_output_cell(cf),
					(int) CONFIG_next(cf),        (int) CONFIG_next(cf), 
					CONFIG_count(cf),  CONFIG_count(cf), 
					CONFIG_c_papa(cf), CONFIG_c_papa(cf),
					CONFIG_s_papa(cf), CONFIG_s_papa(cf),
					CONFIG_own(cf),    CONFIG_own(cf), 
					(int) CONFIG_flag_d_reg(cf), (int) CONFIG_flag_d_reg(cf),
					CONFIG_tail_c_grandpa(cf),         CONFIG_tail_c_grandpa(cf), 
					CONFIG_tail_own(cf),               CONFIG_tail_own(cf));
	fprintf (stderr,"\t out_cells=%d=0x%x:", (int) CONFIG_output_cell(cf), (int) CONFIG_output_cell(cf));
	if (with_count)
		debug_print_out_cells_with_cnt(CONFIG_output_cell(cf));
	else
		debug_print_out_cells(CONFIG_output_cell(cf));
	fprintf (stderr,"\n");
}

void debug_print_configs(CONFIGptr cf, int with_count)
{
	unsigned int i=1;

	if (!cf)
		return;
	while (cf)
	{ 
		fprintf (stderr,"%d. ",i++);
		debug_print_config(cf, with_count);
		cf = CONFIG_next(cf);
	}
}

#endif /* DEBUG */

/* -----------------------------------------------------------------------*/

static OUT_CELLptr create_output_cell(id_type outid)
{
	/* Allocates an output state, getting one from the freelist,
	if possible. The call to create_output_cell() creates a new
	state: an OUT_CELL is requested from its HEAP
	The reference count of the new state is initialized to 0.
	*/

	OUT_CELLptr new_out_cell = (OUT_CELLptr) cell_alloc(LK_OUT_CELL_HEAP);
	if (new_out_cell == NULL)
		not_enough_memory ("ALLOC OUT_CELL");
	new_out_cell->link = NULL;
	new_out_cell->refcount = 0;
	new_out_cell->symbol_cnt = SYMBOL_COUNT; /* new; gets a global value */
	new_out_cell->outid =  outid;
	return (new_out_cell);
}

static CONFIGptr make_config_det(STATEptr fst_state)
{
	CONFIGptr config = (CONFIGptr) cell_alloc(LK_CONFIG_HEAP);
	CONFIG_fst_state(config) = fst_state;
	CONFIG_next(config) = NULL;
	CONFIG_count(config) = SYMBOL_COUNT; /* new - config-s now carry this info too */
	CONFIG_flag_d_reg(config) = NULL;
	return(config);
}

static void free_config(CONFIGptr config)
{
	if (config)
	{
		if (CONFIG_flag_d_reg(config))
		{
			free_alph(CONFIG_flag_d_reg(config));
		}
#ifdef LKDEBUG_HEAP  
		/* IF DEBUGGING set BREAKPOINT in heap.c at a marked line */
		free_cell_debug((void *) config, LK_CONFIG_HEAP); 
#else
		free_cell((void *) config, LK_CONFIG_HEAP);
#endif
	}
}

static CONFIGptr new_cur_config(CONFIGptr cur_cnfg, STATEptr dest)
{
	/* Makes a new config and splices it in after cur_cnfg */

	CONFIGptr new = make_config_det(dest);
	CONFIG_next(new) = CONFIG_next(cur_cnfg);
	CONFIG_next(cur_cnfg) = new;
	CONFIG_s_papa(new) = CONFIG_s_papa(cur_cnfg);
	CONFIG_c_papa(new) = CONFIG_c_papa(cur_cnfg);
	CONFIG_own(new)=CONFIG_own(cur_cnfg);
	CONFIG_flag_d_reg(new) = copy_alphabet(CONFIG_flag_d_reg(cur_cnfg));
	return(new);
}

static CONFIGptr new_cur_config_in_tail (CONFIGptr cur_cnfg, STATEptr dest)
{
	/* Makes a new config and splices it in after cur_cnfg */

	CONFIGptr new = make_config_det(dest);
	CONFIG_next(new) = CONFIG_next(cur_cnfg);
	CONFIG_next(cur_cnfg) = new;
	/**
	Beware : these 3 marks has a parallel different meaning in producing tail tokens
	but part of the old meaning preserved .... */
	CONFIG_own(new)=CONFIG_own(cur_cnfg);
	CONFIG_tail_c_grandpa(new) = CONFIG_own(cur_cnfg);
	CONFIG_c_papa(new) = CONFIG_c_papa(cur_cnfg);
	CONFIG_tail_own(new) =  ANCESTOR_MARK;
	ANCESTOR_MARK++;
	CONFIG_flag_d_reg(new) = copy_alphabet(CONFIG_flag_d_reg(cur_cnfg));
	return(new);
}


static CONFIGptr new_next_config(CONFIGptr *next_configs, STATEptr dest,
	CONFIGptr cur_cnfg)
{
	/* Makes a new config and pushes it to configp. */

	CONFIGptr new = make_config_det(dest);  
	CONFIG_next(new) = *next_configs;
	*next_configs = new;
	if (CONFIG_flag_d_reg(cur_cnfg))/* IF exists */
		CONFIG_flag_d_reg(new) = copy_alphabet(CONFIG_flag_d_reg(cur_cnfg));
	return(new);
}

/* Allocates a new branch and links it to the output at CNFG. Adds refcount
to its (backwards) destination.
*/
static void add_branch(id_type label_id, 
	OUT_CELLptr prev, 
	CONFIGptr cnfg)
{


	CONFIG_output_cell(cnfg) = create_output_cell(label_id);
	CONFIG_output_cell(cnfg)->refcount++; 
	if (prev)
	{
		prev->refcount++ ;
	}

	CONFIG_output_cell(cnfg)->link = prev;
}

static void add_other_branch(id_type other_exception,
	id_type other_exception_sigma_order,
	OUT_CELLptr prev,
	CONFIGptr cnfg)
{
	OUT_CELLptr other_c=NULL, oe=NULL;
	CONFIG_output_cell(cnfg) = create_output_cell(other_exception_sigma_order);
	CONFIG_output_cell(cnfg)->refcount++;
	oe = create_output_cell(other_exception);
	CONFIG_output_cell(cnfg)->link = oe;
	oe->refcount++;
	other_c=create_output_cell(OTHER);
	oe->link = other_c;
	other_c->refcount++ ;
	other_c->link = prev;
	if (prev)
	{
		prev->refcount++ ;
	}
}

static int is_epsilon_plus (id_type id);
static int is_altchain (id_type id);

/* Allocates a new branch and links it to the output at CNFG. Adds refcount
to its (backwards) destination.
*/  

static void add_branch_with_same_o_chain(
	OUT_CELLptr prev, 
	CONFIGptr cnfg)
{
	/* Links  to the output at CNFG. Adds refcount
	to its (backwards) destination.
	*/
	CONFIG_output_cell(cnfg) = prev; 
	if (prev)
	{
		prev->refcount++ ;
	}
}

static void free_output_cell(OUT_CELLptr cell)
{
	/* Supposes a linear chain backwards from cell;
	If a previous state is referred only by this, it can be deleted.
	Repeat it recursively!
	*/

	OUT_CELLptr prev;

	while (cell && cell->refcount < 1)
	{
		if ((prev = cell->link))
			prev->refcount--;
		free_out_cell(cell);
		cell = prev;
	}
}

static void free_config_and_output_cells(CONFIGptr config)
{
	OUT_CELLptr cell;

	if (config)
	{ 
		if (CONFIG_flag_d_reg(config))
		{
			free_alph(CONFIG_flag_d_reg(config));
		}
		if ((cell = CONFIG_output_cell(config)))
		{
			cell->refcount--; /* refcount now reflects ALL pointers (also those from
							  config-s) to this element */
			free_output_cell(cell);
		}
#ifdef LKDEBUG_HEAP
		/* IF DEBUGGING set BREAKPOINT in heap.c at a marked line */  
		free_cell_debug((void *) config, LK_CONFIG_HEAP); 
#else
		free_cell((void *) config, LK_CONFIG_HEAP); 
#endif    
	}
}

static void free_output_cells_of_config(CONFIGptr config)
{
	OUT_CELLptr cell;

	if (config)
	{
		if ((cell = CONFIG_output_cell(config)))
		{
			cell->refcount--;
			free_output_cell(cell);
		}
	}
}

static int free_config_and_output_cells_onlyif_newsymbol(CONFIGptr config)
{
	if ( (CONFIG_count(config) == SYMBOL_COUNT) )
	{
		free_config_and_output_cells(config);
		return (TRUE);
	}
	else
		return (FALSE);
}

static int free_config_and_output_cells_onlyif_oldsymbol(CONFIGptr config)
{
	if ( (CONFIG_count(config) != SYMBOL_COUNT) )
	{
		free_config_and_output_cells(config);
		return (TRUE);
	}
	else
		return (FALSE);
}

inline
	static int is_epsilon_plus (id_type id)
{
	if (( id == EPSILON) || (id == ALTCHAIN_SYMBOL))
		return (TRUE);
	return(FALSE);
}

inline
	static int is_epsilon_plus_plus (id_type id)
{
	if (   ( id == EPSILON)
		|| ( id == ALTCHAIN_SYMBOL)
		|| ( flag_diacritic_p(id) )
		)
		return (TRUE);

	return(FALSE);
}

inline
	static int is_altchain (id_type id)
{
	if (id == ALTCHAIN_SYMBOL)
		return (TRUE);
	return(FALSE);
}

int lemma_tag_sep_allowed = TRUE; 

/* This enum -  is_first_of_cell_chain - shows whether the sort of cell chain
is what we expect or not */

enum is_first_of_cell_chain {head_reached_but_branching = -1,
	head_not_yet_reached,
	head_reached };

inline
	void print_or_put_to_xelda_buffer(id_type id) {

		/* cell->outid must not be EPSILON. */
		/* direct output */
		if (! XELDA_STYLE_PRINT) {
			if (lemma_tag_sep_allowed && ( fat_strlen (id_to_atom_name (id )) > 1)) {/* multi-char tag
																					 like +NOUN_INV  */    
				fprintf(LOOKUP_OUTPUT_FILE, LOOKUP_FLAGS.lemma_tag_sep);
				lemma_tag_sep_allowed = FALSE;
				print_label(id, LOOKUP_OUTPUT_FILE, DONT_ESCAPE);
			}
			else
				print_label(id, LOOKUP_OUTPUT_FILE, DONT_ESCAPE);
		}
		else {/* Filling a buffer with ids for XELDA */

			*lk_xelda_b_p = id;
			lk_xelda_b_p++;
		}
}

inline
	void put_in_a_buffer(id_type id,void* parseur) {
		static char internal[100];
		/* cell->outid must not be EPSILON. */
		/* direct output */
		if (! XELDA_STYLE_PRINT) {
			if (lemma_tag_sep_allowed && ( fat_strlen (id_to_atom_name (id )) > 1)) {/* multi-char tag
																					 like +NOUN_INV  */    
				insert_token_char(*LOOKUP_FLAGS.lemma_tag_sep,parseur,0);
				lemma_tag_sep_allowed = FALSE;
				//copy_label_name(id, UPPER, internal, 100, TRUE, FALSE);
				insert_token_id(id,parseur);
			}
			else {
				//copy_label_name(id, UPPER, internal, 100, TRUE, FALSE);
				insert_token_id(id,parseur);
			}
		}
		else {/* Filling a buffer with ids for XELDA */

			*lk_xelda_b_p = id;
			lk_xelda_b_p++;
		}
}

static enum is_first_of_cell_chain LKproduce_tokens_nobranching_det (OUT_CELLptr cell, 
	OUT_CELLptr succ,
	unsigned int rs_branch,
	void* parseur) {

		/* Produce tokens from the beginning (left-hand-side end) of the chain
		(let's call it HEAD) until the actual tail cell. We start from the tail
		and descend until the head is found.
		*/
		unsigned int new_rs_branch;
		enum is_first_of_cell_chain  ch_head = head_not_yet_reached;

		if (cell) { /* (right-hand-side) successor MAY exist */

			/* Descend recursively on the chain until the very first cell
			on the left side (head) is reached */
			/* If there was a branch on the right_hand side then the
			remaining left_hand side part mustn't be deleted simply since
			the branch is valid and then those left_hand_side
			cells must be written out later again.  But if there is a segment
			of chain from the head with no branching then this segment
			can not only be produced (printed - ie the characters) but laso freed (the
			cells) 
			In the examples below numbers show reference counts, that
			is, the number of branches from this cell node.

			1 <- 1 <- 9 <- 1 <- 0 All this chain is to be written (from the left);
			the rightmost (after 9) segment must be freed, too.
			9 itself must remain (but decreased to 8).
			3 <- 1 <- 1 <- 1 <- 0 All this chain is to be written (from the left);
			The rightmost segment of this chain
			(after 3) must be freed, too.
			1 <- 1 <- 1 <- 1 <- 0	The whole chain can be written and freed
			including the rightmost (0) cell.
			*/

			if (rs_branch < cell->refcount)
				/* this is a branch. It is told to recursive call(s) for ancestor cell(s). */
				new_rs_branch = cell->refcount;
			else
				new_rs_branch = rs_branch;

			if ( (cell->link) == NULL ) {/* then it is the head:  useless to descend more ! */
				/* This cell is the head of the chain - that is, the first 
				character to be produced  - is reached now - PRINTING HERE */	
				/* 	  if (cell->outid != EPSILON) */
				if ( ! (is_epsilon_plus (cell->outid ))) {
					//print_or_put_to_xelda_buffer(cell->outid);
					put_in_a_buffer(cell->outid,parseur);
				}

				if ( rs_branch < 2 && cell->refcount <2) {
					/* 6.09.96 ** if ( rs_branch < 2 && cell->refcount <1) ** */
					/* That is, no branching among offsprings - a singleton chain/tree.
					Such a linear tree can be produced and (its constituent cells) freed immediately.
					Else this cell has other references as well; so, even
					if the head of the chain is being reached now, this
					cell must only be produced but not freed */

					if (succ) {

						succ->link = NULL;
						if (cell->link)
							(cell->link->refcount)--;
						free_out_cell(cell);
					}
					ch_head = head_reached;
				}
				else {

					/* There is branching of offsprings so no free-ing yet */
					ch_head = head_reached_but_branching;
				}
				return (ch_head); /* ascend immediately */
			}

			/* the new rs-branch is either inherited or syntesized */
			ch_head = LKproduce_tokens_nobranching_det(cell->link, cell, new_rs_branch,parseur);

			if (! (is_epsilon_plus(cell->outid ))) {
				put_in_a_buffer(cell->outid,parseur);
				// print_or_put_to_xelda_buffer(cell->outid);
			}

			/* 6.09.96 ** if (rs_branch < 2 && cell->refcount <1 ) */
			if (rs_branch < 2 && cell->refcount <2 )  {

				if (succ) {

					succ->link = NULL;
					if (cell->link)
						(cell->link->refcount)--;
					free_out_cell(cell);
				}
			}

		}
		return (ch_head);
}

static id_type * read_tokens_norm_ids (OUT_CELLptr cell,
	id_type *here)
{
	/* Produce tokens from the beginning (left-hand-side end) of the chain
	(let's call it HEAD) until the actual tail `cell'.
	Start from the tail and descend until the head is found.
	*/

	if (cell && ( cell->symbol_cnt == SYMBOL_COUNT) ) /* descend only until this
													  symbol's id segment */
	{
		if ( (cell->link) == NULL ) /* then it is the head:  useless to descend more ! */
			/* This cell is the head of the chain - that is, the first 
			character to be produced  - is reached now - "PRINTING" HERE */
		{
			/* ** we don't let go NEITHER epsilons NOR altchain-s  to a next fst at
			all   - but they may be present in the output chains! 
			99-10-22: there must not be any epsilon in the output chains (by
			creation) ... TG
			2000-2-1: ? 0 (OTHER followed - logically - by an EPSILON) is allowed:
			it means that this OTHER has no exception symbol. This change belongs
			to the "OTHER as input" extension. TG.
			*/
			if (! is_altchain (cell->outid ))

			{	 
				/* put into NORM ID buffer */
				*here = cell->outid; here++;
			}

			return (here) ; /* ascend immediately */
		}
		/* descending on the chain back */
		here=read_tokens_norm_ids (cell->link, here);
		/* * we don't let go NEITHER epsilons NOR altchain-s to a next fst at
		all - but they may be present in the output chains!
		99-10-22: there must not be any epsilon in the output chains (by creation)
		2000-2-1: ? 0 (OTHER followed - logically - by an EPSILON is allowed:
		it means that this OTHER has no exception symbol. ?:? and ? as input
		extension. TG.
		*/
		if (! is_altchain (cell->outid ))
		{	 
			/* put into NORM ID buffer */
			*here = cell->outid; here++;
		}
	}
	return(here);
}

inline
	static int is_same_w_chain (OUT_CELLptr a, OUT_CELLptr b, 
	int symbol_count_equivalence_needed, 
	int strict_equivalence_needed)
{
	int result=FALSE;
	OUT_CELLptr  next_a = NULL, next_b = NULL;

	for ( ;
		a || b ;
		a = next_a , b = next_b ) 
	{
		while ( a && !strict_equivalence_needed && is_epsilon_plus (OC_id(a)))
			a = OC_l(a);
		while ( b && !strict_equivalence_needed && is_epsilon_plus (OC_id(b)))
			b = OC_l(b);

		if (( a && b &&  (OC_id(a) ==  OC_id(b)) ) 
			||  ( !a && !b))
		{
			if (symbol_count_equivalence_needed &&
				( OC_sc(a) != OC_sc(b)))
				return (FALSE);
			else
				result = TRUE;
		}
		else
			if ( (a && (!is_epsilon_plus (OC_id(a))) && (!b))
				|| (b && (!is_epsilon_plus (OC_id(b))) && (!a))
				|| (a&& b && ( OC_id(a) !=  OC_id(b))))
				return (FALSE);
		if (a)
			next_a = OC_l(a);
		else 
			next_a = NULL;
		if(b)
			next_b = OC_l(b);
		else 
			next_b = NULL;
	}
	return(result);
}

static int flag_registers_strictly_identical(ALPHABETptr fr1, ALPHABETptr fr2)
	/* Check whether fr1 and fr2 (either can be 0) represent the
	same flag diacritics attributes and values. */
{
	register int i; 

	if (fr1 == fr2) 
		return TRUE;
	else if ( (fr1 && fr2) && 
		( ALPH_len(fr1) == ALPH_len(fr2)))
	{
		for (i = 0; i < ALPH_len(fr1) ; i++)
		{
			if (ALPH_item(fr1, i) != (ALPH_item(fr2, i)))
				return FALSE;
		}
		return TRUE;
	} 
	return FALSE;      
}

static int is_doubled_config (CONFIGptr new_config, CONFIGptr config)
{
	int result=FALSE;

	if (new_config && config &&
		(CONFIG_fst_state(new_config) == CONFIG_fst_state(config)) && 
		flag_registers_strictly_identical
		(CONFIG_flag_d_reg(new_config), CONFIG_flag_d_reg(config)))
		/* there may be identity. Symbol chains must be verified, too */
	{
		result = is_same_w_chain( CONFIG_output_cell(new_config),  CONFIG_output_cell(config),
			FALSE, TRUE /** New 2000-2-2 **/);
	}
	return(result);
}

static CONFIGptr eliminate_double_configs(CONFIGptr *cur_configs, int symbols_only);

static CONFIGptr produce_tail_tokens_det(CONFIGptr *cur_configs,
	NETptr nnet)
{
	/* Produce remaining end-of-string tail tokens. We have to scan the
	remaining configs to add the results of any final epsilon
	transitions.
	Non-final configurations are eliminated. 
	*/

	CONFIGptr config, next, prev = NULL;
	STATEptr fst_state, dest;
	AVptr arc_vector; /* fast */
	ARCptr arc;        /* slow */
	OUT_CELLptr output_cell;
	uint16 i;
	id_type arc_id, up, low, fst_id, attr, old_val  ;
	ALPHABETptr low_eps, low_fld;
	RANGEptr eps_range;

	/*  Now duplications can be eliminated */
	*cur_configs = eliminate_double_configs(cur_configs, FALSE);

	/* Scan config-s for remaining epsilon transitions. 
	New configs may be created.
	*/

	for (config = *cur_configs; config; config = CONFIG_next(config))
	{
		fst_state = CONFIG_fst_state(config);
		output_cell = CONFIG_output_cell(config);

		/* FAST (vectorized) part */
		if (STATE_vector_p(fst_state))
		{
			arc_vector = STATE_arc_vector(fst_state);
			if (arc_vector)
			{
				if ((low_eps = AV_l_eps(arc_vector)))
					/* Process arcs that have an EPSILON on the lower side.
					Staying in the current position. Any new
					config gets stuck on end, so eventually it will be
					processed at this string position.
					*/

					for (i = 0; i < ALPH_len(low_eps); i++)
					{
						fst_id = ALPH_item(low_eps, i);
						up = upper_id(fst_id);
						dest = AV_dest(arc_vector, fst_id);			      
#ifdef LKDEBUG_ADD		 
						/*  ** Test, debug ** */
						debug_printinfo_branch_out("p-t-t_d Arc label-fast-eps: ",
							fst_id, up, output_cell->outid, 0);
#endif
						if (! is_epsilon_plus_plus(up))
							add_branch(up, output_cell,
							new_cur_config_in_tail(config, dest));
						else
							add_branch_with_same_o_chain( output_cell,
							new_cur_config_in_tail(config, dest));
					}

				else if ( NET_l_flag_diacr(nnet) &&
					( low_fld = AV_l_fld(arc_vector)))
					for (i = 0; i < ALPH_len(low_fld); i++)
					{	
						fst_id = ALPH_item(low_fld, i);
						up = upper_id(fst_id);
						low = lower_id(fst_id);
						dest = AV_dest(arc_vector, fst_id);			
						if ( (attr = flag_diacritic_attrib(low)))
						{
							old_val = flag_value(CONFIG_flag_d_reg(config), attr);
							if (flag_action (CONFIG_flag_d_reg(config), flag_diacritic(low)))
							{
								if ( ! is_epsilon_plus_plus (up) )
									add_branch
									(up, output_cell,
									new_cur_config(config, dest));
								else /* this is a hack we know EPSILON == 0  */
									add_branch_with_same_o_chain 
									(output_cell, 
									new_cur_config(config, dest));
							}
							flag_value(CONFIG_flag_d_reg(config), attr) = old_val;
						}
					}
			}
		}

		/* SLOW (non-vectorized)  part */
		else
			for (arc = STATE_arc_set(fst_state); arc; arc = next_arc(arc))
			{
				if (NET_reduced_labelset(nnet)) /* reduced labelset */
				{
					eps_range = RANGE_VECTOR_vector(MATCH_TABLE_table(NET_upmatch_table(nnet))[ARC_label(arc)]);
					i = 0;

					if (eps_range && eps_range->items)
						arc_id = RANGE_item(eps_range, i);
					else
						arc_id = EPSILON;
				}
				else /* labelset not reduced  */
				{
					eps_range = 0;
					arc_id = ARC_label(arc);
				}

				do /* slow inner loop 99-5-3 */
				{      
					if (arc_id)
					{
						up = upper_id(arc_id);
						low = lower_id(arc_id);
						if (is_epsilon_plus (low))
						{
#ifdef LKDEBUG_ADD
							/* ** Test, debug ** */
							debug_printinfo_branch_out("p-t-t Arc label-sl-eps: ",
								arc_id, up,
								output_cell->outid, 0);
#endif
							if (! is_epsilon_plus_plus(up))
								add_branch(up, output_cell,
								new_cur_config_in_tail(config, ARC_destination(arc)));
							else
								add_branch_with_same_o_chain( output_cell,
								new_cur_config_in_tail(config,  ARC_destination(arc)));
						}
						else if ( NET_l_flag_diacr(nnet) &&
							flag_diacritic_p (low) &&
							(attr = flag_diacritic_attrib(low)))
						{
							/* if Flag Diacritics allow then */
							old_val = flag_value(CONFIG_flag_d_reg(config), attr);
							if (flag_action (CONFIG_flag_d_reg(config), flag_diacritic(low)))
							{
								if ( ! is_epsilon_plus_plus (up) )
								{
#ifdef LKDEBUG_ADD
									/* ** Test, debug ** */
									debug_printinfo_branch_out("p-t-t Arc label-fl-dia: ",
										ARC_label(arc), up,
										output_cell->outid, 0);
#endif
									add_branch(up, output_cell,
										new_cur_config_in_tail(config, ARC_destination(arc)));
								}
								else
								{
									add_branch_with_same_o_chain (output_cell,
										new_cur_config_in_tail(config, ARC_destination(arc)));	      
									/* copy flag_diacritic_register to new*/
								}
							}
							flag_value(CONFIG_flag_d_reg(config), attr) = old_val;
						} /* Flag diacritics end */
					}
					/* if reduced labelset: further loop on hidden (virtual) arcs*/ 
					if (NET_reduced_labelset(nnet))
					{
						i++;

						if (eps_range && i < RANGE_len(eps_range) && RANGE_items(eps_range))
							arc_id = RANGE_item(eps_range, i);
						else
							arc_id = EPSILON;
					}
				} while (eps_range && i < RANGE_len(eps_range));
			} /*  slow part End */
	}

	/* Now duplications can be eliminated - again */
	*cur_configs = eliminate_double_configs(cur_configs, FALSE);

	/* checking final state - only finals remain */
	for (config = *cur_configs; config;  config = next)
	{
		next = CONFIG_next(config);

		if (STATE_final(CONFIG_fst_state(config)))
			prev = config;
		else
		{
			if (prev)
				CONFIG_next(prev) = next;
			else
				*cur_configs = next;
			free_config_and_output_cells(config);
		}
	}

	/*  HACK - 3d time: duplications can be eliminated */
	*cur_configs = eliminate_double_configs(cur_configs, TRUE);

	return (*cur_configs);
}

typedef struct RES_CONFS {
	CONFIGptr head;
	CONFIGptr tail;
} RES_CONFStype, *RES_CONFSptr;

unsigned int LK_OUT_CHAIN_DEPTH; /* ** for tuning chain ssize maximum to be compared.
								 If the chain is longer we let memory grow */

OUT_CELLptr has_identical_config (id_type new_id, OUT_CELLptr  new_o_chain, 
	CONFIGptr config) 
{
	OUT_CELLptr a = (OUT_CELLptr ) 33 /* debug */ , b =  (OUT_CELLptr ) 41 /* debug */, 
		result=NULL;
	CONFIGptr con;
	/* **
	"Quantify" measurement shows that if chains are long then it is very time
	consuming. So we try to limit comparisons by tuning to a given depth only */
	if (config
		&& (SYMBOL_COUNT < LK_OUT_CHAIN_DEPTH) /* */
		)
		for ( con = config; con;  con = CONFIG_next(con)) /* traverse the config chain */
		{
			/* there MAY be identity. Symbol chains must be verified, too */
			{
				/* Preparing loop variables for the main loop:
				first the two chains - that of the future new one and that of the actual
				loop variable - are "synchronised": 
				new_id          -> b -> c -> ... -- where  b -> c -> ... is new_o_chain; 
				(O_id(O_l(con)) -> b -> c -> ... -- that is, the chain of the actual config;
				But both chains may contain epsilons. */

				b = CONFIG_output_cell(con);
				if (new_id && (!b))
					break; /* no identity */
				if ( is_epsilon_plus(new_id))
				{
					if ( (!b) && (!new_o_chain))
						return (CONFIG_output_cell(con)); /* identity by nil */
					a = new_o_chain;
				}
				else /* */
				{
					if (! b)
						break; /* difference: b empty */

					if ( new_id == OC_id (b)
						/*new 99-11-10 : */ && (SYMBOL_COUNT == OC_sc (b)))
					{
						a = new_o_chain;
						b = OC_l(b);
					}

					else  break;

					if (is_same_w_chain(a, b, TRUE, TRUE))
						return ( CONFIG_output_cell(con));
				}
			}	
		}
		return result;
}

void kill_cascade_offsprings(unsigned int config_own_mark, CONFIGptr *offsprings_configs,  int net_order)
	/*
	At final epsilon transitions, a config may prove invalid by arriving to a non-final
	ultimate state. Now its cascade-wise offsprings must be eliminated, recursively, as far
	as the last fst.
	*/
{
	CONFIGptr con, next, prev = NULL;

	for (con = *offsprings_configs; con; con = next)
	{
		next =  CONFIG_next(con);
		if (config_own_mark == CONFIG_c_papa(con))
		{
			if (prev)
				CONFIG_next(prev) = next;
			else 
				*offsprings_configs = next;
			if ((NET_NO - net_order -1) > 0) /* if next (in cascade) fst exists */
				kill_cascade_offsprings(CONFIG_own(con),
				&(STRAT_CONTROL[net_order+1].prev_symbol_result),
				net_order+1);
			free_config_and_output_cells(con);
		}
		else
			prev = con;
	}
}

/* This is expensive - but less expensive than letting them multiply ! */
CONFIGptr eliminate_double_configs(CONFIGptr *cur_configs, int symbols_only)
{
	CONFIGptr next, cf, config, prev = NULL; 
	int killed;

	for (config = *cur_configs; config; config = next)
	{
		next = CONFIG_next(config);
		killed = FALSE;
		for (cf =  CONFIG_next(config) ; cf; cf = CONFIG_next(cf))
		{
			killed = FALSE; 

			if ( (symbols_only ?
				is_same_w_chain(CONFIG_output_cell(config), CONFIG_output_cell(cf), FALSE, TRUE) :
			is_doubled_config ( config, cf)))
			{
				if (prev)
					CONFIG_next(prev) = next;
				else
					*cur_configs = next;
				free_config_and_output_cells(config);
				killed = TRUE;
				break;
			}
		}
		if (! killed)
			prev = config; 
	}
	return (*cur_configs);
}

inline
	int inheritance_marks_match ( CONFIGptr norm_config,  CONFIGptr config,
	int downcall_allowed)

{
	if( downcall_allowed)
	{

		if ( (CONFIG_s_papa(norm_config) ==  CONFIG_c_papa(config))
			||
			(CONFIG_own(norm_config) ==  CONFIG_c_papa(config)) 
			) 
			/* **
			if (CONFIG_s_papa(norm_config) ==  CONFIG_c_papa(config)) 
			*/ 
			return (TRUE);
		else 
			return (FALSE);
	}
	else /* tail token handling ! */
	{
		/* **  if (CONFIG_tail_own(norm_config) ==  CONFIG_c_papa(config)) */
		if ((CONFIG_tail_c_grandpa(norm_config) ==  CONFIG_c_papa(config)) ||
			(CONFIG_tail_own(norm_config) ==  CONFIG_c_papa(config)))
			return (TRUE);
		else 
			return (FALSE);
	}
}

/* Setting inheritance marks from both previous symbol and preceding fst in the
cascade  */
inline
	void  set_inheritance_marks (CONFIGptr *next_configs, CONFIGptr config,  
	CONFIGptr norm_config,
	int downcall_allowed,
	int inhibit_symbol_count_ancestry,
	int inhibit_tail_c_papa_ancestry, /* only at tail tokens */
	int net_order)
{
	if (downcall_allowed) /* usually */
	{
		CONFIG_own(*next_configs) =  ANCESTOR_MARK;
		ANCESTOR_MARK++;

		if ( SYMBOL_COUNT == 0 )
			CONFIG_s_papa(*next_configs) = 0; /* at initialization */
		else
		{	
			if (!inhibit_symbol_count_ancestry)
				CONFIG_s_papa(*next_configs) = CONFIG_own(config);
			else /* lest loose track of s-papa too fast */
				CONFIG_s_papa(*next_configs) = CONFIG_s_papa(config);
		}
		if (net_order == 0) /* first fst handled differently */
			CONFIG_c_papa(*next_configs) = 0;
		else
			CONFIG_c_papa(*next_configs) =
			CONFIG_own(norm_config);
	}
	else /* inheritance marks copied from config - we are at tail tokens */

		/* - we are at tail tokens .inheritance marks are set differently! */
		/* Now we use the three fields as follows: 
		- the meaning of c_papa remains
		- the former own becomes c_grandpa (indicating common ancestor with
		offspring's c_papa)
		- the former s_papa becomes tail_own 
		So inheritance relitions can be kept and refined - or I hope so */
	{
		CONFIG_tail_own(*next_configs) =  ANCESTOR_MARK;
		ANCESTOR_MARK++;
		if (inhibit_tail_c_papa_ancestry)
			CONFIG_c_papa(*next_configs) = CONFIG_c_papa(config);
		else /* last char - so it can get a new number */
			CONFIG_c_papa(*next_configs) = CONFIG_tail_own(norm_config);
		CONFIG_tail_c_grandpa(*next_configs) = CONFIG_own(config);
	}

}

/* configs that are not descendants of  norm_configs are freed */
CONFIGptr select_descendant_configs_only(CONFIGptr norm_configs, CONFIGptr *configs)
{
	CONFIGptr next, nc, con, prev = NULL;
	int kill = TRUE;

	for (con = *configs; con ; con = next)
	{
		next = CONFIG_next(con);
		kill = FALSE;
		for (nc =  norm_configs ; nc; nc = CONFIG_next(nc))
		{
			kill = TRUE;
			if ( inheritance_marks_match(nc, con, FALSE))
			{
				kill = FALSE; 
				break;
			}
		}
		{
			if (kill)
			{
				if (prev)
					CONFIG_next(prev) = next;
				else
					*configs = next;
				free_config_and_output_cells(con);
			}
			else
				prev = con; 
		}
	}	 
	return (*configs);
}

static RES_CONFStype fst_subcomposition ( NVptr nv, 
	int net_order,
	CONFIGptr norm_config, 
	int downcall_allowed);

static CONFIGptr produce_tail_tokens(CONFIGptr *cur_configs, int net_order,  NVptr nv)
{
	/* Produce remaining end-of-string tail tokens. We have to scan the remaining configs
	to add the results of any final epsilon transitions.
	Non-final configurations are eliminated. 
	*/

	CONFIGptr cf, config, next, prev = NULL,
		down_res_configs = NULL, d_r_c_tail = NULL, 
		doomed_configs = NULL, offsprings_doomed = NULL ;
	RES_CONFStype rc;
	STATEptr fst_state, dest;
	AVptr arc_vector; /* fast */
	ARCptr arc;        /* slow */
	OUT_CELLptr output_cell;
	int rescued;
	uint16 i;
	id_type  arc_id, up, low, fst_id, attr, old_val;
	ALPHABETptr low_eps, low_fld;
	RANGEptr eps_range;

	/*  Now duplications can be eliminated */
	*cur_configs = eliminate_double_configs(cur_configs, FALSE);

	/* Scan config-s for remaining epsilon transitions. 
	New configs may be created.
	*/

	for (config = *cur_configs; config; config = CONFIG_next(config))
	{
		fst_state = CONFIG_fst_state(config);
		output_cell = CONFIG_output_cell(config);

		/* FAST (vectorized) part */
		if (STATE_vector_p(fst_state))
		{
			arc_vector = STATE_arc_vector(fst_state);
			if (arc_vector)
				if ((low_eps = AV_l_eps(arc_vector)))
					/* Process arcs that have an EPSILON on the lower side.
					Staying in the current position. Any new
					config gets stuck on end, so eventually it will be
					processed at this string position.
					*/

					for (i = 0; i < ALPH_len(low_eps); i++)
					{
						fst_id = ALPH_item(low_eps, i);
						up = upper_id(fst_id);
						dest = AV_dest(arc_vector, fst_id);
#ifdef LKDEBUG_ADD		      
						/* ** Test, debug */
						debug_printinfo_branch_out("p-t-t Arc label-fast-eps:",
							fst_id, up, output_cell->outid, net_order); 
#endif				
						if (! is_epsilon_plus_plus(up))
						{

							if (up == OTHER &&
								((NET_NO - net_order -1) > 0))
							{
								add_other_branch(EPSILON, net_order, output_cell,
									new_cur_config_in_tail(config,
									dest));
							}
							else
								add_branch(up, output_cell,
								new_cur_config_in_tail(config, dest));
						}
						else
							add_branch_with_same_o_chain( output_cell,
							new_cur_config_in_tail(config, dest));
					}
					if (
						/* ** has_flag_diacritics_p (NV_net(nv,net_order)) &&  */
						NET_l_flag_diacr(NV_net(nv, net_order)) &&
						( low_fld = AV_l_fld(arc_vector)))
						for (i = 0; i < ALPH_len(low_fld); i++)
						{	
							fst_id = ALPH_item(low_fld, i);
							up = upper_id(fst_id);
							low = lower_id(fst_id);
							dest = AV_dest(arc_vector, fst_id);			
							if ( (attr = flag_diacritic_attrib(low)))
							{
								old_val = flag_value(CONFIG_flag_d_reg(config), attr);
								if (flag_action (CONFIG_flag_d_reg(config), flag_diacritic(low)))
								{
									if ( ! is_epsilon_plus_plus (up) )
										add_branch
										(up, output_cell,
										new_cur_config(config, dest));
									else /* this is a hack we know EPSILON == 0  */
										add_branch_with_same_o_chain 
										(output_cell, 
										new_cur_config(config, dest));
								}
								flag_value(CONFIG_flag_d_reg(config), attr) = old_val;
							}
						}
		}

		/* SLOW (non-vectorized)  part */
		else
			for (arc = STATE_arc_set(fst_state); arc; arc = next_arc(arc))
			{
				if (NET_reduced_labelset(NV_net(nv, net_order))) /* reduced labelset */
				{
					eps_range = RANGE_VECTOR_vector(MATCH_TABLE_table(NET_upmatch_table(NV_net(nv, net_order)))[ARC_label(arc)]);
					i = 0;

					if (eps_range && eps_range->items)
						arc_id = RANGE_item(eps_range, i);
					else
						arc_id = EPSILON;
				}
				else /* labelset not reduced  */
				{
					eps_range = 0;
					arc_id = ARC_label(arc);
				}

				do /* slow inner loop 99-5-3 */
				{      
					if (arc_id)
					{
						up = upper_id(arc_id);
						low = lower_id(arc_id);
						if (is_epsilon_plus (low))
						{
#ifdef LKDEBUG_ADD
							/* ** Test, debug */
							if (output_cell)
								debug_printinfo_branch_out("p-t-t Arc label-sl-eps: ",
								arc_id, up, output_cell->outid, net_order);
							else
								debug_printinfo_branch_out("p-t-t Arc label-sl-eps: 0 output_cell",
								arc_id, up, 0, net_order);
#endif
							if (! is_epsilon_plus_plus(up))
							{
								if (up == OTHER &&
									((NET_NO - net_order -1) > 0))
								{
									add_other_branch(EPSILON, net_order, output_cell,
										new_cur_config_in_tail(config,
										ARC_destination(arc)));
								}
								else
									add_branch(up, output_cell,
									new_cur_config_in_tail(config, ARC_destination(arc)));
							}
							else
								add_branch_with_same_o_chain(output_cell,
								new_cur_config_in_tail(config, ARC_destination(arc)));
						}
						else if ( NET_l_flag_diacr (NV_net(nv, net_order)) &&
							flag_diacritic_p (low) &&
							(attr = flag_diacritic_attrib(low)))
						{
							/* if Flag Diacritics allow then */
							old_val = flag_value(CONFIG_flag_d_reg(config), attr);
							if (flag_action (CONFIG_flag_d_reg(config), flag_diacritic(low)))
							{
								if ( ! is_epsilon_plus_plus (up) )
								{
#ifdef LKDEBUG_ADD
									/* ** Test, debug ** */
									debug_printinfo_branch_out("p-t-t Arc label-fl-dia: ",
										arc_id, up,
										output_cell->outid, net_order);
#endif
									add_branch(up, output_cell,
										new_cur_config_in_tail(config, ARC_destination(arc)));
								}
								else
								{
									add_branch_with_same_o_chain (output_cell,
										new_cur_config_in_tail(config, ARC_destination(arc)));	      
									/* copy flag_diacritic_register to new*/
								}
							}
							flag_value(CONFIG_flag_d_reg(config), attr) = old_val;
						} /* Flag diacritics end */
					}
					/* if reduced labelset: further loop on hidden (virtual) arcs*/ 
					if (NET_reduced_labelset(NV_net(nv, net_order)))
					{
						i++;

						if (eps_range && i < RANGE_len(eps_range) && RANGE_items(eps_range))
							arc_id = RANGE_item(eps_range, i);
						else
							arc_id = EPSILON;
					}
				} while (eps_range && i < RANGE_len(eps_range));
			} /* End of SLOW (non-vectorized)  part */
	}

	/* Now duplications can be eliminated - again */
	*cur_configs = eliminate_double_configs(cur_configs, FALSE);

	/* checking final state and acceptance by next fst. If a state is non-final then all
	its cascade-wise offsprings must be deleted, recursively. If it is final then it must be 
	accepted by all cascade-offsprings and if not then again those cascade offsprings must
	be deleted.
	*/
	for (config = *cur_configs; config;  config = next)
	{
		next = CONFIG_next(config);

		if (STATE_final(CONFIG_fst_state(config)))
		{
			/* check by next fst if any - but only if a new config got created */
			/* **  WITH a new char */
			if ( ((NET_NO - net_order -1) > 0)/* next fst exists */
				&& CONFIG_output_cell(config)
				/* ** 97-03-25
				&& (OC_sc(CONFIG_output_cell(config)) == SYMBOL_COUNT)
				** *//* new char */
				)
			{
				rc = fst_subcomposition 
					( nv,
					net_order+1,
					config, /* check the newest only */
					FALSE);
				if (rc.head) /* it is worth to go on */
				{
					/* Unify temp results for later */
					CONFIG_next(rc.tail) = down_res_configs;
					down_res_configs = rc.head;
					if (! d_r_c_tail)
						d_r_c_tail = rc.tail;
					prev = config;
				}
				else /* next fst didn't accept it so kill it; its recursive offsprings too */
				{
					if (prev)
						CONFIG_next(prev) = next;
					else
						*cur_configs = next;

					/* put it onto doomed list and delete output cells but don't delete
					it yet because a sibling may survive and so we don't know
					whether its (ie their, including all siblings) may survive. THis
					is possible multiple inheritance */
					CONFIG_next(config) = doomed_configs;
					doomed_configs = config;
					/* ** free_output_cells_of_config(config); */
				}
			}
			else /* either no match or no next fst or no  new symbol to check */
				prev = config;
		}
		else /* not a final state */
		{
			if (prev)
				CONFIG_next(prev) = next;
			else 
				*cur_configs = next;
			CONFIG_next(config) = doomed_configs;
			doomed_configs = config;
			/* ** free_output_cells_of_config(config); */
		}
	}

	/* And now only can failed offsprings in the next fst in the cascade eliminated.
	1. create success_list - this is cur_configs above
	2. create a doomed_list (of this fst configs) this is the list of failed configs
	(that don't have an output_cell already), se above;
	3. create a "real" doomed list for offsprings - if an inheritance path is really
	extinct only then can be cascade offsprings deleted. 
	This is needed because at final epsilon config creation the newly created
	"current config"-s inherit magic ancestry numbers. Now certain members of
	respective subsets of the same ancestry may have a final state other members may
	not have but offsprings has no way of knowing which is which. So if there is at
	least one success (in a subset of the same ancestry) this subset is saved.
	**
	Pb: (28.06.96) this is not fine-granularity enough: unwanted multiplication may
	result if the whole subset is saved. A refined sub-algorithm is under work. 
	**
	4. kill offsprings of real failures: not a single member of the same ancestry
	subset survived so it is useless to leave their offsprings above.
	5. Release doomed_list of all failed config-s (created at 2).
	*/
	if ((*cur_configs) && doomed_configs && ((NET_NO - net_order -1) > 0)) /* if next (in cascade) fst exists */
	{
		for ( config = doomed_configs; config; config = CONFIG_next(config)) /* step 3 above */
		{
			rescued = FALSE;
			for (cf = *cur_configs; cf; cf = CONFIG_next(cf)) /* success list */
			{
				if (CONFIG_own(cf) == CONFIG_own(config)) /* has successful sibling */
				{
					rescued = TRUE; 
					break;
				}
			}
			if (! rescued)
			{
				CONFIG_next(config) = offsprings_doomed; /* offsprings are doomed, too */
				offsprings_doomed = config;
			}
		}
		for ( config = offsprings_doomed; config; config = CONFIG_next(config)) /* step 4 above */
		{
			kill_cascade_offsprings(CONFIG_own(config),
				&(STRAT_CONTROL[net_order+1].prev_symbol_result),
				net_order+1);
		}
	}

	/* step 5 above, releasing all final/cascade-failed configs */
	for (config = doomed_configs; config; config = next)
	{
		next = CONFIG_next(config);
		free_config_and_output_cells(config); /*  */
	}

	/* link new results for next fst */
	if ((NET_NO - net_order -1) > 0)
	{
		if (d_r_c_tail) /* there was a new result in the next fst */
		{
			STRAT_CONTROL [net_order + 1].prev_symbol_result =  down_res_configs;
		}
		/* but possibly next fst result didn't change - no production happened here -
		so delete obsolete ones */
		else
			STRAT_CONTROL [net_order + 1].prev_symbol_result = 
			select_descendant_configs_only
			(*cur_configs, &(STRAT_CONTROL [net_order+1].prev_symbol_result));  
	}

	/* HACK Try doing duplication - elimination here third time ... */
	/* only the last one ... */
	if ((NET_NO - net_order -1) == 0)
		*cur_configs = eliminate_double_configs(cur_configs, TRUE);

	/** 1999-10-20 try more selection downwards by eliminating not descendants */
	for (i = net_order+1 ; i < NET_NO-1 ; i++)
	{
		select_descendant_configs_only(STRAT_CONTROL [i].prev_symbol_result,
			&(STRAT_CONTROL [i+1].prev_symbol_result));
	}


	return (*cur_configs);
}

/* An identical config chain is created pointing to the same respective output
chain-s as the original one. So the 2 chains can be deleted independently */
RES_CONFStype copy_config_chain (CONFIGptr config)
{
	RES_CONFStype res_configs = { NULL,  NULL } ;
	CONFIGptr c, prev = NULL, newc = NULL ;

	if (config)
	{

		for (c = config; c; c = CONFIG_next(c))
		{
			newc = make_config_det (CONFIG_fst_state (c));
			CONFIG_flag_d_reg(newc) = copy_alphabet(CONFIG_flag_d_reg(config));
			CONFIG_s_papa(newc) = CONFIG_s_papa(c);
			CONFIG_c_papa(newc) = CONFIG_c_papa(c);
			CONFIG_own(newc) = CONFIG_own(c);
			CONFIG_output_cell(newc) =  CONFIG_output_cell(c);
			if  (CONFIG_output_cell(newc))
				CONFIG_output_cell(newc)->refcount ++ ;
			if (prev)
				CONFIG_next(prev) = newc;
			else
				res_configs.head = newc;
			prev = newc;
		}
		res_configs.tail = newc;
	}
	return (res_configs);
}

/* If a normalizer sends no new input to an fst it must still keep the relevant
previous results in the hope of a later coming input, eventually for final
tail-token handling - there can be still epsilon-transitions.
*/
RES_CONFStype actualize_by_copy_former_descendants (CONFIGptr norm_config, 
	CONFIGptr config, 
	int net_order,
	int downcall_allowed)  
{
	RES_CONFStype res_configs = { NULL,  NULL }, rc = { NULL,  NULL } ;
	CONFIGptr c, prev = NULL, newc = NULL;

	/* copying config-s that has the same immediate ancestry only */
	if (norm_config && config)
	{

		for (c = config; c; c = CONFIG_next(c))

			if (inheritance_marks_match (norm_config, c, downcall_allowed))
			{
				newc = make_config_det (CONFIG_fst_state (c));
				CONFIG_flag_d_reg(newc) = copy_alphabet(CONFIG_flag_d_reg(c));
				set_inheritance_marks (&newc, c, norm_config, downcall_allowed, FALSE,
					FALSE, net_order);
				CONFIG_output_cell(newc) =  CONFIG_output_cell(c);
				if  (CONFIG_output_cell(newc))
					CONFIG_output_cell(newc)->refcount ++ ;
				if (prev)
					CONFIG_next(prev) = newc;
				else
					res_configs.head = newc;
				prev = newc;
				/* recursive downcall */
				if ( downcall_allowed && ((NET_NO - net_order -1) > 0)) /* next fst exists */
				{
					rc = actualize_by_copy_former_descendants 
						(newc, STRAT_CONTROL[net_order+1].prev_symbol_result, net_order+1,
						downcall_allowed);
					/* accumulating results for the next fst */
					if ( rc.tail)
					{
						CONFIG_next(rc.tail) = STRAT_CONTROL [net_order + 1].temp_result;
						STRAT_CONTROL [net_order + 1].temp_result = rc.head;
					}
				}
			}
	}

	res_configs.tail = newc;
	return (res_configs);
}

void clear_and_free_configs_cells(CONFIGptr cur_configs);

static int match_other(int net_order,
	id_type in_id,
	id_type arc_id, id_type arc_lower_id, id_type * arc_upper_id_ptr, 
	id_type in_other_exception, id_type * out_other_exception_ptr,
	id_type in_other_exception_sigma,
	id_type * out_other_exception_sigma)
	/* 
	If there is a real (non-EPSILON) IN_ID and either this or ARC_LOWER_ID is
	OTHER (but not the identity-OTHER) then some provision is to be made: all the
	possible cases are examined and the result, if any, goes to * ARC_UPPER_ID_PTR
	and/or to * OUT_OTHER_EXCEPTION_PTR .
	SIGMA_PREV is the sigma alphabet of the previous net providing IN_ID.
	SIGMA_THIS is the sigma alphabet of the actual net.
	ARC_LOWER_ID, ARC_UPPER__ID_PTR are variables with id-s of the the actual arc
	(lower 	is matched, upper may be changed).
	IN_OTHER_EXCEPTION is the possible exception symbol of an incoming OTHER. May
	be EPSILON (meaning no exception).
	OUT_OTHER_EXCEPTION_PTR is the exception symbol of an output OTHER (may be 0,
	that is, EPSILON, meaning no such exception). The output OTHER itself is put
	into OUT_OTHER_EXCEPTION_PTR.
	*/
{
	ALPHABETptr sigma_this = NET_sigma(STRAT_CONTROL[net_order].net);
	* out_other_exception_ptr = EPSILON;
	* out_other_exception_sigma = net_order;
	if (! in_id ||
		(! (in_id == OTHER || arc_lower_id == OTHER || *arc_upper_id_ptr)))
		return FALSE;
	if ( ! sigma_this)
	{
		fprintf(stderr, "*** Mustn't happen. No sigma_this=%d.\n",
			(int) sigma_this); 
		exit(1);
	}
	if (in_id == OTHER)
	{
		if (arc_lower_id != OTHER)
		{
			if (in_other_exception &&
				arc_lower_id == in_other_exception)
			{
				return FALSE;
			}
			else
			{
				if((STRAT_CONTROL[net_order].other_exception_map)[in_other_exception_sigma]
				&&
					sigma_member((STRAT_CONTROL[net_order].other_exception_map)[in_other_exception_sigma],
					arc_lower_id)) /** then it is not a real OTHER
								   so not a match */
				{
					return FALSE;
				}					
				else 
				{
					if (*arc_upper_id_ptr == OTHER)
					{
						* out_other_exception_ptr = EPSILON;
					}
				}
			}
		}
		else 
		{
			if (* arc_upper_id_ptr == OTHER)
			{
				if (arc_id != OTHER) /* ?:? */
				{
					* out_other_exception_ptr = EPSILON;
				}
				else /* identity OTHER */
				{
					* arc_upper_id_ptr = in_id;
					* out_other_exception_ptr = in_other_exception; /** enough? **/
					* out_other_exception_sigma = in_other_exception_sigma; 
				}
			}
		}
	}

	else /* in_id != OTHER */
	{
		if (arc_lower_id == OTHER)
		{
			if (sigma_member(sigma_this, in_id))
			{
				return FALSE;
			}
			else
			{
				if (arc_id != OTHER) /* ?:? */
				{
					if (*arc_upper_id_ptr == OTHER)
					{
						* out_other_exception_ptr = in_id; 
					}
				}
				else /* identity OTHER */
				{
					*arc_upper_id_ptr = in_id;
				}
			}
		}
		else if (*arc_upper_id_ptr == OTHER)
		{
			* out_other_exception_ptr = EPSILON;
		}
	}
	return TRUE;
}

/* It is for tail token handling: at the end of produce_tail_tokens() of the n-th fst
we have already actualized the (n+1)-th actual config set. But it is not enough:
the (n+2)-nd must also be actualized because of a refinement in the
tail-token-related inheritance mechanism. 
*/

/* Here a structure is return-ed. If ineffective or nonstandard C change to ptr. */
static RES_CONFStype fst_subcomposition ( NVptr nv, 
	int net_order,
	CONFIGptr norm_config, /* of previous fst */
	int downcall_allowed)
{
	/* Produces an (output) tree of lookup possibilites representing the full
	set of matching ambiguities of some section of the input stream
	(represented by label_id-s at this stage already) as defined
	by (the actual) TOKEN_FST. The domain (upper side) of TOKEN_FST has the token string, 
	its range contains the surface text. Also it is guaranteed
	that none of the tokenization paths in a given fsm will be retracted by
	future date unless the whole text itself does not belong to the
	range of TOKEN_FST ( which cannot happen if RANGE(TOKEN_FST) is ?*). 
	Assumes that TOKEN_FST is epsilon free: no EPSILON, that is,
	EPSILON:EPSILON transitions.
	Output may be produced at points where  the corresponding transducer state
	is final or has a final state in its lower epsilon closure.
	*/

	STATEptr fst_state, dest;
	OUT_CELLptr output_cell, simili_o_chain ;
	CONFIGptr alt, config, cur_configs, next_configs = NULL,
		cf,  act_old, down_res_configs = NULL, d_r_c_tail = NULL ;
	int no_match = FALSE, first_fst = FALSE, first_match_of_this_symbol = TRUE, n,
		this_last_id = TRUE, inhibit_symbol_count_ancestry = FALSE,
		inhibit_tail_c_papa_ancestry = TRUE,
		seq_match = FALSE;
	RES_CONFStype rc, res_configs = { NULL,  NULL } ;
	ALPHABETptr low_eps, low_fld; 
	AVptr arc_vector;  /* fast */
	ARCptr arc;        /* slow */
	uint16 i, fst_id_lim; 
	id_type arc_id, txt_id, fst_id, up, low, *fst_ids, *seq, *norm_id_sequenceptr,
		attr, old_val, in_other_exception=EPSILON, out_other_exception=EPSILON, 
		in_other_exception_sigma=EPSILON, out_other_exception_sigma=EPSILON ;
	unsigned long int symbol_count = 0;
	RANGEptr range_map, range, eps_range;

#ifdef DEBUG
	NETptr token_fst=NV_net(nv,net_order);
#endif

	/* Set these variables so that we can quickly check whether a text
	symbol is in the lower alphabet of the transducer.
	*/

	fst_id_lim = ALPH_len(STRAT_CONTROL[net_order].lsigma );
	fst_ids = ALPH_items(STRAT_CONTROL[net_order].lsigma );  

	/* Lower input side is expected. A  network with an existing uprange_map is expected */
	if (! (range_map = NET_uprange_map (NV_net(nv,net_order))))
	{
		fprintf(stderr, "*** Shouldn't happen. No range map. \n");
		exit(1);
	}

	/* fetching text_id-s - symbol identifiers - from the previous normalizing fst if
	any */
	/* the norm_config's output_cell-s serve as input id-s for this lexical fst */

	if ( (net_order > 0)  && (! norm_config ))
		return (res_configs); /* no continuation */

	if ( ! net_order)
		first_fst = TRUE;

	if (norm_config)
	{
		/*  Create id sequence in norm. */
		/* NORM_ID_SEQUENCEptr = NORM_ID_SEQUENCE; */
		norm_id_sequenceptr = STRAT_CONTROL[net_order].norm_id_sequence;
		if ( CONFIG_output_cell(norm_config))
		{
			/* produce a token - an id sequence -  from the previous normalizing
			configuration. A single chain (a single config) is checked */
			norm_id_sequenceptr=read_tokens_norm_ids (CONFIG_output_cell(norm_config), 
				norm_id_sequenceptr);
		}
		*norm_id_sequenceptr = ID_EOS; /* end of token sequence */
		seq =  STRAT_CONTROL[net_order].norm_id_sequence;
	}
	else /* no previous normalizing transducer: read input directly */
	{
		seq = LOOKUP_ID_SEQUENCE;
	}

	/* Epsilon transition - that is, no input - from the normalizer fst. Config-s are
	actualized even if unchanged to save states for later or final tail-token epsilon
	transitions.
	*/
	if (norm_config  && (net_order > 0) && ( *seq == ID_EOS ))

		return (actualize_by_copy_former_descendants 
		(norm_config,  STRAT_CONTROL[net_order].prev_symbol_result,
		net_order, downcall_allowed));

	if ( (! downcall_allowed)  && (*(seq+1) == ID_EOS))
		inhibit_tail_c_papa_ancestry = FALSE;

	/* ---------- */

	/* ---- read the first symbol - either from input or from a previous fst --- */

	txt_id = *seq; 
	seq++;
	/** NEW 2000-1-25
	Handling input OTHER: if the cascade-previous fst has sent in an
	input OTHER (via norm_config), it must have a coupled "exception"
	symbol, e.g.: ?a (here OTHER can be anything but "a"). It may
	happen that: ?0 (OTHER EPSILON) occurs.  Then "?" will be the
	input symbol, and a read into a variable and the input pointer
	increased so it will point again to the next real input symbol.
	New: 2-10: now it is a triple: {OTHER, exception, exception-alphabet}
	**/
	if (txt_id == OTHER)
	{
		if ( (*seq) !=  ID_EOS)
		{
			in_other_exception = * seq;
			seq++;
			if ( (*seq) !=  ID_EOS)
			{
				in_other_exception_sigma = *seq;
				seq++;
			}
			else
			{
				in_other_exception_sigma = EPSILON;
#ifdef DEBUG
				fprintf (stderr,"** Lookup [-a 2]: Wrongly formed input\
								OTHER. Please report this.\n");
#endif	  
			}
		}
		else
		{
			in_other_exception = EPSILON;
#ifdef DEBUG
			fprintf (stderr,"** Lookup [-a 2]: Wrongly formed input\
							OTHER. Please report this.\n");
#endif
		}
	}
	symbol_count++;

	if  (first_fst)
		SYMBOL_COUNT++; /* ** Attention, GLOBAL variable */

	/* config sub-chain */
	if (STRAT_CONTROL[net_order].prev_symbol_result)
	{
		cur_configs = STRAT_CONTROL[net_order].prev_symbol_result;
	}
	else
		/* config sub-chain */
	{
		fprintf (stderr, "*** Must not happen - no passed down result to continue ***\n");
	}

	/* Now - looping on txt_id-s! */
	if (txt_id == ID_EOS) 
		return (res_configs); /* End of input. All eventual results are
							  complete already. End of this token handling. */
	else /* main symbol reading loop */
		do
		{		
			if (txt_id  == ID_EOS) /* ** what other security check? */
			{
				fprintf(stderr, "*** Shouldn't happen. No symbol at position.\n");
				break;
			}
#ifdef LKDEBUG_ADD
			/* ** Test, debug ** */
			fprintf(stdout, "Input symbol: ");
			print_label(txt_id, stdout, DONT_ESCAPE);
			fprintf(stdout, "\n");
#endif
			/* IF this fst has a prev result then continue that !*/

			for (config = cur_configs; config; config = CONFIG_next(config))
			{
				/* only configs with 1-level backward common ancestry are explored */

				if ( (norm_config && (SYMBOL_COUNT >1)) &&
					(! 
					inheritance_marks_match (norm_config, config, downcall_allowed)
					)
					) 
				{
					; /* otherwise: no brethren - no action */
				}
				else
				{
					fst_state = CONFIG_fst_state(config); 
					output_cell = CONFIG_output_cell(config);
					no_match = TRUE;

					/* If there are no arcs, the output state gets eliminated
					even if the fst_state is final because we are not at the
					end of text.
					*/

					/* Now we proceed to examine the arcs of fst_state.
					The arcs are either in vector or in linked list representation.
					The algorithm is replicated according to this condition to a
					"fast" and a "slow" part */

					if (STATE_vector_p(fst_state))

					{ /* fast part Start */
						arc_vector = STATE_arc_vector(fst_state);
						if (arc_vector)
						{		      
							/* If the text symbol is not in the transducer's alphabet,
							treat it as OTHER.
							*/
							if (!(txt_id < fst_id_lim && fst_ids[txt_id]))
								range = range_map + OTHER;
							else
								range = range_map + txt_id;
							if (txt_id == OTHER) /** NEW 2000-2-7 */
								range =  STRAT_CONTROL[net_order].range_of_OTHER;

							for (i = 0, seq_match = FALSE; i < RANGE_len(range); i++)
							{
								fst_id = RANGE_item(range, i);
								if ((dest = AV_dest(arc_vector, fst_id)))
								{
									/* Match moving forward. */
									up = upper_id(fst_id);
									low = lower_id(fst_id); /** NEW 2000-2-4 */

									if (low == OTHER || up == OTHER || txt_id == OTHER)
									{
										if (! match_other(net_order,
											txt_id, fst_id, low, &up,
											in_other_exception,
											&out_other_exception,
											in_other_exception_sigma,
											&out_other_exception_sigma))
											continue;
									}	 
									seq_match = TRUE;

#ifdef LKDEBUG_ADD
									/* ** Test, debug ** */
									debug_printinfo_branch("Arc label-fast: ", fst_id, up, net_order);
#endif
									/* check if its simili exists; if yes, that is, if
									there is a new config with the same fst state and
									the same chain then only link this config to the
									output chain of the existing one. 
									But it can't be deleted because we don't know which of
									the duplicates will survive; however, the number of
									output chains is reduced to a single one */

									if(is_epsilon_plus(up)) /* */
									{
										add_branch_with_same_o_chain
											(output_cell, new_next_config(&next_configs, dest,config));
									}
									else
									{
										/** 2000-2-1 new: OTHER as output! */
										if (up == OTHER &&
											((NET_NO - net_order -1) > 0))
											add_other_branch(out_other_exception,
											out_other_exception_sigma,
											output_cell,
											new_next_config(&next_configs, dest,config));
										else
										{
											if ( (simili_o_chain = has_identical_config 
												(up, output_cell, next_configs))
												||
												(simili_o_chain = has_identical_config 
												(up, output_cell,
												STRAT_CONTROL[net_order].temp_result)))
												add_branch_with_same_o_chain
												(simili_o_chain, new_next_config(&next_configs,dest,config));
											else
												add_branch(up, output_cell,
												new_next_config(&next_configs, dest,config));
										}
									}
									/* --- */
									/* Setting inheritance marks from both previous symbol
									and preceding fst in the cascade  */
									set_inheritance_marks (&next_configs, config,
										norm_config,
										downcall_allowed,
										inhibit_symbol_count_ancestry,
										inhibit_tail_c_papa_ancestry,
										net_order); 
									/* Now we call the next fst in the cascade. If this (or
									its successors) doesn't produce a result it is not a
									real match then it can be deleted immediately  */ 
									if ( downcall_allowed && ((NET_NO - net_order -1) > 0)) /* next fst exists */
									{
										if ( ( (*seq) == ID_EOS ) || first_fst)
											this_last_id = TRUE;
										else
											this_last_id = FALSE; 

										/* if finished in this fst ONLY then call others */
										if (this_last_id)
										{

											/* check  if any prospect by next */
											rc = fst_subcomposition 
												( nv,
												net_order+1,
												next_configs, /* check the newest only */
												/* previous temporary results from previous
												input symbols must be built further */
												downcall_allowed);

											if (rc.head) /* it is worth to go on */
											{
												no_match = FALSE;
												if (first_match_of_this_symbol)
												{
													res_configs.tail = next_configs;
													first_match_of_this_symbol = FALSE;

													/* if (! d_r_c_tail) *//* link back to
																		   antecedens at this symbol */
													if (this_last_id)
														d_r_c_tail = rc.tail;
												}
												/* Unify temp results for later */
												CONFIG_next(rc.tail) = down_res_configs;
												down_res_configs = rc.head;
											}
											else /* further fst-s in the strategy didn't
												 accept this match so forget it */
											{
												cf =  CONFIG_next (next_configs);
												free_config_and_output_cells(next_configs);
												next_configs = cf;
											}
										}
									}
									else /* last fst in the cascade - no downcall */
									{
										no_match = FALSE;
										if  (first_match_of_this_symbol)
										{
											res_configs.tail = next_configs;
											first_match_of_this_symbol = FALSE;
										}
									}
									/* --- */
									/* IF SEQUENTIALIZED: Major change: loop finished if match found */
									if (NET_lower_seq(NV_net(nv,net_order)) && seq_match)
										break;
								}
							}

							if ((low_eps = AV_l_eps(arc_vector)))
							{
								/* Process arcs that have an EPSILON on the lower side.
								Staying in the current position. Any new
								config gets stuck on end, so eventually it will be
								processed at this string position.
								*/

								for (i = 0; i < ALPH_len(low_eps); i++)
								{
									fst_id = ALPH_item(low_eps, i);
									up = upper_id(fst_id);
									dest = AV_dest(arc_vector, fst_id);
#ifdef LKDEBUG_ADD			      
									/* ** Test, debug ** */
									debug_printinfo_branch("Arc label-fast-eps: ",
										fst_id, up, net_order);
#endif
									/** 2000-2-1 new: OTHER as output! */
									if (up == OTHER &&
										((NET_NO - net_order -1) > 0))
										add_other_branch(EPSILON, net_order, output_cell,
										new_cur_config(config,
										dest));
									else

										add_branch(up, output_cell,
										new_cur_config(config, dest));
								}
							}
							if (NET_l_flag_diacr(NV_net(nv,net_order)) &&
								( low_fld = AV_l_fld(arc_vector)))
							{
								for (i = 0; i < ALPH_len(low_fld); i++)
								{	
									fst_id = ALPH_item(low_fld, i);
									up = upper_id(fst_id);
									low = lower_id(fst_id);
									dest = AV_dest(arc_vector, fst_id);			
									if ( (attr = flag_diacritic_attrib(low)))
									{
										old_val = flag_value(CONFIG_flag_d_reg(config), attr);
										if (flag_action (CONFIG_flag_d_reg(config), flag_diacritic(low)))
										{
											if ( ! is_epsilon_plus_plus (up) )
												add_branch
												(up, output_cell,
												new_cur_config(config,dest));
											else /* this is a hack we know EPSILON == 0  */
												add_branch_with_same_o_chain 
												(output_cell, 
												new_cur_config(config,dest));
											no_match = FALSE;		      
										}
										flag_value(CONFIG_flag_d_reg(config), attr) = old_val;
									}
								}
							}
						}
					}/* fast part end */

					else /* slow part Begin: this fst state is not vectorized */

						/* This is the same algorithm as the one above ("fast part") except
						that it does not convert arc chains to vectors. Consequently, it
						matches the id of a text symbol by going through the arcs in the
						tokenizer transducer one by one. The fast tokenizer uses array
						lookup for direct access.
						*/
						for (arc = STATE_arc_set(fst_state), seq_match = FALSE; arc; arc = next_arc(arc))
						{
							/** IF SEQUENTIALIZED: loop finished if match found  **/
							if (NET_lower_seq(NV_net(nv,net_order)) && seq_match)
								break;

							if (NET_reduced_labelset(NV_net(nv,net_order))) /* reduced labelset */
							{
								eps_range = RANGE_VECTOR_vector(MATCH_TABLE_table(NET_upmatch_table(NV_net(nv,net_order)))[ARC_label(arc)]);

								if ( (txt_id < fst_id_lim  && fst_ids[txt_id]) &&
									txt_id <  RANGE_VECTOR_len(MATCH_TABLE_table(NET_upmatch_table(NV_net(nv,net_order)))[ARC_label(arc)]))
									range = eps_range + txt_id;
								else
									range = eps_range + OTHER;

								if (!(range && range->items))
									range = eps_range;

								i = 0;

								if (range && range->items)
									arc_id = RANGE_item(range, i);
								else
									arc_id = EPSILON;
							}
							else /* labelset not reduced  */
							{
								eps_range = range = 0;
								arc_id = ARC_label(arc);
							}

							do /** slow inner loop 99-4-30 */
							{      
								if (arc_id)
								{
									up = upper_id(arc_id);
									low = lower_id(arc_id);

									if ((txt_id == low ||
										(low == OTHER && !(txt_id < fst_id_lim && fst_ids[txt_id])))
										/** New 2000-1-24: OTHER can be input, too **/
										|| (txt_id == OTHER && (! is_epsilon_plus_plus(low))))
									{
										/* Match moving forward. */
										if (low == OTHER || up == OTHER || txt_id == OTHER)
										{
											if (! match_other(net_order,
												txt_id, arc_id, low, &up,
												in_other_exception,
												&out_other_exception,
												in_other_exception_sigma,
												&out_other_exception_sigma))
												continue;
										}	 
										seq_match = TRUE;
#ifdef LKDEBUG_ADD
										/* ** Test, debug ** */
										debug_printinfo_branch("Arc label-sl: ",
											arc_id, up, net_order);
#endif
										if ( is_epsilon_plus(up)) /* */
											add_branch_with_same_o_chain
											(output_cell,
											new_next_config(&next_configs,
											ARC_destination(arc),config));
										else
										{ /** 2000-2-1 new: OTHER as output! */
											if (up == OTHER &&
												((NET_NO - net_order -1) > 0))
											{
												add_other_branch(out_other_exception, out_other_exception_sigma,
													output_cell,
													new_next_config(&next_configs,
													ARC_destination(arc),config));
											}
											else
											{
												if( (simili_o_chain = has_identical_config 
													(up, output_cell, next_configs))
													||
													(simili_o_chain = has_identical_config 
													(up, output_cell,
													STRAT_CONTROL[net_order].temp_result)))
													add_branch_with_same_o_chain
													(simili_o_chain, 
													new_next_config(&next_configs, 
													ARC_destination(arc),config));
												else
													add_branch(up, output_cell,
													new_next_config(&next_configs,
													ARC_destination(arc),config));
											}
										}
										/* --- */
										/* Setting inheritance marks from both previous symbol
										and preceding fst in the cascade  */
										set_inheritance_marks (&next_configs, config,
											norm_config,
											downcall_allowed,
											inhibit_symbol_count_ancestry,
											inhibit_tail_c_papa_ancestry,
											net_order); 
										/* Now we call the next fst in the cascade. If this (or
										its successors) doesn't produce a result it is not a
										real match then it can be deleted immediately  */ 
										if ( downcall_allowed && ((NET_NO - net_order -1) > 0)) /* next fst exists */
										{
											if ( ((*seq) == ID_EOS ) || first_fst)
												this_last_id = TRUE;
											else
												this_last_id = FALSE; 

											/*  if finished in this fst ONLY then call others */
											if (this_last_id)
											{

												/* check  if any prospect by next */
												rc = fst_subcomposition 
													( nv,
													net_order+1,
													next_configs, /* check the newest only */
													/* previous temporary results from previous
													input symbols must be built further */
													downcall_allowed);

												if (rc.head) /* it is worth to go on */
												{
													no_match = FALSE;
													if (first_match_of_this_symbol)
													{
														res_configs.tail = next_configs;
														first_match_of_this_symbol = FALSE;

														/* if (! d_r_c_tail) *//* link back to
																			   antecedens at this symbol */
														if (this_last_id)
															d_r_c_tail = rc.tail;
													}
													/* Unify temp results for later */
													CONFIG_next(rc.tail) = down_res_configs;
													down_res_configs = rc.head;
												}
												else /* further fst-s in the strategy didn't
													 accept this match so forget it */
												{
													cf =  CONFIG_next (next_configs);
													free_config_and_output_cells(next_configs);
													next_configs = cf;
												}			 
											}
										}
										else /* last fst in the cascade - no downcall */
										{
											no_match = FALSE;
											if  (first_match_of_this_symbol)
											{
												res_configs.tail = next_configs;
												first_match_of_this_symbol = FALSE;
											}
										}
										/* --- */
									}
									/* Epsilon-s or ALTCHAIN-s */
									else if (is_epsilon_plus(low))
									{
										/* Epsilon. Staying in the current position. Any new
										config gets stuck on end, so eventually it will be
										processed at this string position.
										Altchain_symbol-s are handled similarly but they are not
										allowed to become "next_config-s".
										*/ 
#ifdef LKDEBUG_ADD
										/* ** Test, debug ** */
										debug_printinfo_branch("Arc label-sl-eps: ",
											arc_id, up, net_order);
#endif
										if (is_altchain (up))
										{
											add_branch_with_same_o_chain
												(output_cell, 
												new_cur_config(config, ARC_destination(arc))
												);
										}
										else
										{
											/** 2000-2-1 new: OTHER as output! */
											if (up == OTHER &&
												((NET_NO - net_order -1) > 0))
												add_other_branch(EPSILON, net_order, output_cell,
												new_cur_config(config,
												ARC_destination(arc)));				       
											else
												add_branch(up, output_cell,
												new_cur_config(config,
												ARC_destination(arc)));
										}
									}
									/* Flag diacritics begin */
									else 
										if ( NET_l_flag_diacr(NV_net(nv,net_order)) &&
											flag_diacritic_p (low) &&
											(attr = flag_diacritic_attrib(low)))
										{
											/* if Flag Diacritics allow then */
											old_val = flag_value(CONFIG_flag_d_reg(config), attr);
											if (flag_action (CONFIG_flag_d_reg(config), flag_diacritic(low)))
											{
												if ( ! is_epsilon_plus_plus (up) )
													add_branch
													(up, output_cell,
													new_cur_config(config, ARC_destination(arc)));
												else /* this is a hack we know EPSILON == 0  */
													add_branch_with_same_o_chain 
													(output_cell, 
													new_cur_config(config,ARC_destination(arc)));
												no_match = FALSE;		      
												/* copy flag_diacritic_register to new*/
											}
											flag_value(CONFIG_flag_d_reg(config), attr) = old_val;
										} /* Flag diacritics end */
										/* IF SEQUENTIALIZED: Major change: loop finished if match found */
										if (NET_lower_seq(NV_net(nv,net_order)) && seq_match)
											break;
								}	   
								/* if reduced labelset: further loop on hidden (virtual) arcs*/ 
								if (NET_reduced_labelset(NV_net(nv,net_order)))
								{
									i++;
									if (! (i < RANGE_len(range)) && 
										(range != eps_range))
									{
										range = eps_range;
										i = 0;
									}

									if (range && i < RANGE_len(range) && RANGE_items(range))
										arc_id = RANGE_item(range, i);
									else
										arc_id = EPSILON;
								}
							} while (range && i < RANGE_len(range)); 
							/** END NEW slow inner loop 99-4-30 */
						}  /*  slow part End */

						if (no_match)
						{
							/* This path died. Output is cleared but config with its
							fst_state remains for closure on future epsilon matches.
							But if part of the output chain belongs to the inherited
							config then the output chain section belonging to  any
							inherited config must remain.
							NULL as output state informs add_branch() not to bother
							to add additional arcs for those matches because we already
							know there is no future.
							*/
							if ( (CONFIG_count(config) == SYMBOL_COUNT) ) /* don't destroy too much */
							{
								if (CONFIG_output_cell(config))
									CONFIG_output_cell(config)->refcount--; /* new: */
								free_output_cell(CONFIG_output_cell(config));
								CONFIG_output_cell(config) = NULL ;
							}
						}
				}
			} /* end of "for" on config-s */

			/*  Now all the configurations can be freed. The possible continuations - if
			any - are in next_configs */

			/* DON'T FREE configs from previous runs! - except at the first fst */
			for (config = cur_configs, act_old = NULL; config; config = alt)
			{
				alt = CONFIG_next(config);
				if (! first_fst)
				{
					if (free_config_and_output_cells_onlyif_newsymbol(config))
					{
						if (act_old)
							CONFIG_next(act_old) = NULL; /* unfinished: waits for a not-deleted-next
														 next config if any */
					}
					else
					{
						if ( (cf = act_old))
							CONFIG_next(cf) = config;
						act_old = config;
					}
				}
				else /* the first fst loops only once - it calls others at ech match */
				{  
					free_config_and_output_cells(config);
				}
			}

			cur_configs = next_configs;
			next_configs = NULL;

			/* --- Reading further symbols. --- */
			txt_id = * seq;
			seq++; 
			if (txt_id == OTHER)/* NEW 2000-2-1 **/
			{
				if ( (*seq) !=  ID_EOS)
				{
					in_other_exception = * seq;
					seq++;
					if ( (*seq) !=  ID_EOS)
					{
						in_other_exception_sigma = *seq;
						seq++;
					}
					else
					{
						in_other_exception_sigma = EPSILON;
#ifdef DEBUG
						fprintf (stderr,"** Lookup [-a 2]: Wrongly formed input\
										OTHER. Please report this.\n");
#endif	  
					}
				}
				else
				{
					in_other_exception = EPSILON;
#ifdef DEBUG
					fprintf (stderr,"** Lookup [-a 2]: Wrongly formed input\
									OTHER. Please report this.\n");
#endif
				}
			}
			symbol_count++;
			first_match_of_this_symbol = TRUE;

			/* --- setting inheritance mark governing flags --- */
			/* --- accumulating temporary results --- */
			if  (! first_fst)
			{
				inhibit_symbol_count_ancestry = TRUE; /* from now on, certain intermediate
													  configs-s simply inherit the s_papa
													  of their s_papa! */
				if (downcall_allowed)
				{
					if (txt_id != ID_EOS)
						d_r_c_tail = NULL; /* ** new */
					else /* end of this local symbol reading  */
					{ /* unifying temporary results of follower fst*/
						if (this_last_id && d_r_c_tail && ( (NET_NO - net_order -1) > 0))
						{
							CONFIG_next(d_r_c_tail) = STRAT_CONTROL [net_order + 1].temp_result;
							STRAT_CONTROL [net_order + 1].temp_result = down_res_configs;
						}
					}
				}
				else /* tail token */
				{
					/*  governing tail token inheritance marks: */
					if ( *seq == ID_EOS ) /* if last symbol */
						inhibit_tail_c_papa_ancestry = FALSE;
				}
			}
			else  /* if  (first_fst) */
			{	
				/* store temporary results for follower at the end of processing
				this symbol for this fst - but if it was the first do nothing.
				*/
				if ( downcall_allowed && (( NET_NO > 1) )) /* first fst; and there were others, too */
					/* now all intermediate symbol results can be actualized BELOW
					*/
				{
					/* Unify antecendent results for the 2nd fst, too,*/
					STRAT_CONTROL [net_order + 1].temp_result = down_res_configs;
					down_res_configs = NULL;

					for (n=1;      n < NET_NO ;  	       n++)
					{
						/* Now all "previous temporaries" can be freed */
						for (config =STRAT_CONTROL[n].prev_symbol_result; config; config = alt)
						{
							alt = CONFIG_next(config);
							free_config_and_output_cells_onlyif_oldsymbol(config);
						}
						STRAT_CONTROL[n].prev_symbol_result = STRAT_CONTROL[n].temp_result;
						STRAT_CONTROL[n].temp_result = NULL; 
					}
				}

				SYMBOL_COUNT++; /* ** Attention, GLOBAL variable */
#ifdef LKDEBUG_ADD
				fprintf(stdout, "SYMBOL_COUNT = %d \n", SYMBOL_COUNT);
				fflush(stdout);
#endif
			}
		} while (txt_id != ID_EOS); /* end of symbol reading loop */

		res_configs.head = cur_configs; 
		return(res_configs);
}

/*-----------------------------------------------------------*/

ALPHABETptr lower_sigma_tg(NETptr net)
{
	/*
	This is a HACK to correct lower_sigma () (see also side_sigma and upper_sigma() )
	when this side of sigma has OTHER and the original other symbols as well and labels
	only contain OTHER - then these other symbols (in sigma) count as OTHER-s, too !
	17.07.96 TG */
	ALPHABETptr ls;

	ls = lower_sigma(net);
	if (binary_sigma_member (ls, OTHER))
	{
		free_alph(ls);
		ls = sigma(net);
	}
	return (ls);
}

ALPHABETptr lower_sigma_of_net (NETptr net)
{
	int i;
	for (i=0; i< NV_len (LEXNETS); i++)
	{
		if (LOWER_SIGMAS_LEXNETS[i].net == net)
			return (LOWER_SIGMAS_LEXNETS[i].low_sig);
	}
	return(NULL);
}

lower_sigma_netptr make_lower_sigmas_all_networks ()
{
	/* Make lower sigmas of all networks of the net vector 'LEXNETS'.
	LOWER_SIGMAS_LEXNETS will contain an array of lowsig-net pairs.
	*/

	int i; 

	if (LEXNETS != NULL)
	{
		LOWER_SIGMAS_LEXNETS = calloc( NV_len (LEXNETS), sizeof(lower_sigma_net));

		for (i=0; i< NV_len (LEXNETS); i++)
		{
			LOWER_SIGMAS_LEXNETS[i].net = NV_net (LEXNETS, i);
			LOWER_SIGMAS_LEXNETS[i].low_sig = lower_sigma_tg(NV_net (LEXNETS, i));
		}
	}
	else
		LOWER_SIGMAS_LEXNETS= NULL;

	return LOWER_SIGMAS_LEXNETS;
}

void free_lower_sigmas_nets (lower_sigma_netptr nets, int n)
{
	if (nets != NULL)
	{
		int i; 
		for (i=0; i< n; i++)
		{
			free_alph(nets[i].low_sig);
		}
		free(nets);
	}
}

void free_lower_sigmas_all_networks ()
{
	free_lower_sigmas_nets(LOWER_SIGMAS_LEXNETS, NV_len (LEXNETS));
	LOWER_SIGMAS_LEXNETS = NULL;
}

/* Alternative lookup algorithm  functions above Tamas Gaal */
/*==========================================================*/


/*===========================================================
OPEN AND CLOSE NETWORK FILES
===========================================================*/

void open_network_files (void* parseur)
{
	char line [L_LINE+2];  int idx = 0;

	//rewind (TEMP_FILES.netfile_names);
	//while (lookup_read_line (line, L_LINE, TEMP_FILES.netfile_names))
	while(idx<TEMP_FILES.netfile_names->max)
	{
		if (! (NET_FILES.file [idx] = fopen (TEMP_FILES.netfile_names->buffer[idx], "rb")))
		{
#ifdef DEBUG
			fprintf(stderr, "FOPEN_MAX (minimal number of files open by stdio) in this system is %d .\n", FOPEN_MAX);
			fprintf(stderr, "Please check and possibly increase by the C shell command `limit' the\n\
							maximal allowed number of file descriptors.\n\
							Upper (C library) stdio limit is 255.\n");
#endif
			sprintf(buffer_erreur, "network file '%s' could not be found or opened", TEMP_FILES.netfile_names->buffer[idx]);
			message_erreur(parseur,buffer_erreur);

		}
		idx++;
	}
}

void close_network_files ()
{
	int i;

	for (i=0; i< NET_FILES.num; i++)
		fclose (NET_FILES.file [i]);
}

/*===========================================================
READ AND FREE NETWORKS
===========================================================*/

NETptr read_first_nets_only (FILE *file)
{
	NETptr nets [2];  NVtype nv;
	unsigned long byte_count = 0;
	STANDARD_HEADERptr header;
	int i, net_count;

	if (! (header = get_header (file, &byte_count)))
		return (NULL);

	net_count = (int) HEADER_net_count (header);

	if (LOOKUP_FLAGS.checknet_from_file)
		nv.len = min_of (2, net_count);
	else
		nv.len = 1;

	nv.nets = nets;

	for (i = 0; i < nv.len; i++) {
		if (!LOOKUP_FLAGS.vectorize_nets) /** shared arc doesn't work together with vectorization tg**/
			nets[i] = SHARE_make_shared_arclists
			(network_from_stream(file, HEADER_file_type(header)), DONT_COPY);
		else
			nets[i] = network_from_stream(file, HEADER_file_type(header));
	}

	/*  set_header_file_name(NULL, filename); */
#ifndef SMALL_FSM
	convert_twol_nets(&nv, TRUE);
#endif

	if (nv.len == 2)
		CHK_chknet (nets [0]) = nets [1];
	else
		CHK_chknet (nets [0]) = NULL;

	return (*nets);
}

/*-----------------------------------------------------------*/
void get_networks (void* parseur)
{
	NETptr net;  int i;

	alloc_net_file_vector (TEMP_DATA.num_nets);
	open_network_files (parseur);

	LEXNETS = make_nv (TEMP_DATA.num_nets);

	for (i=0; i< NV_len (LEXNETS); i++)
	{
		if (! (net = read_first_nets_only (NET_FILES.file [i]))) {
			sprintf(buffer_erreur,parseur,"file %d does not contain any network", i);
			message_erreur (parseur,buffer_erreur);
		}

		if (ALPH_type (NET_sigma (net)) != BINARY_VECTOR)
			label_to_binary (NET_sigma (net));

		NV_net (LEXNETS, i) = net;
	}

	close_network_files ();
	free_net_file_vector ();
}

void get_strategies () {
	int i, s, len, *strategy, *pt;

	//rewind (TEMP_FILES.strategies);
	TEMP_FILES.strategies->i=0;
	STRATEGIES = make_nvv (TEMP_DATA.num_strategies);

	strategy = (int *) mem_alloc
		((TEMP_DATA.max_len_strategy + 1) * sizeof (int));

	for (s=0; s< NVV_len (STRATEGIES); s++) {

		/* fill strategy vector form temp file */

		for (pt = strategy; ; pt++) {
			//fread (pt, sizeof (int), 1, TEMP_FILES.strategies);
			if (TEMP_FILES.strategies->i>=TEMP_FILES.strategies->max)
				break;
			*pt=TEMP_FILES.strategies->buffer[TEMP_FILES.strategies->i];
			TEMP_FILES.strategies->i++;
			if (*pt == NO_INDEX)   /* end of strategy */
				break;
		}

		/* make a net vector for the strategy */

		len = pt - strategy;
		NVV_nv (STRATEGIES, s) = make_nv (len);

		for (i=0; i< len; i++)
			NV_net (NVV_nv (STRATEGIES, s), i) = NV_net (LEXNETS, strategy [i]);
	}

	free (strategy);
}

/*-----------------------------------------------------------*/
void free_all_networks ()
{
	free_nv_and_nets (LEXNETS);
	free_nvv_and_nvs_only (STRATEGIES);
}

/*-----------------------------------------------------------*/
void vectorize_all_networks ()
{
	/* Vectorizes all dense state (with more than n arcs) of all
	networks of the net vector 'LEXNETS'. */

	NETptr net;  int i, num;  float rate;

	for (i=0; i< NV_len (LEXNETS); i++)
	{
		net = NV_net (LEXNETS, i);
		num = vectorize_dense_states_of_net (net, LOOKUP_FLAGS.vectorize_min);
		rate = num * 100;  rate /= NET_num_states (net);

		if (LOOKUP_FLAGS.show_comments)
			fprintf (stderr, "transducer %d:\t%d out of %li states vectorized\
							 (%.2f %%)\n", i, num, NET_num_states (net), rate);

		if (! (net = CHK_chknet (net)))    /* transducer has no checknet */
			continue;

		/* num = CHK_vectorize_check_net (net); */
		num = vectorize_dense_states_of_net (net, LOOKUP_FLAGS.vectorize_min);
		rate = num * 100;  rate /= NET_num_states (net);

		if (LOOKUP_FLAGS.show_comments)
			fprintf (stderr, " check net %d:\t%d out of %li states vectorized\
							 (%.2f %%)\n", i, num, NET_num_states (net), rate);
	}
}

/*-----------------------------------------------------------*/
void recode_eps_range_map (NETptr net, int input_side)
{
	RANGEptr eps_range;  
	ALPHABETptr labels, eps_labs;
	id_type *match, id;  
	int i, eps_count;

	if (NET_range_len (net) < EPSILON + 1)
		error_message ("range map cannot be recoded. WARN THE PROGRAMMER !!!");

	if (input_side == UPPER)
	{
		eps_range = NET_downrange_map (net) + EPSILON;
		match = UPPER_MATCH;
	}
	else
	{
		eps_range = NET_uprange_map (net) + EPSILON;
		match = LOWER_MATCH;
	}

	labels = NET_labels (net);
	eps_labs = make_alph (ALPH_len (labels), LABEL_VECTOR);
	ALPH_len (eps_labs) = 0;

	for (i=0; i< ALPH_len (labels); i++)
	{
		id = match [ALPH_item (labels, i)];

		if ((id == EPSILON) || (id == ALTCHAIN_SYMBOL) ||
			(flag_diacritic_p (id)) )
			ALPH_item (eps_labs, ALPH_len (eps_labs)++) = ALPH_item (labels, i);
	}

	eps_count = ALPH_len (eps_labs);
	free (RANGE_items (eps_range));          /* free old items */

	RANGE_id (eps_range) = EPSILON;
	RANGE_len (eps_range) = eps_count;
	RANGE_items (eps_range) = (id_type *) malloc (eps_count * sizeof (id_type));

	for (i=0; i< eps_count; i++)
		RANGE_item (eps_range, i) = ALPH_item (eps_labs, i);

	free_alph (eps_labs);
}

void make_range_map_for_every_net (int input_side, int lookup_algorithm)
{
	/* Makes a range map for the input side of every network
	of the net vector 'LEXNETS'. */

	NETptr net;  int i;

	update_match_tables ();
	for (i=0; i< NV_len (LEXNETS); i++)
	{
		net = NV_net (LEXNETS, i);
		make_range_map (net, input_side);

		if (lookup_algorithm == 0)   /* depth-first algorithm */
			recode_eps_range_map (net, input_side);

		if (input_side == UPPER)
			NET_uprange_map (net) = NET_downrange_map (net);
		/* The depth-first alogoritms always works with NET_uprange_map(net).
		Therefore we move the NET_downrange_map(net) here. */
	}
}

/*===========================================================
WRITE A RESULT TO THE OUTPUT
===========================================================*/

void write_buffer_to_output (char *bf_end)
{
	*bf_end = '\0';

	if (! LOOKUP_FLAGS.dont_copy_input)
		fprintf (LOOKUP_OUTPUT_FILE, "%s", LOOKUP_INPUT_WORD);

	fprintf (LOOKUP_OUTPUT_FILE, "%s%s\n",
		LOOKUP_FLAGS.surf_lemma_sep, LOOKUP_OUTPUT_BUFFER);
}

int append_label_to_buffer_no_error (id_type id, char *bf_end, char **new_bf_end, int sep_p)
{
	/* Appends the name of the label to the string that is already in
	the buffer.
	Introduces a separator before a label whose print name is longer
	than one fat_character if sep_p==TRUE. */

	int len, sep_len;
	FAT_STR name = id_to_atom_name(id);

	if (LABEL_arity (id_to_label (id)) != 1)
		return -1;

	if (id == EPSILON)
		return -1;

	if (id == OTHER)
		return -1;

	/* Introduce an OUTPUT_SEPARATOR if required. Note: in UTF-8
	non-ASCII labels print out as several characters but 
	no separator is required in that case.
	To know if we write down a "real" multichar symbol,
	no matter of the encoding mode, we test the length
	of the fat_string print name contained in "name":  */
	if ((fat_strlen(name) > 1) && (sep_p))
	{
		sep_len = strlen (LOOKUP_FLAGS.lemma_tag_sep);
		/* Copy the separator string into the buffer: */
		memmove (bf_end, LOOKUP_FLAGS.lemma_tag_sep, sep_len);

		bf_end += sep_len;
	}

	len = LOOKUP_FLAGS.len_inp_line - 1 - (bf_end - LOOKUP_OUTPUT_BUFFER);
	len = FST_fat_to_thin_name (bf_end, name, len, DONT_ESCAPE);

	if (len < 0)
		error_message ("output buffer overflow");

	*new_bf_end= bf_end + len;

	return (fat_strlen(name) > 1);
}

int append_label_to_buffer (id_type id, char *bf_end, char **new_bf_end, int sep_p)
{
	/* Appends the name of the label to the string that is already in
	the buffer.
	Introduces a separator before a label whose print name is longer
	than one fat_character if sep_p==TRUE. */

	int len, sep_len;
	FAT_STR name = id_to_atom_name(id);

	if (LABEL_arity (id_to_label (id)) != 1)
		error_message ("result contains an ID with arity != 1");

	if (id == EPSILON)
		error_message ("result contains an ID == EPSILON");

	if (id == OTHER)
		error_message ("result contains an ID == OTHER");

	/* Introduce an OUTPUT_SEPARATOR if required. Note: in UTF-8
	non-ASCII labels print out as several characters but 
	no separator is required in that case.
	To know if we write down a "real" multichar symbol,
	no matter of the encoding mode, we test the length
	of the fat_string print name contained in "name":  */
	if ((fat_strlen(name) > 1) && (sep_p))
	{
		sep_len = strlen (LOOKUP_FLAGS.lemma_tag_sep);
		/* Copy the separator string into the buffer: */
		memmove (bf_end, LOOKUP_FLAGS.lemma_tag_sep, sep_len);

		bf_end += sep_len;
	}

	len = LOOKUP_FLAGS.len_inp_line - 1 - (bf_end - LOOKUP_OUTPUT_BUFFER);
	len = FST_fat_to_thin_name (bf_end, name, len, DONT_ESCAPE);

	if (len < 0)
		error_message ("output buffer overflow");

	*new_bf_end= bf_end + len;

	return (fat_strlen(name) > 1);
}

void write_result_net_to_output (STATEptr state, char *bf_end, int sep_p)
{
	ARCptr arc;  char *new_end;

	if (STATE_final (state))
		write_buffer_to_output (bf_end);

	for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
	{
		append_label_to_buffer (ARC_label (arc), bf_end, &new_end, sep_p);

		if ((sep_p) && (new_end - bf_end <= 1))
			write_result_net_to_output
			(ARC_destination (arc), new_end, TRUE);
		else
			write_result_net_to_output
			(ARC_destination (arc), new_end, FALSE);
	}
}

void write_id_seq_to_output (id_type *seq)
{
	char *bf_end, *new_end;  
	int sep_p = TRUE, is_multi_char= FALSE;

	for (bf_end = LOOKUP_OUTPUT_BUFFER; *seq != ID_EOS; seq++, bf_end = new_end)
	{
		is_multi_char= append_label_to_buffer(*seq, bf_end, &new_end, sep_p);

		/* Don't put separators once a first multichar
		has been written out to the buffer:  */
		if (is_multi_char)
			sep_p = FALSE;
	}

	write_buffer_to_output (bf_end);
	LOOKUP_RESULT = (NETptr) NULL+1;   /* mark that a result was found */
}


//Added by C.R.
void write_id_seq_to_buffer (id_type *seq,void* parseur) {
	char *bf_end, *new_end;  
	int sep_p = TRUE, is_multi_char= FALSE;

	for (bf_end = LOOKUP_OUTPUT_BUFFER; *seq != ID_EOS; seq++, bf_end = new_end) {    
		is_multi_char= append_label_to_buffer_no_error(*seq, bf_end, &new_end, sep_p);

		/* Don't put separators once a first multichar
		has been written out to the buffer:  */
		if (is_multi_char)
			sep_p = FALSE;
		if (is_multi_char==-1)
			break;
	}

	insert_token(LOOKUP_INPUT_WORD,parseur);
	insert_token_char(9,parseur,0);
	add_token(LOOKUP_OUTPUT_BUFFER, parseur);
	if (is_multi_char==-1 && CheckEditDistanceAction(parseur,action_prefix_detected)) {
		insert_token_char(9,parseur,0);
		add_token("+PREFIX", parseur);
		insert_all_id_sequence(parseur,1);
	}
	else {
		if (CheckEditDistance(parseur)==1)
			insert_all_id_sequence(parseur,0);
	}
	LOOKUP_RESULT = (NETptr) NULL+1;   /* mark that a result was found */
}

/*===========================================================
PREPARE PARSE TABLE, ID VECTOR, MATCH TABLES
===========================================================*/

static int max_label_length_in_alph (ALPHABETptr alph, int side)
{
	/* Returns the maximal length of the print names of the labels
	on the given side (UPPER, LOWER or BOTH_SIDES). Note that
	non-ASCII labels (> 127) are represented by two characters
	in the input if we are in UTF-8 mode. In that case, a
	a parse table must be built even if there are no multicharacter
	symbols in the alphabt. */

	int i, len, maxlen;

	for (i=0, maxlen = 0; i< ALPH_len (alph); i++)
	{
		if (side == UPPER)
			len = symbol_length (upper_id (ALPH_item (alph, i)));
		else if (side == LOWER)
			len = symbol_length (lower_id (ALPH_item (alph, i)));
		else if (side == BOTH_SIDES)
			len = max_of (symbol_length (upper_id (ALPH_item (alph, i))),
			symbol_length (lower_id (ALPH_item (alph, i))));

		maxlen = max_of (maxlen, len);
	}

	return (maxlen);
}

static ALPHABETptr make_common_label_alph (NVptr nv)
{
	/* Returns a lable alphabet that is common to all networks of nv.
	The labels alphabets of all nets are supposed to be of type
	LABEL_VECTOR. */

	ALPHABETptr common_labels, labels;
	int i, a, n, len;

	for (i=0, len=0; i< NV_len (nv); i++)
	{
		labels = NET_labels (NV_net (nv, i));

		if (ALPH_type (labels) != LABEL_VECTOR)
			binary_to_label (labels);

		len += ALPH_len (labels);
	}

	common_labels = make_alph (len , LABEL_VECTOR);

	/* copy all labels to common_labels; repetitions are allowed */

	for (i=0, a=0; i< NV_len (nv); i++)
	{
		labels = NET_labels (NV_net (nv, i));

		for (n=0; n< ALPH_len (labels); n++)
			ALPH_item (common_labels, a++) = ALPH_item (labels, n);
	}

	if (a != len)  /* this should never happen */
	{
		fprintf (stderr, "ERROR in label count in 'make_common_label_alph()'\n");
		exit (1);
	}

	label_to_binary (common_labels);  /* repetitions will disappear now */

	return (common_labels);
}

PARSE_TBL make_parse_table_from_nv (NVptr nv, int side)
{
	/* Returns a parse table for all nets of nv if there are multi-char
	symbols on the given side (UPPER, LOWER or BOTH_SIDES) in the label
	alphabets.
	If there are only one-char ASCII symbols, NULL is returned. */

	PARSE_TBL parse_tbl;
	ALPHABETptr labels;
	int i, len, maxlen;

	/* calculate the maximal length of a label on 'side' */

	for (i=0, maxlen = 0; i< NV_len (nv); i++)
	{
		labels = NET_labels (NV_net (nv, i));
		len = max_label_length_in_alph (labels, side);
		maxlen = max_of (len, maxlen);
	}

	if (maxlen < 2)         /*no multi-char labels*/
		return (NULL);

	/* make a parse table */

	labels = make_common_label_alph (nv);

	parse_tbl = create_parse_table (NULL, labels, side);
	free_alph (labels);
	return (parse_tbl);
}

void make_id_vector_or_parse_table (int side)
{
	/* Makes either an ID vector or a parse table, according to
	LOOKUP_FLAGS.multi_char_symbols.
	A parse table can be made for multi-char on the UPPER, LOWER
	or BOTH_SIDES. */

	LOOKUP_PARSE_TABLE = NULL;

	if (LOOKUP_FLAGS.multi_char_symbols)
	{
		switch (LOOKUP_FLAGS.multi_char_side)
		{
		case FLAG_MCHAR_INPUT_SIDE:
			break;
		case FLAG_MCHAR_UPPER_SIDE:
			side = UPPER;
			break;
		case FLAG_MCHAR_LOWER_SIDE:
			side = LOWER;
			break;
		case FLAG_MCHAR_BOTH_SIDES:
			side = BOTH_SIDES;
			break;
		default:
			error_message ("in LOOKUP_FLAGS:\n\
						   wrong side '%c' for multi-character symbols specified",
						   LOOKUP_FLAGS.multi_char_side);
		}

	}

	LOOKUP_PARSE_TABLE = make_parse_table_from_nv (LEXNETS, side);

	/* if there were no multi-char symbols and no non-ASCII symbols
	in UTF-8 mode then no parse table was created */

	if (! LOOKUP_PARSE_TABLE)
	{
		LOOKUP_ID_TABLE = make_id_vect ();
		LOOKUP_FLAGS.multi_char_symbols = FALSE;
	}
	else /* In UTF-8 mode we need the parse table if there
		 are non-ASCII characters in the input. */
		 LOOKUP_FLAGS.multi_char_symbols = TRUE;
}

/*-----------------------------------------------------------*/
void select_match_tables (int input_side)
{
	/* Selects the match tables. */

	if (! UPPER_MATCH)
		update_match_tables ();

	if (input_side == UPPER)
	{
		LOOKUP_INPUT_MATCH = UPPER_MATCH;
		LOOKUP_OUTPUT_MATCH = LOWER_MATCH;
	}
	else
	{
		LOOKUP_INPUT_MATCH = LOWER_MATCH;
		LOOKUP_OUTPUT_MATCH = UPPER_MATCH;
	}

	LOOKUP_INPUT_MATCH [ALTCHAIN_SYMBOL] = EPSILON;
	LOOKUP_OUTPUT_MATCH [ALTCHAIN_SYMBOL] = EPSILON;
}

/*===========================================================
PARSE INPUT
===========================================================*/

int parse_string_with_parse_table (char *string, id_type *path)
{
	/* Uses a parse table to convert the string into a sequence of IDs. */

	id_type *seq = INPUT_SEQ;  
	int parse_error = 0;

	if ((parse_error = parse_string (string, LOOKUP_PARSE_TABLE,
		DONT_ZERO_TO_EPS, DONT_IGNORE_WHITES,
		DONT_ESCAPE, DONT_REPORT_ERROR)))

		/** error_message ("string '%s' could not be parsed", string); */
		return parse_error;

	while (*seq != ID_EOS)
		*path++ = *seq++;

	*path = ID_EOS;   /* end of sequence */
	return parse_error;
}

void parse_string_with_id_table (char *string, id_type *path)
{
	/* TEMPORARY FUNCTION.
	WILL BE REPLACED BY AN EQUIVALENT FUNCTION FROM THE FSM-PACKAGE.

	Uses an ID table to convert the string into a sequence of IDs. */

	unsigned char *pt = (unsigned char *) string;
	id_type *vec = ALPH_items (LOOKUP_ID_TABLE);

	for ( ; *pt;  pt++, path++)
		*path = vec [*pt];

	*path = ID_EOS;   /* end of sequence */
}

int convert_string_to_id_seq (char *string, id_type *path)
{
	int parse_error=0;

	if (LOOKUP_FLAGS.multi_char_symbols)
		parse_error=parse_string_with_parse_table (string, path);
	else
		parse_string_with_id_table (string, path);
	return parse_error;
}

/*===========================================================
ALLPY STRINGS TO NETWORKS (with checking on the next net)
===========================================================*/

/* When applying an ID sequence to a net of a vector, we can immediately
check the generated output sequence on the input_side of the next
network. For this purpose we create a check_net that is equivalent to
the (determinized) input_side of a network. */

static int count_arcs (STATEptr s) 
{
	int count;
	ARCptr a;

	for (count = 0, a = STATE_arc_set(s);
		a != NULL;
		count++, a = next_arc(a));

	return (count);
}

long CHK_vectorize_check_net (NETptr net)
{
	STATEptr state;
	long vector_size, num_arcs, num_vec_states = 0;

	if (LOOKUP_FLAGS.vectorize_min < 0)
		return (num_vec_states);        /* do not vectorize */

	/* initialize arc_vector_heap */

	NET_arc_vector_len (net) = vector_size = arc_vector_size (net);

	for (state = NET_states (net); state; state = next_state (state))
	{
		CHK_num_arcs_in_state (state) = num_arcs = count_arcs (state);

		if ((! STATE_vector_p (state)) &&
			(num_arcs >= LOOKUP_FLAGS.vectorize_min))
			num_vec_states++;
	}

	NET_arc_vector_heap(net) =
		init_heap (vector_size * sizeof (STATEptr),
		num_vec_states, "Net arc vectors");

	/* vectorize some states */

	for (state = NET_states (net); state; state = next_state (state))
	{
		if ((! STATE_vector_p (state)) &&
			(CHK_num_arcs_in_state (state) >= LOOKUP_FLAGS.vectorize_min))
			make_vector_state (state, net, vector_size);
	}

	return (num_vec_states);
}

NETptr CHK_make_check_net (NETptr fst, int fst_idx, int input_side)
{
	/* Returns the a net corresponding to one side of the transducer.
	The net is minimal and deterministic. Its labels are in a
	LABEL_VECTOR and its sigma in a BINARY_VECTOR. */

	NETptr net;

	if (! fst)
		return (NULL);

	if (input_side == LOWER)
		net = lower_fsm (fst, DO_COPY, DO_MINIMIZE);
	else
		net = upper_fsm (fst, DO_COPY, DO_MINIMIZE);

	if (LOOKUP_FLAGS.show_comments)
		fprintf (stderr, "transducer %d: check_net made\n", fst_idx);

	return (net);
}

void CHK_make_all_check_nets (NVptr nv)
{
	NETptr net;  int i;

	if ((! nv) || (! NV_len (nv)))
		return;

	for (i=0; i< NV_len (nv); i++)
	{
		net = NV_net (nv, i);

		if (NET_num_states (net) >= LOOKUP_FLAGS.check_net_min)
			CHK_chknet (net) = CHK_make_check_net (net, i, LOWER);
	}
}

void CHK_free_all_check_nets (NVptr nv)
{
	NETptr net;  int i;

	if ((! nv) || (! NV_len (nv)))
		return;

	for (i=0; i< NV_len (nv); i++)
		if ((net = NV_net (nv, i)))
			if (CHK_chknet (net))
			{
				free_network (CHK_chknet (net));
				CHK_chknet (net) = NULL;
			}
}

/*===========================================================*/

STATEptr CHK_map_id_on_check_net (id_type id, STATEptr state, NETptr net)
{
	/* Checks whether can ID can be mapped in the current state. If yes,
	the destination state will be returned.
	The network is a deterministic (epsilon-free) simple fsm. */

	ARCptr arc;

	if (! state)
		return (NULL);

	if (! binary_sigma_member (NET_sigma (net), id))
		id = OTHER;          /* ID can only be mapped by OTHER */

	if (STATE_vector_p (state))   /* state has an arc vector */
	{
		if (id < NET_arc_vector_len (net))
			return (AV_dest (STATE_arc_vector (state), id));
		else
			return (NULL);
	}

	/* state has an arc list */
	for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
		if (ARC_label (arc) == id)
			return (ARC_destination (arc));

	return (NULL);   /* ID could not be mapped */
}

/*===========================================================*/

void apply_id_seq_to_nv_checked(id_type *inseq, id_type *outseq, NVptr nv, int net_idx,STATEptr state, STATEptr chk_state,void* parseur) {
	if ((NET_u_flag_diacr (NV_net (nv, net_idx))) ||
		(NET_l_flag_diacr (NV_net (nv, net_idx)))  )
		error_message ("Sorry, but in the current implementation,\n\
					   flag diacritics and check nets cannot be used together.");

	if (STATE_vector_p (state))    /* state has an arc vector */
		apply_id_seq_to_nv_vstate_checked(inseq, outseq, nv, net_idx, state, chk_state,parseur);
	else                  /* state has an arc list */
		apply_id_seq_to_nv_nstate_checked(inseq, outseq, nv, net_idx, state, chk_state,parseur);

	if (EditDistanceAction(parseur,action_switch)) {
		//we switch the current character with the next and we relaunch all
		id_type idnext=*(inseq+1);
		if (idnext!=ID_EOS) {
			//then we can switch the two characters
			id_type idcurrent=*inseq;
			*inseq=idnext;
			*(inseq+1)=idcurrent;
			//we then relaunch our function with the two characters switched
			if (STATE_vector_p (state))    /* state has an arc vector */
				apply_id_seq_to_nv_vstate_checked(inseq, outseq, nv, net_idx, state, chk_state,parseur);
			else                  /* state has an arc list */
				apply_id_seq_to_nv_nstate_checked(inseq, outseq, nv, net_idx, state, chk_state,parseur);
			//we set the inseq structure back to normal...
			*inseq=idcurrent;
			*(inseq+1)=idnext;
		}
		EditDistancePop(parseur);
	}
}

void apply_a_loop_nb_state_checked(id_type *inseq, id_type *outseq, NVptr nv, int net_idx,STATEptr state, STATEptr chk_state,char action,void* parseur) {
	STATEptr next_chk_state;  
	ARCptr arc;
	id_type inlab;  
	int xin, xout;
	char ret=0;
	id_type addc=0;

	for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
	{
		/* check whether the input ID is accepted by the arc */
		ret=0;
		addc=0;
		inlab = LOOKUP_INPUT_MATCH [ARC_label (arc)];
		if (!inlab)
			continue;

		if (inlab == OTHER)
		{
			if ((! binary_sigma_member (NET_sigma (NV_net (nv,net_idx)),*inseq))
				|| (*inseq == OTHER)) {
				xin = 1;   /* input accepted and one symbol consumed */
				addc=*inseq;
			}
			else
				continue;  /* input not accepted by OTHER */
		}
		else if (inlab == EPSILON)
		{
			if (ARC_destination (arc) != state)  /* avoid EPSILON cycles */
				xin = 0;   /* input accepted, but no symbol consumed */
			else
				continue;  /* input not accepted on EPSILON cycle */
		}
		else  /* inlab == normal symbol */
		{
			if (action==action_change) {
				if ((inlab == *inseq) || (*inseq == OTHER))
					continue;
				xin=1;
			}
			else
				if (action==action_insert)
					xin=0;
			addc=inlab;			
		}

		/* handle output ID */

		if ((*outseq = LOOKUP_OUTPUT_MATCH [ARC_label (arc)]) == OTHER)
		{
			xout = 1;       /* one output symbol will be generated */
			if (inlab == EPSILON)
				*outseq = LOOKUP_UNKNOWN_SYMBOL;  /* a default symbol */
			else
				*outseq = *inseq;
		}
		else if (*outseq == EPSILON)
			xout = 0;       /* no output will be generated */
		else  /* outlab == normal symbol */
			xout = 1;       /* one output symbol will be generated */

		/* check if output symbol is accepted by the next net */

		if (chk_state && xout)  /* checking required */
		{
			if (! (next_chk_state =
				CHK_map_id_on_check_net
				(*outseq, chk_state, CHK_chknet (NV_net (nv, net_idx+1)))))
				continue;   /* checking failed */
		}
		else      /* checking not required */
			next_chk_state = chk_state;

		/* and continue mapping recursively in the current net */
		if (addc && addc != ID_EOS)
			ret=AddId(parseur,addc);

		apply_id_seq_to_nv_checked
			(inseq + xin, outseq + xout, nv, net_idx,
			ARC_destination (arc), next_chk_state,parseur);

		if (ret)
			RemoveId(parseur);

		/*IF SEQUENTIALIZED: Major change:  finish for loop on arcs if match found */
		if (NET_lower_seq(NV_net (nv, net_idx)) && xin)
			break;
	}
}


void apply_id_seq_to_nv_nstate_checked
	(id_type *inseq, id_type *outseq, NVptr nv, int net_idx,
	STATEptr state, STATEptr chk_state,void* parseur)
{
	/* Applies the input sequence recursively to all arcs of one network,
	and to all networks of the net vector.
	It is an enhanced version of the function where every generated
	output symbol is immediately checked on the following network.
	The sigmas of all networks are supposed to be of type
	BINARY_VECTOR !!!*/

	STATEptr next_chk_state;  
	ARCptr arc;
	id_type inlab;  
	int xin, xout;
	char compared=0;
	char ret=0;
	short* previous;

	//AfficheContenu(parseur,inseq,monlocalbuf);

	if (*inseq == ID_EOS) /* case: sequence end */
	{

		if (CheckEditDistanceAction(parseur,action_prefix)) {
			AddEditDistanceAction(parseur,action_prefix_detected);
			LOOKUP_print_result_fn (inseq+1,parseur);
			RemoveEditDistanceAction(parseur,action_prefix_detected);			
			return;
		}

		if (STATE_final (state))   /* input sequence is accepted */
		{
			*outseq = ID_EOS;   /* finish the output sequence */

			/* If the current net is the last one in the nv, we generate
			an ouput, i.e. we add a new path to the LOOKUP_RESULT net.
			The output sequence begins immediately after the input sequence,
			i.e. at inseq+1. */


			if (net_idx == NV_len (nv) - 1)
				LOOKUP_print_result_fn (inseq+1,parseur);

			/* If the current net is not the last one in the nv, we have to
			re-enter recursion with:
			- outseq (i.e. inseq+1) as new inseq,
			- the space after outseq (i.e. outseq+1) as new outseq,
			- the next network and its start state */

			else
			{
				compared=0;
				previous=CleanEditDistance(parseur);

				if (net_idx < NV_len (nv) - 2)
					next_chk_state = CHK_start_state (NV_net (nv, net_idx+2));
				else {
					next_chk_state = NULL;
					compared=RemoveLastEditDistanceAction(parseur);
				}
				
				apply_id_seq_to_nv_checked
					(inseq+1, outseq+1, nv, net_idx+1,
					NET_start_state (NV_net (nv, net_idx+1)), next_chk_state,parseur);
					
				AddLastEditDistanceAction(parseur,compared,previous);
			}
		}

		/* continue matching with EPSILON only */
		for (arc = STATE_arc_set (state); arc; arc = next_arc (arc)) {
			if (LOOKUP_INPUT_MATCH [ARC_label (arc)] == EPSILON)
			{
				if ((*outseq = LOOKUP_OUTPUT_MATCH [ARC_label (arc)]) == OTHER)
					xout = 1;       /* one output symbol will be generated */
				else if (*outseq == EPSILON)
					xout = 0;       /* no output will be generated */
				else  /* outlab == normal symbol */
					xout = 1;       /* one output symbol will be generated */

				/* check if output symbol is accepted by the next net */

				if (chk_state && xout)  /* checking required */
				{
					if (! (next_chk_state =
						CHK_map_id_on_check_net
						(*outseq, chk_state,
						CHK_chknet (NV_net (nv, net_idx+1)))))
						continue;   /* checking failed */
				}
				else      /* checking not required */
					next_chk_state = chk_state;

				/* and continue mapping recursively in the current net */

				apply_id_seq_to_nv_checked
					(inseq, outseq + xout, nv, net_idx,
					ARC_destination (arc), next_chk_state,parseur);
			}
		}

		if (EditDistanceAction(parseur,action_insert)) {
			//We accept that the current character might be changed into another...
			apply_a_loop_nb_state_checked(inseq, outseq, nv,net_idx,state,chk_state,action_insert,parseur);
			EditDistancePop(parseur);
		}

		return;
	}

	/* case: not sequence end; try to map the current input symbol */

	for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
	{
		ret=0;
		/* check whether the input ID is accepted by the arc */

		if ((inlab = LOOKUP_INPUT_MATCH [ARC_label (arc)]) == OTHER)
		{
			if ((! binary_sigma_member (NET_sigma (NV_net (nv,net_idx)),*inseq))
				|| (*inseq == OTHER))
				xin = 1;   /* input accepted and one symbol consumed */
			else
				continue;  /* input not accepted by OTHER */
		}
		else if (inlab == EPSILON)
		{
			if (ARC_destination (arc) != state)  /* avoid EPSILON cycles */
				xin = 0;   /* input accepted, but no symbol consumed */
			else
				continue;  /* input not accepted on EPSILON cycle */
		}
		else  /* inlab == normal symbol */
		{
			compared=1;
			if ((inlab == *inseq) || (*inseq == OTHER))
				xin = 1;   /* input accepted and one symbol consumed */				
			else
				continue;  /* input not accepted */
		}

		/* handle output ID */

		if ((*outseq = LOOKUP_OUTPUT_MATCH [ARC_label (arc)]) == OTHER)
		{
			xout = 1;       /* one output symbol will be generated */
			if (inlab == EPSILON)
				*outseq = LOOKUP_UNKNOWN_SYMBOL;  /* a default symbol */
			else
				*outseq = *inseq;
		}
		else if (*outseq == EPSILON)
			xout = 0;       /* no output will be generated */
		else  /* outlab == normal symbol */
			xout = 1;       /* one output symbol will be generated */

		/* check if output symbol is accepted by the next net */

		if (chk_state && xout)  /* checking required */
		{
			if (! (next_chk_state =
				CHK_map_id_on_check_net
				(*outseq, chk_state, CHK_chknet (NV_net (nv, net_idx+1))))) {
					if (ret)
						RemoveId(parseur);
					continue;   /* checking failed */
			}
		}
		else      /* checking not required */
			next_chk_state = chk_state;

		/* and continue mapping recursively in the current net */
		ret=0;
		if (xin==1) {
			if (inlab!=OTHER)
				ret=AddId(parseur,inlab);
			else
				ret=AddId(parseur,*inseq);
		}

		apply_id_seq_to_nv_checked
			(inseq + xin, outseq + xout, nv, net_idx,
			ARC_destination (arc), next_chk_state,parseur);

		if (ret)
			RemoveId(parseur);

		/*IF SEQUENTIALIZED: Major change:  finish for loop on arcs if match found */
		if (NET_lower_seq(NV_net (nv, net_idx)) && xin)
			break;
	}

	if (compared) {
		if (EditDistanceAction(parseur, action_delete)) {
			//we jump over the current character
			apply_id_seq_to_nv_checked(inseq + 1, outseq, nv, net_idx, state, chk_state, parseur);
			EditDistancePop(parseur);
		}

		if (EditDistanceAction(parseur, action_change)) {
			//We accept that the current character might be changed into another...
			apply_a_loop_nb_state_checked(inseq, outseq, nv, net_idx, state, chk_state, action_change, parseur);
			EditDistancePop(parseur);
		}

		if (EditDistanceAction(parseur, action_insert)) {
			//We accept that the current character might be changed into another...
			apply_a_loop_nb_state_checked(inseq, outseq, nv, net_idx, state, chk_state, action_insert, parseur);
			EditDistancePop(parseur);
		}
	}
}

void apply_id_seq_to_nv_vstate_checked
	(id_type *inseq, id_type *outseq, NVptr nv, int net_idx,
	STATEptr state, STATEptr chk_state,void* parseur)
{
	/* Applies the input sequence recursively to all arcs of one network,
	and to all networks of the net vector.
	It is an enhanced version of the function where every generated
	output symbol is immediately checked on the following network.
	The sigmas of all networks are supposed to be of type
	BINARY_VECTOR !!!*/

	STATEptr *arc_vector, dest, next_chk_state;
	id_type inlab, *range_items [2], *items;
	int n, range_len [2], xin, xout;
	int32 len;
	char compared=0;
	char ret=0;
	short* previous;

	/* init */

	arc_vector = AV_dest_array (STATE_arc_vector (state));

	range_len [0] =
		RANGE_len (NET_uprange_map (NV_net (nv, net_idx)) + EPSILON);
	range_items [0] =
		RANGE_items (NET_uprange_map (NV_net (nv, net_idx)) + EPSILON);

	if ((inlab = *inseq) != ID_EOS)   /* input label exists */
	{
		if (binary_sigma_member (NET_sigma (NV_net (nv, net_idx)), inlab))
		{
			range_len [1] =
				RANGE_len (NET_uprange_map (NV_net (nv, net_idx)) + inlab);
			range_items [1] =
				RANGE_items (NET_uprange_map (NV_net (nv, net_idx)) + inlab);
		}
		else  /* input label can only be accepted by OTHER */
		{
			range_len [1] =
				RANGE_len (NET_uprange_map (NV_net (nv, net_idx)) + OTHER);
			range_items [1] =
				RANGE_items (NET_uprange_map (NV_net (nv, net_idx)) + OTHER);
		}
	}

	/* case: sequence end */

	if (inlab == ID_EOS)
	{
		if (CheckEditDistanceAction(parseur,action_prefix)) {
			AddEditDistanceAction(parseur,action_prefix_detected);
			LOOKUP_print_result_fn (inseq+1,parseur);
			RemoveEditDistanceAction(parseur,action_prefix_detected);			
			return;
		}

		if (STATE_final (state))   /* input sequence is accepted */
		{
			*outseq = ID_EOS;   /* finish the output sequence */

			/* If the current net is the last one in the nv, we generate
			an ouput, i.e. we add a new path to the LOOKUP_RESULT net.
			The output sequence begins immediately after the input sequence,
			i.e. at inseq+1. */

			if (net_idx == NV_len (nv) - 1)
				LOOKUP_print_result_fn (inseq+1,parseur);

			/* If the current net is not the last one in the nv, we have to
			re-enter recursion with:
			- outseq (i.e. inseq+1) as new inseq,
			- the space after outseq (i.e. outseq+1) as new outseq,
			- the next network and its start state */

			else
			{
				compared=0;
				previous=CleanEditDistance(parseur);

				if (net_idx < NV_len (nv) - 2)
					next_chk_state = CHK_start_state (NV_net (nv, net_idx+2));
				else {
					next_chk_state = NULL;
					compared=RemoveLastEditDistanceAction(parseur);
				}

				apply_id_seq_to_nv_checked
					(inseq+1, outseq+1, nv, net_idx+1,
					NET_start_state (NV_net (nv, net_idx+1)), next_chk_state,parseur);

				AddLastEditDistanceAction(parseur,compared,previous);
			}
		}

		/* continue matching with EPSILON only */

		for (len = range_len [0], items = range_items [0];
			len > 0;
			len--, items++)
			if ((dest = arc_vector [*items]))
			{
				if ((*outseq = LOOKUP_OUTPUT_MATCH [*items]) == OTHER)
					xout = 1;       /* one output symbol will be generated */
				else if (*outseq == EPSILON)
					xout = 0;       /* no output will be generated */
				else  /* outlab == normal symbol */
					xout = 1;       /* one output symbol will be generated */

				/* check if output symbol is accepted by the next net */

				if (chk_state && xout)  /* checking required */
				{
					if (! (next_chk_state =
						CHK_map_id_on_check_net
						(*outseq, chk_state,
						CHK_chknet (NV_net (nv, net_idx+1)))))
						continue;   /* checking failed */
				}
				else      /* checking not required */
					next_chk_state = chk_state;

				/* and continue mapping recursively in the current net */

				apply_id_seq_to_nv_checked
					(inseq, outseq + xout, nv, net_idx, dest, next_chk_state,parseur);
			}

			return;
	}

	/* case: not sequence end; try to map the current input symbol */

	for (n = 0; n < 2; n++)
		for (len = range_len [n], items = range_items [n];
			len > 0;
			len--, items++)
			if ((dest = arc_vector [*items]))
			{
				/* handle input label */

				if ((inlab = LOOKUP_INPUT_MATCH [*items]) == EPSILON)
				{
					if (dest != state)  /* avoid EPSILON cycles */
						xin = 0;   /* input accepted, but no symbol consumed */
					else
						continue;  /* input not accepted on EPSILON cycle */
				}
				else   /* OTHER or real symbol */
					xin = 1;   /* input accepted and one symbol consumed */

				/* handle output ID */

				if ((*outseq = LOOKUP_OUTPUT_MATCH [*items]) == OTHER)
				{
					xout = 1;       /* one output symbol will be generated */
					if (inlab == EPSILON)
						*outseq = LOOKUP_UNKNOWN_SYMBOL;  /* a default symbol */
					else
						*outseq = *inseq;
				}
				else if (*outseq == EPSILON)
					xout = 0;       /* no output will be generated */
				else  /* outlab == normal symbol */
					xout = 1;       /* one output symbol will be generated */

				/* check if output symbol is accepted by the next net */

				if (chk_state && xout)  /* checking required */
				{
					if (! (next_chk_state = CHK_map_id_on_check_net
						(*outseq, chk_state,
						CHK_chknet (NV_net (nv, net_idx+1)))))
						continue;   /* checking failed */
				}
				else      /* checking not required */
					next_chk_state = chk_state;

				/* continue mapping recursively in the current net */
				ret=0;
				if (xin==1) {
					if (inlab!=OTHER)
						ret=AddId(parseur,inlab);
					else
						ret=AddId(parseur,*inseq);
				}

				apply_id_seq_to_nv_checked
					(inseq + xin, outseq + xout, nv, net_idx, dest, next_chk_state,parseur);

				if (ret)
					RemoveId(parseur);
			}
}

/*===========================================================
APPLY STRINGS TO NETWORKS (without checking on the next net)
===========================================================*/

int length_of_sequence (id_type *seq)
{
	/* Returns the forward length of an ID sequence. */

	id_type *end;

	for (end = seq; *end != ID_EOS; end++) ;
	return (end - seq);
}

void add_id_seq_to_net (id_type *seq, NETptr net)
{
	/* Adds a new path corresponding to the ID sequence to the net.
	Does no miminization or determinization ! */

	STATEptr prev_state, state = NET_start_state (net);
	ARCptr arc;

	for ( ; *seq != ID_EOS; seq++)
	{
		prev_state = state;
		state = make_state (NULL, net);

		arc = make_arc (*seq, state, net);

		if (prev_state == NET_start_state (net))
		{
			push_arc (arc, prev_state);
			STATE_deterministic (prev_state) = FALSE;
		}
		else
			STATE_arc_set (prev_state) = arc;
	}

	STATE_final (state) = TRUE;
}

void add_id_seq_to_result (id_type *seq,void* parseur)
{
	add_id_seq_to_net (seq, LOOKUP_RESULT);
}

void OLD_apply_id_seq_to_nv
	(id_type *inseq, id_type *outseq, NVptr nv, int net_idx, STATEptr state,void* parseur)
{
	/* Applies the input sequence recursively to all arcs of one network,
	and to all networks of the net vector.
	The sigmas of all networks are supposed to be of type BINARY_VECTOR !!!*/

	ARCptr arc;  
	id_type inlab;  
	int xin, xout;

	/* case: sequence end */

	if (*inseq == ID_EOS )
	{
		if (STATE_final (state))   /* input sequence is accepted */
		{
			*outseq = ID_EOS;   /* finish the output sequence */

			/* If the current net is the last one in the nv, we generate
			an ouput, i.e. we add a new path to the LOOKUP_RESULT net.
			The output sequence begins immediately after the input sequence,
			i.e. at inseq+1. */

			if (net_idx == NV_len (nv) - 1)
				LOOKUP_print_result_fn (inseq+1,parseur);

			/* If the current net is not the last one in the nv, we have to
			re-enter recursion with:
			- outseq (i.e. inseq+1) as new inseq,
			- the space after outseq (i.e. outseq+1) as new outseq,
			- the next network and its start state */

			else
				OLD_apply_id_seq_to_nv
				(inseq+1, outseq+1, nv, net_idx+1,
				NET_start_state (NV_net (nv, net_idx+1)),parseur);
		}
		else   /* state is not final; continue with EPSILON only */
		{
			for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
				if (LOOKUP_INPUT_MATCH [ARC_label (arc)] == EPSILON)
				{
					if ((*outseq = LOOKUP_OUTPUT_MATCH [ARC_label (arc)]) == OTHER)
					{
						xout = 1;       /* one output symbol will be generated */
						*outseq = LOOKUP_UNKNOWN_SYMBOL;  /* a default symbol */
					}
					else if (*outseq == EPSILON)
						xout = 0;       /* no output will be generated */
					else  /* outlab == normal symbol */
						xout = 1;       /* one output symbol will be generated */

					/* continue mapping recursively */

					OLD_apply_id_seq_to_nv
						(inseq, outseq + xout, nv, net_idx, ARC_destination (arc),parseur);
				}
		}

		return;
	}

	/* case: not sequence end; try to map the current input symbol */

	for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
	{
		/* check whether the input ID is accepted by the arc */

		if ((inlab = LOOKUP_INPUT_MATCH [ARC_label (arc)]) == OTHER)
		{
			if (! binary_sigma_member (NET_sigma (NV_net (nv,net_idx)), *inseq))
				xin = 1;   /* input accepted and one symbol consumed */
			else
				continue;  /* input not accepted by OTHER */
		}
		else if (inlab == EPSILON)
		{
			if (ARC_destination (arc) != state)  /* avoid EPSILON cycles */
				xin = 0;   /* input accepted, but no symbol consumed */
			else
				continue;  /* input not accepted on EPSILON cycle */
		}
		else  /* inlab == normal symbol */
		{
			if (inlab == *inseq)
				xin = 1;   /* input accepted and one symbol consumed */
			else
				continue;  /* input not accepted */
		}

		/* handle output ID */

		if ((*outseq = LOOKUP_OUTPUT_MATCH [ARC_label (arc)]) == OTHER)
		{
			xout = 1;       /* one output symbol will be generated */
			if (inlab == EPSILON)
				*outseq = LOOKUP_UNKNOWN_SYMBOL;  /* a default symbol */
			else
				*outseq = *inseq;
		}
		else if (*outseq == EPSILON)
			xout = 0;       /* no output will be generated */
		else  /* outlab == normal symbol */
			xout = 1;       /* one output symbol will be generated */

		/* continue mapping recursively */

		OLD_apply_id_seq_to_nv
			(inseq + xin, outseq + xout, nv, net_idx, ARC_destination (arc),parseur);
	}
}

/*===========================================================*/
void apply_id_seq_to_nv_simple(id_type *inseq, id_type *outseq, NVptr nv, int net_idx, STATEptr state,void* parseur)
{
	if (STATE_vector_p (state))    /* state has an arc vector */
		apply_id_seq_to_nv_vstate_simple (inseq, outseq, nv, net_idx, state,parseur);
	else                  /* state has an arc list */
		apply_id_seq_to_nv_nstate_simple (inseq, outseq, nv, net_idx, state,parseur);

	if (EditDistanceAction(parseur,action_switch)) {
		//we switch the current character with the next and we relaunch all
		id_type idnext=*(inseq+1);
		if (idnext!=ID_EOS) {
			//then we can switch the two characters
			id_type idcurrent=*inseq;
			*inseq=idnext;
			*(inseq+1)=idcurrent;
			//we then relaunch our function with the two characters switched
			if (STATE_vector_p (state))    /* state has an arc vector */
				apply_id_seq_to_nv_vstate_simple (inseq, outseq, nv, net_idx, state,parseur);
			else                  /* state has an arc list */
				apply_id_seq_to_nv_nstate_simple (inseq, outseq, nv, net_idx, state,parseur);
			//we set the inseq structure back to normal...
			*inseq=idcurrent;
			*(inseq+1)=idnext;
		}
		EditDistancePop(parseur);
	}
}

void /*  NEW_TG_99_5_5 */ EXPERIMENTAL_1999_5_5_apply_id_seq_to_nv_nstate_simple
	(id_type *inseq, id_type *outseq, NVptr nv, int net_idx, STATEptr state,void* parseur)
	/*** [-a 1] I have tried to hack into Andre's code (handling the
	[-a 1], depth-first algorithm)  to implement "reduced
	labelset" recognition. It works on individual fst-s but not
	on scripts!

	Still, I install it as is for individual testing purposes.

	In normal cases it works as before. In reduced-labelset cases
	it may not recognize otherwise valid matches! T Gaal
	*/
{
	/* For normal (non-vectorized) states !!!
	Applies the input sequence recursively to all arcs of one network,
	and to all networks of the net vector.
	It is the simple version of this function.
	The sigmas of all networks are supposed to be of type
	BINARY_VECTOR !!!*/

	ARCptr arc; 
	id_type inlab;  
	int xin, xout;
	FLAG_DIACRptr flag;  
	id_type flag_attr, reg_value_old, arc_id, *fst_ids;
	uint16 i, fst_id_lim;
	ALPHABETptr flag_reg = NET_flag_register (NV_net (nv, net_idx));
	RANGEptr range, eps_range;

	fst_id_lim = ALPH_len(LOWER_SIGMAS_LEXNETS[net_idx].low_sig);
	fst_ids = ALPH_items(LOWER_SIGMAS_LEXNETS[net_idx].low_sig);

	/* case: sequence end */

	if (*inseq == ID_EOS)
	{
		
		if (CheckEditDistanceAction(parseur,action_prefix)) {
			AddEditDistanceAction(parseur,action_prefix_detected);
			LOOKUP_print_result_fn (inseq+1,parseur);
			RemoveEditDistanceAction(parseur,action_prefix_detected);
			return;
		}		
		
		if (STATE_final (state))   /* input sequence is accepted */
		{
			*outseq = ID_EOS;   /* finish the output sequence */

			/* If the current net is the last one in the nv, we generate
			an ouput, i.e. we add a new path to the LOOKUP_RESULT net.
			The output sequence begins immediately after the input sequence,
			i.e. at inseq+1. */

			if (net_idx == NV_len (nv) - 1)
				LOOKUP_print_result_fn (inseq+1,parseur);

			/* If the current net is not the last one in the nv, we have to
			re-enter recursion with:
			- outseq (i.e. inseq+1) as new inseq,
			- the space after outseq (i.e. outseq+1) as new outseq,
			- the next network and its start state */

			else
				apply_id_seq_to_nv_simple
				(inseq+1, outseq+1, nv, net_idx+1,
				NET_start_state (NV_net (nv, net_idx+1)),parseur);
		}

		/* continue matching with EPSILON only */

		for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
		{
			if (NET_reduced_labelset(NV_net (nv, net_idx)))
			{
				eps_range = RANGE_VECTOR_vector(MATCH_TABLE_table(NET_upmatch_table(NV_net(nv, net_idx)))[ARC_label(arc)]);
				i = 0;

				if (eps_range && eps_range->items)
					arc_id = RANGE_item(eps_range, i);
				else
					arc_id = EPSILON;
			}
			else /* labelset not reduced  */
			{
				eps_range = 0;
				arc_id = ARC_label(arc);
			}

			do /* slow inner loop 99-5-3 */
			{      
				if (arc_id)
				{

					inlab = LOOKUP_INPUT_MATCH[arc_id];
					flag = NULL;
					if ((inlab == EPSILON) || (inlab == ALTCHAIN_SYMBOL) ||
						(flag = flag_diacritic (inlab)) )
					{
						*outseq = LOOKUP_OUTPUT_MATCH [arc_id];
						if (*outseq == OTHER)
							xout = 1;       /* one output symbol will be generated */
						else if ((*outseq == EPSILON) || (*outseq == ALTCHAIN_SYMBOL))
							xout = 0;       /* no output will be generated */
						else if (flag_diacritic_p (*outseq))
						{
							xout = 0;           /* no output will be generated */
							*outseq = EPSILON;   /* replace flag by EPSILON */
						}
						else  /* outlab == normal symbol */
							xout = 1;       /* one output symbol will be generated */

						/* continue mapping recursively */

						if (flag)
						{
							flag_attr = FLAG_DIACR_attribute (flag);
							reg_value_old = flag_value (flag_reg, flag_attr);

							if (flag_action (flag_reg, flag))     /* flag action ok */
								apply_id_seq_to_nv_simple
								(inseq, outseq+xout, nv, net_idx, ARC_destination (arc),parseur);

							flag_value (flag_reg, flag_attr) = reg_value_old;
						}
						else  /* not a flag */
							apply_id_seq_to_nv_simple
							(inseq, outseq + xout, nv, net_idx, ARC_destination (arc),parseur);
					}
				}
				/* if reduced labelset: further loop on hidden (virtual) arcs*/ 
				if (NET_reduced_labelset(NV_net(nv, net_idx)))
				{
					i++;

					if (eps_range && i < RANGE_len(eps_range) && RANGE_items(eps_range))
						arc_id = RANGE_item(eps_range, i);
					else
						arc_id = EPSILON;
				}
			} while (eps_range && i < RANGE_len(eps_range));
		} /*  slow part end */	

		return;
	}

	/* case: not sequence end; try to map the current input symbol */

	for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
	{
		/* check whether the input ID is accepted by the arc */
		if (NET_reduced_labelset(NV_net(nv, net_idx))) /* reduced labelset */
		{
			eps_range = RANGE_VECTOR_vector(MATCH_TABLE_table(NET_upmatch_table(NV_net(nv, net_idx)))[ARC_label(arc)]);

			if ( (*inseq < fst_id_lim  && fst_ids[*inseq]) &&
				*inseq <  RANGE_VECTOR_len(MATCH_TABLE_table(NET_upmatch_table(NV_net(nv, net_idx)))[ARC_label(arc)]))
				range = eps_range + *inseq;
			else
				range = eps_range + OTHER;

			if (!(range && range->items))
				range = eps_range;

			i = 0;

			if (range && range->items)
				arc_id = RANGE_item(range, i);
			else
				arc_id = EPSILON;
		}
		else /* labelset not reduced  */
		{
			eps_range = range = 0;
			arc_id = ARC_label(arc);
		}

		do /** slow inner loop 99-4-30 */
		{      
			if (arc_id)
			{

				inlab = LOOKUP_INPUT_MATCH [arc_id];
				flag = NULL;
				if (inlab == OTHER)
				{
					if ((! binary_sigma_member (NET_sigma (NV_net (nv,net_idx)),*inseq))
						|| (*inseq == OTHER))
						xin = 1;   /* input accepted and one symbol consumed */
					else
						continue;  /* input not accepted by OTHER */
				}
				else if ((inlab == EPSILON) || (inlab == ALTCHAIN_SYMBOL) ||
					(flag = flag_diacritic (inlab))  )
				{
					if (ARC_destination (arc) != state)  /* avoid EPSILON cycles */
						xin = 0;   /* input accepted, but no symbol consumed */
					else
						continue;  /* input not accepted on EPSILON cycle */
				}
				else  /* inlab == normal symbol */
				{
					if ((inlab == *inseq) || (*inseq == OTHER))
						xin = 1;   /* input accepted and one symbol consumed */
					else
						continue;  /* input not accepted */
				}

				/* handle output ID */

				*outseq = LOOKUP_OUTPUT_MATCH [arc_id];
				if (*outseq == OTHER)
				{
					xout = 1;       /* one output symbol will be generated */
					if ((inlab == EPSILON) || (inlab == ALTCHAIN_SYMBOL) || (flag))
						*outseq = LOOKUP_UNKNOWN_SYMBOL;  /* a default symbol */
					else
						*outseq = *inseq;
				}
				else if ((*outseq == EPSILON) || (*outseq == ALTCHAIN_SYMBOL) ||
					(flag_diacritic_p (*outseq)) )
					xout = 0;       /* no output will be generated */
				else  /* outlab == normal symbol */
					xout = 1;       /* one output symbol will be generated */

				/* continue mapping recursively */

				if (flag)
				{
					flag_attr = FLAG_DIACR_attribute (flag);
					reg_value_old = flag_value (flag_reg, flag_attr);

					if (flag_action (flag_reg, flag))       /* flag action ok */
						apply_id_seq_to_nv_simple
						(inseq, outseq + xout, nv, net_idx, ARC_destination (arc),parseur);

					flag_value (flag_reg, flag_attr) = reg_value_old;
				}
				else
					apply_id_seq_to_nv_simple
					(inseq + xin, outseq + xout, nv, net_idx, ARC_destination (arc),parseur);

				/*IF SEQUENTIALIZED: Major change:  finish for loop on arcs if match found */
				if (NET_lower_seq(NV_net (nv, net_idx)) && xin)
					break;

			}	   
			/* if reduced labelset: further loop on hidden (virtual) arcs*/ 
			if (NET_reduced_labelset(NV_net(nv, net_idx)))
			{
				i++;
				if (! (i < RANGE_len(range)) && 
					(range != eps_range))
				{
					range = eps_range;
					i = 0;
				}

				if (range && i < RANGE_len(range) && RANGE_items(range))
					arc_id = RANGE_item(range, i);
				else
					arc_id = EPSILON;
			}
		} while (range && i < RANGE_len(range)); 
	}
}

void apply_a_loop_nb_state_simple(id_type *inseq, id_type *outseq, NVptr nv, int net_idx, STATEptr state,char action,void* parseur) {
	ARCptr arc;  
	id_type inlab,  flag_attr, reg_value_old;
	int xin, xout;
	FLAG_DIACRptr flag;  
	ALPHABETptr flag_reg = NET_flag_register (NV_net (nv, net_idx));

	char ret=0;
	id_type addc;
	for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
	{
		/* check whether the input ID is accepted by the arc */

		ret=0;
		addc=0;

		inlab = LOOKUP_INPUT_MATCH [ARC_label (arc)];
		if (!inlab)
			continue;

		flag = NULL;
		if (inlab == OTHER)
		{
			if ((! binary_sigma_member (NET_sigma (NV_net (nv,net_idx)),*inseq))
				|| (*inseq == OTHER)) {
				xin = 1;   /* input accepted and one symbol consumed */
				addc=*inseq;
			}
			else
				continue;  /* input not accepted by OTHER */
		}
		else if ((inlab == EPSILON) || (inlab == ALTCHAIN_SYMBOL) ||
			(flag = flag_diacritic (inlab))  )
		{
			if (ARC_destination (arc) != state)  /* avoid EPSILON cycles */
				xin = 0;   /* input accepted, but no symbol consumed */
			else
				continue;  /* input not accepted on EPSILON cycle */
		}
		else  /* inlab == normal symbol */
		{
			/*We keep only a character which is unknown*/
			if (action==action_change) {
				if ((inlab == *inseq) || (*inseq == OTHER))
					continue;
				xin=1;
			}
			else
				if (action==action_insert)
					xin=0;
			addc=inlab;
		}

		/* handle output ID */

		*outseq = LOOKUP_OUTPUT_MATCH [ARC_label (arc)];
		if (*outseq == OTHER)
		{
			xout = 1;       /* one output symbol will be generated */
			if ((inlab == EPSILON) || (inlab == ALTCHAIN_SYMBOL) || (flag))
				*outseq = LOOKUP_UNKNOWN_SYMBOL;  /* a default symbol */
			else
				*outseq = *inseq;
		}
		else if ((*outseq == EPSILON) || (*outseq == ALTCHAIN_SYMBOL) ||
			(flag_diacritic_p (*outseq)) )
			xout = 0;       /* no output will be generated */
		else  /* outlab == normal symbol */
			xout = 1;       /* one output symbol will be generated */

		/* continue mapping recursively */

		if (flag)
		{
			flag_attr = FLAG_DIACR_attribute (flag);
			reg_value_old = flag_value (flag_reg, flag_attr);

			if (flag_action (flag_reg, flag))       /* flag action ok */
				apply_id_seq_to_nv_simple
				(inseq, outseq + xout, nv, net_idx, ARC_destination (arc),parseur);

			flag_value (flag_reg, flag_attr) = reg_value_old;
		}
		else {
			if (addc && addc != ID_EOS)
				ret=AddId(parseur,addc);

			apply_id_seq_to_nv_simple(inseq + xin, outseq + xout, nv, net_idx, ARC_destination (arc),parseur);

			if (ret)
				RemoveId(parseur);
		}

		/*IF SEQUENTIALIZED: Major change:  finish for loop on arcs if match found */
		if (NET_lower_seq(NV_net (nv, net_idx)) && xin)
			break;
	}
}

void apply_id_seq_to_nv_nstate_simple
	(id_type *inseq, id_type *outseq, NVptr nv, int net_idx, STATEptr state,void* parseur)
{
	/* For normal (non-vectorized) states !!!
	Applies the input sequence recursively to all arcs of one network,
	and to all networks of the net vector.
	It is the simple version of this function.
	The sigmas of all networks are supposed to be of type
	BINARY_VECTOR !!!*/

	ARCptr arc;  
	id_type inlab,  flag_attr, reg_value_old;
	int xin, xout;
	FLAG_DIACRptr flag;  
	ALPHABETptr flag_reg = NET_flag_register (NV_net (nv, net_idx));

	char compared=0;
	char ret=0;
	short* previous;

	/* case: sequence end */

	if (*inseq == ID_EOS)
	{

		if (CheckEditDistanceAction(parseur,action_prefix)) {
			AddEditDistanceAction(parseur,action_prefix_detected);
			LOOKUP_print_result_fn (inseq+1,parseur);
			RemoveEditDistanceAction(parseur,action_prefix_detected);
			return;
		}

		if (STATE_final (state))   /* input sequence is accepted */
		{
			*outseq = ID_EOS;   /* finish the output sequence */

			/* If the current net is the last one in the nv, we generate
			an ouput, i.e. we add a new path to the LOOKUP_RESULT net.
			The output sequence begins immediately after the input sequence,
			i.e. at inseq+1. */

			if (net_idx == NV_len (nv) - 1)
				LOOKUP_print_result_fn (inseq+1,parseur);

			/* If the current net is not the last one in the nv, we have to
			re-enter recursion with:
			- outseq (i.e. inseq+1) as new inseq,
			- the space after outseq (i.e. outseq+1) as new outseq,
			- the next network and its start state */

			else {
				compared=0;
				previous=CleanEditDistance(parseur);

				if (net_idx == NV_len (nv) - 2)
					compared=RemoveLastEditDistanceAction(parseur);

				apply_id_seq_to_nv_simple
				(inseq+1, outseq+1, nv, net_idx+1,
				NET_start_state (NV_net (nv, net_idx+1)),parseur);

				AddLastEditDistanceAction(parseur,compared,previous);
			}
		}

		/* continue matching with EPSILON only */

		for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
		{
			inlab = LOOKUP_INPUT_MATCH [ARC_label (arc)];
			flag = NULL;
			if ((inlab == EPSILON) || (inlab == ALTCHAIN_SYMBOL) ||
				(flag = flag_diacritic (inlab)) )
			{
				*outseq = LOOKUP_OUTPUT_MATCH [ARC_label (arc)];
				if (*outseq == OTHER)
					xout = 1;       /* one output symbol will be generated */
				else if ((*outseq == EPSILON) || (*outseq == ALTCHAIN_SYMBOL))
					xout = 0;       /* no output will be generated */
				else if (flag_diacritic_p (*outseq))
				{
					xout = 0;           /* no output will be generated */
					*outseq = EPSILON;   /* replace flag by EPSILON */
				}
				else  /* outlab == normal symbol */
					xout = 1;       /* one output symbol will be generated */

				/* continue mapping recursively */

				if (flag)
				{
					flag_attr = FLAG_DIACR_attribute (flag);
					reg_value_old = flag_value (flag_reg, flag_attr);

					if (flag_action (flag_reg, flag))     /* flag action ok */
						apply_id_seq_to_nv_simple
						(inseq, outseq+xout, nv, net_idx, ARC_destination (arc),parseur);

					flag_value (flag_reg, flag_attr) = reg_value_old;
				}
				else  /* not a flag */
					apply_id_seq_to_nv_simple
					(inseq, outseq + xout, nv, net_idx, ARC_destination (arc),parseur);
			}
		}

		if (EditDistanceAction(parseur,action_insert)) {
			//We accept that the current character might be changed into another...
			apply_a_loop_nb_state_simple(inseq, outseq, nv, net_idx, state,action_insert,parseur);
			EditDistancePop(parseur);
		}

		return;
	}

	/* case: not sequence end; try to map the current input symbol */
	for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
	{
		/* check whether the input ID is accepted by the arc */

		ret=0;
		inlab = LOOKUP_INPUT_MATCH [ARC_label (arc)];
		flag = NULL;
		if (inlab == OTHER)
		{
			if ((! binary_sigma_member (NET_sigma (NV_net (nv,net_idx)),*inseq))
				|| (*inseq == OTHER))
				xin = 1;   /* input accepted and one symbol consumed */
			else
				continue;  /* input not accepted by OTHER */
		}
		else if ((inlab == EPSILON) || (inlab == ALTCHAIN_SYMBOL) ||
			(flag = flag_diacritic (inlab))  )
		{
			if (ARC_destination (arc) != state)  /* avoid EPSILON cycles */
				xin = 0;   /* input accepted, but no symbol consumed */
			else
				continue;  /* input not accepted on EPSILON cycle */
		}
		else  /* inlab == normal symbol */
		{
			compared=1;
			if ((inlab == *inseq) || (*inseq == OTHER)) {
				xin = 1;   /* input accepted and one symbol consumed */				
			}
			else
				continue;  /* input not accepted */
		}

		/* handle output ID */

		*outseq = LOOKUP_OUTPUT_MATCH [ARC_label (arc)];
		if (*outseq == OTHER)
		{
			xout = 1;       /* one output symbol will be generated */
			if ((inlab == EPSILON) || (inlab == ALTCHAIN_SYMBOL) || (flag))
				*outseq = LOOKUP_UNKNOWN_SYMBOL;  /* a default symbol */
			else
				*outseq = *inseq;
		}
		else if ((*outseq == EPSILON) || (*outseq == ALTCHAIN_SYMBOL) ||
			(flag_diacritic_p (*outseq)) )
			xout = 0;       /* no output will be generated */
		else  /* outlab == normal symbol */
			xout = 1;       /* one output symbol will be generated */

		/* continue mapping recursively */

		if (flag)
		{
			flag_attr = FLAG_DIACR_attribute (flag);
			reg_value_old = flag_value (flag_reg, flag_attr);

			if (flag_action (flag_reg, flag))       /* flag action ok */
				apply_id_seq_to_nv_simple
				(inseq, outseq + xout, nv, net_idx, ARC_destination (arc),parseur);

			flag_value (flag_reg, flag_attr) = reg_value_old;
		}
		else {
			ret=0;
			if (xin==1) {
				if (inlab!=OTHER)
					ret=AddId(parseur,inlab);
				else
					ret=AddId(parseur,*inseq);
			}

			apply_id_seq_to_nv_simple
				(inseq + xin, outseq + xout, nv, net_idx, ARC_destination (arc),parseur);

			if (ret)
				RemoveId(parseur);
		}

		/*IF SEQUENTIALIZED: Major change:  finish for loop on arcs if match found */
		if (NET_lower_seq(NV_net (nv, net_idx)) && xin)
			break;
	}

	if (compared) {
		if (EditDistanceAction(parseur, action_delete)) {
			//we jump over the current character
			apply_id_seq_to_nv_simple(inseq + 1, outseq, nv, net_idx, state, parseur);
			EditDistancePop(parseur);
		}

		if (EditDistanceAction(parseur, action_change)) {
			//We accept that the current character might be changed into another...
			apply_a_loop_nb_state_simple(inseq, outseq, nv, net_idx, state, action_change, parseur);
			EditDistancePop(parseur);
		}

		if (EditDistanceAction(parseur, action_insert)) {
			//We accept that the current character might be changed into another...
			apply_a_loop_nb_state_simple(inseq, outseq, nv, net_idx, state, action_insert, parseur);
			EditDistancePop(parseur);
		}
	}
}

void apply_id_seq_to_nv_vstate_simple
	(id_type *inseq, id_type *outseq, NVptr nv, int net_idx, STATEptr state,void* parseur)
{
	/* For vectorized states !!!
	Applies the input sequence recursively to all arcs of one network,
	and to all networks of the net vector.
	It is the simple version of this function.
	The sigmas of all networks are supposed to be of type
	BINARY_VECTOR !!!*/

	STATEptr *arc_vector, dest;
	id_type inlab, *range_items [2], *items, flag_attr, reg_value_old;
	int n, range_len [2], xin, xout;
	int32 len;
	FLAG_DIACRptr flag;
	ALPHABETptr flag_reg = NET_flag_register (NV_net (nv, net_idx));
	char compared=0;
	char ret=0;
	short* previous;

	/* init */

	arc_vector = AV_dest_array (STATE_arc_vector (state));

	range_len [0] =
		RANGE_len (NET_uprange_map (NV_net (nv, net_idx)) + EPSILON);
	range_items [0] =
		RANGE_items (NET_uprange_map (NV_net (nv, net_idx)) + EPSILON);

	if ((inlab = *inseq) != ID_EOS)   /* input label exists */
	{
		if (binary_sigma_member (NET_sigma (NV_net (nv, net_idx)), inlab))
		{
			range_len [1] =
				RANGE_len (NET_uprange_map (NV_net (nv, net_idx)) + inlab);
			range_items [1] =
				RANGE_items (NET_uprange_map (NV_net (nv, net_idx)) + inlab);
		}
		else  /* input label can only be accepted by OTHER */
		{
			range_len [1] =
				RANGE_len (NET_uprange_map (NV_net (nv, net_idx)) + OTHER);
			range_items [1] =
				RANGE_items (NET_uprange_map (NV_net (nv, net_idx)) + OTHER);
		}
	}

	/* case: sequence end */

	if (inlab == ID_EOS)
	{
		if (CheckEditDistanceAction(parseur,action_prefix)) {
			AddEditDistanceAction(parseur,action_prefix_detected);
			LOOKUP_print_result_fn (inseq+1,parseur);
			RemoveEditDistanceAction(parseur,action_prefix_detected);			
			return;
		}

		if (STATE_final (state))   /* input sequence is accepted */
		{
			*outseq = ID_EOS;   /* finish the output sequence */

			/* If the current net is the last one in the nv, we generate
			an ouput, i.e. we add a new path to the LOOKUP_RESULT net.
			The output sequence begins immediately after the input sequence,
			i.e. at inseq+1. */

			if (net_idx == NV_len (nv) - 1)
				LOOKUP_print_result_fn (inseq+1,parseur);

			/* If the current net is not the last one in the nv, we have to
			re-enter recursion with:
			- outseq (i.e. inseq+1) as new inseq,
			- the space after outseq (i.e. outseq+1) as new outseq,
			- the next network and its start state */

			else {
				compared=0;
				previous=CleanEditDistance(parseur);

				if (net_idx == NV_len (nv) - 2)
					compared=RemoveLastEditDistanceAction(parseur);

				apply_id_seq_to_nv_simple
				(inseq+1, outseq+1, nv, net_idx+1,
				NET_start_state (NV_net (nv, net_idx+1)),parseur);
				AddLastEditDistanceAction(parseur,compared,previous);
			}
		}

		/* continue matching with EPSILON only */

		for (len = range_len [0], items = range_items [0];
			len > 0;
			len--, items++)
			if ((dest = arc_vector [*items]))
			{
				flag = flag_diacritic (LOOKUP_INPUT_MATCH [*items]);
				*outseq = LOOKUP_OUTPUT_MATCH [*items];

				if (*outseq == OTHER)
				{
					xout = 1;       /* one output symbol will be generated */
					*outseq = LOOKUP_UNKNOWN_SYMBOL;  /* a default symbol */
				}
				else if ((*outseq == EPSILON) || (*outseq == ALTCHAIN_SYMBOL) ||
					(flag_diacritic_p (*outseq)) )
					xout = 0;       /* no output will be generated */
				else  /* outlab == normal symbol */
					xout = 1;       /* one output symbol will be generated */

				/* continue mapping recursively */

				if (flag)
				{
					flag_attr = FLAG_DIACR_attribute (flag);
					reg_value_old = flag_value (flag_reg, flag_attr);

					if (flag_action (flag_reg, flag))       /* flag action ok */
						apply_id_seq_to_nv_simple
						(inseq, outseq + xout, nv, net_idx, dest,parseur);

					flag_value (flag_reg, flag_attr) = reg_value_old;
				}
				else
					apply_id_seq_to_nv_simple
					(inseq, outseq + xout, nv, net_idx, dest,parseur);
			}

			return;
	}

	/* case: not sequence end; try to map the current input symbol */

	for (n = 0; n < 2; n++)
		for (len = range_len [n], items = range_items [n];
			len > 0;
			len--, items++)
			if ((dest = arc_vector [*items]))
			{
				/* handle input label */
				ret=0;
				inlab = LOOKUP_INPUT_MATCH [*items];
				flag = flag_diacritic (inlab);

				if (n == 0)   /* any ID from EPSILON range_map */
				{
					if (dest != state)  /* avoid EPSILON cycles */
						xin = 0;   /* input accepted, but no symbol consumed */
					else
						continue;  /* input not accepted on EPSILON cycle */
				}
				else  { /* OTHER or real symbol */
					xin = 1;   /* input accepted and one symbol consumed */					
				}

				/* handle output ID */

				*outseq = LOOKUP_OUTPUT_MATCH [*items];
				if (*outseq == OTHER)
				{
					xout = 1;       /* one output symbol will be generated */
					if (n == 0)     /* i.e. if(inlab == EPSILON) */
						*outseq = LOOKUP_UNKNOWN_SYMBOL;  /* a default symbol */
					else
						*outseq = *inseq;
				}
				else if ((*outseq == EPSILON) || (*outseq == ALTCHAIN_SYMBOL) ||
					(flag_diacritic_p (*outseq)) )
					xout = 0;       /* no output will be generated */
				else  /* outlab == normal symbol */
					xout = 1;       /* one output symbol will be generated */

				/* continue mapping recursively */

				if (flag)
				{
					flag_attr = FLAG_DIACR_attribute (flag);
					reg_value_old = flag_value (flag_reg, flag_attr);

					if (flag_action (flag_reg, flag))       /* flag action ok */
						apply_id_seq_to_nv_simple
						(inseq + xin, outseq + xout, nv, net_idx, dest,parseur);

					flag_value (flag_reg, flag_attr) = reg_value_old;
				}
				else {
					ret=0;
					if (xin==1) {
						if (inlab!=OTHER)
							ret=AddId(parseur,inlab);
						else
							ret=AddId(parseur,*inseq);
					}

					apply_id_seq_to_nv_simple
						(inseq + xin, outseq + xout, nv, net_idx, dest,parseur);

					if (ret)
						RemoveId(parseur);
				}

				/*IF SEQUENTIALIZED: Major change:  finish for loop on arcs if match found */
				if (NET_lower_seq(NV_net (nv, net_idx)) && xin)
					break;
			}
}

/*===========================================================*/

NETptr apply_input_seq_to_nv (NVptr nv, int input_side, int minimize_p,void* parseur)
{
	/* Tries to apply the input ID sequence to the net vector and returns
	the resulting network. If there is no result, NULL will be returned. */

	ALPHABETptr flag_reg;  
	STATEptr chk_state;
	id_type *seq, *outseq;  
	int i;
	char ret;

	/* make and clean flag_register */

	for (i=0; i< NV_len (nv); i++)
		if ((NET_u_flag_diacr (NV_net (nv, i))) ||
			(NET_l_flag_diacr (NV_net (nv, i)))  )
		{
			if (! (flag_reg = NET_flag_register (NV_net (nv, i))))
				NET_flag_register (NV_net(nv,i)) = flag_reg = make_flag_register();

			memset (flag_reg->items, 0, flag_reg->len * sizeof (id_type));
		}
		/* --- */

		seq = LOOKUP_ID_SEQUENCE;
		outseq = seq + (length_of_sequence (seq) + 1);

		if (LOOKUP_FLAGS.make_result_net)
			LOOKUP_RESULT = null_fsm ();  /* results will add pathes */
		else
			LOOKUP_RESULT = NULL;   /* results will set it to NULL+1 */
		
		if ((LOOKUP_FLAGS.apply_with_check) && (NV_len (nv) >= 2))
		{
			chk_state = CHK_start_state (NV_net (nv, 1));

			apply_id_seq_to_nv_checked(seq, outseq, nv, 0, NET_start_state (NV_net (nv, 0)), chk_state,parseur);
		}
		else {
			ret=RemoveEditDistanceAction(parseur,action_last);
			apply_id_seq_to_nv_simple(seq, outseq, nv, 0, NET_start_state (NV_net (nv, 0)),parseur);
			if (ret)
				AddEditDistanceAction(parseur,action_last);
		}

		if (! LOOKUP_FLAGS.make_result_net)
			return (LOOKUP_RESULT);    /* result already printed */

		update_label_alphabet (LOOKUP_RESULT);
		NET_sigma (LOOKUP_RESULT) = label_sigma (LOOKUP_RESULT);

		if (minimize_p == DO_MINIMIZE)
		{
			NET_minimized (LOOKUP_RESULT) = FALSE;
			NET_deterministic (LOOKUP_RESULT) = FALSE;
			determinize_fsm (LOOKUP_RESULT, DONT_COPY);
			min_fsm (LOOKUP_RESULT, DONT_COPY);
		}

		/* check whether there is a result */

		if ((! NET_num_arcs (LOOKUP_RESULT))
			&& (! STATE_final (NET_start_state (LOOKUP_RESULT))))
		{
			free_network (LOOKUP_RESULT);
			return (NULL);        /* no result */
		}

		return (LOOKUP_RESULT);
}

void lookup_input_and_write_result_andre (void* parseur)
{
	/* Tries to apply the input line to the net vectors contained in
	STRATEGIES, and returns as soon as a result is found. */

	NETptr res;  int s;
	//static char buff[1000];

	for (s=0; s< NVV_len (STRATEGIES); s++)
		if ((res = apply_input_seq_to_nv
			(NVV_nv (STRATEGIES, s), LOWER, DO_MINIMIZE,parseur)))
		{
			SUCCESS_COUNT [s] ++;
			break;
		}

		NUM_WORDS_TOTAL++;

		if (res)
		{
			if (LOOKUP_FLAGS.make_result_net)
			{                     /* result is a net and not yet printed */
				write_result_net_to_output(NET_start_state (res), LOOKUP_OUTPUT_BUFFER, TRUE);
				free_network (res);
			}
		}
		else
		{
			NUM_WORDS_NOT_FOUND++;

			//fprintf(stdout, "%s\t%s\t%s\n", LOOKUP_INPUT_WORD, LOOKUP_INPUT_WORD,LOOKUP_UNKNOWN_WORD_TAG);
			insert_token(LOOKUP_INPUT_WORD,parseur);
			insert_token_char(9,parseur,0);
			add_token(LOOKUP_INPUT_WORD,parseur);
			insert_token_char(9,parseur,0);
			add_token(LOOKUP_UNKNOWN_WORD_TAG,parseur);
		}
}

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/*  reads LOOKUP_ID_SEQUENCE - stops at Token Boundary: ID_EOS */
/* ** LOOKUP_ID_SEQUENCE is allocated according to the needs of Andre's
algorithm - should be changed (to smaller) later ** */

void clear_and_free_configs_cells(CONFIGptr cur_configs)
{
	CONFIGptr config, alt;

	for (config = cur_configs; config; config = alt)
	{
		alt = CONFIG_next(config);
		free_config_and_output_cells(config);
	}
}

void produce_tokens_by_reverse_configs (CONFIGptr config, char*parseur)
	/* This is to reverse the order at multiple findings (miming Andre Kempe's
	order) */
{
	if(config)
	{
		produce_tokens_by_reverse_configs (CONFIG_next(config),parseur);

		if (! LOOKUP_FLAGS.dont_copy_input)
		{
			fprintf (LOOKUP_OUTPUT_FILE, "%s", LOOKUP_INPUT_WORD); 
		}

		fprintf (LOOKUP_OUTPUT_FILE, "%s", LOOKUP_FLAGS.surf_lemma_sep);
		lemma_tag_sep_allowed = TRUE;
		if (XELDA_STYLE_PRINT)
		{
			lk_xelda_b_p = LK_XELDA_BUFFER;
		}
		LKproduce_tokens_nobranching_det (CONFIG_output_cell(config), NULL, CONFIG_output_cell(config) ? CONFIG_output_cell(config)->refcount : 0,parseur);
		if (!XELDA_STYLE_PRINT)
		{
			fputc('\n',LOOKUP_OUTPUT_FILE);  /* output token separator */
		}
		else
		{
			*lk_xelda_b_p++ = ID_EOS; /* end of string marker */
			lk_xelda_print(LK_XELDA_BUFFER);
		}
		free_config_and_output_cells (config);
	}
}

void produce_tokens_by_configs (CONFIGptr config, int xelda_style_print,void* parseur) {

	if(config) {    
		/* we can't use a for loop here because the config->next field must
		not be accessed after config has been freed (see 2 last line of loop) */
		while (config != NULL) {

			CONFIGptr next;

			if (! xelda_style_print) {

				if (! LOOKUP_FLAGS.dont_copy_input) {

					//fprintf (LOOKUP_OUTPUT_FILE, "%s", LOOKUP_INPUT_WORD); 
					insert_token(LOOKUP_INPUT_WORD,parseur);
				}
				//fprintf (LOOKUP_OUTPUT_FILE, "%s", LOOKUP_FLAGS.surf_lemma_sep);
				insert_token_char(*LOOKUP_FLAGS.surf_lemma_sep,parseur,0);
				lemma_tag_sep_allowed = TRUE;
			}
			else
				lk_xelda_b_p = LK_XELDA_BUFFER;

			LKproduce_tokens_nobranching_det (CONFIG_output_cell(config), 
				NULL, 
				CONFIG_output_cell(config) ? CONFIG_output_cell(config)->refcount : 0,parseur);

			if (! xelda_style_print) {
				//fputc('\n',LOOKUP_OUTPUT_FILE);  /* output token separator */
				insert_token_char('\n',parseur,1);
			}
			else {

				*lk_xelda_b_p++ = ID_EOS;
				lk_xelda_print(LK_XELDA_BUFFER);
			}
			/* store the next field before freeing config! */
			next = CONFIG_next(config);
			free_config_and_output_cells (config);
			config = next;
		}
	}
}

/* Breadth-first algorithm - control part */
/* --------------------------------------------------------------------- */
/* a "token line", that is, a sequence of label_id-s representing input
symbols is treated here */
/* an NVptr is being matched here - that is, a networks vector */
/* side of the transducer matched */
/* one token (often word but not always, see eg multi-word expressions) is
processed by one call. This may change in future */
/* The nets involved has been vectorized by Andre - check it */

/* OUTPUT algorithm : by CONFIG-s;  but retaining out_cell-s if there was a
higher refcount.  See also produce_tail_tokens
*/

/* N1 to N2 fst  transition : by stacking up N2 Config-s on N1_config-s
according to label_id-s in N1_config-s. Then delete N1_config-s.
OR CASH THEM !
Nn to L1  transition: do as in N1 to N2
*/

/* if fst-s finished, print result. Beware empty config-s everywhere */

/* --------------------------------------------------------------------- */
/* --------------------------------------------------------------------- */
/* --------------------------------------------------------------------- */


/* Improved breadth-first algorithm - control part */


STR_CONTROLptr STRATS_CONTROL ; 
unsigned int STR_NO = 0;

RANGEptr other_range(NETptr prev, NETptr this)
	/*
	Special range item for OTHER handling in cases where the input may contain
	OTHER.  This OTHER may be passed on in simulating composition networks.
	The range contains OTHER on its input side and in its range all labels of
	THIS net of  which the input can be considered as OTHER for the PREVious
	net, as well as all labels where OTHER appears on the input (lower) side.
	Beware: it needs at least 2 nets. If OTHER as input is possible for a
	single net (not now, as of  2000-2-7) this should be revised.
	*/
{
	ALPHABETptr sigdiff = NULL;
	id_type  llabel, llow;
	int i, j, cnt;
	RANGEptr result = NULL;

	if (! this)
		return result;
	if (! prev)
		return result;

	if (! ( sigma_member ( NET_sigma(this), OTHER) ||  sigma_member ( NET_sigma(prev), OTHER)))
		return(result);

	if (! NET_sigma(this) || ! NET_sigma(prev))
	{
		fprintf (stderr, "** other-range(): no sigma. Sigma of prev = %d; sigma of this = %d.\n",
			(int) NET_sigma(this), (int) NET_sigma(prev));
		return(result);
	}

	if (! NET_labels(this) || ! NET_labels(prev))
	{
		fprintf (stderr, "** other-range(): no labels. Labels alphabet of of prev = %d; of this = %d.\n",
			(int) NET_labels(this), (int) NET_labels(prev));
		return(result);
	}

	sigdiff = sigma_difference(NET_sigma(this), NET_sigma(prev));

	if (! sigdiff || (sigdiff && ( ALPH_len(sigdiff) == 0)))
		return result;

	cnt = 0;
	for (  i = 0 ;  i < ALPH_len(NET_labels(this)) ; i++)
	{
		llabel = ALPH_item(NET_labels(this), i);
		llow = lower_id(llabel);
		if (( llow && ALPH_item(sigdiff, llow)) ||
			llow == OTHER)
		{
			cnt++;
		}
	}

	if (!cnt)
	{
		free_alph(sigdiff);
		return(result);
	}

	result = (RANGEptr) calloc((size_t) 1, (size_t) sizeof(RANGEtype));
	set_range(result, OTHER, cnt);

	for (  i = j = 0 ;  i < ALPH_len(NET_labels(this)) ; i++)
	{
		llabel = ALPH_item(NET_labels(this), i);
		llow = lower_id(llabel);
		if (( llow && ALPH_item(sigdiff, llow)) ||
			llow == OTHER)
		{
			RANGE_item(result, j) = llabel;
			j++;
		}
	}

	free_alph(sigdiff);
	return(result);
}

static ALPHABETptr * create_static_other_exception_map(int net_order, NET_CONTROLptr strat)
	/*  
	Various possibilities for input OTHER symbols in cascades of fst-s
	necessitate handling of variable number, long-distance context-dependent 
	constraints on OTHER symbols. This constraints can accumulate due to
	intermediate identity OTHER symbols where the sigma of the respective
	intermediate fst contains the OTHER symbol and some other, different
	(from OTHER) symbols, too. In such a case, these appear as additional
	constraints on an input OTHER. An input OTHER can have one or zero
	dynamic constraints on it, depending on the arc label matched and the
	input symbol that caused the match. 
	To facilitate constraint handling, constraints are divided to the dynamic
	constraint (0 or one of them) and a static, but possibly big set. This set
	depends on the previous (in the cascade) fst-s: if the previous fst contains
	an identity OTHER arc (i.e. label) and its sigma has more than the OTHER
	symbol in it then all the other symbols are constraints for an incoming - for
	the previous fst - OTHER (which can match an identity OTHER arc there).

	Now, if somewhere in the cascade an output OTHER is created, say at net[i],
	then net[i+1] will receive it as an input OTHER, created at net[i], with a
	possible dynamic constraint (eg. ?:? matched for an input "a" then "a" will
	be the constraint). Since  net[i=1], net[i+2], ..., net[net_order-1] may all
	contain identity OTHER-s in their respective sigma alphabets, and those sigmas
	may contain more, further constraining other (i.e. not OTHER) symbols,
	net[net_order] must be able to determine the constraint set wrt. net[i].
	That is, it has to see its constraints for all net[i] fst-s (where
	i<net_order).  Since net_order is numbered from 0 (C array indexing
	convention) net_order means the (net_order+1_ fst (if we count form the first)
	so static_other_exception_map[net_order] will be returned.
	It contains alphabets indexed by the absolute order of the (possibly remote)
	predecessor in the cascade that may have caused an input OTHER. Such an
	alphabet contains the accumulated constraints from net[i] up to
	net[net_order]. 	 
	Such a constraint alphabet is only necessary for handling identity OTHER arcs
	that match an incoming OTHER symbol.
	*/
{
	ALPHABETptr actual=NULL, * static_other_exception_map=NULL;
	int can_do, i;

	if (!net_order)
		return static_other_exception_map;
	else
		static_other_exception_map = (ALPHABETptr *) calloc((size_t) net_order,
		(size_t) sizeof(ALPHABETtype));

	for (i = 0, can_do = TRUE ;  i < net_order && can_do ; i++)
	{
		if (sigma_member( NET_sigma(strat[net_order-i-1].net), OTHER))
		{
			if (! sigma_member( NET_labels(strat[net_order-i-1].net), OTHER))
				can_do = FALSE;
			actual = sigma_union(actual, NET_sigma(strat[net_order-i-1].net));
			sigma_remove_from(actual, OTHER); /* may seem to be pedantic since OTHER
											  can't be a constraint on OTHER anyway */
			if (ALPH_len(actual)) /* if there are symbols */
				static_other_exception_map[net_order-i -1] = actual;
			else
			{
				free_alph(actual);
				actual = NULL;
			}
		}
		else
			break;
	}

	return  static_other_exception_map;
}

static void free_static_other_exception_map(ALPHABETptr * soem, int length)
{
	int i;

	if (!soem)
		return;

	for (i = 0; i < length ; i++)
	{
		if(soem[i])
			free_alph(soem[i]);
	}
	free(soem);
}

void init_strat_control( NVptr nv, NET_CONTROLptr str_c, int max_len_inp_line)
{
	int n;

	/* 
	allocate n NET_CONTROL vectors and fill them for this
	strategy */
	for (n=0;      n <NV_len(nv) ;  	       n++)
	{
		str_c[n].net =  NV_net(nv,n);
		str_c[n].lsigma = lower_sigma_of_net (NV_net(nv,n));

		if (n)
			str_c[n].range_of_OTHER = other_range(NV_net(nv,n-1), NV_net(nv,n));
		else
			str_c[n].range_of_OTHER = NULL;

		str_c[n].other_exception_map = create_static_other_exception_map(n, str_c);

		str_c[n].prev_symbol_result = NULL;
		str_c[n].temp_result = NULL;
		str_c[n].norm_id_sequence = (id_type *) mem_alloc(max_len_inp_line * sizeof (id_type));

		if (NET_reduced_labelset(NV_net(nv,n)) &&
			(! NET_upmatch_table(NV_net(nv,n))))
			NET_upmatch_table(NV_net(nv,n)) = get_match_table(NV_net(nv,n), LOWER);

	}
}

void init_strat_control_strategy (int strategy, NVptr nv, int max_len_inp_line)
{
	if (STRATS_CONTROL[strategy].nv == NULL)
	{/* The STR_CONTROL of the strategy has not yet been initialized. */
		STRATS_CONTROL[strategy].nv= nv;
		STRATS_CONTROL[strategy].netno = NV_len(nv);
		STRATS_CONTROL[strategy].nets_ctr =  calloc( (NV_len(nv)) , sizeof( NET_CONTROL));
		init_strat_control(nv, STRATS_CONTROL[strategy].nets_ctr, max_len_inp_line);
	}
}


STR_CONTROLptr init_strats_contr_memory (int num_strategies)
{
	STRATS_CONTROL =  calloc( num_strategies , sizeof( STR_CONTROL));
	STR_NO = num_strategies;
	return STRATS_CONTROL;
}

void init_strats_contr ()
{
	int s; 

	if (STRATEGIES)
	{
		init_strats_contr_memory(NVV_len (STRATEGIES));

		for (s=0; s< NVV_len (STRATEGIES); s++)
		{
			init_strat_control_strategy(s, NVV_nv(STRATEGIES, s),
				(LOOKUP_FLAGS.len_inp_line +1));
		}
	}
}

void free_strats_control( )
{
	int n, s;
	if (STRATS_CONTROL != NULL)
	{
		for (s=0;     s <  STR_NO ; s++)
		{
			for (n=0;      n <STRATS_CONTROL[s].netno  ;  	       n++)
			{
				free (STRATS_CONTROL[s].nets_ctr[n].norm_id_sequence);
				free_static_other_exception_map(STRATS_CONTROL[s].nets_ctr[n].other_exception_map, n);
				if (STRATS_CONTROL[s].nets_ctr[n].range_of_OTHER)
					free(STRATS_CONTROL[s].nets_ctr[n].range_of_OTHER);
			}
			free (STRATS_CONTROL[s].nets_ctr);
		}
		free ( STRATS_CONTROL);
		STRATS_CONTROL = NULL;
	}
}


void reset_strat_control( int16 s, NVptr nv )
{
	int n;
	CONFIGptr  new_config; 

	STRAT_CONTROL = STRATS_CONTROL[s].nets_ctr;
	SYMBOL_COUNT = 0; 
	/* Now SYMBOL_COUNT has a central role because an out_chain element is
	indexed with the actual SYMBOL_COUNT. A next cascading fst reads only
	relevant chains segments (those indexed with the actual SYMBOL_COUNT)
	when inheriting a set of CONFIG-s for further processing. The CONFIG-s
	point to chains of symbols built up so far. */
	ANCESTOR_MARK = 1; /* by ANCESTOR_MARK valid offsprings are traced both
					   symbol-number-wise and fst-wise */

	for (n=0;      n <STRATS_CONTROL[s].netno ;  	       n++)
	{

		STRAT_CONTROL[n].temp_result = NULL;
		new_config = cell_alloc(LK_CONFIG_HEAP);
		CONFIG_fst_state(new_config) = NET_start_state(NV_net(nv,n));
		CONFIG_output_cell(new_config) = NULL;
		CONFIG_next(new_config) = NULL;
		CONFIG_count(new_config) = SYMBOL_COUNT;	  
		CONFIG_own(new_config) =   ANCESTOR_MARK;
		ANCESTOR_MARK++;
		CONFIG_s_papa(new_config) = 0; 
		if (n > 0)
		{
			CONFIG_c_papa(new_config) = CONFIG_own( STRAT_CONTROL[n-1].prev_symbol_result);
		}
		else /* first fst - manual setting of the very first "config ancestry" */
		{
			CONFIG_c_papa(new_config) = 0;
		}
		if (NET_l_flag_diacr (NV_net(nv,n)))
			CONFIG_flag_d_reg (new_config) = make_flag_register();
		else
			CONFIG_flag_d_reg (new_config) = NULL;
		STRAT_CONTROL[n].prev_symbol_result = new_config;
	}
}

int apply_input_seq_to_nv_tamas ( NVptr nv,void* parseur)  {

	int n, result=FALSE;
	/* id_type id, *iid; */
	CONFIGptr cur_configs = NULL,  cf;
	RES_CONFStype res_css;

	if ( *LOOKUP_ID_SEQUENCE == ID_EOS) /* empty line */
		return(FALSE); /* ** further message ? */

	if (nv) {

		NET_NO = NV_len(nv);

		/* normalizing transducers compose their effects via config-s
		passed as call parameter */
		res_css = fst_subcomposition (nv,
			0, /* first fst; will call next */
			NULL, 
			TRUE);
		if ( ( cur_configs = res_css.head))
			result = TRUE;
		else
			result = FALSE;
	}
	else {

		if (LOOKUP_FLAGS.show_comments)
			fprintf (stderr, "** nv (networks vector) is nil.\n");
		return(FALSE);
	}
	/* print result token(s) - looked-up-string(s) - here */
	if (result) {
		/* find config-s of LAST fst ("lexical") and finish them.
		release all other config-s everywhere */

		if (NET_NO == 1) /* only one fst */
			cf = produce_tail_tokens_det (& cur_configs, NV_net(nv,0));
		else {
			/* computing all tail_tokens and verifying in turn */

			/* All fst-s must get into a final state! */
			for (n=0;      n < NET_NO ;  	       n++) {

				/* create final tail tokens and check them on the next fst */
				if (n==0) {

					cf = produce_tail_tokens(& cur_configs,  n, nv);
					/* 6.09.96 : throw away this result from this
					first fst (n == 0) : it only served
					to "normalize" further fst-s) */		   
					if (cf)  
						clear_and_free_configs_cells(cf);
				}
				else {

					cf = produce_tail_tokens
						(&(STRAT_CONTROL[n].prev_symbol_result), n, nv);
					STRAT_CONTROL[n].prev_symbol_result = cf; /* may have changed */
				}
				if (!cf) {

					result = FALSE;
					break;
				}
			}
		}
		if ( (cur_configs = cf) )
			produce_tokens_by_configs(cur_configs, XELDA_STYLE_PRINT,parseur);
		else
			result = FALSE;
		/* release intermediate temp results - but not the last ("lexical") one because
		others have already done that - ** and not the first one either...  */  
		for  (n=1;      n < NET_NO -1 ;  	       n++) {

			clear_and_free_configs_cells(STRAT_CONTROL[n].prev_symbol_result);
		}
	}
	/*
	for (config = cur_configs; config; config = alt)
	{
	alt = CONFIG_next(config);
	iid=LOOKUP_ID_SEQUENCE;
	while ( *iid != ID_EOS )
	{
	print_label(*iid, stream, DONT_ESCAPE);
	iid++;
	}
	fprintf (out_stream, OUTPUT_SEPARATOR);
	LKproduce_tokens_nobranching_det (CONFIG_output_cell(config), NULL,CONFIG_output_cell(config)->refcount, out_stream);
	fprintf(out_stream,"\n"); 
	free_config(config);
	}
	*/
	else
		clear_and_free_configs_cells(cur_configs);

	return (result);
}

void lookup_input_and_write_result_tamas (void* parseur)
{
	/*  The input line is given to the net vectors contained in
	STRATEGIES; returns as soon as a result is found. */

	int s, res=FALSE;
	NVptr nv;
#ifdef LKDEBUG_OUTCELL_HEAP_SIZE
	long tally = 0;
	long ctally = 0;
#endif

	if (STRATEGIES)
	{
		for (s=0; s< NVV_len (STRATEGIES); s++)
		{
			nv=NVV_nv(STRATEGIES, s);
#ifdef LKDEBUG_OUTCELL_HEAP_SIZE
			tally_heap(LK_OUT_CELL_HEAP, &tally, stdout);
			tally_heap(LK_CONFIG_HEAP, &ctally, stdout);
#endif
			reset_strat_control (s, nv); 

			if ( ( res = apply_input_seq_to_nv_tamas (nv,parseur)) )
			{
				SUCCESS_COUNT [s] ++;
				break;
			}
		}
	}
	else   /* no strategy given */
		if (LOOKUP_FLAGS.show_comments)
			fprintf (stderr, "** No STRATEGIES given. Quit Lookup");
		else 
		{
			fprintf (stderr, "*** No STRATEGIES given. Exit Lookup in panic!");
			exit(1);
		}

		if (! res) /* no strategy found any match...  too bad ... */
		{
			NUM_WORDS_NOT_FOUND++;
			fprintf(stdout, "%s\t%s\t%s\n", LOOKUP_INPUT_WORD, LOOKUP_INPUT_WORD,
				LOOKUP_UNKNOWN_WORD_TAG);	        
		}

		NUM_WORDS_TOTAL++; /* this word (or rather token) has been processed anyway */

		/* *** Hack to fix a complicated memory-leak pb */
		{
			reset_heap(LK_CONFIG_HEAP);
			reset_heap(LK_OUT_CELL_HEAP);
		}
}

void look_input_and_write_result_claude (void* parseur)
{
	/* Tries to apply the input line to the net vectors contained in
	STRATEGIES, and returns as soon as a result is found. */

	NETptr res;  int s;
	//static char buff[1000];

	for (s=0; s< NVV_len (STRATEGIES); s++) {
		if ((res = apply_input_seq_to_nv
			(NVV_nv (STRATEGIES, s), UPPER, DO_MINIMIZE,parseur)))
		{
			SUCCESS_COUNT [s] ++;
			break;
		}
	}

	NUM_WORDS_TOTAL++;

	if (res)
	{
		if (LOOKUP_FLAGS.make_result_net)
		{                     /* result is a net and not yet printed */
			write_result_net_to_output(NET_start_state (res), LOOKUP_OUTPUT_BUFFER, TRUE);
			free_network (res);
		}
	}
	else
	{
		NUM_WORDS_NOT_FOUND++;

		//fprintf(stdout, "%s\t%s\t%s\n", LOOKUP_INPUT_WORD, LOOKUP_INPUT_WORD,LOOKUP_UNKNOWN_WORD_TAG);
		insert_token(LOOKUP_INPUT_WORD,parseur);
		insert_token_char(9,parseur,2);
		add_token("?",parseur);
	}
}

void lookup_input_and_write_result_word_to_number ()
{
	long int * res = NULL;
	int i, max, s = 0;
	PARSE_TBL parse_tbl = NULL;
	NVptr nv;
	NETptr fst;

	if (!STRATEGIES)
	{
		if (LOOKUP_FLAGS.show_comments)
			fprintf (stderr, "** No STRATEGIES given. Quit Lookup");
		else 
		{
			fprintf (stderr, "*** No STRATEGIES given. Exit Lookup in panic!");
			exit(1);
		}
	}

	for (s=0; s< NVV_len (STRATEGIES); s++)
	{
		if (s >0)
		{
			fprintf (stderr, "*** for STRATEGIES not yet implemented - only for a single fst. Exit Lookup in panic!\n");
			exit(1);
		}
		nv=NVV_nv(STRATEGIES, s);
		fst = NV_net(nv,0);
		NET_lower_parse_table(fst) =  parse_tbl =  make_parse_table(fst, NULL, LOWER);

		if ( (res =   word_to_numbers( LOOKUP_INPUT_WORD, fst, LOWER)) && (( max = path_index_count()) >0) )
		{
			for (i = 0 ; i < max ; i++)
			{
				if (! XELDA_STYLE_PRINT)
				{
					if (! LOOKUP_FLAGS.dont_copy_input)
						fprintf (LOOKUP_OUTPUT_FILE, "%s", LOOKUP_INPUT_WORD);
					fprintf (LOOKUP_OUTPUT_FILE, "%s", LOOKUP_FLAGS.surf_lemma_sep);
					if (lemma_tag_sep_allowed )
					{
						fprintf(LOOKUP_OUTPUT_FILE, LOOKUP_FLAGS.lemma_tag_sep);
					}
					fprintf(LOOKUP_OUTPUT_FILE, "%ld\n", res[i]);
				}
				else
				{
					fprintf(stderr, "*** lookup_input_and_write_result_word_to_number(): xelda number result not yet implemented.\n"); 
				}
			}
			SUCCESS_COUNT [s] ++;
		}
		else
		{
			NUM_WORDS_NOT_FOUND++;
			fprintf(stdout, "%s\t%s\t%s\n", LOOKUP_INPUT_WORD, LOOKUP_INPUT_WORD,
				LOOKUP_UNKNOWN_WORD_TAG);
		}
	}
	NUM_WORDS_TOTAL++; /* this word (or rather token) has been processed anyway */
}

/* ================================================================= */
/*            Integration of the compose-apply algorithm             */
/* ================================================================= */

/* "Parsing" input function for the compose-apply algorithm:
For a given strategy, the input string of the first context
in the cascade is the word to lookup:
Note that the compose-apply algorithm takes its input from
a (char*) string, whereas the lookup application provides
its input as a (id_type*) string. Consequenlty we cast the
parsed input contained in LOOKUP_ID_SEQUENCE on a (char*)
and store it in the APPLY_input() of the first_context. Then
we will use function read_id_from_str() as the next_symbol_fn
of the first_context:
it just re-casts the current input item to an (id_type*). 
Note that this function never fails but it must be aligned
with the prototype of APPLY_next_symbol_fn() : */
static int read_id_from_str(id_type *in_sym, void * context)
{
	/* Retrieves the next symbol id from the input string of id-s stored
	in the "input" field of the apply_context structure.
	It sets in_sym to ID_NO_SYMBOL if there's no more symbol to read.
	This function is to be assigned to APPLY_next_symbol_fn of the
	first apply_context.   */

	static pos=0;
	id_type* id_string= NULL;

	if (APPLY_in_pos((APPLYptr) context) == 0)
	{
		/* Reset the position for the next input string to be read:  */
		pos= 0;
	}

	id_string= (id_type*) APPLY_input((APPLYptr) context);

	*in_sym= *(id_string+pos);

	/* Set the position to the next id in the input string:     */
	pos++;

	return(NO_ERROR);    
} /* read_id_from_str() */


/* Initialization step required for the compose-apply algorithm:
this function builds the global array of apply_context-s named
STRATEGIES_APPLY_CONTEXT. */
APPLYptr * init_compose_apply_for_lookup()
{
	/* Build for once an apply_context structure for each strategy. */
	/* To be called only when LOOKUP_ALGORITHM is 3 i.e. in the
	compose-apply case. */

	int net_i= 0;
	int strat_index=0;
	int number_of_strat= 0;

	APPLYptr * strategies_apply_contexts= NULL;

	APPLYptr apply_context= NULL; 
	APPLYptr first_context= NULL;

	APPVECptr appvec= NULL;

	NVptr strat= NULL;
	NVptr cascade= NULL;

	select_match_tables_for_apply(LOWER);

	number_of_strat= NVV_len (STRATEGIES);

	/* Allocate an array of apply_context pointers, 1 for each strategy */
	strategies_apply_contexts= (APPLYptr*) malloc(number_of_strat*sizeof(APPLYptr));

	/* Loop on the strategies: */      
	for (strat_index=0; strat_index < number_of_strat; strat_index++)
	{ 
		/* Create an APPLY_CONTEXT structure for this strategy: */
		apply_context= (APPLYptr) malloc (sizeof(APPLYtype));
		memset (apply_context, 0, sizeof(APPLYtype));
		strategies_apply_contexts[strat_index]= apply_context;

		/* Set the apply parameters of the context structure: */
		APPLY_side(apply_context) = LOWER;
		APPLY_out_side(apply_context) = UPPER;
		APPLY_obey_flags_p(apply_context) = TRUE;

		/* Storage of the cascade of nets to apply: */
		strat= NVV_nv(STRATEGIES, strat_index);

		/* The compose-apply algorithm uses the cascade of nets in the order 
		opposite to the order set in the net_vector strat: consequently we
		allocate a new NV and store inside the inverse of "strat". */
		cascade= make_nv(NV_len(strat));

		for(net_i= 0; net_i < NV_len(cascade); net_i++)
		{
			NV_net(cascade, net_i)= NV_net(strat, net_i);
		}

		APPLY_net_vector(apply_context)= nv_reverse(cascade);

		/* (Note that the cascade and the apply_context content are released
		only when the user exits from the lookup application). */

		/* Setting of the APPLY_CONTEXT_VECTOR structure:
		it contains a vector of APPLY_CONTEXT structures, 1 per net
		in the cascade to be applied. 
		The APPLY_CONTEXT_VECTOR is stored in the APPLY_hyper_unit
		field of apply_context. */
		initialize_hyper_unit(apply_context, &store_in_a_tree);

		appvec = (APPVECptr) APPLY_hyper_unit(apply_context);

		/* Initialize the first context of the APPLY_CONTEXT_VECTOR:
		--------------------------------------------------------- */
		first_context = APPVEC_first_context(appvec);

		/* The function for retrieving the id symbols one by one is the
		same as the one used in XeLDA: it is the  read_id_from_str()
		function that reads an (char*) string. */
		APPLY_next_symbol_fn(first_context) = read_id_from_str;

		/* Get the appropriate parse table: it is the parse table of the
		net stored in the first context structure. */
		APPLY_parse_table(first_context) =
			get_parse_table(APPLY_net1(first_context), APPLY_side(first_context));

	} /* for */

	return(strategies_apply_contexts);
} /* init_compose_apply_for_lookup() */



/* Top-level function for looking up a word with the Depth-First
compose-apply algorithm:

- it supports standard nets, bimachines, labelSet reduced and
Karttunen compacted networks.

- it does NOT support vectorized nets.

(Requires function init_compose_apply_for_lookup() to be called
first and for once by the user). */
void lookup_input_and_write_result_compose_apply()
{
	/* Compose-apply algorithm: */

	APPVECptr appvec;
	APPLYptr first_context= NULL;
	APPLYptr apply_context= NULL; 

	id_type *seq, *outseq;

	int strat_index=0, result_count= -1;

	seq = LOOKUP_ID_SEQUENCE;
	outseq = seq + (length_of_sequence (seq) + 1);

	/* Main loop on the strategies:
	---------------------------- */      
	for (strat_index=0; strat_index< NVV_len (STRATEGIES); strat_index++)
	{
		/* Retrieve the apply_context of the current strategy: */
		apply_context= STRATEGIES_APPLY_CONTEXT[strat_index];

		/* Retrieve the (previous) appvec of this apply_context: */
		appvec = (APPVECptr) APPLY_hyper_unit(apply_context);

		/* Reset the appvec: it cleans the buffers and reset the data for a new
		word analysis. */
		reset_apply_context_vector (appvec);

		/* Get the re-initialized appvec: */
		appvec = (APPVECptr) APPLY_hyper_unit(apply_context);

		/* (Re)initialize the first context of the APPLY_CONTEXT_VECTOR: */
		first_context = APPVEC_first_context(appvec);
		init_apply_context(first_context);
		init_apply_net(first_context);

		/* The input string of the first context is the word to be
		analyzed:
		Note that the compose-apply algorithm takes its input from
		a (char*) string, whereas the lookup application provides
		its input as a (id_type*) string. Consequenlty we cast the
		parsed input contained in LOOKUP_ID_SEQUENCE on a (char*)
		and we use subsequently function read_id_from_str() as the
		next_symbol_fn of the first_context. */
		APPLY_input(first_context) = (char*) LOOKUP_ID_SEQUENCE;

		/* Initialize the position in the "APPLY_input" field of the
		context: */
		APPLY_in_pos(first_context) = 0;

		/* Run the compose-apply algorithm on the whole cascade of FSTs
		i.e. on the current APPLY_CONTEXT_VECTOR: it is the depth first
		strategy implemented in the FSM function apply_netvector(). */
		result_count= apply_netvector(appvec);

		if (result_count > 0)
		{
			/* Retrieve the result of the lexeme analysis:
			It is contained in last context of the appvec. */
			APPLYptr last_context= APPVEC_last_context(appvec);

			/* Possible solution(s) found: generate the outputs. */
			/*             STATEptr root_state= APPLY_solution_tree(last_context); */
			/*             write_result_net_to_output(root_state, */
			/*                                        LOOKUP_OUTPUT_BUFFER, */
			/*                                        TRUE); */
			int i;
			IO_SEQ_TABLEptr output_table= APPLY_output_table(last_context);
			char *bf_end, *new_end;
			int sep_p = TRUE, is_multi_char= FALSE;

			for (i=0; i< IO_SEQ_TABLE_pos(output_table); i++)
			{
				IO_SEQptr out_seq = IO_SEQ_TABLE_array(output_table)[i];
				LAB_VECTORptr sym_vector = IO_SEQ_io_vector(out_seq);
				id_type *seq = LAB_VECTOR_array(sym_vector);

				/* Close the sequence: */
				seq[LAB_VECTOR_pos(sym_vector)]= ID_EOS;

				for (bf_end = LOOKUP_OUTPUT_BUFFER; *seq != ID_EOS; seq++, bf_end = new_end)
				{
					is_multi_char= append_label_to_buffer(*seq, bf_end,
						&new_end, sep_p);

					/* Don't put separators once a first multichar
					has been written out to the buffer:  */
					if (is_multi_char)
						sep_p = FALSE;
				}

				write_buffer_to_output (bf_end);
				/* Reset the separator booleans: */
				sep_p = TRUE, is_multi_char= FALSE;

				/* Mark that a result was found: */
				LOOKUP_RESULT = (NETptr) NULL+1;
			}

			SUCCESS_COUNT [strat_index] ++;
			break;
		}
	} /* End of the loop on the strategies. */

	if (strat_index == NVV_len (STRATEGIES))
	{
		/* No solution found. */
		NUM_WORDS_NOT_FOUND++;

		fprintf(stdout, "%s\t%s\t%s\n", LOOKUP_INPUT_WORD, LOOKUP_INPUT_WORD,
			LOOKUP_UNKNOWN_WORD_TAG);
	}

	NUM_WORDS_TOTAL++;

} /* lookup_input_and_write_result_compose_apply() */


/*************************************************
*
*  TEST FUNCTIONS
*
*/

NETptr TEST_print_net(NETptr net)
{
	NETptr xnet;

	if(net)
	{
		xnet = copy_fsm (net);
		fprintf (stderr, "Sigma: ");
		print_sigma_s (xnet, 60, 7, stderr);
		print_net (xnet, stderr);
		free_network (xnet);
	}
	else fsm_fprintf(stderr,"NULL_POINTER\n");

	fsm_fprintf(stderr,"--------------------------\n");
	return(net);
}

void TEST_get_label_name(LABELptr label, char *lab_name)
{
	FAT_STR fstring = LABEL_atom( label );
	fat_to_thin (fstring, lab_name);
}

int TEST_get_label(id_type id, char *lab_name)
{
	TUPLEptr tuple;  LABELptr label;
	int16 arity;
	uint16 id_count = LIM_id_count(LABEL_MAP);
	id_type upper_id, lower_id;
	char *pt;

	if ((id>=id_count) || (id == ID_EOS))
	{ strcpy(lab_name, "WRONG_ID"); return(FALSE);}

	label = id_to_label(id);
	arity = LABEL_arity(label);

	switch(arity)
	{
	case 1:
		TEST_get_label_name(label, lab_name);
		break;
	case 2:
		tuple = LABEL_tuple(label);
		upper_id = TUPLE_labels(tuple)[UPPER];
		lower_id = TUPLE_labels(tuple)[LOWER];

		TEST_get_label(upper_id, lab_name);

		pt = &( lab_name[ strlen(lab_name) ]); *pt++=':';
		TEST_get_label(lower_id, pt);
		break;
	default:
		strcpy(lab_name, "WRONG_ARITY");
		return(FALSE);
	}

	return(TRUE);
}

void TEST_print_label(id_type id)
{
	char name [500];

	TEST_get_label (id, name);
	fprintf (stderr, "ID:%hi\tNAME: <%s>\n", id, name);
}

void TEST_print_idseq(id_type *seq, int32 len)
{
	if (len <= 0)
		while (*seq  != ID_EOS)
			fprintf (stderr, "  %hi", *seq++);
	else
		for ( ; len >0 ; len--)
			fprintf (stderr, "  %hi", *seq++);

	fprintf (stderr, "\n");
}

void TEST_print_labseq(id_type *seq, int32 len)
{
	char name [500];

	if (len <= 0)
		while (*seq != ID_EOS)
		{
			TEST_get_label (*seq++, name);
			fprintf (stderr, " <%s>", name);
		}
	else
		for ( ; len >0 ; len--)
		{
			TEST_get_label (*seq++, name);
			fprintf (stderr, " <%s>", name);
		}

		fprintf (stderr, "\n");
}

