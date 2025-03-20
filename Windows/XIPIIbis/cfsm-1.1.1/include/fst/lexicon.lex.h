/* $Id: lexicon.lex.h,v 1.1 2002/03/27 10:00:07 gaal Exp $Copyright (c) 2002 by Xerox Research Centre Europe.  All rights reserved. */
/* $Id: lexicon.lex.h,v 1.1 2002/03/27 10:00:07 gaal Exp $ */
/* Created by Tamas Gaal 
   export lexicon.l -originated names to the parser 
*/


#define yylex lexicon_lex         /* exported form lexicon.l -> lexicon.lex.c , for lexicon.y */
void lexicon_rem_last(char *str); /* exported form lexicon.l -> lexicon.lex.c */
int lexicon_lex(void);              /* exported form lexicon.l -> lexicon.lex.c */
