/* $Id: memoire.h,v 1.4 2007/02/13 12:54:03 roux Exp $ */
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
   filename   : memoire.h
   Date       : 10/03/2002
   Purpose    : MEMORY HANDLING FUNCTIONS encapsultation
   Programmer : Claude ROUX
   Reviewer   : 
*/

/*************************************************************
 *
 *  FILE  "memory.h"
 *
 *      MEMORY HANDLING FUNCTIONS FOR
 *      DISAMBIGUATION OF ALTERNATIVE TAGS
 *
 *  Andre Kempe, RANK XEROX, Grenoble, France
 *  30/01/96 - ...
 *
 *************************************************************/

void sort_evaluated_tags (VITPOSptr vec);

void *mem_alloc (long size);
void *mem_realloc (void *old, long size);

void alloc_tags (int num_tags);
void free_tags ();

void alloc_classes (int num_classes);
void free_classes ();

short *alloc_tag_vector (int num_tags);
void put_tag_to_sorted_vector (short tag, short *vec);

void alloc_hmm_basic ();
void free_hmm_basic ();

VTREEptr alloc_viterbi (int max);
void free_viterbi (VTREEptr vtree);

void *get_equal_item_from_heap (EIHEAPptr heap);
void put_equal_item_to_heap (EIHEAPptr heap, void *item);

#define alloc_alltag_cell(V)     (ATGCELLptr)get_equal_item_from_heap((V)->heap.atgcells)
#define free_alltag_cell(C,V)    put_equal_item_to_heap((V)->heap.atgcells,(void *)(C))
ATGCELLptr clear_alltag_cell (ATGCELLptr cell);

#define alloc_vit_cell(V)     (VCELLptr)get_equal_item_from_heap((V)->heap.vcells)
#define free_vit_cell(C,V)    put_equal_item_to_heap((V)->heap.vcells,(void *)(C))
VCELLptr clear_vit_cell (VCELLptr cell);

WORDBFptr alloc_word_buffer (long max_words, long max_altbf, long max_linebf);
void adjust_word_buffer
  (WORDBFptr wbf, long max_words, long max_altbf, long max_linebf);
void free_word_buffer (WORDBFptr wbf);

void alloc_most_mem ();

