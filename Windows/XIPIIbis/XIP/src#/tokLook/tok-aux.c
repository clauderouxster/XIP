/* $Id: tok-aux.c,v 1.4 2006/04/14 15:11:40 roux Exp $ */

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
   filename   : tok-aux.c
   Date       : 
   Purpose    : tokenization encapsulation
   Programmer : Claude ROUX
   Reviewer   : 
*/

/* Copyright (c) 1993 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **                    TOK-AUX.C
 **  Lauri Karttunen, RXRC, December 1993
 **  C version of Ronald M. Kaplan's Medley Lisp tokenizing routine.
 **  Tamas Gaal, RXRC, March 1996
 **  Simplified output structure - trees only instead of fsm state graphs
 **
 ******************************************************/

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "fst/modify.h"
#include "fst/pars-tbl.h"
#include "fst/vectorize.h"
#include "fst/flags.h"
#include "fst/eqv-labs.h"
#include "fst/fsm_io.h"
#include "fst/pretty-p.h"
#include "fst/debug.h"
#include "tok-aux.h"
#include "versiontok.h"
#include "utf8_fatstr_util.h"

#ifndef FST_symbol_from_string
#define FST_symbol_from_string	  symbol_from_string
#endif


static char rcs_id[] = "$Id: tok-aux.c,v 1.4 2006/04/14 15:11:40 roux Exp $";
unsigned int16  tok_bound = TOKEN_BOUNDARY_DEFAULT;

int symbolsize(id_type id,void* parseur);
void produce_tokens_det(OUT_CELLptr cell, void *parseur);
void free_output_cells_and_print_if_sure(OUT_CELLptr cell, void* parseur);
enum is_first_of_cell_chain produce_tokens_nobranching_det (OUT_CELLptr cell, 
								   OUT_CELLptr succ,
								   OUT_CELLptr rs_branch,
								   void *parseur);

#ifdef DEBUG
#define cell_alloc(h) FstDebug_cell_alloc(h, __FILE__, __LINE__)
#define free_cell(cell, heap) FstDebug_free_cell(cell, heap, __FILE__, __LINE__)
ALPHABETptr upper_sigma(NETptr net)
{
    return(side_sigma(net, UPPER));
}
ALPHABETptr side_sigma (NETptr net, int side)
{
/* Returns a binary alphabet that contains the symbols that appear
on one or the other side of the net. Assumes that net labels is correctly
maintained. 
If the net has reduced labelsets, the label alphabet must be "reduced",
too, but the restored unreduced label alphabet of the original net is used
instead.
The principle is that sigma alphabets (sigma, upper-, lower-sigma) reflect
the unreduced net and the label alphabet is based on the real arcs (that
is, reduced). 
    */
    
    ALPHABETptr externally_unreduced_net_labels=NULL, labels = NULL;
    ALPH_VECTORptr reduced_to_unreduced_arcs_map=NULL;
    int max_l_c = 0;
    ALPHABETptr alph=NULL;
    id_type lab_id, sym_id, *items=NULL;
    int  i, len=0;
    
#ifndef SLIM_FST
    if (NET_reduced_labelset(net))
    {
        externally_unreduced_net_labels = get_unreduced_labels(net, &reduced_to_unreduced_arcs_map);
        labels = binary_to_label(externally_unreduced_net_labels);
    }
    else
#endif /* SLIM_FST */
        labels = binary_to_label(NET_labels(net));
    
    max_l_c = max_label_component(labels, side);
    alph = make_alph((max_l_c == 0 ? 0 : (max_l_c + 1)),
        BINARY_VECTOR);
    items = ALPH_items(labels);
    len = ALPH_len(labels);
    
    for (i = 0; i < len; i++)
    {
        lab_id = *items++;
        
        if (side == UPPER)
            sym_id = upper_id(lab_id);
        else
            sym_id = lower_id(lab_id);
        
        if (sym_id != EPSILON)
            ALPH_item(alph, sym_id) = 1;
        
        if (side == BOTH_SIDES && lab_id != sym_id)
        {
            sym_id = upper_id(lab_id);
            if (sym_id != EPSILON)
                ALPH_item(alph, sym_id) = 1;
        }
    }
    
    free_reduced_to_unreduced_map(&reduced_to_unreduced_arcs_map);
    free_alph(externally_unreduced_net_labels);
    return(alph);
}
#endif


#ifndef APPLY_TRANSDUCER_API
/* Standard Tokenize */
id_type *  TOK_CHAR_VECT ;
#endif /* !APPLY_TTRANSDUCER_API */


#ifdef DEBUG
void dummy_debug_print(NETptr token_fst, int arcs)
{
  debug_print_net(token_fst, arcs);
}
#endif 

inline
static int is_epsilon_plus (id_type id)
{
  if ( ( id == EPSILON)
       || ( id == ALTCHAIN_SYMBOL) )
    return (TRUE);

  return(FALSE);
}

inline
static int is_epsilon_plus_plus (id_type id)
{
  if (   ( id == EPSILON)
      || ( id == ALTCHAIN_SYMBOL)
      || ( flag_diacritic_p(id) )
)
    return (TRUE);
 
  return(FALSE);
}


/* OUT_CELL is to build trees in the deterministic case. It is used
   like OUTPUT_STATE (in the non-deterministic case) when building
   alternate predictions depending on the input, the tokenizing
   transducer used, and the possible previous set of predictions.
   */

/* is_same_w_chain() is borrowed from ../fst/subseq.c 7.7.97 */
inline
static int is_same_w_chain (OUT_CELLptr a, OUT_CELLptr b)
{
  int result=FALSE;
  OUT_CELLptr  next_a = NULL, next_b = NULL;

  for ( ;
	a || b ;
	a = next_a , b = next_b ) 
    {
      while ( a && is_epsilon_plus (OC_id(a)))
	a = OC_l(a);
      while ( b && is_epsilon_plus (OC_id(b)))
	b = OC_l(b);
	      
      if (( a && b &&  (OC_id(a) ==  OC_id(b)) ) 
	  ||  ( !a && !b))
	result = TRUE;
      else
	if ( (a && (!is_epsilon_plus (OC_id(a))) && (!b))
	     || (b && (!is_epsilon_plus (OC_id(b))) && (!a))
	     || (a&& b && ( OC_id(a) !=  OC_id(b))))
	  return (FALSE);
      if (a)
	next_a = OC_l(a);
      else 
	next_a = NULL;
      if(b)
	next_b = OC_l(b);
      else 
	next_b = NULL;
    }
  return(result);
}

typedef struct CONFIGURATION {
  struct STATE *fst_state;
  union {
    struct OUT_CELL  *cell;
    struct STATE *state;
  } output;
  ALPHABETptr flag_diacritic_register;
  struct CONFIGURATION *alt_config; 
} CONFIG, *CONFIGptr;

#define CONFIG_fst_state(X)	(X)->fst_state
#define CONFIG_output_cell(X)	(X)->output.cell
#define CONFIG_output_state(X)	(X)->output.state
#define CONFIG_flag_d_reg(X)    (X)->flag_diacritic_register
#define CONFIG_next(X)		(X)->alt_config

inline
static int is_same_config (CONFIGptr aa, CONFIGptr bb)
{
  int result=FALSE;

  if (aa && bb &&
      (CONFIG_fst_state(aa) == CONFIG_fst_state(bb)))
    /* there may be identity. Symbol chains must be verified, too */
    {
      result = is_same_w_chain( CONFIG_output_cell(aa),  CONFIG_output_cell(bb));
    }
  return(result);
}

typedef struct LINK_AND_REFCOUNT {
  struct STATE *prevstate;
  long int refcount;
} LINK, *LINKptr;

#define LINK_prevstate(X)	(X)->prevstate
#define LINK_refcount(X)	(X)->refcount

#define state_backlink(X)	LINK_prevstate((LINKptr) STATE_client_cell(X))
#define state_refcount(X)	LINK_refcount((LINKptr) STATE_client_cell(X))

static HEAPptr TOK_CONFIG_HEAP = NULL;
static HEAPptr TOK_LINK_HEAP = NULL;
static STACKptr TOK_OUTPUT_STACK = NULL;

static HEAPptr TOK_OUT_CELL_HEAP = NULL;


static void allocate_memory_det(void)
{
  TOK_CONFIG_HEAP = init_heap(sizeof(CONFIG), 16384/sizeof(CONFIG),
			  "Tokenizer configs");

  TOK_OUT_CELL_HEAP = init_heap(sizeof(OUT_CELLtype), 16384/sizeof(OUT_CELLtype),
			"Tokenizer out_cells");
}

static void reclaim_memory_det(void)
{
  free_heap(TOK_CONFIG_HEAP);
  TOK_CONFIG_HEAP = NULL;

  free_heap(TOK_OUT_CELL_HEAP);
  TOK_OUT_CELL_HEAP = NULL;

}

void free_out_cell(OUT_CELLptr out)
{
  if (out)
#ifdef TK_DEBUG_HEAP
 /* IF DEBUGGING set BREAKPOINT in heap.c at a marked line */
    free_cell_debug((void *) out, TOK_OUT_CELL_HEAP);
#else
  free_cell((void *) out, TOK_OUT_CELL_HEAP);
#endif
}

/* *** BOTH NONDET_LIMIT and TOKEN_BOUNDARY should be command_line-switches ! *** */
/* NONDET_LIMIT can be set by set_nondet_limit() */

unsigned int nondet_limit;



/* #define TOKEN_BOUNDARY_PRINTED (-1) */ /* CP: 19/08/99: unused */

#ifdef APPLY_TRANSDUCER_API
/*----------------------------------------------------------------------------*/
/* Apply_Transducer interface (for XeLDA) */

static ALPHABETptr lower_sigma_tg(NETptr net);

APPLY_TRANSDUCER_HANDLEptr Initialize_Apply_Transducer (NETptr transducer, int dont_vectorize)
{
  APPLY_TRANSDUCER_HANDLEptr handle = NULL;

  if (transducer)
    {
      /* The token boundary is parsed to be used as a symbol in the code. */
      char token_boundary_str[2] = { TOKEN_BOUNDARY_DEFAULT, '\0'};
      char *remain;

      handle = (APPLY_TRANSDUCER_HANDLEptr) calloc (1, sizeof (APPLY_TRANSDUCER_HANDLE));

      handle->config_heap = init_heap(sizeof(CONFIG), 16384/sizeof(CONFIG),
				      "Apply_T configs");
      handle->out_cell_heap = init_heap(sizeof(OUT_CELLtype), 16384/sizeof(OUT_CELLtype),
					"Apply_T out_cells");
      if ( ! dont_vectorize)
        vectorize_net(transducer);
      handle->sigma = lower_sigma_tg(transducer);
      handle->lower_range_map = NET_uprange_map(transducer); /* make a range map if necessary */
      if (handle->lower_range_map == NULL)
          handle->lower_range_map = make_range_map(transducer, LOWER);
      handle->parse_table = get_parse_table(transducer, LOWER);

      symbol_from_string (&handle->tok_bound, token_boundary_str, &remain,
                          handle->parse_table, /* zero_to_eps_p */ FALSE,
                          /* ignore_white_space_p */ FALSE, /* escape_p */ FALSE);
      if (handle->tok_bound == ID_NO_SYMBOL)
	{
          fprintf(stderr, " *** Stop. Can't parse token boundart '0x%x'.\n", TOKEN_BOUNDARY_DEFAULT);
          Release_Apply_Transducer(handle);
          handle = NULL;
	}
     if (NET_reduced_labelset(transducer))
       {
	 handle->apply_match_table = get_match_table(transducer, LOWER);
	 handle->rvtable = MATCH_TABLE_table(handle->apply_match_table);
       }
     else
       {
	 handle->apply_match_table = NULL;
	 handle->rvtable = NULL;
       }
    }
  else
    {
      fprintf(stderr, " *** Initialize_Apply_Transducer - Exit. The fst value is NULL.\n");
      handle = NULL;
    }
  return (handle);
}

void Release_Apply_Transducer(APPLY_TRANSDUCER_HANDLEptr handle)
{
  free_alph(handle->sigma); 
  handle->sigma = NULL;
  free_heap(handle->config_heap); 
  handle->config_heap  = NULL;
  free_heap(handle->out_cell_heap); 
  handle->out_cell_heap = NULL;
  handle->lower_range_map = NULL;
  if (handle->parse_table != NULL)
      free_parse_table(handle->parse_table);
  if (handle->apply_match_table)
    {
      free_match_table(handle->apply_match_table);
      handle->apply_match_table = NULL;
      handle->rvtable = NULL;
    }
  free((void *)handle);
}


/*----------------------------------------------------------------------------*/
#endif /* APPLY_TRANSDUCER_API */

static int is_token_boundary (int ch)
/* Several token boundaries may exist. If so then this
   function needs improvement - now only newline '\n' is checked.
   */
{
  if (ch == tok_bound)
    return TRUE;
  else return FALSE;
}

inline
static int is_significant_begin_token_boundary (OUT_CELLptr ch)
/* Stepping over epsilons and stating whether the first non-epsilon
   id is a token boundary or not
   */
{
  int result = FALSE;
  for ( ; ch ; ch = ch->link)
    {
      if (ch->outid == tok_bound)
	return TRUE;
      if (ch->outid != EPSILON)
	continue;
    }
  return result;
}

static OUT_CELLptr create_output_cell(id_type outid)
{
  /* Allocates an output state, getting one from the freelist,
     if possible. The call to create_output_cell() creates a new
     state: an OUT_CELL is requested from its HEAP ...
     The reference count of the new state is initialized to 0.
     */

  OUT_CELLptr new_out_cell = (OUT_CELLptr) cell_alloc(TOK_OUT_CELL_HEAP);
  if (new_out_cell == NULL)
    not_enough_memory ("ALLOC OUT_CELL");
  new_out_cell->link = NULL;
  new_out_cell->refcount = 0;
/*   if (outid == tok_bound)
    new_out_cell->token_boundary = TRUE;
  else
    new_out_cell->token_boundary = FALSE; 
*/
  new_out_cell->outid =  outid;
  return (new_out_cell);
}

static CONFIGptr make_config_det(STATEptr fst_state)
{
  CONFIGptr config = (CONFIGptr) cell_alloc(TOK_CONFIG_HEAP);
  CONFIG_fst_state(config) = fst_state;
  CONFIG_next(config) = NULL;
  return(config);
}

static int count_alt_configs(CONFIGptr cf, int from)
{
  while (cf && (CONFIG_next(cf)))
    {
      from++;
      cf = CONFIG_next(cf);
    }
  return from;
}

static void free_config(CONFIGptr config)
{
  if (config)
    {
      if (CONFIG_flag_d_reg(config))
	{
	  free_alph(CONFIG_flag_d_reg(config));
	}
#ifdef TK_DEBUG_HEAP  
/* IF DEBUGGING set BREAKPOINT in heap.c at a marked line */
      free_cell_debug((void *) config, TOK_CONFIG_HEAP); 
#else
      free_cell((void *) config, TOK_CONFIG_HEAP);
#endif
    }
}

static CONFIGptr new_cur_config(CONFIGptr cur_cnfg, STATEptr dest)
{
  /* Makes a new config and splices it in after cur_cnfg */

  CONFIGptr new = make_config_det(dest);
  CONFIG_next(new) = CONFIG_next(cur_cnfg);
  CONFIG_next(cur_cnfg) = new;
  CONFIG_flag_d_reg(new) = copy_alphabet(CONFIG_flag_d_reg(cur_cnfg));
  return(new);
}

static CONFIGptr new_next_config(CONFIGptr *next_configs, STATEptr dest,
				 CONFIGptr cur_cnfg )
{
    /* Makes a new config and pushes it to configp. */

  CONFIGptr new = make_config_det(dest);
  CONFIG_next(new) = *next_configs;
  *next_configs = new;
   CONFIG_flag_d_reg(new) = copy_alphabet(CONFIG_flag_d_reg(cur_cnfg));
  return(new);
}

static void add_branch(id_type label_id, OUT_CELLptr prev, CONFIGptr cnfg, long position)
{
  /* Allocates a new branch and links it to the output at CNFG. Adds refcount
     to its (backwards) destination.
     7.7.97: 
     - If label_id is EPSILON, don't reserve a cell for that.
     - Reference count of an output_cell reflects pointing config-s, too.
     */

  if ( ! is_epsilon_plus (label_id))
       {
	 CONFIG_output_cell(cnfg) = create_output_cell(label_id);
	 CONFIG_output_cell(cnfg)->refcount++; /* NEW 7.7.97 */
     CONFIG_output_cell(cnfg)->offset=position; /*C.R. 4.16.2005 */
	 if (prev)
	   {
	     prev->refcount++ ;
	   }

	 CONFIG_output_cell(cnfg)->link = prev;
       }
  else
    {
      CONFIG_output_cell(cnfg) = prev;
      if (prev)
	{
	  prev->refcount++ ;
	}

    }
}

static void free_output_cell(OUT_CELLptr cell)
{
  /* Supposes a linear chain backwards from cell;
     If a previous state is referred only by this, it can be deleted.
     Repeat it recursively!
     */

    OUT_CELLptr prev;

    while (cell && cell->refcount < 1)
    {
        if ((prev = cell->link))
            prev->refcount--;
        free_out_cell(cell);
        cell = prev;
    }
}

static void free_config_and_output_cells(CONFIGptr config)
{
  OUT_CELLptr cell;

  if (config)
    {
      if ((cell = CONFIG_output_cell(config)))
	{
	  cell->refcount--; /* refcount now reflects ALL pointers (also those from
			       config-s) to this element */
	  free_output_cell(cell);
	}
      if (CONFIG_flag_d_reg(config))
	{
	  free_alph(CONFIG_flag_d_reg(config));
	}
#ifdef TK_DEBUG_HEAP
      /* IF DEBUGGING set BREAKPOINT in heap.c at a marked line */  
      free_cell_debug((void *) config, TOK_CONFIG_HEAP); 
#else
      free_cell((void *) config, TOK_CONFIG_HEAP); 
#endif    
    }
}

/* Borrowed and modified from ..lookup/networks.c 7.7.97
   This is expensive - but less expensive than letting them multiply ! */
static CONFIGptr eliminate_doubled_configs(CONFIGptr *cur_configs)
{
  CONFIGptr next, cf, thisconfig, prev = NULL; 
  int killed;

  for (thisconfig = *cur_configs; thisconfig; thisconfig = next)
    {
      next = CONFIG_next(thisconfig);
      killed = FALSE;
      for (cf =  CONFIG_next(thisconfig) ; cf; cf = CONFIG_next(cf))
	{
	  killed = FALSE; 

	  if ( 	is_same_config ( thisconfig, cf))
	    {
	      if (prev)
		CONFIG_next(prev) = next;
	      else
		*cur_configs = next;
	      free_config_and_output_cells(thisconfig);
	      killed = TRUE;
	      break;
	    }
	}
      if (! killed)
	prev = thisconfig; 
    }
  return (*cur_configs);
}



/* This enum -  is_first_of_cell_chain - shows whether the sort of cell chain
   is what we expect or not */

static void produce_tail_tokens_det(CONFIGptr cur_configs,
                                    void *parseur,
                                    int possible_flag_diacritics,
                                    long offset)
{
  /* Produce remaining end-of-string tail tokens. We have to scan the
     remaining configs to output the results of any final epsilon
     transitions. We do it by joining the arcs of all the final-state
     configs into a single last output state then producing it.
     */

  CONFIGptr config, next, prev = NULL;
  OUT_CELLptr last_state = NULL, output_cell;
  ALPHABETptr low_eps, low_fld;
  AVptr arc_vector; /* fast */
  ARCptr arc;        /* slow */
  uint16 i;
  id_type up, fst_id, low, old_val, attr;
  STATEptr fst_state, dest;

  /* 97-03-26 final epsilon transitions may occur. Only FAST - vectorized */
  /********  for (config = cur_configs; config; config = next) -- ?? 2000-9-28 ******/
  for (config = cur_configs; config;  config = CONFIG_next(config))
    { 
      /**** next = CONFIG_next(config); -- ?? 2000-9-28 ******/
      fst_state = CONFIG_fst_state(config) ; 
      output_cell = CONFIG_output_cell(config);

      if (STATE_vector_p(fst_state) &&
	  ( arc_vector = STATE_arc_vector(fst_state)))
	{
	  if ((low_eps = AV_l_eps(arc_vector)))
	    {
	      /* Process arcs that have an EPSILON on the lower side.
		 Staying in the current position. Any new
		 config gets stuck on end, so eventually it will be
		 processed at this final string position (no more input).
		 */

	      for (i = 0; i < ALPH_len(low_eps); i++)
		{
		  fst_id = ALPH_item(low_eps, i);
		  up = upper_id(fst_id);
		  dest = AV_dest(arc_vector, fst_id);
		  /*
		    fprintf(stdout, "Arc label-2: ");
		    print_label(fst_id, stdout);
		    fprintf(stdout, "\n");
		    */
		  add_branch(up, output_cell,
			     new_cur_config(config, dest),
                 offset);
		}
	    }
	  /** flag diacritics */
	  if (possible_flag_diacritics &&
	       ( low_fld = AV_l_fld(arc_vector)))
	    {
	      for (i = 0; i < ALPH_len(low_fld); i++)
		{	
		  fst_id = ALPH_item(low_fld, i);
		  up = upper_id(fst_id);
		  low = lower_id(fst_id);
		  dest = AV_dest(arc_vector, fst_id);			
		  if ( (attr = flag_diacritic_attrib(low)))
		    {
		      old_val = flag_value(CONFIG_flag_d_reg(config), attr);
		      if (flag_action (CONFIG_flag_d_reg(config), flag_diacritic(low)))
			{
			  if ( ! is_epsilon_plus_plus (up) )
			    add_branch
			      (up, output_cell,
			       new_cur_config(config, dest),
                   offset);
			  else
			    add_branch
			      (EPSILON, output_cell, 
			       new_cur_config(config, dest),
                   offset);
			}
		      flag_value(CONFIG_flag_d_reg(config), attr) = old_val;
		    }
		}
	    }  /* end of flag diacritics handling */
	}
      else /* SLOW part - needed by flag diacritics */
	for (arc = STATE_arc_set(fst_state); arc; arc = next_arc(arc))
	  {
	    up = upper_id(ARC_label(arc));
	    low = lower_id(ARC_label(arc));
	    if (is_epsilon_plus (low))
	      {
		/* Epsilon. Staying in the current position. Any new
		   config gets stuck on end, so eventually it will be
		   processed at this string position.
			*/
		if ( ! is_epsilon_plus (up) )
		  add_branch(up, output_cell,
			     new_cur_config(config, ARC_destination(arc)),
                 offset);
		else /* this is a hack we know EPSILON == 0  */
		      add_branch(EPSILON,  output_cell,
				 new_cur_config(config, ARC_destination(arc)),
                 offset);
	      }
	    /**Flag diacritics begin */
	    else if ( possible_flag_diacritics &&
		      flag_diacritic_p (low) &&
		      (attr = flag_diacritic_attrib(low)))
	      {
		/* if Flag Diacritics allow then */
		old_val = flag_value(CONFIG_flag_d_reg(config), attr);
		if (flag_action (CONFIG_flag_d_reg(config), flag_diacritic(low)))
		  {
		    /** warning is_epsilon_plus works on real label ids
		      not on chars ! */
		    if ( ! is_epsilon_plus_plus (up) )
		      add_branch(up, output_cell,
				 new_cur_config(config, ARC_destination(arc)),
                 offset);
		    else /* this is a hack we know EPSILON == 0  */
		      add_branch(EPSILON, output_cell,
				 new_cur_config(config, ARC_destination(arc)),
                 offset);	      
		    /* copy flag_diacritic_register to new*/
		  }
		flag_value(CONFIG_flag_d_reg(config), attr) = old_val;
	      } /* Flag diacritics end */
	  }
    }
  /* Eliminate non-final configurations. */

  for (config = cur_configs; config; config = next)
    {
      next = CONFIG_next(config);

      if (STATE_final(CONFIG_fst_state(config)))
	prev = config;
      else
	{
	  if (prev)
	    CONFIG_next(prev) = next;
	  else
	    cur_configs = next;

	  free_output_cells_and_print_if_sure(CONFIG_output_cell(config), parseur);
	  free_config(config);
	}
    }

  /* If more than one final configuration remains, we have
     to copy arcs. */
  /* ** I think it can't happen in the lightweight case ? TG */

  if (( config = cur_configs ))
    {
      if (CONFIG_next(config))
	{
            fprintf(stderr, "*** This tokenizer is not deterministic. ");
            fprintf(stderr, "There are alternative outputs.\n");
            fprintf(stderr, "*** Only one is produced.\n");
	}

      last_state = CONFIG_output_cell(config);
    }

  if (last_state)
    produce_tokens_det(last_state, parseur);
}

static void reset_for_output_det (CONFIGptr config)
{
  /* Free the previous output net (if any). Create a new one and make
     it the output state of the current configuration.
     */

  CONFIG_output_cell(config) = NULL;
  if (CONFIG_next(config))
    handle_warning("** Unexpected alternate configuration.",
		   "RESET_FOR_OUTPUT_DET", 0);
}

static ALPHABETptr get_restored_sigma_from_reduced(NETptr rlnet, int side)
{
  ALPHABETptr restsig=NULL;
  uint16 i;
  id_type id, lo, up;

  if (!NET_reduced_labelset(rlnet))
    return restsig;

  if (! NET_unreduce_key(rlnet))
    get_unreduce_key(rlnet);
  
  restsig = make_alph(global_alph_size(), BINARY_VECTOR);
  ALPH_len(restsig) = 0;
  
  for (i=0 ;  i < ALPH_len(NET_unreduce_key(rlnet)) ; i++ )
    {
      if (ALPH_item(NET_unreduce_key(rlnet), i))
	{
	  id = i;
	  up = upper_id (id);
	  lo = lower_id (id);
	  assert (up != ID_NO_SYMBOL && 
		  lo != ID_NO_SYMBOL);
	  if (up != EPSILON)
	    sigma_add_to(restsig, up);
	  if (lo != EPSILON)
	    sigma_add_to(restsig, lo);	
	}
    }
  return restsig;
}

static ALPHABETptr get_restored_this_side_sigma_from_reduced(NETptr rlnet, int side)
     /* side is either LOWER or UPPER */
{
  ALPHABETptr restsig=NULL;
  uint16 i;
  id_type id, lo, up;

  if (!NET_reduced_labelset(rlnet))
    return restsig;

  if (! NET_unreduce_key(rlnet))
    get_unreduce_key(rlnet);
  
  restsig = make_alph(global_alph_size(), BINARY_VECTOR);
  ALPH_len(restsig) = 0;
  
  for (i=0 ;  i < ALPH_len(NET_unreduce_key(rlnet)) ; i++ )
    {
      if (ALPH_item(NET_unreduce_key(rlnet), i))
	{
	  id = i;
	  if (side == UPPER)
	    {
	      up = upper_id(id);
	      assert (up != ID_NO_SYMBOL);
	      if (up != EPSILON)
		sigma_add_to(restsig, up);
	    }
	  else
	    {
	      lo = lower_id(id);
	      assert (lo != ID_NO_SYMBOL);
	      if (lo != EPSILON)
		sigma_add_to(restsig, lo);
	    }	
	}
    }
  return restsig;
}

static ALPHABETptr lower_sigma_tg(NETptr net)
{
  /*
    This is a HACK to correct lower_sigma () (see also side_sigma and
    upper_sigma() ) when this side of sigma has OTHER and the original other
    symbols as well and labels only contain OTHER - then these other symbols (in
    sigma) count as OTHER-s, too !  17.07.96 TG */
 ALPHABETptr ls;

  
  ls = lower_sigma(net);
  if (sigma_member (ls, OTHER))
  {
    free_alph(ls);
    ls = sigma(net);
  }
  return (ls);
}

static void match_on_arc_list(ARCptr aarc,
			      id_type arc_id,
			      id_type txt_id,
			      OUT_CELLptr output_cell,
			      CONFIGptr config,
			      CONFIGptr * next_configs,
			      /**** The next 4 parameters could go to a structure */
			      id_type * fst_ids,
			      uint16 fst_id_limit,
			      int can_be_flag_diacritics,
			      int *  no_match,
                  long offset)
     /* Match an incoming txt_id on this arc_id and put a resulting configuration
	into next_configs.
	If there is epsilon match (on the given side) then add its info to the
	current config set so that closures of this epsilon matches can be
	further analyzed.
	The same for flag diacritics.
	*/
{
  id_type  attr, low, old_val, up ;

      /* DEBUG
	 fprintf(stdout, "Arc label-s-1: ");  print_label(txt_id, stdout); fprintf(stdout, "\n");
	      */
 
  assert(arc_id);

  up = upper_id(arc_id);
  low = lower_id(arc_id);
  
  if (txt_id == low ||
      (low == OTHER && !(txt_id < fst_id_limit && fst_ids[txt_id])))
    {
      /* Match moving forward. */
  
      /* If ID is an instance of OTHER and the arc label is not
	 an fstpair, use the text symbol instead.
	 */

      if (low == OTHER && up == OTHER)
	up = txt_id;
		
      add_branch(up, output_cell,
		 new_next_config(next_configs,
				 ARC_destination(aarc), 
				 config),offset);
      *no_match = FALSE;
    }
  else if (is_epsilon_plus (low))
    {
      /* Epsilon. Staying in the current position. Any new
	 config gets stuck on end, so eventually it will be
	 processed at this string position.
			*/
      if ( ! is_epsilon_plus (up) )
	add_branch(up, output_cell,
		   new_cur_config(config, ARC_destination(aarc)),offset);
      else /* this is a hack we know EPSILON == 0  */
	add_branch(EPSILON, output_cell,
		   new_cur_config(config, ARC_destination(aarc)),offset);
  	     
      *no_match = FALSE;
    }
  /* Flag diacritics begin */
  else if ( can_be_flag_diacritics &&
	    flag_diacritic_p (low) &&
	    (attr = flag_diacritic_attrib(low)))
    {
      /* if Flag Diacritics allow then */
      old_val = flag_value(CONFIG_flag_d_reg(config), attr);
      if (flag_action (CONFIG_flag_d_reg(config), flag_diacritic(low)))
	{
	  /** warning is_epsilon_plus works on real label ids
	    not on chars ! */
	  if ( ! is_epsilon_plus_plus (up) )
	    add_branch(up, output_cell,
		       new_cur_config(config, ARC_destination(aarc)),offset);
	  else /* this is a hack we know EPSILON == 0  */
	    add_branch(EPSILON, output_cell,
		       new_cur_config(config, ARC_destination(aarc)),offset);
	  * no_match = FALSE;		      
	  /* copy flag_diacritic_register to new*/
	}
      flag_value(CONFIG_flag_d_reg(config), attr) = old_val;
    } /* Flag diacritics end */
}

#ifdef DEBUG

void TEST_reduced_to_unreduced_map(NETptr nn)
  {
    ALPHABETptr ulabs;
    ALPH_VECTORptr rum=NULL;

    rum = get_reduced_to_unreduced_map(nn);
    TEST_rum(rum);
    ulabs=get_unreduced_labels(nn, &rum);
    fsm_fprintf(stderr, "The unreduced labels alphabet (1):\n");
    print_alphabet(ulabs, 80, 8, 0);
    free_alph(ulabs); ulabs = NULL;
    free_reduced_to_unreduced_map(&rum);
    ulabs=get_unreduced_labels(nn, &rum);
    fsm_fprintf(stderr, "The unreduced labels alphabet (2):\n");
    print_alphabet(ulabs, 80, 8, 0);
    free_alph(ulabs); ulabs = NULL;
    free_reduced_to_unreduced_map(&rum);
  }
#endif


int deterministic_tokenize(NETptr token_fst, 
                           FILE *in_stream, 
                           char* string,
                           void* parseur,
						   RANGEptr* the_range_map,
                           int dont_vectorize,
                           int doubled_config_elimination_allowed) {
                           /* Produces a sequence of (special) finite-state machines (represented by
                           a tree structure), each representing the full
                           set of tokenization ambiguities of some section of the text as defined
                           by TOKEN_FST. The domain (upper side) of TOKEN_FST has the token string,
                           its range contains the surface text. The fsms can be concatenated
                           together to give the tokenization of the whole string. The fsms will
                           be acyclic if TOKEN_FST is linear bounded. Also it is guaranteed
                           that none of the tokenization paths in a given fsm will be retracted by
                           future date unless the whole text itself does not belong to the
                           range of TOKEN_FST, which cannot happen if RANGE(TOKEN_FST) is ?*.
                           Assumes that TOKEN_FST is epsilon free: no EPSILON, that is,
                           EPSILON:EPSILON transitions. (R.M.K. 23-Nov-93)
                           
                            Output may be produced at points where the alternative paths reduce
                            to one and the corresponding transducer state is final or has a final
                            state in its lower epsilon closure. If CONTINUE_FN() returns a
                            non-zero value, the output is delayed until next time or until the
                            input runs out. Output is produced by OUTPUT_FN(). (L.K. 12-01-94.)
    */
    
    STATEptr fst_state, dest;
    OUT_CELLptr output_cell;
    CONFIGptr alt, config, cur_configs, next_configs = NULL;
    int no_match,  nondet_depth = 0, nondet_depth_increased_in_previous_loop = FALSE, CAN_BE_FLAG_DIACRITICS = FALSE;
    int ssize;

    ALPHABETptr low_eps, low_fld, unreduced_lower_sigma=NULL;
    AVptr arc_vector; /* fast */
    ARCptr arc;        /* slow */
    uint16 i, fst_id_lim;
    id_type txt_id, fst_id, up, low, *fst_ids, old_val, attr;
    unsigned long int symbol_count = 0;
    PARSE_TBL parse_table ;
    char continuation=1;
    
    RANGEptr eps_range, range_map,range;
    ALPHABETptr sig = lower_sigma_tg(token_fst);
    

	MATCH_TABLEptr apply_match_table=NULL;
    RANGE_VECTORptr *rvtable=NULL ;
    
    /* BEGIN NEW for reduced labelset nets 99-3-14 */
    
    if (NET_reduced_labelset(token_fst)) {
        apply_match_table = get_match_table(token_fst, LOWER);
        rvtable = MATCH_TABLE_table(apply_match_table);
        unreduced_lower_sigma = get_restored_this_side_sigma_from_reduced(token_fst, LOWER);
        if (sigma_member(unreduced_lower_sigma, OTHER))
            sig = get_restored_sigma_from_reduced(token_fst, LOWER);
        else
        ; /** error report later: `tokenize' needs a lower sigma star language
        */
#ifdef DEBUG
        /** TEST_reduced_to_unreduced_map(token_fst); */
#endif
    }
    /* END reduced labelset */
    
    
    parse_table = get_parse_table(token_fst, LOWER);
    intern_char(&tok_bound, TOKEN_BOUNDARY_DEFAULT);
    
    if (! sigma_member(sig, OTHER))  {/* NET_labels changed to NET_sigma. 23.06.96 */    
        fprintf(stderr, "*** Tokenize Stop. The fst does not contain the OTHER symbol.\n");
        return -1;                 /* error */
    }
    
    /* Set these variables so that we can quickly check whether a text
    symbol is in the lower alphabet of the transducer.
    */
    
    
    fst_id_lim = ALPH_len(sig);
    fst_ids = ALPH_items(sig);
    if (*the_range_map==NULL)
		*the_range_map = make_range_map(token_fst, LOWER);

	range_map=*the_range_map;

#ifdef DEBUG
    /* TEST_print_range_map (token_fst, range_map); */
#endif
    
    if ( ! dont_vectorize)
        vectorize_net(token_fst);
    
    allocate_memory_det();
    
    
    cur_configs = cell_alloc(TOK_CONFIG_HEAP);
    
    CONFIG_fst_state(cur_configs) = NET_start_state(token_fst);
    
    CONFIG_output_cell(cur_configs) = NULL;
    CONFIG_next(cur_configs) = NULL;
    
    if ( (CAN_BE_FLAG_DIACRITICS = has_flag_diacritics_p (token_fst))) {
        
        NET_flag_register(token_fst) = make_flag_register(); /** needed ?  */
        CONFIG_flag_d_reg (cur_configs) = make_flag_register();
    }
    else
        CONFIG_flag_d_reg (cur_configs) = NULL;
    
    /* ** for test, debug - print character set */
    continuation=1;

    while (continuation==1) {
        
        if (string==NULL) {
            if (next_symbol(&txt_id, in_stream, parse_table, FALSE,
                FALSE, FALSE) != NO_ERROR)
                continuation=0;
        }
        else {
            if (FST_symbol_from_string(&txt_id, 
                string, 
                &string, 
                parse_table, 
                FALSE,FALSE,FALSE)!=
                NO_ERROR)
                continuation=0;            
        }
        
        if (!continuation)
            break;

        /** Flag diacritics begin */ 
        /* At a token boundary all Flag diacritics are to be reset to an
        initial status */ 
        if (CAN_BE_FLAG_DIACRITICS && is_token_boundary(txt_id)) {
            
            ; /** Reset flag diacritics registers */
        }
        /** Flag diacritics end */
        
        
        config = cur_configs;
        
        for (config = cur_configs; config; config = CONFIG_next(config)) {
            
            output_cell = CONFIG_output_cell(config);
            fst_state = CONFIG_fst_state(config);
            no_match = TRUE;
            
            /* If there are no arcs, the output state gets eliminated
            even if the fst_state is final because we are not at the
            end of text.
            */
            
            /* Now we proceed to examine the arcs of fst_state.
            The arcs are either in vector or in linked list representation.
            The algorithm is replicated according to this condition to a
            "fast" (vector) and a "slow" (linked list) part */
            
            if (STATE_vector_p(fst_state)) {
                /* fast part Start */
                arc_vector = STATE_arc_vector(fst_state);
                
                if (arc_vector) {
                    
                    
                /* If the text symbol is not in the transducer's alphabet,
                treat it as OTHER.
                    */
                    
                    if (!(txt_id < fst_id_lim && fst_ids[txt_id]))
                        range = range_map + OTHER;
                    else
                        range = range_map + txt_id;
                    
                    for (i = 0; i < RANGE_len(range); i++) {
                        
                        fst_id = RANGE_item(range, i);
                        if ((dest = AV_dest(arc_vector, fst_id))) {
                            
                            /* Match moving forward. */
                            
                            up = upper_id(fst_id);
                            
                            /* If we are matching OTHER, use the text symbol
                            itself.
                            */
                            
                            if (up == OTHER && RANGE_id(range) == OTHER)
                                up = txt_id;
                            
                                /*
                                fprintf(stdout, "Arc label-1: ");
                                print_label(fst_id, stdout);
                                fprintf(stdout, "\n");
                                
                            */
                            add_branch(up, output_cell,
                                new_next_config(&next_configs, dest, config),symbol_count);
                            
                            no_match = FALSE;
                        }
                    }
                    
                    if ((low_eps = AV_l_eps(arc_vector))) {
                        
                    /* Process arcs that have an EPSILON on the lower side.
                    Staying in the current position. Any new
                    config gets stuck on end, so eventually it will be
                    processed at this string position.
                        */
                        
                        for (i = 0; i < ALPH_len(low_eps); i++) {
                            
                            fst_id = ALPH_item(low_eps, i);
                            up = upper_id(fst_id);
                            dest = AV_dest(arc_vector, fst_id);
                            /*
                            fprintf(stdout, "Arc label-2: ");
                            print_label(fst_id, stdout);
                            fprintf(stdout, "\n");
                            */
                            add_branch(up, output_cell,
                                new_cur_config(config, dest),symbol_count);
                        }
                        no_match = FALSE;
                    }
                    /** Flag diacritics begin */
                    if (NET_l_flag_diacr(token_fst)  &&
                        (low_fld = AV_l_fld(arc_vector))) {
                        
                        for (i = 0; i < ALPH_len(low_fld); i++) {
                            
                            fst_id = ALPH_item(low_fld, i);
                            up = upper_id(fst_id);
                            low = lower_id(fst_id);
                            dest = AV_dest(arc_vector, fst_id);			
                            if ( (attr = flag_diacritic_attrib(low))) {
                                
                                old_val = flag_value(CONFIG_flag_d_reg(config), attr);
                                if (flag_action (CONFIG_flag_d_reg(config), flag_diacritic(low))) {
                                    
                                    if ( ! is_epsilon_plus_plus (up) )
                                        add_branch
                                        (up, output_cell,
                                        new_cur_config(config,dest),symbol_count);
                                    else /* this is a hack we know EPSILON == 0  */
                                        add_branch
                                        (EPSILON, output_cell, 
                                        new_cur_config(config,dest),symbol_count);
                                    no_match = FALSE;		      
                                }
                                flag_value(CONFIG_flag_d_reg(config), attr) = old_val;
                            }
                        }
                    }
                    /** Flag diacritics end */
                }
            } /* fast part end */
            
            else {/* slow part Begin: this fst state is not vectorized */
                
                  /* This is the same algorithm as its faster counterpart above
                  except that it does not convert arc chains to vectors. 
                  Consequently, it matches the id of a text symbol by going through
                  the arcs in the tokenizer transducer one by one. The fast
                  tokenizer uses array lookup for direct access. 
                  If the net has its label-set reduced then 
                */
                for (arc = STATE_arc_set(fst_state); arc; arc = next_arc(arc)) {
                    
                    /* BEGIN Reduced Labelset fst-s NEW 99-4-18 */
                    if (NET_reduced_labelset(token_fst)) {
                        
                        eps_range = RANGE_VECTOR_vector(rvtable[ARC_label(arc)]);
                        
                        if ( (txt_id < fst_id_lim && fst_ids[txt_id]) &&
                        /** bug fix 99-4-22: txt_id must be within rvtable
                        - or it is OTHER */
                        (txt_id < RANGE_VECTOR_len(rvtable[ARC_label(arc)])))
                        range = eps_range + txt_id;
                        else
                            range = eps_range + OTHER;
                        
                            /* Try to match reduced arcs and, if success, add
                        them to next_configs */
                        for (i = 0; i < RANGE_len(range); i++) {
                            
                            if (RANGE_item(range, i)) {
                                
                                match_on_arc_list(arc, RANGE_item(range, i),
                                    txt_id,
                                    output_cell, config, & next_configs, 
                                    fst_ids, fst_id_lim,
                                    CAN_BE_FLAG_DIACRITICS, & no_match, symbol_count);
                            }
                        }
                        /* Look for epsilon matches (of Reduced Labelset fst-s) */
                        for (i = 0; i < RANGE_len(eps_range); i++) {
                            
                            match_on_arc_list(arc, RANGE_item(eps_range, i), 
                                txt_id,
                                output_cell, config, & next_configs, 
                                fst_ids, fst_id_lim,
                                CAN_BE_FLAG_DIACRITICS, & no_match,symbol_count);
                        }
                    }
                    /* END Reduced Labelset fst-s */
                    else {/* normal - non-reduced labelset - fst-s */ 
                        
                        match_on_arc_list(arc, ARC_label(arc), 
                            txt_id,
                            output_cell, config, & next_configs, 
                            fst_ids, fst_id_lim,
                            CAN_BE_FLAG_DIACRITICS, & no_match,symbol_count);
                    }
                    
                }  /*  slow part End */
            }
            if (no_match) {
                
            /* This path died. Output is cleared but config with its
            fst_state remains for closure on future epsilon matches.
            NULL as output state informs add_branch() not to bother
            to add additional arcs for those matches because we already
            know there is no future.
                */
                if (config && CONFIG_output_cell(config)) {
                    
                    CONFIG_output_cell(config)->refcount--;  /* NEW 7.7.97 */
                    free_output_cell(CONFIG_output_cell(config)); /* 28.07.96 bug found in apply_transducer */
                }
                CONFIG_output_cell(config) = NULL ;
            }
        }
        
        
        /* Now all the configurations can be freed. */
        
        for (config = cur_configs; config; config = alt) {
            
            alt = CONFIG_next(config);
            /* 99-1-15. If there is an alternate config than it may come from a previous
            main loop execution. If so than now it disappears so there is less
            worry for the nondet_depth counter.
            Such situations may arrive where a couple of config-s co-exist temporarily
            - indicating non-determinism - but most of them die out since there is no
            real non-determinism. They are worrysome at the very end only.*/
            
            if (alt && nondet_depth_increased_in_previous_loop && nondet_depth)
                nondet_depth--;
            /* free_config(config); */
            free_config_and_output_cells(config);
        }
        
        if (!(cur_configs = next_configs)) {
            
        /* If Range is SIGMA*, then CUR_CONFIGS is NULL only at the
        end-of-text. If it isn't SIGMA* then we need to define
        some more client interaction, since we may have to inform
        caller that all previously-provided token-paths have
        died.
            */
            fprintf( stderr,
                "*** Unexpected termination. No configuration available.\n");
            
            break;
        }
        
        next_configs = NULL;
        
        if (!(CONFIG_next(cur_configs))) {
            
            
        /* Singleton marked as allowing incremental output: Produce some
        output and then collect the associated storage.
            */
            
            /* 	  if  ( CONFIG_output_cell(cur_configs) && 
            ( CONFIG_output_cell(cur_configs)->outid == tok_bound) )
            */
            if  ( CONFIG_output_cell(cur_configs) && 
                is_significant_begin_token_boundary (CONFIG_output_cell(cur_configs))) {
                
                CONFIG_output_cell(cur_configs)->refcount--;  /* NEW 7.7.97 */
                
                produce_tokens_det(CONFIG_output_cell(cur_configs), parseur);
                reset_for_output_det(cur_configs);
                nondet_depth = 0;
            } /* else wait for a token_boundary before producing anything */
        }
        else {
            
            if (doubled_config_elimination_allowed) /* New 99-1-18 */
                                                    cur_configs = eliminate_doubled_configs(&cur_configs); /* Try to eliminate EPSILON-identities
                                                                                                           X:0 0:X == X == 0:X X:0 
            NEW 7.7.97 */
            if (CONFIG_next(cur_configs)) {/* this is real non-deterministic ambiguity */
                
                nondet_depth = nondet_depth + count_alt_configs(cur_configs, 0);
                nondet_depth_increased_in_previous_loop = TRUE;
                if ( nondet_depth > nondet_limit) {
                    
                    fprintf(stderr, "\n*** Tokenize Error: nondet_limit = %d reached. Increase it or check the fst.\n",  nondet_depth);
                    fprintf(stderr, "\n*** Tokenize Error. START Context information (starting on a new line):\n");                
                    produce_tokens_det(CONFIG_output_cell(cur_configs), stderr);
                    fprintf(stderr, "\n*** Tokenize Error. END Context information (see \"START Context information\"  above).\n");
                    fflush(stderr);
                    exit(1);
                }
            }
            else 
                nondet_depth_increased_in_previous_loop = FALSE;
        }

        ssize=symbolsize(txt_id,parseur);
        symbol_count+=ssize;
    }
    
    produce_tail_tokens_det(cur_configs, parseur, CAN_BE_FLAG_DIACRITICS,symbol_count);
    
    free_match_table(apply_match_table);
    reclaim_memory_det();
    //free_alph(sig);
    free_alph(unreduced_lower_sigma);
    return(symbol_count);
}


/* ------- NON-DETERMINISTIC PART (former token or token-file before 1996.03) ------------ */

static void allocate_memory(void)
{
  TOK_CONFIG_HEAP = init_heap(sizeof(CONFIG), 16384/sizeof(CONFIG),
			  "Tokenizer configs");
  TOK_LINK_HEAP = init_heap(sizeof(LINK), 16384/sizeof(LINK),
			"Tokenizer links");
  TOK_OUTPUT_STACK = create_stack("Tokenizer output");
}

static void reclaim_memory(void)
{
  free_heap(TOK_CONFIG_HEAP);
  TOK_CONFIG_HEAP = NULL;

  free_heap(TOK_LINK_HEAP);
  TOK_LINK_HEAP = NULL;

  free_stack(TOK_OUTPUT_STACK);
  TOK_OUTPUT_STACK = NULL;
}

static LINKptr make_link(void)
{
  LINKptr link = cell_alloc(TOK_LINK_HEAP);
  LINK_prevstate(link) = NULL;
  LINK_refcount(link) = 0;
  return(link);
}

static void free_state_link(STATEptr state)
{
  LINKptr link = (LINKptr) STATE_client_cell(state);

  if (link)
    {
      free_cell((void *) link, TOK_LINK_HEAP);
      STATE_client_cell(state) = NULL;
    }
}

static STATEptr make_output_state(NETptr net)
{
  /* Allocates an output state, getting one from the freelist,
     if possible. The call to make_state() creates the new
     state adding it to the beginning of the state list of
     the net. The previous head of the list (if any) is
     also chained to the new one via its backlink. The
     reference count of the new state is initialized to 0.
     */

  STATEptr state = make_state(NULL, net);

  STATE_client_cell(state) = make_link();

  if (next_state(state))
    state_backlink(next_state(state)) = state;

  return(state);
}


static void delete_output_state(STATEptr state, NETptr net, int in_tail_processing)
{
  /* Takes STATE out of the state chain. Field inside state
     points to previous state in chain (NULL if the first state),
     and the next_state field points to the next state as usual.
     The state is not yet freed but the state count of the net
     is decremented because the state gets detached.
     */

  STATEptr prev = state_backlink(state),
  next = next_state(state);

  if (next)
    {
      state_backlink(next) = prev;
      next_state(state) = NULL;
    }

  if (prev)
    {
      next_state(prev) = next;
      state_backlink(state) = NULL;
    }
  else
    NET_states(net) = next;

  NET_num_states(net)--;

  if ( (!NET_states(net)) &&
       (!in_tail_processing))
    handle_error("Lost all states.", "DELETE_OUTPUT_STATE", 0);
}

static void free_output_state(STATEptr state, NETptr net, int *epsilon_count, int in_tail_processing)
{
  /* Removes all the output paths starting (or, since the machine is
     stored backwards, ending) at STATE. This works backwards through
     all zero reference-count states.
     */

  STATEptr s, dest, next, last_detached;
  ARCptr arc, next_arc;

  /* Detach the useless state from the state list but dont' free it yet. */

  delete_output_state(state, net, in_tail_processing);

  /* Now eliminate states on the path leading here whose refcount
     is zero.
     */

  last_detached = state;
  next = state;

  while (next)
    {
      s = next;

      for (arc = STATE_arc_set(s); arc; arc = next_arc)
        {
          if (ARC_label(arc) == EPSILON)
            (*epsilon_count)--;

          dest = ARC_destination(arc);
	  state_refcount(dest)--;

          if (state_refcount(dest) == 0)
            {
              /* Set up future collection of destination. Need to have access
                 to cell that links dest into output net's statelist. Strategy:
                 detach the destination state and push it  to the top of the
                 list of states to be freed.
                 */

	      if (dest != state)
		delete_output_state(dest, net, in_tail_processing);

	      next_state(last_detached) = dest;
	      last_detached = dest;
            }

          next_arc = next_arc(arc);

          free_arc(arc);
          NET_num_arcs(net)--;
        }

      next = next_state(s);
    }

  /* Now the detached states can be freed. The state count of the net
     has already been decremented.
     */
 
  for (s = state; s ; s = next)
    {
      next = next_state(s);
      free_state_link(s);
      free_state(s);
    }

}

static void merge_states(STATEptr state1, STATEptr state2)
{
  /* Move all the arcs from state1 to state2 */

  ARCptr last_arc = STATE_arc_set(state1);

  if (last_arc)
    {
      while (next_arc(last_arc))
	last_arc = next_arc(last_arc);

      next_arc(last_arc) = STATE_arc_set(state2);
  	  
      STATE_arc_set(state2) = STATE_arc_set(state1);
      STATE_arc_set(state1) = NULL;
    }
  else
    fprintf(stderr, "No arcs to move.\n");
}

static void free_arc_and_destination(ARCptr arc, STATEptr dest, NETptr net)
{
  /* Called from determinize_tail(). The arc has already been detached.
     Now detach its destination state from the net's statelist and free
     both the arc and the state.
     */

  STATEptr state, prev;

  for (state = NET_states(net), prev = NULL; state; state = next_state(state))
    if ((dest == state))
      {
	if (prev)
	  next_state(prev) = next_state(state);
	else
	  NET_states(net) = next_state(state);

	break;
      }
    else
      prev = state;

  free_state_link(dest);

  free_state(dest);
  NET_num_states(net)--;
             
  free_arc(arc);
  NET_num_arcs(net)--;
}


static NETptr make_output_net(void)
{
  NETptr net = make_network();
  NET_start_state(net) = make_output_state(net);
  return(net);
}

static void reset_for_output (NETptr *output_net, int *epsilon_count,
							  CONFIGptr config)
{
  /* Free the previous output net (if any). Create a new one and make
     it the output state of the current configuration. Reset epsilon
     count to zero.
     */

  NETptr net;

  free_network(*output_net);

  net = make_output_net();

  CONFIG_output_state(config) = NET_start_state(net);
  if (CONFIG_next(config))
    handle_warning("Unexpected alternate configuration.",
		   "RESET_FOR_OUTPUT", 0);

  *epsilon_count = 0;
  *output_net = net;
}

static void reset_config(CONFIGptr config, STATEptr state)
{
  CONFIG_output_state(config) = state;
}

static CONFIGptr make_config(STATEptr fst_state, NETptr net)
{
  CONFIGptr config = (CONFIGptr) cell_alloc(TOK_CONFIG_HEAP);
  CONFIG_fst_state(config) = fst_state;
  reset_config(config, make_output_state(net));
  CONFIG_next(config) = NULL;
  return(config);
}





static CONFIGptr find_token_config(CONFIGptr *configp, STATEptr dest,
	NETptr net)
{
  /* Returns a config in the set pointed by CONFIGP that has ARC's destination
     as its FST_STATE, possibly an empty OUTPUT_STATE. CONFIGP may be set to
     a new value if its NEXT_CONFIG with NULL as the initial value.
     */

  CONFIGptr cnfg = *configp;

  if (cnfg)
    {
      CONFIGptr next = cnfg;

      while (next)
	{
	  cnfg = next;

	  if (dest == CONFIG_fst_state(cnfg))
	    return(cnfg);

	  next = CONFIG_next(cnfg);
	}

      next = make_config(dest, net);
      CONFIG_next(cnfg) = next;
      return(next);
    }
  else
    {
      cnfg = make_config(dest, net);
      *configp = cnfg;
      return(cnfg);
    }
}


static void add_token_arc(id_type label_id, STATEptr dest, NETptr net,
			  CONFIGptr cnfg)
{
  /* Allocates a token arc and links it to the output at CNFG. Adds refcount
     to its (backwards) destination. Perhaps should search arcs at outputstate
     to make sure we aren't duplicating.
     */
  
  /* Output state is NULL if it is know to have no future, so don't add in
     that case.
     */
  
  if (CONFIG_output_state(cnfg))
    {
      state_refcount(dest)++;
      push_arc(make_arc(label_id, dest, net), CONFIG_output_state(cnfg));
    }
}


static void determinize_tail(STATEptr last_state, NETptr net)
{

  /* Minimal step towards minimization. We determinize backwards one
     step from last state to bring together paths that end with
     the same last symbol.
     */

  ARCptr arc1, arc2, prev;
  STATEptr dest1, dest2;

  for (arc1 = STATE_arc_set(last_state), prev = NULL; arc1; arc1 = arc2)
    {
      if ((arc2 = next_arc(arc1)))
	{
	  dest1 = ARC_destination(arc1);
	  dest2 = ARC_destination(arc2);

	  if (ARC_label(arc1) == ARC_label(arc2) &&
	      STATE_client_cell(dest1) && /* bug fix 29.07.96 */
	      state_refcount(dest1) == 1)
	    {
	      /* Two arcs with the same label. Merge the
		 destination state of the former with the
		 destination of the latter and detach arc1
		 */
           
	      merge_states(dest1, dest2);
           
	      /* Detach arc1. */
           
	      if (!prev)
		STATE_arc_set(last_state) = arc2;
	      else
		next_arc(prev) = arc2;
           
	      free_arc_and_destination(arc1, dest1, net);
	    }
	  else
	    prev = arc1;
	}
    }
}

static void produce_tokens(NETptr net, STATEptr last_state, int epsilon_count,
	FILE *stream, void output_fn(NETptr net, FILE *stream))
{
  /* Produce incremental output for the client, perhaps as a callback to
     the client. Ultimately this will depend on the API to the client--we
     might simply copy our local fsm strucutres into his (acyclic, if
     linear-bounded) graph structures. Here we make up a standard C fsm
     (could be constant) so that our existing fsm operations can be used
     to produce a readable transcript. TEXTPOS is used just to inform the
     client of our progress in terms that he also understands. If text
     was provided to us as a file, presumably the client also can see the
     current file position when we invoke his callback.
     */

  STATEptr state;

  STATE_final(last_state) = TRUE;

  if (epsilon_count == 0)
    NET_epsilon_free(net) = TRUE;
  else
    NET_epsilon_free(net) = FALSE;

  /* Free links */

  for (state = NET_states(net); state; state = next_state(state))
    {
      determinize_tail(state, net);
      free_state_link(state);
    }

  invert_arcs(net, 1);

  /* We could clean the machine, although it is probably not worth it.
     If we do minimize, then we also need to compute NET_labels(net)
     and NET_sigma(net). If the net is just used for output, there
     is no need to make the alphabets. Freeing the links should
     of course take place before the minimization as well, not after
     producing the output as we do here.
     */

  output_fn(net, stream);
}

static void produce_tail_tokens(NETptr net, CONFIGptr cur_configs,
				int epsilon_count, FILE *stream,
				void output_fn(NETptr net, FILE *stream))
{
  /* Produce remaining end-of-string tail tokens. We have to scan the
     remaining configs to output the results of any final epsilon
     transitions. We do it by joining the arcs of all the final-state
     configs into a single last output state then producing it.
     */

  ARCptr arc, new_arc, first_arc = NULL;
  CONFIGptr config, next, prev = NULL;
  STATEptr state, last_state = NULL,
     fst_state, output_state, dest;
  AVptr arc_vector;
  ALPHABETptr low_eps;
  uint16 i;
  id_type  fst_id, up, low;

  /* New 2000-9-29 final tail/epsilon closure was missing ?! Now done below. TG */
  for (config = cur_configs; config; config = CONFIG_next(config))
    {
      output_state = CONFIG_output_state(config);
      fst_state = CONFIG_fst_state(config);

      if (STATE_vector_p(fst_state))

	{ /* fast part Start */
	  arc_vector = STATE_arc_vector(fst_state);
	  if (arc_vector)
	    {

	      if ((low_eps = AV_l_eps(arc_vector)))
		{
		  /* Process arcs that have an EPSILON on the lower side.
		     Staying in the current position. Any new
		     config gets stuck on end, so eventually it will be
		     processed at this string position.
		     */

		  for (i = 0; i < ALPH_len(low_eps); i++)
		    {
		      fst_id = ALPH_item(low_eps, i);
		      up = upper_id(fst_id);
		      dest = AV_dest(arc_vector, fst_id);
		      /*
			fprintf(stdout, "Arc label: ");
			print_label(fst_id, stdout);
			fprintf(stdout, "\n");
			*/
		      add_token_arc(up, output_state, net,
				    find_token_config(&config, dest,
						      net));
		    }
		}
	    }
	} /* fast part end */

      /* slow part begin */

      else /* this fst state is not vectorized */
	/* This is the same algorithm as in above in "fast part" except that it
	   does not convert arc chains to vectors. Consequently, it matches
	   the id of a text symbol by going through the arcs in the tokenizer
	   transducer one by one. The fast tokenizer uses array lookup for
	   direct access.
	   */

	for (arc = STATE_arc_set(fst_state); arc; arc = next_arc(arc))
	  {
	    /*
	      fprintf(stdout, "Arc label: ");
	      print_label(txt_id, stdout);
	      fprintf(stdout, "\n");
	      */
	    up = upper_id(ARC_label(arc));
	    low = lower_id(ARC_label(arc));
		
	    if (low == EPSILON)
	      {
		/* Epsilon. Staying in the current position. Any new
		   config gets stuck on end, so eventually it will be
		   processed at this string position.
		   */
  
		add_token_arc(up, output_state, net,
			      find_token_config(&config,
						ARC_destination(arc),
						net));
	      }
	  }  
      /* slow part end */
    }

  /* Eliminate non-final configurations. */

  for (config = cur_configs; config; config = next)
    {
      next = CONFIG_next(config);

      if (STATE_final(CONFIG_fst_state(config)))
	prev = config;
      else
	{
	  if (prev)
	    CONFIG_next(prev) = next;
	  else
	    cur_configs = next;

	  if (state_refcount(CONFIG_output_state(config)) == 0)
	    free_output_state(CONFIG_output_state(config), net, &epsilon_count, TRUE);
	  free_config(config);
	}
    }

  /* If more than one final configuration remains, we have
     to copy arcs. */

  if ((config = cur_configs))
    {
      if (CONFIG_next(config))
        {
          do {
	    next = CONFIG_next(config);
	    state = CONFIG_output_state(config);

	    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	      {
		new_arc = make_arc(ARC_label(arc), ARC_destination(arc), net);
		next_arc(new_arc) = first_arc;
		first_arc = new_arc;
	      }

	    if (state_refcount(state) == 0)
	      free_output_state(state, net, &epsilon_count, TRUE);

          } while ((config = next));

	  last_state = make_output_state(net);
	  STATE_arc_set(last_state) = first_arc;
        }
      else
        last_state = CONFIG_output_state(config);
    }

  if (last_state)
    produce_tokens(net, last_state, epsilon_count, stream,
		   output_fn);
}

static int final_state_p(STATEptr state)
{
  if (STATE_final(state))
    return(TRUE);
  else
    return(FALSE);
}

static int mark_epsclosure_prop(STATEptr state, int side,
				int prop_fn(STATEptr state))
{
  switch (STATE_visit_mark(state))
    {
    case 0:		/* State processed. Doesn't have the property. */
      break;
    case 1:		/* State processed. Has the property. */
      return(TRUE);
      break;
    case 2:		/* State being processed. Don't recurse on it. */
      break;
    default:		/* Unprocessed state. */
      if (prop_fn(state))
	{
	  STATE_visit_mark(state) = 1; /* Has the property. */
    	  return(TRUE);
    	}
      else
	{
	  ARCptr arc;
    
	  /* State doesn't have the property but maybe another
	     state in its epsilon closure does.
	     */
    
	  STATE_visit_mark(state) = 2;
    
	  for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
	    if (side == UPPER)
	      {
		if (upper_id(ARC_label(arc)) == EPSILON &&
		    mark_epsclosure_prop(ARC_destination(arc), side, prop_fn))
		  {
		    STATE_visit_mark(state) = 1;
		    return(TRUE);
		  }
	      }
	    else if (lower_id(ARC_label(arc)) == EPSILON &&
		     mark_epsclosure_prop(ARC_destination(arc), side, prop_fn))
	      {
		STATE_visit_mark(state) = 1;
		return(TRUE);
	      }

	  STATE_visit_mark(state) = 0;
	}
      break;
    }

  return(FALSE);
}

static void mark_outputstates(NETptr token_fst, int text_side)
{
  /* Incremental output is produced only at states vith visitmark == 1.
     This could be based on any interesting or useful property. One
     possibility is to mark all states that have token-boundary arcs
     either leaving or entering. In this instance, we mark states
     whose lower epsilon-closure contains a final state. These are
     states at which the full text might end, so these marks tend to
     pick out positions of unconstrained concatenations of tokens.
     */

  STATEptr state;

  /* We set the visit marks initially to 3 to distinguish states
     that have not yet been processed from states that are being
     processed (mark = 2), states that already are known to have
     (mark = 1) or not to have (mark = 0) a final state in their
     epsilon closure on the text side.
     */

  prepare_visit_marks(token_fst, 3);

  for (state = NET_states(token_fst); state; state = next_state(state))
    if (STATE_visit_mark(state) == 3)
      mark_epsclosure_prop(state, text_side, final_state_p);
}


int nondeterministic_tokenize (NETptr token_fst, FILE *in_stream, FILE *out_stream,
			       int continue_fn(NETptr output_net, FILE *in, FILE *out),
			       void output_fn(NETptr net, FILE *stream),
			       int dont_vectorize)
{
  /* Produces a sequence of finite-state machines, each representing the full
     set of tokenization ambiguities of some section of the text as defined
     by TOKEN_FST. The domain (upper side) of TOKEN_FST has the token string,
     its range contains the surface text. The fsms can be concatenated
     together to give the tokenization of the whole string. The fsms will
     be acyclic if TOKEN_FST is linear bounded. Also it is guaranteed
     that none of the tokenization paths in a given fsm will be retracted by
     future date unlkess the whole text itself does not belong to the
     range of TOKEN_FST, which cannot happen if RANGE(TOKEN_FST) is ?*.
     Assumes that TOKEN_FST is epsilon free: no EPSILON, that is,
     EPSILON:EPSILON transitions. (R.M.K. 23-Nov-93)

     Output may be produced at points where the alternative paths reduce
     to one and the corresponding transducer state is final or has a final
     state in its lower epsilon closure. If CONTINUE_FN() returns a
     non-zero value, the output is delayed until next time or until the
     input runs out. Output is produced by OUTPUT_FN(). (L.K. 12-01-94.)
     */

  STATEptr fst_state, output_state, dest;
  NETptr output_net;
  CONFIGptr alt, config, cur_configs, next_configs = NULL;
  int no_match, epsilon_count = 0;
  ALPHABETptr low_eps;
  AVptr arc_vector;
  ARCptr arc;
  uint16 i, fst_id_lim;
  id_type txt_id, fst_id, up, low, *fst_ids;
  unsigned long int symbol_count = 0;
  PARSE_TBL parse_table = get_parse_table(token_fst, LOWER);
  RANGEptr range_map, range;
  ALPHABETptr sig = lower_sigma_tg(token_fst);

  /* Set these variables so that we can quickly check whether a text
     symbol is in the lower alphabet of the transducer.
     */

  fst_id_lim = ALPH_len(sig);
  fst_ids = ALPH_items(sig);

  if (! sigma_member(NET_sigma(token_fst), OTHER))
    {
      fprintf(stderr, " Stop. The fst does not contain the OTHER symbol.\n");
      exit(1);
    }

   if (has_flag_diacritics_p (token_fst))
    {
      fprintf(stderr, "*** Tokenize Stop: the fst has flag_diacritics; their handling\n\
is not yet implemented for \" -n\" (the non-deterministic variant).\n");
      exit(1);
    }

  range_map = make_range_map(token_fst, LOWER);

  mark_outputstates(token_fst, LOWER);

   if ( ! dont_vectorize)
     vectorize_net(token_fst);

  allocate_memory();

  cur_configs = cell_alloc(TOK_CONFIG_HEAP);
  output_net = make_output_net();

  CONFIG_fst_state(cur_configs) = NET_start_state(token_fst);
  CONFIG_output_state(cur_configs) = NET_start_state(output_net);
  CONFIG_next(cur_configs) = NULL;

  while (!(next_symbol(&txt_id, in_stream, parse_table, FALSE, FALSE, FALSE)))
    {
      if (txt_id == ID_NO_SYMBOL)
	{
	  fprintf(stderr, "Shouldn't happen. No symbol at position: %ld.\n",
		  symbol_count);
	  break;
	}
      symbol_count++;

      config = cur_configs;
/*  
      fprintf(stdout, "Input symbol: ");
      print_label(txt_id, stdout);
      fprintf(stdout, "\n");
*/

      for (config = cur_configs; config; config = CONFIG_next(config))
	{
	  output_state = CONFIG_output_state(config);
  	  fst_state = CONFIG_fst_state(config);
    	  no_match = TRUE;

	  /* If there are no arcs, the output state gets eliminated
	     even if the fst_state is final because we are not at the
	     end of text.
	     */
	  /* Now we proceed to examine the arcs of fst_state.
	     The arcs are either in vector or in linked list representation.
	     The algorithm is is replicated according to this condition to a
	     "fast" and a "slow" part */

	  if (STATE_vector_p(fst_state))

	    { /* fast part Start */
	      arc_vector = STATE_arc_vector(fst_state);
	      if (arc_vector)
		{

		  /* If the text symbol is not in the transducer's alphabet,
		     treat it as OTHER.
		     */

		  if (!(txt_id < fst_id_lim && fst_ids[txt_id]))
		    range = range_map + OTHER;
		  else
		    range = range_map + txt_id;

		  for (i = 0; i < RANGE_len(range); i++)
		    {
		      fst_id = RANGE_item(range, i);
		      if ((dest = AV_dest(arc_vector, fst_id)))
			{
			  /* Match moving forward. */
  
			  up = upper_id(fst_id);

			  /* If we are matching OTHER, use the text symbol
			     itself.
			     */

			  if (up == OTHER && RANGE_id(range) == OTHER)
			    up = txt_id;
  
			  if (up == EPSILON)
			    epsilon_count++;
			  /*  
					     fprintf(stdout, "Arc label: ");
					     print_label(fst_id, stdout);
					     fprintf(stdout, "\n");
					     */
			  add_token_arc(up, output_state, output_net,
					find_token_config(&next_configs, dest,
							  output_net));
  
			  no_match = FALSE;
			}
		    }

		  if ((low_eps = AV_l_eps(arc_vector)))
		    {
		      /* Process arcs that have an EPSILON on the lower side.
			 Staying in the current position. Any new
			 config gets stuck on end, so eventually it will be
			 processed at this string position.
			 */

		      for (i = 0; i < ALPH_len(low_eps); i++)
			{
			  fst_id = ALPH_item(low_eps, i);
			  up = upper_id(fst_id);
			  dest = AV_dest(arc_vector, fst_id);
			  /*
			    fprintf(stdout, "Arc label: ");
			    print_label(fst_id, stdout);
			    fprintf(stdout, "\n");
			    */
			  add_token_arc(up, output_state, output_net,
					find_token_config(&config, dest,
							  output_net));
			}
		      no_match = FALSE;
		    }
		}
	    } /* fast part end */

	  /* slow part begin */

	  else /* this fst state is not vectorized */
	    /* This is the same algorithm as in above in "fast part" except that it
		does not convert arc chains to vectors. Consequently, it matches
		the id of a text symbol by going through the arcs in the tokenizer
		transducer one by one. The fast tokenizer uses array lookup for
		direct access.
		*/

	    for (arc = STATE_arc_set(fst_state); arc; arc = next_arc(arc))
	      {
		/*
		  fprintf(stdout, "Arc label: ");
		  print_label(txt_id, stdout);
		  fprintf(stdout, "\n");
		      */
		up = upper_id(ARC_label(arc));
		low = lower_id(ARC_label(arc));
  
		if (txt_id == low ||
		    (low == OTHER && !(txt_id < fst_id_lim && fst_ids[txt_id])))
		  {
		    /* Match moving forward. */
  
		    /* If ID is an instance of OTHER and the arc label is not
		     an fstpair, use the text symbol instead.
		     */

		    if (low == OTHER && up == OTHER)
		      up = txt_id;

		    if (up == EPSILON)
		      epsilon_count++;
  
		    add_token_arc(up, output_state, output_net,
				  find_token_config(&next_configs,
						    ARC_destination(arc),
						    output_net));
  
		    no_match = FALSE;
		  }
		else if (low == EPSILON)
		  {
		    /* Epsilon. Staying in the current position. Any new
		       config gets stuck on end, so eventually it will be
		       processed at this string position.
		     */
  
		    add_token_arc(up, output_state, output_net,
				  find_token_config(&config,
						    ARC_destination(arc),
						    output_net));
  	           
		    no_match = FALSE;
		  }
	      }  
	  /* slow part end */

	  if (no_match)
	    {
	      /* This path died. Output is cleared but config with its
		 fst_state remains for closure on future epsilon matches.
		 NULL as output state informs add_token_arc() not to bother
		 to add additional arcs for those matches because we already
		 know there is no future.
		 */
  	      
	      free_output_state(output_state, output_net, &epsilon_count, FALSE);
	      CONFIG_output_state(config) = NULL;
	    }
	}

      /* Now all the configurations can be freed. */

      for (config = cur_configs; config; config = alt)
	{
	  alt = CONFIG_next(config);
	  free_config(config);
	}

      if (!(cur_configs = next_configs))
	{
	  /* If Range is SIGMA*, then CUR_CONFIGS is NULL only at the
	     end-of-text. If it isn't SIGMA*, thn we need to define
	     some more client interaction, since we may have to inform
	     caller that all previously-provided token-paths have
	     died.
	     */
	  fprintf( stderr,
		  "*** Unexpected termination. No configuration available.\n");
	  break;
	}

      next_configs = NULL;

      if (!(CONFIG_next(cur_configs) ||
	    STATE_visit_mark(CONFIG_fst_state(cur_configs)) == 0))
	{
	  /* Singleton marked as allowing incremental output.
	     Take a minimal step towards minimization by merging
	     identical arcs leading backwards from the last state.

	     Then call CONTINUE_FN() to determine whether to go on
	     or to do output now.
	     */

	  determinize_tail(CONFIG_output_state(cur_configs), output_net);

	  if (!continue_fn(output_net, in_stream, out_stream))
	    {
	      /* Singleton marked as allowing incremental output: Produce some
		 output and then collect the associated storage.
		 */
      
	      produce_tokens(output_net, CONFIG_output_state(cur_configs),
			     epsilon_count, out_stream, output_fn);
	      reset_for_output(&output_net, &epsilon_count, cur_configs);
	    }
	}
    }

  produce_tail_tokens(output_net, cur_configs, epsilon_count, out_stream,
		      output_fn);
  fflush(out_stream);

  fprintf(stderr, "\n%ld input symbols processed.\n", symbol_count);

  free_network(output_net);
  free_alph(sig);
  reclaim_memory();
  return(0);
}

#ifdef WIN32_DUPLICATE_FST
static int string_to_buffer(char *buffer, char *str, int max)
{
  int len = strlen(str);
  
  if (max <= len)
    return(-1);

  while (*str)
    *buffer++ = *str++;

  return(len);
}

int char_to_buffer(char *buffer, char ch, int max)
{
  if (max <= 1)
    return(-1);
     
  *buffer++ = ch;
  *buffer = '\0';
   
  return(1);
}


static int copy_atom_name(id_type id, char *buffer, int max,
		int eps_to_zero_p, int escape_p)
{
  if ((id == EPSILON) || (id == ALTCHAIN_SYMBOL))
    {
      if (eps_to_zero_p)
	return(char_to_buffer(buffer, '0', max));
      else
	return(0);
    }
  else if (id == OTHER)
    return(char_to_buffer(buffer, '?', max));
  else if (id == BOUNDARY_SYMBOL)
    return(string_to_buffer(buffer, ".#.", max));
  else
    return(fat_to_thin_name(buffer, id_to_atom_name(id), max, escape_p));
}



int copy_label_name(id_type id, int side, char *buffer, int max, int eps_to_zero_p, int escape_p) {

  /* Like print label except that the label is copied to a string buffer.
     MAX is the maximum number of characters.  Returns the number of characters
     copied into the buffer or -1 in the event that the buffer would
     be exceeded.
     */

  int arity = LABEL_arity(id_to_label(id));
  if (arity == 1)
    return(copy_atom_name(id, buffer, max, eps_to_zero_p, escape_p));
  else switch(side)
    {
    case UPPER:
      return(copy_atom_name(upper_id(id), buffer, max, eps_to_zero_p, escape_p));
      break;
    case LOWER:
      return(copy_atom_name(lower_id(id), buffer, max, eps_to_zero_p, escape_p));
      break;
    case BOTH_SIDES:
      {
	int i, len, total = 0;                
	id_type *items = TUPLE_labels(LABEL_tuple(id_to_label(id)));
	  
	if ((len = char_to_buffer(buffer, '<', max)) < 0)
	  return(-1);
	 
	total += len;
	  
	for (i = 0; i < arity; i++)
	  {
	    if (i > 0)
	      {
		if ((len = char_to_buffer(buffer+total, ':', max-total)) < 0)
		  return(-1);
	    	
		total += len;
	      }

	    if ((len = copy_atom_name(items[i], buffer+total, max-total, TRUE, escape_p))
		< 0)
	      return(-1);

	    total += len;
	  }
	    
	if ((len = char_to_buffer(buffer+total, '>', max-total)) < 0)
	  return(-1);
	    
	return(total + len);
      }
      break;
    default:
      handle_error("Argument neither UPPER, LOWER, or BOTH_SIDES",
		   "COPY_LABEL_NAME", side);
      return(-1);
    }
}
#endif
