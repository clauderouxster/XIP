/* $Id: subseq.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */

/*************************************************************
 *
 *  FILE  "subseq.c"
 *
 *      P-subsequentialize an fst
 *      (in the Mohri sense, with XRCE additions);
 *      Test of  subsequentibility XRCE;
 *      Local ((p-)-sub)sequentialization and 
 *      Epsilon-pushing.
 *
 *  Tamas Gaal, XEROX RESEARCH CENTRE EUROPE, Grenoble, France
 *  07.../1996-1999
 *
 *************************************************************/

/*================================================================
  Implementation of Mehryar Mohri's (sub)sequentialization algorithm
  as described in JNLE (revised July 1996).
  New XRCE results added (extensions, precisions, test), to be
  published (1999).  
================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "fst/types.h"
#include "fst/struct.h"
#include "fst/modify.h"
#include "fst/bin-in.h"
#include "fst/look.h"
#include "fst/label.h"
#include "fst/twol-net.h"
#include "fst/pars-tbl.h"
#include "fst/calculus.h"
#include "fst/net-io.h"
#include "fst/cfsm.h"
#include "fst/prompt.h"
#include "fst/fsm_io.h"
#include "fst/mark.h"

#include "fst/fsm-fst-x.h"

#include "fst/subseq.h"

#include "fst/debug.h"
#include "fst/utf8_fatstr_util.h"

#ifdef __GNUC__
static char rcs_id_subseq[] __attribute__ ((unused)) = "$Id: subseq.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $";
#endif

/* T1, a non-sequential fst gets subsequentialized in the Mohri sense
   resulting in T2 */

/* ===================================================================== 
   Basic data types and their creation/deletion. Initializations.
   ====================================================================*/

HEAPptr SQ_CONFIG_HEAP = NULL;
HEAPptr SQ_OUT_CELL_HEAP = NULL;
HEAPptr Q2_LIST_HEAP = NULL;
HEAPptr Q2_QUEL_HEAP = NULL;
HEAPptr HASH_CELL_HEAP = NULL;
HEAPptr STOP_STATES_HEAP= NULL;

/* *** TUNING! Tune heap sizes ! */

static void free_out_cell(OUT_CELLptr out)
{

  if (out)
#ifdef SQ_DEBUG_HEAP
 /* IF DEBUGGING set BREAKPOINT in heap.c at a marked line */
    free_cell_debug((void *) out, SQ_OUT_CELL_HEAP);
#else
  free_cell((void *) out, SQ_OUT_CELL_HEAP);
#endif
}
static OUT_CELLptr create_output_cell(id_type outid)
{
  /* Allocates an output state, getting one from the freelist,
     if possible. The call to create_output_cell() creates a new
     state: an OUT_CELL is requested from its HEAP
     The reference count of the new state is initialized to 0.
     */

  OUT_CELLptr new_out_cell = (OUT_CELLptr) cell_alloc(SQ_OUT_CELL_HEAP);
  if (new_out_cell == NULL)
    not_enough_memory ("ALLOC OUT_CELL");
  new_out_cell->link = NULL;
  new_out_cell->refcount = 0;
  new_out_cell->outid =  outid;
  return (new_out_cell);
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

/* --- CONFIGURATION for final w-s ----------------------------------- */

typedef struct CONFIGURATION {
  union {
    struct OUT_CELL  *cell;
    struct STATE *state;
  } output;
  struct CONFIGURATION *alt_config;
} CONFIG, *CONFIGptr;

#define CONFIG_output_cell(X)	(X)->output.cell
#define CONFIG_output_state(X)	(X)->output.state
#define CONFIG_next(X)		(X)->alt_config

static CONFIGptr make_config(OUT_CELLptr w)
{
  CONFIGptr config = (CONFIGptr) cell_alloc(SQ_CONFIG_HEAP);
  CONFIG_output_cell(config) = w;
  if (w)
    {
      assert (OC_rc(w) < SQ_MAX_REFCOUNT);
      OC_rc(w)++;
    }
  CONFIG_next(config) = NULL;
  return(config);
}

#if NOT_USED
static void free_config(CONFIGptr config)
{
  if (config)
#ifdef SQ_DEBUG_HEAP  
/* IF DEBUGGING set BREAKPOINT in heap.c at a marked line */
   free_cell_debug((void *) config, SQ_CONFIG_HEAP); 
#else
    free_cell((void *) config, SQ_CONFIG_HEAP);
#endif
}

static void free_output_cells_of_config(CONFIGptr config)
{
  OUT_CELLptr cell;

  if (config)
    {
      if ((cell = CONFIG_output_cell(config)))
	{
	  cell->refcount--;
	  free_output_cell(cell);
	}
    }
}

static void free_config_and_output_cells(CONFIGptr config)
{
  OUT_CELLptr cell;

  if (config)
    {
      if ((cell = CONFIG_output_cell(config)))
	{
	  cell->refcount--; /* refcount reflects ALL pointers (also those from
			       config-s) to this element */
	  free_output_cell(cell);
	}
#ifdef SQ_DEBUG_HEAP
      /* IF DEBUGGING set BREAKPOINT in heap.c at a marked line */  
      free_cell_debug((void *) config, SQ_CONFIG_HEAP); 
#else
      free_cell((void *) config, SQ_CONFIG_HEAP); 
#endif    
    }
}
#endif /* NOT_USED */

/* --- Q2_LIST, member of q2 ----------------------------------------*/



static Q2_LISTptr make_qlist(STATEptr fst_state)
{
  Q2_LISTptr ql = (Q2_LISTptr) cell_alloc(Q2_LIST_HEAP);
  ql->st = fst_state;
  ql->ids = NULL;
  ql->whash = 0;
  ql->next = NULL;
  return(ql);
}

static Q2_LISTptr create_qlist (STATEptr state, Q2_LISTptr prev, OUT_CELLptr w)
{
  Q2_LISTptr ql = make_qlist(state);
  QL_w (ql) = w;
  if (w)
    {
      assert (OC_rc(w) < SQ_MAX_REFCOUNT);
      OC_rc (w) ++;
    }
  QL_link (ql) = prev;
  return(ql);
}

static void free_qlist( Q2_LISTptr qlist)
{
  if (qlist)
#ifdef SQ_DEBUG_HEAP  
/* IF DEBUGGING set BREAKPOINT in heap.c at a marked line */
   free_cell_debug((void *) qlist, Q2_LIST_HEAP); 
#else
    free_cell((void *) qlist, Q2_LIST_HEAP);
#endif
}

static void free_qlists_and_ws ( Q2_LISTptr qlist)
{
  Q2_LISTptr qqll, next; 
  OUT_CELLptr w;
  for (qqll = qlist;   qqll  ;        qqll = next )
    {
      next = QL_link(qqll);
      if ( (w = QL_w (qqll)));
      {
	OC_rc(w)--;
	free_output_cell(w);
      }
      free_qlist (qqll);
    }
}

static OUT_CELLptr concat_by_copy_w (OUT_CELLptr w1, id_type id)
{
  OUT_CELLptr c1, c2 = NULL, last = NULL, oc, result = NULL;
  /* ** ? assert (w1);
   */
  if (id != EPSILON)
    c2 = create_output_cell(id);
  for ( oc =  w1 ; oc; oc = OC_l(oc))
    {
      if (OC_id(oc) != EPSILON)
	{
	  c1 = create_output_cell(OC_id(oc));
	  if (! result)
	    result = c1;
	  else
	    {
	      OC_l(last) = c1;
	      assert (OC_rc(c1) < SQ_MAX_REFCOUNT);
	      OC_rc(c1)++;
	    }
	  last = c1;
	}
    }
  if (!last)
    {
      result = c2;
    }
  else
    {
      OC_l(last) = c2;
      if (c2)
	{
	  assert (OC_rc(c2) < SQ_MAX_REFCOUNT);
	  OC_rc(c2)++;
	}
    }
  if (!result)
    result = create_output_cell(EPSILON);
  return result;
}

static OUT_CELLptr concat_by_copy (Q2_LISTptr left, id_type id)
{
  assert (left && QL_w_head(left) );
  return ( concat_by_copy_w (QL_w_head(left), id));
}

static OUT_CELLptr concat_by_copy_w1_w2 (OUT_CELLptr w1, OUT_CELLptr w2)
{
  OUT_CELLptr c1 = NULL, c2 = NULL, last = NULL, last2 = NULL, oc, result = NULL, result2 = NULL;

  for ( oc =  w1 ; oc; oc = OC_l(oc))
    {
      if (OC_id(oc) != EPSILON)
	{
	  c1 = create_output_cell(OC_id(oc));
	  if (! result)
	    result = c1;
	  else
	    {
	      OC_l(last) = c1;
	      assert (OC_rc(c1) < SQ_MAX_REFCOUNT);
	      OC_rc(c1)++;
	    }
	  last = c1;
	}
    }
 for ( oc =  w2 ; oc; oc = OC_l(oc))
    {
      if (OC_id(oc) != EPSILON)
	{
	  c2 = create_output_cell(OC_id(oc));
	  if (! result2)
	    result2 = c2;
	  else
	    {
	      OC_l(last2) = c2;
	      assert (OC_rc(c2) < SQ_MAX_REFCOUNT);    
	      OC_rc(c2)++;
	    }
	  last2 = c2;
	}
    }
 if (!last)
   {
     result = result2;
   }
 else
   {
     OC_l(last)  = result2;
     if (result2)
       {
	 assert (OC_rc(result2) < SQ_MAX_REFCOUNT);
	 OC_rc(result2)++;
       }
   }
  return result;
}

/* --- Q2_QUEL, element of Q ----------------------------------------*/

typedef struct Q2_QUEL {
  STATEptr s2; /* of T2 */
  unsigned int hash_value;
  Q2_LISTptr pairs;
  struct Q2_QUEL * next2;
} Q2_QUELtype, *Q2_QUELptr;

#define QE_st(X)            (X)->s2
#define QE_hash(X)          (X)->hash_value
#define QE_list(X)          (X)->pairs
#define QE_link(X)          (X)->next2

static Q2_QUELptr make_qel(Q2_LISTptr qlist)
{
  Q2_QUELptr qe = (Q2_QUELptr) cell_alloc(Q2_QUEL_HEAP);
  QE_st(qe) = NULL;
  QE_hash(qe) = 0;
  QE_list(qe) = qlist;
  QE_link(qe) = NULL;
  return(qe);
}

static void free_qel(Q2_QUELptr qel)
{
  if (qel)
#ifdef SQ_DEBUG_HEAP  
/* IF DEBUGGING set BREAKPOINT in heap.c at a marked line */
   free_cell_debug((void *) qel, Q2_QUEL_HEAP); 
#else
         free_cell((void *) qel, Q2_QUEL_HEAP);
#endif
}

static void free_quel_and_pairs(Q2_QUELptr q2el)
{ 
  if (q2el)
    {
      free_qlists_and_ws (QE_list(q2el));
      free_qel(q2el);
    }
}

/* --- HASH_CELL for ids (symbols) ----------------------------------- */

typedef struct HASH_CELL {
  Q2_QUELptr q2;
  struct HASH_CELL  *link;
} HASH_CELLtype, *HASH_CELLptr;

#define HC_q2(X)           (X)->q2
#define HC_l(X)            (X)->link

/* HA:  Hash Array of chosen length */

HASH_CELLptr HA = NULL;
int HA_size = 0;

/* *** TUNING! Tune heap sizes ! */

#if NOT_USED
static void free_HASH_CELL(HASH_CELLptr out)
{
  if (out)
#ifdef SQ_DEBUG_HEAP
 /* IF DEBUGGING set BREAKPOINT in heap.c at a marked line */
    free_cell_debug((void *) out, HASH_CELL_HEAP);
#else
  free_cell((void *) out, HASH_CELL_HEAP);
#endif
}
#endif /* NOT_USED */

static HASH_CELLptr create_hash_cell(Q2_QUELptr q2ptr)
{
  /* Allocates a HASH_CELL.  IT is requested from its HEAP.
     The link of the new cell is initialized to 0.
     */

  HASH_CELLptr new_HASH_CELL = (HASH_CELLptr) cell_alloc(HASH_CELL_HEAP);
  if (new_HASH_CELL == NULL)
    not_enough_memory ("ALLOC HASH_CELL");
  new_HASH_CELL->link = NULL;
  new_HASH_CELL->q2 = q2ptr;
  return (new_HASH_CELL);
}

#if NOT_USED
static void free_hash_cells(HASH_CELLptr cell)
{
  /* Supposes a linear chain backwards from cell;
     delete cell;
     Repeat it down the chain
     */

    HASH_CELLptr prev;

    while (cell)
    {
      prev = HC_l(cell);
      free_HASH_CELL(cell);
      cell = prev;
    }
}
#endif /* NOT_USED */

/* --- STOP_STATES for local sequentialization ----------------------------------- */

typedef struct STOP_STATES {
  STATEptr sst;
  STATEptr newsst;
  id_type anchor_id;
  struct STOP_STATES  *link;
} STOP_STATEStype, *STOP_STATESptr;

#define SS_st(X)           (X)->sst
#define SS_nst(X)          (X)->newsst
#define SS_id(X)          (X)->anchor_id
#define SS_l(X)            (X)->link


#ifdef DEBUG

static int even_number(int num)
     /* Is this number even? if yes, return 1, otherwise 0.
	Only 32 bit architectures! */
{
  if ( (num << 31) >> 31 )
    return FALSE;
  return TRUE;
}

static STOP_STATESptr create_stop_state_cell(STATEptr sstat);

static STOP_STATESptr single_linked_list_loop_creator(int length, int point_to)
     /* Test function to create a looping stop-states listy for subsequent testing 
      length is the length, point-to is the index from 1 (!); if 0, no looping. */
{
   STOP_STATESptr  loop=NULL, prev=NULL, this=NULL, last=NULL;
   int cnt = 0 ;
   
   cnt = length;

   while (cnt-- > 0 )
     {
       this = create_stop_state_cell((STATEptr) (int) (cnt+1));
       SS_l(this) = prev;
       prev = this;
       if (cnt == length-1)
	 last = this;
       
       if (cnt == (point_to-1))
	       loop = this;
    }

   /** if (verbose) ; **/
   SS_l(last) = loop;
   return this;
}

static STOP_STATESptr single_linked_list_loop_detector(STOP_STATESptr list, int limit, int verbose)
     /* Detect if there is a loop in a single-linked-list:
	returns the address of the loop if one found otherwise 0.
	If the LIMIT is 0 then a default should be used (but isn't, yet) - the biggest label (?).
	My clever little algorithm (TG):
	Two pointers, the second ticks half as fast as the first.
	If the pointers are equal, there is loop.
	Then the size of the loop is to be found:
	*/
{  
  int cnt_first=1, cnt_second=1, cnt_loop=0;
  STOP_STATESptr first = NULL, second = NULL, result = NULL;
  
  first = second = list;

  if (first && second)
    {

    

      while (first && second)
	{
	  cnt_first++;
	  if (limit && (cnt_first > limit) && verbose)
	    {
	      fsm_fprintf (stderr,"** single_linked_list_loop_detector: limit of %d reached so\n\
leave without finishing. **\n", limit);
	      return result;
	    }

	  first = SS_l(first);

	  if (even_number(cnt_first+1))
	    {
	      second = SS_l(second);
	      cnt_second++;
	    }

	  if (first == second)
	    {
	      result = first;
	      if (verbose)
		fsm_fprintf (stderr,"** single_linked_list_loop_detector: loop detected at step %d: %d=0x%x. **\n",
			     cnt_first, (int) first, (int) first);
	      break;
	    }
	}
      if (verbose)
	fsm_fprintf (stderr,"** single_linked_list_loop_detector: cnt_first=%d, cnt_second=%d.\n",
		     cnt_first, cnt_second);

      if (result)
	{
	  first =  SS_l(first);
	  cnt_loop = 1;

	  while ( first &&  first != result)
	    {
	      first =  SS_l(first);
	      cnt_loop++;
	    }
	  if (verbose)
	    {
	      fsm_fprintf (stderr,"** single_linked_list_loop_detector: loop size (cnt_loop)=%d;\n",
			   cnt_loop);
	      if (cnt_first && cnt_loop)
		{
		  if (cnt_first % cnt_loop)
		    fsm_fprintf (stderr, "\t (cnt_first+1)/cnt_loop=%d.\n", (cnt_first+1)/cnt_loop);
		  else
		    fsm_fprintf (stderr, "\t (cnt_first)/cnt_loop=%d.\n", cnt_first/cnt_loop);
		}
	    }
	  
	  
	}
    }
  return result;
}

void test_single_linked_list(int size, int full)
{
  int sisi, myfull=1;
  STOP_STATESptr this=NULL;

  while (size > 0 && myfull )
    {
      if (full)
	myfull = 1;
      else
	myfull = 0;

      for (sisi = size; sisi; sisi--)
	{
	  this = single_linked_list_loop_creator(size, sisi);
	  fsm_fprintf (stderr,"size = %d, looping back to %d.\n", size, sisi);
	  single_linked_list_loop_detector(this, 0, 1);
	}
      size--;
      fsm_fprintf (stderr,"\n");
    }
}
#endif

static void free_STOP_STATES(STOP_STATESptr out)
{
  if (out)
#ifdef SQ_DEBUG_HEAP
 /* IF DEBUGGING set BREAKPOINT in heap.c at a marked line */
    free_cell_debug((void *) out, STOP_STATES_HEAP);
#else
  free_cell((void *) out, STOP_STATES_HEAP);
#endif
}

static STOP_STATESptr create_stop_state_cell(STATEptr sstat)
{
  /* Allocates a STOP_STATES.  IT is requested from its HEAP.
     The link of the new cell is initialized to 0.
     */

  STOP_STATESptr new_STOP_STATES = (STOP_STATESptr) cell_alloc(STOP_STATES_HEAP);
  if (new_STOP_STATES == NULL)
    not_enough_memory ("ALLOC STOP_STATES");
  new_STOP_STATES->link = NULL;
  new_STOP_STATES->sst = sstat;
  new_STOP_STATES->anchor_id = 0;
  new_STOP_STATES->newsst = NULL;
  return (new_STOP_STATES);
}

static void free_stop_state_cells(STOP_STATESptr cell)
{
  /* Supposes a linear chain backwards from cell;
     delete cell;
     Repeat it down the chain
     */

    STOP_STATESptr prev;

    while (cell)
    {
      prev = SS_l(cell);
      free_STOP_STATES(cell);
      cell = prev;
    }
}

/* --- Q -------------------------------------------------------------*/

struct {
  Q2_QUELptr qhead;
  Q2_QUELptr qtail;
} QQU = {NULL, NULL};

unsigned int NUM_OF_Q2 = 0; 

static void add_Q_element(Q2_QUELptr q2)
{
  if (!QQU.qhead)
    {
      QQU.qhead = QQU.qtail = q2;
    }
  else
    {
      QE_link(QQU.qtail) = q2;
      QQU.qtail = q2;
    } 
  NUM_OF_Q2++;
}

/* --- Debug help ----------------------------------------------------*/

#ifdef DEBUG

static void debug_print_Q2_LISTptr(Q2_LISTptr this)
{
  OUT_CELLptr c;
  fsm_fprintf (stderr, "Eps cl T1 State: %d ", (int) QL_st(this));
  for ( c = QL_w(this);  c;  c = OC_l(c))
    {
      print_label(OC_id(c), stderr, DO_ESCAPE);
    }
  fsm_fprintf (stderr,"\n");
}

static void debug_print_Q2_LIST(Q2_LISTptr this, STATEptr thisst)
{
  Q2_LISTptr c;
  if (!this)
    return;

  fsm_fprintf  (stderr, "\n-- State and its Epsilon Closure --\n");
  debug_print_state(thisst, 0, 1);
  for ( c= this; c ; c = QL_link(c))
    {
      debug_print_Q2_LISTptr(c);
    }
   fsm_fprintf  (stderr, "-.-\n");
}

inline
static void debug_print_final_w (STATEptr st2)
{
  OUT_CELLptr c = NULL;
  CONFIGptr fin_w;
  int i;

  if (st2 
      && (int) STATE_client_cell(st2)
      && CONFIG_output_cell( (CONFIGptr) STATE_client_cell(st2)) )
    {
    fsm_fprintf (stderr,"; ");
    for (fin_w = (CONFIGptr) STATE_client_cell(st2), i=1 ;  fin_w ;  fin_w = CONFIG_next (fin_w), i++)
      {
	fsm_fprintf (stderr,"\t%d. final_w: ", i);
	for ( c =  CONFIG_output_cell(fin_w);  c;  c = OC_l(c))
	  {
	    print_label(OC_id(c), stderr, DO_ESCAPE);
	  }
	/* fsm_fprintf (stderr,"\n"); */
	fsm_fprintf (stderr,"\t%d. final_w ID-s: ", i);
	for ( c =  CONFIG_output_cell(fin_w);  c;  c = OC_l(c))
	  {
	    fsm_fprintf (stderr, "%d, ", (int) OC_id(c));
	  }
	fsm_fprintf (stderr,"\n");
      }
    }
}

inline
static void debug_print_q2 (Q2_QUELptr q2)
{
  Q2_LISTptr l;
  OUT_CELLptr c = NULL;
  /*   int i, ident; */

  if (q2)
    {
      fsm_fprintf (stderr, " q2-pointer: %d ", (int) q2);      
      if (QE_st(q2))
	{
	  /* debug_print_T2_state(QE_st(q2)); */
	  fsm_fprintf (stderr, "T2 State: %d ", (int) QE_st(q2));
	  fsm_fprintf (stderr, "hash  = %d ,", QE_hash(q2));
	  if (STATE_final(QE_st(q2)))
	    {
	      fsm_fprintf (stderr, " FINAL ");
	      debug_print_final_w (QE_st(q2));
	    }
	  else 
	     fsm_fprintf (stderr, " \n");
	}
      else 
	{
	  fsm_fprintf (stderr, "\n");
	}
      for ( l = QE_list(q2); l; l = QL_link (l))
	{
	  fsm_fprintf (stderr, "\t State in T1:  %d | ", (int) QL_st(l));
	  fsm_fprintf (stderr, "First Arc of T1 State:");
	  if (STATE_arc_set(QL_st(l)))
	    print_label(ARC_label(STATE_arc_set(QL_st(l))),  stderr, DO_ESCAPE);
	  else 
	    fsm_fprintf (stderr, "NO_LABEL");
	  fsm_fprintf (stderr, ", State no: %d \n ", (int) STATE_client_cell(QL_st(l)));
	  fsm_fprintf (stderr, "\t\t w: ");
	  for (c = QL_w(l); c ; c = OC_l(c))
	    {
	      print_label(OC_id(c) , stderr, DO_ESCAPE);
	    }
	  fsm_fprintf (stderr," | w ids: ");
	  for ( c = QL_w(l) ;  c;  c = OC_l(c))
	    {
	      fsm_fprintf (stderr, "%d, ", (int) OC_id(c));
	    }
	  fsm_fprintf (stderr,"\n");
	}
    }
  else
    {
      fsm_fprintf (stderr, "** Empty q2 ** \n");
    }
}

inline
static void debug_print_Q ()
{
 register int indent;		      
 Q2_QUELptr q;

  for ( q = QQU.qhead, indent = 0   ; q    ;  q   = QE_link (q), indent ++)
    {
      /*  for (i=0; i < indent ; i++) 
	{
	  fsm_fprintf(stderr, " ");
	  
	} */
       fsm_fprintf(stderr, "|--- ");
       fsm_fprintf(stderr, "q2 %d ", indent);
       debug_print_q2 (q);
    }
  fsm_fprintf(stderr, "*** End of Q *** \n");
  fflush(stderr);
  return;
}

void debug_print_Q_by_HA ()
{
  int i, success=0;
  HASH_CELLptr hq;

  for( i=0; i < HA_size ; i++)
    {
      if ( (int) HC_q2(&HA[i]))
	for ( hq = &HA[i]; hq; hq = HC_l(hq))
	  {
	    debug_print_q2 ( HC_q2(hq));
	    success++;
	  }
			    
    }
  fsm_fprintf(stderr, "*** End of HA q-s , success = %d ***\n", success);
  fsm_fprintf (stderr, "** HA_size = %d. \n", HA_size);
}

#endif

/* --- Memory allocation/freeing (initial) ---------------------------*/

int is_prime (int i)
{
  int j, up = 0;
  double d;
  
  d = (double) i; /* ???*/
  d = ceil (sqrt( d ));
  up = (int) (d) +1;
  for (j = 2 ; j < up ; j++)
    {
      if (! (i % j) )
	return FALSE;
    }
  return TRUE;
}

int calc_seq_hash_prime(NETptr net, int hash_factor)
{
  /* Calculates a prime close to the number of net states.
     This size will be the hash array size for the new (T2) fst.
     May be modulated with arc sizes? */
  int i, up, n;
  
  if (hash_factor)
    n = hash_factor*NET_num_states(net);
  else
    n = NET_num_states(net);

  up = 2*n; /* there must be a prime between n and 2n */
  for (i = n; i < up ; i++)
    {
      if (is_prime (i))
	return(i);
    }
  return n; 
}

int * SQ_VERBOSE = NULL;

void allocate_seq_memory(NETptr net, int hash_factor)
{
  /* 2400 */
  SQ_CONFIG_HEAP = init_heap(sizeof(CONFIG),  16384/sizeof(CONFIG),
			  "SEQ configs");

  /*  2400/sizeof(OUT_CELLtype) */
  SQ_OUT_CELL_HEAP = init_heap(sizeof(OUT_CELLtype), 16384/sizeof(OUT_CELLtype),
			"SEQ out_cells");

  Q2_LIST_HEAP = init_heap(sizeof(Q2_LISTtype),  16384/sizeof(Q2_LISTtype),
			  "SEQ Q2_LISTs");

  Q2_QUEL_HEAP = init_heap(sizeof(Q2_QUELtype),  16384/sizeof(Q2_QUELtype),
			  "SEQ Q2_QUELs");
  
  HASH_CELL_HEAP = init_heap(sizeof(HASH_CELLtype),  16384/sizeof(HASH_CELLtype),
			  "SEQ HASH_CELLs");
  if (net)
    {
      /* hash array of seq_hash_prime elements */
      HA_size = calc_seq_hash_prime(net, hash_factor);
      HA = calloc(HA_size, sizeof (HASH_CELLtype));
    }
  SQ_VERBOSE = fst_IY_VERBOSE();
}

void reclaim_seq_memory(void)
{
  free_heap(SQ_CONFIG_HEAP);
  SQ_CONFIG_HEAP = NULL;

  free_heap(SQ_OUT_CELL_HEAP);
  SQ_OUT_CELL_HEAP = NULL;

  free_heap(Q2_LIST_HEAP);
  Q2_LIST_HEAP = NULL;

  free_heap(Q2_QUEL_HEAP);
  Q2_QUEL_HEAP = NULL;

  free_heap(HASH_CELL_HEAP);
  SQ_OUT_CELL_HEAP = NULL;

  if (HA)
    {
      free(HA); 
      HA = 0;
    }
}

/* --- fsm ancillary functions ------------------------------------------*/

static void sq_normalize_sigma_and_labels_alphabet(NETptr fnet)
{
  update_label_alphabet(fnet);
  free_alph(NET_sigma(fnet));
  NET_sigma(fnet) = label_sigma(fnet);
  set_net_arity(fnet);
}

static int has_net_OTHER_to_OTHER(NETptr fnet, int normalize, int must_have_no_OTHER_in_any_other_fstpair)
{
  int i;
  id_type id, lo, up, OO=EPSILON;

  if (normalize)
    sq_normalize_sigma_and_labels_alphabet(fnet);

  if (ALPH_type(NET_sigma(fnet)) != BINARY_VECTOR)
    label_to_binary(NET_sigma(fnet));
  if (! (binary_sigma_member(NET_sigma(fnet), OTHER)))
    return FALSE;
  
  make_fstlabel(&OO, OTHER, OTHER);
  assert (OO);

  if (ALPH_type(NET_labels(fnet)) != BINARY_VECTOR)
    label_to_binary(NET_labels(fnet));
   if (binary_sigma_member(NET_labels(fnet), OO))
     {
       if(! must_have_no_OTHER_in_any_other_fstpair)
	 return TRUE;
       else
	 {
	   if (! ALPH_type(NET_labels(fnet)) ==  LABEL_VECTOR)
	     binary_to_label(NET_labels(fnet));

	   for ( i= 0 ; i < ALPH_len(NET_labels(fnet)) ; i++)
	     {
	         id = ALPH_item(NET_labels(fnet), i);
		 up = upper_id(id);
		 lo = lower_id(id);

		 if ( ((up == OTHER) && ( lo !=OTHER)) ||
		      ((lo == OTHER) && ( up !=OTHER)) )
		   return FALSE;
	     }
	   return TRUE;
	 }
     }
   return FALSE;
}

static void sq_smooth_OTHER_OTHER_to_OTHER(NETptr fnet, int normalize)
     /* Change <?:?> to <?> (OTHER) if no further ?:x or y:? is present */
{
  ARCptr ari;
  id_type OO = EPSILON;
  STATEptr sti;

  if (! has_net_OTHER_to_OTHER(fnet, normalize, TRUE))
    return;

  make_fstlabel(&OO, OTHER, OTHER);
  assert (OO);

  for (sti = NET_states(fnet) ; sti ; sti = next_state(sti))
    {
      for (ari = STATE_arc_set(sti) ; ari ; ari =  next_arc(ari))
	{
	  if ( ARC_label(ari) == OO)
	    {
	      ARC_label(ari) = OTHER;
	      STATE_deterministic(sti) = FALSE;
	      NET_deterministic(fnet)=NET_minimized(fnet)=NET_pruned(fnet)=
		NET_epsilon_free(fnet)=NET_sorted_states(fnet)=FALSE;
	    }
	}
    }
  sq_normalize_sigma_and_labels_alphabet(fnet);
}

#if NOT_USED
/* index T1 states according to their position in the linear list */
static void sq_debug_number_T1_states (NETptr T1)
{
  int i;
  STATEptr q = NULL;
  assert (T1 != 0);
  for (q = NET_states(T1), i = 0; i <  NET_num_states(T1); 
       q =  next_state(q))
    {
      STATE_client_cell(q) = (char *)  i++;
    }
  assert ( i == NET_num_states(T1));
}
#endif /* NOT_USED */

static int make_new_T2_label (id_type *new_label, id_type in_l, id_type out_l, int16 input_side)
{
  int error= NO_ERROR;

  /* ** Assert on non-Epsilon-ness of at least one ? */
  if (input_side == UPPER)
    {
      error = make_fstlabel(new_label, in_l, out_l );
      if (error != NO_ERROR)
	*new_label= ID_NO_SYMBOL;
    }
  else
    {
      error = make_fstlabel(new_label, out_l, in_l );
      if (error != NO_ERROR)
	*new_label= ID_NO_SYMBOL;
    }
  return error;
}

inline
static int is_epsilon_plus (id_type id)
{
  if (( id == EPSILON) || (id == ALTCHAIN_SYMBOL))
    return (TRUE);
  return(FALSE);
}

/* q2_from and q2_to are elements of Q. They have their representative T2 states
   too, t2_from and t2_to.
   Sigma_2 is the new output label that can comprise several symbols, like this:
   b->c->d.
   The actual input label is "a".
   We prefer to change it into the following:
   t2_from -(a:b)-> t2_new_1 -(0:c)-> t2_new_2 -(0:d)->t2_to.
   If sigma_2 has a single symbol then this simplifies into:
   t2_from -(a:b)->t2_to.
   Input side is observed.
   */
static void create_arc_or_T2states_and_arcs (Q2_QUELptr q2_from, 
					     Q2_QUELptr q2_to, 
					     int input_side,  
					     id_type input_label,
					     OUT_CELLptr sigma_2, 
					     NETptr T2)
{
  id_type iside_label = EPSILON,  oside_label = EPSILON;
  id_type nl = EPSILON;
  STATEptr stateto = NULL, statefrom;
  OUT_CELLptr s2s;

  statefrom = QE_st (q2_from);
  assert ( QE_st(q2_from) &&  QE_st(q2_to));
  iside_label = input_label;
  oside_label = OC_id (sigma_2);

  make_new_T2_label (&nl, iside_label, oside_label, input_side);
  /*  inserting "ancillary" T2 states if sigma_2 has more than one element */ 
  for ( s2s = OC_l (sigma_2) ;   s2s  ;   s2s = OC_l(s2s) )
    { 
      iside_label = EPSILON; /* from now on, input side will be 0 */
      if (iside_label || OC_id(s2s)) /* don't put 0 labelled arcs */
	{
	  stateto = make_state (NULL, T2);  
	  add_new_arc (T2, statefrom, nl, stateto);
	  statefrom = stateto;
	  make_new_T2_label (&nl, iside_label, OC_id (s2s), input_side);
	}
    }
  /* final arc */
  stateto = QE_st(q2_to);
  add_new_arc (T2, statefrom, nl, stateto);
  return;
}

char * SQ_LABELS_BUFFER = NULL;
int SQ_BU_LE = 0;

static void create_arc_multi_ch_label (Q2_QUELptr q2_from, 
				       Q2_QUELptr q2_to, 
				       int input_side,  
				       id_type input_label,
				       OUT_CELLptr sigma_2, 
				       NETptr T2,
				       char * buffer, 
				       int buf_l)
{
  id_type o_l = EPSILON, new_l = EPSILON;
  OUT_CELLptr s2s, next;
  char * bp = NULL;
  int le = 0;
  
  assert ( QE_st(q2_from) &&  QE_st(q2_to));
  if (!buffer)
    {
      fsm_fprintf (stderr,"*** Subsequentialization: intermediate w transformation: no buffer)\n");
      exit (1);
    }
  else
    bp = buffer;

  for ( s2s = sigma_2 ;   s2s  ;   s2s = next)
    { 
      next = OC_l(s2s);
      if (OC_id (s2s) == EPSILON)
	continue;
      if ( OC_id (s2s) == EPSILON)
	continue;
      le = FST_copy_label_name ( OC_id (s2s) , input_side, bp, buf_l - ( bp - buffer), FALSE, FALSE);
      if ( le < 0)
	{
	  fsm_fprintf (stderr,"*** Subsequentialization: intermediate w transformation: unsuccessful id copy\n");
	  exit (1);
	}
      bp += le;
    }
  if (bp - buffer) 
    {
      *bp = '\0' ;
      if ( ( ((int) MAX_LABEL) > ( ((int) (LIM_id_count(LABEL_MAP)) + LIM_alloc_size(LABEL_MAP)))) )
	{
	  FST_intern_literal (&new_l, buffer);
	  sigma_add_to (NET_sigma (T2) , new_l);
	}
      else
	{
	  fsm_fprintf (stderr,"\n*** Subsequentialization: too much labels: %d. Check and\n\
try 'seq-intern-arcs' and 'seq-final-arcs' ON in (x)fst.\n" , LIM_id_count(LABEL_MAP));
	  exit (1);
	}
    }
  make_new_T2_label (&o_l, input_label, new_l, input_side); 
  add_new_arc (T2, QE_st(q2_from), o_l, QE_st(q2_to));
  return;
}

static int is_w_new_label (id_type label, STATEptr state)
{
  ARCptr arc;

  for ( arc = STATE_arc_set(state); arc; arc = next_arc (arc))
    {
      if (ARC_label( arc) == label )
	return FALSE;
    }
  return TRUE;
}

/* if there is a (q1,w1) pair and q1 is final and w1 is nil then this
   q2 - its state - must remain final despite any concurring final w
   strings! */
static int q2_state_must_remain_final(Q2_QUELptr q2)
{
  int result = FALSE;
  Q2_LISTptr ql = NULL;
  
  for (ql = QE_list(q2) ; ql     ; ql = QL_link(ql))
    {
      if (STATE_final(QL_st(ql)) && QL_w_hash(ql) == 0 )
	{
	  result = TRUE;
	  break;
	}
    }
  return(result);
}

/*   w-strings (max p) of each q2 state are transformed to fsm states */
static void transform_final_w_strings_to_fsm (NETptr T2, int input_side)
{
  Q2_QUELptr q2;
  STATEptr st2, stateto = NULL, statefrom;
  CONFIGptr fin_w;
  OUT_CELLptr first_oc, oc, next;
  id_type o_l;

  int i, j, p_max = 0, w_successful = FALSE;

  for ( q2 = QQU.qhead, i=1 ; q2 ; q2 = QE_link(q2))
   {
     w_successful = FALSE;
     if ( (st2 = QE_st(q2)) && STATE_final(QE_st(q2)))
       for (fin_w = (CONFIGptr) STATE_client_cell(st2), j=1 ;  fin_w ;  fin_w = CONFIG_next (fin_w), j++)
	 {
	   if ( (first_oc = CONFIG_output_cell(fin_w)) && (OC_id (first_oc) !=  EPSILON) )
	     {
	       statefrom = st2;

	       for ( oc = first_oc; oc ; oc = next)
		 {
		   next = OC_l (oc);
		   make_new_T2_label (&o_l, EPSILON, OC_id (oc) , input_side); 
		   if (j == 1 && (oc == first_oc))
		     {
		       if ( ! is_w_new_label (o_l, statefrom))
			 {
			   fsm_fprintf (stderr,"*** Subsequentialization: Label clash: q2_i = %d", i);
			   print_label(o_l, stderr, DO_ESCAPE);
			   fsm_fprintf (stderr,"\n");
			 }
		     }
		   stateto = make_state (NULL, T2);  
		   add_new_arc (T2, statefrom, o_l , stateto);
		   statefrom = stateto;
		   if (! next)
		     STATE_final (stateto) = TRUE;
		   w_successful = TRUE;
		 }
	     }
	   if ( j > p_max )
	     p_max = j;
	 }
     if (w_successful)
       {
	 i++ ;
	 /* The Mohri-final state becomes non-final because the w
	    string got transformed to fsm arc(s) and state(s)! */

	 /* unless ther is a (FINAL_STATE_1,EMPTY_W) pair */
	 if (! q2_state_must_remain_final(q2))  
	   STATE_final (st2) = FALSE;
       }
   }
  if (SQ_VERBOSE && *SQ_VERBOSE)
    {
      fsm_fprintf(stderr, "* Subsequentialization: net p=%d p-subsequentialized from %s.\n", p_max, (input_side ? "LOWER": "UPPER"));
      if (i-1)
	fsm_fprintf(stderr, "*\tFinal output strings of %d q2 states p-subsequentialized.\n", i-1);
    }
  return;
}

static void transform_final_w_strings_to_fsm_labels(NETptr T2, int input_side, char * buffer, int buf_l) 
{
  Q2_QUELptr q2;
  STATEptr st2, stateto = NULL, statefrom;
  CONFIGptr fin_w;
  OUT_CELLptr first_oc, oc, next=NULL;
  id_type o_l, new_l ;
  char * bp = NULL;
  int le = 0;

  int i, j, p_max = 0, w_successful = FALSE;

  for ( q2 = QQU.qhead, i=1 ; q2 ; q2 = QE_link(q2))
   {
     w_successful = FALSE;
     if ( (st2 = QE_st(q2)) && STATE_final(QE_st(q2)))
       for (fin_w = (CONFIGptr) STATE_client_cell(st2), j=1 ;  fin_w ;  fin_w = CONFIG_next (fin_w), j++)
	 {
	   if ( (first_oc = CONFIG_output_cell(fin_w)) && (OC_id (first_oc) !=  EPSILON) )
	     {
	       statefrom = st2;
	       if (!buffer)
		 {
		   fsm_fprintf (stderr,"*** Subsequentialization: final w transformation: no buffer)\n");
		   exit (0);
		 }
	       else
		 bp = buffer;

	       for ( oc = first_oc; oc ; oc = next)
		 {
		   next = OC_l (oc);
		   if ( OC_id (oc) == EPSILON)
		     continue;
		   le = FST_copy_label_name ( OC_id (oc) , input_side, bp, buf_l - ( bp - buffer), FALSE, FALSE);
		   if ( le < 0)
		      {
			fsm_fprintf (stderr,"*** Subsequentialization: final w transformation: unsuccessful id copy\n");
			exit (0);
		      }
		   bp += le;
		 }
	       if (bp - buffer) 
		 {
		   *bp = '\0' ;
		   if ( ( ((int) MAX_LABEL) > ( ((int) (LIM_id_count(LABEL_MAP)) + LIM_alloc_size(LABEL_MAP)))) )
		     {
		       FST_intern_literal (&new_l, buffer);
		       sigma_add_to (NET_sigma (T2) , new_l);
		       make_new_T2_label (&o_l, EPSILON, 
						new_l,
						input_side); 

		       if (j == 1 && (oc == first_oc))
			 {
			   if ( ! is_w_new_label (o_l, statefrom))
			     {
			       fsm_fprintf (stderr,"*** Subsequentialization: Label clash: q2_i = %d", i);
			       print_label(o_l, stderr, DO_ESCAPE);
			       fsm_fprintf (stderr,"\n");
			     }
		     
			 }
		       stateto = make_state (NULL, T2);  
		       add_new_arc (T2, statefrom, o_l , stateto);
		       statefrom = stateto;
		       if (! next)
			 STATE_final (stateto) = TRUE;
		       w_successful = TRUE;
		     }
		   else
		     {
		       fsm_fprintf (stderr,"\n*** Subsequentialization: too much labels: %d. Check and\n\
try 'seq-final-arcs' and 'seq-intern-arcs' ON in (x)fst.\n" , LIM_id_count(LABEL_MAP));
	  exit (1);
	}
		 }
	     }
	   if ( j > p_max )
	     p_max = j;
	 }
     if (w_successful)
       {
	 i++ ;
	 /* The Mohri-final state becomes non-final because the w
	    string got transformed to fsm arc(s) and state(s)! */

	 /* unless there is a (FINAL_STATE_1,EMPTY_W) pair */
	 if (! q2_state_must_remain_final(q2))  
	   STATE_final (st2) = FALSE; 
       }
   }
  if (SQ_VERBOSE && *SQ_VERBOSE)
    {
      fsm_fprintf(stderr, "* Subsequentialization: net p=%d p-subsequentialized from %s.\n", p_max, (input_side ? "LOWER": "UPPER"));
      if (i-1)
	fsm_fprintf(stderr, "*\tFinal output strings of %d q2 states p-subsequentialized.\n", i-1);
    }
  return;
}

/* Mark sub-trees that certainly don't loop */
static void mark_non_looping_states_and_arcs (NETptr t1, int mark)
{
  int  mark_success = TRUE, arcs_non_looping = FALSE,
    arcno = 0, stno = 0;
  ARCptr arc1;
  STATEptr st;

  if ( ! mark)
    {
      fsm_fprintf (stderr, "*** Zero mark value in `mark_non_looping_states_and_arcs'\n");
      exit(1);
    }
  /*reset states to 0 mark */
  mark_reachable_states (NET_start_state(t1), 0, 0);

  /* if there is at least one new state mark-ed in this while loop then loop
     again on the net */ 
  while (mark_success)
    {
      mark_success = FALSE;
      for ( st =  NET_states(t1); st; st = next_state (st) )
	{
	  if ( STATE_visit_mark(st) == mark ) /* already mark-ed as non-looping*/
	    continue;
	  if (! STATE_arc_set(st)) /* no outgoing arc - no looping! */
	    {
	      STATE_visit_mark(st) = mark;
	      mark_success = TRUE;
	      stno++;
	      continue;
	    }
	  arcs_non_looping = TRUE; 

	  for (arc1 = STATE_arc_set(st); arc1 ; arc1 = next_arc (arc1))
	    {
	      if (ARC_visit_mark(arc1) == mark)  /* already mark-ed as non-looping */
		continue;
	      if (STATE_visit_mark(ARC_destination(arc1)) == mark)
		{
		  ARC_visit_mark(arc1) = mark;
		  arcno++;
		}
	      else 
		/* this state may or may not be non-looping */
		{
		  ARC_visit_mark(arc1) = 0 ; /* ** may be unnecessary */
		  arcs_non_looping = FALSE;
		}
	    } /* end of for  - 1 */
	  if (arcs_non_looping)
	    {
	      STATE_visit_mark(st) = mark;
	      stno++;
	      mark_success = TRUE;
	    }
	} /* end of outermost for - 0 */
    } /* end of while */

#ifdef SQ_DEBUG_Q
 fsm_fprintf (stderr, "**--- mark_non_looping_states_and_arcs. \
 stno = %d . arcno = %d .  ---**\n", stno, arcno);
 fsm_fprintf (stderr, "**--- All states = %ld. All arcs: = %ld \n", NET_num_states(t1), NET_num_arcs(t1) );
#endif		
}

static void reset_client_cells_to_zero (NETptr t1)
{
  STATEptr st;

  for ( st =  NET_states(t1);    st;    st = next_state (st) )
    {
      STATE_client_cell(st) = NULL;
    }
  return;
}

static void reset_visit_marks_to_zero(NETptr t1)
{
  ARCptr arc1;
  STATEptr st;
  for ( st =  NET_states(t1); st; st = next_state (st) )
    {
      STATE_visit_mark(st) = 0;
      for (arc1 = STATE_arc_set(st); arc1 ; arc1 = next_arc (arc1))
	{
	  ARC_visit_mark(arc1) = 0;
	}
    }
}

static int reset_visit_marks_to_zero_and_detect_self_loops(NETptr t1)
     /* Detect self loops. If none, at least reset (state and arc) visit marks to
	zero.
	If self-loop then returns without full cleanup ! */
{ 
  ARCptr arc1;
  STATEptr st;

  for ( st =  NET_states(t1); st; st = next_state (st) )
    {
      STATE_visit_mark(st) = 0;
      for (arc1 = STATE_arc_set(st); arc1 ; arc1 = next_arc (arc1))
	{
	  ARC_visit_mark(arc1) = 0;
	  if (ARC_destination(arc1) == st)
	    return TRUE;
	}
    }
  return FALSE;
}

void reset_visit_marks_and_client_cells_to_zero (NETptr t1)
{
  STATEptr st;
  ARCptr arc1;

  for ( st =  NET_states(t1);    st;    st = next_state (st) )
    {
      STATE_client_cell(st) = NULL;
      STATE_visit_mark(st) = 0; 
      for (arc1 = STATE_arc_set(st); arc1 ; arc1 = next_arc (arc1))
	{
	  ARC_visit_mark(arc1) = 0;
	}
    }
  return;
}

#if NOT_USED
/* checks if the state chain or or any of the arc chains self-looping, that is,
   incorrect ...*/ 
static int sq_state_arc_chain_selfcycles (NETptr t1)
{
  ARCptr arc1;
  STATEptr st;
  int result = FALSE;
  long
     q1_state_counter = 0, arc_counter, gl_arc_counter = 0;

  reset_visit_marks_to_zero(t1);
  if (*SQ_VERBOSE)
    fsm_fprintf (stderr ,"sq_state_arc_chain_selfcycles().\n");
  for ( st =  NET_states(t1); st; st = next_state (st) )
    {
      if ( STATE_visit_mark(st))
	{
	  fsm_fprintf (stderr ,"*** Fatal: State chain LOOPING on state %ld. \n", q1_state_counter);
	  return TRUE; /* BIG pb */
	}
      q1_state_counter ++;
      STATE_visit_mark(st) = 1;
      arc_counter = 0;
      for (arc1 = STATE_arc_set(st); arc1 ; arc1 = next_arc (arc1)) 
	{	 
	  if (ARC_visit_mark (arc1) )
	    {
	    fsm_fprintf (stderr ,"*** Fatal: Arc chain LOOPING on state %ld, arc %ld.\n", q1_state_counter, arc_counter);
	      return TRUE;  /* BIG pb */
	    }
	  ARC_visit_mark (arc1) = 1;
	  arc_counter ++;
	  gl_arc_counter++;
	}
    }
  if ( (NET_num_states(t1) != q1_state_counter)
       || (NET_num_arcs(t1) != gl_arc_counter))
    {
      fsm_fprintf (stderr ,"*** Fatal: (NET_num_states %ld != %ld state_counter) or \
(NET_num_arcs %ld != %ld arc_counter).\n", NET_num_states(t1),
	       q1_state_counter, NET_num_arcs(t1), gl_arc_counter);  
      return TRUE;  /* BIG pb */
    }
  return result;
}
#endif /* NOT_USED */

/*  A new fsm is created from marked states and arcs.
    Uses - CHANGES - the client cell of states. 
    */
static NETptr sq_extract_fst(NETptr old,
			     STATEptr source_state_of_old_arc,
			     ARCptr old_arc,
			     int mark, int arc_ignoremark)
{
  NETptr new = NULL;
  ARCptr oarc; 
  STATEptr ost, nst, ndest;
  
  reset_client_cells_to_zero(old);
  
  if ( ! (old_arc &&  NET_states(old)) )
    return (new);

  new = make_network(); 
  NET_labels (new) = copy_alphabet ( NET_labels (old)) ; 
  NET_sigma (new) = sigma (new);
  nst = NET_start_state(new) = make_state(NULL, new); /* nst is a NEW start  state, NOT linked to the corresponding old! */

  if (! STATE_client_cell (ARC_destination(old_arc)))
    {
      ndest = make_state(NULL, new);
      STATE_client_cell (ARC_destination(old_arc)) = ndest;
    }
  else
    {
      if ((ARC_destination(old_arc) == source_state_of_old_arc) &&
	  ( STATE_visit_mark (ARC_destination(old_arc))== mark))
	{
	  ndest = nst;
	  /*** 99-12-21 - bug ? STATE_visit_mark (ARC_destination(old_arc)) = 0; ***/
	}
      else
	{
	  fsm_fprintf (stderr , "\n\
*** SEQ INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
sq_extract_fst() found an invalid state or client cell.\n");
	  exit (1);
	}
    }
  add_new_arc (new, nst, ARC_label(old_arc), ndest);

  for ( ost =  NET_states(old);    ost;    ost = next_state (ost) )
    {
      if ((STATE_visit_mark (ost) != mark ) 
	  /*** || ( ost == source_state_of_old_arc) ****/)
	continue;
      if (! STATE_client_cell (ost))
	{
	  nst = make_state(NULL, new);
	  STATE_client_cell (ost) = nst;
	}
      else
	nst =  STATE_client_cell (ost);
      if (STATE_final (ost))
	STATE_final ( nst) = TRUE; 
      
      for ( oarc = STATE_arc_set(ost); oarc ;  oarc = next_arc(oarc))
	{
	  if (! ( STATE_visit_mark (ARC_destination(oarc))== mark ))
	    continue;
	  if (ARC_visit_mark (oarc) == arc_ignoremark)
	    continue;
	  if (! (ndest = STATE_client_cell (ARC_destination(oarc))))
	    {
	      ndest = make_state(NULL, new);
	      STATE_client_cell (ARC_destination(oarc)) = ndest;
	    }
	  if (STATE_final (ARC_destination(oarc)))
	    STATE_final ( ndest) = TRUE;  
	  add_new_arc (new, nst, ARC_label (oarc), ndest);
	  ARC_visit_mark (oarc) = 0; /* *** reset arc-visit_mark to zero ! */
	  
	}
    }

  reset_client_cells_to_zero(old);
  return (new);
}

#if NOT_USED
static void  mark_loopable_reachable_states_and_arcs (STATEptr state, 
						      int mark, 
						      int ignoremark,
						      STACKptr stack)
     /**
      * Borrowed from modify.c:mark_reachable_states() and modified.
      * Marks all states reachable from STATE with MARK 
      * except those marked by ignoremark. 
      * Uses STACK if provided, if not, creates a stack.
      *
      **/
{       
  ARCptr cur_arc = NULL;
  int stack_is_local = FALSE;
  
  if (stack == NULL)
    {
      stack = create_stack("States to reach");
      stack_is_local = TRUE;
    }
  
  for(;;)
    {
      if ((STATE_visit_mark(state) != mark)
	  && (STATE_visit_mark(state) != ignoremark)) 
        {
          STATE_visit_mark(state) = mark;

          cur_arc = STATE_arc_set(state);
	  if (cur_arc)
	    while ( ARC_visit_mark (cur_arc) == ignoremark)
	      {
		cur_arc = next_arc(cur_arc);
		if (! cur_arc)
		  continue;
	      }
        }
      
      if (cur_arc == NULL)
        {
          if (STACK_depth(stack) > 0)
            cur_arc = pop(stack);
          else
            break;
        }
      
      /* ***
      if (next_arc(cur_arc) != NULL)
	{
	  
	  push(next_arc(cur_arc), stack);
	}
	*** */
      while (next_arc(cur_arc) != NULL)
	{
	  if (ARC_visit_mark (next_arc(cur_arc)) == ignoremark)
	    cur_arc = next_arc(cur_arc);
	  else
	    {
	      ARC_visit_mark (cur_arc) = mark;
	      push(next_arc(cur_arc), stack);
	      cur_arc = next_arc(cur_arc);
	      continue;
	    }
	}

      state = ARC_destination(cur_arc);
      cur_arc = NULL;
    }
  
  if (stack_is_local == TRUE)
    free_stack(stack);
}
#endif /* NOT_USED */

static void mark_reachable_nottoignore_states(STATEptr state, 
					      int mark, 
					      int ignoremark,
					      STACKptr stack)
     /**
      * modify.c:mark_reachable_states()
      * Marks all states reachable from STATE with MARK.
      * Uses STACK if provided, if not, creates a stack.
      *
      **/
{       
  ARCptr cur_arc = NULL, real_cur_arc = NULL ;
  int stack_is_local = FALSE;
  
  if (stack == NULL)
    {
      stack = create_stack("States to reach");
      stack_is_local = TRUE;
    }
  
  for(;;)
    {
      if ( (STATE_visit_mark(state) != mark)
	  && (STATE_visit_mark(state) != ignoremark))
        {
          STATE_visit_mark(state) = mark;
          cur_arc = STATE_arc_set(state);

	  while ( ARC_visit_mark (cur_arc) == ignoremark)
	      {
		cur_arc = next_arc(cur_arc);
		if (! cur_arc)
		  continue;
	      }
        }
      
      if (cur_arc == NULL)
        {
          if (STACK_depth(stack) > 0)
            cur_arc = pop(stack);
          else
            break;
        }               

/* **     
      if (next_arc(cur_arc) != NULL)
        push(next_arc(cur_arc), stack);
 */  

      real_cur_arc = cur_arc;
      while (next_arc(cur_arc) != NULL)
	{
	  if (ARC_visit_mark (next_arc(cur_arc)) == ignoremark)
	     cur_arc = next_arc(cur_arc);
	  else
	    {
	      push(next_arc(cur_arc), stack);
	      break;
	    }
	}
      cur_arc = real_cur_arc ;

      state = ARC_destination(cur_arc);
      cur_arc = NULL;
    }
  
  if (stack_is_local == TRUE)
    free_stack(stack);
}

#if NOT_USED
static STATEptr mark_if_no_clash_reachable_nottoignore_states(STATEptr state, 
							      int mark, 
							      int ignoremark,
							      int clashmark,
							      STACKptr stack)
/**
      * modify.c:mark_reachable_states()
      * Marks all states reachable from STATE with MARK.
      * Uses STACK if provided, if not, creates a stack.
      *
      **/
{       
  ARCptr cur_arc = NULL, real_cur_arc = NULL ;
  int stack_is_local = FALSE;
  STATEptr result = NULL;
  
  if (stack == NULL)
    {
      stack = create_stack("States to reach");
      stack_is_local = TRUE;
    }
  
  for(;;)
    {
      if (STATE_visit_mark(state) == clashmark)
	return ( state);

      if ( (STATE_visit_mark(state) != mark)
	  && (STATE_visit_mark(state) != ignoremark))
        {
          STATE_visit_mark(state) = mark;
          cur_arc = STATE_arc_set(state);

	  while ( ARC_visit_mark (cur_arc) == ignoremark)
	      {
		cur_arc = next_arc(cur_arc);
		if (! cur_arc)
		  continue;
	      }
        }
      
      if (cur_arc == NULL)
        {
          if (STACK_depth(stack) > 0)
            cur_arc = pop(stack);
          else
            break;
        }               

/* **     
      if (next_arc(cur_arc) != NULL)
        push(next_arc(cur_arc), stack);
    
 */  

      real_cur_arc = cur_arc;
      while (next_arc(cur_arc) != NULL)
	{
	  if (ARC_visit_mark (next_arc(cur_arc)) == ignoremark)
	     cur_arc = next_arc(cur_arc);
	  else
	    {
	      push(next_arc(cur_arc), stack);
	      break;
	    }
	}
      cur_arc = real_cur_arc ;

      state = ARC_destination(cur_arc);
      cur_arc = NULL;
    }
  
  if (stack_is_local == TRUE)
    free_stack(stack);
  return result;
}

static void make_start_state_nonfinal (NETptr net)
/* the net is transformed into an equivalent one where the start state is non-final.
   So epsilon words are excluded */
{
  STATEptr nsst;

  if ( ( ! NET_start_state_final (net)) &&( !STATE_final(NET_start_state(net))))
    return ;
  nsst = copy_state ( NET_start_state(net), net);
  STATE_final(nsst) = FALSE;
  NET_start_state(net) = nsst;
  NET_start_state_final(net) = FALSE;
  return;
}
#endif /* NOT_USED */

/* Before performing sequentialization, a check is to be made whether the net is
   (p-sub-)sequentiable at all.
   The net is traversed and ambigous loops (with differing outputs) are reported as
   forbidding sequentialization. 
  
   Algorithm:

   1. Find a first state.
   Loop on it (provided they are chained in a list; they are.)

   2. Find a not yet visited (Mohri-type, sequentially) ambiguous state,
   s_i, that is, one with two or more (say, k) identical input labels. Mark
   it as "visited and possibly ambiguous".

   3. Start a traversal path here on the first arc of s_i labeled by the
   ambiguous label, x. Try to get back to this state, s_i,  by exploring ALL
   (???) paths. 
   First go to the destination of the first ambiguous label. Mark it as
   "visited and possibly ambiguous". 
   If there is no outgoing arc at a state, mark it as "visited", simply;
   If the "source" is "visited and ambiguous" and it has no other
   destination, mark it as "visited".
   Repeat the procedure.
   If unsuccessful for at least (k-1) ambiguous arcs of this state,
   go to 2.
   If successful (that is, looping) in at least 2 cases, check whether there
   is any common input acceptable word for both paths as sub-fst-s. It may
   involve looping, worst case : (k x m) where k and m are the respective
   path lengths.
   If there is such a common word, then it is a candidate for non-determinism:
  
   4. the paths are then compared whether respective outputs are different or not; if
   not, then they can just be conceived as the same path (no real ambiguity)!
   ( *** Paths should be traced to be reported?)

   If the output paths are different then
   report non-determinism and stop.
 
   Else Go to 2.
   */


static int looping_fst (NETptr t1)
{
  int result = FALSE;
  STATEptr  st; 
  ARCptr arc;

  if (reset_visit_marks_to_zero_and_detect_self_loops (t1))
    return TRUE;

  for (st = NET_states(t1) ; st ; st = next_state (st))
    {
      reset_visit_marks_to_zero (t1);
      for (arc = STATE_arc_set (st); arc; arc = next_arc(arc))
	{
	  mark_reachable_states(ARC_destination(arc), 1, 0);
	}
      if (STATE_visit_mark(st) == 1)
	return TRUE;
    }
  return result;
}

typedef struct RES_Q2l {
  Q2_LISTptr head;
  Q2_LISTptr tail;
} RES_Q2ltype, *RES_Q2lptr;

static RES_Q2ltype epsilon_closure (STATEptr st, OUT_CELLptr inh_w,
				    id_type (* iside_id) (id_type id), 
				    id_type (* oside_id) (id_type id),
				    STATEptr principal_state);

static ARCptr next_arc_of_eps_closured_state(STATEptr pst, Q2_LISTptr *ec, Q2_LISTptr *actual_ec, ARCptr arc, id_type * init_id);
#define  SQ_SMALL_NET_SIZE 100

static int sq_null_fsm_p (NETptr net)
{
  /* NULLFSMP returns FALSE if NET has a structure more than a null_fsm */
  
  STATEptr state;

  if (NET_num_arcs(net))
    return FALSE;

  for (state = NET_states(net); state != NULL; state = next_state(state))
    if (STATE_final(state))
      return(FALSE);
  
  return(TRUE);
}

static int detect_state_epsilon_loops(STATEptr sourcest, STATEptr actst, id_type (* iside_id) (id_type id))
     /* Detect (and report TRUE) if a state - sourcest - can be reached by a:0 (if side
	is LOWER) or 0:a (if side is UPPER) type arcs, that is, by a sequence of
	(looping) "epsilon closure" arcs; or if any state on its any "epsilon closure"
	path has such an arc. */ 
{
  ARCptr tarc;

  assert (sourcest && actst);

  if (STATE_visit_mark(actst) == 1)
    return TRUE;
    
  STATE_visit_mark(actst) = 1;

  for (tarc = STATE_arc_set(actst) ; tarc ; tarc = next_arc(tarc))
    {
     
       if (( iside_id (ARC_label( tarc)) == EPSILON) &&
	    (ARC_visit_mark (tarc) != 1))
	 {
	   ARC_visit_mark (tarc) = 1;
	   if ( ARC_destination(tarc) == sourcest)
	     {
	       return TRUE;
	     }
	     else
	       {
		 if (detect_state_epsilon_loops(sourcest, ARC_destination(tarc), iside_id))
		   return TRUE;
	       }
	    ARC_visit_mark (tarc) = 0;
	 }
    }
  
  STATE_visit_mark(actst) = 0;
  return FALSE;
}

static int detect_net_epsilon_loops(NETptr nett, id_type (* iside_id) (id_type id))
     /*
       Detect if there is a loop with epsilons only on the given side of "nett".
       */
{
  int q1_state_counter = 0;
  STATEptr nst;
 
  reset_visit_marks_and_client_cells_to_zero(nett);

  for (nst = NET_states(nett) ; nst ; nst = next_state(nst))
    {
      q1_state_counter ++;
      if (detect_state_epsilon_loops(nst, nst, iside_id))
	return TRUE;
    }
  return FALSE;
}

static ARCptr find_this_half_epsilon_arc(STATEptr sst, id_type id, int side)
     /* 
	Find the arc that has "id" on the other (!) side and EPSILON on this (!)
	"side". This is useful when we want to mark an epsilon-closured state: 
	 Example: 
	            regex a | A:0 B:0 C:0 0:a 0:b 0:c ;
	 Here, if we are on the LOWER side, the  next_arc_of_eps_closured_state()
	 will give the  0:a arc first (for the longer chain) but we may have to
	 know the initial arc (which is labeled by A:0 ) from where the closure
	 computation started.

	A deterministic (in the automata sense) net is expected (without double
	sided EPSILON arcs).	
	*/   
{
  ARCptr aarc;
  id_type nl=0;

  if (! (id && sst))
    return NULL;
  else
    make_new_T2_label(&nl, EPSILON, id, side);
 
  for ( aarc = STATE_arc_set(sst) ; aarc ; aarc = next_arc(aarc) )
    {
      if  (ARC_label( aarc) == nl )
	return aarc;
    }
  return NULL;
}

#if NOT_USED
static void recompute_net_states_and_arcs(NETptr fnet)
{
  ARCptr arc1;
  STATEptr st;
  int  stss=0, arcss=0;

  if (!fnet)
    return;

  for ( st =  NET_states(fnet), stss=0, arcss=0; st; st = next_state (st), stss++ )
    {
        for (arc1 = STATE_arc_set(st); arc1 ; arc1 = next_arc (arc1), arcss++ )
	  ;
    }
  if (NET_num_states(fnet) != stss)
    NET_num_states(fnet) = stss;
  if (NET_num_arcs(fnet) != arcss)
    NET_num_arcs(fnet) = arcss;
}

static void sq_kind_of_prune_nonfinal_fst(NETptr fnet)
     /* The standard c-fsm maintenance functions, min_fsm() and	prune_fsm()
	need final states but we need a structure-preserving but otherwise
	determinizing and "pruning" fst that works even if the fst does not 
	have any final state at all!
	*/ 
{
  if (fnet)
    {
      NET_deterministic(fnet)=NET_minimized(fnet)=NET_pruned(fnet)=
	NET_epsilon_free(fnet)=NET_sorted_states(fnet)=FALSE;
      NET_visit_marks_dirty(fnet) = TRUE;
      update_label_alphabet(fnet);
      determinize_fsm(fnet,  DONT_COPY);
	/*** epsilon_remove(fnet, DONT_COPY); */
      make_connected(fnet, FALSE);
      recompute_net_states_and_arcs(fnet);
    }
}
#endif /* NOT_USED */

static int is_fst_sequentiable_7 (NETptr t1, int input_side)
{
  ARCptr arc1, arc2, arc3, harc1=0, harc2=0;
  int  n1_looping=FALSE, n2_looping=FALSE, n3_looping=FALSE, netsize=0, result=TRUE,
    q1_state_counter=0
#ifdef DEBUG
    , q1_arc_counter
#endif
    ;
  id_type (* iside_id) (id_type id),
    (* oside_id) (id_type id),
    i1, i2, hi1=0, hi2=0, hi3=0; 
  NETptr nets[2], other_side_nets[2],
    n1=NULL, n2=NULL, n3=NULL, n1_other_side=NULL, n2_other_side=NULL,
    n3_other_side=NULL,
    (* this_side_fsm) (NETptr net, int copy_p, int minimize_p),
    (* other_side_fsm) (NETptr net, int copy_p, int minimize_p) ;
  NVtype nv, nv_other_side;
  Q2_LISTptr eps_c, ec1, ec2, ec3, act_ec_1, act_ec_2, act_ec_3;
  STATEptr  st, dst1, dst2;
  void * save_st_client_cell; 
  
  nv.nets = nets;
  nv.len = 2;
  nv_other_side.nets = other_side_nets;
  nv_other_side.len = 2;

  if (input_side == UPPER) /* direction is l-r (==a) at  a:b */
    {
      iside_id = upper_id;
      oside_id = lower_id;
      this_side_fsm = upper_fsm_no_compact;
      other_side_fsm = lower_fsm_no_compact;
    } 
  else 
    {
      oside_id = upper_id;
      iside_id = lower_id;
      this_side_fsm = lower_fsm_no_compact;
      other_side_fsm = upper_fsm_no_compact;
    }

  if (detect_net_epsilon_loops(t1, iside_id))
    {
      if (fst_IY_VERBOSE())
	fsm_fprintf (stderr,"** Not sequentiable: Epsilon loop detected on the %s\
 side.\n", (( input_side) ? "LOWER":"UPPER"));
      return FALSE;
    }

  reset_visit_marks_to_zero (t1);
  /* First, prune all subtrees that surely won't loop any way.
     So the search space will be diminished */
  mark_non_looping_states_and_arcs ( t1, 2);

  /* check only ambiguous states, that is, the ones with at least two arcs with
     identical input labels :
     1. Is the state ambiguous ? If not, see next. If there is no more state and
     there was not any ambiguous looping one with at least 2 different paths
     allowing the same input word then this network may be sequentiable; otherwise
     it is not.
     The next condition is that if a loop can be reached by two different ambiguous
     paths from a state then it is not subsequentiable: delayed output length won't
     be predictable.
     2. For all pairs of ambiguous and looping paths (arcs) do:
     3. Mark them so that respective reachable subnets (first, second) of pairs can be extracted;

     4. check if looping on both subnets;
        if yes; intersect them.
        if the intersection has a loop - NOT subsequentiable!
        Report it and stop/exit ?
     */
  netsize = NET_num_states(t1);

#ifdef SQ_DEBUG_Q
  fsm_fprintf (stderr ,"is_fst_sequentiable_7() : ");
#endif

  for ( st =  NET_states(t1)
#ifdef DEBUG
	  , q1_arc_counter = 0
#endif
	  ; st; st = next_state (st) )
    {
      q1_state_counter ++;
      if ((*SQ_VERBOSE) && ( netsize > SQ_SMALL_NET_SIZE) && (q1_state_counter % (netsize/10) == 0))
	fsm_fprintf (stderr , "%d%%", ((q1_state_counter*SQ_SMALL_NET_SIZE)/netsize));

      if (st != NET_states(t1))
	/* ignore those marked by 2 */
	mark_reachable_nottoignore_states(st, 0, 2, 0);
      /* ignore non-looping-states (subtrees that would not ever loop) */
      if ( STATE_visit_mark(st) == 2)
	continue;

      STATE_client_cell(st) = (epsilon_closure (st, NULL, iside_id, oside_id, st)).head ;
      eps_c = (Q2_LISTptr) STATE_client_cell(st);

      for (arc1 = STATE_arc_set(st), ec1 = ec2 = eps_c, hi1=0, act_ec_1=0 ;   arc1 ; 
	   arc1 = next_arc_of_eps_closured_state( st, &ec1,  &act_ec_1, arc1, &hi1), dst1 = NULL
#ifdef DEBUG
	     , q1_arc_counter++
#endif
	   )
	{
	  i1 = iside_id (ARC_label( arc1));

	  if (i1 != EPSILON)
	    {
	      ec2 = ec1;
	      act_ec_2 = act_ec_1;
	      hi2 = 0;

	      for (arc2 = next_arc_of_eps_closured_state(st, &ec2, &act_ec_2, arc1, &hi2);  arc2 ;
		   arc2 = next_arc_of_eps_closured_state(st, &ec2, &act_ec_2, arc2, &hi2))
	      {
		/* assert (arc2 != arc1); -- 99-2-16: too strict: it can happen if eps.cl. gets
		                                      to the same state on two different paths! */
		assert (! (arc2 == arc1 && ec2 == ec1));
		i2 = iside_id (ARC_label( arc2));
		if (i2 == EPSILON)
		  continue;

		if ( i1 == i2 )
		  {
		    /* AMBIGUITY found */
		    /* set marks of all arcs of this node to "ignore" except for this arc1 */
		    ec3 = eps_c;
		    for (arc3 = STATE_arc_set(st), act_ec_3 = 0 ;  arc3 ;
			 arc3 = next_arc_of_eps_closured_state(st,  &ec3, &act_ec_3, arc3, &hi3))
		      {	
			 if ( (ARC_visit_mark (arc3) != 2))
			   {
			     if ( ! (arc3 == arc1))
			       ARC_visit_mark (arc3) = 3;
			     else
			       ARC_visit_mark (arc3) = 0;
			   }
		      }

		    /** NEW 99-1-5 
		      If this arc1 (!) is not a direct arc of this state but
		      comes from an epsilon closure - perhaps through several
		      indirections - then the "head arc" (at this state) of the
		      relevant epsilon closure is to be found and marked for
		      subsequent correct subnet extraction. */
		    if (hi1 && (harc1=find_this_half_epsilon_arc(st, hi1, input_side)))
			 ARC_visit_mark (harc1) = 1;
		    else
		      harc1 = arc1;

		    dst1 = ARC_destination(harc1);
		    if ((! dst1))
		      {
		        fsm_fprintf (stderr , "\n\
*** SEQ INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
No destination state.\n");
			exit(1);
		      }
		    else
		      mark_reachable_nottoignore_states(dst1, 1, 2, NULL);
		    save_st_client_cell = STATE_client_cell(st);
		    ARC_visit_mark (harc1) = 1;
		    n1 = sq_extract_fst (t1, st, harc1, 1, 2);
		    STATE_client_cell(st) =  save_st_client_cell; 
		    n1_other_side = copy_fsm (n1);
		    n1 = this_side_fsm (n1, DONT_COPY, DONT_MINIMIZE);
		    /* check if n1 loops or not. if not, ok (leave) if yes, proceed */
		    n1_looping = looping_fst (n1);
		    if (n1_looping) /* go and explore n2, too */
		      {
			/* set marks of all arcs of this node to "ignore" except for this arc2 */
			ec3 = eps_c;
			for (arc3 = STATE_arc_set(st), act_ec_3 = 0 ;  arc3 ;
			     arc3 = next_arc_of_eps_closured_state(st,  &ec3, &act_ec_3, arc3, &hi3))
			  {
			    if ( (ARC_visit_mark (arc3) != 2))
			      {
				if ( ! (arc3 == arc2))
				  ARC_visit_mark (arc3) = 3 ; 
				else
				  ARC_visit_mark (arc3) = 0;
			      }
			  }

		    /** NEW 99-1-5 
		      If this arc2 (!) is not a direct arc of this state but
		      comes from an epsilon closure - perhaps through several
		      indirections - then the "head arc" (at this state) of the
		      relevant epsilon closure is to be found and marked for
		      subsequent correct subnet extraction. */
			if (hi2 && 
			    (harc2=find_this_half_epsilon_arc(st, hi2, input_side)))
			  ARC_visit_mark (harc2) = 3;
			else
			  harc2 = arc2;

			dst2 = ARC_destination(harc2);
			if ((! dst2))
			  {
			    fsm_fprintf (stderr , "\n\
*** SEQ INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
No destination state.\n");
			    exit(1);
			  }		 
			else
			  mark_reachable_nottoignore_states (dst2, 3, 2, NULL);
			save_st_client_cell = STATE_client_cell(st);
			ARC_visit_mark (harc2) = 3;
			n2 = sq_extract_fst (t1, st, harc2, 3, 2);
			STATE_client_cell(st) =  save_st_client_cell;
			n2_other_side = copy_fsm (n2);
			n2 = this_side_fsm (n2, DONT_COPY, DONT_MINIMIZE);
			/* check if n2 loops or not. if not, ok (leave) if yes, proceed */
			n2_looping = looping_fst (n2);
			if ( n1_looping && n2_looping)
			  {
			    /*** sq_kind_of_prune_nonfinal_fst(n1);
			    sq_kind_of_prune_nonfinal_fst(n2); **/
			    nets[0] = n1; 
			    nets[1] = n2;
			    n3 = intersect_fsm (&nv, DONT_RECLAIM, DONT_MINIMIZE );
			    /*** sq_kind_of_prune_nonfinal_fst(n3); **/
			    n3_looping = looping_fst (n3);
			    if (n3_looping )
			      {
				n1_other_side = other_side_fsm(n1_other_side, DONT_COPY, DONT_MINIMIZE);
				n2_other_side = other_side_fsm(n2_other_side, DONT_COPY, DONT_MINIMIZE);
				/*** sq_kind_of_prune_nonfinal_fst(n1_other_side);
				sq_kind_of_prune_nonfinal_fst(n2_other_side); **/
				other_side_nets[0] = n1_other_side;
				other_side_nets[1] = n2_other_side;
				n3_other_side = intersect_fsm (&nv_other_side, DONT_RECLAIM, DONT_MINIMIZE);
				/***
				  sq_kind_of_prune_nonfinal_fst(n3_other_side); **/
				
				/* If the intersection doesn't exist (?) or is a null fsm or it is an
				   arc-less but final fsm (a single accepting state) and any of its 
				   origins is more than that then we have non-sequentiability ! */
				if ( (! n3_other_side) ||
				     sq_null_fsm_p(n3_other_side)||
				     ( (! NET_num_arcs(n3_other_side)) && (NET_num_arcs(n2_other_side) ||
									   NET_num_arcs(n1_other_side))))				       
				  {
#ifdef SQ_DEBUG_Q
				    fsm_fprintf (stdout, "**--- Test 5 of Subseq. intersection of n1, n2. n3 = %d .---\n", n3_looping);
				/*  Next print_net()-s changes client_cells of the nets ! */
				    fsm_fprintf (stdout, "   %s   n1:  \n",(input_side ? "LOWER": "UPPER"));
				    print_net_s(n1, 80, stdout);
				    fsm_fprintf (stdout, "    %s  n2:  \n",(input_side ? "LOWER": "UPPER"));
				    print_net_s(n2, 80, stdout);
				    fsm_fprintf (stdout, "    %s  n3:  \n",(input_side ? "LOWER": "UPPER"));
				    print_net_s(n3, 80, stdout);
				    fsm_fprintf (stdout, "**--- --- --- --- ---\n");
				    fsm_fprintf (stderr ,"\n");
#endif
				    free_network (n1); free_network (n2); free_network (n3);
				    free_network (n1_other_side); free_network (n2_other_side); free_network (n3_other_side);
				    return (FALSE);
				  }
			      }
			  }
			/* reset arc marks - DANGEROUS  */
			ec3 = eps_c;
			arc3 = NULL;
			if (ARC_visit_mark (harc1) != 2)
			    ARC_visit_mark (harc1) = 0;
			if (ARC_visit_mark (harc2) != 2)
			    ARC_visit_mark (harc2) = 0;
			for (arc3 = STATE_arc_set(st), act_ec_3 = 0 ;  arc3 ;
			     arc3 = next_arc_of_eps_closured_state(st, &ec3, &act_ec_3, arc3, &hi3))
			  {
			    if ( (ARC_visit_mark (arc3) != 2))
			      ARC_visit_mark (arc3) = 0 ; 
			  }
		      }
		    /* reset */
		    free_network (n1); free_network (n2); free_network (n3);
		    free_network (n1_other_side); free_network(n2_other_side); free_network(n3_other_side);
		    n1 = n2 = n3 = NULL; 
		    n1_other_side=n2_other_side=n3_other_side = NULL; 
		    nets[0] = nets[1] = NULL;
		    other_side_nets[0] = other_side_nets[1] = NULL;
		  }
	      }
	    }
	} 
    }
#ifdef SQ_DEBUG_Q
  fsm_fprintf (stderr ,"\nYes.\n");
#endif
  if ( (*SQ_VERBOSE) && netsize > SQ_SMALL_NET_SIZE)
    fsm_fprintf (stderr ,"\n");
  return result;
}

int sq_is_fst_sequentiable (NETptr t1, int input_side)
{
  int result = FALSE;
  allocate_seq_memory( NULL, 0 ); /* no need for the hash array here */
#ifdef SQ_DEBUG_Q
  if ( sq_state_arc_chain_selfcycles (t1))
    exit (0);
#endif
  if (! is_fst_sequentiable_7 (t1, input_side))
    {
      if (*SQ_VERBOSE)
   	fsm_fprintf (stderr , "*** Not ");
    }
  else
    {
      result = TRUE;
      if (*SQ_VERBOSE)
      	fsm_fprintf (stderr , "* Yes, ");
    }
  if (*SQ_VERBOSE)
    fsm_fprintf (stderr , "subsequentiable from the %s direction. \n",
		     (input_side ? "LOWER": "UPPER"));
  reclaim_seq_memory();
  return result;
}

/* --- ancillary functions ----------------------------------------------
   ----------------------------------------------------------------------*/

/* Stepping over (but considering via their w strings)  epsilons in the
   possible follower-destination set of this T1 state */ 
/* epsilon_closure() is recursive - so depth of its application is limited... */

/* explore a downward tree in 2 different ways, recursively, and
   use both inherited and synthesized info in doing that */

static RES_Q2ltype epsilon_closure (STATEptr st, OUT_CELLptr inh_w,
				    id_type (* iside_id) (id_type id), 
				    id_type (* oside_id) (id_type id),
				    STATEptr principal_state)
{
  OUT_CELLptr w = NULL, free_w = NULL;
  RES_Q2ltype  res_w = { NULL,  NULL } , down_res_w = { NULL,  NULL } ;
  Q2_LISTptr ql, self;
  ARCptr arc_a, arc_b;
  int outgoing_nonepsilon_arc = FALSE, next_outgoing_nonepsilon_arc = FALSE,
    terminal_state  = FALSE, next_is_terminal_state = FALSE;
  id_type ilabel, olabel;

  if ( (!(STATE_arc_set(st))) && (STATE_final(st)))
    terminal_state = TRUE;
 
  for ( arc_a = STATE_arc_set(st) ; arc_a; arc_a =  next_arc (arc_a))
    {      
  	if  ( iside_id (ARC_label( arc_a)) != EPSILON )
	{
	  outgoing_nonepsilon_arc = TRUE;
	  break;
	}
    }

  for ( arc_a = STATE_arc_set(st) ; arc_a; arc_a =  next_arc (arc_a))
    {
      /* If (a != epsilon) we search for all a-continuations/closures.
      *** If (a == epsilon) we search for the first non-epsilon continuations/closures.
	 */
      ilabel = iside_id (ARC_label( arc_a));
      
      if ( ilabel != EPSILON )
	{
	  continue;
	}
      olabel = oside_id (ARC_label( arc_a));
      w = concat_by_copy_w (inh_w, olabel);
      ql = create_qlist (ARC_destination(arc_a), res_w.head, w);

      /* descend - provided no epsilon cycles (not checked) */
      down_res_w = epsilon_closure ( QL_st(ql), QL_w(ql),  iside_id, oside_id, principal_state);

      if (
	  /** 97-12-23 ( ! STATE_arc_set (QL_st(ql))) && **/
	  (STATE_final (QL_st(ql))))
	next_is_terminal_state = TRUE;
      else
	{
	  next_outgoing_nonepsilon_arc = FALSE;
	  for ( arc_b = STATE_arc_set (QL_st(ql)) ; arc_b; arc_b =  next_arc (arc_b))
	    {      
	      if  ( iside_id (ARC_label( arc_b)) != EPSILON )
		{
		  next_outgoing_nonepsilon_arc = TRUE;
		  break;
		}
	    }
	  if (next_outgoing_nonepsilon_arc)
	    next_is_terminal_state = TRUE;
	  else 
	    next_is_terminal_state = FALSE;
	}

      if (down_res_w.head && down_res_w.tail)

	{
	  if (next_outgoing_nonepsilon_arc || next_is_terminal_state)
	    {
	       if (! res_w.tail)
		 res_w.tail = ql;
	       res_w.head = ql;
	    }
	  else
	    {
	      if ( (free_w = QL_w(ql)))
		{
		  OC_rc(free_w)--;
		  free_output_cell(free_w);
		}
	      free_qlist (ql);
	    }
	  QL_link (down_res_w.tail) = res_w.head;
	  res_w.head = down_res_w.head;
	  if (! res_w.tail)
	     res_w.tail = down_res_w.tail;
	}
      else /* retarded chaining depending on descendant results (none here) */
	{
	  if ( ((! terminal_state ) && outgoing_nonepsilon_arc)
	       || next_is_terminal_state
	       || terminal_state )
	      {
		if (! res_w.tail)
		  res_w.tail = ql;
		res_w.head = ql;
	      }
	  else
	    {
	      if ( (free_w = QL_w(ql)))
		{
		  OC_rc(free_w)--;
		  free_output_cell(free_w);
		}
	      free_qlist (ql);
	    }
	}
    }
  /* *** NEW 97-3-11: inform compute_sigma_2() that it is a destination itself.
     This check (nonzero arcs out) could be done there, too, but here is less
     expensive). */
  if ( outgoing_nonepsilon_arc && res_w.head &&
      (st == principal_state))
    {
      self = create_qlist (principal_state, res_w.head, NULL);
      res_w.head = self;
    }
  return res_w ;
}

/* Compute eventual epsilon closures of all T1 states and put them into the
   client_cells */
void sq_net_epsilon_closures (NETptr t1,
			      id_type (* iside_id) (id_type id), 
			      id_type (* oside_id) (id_type id))
{
  STATEptr st;

  for ( st =  NET_states(t1); st; st = next_state (st))
    {
      STATE_client_cell(st) = (epsilon_closure (st, NULL, iside_id, oside_id, st)).head ;

#ifdef SQ_DEBUG_Q
      /* 1998-7-22 FT, TG */
      debug_print_Q2_LIST((Q2_LISTptr) STATE_client_cell(st), st);  
#endif

    }
}

static void add_final_w (STATEptr state, CONFIGptr w)
{
  if (state && w)
    {
      CONFIG_next (w) = STATE_client_cell (state);
      STATE_client_cell (state) = w;
    }
}

static OUT_CELLptr longest_common_prefix_string_to_string(OUT_CELLptr a,
							  OUT_CELLptr b)
{
  OUT_CELLptr result = NULL, result_tail = NULL, c;

  if (! (a && b) )
    {
      result = create_output_cell(EPSILON);
      return (result);
    }
  for ( ;           a || b ;    a = OC_l(a), b = OC_l(b))
    {
  /* stepping over epsilon-s */
    while ( a && is_epsilon_plus (OC_id(a)))
			a = OC_l(a);
    while ( b && is_epsilon_plus (OC_id(b)))
			b = OC_l(b);
    if (a &&b &&   (OC_id(a) != EPSILON)
	&&     ( OC_id(a) ==  OC_id(b)))
      {
	c = create_output_cell(OC_id(a));
	if (result_tail)
	  {
	    OC_l(result_tail) = c;
	    result_tail = c;
	    assert (OC_rc(c) < SQ_MAX_REFCOUNT);
	    OC_rc(c)++;
	  }
	else /*first piece of result */
	  {
	    result_tail = result = c;
	  }
      }
    else break; /* no further match */
    }
  if (!result)
    result = create_output_cell(EPSILON);
  return (result);
}

/* difference is left. destructive for the 2-nd parameter */
static OUT_CELLptr left_divide (OUT_CELLptr *ls, OUT_CELLptr *rs)
{
  OUT_CELLptr l, r, result = NULL;

  for ( l = *ls, r = *rs; l || r ; l = OC_l(l), r = OC_l(r))
    {
      /* stepping over epsilon-s */
    while ( l && is_epsilon_plus (OC_id(l)))
			l = OC_l(l);
    while ( r && is_epsilon_plus (OC_id(r)))
			r = OC_l(r);

    if( l && r && (OC_id(l) != EPSILON) &&  (OC_id(l) ==  OC_id(r)))
      {
	result = OC_l(r);
	if (OC_l(r))
	  OC_rc(OC_l(r))--;
	free_out_cell(r);
      } 
    else
      {
	result = r;
	break;
      }
    }
  if (!result)
    result = create_output_cell(EPSILON);
  return result;
}

/* **** should be a macro 
   #define q1_hashed(X) ( ( (unsigned int) (X) >> 2 ) %  HA_size)
 */
inline
static unsigned int q1_hashed(STATEptr q1)
{
  unsigned int result;
  /* Op-system dependent ! */
  result = (unsigned int) q1 >> 2 ; /* 2 last bits are always 0 in a word (4 bytes) address */
  result %= HA_size; /* hash value - a prime - computed from net size */
  return result;
}

static unsigned int hash_of_w (OUT_CELLptr w)
{
  unsigned int result = 0;
  OUT_CELLptr cc;

  for ( cc = w;    cc;    cc = OC_l(cc) )
    {
      result += ( unsigned int) OC_id (cc) ;
    }
  return result;
}

static int is_same_w_chain(OUT_CELLptr a, OUT_CELLptr b);

inline
static int is_same_qel_pair (Q2_LISTptr qp1, Q2_LISTptr qp2)
{
  if ( (QL_st(qp1) ==  QL_st(qp2)) &&
       (QL_w_hash(qp1) == QL_w_hash(qp2)) &&
       is_same_w_chain (QL_w(qp1), QL_w(qp2)))
    return TRUE;
  else return FALSE;			 
}

static int not_yet_in_q2 (Q2_QUELptr q2,  Q2_LISTptr new)
{
  int result = TRUE;
  Q2_LISTptr i2;
  for (i2 = QE_list (q2); i2 ; i2 = QL_link (i2))
    {
      if (is_same_qel_pair (i2, new))
	  return FALSE;
    }
  return result;
}

static Q2_QUELptr add_delta_2_member (Q2_QUELptr q2, STATEptr q_dest, OUT_CELLptr w)
{
  Q2_LISTptr ql = NULL;
  assert (q_dest);
 
  if (! q2)
   {
     q2 = make_qel (make_qlist(NULL));
     ql = QE_list( q2);
     QL_w (ql) = w;
     if (w)
       {
	 assert (OC_rc(w) < SQ_MAX_REFCOUNT);
	 OC_rc (w) ++;
	 QL_w_hash (ql) = hash_of_w (w);
       }
     QL_st(ql) = q_dest;
   }
 
 else /* a pair exists in q2 */
   {
     ql = make_qlist(q_dest);
     QL_w (ql) = w;
     if (w)
       {
	 assert (OC_rc(w) < SQ_MAX_REFCOUNT);
	 OC_rc (w) ++;
	 QL_w_hash (ql) = hash_of_w (w);
       }

     if (QE_list(q2))
       {
	 if (not_yet_in_q2 (q2, ql))
	     {
	       QL_link (ql) = QE_list (q2) ;
	       QE_list( q2) = ql;
	     }
	 else /* no hash increase */
	   {
	     if (w)
	       OC_rc (w)--;
	     free_qlist (ql);
	     return q2;
	   }
       }
     else /* first element (pair) in q2 */
       {
	 QE_list( q2) = ql;
       }
   }

  QE_hash (q2) = (QE_hash (q2) + q1_hashed (q_dest)) % HA_size;
  return q2;  
}

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

static Q2_QUELptr  q1_eq_q2 (Q2_QUELptr q1, Q2_QUELptr q2)
{
  int  match_1 = FALSE, match_2 = FALSE, q2_pairs_no = 0, res_pairs_no = 0 ;
  Q2_QUELptr result = NULL, rev_result = NULL;
  Q2_LISTptr i2, old;

  for (i2 = QE_list (q2), q2_pairs_no = 0 ; i2 ; i2 = QL_link (i2), q2_pairs_no++)
    {
      match_1 = FALSE;
      for (old = QE_list(q1); old ; old = QL_link (old))
	{
	  if (is_same_qel_pair (i2, old))
	    {
	      match_1 = TRUE; /* this (q,w) pair from q2 - i2 - is ok. */
	      result = q1;
	      break;
	    }
	}
      /* if this (q,w) pair from q2 has no match in q see next q) */
      if (!match_1)
	{
	  result = NULL;
	  break; /* this q of Q didn't match. Try next. */
	}
    }

  if (result)
    /* then all (q,w) pairs from q2 has a match in "result". But we need
       exact match!. 
       So reverse match should be checked, too.
       Also, the number of pairs must match, too */
    {
      for (old = QE_list(result), res_pairs_no = 0 ; old ; 
	   old = QL_link (old), res_pairs_no ++)
	{
	  match_2 = FALSE;
	  for (i2 = QE_list (q2) ; i2 ; i2 = QL_link (i2))
	    {
	      if (is_same_qel_pair (i2, old))
		{
		  match_2 = TRUE; /* this (q,w) pair from result - old - is ok. */
		  rev_result = q2;
		  break;
		}
	    }
	  /* if this (q,w) pair from result has no match in q2 see next q) */
	  if (!match_2)
	    {
	      rev_result = NULL;
	      break; /* this result of Q doesn't really match.. */
	    }
	}
      if ( rev_result)
	{
	  if  (! ( res_pairs_no == q2_pairs_no))
	    fsm_fprintf (stderr, "*** debug print: AAB <-> AB type match! \n");
	  return result; 
	}
      else
	result = NULL; /* no real match: "result" has more pairs than q2 */
    }
  return result;
}


static Q2_QUELptr  q2_ident_in_Q (Q2_QUELptr q2)
/* 
   Based on hashing.
 */
{
  Q2_QUELptr  result = NULL;
  HASH_CELLptr h;

  if (! ( (HA [QE_hash(q2)]).q2 ))
    return result;
  else
    for (h  = & (HA [QE_hash(q2)]) ;    h    ;    h = HC_l (h))
      {
	if ( ( result = q1_eq_q2 ( HC_q2(h), q2)))
	  break;
      }
  return result;
}

static void add_HA_element (Q2_QUELptr q2_new)
{
  HASH_CELLptr newhc = NULL;
  Q2_QUELptr old;

  if (! ( old = (HA [QE_hash(q2_new)]).q2 ))
    (HA [QE_hash(q2_new)]).q2 = q2_new;
  else
    {
      newhc = create_hash_cell ( old);
      HC_l (newhc) = (HA [QE_hash(q2_new)]).link;
      (HA [QE_hash(q2_new)]).link = newhc;
      (HA [QE_hash(q2_new)]).q2 = q2_new;
    }
}


/* --- Marking---------------------------------------------------------------------*/
/* Marking: there are two independent marking mechanisms:
   1. by ARC_userflag1-s (2 bits)
   2. by ARC_visit_mark-s (4 bits)
   */
/* --- Marking ARC_userflag1-s of ARC-s -------------------------------------------*/

enum arc_u_marker {arc_not_u_visited,  arc_u_visited };

static void set_arcs_userflag1 (STATEptr state, enum arc_u_marker u_visit_type, 
				id_type in_side_id,  id_type (* iside_id) (id_type id) )
{ 
  ARCptr arc;

  for ( arc = STATE_arc_set(state); arc; arc = next_arc (arc))
    {
      if (iside_id (ARC_label( arc)) == in_side_id )
	ARC_userflag1 (arc ) = (int) u_visit_type;
    }
}

/* reset all userflag1 marks of this q2 to arc_not_u_visited */
void reset_arcs_userflag1 (Q2_QUELptr q2)
{ 
  ARCptr arc = NULL;
  Q2_LISTptr l = NULL;

  if (q2)
    for ( l = QE_list(q2); l; l = QL_link (l))
      {
	for ( arc = STATE_arc_set(QL_st(l)); arc; arc = next_arc (arc))
	  {
	    ARC_userflag1 (arc ) = (int) arc_not_u_visited;
	  }
      }
}

/* userflag1-mark all a arcs of all states of a q2 set {(q1_1, w1), q1_2, w2), ...) */
static void set_userflag1_m_arcs_q2 (Q2_QUELptr q2,
				     enum arc_u_marker u_visit_type, 
				     id_type a,
				     id_type (* iside_id) (id_type id))
{
  Q2_LISTptr ql = NULL;
  assert (q2);
  for (ql = QE_list(q2); ql ; ql = QL_link(ql))
    {
      set_arcs_userflag1 (QL_st (ql), u_visit_type, a , iside_id);
    }
}

/* --- Marking  ARC_visit_mark-s of ARC-s -------------------------------------------*/

enum arc_marker {arc_not_visited, arc_clustered, arc_visited, arc_dont_touch };

static void set_arcs_visit_mark (STATEptr state, 
				 enum arc_marker visit_type, 
				 id_type in_side_id,  
				 id_type (* iside_id) (id_type id), 
				 enum arc_marker ignore_visit_type)
{ 
  ARCptr arc;

  for ( arc = STATE_arc_set(state); arc; arc = next_arc (arc))
    {
      if ( ARC_visit_mark (arc) == ignore_visit_type)
	continue; /* ignore this, see to next arc */
      if (iside_id (ARC_label( arc)) == in_side_id )
	ARC_visit_mark (arc ) = (int) visit_type;
    }
}

/* reset all visit marks of this q2 to arc_not_visited */
static void reset_arcs_visit_marks (Q2_QUELptr q2)
{ 
  ARCptr arc = NULL;
  Q2_LISTptr l = NULL;

  if (q2)
    for ( l = QE_list(q2); l; l = QL_link (l))
      {
	for ( arc = STATE_arc_set(QL_st(l)); arc; arc = next_arc (arc))
	  {
	    ARC_visit_mark (arc ) = (int) arc_not_visited;
	  }
      }
}

/* Visit-mark all a arcs of all states of a q2 set {(q1_1, w1), q1_2, w2), ...) */
static void set_visit_m_arcs_qlist (Q2_LISTptr q2l, 
				    enum arc_marker visit_type, 
				    id_type a,
				    id_type (* iside_id) (id_type id) , 
				    enum arc_marker ignore_visit_type)
{
  Q2_LISTptr ql = NULL;
   for (ql = q2l; ql ; ql = QL_link(ql))
    {
      set_arcs_visit_mark (QL_st (ql), visit_type, a, iside_id, ignore_visit_type);
    }
}

/* Visit-mark all a arcs of all states of a q2 set {(q1_1, w1), q1_2, w2), ...) */
void set_visit_m_arcs_q2 (Q2_QUELptr q2, 
			  enum arc_marker visit_type, 
			  id_type a,
			  id_type (* iside_id) (id_type id) , 
			  enum arc_marker ignore_visit_type)
{
  assert (q2);
  set_visit_m_arcs_qlist (QE_list(q2), visit_type, a,  iside_id, ignore_visit_type);
}

/* === More ancillary functions. =============================================*/

int is_epsilon_cell (OUT_CELLptr this)
{
  int result = FALSE;
  OUT_CELLptr eps = create_output_cell(EPSILON);
  result = is_same_w_chain(this, eps);
  free_output_cell(eps);
  return result;  
}

static OUT_CELLptr compute_sigma_2 (Q2_LISTptr q2_list, 
				    id_type a, 
				    id_type (* iside_id) (id_type id),
				    id_type (* oside_id) (id_type id))
{
  OUT_CELLptr sigma_2 = NULL,  pps2 = NULL, part_sigma_2 = NULL,
    pa_si_2_1, pa_si_2_2 ;

  Q2_LISTptr qqll, qle; 
  ARCptr arc_a;
  int first;

  /* EPSILON input labels participate only implicitely in sigma-2 computations. */
  if ( a == EPSILON )
    return sigma_2;

  /* Looping on all Arc-s belonging to an "a" cluster in q2*/
  for (qqll = q2_list;   qqll  ;        qqll = QL_link(qqll))
    {
      /* New Epsilon closure scheme - 97-01-08 */
      /* Because of { (3, bde), (3, cde)} type arcs - resetting here !*/
      set_visit_m_arcs_qlist( q2_list,  arc_clustered,a, iside_id, arc_dont_touch);
		  
      for ( arc_a = STATE_arc_set(QL_st (qqll)); arc_a; arc_a =
	      next_arc (arc_a))
	{
	  if ( !(ARC_visit_mark (arc_a ) == arc_clustered) )
	    continue;

	  /* * fix a marking pb - only deal with this input label arcs */
	  if ( !( iside_id (ARC_label( arc_a)) == a ))
	    continue;

	  ARC_visit_mark (arc_a ) = arc_visited;

	  /* There are two longest_common_prefix operations ("sums"):
	     first, output labels of a q_i (of T1) state with "a"
	     outgoing arcs must be summed up then their result with
	     other q_j-s (with outgoing "a" input labels).
	     Only if the result is non-epsilon can common initial
	     prefixes of all involved arcs be "unified" otherwise
	     the correspondsing output label on the new arc will be
	     EPSILON. */
	  part_sigma_2 =  concat_by_copy_w
	    (QL_w_head(qqll), oside_id (ARC_label ( arc_a)));

	  /* 97-3-10: epsilon-closure strings preferably into sigma-s than
	     into w strings... */ 

	  /* 97-12-30: try this:
	     if the part_sigma_2 is still epsilon then look ahead for a possible
	     next sigma piece from the epsilon closure. We wish to avoid
	     epsilons as long as possible. If this is not an epsilon then don't
	     do this lookahead */
	  if ( is_epsilon_cell (part_sigma_2))
	    {
	      if ((Q2_LISTptr ) STATE_client_cell(ARC_destination (arc_a)))
		{
		  for (qle = (Q2_LISTptr ) STATE_client_cell(ARC_destination (arc_a)),
			 pa_si_2_2 = pa_si_2_1 = NULL, first = TRUE ;
		       qle  ;        qle = QL_link(qle))
		    {
		      if (first)
			{
		      
			  {
			    pa_si_2_1 = concat_by_copy_w1_w2 (part_sigma_2, QL_w(qle));
			  }
		    
			  first = FALSE;
			  continue;
			}
		      pa_si_2_2 = concat_by_copy_w1_w2 (part_sigma_2, QL_w(qle));
		      pa_si_2_1 = longest_common_prefix_string_to_string (pa_si_2_2, pa_si_2_1);
		      free_output_cell (pa_si_2_2);
		    }
		  part_sigma_2 = pa_si_2_1;
		}
	    }

	  if (!pps2)
	    pps2 = part_sigma_2;
	  else
	    {
	      pps2 = longest_common_prefix_string_to_string (pps2,
							     part_sigma_2);	
	      free_output_cell (part_sigma_2);	 part_sigma_2 = 0; 
	    }

	} /* end of looping on an "a" of a "q" in q2 */
    } /* end of looping on a q2 element pair */

  if (pps2)
    sigma_2 = pps2;
  else
    sigma_2 = create_output_cell(EPSILON);
  /* Now we have sigma_2 */

  return sigma_2;
}

static void add_first_q2_to_Q (NETptr T1)
{
  /* 1. add a (0,0) element;
     2. add all epsilon closure pairs 
     { (q_0_e_1, w_0_e_1) (q_0_e_2, w_0_e_2) ...}
     of the start state of T1 */
  Q2_LISTptr  qle; 

  add_Q_element (add_delta_2_member ( QQU.qhead  , NET_start_state(T1),
				 create_output_cell(EPSILON)));

  for ( qle = (Q2_LISTptr ) STATE_client_cell( NET_start_state(T1));     
	qle  ;        
	qle =  QL_link(qle))
    {
      QQU.qhead = add_delta_2_member
	(QQU.qhead, QL_st(qle), QL_w(qle));
    }
}

static void compute_delta_2_members (Q2_QUELptr  * q2_new, 
				     Q2_LISTptr q2_list, 
				     OUT_CELLptr sigma_2,
				     id_type a,
				     id_type (* iside_id) (id_type id),
				     id_type (* oside_id) (id_type id))
{
  OUT_CELLptr rs, pre_rs;
  Q2_LISTptr qqll, qle; 
  ARCptr arc_a;

 /* Looping on all q2 Arc-s belonging to an "a" cluster */
  for (qqll = q2_list;   qqll  ;        qqll = QL_link(qqll))
   {
     /*** for all q2 members: reset visit marks */
     set_visit_m_arcs_qlist (q2_list,  arc_clustered,a, iside_id, arc_dont_touch );
		 
     if (a)
       for ( arc_a = STATE_arc_set(QL_st (qqll)); arc_a; arc_a =
	       next_arc (arc_a))
	 {
	   if ( !(ARC_visit_mark (arc_a ) == arc_clustered))
	     {
	       /* ARC_visit_mark (arc_a ) = arc_dont_touch;  */
	       continue;
	     };
	   /* * fix a marking pb - only deal with this input label arcs */
	   if ( !( iside_id (ARC_label( arc_a)) == a ))
	     continue;
	   ARC_visit_mark (arc_a ) = arc_visited;
	   rs = concat_by_copy (qqll,  oside_id (ARC_label (arc_a)));
	   /* create new-q2 element structures *//* ***dont't forget to FREE */
	   *q2_new = add_delta_2_member
	     (* q2_new,
	      ARC_destination (arc_a),
	      left_divide  (& sigma_2, & rs));

	   /* computing delta_2_members coming from all a-epsilon-closures of 
	     the destination state of this arc - (q_i_e, w+w_i_e) pairs */
	   if ((Q2_LISTptr ) STATE_client_cell(ARC_destination (arc_a)))
	     for (qle = (Q2_LISTptr ) STATE_client_cell(ARC_destination (arc_a)) ;
		  qle  ;        qle = QL_link(qle))
	       {
		 pre_rs = concat_by_copy_w (QL_w(qqll), oside_id (ARC_label (arc_a)));
		 rs = concat_by_copy_w1_w2 (pre_rs, QL_w(qle));
		 free_output_cell (pre_rs);
		 *q2_new = add_delta_2_member
		   (* q2_new,  QL_st(qle),  left_divide  (& sigma_2, & rs));
	       }
	 } /* end of looping on an "a" of a "q" in q2 */
   } /* end of looping on a q2 element pair */
}


/*============================================================================*/

static NETptr sq_subsequentialize_1 (NETptr T1, 
				     int input_side)
{
  NETptr T2 = NULL;
  /* start state of T2 */
  Q2_QUELptr q2 = NULL, q2_new = NULL, 
    q2_to = NULL;
  Q2_LISTptr ql = NULL; 
  OUT_CELLptr sigma_2;
  ALPHABETptr   s1, ls1 = lower_sigma(T1), 
    us1 = upper_sigma(T1) ;
  id_type (* iside_id) (id_type id);
  id_type (* oside_id) (id_type id); 
  ARCptr arc;
  id_type a;
  CONFIGptr final_w = NULL;
  int q2_state_counter = 0;

  /* STEP 0: test of subsequentiability */
 
  NUM_OF_Q2 = 0;
  QQU.qhead = QQU.qtail = NULL;

  /*  delta_2 below is the state transition function of the fst T2 at
      a given state.
      sigma_2 below is the output function of the fst T2 at a
      given state.
      FI_2 is the output string of T2 at a given final state. If T2 is
      p-subsequential then there are several (at most p, at least at one
      state exactly p) such strings. 
     */


  /* STEP 1: F2 <- 0 */
  /* ------ */
  /* *** initialize a new state chain. Initialize a new net T2 */
  if (T1)
    {
      /* number_T1_states (T1); */

      T2 = make_network();
      /* *** Copying T1 alphabet to T2. */ 
      NET_labels (T2) = copy_alphabet ( NET_labels (T1)) ; 
      /* Determining direction A:B read from left or from right */
      if (input_side == UPPER) /* direction is l-r (==a) at  a:b */
	{
	  s1 = us1;
	  iside_id = upper_id;
	  oside_id = lower_id;
	} 
      else 
	{
	  s1 = ls1;
	  oside_id = upper_id;
	  iside_id = lower_id;
	}
    }
  else 
    {
      if (*SQ_VERBOSE)
	fsm_fprintf(stderr, "sequentialize: no T1 fst.\n");
      exit(1);
    }

#ifdef SQ_DEBUG_Q
      /* 1999-2-4 */
      if (yes_or_no("\n** sq_subsequentialize_1(): print the FULL T1 net?", 'n'))
	debug_print_net(T1, TRUE); 
#endif

  /* 97-1-9: epsilon_closures in client cells. */
  sq_net_epsilon_closures(T1, iside_id, oside_id);

  /* STEP 2: i2  <- union {(i, epsilon)} */
  /* *** 
     Union of all start states of T1 and so creating the start state
     i2 of T2 with epsilon as final output string, w.
     The work set of T2 states, qi-s, consists of pairs of states from
     T1, q1-s, and a final output string, which is now (in the case of
     the initial q2 state of T2) just epsilon.
     Normally, there is only one start state (?) in T1. Its initial
     final_strings set is empty (or an epsilon arc ) */

  if (NET_start_state(T1))
    {
      NET_start_state(T2) = make_state(NULL, T2);
      NET_sigma (T2) = sigma (T2); 
  
      add_first_q2_to_Q (T1);

      QE_st(QQU.qhead ) =  NET_start_state(T2); 
      add_HA_element (QQU.qhead);
    }
  else
    {
      if (*SQ_VERBOSE)
	fsm_fprintf(stderr, "sequentialize: empty start state in T1.\n");
      exit(1);
    }


  /* STEP 3: Q <- {i2} */
  /* *** the work state set, Q, contains i2 as starting
     configuration. It is a queue, that is, there is a full ordering
     relation */ 

  /* done in step 2 */


  /* STEP 4: while Q =/= 0 */ 
  /* *** looping on Q, that is, examining the members (which are
     states) of the work set, Q, and possibly creating more members */

  /* STEP 5: do q2 <- head[Q] */
  /* *** the state in work is the actual head of the work state queue. */
  for ( q2 = QQU.qhead ; q2 ; q2 = QE_link(q2) )
    {
     
  /* ** WARNING: STEPS 6,7,8 go together ** */


  /* STEP 6: 
     subsequential case: 
                      if (there exists (q,w) E q2 such that q E F1)
     p-subsequential case:	      
                      for each (q,w) E q2 such that q E F1 */	 
  /* *** 
     If q2 - the actual state in T2 (q2 is also a pair of a state
     from T1 and a string w) is final, then its component string, w, is
     becoming a final output string coupled to this state.
     That is, if this state is reached, AND we stay there (no more
     input), this w string must also be printed.
     Since we consider the p-subsequential case, we can have several
     such pairs in q2 so all of them must be considered */

#ifdef SQ_DEBUG_Q
      debug_print_q2(q2);
#endif

      for (ql = QE_list(q2); ql ; ql = QL_link(ql))
	{
	  if ( STATE_final(QL_st(ql)))
	    {

  /* STEP 7:
     subsequential case:
                      then F2 <- F2 U {q2}
     p-subsequential case:
                      do   F2 <- F2 U {q2} */
  /* ***
     If q is final then q2 is becoming a final state in T2 */
	      if (!QE_st(q2))
		QE_st(q2) = make_state(NULL, T2);
	      STATE_final(QE_st(q2)) = TRUE;


  /* STEP 8:
     subsequential case:
                      FI_2(q2) <- w
     p-subsequential case:
                      ADD_OUTPUT (FI_2, q2, w) */
  /* ***
     In the subsequential case the output string - when we stay in this
     final q2, that is, there is no more input for the fst - the final
     output string is w.
     In the p-subsequential case the output string set of q2 (since
     there can be several) is augmented by this new member, w . */

	      final_w = make_config (QL_w(ql));

	      add_final_w (QE_st(q2), 
			    final_w);

	    } /* end of if ... state is final .... */
	} /* end of first looping - STEPS 6 7 8 - on pairs of q2 */


  /* STEP 9: for each a such that (q,w) E q2 and delta_1 (q,a)
     defined */
  /* *** for all different output (either upper or lower, depending on
     side) symbols of arcs originating from q, that is, looping on
     arcs and finding all ambiguities (on the actual side)  ... */
      
      /* looping on q2 = {(q_1_1, w_1), (q_1_2, w_2), ... } elements. */

      sigma_2 = NULL;
      q2_new = NULL;

      reset_arcs_visit_marks (q2);
      reset_arcs_userflag1(q2);

      for (ql = QE_list(q2);   ql  ;        ql = QL_link(ql))
	{	 
	  /* next_arc substituted with a more sophisticated function doing epsilon
	     closure states arc iteration, too. 	  
	     for ( arc = next_arc_closure (QL_st (ql), TRUE) ;    arc; 
		arc = next_arc_closure (QL_st (ql), FALSE))
		*/
	  for ( arc = STATE_arc_set(QL_st (ql)); arc ;  arc = next_arc(arc))
	    {
	      a = iside_id (ARC_label( arc)) ;
	     
	      if (ARC_userflag1(arc) == arc_u_visited )
		continue;  /* see to next arc in the input-side-identic cluster */

	      /* Mark all `a' arcs of the WHOLE q2 pair set as userflag1-visited ! */
	      set_userflag1_m_arcs_q2 (q2,  arc_u_visited,  a, iside_id);


  /* STEP 10: do sigma_2(q2,a) <- longest_common_prefix_of_J1 
                ( longest_common_prefix_of_(q,a)-s ( w , sigma_1 (q, a, q' ))) */
  /*            - where (q,a) E J1(a) and q' E delta_1 (q,w) 
		- - where J1(a) = {(q,w)|delta_1(q,a) defined and 
		    (q,w) E q2} */
  /* ***
     J1 is a subset of q2 (that is, {q,w}) where all outgoing arcs
     from q are labelled (symbol-ed) by a.
     The output function, sigma_2, of q2 will be the longest common
     prefix of "w" with all "a" arcs from q (that is, q'-s, in T1), and this
     is repeated for all members of J1, that is, for all q-s (in T1)
     with "a" input and members of q2.
     That is, an output function, sigma_2, is created here.
     */	      
	      sigma_2 = NULL;

	      if (a)
	       sigma_2 = compute_sigma_2 (QE_list(q2), a, iside_id, oside_id);

	  /* BEGIN NEW 98-10-13: try out new idea on symbol/character (NOT
		string) sequentialization: 
		Only the FIRST symbol of sigma_2 is kept as sigma_2, that is,
		the real sigma_2 - say "abc" - is chopped to "a" and the 
		remaining "bc" will get into (subsequent) delta_2-s. 
		It has the advantage of preserving real epsilon-pushing!

		This is fundamental: in fact, this is a different version of the
		(XRCE_modified-) Mohri-sequentialization algorithm.
		The advantage is that it remains strictly epsilon-free to a
		given length - but it may increase the overall size by
		possibly increasing the number of new q_2 states!
		*/
	      if (SQ_STRING_ONELONG && sigma_2 && (OC_l (sigma_2)))
		{
		  OC_rc(OC_l(sigma_2))--;
		  free_output_cell (OC_l (sigma_2));
		  OC_l(sigma_2)=NULL;
		}
	  /* END NEW 98-10-13: try out new idea on character (NOT
		string) sequentialization */

  /* STEP 11: delta_2(q2, a) <- Union {(q'[sigma_2(q2,a)]to_the_minus_one_power w sigma_1 (q,a,q'))}
                                (q,w,q') E J2(a)
		  - where J2(a) = { (q,w,q') | delta_1 (q,a) defined and (q,w) E q2 and q' E delta_1(q,a)}
		  */  
 /* *** Creating the state transition function of a possibly new state in T2.
    All such states (of T1) are considered where q (of T1) gets by a. So this
    possibly new q2' is a set of (q',w') pairs.   */	 

	      /* side-effect: q2_new and/or its elements may change */

	      compute_delta_2_members (& q2_new, QE_list(q2), sigma_2, a, iside_id, oside_id );

 /* STEP 12: if (delta_2(q2,a) is a new state)
   */  
  /* *** Checking whether the new state created in STEP 11  is really
     new in T2, that is, in Q,  - this is hard if the fst is big */

	      /*  Loop on Q  */

	      /* 97-1-13 EPSILON input labels don't count here */
	      if ( a && (! (q2_to =  q2_ident_in_Q (q2_new))))

 /* STEP 13:     then ENQUEUE (Q2, delta_2 (q2,a))
   */	
  /*  *** If STEP12 is true, that is, q2 is a new state then put q2 to
     the end of the Q queue.
     Else free the workspace occupied by this would-be q2.
     Back to STEP 4.
     */
		{
		  q2_to = q2_new;
		  add_Q_element (q2_new);
		  add_HA_element (q2_new);

		  /* add q2_new to T2 too */
		  QE_st (q2_new ) = make_state (NULL, T2);

		  q2_state_counter ++;
		  if ( (*SQ_VERBOSE) && (HA_size > 100) && (q2_state_counter % (HA_size/10) == 0))
		    fsm_fprintf (stderr , "%d%%", ((q2_state_counter*100)/HA_size));
		}
	      else
		/*  free-ing q2_new and ALL its components ! */
		{
		  free_quel_and_pairs(q2_new); 
		}
	       /* arc to a q2 state either old or new... */

	      if (!a) 
		sigma_2 = create_output_cell(EPSILON);
	      if (q2_to && (a || OC_id(sigma_2)))
		{
		  if (SQ_INTERN_STRINGS_ARCS)
		    create_arc_or_T2states_and_arcs
		      (q2, q2_to, input_side,a,  sigma_2, T2);
		  else
		    create_arc_multi_ch_label
		      (q2, q2_to, input_side ,a, sigma_2, T2,
		       SQ_LABELS_BUFFER,SQ_BU_LE);
		}
	      free_output_cell (sigma_2);
	      sigma_2 = NULL;
	      q2_new = NULL;
	      q2_to = NULL;
#ifdef SQ_DEBUG_QA
 fsm_fprintf (stderr, " q2-actual: %d; q2_to  %d;  ", (int) q2, (int) q2_to);
  fsm_fprintf ( stderr, "a = ");
  print_label(a , stderr, DO_ESCAPE);  
  fsm_fprintf (stderr,"\n");
 debug_print_Q();

 /* ** FATAL: Changes client-cell! : print_net(T2, stderr); */
#endif

	    } /* end of looping on T1 state arcs (of q2 ?) - Step 9+ */
	} /* end of second looping - STEPS 9 10 11 ?? - on pairs of q2 */

    } /* end of looping on Q Steps 4, 5 */


  /* STEP 14: DEQUEUE (Q)
    */
  /* ***
     Q is now empty. If T2 is not empty than it is just the
     p-subsequential equivalent of T1.
     In the first implementation, T2 will be an "ordinary' fst (with
     exactly the same structure than any fst) and the
     p-subsequentiality will appear as a separate list of
     (final_state_of_T2, final_output_string_set_of_this_state) pairs
     for all final states in T2.
     Later a changed net/state layout will carry this information
     directly at the state.
     */

  if ((*SQ_VERBOSE) && ( !( NET_start_state(T2) &&  ( NET_num_states(T2) > 0 ))))
   fsm_fprintf (stderr , "p-seq: no result net!");

#ifdef SQ_DEBUG_Q
 fsm_fprintf (stderr, "\n ============= P R I N T   B Y     Q U E U E ===============\n");
 debug_print_Q();
 fsm_fprintf (stderr, "\n ============= P R I N T   B Y     H A S H  ===============\n");
 debug_print_Q_by_HA(); 
#endif

#ifdef SQ_DEBUG_Q
  /* **** Fatal: changes client_cell!!!
  print_net_s(T2, 80, stdout);
  fsm_fprintf (stdout, "**--- Subseq end of not-yet-transformed-T2 debug print ---/n");
   */
#endif

/*=== End of Mohri's subsequentialization algorithm ===================*/

  /* ------- Transform T2 to fsm-i/o-compatible form ------------ */ 
  /* Final w strings are lists originating in a final q2 state. They
     will be transformed into semantically equivalent fsm states and
     arcs. 
     */
 if ((*SQ_VERBOSE) && HA_size > 100)
   fsm_fprintf (stderr,"\n");
 if (*SQ_VERBOSE)
   fsm_fprintf (stderr, "** Subsequentialize: Number of new q2 states: %d.\n", NUM_OF_Q2 );
 if (ls1)
   free_alph (ls1);
 if (us1)
   free_alph (us1);
 return (T2);
}

static int name_cmp(register LABELptr lab1, register LABELptr lab2)
{
  /* copied and CHANGED from struct.c 02 97 TG */

register int cmp = fat_strcmp(LABEL_atom(lab1),
				    LABEL_atom(lab2));
  
  if (cmp == 0)
    return(0);
  else if (cmp < 0)
    return(-1);
  else
    return(1);
}

static int seq_upper_alph_order(register id_type id1, register id_type id2)
{
  register LABELptr lab1 = id_to_label(id1), lab2 = id_to_label(id2);
      
 
  if ((LABEL_arity(lab1) == 1) &&  (LABEL_arity(lab2) == 1))
	return(name_cmp(lab1, lab2));
  else if (LABEL_arity(lab2) == 1)
    {
       switch(name_cmp(id_to_label(TUPLE_labels(LABEL_tuple(lab1))[0]),
		      lab2))
	 {
	 case -1: 
	   return(-1);
	 case 0:
	   return (name_cmp(id_to_label(TUPLE_labels(LABEL_tuple(lab1))[1]),
			  lab2));
	 default: 
	   return(1);
	}
    }
  else if (LABEL_arity(lab1) == 1)
    {
       switch(name_cmp(lab1, id_to_label(TUPLE_labels(LABEL_tuple(lab2))[0])))
	 {
	 case -1: 
	   return(-1);
	 case 0:
	   return (name_cmp(lab1, id_to_label(TUPLE_labels(LABEL_tuple(lab2))[1])));
	 default: 
	   return(1);
	}
    }
  else				/* both lab1 and lab2 are tuples */
    {
      switch(name_cmp(id_to_label(TUPLE_labels(LABEL_tuple(lab1))[0]),
		      id_to_label(TUPLE_labels(LABEL_tuple(lab2))[0])))
	{
	case -1: 
	  return(-1);
	case 0:
	  return (name_cmp(id_to_label(TUPLE_labels(LABEL_tuple(lab1))[1]),
			   id_to_label(TUPLE_labels(LABEL_tuple(lab2))[1])));
	default: 
	  return(1);
	}
    }
}

static int seq_lower_alph_order(register id_type id1, register id_type id2)
{
  register LABELptr lab1 = id_to_label(id1), lab2 = id_to_label(id2);

  if ((LABEL_arity(lab1) == 1) &&  (LABEL_arity(lab2) == 1))
	return(name_cmp(lab1, lab2));
  else if (LABEL_arity(lab2) == 1)
    {
       switch(name_cmp(id_to_label(TUPLE_labels(LABEL_tuple(lab1))[1]),
		      lab2))
	 {
	 case -1: 
	   return(-1);
	 case 0:
	   return (name_cmp(id_to_label(TUPLE_labels(LABEL_tuple(lab1))[0]),
			  lab2));
	 default: 
	   return(1);
	}
    }
  else if (LABEL_arity(lab1) == 1)
    {
       switch(name_cmp(lab1, id_to_label(TUPLE_labels(LABEL_tuple(lab2))[1])))
	 {
	 case -1: 
	   return(-1);
	 case 0:
	   return (name_cmp(lab1, id_to_label(TUPLE_labels(LABEL_tuple(lab2))[0])));
	 default: 
	   return(1);
	}
    }
  else				/* both lab1 and lab2 are tuples */
    {
      switch(name_cmp(id_to_label(TUPLE_labels(LABEL_tuple(lab1))[1]),
		      id_to_label(TUPLE_labels(LABEL_tuple(lab2))[1])))
	{
	case -1: 
	  return(-1);
	case 0:
	  return (name_cmp(id_to_label(TUPLE_labels(LABEL_tuple(lab1))[0]),
			   id_to_label(TUPLE_labels(LABEL_tuple(lab2))[0])));
	default: 
	  return(1);
	}
    }
}

static int seq_lower_label_order(id_type id1, id_type id2)
{
  /* Borrowed and modified from struct.c:default_label_order()
     Returns -1 if ID1 precedes ID2, 0 if they are the same, otherwise 1.
     EPSILON precedes every other label. OTHER precedes all but EPSILON.
     ALTCHAIN_SYMBOL follows all other labels, single labels precede tuples,
     tuples are sorted by considering the coordinate labels in ascending
     order.  The order of single labels, other than EPSILON, OTHER, and
     ALTCHAIN_SYMBOL is the same as the alphabetic order of the ASCII codes
     that constitute their name strings.
     */
  if (id1 == id2)
    return(0);
  else if (id1 == EPSILON)
    return(-1);
  else if (id2 == EPSILON)
    return(1);
  else if ((id1 !=OTHER) && (id2 != OTHER) 
	   && (id1 != ALTCHAIN_SYMBOL) && (id2 != ALTCHAIN_SYMBOL))
    return (seq_lower_alph_order(id1, id2));
  else if (id1 == OTHER)
    return(-1);
  else if (id2 == OTHER)
    return(1);
  else if (id1 == ALTCHAIN_SYMBOL)
    return(1);
  else if (id2 == ALTCHAIN_SYMBOL)
    return(-1);
  else
    {
      if (*SQ_VERBOSE)
	fsm_fprintf (stderr, "***seq_lower_label_order - unexpected condition. Stop.\n");
      exit(1);
    }
}

static int seq_upper_label_order(id_type id1, id_type id2)
{
  /* Borrowed and modified from struct.c:default_label_order()
     Returns -1 if ID1 precedes ID2, 0 if they are the same, otherwise 1.
     EPSILON precedes every other label. OTHER precedes all but EPSILON.
     ALTCHAIN_SYMBOL follows all other labels, single labels precede tuples,
     tuples are sorted by considering the coordinate labels in ascending
     order.  The order of single labels, other than EPSILON, OTHER, and
     ALTCHAIN_SYMBOL is the same as the alphabetic order of the ASCII codes
     that constitute their name strings.
     */
  if (id1 == id2)
    return(0);
  else if (id1 == EPSILON)
    return(-1);
  else if (id2 == EPSILON)
    return(1);
  else if ((id1 !=OTHER) && (id2 != OTHER) 
	   && (id1 != ALTCHAIN_SYMBOL) && (id2 != ALTCHAIN_SYMBOL))
    return (seq_upper_alph_order(id1, id2));
  else if (id1 == OTHER)
    return(-1);
  else if (id2 == OTHER)
    return(1);
  else if (id1 == ALTCHAIN_SYMBOL)
    return(1);
  else if (id2 == ALTCHAIN_SYMBOL)
    return(-1);
  else
    {
      if (*SQ_VERBOSE)
	fsm_fprintf (stderr, "***seq_upper_label_order - unexpected condition. Stop.\n");
      exit(1);
    }
}

static int seq_upper_arc_comp_fn (const void *a1, const void *a2)
{
  return(seq_upper_label_order(ARC_label (* (ARCptr *) a1),
			       ARC_label (* (ARCptr *) a2)));
} 

static int seq_lower_arc_comp_fn (const void *a1, const void *a2)
{
  return(seq_lower_label_order(ARC_label (* (ARCptr *) a1),
			       ARC_label (* (ARCptr *) a2)));
}

/* order of arcs for a "quasi-deterministic" (i.e. sequentialized) lookup:
   EPSILON; labels with lower-side_EPSILON; OTHER;  single-s and tuples
   intermixed according to upper (major) and then lower (minor) */
static int seq_lower_comp_fn(const void *a1, const void *a2)
{
  return (seq_lower_arc_comp_fn (a1, a2));
}

/* order of arcs for a "quasi-deterministic" (i.e. sequentialized) lookup:
   EPSILON; labels with upper-side_EPSILON; OTHER; single-s and tuples
   intermixed according to upper (major) and then lower (minor) */
static int seq_upper_comp_fn(const void *a1, const void *a2)
{
  return (seq_upper_arc_comp_fn (a1, a2));
}

NETptr sq_subsequentialize (NETptr T1, 
			    int input_side,
			    int reclaim_p,
			    int test_of_subsequentiality_needed, 
			    int hash_factor,
			    FST_CNTXTptr fst_cntxt)
{
  NETptr T2 = NULL;
  
 allocate_seq_memory(T1, hash_factor);
#ifdef DEBUG
  if (SQ_VERBOSE && *SQ_VERBOSE)
    fsm_fprintf (stderr, "** Subsequentialize: DEBUG version.\n" );
#endif
 if (!(SQ_FINAL_STRINGS_ARCS &&  SQ_INTERN_STRINGS_ARCS ))
   {
     SQ_BU_LE = 100 * longest_minimal_string ( T1, input_side, FALSE ); /* CHANGES client cell! */
     SQ_LABELS_BUFFER =  calloc (SQ_BU_LE, sizeof (char));
   }
  if (test_of_subsequentiality_needed)
    {
#ifdef SQ_DEBUG_Q
      if ( sq_state_arc_chain_selfcycles (T1))
	exit (0);
#endif
    if (! is_fst_sequentiable_7 (T1,  input_side))
      {
	if (*SQ_VERBOSE)
	  fsm_fprintf (stderr , "*** This net is not subsequentiable from the %s direction. \n",
		   (input_side ? "LOWER": "UPPER"));
	return (T1);
      }
    }
  reset_visit_marks_and_client_cells_to_zero(T1);
  T2 = sq_subsequentialize_1 ( T1, input_side);

#ifdef SQ_DEBUG_Q
  /*** NO! Danger: changes client cell !!: print_net_size(T2, stderr); */
#endif

  if (SQ_FINAL_STRINGS_ARCS)
    transform_final_w_strings_to_fsm ( T2,  input_side );
  else
    {
      transform_final_w_strings_to_fsm_labels( T2, input_side, SQ_LABELS_BUFFER, SQ_BU_LE);
    }
  free (SQ_LABELS_BUFFER);
  reclaim_seq_memory();
  if (T2)
    {
      input_side ? (NET_lower_seq(T2) = TRUE) : (NET_upper_seq(T2) = TRUE);
      sq_normalize_sigma_and_labels_alphabet(T2);
      
      /* New 1999-2-4: if the original net didn't have any ?:? and the sequentialized
	 one does but no other ?:x y:? than we can transofrm ?:? to ?.
	 Otherwise more profound test is needed!
	 */
      if (! has_net_OTHER_to_OTHER(T1, FALSE, TRUE))
	sq_smooth_OTHER_OTHER_to_OTHER(T2, FALSE);
      
      /* minimization. Changes fst structure */
      min_fsm(T2, DONT_COPY);
      reorder_arcs(T2, input_side ? seq_lower_comp_fn : seq_upper_comp_fn ); 
    }
  if (reclaim_p == DO_RECLAIM)
    free_network(T1);

  /* If the net's arcs were previously sorted, this arc ordering is lost. */
  NET_sorted_arcs(T1) = 0;
  NET_sorted_arcs(T2) = 0;
  
  return (T2);
}

/* ======================================================================= */
/* --- Subsequentialization state-wise, to a given depth ----------------- */

static ARCptr next_arc_of_eps_closured_state(STATEptr pst, 
					     Q2_LISTptr *ec,
					     Q2_LISTptr *actual_ec,
					     ARCptr arc, 
					     id_type * init_id)
     /* If a state PST has an epsilon closure list - EC - then not only its own
	arcs (on the STATE_arc_set list) but also the non-epsilon arcs (wrt. a
	given side) of the further epsilon closure states must be considered at
	various sequentialization algorithms.
	This function gives all (own and closure) arcs, once and only once, in
	an orderly way, one after the other in subsequent calls.
	   IN parameters: pst, arc;
	   IN/OUT parameters: ec, actual_ec;
	   OUT parameter: init_id;
	Returns the next arc of arc (in the extended, closured sense) if exists
	otherwise NULL.
	Beware, the parameters EC and ACTUAL_EC are relevant and may be changed
	(in/out parameters).  
	EC serves to administer the epsilon closures. If own arcs of PST
	are exhausted, and there is an EC list, then its first element (which is
	meant for other purposes...) is stepped over. The subsequent EC elements are
	representing epsilon-reachable states.

	ACTUAL_EC: this is needed when PST is exhausted (until then it MUST be 0!)
	to tell the original epsilon arc (on PST) where from this actual arcs comes:
	INIT_ID identifies the other side of the initial epsilon arc. It is needed
	to find the arc itself (deterministic automaton is needed for this!).	
	*/
{
  ARCptr result = 0;

  if (arc)
    result = next_arc (arc);
  *init_id=0;

  if ( (! result) && *ec)
    {
      /* 98-11-24 step over the "self" (first) epsilon closure object */
      if (pst == QL_st(*ec))
	*ec = QL_link(*ec);

      while ( *ec && (! (result = STATE_arc_set(QL_st(*ec)))) )
	{	  
	  *ec = QL_link(*ec);
	}
      if (result && *ec )
	{
	  if (QL_w_head(*ec))
	    *init_id = OC_id(QL_w_head(*ec));
	  *actual_ec = *ec;
	  *ec = QL_link(*ec);
	}
    }
  if (result && *actual_ec)
    {
      if (QL_w_head(*actual_ec))
	*init_id = OC_id(QL_w_head(*actual_ec));
    }
  return result;
}

/* Too simple: what about Eps closures? */
static void mark_arc_and_dest_state_to_depth (int  mark, ARCptr arc,  int depth)
{
  ARCptr arc1;
  STATEptr dest;
  ARC_visit_mark(arc) = mark;
  if ( (depth > 0))
    {
      if ( (dest =  ARC_destination (arc)) )
	{
	  STATE_visit_mark(dest) = mark;
	  if (depth)
	    for ( arc1 =  STATE_arc_set(dest); arc1; arc1=next_arc(arc1))
	      mark_arc_and_dest_state_to_depth (mark, arc1, depth -1);
	  if ( ((depth-1 <0))) /* "MARK-with-self-address" last state in this depth */
	    STATE_client_cell (dest) = dest; 
	}
    }
}

#if NOT_USED
/* Too simple: what about Eps closures? */
static void mark_arc_and_dest_state_to_depth_2 (int  mark, ARCptr arc,  int depth)
{
  ARCptr arc1;
  STATEptr dest;
  ARC_visit_mark(arc) = mark;
  dest=ARC_destination(arc);
  if (dest)
    {
      if ( (depth-1) > 0 )
	{
	  for ( arc1=STATE_arc_set(dest) ; arc1 ; arc1=next_arc(arc1) )
	    mark_arc_and_dest_state_to_depth (mark, arc1, depth -1);
	  STATE_visit_mark(dest) = mark;
	}
    }

}
#endif /* NOT_USED */

/* 
   Local Sequentialization - 1998-1999

   The following attempt to Local Sequentialization is based on observations from
   replace calculus operations related pseudo-bugs (or non-optimal solutions) that
   revealed interesting, sequentiable subnets. The idea is identifying such
   sequentiable subnets, extracting them, sequentializing them, and putting them back
   to the original network. A particular subnet is taken into consideration only once. 

   The aim is two-fold: epsilon-pushing (towards "the end") and eradicating spurious
   (but otherwise correct) paths that may cause two much (identical) output,
   especially when further looping, involving ambiguous paths, is involved. Real
   ambiguity is preserved and - at looping situations - not all ambiguities can be
   eradicated.

   The choice of the subnet is determined by the two aims in the previous paragraph:
   note that the purpose is not full sequentialization (see also 1. below).

   In the "external" algorithm, that traverses all states of the net, several
   traversal strategies are possible: one can do it according to a (random) state
   list, following the NETtype body.state pointer or it can be done
   implementation-independently, starting by the initial state and stepping one state
   further in-depth and so on. There may be several such "external" traversal
   algorithms: this shouldn't concern the result but a traversal may be more
   optimal than another.

   1. If a state has epsilon-containing arcs (like a:0 or 0:b) this is detected. A
   subset of the arcs of the states is separated (according to the arcs, see later).
   
   2. A reachable subgraph (of the net) is marked. The start sate is the actual one
   fulfilling the condition under 1. States and arcs are marked, according to a
   later to be detailed algorithm that supposes, searches and identifies  a single
   stop-state (but see (1)) (or "destination" or "continuation" or "lattice-bottom"
   state) for the subnet - if such a state exists.  Otherwise there is no subnet
   and the next state is to be taken into consideration.
   There can be 
     - more than one stop states (example: two paths ending in different
       self-loops but leading nowhere else) 
     - a single stop state - this is the expected default.

   3. Then this subnet is marked as "working subnet" and its copy is created.
   The copy may contain slightly different initial (note, not necessarily start)
   and stop-states than the original one.

   4. The copy subnet is sequentialized (if sequentiable) and the original subnet is
   substituted by the sequentialized one. The new subnet is marked so as not to
   participate in further sequentializations.

   5. This (1-4) is done for all states of the net.

   There are several sub-algorithms for 1-5.

   (1) In the improved version more than one "stop-state" is possible.
   
   */

enum twobit_userflag2s {SQ_NOT_VISITED = 0, SQ_IN_PROCESS = 1, SQ_DONE = 2, SQ_STOP = 3};

#if NOT_USED
static void reset_userflag2s_to_zero(NETptr t1)
{
  ARCptr arc1;
  STATEptr st;
  assert (t1 && NET_states(t1));

  for ( st =  NET_states(t1); st ; st = next_state (st) )
    {
      STATE_userflag2(st) = 0;
      for (arc1 = STATE_arc_set(st); arc1 ; arc1 = next_arc (arc1))
	{
	  ARC_userflag2(arc1) = 0;
	}
    }
}
#endif /* NOT_USED */

static void reset_visit_marks_userflag2s_client_cells_to_zero(NETptr t1, int states_non_det)
{
  STATEptr st;
  ARCptr arc1;

  for ( st =  NET_states(t1);    st;    st = next_state (st) )
    {
      STATE_client_cell(st) = NULL;
      STATE_visit_mark(st) = 0; 
      STATE_userflag2(st) = 0;
      if (states_non_det)
	STATE_deterministic(st) = FALSE;
	  
      for (arc1 = STATE_arc_set(st); arc1 ; arc1 = next_arc (arc1))
	{
	  ARC_visit_mark(arc1) = 0;
	  ARC_userflag2(arc1) = 0;
	}
    }
  return;
}

#if NOT_USED
static void set_state_and_arc_userflag2s(NETptr t1, int mark)
{
  ARCptr arc1;
  STATEptr st;
  assert ((mark <= SQ_STOP) && t1 && NET_states(t1));

  for ( st =  NET_states(t1); st ; st = next_state (st) )
    {
      STATE_userflag2(st) = mark;
      for (arc1 = STATE_arc_set(st); arc1 ; arc1 = next_arc (arc1))
	{
	  ARC_userflag2(arc1) = mark;
	}
    }
}

static void set_state_userflag2s(NETptr t1, int mark)
{
  STATEptr st;
  assert ((mark <= SQ_STOP) && t1 && NET_states(t1));

  for ( st =  NET_states(t1); st ; st = next_state (st) )
      STATE_userflag2(st) = mark;
}
#endif /* NOT_USED */

static void set_arc_userflag2s(NETptr t1, int mark)
{
  ARCptr arc1;
  STATEptr st;
  assert ((mark <= SQ_STOP) && t1 && NET_states(t1));

  for ( st =  NET_states(t1); st ; st = next_state (st) )
    {
      for (arc1 = STATE_arc_set(st); arc1 ; arc1 = next_arc (arc1))
	{
	  ARC_userflag2(arc1) = mark;
	}
    }
}

static void count_incoming_arcs_of_states(NETptr fnet)
     /* count the number of incoming arcs of states of this net 
	and put the result into the client cell (of the state).
	*/
{
  STATEptr nstate;
  ARCptr narc;
#ifdef DEBUG
  int debug_arc_cnt = 0;
#endif /* DEBUG */

  if (! fnet)
    return;

  reset_visit_marks_userflag2s_client_cells_to_zero(fnet, FALSE);
  
  for (nstate = NET_states(fnet) ; nstate ; nstate = next_state(nstate))
    {
      for (narc = STATE_arc_set(nstate) ; narc ; narc = next_arc(narc))
	{
	  if (ARC_userflag2(narc))
	    continue;

	  /* cc has pb with:  STATE_client_cell(ARC_destination(narc))++;    */
	  STATE_client_cell(ARC_destination(narc)) = 
	    (char *) STATE_client_cell(ARC_destination(narc)) + 1;	 

#ifdef DEBUG	   
	  debug_arc_cnt++;
#endif /* DEBUG */

	  ARC_userflag2(narc) = 1;
	}
    }
#ifdef DEBUG
  if (debug_arc_cnt != NET_num_arcs(fnet))
      {
	fsm_fprintf (stderr, "\n\
*** SEQ INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
debug_cnt = %d ; NET_num_arcs =  %ld.\n", debug_arc_cnt, NET_num_arcs(fnet));
	      exit (1);
      }
#endif /* DEBUG */

  set_arc_userflag2s(fnet, 0);  
}

static int in_stop_state_set(NETptr fullnet, STATEptr loc_seq_init_state, STATEptr cstate)
     /*
	Check if this state is/can be  a "stop-state" of this net.
	Basic stop-state set:
	 - start state (of the original net)
	 - initial state (of local sequentialization)
	 - any Final state with no outgoing arcs and more than one incoming arcs. 
	   If in this case there is only one incoming arc then this state has not
	   to be considered as as stop state since it can be extracted and even
	   free-d during sequentialization, without effecting the original net
	   (when a subnet is extracted).
	 - any already "done" states (in local sequentialization, marked by SQ_DONE)

	Real stop states serve as anchoring points where arcs of extracted states
	will go. 

	There can be 0, 1, 2 or any number of stop states.
	   
	To do refined, good quality local sequentialization, finding a proper
	stop state set is primordial.
	*/
{
  assert (fullnet && cstate && NET_start_state(fullnet));

  if ((STATE_userflag2(cstate) == SQ_DONE) ||
      (cstate == loc_seq_init_state) || 
      (cstate == NET_start_state(fullnet)) ||
      (STATE_final(cstate) && ((int) STATE_client_cell(cstate) > 1) && ( !STATE_arc_set(cstate))))
    return TRUE; 
  return FALSE;
}


static STATEptr is_on_stop_state_list (STATEptr cstate, STOP_STATESptr stop_states)
     /* 
	If CSTATE is on the list of "stop states" returns TRUE otherwise FALSE.
	STOP_STATES contains the list of such states.
	*/
{
  STOP_STATESptr st1;
  
  assert(cstate);

  if (! stop_states)
    return NULL;

  for (st1 = stop_states; st1 ; st1 =  SS_l(st1) )
    {
      if (SS_st(st1) == cstate)
	return SS_st(st1);
    }
  return NULL;
}

static STATEptr is_new_of_old_on_stop_state_list (STATEptr cstate, STOP_STATESptr stop_states)
     /* 
	If CSTATE is on the list of "stop states" returns TRUE otherwise FALSE.
	STOP_STATES contains the list of such states.
	*/
{
  STOP_STATESptr st1;
  
  assert(cstate);

  if (! stop_states)
    return NULL;

  for (st1 = stop_states; st1 ; st1 =  SS_l(st1) )
    {
      if (SS_st(st1) == cstate)
	return SS_nst(st1);
    }
  return NULL;
}

#if NOT_USED
static STATEptr is_new_on_stop_state_list (STATEptr new_cstate, STOP_STATESptr old_stop_states)
     /* 
	If NEW_CSTATE (which is a copy of an old stop state) is on the list of "stop
	states" returns TRUE otherwise FALSE. 
	STOP_STATES contains the list of old stop states and copies in the (new) subnet.
	*/
{ 
  STOP_STATESptr st1;

  assert(new_cstate);

  if (! old_stop_states)
    return NULL;

  for (st1 = old_stop_states; st1 ; st1=  SS_l(st1) )
    {
      if (SS_nst(st1) == new_cstate)
	return SS_nst(st1);
    }
  return NULL;
}
#endif /* NOT_USED */


static void add_to_stop_states (STATEptr cstate, STOP_STATESptr *stop_states)
     /* Add to a list of stop states. If the list doesn't exist, create one.
      */
{
  STOP_STATESptr newsst=NULL;
  assert(cstate);
  if (! (*stop_states))
    {
      *stop_states = create_stop_state_cell(cstate);
      return;
    }
  else
    {
      if (! is_on_stop_state_list(cstate, *stop_states))
	{
	  newsst = create_stop_state_cell(cstate);
	  SS_l(newsst) = *stop_states;
	  *stop_states = newsst;
	}
    }
}

static void mark_arc_and_reachables_until_stop_state(NETptr fnet, 
						     STATEptr cstate,
						     STOP_STATESptr * stop_states,
						     ARCptr carc,  
						     int in_work)
     /* Mark this arc and destination and the closure of the arc until a "stop
	state" is reached.
	Don't mark already mark-ed arcs. 
	The value "in_work" mustn't be 0.
	 Don't mark stop states ? **
	*/
{
  ARCptr narc;

  if ((! carc) || (ARC_userflag2(carc)))
    return;

  ARC_userflag2(carc) = in_work;

  if  ( in_stop_state_set(fnet, cstate, ARC_destination(carc)))
    {
      if (! (is_on_stop_state_list(cstate, *stop_states)))
	add_to_stop_states(ARC_destination(carc), stop_states);
      return;
    }
  else if (ARC_destination(carc) != cstate)
    {
      if (! STATE_userflag2(ARC_destination(carc)))
	{
	  STATE_userflag2(ARC_destination(carc)) = in_work;

	  for (narc = STATE_arc_set(ARC_destination(carc)) ; narc ; 
	       narc = next_arc(narc))
	    {
	      mark_arc_and_reachables_until_stop_state(fnet, cstate, stop_states, narc, in_work);
	    }
	}
    }
}

static void sq_mark_userflag2_states_and_arcs(NETptr fnet, int newmark)
     /* All arcs and states are marked newmark. */
{
  ARCptr narc;
  STATEptr tstate;

  if ( (!fnet))
    return;
  
  for ( tstate = NET_states(fnet) ; tstate; tstate = next_state(tstate) )
    {
      STATE_userflag2(tstate) = newmark;

      for (narc = STATE_arc_set(tstate) ; narc ; narc = next_arc(narc))
	  ARC_userflag2(narc) = newmark;
    }
}

static void re_mark_userflag2s_states_and_arcs(NETptr fnet, int oldmark, int newmark)
     /* If an arc or state is marked with oldmark it will be changed to
	newmark. */
{
  ARCptr narc;
  STATEptr tstate;

  if ( (!fnet) || (oldmark == newmark) )
    return;
  
  for ( tstate = NET_states(fnet) ; tstate; tstate = next_state(tstate) )
    {
      if (STATE_userflag2(tstate) == oldmark)
	STATE_userflag2(tstate) = newmark;

      for (narc = STATE_arc_set(tstate) ; narc ; narc = next_arc(narc))
	if (ARC_userflag2(narc) == oldmark )
	  ARC_userflag2(narc) = newmark;
    }
}

static int mark_sequentiable_subnet_1(NETptr fullnet, int input_side, STATEptr cstate, 
				      STOP_STATESptr * stop_states, int in_work)
     /* 
	Try and find out whether this state has an arc-set that leads to a separable
	subnet that requires epsilon-pushing. If the answer is yes, mark the arcs
	and states (transitive closure 	until a "stop state" (see later) is reached),
	so that they (this subnet) can be extracted later.

	1. If it has an EPSILON on its lower side
	  a:0
	arc then all further upper-ambiguous
	  a, a:b, a:c, ...
	arcs will be included, as well as all of their offsprings states and
	arcs. 
	The union of all so created arc subsets is candidate for a `sequentialize
	downwards' operation. 

	2. Similarly, an arc with an EPSILON on its upper side
	  0:b
	requires all lower-ambiguous
	  b, d:b, c:b, ...
	arcs to be marked. The union of all so created arc subsets is candidate for
	a `sequentialize upwards' operation. 

	NOTE that 1. and 2. are not conditions for full sequentializations - rather
	indications for epsilon-pushing. There may be further { U, U:f } or  { L,
	i:L } type ambiguities (i.e. such arcs on this state). They may have to be
	addressed in another local sequentialization strategy.

	The union of the so (1. and 2.) created subnets - the extractable, and
	possibly to be sequentiable "complete" subnet - must meet the next conditions:

	- must have a "stop-state" where all traversal paths would conclude:

	    = either the start state of the whole net (meaning "looping back" there)
	    = or another such common state for all paths (where from the net may go
	    further to all directions)
	    = or a tail state (set) with no outgoing arcs (or with outgoing arcs
	    remaining in the subnet, that is,  their destination states are in the
	    subnet already)
	    = or sensible (i.e. interpretable) combinations of the preceding.

	    This extractability conditions/properties above mean simply that if this
	    subnet is extracted and transformed in any way, a semantically
	    equivalent, but topologically, etc. different FST must be able to
	    replace it. It will require one or two "hooks": one is the initial state
	    - this is given - the other is the "stop-state" - that may or may not
	    exist and must provide continuation (of both the original subnet and its
	    new transformed/sequentialized equivalent) towards the original net.

	- must be separable from the original net: that is, they must form a
        separable subgraph with the possible exception of the "subnet-start state"
	and the "subnet-stop-state".
	    = This condition will be checked by counting the incoming arcs of states.
	*/
{
 ARCptr narc, narc1;
 int  result = 0;
 id_type lowid, uppid;
 id_type (* iside_id) (id_type id);
 id_type (* oside_id) (id_type id); 

  if (! fullnet)
    return result;

  if (input_side == UPPER) /* direction is l-r (i.e. "a") at  a:b */
    {
      iside_id = upper_id;
      oside_id = lower_id;
    } 
  else 
    {
      oside_id = upper_id;
      iside_id = lower_id;
    }

  for (narc = STATE_arc_set(cstate) ; narc ; narc = next_arc(narc))
    {
      if (upper_id(ARC_label(narc)) == EPSILON)
	{
	  lowid = lower_id(ARC_label(narc));
	  
	  if (!ARC_userflag2(narc))
	    {
	      mark_arc_and_reachables_until_stop_state(fullnet, cstate, stop_states, narc, in_work);
	      result = TRUE;
	    }
	  
	  for (narc1 = STATE_arc_set(cstate) ; narc1 ; narc1 = next_arc(narc1))
	    {
	      if ( (!ARC_userflag2(narc1)) &&
		   (lower_id (ARC_label(narc1)) == lowid))
		mark_arc_and_reachables_until_stop_state(fullnet, cstate, stop_states, narc1, in_work);
	    }
	}
      else if
	(lower_id (ARC_label(narc)) == EPSILON)
	{
	  uppid = upper_id(ARC_label(narc));

	  if (!ARC_userflag2(narc))
	    {
	      mark_arc_and_reachables_until_stop_state(fullnet, cstate, stop_states, narc, in_work);
	      result = TRUE;
	    }
	  
	  for (narc1 = STATE_arc_set(cstate) ; narc1 ; narc1 = next_arc(narc1))
	    {
	      if ( (!ARC_userflag2(narc1)) &&
		   (upper_id(ARC_label(narc1)) == uppid))
		mark_arc_and_reachables_until_stop_state(fullnet, cstate, stop_states, narc1, in_work);
	    }
	}
    }
return result;
}

static int mark_sequentiable_subnet_2(NETptr fullnet, int input_side, STATEptr cstate, 
				      STOP_STATESptr * stop_states, int in_work)
     /* 
	Mark all mark-able arcs of this state irrespective of whether it has any
	epsilon-containing arc or not. This way we hope to catch cases that
	another marking algorithm - mark_sequentiable_subnet_1() - doesn't handle
	(see also there).
	*/
{
 ARCptr narc, narc1;
 int  result = 0;
 id_type lowid;
 id_type (* iside_id) (id_type id);
 id_type (* oside_id) (id_type id); 

  if (! fullnet)
    return result;

  if (input_side == UPPER) /* direction is l-r (i.e. "a") at  a:b */
    {
      iside_id = upper_id;
      oside_id = lower_id;
    } 
  else 
    {
      oside_id = upper_id;
      iside_id = lower_id;
    }

  for (narc = STATE_arc_set(cstate) ; narc ; narc = next_arc(narc))
    { 
      lowid = lower_id(ARC_label(narc));
	  
      if (!ARC_userflag2(narc))
	{
	  mark_arc_and_reachables_until_stop_state(fullnet, cstate, stop_states, narc, in_work);
	  result = TRUE;
	}
	  
      for (narc1 = STATE_arc_set(cstate) ; narc1 ; narc1 = next_arc(narc1))
	{
	  if ( (!ARC_userflag2(narc1)) &&
	       (lower_id (ARC_label(narc1)) == lowid))
	    mark_arc_and_reachables_until_stop_state(fullnet, cstate, stop_states, narc1, in_work);
	}
    }
  return result;
}

static int mark_sequentiable_subnet_3(NETptr fullnet, int input_side, STATEptr cstate, 
				      STOP_STATESptr * stop_states, int in_work)
     /* 
	If it has EPSILON on its input side (like <a:0> if input_side=UPPER)
	then this arc and all states and arcs reachable from here are marked.
	The union of all so created arc subsets is candidate for a `sequentialize upwards' operation. 
	*/
{
 ARCptr narc;
 int  result = 0;
 id_type lowid;
 id_type (* iside_id) (id_type id);
 id_type (* oside_id) (id_type id); 

  if (! fullnet)
    return result;

  if (input_side == UPPER) /* direction is l-r (i.e. "a" at <a:b>) */
    {
      iside_id = upper_id;
      oside_id = lower_id;
    } 
  else 
    {
      oside_id = upper_id;
      iside_id = lower_id;
    }

  for (narc = STATE_arc_set(cstate) ; narc ; narc = next_arc(narc))
    {
      if (iside_id(ARC_label(narc)) == EPSILON)
	{
	  lowid = lower_id(ARC_label(narc));
	  
	  if (!ARC_userflag2(narc))
	    {
	      mark_arc_and_reachables_until_stop_state(fullnet, cstate, stop_states, narc, in_work);
	      result = TRUE;
	    }
	}
    }
return result;
}

#if NOT_USED
static int all_arcs_of_state_marked (STATEptr cstate, int cmark)
     /* Report whether every single arc of this state is marked by cmark or not */
{
 ARCptr narc;
 
  if (! cstate)
    return FALSE;

  for (narc = STATE_arc_set(cstate) ; narc ; narc = next_arc(narc))
    {
      if (! (ARC_userflag2(narc) == cmark))
	{
	  return FALSE;
	}
    }
  return TRUE;
}
#endif /* NOT_USED */

static STATEptr is_this_old_state_copied(STATEptr oldstate, NETptr newnet, int mark_of_copied)
     /* Check whether this state - from the old net - has a copy in the new.
	Copied states are probably marked as SQ_STOP.
      */
{
  STATEptr nst=NULL;

  if (!( oldstate && newnet))
    return nst;

  if (STATE_userflag2(oldstate) == mark_of_copied)
    for ( nst=NET_states(newnet) ; nst ; nst=next_state(nst) )
      {
	if (STATE_client_cell(nst) == oldstate)
	  return nst;
      }
  return NULL;
}

#if NOT_USED
static void copy_client_cells_from_old_net(NETptr oldnet, NETptr newnet)
{
  /* The client cells of the states of the new net contain the ancestor state
     (except for the start state and one or more  possible "newstopstate").
     The client cells of the ancestor state (containing incoming arc counter in
     this application) will replace their addresses. */

 STATEptr wst;

 if( !(oldnet && newnet))
   return;

  for (wst = NET_states(newnet) ; wst ; wst = next_state(wst))
    {
      if (STATE_userflag2(wst) == SQ_STOP)
	continue;
      if (STATE_client_cell(wst) &&
	  (wst != NET_start_state(newnet)))
	STATE_client_cell(wst) = STATE_client_cell((STATEptr) STATE_client_cell(wst));
    }
}

static int test_subnet_separability (NETptr newnet, STOP_STATESptr stop_states)
     /*
       Verify if this (sub)net is self-contained, that is, whether the number of
       incoming arcs of a state is equal to the client-cell counters of states.
       These counters are (must be there already) inherited (copied) from the
       ancestor (full) network at this point.
       The "anchor" states (marked by SQ_STOP) that serve to get back to the original
       (full) net have no valid client_cell counter so they can be ignored so can the
       start sate of newnet!
       *** 1999-1-29 not needed since the new idea (of LK) of using pruning is better.
	*/
{
  ARCptr narc;
#ifdef DEBUG
  int debug_arc_cnt = 0;
#endif /* DEBUG */
  int result=FALSE;
 STATEptr nstate;

  if (! newnet)
    return result;

  set_arc_userflag2s(newnet, 0);

  for (nstate = NET_states(newnet) ; nstate ; nstate = next_state(nstate))
    {
      if ( is_new_on_stop_state_list(nstate, stop_states))
       continue;
      for (narc = STATE_arc_set(nstate) ; narc ; narc = next_arc(narc))
	{
	  if (ARC_userflag2(narc))
	    continue;
	  /** cc has pb-s with: STATE_client_cell(ARC_destination(narc))--;  */
	  STATE_client_cell(ARC_destination(narc)) = 
	    (char *)STATE_client_cell(ARC_destination(narc)) - 1;	 
#ifdef DEBUG	   
	  debug_arc_cnt++;
#endif /* DEBUG */

	  ARC_userflag2(narc) = 1;
	}
    }

  for (nstate = NET_states(newnet), result = TRUE ; nstate ; nstate =  next_state(nstate))
    {
      if ((STATE_userflag2(nstate) == SQ_STOP) || ( nstate == NET_start_state(newnet)))
	continue;

      if (STATE_client_cell(nstate))
	{
	  result = FALSE;
	  break;
	}
    }
  reset_userflag2s_to_zero(newnet);
  reset_visit_marks_userflag2s_client_cells_to_zero(newnet, FALSE);
  return result;
}
#endif /* NOT_USED */

#define lattice_handle_arc "**SQ_LATTICE_HANDLE_ARC_0000*"
#define SQ_RECURSION_LIMIT 50000
/* SQ_BOTH_SIDES_LOCAL_SEQUENTIALIZATION_LIMIT is the number of states that governs
   local sequentialization whether to sequentialize on the both sides or just on the
   input side (if the subnet size in terms of  states is bigger) */ 
#define SQ_BOTH_SIDES_LOCAL_SEQUENTIALIZATION_LIMIT 100

char silly_conv_singlenum_to_char(int Int)
{ 
  switch (Int)
    {
    case 0: return '0';
    case 1: return '1';
    case 2: return '2';
    case 3: return '3';
    case 4: return '4';
    case 5: return '5';
    case 6: return '6';
    case 7: return '7';
    case 8: return '8';
    case 9: return '9';
    default: 
      return '?' ;
    }
}

#define SQ_MAX_ID_CHAR_LENGTH 5
inline
void silly_conv_num_to_char(int num, char * buf, int *level)
     /* 
	 Convert this integer to a long enough buffer as a sequence of characters
	 There must exist a library routine but it is too late to find it ...
	 (buf - MAX_ID_CHAR_LENGTH) must be writable!
	  */
{
  int n=0, m ;
  (*level)++;
  if(*level > SQ_MAX_ID_CHAR_LENGTH )
    { 
      (*level)--;
      return;
    }
  n = (num) % 10 ;
  m = (num) / 10;
 
  buf[0]=silly_conv_singlenum_to_char(n);

  if (! (m))
    return;

  silly_conv_num_to_char(m, buf-1,  level);
  (*level)--;
}

#define SQ_TWO 2

static NETptr extract_by_copy_marked_subnet(NETptr fnet,
					    NETptr subnet,
					    STATEptr copy_start_state, 
					    STOP_STATESptr copy_stop_states,
					    int work_mark,
					    int * recursion_counter)
     /* Extract the net of marked - by work_mark - descendants (arcs and states) of
	copy_start_state until (NOT including) copy_stop_state. 
	Create a FINAL state and substitute it for copy_stop_state.
	Firstly, client-cells of new states point to old corresponding states.
	Then copy client cells for extractability check!
	Copies of copy_stop_states get marked (by SQ_STOP) so that they can be handled
	specially: their client cells won't be copied in
	copy_client_cells_from_old_net().
	The purpose of this marking is to help est_subnet_separability() in deciding
	separability. 
	*/
{
  ARCptr oarc;
  char buf[sizeof(lattice_handle_arc)+SQ_MAX_ID_CHAR_LENGTH+SQ_TWO]={'\0'}; 
  int csti, level=0; 
  id_type lha;
  NETptr new_subnet=NULL;
  STATEptr lattice_bottom_state=NULL, nss=NULL, nst=NULL, ndest=NULL;
  STOP_STATESptr cst;

  if (! (*recursion_counter && subnet))
    {
      new_subnet = make_network(); 
      NET_labels(new_subnet) = copy_alphabet(NET_labels(fnet)) ; 
      NET_sigma(new_subnet) = sigma(new_subnet);

      NET_start_state(new_subnet) = make_state(NULL, new_subnet);
      nst = NET_start_state(new_subnet);
    }
  else
    new_subnet = subnet;

  (*recursion_counter)++;

  if ( (*recursion_counter==1) &&
       copy_stop_states)
    {
      for (cst = copy_stop_states, csti=1, level=0; cst; cst=SS_l(cst), level=0, csti++)
	{
	  if (! (SS_nst(cst)))
	    {
	      nss = make_state(NULL, new_subnet);
	      STATE_final(nss) = FALSE;
	      STATE_userflag2(nss) = SQ_STOP;
	      SS_nst(cst) = nss;
	    }
	  else
	    nss = SS_nst(cst);
	  lattice_bottom_state = make_state(NULL, new_subnet);
	  STATE_final(lattice_bottom_state) = TRUE;
	  STATE_userflag2(lattice_bottom_state) = SQ_STOP;
	  STATE_client_cell(lattice_bottom_state) = (void *) 1;
	  strncpy(buf,lattice_handle_arc, sizeof(lattice_handle_arc));
	  silly_conv_num_to_char(csti, buf+sizeof(lattice_handle_arc)-3, &level);
	  FST_intern_literal(&lha, buf);
	  if (lha)
	    {
	      SS_id(cst)=lha;
	      label_to_sigma(NET_sigma(new_subnet), lha);
	      binary_to_label(sigma_add_to(NET_labels(new_subnet), lha));
	      add_new_arc(new_subnet, nss, lha, lattice_bottom_state);  
	    }
	  else
	    {
	      fsm_fprintf (stderr, "\n\
*** SEQ INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
No lattice_handle_arc.\n");
	  exit (1);
	    }
	}
    }

  if (! (nst ||
	 (nst = is_this_old_state_copied(copy_start_state, new_subnet, SQ_STOP))))
    {
      nst = make_state(NULL, new_subnet); 
      STATE_client_cell(nst) = copy_start_state;
      if (STATE_final(copy_start_state))
	STATE_final(nst) = TRUE;
    }

  for (oarc = STATE_arc_set(copy_start_state) ; oarc ; oarc = next_arc(oarc))
    {
      if (ARC_userflag2(oarc) != work_mark)
	continue;

      ARC_userflag2(oarc) = SQ_STOP;

      if (ARC_destination(oarc) == copy_start_state)
	{
	  /*  copy_start_state can be a stop state itself. */
	  if   (! (ndest = is_new_of_old_on_stop_state_list(ARC_destination(oarc), copy_stop_states)))
	    ndest = nst;
	}
      else
	{
	  if (! (ndest = is_new_of_old_on_stop_state_list(ARC_destination(oarc), copy_stop_states)))
	    {
	      if (! (ndest = is_this_old_state_copied(ARC_destination(oarc),
						      new_subnet, SQ_STOP)))
		{
		  ndest = make_state(NULL, new_subnet); 
		  STATE_client_cell(ndest) = ARC_destination(oarc);
		  if (STATE_userflag2(ARC_destination(oarc)) == work_mark)
		    STATE_userflag2(ARC_destination(oarc)) = SQ_STOP;
		  if (STATE_final(ARC_destination(oarc)))
		    STATE_final(ndest) = TRUE;
		}
	  
	      if (ARC_destination(oarc) != copy_start_state)
		{
		  assert ((*recursion_counter) < SQ_RECURSION_LIMIT);
		  extract_by_copy_marked_subnet(fnet, new_subnet, ARC_destination(oarc),
						copy_stop_states, work_mark, recursion_counter);
		}
	      else
		ndest = nst;
	    }
	}

      if  (! ndest)
	{
	  fsm_fprintf (stderr, "\n\
*** SEQ INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
No destination state for arc.\n");
	  exit (1);
	}
      add_new_arc(new_subnet, nst, ARC_label(oarc), ndest);
    }
    
  (*recursion_counter)--;
  return(new_subnet);
}

static void sq_remove_state_from_net(NETptr fnet, STATEptr dstate, int report_if_not_found)
     /* delete state from NET_states list; free dstate
	Arc list of dstate must be empty!
	*/
{
   STATEptr curr, prev = NULL;
     curr = NET_states(fnet);

  if (!(dstate && fnet))
      fsm_fprintf (stderr, "\n\
** SEQ INTERNAL PROGRAMMING WARNING ** Please, inform the programmer.\n\
No net or state to be removed.\n");

  for (curr = NET_states(fnet); curr; curr = next_state (curr))
    {
      if (curr == dstate)
	{
	  if (prev == NULL)
	    NET_states(fnet) = next_state (dstate);
	  else
	    next_state(prev) = next_state(curr);
	  free_state(dstate); 
	  NET_num_states(fnet)--;
	  return;
	}
      prev = curr;
    }

  if (report_if_not_found)
    fsm_fprintf (stderr, "\n\
** SEQ INTERNAL PROGRAMMING WARNING ** Please, inform the programmer.\n\
State to be removed is not in the net.\n"); 
}

#if NOT_USED
static void remove_marked_subnet(NETptr fnet, int work_mark)
     /* Remove all arcs and states marked by work_mark from fsnet.
	This subnet must be separable from the original one, that is, no
	external arcs should arrive to such marked states and no marked
	arcs (except for those arriving to a "stop-state") should go to
	there than marked states. 
	*** Suboptimal: could be done directly from the start state of
	the subnet rather than doing on the whole 
	1999-1-28: not needed any more since the prune idea of LK works better. TG */
{
  ARCptr nextarc, narc;
  STATEptr nextst, nstate;
 
  for (nstate = NET_states(fnet) ; nstate ; nstate = nextst)
    {
      nextst = next_state(nstate);
      for (narc = STATE_arc_set(nstate) ; narc ; narc = nextarc)
	{
	  nextarc = next_arc(narc);
	  if (ARC_userflag2(narc) == work_mark)
	    cfsm_remove_arc((cfsm_arc) narc, (cfsm_state) nstate,
			    (cfsm_fsm) fnet);
	}
      
      if (STATE_userflag2(nstate) == work_mark)
	{
	  if (STATE_arc_set(nstate))
	    {
	      fsm_fprintf (stderr, "\n\
*** SEQ INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
State must have no arcs at this point.\n");
	      exit (1);
	    }
	  sq_remove_state_from_net(fnet, nstate, TRUE);	   
	}  
    }
}
#endif /* NOT_USED */

static void push_sym_to_end_side(STATEptr pstate, ARCptr parc, 
				 id_type this_side_sym, int this_side)
     /* The this_side_sym symbol must be transmit to the end of the path on
	this side (UPPER or LOWER). 
	The (remaining) path must be fit for the purpose, for example,
	let A be the symbol to  be propagated;  an initial
	A:a 0:b 0:c 0:d 0:A 
	is acceptable. It will become, by push_sym_to_end_side(arc, A, upper):
	0:a 0:b 0:c 0:d A
	Mustn't be circular.
	Sigma and label cleanup, etc. remains to higher caller levels.*/ 
{
   ARCptr narc;
   id_type other_side_sym, new_label;
   STATEptr ds;

   if (!( narc=STATE_arc_set(ARC_destination(parc))))
     {
       if ( this_side == UPPER )
	 {
	   other_side_sym = lower_id(ARC_label(parc));
	   make_new_T2_label(&new_label, this_side_sym, other_side_sym, this_side);
	   ARC_label(parc) = new_label;
	 }
       else
	 {
	   other_side_sym = upper_id( ARC_label(parc));
	   make_new_T2_label(&new_label, this_side_sym, other_side_sym, this_side);
	   ARC_label(parc) = new_label;
	 }
       return;
     }
   else if ( this_side == UPPER )
       {
	 other_side_sym = lower_id(ARC_label(parc));
	 make_new_T2_label(&new_label, EPSILON, other_side_sym, this_side);
	 ARC_label(parc) = new_label;
	 ds=ARC_destination(parc);
	 for (narc=STATE_arc_set(ds); narc; narc=next_arc(narc))
	   {
	     other_side_sym = lower_id( ARC_label(narc));
	     push_sym_to_end_side(ds, narc, this_side_sym, UPPER);
	   }
       }
     else
       {
	 other_side_sym = upper_id( ARC_label(parc));
	 make_new_T2_label(&new_label, EPSILON, other_side_sym, this_side);
	 ARC_label(parc) = new_label;
	 ds=ARC_destination(parc);
	 for (narc=STATE_arc_set(ds); narc; narc=next_arc(narc))
	   {
	     other_side_sym = upper_id( ARC_label(narc));
	     push_sym_to_end_side(ds, narc, this_side_sym, LOWER);
	   }
       }
   return;
}

#if NOT_USED
static int only_one_arc_or_less_on_state(STATEptr sstate, id_type id)
{
  assert(sstate);
 
  if ((! STATE_arc_set(sstate)) ||
      (STATE_arc_set(sstate) && ( ! next_state(STATE_arc_set(sstate))) && 
       (ARC_label(STATE_arc_set(sstate))== id)))
    return TRUE;
  return FALSE;
}
#endif /* NOT_USED */

static STATEptr re_normalize_anchor_arc_and_states(NETptr s_net, STOP_STATESptr stop_state)
{
  /* A single, non-looping, final suffix of ALL (upper and lower) paths,
   *ANCHOR-LINE*, of this network, is expected (by construction). In the net,
   there  must be similar common pieces for all such special path suffixes:

     s_anchor_1_1_state -*ANCHOR-LINE*1-> fs_anchor_1_2_state
     s_anchor_2_1_state -*ANCHOR-LINE*2-> fs_anchor_2_2_state
     ...
     s_anchor_n_1_state -*ANCHOR-LINE*2-> fs_anchor_n_2_state

     But during various sequentializations of s_net such a sub-path may be changed
     to     

     s_j -a:*ANCHOR-LINE*-> s_i -*ANCHOR-LINE*:0-> fs_anchor_2_state
     s_m -b:*ANCHOR-LINE*-> s_b -*ANCHOR-LINE*:0-> fs_anchor_2_state
     ...
     s_k -*ANCHOR-LINE*:c-> s_l -0:*ANCHOR-LINE*-> fs_anchor_2_state
   
     etc. (semantically equivalent) pieces.

     The desired outlay is restored here (for one such anchor label): first the

     s_anchor_1_state -*ANCHOR-LINE*-> fs_anchor_2_state

     outlay is achieved (by pushing labels for path suffixes) then
     this suffix, including the states, is deleted (since they are ancillary
     markers of "end of net" only) BUT the arcs pointing to s_anchor_1_state(s) preserve this reference for later processing!
   */

  ARCptr narc;
  id_type al, anchor, up, lo;
  STATEptr nstate, s_anchor_2_state=NULL;

 if (! s_net )
   return s_anchor_2_state;

 update_label_alphabet(s_net);

 if (! stop_state)
   return NULL;

 if (! ALPH_type(NET_labels(s_net)) ==  LABEL_VECTOR)
   binary_to_label(NET_labels(s_net));

 anchor= SS_id(stop_state);
 assert(anchor);
 set_arc_userflag2s(s_net, 0);

 for (nstate = NET_states(s_net) ; nstate ; nstate = next_state(nstate))
   {
     for (narc = STATE_arc_set(nstate) ; narc ; narc = next_arc(narc))
       {
	 al = ARC_label(narc);
	 if (LABEL_arity(id_to_label(al)) > 1)
	   {
	     up = upper_id(al);
	     lo = lower_id(al);  
	     if (up == anchor)
	       {
		 if (ARC_destination(narc))
		   {
		     push_sym_to_end_side(nstate, narc, anchor, UPPER);
		   }
		 else
		   {
		     fsm_fprintf (stderr , "\n\
*** SEQ INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
No destination state.\n");
		     exit (1);
		   }
	       }
	     else if (lo == anchor)
	       {
		 if (ARC_destination(narc))
		   {
		     push_sym_to_end_side(nstate, narc, anchor, LOWER);
		   }
		 else
		   {
		     fsm_fprintf (stderr , "\n\
*** SEQ INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
No destination state.\n");
		     exit (1);
		   }
	       }	    
	   }
       }
   }

#ifdef SQP_DEBUG
 if (yes_or_no("\n** re_normalize_anchor_arc_and_states() - debug print subnet?", 'n'))
   debug_print_net(s_net, TRUE);
#endif /* SQP_DEBUG */

 update_label_alphabet(s_net);
 
 /* find anchor states (from, to): the net does have anchor symbol(s).
    s_anchor_2_state is where the arcs(s) labelled by "anchor" point at;
    it should be changed to the original "stop state".
     */

 for (nstate = NET_states(s_net) ; nstate ; nstate = next_state(nstate))
   {
     for (narc = STATE_arc_set(nstate) ; narc ; narc = next_arc(narc))
       {
	 al = ARC_label(narc);
	 if (al == anchor)
	   {
	     s_anchor_2_state = ARC_destination(narc);
	     ARC_destination(narc) = SS_st(stop_state);
	     ARC_label(narc) = EPSILON;
	   }
       }
   }
 return s_anchor_2_state;
}

static void insert_subnet_segment(NETptr f_net, NETptr s_net, STATEptr f_state,
				  STOP_STATESptr f_stop_states, int work_mark, int finished_mark)
{
  /* Insert s_net into f_net at f_state so that the arcs of the start
     state of s_net become arcs of fstate of f_net. Also, upper and lower paths
     of s_net may have one or more special suffixes, **SPECIAL*1, **SPECIAL*2, ...
     **SPECIAL*n, to assure, after sequentialization, re-synchronization with the
     original f_net (If and when needed). 
   
     It is represented, in (the original version, before sequentialization, of)
     s_net by a singe (unique) arc, labelled by a special symbol, let us call it
     *ANCHOR-LINE*, which is a single (unique) arc between two states (of
     s_net). The destination state of *ANCHOR-LINE* is final and has 
     no arcs. This pattern means

     s_anchor_1_state -*ANCHOR-LINE*-> s_anchor_2_state

     The source state of the *ANCHOR-LINE*-labelled arc (in s_net), let's
     call it s_anch_1_state, corresponds to the f_stop_state arc (in
     f_net), that is, arcs arriving to s_anch_1_state must be reoriented
     to f_stop_state.

     During sequentialization, in extreme cases, *ANCHOR-LINE* may undergo
     confusingly looking but equivalent transformations like

         a *ANCHOR-LINE* => a:0 *ANCHOR-LINE*:a 0:*ANCHOR-LINE*
     or
         a b c *ANCHOR-LINE*  => a:0 b:0 c:0 *ANCHOR-LINE*:0 0:a 0:b 0:c 0:*ANCHOR-LINE*
     
     Even if this phenomenon is not frequent, these must be re-normalized into
     the left sides (of the transformation) since this is what governs us in
     anchoring back the sequentialized s-net into the original f_net.

     The new (sub)net is marked as finished (no more local sequentialization allowed). 
     */
  
  ARCptr larc, narc;
  STATEptr lst=NULL, nstate, nstst;
  STOP_STATESptr sst;

  /* sets client cells to the number of incoming arcs */
  count_incoming_arcs_of_states(s_net); 

  /* "continuation" arcs of s_net between s_net and f_net re-targeted to f_net */

  for (sst = f_stop_states; sst ; sst = SS_l(sst))
    {
      SS_nst(sst)=re_normalize_anchor_arc_and_states(s_net, sst);
    }

  sq_mark_userflag2_states_and_arcs(s_net, finished_mark);


  for (sst = f_stop_states; sst ; sst = SS_l(sst))
    {
      STATE_client_cell(SS_nst(sst)) = (char *) STATE_client_cell(SS_nst(sst)) - 1;
      if (! STATE_client_cell(SS_nst(sst)) ) /* may be pointed at by several arcs */
	sq_remove_state_from_net(s_net, SS_nst(sst), TRUE);
    }
  
  /* remove marked arcs from the state */
  for (narc = STATE_arc_set(f_state); narc ; narc = next_arc(narc))
    {
      if (ARC_userflag2(narc) == work_mark)
	cfsm_remove_arc((cfsm_arc) narc, (cfsm_state) (f_state),
			    (cfsm_fsm) f_net);
    }

  sq_mark_userflag2_states_and_arcs(s_net, finished_mark);

  /* unite arc lists of s_net start state and initial state */
  for (narc = STATE_arc_set(NET_start_state(s_net)); narc ; narc = next_arc(narc))
    {
      larc = narc;
    }

  if(larc)
    {
      next_arc(larc) = STATE_arc_set(f_state);
      STATE_arc_set(f_state) = STATE_arc_set(NET_start_state(s_net));
      STATE_arc_set(NET_start_state(s_net))=NULL;
    }

  nstst=NET_start_state(s_net);
  NET_start_state(s_net)=next_state(NET_start_state(s_net));
  sq_remove_state_from_net(s_net, nstst, TRUE);

  /* unite state lists of s_net and f_net */
  for (nstate = NET_states(s_net) ; nstate ; nstate = next_state(nstate))
   {
     lst=nstate;
   }
  if (lst)
    {
      next_state(lst) = NET_states(f_net);
      NET_states(f_net) = NET_states(s_net);
      NET_states(s_net) = NULL;
    }

  /* administration of state and arc numbers, free-ing remaining s_net chunk */

 NET_num_arcs(f_net) = NET_num_arcs(f_net) + NET_num_arcs(s_net);
 NET_num_states(f_net) = NET_num_states(f_net) + NET_num_states(s_net);
 NET_num_arcs(s_net) = 0;
 NET_num_states(s_net) = 0;
 free_network(s_net); 
}

static int local_sequentialize_state(NETptr fnet,
				     STATEptr fstate, 
				     int input_side,
				     int sequentialize_other_side_too,
				     int mark_sequentiable_subnet(NETptr fullnet, int input_side, STATEptr cstate,
								  STOP_STATESptr * stop_states, int in_work),
				     FST_CNTXTptr fst_cntxt,
				     int * success_sep_count,
				     int * success_inp_sequ_count,
				     int * success_outp_sequ_count)
     /* 
     For this state, do:

     Check if there is an epsilon-pushable subnet (has a:0 or 0:b type arcs). 
       If yes then mark it - both its states and arcs (to SQ_IN_PROCESS).
         Extract - by copy - the marked subnet. Initial and "stop" states
	 are handled specially.
           If sequentializable, do so, first on the other (output!) side then on
	   the given (input) side, if possible. 
	   (*** may have to be refined   **99-1-8 **)

           Delete the originally marked  subnet and insert back the
	   sequentialized one into fnet.
	   Report success.
  */
{
  int extract_recursion_counter=0, output_side=(!input_side),
    success=FALSE;
  NETptr newnet=NULL, newseqnet=NULL;
  STOP_STATESptr stop_states=NULL;

  if ( (! fnet) || (!fstate) )
    return success;

  if (mark_sequentiable_subnet(fnet, input_side, fstate, &stop_states, SQ_IN_PROCESS))
    {
#ifdef SQP_DEBUG
      if (yes_or_no("\n** local_sequentialize_state: print the FULL marked net?", 'n'))
	{
	  fsm_fprintf (stderr,"This State =%d=0x%x. Flag=%d.\n", (int) fstate, (int) fstate, STATE_userflag2(fstate));
	  debug_print_net(fnet, TRUE);
	}
#endif /* SQP_DEBUG */
      
      newnet = extract_by_copy_marked_subnet(fnet, NULL, fstate, stop_states, SQ_IN_PROCESS, &extract_recursion_counter);
      re_mark_userflag2s_states_and_arcs(fnet, SQ_STOP, SQ_IN_PROCESS);

      if (newnet)
	{
	  (*success_sep_count)++;
	  /** what if we don't minimize here ?
	    NET_minimized(newnet) = NET_pruned(newnet) = FALSE;
	    min_fsm(newnet, DONT_COPY);  ** ?? if no final state - in a non-final subnet ? **/
	  
#ifdef SQP_DEBUG	   
	  fprintf(stderr,"\n** local_sequentialize_state: (Sub)net sizes: ");
	  print_net_size((NETptr) newnet, stderr);
	  if (yes_or_no("\n** local_sequentialize_state: print (Sub)net?", 'n'))
	    {
	      print_net_s(newnet, 80, stderr);
	      fflush(stderr);
	    }
#endif /* SQP_DEBUG */
	  /* test sequentiability of the the new net */
	  if (sq_is_fst_sequentiable(newnet, input_side))
	    {
	      /* Try sequentializing first the other (!) side of the subnet */
	      if (sequentialize_other_side_too)
		{  
		  if ( (NET_num_states(newnet) < SQ_BOTH_SIDES_LOCAL_SEQUENTIALIZATION_LIMIT) &&
		       sq_is_fst_sequentiable(newnet, output_side))
		    {
		      /* sequentialize the other side, too - DO_RECLAIM, that is, frees newseqnet !*/
		      newseqnet = sq_subsequentialize(newnet, output_side, DO_RECLAIM, 0,0, fst_cntxt);
		      (*success_outp_sequ_count)++; 
		      newnet = newseqnet; 
		    }
#ifdef SQP_DEBUG
		  else
		    {
		      fprintf(stderr,"\n** local_sequentialize_state: this side OK but other side is NOT sequentiable.\n");
		    }
#endif /* SQP_DEBUG */
		}
	      /* sequentialize - DO_RECLAIM, that is, frees newnet !*/
	      newseqnet=sq_subsequentialize(newnet, input_side, DO_RECLAIM, 0,0, fst_cntxt);
	      (*success_inp_sequ_count)++;
	      newnet = NULL;
	      if (newseqnet)
		{
#ifdef SQP_DEBUG	   
		  if (yes_or_no("\n** local_sequentialize_state: print new seq (sub)net?", 'n'))
		    {
		      print_net_s(newseqnet, 80, stderr); 
		      print_net_size((NETptr) newseqnet, stderr);  fflush(stderr);
		    }
#endif /* SQP_DEBUG */

		  /* mark to untouchable and re-insert sequentialized subnet; 
		     cut marked arcs from original;
		     re-mark the actual extracted subnet as yet not done since others may
		     want to locally sequentialize it, too */
		  insert_subnet_segment(fnet, newseqnet, fstate, stop_states,
						  SQ_IN_PROCESS, SQ_DONE);
		  re_mark_userflag2s_states_and_arcs(fnet, SQ_IN_PROCESS, SQ_NOT_VISITED);
		  free_stop_state_cells(stop_states);
		  stop_states=NULL;
		  newseqnet=NULL;
		}
	    }
#ifdef SQP_DEBUG
	  else 
	    fprintf(stderr,"\n** local_sequentialize_state: NOT sequentiable\n");
#endif /* SQP_DEBUG */
	  success = TRUE;

	  newnet=NULL;
	}
      if (!success)
	{
#ifdef SQP_DEBUG
	  if (yes_or_no("\n**** local_sequentialize_state: try this: RE-MARK to DONE (y) or to 0 (n)?", 'n'))
	    re_mark_userflag2s_states_and_arcs(fnet, SQ_IN_PROCESS, SQ_DONE);
	  else
	    {
	      re_mark_userflag2s_states_and_arcs(fnet, SQ_IN_PROCESS, SQ_NOT_VISITED);
	      fprintf (stderr,"*** Re-marked as Not yet Visited!");
	    }
#else
	  /**** Check whether it is a good default ! ***/
	  re_mark_userflag2s_states_and_arcs(fnet, SQ_IN_PROCESS, SQ_NOT_VISITED);
#endif /* SQP_DEBUG */

#ifdef SQP_DEBUG
	  if (yes_or_no("\n** local_sequentialize_state: print the FULL marked net after re_mark (no success)?", 'n'))
	    {
	      fprintf (stderr,"This State  =%d=0x%x. Flag=%d.\n", (int) fstate, (int) fstate, STATE_userflag2(fstate));
	      debug_print_net(fnet, TRUE);
	    }
#endif /* SQP_DEBUG */
	}
    }

  if (success)
    {
      /*** if verbose: statistics; ***/
    }
		 
  return success;
}

void allocate_loc_seq_memory(void)
{
  STOP_STATES_HEAP = init_heap(sizeof(STOP_STATEStype),  16384/sizeof(STOP_STATEStype),
			     "LOC SEQ STOP_STATES");
}

void reclaim_loc_seq_memory(void)
{
  free_heap(STOP_STATES_HEAP);
  STOP_STATES_HEAP = NULL;
}

static void sq_standard_prune_fst(NETptr fnet, STATEptr fstate)
{
  if (fstate)
    STATE_deterministic(fstate)=FALSE;
  if (fnet)
    {
      NET_deterministic(fnet)=NET_minimized(fnet)=NET_pruned(fnet)=
	NET_epsilon_free(fnet)=NET_sorted_states(fnet)=FALSE;
      NET_visit_marks_dirty(fnet) = TRUE;
      update_label_alphabet(fnet);
      epsilon_remove(fnet, DONT_COPY); /* Changes visit marks and client cells so we use
					  userflag2-s for marking in LOCAL seq */
      prune_fsm(fnet, DONT_COPY);
    }
}

static void sq_standard_minimize_fst(NETptr fnet, int input_side)
{     
  if( fnet)
    {
      NET_deterministic(fnet) = NET_minimized(fnet) = NET_pruned(fnet) = 
	NET_epsilon_free(fnet) = NET_sorted_states(fnet) = FALSE;
      /* tell minimizer that states may be non-deterministic */
      reset_visit_marks_userflag2s_client_cells_to_zero(fnet, TRUE);
      NET_visit_marks_dirty (fnet) = FALSE; /* ! */
      update_label_alphabet(fnet);
      /*** actualize sigma, too! ***/
      min_fsm(fnet, DONT_COPY);
      reorder_arcs(fnet, input_side ? seq_lower_comp_fn : seq_upper_comp_fn);
    }
}

NETptr local_sequentialize_net(NETptr fnet, int input_side, FST_CNTXTptr fst_cntxt)
{
  ARCptr tarc;
  int  level=0, sc=0, success=0, success_sep_count=0, success_inp_sequ_count=0, success_outp_sequ_count=0 ;
  NETptr result=NULL;
  STACKptr sstack1=create_stack("1 Loc Seq States to reach"), sstack2=create_stack("2 Loc Seq States");
  STATEptr tstate;

  if (! fnet)
    return result;

  allocate_loc_seq_memory();

  count_incoming_arcs_of_states(fnet);

  tstate = NET_start_state(fnet);
  push(tstate, sstack1);
  
  while (STACK_depth(sstack1) > 0)
    {
      level++;
      while (STACK_depth(sstack1) > 0)
	{ 
	  sc++;
	  tstate = pop(sstack1);
	  if ( STATE_userflag2(tstate) == SQ_NOT_VISITED)
	    {
	      STATE_userflag2(tstate) = SQ_DONE;

	      if (local_sequentialize_state(fnet, tstate, input_side, TRUE,
						      mark_sequentiable_subnet_1, fst_cntxt,
						      &success_sep_count, &success_inp_sequ_count, &success_outp_sequ_count))
		{
		  success++; 
		  sq_standard_prune_fst(fnet, tstate);
		}
	    }
	  push (tstate, sstack2);
	}
  
      /* prepare next round */
      while (STACK_depth(sstack2) > 0)
	{
	  tstate = pop(sstack2);
	  for (tarc = STATE_arc_set(tstate) ; tarc ; tarc = next_arc(tarc))
	    {
	      if ((ARC_userflag2(tarc)==SQ_NOT_VISITED) &&
		  (STATE_userflag2(ARC_destination(tarc))==SQ_NOT_VISITED) &&
		  (ARC_destination(tarc)!=tstate))
		push (ARC_destination(tarc), sstack1);
	    }
	}
    }

  if (success)
    {
      /*** if verbose: statistics; ***/

      result = fnet;
      sq_standard_minimize_fst(result, input_side);
    }
#ifdef DEBUG
      if (result || success_sep_count || success_inp_sequ_count || success_outp_sequ_count)
	fsm_fprintf (stderr,"** local_sequentialize_net: level reached:%d\n\
\tSeparated subnets: %d;\n\
\t%d this (%s) side subnets sequentialized,\n\
\t%d other (%s) side subnets sequentialized.\n", level, success_sep_count, 
		     success_inp_sequ_count,  (( input_side) ? "LOWER": "UPPER"),
		     success_outp_sequ_count, ((!input_side) ? "LOWER": "UPPER"));
#endif /* DEBUG */

  free_stack(sstack1);
  free_stack(sstack2);
  reclaim_loc_seq_memory();
  return result;
}

static int check_net_input_side_U_or_L(int netside)
{
  if (! ((netside == LOWER) || (netside == UPPER)))
    {
      if (*fst_IY_VERBOSE())
	fsm_fprintf (stderr,"** Sequ. net input side must be %d (LOWER) or %d (UPPER) not %d.\n", LOWER, UPPER, netside);
      return FALSE;
    }

  return TRUE;
}

#define SQ_ALGO_LIMIT 3

NETptr local_from_to_sequentialize_net(NETptr fnet, 
				       int input_side, 
				       int from_level, 
				       int over_level,
				       int marking_algorithm,
				       FST_CNTXTptr fst_cntxt)
     /* 
	This version of local sequentialization takes a net and renders the states of
	levels (0 to from_level) and (over_level+1) stop-states, that is, no sequentialization
	will happen until/beyond them, only in-between (in a left-to-right
	traversal of paths of the net). 
	Since the new (99-1-26, LK) idea of using pruning and  epsilon-removal for
	putting back subnets, the routines used epsilon_remove() and prune_fsm()
	use the visit_mark and client_cell  fields of states, the information about
	having already visited a state/arc must be stored elsewhere.
	A new, 2-bit flag, userflag2, will be used, both for arcs and states.
	*/
{
  ARCptr tarc;
  int  level=0, save_sq_verbose=0, sc=0, success=0, success_sep_count=0, success_inp_sequ_count=0,
    success_outp_sequ_count=0, sequentialize_other_side_too=FALSE;
  int (* mark_sequentiable_subnet) (NETptr fullnet, int input_side, STATEptr cstate,
				    STOP_STATESptr * stop_states, int in_work);
  NETptr result=NULL;
  STACKptr sstack1=create_stack("1 Loc Seq States to reach"), sstack2=create_stack("2 Loc Seq States");
  STATEptr tstate;

  if (! check_net_input_side_U_or_L(input_side))
      return result;

  if (! fnet)
    return result;

  if ((from_level || over_level) &&
      ((from_level < 0 ) || (over_level < 0 ) || (from_level >= over_level)))
    {
       if (*fst_IY_VERBOSE())
	 fsm_fprintf (stderr,
"** WARNING local_sequentialize_net: invalid \"from_level (%d)\" or \"over_level (%d)\"\n\
values. Return without any work.\n", from_level, over_level);
      return result;
    }

  allocate_loc_seq_memory();

   if (!((marking_algorithm == 0) || (marking_algorithm <= SQ_ALGO_LIMIT)))
    {
      if (*fst_IY_VERBOSE())
	fsm_printf("Warning: only 0 <= %d allowed for \"marking_algorithm\". 0 taken instead of %d.\n", SQ_ALGO_LIMIT, marking_algorithm);
      marking_algorithm = 0;
    }

  save_sq_verbose = *fst_IY_VERBOSE();
  *fst_IY_VERBOSE() = 0;
 
  switch (marking_algorithm)
    {
    case 0:
      mark_sequentiable_subnet = mark_sequentiable_subnet_3;
      break;
    case 1:
      mark_sequentiable_subnet = mark_sequentiable_subnet_2;
      break;
    case 2:
      mark_sequentiable_subnet = mark_sequentiable_subnet_1;
      sequentialize_other_side_too = TRUE;
      break;
    case 3:
      mark_sequentiable_subnet = mark_sequentiable_subnet_1;
      sequentialize_other_side_too = FALSE;
      break;
    default:
      mark_sequentiable_subnet = mark_sequentiable_subnet_3;
    }

  /* Mark states of level (over_level+1) as stop states (inhibiting any further sequentialization)
   */

  if (over_level)
    {
      tstate = NET_start_state(fnet);
      push(tstate, sstack1);
  
      while (STACK_depth(sstack1) > 0)
	{
	  level++;
	  while (STACK_depth(sstack1) > 0)
	    { 
	      sc++;
	      tstate = pop(sstack1);

	      if (level == (over_level+1))
		{
		  STATE_userflag2(tstate) = SQ_DONE; 
		}
	      else
		{
		  STATE_userflag2(tstate) = SQ_IN_PROCESS;
		  push (tstate, sstack2);
		}
	    }
  
	  /* prepare next round */
	  while (STACK_depth(sstack2) > 0)
	    {
	      tstate = pop(sstack2);
	      for (tarc = STATE_arc_set(tstate) ; tarc ; tarc = next_arc(tarc))
		{
		  if ((ARC_userflag2(tarc)==SQ_NOT_VISITED) &&
		      (STATE_userflag2(ARC_destination(tarc))==SQ_NOT_VISITED) &&
		      (ARC_destination(tarc)!=tstate))
		    push (ARC_destination(tarc), sstack1);
		}
	    }
	}
    }

  /* local sequentialization of states via left-to-right b-f traversal: if there is
     a from_level parameter given then don't locally sequentialize states on a
     level smaller than from_level.
     */
  
  re_mark_userflag2s_states_and_arcs(fnet, SQ_IN_PROCESS, SQ_NOT_VISITED);
  level=sc=0;
  tstate = NET_start_state(fnet);
  push(tstate, sstack1);
  
  while (STACK_depth(sstack1) > 0)
    {
      level++;
      while (STACK_depth(sstack1) > 0)
	{ 
	  sc++;
	  tstate = pop(sstack1);

	  if (level < from_level)
	    {
	      STATE_userflag2(tstate) = SQ_DONE;
	    }
	  else 
	    {
	      if ( STATE_userflag2(tstate) == SQ_NOT_VISITED)
		{
		  STATE_userflag2(tstate) = SQ_DONE;

		  if (local_sequentialize_state(fnet, tstate, input_side, sequentialize_other_side_too, 
						mark_sequentiable_subnet, fst_cntxt,
						&success_sep_count, &success_inp_sequ_count, &success_outp_sequ_count))
		    {
		      success++;
		      sq_standard_prune_fst(fnet, tstate);
		    }
		}
	    }

	  push (tstate, sstack2);
	}
  
      /* prepare next round */
      while (STACK_depth(sstack2) > 0)
	{
	  tstate = pop(sstack2);
	  for (tarc = STATE_arc_set(tstate) ; tarc ; tarc = next_arc(tarc))
	    {
	      if ((ARC_userflag2(tarc)==SQ_NOT_VISITED) &&
		  (STATE_userflag2(ARC_destination(tarc))==SQ_NOT_VISITED) &&
		  (ARC_destination(tarc)!=tstate))
		push (ARC_destination(tarc), sstack1);
	    }
	}
    }

  if (success)
    {
      /*** if verbose: statistics; ***/

      result = fnet;
      sq_standard_minimize_fst(result, input_side);
    }

   *fst_IY_VERBOSE() = save_sq_verbose;

#ifdef DEBUG
  if (save_sq_verbose &&
      (result || success_sep_count || success_inp_sequ_count || success_outp_sequ_count))
    fsm_fprintf (stderr,"** local_from_to_sequentialize_net: level reached: %d\n\
\tSeparated subnets: %d;\n\
\t%d this (%s) side subnets sequentialized,\n\
\t%d other (%s) side subnets sequentialized.\n", level, success_sep_count, 
		     success_inp_sequ_count,  (( input_side) ? "LOWER": "UPPER"),
		     success_outp_sequ_count, ((!input_side) ? "LOWER": "UPPER"));
      
#endif /* DEBUG */

  free_stack(sstack1);
  free_stack(sstack2);
  reclaim_loc_seq_memory();
  return result;
}
