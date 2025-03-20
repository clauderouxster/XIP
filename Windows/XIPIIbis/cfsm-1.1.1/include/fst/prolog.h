/* $Id: prolog.h,v 1.5 2002/05/07 01:42:09 lk Exp $ */
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

#ifdef __cplusplus
extern "C" {
#endif

void networks_to_prolog_file(NVptr nets, FILE *stream, int sort_states_and_arcs);
void network_to_prolog_file(NETptr net, FILE *stream, int sort_states_and_arcs);

NVptr networks_from_prolog_file(FILE *stream);

extern int PROLOG_PARSE_ERROR;

NETptr pl_net(char *name);
void pl_start(NETptr net, int state_id);
void pl_arc(NETptr net, int orig_id, int dest_id, id_type label_id);
void pl_final(NETptr net, int state_id);
void pl_symbol(NETptr net, int symbol_id);

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
