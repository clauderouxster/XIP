/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: iy-fs-ops.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

#define LEX_ANALYSE 0
#define LEX_GENERATE 1
enum { READ_FROM_CURRENT, READ_FROM_FILE, READ_FROM_PIPE };

/****************************************************************
  DUMMY FUNCTION THAT NEVER SHOULD BE CALLED.
  PLEASE, REMOVE IT IF YOU KNOW HOW TO MODIFY ALL PLACES
  FROM WHERE IT IS CALLED.
  02/03/99 AK
 ****************************************************************/

int unambiguous_p (NETptr net, int input_side);

/****************************************************************/

void iy_fs_stack_op(NETptr (*func)(), FST_CNTXTptr fst_cntxt);
void iy_fs_all1_op(NETptr (*func)());
void iy_fs_1_op(NETptr (*func)(), FST_CNTXTptr fst_cntxt);
void iy_fs_1_virt_op(NETptr (*func)(), FST_CNTXTptr fst_cntxt);
void iy_fs_2_op(NETptr (*func)(), FST_CNTXTptr fst_cntxt);
void iy_fs_2_virt_op(NETptr (*func)(), FST_CNTXTptr fst_cntxt);
void iy_test_2_op(int (*func)());
void iy_1_to_n_op(NVptr (*func)());
void iy_m_to_n_op (NVptr (*func)(), int m);
void iy_fs_apply(ifile *file, char *word, char *prompt, int input_side, void (*apply_fn)(char *));
void iy_apply_and_print(char *input);
void iy_apply_patterns(ifile *file, char *word, int input_side, char *prompt);
void iy_compose_apply_and_print(char *input);
int iy_tally_sigma_by_words(FILE *f, NETptr net, int n, char * sidestr);
void iy_print_net(FILE *, int, NETptr);
void iy_print_nets(FILE *, int);
void iy_print_words(FILE *, NETptr, int, int);
void iy_print_rand_words(FILE *, int, int);
void iy_do_word_to_numbers(NETptr net, char * word, int side, FILE *);
void iy_do_number_to_word(NETptr net, int num, int side, FILE *);

NETptr iy_negate(NETptr, int, int);

NETptr iy_one_plus(NETptr net, int copy_p, int minimize_p);
NETptr iy_zero_plus(NETptr net, int copy_p, int minimize_p);

NETptr iy_label_net(NETptr, int, int);
NETptr iy_lower(NETptr, int, int);
NETptr iy_upper(NETptr, int, int);

NETptr iy_reduce_labelset(NETptr net);
NETptr iy_unreduce_labelset(NETptr net);

NETptr iy_share_arcs(NETptr net);
NETptr iy_unshare_arcs(NETptr net);
NETptr network_from_lexc_file(FILE *stream);
int iy_read_networks(void* input_fn(FILE *stream), ifile *file);
void iy_net_to_text_file(void output_fn (NETptr, FILE *), FILE *ofile );

NETptr iy_compact_sigma(NETptr net);
NETptr iy_sigma_net(NETptr, int, int);
NETptr iy_single_char_sigma_net(NETptr, int, int);
NETptr iy_multi_char_sigma_net(NETptr net, int cp, int mn, FST_CNTXTptr fst_cntxt);
NETptr iy_paste_net_labels(NETptr net, int cp, int mn, FST_CNTXTptr fst_cntxt);

NETptr iy_TEST_1_unicode_parse (NETptr net, char * nonunic_str, FST_CNTXTptr fst_cntxt);

NETptr iy_sort (NETptr net);
NETptr iy_tabular(NETptr, int, int);
NETptr iy_add_prop(NETptr, int, int);
NETptr iy_add_props(NETptr net, int cp, int mn);
NETptr iy_reset_props(NETptr, int, int);
NETptr iy_edit_props(NETptr, int, int);
NETptr iy_name_net(NETptr, int, int);
NETptr iy_print_name_old(NETptr, int, int, FILE *);

int FstCalcExport iy_set_char_encoding(int valid_code, char *err_str);

NETptr iy_eliminate_flag(NETptr);

void iy_print_name( FILE * ofile);

NETptr iy_cleanup_net(NETptr net);
NETptr iy_optimize_arcs(NETptr net);
NETptr iy_unoptimize_arcs(NETptr net);
NETptr iy_ambclass(NETptr net);
NETptr iy_compile_replace(NETptr net);

#ifndef WORLD
NETptr iy_lseq_up(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt);
NETptr iy_lseq_down(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt);
NETptr iy_push_epsilons(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt);
NETptr iy_seq_up(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt);
NETptr iy_seq_down(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt);
NETptr iy_test_seq_up(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt);
NETptr iy_test_seq_down(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt);

NVptr iy_bimachine_up(NETptr net);
NVptr iy_bimachine_down(NETptr net);
int iy_test_unamb_up(NETptr net);
int iy_test_unamb_down(NETptr net);
NETptr iy_removeepsilon (NETptr net);

void iy_show_use_mmap(void);
void iy_set_use_mmap(int use_mmap);
#endif /* WORLD */

#ifndef WORLD
/* Various factorization commands */

NVptr iy_factorize_down (NVptr nv);
NVptr iy_factorize_up (NVptr nv);
NVptr iy_unfactorize_down (NVptr nv);
NVptr iy_unfactorize_up (NVptr nv);

NVptr iy_factorize_down_infinite_amb (NVptr nv);
NVptr iy_factorize_up_infinite_amb (NVptr nv);
NVptr iy_unfactorize_down_infinite_amb (NVptr nv);
NVptr iy_unfactorize_up_infinite_amb (NVptr nv);

NVptr iy_factorize_down_finite_amb (NVptr nv);
NVptr iy_factorize_up_finite_amb (NVptr nv);
NVptr iy_unfactorize_down_finite_amb (NVptr nv);
NVptr iy_unfactorize_up_finite_amb (NVptr nv);

NVptr iy_factorize_down_short_amb (NVptr nv);
NVptr iy_factorize_up_short_amb (NVptr nv);
NVptr iy_unfactorize_down_short_amb (NVptr nv);
NVptr iy_unfactorize_up_short_amb (NVptr nv);

NVptr iy_factorize_down_unambiguous (NVptr nv);
NVptr iy_factorize_up_unambiguous (NVptr nv);
NVptr iy_unfactorize_down_unambiguous (NVptr nv);
NVptr iy_unfactorize_up_unambiguous (NVptr nv);

NVptr iy_align_down_finite_amb (NVptr nv);
NVptr iy_align_up_finite_amb (NVptr nv);
NVptr iy_unalign_finite_amb (NVptr nv);

NVptr iy_reduce_down_intermediate_alph (NVptr nv);
NVptr iy_reduce_up_intermediate_alph (NVptr nv);

NVptr iy_test_down_fail_safety (NVptr nv);
NVptr iy_test_up_fail_safety (NVptr nv);
#endif /* WORLD */

NETptr iy_ambclass(NETptr net);
extern void *fs_arg1, *fs_arg2, *fs_arg3, *fs_arg4;

#ifndef max
#define max(a, b)       ((a) < (b) ? (b) : (a))
#endif

ALPHABETptr iy_new_alph();
void iy_add_symbol(ALPHABETptr alph, id_type id);
NETptr iy_symsubst(NETptr net);
NETptr iy_labsubst(NETptr net);
NETptr iy_defsubst(NETptr net);

#ifndef WORLD
NETptr iy_tb_insert(NETptr net);
NETptr iy_tagsep_insert(NETptr net);
NETptr iy_encode_nonid(NETptr net);
NETptr iy_tag_plus_strip_down (NETptr net);
NETptr iy_fsopt_reduce_sigma(NETptr net, 
			     int pointer_prefixes_desired, 
			     int save_mapper_fsts, 
			     int fsopt_interactive,
			     char * filler);
void iy_show_license_type(void);
#endif /* WORLD */

NETptr iy_twosided_flag_diacritics(NETptr net);
/* 2002-5-30 made public by TG */

/* Path addition */
#ifndef WORLD
NETptr iy_add_word_from_line (NETptr fst, char *word, char *model);
NETptr iy_add_word_from_file (NETptr fst, ifile *file);
NETptr iy_add_path_from_line (NETptr fst, char *path);
NETptr iy_add_path_from_file (NETptr fst, ifile *file);

#endif /* WORLD */
#ifndef WORLD  
NETptr iy_compact_net(NETptr net, int COMPACT_OPTION);
NETptr iy_uncompact_net(NETptr net);
#endif /* WORLD */

#ifndef WORLD  
void iy_show_license_type();
void iy_set_license_type(int license_type);
#endif /* WORLD */
