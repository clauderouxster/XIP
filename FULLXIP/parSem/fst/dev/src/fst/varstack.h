/* $Id: varstack.h,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 1993-94 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  VARSTACK.H
 **
 **  Andr» Kempe, RXRC, April 1997
 **
 **  Functions for variable stacks:
 **
 **  - create and remove a stack
 **  - save, replace, remove a variable on the stack (access by name)
 **
 ******************************************************/

#ifndef VARSTACK_H
#define VARSTACK_H

#include "fst/types.h"

typedef struct RE2N_VAR_STACK_TYPE {
  NVptr stack;    /* variable nets */
  char **names;    /* variable names */
}
RE2N_VSTtype, *RE2N_VSTptr;

RE2N_VSTptr RE2N_create_var_stack ();
void RE2N_remove_var_stack (RE2N_VSTptr varstack, int nets_to_free);

int RE2N_get_var_number (RE2N_VSTptr varstack, char *name);
int RE2N_save_var (RE2N_VSTptr varstack, char *name, NETptr net);
int RE2N_remove_var (RE2N_VSTptr varstack, char *name);
NETptr RE2N_copy_var (RE2N_VSTptr varstack, char *name);
ALPHABETptr get_var_labels (RE2N_VSTptr varstack, char *name);

#endif /* VARSTACK_H */

