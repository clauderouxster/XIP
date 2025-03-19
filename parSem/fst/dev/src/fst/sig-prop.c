/* $Id: sig-prop.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1990-93  by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "fst/struct.h"
#include "fst/sig-prop.h"


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
#include "fst/lab-vect.h"
#include "fst/fsm_io.h"
#endif /* SLIM_FST */

/*****************************************************************
 **                                                              *
 **  SIG-PROP.C                                                  *
 **                                                              *
 **  Lauri Karttunen, RXRC 1993                                  *
 **                                                              *
 **  Functions for storing and retrieving alphabets              *
 **  as network properties. Used to be in SIGMA-IO.C.            *
 *   Temporary hack to complement bin-in.c so that SIGMA         *
 *   may be preserved in cases where it includes symbols         *
 *   that are not represented in the label set of the machine.   *
 *   (This could go away in The Great SIGMA Reform some day.)    *
 *                                                               *
 *****************************************************************/


static int empty_list_p (OBJECTptr object)
{
  return(OBJ_type(object) == Symbol &&
	 fat_strcmp(IO_SYM_name(OBJ_symbol(object)), NIL_NAME) == 0);
}


void recover_sigma(NETptr net)
{
  /* Looks for the :SAVED-SIGMA attribute on the property list.
     If SIGMA is specified, the NET_sigma field is set
     to it.  This preseves the correct semantics of
     networks (e.g. SIGMA_BUT(#\A)) whose extensional
     sigma is a subset of the true sigma.  After the
     operation is complete, the SIGMA property is removed
     because the reason for having it (presence of OTHER)
     might disappear.
     */
  
  OBJECTptr sigma_val;
  
  /* Install values for OTHER_STR, IL_PACKAGE, KEYWORD_PACKAGE,
     and SAVED_SIGMA_ATTR.
     */

  sigma_val = get_value(net, SAVED_SIGMA_ATTR);
  
  if (sigma_val == NULL)
    sigma(net);
  else
    {
      
      if (OBJ_type(sigma_val) == List)
	{
	  NET_sigma(net) = prop_val_to_alph(sigma_val, BINARY_VECTOR);
	  /****
	  remove_prop(net, SAVED_SIGMA_ATTR);
	  free_object(sigma_val);
	  */
	  free_prop(remove_prop(net, SAVED_SIGMA_ATTR));
	}
      else if (OBJ_type(sigma_val) == Symbol && empty_list_p(sigma_val))
      	/* The symbol LISP::NIL as value is equivalent to not having
	   SAVED_SIGMA_ATTR at all. */
      	{
	   /****
	  remove_prop(net, SAVED_SIGMA_ATTR);
	  free_object(sigma_val);
	  */
	  free_prop(remove_prop(net, SAVED_SIGMA_ATTR));
      	}
      else
	handle_error("Unexpected type of SAVED-SIGMA", "RECOVER_SIGMA",
		     OBJ_type(sigma_val));
    }
} /* recover_sigma */

#if NOT_USED
/* Support for PROP_VAL_TO_ALPH */

static void int_to_label_id (id_type *id, int char_code)
{
  intern_atomic_label(id, integer_to_fstring(char_code));
}

static int fchar_to_label_id (id_type *id, FAT_CHAR *fc)
{
  return(intern_atomic_label(id, fchar_to_fstring(fc)));
}

static int fstring_to_label_id(id_type *id, FAT_STR fs)
{
  return(intern_atomic_label(id, fs));
}
#endif /* NOT_USED */

static void symbol_to_label_id(id_type *id, FAT_STR name)
{ 

  if (fat_strcmp(name, FAT_OTHER) == 0)
    /* Found OTHER in SIGMA. */
    *id = OTHER;
  else
    intern_atomic_label(id, name);
} /*  symbol_to_label_id */


static int obj_to_symbol_id(id_type *id, OBJECTptr obj)
{
  switch (OBJ_type(obj))
    {
    case (Symbol):
      {
	FAT_STR name = IO_SYM_name(OBJ_symbol(obj));
	symbol_to_label_id(id, name);
      }
      break;
    default:
      handle_error("Illegal object type", "OBJ_TO_SYMBOL_ID",
		   OBJ_type(obj));
      *id= ID_NO_SYMBOL;
      return(ERR_IN_PROPERTY);
    }
return(NO_ERROR);
}

static int obj_to_tuple_id(id_type *tuple_id, OBJECTptr obj)
{
  SEQUENCEptr seq = OBJ_array(obj);
  uint16 arity = SEQ_len(seq);
  id_type id;
  uint16 i;
  id_type *items = (id_type *) malloc((size_t) arity * sizeof(id_type));

  if (SEQ_type(seq) == String)
    {
      FAT_STR *strings= SEQ_strings(seq);
      for (i = 0; i < arity; i++)
	{
	  symbol_to_label_id(&id, strings[i]);
	  items[i] = id;
	}
    }
  else if (SEQ_type(seq) == Object)
    {
      OBJECTptr *objects = SEQ_objects(seq);
      for (i = 0; i < arity; i++)
	{
	  obj_to_symbol_id(&id, objects[i]);
	  if (id == ID_NO_SYMBOL)
	    {
	      *tuple_id= ID_NO_SYMBOL;
	      return(ERROR);
	    }
	  items[i] = id;
	}
    }
  else
      handle_error("Wrong type of array", "OBJ_TO_TUPLE_ID",
		 SEQ_type(seq));
  /*  
      Removed this warning because this function is used to
      collect the labels removed by delete_eqv_labels. The
      list may well contain symbol pairs.

  if (FSM_MODE != TWOL_MODE)
    fsm_fprintf(stderr,
	    "\nFSTPAIR in SIGMA is not legal in standard mode.\n");
  */
  intern_tuple_label(tuple_id, items, arity);
  return(NO_ERROR);
} /* obj_to_tuple_id */



ALPHABETptr prop_val_to_alph (OBJECTptr val, int type)
{
  /* Returns an alphabet in the requested format that contains the symbols
     in the list object VAL.  Recognize the special case of the :OTHER
     symbol.  If type is LABEL_VECTOR, the symbols are stored in the same
     order as they appear in the list.  Uses LAB_VECTOR for temporary storage
     of label ids.
     */
  OBJECTptr obj;
  SEQUENCEptr seq;
  FAT_STR name;
  uint16 i; 
  id_type id;
  
  if (OBJ_type(val) != List)
    handle_error("Unexpected type of alphabet property",
		 "PROP_VAL_TO_ALPH", OBJ_type(val));
  
  init_lab_vector();
  seq = OBJ_list(val);
  
  for (i = 0; i < SEQ_len(seq); i++)
    {
      switch(SEQ_type(seq))
	{
	case (Symbol_package):
	  name = SYM_PACK_sym(SEQ_symbol_package(seq), i);
	  symbol_to_label_id(&id, name);

	  break;
	case (Object):
	  obj = SEQ_objects(seq)[i];
	  if (OBJ_type(obj) == Array)
	    obj_to_tuple_id(&id, obj);
	  else
	    obj_to_symbol_id(&id, obj);
	  break;
	default:
	  id = ID_NO_SYMBOL;
	  handle_error("Illegal symbol type", "PROP_VAL_TO_ALPH",
		       SEQ_type(seq));
	}
      add_label(id);
    }
  
  /* Return the value in the requested format. */
  
  if (!(type == BINARY_VECTOR || type == LABEL_VECTOR))
    handle_error("Bad alphabet type", "PROP_VAL_TO_ALPH", type);
  
  if (lab_vect_len() == 0)
    return(make_alph(0, type));
  
  if (type == LABEL_VECTOR)
    {
      ALPHABETtype alph;
      
      alph.type = LABEL_VECTOR;
      alph.len = lab_vect_len();
      alph.items = lab_vect_labs();
      
      return(copy_alphabet(&alph));
    }
  else
    {
      uint16 len = lab_vect_len();
      id_type *items = lab_vect_labs();
      ALPHABETptr alph =
      	make_alph(max_label_item(items, len) + 1, BINARY_VECTOR);
      
      add_label_alph(ALPH_items(alph), items, len);
      return(alph);
    }
} /* prop_val_to_alph */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/* Support for ALPH_TO_PROP_VAL */

#if NOT_USED

static long int *sigma_to_ints(ALPHABETptr sigma)
{
  LABELptr label;
  FAT_STR name;
  long int *ints;
  int i, len = ALPH_len(sigma);
  binary_to_label(sigma);
  ints = (long int *) calloc((size_t) len, (size_t) sizeof(long int));
  if (ints == NULL)
    not_enough_memory("RECORD_SIGMA");
  
  for (i = 0; i < len; i++)
    {
      label = id_to_label(ALPH_item(sigma, i));
      name = LABEL_atom(label);
      ints[i] = (long) fstring_to_charcode(name);
    }
  return(ints);
}


static FAT_CHAR *sigma_to_fchars(ALPHABETptr sigma)
{
  LABELptr label;
  FAT_STR name;
  FAT_CHAR *chars;
  int i, len = ALPH_len(sigma);
  binary_to_label(sigma);
  chars = (FAT_CHAR *) calloc((size_t) len, (size_t) sizeof(FAT_CHAR));
  if (chars == NULL)
    not_enough_memory("RECORD_SIGMA");
  for (i = 0; i < len; i++)
    {
      label = id_to_label(ALPH_item(sigma, i));
      name = LABEL_atom(label);
      fat_charcpy(chars[i], *name);
    }
  return(chars);
}


static FAT_STR *sigma_to_strings(ALPHABETptr sigma)
{
  LABELptr label;
  FAT_STR name, *strings;
  int i, len = ALPH_len(sigma);
  binary_to_label(sigma);
  strings = (FAT_STR *) calloc((size_t) len, (size_t) sizeof(FAT_STR));
  if (strings == NULL)
    not_enough_memory("RECORD_SIGMA");
  for (i = 0; i < len; i++)
    {
      label = id_to_label(ALPH_item(sigma, i));
      name = LABEL_atom(label);
      strings[i] = name;
    }
  return(strings);
}


static IO_SYMBOLptr sigma_to_symbols(ALPHABETptr sigma)
{
  LABELptr label;
  IO_SYMBOLptr sym, symbols;
  int i, len = ALPH_len(sigma);
  binary_to_label(sigma);
  symbols = (IO_SYMBOLptr) calloc((size_t) len,
				  (size_t) sizeof(IO_SYMBOLtype));
  if (symbols == NULL)
    not_enough_memory("RECORD_SIGMA");
  sym = symbols;
  for (i = 0; i < len; i++)
    {
      label = id_to_label(ALPH_item(sigma, i));
      IO_SYM_name(sym) = LABEL_atom(label);
      sym++;
    }
  return(symbols);
}
#endif /* NOT_USED */


static IO_SYMBOL_PACKAGEptr sigma_to_sym_pack(ALPHABETptr sigma)
{
  LABELptr label;
  IO_SYMBOL_PACKAGEptr sym_pack;
  FAT_STR *sym_name;
  int i, len = ALPH_len(sigma);

  binary_to_label(sigma);
  sym_pack = (IO_SYMBOL_PACKAGEptr)
    calloc((size_t) 1, (size_t) sizeof(IO_SYMBOL_PACKAGEtype));
  sym_name = (FAT_STR *)
    calloc((size_t) len, (size_t) sizeof(FAT_STR));
  
  if (sym_pack == NULL || sym_name == NULL)
    not_enough_memory("RECORD_SIGMA");
  
  SYM_PACK_sym_names(sym_pack) = sym_name;
  
  for (i = 0; i < len; i++)
    {
      label = id_to_label(ALPH_item(sigma, i));
      sym_name[i] = LABEL_atom(label);
    }
  return(sym_pack);
}


static OBJECTptr symbol_to_object(LABELptr label)
{ 
/*   converts atoms to objects, (for converting tuples, 
     use the function tuple_to_object()).
     It is supposed at this stage that the labels
     do NOT point to ATOM structures anymore; they
     just contain an 'atomic' name */

  OBJECTptr obj = (OBJECTptr)
    calloc((size_t) 1, (size_t) sizeof(OBJECTtype));

  FAT_STR name = LABEL_atom(label);
  IO_SYMBOLptr sym = (IO_SYMBOLptr) calloc((size_t) 1,
				  (size_t) sizeof(IO_SYMBOLtype));
  OBJ_type(obj) = Symbol; 

  if (sym == NULL)
    not_enough_memory("RECORD_SIGMA");
  OBJ_symbol(obj) = sym;
  IO_SYM_name(sym) = name;

  return(obj);
}

static OBJECTptr tuple_to_object(LABELptr label)
{  
  uint16 i;
  int16 len = LABEL_arity(label);
  TUPLEptr tuple = LABEL_tuple(label);
  OBJECTptr obj = (OBJECTptr) malloc((size_t) sizeof(OBJECTtype));
  FAT_STR *strings =
    (FAT_STR *) malloc((size_t) len * sizeof(FAT_STR));

  SEQUENCEptr seq = (SEQUENCEptr) malloc((size_t) sizeof(SEQUENCEtype));
  
  OBJ_type(obj) = Array;
  OBJ_array(obj) = seq;
  SEQ_len(seq) = len;
  SEQ_type(seq) = String;
  SEQ_strings(seq) = strings;

  for (i = 0; i < len; i++)
    strings[i] = LABEL_atom(id_to_label(TUPLE_labels(tuple)[i]));
     
  return(obj);
}

static OBJECTptr *sigma_to_objects(ALPHABETptr sigma)
{
  OBJECTptr *object;
  
  int i, len = ALPH_len(sigma);
  binary_to_label(sigma);
  object = (OBJECTptr *)
    calloc((size_t) len, (size_t) sizeof(OBJECTptr));
  if (object == NULL)
    not_enough_memory("RECORD_SIGMA");
  for (i = 0; i < len; i++)
    {
      LABELptr label = id_to_label(ALPH_item(sigma, i));
      if (LABEL_arity(label) == 1)
	object[i] = symbol_to_object(label);
      else
	object[i] = tuple_to_object(label);
    }
  return(object);
}

OBJECTptr alph_to_prop_val(ALPHABETptr alph)
{
  SEQUENCEptr seq;
  OBJECTptr prop_val;
  LABELptr label;
  id_type id;
  int i, len;
  int type = -1;
  
  binary_to_label(alph);
  
  len = ALPH_len(alph);
  
  seq = (SEQUENCEptr) calloc((size_t) 1, (size_t) sizeof(SEQUENCEtype));
  if (seq == NULL)
    not_enough_memory("RECORD_SIGMA");
  SEQ_len(seq) = len;
  
  prop_val =
    (OBJECTptr) calloc((size_t) 1, (size_t) sizeof(OBJECTtype));
  if (prop_val == NULL)
    not_enough_memory("RECORD_SIGMA");
  
  OBJ_type(prop_val) = List;
  OBJ_list(prop_val) = seq;
  
  type = Symbol_package; /* default type: for storing atomic names a priori */
  for (i = 0; i < len; i++) 
    { /* check if the alphabet contains tuples */
      id = ALPH_item(alph,i);
      label = id_to_label(id);
      if (LABEL_arity(label) > 1)
	{
	  type = Object; /* type becomes Object for storing tuples */
	  break;
	}
    } /* for */
  SEQ_type(seq) = type;
  if (type == Symbol_package)
    {
      SEQ_symbol_package(seq) = sigma_to_sym_pack(alph);
    }
  else
    {
      SEQ_objects(seq) = sigma_to_objects(alph);
    }

  return(prop_val);
}
#endif /* SLIM_FST */
