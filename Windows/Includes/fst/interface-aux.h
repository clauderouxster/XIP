/* Copyright (c) 1996-99 by Xerox Research Centre Europe.  All rights reserved. */
/* Created by Tamas Gaal (copied from lex-aux.h of Lauri Karttunen). */
/* $Id: interface-aux.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

unsigned char *interface_remove_percent(unsigned char *str, int strip);
unsigned char *interface_decode_quoted(unsigned char *str);
unsigned char *interface_expand_quoted(unsigned char *str);
char *interface_last_lex_string();
