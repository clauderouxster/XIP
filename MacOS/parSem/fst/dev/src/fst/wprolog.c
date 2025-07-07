/* $Id: wprolog.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 200 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "fst/struct.h"
#include "fst/wprolog.h"
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
 **  Tamas Gaal, Xerox Research Centre Europe, October 2001
 **
 **  Input/output routines for representing finite-state
 **  networks as weighted Prolog clauses.
 **  
 **
 ******************************************************/

#define state_name(X)		(long int) STATE_client_cell((X))

extern FILE *wprolog_in;
extern int wprolog_parse(void);

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


#if NOT_USED
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
#endif /* NOT_USED */

void network_to_weighted_prolog_file(NETptr net, FILE *stream, int sort_states_and_arcs)
{
  /* Converts a network to a set of weighted Prolog clauses.
   */
 fsm_fprintf(stream, "Not yet implemented: wprolog.c:network_to_weighted_prolog_file().\n") ;
  
}


void networks_to_weighted_prolog_file(NVptr nv, FILE *stream, int sort_states_and_arcs)
{
  /* Writes the networks in the network array NV as weighted Prolog clauses. */
  
  int i;
  
  for (i = 0; i < NV_len(nv); i++)
    {
      if (i > 0)
      fsm_fprintf(stream, ").\n") ;
      network_to_weighted_prolog_file(NV_net(nv, i), stream, sort_states_and_arcs);
    }
}

/****************************************************** 
 **
 **  Functions for constructing networks from
 **   Weighted Prolog clauses.
 **  
 **
******************************************************/

#if NOT_USED
static char *net_name(NETptr net)
{
 return((char *) NET_client_cell(net));
}
#endif /* NOT_USED */

NVptr networks_from_weighted_prolog_file(FILE *stream)
{
  fsm_fprintf(stream, "Not yet implemented: wprolog.c:networks_from_weighted_prolog_file().\n") ;
  wprolog_parse();
  return (NVptr) 0;
}
