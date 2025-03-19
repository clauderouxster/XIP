/* $Id: config.h,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1994 by the Xerox Corporation.  All rights reserved */

/*********************************************************************
 *                                                                   *
 *                                                                   *
 *                                                                   *
 *********************************************************************/

#ifndef _FsmConfig_h
#define _FsmConfig_h

#ifdef __cplusplus
extern "C" {
#endif

enum ConfigState { ConfigEmpty, ConfigInitialized };

typedef struct _fsm_config
{
    int state;
    char *home;
    char *copyright;
    int  completion;
} FsmConfigRec, *FsmConfig;

extern void FstBaseExport fsm_config_init();
extern void FstBaseExport fsm_read_rcfile();
extern char FstBaseExport *fsm_copyright(char *new_owner);

#ifdef __cplusplus
}
#endif

#endif /* _FsmConfig_h */

