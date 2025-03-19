/* $Id: flags.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
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
void FstBaseExport check_for_flags(NETptr net);
int FstBaseExport has_flag_diacritics_p(NETptr net);
int FstBaseExport has_flag_diacritics_side_p(NETptr net, int side);
ALPHABETptr FstBaseExport make_flag_register(void);
int FstBaseExport flag_action(ALPHABETptr register, FLAG_DIACRptr flag);
int FstBaseExport flag_attribute_occurs_in_p(NETptr net, id_type attr);

#define flag_value(X, Y)      ALPH_item((X), (Y))

/* added by CP: 25/05/99: for the id extension */
#define FLAG_DIACR_complement(X)      (X ^ 0x8000)
#define FLAG_DIACR_is_a_complement(X) (X & 0x8000) /* false iff X has its last bit
					   on the left equal to zero */

#ifdef __cplusplus
}
#endif
    
#endif /* C_FSM_FLAGS */
