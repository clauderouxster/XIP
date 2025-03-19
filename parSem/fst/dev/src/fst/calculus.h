/* $Id: calculus.h,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

/**************
 **
 **  CALCULUS.H
 **
 **************/

#ifndef C_FSM_CALCULUS

#define C_FSM_CALCULUS

#ifdef __cplusplus
extern "C" {
#endif

#include "fst/modify.h"

/* Use these constants as SMASH_N1_P and SMASH_N2_P arguments to MINUS_FSM. */
/*
  Eliminating the SMASH constants to avoid confusion with
  DONT_RECLAIM and DO_RECLAIM.    L.K. 96-09-05

enum {DONT_SMASH, DO_SMASH};
*/

/* Use these constants as arguments to COMPOSE_FST. */

/*
enum {DONT_RECLAIM, DO_RECLAIM};
*/

/* Switching the order so that DO_RECLAIM == DONT_COPY == 0
   and DONT_RECLAIM == DO_COPY == 1  L.K. 96-09-05
   To make sure it stays that way, moved the definition
   of DO_RECLAIM and DONT_RECLAIM to types.h
   */

  /* enum {SEQUENTIALLY, PAIRWISE}; definition moved to types.h */

  /*  These shortcuts to commonly used  calculus operations used
      to be in RE-AUX.C */

NETptr FstCalcExport lenient_compose(NETptr upper, NETptr lower);
NETptr FstCalcExport compose(NETptr upper, NETptr lower);
NETptr FstCalcExport concat(NETptr net1, NETptr net2);
NETptr FstCalcExport contains(NETptr net);
NETptr FstCalcExport ignore(NETptr net1, NETptr net2);
NETptr FstCalcExport intersect(NETptr net1, NETptr net2);
NETptr FstCalcExport kleene_star(NETptr net);
NETptr FstCalcExport kleene_plus(NETptr net);
NETptr FstCalcExport minus(NETptr net1, NETptr net2);
NETptr FstCalcExport negate(NETptr net);
NETptr FstCalcExport net_union(NETptr net1, NETptr net2);
NETptr FstCalcExport optional(NETptr net);
NETptr FstCalcExport priority_union (NETptr net1, NETptr net2, int side);

  /* End of shortcuts */

NETptr FstCalcExport symbol_fsm(id_type id);
NETptr FstCalcExport label_fsm (char *symbol);
NETptr FstCalcExport kleene_plus_fsm(id_type id);
NETptr FstCalcExport kleene_star_fsm(id_type id);
NETptr FstCalcExport sigma_star_fsm(void);
NETptr FstCalcExport path_fsm(id_type *path);
NETptr FstCalcExport string_to_fsm(char *str, int literal_p);
NETptr FstCalcExport utf8_string_to_fsm(char *str, int literal_p);
NETptr FstCalcExport unicode_string_to_fsm(FAT_STR str, int literal_p);
NETptr FstCalcExport sigma_fsm(ALPHABETptr alph);
NETptr FstCalcExport multi_char_sigma_fsm(NETptr single_char_net, FST_CNTXTptr fst_cntxt);
NETptr FstCalcExport single_char_sigma_fsm(ALPHABETptr alph);
int FstCalcExport parse_unicode_string(FAT_STR string, NETptr single_char_symbol_fsm,  
			 id_type *result, int result_len, FST_CNTXTptr fst_cntxt);
int FstCalcExport TEST1_parse_unicode_string (NETptr uc_fst, 
				char *nonunic_str,
				FST_CNTXTptr fst_cntxt);

NETptr FstCalcExport sigma_but(ALPHABETptr alph);
NETptr FstCalcExport optional_fsm(NETptr net, int copy_p, int minimize_p);
NETptr FstCalcExport one_plus(NETptr net, int copy_p, int minimize_p);
NETptr FstCalcExport zero_plus(NETptr net, int copy_p, int minimize_p);
NETptr FstCalcExport repeat_label(id_type label_id, int min, int max);
NETptr FstCalcExport repeat_fsm(NETptr net, int min, int max, int copy_p, int minimize_p);
NETptr FstCalcExport contains_label(id_type id);
NETptr FstCalcExport contains_no_label(id_type id);
NETptr FstCalcExport contains_fsm(NETptr net, int copy_p, int minimize_p);
NETptr FstCalcExport contains_all(NVptr net, int copy_p, int minimize_p);
NETptr FstCalcExport complete_fsm(NETptr net, int copy_p);
NETptr FstCalcExport complement_side(NETptr net, int side, id_type fail_tag, int copy_p);
NETptr FstCalcExport negate_fsm(NETptr net, int copy_p, int minimize_p);
NETptr FstCalcExport negate_net(NETptr net, int copy_p, int minimize_p);
NETptr FstCalcExport reverse_fsm(NETptr net, int copy_p, int minimize_p);
NETptr FstCalcExport substring_fsm(NETptr net, int copy_p, int minimize_p);
NETptr brzozowski_minimize(NETptr net, int copy_p);

NETptr FstCalcExport ignore_fsm(NETptr net, NETptr noise, int copy_net_p, int copy_fluff_p,
		  int minimize_p);
NETptr FstCalcExport ignore_labels(NETptr net, ALPHABETptr alph, int copy_p, int minimize_p);
NETptr FstCalcExport ignore_label(NETptr net, id_type id, int copy_p, int minimize_p);
NETptr FstCalcExport ignore_alph(NETptr net, ALPHABETptr alph, int copy_p, int min_p, int where, int side, int expand_other_p);

enum internal_p {EVERYWHERE, INTERNALLY};
enum expand_other_p {DONT_EXPAND, DO_EXPAND};

void FstCalcExport extend_net_with_label(NETptr net, id_type id, int optional_p);
NETptr FstCalcExport concat_label(NETptr net, id_type id, int copy_p, int minimize_p);
NETptr FstCalcExport concat_fsm(NVptr nets, int copy_p, int minimize_p);
NETptr FstCalcExport shuffle_fsm(NVptr nets, int copy_p, int minimize_p);

NETptr FstCalcExport union_fsm (NVptr net_list, int copy_p, int minimize_p);
int FstCalcExport equal_length_relation_p(NETptr net);
NETptr FstCalcExport intersect_fsm(NVptr nets, int reclaim_p, int minimize_p);
int FstCalcExport intersect_p (NETptr n1, NETptr n2);
NETptr FstCalcExport minus_fsm(NETptr n1, NETptr n2, int reclaim_n1_p, int reclaim_n2_p,
	int minimize_p);

NETptr FstCalcExport oneside_minus_fsm
  (NETptr n1, NETptr n2, int side,
   int reclaim_n1_p, int reclaim_n2_p, int minimize_p);
NETptr FstCalcExport priority_union_fst_pair
  (NETptr n1, NETptr n2, int side, int copy_p, int minimize_p);
NETptr FstCalcExport priority_union_nv
  (NVptr nv, int side, int copy_p, int minimize_p);

NETptr FstCalcExport invert_fst(NETptr net, int copy_p);
NETptr FstCalcExport upper_fsm(NETptr net, int copy_p, int minimize_p);
NETptr FstCalcExport lower_fsm(NETptr net, int copy_p, int minimize_p);
NETptr FstCalcExport upper_fsm_no_compact(NETptr net, int copy_p, int minimize_p);
NETptr FstCalcExport lower_fsm_no_compact(NETptr net, int copy_p, int minimize_p);

NETptr FstCalcExport compose_fst(NVptr nets, int reclaim_p, int minimize_p, int cleanup_p,
	int sequentially_p, int flags_special_p);
NETptr FstCalcExport crossproduct_fst (NETptr upper_net, NETptr lower_net, int reclaim_p, int minimize_p);
STATEptr FstCalcExport make_phantom_state(STATEptr model_state,
	NETptr *phantom_net, int no_arcs_p, int flags_special_p);
NETptr FstCalcExport crossproduct_main(NETptr upper_net, NETptr lower_net, ALPHABETptr upper_other, ALPHABETptr lower_other, CONSptr **upper_array, id_type lower_max, HEAPptr cons_heap, int minimize_p);

NETptr FstCalcExport pref_if_suff(NETptr pref, NETptr suff);
NETptr FstCalcExport suff_if_pref(NETptr suff, NETptr pref);

NETptr FstCalcExport pref_iff_suff(NETptr pref, NETptr suff);
NETptr FstCalcExport suff_iff_pref(NETptr suff, NETptr pref);

NETptr FstCalcExport suffix_iff_prefix(NETptr suffix, NETptr prefix);
NETptr FstCalcExport prefix_iff_suffix(NETptr prefix, NETptr suffix);
NETptr FstCalcExport prefix_iff_suffix_initial(NETptr prefix, NETptr suffix,
                                 NETptr extension);
NETptr FstCalcExport suffix_iff_prefix_final(NETptr suffix, NETptr prefix, NETptr extension);

int FstCalcExport side_universal_p (NETptr net, ALPHABETptr sigma, int side);

int FstCalcExport scan_labels(NETptr net, NETptr other_net, id_type *max_label, id_type *max_low, id_type *max_hi, ALPHABETptr *other_sigma);

NETptr FstCalcExport universal_language(void);
NETptr FstCalcExport universal_relation(void);
NETptr FstCalcExport universal_equal_length_relation(void);
NETptr FstCalcExport universal_identity_relation(void);

NETptr FstCalcExport eliminate_flag_attribute(NETptr net, id_type attr, int reclaim_p,
				int minimize_p);

NETptr FstCalcExport netsubst(id_type label_id, NETptr insert, NETptr target,
		int copy_target_p, int copy_insert_p, int minimize_p,
		int expand_other_p);

CONSptr FstCalcExport *get_lower_array(CONSptr **upper_array, id_type upper, id_type lower,
			 id_type lower_max);
void FstCalcExport free_upper_array(CONSptr **upper_array, ALPHABETptr upper_labels);

CONSptr FstCalcExport expand_fstpair (id_type upper, id_type lower, ALPHABETptr upper_other, 
			ALPHABETptr lower_other, int non_identity_p,
			HEAPptr cons_heap);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_CALCULUS */
