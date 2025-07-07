/* $Id: define.h,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
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

int FstBaseExport define_label(id_type label_id, NETptr net, int recursive_p);
NETptr FstBaseExport reset_label(id_type label_id);
int FstBaseExport undefine_label(id_type label_id);
NETptr FstBaseExport get_def_uncopied(id_type label_id);
uint32 FstBaseExport def_id_hashfn(void *label_id);
int FstBaseExport def_id_comparefn(void *id1, void *id2);

NETptr FstBaseExport def_value(id_type label_id);
int FstBaseExport def_value_p(id_type label_id);

int FstBaseExport define_label_set(id_type label_id, NETptr net);
int FstBaseExport undefine_label_set(id_type label_id);
ALPHABETptr FstBaseExport label_set_value(id_type label_id);
int FstBaseExport label_set_value_p(id_type label_id);
int FstBaseExport label_set_net_p(NETptr net);
int FstBaseExport indirect_cmp_label_fn (id_type lab, id_type candidate_lab);

void FstBaseExport init_def_table(void);

extern HASH_TABLEptr FstBaseExport DEF_TABLE;
/* DEF_TABLE is defined in hash-table.c */

#define DEF_BUCKETS 50	/* Hash keys range from 0 to DEF_BUCKETS-1 */

#ifdef __cplusplus
}
#endif
#endif
