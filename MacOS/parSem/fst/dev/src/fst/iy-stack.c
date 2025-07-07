/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: iy-stack.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
/* #include <sys/ddi.h> */

#if (defined(MAC_OS) || defined(__MWERKS__))
#include <cstring>
#include <unix.h>		
char *	strdup(const char *str);	/* Only needed if prototypes are obligatory */
#endif /* MAC_OS */

#include "fst/fsm-package.h"
#include "fst/comm-def.h"
#include "fst/interface.h"
#include "fst/iy-stack.h"
#include "fst/define.h"
#include "fst/error.h"
#include "fst/fsm_io.h"
#include "fst/twol-net.h"
#include "fst/warning.h"
#include "fst/fsm-version.h"
#include "fst/eqv-labs.h"
#include "fst/virtual-fsm.h"
#include "fst/cfsm.h"
#include "fst/utf8_fatstr_util.h"

#define MAX_STACK	30

static struct 
{
  NVptr stack;
  char *name;
  int system_stack;
} all_stacks[MAX_STACK+1] = { { NULL, "Standard", 1 },
			      { NULL, "Lexicon", 1 },
			      { NULL, "Tokens", 1 },
			      { NULL, "Rule", 1 },
			      { NULL, "ParallelRules", 1 },
			      { NULL, "HeuristicalRules", 1 },
			      { NULL, "Sentence", 1 },
			      { NULL, NULL, 0 },
			      { NULL, NULL, 0 },
			      { NULL, NULL, 0 },
			      { NULL, NULL, 0 },
			      { NULL, NULL, 0 },
			      { NULL, NULL, 0 }};

static int current_index_stack=0;
static NVptr *current_stack = &all_stacks[0].stack;

/****************************************************************
  STACK access
 ****************************************************************/
static int loc_iy_net_to_stack(NETptr net)
{
  if (net)
    {
      if ((*current_stack) == NULL)
	(*current_stack) = net2nv(net);
      else
	nv_push(net, (*current_stack));
      return 1;
    }

  return 0;
}
void iy_net_to_stack(NETptr net)
{
  if (loc_iy_net_to_stack(net) && IY_VERBOSE) print_net_size(net, stdout);
}

void iy_nv_to_stack(NVptr nv)
{
  /* The nets in NV get pushed onto the stack maintaining their order. */

  int i;
  int ok = 1;

  for (i = NV_len(nv) -1 ; i >= 0 && ok; i--)
    ok = loc_iy_net_to_stack(NV_net(nv, i));

  if (IY_VERBOSE && ok)
    iy_print_stack(stdout, current_index_stack);

  free_nv_only(nv);
}

NETptr iy_net_from_a_stack(int stack, int net)
{
  if ((stack < MAX_STACK) && all_stacks[stack].stack &&
      (net < NV_len(all_stacks[stack].stack)))
    return NV_net(all_stacks[stack].stack,net);
  else
    return NULL;
}

NVptr iy_get_stack()
{
  NVptr s;
  s = (*current_stack);
  (*current_stack) = NULL;
  return s;
}

NVptr iy_stack(int n)
{
  return all_stacks[n].stack;
}

void iy_set_stack(int n, NVptr newstack)
{
  if ( ! (all_stacks[n].stack))
    all_stacks[n].stack = newstack;
  else
    fsm_fprintf (stderr, "compiler error: in iy_set_stack()\n");
}


NVptr * iy_stackptr(int n)
{
  return & (all_stacks[n].stack) ;
}


int iy_stack_length()
{
  return ((*current_stack) ? NV_len(*current_stack) : 0);
}

static int compnames(char *stackname, char *name)
{
  if (name)
    {
      while (*name)
	{
	  if (!*stackname || (tolower(*stackname) != tolower(*name))) return 0;
	  name++; stackname++;
	}
      return 1;
    }
  else return 0;
}

char *iy_current_stack_name()
{
  return all_stacks[current_index_stack].name;
}

unsigned int iy_find_stack(char *name)
{
  int i;

  for (i=0; (i<MAX_STACK) && all_stacks[i].name; i++)
    if (compnames(all_stacks[i].name,name)) return i;
  for (i=USER_STACKS; i<MAX_STACK; i++)
    if (!all_stacks[i].stack)
      {
	if (all_stacks[i].name) free(all_stacks[i].name);
	all_stacks[i].name = (char *) strdup(name);
	return i;
      }
  return MAX_STACK;
}
/****************************************************************
  Print A Prompt
 ****************************************************************/
void print_stack_prompt(char *mode)
{
  printf("%s[%d]: ",mode,iy_stack_length());
}
/****************************************************************
  inter STACK access
 ****************************************************************/
int iy_change_stack(unsigned int i, FILE * ofile)
{
  if (i < MAX_STACK)
    {
      current_index_stack = i;
      current_stack = &all_stacks[i].stack;
      if (IY_VERBOSE)
	{
	  fputs("Current stack: ",ofile);
	  if (all_stacks[i].name)
	    {
	      fputs(all_stacks[i].name,ofile);
	    }
	  else
	    {
	      fsm_fprintf(ofile,"%d",i);
	    }
	  fputs("\n",ofile);
	}
      return 1;
    }
  else
    {
      fsm_fprintf(ofile,"Stacks are numbered 0 -- %d\n",MAX_STACK-1);
      return 0;
    }
}
/****************************************************************
  STACK check (OLD ROUTINES)
 ****************************************************************/
int iy_check_args(int n, char* fn)
     /*****
      *
      *  Check to see if there are at least N nets
      *  on the stack (*current_stack).  If so, return 1.  If
      *  not, print a standard error message and return 0. 
      *
      *****/
{
  int stack_depth;
  
  if ((*current_stack) == NULL)
    stack_depth = 0;
  else
    stack_depth = NV_len((*current_stack));
  
  if (n > stack_depth)
    {
      if (n == 1)
      	fsm_fprintf(stderr,"'%s' requires a net.\n", fn);
      else
      	fsm_fprintf(stderr,"'%s' requires %d nets.\n", fn, n);
      return (0);
    }
  else return(1);
}

int iy_can_do_on(int type, char *fn, int lim)
{
  /* Check up to LIM arguments on the stack.  Return TRUE if one of
     them is of type TYPE.  If LIM is a negative number, all
     nets on the stack are checked.
     */
  
  int i;
  NETptr net;

  for (i = 0; i < NV_len((*current_stack)) && i != lim; i++)
    {
      net = NV_net((*current_stack), i);

    if ((NET_compacted(net) && type != COMPACTED_NET) ||
	(NET_is_virtual(net) && type != VIRTUAL_NET))
      {
        fsm_fprintf(stderr, "Cannot apply '%s' to ", fn);

	if (NET_is_virtual(net))
	  fputs("a virtual net.\n", stderr);
        else if (type == COMPACTED_NET)
          {
            fputs("a standard net.\n", stderr);
            if (strcmp(fn, "uncompact"))
              fputs("Please use 'compact' first.\n", stderr);
          }
        else if (type == STANDARD_NET)
          {
            fputs("a compacted net.\n", stderr);
            if (strcmp(fn, "compact"))
              fputs("Please use 'uncompact' first.\n", stderr);
          }
        else
          fputs("What type is %d?", stderr);
        return(0);
      }
	}
  return(1);
}    

/****************************************************************
  COMMAND: print stack,  print eqv-labels
 ****************************************************************/
/*
#ifdef MAC_OS 
unsigned char *CONSOLE_WINDOW_TITLE = "\pXerox Finite-State Calculus";
extern int CONSOLE_WINDOW_ACTUAL_COLUMNS ;
#else
*/
int CONSOLE_WINDOW_ACTUAL_COLUMNS = 80;
/*
#endif
*/  
int right_margin(void)
{
   if (CONSOLE_WINDOW_ACTUAL_COLUMNS > 20)
     return(CONSOLE_WINDOW_ACTUAL_COLUMNS - 2);
   else
     return(CONSOLE_WINDOW_ACTUAL_COLUMNS - 2);
}

void iy_print_eqv_labels (FILE *file)
{
  ALPH_VECTORptr classes ;
  NETptr net;

  if (current_stack && NV_len(*current_stack) > 0)
    {
      net = NV_net(*current_stack, 0);

      if (NET_is_virtual(net))
	{
	  not_applicable_to_virtual_net(IY_ASSERT && IY_QUIT_ON_FAIL);
	  return;
	}
      if (has_arc_vectors_p(net))
	{
	  fsm_fprintf(stdout, "Unvectorizing...\n");
	  unvectorize_net(net);
	}
      classes = net_eqv_classes(net);
      print_alph_vector_s(classes, right_margin(), 0, file);
      fsm_fprintf(file, "%d classes\n", av_len(classes));
      free_alph_vector(classes);
    }
  else 
    {
      printf ("Stack is empty.\n");
      quit_on_fail_error(IY_QUIT_ON_FAIL);
    }
  return;
}

void iy_print_stacks(FILE *f)
{
  int i, n=0;
  for (i=0; i<MAX_STACK; i++)
    {
      if (all_stacks[i].stack)
	{
	  fsm_fprintf(f,"%-10s contains %3d net(s)\n",
		  all_stacks[i].name, NV_len(all_stacks[i].stack));
	  n++;
	}
    }
  if (!n) fputs("no stacks in use.\n",f);
}

void iy_print_stack(FILE *f, int stack)
{
  int i, len, old;
  NETptr net;
  old = current_index_stack;
  if ((stack >= 0) && (stack != old))
    if (!iy_change_stack(stack, f))
      return; /* given stack not found */
  if ((*current_stack) == NULL)
    {
      fputs("Stack is empty.\n",f);
    }
  else
    {
      len = NV_len((*current_stack));
      for (i = 0; i < len; i++)
        {
          net = NV_net((*current_stack), i);
          fsm_fprintf(f,"%d: ", i);
          print_net_size(net, f);
        }
    }
  /* No need to change if the stack to be printed is the current
     stack. 97-06-20. LK */

  if (stack != old)
  {
      iy_change_stack(old,f);
  }
}

/****************************************************************
  COMMAND: stack read and stack save
 ****************************************************************/
void iy_stack_save(char *name, NVptr nets)
{
    FILE *f;
    
  
    if ((nets = (nets ? nets : (*current_stack))))
    {
    	/* On Macintosh, change file type temporarily to 'XFSN' for binary files. */
#ifdef MAC_OS
		extern long int _ftype, _fcreator;
		int file_type = _ftype;
		int file_creator = _fcreator;
		_fcreator = 'XFSC';
		_ftype = 'XFSN';
#endif /* MAC_OS */

        if ((f=fopen(name,"wb")))
        {
	  if (IY_VERBOSE)
	    fsm_fprintf(stdout, "Opening '%s'\n", name);

	  set_header_file_name(get_last_header(), name);

	  if (IY_MARK_VERSION)
	    {
	       if (nv_mark_version(nets, "XFST", FSM_VERSION_STRING))
		 {
		   quit_on_fail_error(IY_QUIT_ON_FAIL);
		 }
	    }

	  /* If license type is set to 0 (== LICENSE_TYPE_NONE),
	     we save in the old format that can be processed by
	     pre 8.0.0 (x) fst applications. */

	  real_networks_to_file(nets, f, NV_len(nets), IY_LICENSE_TYPE == 0);

	  if (IY_VERBOSE)
	    fsm_fprintf(stdout, "Closing '%s'\n", name);

	  fclose(f);
        }
        else
        {
            perror(name);
	    quit_on_fail_error(IY_QUIT_ON_FAIL);
        }
#ifdef MAC_OS
			_ftype = file_type;
			_fcreator = file_creator;
#endif /* MAC_OS */
    }
    else
    {
        if (IY_VERBOSE)
            fsm_fprintf(stderr, "No nets to save.\n");
	quit_on_fail_error(IY_QUIT_ON_FAIL);
    }
  
}

void iy_stack_write(void write_fn(NVptr nv, FILE *stream, int sort_states_and_arcs), FILE *stream, int sort_states_and_arcs)
{
  NVptr nv;

  if ((nv = (*current_stack)))
    write_fn(nv, stream, sort_states_and_arcs);
  else
    {
      if (IY_VERBOSE)
	fsm_fprintf(stderr, "No nets to write.\n");

      quit_on_fail_error(IY_QUIT_ON_FAIL);
    }
}

static NVptr nets_to_stack(NVptr nets, int method)
{
  if (nets)
    {
      if (method == USE_CURRENT_STACK)
	{
	  if ((*current_stack) == NULL)
	    (*current_stack) = nets;
	  else
	    (*current_stack) = nv_append(nets, (*current_stack));
	  return NULL;
	}
      else
	return nets;
    }
  else
    {
      quit_on_fail_error(IY_QUIT_ON_FAIL);
      return NULL;
    }
}
      
NVptr iy_stack_load(char *name, int method)
{
  int type;
  NVptr nv;

  if (get_file_type(name, &type) == NO_ERROR)
    {
      if (IY_VERBOSE)
	fsm_fprintf(stdout, "Opening '%s'\n", name);

      if ((nv = read_nets(name)))
	{
	  if (IY_VERBOSE)
	    fsm_fprintf(stdout, "Closing '%s'\n", name);
	}
      return(nets_to_stack(nv, method));
    }
  else
    {
      quit_on_fail_error(IY_QUIT_ON_FAIL);
      return(NULL);
    }
}

NVptr iy_stack_load_from_stream(FILE *stream, int method)
{
	return(nets_to_stack(standard_nets_from_stream(stream), method));
}

/****************************************************************
  COMMAND: stack pop, stack drop, stack swap, stack totop
 ****************************************************************/
static int check_net(int n)
{
  if ((*current_stack) == NULL)
    {
      if (IY_VERBOSE)
	fsm_fprintf(stderr, "Stack is empty.\n", n);

      return(0);
    }
  else if (NV_len((*current_stack)) <= n)
    {
      if (n == 0)
	{
	  if (IY_VERBOSE)
	    fsm_fprintf(stderr, "*** Programmer alert: Stack is an empty \
net vector.\n",  n);

	  /* Clear the error to prevent a possible infinite loop. */

	  free_nv_only(*current_stack);
	  *current_stack = NULL;
	}
      else  if (IY_VERBOSE)
	fsm_fprintf(stderr, "Can't access net %d. Stack has %d nets.\n",
		    n, NV_len(*current_stack));
      return(0);
    }
  else return(1);
}

static NETptr iy_stack_get_item(int n)
{
  NETptr net;
  if (check_net(n))
    {
      net = nv_extract((*current_stack), n);
      if (NV_len((*current_stack)) == 0)
        {
          free_nv_only((*current_stack));
          (*current_stack) = NULL;
        }
      return(net);
    }

  quit_on_fail_error(IY_QUIT_ON_FAIL);

 return(NULL);
}

void iy_stack_drop(int n)
{
  NETptr net;
  net=iy_stack_get_item(n);
  if (net) free_network(net);
}

NETptr iy_stack_pop(void)
{
  return iy_stack_get_item(0);
}

NETptr iy_top_stack(int n)
{
  if (check_net(n)) return nv_get((*current_stack), n);
  else return(NULL);
}

void iy_stack_totop(int n)
{
  loc_iy_net_to_stack(iy_stack_get_item(n));
}

void iy_stack_swap(int n, int i)
{
  NETptr net;
  if (check_net(max(i,n)))
    {
      net = NV_net((*current_stack), n);
      NV_net((*current_stack), n) = NV_net((*current_stack), i);
      NV_net((*current_stack), i) = net;
    }
}
void iy_stack_dup(int n)
{
  if (check_net(n)) iy_net_to_stack(copy_fsm(NV_net((*current_stack), n)));
}


NETptr iy_stack_push_defined(char *sym)
{
  id_type id;
  NETptr net;

  FST_intern_literal(&id, sym);
  if ( (net = get_def_uncopied(id)) )
    {

      /* Have to check if the net is virtual or not to copy it properly */
        
#ifndef WORLD
      if (NET_is_virtual(net))
          net = virtual_copy_fsm(net, DONT_RECLAIM);
      else
#endif
        net = copy_fsm(net);

      loc_iy_net_to_stack(net);
    }
  else
  {
      fsm_fprintf(stderr, "'%s' is not a defined symbol\n", sym);
      quit_on_fail_error(IY_QUIT_ON_FAIL);
  }
  return(net);
}

/****************************************************************
  COMMAND: stack clear
 ****************************************************************/
void iy_stack_clear()
{
  while ((*current_stack))
    free_network(iy_stack_pop());
}

/****************************************************************
  COMMAND: stack reverse, rotate
 ****************************************************************/
void iy_stack_reverse(void)
{
  int i, n, last;
  if ((*current_stack) == NULL)
    {
      if (IY_VERBOSE) printf("Can't. Stack is empty.\n");
    }
  else
    {
      n = NV_len((*current_stack)) / 2;
      last = NV_len((*current_stack)) - 1;
      for (i = 0; i < n; i++)
        {
          NETptr net = nv_get((*current_stack), i);
          NV_net((*current_stack), i) = nv_get((*current_stack), last - i);
          NV_net((*current_stack), last - i) = net;
        }
    }
}

void iy_stack_rotate(void)
{
  int i;
  NETptr temp;
  if ((*current_stack) == NULL)
    {
      if (IY_VERBOSE) printf("Can't. Stack is empty.\n");
    }
  else
    {
      temp = NV_net((*current_stack), 0);
      for (i = 0; i < NV_len((*current_stack)) - 1; i++)
        {
          NV_net((*current_stack), i) = NV_net((*current_stack), i+1);
        }
      NV_net((*current_stack), i) = temp;
    }
}
/****************************************************************
  END.
 ****************************************************************/
