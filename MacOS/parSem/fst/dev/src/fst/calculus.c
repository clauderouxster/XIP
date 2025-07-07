/* $Id: calculus.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1990-97 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  CALCULUS.C
 **
 **  Todd Yampol, Xerox PARC, May 1990
 **  Lauri Karttunen, Xerox PARC, October 1992
 **  Lauri Karttunen, RXRC, February 1994
 **
 **  Calculus of finite-state networks.
 **
 ******************************************************/

#include <stddef.h>
#include <stdlib.h> 
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <string.h>

#include "fst/calculus.h"
#include "fst/btree.h"
#include "fst/lab-vect.h"
#include "fst/fsm_io.h"
#include "fst/pars-tbl.h"
#include "fst/define.h"
#include "fst/flags.h"
#include "fst/state-mr.h"
#include "fst/pretty-p.h"
#include "fst/fsm-fst-x.h"
#include "fst/compact.h"
#include "fst/cfsm.h"
#include "fst/mark.h"
#include "fst/struct.h"
#include "fst/utf8_fatstr_util.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

/* Defined in MODIFY.C. Default setting is STANDARD_MODE
   (defined in TYPES.H).  Another option is TWOL_MODE.
   */

/************************
 *
 *  Shortcuts: Simple ways of calling commonly used functions.
 *  These used to be in RE-AUX.C
 *
 ************************/

NETptr lenient_compose(NETptr upper, NETptr lower)
{
  /* Quick implementation of "lenient composition" for experimenting
     with optimality theory. Lenient composition of UPPER and LOWER
     is defined as follows:
     
     UPPER .O. LOWER = [[UPPER .o. LOWER] .P. UPPER]
     
     where .0. is the lenient compose operator,. .o. is ordinary
     composition and .P. is priority union.
     */

  NVtype nv;
  NETptr nets[2];
  
  nv.len = 2;
  nv.nets = nets;
  
  nets[0] = copy_fsm(upper);
  nets[1] = lower;
  
  return(priority_union_fst_pair(compose_fst(&nv, DO_RECLAIM, DO_MINIMIZE,
					     DONT_CLEANUP, PAIRWISE,
					     FLAGS_NOT_SPECIAL),
				 upper, UPPER, DONT_COPY, DO_MINIMIZE));
}


NETptr compose(NETptr upper, NETptr lower)
{
  NVtype nv;
  NETptr nets[2];
  
  nv.len = 2;
  nv.nets = nets;
  
  nets[0] = upper;
  nets[1] = lower;
  
  return(compose_fst(&nv, DO_RECLAIM, DO_MINIMIZE, DONT_CLEANUP, PAIRWISE,
		     *fst_IY_COMPOSE_FLAG_AS_SPECIAL()));
}


NETptr concat(NETptr net1, NETptr net2)
{
  NVtype nv;
  NETptr nets[2];
  
  nv.len = 2;
  nv.nets = nets;
  
  nets[0] = net1;
  nets[1] = net2;
  
  return(concat_fsm(&nv, DONT_COPY, DONT_MINIMIZE));
}

NETptr contains(NETptr net)
{
  return(contains_fsm(net, DONT_COPY, DONT_MINIMIZE));
}

NETptr ignore(NETptr net1, NETptr net2)
{
  return(ignore_fsm(net1, net2, DONT_COPY, DONT_COPY, DONT_MINIMIZE));
}


NETptr intersect(NETptr net1, NETptr net2)
{
  NETptr nets[2];
  NVtype nv;
  
  nv.len = 2;
  nv.nets = nets;
  
  nets[0] = net1;
  nets[1] = net2;
  
  return(intersect_fsm(&nv, DO_RECLAIM, DO_MINIMIZE));
}

NETptr kleene_plus(NETptr net)
{
  return(one_plus(net, DONT_COPY, DONT_MINIMIZE));
}

NETptr kleene_star(NETptr net)
{
  return(zero_plus(net, DONT_COPY, DONT_MINIMIZE));
}


NETptr minus(NETptr net1, NETptr net2)
{
  return(minus_fsm(net1, net2, DO_RECLAIM, DO_RECLAIM, DONT_MINIMIZE));
}

static void complement_undefined_msg(void)
{
	fsm_fprintf(stderr, "The complement operator ~ is defined only for ");
	fsm_fprintf(stderr, "regular languages:\n");
	fsm_fprintf(stderr, "~A denotes the language [?* - A]. You must use the ");
	fsm_fprintf(stderr, "expression\n");
	fsm_fprintf(stderr, "[[?:?]* - A] instead of ~A if A represents a relation.\n");
}

NETptr negate(NETptr net)
{
	if (NET_arc_label_arity(net) > 1 && FSM_MODE == STANDARD_MODE)
		{
			complement_undefined_msg();
			free_network(net);
			return(null_fsm());
		}
  return(negate_fsm(net, DONT_COPY, DONT_MINIMIZE));
}


NETptr net_union(NETptr net1, NETptr net2)
{
  NVtype nv;
  NETptr nets[2];
  
  nv.len = 2;
  nv.nets = nets;
  
  nets[0] = net1;
  nets[1] = net2;
  
  return(union_fsm(&nv, DONT_COPY, DONT_MINIMIZE));
}

NETptr optional(NETptr net)
{
  return(optional_fsm(net, DONT_COPY, DONT_MINIMIZE));
}

NETptr priority_union (NETptr net1, NETptr net2, int side)
{
  return (priority_union_fst_pair(net1,net2, side,DONT_COPY,DONT_MINIMIZE));
}


/************************
 *
 *  Simple construction
 *
 ************************/

NETptr symbol_fsm(id_type id)
{
  /* Returns a network that accepts a string consisting of a single symbol,
     or epsilon fsm if ID is EPSILON. If id is an fstpair containing
     OTHER, the network will contain more than one arc. For example,
     a:? also adds an 'a' arc.
     */
  
  if (id == EPSILON)
    return(epsilon_fsm());
  else
    {
      
      NETptr net = null_fsm();
      ALPHABETptr labels = NET_labels(net); 
      STATEptr dest_state = make_state(NULL, net);
      
      STATE_final(dest_state) = TRUE;
      add_arc(make_arc(id, dest_state, net), NET_start_state(net), net);
      
      adjust_alph_size(labels, 1);
      ALPH_item(labels, 0) = id;

      label_to_sigma(NET_sigma(net), id);

      /* If the label is an fstpair and one of the components is OTHER,
	 we also need to add an arc for the other component in order
	 to uphold the convention that a:b is equivalent to [a .x. b]
	 in all cases. Note that [a .x. ?] yields [a:? | a] since
	 ? gets expanded in the course of building the crossproduct.
	 */
      
      if (tuple_id_p(id) &&
	  ((id_to_upper_id(id) == OTHER && id_to_lower_id(id) != EPSILON) ||
	   (id_to_lower_id(id) == OTHER && id_to_upper_id(id) != EPSILON)))
	expand_tuple_other(net, DONT_COPY);

      return(net);
    }
}

NETptr label_fsm (char *symbol_name)
{
  /* Returns a net that only accepts the string that only consists of
     the argument symbol. */
  id_type symbol_id;
  FST_intern_literal (&symbol_id, symbol_name);
  return (symbol_fsm (symbol_id));
}

NETptr kleene_plus_fsm (id_type id)
{
  if (id == EPSILON)
    return(epsilon_fsm());
  else
    {
      STATEptr state;
      NETptr net = symbol_fsm(id);
      
      for (state = NET_states(net); state != NULL; state = next_state(state))
        if (STATE_final(state))
          {
            push_arc(make_arc(id, state, net), state);
            break;
          }
      NET_loop_free(net) = FALSE;

      if (tuple_id_p(id) &&
	  ((id_to_upper_id(id) == OTHER && id_to_lower_id(id) != EPSILON) ||
	   (id_to_lower_id(id) == OTHER && id_to_upper_id(id) != EPSILON)))
	expand_tuple_other(net, DONT_COPY);

      return(net);
    }
}

NETptr kleene_star_fsm(id_type id)
     /*****
      *
      * Returns an "kleene_star" fsm with a single looping ID arc
      * leading to the final initial state.
      *
      ******/
{
  NETptr net = epsilon_fsm();
  
  if (id != EPSILON)
    {
      
      ALPHABETptr labels = NET_labels(net);
      
      adjust_alph_size(labels, 1);
      ALPH_item(labels, 0) = id;
      
      add_arc(make_arc(id, NET_start_state(net), net),
              NET_start_state(net), net);
      
      NET_loop_free(net) = FALSE;
      
      label_to_sigma(NET_sigma(net), id);

      if (tuple_id_p(id) &&
	  ((id_to_upper_id(id) == OTHER && id_to_lower_id(id) != EPSILON) ||
	   (id_to_lower_id(id) == OTHER && id_to_upper_id(id) != EPSILON)))
	expand_tuple_other(net, DONT_COPY);
    }
  return(net);
}

NETptr sigma_star_fsm(void)
{
  return(kleene_star_fsm(OTHER));
}

NETptr universal_language(void)
{
  return(sigma_star_fsm());
}

NETptr universal_relation(void)
{
  return(crossproduct_fst(universal_language(), universal_language(),
			  DO_RECLAIM, DO_MINIMIZE));
}


NETptr universal_equal_length_relation(void)
{
	NETptr net = kleene_star_fsm(OTHER);
	id_type neq_other;
	make_pair_label(&neq_other, OTHER, OTHER);
	
	add_arc(make_arc(neq_other, NET_start_state(net), net),
					NET_start_state(net), net);
	return(net);
}

NETptr universal_identity_relation(void)
{
	return(sigma_star_fsm());
}

NETptr string_to_fsm(char *str, int literal_p)
{
  /* Parses a simple string to a sequence of symbol id's
     character by character. If LITERAL_P is true, all characters
     are taken literally. If LITERAL_P is false, '0' is interpreted
     as EPSILON, '?' as OTHER, and '%' acts as the escape character.
     */
  id_type id;

  if (str == NULL)
    return(null_fsm());
  else
    {
      char sym[2];

      sym[1] = '\0';
      init_lab_vector();

      while(*str)
	{
	  if (literal_p)
	    {
	      sym[0] = *str++;
	      intern_literal(&id, sym);
	      add_label(id);
	    }
	  else if (*str == '%')
	    {
	      str++;
	      sym[0] = *str++;
	      intern_literal(&id, sym);
	      add_label(id);
	    }
	  else
	    {
	      sym[0] = *str++;
	      intern_symbol(&id, sym);
	      add_label(id);
	    }
	}

      add_label(ID_EOS);

      return(path_fsm(lab_vect_labs()));
    }
}

NETptr utf8_string_to_fsm(char *str, int literal_p)
{
	// Parses a simple string to a sequence of symbol id's
	//  character by character. If LITERAL_P is true, all characters
	// are taken literally. If LITERAL_P is false, '0' is interpreted
	// as EPSILON, '?' as OTHER, and '%' acts as the escape character.
	
	FAT_STR utf8str;
	if (str == NULL)
	  return(null_fsm());
	else
	{
	  // Make a Unicode string from the UTF-8 string
	  utf8str = utf8_thin_to_temp_fat(str);

	  if (!isLegalUTF8Sequence(str, str + strlen(str)))
	    return(null_fsm());

	  // Intern it char by char
	  return unicode_string_to_fsm(utf8str, literal_p);
	}
}

NETptr unicode_string_to_fsm(FAT_STR str, int literal_p)
{
  /* Parses a unicode (fat) string to a sequence of symbol id's
     fat-character by fat-character.
     - If LITERAL_P is true,
       all fat-characters are taken literally.
     - If LITERAL_P is false, '0' is interpreted as EPSILON, '?'
       as OTHER, and '%' acts as the escape character.
     */
  id_type id;

  if (str == NULL)
    return(null_fsm());
  else
    {
      FAT_CHAR unicode_sym[2];

      CHARSET(&unicode_sym[1]) = '\0';
      CHARNUM(&unicode_sym[1]) = '\0';
      init_lab_vector();
          
      while ((CHARSET(str) != '\0') || (CHARNUM(str) !='\0') )
	{
	  if (literal_p)
	    {
	      CHARSET(unicode_sym) = CHARSET(str);
              CHARNUM(unicode_sym) = CHARNUM(str);
              str++;
              intern_atomic_label(&id, unicode_sym);
	      add_label(id);
	    }
	  else if ((CHARSET(str) == '\0') && (CHARNUM(str) =='%') )
	    {
              str++;
	      CHARSET(unicode_sym) = CHARSET(str);
              CHARNUM(unicode_sym) = CHARNUM(str);
              str++;
	      intern_atomic_label(&id, unicode_sym);
	      add_label(id);
	    }
          else 
	    {
	      CHARSET(unicode_sym) = CHARSET(str);
              CHARNUM(unicode_sym) = CHARNUM(str);
              str++;
	      intern_unicode_symbol(&id, unicode_sym);
	      add_label(id);
	    }
	}

      add_label(ID_EOS);

      return(path_fsm(lab_vect_labs()));
    }
} /* unicode_string_to_fsm */


NETptr path_fsm(id_type *path)
{
  /* Returns a network that accepts a string
     consisting of symbols in the vector PATH.
     ID_EOS indicates the end of the path.  Returns
     a NULL fsm if the path is NULL, an EPSILON
     fsm, if the path is empty. Need to check for OTHER.
     */
  
  NETptr net = null_fsm();
  int contains_other_p = FALSE;
  id_type id;

  if (path != NULL)
    {
      STATEptr nextstate = NET_start_state(net), state;
      ALPHABETptr labels = label_to_binary(NET_labels(net));

      while ((id = *path) != ID_EOS)
	{
	  state = nextstate;
	  nextstate = make_state(NULL, net);
	  add_arc(make_arc(id, nextstate, net), state, net);
	  sigma_add_to(labels, id);
	  label_to_sigma(sigma(net), id);

	  if (id == OTHER ||
	      (tuple_id_p(id) &&
	       ((id_to_upper_id(id) == OTHER &&
		 id_to_lower_id(id) != EPSILON) ||
		(id_to_lower_id(id) == OTHER &&
		 id_to_upper_id(id) != EPSILON))))
	    contains_other_p = TRUE;
	  path++;
	}

      binary_to_label(labels);
      STATE_final(nextstate) = TRUE;

      /* If one of the labels on the path contains OTHER, we need
	 to expand it properly. */

      if (contains_other_p)
	expand_other(net, NET_sigma(net), DONT_COPY);
    }

  return(net);
}

NETptr sigma_fsm(ALPHABETptr alph)
{
  /* Returns a network that accepts any single
     symbol string constructed from the alphabet.
     A null pointer is taken to be equivalent to
     the alphabet containing just OTHER, an alphabet
     with no symbols yiels NULL_FSM.
     */
  
  int32 i;
  int null_p = TRUE;
  ALPHABETptr sigma, labels;
  NETptr net;
  STATEptr dest_state, start_state;
  
  if (alph == NULL)
    return(symbol_fsm(OTHER));
  
  if (ALPH_type(alph) == LABEL_VECTOR)
    {
        if  (ALPH_len(alph) > 0)
            null_p = FALSE;
    }
    else
      for (i = 0; i < ALPH_len(alph); i++)
        if (ALPH_items(alph)[i] != 0)
          {
            null_p = FALSE;
            break;
          }
  
  if (null_p == TRUE)
    return(null_fsm());
  
  /* Otherwise there is work to be done */
  
  net = null_fsm();
  start_state = NET_start_state(net);
  dest_state = make_state(NULL, net);
  STATE_final(dest_state) = TRUE;
  labels = NET_labels(net);
  sigma = NET_sigma(net);
  
  label_augment(labels, alph);
  sigma_augment(sigma, alph);
  
  /* Make sure we don't make an EPSILON arc.
     Instead, the start state becomes final.
     */
  
  if (binary_sigma_member(sigma, EPSILON))
    {
      STATE_final(start_state) = TRUE;
      sigma_remove_from(sigma, EPSILON);
      sigma_remove_from(labels, EPSILON);
    }
  
  for (i = ALPH_len(labels) -1; i >= 0; i--)
    add_arc(make_arc(ALPH_item(labels, i), dest_state, net),
            start_state, net);
  
  return(net);
}

NETptr single_char_sigma_fsm(ALPHABETptr alph)
{
  /* Returns a network that accepts any single
     symbol string constructed from the alphabet.

     A multi-character symbol is decomposed into a
     sequence of single character ones. TG.

     A null pointer is taken to be equivalent to
     the alphabet containing just OTHER, an alphabet
     with no symbols yields NULL_FSM.
     */
  
  int32 i;
  int null_p = TRUE;
  ALPHABETptr sigma_of_net, labels;
  NETptr net;
  STATEptr dest_state, start_state, statefrom, stateto;
  id_type new_l;
  FAT_CHAR singlechar_fat_str [2] = {{ '\0', '\0'},{ '\0', '\0'} };
  FAT_STR this_fat_str, next_fat_str;
  
  if (alph == NULL)
    return(symbol_fsm(OTHER));
  
  if (ALPH_type(alph) == LABEL_VECTOR)
    {
        if  (ALPH_len(alph) > 0)
            null_p = FALSE;
    }
    else
      for (i = 0; i < ALPH_len(alph); i++)
        if (ALPH_items(alph)[i] != 0)
          {
            null_p = FALSE;
            break;
          }
  
  if (null_p == TRUE)
    return(null_fsm());
  
  /* Otherwise there is work to be done */
  
  net = null_fsm();
  start_state = NET_start_state(net);
  dest_state = make_state(NULL, net);
  STATE_final(dest_state) = TRUE;
  labels = copy_alphabet (NET_labels(net));
  sigma_of_net = NET_sigma(net);
  
  label_augment(labels, alph);
  sigma_augment(sigma_of_net, alph);
  
  /* Make sure we don't make an EPSILON arc.
     Instead, the start state becomes final.
     */
  
  if (binary_sigma_member(sigma_of_net, EPSILON))
    {
      STATE_final(start_state) = TRUE;
      sigma_remove_from(sigma_of_net, EPSILON);
      sigma_remove_from(labels, EPSILON);
    }
  
  for (i = ALPH_len(labels) -1; i >= 0; i--)
    {
      if (LABEL_arity(LIM_map (LABEL_MAP)[(ALPH_item(labels, i))]) == 1)
	{
	  /* if not multi-char */

	  this_fat_str = LABEL_atom (LIM_map (LABEL_MAP) [ALPH_item(labels, i)]);

   	  if ( (! fat_charcmp( *(this_fat_str +1), (unsigned char *) "\0\0" ) )
	       || ALPH_item(labels, i) == OTHER 
	       || ALPH_item(labels, i) == DUMMY_LABEL
	       || ALPH_item(labels, i) == BOUNDARY_SYMBOL
	       || ALPH_item(labels, i) == ALTCHAIN_SYMBOL
	       || ALPH_item(labels, i) == SPACE_SYMBOL
	       || ALPH_item(labels, i) == REPEAT_SYMBOL)
	    add_arc(make_arc(ALPH_item(labels, i), dest_state, net),
		    start_state, net);
	  /* if multi-char: decompose to single-char-s. Components may have to be intern-ed. */
	  else
	    {
	      statefrom = start_state; 
	      while ( fat_charcmp( *this_fat_str, (unsigned char *) "\0\0" ))
		{
		  next_fat_str = this_fat_str+1; 
		  if (fat_charcmp( *next_fat_str, (unsigned char *) "\0\0" ))
		    stateto = make_state(NULL, net);
		  else
		    stateto = dest_state;
		  fat_charcpy(singlechar_fat_str[0], this_fat_str[0]);

		  intern_atomic_label (&new_l, singlechar_fat_str);

   		  sigma_add_to(NET_sigma (net), new_l);
		  add_new_arc(net, statefrom, new_l, stateto);
		  sigma_add_to(NET_labels(net), new_l);
		  statefrom = stateto;
		  this_fat_str = next_fat_str;
		}
	    }
	}
      else
	{
	  handle_error("symbol pair not allowed in sigma","SINGLE_CHAR_SIGMA_FSM",(ALPH_item(labels, i)));
	}
    }

  update_label_alphabet (net);
  free_alph(NET_sigma(net));
  NET_sigma(net) = NULL;
  NET_sigma(net) = sigma (net);
  min_fsm(net, DONT_COPY);
  reorder_arcs(net, default_arc_comp_fn);
  free_alph(labels);
  return(net);
}

static int add_word_arc_to_net (NETptr *newnet, char * labbuf)
     /* Add this word - as an arc with the content of labbuf as
	a multi-character label -  to newnet. If newnet doesn't exist,
	create one. 
	*/
{
  int error= NO_ERROR;
  id_type newl = 0;
  FAT_STR fstr=NULL;
  STATEptr dest;

  if (! (*newnet))
    *newnet = null_fsm();
  fstr = FST_thin_to_fat (labbuf);
   
  error= intern_atomic_label(&newl, fstr);
  if (error != NO_ERROR)
    return(error);
  
  if (newl)
    {
      dest = make_state(NULL, *newnet);
      STATE_final(dest) = TRUE;	   
      add_new_arc(*newnet, NET_start_state(*newnet),  newl, dest);
    }
  free(fstr);
  return(error);
}

NETptr multi_char_sigma_fsm(NETptr single_char_net, FST_CNTXTptr fst_cntxt)
{
  /* Returns a network that accepts any word constructed from the
     words of single_char_net: if a word contains more than one
     character, they are composed into a single multi-character
     symbol. 
     Expects a special net created by single_char_sigma_fsm() but 
     works on other real fsm-s (not fst-s!), containing single-
     or multi-characters, if they are neither circular nor too big
     (limited by available label space).
     The algorithm is suboptimal in case of a biggish SINGLE_CHAR_NET
     because number_to_word() does an exhaustive search. If this
     situation is frequent, another path-finding strategy (where a path
     is put out as an arc label) is to be used, e.g. an adapted
     net-io.c:print_paths(). 
     */
  char * wd = NULL;
  unsigned long i;
  NETptr result = NULL;
  unsigned long path_count = 0;

  if (! (single_char_net && (NET_sigma(single_char_net) = sigma(single_char_net))
      && (NET_sigma(single_char_net) = binary_to_label(NET_sigma(single_char_net)))
      && (ALPH_len (NET_sigma(single_char_net)) > 0)))
    return result;

  if (NET_arc_label_arity(single_char_net) >1)
    handle_error("Only FSM-s allowed.","MULTI_CHAR_SIGMA_FSM",
		 NET_arc_label_arity(single_char_net)); 

  /*  check for OTHER */
  if (! (ALPH_type(NET_sigma(single_char_net)) == BINARY_VECTOR))
    label_to_binary(NET_sigma(single_char_net));

  if (binary_sigma_member (NET_sigma(single_char_net), OTHER))
    handle_warning("OTHER not handled yet.","MULTI_CHAR_SIGMA_FSM",
		   OTHER); 
 
  /* check for BOUNDARY_SYMBOL */
  if (binary_sigma_member (NET_sigma(single_char_net), BOUNDARY_SYMBOL))
    handle_warning("BOUNDARY_SYMBOL not handled yet.","MULTI_CHAR_SIGMA_FSM",
		   BOUNDARY_SYMBOL); 
 
  if (!NET_compacted(single_char_net) && !NET_tabular(single_char_net))
    switch (index_net(single_char_net))
      {
      case 2: /* Net is cyclic */
	handle_warning("Circular.","MULTI_CHAR_SIGMA_FSM", 2);
	return result;
	break;
      case 1: /* Overflow */
	handle_warning("more than %lu paths.","MULTI_CHAR_SIGMA_FSM", ULONG_MAX);
	return result;
	break;
      case 0:
      default:
	path_count =
	  (unsigned long) STATE_client_cell(NET_start_state(single_char_net));
	break;
      }
  if (path_count > MAX_LABEL)
    {
    	handle_warning("more than max. number of allowed paths.","MULTI_CHAR_SIGMA_FSM",
		       MAX_LABEL); 
      return result;
    }

  for (i = 0; i < path_count; i++)
    {
      wd = number_to_word(i, single_char_net, LOWER, FALSE);
      if (wd)
	add_word_arc_to_net (&result, wd);
    }

  /* reform result */
  if (result)
    {
      STATE_deterministic(NET_start_state(result)) = FALSE;
      NET_deterministic(result) = FALSE;
      NET_minimized(result) = FALSE;
      update_label_alphabet (result);
      free_alph(NET_sigma(result));
      NET_sigma(result) = NULL;
      NET_sigma(result) = sigma (result);
      min_fsm(result, DONT_COPY);
      reorder_arcs(result, default_arc_comp_fn);
    }
  return result;
}

# define PUS_INTERNAL_BUFFER_LENGTH 256

static STATEptr match_id_of_single_fat_char_on_symbol_fsm_state(STATEptr systat, id_type sid)
{
  STATEptr dest = NULL;
  ARCptr sarc, narc;

  if (systat)
    for (sarc = STATE_arc_set(systat) ; sarc; sarc = narc)
      {
	narc = next_arc (sarc);
	if ( (sid == lower_id(ARC_label(sarc))))
	  dest = (ARC_destination(sarc));
      }
  return dest;
}

int parse_unicode_string(FAT_STR string, 
			 NETptr single_char_symbol_fsm, 
			 id_type *result, 
			 int result_len,
			 FST_CNTXTptr fst_cntxt)
{
  int ssp = 0, sp = 0 , pres = 0, old_sp = 0,  match = FALSE,
    intern_fat_buf_index = 0, longest_m_p = 11, longest_m_in_string = 0; 
  int32 i;  
  id_type sid = 0, fid = 0 ;
  FAT_CHAR intern_fat_buf[PUS_INTERNAL_BUFFER_LENGTH]={ {'\0'} }, 
          sfstr[2]={{'\0', '\0'},{ '\0', '\0'} };
  STATEptr systat;
  ALPHABETptr labels;

  /* check unicode_parse_table_symbol_fsm */
  if ( !( NET_deterministic (single_char_symbol_fsm) && NET_minimized (single_char_symbol_fsm) &&
	  NET_loop_free(single_char_symbol_fsm) ))
    handle_error("symbol fsm is not as expected", "PARSE_UNICODE_STRING", (int) single_char_symbol_fsm);

  /* ** check if really an fsm 
     - may not be always necessary 
     - or do it once for all on a higher level */
  labels = NET_labels(single_char_symbol_fsm);
  for (i = ALPH_len(NET_labels(single_char_symbol_fsm)) -1; i >= 0; i--)
    {
      if (LABEL_arity(LIM_map (LABEL_MAP)[(ALPH_item(labels, i))]) != 1)
	handle_error("symbol pair not allowed in an fsm","PARSE_UNICODE_STRING",(ALPH_item(labels, i)));
    };

  /* Parse on the symbol_fsm */
  systat = NET_start_state (single_char_symbol_fsm);
  while ( fat_charcmp( string[ssp], (unsigned char *) "\0\0" ) && pres < result_len)
    {
      sp = ssp; 
      ssp++;
      old_sp = sp;
      fat_charcpy(sfstr[0], string[sp]);
      atomic_label_to_id(&sid, sfstr);

      if (!( sid != 0 && sid != ID_NO_SYMBOL && 
	  binary_sigma_member (sigma(single_char_symbol_fsm), sid)))
	/* Stop Parse. This Unicode single-symbol Unicode string is not in the
	   sigma of symbol-fsm */
	break;

      if ( (systat = match_id_of_single_fat_char_on_symbol_fsm_state(systat, sid)))
	{
	  fat_charcpy(intern_fat_buf[intern_fat_buf_index], sfstr[0]);
	  intern_fat_buf_index++;
	  if (STATE_final(systat))
	   {
	     match = TRUE;
	     longest_m_p = intern_fat_buf_index;
	     longest_m_in_string = ssp;
	   }
	  if ( STATE_arc_set (systat))
	    continue ; /* read next char */
	  else
	    {
	      if (longest_m_p ) /* match found */
		{
		  fat_charcpy ( intern_fat_buf[longest_m_p] , "\0\0");

		  atomic_label_to_id (&fid, intern_fat_buf);

		  if (pres  < result_len && fid )
		    {
		      result[pres] = fid;
		      pres++;
		    }
		  else
		    {
		      if (fid)
			return old_sp; /** Result Buffer too small **/
		    }
	
		  /* reset for a new char/id */
		  longest_m_p = intern_fat_buf_index = 0;
		  match = FALSE;
		  systat = NET_start_state (single_char_symbol_fsm);
		}
	    }     
	}

      if (match) /* match found but not yet handled */
	{
	  if (longest_m_p ) /* match found */
	    {
	      fat_charcpy ( intern_fat_buf[longest_m_p] , "\0\0");

	      atomic_label_to_id (&fid, intern_fat_buf);

	      if (pres  < result_len && fid )
		{
		  result[pres] = fid;
		  pres++;
		}
	      else
		{
		  if (fid)
		    return old_sp; /** Result Buffer too small **/
		}
	
	      /* reset for a new char/id */
	      longest_m_p = intern_fat_buf_index = 0;
	      match = FALSE;
	      systat = NET_start_state (single_char_symbol_fsm);

	      /* backtrack in input string ! */
	      ssp = longest_m_in_string;
	    }  
	}
    }

  if (match) /* match found but not yet handled */
    {
      if (longest_m_p ) /* match found */
	{
	  fat_charcpy ( intern_fat_buf[longest_m_p] , "\0\0");
	  atomic_label_to_id (&fid, intern_fat_buf);

	  if (pres  < result_len && fid )
	    {
	      result[pres] = fid;
	      pres++;
	    }
	  else
	    {
	      if (fid)
		return old_sp; /** Result Buffer too small **/
	    }
	}  
    }

  if (pres && pres < result_len)
    {
      result [pres] = 0;
    }
  return  longest_m_in_string;
}

#define TEST_1_UNIC_BUF_LEN 10

int TEST1_parse_unicode_string (NETptr uc_fst, 
				char *nonunic_str,
				FST_CNTXTptr fst_cntxt) 
{
  FAT_STR unic_str = NULL;
  int result = FALSE, uprlen = TEST_1_UNIC_BUF_LEN, i;
  id_type upresult [TEST_1_UNIC_BUF_LEN] = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 110 } ;
  NETptr symb_fsm = NULL;

  /* Convert 8-bit string to Unicode */
  unic_str = FST_thin_to_fat (nonunic_str); 
  
  if (! uc_fst || ! NET_sigma(uc_fst))
    return result;
 
  /* Create the symbol-fsm - the basis of unicode parsing - of the FST */
  symb_fsm = single_char_sigma_fsm (NET_sigma(uc_fst));

  /* Parse the unicode string */
  result = parse_unicode_string (unic_str, symb_fsm, upresult, uprlen, fst_cntxt);

  /* TEST printing of result */
  fsm_fprintf(stdout,"String parsed to ID-s: ");
  for (i = 0 ; i < TEST_1_UNIC_BUF_LEN + 3; i++) /* read 3 more ! */
     fsm_fprintf(stdout," %d, ", upresult[i]);

  fsm_fprintf(stdout,"\nString parsed to characters/symbols: ");
  for (i = 0 ; i <  TEST_1_UNIC_BUF_LEN + 3; i++) /* read 3 more ! */
    {
      if (! binary_sigma_member (sigma(uc_fst), upresult[i]))
	break;
      print_label(upresult[i], stderr, DO_ESCAPE);
      fsm_fprintf(stdout," ");
    }
  fsm_fprintf(stdout,"\n");

  if (symb_fsm)
    free_network(symb_fsm);
  return result;
}

NETptr sigma_but(ALPHABETptr sig)
{
  /* Returns a network that accepts the complement of the
     labels in SIG.  SIG should not contain OTHER.
     */
  
  NETptr net = symbol_fsm(OTHER);
  ALPHABETptr other_sigma = label_to_binary(copy_alphabet(sig));
  
  /* Make sure that OTHER is not yet member of OTHER_SIGMA, add it,
     and replace the original sigma of NET with OTHER_SIGMA.
     */
  
  if (binary_sigma_member(other_sigma, OTHER))
    handle_error("Input alphabet contains OTHER", "SIGMA_BUT", OTHER);
  else
    sigma_add_to(other_sigma, OTHER);
  
  free_alph(sigma(net));
  NET_sigma(net) = other_sigma;
  
  return(net);
}


NETptr optional_fsm(NETptr net, int copy_p, int minimize_p)
{
  /* Returns a network that recognizes the language of NET and
     the empty string
     */
  
  STATEptr start_state;
  
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  start_state = NET_start_state(net);
  
  /* If the start state is final, we can stop right here */
  
  if (STATE_final(start_state) == TRUE)
    return(net);
  
  /* Otherwise, there is work to be done */
  
  if (start_loops_p(net) == TRUE)
    {
      /* The original start state must be kept. */
      
      start_state = copy_state(start_state, net);
      NET_start_state(net) = start_state;
    }
  
  STATE_final(start_state) = TRUE;      
  NET_minimized(net) = FALSE;  
  
  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);
  
  return(net);
}


NETptr one_plus(NETptr net, int copy_p, int minimize_p)
{
  /* Returns a network that accepts Kleene-plus of the language of NET */
  
  STATEptr start_state, state;
  ARCptr arc;
  long arc_count = NET_num_arcs(net);
  
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  start_state = NET_start_state(net);
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    if (STATE_final(state) == TRUE && state != start_state)
      for (arc = STATE_arc_set(start_state); arc != NULL; arc = next_arc(arc))
#ifdef FULL_FSM
        add_new_arc(net, state, ARC_label(arc), ARC_destination(arc), ARC_user_pointer (arc));
#else
  add_new_arc(net, state, ARC_label(arc), ARC_destination(arc));
#endif
  
  /* If we added arcs, the net is certainly not loopfree
     perhaps not minimized. */

  if (NET_num_arcs(net) > arc_count)
      {
        NET_loop_free(net) = FALSE;
        NET_minimized(net) = FALSE;
      }
  
  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);
  
  return(net);
}

NETptr zero_plus(NETptr net, int copy_p, int minimize_p)
{
  /* Returns a network that accepts Kleene-star of the language of NET
     If a copy is to be made, do it in OPTIONAL_FSM; if the result is
     to be minimized, do it in ONE_PLUS.
     */
  
  return(one_plus(optional_fsm(net, copy_p, DONT_MINIMIZE),
                  DONT_COPY, minimize_p));
}

NETptr repeat_label(id_type label_id, int min, int max)
{
  /* Returns a network that accepts strings that consist of
     at least MIN and at most MAX concatenations of label.
     MAX < 0 means no upper limit. If MAX is smaller than MIN,
     we interchange them.
     */
  NETptr net;
  STATEptr dest, state;
  int i;
  
  if (id_to_label(label_id) == NULL)
    handle_error("Illegal label id", "REPEAT_LABEL", label_id);
  
  if (max >= 0 && max < min)
    {
      /* handle_warning("MAX less than MIN, switching", "REPEAT_LABEL", max);
       */
      /* Let's not bother the user. Just switch the two parameters. */
      i = min;
      min = max;
      max = i;
    }
  
  if (label_id == EPSILON)
    return(epsilon_fsm());
  
  net = null_fsm();
  state = NET_start_state(net);

  if (min > 0 || max > 0)
    {
      label_to_sigma(sigma(net), label_id);   
      adjust_alph_size(NET_labels(net), 1);
      
      if (ALPH_items(NET_labels(net)) == NULL)
        not_enough_memory("REPEAT_LABEL");
      
      ALPH_item(NET_labels(net),0) =  label_id;
    }
  
  for (i = 0; i < min; i++)
    {
      dest = make_state(NULL, net);
      add_arc(make_arc(label_id, dest, net), state, net);
      state = dest;
    }
  
  STATE_final(state) = TRUE;
  
  /* Negative value for MAX means no upper limit. */
  
  if (max < 0)
    add_arc(make_arc(label_id, state, net), state, net);
  else
    for (i = min; i < max; i++)
      {
        dest = make_state(NULL, net);
        add_arc(make_arc(label_id, dest, net), state, net);
        state = dest;
        STATE_final(state) = TRUE;
      }

  /* if label contains a one-sided OTHER, we need to expand it. */

      if (tuple_id_p(label_id) &&
	  ((id_to_upper_id(label_id) == OTHER &&
	    id_to_lower_id(label_id) != EPSILON) ||
	   (id_to_lower_id(label_id) == OTHER &&
	    id_to_upper_id(label_id) != EPSILON)))
	expand_tuple_other(net, DONT_COPY);

  return(net);
}

NETptr repeat_fsm(NETptr net, int min, int max, int copy_p, int minimize_p)
{
  /* Returns a network that accepts strings that consist of
     at least MIN and at most MAX concatenations of strings
     in the language of NET.  MAX < 0 means no upper limit.
     */
  
  NETptr new_net = NULL, last_net;
  NVptr new_nets;
  int i, rptn;
  
  if (max >= 0 && max < min)
    {
      handle_warning("MAX less than MIN", "REPEAT_FSM", max);
      min = max;
    }
  
  if (max > 0 || min > 0)
    {
      if (copy_p == DO_COPY)
	new_net = copy_fsm(net);
      else
	new_net = net;
    }
  else
    return(epsilon_fsm());
  
  if (max > 0)
    rptn = max;
  else if (min > 0)
    rptn = min;
  else
    return(zero_plus(new_net, DONT_COPY, DONT_MINIMIZE));
  
  new_nets = make_nv(rptn);
  NV_net(new_nets, 0) = last_net = new_net;
  
  for (i = 1; i < min; i++)
    {
      NV_net(new_nets, i) = copy_fsm(net);
      last_net = NV_net(new_nets, i);
    }
  
  if (max < 0)
    one_plus(last_net, DONT_COPY, DONT_MINIMIZE);
  else
    for (i = min; i < max; i++)
      NV_net(new_nets, i) = optional_fsm(net, DO_COPY, DONT_MINIMIZE);
  
  new_net = concat_fsm(new_nets, DONT_COPY, minimize_p);
  free_nv_only(new_nets);
  
  return(new_net);
}

NETptr contains_no_label(id_type id)
{
    /* A fast way of computing ~$[id] */

  if (id == OTHER)
    return(epsilon_fsm());
  else if (id == EPSILON)
    return(null_fsm());
  else
    {
      NETptr net = sigma_star_fsm();

      if (FSM_MODE != TWOL_MODE && tuple_id_p(id))
	handle_error("Cannot add fstpair to sigma", "CONTAINS_NO_LABEL", id); 
        
      sigma_add_to(NET_sigma(net), id);
      return(net);
    }
}

NETptr contains_label(id_type id)
{
  /* Accepts everything that contains label ID */
  
  if (id == EPSILON)
    return(sigma_star_fsm());
  else if (id == OTHER)
    return(kleene_plus_fsm(OTHER));
  else if (tuple_id_p(id))
      return(contains_fsm(symbol_fsm(id), DONT_COPY, DO_MINIMIZE));
  else
  {
      /* Special case: ID is an atomic symbol. We know the result
         is deterministic and minimal but not loopfree. */

      NETptr net = symbol_fsm(id);
      ALPHABETptr alph = make_alph(2, LABEL_VECTOR);
      STATEptr state;
      
      /* Replace the label alphabet, augment the sigma. */

      ALPH_item(alph, 0) = id;
      ALPH_item(alph, 1) = OTHER;
      free_alph(NET_labels(net));
      NET_labels(net) = alph;

      sigma_add_to(NET_sigma(net), OTHER);
      
      /* Add looping arcs for ID and OTHER to the final state and
         a looping OTHER arc to the initial state.
         */
      for (state = NET_states(net); state != NULL; state = next_state(state))
          if (STATE_final(state))
          {
              push_arc(make_arc(id, state, net), state);
              push_arc(make_arc(OTHER, state, net), state);
          }
          else
              push_arc(make_arc(OTHER, state, net), state);
      
      NET_loop_free(net) = FALSE;
      return(net);

  }
}


NETptr contains_fsm(NETptr net, int copy_p, int minimize_p)
{
  /* Accepts everything that contains a string in the language of net. */
  
  NETptr new_net;
  NVptr new_nets = make_nv(3);
  
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  NV_net(new_nets, 0) = sigma_star_fsm();
  NV_net(new_nets, 1) = net;
  NV_net(new_nets, 2) = sigma_star_fsm();
  
  new_net = concat_fsm(new_nets, DONT_COPY, minimize_p);
  free_nv_only(new_nets);
  return(new_net);
}

static void scramble_nets(NVptr nets, NVptr *array)  
{
  /* Fills the vectors in array with all possible orderings
     of nets.
     */
  int i, j, k, n, pos, col, edge = 0, len = NV_len(nets);
  NETptr net;
  
  for (pos = 0; pos < len; pos++)
    {
      /* Append the current element to the previous results,
         first the original, then copies of it. */
      col = 0;
      net = NV_net(nets, pos);
      NV_net(array[col++], pos) = net;
      
      while (col < edge)
        NV_net(array[col++], pos) = copy_fsm(net);
      
      /* Duplicate the results of the previous cycle POS x EDGE times
         with a copy of the current net in Nth position.
         */
      
      for (i = 0, n = 0; i < pos; i++, n++)
        for (j = 0; j < edge; j++, col++)    
          for (k = 0; k < pos; k++)
            if (k == n)
              {
                NV_net(array[col], k) = copy_fsm(net);
                NV_net(array[col], k+1) = copy_fsm(NV_net(array[j], k));
              }
            else if (k > n)
              NV_net(array[col], k+1) = copy_fsm(NV_net(array[j], k));
            else
              NV_net(array[col], k) = copy_fsm(NV_net(array[j], k));
      
      /* EDGE = [pos+1]! that is: 1, 2, 6, 24, 120, ...  */
      edge = col;
    }
}

NETptr contains_all(NVptr nets, int copy_p, int minimize_p)
{
  /* Accepts all and only strings that consist of a concatenated sequence
     of substrings drawn from the language of every net in NETS regardless
     of the order.  For example ${A,B} is the set of strings of type xy
     such that either x is in A and y is in B or x is in B and y is in A.
     The result is  equivalent to the intersection of $A and $B provided that
     the strings of A and B do not partially overlap.  Thus ${a,b} is
     equivalent to $a & $b but ${a b, b} is not equivalent to $[a b] & $b
     because the string ab is in the language of $[a b] & $b but it is not
     in ${a b, b}.
     Mathematical note:  The number of orders of labels in ${1,2,3,...n} is
     the factorial of n.
     */
  
  int i, count, size;
  NETptr net;
  NVptr *net_set, concat_nets;
  
  /* compute size and apply contains_fsm. */
  for (count = 0, size = 1; count < NV_len(nets);  count++, size *= count)
    NV_net(nets, count) =
      contains_fsm(NV_net(nets, count), copy_p, minimize_p);
  
  /* produce all permutations of the nets in the set. */
  net_set = (NVptr *) malloc((size_t) size * sizeof(NVptr));
  
  if (net_set == NULL)
    not_enough_memory("CONTAINS_ALL");
  
  for (i = 0; i < size; i++)
    {
      net_set[i] = (NVptr) malloc((size_t) sizeof(NVtype));
      if (net_set[i] == NULL)
        not_enough_memory("CONTAINS_ALL");
      
      NV_nets(net_set[i]) = (NETptr *) malloc((size_t) count * sizeof(NETptr));
      if (NV_nets(net_set[i]) == NULL)
        not_enough_memory("CONTAINS_ALL");
      
      NV_len(net_set[i]) = count;
    }
  
  scramble_nets(nets, net_set);
  
  /* concatenate every ordering to a single net, return their union */
  concat_nets = make_nv(size);
  for (i = 0; i < size; i++)
    {
      NV_net(concat_nets, i) = concat_fsm(net_set[i], DONT_COPY, minimize_p);
      free_nv_only(net_set[i]);
    }
  net = union_fsm(concat_nets, DONT_COPY, minimize_p);
  
  free((void *) net_set);
  free_nv_only(concat_nets);
  return(net);
}


static int terminal_state(STATEptr state)
{
  /* A state is terminal iff it has no outgoing arcs. */

  ARCptr arc;

  for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
    if (ARC_destination(arc) != state)
      return(FALSE);

  return(TRUE);
}

NETptr reverse_fsm(NETptr net, int copy_p, int minimize_p)
{
  /* REVERSE-FSM produces a network that recognizes the mirror image
     of the language or relation encoded by NET.
  */
  
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  /* If the start state has no arcs, no work needs to be done. */

  if (STATE_arc_set(NET_start_state(net)))
   {
     STATEptr state, new_start = NULL;
     ARCptr arc, new_arc;
     int final_state_p = FALSE;

     prune_fsm(net, DONT_COPY);

     /* If the net contains a final terminal state, that is,
     	a final state with no outgoing arcs, or a state with
	paths that all lead back to the same state,
	choose it as the new start state.

	If the original start state is final
	so is the new start state. If no final terminal state
	is found, we make a new start state. */

     for (state = NET_states(net); state; state = next_state(state))
       if (STATE_final(state) && terminal_state(state))
	 {
	   new_start = state;
	   final_state_p = TRUE;
	   break;
	 }

     prepare_arc_marks(net, 1);
     invert_arcs(net, 0);
  
     NET_deterministic(net) = FALSE;
     NET_minimized(net) = FALSE;

     if (!new_start)
       new_start = make_state(NULL, net);

     STATE_final(new_start) = STATE_final(NET_start_state(net));

     for (state = NET_states(net); state != NULL; state = next_state(state))
       {
	 arc = STATE_arc_set(state);
      
	 if (arc == NULL)
	   STATE_deterministic(state) = TRUE;
	 else if (next_arc(arc) == NULL && ARC_label(arc) != EPSILON)
	   STATE_deterministic(state) = TRUE;
	 else
	   STATE_deterministic(state) = FALSE;
      
	 if (STATE_final(state) && state != new_start)
           {
	     /* Copy all arcs from the final state to
		the new start state. The original final
		state becomes non-final. */

	     STATE_final(state) = FALSE;

	     for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	       {
		 new_arc = copy_arc(arc, net);
		 next_arc(new_arc) = STATE_arc_set(new_start);
		 STATE_arc_set(new_start) = new_arc;
	       }

             /* This original final state may be unreachable. */

             NET_pruned(net) = FALSE;

	     STATE_deterministic(new_start) = FALSE;

	     final_state_p = TRUE;
           }
       }
     STATE_final(NET_start_state(net)) = TRUE;
  
     NET_start_state(net) = new_start;
  
     /* If we did not encounter any final state. The new
     	start_state is unreachable. */

     if (!final_state_p)
       NET_pruned(net) = FALSE;

     NET_completed(net) = FALSE;
   }
  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);
  return(net);
}


NETptr brzozowski_minimize(NETptr net, int copy_p)
{
  /* Implements a variant of Brzozowski's 1962 minimization algorithm:
     reverse-determinize-reverse-determinize. We start by concatenating
     a dummy arc to the beginning of the net a and remove it after the
     operation has been done. This trick seems to make the result come
     out right in the class of problematic cases discovered by Tamás
     Gaál at XRCE. Without the added initial arc, Brzozowski minimization
     does not yield a minimal network in cases such as

     fs0 [non-det]: a -> s1, a -> s2.
         s1:   b -> s3.
         s2:   b -> s4.
         s3:   c -> fs5.
         s4:   c -> fs0.
         fs5:  (no arcs)

     which should minimize to

         fs0:  a -> s1.
         s1:   b -> s2.
         s2:   c -> fs3.

     as it does with this modified Brzozowski algorithm. The unmodified
     version produces a network with a state that is equivalent to
     fs0. The finality of the start state is not relevant. For
     example, a non-deterministic network constructed for
     [[a b]* c d | a b c d] has a non-final start state and is
     not minimizable with the unmodified Brzozowski algorithm.
  */

  ARCptr dummy_arc = NULL;
  STATEptr dummy_state = NULL;
  id_type dummy_label = DET_EPSILON;

  if (copy_p == DO_COPY)
    net = copy_fsm(net);

  if (!NET_minimized(net))
    {
      if (!(NET_deterministic(net) && NET_epsilon_free(net)))
	{

	  if (NET_optimized(net) || possibly_already_optimized_p(net))
	    {
	      handle_warning("Can't determinize an optimized network.\n",
			     "determinize_fsm", 0);
	      fsm_fprintf(stdout, "Un-optimizing ...\n");
	  
	      do_unoptimize_arcs(net);
	  
	      fsm_fprintf(stdout, "Determimizing ...\n");
	    }
	}
      prune_fsm(net, DONT_COPY);
      epsilon_remove(net, DONT_COPY);

      /* If the start state is final, set up a dummy arc pointing to
	 the start state */

      dummy_arc = make_arc(dummy_label, NET_start_state(net), net);
      dummy_state = make_state(dummy_arc, net);
      NET_start_state(net) = dummy_state;
      sigma_add_to(NET_sigma(net), dummy_label);
      binary_to_label(sigma_add_to(NET_labels(net), dummy_label));

      /* Initialize and renumber just once. */

      init_cons_mem();
      renumber_net(net);
    
      reverse_fsm(net, DONT_COPY, DONT_MINIMIZE);
      determinize_basic(net);
      reverse_fsm(net, DONT_COPY, DONT_MINIMIZE);
      determinize_basic(net);

      /* Restore the original arc labels */

      renumber_arc_labels(net, NET_labels(net));
      free_cons_mem();

      /* Don't try to use the original dummy arc and dummy state. */

      dummy_state = NET_start_state(net);
      dummy_arc = STATE_arc_set(dummy_state);
      NET_start_state(net) = ARC_destination(dummy_arc);

      /* Splice out the dummy from the state list and delete it */

      NET_states(net) = delete_state_from_list(dummy_state, NET_states(net));
      NET_num_states(net)--;

      /* Free the dummy arc and remove the dummy label from the
	 sigma and label alphabet. */

      free_arc(dummy_arc);
      NET_num_arcs(net)--;

      sigma_remove_from(NET_sigma(net), dummy_label);
      label_remove_from(NET_labels(net), dummy_label);
    }

  return(net);
}


NETptr invert_fst(NETptr net, int copy_p)
{
  /* INVERT_FST exchanges the two sides of a transducer
     and the upper_seq/lower_seq flags (1999-02-14 LK).
   */
  
  STATEptr state;
  ARCptr arc;
  ALPHABETptr labels;
  LABELptr label;
  PARSE_TBL parse_table;
  int len, i;
  int upper_seq = NET_upper_seq(net), lower_seq = NET_lower_seq(net);
  id_type inverted_tuple_id;
  
  if (copy_p == DO_COPY)
    net = copy_fsm(net);

  labels = label_to_binary(NET_labels(net));
  len = ALPH_len(labels);
  
  for (i = 0; i < len; i++)
    ALPH_item(labels,i) = 0;
  
  for (state = NET_states(net); state != NULL ; state = next_state(state))
    for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
      {
        label = id_to_label(ARC_label(arc));
        
        if (LABEL_arity(label) == 2)
	  {
	    invert_tuple(&inverted_tuple_id, label);
	    ARC_label(arc) = inverted_tuple_id;
	  }
        
        sigma_add_to(labels, ARC_label(arc));
      }
  
  NET_labels(net) = binary_to_label(labels);

  parse_table = NET_upper_parse_table(net) ;
  NET_upper_parse_table(net) = NET_lower_parse_table(net) ;
  NET_upper_parse_table(net) = parse_table ;
  update_match_tables();
  
  NET_upper_seq(net) = lower_seq;
  NET_lower_seq(net) = upper_seq;

  /* Range maps and match tables need to be recomputed */

  if (NET_uprange_map(net))
    {
      free_range_map(NET_uprange_map(net), NET_range_len(net));
      NET_uprange_map(net) = NULL;
    }

  if (NET_downrange_map(net))
    {
      free_range_map(NET_downrange_map(net), NET_range_len(net));
      NET_downrange_map(net) = NULL;
    }

  if (NET_upmatch_table(net))
    {
      free_match_table(NET_upmatch_table(net));
      NET_upmatch_table(net) = NULL;
    }

  if (NET_downmatch_table(net))
    {
      free_match_table(NET_downmatch_table(net));
      NET_downmatch_table(net) = NULL;
    }
  
  return(net);
}

NETptr complete_fsm(NETptr net, int copy_p)
{
  /* COMPLETE_FSM returns a network that has a transition for every
     symbol in sigma in every state.  Unless closed_sigma == TRUE,
     OTHER is included in the alphabet.  Adds a failure state unless
     the start state is non-final and has no arcs.  Defined only for
     simple networks, generally meaningless for transducers.
     */
  
  ALPHABETptr alph;
  ARCptr arc;
  STATEptr state, failure_state, start_state;
  id_type *items;
  int  i, len;
  long old_count = NET_num_arcs(net);
  
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  if (NET_completed(net))
    return(net);
  
  if (NET_arc_label_arity(net) > 1 && FSM_MODE != TWOL_MODE)
    {
      fsm_fprintf(stderr, "Complete is not implemented for transducers.  Aborting.\n");
      return(net);
    }
  
  label_to_binary(sigma(net));
  
  start_state = NET_start_state(net);
  
  if (STATE_arc_set(start_state) == NULL && !STATE_final(start_state))
    failure_state = start_state;
  else
    failure_state = make_state(NULL, net);
  
  if (NET_closed_sigma(net))
    alph = NET_sigma(net);
  else
    alph = sigma_add_to(NET_sigma(net), OTHER);
  
  len = ALPH_len(alph);
  items = ALPH_items(alph);
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      /* first sweep the arcs to mark labels in sigma that already have
         arcs in the state.  turn 1's to 2's for for labels that are found.
         */
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
        items[ARC_label(arc)] = 2;
      
      /* now sweep the labels in sigma turning 2's back to 1's and adding
         arcs for labels that did not get found.
         */    
      for (i = 0; i < len; i++)
        if (items[i] == 1)
          {
            arc = make_arc(i, failure_state, net);
            next_arc(arc) = STATE_arc_set(state);
            STATE_arc_set(state) = arc;
          }
        else if (items[i] == 2)
          items[i] = 1;
    }
  /* If there are EPSILON arcs, EPSILON is now in SIGMA.
     Remove it and make sure the flag bit is correctly set.
     This should never occur!
     */
  
  if (items[EPSILON])
    {
      handle_warning("EPSILON in the sigma", "COMPLETE_NET", 0);
      items[EPSILON] = 0;
      if (NET_epsilon_free(net))
        {
          NET_epsilon_free(net) = FALSE;
          handle_warning("EPSILON found in a net marked as epsilonfree.",
                         "COMPLETE_NET", 0);
        }
    }
  
  if (NET_num_arcs(net) > old_count)
    {
      /* new arcs were added */
      NET_loop_free(net) = FALSE;
      NET_pruned(net) = FALSE;
      NET_minimized(net) = FALSE;
      
      /* new arcs may have labels that did not previously occur in NET */
      label_augment(NET_labels(net), NET_sigma(net));
    }
  else if (failure_state != start_state)
    {
      NET_states(net) = next_state(failure_state);
      NET_num_states(net)--;
      free_state(failure_state);
    }
  
  NET_completed(net) = TRUE;
  
  return(net);
  
}

static STATEptr find_next_fail_state(id_type id, STATEptr fail_state, NETptr net)
{
  ARCptr arc;

  for (arc = STATE_arc_set(fail_state); arc; arc = next_arc(arc))
    if (ARC_label(arc) == id)
      break;

  if (!arc)
    {
      arc = make_arc(id, make_state(NULL, net), net);
      push_arc(arc, fail_state);
     
    }

  return(ARC_destination(arc));
}

static STATEptr find_fail_partition(STATEptr state, id_type *items, uint16 len,
				    STATEptr fail_state, STATEptr final_state,
				    id_type fail_tag, NETptr net)
{
   /* Sweep the labels in items turning 2's back to 1's and adding
      arcs for labels that did not get found. 1 indicates a missing arc.
      If the current fail state has an arc for the label, continue
      processing on its destination, otherwise, create a new fail state.
      */    
 

  id_type id;

  for (id = 0; id < len; id++)
    switch(items[id])
      {
      case 2:
	items[id] = 1;
	break;
      case 1:
	fail_state = find_next_fail_state(id, fail_state, net);
	break;
      default:
	break;
      }

  /* If the state is non-final, look for an altchain arc. If
     it is a new state, that is, with an empty arc set, add a
     fail_tag transition to the final state.
     */

  if (final_state && !STATE_final(state))
    {
      fail_state = find_next_fail_state(ALTCHAIN_SYMBOL, fail_state, net);

      if (!STATE_arc_set(fail_state))
	push_arc(make_arc(fail_tag, final_state, net), fail_state);
    }


  return(fail_state);
}

static void prune_fail_partition(NETptr net)
{
  /* Fail partition is a tree rooted at failure_state. Each branch of
     the tree represents a set of partitions of absent labels.
     We can eliminate nodes that (1) do not have outgoing arcs
     leading to different destinations and (2) have not been marked.
     (A marked state has an incoming altchain arc from some state
     outside the fail tree.)
     N.B. At this point the state list of the net only contains
     fail states, the regular states have been temporarily
     disconnected.
     */

  STATEptr state, prev, next, dest, dest2;
  ARCptr arc, arc2;

  for (state = NET_states(net); state; state = next_state(state))
    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
      {
	dest = ARC_destination(arc);

	if (STATE_visit_mark(dest) != 1 && STATE_arc_set(dest))
	  {
	    dest2 = NULL;

	    for (arc2 = STATE_arc_set(dest); arc2; arc2 = next_arc(arc2))
	      if (!dest2)
		dest2 = ARC_destination(arc2);
	      else if (dest2 != ARC_destination(arc2))
		break;

	    /* If we go through the previous loop without breaking,
	       every arc of the destination state leads to the same
	       place. Because the destination has no incoming arcs
	       from the outside (visit mark is 0), we can eliminate
	       it.
	       */

	    if (!arc2)
	      {
		/* Redirect the current arc to the new destination. */

		ARC_destination(arc) = dest2;

		/* Peel off the arcs of the destination and push
		   them to the current state. We can discard any
		   altchain arcs. (An upward pointing altchain
		   arc leads to state that contains a fail tag
		   transition to the final state.
		   */

		while ((arc2 = STATE_arc_set(dest)))
		  {
		    STATE_arc_set(dest) = next_arc(arc2);
		    next_arc(arc2) = STATE_arc_set(state);
		    STATE_arc_set(state) = arc2;
		  }

		/* The destination state is now empty. Mark it for deletion.*/

		STATE_visit_mark(dest) = 2;
	      }
	  }
      }

  /* Prune unreachable states. */

  prev = NULL;

  for (state = NET_states(net); state; state = next)
    {
      next = next_state(state);

      if (STATE_visit_mark(state) == 2)
	{
	  if (prev)
	    next_state(prev) = next_state(state);
	  else
	    NET_states(net) = next_state(state);

	  free_state(state);
	  NET_num_states(net)--;
	}
      else
	{
	  prev = state;
	  STATE_visit_mark(state) = 0;
	}
    }
}

static void invert_failure_partition(NETptr net, STATEptr failure_state,
				     id_type fail_tag)
{
  /* Invert the failure tree so that the outgoing arcs in each
     state are directed to the failure state and append to the
     original destination state a backwards altchain arc.
     N.B. At this point, the state list of the net contains only
     failure states, the ordinary states have been temporarily
     disconnected.
     */

  STATEptr state, dest;
  ARCptr arc, next, back_arc;

  for (state = NET_states(net); state; state = next_state(state))
    for (arc = STATE_arc_set(state); arc; arc = next)
      {
	next = next_arc(arc);

	dest = ARC_destination(arc);
   
	/* If the arc is an altchain arc, its destination
	   is pointed to by a nonfinal state in the original net
	   and has an arc pointing to the final state.
	   In that case, we just invert the altchain arc.
	   */

	if (ARC_label(arc) == ALTCHAIN_SYMBOL)
	  {
	    ARC_destination(arc) = state;
	    next_arc(arc) = (ARCptr) STATE_client_cell(dest);
	    STATE_client_cell(dest) = (void *) arc;
	  }
	else if (ARC_label(arc) == fail_tag)
	  {
	    /* If the arc is a fail tag arc, we keep it
	       as it is.
	       */
	    next_arc(arc) = (ARCptr) STATE_client_cell(state);
	    STATE_client_cell(state) = (void *) arc;
	  }
	else
	  {
	    /* If the state is not the failure state, and
	       the client cell of the arc's destination is
	       still empty, we push a backward altchain
	       arc to its client cell.
	       */
	
	    if (!(state == failure_state || STATE_client_cell(dest)))
	      {
		back_arc = make_arc(ALTCHAIN_SYMBOL, state, net);
		STATE_client_cell(dest) = (void *) back_arc;
	      }
       
	    /* Make the original arc point to the failure state.
	       and push it to the client cell of the destination.
	       */

	    ARC_destination(arc) = failure_state;
	    next_arc(arc) = (ARCptr) STATE_client_cell(dest);
	    STATE_client_cell(dest) = (void *) arc;
	  }
      }

  /* Replace the original arcs with new arcs temporarily stored
     in the client cells. Make the altchain arc be the final arc
     of the state;
     */

  for (state = NET_states(net); state; state = next_state(state))
    {
      STATE_arc_set(state) = (ARCptr) STATE_client_cell(state);
      STATE_client_cell(state) = NULL;

      if ((next = STATE_arc_set(state)))
	{
	  /* Look for a nonfinal altchain arc. */

	  back_arc = NULL;
	  arc = NULL;

	  while(next_arc(next))
	    {
	      if (ARC_label(next) == ALTCHAIN_SYMBOL)
		{
		  if (back_arc)
		    handle_warning("Two altchain arcs in one state.",
				   "COMPLETE_SIDE", ARC_label(arc));
		  else
		    {
		      /* Splice out the nonfinal altchain arc. */

		      back_arc = next;
		      if (arc)
			next_arc(arc) = next_arc(next);
		      else
			STATE_arc_set(state) = next_arc(next);
		    }
		}
	      else
		arc = next;

	       next = next_arc(next);
	    }

	  /* If we found a nonfinal ALTCHAIN arc in the list, append it
	     to the final arc.
	     */

	  if (next && back_arc)
	    {
	      next_arc(next) = back_arc;
	      next_arc(back_arc) = NULL;
	    }
	}
    }
}

NETptr complement_side(NETptr net, int side, id_type fail_tag, int copy_p)
{
  /* COMPLEMENT_SIDE returns the union of the original network with
     its complement on the chosen side. If the string is in the
     original network, it will appear in the result unchanged. All
     other strings are tagged with FAIL_TAG to distinguish them from
     the original language:

     - FAIL_TAG equal to ID_NO_SYMBO, means no FAIL_TAG;
     - If FAIL_TAG is an fstpair containing an EPSILON on chosen side, 
     for example, +Unknown:0, the language on that side of the network 
     is SIGMA*: it contains every string.

     COMPLEMENT_SIDE adds OTHER to the alphabet unless NET_closed_sigma(net)
     is TRUE. Adds a failure state unless the start state is empty and
     non-final. Uses ALTCHAIN_SYMBOL as the last transition of a state
     to minimize the number of new arcs.
     */

  NETptr net_copy = NULL;
  ALPHABETptr alph;
  ARCptr a, arc;
  STATEptr state, failure_state, start_state, tail_state,
  ordinary_states, final_state = NULL;
  id_type *items;
  int  i, len;
  long old_arc_count = NET_num_arcs(net);
  long old_state_count = NET_num_states(net);
  
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  if (NET_completed(net))
    return(net);

  if (fail_tag == EPSILON || fail_tag == ALTCHAIN_SYMBOL || fail_tag == OTHER)
    {
      handle_warning("EPSILON, OTHER, and ALTCHAIN_SYMBOL cannot be used as a fail_tag. Aborting.",
		     "COMPLETE_SIDE", 0);
      return(net);
    }

  if (NET_arc_label_arity(net) == 2)
    {

      fsm_fprintf(stderr, "Copying the original network...");
      net_copy = copy_fsm(net);

      switch (side)
	{
	case UPPER:
	  fsm_fprintf(stderr, "\nExtracting the upper side of the network...");
	  net = upper_fsm(net, DONT_COPY, DO_MINIMIZE);
	  break;
	case LOWER:
	  fsm_fprintf(stderr, "\nExtracting the lower side of the network...");
	  net = lower_fsm(net, DONT_COPY, DO_MINIMIZE);
	  break;
	default:
	  handle_error("Side neither UPPER nor LOWER", "COMPLETE_SIDE",
		       side);
	  break;
	}

      fsm_fprintf(stderr, "\nComplementing...");
    }

  label_to_binary(sigma(net));
  
  start_state = NET_start_state(net);
  
  /* If we have a fail_tag label, add its components to SIGMA.
     Make a final state but do not add it to the state list yet.
   */

  if (fail_tag !=0 && fail_tag != ID_NO_SYMBOL)
    {
      label_to_sigma(NET_sigma(net), fail_tag);
      final_state = alloc_state();
      STATE_final(final_state) = TRUE;
      STATE_deterministic(final_state) = TRUE;
    }

  if (STATE_arc_set(start_state) == NULL && !STATE_final(start_state)
      && start_state == NET_states(net))
    failure_state = start_state;
  else
    failure_state = make_state(NULL, net);

  /* Disconnect the failure state from the rest of the network until
     we are done.
     */

  ordinary_states = next_state(failure_state);
  next_state(failure_state) = NULL;
  
  if (NET_closed_sigma(net))
    alph = NET_sigma(net);
  else
    alph = sigma_add_to(NET_sigma(net), OTHER);
  
  len = ALPH_len(alph);
  items = ALPH_items(alph);
  
  for (state = ordinary_states; state != NULL; state = next_state(state))
    {

      if ((arc = STATE_arc_set(state)) || STATE_final(state))
	{
	  /* first sweep the arcs to mark labels in sigma that already have
	     arcs in the state.  turn 1's to 2's for for labels that are found.
	     */

	  while (arc)
	    {
	      items[ARC_label(arc)] = 2;
	      arc = next_arc(arc);
	    }

	  /* now sweep the labels in sigma turning 2's back to 1's and
	     find the failure partition for the state. If there are no
	     missing arcs, the returned tail_state is the failure_state
	     itself, representing the empty set in the partition.
	     */

	  tail_state = find_fail_partition(state, items, len, failure_state,
					   final_state, fail_tag, net);

	  if (tail_state != failure_state)
	    {
	      /* Make an altchain arc. Mark the tail_state so that
		 we know it cannot be pruned later.
		 */

	      arc = make_arc(ALTCHAIN_SYMBOL, tail_state, net);
	      STATE_visit_mark(tail_state) = 1;

	      /* Append the new arc to the current state's arcset. */

	      if ((a = STATE_arc_set(state)))
		{
		  while (next_arc(a)) {
		    a = next_arc(a) ;
		  }
	      
		  next_arc(a) = arc;
		}
	      else
		STATE_arc_set(state) = arc;
	    }
	}
      else
	{
	  /* State has no arcs. Link directly to failure state. */
	  push_arc(make_arc(ALTCHAIN_SYMBOL, failure_state, net), state);
	}

      if (net_copy && STATE_final(state))
	STATE_final(state) = FALSE;
    }

  /* If there are EPSILON arcs, EPSILON is now in SIGMA.
     Remove it and make sure the flag bit is correctly set.
     This should never occur!
     */
  
  if (items[EPSILON])
    {
      handle_warning("EPSILON in the sigma", "COMPLETE_SIDE", 0);
      items[EPSILON] = 0;
      if (NET_epsilon_free(net))
        {
          NET_epsilon_free(net) = FALSE;
          handle_warning("EPSILON found in a net marked as epsilonfree.",
                         "COMPLETE_SIDE", 0);
        }
    }
  
  if (NET_num_arcs(net) > old_arc_count)
    {
      /* New arcs were added. The network was not already complete. */

      /* Prune failure tree. */

      prune_fail_partition(net);

      /* Invert failure tree. */

      invert_failure_partition(net, failure_state, fail_tag);

      /* Restore the ordinary states to the state list. */
      
      next_state(failure_state) = ordinary_states;

      /* If we have a fail_label, add a transition to a final state.
	 Otherwise, mark net as unpruned and non-minimal.
	 */

      if (fail_tag != 0 && fail_tag != ID_NO_SYMBOL)
	{
	  /* Add the final state to the state list. */

	  next_state(final_state) = NET_states(net);
	  NET_states(net) = final_state;
	  NET_num_states(net)++;

	  /* Make an arc from the failure state to the new
	     final state. All strings not recognized by the
	     original network will be marked with the fail_tag.
	     */

	  push_arc(make_arc(fail_tag, final_state, net), failure_state);

	  if (LABEL_arity(id_to_label(fail_tag)) >
	      NET_arc_label_arity(net))
	    NET_arc_label_arity(net) = LABEL_arity(id_to_label(fail_tag));

	  /* And fail_tag to labels. */

	  if (fail_tag != 0 && fail_tag != ID_NO_SYMBOL)
	    sigma_add_to(NET_labels(net), fail_tag);

	  fsm_fprintf(stderr, "%ld states, %ld arcs added.\n",
		  NET_num_states(net) - old_state_count,
		  NET_num_arcs(net) - old_arc_count);

	}
      else
	{
	  /* The failure state becomes final. */

	  STATE_final(failure_state) = TRUE;
	}

      /* Add looping arcs to the failure state. */
      
      NET_loop_free(net) = FALSE;

      for (i = 0; i < len; i++)
	if (items[i])
	  push_arc(make_arc(i, failure_state, net), failure_state);

      /* Add ALTCHAIN_SYMBOL to sigma. */

      items[ALTCHAIN_SYMBOL] = 1;

      /* All symbols in sigma now appear on the net. */

      label_augment(NET_labels(net), NET_sigma(net));

      if (net_copy)
	{
	  NVtype nv;
	  NETptr nets[2];

	  nv.len = 2;
	  nv.nets = nets;

	  nets[0] = net_copy;
	  nets[1] = net;

	  fsm_fprintf(stderr, "Merging original net with the complement...");
	  
	  net = union_fsm(&nv,DONT_COPY, DO_MINIMIZE);
	  fsm_fprintf(stderr, "\nDone.\n");
	}
    }
  else
    {
      /* Free the disconnected final state, if any. */
      if (final_state)
	free_state(final_state);

      if (net_copy)
	free_network(net_copy);

      if (failure_state != start_state)
	{
	  /* Prune the useless failure state. */

	  free_state(failure_state);
	  NET_num_states(net)--;
	  NET_states(net) = ordinary_states;

	}
      else
	{
	  /* Just reconnect the start state to the network. */
      
	  next_state(failure_state) = ordinary_states;
	}
    }

  NET_completed(net) = TRUE;
  
  return(net);
}

NETptr negate_fsm(NETptr net, int copy_p, int minimize_p)
{
  /* NEGATE_FSM returns a machine that recognizes the complement of net.
     Warns if called on a transducer because the result in general is
     meaningful only for simple fsms.  Use NEGATE_NET when the warning
     is not appropriate. */

  if (NET_arc_label_arity(net) > 1 && FSM_MODE != TWOL_MODE)
  {
  	complement_undefined_msg();
    if (copy_p == DONT_COPY)
    	free_network(net);
    return(null_fsm());
  }
	else
  	return(negate_net(net, copy_p, minimize_p));
}

NETptr negate_net(NETptr net, int copy_p, int minimize_p)
{
  /* NEGATE_NET returns a machine that recognizes the complement of net.
     The net should represent a simple language or an equal-length
     relation. Other kinds of regular relations are not closed
     under complementation. If net represents a relation, we
     compute the complement of A as [[?:?]* - A]. The
     complete_fsm() function is not defined for relations.
  */

    if (NET_optimized(net) || possibly_already_optimized_p(net))
    {
        handle_warning("Can't compute the negation of an optimized network.\n",
		     "negate_net", 0);
        fsm_fprintf(stdout, "Un-optimizing ...\n");

        do_unoptimize_arcs(net);
    }
  
  if (NET_arc_label_arity(net) > 1 && FSM_MODE == STANDARD_MODE)
    return(minus_fsm(universal_equal_length_relation(),
		     net, DO_RECLAIM, copy_p, minimize_p));
  else
    {
      STATEptr state;

      /* Need to determinize because a state might have two arcs with
	 the same label, one leading to a final state, the other leading
	 to a nonfinal state. Without determinization, the latter path
	 would remain in the result.
      */
      net = complete_fsm(determinize_fsm(net, copy_p), DONT_COPY);

      for (state = NET_states(net); state != NULL; state = next_state(state))
	STATE_final(state) = !STATE_final(state);
  
      NET_pruned(net) = FALSE;
      NET_minimized(net) = FALSE;
  
      if (minimize_p == DO_MINIMIZE)
	min_fsm(net, DONT_COPY);
  
      return(net);
    }
}

NETptr substring_fsm(NETptr net, int copy_p, int minimize_p)
{
  /* Returns a network that accepts every string that is a substring
     of some string in the language of NET.
     */
  STATEptr state, dest, start_state;
  ARCptr arc1, arc2, last_arc;
  id_type id;
  
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  else
    clear_client_cells(net);
  
  start_state = NET_start_state(net);
  
  /* Find the last arc of the start state. */
  
  last_arc = STATE_arc_set(start_state);
  
  if (last_arc == NULL)
    return(net);
  
  while (next_arc(last_arc) != NULL)
    last_arc = next_arc(last_arc);
  
  /* Push a copy of every arc into the client cell of its destination
     state, unless it already contains an arc with the same label.
     */
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      /* Every state is final. */
      
      STATE_final(state) = TRUE;
      
      for (arc1 = STATE_arc_set(state); arc1 != NULL; arc1 = next_arc(arc1))
        {
          dest = ARC_destination(arc1);
          id = ARC_label(arc1);
          
          arc2 = (ARCptr) STATE_client_cell(dest);
          
          while (arc2 != NULL && ARC_label(arc2) != id)
            arc2 = next_arc(arc2);
          
          /* If an arc with a matching label was not found
             make a copy of arc1 and push it to the
             client cell of its destination.
             */
          
          if (arc2 == NULL)
            {
              arc2 = copy_arc(arc1, net);
              next_arc(arc2) = (ARCptr) STATE_client_cell(dest);
              STATE_client_cell(dest) = (void *) arc2;
            }
        }
    }
  /* Append the arc chains from the client cells to the start state. */
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      next_arc(last_arc) = (ARCptr) STATE_client_cell(state);
      STATE_client_cell(state) = NULL;
      while (next_arc(last_arc) != NULL)
        last_arc = next_arc(last_arc);
    }
  
  STATE_deterministic(start_state) = FALSE;
  NET_deterministic(net) = FALSE;
  NET_minimized(net) = FALSE;
  
  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);
  
  return(net);
}


/**************************************
 *
 *  Extracting an fsm from an fst
 *
 ***************************************/


static void reclaim_cashed_label_data(NETptr net)
{
  /* Reclaims parse tables and flag register. This needs to be done
     by functions such as union_fsm() and concat_fsm() that
     may introduce new symbols to the alphabet of the
     network invalidating the data stored in these registers
     as a result of a previous call to apply_to_string().
     */
  free_parse_table(NET_upper_parse_table(net));
  NET_upper_parse_table(net) = NULL;
  free_parse_table(NET_lower_parse_table(net));
  NET_lower_parse_table(net) = NULL;
  free_alph(NET_flag_register(net));
  NET_flag_register(net) = NULL;
}


static NETptr fst_to_fsm(NETptr net, int copy_p, int minimize_p, int compact_sigma_p, int field)
{
  STATEptr state;
  ARCptr arc;
  id_type old_id, new_id;
  int changed_p = FALSE;
  ALPHABETptr sig, labs;
  
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  if (NET_arc_label_arity(net) == 1)
    return(net);

  if (copy_p == DONT_COPY)
    reclaim_cashed_label_data(net);

  sig = label_to_binary(sigma(net));
  labs = label_to_binary(NET_labels(net));
  
  /* relabel arcs and reconstitute the alphabet */
  
  /* The SIGMA of the new machine is the same as the SIGMA of
     the the original network except that, if the extraction
     of one side is done in TWOL_MODE, we remove all fstpairs
     from SIGMA and add the extracted component.
    
     This changes the original policy of just taking the
     structural sigma of the result net. L.K. 94-02-25.
     */

  for (state = NET_states(net); state != NULL; state = next_state(state))
    for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
      {
        old_id = ARC_label(arc);
        new_id = get_field(id_to_label(old_id), field);
        
        if (old_id != new_id)
          {
            ARC_label(arc) = new_id;
            changed_p = TRUE;

	    if (FSM_MODE == TWOL_MODE)
	      sigma_remove_from(sig, old_id);

            sigma_remove_from(labs, old_id);
            
            STATE_deterministic(state) = FALSE;
            if (new_id == EPSILON)
              NET_epsilon_free(net) = FALSE;
            else
              {
		if (FSM_MODE == TWOL_MODE)
		  sigma_add_to(sig, new_id);
                sigma_add_to(labs, new_id);
              }
          }
      }
  
  NET_arc_label_arity(net) = 1;
  NET_left_bimachine(net) = FALSE;
  NET_right_bimachine(net) = FALSE;
  NET_upper_seq(net) = FALSE;
  NET_lower_seq(net) = FALSE;

  binary_to_label(labs);
 
  if (changed_p)
    {
      NET_deterministic(net) = FALSE;
      NET_minimized(net) = FALSE;
    }
  
  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);
  else
    if (!NET_epsilon_free(net))
      epsilon_remove(net, DONT_COPY);
  
  if (compact_sigma_p)
    compact_sigma(net);

  return(net);
}


NETptr upper_fsm(NETptr net, int copy_p, int minimize_p)
{
  return(fst_to_fsm(net, copy_p, minimize_p, TRUE, UPPER));
}

NETptr lower_fsm(NETptr net, int copy_p, int minimize_p)
{
  return(fst_to_fsm(net, copy_p, minimize_p, TRUE, LOWER));
}

/* compact_sigma() prunes the net: sometimes it is not desirable TG 99-1-5 */
NETptr upper_fsm_no_compact(NETptr net, int copy_p, int minimize_p)
{
  return(fst_to_fsm(net, copy_p, minimize_p, FALSE, UPPER));
}

NETptr lower_fsm_no_compact(NETptr net, int copy_p, int minimize_p)
{
  return(fst_to_fsm(net, copy_p, minimize_p, FALSE, LOWER));
}


NETptr ignore_fsm(NETptr net, NETptr fluff, int copy_net_p, int copy_fluff_p,
                  int minimize_p)
{
  /* Returns a network which is like net except that every state
     of the new fsm contains a FLUFF loop back to itself.  If COPY_NET_P
     is DO_COPY, NET is always copied, DONT_COPY means that NET becomes the
     result. If COPY_FLUFF_P is DO_COPY, FLUFF is copied if any inserting is
     done, DONT_COPY means that FLUFF is incorporated in the result or freed.
     The result is a null fsm if either NET or FLUFF is a null fsm, just
     as in concatenation.
     */
  
    if (copy_net_p == DO_COPY)
    net = copy_fsm(net);
  
  if (!null_fsm_p(net))
  {
      /* Minimize fluff without making a copy. */

      fluff = min_fsm(fluff, DONT_COPY);
  
      align_sigmas(net, fluff);
  
      if (STATE_arc_set(NET_start_state(fluff)) != NULL)
      {
          STATEptr state;
  
          sigma_augment(sigma(net), sigma(fluff));
          label_augment(NET_labels(net), NET_labels(fluff));
      
          for (state = NET_states(net); state != NULL;
               state = next_state(state))
              if (next_state(state) == NULL && copy_fluff_p == DONT_COPY)
                  splice_fsm(net, state, state, fluff, FALSE, ID_NO_SYMBOL);
              else
                  splice_fsm(net, state, state, copy_fsm(fluff), FALSE, ID_NO_SYMBOL);
      }
      else
      {
          /* No arcs in fluff. If it is an epsilon fsm, there is
             nothing to do. If it is a null fsm, we return a
             null fsm just as in concatenation.
             */

          if (!(STATE_final(NET_start_state(fluff))))
          {
              /* Null fsm! */
           
              free_network(net);
              net = null_fsm();
          } 

          if (copy_fluff_p == DONT_COPY)
              free_network(fluff);
      }
  }
  else
    {
      /* Free the fluff and return the net unchanged. */

          if (copy_fluff_p == DONT_COPY)
              free_network(fluff);
    }

  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);
  
  return(net);
}

#if NOT_USED
static void sigma_update(NETptr net, id_type id)
{
   /* ID is assumed to be the id of an atomic symbol. Checks if it
      is in the sigma and adds it, if necessary.
      */
 
  if (!sigma_member(sigma(net), id))
    {
      if (sigma_member(sigma(net), OTHER))
	{
	  ALPHABETptr alph =
            make_alph(LIM_id_count(LABEL_MAP), BINARY_VECTOR);
        
	  ALPH_item(alph, id) = 1;
            
	  expand_other(net, alph, DONT_COPY);
	  free_alph(alph);
	}
      else
	label_to_sigma(sigma(net), id);
    }
}
#endif /* NOT_USED */

static void manage_start_and_final_states(NETptr net)
{
	/* 	Nonempty final states need to be split into a final
		and a nonfinal state. We look for an empty final state in the
		network and create one if such a state does not yet exist.
		We make the original state nonfinal and store the empty
		final state temporarily on the client cell of the original one
		so that we can duplicate the arcs leading to the the original.
	    Client cells don't have to be clean.
	*/
	
	STATEptr state, state2, start_state = NET_start_state(net);
	int split_p = FALSE;
	ARCptr arc, arc2;

	/* If the start state has arcs pointing back to it, we make a
		new start state copying the arcs from the original one.
	*/
	
	if (start_loops_p(net))
	{
		start_state = copy_state(start_state, net);
		NET_start_state(net) = start_state;
		NET_minimized(net) = FALSE;
	}

	
	
	for (state = NET_states(net); state; state = next_state(state))
	{
		if (STATE_final(state) && state != start_state && STATE_arc_set(state))
		{
			for (state2 = NET_states(net); state2; state2 = next_state(state2))
				if (STATE_final(state2) && !STATE_arc_set(state2))
					break;
						
			if (!state2)
			{
				state2 = (void *) make_state(NULL, net);
				STATE_final(state2) = TRUE;
			}
				
			STATE_client_cell(state) = (void *) state2;
			STATE_final(state) = FALSE;
			split_p = TRUE;
		}
		else
			STATE_client_cell(state) = NULL;
	}
	
	/* If we some state was split, then every state that has arcs leading
	   to the original must also have the arcs leading to the new one. */
	   
	 if (split_p)
	 {
		for (state = NET_states(net); state; state = next_state(state))
		 	for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	 		{
				state2 = (STATEptr) STATE_client_cell(ARC_destination(arc));
	 		
				if (state2)
	 			{
	 				/* Arc points to a state that was split. Add a new arc
	 			   	to its counterpart. */
	 			   
	 			  	arc2 = make_arc(ARC_label(arc), state2, net);
	 			  	push_arc(arc2, state);
	 			  	STATE_deterministic(state) = FALSE;
	 			}
			}
		
		clear_client_cells(net);
	 	NET_deterministic(net) = FALSE;
	 	NET_minimized(net) = FALSE;
	}	
}


static id_type ignore_upper(id_type id)
{
  id_type tuple_id;

  make_fstlabel(&tuple_id, id, EPSILON);
  return(tuple_id);
}

static id_type ignore_lower(id_type id)
{
  id_type tuple_id;

  make_fstlabel(&tuple_id, EPSILON, id);
  return(tuple_id);
}

static id_type ignore_both(id_type id)
{
  return(id);
}


static ALPHABETptr make_aux_alph(ALPHABETptr alph, int side)
{
  /* Returns a copy of ALPH if SIDE is BOTH_SIDES except that any
     duplicates are eliminated. If SIDE is UPPER, returns an
     alphabet of fstpairs with the original ALPH symbol on the
     upper side and an epsilon on the lower side; the inverse
     of that for LOWER.
     We return AUX as a label vector but make it first in the binary
     form to avoid duplicates if ALPH by some mistake contains the
     same symbol twice.
  */
  uint16 i, len;
  id_type (*label_fn)(id_type);
  ALPHABETptr aux;
	
  binary_to_label(alph);
  len = max_label_item(ALPH_items(alph), ALPH_len(alph)) + 1;
	
  aux = make_alph(len, BINARY_VECTOR);
	
  switch(side)
    {
    case UPPER:
      label_fn = ignore_upper;
      break;
    case LOWER:
      label_fn = ignore_lower;
      break;
    case BOTH_SIDES:
      label_fn = ignore_both;
      break;
    default:
      handle_error("Bad side_p argument", "IGNORE_ALPH", side);
    }

  /* It the side_p argument is UPPER or LOWER, ALPH must consist of
     single symbols because they will be paired with an epsilon.
     If this is not the case, label_fn will generate an error. */
		
  for (i = 0; i < ALPH_len(alph); i++)
    {
      sigma_add_to(aux, label_fn(ALPH_item(alph, i)));
    }
		
  binary_to_label(aux);
		
  return(aux);
}

NETptr ignore_alph(NETptr net, ALPHABETptr alph, int copy_p, int min_p,
		   int where, int side, int expand_other_p)
{
  /* Inserts a looping arc at each state for every label in alph unless
     the state already has an outgoing arc for that symbol. If the
     internal_p argument is INTERNALLY, the arcs are added only in internal
     positions; if it is EVERYWHERE, the ignore loops are introduced
     everywhere.
     ignore_alph(net, alph, copy_p, minimize_p, EVERYWHERE, ...)
     is generally equivalent to but more lightweight than
     ignore_fsm(net, sigma_fsm(alph), copy_p, DONT_COPY, minimize_p)
     because it deals with the labels directly without calling splice_fsm()
     to insert a network.
     For the same reason,
     ignore_alph(net, alph, copy_p, minimize_p, INTERNALLY, ...)
     is generally equivalent to but more lightweight than
     ignore_intern_fsm(net, sigma_fsm(alph), copy_p, DONT_COPY);
     I say 'generally' because it depends on whether the alphabet is
     empty or not. If alph is empty, sigma_fsm(alph) is a null fsm.
     In this case ignore_fsm() returns a null fsm but ignore_alph()
     returns its first argument as there are no labels to ignore.
     For ignore_fsm(), a null fsm is different from epsilon fsm,
     although both have empty alphabets. We choose to interpret the
     empty alphabet as if it came from an epsion fsm. One could have
     it the other way round, too, because it is not likely to make any
     difference in practice. 1998-11-17 LK.
   	  
     The SIDE argument should be UPPER, LOWER, or BOTH_SIDES.
     In the case of UPPER and LOWER, the looping arcs are labeled with
     symbol pairs that contain an ALPH symbol on the specified side and
     an epsilon on the opposite side. No symbol pairs are constructed
     if side_p == BOTH_SIDES. BOTH_SIDES is the only legal argument
     if ALPH contains symbol pairs.
   	  
     The expand_p argument should be DO_EXPAND unless the calling function
     has already taken care of the expansion of OTHER or if the alphabet
     consists of auxiliary symbols that are handled in a special way,
     in this case it should be DONT_EXPAND.
     */

  int i, restore_p = FALSE, changed_p = FALSE;
  STATEptr state;
  ARCptr arc;
  ALPHABETptr aux, bin_aux, aux_record, syms;
  id_type id;
	
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
		
  /* As there is nothing to ignore, we return NET. See the comment above.
   */
  if (ALPH_len(alph) == 0)
    return(net);
	  
  if (ALPH_type(alph) == BINARY_VECTOR)
    {
      /* We are anticipating that alph is in the label format, if it
	 isn't we'll convert it here and convert back again at the end.
	 */
      binary_to_label(alph);
      restore_p = TRUE;
    }
	
  /* If the calling function has not dealt with the expansion of
     OTHER, we do it here. */
		
  if (expand_other_p)
    {
      /* If the NET contains other, we need to expand it with
	 respect to the label components of ALPH. */
	
      if (sigma_member(NET_sigma(net), OTHER))
	{	
	  syms = label_components(alph);
	  expand_other(net, syms, DONT_COPY);
	  free_alph(syms);
	}
	
      /* If the labels in ALPH include OTHER, ALPH needs to be expanded
	 taking into account NET's sigma. */
	
      if (other_in_alph_p(alph))
	alph = expand_other_in_alph(alph, NET_sigma(net), DONT_COPY);
    }
	
  /* AUX is a copy of ALPH when side is BOTH_SIDES, a list of symbol
     pairs with an epsilon on the opposite side when side is UPPER
     or LOWER. BIN_AUX and AUX_RECORD are used in checking whether
     the state already has an arc labeled with an AUX symbol. */
		
  aux = make_aux_alph(alph, side);
  bin_aux = label_to_binary(copy_alphabet(aux));
  aux_record = make_alph(ALPH_len(bin_aux), BINARY_VECTOR);
	
  /* If we are going to only ignore internal labels, some preliminary
     work may have to be done. This concerns nets whose start state
     has incoming arcs and nets with non-empty final states.
     */
	   
  if (where == INTERNALLY)
    manage_start_and_final_states(net);
	
  /* Add the looping arcs for the symbols to be ignored. If we
     are ignoring INTERNALLY, we skip the start state and the final
     states. */
	   
  for (state = NET_states(net); state; state = next_state(state))
    {			
      if ((where == EVERYWHERE ||
	   !(state == NET_start_state(net) || STATE_final(state))))
	{
		
	  /* First: mark the auxiliary symbols that have the same label
	     as some outgoing arc so that we know to skip them in
	     the loop below.
	     */			 
	
	  for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	    if (sigma_member(bin_aux, ARC_label(arc)))
	      ALPH_item(aux_record, ARC_label(arc)) = 1;
				
	  /* Second: Add looping aux arcs for labels that do
	     not yet appear on any outgoing arc. */
			
	  for (i = 0; i < ALPH_len(aux); i++)
	    {
	      id = ALPH_item(aux, i);
	 		  		
	      if (ALPH_item(aux_record, id) == 0)
		{
		  arc = make_arc(id, state, net);
		  push_arc(arc, state);
		  changed_p = TRUE;
		}
	      else
		ALPH_item(aux_record, id) = 0;
	    }
	}
    }

  if (changed_p)
    {
      /* Update the sigma and label alphabets, mark as not loopfree. */
		
      NET_loop_free(net) = FALSE;
		
      label_to_binary(NET_labels(net));
      label_to_binary(NET_sigma(net));
		
      for (i = 0; i < ALPH_len(aux); i++)
	{
	  id = ALPH_item(aux, i);

	  /* Don't augment the sigma alphabet with the
	     components of the symbol pair if we are
	     in TWOL_MODE, just add the pair itself.
	     Bug fix. 2001-10-03 LK. */

	  if (tuple_id_p(id) && FSM_MODE != TWOL_MODE)
	    {
	      sigma_add_to(NET_sigma(net), upper_id(id));
	      sigma_add_to(NET_sigma(net), lower_id(id));
	      sigma_add_to(NET_labels(net), id);
	    }
	  else
	    {
	      sigma_add_to(NET_sigma(net), id);
	      sigma_add_to(NET_labels(net), id);
	    }
	}
	 	
      binary_to_label(NET_labels(net));
    }

  if (restore_p)
    label_to_binary(alph);
	  
  if (min_p)
    min_fsm(net, DONT_COPY);
		
  free_alph(aux);
  free_alph(bin_aux);
  free_alph(aux_record);
  return(net);
}

NETptr ignore_labels(NETptr net, ALPHABETptr labels, int copy_p,
                     int minimize_p)
{
  /* Returns a network which is like net except that every state
     of the new fsm contains a looping arc back to itself for every
     label in LABELS. Equivalent to but faster than IGNORE_FSM for
     simple cases. Assumes that LABELS is in label format, does
     not modify or reclaim LABELS. Now checks for the presence
     of OTHER in LABELS. If LABELS contains OTHER, we augment LABELS
     with symbols from the NET's sigma--in effect doing on the
     LABELS alphabet the same work as EXPAND_OTHER would do on
     a network. This case was not handled properly in previous
     versions.  Indirectly fixes similar problem with IGNORE_LABEL.
     Nov-4-96 LK.

     NB. Still not correct if LABELS contains an fstpair with OTHER
     as a component. Fix this later!.  Feb-14-97 LK.
     
     Now correct. 1998-11-16. LK
     */
  
	return(ignore_alph(net, labels, copy_p, minimize_p, EVERYWHERE,
					  BOTH_SIDES, DO_EXPAND));
}

NETptr ignore_label(NETptr net, id_type id, int copy_p, int minimize_p)
{
  /* Returns a network which is like NET except that every state
     of the new fsm contains an arc with the label ID back to itself.
     Equivalent to but faster than IGNORE_FSM for simple cases.
     This function was previously incorrect in not checking whether
     ID is OTHER. Makes use of a IGNORE_LABELS, which previously
     also did not properly handle cases involving OTHER. L.K. Nov-4-96.

     NB. Still not correct if LABELS contains an fstpair with OTHER
     as a component. Fix this later!.  Feb-14-97 LK.
     
     Now correct.
     */

  NETptr result;  
  ALPHABETptr alph;

  /* If ID is a symbol pair with OTHER as a component, we expand it
     here. ?:X yields ?:X and X (unless X is EPSILON). ?:? yields ?:? and ?. 
     */

  if (tuple_id_p(id)  &&
      ((upper_id(id) == OTHER && lower_id(id) != EPSILON) ||
       (lower_id(id) == OTHER && upper_id(id) != EPSILON)))
    {
      alph = make_alph(2, LABEL_VECTOR);

      if (upper_id(id) == OTHER)
	ALPH_item(alph,1) = lower_id(id);
      else
	ALPH_item(alph,1) = upper_id(id);
    }
  else
    alph = make_alph(1, LABEL_VECTOR);
	 
  ALPH_item(alph, 0) = id;
	 
  result = ignore_alph(net, alph, copy_p, minimize_p, EVERYWHERE,
		       BOTH_SIDES, DO_EXPAND);
  free_alph(alph);
  return(result);
}


/************************
 *
 *  concatenation
 *
 ************************/

void extend_net_with_label(NETptr net, id_type id, int optional_p)
{
  /* Extends all paths in NET with an ID arc unless id is EPSILON
     or the net is a null fsm.

     If id is OTHER, also add arcs for its expansion with respect
     to the net's sigma. Similarly when OTHER appears on one side
     of an fstpair. In that case, we also need to add an identity
     arc for the symbol that opposes it.
     */

  int contains_other_p = FALSE;
  id_type other_id, new_id;

  if (id != EPSILON && !null_fsm_p(net))
    {
      STATEptr state = NET_states(net), final_state = make_state(NULL, net);
      int i, j;
      ALPHABETptr sig = sigma(net);

      /* A new final state is added to the net. */

      STATE_final(final_state) = TRUE;

      /* If id is OTHER we temporarily convert the sigma to label
	 format for the loop below.
	 */

      if (id == OTHER || (tuple_id_p(id) &&
			  (id_to_upper_id(id) == OTHER ||
			   id_to_lower_id(id) == OTHER)))
	{
	  contains_other_p = TRUE;
	  binary_to_label(sig);
	}

      do {
	if (STATE_final(state))
	  {
	    /* Make every existing final state non-final, unless
               the extension is optional, and add an ID arc to the
               new final state. 
	       */

            if (!optional_p)
	      STATE_final(state) = FALSE;
	    add_arc(make_arc(id, final_state, net), state, net);

	    /* If ID is OTHER, also add arcs for all the symbols
	       in the net's sigma, except for OTHER, of course.
               We also exempt the BOUNDARY_SYMBOL. It is *never*
               in the domain of OTHER. If ID is an fstpair containing
	       OTHER, we need to make fstpair arcs instead.
	       */

	    if (id == OTHER)
	      {
		for (i = 0; i < ALPH_len(sig); i++)
		  if (ALPH_item(sig, i) != OTHER &&
		      ALPH_item(sig, i) != BOUNDARY_SYMBOL)
		    {
		      add_arc(make_arc(ALPH_item(sig, i), final_state, net),
			      state, net);
		      sigma_add_to(NET_labels(net), ALPH_item(sig, i));
		    }
	      }
	    else if (contains_other_p)
	      {
		/* One-sided OTHER. */
		if (id_to_upper_id(id) == OTHER)
		  {
		    /* Substitute all the symbols in the sigma for
		       the upper-side OTHER and for the lower-side
                       OTHER if the label is OTHER:OTHER. */

		    other_id = id_to_lower_id(id);

		    for (i = 0; i < ALPH_len(sig); i++)
		      if (ALPH_item(sig, i) != OTHER &&
			  ALPH_item(sig, i) != BOUNDARY_SYMBOL)
			{
			  make_fstlabel(&new_id, ALPH_item(sig, i), other_id);
			  add_arc(make_arc(new_id, final_state, net),
				  state, net);
			  sigma_add_to(NET_labels(net), new_id);

                          if (id_to_lower_id(id) == OTHER)
                          {
                              make_fstlabel(&new_id, OTHER, ALPH_item(sig,i));
                              add_arc(make_arc(new_id, final_state, net),
                                      state, net);
                              sigma_add_to(NET_labels(net), new_id);

                              /* In the case of OTHER:OTHER there is a
                                 lot of work to do!
                                 */

                              for (j = 0; j < ALPH_len(sig); j++)
                              {
                                  make_fstlabel(&new_id, ALPH_item(sig, i),
                                                         ALPH_item(sig, j));
                                  add_arc(make_arc(new_id, final_state, net),
                                          state, net);
                                  sigma_add_to(NET_labels(net), new_id);

                                  make_fstlabel(&new_id, ALPH_item(sig, j),
                                                         ALPH_item(sig, i));
                                  add_arc(make_arc(new_id, final_state, net),
                                          state, net);
                                  sigma_add_to(NET_labels(net), new_id);
                              }
                          }
			}
		  }
		else
		  {
		    /* Substitute all the symbols in the sigma for
		       the lower-side OTHER */

		    other_id = id_to_upper_id(id);

		    for (i = 0; i < ALPH_len(sig); i++)
		      if (ALPH_item(sig, i) != OTHER &&
			  ALPH_item(sig, i) != BOUNDARY_SYMBOL)
			{
			  make_fstlabel(&new_id, other_id, ALPH_item(sig, i));
			  add_arc(make_arc(new_id, final_state, net),
				  state, net);
			  sigma_add_to(NET_labels(net), new_id);
			}
		  }

		/* Also add an arc substituting for OTHER
		   the symbol that is facing it, if it is not EPSILON.
		   */

		if (other_id != EPSILON)
		  {
		    add_arc(make_arc(other_id, final_state, net), state, net);
		    sigma_add_to(NET_labels(net), other_id);
		  }
	      }
	  }
      } while ((state = next_state(state)));
      
      /* Add ID to the label alphabet of the new net, and, if ID is
	 OTHER, also add the contents of the sigma. (The first call
	 converts the alphabet to BINARY_VECTOR format, so we need
	 to convert it back to a LABEL_VECTOR when done.
	 */

      sigma_add_to(NET_labels(net), id);

      if (id == OTHER || contains_other_p)
	{
	  /* Convert sigma back to binary format. */
	  label_to_binary(sig);
	}

      label_to_sigma(sig, id);
      
      /* Convert labels back to LABEL_VECTOR format. */

      binary_to_label(NET_labels(net));
    }
}


NETptr concat_label(NETptr net, id_type id, int copy_p, int minimize_p)
{
  /* Extends NET with an ID arc, unless ID is the epsilon symbol.
     or the net is a null fsm. */
  
  if (copy_p == DO_COPY)
    net = copy_fsm(net);
  
  if (id != EPSILON && !null_fsm_p(net))
    {
      if (sigma_member(sigma(net), OTHER))
        {
	  /* Check if OTHER needs to be expanded. */

	  ALPHABETptr sig = sigma(net);
	  ALPHABETtype alph;
	  id_type items[3];
	  LABELptr label = id_to_label(id);
          
          alph.type = LABEL_VECTOR;
          alph.len = 0;
          alph.items = items;
          items[alph.len++] = OTHER;
          
	  /* N.B. At this point ALPH.LEN is 1. */

          if (LABEL_arity(label) == 1 || FSM_MODE == TWOL_MODE)
	    {
	      /* Add ID to ALPH.ITEMS if ID is not yet in the sigma.
                 and is something other than the BOUNDARY_SYMBOL,
                 which is never in the domain of OTHER.
                 */

	      if (!(id == BOUNDARY_SYMBOL || sigma_member(sig, id)))
		items[alph.len++] = id;
	    }
          else
            {
	      /* New label is an fstpair. Add its components to ALPH.ITEMS
		 unless they are EPSILONs or already belong to sigma.
		 */

	      id_type upper = fstpair_upper(label), lower = fstpair_lower(label);
                              
	      if (upper == lower && upper != OTHER)
		handle_error("X:X", "CONCAT_LABEL", upper);

	      if (!(upper == EPSILON || sigma_member(sig, upper)))
		items[alph.len++] = upper;

	      if (!(lower == EPSILON || sigma_member(sig, lower)))
		items[alph.len++] = lower;
            }

	  /* If something was added to ALPH.ITEMS, we expand OTHER. */

	  if (alph.len > 1)
	    symsubst(OTHER, &alph, net, FALSE, FALSE);
        }

      extend_net_with_label(net, id, FALSE);
    }

  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);
  return(net);
}


static NVptr concat_prep(NVptr nets, int copy_p,
                         ALPHABETptr *comm_labels, ALPHABETptr *comm_sigma)
{
  
  /* Preparatory steps for CONCAT_FSM. Makes copies if necessary,
     and computes the common sigma and the common label sets.
     Returns NULL if one of the nets is a null fsm, otherwise a net vector.
     Returns a new vector if COPY_P is DO_COPY or if the same net appears
     more than once in NETS.
     */
  
  NVptr return_nets;
  NETptr cur;
  int i, new_nv_p = copy_p;
  
  /* If any of the arguments is a null fsm, we can stop right here */
  
  for (i = 0; i < NV_len(nets); i++)
    if (null_fsm_p(NV_net(nets, i)))
      return(NULL);
    else
      NET_client_cell(NV_net(nets, i)) = NULL;
  
  /* Check if we need to copy some nets */
  
  if (copy_p == DONT_COPY)
    {
      for (i = 0; i < NV_len(nets); i++)
        {
          cur = NV_net(nets, i);
          if ((int) NET_client_cell(cur) > 0)
            new_nv_p = DO_COPY;    
          NET_client_cell(cur) = (void *) ((int) NET_client_cell(cur) + 1);
        }
    }
  
  /* Reconstitute list making copies, if necessary, of the original
     networks.  Compose the joint sigma and label sets.
     */
  
  if (new_nv_p == DO_COPY)
    return_nets = make_nv(NV_len(nets));
  else 
    {
      return_nets = nets;
      if (copy_p == DONT_COPY)
        /* Clear NET_client_cell on the net that will be the result. */
        NET_client_cell(NV_net(nets,0)) = NULL;
    }
  
  for (i = 0; i < NV_len(nets); i++)
    {
      cur = NV_net(nets, i);
      
      if (new_nv_p == DO_COPY)
	{
	  if (copy_p == DO_COPY)
	    NV_net(return_nets, i) = copy_fsm(cur);
	  else
	    {
	      /* Make a copy if CLIENT_CELL > 1, then decrement. */
	      if ((int) NET_client_cell(cur) > 1)
		NV_net(return_nets, i) = copy_fsm(cur);
	      else
		NV_net(return_nets, i) = cur;   
	      NET_client_cell(cur) = (void *) ((int) NET_client_cell(cur) - 1);
	    }
	}
      
      if (i == 0) /* first time only */
        {
          *comm_labels = binary_to_label(copy_alphabet(NET_labels(cur)));
          *comm_sigma = copy_alphabet(sigma(cur));              
        }
      else
        {
          label_augment(*comm_labels, NET_labels(cur));
          sigma_augment(*comm_sigma, sigma(cur));
        }
    }
  
  return(return_nets);
}

NETptr concat_fsm(NVptr nets, int copy_p, int minimize_p)
{
  /*  CONCAT_FSM concatenates an arbitrary number of networks. Smashes 
      them together without copying if  DONT-COPY-P. In this  case, the
      result is EQ to the first  network argument, and all the other
      arguments are reclaimed.  Returns concatenation identity, EPSILON_FSM,
      if no args are given, NULL_FSM if any of the arguments is a null fsm.
      */
  NVptr prep_nets;
  NETptr net, cur;
  ALPHABETptr common_sigma, common_labels;
  STATEptr state, last_state, start_state;
  ARCptr arc;
  
  int i, loop_state_p;
  
  if (nets == NULL || NV_len(nets) == 0)
    return(epsilon_fsm());
  
  if (NV_len(nets) == 1)
    {
      if (copy_p == DO_COPY)
	return(copy_fsm(NV_net(nets, 0)));
      else
	return(NV_net(nets, 0));
    }

  /* Check nets for optimization: */
  for (i = 0; i < NV_len(nets); i++)
  {
    if (NET_optimized(NV_net(nets, i)) || possibly_already_optimized_p(NV_net(nets, i)))
    {
        handle_warning("Can't concatenate optimized networks.\n",
		     "concat_fsm", 0);
        fsm_fprintf(stdout, "Un-optimizing ...\n");

        do_unoptimize_arcs(NV_net(nets, i));
    }
  }
  
  prep_nets = concat_prep(nets, copy_p, &common_labels, &common_sigma);
  
  if (prep_nets == NULL)
    {
      /* One of the nets is a NULL fsm.  Unless the original nets are
         to be preserved, find the first null fsm, return it as value
         and free the rest.
         */
      net = NULL;
      if (copy_p == DONT_COPY)
        for (i = 0; i < NV_len(nets); i++)
          {
            cur = NV_net(nets, i);
            if (net == NULL && null_fsm_p(cur))
              net = cur;
            else
              free_network(cur);
          }
      else
        net = null_fsm();
      return(net);
    }

  /* Now go to work, move state and arcs of CUR to NET,
     and free what remains. If the net was not copied,
     free the parse_tables and flag_register, if any,
     of the first net because they will not be valid
     for the result. */
  
  net = expand_other(NV_net(prep_nets, 0), common_sigma, DONT_COPY);
  label_augment(common_labels, NET_labels(net));

  if (copy_p == DONT_COPY)
    reclaim_cashed_label_data(net);
  
  for (last_state = NET_states(net);
       next_state(last_state) != NULL;
       last_state = next_state(last_state));
  
  for (i = 1; i < NV_len(prep_nets); i++)
    {
      cur = NV_net(prep_nets, i);
      
      loop_state_p = start_loops_p(cur);
      
      expand_other(cur, common_sigma, DONT_COPY);
      label_augment(common_labels, NET_labels(cur));
      
      
      start_state = NET_start_state(cur);
      
      for (state = NET_states(net); state != NULL; state = next_state(state))
        if (STATE_final(state))
          {
            STATE_final(state) = STATE_final(start_state);
            
            for (arc = STATE_arc_set(start_state); arc != NULL;
                 arc = next_arc(arc))
              add_arc(copy_arc(arc, net), state, net);
          }
      
      next_state(last_state) = NET_states(cur);
      
      /* Link the state set of the NET to the state set of CUR
         eliminating the original start state of CUR if it is not
         a loop state, that is, if there are no arcs in CUR pointing
         to it.  Move the last_state pointer to the end of the new
         states aquired from CUR.  If the original start state of CUR
         is kept, the result may need minimization even if NET and
         CUR are minimal and deterministic.      
         */
      
      for (state = next_state(last_state);
           state != NULL;
           state = next_state(last_state))
        if (state == start_state && !loop_state_p)
          {
            /* Splice out the original start state of CUR */
            next_state(last_state) = next_state(state);
            free_state_and_arcs(state, cur);
          }
        else
          last_state = state;
      
      /* The following properties are preserved in the result only
         if all the component nets have them.
         */
      if (NET_pruned(net))
        NET_pruned(net) = NET_pruned(cur);
      if (NET_completed(net))
        NET_completed(net) = NET_completed(cur);
      if (NET_loop_free(net))
        NET_loop_free(net) = NET_loop_free(cur);
      if (NET_epsilon_free(net))
        NET_epsilon_free(net) = NET_epsilon_free(cur);
      if (NET_deterministic(net))
        NET_deterministic(net) = NET_deterministic(cur);
      if (NET_minimized(net))
        NET_minimized(net) = 
          (!loop_state_p && NET_deterministic(net) && NET_minimized(cur));
      
      /* The following properties are acquired by the result if any
         component network has the property.
         */
      
      if (NET_visit_marks_dirty(cur))
        NET_visit_marks_dirty(net) = TRUE;
      if (NET_has_arc_user_pointer(cur))
        NET_has_arc_user_pointer(net) = TRUE;
      
      /* The highest arity wins */
      
      if (NET_arc_label_arity(cur) > NET_arc_label_arity(net))
        NET_arc_label_arity(net) = NET_arc_label_arity(cur);
      
      /* Adjust state and arc counts. */  
      
      NET_num_states(net) += NET_num_states(cur);
      NET_num_arcs(net) += NET_num_arcs(cur);
      
      sever_states(cur);
      free_network(cur);
    }
  
  free_alph(NET_labels(net));
  NET_labels(net) = binary_to_label(common_labels);
  free_alph(NET_sigma(net));
  NET_sigma(net) = common_sigma;
  NET_properties(net) = NULL;
  
  if (nets != prep_nets)
    free_nv_only(prep_nets);
  
  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);
  
  return(net); 
}



/*************
 *           *
 *   UNION   *
 *           *
 *************/

static NETptr union_fsm_aux(NETptr n1, NETptr n2, int copy_p)
     /*****
       UNION_FSM_AUX() smashes N1 so that it recognizes
       the union of N1 and N2's languages.
       It also smashes N2 if COPY_P is DONT_COPY 
       *****/
{
    ALPHABETptr n1_sigma, n2_sigma;
    STATEptr n1_start_state, n2_start_state;
    n1_sigma = sigma(n1);
    n2_sigma = sigma(n2);

    /* Check n1 and n2 for optimization: */
    if (NET_optimized(n1) || possibly_already_optimized_p(n1))
    {
        handle_warning("Can't compute the union with optimized networks.\n",
		     "union_fsm_aux", 0);
        fsm_fprintf(stdout, "Un-optimizing ...\n");

        do_unoptimize_arcs(n1);
    }

    if (NET_optimized(n2) || possibly_already_optimized_p(n2))
    {
        handle_warning("Can't compute the union with an optimized network.\n",
		     "union_fsm_aux", 0);
        fsm_fprintf(stdout, "Un-optimizing ...\n");

        do_unoptimize_arcs(n2);

        fsm_fprintf(stdout, "Unioning ...\n");      
    }
  
    n1 = expand_other(n1, n2_sigma, DONT_COPY);
    n2 = expand_other(n2, n1_sigma, copy_p);
  
    /* If COPY_P is true, n2 is now a copy of the original. */
    /* Protect the original property list of n2 */
    if (copy_p == TRUE)
       NET_properties(n2) = NULL;
  
    n1_start_state = NET_start_state(n1);
    n2_start_state = NET_start_state(n2);
  
    if (null_fsm_p(n1))
        /* n1 is NULL, return n2 (or a copy depending on copy_p) */
    {
        free_network(n1);
        return (n2);
    }
    else if (null_fsm_p(n2))
        /* n2 is NULL, return n1 */
    {
        /* Free the copy make above. */

        free_network(n2);
        return (n1);
    }
    else
    {
        STATEptr start_state, s1_final, s2_final, prev, stail, s;
        int n1_start_final = STATE_final(n1_start_state),
            n2_start_final = STATE_final(n2_start_state),
            n1_start_deterministic = STATE_deterministic(n1_start_state),
            n2_start_deterministic = STATE_deterministic(n2_start_state);
        ARCptr arc, new_arc, a, temp, start_state_arcs = NULL;
          
        /*****
          ;;; Original start states are not accessible unless they
          ;;; loop.  We can eliminate N1_START_STATE here but we still
          ;;; have to keep N2_START_STATE in N2_STATES.
          ;;; NB. We assume now that the LOOP_FREE flag is correctly
          ;;; set.
          *****/
          
        if ((!NET_loop_free(n1)) && start_loops_p(n1))
            for (arc = STATE_arc_set(n1_start_state);
                 arc != NULL;
                 arc = next_arc(arc))
            {
                new_arc = copy_arc(arc, n1);
                next_arc(new_arc) = start_state_arcs;
                start_state_arcs = new_arc;
            }
        else
        {
            start_state_arcs = STATE_arc_set(n1_start_state);
            STATE_arc_set(n1_start_state) = NULL;
            NET_states(n1) = delete_state_from_list(n1_start_state,
                                                    NET_states(n1));
            NET_num_states(n1)--;
        }
          
        if (NET_minimized(n1))
        {
            if (NET_minimized(n2) && start_state_arcs != NULL &&
                STATE_arc_set(n2_start_state) != NULL &&
                sigma_disjoint_p(NET_labels(n1), NET_labels(n2)))
            {
                /*****
                  ;; Try to preserve minimality in a very special case.
                  ;; Only equivalent states can be empty finals.
                  ;; Since arcsets exist, we know neither N1 or N2 is null
                  ;; or epsilon machine, and since minimized, each
                  ;; contains at most one state with no arcs.
                  *****/
                  
                s2_final = NULL;

                for (s1_final = NET_states(n1);
                     s1_final != NULL;
                     s1_final = next_state(s1_final))
                {
                    if (STATE_arc_set(s1_final) == NULL)
                    {
                        /*****
                          ;; S1_FINAL is a unique empty final state, it
                          ;; displaces unique empty final state in N2, if
                          ;; such a state exists.  If S2_FINAL is found,
                          ;; we remove it from the arcset of N2 and
                          ;; replace it by S1_FINAL in all the arcs of
                          ;; N2--including the initial state
                          ;; N2_START_STATE that is about to be removed
                          ;; from N2_STATES. 
                          *****/
                          
                        for (prev = NULL, stail = NET_states(n2);
                             stail != NULL;
                             prev = stail, stail = next_state(stail))
                            if (STATE_arc_set(stail) == NULL)
                            {
                                if (prev == NULL)
                                    NET_states(n2) =
                                        next_state(NET_states(n2));
                                else
                                    next_state(prev) = next_state(stail);
                                
                                s2_final = stail;
                                NET_num_states(n2)--;
                                free_state(s2_final);
                                break;
                            }
                          
                        if (s2_final != NULL)
                            for (s = NET_states(n2);
                                 s != NULL;
                                 s = next_state(s))
                                for (a = STATE_arc_set(s);
                                     a != NULL;
                                     a = next_arc(a))
                                    if (s2_final == ARC_destination(a))
                                        ARC_destination(a) = s1_final;
                        break;
                    }
                }
                if (s2_final == NULL)
		  NET_minimized(n1) = FALSE;
            }
            else
                /* result needs minimization */
                NET_minimized(n1) = FALSE;
        }
          
        /*****
          ;; Remove N2-START-STATE after final-state sharing so that
          ;; the arcs leading from it to the final state get updated.
          ;;; NB. We assume now that the LOOP_FREE flag is correctly
          ;;; set.
          *****/
        if ((!NET_loop_free(n2)) && start_loops_p(n2))
/*            if (start_loops_p(n2)) */
            for (arc = STATE_arc_set(n2_start_state);
                 arc != NULL;
                 arc = next_arc(arc))
            {
                new_arc = copy_arc(arc, n1);
                /* must copy because n2_start_state is kept */
                next_arc(new_arc) = start_state_arcs;
                start_state_arcs = new_arc;
            }
        else
        {
            for (temp = arc = STATE_arc_set(n2_start_state);
                 temp != NULL; arc = temp)
            {
                /* don't need copy because n2_start_state is not kept */
                /* don't need to adjust arc count because the arcs
                   are moved to n1 */
                temp = next_arc(arc);
                next_arc(arc) = start_state_arcs;
                start_state_arcs = arc;
            }
            /* Set arc set to NULL before freeing state because the
               arcs are kept.
               */
              
            STATE_arc_set(n2_start_state) = NULL;
            NET_states(n2) = delete_state_from_list(n2_start_state,
                                                    NET_states(n2));
            NET_num_states(n2)--;
        }
          
        start_state = make_state(NULL, n1);
        NET_start_state(n1) = start_state;
        STATE_arc_set(start_state) = start_state_arcs;
        STATE_final(start_state) = n1_start_final || n2_start_final;
          
        /* Bug in MAC_OS.  Direct assignment, that is
           STATE_deterministic(start_state) = ... && ... && ...;
           doesn't work here for some reason.
           */
          
        if (n1_start_deterministic && n2_start_deterministic &&
            deterministic_arc_set_p(start_state_arcs))
            STATE_deterministic(start_state) = TRUE;
        else
            STATE_deterministic(start_state) = FALSE;
          
        /* Append the longer state list at the end of the
           shorter one. N1 must have states but N2 might
           have none left at this point. In that case,
           the N1's state list need not be modified.
           */

        if (NET_num_states(n1) <= NET_num_states(n2))
        {
            for (s = NET_states(n1);
                 next_state(s) != NULL;
                 s = next_state(s));
            next_state(s) = NET_states(n2);
        }
        else if (NET_num_states(n2) > 0)
        {
            for (s = NET_states(n2);
                 next_state(s) != NULL;
                 s = next_state(s));
            next_state(s) = NET_states(n1);
            NET_states(n1) = NET_states(n2);
        }
        else
        {
            /* State count is wrong if the state list is empty. */
            if (NET_states(n2))
                handle_warning("State count is 0 but statelist not empty",
                               "UNION_FSM_AUX", 0);
        }

        if (NET_completed(n1) == TRUE)
            NET_completed(n1) = NET_completed(n2);
          
        if (NET_loop_free(n1) == TRUE)
            NET_loop_free(n1) = NET_loop_free(n2);
          
        if (NET_has_arc_user_pointer(n1) == FALSE)
            NET_has_arc_user_pointer(n1) = NET_has_arc_user_pointer(n2);
          
        if (NET_visit_marks_dirty(n2) == TRUE)
            NET_visit_marks_dirty(n1) = TRUE;
          
        if (NET_pruned(n1))
            NET_pruned(n1) = NET_pruned(n2);
          
        if (NET_epsilon_free(n1))
            NET_epsilon_free(n1) = NET_epsilon_free(n2);
          
        if (NET_deterministic(n1))
            if (!(NET_deterministic(n2) &&
                  STATE_deterministic(start_state)))
            {
                NET_deterministic(n1) = FALSE;
                NET_minimized(n1) = FALSE;
            }
          
        NET_sigma(n1) = sigma_union_dont_copy(n1_sigma, n2_sigma);
        NET_labels(n1) =
            binary_to_label(sigma_union_dont_copy(NET_labels(n1),
                                                  NET_labels(n2)));
        NET_num_states(n1) += NET_num_states(n2);
        NET_num_arcs(n1) += NET_num_arcs(n2);
          
        if (NET_arc_label_arity(n2) > NET_arc_label_arity(n1))
            NET_arc_label_arity(n1) = NET_arc_label_arity(n2);

        sever_states(n2);
        free_network(n2);
        return (n1);
        
    }
}

NETptr union_fsm (NVptr net_list, int copy_p, int minimize_p)
     /******
      *
      * TOP LEVEL FUNCTION FOR UNION
      *
      * UNION_FSM unions an arbitrary number of networks.
      * Returns identity NULLFSM if no arguments
      * are given.
      *
      * Destructive to all nets in net_list unless COPY_P is DO_COPY.
      * Minimizes result unless MINIMIZE_P is DONT_MINIMIZE.
      *
      ******/
{
  NVptr min_list;
  NETptr result;
  int i;
  
  if (net_list == NULL || NV_len(net_list) == 0)
    return (null_fsm());
  
  min_list = remove_duplicates(net_list);
  
  if (copy_p == DO_COPY)
    result = copy_fsm(NV_net(min_list, 0));
  else
    {
      result = NV_net(min_list, 0);

      /* Reclaim the parse tables (if any) and set the
         pointers to NULL because the old tables
         are not valid if new symbols get added to
         the alphabet.

	 Also, reclaim flag_register for the same reason.
         */
      reclaim_cashed_label_data(result);
    }
  
   NET_properties(result) = NULL;
  
  if (minimize_p != DONT_MINIMIZE)
      min_fsm(result, DONT_COPY);

  for (i = 1; i < NV_len(min_list); i++)
    {
      result = union_fsm_aux(result, NV_net(min_list, i), copy_p);
      if (minimize_p != DONT_MINIMIZE)
        min_fsm(result, DONT_COPY);
    }
  
  /* NET_LIST was truncated because it contained duplicates. */
  
  if (net_list != min_list)
    free_nv_only(min_list);
  
  check_for_flags(result);

  return(result);
} /* union_fsm */

/****************************
 *
 *  Expanding OTHER in labels
 *
 ***************************/

static CONSptr expand_upper_other (id_type lower_id, id_type except_id,
				   ALPHABETptr sigma,
                                   CONSptr labels, int non_identity_p,
				   HEAPptr cons_heap)
{
  /* Returns LABELS augmented with fstpairs resulting from the
     expansion of the first coordinate in SIGMA.  UPPER_OTHER
     must be of type LABEL_VECTOR.
     */
  int i;
  id_type id, pair_id;

  for (i = 0; i < ALPH_len(sigma); i++)
    {
      id = ALPH_item(sigma, i);

      if (!((lower_id == id && non_identity_p) ||
	    id == except_id))
	{
	  make_fstlabel(&pair_id, id, lower_id);
	  labels = cons_cell(cast_to_void(pair_id),
			   labels, cons_heap);
	}
    }
  
  return(labels);
}

static CONSptr expand_lower_other (id_type upper_id, id_type except_id,
				   ALPHABETptr sigma,
                                   CONSptr labels, int non_identity_p,
				   HEAPptr cons_heap)
{
  /* Returns LABELS augmented with fstpairs resulting from the
     expansion of the second coordinate in SIGMA.  UPPER_OTHER
     must be of type LABEL_VECTOR.
     */
  int i;
  id_type id, pair_id;

  for (i = 0; i < ALPH_len(sigma); i++)
    {
      id = ALPH_item(sigma, i);

      if (!((upper_id == id && non_identity_p) ||
	    id == except_id))
	{
	  make_fstlabel(&pair_id, upper_id, id);
	  labels = cons_cell(cast_to_void(pair_id),
			   labels, cons_heap);
	}
    }
  return(labels);
}


static CONSptr expand_pair_other(id_type upper_low, id_type lower_hi,
				 ALPHABETptr upper_other,
                                 ALPHABETptr lower_other, CONSptr labels,
				 HEAPptr cons_heap)
{
  /* Returns LABELS augmented with new pairs resulting from the
     expansion of matching OTHERs in their respective alphabets.
     */
  id_type upper_id, lower_id, pair_id;
  int i, j;
  
  for (i = 0; i < ALPH_len(upper_other); i++)
    {
      upper_id = ALPH_item(upper_other, i);

      /* In composing OTHER:OTHER with a:OTHER we must not
	 give rise to a:OTHER. */

      if (upper_id != lower_hi)
	{
	  make_fstlabel(&pair_id, upper_id, OTHER);
	  labels = cons_cell(cast_to_void(pair_id),
			     labels, cons_heap);
	}
      for (j = 0; j < ALPH_len(lower_other); j++)
        {
	  lower_id = ALPH_item(lower_other, j);
	  
	  make_fstlabel(&pair_id, upper_id, lower_id);
	  labels = cons_cell(cast_to_void(pair_id),
			     labels, cons_heap);
        }
    }

  for (i = 0; i < ALPH_len(lower_other); i++)
    {
      /* In composing OTHER:a with OTHER:OTHER we must
	 not give rise to OTHER:a */

      lower_id = ALPH_item(lower_other, i);

      if (lower_id != upper_low)
	{
	  make_fstlabel(&pair_id, OTHER, lower_id);
	  labels = cons_cell(cast_to_void(pair_id),
			     labels, cons_heap);
	}
    }
  
  return(labels);
}

static CONSptr add_fstpair(id_type pair_id, CONSptr labels, HEAPptr cons_heap)
{
  return (cons_cell(cast_to_void(pair_id), labels, cons_heap));

}

CONSptr expand_fstpair (id_type upper, id_type lower,
                               ALPHABETptr upper_other, 
			       ALPHABETptr lower_other,
			       int non_identity_p,
			       HEAPptr cons_heap)
{
  /* Returns a list of labels consisting of UPPER_ID:LOWER_ID and the
     labels resulting from the expansion of OTHER on each side. If
     UPPER_ID and LOWER_ID are both OTHER, we add the atomic OTHER
     to the result to represent any unknown symbol mapped to itself.
     */
  
  CONSptr labels;
  id_type id;
  id_type except_id = ID_NO_SYMBOL;
  
  id_type upper_hi = upper_id(upper), upper_lo = lower_id(upper);
  id_type lower_lo = lower_id(lower), lower_hi = upper_id(lower);

  make_fstlabel(&id, upper_hi, lower_lo);
  labels = add_fstpair(id, NULL, cons_heap);  
		          
  if (upper_hi == OTHER && lower_lo == OTHER)
    {
      labels = add_fstpair(OTHER, labels, cons_heap);

      labels = expand_pair_other(upper_lo, lower_hi, upper_other, lower_other,
				 labels, cons_heap);
    }
  else if (upper_hi == OTHER)
    {
      /* In composing OTHER:OTHER with a:b we must not give rise
	 to a:b */

      if (upper_lo == OTHER && upper != OTHER)
	except_id = lower_hi;

      labels = expand_upper_other(lower_lo, except_id, upper_other, labels,
				  non_identity_p, cons_heap);
    }
  else if (lower_lo == OTHER)
    {
      /* In composing a:b with OTHER:OTHER we must not give rise
	 to a:b */

      if (lower_hi == OTHER && lower != OTHER)
	except_id = upper_lo;

      labels = expand_lower_other(upper_hi, except_id, lower_other, labels,
				  non_identity_p, cons_heap);
    }

  return(labels);
}

static CONSptr expand_upper_side (id_type upper_id, id_type lower_id,
				  ALPHABETptr upper_other,
				  int non_identity_p,
				  HEAPptr cons_heap)
{
  /* Returns a list of labels consisting of UPPER_ID:LOWER_ID and the
     labels resulting from the expansion of OTHER on the upper side. If
     UPPER_ID and LOWER_ID are both OTHER, we add the atomic OTHER
     unless NON-IDENTITY_P flag is set to true. This flag is set
     in composing OTHER:OTHER with OTHER and OTHER with OTHER:OTHER.
     In these cases non-identity must be preserved, that is, 
     we generate only OTHER:OTHER and not OTHER. However, composing
     OTHER:OTHER with OTHER:OTHER, we generate both OTHER:OTHER and OTHER.
     */
  		          
  CONSptr labels;
  id_type id;

  make_fstlabel(&id, upper_id, lower_id);
  labels = add_fstpair(id, NULL, cons_heap);
	
  if (upper_id == OTHER)
    {

      if (lower_id == OTHER && !non_identity_p)
	labels = add_fstpair(OTHER, labels, cons_heap);

      if (upper_other)
	labels = expand_upper_other(lower_id, ID_NO_SYMBOL, upper_other,
				    labels, non_identity_p, cons_heap);
    }

  return(labels);
}

static CONSptr expand_lower_side (id_type upper_id, id_type lower_id,
				  ALPHABETptr lower_other,
				  int non_identity_p,
				  HEAPptr cons_heap)
{
  /* Returns a list of labels consisting of UPPER_ID:LOWER_ID and the
     labels resulting from the expansion of OTHER on the lower side. If
     UPPER_ID and LOWER_ID are both OTHER, we add the atomic OTHER
     unless NON-IDENTITY_P flag is set to true. This flag is set
     in composing OTHER:OTHER with OTHER and OTHER with OTHER:OTHER.
     In these cases non-identity must be preserved, that is, 
     we generate only OTHER:OTHER and not OTHER. However, composing
     OTHER:OTHER with OTHER:OTHER, we generate both OTHER:OTHER and OTHER.
     */

  CONSptr labels;
  id_type id;

  make_fstlabel(&id, upper_id, lower_id);
  labels = add_fstpair(id, NULL, cons_heap);
	
  if (lower_id == OTHER)
    {
      if (upper_id == OTHER && !non_identity_p)
	labels = add_fstpair(OTHER, labels, cons_heap);

      if (lower_other)
	labels = expand_lower_other(upper_id, ID_NO_SYMBOL, lower_other,
				    labels, non_identity_p, cons_heap);
    }

  return(labels);
}

/*****************************************
 *                                       *
 *  INTERSECTION                         *
 *   Still used
 *                                       *
 *****************************************/

/* Don't use the one declared above for DETERMINIZE because intersection
   calls MIN-FSM, which in turn calls the determinizer */

typedef struct DEST_CELL {
  fst_byte type;     /* 0 = empty, 1 = single state, 2 = list of states */
  fst_byte mark; /* 0 = unmarked, 1 = visited */
  union {
    STATEptr state;
    CONSptr cons;
  } content;
} DESTtype, *DESTptr;
 
#define DEST_type(X) (X)->type
#define DEST_mark(X) (X)->mark
#define DEST_state(X) (X)->content.state
#define DEST_cons(X) (X)->content.cons
  
static id_type pair_intersect_preprocess(NETptr n1, NETptr n2)
     /*****
      *
      * Compute maximum label among n1 & n2.
      *
      *****/
{
  id_type max1, max2;
  ALPHABETptr labels1 = NET_labels(n1), labels2 = NET_labels(n2);
  
  binary_to_label(labels1);
  binary_to_label(labels2);
  
  max1 = max_label_item(ALPH_items(labels1), ALPH_len(labels1));
  max2 = max_label_item(ALPH_items(labels2), ALPH_len(labels2));
  
  if (max2 > max1)
    return(max2);
  else
    return(max1);
}

#ifdef FULL_FSM
static void add_new_arcs(NETptr net, CONSptr labels,
                         STATEptr state, STATEptr dest, void *arc_user)
#else
     static void add_new_arcs(NETptr net, CONSptr labels,
                              STATEptr state, STATEptr dest)
#endif
{
  /* Adds arcs for all the ids in the LABELS list */
  
  for (; labels != NULL; labels = cdr(labels))
#ifdef FULL_FSM
    add_new_arc(net, state, (int) car(labels), dest, arc_user);
#else
  add_new_arc(net, state, (int) car(labels), dest);
#endif
}

#if NOT_USED
static NETptr pair_intersect_fsm (NETptr n1, NETptr n2, int minimize_p)
     /*****
      *
      * Returns the intersection of two FSMs.  Do not call directly.
      * The top level function INTERSECT_FSM initializes important values.
      *
      ******/
{  
  STATEptr state, dest_state, old1, old2;
  DESTptr dest_array;
  ARCptr a, new_arc;
  int max;
  float arc_density1, arc_density2;
  NETptr new_net, dense_net;
  DESTptr dest;
  CONSptr cur;
  STATE_MRptr state_mr = NULL;
  
  align_sigmas(n1, n2);
  
  /* Nets don't have to be deterministic but they must be epsilonfree. */
  epsilon_remove(n1, DONT_COPY);
  epsilon_remove(n2, DONT_COPY);

  max = pair_intersect_preprocess(n1, n2);
  /* initialize memory */
  dest_array = (DESTptr) calloc((size_t) max+1, (size_t) sizeof(DESTtype));
  
  if (dest_array == NULL)
    not_enough_memory("INTERSECT_FSM");
  
  new_net = make_network();
  NET_labels(new_net) =
    make_alph(LIM_id_count(LABEL_MAP), BINARY_VECTOR);
  NET_sigma(new_net) = sigma_union(NET_sigma(n1), NET_sigma(n2));

  /* No epsilons because N1 and N2 are known to be epsilonfree. */

  NET_epsilon_free(new_net) = TRUE;
  
  /* Adjusted later, if a non-deterministic state appears. */
  NET_deterministic(new_net) = TRUE;
  
  if (NET_loop_free(n1) && NET_loop_free(n2))
    NET_loop_free(new_net) = TRUE;
  
  if (NET_closed_sigma(n1) && NET_closed_sigma(n2))
    NET_closed_sigma(new_net) = TRUE;
  
  if (NET_has_arc_user_pointer(n1) || NET_has_arc_user_pointer(n2))
    NET_has_arc_user_pointer(new_net) = TRUE;
  
  /* dest_array gets loaded from net with lower arc density. */
  
  arc_density1 = (float) NET_num_arcs(n1) / NET_num_states(n1);
  arc_density2 = (float) NET_num_arcs(n2) / NET_num_states(n2);
  if (arc_density2 > arc_density1)
    dense_net = n2;
  else
    dense_net = n1;
  
  /* The net larger net is used for indexing state pairs. */

  if (NET_num_states(n2) > NET_num_states(n1))
    init_state_manager(n1, n2, new_net, n2);
  else
    init_state_manager(n1, n2, new_net, n1);

  NET_start_state(new_net) =
    find_pair_state(NET_start_state(n1), NET_start_state(n2), state_mr);
  
  while ((state = next_new_state(state_mr)))
    {
      cur = (CONSptr) STATE_client_cell(state);
      STATE_client_cell(state) = NULL;

      old1 = (STATEptr) car(cur);
      old2 = (STATEptr) cdr(cur);
      reclaim_cell(cur, state_mr);
      
      STATE_final(state) = STATE_final(old1) && STATE_final(old2);
      
      /* Initially deterministic. */
      
      if (STATE_arc_set(old1) != NULL && STATE_arc_set(old2) != NULL)
        { 
          
          /* 1st sweep:  load the array with destinations of arcs
             from less dense net */
          
          if (dense_net == n2)
            a = STATE_arc_set(old1);
          else
            a = STATE_arc_set(old2);
          
          do {
            dest = &dest_array[ARC_label(a)];
            switch (DEST_type(dest))
              {
                case(0):  /* dest is NULL */
                DEST_state(dest) = ARC_destination(a);
                DEST_type(dest) = 1;
                DEST_mark(dest) = 0;
                break;
                case(1): /* dest currently contains one item -- make a list */
                  DEST_cons(dest) = cons(ARC_destination(a),
                                         cons(DEST_state(dest), NULL));
                DEST_type(dest) = 2;
                break;
                case(2): /* dest contains a list of items */
                  DEST_cons(dest) = cons(ARC_destination(a), DEST_cons(dest));
                break;
              }
            a = next_arc(a);
          } while (a != NULL);
          
          /* 2nd sweep:  scan for matching arcs in more dense net */
          if (dense_net == n2)
            a = STATE_arc_set(old2);
          else
            a = STATE_arc_set(old1);
          
          do {
            dest = &dest_array[ARC_label(a)];
            
            switch (DEST_type(dest))
              {
              case (0):  /* there is no match */
                break;
              case (1): /* match has only one destination */
                
                NET_labels(new_net) = sigma_add_to(NET_labels(new_net),
                                                   ARC_label(a));
                if (dense_net == n2) /* make sure state from n1 is first */
                  dest_state =
                    find_pair_state(DEST_state(dest), ARC_destination(a),
				    state_mr);
                else
                  dest_state =
                    find_pair_state(ARC_destination(a), DEST_state(dest),
				    state_mr);
                
                new_arc = make_arc(ARC_label(a), dest_state, new_net);
                next_arc(new_arc) = STATE_arc_set(state);
                STATE_arc_set(state) = new_arc;
                
                /* If mark is not 0, we already have an arc with
                   the same label.
                   */
                
                if (DEST_mark(dest) == 0)
                  DEST_mark(dest) = 1;
                else
                  STATE_deterministic(state) = FALSE;
                
                break;
              case (2): /* match has several destinations */
                NET_labels(new_net) = sigma_add_to(NET_labels(new_net),
                                                   ARC_label(a));
                STATE_deterministic(state) = FALSE;
                for (cur = DEST_cons(dest); cur != NULL; cur = cdr(cur))
                  {
                    if (dense_net == n2)
                      dest_state =
                        find_pair_state(car(cur), ARC_destination(a),
					state_mr);
                    else
                      dest_state =
                        find_pair_state(ARC_destination(a), car(cur),
				       state_mr);
                    
                    new_arc = make_arc(ARC_label(a), dest_state, new_net);
                    next_arc(new_arc) = STATE_arc_set(state);
                    STATE_arc_set(state) = new_arc;
                  }
                break;
              }
            a = next_arc(a);
          } while (a != NULL);
          
          /* 3rd sweep:  clear array by sweeping the arcs of a state
             from the less dense net */
          if (dense_net == n2)
            a = STATE_arc_set(old1);
          else
            a = STATE_arc_set(old2);
          
          do {
            dest = &dest_array[ARC_label(a)];
            switch (DEST_type(dest))
              {
              case (1):
                DEST_state(dest) = NULL;
                break;
              case (2):
                /* release cons cells for future use */
                for (cur = DEST_cons(dest);
                     cur != NULL;
                     cur = freeing_CDR(cur));
                DEST_cons(dest) = NULL;
                break;
              }
            DEST_type(dest) = 0;
            DEST_mark(dest) = 0;
            a = next_arc(a);
          } while (a != NULL);
          
          if (!(STATE_deterministic(state)))
            NET_deterministic(new_net) = FALSE;
        }  
    }
  
  /* release memory */
  free((void *) dest_array);
  free_state_manager(state_mr);
  
  /* Convert to standard type */
  binary_to_label(NET_labels(new_net));
  
  if (minimize_p == DO_MINIMIZE)
    min_fsm(new_net, DONT_COPY);
  
  set_net_arity(new_net);
  
  return(new_net);
}
#endif /* NOT_USED */

#include "fst/xle_intersect.h"
/* XLE needs to know the size of these objects, because it allocates
 * an array of them for intersection.  therefore they're declared in
 * xle_intersect.h rather than here.
*/

#if 0 
typedef struct CONTENTS {
  STATEptr state;
  void *user_ptr;
  struct CONTENTS *next;
}CONTENTStype, *CONTENTSptr;
 
typedef struct DEST2_CELL {
  byte type;     /* 0 = empty, 1 = single state, 2 = list of states */
  byte mark;     /* 0 = unmarked, 1 = visited */
  CONTENTSptr content_ptr;
} DEST2type, *DEST2ptr;
#endif /*  0 */

#define DEST2_type(X) (X)->type
#define DEST2_mark(X) (X)->mark
#define DEST2_content(X) (X)->content_ptr
#define DEST2_state(X) (X)->content_ptr->state
#define DEST2_user_ptr(X) (X)->content_ptr->user_ptr
#define DEST2_next(X) (X)->content_ptr->next
#define CONTENTS_state(X) (X)->state
#define CONTENTS_user_ptr(X) (X)->user_ptr
#define CONTENTS_next(X) (X)->next

static void free_contents_cell(CONTENTSptr *cell_ptr)
{ CONTENTSptr contents_cell = *cell_ptr;
  contents_cell->state = NULL;
  contents_cell->user_ptr = NULL;
  contents_cell->next = NULL;
  free(contents_cell);
  *cell_ptr = NULL;
}


static CONTENTSptr new_contents_cell(STATEptr state, void *user_ptr, 
                                     CONTENTSptr next)
{
  CONTENTSptr contents_cell = malloc(sizeof(CONTENTStype));
  if (contents_cell == NULL) {
    not_enough_memory("INTERSECT_FSM");
  }
  contents_cell->state = state;
  contents_cell->user_ptr = user_ptr;
  contents_cell->next = next;
  return contents_cell;
}

static DEST2ptr make_dest_array(NETptr n1, NETptr n2)
{
  int max = pair_intersect_preprocess(n1, n2);
  /* initialize memory */
  DEST2ptr dest_array = 
    (DEST2ptr) calloc((size_t) max + 1, (size_t) sizeof(DEST2type));
 
  if (dest_array == NULL)
    not_enough_memory("INTERSECT_FSM");
  
  return dest_array;
}

static  DEST2ptr index_dest_array(ARCptr a,  DEST2ptr dest_array,
				  void **user_ptr)
{
  *user_ptr = NULL;
  return &dest_array[ARC_label(a)];
}

static id_type get_new_label(ARCptr a, void *user_ptr)
{
   return ARC_label(a);
}

static NETptr new_pair_intersect_fsm (NETptr n1, NETptr n2, int minimize_p, 
				      DEST2ptr
				      (*make_dest_array)(NETptr, NETptr),
				      DEST2ptr (*index_dest_array)
				      (ARCptr, DEST2ptr, void **),
				      id_type (*get_new_label)
				      (ARCptr, void *))

 /***** * * Returns the intersection of two FSMs.  Do not call directly.
          * The top level functionINTERSECT_FSM initializes important values.  
 ******/ 
{
  STATEptr state, dest_state, old1, old2;
  STATE_MRptr state_mr;
  DEST2ptr dest_array;
  ARCptr a, new_arc; 
  float arc_density1, arc_density2;
  NETptr new_net, dense_net;
  DEST2ptr dest;
  CONTENTSptr temp_cell;
  CONSptr cur;
  id_type new_label;
  void *user_ptr;
                       
  align_sigmas(n1, n2);

 /* Nets don't have to be deterministic but they must be epsilonfree. */
  epsilon_remove(n1, DONT_COPY); 
  epsilon_remove(n2, DONT_COPY);

  dest_array = make_dest_array(n1, n2);
  
  new_net = make_network();

  NET_labels(new_net) =
    make_alph(LIM_id_count(LABEL_MAP), BINARY_VECTOR);
  NET_sigma(new_net) = sigma_union(NET_sigma(n1), NET_sigma(n2));

  /* No epsilons because N1 and N2 are known to be epsilonfree. */

  NET_epsilon_free(new_net) = TRUE;
  
  /* Adjusted later, if a non-deterministic state appears. */
  NET_deterministic(new_net) = TRUE;
  
  if (NET_loop_free(n1) && NET_loop_free(n2))
    NET_loop_free(new_net) = TRUE;
  
  if (NET_closed_sigma(n1) && NET_closed_sigma(n2))
    NET_closed_sigma(new_net) = TRUE;
  
  if (NET_has_arc_user_pointer(n1) || NET_has_arc_user_pointer(n2))
    NET_has_arc_user_pointer(new_net) = TRUE;
  
  /* dest_array gets loaded from net with lower arc density. */
  
  arc_density1 = (float) NET_num_arcs(n1) / NET_num_states(n1);
  arc_density2 = (float) NET_num_arcs(n2) / NET_num_states(n2);
  if (arc_density2 > arc_density1)
    dense_net = n2;
  else
    dense_net = n1;
  
  /* The net larger net is used for indexing state pairs. */
  if (NET_num_states(n2) > NET_num_states(n1))
    state_mr = init_state_manager(n1, n2, new_net, n2);
  else
    state_mr = init_state_manager(n1, n2, new_net, n1);

  NET_start_state(new_net) =
    find_pair_state(NET_start_state(n1), NET_start_state(n2), state_mr);
  
  while ((state = next_new_state(state_mr)))
    {
      cur = (CONSptr) STATE_client_cell(state);
      STATE_client_cell(state) = NULL;
      
      old1 = (STATEptr) car(cur);
      old2 = (STATEptr) cdr(cur);
      
      reclaim_cell(cur, state_mr);
      
      STATE_final(state) = STATE_final(old1) && STATE_final(old2);
      
      /* Initially deterministic. */
      
      if (STATE_arc_set(old1) != NULL && STATE_arc_set(old2) != NULL)
        { 
          
          /* 1st sweep:  load the array with destinations of arcs
             from less dense net */
          
          if (dense_net == n2)
            a = STATE_arc_set(old1);
          else
            a = STATE_arc_set(old2);
          
          do {
            dest = index_dest_array(a, dest_array, &user_ptr);
            switch (DEST2_type(dest))
              {
                case(0): {  /* dest is NULL */
                  DEST2_content(dest) = new_contents_cell(ARC_destination(a),
                                                       user_ptr, NULL);
                  DEST2_type(dest) = 1;
                  DEST2_mark(dest) = 0;
                  break;
		}
                case(1): {/* dest currently contains one item -- make a list */
                  DEST2_next(dest) = new_contents_cell(ARC_destination(a),
                                                       user_ptr, NULL);
                  DEST2_type(dest) = 2;
                  break;
                }
                case(2): { /* dest contains a list of items */ 
                  temp_cell = new_contents_cell(ARC_destination(a), user_ptr,
                                           DEST2_content(dest));
                  DEST2_content(dest) = temp_cell;
                  break;
                }
              }
            a = next_arc(a);
          } while (a != NULL);
          
          /* 2nd sweep:  scan for matching arcs in more dense net */
          if (dense_net == n2)
            a = STATE_arc_set(old2);
          else
            a = STATE_arc_set(old1);
          
          do {
            dest = index_dest_array(a, dest_array, &user_ptr);
            
            switch (DEST2_type(dest))
              {
              case (0):  { /* there is no match */
                break;
              }
              case (1): { /* match has only one destination */
                new_label = get_new_label(a, DEST2_user_ptr(dest));
      
                NET_labels(new_net) = sigma_add_to(NET_labels(new_net),
                                                   new_label);
                if (dense_net == n2) /* make sure state from n1 is first */
                  dest_state =
                    find_pair_state(DEST2_state(dest), ARC_destination(a),
				    state_mr);
                else
                  dest_state =
                    find_pair_state(ARC_destination(a), DEST2_state(dest),
				    state_mr);
                
                new_arc = make_arc(new_label, dest_state, new_net);
                next_arc(new_arc) = STATE_arc_set(state);
                STATE_arc_set(state) = new_arc;
                
                /* If mark is not 0, we already have an arc with
                   the same label.
                   */
                
                if (DEST2_mark(dest) == 0)
                  DEST2_mark(dest) = 1;
                else
                  STATE_deterministic(state) = FALSE;
                
                break;
              }
              case (2): { /* match has several destinations */

                STATE_deterministic(state) = FALSE;
                for (temp_cell = DEST2_content(dest);
		     temp_cell != NULL;
		     temp_cell = temp_cell->next)
                  {
                    if (dense_net == n2)
                      dest_state =
                        find_pair_state(CONTENTS_state(temp_cell),
                                       ARC_destination(a), state_mr);

                    else
                      dest_state =
                        find_pair_state(ARC_destination(a),
					CONTENTS_state(temp_cell),
					state_mr);

                    new_label= get_new_label(a, CONTENTS_user_ptr(temp_cell));
                    NET_labels(new_net) = sigma_add_to(NET_labels(new_net),
                                                  new_label);
                    new_arc = make_arc(new_label, dest_state, new_net);
                    next_arc(new_arc) = STATE_arc_set(state);
                    STATE_arc_set(state) = new_arc;
                  }
                break;
              }}
            a = next_arc(a);
          } while (a != NULL);
          
          /* 3rd sweep:  clear array by sweeping the arcs of a state
             from the less dense net */
          if (dense_net == n2)
            a = STATE_arc_set(old1);
          else
            a = STATE_arc_set(old2);
          
          do {
            dest = index_dest_array(a, dest_array, &user_ptr);
            switch (DEST2_type(dest))
              {
              case (1): {
               free_contents_cell(&DEST2_content(dest));
                break;
              }
              case (2): {
                /* release content cells for future use */
                while (DEST2_content(dest) != NULL) {
                  temp_cell = DEST2_content(dest);
                  DEST2_content(dest) = DEST2_next(dest);
                  free_contents_cell(&temp_cell);
                }
                break;
              }
            }
            DEST2_type(dest) = 0;
            DEST2_mark(dest) = 0;
            a = next_arc(a);
          } while (a != NULL);
          
          if (!(STATE_deterministic(state)))
            NET_deterministic(new_net) = FALSE;
        }  
    }
  
  /* release memory */
  free((void *) dest_array);
  /* PURIFY PROBLEM---HAVE FST TEAM REVIEW */

  free_state_manager(state_mr);
  
  /* Convert to standard type */
  binary_to_label(NET_labels(new_net));
  
  if (minimize_p == DO_MINIMIZE)
    min_fsm(new_net, DONT_COPY);
  
  set_net_arity(new_net);
  
  return(new_net);
}

int intersect_p (NETptr n1, NETptr n2)
     /*****
      *
      * Returns TRUE if the languages of N1 and N2 have at least one string
      * in common, otherwise false.  Similar to PAIR_INTERSECT_FSM except
      * that we don't compute the intersection and quit as soon as we find
      * that the intersection, if computed, would not be a null fsm.
      *
      ******/
{  
  STATEptr s1, s2;
  DESTptr dest_array, dest;
  CONSptr pair, cur;
  HEAPptr cons_heap;
  STACKptr cons_stack;
  STATE_MRptr state_mr;
  float arc_density1, arc_density2;
  NETptr dense_net;
  ARCptr a;
  
  int max, result = FALSE;
  
  if (n1 == n2)
    return(TRUE);
  
  
  /* If necessary, switch argument nets so that the N1 is the net with the
     most states.  State client cells in N1 are used to store a binary tree
     containing sister states in N2.
     */
  
  if (NET_num_states(n2) > NET_num_states(n1))
    {
      NETptr temp = n2;
      n2 = n1;
      n1 = temp;
    }
  
  clear_client_cells(n1);
  
  /* Nets don't have to be deterministic but they must be epsilonfree. */

  epsilon_remove(n1, DONT_COPY);
  epsilon_remove(n2, DONT_COPY);

  align_sigmas(n1, n2);
  
  max = pair_intersect_preprocess(n1, n2);
  /* initialize memory */
  dest_array = (DESTptr) calloc((size_t) max + 1, (size_t) sizeof(DESTtype));
  
  if (dest_array == NULL)
    not_enough_memory("INTERSECT_P");
  
  /* dest_array gets loaded from net with lower arc density. */
  
  arc_density1 = (float) NET_num_arcs(n1) / NET_num_states(n1);
  arc_density2 = (float) NET_num_arcs(n2) / NET_num_states(n2);
  
  if (arc_density2 > arc_density1)
    dense_net = n2;
  else
    dense_net = n1;
  
  /* This application does not create any new states. For this reason
     the third argument in the next call is NULL. */

  state_mr = init_state_manager(n1, n2, NULL, n1);
  cons_heap = STATE_MR_cons_heap(state_mr);
  cons_stack = STATE_MR_new_states(state_mr);

  pair_states(NET_start_state(n1), NET_start_state(n2), state_mr);
  
  while ((pair = (CONSptr) pop(cons_stack)))
    {
      s1 = (STATEptr) car(pair);
      s2 = (STATEptr) cdr(pair);
      
      /* If S1 and S2 are both final, the intersection of N1 and N2
         is non-empty.
         */
      
      free_cons_cell(pair, cons_heap);
      
      if (STATE_final(s1) && STATE_final(s2))
        {
          result = TRUE;
          break;
        }
      
      if (STATE_arc_set(s1) != NULL && STATE_arc_set(s2) != NULL)
        {
          /* 1st sweep:  load the array with destinations of arcs
             from less dense net */
          
          if (dense_net == n2)
            a = STATE_arc_set(s1);
          else
            a = STATE_arc_set(s2);
          
          do {
            dest = &dest_array[ARC_label(a)];
            switch (DEST_type(dest))
              {
                case(0):  /* dest is NULL */
                  DEST_state(dest) = ARC_destination(a);
                DEST_type(dest) = 1;
                break;
                case(1): /* dest currently contains one item -- make a list */
                  DEST_cons(dest) = cons_cell(ARC_destination(a),
					      cons_cell(DEST_state(dest), NULL,
							cons_heap),
					      cons_heap);
                DEST_type(dest) = 2;
                break;
                case(2): /* dest contains a list of items */
                  DEST_cons(dest) = cons_cell(ARC_destination(a),
					      DEST_cons(dest),
					      cons_heap);
                break;
              }
            a = next_arc(a);
          } while (a != NULL);
          
          /* 2nd sweep:  scan for matching arcs in more dense net */
          if (dense_net == n2)
            a = STATE_arc_set(s2);
          else
            a = STATE_arc_set(s1);
          
          do {
            dest = &dest_array[ARC_label(a)];
            
            switch (DEST_type(dest))
              {
              case (0):  /* there is no match */
                break;
              case (1): /* match has only one destination */
                
                if (dense_net == n2) /* make sure state from n1 is first */
                  pair_states(DEST_state(dest), ARC_destination(a),
			      state_mr);
                else
                  pair_states(ARC_destination(a), DEST_state(dest),
			      state_mr);
                break;
              case (2): /* match has several destinations */
                for (cur = DEST_cons(dest); cur != NULL; cur = cdr(cur))
                  {
                    if (dense_net == n2)
                      pair_states(car(cur), ARC_destination(a), state_mr);
                    else
                      pair_states(ARC_destination(a), car(cur), state_mr);
                  }
                break;
              }
            a = next_arc(a);
          } while (a != NULL);
          
          /* 3rd sweep:  clear array by sweeping the arcs of a state
             from the less dense net */
          if (dense_net == n2)
            a = STATE_arc_set(s1);
          else
            a = STATE_arc_set(s2);
          
          do {
            dest = &dest_array[ARC_label(a)];
            switch (DEST_type(dest))
              {
              case (1):
                DEST_state(dest) = NULL;
                break;
              case (2):
                /* release cons cells for future use */
                for (cur = DEST_cons(dest);
                     cur != NULL;
                     cur = freeing_CDR_cell(cur, cons_heap));
                DEST_cons(dest) = NULL;
                break;
              }
            DEST_type(dest) = 0;
            a = next_arc(a);
          } while (a != NULL);
        }
    }
  
  /* release memory */
  free((void *) dest_array);
  free_state_manager(state_mr);
  
  return(result);
}

static int netcmp(NETptr n1, NETptr n2)
{
  /* Returns 0 if N1 has the same number of arcs and states as N2
     -1 if N1 has fewer arcs than N2 or the same number of arcs
     but fewer states, otherwise +1.
     */
  
  if (NET_num_arcs(n1) == NET_num_arcs(n2))
    {
      if (NET_num_states(n1) == NET_num_states(n2))
        return (0);
      else if (NET_num_states(n1) < NET_num_states(n2))
        return (-1);
      else
        return (1);
    }
  else if (NET_num_arcs(n1) < NET_num_arcs(n2))
    return (-1);
  else
    return (1);
  
}

static int nv_comp_fn(const void *n1, const void *n2)
{
  /* can't return difference directly because the values are long.  */
  
  return(netcmp(*(NETptr *) n1, *(NETptr *) n2));
}

static void sort_nets(NETptr *nets, int len)
     /*****
      *
      * Sorts a vector of nets.  First key is number of states.
      * Second key is number of arcs.
      *
      *****/
{
  QSORT((void *) nets, (size_t) len, (size_t) sizeof(NETptr),
        nv_comp_fn);
}

/********************
 *                  *
 *   COMPOSITION    *
 *                  *
 ********************/


static CONSptr record_labels(CONSptr new_list, id_type lower,
                             CONSptr *lower_array, CONSptr *LABEL_LIST,
			     HEAPptr cons_heap)
{
  /* Stores NEW_LIST in the location LOWER_ARRAY[LOWER].  Updates
     LABEL_LIST by appending NEW_LIST to it.  Returns NEW_LIST.
     */
  lower_array[lower] = new_list;
  *LABEL_LIST = cons_cell_append(new_list, *LABEL_LIST, cons_heap);
  return(new_list);
}


static CONSptr upper_eps_labels (id_type upper, ALPHABETptr upper_other,
				 HEAPptr cons_heap)
{
  /* UPPER is an ID for a label that has an EPSILON or the lower
     side or a flag diacritic that is being treated as an epsilon.
     Returns a list of label ids containing UPPER and the labels that
     result from the expansion if X = OTHER.
     */
  return(expand_upper_side(upper_id(upper), lower_id(upper), upper_other, 
			   FALSE, cons_heap));
}

static CONSptr lower_eps_labels (id_type lower, ALPHABETptr lower_other,
				 HEAPptr cons_heap)
{
  /* LOWER is an ID for a label that has an EPSILON on the upper
     side or a flag diacritic that is being treated as an epsilon.
     Returns a list of label ids containing LOWER and the labels that
     result from the expansion if Y = OTHER.
     */
  return(expand_lower_side(upper_id(lower), lower_id(lower), lower_other,
			   FALSE, cons_heap));
}

CONSptr *get_lower_array(CONSptr **upper_array, id_type upper, id_type lower,
				id_type lower_max)
{

  CONSptr *lower_array;

  /* Lower should always be smaller than lower_max. If it isn't we
     have a programming error. Instead of crashing, we'll recover
     recover quietly. In DEBUG mode, we print an error message. */

  lower_array = upper_array[upper];

  if (!lower_array)
    {
      if (lower >= lower_max)
        {
          lower_max = lower+1;
#ifdef DEBUG
      fsm_fprintf(stderr, "Array size too small in 'get_lower_array'.\n");
#endif          
        }

      lower_array =
	(CONSptr*) calloc((size_t) lower_max, (size_t) sizeof(CONSptr));

      if (!lower_array)
	not_enough_memory("GET_LOWER_ARRAY");

      upper_array[upper] = lower_array;
    }
  else if (lower >= lower_max)
    {
      lower_array = upper_array[upper] = realloc(upper_array[upper],
                                                 (lower + 1)*sizeof(CONSptr));
#ifdef DEBUG
      fsm_fprintf(stderr, "Array size too small in 'get_lower_array'.\n");
#endif
    }


  return(lower_array);
}                  



static int other_other_p(id_type id)
{
  /* Returns TRUE iff the label is OTHER:OTHER */

  return(tuple_id_p(id) && 
	 id_to_upper_id(id) == OTHER &&
	 id_to_lower_id(id) == OTHER);
}

static int non_identity_test(id_type upper, id_type lower)
{
  /* If OTHER appears on the outer side of UPPER or LOWER,
     we need to know whether it's expansion in expand_fstpair
     should be allowed to give rise to an identity.
     This question arises when one of the two labels is
     an OTHER:OTHER. Composing OTHER:OTHER OTHER:b and
     two results OTHER:b and b if b is not in the sigma of
     the upper machine. For the same reason, the composition
     of OTHER:OTHER with b:OTHER may yield both OTHER:OTHER
     and OTHER, if b is not in the sigma of the upper
     network.
     
     However the composition of OTHER:OTHER b only should yield
     only OTHER:b and not b even if b is not in the
     signa of the upper network because OTHER:OTHER
     is a combination of two non-identical symbol. The
     same goes for the composition of OTHER:OTHER OTHER
     and OTHER OTHER:OTHER.
     */


  if (other_other_p(upper))
    return(!tuple_id_p(lower));

  if (other_other_p(lower))
    return(!tuple_id_p(upper));

  return(FALSE);
}

static CONSptr compose_labels (id_type upper, id_type lower,
			       ALPHABETptr upper_other,
			       ALPHABETptr lower_other,
			       HEAPptr cons_heap)
{
  /* Returns a list of IDs of one or more composite labels constructed
     from the components of UPPER and LOWER. Need to distinguish five
     cases because we are expanding OTHER on the fly:
     
     (1) if UPPER and LOWER are both OTHER (= unknown mapped to itself),
     the result is OTHER.
     (2) If UPPER is OTHER, we examine the components of LOWER
     to determine whether it contains a one-sided other that
     needs to be expanded. If not, the result is LOWER plus
     any labels resulting from the expansion of OTHER on the
     lower side of LOWER.
     (3) If LOWER is OTHER, we examine the components of UPPER to
     see if it needs expansion. If not the result is UPPER plus
     any labels resulting from the expansion OTHER on the upper
     side of UPPER.
     (4) In the case of a direct literal match, we pair the upper side
     of UPPER with the lower side of LOWER, expanding OTHER
     on both sides provided that UPPER and LOWER are both symbol
     pairs.
      
     We use UPPER_OTHER to expand OTHER in the components of UPPER
     and LOWER_OTHER for the components of LOWER.
  
     */

  if (upper == OTHER && lower == OTHER)
    return(cons_cell(cast_to_void(OTHER), NULL, cons_heap));
  else if (upper == OTHER)
    return(expand_lower_side(upper_id(lower), lower_id(lower),
			     lower_other, TRUE, cons_heap));
  else if (lower == OTHER)
    return(expand_upper_side(upper_id(upper), lower_id(upper), upper_other,
			     TRUE, cons_heap));
  else 
    return(expand_fstpair(upper, lower, upper_other, lower_other,
			  non_identity_test(upper, lower), cons_heap));
}

int scan_labels(NETptr net, NETptr other_net, id_type *max_label,
		id_type *max_low, id_type *max_hi, ALPHABETptr *other_sigma)
{
  /*  Scans the labels of NET and sets the variable *MAX_LABEL,
   *LOW_MAX, *OTHER_SIGMA to appropriate values.  *MAX_LABEL
   will be the largest label ID.  If *MAX_LOW is not a NULL
   pointer, it will be set to the largest lower ID considering all
   simple labels and the second coordinates of fstpairs.
   If OTHER appears as a label constituent in NET, *OTHER_SIGMA
   will be an alphabet pointer containing the labels in OTHER_NET
   that don't appear in NET.  Otherwise *OTHER_SIGMA is set to
   an alphabet pointer for a null sigma.
   */
  
  ALPHABETptr labels = binary_to_label(NET_labels(net)); 
  id_type id;
  int  i, other_p = FALSE;
  LABELptr label;
  
  *max_label = 0;
  if (max_low != NULL)
    *max_low = 0;
  if (max_hi != NULL)
    *max_hi = 0;
  
  for (i = 0; i < ALPH_len(labels); i++)
    {
      id = ALPH_item(labels, i);
      
      if (id > *max_label)
        *max_label = id;
      
      label = id_to_label(id);
      if (LABEL_arity(label) == 2 && FSM_MODE != TWOL_MODE)
        {
          id_type upper = get_field(label, UPPER);
          id_type lower = get_field(label, LOWER);
          
          if (upper == OTHER || lower == OTHER)
            other_p = TRUE;
          
          if (max_low != NULL && lower > *max_low)
            *max_low = lower;
          if (max_hi != NULL && upper > *max_hi)
            *max_hi = upper;        
        }
      else
        {
          if (id == OTHER)
            other_p = TRUE;
          
          if (max_low != NULL && id > *max_low)
            *max_low = id;
          if (max_hi != NULL && id > *max_hi)
            *max_hi = id;        
        }
    }
  
  if (other_p)
    *other_sigma = sigma_difference(sigma(other_net), sigma(net));
  else
    *other_sigma = make_alph(0, BINARY_VECTOR);
  
  return (other_p);
}


static void compose_make_arc_stacks(NETptr upper_fst, int upper_max,
                                    int upper_low_max, STACKptr **stacks,
                                    STACKptr **map, int flags_special_p)
{
  /* 
    STACKS has a place for each symbol in the lower alphabet
    of UPPER_FST.  We create a stack for storing arcs for all
    labels excluding the ones that have EPSILON on the lower
    side. The position corresponding to EPSILON remains NULL.
    MAP is another indexing of the same stacks based on the
    labels of UPPER_FST.  It maps each label to the stack of
    its lower coordinate.  This saves time in the main loop
    because we can locate the stack for storing the arc by
    its label without having to compute the lower coordinate
    of the label.
    */
  
  ALPHABETptr upper_labels = NET_labels(upper_fst);
  STACKptr stack, *arc_stacks, *upper_map;
  int i;
  
  arc_stacks = (STACKptr *) calloc((size_t) upper_low_max + 1,
                                   (size_t) sizeof(STACKtype));
  
  upper_map = (STACKptr *) calloc ((size_t) upper_max + 1,
                                   (size_t) sizeof(STACKtype));
  
  if (arc_stacks == NULL || upper_map == NULL)
    not_enough_memory("MAKE_ARC_STACKS");
  
  *stacks = arc_stacks;
  *map = upper_map;
  
  for (i = 0; i < ALPH_len(upper_labels); i++)
    {
      id_type upper = ALPH_item(upper_labels, i),
      upper_low = lower_id(upper);
      
      if (!(upper_low == EPSILON ||
	    (flags_special_p == DO_OBEY && flag_diacritic_p(upper_low))))
        {
          stack = upper_map[upper];
          if (stack == NULL)
            {
              stack = arc_stacks[upper_low];
              
              if (stack == NULL)
                {
                  stack = create_stack("Compose arcs to do");
                  arc_stacks[upper_low] = stack;
                }
              upper_map[upper] = stack;
            }
        }
    }
}

static void free_arc_stacks(STACKptr *stack_vector, int max_element)
{
  /* Frees all the stacks in the STACK_VECTOR. */
  
  int i;
  STACKptr stack;
  for (i = 0; i <= max_element; i++)
    {
      stack = stack_vector[i];
      if (stack != NULL)
        free_stack(stack);
    }
}

static void compose_make_lower_map (id_type lower_max, id_type max_addr,
                                    ALPHABETptr lower_labels,
                                    ALPHABETptr upper_other,
                                    ALPHABETptr lower_other,
                                    STACKptr *arc_stacks,
				    STATE_MRptr state_mr,
                                    CONSptr **lower_map,
				    int flags_special_p)
{
  /* 
    ADDR_LISTS has a list for each label in  LOWER_FST.  The list
    contains the stacks in ARC_STACKS that in which matching arcs
    of UPPER_FST are stored.  The matching conditions are:
    (1) epsilon doesn't match anything.
    (2) every non-epsilon symbol matches itself.
    (3) a non-other, non-epsilon symbol matches OTHER if it is not
    in the alphabet of the other transducer.
    */
  STACKptr stack;
  HEAPptr cons_heap = STATE_MR_cons_heap(state_mr);
  
  id_type lower, lower_high, id;
  int i, j;
  
  CONSptr *addr_lists = (CONSptr *) calloc((size_t) lower_max + 1,
                                           (size_t) sizeof(CONSptr));
  
  if (addr_lists == NULL)
    not_enough_memory("MAKE_LOWER_MAP");
  
  
  
  /* If the lower side of UPPER_FST or the upper side of LOWER_FST
     contains OTHER, we need to augment address lists.
     Since they were created by SIGMA_DIFFERENCE, UPPER_OTHER and
     LOWER_OTHER are binary vectors.  To speed up processing, we
     use convert LOWER_OTHER to a label vector now. Both need to be
     label vectors in the COMPOSE_STATE_PAIRS loop.
     */
  
  label_to_binary(upper_other);
  binary_to_label(lower_other);
  
  for (i = 0; i < ALPH_len(lower_labels); i++)
    {
      lower = ALPH_item(lower_labels, i);
      lower_high = upper_id(lower);
      
      if (!(lower_high == EPSILON ||
	    (flags_special_p == DO_OBEY && flag_diacritic_p(lower_high))))
        {
          /* Case (2) every symbol matches itself. */
          if (lower_high <= max_addr)
            {
              stack = arc_stacks[lower_high];
              if (stack != NULL)
                addr_lists[lower]
		  = cons_cell((void *) stack, NULL, cons_heap);
            }
          if (lower_high == OTHER)
            /* Add all the symbols that OTHER can match, besides itself,
               to the list of matches for this label. */
            for (j = 0; j < ALPH_len(lower_other); j++)
              {
                id = ALPH_item(lower_other,j);
                if (id <= max_addr)
                  {
                    stack = arc_stacks[id];
                    if (stack != NULL)
                      addr_lists[lower] =
			cons_cell((void *) stack,
				  addr_lists[lower],
				  cons_heap);
                  }
              }
          else
            /* If the upper symbol is in UPPER_OTHER, add OTHER to the
               list of matches for this label.
               */
            if (lower_high < ALPH_len(upper_other) &&
                ALPH_item(upper_other, lower_high) == 1)
              {
                stack = arc_stacks[OTHER];
                if (stack != NULL)
                  addr_lists[lower] =
                    cons_cell((void *) stack, addr_lists[lower], cons_heap);
              }
          if (addr_lists[lower] == NULL)
            /* No matches for this label in UPPER_FST. */
            addr_lists[lower] = cons_cell(NULL, NULL, cons_heap);
        }               
    }   
  *lower_map = addr_lists;
}


static void collect_labels(NETptr net, CONSptr LABEL_LIST)
{
  /* LABEL_LIST is a list of labels created for NET.  The list may contain
     several instances of the same label.
     */
  
  ALPHABETptr labels;
  
  labels = make_alph(LIM_id_count(LABEL_MAP), BINARY_VECTOR);
  
  for ( ; LABEL_LIST != NULL; LABEL_LIST = cdr(LABEL_LIST))
      sigma_add_to(labels, (int) car(LABEL_LIST));
  
  binary_to_label(labels);
  NET_labels(net) = labels;
}

STATEptr make_phantom_state(STATEptr model_state,
			    NETptr *phantom_net, int no_arcs_p,
			    int flags_special_p)
{
  /* Adds a state to PHANTOM_NET that is similar to MODEL_STATE.
     If NO_ARCS_P is TRUE only finality is copied from MODEL_STATE;
     otherwise, copies are made of all arcs that are not of type X:EPSILON.
     Use with care (as in COMPOSE_STATE_PAIRS). If the FLAGS_SPECIAL_P
     is ON, a flag_
     */
  STATEptr phantom_state;
  ARCptr arc;
  id_type id;
  
  if (*phantom_net == NULL)
  	*phantom_net = null_fsm();
  
  phantom_state = make_state(NULL, *phantom_net);
  STATE_final(phantom_state) = STATE_final(model_state);
  
  if (!no_arcs_p)
    for (arc = STATE_arc_set(model_state); arc != NULL; arc = next_arc(arc))
      {
	id = lower_id(ARC_label(arc));
	if (!(id == EPSILON || (flags_special_p && flag_diacritic_p(id))))
	  push_arc(copy_arc(arc, *phantom_net), phantom_state);
      }
  
  return(phantom_state);
}


static void compose_state_pairs(NETptr net, STACKptr *upper_map,
                                CONSptr *lower_map, CONSptr **upper_array,
                                id_type lower_max,
                                ALPHABETptr upper_other,
                                ALPHABETptr lower_other,
				STATE_MRptr state_mr,
				int flags_special_p)
{
  /* The working kernel of COMPOSE_FST. */
  
  STATEptr upper_state, lower_state, result_state;
  STATEptr mini_upper, final_upper = NULL;
  STATEptr upper_dest, lower_dest, comp_dest;
  NETptr phantom_net = NULL;
  ARCptr upper_arc, lower_arc;
  CONSptr addr_list, comp_labels, cell, LABEL_LIST = NULL;
  HEAPptr cons_heap = STATE_MR_cons_heap(state_mr);
  CONSptr *lower_array;
  STACKptr stack;
  STACK_BLOCKptr stack_top;
  id_type upper, lower;
  int upper_non_eps_p, upper_eps_p;
  int i, j, k;
  
  binary_to_label(upper_other);
  binary_to_label(lower_other);
  
  /*  When a new label corresponding to upper label U lower label L
      is constructed for the first time it is stored in order for
      it to be quickly accessible. This is useful because the
      composition of two labels can give rise to multiple labels
      if identity OTHER or one-sided OTHER is involved.
      For example, when X:Y and Y:Z meet for the first time,
      the list consisting of X:Z is stored.

      For storage, we previously  used a two-dimensional array of the
      size ((max_upper_id+1) * (max_lower_id+1)) where
      max_upper_id and max_lower_id are the maximal label ids
      of the upper and lower network, respectively. This is
      wasteful and literally impossible with large labelsets.
      John Maxwell observed in XLE that allocating and freeing
      large arrays in the previous version of this function
      leads to memory fragmentation. If a 50M array is used,
      released, and allocated again, the operating system may
      have to find a new continuous 50M and the total memory used by
      the application keeps creeping up in large chunks.

      In the interest of not wasting space and to avoid fragmentation,
      the information is now stored differently. The calling function
      allocates an array of the size (max_upper_id+1). This is passed
      on to compose_state_pairs() as the argument UPPER_ARRAY.
      Initially all the cells of the array are empty. When an upper
      label U and a lower label L are composed for the first time, the
      new composite labels are stored in the location
      (upper_array[U])[L]. If upper_arrray[U] is NULL, which is always
      the case initially, we first fill it with an array of size
      (max_lower_id+1) and then store the list of composite labels
      in the location L.
      
      The new scheme has two advantages:

         (1) it uses much less space overall.
	 (2) it allocates memory in smaller chunks leading
             to less fragmentation.

      The size of UPPER_ARRAY is upper_max+1 and for any
      UPPER_ARRAY[lower_id] that needs to be allocated, the size
      is lower_max+1. But most of UPPER_ARRAY locations
      will remain empty as we don't need to fill positions
      that do not correspond to label ids in the lower network.

      When compose_state_pairs() is finished, the calling function
      frees all the arrays that have been allocated.

      2001-10-04 LK

      */
  
   lower_max++;

  LABEL_LIST = NULL;
  
  /* Loop around creating arcs for new composite states using the
     arcs from the corresponding pair of original states.  The routine
     consist of three cycles.  On the first cycle, the arcs of UPPER_STATE
     are sorted into stacks in ARC_STACKS on the basis of the lower side
     of the label.  On the second cycle, arcs of the lower stated are paired
     with the arcs in stacks determined by LOWER_MAP.  This vector maps
     the labels of the arcs in LOWER_STATE to a list of stack positions
     containing matching upper state arcs.  On the third cycle, we again
     cycle through the arcs of the upper state and clear the stack.
     */

  while ((result_state = next_new_state(state_mr)))
    {
      cell = (CONSptr) STATE_client_cell(result_state);
      upper_state = (STATEptr) car(cell);
      lower_state = (STATEptr) cdr(cell);
      
      STATE_final(result_state) =
        STATE_final(upper_state) && STATE_final(lower_state);
      
      STATE_client_cell(result_state) = NULL;
      reclaim_cell(cell, state_mr);
      
      /* If one of the original states is nonfinal and empty, we can skip
         processing the other.
         */
      if ((STATE_arc_set(upper_state) != NULL || STATE_final(upper_state)) &&
          (STATE_arc_set(lower_state) != NULL || STATE_final(lower_state)))
        {
          upper_non_eps_p = FALSE;
          upper_eps_p = FALSE;
          
          /* 1st sweep: distribute the arcs of UPPER_STATE to the
             appropriate cells in ARC_STACK_VECTOR.
             */
          
          for (upper_arc = STATE_arc_set(upper_state);
               upper_arc != NULL;
               upper_arc = next_arc(upper_arc))
            {
              upper = ARC_label(upper_arc);
              stack = upper_map[upper];
              /* If STACK is NULL, the label has EPSILON on the lower side.
                 If all arcs in the upper state are of this type and the state
                 is nonfinal, we can minimize redundancy by skipping the arcs
                 that have epsilons on the upper side.  Note that the sequence
                 <X:EPSILON><EPSILON:Y> and its inverse are equivalent.
                 */
              if (stack == NULL)
                {
                  upper_eps_p = TRUE;
                  upper_dest = ARC_destination(upper_arc);
                  comp_dest = find_pair_state(upper_dest, lower_state, 
					      state_mr);
                  
		  lower_array = 
		    get_lower_array(upper_array, upper, EPSILON, lower_max);

		  comp_labels = lower_array[EPSILON];

                  if (comp_labels == NULL)
                    /* first encounter of the pair */
                    comp_labels =
                      record_labels(upper_eps_labels(upper, upper_other,
						     cons_heap),
				    EPSILON, lower_array, &LABEL_LIST,
				    cons_heap);
                  
#ifdef FULL_FSM
                  add_new_arcs(net, comp_labels, result_state,
                               comp_dest, NULL);
#else
                  add_new_arcs(net, comp_labels, result_state,
                               comp_dest);
#endif
                }
              else
                {
                  push(upper_arc, stack);
                  upper_non_eps_p = TRUE;
                }
            }
          
          /* 2nd sweep: pair the arcs of LOWER_STATE with the matching arcs in
             UPPER_STATE.  The stacks containing them are in LOWER_MAP in
             the cell indexed by the arc label. If the cell is NULL, the lower
             arc has EPSILON on the upper side of the label.  If the cell
             contains the list (NULL), the arc label has no potential matches
             in UPPER_FST.  Otherwise the cell contains a list stacks,
             (s1 s2 ...), which contain the matching arcs (if any) encountered
             in the first sweep.  Labels of type OTHER:Y have a stack for each
             expansion of OTHER in the upper transducers alphabet, in addition
             to the stack for OTHER itself.
             */
          for (lower_arc = STATE_arc_set(lower_state);
               lower_arc != NULL;
               lower_arc = next_arc(lower_arc))
            {
              lower = ARC_label(lower_arc);
              addr_list = lower_map[lower];
              
              if (addr_list == NULL)
                {
                  /* Epsilon arc on the upper side of LOWER_FST.  Skip it
                     if the upper state is nonfinal and contains only arcs
                     of type X:EPSILON.
                     */
                  if (upper_non_eps_p || STATE_final(upper_state))
                    {
                      lower_dest = ARC_destination(lower_arc);
                      
                      /* If the upper state is final but only has epsilon
                         arcs, we can avoid redundancy by substituting a
                         phantom final state for it.  All that matters is that
                         finality is propagated along the EPSILON:Y arcs in
                         the composite network.
                         */
                      if (!upper_non_eps_p)
                        {
                          if (final_upper == NULL)
                            final_upper = 
                              make_phantom_state(upper_state, &phantom_net,
                                                 TRUE, flags_special_p);
                          
                          comp_dest =
                            find_pair_state(final_upper, lower_dest,
					    state_mr);
                        }
                      else if (upper_eps_p)
                        {
                          /* We need here a copy of the upper state containing
                             only the arcs that are not of the type X:EPSILON
                             to avoid adding a EPSILON:Y X:EPSILON path when
                             X:EPSILON EPSILON:Y is already in the works.
                             We call GET_PAIR_STATE with the order of nets
                             reversed so that LOWER_DEST may end up with
                             two composites with respect to UPPER_STATE
                             that differ in that one of them, GET_PAIR_STATE
                             (UPPER_STATE, LOWER_DEST, STATE_MR) is
			     processed with the X:EPSILON arcs in
			     UPPER_STATE whereas the other,
			     GET_PAIR_STATE(LOWER_DEST, MINI_UPPER,
                             STATE_MR), is processed without them.
                             */
                          comp_dest =
                            find_pair_state(lower_dest, upper_state,
					    state_mr);
                          
                          /* If the CAR of the client cell is
                             LOWER_DEST, this configuration has not been
                             encountered before.  If the cell is NULL,
                             COMP_DEST has already been processed.
                             */
                          cell = STATE_client_cell(comp_dest);
                          
                          if (cell != NULL &&
                              (STATEptr) car(cell) == lower_dest)
                            {
                              /* COMP_DEST is a newly created phantom state.
                                 The client cell of the state initially
                                 holds the pair (LOWER_DEST.UPPER_STATE)
                                 because of how GET_PAIR_STATE was called.
                                 We'll set MINI_UPPER to a copy of
				 UPPER_STATE without X:EPSILON arcs and
				 reset the client cell to
				 (MINI_UPPER.LOWER_DEST).  Next time
                                 around the phantom state is ready to use.
                                 */
                              mini_upper =
                                make_phantom_state(upper_state, &phantom_net,
                                                   FALSE, flags_special_p);
                              rplaca(cell, mini_upper);
                              rplacd(cell, (CONSptr) lower_dest);
                            }
                        }
                      else
                        comp_dest =
                          find_pair_state(upper_state, lower_dest,
					  state_mr);
                      
		      lower_array = get_lower_array(upper_array, EPSILON,
                                                    lower, lower_max);

		      comp_labels = lower_array[lower];

                      if (comp_labels == NULL)
                        /* first encounter of the pair */
                        comp_labels =
                          record_labels(lower_eps_labels(lower, lower_other,
							 cons_heap),
                                        lower, lower_array, &LABEL_LIST,
					cons_heap);
                      
#ifdef FULL_FSM
                      add_new_arcs(net, comp_labels, result_state,
                                   comp_dest, NULL);
#else
                      add_new_arcs(net, comp_labels, result_state,
                                   comp_dest);
#endif
                    }
                }
              else if (car(addr_list) != NULL)
                /* Arcs with a label whose upper side has no potential
                   matches on the lower side of UPPER_STATE have an
                   cell containing (NULL) to distinguish them from
                   the arcs with EPSILON.
                   */
                {
                  lower_dest = ARC_destination(lower_arc);
                  
                  do {

                  stack = (STACKptr) car(addr_list);

                  /* Preserve stack pointers so that we can
                     restore them at the end of the loop.
                     */
                    
                  i = STACK_depth(stack);
                  j = STACK_current_blocks(stack);
                  k = STACK_cur_loc(stack);
                  stack_top = STACK_top(stack);

                  while((upper_arc = (ARCptr) pop(stack)))
                    {
                      upper = ARC_label(upper_arc);
                      upper_dest = ARC_destination(upper_arc);
                      comp_dest =
                        find_pair_state(upper_dest, lower_dest,
					state_mr);

		      lower_array = get_lower_array(upper_array, upper, lower,
						    lower_max);

		      comp_labels = lower_array[lower];
                        
                      if (comp_labels == NULL)
                        /* first encounter of the pair */
                        comp_labels =
                          record_labels
                            (compose_labels(upper, lower,
                                            upper_other, lower_other, cons_heap),
                             lower, lower_array, &LABEL_LIST, cons_heap);
                      
#ifdef FULL_FSM
                      add_new_arcs(net, comp_labels, result_state,
                                   comp_dest, NULL);
#else
                      add_new_arcs(net, comp_labels, result_state,
                                   comp_dest);
#endif
                    }

                    /* Restore stack */
                    STACK_depth(stack) = i;
                    STACK_current_blocks(stack) = j;
                    STACK_cur_loc(stack) = k;
                    STACK_top(stack) = stack_top;
                    
                    addr_list = cdr(addr_list);
                  } while (addr_list != NULL);
              }
            }
          
          /* 3rd sweep: cycle through the arcs of UPPER_STATE for the second
             time and do a POP corresponding to each PUSH in the first cycle
             to clear the stacks.
             */
          for (upper_arc = STATE_arc_set(upper_state);
               upper_arc != NULL;
               upper_arc = next_arc(upper_arc))
            {
              stack = upper_map[ARC_label(upper_arc)];
              if (stack != NULL)
                pop(stack);
            }
        }
    }

  if (phantom_net != NULL)
    free_network(phantom_net);
  
  collect_labels(net, LABEL_LIST);
}

static int flags_present_p(NETptr net, int side)
{
  int i;
  ALPHABETptr labs = binary_to_label(NET_labels(net));
  int len = ALPH_len(labs);

    switch (side) {
    case UPPER:
      for (i = 0; i < len; i++)
	if (flag_diacritic_p(upper_id(ALPH_item(labs, i))))
	  return(TRUE);
      break;
    case LOWER:
      for (i = 0; i < len; i++)
	if (flag_diacritic_p(lower_id(ALPH_item(labs, i))))
	  return(TRUE);
      break;
    default:
      handle_error("Bad argument", "FLAGS_PRESENT_P", side);
      break;
    }
  return(FALSE);
}

void flag_test(NETptr upper, NETptr lower)
{
  if (flags_present_p(lower, UPPER) && flags_present_p(upper, LOWER))
    {
      fsm_fprintf(stderr, "*** Warning: It is unsafe to treat flag ");
      fsm_fprintf(stderr, "diacritics as special in\n");
      fsm_fprintf(stderr, "    composition when both networks contain ");
      fsm_fprintf(stderr, "flags. Please set the\n");
      fsm_fprintf(stderr, "    variable compose-flag-as-special to OFF.\n");
    }
}

void free_upper_array(CONSptr **upper_array, ALPHABETptr upper_labels)
{
  /* Frees the vectors allocated in compose_state_pairs and
     finally the array itself.
  */

  int i;
  CONSptr *lower_array;
  
  binary_to_label(upper_labels);

  for (i = 0; i < ALPH_len(upper_labels); i++)
    {
      lower_array = upper_array[ALPH_item(upper_labels, i)];
      
      if (lower_array)
	free((void *) lower_array);
    }
      
  lower_array = upper_array[EPSILON];

  if (lower_array)
    free((void *) lower_array);

  free((void *) upper_array);
}


static NETptr compose_fst_pair(NETptr upper_fst, NETptr lower_fst,
                               int minimize_p, int flags_special_p)
{
  NETptr net;
  ALPHABETptr lower_labels = NET_labels(lower_fst), 
  upper_other = NULL, lower_other = NULL;
  STACKptr *stack_vector, *upper_map;
  STATE_MRptr state_mr;
  CONSptr *lower_map;
  CONSptr **upper_array;
  id_type upper_max, lower_max, upper_low_max, upper_hi_max;
  int other_p;
  
  /* If either one of the incoming networks represents the null
     relation, there is no work to be done because the result
     is guaranteed to be the null relation, too.
     */

  if (null_fsm_p(upper_fst) || null_fsm_p(lower_fst))
    return(null_fsm());


    /* Check n1 and n2 for optimization: */
    if (NET_optimized(upper_fst) || possibly_already_optimized_p(upper_fst))
    {
        handle_warning("Can't compute the composition with an optimized network.\n",
		     "compose_fst_pair", 0);
        fsm_fprintf(stdout, "Un-optimizing ...\n");

        do_unoptimize_arcs(upper_fst);
    }

    if (NET_optimized(lower_fst) || possibly_already_optimized_p(lower_fst))
    {
        handle_warning("Can't compute the composition with an optimized network.\n",
		     "compose_fst_pair", 0);
        fsm_fprintf(stdout, "Un-optimizing ...\n");

        do_unoptimize_arcs(lower_fst);

        fsm_fprintf(stdout, "Composing ...\n");      
    }
  
  /* If flag diacritics are treated as epsilons in the composition,
     they should not be present on both sides of the join.
     */

  if (flags_special_p)
    flag_test(upper_fst, lower_fst);

  other_p =  scan_labels(upper_fst, lower_fst, &upper_max, 
                         &upper_low_max, &upper_hi_max, &upper_other);
  other_p += scan_labels(lower_fst, upper_fst, &lower_max, NULL, NULL,
                         &lower_other);  
  net = make_network();
  NET_sigma(net) = sigma_union(NET_sigma(upper_fst),
                               NET_sigma(lower_fst));
  
  /* ARC_STACKS has a stack for each symbol in the lower alphabet
     of UPPER_FST.  We use it to sort the arcs of the upper state
     in PROCESS_STATE_PAIRS.  UPPER_MAP maps each arc label of UPPER_FST
     to the aprropriate stack for storing it.
     
     LOWER_MAP maps each label of LOWER_FST to a list of stacks to
     which matching arcs of UPPER_FST are stored.  The value is a
     list of stacks because the upper side of a lower arc label
     might be OTHER or a symbol that matches itself and OTHER.
     
     */
  
  if (NET_num_states(lower_fst) > NET_num_states(upper_fst))
    state_mr = init_state_manager(upper_fst, lower_fst, net, lower_fst);
  else
    state_mr = init_state_manager(upper_fst, lower_fst, net, upper_fst);
  
  compose_make_arc_stacks(upper_fst, upper_max, upper_low_max,
                          &stack_vector, &upper_map, flags_special_p);
  
  compose_make_lower_map(lower_max, upper_low_max, lower_labels,
                         upper_other, lower_other, stack_vector,
                         state_mr, &lower_map, flags_special_p);
  
  upper_array = (CONSptr **)calloc((size_t) (upper_max+1),
				   (size_t) sizeof(CONSptr *));
  if (!upper_array)
    not_enough_memory("COMPOSE_FST");

  NET_start_state(net) =
    find_pair_state(NET_start_state(upper_fst), NET_start_state(lower_fst),
		    state_mr);
  NET_deterministic(net)        = TRUE;
  NET_epsilon_free(net)         = TRUE;
  
  compose_state_pairs(net, upper_map, lower_map, upper_array, lower_max,
                      upper_other, lower_other, state_mr, flags_special_p);
  
  free_arc_stacks(stack_vector, upper_low_max);
  free((void *) upper_map);
  
  free((void *) stack_vector);
  free((void *) lower_map);
  free_state_manager(state_mr);
  free_alph(upper_other);
  free_alph(lower_other);

  free_upper_array(upper_array, NET_labels(upper_fst));
  
  /*    Always prune to throw out dead paths. Need to do this
        before determinizing and minimizing to guarantee that
        the label set is correct.
        */
  
  prune_fsm(net, DONT_COPY);      
  
  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);
  
  return(net);
}

static void compose_sequentially(NVptr to_do, int reclaim_p, int minimize_p,
                                 int cleanup_p, int flags_special_p)
{
  /* Composes a sequence of networks left to right in one pass,
     always replacing the first net with the result.
     */ 
 
  NETptr net, first = NV_net(to_do, 0), second;
  int i, count = NV_len(to_do);

  clear_client_cells(first);
  
  for (i = 1; i < count; i++)
    {
      second = NV_net(to_do, i);
      clear_client_cells(second);

      net = compose_fst_pair(first, second, minimize_p, flags_special_p);
      
      /* If the arguments are being reclaimed,  we must keep track of
	 how many times they appear on the to_do list so that
	 we do not reclaim themn too early
	 */

      if (reclaim_p == DO_RECLAIM)
        {
          if ((int) NET_client_cell(first) <= 1)
	    free_network(first);
	  else
	    NET_client_cell(first) = (void *)
	      ((int) NET_client_cell(first) - 1);            
                
	  if ((int) NET_client_cell(second) <= 1)
	    free_network(second);
	  else
	    NET_client_cell(second) = (void *)
	      ((int) NET_client_cell(second) - 1);
        }

      if (cleanup_p == DO_CLEANUP)
        net = cleanup_fst(net, DONT_COPY, minimize_p);
       first  = net;
    }

  NV_net(to_do, 0) = first;
}

static void compose_pairwise(NVptr to_do, int reclaim_p, int minimize_p,
                             int cleanup_p, int flags_special_p)
{
  /* Composes a sequence of networks by successive traversals through the list.
     On each iteration, a net is composed with its nearest neighbor.
     */ 
 
  NETptr first, second, net, *temp, *comp_results, *next_result;
  int i, count = NV_len(to_do), first_round_p = TRUE;
 
  comp_results = (NETptr *) malloc((size_t) count * (size_t) sizeof(NETptr));
  
  do {
    /* Starting a new round from the beginning of remaining NETS. At least
       two nets are still left.
       */
    i = 0;
    next_result = comp_results;
    
    do {
      /* Do pairwise compositions until there is no SECOND.
       */
      first = NV_net(to_do, i++);
      
      if (i < NV_len(to_do))
        {
          second = NV_net(to_do, i++);
          count--;
          
          /* On subsequent rounds client cells are known to be clear */
          if (first_round_p)
            {
              clear_client_cells(first);
              clear_client_cells(second);
            }
          
          net = compose_fst_pair(first, second, minimize_p, flags_special_p);
  
          if (cleanup_p == DO_CLEANUP)
            net = cleanup_fst(net, DONT_COPY, minimize_p);

          *next_result++ = net;         
          
          /* If the arguments are being reclaimed, on the first round
             we must keep track of how many times they appear on the
             to_do list.
             */
          
          if (reclaim_p == DO_RECLAIM)
	    {
	      if (first_round_p)
		{
		  if ((int) NET_client_cell(first) == 1)
		    free_network(first);
		  else
		    NET_client_cell(first) = (void *)
		      ((int) NET_client_cell(first) - 1);
                
                
		  if ((int) NET_client_cell(second) == 1)
		    free_network(second);
		  else
		    NET_client_cell(second) = (void *)
		      ((int) NET_client_cell(second) - 1);
		}
	      else
		{
		  free_network(first);
		  free_network(second);
		}
	    }
        }
      else
        {
          /* No SECOND, FIRST will be taken up on the next round. */
          if (first_round_p)
            clear_client_cells(first);
          
          *next_result = first;
        }
    } while (i < NV_len(to_do));
    
    /* nets composed in previous round become to_do in next round */
    
    temp = NV_nets(to_do);
    NV_nets(to_do) = comp_results;
    NV_len(to_do) = count;
    comp_results = temp;
    
    first_round_p = FALSE;
  } while (count > 1);
  
  free((void *) comp_results);
}

NETptr compose_fst(NVptr nets, int reclaim_p, int minimize_p, int cleanup_p,
	int sequentially_p, int flags_special_p)
{
  /* 
    Composes a series of netwporks to a single transducer. Equivalent to
    intersection, if all networks are simple finite-state machines rather
    than transducers.  If only one argument is given, it becomes the the
    returned value. If RECLAIM_P is DO_RECLAIM, the arguments (but not the
    NV pointer containing them) are freed.
    If the argument is NULL, returns the identity, SIGMA_STAR_FSM.
    If any of the arguments or intermediate results is a NULL, returns
    a null fsm.
    IF FLAGS_SPECIAL_P is FLAGS_SPECIAL (=TRUE), flag diacriticts are
    treated as EPSILONS.
    */

  NVptr to_do;
  NETptr net;
  int count, i;
  
  if (nets == NULL || NV_len(nets) == 0)
    return (sigma_star_fsm());
  else if (NV_len(nets) == 1)
    {
      net = NV_net(nets, 0);
      if (minimize_p == DO_MINIMIZE)
        min_fsm(net, DONT_COPY);
      return(net);
    }
  
  count = NV_len(nets);
  
  to_do = make_nv(count);
  
  /* Make sure NET client cells are all clean. */
  
  for (i = 0; i < count; i++)
    {
      net = NV_net(to_do, i) = NV_net(nets, i);
      NET_client_cell(net) = (void *) 0;   
    }
  
  
  /* Client cells are used to keep count of duplicate occurrences of the
     same net.  Must be careful not to reclaim too soon.
     */
  
  for (i = 0; i < count; i++)
    {
      net = NV_net(to_do, i) = NV_net(nets, i);
      epsilon_remove(net, DONT_COPY);
      NET_client_cell(net) = (void *) ((int) NET_client_cell(net) + 1);
    }
  
  
  /* we only need to create & destroy this
	stack once, since it is emptied after
	each pairwise composition -- TSY 6/91 */
  
  if (sequentially_p == SEQUENTIALLY)
    compose_sequentially(to_do, reclaim_p, minimize_p, cleanup_p,
			 flags_special_p);
  else
    compose_pairwise(to_do, reclaim_p, minimize_p, cleanup_p, flags_special_p);
 
  net = NV_net(to_do, 0);

  free_nv_only(to_do);
 
  check_for_flags(net);
 
  return(net);
}

#ifdef NEW_INTERSECT_FSM_IS_BROKEN

new_intersect_fsm is a minor modification of what used to be
intersect_fsm (and is now old_intersect_fsm).
If some problem arises with it, the old version is here
and can be pressed into service.  -Max Copperman, 8/4/97

NETptr old_intersect_fsm(NVptr nets, int reclaim_p, int minimize_p)
{
  /*****
   *
   INTERSECTFSM intersects an arbitrary number of FSMs.
   Minimizes, unless minimize_p is DONT_MINIMIZE.
   Frees argument nets unless RECLAIM_P is DONT_RECLAIM.
   The identity for this operation is SIGMA*,
   which is returned if NETS is NULL.
   
   The original nets provided to this function are not altered,
   but they are reclaimed depending on reclaim_p.
   *
   *****/
  
  NVptr min_nets;
  NETptr net1, net2, result = NULL;
  int net_count;
  
  if (nets == NULL || NV_len(nets) == 0)        /* where there are no nets */
    return (sigma_star_fsm());
  
  min_nets = remove_duplicates(nets);
  net_count = NV_len(min_nets);
  
  if (net_count == 1)   /* where there is only one net */
    {
      
      if (reclaim_p == DO_RECLAIM)
        {
          result = copy_fsm(NV_net(min_nets,0));
          free_network(NV_net(min_nets, 0));
        }
      else
        result = NV_net(min_nets,0);
      
      if (minimize_p == DONT_MINIMIZE)
        return (result);
      else
        return (min_fsm(result, DONT_COPY));
    }
  
  /* At least two nets. There is work to be done. */
  
  if (net_count == 2)
    {
      /* Simple case.  Just return the result of PAIR_INTERSECT_FSM. */
      
      net1 = NV_net(min_nets, 0);
      net2 = NV_net(min_nets, 1);
      
      /* EPSILON_REMOVE, if done, also clears client cells. */
      
      if (NET_epsilon_free(net1))
        clear_client_cells(net1);
      else
        epsilon_remove(net1, DONT_COPY);
      
      if (NET_epsilon_free(net2))
        clear_client_cells(net2);
      else
        epsilon_remove(net2, DONT_COPY);
      
      result = pair_intersect_fsm(net1, net2, minimize_p);
      
      if (reclaim_p == DO_RECLAIM)
        {
          free_network(net1);
          free_network(net2);
        }
    }
  else
    {
      /* do pairwise intersection until only one net remains */
      
      NETptr *net_vector = (NETptr *)
        malloc((size_t) net_count * (size_t) sizeof(NETptr));
      NETptr *nets_to_do = net_vector;
      int i, j;
      
      /* Make a working array of nets.  Do epsilon-removal to prepare
         for intersection and set net client cell to 0 so that we can
         recognize original nets from the intermediate results.
         */
      
      for (i = 0; i < net_count; i++)
        {
          net1 = epsilon_remove(NV_net(min_nets, i), DONT_COPY);
          nets_to_do[i] = net1;
          NET_client_cell(net1) = (void *) 0;
        }
      
      sort_nets(nets_to_do, net_count);
      
      while (net_count > 1)
        {
          /* There are at least two nets at this point.  Each iteration
           * of this loop reduces the number of nets by one.  Quit
           * processing when only one network remains.  At each stage,
           * intersect the two nets with the smallest number of states
           * and place the result on the NETS_TO_DO list according to
           * its size.
           *
           * PAIR_INTERSECT_FSM creates a new network without smashing
           * the arguments.  No need to copy.
           */
          
          net1 = *nets_to_do++;
          net2 = *nets_to_do;
          net_count--;
          
          /* If client mark is 1, the net is an intermediate result
             coming from PAIR_INTERSECT_FSM and its state client
             cells are known to be clean.  If the mark is 0, the net
             is an original net.
             */
          
          if ((int) NET_client_cell(net1) == 0)
            clear_client_cells(net1);
          if ((int) NET_client_cell(net2) == 0)
            clear_client_cells(net2);
          
          result = pair_intersect_fsm(net1, net2, minimize_p);
          /* Mark the intermediate result. */
          NET_client_cell(result) = (void *) 1;
          
          /* If two nets remain, place the result in the first position
             because with two nets, the order doesn't matter.  If there
             are more than two nets remaining, move smaller nets ahead
             of the result.  If NET_COUNT is one, we are done.
             */
          
          if (net_count == 2)
            *nets_to_do = result;
          else if (net_count > 2)
            {
              for (i = 0, j = 1; j < net_count; i++, j++)
                if (netcmp(result, nets_to_do[j]) < 1)
                  {
                    nets_to_do[i] = result;
                    break;
                  }
                else
                  nets_to_do[i] = nets_to_do[j];
              
              /* All other remaining nets are smaller than RESULT.
                 Place it in the last position.
                 */
              
              if (j == net_count)
                nets_to_do[i] = result;
            }
          
          /* Original nets (client mark 0) are kept, intermediate
             results (client mark 1) are reclaimed. */
          
          
          if (reclaim_p == DO_RECLAIM || (int) NET_client_cell(net1) == 1)
            free_network(net1);
          
          if (reclaim_p == DO_RECLAIM || (int) NET_client_cell(net2) == 1)
            free_network(net2);
        }
      
      NET_client_cell(result) = NULL;
      free((void *) net_vector);
    }
  
  
  /* If MIN_NETS is not the same as NETS, it was created by
     REMOVE_DUPLICATES above and can now be reclaimed.
     */
  
  if (min_nets != nets) 
    free_nv_only(min_nets);
  
  return (result);
}
#endif /* NEW_INTERSECT_FSM_IS_BROKEN */

int equal_length_relation_p(NETptr net)
{
  /* Return TRUE if the network appears to represent an
     equal length relation. This is an imperfect test at
     present because a network could have this property
     even if there are one-sided epsilons in the label
     set. */

  int i;
  id_type id;
	
  ALPHABETptr alph = binary_to_label(NET_labels(net));
	
  for (i = 0; i < ALPH_len(alph); i++)
    {
      id = ALPH_item(alph, i);
      if (tuple_id_p(id) && (id_to_upper_id(id) == EPSILON ||
			     id_to_lower_id(id) == EPSILON))
	return(FALSE);
    }	
  return(TRUE);
}

static void not_equal_length_p_warning(void)
{
  fsm_fprintf(stderr, "*** Warning: Only regular languages and equal \
length relations are closed\n");
  fsm_fprintf(stderr, "    under intersection. This network appears to \
represent a relation that\n");
  fsm_fprintf(stderr, "    includes pairs of different lengths. \
The result may be incorrect.\n");
}

NETptr new_intersect_fsm(NVptr nets, int reclaim_p, int minimize_p,
                       DEST2ptr (*make_dest_array)(NETptr, NETptr),
                       DEST2ptr (*index_dest_array)(ARCptr, DEST2ptr, void **),
                       id_type (*get_new_label)(ARCptr, void *)
)
{
  /*****
   *
   INTERSECTFSM intersects an arbitrary number of FSMs.
   Minimizes, unless minimize_p is DONT_MINIMIZE.
   Frees argument nets unless RECLAIM_P is DONT_RECLAIM.
   The identity for this operation is SIGMA*,
   which is returned if NETS is NULL.
   
   The original nets provided to this function are not altered,
   but they are reclaimed depending on reclaim_p.
   *
   *****/
  
  NVptr min_nets;
  NETptr net1, net2, result = NULL;
  /*  static unsigned intersections = 0;    Debugging variable? */
  int i, net_count;
  
  if (nets == NULL || NV_len(nets) == 0)        /* where there are no nets */
    return (sigma_star_fsm());
  
  min_nets = remove_duplicates(nets);
  net_count = NV_len(min_nets);
  
  if (net_count == 1)   /* where there is only one net */
    {
      
      if (reclaim_p == DO_RECLAIM)
        {
          result = copy_fsm(NV_net(min_nets,0));
          free_network(NV_net(min_nets, 0));
        }
      else
        result = NV_net(min_nets,0);
      
      if (minimize_p == DONT_MINIMIZE)
        return (result);
      else
        return (min_fsm(result, DONT_COPY));
    }
  
  /* At least two nets. There is work to be done. */
  
  /* Make sure that the nets represent simple automata
     or equal length relations if we are not in TWOL_MODE. */
     
  if (FSM_MODE == STANDARD_MODE)
    for (i = 0; i < net_count; i++)
      if (NET_arc_label_arity(NV_net(min_nets, i)) > 1 &&
	  !equal_length_relation_p(NV_net(min_nets, i)))
	not_equal_length_p_warning();


  /* Check nets for optimization: */
  for (i = 0; i < net_count; i++)
  {
    if (NET_optimized(NV_net(min_nets, i))
        || possibly_already_optimized_p(NV_net(min_nets, i)))
    {
        handle_warning("Can't intersect with an optimized network.\n",
		     "new_intersect_fsm", 0);
        fsm_fprintf(stdout, "Un-optimizing ...\n");

        do_unoptimize_arcs(NV_net(min_nets, i));
    }
  }
  
  if (net_count == 2)
    {
      /* Simple case.  Just return the result of PAIR_INTERSECT_FSM. */
      
      net1 = NV_net(min_nets, 0);
      net2 = NV_net(min_nets, 1);
      
      /* EPSILON_REMOVE, if done, also clears client cells. */
      
      if (NET_epsilon_free(net1))
        clear_client_cells(net1);
      else
        epsilon_remove(net1, DONT_COPY);
      
      if (NET_epsilon_free(net2))
        clear_client_cells(net2);
      else
        epsilon_remove(net2, DONT_COPY);
      
      result = new_pair_intersect_fsm(net1, net2, minimize_p,
				      make_dest_array, index_dest_array,
				      get_new_label);
      
      if (reclaim_p == DO_RECLAIM)
        {
          free_network(net1);
          free_network(net2);
        }
    }
  else
    {
      /* do pairwise intersection until only one net remains */
      
      NETptr *net_vector = (NETptr *)
        malloc((size_t) net_count * (size_t) sizeof(NETptr));
      NETptr *nets_to_do = net_vector;
      int i, j;
      
      /* Make a working array of nets.  Do epsilon-removal to prepare
         for intersection and set net client cell to 0 so that we can
         recognize original nets from the intermediate results.
         */
      
      for (i = 0; i < net_count; i++)
        {
          net1 = epsilon_remove(NV_net(min_nets, i), DONT_COPY);
          nets_to_do[i] = net1;
          NET_client_cell(net1) = (void *) 0;
        }
      
      sort_nets(nets_to_do, net_count);
      
      while (net_count > 1)
        {
          /* There are at least two nets at this point.  Each iteration
           * of this loop reduces the number of nets by one.  Quit
           * processing when only one network remains.  At each stage,
           * intersect the two nets with the smallest number of states
           * and place the result on the NETS_TO_DO list according to
           * its size.
           *
           * PAIR_INTERSECT_FSM creates a new network without smashing
           * the arguments.  No need to copy.
           */
          
          net1 = *nets_to_do++;
          net2 = *nets_to_do;
          net_count--;
          
          /* If client mark is 1, the net is an intermediate result
             coming from PAIR_INTERSECT_FSM and its state client
             cells are known to be clean.  If the mark is 0, the net
             is an original net.
             */
          
          if ((int) NET_client_cell(net1) == 0)
            clear_client_cells(net1);
          if ((int) NET_client_cell(net2) == 0)
            clear_client_cells(net2);
          
	  result = new_pair_intersect_fsm(net1, net2, minimize_p,
					  make_dest_array,  index_dest_array, get_new_label);
 
          /* Mark the intermediate result. */
          NET_client_cell(result) = (void *) 1;
          
          /* If two nets remain, place the result in the first position
             because with two nets, the order doesn't matter.  If there
             are more than two nets remaining, move smaller nets ahead
             of the result.  If NET_COUNT is one, we are done.
             */
          
          if (net_count == 2)
            *nets_to_do = result;
          else if (net_count > 2)
            {
              for (i = 0, j = 1; j < net_count; i++, j++)
                if (netcmp(result, nets_to_do[j]) < 1)
                  {
                    nets_to_do[i] = result;
                    break;
                  }
                else
                  nets_to_do[i] = nets_to_do[j];
              
              /* All other remaining nets are smaller than RESULT.
                 Place it in the last position.
                 */
              
              if (j == net_count)
                nets_to_do[i] = result;
            }
          
          /* Original nets (client mark 0) are kept, intermediate
             results (client mark 1) are reclaimed. */
          
          
          if (reclaim_p == DO_RECLAIM || (int) NET_client_cell(net1) == 1)
            free_network(net1);
          
          if (reclaim_p == DO_RECLAIM || (int) NET_client_cell(net2) == 1)
            free_network(net2);
        }
      
      NET_client_cell(result) = NULL;
      free((void *) net_vector);
    }
  
  /* If MIN_NETS is not the same as NETS, it was created by
     REMOVE_DUPLICATES above and can now be reclaimed.
     */
  
  if (min_nets != nets) 
    free_nv_only(min_nets);

  check_for_flags(result);
 
  return (result);
}

NETptr intersect_fsm(NVptr nets, int reclaim_p, int minimize_p)
{
  return(new_intersect_fsm(nets, reclaim_p, minimize_p, make_dest_array,
			   index_dest_array, get_new_label));
}



/********************
 *                  *
 *   MINUS          *
 *                  *
 ********************/

NETptr old_minus_fsm(NETptr net1, NETptr net2, int copy_p, int minimize_p)
{
  NVtype nv;
  NETptr net, nets[2];
  
  nv.len = 2;
  nv.nets = nets;
  
  align_sigmas(net1, net2);
  
  /* First take care of some easy cases. 
   */
  
  if (null_fsm_p(net2))
    {
      if (copy_p)
	return(copy_fsm(net1));
      else
	{
	  free_network(net2);
	  return(net1);
	}
    }
  else if (equiv_fsm_p(net1, net2))
    {
      if (copy_p)
	return(null_fsm());
      else
	{
	  free_network(net1);
	  free_network(net2);
	  return(null_fsm());
	}
    }
  
  nets[0] = net1;
  /* If NET2 is not going to get OTHER added to its alphabet,
     we need to make sure that all of the symbols in NET1
     are included in the sigma of NET2.
     */
  if (NET_closed_sigma(net2))
    sigma_union_dont_copy(sigma(net2), sigma(net1));
  
  nets[1] = negate_net(net2, copy_p, DO_MINIMIZE);
  
  net = intersect_fsm(&nv, !copy_p, minimize_p);
  
  if (NET_closed_sigma(net1))
    NET_closed_sigma(net) = TRUE;
  
  free_network(nets[0]);
  free_network(nets[1]);
  return(net);
}


static void make_start_state_non_final(NETptr net)
{
  /* Make the start state non-final. If there are arcs leading back to it,
     we need to reroute them to a new state that has the same outgoing arcs
     as the start state.
     */
  
  ARCptr arc;
  STATEptr state, start_state = NET_start_state(net), new_state = NULL;

  for (state = NET_states(net); state; state = next_state(state))
    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
      if (ARC_destination(arc) == start_state)
        {
          if (!new_state)
             {
                /* Make a new state which is isomorphic to the start state.
                   That is, the new state contains a copy of each arc that 
                   leads away from the start state and a looping arc to the 
                   news state for each looping arc to the start state.
                   */
                   
                ARCptr arc2, new_arc;
                
                new_state = make_state(NULL, net);
                STATE_final(new_state) = STATE_final(start_state);
                STATE_deterministic(new_state) =
                        STATE_deterministic(start_state);
                
                for (arc2 = STATE_arc_set(start_state); arc2;
                     arc2 = next_arc(arc2))
                  {
                    new_arc = copy_arc(arc2, net);
                    
                    if (ARC_destination(new_arc) == start_state)
                      ARC_destination(new_arc) = new_state;
                      
                    add_arc(new_arc, new_state, net);
                  }
             }
             
          ARC_destination(arc) = new_state;
        }

  STATE_final(start_state) = FALSE;
  NET_minimized(net) = FALSE;
}

static void reduce_to_null_fsm(NETptr net)
{
  /* Reduces net to a null fsm. */

  STATEptr state, next_s, start_state = NET_start_state(net);
  int32 i;
  ALPHABETptr sig = sigma(net);

  for (state = NET_states(net); state; state = next_s)
    {
      next_s = next_state(state);
      next_state(state) = NULL;
      
      if (state != start_state)
	free_state_and_arcs(state, net);
    }
	  
  free_arc_set(start_state, net);
  STATE_final(start_state) = FALSE;

  NET_states(net) = start_state;

  binary_to_label(NET_labels(net));
  ALPH_len(NET_labels(net)) = 0;

  label_to_binary(sig);
  for (i = ALPH_len(sig) - 1; i >= 0; i--)
    ALPH_item(sig, i) = 0;
  ALPH_len(sig) = 0;

  NET_epsilon_free(net) = TRUE;
  NET_deterministic(net) = TRUE;
  NET_minimized(net) = TRUE;
}


NETptr minus_fsm(NETptr n1, NETptr n2, int reclaim_n1_p, int reclaim_n2_p,
                 int minimize_p)
{
  /* Computes the intersection of N1 and the complement of N2 but is
     more efficient because it does completion only with respect to the
     transitions of N1.  Only makes sense for a certain subclass of
     transducers.  If RECLAIM_N1_P is TRUE, N1 is smashed, otherwise
     N1 is not affected.  If SHASH_N2_P is true, N2 is reclaimed and freed,
     otherwise it is unaffected except that N1 and N2 sigmas get aligned.
     */
  
  NETptr net;
  STATEptr state, dest_state, old1, old2, fail2;
  /* STATEtype fail2;*/
  STACKptr arc_stack;
  STATE_MRptr state_mr;
  HEAPptr cons_heap;
  DESTptr dest_array;
  ARCptr a;
  int max, n2_index_p;
  DESTptr dest;
  CONSptr cur;
  int easy_p = FALSE;

  /* First take care of some easy cases. 
   */
  
  if (null_fsm_p(n2))
    {
      /* N2 is NULL, no change in N1 unless we need to copy or minimize it. */

      easy_p = TRUE;

      if (reclaim_n1_p == DONT_RECLAIM)
	n1 = copy_fsm(n1);
    }
  
  if (epsilon_fsm_p(n2) &&
      !(STATE_final(NET_start_state(n1)) &&
	has_arc_p(NET_start_state(n1), EPSILON)))
    {
      /* N2 is an epsilon fsm and either the start state is non-final or has no
	 outgoing epsilon arcs.
	 */

      easy_p = TRUE;

     if (reclaim_n1_p == DONT_RECLAIM)
	n1 = copy_fsm(n1);
      
      if (STATE_final(NET_start_state(n1)))
	make_start_state_non_final(n1);
    }

  if (sigma_star_fsm_p(n2))
    {
      /* N2 is sigma star fsm. N1 reduces to a null fsm. */

      easy_p = TRUE;

      if (reclaim_n1_p == DONT_RECLAIM)
	n1 = null_fsm();
      else
	reduce_to_null_fsm(n1);
    }

  if (sigma_star_fsm_p(n1))
    {
      /* Swap n1 and n2, we'll return n2's complement */

      easy_p = TRUE;
      net = negate_net(n2, reclaim_n2_p, DONT_MINIMIZE);
      n2 = n1;
      reclaim_n2_p = reclaim_n1_p;
      n1 = net;
    }

  if (easy_p)
    {
      if (reclaim_n2_p == DO_RECLAIM)
        free_network(n2);

      if (minimize_p == DO_MINIMIZE)
        return(min_fsm(n1, DONT_COPY));
      else
	return(n1);
    }

    /* Check n1 and n2 for optimization: */
    if (NET_optimized(n1) || possibly_already_optimized_p(n1))
    {
        handle_warning("Can't run the minus operator with an optimized network.\n",
		     "minus_fsm", 0);
        fsm_fprintf(stdout, "Un-optimizing ...\n");

        do_unoptimize_arcs(n1);
    }

    if (NET_optimized(n2) || possibly_already_optimized_p(n2))
    {
        handle_warning("Can't run the minus operator with an optimized network.\n",
		     "minus_fsm", 0);
        fsm_fprintf(stdout, "Un-optimizing ...\n");

        do_unoptimize_arcs(n2);
    }
  
  /* Make sure that N2 is deterministic, otherwise the algorithm
     is not guaranteed to produce the right result. */
  
  determinize_fsm(n2, DONT_COPY);

  /* Tempted to add this line because it appears a nondeterministic
     N1 can result in a segmantation, so the result passed back
     must be in some way incorrect. I don't really understand the
     problem. L.K. Jan. 30, 1997.
  
     determinize_fsm(n1, DONT_COPY);
  */
  
  net = make_network();

  /* Make sure that N1 is epsilon free.  If N1 is going to be smashed, state
     client cells of the larger net are used for a binary tree that records
     the corresponding states in the smaller net.  If N1 is not smashed, we
     its states are used for indexing even if N2 is bigger to avoid using
     the phantom FAIL2 state to index presumably numerous N1 states that
     match it.
     */
  
  epsilon_remove(n1, DONT_COPY);

  if (NET_num_states(n2) > NET_num_states(n1))
    {
      state_mr = init_state_manager(n1, n2, net, n2);
      n2_index_p = TRUE;
    }
  else
    {
      state_mr = init_state_manager(n1, n2, net, n1);
      n2_index_p = FALSE;
    }
  
  cons_heap = STATE_MR_cons_heap(state_mr);

  align_sigmas(n1, n2);
  
  max = pair_intersect_preprocess(n1, n2);
  
  /* initialize memory */
  dest_array = (DESTptr) calloc((size_t) max + 1, (size_t) sizeof(DESTtype));
  fail2 = alloc_state();

  if (dest_array == NULL)
    not_enough_memory("MINUS_FSM");
  
  arc_stack = create_stack("Minus arcs to do");

  NET_epsilon_free(net) = TRUE;
  NET_loop_free(net) = NET_loop_free(n1);
  NET_deterministic(net) = NET_deterministic(n1);
  
  NET_start_state(net) =
    find_pair_state(NET_start_state(n1), NET_start_state(n2), state_mr);
  
  while((state = next_new_state(state_mr)))
    {
      cur = (CONSptr) STATE_client_cell(state);
      old1 = (STATEptr) car(cur);
      old2 = (STATEptr) cdr(cur);
      
      assert(old1);
      assert(old2);

      free_cons_cell(cur, cons_heap);
      STATE_client_cell(state) = NULL;
      
      STATE_final(state) = STATE_final(old1) && !STATE_final(old2);
      STATE_deterministic(state) = STATE_deterministic(old1);
      
      if (STATE_arc_set(old1) != NULL && STATE_arc_set(old2) != NULL)
        { 
          
          /* 1st sweep:  load the array with destinations of arcs from N1
           */
          
          a = STATE_arc_set(old1);
          
          do {
            dest = &dest_array[ARC_label(a)];
            switch (DEST_type(dest))
              {
                case(0):  /* dest is NULL */
                  DEST_state(dest) = ARC_destination(a);
                DEST_type(dest) = 1;
                DEST_mark(dest) = 0;
                break;
                case(1): /* dest currently contains one item -- make a list */
                  DEST_cons(dest) = cons_cell(ARC_destination(a),
					      cons_cell(DEST_state(dest), NULL,
							cons_heap),
					      cons_heap);
                DEST_type(dest) = 2;
                break;
                case(2): /* dest contains a list of items */
                  DEST_cons(dest) = cons_cell(ARC_destination(a), DEST_cons(dest),
					      cons_heap);
                break;
              }
            a = next_arc(a);
          } while (a != NULL);
          
          /* 2nd sweep:  scan for matching arcs in N2 */
          
          a = STATE_arc_set(old2);
          
          do { 
            dest = &dest_array[ARC_label(a)];
            
            switch (DEST_type(dest))
              { 
              case (0):  /* there is no match */
                break;
              case (1): /* match has only one destination */
                /* Increase the counter in the DEST cell to
                   indicate that OLD1 and OLD2 had matching arcs
                   */
                DEST_mark(dest)++;
                
                dest_state =
                  find_pair_state(DEST_state(dest), ARC_destination(a), state_mr);
                
                push_arc(make_arc(ARC_label(a), dest_state, net), state);
                break;
              case (2): /* match has several destinations */
                /* Set DEST mark to 1 to indicate that OLD1 and
                   old 2 had matching arcs.
                   */
                DEST_mark(dest) = 1;
                for (cur = DEST_cons(dest); cur != NULL; cur = cdr(cur))
                  {
                    dest_state =
                      find_pair_state(car(cur), ARC_destination(a), state_mr);

                    push_arc(make_arc(ARC_label(a), dest_state, net), state);
                  }
                break;
              }
            a = next_arc(a);
          } while (a != NULL);
          
          /* 3rd sweep:  clear array by sweeping the arcs of a state
             from N1 */
          
          a = STATE_arc_set(old1);
          
          do {
            dest = &dest_array[ARC_label(a)];
            switch (DEST_type(dest))
              {
              case (1):
                /* If the visit mark is 0, no matching arc in OLD2
                   was found. If it is more than 1 the state and
                   the net is non-deterministic.
                   */
                if (DEST_mark(dest) == 0)
                  {
                    if (reclaim_n1_p == DO_RECLAIM)
                      {
                        dest_state = DEST_state(dest);
                        mark_reachable_states(dest_state, 1, arc_stack);
                      }
                    else
                      dest_state =
                        find_pair_state(DEST_state(dest), fail2, state_mr);
                    
                    push_arc(make_arc(ARC_label(a), dest_state, net), state);
                  }
                else if (DEST_mark(dest) > 1)
                  /* Should not happen because N2 was determinized. */
                  handle_error("N2 not deterministic", "MINUS_FSM",
                               DEST_mark(dest));
                DEST_state(dest) = NULL;
                break;
              case (2):
                
                /* release cons cells for future use */
                for (cur = DEST_cons(dest);
                     cur != NULL;
                     cur = freeing_CDR_cell(cur, cons_heap))
                  if (DEST_mark(dest) == 0)
                    {
                      if (reclaim_n1_p == DO_RECLAIM)
                        {
                          dest_state = car(cur);
                          mark_reachable_states(dest_state, 1, arc_stack);
                        }
                      else
                        dest_state = find_pair_state(car(cur), fail2, state_mr);
                      
                      push_arc(make_arc(ARC_label(a), dest_state, net),
                               state); 
                    }
                
                STATE_deterministic(state) = FALSE;
                NET_deterministic(net) = FALSE;
                DEST_cons(dest) = NULL;
                break;
              }
            DEST_mark(dest) = 0;           
            DEST_type(dest) = 0;
            a = next_arc(a);
          } while (a != NULL);
        }
      else
        for (a = STATE_arc_set(old1); a != NULL; a = next_arc(a))
          {
            /* No matching arcs in OLD2 because OLD2 is empty.
               If N1 is being smashed, just copy the arc to the
               new net and mark the subnet reachable from its
               destination. If N1 is not being smashed, pair
               the destination with the phantom failure state
               in N2.  This will have the effect of copying the
               subnet starting from the destination into the
               result.
               */
            if (reclaim_n1_p == DO_RECLAIM)
              {
                dest_state = ARC_destination(a);
                mark_reachable_states(dest_state, 1, arc_stack);
              }
            else
              dest_state = find_pair_state(ARC_destination(a), fail2, state_mr);
            
            push_arc(make_arc(ARC_label(a), dest_state, net), state);
          }
    }
  
  /* release memory */
  
  free_state(fail2);
  free((void *) dest_array);
  free_state_manager(state_mr);
  free_stack(arc_stack);
  
  if (reclaim_n1_p == DO_RECLAIM)
    {
      /* Prune useless states */
      
      NET_states(n1) = delete_states(n1, state_mark_one_p);
      
      /* Move the remaining N1 states to the new net. */
      
      if (NET_states(n1) != NULL)
        {
          for (state = NET_states(n1);
               next_state(state) != NULL;
               state = next_state(state))
            {
              STATE_visit_mark(state) = 0;
              STATE_client_cell(state) = NULL;
            }
          
          /* STATE is now the last state in N1's state list. */
          
          STATE_visit_mark(state) = 0;
          STATE_client_cell(state) = NULL;
          
          next_state(state) = NET_states(net);
          NET_states(net) = NET_states(n1);
          
          NET_num_states(net) += NET_num_states(n1);
          NET_num_arcs(net) += NET_num_arcs(n1);
          
          sever_states(n1);
        }
      
      NET_sigma(net) = NET_sigma(n1);
      NET_sigma(n1) = NULL;
      
      NET_labels(net) = NET_labels(n1);
      NET_labels(n1) = NULL;
      free_network(n1);
    }
  else
    {
      if (!n2_index_p)
        clear_client_cells(n1);
      
      NET_sigma(net) = copy_alphabet(NET_sigma(n1));
      NET_labels(net) = copy_alphabet(NET_labels(n1));
    }
  
  
  if (reclaim_n2_p == DO_RECLAIM)
    free_network(n2);
  else if (n2_index_p)
    clear_client_cells(n2);
  
  /* Always prune to throw out dead paths. Need to do this
     before determinizing and minimizing to guarantee that
     the label set is correct.  
     */
  
  prune_fsm(net, DONT_COPY);
  
  /* Arity is initially set to 1. */
  
  set_net_arity(net);
  
  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);
  
  /* If it is clear that we produced a null fsm or an epsilon fsm
     eliminate the remaining symbols from the sigma. The label
     alphabet should then be empty as well.
     */

  if (NET_num_arcs(net) == 0)
    {
      ALPH_len(NET_sigma(net)) = 0;

      if (ALPH_len(NET_labels(net)) != 0)
        handle_error("Label alphabet not correctly maintained", "MIN_FSM",
                     ALPH_len(NET_labels(net)));
    }
  check_for_flags(net);
  return(net);
}

NETptr oneside_minus_fsm
  (NETptr n1, NETptr n2, int side,
   int reclaim_n1_p, int reclaim_n2_p, int minimize_p)
{
  /*  [ n1 .-u. n2 ] == [ [n1.u - n2.u] .o. n1 ]
      [ n1 .-l. n2 ] == [ n1 .o. [n1.l - n2.l] ]

      For the moment we implement it exactly like that.
      Later we should find a more efficient algorithm. */

  NVtype nv;
  NETptr a1, a2, result, nets [2];

  nv.len = 2;
  nv.nets = nets;

  a1 = fst_to_fsm (n1, DO_COPY, minimize_p, TRUE, side);
  a2 = fst_to_fsm (n2, DO_COPY, minimize_p, TRUE, side);

  if (side == UPPER)
    {
      nets [0] = minus_fsm (a1, a2, DO_RECLAIM, DO_RECLAIM, minimize_p);
      nets [1] = copy_fsm (n1);
    }
  else if (side == LOWER)
    {
      nets [0] = copy_fsm (n1);
      nets [1] = minus_fsm (a1, a2, DO_RECLAIM, DO_RECLAIM, minimize_p);
    }
  else
    {
      fsm_fprintf (stderr, "*** ERROR: wrong side-parameter \
in 'oneside_minus_fsm'\n");
      exit (1);
    }
  
  result = compose_fst (&nv, DO_RECLAIM, minimize_p, DO_CLEANUP, PAIRWISE,
			FLAGS_NOT_SPECIAL);
  
  if (reclaim_n1_p == DO_RECLAIM)
    free_network (n1);
  if (reclaim_n2_p == DO_RECLAIM)
    free_network (n2);
  check_for_flags(result);
  return (result);
}

/****************************************
 *
 *   PRIORITY UNION
 */

NETptr priority_union_fst_pair
  (NETptr n1, NETptr n2, int side, int copy_p, int minimize_p)
{
  /*  [ n1 .|u. n2 ] == [ n1 | [n2 .-u. n1] ]
      [ n1 .|l. n2 ] == [ n1 | [n2 .-l. n1] ]

      For the moment we implement it exactly like that.
      Later we should find a more efficient algorithm. */

  NVtype nv;  NETptr result, nets [2];

  if (copy_p == DO_COPY)
    {
      n1 = copy_fsm (n1);
      n2 = copy_fsm (n2);
    }

  nv.len = 2;
  nv.nets = nets;

  nets [0] = n1;
  nets [1] = oneside_minus_fsm (n2, n1, side,
				DO_RECLAIM, DONT_RECLAIM, minimize_p);

  result = union_fsm (&nv, DONT_COPY, minimize_p);

  return (result);
}

NETptr priority_union_nv
  (NVptr nv, int side, int copy_p, int minimize_p)
{
  NETptr n1, n2, result;
  int i;

  if (NV_len (nv) < 1)
    return (null_fsm ());

  result = NV_net (nv, 0);

  for (i=1; i< NV_len (nv); i++)
    {
      n1 = result;
      n2 = NV_net (nv, i);
      result = priority_union_fst_pair (n1, n2, side, copy_p, minimize_p);

      if ((i > 1) && (copy_p == DO_COPY))
	free_network (n1);
    }

  return (result);
}

/********************
 *                  *
 *   CROSS PRODUCT  *
 *   (mapping fst)  *
 *                  *
 ********************/


static void xproduct_fst_add_arcs(STATEptr source, 
                                  ALPHABETptr upper_other,
                                  ALPHABETptr lower_other, 
                                  id_type upsym, id_type losym, 
                                  STATEptr dest,
                                  CONSptr **upper_array, id_type lower_max,
                                  NETptr net, HEAPptr cons_heap)
     /* Adds to SOURCE the arcs implied by (possibly OTHER) UPSYM and LOSYM.
        Does nothing if DEST is NULL.

	This function used to call xproduct_get_labels() to get the
	list of symbols pairs resulting from the on-the-fly
	expansion of OTHER in upsym and lowsym. It now calls expand_fstpair()
	that is part of the composition. The two functions did essentially
	the same thing but had different bugs. xproduct_get_labels() has
	been removed. 1999-01-22 L.K.
        */
{       
  CONSptr list;
  CONSptr *lower_array;
  
  if (dest != NULL)
    {
      lower_array = get_lower_array(upper_array, upsym, losym, lower_max);

      /*      assert(losym < lower_max);  */
      /* If losym is not smaller than lower_max, we have a programming
         error of some sort. Instead of breaking, and baffling the
         end user with a cryptic error message, we now quietly fix
         the problem in get_lower_array() without any error message
         unless we are in DEBUG mode.  2004-02-05 LK */

      if (lower_array[losym] == NULL)
	{
	  /* First encounter of the upsym:lowsym pair. */

	  lower_array[losym] = expand_fstpair(upsym, losym, upper_other,
					      lower_other, FALSE, cons_heap);
	}

      /* label_list contains a list of labels implied by UPSYM and LOSYM.  
         This is how OTHER gets expanded. 
         */
      
      for (list = lower_array[losym]; list; list = cdr(list))
	push_arc(make_arc((int) car(list), dest, net), source);
    }
}


NETptr crossproduct_main (NETptr upper_net, NETptr lower_net,
			  ALPHABETptr upper_other, ALPHABETptr lower_other,
			  CONSptr **upper_array, id_type lower_max,
			  HEAPptr cons_heap, int minimize_p)
     /*****

	   The working core of the crossprouct algorithm.
	   Split off so that it can be called efficiently
	   from the compile-replace algorithm that may
	   call the crossproduct operation thousands of
	   times in a row with a label alphabet that can
	   be calculated in advance just once.
      *****/
{
  NETptr result;
  STATE_MRptr state_mr;
  STATEtype f1, f2;
  STATEptr result_state, old1, old2, dest1, dest2, fin1 = &f1, fin2 = &f2;
  ARCptr a1, a2;
  id_type lab1, lab2;
  int n2_index_p = FALSE;
  CONSptr pair;

  STATE_final(fin1) = STATE_final(fin2) = TRUE;
  STATE_deterministic(fin1) = STATE_deterministic(fin2) =  TRUE;
  STATE_arc_set(fin1) = STATE_arc_set(fin2) = NULL;
  STATE_client_cell(fin1) = STATE_client_cell(fin2) = NULL;
  next_state(fin1) = next_state(fin2) = NULL;
  STATE_visit_mark(fin1) = STATE_visit_mark(fin2) = 0;
  
  clear_client_cells(upper_net);
  clear_client_cells(lower_net);

  result = make_network();
  NET_labels(result) = make_alph(0, BINARY_VECTOR);
  NET_epsilon_free(result) = NET_epsilon_free(upper_net) && 
    NET_epsilon_free(lower_net);                                    
  NET_deterministic(result) = NET_deterministic(upper_net) &&
    NET_deterministic(lower_net);                                   
  NET_pruned(result) = NET_pruned(upper_net) &&
    NET_pruned(lower_net);
  NET_loop_free(result) = NET_loop_free(upper_net) &&
    NET_loop_free(lower_net);
  NET_arc_label_arity(result) = 2;
  
  if (NET_num_states(lower_net) > NET_num_states(upper_net))
    {
      n2_index_p = TRUE;
      state_mr = init_state_manager(upper_net, lower_net, result, lower_net);
    }
  else
    {
      n2_index_p = FALSE;
      state_mr = init_state_manager(upper_net, lower_net, result, upper_net);
    }
  
  NET_start_state(result) = find_pair_state(NET_start_state(upper_net), 
					    NET_start_state(lower_net),
					    state_mr);
  
  while ((result_state = next_new_state(state_mr)))
    {
      pair = (CONSptr) STATE_client_cell(result_state);
      old1 = (STATEptr) car(pair);
      old2 = (STATEptr) cdr(pair);
      
      free_cons_cell(pair, STATE_MR_cons_heap(state_mr));
      STATE_client_cell(result_state) = NULL;
      
      for (a1 = STATE_arc_set(old1); a1 != NULL; a1 = next_arc(a1))
        {
          dest1 = ARC_destination(a1);
          lab1 = ARC_label(a1);
          for (a2 = STATE_arc_set(old2); a2 != NULL; a2 = next_arc(a2))
            {
              dest2 = ARC_destination(a2);
              lab2 = ARC_label(a2);
              xproduct_fst_add_arcs(result_state, upper_other, lower_other, 
                                    lab1,lab2, 
                                    find_pair_state(dest1, dest2, state_mr),
                                    upper_array, lower_max, result, cons_heap);
            }
          
          /* Fake epsilon to fin2 from every final state,
             to allow for string lengthening */
          if (STATE_final(old2))
            xproduct_fst_add_arcs(result_state, upper_other, lower_other, 
                                  lab1, EPSILON, 
                                  find_pair_state(dest1, fin2, state_mr),
                                  upper_array, lower_max, result, cons_heap);
        }
      
      /* Fake epsilon to fin1 from every final state,
         to allow for string lengthening */
      if (STATE_final(old1))
        for (a2 = STATE_arc_set(old2); a2 != NULL; a2 = next_arc(a2))
          {
            dest2 = ARC_destination(a2);
            lab2 = ARC_label(a2);
            xproduct_fst_add_arcs(result_state, upper_other, lower_other, 
                                  EPSILON,lab2, 
                                  find_pair_state(fin1, dest2, state_mr),
                                  upper_array, lower_max, result, cons_heap);
          }
      
      STATE_deterministic(result_state) =
        STATE_deterministic(old1) && STATE_deterministic(old2);
      
      STATE_final(result_state) = STATE_final(old1) && STATE_final(old2);
    }
  
  update_label_alphabet(result);

  free_state_manager(state_mr);

  if (n2_index_p)
    clear_client_cells(lower_net);
  else
    clear_client_cells(upper_net);
  
  if (FSM_MODE == TWOL_MODE)
    {
      NET_twol_net(result) = TRUE;
      NET_sigma(result) = copy_alphabet(NET_labels(result));
    }
  else
    NET_sigma(result) =
      sigma_union(NET_sigma(upper_net), NET_sigma(lower_net));

  binary_to_label(NET_labels(result));

  if (minimize_p == DO_MINIMIZE)
    min_fsm(result, DONT_COPY);
  
  check_for_flags(result);

  return(result);
}

static NETptr abort_crossproduct(NETptr upper_net, NETptr lower_net,
				 int reclaim_p, char *reason)
{
  fsm_fprintf(stderr, "*** Aborting crossproduct.\n    %s.\n", reason);

  if (reclaim_p == DO_RECLAIM)
    {
      free_network(upper_net);
      free_network(lower_net);
    }
  
  return(null_fsm());
}



NETptr crossproduct_fst (NETptr upper_net, NETptr lower_net, int reclaim_p,
			 int minimize_p)
     /*****
      *
      * Produces an FST that will accept pairs of tapes if the upper contains
      * an instance of UPPER_NET and the lower an instance of LOWER_NET--i.e., 
      * the result transduces between the two languages.
      * 
      * Note that the FSMs are mutually unconstraining, so all the properties
      * propagate conjointly.
      * 
      * If RECLAIM_P is DO_RECLAIM, both UPPER_NET and LOWER_NET will be
      * reclaimed.

      Now using the same scheme to keep track of newly created labels
      as compose_fst().  Better use of memory. LK 2002-02-11

      *****/
{
  ALPHABETptr upper_other, lower_other;
  NETptr result;

  id_type upper_max, lower_max;
  CONSptr **upper_array;
  HEAPptr cons_heap;

  /* If either one of the incoming networks is a null fsm there is
     no work to be done because the result is guaranteed to be
     a null fsm, too.
     */

  if (null_fsm_p(upper_net) || null_fsm_p(lower_net))
    {
      if (reclaim_p == DO_RECLAIM)
	{
	  free_network(upper_net);
	  free_network(lower_net);
	}

      return(null_fsm());
    }

  if (NET_arc_label_arity(upper_net) > 1 ||
      NET_arc_label_arity(lower_net) > 1)
    return(abort_crossproduct(upper_net, lower_net, reclaim_p,
			      "Operation is not applicable to a transducer"));

  scan_labels(upper_net, lower_net, &upper_max, NULL, NULL, &upper_other);
  scan_labels(lower_net, upper_net, &lower_max, NULL, NULL, &lower_other); 
  binary_to_label(upper_other);
  binary_to_label(lower_other);
  
  upper_max++; lower_max++;

  if (!(cons_heap = make_cons_heap()))
    return(abort_crossproduct(upper_net, lower_net, reclaim_p,
			      "Not enough memory for this operation"));

  upper_array = (CONSptr **)calloc((size_t) (upper_max),
				   (size_t) sizeof(CONSptr *));
  if (!upper_array)
    return(abort_crossproduct(upper_net, lower_net, reclaim_p,
			      "Not enough memory for this operation"));

  result = crossproduct_main(upper_net, lower_net, upper_other, lower_other,
			     upper_array, lower_max, cons_heap, minimize_p);

  free_alph(upper_other);
  free_alph(lower_other);

  free_upper_array(upper_array, NET_labels(upper_net));
  free_heap(cons_heap);

  if (reclaim_p == DO_RECLAIM)
    {
      free_network(upper_net);
      free_network(lower_net);
    }

  return(result);
}

/******************************************
 *
 *      SHUFFLE
 *
 *******************************************/

static NVptr shuffle_prep(NVptr nets, ALPHABETptr *comm_labels,
                          ALPHABETptr *comm_sigma)
{
  
  /* Preparatory steps to SHUFFLE_FSM. Makes copies if necessary,
     and computes the common sigma and the common label sets.  Returns NULL
     if one of the nets is a null fsm, otherwise a net vector.
     */
  
  NETptr cur;
  int i;
  
  /* If any of the arguments is a null fsm, we can stop right here,
     otherwise check if copies really need to be made when COPY_P is DO_COPY.
     */
  
  for (i = 0; i < NV_len(nets); i++)
    if (null_fsm_p(NV_net(nets, i)))
      return(NULL);
  
  /*  Compose the joint sigma and label sets.
   */
  
  for (i = 0; i < NV_len(nets); i++)
    {
      cur = NV_net(nets, i);
      
      if (i == 0) /* first time only */
        {
          *comm_labels = binary_to_label(copy_alphabet(NET_labels(cur)));
          *comm_sigma = copy_alphabet(sigma(cur));              
        }
      else
        {
          label_augment(*comm_labels, NET_labels(cur));
          sigma_augment(*comm_sigma, sigma(cur));
        }
    }
  
  return(nets);
}

static NETptr do_shuffle(NETptr n1, NETptr n2, ALPHABETptr common_sigma,
                         ALPHABETptr common_labels, int16 *label_vector,
                         int minimize_p)
{
  NETptr net;
  STATEptr state, n1_state, n2_state, dest;
  STATE_MRptr state_mr;
  HEAPptr cons_heap;
  ARCptr arc;
  CONSptr pair;
  int det_p, n2_index_p;
  
  net = make_network();
  free_alph(NET_labels(net));
  NET_labels(net) = common_labels;
  NET_sigma(net) = common_sigma;
  
  expand_other(n2, common_sigma, DONT_COPY);
  
  if (NET_num_states(n2) > NET_num_states(n1))
    {
      state_mr = init_state_manager(n1, n2, net, n2);
      n2_index_p = TRUE;
    }
  else
    {
      state_mr = init_state_manager(n1, n2, net, n1);
      n2_index_p = FALSE;
    }
  
  cons_heap = STATE_MR_cons_heap(state_mr);

  NET_start_state(net) =
    find_pair_state(NET_start_state(n1), NET_start_state(n2), state_mr);
  
  /* If both arguments are deterministic, watch out for duplicate labels */
  if (NET_deterministic(n1) && NET_deterministic(n2))
    NET_deterministic(net) = TRUE;
  
  if (NET_arc_label_arity(n1) >= NET_arc_label_arity(n2))
    NET_arc_label_arity(net) = NET_arc_label_arity(n1);
  else
    NET_arc_label_arity(net) = NET_arc_label_arity(n2);
  
  if (NET_pruned(n1) && NET_pruned(n2))
    NET_pruned(net) = TRUE;
  
  if (NET_epsilon_free(n1) && NET_epsilon_free(n2))
    NET_epsilon_free(net) = TRUE;
  
  if (NET_loop_free(n1) && NET_loop_free(n2))
    NET_loop_free(net) = TRUE;
  
  while((state = next_new_state(state_mr)))
    {
      pair = (CONSptr) STATE_client_cell(state);
      n1_state = (STATEptr) car(pair);
      n2_state = (STATEptr) cdr(pair);
      
      free_cons_cell(pair, cons_heap);
      STATE_client_cell(state) = NULL;
      
      STATE_final(state) = STATE_final(n1_state) && STATE_final(n2_state);
      
      /* Every new state is initially flagged as deterministic. If N1_STATE
         and N2_STATE are both deterministic, keep track of labels and
         change the flag when a duplicate is encountered.
         */
      
      if (!(STATE_deterministic(n1_state) && STATE_deterministic(n2_state)))
        {
          STATE_deterministic(state) = FALSE;
          NET_deterministic(net) = FALSE;
          det_p = FALSE;
        }
      else
        det_p = TRUE;
      
      for (arc = STATE_arc_set(n1_state); arc != NULL; arc = next_arc(arc))
        {
          dest = find_pair_state(ARC_destination(arc), n2_state, state_mr);

          push_arc(make_arc(ARC_label(arc), dest, net), state);
          if (det_p)
            label_vector[ARC_label(arc)] = 1;
        }
      for (arc = STATE_arc_set(n2_state); arc!= NULL; arc = next_arc(arc))
        {
          dest = find_pair_state(n1_state, ARC_destination(arc), state_mr);

          push_arc(make_arc(ARC_label(arc), dest, net), state);
          if (det_p && label_vector[ARC_label(arc)] == 1)
            {
              STATE_deterministic(state) = FALSE;
              NET_deterministic(net) = FALSE;
            }
        }
      /* Clear LABEL_VECTOR for the next round if we used it. */
      if (det_p)
        for (arc = STATE_arc_set(n1_state); arc != NULL; arc = next_arc(arc))
          label_vector[ARC_label(arc)] = 0;
    }
  
  
  free_state_manager(state_mr);
  
  if (minimize_p)
    min_fsm(net, DONT_COPY);
  
  return(net);
}

NETptr shuffle_fsm(NVptr nets, int reclaim_p, int minimize_p)
{
  /* Computes the shuffle of an arbitrary number of networks. Returns
     concatenation identity, an epsilon fsm, if no args are given,
     the single argument or its copy if only one is given, a null
     fsm if any of the arguments is a null fsm. Unlike CONCAT_FSM,
     SHUFFLE_FSM does not invalidate its arguments but any nets
     containing OTHER may be get additional arcs because of the call
     to EXPAND_OTHER. If RECLAIM_P is DO_RECLAIM, the argument nets (but
     not the NV pointer containing them) are freed.
     
     Mathematical note: The number of words produced by the shuffle
     of two words A and B is the factorial of the sum of their lengths
     divided by the product of the factorial of A's length and the
     factorial of B's length:
     
     (len(A) + len(B))!   
     ------------------
     len(A)! * len(B)!
     
     For example, if A is "abc" and B is "xy", we get 5!/(3! * 2!),
     that is, 10 words:
     
     abcxy abxcy abxyc axbcy axbyc axybc xabcy xabyc xaybc xyabc
     
     The number of words produced by the shuffle of two networks N1
     and N2 is the sum of the above function for all pairs A and B
     such that A is in the language of N1 and B is in the language
     of N2.
     */
  
  NVptr prep_nets;
  NETptr n1, n2, prev;
  ALPHABETptr common_sigma, common_labels;
  int16 *label_vector;
  int i;

  if (nets == NULL || NV_len(nets) == 0)
    return(epsilon_fsm());
  
  if (NV_len(nets) == 1)
    return(NV_net(nets,0));
  
  prep_nets = shuffle_prep(nets, &common_labels, &common_sigma);
  
  if (prep_nets == NULL)
    {
      if (reclaim_p == DO_RECLAIM)
        free_nets_only(nets);
      return(null_fsm());
    }
  
  label_vector =
    (int16 *) calloc((size_t) ALPH_len(common_sigma), (size_t) sizeof(int16));
  
  /* In the calls to DO_SHUFFLE, use the larger net as the first argument
     for better indexing in GET_PAIR_STATE and make sure the client
     cells of the first argument are clean.
     */
  
  if (NET_num_states(NV_net(prep_nets, 0)) <
      NET_num_states(NV_net(prep_nets, 1)))
    {
      n2 = NV_net(prep_nets, 0);
      n1 = NV_net(prep_nets, 1);
    }
  else
    {
      n1 = NV_net(prep_nets, 0);
      n2 = NV_net(prep_nets, 1);
    }
  
  expand_other(n1, common_sigma, DONT_COPY);
  clear_client_cells(n1);
  n1 = do_shuffle(n1, n2, common_sigma, common_labels, label_vector,
                  minimize_p);
  
  /* Repeat for the remaining arguments releasing
     the intermediate results. */
  
  for (i = 2; i < NV_len(prep_nets); i++)
    {
      prev = n1;
      n2 = NV_net(prep_nets, i);
      
      if (NET_num_states(prev) < NET_num_states(n2))
        {
          expand_other(n1, common_sigma, DONT_COPY);
          clear_client_cells(n2);
          n1 = do_shuffle(n2, prev, common_sigma, common_labels,
                          label_vector, minimize_p);
        }
      else
        /* Client cells of PREV are known to be clean. */
        n1 = do_shuffle(prev, n2, common_sigma, common_labels,
                        label_vector, minimize_p);
      
      /* %%%TSY 11/29/92 BUG FIX:  
         Don't free labels/sigma of prev if used elsewhere. */
      if (NET_labels(prev) == common_labels)
        NET_labels(prev) = NULL;
      if (NET_sigma(prev) == common_sigma)
        NET_sigma(prev) = NULL;
      free_network(prev);
    }
  
  free((void *) label_vector);
  
  if (reclaim_p == DO_RECLAIM)
    free_nets_only(prep_nets);
  
  return(n1);
}


NETptr prefix_iff_suffix(NETptr prefix, NETptr suffix)
{
    /* Computes
       
       [~[ [?* PREFIX] ~[[SUFFIX ?*] - [PREFIX ?*]]] &
        ~[~[?* PREFIX]  [[SUFFIX ?*] - [PREFIX ?*]]]
       
       without consuming any of the arguments.
       */

    NETptr prefix_if_suffix, suffix_if_prefix;
    NETptr ends_in_prefix, starts_with_suffix, starts_with_prefix;
    NVtype nv;
    NETptr nets[2];

    nv.nets = nets;
    nv.len = 2;

    nets[0] = sigma_star_fsm();
    nets[1] = copy_fsm(prefix);

    ends_in_prefix = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = copy_fsm(suffix);
    nets[1] = sigma_star_fsm();

    starts_with_suffix = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = copy_fsm(prefix);
    nets[1] = sigma_star_fsm();

    starts_with_prefix = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    starts_with_suffix = minus_fsm(starts_with_suffix, starts_with_prefix,
                                   DO_RECLAIM, DO_RECLAIM, DO_MINIMIZE);

    nets[0] = copy_fsm(ends_in_prefix);
    nets[1] = negate_fsm(starts_with_suffix, DO_COPY, DO_MINIMIZE);

    suffix_if_prefix = negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                                  DONT_COPY, DO_MINIMIZE);

    nets[0] = negate_fsm(ends_in_prefix, DONT_COPY, DO_MINIMIZE);
    nets[1] = starts_with_suffix;

    prefix_if_suffix = negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                                  DONT_COPY, DO_MINIMIZE);
    nets[0] = suffix_if_prefix;
    nets[1] = prefix_if_suffix;

    return(intersect_fsm(&nv, DO_RECLAIM, DO_MINIMIZE));
}


NETptr suffix_iff_prefix(NETptr suffix, NETptr prefix)
{
    /* Computes
       
       [~[~[[?* PREFIX] - [?* SUFFIX]]  [SUFFIX ?*]] &
        ~[ [[?* PREFIX] - [?* SUFFIX]] ~[SUFFIX ?*]]]
       
       without consuming any of the arguments.
       
       Equivalent to REVERSE(prefix_iff_suffix(prefix, REVERSE(suffix)))
       
       */

    NETptr suffix_if_prefix, prefix_if_suffix;
    NETptr ends_in_prefix, starts_with_suffix, ends_in_suffix;
    NVtype nv;
    NETptr nets[2];

    nv.nets = nets;
    nv.len = 2;

    nets[0] = sigma_star_fsm();
    nets[1] = copy_fsm(prefix);

    ends_in_prefix = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = sigma_star_fsm();
    nets[1] = copy_fsm(suffix);

    ends_in_suffix = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    ends_in_prefix = minus_fsm(ends_in_prefix, ends_in_suffix,
                               DO_RECLAIM, DO_RECLAIM, DO_MINIMIZE);

    nets[0] = copy_fsm(suffix);
    nets[1] = sigma_star_fsm();

    starts_with_suffix = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = negate_fsm(ends_in_prefix, DO_COPY, DO_MINIMIZE);
    nets[1] = copy_fsm(starts_with_suffix);

    prefix_if_suffix = negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                                  DONT_COPY, DO_MINIMIZE);

    nets[0] = ends_in_prefix;
    nets[1] = negate_fsm(starts_with_suffix, DONT_COPY, DO_MINIMIZE);

    suffix_if_prefix = negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                                  DONT_COPY, DO_MINIMIZE);
    nets[0] = prefix_if_suffix;
    nets[1] = suffix_if_prefix;

    return(intersect_fsm(&nv, DO_RECLAIM, DO_MINIMIZE));
}

NETptr suffix_iff_prefix_final(NETptr suffix, NETptr prefix, NETptr extension)
{
    /* Computes
       
       [~[~[[?* PREFIX] - [?* SUFFIX]]  [SUFFIX ?*]] &
        ~[ [?* SUFFIX] [[EXTENSION ?*] - [SUFFIX ?*]]] &
        ~[ [[?* PREFIX] - [?* SUFFIX]] ~[[SUFFIX | EXTENSION] ?*]]]
       
       without consuming any of the arguments.

       Allows SUFFIX after PREFIX when not followed by EXTENSION.
       Requires PREFIX to be followed by EXTENSION or SUFFIX.
       
       */

    NETptr suffix_if_prefix, prefix_if_suffix;
    NETptr ends_in_prefix, starts_with_suffix, ends_in_suffix;
    NETptr starts_with_ext, not_suffix_ext, starts_with_suffix_or_ext;
    NVtype nv;
    NETptr nets[3];

    nv.nets = nets;
    nv.len = 2;

    nets[0] = sigma_star_fsm();
    nets[1] = copy_fsm(prefix);
    ends_in_prefix = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = sigma_star_fsm();
    nets[1] = copy_fsm(suffix);
    ends_in_suffix = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    ends_in_prefix = minus_fsm(ends_in_prefix, ends_in_suffix,
                               DO_RECLAIM, DONT_RECLAIM, DO_MINIMIZE);

    nets[0] = copy_fsm(suffix);
    nets[1] = sigma_star_fsm();
    starts_with_suffix = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = negate_fsm(ends_in_prefix, DO_COPY, DO_MINIMIZE);
    nets[1] = copy_fsm(starts_with_suffix);
    prefix_if_suffix = negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                                  DONT_COPY, DO_MINIMIZE);

    nets[0] = copy_fsm(extension);
    nets[1] = sigma_star_fsm();
    starts_with_ext = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);
    starts_with_ext = minus_fsm(starts_with_ext, starts_with_suffix,
                                   DO_RECLAIM, DONT_RECLAIM, DO_MINIMIZE);
    nets[0] = ends_in_suffix;
    nets[1] = copy_fsm(starts_with_ext);
    not_suffix_ext = negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                                   DONT_COPY, DO_MINIMIZE);

    nets[0] = starts_with_suffix;
    nets[1] = starts_with_ext;
    starts_with_suffix_or_ext = union_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = ends_in_prefix;
    nets[1] = negate_fsm(starts_with_suffix_or_ext, DONT_COPY, DO_MINIMIZE);

    suffix_if_prefix = negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                                  DONT_COPY, DO_MINIMIZE);
    nv.len = 3;

    nets[0] = prefix_if_suffix;
    nets[1] = not_suffix_ext;
    nets[2] = suffix_if_prefix;

    return(intersect_fsm(&nv, DO_RECLAIM, DO_MINIMIZE));
}

NETptr prefix_iff_suffix_initial(NETptr prefix, NETptr suffix,
                                 NETptr extension)
{
    /* Computes
       
       [~[[?* PREFIX] ~[[SUFFIX ?*] - [PREFIX ?*]]] &
        ~[[[?* EXTENSION] - [?* PREFIX]] [PREFIX ?*]] &
        ~[~[?* [PREFIX | EXTENSION]]  [[SUFFIX ?*] - [PREFIX ?*]]]
       
       without consuming any of the arguments.

       Allows PREFIX before SUFFIX when not preceded by EXTENSION
       Requires SUFFIX to be preceded by PREFIX or EXTENSION.
       
       */
    NETptr prefix_if_suffix, suffix_if_prefix;
    NETptr ends_in_prefix, starts_with_suffix, starts_with_prefix;
    NETptr ends_in_ext, not_ext_prefix, ends_in_ext_or_prefix;
    NVtype nv;
    NETptr nets[3];

    nv.nets = nets;
    nv.len = 2;

    nets[0] = sigma_star_fsm();
    nets[1] = copy_fsm(prefix);

    ends_in_prefix = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = copy_fsm(suffix);
    nets[1] = sigma_star_fsm();
    starts_with_suffix = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = copy_fsm(prefix);
    nets[1] = sigma_star_fsm();
    starts_with_prefix = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    starts_with_suffix = minus_fsm(starts_with_suffix, starts_with_prefix,
                                   DO_RECLAIM, DONT_RECLAIM, DO_MINIMIZE);
    nets[0] = copy_fsm(ends_in_prefix);
    nets[1] = negate_fsm(starts_with_suffix, DO_COPY, DO_MINIMIZE);

    suffix_if_prefix = negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                                  DONT_COPY, DO_MINIMIZE);

    nets[0] = sigma_star_fsm();
    nets[1] = copy_fsm(extension);
    ends_in_ext = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);
    ends_in_ext = minus_fsm(ends_in_ext, ends_in_prefix,
                               DO_RECLAIM, DONT_RECLAIM, DO_MINIMIZE);

    nets[0] = copy_fsm(ends_in_ext);
    nets[1] = starts_with_prefix;
    not_ext_prefix = negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                                   DONT_COPY, DO_MINIMIZE);

    nets[0] = ends_in_ext;
    nets[1] = ends_in_prefix;
    ends_in_ext_or_prefix = union_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = negate_fsm(ends_in_ext_or_prefix, DONT_COPY, DO_MINIMIZE);
    nets[1] = starts_with_suffix;

    prefix_if_suffix = negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                                  DONT_COPY, DO_MINIMIZE);
    nv.len = 3;

    nets[0] = suffix_if_prefix;
    nets[1] = not_ext_prefix;
    nets[2] = prefix_if_suffix;

    return(intersect_fsm(&nv, DO_RECLAIM, DO_MINIMIZE));
}

NETptr pref_if_suff(NETptr pref, NETptr suff)
{
    /* Computes
       
        ~[~[?* PREFIX]  [SUFFIX ?*]]
       
       without consuming any of the arguments.
       */

    NETptr ends_in_pref, starts_with_suff;
    NVtype nv;
    NETptr nets[2];

    nv.nets = nets;
    nv.len = 2;

    nets[0] = sigma_star_fsm();
    nets[1] = copy_fsm(pref);

    ends_in_pref = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = copy_fsm(suff);
    nets[1] = sigma_star_fsm();

    starts_with_suff = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = negate_fsm(ends_in_pref, DONT_COPY, DO_MINIMIZE);
    nets[1] = starts_with_suff;

    return(negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                      DONT_COPY, DO_MINIMIZE));
}

NETptr suff_if_pref(NETptr suff, NETptr pref)
{
    /* Computes
       
       ~[ [?* PREFIX] ~[SUFFIX ?*]]
       
       without consuming any of the arguments.
       */

    NETptr ends_in_pref, starts_with_suff;
    NVtype nv;
    NETptr nets[2];

    nv.nets = nets;
    nv.len = 2;

    nets[0] = sigma_star_fsm();
    nets[1] = copy_fsm(pref);

    ends_in_pref = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = copy_fsm(suff);
    nets[1] = sigma_star_fsm();

    starts_with_suff = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    nets[0] = ends_in_pref;
    nets[1] = negate_fsm(starts_with_suff, DONT_COPY, DO_MINIMIZE);

    return(negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                      DONT_COPY, DO_MINIMIZE));
}

NETptr pref_iff_suff(NETptr pref, NETptr suff)
{
    /* Computes
       
       [~[ [?* PREFIX] ~[SUFFIX ?*]] &
        ~[~[?* PREFIX]  [SUFFIX ?*]]]
       
       without consuming any of the arguments.
       */

    NVtype nv;
    NETptr nets[2];

    nv.nets = nets;
    nv.len = 2;

    nets[0] = pref_if_suff(pref, suff);
    nets[1] = suff_if_pref(suff, pref);

    return(intersect_fsm(&nv, DO_RECLAIM, DO_MINIMIZE));
}


NETptr suff_iff_pref(NETptr suff, NETptr pref)
{
    /* Computes
       
       [~[~[?* PREFIX]  [SUFFIX ?*]] &
        ~[ [?* PREFIX] ~[SUFFIX ?*]]]
       
       without consuming any of the arguments.
       
       */

    NVtype nv;
    NETptr nets[2];

    nv.nets = nets;
    nv.len = 2;

    nets[0] = suff_if_pref(suff, pref);
    nets[1] = pref_if_suff(pref, suff);

    return(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE));
}

int side_universal_p (NETptr net, ALPHABETptr sigma, int side)
{
  /* Returns TRUE if the network accepts every symbol from 'sigma'
     in any number and order, on the specified side (UPPER or LOWER
     or BOTH_SIDES).  BOTH_SIDES means UPPER and LOWER.
     If sigma == NULL then it is assumed to be [ ? ], i.e.
     the network will be tested for universality.

     No arguments are consumed or modified !!! */

  NVptr symbols;
  NETptr side_net = NULL, ref_net, result_net;
  STATEptr start;
  int i, result;

  if (NET_arc_label_arity(net) == 1)   /* simple fsm */
    side_net = copy_fsm (net);
  else                /* transducer */
    switch (side)
      {
      case UPPER:
	side_net = upper_fsm (net, DO_COPY, DO_MINIMIZE);
	break;
      case LOWER:
	side_net = lower_fsm (net, DO_COPY, DO_MINIMIZE);
	break;
      case BOTH_SIDES:
	result = (side_universal_p (net, sigma, UPPER)
		  & side_universal_p (net, sigma, LOWER));
	return (result);
      }

  /* build the reference network */

  if (sigma)  /* defined alphabet */
    {
      sigma = binary_to_label (copy_alphabet (sigma));
      symbols = make_nv (ALPH_len (sigma));

      for (i=0; i< ALPH_len (sigma); i++)
	NV_net (symbols, i) = symbol_fsm (ALPH_item (sigma, i));

      ref_net = union_fsm (symbols, DONT_COPY, DONT_MINIMIZE);
      ref_net = zero_plus (ref_net, DONT_COPY, DO_MINIMIZE);

      free_nv_only (symbols);
      free_alph (sigma);
    }
  else      /* universal alphabet */
    ref_net = sigma_star_fsm ();

  /* compare both networks */

  result_net =
    minus_fsm (ref_net, side_net, DO_RECLAIM, DO_RECLAIM, DO_MINIMIZE);

  start = NET_start_state (result_net);
  result = ((! STATE_final (start)) && (! STATE_arc_set (start)));

  free_network (result_net);
  return (result);
}

/*

  ELIMINATING FLAG DIACRITICS 

 */

static void not_yet_implemented(char *action)
{
  fsm_fprintf(stdout, "Sorry. %s flags cannot yet be eliminated.\n", action);
}

static NETptr compile_flag_constraint(id_type id, ALPHABETptr disable_ids,
				      ALPHABETptr enable_ids)
{
  /* Compiles  ~[?* DISABLE_IDS ~$[ENABLE_IDS] ID ?*]  */

  NVtype nv;
  NETptr nets[5];

  nv.nets = nets;
  nv.len = 5;

  nets[0] = sigma_star_fsm();
  nets[1] = sigma_fsm(disable_ids);
  nets[2] = negate(contains(sigma_fsm(enable_ids)));
  nets[3] = symbol_fsm(id);
  nets[4] = sigma_star_fsm();

  return(negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
		    DONT_COPY, DO_MINIMIZE));
}

static NETptr unify_test(id_type self_id, id_type clear_id, ALPHABETptr all_flags,
			 uint16 alph_len)

{
  /* Converts the constraint on the distribution of self_id in the
     into a network. The constraint on "@U.FEATURE.VALUEi@" is
            ~[?* DISABLE_IDS ~$[ENABLE_IDS] SELF_ID ?*]
     where SELF_ID is the symbol in question, ENABLE_IDS is the union
     of clear_id with all the flags that permit self_id; that is
     a positive setting for the value in question or a negative
     setting to some other value.
     DISABLE_IDS is the union of all feature settings and unifications
     that are incompatible with SELF_ID.
     */

  NETptr constr;
  uint16 i;
  id_type id;
  FLAG_DIACRptr flag, self_flag = flag_diacritic(self_id);
  ALPHABETptr disable_ids = make_alph(alph_len, BINARY_VECTOR),
    enable_ids = make_alph(alph_len, BINARY_VECTOR);

  /* Initialize ENABLE_IDS */

  if (clear_id != 0 && clear_id != ID_NO_SYMBOL)
    sigma_add_to(enable_ids, clear_id);

  /* Collect DISABLE_IDS and ENABLE_IDS */

  for (i = 0; i < ALPH_len(all_flags); i++)
    {
      id = ALPH_item(all_flags, i);
      flag = flag_diacritic(id);

      switch (FLAG_DIACR_action(flag))
	{
	case CLEAR_SETTING:
	  /* No action required */
	  break;
	case POSITIVE_SETTING:
	  if (FLAG_DIACR_value(flag) == FLAG_DIACR_value(self_flag))
	    sigma_add_to(enable_ids, id);
	  else
	    sigma_add_to(disable_ids, id);
	  break;
	case NEGATIVE_SETTING:
	  if (FLAG_DIACR_value(flag) == FLAG_DIACR_value(self_flag))
	    sigma_add_to(disable_ids, id);
	  else
	    sigma_add_to(enable_ids, id);
	  break;
	case UNIFY_TEST:
	  if (FLAG_DIACR_value(flag) != FLAG_DIACR_value(self_flag))
	    sigma_add_to(disable_ids, id);
	  break;
	case DISALLOW_TEST:
	case REQUIRE_TEST:
	case INSERT_SUBNET:
	default:
	  /* No action required */
	  break;
	}
    }
  
  /* Assemble constraint ~[?* DISABLE_IDS ~$[ENABLE_IDS] SELF_ID ?*] */

  constr = compile_flag_constraint(self_id, disable_ids, enable_ids);

  free_alph(disable_ids);
  free_alph(enable_ids);

  return(constr);
}

static NETptr disallow_test(id_type self_id, id_type clear_id,
			    ALPHABETptr all_flags, uint16 alph_len)

{
  /* Converts the constraint on the distribution of self_id in the
     into a network. The constraint on "@D.FEATURE.VALUEi@" is
	   ~[?* DISABLE_IDS ~$[ENABLE_IDS] SELF_ID ?*] 
     where SELF_ID is the symbol in question, ENABLE_IDS is the union
     of clear_id with all the flags that are incompatible with self_id.
     DISABLE_IDS is the union of all feature settings and unifications
     that are that set the register to self_id or a disjunction containing
     it.
     */

  NETptr constr;
  uint16 i;
  id_type id;
  FLAG_DIACRptr flag, self_flag = flag_diacritic(self_id);
  ALPHABETptr enable_ids = make_alph(alph_len, BINARY_VECTOR),
    disable_ids = make_alph(alph_len, BINARY_VECTOR);

  /* Initialize ENABLE_IDS */

  if (clear_id != 0 && clear_id != ID_NO_SYMBOL)
    sigma_add_to(enable_ids, clear_id);

  /* Collect ENABLE_IDS and DISABLE_IDS */

  for (i = 0; i < ALPH_len(all_flags); i++)
    {
      id = ALPH_item(all_flags, i);
      flag = flag_diacritic(id);

      switch (FLAG_DIACR_action(flag))
	{
	case CLEAR_SETTING:
	  break;
	case POSITIVE_SETTING:
	  if (FLAG_DIACR_value(self_flag) == UNDEFINED_FLAG_VALUE)
	    sigma_add_to(disable_ids, id);
	  else if (FLAG_DIACR_value(flag) == FLAG_DIACR_value(self_flag))
	    sigma_add_to(disable_ids, id);
	  else
	    sigma_add_to(enable_ids, id);
	  break;
	case NEGATIVE_SETTING:
	  if (FLAG_DIACR_value(self_flag) == UNDEFINED_FLAG_VALUE)
	    sigma_add_to(disable_ids, id);
	  else if (FLAG_DIACR_value(flag) == FLAG_DIACR_value(self_flag))
	    sigma_add_to(enable_ids, id);
	  else
	    sigma_add_to(disable_ids, id);
	  break;
	case UNIFY_TEST:
	  if (FLAG_DIACR_value(self_flag) == UNDEFINED_FLAG_VALUE)
	    sigma_add_to(disable_ids, id);
	  else if (FLAG_DIACR_value(flag) == FLAG_DIACR_value(self_flag))
	    sigma_add_to(disable_ids, id);
	  else
	    sigma_add_to(enable_ids, id);
	  break;
	case DISALLOW_TEST:
	case REQUIRE_TEST:
	case INSERT_SUBNET:
	default:
	  /* No action required */
	  break;
	}
    }
  
  /* Assemble constraint ~[?* DISABLE_IDS ~$[ENABLE_IDS] SELF_ID ?*] */

  constr = compile_flag_constraint(self_id, disable_ids, enable_ids);

  free_alph(enable_ids);
  free_alph(disable_ids);

  return(constr);
}

static NETptr require_test(id_type self_id, id_type clear_id,
			    ALPHABETptr all_flags, uint16 alph_len)

{
  /* Converts the constraint on the distribution of self_id in the
     into a network. The constraint on "@R.FEATURE.VALUEi@" is
	   ~[?* DISABLE_IDS ~$[ENABLE_IDS] SELF_ID ?*]
	          && ~[~$[ENABLE_IDS] SELF_ID ?*]
     where SELF_ID is the symbol in question, ENABLE_IDS is the union
     of all the flags that set the register to self_id. DISABLE_IDS
     is the union of all feature settings and unifications
     that set the register to some other value.
     */

  NVtype nv;
  NETptr constr, nets[3];
  int16 i, id;
  FLAG_DIACRptr flag, self_flag = flag_diacritic(self_id);
  ALPHABETptr enable_ids = make_alph(alph_len, BINARY_VECTOR),
    disable_ids = make_alph(alph_len, BINARY_VECTOR);

  nv.len = 3;
  nv.nets = nets;

  /* Initialize DISABLE_IDS */

  if (clear_id != 0 && clear_id != ID_NO_SYMBOL)
    sigma_add_to(disable_ids, clear_id);

  /* Collect ENABLE_IDS and DISABLE_IDS */

  for (i = 0; i < ALPH_len(all_flags); i++)
    {
      id = ALPH_item(all_flags, i);
      flag = flag_diacritic(id);

      switch (FLAG_DIACR_action(flag))
	{
	case CLEAR_SETTING:
	  break;
	case POSITIVE_SETTING:
	  if (FLAG_DIACR_value(self_flag) == UNDEFINED_FLAG_VALUE)
	    sigma_add_to(enable_ids, id);
	  else if (FLAG_DIACR_value(flag) == FLAG_DIACR_value(self_flag))
	    sigma_add_to(enable_ids, id);
	  else
	    sigma_add_to(disable_ids, id);
	  break;
	case NEGATIVE_SETTING:
	  if (FLAG_DIACR_value(self_flag) == UNDEFINED_FLAG_VALUE)
	    sigma_add_to(enable_ids, id);
	  else
	    sigma_add_to(disable_ids, id);
	  break;
	case UNIFY_TEST:
	  if (FLAG_DIACR_value(self_flag) == UNDEFINED_FLAG_VALUE)
	    sigma_add_to(enable_ids, id);
	  else if (FLAG_DIACR_value(flag) == FLAG_DIACR_value(self_flag))
	    sigma_add_to(enable_ids, id);
	  else
	    sigma_add_to(disable_ids, id);
	  break;
	case DISALLOW_TEST:
	case REQUIRE_TEST:
	case INSERT_SUBNET:
	default:
	  /* No action required */
	  break;
	}
    }
  
  /* Compute the constraint ~[?* DISABLE_IDS ~$[ENABLE_IDS] SELF_ID ?*]   */

  constr = compile_flag_constraint(self_id, disable_ids, enable_ids);

  /* For the REQUIRE_TEST this common flag constraint is not
     sufficient as it only requires an enabler after each
     disabler. We need to add the requirement that some enabler
     is must precede the in any case:

     ~[~$[ENABLE_IDS] SELF_ID ?*]

     */

  nets[0] = negate(contains(sigma_fsm(enable_ids)));
  nets[1] = symbol_fsm(self_id);
  nets[2] = sigma_star_fsm();

  constr = intersect(constr,
		     negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
				DONT_COPY, DO_MINIMIZE));

  free_alph(enable_ids);
  free_alph(disable_ids);

  return(constr);
}

static void flags_to_epsilons(ALPHABETptr flags, NETptr net)
{
  STATEptr state;
  ARCptr arc;
  LABELptr lab;
  uint16 i;
  id_type upper, lower, tuple_id;
  ALPHABETptr labels, sig = label_to_binary(sigma(net));

  /* Coerce the label alphabet temporarily into binary format
     for easier maintenance.
     */

  labels = label_to_binary(NET_labels(net));

  /* Eliminate all the flag diacritics in question from the sigma. */

  for (i = 0; i < ALPH_len(flags); i++)
  {
    sigma_remove_from(sig, ALPH_item(flags, i));
    sigma_remove_from(labels, ALPH_item(flags, i));
  }

  /* Coerce flags binary format for faster membership checking .*/

  label_to_binary(flags);

  for (state = NET_states(net); state; state = next_state(state))
    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
    {
      lab = id_to_label(ARC_label(arc));

      if (LABEL_arity(lab) == 1)
	{
	  if (binary_sigma_member(flags, LABEL_id(lab)))
	    {
	      /* Already removed from the sigma and labels alphabets */
	      ARC_label(arc) = EPSILON;
	      NET_epsilon_free(net) = FALSE;
	      STATE_deterministic(state) = FALSE;
	    }
	}
      else
	{
	  upper = fstpair_upper(lab);
	  lower = fstpair_lower(lab);

	  if (binary_sigma_member(flags, upper))
	    {
	      if (binary_sigma_member(flags, lower))
		{
		  sigma_remove_from(labels, ARC_label(arc));
		  ARC_label(arc) = EPSILON;
		  STATE_deterministic(state) = FALSE;
		  NET_epsilon_free(net) = FALSE;
		}
	      else
		{
		  sigma_remove_from(labels, ARC_label(arc));
		  make_fstlabel(&tuple_id, EPSILON, lower);
		  ARC_label(arc) = tuple_id;
		  if (lower == EPSILON)
		    NET_epsilon_free(net) = FALSE;
		  else
		    sigma_add_to(labels, ARC_label(arc));
		  STATE_deterministic(state) = FALSE;
		}
	    }
	  else if (binary_sigma_member(flags, lower))
	    {
	      sigma_remove_from(labels, ARC_label(arc));
	      make_fstlabel(&tuple_id, upper, EPSILON);
	      ARC_label(arc) = tuple_id;
	      if (upper == EPSILON)
		NET_epsilon_free(net) = FALSE;
	      else
		sigma_add_to(labels, ARC_label(arc));
	      STATE_deterministic(state) = FALSE;
	    }
	}
    }
 
  /* Restore label alphabet to standard format */

  binary_to_label(labels);

  NET_deterministic(net) = FALSE;
  NET_minimized(net) = FALSE;
}


static int attr_flag_test(id_type id, id_type attr, ALPHABETptr flags,
			  id_type *clear_id)
{
  /* Returns TRUE of FALSE depending on whether ID is a flag diacritic
     involving ATTR. If it is, the value is collected in FLAGS.
     Sets *CLEAR_ID to the id of the flag diacritic that clears
     the value of the attribute.
     */

  FLAG_DIACRptr flag;

  if ((flag = flag_diacritic(id)) && FLAG_DIACR_attribute(flag) == attr)
    {
      ALPH_item(flags, id) = 1;
      if (FLAG_DIACR_action(flag) == CLEAR_SETTING)
	*clear_id = id;
      return(TRUE);
    }
  else
    return(FALSE);
}



NETptr eliminate_flag_attribute(NETptr net, id_type attr, int reclaim_p,
				int minimize_p)
{
  /* Compiles the constraints involving ATTR, intersects them with
     the network and eliminates the corresponding flag arcs, now
     redundant, from the network.
     */

  ALPHABETptr all_flags;
  LABELptr lab;
  FLAG_DIACRptr flag;
  NETptr constr, nets[2];
  NVtype nv;
  int upper_flags_p = FALSE, lower_flags_p = FALSE;
  uint16 i, alph_len = ALPH_len(sigma(net));
  id_type  id, clear_id = 0;

  if (!(flag_attribute_occurs_in_p(net, attr)))
    return(net);

  /* Collect all the flag diacritics containing ATTR */
  
  all_flags = make_alph(alph_len, BINARY_VECTOR);

  for (i = 0; i < ALPH_len(NET_labels(net)); i++)
    {
      lab = id_to_label(ALPH_item(NET_labels(net), i));

      switch (LABEL_arity(lab))
	{
	case 2:
	  /* Note: we allow flags on both sides of the network. */

	  if (attr_flag_test(fstpair_upper(lab), attr, all_flags, &clear_id))
	    upper_flags_p = TRUE;
	  if (attr_flag_test(fstpair_lower(lab), attr, all_flags, &clear_id))
	    lower_flags_p = TRUE;
	  break;
	case 1:
	  /* The upper/lower_flag_p distinction is irrelevant for
	     flag diacritics that are not part of an fstpair. */
	  
	  attr_flag_test(LABEL_id(lab), attr, all_flags, &clear_id);
	  break;
	default:
	  handle_error("Odd label arity", "ELIMINATE_FLAG_ATTRIBUTE",
		       LABEL_arity(lab));
	  break;
	}
    }

  /* Convert to LABEL_VECTOR format */

  binary_to_label(all_flags);

  /*  print_alphabet_s(all_flags, 72, 0, FALSE, stdout); */

  /* Collect and intersect the constraints for each value of
     the attribute. Initially there is no constraint, i.e.
     the constraint net is the universal language. */

  constr = sigma_star_fsm();

  for (i = 0; i < ALPH_len(all_flags); i++)
    {
      id = ALPH_item(all_flags, i);
      flag = flag_diacritic(id);

      switch (FLAG_DIACR_action(flag))
	{
	case CLEAR_SETTING:
	case POSITIVE_SETTING:
	case NEGATIVE_SETTING:
	  /* No action required */
	  break;
	case UNIFY_TEST:
	  constr = intersect(constr,
			     unify_test(id, clear_id, all_flags, alph_len));
	  break;
	case DISALLOW_TEST:
	  constr = intersect(constr,
			     disallow_test(id, clear_id, all_flags, alph_len));
	  break;
	case REQUIRE_TEST:
	  constr = intersect(constr,
			     require_test(id, clear_id, all_flags, alph_len));
	  break;
	case INSERT_SUBNET:
	  not_yet_implemented("INSERT");
	  break;
	default:
	  handle_error("Unknown flag action", "ELIMINATE_FLAG_ATTRIBUTE",
		       FLAG_DIACR_action(flag));
	  break;
	}
	
    }

  /* If the constraint network encodes the universal language
     there is no work to do here. This could happen if, for
     example, the attribute occurs only with one value.
     Since the original net is kept, we free its now obsolete
     flag register (if any).
     */

  if (sigma_star_fsm_p(constr))
    {
      free_network(constr);
      free_alph(NET_flag_register(net));
      NET_flag_register(net) = NULL;
    }
  else
    {
      /* Compose the constraint and the network */

      /* The values of upper_flags_p and lower_flags_p determine
	 which side of the network need to be composed with the constraint.
	 If only one of them is TRUE, we compose on that side. If both
	 are true, the constraint needs to be composed on both sides of
	 the network. It seems unlikely that such cases arise in practice
	 but it is not inconceivable or incorrect. If upper_flags_p and
	 lower_flags_p are both FALSE, the flag diacritics all constitute
	 atomic symbols (i.e. identity pairs). In that case, we compose
	 on the lower side.
	 */

      nv.nets = nets;
      nv.len = 2;

      if (lower_flags_p)
	{
	  if (upper_flags_p)
	    {
	      /* Need to compose constraints on both sides. Do
		 the upper side first and set up to the default lower
		 side composition in the next step.
		 */

	      nets[0] = copy_fsm(constr);
	      nets[1] = net;
	      nets[0] = compose_fst(&nv, reclaim_p, minimize_p, DONT_CLEANUP,
				    PAIRWISE, FLAGS_NOT_SPECIAL);
	      nets[1] = constr;
	    }
	  else
	    {
	      /* Lower side only */
	      nets[0] = net;
	      nets[1] = constr;
	    }
	}
      else 
	{
	  /* All flags are identity pairs. Composing on the lower side
	     by default. Side does not matter.
	     */
	  nets[0] = net;
	  nets[1] = constr;
	}

      net = compose_fst(&nv, reclaim_p, minimize_p, DONT_CLEANUP, PAIRWISE,
			FLAGS_NOT_SPECIAL);
    }

  /* At least prune the net if it is not being minimized */

  prune_fsm(net, DONT_COPY);

  flags_to_epsilons(all_flags, net);

  free_alph(all_flags);

  if (minimize_p == DO_MINIMIZE)
    min_fsm(net, DONT_COPY);

  check_for_flags(net);

  return(net);
}

/**************
 *            *
 *  NETSUBST  *
 *            *
 **************/


static ARCptr splice_in (NETptr target, NETptr insert, STATEptr state,
			ARCptr prev, ARCptr arc, ARCptr next, int type)
{
  STATEptr dest = ARC_destination(arc);
  id_type arc_id = ARC_label(arc);
  NETptr ins = copy_fsm(insert);

  if (prev == NULL)
    STATE_arc_set(state) = next;
  else
    next_arc(prev) = next;
                  
  NET_num_arcs(target)--;
  free_arc(arc);
                  
  switch(type)
    {
    case 1:  /* upper-side match */
      ins = crossproduct_fst(ins, symbol_fsm(lower_id(arc_id)), DO_RECLAIM,
			     DO_MINIMIZE);
      break;
    case 2:  /* lower-side match */
      ins = crossproduct_fst(symbol_fsm(upper_id(arc_id)), ins, DO_RECLAIM,
			     DO_MINIMIZE);
      break;
    default: /* direct match */
      break;
    }

  if (!(sigma_member(NET_labels(ins), arc_id)))
    sigma_remove_from(NET_labels(target), arc_id);

  splice_fsm (target, state, dest, ins, FALSE, arc_id);

  /* If the first arc of the state is replaced, i.e. prev == NULL, 
     need to reset prev to the last of the newly inserted arcs.
     splice_fsm() adds the new arcs to the top of the state's arc set.
     Therefore the last of the new arcs is the one pointing to
     the next of the old arcs. If insert is a NULL fsm, no arcs
     have been added.
     */

  if (prev == NULL)
    {
      if (STATE_arc_set(state) != next)
        {
          /* New arcs have been added. */

          if ((prev = STATE_arc_set(state)))
            while (next_arc(prev) != next)
              prev = next_arc(prev);
        }
    }
  return(prev);
}


NETptr netsubst(id_type sym_id, NETptr insert, NETptr target,
                int copy_target_p, int copy_insert_p, int minimize_p,
		int expand_other_p)
{
  /* Replaces arcs with SYM_ID in TARGET by splicing INSERT between
     the origin and destination of the arc.
     */
    
    target = min_fsm(target, copy_target_p);
  
  if (sigma_member(sigma(target), sym_id))
    {
      STATEptr state;
      ARCptr arc, next, prev;

      /* Don't understand the reason for testing whether OTHER is in the
	 sigma. Decomissioning this test. L.K. 1.11.97

      if (copy_insert_p == DO_COPY && sigma_member(sigma(insert), OTHER))
        insert = copy_fsm(insert);
	*/

      if (copy_insert_p == DO_COPY)
        insert = copy_fsm(insert);

      if (!(sigma_member(sigma(insert), sym_id)))
	sigma_remove_from(sigma(target), sym_id);

      /* The label alphabet now maintained in splice_in() */

      /*
	if (!(sigma_member(NET_labels(insert), sym_id)))
	  sigma_remove_from(NET_labels(target), sym_id);
	*/

      if (expand_other_p)
	align_sigmas(target, insert);
      
      for (state = NET_states(target);
	   state != NULL;
	   state = next_state(state))
        {
          prev = NULL;
          for (arc = STATE_arc_set(state); arc != NULL; arc = next)
            {
              next = next_arc(arc);
              
              if (ARC_label(arc) == sym_id)
                {
		  prev = splice_in(target, insert, state, prev, arc, next, 0);

                  /* If target is a simple net we can stop after
		   a match because the net is deterministic.
		   after minimization. */

		  if (NET_arc_label_arity(target) == 1)
		     break;

                }
	      else if (tuple_id_p(ARC_label(arc)))
		{
		  if (upper_id(ARC_label(arc)) == sym_id)
		    prev = splice_in(target, insert, state, prev, arc, next, 1);
		  else if (lower_id(ARC_label(arc)) == sym_id)
		    prev = splice_in(target, insert, state, prev, arc, next, 2);
		  else
		    prev = arc;
		}
              else
                prev = arc;
            }
        }
      
      binary_to_label(NET_labels(target));
      
      /* Free the unused copy, or the original, if we didn't copy */    
      free_network((void *) insert);
    }
  
  if (minimize_p == DO_MINIMIZE)
    min_fsm(target, DONT_COPY);

  /* If the net's arcs were previously sorted, this arc ordering is lost.*/
  NET_sorted_arcs(target)= 0;
  
  return(target);
  
} /* netsubst */

#endif /* SLIM_FST */
