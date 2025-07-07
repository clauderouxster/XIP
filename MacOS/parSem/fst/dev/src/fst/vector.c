/* $Id: vector.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "fst/error.h"
#include "fst/struct.h"
#include "fst/cmp-net.h"

#include "fst/pars-tbl.h"
#include "fst/flags.h"
#include "fst/vector.h"



/******************************************************
 **
 **  VECTOR.C
 **
 **  Lauri Karttunen, XRCE, February 1999
 **
 **  New lookup routines
 **
 ******************************************************/

VECTORptr make_vector(int length)
{
  VECTORptr vector;
  void **array;

  if (length <= 0)
    handle_error("Negative or zero length", "MAKE_VECTOR", length);

  vector = (VECTORptr) malloc((size_t) sizeof(VECTORtype));

 if (!vector)
   not_enough_memory("MAKE_VECTOR");

 array = (void **) calloc(sizeof(void*), length);

 if (!array)
   not_enough_memory("MAKE_VECTOR");

 VECTOR_array(vector) = array;
 VECTOR_length(vector) = length;
 VECTOR_pos(vector) = 0;

 return(vector);
}


void free_vector(VECTORptr vector)
{
  if (vector)
    {
      free(VECTOR_array(vector));
      free(vector);
    }
}


void assure_vector_space(int len, VECTORptr vector)
{
    int new_pos = VECTOR_pos(vector) + len;
    int cur_length = VECTOR_length(vector);

    if (new_pos > cur_length)
    {
        int new_length = cur_length;
        void **array;
        int j= 0;
        
        do {
            new_length = 2 * new_length;
        } while (new_pos > new_length);

        array= VECTOR_array(vector);
        array = (void **) realloc(array,
				 (size_t) (new_length * sizeof(void *)));

        if (!array)
            not_enough_memory("ASSURE_VECTOR_SPACE");


/*         memset(array+cur_length*sizeof(void *), 0, new_length-cur_length); */
      
        VECTOR_length(vector) = new_length;
        VECTOR_array(vector) = array;

        
        /* Fill the newly allocated part of the table with NULL pointers: */
        for (j= cur_length; j< new_length; j++)
        {
            vector->array[j]= NULL;
        }
    }
}


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void initialize_vector(VECTORptr *vector, int min_length)
{
  if (*vector)
    {
      VECTOR_pos(*vector) = 0;
      if (VECTOR_length(*vector) < min_length)
	assure_vector_space(min_length, *vector);
    }
  else
    *vector =make_vector(min_length);
}
#endif /* SLIM_FST */

int vector_element_at(void **element, int pos, VECTORptr vector)
{
  if (pos >= VECTOR_pos(vector))
    {
      *element = NULL;
      return(1);
    }
  
  *element = VECTOR_array(vector)[pos];

  return(0);
}


int set_vector_element_at(void *element, int pos, VECTORptr vector)
{
  void **array;
  VECTOR_pos(vector) = pos;

  assure_vector_space(1, vector);
  array = VECTOR_array(vector);

  array[pos++] = element;

  VECTOR_pos(vector) = pos;
  return(pos);
}



int append_to_vector(void *object, VECTORptr vector)
{
  void **array;
  int pos = VECTOR_pos(vector);

  assure_vector_space(1, vector);
  array = VECTOR_array(vector);
  
  array[pos++] = object;

  VECTOR_pos(vector) = pos;
  return(pos);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void increment_vector(VECTORptr vector)
{
  assure_vector_space(1, vector);
  VECTOR_pos(vector)++;
}
#endif /* SLIM_FST */

int decrement_vector(VECTORptr vector)
{
  if (VECTOR_pos(vector) > 0)
    {
      VECTOR_pos(vector)--;
      return(0);
    }
  else
    return(1);
}


int subtract_from_vector(void **last_element, VECTORptr vector)
{
  VECTOR_pos(vector)--;
  
  if (VECTOR_pos(vector) < 0)
    {
      *last_element = NULL;
      VECTOR_pos(vector) = 0;
      return(1);
    }

  *last_element = VECTOR_array(vector)[VECTOR_pos(vector)];
  return(0);
}


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int vector_size(VECTORptr vector)
{
  return(VECTOR_pos(vector));

}

void reset_vector(VECTORptr vector)
{
  VECTOR_pos(vector) = 0;
}

int concatenate_vectors(VECTORptr vector1, VECTORptr vector2)
{

  int i=0, pos = VECTOR_pos(vector1), len = VECTOR_pos(vector2);
  void **array1, **array2 = VECTOR_array(vector2);

  assure_vector_space(len, vector1);

  array1 = VECTOR_array(vector1);

  while (i < len)
    array1[pos++] = array2[i++];

  VECTOR_pos(vector1) = pos;

  return(pos);
}


/************************* VECTOR_ENUMERATOR ***************/


VECT_ENUMptr create_vector_enumerator(VECTORptr vector)
{
  VECT_ENUMptr vect_enum;

  if (!vector)
    prog_error("create_vector_enumerator",
	       "Attempting to create an enumerator for a NULL vector.\n");

  vect_enum = (VECT_ENUMptr) malloc((size_t) sizeof(VECT_ENUMtype));

  if (!vect_enum)
    not_enough_memory("CREATE_VECTOR_ENUMERATOR");

  VECT_ENUM_vector(vect_enum) = vector;
  VECT_ENUM_pos(vect_enum) = 0;

  return(vect_enum);
}

void free_vector_enumerator(VECT_ENUMptr vect_enum)
{
  if (vect_enum)
    free(vect_enum);
}

void reset_vector_enumerator(VECT_ENUMptr vect_enum)
{
  VECT_ENUM_pos(vect_enum) = 0;
}
#endif /* SLIM_FST */

int next_vector_element(void **element, VECT_ENUMptr vect_enum)
{
  if (VECT_ENUM_pos(vect_enum) < VECTOR_pos(VECT_ENUM_vector(vect_enum)))
    {
      vector_element_at(element, VECT_ENUM_pos(vect_enum)++, 
			VECT_ENUM_vector(vect_enum));
      return(0);
    }
  else
    {
      *element = NULL;
      return(1);
    }
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/************************* VECTOR_TABLE ********************/


VECTOR_TABLEptr make_vector_table(int table_length, int vector_length)
{
  VECTOR_TABLEptr table;
  VECTORptr *array;
  int i;

  check_for_negative_or_zero(table_length, "MAKE_VECTOR_TABLE");
  check_for_negative_or_zero(vector_length, "MAKE_VECTOR_TABLE");

  table = (VECTOR_TABLEptr) malloc((size_t) sizeof(VECTOR_TABLEtype));

  if (!table)
    not_enough_memory("MAKE_VECTOR_TABLE");

  array =
    (VECTORptr *) malloc((size_t) (table_length * sizeof(VECTORptr)));

  if (!array)
    not_enough_memory("MAKE_VECTOR_TABLE");

  VECTOR_TABLE_array(table) = array;
  VECTOR_TABLE_length(table) = table_length;
  VECTOR_TABLE_vector_length(table) = vector_length;
  VECTOR_TABLE_pos(table) = 0;

  for (i = 0; i < table_length; i++)
    array[i] = make_vector(vector_length);

  return(table);
}

void reset_vector_table(VECTOR_TABLEptr table)
{
  int i, pos = VECTOR_TABLE_pos(table);
  VECTORptr *array = VECTOR_TABLE_array(table);

  for (i = 0; i < pos; i++)
    reset_vector(array[i]);

  VECTOR_TABLE_pos(table) = 0;
}


int vector_at(VECTORptr *vect, int pos, VECTOR_TABLEptr table)
{

  if (pos >= VECTOR_TABLE_pos(table))
    {
      *vect = NULL;
      return(1);
    }

  *vect = VECTOR_TABLE_array(table)[pos];

  return(0);

}

void assure_vector_table_space(int len, VECTOR_TABLEptr table)
{
  int new_pos = VECTOR_TABLE_pos(table) + len;
  int cur_length = VECTOR_TABLE_length(table);
  int i;

  if (new_pos > cur_length)
    {
      int new_length = cur_length;
      int vector_length = VECTOR_TABLE_vector_length(table);
      VECTORptr *array = VECTOR_TABLE_array(table);

      do {
        new_length = 2 * new_length;
      } while (new_pos > new_length);

      array = (VECTORptr *)
        realloc(array, (size_t) (new_length * sizeof(VECTORptr)));

      if (!array)
        not_enough_memory("ASSURE_VECTOR_TABLE_SPACE");

      for (i = cur_length; i < new_length; i++)
        array[i] = make_vector(vector_length);

      VECTOR_length(table) = new_length;
      VECTOR_array(table) = array;
    }
}

#endif /* SLIM_FST */
