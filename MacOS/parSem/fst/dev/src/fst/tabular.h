/* $Id: tabular.h,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/******************************************************
 **
 **  TABULAR.H
 **
 **  Andre Kempe, RXRC Grenoble, November 1995
 **
 **  Definitions for networks in tabular format.
 **  
 **
 ******************************************************/

/* general macros */

#ifndef min_of
#define min_of(A,B)  ((A<B)?A:B)
#define max_of(A,B)  ((A>B)?A:B)
#endif

#ifndef TABULAR_H

/* macro to read and write a state number in a network */
#define NTT_num_of_state(X)   *(long *)&((X)->client_cell)

/* pointer inside a network used for inclusion of a transition table */
#define TTB_ttable(X)   ((NTTptr)NET_states(X))  /* initially STATEptr */

/* all flags of a network */
#define TTB_flags(X)    (X)->flags

/* structure used for quicksort */
typedef struct TT_LABEL_SORT {
  long freq;
  id_type lab;
} TTLSORTtype, *TTLSORTptr;

/* global and local alphabets */
typedef struct TRANS_TABLE_ALPHABET {
  int num_items;         /* number of items in the alphabet */
  id_type *items;            /* alphabet */
} TTALPHtype, *TTALPHptr;

#define TTALPH_num(X)     (X)->num_items
#define TTALPH_items(X)   (X)->items
#define TTALPH_item(X,Y)  (X)->items[(Y)]

#define TTALPH_EPSILON    0
#define TTALPH_OTHER      1
#define TTALPH_FINLABEL   2
#define TTALPH_1ST_ID     3

#define TTALPH_FIN_NAME   "**NTT-FINAL-ARC**"

/* the transition table */
typedef struct NET_TRANSITION_TABLE {
  struct {
    fst_bit simple_fsm:1;          /* flag: the net is not a transducer */
    fst_bit single_char_upper:1;   /* flag: all upper labels are single chars */
    fst_bit single_char_lower:1;   /* flag: all lower labels are single chars */
    fst_bit unused:5;              /* 5 bit for any purpose */
  } flags;

  TTALPHptr glob_labels;       /* conversion local to global labels */
  TTALPHptr loc_labels;        /* conversion global to local labels */

  id_type loc_epsilon;           /* local id of EPSILON */
  id_type loc_other;             /* local id of OTHER */
  id_type loc_finlabel;          /* local id of FINAL-LABEL */
  int16 maxlen_upper;          /* maximal length of upper label names */
  int16 maxlen_lower;          /* maximal length of lower label names */

  /* counts of states and arcs */
  long num_net_arcs;           /* original number of arcs in the network */
  long num_arcs;               /* number of arcs */
  long num_net_states;         /* original number of states in the network */
  long num_states;             /* number of states and default states */
  long other_side_len;         /* length of other-side vectors */

  /* vectors of states and arcs */
  long *defstates;             /* default states */
  long *bases;                 /* bases of this-side labels */
  id_type *labels;               /* this-side labels */
  long *destinations;          /* destination states */

  long *upper_base;            /* bases of upper-side labels */
  long *lower_base;            /* bases of lower-side labels */
  id_type *other_labels;         /* other-side labels */
  long *other_destinations;    /* other-side destination states */

} NTTtype, *NTTptr;

#define NTT_simple_fsm(X)    (X)->flags.simple_fsm
#define NTT_single_upper(X)  (X)->flags.single_char_upper
#define NTT_single_lower(X)  (X)->flags.single_char_lower

#define NTT_glob_labs(X)   (X)->glob_labels
#define NTT_loc_labs(X)    (X)->loc_labels

#define NTT_loc_epsilon(X) (X)->loc_epsilon
#define NTT_loc_other(X)   (X)->loc_other
#define NTT_loc_final(X)   (X)->loc_finlabel
#define NTT_max_upper(X)   (X)->maxlen_upper
#define NTT_max_lower(X)   (X)->maxlen_lower

#define NTT_net_states(X)  (X)->num_net_states
#define NTT_num_states(X)  (X)->num_states
#define NTT_net_arcs(X)    (X)->num_net_arcs
#define NTT_num_arcs(X)    (X)->num_arcs
#define NTT_other_len(X)   (X)->other_side_len

#define NTT_defs(X)        (X)->defstates
#define NTT_bases(X)       (X)->bases
#define NTT_labs(X)        (X)->labels
#define NTT_dests(X)       (X)->destinations
#define NTT_uppers(X)      (X)->upper_base
#define NTT_lowers(X)      (X)->lower_base
#define NTT_other_labs(X)  (X)->other_labels
#define NTT_other_dests(X) (X)->other_destinations

#define NTT_def(X,Y)       (X)->defstates[(Y)]
#define NTT_base(X,Y)      (X)->bases[(Y)]
#define NTT_lab(X,Y)       (X)->labels[(Y)]
#define NTT_dest(X,Y)      (X)->destinations[(Y)]
#define NTT_upper(X,Y)     (X)->upper_base[(Y)]
#define NTT_lower(X,Y)     (X)->lower_base[(Y)]
#define NTT_other_lab(X,Y) (X)->other_labels[(Y)]
#define NTT_other_dest(X,Y)  (X)->other_destinations[(Y)]


typedef struct TT_AUXILIARY_TABLE {
  long trans_vec_length;
  long *trans_vector;     /* transition vector of current state */
  long *trans_upper_vector; /*transition vector of current state, upper IDs*/
  long *trans_lower_vector; /*transition vector of current state, lower IDs*/
  long *next_position;    /* next position for search of 1st arc */

  long arcs_in_vec;       /* number of arcs in the transition vector */
  long cur_state;         /* number of the current state */
  char final;             /* flag: current state is final */

  long base;              /* last used item in base vector */
  long arc_1st;           /* content of last used item in base vector */
  char conflict;          /* flag: conflict with existing states or arcs */
  char base_used;         /* flag: base already used */
  char useless;           /* flag: useless position */
  char state_identical;   /* flag: identical state found */
  long arcs_remain;       /* number of remaining arcs */

  long net_states;        /* number of states in the network */
  long net_arcs;          /* number of arcs in the network */
  long print_states;      /* number of states for one printed '>' */

  long num_states;        /* highest state index + 1 */
  long size_states;       /* size of allocated state vectors */
  long num_arcs;          /* highest arc index + 1 */
  long size_arcs;         /* size of allocated arc vectors */
  long num_other;         /* highest other-side index + 1 */
  long size_other;        /* size of allocated other-side vectors */

} TTAUXtype, *TTAUXptr;

#define TTAUX_tvec_len(X)     (X)->trans_vec_length
#define TTAUX_tvec(X)         (X)->trans_vector
#define TTAUX_tvec_upper(X)   (X)->trans_upper_vector
#define TTAUX_tvec_lower(X)   (X)->trans_lower_vector
#define TTAUX_next_pos_vec(X) (X)->next_position
#define TTAUX_next_pos(X,Y)   (X)->next_position[(Y)]
#define TTAUX_arcs_in_vec(X)  (X)->arcs_in_vec
#define TTAUX_cur_state(X)    (X)->cur_state
#define TTAUX_final(X)        (X)->final

#define TTAUX_base(X)         (X)->base
#define TTAUX_1st_arc(X)      (X)->arc_1st
#define TTAUX_confl(X)        (X)->conflict
#define TTAUX_used(X)         (X)->base_used
#define TTAUX_useless(X)      (X)->useless
#define TTAUX_identical(X)    (X)->state_identical
#define TTAUX_remain(X)       (X)->arcs_remain

#define TTAUX_net_states(X)   (X)->net_states
#define TTAUX_net_arcs(X)     (X)->net_arcs
#define TTAUX_print(X)        (X)->print_states

#define TTAUX_num_states(X)   (X)->num_states
#define TTAUX_size_states(X)  (X)->size_states
#define TTAUX_num_arcs(X)     (X)->num_arcs
#define TTAUX_size_arcs(X)    (X)->size_arcs
#define TTAUX_num_other(X)    (X)->num_other
#define TTAUX_size_other(X)   (X)->size_other

#define NTT_FACTOR_STATES     2.0
#define NTT_FACTOR_ARCS       2.0
#define NTT_FACTOR_REALLOC    1.5
#define NTT_FACTOR_REMAIN     0.35  /* ratio: existing to remaining arcs */
#define NTT_PRINT_FACTOR      50    /* total number of NTT_PRINT_SYMBOLs */
#define NTT_PRINT_SYMBOL      '>'
#define NTT_PRINT_HEAD        \
        "----|----|----|----|----|----|----|----|----|----|"

#define NTT_NO_STATE            -1
#define NTT_NO_ID     (id_type) -1
/* #define NTT_NO_ARCS          -2 */


typedef struct TT_LABEL_INFO {
  struct {
    fst_bit glob_id:1;              /* flag: the ID is a global one */
    fst_bit simple_id:1;            /* flag: the ID is of arrity one */
    fst_bit single_char_upper:1;    /* flag: upper label is a single char */
    fst_bit single_char_lower:1;    /* flag: lower label is a single char */
  } flags;

  id_type id;                     /* the ID */
  id_type upper_id;               /* ID of upper label */
  id_type lower_id;               /* ID of lower label */
  char *upper_string;           /* name of upper label */
  char *lower_string;           /* name of lower label */
  int16 upper_length;           /* length of upper label name */
  int16 lower_length;           /* length of lower label name */
} TTLABtype, *TTLABptr;

#define TTLAB_glob(X)          (X)->flags.glob_id
#define TTLAB_simple(X)        (X)->flags.simple_id
#define TTLAB_single_upper(X)  (X)->flags.single_char_upper
#define TTLAB_single_lower(X)  (X)->flags.single_char_lower

#define TTLAB_id(X)            (X)->id
#define TTLAB_upper_id(X)      (X)->upper_id
#define TTLAB_lower_id(X)      (X)->lower_id
#define TTLAB_upper_str(X)     (X)->upper_string
#define TTLAB_lower_str(X)     (X)->lower_string
#define TTLAB_upper_len(X)     (X)->upper_length
#define TTLAB_lower_len(X)     (X)->lower_length

#define TTLAB_LABEL_MAXLEN             50

enum TTLAB_name_p { TTLAB_ZERO, TTLAB_EPSILON };
enum TTLOOK_side_p { TTL_SD_NONE, TTL_SD_UPPER, TTL_SD_LOWER, TTL_SD_BOTH };

#define TTLOOK_NO_ID    (id_type) -1   /* end of input for lookup */
#define TTLOOK_MAX_EPSREC  10 /* maximum of recursive insertion of EPSILON */

#define TTLOOK_global_to_local_id(I,N)  \
   ((I< TTALPH_num (NTT_loc_labs (N)))?   \
    TTALPH_item (NTT_loc_labs (N),I) : NTT_loc_other (N))

#define TTLOOK_local_to_global_id(I,N)   TTALPH_item(NTT_glob_labs(N),I)


/* FUNCTION DEFINITIONS */

id_type *TTB_alloc_idtype_vector (long len);
id_type *TTB_realloc_idtype_vector (id_type *oldvec, long len);
long *TTB_alloc_long_vector (long len);
long *TTB_realloc_long_vector (long *oldvec, long len);
void TTB_init_long_vector (long *items, long num_items, long init_value);
void TTB_init_idtype_vector (id_type *items, long num_items, id_type init_value);
void TTB_init_pointer_vector (void **items, long num_items, void *init_value);

TTALPHptr TTALPH_alloc_alphabet (int len);
TTALPHptr TTALPH_realloc_alphabet (TTALPHptr alph, int len);
void TTALPH_free_alphabet (TTALPHptr alph);

TTLABptr TTLAB_alloc_label_structure ();
void TTLAB_free_label_structure (TTLABptr ttlab);
int TTLAB_get_label_length (LABELptr label);
void TTLAB_get_label_name (LABELptr label, char *name);
void TTLAB_correct_one_printname
  (id_type id, char *name, int16 *length, int TTLAB_name_p);
void TTLAB_correct_printnames (TTLABptr ttlab, int TTLAB_name_p);
void TTLAB_get_label_information (id_type id, TTLABptr ttlab);
void TTLAB_get_id_of_label (TTLABptr ttlab);

TTAUXptr NTT_alloc_ttaux (NETptr net, NTTptr ntt);
void NTT_free_ttaux (TTAUXptr aux);
int NTT_id_in_alphabet (id_type id, TTALPHptr alph, int len);
id_type NTT_highest_id (id_type *id_vector, int len);
/* static int NTT_qsort_comp (int *p1, int *p2); */
void NTT_sort_labels (TTALPHptr alph, NETptr net);
TTALPHptr NTT_make_glob_labels (NETptr net);
TTALPHptr NTT_make_local_labels (NTTptr ntt);
void NTT_set_table_flags (NETptr net, NTTptr ntt);
void NTT_enumerate_states (NETptr net);
void NTT_fsm_state_to_auxtable (STATEptr state, NTTptr ntt, TTAUXptr aux);
long NTT_other_side_to_vector (ARCptr arc, TTAUXptr ttaux, NTTptr ntt);
void NTT_free_aux_arc_list (ARCptr arc);
void NTT_fst_state_to_auxtable (STATEptr state, NTTptr ntt, TTAUXptr aux);
void NTT_analyze_1st_arc (long arc_1st, TTAUXptr aux, NTTptr ntt);
long NTT_calc_max_remaining_arcs (TTAUXptr aux);
void NTT_handle_next_pos_vector (TTAUXptr aux);
long NTT_find_new_1st_arc (TTAUXptr aux, NTTptr ntt);
void NTT_check_realloc_ntt (TTAUXptr aux, NTTptr ntt);
void NTT_base_and_arcs_to_ntt (TTAUXptr aux, NTTptr ntt);
long NTT_find_new_base (TTAUXptr aux, NTTptr ntt);
void NTT_auxtable_to_ntt (TTAUXptr aux, NTTptr ntt);
void NTT_final_work_on_ntt (TTAUXptr aux, NTTptr ntt);
void NTT_conversion_print_begin (TTAUXptr aux);
void NTT_conversion_print (TTAUXptr aux);
void NTT_convert_net_to_table (NETptr net, NTTptr ntt);
NTTptr NTT_alloc_transition_table (NETptr net);
NTTptr NTT_make_transition_table_from_net (NETptr old_net, int reclaim_p);
void NTT_free_transition_table (NTTptr ntt);

int NTT_is_arclabel_in_arclist (id_type id, ARCptr list);
void NTT_arc_vector_to_arc_list
  (id_type lab_this_side, int this_side_p, long idx_other_side, NTTptr ntt,
   STATEptr *state_vec, STATEptr state, NETptr net);
void NTT_add_arcs_to_net_state
  (NTTptr ntt, STATEptr *states, long state_idx, NETptr net);
void NTT_convert_table_to_net (NTTptr ntt, NETptr net);
void NTT_generate_net_alphabets (NTTptr ntt, NETptr net);
NETptr NTT_make_net_from_transition_table (NTTptr ntt, int reclaim_p);

NTTptr FstCalcExport NTT_evaluate_table (NTTptr ntt);

int NTT_write_vector (void *vec, long bytes, FILE *file);
int NTT_write_string (char *str, FILE *file);
int NTT_write_label (TTLABptr ttlab, FILE *file);
int NTT_write_alphabet (TTALPHptr alph, FILE *file);
int NTT_write_table (NTTptr ntt, FILE *file);
void *NTT_read_vector (long bytes, FILE *file);
int NTT_read_string (char *str, int maxlen, FILE *file);
int NTT_read_label (TTLABptr ttlab, FILE *file);
TTALPHptr NTT_read_alphabet (FILE *file);
NTTptr NTT_read_table (FILE *file);

id_type TTLOOK_get_first_id_from_string
  (char *str, char *buffer, int maxlen, int *lablen);
id_type *TTLOOK_convert_string_to_vector (char *str, int side_in, NTTptr ntt);
long TTLOOK_get_next_fsm_state (id_type id, long state, NTTptr ntt);
STATEptr TTLOOK_generate_tail_fsm
  (id_type *inp, int side_out, long nttstate, NTTptr ntt);
long TTLOOK_get_next_fst_index (id_type id, int side_in, long state, NTTptr ntt);
STATEptr TTLOOK_generate_tail_fst
  (id_type *inp, int side_in, int side_out, int eps_recurs,
   long nttstate, NTTptr ntt);
void TTLOOK_make_outnet_statelist (STATEptr state, NETptr net);
void TTLOOK_make_outnet_alphabets (NETptr net, NTTptr ntt);
NETptr TTLOOK_lookup_vector_to_net
  (id_type *vec_in, int side_in, int side_out, NTTptr ntt);

int TTLOOK_continue_longest_match_fsm
  (char *str, char *buffer, int maxlabel, int cur_prefix,
   long nttstate, NTTptr ntt);
int TTLOOK_continue_longest_match_fst
  (char *str, char *buffer, int maxlabel, int cur_prefix, int side,
   int eps_recurs, long nttstate, NTTptr ntt);
int TTLOOK_get_longest_match (char *str, int side, NTTptr ntt);

void TTB_incorporate_ttable_in_ttnet (NTTptr ntt, NETptr ttnet);
NTTptr TTB_excorporate_ttable_from_ttnet (NETptr ttnet);
NETptr TTB_net_to_ttnet (NETptr net, int reclaim_p);
NETptr FstCalcExport TTB_ttnet_to_net (NETptr ttnet, int reclaim_p);
NETptr TTB_lookup_ttnet_make_net
  (char *word, int input_side, int output_side, NETptr ttnet);

/* CONNECTION OF TABULAR FUNCTIONS WITH OTHER FUNCTIONS */

void TABULAR_table_from_stream (FILE *stream, NETptr net);
NTTptr TABULAR_table_from_net (NETptr net);
void TABULAR_table_to_file (FILE *stream, NETptr net, void *vntt);
void TABULAR_free_included_table (NETptr net);

void TABULAR_make_all_function_assignments ();

/* TEST FUNCTIONS FOR DEBUGGING */

void TEST_print_ptr_vec (void **vec, long len);
void TEST_print_long_vec (long *vec, long len);
void TEST_print_idtype_vec (id_type *vec, long len);

#define TABULAR_H
#endif  /* TABULAR_H */

