/* $Id: flags.h,v 1.10 2002/06/11 15:25:05 jmcoursi Exp $ */
/* Copyright (c) 1997 Xerox Xorporation. All rights reserved */

/**************
 **
 **  FLAGS.H
 **
 **************/


#ifndef C_FSM_FLAGS
#define C_FSM_FLAGS

#ifdef __cplusplus
extern "C" {
#endif

int print_net_flag_diacritics(NETptr net, FILE *stream);
void FsmFstExport check_for_flags(NETptr net);
int FsmFstExport has_flag_diacritics_p(NETptr net);
int has_flag_diacritics_side_p(NETptr net, int side);
ALPHABETptr FsmFstExport make_flag_register(void);
int FsmFstExport flag_action(ALPHABETptr register, FLAG_DIACRptr flag);
int flag_attribute_occurs_in_p(NETptr net, id_type attr);

#define flag_value(X, Y)      ALPH_item((X), (Y))

/* added by CP: 25/05/99: for the id extension */
#define FLAG_DIACR_complement(X)      (X ^ 0x8000)
#define FLAG_DIACR_is_a_complement(X) (X & 0x8000) /* false iff X has its last bit
					   on the left equal to zero */

#ifdef __cplusplus
}
#endif
    
#endif /* C_FSM_FLAGS */
