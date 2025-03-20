/* $Id: calculus.h,v 1.37 2003/01/10 08:32:47 jmcoursi Exp $ */
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

#include "modify.h"

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

NETptr lenient_compose(NETptr upper, NETptr lower);
NETptr FsmFstExport compose(NETptr upper, NETptr lower);
NETptr FsmFstExport concat(NETptr net1, NETptr net2);
NETptr contains(NETptr net);
NETptr FsmFstExport ignore(NETptr net1, NETptr net2);
NETptr FsmFstExport intersect(NETptr net1, NETptr net2);
NETptr FsmFstExport kleene_star(NETptr net);
NETptr FsmFstExport kleene_plus(NETptr net);
NETptr FsmFstExport minus(NETptr net1, NETptr net2);
NETptr negate(NETptr net);
NETptr FsmFstExport net_union(NETptr net1, NETptr net2);
NETptr FsmFstExport optional(NETptr net);
NETptr priority_union (NETptr net1, NETptr net2, int side);

  /* End of shortcuts */

NETptr FsmFstExport symbol_fsm(id_type id);
NETptr label_fsm (char *symbol);
NETptr kleene_plus_fsm(id_type id);
NETptr kleene_star_fsm(id_type id);
NETptr FsmFstExport sigma_star_fsm(void);
NETptr FsmFstExport path_fsm(id_type *path);
NETptr FsmFstExport string_to_fsm(char *str, int literal_p);
NETptr FsmFstExport utf8_string_to_fsm(char *str, int literal_p);
NETptr FsmFstExport unicode_string_to_fsm(FAT_STR str, int literal_p);
NETptr FsmFstExport sigma_fsm(ALPHABETptr alph);
NETptr multi_char_sigma_fsm(NETptr single_char_net, FST_CNTXTptr fst_cntxt);
NETptr single_char_sigma_fsm(ALPHABETptr alph);
int parse_unicode_string(FAT_STR string, NETptr single_char_symbol_fsm,  
			 id_type *result, int result_len, FST_CNTXTptr fst_cntxt);
int TEST1_parse_unicode_string (NETptr uc_fst, 
				char *nonunic_str,
				FST_CNTXTptr fst_cntxt);

NETptr sigma_but(ALPHABETptr alph);
NETptr FsmFstExport optional_fsm(NETptr net, int copy_p, int minimize_p);
NETptr FsmFstExport one_plus(NETptr net, int copy_p, int minimize_p);
NETptr FsmFstExport zero_plus(NETptr net, int copy_p, int minimize_p);
NETptr repeat_label(id_type label_id, int min, int max);
NETptr FsmFstExport repeat_fsm(NETptr net, int min, int max, int copy_p, int minimize_p);
NETptr contains_label(id_type id);
NETptr contains_no_label(id_type id);
NETptr FsmFstExport contains_fsm(NETptr net, int copy_p, int minimize_p);
NETptr contains_all(NVptr net, int copy_p, int minimize_p);
NETptr complete_fsm(NETptr net, int copy_p);
NETptr complement_side(NETptr net, int side, id_type fail_tag, int copy_p);
NETptr FsmFstExport negate_fsm(NETptr net, int copy_p, int minimize_p);
NETptr FsmFstExport negate_net(NETptr net, int copy_p, int minimize_p);
NETptr reverse_fsm(NETptr net, int copy_p, int minimize_p);
NETptr substring_fsm(NETptr net, int copy_p, int minimize_p);

NETptr FsmFstExport ignore_fsm(NETptr net, NETptr noise, int copy_net_p, int copy_fluff_p,
		  int minimize_p);
NETptr FsmFstExport ignore_labels(NETptr net, ALPHABETptr alph, int copy_p, int minimize_p);
NETptr ignore_label(NETptr net, id_type id, int copy_p, int minimize_p);
NETptr ignore_alph(NETptr net, ALPHABETptr alph, int copy_p, int min_p, int where, int side, int expand_other_p);

enum internal_p {EVERYWHERE, INTERNALLY};
enum expand_other_p {DONT_EXPAND, DO_EXPAND};

void extend_net_with_label(NETptr net, id_type id, int optional_p);
NETptr FsmFstExport concat_label(NETptr net, id_type id, int copy_p, int minimize_p);
NETptr FsmFstExport concat_fsm(NVptr nets, int copy_p, int minimize_p);
NETptr shuffle_fsm(NVptr nets, int copy_p, int minimize_p);

NETptr FsmFstExport union_fsm (NVptr net_list, int copy_p, int minimize_p);
int equal_length_relation_p(NETptr net);
NETptr FsmFstExport intersect_fsm(NVptr nets, int reclaim_p, int minimize_p);
int FsmFstExport intersect_p (NETptr n1, NETptr n2);
NETptr FsmFstExport minus_fsm(NETptr n1, NETptr n2, int reclaim_n1_p, int reclaim_n2_p,
	int minimize_p);

NETptr oneside_minus_fsm
  (NETptr n1, NETptr n2, int side,
   int reclaim_n1_p, int reclaim_n2_p, int minimize_p);
NETptr priority_union_fst_pair
  (NETptr n1, NETptr n2, int side, int copy_p, int minimize_p);
NETptr priority_union_nv
  (NVptr nv, int side, int copy_p, int minimize_p);

NETptr FsmFstExport invert_fst(NETptr net, int copy_p);
NETptr FsmFstExport upper_fsm(NETptr net, int copy_p, int minimize_p);
NETptr FsmFstExport lower_fsm(NETptr net, int copy_p, int minimize_p);
NETptr upper_fsm_no_compact(NETptr net, int copy_p, int minimize_p);
NETptr lower_fsm_no_compact(NETptr net, int copy_p, int minimize_p);

NETptr FsmFstExport compose_fst(NVptr nets, int reclaim_p, int minimize_p, int cleanup_p,
	int sequentially_p, int flags_special_p);
NETptr FsmFstExport crossproduct_fst (NETptr upper_net, NETptr lower_net, int reclaim_p, int minimize_p);
STATEptr FsmFstExport make_phantom_state(STATEptr model_state,
	NETptr *phantom_net, int no_arcs_p);
NETptr crossproduct_main(NETptr upper_net, NETptr lower_net, ALPHABETptr upper_other, ALPHABETptr lower_other, CONSptr **upper_array, id_type lower_max, HEAPptr cons_heap, int minimize_p);

NETptr pref_if_suff(NETptr pref, NETptr suff);
NETptr suff_if_pref(NETptr suff, NETptr pref);

NETptr pref_iff_suff(NETptr pref, NETptr suff);
NETptr suff_iff_pref(NETptr suff, NETptr pref);

NETptr suffix_iff_prefix(NETptr suffix, NETptr prefix);
NETptr prefix_iff_suffix(NETptr prefix, NETptr suffix);
NETptr prefix_iff_suffix_initial(NETptr prefix, NETptr suffix,
                                 NETptr extension);
NETptr suffix_iff_prefix_final(NETptr suffix, NETptr prefix, NETptr extension);

int side_universal_p (NETptr net, ALPHABETptr sigma, int side);

int scan_labels(NETptr net, NETptr other_net, id_type *max_label, id_type *max_low, id_type *max_hi, ALPHABETptr *other_sigma);

NETptr universal_language(void);
NETptr universal_relation(void);
NETptr universal_equal_length_relation(void);
NETptr universal_identity_relation(void);

NETptr FsmFstExport eliminate_flag_attribute(NETptr net, id_type attr, int reclaim_p,
				int minimize_p);

NETptr netsubst(id_type label_id, NETptr insert, NETptr target,
		int copy_target_p, int copy_insert_p, int minimize_p,
		int expand_other_p);

CONSptr *get_lower_array(CONSptr **upper_array, id_type upper,
			 id_type lower_max);
void free_upper_array(CONSptr **upper_array, ALPHABETptr upper_labels);

CONSptr expand_fstpair (id_type upper, id_type lower, ALPHABETptr upper_other, 
			ALPHABETptr lower_other, int non_identity_p,
			HEAPptr cons_heap);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_CALCULUS */
