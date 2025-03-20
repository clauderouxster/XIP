/* $Id: re-aux.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/***********************************************************
 *
 *			    RE-AUX.C
 *
 *	 Functions definitions for the RE.Y, the source of RE-TAB.C
 *
 *
 ***********************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>

#include "fst/struct.h"
#include "fst/define.h"
#include "fst/pretty-p.h"
#include "fst/copy-fsm.h"
#include "fst/modify.h"
#include "fst/calculus.h"
#include "fst/header.h"
#include "fst/bin-in.h"
#include "fst/bin-out.h"
/* #include "fst/fst-types.h" */
/* #include "fst/interface.h" */
/* #include "fst/regex-in.h" */
#include "fst/varstack.h"
#include "fst/compact.h"
#include "fst/calc-aux.h"
#include "fst/re-aux.h"
/* #include "fst/re.tab.h" */
/* * #include "fst/regex.tab.h" */
#include "fst/fsm_io.h"
#include "fst/replace.h"
#include "fst/fsm-fst-x.h"
#include "fst/types.h"
#include "fst/utf8_fatstr_util.h"

#ifdef MAC_OS
#include <ctype.h>
/* #include "bison.h"  Now obsolete. LK. 98-09-26 */
#endif

/* 
 *  Functions called by action statements in the grammar of
 *	regular expressions.
 */

static int assert_no_def(id_type label_id)
{
  /* Returns TRUE if ID is the id of a label that has not been defined
     to represent a network.  Otherwise complains and returns FALSE.
     */
  if (def_value_p(label_id))
    {
      fsm_fprintf(stderr, "***Illegal use of '");

      FST_fprint_fat_string(stderr, LABEL_atom(id_to_label(label_id)));

         fsm_fprintf(stderr, "' as an ordinary label.\n");
      return(FALSE);
    }
  else
    return(TRUE);
}

void new_sym(ALPHABETptr alph, id_type label_id)
{
  /* Adds LABEL_ID to ALPH, if it is an ordinary label, that is, a label
     without a definition.  If LABEL_ID is not an ordinary label, it is
     not added and an error message is printed.
     */
  if (assert_no_def(label_id))
  {
      /* sigma_add_to does not add EPSILON */

    if (label_id == EPSILON)
        ALPH_item(alph, label_id) = 1;
    else
        sigma_add_to(alph, label_id);

  }
}

NETptr sym_subst(NETptr target, id_type label_id, ALPHABETptr alph)
{
  NETptr net;

  binary_to_label(alph);

  if (assert_no_def(label_id))
  {
      net = symsubst(label_id, alph, target, DONT_COPY, FALSE);
      compact_sigma(net);
  }
  else
    net = target;
  
  free_alph(alph);

  return(net);
}

NETptr net_subst(NETptr target, id_type label_id, NETptr insert)
{
  if (assert_no_def(label_id))
    return(netsubst(label_id, insert, target, DONT_COPY, DONT_COPY,
		    DO_MINIMIZE, FALSE));
  else
    return(target);
}


#if NOT_USED
static void complement_undefined_msg(void)
{
	fsm_fprintf(stderr, "The complement operator ~ is defined only for ");
	fsm_fprintf(stderr, "regular languages:\n");
	fsm_fprintf(stderr, "~A denotes the language [?* - A]. You must use the ");
	fsm_fprintf(stderr, "expression\n");
	fsm_fprintf(stderr, "[[?:?]* - A] instead of ~A if A represents a relation.\n");
}
#endif /* NOT_USED */

NETptr negate_lab(id_type id)
{
  /* Relying on the yacc grammar, reg.y, to guarantee that ID is a 
     single symbol, not a pair. */

  NETptr net = def_value(id);
  
  if (net == NULL)
    return(negate_fsm(symbol_fsm(id), DONT_COPY, DONT_MINIMIZE));
  else
    return(negate_fsm(net, DONT_COPY, DONT_MINIMIZE));
}

NETptr shuffle(NETptr net1, NETptr net2)
{
  NVtype nv;
  NETptr nets[2];
  
  nv.len = 2;
  nv.nets = nets;
  
  nets[0] = net1;
  nets[1] = net2;
  
  return(shuffle_fsm(&nv, DO_RECLAIM, DO_MINIMIZE));
}

static void term_complement_undefined_msg(void)
{
  fsm_fprintf(stderr, "The term complement operator \\ is defined only for ");
  fsm_fprintf(stderr, "regular languages:\n");
  fsm_fprintf(stderr, "\\A denotes the language [? - A]. You must use the ");
  fsm_fprintf(stderr, "expression\n");
  fsm_fprintf(stderr, "[?:? - A] instead of \\A if A represents a relation.\n");
}

NETptr other_than(NETptr net)
{
  /*
    Implements the complement operator \. In the regular expression
    calculus \A is equivalent to [? - A].  This definition requires
    that A be a language rather than a relation. If we allowed \ to
    apply to relations, we would have to define it as equivalent
    to [?:? - A]. This would have the disadvantage that \a, for example,
    would denote the relation containing
        a:?  - a mapped to some unknown symbol,
        ?:a  - some unknown symbol mapped to a,
        ?:?  - any unknown symbol mapped to some other unknown symbol, and
	 ?   - any unknown symbol mapped to itself.
    This more general notion of term complement would be applicable
    to any equal length relation. However, it does not seem to be
    useful in practice. Consequently, we restrict \ to simple languages
    and define \A as just [? - A]. */

  NETptr result, other_net;
  
  if (NET_arc_label_arity(net) > 1 &&
      FSM_MODE == STANDARD_MODE)
    {
      term_complement_undefined_msg();
      free_network(net);
      return(null_fsm());
    }

  other_net = symbol_fsm(OTHER);
  result = minus_fsm(other_net, net, DO_RECLAIM, DO_RECLAIM, DONT_MINIMIZE);

  /* If NET is a transducer, the result's sigma may have redundant
     symbols. */

  if (NET_arc_label_arity(net) > 1)
    compact_sigma(result);

  return(result);

}

NETptr other_than_lab(id_type id)
{
  /* Returns the term complement, that is, the language of all
     single symbol strings other than ID. Implements the regular
     expression operator \. In the regular expression calculus
     \A is equivalent to [? - A].  Note that this definition
     presupposes that A represents a simple language. That is,
     ID must not be a symbol pair. See the comment on the
     function other_than().
  */

  NETptr net = def_value(id);
  
  if (net == NULL)
    {
      if (tuple_id_p(id) &&
	  FSM_MODE == STANDARD_MODE)
	{
	  term_complement_undefined_msg();
	  return(null_fsm());
	}
      else if (id == OTHER)
	net = null_fsm();
      else
	{
	  net = symbol_fsm(OTHER);
	  sigma_add_to(sigma(net), id);
	}
      return(net);
    }
  else
    return(other_than(net));
}


NETptr exactly_one_OLD(NETptr net)
{
  NVtype nv;
  NETptr nets[3];
  
  nv.len = 3;
  nv.nets = nets;
  
  nets[0] = negate_fsm(contains_fsm(net, DO_COPY, DONT_MINIMIZE),
		       DONT_COPY, DO_MINIMIZE);
  nets[1] = net;
  
  nets[2] = copy_fsm(nets[0]);
  
  return(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE));
}

NETptr exactly_one(NETptr net)
     /* 
	The old version (up to fst 7.3.8) did not take into consideration the
	case of languages in { L^2 } (like "[ a a ]"). 
	
	It can be done by excluding the (quite) special case where a network
	is represented something like 
	... first-half_NET NET second_half_NET ...
	leading to recognizing, incorrectly,  [NET NET].
	The solution used is:
	regex [ ~$NET NET ~$NET ] - $[ NET^2 ] ;
	TG July 2000
	*/
{
  NVtype nv;
  NETptr nets[3], has_square;
  
  nv.len = 3;
  nv.nets = nets;
  
  nets[0] = negate_fsm(contains_fsm(net, DO_COPY, DONT_MINIMIZE),
		       DONT_COPY, DO_MINIMIZE);
  nets[1] = net;
  
  nets[2] = copy_fsm(nets[0]);

  has_square = contains_fsm(repeat_fsm(min_fsm(net,DO_COPY), 2, 2, DONT_COPY, DO_MINIMIZE), 
				 DONT_COPY, DONT_MINIMIZE);
  
  return (minus_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
		    has_square,
		    DO_RECLAIM, DO_RECLAIM, DO_MINIMIZE));
}

NETptr exactly_one_lab(id_type id)
{
  return(exactly_one(label_to_net(id)));
}

NETptr at_most_one_OLD(NETptr net)
{
  NVtype nv;
  NETptr nets[3];
  
  nv.len = 3;
  nv.nets = nets;
  
  nets[0] = negate_fsm(contains_fsm(net, DO_COPY, DONT_MINIMIZE),
		       DONT_COPY, DO_MINIMIZE);
  nets[1] = optional_fsm(net, DONT_COPY, DONT_MINIMIZE);
  
  nets[2] = copy_fsm(nets[0]);
  
  return(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE));
}

NETptr at_most_one(NETptr net)
     /* 
	The old version (up to fst 7.3.8) did not take into consideration the
	case of languages in { L^2 } (like "[ a a ]"). 
	
	It can be done by excluding the (quite) special case where a network
	is represented something like 
	... first-half_NET NET second_half_NET ...
	leading to recognizing, incorrectly,  [NET NET].
	The solution used is:
	regex [ ~$NET [ NET | [] ] ~$NET ] - $[ NET^2 ] ;
	TG July 2000
	*/
{
  NVtype nv;
  NETptr nets[3], has_square;
  
  nv.len = 3;
  nv.nets = nets;
  
  has_square = contains_fsm(repeat_fsm(min_fsm(net,DO_COPY), 2, 2, DONT_COPY, DO_MINIMIZE), 
			    DONT_COPY, DONT_MINIMIZE);

  nets[0] = negate_fsm(contains_fsm(net, DO_COPY, DONT_MINIMIZE),
		       DONT_COPY, DO_MINIMIZE);
  nets[1] = optional_fsm(net, DONT_COPY, DONT_MINIMIZE);
  
  nets[2] = copy_fsm(nets[0]);

  return (minus_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
		    has_square,
		    DO_RECLAIM, DO_RECLAIM, DO_MINIMIZE));
}

NETptr at_most_one_lab(id_type id)
{
  return(at_most_one(symbol_to_fsm(id)));
}


NETptr contains_set(NVptr net_seq)
{
  return(contains_all(net_seq, DONT_COPY, DO_MINIMIZE));
}

NETptr precedes(NETptr net1, NETptr net2)
{
  /* Returns a net that accepts any string that doesn't
     contain a string from NET2 preceding a string from
     NET1.
     */
  
  NVtype nv;
  NETptr net, nets[2];
  
  nv.len = 2;
  nv.nets = nets;
  
  nets[0] = contains_fsm(net2, DONT_COPY, DONT_MINIMIZE);
  nets[1] = contains_fsm(net1, DONT_COPY, DONT_MINIMIZE);
  
  net = concat_fsm(&nv, DONT_COPY, DONT_MINIMIZE);
  
  return(negate_fsm(net, DONT_COPY, DONT_MINIMIZE));
}

NETptr follows(NETptr net1, NETptr net2)
{
  /* Returns a net that accepts any string that doesn't
     contain a string from NET1 preceding a string from
     NET2.
     */
  NVtype nv;
  NETptr net, nets[2];
  
  nv.len = 2;
  nv.nets = nets;
  
  nets[0] = contains_fsm(net1, DONT_COPY, DONT_MINIMIZE);
  nets[1] = contains_fsm(net2, DONT_COPY, DONT_MINIMIZE);
  
  net = concat_fsm(&nv, DONT_COPY, DONT_MINIMIZE);
  
  return(negate_fsm(net, DONT_COPY, DONT_MINIMIZE));
}


NETptr ignore_intern_fsm(NETptr net1, NETptr net2, int copy1_p, int copy2_p, int min_p)
{
	/* Introduces a temporary symbol in internal positions of NET1 and then
	   splices in NET2 for every arc labeled with this special symbol.
	    */
	
	ALPHABETptr alph = make_alph(1, LABEL_VECTOR);
	NETptr net;
	
	ALPH_item(alph, 0) = DUMMY_LABEL;
	ALPH_len(alph) = 1;
	
	if (copy1_p == DO_COPY)
		net1 = copy_fsm(net1);
		
	if (copy2_p == DO_COPY)
		net1 = copy_fsm(net2);
		
	sigma_add_to(NET_sigma(net1), DUMMY_LABEL);
	sigma_add_to(NET_sigma(net2), DUMMY_LABEL);
	align_sigmas(net1, net2);
	
	net = ignore_alph(net1, alph, DONT_COPY, DO_MINIMIZE, INTERNALLY, BOTH_SIDES, FALSE);

	net = netsubst(DUMMY_LABEL, net2, net, DONT_COPY, DONT_COPY, min_p,
					FALSE);
					
	sigma_remove_from(NET_sigma(net), DUMMY_LABEL);

	free_alph(alph);
	return(net);

}


#if NOT_USED
static NETptr old_ignore_intern_fsm
  (NETptr net1, NETptr net2, int copy1_p, int copy2_p, int min_p)
{
  /*  This function has been retired. The subtraction introduces
  	  errors in some cases such as [[a b c]* (e (f g))] ./. [a b c].
  	  1998-11-17. LK.
  
   Fixed a longstanding dorman but in this function. The previous version
     had the logic [NET1/NET2 - [NET2 ?*] - [?* NET2]]. This gives the
     wrong result in the case that NET1 and NET2 have a non-empty intersection.
     For example,  [a/a - [?* a] - [?* a]], gives a null fsm. This is incorrect
     because the intent of ignore_intern_fsm is to preserve the original
     content of NET1 and allow the language of NET2 to occur freely internally
     in the strings of NET1. The new logic of this operation in the general case
     is
     
     [NET1/NET2 - [NET2 ?*] - [?* NET2]] | NET1
     
     but we can omit the union with NET1 if the labels of NET2 do not occur in NET1;
     
     1998-11-11 L.K.
  
  */
  
  NVtype nv;  NETptr nets[2], beg, end, n;
  int union_p = FALSE;
  int i;
  
  binary_to_label(NET_labels(net1));
  binary_to_label(NET_labels(net2));
  for (i = 0; i < ALPH_len(NET_labels(net2)); i++)
  	if (sigma_member(NET_labels(net1), ALPH_item(NET_labels(net2), i)))
  	{
  		union_p = TRUE;
  		break;
  	}
  

  nv.len = 2;
  nv.nets = nets;
  
  nets[0] = copy_fsm(net2);
  nets[1] = sigma_star_fsm();
  beg = concat_fsm(&nv, DONT_COPY, DONT_MINIMIZE);

  nets[0] = sigma_star_fsm();
  nets[1] = copy_fsm(net2);
  end = concat_fsm(&nv, DONT_COPY, DONT_MINIMIZE);

  n = ignore_fsm(net1, net2, DO_COPY, copy2_p, DONT_MINIMIZE);
  n = minus_fsm(n, beg, DO_RECLAIM, DO_RECLAIM, DONT_MINIMIZE);
  n = minus_fsm(n, end, DO_RECLAIM, DO_RECLAIM, min_p);
  
  if (union_p)
  {
  	/* We may have lost some strings from NET1 */
  	
  	nets[0] = n;
  	if (copy1_p == DO_COPY)
  		nets[1] = copy_fsm(net1);
  	else
  		nets[1] = net1;
  
  	n = union_fsm(&nv, DONT_COPY, DONT_MINIMIZE);
  }

  return(n);
}
#endif /* NOT_USED */


NETptr ignore_internally(NETptr net1, NETptr net2)
{
  return(ignore_intern_fsm(net1, net2, DONT_COPY, DONT_COPY, DONT_MINIMIZE));
}


NETptr starts_with(NETptr net)
{
    /* Note: net is reclaimed! */

  return(min_fsm(concat(net, sigma_star_fsm()), DONT_COPY));
}

NETptr ends_with(NETptr net)
{
    /* Note: net is reclaimed! */

  return(min_fsm(concat(sigma_star_fsm(), net), DONT_COPY));
}


NETptr starts_with_label(id_type label)
{
    return(starts_with(symbol_fsm(label)));
}


NETptr ends_with_label(id_type label)
{
    return(ends_with(symbol_fsm(label)));
}

NETptr oneside_minus (NETptr net1, NETptr net2, int side)
{
  return (oneside_minus_fsm (net1, net2, side,
			     DO_RECLAIM, DO_RECLAIM, DONT_MINIMIZE));
}



NETptr slice(NETptr net, char *layer)
{
  if (! strcmp(layer, "1"))
    return (upper_fsm(net, DONT_COPY, DO_MINIMIZE));
  else if (! strcmp(layer, "2"))
    return (lower_fsm(net, DONT_COPY, DO_MINIMIZE));

  /* the two following operations are not a slicing, but this is the
     best place where to add them. [AK,27/10/95] */

  else if (! strcmp(layer, "i"))
    return (invert_fst (net, DONT_COPY));
  else if (! strcmp(layer, "r"))
    return (reverse_fsm (net, DONT_COPY, DO_MINIMIZE));

  else
    {
      fsm_fprintf(stderr, "*** %s is not valid:\n", layer);
      fsm_fprintf(stderr, "\t1 = upper, 2 = lower, i = invert, r = reverse\n");
      return(net);
    }
}

/****************************************
 *
 *  FUNCTIONS FOR NET VECTORS AND NETWORKS
 *
 ***********************************/


NETptr copy_non_null_fsm(NETptr net)
{
  /* Return a copy of net if not NULL. Otherwise returns NULL. */

  if (net == NULL)  return (NULL);

  return (copy_fsm (net));
}


NVptr NV_select_copy_nets(NVptr nv1, NVptr nv2, int start, int step)
{
  /* Selects and copies some of the networks from nv1 to nv2.
     Begins with net[start] and proceeds in steps of 'step'.
     If start<0 : Counted from the end.
     If step<0 : Reversed order.

     The NETptr's are allowed to be NULL.
     */

  NETptr net;
  int i, k, max = NV_len(nv2);

  if (start<0)  start += NV_len(nv1);

  for (i=0, k=start; i<max; i++, k+=step)
    {
      net = NV_net(nv1, k);
      if (net)  net = copy_fsm(net);
      NV_net(nv2, i) = net;
    }

  return(nv2);
}


NVptr NV_select_non_null_nets(NVptr nv, int copy_p)
{
  /* Puts all non-NULL nets from nv to nv2.
     If all nets are NULL then NULL is returned. */

  NVptr nv2; NETptr net; int i, i2, len, len2;

  len = NV_len(nv);                  /* count non-NULL nets */
  for (i=0, len2=0; i<len; i++)  if (NV_net(nv, i))  len2++;

  if (! len2)  return (NULL);

  nv2 = make_nv(len2);

  for (i=0, i2=0; i<len; i++)
    {
      net = NV_net(nv, i);
      if (! net)  continue;

      if (copy_p == DO_COPY)  net = copy_fsm(net);
      NV_net(nv2, i2++) = net;
    }

  return (nv2);
}


NETptr union_null_fsm(NVptr nv, int copy_p, int minimize_p)
{
  NVptr nvA; NETptr net;

  nvA = NV_select_non_null_nets(nv, copy_p);
  if (! nvA)  return (NULL);

  net = union_fsm(nvA, DONT_COPY, minimize_p);
  free_nv_only(nvA);

  return (net);
}


NETptr intersect_null_fsm(NVptr nv, int reclaim_p, int minimize_p)
{
  NVptr nvA; NETptr net; int copy_p;

  if (reclaim_p == DO_RECLAIM)  copy_p = DONT_COPY;
  else  copy_p = DO_COPY;

  nvA = NV_select_non_null_nets(nv, copy_p);
  if (! nvA)  return (NULL);

  net = intersect_fsm(nvA, DO_RECLAIM, minimize_p);
  free_nv_only(nvA);

  return (net);
}


NETptr compose_null_fst(NVptr nv, int reclaim_p, int minimize_p,
			int flags_special_p)
{
  NVptr nvA; NETptr net; int copy_p;

  if (reclaim_p == DO_RECLAIM)  copy_p = DONT_COPY;
  else  copy_p = DO_COPY;

  nvA = NV_select_non_null_nets(nv, copy_p);
  if (! nvA)  return (NULL);

  net = compose_fst(nvA, DO_RECLAIM, minimize_p, DO_CLEANUP, PAIRWISE,
		    flags_special_p);
  free_nv_only(nvA);

  return (net);
}


int NV_fst_check(NVptr nv)
{
  /* Returns TRUE if there is an fst among the nets of nv. */

  NETptr net;
  int i, len = NV_len(nv);

  for (i=0; i<len; i++)
    {
      net = NV_net(nv, i);
      if (net && NET_arc_label_arity(net) > 1)	return(TRUE);
    }

  return(FALSE);
}


NVptr NV_2_nets_to_nv(NETptr net1, NETptr net2)
{
  /* Makes a net vektor of two networks */

  NVptr nv;

  nv = make_nv(2);
  NV_net(nv,0) = net1;  NV_net(nv,1) = net2;
  return(nv);
}


/***************************************
 *
 *   Replacement and Restriction
 *
 **************************/

RSptr REAUX_rs_append (RSptr rs1, RSptr rs2)
{
  return (rs_append (rs1, rs2));
}

RSptr REAUX_rs_upper_lower (NETptr upper, NETptr lower, int arrow)
{
  return (rs_upper_lower (upper, lower, arrow));
}

NETptr REAUX_rs_left_right(NETptr left, NETptr right)
{
  return (rs_left_right(left, right));
}

RSptr REAUX_crossprod_uplow_leftright
  (RSptr uplow, NVptr leftright, int ctxdir)
{
  return (crossprod_uplow_leftright(uplow,leftright,ctxdir, DO_RECLAIM));
}

/*--------------------------------------------------------*/
NETptr REAUX_rs_set_single_epsilon (NETptr net)
{
  return (rs_set_single_epsilon (net));
}

NETptr REAUX_rs_reset_single_epsilon (NETptr net)
{
  return (rs_reset_single_epsilon (net));
}

/*--------------------------------------------------------*/
NETptr REAUX_uncond_replace (RSptr uplow)
{
  return (uncond_replace(uplow, DO_RECLAIM));
}

NETptr REAUX_conditional_replace (RSptr repl_rs)
{
  return (conditional_replace (repl_rs, DO_RECLAIM));
}

/*--------------------------------------------------------*/

NETptr REAUX_restrict_in_multctx (NETptr center, NVptr ctxlist, int arrow)
{
  return (RST_restrict_in_multctx (center, ctxlist, arrow, DO_RECLAIM));
}

/*
NETptr CPDR_cond_determ_replace (RSptr repl_rs);
*/

