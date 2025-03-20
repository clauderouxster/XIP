/* $Id: debug.h,v 1.8 2001/08/30 07:45:19 jmcoursi Exp $ */
/**************************************
 *
 *  DEBUG.H
 *
 *  Lauri Karttunen, RXRC
 *  December 1993
 *
 *
 *  COLLECTION OF TEST FUNCTIONS
 *
 *
 ***************************************/

#ifdef DEBUG
#ifndef DEBUG_H

#define DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif
/*-----------------------------------------------------------*/

#include "apply.h"

void TEST_print_ptr_vec (void **vec, long len);
void TEST_print_long_vec (long *vec, long len);
void TEST_print_idtype_vec (id_type *vec, long len);

NETptr TEST_print_net (NETptr net);
NVptr TEST_print_nv(NVptr nv);

void TEST_print_storage ();
void TEST_num_nets();

void TEST_xprint_net(NETptr net, int smax, int amax);

void TEST_save_net (NETptr net, char *filename);

void TEST_print_label (id_type id);
void TEST_print_alph (ALPHABETptr alph);

void TEST_print_arc_list (ARCptr arc);
void TEST_print_state (STATEptr state);
void TEST_print_net_info (NETptr net);

void TEST_print_io_seq(IO_SEQptr io_seq);

/*-----------------------------------------------------------*/

void FsmFstExport debug_print_arc(ARCptr narc, int arc_number);
void debug_print_state(STATEptr nstate, int state_number, int with_arcs);
void FsmFstExport debug_print_net(NETptr net, int with_arcs);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */
#endif /* DEBUG */
