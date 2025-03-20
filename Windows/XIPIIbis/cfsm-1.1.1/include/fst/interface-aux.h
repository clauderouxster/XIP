/* Copyright (c) 1996-99 by Xerox Research Centre Europe.  All rights reserved. */
/* Created by Tamas Gaal (copied from lex-aux.h of Lauri Karttunen). */
/* $Id: interface-aux.h,v 1.1 1999/06/08 13:06:33 gaal Exp $ */

unsigned char *interface_remove_percent(unsigned char *str, int strip);
unsigned char *interface_decode_quoted(unsigned char *str);
unsigned char *interface_expand_quoted(unsigned char *str);
char *interface_last_lex_string();
