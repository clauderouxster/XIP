/* $Id: ems.h,v 1.2 2007/02/13 12:55:57 roux Exp $ */
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
   filename   : ems.h
   Date       : 21/01/2002
   Purpose    : Encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/

/*************************************************************
 *
 *  FILE  "ems.h"
 *
 *      ERROR HANDLING,
 *      MEMORY HANDLING and
 *      STRING HANDLING for
 *      LOOK-UP OF WORDS IN DICTIONARIES
 *
 *  Andre Kempe, RANK XEROX, Grenoble, France
 *  30/01/96 - ...
 *
 *************************************************************/

#ifndef _LookupEms_h
#define _LookupEms_h

#ifdef __cplusplus
extern "C" {
#endif

void FsmLookupExport error_message (char *msg, ...);
void FsmLookupExport work_message (char *msg, ...);
void FsmLookupExport print_success_count ();
void FsmLookupExport set_lookup_flags(int argn, char **argv);

void FsmLookupExport *mem_alloc (long size);
void *mem_realloc (void *old, long size);
void FsmLookupExport alloc_symbol_table (long num_sym, long bf_len);
void FsmLookupExport free_symbol_table ();
void alloc_net_file_vector (long num);
void free_net_file_vector ();
void FsmLookupExport alloc_success_count ();
void FsmLookupExport free_success_count ();
void FsmLookupExport alloc_in_out_buffers ();
void FsmLookupExport free_in_out_buffers ();

char FsmLookupExport *lookup_read_line (char *line, int len, FILE* file);
int FsmLookupExport tokenize_line (char *line, char **token, char *sep);

int FsmLookupExport line_is_empty (char *line);
int FsmLookupExport line_contains_tags (char *line);
void FsmLookupExport write_tagged_line (char *line);

void FsmLookupExport select_result_function ();

#ifdef __cplusplus
}
#endif

#endif /* _LookupEms_h */
