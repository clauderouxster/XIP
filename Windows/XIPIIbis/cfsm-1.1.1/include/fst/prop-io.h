/* $Id: prop-io.h,v 1.3 2002/04/24 12:23:28 jmcoursi Exp $ */
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

int propio_parse(void);
int propio_error(char *str);
int propio_lex(void);

int add_props(NETptr net, char *file);
int reset_props(NETptr net, char *filee);
int save_props(NETptr net, char *file);

int FsmFstExport add_props_s(NETptr net, FILE *stream);
int FsmFstExport reset_props_s(NETptr net, FILE *stream);
int FsmFstExport save_props_s(NETptr net, FILE *stream);

OBJECTptr NewObject(int);
SEQUENCEptr FsmFstExport NewSequence(int);
void FsmFstExport SetSequenceLen(SEQUENCEptr, int);
int SameObjectsInSequence(SEQUENCEptr);
void ReduceSequenceOfSameObjects(SEQUENCEptr);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_PROP_IO */
