/* $Id: lexicon.lex.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $Copyright (c) 2002 by Xerox Research Centre Europe.  All rights reserved. */
/* $Id: lexicon.lex.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Created by Tamas Gaal 
   export lexicon.l -originated names to the parser 
*/

#include "fst/fst_windefs.h"

extern void lexicon_rem_last(char *str); /* exported form lexicon.l -> lexicon.lex.c */
extern int lexicon_lex(void);              /* exported form lexicon.l -> lexicon.lex.c */
