/* $Id: net-io.c,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990-92 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
#include "fst/mark.h"
#include "fst/modify-base.h"
#include "fst/rename.h"
#include "fst/cmp-net.h"
#include "fst/pars-tbl.h"
#include "fst/pretty-p.h"
#include "fst/flags.h"
#include "fst/net-io.h"
#include "fst/fsm_io.h"
#include "fst/bin-out.h"
#include "fst/define.h"

#include "fst/virtual-base.h"
#include "fst/types.h"
#include "fst/utf8_fatstr_util.h"

#ifndef RAND_MAX
#define RAND_MAX 32767
#endif /* RAND_MAX */

#ifdef MAC_OS
  /* The prototype for Random() is in Quickdraw.h. It uses the random
  	seed of the application's GrafPort that is initialized in DoSetupToolbox()
  	in SIOUX.c with the current date and time. Defining rand() as Random() has
  	the effect that the sequence of random numbers is different for each run
  	of the application. */
  extern short Random();
#define rand Random
#endif /* MAC_OS */

/******************************************************
 **
 **  NET_IO.C
 **
 **	 Todd Yampol, Xerox PARC, January 1990
 **  Lauri Karttunen, Xerox PARC, October 1992
 **  Lauri Karttunen, RXRC, April 1994
 **
 ******************************************************/

static int RIGHT_MARGIN = 72;

/***********************************
 *
 *  Printing the contents of NET
 *
 ************************************/

int print_path_fn(char *buf, FILE *stream)
{
  fsm_fprintf(stream, "%s\n", buf);
  return(0);
}

int print_paths(NETptr net, int max_visits, int max_paths, int side,
		char *separator, unsigned long *word_count, FILE *stream,
		int escape_p, int obey_flags_p, int show_flags_p,
		int print_fn(char *buf, FILE *stream))
{
  /***********
   *
   * Print a possibly circular net allowing MAX_VISITS to a state.
   *
   ************/

  unsigned long count = 0;
  STACKptr state_stack = create_stack("States to print");
  STACKptr pos_stack = create_stack("Positions to remember");
  STACKptr arc_stack = create_stack("Arcs to print");
  STACKptr attr_stack = create_stack("Attributes to remember");
  STACKptr val_stack = create_stack("Values to remember");
  ALPHABETptr flag_reg = make_flag_register();
  STATEptr state = NET_start_state(net);
  ARCptr arc = virtual_arc_set(state);
  int pos = 0, circular_p = FALSE, continue_p;
  FLAG_DIACRptr flag= NULL;
  id_type id, attr = 0, old_val = 0;
  
  if (NET_visit_marks_dirty(net))
    prepare_visit_marks(net, 0);
  
  STATE_visit_mark(state)++;
  NET_visit_marks_dirty(net) = TRUE;
  
  /* Visit marks will be set back to 0
     if the printing is not interrupted */
  
  reset_out_buffer();

  while (state != NULL)
    {
      if (virtual_final_p(state) == TRUE && arc == virtual_arc_set(state))
	{
	  /* Reached a final state, print only once, not when
	     we are rebounding from recursion
	     */
	  
	  if (STATE_visit_mark(state) > 1)
	    /* Mark word-final looping site */
	    string_out("..");
	  
	  char_out('\0');
	  print_fn(output_buffer, stream);
	  if (++count == (unsigned long) max_paths)
	    {
	      STATE_visit_mark(state)--;
	      while ((state = (STATEptr) pop(state_stack)))
		STATE_visit_mark(state)--;
	      break;
	    }
	}
      
      if (arc != NULL)
	{
	  /* Set position in the output buffer and print arc label
	     unless it is a flag diacritic */

	  cur_pos = pos;
	  
	  if (STATE_visit_mark(state) > 1)
	    /* Mark word-internal looping site */
	    string_out("..");
	  
	  id = ARC_label(arc);

	  /* If ID is a tuple ID, it may contain a flag
	     diacritic on one side. */

/*
	  if (!(flag = flag_diacritic(upper_id(id))))
	    flag = flag_diacritic(lower_id(id));
*/	    
	  switch (side)
	    {
	    case UPPER:
	      id = upper_id(id);
	      flag = flag_diacritic(id);
	      break;
	    case LOWER:
	      id = lower_id(id);
	      flag = flag_diacritic(id);
	      break;
	    default:
	      /* Only consider "double-sided" flags. */
	      if (!(tuple_id_p(id)))
		flag = flag_diacritic(id);
	      break;
	    }

	  if (flag && obey_flags_p)
	    {
	      attr = FLAG_DIACR_attribute(flag);
	      old_val =  flag_value(flag_reg, attr);
	      if (flag_action(flag_reg, flag))
		{
		  /* If the label is a tuple print the label on
		     the opposite side */

		  if (show_flags_p)
		    {
		      if (separator && pos > 0)
			string_out(separator);

		      label_out(id, escape_p);
		    }
		  continue_p = TRUE;
		}
	      else
		continue_p = FALSE;
	    }
	  else 
	    {
	      if (!(id == EPSILON || id == ALTCHAIN_SYMBOL))
		{

		  if (separator && pos > 0)
		    string_out(separator);

		    label_out(id, escape_p);
		}
	      continue_p = TRUE;
	      attr = 0;
	      old_val = 0;
	    }

	  /* Recurse on destination unless we have been here too often or
	     prohibited by a flag_diacritic constraint. */
	  
	  if ((int)STATE_visit_mark(ARC_destination(arc)) < max_visits &&
	      continue_p)
	    {
	      push((void *) state, state_stack);
	      push((void *) pos, pos_stack);
	      push((void *) next_arc(arc), arc_stack);
	      push((void *) (int) attr, attr_stack);
	      push((void *) (int) old_val, val_stack);

	      state = ARC_destination(arc);
	      STATE_visit_mark(state)++;
	      pos = cur_pos;
	      arc = virtual_arc_set(state);
	    }
	  else
	    {
	      /* Don't recurse, just remember the loop */
	      arc = next_arc(arc);
	      circular_p = TRUE;
	      cur_pos = 0;
	    }
	}
      else
	{
	  STATE_visit_mark(state)--;
	  state = (STATEptr) pop(state_stack);
	  pos = (int) pop(pos_stack);
	  arc = (ARCptr) pop(arc_stack);
	  flag_value(flag_reg, (int) pop(attr_stack))
	    = (int) pop(val_stack);
	}
    }
  
  NET_visit_marks_dirty(net) = FALSE;
  free_stack(state_stack);
  free_stack(pos_stack);
  free_stack(arc_stack); 
  free_stack(attr_stack); 
  free_stack(val_stack); 
  free_alph(flag_reg);
  reset_out_buffer();
  fresh_line_s(72, 0, stream);

  *word_count = count;
  return(circular_p);
}


void print_nth_word(NETptr net, int n, int max_visits, int rm, int indent, 
                    int space_p, FILE *stream)
{

    /***********
     *
     * Print the NTH path from a possibly circular net allowing MAX_VISITS
     * to a state.
     *
     ************/
    
    int word_count = 0;
    STACKptr state_stack = create_stack("States to print");
    STACKptr pos_stack = create_stack("Positions to remember");
    STACKptr arc_stack = create_stack("Arcs to print");
    STATEptr state = NET_start_state(net);
    ARCptr arc = STATE_arc_set(state);
    int pos = 0 ;
  
    prepare_visit_marks(net, 0);
  
    STATE_visit_mark(state)++;
    NET_visit_marks_dirty(net) = TRUE;
  
    /* Visit marks will be set back to 0
       if the printing is not interrupted */
  
    while (state != NULL)
    {
        if (STATE_final(state) == TRUE && arc == STATE_arc_set(state))
        {
            if (word_count == n)
            {
                /* Reached a final state, print only once, not when
                   we are rebounding from recursion
                   */
	  
                if (STATE_visit_mark(state) > 1)
                    /* Mark word-final looping site */
                    string_out("..");
	  
                if (cur_pos == 0)
                    /* Delimiter for the empty word */
                    string_out(" ");
	  
                buffer_out_s(rm, 0, stream);
            }
            word_count++;
        }
      
        if (arc != NULL && word_count < n)
	{
            /* Set position in the output buffer and print arc label */
            cur_pos = pos;
	  
            if (STATE_visit_mark(state) > 1)
                /* Mark word-internal looping site */
                string_out("..");
	  
            if (!(ARC_label(arc) == EPSILON ||
                  ARC_label(arc) == ALTCHAIN_SYMBOL))
                {
                    if (space_p && cur_pos > 0)
                        string_out(" ");

                    label_out(ARC_label(arc), DO_ESCAPE);
                }
	  
            /* Recurse on destination unless we have been here too often */
	  
            if ((int)STATE_visit_mark(ARC_destination(arc)) < max_visits)
	    {
                push((void *) state, state_stack);
                push((void *) pos, pos_stack);
                push((void *) next_arc(arc), arc_stack);
	      
                state = ARC_destination(arc);
                STATE_visit_mark(state)++;
                pos = cur_pos;
                arc = STATE_arc_set(state);
	    }
            else
	    {
                /* Don't recurse, just remember the loop */
                arc = next_arc(arc);
                cur_pos = 0;
	    }
	}
        else
	{
            STATE_visit_mark(state)--;
            state = (STATEptr) pop(state_stack);
            pos = (int) pop(pos_stack);
            arc = (ARCptr) pop(arc_stack);
	}
    }
  
    NET_visit_marks_dirty(net) = FALSE;
    free_stack(state_stack);
    free_stack(pos_stack);
    free_stack(arc_stack);
    fresh_line_s(72, 0, stream);
}

void print_words(NETptr net, int rm, int indent, int max_visits, int count_p,
		   int obey_flags_p, FILE *stream)
{
  /***********
   *
   * Print a possibly circular net allowing MAX_VISITS to a state.
   * If OBEY_FLAGS_P is nonzero, the flag diacritics are treate as
   * constrained epsilon transitions. If OBEY_FLAGS_P is zero, arcs
   * labelled with diacritics are treated like ordinary arcs.
   *
   ************/
  long word_count = 0;
  STACKptr state_stack = create_stack("States to print");
  STACKptr pos_stack = create_stack("Positions to remember");
  STACKptr arc_stack = create_stack("Arcs to print");
  STATEptr state = NET_start_state(net);
  ALPHABETptr flag_reg = make_flag_register();
  ARCptr arc = STATE_arc_set(state);
  int pos = 0, circular_p = FALSE;
  
  prepare_visit_marks(net, 0);
  
  STATE_visit_mark(state)++;
  NET_visit_marks_dirty(net) = TRUE;
  
  /* Visit marks will be set back to 0
     if the printing is not interrupted */
  
  while (state != NULL)
    {
      if (STATE_final(state) == TRUE && arc == STATE_arc_set(state))
	{
	  /* Reached a final state, print only once, not when
	     we are rebounding from recursion
	     */
	  
	  if (STATE_visit_mark(state) > 1)
	    /* Mark word-final looping site */
	    string_out("..");
	  
	  if (cur_pos == 0)
	    /* Delimiter for the empty word */
	    string_out(" ");
	  
	  buffer_out_s(rm, 0, stream);
	  word_count++;
	}
      
      if (arc != NULL)
	{
	  /* Set position in the output buffer and print arc label */
	  cur_pos = pos;
	  
	  if (STATE_visit_mark(state) > 1)
	    /* Mark word-internal looping site */
	    string_out("..");
	  
	  if (!(ARC_label(arc) == EPSILON ||
		ARC_label(arc) == ALTCHAIN_SYMBOL))
	    label_out(ARC_label(arc), DONT_ESCAPE);
	  
	  /* Recurse on destination unless we have been here too often */
	  
	  if ((int)STATE_visit_mark(ARC_destination(arc)) < max_visits)
	    {
	      push((void *) state, state_stack);
	      push((void *) pos, pos_stack);
	      push((void *) next_arc(arc), arc_stack);
	      
	      state = ARC_destination(arc);
	      STATE_visit_mark(state)++;
	      pos = cur_pos;
	      arc = STATE_arc_set(state);
	    }
	  else
	    {
	      /* Don't recurse, just remember the loop */
	      arc = next_arc(arc);
	      circular_p = TRUE;
	      cur_pos = 0;
	    }
	}
      else
	{
	  STATE_visit_mark(state)--;
	  state = (STATEptr) pop(state_stack);
	  pos = (int) pop(pos_stack);
	  arc = (ARCptr) pop(arc_stack);
	}
    }
  
  NET_visit_marks_dirty(net) = FALSE;
  free_stack(state_stack);
  free_stack(pos_stack);
  free_stack(arc_stack);
  free_alph(flag_reg);
  fresh_line_s(72, 0, stream);
  
  if (count_p)
    {
	  fsm_fprintf(stream, "Word count: %ld", word_count);
	  if (circular_p == TRUE)
		fsm_fprintf(stream, "  (actually, infinite)\n");
	  else
		fsm_putc('\n', stream);
    }
}



#ifndef LEXC_APPLICATION
void print_compact_words(NETptr net, int rm, int indent, int max_visits, 
			 int count_p, FILE *stream)
{
  /***********
   *
   * Print a possibly circular net allowing MAX_VISITS to a state.
   *
   ************/
  long word_count = 0;
  CSptr state, dest_state;
  STACKptr stack = create_stack("States to print");
  id_type *key = ALPH_items(NET_recode_key(net));
  int pos = 0, circular_p = FALSE;
  unsigned char *loc;
  
  /* Initialize work space. */
  
  init_cs_mem();
  
  ARC_BLOCK = NET_arc_block(net);
  DEST_ADDR = NET_start_loc(net);
  SKIP_DATA_P = NET_names_matter(net);
  
  /* Make CS structure for the start state */
  
  DEST_FINAL_P = NET_start_state_final(net);
  state = get_dest_state();
  
  /* Select address decoding function. */
  
  set_decoding_fns(*ARC_BLOCK);
  
  /* Dummy label marking an empty state. */
  
  LABEL_LIMIT = ALPH_len(NET_recode_key(net));
  
  /* Go to the start location and start processing arcs. */
  
  CS_mark(state)++;
  loc = DEST_ADDR;
  
  while (state != NULL)
    {
      if (loc == CS_loc(state))
	{
	  
	  if (CS_final(state))
	    {
	      /* Reached a final state, print only once, not when
		 we are rebounding from recursion
		 */
	      
	      if (CS_mark(state) > 1)
		/* Mark word-final looping site */
		string_out("..");
	      
	      if (cur_pos == 0)
		/* Delimiter for the empty word */
		string_out(" ");
	      
	      buffer_out_s(rm, 0, stream);
	      word_count++;
	    }
	  
	  /* Skip the client number at the beginning of the
	     state in nets that are marked for word-number
	     mapping.
	     */ 
	  
	  if (SKIP_DATA_P)
	    loc = SKIP_DATA(loc);
	}
      /* LOC is 0 when an already processed state is popped
	 from the stack.
	 */
      
      if (loc == 0)
	LABEL_ID = LABEL_LIMIT;
      else
	loc = DECODE_LABEL(loc);
      
      if (LABEL_ID >= LABEL_LIMIT)
	{
	  /* No arcs to process. Rewind the stack. */
	  DEST_FINAL_P = FALSE;
	  CS_mark(state)--;
	  
	  /* When the mark is 0 there are no more copies of the
	     state on the stack.  Unless the state is a high
	     frequency state (i.e. the address is small), we
	     reclaim the data structure.
	     */
	  
	  if (CS_mark(state) == 0 && CS_reclaimable(state))
	    free_cs(state);
	  
	  pos = (int) pop(stack);
	  loc = (unsigned char *) pop(stack);
	  state = (CSptr) pop(stack);
	}
      else
	{
	  loc = DECODE_ADDR(loc);
	  dest_state = get_dest_state();
	  
	  /* Set position in the output buffer and print arc label */
	  cur_pos = pos;
	  
	  if (CS_mark(state) > 1)
	    /* Mark word-internal looping site */
	    string_out("..");
	  
	  LABEL_ID = key[LABEL_ID];
	  
	  if (!(LABEL_ID == EPSILON || LABEL_ID == ALTCHAIN_SYMBOL))
	    label_out(LABEL_ID, DONT_ESCAPE);
	  
	  
	  /* Recurse on destination unless we have been there too often */
	  
	  if (CS_mark(dest_state) < max_visits)
	    {	  
	      /* Push STATE, LOC, and POS. */
	      /* If the last arc has been processed we still need to keep
		 the state on the stack to make sure we don't recurse
		 on it too many times.
		 */
	      
	      push((void *) state, stack);
	      if (LAST_ARC_P)
		push((void *) 0, stack);
	      else
		push((void *) loc, stack);
	      push((void *) pos, stack);
	      
	      state = dest_state;
	      loc = CS_loc(dest_state);
	      CS_mark(state)++;
	      pos = cur_pos;
	    }
	  else
	    {
	      /* Don't recurse, just remember the loop */
	      circular_p = TRUE;
	      cur_pos = 0;
	      
	      if (LAST_ARC_P)
		{
		  CS_mark(state)--;
		  
		  if (CS_mark(state) == 0 && CS_reclaimable(state))
		    free_cs(state); 
		  
		  /* Pop POS, LOC, and STATE. */
		  
		  pos = (int) pop(stack);
		  loc = (unsigned char *) pop(stack);
		  state = (CSptr) pop(stack);
		}
	    }
	}
    }
  
  free_stack(stack);
  free_cs_mem();
  fresh_line_s(72, 0, stream);
  
  if (count_p)
    {
      fsm_fprintf(stream, "Word count: %ld", word_count);
      if (circular_p == TRUE)
	fsm_fprintf(stream, "  (actually, infinite)\n");
      else
	fsm_putc('\n', stream);
    }
}
#endif

/***********************************
 *
 *  Printing size
 *
 ************************************/

static char *s_or_null(unsigned long ln)
{
  if (ln == (unsigned long) 1)
    return("");
  else
    return("s");
}

static void print_num_bytes(unsigned long size, FILE *stream)
{
  unsigned long kilo = 1000;
  unsigned long mega = 1000 * kilo;
  unsigned long giga = 1000 * mega;
  float fsize = (float) size;

  if (size > giga)
    fsm_fprintf(stream, "%.1f Gb", fsize/giga);
  else if (size > mega)
    fsm_fprintf(stream, "%.1f Mb", fsize/mega);
  else if (size > kilo)
    fsm_fprintf(stream, "%.1f Kb", fsize/kilo);
  else
    fsm_fprintf(stream, "%d bytes", size);
}

/* Need this header here because get_net_size() and get_virtual_net_size()
   call each other recursively */
static unsigned long get_net_size(NETptr net);

static unsigned long get_virtual_net_size(NETptr net)
{
  unsigned long size;
  VirtualInfo *vinfo = NET_virtual_info(net);
  VMethods *vmethods;
  HEAPptr btree_heap;

  assert(vinfo);
  vmethods = vinfo->methods;
  btree_heap = (HEAPptr) vinfo->btree_heap;

  size = sizeof(VirtualInfo);

  if (btree_heap)
    size += heap_size(btree_heap, TRUE);

  /* Virtual nets currently don't have the net.labels field set
     properly */

  size += (sizeof(ALPHABETtype) +
	   ((ALPH_max(NET_sigma(net))) * sizeof(id_type)));

  size += ((NET_num_states(net) * sizeof(STATEtype)) +
	   (NET_num_arcs(net) * sizeof(ARCtype)));
  
  assert(vinfo->op1);

  size += get_net_size(vinfo->op1);

  if (vinfo->op2)
    size += get_net_size(vinfo->op2);

  return(size);
}


static unsigned long get_net_size(NETptr net)
{
  /* Computes the size of an ordinary or a compacted
     network. Does not know about vectorized or
     tabularized networks.
  */

  unsigned long size = 0;

  if (NET_is_virtual(net))
    return(get_virtual_net_size(net));
	   
  if (NET_sigma(net))
    size += sizeof(ALPHABETtype)+(ALPH_max(NET_sigma(net)) * sizeof(id_type));

  if (NET_labels(net))
    size += sizeof(ALPHABETtype)+(ALPH_max(NET_labels(net)) * sizeof(id_type));

  if (NET_compacted(net))
    size += NET_block_size(net);
  else if (NET_tabular(net))
    size += (NET_num_states(net) * sizeof(STATEtype));
  else
    size += ((NET_num_states(net) * sizeof(STATEtype)) +
	     (NET_num_arcs(net) * sizeof(ARCtype)));

  return(size);
}

void print_net_size(NETptr net, FILE *stream)
{
  /* Print the number of states and arcs.  If net is not
     compacted, try to count the number of paths as well.
     If the number of paths is larger than the maximum
     size of unsigned long, we cannot print the exact number.
  */
  
  unsigned long net_size = get_net_size(net);

  if (!stream)
    stream = stdout;

  print_num_bytes(net_size, stream);

  fsm_fprintf(stream, ". %ld state%s, %ld arc%s", 
	      NET_num_states(net),
	      s_or_null(NET_num_states(net)), NET_num_arcs(net),
	      s_or_null(NET_num_arcs(net)));
  
  
  if (!(NET_compacted(net)  || NET_tabular(net) || has_arc_vectors_p(net)))
    {
      unsigned long path_count;
      
      switch (index_net(net))
        {
	case 3: /* Net is a virtual net. */
	  fsm_fprintf(stream, ", Virtual");
	  break;
        case 2: /* Net is cyclic */
	  fsm_fprintf(stream, ", Circular");
	  break;
        case 1: /* Overflow */
	  fsm_fprintf(stream, ", more than %lu paths", ULONG_MAX);
	  break;
        case 0:
        default:
	  path_count =
	    (unsigned long) STATE_client_cell(NET_start_state(net));
	  fsm_fprintf(stream, ", %lu path%s", path_count,
		      s_or_null(path_count));
	  break;
        }

      /*
	if (path_count == (unsigned long) 0)
        if (NET_loop_free(net))
	{
	if (null_fsm_p(net))
	fsm_fprintf(stream, ", 0 paths");
	else
	fsm_fprintf(stream, ", more than %lu paths", ULONG_MAX);
	}
        else
	fsm_fprintf(stream, ", Circular");
	else
        fsm_fprintf(stream, ", %lu path%s", path_count,
	s_or_null(path_count));
      */
    }

  if (NET_tabular(net))
    fsm_fprintf(stream, ". Tabular.\n");
  else if (NET_compacted(net))
    fsm_fprintf(stream, ". Compacted.\n");
  else
    fsm_fprintf(stream, ".\n");
}

/***********************************
 *
 *  Printing networks
 *
 ************************************/
#ifndef LEXC_APPLICATION

static void create_reachable_states(unsigned char *loc)
{
  CSptr dest_state;
  int continue_p;
  
  if (SKIP_DATA_P)
    loc = SKIP_DATA(loc);
  
  do {
    loc = DECODE_LABEL(loc);
    if (LABEL_ID >= LABEL_LIMIT)
      return;
    
    continue_p = !LAST_ARC_P;
    loc = DECODE_ADDR(loc);
    dest_state = get_dest_state();
    if (CS_mark(dest_state) == 0)
      {
	CS_mark(dest_state) = 1;
	create_reachable_states(CS_loc(dest_state));
      }
  } while (continue_p);
}

static void print_compact_state(void *cell)
{
  CSptr state = (CSptr) cell;
  unsigned char *loc = CS_loc(state);
  
  state_label_out(CS_addr(state), CS_final(state));
  
  string_out(": ");
  spaces_out(6 - cur_pos);
  
  if (SKIP_DATA_P)
    loc = SKIP_DATA(loc);
  
  do {
    loc = DECODE_LABEL(loc);
    
    if (LABEL_ID >= LABEL_LIMIT)
      {
	word_out_s("(no arcs)", RIGHT_MARGIN, 0, stdout);
	break;
      }
    
    label_out(RECODE_KEY[LABEL_ID], DONT_ESCAPE);
    string_out(" -> ");
    loc = DECODE_ADDR(loc);
    state = get_dest_state();
    state_label_out(CS_addr(state), CS_final(state));
    if (LAST_ARC_P)
      char_out('.');
    else
      char_out(',');
    buffer_out_s(RIGHT_MARGIN, 0, stdout);
    
  } while (!LAST_ARC_P);
  
  fresh_line_s(RIGHT_MARGIN, 0, stdout);
}

static void assign_state_name(void *cell)
{
  CS_addr((CSptr) cell) = STATE_NAME++;
}

void print_compacted_net(NETptr net, int rm, FILE *stream)
{
  extern HEAPptr CS_HEAP;
  
  init_cs_mem();
  
  DEST_FINAL_P = NET_start_state_final(net);
  DEST_ADDR = NET_start_loc(net);
  ARC_BLOCK = NET_arc_block(net);
  RECODE_KEY = ALPH_items(NET_recode_key(net));
  LABEL_LIMIT = ALPH_len(NET_recode_key(net));
  SKIP_DATA_P = NET_names_matter(net);
  
  /* Select address decoding function. */
  
  set_decoding_fns(*ARC_BLOCK);
  
  /*  Create a state record for each state in CS_HEAP
      in the order of reachability from the start state.
      */
  get_dest_state();
  create_reachable_states(DEST_ADDR);
  /* Use STATE_NAME as state counter */
  STATE_NAME = 0;
  map_heap(CS_HEAP, assign_state_name);
  /* Now print the states. */
  
  RIGHT_MARGIN = rm;
  map_heap(CS_HEAP, print_compact_state);
  
  free_cs_mem();
}
#endif

/***********************************
 *
 *  Printing uncompacted nets
 *
 ************************************/

static void print_arc_s(ARCptr arc, int rm, int indent, FILE *stream)
{
  STATEptr dest_state = ARC_destination(arc);
  long int dest_name = (long int) STATE_client_cell(dest_state);
  label_out(ARC_label(arc), DO_ESCAPE);
  string_out(" -> ");
  state_label_out(dest_name, STATE_final(dest_state));
  
  if (next_arc(arc) != NULL)
    char_out(',');
  else 
    char_out('.');
  
  buffer_out_s(rm, indent, stream);
}

static void print_arcs_s(ARCptr arcs, int rm, int indent, FILE *stream)
{
  ARCptr cur;
  
  if (arcs == NULL)
    word_out_s("(no arcs)", rm, indent, stream);
  else
    {
      for (cur = arcs; cur != NULL; cur = next_arc(cur))
	print_arc_s(cur, rm, indent, stream);
    }
  
  fresh_line_s(rm, indent,stream);
}


static void print_state_s(STATEptr state, int indent, int rm, FILE *stream)
{
  long int name = (long int) STATE_client_cell(state);
  
  spaces_out(indent);
  state_label_out(name, STATE_final(state));
  if (STATE_deterministic(state) != TRUE || STATE_visit_mark(state) != 0)
    {
      string_out(" [");
      if (!STATE_deterministic(state))
	{
	  string_out("non-det");
	  if (STATE_visit_mark(state) != 0)
	    string_out(", ");
	}
      if (STATE_visit_mark(state) != 0)
	int_out((long) STATE_visit_mark(state));
      char_out(']');
    }
  string_out(": ");
  spaces_out(indent + (6 - cur_pos));
  print_arcs_s(STATE_arc_set(state), rm, cur_pos, stream);
}

void print_states_s(NETptr net, int rm, FILE *stream)
{
  STATEptr cur;
  
  for (cur = NET_states(net); cur != NULL; cur = next_state(cur))
    print_state_s(cur, 0, rm, stream);
}

void print_uncompacted_net(NETptr net, int rm, FILE *stream)
{
  /* rename_states(net); */
  top_sort_net(net);
  reorder_states(net, state_num_less_p);
  
  print_states_s(net, rm, stream);
}


static void print_virtual_net(NETptr net, int indent, int rm, FILE *stream);

static void print_indented_arcs(STATEptr state,int indent,
				int rm, FILE *stream)
{
  long int name = (long int) STATE_client_cell(state);
  
  spaces_out(indent);
  state_label_out(name, STATE_final(state));
  string_out(": ");
  spaces_out(6 - cur_pos);
  print_arcs_s(STATE_arc_set(state), rm, cur_pos, stream);
}

static void print_indented_states(NETptr net,int indent,int rm,FILE *stream)
{
  STATEptr state;

  top_sort_net(net);
  reorder_states(net, state_num_less_p);
  for (state = NET_states(net); state; state=next_state(state))
    if (!STATE_is_virtual(state))
      print_indented_arcs(state, indent, rm, stream);
}

static void print_indented_net_s(NETptr net, int indent, int rm, FILE *stream)
{
  OBJECTptr name = get_value(net, NETWORKNAME_ATTR);

  if (name && OBJ_type(name) == String)
    fat_string_out(OBJ_str(name));
  fresh_line_s(rm, indent, stream);
/*  spaces_out(indent);
  word_out_s("Flags:", rm, 7, stream);
  print_flags_s(net, rm, 7, stream);
*/  
  if (NET_is_virtual(net))
    print_virtual_net(net, indent, rm, stream);
  else
    print_indented_states(net, indent, rm, stream);
}

static void virtual_net_stats(NETptr net, int *num_virt, int *num_real)
{
  STATEptr state;

  for (state = NET_states(net); state; state=next_state(state))
    if (STATE_is_virtual(state))
      (*num_virt)++;
    else
      (*num_real)++;
}

static void print_virtual_net(NETptr net, int indent, int rm, FILE *stream)
{
  VirtualInfo *vinfo = NET_virtual_info(net);
  VMethods *methods = vinfo->methods;
  NETptr op1 = vinfo->op1;
  NETptr op2 = vinfo->op2;
  int num_real=0, num_virt=0;
  VECTORptr state_vect, data_vect;
  STATEptr state;

  virtual_net_stats(net, &num_virt, &num_real);

  spaces_out(indent);
  string_out("States: ");
  int_out(NET_num_states(net));
  if (num_real > 0)
    {
      string_out(" (");
      int_out(num_real);
      string_out(" real)");
      string_out(", Arcs: ");
      int_out(NET_num_arcs(net));
    }
  fresh_line_s(rm, indent, stream);

  if (NET_num_arcs(net) > 0)
    {
      /* Need to temporarily clear the client cells. Client
	 cells arre used by in top_sort_net() for state numbers.
	 We store the state and its data into two vectors
	 so that we can restore the information after
	 printing the net.
      */

      int i;
      state_vect = make_vector(NET_num_states(net));
      data_vect = make_vector(NET_num_states(net));

      for (state = NET_states(net); state; state=next_state(state))
	{
	  append_to_vector((void *) state, state_vect);
	  append_to_vector((void *) STATE_state_data(state), data_vect);
	  STATE_state_data(state) = NULL;
	}

      print_indented_states(net, indent, rm, stream);

      /* Now restore the state data. */
      for (i = 0; i < NET_num_states(net); i++)
	{
	  vector_element_at((void**)&state, i, state_vect);
	  vector_element_at(&(STATE_state_data(state)), i, data_vect);
	}
      free_vector(state_vect);
      free_vector(data_vect);
    }

  spaces_out(indent);
  string_out("Operation: ");
  string_out(methods->operation);
  fresh_line_s(rm, indent, stream);
  spaces_out(indent);
  if (op1)
    {
      if (op2)
	string_out("operand 1: ");
      else
	string_out("operand: ");

      print_indented_net_s(op1, indent+2, rm, stream);
    }
  if (op2)
    {
      spaces_out(indent);
      string_out("operand 2: ");
      print_indented_net_s(op2, indent+2, rm, stream); 
    }
}


void print_net_s(NETptr net, int rm, FILE *stream)
{
  string_out("Net: ");
  net_name_out(net);
  fresh_line_s(rm, 0, stream);
  
  word_out_s("Flags:", rm, 7, stream);
  print_flags_s(net, rm, 7, stream);
  
#ifndef LEXC_APPLICATION
  if (NET_compacted(net))
    print_compacted_net(net, rm, stream);
  else if (NET_is_virtual(net))
    print_virtual_net(net, 0, rm, stream);
  else
#endif
    print_uncompacted_net(net, rm, stream);
}

void print_net(NETptr net, FILE *stream)
{
  if (!stream)
    stream = stdout;

    print_net_s(net, 72, stream);
}

/*****************************
 *                           *
 *   printing random words   *
 *                           *
 *****************************/

static int find_random_word(NETptr net, int side, int obey_flags_p,
			    int show_flags_p, char *separator)
     /***********
      *
      * Looks for a random path in NET. Collects the output into
      * out_buffer. If obey_flags is non-zero, a seemingly valid
      * path may at the end fail. Thus we must delay producing
      * output until we arrive at a final state.
      *
      ***********/
{
  STATEptr cur_state;
  ARCptr cur_arc;
  int r, arc_count, fail_count;
  id_type l = 0;
  LABELptr label;
  ALPHABETptr flag_reg = make_flag_register();
  FLAG_DIACRptr flag = NULL;
  int n = 0, pos = 0; 

  cur_state = NET_start_state(net);
  
  while (STATE_arc_set(cur_state))
    {		
      cur_arc = NULL;

      arc_count = 0;
      fail_count = 0;

      /* Count the number of arcs in the state and clear the
	 visit marks.
	 */

      for (cur_arc = STATE_arc_set(cur_state);
	   cur_arc != NULL;
	   cur_arc = next_arc(cur_arc))
	{
	  arc_count++;
	  ARC_visit_mark(cur_arc) = 0;
	}


      while (!cur_arc && fail_count < arc_count)
	{

	  /* Randomly choose one arc from the set */

	  for (r = rand() % arc_count, cur_arc = STATE_arc_set(cur_state);
	       r > 0;
	       r--, cur_arc = next_arc(cur_arc));

	  /* If we are obeying flags and if the arc has a
	     flag diacritic, the flag action must succeed.
	     Otherwise we have to try again.
	     */

	  l = ARC_label(cur_arc);
	  if (obey_flags_p)
	    {
	      if (ARC_visit_mark(cur_arc))
		{
		  /* Trying a failed arc for a second time. */
		  cur_arc = NULL;
		}
	      else
		{
		  /* We are only interested in flags that are on
		     the output side of the net. In the event that
		     side is BOTH_SIDES it is not clear what to do
		     here. If there are flags on both sides of an
		     fstpair, the order in which they are tested
		     could make a difference on the outcome. This
		     situation should not be allowed to arise.
		     */

		  switch(side)
		    {
		    case UPPER:
		      flag = flag_diacritic(upper_id(l));
		      break;
		    case LOWER:
		      flag = flag_diacritic(lower_id(l));
		      break;
		    default: /* We only consider "double-sided flags"
				See comment above. */
		      if (!(tuple_id_p(l)))
			flag = flag_diacritic(l);
		    }
		  if (flag && !(flag_action(flag_reg, flag)))
		    {
		      /* Flag constraint fails. Mark the arc and up
			 the failure count. */

		      cur_arc = NULL;
		      fail_count++;
		    }
		}
	    }
	}

      /* If CUR_ARC is NULL at this point, no valid arc could be
	 found. Cannot continue. Just break. */

      if (!cur_arc)
	break;

      label = id_to_label(l);

      /* Set position in the output buffer */
      
      cur_pos = pos;
      
      if (side != BOTH_SIDES && LABEL_arity(label) > 1)
	l = TUPLE_labels(LABEL_tuple(label))[side];
      
      /* Epsilon and altchain symbols are never displayed. Flag
	 diacritics are displayed only if we are not obeying flags
	 or if show_flags_p is TRUE. */

      if (l != EPSILON && l != ALTCHAIN_SYMBOL &&
	  (!flag_diacritic_p(l) || (!obey_flags_p || show_flags_p)))
	{
	  /*	  print_label(l, stream, DONT_ESCAPE); */
	  if (separator && pos > 0)
	    string_out(separator);
	  label_out(l, DONT_ESCAPE);
	  n++;
	  pos = cur_pos;
	}
      
      cur_state = ARC_destination(cur_arc);
      
      /* Quit if the state has no arcs. Otherwise, if the state is final, 
	 there is a 50/50 chance of quitting for words shorter than
	 20 symbols. The longer the word, the more likely it is that
	 we quit. We do not attempt to make random words longer than
	 100 symbols.
	 */
      
      if (!STATE_arc_set(cur_state) || 
	  (STATE_final(cur_state) &&
	   ((n < 20 && (rand() % 2)) ||
	    (n < 40 && (rand() % 3)) ||
	    (n < 60 && (rand() % 4)) ||
	    (n < 80 && (rand() % 5)) ||
	    (n < 100 && (rand() % 6)) ||
	    n >= 100)))
	break;
    }
  
  free_alph(flag_reg);

  if (!STATE_final(cur_state))
    {
      /* The path failed! */

      /*
	fsm_printf ("<-- Oops! This is an error.");
	*/
      return(0);
    }
  char_out('\0');
  return(1);
}

static void random_words_from_cyclic_net(NETptr net, int num, int side,
					 int obey_flags_p, int show_flags_p,
					 FILE *stream,
					 char * separator, int escape_p, 
					 int print_fn(char *buf, FILE *stream)) 
{
  /* If obey_flags is set to non-zero, we may fail to find any
     any valid path in net. In that case we want to inform the
     user instead of looping forever.
     */

  int failure_count = 0;

  reset_out_buffer();

  while (num > 0)
    {
      if (find_random_word(net, side, obey_flags_p, show_flags_p, separator))
	{
	  /* fresh_line_s(72, 0, stream); */
	  print_fn(output_buffer, stream);
	  reset_out_buffer();
	  num--;
	  failure_count = 0;
	}
      else
	{
	  failure_count++;

	  if (failure_count > 50)
	    {
	      fsm_fprintf(stdout, "*** Sorry. Have trouble finding valid ");
	      fsm_fprintf(stdout, " paths in this net.\n");
	      fsm_fprintf(stdout, "*** Quitting after 50 failed attempts.\n");
	      return;
	    }
	  else
	    reset_out_buffer();
	}
    }
  fresh_line_s(72, 0, stream);
}

static char *word_from_number(unsigned long num, STATEptr state, int side, int escape_p, char *separator)
{
  char *word = output_buffer;
  int limit = OUTPUT_BUFFER_SIZE - 1;
  ARCptr arc;
  unsigned long dest_num;
  int len = 0;
  int sep_len = 0;
  id_type id;

  if (separator)
    sep_len = strlen(separator);

  while (STATE_arc_set(state))
    {
      if (STATE_final(state))
	{
	  if (num == 0)
	    break;
	  else
	    num--;
	}
          
      for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	{
	  dest_num = (unsigned long) STATE_client_cell(ARC_destination(arc));
           
	  if (num < dest_num)
	    {
	      switch(side)
		{
		case UPPER:
		  id = upper_id(ARC_label(arc));
		  break;
		case LOWER:
		  id = lower_id(ARC_label(arc));
		  break;
		default:
		  id = ARC_label(arc);
		  break;
		}

	      if (sep_len > 0 && id != EPSILON && id != ALTCHAIN_SYMBOL &&
		  word != output_buffer && (sep_len < limit))
		{
		  strcpy(word, separator);
		  word = word + sep_len;
		  limit -= sep_len;
		}

	      len = FST_copy_label_name(ARC_label(arc), side, word, limit, FALSE,
				    escape_p);

	      if (len < 0)
		{
		  fsm_fprintf(stderr, "Buffer size exceeded: %d.\n",
			  OUTPUT_BUFFER_SIZE);
		  *word = '\0';
		  return(output_buffer);
		}

	      word = word + len;
	      limit -= len;
	      state = ARC_destination(arc);
                
	      break;
	    }
	  else
	    num -= dest_num;
	}
    }
  
  if (!(num == 0 && (STATE_final(state))))
    handle_error("Word not found", "WORD_FROM_NUMBER", (int) num);
  
  *word = '\0';
  return(output_buffer);
}

static void random_words_from_acyclic_net(NETptr net, int num, int side,
					  int obey_flags_p, int show_flags_p,
					  FILE *stream, char * separator,
					  int escape_p, 
					  int print_fn(char *buf, FILE *stream) )
{
  unsigned long int wnum;
  unsigned long lim = (unsigned long) NET_client_cell(NET_start_state(net));
  char *word;

#ifdef MAC_OS
  if (!lim || lim > RAND_MAX)
    {
      random_words_from_cyclic_net(net, num, side, obey_flags_p,
				   show_flags_p, stream,
				   separator, escape_p, print_fn);
      return;
    }
#endif

  while (num-- > 0)
    {
#if defined(MAC_OS) || defined(_WIN32) || defined(Darwin)
      if (lim)
	wnum = rand() % lim;
      else
	{
	  random_words_from_cyclic_net(net, num, side, obey_flags_p, 
				       show_flags_p, stream,
				       separator, escape_p, print_fn);
	  return;
	}
#else
      if (lim && lim > RAND_MAX)
	wnum = lrand48() % lim;
      else if (lim)
	wnum = rand() % lim;
      else
	{
	  random_words_from_cyclic_net(net, num, side, obey_flags_p, 
				       show_flags_p, stream,
				       separator, escape_p, print_fn);
	  return;
	}	
#endif

      if ((word = word_from_number(wnum, NET_start_state(net), side,
				   escape_p, separator)))
	print_fn(word, stream);

    }
}

static int indexed_net_p(NETptr net)
{
  if (NET_names_matter(net) && !(NET_is_virtual(net)) &&
      /* !NET_names_smashed(net) && */
      ((long int) STATE_client_cell(NET_start_state(net)) > 0))
    return(TRUE);
  else
    return(FALSE);
}

void random_words_s(NETptr net, int num, int side, int obey_flags_p,
		    int show_flags_p, FILE *stream, char * separator,
		    int escape_p, int print_fn(char *buf, FILE *stream))
{
  /*********
   *
   * Prints NUM random words from NET with the best available method.
   * Macro random_words(net, num) prints from lower side to standard output.
   *
   **********/
  
  if (STATE_arc_set(NET_start_state(net)) == NULL)
    {
      fsm_fprintf(stderr, "Network is empty.\n");
      return;
    }

  /* We can use the method for acyclic nets if the net is loop free
     and either does not have flag diacritics or we are not obeying
     them */

  if (NET_loop_free(net) && (!(obey_flags_p && has_flag_diacritics_p(net))))
    {
      if (indexed_net_p(net))
	random_words_from_acyclic_net(net, num, side, obey_flags_p,
				      show_flags_p, stream,
				      separator, escape_p, print_fn);
      else
	{
	  if (!index_net(net))
	    random_words_from_acyclic_net(net, num, side, obey_flags_p,
					  show_flags_p, stream,
					  separator,  escape_p, print_fn);
	  else
	    random_words_from_cyclic_net(net, num, side, obey_flags_p, 
					 show_flags_p, stream,
					 separator, escape_p, print_fn);
	}
    }
  else
    random_words_from_cyclic_net(net, num, side, obey_flags_p, show_flags_p,
				 stream, separator, escape_p, print_fn); 
}

/*************************
 *                       *
 *  WORD/NUMBER MAPPING  *
 *                       *
 *************************/

/* Vector for storing the result of word/number mapping. */

static long int *PATH_INDEX_VECTOR = NULL;
static int PATH_INDEX_POS = 0, MAX_PATH_INDEX_POS = 0, PATH_INDEX_INCR = 8;

static void adjust_path_index_vector(void)
{
  if (!PATH_INDEX_VECTOR)
    PATH_INDEX_VECTOR = (long int *)
      malloc((size_t) PATH_INDEX_INCR * sizeof(long int *));
  else
    PATH_INDEX_VECTOR = (long int *)
      realloc(PATH_INDEX_VECTOR, (size_t) PATH_INDEX_INCR*sizeof(long int *));

  if (!PATH_INDEX_VECTOR)
    not_enough_memory("ADJUST_PATH_INDEX_VECTOR");

  /* Max pos is always 1 less than the length because we mark the end
     of the vector by -1. New size increment is double the old increment,
     so the length is increased in powers of 2: 8, 16, 32, etc.
     */

  MAX_PATH_INDEX_POS = PATH_INDEX_INCR - 1;
  PATH_INDEX_INCR = 2 * PATH_INDEX_INCR;
}

static void init_path_index_vector(void)
{
  if (!PATH_INDEX_VECTOR)
    adjust_path_index_vector();

  PATH_INDEX_VECTOR[0] = -1;
  PATH_INDEX_POS = 0;
}

int path_index_count(void)
{
  return(PATH_INDEX_POS);
}

static void record_path_index(long int index)
{
  if (PATH_INDEX_POS > MAX_PATH_INDEX_POS)
    adjust_path_index_vector();

  PATH_INDEX_VECTOR[PATH_INDEX_POS++] = index;
  PATH_INDEX_VECTOR[PATH_INDEX_POS] = -1;
}

static long int *path_index_vector(void)
{
  return(PATH_INDEX_VECTOR);
}

/* Kernel function of the path-to-number mapping: */
static void indices_from_state(NETptr net, id_type *path, STATEptr state,
                               STATEptr return_state, NETptr return_net,
                               id_type *match_table,
                               long int num,
                               int symbol_id_cmp_fn(id_type, id_type))
{
  /* Returns the numbers associated with the path in network. The algorithm
     works as follows:
     If the id at the header of the path matches the first arc leaving
     the state, the count remains the same (initially zero). Every time we
     skip an arc in looking for the match, the count is increased by the 
     number at the destination of the skipped arc (= the number of words
      reachable from that state). Explores the net recursively.

      ** Supported Formats:
     - this function supports labelSet reduced networks. They must have
       been previously indexed by index_net() that also supports the
       labelSet reduction.
     - this function supports the subnet factorization. The subnets
       (represented by def_value arc labels) are traversed by the algorithm
       when matching the input path; however they don't contribute to the
       computation of the number associated to the path. In other words,
       the path in the original network and all its derived paths through
       a subnet have the same number. Consequenlty, it is not necessary to
       index each of the subnets contained in the net before calling
       indices_from_state() on the global network. Moreover, function
       index_net() treats the def_value symbols (i.e. the subnet id-s) as
       ordinary symbols.
  */
      
  ARCptr arc;
  id_type id;
  NETptr subnet; /* for supporting subnet factorization. */

  /* Test if we found a path and if we are traversing a subnet: */
  if (STATE_final(state) && (return_state != NULL))
  {
      /* Return to the global network i.e. continue the mapping from 
         the return_state now instead of the current state. */
      indices_from_state(return_net, path, return_state,
                         NULL, NULL, match_table, num,
                         symbol_id_cmp_fn);

  } 
  else if (*path == ID_EOS)
  {
      /* The path is exhausted: check if the current state is a hit. */
      
      if (STATE_final(state))
      {
	  record_path_index(num);
	  num++;
      }
      	
      /* Look for epsilon arcs: */ 
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))    
	{
	  if (match_table[ARC_label(arc)] == EPSILON ||
	      match_table[ARC_label(arc)] == ALTCHAIN_SYMBOL)
              indices_from_state(net, path, ARC_destination(arc),
                                 return_state,
                                 return_net,
                                 match_table,
                                 num,
                                 symbol_id_cmp_fn);
          if (return_state == NULL)   
              num += (long int) STATE_client_cell(ARC_destination(arc));
	}
  } /* if (*path) == ID_EOS */
  else
  {
      if (STATE_final(state))
          num++;

      /* Sweep the arcs of the current state location: */
      for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	{
            if (NET_reduced_labelset(net))
            {
                /* Reduced case:
                   ------------- */
                int i=0;
                RANGEptr equivalence_class= NULL;
                MATCH_TABLEptr net_match_table= NET_upmatch_table(net);
                equivalence_class= MATCH_TABLE_class(net_match_table) + ARC_label(arc);
                /* Sweep the arcs of the class: */
                for (i=0; i < RANGE_len(equivalence_class); i++)
                {
                    id = match_table[RANGE_items(equivalence_class)[i]];
	      
                    if (id == EPSILON || id == ALTCHAIN_SYMBOL)
                        indices_from_state(net, path, ARC_destination(arc),
                                           return_state, return_net,
                                           match_table, num,
                                           symbol_id_cmp_fn);
                    else if (symbol_id_cmp_fn(id, *path))
                        indices_from_state(net, path+1,ARC_destination(arc),
                                           return_state, return_net,
                                           match_table, num,
                                           symbol_id_cmp_fn);

                    /* No match: we skip the arc and increase the count by the
                       number at the destination of the skipped arc: 
                       Note that if we are traversing a subnet (i.e. when
                       return_state is non NULL), it is not necessary to update num
                       since subnets are not supposed to have been indexed. */
                    if (return_state == NULL)   
                        num += (long int) STATE_client_cell(ARC_destination(arc));   
                } /* for i */
            }
            else
            {                                    
                /* Standard case:
                   -------------- */
                id = match_table[ARC_label(arc)];
                    
                if (id == EPSILON || id == ALTCHAIN_SYMBOL)
                    indices_from_state(net, path, ARC_destination(arc),
                                       return_state, return_net,
                                       match_table, num,
                                       symbol_id_cmp_fn);
                else if (symbol_id_cmp_fn(id, *path))
                    indices_from_state(net, path+1, ARC_destination(arc),
                                       return_state, return_net,
                                       match_table, num,
                                       symbol_id_cmp_fn);
                else
                {
                    /* Look for a possible subnet label:
                       ---------------------------------

                       We check if the id is a def_value. */
                    subnet= (NETptr) get_def_uncopied(id);
                        
                    if (subnet != NULL)
                    {
                        /* We have detected a subnet: we continue the search in the
                           subnet, but we don't go ahead in the input path. */
                        indices_from_state(subnet, path,
                                           NET_start_state(subnet),
                                           ARC_destination(arc),
                                           net,
                                           match_table, num,
                                           symbol_id_cmp_fn);
                    }  /* subnet != NULL */
                }
                
                /* Skip the arc and increase the count by the number at the
                   destination of the skipped arc.
                   Note that if we are traversing a subnet (i.e. when 
                   return_state is non NULL), it is not necessary to update
                   num since subnets are not supposed to have been indexed.
                */
                if (return_state == NULL)                 
                    num += (long int) STATE_client_cell(ARC_destination(arc));
            } /* else NET_reduced_labelset(net) */
	} /* for arc */
  } /* *path == ID_EOS */
} /* indices_from_state */


static int no_index_p(NETptr net)
{
    /* Checks NET_names_matter flag to verify that has been processed
       by index_net(). If the net has not been indexed, index_net() is
       called.
    */
     
    /* Note that we can't rely on the flag NET_loop_free(net) to reject
       the net as non-indexable, since when this flag is FALSE it gives
       in fact no information: it would require to call first a function 
       for detecting loops in a net, which is nearly as costly as running
       directly index_net().
       Once index_net() has been called on net , the flag NET_loop_free
       is correctly set. */
   
    if (indexed_net_p(net))
        return(FALSE);
    else
    {
        if (index_net(net) == 0)
	  {
	    if (NET_loop_free(net))
	      return(FALSE);
	    else
	      return(TRUE);
	  }
	return(FALSE);
    }
} /*  no_index_p() */


int default_symbol_id_cmp_fn(id_type id1, id_type id2)
{
    return(id1 == id2);
}


long int *path_to_numbers(id_type *path, NETptr net, id_type *match_table)
{
    return(path_to_numbers_cmp(path, net, match_table,
                               default_symbol_id_cmp_fn));
}


long int *path_to_numbers_cmp(id_type *path, NETptr net, id_type* match_table,
                              int symbol_id_cmp_fn(id_type, id_type))
{
  /*  Assumes that NET has been indexed by index_net(). If PATH is found,
      returns the numbers associated with the path, otherwise NULL to signal
      failure.
      Note:
      - this function supports the labelSet reduced networks.
      - this function supports the subnet factorization, (see the head comment
        of function indices_from_states() for more details).
  */

      /* Initializes the record vector. */
    init_path_index_vector();
    
    /* Check the NET_names_matter flag and the content of the client_cell of the
       net start state to verify that NET has been processed by index_net(). */  
    if ( NET_names_matter(net) &&
         ((long int) STATE_client_cell(NET_start_state(net)) > 0))    
    {

        /* If the net is reduced, build one of the net's match tables: */
        if (NET_reduced_labelset(net) && (NET_upmatch_table(net) == NULL))
        { /* Build one of the 2 match tables, (the side -upper or lower-
             doesn't matter since what we need is only the "eqv_classes"
             array). */
            get_match_table(net, LOWER);
        }
  
        indices_from_state(net, path, NET_start_state(net), NULL, NULL,
                           match_table, 0,
                           symbol_id_cmp_fn);
    }
    else
    {
        fsm_fprintf(stderr, "Can't compute path numbers: the net is not indexed !\n");
    }

  return(path_index_vector());
} /* path_to_numbers_cmp */




char *number_to_word(unsigned long num, NETptr net, int side, int escape_p)
{
  /* Assumes that NET has been indexed by index_net(). Returns the word
     associated with the number. Uses output_buffer (in pretty-p.c) to
     store the word. If net is not and cannot be indexed, returns NULL
     to signal failure, otherwise a pointer to the output buffer.
     If the net is a transducer the word is printed from the indicated
     side.
     */

  if (no_index_p(net))
    return(NULL);
    
  if (num >= (unsigned long) STATE_client_cell(NET_start_state(net)))
    {
      fsm_fprintf(stderr, "Argument too large: %ld.\n", num);
      return(NULL);
    }
     
  return(word_from_number(num, NET_start_state(net), side, escape_p, ""));
}


long *word_to_numbers(char *word, NETptr net, int side)
{
  /* Returns the numbers associated with a word in an indexed network.
     If the net is a deterministic simple network, it is associated
     with a unique number. If the net is a transducer, there may
     be several paths that contain the word on one side.
     
     If the network does not have parse table, the table is created
     and assigned to NET_upper_parse_table or NET_lower_parse_table
     depending on the side.
     If the net has not been indexed before, index_net() is called
     from no_index_p().
     Prints an error message to stderr and returns -1 if the operation
     fails for some reason.
     */
     
  PARSE_TBL parse_table;
  int error;
  id_type *match_table;
    
  switch (side)
    {
    case UPPER:
      if (!(parse_table = NET_upper_parse_table(net)))
	parse_table = make_parse_table(net, NULL, UPPER);

      match_table = UPPER_MATCH;
      break;
    case LOWER:
      if (!(parse_table = NET_lower_parse_table(net)))
	parse_table = make_parse_table(net, NULL, LOWER);

      match_table = LOWER_MATCH;
      break;
    default:
      handle_error("Argument is neither UPPER nor LOWER", "WORD_TO_NUMBER",
		   side);
      return(NULL);
    }    

  if ((error = FST_parse_string (word, parse_table, DONT_ZERO_TO_EPS,
                             DONT_IGNORE_WHITES, DONT_ESCAPE,
                             DONT_REPORT_ERROR)))
      return(NULL);

  if (no_index_p(net))
      return(NULL);
  
  return(path_to_numbers(INPUT_SEQ, net, match_table));
}

static long int *path_nums_to_locations(uint32 *block)
{
  int i;

  for (i = 0; i < PATH_INDEX_POS; i++)
    PATH_INDEX_VECTOR[i] = block[PATH_INDEX_VECTOR[i]];

  return(PATH_INDEX_VECTOR);
}


long int *path_to_locations(id_type *path, NETptr index_net, int side)
{
  /* Returns the location corresponding to the word number of
     the path in the file position map stored as the property
     WORD-TO-FILEPOS-MAP, as built by build_headwork_index().
     */

  OBJECTptr obj;
  id_type *match_table;

  if (!(obj = get_value(index_net, WORD_TO_FILEPOS_MAP_ATTR)) ||
      OBJ_type(obj) != Byte_block)
    {
      fsm_fprintf(stderr, "No WORD-TO-FILEPOS-MAP on the index net!\n");
      return(NULL);
    }
  
  update_match_tables();

  switch (side)
    {
    case UPPER:
      match_table = UPPER_MATCH;
      break;
    case LOWER:
      match_table = LOWER_MATCH;
      break;
    default:
      handle_error("Argument is neither UPPER nor LOWER", "WORD_TO_NUMBER",
		   side);
      return(NULL);
    }    

  if (!path_to_numbers(path, index_net, match_table))
    return(NULL);
 
  return(path_nums_to_locations(BYTE_BLOCK_block(OBJ_byte_block(obj))));
}

long int *word_to_locations(char *word, NETptr index_net, int side)
{
  /* Returns the locations corresponding to the word number of
     word in the file position map stored as the property
     WORD-TO-FILEPOS-MAP, as built by build_headwork_index().
     */

  OBJECTptr obj;

  if (!(obj = get_value(index_net, WORD_TO_FILEPOS_MAP_ATTR)) ||
      OBJ_type(obj) != Byte_block)
    {
      fsm_fprintf(stderr, "No WORD-TO-FILEPOS-MAP on the index net!\n");
      return(NULL);
    }
  
  if (!word_to_numbers(word, index_net, side))
    return(NULL);

  return(path_nums_to_locations(BYTE_BLOCK_block(OBJ_byte_block(obj))));
}

/****************************************
*
*	INSPECT
*
****************************************/

static void print_path (STACKptr stack)
{
  int i, lim = STACK_cur_loc(stack);
  
  for (i = 1; i < lim; i=i+2)
    label_out(ARC_label((ARCptr) STACK_cell(stack, i)), DO_ESCAPE);
  
  if (lim > 0)
    {
      string_out(" --> ");
    }
}

static int state_out(STATEptr state, int rm, int level, STACKptr stack)
{
  ARCptr arc;
  int n = 0;
  
  print_path(stack);
  
  string_out("Level ");
  int_out(level);
  
  if (STATE_visit_mark(state) > 0)
    {
      int i, lv = 1;
      for (i = 0; i < STACK_cur_loc(stack); i = i+2, lv++)
	if (state == (STATEptr) STACK_cell(stack, i))
	  break;
      string_out(" (= ");
      int_out(lv);
      char_out(')');
    }
  
  if (STATE_final(state))
    string_out(" (final)");
  fresh_line(rm,0);
  
  if (STATE_arc_set(state) != NULL)
    {
      string_out("Arcs: ");
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
	{
	  int_out(++n);
	  string_out(": ");
	  label_out(ARC_label(arc), DO_ESCAPE);
	  if (next_arc(arc) != NULL)
	    char_out(',');
	  buffer_out(rm, 6);
	}
      fresh_line(rm, 0);
    }
  return(n);
}


/***********************************
 *
 *  Inspecting
 *
 ************************************/

static ARCptr nth_arc (STATEptr state, int n)
{
  ARCptr arc;
  for (arc = STATE_arc_set(state); n > 1 && arc != NULL; n--)
    arc = next_arc(arc);
  return(arc);
}

static char *get_word(char *str, int limit)
{
  /* gets a line of input from STDIN and places it to STR */
  int ch ;
  int i = 0 ;

  /* skip white space */
  do {
    ch = fsm_getc(stdin);
  } while (isspace(ch) && ch != (int) '\n' && ch != EOF);
  
  if (ch != EOF && ch != (int) '\n')
    /* Found first character of a word */
    do {
      str[i++] = (char) ch;
      ch = fsm_getc(stdin);
    } while (ch != '\n' && i < limit && ch != EOF);
      
  /* Recover from error if necessary */

  if (ch == EOF)
    clearerr(stdin);

  str[i] = '\0';
  return(str);
}

void inspect_net (NETptr net, int rm, int output_side)
{
  /* Interactive inspector for finite-state networks.  Shows
     path to the current state and the number of previous visits,
     if the path loops.
     */
  
  char word[16];
  int command, error_p, arc_count, new_level, i, diff, level = 1;
  ARCptr arc = NULL;
  STATEptr state;
  STACKptr stack;
  
  if (NET_is_virtual(net))
    {
      fsm_fprintf(stderr, "Sorry. Cannot inspect a virtual net yet.\n");
      return;
    }

  state = NET_start_state(net);
  stack = create_stack("States inspected");
    
  fsm_printf("'N' transits arc N, '-N' returns to level N, ");
  fsm_printf("'<' to previous level, '0' quits.\n");
  
  do {
    error_p = FALSE;
    arc_count = state_out(state, rm, level, stack);
    
    fsm_printf("\nInspect: ");
    get_word(word, 15);
    
    if (strcmp(word, "<") == 0)
      command = 0 - level + 1;
    else
      command = atoi(word);
    
    if (command == 0 && !(strcmp(word, "0") == 0 || strcmp(word, "<") == 0))
      {
	fsm_printf("'N' transits arc N, '-N' returns to level N, ");
	fsm_printf("'<' to previous level, '0' quits.\n");
	error_p = TRUE;
      }
    else if (command < 0)
      {
	new_level = 0 - command;
	if (new_level < level)
	  {
	    diff = level - new_level;
	    for (i = 0; i < diff; i++)
	      {
		pop(stack);
		state = (STATEptr) pop(stack);
		STATE_visit_mark(state)--;
	      }
	    level = new_level;
	  }
	else if (new_level > level)
	  fsm_printf("State names range from 1 to %d\n", level);
      }
    else if (command > 0)
      {
	if (command <= arc_count)
	  {
	    arc = nth_arc(state, command);
	    STATE_visit_mark(state)++;
	    push((void *) state, stack);
	    push((void *)arc, stack);
	    state = ARC_destination(arc);
	    level++;
	  }
	else
	  fsm_printf("Arc names range from 1 to %d\n", arc_count);
      }
  } while (command != 0 || error_p);
  
  /* Clear out remaining visit marks before releasing the stack */
  for (i = 1; i < level; i++)
    {
      pop(stack);
      state = (STATEptr) pop(stack);
      STATE_visit_mark(state)--;
    }
  
  free_stack(stack);
}

static void id_to_tally(id_type id, TALLYptr tally, 
		 int * tally_count, id_type *max_tally_id)
{
  if (id != EPSILON)
    {
      if (TALLY_freq(tally + id) == 0)
	( * tally_count )++;
      if (id > (* max_tally_id))
	(* max_tally_id) = id;
      TALLY_freq(tally + id)++;
    }
}

static void id_to_buffer(id_type id, id_type * buffer, int * cur_pos)
{
    static int add_size = 0;

    if ( (* cur_pos) < MAX_TALLY_ID_LENGTH )
      {
        buffer[(*cur_pos)++] = id;
      }
    else
    {
        if ((add_size & 0xFF) == 0)       /* each 256 call  */
            fsm_fprintf(stderr, "Output buffer full, please increase MAX_TALLY_ID_LENGTH to at least %d\n", MAX_TALLY_ID_LENGTH + add_size + 256);
        add_size++;
    }
}

TALLYptr tally_paths(NETptr net, int max_visits, int max_paths, int side,
		     unsigned long * word_count,
		     int * tally_count,
		     id_type * max_tally_id,
		     int escape_p, int obey_flags_p)
{
  /***********
   *
   * Adapted from net-io.c:print_paths()
   * tally a possibly circular net allowing MAX_VISITS to a state. TG
   *
   ************/

  unsigned long count = 0;
  STACKptr state_stack = create_stack("States to print");
  STACKptr pos_stack = create_stack("Positions to remember");
  STACKptr arc_stack = create_stack("Arcs to print");
  STACKptr attr_stack = create_stack("Attributes to remember");
  STACKptr val_stack = create_stack("Values to remember");
  ALPHABETptr flag_reg = make_flag_register();
  STATEptr state = NET_start_state(net);
  ARCptr arc = STATE_arc_set(state);
  int pos = 0, cur_pos = 0, circular_p = FALSE, continue_p, i,
    tally_size = LIM_id_count(LABEL_MAP);
  FLAG_DIACRptr flag;
  id_type id, attr = 0, old_val = 0;
  id_type outids[MAX_TALLY_ID_LENGTH] = {0};
  TALLYptr word_tally = 0;

  * tally_count =  0;
  word_tally = make_symbol_tally_export(tally_size);
  * max_tally_id = 0;
 

  for (i = 0; i < tally_size ; i++)
    TALLY_label(word_tally + i) = i;

  if (NET_visit_marks_dirty(net))
    prepare_visit_marks(net, 0);
  
  STATE_visit_mark(state)++;
  NET_visit_marks_dirty(net) = TRUE;
  
  /* Visit marks will be set back to 0
     if the printing is not interrupted */
  
  /* reset_out_buffer(); */
  cur_pos = 0;

  while (state != NULL)
    {
      if (STATE_final(state) == TRUE && arc == STATE_arc_set(state))
	{
	  /* Reached a final state, print only once, not when
	     we are rebounding from recursion
	     */
	  for ( i = 0; i < cur_pos; i++)
	    {
	      id_to_tally(outids[i], word_tally,
			  tally_count, max_tally_id);
	    }
	  
	  if (++count == (unsigned long) max_paths)
	    {
	      STATE_visit_mark(state)--;
	      while ((state = (STATEptr) pop(state_stack)))
		STATE_visit_mark(state)--;
	      break;
	    }
	}
      
      if (arc != NULL)
	{
	  /* Set position in the output buffer and print arc label
	     unless it is a flag diacritic */

	  cur_pos = pos;

	  id = ARC_label(arc);

	  /* If ID is a tuple ID, it may contain a flag
	     diacritic on one side. */

/*
	  if (!(flag = flag_diacritic(upper_id(id))))
	    flag = flag_diacritic(lower_id(id));
*/	    
	  switch (side)
	    {
	    case UPPER:
	      flag = flag_diacritic(upper_id(id));
	      break;
	    case LOWER:
	      flag = flag_diacritic(lower_id(id));
	      break;
	    default:
	      if (!(flag = flag_diacritic(upper_id(id))))
		flag = flag_diacritic(lower_id(id));
	      break;
	    }

	  if (flag && obey_flags_p)
	    {
	      attr = FLAG_DIACR_attribute(flag);
	      old_val =  flag_value(flag_reg, attr);
	      if (flag_action(flag_reg, flag))
		{
		  /* If the label is a tuple print the label on
		     the opposite side */

		  if ((side == UPPER && !(flag_diacritic(upper_id(id)))))
		    {
		      id_to_buffer(upper_id(ARC_label(arc)), outids, &cur_pos);
		    }
		    
		  else if ((side == LOWER &&
			    !(flag_diacritic(lower_id(id)))))
		    {
		      id_to_buffer(lower_id(ARC_label(arc)), outids, &cur_pos);
		    }
		  continue_p = TRUE;
		}
	      else
		continue_p = FALSE;
	    }
	  else 
	    {
	      if (!(id == EPSILON || id == ALTCHAIN_SYMBOL))
		{
		  if (side == BOTH_SIDES)
		    {
		      id_to_buffer (ARC_label(arc), outids, &cur_pos);
		    }
		  else if (side == UPPER)
		    {
		      id_to_buffer(upper_id(ARC_label(arc)), outids, &cur_pos);
		    }
		  else if (side == LOWER)
		    {
		      id_to_buffer(lower_id(ARC_label(arc)), outids, &cur_pos);
		    }
		  else if (side == UPPER_OR_LOWER )
		    {
		      if (LABEL_arity(id_to_label(id)) == 1)
			{
			  id_to_buffer(ARC_label(arc), outids, &cur_pos);
			}
		      else
			{
			  id_to_buffer(upper_id(ARC_label(arc)), outids, &cur_pos);
			  id_to_buffer(lower_id(ARC_label(arc)), outids, &cur_pos);
			}
		    }
		}

	      continue_p = TRUE;
	      attr = 0;
	      old_val = 0;
	    }

	  /* Recurse on destination unless we have been here too often or
	     prohibited by a flag_diacritic constraint. */
	  
	  if ((int)STATE_visit_mark(ARC_destination(arc)) < max_visits &&
	      continue_p)
	    {
	      push((void *) state, state_stack);
	      push((void *) pos, pos_stack);
	      push((void *) next_arc(arc), arc_stack);
	      push((void *) (int) attr, attr_stack);
	      push((void *) (int) old_val, val_stack);

	      state = ARC_destination(arc);
	      STATE_visit_mark(state)++;
	      pos = cur_pos;
	      arc = STATE_arc_set(state);
	    }
	  else
	    {
	      /* Don't recurse, just remember the loop */
	      arc = next_arc(arc);
	      circular_p = TRUE;
	      cur_pos = 0;
	    }
	}
      else
	{
	  STATE_visit_mark(state)--;
	  state = (STATEptr) pop(state_stack);
	  pos = (int) pop(pos_stack);
	  arc = (ARCptr) pop(arc_stack);
	  flag_value(flag_reg, (int) pop(attr_stack))
	    = (int) pop(val_stack);
	}
    }
  
  NET_visit_marks_dirty(net) = FALSE;
  free_stack(state_stack);
  free_stack(pos_stack);
  free_stack(arc_stack); 
  free_stack(attr_stack); 
  free_stack(val_stack); 
  free_alph(flag_reg);
  cur_pos = 0;

  *word_count = count;
  return word_tally;
}

#endif /* SLIM_FST */
