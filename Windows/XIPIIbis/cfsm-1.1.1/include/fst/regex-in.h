/* $Id: regex-in.h,v 1.12 2002/06/11 15:29:07 jmcoursi Exp $ */
/* Copyright (c) 1990-93 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  REGEX-IN.H
 **
 **  André Kempe, RXRC, January 1997
 **
 **  Functions for generating networks from:
 **  - regular expression files
 **  - text files (lists of words)
 **  - spaced text files (lists of spaced word pairs)
 **
 ******************************************************/

/* Pasi's variables and functions */

#include "fst-types.h"

extern int LexEOF;
extern finfo *regex_file_info;
NETptr last_regex();

/* new variables and functions for nets from regex files */

typedef struct INPUT_HIST_CELL_TYPE *INPUT_HIST_CELLptr;
typedef struct INPUT_HIST_CELL_TYPE {
  finfo *input;                /* input info of this cell */
  INPUT_HIST_CELLptr prev;    /* previous cells */
}
INPUT_HIST_CELLtype;

NVptr regex_from_atfile (char *filename, FILE *stream, int num_nets);
NETptr FsmFstExport net_from_atfile (char *filename, int filetype, int fatstr_p);

/* variables and functions for nets from regex strings */

#include "varstack.h"

#define RE2N_VAR_BEGIN  "$("
#define RE2N_VAR_END    ")"
#define RE2N_VAR_TOKEN  ".RE2N_VAR."

NETptr RE2N_var_net (char *num_str);

NETptr RE2N_create_net (RE2N_VSTptr varstack, char *regex_str);
void RE2N_def_var (RE2N_VSTptr varstack, char *name, char *regex_str);

NETptr FsmFstExport RE2N_net_from_regex_string (char *regex_str);
