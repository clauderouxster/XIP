/* $Id: varstack.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 1993-94 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  VARSTACK.C
 **
 **  Andr» Kempe, RXRC, April 1997
 **
 **  Functions for variable stacks:
 **
 **  - create and remove a stack 
 **  - save, replace, remove a variable on the stack (access by name)
 **
 ******************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "fst/types.h"
#include "fst/struct.h"
#include "fst/copy-fsm.h"
#include "fst/varstack.h"
#include "fst/fsm_io.h"

/*******************
 * Functions to create and remove a stack
 */

RE2N_VSTptr RE2N_create_var_stack ()
{
  /* Creates an empty variable stack, for networks from regex strings. */

  RE2N_VSTptr vst;

  if (! (vst = (RE2N_VSTptr) malloc (sizeof (RE2N_VSTtype))))
    not_enough_memory ("RE2N_create_var_stack");

  vst->stack = make_nv (0);
  vst->names = NULL;

  return (vst);
}

void RE2N_remove_var_stack (RE2N_VSTptr varstack, int nets_to_free)
{
  /* Removes a variable stack, for networks from regex strings. */

  int i, num = NV_len (varstack->stack);

  if (nets_to_free)
    free_nv_and_nets (varstack->stack);
  else
    free_nv_only(varstack->stack);

  for (i=0; i< num; i++)
    free (varstack->names [i]);

  free (varstack->names);
  free (varstack);
}

/*******************
 * Functions to save, replace, remove a variable on the stack
 * (access by name)
 */

int RE2N_get_var_number (RE2N_VSTptr varstack, char *name)
{
  /* Returns the number of the variable_net with name[] in varstack[].
     Returns -1 if not found. */

  int i, num = NV_len (varstack->stack);

  for (i=0; i< num; i++)
    if (! strcmp (varstack->names [i], name))   /* variable found */
      break;

  if (i >= num)     /* variable not found */
    i = -1;
  
  return (i);
}

int RE2N_save_var (RE2N_VSTptr varstack, char *name, NETptr net)
{
  /* Adds a variable (name and net) to the stack if not already there. */

  int newlen = NV_len (varstack->stack) + 1;

  if (RE2N_get_var_number (varstack, name) >= 0)  /* var already on stack */
    {
      fsm_fprintf (stderr, "\n\
**** PROGRAMMING ERROR ****   Please, inform the programmer.\n\
Attempt to save variable '%s' twice to the stack in 'RE2N_save_var()'\n\n",
	       name);
      return (FALSE);
    }

  nv_add (net, varstack->stack);  /* add net to the end of the stack */

  /* See comment below about the following if-else clause. */

  if (!(varstack->names))
    {
      if (!(varstack->names = (char **) malloc (newlen * sizeof (char *))))
	not_enough_memory ("RE2N_save_var");
    }
  else
    {
      if (!(varstack->names =
	    (char **) realloc (varstack->names, newlen * sizeof (char *))))
	not_enough_memory ("RE2N_save_var");
    }

  /* Note: The code above should be equivalent to the commented-out
     piece below. But the latter does not work on SunOS. It appears
     that if the first argument of realloc is a NULL pointer, 
     realloc returns NULL instead of allocating the requested memory.
     The fix above gets around this problem by calling malloc instead
     of realloc at the point where varstack->name is still NULL.
     Sept 14, 1997. LK
     */

  /*    
  if (! (varstack->names =
	 (char **) realloc (varstack->names, newlen * sizeof (char *))))
    not_enough_memory ("RE2N_save_var");
    */

  if (! (varstack->names [newlen -1] =
	 (char *) malloc ((strlen (name) +1) * sizeof (char))))
    not_enough_memory ("RE2N_save_var");

  strcpy (varstack->names [newlen -1], name);
  return (TRUE);
}

int RE2N_remove_var (RE2N_VSTptr varstack, char *name)
{
  /* Removes a variable (name and net) from the stack if there. */

  int i, pos, num;

  if ((pos = RE2N_get_var_number (varstack, name)) < 0) /* var not on stack */
    return (FALSE);

  num = NV_len (varstack->stack);
  nv_extract (varstack->stack, pos);
  free (varstack->names [pos]);

  for (i= pos+1; i< num; i++)
    varstack->names [i-1] = varstack->names [i];

  if (! (varstack->names =
	 (char **) realloc (varstack->names, (num-1) * sizeof (char *))))
    not_enough_memory ("RE2N_save_var");

  return (TRUE);
}

NETptr RE2N_copy_var (RE2N_VSTptr varstack, char *name)
{
  /* Returns a copy of the variable network corresponding to name[]. */

  int pos;

  if ((pos = RE2N_get_var_number (varstack, name)) < 0) /* var not on stack */
    return (NULL);

  return (copy_fsm (NV_net (varstack->stack, pos)));
}

ALPHABETptr get_var_labels (RE2N_VSTptr varstack, char *name)
{
  /* Returns a pointer to the label alphabet of the variable network
     corresponding to name[]. */

  int pos;

  if ((pos = RE2N_get_var_number (varstack, name)) < 0) /* var not on stack */
    return (NULL);

  return (NET_labels(NV_net (varstack->stack, pos)));
}
