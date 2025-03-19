/* $Id: prop-io.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1994  by the Xerox Corporation.  All rights reserved */

#ifdef __GNUC__
static char rcsid[] __attribute__ ((unused)) = 
"$Id: prop-io.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $";
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "fst/struct.h"
#include "fst/prompt.h"
#include "fst/pretty-p.h"
#include "fst/prop-io.h"
#include "fst/fsm_io.h"
#include "fst/types.h"
#include "fst/utf8_fatstr_util.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/******************************************************
 **
 **  PROP-IO.C
 **
 **  Lauri Karttunen, RXRC 1993
 **  Herv‰ Poirier, RXRC 1994
 **  
 **  Functions for setting and adding network properties
 **  from files.
 **
 ******************************************************/

static int PROP_FILE_LINE_NUM = 0;

int prop_io_line_count(void)
{
  return(PROP_FILE_LINE_NUM);
}

void init_prop_io_line_count(void)
{
  PROP_FILE_LINE_NUM = 0;
}


void increment_prop_io_line_count(void)
{
  PROP_FILE_LINE_NUM++;
}

int add_props(NETptr net, char *file)
{
  FILE *stream;
  int err;

  if (!net)
    return(1);
     
  if ((stream = fopen(file, "r")))
    {
      init_prop_io_line_count();
      err = add_props_s(net, stream);
      fclose(stream);
      return(err);
    }
  else
    return(1);
}

int reset_props(NETptr net, char *file)
{
  FILE *stream;
  int err;
   
  if (!net)
    return(1);
     
  if ((stream = fopen(file, "r")))
    {
      init_prop_io_line_count();
      err = reset_props_s(net, stream);
      fclose(stream);
      return(err);
    }
  else
    return(1);
}

int save_props(NETptr net, char *file)
{
  FILE *stream;
  int err;
   
  if (!net)
    return(1);
     
  if ((stream = fopen(file, "w")))
    {
      err = save_props_s(net, stream);
      fclose(stream);
      return(err);
    }
  else
    return(1);
}

void print_prop(PROPptr prop, FILE *stream);

int add_props_s(NETptr net, FILE *stream)
{
  extern FILE *propio_in;
  extern FAT_STR propio_attr;
  extern OBJECTptr propio_value;
  extern int propio_eof;

  if (!(net && stream))
    return(1);

  propio_in = stream;
  propio_eof = FALSE;
  init_prop_io_line_count();

  while (!propio_eof)
    {
      if (propio_parse() == 1)
        {
	  fsm_printf("Aborting\n");
	  return(1);
        }
      else
	if (!propio_eof && (propio_attr != NULL))
	  add_prop(net, propio_attr, propio_value);
    }

  return(0);
}

int reset_props_s(NETptr net, FILE *stream)
{
  if (!(net && stream))
    return(1);

  /* WARNING: memory not freed */
  NET_properties(net) = NULL;
  add_props_s(net, stream);

  return(0);
}

void sequence_print(SEQUENCEptr, FILE *);


void print_symbol(FILE *stream, FAT_STR name)
{
  char *thin_name, *pt;

  /* the only case we need a : before the symbol is when there's no
     letter in the symbol name */
  thin_name = FST_fat_to_thin(name, NULL);
  for (pt = thin_name; *pt != '\0'; pt++)
    if (!isdigit((int) *pt))
      break;
  if (*pt == '\0')		/* they are all digits */
    fsm_fprintf(stream, ":");
  free(thin_name);

  FST_fprint_fat_string_esc(stream, name, ":,[]<>()'\" ");
}

static void byte_block_print(BYTE_BLOCKptr byte_block, FILE *stream)
{
  /* Dump contents of the block as a stream of hex characters. */

  unsigned long i, len = BYTE_BLOCK_len(byte_block);
  char *block = (char *) BYTE_BLOCK_block(byte_block);

  for (i = 0; i < len; i++)
    fsm_fprintf(stream, "%X", *block++);
}


void object_print(OBJECTptr object, FILE *stream)
{
  switch(OBJ_type(object))
    {
    case Object:
      handle_error("Illegal Object type \"Object\"",
		   "PRINT_OBJECT", OBJ_type(object));
      break;

    case Int:
      fsm_fprintf(stream, "%ld", OBJ_integer(object));
      break;

    case Character:
      fsm_fprintf(stream, "'");
      FST_fprint_fat_char_esc(stream, &OBJ_character(object), "'\n");
      fsm_fprintf(stream, "'");
      break;

    case String:
      fsm_fprintf(stream, "\"");
      FST_fprint_fat_string_esc(stream, OBJ_str(object), "\"\n");
      fsm_fprintf(stream, "\"");
      break;

    case Symbol_package:
      handle_error("Illegal Object type \"Symbol_package\"",
		   "PRINT_OBJECT", OBJ_type(object));
      break;

    case Symbol:
      print_symbol(stream, IO_SYM_name(OBJ_symbol(object)));

      break;

    case Array:
      fsm_fprintf(stream, "< ");
      sequence_print(OBJ_array(object), stream);
      fsm_fprintf(stream, " >");
      break;

    case List:
      fsm_fprintf(stream, "( ");
      sequence_print(OBJ_list(object), stream);
      fsm_fprintf(stream, " )");
      break;

    case Byte_block:
      fsm_fprintf(stream, "#");
      byte_block_print(OBJ_byte_block(object), stream);
      fsm_fprintf(stream, "#");
      break;
    default:
      handle_error("Illegal Object type",
		   "PRINT_OBJECT", OBJ_type(object));
      break;
    }
}

void sequence_print(SEQUENCEptr sequence, FILE *stream)
{
  int i;

  for (i = 0; i < SEQ_len(sequence); i++)
    {
      if (i != 0)
	fsm_fprintf(stream, " ");

      switch(SEQ_type(sequence))
        {
        case Object:
#ifdef TEST_PROPIO
	  fsm_fprintf(stream, "[ ");
#endif
	  object_print(SEQ_objects(sequence)[i], stream);
#ifdef TEST_PROPIO
	  fsm_fprintf(stream, " ]");
#endif
	  break;

        case Int:
	  fsm_fprintf(stream, "%ld", SEQ_ints(sequence)[i]);
	  break;

        case Character:
	  fsm_fprintf(stream, "'");
	  FST_fprint_fat_char_esc(stream, &SEQ_characters(sequence)[i], "'\n");
	  fsm_fprintf(stream, "'");
	  break;

        case String:
	  fsm_fprintf(stream, "\"");
	  FST_fprint_fat_string_esc(stream, SEQ_strings(sequence)[i], "\"\n");
	  fsm_fprintf(stream, "\"");
	  break;

        case Symbol_package:
#ifdef TEST_PROPIO
	  if (i == 0)
	    fsm_fprintf(stream, "$");
#endif

	  print_symbol(stream, SYM_PACK_sym(SEQ_symbol_package(sequence), i) );

	  break;

        case Symbol:
	  print_symbol(stream, IO_SYM_name(&(SEQ_symbols(sequence)[i])) );

	  break;

        case Array:
	  fsm_fprintf(stream, "< ");
	  sequence_print(SEQ_array(sequence)[i], stream);
	  fsm_fprintf(stream, " >");
	  break;

        case List:
	  fsm_fprintf(stream, "( ");
	  sequence_print(SEQ_list(sequence)[i], stream);
	  fsm_fprintf(stream, " )");
	  break;

	case Byte_block:
	  fsm_fprintf(stream, "#");
	  byte_block_print(SEQ_byte_block(sequence)[i], stream);
	  fsm_fprintf(stream, "#");
	  break;

        default:
	  handle_error("Illegal Sequence type",
		       "PRINT_SEQUENCE", SEQ_type(sequence));
	  break;
        }
    }
}

void print_prop(PROPptr prop, FILE *stream)
{
    FST_fprint_fat_string(stream, PROP_attr(prop));
    fsm_fprintf(stream, ": ");

    object_print(PROP_val(prop), stream);

    fsm_fprintf(stream, "\n");
}



int save_props_s(NETptr net, FILE *stream)
{
    PROPptr prop;
    
    if (!(net && stream))
        return(1);
    
    for (prop = NET_properties(net); prop; prop = next_prop(prop))
        print_prop(prop, stream);
    
    return(0);
}

OBJECTptr NewObject(int type)
{
    OBJECTptr object = (OBJECTptr)malloc(sizeof(OBJECTtype));

    if (object == NULL)
	not_enough_memory("NewObject");

    OBJ_type(object) = type;
    return(object);
}

SEQUENCEptr NewSequence(int type)
{
    SEQUENCEptr sequence = (SEQUENCEptr)malloc(sizeof(SEQUENCEtype));

    if (sequence == NULL)
	not_enough_memory("NewSequence");

    SEQ_type(sequence) = type;
    SEQ_len(sequence) = 0;
    return(sequence);
}

void SetSequenceLen(SEQUENCEptr sequence, int len)
{
    int size = 0;

    switch(SEQ_type(sequence))
    {
    case Object :
	size = sizeof(OBJECTptr) * len;
	break;
    case Int :
	size = sizeof(int) * len;
	break;
    case Character :
	size = sizeof(FAT_CHAR) * len;
	break;
    case String :
	size = sizeof(FAT_STR) * len;
	break;
    case Symbol :
	size = sizeof(IO_SYMBOLtype) * len;
	break;
    case Symbol_package :
	size = sizeof(FAT_STR) * len;
	break;
    case Array :
    case List :
	size = sizeof(SEQUENCEptr *) * len;
	break;
    }

    if (SEQ_type(sequence) != Symbol_package)
      {
	if (SEQ_len(sequence) == 0)
	  SEQ_objects(sequence) = (OBJECTptr *)malloc(size);
	else
	  SEQ_objects(sequence) = (OBJECTptr *)realloc(SEQ_objects(sequence),
						       size);
      }
    else
	if (SEQ_len(sequence) == 0)
	{
	    SEQ_symbol_package(sequence) =
	      (IO_SYMBOL_PACKAGEtype *)malloc(sizeof(IO_SYMBOL_PACKAGEtype));
	    SYM_PACK_sym_names(SEQ_symbol_package(sequence)) =
	      (FAT_STR *) malloc(size);
	}
	else
	    SYM_PACK_sym_names(SEQ_symbol_package(sequence)) = (FAT_STR *)
	      realloc(SYM_PACK_sym_names(SEQ_symbol_package(sequence)),
		      size);

    SEQ_len(sequence) = len;

    if (SEQ_objects(sequence) == NULL)
	not_enough_memory("SetSequenceLen");
}

int SameObjectsInSequence(SEQUENCEptr sequence)
{
    int type, i;

    if (SEQ_len(sequence) == 0)
	return(FALSE);

    type = OBJ_type(SEQ_objects(sequence)[0]);

    for (i = 1; i < SEQ_len(sequence); i++)
	if (OBJ_type(SEQ_objects(sequence)[i]) != type)
	    return(FALSE);

    return(TRUE);
}

void ReduceSequenceOfSameObjects(SEQUENCEptr sequence)
{
  SEQUENCEptr new_seq;
  OBJECTptr object;
  int type, i;

  type = OBJ_type(SEQ_objects(sequence)[0]);

  if (type == Symbol)
    {
      /* Special case when the type is symbol: now all the symbols 
	 are supposed to belong to the same package */
      type = Symbol_package;
    }

  new_seq = NewSequence(type);


  SetSequenceLen(new_seq, SEQ_len(sequence));

  for (i = 0; i < SEQ_len(sequence); i++)
    {
      object = SEQ_objects(sequence)[i];

      switch(type)
	{
	case Int :
	  SEQ_ints(new_seq)[i] = OBJ_integer(object);
	  break;

	case Character :
	  fat_charcpy(SEQ_characters(new_seq)[i], OBJ_character(object));
	  break;

	case String :
	  SEQ_strings(new_seq)[i] = OBJ_str(object);
	  break;


	case Symbol :
	  IO_SYM_name(&SEQ_symbols(new_seq)[i]) =
	    IO_SYM_name(OBJ_symbol(object));
	  free((void *)OBJ_symbol(object));
	  break; 

	case Symbol_package :
	  SYM_PACK_sym(SEQ_symbol_package(new_seq), i) =
	    IO_SYM_name(OBJ_symbol(object));
	  free((void *)OBJ_symbol(object));
	 
	  break;
	case Array :
	case List :
	  SEQ_array(new_seq)[i] = OBJ_array(object);
	  break;
	}

      free((void *)object);
    }

  free((void *)SEQ_objects(sequence));
  /* to avoid some pointer problems */
  *sequence = *new_seq;
  free((void *)new_seq);
}

#endif /* SLIM_FST */
