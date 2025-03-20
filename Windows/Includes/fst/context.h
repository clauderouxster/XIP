/* $Id: context.h,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
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
 
#include "fst/types.h"
#include "fst/error.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

FST_CNTXTptr FstBaseExport make_default_context(void);
FST_CNTXTptr FstBaseExport set_default_context (void);
void FstBaseExport free_default_context(FST_CNTXTptr fst_cntxt);
FST_CNTXTptr copy_fst_context(FST_CNTXTptr cntxt);

FST_CNTXTptr FstBaseExport set_default_context (void);
void FstBaseExport *get_default_context(void);
void FstBaseExport unset_default_context(void);

void FstBaseExport set_io_char_encoding(FST_CNTXTptr fst_cntxt, int io_char_encoding);

#ifdef __cplusplus
}
#endif /* __ cplusplus */

#endif /* C_FSM_CONTEXT */

/* DON'T ADD ANYTHING AFTER THIS #endif */


