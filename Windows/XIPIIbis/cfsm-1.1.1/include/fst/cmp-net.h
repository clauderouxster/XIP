/* $Id: cmp-net.h,v 1.5 2002/12/13 09:23:10 privault Exp $ */
/* Copyright (c) 1992-93 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  CMP-NET.H
 **
 **  Lauri Karttunen, Xerox PARC, October 1992
 **
 **  Conversion between standard and compact network formats.
 **
 ******************************************************/

#ifndef C_FSM_CMP_NET

#define C_FSM_CMP_NET

#ifdef __cplusplus
extern "C" {
#endif


#include "types.h"

void init_cs_mem(void);
void free_cs_mem(void);
void clear_cs_mem(void);
CSptr get_dest_state(void);
void free_cs(CSptr cs);

unsigned char *four_byte_label(unsigned char *loc);
unsigned char *two_byte_label(unsigned char *loc);
unsigned char FsmFstExport *decode_var_label(unsigned char *loc);
unsigned char *decode_var_addr(unsigned char *loc);
unsigned char *four_byte_data(STATEptr state, unsigned char *loc);
unsigned char *decode_var_data(STATEptr state, unsigned char *loc);
unsigned char *skip_four_bytes(unsigned char *loc);
unsigned char *skip_var_data(unsigned char *loc);
unsigned char *one_byte_addr(unsigned char *loc);
unsigned char *two_byte_addr(unsigned char *loc);
unsigned char *three_byte_addr(unsigned char *loc);
unsigned char *four_byte_addr(unsigned char *loc);
unsigned char *four_byte_absolute_addr(unsigned char *loc);
void FsmFstExport set_decoding_fns(char first_byte);

enum compact_options {SMALLEST = 1, SMALL = 2, FAST = 3, FASTEST = 4};

#define include_next_mask 128	/* Bit 7 */
#define more_to_come_p(X)		((X) & include_next_mask)

#define final_dest_bit 6	/* Special bits in the first label byte */
#define final_dest_mask 64	/* Bit 6 */

#define last_bit 5
#define last_mask 32	/* Bit 5 */

#define first_label_size 5	/* First five bits of the label start at 4 */
#define first_label_mask 31	/* 11111 in binary */
	
#define first_addr_size 7 	/* Address bits start at 6 */
#define first_addr_mask 127	/* 1111111 in binary */

#define next_data_size 7	/* Additional data in chunks of seven bits. */
#define next_data_mask 127	/* 1111111 in binary */


extern unsigned char FsmFstExport *ARC_BLOCK;
extern unsigned char FsmFstExport *START_LOC;
extern unsigned char FsmFstExport *DEST_ADDR;
extern unsigned char *ADDR_LIMIT;
extern id_type FsmFstExport LABEL_ID;
extern int FsmFstExport LABEL_LIMIT;
extern int FsmFstExport LAST_ARC_P;
extern int FsmFstExport DEST_FINAL_P;
extern unsigned char *(*DECODE_LABEL)(unsigned char *loc);
extern unsigned char FsmFstExport *(*DECODE_ADDR)(unsigned char *loc);
extern unsigned char FsmFstExport *(*DECODE_DATA)(STATEptr state, unsigned char *loc);
extern unsigned char FsmFstExport *(*SKIP_DATA)(unsigned char *loc);

extern int FsmFstExport SKIP_BYTES;
extern int FsmFstExport SKIP_DATA_P;
extern long int STATE_NAME;
extern id_type FsmFstExport *RECODE_KEY;

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_CMP_NET */

