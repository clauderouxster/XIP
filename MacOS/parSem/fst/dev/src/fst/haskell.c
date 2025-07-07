/* $Id: haskell.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1997 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "fst/struct.h"
#include "fst/bin-out.h"
#include "fst/net-io.h"
#include "fst/rename.h"
#include "fst/haskell.h"
#include "fst/fsm_io.h"
#include "fst/pretty-p.h"

/******************************************************
 **
 **  HASKELL.C
 **
 **  Lauri Karttunen, XRCE, November 1997,
 **
 **  Input/output routines for representing finite-state
 **  networks as Haskell structures.
 **  
 ******************************************************/

#define state_name(X)		(unsigned long int) STATE_client_cell((X))

extern FILE *haskell_in;
extern int haskell_parse(void);

static NVptr HASKELL_NET_VECTOR = NULL;

static int hs_num_width(unsigned long num)
{
  if (num == 0)
    return(1);
  else
    return(1 + (int) log10(num));
}

static int hs_symbol_width(id_type id)
{
  /* Format: sb "<symbol name>" */

  if (id == EPSILON || id == OTHER)
    return(3);
  else
    return(fat_strlen(id_to_atom_name(id)) + 5);
}

static int hs_label_width(id_type id)
{
  if (tuple_id_p(id))
    return (hs_symbol_width(upper_id(id)) + hs_symbol_width(lower_id(id)) + 4);
  else
    return(hs_symbol_width(id));
}

static int hs_arc_width(unsigned long state, id_type id, unsigned long dest)
{
  return(hs_num_width(state) + hs_label_width(id) + 
	 hs_num_width(dest) + 6);
}

static int final_state_p(STATEptr state)
{
  return(STATE_final(state));
}

static int every_state_p(STATEptr state)
{
  return(TRUE);
}

static void print_spaces(FILE *stream, int count)
{
  int i;

  for (i = 0; i < count; i++)
    fsm_fprintf(stream, " ");
}

static void print_if_specified(FILE *stream, char *str)
{
  if (str)
    fsm_fprintf(stream, "%s", str);
}

static void comma_or_nothing(int *first_p, int *total, FILE *stream)
{
  if (*first_p)
    *first_p = FALSE;
  else
    {
      fsm_fprintf(stream, ",");
      (*total)++;
    }
}

static void space_or_newline(FILE *stream, int *total, int next,
			     int lm, int rm)
{
  /* Outputs either a space or a newline or nothing depending
     on whether the width of NEXT added to TOTAL exceeds RM (= right margin)
     Increments the value of TOTAL accordingly. After a newline
     spaces are printed up to LM (= left margin). */

  if (*total <= lm)
    (*total) += next;                /* At the left margin, print flush */
  else if ((*total + next) < rm)
    {
      fsm_fprintf(stream, " ");      /* Print a space */
      (*total) += next + 1;
    }
  else
    {
      fsm_fprintf(stream, "\n");     /* Print a newline and space to LM */
      print_spaces(stream, lm);      /* Space to LM */
      (*total) = lm + next;
    }
}

static void hs_print_comment(char *comment, FILE *stream, int *total,
			     int lm, int rm)
{
  int next;

  if (comment != NULL)
    {
      next = 3 + strlen(comment);
      space_or_newline(stream, total, next, lm, rm);
      fsm_fprintf(stream, "-- %s", comment);    
    }
}

static void hs_print_states(NETptr net, FILE *stream, int filter_fn(),
			    int lm, int rm, char *separator, char *comment)
{
  STATEptr state;
  int next_width, total_width, new_lm, first_p = TRUE;
  unsigned long state_num;
  
  print_spaces(stream, lm);

  fsm_fprintf(stream, "[");
  new_lm = lm + 1;
  total_width = lm;

  for (state = NET_states(net); state; state = next_state(state))
    if (filter_fn(state))
      {
	state_num = state_name(state);
	comma_or_nothing(&first_p, &total_width, stream);
	next_width = hs_num_width(state_num);
	space_or_newline(stream, &total_width, next_width, new_lm, rm);
	fsm_fprintf(stream, "%ld", state_num);
      }
    
  fsm_fprintf(stream, "]");
  total_width++;

  print_if_specified(stream, separator);

  hs_print_comment(comment, stream, &total_width, new_lm, rm);
}

static void hs_print_symbol(id_type id, FILE *stream)
{
  if (id == EPSILON)
    fsm_fprintf(stream, "eps");
  else if (id == OTHER)
    fsm_fprintf(stream, "qmk");
  else
    {
      fsm_fprintf(stream, "sb \"");
      FST_fprint_fat_string(stream, id_to_atom_name(id));
      fsm_fprintf(stream, "\"");
    }
}

static void hs_print_label(id_type id, FILE *stream)
{
  if (tuple_id_p(id))
    {
      fsm_fprintf(stream, "(");
      hs_print_symbol(upper_id(id), stream);
      fsm_fprintf(stream, ", ");
      hs_print_symbol(lower_id(id), stream);
      fsm_fprintf(stream, ")");
    }
  else
    hs_print_symbol(id, stream);
}

static void hs_print_arc(FILE *stream, unsigned long state, id_type lab,
			 unsigned long dest)
{
  fsm_fprintf(stream, "(%ld, ", state);
  hs_print_label(lab, stream);
  fsm_fprintf(stream, ", %ld)", dest);
}

static void hs_print_arcs(NETptr net, FILE *stream, int lm, int rm,
			 char *separator,  char *comment)
{
  STATEptr state;
  ARCptr arc;
  int next_width, total_width, new_lm, first_p = TRUE;
  unsigned long state_num, dest_num;
  id_type lab_id;
  
  print_spaces(stream, lm);

  fsm_fprintf(stream, "[");
  new_lm = lm + 1;
  total_width = new_lm;

  for (state = NET_states(net); state; state = next_state(state))
    {
      state_num = state_name(state);
      for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	{
	  lab_id = ARC_label(arc);
	  dest_num = state_name(ARC_destination(arc));

	  comma_or_nothing(&first_p, &total_width, stream);
	  next_width = hs_arc_width(state_num, lab_id, dest_num);
	  space_or_newline(stream, &total_width, next_width, new_lm, rm);
	  hs_print_arc(stream, state_num, lab_id, dest_num);
	}
    }
 
  fsm_fprintf(stream, "]");

  print_if_specified(stream, separator);

  hs_print_comment(comment, stream, &total_width, new_lm, rm);
}


static void  hs_print_start_state(NETptr net, FILE *stream, int lm, int rm,
				  char *separator, char *comment)
{
  int total_width = lm ;

  unsigned long state_num = state_name(NET_start_state(net));

  print_spaces(stream, lm);
  fsm_fprintf(stream, "%ld", state_num);

  total_width += hs_num_width(state_num);

  print_if_specified(stream, separator);

  hs_print_comment(comment, stream, &total_width, lm, rm);
}



static void hs_print_sigma(NETptr net, FILE *stream, int lm, int rm,
			   char *separator, char *comment)
{
  ALPHABETptr sig = label_to_binary(NET_sigma(net));
  int i, next_width, total_width, new_lm;
  int len = ALPH_len(sig), first_p = TRUE;

  total_width = lm;

  fsm_fprintf(stream, "[");
  new_lm = lm + 1;
  total_width = new_lm;

  for(i = 0; i < len; i++)
    if (ALPH_item(sig, i))
      {
	comma_or_nothing(&first_p, &total_width, stream);
	next_width = hs_label_width(i);
	space_or_newline(stream, &total_width, next_width, new_lm, rm);
	hs_print_label(i, stream);
      }
  fsm_fprintf(stream, "]");

  print_if_specified(stream, separator);

  hs_print_comment(comment, stream, &total_width, new_lm, rm);
}

static void hs_print_net(NETptr net, FILE *stream, int lm, int rm,
			 int comment_p)
{
  int new_lm;

  print_spaces(stream, lm);
  fsm_fprintf(stream, "(");
  new_lm = lm + 1;

  hs_print_sigma(net, stream, new_lm, rm, ",", (comment_p ? "sigma" : NULL));
  fsm_fprintf(stream, "\n");

  hs_print_states(net, stream, every_state_p, new_lm, rm, ",",
		  (comment_p ? "states" : NULL));
  fsm_fprintf(stream, "\n");

  hs_print_states(net, stream, final_state_p, new_lm, rm, ",",
		  (comment_p ? "final states" : NULL));
  fsm_fprintf(stream, "\n");

  hs_print_start_state(net, stream, new_lm, rm, ",",
		       (comment_p ? "start state" : NULL));
  fsm_fprintf(stream, "\n");

  hs_print_arcs(net, stream, new_lm, rm, NULL, (comment_p ? "arcs" : NULL));

  fsm_fprintf(stream, "\n");
  print_spaces(stream, lm);
  fsm_fprintf(stream, ")");
}

static void hs_print_header(NETptr net, FILE *stream)
{
  OBJECTptr name_val = get_value(net, NETWORKNAME_ATTR);

  if (NET_arc_label_arity(net) == 1)
    fsm_fprintf(stream, "-- Simple automaton");
  else
    fsm_fprintf(stream, "-- Transducer");

  if (name_val && OBJ_type(name_val) == String)
    {
      fsm_fprintf(stream, " \"");
      FST_fprint_fat_string(stream, OBJ_str(name_val));
      fsm_fprintf(stream, "\"");
    }
  fsm_fprintf(stream, ": ");
  print_net_size(net, stream);
}

static void hs_print_trailer(NETptr net, FILE *stream)
{
  fsm_fprintf(stream, " -- network");
}

void network_to_haskell_file(NETptr net, FILE *stream, int sort_states_and_arcs)
{
  /* Converts a network to a Haskell structure, defined as

     type State = Int
     type Arc a = (Int,Symbol a,Int)  -- a and b are symbol types
     data Symbol a = sb a | eps | qmk
     type Sigma a = [a]

     type Autom a = (Sigma a,[Int],[Int],Int,[Arrow a]) 
     -- (Sigma, states, finstates, initial, arrows)
     */
  
  hs_print_header(net, stream);

  /* Sort arc and state lists into canonical order */

  reorder_arcs(net, default_arc_comp_fn);
  top_sort_net(net);
  reorder_states(net, state_num_less_p);
  
  hs_print_net(net, stream, 1, 72, TRUE);
  hs_print_trailer(net, stream);

  fsm_fprintf(stream, "\n");
}


void networks_to_haskell_file(NVptr nv, FILE *stream, int sort_states_and_arcs)
{
  /* Writes the networks in the network array NV as Haskell clauses. */
  
  int i;
  
  fsm_fprintf(stream, "[");

  for (i = 0; i < NV_len(nv); i++)
    {
      if (i > 0)
       fsm_fprintf(stream, ",\n");
      
      network_to_haskell_file(NV_net(nv, i), stream, sort_states_and_arcs);
    }

  fsm_fprintf(stream, "] -- list of finite-state networks\n");
}

/****************************************************** 
 **
 **  Functions for constructing networks from
 **   Haskell clauses.
 **  
 **
******************************************************/

void hs_net(void)
{
  NETptr net;

  net = null_fsm();
  nv_push(net, HASKELL_NET_VECTOR);
}

static char *net_name(NETptr net)
{
 return((char *) NET_client_cell(net));
}

STATEptr hs_state(int state_id)
{
  NETptr net = NV_net(HASKELL_NET_VECTOR, 0);
  STATEptr state = NET_states(net);

  /* Find the state on the net's state list or push it there. */

  while (state)
    {
      if ((int)state_name(state) == state_id)
        break;

      state = next_state(state);
    }

  if (!state)
    {
      state = make_state(NULL, net);
      STATE_client_cell(state) = (void *) state_id;
    }
   
  return(state);
}

void hs_start(STATEptr state)
{
  NETptr net = NV_net(HASKELL_NET_VECTOR, 0);  
  NET_start_state(net) = state;
}

void hs_arc(STATEptr orig, id_type label_id, STATEptr dest)
{
  NETptr net = NV_net(HASKELL_NET_VECTOR, 0);

  label_to_sigma(NET_sigma(net), label_id);
  sigma_add_to(NET_labels(net), label_id);
#ifdef FULL_FSM
  add_new_arc(net, orig, label_id, dest, NULL);
#else
  add_new_arc(net, orig, label_id, dest);
#endif
}

void hs_final(STATEptr state)
{
  STATE_final(state) = TRUE;
}

void hs_sym(int symbol_id)
{
  NETptr net = NV_net(HASKELL_NET_VECTOR, 0);
  sigma_add_to(NET_sigma(net), symbol_id);
}

NVptr networks_from_haskell_file(FILE *stream)
{
  /* Converts a file of Haskell clauses to one or more networks. See
     the documentation of 'network_to_haskell_file()' for a description of
     the expected format.

     A file may contain any number of networks.

     */

 int i;
 NETptr net;

 HASKELL_NET_VECTOR = make_nv(0);

 haskell_in = stream;

 haskell_parse();

 for (i = 0; i < NV_len(HASKELL_NET_VECTOR); i++)
 {
   net = NV_net(HASKELL_NET_VECTOR, i);

   /* Free temporary name. */
   free((void *) net_name(net));

   /* Put label alphabet in standard form. */
   binary_to_label(NET_labels(net));

   /* We are not sure about any other properties of the net
      except that it is epsilonfree unless marked otherwise
      by add_new_arc().
      */

   NET_loop_free(net) = FALSE;
   NET_pruned(net) = FALSE;
   NET_deterministic(net) = FALSE;
   NET_minimized(net) = FALSE;
 }
 
  /* Reverse the net vector so that the networks are in the
     order they were created from the file.
     */

 return(nv_reverse(HASKELL_NET_VECTOR));
}
