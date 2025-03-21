/* $Id: id_type.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */

#ifndef FST_ID_TYPE_H
#define FST_ID_TYPE_H


/******************************************************
 **
 **
 *******************************************************/

#include "fst/syst-dep.h"

#ifdef __cplusplus
extern "C" {
#endif


/* DEFINITION OF THE ID TYPE: CP 30/04/99 */

/* typedef unsigned short id_type; */
typedef uint16 id_type;


/** Id symbol denoting the end of string */
#define ID_EOS ( (id_type) -1)
/** Special Id symbol meaning "no symbol" or "unitialized id value" */
#define ID_NO_SYMBOL ((id_type) -1)

#ifdef __cplusplus
}
#endif
#endif
