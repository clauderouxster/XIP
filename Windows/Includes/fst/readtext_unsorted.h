/* $Id: readtext_unsorted.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 2001-2004 Xerox Corporation. All rights reserved. */

/********************************************************************************
 **
 **  readtext_unsorted.h
 **
 **	 Florent NICART, Franck GUINGNE, Xerox XRCE Grenoble, June 2001
 **
 **  Function prototypes and global variables for add_alg2.c
 **
 ********************************************************************************/

#ifndef C_FSM_READ_TEXT_UNSORTED

#define C_FSM_READ_TEXT_UNSORTED

#ifdef __cplusplus
extern "C" {
#endif

typedef int BOOL;

NETptr seqs_to_net(id_type *(*seq_form)(void));

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_READ_TEXT_UNSORTED */

