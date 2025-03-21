/* $Id: tok-out.h,v 1.2 2007/02/13 12:55:58 roux Exp $ */
/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.60
   filename   : tok-out.h
   Date       : 21/01/2002
   Purpose    : Encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/
/* Copyright (c) 1994 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **                    TOK-OUT.H
 **  Lauri Karttunen, RXRC, January 1994
 **  Output functions for the fst tokenizer
 **
 ******************************************************/

int small_net_p(NETptr net, FILE *in, FILE *out);
void tokenize_print_paths(NETptr net, FILE *stream);



