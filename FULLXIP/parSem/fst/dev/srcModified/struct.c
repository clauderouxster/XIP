/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#if defined _WINDOWS
#include <windows.h>
#elif !defined UNDER_CE
#include <sys/mman.h>
#endif

#include "fst/struct.h"
#include "fst/pretty-p.h"
#include "fst/pars-tbl.h"
#include "fst/header.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
#include "fst/context.h"
#include "fst/types.h"
#endif /* SLIM_FST */

#include "fst/fsm_io.h"
#include "fst/eqv-labs.h"
#include "fst/utf8_fatstr_util.h"


/******************************************************
 **
 **  struct.c
 **
 **      Todd Yampol, Xerox PARC, January 1990
 **      Lauri Karttunen, Xerox PARC, March 1991
 **  Basic functions for manipulating FSM data structures.
 **
 ******************************************************/

/*******
 *
 * GLOBAL VARIABLES
 *
 ********/


/* Global array of pointers to all the net loading functions in use;
   call function init_restore_functions() in your main program for filling this
   table with the appropriate functions. */
restore_func restore_functions[LastRestoreFunctionType];

/* int (*restore_functions[LastRestoreFunctionType])(byte*, long int, long int, NETptr, FILE *, unsigned long *, int); */

/* Global array of pointers to all the net saving functions in use;
   call function init_save_functions() in your main program for filling 
   this table with the appropriate functions. */
save_func save_functions[LastSaveFunctionType];


time_t OLDEST_WRITE_DATE;

HEAPptr NET_HEAP; /* global heap of network structures */
HEAPptr STATE_HEAP, ARC_HEAP, BIG_ARC_HEAP, ARC_VECTOR_HEAP;

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
#ifdef MAC_OS
long BASE_FREE_MEMORY;
#endif
#endif /* SLIM_FST */

/* Heaps for alphabets - %%%TSY 6/92
   Revised %%%TSY 12/92
   
   LV_HEAPS is an array of heaps.  Each element points
   to a heap for storing label (id_type) vectors for use in
   the ALPHABET data structure.  The element LV_HEAPS[n]
   points to the heap for label vectors of size 2^n.  
   Eg. LV_HEAPS[4] points to the heap for vectors of 
   length 2^4 = 16 labels.
   
   For ALPHABETS of length 4 or shorter, we use a vector of
   length 4.  Thus, LV_HEAP[0] and LV_HEAP[1] are not used
   in the usual way.  By convention, LV_HEAPS[0] points
   to the heap of ALPHABET structures.  LV_HEAPS[1] is reserved
   for future use.
   */

/* #define ALPH_HEAP LV_HEAPS[0]  heap for alphabet cells */
/* #define MAX_LV 16 -- defined in struct.h -- max of 2^16 labels */
/* #define MAX_LABEL 16383    -- defined in struct.h -- max label has id 2^15 - 1 */
HEAPptr LV_HEAPS[MAX_LV+1] =            /* heaps for label vectors of len 2^0 ... 2^15 */
{NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL};           

HEAPptr CH_VECTOR_HEAP = NULL;
int CH_VECTOR_BLOCK_SIZE = 5;

HEAPptr CH_NODE_HEAP = NULL;
int CH_NODE_BLOCK_SIZE = 127;

HEAPptr PARSE_TBL_HEAP = NULL;
int PARSE_TBL_BLOCK_SIZE = 10;

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
#endif /* SLIM_FST */

int FSM_MODE = STANDARD_MODE;

int NOT_INITIALIZED_P = TRUE;


/* STANDARD_MODE is defined in TYPES.H.  The other alternative is TWOL_MODE.
   In TWOL_MODE, fstpairs are treated as if they were atomic symbols and
   SIGMA is composed of labels rather than atomic symbols.
   */


FAT_STR AUTHOR_ATTR = NULL,
        CHARENCODING_ATTR = NULL,
        CHARNAMES_ATTR = NULL,
        COMPACTED_ATTR = NULL,
        COMPOUNDCHAR_ATTR = NULL,
        COPYRIGHT_ATTR = NULL,
        DATE_ATTR = NULL,
	DIACRITICS_ATTR = NULL,
        EQV_VECTOR_ATTR = NULL,
        FUNCTION_ATTR = NULL,
        LANGUAGE_ATTR = NULL,
        LITERALNAMES_ATTR = NULL,
        NETWORKNAME_ATTR = NULL,
        NEUTRALLINKINGCHAR_ATTR = NULL,
        OPERATION_ATTR = NULL,
        POSTFIXLINKINGCHAR_ATTR = NULL,
        PREFIXLINKINGCHAR_ATTR = NULL,
        SAVED_SIGMA_ATTR = NULL,
        SIGMA_ATTR = NULL,
        SOURCE_ATTR = NULL,
        STATUS_ATTR = NULL,
        TAGS_ATTR = NULL,
        TOOLVERSIONS_ATTR = NULL,
        TRANSCRIPT_ATTR = NULL,
        TYPE_ATTR = NULL,
        VECTORIZE_N_ATTR = NULL,
        VERSIONSTRING_ATTR = NULL,
        WORD_TO_FILEPOS_MAP_ATTR = NULL;   /* 28  pointers to ..._ATTR */

FAT_STR EPSILON_NAME,
        OTHER_NAME;                  /* 2 pointers to ..._NAME */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
extern FAT_STR KEYWORD_PACKAGE;     
#endif /* SLIM_FST */

FAT_STR IL_PACKAGE = NULL,
        LISP_PACKAGE = NULL;        /* 3 pointers to ..._PACKAGE */ 

FAT_STR FAT_OTHER = NULL,
        NIL_NAME = NULL,
        T_NAME = NULL;              /* 3 pointers to special names. */


/* Global attribute and package name constants. These strings should
   never get reclaimed by free(). The function common_fat_string_p(FAT_STR name)
   defined below returns TRUE for these names and the name
   KEYWORD_PACKAGE declared in "label.c." */ 

static HASH_TABLEptr COMMON_FAT_STRINGS = NULL;


void register_common_fs(FAT_STR fs)
{
  put_hash(COMMON_FAT_STRINGS, (void *) fs, (void *) fs, hash_fatstr,
	   fat_strcmp);
}		  


static FAT_STR make_common_fs(char *str)
{
  FAT_STR fs = FST_thin_to_fat(str);

  register_common_fs(fs);

  return(fs);
}


static void init_props (void)
{
  if (NOT_INITIALIZED_P)
    {
      COMMON_FAT_STRINGS = make_hash_table(101, "Names");

      AUTHOR_ATTR = make_common_fs("AUTHORS");
      CHARENCODING_ATTR = make_common_fs("CHARENCODING");
      CHARNAMES_ATTR = make_common_fs("CHARNAMES");
      COMPACTED_ATTR = make_common_fs("COMPACTED-P");
      COMPOUNDCHAR_ATTR = make_common_fs("COMPOUNDHAR");
      COPYRIGHT_ATTR = make_common_fs("COPYRIGHT");
      DATE_ATTR = make_common_fs("DATE");
      DIACRITICS_ATTR = make_common_fs("DIACRITICS");
      EQV_VECTOR_ATTR = make_common_fs("EQV-VECTOR");
      FUNCTION_ATTR = make_common_fs("FUNCTION");
      LANGUAGE_ATTR = make_common_fs("LANGUAGE");
      LITERALNAMES_ATTR = make_common_fs("LITERALNAMES");
      NETWORKNAME_ATTR = make_common_fs("NETWORKNAME");
      NEUTRALLINKINGCHAR_ATTR = make_common_fs("NEUTRALLINKINGCHAR");
      OPERATION_ATTR = make_common_fs("OPERATION");
      POSTFIXLINKINGCHAR_ATTR = make_common_fs("POSTFIXLINKINGCHAR");
      PREFIXLINKINGCHAR_ATTR = make_common_fs("PREFIXLINKINGCHAR");
      SAVED_SIGMA_ATTR = make_common_fs("SAVED-SIGMA");
      SIGMA_ATTR = make_common_fs("SIGMA");
      SOURCE_ATTR = make_common_fs("SOURCE");
      STATUS_ATTR = make_common_fs("STATUS");
      TAGS_ATTR = make_common_fs("TAGS");
      TOOLVERSIONS_ATTR = make_common_fs("TOOLVERSIONS");
      TRANSCRIPT_ATTR = make_common_fs("TRANSCRIPT");
      TYPE_ATTR = make_common_fs("TYPE");
      VECTORIZE_N_ATTR = make_common_fs("VECTORIZE-N");
      VERSIONSTRING_ATTR = make_common_fs("VERSIONSTRING");
      WORD_TO_FILEPOS_MAP_ATTR = make_common_fs("WORD-TO-FILEPOS-MAP");

      EPSILON_NAME = make_common_fs("0");
      OTHER_NAME = make_common_fs("?");

      NIL_NAME = make_common_fs("NIL");
      T_NAME = make_common_fs("T");

      FAT_OTHER = make_common_fs("OTHER");
      IL_PACKAGE = make_common_fs("INTERLISP");
      LISP_PACKAGE = make_common_fs("LISP");
      register_common_fs(KEYWORD_PACKAGE);
    }
} /* init_props */

static void initialize_dates(void)
{   
  /* Called once once only from init_fsm_memory() */
  {
    struct tm first_date = {0,0,0,1,2,0,0,0,0};
      
    first_date.tm_sec =   0;
    first_date.tm_min =   0;
    first_date.tm_hour =  0;
#ifdef WORLD
    /* Don't authorize xfst to read networks created before June-2000 */
    first_date.tm_mday =  1;
    first_date.tm_mon =   5;
    first_date.tm_year = 100;  
#else
    first_date.tm_mday =  1;
    first_date.tm_mon =   2;
    first_date.tm_year = 90;  
#endif

    OLDEST_WRITE_DATE = mktime(&first_date);

  }
}


static int init_ch_node_memory(void)
{
  if (CH_NODE_HEAP != NULL)
    return(1);
  
  CH_VECTOR_HEAP = init_heap((size_t) 256 *  (size_t) sizeof(CH_NODEptr),
			     CH_VECTOR_BLOCK_SIZE, "Parse table vectors");
  
  CH_NODE_HEAP = init_heap(sizeof(CH_NODEtype), CH_NODE_BLOCK_SIZE,
			   "Parse table nodes");
  
  if (CH_VECTOR_HEAP && CH_NODE_HEAP)
    return(0);
  else
    return(1);
}


static int init_parse_table_memory(void)
{
  if (init_ch_node_memory())
    return(1);
  
  if (PARSE_TBL_HEAP != NULL)
    return(1);

  PARSE_TBL_HEAP = init_heap(sizeof(PARSE_TBLtype), PARSE_TBL_BLOCK_SIZE,
			   "Parse table");
  
  if (PARSE_TBL_HEAP)
    return(0);
  else
    return(1);
}


int SQ_FINAL_STRINGS_ARCS, SQ_INTERN_STRINGS_ARCS, SQ_STRING_ONELONG; 
int IY_UNICODE;

static void initialize_global_variables(FST_CNTXTptr fst_cntxt)
{
    /* if the interface command interpreter is able to read and
    set bit variables then SQ_FINAL_STRINGS_ARCS and
    SQ_INTERN_STRINGS_ARCS  will become obsolete. */
  IY_UNICODE = TRUE;
  SQ_FINAL_STRINGS_ARCS = (int) FST_sq_final_strings_arcs(fst_cntxt);
  SQ_INTERN_STRINGS_ARCS = (int) FST_sq_intern_strings_arcs(fst_cntxt);
  SQ_STRING_ONELONG = (int) FST_sq_string_onelong(fst_cntxt);
}


void init_fsm_memory(FST_CNTXTptr fst_cntxt)
     /********
      *
      *  Allocates memory for FSM arcs & states.
      *  IMPORTANT NOTE: THIS FUNCTION MUST BE CALLED FIRST IN 
      *            >>> ANY <<< FSM APPLICATION!!!
      *
      ********/
{
  if (NOT_INITIALIZED_P)
    {
      initialize_global_variables(fst_cntxt);
#ifdef MAC_OS
	/* initializes so that memory measurement is correct */
  /*    BASE_FREE_MEMORY = FreeMem();
  		Obsolete now.  LK 98-09-25
  */
#endif

      /* Check this once! */

      assert(DO_RECLAIM == DONT_COPY && DONT_RECLAIM == DO_COPY);

      /* Allocate NETtype & ALPHtype in 16k blocks */
      NET_HEAP = init_heap(sizeof(NETtype), 16384/sizeof(NETtype),
			   "Networks");
      ALPH_HEAP = init_heap(sizeof(ALPHABETtype), 16384/sizeof(ALPHABETtype),
			    "Alphabets");
  
      /* Allocte heaps in 32k blocks */
      STATE_HEAP = init_heap(sizeof(STATEtype), 32768/sizeof(STATEtype),
			     "States");
      ARC_HEAP   = init_heap(sizeof(ARCtype),   32768/sizeof(ARCtype),
			     "Arcs");
  
      BIG_ARC_HEAP = NULL;  /* Initialized in compact.c, if needed. */

      init_label_table();
      init_props();
      initialize_dates();
      init_parse_table_memory();
      
      NOT_INITIALIZED_P = FALSE;
    }
} /* init_fsm_memory */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
void print_storage_info(FILE *stream)
{
/* Obsolete now. LK 98-09-25
#ifdef MAC_OS
  long cur_free_mem, lost_mem;
#endif
*/
  long total_bytes;
  
  total_bytes = tally_stacks(stream);
  fputc('\n', stdout);
  total_bytes += tally_heaps(stream);

/* Obsolete now. LK 98-09-25
#ifdef MAC_OS
  cur_free_mem = FreeMem();
  lost_mem = BASE_FREE_MEMORY - cur_free_mem - total_bytes;
  fsm_fprintf(stream, "Global+Unaccounted:  %ldk\n", lost_mem/1024);
#endif
*/
}

void print_net_name(NETptr net, FILE *stream)
{
  OBJECTptr name = get_value(net, NETWORKNAME_ATTR);

  if (name != NULL && OBJ_type(name) == String)
    FST_fprint_fat_string(stream, OBJ_str(name));
  else
#ifdef MAC_OS
    fsm_fprintf(stream, "%lX", (size_t) &*net);
#else
    fsm_fprintf(stream, "%X", (size_t) &*net);
#endif
}
#endif /* SLIM_FST */



NETptr make_network (void)
     /********
      *
      * Allocates a network, and initializes important variables.
      * Use only this function to create new networks.  It is 
      * an error to use malloc() to create a network.
      *
      ********/
{
  
  NETptr new_net = (NETptr) cell_alloc(NET_HEAP);
  
  if (new_net == NULL)
    not_enough_memory("MAKE_NETWORK");
  
  NET_deterministic(new_net)            = FALSE;
  NET_pruned(new_net)                   = FALSE;
  NET_completed(new_net)                = FALSE;
  NET_minimized(new_net)                = FALSE;
  NET_epsilon_free(new_net)             = FALSE;
  NET_sorted_states(new_net)            = FALSE;
  NET_loop_free(new_net)                = FALSE;
  NET_twol_net(new_net)                 = FSM_MODE; /* STANDARD_MODE = 0 */
  NET_visit_marks_dirty(new_net)        = FALSE; 
  NET_names_matter(new_net)             = FALSE; 
  NET_shared_arc_lists(new_net)         = FALSE;
  NET_has_arc_user_pointer(new_net)     = FALSE;
  NET_closed_sigma(new_net)             = FALSE;
  NET_start_state_final(new_net)        = FALSE;
  NET_tabular(new_net)                  = FALSE; 
  NET_compacted(new_net)                = FALSE;
  NET_upper_seq(new_net)                = FALSE;
  NET_lower_seq(new_net)                = FALSE;
  NET_mark(new_net)                     = FALSE;
  NET_u_flag_diacr(new_net)             = FALSE;
  NET_l_flag_diacr(new_net)             = FALSE;

  NET_left_bimachine(new_net)  		= FALSE;
  NET_right_bimachine(new_net)		= FALSE;
  NET_reduced_labelset(new_net)		= FALSE;
  NET_is_virtual(new_net)               = FALSE;
  NET_optimized(new_net)                = FALSE;
  
  NET_arc_label_arity (new_net)         = 1;
  NET_labels(new_net)                   = NULL;
  
  NET_sigma(new_net)                    = NULL;
  NET_recode_key(new_net)               = NULL;
  NET_unreduce_key(new_net)             = NULL;
  
  NET_start_state(new_net)              = NULL;
  NET_states(new_net)                   = NULL;
  NET_arc_vector_heap(new_net)		= NULL;
  NET_arc_vector_len(new_net)		= 0;
  NET_range_len(new_net)                = 0;
  NET_properties(new_net)               = NULL;
  
  NET_upper_parse_table(new_net)        = NULL;
  NET_lower_parse_table(new_net)        = NULL;
  NET_uprange_map(new_net)              = NULL;
  NET_downrange_map(new_net)            = NULL;
  NET_upmatch_table(new_net)            = NULL;
  NET_downmatch_table(new_net)          = NULL;

  NET_num_states(new_net)               = 0;
  NET_num_arcs(new_net)                 = 0;
  NET_block_size(new_net)               = 0;
  
  NET_client_cell(new_net)              = NULL;
  NET_flag_register(new_net)            = NULL;

  return (new_net);
} /* make_network */


NETptr null_fsm(void)
     /*****
      *
      * Returns an FSM that accepts no strings at all,
      * not even the empty string.
      *
      *****/
{
  NETptr  net = make_network();
  STATEptr start_state = make_state(NULL, net);
  NET_start_state(net) = start_state;
  
  NET_deterministic(net)        = TRUE;
  NET_pruned(net)               = TRUE;
  NET_minimized(net)            = TRUE;
  NET_epsilon_free(net)         = TRUE;
  NET_loop_free(net)            = TRUE;
  NET_labels(net) = make_alph(0, LABEL_VECTOR);
  NET_sigma(net) = make_alph(0, BINARY_VECTOR);
  return (net);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
NETptr epsilon_fsm (void)
{
  /* EPSILON_FSM returns an FSM that accepts just the empty string */
  
  NETptr net = null_fsm();
  
  STATE_final(NET_start_state(net)) = TRUE;
  
  return(net);
}

id_type get_field (LABELptr label, int field)
     /*****
      *
      * Returns the appropriate FIELD of LABEL. 
      * If label is a tuple (arity >= 2), field must
      * be <= the arity of the label.  If the label
      * is atomic (arity == 1), simply returns the
      * id of the label.  Note that UPPER = 0, LOWER = 1;
      *
      *****/
{
  switch (LABEL_arity(label))
    {
    case 1:			/* identity map, eg. a:a */
      return (LABEL_id(label));
      break;
    default:			/* tuple */
      return (TUPLE_labels(LABEL_tuple(label))[field]);
      break;
    }
}
#endif /* SLIM_FST */

ARCptr arc_alloc(id_type label, STATEptr dest)
{
  /* A primitive function to create an arc. Be careful.
     You should not use this function to create arcs that are
     incorporated in a network. Use make_arc() instead. 
     */
     
   ARCptr new_arc = (ARCptr) cell_alloc(ARC_HEAP);
   
   if (new_arc == NULL)
      not_enough_memory("ARC_ALLOC");
      
#ifndef SMALL_FSM
  ARC_type_bit(new_arc) = 0;	/* Arc = 0, STATE = 1, for ALTCHAINS */
  ARC_userflag1(new_arc) = 0;
  ARC_visit_mark(new_arc) = NOT_VISITED;
  ARC_big_arc_flag(new_arc) = 0;
#ifdef FULL_FSM
  ARC_user_pointer(new_arc) = NULL;
#endif
#endif /* SMALL_FSM */

  ARC_label(new_arc)            = label;
  ARC_destination(new_arc)      = dest;
  next_arc(new_arc)             = NULL;
  return(new_arc);
}

ARCptr  make_arc (id_type label, STATEptr dest, NETptr net)
     /*******
      *
      * Creates a new arc in NET.  Call this function to create arcs
	  * that are incorporated in a network.
      *
      *******/
{
  ARCptr new_arc = arc_alloc(label, dest);
  
  NET_num_arcs(net)++;

  return (new_arc);
} /* make_arc */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
void push_arc(ARCptr arc, STATEptr state)
{
  /* Quick way to add arcs.  Use carefully because this function does not
     care about determinism, loop_freeness, epsilon labels, etc.  When in
     doubt, use ADD_ARC or ADD_NEW_ARC instead.
     */
  next_arc(arc) = STATE_arc_set(state);
  STATE_arc_set(state) = arc;
}
#endif /* SLIM_FST */

void add_arc (ARCptr arc, STATEptr state, NETptr net)
     /****************
      *
      * Adds arc to state, checks whether state is still
      * deterministic with new arc.
      *
      *****************/
{       
  ARCptr cur, prev;
  id_type label_id = ARC_label(arc) ;
  LABELptr label = id_to_label(label_id);
  

  if (label_id == EPSILON)
    {
      if (ARC_destination(arc) == state)
	/* Don't add looping EPSILON arcs */
	{ 
	  NET_num_arcs(net)--;
	  free_arc(arc);
	  return;
	}
      else
	{
	  STATE_deterministic(state) = FALSE;
	  NET_deterministic(net) = FALSE;
	  NET_epsilon_free(net) = FALSE;
	  NET_minimized(net) = FALSE;
	}
    }
  
  if ((int16)LABEL_arity(label) > NET_arc_label_arity(net))
    NET_arc_label_arity(net) = LABEL_arity(label);
  
  if (state == ARC_destination(arc))
    NET_loop_free(net) = FALSE;
  
  if (STATE_arc_set(state) == NULL)
    STATE_arc_set(state) = arc;
  else
    {
      switch (STATE_deterministic(state))
        {
        case (TRUE):
          for (prev = NULL, cur = STATE_arc_set(state); 
               cur != NULL; 
               prev = cur, cur = next_arc(cur))
            {
              if (label_id == ARC_label(cur))
                {
                  STATE_deterministic(state) = FALSE;
                  NET_deterministic(net) = FALSE;
                  NET_minimized(net) = FALSE;
                }
            }
          next_arc(prev) = arc;
          break;
        case (FALSE):
          for (cur = STATE_arc_set(state);
               next_arc(cur) != NULL;
               cur = next_arc(cur));
          next_arc(cur) = arc;
          break;
        }
    }
} /* add_arc */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#ifdef FULL_FSM
void add_new_arc (NETptr net, STATEptr start, id_type l, STATEptr dest,
                  void *user_pointer)
#else
     void add_new_arc (NETptr net, STATEptr start, id_type l, STATEptr dest)
#endif
     /****************
      *
      * If no identical arc exists, 
      * creates an arc from START to DEST with label L.
      *
      *****************/
{       
  ARCptr arc, cur;
  

  /* don't add an epsilon arc that loops back to the start */
  if (l == EPSILON)
    {
      if (start == dest)
	return;
      else
	{
	  STATE_deterministic(start) = FALSE;
	  NET_deterministic(net) = FALSE;
	  NET_epsilon_free(net) = FALSE;
	  NET_minimized(net) = FALSE;
	}
    }
  else
    {
      LABELptr label = id_to_label(l);
      
      if ((int16)LABEL_arity(label) > NET_arc_label_arity(net))
        NET_arc_label_arity(net) = LABEL_arity(label);
    }
  
  if (start == dest)
    NET_loop_free(net) = FALSE;
  
  for (cur = STATE_arc_set(start); cur != NULL; cur = next_arc(cur))
    {
      if (ARC_label(cur) == l)
	{
	  if (ARC_destination(cur) == dest)
	    /* found an identical arc, so quit */
	    return;                                       
	  else
	    /* record non-determinism */
	    {
	      STATE_deterministic(start) = FALSE;
	      NET_deterministic(net) = FALSE;
	      NET_minimized(net) = FALSE;
	    }
	}
    }
  
  arc = make_arc (l, dest, net);
#ifdef FULL_FSM
  ARC_user_pointer(arc) = user_pointer;
#endif
  next_arc(arc) = STATE_arc_set(start);
  STATE_arc_set(start) = arc;

}


int num_arcs(STATEptr state)
     /*********
      *
      * Returns the number of arcs originating at STATE.
      *
      *********/
{
  int i;
  ARCptr cur_arc;
  
  for (i = 0, cur_arc = STATE_arc_set(state);
       cur_arc != NULL; i++, cur_arc = next_arc(cur_arc));
  
  return i;
}
#endif /* SLIM_FST */


STATEptr alloc_state(void)
{
  /* A primitive function for creating a state. Gets a state from the
     freelist if possible. Be careful using alloc_state() because
     this routine does not incorporate the new state in any network
     structure. For most purposes, you should use make_state() instead.
     */
     
  STATEptr new_state = (STATEptr) cell_alloc(STATE_HEAP);
  
  if (new_state == NULL)
    not_enough_memory("ALLOC_STATE");
  
  STATE_type_bit(new_state) = 1; /* Arcs = 0, States = 1, for ALTCHAINS */
  STATE_visit_mark (new_state) = NOT_VISITED;
  STATE_final (new_state) = FALSE;
  STATE_vector_p(new_state) = FALSE;
  STATE_deterministic (new_state) = FALSE;
  STATE_is_virtual(new_state) = FALSE;
  STATE_client_cell(new_state) = NULL;
  STATE_arc_set(new_state) = NULL;
  
  return(new_state);
}

STATEptr make_state (ARCptr arcs, NETptr net)
     /******
      *
      * Creates a new state, adds it to the state list of NET.  Use only this
      * function to create states that are incorporated in a network.
      *
      ******/
{
  STATEptr new_state = alloc_state();
  
  STATE_type_bit(new_state) = 1; /* Arcs = 0, States = 1, for ALTCHAINS */
  
  /* Initially every state with less than two arcs gets marked as deterministic. */

  if (!arcs || !next_arc(arcs))
    STATE_deterministic (new_state) = TRUE;

  /* N.B. It is assumed here that the arcs are already in the arc count of
     the net. This is correct if the arcs were created by make_arc() as is
     normally the case.
     */
  
  STATE_arc_set (new_state) = arcs;
  
  /* add state to state list of NET */
  
  next_state(new_state) = NET_states(net);
  NET_states(net) = new_state;
  NET_num_states(net)++;
  
  return (new_state);
} /* make_state */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/*  delete_state_from_list() and delete_states() are displaced from 
    modify.h  TG */

STATEptr delete_state_from_list (STATEptr s, STATEptr states)
{
  STATEptr cur, prev;
  
  for (prev = NULL, cur = states; 
       cur != NULL; 
       prev = cur, cur = next_state(cur))
    if (cur == s)       /* found a match */
      {
        if (prev == NULL)       /* s is first in states */
          states = next_state(cur);
        else
          next_state(prev) = next_state(cur);
        
        free_state(s);
        return(states);
      }
  
  /* s not in states */ 
  return (states);
}


STATEptr delete_states(NETptr net, int keep_pred(STATEptr state))
/*****
 *
 * DELETESTATES evaluates KEEP_PRED for each state in 
 * NET, and splices states out unless form is true.
 * Appends new_states to end of list after deletion.
 *
 * Returns the new (shortened) list.
 *****/
{
  STATEptr states = NET_states(net), state, next_state, prev;
  
  for (state = states, prev = NULL; state != NULL; state = next_state)
    {
      next_state = next_state(state);
      
      if (!keep_pred((void *) state))
        {
          free_state_and_arcs(state, net);
          
          if (prev == NULL)
            NET_states(net) = next_state;
          else
            next_state(prev) = next_state;
        }
      else
        prev = state;
    }
  
  return (NET_states(net));
}

/********************************************
 *                                          *
 *   renumbering and restoring arc labels   *
 *                                          *
 ********************************************/

ALPHABETptr reverse_map(ALPHABETptr map)
/***************************************
 *
 * NB: be aware that this function may introduce 
 * ID_NO_SYMBOL in the resulting alphabet...

 * The returned alphabet REV is in the LABEL_VECTOR  
 * format:
 * ALPH_item(rev, i) contains the index of the
 * label i in the items array of the MAP alphabet.
 * If ALPH_item(rev, i) is ID_NO_SYMBOL, it means
 * that the label i was not in the original MAP.
 *                                          
 **************************************/
{
  id_type max = 0;
  int i;
  ALPHABETptr rev;
  
  /* MAP assumed to be in LABEL_VECTOR format. */
  
  map = binary_to_label(map);
  
  /* find the largest element in map */
  for (i = 0; i < ALPH_len(map); i++)
    if (ALPH_item(map,i) > max && ALPH_item(map,i) != ID_NO_SYMBOL)
      max = ALPH_item(map,i);
  
  /* need 1 extra for item 0 */
  max++;
  
  rev = make_alph(max, LABEL_VECTOR);
  
  /* initialize reverse map */
  for (i = 0; i < max; i++)
    ALPH_item(rev,i) = ID_NO_SYMBOL;
  
  /* create reverse map.  It may be a partial map */
  for (i = 0; i < ALPH_len(map); i++)
    if (ALPH_item(map,i) != ID_NO_SYMBOL)
      ALPH_item(rev,ALPH_item(map,i)) = i;
  
  return (rev); 
}

void renumber_arc_labels(NETptr net, ALPHABETptr key)
     /**********
      *
      * Call this directly with a key (an ALPHABET mapping the current labels
      * to a new set of labels), or use the front end functions
      * renumber_net() and renumber_nets().
      *
      ***********/
{
  STATEptr cur_state;
  ARCptr cur_arc;
  
  binary_to_label(key);
  for (cur_state = NET_states(net); 
       cur_state != NULL;
       cur_state = next_state(cur_state))
    for (cur_arc = STATE_arc_set(cur_state);
         cur_arc != NULL; 
         cur_arc = next_arc(cur_arc))
      if (ARC_label(cur_arc) != ID_NO_SYMBOL && ARC_label(cur_arc) < ALPH_len(key))
        {
          if (ALPH_item(key, ARC_label(cur_arc)) != ID_NO_SYMBOL &&
              ALPH_item(key, ARC_label(cur_arc)) < LIM_id_count(LABEL_MAP))
            {
              ARC_label(cur_arc) = ALPH_item(key,ARC_label(cur_arc));
            }
          else
            {
	      fsm_fprintf(stderr, "*** Problem with arc label '");
	      print_label(ARC_label(cur_arc), stderr, DO_ESCAPE);
	      fsm_fprintf(stderr, "'\n");
              handle_error("key item out of range", "renumber_arc_labels",
                           ALPH_item(key, ARC_label(cur_arc)));
            }
        }
      else
        {
          handle_error("arc label out of range", "renumber_arc_labels",
                       ARC_label(cur_arc));
        }
}

#endif /* SLIM_FST */

/**************
 *            *
 *  Alphabet  *
 *            *
 **************/

static int len_to_max(int len)
{
  int max;
  
  for (max = 4; 
       max < len; 
       max <<= 1);
  
  return(max);
}

#if NOT_USED
static int len_to_log2_of_max(int len)
{
  int max, log;
  
  for (log = 2, max = 4; 
       max < len; 
       max <<= 1, log++);
  
  return(max);
}
#endif /* NOT_USED */

static int log_base_2(int n)
{
  int log, max;
  
  for (log = 0, max = 1; 
       max < n; 
       max <<= 1, log++);
  
  return(log);
}

id_type *make_alph_items(int len, int *max)
     /******
      *
      * Allocate space for alphabet items (a vector of int),
      * from the appropriate "heap".  (There are heaps defined for
      * alphabets of length 4, 8, 16, 32, 64, 128, ..., 16k elements.)
      *
      * LEN is the length of the vector desired.
      * MAX is the address of an integer that will store
      * the actual length of the vector returned, generally
      * the smallest power of two that is greater than LEN. 
      *
      * Returns a pointer to the new vector, and stores
      * the length of this vector in *MAX.
      *
      * The size of the vector returned is the least power
      * of 2 greater than LEN.  For instance, if LEN is
      * 5 then the size of the vector returned is 8.  If LEN
      * is 19, then the size is 32.
      *
      * If LEN is 0, then NULL is returned.  Otherwise,
      * the minimum size of the returned vector is 4.
      * The maximum size is 1024.  If an alphabet with 
      * more than 1024 elements is ever needed, then
      * code for additional heap(s) will have to be written.
      * 
      *****/
{
  id_type *items;
  uint16 power;

  unsigned int m;
  
  if (len <= 0)
    {
      *max = 0;
      return(NULL);
    }
  
  if (len > (MAX_LABEL + 1) )
    { /* The test is against MAX_LABEL+1 because MAX_LABEL is equal to
	 65534 instead of 65535, (which is a reserved value for special 
	 symbols). */
          
      handle_error("ALPHABET length exceeds MAX_LABEL", "MAKE_ALPH_ITEMS", len);
      return(NULL);
    }
  
  /* We know 1 <= LEN <= MAX_LABEL at this point. */
  /*  Compute m (max), the smallest power of 2 that is >= max(LEN, 4) */
  m = len_to_max(len);
  power = log_base_2(m);
  
  if (power > MAX_LV)
    handle_error ("log(max) exceeds 'MAX_LV'", "MAKE_ALPH_ITEMS", len);
  
  if (LV_HEAPS[power] == NULL)	/* initialize the heap if necessary */
    {
      int cell_size = sizeof(id_type) * m;
      LV_HEAPS[power] = init_heap(cell_size, 8, "Label vectors");
    }
  
  *max = m;
  
  items = (id_type *) cell_alloc(LV_HEAPS[power]);
  
  /* clear new items */
  memset(items, 0, (size_t) sizeof(id_type) * m);
  
  return (items);
}

ALPHABETptr make_alph(int len, int type)
     /*****
      *
      * Returns a new alphabet of length LEN and type TYPE
      * (BINARY_VECTOR or LABEL_VECTOR).
      * Caller is responsible to fill alphabet with appropriate value.
      *
      * TSY 6/92:  ALPHABET and ALPH_items now allocated from 
      *            ALPH_HEAP and LV_HEAP_# in order to make 
      *            use of heap free lists.
      *
      *****/
{
  ALPHABETptr a;
  
  if (!(type == BINARY_VECTOR || type == LABEL_VECTOR))
    handle_error("Bad type", "MAKE_ALPH", type);
  if (len > MAX_LABEL)
    handle_error("Cannot allocate alphabet larger than MAX_LABEL",
                 "MAKE_ALPH", (unsigned int) len);
  
  a = (ALPHABETptr) cell_alloc(ALPH_HEAP);
  
  if (a == NULL)
    not_enough_memory("MAKE_ALPH");
  ALPH_type(a) = type;
  ALPH_len(a) = len;
  ALPH_items(a) = make_alph_items(len, &ALPH_max(a));  

  return(a);
}/* make_alph */


ALPHABETptr copy_alphabet(ALPHABETptr old_alph)
     /******
      *
      * Returns a copy of the alphabet old_alph.
      *
      ******/
{
  if (old_alph == NULL)
    return(NULL);
  else
    {
      ALPHABETptr new_alph =
        make_alph(ALPH_len(old_alph), ALPH_type(old_alph));
      
      if (ALPH_len(old_alph) > 0)
        memcpy((void *) ALPH_items(new_alph),
               (void *) ALPH_items(old_alph),
               (size_t) ALPH_len(old_alph) * (size_t) sizeof(id_type));
      
      return(new_alph);
    }
} /* copy_alphabet */


void add_binary_alph(id_type *target, id_type *source, int len)
{
  /* Adds every label in the binary alphabet SOURCE to the
     binary alphabet TARGET.  Doesn't check for any bounds.
     */
  
  int i;
  
  for (i = 0; i < len; i++, target++)
    if (*source++)
      *target = 1;
}

void add_label_alph(id_type *target, id_type *source, int len)
{
  /* Adds every label in the label alphabet SOURCE to the
     binary alphabet TARGET Doesn't check for any bounds.
     */
  
  int i;
  
  for (i = 0; i < len; i++)
    target[*source++] = 1;
} /* add_label_alph */


void subtract_binary_alph(id_type *target, id_type *source, int len)
{
  /* Subtracts every label in the binary alphabet SOURCE from the
     binary alphabet TARGET.  Doesn't check for any bounds.
     */
  
  int i;
  
  for (i = 0; i < len; i++, target++)
    if (*source++)
      *target = 0;
} /* subtract_binary_alph */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void subtract_label_alph(id_type *target, id_type *source, int len)
{
  /* Subtracts every label in the label alphabet SOURCE from the
     binary alphabet TARGET Doesn't check for any bounds.
     */
  
  int i;
  
  for (i = 0; i < len; i++)
    target[*source++] = 0;
}
#endif /* SLIM_FST */


id_type max_binary_item(id_type *items, int len)
{
  /* Returns the highest ID position in the binary vector ITEMS
     that is nonzero.  Returns 0 in two cases: (1) all positions in
     ITEMS are zeros and (2) the EPSILON position in ITEMS is
     non-zero.  Be careful to distinguish these cases in the
     calling function.
     */
  
  int i, max = 0;
  for (i = 0; i < len; i++)
    if (*items++)
      max = i;
  return(max);
}

id_type max_label_item(id_type *items, int len)
{
  /* Returns the highest label in the label vector ITEMS.  Returns 0
     in two cases: (1) LEN is 0 and (2) LEN is 1 and the only
     label is EPSILON.  Be careful to distinguish these cases in the
     calling function.
     */
  
  int i;
  id_type max = 0;

  for (i = 0; i < len; i++, items++)
    if (*items > max)
      max = *items;
  return(max);
}



ALPHABETptr labels_to_alph(id_type *symbols, int len, int type)
{
  /* Returns an alphabet of type LABEL_VECTOR or BINARY_VECTOR
     that contains the symbols in the LABEL_VECTOR. Checks
     that the integers in SYMBOLS are in the proper range.
     */
 
  ALPHABETptr alph;
  id_type max = max_label_item(symbols, len);
  
  if(max >= LIM_id_count(LABEL_MAP))
    handle_error("Bad label id", "LABELS_TO_ALPH", max);
    
  if (type == LABEL_VECTOR)
    {
      alph = make_alph(len, LABEL_VECTOR);
      memcpy((void *) ALPH_items(alph), (void *) symbols,
	     (size_t) (sizeof(id_type) * len));
    }
  else
    {
      alph = make_alph(max + 1, BINARY_VECTOR);
      add_label_alph(ALPH_items(alph), symbols, len);
    }

  return(alph);
} /* labels_to_alph */


int sigma_member(ALPHABETptr a, id_type item)
{
  /* Returns 1 if ITEM is a member of A, 0 otherwise.
   */
  
  if (ALPH_type(a) == BINARY_VECTOR)
    return (binary_sigma_member(a, item));
  else
    {
      id_type *items = ALPH_items(a);
      int  i, len = ALPH_len(a);
      for (i = 0; i < len; i++)
        if (*items++ == item)
          return(TRUE);
      return(FALSE);
    }
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void sigma_augment(ALPHABETptr sigma, ALPHABETptr alph)
{
  /* Augments SIGMA with all the simple symbols and components
     of fst tuples in ALPH.  Use this to augment the NET_sigma
     field.
     */
  
  int16 alph_type = ALPH_type(alph);
  id_type *items;
  int i, len;

  /* Make sure that SIGMA is a binary vector */
  
  label_to_binary(sigma);
  
  len = ALPH_len(alph);
  items = ALPH_items(alph);
  
  for (i = 0; i < len; i++)
    if (alph_type == BINARY_VECTOR)
      {
        if (items[i] == 1)
          label_to_sigma(sigma, (uint16)i);
      }
    else
      label_to_sigma(sigma, items[i]);
}                  

void label_augment(ALPHABETptr labels, ALPHABETptr alph)
{
    /* Augments LABELS with all the labels in ALPH.  Use this to
       modify the NET_labels field.
       */
    
    if (alph && ALPH_len(alph) > 0)
    {
        sigma_union_dont_copy(labels, alph);
        binary_to_label(labels);
    }
} 
#endif /* SLIM_FST */

ALPHABETptr sigma_union(ALPHABETptr s1, ALPHABETptr s2)
     /*****
      *
      * Returns the union of S1 and S2 as a binary vector.
      * Preserves S1 and S2.
      *
      *****/
{
  return(sigma_union_dont_copy(copy_alphabet(s1), s2));
} /* sigma_union */



ALPHABETptr sigma_union_dont_copy(ALPHABETptr s1, ALPHABETptr s2)
     /*****
      *
      * Returns the union of S1 and S2 in binary format.  Alters S1.
      *
      *****/
{
  int len1, len2;
  
  if (!s1)
    return(copy_alphabet(s2));
  if (!s2)
    return (s1);
  
  len1 = ALPH_len(s1), len2 = ALPH_len(s2);
  if (ALPH_type(s1) == BINARY_VECTOR)
    {
      if (ALPH_type(s2) == BINARY_VECTOR)
        {
          /* S1 and S2 are in binary format */
          
          if (len1 < len2)
            adjust_alph_size(s1, len2);
          
          add_binary_alph(ALPH_items(s1), ALPH_items(s2), len2);
        }
      else
        {
          /* S1 is in binary, S2 in label format */
          
          id_type max_id = max_label_item(ALPH_items(s2), len2);
          
          if (len1 <= max_id && len2 > 0)
            adjust_alph_size(s1, max_id + 1);
          
          add_label_alph(ALPH_items(s1), ALPH_items(s2), len2);
        }
    }
  else   /* S1 is in label format */
    {
      id_type *new_items;
      int new_len, new_max;
      id_type max1 = max_label_item(ALPH_items(s1), len1);
      
      if (ALPH_type(s2) == BINARY_VECTOR)
        {
          /* S1 label, S2 binary */
          
          if (max1 >= len2)
            new_len = max1 + 1;
          else
            new_len = len2;
        }
      else
        {
          /* S1, S2  both in label format. */
          int max2 = max_label_item(ALPH_items(s2), len2);
          
          if (max1 >= max2)
            new_len = max1 + 1;
          else
            new_len = max2 + 1;       
        }
      /* beginning of changes %%TSY 11/18/92 */
      new_items = make_alph_items(new_len, &new_max);
      
      add_label_alph(new_items, ALPH_items(s1), len1);
      
      if (ALPH_type(s2) == BINARY_VECTOR)
        add_binary_alph(new_items, ALPH_items(s2), len2);
      else
        add_label_alph(new_items, ALPH_items(s2), len2);
      
      free_alph_items(s1);
      ALPH_items(s1) = new_items;
      ALPH_len(s1) = new_len;
      ALPH_max(s1) = new_max;
      ALPH_type(s1) = BINARY_VECTOR;
    }  
  /* end of changes %%TSY 11/18/92 */
  
  return (s1);
} /* sigma_union_dont_copy */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

ALPHABETptr sigma_intersect(ALPHABETptr s1, ALPHABETptr s2)
     /*****
      *
      * Returns the intersection of S1 and S2. 
      *
      *****/
{
  id_type *check_items, *result_items;
  int   i, lim;
  ALPHABETptr check, result;
  
  label_to_binary(s1);
  label_to_binary(s2);
  
  /* Copy the shorter alphabet and remove items that are not
     present in the other one.
     */
  
  if (ALPH_len(s1) <= ALPH_len(s2))
    {
      result = copy_alphabet(s1);
      check = s2;
    }
  else
    {
      check = s1;
      result = copy_alphabet(s2);
    }
  
  lim = ALPH_len(result);
  check_items = ALPH_items(check);
  result_items = ALPH_items(result);
  
  for (i = 0; i < lim; i++, result_items++)
    if (*check_items++ == 0)
      *result_items = 0;
  
  return (result);
}
#endif /* SLIM_FST */

ALPHABETptr sigma_difference(ALPHABETptr s1, ALPHABETptr s2)
     /*****
      *
      * Returns the difference of S1 and S2 as a binary vector.
      * (all those elements in S1 that are not in S2, i.e. a subset of S1)
      *
      *
      *****/
{
  ALPHABETptr diff;
  id_type id, max_id ;
  int  i, len, s1_len = ALPH_len(s1), s2_len = ALPH_len(s2);
  
  if (s1_len == 0)
    return(make_alph(0, BINARY_VECTOR));
    
  if (ALPH_type(s1) == BINARY_VECTOR)
    diff = copy_alphabet(s1);
  else
    {
      diff = labels_to_alph(ALPH_items(s1), s1_len, BINARY_VECTOR);
      s1_len = ALPH_len(diff);
    }
      
  if (ALPH_type(s2) == BINARY_VECTOR)
    {
      if (s1_len <= s2_len)
	len = s1_len;
      else
	len = s2_len;
        
      subtract_binary_alph(ALPH_items(diff), ALPH_items(s2), len);
    }
  else
    for (i = 0; i < s2_len; i++)
      if ((id = ALPH_item(s2, i)) < s1_len)
	ALPH_item(diff, id) = 0;
  /*
    The previous version had at this point, incorrectly,
        ALPH_item(s1, id) = 0;
    making a destructive modification in s1 and not in
    diff as intended.  2000-06-20. LK.
   */
  
  max_id = max_binary_item(ALPH_items(diff), ALPH_len(diff));
  
  if (max_id > 0)
    {
      ALPH_len(diff) = max_id + 1;
      
      /* EPSILON should not be in SIGMA.  Check for it here. */
      if (ALPH_item(diff, 0))
        fsm_fprintf(stderr, "Programmer alert: EPSILON in SIGMA.\n");
    }
  else
    ALPH_len(diff) = 0;
  
  return (diff);
}


ALPHABETptr adjust_alph_size(ALPHABETptr alph, int new_len)
     /*****
      * Extends the size of ALPH_items(alph), if necessary.
      * Fails if alph is NULL.
      * Works properly if alph is empty (len or max = 0).
      * Returns alph (same as the argument).
      * 
      * Logic:
      * If new_len <= ALPH_len(alph), does nothing.
      * If new_len > ALPH_len(alph), sets ALPH_len(alph) to new_len.
      * If new_len > ALPH_max(alph), 
      *         (1) allocates space for new alph_items.
      *         (2) copies old ALPH_items onto new_items.
      *         (3) releases old ALPH_items.
      *         (4) Sets new ALPH_max & ALPH_items appropriately.
      * 
      * NOTE:  Memory error checking is done by lower level functions. 
      * 
      *   %%TSY 11/18/92
      *****/
{

  if (!alph)
    {
      handle_error("can't adjust null alph", "adjust_alph_size", 0);
      return (NULL);
    }
  
  /* new_len++; */
  if (new_len > ALPH_len(alph))
    /* else do nothing -- alph is already big enough */
    {
      if (new_len > ALPH_max(alph)) /* need to resize */
        {
          id_type *new_items;
	  int new_max;
          
          new_items = make_alph_items(new_len, &new_max);
          /* Since all cells are initialized to 0 by make_alph_items,
             we only need to copy the old values */
	  /* 3.3.1997: memmove instead of memcpy TG */
	  /* 18.4.1997: mempy instead of memmove MC (memmove is not
						     supported at PARC) */
          memcpy(new_items, ALPH_items(alph),
                 (size_t) sizeof(id_type) * ALPH_len(alph));
          
          free_alph_items(alph);
          ALPH_items(alph) = new_items;
          ALPH_max(alph) = new_max;
        }
      ALPH_len(alph) = new_len;
    }
  return (alph);
}


ALPHABETptr binary_to_label (ALPHABETptr a)
     /******
      *
      * Converts an alphabet from binary form to label form.
      * See types.h for details on these forms.
      *
      ******/
{
  if (a == NULL)
    return(NULL);
  
  if (ALPH_type(a) != LABEL_VECTOR)
    {
      /* For a null alphabet, only change the type indicator. */
      if (ALPH_len(a) > 0)
        {
          id_type *new_labels = NULL;
	  int i, label_count, pos, new_max = 0;
          
          for (i = 0, label_count = 0; i < ALPH_len(a); i++)
            if (ALPH_item(a,i) == 1)
              label_count++;
          
          if (label_count > 0)
            {
              new_labels = make_alph_items(label_count, &new_max);
              if (new_labels == NULL)
                not_enough_memory("BINARY_TO_LABEL");
              
              for (i = 0, pos = 0; i < ALPH_len(a); i++)
                if (ALPH_item(a,i) == 1)
                  {
                    new_labels[pos] = i;
                    pos++;
                  }
            }
          
          free_alph_items(a);
          
          ALPH_len(a) = label_count;
          ALPH_max(a) = new_max;
          ALPH_items(a) = new_labels;
        }
      
      ALPH_type(a) = LABEL_VECTOR;
    }
  
  return (a);
}/* binary_to_label */


ALPHABETptr label_to_binary (ALPHABETptr a)
     /******
      *
      * Converts an alphabet from labels form to binary form.
      * See fsm-types.h for details on these forms.
      *
      ******/
{
  if (a == NULL)
    return(NULL);
  
  if (ALPH_type(a) != BINARY_VECTOR)
    {
      /* For a null alphabet, only change the type indicator. */
      if (ALPH_len(a) > 0)
        {
          /* items may not be sorted, find largest one */
          id_type max_lab = 0, *new_binaries;
	  int i, new_max;
          
          for (i = 0; i < ALPH_len(a); i++)
            if ( (ALPH_item(a,i) > max_lab) && (ALPH_item(a,i) != ID_NO_SYMBOL))
              max_lab = ALPH_item(a,i);
          
          /* MAX_LAB remains 0 only when EPSILON is the only symbol
             in the alphabet. */
          if (max_lab > 0 || ALPH_item(a, 0) == EPSILON)
            {
              /* vector must be one longer than largest label */
              max_lab++;
              
              new_binaries = make_alph_items(max_lab, &new_max);
              
              if (new_binaries == NULL)
                not_enough_memory("LABEL_TO_BINARY");
              
              for (i = 0; i < ALPH_len(a); i++)
		{
		   if (ALPH_item(a,i) != ID_NO_SYMBOL)
		     new_binaries[ALPH_item(a,i)] = 1;
		}
            }
          else
            new_binaries = NULL;
          
          free_alph_items(a);
          
          ALPH_items(a) = new_binaries; 
          ALPH_len(a) = max_lab;
          ALPH_max(a) = new_max;
        }
      ALPH_type(a) = BINARY_VECTOR;
    }
  
  return (a);
} /* label_to_binary */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

ALPHABETptr expand_sigma(ALPHABETptr sig, int newsize)
     /*****
      *
      * Increases the length of SIG to NEWSIZE.
      *
      *****/
{ 
  handle_warning("This function is obsolete.  Call adjust_alph_size() instead",
                 "expand_sigma", 0);
  
  return (adjust_alph_size(sig, newsize));
}

#endif /* SLIM_FST */

ALPHABETptr sigma_add_to (ALPHABETptr sig, id_type newsym)
     /*****
      *
      * Adds NEWSYM to SIG.  If NEWSYM is already in SIG,
      * sigma_add_to() has no effect. Does not add EPSILON
      * and ID_NO_SYMBOL.
      *
      *****/
{
  if (newsym != EPSILON && newsym != ID_NO_SYMBOL)
    {
      if (ALPH_type(sig) != BINARY_VECTOR)
        label_to_binary(sig);
      
      if (newsym >= ALPH_len(sig))
	{
	  if (newsym >= ALPH_max(sig))
	    adjust_alph_size(sig, newsym + 1);
	  else
	    ALPH_len(sig) = newsym + 1;
	}
      
      ALPH_item(sig, newsym) = 1;
    }
  return (sig);
} /* sigma_add_to */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

ALPHABETptr sigma_remove_from (ALPHABETptr sig, id_type sym)
{
  /* Removes SYM from SIG.  If SIG is NULL or SYM is not in SIG,
     this has no effect.  If SIG is of type LABEL_VECTOR,
     call LABEL_REMOVE_FROM to do the job.
     */
  
  if (sig == NULL)
    return(sig);
  
  if (ALPH_type(sig) == LABEL_VECTOR)
    return(label_remove_from(sig, sym));
  
  if (sym < ALPH_len(sig))
    {
      ALPH_item(sig, sym) = 0;

      if (ALPH_len(sig) == sym + 1)
        {
          uint16 i = sym - 1;

          /* Set a  new length mark. */

         while (i > 0 && ALPH_item(sig, i) == 0)
           i--;

          /* We are not enforcing here the convention that EPSILON
             is not properly in the sigma.
             */

         if (i > 0)
          ALPH_len(sig) = i + 1;
         else if (ALPH_item(sig, i) == 1)
          ALPH_len(sig) = 1;          /* EPSILON is the only symbol. (?) */
         else
           ALPH_len(sig) = 0;         /* No symbols left in the sigma. */
        }
    }
  
  return (sig);
}

ALPHABETptr label_remove_from(ALPHABETptr labels, id_type lab)
{
  /* Removes LAB from LABELS.  If LABELS is NULL or LAB
     is not in LABELS, this has no effect.  If LABELS is of
     type LABEL_VECTOR, we do what SIGMA_REMOVE_FROM would do.
  */
  
  if (labels == NULL)
    return(labels);
  
  if (ALPH_type(labels) == BINARY_VECTOR)
    {
      if (lab < ALPH_len(labels))
        ALPH_item(labels, lab) = 0;
    }
  else
    {
      /* Eliminate every occurrent of LAB from LABELS (there should
         be only one, if any, but we check for more just in case.)
         by moving the rest of the labels one step forward.  The
         vector remains the same but the length of the alphabet
         is reduced.
      */
      
      id_type   *items = ALPH_items(labels);
      int  found_p = FALSE, i = 0, j, len = ALPH_len(labels);
      
      while (i < len)
        {
          if (items[i] == lab)
            {
              /* Move all the remaining labels forward by one position
                 and subtract from the label count. Keep on checking
                 just in case there are duplicate labels. (Remove this
                 debugging feature eventually.)
	      */
              if (found_p == TRUE)
                {
                  fsm_fprintf(stderr, "Programmer alert: two occurrences of ");
                  fsm_fprintf(stderr, "the same label in a label alphabet.\n");
                }
              found_p = TRUE;
              
              for (j = i + 1; j < len; j++)
                items[j - 1] = items[j];
              len--;
            }
          else
            i++;
        }
      ALPH_len(labels) = len;
    }
  return(labels);
}
#endif /* SLIM_FST */

void label_to_sigma (ALPHABETptr sigma, id_type id)
{
  /* Augment SIGMA with the label ID, or its components in the
     case ID is an fst tuple and FSM_MODE is not TWOL_MODE.
     */
  
  if (FSM_MODE == TWOL_MODE)
    sigma_add_to(sigma, id);
  else
    {
      int i;
      LABELptr label = id_to_label(id);
      
      if (LABEL_arity(label) == 1)
        sigma_add_to(sigma, id);
      else
        for (i = 0; i < (int)LABEL_arity(label); i++)
          {
            id = TUPLE_labels(LABEL_tuple(label))[i];
            sigma_add_to(sigma, id);
          }
    }
}                       

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int other_in_alph_p(ALPHABETptr alph)
{
  /* Returns TRUE is ALPH contains OTHER or some symbol pair
     containing OTHER. */

  int i, restore_p = FALSE;
	
  if (ALPH_type(alph) == BINARY_VECTOR)
    {
      binary_to_label(alph);
      restore_p = TRUE;
    }

  for (i = 0; i < ALPH_len(alph); i++)
    if (upper_id(ALPH_item(alph,i)) == OTHER ||
	lower_id(ALPH_item(alph, i)) == OTHER)
      break;
			
  if (restore_p)
    label_to_binary(alph);
			
  /* If we broke the loop, OTHER is present. */
	
  return(i < ALPH_len(alph));
}

ALPHABETptr label_components(ALPHABETptr alph)
{
  /* Returns a binary alphabet that contains all the symbols that appear
     in ALPH, either as such, or as a component of some symbol pair. */

  ALPHABETptr syms;
  int i;
	
  syms = make_alph(max_label_component(alph, BOTH_SIDES) + 1,
		   BINARY_VECTOR);
				   	 
  for (i = 0; i < ALPH_len(alph); i++)
    label_to_sigma(syms, ALPH_item(alph, i));			   	 

  return(syms);
}
#endif /* SLIM_FST */

static ALPHABETptr sigma_struct (NETptr net)
{
  STATEptr s;
  ARCptr a;
  ALPHABETptr sigma;
  
  sigma = make_alph(LIM_id_count(LABEL_MAP), BINARY_VECTOR);    
  
  for (s = NET_states(net); s != NULL; s = next_state(s))
    for (a = STATE_arc_set(s); a != NULL; a = next_arc(a))
      label_to_sigma(sigma, ARC_label(a));
  
  /* EPSILON is never in SIGMA */
  ALPH_item(sigma, EPSILON) = 0;
  
  return (sigma);
} /* sigma_struct */



ALPHABETptr update_label_alphabet(NETptr net)
{
  /* Recomputes the label alphabet of net and sets NET_labels
     to the correct value.
     */

  STATEptr state;
  ARCptr arc;

  if (!NET_labels(net))
    NET_labels(net) = make_alph(0, BINARY_VECTOR);
  else
    {
      label_to_binary(NET_labels(net));
      ALPH_len(NET_labels(net)) = 0;
    }

  for (state = NET_states(net); state; state = next_state(state))
    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
      sigma_add_to(NET_labels(net), ARC_label(arc));

  binary_to_label(NET_labels(net));

  return(NET_labels(net));
}

id_type max_label_component(ALPHABETptr alph, int side)
{
  /* Computes the highest label component on one or the
     other side in the labels of the alphabet ALPH.
     */

  id_type *items, lab_id, sym_id, max = 0;
  int i, len, restore_p = FALSE;

  if (ALPH_type(alph) == BINARY_VECTOR)
    {
      restore_p = TRUE;
      binary_to_label(alph);
    }
  
  len = ALPH_len(alph);
  items = ALPH_items(alph);

  for (i = 0; i < len; i++)
    {
      lab_id = *items++;
      
      if (side == UPPER)
	sym_id = upper_id(lab_id);
      else
	sym_id = lower_id(lab_id);

      if (sym_id > max)
	max = sym_id;

      if (side == BOTH_SIDES && lab_id != sym_id)
	{
	  sym_id = upper_id(lab_id);
	  if (sym_id > max)
	    max = sym_id;
	}
    }
  if (restore_p)
    label_to_binary(alph);

  return(max);
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



ALPHABETptr label_sigma(NETptr net)
{
  /* A faster way to collect SIGMA than SIGMA_STRUCT.  Presupposes
     that the NET_labels is properly maintained.
     */

  if (FSM_MODE == STANDARD_MODE)
    return(side_sigma(net, BOTH_SIDES));
  else
    {
      ALPHABETptr labs = binary_to_label(NET_labels(net));
      return(labels_to_alph(ALPH_items(labs), ALPH_len(labs), BINARY_VECTOR));
    }
}

ALPHABETptr sigma(NETptr net)
{
  /* Return the contents of NET_sigma field.  If SIGMA is unspecified,
     compute it from the NET_labels field, which must be properly
     maintained by all operations.
     */
  if (NET_sigma(net) == NULL)
    {
      if (NET_labels(net) == NULL)
	{
	  handle_warning("NET_labels unspecified", "SIGMA", 0);
	  NET_sigma(net) = sigma_struct(net);
	}
      else    
	NET_sigma(net) = label_sigma(net);
    }
  
  return (NET_sigma(net));
}/* sigma */




ALPHABETptr upper_sigma(NETptr net)
{
  return(side_sigma(net, UPPER));
}

ALPHABETptr lower_sigma(NETptr net)
{
  return(side_sigma(net, LOWER));
}


ALPHABETptr excluded_symbols (NETptr net)
{
  /* Returns a (BINARY_VECTOR) alphabet containing all symbols that are
     in sigma but not in the labels. */

  ALPHABETptr excl, symbols;

  symbols = label_sigma (net);
  excl = sigma_difference (NET_sigma (net), symbols);
  free_alph (symbols);

  return (excl);
}

void update_net_labels_and_sigma (NETptr net)
{
  /* Updates the label and sigma alphabet and the arity of the net.
     To be called on nets that have undergone some transformation.
     We allow two cases:

     [1] labels and sigma both NULL:
             labels and sigma are both rebuilt from the arc labels

     [2] labels and sigma both not NULL:
             labels and sigma are both consistent wrt. the previous
	     form of the net (before the transformation), and carry
	     useful information, namely IDs that are not part of
	     OTHER. These IDs must be preserved when labels and sigma
	     are rebuilt.

     Other cases are not allowed !!!
     */

  ALPHABETptr sigma, labels, sigdiff;  int i;

  /* distinguish cases [1] and [2] */

  if ((! NET_labels (net)) && (! NET_sigma (net)))  /* case [1] */
    {
      sigdiff = NULL;
    }
  else if ((NET_labels (net)) && (NET_sigma (net)))  /* case [2] */
    {
      sigdiff = excluded_symbols (net);

      free_alph (NET_labels (net));
      free_alph (NET_sigma (net));
      NET_labels (net) = NET_sigma (net) = NULL;
    }
  else
    prog_error ("update_net_labels_and_sigma",
		"labels and sigma must be either both NULL or both non-NULL");

  /* update labels and sigma */

  update_label_alphabet (net);
  sigma = label_sigma (net);

  if (sigdiff)
    {
      NET_sigma (net) = sigma_union (sigma, sigdiff);
      free_alph (sigma);
      free_alph (sigdiff);
    }
  else
    {
      NET_sigma (net) = sigma;
    }

  /* update arity */

  NET_arc_label_arity (net) = 1;
  labels = NET_labels (net);

  for (i=0;  i< ALPH_len (labels);  i++)
    if (LABEL_arity (id_to_label (ALPH_item (labels, i))) == 2)
      {
	NET_arc_label_arity (net) = 2;
	break;
      }
}

void set_net_arity(NETptr net)
     /* Displaced from modify.c 2000-4-21 TG */
{
  /* Adjusts the arity of NET to the labels that appear in the label
     set. */
  
  LABELptr label;
  id_type  *items;
  int i, len;
  int16  net_arity = 1;
  
  binary_to_label(NET_labels(net));
  len = ALPH_len(NET_labels(net));
  items = ALPH_items(NET_labels(net));
  
  for (i = 0; i < len; i++)
    {
      label = id_to_label(*items++);
      if ((int16)LABEL_arity(label) > net_arity)
        net_arity = LABEL_arity(label);
    }
  NET_arc_label_arity(net) = net_arity;
}


/************************
 *
 * Managing property lists
 *
 ************************/


void add_prop(NETptr net, FAT_STR attr, OBJECTptr value)
     /*****
      *
      * Adds a new propery to NET.  If the property already
      * exists, the new value replaces the old one.
      *
      *****/
{
  PROPptr prop, last_prop = NULL;
  
  prop = NET_properties(net);
  
  for (prop = NET_properties(net); prop != NULL; prop = next_prop(last_prop))
    {
      if (fat_strcmp(PROP_attr(prop), attr) == 0)
        {
          PROP_val(prop) = value;
          return;
        }
      last_prop = prop;
    }
  
  prop = (PROPptr) calloc((size_t) 1,
                          (size_t) sizeof (PROPtype));
  
  if (prop == NULL)
    not_enough_memory("ADD_PROP");
  
  PROP_attr(prop) = attr;
  PROP_val(prop) = value;
  
  if (last_prop == NULL)
    NET_properties(net) = prop;
  else
    next_prop(last_prop) = prop;
} /* add_prop */


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
OBJECTptr make_object(int type)
{
  OBJECTptr obj = (OBJECTptr) malloc((size_t) 1 * sizeof(OBJECTtype));

  if (!obj)
    not_enough_memory("MAKE_OBJECT");

  OBJ_type(obj) = type;

  return(obj);
}

SEQUENCEptr make_sequence(int len, int type)
{
  SEQUENCEptr seq = (SEQUENCEptr) malloc((size_t) 1 * sizeof(SEQUENCEtype));

  if (!seq)
    not_enough_memory("MAKE_SEQUENCE");

  SEQ_type(seq) = type;
  SEQ_len(seq) = len;
  if (len > 0)
    switch(type)
      {
      case Object:
	SEQ_objects(seq) = (OBJECTptr *) malloc(len * sizeof(OBJECTptr));
	break;
      case Int:
	SEQ_ints(seq) = (long int *) malloc(len * sizeof(long int));
	break;
      case Character:
	SEQ_characters(seq) = (FAT_CHAR *) malloc(len * sizeof(FAT_CHAR));
	break;
      case String:
	SEQ_characters(seq) = (FAT_STR) malloc(len * sizeof(FAT_STR));
	break;
      case Symbol_package:
	SEQ_symbol_package(seq) =
	  (IO_SYMBOL_PACKAGEptr) malloc(len * sizeof(IO_SYMBOL_PACKAGEtype));
	break;
      case Symbol:
	SEQ_symbols(seq) = (IO_SYMBOLptr) malloc(len * sizeof(IO_SYMBOLptr));
	break;
      case Array:
	SEQ_array(seq) = (SEQUENCEptr *) malloc(len * sizeof(SEQUENCEptr));
	break;
      case List:
	SEQ_list(seq) = (SEQUENCEptr *) malloc(len * sizeof(SEQUENCEptr));
	break;
      case Byte_block:
	SEQ_byte_block(seq) =
	  (BYTE_BLOCKptr *) malloc(len * sizeof(BYTE_BLOCKptr));
	break;
      default:
	handle_warning("Unknown type", "MAKE_SEQUENCE", type);
	free(seq);
	return(NULL);
      }
  else
    seq->data.objects = NULL;

  return(seq);
}

static void add_prop_of_type(NETptr net, FAT_STR attr, int type, void *value)
{
  OBJECTptr obj = make_object(type);
  
  switch (type)
    {
    case(Int):
      OBJ_integer(obj) = (int) value;
      break;
    case(Character):
      fat_charcpy(OBJ_character(obj), *(FAT_STR) value);
      break;
    case(String):
      OBJ_str(obj) = (FAT_STR) value;
      break;
    case(Symbol):
      OBJ_symbol(obj) = (IO_SYMBOLptr) value;
      break;
    case(Array):
      OBJ_array(obj) = (SEQUENCEptr) value;
      break;
    case(List):
      OBJ_list(obj) = (SEQUENCEptr) value;
      break;
    case(Byte_block):
      OBJ_byte_block(obj) = (BYTE_BLOCKptr) value;
      break;
    default:
      handle_error("Unknown type", "ADD_PROP_OF_TYPE", type);
     break;
    }
  
  add_prop(net, attr, obj);
}

void add_integer_prop(NETptr net, FAT_STR attr, int value)
{
  add_prop_of_type(net, attr, Int, (void *) value);
}

void add_character_prop(NETptr net, FAT_STR attr, char value)
{
  /* Stores an ordinary character as a fat character. */

  FAT_CHAR fatc;

  CHARSET(&fatc) = '\0';
  CHARNUM(&fatc) = value;
  
  add_prop_of_type(net, attr, Character, (void *) fatc);
}

void add_string_prop(NETptr net, FAT_STR attr,  FAT_STR value)
{
  add_prop_of_type(net, attr, String, (void *) value);
}

void add_symbol_prop(NETptr net, FAT_STR attr, IO_SYMBOLptr value)
{
  add_prop_of_type(net, attr, Symbol, (void *) value);
}

void add_array_prop(NETptr net, FAT_STR attr, SEQUENCEptr value)
{
  add_prop_of_type(net, attr, Array, (void *) value);
} 

void add_list_prop(NETptr net, FAT_STR attr, SEQUENCEptr value)
{
  add_prop_of_type(net, attr, List, (void *) value);
} 

void add_byte_block_prop(NETptr net, FAT_STR attr, BYTE_BLOCKptr value)
{
  add_prop_of_type(net, attr, Byte_block, (void *) value);
}

void name_network(NETptr net, char *name)
{
  add_string_prop(net, NETWORKNAME_ATTR, FST_thin_to_fat(name));
}
#endif /* SLIM_FST */

OBJECTptr get_value(NETptr net, FAT_STR attr)
{
  /****
   * 
   * Retrives a property, given its ATTR.
   *
   ****/
  PROPptr cur;
  
  for (cur = NET_properties(net); cur != NULL; cur = next_prop(cur))
    if (fat_strcmp(PROP_attr(cur), attr) == 0)
      return (PROP_val(cur));
  
  return NULL;
} /* get_value */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
static int update_sublist(SEQUENCEptr seq, FAT_STR fs1, FAT_STR fs2)
{
  /* Looks for a previous instance of (FS1 FS2) pair in the
     sequence. If the new FS2 is different from OLD_FS2, OLD_FS2
     is replaced by FS2. If no previous instance of FS1 is found,
     the (FS1 FS2) pair is added to the end of SEQ.
     Unused or replaced fat strings are freed. Assumes that SEQ
     is a list whose items are lists that contain two fat strings.
     */

  int i;
  SEQUENCEptr sublist;

  for (i = 0; i < SEQ_len(seq); i++)
    {
      sublist = SEQ_list(seq)[i];
      switch (SEQ_type(sublist))
	{
	case String:
	  switch (SEQ_len(sublist))
	    {
	    case 2:
	      if (!(fat_strcmp(SEQ_strings(sublist)[0], fs1)))
		{
		  /* Found a match for FS1! */
		  free(fs1);

		  if (!(fat_strcmp(SEQ_strings(sublist)[1], fs2)))
		    {
		      /* Found a match for FS2. No update needed. */
		      free(fs2);
		    }
		  else
		    {
		      /* Replace the old string with the new FS2 */
		      free(SEQ_strings(sublist)[1]);
		      SEQ_strings(sublist)[1] = fs2;
		    }
		  return(0);
		}
	      break;
	    default:
	      handle_warning("Expecting a list of two strings",
			     "UPDATE_SUBLIST", SEQ_len(sublist));
	      return(1);
	    }
	  break;
	default:
	  handle_warning("Expecting a list of strings", "UPDATE_SUBLIST",
			 SEQ_type(sublist));
	  return(1);
	}
    }

  /* Not found. Make a new sublist to record the pair. */

  sublist = make_sequence(2, String);
  SEQ_strings(sublist)[0] = fs1;
  SEQ_strings(sublist)[1] = fs2;

  /* Append the new list to the end of the current list */

  if (SEQ_len(seq) == 0)
    {
      /* No sublist yet. Allocate a new sequence pointer  */
      SEQ_list(seq) = (SEQUENCEptr *) malloc(sizeof(SEQUENCEptr));
      }
    else
      {
	/* Extend the current sequence by one. */
	if (!(SEQ_list(seq) =
	      realloc(SEQ_list(seq),(SEQ_len(seq) + 1) * sizeof(SEQUENCEptr))))
	  not_enough_memory("PAIR_TO_LIST");
      }
  
  /* Make the new sublist the last element of the sequence */

    SEQ_list(seq)[SEQ_len(seq)] = sublist;
    SEQ_len(seq)++;
    return(0);
}


int mark_toolversion(NETptr net, char *tool, char *version)
{
  /* Appends the TOOL name and the VERSION string as a list
     of two fat strings under the property TOOLVERSIONS on
     the property list of the net. It is assumed that the
     value of this attribute, if it exists, is a list containing
     (TOOL VERSION) pairs, each item being a list of two strings.
     For example TOOLVERSIONS: ( ("XFST" "5.6.7")).
     If the tool name is already on the the list, the new
     version replaces the previous one.
     */

  OBJECTptr obj;
  SEQUENCEptr seq;

  if ((obj = get_value(net, TOOLVERSIONS_ATTR)))
    switch (OBJ_type(obj))
      {
      case(List):
	seq = OBJ_list(obj);
	break;
      default:
        handle_warning("Value of TOOLVERSIONS should be a list of lists.",
		       "MARK_VERSION", OBJ_type(obj));
      return(1);
      }
  else
    {
      obj = make_object(List);
      seq = make_sequence(0, List); /* No content yet */
      OBJ_list(obj) = seq;
      add_prop(net, TOOLVERSIONS_ATTR, obj);
    }
  return(update_sublist(seq, thin_to_fat(tool), FST_thin_to_fat(version)));
}

int nv_mark_version(NVptr nv, char *tool, char *version)
{
  /* Records TOOL and VERSION under TOOLVERRSIONS on the
     property list of each network in NV.
     */

  int i, error_p = FALSE;
  
  for (i = 0; i < NV_len(nv); i++)
    if (mark_toolversion(NV_net(nv, 0), tool, version))
      error_p = TRUE;

  return(error_p);
}
#endif /* SLIM_FST */

PROPptr remove_prop(NETptr net, FAT_STR attr)
     /*****
      *
      * Removes the property keyed by ATTR from net.  Returns the
      * property so that it can be reclaimed or NULL if no value
      * for ATTR is found.
      *
      *****/
{
  PROPptr prev = NULL, cur;
  
  for (cur = NET_properties(net);
       cur != NULL;
       cur = next_prop(cur))
    {
      /* if first one matches */
      if (fat_strcmp(PROP_attr(cur), attr) == 0)
        {  
          if (prev == NULL)
            NET_properties(net) = next_prop(cur);
          else
            next_prop(prev) = next_prop(cur);
          
          return(cur);
        }
      prev = cur;
    }
  return(NULL);
} /* remove_prop */

/****************************
 *
 *  Reclaiming space
 *
 ****************************/

void free_alph_items(ALPHABETptr a)
     /*****
      *
      * Places items (ALPH_items from an ALPHABET)
      * on the correct free list.  To free items 
      * only, call this function.  To free entire
      * ALPHABET struct, call free_alph().
      *
      *****/
{
  if (a != NULL)
    {
      id_type *items = ALPH_items(a);
      int p;
      
      if (items != NULL)
        {
          if (ALPH_max(a) == 0)
            handle_error("Illegal ALPH max", "FREE_ALPH_ITEMS", 0);
          p = log_base_2(ALPH_max(a));          
          free_cell((void *) items, LV_HEAPS[p]);
          ALPH_items(a) = NULL;
          ALPH_len(a) = 0;
          ALPH_max(a) = 0;
        }
    }
}

void free_alph(ALPHABETptr a)
     /*****
      *  Release storage occupied by alphabet & alphabet items.
      *
      *  ALPHABET structs & ALPH_items (id_type) vectors are now
      *  stored in HEAPs.  All released structures are now placed
      *  on free lists for re-use.  - TSY 6/92
      *
      *****/
{       
  if (a != NULL)
    {
      free_alph_items(a);
      free_cell((void *) a, ALPH_HEAP);
    }
}


static int same_p(void *x, void *y)
{
  /* Returns 0 if x and y are the same pointer, otherwise 1.
     N.B. It is important that the function does not return
     a negative value. If the hash cell contains more than
     one item, they are stored in the order determined by
     the hashing function. If the comparison function returns
     a negative value, get_hash() stops looking for a match
     assuming that the remaining items in the cell do not
     need to be checked.
     */

  if (x == y)
    return(0);
  else
    return(1);
}


int common_fat_string_p (FAT_STR name)
{
  /* Recognize common attribute and package names so that they do
     not get clobbered when data structures are freed.
     */
  	
  void *value;
  
  get_hash(COMMON_FAT_STRINGS, name, &value, hash_fatstr, same_p);

  if (!value)
    return(FALSE);
  else
    return(TRUE);
}


int symbol_name_p(FAT_STR name)
{
  /* Recognize fat strings that are in use in the atom name field
     of a label to prevent them from being clobbered when data
     structures are freed.
     */
  id_type id;

  if ( atomic_label_to_id(&id, name) != ERR_NOLABEL && LABEL_atom(id_to_label(id)) == name)
    return(TRUE);
  else 
    return(FALSE);

}


static void maybe_free_string(FAT_STR fs)
{
  /* Reclaim fat string if it is not in use as constant attribute
     (NETWORK-NAME) or pakcage name (KEYWORD_PACKAGE) or as
     a name of a symbol in the label table.
     */
    
  if (fs && !(common_fat_string_p(fs) || symbol_name_p(fs)))
    free((void *) fs);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
BYTE_BLOCKptr make_byte_block(unsigned long len)
{
  BYTE_BLOCKptr byte_block = (BYTE_BLOCKptr) malloc(sizeof(BYTE_BLOCKtype));
  void *block = malloc((size_t) len * sizeof(char));
  
  if (!(byte_block && block))
    not_enough_memory("MAKE_BYTE_BLOCK");
    
  BYTE_BLOCK_len(byte_block) = len;
  BYTE_BLOCK_block(byte_block) = block;
  
  return(byte_block);
}
#endif /* SLIM_FST */

void free_byte_block(BYTE_BLOCKptr byte_block)
{
  if (byte_block)
    {
      if (BYTE_BLOCK_block(byte_block))
	free((void *) BYTE_BLOCK_block(byte_block));
         
      free((void *) byte_block);
    }
}



void free_sequence(SEQUENCEptr s)
{
  int i;
  
  if (s == NULL)
    return;

  switch (SEQ_type(s))
    {
    case (Object):		/* type -1 */
      for (i = 0; i < SEQ_len(s); i++)
        free_object(SEQ_objects(s)[i]);
      if (SEQ_objects(s) != NULL)
          free((void *) SEQ_objects(s));
      break;
    case (Int):			/* type 0 */
      if (SEQ_ints(s) != NULL)
          free((void *) SEQ_ints(s));
      break;
    case (Character):		/* type 1 */
      if (SEQ_characters(s) != NULL)
          free((void *) SEQ_characters(s));
      break;
    case (String):		/* type 2 */
      for (i = 0; i < SEQ_len(s); i++)
	maybe_free_string(SEQ_strings(s)[i]);
      if (SEQ_strings(s) != NULL)
          free((void *) SEQ_strings(s));
      break;
    case (Symbol_package):	/* type -3 */
      for (i = 0; i < SEQ_len(s); i++)
        maybe_free_string(SYM_PACK_sym_names(SEQ_symbol_package(s))[i]);
      if (SYM_PACK_sym_names(SEQ_symbol_package(s)) != NULL)
          free((void *) SYM_PACK_sym_names(SEQ_symbol_package(s)));
      if (SEQ_symbol_package(s) != NULL)
          free((void *) SEQ_symbol_package(s));
      break;
    case (Symbol):		/* type 3 */
      for (i = 0; i < SEQ_len(s); i++)
        {
   	  maybe_free_string(IO_SYM_name(SEQ_symbols(s) + i));
        }
      if (SEQ_symbols(s) != NULL)
          free((void *) SEQ_symbols(s));
      break;
    case (Array):		/* type 4 */
      for (i = 0; i < SEQ_len(s); i++)
        free_sequence(SEQ_array(s)[i]);                         
      if (SEQ_array(s) != NULL)
          free((void *) SEQ_array(s));
      break;
    case (List):		/* type 5 */
      for (i = 0; i < SEQ_len(s); i++)
        free_sequence(SEQ_list(s)[i]);                          
      if (SEQ_list(s) != NULL)
          free((void *) SEQ_list(s));
      break;
    case (Byte_block):         /* type 6 */
      for (i = 0; i < SEQ_len(s); i++)
	free_byte_block(SEQ_byte_block(s)[i]);
      break;
    }
  free((void *) s);
}/* free_sequence */


void free_object(OBJECTptr obj)
{       
  if (obj != NULL)
    {
      switch (OBJ_type(obj))
        {
	case(String):
	  maybe_free_string((void *) OBJ_str(obj));
          break;
	case(Symbol):
   	  maybe_free_string(IO_SYM_name(OBJ_symbol(obj)));
	  free((void *) OBJ_symbol(obj));
	  break;
	case(Array):
	  free_sequence(OBJ_array(obj));
          break;
	case(List):
	  free_sequence(OBJ_list(obj));
          break;
	case(Byte_block):
	  free_byte_block(OBJ_byte_block(obj));
	  break;
        }
      free((void *) obj);
    }
}

void free_prop(PROPptr prop)
{
  if (prop != NULL)
    {
      maybe_free_string(PROP_attr(prop));
      free_object(PROP_val(prop));
      free((void *) prop);
    }
}

void free_props(NETptr net)
{
  PROPptr prop, next;
  
  prop = NET_properties(net);
  
  while (prop != NULL)
    {
      next = next_prop(prop);      
      free_prop(prop);     
      prop = next;
    }
}

/* #define ARC_FREED  -1 CP:05/05/99*/
/* ARC_FREED is used only in the following function: use DUMMY_LABEL instead */
void free_shared_arc_lists (NETptr net)
{
  STATEptr state;
  ARCptr arc, nextarc, freelist = NULL;

  if (! NET_shared_arc_lists (net))
    return;    /* arc lists are not shared */

  for (state = NET_states(net); state; state = next_state (state))
    {
      for (arc = STATE_arc_set (state); arc; arc = nextarc)
	{
	  if (ARC_label (arc) == DUMMY_LABEL)
	    break;    /* this arc and all following arcs are already freed */

	  nextarc = next_arc (arc);
	  ARC_label (arc) = DUMMY_LABEL;

	  next_arc (arc) = freelist;
	  freelist = arc;
	}

      STATE_arc_set (state) = NULL;
    }

  for (arc = freelist; arc; arc = nextarc)
    {
      nextarc = next_arc (arc);
      free_arc (arc);
    }

  NET_shared_arc_lists (net) = FALSE;
  NET_num_arcs (net) = 0;
}/* free_shared_arc_lists */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
void free_arc_set(STATEptr state, NETptr net)
{
  /* Free the arcs and set the state's arc set to NULL. */
  
  ARCptr arc, next_arc;
  
  for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc)
    {
      next_arc = next_arc(arc);
      free_arc(arc);
      NET_num_arcs(net)--;
    }
  STATE_arc_set(state) = NULL;
}


void free_state_and_arcs(STATEptr state, NETptr net)
{
  /* The state as well as its arcs are put on the freelist. */
  
  free_arc_set(state, net);
  free_state(state);
  NET_num_states(net)--;
}
#endif /* SLIM_FST */

void free_arc_vector(AVptr av, NETptr net)
{
  free_cell(AV_dest_array(av), NET_arc_vector_heap(net));
  free_alph(AV_l_eps(av));
  free_alph(AV_u_eps(av));
  free_alph(AV_l_fld(av));
  free_alph(AV_u_fld(av));
  free_cell(av, ARC_VECTOR_HEAP);
}


static void free_parse_table_nodes(CH_NODEptr *ch)
{
  if (ch)
    {
      int i;
  
      for (i = 0; i < 256; i++)
	{
	  if (ch[i] != NULL)
	    {
	      free_parse_table_nodes(CH_NODEnext(ch[i]));	  
	      CH_NODEid(ch[i]) = ID_NO_SYMBOL;
	      CH_NODEnext(ch[i]) = NULL;
	      free_cell((void *) ch[i], CH_NODE_HEAP);
	      ch[i] = NULL;
	    }
	}  
      free_cell((void *) ch, CH_VECTOR_HEAP);
    }
}

void free_parse_table(PARSE_TBL tbl)
{
  if (tbl)
    {
      free_parse_table_nodes(PARSE_TBL_TOP_NODE(tbl));
      free_cell((void *) tbl, PARSE_TBL_HEAP);
    }
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
void free_net_parse_tables(NETptr net)
{
    if (net)
    {
        free_parse_table(NET_upper_parse_table(net));
        NET_upper_parse_table(net) = NULL;
        free_parse_table(NET_lower_parse_table(net));
        NET_lower_parse_table(net) = NULL;
    }
}
#endif /* SLIM_FST */

void free_range_map(RANGEptr range_map, int len)
{
	if (range_map)
	{
		int i;
		RANGEptr range = range_map;
		for (i = 0; i < len; i++, range++)
			if (RANGE_len(range) > 0)
				free((void *) RANGE_items(range));

		free((void *) range_map);
	}
}


void free_range_vector(RANGE_VECTORptr range_vector)
{
  if (range_vector)
    {
      free_range_map(RANGE_VECTOR_vector(range_vector),
		     RANGE_VECTOR_len(range_vector));
      
      free((void *) range_vector);
    }
}

void free_match_table(MATCH_TABLEptr match_table)
{
  int i;
  int len;

  if (match_table)
    {
      len  = MATCH_TABLE_len(match_table);

      for (i = 0; i < len; i++)
	free_range_vector(MATCH_TABLE_table(match_table)[i]);

      free_range_map(MATCH_TABLE_class(match_table), len);

      free((void *) match_table);
    }
}

static void free_global_match_tables()
/***
 *
 * Frees the 2 global match tables, UPPER_MATCH and LOWER_MATCH.
 * Do not confuse with function free_match_table() that is
 * intended to free the network downrange_map and uprange_map.
 *
 ***/
{ 
    if (UPPER_MATCH != NULL)
    {
        free(UPPER_MATCH);
        UPPER_MATCH= NULL;
    }
    
    if (LOWER_MATCH != NULL)
    {
        free(LOWER_MATCH);
        LOWER_MATCH= NULL;
    }     
} /* free_global_match_tables() */


void free_net_and_periphery(NETptr net)
{
  /* Frees all net data structures except the states and arcs. */

  free_alph(NET_labels(net));
  NET_labels(net)= NULL;
  
  free_alph(NET_sigma(net));
  NET_sigma(net)= NULL;
  
  free_alph(NET_unreduce_key(net));
  NET_unreduce_key(net)= NULL;
  
  free_alph(NET_flag_register(net));
  NET_flag_register(net)= NULL;
  
  free_props(net);
  NET_properties(net)= NULL;
  
  free_heap(NET_arc_vector_heap(net));
  NET_arc_vector_heap(net)= NULL;
  
  free_parse_table(NET_upper_parse_table(net));
  NET_upper_parse_table(net)= NULL;
  
  free_parse_table(NET_lower_parse_table(net));
  NET_lower_parse_table(net)= NULL;
  
  free_range_map(NET_uprange_map(net), NET_range_len(net));
  NET_uprange_map(net)= NULL;
  
  //free_range_map(NET_downrange_map(net), NET_range_len(net));
  NET_downrange_map(net)= NULL;
  
  free_match_table(NET_upmatch_table(net));
  NET_upmatch_table(net)= NULL;
  
  free_match_table(NET_downmatch_table(net));
  NET_downmatch_table(net)= NULL;
  
  free_cell((void *) net, NET_HEAP);
} /* free_net_and_periphery */

static void strip_networks_in_NET_HEAP()
{
    /* Frees all net data structures except the states and arcs:
       the external structures pointed by the net such as the
       range_maps, parse_tables, etc are released; if not, memory
       leaks will occur when freeing NET_HEAP.
       This function is to be called by release_fsm_memory(). */
    NETptr net= NULL;
    BLOCKptr b= NULL;
    long loc= 0, cell_number= HEAP_block_size(NET_HEAP);
    void *cell= NULL;
    
    for (b= HEAP_first(NET_HEAP); b != NULL; b= BLOCK_next(b))
    {
        if (BLOCK_next(b) == NULL)
            /* we are in the last block: iterate until cur_loc */
            cell_number=  HEAP_cur_loc(NET_HEAP);
                
        for (loc = 0; loc < cell_number; loc++)
        {
            cell= (char*)BLOCK_cells(b) + (loc * HEAP_cell_size(NET_HEAP));
            net= (NETptr) cell;
            
            /* Free all net data structures except the states and arcs: */
            if (NET_properties(net) != NULL)
                free_props(net);
  
            if (NET_upper_parse_table(net) != NULL)
                free_parse_table(NET_upper_parse_table(net));

            if (NET_lower_parse_table(net) != NULL)
                free_parse_table(NET_lower_parse_table(net));

            if (NET_uprange_map(net) != NULL)
                free_range_map(NET_uprange_map(net), NET_range_len(net));

            if (NET_downrange_map(net) != NULL)
                free_range_map(NET_downrange_map(net), NET_range_len(net));

            if (NET_upmatch_table(net) != NULL)
                free_match_table(NET_upmatch_table(net));

            if (NET_downmatch_table(net) != NULL)
                free_match_table(NET_downmatch_table(net));
        } /* for loc */
    } /* for b */
} /* strip_networks_in_NET_HEAP */


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#ifndef SMALL_FSM /* No virtual networks in the SMALL_FSM world! */
void free_virtual_network_storage(NETptr net) {
  STATEptr state;
  VirtualInfo *vinfo = NET_virtual_info(net);
  /* --------------------------------------------------------------- */
  /* Free each state's virtual data. This data exists whether or not */
  /* the state was virtual to support flushing virtual arcs.         */
  /* --------------------------------------------------------------- */
  if (vinfo->methods->data_type == STATE_LIST) {
    for (state = NET_states(net); state; state = next_state(state)) {
      CONSptr tail, list = STATE_state_data(state);
      assert(list);
      while (list) {
	tail = cdr(list);
	free(list);
	list = tail;
      }
      STATE_state_data(state) = NULL;
    }
  } else if (vinfo->methods->data_type == STATE_PAIR) {
    for (state = NET_states(net); state; state = next_state(state)) {
      StatePair *data = STATE_state_data(state);
      /* If the state came from a real operand network, 
	 it has no state data. LK 2001-06-11. */

      if (data)
	{
	  free(data);
	  STATE_state_data(state) = NULL;
	}
    }
  } else {
    assert(vinfo->methods->data_type == DATA_STATE);
  }

  /* -----------------------------*/
  /* Free the argument networks?  */
  /* -----------------------------*/

  if (vinfo->op1_reclaimable)
    free_network(vinfo->op1);
  if (vinfo->op2_reclaimable)  /* Always 0 if op2 is NULL */
    free_network(vinfo->op2);

  /* ------------------------- */
  /* Free other storage.       */
  /* ------------------------- */
  free_heap(vinfo->btree_heap);
  if (vinfo->methods->reclaimable)
    free(vinfo->methods);
  free(vinfo);
  
  NET_virtual_info(net) = NULL;
}
#endif /* SMALL_FSM */
#endif /* SLIM_FST */

void (*free_included_table)(NETptr net) = NULL;


void free_network(NETptr net)
{
    if (net == NULL)
        return;

    if (NET_Kaplan_compressed (net))
    {
        handle_warning("Wrong file format: can't free the network...", "FREE_NETWORK", 0);

        return;
    }
    
    if (NET_shared_arc_lists (net))
      free_shared_arc_lists (net);

    if (NET_tabular(net))  /* table representation */
      {
	if (! free_included_table)     /* function not available */
	  fsm_fprintf (stderr, "*** ERROR: network contains a transition table\
which cannot be handled in this implementation.\n");

	else                            /* function available */
	  free_included_table (net);
      }
    else if (NET_compacted(net))
      {
        if (net->mmap_handle != NULL) /* The network has been memory-mapped */
		{
#if defined _WINDOWS || defined UNDER_CE
          if (UnmapViewOfFile((LPCVOID)net->mmap_handle) == 0)
          {
                  fsm_fprintf (stderr, "*** ERROR: unmap failure.\n");
          }
#else /* UNIX */
          if (munmap(net->mmap_handle, net->mmap_size) != 0)
          {
                  fsm_fprintf (stderr, "*** ERROR: unmap failure.\n");
          }
#endif /* _WINDOWS */

		}
		else /* no memory mapping */
		{
          free(NET_arc_block(net));
		} /* memory mapping */
	NET_arc_block(net) = NULL;
	free_alph(NET_recode_key(net));
      }
    else
      {
	STATEptr state, next_state;
	ARCptr arc, next_arc;
#ifndef SLIM_FST
#ifndef SMALL_FSM /* No virtual nets in the SMALL_FSM world! */
	if (NET_is_virtual(net))
	  free_virtual_network_storage(net);
#endif /* SMALL_FSM */
#endif /* SLIM_FST */

	for(state = NET_states(net); state != NULL; state = next_state)
	  {
	    next_state = next_state(state);
	    if (STATE_vector_p(state))
	      {
		free_arc_vector(STATE_arc_vector(state), net);
	      }
	    else if (!STATE_is_virtual(state))
	      {
		for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc)
		  {
		    next_arc = next_arc(arc);
		    free_arc(arc);
		  }
	      }
	    free_state(state);
	  }
      }
    free_net_and_periphery(net);
} /* free_network */


static void free_global_variables()
{
    /* Release memory pointed by LAST_HEADER and NEXT_HEADER: */
    strip_global_headers();

    /* Release UPPER_MATCH and LOWER_MATCH: */
    free_global_match_tables();

    /* Free property global variables: */

    /* Free the hash table: */
    free_hash_table(COMMON_FAT_STRINGS);
      
    /* Free the fat strings: */
    free(AUTHOR_ATTR);
    free(CHARENCODING_ATTR);
    free(CHARNAMES_ATTR);
    free(COMPACTED_ATTR);
    free(COMPOUNDCHAR_ATTR);
    free(COPYRIGHT_ATTR);
    free(DATE_ATTR);
    free(DIACRITICS_ATTR);
    free(EQV_VECTOR_ATTR);
    free(FUNCTION_ATTR);
    free(LANGUAGE_ATTR);
    free(LITERALNAMES_ATTR);
    free(NETWORKNAME_ATTR);
    free(NEUTRALLINKINGCHAR_ATTR );
    free(OPERATION_ATTR);
    free(POSTFIXLINKINGCHAR_ATTR);
    free(PREFIXLINKINGCHAR_ATTR);
    free(SAVED_SIGMA_ATTR);
    free(SIGMA_ATTR);
    free(SOURCE_ATTR);
    free(STATUS_ATTR);
    free(TAGS_ATTR);
    free(TOOLVERSIONS_ATTR);
    free(TRANSCRIPT_ATTR);
    free(TYPE_ATTR);
    free(VECTORIZE_N_ATTR);
    free(VERSIONSTRING_ATTR);
    free(WORD_TO_FILEPOS_MAP_ATTR);
    free(EPSILON_NAME);
    free(OTHER_NAME);
    free(NIL_NAME);
    free(T_NAME);
    free(FAT_OTHER);
    free(IL_PACKAGE);
    free(LISP_PACKAGE); 
} /* free_global_variables */

void release_fsm_memory(void)
{
  int i;

  /* free the heaps: */
  strip_networks_in_NET_HEAP();

  free_global_variables();

  /* Free temporary buffers: */
  free_UTF8fat_string_memory();
  free_fat_string_memory();

  /* Free the LABEL_MAP: */
  free_label_table();

  free_all_heaps();
  
  NET_HEAP = NULL;
  ALPH_HEAP = NULL;
  STATE_HEAP = NULL;
  ARC_HEAP = NULL;
  CH_VECTOR_HEAP = NULL;
  CH_NODE_HEAP = NULL;
  
  for (i = 0; i <= MAX_LV; i++)
    LV_HEAPS[i] = NULL;

  /* Anticipate possible new calls to InitFsmMemory (without leaving the 
     application) by re-initializing the global variable NOT_INITIALIZED_P: */
  NOT_INITIALIZED_P = TRUE;
  
} /* release_fsm_memory */



/**********************
 *                    *
 *  ALPHABET VECTORS  *
 *                    *
 **********************/
#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
ALPH_VECTORptr make_alph_vector(int len)
{
  ALPH_VECTORptr av =
    (ALPH_VECTORptr) malloc((size_t) sizeof(ALPH_VECTORtype));
  ALPHABETptr *alphs = 
    (ALPHABETptr *) malloc((size_t) len * (size_t) sizeof(ALPHABETptr));
  
  if (av == NULL || alphs == NULL)
    not_enough_memory("MAKE_ALPH_VECTOR");
  
  av_alphs(av) = alphs;
  av_len(av) = len;
  next_av(av) = (ALPH_VECTORptr) NULL;
  
  return(av);  
}

void free_alph_vector(ALPH_VECTORptr av)
{
  int i;
  
  if (av == NULL)
    return;
  
  for (i = 0; i < av_len(av); i++)
    free_alph(av_alphs(av)[i]);
  
  free(av_alphs(av));
  free(av);
}
#endif /* SLIM_FST */

/*****************
 *               *
 *  NET VECTORS  *
 *               *
 *****************/

NVptr make_nv(int len)
     /*****
      *
      * Create a NET_VECTOR of length LEN.  
      * It is caller's responsibility to ensure that vector is properly filled
      * with LEN nets.
      *
      *****/
{
  NVptr new;
  
  new = (NVptr) calloc((size_t) 1, (size_t) sizeof(NVtype));
  if (new == NULL)
    not_enough_memory("MAKE_NV");
  
  NV_len(new) = len;
  if (len > 0)
    {
      NV_nets(new) = (NETptr *) calloc((size_t) len, (size_t) sizeof(NETptr));
      if (NV_nets(new) == NULL)
	not_enough_memory("MAKE_NV");
    }
  else 
    NV_nets(new) = NULL;

  return (new);
} /* make_nv */

void expand_nv(NVptr nv, int num)
     /*****
      *
      * Expand NV by NUM cells.  The new cells are placed at the end of
      * the vector.
      * It is caller's responsibility to ensure that vector is filled with
      * NUM additional nets.
      *
      *****/
{
  int i, new_len;
  
  if (!(num > 0))
    return;

  new_len = NV_len(nv) + num;

  if (NV_len(nv) == 0)
    NV_nets(nv) = (NETptr *) calloc((size_t) new_len, (size_t) sizeof(NETptr));
  else
    NV_nets(nv) =
      (NETptr *) realloc(NV_nets(nv), (size_t) new_len * sizeof(NETptr));

  if (NV_nets(nv) == NULL)
    not_enough_memory("EXPAND_NV");

  for (i = NV_len(nv); i < new_len; i++)
    NV_net(nv, i) = NULL;
  
  NV_len(nv) = new_len;
}


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
NVptr net2nv(NETptr net)
     /*****
      *
      * Wrap a NET_VECTOR around a net.  Return the new vector.
      *
      *****/
{
  NVptr new;
  
  new = make_nv(1);
  NV_net(new, 0) = net;
  return(new);
}

void nv_push(NETptr net, NVptr nv)
     /*****
      *
      * Push NET onto the beginning of NV.
      *
      *****/
{
  int i;
  
  expand_nv(nv, 1);
  
  for (i = NV_len(nv) - 1; i > 0; i--)
    NV_net(nv, i) = NV_net(nv, i - 1);
  
  NV_net(nv, 0) = net;
}

void nv_add(NETptr net, NVptr nv)
     /*****
      *
      * Add NET onto the end of NV.
      *
      *****/
{
  expand_nv(nv, 1);     
  NV_net(nv, NV_len(nv) - 1) = net;
}

NVptr nv_insert (NETptr net, NVptr nv, int pos)
     /*****
      *
      *  Splices NET into NETS at POS.  Shifts nets above POS up one slot.
      *  Allows splicing at the end.
      *
      *****/
{
  int i;
  
  if (pos < 0 || pos > NV_len(nv))
    handle_error("POS out of range", "nv_insert", pos);
  
  expand_nv(nv, 1);
  for (i = NV_len(nv) - 1; i > pos; i--)
    NV_net(nv, i) = NV_net(nv, i - 1);
  NV_net(nv, pos) = net;
  
  return(nv);
}
#endif /* SLIM_FST */


NETptr nv_get(NVptr nv, int pos)
     /*****
      * 
      *  Returns the net at position POS.  NETS remains unchanged.
      *  Returns NULL if POS is out of range.  
      *  Safer than using NV_net macro directly.
      *
      *****/
{
  if ( (!nv) ||
    (pos < 0 || pos >= NV_len(nv)))
    {
      fsm_fprintf(stderr, "*** No net at position %d\n", pos);
      return (NULL);
    }
  else
    return (NV_net(nv, pos));
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

NETptr nv_extract(NVptr nv, int pos)
     /*****
      *
      * Returns the net at position POS, shifts nets at higher positions down.
      * Error if POS is out of range.
      *
      *****/
{
  NETptr net;
  int16 i;
  
  if (pos < 0 || pos >= NV_len(nv))
    handle_error("POS out of range", "nv_extract", pos);
  
  net = NV_net(nv, pos);
  NV_len(nv)--;
  for (i = pos; i < NV_len(nv); i++)
    NV_net(nv, i) = NV_net(nv, i+1);
  
  NV_net(nv, i) = NULL;         /* clear last cell */
  
  return (net);
}

NVptr nv_append(NVptr nv1, NVptr nv2)
     /*****
      *
      * If one of the net vectors is NULL, return the other.
      * Otherwise append nv2 to the end of nv1.  Release nv2.
      *
      *****/
{
  int i, j, new_len;
  
  if (nv1 == NULL)
    return(nv2);
  
  if (nv2 == NULL)
    return(nv1);

  if (!(NV_len(nv2) > 0))
    {
      free_nv_only(nv2);
      return(nv1);
    }

  new_len = NV_len(nv1) + NV_len(nv2);

  if (NV_len(nv1) == 0)
    NV_nets(nv1) =
      (NETptr *) calloc((size_t) new_len, (size_t) sizeof(NETptr));
  else
    NV_nets(nv1) =
      (NETptr *) realloc(NV_nets(nv1), (size_t) new_len * sizeof(NETptr));

  if (NV_nets(nv1) == NULL)
    not_enough_memory("NV_APPEND");
  for (i = NV_len(nv1), j = 0; i < new_len; i++, j++)
    NV_net(nv1, i) = NV_net(nv2, j);
  
  NV_len(nv1) = new_len;
  
  free_nv_only(nv2);
  return(nv1);
}
#endif /* SLIM_FST */

NVptr nv_reverse(NVptr nv)
{
  /* Reverses the order of nets in a net vector. */

  if (nv && NV_len(nv) >= 2)
    {
      int i, n = NV_len(nv) / 2, last = NV_len(nv) - 1;
      
      for (i = 0; i < n; i++)
        {
          NETptr net = nv_get(nv, i);
          NV_net(nv, i) = nv_get(nv, last - i);
          NV_net(nv, last - i) = net;
        }
    }

  return(nv);
}


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

NVptr copy_nv_only(NVptr old)
     /*****
      *
      * Copies the NV wrapper (& pointers).  Does not copy the 
      * networks.
      *
      *****/
{
  NVptr new;
  int i;
  
  new = make_nv(NV_len(old));
  
  for (i = 0; i < NV_len(new); i++)
    NV_net(new, i) = NV_net(old, i);
  
  return (new);
}
#endif /* SLIM_FST */
/* NVptr copy_nv_and_nets(NVptr old) */
/*****
 *
 * Copies both the NV wrapper and the networks it
 * points to.
 *  NOTE:  DEFINED IN FSM_BASICS.C BECAUSE
 *         IT MUST CALL FUNCTIONS DEFINED THERE.
 *
 *****/


void free_nv_only(NVptr nv)
     /*****
      *
      * Frees NV wrapper. Does not free the nets it contains.
      *
      *****/
{
  if (nv != NULL)
    {
      if (NV_nets(nv) != NULL)
        {
	  free((void *) NV_nets(nv));
	  NV_nets(nv) = NULL;
	}
      free((void *) nv);
    }
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void free_nets_only(NVptr nv)
     /*****
      *
      * Frees nets preserving the wrapper.
      *
      *****/
{
  int i;
  
  if (nv != NULL)
    {
      for (i = 0; i < NV_len(nv); i++)
	{
	  free_network(NV_net(nv, i));
	  NV_net(nv, i) = NULL;
	}
    }
}
#endif /* SLIM_FST */

void free_nv_and_nets(NVptr nv)
     /*****
      *
      *  Frees NV and the nets it contains.
      *  
      *
      *****/
{
  int i;
  
  if (nv != NULL)
    {
      for (i = 0; i < NV_len(nv); i++)
        {
	  free_network(NV_net(nv, i));
	  NV_net(nv, i) = NULL;
	}
      free_nv_only(nv);
    }
}


static int name_cmp(register LABELptr lab1, register LABELptr lab2)
{
  /* changed 31 Mar 92 to fat_strcmp_lab from fat_strcmp by KRB */
  /* with fat_strcmp_lab, alphabetic always lower than non-alphabetic */

 register int cmp = fat_strcmp_lab(LABEL_atom(lab1),
				   LABEL_atom(lab2) );
 if (cmp == 0)
   return(0);
 else if (cmp < 0)
   return(-1);
 else
   return(1);
}/* name_cmp */

int alph_order(register id_type id1, register id_type id2)
{
  register LABELptr lab1 = id_to_label(id1), lab2 = id_to_label(id2);
      
  if (LABEL_arity(lab1) == 1)
    {
      if (LABEL_arity(lab2) == 1)
	return(name_cmp(lab1, lab2));
      else
	return(-1);		/* single labels precede tuples */
    }
  else if (LABEL_arity(lab2) == 1)
    return(1);			/* single labels precede tuples */
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
} /* alph_order */


/* used to sort the labels of a net into cannonical order for display
 * of the compiled rule as a state-transition grid
 */

int default_label_order(id_type id1, id_type id2)
{
  /* Returns -1 if ID1 precedes ID2, 0 if they are the same, otherwise 1.
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
  else if (id1 == OTHER)
    return(-1);
  else if (id2 == OTHER)
    return(1);
  else if (id1 == ALTCHAIN_SYMBOL)
    return(1);
  else if (id2 == ALTCHAIN_SYMBOL)
    return(-1);
  else
    return (alph_order(id1, id2));
}

int default_alph_order(const void *id1, const void *id2)
{
  /* like strcmp, ret 0 if equal, - if id1 < id2, + if id1 > id2 */
  return(default_label_order(*(id_type *)id1, *(id_type *)id2));
}


void sort_alph(ALPHABETptr alph)
{
  binary_to_label(alph);
  QSORT((void *) ALPH_items(alph), (size_t) ALPH_len(alph),
        (size_t) sizeof(id_type), default_alph_order);
} /* sort_alph */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

ALPHABETptr make_identity_map(NETptr net)
{
  /* Makes a dummy recoding key that maps every label in the
     net's label inventory to itself except that the two
     positions beyond the largest label are mapped to
     EPSILON and OTHER, respectively.
     */


  id_type *items = ALPH_items(NET_labels(net));
  int len = ALPH_len(NET_labels(net));
  id_type i; 
  id_type max_id = max_label_item(items, len);
  ALPHABETptr identity_map = make_alph(max_id + 3, LABEL_VECTOR);

  for (i = 0; i <= max_id; i++)
    ALPH_item(identity_map, i) = i;

  ALPH_item(identity_map, max_id + 1) = EPSILON;
  ALPH_item(identity_map, max_id + 2) = OTHER;

  return(identity_map);
}          


/************************
 *                      *
 *  NET-VECTOR VECTORS  *
 *                      *
 ************************/

NVVptr make_nvv(int len)
     /*****
      *
      * Create a NET_VECTOR_VECTOR of length LEN.  
      * It is caller's responsibility to ensure that NVV is properly filled
      * with LEN net vectors.
      *
      *****/
{
  NVVptr new;  int i;
  
  new = (NVVptr) calloc((size_t) 1, (size_t) sizeof(NVVtype));
  if (new == NULL)
    not_enough_memory("MAKE_NVV");
  
  NVV_len(new) = len;
  if (len > 0)
    {
      NVV_nvs(new) = (NVptr *) calloc((size_t) len, (size_t) sizeof(NVptr));

      if (NVV_nvs(new) == NULL)
	not_enough_memory("MAKE_NVV");
      else
	for (i=0; i< NVV_len(new); i++)  NVV_nv(new, i) = NULL;
    }
  else 
    NVV_nvs(new) = NULL;

  return (new);
}

NVVptr append_nvv(NVVptr nvv1, NVVptr nvv2)
     /*****
      *
      * If one of the net-vector vectors is NULL, return the other.
      * Otherwise append nvv2 to the end of nvv1.  Release nvv2.
      *
      *****/
{
  int i, j, new_len;
  
  if (nvv1 == NULL)
    return(nvv2);
  
  if (nvv2 == NULL)
    return(nvv1);

  if (!(NV_len(nvv2) > 0))
    {
      free_nvv_only(nvv2);
      return(nvv1);
    }

  new_len = NVV_len(nvv1) + NVV_len(nvv2);

  if (NVV_len(nvv1) == 0)
    NVV_nvs(nvv1) =
      (NVptr *) calloc((size_t) new_len, (size_t) sizeof(NVptr));
  else
    NVV_nvs(nvv1) =
      (NVptr *) realloc(NVV_nvs(nvv1), (size_t) new_len * sizeof(NVptr));

  if (NVV_nvs(nvv1) == NULL)
    not_enough_memory("NVV_APPEND");
  for (i = NVV_len(nvv1), j = 0; i < new_len; i++, j++)
    NVV_nv(nvv1, i) = NVV_nv(nvv2, j);
  
  NVV_len(nvv1) = new_len;
  
  free_nvv_only(nvv2);
  return(nvv1);
}

NVVptr expand_nvv(NVVptr nvv, int num)
     /*****
      *
      * Expand NVV by NUM cells.  The new cells are placed at the end.
      * It is caller's responsibility to ensure that NVV is filled with
      * NUM additional NVs.
      *
      *****/
{
  return (append_nvv (nvv, make_nvv (num)));
}

NVVptr nv_to_nvv(NVptr nv)
     /*****
      *
      * Wrap a NET_VECTOR_VECTOR around a NET_VECTOR.  Return the NVV.
      *
      *****/
{
  NVVptr new;
  
  new = make_nvv(1);
  NVV_nv(new, 0) = nv;
  return(new);
}

void free_nvv_only(NVVptr nvv)
     /*****
      *
      * Frees NVV wrapper. Does not free the nvs it contains.
      *
      *****/
{
  if (nvv != NULL)
    {
      if (NVV_len(nvv) > 0)
        {
	  free((void *) NVV_nvs(nvv));
	  NVV_nvs(nvv) = NULL;
	}
      free((void *) nvv);
    }
}

void free_nvv_and_nvs_only(NVVptr nvv)
     /*****
      *
      *  Frees NVV and the nvs it contains.
      *  Does not free the nets they contain.
      *
      *****/
{
  int i;
  
  if (nvv != NULL)
    {
      for (i = 0; i < NVV_len(nvv); i++)
        {
	  free_nv_only( NVV_nv(nvv, i));
	  NVV_nv(nvv, i) = NULL;
	}
      free_nvv_only(nvv);
    }
}

void free_nvv_and_nvs_and_nets(NVVptr nvv)
     /*****
      *
      *  Frees NVV, the nvs it contains and their networks.
      *
      *****/
{
  int i;
  
  if (nvv != NULL)
    {
      for (i = 0; i < NVV_len(nvv); i++)
        {
	  free_nv_and_nets( NVV_nv(nvv, i));
	  NVV_nv(nvv, i) = NULL;
	}
      free_nvv_only(nvv);
    }
}


/****************************
 *                          *
 *  REPLACEMENT STRUCTURES  *
 *                          *
 ***************************/

/* A replacement structure contains all arguments for 
   parallel replacement.
   */

RSptr make_rs(int length, int arrow, int ctxdir)
{
  /* Makes and initializes a replacement structure */

  RSptr rs;

  rs = (RSptr) malloc (sizeof (RStype));
  RS_nv(rs) = make_nv(length);
  RS_arrow(rs) = arrow;
  RS_ctxdir(rs) = ctxdir;

  return(rs);
}

void free_rs(RSptr rs)
{
  /* Frees a replacement structure, the net vektor the nets.
     If nets should not be freed:        Set  RS_len = 0;
     If net vektor should not be freed:  Set  RS_nv = NULL;
     */

  if (RS_nv(rs))  free_nv_and_nets(RS_nv(rs));
  free(rs);
}

RSptr empty_rs()
{
  /* Returns a replacement structure with a net vektor of length 0
     and no information about arrow and context direction.
     */

  return( make_rs(0, -1, -1) );
}

int rs_compair_and_copy_int(int *n1, int *n2)
{
  /* Compairs n1 and n2 if not negative and returns TRUE or FALSE.
     If n1 is negative but not n2 then n2 is copied to n1.
     */

  if (*n2 <0)  return(TRUE);
  if (*n1 <0)  { *n1 = *n2;  return(TRUE); }
  if (*n1 == *n2)  return(TRUE);

  return(FALSE); /* n1 and n2 are different and both not negative */
}

void rs_handle_int_comparison_error
            (char *operation, char *xerror, int n1, int n2)
{
  fsm_fprintf(stderr, "***** ERROR in %s\n", operation);
  fsm_fprintf(stderr, "***** %s : %d and %d\n", xerror, n1, n2);
}

RSptr rs_append(RSptr rs1, RSptr rs2)
{
  /* Checks the equality of 'arrow' and 'ctxdir' except they have 
     negativ (which means no) values.
     If equal: Appends rs2 to rs1. Returns rs1.
     If not equal: Frees rs1 and rs2. Returns NULL.
     */

  int error_p = FALSE;

  if (rs_compair_and_copy_int( &(RS_arrow(rs1)), &(RS_arrow(rs2)) ) 
      == FALSE)
    {
      rs_handle_int_comparison_error
	("APPEND REPLACEMENT STRUCTURE", "different arrows",
	 RS_arrow(rs1), RS_arrow(rs2));
      error_p = TRUE;
    }

  if (rs_compair_and_copy_int( &(RS_ctxdir(rs1)), &(RS_ctxdir(rs2)) )
      == FALSE)
    {
      rs_handle_int_comparison_error
	("APPEND REPLACEMENT STRUCTURE", "different context directions",
	 RS_ctxdir(rs1), RS_ctxdir(rs2));
      error_p = TRUE;
    }

  if (error_p == TRUE)
    { free_rs(rs1);  free_rs(rs2);  return(empty_rs()); }

  nv_append( RS_nv(rs1), RS_nv(rs2));
  return(rs1);
}


/******************************************/

int init_string_functions(int char_encoding)
{
  switch(char_encoding)
    {
    case CHAR_ENC_UTF_8:
		set_io_char_encoding(get_default_context(),CHAR_ENC_UTF_8);
		break;
    case CHAR_ENC_ISO_8859_1:
		set_io_char_encoding(get_default_context(),CHAR_ENC_ISO_8859_1);
		break;
    default:
      /* Illegal code */
      return(1);
      break;
    }
  return(0);
}

char *decode_char_encoding(int char_encoding)
{
  switch(char_encoding)
    {
    case CHAR_ENC_UTF_8:
      return("UTF-8");
      break;
    case CHAR_ENC_ISO_8859_1:
      return("ISO-8859-1");
      break;
    default:
      return("Unknown character encoding");
      break;
    }
}


#endif /* SLIM_FST */
/******************************************/


