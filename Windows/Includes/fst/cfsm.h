/* $Id: cfsm.h,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Interface to C-FSM library.
   Copyright (C) 1995 Xerox Corporation.
   Copyright (C) 1998 Inxight Software Inc.
   All Rights Reserved. */


#ifndef CFSM_INCLUDED
#define CFSM_INCLUDED

#include <stdlib.h>

#include "fst/fst_windefs.h"


#define CFSM_NO_ERROR 0
#define CFSM_WRITE_ERROR -1
#define CFSM_BUFFER_OVERFLOW -2



/* #define CFSM_UNIQUE_LONGWORD_TYPE(T)\
	typedef struct T ## _type { int unique; }* T;


CFSM_UNIQUE_LONGWORD_TYPE(cfsm_fsm_sequence)
CFSM_UNIQUE_LONGWORD_TYPE(cfsm_fsm)
CFSM_UNIQUE_LONGWORD_TYPE(cfsm_state)
CFSM_UNIQUE_LONGWORD_TYPE(cfsm_arc)
CFSM_UNIQUE_LONGWORD_TYPE(cfsm_label)
CFSM_UNIQUE_LONGWORD_TYPE(cfsm_symbol)
CFSM_UNIQUE_LONGWORD_TYPE(cfsm_object)
*/

typedef struct NETWORK *cfsm_fsm;
typedef struct NET_VECTOR *cfsm_fsm_sequence;
typedef struct ARC *cfsm_arc;
typedef struct STATE *cfsm_state;
typedef int	cfsm_label;
typedef int	cfsm_symbol;
typedef struct OBJECT *cfsm_object;


typedef unsigned short cfsm_fat_char;



#ifdef __cplusplus
extern "C" {
#endif

extern void* cfsm_error_object;

enum {CFSM_UPPERSIDE_SYMBOL = 0 /* UPPER */, CFSM_LOWERSIDE_SYMBOL = 1 /* LOWER */};

/* Initialize (free) the CFSM library, before (after) any functions are called. */
void FstCalcExport cfsm_init(void);
void FstCalcExport cfsm_free(void);

int FstCalcExport cfsm_fsm_sequence_length (cfsm_fsm_sequence s);
cfsm_fsm FstCalcExport cfsm_fsm_sequence_ref (cfsm_fsm_sequence s, int i);
cfsm_fsm_sequence FstCalcExport cfsm_read_fsm_sequence (const char* filename);
cfsm_fsm FstCalcExport cfsm_read_fsm (const char* filename);

/* When building FSMs using low-level operations, it may be necessary to
   explicitly update the sigma before using the FSM with high-level operators
   such as cfsm_compose. */
void FstCalcExport cfsm_update_sigma(cfsm_fsm);

/* Write out the given FSM to a file whose name is given as a string. */
int FstCalcExport cfsm_write_fsm (cfsm_fsm fsm, const char* filename);

/* Return the number of states in the given FSM. */
int FstCalcExport cfsm_state_count(cfsm_fsm);

/* Return the number of arcs in the given FSM. */
int FstCalcExport cfsm_arc_count(cfsm_fsm);

/* Return true if the FSM contains composite (FST) labels. */
int FstCalcExport cfsm_is_fst(cfsm_fsm);

/* Return the start state of the FSM. */
cfsm_state FstCalcExport cfsm_start_state (cfsm_fsm fsm);

/* Set the start state of the FSM to the given state. */
void FstCalcExport cfsm_set_start_state (cfsm_fsm fsm, cfsm_state s);

/* The FSM keeps a list of all states that is not related to the network
   structure itself.  The state list is useful when it is desired to
   run through all the states.  The links that make up the state list
   are stored in the states themselves. */
cfsm_state FstCalcExport cfsm_state_list (cfsm_fsm fsm);
cfsm_state FstCalcExport cfsm_next_state (cfsm_state state, cfsm_fsm fsm);

/* Return the visit mark bit stored in the state.  Use of this function
   is discouraged; perform a state numbering and make a bit vector indexed
   by state number instead. */
int FstCalcExport cfsm_state_visit_mark (cfsm_state state, cfsm_fsm fsm);

/* Set the visit mark bit stored in the state.  Use of this function
   is discouraged; perform a state numbering and make a bit vector indexed
   by state number instead. */
void FstCalcExport cfsm_set_state_visit_mark (cfsm_state state, int value, cfsm_fsm fsm);


cfsm_state FstCalcExport cfsm_the_invalid_state(void);
int FstCalcExport cfsm_valid_state(cfsm_state);

/* Add a new state to the FSM.  If final is true, make the state final
   else nonfinal.  This operation will invalidate any state numbering that
   has been applied to the FSM. */
cfsm_state FstCalcExport cfsm_add_state (int final, cfsm_fsm fsm);

/* Fetch the client cell or "name field" of the given state.  Use of this function
   is discouraged; perform a state numbering and store state-related information
   in a vector indexed by state number instead. */
void FstCalcExport * cfsm_state_name (cfsm_state s, cfsm_fsm fsm);

/* Set the client cell or "name field" of the given state.  Use of this function
   is discouraged; perform a state numbering and store state-related information
   in a vector indexed by state number instead. */
void FstCalcExport cfsm_set_state_name (cfsm_state s, const void* val, cfsm_fsm fsm);

/* Return true if state is final. */
int FstCalcExport cfsm_state_final (cfsm_state s, cfsm_fsm fsm);

/* If b is true make state final, else make it nonfinal. */
void FstCalcExport cfsm_set_state_final (cfsm_state s, int b, cfsm_fsm fsm);

/* Assign the states in the FSM consecutive numbers, starting from 0.
   The numbers are stored in the client cell ("name field") of the state,
   so while the numbers are used, the client cell cannot be otherwise
   used.  Some higher-level operators, such as cfsm_compose, may overwrite
   the client cell.  Use cfsm_assert_state_numbering() to ensure that
   the states are numbered correctly.  */
void FstCalcExport cfsm_number_the_states(cfsm_fsm);

/* Check that the FSM states are numbered correctly as performed by
   cfsm_number_the_states(). */
int FstCalcExport cfsm_assert_state_numbering(cfsm_fsm);

/* Retrieve the state number for the given state. */
int FstCalcExport cfsm_state_number(cfsm_state);

/* Return true if arc is a valid arc object that can be used with arc
   operators. */
int FstCalcExport cfsm_valid_arc(cfsm_arc arc);

/* Return the invalid arc. */
cfsm_arc FstCalcExport cfsm_the_invalid_arc(void);

/* Return the first arc in the arc list of the state, or return the
   invalid arc.  Test using cfsm_valid_arc(). */
cfsm_arc FstCalcExport cfsm_first_arc (cfsm_state s, cfsm_fsm fsm);

/* Return the next arc in the arc list, or the invalid arc.  Test
   using cfsm_valid_arc(). */
cfsm_arc FstCalcExport cfsm_next_arc (cfsm_arc a, cfsm_fsm fsm);

/* Return the destination state for the given arc. */
cfsm_state FstCalcExport cfsm_arc_destination (cfsm_arc arc, cfsm_fsm fsm);
void FstCalcExport cfsm_add_arc (cfsm_label label,
		   cfsm_state from,
		   cfsm_state to,
		   cfsm_fsm fsm);
/* Remove the given arc from the given state. */
void FstCalcExport cfsm_remove_arc(cfsm_arc, cfsm_state, cfsm_fsm);

/* Make the given arc the first arc at the given state.  The state must
   already contain the arc. */
void FstCalcExport cfsm_top_arc(cfsm_arc, cfsm_state, cfsm_fsm);


/* Return the label on the given arc. */
cfsm_label FstCalcExport cfsm_arc_label (cfsm_arc arc, cfsm_fsm fsm);




void FstCalcExport cfsm_set_prop (const char* attribute, const char* value, cfsm_fsm);
void FstCalcExport cfsm_unset_prop (const char* attribute, cfsm_fsm);
int FstCalcExport cfsm_get_prop (const char* attribute, char* buffer, size_t buffer_size, cfsm_fsm fsm);
int FstCalcExport cfsm_escapify (const char*, char*, size_t, int);
void FstCalcExport cfsm_unescapify (char*);

cfsm_label FstCalcExport cfsm_the_invalid_label(void);
int FstCalcExport cfsm_valid_label(cfsm_label);
cfsm_label FstCalcExport cfsm_make_label (cfsm_symbol sym);
cfsm_label FstCalcExport cfsm_make_fst_label (cfsm_symbol upper, cfsm_symbol lower);
int FstCalcExport cfsm_label_arity (cfsm_label l, cfsm_fsm fsm);
cfsm_symbol FstCalcExport cfsm_label_symbol (cfsm_label l, cfsm_fsm fsm);
cfsm_symbol FstCalcExport cfsm_label_ref (cfsm_label l, int i, cfsm_fsm fsm);
cfsm_symbol FstCalcExport cfsm_label_upperside(cfsm_label, cfsm_fsm);
cfsm_symbol FstCalcExport cfsm_label_lowerside(cfsm_label, cfsm_fsm);

/* DEPRECATED: Return the nth label in the given FSM's label set.  If there is
   no such label, return the invalid label.  Please use the following functions
   instead of this one. */
cfsm_label FstCalcExport cfsm_nth_label(cfsm_fsm, int n);

/* Return the size of the label set: */
int FstCalcExport cfsm_label_set_size(cfsm_fsm);
/* Store the FSM's label set in the given array: */
void FstCalcExport cfsm_label_set(cfsm_fsm fsm, cfsm_label* out);
/* Return the size of the symbol set (sigma): */
int FstCalcExport cfsm_symbol_set_size(cfsm_fsm fsm);
/* Store the FSM's set of symbols in the given array.  It is assumed that the array
   is large enough to hold cfsm_symbol_set_size() items. */
void FstCalcExport cfsm_symbol_set(cfsm_fsm fsm, cfsm_symbol* out);



/* Return the highest label number plus one.  The numbering includes symbol numbers.
   This is not the same as the number of labels in the FSM: */
int FstCalcExport cfsm_label_number_limit(cfsm_fsm);

/* Return the integer number for the given label, and vice versa: */
int FstCalcExport cfsm_label_number(cfsm_label);
cfsm_label FstCalcExport cfsm_number_to_label(int);

/* Return the integer number for the given symbol, and vice versa: */
int FstCalcExport cfsm_symbol_number(cfsm_symbol);
cfsm_symbol FstCalcExport cfsm_number_to_symbol(int);

/* Return the nth char in the given symbol: */
int FstCalcExport cfsm_symbol_ref(cfsm_symbol, int n);

/* Return the length in chars of the given symbol: */
int FstCalcExport cfsm_symbol_length(cfsm_symbol);


/* Return true if the given symbol is a valid symbol: */
int FstCalcExport cfsm_valid_symbol(cfsm_symbol);
/* Return the invalid symbol: */
cfsm_symbol FstCalcExport cfsm_the_invalid_symbol(void);

/* Return the symbol associated with the given name; if there exists no
   symbol by that name, return the invalid symbol: */
cfsm_symbol FstCalcExport cfsm_find_symbol(const char*, cfsm_fsm);

/* Return the label for the given symbol.  We assume that the singleton sym
   appears in the label set: */
cfsm_label FstCalcExport cfsm_symbol_label(cfsm_symbol);



cfsm_symbol FstCalcExport cfsm_make_symbol(const char*, cfsm_fsm);
cfsm_symbol FstCalcExport cfsm_make_unicode_symbol(const cfsm_fat_char* str, cfsm_fsm);
cfsm_symbol FstCalcExport cfsm_epsilon_symbol(void);
cfsm_symbol FstCalcExport cfsm_other_symbol(void);

/* Return true if the two symbols are the same. */
int FstCalcExport cfsm_symbol_eq (cfsm_symbol s1, cfsm_symbol s2);

/* Return true if the symbol is equivalent to the string, for example:
   if (cfsm_symbol_eq_string(sym, "*TAGSEP*")) ... */
int FstCalcExport cfsm_symbol_eq_string(cfsm_symbol sym, const char* str);

cfsm_symbol FstCalcExport cfsm_altchain_symbol (void);
int FstCalcExport cfsm_epsilon_p (cfsm_symbol s);
int FstCalcExport cfsm_other_p (cfsm_symbol s);
int FstCalcExport cfsm_symbol_name (cfsm_symbol sym, char* buffer, size_t buffer_size);
int FstCalcExport cfsm_fat_symbol_name (cfsm_symbol, cfsm_fat_char*, size_t);
int FstCalcExport cfsm_symbol_char_p (cfsm_symbol);
int FstCalcExport cfsm_symbol_multichar_p (cfsm_symbol);
int FstCalcExport cfsm_symbol_tag_p (cfsm_symbol);
int FstCalcExport cfsm_symbol_char (cfsm_symbol, cfsm_fsm);


int FstCalcExport cfsm_string_symbol_set(cfsm_fsm, char* buffer, size_t buffer_size);


cfsm_fsm FstCalcExport cfsm_make_empty_fsm (void);
void FstCalcExport cfsm_set_dirty_visit_marks(int b, cfsm_fsm);
void FstCalcExport cfsm_init_visit_marks(int val, cfsm_fsm);
void FstCalcExport cfsm_prune (cfsm_fsm);
void FstCalcExport cfsm_minimize (cfsm_fsm);
cfsm_fsm FstCalcExport cfsm_union(const cfsm_fsm* fsms, int num_fsms);

/* reorder_p: allow reordering of arcs */
void FstCalcExport cfsm_optimize_arcs
(cfsm_fsm net, cfsm_label* initial_labels, int num_initial_labels, int reorder_p);
/* Remove arc optimization.  Arcs will be reordered: */
void FstCalcExport cfsm_remove_arc_optimization(cfsm_fsm);
/* Take upper (lower) side of FST.  Inplace=true will change the given FST into an FSM;
   inplace=false will return a new FSM.  Minimize=false will inhibit minimization (sharing
   common endings). */
cfsm_fsm FstCalcExport cfsm_upper_side(cfsm_fsm m, int inplace, int minimize);
cfsm_fsm FstCalcExport cfsm_lower_side(cfsm_fsm m, int inplace, int minimize);

void FstCalcExport cfsm_erase_state_names (cfsm_fsm);
/* obsolete: */ void cfsm_set_state_names_smashed (int val, cfsm_fsm);
void FstCalcExport cfsm_free_fsm (cfsm_fsm);

void FstCalcExport clean_state_fields(cfsm_fsm net);
  /* For all states, STATE_visit_mark, STATE_client_cell,
     STATE_type_bit to NULL, STATE_deterministic to FALSE. */ 

void FstCalcExport cfsm_normal_form_net(cfsm_fsm net);
  /* Actualize the net: 
     - recompute the labels and sigma alphabets
     - update/reset network and state flags
     - minimize (and determinize)
     - reorder arcs (according to default_arc_comp_fn)
*/

cfsm_fsm FstCalcExport flag_diacritics_two_sided(cfsm_fsm net);
  /* Process instructions (aka flag_diacritics).  Convert any one-sided
     instructions to sequences such that all instructions are thereafter
     two-sided.  So convert: 

	@U.CAP.Y@:a ==> 0:a @U.CAP.Y@
	b:@U.CAP.Y@ ==> b:0 @U.CAP.Y@
	@U.CAP.Y@:0 ==>  @U.CAP.Y@
	0:@U.CAP.Y@ ==>  @U.CAP.Y@

      Report error and exit(1) on:

        @U.FL1.v1@:@U.FL2.v2@ 
*/

#ifdef __cplusplus
}
#endif


#endif /* CFSM_INCLUDED */
