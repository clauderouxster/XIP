/* $Id: dbl-link.h,v 1.2 2000/01/12 09:24:24 privault Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  DBL-LINK.H
 **
 **	 Todd Yampol, Xerox PARC, May 1990
 **
 **  exported definitions, variables, and function 
 **	 prototypes for double-linked lists.
 **
 ******************************************************/

#ifndef C_FSM_DBL_LINK

#define C_FSM_DBL_LINK

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DOUBLE_LINK {
	STATEptr state;
	struct DOUBLE_LINK  *dbl_class;		/* points to head cell, for quick access */
	struct DOUBLE_LINK  *back_link;
	struct DOUBLE_LINK  *CDR;
} DBLtype, *DBLptr;

#define DBL_state(X) 		(X)->state
#define DBL_class(X) 		(X)->dbl_class
#define DBL_back_link(X) 	(X)->back_link
#define DBL_cdr(X) 			(X)->CDR

int init_dbl_mem(long size);
void free_dbl_mem(void);
DBLptr make_dbl(STATEptr state);
DBLptr dbl_splice (DBLptr prior, DBLptr link, DBLptr next);
int dbl_longer_p(register DBLptr var_lst1, register DBLptr var_lst2);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_DBL_LINK */
