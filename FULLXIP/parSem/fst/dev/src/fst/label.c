/* $Id: label.c,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/*************************
 *
 *  label.c
 *
 *  Implements atomic and n-tuple arc labels.
 *	
 *	Convention:  identity tuples (a:a, b:b, etc.) are
 *               stored as corresponding arity 1 symbol (a, b, etc.)
 *
 ***************************/

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "fst/label.h"
#include "fst/error.h"
#include "fst/fsm_io.h"
#include "fst/types.h"
#include "fst/utf8_fatstr_util.h"

HASH_TABLEptr LABEL_TABLE = NULL;
LABEL_ID_MAPptr LABEL_MAP = NULL;

#define ATTRIBUTE_ALLOC 10     /* Initial number of attribute names */
#define VALUE_ALLOC 50         /* Initial number of value names */

static LABEL_ID_MAPptr ATTRIBUTE_MAP ;
static LABEL_ID_MAPptr VALUE_MAP ;

id_type EPSILON = (id_type) -1;          /* = 0 once initialized */
id_type OTHER = (id_type) -1;            /* = 1 once initialized */
id_type DUMMY_LABEL = (id_type) -1;      /* = 2 once initialized */
id_type BOUNDARY_SYMBOL = (id_type) -1;  /* = 3 once initialized */
id_type ALTCHAIN_SYMBOL = (id_type) -1;  /* = 4 once initialized */
id_type SPACE_SYMBOL = (id_type) -1;     /* = 5 once initialized */
id_type REPEAT_SYMBOL = (id_type) -1;    /* = 6 once initialized */
id_type DET_EPSILON = (id_type) -1;      /* = 7 once initialized */

id_type FIRST_FREE_LABEL = (id_type) -1; /* = 8 */

FAT_STR KEYWORD_PACKAGE = NULL;

#ifndef TRUE 
#define TRUE 1
#define FALSE 0
#endif


/********************************************************
 *                                                      *
 *           Comparison functions for labels            *
 *                                                      *
 *******************************************************/


/***** 
 Old atom compare fn differentiated atoms based on type, name, and package.
 New macro atom_cmp (below) now uses only type and name to identify atoms.
 If atoms arise that have the same name, but different package, 
 a warning is produced by intern_atomic_label().
 %%% TSY 11/4/92
*****/

static int tup_cmp(register int arity, register TUPLEptr t1,
		   register TUPLEptr t2)
{
  register id_type i, *labels1 = TUPLE_labels(t1), *labels2 = TUPLE_labels(t2);
  
  for (i = 0; i < arity; i++)
    if (labels1[i] != labels2[i])
      /* tuples differ */
      return (labels1[i] - labels2[i]);
  
  return (0);			/* tuples are the same */
}

int label_comparefn(void *lab1, void *lab2)
     /******
      * 
      * Comparison function for labels.
      * Returns:
      * 0 if labels are the same,
      * POSITIVE value is lab1 is greater
      * NEGATIVE value if lab2 is greater.
      *
      *     ordering for labels:
      *		 1st key:  arity		
      *		 2nd key:  arity specific:
      *	arity = 1 (atomic):  type 1st, name 2nd, package 3rd
      *	arity > 1 (tuple): by 1st field 1st, 2nd field 2nd, 3rd field 3rd, etc
      *
      *******/
{
  if (LABEL_arity((LABELptr) lab1) != LABEL_arity((LABELptr) lab2))
    return (LABEL_arity((LABELptr) lab1) - LABEL_arity((LABELptr) lab2));
  else
    {
      if (LABEL_arity((LABELptr) lab1) == 1)
	/* atomic labels */
	return (fat_strcmp(LABEL_atom((LABELptr) lab1), LABEL_atom((LABELptr) lab2)));
      else
	/* tuples */
	return (tup_cmp(LABEL_arity((LABELptr) lab1), LABEL_tuple((LABELptr) lab1),
		LABEL_tuple((LABELptr) lab2)));
    }
}


/************************************************\
 *                                                *
 *               Hash functions for labels        *
 *                                                *
 \************************************************/

uint32 hash_fatstr (void *str)
{
  register uint32 g, hash = 0;
  register FAT_STR fs = (FAT_STR) str;

  while (!(CHARSET(fs) == 0 && CHARNUM(fs) == 0))
    {
      if (CHARSET(fs))
	{
	  hash = ( hash << 4 ) + CHARSET(fs);
	  if ((g = hash & 0xf0000000))
	    { 
	      hash ^= g >> 24;
	      hash ^= g;
	    }
	}

      hash = ( hash << 4 ) + CHARNUM(fs);
      if ((g = hash & 0xf0000000))
	{ 
	  hash ^= g >> 24;
	  hash ^= g;
	}
      fs++;
    }
  return (hash);
} /* hash_fatstr */


static uint32 hash_tuple (register TUPLEptr tuple, register int arity)
{
  register int16 i, k;
  uint16 n; 
  register id_type *labels = TUPLE_labels(tuple);
  uint32 hash = ':';
  
  for (i = 0; i < arity; i++)
    {
      n = 0;
      k = 0;
      while (n < labels[i])
	{
	  n = (n << 1) | 1 ;
	  k++;
	}
      
      hash = (hash << k) + labels[i];
    }

  return (hash);
}

uint32 label_hashfn(void *label)
{
  switch(LABEL_arity((LABELptr) label))
    {
    case (1): 
      return (hash_fatstr(LABEL_atom((LABELptr) label)));
    default:
      return (hash_tuple(LABEL_tuple((LABELptr) label), LABEL_arity((LABELptr) label)));
    }
}


/************************************************\
 *                                                *
 *   Print functions for labels and tuples        *
 *                                                *
 \************************************************/

static int print_tuple(TUPLEptr tuple, int arity, FILE *stream, int escape_p)
{
  int16 i, len = 2;
  id_type *labels = TUPLE_labels(tuple);
  
  fputc('<', stream);
  for (i = 0; i <  arity; i++)
    {
      len += print_label(labels[i], stream, escape_p);
      if (i + 1 < arity)
	{
	  fputc(':', stream);
	  len++;
	}
    }
  fsm_putc('>', stream);
  return(len);
}

int print_label(id_type id, FILE *stream, int escape_p)
{
  FAT_STR name;

  switch(id)
    {
    case 0:			/* EPSILON */
      fputc('0', stream);
      return(1);
      break;
    case 1:			/* OTHER */
      fputc('?', stream);
      return(1);
      break;
    default:
      {
	LABELptr label = id_to_label(id);
	switch (LABEL_arity(label))
	  {
	  case 1:
	    if (id == BOUNDARY_SYMBOL)
	      {
		fputs(".#.", stream);
		return(3);
	      }
	    else
	      {
		name = LABEL_atom(label);
		if (escape_p && fat_strlen(name) == 1)
	/* Space, zero, newline, question mark, percent sign, tab, carriage return,
	   vertical tabulate, backspace, form feed */
		  return(FST_fprint_fat_string_esc(stream, name, " 0\n?%	\r\v\b\f"));
		else
		  return(FST_fprint_fat_string(stream, name));
	      }
	    break;
	  default:
	    /* tuple */
	    return(print_tuple(LABEL_tuple(label), LABEL_arity(label), stream,
			       escape_p));
	    break;
	  }
	break;
      }
    }
} /* print_label */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int fat_to_thin_name(char *buffer, FAT_STR name, int max, int escape_p)
{
  int len = fat_strlen(name);
   
  if (len + 1 > max)
    return(-1);
   
  if (escape_p)
    {
      fat_to_thin_esc(name, buffer, " 0\n?%	\t\r\v\b\f", FALSE);
      len = strlen(buffer);
      if (len + 1 > max)
	return(-1);
    }
  else
    fat_to_thin(name, buffer);

  return(len);
}

int utf8_fat_to_thin_name(char *buffer, FAT_STR name, int max, int escape_p)
{

  int len = fat_strlen(name);
 
  if (len + 1 > max)
    return(-1);
   
  if (escape_p)
    {
      utf8_fat_to_thin_esc(name, buffer, " 0\n?%	\t\r\v\b\f", FALSE);
      len = strlen(buffer);
      if (len + 1 > max)
	return(-1);
    }
  else
  {
    utf8_fat_to_thin(name, buffer);
	len = strlen(buffer);
  }

  return(len);
}

int char_to_buffer(char *buffer, char ch, int max)
{
  if (max <= 1)
    return(-1);
     
  *buffer++ = ch;
  *buffer = '\0';
   
  return(1);
}

static int string_to_buffer(char *buffer, char *str, int max)
{
  int len = strlen(str);
  
  if (max <= len)
    return(-1);

  while (*str)
    *buffer++ = *str++;

  return(len);
}

static int copy_atom_name(id_type id, char *buffer, int max,
		int eps_to_zero_p, int escape_p)
{
  if ((id == EPSILON) || (id == ALTCHAIN_SYMBOL))
    {
      if (eps_to_zero_p)
	return(char_to_buffer(buffer, '0', max));
      else
	return(0);
    }
  else if (id == OTHER)
    return(char_to_buffer(buffer, '?', max));
  else if (id == BOUNDARY_SYMBOL)
    return(string_to_buffer(buffer, ".#.", max));
  else
    return(fat_to_thin_name(buffer, id_to_atom_name(id), max, escape_p));
}
static int utf8_copy_atom_name(id_type id, char *buffer, int max,
		int eps_to_zero_p, int escape_p)
{
	
	if ((id == EPSILON) || (id == ALTCHAIN_SYMBOL))
    {
      if (eps_to_zero_p)
	return(char_to_buffer(buffer, '0', max));
      else
	return(0);
    }
  else if (id == OTHER)
    return(char_to_buffer(buffer, '?', max));
  else if (id == BOUNDARY_SYMBOL)
    return(string_to_buffer(buffer, ".#.", max));
  else
    return(utf8_fat_to_thin_name(buffer, id_to_atom_name(id), max, escape_p));
}


int copy_label_name(id_type id, int side, char *buffer, int max, int eps_to_zero_p, int escape_p)
{
  /* Like print label except that the label is copied to a string buffer.
     MAX is the maximum number of characters.  Returns the number of characters
     copied into the buffer or -1 in the event that the buffer would
     be exceeded.
     */

  int arity = LABEL_arity(id_to_label(id));
  if (arity == 1)
    return(copy_atom_name(id, buffer, max, eps_to_zero_p, escape_p));
  else switch(side)
    {
    case UPPER:
      return(copy_atom_name(upper_id(id), buffer, max, eps_to_zero_p, escape_p));
      break;
    case LOWER:
      return(copy_atom_name(lower_id(id), buffer, max, eps_to_zero_p, escape_p));
      break;
    case BOTH_SIDES:
      {
	int i, len, total = 0;                
	id_type *items = TUPLE_labels(LABEL_tuple(id_to_label(id)));
	  
	if ((len = char_to_buffer(buffer, '<', max)) < 0)
	  return(-1);
	 
	total += len;
	  
	for (i = 0; i < arity; i++)
	  {
	    if (i > 0)
	      {
		if ((len = char_to_buffer(buffer+total, ':', max-total)) < 0)
		  return(-1);
	    	
		total += len;
	      }

	    if ((len = copy_atom_name(items[i], buffer+total, max-total, TRUE, escape_p))
		< 0)
	      return(-1);

	    total += len;
	  }
	    
	if ((len = char_to_buffer(buffer+total, '>', max-total)) < 0)
	  return(-1);
	    
	return(total + len);
      }
      break;
    default:
      handle_error("Argument neither UPPER, LOWER, or BOTH_SIDES",
		   "COPY_LABEL_NAME", side);
      return(-1);
    }
}

int utf8_copy_label_name(id_type id, int side, char *buffer, int max, int eps_to_zero_p, int escape_p)
{
  /* Like print label except that the label is copied to a string buffer.
     MAX is the maximum number of characters.  Returns the number of characters
     copied into the buffer or -1 in the event that the buffer would
     be exceeded.
     */

  int arity = LABEL_arity(id_to_label(id));
  if (arity == 1)
    return(utf8_copy_atom_name(id, buffer, max, eps_to_zero_p, escape_p));
  else switch(side)
    {
    case UPPER:
      return(utf8_copy_atom_name(upper_id(id), buffer, max, eps_to_zero_p, escape_p));
      break;
    case LOWER:
      return(utf8_copy_atom_name(lower_id(id), buffer, max, eps_to_zero_p, escape_p));
      break;
    case BOTH_SIDES:
      {
	int i, len, total = 0;
	id_type *items = TUPLE_labels(LABEL_tuple(id_to_label(id)));
	  
	if ((len = char_to_buffer(buffer, '<', max)) < 0)
	  return(-1);
	 
	total += len;
	  
	for (i = 0; i < arity; i++)
	  {
	    if (i > 0)
	      {
		if ((len = char_to_buffer(buffer+total, ':', max-total)) < 0)
		  return(-1);
	    	
		total += len;
	      }

	    if ((len = utf8_copy_atom_name(items[i], buffer+total, max-total, TRUE, escape_p))
		< 0)
	      return(-1);

	    total += len;
	  }
	    
	if ((len = char_to_buffer(buffer+total, '>', max-total)) < 0)
	  return(-1);
	    
	return(total + len);
      }
      break;
    default:
      handle_error("Argument neither UPPER, LOWER, or BOTH_SIDES",
		   "COPY_LABEL_NAME", side);
      return(-1);
    }
}

void print_global_labels(FILE *stream)
{
  id_type id, id_max;
  
  fsm_fprintf(stream, "NOTE:  \n");
  fsm_fprintf(stream, "For atomic labels, the printing format is:\n");
  fsm_fprintf(stream, "   id{'symbol_name}(has label data) ");
  fsm_fprintf(stream, "For tuple labels, the printing format is:\n");
  fsm_fprintf(stream, "   id<upper_symbol_name:lower_symbol_name>(has label data) ");

  id_max = LIM_id_count(LABEL_MAP);
  for (id = 0; id < id_max; id++)
    { 	
      fsm_fprintf(stream, "%3d. ", id);

      if (LABEL_arity(id_to_label(id)) == 1)	
	fsm_fprintf(stream, "{'");

      print_label(id, stream, DO_ESCAPE);

      if (LABEL_arity(id_to_label(id)) == 1)	
	fsm_fprintf(stream, "'}");

      if (LABEL_data(id_to_label(id)))
	fsm_printf ("(has label data)");
      fsm_fprintf(stream, "\n");
    } /* for id */
}

/************************************************\
 *                                                *
 *           Flag diacritics                      *
 *                                                *
 \************************************************/

FAT_STR id_to_flag_attribute_name(id_type id)
{
  LABELptr lab;

  if (id < LIM_id_count(ATTRIBUTE_MAP))
    {
      lab = (LABELptr)LIM_map(ATTRIBUTE_MAP)[id];
      return(LABEL_atom(lab));
    }
  else
    return(NULL);
}

FAT_STR id_to_flag_value_name(id_type id)
{
  LABELptr lab;

  if (id < LIM_id_count(VALUE_MAP))
    {
      lab = (LABELptr)LIM_map(VALUE_MAP)[id];
      return(LABEL_atom(lab));
    }
  else
    return(NULL);
}
#endif /* SLIM_FST */

int flag_attribute_count(void)
{
  return((int)LIM_id_count(ATTRIBUTE_MAP));
}


int flag_value_count(void)
{
  return((int)LIM_id_count(VALUE_MAP));
}


static int end_of_flag_name_p(FAT_STR str)
{
  return(CHARNUM(str) == '@' && CHARNUM(str+1) == '\0' &&
	 CHARSET(str) == '\0' && CHARSET(str+1) == '\0');
}

static int flag_diacr_action(FAT_STR name)
{
  int act = 0;

  if (CHARSET(name) == '\0' && CHARNUM(name) == '@' &&
      CHARSET(name+1) == '\0' && CHARSET(name+2) == '\0')
    {
      if (CHARNUM(name+1) == 'F' && end_of_flag_name_p(name+2))
	act = FAIL_ACTION;
      else if (CHARNUM(name+2) == '.')
	switch (CHARNUM(name+1))
	  {
	  case 'C':
	    act = CLEAR_SETTING;
	    break;
	  case 'P':
	    act = POSITIVE_SETTING;
	    break;
	  case 'N':
	    act = NEGATIVE_SETTING;
	    break;
	  case 'U':
	    act = UNIFY_TEST;
	    break;
	  case 'D':
	    act = DISALLOW_TEST;
	    break;
	  case 'R':
	    act = REQUIRE_TEST;
	    break;
	  case 'I':
	    act = INSERT_SUBNET;
	    break;
	  case 'S':
	    act = SET_TO_ATTR;
	    break;
	  case 'E':
	    act = EQUAL_ATTR_TEST;
	    break;
	  default:
	    handle_error("No such action", "FLAG_DIACR_ACTION", CHARNUM(name+1));
	    break;
	  }
      else
	handle_error("No such action", "FLAG_DIACR_ACTION", CHARNUM(name+1));
    }
  return(act);
}

int intern_flag_value(id_type *id, FAT_STR name)
{
  int error;

  error= intern_symbol_name(id, name, VALUE_MAP);
  if ( (error == NO_ERROR) && (*id > 32767) )
    {
      error= ERR_ID_FLAG_VALUE_TOO_BIG;
      fsm_fprintf(stderr, "** WARNING: id of the flag value greater than 32767 ---> some flag actions may fail\n");
    }
  return(error);
}

int intern_flag_attribute(id_type *id, FAT_STR name)
{
  return(intern_symbol_name(id, name, ATTRIBUTE_MAP));
}

static int intern_flag_subnet(id_type *id, FAT_STR name)
{
  return(intern_symbol_name(id, name, LABEL_MAP));
}

static void init_flag_diacritics(void)
{

  FAT_CHAR fc;
  id_type id;

  ATTRIBUTE_MAP = make_symbol_map(ATTRIBUTE_ALLOC, "Attribute names");
  VALUE_MAP = make_symbol_map(VALUE_ALLOC, "Value names");

  /* commented out by CP: make_symbol_map() never returns a NULL pointer*/
  /*   if (!(ATTRIBUTE_MAP && VALUE_MAP)) */
  /*     return(1); */

  /* Note that we defined (in label h):
        UNDEFINED_FLAG_VALUE = 0 
        UNDEFINED_FLAG_ATTRIBUTE = 0
     */

#ifdef DEBUG
  assert(flag_attribute_count() == UNDEFINED_FLAG_ATTRIBUTE);
  assert(flag_value_count() == UNDEFINED_FLAG_VALUE);
#endif /* DEBUG */

  /* Initialize the undefined elements with empty strings as names.
     The numbering of real attributes and values starts
     with 1 rather than 0.
     */

  CHARSET(&fc) = '\0';
  CHARNUM(&fc) = '\0';

  intern_flag_attribute(&id, &fc);  /* null attribute: id should be 0 */
  intern_flag_value(&id, &fc); /* null value: id should be 0 */

} /* init_flag_diacritics */

static void free_label_map(LABEL_ID_MAPptr label_map);

static void free_flag_diacritics(void)
{
  free_label_map(ATTRIBUTE_MAP);
  ATTRIBUTE_MAP = NULL;
  free_label_map(VALUE_MAP);
  VALUE_MAP = NULL;
}

static FAT_STR extract_next_component(FAT_STR str, int error_p)
{
  /* Extracts a component of the name of a flag diacritic symbol.
     returns a pointer to the first fat character following
     the extracted component.
     */

  while (!(CHARSET(str) == '\0' && (CHARNUM(str) == '@' ||
	   CHARNUM(str) == '.' || CHARNUM(str) == '\0')))
    str++;
	  
  /*  Save the last character. Should be '@' or '.'. Then
      truncate the name temporarily.
      */

  if (CHARNUM(str) == '\0')
    {
      if (error_p == DO_REPORT_ERROR)
	handle_error("Bad flag diacritic name", "EXTRACT_NEXT_COMPONENT", 0);
      else
	return NULL;
    }
  
  return(str);
}

int flag_diacritic_name_p(FAT_STR name, int error_p)
{
  if (CHARSET(name) == '\0' && CHARNUM(name) == '@' &&
      CHARSET(name+1) == '\0')
    {
      if (CHARNUM(name+1) == 'F' && end_of_flag_name_p(name+2))
	return(TRUE);
      else if ((CHARNUM(name+1) == 'C' || CHARNUM(name+1) == 'P' ||
		CHARNUM(name+1) == 'N' || CHARNUM(name+1) == 'U' ||
		CHARNUM(name+1) == 'D' || CHARNUM(name+1) == 'R' ||
		CHARNUM(name+1) == 'S' || CHARNUM(name+1) == 'E') &&
	       CHARSET(name+2) == '\0' && CHARNUM(name+2) == '.')
	{
	  /* First part of the label is an operation. */

	  FAT_STR tail = (name + 3);
	  if ((tail = extract_next_component(tail, error_p)))
	    {
	      /* The second part is an attribute. */

	      /* If the flag action is DISALLOW_TEST or REQUIRE_TEST
		 the value component is optional. If the action is
		 CLEAR_SETTING, no value component is allowed.
		 */

	      if (!(end_of_flag_name_p(tail)))
		{
		  if (CHARNUM(name+1) == 'C')
		    {
		      if (error_p == DO_REPORT_ERROR)
			{
			  handle_error("Cannot parse the diacritic label",
				       "FLAG_DIACRITIC_NAME_P",
				       (int) CHARNUM(tail));
			  return(FALSE);
			}
		      else
			return(FALSE);
		    }

		  if ((tail = extract_next_component(tail+1, error_p)))
		    {
		      /* The third part is a value */
		  
		      if (end_of_flag_name_p(tail))
			return(TRUE);
		      else if (error_p == DO_REPORT_ERROR)
			{
			  handle_error("Cannot parse the diacritic label",
				       "FLAG_DIACRITIC_NAME_P",
				       (int) CHARNUM(tail));
			  return(FALSE);
			}
		      else
			return(FALSE);
		    }
		  else if (error_p == DO_REPORT_ERROR)
		    {
		      handle_error("Cannot parse the diacritic label",
				   "FLAG_DIACRITIC_NAME_P", (int) CHARNUM(tail));
		      return(FALSE);
		    }
		  else
		    return(FALSE);
		}
	      else if (CHARNUM(name+1) == 'C' || CHARNUM(name+1) == 'D' ||
		       CHARNUM(name+1) == 'R')
		return(TRUE);
	      else if (error_p == DO_REPORT_ERROR)
		{
		  handle_error("Cannot parse the diacritic label",
			       "FLAG_DIACRITIC_NAME_P", (int) CHARNUM(tail));
		  return(FALSE);
		}
	      else
		return(FALSE);
	    }
	  else if (error_p == DO_REPORT_ERROR)
	    {
	      handle_error("Cannot parse the diacritic label",
			   "FLAG_DIACRITIC_NAME_P", (int) CHARNUM(name));
	      return(FALSE);
	    }
	  else
	    return(FALSE);
	}
      else if (error_p == DO_REPORT_ERROR)
	{
	  handle_error("Cannot parse the diacritic label",
		       "FLAG_DIACRITIC_NAME_P", (int) CHARNUM(name));
	  return(FALSE);
	}
      else
	return(FALSE);
    }
  else
    return(FALSE);
} /* flag_diacritic_name_p */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void print_flag_diacritic(FLAG_DIACRptr flag_diacr, FILE *stream)
{
  int act;
  FAT_STR name;

  if (!flag_diacr)
    return;

  switch((act = FLAG_DIACR_action(flag_diacr)))
    {
    case CLEAR_SETTING:
      fsm_fprintf(stream, "CLEAR ");
      break;
    case POSITIVE_SETTING:
      fsm_fprintf(stream, "SET ");
      break;
    case NEGATIVE_SETTING:
      fsm_fprintf(stream, "SET ");
      break;
    case UNIFY_TEST:
      fsm_fprintf(stream, "UNIFY ");
      break;
    case DISALLOW_TEST:
      fsm_fprintf(stream, "DISALLOW ");
      break;
    case REQUIRE_TEST:
      fsm_fprintf(stream, "REQUIRE ");
      break;
    case FAIL_ACTION:
      fsm_fprintf(stream, "FAIL");
      return;
      break;
    case INSERT_SUBNET:
      fsm_fprintf(stream, "INSERT ");
      break;
    case SET_TO_ATTR:
      fsm_fprintf(stream, "SET ");
      break;
    case EQUAL_ATTR_TEST:
      fsm_fprintf(stream, "EQUALITY TEST of ");
      break;
    default:
      handle_error("Bad data", "PRINT_FLAG_DIACRITIC", act);
      break;
    }

  name = id_to_flag_attribute_name(FLAG_DIACR_attribute(flag_diacr));
#ifdef DEBUG
  fsm_fprintf(stream, "%d '", FLAG_DIACR_attribute(flag_diacr));
#else
  fsm_fprintf(stream, "feature '");
#endif /* DEBUG */
  FST_fprint_fat_string(stream, name);
  fsm_fprintf(stream, "'");

  if (FLAG_DIACR_value(flag_diacr) != 0)
    {
      if ((act == SET_TO_ATTR) || (act == EQUAL_ATTR_TEST))
	name = id_to_flag_attribute_name(FLAG_DIACR_value(flag_diacr));
      else
	name = id_to_flag_value_name(FLAG_DIACR_value(flag_diacr));

      switch(act)
	{
	case POSITIVE_SETTING:
	  fsm_fprintf(stream, " to ");
	  break;
	case NEGATIVE_SETTING:
	  fsm_fprintf(stream, " to the complement of ");
	  break;
	case UNIFY_TEST:
	case DISALLOW_TEST:
	case REQUIRE_TEST:
	  fsm_fprintf(stream, " with ");
	  break;
	case FAIL_ACTION:
	  break;
	case INSERT_SUBNET:
	  break;
	case SET_TO_ATTR:
	  fsm_fprintf(stream, " to ");
	  break;
	case EQUAL_ATTR_TEST:
	  fsm_fprintf(stream, " and ");
	  break;
	default:
	  handle_error("Unknown action or test","PRINT_FLAG_DIACRITIC",act);
	  break;
	}
#ifdef DEBUG
      fsm_fprintf(stream, "%d '", FLAG_DIACR_value(flag_diacr));
#else
      if ((act == SET_TO_ATTR) || (act == EQUAL_ATTR_TEST))
	fsm_fprintf(stream, " feature '");
      else
	fsm_fprintf(stream, " value '");
#endif /* DEBUG */
      FST_fprint_fat_string(stream, name);
      fsm_fprintf(stream, "'");

    }
}

void print_label_and_flag_diacritic(id_type id)
{
  LABELptr label = id_to_label(id);
#ifdef DEBUG
  fsm_fprintf(stdout, "%d ", id);
#endif /* DEBUG */
  print_label(id, stdout, DONT_ESCAPE);
  if (LABEL_flag(label))
    {
      fsm_fprintf(stdout, " :");
      print_flag_diacritic(LABEL_flag(label), stdout);
    }
  fsm_fprintf(stdout, "\n");
}
#endif /* SLIM_FST */

static FLAG_DIACRptr make_flag_diacritic(FAT_STR name)
{

  FAT_STR start, end;
  unsigned char temp;
  id_type id;
  FLAG_DIACRptr flag_diacr = (FLAG_DIACRptr) malloc(sizeof(FLAG_DIACRtype));

  if (!flag_diacr)
    not_enough_memory("MAKE_FLAG_DIACR");

  /* Fill the data slots */

  FLAG_DIACR_action(flag_diacr) = flag_diacr_action(name);

  /* FAIL_ACTION has no attribute and no value */

  if (FLAG_DIACR_action(flag_diacr) == FAIL_ACTION)
    {
      FLAG_DIACR_attribute(flag_diacr) = UNDEFINED_FLAG_ATTRIBUTE;
      FLAG_DIACR_value(flag_diacr) = UNDEFINED_FLAG_VALUE;
    }
  else
    {
      /* Extract the attribute part of the label and intern it */

      start = name+3;
      end = extract_next_component(start, DO_REPORT_ERROR);
      temp = CHARNUM(end);
      CHARNUM(end) = '\0';
      /* If the flag action is INSERT_SUBNET, the attribute id refers
	 to a symbol in the global LABEL_MAP, to be defined as a
	 name of a network. In all other cases
	 (CLEAR_TEST, UNIFY_TEST, DISALLOW_TEST, REQUIRE_TEST,
	 POSITIVE_SETTING, NEGATIVE_SETTING) the attribute id
	 refers to a symbol in ATTRIBUTE_MAP. */
      if (FLAG_DIACR_action(flag_diacr) == INSERT_SUBNET)
	{
	  intern_flag_subnet(&id, start);
	  FLAG_DIACR_attribute(flag_diacr) = id;
	}
      else
	{
	  intern_flag_attribute(&id, start);
	  FLAG_DIACR_attribute(flag_diacr) = id;
	}
      CHARNUM(end) = temp;

      /* Extract the value if we are not at the end of the flag diacritic
	 name. In that case the action must be either CLEAR_SETTING,
	 obligatorily, or DISALLOW_TEST or REQUIRE_TEST optionally.
	 In case of SET_TO_ATTR and EQUAL_ATTR_TEST the next item
	 is the 2nd attribute rather than a value.
	 */

      if (!(end_of_flag_name_p(end) &&
	    (FLAG_DIACR_action(flag_diacr) == CLEAR_SETTING ||
	     FLAG_DIACR_action(flag_diacr) == DISALLOW_TEST ||
	     FLAG_DIACR_action(flag_diacr) == REQUIRE_TEST)))
	{
	  start = end+1;
	  end = extract_next_component(start, DO_REPORT_ERROR);
	  temp = CHARNUM(end);
	  CHARNUM(end) = '\0';

	  if ((FLAG_DIACR_action(flag_diacr) == SET_TO_ATTR) ||
	      (FLAG_DIACR_action(flag_diacr) == EQUAL_ATTR_TEST) )
	    {
	      intern_flag_attribute(&id, start);
	      FLAG_DIACR_value(flag_diacr) = id;
	    }
	  else
	    {
	      intern_flag_value(&id, start);
	      FLAG_DIACR_value(flag_diacr) = id;
	    }

	  CHARNUM(end) = temp;
	}
      else
	{
#ifdef DEBUG_P
	  assert(FLAG_DIACR_action(flag_diacr) == CLEAR_SETTING ||
		 FLAG_DIACR_action(flag_diacr) == DISALLOW_TEST ||
		 FLAG_DIACR_action(flag_diacr) == REQUIRE_TEST);
#endif /* DEBUG */
	  FLAG_DIACR_value(flag_diacr) = 0;
	}
    }
  /*
    print_flag_diacritic(flag_diacr, stdout);
    fsm_fprintf(stdout, "\n");
    */

  return(flag_diacr);
} /*  make_flag_diacritic */

/************************************************\
 *                                                *
 *           Basic functions for labels           *
 *                                                *
 \************************************************/

LABEL_ID_MAPptr make_symbol_map(int alloc_size, char *name)
{
  LABEL_ID_MAPptr label_map =
    (LABEL_ID_MAPptr) malloc((size_t) sizeof(LABEL_ID_MAPtype));

  LIM_hash_table(label_map) = make_hash_table(LABEL_BUCKETS, name);
  LIM_id_count(label_map) = 0;
  LIM_alloc_size(label_map) = alloc_size;
  LIM_id_max(label_map) = alloc_size-1; /* LIM_id_max(label_map) = alloc_size; */
  LIM_map(label_map) =
    (LABELptr *) calloc((size_t) alloc_size, (size_t) sizeof( LIM_map(label_map)[0]));
  
  if (!(label_map && LIM_hash_table(label_map) && LIM_map(label_map)))
    not_enough_memory("MAKE_SYMBOL_MAP");

  return(label_map);
} /* make_symbol_map */


/* Freeing a LABEL structure: */
static void free_label(LABELptr label)
{
    /* Free any data strored in this label: */
  /* Decommisioned 2002-09-03 by LK.

    if (LABEL_data(label) != NULL)
        free(LABEL_data(label));
  */

    /* Free the flag diacritic info if any: */
    if (LABEL_flag(label) != NULL)
        free(LABEL_flag(label));
    
    if (LABEL_arity(label) == 1)
    {
        /* free the atom structure: */
        free(LABEL_atom(label));
    }
    else
    {
        /* free the tuple structure: */
        free(LABEL_tuple(label)->labels);
        free(LABEL_tuple(label));
    }

  free(label);
}

/* Freeing the "map" array of the LABEL_MAP: */
static void free_lim_map(LABELptr *map, size_t length)
{
    size_t l= 0;
    
    for (l=0; l< length; l++)
    {
        if (map[l] != NULL)
            free_label(map[l]);
    }
  free(map);
}


static void free_label_map(LABEL_ID_MAPptr label_map)
{
  free_lim_map(LIM_map(label_map), LIM_id_count(label_map));
  free_hash_table(LIM_hash_table(label_map));
  free((void *)label_map);
}

int init_label_table(void)
     /******
      *
      * Initializes LABEL_TABLE, EPSILON, OTHER, and KEYWORD_PACKAGE.
      * Used to be called automatically the first time intern_atomic_label()
      * or intern_tuple_label() was called - but now done on a higher level in
      * struct.c:init_fsm_memory().  If the label table
      * is smashed for any reason, be sure to set it to NULL
      * so that it is properly re-initialized.
      *
      * Also initializes data structures for flag diacricits, that is,
      * ATTRIBUTE_TABLE and VALUE_TABLE so that flag diacritic slot
      * can be filled at the same time when a label is interned.
      * Aug 8, 1997. LK
      *******/
     
{

  int error= NO_ERROR;

  if (LABEL_TABLE != NULL)
    /* don't re-initialize. */
    return(error);			

  LABEL_MAP = make_symbol_map(LABEL_ALLOC, "Symbol names");
  LABEL_TABLE = LIM_hash_table(LABEL_MAP);
  
#define Symbol 3
  /* duplicates the definition in types.h */
  
  if (KEYWORD_PACKAGE == NULL)
    KEYWORD_PACKAGE = thin_to_fat("KEYWORD");
  
  /*           Intern Special symbols

       ID     NAME             USE

       0      EPSILON          the empty string
       1      OTHER            any known and unknown symbol
       2      DUMMY_LABEL      no special meaning
       3      BOUNDARY_SYMBOL  context boundary in replacement rules
       4      ALTCHAIN_SYMBOL  pseudo epsilon
       5      SPACE_SYMBOL     symbol for blank space
       6      REPEAT_SYMBOL    repeat for infl
       7      DET_EPSILON      special empty string for bimachines

       The print names of these symbols are irrelevant except for
       *ALTCHAIN* that stands for an EPSILON in networks that would
       blow up in the determinizer if real EPSILONs were present.
  */

  error= intern_literal(&EPSILON, "**EPSILON**");
  if (error != NO_ERROR)
    return(error);

  error=intern_literal(&OTHER, "**OTHER**");
  if (error != NO_ERROR)
    return(error);

  error=intern_literal(&DUMMY_LABEL, "**DUMMY_SYMBOL**");
  if (error != NO_ERROR)
    return(error);

  error=intern_literal(&BOUNDARY_SYMBOL, "**BOUNDARY_SYMBOL**");
  if (error != NO_ERROR)
    return(error);

  error=intern_literal(&ALTCHAIN_SYMBOL, "*ALTCHAIN*");
  if (error != NO_ERROR)
    return(error);

  error=intern_literal(&SPACE_SYMBOL, SPACE_SYMBOL_CHAR);
  if (error != NO_ERROR)
    return(error);

  error=intern_literal(&REPEAT_SYMBOL, REPEAT_SYMBOL_CHAR);
  if (error != NO_ERROR)
    return(error);

  error=intern_literal(&DET_EPSILON, "_DET_EPSILON_");
  if (error != NO_ERROR)
    return(error);

  FIRST_FREE_LABEL = global_alph_size();

  init_flag_diacritics();

  return(error);
} /* init_label_table */


void free_label_table(void)
{
  free_flag_diacritics();
  free_label_map(LABEL_MAP);
  free((void *) KEYWORD_PACKAGE);
  LABEL_TABLE = NULL;
  LABEL_MAP = NULL;
  KEYWORD_PACKAGE = NULL;

  EPSILON = (id_type) -1;
  OTHER = (id_type) -1;
  DUMMY_LABEL = (id_type) -1;
  BOUNDARY_SYMBOL = (id_type) -1;
  ALTCHAIN_SYMBOL = (id_type) -1;
  SPACE_SYMBOL = (id_type) -1;
  REPEAT_SYMBOL =(id_type) -1;
  DET_EPSILON = (id_type) -1;

  FIRST_FREE_LABEL =(id_type) -1;
}


static int label_to_old_id (id_type *id, LABELptr label)
{
  /* Returns the code NO_ERROR and sets ID to the id of LABEL if it 
     has been interned in LABEL_TABLE and LABEL_MAP;
     otherwise, returns the code ERR_NO_LABEL and sets ID to ID_NO_SYMBOL.
     */
  
  void *value;
  
  if (get_hash(LABEL_TABLE, (void *) label, &value,
	       label_hashfn, label_comparefn) == -1)
    {
      *id= ID_NO_SYMBOL;
      return(ERR_NOLABEL);
    }
  else
    {
      *id= (int) value;
      return(NO_ERROR);
    }
}


static int hash_label (id_type *id, LABELptr label, LABEL_ID_MAPptr symbol_table)
{

  /* Returns an error code which is NO_ERROR if LABEL has 
     been interned in LIM_hash_table(symbol_table) and 
     LIM_map(symbol_table), and ERR_NOLABEL otherwise.
     The id of LABEL, if LABEL has already been interned,
     is assigned to ID; otherwise ID is assigned DUMMY_LABEL. 
     This function is a generalized version of label_to_old_id.
*/


  void *value;
  
  if (get_hash(LIM_hash_table(symbol_table), (void *) label, &value,
	       label_hashfn, label_comparefn) == -1)
    {
      *id= DUMMY_LABEL;
      return(ERR_NOLABEL);
    }
  else
    {
      *id= (int) value;
      return(NO_ERROR);
    }
}


int atomic_label_to_id(id_type *id, FAT_STR name)
{
  /* Constructs a temporary label out of NAME.  
     Returns NO_ERROR if the index of the matching entry in LABEL_TABLE
     exists: the index value is assiged to ID;
     and ERR_NOLABEL otherwise: ID is assigned ID_NO_SYMBOL.
     For the sake of consistency with intern_literal(),
     an empty name is treated as EPSILON. See the comment there.
     */
  LABELtype label;
  
  if (CHARNUM(name) == '\0' && CHARSET(name) == '\0')
      *id=0;

  LABEL_arity(&label) = 1;
  LABEL_atom(&label)= name;
  
  return( label_to_old_id(id, &label) );
} /* atomic_label_to_id */


int symbol_to_id(id_type *id, FAT_STR name, LABEL_ID_MAPptr symbol_table)
{
  /* Constructs a temporary label out of NAME in the KEYWORD_PACKAGE.
     Returns an error code which is NO_ERROR if there is a matching 
     entry in symbol_table, and ERR_NOLABEL otherwise.
     The index of the matching entry in symbol_table if one exists is 
     assigned to ID; if there is none, ID is assigned DUMMY_LABEL.
     This function is a generalized version of atomic_label_to_id.
     NB. No special treatment of an empty name.
     */
  LABELtype label;

  LABEL_arity(&label)=1;
  LABEL_atom(&label)=name;
 
  return(hash_label(id, &label, symbol_table));
}

static void too_many_labels(char *function_name, int id);

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/* static void intern_label(LABELptr label) */
/* { */
/*   id_type id = LABEL_id(label); */
  
/*   if (put_hash(LABEL_TABLE, (void *) label, cast_to_void(id), */
/* 	       label_hashfn, label_comparefn) == -1) */
/*     { */
/*       not_enough_memory("INTERN_LABEL"); */
/*       return(ERR_NO_SPACE_IN_HASHTABLE); */
/*     } */
  
/*   if (id >= LIM_id_max(LABEL_MAP)) */
/*     {*/ /* re-allocate memory in map for the next symbols to be interned */ 

/*       size_t new_max = id + LIM_alloc_size(LABEL_MAP); */
/*       if (new_max > MAX_LABEL) */
/* 	new_max = MAX_LABEL; */


/*      if (id <= MAX_LABEL - LIM_alloc_size(LABEL_MAP) )  */
/*        LIM_id_max(LABEL_MAP) += LIM_alloc_size(LABEL_MAP);  */
/*      else  */
/*        LIM_id_max(LABEL_MAP)= MAX_LABEL;  */

/*       LIM_map(LABEL_MAP) = */
/* 	    (LABELptr *) realloc(LIM_map(LABEL_MAP), */
/* 				 (new_max + 1) * sizeof(LIM_map(LABEL_MAP)[0])); */

/* if (LIM_map(LABEL_MAP) == NULL)      */
/* { */
/*   not_enough_memory("INTERN_LABEL");      */
/*   return(ERR_NO_SPACE_LEFT); */
/* } */

/* } */

/*      LIM_id_max(LABEL_MAP) = new_max; */
   
/*     } */

/*   LIM_map(LABEL_MAP)[id] = label; */
/*   LIM_id_count(LABEL_MAP)++; */
/* return(NO_ERROR); */

/* } */
#endif /* SLIM_FST */

static int intern_new_symbol(LABELptr label, LABEL_ID_MAPptr symbol_table)
{
  id_type id =  LIM_id_count(symbol_table);

  if (id > MAX_LABEL)
    {
      too_many_labels("intern_new_symbol", id); 
      return(ERR_MAX_LABELS_REACHED);
    }

  if (id > LIM_id_max(symbol_table))
    { /* re-allocate memory in map for the current candidate symbol 
	 and next SIZE_ALLOC labels to be interned */
      size_t new_max = id + LIM_alloc_size(symbol_table);
      if (new_max > MAX_LABEL)
	new_max = MAX_LABEL;

      LIM_map(symbol_table) =
	    (LABELptr *) realloc( LIM_map(symbol_table), (new_max + 1) * sizeof(LIM_map(symbol_table)[0] ) );
      
      if (LIM_map(symbol_table) == NULL)     
	{
	  not_enough_memory("INTERN_NEW_LABEL");     
	  return(ERR_NO_SPACE_LEFT);
	}

      LIM_id_max(symbol_table) = new_max;
    }

  /* id is a valid number: assign it to the label to be interned */
  LABEL_id(label)= id;
  
  if (put_hash(LIM_hash_table(symbol_table), (void *) label, 
	       cast_to_void(id), label_hashfn, label_comparefn) == -1)
    {
      not_enough_memory("INTERN_NEW_SYMBOL");
      return(ERR_NO_SPACE_IN_HASHTABLE);
    }

  LIM_map(symbol_table)[id] = label;
  LIM_id_count(symbol_table)++;
  return(NO_ERROR);
} /* intern_new_symbol */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */


/* static void intern_new_symbol(LABELptr label, LABEL_ID_MAPptr symbol_table) */
/* { */
/*   id_type id = LABEL_id(label); */
  
/*   if (put_hash(LIM_hash_table(symbol_table), (void *) label, cast_to_void(id), */
/* 	       label_hashfn, label_comparefn) == -1) */
/*     not_enough_memory("INTERN_NEW_SYMBOL"); */
  
/*   if (id == LIM_id_max(symbol_table)) */
/*     { */
/*       uint16 new_max = id + LIM_alloc_size(symbol_table); */
/*       LIM_map(symbol_table) = */
/* 	(LABELptr *) realloc(LIM_map(symbol_table), */
/* 			     (size_t) new_max * sizeof(LABELtype)); */
/*       LIM_id_max(symbol_table) = new_max; */
/*       if (!(LIM_map(symbol_table))) */
/* 	not_enough_memory("INTERN_NEW_SYMBOL"); */
/*     } */
/*   LIM_map(symbol_table)[id] = label; */
/*   LIM_id_count(symbol_table)++; */
/* return(NO_ERROR); */
/* } */

#endif /* SLIM_FST */

static int intern_label(LABELptr label)
{
  return( intern_new_symbol(label, LABEL_MAP) );
}

static void too_many_labels(char *function_name, int id)
{
    fsm_fprintf(stderr, "** ERROR: Too many labels. Maximum id number is %d\n",
            (int) MAX_LABEL);
    handle_error("Cannot continue", function_name, id);
}

int intern_atomic_label(id_type *id, FAT_STR name)
     /*******
      *
      * Interns an atomic label:  assignes to ID the unique index for 
      * that label in the LABEL_MAP; it only depends on the name. 
      *
      * Use intern_tuple_label() to intern a label with tuple content.
      *
      * When a new label is created, the NAME is copied. 
      *
      *******/
{
  int error; 

  if (atomic_label_to_id(id, name) == ERR_NOLABEL ) 
    {
      LABELptr new_label = (LABELptr) malloc((size_t) sizeof(LABELtype));

      LABEL_arity(new_label) = 1;
      LABEL_data(new_label) = NULL;  /* Plain vanilla label */

      LABEL_atom(new_label) = copy_fat_string(name);

      error= intern_label(new_label);
      if (error != NO_ERROR)
	{
	  *id= ID_NO_SYMBOL;
	  return(error);
	}
      else     
	{
	  if (flag_diacritic_name_p(name, DONT_REPORT_ERROR))
	    LABEL_flag(new_label) = make_flag_diacritic(name);
	  else
	    LABEL_flag(new_label) = NULL;

	  *id= LABEL_id(new_label);
	  return(NO_ERROR);
	}
    }

  return(NO_ERROR);
} /* intern_atomic_label */


int intern_symbol_name(id_type *id, FAT_STR name, LABEL_ID_MAPptr symbol_table)
     /*******
      *
      * Interns an atomic label in SYMBOL_MAP
      * Label ID only depends on the name. Package distinctions are not
      * maintained.
      * When a new label is created, the NAME is copied.
      *******/
{ 
  int error = NO_ERROR;
  
  if (symbol_to_id(id, name, symbol_table) == ERR_NOLABEL)
    {

      LABELptr new_label = (LABELptr) malloc((size_t) sizeof(LABELtype));


      LABEL_arity(new_label) = 1;
      LABEL_data(new_label) = NULL;  /* Plain vanilla label */
      LABEL_flag(new_label) = NULL;
      LABEL_atom(new_label) = copy_fat_string(name);
      
      error= intern_new_symbol(new_label, symbol_table);
      if (error == NO_ERROR)
	*id = LABEL_id(new_label);
      else
	*id= ID_NO_SYMBOL;
    }
return(error);
} /* intern_symbol_name */

static int tuple_label_to_id(id_type *id, id_type *labels, int arity)
{
  /* Constructs a temporary tuple label.  
     Returns the code NO_ERROR if the index of the matching
     entry in LABEL_TABLE exists, and the code ERR_NOLABEL otherwise.
     The index value is assigned to ID. It is ID_NO_SYMBOL if no 
     matching entry was found.
     */
  LABELtype label;
  TUPLEtype tuple;
  
  tuple.labels = labels;
  tuple.inverse = -1;
  
  label.arity = arity;
  label.content.tuple = &tuple;
  
  return(label_to_old_id(id, &label));
}

static void tuple_error(char *fn, id_type id)
{
  print_label(id, stderr, DO_ESCAPE);
  handle_error("Tuple as a tuple constituent", fn, id);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

static void flag_diacritic_warning(id_type id)
{
  fsm_fprintf(stdout, "*** Warning: label '");
  print_label(id, stdout, DONT_ESCAPE);
  fsm_fprintf(stdout, "' is illegal: flag diacritics ");
  fsm_fprintf(stdout, " on both sides of the symbol pair.\n");
}
#endif /* SLIM_FST */

static int make_new_tuple_label(id_type *tuple_id, id_type *labels, int arity)
{
  int error;
  /* Constructs a new label using LABELS. */
  
  LABELptr new_label = (LABELptr) malloc((size_t) sizeof(LABELtype));
  TUPLEptr new_tuple = (TUPLEptr) malloc((size_t) sizeof(TUPLEtype));

  LABEL_arity(new_label) = arity;
  LABEL_tuple(new_label) = new_tuple;
  LABEL_data(new_label) = NULL;
  LABEL_flag(new_label) = NULL;

  TUPLE_labels(new_tuple) = labels;
  TUPLE_inverse(new_tuple) = ID_NO_SYMBOL;
  /* ID_NO_SYMBOL in this field means that the inverse of the tuple has not been
     determined.  (Set by INVERT_TUPLE, called by INVERT_FST.)
     */
  
  error=intern_label(new_label);
  if (error== NO_ERROR)
    {
      *tuple_id = LABEL_id(new_label);
      return(NO_ERROR);
    }
  else
    {
      *tuple_id= ID_NO_SYMBOL;
      return(error);
    }
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int make_pair_label(id_type *tuple_id, id_type upper_id, id_type lower_id)
{
  /* Makes a pair label without checking for identity */
  
  /* First check if the label already exists. */
  
  static id_type pair[2];
  int error = NO_ERROR;

  pair[0] = upper_id;
  pair[1] = lower_id;
  
  
  if (tuple_label_to_id(tuple_id, pair, 2) == ERR_NOLABEL)
    /* Intern a new tuple */
    {
      id_type *labels;
      labels = (id_type *) malloc((size_t) 2 * (size_t) sizeof(id_type));
      labels[0] = upper_id;
      labels[1] = lower_id;

      /* Make sure that the components of a tuple are atomic symbols */

      if (LABEL_arity(id_to_label(upper_id)) > 1)
	tuple_error("TUPLE_ERROR", upper_id);
      
      if (LABEL_arity(id_to_label(lower_id)) > 1)
	tuple_error("TUPLE_ERROR", lower_id);

      error= make_new_tuple_label(tuple_id, labels, 2);
      if (error != NO_ERROR)
	return(error);
      else
	/* Warn the user if both components are flag diacritics. */
	if (flag_diacritic_p(upper_id) && flag_diacritic_p(lower_id))
	  flag_diacritic_warning(*tuple_id);
    }
  return(error);
}

int make_fstlabel(id_type *tuple_id, id_type upper_id, id_type lower_id)
{
  /* Compresses X:X to X */
  int error= NO_ERROR;
  
  if (upper_id == lower_id && upper_id != OTHER)
    {
      if (LABEL_arity(id_to_label(upper_id)) > 1)
	tuple_error("TUPLE_ERROR", upper_id);
      
      if (LABEL_arity(id_to_label(lower_id)) > 1)
	tuple_error("TUPLE_ERROR", lower_id);

      *tuple_id= upper_id;
    }
  else
    error= make_pair_label(tuple_id, upper_id, lower_id);

return(error);
}
#endif /* SLIM_FST */

int intern_tuple_label(id_type *tuple_id, id_type *labels, int16 arity)
     /*******
      *
      * Interns a tuple label.  
      * Returns the unique id number for that label.
      * NB: Use intern_atomic_label() to intern a label with 
      * atom content.
      *******/
{
  
  int16 i;
  int error= NO_ERROR;
  
  /*  Harmless here, but unnecessary.  Now done in
      init_fsm_memory();
      
      if (LABEL_TABLE == NULL)
      init_label_table();
      */
  
  for (i = 0; i < arity; i++)
    if (LABEL_arity(id_to_label(labels[i])) > 1)
      tuple_error("TUPLE_ERROR", labels[i]);
  
  if (tuple_label_to_id(tuple_id, labels, arity) == ERR_NOLABEL)
    {
      id_type *new_labels = (id_type *) malloc((size_t) arity * sizeof(id_type));      
      for (i = 0; i < arity; i++)
        new_labels[i] = labels[i];
      
      error = make_new_tuple_label(tuple_id, new_labels, arity);
    }
  return(error);
} /* intern_tuple_label */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int name_to_atom_id (id_type *id, FAT_STR name)
     /*****
      *
      * Assigns to ID the id number of an atomic (non-tuple) label, 
      * given the NAME of the atomic symbol.  
      * (if no label with that name exists ID is assigned DUMMY_LABEL.
      * The returned value is the error code NO_ERROR if the name 
      * exists in LABEL_TABLE, and ERR_NOLABEL otherwise.
      *
      *****/
{
  int key;
  CELLptr cell;
  LABELptr label;
  
  key = hash_fatstr(name) % LABEL_BUCKETS;
  
  for (cell = HT_bucket(LABEL_TABLE, key); cell != NULL; cell = CELL_next(cell))
    {
      label = (LABELptr) CELL_element(cell);
      if (LABEL_arity(label) == 1)
	if (fat_strcmp(name, LABEL_atom(label)) == 0)
	  {
	    *id = LABEL_id(label);
	    return (NO_ERROR);
	  }
    }

  *id= ID_NO_SYMBOL;
  return (ERR_NOLABEL);
}


void remove_escapes(char *str)
{
  while ((str = strchr(str,'%')) != NULL)
    {
      strcpy(str, str + 1);
      str++;
    }
}
#endif /* SLIM_FST */

int intern_literal(id_type *id, char *name)
{
    /* The id of an atomic label whose name is the literal
       fat string counterpart of NAME is assigned to ID.
       Does not interpret "0",
       "?", 
       "=", <<< what is it ? disregard this line 1998-3-20 TG
       or initial "%" as special.

       If the string is a null string, it is treated as EPSILON.
       The previous version produced a nasty segmentation fault
       in add_symbol_to_parse_table() when the alphabet contains
       a symbol with an empty string as its print name. Treating
       the empty string as an EPSILON solves the parsing problem
       and it is more intuitive. It is unfortunate that we introduced
       0 as the conventional name for EPSILON, the empty string
       would been a better choice.

       L.K. 09/16/96.
       */

  int error= NO_ERROR;

  if (*name == '\0')
    *id= EPSILON;
  else
    error= intern_atomic_label(id, FST_thin_to_temp_fat(name));

  return(error);
} /* intern_literal */

int utf8_intern_literal(id_type *id, char *name)
{
    /* The id of an atomic label whose name is the literal
       fat string counterpart of NAME is assigned to ID.
       Does not interpret "0",
       "?", 
       "=", <<< what is it ? disregard this line 1998-3-20 TG
       or initial "%" as special.

       If the string is a null string, it is treated as EPSILON.
       The previous version produced a nasty segmentation fault
       in add_symbol_to_parse_table() when the alphabet contains
       a symbol with an empty string as its print name. Treating
       the empty string as an EPSILON solves the parsing problem
       and it is more intuitive. It is unfortunate that we introduced
       0 as the conventional name for EPSILON, the empty string
       would been a better choice.

       L.K. 09/16/96.
       */

  int error= NO_ERROR;
  int utf8_error= NO_ERROR;

  if (*name == '\0')
    *id= EPSILON;
  else
    {
      utf8_error = !isLegalUTF8Sequence(name, (name+strlen(name)));
      error= intern_atomic_label(id, utf8_thin_to_temp_fat(name));
    }

  return(error || utf8_error);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int intern_unicode(id_type *id, char *name)
{
  int error= NO_ERROR;

  if (*name == '\0' && *(name + 1) == '\0')
    *id=EPSILON;
  else
    error= intern_atomic_label(id, (FAT_STR) name);

return(error);
}

int utf8_intern_unicode(id_type *id, char *name)
{
	/* This function appears to receive a FAT_STR and not a char *, coming from
	expand_quoted in lex-aux.c
	We should rewrite the latter to accept UTF-8 input as well.
	*/
  int error= NO_ERROR;
  char thin_buf[1024]; // Same limit as in lex-aux.c

  if (*name == '\0' && *(name + 1) == '\0')
    *id=EPSILON;
  else
	fat_to_thin((FAT_STR)name,thin_buf);
    error= intern_atomic_label(id, utf8_thin_to_temp_fat(thin_buf));

return(error);
}


void intern_int(id_type *id, int i)
{
  FAT_CHAR fs[2];
  
  CHARNUM(fs) = i & 0xff;	/* low bits */
  CHARSET(fs) = i >> 8;		/* high bits */
  CHARNUM(fs+1) = 0;
  CHARSET(fs+1) = 0;

  intern_atomic_label(id, fs);

}


int intern_symbol(id_type *id, char *name)
{
  /* Assigns to ID the id of an atomic label whose name is the 
     fat string counterpart of NAME, except that thin names 
     EPSILON, OTHER, 
           and ANY <<< what is it ? disregard this line 1998-3-12 TG
     are "0", "?", 
           and "=",  <<< what is it ? disregard this line 1998-3-12 TG
     respectively.  Strips of an initial '%'
     like the regex parser.
     */

  int error= NO_ERROR;

  if (strcmp(name, "0") == 0)
    *id =EPSILON;
  else if (strcmp(name, "?") == 0)
    *id =OTHER;
  else
    {
      if (name[0] == '%' && name[1] != '\0')
	name++;
      error= FST_intern_literal(id, name);
    }
  return(error);
}


int intern_unicode_symbol(id_type *id, FAT_STR name)
{
  /* Assigns to ID the id of an atomic label whose name is the 
     input fat string NAME, except that fat names EPSILON and OTHER,     
     are "0" and "?" respectively.  Strips the initial '%' fat char
     if any. 
     */

  int error= NO_ERROR;

  if (CHARSET(name) == '\0' && CHARNUM(name) == '\0')
    *id =EPSILON;
  else
  {
      if ((CHARSET(name) == '\0' && CHARNUM(name) == '?')
          && (CHARSET(name+1) == '\0' && CHARNUM(name+1) == '\0'))
          *id =OTHER;
      else
      { /* Strip the intial '%' fat char if any: */
          if (CHARNUM(name) ==  '%' && ((CHARSET(name+1) != '\0' || CHARNUM(name+1) != '\0')))
              name++;
          error= intern_atomic_label(id, name);
      }
  }
  return(error);
}

#endif /* SLIM_FST */

void intern_char(id_type *new_id, char c)
{
  char stringed_char[2];  /* instead of static, load each time to be sure */

  stringed_char[0] = c;
  stringed_char[1] = '\0';

  intern_literal(new_id, stringed_char);
}

id_type upper_id(id_type id)
{
  /* Returns the argument if it does not designate a tuple, otherwise
     the ID of the first coordinate
     */
  if (LABEL_arity(id_to_label(id)) == 1) 
    return(id); 
  else 
    return(fstpair_upper(id_to_label(id)));
}


id_type lower_id(id_type id)
{
  /* Returns the argument if it does not designate a tuple, otherwise
     the ID of the second coordinate
     */

  if (LABEL_arity(id_to_label(id)) == 1) 
    return(id); 
  else 
    return(fstpair_lower(id_to_label(id)));
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void clear_label_data(void)
{
  id_type id;
  
  for (id = 0; id < LIM_id_count(LABEL_MAP); id++)
      LABEL_data(id_to_label(id)) = NULL;
}

/* A tag is a multichar symbol that does not start with
   * or @.   From Mike Wilkens. */
int id_tag_p (id_type id)
{
  FAT_STR fs = LABEL_atom(id_to_label (id));

  return (CHARNUM(fs) != 0 &&
	  CHARNUM(fs) != '*' &&
	  CHARNUM(fs) != '@' &&
	  CHARNUM(fs+1) != 0);
}
#endif /* SLIM_FST */
