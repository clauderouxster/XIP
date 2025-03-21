/* $Id: cons.h,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
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



void FstBaseExport init_cons_mem(void);
HEAPptr FstBaseExport make_cons_heap(void);
void FstBaseExport free_cons_mem(void);

int list_len(CONSptr list);

CONSptr FstBaseExport cons(void *CAR, CONSptr CDR);
CONSptr FstBaseExport cons_cell(void *CAR, CONSptr CDR, HEAPptr cons_heap);

CONSptr copy_list (CONSptr orig);
CONSptr copy_cell_list (CONSptr orig, HEAPptr cons_heap);

CONSptr cons_append(CONSptr list1, CONSptr list2);
CONSptr FstBaseExport cons_cell_append(CONSptr list1, CONSptr list2, HEAPptr cons_heap);

CONSptr freeing_CDR(CONSptr cons);
CONSptr FstBaseExport freeing_CDR_cell(CONSptr cons, HEAPptr cons_heap);

void free_list(CONSptr list);
void FstBaseExport free_cell_list(CONSptr list, HEAPptr cons_heap);

void FstBaseExport free_cons(CONSptr cons);
void FstBaseExport free_cons_cell(CONSptr cons, HEAPptr cons_heap);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_CONS */
