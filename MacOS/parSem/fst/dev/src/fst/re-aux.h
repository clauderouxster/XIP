/* $Id: re-aux.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/***********************************************************
*
*			    RE-AUX.H
*
*	Function prototypes for RE.Y (source of RE-TAB.C) and RE-AUX.C
*
*
***********************************************************/

#ifndef C_FSM_RE_AUX
#define C_FSM_RE_AUX

#ifdef __cplusplus
extern "C" {
#endif

/* Macros to make the YACC grammar prettier to look at. */

#define new_alph make_alph(LIM_id_count(LABEL_MAP), BINARY_VECTOR)

  /*int regex_parse(void); */
int regex_error(char *str);
int regex_lex(void);

/* Functions called by action statements in REGEX.Y */

NETptr negate_lab(id_type id);

NETptr precedes(NETptr net1, NETptr net2);
NETptr follows(NETptr net1, NETptr net2);

NETptr shuffle(NETptr net1, NETptr net2);

NETptr oneside_minus (NETptr net1, NETptr net2, int side);

NETptr ignore_intern_fsm
  (NETptr net1, NETptr net2, int copy1_p, int copy2_p, int min_p);

NETptr ignore_internally(NETptr net1, NETptr net2);

NETptr starts_with(NETptr net);
NETptr ends_with(NETptr net);
NETptr starts_with_label(id_type label);
NETptr ends_with_label(id_type label);

NETptr FstCalcExport less_than(NETptr net, int n);

NETptr other_than(NETptr net);
NETptr other_than_lab(id_type id);

NETptr contains_set(NVptr net_seq);
NETptr at_most_one(NETptr net);
NETptr at_most_one_lab(id_type id);
NETptr exactly_one(NETptr net);
NETptr exactly_one_lab(id_type id);

NETptr slice(NETptr net, char *layer);
NETptr sym_subst(NETptr target, id_type label_id, ALPHABETptr alph);
NETptr net_subst(NETptr target, id_type label_id, NETptr insert);

void new_sym(ALPHABETptr alph, id_type label_id);

/* Functions for net vectors and networks */

NETptr copy_non_null_fsm(NETptr net);
NVptr NV_select_copy_nets(NVptr nv1, NVptr nv2, int start, int step);
NVptr NV_select_non_null_nets(NVptr nv, int copy_p);
NETptr union_null_fsm(NVptr nv, int copy_p, int minimize_p);
NETptr intersect_null_fsm(NVptr nv, int reclaim_p, int minimize_p);
NETptr compose_null_fst(NVptr nv, int reclaim_p, int minimize_p, int flags_special_p);
int NV_fst_check(NVptr nv);
NVptr NV_2_nets_to_nv(NETptr net1, NETptr net2);

/* Functions for replacement and restriction */

RSptr REAUX_rs_append (RSptr rs1, RSptr rs2);
RSptr REAUX_rs_upper_lower (NETptr upper, NETptr lower, int arrow);
NETptr REAUX_rs_left_right(NETptr left, NETptr right);
RSptr REAUX_crossprod_uplow_leftright
  (RSptr uplow, NVptr leftright, int ctxdir);
NETptr REAUX_rs_set_single_epsilon (NETptr net);
NETptr REAUX_rs_reset_single_epsilon (NETptr net);
NETptr REAUX_uncond_replace (RSptr uplow);
NETptr REAUX_conditional_replace (RSptr repl_rs);
NETptr REAUX_restrict_in_multctx (NETptr center, NVptr ctxlist, int arrow);


#ifdef __cplusplus
}
#endif

#endif /* C_FSM_RE_AUX */
