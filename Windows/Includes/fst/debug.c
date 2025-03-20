/* $Id: debug.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/**************************************
 *
 *  DEBUG.C
 *
 *  Andre Kempe, XRCE Grenoble
 *  August 1999
 *  Copyright (c) 1999 by the Xerox Corporation.  All rights reserved
 *
 *
 *  COLLECTION OF TEST FUNCTIONS
 *
 *
 ***************************************/

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

#ifdef DEBUG
/*-----------------------------------------------------------*/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "fst/types.h"
#include "fst/struct.h"
#include "fst/apply.h"
#include "fst/fsm_io.h"
#include "fst/net-io.h"
#include "fst/copy-fsm.h"
#include "fst/pretty-p.h"
#include "fst/bin-out.h"
#include "fst/prompt.h"
#include "fst/debug.h"
#include "fst/cfsm.h"
#include "fst/utf8_fatstr_util.h"

/*-----------------------------------------------------------*/

void TEST_print_ptr_vec (void **vec, long len)
{
  long i;

  for (i=0 ; i< len; i++, vec++)
    fsm_fprintf (stderr, " %li:%p", i, *vec);

  fsm_fprintf (stderr, "\n");
}

void TEST_print_long_vec (long *vec, long len)
{
  long i;

  for (i=0 ; i< len; i++, vec++)
    fsm_fprintf (stderr, " %li:%li", i, *vec);

  fsm_fprintf (stderr, "\n");
}

void TEST_print_idtype_vec (id_type *vec, long len)
{
  long i;

  for (i=0 ; i< len; i++, vec++)
    fsm_fprintf (stderr, " %li:%hu", i, *vec);

  fsm_fprintf (stderr, "\n");
}

NETptr TEST_print_net (NETptr net)
{
  NETptr xnet;

  if(net)
    {
      xnet = copy_fsm (net);
      fsm_fprintf (stderr, "Sigma: ");
      print_sigma_s (xnet, 60, 7, stderr);
      print_net (xnet, stderr);
      free_network (xnet);
    }
  else fsm_fprintf(stderr,"NULL_POINTER\n");

  fsm_fprintf(stderr,"--------------------------\n");
  return(net);
}
 
NVptr TEST_print_nv(NVptr nv)
{
  int i;
 
  fsm_fprintf (stderr,"\n----------- BEGIN NETVEKTOR (%d) -----------\n",
	       nv->len);
  for (i=0;  i< NV_len(nv);  i++)
    TEST_print_net(NV_net(nv,i));

  fsm_fprintf(stderr,"------------- END NETVEKTOR -------------\n\n");
  return(nv);
}

void TEST_print_storage ()
{
  print_storage_info(stdout);
}


void TEST_num_nets()
{
  long in_use = 0, total = 0, size = 0;

  heap_info (NET_HEAP, &in_use, &total, &size);

  fsm_fprintf(stderr,"Currently in use:  %li nets\n", in_use);
}

static void TEST_get_label_name(LABELptr label, char *lab_name)
{
  FAT_STR fstring = LABEL_atom(label);
     
  FST_fat_to_thin (fstring, lab_name);
}

static int TEST_get_label (id_type id, char *lab_name)
{
  TUPLEptr tuple;  LABELptr label;
  int16 arity;
  id_type  upper_id, lower_id;
  uint16 id_count = LIM_id_count(LABEL_MAP);
  char *pt;

  if (id>=id_count)
    { strcpy(lab_name, "WRONG_ID"); return(FALSE);}

  label = id_to_label(id);
  arity = LABEL_arity(label);

  switch(arity)
    {
    case 1:
      TEST_get_label_name(label, lab_name);
      break;
    case 2:
      tuple = LABEL_tuple(label);
      upper_id = TUPLE_labels(tuple)[UPPER];
      lower_id = TUPLE_labels(tuple)[LOWER];

      TEST_get_label(upper_id, lab_name);

      pt = &( lab_name[ strlen(lab_name) ]); *pt++=':';
      TEST_get_label(lower_id, pt);
      break;
    default:
      strcpy(lab_name, "WRONG_ARITY");
      return(FALSE);
    }

  return(TRUE);
}

void TEST_xprint_net (NETptr net, int smax, int amax)
{
  /* Can print corrupted nets without crashing. */

  STATEptr state;  ARCptr arc;  int sidx, aidx;  char label[100];

  fsm_fprintf(stderr,"NETWORK %p - STATES %li - ARCS %li\n",
	  net, NET_num_states(net), NET_num_arcs(net));

  for (sidx=0, state = NET_states(net); (sidx<smax)&&(state);
       sidx++, state = next_state(state))
    {
      fsm_fprintf(stderr,"%d ", sidx);
      if (state == NET_start_state(net))
	fsm_fprintf(stderr,"S");
      if (STATE_final(state))
	fsm_fprintf(stderr,"F");
      fsm_fprintf(stderr,":\t%p : ", state);

      for (aidx=0, arc = STATE_arc_set(state); (aidx<amax)&&(arc);
	   aidx++, arc = next_arc(arc))
	{
	  TEST_get_label (ARC_label(arc), label);
	  fsm_fprintf (stderr," %s", label);

	  if (state == ARC_destination (arc))
	    fsm_fprintf (stderr," --O  LOOP");
	  else
	    fsm_fprintf (stderr," -> %p", ARC_destination (arc));

	  if (next_arc(arc))
	    fsm_fprintf(stderr,",");
	  else
	    fsm_fprintf(stderr,".");
	}

      if (arc)
	fsm_fprintf(stderr," >>>");
      fsm_fprintf(stderr,"\n");
    }

  if (state)
    fsm_fprintf(stderr,"  >>> MORE STATES >>>\n");
  fsm_fprintf(stderr,"-------------------------------\n");
}

void TEST_save_net (NETptr net, char *filename)
{
  write_net(net, filename);
}

void TEST_print_label (id_type id)
{
  char label[100];

  if (TEST_get_label (id, label))
    fsm_fprintf (stderr, "ID %hu  LABEL <%s>\n", id, label);
  else
    fsm_fprintf (stderr, "  *** ERROR ***  %s\n", label);
}

void TEST_print_alph (ALPHABETptr alph)
{
  print_alphabet_s (alph, 64, 2, (ALPH_type (alph) == BINARY_VECTOR), stderr);
}

void TEST_print_arc_list (ARCptr arc)
{
  char label[100];  int i;

  for (i=0;  arc;  i++, arc = next_arc (arc))
    {
      TEST_get_label (ARC_label (arc), label);
      fprintf (stderr, "[%d]\t%hi\t%s\t%#lx\n",
	       i, ARC_label (arc),  label, (long int) ARC_destination (arc));
    }
}

void TEST_print_state (STATEptr state)
{
  ARCptr arc;  char label[100], dest [20];  int i;

  fprintf (stderr, "State: %sfinal\t%#lx\n",
	   (STATE_final (state) ? "" : "not "), (long int) state);
  fprintf (stderr, "Arcs:  %d\n", num_arcs (state));

  for (i=0, arc = STATE_arc_set (state);  arc;  i++, arc = next_arc (arc))
    {
      sprintf (dest, "%#lx", (long int) ARC_destination (arc));

      TEST_get_label (ARC_label (arc), label);
      fprintf (stderr, "[%d]\t%hi\t%s\t%s\n",
	       i, ARC_label (arc), label,
	       ((ARC_destination (arc) == state) ? "looping" : dest));
    }
}

void TEST_print_net_info (NETptr net)
{
  STATEptr state;  ARCptr arc;  long num_states, num_arcs, num_final;

  num_states = num_arcs = num_final = 0;

  for (state = NET_states (net);  state;  state = next_state (state))
    {
      num_states ++;
      num_final += (STATE_final (state)) ? 1 : 0;

      for (arc = STATE_arc_set (state);  arc;  arc = next_arc (arc))
	num_arcs ++;
    }

  fprintf (stderr, "Counted: %li states, %li final states, %li arcs\n",
	   num_states, num_final, num_arcs);
  fflush (stderr);
}

void TEST_print_io_seq(IO_SEQptr io_seq)
{
  if (io_seq)
    {
      int i, j, pos;
      id_type id;
      ALPHABETptr prev_sig;
      LAB_VECTORptr io_vect = IO_SEQ_io_vector(io_seq);
      LAB_VECTORptr ot_vect = IO_SEQ_other_than_vector(io_seq);
      VECTORptr ot_index = IO_SEQ_other_than_index(io_seq);
      VECTORptr prev_sigma_vector = IO_SEQ_prev_sigma_vector(io_seq);
      VECTORptr sig_vect;

       fprintf(stderr, "io_vector:\n");
      for (i = 0; i < LAB_VECTOR_pos(io_vect); i++)
	{
	  lab_vector_element_at(&id, i, io_vect);
	  fprintf(stderr, "\t%d", id);
	}

      fprintf(stderr, "\n\nother_than_vector:\n");
      for (i = 0; i < LAB_VECTOR_pos(ot_vect); i++)
	{
	  lab_vector_element_at(&id, i, ot_vect);
	  fprintf(stderr, "\t%d", id);
	}

      fprintf(stderr, "\n\nother_than_index:\n");
      for (i = 0; i < VECTOR_pos(ot_index); i++)
	{
	  vector_element_at((void*)&pos, i, ot_index);
	  fprintf(stderr, "\t%d", (int) pos);
	}

      fprintf(stderr, "\n\n");
      for (i = 0; i < VECTOR_pos(ot_index); i++)
	{
	  vector_element_at((void *)&pos, i, ot_index);
	  vector_element_at((void **)&sig_vect, pos, prev_sigma_vector);

	  if (sig_vect)
	    {
	      fprintf(stderr, "previous sigmas at %d:\n", pos);
	      
	      for (j = 0; j < VECTOR_pos(sig_vect); j++)
		{
		  vector_element_at((void *) &prev_sig, j, sig_vect);
		  TEST_print_alph(prev_sig);
		}
	    }
	  else
	    fprintf(stderr, "No prev sigma at %d\n", pos);
	}
    }
}

/*-----------------------------------------------------------*/

/* --- Begin Debug help TG ----------------------------------------------------*/

#define DEBUG_MAX_PRINT_STATES 50

void debug_print_arc(ARCptr narc, int arc_number)
{ 
  if (!narc)  
    {
      fprintf (stderr,"\t arc %d=%d=0x%x.", arc_number, (int) narc, (int) narc);  
      fflush(stderr); 
      return;
    }

  fprintf (stderr,"\t arc %d=%d=0x%x. Flag=%d, Vm=%d, label=%d=", arc_number, (int) narc, (int) narc,
	   ARC_userflag2(narc), ARC_visit_mark(narc), ARC_label(narc));
  print_label(ARC_label(narc), stderr, DO_ESCAPE);
  fprintf (stderr,"  destination=%d=0x%x\n",  (int) ARC_destination(narc), (int) ARC_destination(narc)); 
  fflush(stderr); 
}

void debug_print_state(STATEptr nstate, int state_number, int with_arcs)
{
  ARCptr narc;
  int arc_i;
  if (! nstate)  
    {
      fprintf (stderr, " %d  state =%d=0x%x.", state_number, (int) nstate, (int) nstate); 
      fflush(stderr); return;
    }

  if (STATE_final(nstate))
    fprintf (stderr, " %d Fstate", state_number);
  else
    fprintf (stderr, " %d  state", state_number);
  fprintf (stderr," =%d=0x%x. Flag=%d, Vm=%d, Client_cell=%d=0x%x\n",
	   (int) nstate, (int) nstate,
	   STATE_userflag2(nstate),
	   STATE_visit_mark(nstate),
	   (int) STATE_client_cell(nstate), (int) STATE_client_cell(nstate));
  if (with_arcs)
    {
      if (! STATE_vector_p(nstate))
	  {
	    for (narc = STATE_arc_set(nstate), arc_i=1 ; narc ; narc = next_arc(narc), arc_i++)
	      {
		debug_print_arc(narc, arc_i);
	      }
	  }
      else
	fprintf (stderr,"** State vectorized. No printing implemented for a vectorized state.\n");
    }
  fflush(stderr);
}

void debug_print_net(NETptr net, int with_arcs)
{
  int i;
  STATEptr nstate;

  if (!net)
    { 
      fprintf (stderr, "\n\
SEQ Debug: No net to print.\n");
      return;
    }

  if  (NET_num_states(net) > DEBUG_MAX_PRINT_STATES)
    {
      fprintf (stderr,"Number of states:  %ld",  NET_num_states(net));
      if (! yes_or_no(" Debug print net: print all these states ?",'n'))
	return;
    }

  fprintf (stderr,"\n*** Net Start state==%d=0x%x.",  (int) NET_start_state(net),
	     (int) NET_start_state(net));
 
  if (NET_start_state(net))
    fprintf (stderr," Flag=%d.\n", STATE_userflag2( NET_start_state(net)));
  else
    fprintf (stderr,"\n");

  for (nstate = NET_states(net), i=1 ; nstate ; nstate = next_state(nstate), i++)
    debug_print_state(nstate, i, with_arcs);
  fflush(stderr);
}
/* --- End Debug help TG ----------------------------------------------------*/

#endif /* DEBUG */

#endif /* SLIM_FST */
