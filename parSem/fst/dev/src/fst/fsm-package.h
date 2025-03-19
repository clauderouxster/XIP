/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: fsm-package.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

/* all includes needed from c-fsm (I hope) */

#ifndef FSM_PACKAGE_H
#define FSM_PACKAGE_H

#define STANDARD_NET 0
#define COMPACTED_NET 1
#define VIRTUAL_NET 2

#include <stdio.h>
#include "fst/calculus.h"
#include "fst/context.h"
#include "fst/compact.h"
#include "fst/bin-in.h"
#include "fst/bin-out.h"
#include "fst/flags.h"
#include "fst/net-io.h"
#include "fst/text-io.h"
#include "fst/pretty-p.h"
#include "fst/prop-io.h"
#include "fst/re-aux.h"
#include "fst/pars-tbl.h"
#include "fst/look.h"
#include "fst/eqv-labs.h"
#include "fst/prolog.h"
#include "fst/subseq.h"
#include "fst/haskell.h"
#include "fst/ambclass.h"
#include "fst/virtual-fsm.h"
#include "fst/fst-types.h"  /* This is a local file */
#endif
