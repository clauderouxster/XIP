/* $Id: replace.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/*
#include <stddef.h>
#include <ctype.h>
#include <time.h>
*/

#include "fst/types.h"
#include "fst/struct.h"
#include "fst/calculus.h"
#include "fst/modify.h"
#include "fst/varstack.h"
#include "fst/fsm_io.h"
#include "fst/calc-aux.h"
#include "fst/re-aux.h"
#include "fst/pars-tbl.h"
#include "fst/pretty-p.h"

/*
#include "fst/define.h"
#include "fst/pretty-p.h"
#include "fst/copy-fsm.h"
#include "fst/modify.h"
#include "fst/header.h"
#include "fst/bin-in.h"
#include "fst/bin-out.h"
#include "fst/fst-types.h"
#include "fst/interface.h"
#include "fst/compact.h"
*/
#include "fst/replace.h"

#include "fst/utf8_fatstr_util.h"

#ifdef MAC_OS
#include <ctype.h>
/* #include "bison.h" */
#endif


/* The networks in the replace_structure 'uplow' and in the net_vector
   'leftright' (from conditional and unconditional parallel replacement)
   can be preprocessed in any particular application, if the application
   defines and assigns functions to the corresponding global variables
   below. [AK 12/09/97] */

void (*applic_preprocess_uplow)(RSptr uplow) = NULL;
void (*applic_preprocess_leftright)(NVptr leftright) = NULL;

/*----- End of preprocessing -----*/


/* INSERT_SYMBOL will be redefined as intern_literal(INSERT_STRING)
   before it is used. INSERT_STRING is defined in re-aux.h.
   */

static id_type INSERT_SYMBOL = 0;


/****************************************
 *
 *  Auxiliary routines
 *
 *
 ***********************************/


static NETptr force_prune(NETptr net, int copy_p)
{
  /* forces the pruning of a net */

  NET_deterministic(net) = FALSE;
  NET_pruned(net)        = FALSE;
  NET_completed(net)     = FALSE;
  NET_minimized(net)     = FALSE;
  NET_epsilon_free(net)  = FALSE;
  NET_sorted_states(net) = FALSE;
  NET_loop_free(net)     = FALSE;

  return (prune_fsm(net, copy_p));
}

/*********
*
*  RESTRICTION
*
**********/

static NETptr insert_left_ctxboundary(NETptr oldctx, int copy_p)
{
  /* introduces a new start state and links it to the old one
     and to all destinations of arcs labeled with boundary symbols.
     deletes those arcs
     */

  NETptr newctx; STATEptr state, start, new_state;
  ARCptr arc, last_arc, next_arc;

  if(copy_p == DO_COPY) newctx = copy_fsm(oldctx);
  else newctx = oldctx;

  start = NET_start_state(newctx);
  new_state = NULL;

  for(state = NET_states(newctx); state; state = next_state(state))
    {
      last_arc = NULL;
      for(arc = STATE_arc_set(state); arc; arc = next_arc)
	{
	  next_arc = next_arc(arc);
	  if(ARC_label(arc) != BOUNDARY_SYMBOL) { last_arc = arc; continue; }

          if (!new_state)  new_state = make_state (NULL, newctx);

#ifdef FULL_FSM
	  add_new_arc(newctx, new_state, EPSILON, ARC_destination(arc), NULL);
#else
	  add_new_arc(newctx, new_state, EPSILON, ARC_destination(arc));
#endif
	  free_arc(arc); NET_num_arcs(newctx)--;
	  if(last_arc) next_arc(last_arc) = next_arc;
	  else STATE_arc_set(state) = next_arc;
	}
    }

  if(!new_state)  return(newctx);  /* no arcs were deleted or introduced 
			      since no boundary symbol arcs were found*/

#ifdef FULL_FSM
  add_new_arc(newctx, new_state, EPSILON, start, NULL);
#else
  add_new_arc(newctx, new_state, EPSILON, start);
#endif

  NET_start_state(newctx) = new_state;

  sigma_remove_from(NET_sigma(newctx), BOUNDARY_SYMBOL);  
  label_remove_from(NET_labels(newctx), BOUNDARY_SYMBOL);

  force_prune(newctx, DONT_COPY);
  return(newctx);
}


static NETptr insert_right_ctxboundary(NETptr oldctx, int copy_p)
{
  /* deletes all arcs labeled with boundary symbols,
     and makes their source states final.
     */

  NETptr newctx; STATEptr state;
  ARCptr arc, last_arc, next_arc;

  if(copy_p == DO_COPY) newctx = copy_fsm(oldctx);
  else newctx = oldctx;

  for(state = NET_states(newctx); state; state = next_state(state))
    {
      last_arc = NULL;
      for(arc = STATE_arc_set(state); arc; arc = next_arc)
	{
	  next_arc = next_arc(arc);
	  if(ARC_label(arc) != BOUNDARY_SYMBOL) { last_arc = arc; continue; }

	  STATE_final(state) = TRUE;

	  free_arc(arc); NET_num_arcs(newctx)--;
	  if(last_arc) next_arc(last_arc) = next_arc;
	  else STATE_arc_set(state) = next_arc;
	}
    }

  sigma_remove_from(NET_sigma(newctx), BOUNDARY_SYMBOL);  
  label_remove_from(NET_labels(newctx), BOUNDARY_SYMBOL);

  force_prune(newctx, DONT_COPY);
  return(newctx);
}


static int fst_check(NETptr net, char *component, char *kind)
{
  if (net && NET_arc_label_arity(net) > 1)
    {
      fsm_fprintf(stdout, "*** Error in  a %s statement. ", kind);
      fsm_fprintf(stdout, "%s must be a simple network, not a transducer.\n",
	      component);
      return(TRUE);
    }
  else
    return(FALSE);
}

static int fst_restrict_p(NETptr center, NETptr left, NETptr right)
{
  int error_p = FALSE;

  if (fst_check(center, "CENTER", "restrict"))
    error_p = TRUE;
  if (fst_check(left, "LEFT", "restrict"))
    error_p = TRUE;
  if (fst_check(right, "RIGHT", "restrict"))
    error_p = TRUE;

  return(error_p);
}

static NETptr no_ctx_boundary_copy(NETptr net)
{
  NETptr copy = copy_fsm(net);

  if (sigma_member(NET_sigma(copy), BOUNDARY_SYMBOL))
    {
      ALPHABETtype alph;
      id_type items[1];

      alph.len = 1;
      alph.max = 1;
      alph.type = LABEL_VECTOR;
      alph.items = items;

      items[0] = EPSILON;
      symsubst(BOUNDARY_SYMBOL, &alph, copy, DONT_COPY, FALSE);
    }
  return(copy);
}


static NETptr restrict_context(NETptr center, NETptr left, NETptr right)
{
   /* Computes

      ~[[?* LEFT] [~CENTER - [CENTER RIGHT ?*] - [?* LEFT CENTER]] [RIGHT ?*]]

      "Between matching a LEFT and RIGHT pair, CENTER must occur."

      The notion 'matching pair' is defined as follows:

      LEFT and RIGHT count as a matching pair in ...LEFT...RIGHT...
      iff (1) there is no intervening RIGHT', ...LEFT...RIGHT'...RIGHT,
              such that LEFT and RIGHT' are a matching pair;
          and
          (2) there is no intervening LEFT', ...LEFT...LEFT'...RIGHT,
              such that LEFT' and RIGHT are a matching pair.
      
      By the definition of <=, LEFT ~CENTER RIGHT is prohibited unless
      the left edge of ~CENTER starts with CENTER RIGHT or the right
      edge of ~CENTER terminates with LEFT CENTER. In either case,
      the outermost LEFT and RIGHT do not match, therefore they do
      not need to have a CENTER between them.

      */

   NVtype nv;
   NETptr net, nets[3];

   nv.nets = nets;
   nv.len = 3;

   /* Compute ~CENTER */
   net = negate_fsm(center, DO_COPY, DO_MINIMIZE);
   
   nets[0] = copy_fsm(center);
   nets[1] = no_ctx_boundary_copy(right);
   nets[2] = sigma_star_fsm();

   /* Subtract [CENTER [RIGHT ?*]] from ~CENTER */
   net = minus_fsm(net, concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                   DO_RECLAIM, DO_RECLAIM, DO_MINIMIZE);

   nets[0] = sigma_star_fsm();
   nets[1] = no_ctx_boundary_copy(left);
   nets[2] = center;

   /* Subtract [[?* LEFT] CENTER] from ~CENTER */
   net = minus_fsm(net, concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                   DO_RECLAIM, DO_RECLAIM, DO_MINIMIZE);

   /* Extend the contexts. Deal with the boundary symbol */

   left = insert_left_ctxboundary(concat(sigma_star_fsm(), left),
                                  DONT_COPY);
   right = insert_right_ctxboundary(concat(right, sigma_star_fsm()),
				     DONT_COPY);

   nets[0] = left;
   nets[1] = net;
   nets[2] = right;
   nv.len = 3;

   /* Finish */
   net = negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                    DONT_COPY, DONT_MINIMIZE);
   return(net);
}

static NETptr restrict_center(NETptr center, NETptr left, NETptr right)
{
    /* Computes ~[~[?* LEFT] CENTER ?*] & ~[?* CENTER ~[RIGHT ?*]]
       The left hand side of the intersection does not need to be
       constructed if [?* LEFT] is equivalent ?*, as it is
       if LEFT accepts the empty string. Similarly, for the right context
       restriction. If [?* LEFT] and [RIGHT ?*] are both sigma
       star networks, the value returned is also a sigma star net,
       which accepts CENTER in any environment.
     */

    NVtype nv;
    NETptr left_restr, right_restr, xleft, xright, nets[3];

    nv.nets = nets;
    nv.len = 3;

    /* First compute the left-side restriction. */

    if (STATE_final(NET_start_state(left)))
    {
        /* No restriction on the left context. */

        left_restr = NULL;
        free_network(left);
    }
    else
    {
        xleft = insert_left_ctxboundary(concat(sigma_star_fsm(), left),
                                        DONT_COPY);

        nets[0] = negate_fsm(xleft,DONT_COPY, DONT_MINIMIZE);
        nets[1] = copy_fsm(center);
        nets[2] = sigma_star_fsm();

        left_restr = negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                                DONT_COPY, DO_MINIMIZE);
    }

    /* Then the right-side restriction. Note that if there isn't any,
       CENTER must be freed since it is not used in the computation
       at all.
       */

    if (STATE_final(NET_start_state(right)))
    {
        /* No restriction on the right context. */

        free_network(right);
        right_restr = NULL;
        free_network(center);  
    }
    else
    {

        xright = insert_right_ctxboundary(concat(right, sigma_star_fsm()),
                                          DONT_COPY);
        nets[0] = sigma_star_fsm();
        nets[1] = center;
        nets[2] = negate_fsm(xright,DONT_COPY, DONT_MINIMIZE);
   
        right_restr = negate_fsm(concat_fsm(&nv, DONT_COPY, DO_MINIMIZE),
                                 DONT_COPY, DO_MINIMIZE);
    }

    /* If both left and right restriction exist, intersect
       them, if only one exists return it, otherwise return
       sigma star.
       */

    if (left_restr && right_restr)
        return(intersect(left_restr, right_restr));
    else if (left_restr)
        return(left_restr);
    else if (right_restr)
        return(right_restr);
    else
        return(sigma_star_fsm());
}

NETptr simple_restriction(NETptr center, NETptr left, NETptr right,
                          int arrow_type, int copy_p)
{
  /*
     CENTER, LEFT, and RIGHT must be simple fsms, not transducers.
     Constrains CENTER, LEFT, and RIGHT with respect to each other.
     The type of constraint depends on the type of arrow. The
     options for arrow_type are:

     0   =>
     1  <=
     2  <=>

     The constraints are defined as follows

     CENTER => LEFT _ RIGHT
       means
             ~[?* CENTER ~[RIGHT ?*]] & ~[~[?* LEFT] CENTER ?*]

     CENTER <= LEFT _ RIGHT
       means
       ~$[LEFT [~CENTER - [CENTER RIGHT ?*] - [? LEFT CENTER]] RIGHT]

     CENTER <=> LEFT _ RIGHT
        means
              [CENTER => LEFT _ RIGHT] & [CENTER <= LEFT _ RIGHT]

     If copy_p is DO_COPY, CENTER, LEFT, and RIGHT are preserved,
     otherwise they are reclaimed in the process.
     */

  NETptr net;

  if (fst_restrict_p(center, left, right))
    {
      fsm_fprintf(stdout, "    Aborting. Returning ?* instead.\n");

      if (copy_p == DONT_COPY)
        {
          free_network(center);
          free_network(left);
          free_network(right);
       }
      net = sigma_star_fsm();
    }
  else
    {
      if (copy_p == DO_COPY)
        {
          if (center)
            center = copy_fsm(center);
          if (left)
            left = copy_fsm(left);
          if (right)
            right = copy_fsm(right);
        }
      
      if (!center)
        center = epsilon_fsm();
      if (!left)
        left = epsilon_fsm();
      if (!right)
        right = epsilon_fsm();

      switch(arrow_type)
      {
      case 0:
         net = restrict_center(center, left, right);
         break;
      case 1:
          net = restrict_context(center, left, right);
          break;
      case 2:
         net = intersect(restrict_center(copy_fsm(center),
                                         copy_fsm(left),
                                         copy_fsm(right)),
                         restrict_context(center, left, right));
          break;
      default:
          net = NULL;
          handle_error("Wrong arrow type", "SIMPLE_RESTRICTION", arrow_type);
          break;
      }
    }

  return(net);
}

/***********************************
*
*  END OF RESTRICTION
*
************************************/




/***********************************
*
*  SIMPLE REPLACEMENT
*
***********************************/


NETptr simple_replacement(NETptr upper, NETptr lower, int copy_p,
			  int optional_p)
{
  /* UPPER -> LOWER returns a simple substitution transducer
     that obligatorily maps upper to lower. Equivalent to
     [~$[UPPER - []] [UPPER .x. LOWER]]* ~$[UPPER - []]
     when OPTIONAL_P is FALSE, otherwise the same as
     [?* [UPPER .x. LOWER]]* ?*
     In other words, any number of instances of UPPER .x. LOWER
     possibly alternating with  sequences not containing
     upper that are mapped to themselves.
     If copy_p is DO_COPY, UPPER and LOWER are preserved,
     otherwise they are reclaimed in the process.
     If IGNORE_ALPH is non-null, all symbols it contains are ignored
     in the substitution part.
     */

  if (NET_arc_label_arity(upper) > 1 ||
      NET_arc_label_arity(lower) > 1)
    {
      fsm_fprintf(stderr, "******* The -> operator cannot be applied ");
      fsm_fprintf(stderr, "to a transducer. *******");
      if (copy_p == DONT_COPY)
        {
          free_network(upper);
          free_network(lower);
       }
      return(null_fsm());
    }
  else
    {
      NVtype nv;
      NETptr nets[2], net;
      NETptr subst, filler;

      nv.nets = nets;
      nv.len = 2;

      if (copy_p == DO_COPY)
        {
          upper = copy_fsm(upper);
          lower = copy_fsm(lower);
        }
	    	
      subst = crossproduct_fst(upper, lower, DONT_RECLAIM, DO_MINIMIZE);
      
      /* We can release lower immediately. */
      
      free_network(lower);

      if (optional_p)
	{
	  /* The optional case is trivial. Upper can be released. */
	  filler = sigma_star_fsm();
	  free_network(upper);
	}
      else
	{
	  /* We simulate the effect of [UPPER - []] by just
	     making sure that the start state of UPPER
	     is nonfinal. But we need to make sure UPPER is
             epsilonfree...
	     */

          epsilon_remove(upper, DONT_COPY);
          STATE_final(NET_start_state(upper)) = FALSE;

	  filler = negate_net(contains_fsm(upper, DONT_COPY, DO_MINIMIZE),
				DONT_COPY, DO_MINIMIZE);
	}
      /* First make [filler [upper .x. lower]]* network.
	 Need to copy filler because we need it twice.
	 */

      nets[0] = copy_fsm(filler);
      nets[1] = subst;

      net = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);
      
      nets[0] = zero_plus(net, DONT_COPY, DONT_MINIMIZE);
      nets[1] = filler;

      /* And now add the final filler */

      net = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

      return(net);
    }
}

/***************************************
*
*  END OF SIMPLE REPLACEMENT
*
****************************************/


/************************************************************************
*
*              UNCONDITIONAL DIRECTED REPLACEMENT
*                          Lauri Karttunen
*                           Sept. 1, 1995
*
*              redone      Aug. 29, 1996
************************************************************************/


static void cut_off_tails(NETptr net, int direction)
{
    STATEptr state;

    /* Minimize because the algorithm depends on that.
       Reverse the argument and the result if the starting
       point is on the right.
       */

    if (direction == 1)
        net = reverse_fsm(net, DONT_COPY, DO_MINIMIZE);
    else
        net = min_fsm(net, DONT_COPY);

    for (state = NET_states(net); state; state = next_state(state))
        if (STATE_final(state))
            if (STATE_arc_set(state))
            {
                ARCptr arc;

                while ((arc = STATE_arc_set(state)))
                {
                    STATE_arc_set(state) = next_arc(arc);
                    free_arc(arc);
                    NET_num_arcs(net)--;
                }

                NET_pruned(net) = FALSE;
                NET_minimized(net) = FALSE;
            }

    if (direction == 1)
        net = reverse_fsm(net, DONT_COPY, DO_MINIMIZE);

    /* NET is now known to be loopfree. */

    NET_loop_free(net) = TRUE;

    min_fsm(net, DONT_COPY);
}

static void check_uppers(NVptr uppers)
{
    int i, j, len = NV_len(uppers);

    for (i = 0; i < len; i++)
        for (j = i+1; j < len; j++)
            NV_net(uppers,i) = minus_fsm(NV_net(uppers, i),
                                         NV_net(uppers, j),
                                         DO_RECLAIM, DONT_RECLAIM,
                                         DO_MINIMIZE);
}


static NETptr map_to_eps(id_type id, NETptr net)
{
    /* Turns ID labels to ID:EPSILON labels */

    int changed_p = FALSE;
    ALPHABETptr labs;
    uint16 i;
    id_type pair;
    STATEptr state;
    ARCptr arc;
    

    make_pair_label(&pair, id, EPSILON);
    for (state = NET_states(net); state; state = next_state(state))
        for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
            if (ARC_label(arc) == id)
            {
                ARC_label(arc) = pair;
                changed_p = TRUE;
            }

    if (changed_p)
    {
       labs = binary_to_label(NET_labels(net));

        for (i = 0; i < ALPH_len(labs); i++)
            if (ALPH_item(labs, i) == id)
            {
                ALPH_item(labs, i) = pair;
                changed_p = FALSE;
                break;
            }
    }

    if (changed_p)
        handle_error("Shouldn't happen", "MAP_TO_EPS", 0);

    return(net);
}

static NETptr extract_prefix(NETptr lower)
{
    NETptr pref = copy_fsm(lower);
    STATEptr state;
    ARCptr arc;
    int found_p = FALSE;
 
    for (state = NET_states(pref); state; state = next_state(state))
    {
        STATE_final(state) = FALSE;
        for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
            if (ARC_label(arc) == INSERT_SYMBOL)
                {
                    STATE_final(state) = TRUE;
                    found_p = TRUE;
                    break;
                }
    }
    assert(found_p);
    NET_pruned(pref) = FALSE;
    NET_minimized(pref) = FALSE;
    min_fsm(pref, DONT_COPY);
    /* We can now remove INSERT_SYMBOL from sigma. It was put there
       in rs_left_righth() to prevent an unwanted expansion of OTHER.
       */
    sigma_remove_from(NET_sigma(pref), INSERT_SYMBOL);
    return(pref);
}
 
static NETptr extract_suffix(NETptr lower)
{
    NETptr suff = copy_fsm(lower);
    STATEptr state;
    ARCptr arc = 0;
 
    for (state = NET_states(suff); state; state = next_state(state))
    {
        for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
            if (ARC_label(arc) == INSERT_SYMBOL)
                break;
        if (arc)
            break;
    }
    assert(state && arc);
    NET_start_state(suff) = ARC_destination(arc);
    NET_pruned(suff) = FALSE;
    NET_minimized(suff) = FALSE;
    min_fsm(suff, DONT_COPY);
    /* We can now remove INSERT_SYMBOL from sigma. It was put there
       in rs_left_right() to prevent an unwanted expansion of OTHER.
       */
    sigma_remove_from(NET_sigma(suff), INSERT_SYMBOL);
    return(suff);
}


static NETptr insert_upper(NETptr upper, NETptr lower, id_type lb, id_type rb)
{
    /* Divides LOWER to two parts at the INSERT_SYMBOL arc
       and splices UPPER between the two parts.
       */
 
    NVtype nv;
    NETptr net, nets[3];
 
    nv.nets = nets;
    nv.len = 3;
 
    nets[0] = product(symbol_fsm(lb), extract_prefix(lower));
    nets[1] = copy_fsm(upper);
    nets[2] = product(symbol_fsm(rb), extract_suffix(lower));

    net = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);
 
    return(net);
}

static NETptr repl_cell(id_type lb, NETptr center, id_type rb, NETptr lower)
{

    /* If the lower net includes the insert symbol, we first divide
       the lower net to prefix and suffix and then splice the center
       net in between the two parts; otherwise we compute the
       crossproduct of the center and the lower net:
       LB:0 [CENTER .x. LOWER] RB:0.
    */

    NETptr net;
    id_type id1, id2;

    if (INSERT_SYMBOL == 0)
      {
	FST_intern_literal(&INSERT_SYMBOL, INSERT_STRING);
      }

    /* Make sure we don't get auxiliary brackets into the
       replace expression by accident. LK 24.09.97 */

    if (sigma_member(NET_sigma(center), OTHER))
      {
	sigma_add_to(NET_sigma(center), lb);
	sigma_add_to(NET_sigma(center), rb);
      }

    if (sigma_member(NET_sigma(lower), OTHER))
      {
	sigma_add_to(NET_sigma(lower), lb);
	sigma_add_to(NET_sigma(lower), rb);
      }

    if (sigma_member(NET_sigma(lower), INSERT_SYMBOL))
        net = insert_upper(center, lower, lb, rb);
    else  
      {
	make_fstlabel(&id1, lb, EPSILON);
	make_fstlabel(&id2, rb, EPSILON);
	net = concat(symbol_fsm(id1), concat(product(copy_fsm(center), copy_fsm(lower)),
	             symbol_fsm(id2)) );
      }
    return(net);
}

static NETptr allow_labs(id_type lab1, id_type lab2)
{
    return(simple_replacement(net_union(symbol_fsm(lab1), symbol_fsm(lab2)),
                              epsilon_fsm(), DONT_COPY, FALSE));
}

static NETptr assemble_replacement(NVptr upper_nets, NVptr lower_nets,
                                   id_type lb, id_type rb, int direction,
                                   int optional_p)
{
    int i, len = NV_len(upper_nets);
    NVtype nv;
    NETptr net, nets[2];

    nv.nets = nets;
    nv.len = 2;

    net = repl_cell(lb, NV_net(upper_nets, 0), rb, NV_net(lower_nets, 0));

    for (i = 1; i < len; i++)
        net = net_union(net, repl_cell(lb, NV_net(upper_nets, i), rb,
                                       NV_net(lower_nets, i)));
    
    switch(direction)
    {
    case 0:
        /* left-to-right:  [[~$[LB] REPL]* ~$[LB]]   */

        nets[0] = optional_p ? allow_labs(lb, rb) : contains_no_label(lb);
        nets[1] = net;
        nets[0] = zero_plus(concat_fsm(&nv, DONT_COPY, DONT_MINIMIZE),
                            DONT_COPY, DO_MINIMIZE);
        nets[1] = optional_p ? allow_labs(lb, rb) : contains_no_label(lb);
        break;
    case 1:
        /* right-to-left: [~$[RB] [REPL ~$[RB]]*]   */
        nets[0] = net;
        nets[1] = optional_p ? allow_labs(lb, rb) : contains_no_label(rb);
        nets[1] = zero_plus(concat_fsm(&nv, DONT_COPY, DONT_MINIMIZE),
                            DONT_COPY, DO_MINIMIZE);
        nets[0] = optional_p ? allow_labs(lb, rb) : contains_no_label(rb);
        break;
    default:
        break;
    }
    net = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);
 
    return(net);

}

static int aux_sym_is_net_sym_error(NETptr net, id_type mk, id_type lb, id_type rb)
{
    if (sigma_member(NET_sigma(net), mk) |
        sigma_member(NET_sigma(net), lb) |
        sigma_member(NET_sigma(net), rb))
    {
        fsm_fprintf(stderr, "*** Sorry. Cannot apply to a net containing: ");
        print_label(mk, stderr, DO_ESCAPE);
        fsm_fprintf(stderr, ", or ");
        print_label(lb, stderr, DO_ESCAPE);
        fsm_fprintf(stderr, ", or ");        
        print_label(rb, stderr, DO_ESCAPE);
        fsm_fprintf(stderr, ".\n");
        return(TRUE);
    }
    else
        return(FALSE);
}

static void ignore_nonfinal_initial(NETptr net, id_type sym)
{
	/* If the start state is final, we cannot add
	a looping arc to it. In that case we make a non-final
	copy of it and point the arc to the new state */

	STATEptr start_state = NET_start_state(net);
	STATEptr dest_state;
	
	if (STATE_final(start_state))
	{
		dest_state = copy_state(start_state, net);
		push_arc(make_arc(sym, dest_state, net), dest_state);
		STATE_final(dest_state) = FALSE;
		NET_minimized(net) = FALSE;
	}
	else
		dest_state = start_state;
		
	push_arc(make_arc(sym, dest_state, net), start_state);
	
	label_to_sigma(NET_sigma(net), sym);
	sigma_add_to(NET_labels(net), sym);
	binary_to_label(NET_labels(net));
}

static void ignore_noninitial_final(NETptr net, id_type sym)
{
	/* Adds a loop to every final state except the start state. */
	
	STATEptr state;
	
	for (state = NET_states(net); state; state = next_state(state))
		if (STATE_final(state) && state != NET_start_state(net))
			push_arc(make_arc(sym, state, net), state);
	label_to_sigma(NET_sigma(net), sym);
	sigma_add_to(NET_labels(net), sym);
	binary_to_label(NET_labels(net));
}

static NETptr directed_replace(NVptr upper_nets, NVptr lower_nets,
                                    int direction, int length_of_match,
                                    int optional_p)
{
    NETptr net, upper, upper1, upper2, aux, im, dir, lm, map;
    int i, empty_string_included_p ;
    id_type mk;
    id_type lb;
    id_type rb;
    id_type pair_id1, pair_id2;
    ALPHABETptr alph;
   
    NVtype nv;
    NETptr nets[4];

    nv.nets = nets;
    FST_intern_literal(&mk, "*^^*");
    FST_intern_literal(&lb, "*<<*");
    FST_intern_literal(&rb, "*>>*");

    /* Check the input. */

    assert((upper_nets && lower_nets &&
            NV_len(upper_nets) == NV_len(lower_nets)) ||
           !(upper_nets || lower_nets));

    if (!upper_nets || NV_len(upper_nets) == 0)
    {
        free_nv_only(upper_nets);
        free_nv_only(lower_nets);
        return(sigma_star_fsm());
    }

    /* Force determinism by making sure that the upper languages
       are disjoint. */

    check_uppers(upper_nets);

    /* Construct the union of all the individual UPPERs. */

    upper = union_fsm(upper_nets, DO_COPY, DO_MINIMIZE);

    /* Make sure the auxiliary symbols don't occur in the net */

    if (aux_sym_is_net_sym_error(upper, mk, lb, rb))
    {

        free_network(upper);
        return(sigma_star_fsm());
    }

    /* If the sigma of upper or lower includes OTHER, add the auxiliary
       symbols to the its sigma so that they are not included
       in the expansion of the OTHER in composition and thus
       get to wrong places.
       */

    if (sigma_member(NET_sigma(upper), OTHER))
    {
        sigma_add_to(NET_sigma(upper), mk);
        sigma_add_to(NET_sigma(upper), lb);
        sigma_add_to(NET_sigma(upper), rb);
    }

    for (i = 0; i < NV_len(lower_nets); i++)
        if (sigma_member(NET_sigma(NV_net(lower_nets, i)), OTHER))
        {
            sigma_add_to(NET_sigma(NV_net(lower_nets, i)), mk);
            sigma_add_to(NET_sigma(NV_net(lower_nets, i)), lb);
            sigma_add_to(NET_sigma(NV_net(lower_nets, i)), rb);
        }

    /* Check if UPPER includes the empty string. Many things
       depend on it.
       */

    empty_string_included_p = STATE_final(NET_start_state(upper));


    /* For the shortest match, cut off at final states
       for the longest match, partition upper to sublanguages
       represented by a network with a unique final state.
       Note: either way, UPPER is reclaimed.
       */

    if (length_of_match == 0)
        cut_off_tails(upper, direction);

    /* Construct auxiliary nets */

    alph = make_alph(2, LABEL_VECTOR);
    ALPH_item(alph, 0) = mk;
    ALPH_len(alph) = 1; 			/* ALPH includes just mk */
    
    /* UPPER1: [UPPER ./. mk  */
    
    upper1 = ignore_alph(upper, alph, DO_COPY, DO_MINIMIZE,
    					 INTERNALLY, BOTH_SIDES, DONT_EXPAND);


    ALPH_item(alph, 0) = lb;
    ALPH_item(alph, 1) = rb; 
    ALPH_len(alph) = 2 ;		/* ALPH includes lb and rb */

    /* UPPER2: [UPPER ./. [lb | rb] */
    
	upper2 = ignore_alph(upper,alph, DO_COPY, DO_MINIMIZE,
						 INTERNALLY, BOTH_SIDES, DONT_EXPAND);
	
	ALPH_item(alph, 2) = mk; 				 
	ALPH_len(alph) = 3 ; 
	aux = sigma_fsm(alph);		/* ALPH includes lb and rb, and mk */
						 
	free_alph(alph);
						 
    if (empty_string_included_p)
    {
        /* UPPER contains the empty string, take direction into account */

        if (direction == 0)
        {
            /* L-TO-R : [UPPER/[LB|RB] - [?* [LB|RB]] - [LB ?*]] */
			
			ignore_nonfinal_initial(upper2, rb);
        }
        else
        {
            /* R-TO-L : [UPPER/[LB|RB] - [?* RB] - [[LB|RB] ?*]] */
            
			ignore_noninitial_final(upper2, lb);
        }
    }

    /* Go to work! */

    free_network(upper);
    
    /* Initial Match */

    nv.len = 3;
    
    /* ~$[AUX] */

    nv.nets[0] = negate_net(contains_fsm(aux, DONT_COPY, DONT_MINIMIZE),
                                DONT_COPY, DO_MINIMIZE);
    /* [] -> MK */

    nv.nets[1] = simple_replacement(epsilon_fsm(), symbol_fsm(mk),
                                        DONT_COPY, FALSE);

    if (empty_string_included_p)
    {
        /* UPPER includes the empty string. MK everywhere: [[MK \MK]* MK]] */

        nv.nets[2] = min_fsm(concat(zero_plus(concat(symbol_fsm(mk),
                                                     other_than_lab(mk)),
                                              DONT_COPY, DONT_MINIMIZE),
                                    symbol_fsm(mk)),
                             DONT_COPY);
    }
    else
    {
        /* No empty string. Take direction into account. */

        if (direction == 0)
        {
            /* left-to-right: UPPER' => MK _ ] */

            nv.nets[2] = restrict_center(copy_fsm(upper1), symbol_fsm(mk),
                                         epsilon_fsm());
        }
        else
        {
            /* right-to-left: UPPER' => _ MK ] */

            nv.nets[2] = restrict_center(copy_fsm(upper1), epsilon_fsm(),
                                         symbol_fsm(mk));
        }
    }
    im = compose_fst(&nv, DO_RECLAIM, DO_MINIMIZE, 
                     DONT_CLEANUP, SEQUENTIALLY, FLAGS_NOT_SPECIAL);

    /* Direction */

    nv.len = 2;

    if (direction == 0)
    {
        /* L-to-R: [[[~$[MK][MK:LB UPPER1 0:RB]]* ~$[MK]] .o. MK->[]] */

        nv.nets[0] = negate_fsm(contains_label(mk), DONT_COPY, DONT_MINIMIZE);
    
	make_pair_label(&pair_id1, mk, lb);
	make_pair_label(&pair_id2, EPSILON, rb);
        nv.nets[1] = min_fsm(concat(symbol_fsm(pair_id1),
                                    concat_label(upper1,
                                                 pair_id2,
                                                 DONT_COPY, DONT_MINIMIZE)),
                             DONT_COPY);

        nv.nets[0] = zero_plus(concat_fsm(&nv, DONT_COPY, DONT_MINIMIZE),
                               DONT_COPY, DO_MINIMIZE);

        nv.nets[1] = negate_fsm(contains_label(mk), DONT_COPY, DONT_MINIMIZE);
    }
    else
    {
        /* R-to-L: [~$[MK] [[0:LB UPPER1 MK:RB]* ~$[MK]]* ] .o. MK->[]] */
        make_pair_label(&pair_id1, EPSILON, lb);
	make_pair_label(&pair_id2, mk, rb);
        nv.nets[0] = min_fsm(concat(symbol_fsm(pair_id1),
                                    concat_label(upper1,
                                                 pair_id2,
                                                 DONT_COPY, DONT_MINIMIZE)),
                             DONT_COPY);

        nv.nets[1] = negate_fsm(contains_label(mk), DONT_COPY, DONT_MINIMIZE);
    
        nv.nets[1] = zero_plus(concat_fsm(&nv, DONT_COPY, DONT_MINIMIZE),
                               DONT_COPY, DO_MINIMIZE);

        nv.nets[0] = negate_fsm(contains_label(mk), DONT_COPY, DONT_MINIMIZE);
    }

    /* Instead of composing with MK -> [], just turn remaining MK
       labels to MK:EPSILON labels. 
       */

    dir = map_to_eps(mk, concat_fsm(&nv, DONT_COPY, DO_MINIMIZE));
    
    /* Length of Match */

    nv.nets[0] = upper2;

    if (direction == 0)
    {
        /* left-to-right, longest match: [ ~$[LB [UPPER2 & $[RB]]]  */

        nv.nets[1] = contains_label(rb);

        nv.nets[1] = intersect_fsm(&nv, DO_RECLAIM, DO_MINIMIZE);
        nv.nets[0] = symbol_fsm(lb);
    }
    else
    {
        /* right-to-left, longest match: [ ~$[[UPPER2 & $[LB]] RB]  */

        nv.nets[1] = contains_label(lb);

        nv.nets[0] = intersect_fsm(&nv, DO_RECLAIM, DO_MINIMIZE);
        nv.nets[1] = symbol_fsm(rb);
    }

    lm = negate_fsm(contains_fsm(concat_fsm(&nv, DONT_COPY, DONT_MINIMIZE),
                                 DONT_COPY, DONT_MINIMIZE),
                    DONT_COPY, DO_MINIMIZE);

    /* If UPPER includes the empty string, conside the empty strings
       on both sides of UPPER as being included in the match for UPPER. */

    if (length_of_match == 1 && empty_string_included_p)
    {
        nv.nets[0] = lm;

        if (direction == 0)
        {
            /* RB LB -> [] || _ RB */  /* RB LB RB -> RB */
            
            nv.nets[1] =
                simple_replacement(concat_label(concat_label(symbol_fsm(rb),
                                                             lb, DONT_COPY,
                                                             DONT_MINIMIZE),
                                                rb, DONT_COPY, DONT_MINIMIZE),
                                   symbol_fsm(rb), DONT_COPY, FALSE);
        }
        else
        {
            /* RB LB -> [] || LB _ */  /* LB RB LB -> LB */
            nv.nets[1] =
                simple_replacement(concat_label(concat_label(symbol_fsm(lb),
                                                             rb, DONT_COPY,
                                                             DONT_MINIMIZE),
                                                lb, DONT_COPY, DONT_MINIMIZE),
                                   symbol_fsm(lb), DONT_COPY, FALSE);
        }

        lm = compose_fst(&nv, DO_RECLAIM, DO_MINIMIZE, DO_CLEANUP,
                          SEQUENTIALLY, FLAGS_NOT_SPECIAL);
    }

    /* Replace or Insert */

    /* map: [ LB UPPER RB -> LOWER ]  or [ LB -> PREF, RB -> SUFF ] */

    map = assemble_replacement(upper_nets, lower_nets, lb, rb,
                               direction, optional_p);
/*
    nv.nets[0] = symbol_fsm(lb);
    nv.nets[1] = upper;

    map = simple_replacement(concat_label(concat_fsm(&nv, DONT_COPY,
                                                     DONT_MINIMIZE),
                                          rb, DONT_COPY, DO_MINIMIZE),
                             copy_fsm(NV_net(lower_nets, 0)),
                             DONT_COPY, FALSE);
*/

    nv.len = 4;
    nv.nets[0] = im;
    nv.nets[1] = dir;
    nv.nets[2] = lm;
    nv.nets[3] = map;

    net = compose_fst(&nv, DO_RECLAIM, DO_MINIMIZE, DO_CLEANUP,
                      SEQUENTIALLY, FLAGS_NOT_SPECIAL);

    /* Eliminate auxiliary symbols from sigma */

    sigma_remove_from(NET_sigma(net), mk);
    sigma_remove_from(NET_sigma(net), lb);
    sigma_remove_from(NET_sigma(net), rb);

    /* If upper contains OTHER and one of the lower nets
       contains the INSERT_SYMBOL, the net may now have
       redundant INSERT_SYMBOL arcs. If everything is perfect,
       compaction of the sigma should remove those arcs eliminating
       the unwanted INSERT_SYMBOL from the sigma.
       */

    if (sigma_member(NET_sigma(net), INSERT_SYMBOL))
        compact_sigma(net);

    return(net);
}


/************************************************************************
*
*          END OF UNCONDITIONAL DIRECTED REPLACEMENT
*
************************************************************************/


/****************************************
 *
 *  UNCONDITIONAL PARALLEL REPLACEMENT
 *
 *  UPPER1 arrow LOWER1 , ... , UPPERn arrow LOWERn
 *
 *    arrow:        ->  <-  <->  (->)  (<-)  (<->)
 *     (has to be the same in all replacements)
 *
 ***********************************/


static void print_error(char *message)
{
  /* Prints an error message concerning parallel replacement. */

  fsm_fprintf(stderr,
	  "\n***** ERROR in PARALLEL REPLACEMENT:\n");
  fsm_fprintf(stderr, "  %s\n\n", message);
}


static void make_aux_symbols(NVptr aux, id_type *aux_id, char *str1,
				 char *str2)
{
  /* Makes labels, ids and fsm for as many auxiliary symbols
     as NV_len(aux). The labels are 'str1+index+str2'.
     The ids are written to aux_id[n] and the fsm to aux->net[n].
     */

  char label[20], *ptdig;
  int i, k, num_dig, idx, aux_len;
  id_type id;

  aux_len = NV_len(aux);
  num_dig = 0;            /* calculate number of digits */
  for(i=aux_len; i>0; i/=10) num_dig++;

  strcpy(label, str1);          /* prepare label */
  ptdig = &(label [strlen(str1) +num_dig -1] ); /* on last digit */
  strcpy( ptdig+1, str2);

  for(i=0; i<aux_len; i++)
    {
                    /* write index 'idx' of aux_symbol to its label */
      for(idx=i+1, k=0; k<num_dig; k++)
	{ 
	  ptdig[-k] = idx%10+'0'; 
	  idx /= 10; 
	}

      /* make id and fsm */
      FST_intern_literal(&id, label);
      aux_id[i] = id;     
      NV_net(aux, i) = symbol_to_fsm(aux_id[i]);
    }
}



NETptr uncond_repl_unidir(NVptr upper, NVptr lower, int optional)
{
  /* Replaces in parallel all UPPERi unidirectional and context-free
     by the corresponding LOWERi:

              U1 -> L1 , ... , Un -> Ln

     This is calculated:

              [ [ no_upper  UxL ]*  no_upper ]

     with:
              no_upper  =  ~$[ [ U1 | ... | Un ] - [] ]
	      UxL       =  [ [ U1 .x. L1 ] | ... | [ Un .x. Ln ] ]

     If the replacement is optional then we use ?* instead of no_upper:

              [ [ ?*  UxL ]*  ?* ]

     No arguments are consumed.
     */

  NVptr nvp;
  NETptr no_upper, UxL, net, xupper, xlower;
  int i, len;

  len = NV_len(upper);
  nvp = make_nv(len);

  /* make  no_upper = ~$[ [ U1 | ... | Un ] - [] ]

  if(optional == FALSE)  -- obligatory replacement --
    {
      net = minus_fsm(min_fsm(union_fsm(upper, DO_COPY, DONT_MINIMIZE),
                              DONT_COPY),
		      epsilon_fsm(), DO_RECLAIM, DO_RECLAIM, DO_MINIMIZE);

      no_upper = negate_net(contains_fsm(net, DONT_COPY, DO_MINIMIZE),
			    DONT_COPY, DO_MINIMIZE);
    }
  else               -- optional replacement --
    no_upper = sigma_star_fsm();
   */

  /* make  UxL =  [ [ U1 .x. L1 ] | ... | [ Un .x. Ln ] ] */

  for(i=0; i<len; i++)
    {
      xupper = NV_net(upper, i);
      xlower = NV_net(lower, i);
      if (sigma_member(NET_sigma(xlower), INSERT_SYMBOL))
	NV_net(nvp, i) = insert_upper(xupper, xlower, EPSILON, EPSILON);
      else
	NV_net(nvp, i) = crossproduct_fst(xupper, xlower, DONT_RECLAIM,
					  DONT_MINIMIZE);
    }

  UxL = min_fsm(union_fsm(nvp, DONT_COPY, DONT_MINIMIZE), DONT_COPY);


  if (optional == FALSE)  /* obligatory replacement */
  {
      /* make  no_upper = ~$[ [ U1 | ... | Un ] - [] ] */

      net = minus_fsm(union_fsm(upper, DO_COPY, DONT_MINIMIZE),
		      epsilon_fsm(), DO_RECLAIM, DO_RECLAIM, DONT_MINIMIZE);

      no_upper = negate_net(contains_fsm(net, DONT_COPY, DO_MINIMIZE),
			    DONT_COPY, DO_MINIMIZE);
  }
  else
      no_upper = sigma_star_fsm();

  /* make  [ [ no_upper  UxL ]*  no_upper ] */

  net = concat(copy_fsm(no_upper), UxL);
  net = concat(zero_plus(net, DONT_COPY, DO_MINIMIZE), no_upper);

  free_nv_only(nvp);

  return(net);
}


NETptr uncond_repl_bidir(NVptr upper, NVptr lower, int optional)
{
  /* Replaces in parallel all UPPERi bidirectional and context-free
     by the corresponding LOWERi. The replacement can be optional:

              U1 <-> L1 , ... , Un <-> Ln

     This is computed by:

     ~$[ AUX1 | ... | AUXn ]                network: no_aux
        .o.
     U1 -> AUX1 , ... , Un -> AUXn
        .o.
     AUX1 <- L1 , ... , AUXn <- Ln
        .o.
     ~$[ AUX1 | ... | AUXn ]                network: no_aux

     No arguments are consumed.
     */

  NVtype nvA;  NVptr aux;
  NETptr netsA[4], no_aux, Result;
  int i, len;  
  id_type *aux_id;

  /* prepare some variables */

  nvA.nets = netsA;  nvA.len = 4;

  len = NV_len(upper);
  aux = make_nv(len);
  aux_id = (id_type *) malloc (len * sizeof(id_type));

  make_aux_symbols(aux, aux_id, "*UPR-AUX", "*");

  /* make  no_aux = ~$[ AUX1 | ... | AUXn ] */

  no_aux = negate_net(contains_fsm(union_fsm(aux, DO_COPY, DO_MINIMIZE),
				   DONT_COPY, DO_MINIMIZE),
		      DONT_COPY, DO_MINIMIZE);

  /* compose the four steps */

  netsA[0] = copy_fsm(no_aux);

  netsA[1] = uncond_repl_unidir(upper, aux, optional);

  netsA[2] = uncond_repl_unidir(lower, aux, optional);
  netsA[2] = invert_fst(netsA[2], DONT_COPY);

  netsA[3] = no_aux;   /* will be consumed here */

  Result = compose_fst(&nvA, DO_RECLAIM, DO_MINIMIZE, DO_CLEANUP, PAIRWISE,
		       FLAGS_NOT_SPECIAL);

  /* Remove the auxiliary symbols from the sigma of the Result.
     Free variables. */

  for(i=0; i<len; i++)
    sigma_remove_from(NET_sigma(Result), aux_id[i]);

  free_nv_and_nets(aux); free(aux_id);

  return(Result);
}

static void no_insert_site_allowed(NETptr net, int msg)
{
    if (sigma_member(NET_sigma(net), INSERT_SYMBOL))
    {
        ALPHABETptr alph = make_alph(1, LABEL_VECTOR);
        ALPH_item(alph, 0) = EPSILON;
 
        net = symsubst(INSERT_SYMBOL, alph, net, DONT_COPY, FALSE);
 
        free_alph(alph);
 
        min_fsm(net, DONT_COPY);
 
        switch(msg)
        {
        case 0:
            fsm_fprintf(stdout, "*** Only one '%s' allowed in an insert clause!\n",
                    INSERT_STRING);
            break;
        case 1:
            fsm_fprintf(stdout, "*** Sorry, insert option is only supported ");
            fsm_fprintf(stdout, "in directed replacement.\n    '%s' is ",
                    INSERT_STRING);
            fsm_fprintf(stdout, "not allowed here yet.\n");
            break;
        default:
            assert(FALSE);
            break;
        }
    }
}
RSptr rs_upper_lower(NETptr upper, NETptr lower, int arrow)
{
  /* Makes a replacement structure with the arrow information and
     a net vektor containing UPPER and LOWER of a replacement.
     */

  RSptr rs;
                          /* -1 means: no context direction */
  rs = make_rs( 2, arrow, -1);
  RS_net(rs, 0) = upper;
  RS_net(rs, 1) = lower;

  return(rs);
}

NETptr rs_left_right(NETptr left, NETptr right)
{
    NVtype nv;
    NETptr net, nets[3];
 
    nv.nets = nets;
    nv.len = 3;
 
    if (INSERT_SYMBOL == 0)
      {
	FST_intern_literal(&INSERT_SYMBOL,INSERT_STRING);
      }
 
    /* Default the arguments to the null string language. */
 
    if (!left)
/*      left = net_union(symbol_fsm(BOUNDARY_SYMBOL), symbol_fsm(OTHER)); */
        left = epsilon_fsm();
    else
        no_insert_site_allowed(left, 0);
 
    if (!right)
/*        right = net_union(symbol_fsm(BOUNDARY_SYMBOL), symbol_fsm(OTHER));*/
        right = epsilon_fsm();
    else
        no_insert_site_allowed(right, 0);
 
    /* Add the insert mark to the sigma so that we don't
       get an unwanted expansion of OTHER in the concatenation.
       */
 
    sigma_add_to(NET_sigma(left), INSERT_SYMBOL);
    sigma_add_to(NET_sigma(right), INSERT_SYMBOL);
 
    nets[0] = left;
    nets[1] = symbol_fsm(INSERT_SYMBOL);
    nets[2] = right;
 
    net = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);
    return(net);
}


static void PR_expand_other(NVptr nv)
{
    /* Scans the nets in NV for the presence of OTHER. If OTHER
       is found, collects the alphabets from all of the nets in NV
       and expands OTHER accordingly in all the nets.
       */

    ALPHABETptr alph;
    int i, len = NV_len(nv);

    /* First look for OTHER */

    for (i = 0; i < len; i++)
      if (NV_net(nv, i) && sigma_member(NET_sigma(NV_net(nv, i)), OTHER))
      {
          /* Do the work and break the loop. */

          alph = make_alph(global_alph_size(), BINARY_VECTOR);

          for (i = 0; i < len; i++)
              if (NV_net(nv, i))
                  sigma_union_dont_copy(alph, NET_sigma(NV_net(nv, i)));

          for (i = 0; i < len; i++)
              if (NV_net(nv, i))
                  expand_other(NV_net(nv, i), alph, DONT_COPY);

          free_alph(alph);
 
          break;
      }
}

/*
#define NET_single_epsilon(N)   NET_mark(N)    --- defined in re-aux.h ---
*/

NETptr rs_set_single_epsilon (NETptr net)
{
  NET_single_epsilon (net) = TRUE;
  return (net);
}

NETptr rs_reset_single_epsilon (NETptr net)
{
  NET_single_epsilon (net) = FALSE;
  return (net);
}

static NETptr uncond_dir_repl_unidir(NVptr upper_nets, NVptr lower_nets,
                                  int direction, int length_of_match,
                                  int optional_p)
{
  /* Replaces in parallel all UPPERi unidirectional and context-free
     by the corresponding LOWERi giving preference to the longest
     match of UPPERi from left-to-right.

              U1 @-> L1 , ... , Un @-> Ln
              U1 ->@ L1 , ... , Un ->@ Ln

     DIRECTION must be either 0 (= left) or 1 (= right).
     LENGTH_OF_MATCH must be either 0 (= shortest) or 1 (= longest).

     No arguments are consumed.
     */

    NETptr net;
  
  net = directed_replace(upper_nets, lower_nets, direction,
                              length_of_match, optional_p);

  return(net);
}

NETptr uncond_replace(RSptr uplow, int reclaim_p)
{
  /* Replaces in parallel all UPPERi by the corresponding LOWERi:

              U1 arrow L1 , ... , Un arrow Ln

     with:
              arrow     ->  <-  <->  (->)  (<-)  (<->)
	    (Has to be the same in every single replacement !!!)
     */

  RSptr cond_repl_rs;
  NVptr upper, lower, leftright;
  NETptr Result;
  int i, len, arrow, optional, single_eps;

  /* The replace_structure 'uplow' can be preprocessed in any particular
     application, if the application defines and assigns a function to
     the corresponding global variable.
     (see definition of applic_preprocess_uplow) [AK 12/09/97] */

  if (applic_preprocess_uplow)  /* execute if function is assigned */
    applic_preprocess_uplow (uplow);

  /* check whether all arguments are simple fsm, and no transducers */

  if (NV_fst_check( RS_nv(uplow) ))
    {
      print_error("All UPPER, LOWER, LEFT and RIGHT must be simple fsm, and no transducers.\n");
                                   /* free the function argument */
      if(reclaim_p == DO_RECLAIM) free_rs(uplow);
      return(sigma_star_fsm());
    }

  /* if any one of the UPPERi or LOWERi contains a SINGLE EPSILON
     we make a condictional replacement */

  single_eps = FALSE;
  for (i = 0; i < NV_len (RS_nv (uplow)); i++)
    if ((single_eps = NET_single_epsilon (NV_net (RS_nv (uplow), i))))
      break;

  if (single_eps)
    {
      leftright = NV_2_nets_to_nv (NULL, NULL);   /* unspecified context */
      cond_repl_rs =
	crossprod_uplow_leftright (uplow, leftright, 0, DONT_RECLAIM);

      free_nv_only (leftright);   /* contains NULL instead of nets */

      if(reclaim_p == DO_RECLAIM)
	free_rs (uplow);       /* free the function argument */

      Result = conditional_replace (cond_repl_rs, DO_RECLAIM);
      return (Result);
    }

  /* If any of the components has OTHER in its sigma, we need to
     expand it */

  PR_expand_other(RS_nv(uplow));

  /* extract all information from 'uplow' */

  len = RS_len(uplow) /2 ;
  upper = make_nv(len);  lower = make_nv(len);

  NV_select_copy_nets( RS_nv(uplow), upper, 0, 2);
  NV_select_copy_nets( RS_nv(uplow), lower, 1, 2);

  arrow = RS_arrow(uplow) % 10;
  optional = RS_arrow(uplow) / 10;

  if(reclaim_p == DO_RECLAIM) free_rs(uplow);  /* free the function argument */

  /* Warn the user if INSERT_SYMBOL appears where it is not yet
     supported. Remove later. INSERT_SYMBOL now supported.
     14.11.1997 LK
     */

  if (INSERT_SYMBOL == 0)
    {
       FST_intern_literal(&INSERT_SYMBOL, INSERT_STRING);
    }

  /* handle the different arrows */

  switch(arrow)    /*  ->  <-  <->  */
    {
    case 0:  /* -> */
      /*      insert_symbol_check(lower); */
      Result = uncond_repl_unidir(upper, lower, optional);
      break;
    case 1:   /* <- */
      /* insert_symbol_check(lower); */
      Result = uncond_repl_unidir(lower, upper, optional);
      Result = invert_fst(Result, DONT_COPY);
      break;
    case 2:  /* <-> */
      /* insert_symbol_check(lower); */
      Result = uncond_repl_bidir(upper, lower, optional);
      break;
    case 3:  /* @-> */
      Result = uncond_dir_repl_unidir(upper, lower, 0, 1, optional);
      break;
    case 4:  /* ->@ */
      Result = uncond_dir_repl_unidir(upper, lower, 1, 1, optional);
      break;
    case 5:  /* @> */
      Result = uncond_dir_repl_unidir(upper, lower, 0, 0, optional);
      break;
    case 6:  /* @< */
      Result = uncond_dir_repl_unidir(upper, lower, 1, 0, optional);
      break;
    case 7:  /* @<-  i.e. case 3 backwards and inverted  */
      Result = uncond_dir_repl_unidir(lower, upper, 0, 1, optional);
      invert_fst(Result, DONT_COPY);
      break;
    case 8:  /* <-@  case 4 backwards and inverted */
      Result = uncond_dir_repl_unidir(lower, upper, 1, 1, optional);
      invert_fst(Result, DONT_COPY);
      break;
    case 9:  /* @<  case 5 backwards and inverted. */
      Result = uncond_dir_repl_unidir(lower, upper, 0, 0, optional);
      invert_fst(Result, DONT_COPY);
      break;
    case -1: /* <@ case 6 backwards and inverted, switching -1 to
	      1 (not really necessary) if needed */
      if (optional == -1)
	optional = 1;
      Result = uncond_dir_repl_unidir(lower, upper, 1, 0, optional);
      invert_fst(Result, DONT_COPY);
      break;
    default:
      print_error("Invalide arrow type");
      Result = sigma_star_fsm();
      break;
    }

  /* free intermadiate nv and networks */

  free_nv_and_nets(upper);  free_nv_and_nets(lower);

  compact_sigma(Result);

  return(Result);
}


/***********************************************
*
*  END OF UNCONDITIONAL PARALLEL REPLACEMENT
*
************************************************/


/****************************************
 *
 *  FUNCTIONS FOR THE CONDITIONAL PARALLEL REPLACEMENT
 *  IN MULTIPLE CONTEXTS
 *
 *  { UP1 -> LOW1 , ... , UPn -> LOWn } || { L1 _ R1 , ... , Lm _ Rm } ,
 *        ......... ,
 *        ......... ,
 *
 *  { UPx -> LOWx , ... , UPy -> LOWy } || { Lp _ Rp , ... , Lq _ Rq } ;
 *
 *
 *    arrow:              ->  <-  <->  (->)  (<-)  (<->)
 *    context direction:  ||  //  \\  \/
 *     (have to be the same in all replacements)
 *
 ***********************************/



/* At the beginning we store information concerning all replacements
   to do in parallel in a 'replacement structure' (RStype):

       RS_net(rs, 0) = UPPER1
       RS_net(rs, 1) = LOWER1
       RS_net(rs, 2) = LEFT1
       RS_net(rs, 3) = RIGHT1

       RS_net(rs, 4) = UPPER2
       RS_net(rs, 5) = LOWER2
        ....

       RS_net(rs, n*4-1) = RIGHTn

   Later we put this and additional information to a 'net vector vector'
   (NVVtype):

       NVV_nv(nvv, 0) = net vector containing UPPER1 until UPPERn
       NVV_nv(nvv, 1) = LOWER1 ... n
       NVV_nv(nvv, 2) = LEFT1 ... n
       NVV_nv(nvv, 3) = RIGHT1 ... n
       NVV_nv(nvv, 4) = LeftBrackets1 ... n
       NVV_nv(nvv, 5) = RightBrackets1 ... n

   We store the ids of all brackets to  (int16) ids[X][Y].
*/

RSptr crossprod_uplow_leftright
  (RSptr uplow, NVptr leftright, int ctxdir, int reclaim_p)
{
  /* Creates a new replacement structure 'rs' where all UPPER_LOWER
     from 'uplow' are combined with all LEFT_RIGHT from 'leftright':

         UP1 LOW1 L1 R1  UP1 LOW1 L2 R2  ...  UP1 LOW1 Lm Rm  ...
         ...  UPn LOWn L1 R1  UPn LOWn L2 R2  ...  UPn LOWn Lm Rm
     */

  RSptr rs;  NETptr left, right;
  int i, k, idx, len1, len2;

  /* The networks in the replace_structure 'uplow' and in the net_vector
     'leftright' can be preprocessed in any particular application, if
     the application defines and assigns functions to the corresponding
     global variables (see definition of applic_preprocess_uplow and
     applic_preprocess_leftright). [AK 12/09/97] */

  if (applic_preprocess_uplow)  /* execute if function is assigned */
    applic_preprocess_uplow (uplow);

  if (applic_preprocess_leftright)  /* execute if function is assigned */
    applic_preprocess_leftright (leftright);

  /* we pair every 'uplow' with every 'leftright' */

  idx = 0;
  len1 = RS_len(uplow);
  len2 = NV_len(leftright);

  rs = make_rs(len1*len2, RS_arrow(uplow), ctxdir);

  for (i=0; i<len1; i+=2)
    for (k=0; k<len2; k+=2)
      {
	RS_net(rs, idx++) = copy_fsm( RS_net(uplow, i));      /* UPPER */
	RS_net(rs, idx++) = copy_fsm( RS_net(uplow, i+1));    /* LOWER */

	left = NV_net(leftright, k);          /* LEFT */
	if (left)  left = copy_fsm(left);
	RS_net(rs, idx++) = left;

	right = NV_net(leftright, k+1);       /* RIGHT */
	if (right)  right = copy_fsm(right);
	RS_net(rs, idx++) = right;
      }

  if (reclaim_p == DO_RECLAIM)
    { free_rs(uplow); free_nv_and_nets(leftright); }

  return(rs);
}


static int is_fsm_equivalent (NETptr n1, NETptr n2)
{
  if ((! n1) && (! n2))
    return (TRUE);
  if ((! n1) || (! n2))
    return (FALSE);

  return (equiv_fsm_p (n1, n2));
}

static void handle_equivalent_contexts(NVVptr repl, id_type **br_ids)
{
  /* Equivalent context pairs get the same bracket pairs. */

  int i, k, len = NV_len( LEFT_CTX_NV(repl)), *handled;

  handled = (int *) malloc (len * sizeof(int));
  for (i=0; i<len; i++)  handled[i] = FALSE;

  for (i=0; i<len; i++)
    {
      if (handled[i])
	continue;

      for (k=i+1; k<len; k++)
	{
	  if (handled[k])
	    continue;
	  if (! is_fsm_equivalent (NV_net(LEFT_CTX_NV(repl), i),
				       NV_net(LEFT_CTX_NV(repl), k)))
	    continue;
	  if (! is_fsm_equivalent (NV_net(RIGHT_CTX_NV(repl), i),
				       NV_net(RIGHT_CTX_NV(repl), k)))
	    continue;

	  free_network (NV_net(LEFT_BR_NV(repl), k));  /* replace <k by <i */
	  NV_net( LEFT_BR_NV(repl), k) = 
	    copy_fsm( NV_net( LEFT_BR_NV(repl), i));
	  LEFT_BR_ID( br_ids, k) = LEFT_BR_ID( br_ids, i);

	  free_network (NV_net(RIGHT_BR_NV(repl), k));  /* replace >k by >i */
	  NV_net( RIGHT_BR_NV(repl), k) =
	    copy_fsm( NV_net( RIGHT_BR_NV(repl), i));
	  RIGHT_BR_ID( br_ids, k) = RIGHT_BR_ID( br_ids, i);

	  handled[k] = TRUE;
	}
    }

  free(handled);
}


/****************************************
 *
 *     * * * * *   N E W   V E R S I O N   * * * * *
 *
 *  FUNCTIONS FOR THE CONDITIONAL PARALLEL REPLACEMENT
 *  IN MULTIPLE CONTEXTS
 *
 *  { UP1 -> LOW1 , ... , UPn -> LOWn } || { L1 _ R1 , ... , Lm _ Rm } ,
 *        ......... ,
 *        ......... ,
 *
 *  { UPx -> LOWx , ... , UPy -> LOWy } || { Lp _ Rp , ... , Lq _ Rq } ;
 *
 *
 *    arrow:             ->   <-   <->        (->)   (<-)   (<->)
 *                       @->  ->@  @>   >@    (@->)  (->@)  (@>)   (>@)
 *
 *    context direction:  ||  //  \\  \/
 *
 *     (arrow and context direction must be the same in all replacements)
 *
 ***********************************/


/**************
 *   Handle variable stack
 */

static void put_first_auxiliaries_on_stack
  (NVVptr repl_nvv, RE2N_VSTptr variables, int appl_brs_p, int depth)
{
  /* Puts all epsilon, boundary and bracket symbols on the variable stack. */

  NVtype nv;  NETptr nets [2];  char name [100];

  nv.len = 2;
  nv.nets = nets;

  /* epsilon symbols */

  sprintf (name, "%s%d*", EPS_SINGLE_NAME, depth);
  RE2N_save_var (variables, "eps_single", label_fsm (name));

  sprintf (name, "%s%d*", EPS_MULTI_NAME, depth);
  RE2N_save_var (variables, "eps_multi", label_fsm (name));
/*
  nets [0] = RE2N_copy_var (variables, "eps_single");
  nets [1] = RE2N_copy_var (variables, "eps_multi");
  RE2N_save_var (variables, "epsilons",
		 union_fsm (&nv, DONT_COPY, DO_MINIMIZE));
*/
  /* boundary symbols */

  sprintf (name, "%s%d*", STRING_BEGIN_NAME, depth);
  RE2N_save_var (variables, "str_begin", label_fsm (name));

  sprintf (name, "%s%d*", STRING_END_NAME, depth);
  RE2N_save_var (variables, "str_end", label_fsm (name));

  nets [0] = RE2N_copy_var (variables, "str_begin");
  nets [1] = RE2N_copy_var (variables, "str_end");
  RE2N_save_var (variables, "boundaries",
		 union_fsm (&nv, DONT_COPY, DO_MINIMIZE));

  /* context brackets */

  RE2N_save_var (variables, "<all",
		 union_fsm (LEFT_BR_NV (repl_nvv), DO_COPY, DO_MINIMIZE));
  RE2N_save_var (variables, ">all",
		 union_fsm (RIGHT_BR_NV (repl_nvv), DO_COPY, DO_MINIMIZE));

  nets [0] = RE2N_copy_var (variables, "<all");
  nets [1] = RE2N_copy_var (variables, ">all");
  RE2N_save_var (variables, "<all|>all",
		 union_fsm (&nv, DONT_COPY, DO_MINIMIZE));

  /* application brackets (if needed) */

  if (appl_brs_p)
    {
      sprintf (name, "%s%d*", APPLIC_LEFT, depth);
      RE2N_save_var (variables, "<AP", label_fsm (name));

      sprintf (name, "%s%d*", APPLIC_RIGHT, depth);
      RE2N_save_var (variables, ">AP", label_fsm (name));

      nets [0] = RE2N_copy_var (variables, "<AP");
      nets [1] = RE2N_copy_var (variables, ">AP");
      RE2N_save_var (variables, "<AP|>AP",
		     union_fsm (&nv, DONT_COPY, DO_MINIMIZE));

      nets [0] = RE2N_copy_var (variables, "<all");
      nets [1] = RE2N_copy_var (variables, "<AP");
      RE2N_save_var (variables, "<all|<AP",
		     union_fsm (&nv, DONT_COPY, DO_MINIMIZE));

      nets [0] = RE2N_copy_var (variables, ">all");
      nets [1] = RE2N_copy_var (variables, ">AP");
      RE2N_save_var (variables, ">all|>AP",
		     union_fsm (&nv, DONT_COPY, DO_MINIMIZE));
    }
  else
    {
      RE2N_save_var (variables,"<all|<AP", RE2N_copy_var (variables,"<all"));
      RE2N_save_var (variables,">all|>AP", RE2N_copy_var (variables,">all"));
    }

  /* all brackets (application and context) */

  nets [0] = RE2N_copy_var (variables, "<all|<AP");
  nets [1] = RE2N_copy_var (variables, ">all|>AP");
  RE2N_save_var (variables, "brackets",
		 union_fsm (&nv, DONT_COPY, DO_MINIMIZE));
}

static void put_more_auxiliaries_on_stack
  (RE2N_VSTptr variables, int eps_single_p, int eps_multi_p)
{
  NVtype nv;  NETptr nets [2];
  nv.len = 2;
  nv.nets = nets;
  
  /* auxiliary symbols in general */
  
  if (eps_single_p && eps_multi_p)
    {
      nets [0] = RE2N_copy_var (variables, "eps_single");
      nets [1] = RE2N_copy_var (variables, "eps_multi");
      RE2N_save_var (variables, "epsilons",
		     union_fsm (&nv, DONT_COPY, DO_MINIMIZE));
  }
  else if (eps_single_p)
    RE2N_save_var (variables, "epsilons",
		   RE2N_copy_var (variables, "eps_single"));
  else if (eps_multi_p)
    RE2N_save_var (variables, "epsilons",
		   RE2N_copy_var (variables, "eps_multi"));
  else
    RE2N_save_var (variables, "epsilons", null_fsm());

  nets[0] = RE2N_copy_var (variables, "epsilons");
  nets [1] = RE2N_copy_var (variables, "brackets");
  RE2N_save_var (variables, "eps+brs",
		 union_fsm (&nv, DONT_COPY, DO_MINIMIZE));

  nets [0] = RE2N_copy_var (variables, "eps+brs");
  nets [1] = RE2N_copy_var (variables, "boundaries");
  RE2N_save_var (variables, "all_aux",
		 union_fsm (&nv, DONT_COPY, DO_MINIMIZE));

  RE2N_save_var (variables, "real_symbol",
		 other_than (RE2N_copy_var (variables, "all_aux")));
}


static void union_all_bracketed_upper
  (NVVptr repl_nvv, RE2N_VSTptr variables, int appl_brs_p)
{
  /* Puts the union of all bracketed UPPER on the variable stack:

         BrUp = <i <other* upper./.brs >other* >i
         NoBrUp = ~$BrUp

     If there are application brackets, we also need:

         ApBrUp = <AP BrUp >AP
	 NoApBrUp = ~$ApBrUp
     */

  NVptr BrUp;  NVtype nv;
  NETptr aux, left, other_left, right, other_right, upper, nets [5];
  int i;

  nv.len = 5;
  nv.nets = nets;

  BrUp = make_nv (NV_len (UPPER_NV (repl_nvv)));
  aux = net_union(RE2N_copy_var (variables, "<all|>all"),
		  RE2N_copy_var (variables, "epsilons"));
  for (i=0; i< NV_len (BrUp); i++)
    {
      left = copy_fsm (NV_net (LEFT_BR_NV (repl_nvv), i));
      right = copy_fsm (NV_net (RIGHT_BR_NV (repl_nvv), i));

      other_left = minus_fsm (RE2N_copy_var (variables, "<all"), left,
			      DO_RECLAIM, DONT_RECLAIM, DO_MINIMIZE);
      other_left = kleene_star (other_left);

      other_right = minus_fsm (RE2N_copy_var (variables, ">all"), right,
			       DO_RECLAIM, DONT_RECLAIM, DO_MINIMIZE);
      other_right = kleene_star (other_right);

      upper = copy_fsm (NV_net (UPPER_NV (repl_nvv), i));

      upper = ignore_alph(upper, NET_labels(aux), DONT_COPY, DO_MINIMIZE,
			  INTERNALLY, BOTH_SIDES, DONT_EXPAND);
      nets [0] = left;
      nets [1] = other_left;
      nets [2] = upper;
      nets [3] = other_right;
      nets [4] = right;
      NV_net (BrUp, i) = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);
    }

  upper = union_fsm (BrUp, DONT_COPY, DO_MINIMIZE);
  RE2N_save_var (variables, "BrUpper", copy_fsm (upper));

  upper = negate_net (contains_fsm (upper, DONT_COPY, DONT_MINIMIZE),
		      DONT_COPY, DO_MINIMIZE);
  RE2N_save_var (variables, "NoBrUpper", upper);   /*  ~$ BrUpper  */

  if (appl_brs_p)
    {
      nv.len = 3;
      nets [0] = RE2N_copy_var (variables, "<AP");
      nets [1] = RE2N_copy_var (variables, "BrUpper");
      nets [2] = RE2N_copy_var (variables, ">AP");
      upper = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);  /* <AP BrUpper >AP */
      RE2N_save_var (variables, "ApBrUpper", copy_fsm (upper));

      upper = negate_net (contains_fsm (upper, DONT_COPY, DONT_MINIMIZE),
			  DONT_COPY, DO_MINIMIZE);
      RE2N_save_var (variables, "NoApBrUpper", upper);   /*  ~$ ApBrUpper  */
    }

  free_nv_only (BrUp);
  free_network(aux);
}

/**************
 *   Copy and preprocess argument networks
 *   Clean final results (sigma)
 */

static int repl_struct_to_nvv
  (RSptr repl_rs, NVVptr repl_nvv, id_type **br_ids, int upwards_p, int depth)
{
  /* Writes the information concerning the parallel replacements
     from the 'replacement structure' repl_rs to the 'net vector vector'
     repl_nvv.

     'upwards_p' does not need to correspond with the arrow in the
     replacement structure RS_arrow(repl_rs), which here will not be taken
     into account at all !!!

     If (upwards_p==FALSE) then UPPERi is copied from RS_net(rs,i*4+0)
     and LOWERi from RS_net(rs,i*4+1).
     If (upwards_p==TRUE) then these indices are reversed.

     'repl_nvv' has to be of length 6 and must not point to NVs.
     '(id_type **)br_ids' must contain  br_ids[2][RS_len(repl_rs)/4].

     No arguments are consumed. 
     */

  NETptr net;
  char left_prefix [10], right_prefix [10];
  int len, i, k, Uidx, Lidx;

  /* set indices according to 'upwards_p' */

  if (upwards_p == FALSE)
    {
      Uidx = 0;  Lidx = 1;
    }
  else if (upwards_p == TRUE)
    {
      Uidx = 1;  Lidx = 0;
    }
  else
    {
      print_error ("wrong upwards_p in 'repl_struct_to_nvv'\n");
      return (FALSE);
    }

  /* copy UPPERi, LOWERi, LEFTi and RIGHTi to net vectors */

  len = RS_len(repl_rs)/4;
  for (i=0; i<6; i++)
    NVV_nv(repl_nvv, i) = make_nv(len);

  for (i=0, k=0;  i< len;  i++, k+=4)
    {
      NV_net (UPPER_NV (repl_nvv), i) = copy_fsm (RS_net(repl_rs, k+Uidx));
      NV_net (LOWER_NV (repl_nvv), i) = copy_fsm (RS_net(repl_rs, k+Lidx));

      if ((net = RS_net (repl_rs, k+2)))
	NV_net (LEFT_CTX_NV (repl_nvv), i) = copy_fsm (net);
      else
	NV_net (LEFT_CTX_NV (repl_nvv), i) = NULL;

      if ((net = RS_net (repl_rs, k+3)))
	NV_net (RIGHT_CTX_NV (repl_nvv), i) = copy_fsm (net);
      else
	NV_net (RIGHT_CTX_NV (repl_nvv), i) = NULL;
    }

  /* make brackets */

  sprintf (left_prefix, "*<%d-", depth);
  sprintf (right_prefix, "*>%d-", depth);

  make_aux_symbols
    (LEFT_BR_NV(repl_nvv), LEFT_BR_IDS(br_ids), left_prefix, "*");
  make_aux_symbols
    (RIGHT_BR_NV(repl_nvv), RIGHT_BR_IDS(br_ids), right_prefix, "*");

  return (TRUE);
}

static void exclude_aux_from_other (NETptr net, NETptr aux)
{
  /* Excludes auxiliary symbols from OTHER in the sigma of 'net'. */

  ALPHABETptr n, a;  int i;

  if (! sigma_member (NET_sigma (net), OTHER))  /* no OTHER */
    return;                        /* nothing to do */
  						 
  n = label_to_binary (NET_sigma (net));
  a = label_to_binary (NET_sigma (aux));

  for (i=0; i< ALPH_len (a); i++)
    if (i != OTHER)
      if (ALPH_item (a, i))    /* ID of an auxiliary symbol */
	sigma_add_to (n, i);      /* add ID of auxiliary symbol */
	
}

static NETptr preprocess_upper
  (NETptr upper, RE2N_VSTptr variables, int *single_eps_p, int *multi_eps_p)
{
  /* Handles epsilons in UPPER (or LOWER). 'upper' is modified:

             [[U - []] ./. all_eps] | one_eps
     */

  NVtype nv;  NETptr aux, one_eps, nets [2];

  nv.len = 2;
  nv.nets = nets;
  
  /* select epsilons */

  if (! STATE_final (NET_start_state (upper)))  /* epsilon not in UPPER */
    {
      one_eps = null_fsm ();
      *single_eps_p = FALSE;
      *multi_eps_p = FALSE;
      nets [0] = RE2N_copy_var (variables, "brackets");
    }
  else if (NET_single_epsilon (upper))  /* single epsilon in UPPER */
    {
      one_eps = RE2N_copy_var (variables, "eps_single");
      *single_eps_p = TRUE;
      *multi_eps_p = FALSE;
      nets [0] = net_union(RE2N_copy_var (variables, "brackets"),
			   RE2N_copy_var (variables, "eps_single"));
    }
  else                                  /* multiple epsilon in UPPER */
    {
      one_eps = RE2N_copy_var (variables, "eps_multi");
      *single_eps_p = FALSE;
      *multi_eps_p = TRUE;
      nets [0] = net_union(RE2N_copy_var (variables, "brackets"),
			   RE2N_copy_var (variables, "eps_multi"));
    }

  /* exclude all auxiliary symbols from OTHER in the sigma of UPPER */

  nets [1] = RE2N_copy_var (variables, "boundaries");
  aux = union_fsm (&nv, DONT_COPY, DO_MINIMIZE);
  
  exclude_aux_from_other (upper, aux);
  
  free_network (aux);

  /* modify UPPER */

  upper = minus_fsm (upper, epsilon_fsm(), DO_RECLAIM,DO_RECLAIM,DO_MINIMIZE);

  nets [0] = upper;
  nets [1] = one_eps;
  upper = union_fsm (&nv, DONT_COPY, DO_MINIMIZE);

  return (upper);
}

static NETptr preprocess_context
  (NETptr context, RE2N_VSTptr variables, int left_p)
{
  /* Handles NULL and boundary contexts.

             \eps+brs & \str_begin      ... for right NULL context
             \eps+brs & \str_end        ... for left NULL context
     */

  NVtype nv;  NETptr aux, filter, boundary, nets [2];
  STATEptr state; 
  ARCptr arc;  
  id_type boundary_id;

  nv.len = 2;
  nv.nets = nets;

  /* If context contains epsilon, it is equivalent to NULL */

  if ((context) && (STATE_final (NET_start_state (context))))
    {
      free_network (context);
      context = NULL;
    }

  /* NULL contexts:  [? | .#.] */

  if (! context)
    {
      nets [0] = symbol_fsm (OTHER);
      nets [1] = symbol_fsm (BOUNDARY_SYMBOL);
      context = union_fsm (&nv, DONT_COPY, DO_MINIMIZE);
    }

  /* boundary contexts */

  if (sigma_member (NET_sigma (context), BOUNDARY_SYMBOL))
    {
      if (left_p)     /*  (#B) ~$[#B | #E]  */
	{
	  boundary = RE2N_copy_var (variables, "str_begin");
	  boundary_id = ARC_label (STATE_arc_set (NET_start_state (boundary)));

	  nets [0] = optional (boundary);
	  nets [1] = negate (contains (RE2N_copy_var (variables, "boundaries")));
	}
      else            /*  ~$[#B | #E] (#E)  */
	{
	  boundary = RE2N_copy_var (variables, "str_end");
	  boundary_id = ARC_label (STATE_arc_set (NET_start_state (boundary)));

	  nets [0] = negate (contains (RE2N_copy_var (variables, "boundaries")));
	  nets [1] = optional (boundary);
	}
      filter = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

      for (state = NET_states (context);  state;  state = next_state (state))
	for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
	  if(ARC_label(arc) == BOUNDARY_SYMBOL)
	    ARC_label(arc) = boundary_id;

      sigma_remove_from (NET_sigma (context), BOUNDARY_SYMBOL);
      sigma_remove_from (NET_labels (context), BOUNDARY_SYMBOL);

      sigma_add_to (NET_sigma (context), boundary_id);
      sigma_add_to (NET_labels (context), boundary_id);
    }

  /* non-boundary contexts */

  else
    filter = negate (contains (RE2N_copy_var (variables, "boundaries")));

  /* filter the result */

  nets [0] = context;
  nets [1] = filter;
  context = intersect_fsm (&nv, DO_RECLAIM, DO_MINIMIZE);

  aux = RE2N_copy_var (variables, "eps+brs");
  exclude_aux_from_other (context, aux);
  free_network (aux);

  return (context);
}

static int preprocess_upper_nets
  (NVVptr repl_nvv, RE2N_VSTptr variables, int *single_eps_p, int *multi_eps_p)
{
  /* Preprocesses all UPPER, LOWER
     Check for INSERT_SYMBOL "..." */

  NETptr net;  int i, len, seps_p, meps_p; char *s = NULL;

  len = NV_len (UPPER_NV (repl_nvv));
  *single_eps_p = *multi_eps_p = FALSE;

  for (i=0; i< len; i++)
    {
      /* UPPER */
      net = NV_net (UPPER_NV (repl_nvv), i);
      if (sigma_member (NET_sigma (net), INSERT_SYMBOL))
	{ s = "UPPER";  break; }

      NV_net (UPPER_NV (repl_nvv), i) =
	preprocess_upper (net, variables, &seps_p, &meps_p);

      if (seps_p)
	*single_eps_p = TRUE;
      if (meps_p)
	*multi_eps_p = TRUE;

      /* LOWER */
      net = NV_net (LOWER_NV (repl_nvv), i);
      if (! sigma_member (NET_sigma (net), INSERT_SYMBOL))
	NV_net (LOWER_NV (repl_nvv), i) =
	  preprocess_upper (net, variables, &seps_p, &meps_p);
	  
      if (seps_p)
	*single_eps_p = TRUE;
      if (meps_p)
	*multi_eps_p = TRUE;
    }

  if (s)   /* INSERT_SYMBOL occured in UPPER or LEFT or RIGHT */
    {
      fsm_fprintf (stderr, "*** ERROR: INSERT_SYMBOL occured in %s\n", s);
      return (FALSE);
    }

  return (TRUE);
}

static int preprocess_context_nets
  (NVVptr repl_nvv, RE2N_VSTptr variables)
{
  /* Preprocesses all LEFT and RIGHT.  */

  NETptr net;  int i, len;
  char *s = NULL;

  len = NV_len (UPPER_NV (repl_nvv));

  for (i=0; i< len; i++)
    {

      /* LEFT */
      net = NV_net (LEFT_CTX_NV (repl_nvv), i);
      if ((net) && (sigma_member (NET_sigma (net), INSERT_SYMBOL)))
	{ s = "LEFT context";  break; }

      NV_net (LEFT_CTX_NV (repl_nvv), i) =
	preprocess_context (net, variables, TRUE);

      /* RIGHT */
      net = NV_net (RIGHT_CTX_NV (repl_nvv), i);
      if ((net) && (sigma_member (NET_sigma (net), INSERT_SYMBOL)))
	{ s = "RIGHT context";  break; }

      NV_net (RIGHT_CTX_NV (repl_nvv), i) =
	preprocess_context (net, variables, FALSE);
    }

  if (s)   /* INSERT_SYMBOL occured in UPPER or LEFT or RIGHT */
    {
      fsm_fprintf (stderr, "*** ERROR: INSERT_SYMBOL occured in %s\n", s);
      return (FALSE);
    }

  return (TRUE);
}
static void clean_sigma_from_auxiliaries
  (NETptr net, RE2N_VSTptr variables)
{
  /* Removes all auxiliary symbols from the final result. */

  NETptr auxnet;  ALPHABETptr aux, res;  int i;

  auxnet = RE2N_copy_var (variables, "all_aux");

  aux = label_to_binary (NET_sigma (auxnet));
  res = label_to_binary (NET_sigma (net));

  for (i=0; (i< ALPH_len (aux)) && (i< ALPH_len (res)); i++)
    if (i != OTHER)
      if (ALPH_item (aux, i))    /* ID of an auxiliary symbol */
	ALPH_item (res, i) = 0;      /* remove ID of auxiliary symbol */

  free_network (auxnet);
}


/**************
 *   steps of replacement
 */

static NETptr STEP_remove_auxiliaries (RE2N_VSTptr variables)
{
  /* STEP of replacement: Removes all auxiliary symbols.

             [ brackets | epsilons | boundaries ] -> []
     */

  NETptr Result;

  Result = RE2N_copy_var (variables, "all_aux");
  Result = simple_replacement (Result, epsilon_fsm(), DONT_COPY, FALSE);

  return (Result);
}

static NETptr STEP_constrain_auxiliaries
  (RE2N_VSTptr variables, int single_eps_p, int multi_eps_p)
{
  /* STEP of replacement: Constrains the order of all auxiliary symbols.
     No arguments are consumed.
     */

  NVtype nv;
  NETptr Result, epsilon_cons = 0, bracket_cons, boundary_cons,
    e1, e2, e3, e4, nets [4];

  nv.nets = nets;

  /* constrain epsilons:  case 1:  no epsilons
     ?*    */ /*DEBUG:AK,031119*/

  if ((! single_eps_p) && (! multi_eps_p))
    epsilon_cons = sigma_star_fsm();

  /* constrain epsilons:  case 2:  multiple epsilons only
     ?*    */ /*DEBUG:AK,031119*/

  else if ((! single_eps_p) && (multi_eps_p))
    epsilon_cons = sigma_star_fsm();

  /* constrain epsilons:  case 3:  multiple and single epsilons
     e1 & e2
       e1 = $[ Es ] ;                 DEBUG:AK,031119
       e2 = ~$[ brs+eps* & ~[ [Es | Em*]/brackets ] ]  */

  else if ((single_eps_p) && (multi_eps_p))
    {
      e1 = min_fsm (contains (RE2N_copy_var (variables, "eps_single")), DONT_COPY); /*DEBUG:AK,031119*/

      nv.len = 2;
      nets [0] = RE2N_copy_var (variables, "eps_single");
      nets [1] = kleene_star (RE2N_copy_var (variables, "eps_multi"));
      Result = union_fsm (&nv, DONT_COPY, DO_MINIMIZE);
      Result = ignore (Result, RE2N_copy_var (variables, "brackets"));

      nets [0] = kleene_star (RE2N_copy_var (variables, "eps+brs"));
      nets [1] = negate (Result);
      Result = intersect_fsm (&nv, DO_RECLAIM, DO_MINIMIZE);

      e2 = min_fsm (negate (contains (Result)), DONT_COPY);

      /*  e1 & e2  */
      nets [0] = e1;
      nets [1] = e2;

      epsilon_cons = intersect_fsm (&nv, DO_RECLAIM, DO_MINIMIZE);
    }

  /* constrain epsilons:  case 4:  single epsilons only
     e1 & e2 & e3 & e4   with
       e1 = $[ Es ] ;                 DEBUG:AK,031119
       e2 = ~$[ Es brs* Es ] ;
       e3 = ~[ ?* sym ~[ brs* Es ?* ] ] ;
       e4 = ~[ ~[ ?* Es brs* ] sym ?* ] ;  */

  else if ((single_eps_p) && (! multi_eps_p))
    {
      /*  e1 = $[ Es ]  */                 /*DEBUG:AK,031119*/
      e1 = min_fsm (contains (RE2N_copy_var (variables, "eps_single")), DONT_COPY); /*DEBUG:AK,031119*/

      /*  e2 = ~$[ Es brs* Es ]  */
      nv.len = 3;
      nets [0] = RE2N_copy_var (variables, "eps_single");
      nets [1] = kleene_star (RE2N_copy_var (variables, "brackets"));
      nets [2] = RE2N_copy_var (variables, "eps_single");
      e2 = min_fsm (negate (contains (concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE))), DONT_COPY);

      /*  e3 = ~[ ?* sym ~[ brs* Es ?* ] ]  */
      nv.len = 3;
      nets [0] = kleene_star (RE2N_copy_var (variables, "brackets"));
      nets [1] = RE2N_copy_var (variables, "eps_single");
      nets [2] = sigma_star_fsm ();
      Result = min_fsm (negate (concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE)), DONT_COPY);

      nets [0] = sigma_star_fsm ();
      nets [1] = RE2N_copy_var (variables, "real_symbol");
      nets [2] = Result;
      e3 = min_fsm (negate (concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE)), DONT_COPY);

      /*  e4 = ~[ ~[ ?* Es brs* ] sym ?* ] = e3.r  */
      e4 = reverse_fsm (e3, DO_COPY, DO_MINIMIZE);

      /*  e1 & e2 & e3 & e4  */
      nv.len = 4;
      nets [0] = e1;
      nets [1] = e2;
      nets [2] = e3;
      nets [3] = e4;
      epsilon_cons = intersect_fsm (&nv, DO_RECLAIM, DO_MINIMIZE);
    }

  /* constrain brackets:  ~$[ left_brs right_brs ]  */ /*DEBUG:AK,031119*/

  nv.len = 2;
  nets [0] = RE2N_copy_var (variables, "<all|<AP");
  nets [1] = RE2N_copy_var (variables, ">all|>AP");
  Result = concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE);

  bracket_cons = min_fsm (negate (contains (Result)), DONT_COPY);

  /* constrain boundaries:  str_begin ~$[str_begin | str_end] str_end  */

  nv.len = 2;
  nets [0] = RE2N_copy_var (variables, "str_begin");
  nets [1] = RE2N_copy_var (variables, "str_end");
  Result = union_fsm (&nv, DONT_COPY, DONT_MINIMIZE);

  nv.len = 3;
  nets [0] = RE2N_copy_var (variables, "str_begin");
  nets [1] = negate (contains (Result));
  nets [2] = RE2N_copy_var (variables, "str_end");
  boundary_cons = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

  /* intersect all constraints */

  nv.len = 3;
  nets [0] = epsilon_cons;
  nets [1] = bracket_cons;
  nets [2] = boundary_cons;
  Result = intersect_fsm (&nv, DO_RECLAIM, DO_MINIMIZE);

  return (Result);
}

static NETptr STEP_left_context
  (NVVptr repl_nvv, RE2N_VSTptr variables, int left_p, int single_eps_p, int multi_eps_p)
{
  /* STEP of replacement: Constrains contexts and brackets by each other.
     The computation is always done for left contexts.

         cons_1 & cons_2 & .... & cons_n

     with
         cons_i =
	   ~[ ~[?* left_i/aux]  <i ?*] &
	   ~[?* left_i./.aux  ~[[[<i epsilons]* <i]/[brackets - <i] \aux ?*]] 

     with
         aux = brackets | epsilons

     For right contexts (left_p == FALSE) the argument are reversed at the
     beginning and the final result is reversed at the end.
     No arguments are consumed. 
     */

  NVptr cons;  NVtype nv;
  NETptr Result, left, br, aux, no_aux, other_brs, consA, consB, nets [3];
  int i;

  nv.nets = nets;
  cons = make_nv (NV_len (LEFT_CTX_NV (repl_nvv)));

  aux = RE2N_copy_var (variables, "eps+brs");
  no_aux = other_than (copy_fsm (aux));

  for (i=0; i< NV_len (cons); i++)
    {
      /* prepare context and bracket */

      if (left_p)
	{
	  left = copy_fsm (NV_net (LEFT_CTX_NV (repl_nvv), i));
	  br = copy_fsm (NV_net (LEFT_BR_NV (repl_nvv), i));
	}
      else
	{
	  left = reverse_fsm (NV_net (RIGHT_CTX_NV (repl_nvv), i),
			      DO_COPY, DO_MINIMIZE);
	  br = copy_fsm (NV_net (RIGHT_BR_NV (repl_nvv), i));
	}

      /* (A) bracket requires context:  ~[ ~[?* left_i/aux]  <i ?*]  */

      nv.len = 2;
      nets [0] = sigma_star_fsm ();
      nets [1] = ignore_alph(left, NET_labels(aux), DO_COPY, DONT_MINIMIZE,
			     EVERYWHERE, BOTH_SIDES, DONT_EXPAND);
      Result = concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE);

      nv.len = 3;
      nets [0] = negate (Result);
      nets [1] = copy_fsm (br);
      nets [2] = sigma_star_fsm ();
      Result = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

      consA = negate (Result);

      /* (B) context requires bracket:
	 ~[?* left_i./.aux  ~[[[<i epsilons]* <i]/[brackets - <i] \aux ?*]] */

      nv.len = 2;
      nets [0] = copy_fsm (br);
      if (single_eps_p && multi_eps_p)
        nets [1] = RE2N_copy_var (variables, "epsilons");
      else if (single_eps_p)
        nets [1] = RE2N_copy_var (variables, "eps_single");
      else if (multi_eps_p)
        nets [1] = RE2N_copy_var (variables, "eps_multi");
      else
        nets [1] = epsilon_fsm();
      Result = concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE);

      nets [0] = kleene_star (Result);
      nets [1] = copy_fsm (br);
      Result = concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE);

      other_brs = minus_fsm (RE2N_copy_var (variables, "brackets"), br,
			     DO_RECLAIM, DONT_RECLAIM, DO_MINIMIZE);

      nv.len = 3;

      nets [0] = ignore_alph(Result, NET_labels(other_brs), DONT_COPY,
			     DONT_MINIMIZE, EVERYWHERE, BOTH_SIDES, DONT_EXPAND);
      free_network(other_brs);
      nets [1] = copy_fsm (no_aux);
      nets [2] = sigma_star_fsm ();
      Result = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

      nets [0] = sigma_star_fsm ();
	  nets[1] = ignore_alph(left, NET_labels(aux), DO_COPY, DONT_MINIMIZE,
	  						INTERNALLY, BOTH_SIDES, DONT_EXPAND);
      nets [2] = negate (Result);
      Result = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

      consB = negate (Result);

      /* intersect both constraints */

      nv.len = 2;
      nets [0] = consA;
      nets [1] = consB;
      NV_net (cons, i) = intersect_fsm (&nv, DO_RECLAIM, DO_MINIMIZE);

      free_network (left);
      free_network (br);
    }

  /* intersect all single constraints */

  Result = intersect_fsm (cons, DO_RECLAIM, DO_MINIMIZE);

  if (! left_p)
    Result = reverse_fsm (Result, DONT_COPY, DO_MINIMIZE);
    
  /*  (Result, stdout);*/

  free_nv_only (cons);
  free_network (aux);
  free_network (no_aux);

  return (Result);
}

static NETptr extract_chunk_marker (NETptr lower, int left_chunk_p)
{
  /* Extracts the left or the right chunk marker from the network.

         [lower .o. filter].l

     No arguments are consumed.
     */

  NVtype nv;  NETptr Result, No_InsSym, InsSym_0, any_0, filter, nets [3];

  nv.len = 3;
  nv.nets = nets;

  No_InsSym = negate (contains (symbol_fsm (INSERT_SYMBOL)));
  InsSym_0 = product (symbol_fsm (INSERT_SYMBOL), epsilon_fsm());
  any_0 = product (symbol_fsm (OTHER), epsilon_fsm());

  /* left chunk marker, filter:  ~$InsSym InsSym:0 [?:0]*  */

  if (left_chunk_p)
    {
      nets [0] = No_InsSym;
      nets [1] = InsSym_0;
      nets [2] = kleene_star (any_0);
    }

  /* right chunk marker, filter:  [?:0]* InsSym:0 ~$InsSym  */

  else
    {
      nets [0] = kleene_star (any_0);
      nets [1] = InsSym_0;
      nets [2] = No_InsSym;
    }

  /* extract the chunk marker */

  filter = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

  nv.len = 2;
  nets [0] = copy_fsm (lower);
  nets [1] = filter;
  Result = compose_fst (&nv, DO_RECLAIM, DO_MINIMIZE, DO_CLEANUP,
			PAIRWISE, FLAGS_NOT_SPECIAL);
  Result = lower_fsm (Result, DONT_COPY, DO_MINIMIZE);

  sigma_remove_from (NET_sigma (Result), INSERT_SYMBOL);
  sigma_remove_from (NET_labels (Result), INSERT_SYMBOL);

  return (Result);
}

static void epsilons_to_sigmas(NETptr net1, NETptr net2, NETptr eps)
{
  /* Distributes the special epsilon symbols, if any, into the sigma
     of both nets to prevent unwanted expansion of OTHER.
     */

  int i;

  for (i=0; i< ALPH_len (NET_labels (eps)); i++)
    {
      sigma_add_to (NET_sigma (net1), ALPH_item (NET_labels (eps), i));
      sigma_add_to (NET_sigma (net2), ALPH_item (NET_labels (eps), i));  
    }
}

static NETptr make_replacement(NETptr upper, NETptr lower,
                                       NETptr brs, NETptr eps, int ctx_dir)
{
  /* No components are consumed. */
   
  NVtype nv;
  NETptr nets[3];
  ALPHABETptr alph = sigma_union(NET_labels(brs), NET_labels(eps));
   
  nv.nets = nets;

  epsilons_to_sigmas(upper, lower, eps);

  binary_to_label(alph);
  
  nets[0] = ignore_alph (upper, alph, DO_COPY, DO_MINIMIZE, INTERNALLY,
			 UPPER, DONT_EXPAND);

  nets[1] = product(copy_fsm(upper), copy_fsm(lower));
  
  switch(ctx_dir)
    {
    case 0:	/* || */
    	/* Checks auxiliary symbols only on the upper side. */
      
      /*  UPi ./. [brs+eps .x. 0] .o. [UPi .x. LOWi] */
      nv.len = 2;
      break;
    default: /* //, \\, \/ */
   	 
      /*  UPi./. [brs+eps .x. 0] .o.
	  [UPi  .x. LOWi] .o.
	  LOWi./.[ 0 .x. brs]  */
     
      /* Epsilons checked on the upper side.. */

      nets[2] = ignore_alph(lower, NET_labels(brs), DO_COPY, DO_MINIMIZE,
      						INTERNALLY, LOWER, DONT_EXPAND);
      nv.len = 3;	 
    }
  free_alph(alph);
  return(compose_fst(&nv, DO_RECLAIM, DO_MINIMIZE, DONT_CLEANUP, PAIRWISE, FALSE));
}

static NETptr upper_to_lower
  (RE2N_VSTptr variables, NETptr upper, NETptr lower, int ctx_dir)
{
  /* Replaces or chunks one single UPPER.
     No arguments are consumed. */

  NVtype nv;
  NETptr Result, brs, aux, aux2, eps, A, B, n, nets [3];
  
  nv.nets = nets;

  /* case replacement:
  The general case is
   		UPi./.brs+eps
   		  .o.
   		brs+eps -> [] || ? _ ?
   		  .o.
   	   [UPi .x. LOWi]
   		  .o.
   	   [] <- brs || ? _ ?
   	      .o.
   	   LOWi./brs
   but we do not need to introduce brackets where they are not being checked.
   Thus in the most common case only the first three components are needed.
    */
  
  if (! sigma_member (NET_sigma (lower), INSERT_SYMBOL))
    {
      eps = RE2N_copy_var (variables, "epsilons");
      brs = RE2N_copy_var (variables, "<all|>all");
      
      Result = make_replacement(upper, lower, brs, eps, ctx_dir);

      free_network(eps);
      free_network(brs);
    }
    
  /* case: chunking */

  else
    {
      aux = RE2N_copy_var (variables, "eps+brs");
      aux2 = net_union(RE2N_copy_var (variables, "<all|>all"),
	      			   RE2N_copy_var (variables, "epsilons"));

      /* chunk:
	 [0 .x. [A/aux - [aux ?*]] upper./.brs [0 .x. [B/aux - [?* aux]]  */

      nv.len = 2;
      nets [0] = copy_fsm (aux);
      nets [1] = sigma_star_fsm ();
      n = concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE);  /*  [aux ?*]  */

      A = extract_chunk_marker (lower, TRUE);
      A = ignore_alph(A, NET_labels(aux), DONT_COPY, DO_MINIMIZE,
      				  EVERYWHERE, BOTH_SIDES, DONT_EXPAND);
      A = minus_fsm (A, n, DO_RECLAIM, DO_RECLAIM, DO_MINIMIZE);

      nets [0] = sigma_star_fsm ();
      nets [1] = copy_fsm (aux);
      n = concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE);  /*  [?* aux]  */

      B = extract_chunk_marker (lower, FALSE);
      B = ignore_alph(B, NET_labels(aux), DONT_COPY, DO_MINIMIZE,
      				  EVERYWHERE, BOTH_SIDES, DONT_EXPAND);
      B = minus_fsm (B, n, DO_RECLAIM, DO_RECLAIM, DO_MINIMIZE);

      if ((sigma_member (NET_sigma (A), OTHER)) ||
	  (sigma_member (NET_sigma (B), OTHER)))
	fsm_fprintf (stderr, "*** ERROR: Chunk markers must not contain OTHER.\n");

      nv.len = 3;
      nets [0] = product (epsilon_fsm(), A);  /* A consumed */
	  nets [1] = ignore_alph(upper, NET_labels(aux2), DO_COPY, DO_MINIMIZE,
	  						INTERNALLY, BOTH_SIDES, DONT_EXPAND);
      nets [2] = product (epsilon_fsm(), B);  /* B consumed */
      Result = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

      free_network(aux);
      free_network(aux2);
    }
  return (Result);
}

static NETptr STEP_replace
  (NVVptr repl_nvv, RE2N_VSTptr variables, int optional_p, int appl_brs_p, int ctx_dir)
{
  /* STEP of replacement: Replaces (or chunks) bracketed UPPER.

         [NoUpper ReplUpper]* NoUpper

     with
	 NoUpper = ?*                               ... if optional
         NoUpper = NoUp_1 & NoUp_2 & ... & NoUp_n   ... if obligatory
	 NoUp_i =  ~$[ <i [<all - <i]* [UPi./.><all] [>all - >i]* >i ]

	 ReplUpper = ReplUp_1 | ReplUp_2 | ... | ReplUp_n
	 ReplUp_i =
	  [<i [<all - <i]* [[UPi./.><all] .x. [LOWi./.><all]] [>all - >i]* >i]

     variables
         left = <i
	 right = >i
	 brs = ><all
         other_left = <all - <i
	 other_right = >all - >i
	 upperB = UPi ./. ><all

	 upper_to_lower = upper .x. lower             ... for replacement
	 upper_to_lower = [0 .x. A] upper [0 .x. B]   ... for chunking

     Brackets may simly be <i and >i  (non-determ. replacement)
     or they may be [ <AP <i ] and [ >i >AP ]  (determ. replacement).

     Note: We need not to ignore epsilons in UPPER and LOWER because.
     they were added to both nets in the preprocessing.
     No arguments are consumed. 
     */

  NVptr /*NoUp,*/ ReplUp;  NVtype nv;
  NETptr Result, NoUpper, ReplUpper, left, right, brs, other_left,
    other_right, upper, lower, nets [5];
  int i;

  nv.nets = nets;
  /*  NoUp = make_nv (NV_len (UPPER_NV (repl_nvv))); */
  ReplUp = make_nv (NV_len (UPPER_NV (repl_nvv)));

  brs = RE2N_copy_var (variables, "brackets");

  for (i=0; i< NV_len (ReplUp); i++)
    {
      /* prepare variables */

      left = copy_fsm (NV_net (LEFT_BR_NV (repl_nvv), i));
      right = copy_fsm (NV_net (RIGHT_BR_NV (repl_nvv), i));

      other_left = minus_fsm (RE2N_copy_var (variables, "<all|<AP"), left,
			      DO_RECLAIM, DONT_RECLAIM, DO_MINIMIZE);
      other_right = minus_fsm (RE2N_copy_var (variables, ">all|>AP"), right,
			       DO_RECLAIM, DONT_RECLAIM, DO_MINIMIZE);

      upper = copy_fsm (NV_net (UPPER_NV (repl_nvv), i));
      lower = copy_fsm (NV_net (LOWER_NV (repl_nvv), i));

      /* make ReplUp_i:  [ <i <other* [upper .x. lower] >other* >i ]  */

      nv.len = 5;
      nets [0] = copy_fsm (left);
      nets [1] = kleene_star (copy_fsm (other_left));
      nets [2] = upper_to_lower (variables, upper, lower, ctx_dir);
      nets [3] = kleene_star (copy_fsm (other_right));
      nets [4] = copy_fsm (right);
      NV_net (ReplUp, i) = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

      /* free variables */

      free_network (left);
      free_network (right);
      free_network (other_left);
      free_network (other_right);
      free_network (upper);
      free_network (lower);
    }

  /* make NoUpper (with application brackets if necessary) */

  if (optional_p)
    NoUpper = sigma_star_fsm ();
  else if (appl_brs_p)
    NoUpper = RE2N_copy_var (variables, "NoApBrUpper");
  else
    NoUpper = RE2N_copy_var (variables, "NoBrUpper");

  /* make ReplUpper (with application brackets if necessary) */

  ReplUpper = union_fsm (ReplUp, DONT_COPY, DO_MINIMIZE);

  if (appl_brs_p)
    {                      /* <AP ReplUpper >AP */
      nv.len = 3;
      nets [0] = RE2N_copy_var (variables, "<AP");
      nets [1] = ReplUpper;
      nets [2] = RE2N_copy_var (variables, ">AP");
      ReplUpper = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);
    }

  /* make the complete STEP:  [NoUpper ReplUpper]* NoUpper  */

  nv.len = 2;
  nets [0] = copy_fsm (NoUpper);
  nets [1] = ReplUpper;
  Result = concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE);

  nets [0] = kleene_star (Result);
  nets [1] = NoUpper;
  Result = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);
  
  /* free_nv_only (NoUp); */
  free_nv_only (ReplUp);
  free_network (brs);

  return (Result);
}

/**************
 *   additional steps for deterministic replacement
 */

static NETptr not_contains_ancre (RE2N_VSTptr locvars)
{
  /* Forbids ancre, and temporary application brackets.

         ~$[ @ | <AP | >AP ]
   */

  NVtype nv;  NETptr Result, nets [3];

  nv.nets = nets;
  nv.len = 3;
  nets [0] = RE2N_copy_var (locvars, "@");
  nets [1] = RE2N_copy_var (locvars, "<AP");
  nets [2] = RE2N_copy_var (locvars, ">AP");
  Result = union_fsm (&nv, DONT_COPY, DO_MINIMIZE);
  Result = min_fsm (negate (contains (Result)), DONT_COPY);

  return (Result);
}

static NETptr introduce_ancre (RE2N_VSTptr locvars)
{
  /* Introduces ancres on the left side of all possible applications.
     INSTEAD OF DOING THIS SIMPLY BY REPLACEMENT (RECURSIVE CALL):

         [. .] -> @ || \<all _ <all* BrUp

     WE HAVE TO USE A LESS ELEGANT SOLUTION WITHOUT REPLACEMENT
     (FOR EFFICIENCY REASONS):

         I .o. [ C1 & C2 & C3 ]

	 I  = [ ?* ] / [0 .x. @]
	 C1 = ~[  [ ?* @ ]       ~[ <all* BrUp./.@ ?* ] ]
	 C2 = ~[ ~[ ?* @ <all* ]         [BrUp./.@ ?* ] ]
	 C3 = ~$[ <all @ ]
     */

  NVtype nv;
  NETptr Result, I, C1, C2, C3, aux, BrUp_Anc, nets [3];

  nv.nets = nets;

  BrUp_Anc = ignore_alph(RE2N_copy_var (locvars, "BrUp"),
			 get_var_labels(locvars, "@"), DONT_COPY, DO_MINIMIZE,
			 INTERNALLY, BOTH_SIDES, DONT_EXPAND);

  /* make I = [ ?* ] / [0 .x. @] */

  aux = crossproduct_fst (epsilon_fsm(), RE2N_copy_var (locvars, "@"),
			  DO_RECLAIM, DO_MINIMIZE);
 
  I = ignore_alph(sigma_star_fsm(), NET_labels(aux), DONT_COPY, DO_MINIMIZE,
		  EVERYWHERE, BOTH_SIDES, DONT_EXPAND);
  free_network(aux);

  /* make C1 = ~[ [ ?* @ ] ~[ <all* BrUp./.@ ?* ] ] */

  nv.len = 3;
  nets [0] = kleene_star (RE2N_copy_var (locvars,"<all"));   /*  <all*  */
  nets [1] = copy_fsm (BrUp_Anc);                           /* BrUp./.@ */
  nets [2] = sigma_star_fsm ();
  C1 = concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE);
  C1 = negate_fsm (C1, DONT_COPY, DO_MINIMIZE);  /* ~[ <all* BrUp./.@ ?* ] ] */

  nets [0] = sigma_star_fsm ();
  nets [1] = RE2N_copy_var (locvars, "@");
  nets [2] = C1;
  C1 = concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE);
  C1 = negate_fsm (C1, DONT_COPY, DO_MINIMIZE);

  /* make C2 = ~[ ~[ ?* @ <all* ] [BrUp./.@ ?* ] ] */

  nets [0] = sigma_star_fsm ();
  nets [1] = RE2N_copy_var (locvars, "@");
  nets [2] = kleene_star (RE2N_copy_var (locvars,"<all"));   /*  <all*  */
  C2 = concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE);
  C2 = negate_fsm (C2, DONT_COPY, DO_MINIMIZE);     /* ~[ ?* @ <all* ] */

  nets [0] = C2;
  nets [1] = BrUp_Anc;                           /* BrUp./.@ */
  nets [2] = sigma_star_fsm ();
  C2 = concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE);
  C2 = negate_fsm (C2, DONT_COPY, DO_MINIMIZE);

  /* make C3 = ~$[ <all @ ] */

  nv.len = 2;
  nets [0] = RE2N_copy_var (locvars,"<all");
  nets [1] = RE2N_copy_var (locvars, "@");
  C3 = concat_fsm (&nv, DONT_COPY, DONT_MINIMIZE);
  C3 = contains_fsm (C3, DONT_COPY, DONT_MINIMIZE);
  C3 = negate_fsm (C3, DONT_COPY, DO_MINIMIZE);

  /* make final Result = I .o. [ C1 & C2 & C3 ] */

  nv.len = 3;
  nets [0] = C1;
  nets [1] = C2;
  nets [2] = C3;
  Result = intersect_fsm (&nv, DO_RECLAIM, DO_MINIMIZE);

  nv.len = 2;
  nets [0] = I;
  nets [1] = Result;
  Result = compose_fst (&nv, DO_RECLAIM, DO_MINIMIZE, DO_CLEANUP, PAIRWISE,
			FLAGS_NOT_SPECIAL);
  return (Result);
}

static NETptr mark_application (RE2N_VSTptr locvars)
{
  /* Marks all possible applications by application brackets.

         [ ~$@  @:<AP  BrUp./.@  0:>AP ]*  ~$@
     */

  NVtype nv;  NETptr Result, nets [4];

  nv.nets = nets;
  nv.len = 4;
  nets [0] = RE2N_copy_var (locvars, "~$@");
  nets [1] = product (RE2N_copy_var (locvars, "@"),
		      RE2N_copy_var (locvars, "<AP"));
  nets [2] = ignore_alph(RE2N_copy_var (locvars, "BrUp"),
  						 get_var_labels(locvars, "@"), DONT_COPY,
  						 DO_MINIMIZE, INTERNALLY, BOTH_SIDES, DONT_EXPAND);
  nets [3] = product (epsilon_fsm (), RE2N_copy_var (locvars, ">AP"));
  Result = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);
  Result = zero_plus (Result, DONT_COPY, DO_MINIMIZE);

  nv.len = 2;
  nets [0] = Result;
  nets [1] = RE2N_copy_var (locvars, "~$@");
  Result = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

  return (Result);
}

static NETptr delete_ancre (RE2N_VSTptr locvars)
{
  /* Deletes the ancres.

         @ -> 0
     */

  NETptr Result;

  Result = simple_replacement(RE2N_copy_var(locvars, "@"), epsilon_fsm(), DONT_COPY, 0);
  return (Result);
}

static NETptr longest_match (RE2N_VSTptr locvars)
{
  /* Allows only the longest possible application.

         ~$[ [ <AP  [<all - <AP]*  BrUp./.[<AP|>AP] ] & $[ >AP ] ]
   */

  NVtype nv;  NETptr Result, nets [3];

  nv.nets = nets;

  Result = minus_fsm (RE2N_copy_var (locvars, "<all"),
		      RE2N_copy_var (locvars, "<AP"),
		      DO_RECLAIM, DO_RECLAIM, DO_MINIMIZE);

  nv.len = 3;
  nets [0] = RE2N_copy_var (locvars, "<AP");
  nets [1] = kleene_star (Result);
  nets[2] = ignore_alph(RE2N_copy_var (locvars, "BrUp"),
			get_var_labels(locvars, "<AP|>AP"), DONT_COPY,
			DONT_MINIMIZE, INTERNALLY, BOTH_SIDES, DONT_EXPAND);
  Result = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

  nv.len = 2;
  nets [0] = Result;
  nets [1] = contains (RE2N_copy_var (locvars, ">AP"));
  Result = intersect_fsm (&nv, DO_RECLAIM, DO_MINIMIZE);
  Result = min_fsm (negate (contains (Result)), DONT_COPY);

  return (Result);
}

static NETptr min_strings(NETptr n)
{
  /* min_strings(N) is equivalent to [N - [N & [N ?+]]]  */

  STATEptr state;
  ARCptr arc, next;
  int num_arcs = NET_num_arcs(n);

  for (state = NET_states(n); state; state = next_state(state))
    if (STATE_final(state))
      {
	for (arc = STATE_arc_set(state); arc; arc = next)
	  {
	    next = next_arc(arc);
	    free_arc(arc);
	    NET_num_arcs(n)--;
	  }
	STATE_arc_set(state) = NULL;
      }

  if (num_arcs != NET_num_arcs(n))
    {
      NET_pruned(n) = FALSE;
      NET_minimized(n) = FALSE;

      label_check(n);

      prune_fsm(n, DONT_COPY);
    }

  return(n);
}

static NETptr shortest_match (RE2N_VSTptr locvars)
{
  /* Allows only the shortest possible application.

  The condition was originally

       ~$[ <AP  [<all - <AP]*  BrUp./.[<AP|>AP]  [$sym & ~$<AP]  >AP ]

  It has now been changed by LK (2005-02-14) to

   ~$[ <AP  [<all - <AP]*  min_strings(BrUp)./.[<AP|>AP]  [$sym & ~$<AP]  >AP ]

   min_strings(BrUp) is equivalent to [BrUp - [BrUp & [BrUp ?+]]].

   The original definition of shortest_match() gave incorrect results
   in cases such as

       (C) V (C) @> ... "-" || _ (C) V ;
       
   yielding a network whose input side language was not ?*. The bug
   was reported to us by Måans Huldén.
   */

  NVtype nv;  NETptr Result, brs, sym, nets [5];

  nv.nets = nets;

  brs = minus_fsm (RE2N_copy_var (locvars, "<all"),
		   RE2N_copy_var (locvars, "<AP"),
		   DO_RECLAIM, DO_RECLAIM, DO_MINIMIZE);
  brs = zero_plus (brs, DONT_COPY, DO_MINIMIZE);

  nv.len = 2;
  nets [0] = contains (RE2N_copy_var (locvars, "real_symbol"));
  nets [1] = negate (contains (RE2N_copy_var (locvars, "<AP")));
  sym = intersect_fsm (&nv, DO_RECLAIM, DO_MINIMIZE);

  nv.len = 5;
  nets [0] = RE2N_copy_var (locvars, "<AP");
  nets [1] = brs;
  nets [2] = ignore_alph(min_strings(RE2N_copy_var (locvars, "BrUp")),
                         get_var_labels(locvars, "<AP|>AP"), DONT_COPY,
                         DONT_MINIMIZE, INTERNALLY, BOTH_SIDES, DONT_EXPAND);
  nets [3] = sym;
  nets [4] = RE2N_copy_var (locvars, ">AP");
  Result = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);
  Result = min_fsm (negate (contains (Result)), DONT_COPY);

  return (Result);
}

static NETptr map_epsilon_application (RE2N_VSTptr locvars, int del_p)
{
  /* Subfunction to 'cancel_epsilon_application()'.
     Returns either:

         [ [<all|<AP:0]*  eps  [>all|>AP:0]* ]*      if del_p==TRUE
     or
         [ [<all|<AP  ]*  eps  [>all|>AP  ]* ]*      if del_p==FALSE
     */

  NVtype nv;
  NETptr Result, left, right, nets [4];

  nv.nets = nets;
  nv.len = 2;

  /* left brackets  [<all|<AP:0]*  */

  nets [0] = RE2N_copy_var (locvars, "<all");
  nets [1] = RE2N_copy_var (locvars, "<AP");

  if (del_p)
    nets [1] = crossproduct_fst (nets [1], epsilon_fsm(),
				 DO_RECLAIM, DO_MINIMIZE);   /*  <AP:0  */

  left = kleene_star (union_fsm (&nv, DONT_COPY, DO_MINIMIZE));

  /* right brackets  [>all|>AP:0]*  */

  nets [0] = RE2N_copy_var (locvars, ">all");
  nets [1] = RE2N_copy_var (locvars, ">AP");

  if (del_p)
    nets [1] = crossproduct_fst (nets [1], epsilon_fsm(),
				 DO_RECLAIM, DO_MINIMIZE);   /*  >AP:0  */

  right = kleene_star (union_fsm (&nv, DONT_COPY, DO_MINIMIZE));

  /* final Result */

  nv.len = 3;
  nets [0] = left;
  nets [1] = RE2N_copy_var (locvars, "epsilons");
  nets [2] = right;
  Result = kleene_star (concat_fsm (&nv, DONT_COPY, DO_MINIMIZE));

  return (Result);
}

static NETptr cancel_epsilon_application (RE2N_VSTptr locvars)
{
  /* Cancels epsilon replacement if adjacent to non-epsilon replacement,
     by deleting application brackets.
     INSTEAD OF DOING THIS SIMPLY BY REPLACEMENT (RECURSIVE CALL):

       >AP -> 0 || eps >all* _ [[eps|><all|><AP]* - [?* <AP]] <AP <all* sym

     WE HAVE TO USE A LESS ELEGANT SOLUTION WITHOUT REPLACEMENT
     (FOR EFFICIENCY REASONS):

       [ N  [ R | K ]]*  F

       N = ~$[ eps ]
       R = [ [<all|<AP:0]*  eps  [>all|>AP:0]* ]*  <AP <all* sym
       K = [ [<all|<AP  ]*  eps  [>all|>AP  ]* ]*      <all* sym
       F = ~$[ sym ]
     */

  NVtype nv;
  NETptr Result, N, R, K, F, nets [4];

  nv.nets = nets;

  /* make R */

  nv.len = 4;
  nets [0] = map_epsilon_application (locvars, TRUE);
  nets [1] = RE2N_copy_var (locvars, "<AP");
  nets [2] = kleene_star (RE2N_copy_var (locvars, "<all"));
  nets [3] = RE2N_copy_var (locvars, "real_symbol");
  R = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

  /* make K */

  nv.len = 3;
  nets [0] = map_epsilon_application (locvars, FALSE);
  nets [1] = kleene_star (RE2N_copy_var (locvars, "<all"));
  nets [2] = RE2N_copy_var (locvars, "real_symbol");
  K = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

  /* make N and F */

  N = negate (contains (RE2N_copy_var (locvars, "epsilons")));
  F = negate (contains (RE2N_copy_var (locvars, "real_symbol")));

  /* make final Result */

  nv.len = 2;
  nets [0] = R;
  nets [1] = K;
  R = union_fsm (&nv, DONT_COPY, DO_MINIMIZE);   /*  [ R | K ]  */

  nets [0] = N;
  nets [1] = R;
  R = kleene_star (concat_fsm (&nv, DONT_COPY, DO_MINIMIZE));/* [N [R|K]]*  */

  nets [0] = R;
  nets [1] = F;
  Result = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

  return (Result);
}

static NETptr cancel_nonepsilon_application
  (RE2N_VSTptr locvars)
{
  /* Cancels non-epsilon replacement if adjacent to epsilon replacement,
     by deleting application brackets.
     INSTEAD OF DOING THIS SIMPLY BY REPLACEMENT (RECURSIVE CALL):

         <AP -> 0 || eps >all* >AP _ <all* sym

     WE HAVE TO USE A LESS ELEGANT SOLUTION WITHOUT REPLACEMENT
     (FOR EFFICIENCY REASONS):

         [ N R ]* N

	 N = ~$[ eps >all* >AP <AP   <all* sym ]
	 R =     eps >all* >AP <AP:0 <all* sym
     */

  NVtype nv;
  NETptr Result, N, R, nets [6];

  nv.len = 6;
  nv.nets = nets;

  /* collect elements of N and R */

  nets [0] = RE2N_copy_var (locvars, "epsilons");           /*  eps    */
  nets [1] = kleene_star (RE2N_copy_var (locvars,">all"));  /*  >all*  */
  nets [2] = RE2N_copy_var (locvars, ">AP");                /*  >AP    */
  nets [3] = RE2N_copy_var (locvars, "<AP");                /*  <AP    */
  nets [4] = kleene_star (RE2N_copy_var (locvars,"<all"));  /*  <all*  */
  nets [5] = RE2N_copy_var (locvars, "real_symbol");        /*  sym    */

  /* build N */

  N = concat_fsm (&nv, DO_COPY, DO_MINIMIZE);  /* still keep all elements */
  N = contains_fsm (N, DONT_COPY, DO_MINIMIZE);
  N = negate_net (N, DONT_COPY, DO_MINIMIZE);

  /* build R */

  nets [3] = crossproduct_fst (nets [3], epsilon_fsm(),
			       DO_RECLAIM, DO_MINIMIZE);    /*  <AP:0  */
  R = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

  /* build final Result */

  nv.len = 2;
  nets [0] = copy_fsm (N);
  nets [1] = R;

  nets [0] = kleene_star (concat_fsm (&nv, DONT_COPY, DO_MINIMIZE));
  nets [1] = N;
  Result = concat_fsm (&nv, DONT_COPY, DO_MINIMIZE);

  return (Result);
}

static NETptr STEP_constrain_application
  (RE2N_VSTptr variables, int left_ancre_p, int longest_match_p)
{
  /* Constrains all occurences of application brackets, from left to right.
     For right-to-left application the result is reverted.

       NoAnc .o. IntrAnc .o. MarkApp .o. DelAnc .o. Length .o. Cancel

     with

       NoAnc   = ~$[ @ | <AP | >AP ]
       IntrAnc = [. .] -> @ || \<all _ <all* BrUp
       MarkApp = [ ~$@  @:<AP  BrUp./.@  0:>AP ]*  ~$@
       DelAnc  = @ -> 0

     for Longest Match:

       Length  = ~$[ [ <AP  [<all - <AP]*  BrUp./.[<AP|>AP] ] & $[ >AP ] ]
       Cancel  = >AP -> 0 ||
                   eps >all* _ [[eps|><all|><AP]* - [?* <AP]] <AP <all* sym

     for Shortest Match:

       Length  = ~$[ <AP [<all - <AP]* BrUp./.[<AP|>AP] [$sym & ~$<AP] >AP ]
       Cancel  = <AP -> 0 || eps >all* >AP _ <all* sym

     with

       BrUp    .... union of all UPPER with context brackets: <i Ui >i
       @       .... ancre, on left side of every possible application
       <AP >AP .... application brackets
       <all    .... union of all left context brackets <i
       <all    .... union of all right context brackets <i
       ><all   .... union of all left and right context brackets <i >i
       ><AP    .... union of both application brackets <AP >AP
       eps     .... union of single and multiple epsilon
       sym     .... real symbol (not an auxiliary like @ <AP eps ...)

     All computation is done for left-to-right application.
     In case of right-to-left application, we reverse all arguments,
     exchange left with right brackets, and reverse the final result.
     */

  RE2N_VSTptr locvars;
  NVtype nv;  NETptr net, Result, nets [6];
  ALPHABETptr sigma;  
  id_type ancre_ID;

  nv.nets = nets;
  locvars = RE2N_create_var_stack ();

  FST_intern_literal (&ancre_ID, "++++ANCRE++++");

  /* make local variables (networks) */

  RE2N_save_var (locvars, "@", symbol_fsm (ancre_ID));
  RE2N_save_var (locvars, "<AP|>AP", RE2N_copy_var (variables,"<AP|>AP"));
  RE2N_save_var (locvars, "<all|>all", RE2N_copy_var (variables,"<all|>all"));
  RE2N_save_var (locvars, "epsilons", RE2N_copy_var (variables,"epsilons"));
  RE2N_save_var (locvars, "eps+brs", RE2N_copy_var (variables, "eps+brs"));
  RE2N_save_var (locvars, "real_symbol",
		 RE2N_copy_var (variables,"real_symbol"));

  net = symbol_fsm (ancre_ID);
  net = negate_net (contains (net), DONT_COPY, DO_MINIMIZE);
  RE2N_save_var (locvars, "~$@", net);

  if (left_ancre_p)
    {
      RE2N_save_var (locvars, "BrUp", RE2N_copy_var (variables, "BrUpper"));
      RE2N_save_var (locvars, "~$BrUp",RE2N_copy_var (variables,"NoBrUpper"));

      RE2N_save_var (locvars, "<AP", RE2N_copy_var (variables, "<AP"));
      RE2N_save_var (locvars, ">AP", RE2N_copy_var (variables, ">AP"));

      RE2N_save_var (locvars, "<all", RE2N_copy_var (variables, "<all"));
      RE2N_save_var (locvars, ">all", RE2N_copy_var (variables, ">all"));
    }
  else
    {
      RE2N_save_var (locvars, "BrUp",
		     reverse_fsm (RE2N_copy_var (variables, "BrUpper"),
				  DONT_COPY, DO_MINIMIZE));
      RE2N_save_var (locvars, "~$BrUp",
		     reverse_fsm (RE2N_copy_var (variables, "NoBrUpper"),
				  DONT_COPY, DO_MINIMIZE));

      RE2N_save_var (locvars, "<AP", RE2N_copy_var (variables, ">AP"));
      RE2N_save_var (locvars, ">AP", RE2N_copy_var (variables, "<AP"));

      RE2N_save_var (locvars, "<all", RE2N_copy_var (variables, ">all"));
      RE2N_save_var (locvars, ">all", RE2N_copy_var (variables, "<all"));
    }

  /* NoAnc .o. IntrAnc .o. MarkApp .o. DelAnc .o. Length .o. Cancel */

  nv.len = 6;
  nets [0] = not_contains_ancre (locvars);
  nets [1] = introduce_ancre (locvars);
  nets [2] = mark_application (locvars);
  nets [3] = delete_ancre (locvars);

  if (longest_match_p)
    {
      nets [4] = longest_match (locvars);
      nets [5] = cancel_epsilon_application (locvars);
    }
  else
    {
      nets [4] = shortest_match (locvars);
      nets [5] = cancel_nonepsilon_application (locvars);
    }

  Result = compose_fst (&nv, DO_RECLAIM, DO_MINIMIZE, DO_CLEANUP, PAIRWISE,
			FLAGS_NOT_SPECIAL);

  /* reverse if right-to-left application */

  if (! left_ancre_p)
    Result = reverse_fsm (Result, DONT_COPY, DO_MINIMIZE);

  /* clean sigma:  remove @  */

  sigma = label_to_binary (NET_sigma (Result));
  ALPH_item (sigma, ancre_ID) = FALSE;

  RE2N_remove_var_stack (locvars, TRUE);

  return (Result);
}

/**************
 *   complete replacement
 */

NETptr cond_repl_unidir (RSptr repl_rs, int upwards_p, int depth)
{
  /* Replaces in parallel all UPPERi unidirectionally by the corresponding
     LOWERi if (upwards_p==FALSE) or vice versa if (upwards_p==TRUE)
     in the context of LEFTi and RIGHTi.
     This is in case of upwards_p==FALSE :

       UP1 -> LOW1 || L1 _ R1 , ... , UPn -> LOWn || Ln _ Rn

     This is computed by:

         InsertAux
     .o. ConstrainAux
     .o. LeftContext
     .o. RightContext
     .o. Replace
     .o. RemoveAux

     'upwards_p' does not need to correspond with the arrow in the
     replacement structure RS_arrow(repl_rs), which here will not be taken
     into account to decide on the replacement direction !!!

     No arguments are consumed.

     Argument 'depth': Depth of embedding of replacements.
     It allows to use replacement as sub-operation inside replacement,
     without conflict of context brackets or other auxiliary symbols.
     */

  RE2N_VSTptr variables;
  NVVptr repl_nvv; NVtype nv;
  NETptr InsertAux, ConstrainAux, LeftContext, RightContext, Replace,
    RemoveAux, Result, FirstSteps, MiddleSteps, LastSteps, nets [6];
  int size_ids, single_eps_p, multi_eps_p,
    optional_p, determ_p = 0, longest_match_p = FALSE, left_ancre_p = FALSE;
  id_type *br_ids[2];
  int ctx_dir = RS_ctxdir (repl_rs);

  nv.nets = nets;
  variables = RE2N_create_var_stack ();

  /* check recursion depth */

  if (depth > MAX_RECURSION)
    {
      fsm_fprintf (stderr, "*** PROGRAMMING ERROR in Parallel Replacement:\n\
Recursion depth > %d reached\n\n\
PLEASE, WARN THE PROGRAMMER.\n\n", depth);
      return (null_fsm ());
    }

  /* write all information from 'repl_rs' to 'repl_nvv' and make brackets */

  size_ids = sizeof(id_type) * RS_len(repl_rs)/4;
  LEFT_BR_IDS (br_ids) = (id_type *) malloc (size_ids);
  RIGHT_BR_IDS (br_ids) = (id_type *) malloc (size_ids);

  repl_nvv = make_nvv(6);
  repl_struct_to_nvv (repl_rs, repl_nvv, br_ids, upwards_p, depth);

  /* extract arrow information from the replacement structure */

  optional_p = RS_arrow (repl_rs) / 10;

  switch (RS_arrow (repl_rs) % 10)
    {
    case 0:    /*  0: ->  */
    case 1:    /*  1: <-  */
    case 2:    /*  2: <->  */
      determ_p = FALSE;
      break;
    case 3:    /*  3: @->  */
      determ_p = TRUE;
      longest_match_p = TRUE;
      left_ancre_p = TRUE;
      break;
    case 4:    /*  4: ->@  */
      determ_p = TRUE;
      longest_match_p = TRUE;
      left_ancre_p = FALSE;
      break;
    case 5:    /*  5: @>   */
      determ_p = TRUE;
      longest_match_p = FALSE;
      left_ancre_p = TRUE;
      break;
    case 6:    /*  6: >@   */
      determ_p = TRUE;
      longest_match_p = FALSE;
      left_ancre_p = FALSE;
      break;
    case 7:    /*  7: @<-  */
      determ_p = TRUE;
      longest_match_p = TRUE;
      left_ancre_p = TRUE;
      break;
    case 8:    /*  8: <-@   */
      determ_p = TRUE;
      longest_match_p = TRUE;
      left_ancre_p = FALSE;
      break;
    case 9:    /*  9: @<   */
      determ_p = TRUE;
      longest_match_p = FALSE;
      left_ancre_p = TRUE;
      break;
    case -1:    /*  6: <@   */
      determ_p = TRUE;
      longest_match_p = FALSE;
      left_ancre_p = FALSE;
      if (optional_p == -1)
	optional_p = 1;
      break;
    }

  /* other preprocessing */

             /* Equivalent context pairs get the same bracket pairs */
  handle_equivalent_contexts (repl_nvv, br_ids);
  put_first_auxiliaries_on_stack (repl_nvv, variables, determ_p, depth);

  if (! preprocess_upper_nets(repl_nvv, variables, &single_eps_p, &multi_eps_p))
    return (null_fsm());  /* ERROR occured in input nets */
    
  put_more_auxiliaries_on_stack (variables, single_eps_p, multi_eps_p);
    
  if (! preprocess_context_nets(repl_nvv, variables))
    return (null_fsm());  /* ERROR occured in input nets */

  union_all_bracketed_upper (repl_nvv, variables, determ_p);

  /* steps of non-deterministic replacement */

  RemoveAux = STEP_remove_auxiliaries (variables);
  InsertAux = invert_fst (RemoveAux, DO_COPY);
  ConstrainAux =
    STEP_constrain_auxiliaries (variables, single_eps_p, multi_eps_p);

  LeftContext = STEP_left_context (repl_nvv, variables, TRUE, single_eps_p, multi_eps_p);
  RightContext = STEP_left_context (repl_nvv, variables, FALSE, single_eps_p, multi_eps_p);
  Replace = STEP_replace (repl_nvv, variables, optional_p, determ_p, ctx_dir);

  /* compose FirstSteps */

  nets [0] = InsertAux;
  nets [1] = ConstrainAux;

  if (determ_p)
    {
      nv.len = 3;
      nets [2] = STEP_constrain_application
	(variables, left_ancre_p, longest_match_p);
    }
  else
    nv.len = 2;

  FirstSteps = compose_fst (&nv, DO_RECLAIM, DO_MINIMIZE, DO_CLEANUP,
			    PAIRWISE, FLAGS_NOT_SPECIAL);

  /* compose LastSteps */

  LastSteps = RemoveAux;

  /* compose MiddleSteps */

  switch (ctx_dir)  /*  ||  //  \\  \/  */
    {
    case 0:     /*  || ,  [Left & Right] .o. Replace  */
      nets [0] = intersect(LeftContext, RightContext);
      nets [1] = Replace;
      nv.len = 2;
      break;
    case 1:     /*  // ,  Right .o. Replace .o. Left  */
      nets [0] = RightContext;
      nets [1] = Replace;
      nets [2] = LeftContext;
      nv.len = 3;
      break;
    case 2:     /*  \\ ,  Left .o. Replace .o. Right  */
      nets [0] = LeftContext;
      nets [1] = Replace;
      nets [2] = RightContext;
      nv.len = 3;
      break;
    case 3:     /*  \/ ,  Replace .o. Left .o. Right  */
      nets [0] = Replace;
      nets [1] = LeftContext;
      nets [2] = RightContext;
      nv.len = 3;
      break;
    default:
      print_error("wrong context direction");
      break;
    }

  MiddleSteps = compose_fst (&nv, DO_RECLAIM, DO_MINIMIZE, DO_CLEANUP,
			     PAIRWISE, FLAGS_NOT_SPECIAL);

  /* compose all steps --> final result */

  nv.len = 3;
  nets [0] = FirstSteps;
  nets [1] = MiddleSteps;
  nets [2] = LastSteps;
  Result = compose_fst (&nv, DO_RECLAIM, DO_MINIMIZE, DO_CLEANUP, PAIRWISE,
			FLAGS_NOT_SPECIAL);

  /* finish */

  if (upwards_p == TRUE)          /*  <-  or  (<-)  */
    Result = invert_fst (Result, DONT_COPY);

  clean_sigma_from_auxiliaries (Result, variables);
  compact_sigma (Result);

  RE2N_remove_var_stack (variables, TRUE);
  free_nvv_and_nvs_and_nets (repl_nvv);
  free (LEFT_BR_IDS(br_ids));
  free (RIGHT_BR_IDS(br_ids));

  return (Result);
}


static NETptr cond_repl_bidir (RSptr repl_rs)
{
  /* Replaces in parallel all UPPERi bidirectionally by the 
     corresponding LOWERi in the context of LEFTi and RIGHTi:

       UP1 <-> LOW1 || L1 _ R1 , ... , UPn <-> LOWn || Ln _ Rn

     This is computed by:

     ~$[ AUX1 | ... | AUXn | AUX1E | ... | AUXnE ]            (net: no_aux)
        .o.
     UP1 -> AUX1 || L1 _ R1 ,  ... , UPn -> AUXn || Ln _ Rn   (net: UP_AUX)
        .o.
     AUX1 <- LOW1 || L1 _ R1 , ... , AUXn <- LOWn || Ln _ Rn  (net: AUX_LOW)
        .o.
     ~$[ AUX1 | ... | AUXn | AUX1E | ... | AUXnE ]            (net: no_aux)

     The replacements can all be optional and with the context
     directions:  ||  //  \\  \/ . No arguments are consumed.
     */

  NVtype nv;  NVptr aux, repl2;
  NETptr nets[4], no_aux, UP_AUX, AUX_LOW, net,  Result;
  int i, U, L, aux_len = RS_len (repl_rs) /4;
  id_type *aux_id;

  /* prepare some variables */

  nv.nets = nets;

  aux = make_nv (aux_len);
  aux_id = (id_type *) malloc (aux_len * sizeof(id_type));
  make_aux_symbols(aux, aux_id, "*CPR-AUX", "*");

  /* make  no_aux = ~$[ AUX1 | ... | AUXn ]  */

  net = union_fsm(aux, DO_COPY, DONT_MINIMIZE);
  no_aux = negate_net (contains_fsm (net, DONT_COPY, DO_MINIMIZE),
		       DONT_COPY, DO_MINIMIZE);

  /* make UP_AUX */

  repl2 = copy_net_vector (RS_nv (repl_rs));

  for (i=0, L=1; i< aux_len; i++, L+=4)
    {
      free_network (RS_net (repl_rs, L));              /* free LOWERi */
      RS_net (repl_rs, L) = copy_fsm (NV_net (aux, i));  /* put AUXi instead */
    }

  UP_AUX = cond_repl_unidir (repl_rs, FALSE, 0);    /* Ui -> AUXi */

  free_nv_and_nets (RS_nv (repl_rs));
  RS_nv (repl_rs) = repl2;           /* restore old vector */

  /* make AUX_LOW */

  repl2 = copy_net_vector (RS_nv (repl_rs));

  for (i=0, U=0; i< aux_len; i++, U+=4)
    {
      free_network (RS_net (repl_rs, U));              /* free UPPERi */
      RS_net (repl_rs, U) = copy_fsm (NV_net (aux, i));  /* put AUXi instead */
    }

  AUX_LOW = cond_repl_unidir (repl_rs, TRUE, 0);    /* AUXi <- Li */

  free_nv_and_nets (RS_nv (repl_rs));
  RS_nv (repl_rs) = repl2;           /* restore old vector */

  /* compose the four steps */

  nv.len = 4;
  nets[0] = copy_fsm(no_aux);
  nets[1] = UP_AUX;
  nets[2] = AUX_LOW;
  nets[3] = no_aux;

  Result = compose_fst(&nv, DO_RECLAIM, DO_MINIMIZE, DO_CLEANUP, PAIRWISE,
		       FLAGS_NOT_SPECIAL);

  /* remove the auxiliary symbols from the sigma of the Result */

  for(i=0; i< aux_len; i++)
    sigma_remove_from (NET_sigma (Result), aux_id [i]);

  /* free nets and variables */

  free_nv_and_nets(aux);
  free(aux_id);

  return(Result);
}

NETptr conditional_replace (RSptr repl_rs, int reclaim_p)
{
  /* Makes in parallel all replacements noted in 'RSptr repl'.
     The net vektor 'RS_nv(repl)' contains:

       UP1 LOW1 L1 R1  ...  UPn LOWn Ln Rn

       1 arrow:         ->    <-    <->      @->    @>    ->@    >@
          or optional  (->)  (<-)  (<->)    (@->)  (@>)  (->@)  (>@)

       1 context direction:    ||  //  \\  \/

       (All parallel replacements have this one arrow and context
       direction in common.)

     In case of '->' and '||' the replacements can be noted as:

       UP1 -> LOW1 || L1 _ R1 ,  ...  , UPn -> LOWn || Ln _ Rn
     */

  NETptr Result;

  /* check whether all arguments are simple fsm, and no transducers */

  if (NV_fst_check( RS_nv(repl_rs) ))
    {
      print_error("All UPPER, LOWER, LEFT and RIGHT must be simple fsm, and no transducers.\n");
                                   /* free the function argument */
      if(reclaim_p == DO_RECLAIM)
	free_rs(repl_rs);
      return(sigma_star_fsm());
    }

  /* handle the different arrows */

  switch (RS_arrow (repl_rs) % 10)  /* eliminate optionality information */
    {
    case 1:    /*  1: <-  */
    case 7:    /*  7: @<-  */
    case 8:    /*  8: <-@  */
    case 9:    /*  9: @<   */
    case -1:   /* -1: <@   */
      Result = cond_repl_unidir (repl_rs, TRUE, 0);
      break;
    case 2:    /*  2: <->  */
      Result = cond_repl_bidir (repl_rs);
      break;
    case 0:    /*  0: ->   */
    case 3:    /*  3: @->  */
    case 4:    /*  4: ->@  */
    case 5:    /*  5: @>   */
    case 6:    /*  6: >@   */
      Result = cond_repl_unidir (repl_rs, FALSE, 0);
      break;
    default:
      fsm_fprintf (stderr, "*** ERROR in Parallel Replacement:\n\
Arrow variable must not have value %d.\n\n", RS_arrow (repl_rs));
      Result = null_fsm ();
      break;
    }

  /* smash argument if required */

  if (reclaim_p == DO_RECLAIM)
    free_rs (repl_rs);

  /* Result = LEP_local_eps_push_fst (Result, DO_RECLAIM, DO_MINIMIZE);
     This function is not very seccessful. It could be tried with
     sequentialization.  04/11/98 AK */

  compact_sigma(Result);

 return (Result);
}

/****************************************
 *
 *  FUNCTIONS FOR THE
 *  CONDITIONAL PARALLEL DETERMINISTIC REPLACEMENT
 *  IN MULTIPLE CONTEXTS
 *
 *  { UP1 @-> LOW1 , ... , UPn @-> LOWn } || { L1 _ R1 , ... , Lm _ Rm } ,
 *        ......... ,
 *        ......... ,
 *
 *  { UPx @-> LOWx , ... , UPy @-> LOWy } || { Lp _ Rp , ... , Lq _ Rq } ;
 *
 *
 *    arrow:              @->   ->@   @>   >@
 *                       (@->) (->@) (@>) (>@)
 *    context direction:  ||  //  \\  \/
 *     (arrow and context directions must be the same in all replacements)
 *
 ***********************************/

NETptr CPDR_cond_determ_replace (void)
{
  fsm_fprintf (stderr, "\n\
\ttest01 functions for\n\
\tConditional Parallel Deterministic Replacement\n\
\tare not implemented yet.\n\n");

  return (sigma_star_fsm());
}


/****************************************
 *
 *  FUNCTIONS FOR THE RESTRICT OPERATION IN MULTIPLE CONTEXTS
 *
 *  CENTER  arrow  LEFT1 _ RIGHT1 , ... , LEFTn _ RIGHTn
 *
 *    arrow:        =>  <=  <=>
 *
 ***********************************/


static int RST_fst_restrict_p(NETptr center, NVptr ctxlist)
{
  int error_p = FALSE;

  if (fst_check(center, "CENTER", "restrict"))  error_p = TRUE;

  if (NV_fst_check(ctxlist))
    {
      fsm_fprintf(stderr, "**** Every context of the restrict operation\n");
      fsm_fprintf(stderr, "      has to be a simple fsm and not a transducer.\n");
      error_p = TRUE;
    }

  return(error_p);
}

static NETptr RST_restrict_center(NETptr center, NVptr ctxlist)
{
  /* This algorithm is due to Anssi Yli-Jyrä (aylijyra@ling.helsinki.fi).
     It is presented in a yet unpublished paper "A Simple Compilation
     method for the Context Restriction Operator". The paper points
     out that the previous compilation method is not correct and that
     the more complex method presented in the 1994 Kaplan and Kay paper
     is also erroneous. This function replaces the one below,
     RST_restrict_unidir(), except for the rarely used
     <= operator. 2003-05-12. LK */

  NETptr work_net, ok_net, star_net, lc, rc;
  STATEptr state;
  ARCptr arc;
  NVtype nv;
  NETptr nets[5];
  int i;
  id_type lb, rb, id;

  nv.nets = nets;
  nv.len = 5;

  /* Auxiliary brackets are supposed not to occur in the sigma.of any
     of the argument networks. "*<<*" and "*>>*". */

  FST_intern_literal(&lb, "*<<*");
  FST_intern_literal(&rb, "*>>*");

  /* We add the auxiliary symbols to the sigma of all the
     component nets to prevent unwanted OTHER expansion. */

  star_net = sigma_star_fsm();

  sigma_add_to(NET_sigma(star_net), lb);
  sigma_add_to(NET_sigma(star_net), rb);
  sigma_add_to(NET_sigma(center), lb);
  sigma_add_to(NET_sigma(center), rb);

  /* At start our WORK_NET consists of
     [STAR_NET *<<* CENTER *>>* STAR_NET]
     where *<<* and *>>* do not occur in any of the three
     components although they are included in their sigmas. */

  nv.nets[0] = copy_fsm(star_net);
  nv.nets[1] = symbol_fsm(lb);
  nv.nets[2] = copy_fsm(center);
  nv.nets[3] = symbol_fsm(rb);
  nv.nets[4] = copy_fsm(star_net);

  work_net = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);
    
  for (i = 0; i < NV_len(ctxlist); i+=2) {

    if (NV_net(ctxlist, i))     /* Left context */
      {
	lc = concat(copy_fsm(star_net), copy_fsm(NV_net(ctxlist, i)));

	if (sigma_member(NET_sigma(lc), BOUNDARY_SYMBOL))
	  insert_left_ctxboundary(lc, DONT_COPY);
      }
    else
      lc = copy_fsm(star_net);

    if (NV_net(ctxlist, i+1))   /* Right context */
      {
	rc = concat(copy_fsm(NV_net(ctxlist, i+1)), copy_fsm(star_net));
	
	if (sigma_member(NET_sigma(rc), BOUNDARY_SYMBOL))
	  insert_right_ctxboundary(rc, DONT_COPY);
      }
    else
      rc = copy_fsm(star_net);

    /* Our OK_NET consists of
       [STAR_NET (LC) *<<* STAR_NET *>>* (RC) STAR_NET]
       where the five components do not contain *<<* *>>*. */

    nv.nets[0] = lc;
    nv.nets[1] = symbol_fsm(lb);
    nv.nets[2] = copy_fsm(star_net);
    nv.nets[3] = symbol_fsm(rb);
    nv.nets[4] = rc;

    ok_net = concat_fsm(&nv, DONT_COPY, DO_MINIMIZE);

    /* We subtract from WORK_NET all the strings which contain
       a correctly bracketed LC ... RC pair. Consequently,
       WORK_NET eventually contains no instances of CENTER
       that appears in the proper context. */

    work_net = minus_fsm(work_net, ok_net, DO_RECLAIM, DO_RECLAIM,
			 DO_MINIMIZE);
  }

  /* We no longer need STAR_NET. */

  free_network(star_net);

  /* We can now eliminate the auxiliary symbols from WORK_NET.*/

  for (state = NET_states(work_net);state;state=next_state(state)) {
    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc)) {
      id = ARC_label(arc);
      if (id == lb || id == rb) {
	ARC_label(arc) = EPSILON;
	STATE_deterministic(state) = FALSE;
	NET_epsilon_free(work_net) = FALSE;
      }
    }
  }

  /* At this point, WORK_NET contains only invalid strings, that is,
     strings where CENTER does not appear in a proper context.
     For the last step we need to at least determinize WORK_NET. */

  NET_epsilon_free(work_net) = FALSE;
  NET_deterministic(work_net) = FALSE;
  NET_minimized(work_net) = FALSE;
    
  /* Finally, we eliminate the auxiliary symbols from the sigmas of WORK_NET
     and CENTER (the calling function might expect an unmodified CENTER). */

  sigma_remove_from(NET_sigma(work_net), lb);
  sigma_remove_from(NET_sigma(work_net), rb);
  sigma_remove_from(NET_sigma(center), lb);
  sigma_remove_from(NET_sigma(center), rb);

  /* WORK_NET now contains all and only strings where CENTER appears
     in some invalid environment. We return the language of its
     complement. */

  return(negate_fsm(work_net, DONT_COPY, DO_MINIMIZE));
}

static NETptr RST_restrict_context(NETptr center, NVptr ctxlist)
{
  /* The original algorithm was buggy in both => and <= directions.
     In the => direction, it yielded incorrect results in cases
     such as
     
     (a) b (c) => x (a) _ , _ (c) y ;

     The => expressions are now compiled by the function above,
     RST_restrict_center(), that implements Anssi Yli-Jyrä's algorithm.
     2003-05-12 LK

     The original compilation for expressions such as

     a <= b _ c;

     had two problems. The resulting network accepted strings
     such as "bc" with no intervening "a". Secondly, strings
     such as "bacbac" were rejected because the outermost
     frame "b ... c" was filled with "acba" even though the
     two occurrences of "bac" satisfy the constraint.

     The fix for <= was written by LK in 2005-02-12.

     Here is the new formula for compiling CENTER <= LEFT _ RIGHT:
     `[
       [        ~$[OK]                                    # Step 1    
                 .o.                                 
       [. CENTER .] -> OK ... OK || LEFT _ RIGHT          # Step 2
                 .o.
       [ .[~CENTER && ~$OK] .] -> \? || LEFT _ RIGHT      # Step 3
      ].u,                                                # Step 4
      OK, 0];                                             # Step 5
      
     The first step declares that the auxiliary OK symbol is not in
     the input alphabet.  The second step marks all pairs of LEFT and
     RIGHT surrounding CENTER with OK to indicate that their restriction
     is satisfied. The third step maps the unfulfilled LEFT _ RIGHT pairs
     to a null fsm thus eliminating them from the result. In step 4
     we take the upper language of the result and, finally, in step 5
     auxiliary OK symbol is eliminated. Note the use of [. .] in the
     second and third step.

     No arguments are consumed.
  */
  RSptr rs, uplow;
  NVtype nv;
  NETptr nets[3], xcenter, result;
  id_type ok_mark;

  FST_intern_literal(&ok_mark, "*RST-OK*");

  nv.nets = nets;  nv.len = 3;

  /* Make the three nets, compose them, take the upper side
     and eliminate the auxiliary ok_mark from the sigma. */

  /*  ~$[OK]                                           Step 1  */

  nets[0] = negate_net(contains(symbol_fsm(ok_mark)),
		       DONT_COPY, DO_MINIMIZE);

  xcenter   = min_fsm(center, DO_COPY);

  /* Set the single epsilon flag if the center includes an empty string. */

  if (STATE_final(NET_start_state(xcenter)))
    NET_single_epsilon(xcenter) = TRUE;
  else
    NET_single_epsilon(xcenter) = FALSE;   /* Just in case ... */

  uplow = rs_upper_lower(xcenter, symbol_fsm(ok_mark), 0);
  /* uplow contains: xcenter -> aux */
  rs = crossprod_uplow_leftright(uplow, ctxlist, 0, DONT_RECLAIM);
  /* rs contains: xcenter -> aux || L1 _ R1, ... , Ln _ Rn */
  free_rs(uplow);


  /* [. CENTER .] -> OK ... OK || LEFT _ RIGHT          Step 2 */

  nets[1] = conditional_replace(rs, DO_RECLAIM);

  xcenter = intersect(negate(copy_fsm(center)),
                      negate(contains(symbol_fsm(ok_mark))));

  /* Set the single eps flag because the center includes an empty string. */

  NET_single_epsilon(xcenter) = TRUE;

  uplow = rs_upper_lower(xcenter, null_fsm(), 0);

  rs = crossprod_uplow_leftright(uplow, ctxlist, 0, DONT_RECLAIM);

  free_rs(uplow);

  /* [ .[~CENTER && ~$OK] .] -> \? || LEFT _ RIGHT      Step 3 */

  nets[2] = conditional_replace(rs, DO_RECLAIM);


  result = compose_fst(&nv, DO_RECLAIM, DO_MINIMIZE, DO_CLEANUP, PAIRWISE,
		       FLAGS_NOT_SPECIAL);

  /*  Step 4 */

  upper_fsm(result, DONT_COPY, DO_MINIMIZE);

  /*  Step 5 */

  sigma_remove_from(NET_sigma(result), ok_mark);

  return(result);
}

NETptr RST_restrict_in_multctx
(NETptr center, NVptr ctxlist, int arrow, int reclaim_p)
{
  /* CENTER, LEFT, and RIGHT must be simple fsms, not transducers.
     Constrains CENTER, LEFT, and RIGHT with respect to each other.
     The type of constraint depends on the type of arrow. The
     options for 'arrow' are:

     0   =>
     1  <=
     2  <=>

     Constraint definitions:

     CENTER => LEFT1 _ RIGHT1 , ... , LEFTn _ RIGHTn
         cf. function  'RST_restrict_center(...)'

     CENTER <= LEFT1 _ RIGHT1 , ... , LEFTn _ RIGHTn
         cf. function  'RST_restrict_context(...)'

     CENTER <=> LEFT1 _ RIGHT1 , ... , LEFTn _ RIGHTn
        means
	    [ CENTER => LEFT1 _ RIGHT1 , ... , LEFTn _ RIGHTn ]
	  & [ CENTER <= LEFT1 _ RIGHT1 , ... , LEFTn _ RIGHTn ]

     If reclaim_p is DONT_RECLAIM, CENTER and CTXLIST are preserved,
     otherwise they are smashed at the end.
     */

  NETptr result;

  /* CENTER and all nets of CTXLIST must be simple fsm, no transducers */

  if(RST_fst_restrict_p(center, ctxlist))
    {
      fsm_fprintf(stdout, "    Aborting. Returning [?*] instead.\n");
      if (reclaim_p == DO_RECLAIM)
        { free_network(center);  free_nv_and_nets(ctxlist); }
      return (sigma_star_fsm());
    }

  switch (arrow)
    {
    case 0:    /*  =>  */
      result = RST_restrict_center(center, ctxlist);
      break;
    case 1:    /*  <=  */
	result = RST_restrict_context(center, ctxlist);
      break;
    case 2:    /*  <=>  */
      result = intersect(RST_restrict_center(center, ctxlist),
			 RST_restrict_context(center, ctxlist));
      break;
    default:
      handle_error("Wrong arrow type", 
		   "RESTRICTION IN MULTIPLE CONTEXTS", arrow);
      result = sigma_star_fsm();
      break;
    }

  if (reclaim_p == DO_RECLAIM)
    { free_network(center);  free_nv_and_nets(ctxlist); }

  return (result);
}


/***************************************
 *
 *   Test Functions. To delete later.
 *
 **************************/

#ifdef DEBUG

#include "fst/debug.h"

RSptr TEST_print_rs(RSptr rs)
{
  fsm_fprintf(stderr,"\n-------- BEGIN REPLACEMENT STRUCTURE --------\n");

  if(rs)
    {
      fsm_fprintf(stderr,"ARROW :            %d\n", RS_arrow(rs));
      fsm_fprintf(stderr,"CONTEXT DIRECTION: %d\n", RS_ctxdir(rs));
      TEST_print_nv( RS_nv(rs) );
    }
  else
    fsm_fprintf(stderr,"\n     NULL - POINTER\n\n");

  fsm_fprintf(stderr,"--------- END REPLACEMENT STRUCTURE ---------\n\n");
 
  return(rs);
}

#endif /* DEBUG */

