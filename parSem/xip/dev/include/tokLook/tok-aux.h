/* $Id: tok-aux.h,v 1.16 2017/03/17 09:37:26 roux Exp $ */
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
   filename   : tok-aux.h
   Date       : 21/01/2002
   Purpose    : Encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/
/* Copyright (c) 1993 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **                    TOK-AUX.H
 **  Lauri Karttunen, Xerox PARC, December 1993
 **  
 **
 ******************************************************/

#ifndef _TokenizeTokAux_h
#define _TokenizeTokAux_h

#include "tokenize_windefs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OUT_CELL {
  id_type outid;
  uint16 refcount;
  long offset;
  /* unsigned int token_boundary; */
  struct OUT_CELL  *link;
} OUT_CELLtype, *OUT_CELLptr;

#define OC_id(X)           (X)->outid
#define OC_l(X)            (X)->link

#define TOKEN_BOUNDARY_DEFAULT  '\n' 
extern unsigned int16  tok_bound;

enum is_first_of_cell_chain {head_reached_but_branching = -1,
			     head_not_yet_reached,
			     head_reached };


int FsmTokenizeExport deterministic_tokenize(NETptr token_fst, FILE *in_stream, char* string,
											void* parseur, RANGEptr* range_map, PARSE_TBL* parse_table,
                                             int flag_diacritics_and_slow,
                                             int doubled_config_elimination_allowed);

int FsmTokenizeExport nondeterministic_tokenize(NETptr token_fst, FILE *in_stream, FILE *out_stream,
			      int continue_fn(NETptr net, FILE *in, FILE *out),
			      void output_fn(NETptr net, FILE *stream),
			      int dont_vectorize);

#define tokenize deterministic_tokenize

/* unsigned int FsmTokenizeExport set_nondet_limit (unsigned int length_of_longest_allowed_deterministic_chain); */
extern unsigned int FsmTokenizeExport nondet_limit;

typedef struct apply_transducer_handle {
  ALPHABETptr sigma;
  RANGEptr lower_range_map;
  HEAPptr config_heap;
  HEAPptr out_cell_heap;
  PARSE_TBL parse_table ;
  MATCH_TABLEptr apply_match_table;
  RANGE_VECTORptr * rvtable;
  id_type tok_bound;
} APPLY_TRANSDUCER_HANDLE, *APPLY_TRANSDUCER_HANDLEptr;

int FsmTokenizeExport Apply_Transducer (id_type (* TX_Read) (void * user_data,  PARSE_TBL parse_table),
		       void (* TX_Write) (id_type id, void *user_data),
		       NETptr token_fst,
		       void * user_data,
		       APPLY_TRANSDUCER_HANDLEptr apply_transducer_handle, int flag_diacritics_and_slow);

APPLY_TRANSDUCER_HANDLEptr FsmTokenizeExport Initialize_Apply_Transducer (NETptr transducer, int flag_diacritics_and_slow);

void FsmTokenizeExport Release_Apply_Transducer(APPLY_TRANSDUCER_HANDLEptr handle);

#ifdef __cplusplus
}
#endif

#endif /* _ApplyTransducerTokAux_h */
