/* $Id: prolog.c,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1992 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "fst/struct.h"
#include "fst/bin-out.h"
#include "fst/net-io.h"
#include "fst/rename.h"
#include "fst/prolog.h"
#include "fst/fsm_io.h"
#include "fst/pretty-p.h"
#include "fst/utf8_fatstr_util.h"

/******************************************************
 **
 **  PROLOG.C
 **
 **  Lauri Karttunen, Xerox PARC, October 1992,
 **  Rank Xerox Research Centre, February 1995.
 **  June 26, 1998
 **  Input/output routines for representing finite-state
 **  networks as Prolog clauses.
 **  
 **
 ******************************************************/

#define state_name(X)		(long int) STATE_client_cell((X))

#include "fst/fst-types.h"

extern FILE *prolog_in;
extern int prolog_parse(void);
extern finfo *prolog_file_info;
int PROLOG_PARSE_ERROR = 0;

static NVptr PROLOG_NET_VECTOR = NULL;

/* static int special_p(FAT_STR name)
   Now calling print_fat_str_esc instead
{
  / Returns TRUE if name is the fat string counterpart of
     "0", "?" or "%"; otherwise FALSE.
     /

  return((CHARNUM(name) == '0' || CHARNUM(name) == '?' ||
          CHARNUM(name) == '%' || CHARNUM(name) == '"') &&
	 CHARSET(name) == '\0' &&
          CHARNUM(name+1) == '\0' && CHARSET(name+1) == '\0');
}
*/


static void print_quoted_label(LABELptr label, FILE *stream)
{

  fputc('\"', stream);
  
  if (LABEL_id(label) == EPSILON)
    fputc('0', stream);
  else if (LABEL_id(label) == OTHER)
    fputc('?', stream);
  else
  {
    /*    Now calling print_fat_str_esc instead
          if (special_p((LABEL_atom(label)))
	  fputc('%', stream);
	  */
    FST_fprint_fat_string_with_esc(stream, LABEL_atom(label));
  }
  fputc('\"', stream);
}

void network_to_prolog_file(NETptr net, FILE *stream, int sort_states_and_arcs)
{
  /* Converts a network to a set of Prolog clauses.
     
     The file consists of three types of clauses, beginning with

         network(<net_name>).

     If the network has a name (a value for NETWORKNAMEPROP attribute),
     then it is used as <net name>, converting it to lower case, if
     necessary, and replacing any spaces or tabs by an underscore.
     If the network does not have a name, an arbitrary unique name
     is created for it from the hex address of the network structure
     (in lower case) prefixed with "net_". For example,

         network(net_baec0).

     Every arc in the network is represented by a clause of the form
     
         arc(<net_name>, <origin_state>, <destination_state>, <label>).
     
     For example,

         arc(net_baec0, 0, 1, "a").

     states that an arc is labeled "a" goes from state 0 to state 1
     in network net_baec0.
     
     Label is a quoted string or a pair of quoted strings separated by a
     colon. The two special labels are: "0" (zero) and "?". Zero stands for
     the epsilon symbol, "?" represents OTHER. If some symbols
     are pairs, the network is a transducer. The ordinary numeral zero
     is written as "%0", the question mark as "%?".

     Origin state and destination states are integers. By convention
     the start state is state 0.

     Final states are enumerated by clauses of the form
     
       final(<net_name>, <state>).

     Symbols that belong to the sigma but do not appear explicitly
     are indicated by clauses of the form
       symbol(<net_name>, <sym>).

     */
  
  STATEptr state, dest;
  ARCptr arc;
  LABELptr label;
  char net_name[80]={0};
  int i;
  ALPHABETptr lab_sig, diff;
  OBJECTptr name_val = get_value(net, NETWORKNAME_ATTR);

  if (name_val && OBJ_type(name_val) == String &&
      fat_strlen(OBJ_str(name_val)) < 79)
    {
      char *str = net_name;

      FST_fat_to_thin(OBJ_str(name_val), net_name);

      /* Replace spaces and tabs, if any, with an underscore.
         Convert to lower case to make sure that the name is
         a Prolog constant.
         */
         
      while (*str)
        {
          if (*str == ' ' || *str == '\t')
           *str = '_';
          else if(isupper((int) *str))
           *str += 32;

          str++;
        }
    }
  else
    {
      /* Use the Hex address of the network as a pseudo name. */
#ifdef MAC_OS
      sprintf(net_name, "net_%lx", (size_t) &*net);
#else
	  sprintf(net_name, "net_%x", (size_t) &*net);
#endif
    }

  if (sort_states_and_arcs)
    {
      reorder_arcs(net, default_arc_comp_fn); /* 2000-4-20 TG */
      top_sort_net(net); /* 2000-4-20 TG */
    }
  else
    rename_states(net); 
  reorder_states(net, state_num_less_p);
  
  state = NET_start_state(net);
  fsm_fprintf(stream, "network(%s", net_name);
  fsm_fprintf(stream, ").\n") ;
  /* Indicate symbols that belong to the sigma but do not appear
     explicitly in the network.
     */

  lab_sig = label_sigma(net);
  diff = sigma_difference(sigma(net), lab_sig);
  for (i = 0; i < ALPH_len(diff); i++)
    if (ALPH_item(diff, i) == 1)
      {
	fsm_fprintf(stream, "symbol(%s, \"", net_name);
	print_label(i, stream, DO_ESCAPE);
	fsm_fprintf(stream, "\"");
	fsm_fprintf(stream, ").\n") ;
      }
  free_alph(lab_sig);
  free_alph(diff);

  for (state = NET_states(net); state != NULL; state = next_state(state))
    for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
      {
        dest = ARC_destination(arc);
	
        label = id_to_label(ARC_label(arc)); 
	
	fsm_fprintf(stream, "arc(%s, ", net_name);
	fsm_fprintf(stream, "%ld, %ld, ", state_name(state), state_name(dest));
	
	if (LABEL_arity(label) == 1)
	  print_quoted_label(label, stream);
	else
	  {
	    print_quoted_label(id_to_label(get_field(label, UPPER)), stream);
	    fputc(':', stream);
	    print_quoted_label(id_to_label(get_field(label, LOWER)), stream);
	  }
	fsm_fprintf(stream, ").\n") ;
      }
  for (state = NET_states(net); state != NULL; state = next_state(state))
    if (STATE_final(state))
	 {
	  	fsm_fprintf(stream, "final(%s, %ld", net_name, state_name(state));
	  	fsm_fprintf(stream, ").\n") ;
	 }
}


void networks_to_prolog_file(NVptr nv, FILE *stream, int sort_states_and_arcs)
{
  /* Writes the networks in the network array NV as Prolog clauses. */
  
  int i;
  
  for (i = 0; i < NV_len(nv); i++)
    {
#ifdef WORLD
      if (NET_num_states(NV_net(nv, i)) > WORLD_STATE_LIMIT)
	{
	  fsm_fprintf(stderr, "Sorry. This network is too large to be saved \
as a prolog file by xfst.\n");
	  fsm_fprintf(stderr, "You need to license a professional version of this program.\n");
	}
      else
#endif /* WORLD */
	{
	  if (i > 0)
	    fsm_fprintf(stream, "\n") ; /* 2002-3-11 TG */
	  network_to_prolog_file(NV_net(nv, i), stream, sort_states_and_arcs);
	}
    }
}

/****************************************************** 
 **
 **  Functions for constructing networks from
 **   Prolog clauses.
 **  
 **
******************************************************/

static char *net_name(NETptr net)
{
 return((char *) NET_client_cell(net));
}

static void name_net(NETptr net, char *name)
{
  /* Copies the name and puts it to the client cell of the net. */

  char *copy = (char *) malloc((size_t) (strlen(name) + 1) * sizeof(char));

  strcpy(copy, name);

  NET_client_cell(net) = copy;
}

static int pseudo_name_p(char *name)
{
  /* Returns TRUE just in case NAME of the form net_<hex number>,
     for example, net_eb4e0. A name of that type is indicates that
     the net does not have a real name; it is just the physical address
     of the structure used by network_to_prolog_file to provide a unique
     identifier for each network in the file.
     */

    if (*name++ == 'n' && *name++ == 'e' && *name++ == 't' &&
    	*name++ == '_' && *name)
      {
        while(*name)
          {
            if (!isxdigit((int) *name))
              return(FALSE);

            name++;
         }
        return(TRUE);
     }
   else
     return(FALSE);
} 

NETptr pl_net(char *name)
{
  /* Find a network with NAME on the stack or create it. */

  NETptr net = NULL;
  int i, max = NV_len(PROLOG_NET_VECTOR);
  
  for (i = 0; i < max; i++)
    if (strcmp(name, net_name(NV_net(PROLOG_NET_VECTOR, i))) == 0)
        net = NV_net(PROLOG_NET_VECTOR, i);

  if (!net)
    {
      net = null_fsm();
      /* name_net puts the name in the client cell for quick
         comparisons. name_network() creates a permanent name
         on the property list. Before giving a permanent name,
         make sure it is not a hex number (= a pseudo name used
         to give each net in the file a unique id.)
         */
      name_net(net, name);
      if (!pseudo_name_p(name))
        name_network(net, net_name(net));
      nv_push(net, PROLOG_NET_VECTOR);
    }
  return(net);
}

static STATEptr pl_state(NETptr net, int state_id)
{
  /* Find the state on the net's state list or push it there. */

  STATEptr state = NET_states(net);

  while (state)
    {
      if ((int) STATE_client_cell(state) == state_id)
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

void pl_start(NETptr net, int state_id)
{
  NET_start_state(net) = pl_state(net, state_id);
}

void pl_arc(NETptr net, int orig_id, int dest_id, id_type label_id)
{
  STATEptr orig=pl_state(net, orig_id);
  STATEptr dest=pl_state(net, dest_id);
  
  label_to_sigma(NET_sigma(net), label_id);
  sigma_add_to(NET_labels(net), label_id);
  /*
    Add an arc to ORIG_ID - if there was already one then ORIG_ID may
    become non-deterministic.  
    We want to do it fast so no checking of real non-determinism. 
    2002-2-7 TG */
  
  if (STATE_arc_set(orig))
    {
      STATE_deterministic(orig) = FALSE;
      NET_deterministic(net) = FALSE;
      NET_minimized(net) = FALSE;
    }

  if (label_id == EPSILON)
    {
      STATE_deterministic(orig) = FALSE;
      NET_deterministic(net) = FALSE;
      NET_epsilon_free(net) = FALSE;
      NET_minimized(net) = FALSE;
    }

  push_arc(make_arc(label_id, dest, net), orig);
}

void pl_final(NETptr net, int state_id)
{
  STATE_final(pl_state(net, state_id)) = TRUE;
/*
  fsm_fprintf(stdout, "In %s state %d is final.\n",
          net_name(net), state_id);
*/
}

void pl_symbol(NETptr net, int symbol_id)
{
  sigma_add_to(NET_sigma(net), symbol_id);
}

NVptr networks_from_prolog_file(FILE *stream)
{
  /* Converts a file of Prolog clauses to one or more networks. See
     the documentation of 'network_to_prolog_file()' for a description of
     the expected format.

     A file may contain any number of networks, so the first component
     of each clause is the name of the network it is about. If the
     name is of the default type, i.e. "net_" followed by a hex number,

        network(net_baec0).
        arc(net_baec0, 0, 1).
        final(net_baec0, 1).

     then the name is used only in the construction and the resulting
     network will be nameless. If the name is something special, as
     in
     
        network(foo).

     the resulting network will have the name "foo".

     State 0 is the default start state. Another state can be designated
     with a clause of the form

        start(<net_name>, <state>).

     The order of clauses in a file is unimportant. network(<net_name>
     statements are generally redundant because if a network is created
     if its name appears in any of the other three types of recognized
     clauses.
*/

 int i;
 NETptr net;

 PROLOG_NET_VECTOR = make_nv(0);

 prolog_in = stream;

 prolog_file_info->line_number = 0;

 PROLOG_PARSE_ERROR = prolog_parse();

 if (PROLOG_PARSE_ERROR)
   {
     fsm_fprintf(stderr, "*** Error on line %d",
		 prolog_file_info->line_number+1);

     if (prolog_file_info->file.name)
       fsm_fprintf(stderr, " in file '%s.\n", prolog_file_info->file.name);
     else
       fsm_fprintf(stderr, ".\n");
   }

 for (i = 0; i < NV_len(PROLOG_NET_VECTOR); i++)
 {
   net = NV_net(PROLOG_NET_VECTOR, i);

   set_net_arity(net);

   /* Free temporary name. */
   free((void *) net_name(net));

   /* Put label alphabet in standard form. */
   binary_to_label(NET_labels(net));

   /* We are not sure about any other properties of the net
      except that it is epsilonfree unless marked otherwise
      by add_new_arc() 0add_new_arbitrary_arc() as of 2002-2-6 TG.
      */

   NET_loop_free(net) = FALSE;
   NET_pruned(net) = FALSE;
   NET_deterministic(net) = FALSE;
   NET_minimized(net) = FALSE;
 }
 
  /* Reverse the net vector so that the networks are in the
     order they were created from the file.
     */

 return(nv_reverse(PROLOG_NET_VECTOR));
}
