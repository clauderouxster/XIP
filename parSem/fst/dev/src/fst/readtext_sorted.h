/* $Id: readtext_sorted.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 2001-2004 Xerox Corporation. All rights reserved. */

/********************************************************************************
 **
 **  readtext_sorted.h
 **
 **	 Florent NICART, Franck GUINGNE, Xerox XRCE Grenoble, April 2001
 **
 **  Function prototypes and global variables for readtext_sorted.c
 **
 ********************************************************************************/

#ifndef C_FSM_READ_TEXT_SORTED

#define C_FSM_READ_TEXT_SORTED

#ifdef __cplusplus
extern "C" {
#endif


NETptr seqs_to_net(id_type *(*seq_form)(void));

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_READ_TEXT_SORTED */

