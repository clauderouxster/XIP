/* $Id: context.h,v 1.10 2003/01/31 14:10:25 jmcoursi Exp $ */
/* Copyright (c) 1998 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  context.h
 **
 **  Lauri Karttunen, January 1998
 **
 **
 ******************************************************/

#ifndef C_FSM_CONTEXT
#define C_FSM_CONTEXT
 
#include "types.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

FST_CNTXTptr FsmFstExport make_default_context(void);
FST_CNTXTptr FsmFstExport set_default_context (void);
void FsmFstExport free_default_context(FST_CNTXTptr fst_cntxt);
FST_CNTXTptr copy_fst_context(FST_CNTXTptr cntxt);

extern void* default_context_object;
FST_CNTXTptr FsmFstExport set_default_context (void);
void FsmFstExport *get_default_context(void);
void FsmFstExport unset_default_context(void);

void FsmFstExport set_io_char_encoding(FST_CNTXTptr fst_cntxt, int io_char_encoding);

#ifdef __cplusplus
}
#endif /* __ cplusplus */

#endif /* C_FSM_CONTEXT */

/* DON'T ADD ANYTHING AFTER THIS #endif */


