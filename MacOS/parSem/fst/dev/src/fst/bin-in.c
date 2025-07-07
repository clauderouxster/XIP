/* $Id: bin-in.c,v 1.1 2005/10/10 12:52:27 grondeau Exp $ */
/* Copyright (c) 1993-94 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <errno.h>

#if defined _WINDOWS
#include <windows.h>
#elif !defined UNDER_CE
#include <sys/mman.h>
#endif

#include "fst/bin-in.h"
#include "fst/utf8_fatstr_util.h"
#include "fst/arc-optimization.h"

#ifndef FST_SLIM /* Only a minimal set of functions is
                    compiled when FST_SLIM is defined. */
#include "fst/struct.h"
#include "fst/fsm_io.h"
#include "fst/twol-net.h"
#include "fst/machine.h"
#include "fst/fsm_io.h"
#endif /* FST_SLIM */

#ifdef MAC--OS
typedef char* caddr_t;
#endif

/******************************************************
 **
 **  BIN-IN.C
 **
 **  Lauri Karttunen, RXRC, December 1993
 **
 **  Binary input functions for FSMs.
 **
 ******************************************************/

/***************************
 *                         *
 *   global declarations   *
 *                         *
 ***************************/

/* whether to print status bar when reading or not */
static int DISPLAY_READ_STATUS_BAR = 1;/* default on */

static long STATUS_BAR_INCREMENT = 0;

static STACKptr STATE_STACK;

static unsigned long ARC_COUNT;

static STATEptr *STATE_VECTOR, CUR_STATE;

static fst_byte CUR_BYTE;

#ifdef SMALL_FSM
static int ALTCHAIN_P;     
#endif

#if defined _WINDOWS || defined UNDER_CE
/* Keeps the current fst filename for windows mmap functions */
static char* current_filename = NULL;
#endif

int MEMORY_MAP= 1; /* controls the use of mmap() for compacted net files. */

/* Default loading function:
   its signature is aligned with the restore_func prototype. */
static NETptr load_unsupported_network_format(char* filename,
                                              fst_byte* cur_byte,
                                              long int num_states,
                                              long int num_arcs,
                                              NETptr net,
                                              FILE *stream,
                                              unsigned long *byte_countp,
                                              int file_type)
{
    fsm_fprintf(stderr, "Unsupported file format: ");
    fsm_fprintf(stderr, " cannot load this network.\n");
    return(NULL);
}

#ifdef WORLD
void init_restore_functions()
{
    /* Loading of Karttunen compacted networks:
       this format is not supported by the licensable version of the lib. */
    restore_functions[TypeRestoreCompact]= load_unsupported_network_format;

    /* Loading of virtual networks:
       this format is not supported by the licensable version of the lib. */
    restore_functions[TypeRestoreVirtual]= load_unsupported_network_format;

    /* Loading of standard networks: */
    restore_functions[TypeRestoreStandard]= get_arcs_and_states;

    /* Loading of Kaplan compressed networks:
       this format is not supported by the licensable version of the lib. */
    restore_functions[TypeRestoreCompress]= load_unsupported_network_format;
    
} /* init_restore_functions */
#else
void init_restore_functions()
{
    /* Loading of compacted networks: */
    restore_functions[TypeRestoreCompact]= get_arc_block;

    /* Loading of standard networks: */
    restore_functions[TypeRestoreStandard]= get_arcs_and_states;

    /* Loading of virtual networks: */
    restore_functions[TypeRestoreVirtual]= load_unsupported_network_format;

    /* Loading of Kaplan compressed networks: */
    restore_functions[TypeRestoreCompress]= load_unsupported_network_format;
    
} /* init_restore_functions */
#endif /* WORLD */

/************************************
 *                                  *
 *   binary i/o utilities: reading  *
 *                                  *
 ************************************/

inline
  static long byte_to_number (fst_byte b, FILE *stream, unsigned long *byte_countp)
{
  long int num = 0;
  
  num = b & (unsigned char) first_number_mask;
  while (continue_p(b))
    {
      b = next_byte(stream, byte_countp);
      num = (num << 7) | (b & more_data_mask);
    }
  
  return (num);
}


long get_number(FILE *stream, unsigned long *byte_countp)
     /*****
      *
      * Reads a variable length number from STREAM.
      *
      *****/
{
  fst_byte b;
  
  b = next_byte(stream, byte_countp);
  
  if (number_p(b))
    return (byte_to_number(b, stream, byte_countp));
  else
    return (-1 * byte_to_number(b, stream, byte_countp));
}


static int byte_to_label(fst_byte b, label *l, FILE *stream, unsigned long *byte_countp)
{
  if (label_p(b))
    {
      l->final = final_p(b);
      l->last_arc = last_arc_p(b);
      l->code = b & 15;
      while (continue_p(b))
        {
          b = next_byte(stream, byte_countp);
          l->code = (l->code << 7) | (b & more_data_mask);
        }
      /* success */
      return (1);
    }
  else
    /* failure */
    return (0);
} /* byte_to_label */

static long int decrypt_number(long int num, unsigned long noise)
{
  int offset = noise & 127;

  if (offset < 64)
    return(num - offset);
  else
    return(num + offset);
}

static FAT_STR get_fat_str(FILE *stream, unsigned long *byte_countp)
{
  FAT_STR f;
  long int len, i;
  
  len = get_number(stream, byte_countp);
  f = (FAT_STR) calloc((size_t) len + 1, (size_t) sizeof(FAT_CHAR));
  
  if (f == NULL)
    not_enough_memory("GET_FAT_STR");
  
  for (i = 0; i < len; i++)
    {
      CHARSET(f + i) = next_byte(stream, byte_countp);
      CHARNUM(f + i) = next_byte(stream, byte_countp);
    }
  CHARSET(f + i) = 0;
  CHARNUM(f + i) = 0;
  
  return (f);
}


static FAT_STR get_encrypted_fat_str(FILE *stream, unsigned long *byte_countp)
{
  FAT_STR f;
  long int len, i;
  uint16 code;
  int noise = *byte_countp;

  len = decrypt_number(get_number(stream, byte_countp), noise);
  f = (FAT_STR) calloc((size_t) len + 1, (size_t) sizeof(FAT_CHAR));
  
  if (f == NULL)
    not_enough_memory("GET_ENCRYPTED_FAT_STR");
  
  for (i = 0; i < len; i++)
    {
      noise = *byte_countp;
      code = (uint16) decrypt_number(get_number(stream, byte_countp), noise);  
      CHARSET(f + i) = code >> 8;
      CHARNUM(f + i) = code & 255 ;
    }
  CHARSET(f + i) = 0;
  CHARNUM(f + i) = 0;
  
  return (f);
}


static IO_SYMBOLptr get_io_symbol(FILE *stream, unsigned long *byte_countp,
				  int file_type)
{
  /* This routine is obsolete, not used since the days of
     OLD_STANDARD_FILE_TYPE but we update it anyhow just in case...
     2002-05-21 LK */

  IO_SYMBOLptr s;
  
  s = (IO_SYMBOLptr) calloc((size_t) 1, (size_t) sizeof(IO_SYMBOLtype));

  /* Read and disregard obsolete package name. */
  if (file_type == OLD_STANDARD_FILE_TYPE)
    get_fat_str(stream, byte_countp);

  if (file_type == NEW_STANDARD_FILE_TYPE ||
      file_type == NEW_COMPACTED_FILE_TYPE)
    IO_SYM_name(s) = get_encrypted_fat_str(stream, byte_countp);
  else
    IO_SYM_name(s) = get_fat_str(stream, byte_countp);

  if (s == NULL)
    not_enough_memory("GET_IO_SYMBOL");
  
  return (s);
}

static BYTE_BLOCKptr get_byte_block(FILE *stream, unsigned long *byte_countp)
{
  BYTE_BLOCKptr byte_block =
    (BYTE_BLOCKptr) calloc((size_t) 1, (size_t) sizeof(BYTE_BLOCKtype));
  void *block;
  unsigned long len;

  if (byte_block == NULL)
    not_enough_memory("GET_BYTE_BLOCK");
     
  len = get_number(stream, byte_countp);
   
  if (!(block = (void *) malloc((size_t) len)))
    not_enough_memory("GET_BYTE_BLOCK");
     
  BYTE_BLOCK_block(byte_block) = block;
  BYTE_BLOCK_len(byte_block) = fread(block, (size_t) 1, (size_t) len, stream);
   
  if (BYTE_BLOCK_len(byte_block) != len)
    handle_warning("Read error", "GET_BYTE_BLOCK",
		   BYTE_BLOCK_len(byte_block));
   
  *byte_countp += BYTE_BLOCK_len(byte_block);
   
  return(byte_block);
}

static SEQUENCEptr get_sequence(FILE *stream, unsigned long *byte_countp,
				int file_type);


static OBJECTptr get_object(FILE *stream, unsigned long *byte_countp,
			    int file_type)
{
  OBJECTptr obj;
  
  obj = (OBJECTptr) calloc((size_t) 1, (size_t) sizeof(OBJECTtype));
  
  if (obj == NULL)
    not_enough_memory("GET_SEQUENCE");
  
  OBJ_type(obj) = get_number(stream, byte_countp);
  
  switch (OBJ_type(obj))
    {
      case(Int):                /* type 0 */
        OBJ_integer(obj) = get_number(stream, byte_countp);
      break;
      case(Character):          /* type 1 */
        CHARSET(&OBJ_character(obj)) =
        	next_byte(stream, byte_countp);
        CHARNUM(&OBJ_character(obj)) =
        next_byte(stream, byte_countp);
      break;
      case(String):             /* type 2 */
	if (file_type == NEW_STANDARD_FILE_TYPE ||
	    file_type == NEW_COMPACTED_FILE_TYPE)
	  OBJ_str(obj) = get_encrypted_fat_str(stream, byte_countp);
	else
	  OBJ_str(obj) = get_fat_str(stream, byte_countp);
      break;
      case(Symbol):             /* type 3 */
        OBJ_symbol(obj) = get_io_symbol(stream, byte_countp, file_type);
      break;
      case(Array):              /* type 4 */
        OBJ_array(obj) = get_sequence(stream, byte_countp, file_type);
      break;
      case(List):               /* type 5 */
        OBJ_list(obj) = get_sequence(stream, byte_countp, file_type);
      break;
      case(Byte_block):         /* type 6 */
        OBJ_byte_block(obj) = get_byte_block(stream, byte_countp);
      break;
    default:
      handle_error("Illegal object type", "get_object", OBJ_type(obj));
      break;
    }
  return (obj);
}


static SEQUENCEptr get_sequence(FILE *stream, unsigned long *byte_countp,
				int file_type)
{
  SEQUENCEptr s;
  int i;
  
  s = (SEQUENCEptr) calloc((size_t) 1, (size_t) sizeof(SEQUENCEtype));
  
  if (s == NULL)
    not_enough_memory("GET_SEQUENCE");
  
  SEQ_len(s) = get_number(stream, byte_countp);
  SEQ_type(s) = get_number(stream, byte_countp);
  
  if (SEQ_len(s) == 0)
    return(s);
  
  switch (SEQ_type(s))
    /* allocate memory for sequence */
    {
    case (Object):
      /* type -1 */
      SEQ_objects(s) = (OBJECTptr *) calloc ((size_t) SEQ_len(s),
                                             (size_t) sizeof(OBJECTptr));
      if (SEQ_objects(s) == NULL)
        not_enough_memory("GET_SEQUENCE");
      break;
    case (Int):
      /* type 0 */
      SEQ_ints(s) = (long int *) calloc((size_t) SEQ_len(s),
                                        (size_t) sizeof(long int));
      if (SEQ_ints(s) == NULL)
        not_enough_memory("GET_SEQUENCE");
      break;
    case (Character):
      /* type 1 */
      SEQ_characters(s) = (FAT_CHAR *) calloc((size_t) SEQ_len(s),
                                              (size_t) sizeof(FAT_CHAR));
      if (SEQ_characters(s) == NULL)
        not_enough_memory("GET_SEQUENCE");
      break;
    case (String):
      /* type 2 */
      SEQ_strings(s) = (FAT_STR *) calloc((size_t) SEQ_len(s),
                                          (size_t) sizeof(FAT_STR));
      if (SEQ_strings(s) == NULL)
        not_enough_memory("GET_SEQUENCE");
      
      break;
    case (Symbol_package):
      /* type -3 */
      SEQ_symbol_package(s) =           
        (IO_SYMBOL_PACKAGEptr) calloc((size_t) 1,
                                      (size_t) sizeof (IO_SYMBOL_PACKAGEtype));
      if (SEQ_symbol_package(s) == NULL)
        not_enough_memory("GET_SEQUENCE");
      
      /* Read and discard obsolete package name */
  
      if (file_type == OLD_STANDARD_FILE_TYPE)
      {
	FAT_STR str= get_fat_str(stream, byte_countp);
        free(str);
      }
      
      SYM_PACK_sym_names(SEQ_symbol_package(s)) = 
        (FAT_STR *) calloc ((size_t) SEQ_len(s), (size_t) sizeof(FAT_STR));
      if (SYM_PACK_sym_names(SEQ_symbol_package(s)) == NULL)
        not_enough_memory("GET_SEQUENCE");
      break;
    case (Symbol):
      /* type 3 */
      SEQ_symbols(s) =  (IO_SYMBOLptr) calloc((size_t) SEQ_len(s),
                                              (size_t) sizeof(IO_SYMBOLtype));
      if (SEQ_symbols(s) == NULL)
        not_enough_memory("GET_SEQUENCE");
      break;
    case (Array):
      /* type 4 */
      SEQ_array(s) = (SEQUENCEptr *) calloc((size_t) SEQ_len(s),
                                            (size_t) sizeof(SEQUENCEptr));
      if (SEQ_array(s) == NULL)
        not_enough_memory("GET_SEQUENCE");
      break;
    case (List):
      /* type 5 */
      SEQ_list(s) = (SEQUENCEptr *) calloc((size_t) SEQ_len(s),
                                           (size_t) sizeof(SEQUENCEptr));
      if (SEQ_list(s) == NULL)
        not_enough_memory("GET_SEQUENCE");
      break;
    case (Byte_block):
      /* type 6 */
      SEQ_byte_block(s) =
	(BYTE_BLOCKptr *) calloc((size_t) SEQ_len(s),
				 (size_t) sizeof(BYTE_BLOCKptr));
      if (SEQ_byte_block(s) == NULL)
        not_enough_memory("GET_SEQUENCE");
      break;
    default:
      handle_error("Illegal sequence type", "get_sequence", SEQ_type(s));
      return (0);
    }
  for (i = 0; i < SEQ_len(s); i++) /* fill sequence with data */
    {
      switch(SEQ_type(s))
        {
        case (Object):
          /* type -1 */
          SEQ_objects(s)[i] = get_object(stream, byte_countp, file_type);
          break;
        case (Int):
          /* type 0 */
          SEQ_ints(s)[i] = get_number(stream, byte_countp);
          break;
        case (Character):
          /* type 1 */
          CHARSET(&SEQ_characters(s)[i]) =
            next_byte(stream, byte_countp);
          CHARNUM(&SEQ_characters(s)[i]) =
            next_byte(stream, byte_countp);
          break;
        case (String):
          /* type 2 */
	  if (file_type == NEW_STANDARD_FILE_TYPE ||
	      file_type == NEW_COMPACTED_FILE_TYPE)
	    SEQ_strings(s)[i] = get_encrypted_fat_str(stream, byte_countp);
	  else
	    SEQ_strings(s)[i] = get_fat_str(stream, byte_countp);
          break;
        case (Symbol_package):
          /* type -3 */
	if (file_type == NEW_STANDARD_FILE_TYPE ||
	    file_type == NEW_COMPACTED_FILE_TYPE)
          SYM_PACK_sym(SEQ_symbol_package(s), i) =
	    get_encrypted_fat_str(stream, byte_countp);
	else
          SYM_PACK_sym(SEQ_symbol_package(s), i) =
	    get_fat_str(stream, byte_countp);
          break;
        case (Symbol):
          /* type 3 */

	  /* Read and disregard obsolete package name */
	  if (file_type == OLD_STANDARD_FILE_TYPE)
	    get_fat_str(stream, byte_countp);

	if (file_type == NEW_STANDARD_FILE_TYPE ||
	    file_type == NEW_COMPACTED_FILE_TYPE)
          IO_SYM_name(SEQ_symbols(s) + i) =
	    get_encrypted_fat_str(stream, byte_countp);
	else
          IO_SYM_name(SEQ_symbols(s) + i) =
	    get_fat_str(stream, byte_countp);      
          break;
        case (Array):
          /* type 4 */
          SEQ_array(s)[i] = get_sequence(stream, byte_countp, file_type);
          break;
        case (List):
          /* type 5 */
          SEQ_list(s)[i] = get_sequence(stream, byte_countp, file_type);
          break;
        case (Byte_block):
          /* type 5 */
          SEQ_byte_block(s)[i] = get_byte_block(stream, byte_countp);
	  break;
        }
    }
  
  return s;
} /* get_sequence */


/********************************************
 *                                          *
 * functions for reading fsms from a stream *
 *                                          *
 ********************************************/

static void number_to_network_flags(long int b, NETptr net)
{
  NET_deterministic(net)        = getbit(b, 0);
  NET_pruned(net)                       = getbit(b, 1);
  NET_completed(net)            = getbit(b, 2);
  NET_minimized(net)            = getbit(b, 3);
  NET_epsilon_free(net)         = getbit(b, 4);
  NET_loop_free(net)            = getbit(b, 5);
  NET_twol_net(net)                     = getbit(b, 6);
  NET_closed_sigma(net)         = getbit(b, 7);  
  NET_start_state_final(net) = getbit(b, 8);
  NET_compacted(net)            = getbit(b, 9);
  NET_sorted_arcs(net)          = getbit(b, 10);
  NET_tabular(net)              = getbit(b, 11);
  NET_upper_seq(net)            = getbit(b, 12);
  NET_lower_seq(net)            = getbit(b, 13);
  NET_left_bimachine(net)	= getbit(b, 14);
  NET_right_bimachine(net)	= getbit(b, 15);
  NET_reduced_labelset(net)	= getbit(b, 16);
  NET_is_virtual(net)	        = getbit(b, 17);
  NET_optimized(net)	        = getbit(b, 18);
  NET_names_matter(net)	        = getbit(b, 19);
} /* number_to_network_flags */


static void get_networkprops(long int prop_count, NETptr net,
	FILE *stream, unsigned long *byte_countp, int file_type)
{
  int i;
  FAT_STR attr;
  OBJECTptr value;
  
  for (i = 0; i < prop_count; i++)
    {
      	if (file_type == NEW_STANDARD_FILE_TYPE ||
	    file_type == NEW_COMPACTED_FILE_TYPE)
	  attr = get_encrypted_fat_str(stream, byte_countp);	  
	else
	  attr = get_fat_str(stream, byte_countp);

      value = get_object(stream, byte_countp, file_type);
      
      add_prop(net, attr, value);
    }
  
#ifdef SMALL_FSM
#ifndef WINSOFT
  value = get_value(net, AUTHOR_ATTR);
  if (value != NULL)
    {
      fsm_printf ("Created by ");
      print_fat_string(OBJ_str(value));
      fsm_printf (".\n");
    }
  value = get_value(net, COPYRIGHT_ATTR);
  if (value != NULL)
    {
      print_fat_string(OBJ_str(value));
      fsm_printf ("\n");
    }
#endif
#endif
} /* get_networkprops */

static ALPHABETptr file_to_global_atom_map;

static void get_atoms(FILE *stream, id_type *max_id, unsigned long *byte_countp, int file_type)

     /*** New version of get_atoms %%%TSY 11/4/92, 11/5/92 
       Now properly handles files with mixed-type atom lists,
       like those that Ron Kaplan typically produces.
       ***/
{
  SEQUENCEptr atoms;
  FAT_STR name = NULL;
  int i, type = -2;
  id_type id;
  
  atoms = get_sequence(stream, byte_countp, file_type);
  
#ifdef SMALL_FSM
  file_to_global_atom_map = make_alph(SEQ_len(atoms) + 4, LABEL_VECTOR);
  ALPH_item(file_to_global_atom_map,2) = REPEAT_SYMBOL;
#else
  file_to_global_atom_map = make_alph(SEQ_len(atoms) + 2, LABEL_VECTOR);
#endif
  
  ALPH_item(file_to_global_atom_map,0) = EPSILON; /* default value */
  ALPH_item(file_to_global_atom_map,1) = OTHER; /* default value */
  
  for (i = 0; i < SEQ_len(atoms); i++) /* this starts with atom 2 */
    /* 0 is reserved for EPSILON, 1 for OTHER */
    {
      switch(SEQ_type(atoms))
        {
        case (Int):
          name = integer_to_fstring(SEQ_ints(atoms)[i]);
          break;
        case (Character):
          name = fchar_to_fstring(&SEQ_characters(atoms)[i]);
          break;        
        case (String):
          name = SEQ_strings(atoms)[i];
          break;
        case (Symbol_package):
          name = SYM_PACK_sym(SEQ_symbol_package(atoms), i);
          break;
        case (Symbol):
          name = IO_SYM_name(SEQ_symbols(atoms) + i);
          break;
        case (Object):  /* handle mixed types properly */
          type = OBJ_type(SEQ_objects(atoms)[i]);
          switch (type)
            {
            case (Int):
              name = integer_to_fstring(OBJ_integer(SEQ_objects(atoms)[i]));
              break;
            case (Character):
              name = fchar_to_fstring(&OBJ_character(SEQ_objects(atoms)[i]));
              break;    
            case (String):
              name = OBJ_str(SEQ_objects(atoms)[i]);
              break;
            case (Symbol):
              name = IO_SYM_name(OBJ_symbol(SEQ_objects(atoms)[i]));
              break;
            default:
              handle_error ("Unimplemented object type", "get_atoms", 
                            OBJ_type(SEQ_objects(atoms)[i]));
              break;
            }
          break;
        default:
          handle_error("Illegal symbol type", "get_atoms", SEQ_type(atoms));
          break;
        }
      
      if (SEQ_type(atoms) == Symbol_package)
        type = Symbol;
      else
        if (SEQ_type(atoms) != Object) /* in mixed case, type is set above */
          type = SEQ_type(atoms);
      
      intern_atomic_label(&id, name);

      ALPH_item(file_to_global_atom_map,i+2) = id;

      if (id > *max_id)
	*max_id = id;
    }
  free_sequence(atoms);
} /* get_atoms */

static void old_get_alphabet(NETptr net, FILE *stream, unsigned long
			     *byte_countp)
{
  int16 k, match, arity;
  int32 i;
  uint16 j, num;
  id_type id, max_id = 0;
  uint16 label_count;
 
  static id_type *tuple; /* final collection of ids for building the tuple */
  static int *tuple_labels;/* collected numbers to be converted in an id set */
  static int16 max_arity = 0; /* for being able to re-use the id-arrays already allocated*/

  /* MAX_ID is not used here but we need to pass it as a parameter
     to GET_ATOMS() because the new GET_ALPHABET() that also calls
     GET_ATOMS() needs the value of MAX_ID.
     */

  get_atoms(stream, &max_id, byte_countp, OLD_STANDARD_FILE_TYPE);
  
  label_count = (uint16) get_number(stream, byte_countp);
  
  arity = (int16) get_number(stream, byte_countp);
  NET_arc_label_arity(net) = arity;
  
  NET_labels(net) = make_alph(label_count, LABEL_VECTOR);
  
  /* Watch out for EPSILON.  It must be added to NET_labels
     here to interpret arc labels properly and removed later
     in keeping with the convention that EPSILON does not
     belong to LABELS or SIGMA.
     */
  
  NET_epsilon_free(net) = TRUE;
  
  if (arity == 1)
    for (j = 0; j < label_count; j++)
      {
        num = (uint16) get_number(stream, byte_countp);
        id = ALPH_item(file_to_global_atom_map, num);
        
        if (id == EPSILON)
          NET_epsilon_free(net) = FALSE;
        
        ALPH_item(NET_labels(net), j) = id;
      }
  else
    for (j = 0; j < label_count; j++)
      {
        match = TRUE;
        if (arity > max_arity)
          {
            if (max_arity > 0)
	      {
		free((void *) tuple_labels);
		free((void *) tuple);
	      }
            
            tuple_labels = (int *)
              calloc((size_t) arity, (size_t) sizeof (int));
            if (tuple_labels == NULL)
              not_enough_memory("NETWORK_FROM_FILE");

	    tuple = (id_type *)
              calloc((size_t) arity, (size_t) sizeof (id_type));
            if (tuple == NULL)
              not_enough_memory("NETWORK_FROM_FILE");
            
            
            max_arity = arity;
          }
        
        for (k = 0; k < arity; k++)
          {
            i = (int32) get_number(stream, byte_countp);
            /* If the value is negative, it must be a part of OTHER:OTHER */
            if (i < 0)
              if ( (i != ONE_SIDED_OTHER) ||
                    (k > 0 && tuple_labels[k-1] != ONE_SIDED_OTHER) )
                handle_error("Illegal negative label", "GET_ALPHABET", i);
            
            /* watch for X:X:X... */
            if (match == TRUE && k >= 1 && i != tuple_labels[k-1])
              match = FALSE;
            
            tuple_labels[k] = i;
          }
        
        /* Tuple construction */
        if (match == TRUE && tuple_labels[0] != ONE_SIDED_OTHER) 
          { /* tuple_labels contains an identity pair different from
	       <-1, -1>; the pair <1, 1> becomes OTHER */
             id = ALPH_item(file_to_global_atom_map, tuple_labels[0]);
            
            if (id == EPSILON)
              NET_epsilon_free(net) = FALSE;
            
            ALPH_item(NET_labels(net), j) = id;
          }
        else
          {
            for (k = 0; k < arity; k++)
              if (tuple_labels[k] == ONE_SIDED_OTHER)
		/* <-1,-1> becomes OTHER:OTHER */
                tuple[k] = ALPH_item(file_to_global_atom_map, OTHER);
              else
                tuple[k] =
                  ALPH_item(file_to_global_atom_map, tuple_labels[k]);
            
            intern_tuple_label(&id, tuple, arity);
            ALPH_item(NET_labels(net), j) = id;
          }
      }
  
  free_alph(file_to_global_atom_map);
} /* old_get_alphabet */


static void get_alphabet(NETptr net, FILE *stream, unsigned long *byte_countp,
			 int file_type)
{
  int16 k, match, arity;
  int32 i;
  uint16 j, num;
  uint16 label_count;
  int other_present_p = FALSE;
  id_type id, max_id = 0;
  ALPHABETptr sig;

  /* Final collection of ids for building the tuple: */
  id_type *tuple;

  /* Collected numbers to be converted in an id set: */
  int *tuple_labels;

  get_atoms(stream, &max_id, byte_countp, file_type);
  
  label_count = (uint16) get_number(stream, byte_countp);

  /* Get the label arity and allocate temporary buffers accordingly: */
  arity = (int16) get_number(stream, byte_countp);
  NET_arc_label_arity(net) = arity;
 
  tuple_labels= (int *) calloc((size_t) arity, (size_t) sizeof (int));
  if (tuple_labels == NULL)
      not_enough_memory("NETWORK_FROM_FILE");
            
  tuple= (id_type *) calloc((size_t) arity, (size_t) sizeof (id_type));
  if (tuple == NULL)
      not_enough_memory("NETWORK_FROM_FILE");
  
  NET_labels(net) = make_alph(label_count, LABEL_VECTOR);
  
  /* Watch out for EPSILON.  It must be added to NET_labels
     here to interpret arc labels properly and removed later
     in keeping with the convention that EPSILON does not
     belong to LABELS or SIGMA.  */
  
  NET_epsilon_free(net) = TRUE;
  
  if (arity == 1)
    for (j = 0; j < label_count; j++)
      {
        num = (uint16) get_number(stream, byte_countp);
        id = ALPH_item(file_to_global_atom_map, num);
        
        if (id == EPSILON)
          NET_epsilon_free(net) = FALSE;
	else if (id == OTHER)
	  other_present_p = TRUE;
        
        ALPH_item(NET_labels(net), j) = id;
      }
  else
    for (j = 0; j < label_count; j++)
      {
        match = TRUE;
        
        for (k = 0; k < arity; k++)
          {
            i = (int32) get_number(stream, byte_countp);
            /* If the value is negative, it must be a part of OTHER:OTHER */
            if (i < 0)
              if ((i != ONE_SIDED_OTHER ||
                    (k > 0 && tuple_labels[k-1] != ONE_SIDED_OTHER)))
                handle_error("Illegal negative label", "GET_ALPHABET", i);
            
            /* watch for X:X:X... */
            if (match == TRUE && k >= 1 && i != tuple_labels[k-1])
              match = FALSE;
            
	    if (i == OTHER || i == ONE_SIDED_OTHER)
	      other_present_p = TRUE;

            tuple_labels[k] = i;
          }
        
        /* Tuple construction */
        if (match == TRUE && tuple_labels[0] != ONE_SIDED_OTHER) 
          {/* tuple_labels contains an identity pair different from
	      <-1, -1>; the pair <1, 1> becomes OTHER */
            id = ALPH_item(file_to_global_atom_map, tuple_labels[0]);
            
            if (id == EPSILON)
              NET_epsilon_free(net) = FALSE;
            
            ALPH_item(NET_labels(net), j) = id;
          }
        else
          {
            for (k = 0; k < arity; k++)
              if (tuple_labels[k] == ONE_SIDED_OTHER)
		/* The pair <-1,-1> becomes OTHER:OTHER */
		  tuple[k] = ALPH_item(file_to_global_atom_map, OTHER);
              else
                tuple[k] =
                  ALPH_item(file_to_global_atom_map, tuple_labels[k]);
            
            intern_tuple_label(&id, tuple, arity);
            ALPH_item(NET_labels(net), j) = id;
          }
      }
  
  /* Turn file_to_gloabal_atom_map into sigma.
     OTHER will be in the sigma only if it occurs
     in the label alphabet. EPSILON is never in the sigma.
     */

  if (other_present_p && max_id < OTHER)
    max_id = OTHER;

  sig = make_alph(max_id + 1, BINARY_VECTOR);

  if (other_present_p)
    ALPH_item(sig, OTHER) = 1;

  for (i = 2; i < ALPH_len(file_to_global_atom_map); i++)
      ALPH_item(sig, ALPH_item(file_to_global_atom_map, i)) = 1;
  
  NET_sigma(net) = sig;
  free_alph(file_to_global_atom_map);
  free(tuple_labels);
  free(tuple);

} /* get_alphabet */

#ifndef FST_SLIM /* Only a minimal set of functions is
                    compiled when FST_SLIM is defined. */


#ifdef SMALL_FSM

/* Auxiliary routines used in the eliminatation of trailing epsilon arcs. */

inline static ARCptr last_arc(STATEptr state)
{
  ARCptr next, arc;

  if ((arc = STATE_arc_set(state)))
    while ((next = next_arc(arc)))
      arc = next;

  return(arc);
}


inline static ARCptr last_eps_arc_predecessor(STATEptr state)
{
  /* Returns the penultimate arc that is followed by a final epsilon
     or altchain arc, if one exists, otherwise NULL.
     */

  ARCptr prev = NULL, next, arc;

  if ((arc = STATE_arc_set(state)))
    while ((next = next_arc(arc)))
      {
	prev = arc;
	arc = next;
      }
 
  if (prev &&
      (ARC_label(arc) == EPSILON || ARC_label(arc) == ALTCHAIN_SYMBOL))
    return(prev);
  else
    return(NULL);
}
#endif

#endif /* FST_SLIM */

inline
static int get_arc(NETptr net, FILE *stream, unsigned long *byte_countp)
{
  STATEptr dest_state;
  ARCptr new_arc;
#ifdef SMALL_FSM
  ARCptr prev_arc;
#endif
#ifdef DEBUG
  label l = {0, 0, 0};
#else
  label l;
#endif
  long int dest_num;
  fst_byte b;
  
  
  if (STATUS_BAR_INCREMENT > 0 
      && ARC_COUNT > 0 
      && ARC_COUNT % STATUS_BAR_INCREMENT == 0)
    {
      fsm_putc('>', stderr);
      fflush(stderr);
    }
  
  byte_to_label(CUR_BYTE, &l, stream, byte_countp);  

  new_arc = make_arc(ALPH_item(NET_labels(net), (id_type) LABEL_code(l)),
		     NULL, net);
  if (new_arc == NULL)
    return (-1);
  
  ARC_COUNT++;
#ifdef SMALL_FSM
  prev_arc = last_arc(CUR_STATE);
#endif
  add_arc(new_arc, CUR_STATE, net);
  
  b = next_byte(stream, byte_countp);
  
  if (number_p(b))
    {
      dest_num = byte_to_number(b, stream, byte_countp);
      dest_state = STATE_VECTOR[dest_num];
      if (dest_state == NULL)
        {       
          if (dest_num == NET_num_states(net))
            {
              dest_state = make_state(NULL, net);
              if (dest_state == NULL)
                return (-1);
              STATE_VECTOR[NET_num_states(net) - 1] = dest_state;
              if (LABEL_final_p(l))
                STATE_final(dest_state) = TRUE;
            }
          else
            handle_error("no state at loc", "get_arc", (int) dest_num);
        }
      
      ARC_destination(new_arc) = dest_state;
      
      if (LABEL_last_p(l))
        {
#ifdef SMALL_FSM
	  /* If NEW_ARC is the final epsilon or altchain arc in its state
	     and the destination state has a non-empty arc set and we have
	     a previous arc, then free the new arc and instead make the next
	     field of the previous arc point to the arc set of the
	     destination state. If we are not able to deal with the
	     EPSILON/ALTCHAIN label at this point, we set a global
	     flag so that we'll do it later.
	     */

	  if (ARC_label(new_arc) == EPSILON ||
	      ARC_label(new_arc) == ALTCHAIN_SYMBOL)
	    {
	      if (STATE_arc_set(dest_state) && prev_arc)
		{
		  next_arc(prev_arc) = STATE_arc_set(dest_state);
		  free_arc(new_arc);
		}
	      else
		ALTCHAIN_P = TRUE;
	    }
#endif
          pop(STATE_STACK);
          CUR_STATE = (STATEptr) peek(STATE_STACK);
        }
      
      CUR_BYTE = next_byte(stream, byte_countp);
      /* for consistency with the other case, we read an extra byte */
    }
  else
    {
      dest_state = make_state(NULL, net);
      if (dest_state == NULL)
        return (-1);
      STATE_VECTOR[NET_num_states(net) - 1] = dest_state;
      
      ARC_destination(new_arc) = dest_state;
      if (LABEL_final_p(l))
        STATE_final(dest_state) = TRUE;
      
      if (LABEL_last_p(l))
        pop(STATE_STACK);
      
      push(dest_state, STATE_STACK);
      CUR_STATE = dest_state;
      
      CUR_BYTE = b;
    }
  
  return (1);
} /* get_arc */


/* static int get_arcs_and_states(long int num_states, long int num_arcs, NETptr net, */
/* 	FILE *stream, unsigned long *byte_countp) */
NETptr get_arcs_and_states(char *filename, fst_byte* cur_byte, long int num_states,
                        long int num_arcs, NETptr net, FILE *stream,
                        unsigned long *byte_countp, int file_type)
{
    /* Note that the first input parameter is not needed by this function:
       it is only to cope with the function pointer typedef "restore_func". */ 
  int err;
  long i;

#ifdef SMALL_FSM
  OBJECTptr vectorize_n;
  ALTCHAIN_P = FALSE;
#endif
  
  NET_start_state(net) = make_state(NULL, net);
  STATE_final(NET_start_state(net)) = NET_start_state_final(net);
  *cur_byte = next_byte(stream, byte_countp);
  
  if (num_arcs > 0)
    {
      STATE_VECTOR =
        (STATEptr *) calloc ((size_t) num_states, (size_t) sizeof(STATEptr));
      
      STATE_STACK = create_stack("States to read");
      
      if (STATE_VECTOR == NULL || STATE_STACK == NULL)
        {
          fsm_fprintf(stderr, "Read failed. Not enough memory ");
          fsm_fprintf(stderr, "for %ld states.\n", num_states);
          return(NULL);
        }
      STATE_VECTOR[0] = NET_start_state(net);  
      
      /* initialize status bar printout for big nets */   
      if (DISPLAY_READ_STATUS_BAR && (num_arcs >= 2500))
        {
          STATUS_BAR_INCREMENT = num_arcs/STATUS_BAR_WIDTH;
          
          fsm_fprintf (stderr, "  ");
          for (i = 0; i < STATUS_BAR_WIDTH; i++)
            fsm_fprintf (stderr, " ");
          
          fsm_fprintf (stderr, "100%%\r0%%");
          fflush(stderr);
        }
      else 
        STATUS_BAR_INCREMENT = -1;
      
      CUR_STATE = NET_start_state(net);
      push(CUR_STATE, STATE_STACK);
      ARC_COUNT = 0;
      
      while (STACK_depth(STATE_STACK) > 0)
        {       
          err = get_arc(net, stream, byte_countp);
          if (err == -1)
            return (NULL);
        }
      
      if (STATUS_BAR_INCREMENT > 0)
        fputc('\n', stderr);
      
      free_stack(STATE_STACK);
      
      if (num_arcs != NET_num_arcs(net))
        {
          handle_warning ("arc count mismatch", "GET_ARCS_AND_STATES",
                          (int) num_arcs - NET_num_arcs(net));
          return(NULL);
        }
#ifdef SMALL_FSM
      NET_states(net) = NULL;
      /* Eliminate state-final epsilon and altchain arcs */
      if (ALTCHAIN_P)
	{
	  ARCptr last_arc, prev_arc;

	  for (i = 0; i < NET_num_states(net) - 1; i++)
	    if ((prev_arc = last_eps_arc_predecessor(STATE_VECTOR[i])))
	      {
		last_arc = next_arc(prev_arc);

		if (STATE_arc_set(ARC_destination(last_arc)))
		  next_arc(prev_arc) = STATE_arc_set(ARC_destination(last_arc));
		
		free_arc(last_arc);
	      }
	}
	
#else
      /* Relink the state list to make it the same as the
         Lisp version of this function produces.  This
         facilitates debugging. The order of linking
         as such is irrelevant. Remove this later.
         
         */          
      for (i = 0; i < NET_num_states(net) - 1; i++)
        next_state(STATE_VECTOR[i]) = STATE_VECTOR[i+1];
      
      next_state(STATE_VECTOR[i]) = NULL;
      
      NET_states(net) = STATE_VECTOR[0];
#endif

#ifdef SMALL_FSM
      
      /* A simple call to vectorize_net won't work here, because in INFL,
	 states don't have a NEXT field.
	 */
      vectorize_n = get_value(net, VECTORIZE_N_ATTR);
      
      if (vectorize_n != NULL)
        {
          int vector_size = arc_vector_size(net);
	  NET_arc_vector_heap(net) =
	    init_heap(vector_size * sizeof(STATEptr), 
		      NET_num_states(net), "Arc vectors");		  
          for (i = 0; i < num_states && STATE_VECTOR[i] != NULL; i++)
            make_vector_state(STATE_VECTOR[i], net, vector_size);
        }
#endif

      free((void *) STATE_VECTOR);
    }
  else
    NET_states(net) = NET_start_state(net);
  
  if (num_states != NET_num_states(net))
    {
      handle_warning ("state count mismatch", "GET_ARCS_AND_STATES",
                      (int) num_states - NET_num_states(net));
      return(NULL);
    }

  return (net);
} /* get_arcs_and_states */


#ifndef TWOLC_APPLICATION /* No compaction in the rule compiler world. */

/* static int get_arc_block(NETptr net, FILE *stream, unsigned long *byte_countp) */
NETptr get_arc_block(char *filename, fst_byte* cur_byte, long int num_states,
                  long int num_arcs, NETptr net, FILE *stream,
                  unsigned long *byte_countp, int file_type)
{

    /* Note that the first input parameter is not needed by this function:
       it is only to cope with the function pointer typedef "restore_func". */ 

    /* MEMORY_MAP: This functionality maps the arc block part of a compacted 
                   network directly in memory: It means that when reading a 
                   network from a file, the arc block part stays on the disk 
                   instead of being copied in memory as usual. Afterwards, 
                   the process can access the arcs and states of the network 
                   directly on the disk. */

  long int size, start_addr;
  unsigned char *block = NULL;

  start_addr = get_number(stream, byte_countp);
  size = get_number(stream, byte_countp);

  if (MEMORY_MAP)
  {
      unsigned long off = 0; /* Offset for mmap */

#if defined _WINDOWS || defined UNDER_CE
      HANDLE hFMap = NULL; /* mmap handles */
      HANDLE hMap = NULL;
      int i;
      int sz = strlen(filename);

      /* Converts from char* to wchar* */
      LPTSTR Wfilename = (TCHAR*)malloc((sz+1)*sizeof(TCHAR));
      for (i=0 ;i<sz;i++)
      {
	      Wfilename[i] = (TCHAR)((unsigned char)filename[i]);
      }
      Wfilename[sz] = '\0';
#endif /* _WINDOWS || UNDER_CE */
	  /* The offset argument in mmap() must be multiple of the page size.
         So we map the entire file and set block address to address of 
         the memory mapped block + offset */
      off = ftell(stream);
      net->mmap_size = (size_t)size + off;

#if defined _WINDOWS || defined UNDER_CE
#if defined UNDER_CE
      hFMap = CreateFileForMapping(
#else /* _WINDOWS */
      hFMap = CreateFile(
#endif
        Wfilename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,		  
	    OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
	    NULL);
      free(Wfilename);
      if (hFMap != NULL)
      {
          hMap = CreateFileMapping(
              hFMap,
              NULL, 
              PAGE_READONLY,
              0,
              0,
              NULL);
      }
      if (hMap != NULL)
      {
          block = (unsigned char*)MapViewOfFile(
              hMap,
              FILE_MAP_READ,
              (DWORD)0,
              (DWORD)0,
              (DWORD)net->mmap_size);
      }
      if (block == NULL) /* map failure */
      {
          handle_error(strerror(errno), "MAP_ARC_BLOCK", errno);
          return(NULL);
      }
#else /* UNIX */
      block = (unsigned char*)mmap((caddr_t)0, net->mmap_size, PROT_READ,
                                   MAP_SHARED,
                                   fileno(stream), 0);
      if (block == MAP_FAILED)
      {
          handle_error("mMap failed", "MAP_ARC_BLOCK", 0);
          return(NULL);
      }
#endif /* if defined _WINDOWS || defined UNDER_CE */

      /* Saves the handle in order to be able to "unmap" at net freeing */
      net->mmap_handle = block;

      block += off;
      fseek(stream, size, 1);
    }
    else /* !MEMORY_MAP */
    {
        block = (unsigned char *) malloc((size_t) size);

        if (block == NULL && size != 0)
        {
            not_enough_memory("GET_ARC_BLOCK");
            return(NULL);
        }
        if (fread((void *) block, (size_t) 1, (size_t) size, stream) != (size_t)size)
        {
            handle_error("Read error", "GET_ARC_BLOCK", 0);
            return(NULL);
        }
    } /* if (MEMORY_MAP) */
      
    NET_block_size(net) = size;
    NET_arc_block(net) = block;
    NET_start_loc(net) = block + start_addr;
  
    *byte_countp += size;
  
    if (size > 0)
        CUR_BYTE = next_byte(stream, byte_countp);
    return (net);
}  /* get_arc_block */
#endif /* TWOLC_APPLICATION */

              
static void remove_epsilon(ALPHABETptr labels)
{
  /* If an EPSILON is found in the label set, shift the
     remaining labels one position to the left.
     */
  
  id_type *items = ALPH_items(labels);
  id_type *cur = items;
  int  i, len = ALPH_len(labels), epsilon_found_p = FALSE;
  
  for (i = 0; i < len; i++)
    {
      if (*items == EPSILON)
        {
          if (epsilon_found_p == FALSE)
            {
              epsilon_found_p = TRUE;
              ALPH_len(labels)--;
              len--;
              if (i == len)
                break;          /* Last item, nothing left to shift. */
              else
                cur = items + 1;
            }
          else
            handle_error("Two epsilons?", "REMOVE_EPSILON", 0);
        }
      *items++ = *cur++;
    }
  if (!epsilon_found_p)
    handle_error("No epsilons?", "REMOVE_EPSILON", 0);
} /* remove_epsilon */


/******************************************************************
 *                                                                *
 * Top-level functions for reading fsms from a file               *
 *                                                                *
 ******************************************************************/

void (*transition_table_from_stream)(FILE *stream, NETptr net) = NULL;

NETptr network_from_stream(FILE *stream, int file_type)
{
  /* This function is intended to load common fsm files;
     it cannot handle Kaplan compressed networks,
     (for loading from a compressed file call the function
     read_Kaplan_compressed_network()). */

  unsigned long expected_states, expected_arcs, expected_bytes;
  unsigned long prop_count;
  int err= 0;
  NETptr net;
  unsigned long byte_count = 0;

  switch (file_type)
    {

    case OLD_STANDARD_FILE_TYPE:
    case STANDARD_FILE_TYPE:
    case OLD_STANDARD_COMPACTED_FILE_TYPE:
    case STANDARD_COMPACTED_FILE_TYPE:
    case NEW_STANDARD_FILE_TYPE:
    case NEW_COMPACTED_FILE_TYPE:
      break;
    default:
      fsm_fprintf(stderr, "Unrecognized file type: %d", file_type);
      return(NULL);
      break;
    }


  /* read the number of states */
  expected_states = get_number(stream, &byte_count);
  
  expected_arcs = get_number(stream, &byte_count);
  
  net = make_network();
  
  /* read network flags */
  number_to_network_flags(get_number(stream, &byte_count), net);
    
  /*  Should we abort here in xfst?

#ifdef WORLD
  if (NET_is_virtual(net))
    {
      fsm_fprintf(stderr, "Unsupported file format: ");
      fsm_fprintf(stderr, " cannot load the network.\n");
      NET_is_virtual(net) = FALSE;
      free(net);
      return(null_fsm());
    }
#endif

  */

  /* read number of saved properties */
  prop_count = get_number(stream, &byte_count);
    
  if (prop_count > 0)
    get_networkprops(prop_count, net, stream, &byte_count, file_type);
    
  /* read arc_labels */
    
  /* In OLD_STANDARD_FILE_TYPE files the file alphabet starts with
     the components of the networks label set. In the current
     STANDARD_FILE_TYPE files, the file alphabet starts with
     the sigma (but not including OTHER). The new get_alphabet
     function assembles both the label and sigma alphabet.
     The old get_alphabet function deals only with the
     label alphabet.
  */

  if (file_type == OLD_STANDARD_FILE_TYPE)
    old_get_alphabet(net, stream, &byte_count);
  else
    get_alphabet(net, stream, &byte_count, file_type);
    
  /* read arcs */
    
#ifndef TWOLC_APPLICATION  /* No compaction in the rule compiler world. */
#ifndef LEXC_APPLICATION  
#ifndef XIS_APPLICATION  

  if (NET_compacted(net))
    {
      /* Let's believe the expectations. */
        
      NET_num_states(net) = expected_states;
      NET_num_arcs(net) = expected_arcs;
      if (MEMORY_MAP)
	  {
#if defined _WINDOWS || defined UNDER_CE
        net = restore_functions[TypeRestoreCompact](current_filename, &CUR_BYTE,
                                                    expected_states, expected_arcs,
                                                    net, stream,
                                                    &byte_count, file_type);
        free(current_filename);
#else
        net = restore_functions[TypeRestoreCompact]("Compacted", &CUR_BYTE,
                                                    expected_states, expected_arcs,
                                                    net, stream,
                                                    &byte_count, file_type);
#endif /* defined _WINDOWS || defined UNDER_CE */
	  }
	  else /* !MEMORY_MAP */
	  {
         net = restore_functions[TypeRestoreCompact]("Compacted", &CUR_BYTE,
                                                    expected_states, expected_arcs,
                                                    net, stream,
                                                    &byte_count, file_type);
      }
      if (net != NULL)
          NET_recode_key(net) = copy_alphabet(NET_labels(net));
    }
#ifndef WORLD
#ifndef SLIM_FST /* No virtual nets when SLIM_FST is defined. */
  
  else if (NET_is_virtual(net))
    net = restore_functions[TypeRestoreVirtual]("Virtual", &CUR_BYTE,
                                                expected_states, expected_arcs,
                                                net, stream,
                                                &byte_count, file_type);
#endif /* SLIM_FST */
#endif /* WORLD */
  else
#endif
#endif
#endif
    {
      if (expected_states < 1)
	handle_warning("fewer than 1 state expected!",
		       "NETWORK_FROM_STREAM", expected_states);
  
      net = restore_functions[TypeRestoreStandard] ("Standard", &CUR_BYTE,
                                                    expected_states, expected_arcs,
                                                    net, stream,
                                                    &byte_count, file_type);
    }

  /* The network restoration must be pursued only if at this stage net has
     not become a NULL pointer:  */
  if (net != NULL)
  {
      
      /* Adhere to the convention that EPSILON does not belong to
         LABELS or SIGMA. */
    
      if (!NET_epsilon_free(net))
          remove_epsilon(NET_labels(net));
    
      expected_bytes = byte_to_number(CUR_BYTE, stream, &byte_count);
    
      if (expected_bytes != byte_count)
      {
          handle_warning ("byte count mismatch", "network_from_stream",
                          (int) expected_bytes - byte_count);
          err = -1;
      }
    
#ifndef SMALL_FSM
    
      /* In OLD_STANDARD_FILE_TYPE files the symbols that belong to
         sigma but do not occur as the component of any label
         are recorded on the property list of the network. The
         current STANDARD_FILE_TYPE files do not need this hack
         because the get_atoms() function assembles the sigma
         (- OTHER) rather than the label components.
      */
    
      if (file_type == OLD_STANDARD_FILE_TYPE)
          recover_sigma(net);   
#endif
    
      if (err == -1)
      {
#ifndef TWOLC_APPLICATION
#ifndef XIS_APPLICATION
          if (NET_compacted(net))
          {
              free((void *) NET_arc_block(net));
              NET_arc_block(net) = NULL;
          }
#endif
#endif
          free_network(net);
          net = null_fsm();
      }
    
      /* If the tabular flag is TRUE, we now must read the transition table
         and include it into the net. */
    
      if (NET_tabular (net))
      {
          if (! transition_table_from_stream)     /* function not available */
              fsm_fprintf (stderr, "*** ERROR: file contains a transition table\
which cannot be read in this implementation.\n");
        
          else                                    /* function available */
              transition_table_from_stream (stream, net);
      }
    
      check_for_flags(net);

      /* Quietly convert a virtual net to a real one for xfst users. */

      /* Had to back out of this solution. It appears that this file
         is always compiled with the -DWORLD flag.
         #ifdef WORLD
         if (NET_is_virtual(net))
         virtual_to_real_fsm(net, DO_MINIMIZE);
         #endif 
      */

#ifdef WORLD
/* The arc_optimization is not supported by Xfst:
   if the loaded net has its flag "optimized" ON, unoptimize it:
*/
      if (NET_optimized(net) || possibly_already_optimized_p(net))
      {
          if (HEADER_license_type(get_last_header()) == FILE_TYPE_LICENSE_PUBLIC)
          {
              /* Reject the file: Xfst is not allowed to process
                 optimized networks of type "PUBLIC". */
              fsm_fprintf(stderr, "Unsupported file format: ");
              fsm_fprintf(stderr, "*** The application is not permitted to load \
this file.\n    You need a licensed version of the program.\n");
              free_network(net);
              net = NULL;
          }
          else
          {
              /* Un-optimize the network:*/
              do_unoptimize_arcs(net);
          }
      }
#endif /* WORLD */
      
  } /* net != NULL */
  return (net);
} /* network_from_stream */


NVptr networks_from_file(FILE *stream)
{
  NVptr nets;
  int i, net_count;
  STANDARD_HEADERptr header;
  unsigned long byte_count =  0;
  int net_index= 0;
  
  if (!(header = get_header(stream, &byte_count)))
    return(NULL);
  
  net_count = (int) HEADER_net_count(header);
  nets = make_nv(net_count);
  
/*
#ifndef SMALL_FSM
  if (net_count == 1)
    fsm_fprintf(stdout, "reading 1 net\n");
  else
    fsm_fprintf(stdout, "reading %d nets\n", net_count);
#endif
*/
  for (i = 0; i < net_count; i++)
  {
      NETptr restored_net= NULL;

      if (HEADER_start_locations(header) != NULL)
      {
          if (HEADER_net_location(header, i) != 0)
              restored_net= network_from_stream(stream, HEADER_file_type(header));
      }
      else
          /* Old file type: no net_location array in the header. */
          restored_net= network_from_stream(stream, HEADER_file_type(header));


      /* Don't keep the NULL net pointers: */
      if (restored_net != NULL)
      {
          NV_net(nets, net_index)= restored_net;
          net_index++;
      }
      else
          NV_len(nets)--;
  }

  if (NV_len(nets) == 0)
  {
      free_nv_only(nets);
      nets= NULL;
  }
 
  return (nets);
      
} /* networks_from_file */


NVptr standard_nets_from_stream(FILE *stream)
{
	NVptr nets = NULL;
	  nets = networks_from_file(stream);
#ifndef SMALL_FSM
#ifndef SLIM_FST /* twol nets are not in the slimFst lib */
	  convert_twol_nets(nets, TRUE);
#endif
#endif /* SMALL_FSM */
	return(nets);
}

NVptr read_nets(char *filename)
{
  FILE *stream;
  NVptr nets = NULL;
  int error;
  int file_type;

  error= get_file_type (filename, &file_type);

  if (error == NO_ERROR )
    {
        if (file_type == KAPLAN_COMPACTED_FILE_TYPE) 
        { /* The net is in Kaplan compressed format:           
             The lxrt library should be available =>  We can use the xlt functions.
          */
            NETptr net= make_network();

            net= restore_functions[TypeRestoreCompress](filename, NULL, -1, -1,
                                                        net, NULL, NULL, file_type);
            if (net != NULL)
            {
                nets = make_nv(1);
                NV_nets(nets)[0]= net;
            }
        }
        else
        {
#if defined _WINDOWS || defined UNDER_CE
			if (MEMORY_MAP)
			{
              current_filename = (char*) malloc(strlen(filename)+1);
              strcpy(current_filename, filename);
		    }
#endif /* defined _WINDOWS || defined UNDER_CE */
            stream = fopen(filename, "rb");
            if (stream == NULL)
            {
                fsm_fprintf(stderr, "Cannot open '%s'\n", filename);
            }
            else
            {
                if (!(std_file_header_p(stream)))
                {
                  fsm_fprintf(stderr, "File cannot be loaded.\n", filename);
                }
                else
                {
                    nets = standard_nets_from_stream(stream);
                    set_header_file_name(NULL, filename);
                }
                fclose(stream);
            }
        } /* file_type != KAPLAN_COMPACTED_FILE_TYPE */
    } /* if (error !=0 ) */
  return(nets);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */


NETptr read_first_net(char *filename)
{
  unsigned long byte_count;
  FILE *stream;
  NETptr net = NULL;
  STANDARD_HEADERptr header;

  if (!(stream = fopen(filename, "rb")))
    fsm_fprintf(stderr, "Cannot open '%s'\n", filename);
  else
    {
      header = get_header(stream, &byte_count);

      if (header)
	{
	  NVtype nv;
	  NETptr nets[1];

	  nv.len = 1;
	  nv.nets = nets;

	  net = network_from_stream(stream, HEADER_file_type(header));

	  nets[0] = net;
	  set_header_file_name(NULL, filename);
#ifndef SMALL_FSM
	  convert_twol_nets(&nv, TRUE);
#endif
	}
      fclose(stream);
    }

  return(net);
} /* read_first_net */

/* Change Display status bar */  
int display_read_status_bar(int on_off)
{
    int old_mode = DISPLAY_READ_STATUS_BAR;
    DISPLAY_READ_STATUS_BAR = on_off;

    return old_mode;
}

#endif /* SLIM_FST */
