/* $Id: cons.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

/******************************************************************************
 **
 **  cons.c
 **
 **	 Todd Yampol, Xerox PARC, June 1990
 **
 **  Implements lisp-like cons cells.
 **
 ******************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "fst/error.h"
#include "fst/heap.h"
#include "fst/cons.h"
#include "fst/fsm_io.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#define CONS_BLOCK_SIZE 2000



HEAPptr CONS_HEAP = NULL;

void init_cons_mem(void)
{
  if (CONS_HEAP != NULL)
    handle_error("CONS_HEAP cannot be reinitialized", "init_btree_mem", 0);

  CONS_HEAP = init_heap((int) sizeof(CONStype), CONS_BLOCK_SIZE,
			"Cons cells");
  
    if (CONS_HEAP == NULL)
    not_enough_memory("init_cons_mem");

}

HEAPptr make_cons_heap(void)
{
 return(init_heap((int) sizeof(CONStype), CONS_BLOCK_SIZE, "Cons cells"));
}


void free_cons_mem(void)
{
  /*
    static long int max_size = 0;
    long int size = heap_size(CONS_HEAP, TRUE);
    
    if (size > max_size)
    {
    max_size = size;
    fsm_fprintf(stdout, "Max size of CONS_HEAP: %ld\n", max_size);
    }
    */
  if (CONS_HEAP)
    free_heap(CONS_HEAP);
  CONS_HEAP = NULL;
}


CONSptr cons(void *CAR, CONSptr CDR)
{
  CONSptr new_cons = (CONSptr) cell_alloc(CONS_HEAP);
  
  car(new_cons) = CAR;
  cdr(new_cons) = CDR;
  
  return (new_cons);
}

CONSptr cons_cell(void *CAR, CONSptr CDR, HEAPptr cons_heap)
{
  CONSptr cell = (CONSptr) cell_alloc(cons_heap);
  
  car(cell) = CAR;
  cdr(cell) = CDR;
  
  return (cell);
}

CONSptr copy_list (CONSptr orig)
{
  if (orig == NULL)
    return (NULL);
  else
    return (cons(car(orig), copy_list(cdr(orig))));
}

CONSptr copy_cell_list (CONSptr orig, HEAPptr cons_heap)
{
  if (orig == NULL)
    return (NULL);
  else
    return (cons_cell(car(orig), copy_cell_list(cdr(orig), cons_heap),
		      cons_heap));
}

int list_len (CONSptr list)
{
  int len;
  
  for (len = 0; list != NULL; len++, list = cdr(list));
  
  return (len);
}

CONSptr cons_append(CONSptr list1, CONSptr list2)
{
  /* Appends a copy of list1 to list2 returning the combined list.
   */
  CONSptr copy, cell = list1, last = NULL;
  
  if (cell == NULL)
    list1 = list2;
  else
    {
      do {
	copy = cons(car(cell), NULL);
	if (last == NULL)
	  list1 = copy;
	else
	  cdr(last) = copy;
	last = copy;
	cell = cdr(cell);
      } while (cell != NULL);
      cdr(last) = list2;
    }
  return(list1);
}

CONSptr cons_cell_append(CONSptr list1, CONSptr list2, HEAPptr cons_heap)
{
  /* Appends a copy of list1 to list2 returning the combined list.
   */
  CONSptr copy, cell = list1, last = NULL;
  
  if (cell == NULL)
    list1 = list2;
  else
    {
      do {
	copy = cons_cell(car(cell), NULL, cons_heap);
	if (last == NULL)
	  list1 = copy;
	else
	  cdr(last) = copy;
	last = copy;
	cell = cdr(cell);
      } while (cell != NULL);
      cdr(last) = list2;
    }
  return(list1);
}

void free_cons(CONSptr cons)
{
  free_cell((void *) cons, CONS_HEAP);
}


void free_cons_cell(CONSptr cons, HEAPptr cons_heap)
{
  free_cell((void *) cons, cons_heap);
}


void free_list(CONSptr list)
{ 
  CONSptr next;
  while (list != NULL)
    {
      next = cdr(list);
      free_cell((void *) list, CONS_HEAP);
      list = next;
    }
}

void free_cell_list(CONSptr list, HEAPptr cons_heap)
{ 
  CONSptr next;
  while (list != NULL)
    {
      next = cdr(list);
      free_cell((void *) list, cons_heap);
      list = next;
    }
}

CONSptr freeing_CDR(CONSptr cons)
     /****
      *
      * Returns cdr of CONS, but places first cell of CONS 
      * on the free list.
      *
      ****/
{
  CONSptr CDR = cdr(cons);
  
  free_cell((void *) cons, CONS_HEAP);
  return(CDR);
}


CONSptr freeing_CDR_cell(CONSptr cons, HEAPptr cons_heap)
     /****
      *
      * Returns cdr of CONS, but places first cell of CONS 
      * on the free list.
      *
      ****/
{
  CONSptr CDR = cdr(cons);
  
  free_cell((void *) cons, cons_heap);
  return(CDR);
}

#endif /* SLIM_FST */
