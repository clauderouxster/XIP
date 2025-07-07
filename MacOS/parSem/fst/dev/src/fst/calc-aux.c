/* $Id: calc-aux.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1997 by the Xerox Corporation.  All rights reserved */

/***********************************************************
 *
 *			    CALC-AUX.C
 *
 *	 Auxiliary function definitions (displaced from re-aux.c)
 *
 *
 ***********************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>

#include "fst/struct.h"
#include "fst/define.h"
#include "fst/pretty-p.h"
#include "fst/copy-fsm.h"
#include "fst/modify.h"
#include "fst/calculus.h"
#include "fst/header.h"
#include "fst/bin-in.h"
#include "fst/fsm_io.h"
#include "fst/fsm-fst-x.h"
#include "fst/calc-aux.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

NETptr *REGISTERS;
int REGISTER_LEN = 10;

void TEST_net_to_reg(NETptr net, int idx)
{
  if (net)  net = copy_fsm(net);

  REGISTERS [idx] = net;
}

void TEST_nv_to_regs(NVptr nv, int idx, int len)
{
  int ireg, inet;

  if (len > nv->len)  len = nv->len;
  if (len > 10-idx)  len = 10-idx;

  for (ireg=idx, inet=0; inet<len; ireg++, inet++)
    TEST_net_to_reg( NV_net(nv, inet), ireg );
}

int nat_number(char *str)
{
  int n = atoi(str);
  
  if (n <= 0)
    {
      handle_warning("Positive integer expected", "REGEX_PARSE", n);
      /* n = 0 - n;  
	 ?? 98-6-5 TG */
    }	
  return(n);
}

static int product_label_error(id_type label_id, id_type id, NETptr def)
{
  if (def && NET_arc_label_arity(def) > 1)
    {
      fsm_fprintf(stdout, "\n Cannot use the definition of '");
      print_label(id, stdout, TRUE);
      fsm_fprintf(stdout, "' in '");
      print_label(upper_id(label_id), stdout, TRUE);
      fsm_fprintf(stdout, ":");
      print_label(lower_id(label_id), stdout, TRUE);
      fsm_fprintf(stdout, "' because it denotes a relation.\n");
      return(TRUE);
    }
  else
    return(FALSE);
}

static int impossible_product(id_type label_id, id_type up_id, NETptr up_def,
			      id_type lo_id, NETptr lo_def)
{
  int up_error, lo_error;
  
  up_error = product_label_error(label_id, up_id, up_def);
  lo_error = product_label_error(label_id, lo_id, lo_def);

  return(up_error || lo_error);
}

NETptr exactly(NETptr net, int n)
{
  return(repeat_fsm(min_fsm(net,DONT_COPY), n, n,
		    DONT_COPY, DONT_MINIMIZE));
}

NETptr product(NETptr upper, NETptr lower)
{
  NETptr net;

  if (NET_arc_label_arity(upper) > 1 || NET_arc_label_arity(lower) > 1)
    {

      fsm_fprintf(stdout, "\n Cannot compile the product.\n");

      quit_on_fail_error(*fst_IY_QUIT_ON_FAIL());

      net = null_fsm();
    }
  else
    net = crossproduct_fst(upper, lower, DO_RECLAIM, DO_MINIMIZE);

  return(net);
}

static NETptr def_value2(id_type label_id)
{
  /* If LABEL_ID is a simple symbol that has a value, return it;
     if LABEL_ID and one or both components have a value, return
     the crossproduct.
     */
  
  NETptr net;

  if (tuple_id_p(label_id))
    {
      NETptr up_def = def_value(upper_id(label_id)),
	lo_def = def_value(lower_id(label_id));

      if (impossible_product(label_id, upper_id(label_id), up_def,
			     lower_id(label_id), lo_def))
	{
	  
	  quit_on_fail_error(*fst_IY_QUIT_ON_FAIL());
	  return(NULL);
	}

      if (up_def)
	{
	  if (lo_def)
	    net = product(up_def, lo_def);
	  else
	    net = product(up_def, symbol_fsm(lower_id(label_id)));
	}
      else if (lo_def)
	{
	  net = product(symbol_fsm(upper_id(label_id)), lo_def);
	}
      else
	net = NULL;
    }
  else
    net = def_value(label_id);

  return(net);
}

NETptr lab_exactly(id_type label_id, int n)
{
  NETptr net = def_value2(label_id);
  
  if (net == NULL)
    return(repeat_label(label_id, n, n));
  else
    return(exactly(net, n));
}

NETptr between(NETptr net, int m, int n)
{
  return(repeat_fsm(min_fsm(net, DONT_COPY), m, n,
		    DONT_COPY, DONT_MINIMIZE));
}

NETptr lab_between(id_type label_id, int m, int n)
{
  NETptr net = def_value2(label_id);
  
  if (net == NULL)
    return(repeat_label(label_id, m, n));
  else
    return(between(net, m, n));
}

NETptr less_than(NETptr net, int n)
{
  return(repeat_fsm(min_fsm(net, DONT_COPY), 0, n-1,
		    DONT_COPY, DONT_MINIMIZE));
}

NETptr lab_less_than(id_type label_id, int n)
{
  NETptr net = def_value2(label_id);
  
  if (net == NULL)
    return(repeat_label(label_id, 0, n-1));
  else
    return(less_than(net, n));
}


NETptr more_than(NETptr net, int n)
{
  return(repeat_fsm(min_fsm(net, DONT_COPY), n+1, -1,
		    DONT_COPY, DONT_MINIMIZE));
}

NETptr lab_more_than(id_type label_id, int n)
{
  NETptr net = def_value2(label_id);
  
  if (net == NULL)
    return(repeat_label(label_id, n+1, -1));
  else
    return(more_than(net, n));
}


NETptr concat_lab(NETptr net1, id_type id)
{
  NETptr net2 = def_value2(id);
  
  if (net2 == NULL)
    return(concat_label(net1, id, DONT_COPY, DONT_MINIMIZE));
  else
    return(concat(net1, net2));
}


NETptr kleene_plus_lab(id_type id)
{
  NETptr net = def_value2(id);
  
  if (net == NULL)
    return(kleene_plus_fsm(id));
  else
    return(kleene_plus(net));
}

NETptr kleene_star_lab(id_type id)
{
  NETptr net = def_value2(id);
  
  if (net == NULL)
    return(kleene_star_fsm(id));
  else
    return(kleene_star(net));
}

NETptr symbol_to_fsm(id_type label_id)
{
  /* If LABEL_ID has a definition, return it, otherwise make
     a symbol_fsm. If label_id refers to a symbol pair, check
     for a definition on both sides.
     */
  
  NETptr net;

  if (!(net = def_value(label_id)))
    net = symbol_fsm(label_id);

  return(net);
}

NETptr ignore_lab(NETptr net1, id_type id)
{
  NETptr net2 = def_value2(id);
  
  if (net2 == NULL)
    return(ignore_label(net1, id, DONT_COPY, DONT_MINIMIZE));
  else
    return(ignore(net1, net2));
}

NETptr label_to_net(id_type label_id)
{
  /* If LABEL_ID has a definition, return it, otherwise make
     a symbol_fsm. If label_id refers to a symbol pair, check
     for a definition on both sides.
     */
  
  NETptr net;

  if (!(net = def_value2(label_id)))
    net = symbol_fsm(label_id);

  return(net);
}

NETptr contains_lab(id_type id)
{
  NETptr net = def_value2(id);
  
  if (net == NULL)
    return(contains_label(id));
  else
    return(contains(net));
}
NETptr read_file(char *name)
{
  FILE *f;
  NVptr nv;
  
  f = fopen(name, "rb");
  if (f == NULL)
    {
      fsm_fprintf(stderr, "*** Cannot open %s\n", name);
      return(epsilon_fsm());
    }
  else
    {
      nv = networks_from_file(f);
      fclose(f);
      if (NV_len(nv) > 1)
	{
	  fsm_fprintf(stderr, "*** \"@\" operator returns first net only.\n");
	  fsm_fprintf(stderr, "Ignoring additional nets in \"%s\".\n", name);
	}
#ifndef FULL_FSM
      return(NV_net(nv, 0));
#else
      return(uncompact_net(NV_net(nv, 0)));
#endif
    }
}


NETptr read_or_take(char *str)
{
  char *ch;
  int num;
  NETptr net;
  for (ch = str; *ch != '\0'; ch++)
    if (!isdigit((int) *ch))
      return(read_file(str));
  
  num = atoi(str);
  if (num < REGISTER_LEN && REGISTERS[num] != NULL)
    {
      net = REGISTERS[num];
      REGISTERS[num] = NULL;
#ifndef FULL_FSM
	  return(net);
#else
      return(uncompact_net(net));
#endif
    }
  else
    {
      fsm_fprintf(stderr, "*** Nothing in REGISTER %d\n", num);
      return(null_fsm());
    }
}

#endif /* SLIM_FST */
