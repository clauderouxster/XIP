/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: iy-define.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

#include <stdlib.h>
#include <string.h>

#include "fst/fsm-package.h"
#include "fst/comm-def.h"
#include "fst/interface.h"
#include "fst/interface.lex.h"
#include "fst/functions.h"
#include "fst/iy-define.h"
#include "fst/define.h"
#include "fst/re.lex.h"
#include "fst/warning.h"
#include "fst/context.h"
#include "fst/fsm-fst-x.h"
#include "fst/fsm_io.h"
#include "fst/types.h"
#include "fst/cfsm.h"
#include "fst/utf8_fatstr_util.h"

#if (defined(MAC_OS))
#include <cstring>
char *	strdup(const char *str);
#endif

int too_many_aliases = 0;
/****************************************************************
  COMMAND: define, undefine, print defined
 ****************************************************************/

void switch_verbose(int restore_p)
{
  /* A non-restore call turns IY_VERBOSE off, and saves the previous value.
    a restore call restores the previous value. Used to prevent
     the size of the network being printed too early by iy_net_to_stack().
     In some cases of 'define XX regex2' it may change in iy_define().
     */

  static int hold;
  
  if (restore_p)
    *fst_IY_VERBOSE() = hold;
  else
    {
      hold = *fst_IY_VERBOSE();
      *fst_IY_VERBOSE() = 0;
    }
}

void iy_define(char *atom, NETptr net, int print_size_p)
{
  id_type id;
  NETptr old_def = NULL;

  if (net)
    {
      if (FST_intern_literal(&id, atom) != NO_ERROR)
	quit_on_fail_error(IY_QUIT_ON_FAIL);

      old_def = get_def_uncopied(id);

      define_label(id, net, *fst_IY_RECURSIVE_DEFINE());
      if (print_size_p && *fst_IY_VERBOSE())
	{
	  if (old_def)
	    fprintf(stdout, "redefined %s: ", atom);
	  else
	    fprintf(stdout, "defined %s: ", atom);
	  print_net_size(net, stdout);
	}
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}

void iy_undefine(char *atom)
{
  id_type id;
  FST_intern_literal(&id, atom);
  if (undefine_label(id))
    {
      fprintf(stdout, "%s has not been defined\n", atom);
      quit_on_fail_error(IY_QUIT_ON_FAIL);
    }
}

void iy_undef_all()
{
  int32 i;
  for (i=LIM_id_count(LABEL_MAP); i>0; i--)
    {
      if (def_value_p(i)) undefine_label(i);
    }
}

void iy_print_defs(FILE *f)
{
  int i, max, b;
  LABELptr label;
  max=LIM_id_count(LABEL_MAP);
  for (b=i=1; i<max; i++)
    {
      if (def_value_p(i))
	{
	  if (b) b=0;
	  label = id_to_label(i);
	  FST_fprint_fat_string(f, LABEL_atom(label));
   	  fputs("\t",f);
	  print_net_size(get_def_uncopied(i), f);
	}
    }
  if (iy_list_funcs(f)) b=0;
  if (b) fputs("no definitions\n",f);
}

NVptr iy_defs_to_stack()
{
  int i, max;
  NETptr net;
  NVptr nets = NULL;
  max=LIM_id_count(LABEL_MAP);
  for (i=1; i<max; i++)
    {
      if (def_value_p(i))
	{
	  net = reset_label(i);
	  if (net)
	    {
	      add_string_prop(net, NETWORKNAME_ATTR, 
			      LABEL_atom(id_to_label(i)));

	      add_character_prop(net, TYPE_ATTR, 'D');
	      if (!nets) nets = net2nv(net);
	      else nv_push(net,nets);
	    }
	}
    }
  return nets;
}

int iy_stack_to_defs(NVptr nets)
{
  int i, n=0;
  id_type id;

  OBJECTptr str;
  if (nets)
    {
      for (i=0; i<NV_len(nets); i++)
	{
	  if ((str=get_value(NV_net(nets,i), NETWORKNAME_ATTR)) &&
	      (OBJ_type(str) == String))
	    {
	      intern_atomic_label(&id, OBJ_str(str));

	      define_label(id, NV_net(nets,i), *fst_IY_RECURSIVE_DEFINE());
	      n++;
	    }
	  else
	    {
	      free_network(NV_net(nets,i));
	    }
	}
      free_nv_only(nets);
    }
  return n;
}

/****************************************************************
  COMMAND: alias
 ****************************************************************/
static HASH_TABLEptr hal;
static FILE *alias_f;
static int hal_pr=0;

static void init_aliases()
{
  hal=make_hash_table(123,"Aliases");
}

static unsigned int al_hashfn(unsigned char *str)
{
  int i=0;
  while (*str) i=13*i + *str++;
  return i;
}

static int alias_pr(char *el, char *val)
{
  hal_pr++;
  return fsm_fprintf(alias_f, "alias %s\t%s\n", el, val);
}

void iy_list_aliases(FILE *f)
{
  hal_pr=0;
  alias_f = f;
  if (hal) map_hash(hal, (int (*)(void *, void *)) alias_pr); 
  if (!hal || !hal_pr) fputs("No aliases.\n",f);
}

char *iy_find_alias(char *a)
{
  void *p;    
  if (hal && !get_hash(hal,a,&p, (uint32 (*)(void *)) al_hashfn, (int (*)(void *, void *)) strcmp))
    return p;
  else
    return NULL;
}

void iy_save_alias(char *a, char *str)
{
  static char prompt[] = "alias> ";
  char *to, *p;
  char buf[30000]; /* An arbitary limit */
  if (!str)
    {
      print_a_prompt(prompt);
      if ((p = lookup_line()))
	{
	  to = buf;
	  do
	    {
	      strcpy(to, p);
	      to += strlen(to);
	      *to++ = '\n';
	      print_a_prompt(prompt);
	    } while ((p = lookup_line()));
	  to[-1] = ' ';
	  *to = '\0';
	  str = buf;
	}
      else if (hal && (p = iy_find_alias(a)))
	{
	  free(p);
	  del_hash(hal,a, (uint32 (*)(void *)) al_hashfn, (int (*)(void *, void *)) strcmp); /* Note: everything is not freed */  
	  return;
	}
    }
  if (!hal) init_aliases();
  if ((p = iy_find_alias(a)))
    {
      free(p);
      put_hash(hal, a, strdup(str), (uint32 (*)(void *)) al_hashfn, (int (*)(void *, void *)) strcmp);
    }
  else
    if (str && a)
      put_hash(hal, strdup(a), strdup(str), (uint32 (*)(void *)) al_hashfn, (int (*)(void *, void *)) strcmp);
}

int iy_alias_pars(char *p)
{
  int n=0;
  while ((p=strchr(p,'$')))
    {
      ++p;
      if ((*p > '0') && (*p <= '9') && ((*p - '0') > n))
	n = *p - '0';
    }
  return n;
}

void iy_execute_alias(char *alias, char *pars[], int n, int len)
{
  static int level=0;
  char *to, *from, *tmp;
  int i;
  FST_CNTXTptr context;
#ifdef __GNUC__
  char buf[strlen(alias)+strlen(pars[0])+len*n+1024]; /* A risk of overflow */
#else
  char *buf = (char *)malloc(sizeof(buf[0]) * (strlen(alias)+strlen(pars[0])+len*n+1024));
#endif
  if (level > 20)
    {
      deep_alias();
      for (i=0; i<=n; i++)
	if (pars[i]) free(pars[i]); /* ALL the pars[] will be freed. */
      too_many_aliases = 1; /* This has an effect in execute_commands */
      wait_for_END();
    }
  else
    {
      for (to=buf, from=alias; *from; from++, to++)
	{
	  if ((*from == '$') && (from[1] > '0') && (from[1] <= '9'))
	    {
	      /* Arguments of an alias */
	      ++from;
	      tmp = pars[*from - '0'];
	      if (tmp)
		{
		  strcpy(to,tmp);
		  to += strlen(to)-1;
		}
	      else --to;
	    }
	  else
	    *to = *from;
	}
      strcpy(to, pars[0]);
      strcat(to, "\n");
      for (i=0; i<=n; i++)
	if (pars[i]) free(pars[i]); /* ALL the pars[] will be freed. */
      level++;
      context = get_default_context();
      execute_string(buf, context);
      if (!--level) too_many_aliases = 0;
    }
#ifndef __GNUC__
  free(buf);
#endif
}
/****************************************************************
  COMMAND: alias in regex
 ****************************************************************/


void iy_list(id_type id, ALPHABETptr sym_list)
{
  free_alph(LABEL_data(id_to_label(id)));
  LABEL_data(id_to_label(id)) = (void *) sym_list;
}

void iy_unlist(id_type id)
{
  int i, lim = global_alph_size();

  if (id == 0)
    for (i = 0; i < lim; i++)
      {
	free_alph(LABEL_data(id_to_label(i)));
	LABEL_data(id_to_label(i)) = NULL;
      }
  else
    {
      free_alph(LABEL_data(id_to_label(id)));
      LABEL_data(id_to_label(id)) = NULL;
    }
}

void iy_print_lists(FILE *f)
{
  int i, max, b;
  LABELptr label;
  ALPHABETptr alph;
  max=global_alph_size();
  for (b=i=1; i<max; i++)
    {
      if (LABEL_data(id_to_label(i)))
	{
	  if (b) b=0;
	  label = id_to_label(i);
	  FST_fprint_fat_string(f, LABEL_atom(label));

	  fputs("\t",f);
	  reset_out_buffer();
	  alph = LABEL_data(id_to_label(i));
	  alph_out_s(alph, 72, 8, f);
	  fputs("\n", f);
	}
    }
  if (b) fputs("no list symbols\n",f);
}


