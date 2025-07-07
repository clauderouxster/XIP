/* $Id: compact.h,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  COMPACT.H
 **
 **  Lauri Karttunen, Xerox PARC, October 1992
 **
 **  Conversion between standard and compact network formats.
 **
 ******************************************************/

#ifndef C_FSM_COMPACT
#define C_FSM_COMPACT

#include "fst/arc-optimization.h"
#include "compact-defs.h"

#ifdef __cplusplus
extern "C" {
#endif
 
void init_cs_mem(void);
void free_cs(CSptr cs);
void free_cs_mem(void);
void clear_cs_mem(void);
/* Sorts arcs by increasing lower label values. */
void sort_arcs_lower_label(NETptr net);
unsigned long int collect_state_statistics(NETptr net, int speed_option,
					   int *addr_size);

void abs_to_rel_block(NETptr net);

CSptr get_dest_state(void);
NETptr FstCalcExport compact_net(NETptr net, int speed_option);
NETptr FstCalcExport uncompact_net(NETptr net);

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

#endif /* C_FSM_COMPACT */



