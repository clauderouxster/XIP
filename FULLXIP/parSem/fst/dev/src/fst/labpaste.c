/* $Id: labpaste.c,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/***************************************
 *
 *   FUNCTIONS FOR LABEL PASTING
 *
 *     'a'  'b'  'c'  ->  'abc'
 *
 **************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fst/types.h"
#include "fst/struct.h"
#include "fst/modify.h"
#include "fst/labpaste.h"
#include "fst/cfsm.h"
#include "fst/mark.h"
#include "fst/utf8_fatstr_util.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 


/* Pasting is not allowed if length != 1 (tag) or if arrity != 1.

   FLAG_handle_state(X) says what has to be done when we arrive at state X.
   The flag can have the values:

       AHEAD         ... go ahead
       ARC_AHEAD     ... make an arc + go ahead
       ARC_BACK      ... make an arc + go back
       ARC_SET_AHEAD ... make an arc + set label buffer + go ahead

   FLAG_handle_arc(X) says what has to be done with the label of arc X.
   The flag can have the values:

       NO_INFO       ... we have no information what to do
       DO_PASTE      ... label has to be pasted with other labels
       DONT_PASTE    ... label must not be pasted with other labels
   */

enum handle_state_p { AHEAD, ARC_AHEAD, ARC_BACK, ARC_SET_AHEAD } ;
enum handle_arc_p { NO_INFO, DO_PASTE, DONT_PASTE } ;

static struct PASTED_LABELS {
  long buffer_size;
  char *buffer;              /* temporary buffer to paste labels */
}
*pasted_labels;


static void PLB_alloc_paste_buffer(long bf_size)
{
  PSTLAB_ptr = (struct PASTED_LABELS *) malloc (sizeof(struct PASTED_LABELS));
  PSTLAB_size = bf_size;
  PSTLAB_buffer = (char *) malloc ( bf_size * sizeof (char));
}

static void PLB_free_paste_buffer()
{
  free (PSTLAB_buffer);  free (PSTLAB_ptr);
}

static void PLB_set_flags_handle_state(NETptr net, int handle_state_p)
{
  /* Sets all FLAG_handle_state(X) to handle_state_p. */

  STATEptr state;

  for (state = NET_states(net); state; state = next_state(state))
    FLAG_handle_state(state) = handle_state_p;
}


static void PLB_set_flags_handle_arc(NETptr net, int handle_arc_p)
{
  /* Sets all FLAG_handle_arc(X) to handle_arc_p. */

  STATEptr state;  ARCptr arc;

  for (state = NET_states(net); state; state = next_state(state))
    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
      FLAG_handle_arc(arc) = handle_arc_p;
}


static void PLB_initialize_one_state_flag
  (STATEptr state, long len, long *max_len)
{
    /* Marks the state with ARC_SET_AHEAD if a loop of any length starts/ends
       there, with ARC_BACK if it is a final state without outgoing arcs,
       with ARC_AHEAD if it is a final state with outgoing arcs and otherwise
       with AHEAD.
       Then all following states are visited. Visit marks are used.
       The length of the longest concatenatenation of symbols is memorized.
       */

    ARCptr arc;

    /* first of all, unconditionally  update the max length of the 
     * label buffer
     */

    if (*max_len < len)
        *max_len = len;

    if (STATE_visit_mark(state) == DONE)  return; /* already marked */

    if (STATE_visit_mark(state) == IN_PROCESS)
    { FLAG_handle_state(state) = ARC_SET_AHEAD; return; }

    if (STATE_final(state) == TRUE)
    {
        if (! STATE_arc_set(state))
            FLAG_handle_state(state) = ARC_BACK;
        else
            FLAG_handle_state(state) = ARC_AHEAD;
    }

    STATE_visit_mark(state) = IN_PROCESS;

    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
    {
        int sym_len = fat_strlen(id_to_atom_name(upper_id(ARC_label(arc))));

        PLB_initialize_one_state_flag (ARC_destination(arc),
                                       len + sym_len, max_len);
    }
    STATE_visit_mark(state) = DONE;
}


static void PLB_initialize_all_state_flags(NETptr net, long *max_len)
{
  /* Initializes FLAG_handle_state(X) of all states. */

  *max_len = 0;
  PLB_set_flags_handle_state(net, AHEAD);
  PLB_initialize_one_state_flag(NET_start_state(net), 0, max_len);
}


static char *PLB_paste_label_to_buffer(ARCptr arc, char *bf_end)
{
  /* The label of 'arc' is appended to the buffer content if allowed
     and a pointer to the new buffer end is returned.
     Pasting is not allowed if length > 1 (tag) or if arrity != 1.
     */

  LABELptr label;  int len;

  if (FLAG_handle_arc(arc) == DONT_PASTE)  return (bf_end);

  label = id_to_label( ARC_label(arc));
  if (LABEL_arity(label) != 1)
    { FLAG_handle_arc(arc) = DONT_PASTE; return (bf_end); }

  /* append the print name of the label to the buffer content */

  FST_fat_to_thin( LABEL_atom( label ), bf_end);

  len = strlen(bf_end);
  if (len > 1)
    { FLAG_handle_arc(arc) = DONT_PASTE; return (bf_end); }

  /* when we came until here then pasting was ok */

  FLAG_handle_arc(arc) = DO_PASTE;
  bf_end += len;
  return (bf_end);
}


static STATEptr PLB_get_client_state(NETptr net2, STATEptr state)
{
  /* Returns a pointer to the client cell of 'state' which is in an other
     network. If this client cell does not exist then it will be made.
     */

  STATEptr client;

  if (! STATE_client_cell(state))
    {
      client = make_state(NULL, net2);
      STATE_client_cell(state) = (void *) client;

      if (FLAG_start_state(state) == TRUE)  NET_start_state(net2) = client;
      if (STATE_final(state) == TRUE)  STATE_final(client) = TRUE;
    }
  else                           /* client cell exists already */
    client = (STATEptr) STATE_client_cell(state);

  return (client);
}


static int PLB_arc_exists_already
  (STATEptr source, STATEptr destination, id_type id)
{
  /* Checks whether an arc exists already between the client cells of
     source and destination.
     */

  ARCptr arc;

  if (! STATE_client_cell(source))  return (FALSE);
  if (! STATE_client_cell(destination))  return (FALSE);

  source = (STATEptr) STATE_client_cell(source);
  destination = (STATEptr) STATE_client_cell(destination);

  for (arc = STATE_arc_set(source); arc; arc = next_arc(arc))
    {
      if (ARC_destination(arc) != destination) 
	continue;
      if (ARC_label(arc) == id)  
	return (TRUE);
    }

  return (FALSE);
}


static void PLB_make_arc_with_id
  (NETptr net2, STATEptr source, STATEptr destination, id_type id)
{
  /* Makes an arc labled with 'id' from the client state of source to
     the client state of destination. If those client states do not
     exist then they are made.
     */

  if (PLB_arc_exists_already (source, destination, id))  return;

  source = PLB_get_client_state (net2, source);
  destination = PLB_get_client_state (net2, destination);

  push_arc (make_arc (id, destination, net2), source);
}


static int PLB_make_arc_from_buffer
  (NETptr net2, STATEptr source, STATEptr destination,
   char *bf_begin, char *bf_end)
{
  id_type id;
  int error= NO_ERROR;

  if (bf_begin == bf_end)  
    return(ERR_BAD_INPUT);

  *bf_end = '\0';

  error = FST_intern_literal (&id, bf_begin);

  if (error== NO_ERROR)
    PLB_make_arc_with_id(net2, source, destination, id);
   
  return(error);
}


static void PLB_handle_next_state
  (NETptr net2, STATEptr source, STATEptr current,
   char *bf_begin, char *bf_end)
{
  /* Handles one state and calls the same function recursively for all
     following states.
     */

  ARCptr arc;  char *new_bf_end;

  /* if the current state is marked ARC_SET_AHEAD, ARC_AHEAD or ARC_BACK
     and the buffer is not empty then an arc in net2 labled with the
     current buffer content must be created. */

  if (FLAG_handle_state(current) != AHEAD)
    PLB_make_arc_from_buffer(net2, source, current, bf_begin, bf_end);

  if (FLAG_handle_state(current) == ARC_BACK)  return;

  if (FLAG_handle_state(current) == ARC_SET_AHEAD)
    {
      source = current;  bf_begin = bf_end;
      FLAG_handle_state(current) = ARC_BACK;
    }

  /* the flag of the current state was not ARC_BACK. therefore go ahead
     to all following states */

  for (arc = STATE_arc_set(current); arc; arc = next_arc(arc))
    {
      new_bf_end = PLB_paste_label_to_buffer(arc, bf_end);
                        /* only done if pasting of this label is allowed */

      if (FLAG_handle_arc(arc) == DO_PASTE)
	{
	  PLB_handle_next_state
	    (net2, source, ARC_destination(arc), bf_begin, new_bf_end);
	}
      else    /* FLAG_handle_arc(arc) == DONT_PASTE */
	{
	  PLB_make_arc_from_buffer
	    (net2, source, current, bf_begin, new_bf_end);

	  PLB_make_arc_with_id
	    (net2, current, ARC_destination(arc), ARC_label(arc));

	  PLB_handle_next_state
	    (net2, ARC_destination(arc), ARC_destination(arc),
	     new_bf_end, new_bf_end);
	}
    }
}


NETptr PLB_paste_net_labels(NETptr net1, int reclaim_p, int minimize_p)
{
  /* Returns net2 which is equivalent to net1 but where all labels are
     pasted as far as possible.
     */

  NETptr net2;  long max_len;

  /* return net1 if it is a null fsm or an epsilon fsm */

  if (! STATE_arc_set (NET_start_state (net1)))
    return (net1);

  /* initialize all state and arc flags and all client cell pointers*/

  clear_state_marks_and_client_cells (net1); /* all visit marks = 0 */
  PLB_initialize_all_state_flags (net1, &max_len);
  PLB_set_flags_handle_arc (net1, NO_INFO);

  clear_state_marks_and_client_cells (net1); /* all visit marks = 0 */
  FLAG_start_state (NET_start_state (net1)) = TRUE; /* mark start state */

  /* visite all states in net1 and generate in parallel states in net2 */

  net2 = make_network();

  PLB_alloc_paste_buffer(max_len + 1);

  PLB_handle_next_state(net2, NET_start_state(net1), NET_start_state(net1),
			PSTLAB_buffer, PSTLAB_buffer);

  PLB_free_paste_buffer();

  /* free net1 if required */

  if (reclaim_p == DO_RECLAIM)
    free_network(net1);
  else
    clear_state_marks_and_client_cells(net1);

  /* update sigma and labels of net2 */

  update_label_alphabet (net2);
  free_alph (NET_sigma(net2));
  NET_sigma(net2) = label_sigma(net2);

  NET_minimized(net2) = FALSE;

  if (minimize_p == DO_MINIMIZE)
    net2 = min_fsm(net2, DONT_COPY);

  return (net2);
}

#endif /* SLIM_FST */
