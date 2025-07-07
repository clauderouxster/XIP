/*  $Id: dot-net-io.h,v 1.4 2005/03/04 15:45:22 grondeau Exp $
Copyright (c) 2001 by the Xerox Corporation.  All rights reserved */

/*************************************************************
 *
 * FILE "vcg-io.h"
 *
 * Interface for visualizing networks with the dot tool of
 * AT&T
 *
 * Tamas Gaal, XEROX RESEARCH CENTRE EUROPE, Grenoble, France
 * 2001-12
 */

/* Prints a dot representation of NET into FILE.  */
void FstCalcExport print_dot_net(NETptr net, FILE *stream);

/* Prints a dot representation of NET into FILE. If FILE is stdout,
   it prints to a temporary file, then opens an xdot window directly,
   in a separate window, in parallel, and deletes the temporary file.
   Requires the dot program in the program PATH.
   */
#ifndef _WIN32
void FstCalcExport dot_to_file_or_to_window(FILE *f, NETptr net, int interactive, int verbose);
#endif
