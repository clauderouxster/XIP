/* $Id: merge.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990-97 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  MERGE.H
 **
 **  Lauri Karttunen, XRCE, November 1997
 **
 **  A variant of intersection: specialization of one finite-state
 **  language by another one. Support for the merge operators,
 **  .m>. and .<m.
 **
 ******************************************************/

NETptr merge(NETptr n1, NETptr n2);
NETptr merge_fsm(NETptr net1, NETptr net2, int reclaim_p, int minimize_p);
