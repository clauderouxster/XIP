/* $Id $ */
/* Copyright (c) 2001 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  WPROLOG.H
 **
 **  Tamas Gaal, Xerox Research Centre Europe, October 2001
 **
 **  Functions for converting finite-state networks
 **  to Weighted Prolog clauses.
 **  
 **
 ******************************************************/
 
#ifndef C_FSM_WPROLOG
#define C_FSM_WPROLOG

#ifdef __cplusplus
extern "C" {
#endif

void networks_to_weighted_prolog_file(NVptr nets, FILE *stream, int sort_states_and_arcs);
void network_to_weighted_prolog_file(NETptr net, FILE *stream, int sort_states_and_arcs);

NVptr networks_from_weighted_prolog_file(FILE *stream);

NETptr wpl_net(char *name);
void wpl_start(NETptr net, int state_id);
void wpl_arc(NETptr net, int orig_id, int dest_id, id_type label_id);
void wpl_final(NETptr net, int state_id);
void wpl_symbol(NETptr net, int symbol_id);

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

#endif /* C_FSM_WPROLOG */
