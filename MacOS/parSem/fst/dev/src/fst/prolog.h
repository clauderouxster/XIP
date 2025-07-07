/* $Id: prolog.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1992 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  PROLOG.H
 **
 **  Lauri Karttunen, Xerox PARC, October 1992
 **
 **  Functions for converting finite-state networks
 **  to Prolog clauses.
 **  
 **
 ******************************************************/
 
#ifndef C_FSM_PROLOG
#define C_FSM_PROLOG

#include "fst/fst-types.h"

#ifdef __cplusplus
extern "C" {
#endif

void FstCalcExport networks_to_prolog_file(NVptr nets, FILE *stream, int sort_states_and_arcs);
void FstCalcExport network_to_prolog_file(NETptr net, FILE *stream, int sort_states_and_arcs);

NVptr FstCalcExport networks_from_prolog_file(FILE *stream);

extern int FstCalcExport PROLOG_PARSE_ERROR;

NETptr FstCalcExport pl_net(char *name);
void FstCalcExport pl_start(NETptr net, int state_id);
void FstCalcExport pl_arc(NETptr net, int orig_id, int dest_id, id_type label_id);
void FstCalcExport pl_final(NETptr net, int state_id);
void FstCalcExport pl_symbol(NETptr net, int symbol_id);

extern finfo FstCalcExport *prolog_file_info;

/******************************************************
  Example:
  
  Operation (cmd-Q = quit): regex		[Construct a network from a
  Input file [stdin]:				 regular expression.]
  Regular expression (;<CR> terminates):
  [a:x 0:y (b:z)]*;
  3 states, 4 arcs. Circular.
  Stack: 1 net

  Operation (cmd-Q = quit): lookdown ab		[Check it out.]
  xyz

  Operation (cmd-Q = quit): save-as-prolog	[Write it to the screen as
  Output file: -				 Prolog clauses.]

    network(net_bbf68).
    arc(net_bbf68, 0, 1, "a":"x").
    arc(net_bbf68, 1, 2, "0":"y").
    arc(net_bbf68, 2, 0, "b":"z").
    arc(net_bbf68, 2, 1, "a":"x").
    final(net_bbf68, 0).
    final(net_bbf68, 2).
	
 ******************************************************/
#ifdef __cplusplus
}
#endif

#endif /* C_FSM_PROLOG */
