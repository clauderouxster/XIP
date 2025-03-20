/* $Id: hash-tbl.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/*****************************************************
 *
 * hash-tbl.c
 *
 * Todd Yampol, Xerox PARC, May 1990.
 * Lauri Karttunen, Xerox PARC, December 1991
 * Implements a general hash table.
 *
 * Create table with MAKE_HASH_TABLE(BUCKET_COUNT)
 * Install elements with PUT_HASH(TABLE, ELEMENT, VALUE, HFN, CFN)
 * Retrieve elements with GET_HASH(TABLE, ELEMENT, PVALUE, HFN, CFN)
 * 
 *
 *****************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "fst/hash-tbl.h"
#include "fst/error.h"


HASH_TABLEptr make_hash_table(size_t bucket_count, char *name)
     /******
      *
      * Creates a new hash table.  The number of buckets remains
      * the same, the number of elements grows as needed.
      *
      ******/
{
  register HASH_TABLEptr new;
  
  new = (HASH_TABLEptr) calloc((size_t) 1, (size_t) sizeof(HASH_TABLEtype));
  
  if (new)
    {
      HT_buckets(new) =
	(CELLptr *) calloc((size_t) bucket_count, (size_t) sizeof(CELLtype));

      HT_cell_heap(new) = init_heap((int) sizeof(CELLtype), bucket_count,
				    name);
    }
  
  if (new == NULL || HT_buckets(new) == NULL || HT_cell_heap(new) == NULL)
    not_enough_memory("MAKE_HASH_TABLE");
  
  HT_bucket_count(new) = bucket_count;  
  return (new);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

long int hash_table_size(HASH_TABLEptr table, int total_p)
{
  /* When TOTAL_P is TRUE, returns the total number of bytes allocated
     to, otherwise the number of bytes that are currently in use.
     */
     
  long int byte_count = heap_size(HT_cell_heap(table), total_p);
  
  byte_count += HT_bucket_count(table) * sizeof(CELLtype);
  byte_count += sizeof(HASH_TABLEtype);
  
  return(byte_count);

}

#endif /* SLIM_FST */

void free_hash_table(HASH_TABLEptr h)
{
  if (h)
    {
      free_heap(HT_cell_heap(h));
      free((void *) HT_buckets(h));
      free((void *) h);
    }
}


int put_hash (HASH_TABLEptr table, void *element, void *value,
	      uint32 (*hashfn)(void *), int (*comparefn)(void *, void *) )
     /********
      *
      * Searches for ELEMENT in TABLE, using HASHFN to determine a key.
      * COMPAREFN is used to determine if the element has been previously
      * installed. If a previously installed element is found, it is
      * replaced by VALUE.  If no previous value exists, a new cell
      * is created for VALUE and the key counter is incremented.  Returns
      *  -1 if the new cell cannot be created, 0 otherwise.
      
      * The hash key of an element is the value returned by HASHFN for that
      * element modulo the number of hash buckets. 
      *
      * COMPAREFN takes two elements, returning 0 if they are the same, 
      * a positive integer if the first is greater, and a negative integer if
      * the second is greater.
      *
      ********/
{
  uint32 key;
  int cmp;

  CELLptr cell, next, prev = NULL;
  
  key = hashfn(element) % HT_bucket_count(table);
  
 /*
  if (key < 0)
    handle_error("Shouldn't", "PUT_HASH", key);
  */
  
  for (next = HT_bucket(table, key); next != NULL; next = CELL_next(next))
    {
      cmp = comparefn(element, CELL_element(next));
      
      if (cmp == 0)                  /* found a match */
	{
	  CELL_content(next) = value; /* change the existing value */
	  return(0);
	}
      
      if (cmp < 0)                /* found insertion point */
	break;
      
      prev = next;
    }
  
  cell = (CELLptr) cell_alloc(HT_cell_heap(table));
  if (cell == NULL)
    return(-1);
  
  if (prev == NULL)
    HT_bucket(table, key) = cell;
  else
    CELL_next(prev) = cell;

  CELL_next(cell) = next;
  CELL_element(cell) = element;
  CELL_content(cell) = value;
  
  return(0);
}



int get_hash(HASH_TABLEptr table, void *element, void **pvalue,
	     uint32 (*hashfn)(void *), int(*comparefn)(void *, void *) )
     /********
      *
      * Searches for ELEMENT in TABLE, using HASHFN to determine a key.
      * COMPAREFN is used to determine if the element has been previously
      * installed.  PVALUE is set to point to the previously installed value
      * or to NULL, if no value has been installed. Returns 0 when PVALUE
      * is set from the table, -1 means the element was not found, that is,
      * NULL means undefined rather than a null value.
      
      * The hash key of an element is the value returned by HASHFN for that
      * element modulo the number of hash buckets. 
      *
      * COMPAREFN takes two elements, returning 0 if they are the same, 
      * a positive integer if the first is greater, and a negative integer if
      * the second is greater.
      *
      ********/
{
  uint32 key;
  int cmp;

  CELLptr next, prev = NULL;
  
  key = hashfn(element) % HT_bucket_count(table);
  
  /*
  if (key < 0)
    handle_error("Shouldn't", "GET_HASH", key);
  */
  for (next = HT_bucket(table, key); next != NULL; next = CELL_next(next))
    {
      cmp = comparefn(element, CELL_element(next));
      
      if (cmp == 0)                /* found a match */
	{
	  *pvalue = CELL_content(next);
	  return(0);    /* return the existing value */
	}
      
      if (cmp < 0)                /* not found */
	break;
      
      prev = next;
    }
  *pvalue = NULL;
  return(-1);
}


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int del_hash(HASH_TABLEptr table, void *element, uint32 (*hashfn)(void *),
	     int (*comparefn)(void *, void *) )
/********
 *
 * Searches for ELEMENT in TABLE, using HASHFN to determine a key.
 * COMPAREFN is used to determine if the element has been previously
 * installed.  If a cell containing the value is found it is
 * deleted from the table.  Returns 0 if the cell for ELEMENT was
 * found and eliminated, -1 if it was not found.
 *
 ********/
{
  uint32 key;
  int cmp;

  CELLptr next, prev = NULL;
  
  key = hashfn(element) % HT_bucket_count(table);
  
  /*
  if (key < 0)
    handle_error("Shouldn't", "DEL_HASH", key);
  */
    
  for (next = HT_bucket(table, key); next != NULL; next = CELL_next(next))
    {
      cmp = comparefn(element, CELL_element(next));
      
      if (cmp == 0)                /* found a match */
	{
	  if (prev != NULL)
	    CELL_next(prev) = CELL_next(next);
	  else
	    HT_bucket(table, key) = CELL_next(next);
	  free_cell((void *) next, HT_cell_heap(table));
	  return(0);
	}
      
      if (cmp < 0)                /* not found */
	break;
      
      prev = next;
    }
  
  return(-1);
}

int update_hash(HASH_TABLEptr table, void *element, void **pvalue,
	     uint32 hashfn(void *), int comparefn(void *, void *),
		void *combinefn(void *, void *) )

     /* Searches for ELEMENT in TABLE, using HASHFN to determine a key.
      * COMPAREFN is used to determine if the element has been previously
      * installed.  If there was a previous value, the two values are
      * combined using COMBINEFN(old, *PVALUE) and the result of that is 
      * stored in the hash table.  PVALUE is then set to point to the new
      * combined value installed in the table.  If no value was previously
      * stored, then a value is added, as per PUT_HASH.
      * Not that combinefn _isn't called_ in this case.
      * Returns 0 when ELEMENT already had a value stored in the table,
      * 1 when a new element has been added,
      * and -1 means that a new element could not be added.
      * The hash key of an element is the value returned by HASHFN for that
      * element modulo the number of hash buckets. 
      *
      * COMPAREFN takes two elements, returning 0 if they are the same, 
      * a positive integer if the first is greater, and a negative integer if
      * the second is greater.
      */

{
  uint32 key;
  int cmp;

  CELLptr next, prev, cell;

  key = hashfn(element) % HT_bucket_count(table);
  
  /*
  if (key < 0)
    handle_error("Shouldn't", "GET_HASH", key);
  */

  prev = NULL;
  for (next = HT_bucket(table, key); next; next = CELL_next(next))
    {
      cmp = comparefn(element, CELL_element(next));
      
      if (cmp == 0)                /* found a match */
	{
	  CELL_content(next) = combinefn(CELL_content(next), *pvalue);
	  *pvalue = CELL_content(next);
	  return(0);    /* existing value */
	}
      
      if (cmp < 0)                /* not found */
	break;
      
      prev = next;
    }

  cell = (CELLptr) cell_alloc(HT_cell_heap(table));
  if (cell == NULL)
    return(-1);
  
  if (prev == NULL)
    HT_bucket(table, key) = cell;
  else
    CELL_next(prev) = cell;

  CELL_next(cell) = next;
  CELL_element(cell) = element;
  CELL_content(cell) = *pvalue;
  
  return 1;
}

void map_hash(HASH_TABLEptr table, int (*map_fn)(void *, void *))
{
  /********
   *
   * Maps through the hash table and applies MAP_FN to each <element, value>
   * pair.
   *
   ********/

  int i, bucket_count = HT_bucket_count(table);
  CELLptr cell;

  for (i = 0; i < bucket_count; i++)
    for (cell = HT_bucket(table, i); cell != NULL; cell = CELL_next(cell))
      map_fn(CELL_element(cell), CELL_content(cell));
}

#endif /* SLIM_FST */
