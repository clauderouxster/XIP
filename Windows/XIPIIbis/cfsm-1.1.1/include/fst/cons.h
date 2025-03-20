/* $Id: cons.h,v 1.6 2000/06/27 07:33:59 jmcoursi Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  CONS.H
 **
 **	 Todd Yampol, Xerox PARC, June 1990
 **
 **  exported definitions, variables, and function 
 **	 prototypes for cons cells.
 **
 ******************************************************/
 
#ifndef C_FSM_CONS
 
#define C_FSM_CONS

#ifdef __cplusplus
extern "C" {
#endif
 
typedef struct CONS {
	void *CAR;
	struct CONS *CDR;
} CONStype, *CONSptr;

#define car(X)	(X)->CAR
#define caar(X) car((CONSptr) car(X))
#define cdr(X)	(X)->CDR
#define cdar(X) cdr((CONSptr) car(X))
#define rplaca(X, Y) (X)->CAR = (Y)
#define rplacd(X, Y) (X)->CDR = (Y)



void FsmFstExport init_cons_mem(void);
HEAPptr make_cons_heap(void);
void FsmFstExport free_cons_mem(void);

int list_len(CONSptr list);

CONSptr FsmFstExport cons(void *CAR, CONSptr CDR);
CONSptr cons_cell(void *CAR, CONSptr CDR, HEAPptr cons_heap);

CONSptr copy_list (CONSptr orig);
CONSptr copy_cell_list (CONSptr orig, HEAPptr cons_heap);

CONSptr cons_append(CONSptr list1, CONSptr list2);
CONSptr cons_cell_append(CONSptr list1, CONSptr list2, HEAPptr cons_heap);

CONSptr freeing_CDR(CONSptr cons);
CONSptr freeing_CDR_cell(CONSptr cons, HEAPptr cons_heap);

void free_list(CONSptr list);
void FsmFstExport free_cell_list(CONSptr list, HEAPptr cons_heap);

void FsmFstExport free_cons(CONSptr cons);
void FsmFstExport free_cons_cell(CONSptr cons, HEAPptr cons_heap);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_CONS */
