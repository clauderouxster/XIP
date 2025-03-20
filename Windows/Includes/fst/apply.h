/* $Id: apply.h,v 1.4 2005/03/04 15:45:21 grondeau Exp $ */
/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */


/******************************************************
 **
 **  APPLY.H
 **
 **  Lauri Karttunen, XRCE, February 1999
 **
 **  Function prototypes and data structures for new lookup routines
 **
 ******************************************************/

#ifndef C_FSM_APPLY

#define C_FSM_APPLY

#include "fst/id_type.h"
#include "fst/types.h"


#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
 *
 * Structures and variables for the vectorized version
 * of compose-apply.
 * CP, XRCE, November 2002.
 */    

/* Fix maximum input line length (same value as in "lookup"):
   It is used for building the IO_SEQ_io_vector of in_seq, (also
   known as APPLY_in_vect).                                   */
#define INPUT_LINE_MAX_LEN 2000
    
id_type FstBaseExport *INPUT_MATCH;  /* match table of the input side */
id_type FstBaseExport *OUTPUT_MATCH; /* match table of the output side */
    
/* Temporary location objects used to navigate in the network by the
   vectorized compose-apply.
   Each object recordes a position in the network; it works for both
   vectorized and non-vectorized states:
     -> if the location corresponds to a vectorized state:
        -  field "arc" is unused; remains always NULL.
     -> if the location corresponds to a standard state:
        -  field "epsilon_pos" is unused; remains always -1
        -  field "real_match_pos" is unused; remains always -1
   Allocation is made through static function make_location();
   Init is made through static function set_location().
*/
typedef struct LOCATION_IN_NET {
  id_type matched_id;/* Last id matched to reach that location. */
  int epsilon_pos;   /* Last index visited in STATE_arc_vector epsilon array. */
  int real_match_pos;/* Last index visited in STATE_arc_vector dest_array. */
  STATEptr state;    /* Current state to continue the apply from. */
  ARCptr arc;        /* Next arc to explore. */
} LOCATIONtype, *LOCATIONptr;

#define LOCATION_id(X)                     (X)->matched_id
#define LOCATION_eps_pos(X)                (X)->epsilon_pos
#define LOCATION_match_pos(X)              (X)->real_match_pos
#define LOCATION_state(X)                  (X)->state
#define LOCATION_arc(X)                    (X)->arc

/* Set up of the global variables INPUT_MATCH and OUTPUT_MATCH at the
   initialization of the compose-apply, depending on the
   value of the input_side.  (Imported from lookup/networks.c) */
void FstBaseExport select_match_tables_for_apply (int input_side);

    
typedef struct IO_SEQUENCE {
  int io_pos;
  LAB_VECTORptr io_vector;                    
  LAB_VECTORptr other_than_vector;            
  VECTORptr other_than_index;
  VECTORptr prev_sigma_vector;
} IO_SEQtype, *IO_SEQptr;

#define IO_SEQ_io_pos(X)                (X)->io_pos
#define IO_SEQ_io_vector(X)             (X)->io_vector
#define IO_SEQ_other_than_vector(X)     (X)->other_than_vector
#define IO_SEQ_pos(X)			LAB_VECTOR_pos(IO_SEQ_io_vector(X))
#define IO_SEQ_length(X)		LAB_VECTOR_length(IO_SEQ_io_vector(X))
#define IO_SEQ_other_than_index(X)      (X)->other_than_index
#define IO_SEQ_prev_sigma_vector(X)     (X)->prev_sigma_vector

typedef struct IO_SEQUENCE_TABLE {
  int length;
  int sequence_length;
  int pos;
  IO_SEQptr *array;
} IO_SEQ_TABLEtype, *IO_SEQ_TABLEptr;

#define IO_SEQ_TABLE_length(X)          (X)->length
#define IO_SEQ_TABLE_seq_length(X)      (X)->sequence_length
#define IO_SEQ_TABLE_pos(X)             (X)->pos
#define IO_SEQ_TABLE_array(X)           (X)->array


/* APPLY_CONTEXT */
typedef struct APPLY_CONTEXT {
    NETptr net1;
    NETptr net2;
    NVptr net_vector;
    int side;
    int out_side;
    int obey_flags_p;
    int print_space_p;
    int show_flags_p;
    int flags_p;
    int recursive_p;
    int need_separators_p;     /* separators required in apply_patterns(). */
    int count_patterns_p;      /* count pattern matches                    */
    int delete_patterns_p;     /* delete material that matches             */
    int extract_patterns_p;    /* delete material that does not match      */
    int mark_patterns_p;       /* mark patterns with tags                  */
    int in_pos;
    int nv_pos;
    PARSE_TBL parse_table;
    int (*next_symbol_fn)(id_type *, void *);
    id_type (*in_fn)(id_type);
    id_type (*out_fn)(id_type);
    MATCH_TABLEptr match_table;
    char *input;
    char *remainder;
    FILE *in_stream;
    FILE *out_stream;
    int out_count;  /*AK*/
    void (*output_fn)(void *cntxt);
    LAB_VECTORptr in_vector;
    LAB_VECTORptr mid_vector;
    LAB_VECTORptr out_vector;
    LAB_VECTOR_TABLEptr in_table;
    LAB_VECTOR_TABLEptr out_table;
    ALPHABETptr sigma;
    ALPHABETptr prev_sigma;
    ALPHABETptr flag_register;
    LAB_VECTORptr flag_vector;
    VECTORptr arc_vector;
    VECTORptr state_vector;
    STRING_BUFFERptr out_buffer;
    void *hyper_unit;  /*AK*/

    LAB_VECTORptr other_than_vector;

    IO_SEQptr in_seq;
    IO_SEQptr out_seq;
    IO_SEQ_TABLEptr input_table;
    IO_SEQ_TABLEptr output_table;
    
    /* Net traversal call-back functions: */
    void* (*start_state_fn)(NETptr, void**, int*);
    id_type (*label_from_arc_fn)(NETptr, void**, int*, int*);
    void (*next_arc_fn)(NETptr, void**, int);
    void* (*destination_fn)(NETptr, void**);

    STATEptr solution_tree;  /* For storing the final result in a tree
                               instead of the table. */
    LAB_RINGptr input_ring;
    LOCATIONptr location_heap; /* Working space for apply_vectorized_network */ 
  } APPLYtype, *APPLYptr;

#define APPLY_net1(X)			(X)->net1
#define APPLY_net2(X)			(X)->net2
#define APPLY_net_vector(X)		(X)->net_vector
#define APPLY_side(X)	        	(X)->side
#define APPLY_out_side(X)	    	(X)->out_side
#define APPLY_obey_flags_p(X)		(X)->obey_flags_p
#define APPLY_print_space_p(X)		(X)->print_space_p
#define APPLY_show_flags_p(X)		(X)->show_flags_p
#define APPLY_flags_p(X)		(X)->flags_p
#define APPLY_recursive_p(X)		(X)->recursive_p
#define APPLY_in_pos(X)	    		(X)->in_pos
#define APPLY_nv_pos(X)	    		(X)->nv_pos
#define APPLY_parse_table(X)		(X)->parse_table
#define APPLY_next_symbol_fn(X)		(X)->next_symbol_fn
#define APPLY_in_fn(X)			(X)->in_fn
#define APPLY_out_fn(X)			(X)->out_fn
#define APPLY_in_stream(X)              (X)->in_stream  /*AK*/
#define APPLY_out_stream(X)             (X)->out_stream  /*AK*/
#define APPLY_out_count(X)              (X)->out_count  /*AK*/
#define APPLY_output_fn(X)		(X)->output_fn
#define APPLY_match_table(X)		(X)->match_table
#define APPLY_input(X)			(X)->input
#define APPLY_remainder(X)		(X)->remainder
#define APPLY_in_vector(X)		(X)->in_vector
#define APPLY_sigma(X)                  (X)->sigma
#define APPLY_prev_sigma(X)             (X)->prev_sigma
#define APPLY_mid_vector(X)		(X)->mid_vector
#define APPLY_out_vector(X)		(X)->out_vector
#define APPLY_in_table(X)		(X)->in_table
#define APPLY_out_table(X)		(X)->out_table
#define APPLY_flag_register(X)		(X)->flag_register
#define APPLY_flag_vector(X)		(X)->flag_vector
#define APPLY_arc_vector(X)		(X)->arc_vector
#define APPLY_state_vector(X)		(X)->state_vector
#define APPLY_out_buffer(X)		(X)->out_buffer
#define APPLY_hyper_unit(X)		(X)->hyper_unit  /*AK*/
#define APPLY_other_than_vector(X)      (X)->other_than_vector
#define APPLY_in_seq(X)                 (X)->in_seq
#define APPLY_out_seq(X)                (X)->out_seq
#define APPLY_input_table(X)            (X)->input_table
#define APPLY_output_table(X)           (X)->output_table
#define APPLY_in_vect(X)                IO_SEQ_io_vector(APPLY_in_seq(X))
#define APPLY_ot_vect(X)         IO_SEQ_other_than_vector(APPLY_in_seq(X))
#define APPLY_ot_index(X)        IO_SEQ_other_than_index(APPLY_in_seq(X))
#define APPLY_start_state_fn(X)         (X)->start_state_fn     /* CP */
#define APPLY_label_from_arc_fn(X)      (X)->label_from_arc_fn  /* CP */
#define APPLY_next_arc_fn(X)            (X)->next_arc_fn        /* CP */
#define APPLY_destination_fn(X)         (X)->destination_fn     /* CP */
#define APPLY_solution_tree(X)          (X)->solution_tree      /* CP */
#define APPLY_need_separators_p(X)      (X)->need_separators_p  /* LK */
#define APPLY_input_ring(X)             (X)->input_ring         /* LK */
#define APPLY_location_heap(X)          (X)->location_heap      /* CP */
#define APPLY_count_patterns_p(X)       (X)->count_patterns_p   /* LK */
#define APPLY_delete_patterns_p(X)      (X)->delete_patterns_p  /* LK */
#define APPLY_extract_patterns_p(X)     (X)->extract_patterns_p /* LK */
#define APPLY_mark_patterns_p(X)        (X)->mark_patterns_p    /* LK */

IO_SEQptr FstBaseExport make_io_sequence(int length);
void FstBaseExport free_io_sequence(IO_SEQptr io_sequence);
void FstBaseExport initialize_io_sequence(IO_SEQptr *io_sequence);
void FstBaseExport reset_io_sequence(IO_SEQptr io_sequence);
IO_SEQ_TABLEptr FstBaseExport make_io_sequence_table(int table_length, int seq_length);
void FstBaseExport free_io_sequence_table(IO_SEQ_TABLEptr io_table);
void FstBaseExport reset_io_sequence_table(IO_SEQ_TABLEptr table);
int FstBaseExport io_sequence_at(IO_SEQptr *io_seq, int pos, IO_SEQ_TABLEptr table);
void FstBaseExport assure_io_sequence_table_space(int len, IO_SEQ_TABLEptr table);
void FstBaseExport print_io_seq_table(IO_SEQ_TABLEptr table, FILE *stream, int escape_p);
/* Printing of the final output strings when they are stored in a tree: */
int FstBaseExport print_solution_tree(STATEptr solution_tree, LAB_VECTORptr buffer, FILE *stream, int escape_p); 


APPLYptr FstBaseExport init_apply_context(APPLYptr cntxt);
void FstBaseExport free_apply_context(APPLYptr cntxt);
    
/* Freeing the solution tree structure except its root state: */
void FstBaseExport reset_solution_tree(APPLYptr apply_context);

int FstBaseExport choose_nets_to_apply(APPLYptr apply_context, int inverse_p);
void FstBaseExport init_apply_to_string(char *input, APPLYptr apply_context);
void FstBaseExport init_apply_to_stream(FILE *stream, APPLYptr apply_context);
void FstBaseExport init_apply_net(APPLYptr cntxt);

/* Creating and initializing an APPLY_CONTEXT_VECTOR structure: */
void FstBaseExport initialize_hyper_unit(APPLYptr apply_context, void (*final_output_fn)(void*));

int FstBaseExport apply_standard_network(APPLYptr apply_context);
int FstBaseExport apply_reduced_network(APPLYptr apply_context);
int FstBaseExport apply_bimachine(APPLYptr apply_context);
int FstBaseExport apply_network(APPLYptr apply_context);
int FstBaseExport apply_patterns(APPLYptr apply_context);
void FstBaseExport reset_pattern_match_counts(APPLYptr apply_context);
void FstBaseExport print_pattern_match_counts(APPLYptr apply_context, FILE *stream);

int FstBaseExport compose_apply_networks(char *input, APPLYptr apply_context);
void FstBaseExport write_to_buffer(void *apply_context);
void FstBaseExport merge_to_buffer(void *apply_context);
void FstBaseExport store_in_table(void *apply_context);
void FstBaseExport write_to_out_stream(void *cntxt);

/* Storing the final output of compose-apply in a tree structure: */
void FstBaseExport store_in_a_tree(void *cntxt);
void FstBaseExport store_in_out_seq(void *apply_context);
void FstBaseExport fetch_output(void *cntxt);
int FstBaseExport symbol_from_in_vector(id_type *in_sym, void *apply_context);
    

/****************************************************************
 *
 * RECURSIVE DEPTH-FIRST APPLY FUNCTION FOR NET VECTORS
 *   AK, XRCE, May 1999
 */

typedef struct APPLY_CONTEXT_VECTOR {
  int len;            /* number of apply-contexts */
  int cur;            /* currently used apply-context */
  APPLYptr *context;  /* vector of apply-contexts; 1 per net or bimachine */
  int out_count;      /* number of results */
} APPVECtype, *APPVECptr;

#define APPVEC_len(V)            ((V)->len)
#define APPVEC_cur(V)            ((V)->cur)
#define APPVEC_context(V,I)      ((V)->context[(I)])
#define APPVEC_first_context(V)  ((V)->context[0])
#define APPVEC_last_context(V)   ((V)->context[(V)->len-1])
#define APPVEC_out_count(V)      ((V)->out_count)

APPVECptr FstBaseExport create_apply_context_vector(NVptr nv, void (*final_output_fn)(void*), int side_p, APPLYptr toplevel_apply_context);
void FstBaseExport free_apply_context_vector (APPVECptr appvec);
void FstBaseExport reset_apply_context_vector (APPVECptr appvec);

int FstBaseExport apply_netvector (APPVECptr appvec);

/* Set up of the global variables INPUT_MATCH and OUTPUT_MATCH at the
   initialization of the recursive compose-apply, depending on the
   value of the input_side. */
int FstBaseExport recursive_compose_apply_networks(char *string, APPLYptr apply_context);
    
char FstBaseExport *apply_netvector_to_string (APPVECptr appvec, char *str);

/****************************************************************
 *
 * SIMPLE APPLY FUNCTIONS FOR BIMACHINES
 *   AK, XRCE, April 1999
 */

/* (I) = input symbol, (O) = output symbol, (P) = pair label, (A) = net sigma */
#define match_input_symbol(I,A)    ((I)<(A)->len)&&((A)->items[(I)])?(I):(OTHER)
#define match_output_symbol(I,O,P)    ((P)==OTHER)?(I):(O)

int apply_bimach_to_sequence
  (NETptr left, NETptr right, id_type *in_seq, id_type *out_seq,
   id_type *in_match, id_type *out_match);
int apply_bimach_to_string
  (NETptr left, NETptr right, char *in_str, char *out_str, int side_p);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_APPLY */

