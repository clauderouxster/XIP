/* $Id: modify.h,v 1.34 2002/11/29 08:32:55 privault Exp $ */
/* Copyright (c) 1990-94 Xerox Corporation. All rights reserved. */
/***************************
 *
 * MODIFY.H
 *
 * Function prototypes for functions that maintain
 * and modify fsms. This used to be called BASICS.H.
 *
 *
 ***************************/

#ifndef C_FSM_MODIFY

#define C_FSM_MODIFY

#include "struct.h"
#include "dbl-link.h"
#include "copy-fsm.h"
#include "eqv-labs.h"
#include "net-io.h"
#include "rename.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Set up of the global restore_functions array. */
void FsmFstExport init_restore_functions();

/* Set up of the global save_functions array. */
void FsmFstExport init_save_functions();

int symbol_name_from_line(char **name, char** line);

void prepare_arc_marks(NETptr net, int mark);
void FsmFstExport prepare_state_marks(NETptr net, int mark);
NVptr remove_duplicates(NVptr nv);

int FsmFstExport longest_minimal_string(NETptr net, int side, int obey_flags_p);
int print_longest_minimal_string(NETptr net, int side, int obey_flags_p, int escape_p, FILE *ffi);

#define clear_arc_marks(X)	prepare_arc_marks((X), NOT_VISITED)
#define clear_state_marks(X) prepare_state_marks((X), NOT_VISITED)

void clear_all_visit_marks_and_state_client_cells(NETptr net);
void FsmFstExport clear_client_cells(NETptr net);
void clear_state_marks_and_client_cells(NETptr net);
void FsmFstExport prepare_visit_marks(NETptr net, int mark);
void mark_reachable_states(STATEptr state, int mark, STACKptr stack);
int FsmFstExport null_fsm_p(NETptr net);
int FsmFstExport epsilon_fsm_p(NETptr net);
int deterministic_arc_set_p(ARCptr arc);
int disjoint_arcs_p(STATEptr s1, STATEptr s2);
int FsmFstExport has_arc_p(STATEptr state, id_type id);
int start_loops_p(NETptr net);
int FsmFstExport sigma_star_fsm_p(NETptr net);

int sigma_fsm_p(NETptr net);
int FsmFstExport sublanguage_p (NETptr net1, NETptr net2);
int equiv_fsm_p (NETptr net1, NETptr net2);
int occurs_as_label_component_p(id_type id, ALPHABETptr labs);

int FsmFstExport linearbounded_p(NETptr net, int side);

void FsmFstExport invert_arcs(NETptr net, int mark);

int FsmFstExport index_net (NETptr net);

int state_mark_zero_p(STATEptr state);
int state_mark_one_p(STATEptr state);

NETptr make_connected(NETptr net, int inverted_p);

int invert_tuple(id_type *inv_id, LABELptr label);

  /* void set_net_arity(NETptr net);  -- displaced to struct.h TG */

int label_member(ALPHABETptr labels, id_type item);
ALPHABETptr FsmFstExport subtract_alph(ALPHABETptr a1, ALPHABETptr a2);

int sigma_disjoint_p(ALPHABETptr s1, ALPHABETptr s2);
void FsmFstExport align_sigmas(NETptr net1, NETptr net2);
void align_multiple_sigmas(NVptr nv);
void binary_labels_add_to(ALPHABETptr labels, id_type id);

void FsmFstExport label_check (NETptr net);
void label_type_conv (int old_type, int new_type);

void renumber_net(NETptr net);
ALPHABETptr renumber_nets(NVptr nets);

NETptr FsmFstExport symsubst(id_type old_sym, ALPHABETptr new_syms, NETptr net,
		int copy_p, int no_connect_p);

NETptr substitute_symbol(id_type old_sym, ALPHABETptr new_syms, NETptr net,
			 int copy_p, int no_connect_p);

NETptr substitute_symbol_alph(ALPHABETptr old_syms, ALPHABETptr new_syms,
			      NETptr net, int copy_p, int no_connect_p);

NETptr substitute_label(id_type old_label, ALPHABETptr new_labels, NETptr net,
			int copy_p, int no_connect_p);

NETptr substitute_label_alph(ALPHABETptr old_labels, ALPHABETptr new_labels,
			     NETptr net, int copy_p, int no_connect_p);

int interactive_substitute(FILE *stream, NVptr nets, int minimize_p);

/* Displaced to calculus.h to resolve a library cross-reference problem TG 
NETptr netsubst(id_type label_id, NETptr insert, NETptr target,
		int copy_target_p, int copy_insert_p, int minimize_p,
		int expand_other_p);
		*/ 
				 
NETptr splice_fsm (NETptr target, STATEptr from_state, STATEptr to_state,
			 NETptr insert, int expand_other_p, id_type beware_label);

NETptr expand_tuple_other(NETptr net, int copy_p);
NETptr FsmFstExport expand_other (NETptr net, ALPHABETptr other_sigma, int copy_p);
ALPHABETptr expand_other_in_alph(ALPHABETptr alph, ALPHABETptr other_sigma, int copy_p);
NETptr FsmFstExport close_sigma(NETptr net, ALPHABETptr new_symbols,
		   int copy_p, int minimize_p);

NETptr FsmFstExport prune_fsm (NETptr net, int copy_p);
NETptr epsilon_remove (NETptr net, int copy_p);
NETptr FsmFstExport determinize_fsm(NETptr net, int copy_p);
int determinize_basic (NETptr net);
void prune_sigma(NETptr net);
void FsmFstExport compact_sigma(NETptr net);
NETptr FsmFstExport min_fsm(NETptr net, int copy_p);

NETptr cleanup_fst(NETptr net, int copy_p, int minimize_p);

void FsmFstExport sever_states(NETptr net);

FsmFstExport void recover_temp_memory(void);

NVptr copy_nv_and_nets(NVptr old);


void rmap_net(NETptr net, int pre_state_fn(STATEptr), int arc_fn(ARCptr),
	int post_state_fn(STATEptr), STACKptr stack);

/*-----------------------------------------------------------*/
/* BEGIN: Local Epsilon Pushing */

#define LEP_STATE_twin(S)    (*((STATEptr *) &((S)->client_cell)))
#define LEP_STATE_count(S)   (*((int *) &((S)->client_cell)))

#define LEP_SEQ_LENGTH_INIT  1000

NETptr LEP_local_eps_push_fst (NETptr oldnet, int reclaim_p, int minimize_p);

/* END: Local Epsilon Pushing */
/*-----------------------------------------------------------*/

/* Add path */
NETptr FsmFstExport add_path(NETptr fst, id_type* path);
NETptr add_word(NETptr fst, id_type* word, id_type* model);

    
#ifdef __cplusplus
}
#endif

#endif /* C_FSM_MODIFY */

