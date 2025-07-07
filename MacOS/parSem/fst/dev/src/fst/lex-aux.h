/* Copyright (c) 1996 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Lauri Karttunen. */
/* $Id: lex-aux.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
#include "fst/fst_windefs.h"

unsigned char *remove_percent(unsigned char *str, int strip);
unsigned char *remove_braces(unsigned char *str);
unsigned char *decode_quoted(unsigned char *str);
unsigned char FstCalcExport * expand_quoted(unsigned char *str);
char *last_lex_string();
