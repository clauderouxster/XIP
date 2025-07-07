/* $Id: pars-tbl.c,v 1.5 2015/11/19 09:24:27 roux Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "fst/pars-tbl.h"
#include "fst/error.h"
#include "fst/struct.h"



#include "fst/fsm_io.h"
#include "fst/eqv-labs.h"
#include "fst/utf8_fatstr_util.h"
#include "fst/utf8_util.h"

/******************************************************
 **
 **  PARS-TBL.C
 **
 **  Lauri Karttunen, RXRC 1993
 **
 **  Functions relocated from ASCII-IO.C and INFL-AUX.C
 **
 ******************************************************/

id_type INPUT_SEQ[WORD_STRING_SIZE +1];

id_type *UPPER_MATCH = NULL; /* UPPER_MATCH[i] the upper_id of the i'th label */
id_type *LOWER_MATCH = NULL; /* LOWER_MATCH[i] the lower_id of the i'th label */
id_type *MATCH_TABLE = NULL; /* Alternates between UPPER_MATCH and LOWER_MATCH */


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/*****************************************
 *                                       *
 *  parse table for lookup and lookdown  *
 *                                       *
 *****************************************/


static CH_NODEptr clean_ch_node(CH_NODEptr *node)
{
  int i;
  CH_NODEptr ch_node;
  
  for (i = 0; i < 256; i++)
    {
      ch_node = node[i];
      if (ch_node != NULL)
	{
	  CH_NODEid(ch_node) = ID_NO_SYMBOL;
	  if (CH_NODEnext(ch_node) != NULL)
	    clean_ch_node(CH_NODEnext(ch_node));
	}
    }
  return(*node);
}
#endif /* SLIM_FST */

CH_NODEptr make_ch_node(id_type id)
{
  CH_NODEptr ch_node;
  
  ch_node = (CH_NODEptr) cell_alloc(CH_NODE_HEAP);
  
  if (ch_node == NULL)
    not_enough_memory("make_ch_node");
  CH_NODEid(ch_node) = id;
  CH_NODEnext(ch_node) = NULL;
  
  return (ch_node);
}



static CH_NODEptr *make_ch_node_next(void)
{
  CH_NODEptr *node;

  node = (CH_NODEptr *) cell_alloc(CH_VECTOR_HEAP);
  if (node == NULL)
    not_enough_memory("make_ch_node_next");

  /* When CELL_ALLOC returns a cell from the free list,
     the initial bytes of the object may contain the
     address of the next free cell. Always clear them.
     */
  
  *node = (void *) NULL;
  
  return(node);
}



static CH_NODEptr continue_branch(CH_NODEptr *ch_node, unsigned char i)
{
  /* Returns the next parse tree node. If such a node does
     not yet exist, it is created and place in the
     current node.
     */
     
  CH_NODEptr next;

  next = ch_node[i];
  if (next == NULL)
    {
      next = make_ch_node(ID_NO_SYMBOL);
      ch_node[i] = next;
    }

  return(next);
}



static CH_NODEptr *next_vector(CH_NODEptr ch_node)
{
  CH_NODEptr *next = CH_NODEnext(ch_node);

  if (next == NULL)
    {
      next = make_ch_node_next();
      CH_NODEnext(ch_node) = next;
    }
  return(next);
}


static char short2hex(short i)
{
  char c = 0;

  if (i < 10)
    c = '0' + i;
  else if (i < 16)
    c = 'A' + (i - 10);
  else
    handle_error("Invalid hex number", "SHORT2HEX", i);
  
  return c;
}
 

void add_symbol_to_parse_table(PARSE_TBL parse_table, int id)
{
  /* Don't add print names of reserved symbols into the
     parse table. */

  char str[5];
  FAT_CHAR fs[2];
  int i;
  CH_NODEptr ch_node = NULL, *next = PARSE_TBL_TOP_NODE(parse_table);
  FAT_STR name = LABEL_atom(id_to_label(id));
  CHARNUM(&fs[1]) = CHARSET(&fs[1]) = 0;

  switch (id)
    {
    case 0:			/* EPSILON */
    case 1:			/* OTHER */
      break;
    default:
      /* If name has more than one character it is a multichar
	 symbol. Multichar flag is also set to 1 below if we
	 encounter a single non-ASCII symbol in UTF-8 mode
	 or non-ISO-8859-1 symbol in ISO-8859-1 mode.
      */

      if (CHARNUM(name+1) != 0 || CHARSET(name+1) != 0)
	PARSE_TBL_MULTICHAR(parse_table) = TRUE;

      while (!(CHARNUM(name) == 0 && CHARSET(name) == 0))
	{
	  if (next == NULL)
	    {
	      next = make_ch_node_next();
	      CH_NODEnext(ch_node) = next;
	    }

	  /* ASCII characters up to 128 are added to the table
	     as such in both UTF-8 and ISO-8859-1 modes.
	     Characters in the 128-255 range are entered
	     as such in ISO-8859 mode and converted to
	     two characters in UTF-8 mode. UCS-2 characters
	     beyond 255 are conveted to UTF-8 sequences in
	     UTF-8 mode and to \uXXXX format in ISO-8859-1 mode. */

	  if (CHARSET(name) == 0 && CHARNUM(name) < 128)
	    ch_node = continue_branch(next, CHARNUM(name));
	  else if (IS_UTF8_MODE)
	    {
	      /* Convert to UTF-8 */
	      CHARSET(fs) = CHARSET(name);
	      CHARNUM(fs) = CHARNUM(name);
	      utf8_fat_to_thin((FAT_STR)fs, (char *) str);
	      i = 0;
	      ch_node = continue_branch(next, str[i++]);
	      while (str[i])
		ch_node = continue_branch(next_vector(ch_node), str[i++]);
	      PARSE_TBL_MULTICHAR(parse_table) = TRUE;
	    }
	  else if (CHARSET(name) == 0)
	    ch_node = continue_branch(next, CHARNUM(name));
	  else
	    {
	      str[0] = short2hex((short)(CHARSET(name) >> 4));
	      str[1] = short2hex((short)(CHARSET(name) & 15));
	      str[2] = short2hex((short)(CHARNUM(name) >> 4));
	      str[3] = short2hex((short)(CHARNUM(name) & 15));
	      str[4] = '\0';
	      
	      ch_node = continue_branch(next, (unsigned char) '\\');
	      ch_node = continue_branch(next_vector(ch_node),
					(unsigned char) 'u');
	      ch_node = continue_branch(next_vector(ch_node),
					(unsigned char) str[0]);
	      ch_node = continue_branch(next_vector(ch_node),
					(unsigned char) str[1]);
	      ch_node = continue_branch(next_vector(ch_node),
					(unsigned char) str[2]);
	      ch_node = continue_branch(next_vector(ch_node),
					(unsigned char) str[3]);
	      PARSE_TBL_MULTICHAR(parse_table) = TRUE;
	    }

	  next = CH_NODEnext(ch_node);

	  name++;
	}

      if (ch_node)
	CH_NODEid(ch_node) = id;
      else
	fsm_printf("*** Warning: Symbol %d is an empty string.\n", id);

      break;
    }
}
 

static void augment_parse_table(ALPHABETptr alph, PARSE_TBL parse_table, int side)
{ 
  /* The alphabet ALPH must be in binary format */

  int i, len = ALPH_len(alph);
  
  for (i = 0; i < len; i++)
    if (ALPH_item(alph, i) == 1 && i != EPSILON && i != OTHER &&
	i != ALTCHAIN_SYMBOL &&	i != DET_EPSILON)
      {
	if (LABEL_arity(id_to_label(i)) == 1)
	  add_symbol_to_parse_table(parse_table, i);
	else
	  /* tuple--get the id for the lexical side symbol */
	  switch (side)
	    {
	    case UPPER:
	      add_symbol_to_parse_table(parse_table, (int)upper_id((uint16)i));
	      break;
	    case LOWER:
	      add_symbol_to_parse_table(parse_table, (int)lower_id((uint16)i));
	      break;
	    default:
	      /* BOTH */
	      add_symbol_to_parse_table(parse_table, (int)upper_id((uint16)i));
	      add_symbol_to_parse_table(parse_table, (int)lower_id((uint16)i));
	      break;
	    }
      }
}


PARSE_TBL create_parse_table(NETptr net, ALPHABETptr alphp, int side)
{
  /*****
   * pass in either a non-null NETptr (lookup or lookdown,
   * involving a single net) xor a non-null ALPHABETptr (for test_rules()
   * which involves a net for each rule.)
   * If SIDE is UPPER, only upper symbols of fstpairs are included;
   * if SIDE is LOWER, only lower symbols are included; if SIDE is BOTH,
   * all symbols are included.
   *
   * the resulting parse table is used by parse_string() to convert an input
   * string of alphabetic characters into a string of upper or lower
   * (as necessary) label_ids, terminated by ID_EOS.
   ******/

  /* Returns a parse table consisting of an int that indicates whether
     the parse table is for UTF-8 encoding and the top level CH_NODE,
     a pointer to a 256 place array of CH_NODEptrs. The array maps
     characters of the symbols in the net's sigma to CH_NODEs, excluding
     EPSILON and OTHER.
     
     The flag PARSE_TBL_CHAR_ENC(parse_table) indicates whether the parse
     table is made for UTF-8 or ISO-8859-1 encoding.

     The flag PARSE_TBL_MULTICHAR(parse table) indicates whether the
     parse table contains symbols that are encoded by more that one
     8-bit character. In ISO-8859-1 parse tables this flag is TRUE
     iff there are multicharacter symbols. In UTF-8 parse tables
     the flag is TRUE iff either there are multicharacter or single
     non-ASCII symbols, that is, characters in the 128-255 range.

     A CH_NODE is a record with two slots: ID and NEXT.  If the character
     string up to that point is a symbol in the net, the ID slot contains
     the id of the symbol, otherwise the value ID_NO_SYMBOL. 
     If the string is the initial substring of a longer symbol name, the 
     NEXT slot contains another 256 place array of CH_NODEptrs, otherwise 
     the NEXT slot is NULL.
     
     Note: Actually symbol names are fat characters.  Here we are only
     considering the CHARNUM half of the fat character.  This needs to be
     changed for nets that really have 16 bit characters.
     */
  
  ALPHABETptr syms;
  int len;
  PARSE_TBL parse_table = NULL;
/*  FAT_STR name; */
  int label_vector_p;
  
  /* added by KRB */
  if (net)
    /* if a non-null net was passed in, for lookup or lookdown */
    syms = sigma(net);
  else
    /* an alphabet was passed in, for use with test_rules() */
    syms = alphp;

  /* If the alphabet is in LABEL_VECTOR format, remeber to convert
     back just in case it matters. For this computation we need
     binary labels.
     */
       
  if (ALPH_type(syms) == LABEL_VECTOR)
    {
      label_vector_p = TRUE;
      syms = label_to_binary(syms);
    }
  else
    label_vector_p = FALSE;
        
  len = ALPH_len(syms);
  /* end additions by KRB */
  
  parse_table = (PARSE_TBL) cell_alloc(PARSE_TBL_HEAP);
  
  if (IS_UTF8_MODE)
    PARSE_TBL_CHAR_ENC(parse_table) = CHAR_ENC_UTF_8;
  else
    PARSE_TBL_CHAR_ENC(parse_table) = CHAR_ENC_ISO_8859_1;

  PARSE_TBL_TOP_NODE(parse_table) = make_ch_node_next();
  
  /* Initially set to FALSE, reset in augment parse_table. */

  PARSE_TBL_MULTICHAR(parse_table) = FALSE;

  augment_parse_table(syms, parse_table, side);

  /* Fill the EPSILON slot of the parse table. Because '\0' marks the end
     of string, this node can be accessed only if another character
     (typically '0') is mapped to '\0' by PARSE_STRING.
     */
  
  PARSE_TBL_TOP_NODE(parse_table)[EPSILON] = make_ch_node(EPSILON);
  
  /* If an alphabet was passed in as argument in LABEL_VECTOR format,
     we convert it back to that format.
     */
  if (alphp && label_vector_p)
    binary_to_label(alphp);
    
  return (parse_table);
}


void update_match_tables(void)
/***
 *
 * I split this out of make_lookup_tables() because
 * sometimes (i.e. in LEXC check_all()), you need
 * to generate match tables, but not a parse table.
 * %%TSY, 2/24/93
 *
 * There are two match tables, UPPER_MATCH and LOWER_MATCH.
 * For every label in the environment, these map from the
 * label id to the upper and lower portions respectively.
 * Thus, for an atomic label, id = UPPER_MATCH[id] = LOWER_MATCH[id].
 * For a tuple, UPPER_MATCH[id] is the id of the upper atom,
 * and LOWER_MATCH[id] is the id of the lower atom.  Note that the match
 * tables depend only on the global label space, not any particular
 * network or parse table.  Since the global labels never change once
 * they are created, a call to update_match_tables() simply resizes 
 * updates the match tables if new labels have been created since
 * the last call.  The first call creates the tables.
 *
 ***/
{
  static uint16 prev_count = 0;
  uint16 elem_count = LIM_id_count(LABEL_MAP);
  id_type i, lower, upper;
  id_type *tuple_ids;
  LABELptr label;
  
  if (UPPER_MATCH == NULL)
	  prev_count = 0;

  /* return if the label space hasn't grown */
  if (elem_count == prev_count)
    return;
  
  if (UPPER_MATCH == NULL)
    {
      UPPER_MATCH = (id_type *) malloc((size_t) elem_count * sizeof(id_type));
      LOWER_MATCH = (id_type *) malloc((size_t) elem_count * sizeof(id_type));
    }
  else
    {
      UPPER_MATCH =
	(id_type *) realloc((void *) UPPER_MATCH,
			  (size_t) elem_count * sizeof(id_type));
      LOWER_MATCH =
	(id_type *) realloc((void *) LOWER_MATCH,
			  (size_t) elem_count * sizeof(id_type));
    }
  
  if (UPPER_MATCH == NULL || LOWER_MATCH == NULL)
    not_enough_memory("MAKE_LOOKUP_TABLES");
  
  for  (i = prev_count; i < elem_count; i++)
    {
      label = id_to_label(i);
      
      switch (LABEL_arity(label))
	{
	case 1:
	  upper = i;
	  lower = i;
	  break;
	default:
	  tuple_ids = TUPLE_labels(LABEL_tuple(label));
	  upper = tuple_ids[UPPER];
	  lower = tuple_ids[LOWER];
	  break;
	}
      
      UPPER_MATCH[i] = upper;
      LOWER_MATCH[i] = lower;
    }
  
  prev_count = elem_count;
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

id_type upper_side_id(id_type id)
{
  return(UPPER_MATCH[id]);
}

id_type lower_side_id(id_type id)
{
  return(LOWER_MATCH[id]);
}
#endif /* SLIM_FST */


PARSE_TBL make_parse_table(NETptr net, ALPHABETptr alphp, int side)
  /******
   *
   * Pass in either a non-null NETptr (for lookup/lookdown
   * with a single rule) or a non-null ALPHABETptr (for test-rules())
   * The value of SIDE should be UPPER, LOWER, or BOTH depending
   * to indicate which side of fstpairs should be included in the table.
   *
   * Now returns pointer to a new PARSE_TBL, rather than setting global
   * variable.  Caller must now maintain a variable.  %%%TSY 3/8/93
   * 
   * The parse table is used by parse_string() to convert the input alphabetic
   * string into a string of upper or lower (as necessary) label_ids,
   * terminated by a ID_EOS.
   *
   ******/
{
  PARSE_TBL parse_table;
  
  update_match_tables();
  parse_table = create_parse_table(net, alphp, side);
  
  if (net)
    {
      if (side == UPPER)
        NET_upper_parse_table(net) = parse_table;
      else
        NET_lower_parse_table(net) = parse_table;
    }
    
  return (parse_table);
}

static int wrong_char_encoding_p(PARSE_TBL parse_table)
{
    if (IS_UTF8_MODE)
        return(PARSE_TBL_CHAR_ENC(parse_table) != CHAR_ENC_UTF_8);
    else
        return(PARSE_TBL_CHAR_ENC(parse_table) != CHAR_ENC_ISO_8859_1);
}


PARSE_TBL get_parse_table(NETptr net, int side)
{
  if (side == UPPER)
    {
      if (!NET_upper_parse_table(net))
	make_parse_table(net, NULL, UPPER);
      else if (wrong_char_encoding_p(NET_upper_parse_table(net)))
	{
	  free_parse_table(NET_upper_parse_table(net));
	  make_parse_table(net, NULL, UPPER);
	}
      return(NET_upper_parse_table(net));
    }
  else
    {
      if (!NET_lower_parse_table(net))
	make_parse_table(net, NULL, LOWER);
      else if (wrong_char_encoding_p(NET_lower_parse_table(net)))
	{
	  free_parse_table(NET_lower_parse_table(net));
	  make_parse_table(net, NULL, LOWER);
	}
	  	
      return(NET_lower_parse_table(net));
    }
}


PARSE_TBL get_tokenizer_parse_table(NETptr net, ALPHABETptr lower_sig)
{
  /* A variant of get_parse_table(). The lower_sig should contain
     multicharacter symbols only if they appear on the LOWER (input)
     side of the network. The standard get_parse_table() includes
     all the symbols in the sigma.
  */

  if (!NET_lower_parse_table(net))
    NET_lower_parse_table(net) = make_parse_table(NULL, lower_sig, LOWER);
  else if (wrong_char_encoding_p(NET_lower_parse_table(net)))
    {
      free_parse_table(NET_lower_parse_table(net));
      NET_lower_parse_table(net) = make_parse_table(NULL, lower_sig, LOWER);
    }
	  	
  return(NET_lower_parse_table(net));
}

/*****************************************
 *                                       *
 *  RANGE arrays
 *                                       *
 *****************************************/
 
 static void collect_from_alph(ALPHABETptr alph, int16 *atom_ids,
 	int *max, int *count, int side)
{
  uint16 i;
  id_type id;
  LABELptr label;
 	
  for (i = 0; i < ALPH_len(alph); i++)
    {
      /* Sets id to the item if the alphabet is in form of
	 a label vector or to the position number if the
	 alphabet is of type BINARY_VECTOR.
      */
      if (ALPH_type(alph) == LABEL_VECTOR)
      	id = ALPH_item(alph, i);
      else if (ALPH_item(alph, i) != 0)
      	id = i;
      else
      	continue;
      	
      if (id > *max)
	*max = id;
      label = id_to_label(id);
      switch(LABEL_arity(label))
	{
	case (1):		/* label is an identity map, i.e. a:a */
	  break;
	default:		/* label is a tuple */
	  id = TUPLE_labels(LABEL_tuple(label))[side];
	  break;
	}

      if (id == ALTCHAIN_SYMBOL || flag_diacritic_p(id))
	atom_ids[EPSILON]++;
      else
	atom_ids[id]++;

      (*count)++;
    }
}


static int16 *collect_atoms(NETptr net, int side, int *total, int *max_id)
{
  /*
   * 
   * This function collects ids of all atoms in one alphabet of NET,
   * as indicated by SIDE (UPPER or LOWER).
   *
   * Returns a vector indexing over label id's which indicates how many
   * labels there are containing the symbol in question on the relevant side.
   * For example, if side is LOWER, then atom_ids[50] contains the total
   * number of labels that have symbol 50 on the lower side, that is,
   * labels such as  50, 51:50, 60:50, etc.
   * Other labels (tuples, atomic labels which do not occur in NET)
   * are marked 0.  
   *
   **********/

  int16 *atom_ids;
  
  atom_ids = (int16 *) calloc((size_t) LIM_id_count(LABEL_MAP) * sizeof(int16),
			      (size_t) sizeof(int16));
  
  if (!(atom_ids))
    not_enough_memory("COLLECT_ATOMS");

  /* BEGIN - Insert bug fix - TG 98-10-15:
     Not only the "labels" but the "sigma" alphabet must be taken into
     consideration! Otherwise the next bad case can happen:
     There may be (spurious) symbols present in "sigma" but not in
     either side of a label symbol (that is, not in the "labels" alphabet).
     Now, such a character WILL be considered as part of the network -
     since it is in "sigma" - but the returned "atom_ids" and "max_id"
     value will ignore it. If 
     id_of_spurious_sigma_alphabet _element > id_of_max_label_element
     then we WILL have a problem later since the resulting atoms
     max_id value  will be incorrect.
     The solution is preprocessing the sigma (setting 0 to all places), but
     defining max to be the really biggest id value (in this net)
     irrespective of whether it is in the "labels" or the "sigma" alphabet.

     On the user level, it is recommended to perform a compact_sigma
     operation and then this case can't happen.
  */

  if (!NET_sigma(net))
    NET_sigma(net)=sigma(net);
  label_to_binary(NET_sigma(net));
  *max_id = ALPH_len(NET_sigma(net));

  /* END - Insert bug fix - TG 98-10-15 */


  if (NET_reduced_labelset(net))
    collect_from_alph(get_unreduce_key(net), atom_ids, max_id, total, side);
  else
    collect_from_alph(NET_labels(net),  atom_ids, max_id, total, side);
  
  return (atom_ids);
}

void set_range(RANGEptr range, id_type id, int count)
{
  RANGE_id(range) = id;
  RANGE_len(range) = count;

  if (RANGE_items(range) == NULL)
      RANGE_items(range) = (id_type *) malloc(count * (size_t) sizeof(id_type));
  
  if (!(RANGE_items(range)))
    not_enough_memory("SET_RANGE");
}



static void fill_range(ALPHABETptr alph, RANGEptr range,
	int side, int *count)
{
  int i;
  
  id_type id, symbol_id, same_side_id, *items;

  items = RANGE_items(range);
  symbol_id = RANGE_id(range);

  /* ALTCHAIN_SYMBOL and flag diacritics are subsumed under EPSILON */

  if (symbol_id == ALTCHAIN_SYMBOL || flag_diacritic_p(symbol_id))
    return;
    
  for (i = 0; i < ALPH_len(alph); i++)
    {
      if (ALPH_type(alph) == LABEL_VECTOR)
      	id = ALPH_item(alph, i);
      else if (ALPH_item(alph, i) != 0)
      	id = i;
      else
      	continue;
      
      if (side == UPPER)
	same_side_id = UPPER_MATCH[id];
      else
	same_side_id = LOWER_MATCH[id];
      
      if (same_side_id == symbol_id)
	{
	  items[*count] = id;
	  (*count)++;
	}
      else if (symbol_id == EPSILON && 
	       (same_side_id == ALTCHAIN_SYMBOL ||
		flag_diacritic_p(same_side_id)))
	{
	  items[*count] = id;
	  (*count)++;
	}
    }
}
 

static void map_symbol_range(id_type symbol_id, int n, int side, NETptr net,
		      RANGEptr range)
   /****
    * N is the number of labels in the LABEL_TABLE containing SYMBOL_ID
    * on the SIDE.
    * MAP is an array of integer n-tuples type [n, R1...Rn], where N is 
    * the length of the tuple and R1...Rn are label id's that appear 
    * opposite to SYMBOL_ID in those N labels. If the net is reduced, we
    * collect the labels from the net's unreduce_key, otherwise from the
    * nets labels.
    ****/
{
  int j = 0; 
  
  set_range(range, symbol_id, n);
  
  if (NET_reduced_labelset(net))
    fill_range(get_unreduce_key(net), range, side, &j);
  else
    fill_range(NET_labels(net), range, side, &j);


  if (j < n)
    {
      if (n == 1 && (symbol_id == OTHER || symbol_id == REPEAT_SYMBOL))
	RANGE_item(range, 0) = symbol_id;
      else
	{
	  print_label(symbol_id, stderr, DO_ESCAPE);
	  fsm_fprintf(stderr, "\n");
	  handle_error("Missing label", "map_symbol_range", symbol_id);
	}
    }
}


RANGEptr make_range_map(NETptr net, int input_side)
{
  int i, total = 0, max_id = 0;
  int16 *atom_ids= NULL;
  RANGEptr range_map= NULL;

  /* Make sure that UPPER_MATCH and LOWER_MATCH are up to date. */
  update_match_tables();
      
  if (input_side == UPPER)
      range_map= NET_downrange_map(net);
  else
      range_map= NET_uprange_map(net);
  
  if (range_map == NULL)
  {
      atom_ids= collect_atoms(net, input_side, &total, &max_id);
  
      range_map= (RANGEptr) calloc((size_t) max_id + 1,
                                   (size_t) sizeof(RANGEtype));

      if (!range_map)
          not_enough_memory("MAKE_RANGE_MAP");
  
      NET_range_len(net) = max_id + 1;

      if (input_side == UPPER)
          NET_downrange_map(net) = range_map;
      else
          NET_uprange_map(net) = range_map;

      for (i = 0; i < LIM_id_count(LABEL_MAP); i++)
          if (atom_ids[i] != 0)   
              map_symbol_range((id_type) i, atom_ids[i], input_side, net,
                               range_map + i);

      free((void *) atom_ids);
  }

  /* Fill the slot for OTHER in the range map if it is not in the
     alphabet already. parse_word() maps unknown characters
     in the alphabet to OTHER. */
  
  if (RANGE_len(range_map + OTHER) == 0)
  {
      set_range(range_map + OTHER, OTHER, 1);
      RANGE_item (range_map + OTHER, 0) = OTHER;
      /* fill the only item of OTHER with OTHER; AK 27/03/96 */
  }

  return(range_map);
}



static RANGE_VECTORptr make_range_vector(int len)
{
  RANGE_VECTORptr vector;
  RANGEptr range;

  vector = malloc(sizeof(RANGE_VECTORtype));

  if (!vector)
    not_enough_memory("MAKE_RANGE_VECTOR");

  range = (RANGEptr) calloc((size_t) len, sizeof(RANGEtype));

  if (!range)
    not_enough_memory("MAKE_RANGE_VECTOR");

  RANGE_VECTOR_len(vector) = len;
  RANGE_VECTOR_vector(vector) = range;

  return(vector);
}


/* For labelSet reduced networks */


static void initialize_range_record(RANGEptr vect, id_type id)
{

  /* Allocate space for the vector. Reset the length counter to 0. */

  RANGE_items(vect) = (id_type *)
    malloc((size_t) RANGE_len(vect) * sizeof(id_type));

  if (!RANGE_items(vect))
    not_enough_memory("MAKE_MATCH_TABLE");

  RANGE_id(vect) = id;
  RANGE_len(vect) = 0;
}


static MATCH_TABLEptr make_match_table(NETptr net, int side)
{
  /* Assumes that NET has a reduced label set. Returns an object
     that relates the labels that remain in the net to the symbols
     in the corresponging equivalence class. The match table
     contains three components: (1) the length of two vectors
     (long enough to be indexed by all label ids that remain
     in the net); (2) a table that contains for each label J
     a record, another vector, that associates each symbol K of
     input side of the network with all the symbols in J's
     equivalence class that have K on the input side; (3)
     a vector that contains for each label J the complete
     equivalence class of the labels represented by J.

     (2) is needed  in apply_reduced_network() for ordinary input symbols;
     (3) is needed to interpret OTHER as input symbol.
  */

  id_type i;
  id_type id, j, k, sym_max;
  ALPHABETptr unreduce_key = get_unreduce_key(net);
  ALPHABETptr labels = binary_to_label(NET_labels(net));
  RANGEptr range, class;
  RANGE_VECTORptr *match_vector;
  RANGEptr class_vector;
  RANGE_VECTORptr vector;
  MATCH_TABLEptr match_table;
	
  if (!NET_reduced_labelset(net))
    prog_error("make_match_table", 
	       "The nework does not have a reduced labelset.\n");
	   
  j = max_label_item(ALPH_items(labels), ALPH_len(labels));
	
  sym_max = 0;
	
  /* Find the largest symbol id on the input side */
	
  for (i = 0; i < ALPH_len(unreduce_key); i++)
    {
      id = ALPH_item(unreduce_key, i);
      if (id != 0)
	{
	  if (side == UPPER)
	    id = upper_id(i);
	  else
	    id = lower_id(i);
			  
	  if (id > sym_max)
	    sym_max = id;
	}
    } 
	
  /* Make the structures. */

  match_table = (MATCH_TABLEptr) malloc(sizeof(MATCH_TABLEtype));

  match_vector = 
    (RANGE_VECTORptr *) calloc((size_t) (j + 1), sizeof(RANGE_VECTORptr));
  class_vector = 
    (RANGEptr) calloc((size_t) (j + 1), sizeof(RANGEtype));

  if (!(match_table && match_vector && class_vector))
    not_enough_memory("MAKE_MATCH_TABLE");
	
  MATCH_TABLE_len(match_table) = j + 1;
  MATCH_TABLE_table(match_table) = match_vector;
  MATCH_TABLE_class(match_table) = class_vector;

  /* Reserve space for EPSILON. */

  vector = make_range_vector(sym_max+1);
  match_vector[EPSILON] = vector;


  /* Reserve space for ALTCHAIN if the network has a reduced
   labelset but ALTCHAIN does not appear in the network because
   arcs are shared.
   */

  if (!sigma_member(sigma(net), ALTCHAIN_SYMBOL) &&
      get_value(net, EQV_VECTOR_ATTR))
    {
      vector = make_range_vector(sym_max+1);
      match_vector[ALTCHAIN_SYMBOL] = vector;
    }

  /* Reserve space for other symbols. */

  for (i = 0; i < ALPH_len(labels); i++)
    {
      vector = make_range_vector(sym_max+1);
      match_vector[ALPH_item(labels, i)] = vector;
    }
	

  /* Count the number of labels to be stored at each
     match_match_vector[j] + k position and each
     class_vector[j] position.
  */
	   
  for (i = 0; i < ALPH_len(unreduce_key); i++)
    {
      j = ALPH_item(unreduce_key, i);
		
      if (j != 0)
	{
	  class = class_vector + j;
	  RANGE_len(class)++;
	  
	  if (side == UPPER)
	    k = upper_id(i);
	  else
	    k = lower_id(i);
		    
	  if (k == ALTCHAIN_SYMBOL || flag_diacritic_p(k))
	    k = EPSILON;
		 
	  range = RANGE_VECTOR_vector(match_vector[j]) + k;
	  RANGE_len(range)++;
	}
	
    }
	
  /* Fill the match and class vectors */
	
  for (i = 0; i < ALPH_len(unreduce_key); i++)
    {
      j = ALPH_item(unreduce_key, i);
		
      if (j != 0)
	{
	  class = class_vector + j;

	  if (!RANGE_items(class))
	    initialize_range_record(class, j);

	  /* Add class member */

	  RANGE_item(class, RANGE_len(class)++) = i;

	  if (side == UPPER)
	    k = upper_id(i);
	  else
	    k = lower_id(i);
		    
	  if (k == ALTCHAIN_SYMBOL || flag_diacritic_p(k))
	    k = EPSILON;
		   
	  range = RANGE_VECTOR_vector(match_vector[j]) + k;

	  if (!RANGE_items(range))
	    initialize_range_record(range, k);

	  /* Add the label */
	  RANGE_item(range, RANGE_len(range)++) = i;
	}
	
    }
	
  return(match_table);
} /* make_match_table */


MATCH_TABLEptr get_match_table(NETptr net, int side)
{
  if (side == UPPER)
    {
      if (!NET_downmatch_table(net))
	NET_downmatch_table(net) = make_match_table(net, UPPER);

      return(NET_downmatch_table(net));
    }
  else		
    {
      if (!NET_upmatch_table(net))
	NET_upmatch_table(net) = make_match_table(net, LOWER);
			
      return(NET_upmatch_table(net));
    }

} /* get_match_table */



/***************************************
 *                                     *
 *   Parse string to labels            *
 *                                     *
 ***************************************/

static void intern_one_character_symbol(id_type *id, char c)
{
  char sym[2];

  sym[0] = c;
  sym[1] = '\0';

  FST_intern_literal(id, sym);
}


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int parse_string (char *word, PARSE_TBL parse_table, int zero_to_eps_p,
		  int ignore_white_space_p, int escape_p, int report_error_p)
     /*****
      *
      * Converts a string to a sequence of label ids using PARSE_TABLE;
      * storing the labels in INPUT_SEQ. ID_EOS marks the end of the sequence.
      * Returns NO_ERROR if the parse is successful; if the string cannot be
      * parsed the returned value is ERROR;
      *
      *****/
{
  CH_NODEptr ch_node, *next;
  char *str, *symbol;
  unsigned char char_code ;
  id_type *path = INPUT_SEQ;
  UTF16 utf16;       /* for interning possible unknown symbol in UTF8 mode */
  FAT_CHAR fatStr[2];/* for interning possible unknown symbol in UTF8 mode */

  while (*word != '\0')
    {
      next = PARSE_TBL_TOP_NODE(parse_table);;
      *path = ID_NO_SYMBOL;
      str = word;
      
      /* If IGNORE_WHITE_SPACE is true, strip off initial blanks and tabs */
      
      if (ignore_white_space_p)
        while (myisspace(*str))
	  str++;
      
      if (*str == '\0')
	break;
      else if (escape_p && *str == '%' && *(str + 1) != '\0')
	str++;
      else if (zero_to_eps_p && *str == '0')
	*str = (char)EPSILON;  /* a.k.a. '\0' */

      symbol = str;

      do {
	char_code = (unsigned char) *str++ ;
	
	ch_node = next[char_code];
	if (ch_node == NULL)
	  break;
	
	if (CH_NODEid(ch_node) != ID_NO_SYMBOL)
	  {
	    word = str;
	    *path = CH_NODEid(ch_node);
	  }
	next = CH_NODEnext(ch_node);

	if (escape_p && next && *str == '%' && *(str + 1) != '\0')
	  str++;

      } while(next != NULL);
      
      if (*path == ID_NO_SYMBOL)
	{
            /* No symbol starting with this input in the parse table
               Intern the first character of the input string and
               store the symbol into the table. In UTF-8 mode, the
               unknown symbol may consist of more than one 8 bit
               character. Advance word accordingly. */

            if (IS_UTF8_MODE)
	    {
                ConversionResult res= ConvertUTF8toUTF16_1(
                                              ((const UTF8**)&symbol),
                                              (UTF8*)(symbol+strlen(symbol)),
                                              &utf16, lenientConversion);

                if (res != conversionOK)
                {
                    /* The input word could not be parsed as utf8:
                       maybe the input is iso_8859_1 encoded ?
                       Anyway, the string 'word' can't be parsed;
                       we fill INPUT_SEQ with the end_of_string symbol
                       and return an error code: */
                    INPUT_SEQ[0]= ID_NO_SYMBOL;
                    return(ERROR);
                }
                else
                {
                    /* Convert to a fat_string while taking care of
                       endianness: */
                    ConvertUTF16_1toFatStr(utf16, fatStr);

                    /* Get the id of that symbol, (and possibly intern it) */
                    intern_atomic_label(path, fatStr);

                    /* Go-ahead in the 'word' string: */
                    word = symbol;
                }
            }
            else
            { /* iso_8859_1 encoding: */
                intern_one_character_symbol(path, *symbol);
                word = symbol + 1;
            }
            /* CP 25/04/2003:
               Why adding this unknown symbol to the parse_table of the
               network ?  */
            add_symbol_to_parse_table(parse_table, *path);
	} /*  (*path == ID_NO_SYMBOL) */
          
      path++;
    } /*   while (*word != '\0') */
  *path = ID_EOS;
  return(0);
}

int utf8_parse_string (char *word, PARSE_TBL parse_table, int zero_to_eps_p,
		  int ignore_white_space_p, int escape_p, int report_error_p)
     /*****
      *
      * Converts a UTF-8 string to a sequence of label ids using PARSE_TABLE;
      * storing the labels in INPUT_SEQ. ID_EOS marks the end of the sequence.
      * Returns NO_ERROR if the parse is successful; if the string cannot be
      * parsed the returned value is ERROR;
      *
      *****/
{
  CH_NODEptr ch_node, *next;
  char *str, *symbol;
  unsigned char char_code ;
  id_type *path = INPUT_SEQ;

  UTF16 utf16;
  FAT_CHAR fatStr[3];
  ConversionResult conversionResult;
  id_type id = ID_NO_SYMBOL;


  while (*word != '\0')
    {
      next = PARSE_TBL_TOP_NODE(parse_table);
      *path = ID_NO_SYMBOL;
      str = word;

      /* If IGNORE_WHITE_SPACE is true, strip off initial blanks and tabs */

      if (ignore_white_space_p)
	while (myisspace(*str))
	  str++;

      if (*str == '\0')
	break;
      else if (escape_p && *str == '%' && *(str + 1) != '\0')
	str++;
      else if (zero_to_eps_p && *str == '0')
	*str = (char)EPSILON;  /* a.k.a. '\0' */

      symbol = str;
      if (trailingBytesForUTF8[(int) *str])
	{
	  /* Collect the first UTF-8 char. It's a non-ASCII char, then it
             cannot be part of a multichar symbol, just append its ID to
             path. */
	  conversionResult =
	    ConvertUTF8toUTF16_1_length_unknown (((const UTF8 **) &str),
						 &utf16, lenientConversion);
		
	  /* Non-ASCII chars are forbidden into multicharacter symbols	*/
	  ConvertUTF16_1toFatStr (utf16, fatStr);
	  intern_atomic_label(&id, fatStr);
	  *path = id;
	  word = str;
	}
      else 
	{
	  do 
	    {

	      char_code = (unsigned char) *str ;
              /* If the next character is the beginning of a non-ASCII char,
                 the symbol is ended */
	      if ( char_code > 127 )
		{
		  /* Not an ASCII char */
		  /* Leave it for next time */
		  break;
		}
	      str++;

	      ch_node = next[char_code];
	      if (ch_node == NULL)
		break; /* End of multicharacter symbol */

	      if (CH_NODEid(ch_node) != ID_NO_SYMBOL)
		{
		  word = str;
		  *path = CH_NODEid(ch_node);
		}
	      next = CH_NODEnext(ch_node);

	      if (escape_p && next && *str == '%' && *(str + 1) != '\0')
		str++;

	    } while(next != NULL);

	  if (*path == ID_NO_SYMBOL)
	    {
                /* No symbol starting with this input in the parse table
                   Intern the first character of the input string.
                */
	      intern_one_character_symbol(path, *symbol);
	      word = symbol + 1;
	    }
	}
      path++;
    } /* while (*word != '\0') */
	*path = ID_EOS;
	return(0);
}

void report_parse_error(char *word, int pos)
{
  /* Print up to POS characters of WORD. */

  fsm_fprintf(stderr, "Unknown symbol: '");

  while (pos > 0) {
    fputc(*word++, stderr);
    pos--;
  }

  fsm_fprintf(stderr, "'\n");
}
#endif /* SLIM_FST */

int symbol_from_string (id_type *id, char *input, char **remain,
			PARSE_TBL parse_table, int zero_to_eps_p,
			int ignore_white_space_p, int escape_p)
     /*****
      *
      * Sets id ID the next symbol from input using PARSE_TABLE. The value at
      * ID is OTHER if the parse is unsuccessful and ID_NO_SYMBOL when the
      * string has been exhausted. The unprocessed part of the input string
      * is assigned to REMAIN. At least one character from the input will
      * be processed unless the input is NULL or an empty string.
      * If ZERO_TO_EPS_P is true, the numeral 0 is parsed as EPSILON unless
      * it is preceded with a % and ESCAPE_P is true. ESCAPE_P makes the
      * percent sign to an escape character. If IGNORE_WHITE_SPACE_P is true,
      * trailing blanks, tabs and linefeeds are ignored.
      *
      *****/
{
  CH_NODEptr ch_node, *next = PARSE_TBL_TOP_NODE(parse_table);
  unsigned char char_code ;
  char *in = input;
  UTF16 utf16;
  FAT_CHAR fatStr[2];
  ConversionResult conversionResult;  
  *id =ID_NO_SYMBOL;
  
  if (!in || *in == '\0')
    {
      *remain = in;
      return(ERR_BAD_INPUT);
    }
     
  /* At least one character will be processed from the input. */

  *remain = in;


  /* If ZERO_TO_EPS_P is true, '0' is parsed as EPSILON */
          
  if (zero_to_eps_p && *in == '0')
    {
      *id = EPSILON;
      *remain=++in;
    }    
  else
    {
      /* If ESCAPE_P is true, % acts as an escape character */
       
      if (escape_p && *in == '%' && *(in + 1) != '\0')
	in++;
		
      if (*in != '\0')
	{
	  do {
	    char_code = (unsigned char) *in++ ;
	
	    ch_node = next[char_code];
	    if (ch_node == NULL)
	      break;
	
	    if (CH_NODEid(ch_node) != ID_NO_SYMBOL)
	      {
		*id = CH_NODEid(ch_node);
		*remain = in;
	      }
	    next = CH_NODEnext(ch_node);
	  } while(next != NULL);
	}
    }

  if (*id == ID_NO_SYMBOL)
    {
      
      char_code = (unsigned char) **remain;

      if (IS_UTF8_MODE)
	{     
            conversionResult = ConvertUTF8toUTF16_1_length_unknown
	    ((const UTF8**) remain, &utf16, lenientConversion);

            ConvertUTF16_1toFatStr (utf16, fatStr);
	  
            intern_atomic_label(id, fatStr);

            if (conversionResult != conversionOK)
            {
                /* Conversion failed:
                   The input string could not be parsed as utf8:
                   maybe the input is iso_8859_1 encoded ?
                   Anyway, the next symbol can't be parsed correctly;
                   id should contain a dummy symbol though not
                   ID_NO_SYMBOL and hence we return an error code. */
                return(ERROR);
            }
	}
      else
	{
	  intern_char(id, char_code);
	  (*remain)++;
	}
    }

  if (ignore_white_space_p)
    while (myisspace(**remain))
      (*remain)++;

  return(NO_ERROR);
} /* symbol_from_string */


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

int pair_symbol_from_string (id_type *id, char *input, char **remain,
                             PARSE_TBL upper_parse_table, PARSE_TBL lower_parse_table,
                             int zero_to_eps_p, int ignore_white_space_p, int escape_p)
      /*****
      * This is a subroutine for the iy_add_path_from_line() function,
      * (see file iy-fs-ops.c):
      * The parameter INPUT is supposed to be a string of symbol pairs.
      * Identity pairs are specified by just a symbol, and non-identity
      * pairs are separated by ":".
      * Example:
      *         Input = fgk:lar:0
      *         the pair sequence is
      *                 <f:f> <g:g> <k:l> <a:a> <r:0>
      * Parameter ID is set to the next symbol PAIR from input using both
      * the UPPER_PARSE_TABLE and the LOWER_PARSE_TABLE of the fst.
      * It is based on the symbol_from_string() function.
      *****/
{

    id_type upper_id, lower_id;
    char *in = input;
    int error;


    error= symbol_from_string (&upper_id, in, remain, upper_parse_table, zero_to_eps_p,
                         ignore_white_space_p, escape_p);
    if (error != ERR_BAD_INPUT)
    {

        /* Upper symbol found or interned: we must look for ':' to know if
           there is a lower symbol to retrieve */
        if (**remain == ':')
        {
            /* Parsing of the lower symbol */
            (*remain)++;
            in=*remain;
            error= symbol_from_string (&lower_id, in, remain, lower_parse_table,
                                    zero_to_eps_p,ignore_white_space_p, escape_p);
            if (error != ERR_BAD_INPUT)
            {
      
                /* Retrieves the id of the symbol pair if it already exists, or
                   allocates a new tuple-label structure and intern it */
                make_pair_label(id, upper_id, lower_id);
                return(NO_ERROR);
            }
            else
            { /* error in the input string: symbol ':' should be followed by a
                 lower symbol; *id is equal to EPSILON */
                return(error);
            }
        }
        else
        { /* upper_id is implicitly an identity pair */
            *id=upper_id;
             return(NO_ERROR);
        }
    }
    else
    {
         /* No upper symbol found: there is an error in the input or the string
            is consumed: *id is equal to EPSILON */               
        return(error);
    }
} /* pair_symbol_from_string */




int symbol_from_stream (id_type *id, FILE *stream, PARSE_TBL parse_table,
	int zero_to_eps_p, int ignore_white_space_p, int escape_p)
     /*****
      *
      * Sets ID to the next symbol from STREAM using PARSE_TABLE.
      * The ID is ID_NO_SYMBOL if the parse is unsuccessful or the      
      * string has been exhausted.
      * If ZERO_TO_EPS_P is true, '0' is parsed as EPSILON. The special
      * meaning of zero can be turned off by prefixing it with % when
      * ESCACE_P is true.
      * If IGNORE_WHITE_SPACE is true, blanks, tabs and newlines are 
      * ignored unless they are prefixed with % and ESCAPE_P is true.
      * Returns the code NO_ERROR on success. Returns ERROR when an  
      * end-of-file or another error occurs. In that case ID is set 
      * to ID_NO_SYMBOL. If the error is an unparsable character in the 
      * input--a character for which there is no entry in the parse 
      * table it is directed to stderr. 
      *
      * N.B. This routine uses fgetpos() and fsetpos(). Consequently, it
      * cannot be used safely to read from the console stream if the
      * parse table contains multicharacter symbols.
      * Use next_symbol() to process symbols from the console.
      *  
      *
      *****/
{
#ifdef _WIN32
    typedef long fpos_t;
#endif
  fpos_t file_pos;
  CH_NODEptr ch_node, *next = PARSE_TBL_TOP_NODE(parse_table);
  unsigned char char_code = (unsigned char) fsm_fgetc(stream);
  unsigned char first_char;
  int err = NO_ERROR;
  int sym_len = 0, char_count = 0;
       
#ifdef __svr4__
  err = fgetpos(stream, &file_pos);    /* Not available on sunos. */
#else
  file_pos = ftell(stream);
#endif

  *id = ID_NO_SYMBOL;
   
  /* If IGNORE_WHITE_SPACE is true, strip off initial blanks and tabs */
     
  if (ignore_white_space_p)
    while (myisspace(char_code))
      char_code = (unsigned char) fsm_fgetc(stream);
      
  if (char_code == (unsigned char) EOF)
    return(ERROR);
	
  if (zero_to_eps_p && char_code == '0')
    {
      *id = EPSILON;
      return(NO_ERROR);
    }

  if (escape_p && char_code == '%')
    {
      unsigned char next = (unsigned char) fsm_fgetc(stream); /* YM, 2/26/96 : replaced unsigned short next with unsigned char next */
             
      if (next != (unsigned char) EOF)
	char_code = next;
    }

  first_char = char_code;

  do {
	
    char_count++;
	
    ch_node = next[char_code];

    if (ch_node == NULL)
	break;

    if (CH_NODEid(ch_node) != ID_NO_SYMBOL)
      {
	*id = CH_NODEid(ch_node);
	/* Found a symbol. Take note of the position
	   in the stream in case we need to reset
	   later.
	   */
#ifdef __svr4__
	err = fgetpos(stream, &file_pos);    /* Not available on sunos. */
#else
	file_pos = ftell(stream);
#endif
	sym_len = char_count;
      }
    next = CH_NODEnext(ch_node);
  } while( next != NULL &&
	  (char_code = (unsigned char) fsm_fgetc(stream)) != (unsigned char) EOF);
    
  if (*id == ID_NO_SYMBOL)
  {
    /* An unparsable symbol. Intern the first character of the
       input string. */
    intern_one_character_symbol(id, first_char);
    sym_len = 1;
    err = ERR_NOLABEL;
  }

  if (char_count > sym_len)
#ifdef __svr4__
    err = fsetpos(stream, &file_pos);
#else
    err = fseek(stream, file_pos, SEEK_SET);
#endif

  return(err);
}


int next_symbol (id_type *id, FILE *stream, PARSE_TBL parse_table,
	int zero_to_eps_p, int ignore_white_space_p, int escape_p)
{
  static char in_buffer[512], *line = NULL;
  int error, rest;
  
  /* Sets ID to the next symbol id from STREAM. Returns code NO_ERROR
     on success, and code ERROR when end-of-file or some other error 
     is encountered. 
     When an error occurs, the ID is set to ID_NO_SYMBOL. If the
     error is due to an unparsable symbol, the first character
     of the input is interned as such.
     
     in_buffer is a static buffer, line contains the unprocessed
     part of the input. When more than a half of the in_buffer
     has been processed, the rest is moved to the beginning
     of the buffer and the buffer is filled up to the capacity
     again.

     */
  
 
  if (!line || *line == '\0')
    {
      do {
	if (!(line = fsm_fgets(in_buffer, 512, stream)))
	  {
	    /* End of file or some other error. */
	    *id =ID_NO_SYMBOL;
	    return(ERROR);
	  }
	if (ignore_white_space_p)
	  while (myisspace(*line))
	    line++;
      } while (*line == '\0');
    }

  error = symbol_from_string(id, line, &line, parse_table, zero_to_eps_p, ignore_white_space_p, escape_p);

  if (error == ERR_BAD_INPUT)
    return(ERROR);

 if ((line-in_buffer) > 256)
    {
      rest = strlen(line);

      /* More than half of the input buffer has been used up.
	 Move the rest of the line forward and refill to the end.
	 */

      memmove(in_buffer, line, rest);

      line = (in_buffer + rest);

      /* If there is no more input. Add '\0' to terminate the line in the buffer. */

      if (!fsm_fgets(line, (512 - rest), stream))
	*line = '\0';

      line = in_buffer;
    }

 return(NO_ERROR);
}

ALPHABETptr make_id_vect ()
/* Take id-s from the global id table and build a vector of id-s
   so that an input (ASCCI) character can index its label id.
   An ALPHABETptr is returned - it is not strictly necessary but
   useful eg when freeing memory reserved here: use free_alph(...).
*/
{
  ALPHABETptr ch_alph = make_alph(256, LABEL_VECTOR); 
  int i;
  id_type id;
  int lim = (IS_UTF8_MODE ? 128 : 256);
  /*  collect all the single character symbols */

  for (i = 1; i < lim; i++)
    {
      intern_one_character_symbol(&id, (char)i);
      if (ALPH_len(ch_alph) < id)
	ALPH_len(ch_alph) = id;
      ALPH_item(ch_alph, i) = id;
    }

  return (ch_alph);
}

ALPHABETptr make_outchar_vect (ALPHABETptr iv) 
/* static ALPHABETptr make_outchar_vect (OUT_ID_VECT iv) */
/* in make_id_vect(net) we built up a vector indexed by the 
   (input, ASCII) character values to find corresponding label id-s.
   Now the opposite is done, a vector, indexed by the label id-s,
   is built so that output characters can be found by label id-s.
   An ALPHABETptr is returned - it is not strictly necessary but
   useful eg when freeing memory reserved here: use free_alph(...).
   */
{
 short i;
 int le = (IS_UTF8_MODE ? 128 : 256);
 LABELptr label; 
 ALPHABETptr ch;

 le = max_of(le, (ALPH_len(iv)+1));
 ch = make_alph(le, LABEL_VECTOR);
 
 ALPH_item (ch, 0) = 0;/* This is Epsilon. */
 for (i = 1; i < le; i++)
   ALPH_item(ch, i)= '?';/* OTHER is the default for unspecified characters */

 for (i = 1; i < le; i++)
   {
     label = id_to_label (ALPH_item (iv, i));
     /* Labels may have tuples ! Only single char labels are considered here */
     if (label->arity == 1)
     ALPH_item(ch, ALPH_item(iv,i)) =  fstring_to_charcode (LABEL_atom (label));

    }
 return (ch);
} 


/*****************************************
 *                                       *
 *  parse table for bimachines           *
 *                                       *
 *****************************************/

int verify_bimachine_flags (NETptr left, NETptr right, int side_p)
{
  /* Returns TRUE if the left and right FST of the bimachine have
     correct flags, and FALSE otherwise. */

  if ((! left) || (! right))
    return (FALSE);

  if ((! NET_left_bimachine (left)) || (! NET_right_bimachine (right)))
    return (FALSE);

  if (side_p == UPPER)
    {
      if ((! NET_upper_seq (left)) || (! NET_upper_seq (right)))
	return (FALSE);
    }
  else if (side_p == LOWER)
    {
      if ((! NET_lower_seq (left)) || (! NET_lower_seq (right)))
	return (FALSE);
    }
  else
    return (FALSE);

  return (TRUE);
}

PARSE_TBL create_bimachine_parse_table (NETptr left, NETptr right, int side_p)
{
  /* Returns a parse table created from the input symbols of both
     FSTs of the bimachine, or NULL on failure. */

  PARSE_TBL ptb;  ALPHABETptr a1, a2, alph;

  if (! verify_bimachine_flags (left, right, side_p))
    {
      error_buffer ("put", "No parse table created because FSTs do not belong\
to a bimachine or are incorrectly marked");
      return (NULL);
    }

  if ((! NET_labels (left)) || (! NET_labels (right)))
    {
      error_buffer ("put", "No parse table created because one or both FSTs\
have no label alphabet");
      return (NULL);
    }

  a1 = side_sigma (left, side_p);
  a2 = side_sigma (right, side_p);

  alph = sigma_union (a1, a2);
  free_alph (a1);
  free_alph (a2);

  ptb = make_parse_table (NULL, alph, side_p);
  free_alph (alph);

  return (ptb);
}

PARSE_TBL get_bimachine_parse_table (NETptr left, NETptr right, int side_p)
{
  /* Returns the upper or lower parse table of the "left" FST
     (1st FST) of the bimachine, according to "side_p".
     If this parse table does not exist yet, it is created,
     connected to the "left" FST, and returned.
     Returns NULL on failure. */

  PARSE_TBL ptb;

  ptb = (side_p == UPPER) ?
    NET_upper_parse_table (left) : NET_lower_parse_table (left);

  if (! ptb)
    ptb = create_bimachine_parse_table (left, right, side_p);

  if (side_p == UPPER)
    NET_upper_parse_table (left) = ptb;
  else
    NET_lower_parse_table (left) = ptb;

  return (ptb);
}

#ifdef DEBUG
void TEST_print_range_map (NETptr nn, RANGEptr rm)
{
  uint16 i, j, llen =0;

  if (! (nn && rm))
    return;
  
 llen = NET_range_len(nn);   
 fsm_fprintf (stderr, "Range length=%d.\n", llen);
    
 for (i=0; i<llen; i++)
   {
     if (RANGE_id(rm+i) || (i == 0 && RANGE_len(rm+i)))
       {
	 fsm_fprintf (stderr, " %d=", i);
	 print_label(i, stderr, DO_ESCAPE); 

	 if(i != RANGE_id(rm+i))
	   fsm_fprintf (stderr, "\n??? i=%d RANGE_id(rm+i)=%d\n", i, RANGE_id(rm+i)) ; 

	 fsm_fprintf (stderr, " ==> le=%d;", RANGE_len(rm+i));

	 for (j=0; j < RANGE_len(rm+i); j++)
	   {
	     fsm_fprintf (stderr, " %d. %d=", j+1, RANGE_item(rm+i, j));
	     print_label(RANGE_item(rm+i, j), stderr, DO_ESCAPE);
	   }
	 fsm_fprintf (stderr, "\n", i);
       }
   }
}
#endif

#endif /* SLIM_FST */
