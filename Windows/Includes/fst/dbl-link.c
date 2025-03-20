/* $Id: dbl-link.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

/******************************************************************************
 **
 **  DBL-LINK.C
 **
 **	 Todd Yampol, Xerox PARC, May 1990
 **
 **  Implements double-links (used by the minimizer).
 **
 ******************************************************************************/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "fst/types.h"
#include "fst/dbl-link.h"
#include "fst/error.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

DBLptr DBLheap = NULL;
long dblheapptr = 0, dblheapmax = 0;

int init_dbl_mem(long size)
     /********
      *
      *  Allocates memory for dbl-links.  You MUST call this function
      *  before calling make_dbl.  SIZE is the maximum number of cells.
      *  For minimizing, set SIZE to be the number of states.
      *
      *********/
{
  if (DBLheap != NULL)
    handle_error("DBLheap cannot be reinitialized", "init_dbl_mem", 0);

  DBLheap = (DBLptr) calloc((size_t) size, (size_t) sizeof(DBLtype));
  if (DBLheap == NULL)
    handle_error("couldn't allocate memory for DBL_LINKs", "init_dbl",
		 size);
  
  dblheapptr = 0;
  dblheapmax = size;
  return (1);
}

void free_dbl_mem()
{
  if (DBLheap)
    free((void *) DBLheap);
  
  dblheapptr = 0;
  dblheapmax = 0;
  DBLheap = NULL;
}

static DBLptr dblalloc(void)
{
  if (dblheapptr < dblheapmax)
    return(DBLheap + dblheapptr++);
  else
    {
      handle_error("couldn't allocate DBL_LINK", "dblalloc", dblheapptr);
      return(NULL);
    }
}


DBLptr make_dbl(STATEptr state)
{
  DBLptr new;
  
  new = dblalloc();
  DBL_state(new) = state;
  DBL_cdr(new) = NULL;
  DBL_back_link(new) = NULL;
  DBL_class(new) = NULL;
  
  return (new);
}

DBLptr dbl_splice (DBLptr prior, DBLptr link, DBLptr next)
{
  if ((DBL_cdr(link) = next))
    DBL_back_link(next) = link;
  
  if ((DBL_back_link(link) = prior))
    DBL_cdr(prior) = link;
  
  return (link);
}


int dbl_longer_p(register DBLptr lst1, register DBLptr lst2)
     /*****
      *
      * Returns TRUE if lst1 is longer than list2, FALSE otherwise.
      * (Returns FALSE if lists are the same length.)
      *
      *****/
{
  while(1)
    {
      if (lst1 == NULL)
	return (FALSE);
      if (lst2 == NULL)
	return (TRUE);
      
      lst1 = DBL_cdr(lst1);
      lst2 = DBL_cdr(lst2);
    }
}

#endif /* SLIM_FST */
