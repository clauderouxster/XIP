/* $Id: readscript.h,v 1.3 2008/01/21 14:35:23 roux Exp $ */
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
   filename   : readscript.h
   Date       : 21/01/2002
   Purpose    : Encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/

/*************************************************************
 *
 *  FILE  "readscript.h"
 *
 *      SCRIPT HANDLING FUNCTIONS FOR
 *      LOOK-UP OF WORDS IN DICTIONARIES
 *
 *  Andre Kempe, RANK XEROX, Grenoble, France
 *  30/01/96 - ...
 *
 *************************************************************/

#ifndef _LookupReadScript_h
#define _LookupReadScript_h

#ifdef __cplusplus
extern "C" {
#endif

long calculate_symbol_hash_index (unsigned char *key);
void hash_insert_symbols ();
short hash_find_symbol (char *key);
void make_symbol_table ();

void open_temp_files ();
void close_temp_files ();
void symbols_to_temp_files (FILE *script,void*);
void strategies_to_temp_files (FILE *script,void*);

int get_id_from_language (char *language);
char *get_script_name_from_environment (int id);
char *get_script_name_from_default (int id);
char *get_script_name_from_id (int id);
void find_default_script_name (char *language, char *script_name);
void find_script_name (char *option, char *argument, char *name);

void read_script_get_networks (char *option, char *argument,void*);
NVptr no_script_get_networks (char *argument);

#ifdef __cplusplus
}
#endif

#endif /* _LookupReadScript_h */
