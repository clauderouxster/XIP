/* $Id: eqv-labs.h,v 1.14 2002/04/24 15:43:54 jmcoursi Exp $ */
/**************************************
 *
 *  EQV-LABS.H
 *
 *  Lauri Karttunen, RXRC
 *  December 1993
 *
 ***************************************/

#ifndef C_FSM_EQV_LABS

#define C_FSM_EQV_LABS



#ifdef __cplusplus
extern "C" {
#endif

enum keep_old_sigma_p {DONT_KEEP = 0, DO_KEEP = 1};

int FsmFstExport label_class_order (const void *x, const void *y);
ALPH_VECTORptr FsmFstExport eqv_classes (NVptr nets);
ALPH_VECTORptr FsmFstExport net_eqv_classes(NETptr net);
void inforce_singleton_classes
  (ALPH_VECTORptr eqv_partition, ALPHABETptr singletons);

int get_labels_from_prop_list(NETptr net, ALPHABETptr *labels);
int get_eqv_map_from_prop_list(NETptr net, SEQUENCEptr *seq);

long int *make_eqv_vector(ALPHABETptr alph, ALPH_VECTORptr eqv_partition,
	int *vector_size);
ALPH_VECTORptr FsmFstExport make_eqv_partition(ALPHABETptr alph, long int *eqv_vector,
    int vector_size);
SEQUENCEptr FsmFstExport make_eqv_label_seq (ALPHABETptr alph, ALPH_VECTORptr eqv_partition);
void FsmFstExport reduce_arcs (NETptr net, int *keep_map, int keep_old_sigma_p);
ALPHABETptr FsmFstExport get_unreduce_key(NETptr net);
ALPH_VECTORptr FsmFstExport get_reduced_to_unreduced_map(NETptr rlnet);
void FsmFstExport free_reduced_to_unreduced_map(ALPH_VECTORptr *rum);
#ifdef DEBUG
void FsmFstExport TEST_print_alphabet(ALPHABETptr aa);
void FsmFstExport TEST_rum(ALPH_VECTORptr rum);
#endif
ALPHABETptr FsmFstExport get_unreduced_labels(NETptr rlnet, ALPH_VECTORptr *rum);
NETptr FsmFstExport delete_eqv_arcs
  (NETptr net, ALPHABETptr singletons, int record_alphabet_p,
   int keep_old_sigma_p);
NETptr FsmFstExport restore_eqv_arcs(NETptr net,  ALPHABETptr alph);

NETptr reduce_eqv_arcs (NETptr net, ALPH_VECTORptr eqv_classes);
NETptr unreduce_eqv_arcs (NETptr net, ALPH_VECTORptr eqv_classes);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_EQV_LABS */
