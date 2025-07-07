/* $Id: prop-io.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1994  by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  PROP-IO.H
 **
 **  Lauri Karttunen, RXRC 1994
 **  
 **  Functions for setting and adding network properties
 **  from files.
 **
 ******************************************************/

#ifndef C_FSM_PROP_IO
#define C_FSM_PROP_IO

#ifdef __cplusplus
extern "C" {
#endif

int FstCalcExport propio_parse(void);
int FstCalcExport propio_error(char *str);
int FstCalcExport propio_lex(void);
/* void FstCalcExport propio_restart(FILE *stream); */

int FstCalcExport add_props(NETptr net, char *file);
int FstCalcExport reset_props(NETptr net, char *filee);
int FstCalcExport save_props(NETptr net, char *file);

int FstCalcExport add_props_s(NETptr net, FILE *stream);
int FstCalcExport reset_props_s(NETptr net, FILE *stream);
int FstCalcExport save_props_s(NETptr net, FILE *stream);

void FstCalcExport init_prop_io_line_count(void);
void FstCalcExport increment_prop_io_line_count(void);
int FstCalcExport prop_io_line_count(void);

OBJECTptr FstCalcExport NewObject(int);
SEQUENCEptr FstCalcExport NewSequence(int);
void FstCalcExport SetSequenceLen(SEQUENCEptr, int);
int FstCalcExport SameObjectsInSequence(SEQUENCEptr);
void FstCalcExport ReduceSequenceOfSameObjects(SEQUENCEptr);

void FstCalcExport parse_propio();

/* extern OBJECTptr FstCalcExport propio_value;
extern FAT_STR FstCalcExport propio_attr;
extern int FstCalcExport propio_eof;
extern FILE FstCalcExport *propio_in;
*/

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_PROP_IO */
