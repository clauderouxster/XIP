/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */

/*************************************************************
 *
 * FILE "vcg-io.h"
 *
 * Interface for visualizing networks with the VCG tool of
 * Saarbruecken University.  
 *
 * Tamas Gaal, XEROX RESEARCH CENTRE EUROPE, Grenoble, France
 * 1999-03 
 */

/* Prints a vcg/gdl representation of NET into FILE.  */
void FstCalcExport print_vcg_net(NETptr net, FILE *stream);

/* Prints a vcg/gdl representation of NET into FILE. If FILE is stdout,
   it prints to a temporary file, then opens an xvcg window directly,
   in a separate window, in parallel, and deletes the temporary file.
   Requires the xvcg program in the program PATH.
   */
#ifndef _WIN32
void FstCalcExport vcg_to_file_or_to_window(FILE *f, NETptr net, int interactive, int verbose);
#endif
