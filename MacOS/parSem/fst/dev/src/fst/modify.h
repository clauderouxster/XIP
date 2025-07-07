/* $Id: modify.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
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

#include "fst/struct.h"
#include "fst/dbl-link.h"
#include "fst/copy-fsm.h"
#include "fst/eqv-labs.h"
#include "fst/net-io.h"
#include "fst/rename.h"
#include "fst/modify-base.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Set up of the global save_functions array. */
void FstCalcExport init_save_functions();

int FstCalcExport symbol_name_from_line(char **name, char** line);

int FstCalcExport longest_minimal_string(NETptr net, int side, int obey_flags_p);
int FstCalcExport print_longest_minimal_string(NETptr net, int side, int obey_flags_p, int escape_p, FILE *ffi);

int FstCalcExport null_fsm_p(NETptr net);
int FstCalcExport epsilon_fsm_p(NETptr net);
int FstCalcExport deterministic_arc_set_p(ARCptr arc);
int FstCalcExport disjoint_arcs_p(STATEptr s1, STATEptr s2);
int FstCalcExport has_arc_p(STATEptr state, id_type id);
int FstCalcExport start_loops_p(NETptr net);
int FstCalcExport sigma_star_fsm_p(NETptr net);

int FstCalcExport sigma_fsm_p(NETptr net);
int FstCalcExport sublanguage_p (NETptr net1, NETptr net2);
int FstCalcExport equiv_fsm_p (NETptr net1, NETptr net2);
int FstCalcExport occurs_as_label_component_p(id_type id, ALPHABETptr labs);

int FstCalcExport linearbounded_p(NETptr net, int side);

int FstCalcExport invert_tuple(id_type *inv_id, LABELptr label);

  /* void set_net_arity(NETptr net);  -- displaced to struct.h TG */

int FstCalcExport label_member(ALPHABETptr labels, id_type item);
ALPHABETptr FstCalcExport subtract_alph(ALPHABETptr a1, ALPHABETptr a2);

int FstCalcExport sigma_disjoint_p(ALPHABETptr s1, ALPHABETptr s2);
void FstCalcExport align_sigmas(NETptr net1, NETptr net2);
void FstCalcExport align_multiple_sigmas(NVptr nv);
void FstCalcExport binary_labels_add_to(ALPHABETptr labels, id_type id);

void FstCalcExport label_type_conv (int old_type, int new_type);

NETptr FstCalcExport symsubst(id_type old_sym, ALPHABETptr new_syms, NETptr net,
		int copy_p, int no_connect_p);

NETptr FstCalcExport substitute_symbol(id_type old_sym, ALPHABETptr new_syms, NETptr net,
			 int copy_p, int no_connect_p);

NETptr FstCalcExport substitute_symbol_alph(ALPHABETptr old_syms, ALPHABETptr new_syms,
			      NETptr net, int copy_p, int no_connect_p);

NETptr FstCalcExport substitute_label(id_type old_label, ALPHABETptr new_labels, NETptr net,
			int copy_p, int no_connect_p);

NETptr FstCalcExport substitute_label_alph(ALPHABETptr old_labels, ALPHABETptr new_labels,
			     NETptr net, int copy_p, int no_connect_p);

int FstCalcExport interactive_substitute(FILE *stream, NVptr nets, int minimize_p);

/* Displaced to calculus.h to resolve a library cross-reference problem TG 
NETptr netsubst(id_type label_id, NETptr insert, NETptr target,
		int copy_target_p, int copy_insert_p, int minimize_p,
		int expand_other_p);
		*/ 
				 
NETptr FstCalcExport splice_fsm (NETptr target, STATEptr from_state, STATEptr to_state,
			 NETptr insert, int expand_other_p, id_type beware_label);

NETptr FstCalcExport expand_tuple_other(NETptr net, int copy_p);
NETptr FstCalcExport expand_other (NETptr net, ALPHABETptr other_sigma, int copy_p);
ALPHABETptr FstCalcExport expand_other_in_alph(ALPHABETptr alph, ALPHABETptr other_sigma, int copy_p);
NETptr FstCalcExport close_sigma(NETptr net, ALPHABETptr new_symbols,
		   int copy_p, int minimize_p);

void FstCalcExport compact_sigma(NETptr net);

NETptr FstCalcExport cleanup_fst(NETptr net, int copy_p, int minimize_p);

void FstCalcExport sever_states(NETptr net);

NVptr FstCalcExport copy_nv_and_nets(NVptr old);


void FstCalcExport rmap_net(NETptr net, int pre_state_fn(STATEptr), int arc_fn(ARCptr),
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
NETptr FstCalcExport add_path(NETptr fst, id_type* path);
NETptr FstCalcExport add_word(NETptr fst, id_type* word, id_type* model);

    
#ifdef __cplusplus
}
#endif

#endif /* C_FSM_MODIFY */

