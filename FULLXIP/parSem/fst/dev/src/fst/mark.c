/* $Id: mark.c,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  MODIFY.C
 **
 **  Todd Yampol, Xerox PARC, April 1990
 **  Lauri Karttunen, Xerox PARC, February 1991
 **
 **  Support for the finite-state calculus
 **
 ******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>


#include "fst/mark.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */


/*********************************
 *
 *  Setting marks
 *
 **********************************/

void prepare_arc_marks(NETptr net, int mark)
     /***********
      *
      * Iteratively sets the arc marks to mark.
      *
      ***********/
{
  STATEptr cur_state;
  ARCptr cur_arc;
  
  for (cur_state = NET_states(net);
       cur_state != NULL;
       cur_state = next_state(cur_state))
    for (cur_arc = STATE_arc_set(cur_state);
         cur_arc != NULL;
         cur_arc = next_arc(cur_arc))
      ARC_visit_mark(cur_arc) = mark;
}

void prepare_state_marks(NETptr net, int mark)
     /***********
      *
      * Iteratively sets the state marks to mark.
      *
      ***********/
{
  STATEptr cur_state;
  
  for (cur_state = NET_states(net);
       cur_state != NULL;
       cur_state = next_state(cur_state))
    STATE_visit_mark(cur_state) = mark;
}

void prepare_visit_marks(NETptr net, int mark)
     /****
      *
      * PREPAREVISITMARKS insures that the visitmarks of NET are all MARK
      * and that NET  is marked as having dirty marks, in preparation for 
      * algorithms that utilize  them.
      * 
      ****/
{
  if (NET_visit_marks_dirty(net) || mark != 0)
    prepare_state_marks(net, mark);
  
  NET_visit_marks_dirty(net) = (mark != 0);
}

void clear_client_cells(NETptr net)
     /***********
      *
      * Clears the client cell (Lisp:name-field) of all states in NET.
      *
      ***********/
{
  STATEptr cur_state;
  
  for (cur_state = NET_states(net);
       cur_state != NULL;
       cur_state = next_state(cur_state))
    STATE_client_cell(cur_state) = 0;
  
/*  NET_names_smashed(net) = TRUE; */
}

void clear_state_marks_and_client_cells(NETptr net)
     /***********
      *
      * Sets the state visit marks to 0 and client cells to NULL.
      *
      ***********/
{
  STATEptr cur_state;
  
  for (cur_state = NET_states(net);
       cur_state != NULL;
       cur_state = next_state(cur_state))
    {
      STATE_visit_mark(cur_state) = 0;
      STATE_client_cell(cur_state) = NULL;
    }
  NET_visit_marks_dirty(net) = FALSE;
/*  NET_names_smashed(net) = TRUE; */
}

void clear_all_visit_marks_and_state_client_cells(NETptr net)
     /***********
      *
      * Sets state and arc visit marks to NOT_VISITED (=0)
      * and state client cells to NULL.
      *
      ***********/
{
  STATEptr state;
  ARCptr arc;
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      STATE_client_cell(state) = NULL;
      STATE_visit_mark(state) = NOT_VISITED;
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
	ARC_visit_mark(arc) = NOT_VISITED;
    }
  NET_visit_marks_dirty(net) = FALSE;
}

void mark_reachable_states(STATEptr state, int mark, STACKptr stack)
     /*****
      *
      * Marks all states reachable from STATE with MARK.
      * Uses STACK if provided, if not, creates a stack.
      *
      *****/
{       
  ARCptr cur_arc = NULL;
  int stack_is_local = FALSE;
  
  if (stack == NULL)
    {
      stack = create_stack("States to reach");
      stack_is_local = TRUE;
    }
  
  for(;;)
    {
      if (STATE_visit_mark(state) != mark)
        {
          STATE_visit_mark(state) = mark;
          cur_arc = STATE_arc_set(state);
        }
      
      if (cur_arc == NULL)
        {
          if (STACK_depth(stack) > 0)
            cur_arc = pop(stack);
          else
            break;
        }               
      
      if (next_arc(cur_arc) != NULL)
        push(next_arc(cur_arc), stack);
      
      state = ARC_destination(cur_arc);
      cur_arc = NULL;
    }
  
  if (stack_is_local == TRUE)
    free_stack(stack);
}


NVptr remove_duplicates(NVptr nv)
     /*****
      *
      * Returns a net vector in which repeated instances of the
      * same net have been removed. Returns the input vector if
      * it contains no duplicates.
      *
      *****/
{
  int i, len = NV_len(nv), rm = 0;
  
  /* Clear client marks */
  
  for (i = 0; i < len; i++)
    NET_client_cell(NV_net(nv, i)) = (void *) 0;
  
  /* Count the number of duplicate occurrences
   */
  
  for (i = 0; i < len; i++)
    {
      if ((int) NET_client_cell(NV_net(nv, i)) == 1)
        rm++;
      else
        NET_client_cell(NV_net(nv, i)) = (void *) 1;
    }
  
  if (rm > 0)
    {
      /* There are some duplicates, make a new vector containing
         only one occurrence of each net.
         */
      NVptr new_nv = make_nv(len - rm);
      int j = 0;
      for  (i = 0; i < len; i++)
        {
          if ((int) NET_client_cell(NV_net(nv, i)) == 1)
            {
              NV_net(new_nv, j++) = NV_net(nv, i);
              NET_client_cell(NV_net(nv, i)) = (void *) 0;
            }
        }    
      return(new_nv);
    }
  else
    {
      /* No change, just clear the marks */
      for (i = 0; i < len; i++)
        NET_client_cell(NV_net(nv, i)) = NULL;   
      return(nv);
    }
}
#endif /* SLIM_FST */
/******************************************/
