/* $Id: config.h,v 1.2 2002/04/24 12:23:20 jmcoursi Exp $ */
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

extern void fsm_config_init();
extern void FsmFstExport fsm_read_rcfile();
extern char *fsm_copyright();

#ifdef __cplusplus
}
#endif

#endif /* _FsmConfig_h */

