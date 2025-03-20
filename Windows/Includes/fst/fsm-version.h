/* $Id: fsm-version.h,v 1.5 2005/03/29 15:02:05 grondeau Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved.
   Copyright protection claimed includes all forms and matters of
   copyrightable material and information now allowed by statutory or
   judicial law or hereinafter granted, including without limitation,
   material generated from the software programs which are displayed 
   on the screen such as icons, screen display looks, etc.
   */
/*
 * Description: Version and Revision defintions for the FSM library
 * Created:     25 Feb 94 by Herv‰ POIRIER
 */

/* DON'T EDIT THIS FILE. USE THE 'update-rev' awk script or gmake update-{version|revision|level}
 */
 
#ifndef FSM_VERSION_H
#define FSM_VERSION_H

#define FSM_VERSION      8
#define FSM_REVISION     4
#define FSM_UPDATE_LEVEL 4
#define FSM_Version      ((FSM_VERSION * 100 + FSM_REVISION) * 100 + FSM_UPDATE_LEVEL)
#define FSM_FileVersion  ((FSM_VERSION << 4) | FSM_REVISION)
#define FSM_VERSION_STRING "8.4.4"

#endif /* FSM_VERSION_H */
