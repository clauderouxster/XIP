/* $Id: lab-vect.c,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990-93  by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/* #include <memory.h>	*/ /* YM, 2/9/96 Harmful, need to include <cstring> instead. LK 98-09-25.*/

#ifdef MAC_OS
#include <cstring>
#endif

#include "fst/syst-dep.h"
#include "fst/lab-vect.h"
#include "fst/error.h"
#include "fst/fsm_io.h"
#include "fst/label.h"


/******************************************************
 **
 **  LAB-VECT.C
 **
 **  
 **  Lauri Karttunen, RXRC, December 1993
 **
 ******************************************************/

/* LAB_VECTOR[] is a temporary vector for holding labels.  LAB_VECTOR[0]
   is used as a counter, LAB_VECTOR[1] onwards holds the labels.  The initial
   space allocated for LAB_VECTOR is LAB_VECTOR_ALLOC_SIZE.  If it
   turns out to be insufficient, we increment in blocks of of that size
   in ADD_LABEL.  Because the first cell is used as a counter, LAB_COUNT_MAX,
   the maximum number of labels that can be stored without reallocation,
   is one less than the size of the vector.
   */


id_type *LAB_VECTOR = NULL;
int LAB_COUNT_MAX = 0;
int LAB_VECTOR_ALLOC_SIZE = 256;
int TEMP[512];

void init_lab_vector(void)     /* also to reinitialize it */
{
  if (LAB_VECTOR == NULL)
    {
      LAB_VECTOR =
        (id_type *) calloc(sizeof(id_type), LAB_VECTOR_ALLOC_SIZE);
      
      if (LAB_VECTOR == NULL)
        not_enough_memory("INIT_LAB_VECTOR");
      
      LAB_COUNT_MAX = LAB_VECTOR_ALLOC_SIZE - 1;
    }
  
  /* just reuse the same vector space as last time */
  LAB_VECTOR[0] = 0;  /* this is the counter location */
  /* indicates the number of labels actually held in the vector */
}


id_type lab_vect_len(void)
{
  if (LAB_VECTOR)
    return(LAB_VECTOR[0]);
  else
    return(0);
} /* lab_vect_len */


id_type *lab_vect_labs(void)
{
  if (LAB_VECTOR)
    return(LAB_VECTOR + 1);
  else
    return(NULL);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void free_lab_vector(void)
{
  if (LAB_VECTOR)
    {
      free((void *) LAB_VECTOR);
      LAB_VECTOR = NULL;
      LAB_COUNT_MAX = 0;
    }
}

int lab_vector_size(LAB_VECTORptr lab_vect)
{
  return(LAB_VECTOR_pos(lab_vect));
}

#endif /* SLIM_FST */

/* add an integer to the label vector */

void add_label(id_type i)
{
  register int old_alloc_size, new_alloc_size;
  register id_type *new_mem;
  
  if (LAB_VECTOR[0] == LAB_COUNT_MAX)
    {
      /* full--allocate a larger vector */
      old_alloc_size = LAB_COUNT_MAX + 1;
      new_alloc_size = old_alloc_size + LAB_VECTOR_ALLOC_SIZE;
      new_mem = (id_type *) malloc((size_t) new_alloc_size * sizeof(id_type));
      if (new_mem  == (id_type *) NULL)
        not_enough_memory("ADD_LABEL_VECTOR");
      
      /* copy old vector into new one */
      memcpy((void *) new_mem,
             (void *) LAB_VECTOR, (size_t) old_alloc_size * sizeof(id_type));
      /* free the old vector space */
      free((void *) LAB_VECTOR);
      LAB_VECTOR = new_mem;
      LAB_COUNT_MAX = new_alloc_size - 1;
    }
  /* put the new value on the end of the vector */
  LAB_VECTOR[++LAB_VECTOR[0]] = i;   /* i is passed in */
} /* add_label */



/*********************** LAB_VECTOR *********************************/

/* Replacement for the above ancient routines that use a global vector. */

LAB_VECTORptr make_lab_vector(int length)
{
  LAB_VECTORptr lab_vect;
  id_type *array;
  int i;

  check_for_negative_or_zero(length, "MAKE_LAB_VECTOR");

  lab_vect = (LAB_VECTORptr) malloc((size_t) sizeof(LAB_VECTORtype));

 if (!lab_vect)
   not_enough_memory("MAKE_LAB_VECTOR");

 array = (id_type *) malloc((size_t)(length * sizeof(id_type)));

 if (!array)
   not_enough_memory("MAKE_LAB_VECTOR");

 /* Fill initially with ID_NO_SYMBOL */

 for (i = 0; i < length; i++)
   array[i] = ID_NO_SYMBOL;

 LAB_VECTOR_array(lab_vect) = array;
 LAB_VECTOR_length(lab_vect) = length;
 LAB_VECTOR_pos(lab_vect) = 0;

 return(lab_vect);
}


void reset_lab_vector(LAB_VECTORptr lab_vect)
{
  LAB_VECTOR_pos(lab_vect) = 0;
}


void reclaim_lab_vector(LAB_VECTORptr lab_vect)
{
  if (lab_vect)
    {
      free(LAB_VECTOR_array(lab_vect));
      free(lab_vect);
    }
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void reverse_lab_vector(LAB_VECTORptr lab_vect)
{
  /* New function. See old function below.  AK */

  id_type *head, *tail, tmp;

  if (lab_vect && LAB_VECTOR_pos (lab_vect) > 1)
    {
      /* tail is set at the last occupied position */

      head = LAB_VECTOR_array (lab_vect);
      tail = head + (LAB_VECTOR_pos (lab_vect) - 1);
      
      while (tail > head)
	{
	  tmp = *head;
	  *head++ = *tail;
	  *tail-- = tmp;
	}
    }
}

void OLD_reverse_lab_vector(LAB_VECTORptr lab_vect)
{
  /* Old unmodified function. See new function above.  AK */

  id_type *head, *tail, tmp;

  if (lab_vect && LAB_VECTOR_length(lab_vect) > 1)
    {
      /* tail is set at the last occupied position */

      head = LAB_VECTOR_array(lab_vect);
      tail = head + (LAB_VECTOR_length(lab_vect) - 1);
      
      while(tail > LAB_VECTOR_array(lab_vect))
	{
	  tmp = *head;
	  *head++ = *tail;
	  *tail++ = tmp;
	}
    }
}
#endif /* SLIM_FST */

void assure_lab_vector_space(int len, LAB_VECTORptr lab_vect)
{
  int new_pos = LAB_VECTOR_pos(lab_vect) + len;
  int cur_length = LAB_VECTOR_length(lab_vect);

  if (new_pos > cur_length)
    {
      int new_length = cur_length;
      id_type *array;
      int i;

      do {
	new_length = 2 * new_length;
      } while (new_pos > new_length);

      array = (id_type *) realloc(LAB_VECTOR_array(lab_vect),
				  (size_t) (new_length * sizeof(id_type)));

      if (!array)
	not_enough_memory("ASSURE_LAB_VECT_SPACE");

      /* Initialize with ID_NO_SYMBOL */

      for (i = new_pos; i < new_length; i++)
	array[i] = ID_NO_SYMBOL;

      LAB_VECTOR_length(lab_vect) = new_length;
      LAB_VECTOR_array(lab_vect) = array;
    }
}



int lab_vector_element_at(id_type *lab, int pos, LAB_VECTORptr lab_vect)
{
  if (pos >= LAB_VECTOR_pos(lab_vect))
    {
      *lab = DUMMY_LABEL;
      return(ERR_NOLABEL);
    }
  
  *lab = LAB_VECTOR_array(lab_vect)[pos];

  return(0);
}



int set_lab_vector_element_at(id_type lab, int pos, LAB_VECTORptr lab_vect)
{
  id_type *array;
  LAB_VECTOR_pos(lab_vect) = pos;

  assure_lab_vector_space(1, lab_vect);
  array = LAB_VECTOR_array(lab_vect);
  
  array[pos++] = lab;

  LAB_VECTOR_pos(lab_vect) = pos;
  return(pos);
}



int append_to_lab_vector(id_type lab, LAB_VECTORptr lab_vect)
{
  id_type *array;
  int pos = LAB_VECTOR_pos(lab_vect);

  assure_lab_vector_space(1, lab_vect);
  array = LAB_VECTOR_array(lab_vect);
  
  array[pos++] = lab;

  LAB_VECTOR_pos(lab_vect) = pos;
  return(pos);
}



void increment_lab_vector(LAB_VECTORptr lab_vect)
{
  assure_lab_vector_space(1, lab_vect);
  LAB_VECTOR_pos(lab_vect)++;
}


int decrement_lab_vector(LAB_VECTORptr lab_vect)
{
  if (LAB_VECTOR_pos(lab_vect) > 0)
    {
      LAB_VECTOR_pos(lab_vect)--;
      return(0);
    }
  else
    return(1);
}


int subtract_from_lab_vector(id_type *lab, LAB_VECTORptr lab_vect)
{
  LAB_VECTOR_pos(lab_vect)--;
  
  if (LAB_VECTOR_pos(lab_vect) < 0)
    {
      *lab = DUMMY_LABEL;
      LAB_VECTOR_pos(lab_vect) = 0;
      return(ERR_NOLABEL);
    }

  *lab = LAB_VECTOR_array(lab_vect)[LAB_VECTOR_pos(lab_vect)];
  return(0);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void copy_lab_vector(LAB_VECTORptr to, LAB_VECTORptr from)
{
	int i;
	int len = LAB_VECTOR_pos(from);
	id_type *to_array, *from_array = LAB_VECTOR_array(from);
	
	LAB_VECTOR_pos(to) = 0;
  assure_lab_vector_space(LAB_VECTOR_pos(from), to);
	to_array = LAB_VECTOR_array(to);

	for (i = 0; i < len; i++)
		to_array[i] = from_array[i];
		
	LAB_VECTOR_pos(to) = len;
}


int concatenate_lab_vectors(LAB_VECTORptr vect1, LAB_VECTORptr vect2)
{

  int i=0, pos = LAB_VECTOR_pos(vect1), len = LAB_VECTOR_pos(vect2);
  id_type *array1, *array2 = LAB_VECTOR_array(vect2);

  assure_lab_vector_space(len, vect1);

  array1 = LAB_VECTOR_array(vect1);

  while (i < len)
    array1[pos++] = array2[i++];

  LAB_VECTOR_pos(vect1) = pos;

  return(pos);
}

int lab_vector_contains(id_type lab, LAB_VECTORptr lab_vector)
{
  /* Returns TRUE or FALSE depending on whether the lab vector contains
     the label.
  */
  int i;
  id_type *array = LAB_VECTOR_array(lab_vector);

  for (i = 0; i < LAB_VECTOR_pos(lab_vector); i++)
    if (array[i] == lab)
      return(1);
  
  return(0);
}
#endif /* SLIM_FST */

void print_lab_vector(LAB_VECTORptr lab_vect, FILE *stream, int escape_p)
{

  int i;
  id_type id;
  id_type *array = LAB_VECTOR_array(lab_vect);
  
  if (!stream)
    stream = stdout;

  for (i = 0; i < LAB_VECTOR_pos(lab_vect); i++)
    {
      id = array[i];

      if (id)
	print_label(id, stream, escape_p);
    }
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/************************* LAB_ENUMERATOR ********************/


LAB_ENUMptr create_lab_enumerator(LAB_VECTORptr lab_vect)
{
  LAB_ENUMptr lab_enum;

  if (!lab_vect)
    prog_error("create_lab_enumerator",
	       "Attempting to create an enumerator for a NULL vector.\n");

  lab_enum = (LAB_ENUMptr) malloc((size_t) sizeof(LAB_ENUMtype));

  if (!lab_enum)
    not_enough_memory("CREATE_LAB_ENUMERATOR");

  LAB_ENUM_vector(lab_enum) = lab_vect;
  LAB_ENUM_pos(lab_enum) = 0;

  return(lab_enum);
}

void free_lab_enumerator(LAB_ENUMptr lab_enum)
{
  if (lab_enum)
    free(lab_enum);
}

void reset_lab_enumerator(LAB_ENUMptr lab_enum)
{
  LAB_ENUM_pos(lab_enum) = 0;
}
#endif /* SLIM_FST */

int next_lab_vector_element(id_type *id, LAB_ENUMptr lab_enum)
{
  if (LAB_ENUM_pos(lab_enum) < LAB_VECTOR_pos(LAB_ENUM_vector(lab_enum)))
    {
      lab_vector_element_at(id, LAB_ENUM_pos(lab_enum)++, 
			    LAB_ENUM_vector(lab_enum));
      return(0);
    }
  else
    {
      *id = ID_NO_SYMBOL;
      return(1);
    }

}


/************************* LAB_VECTOR_TABLE ********************/


LAB_VECTOR_TABLEptr make_lab_vector_table(int table_length, int vector_length)
{
  LAB_VECTOR_TABLEptr table;
  LAB_VECTORptr *array;
  int i;

  check_for_negative_or_zero(table_length, "MAKE_LAB_VECTOR_TABLE");
  check_for_negative_or_zero(vector_length, "MAKE_LAB_VECTOR_TABLE");

  table = (LAB_VECTOR_TABLEptr) malloc((size_t) sizeof(LAB_VECTOR_TABLEtype));

  if (!table)
    not_enough_memory("MAKE_LAB_VECTOR_TABLE");

  array = 
    (LAB_VECTORptr *) malloc((size_t) (table_length * sizeof(LAB_VECTORptr)));

  if (!array)
    not_enough_memory("MAKE_LAB_VECTOR_TABLE");

  LAB_VECTOR_TABLE_array(table) = array;
  LAB_VECTOR_TABLE_length(table) = table_length;
  LAB_VECTOR_TABLE_vector_length(table) = vector_length;
  LAB_VECTOR_TABLE_pos(table) = 0;
 
  for (i = 0; i < table_length; i++)
    array[i] = make_lab_vector(vector_length);

  return(table);
}

void reset_lab_vector_table(LAB_VECTOR_TABLEptr table)
{
  int i, pos = LAB_VECTOR_TABLE_pos(table);
  LAB_VECTORptr *array = LAB_VECTOR_TABLE_array(table);

  for (i = 0; i < pos; i++)
    reset_lab_vector(array[i]);

  LAB_VECTOR_TABLE_pos(table) = 0;
}


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int lab_vector_at(LAB_VECTORptr *lab_vect, int pos, LAB_VECTOR_TABLEptr table)
{

  if (pos >= LAB_VECTOR_TABLE_pos(table))
    {
      *lab_vect = NULL;
      return(1);
    }
  
  *lab_vect = LAB_VECTOR_TABLE_array(table)[pos];

  return(0);

}
#endif /* SLIM_FST */

void reclaim_lab_vector_table(LAB_VECTOR_TABLEptr table)
{
	int i;
	
  if (table)
    {
    	for (i = 0; i < LAB_VECTOR_TABLE_length(table); i++)
    		reclaim_lab_vector(LAB_VECTOR_TABLE_array(table)[i]);
    		
      free(LAB_VECTOR_TABLE_array(table));
      free(table);
    }
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */


void assure_lab_vector_table_space(int len, LAB_VECTOR_TABLEptr table)
{
  int new_pos = LAB_VECTOR_TABLE_pos(table) + len;
  int cur_length = LAB_VECTOR_TABLE_length(table);
  int i;

  if (new_pos > cur_length)
    {
      int new_length = cur_length;
      int vector_length = LAB_VECTOR_TABLE_vector_length(table);
      LAB_VECTORptr *array = LAB_VECTOR_array(table);

      do {
	new_length = 2 * new_length;
      } while (new_pos > new_length);

      array = (LAB_VECTORptr *)
	realloc(array, (size_t) (new_length * sizeof(LAB_VECTORptr)));

      if (!array)
	not_enough_memory("ASSURE_LAB_VECTOR_TABLE_SPACE");

      for (i = cur_length; i < new_length; i++)
	array[i] = make_lab_vector(vector_length);
		
      LAB_VECTOR_length(table) = new_length;
      LAB_VECTOR_array(table) = array;
    }
}

void print_lab_vector_table(LAB_VECTOR_TABLEptr table, FILE *stream,
			    int escape_p)
{

  int i;
  LAB_VECTORptr *array = LAB_VECTOR_TABLE_array(table);
  
  for (i = 0; i < LAB_VECTOR_TABLE_pos(table); i++)
    {
      print_lab_vector(array[i], stream, escape_p);
      fsm_fprintf(stream, "\n");
    }

}

/************************* LAB_RING ********************/

LAB_RINGptr make_lab_ring(int size, int min_free)
{
  /* Size is the circumfrence of the ring, min_free
     is the minimal amount of free space in the
     ring that must be maintained.
  */

  LAB_RINGptr lab_ring;
  LAB_VECTORptr lab_vector;

  lab_ring = (LAB_RINGptr) malloc((size_t) sizeof(LAB_RINGtype));

 if (!lab_ring)
   not_enough_memory("MAKE_LAB_VECTOR_RING");

 /* Size cannot be smaller than the min_free space! */

 if (min_free > size)
   size = min_free;

  lab_vector = make_lab_vector(size);
  LAB_RING_lab_vector(lab_ring) = lab_vector;
  LAB_RING_start_pos(lab_ring) = LAB_RING_cur_pos(lab_ring);
  LAB_RING_min_free(lab_ring) = min_free;
  return(lab_ring);
}

void reset_lab_ring(LAB_RINGptr lab_ring)
{
  LAB_VECTORptr lab_vector = LAB_RING_lab_vector(lab_ring);
  int size = LAB_VECTOR_length(lab_vector);

  LAB_RING_start_pos(lab_ring) = 0;
  reset_lab_vector(lab_vector);
  
  /* Mark the end with ID_NO_SYMBOL to stop premature
     left context checking.
  */

  LAB_VECTOR_array(lab_vector)[size - 1] = ID_NO_SYMBOL;
}

static void copy_label_ids(id_type *target, id_type *source, int n)
{
  int i;

  for (i = 0; i < n; i++)
    target[i] = source[i];
}

void append_to_lab_ring(id_type id, LAB_RINGptr lab_ring)
{
  int cur_pos = LAB_RING_cur_pos(lab_ring) % LAB_RING_size(lab_ring);
  int next_pos;
  int count;

  set_lab_vector_element_at(id, cur_pos, LAB_RING_lab_vector(lab_ring));

  next_pos = (cur_pos + 1) % LAB_RING_size(lab_ring);

  /* Loop around if necessary. */

  if (next_pos == 0)
    LAB_RING_cur_pos(lab_ring) = 0;

  /* Check whether the space remaining is less than min_free.
     If so we double the size of the ring. The free space
     extends from the current position to the start
     position. It keeps track of the left context
     as the ring turns forward. */

  count = count_symbols_in_ring(lab_ring);

  if ((LAB_RING_size(lab_ring) - count) <= LAB_RING_min_free(lab_ring))
      {
	int old_size = LAB_RING_size(lab_ring);
	int new_size = 2 * old_size;
	int start_pos = LAB_RING_start_pos(lab_ring);
	int new_start_pos, n1, n2;
	LAB_VECTORptr old_lab_vector = LAB_RING_lab_vector(lab_ring);
	LAB_VECTORptr new_lab_vector = make_lab_vector(new_size);
	id_type *old_array = LAB_VECTOR_array(old_lab_vector);
	id_type *new_array = LAB_VECTOR_array(new_lab_vector);

       if (next_pos >= start_pos)
	 {
	   /* If (next_pos >= start_pos) symbols up to the
	      end of the current vector are the oldest part
	      of the ring and must be copied first, followed
	      by the old context from the beginning to the
	      start location, and finally the current segment
	      of the ring. */
  
	   n1 = old_size-next_pos;
	   n2 = start_pos;
	   new_start_pos = n1 + n2;

	   /* Copy old segments in order */
	   copy_label_ids(new_array, (old_array + next_pos), n1);
	   copy_label_ids((new_array+n1), old_array, n2);

	   /* Copy current segment */
	   copy_label_ids((new_array + new_start_pos),
			  (old_array+start_pos), count);

	 }
       else if (next_pos < start_pos)
	 {
	   /* If (next_pos >= start_pos) symbols between
	      next_pos and start_pos are the old context
	      that is copied first, followed by the
	      the two segments from start_pos to the
	      end of the current vector and from the
	      beginning of the current vector that
	      constitute the currently used part of
	      the ring. */

	   n1 = start_pos - next_pos;
	   n2 = old_size - start_pos;
	   new_start_pos = n1;

	   /* Copy old context. */

	   copy_label_ids(new_array, (old_array + next_pos), n1);

	   /* Copy current segments in order */

	   copy_label_ids((new_array+n1), (old_array+start_pos), n2);
	   copy_label_ids((new_array+(n1+n2)), old_array, next_pos);
	 }

	   LAB_RING_lab_vector(lab_ring) = new_lab_vector;
	   LAB_RING_start_pos(lab_ring) = new_start_pos;
	   LAB_RING_cur_pos(lab_ring) = new_start_pos+count;

	   reclaim_lab_vector(old_lab_vector);
      }
}

static int compute_ring_address(LAB_RINGptr lab_ring, int diff)
{
  int start_pos = LAB_RING_start_pos(lab_ring);
  int size = LAB_RING_size(lab_ring);
  int addr = start_pos + diff;

  if (addr >= size)
    {
      addr = addr - size;
      assert(addr < size);
    }
  else if (addr < 0)
    {
      addr = size - addr;
      assert(addr >= start_pos);
    }

  return(addr);
}


int lab_ring_element_at(id_type *lab, int addr, LAB_RINGptr lab_ring)
{

  LAB_VECTORptr lab_vector = LAB_RING_lab_vector(lab_ring);

  assert(addr < LAB_VECTOR_length(lab_vector));

  *lab = LAB_VECTOR_array(lab_vector)[addr];

  return(0);
}

void turn_lab_ring(LAB_RINGptr lab_ring, int dist)
{
  int addr = compute_ring_address(lab_ring, dist);

  /* If the ring is full we need to start from the beginning. */

  if (LAB_RING_cur_pos(lab_ring) >= LAB_RING_size(lab_ring))
    {
      LAB_RING_cur_pos(lab_ring) = 0;
      LAB_RING_start_pos(lab_ring) = 0;
    }
  else
    LAB_RING_start_pos(lab_ring) = addr;
}

int count_symbols_in_ring(LAB_RINGptr lab_ring)
{
  int size = LAB_RING_size(lab_ring);
  int start_pos = LAB_RING_start_pos(lab_ring);
  int cur_pos = LAB_RING_cur_pos(lab_ring);

  if (cur_pos >= start_pos)
    return(cur_pos - start_pos);
  else
    return((size - start_pos) + cur_pos);
}
#endif /* SLIM_FST */

void reclaim_lab_ring(LAB_RINGptr lab_ring)
{
  reclaim_lab_vector(LAB_RING_lab_vector(lab_ring));
  free(lab_ring);
}


