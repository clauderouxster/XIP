/* $Id: compact-defs.h,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  COMPACT.H
 **
 **  Lauri Karttunen, Xerox PARC, October 1992
 **
 **  Definitions for compact network formats.
 **
 ******************************************************/

#ifndef C_FSM_COMPACT_DEFS
#define C_FSM_COMPACT_DEFS

#ifdef __cplusplus
extern "C" {
#endif
 

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

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_COMPACT_DEFS */
