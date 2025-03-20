/* $Id: define.h,v 1.8 2001/10/02 07:34:51 privault Exp $ */
/* Copyright (c) 1997 by the Xerox Corporation.  All rights reserved */

/***********************************************************
 *
 *		       DEFINE.H
 *
 *	Defining symbols as names of networks.
 *      These headers used to be in RE-AUX.H.
 *
 ***********************************************************/


#ifndef FST_DEFINE_H

#define FST_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

/* The macro SPECIAL_MATCH which is used in indirect_cmp_label_fn() 
   The definition is FALSE. Can be redefined to do something more
   interesting by the code calling this function.
 */
 
#define SPECIAL_MATCH(X, Y) FALSE

int FsmFstExport define_label(id_type label_id, NETptr net, int recursive_p);
NETptr reset_label(id_type label_id);
int undefine_label(id_type label_id);
NETptr FsmFstExport get_def_uncopied(id_type label_id);
uint32 def_id_hashfn(void *label_id);
int def_id_comparefn(void *id1, void *id2);

NETptr def_value(id_type label_id);
int FsmFstExport def_value_p(id_type label_id);

int define_label_set(id_type label_id, NETptr net);
int undefine_label_set(id_type label_id);
ALPHABETptr label_set_value(id_type label_id);
int label_set_value_p(id_type label_id);
int label_set_net_p(NETptr net);
int indirect_cmp_label_fn (id_type lab, id_type candidate_lab);

void init_def_table(void);

extern HASH_TABLEptr DEF_TABLE;
/* DEF_TABLE is defined in hash-table.c */

#define DEF_BUCKETS 50	/* Hash keys range from 0 to DEF_BUCKETS-1 */

#ifdef __cplusplus
}
#endif
#endif
