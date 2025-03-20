/* Copyright (c) 1992 by the Xerox Corporation.  All rights reserved */


/******************************************************
 **
 **  lexc-aux.c
 **
 **  Todd Yampol, Xerox PARC, 12/92.
 **  Functions for parsing two-level lexicons.
 **
 **  Imported to (x)fst at Ken Beesley's request from
 **  lexc, where the file is called lex-aux.c
 **  2002-03-19  Lauri Karttunen
 ******************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "fst/error.h"
#include "fst/struct.h"
#include "fst/modify.h"
#include "fst/calculus.h"
#include "fst/pars-tbl.h"
#include "fst/text-io.h"
#include "fst/bin-in.h"
#include "fst/pretty-p.h"
#include "fst/lab-vect.h"
#include "fst/lexicon-aux.h"
#include "fst/flags.h"
#include "fst/regex-in.h"
#include "fst/cfsm.h"
#include "fst/utf8_fatstr_util.h"
/* #include "fst/interface.h" */
#include "fst/fsm-fst-x.h"
#include "fst/pars-tbl.h"

int PARSE_ERRORS_P;
int PARSE_ERROR_GIVEN = FALSE;
int INTERACTIVE_P = FALSE;
extern int DEBUG_P;
PARSE_TBL LEXICON_PARSE_TABLE = NULL;
extern FILE *lexicon_in;
ALPHABETptr FILE_ALPH = NULL;
static int INFO_STRINGS_SEEN = FALSE;
static long ENTRY_COUNT = 0;

static STATEptr *START_DESTS = NULL, FINAL_STATE = NULL;
static NETptr CUR_NET = NULL, CUR_REGEX = NULL;
static int SD_LEN = 0;
long LINE_COUNT = 0, ERROR_COUNT = 0;

NVptr LEX_ARRAY = NULL;
int LEX_COUNT = 0, FIRST_ENTRY;
NETptr FINAL_LEXICON = NULL;

id_type UPPER_SEQ[WORD_STRING_SIZE + 1];
id_type LOWER_SEQ[WORD_STRING_SIZE + 1];
ALPHABETptr GLOBAL_SIGMA;

extern long int LEXICON_LINE_NUM;

/* Properties to add after having loaded the lexicon */
PROPptr LexProps = NULL;

static void fprint_sublex_name(FILE *stream, FAT_STR sublex)
     /*
       print SUBLEX, stripping initial "<<<" and final ">>>"
       */
{
  int end_pos = fat_strlen(sublex) - 3, 
  temp = CHARNUM(sublex+end_pos);
  
  CHARNUM(sublex+end_pos) = '\0';
  if (stream == NULL)
    fat_string_out(sublex + 3);
  else
    FST_fprint_fat_string(stream, sublex + 3);
  CHARNUM(sublex+end_pos) = temp;
  
}

static void user_warning(char *warning, FAT_STR sublex, int quit_on_fail_p)
{
  fprintf(stderr, "\n*** Warning:  %s", warning);
  if (sublex != NULL)
    {
      fprintf(stderr, " (");
      fprint_sublex_name(stderr, sublex);
      fprintf(stderr,")\n");
    }
  else
    fprintf(stderr, "\n");
  if (quit_on_fail_p)
    quit_on_fail_error(quit_on_fail_p);
}

/*  Strings */

static char *STRING, *STRING_BLOCK = NULL;
static int STRING_POS = STRING_BLOCK_SIZE + 1;

void reset_string_mem(void)
{
  if (STRING == NULL)
    {
      STRING_BLOCK = (char *)
      	malloc((size_t) STRING_BLOCK_SIZE * (size_t) sizeof(char));
      if (STRING_BLOCK == NULL)
        not_enough_memory("RESET_STRING_MEM");
    }
  STRING = STRING_BLOCK;
  STRING_POS = 0;
}

void free_string_mem(void)
{
  free((void *) STRING_BLOCK);
  STRING_BLOCK = NULL;
  STRING = NULL;
  STRING_POS = STRING_BLOCK_SIZE + 1;
}

static char *alloc_string(int len)
{
  /* Space for strings is allocated from the buffer STRING_BLOCK
     that is of length STRING_BLOCK_SIZE. When all the space has
     been used, we start allocating from the beginning again.
     This exploits the fact that strings in the parsing routine
     are ephemeral. Only a few strings are in use at any given
     time.
  	 
     Trying to allocate space for a string that is longer than
     STRING_BLOCK_SIZE causes an error.
     */
  char *str = STRING;
  
  STRING_POS += len;
  
  if (STRING_POS > STRING_BLOCK_SIZE)
    {
      if (len > STRING_BLOCK_SIZE)
	handle_error("String size too long", "ALLOC_STRING", len);
      else
	{
	  reset_string_mem();
	  str = STRING;
	  STRING_POS = len;
	}
    }
  STRING = STRING + len;
  return(str);
}

char *new_str(char *str)
{
  return(strcpy(alloc_string(strlen(str) +1), str));
}

char *rm_escapes(char *str)
{
  char *save = str;
  char *dst = NULL;
  
  if (str != NULL)
    {
      while (*str != '\0')
        {
          if (*str == '%' && *(str+1) != '\0')
            {
              /* No % found yet, start copy */
              if (dst == NULL)
                  dst = str;
              str++;
            }
          if (dst != NULL)
            {
              *dst = *str;
              dst++;
            }
          str++;
        }
      if (dst != NULL)
          *dst = '\0';
    }
#if 0
  for (str = strchr(str,'%'); 
       str != NULL; 
       str = strchr(str, '%'))
    { 
      strcpy(str, str + 1); 
      str++; 
    }
#endif
  return (save);
}


char *rm_mult_escapes(char *str)
{
  char *end = str + (strlen(str) - 1);
  
  if (*str == '"' && *end == '"')
    {
      *end = '\0';
      return(str + 1);
    }
  else
   return(str);
}

void report_progress(char *phase)
{
  string_out(phase);
  string_out("...");
  buffer_out(72, 0);
  fflush(stdout);
}

void lex_add_prop(FAT_STR attr, OBJECTptr value)
{
    PROPptr prop, last_prop = NULL;
  
    for (prop = LexProps; prop != NULL; prop = next_prop(last_prop))
    {
	if (fat_strcmp(PROP_attr(prop), attr) == 0)
        {
	    PROP_val(prop) = value;
	    return;
        }
	last_prop = prop;
    }
  
    prop = (PROPptr)calloc(1, (size_t)sizeof (PROPtype));
  
    if (prop == NULL)
	not_enough_memory("LEX_ADD_PROP");
  
    PROP_attr(prop) = attr;
    PROP_val(prop) = value;

    if (last_prop == NULL)
	LexProps = prop;
    else
	next_prop(last_prop) = prop;
}
   
static ALPHABETptr labs_to_alph(void)
{
  id_type len = LAB_VECTOR[0];
  ALPHABETptr alph = make_alph(len, LABEL_VECTOR);
  
  memcpy((void *) ALPH_items(alph), (void *) (LAB_VECTOR + 1),
         (size_t) len * (size_t) sizeof(id_type));
  
  return(alph);
}

void end_alph(void)
     /***
      *
      * Create an alphabet out of the symbols 
      * that have been declared.  Later, this will
      * be unioned with other alphabets, and a parse
      * table will be created from the result.
      *
      ***/
{
  FILE_ALPH = labs_to_alph();
}

void begin_syms(void)
{
  int i;
  char ch[2];
  id_type sym_id;
  int lim = (IS_UTF8_MODE ? 128 : 256);

  init_lab_vector();
  /* install all single-char symbols */
  ch[1] = '\0';
  for (i = 1; i < lim; i++)
    {
      ch[0] = i;
      FST_intern_literal(&sym_id, ch);
      add_label(sym_id);
    }
}

void no_alph(void)
{
  FILE_ALPH = NULL;
}

static ALPHABETptr make_std_alph(void)
{
  ALPHABETptr std_alph;
  
  int i;
  char ch[2];
  id_type sym_id;
  int lim = (IS_UTF8_MODE ? 128 : 256);
  
  std_alph = make_alph(255, LABEL_VECTOR);

  ch[1] = '\0';
  /* Start add 1. We don't want EPSILON. */
  for (i = 1; i < lim; i++)
    {
      ch[0] = i;
      FST_intern_literal(&sym_id, ch);
      ALPH_item(std_alph, i-1) = sym_id;
    }
  
  return (std_alph);
}

static void dispose_lexicon_parse_table(void)
     /*****
      *
      * Free LEXICON_PARSE_TABLE and set it to
      * NULL. 
      *
      *****/
{
  free_parse_table(LEXICON_PARSE_TABLE);	
  LEXICON_PARSE_TABLE = NULL;
}


static void make_lexicon_parse_table()
     /*****
      *
      * The parse table for the lexicon is the union of:
      * 1.  The standard alph (all single-char symbols).
      * 2.  The MULTICHAR_SYMBOLS declared in the lexicon file.
      * 3.  The rule's upper alphabet.
      *
      ******/
{
  ALPHABETptr lexicon_alph;
  
  lexicon_alph = make_std_alph();
  
  if (FILE_ALPH)
    {
      lexicon_alph = sigma_union_dont_copy(lexicon_alph, FILE_ALPH);
      free_alph(FILE_ALPH);
      FILE_ALPH = NULL;
    }
  
  LEXICON_PARSE_TABLE = make_parse_table(NULL, lexicon_alph, UPPER);
  
  free_alph(lexicon_alph);
}

static NETptr init_sublexicon(FAT_STR name)
     /*****
      ** This creates a net for the sublexicon NAME
      ** if it doesn't exist already.  In either case
      ** the return value is the net.
      **
      ** When this is called, NAME must have already
      ** been interned as a label.
      **
      ******/
{
  NETptr net;
  STATEptr terminal_state;
  id_type id;
  LABELptr lab;

  name_to_atom_id(&id, name);
  if (id == ID_NO_SYMBOL) 
    {
      handle_error("Class name should already be a label", 
                   "init_sublexicon", 0);
      return(NULL);
    }
  else
    {
      lab = id_to_label(id);
      if (LABEL_data(lab) == NULL)
        {
          /* Create & initialize new sublexicon. */
          net = null_fsm();
          label_to_binary(NET_labels(net));
          
          /* Make terminal_state state and place in NET_client_cell */
          terminal_state = make_state(NULL, net);
          STATE_final(terminal_state) = TRUE;
          NET_client_cell(net) = (void *) terminal_state;
          
          /* Add network name */
	  
          add_string_prop(net, NETWORKNAME_ATTR, copy_fat_string(name));
          
          /* Address of NET gets stored in two places:
             1)  LEX_ARRAY (in declaration order)
             2)  LABEL_data of name in LABEL_TABLE (hashed by name)
             */
          
          /* resize LEX_ARRAY if necessary */
          if (LEX_COUNT == NV_len(LEX_ARRAY)) 
            expand_nv(LEX_ARRAY, 100);
          
          NV_net(LEX_ARRAY, LEX_COUNT) = net;
          LEX_COUNT++;
          LABEL_data(lab) = (void *) net;
        }
      else
	net = (NETptr) LABEL_data(lab);
    }
  return (net);
}

FAT_STR make_sublex_name(char *str)
     /*  Check to see if class name has already been
         created.  If so, return old instance.  If not,
         Add "<<<" and ">>>" to str, and intern new label
         with that name. */
{
  id_type id;
  char s[100];
  
  rm_escapes(str);
  
  strcpy(s, "<<<");
  strcat(s, str);
  strcat(s, ">>>");
  
  thin_name_to_id(&id, s);
  if (id == ID_NO_SYMBOL)
    {
      FST_intern_literal(&id, s);
      init_sublexicon(id_to_atom_name(id)); 
    }
  else if (LABEL_data(id_to_label(id)) == NULL)
    init_sublexicon(id_to_atom_name(id));

  return (id_to_atom_name(id));
}

static NETptr extract_lexicon()
{
  int i, warning_given = FALSE;
  id_type id;
  NETptr n, root_net = NULL, cont_net;
  STATEptr s, cont_state, last_state;
  ARCptr a /*, cont_arc */ ;
  LABELptr l;

  FAT_STR root1 = thin_to_fat("<<<ROOT>>>");
  FAT_STR root2 = thin_to_fat("<<<Root>>>");
  FAT_STR last_name = thin_to_fat("<<<#>>>"), net_name;
  
  if (!INTERACTIVE_P && *fst_IY_VERBOSE())
    {
      printf("Building lexicon...");
      fflush(stdout);
    }

  /* If OTHER is in the global sigma, we need to expand first. */

  if (sigma_member(GLOBAL_SIGMA, OTHER))
    for (i = 0; i < LEX_COUNT; i++)
      {
	n = NV_net(LEX_ARRAY, i);
	if (sigma_member(sigma(n), OTHER))
	  {
	    ALPHABETptr diff = sigma_difference(GLOBAL_SIGMA, NET_sigma(n));
	    
	    if (ALPH_len(diff) > 0)
	      expand_other(n, diff, DONT_COPY);

	    free_alph(diff);
	  }
      }

  /* PIECE THE SUBLEXICONS TOGETHER */
  for (i = 0; i < LEX_COUNT; i++)
    {
      n = NV_net(LEX_ARRAY, i);
      net_name = OBJ_str(get_value(n, NETWORKNAME_ATTR));
      for (s = NET_states(n); s != NULL; s = next_state(s))
        {
          for(a = STATE_arc_set(s); a != NULL; a = next_arc(a))
            {
	      l = id_to_label(ARC_label(a));

              if (LABEL_data(l))
		{ 
		  /* a points to a sublexicon */

		  cont_net = (NETptr) LABEL_data(l);
	      
		  /* Clear client cell to mark net as visited */
		  NET_client_cell(cont_net) = (void *) NULL;

                  STATE_final(ARC_destination(a)) = FALSE;
		  cont_state = NET_start_state(cont_net);
              
		  /* Check for undefined continuation class */
		  if (STATE_arc_set(cont_state) == NULL &&
		      STATE_final(cont_state) == FALSE)
		    {
		      user_warning("Sublexicon is mentioned but not defined.",
				   id_to_atom_name(ARC_label(a)), TRUE);
		      /* Make this cont_state final so the result
			 is at least well-formed and so the error message
			 isn't produced twice for the same sublexicon.
		      */
		      STATE_final(cont_state) = TRUE;
		    }

		  /* Replace original destination with the start state
		     of the continuation lexicon.
		  */
		  ARC_destination(a) = cont_state;
		  ARC_label(a) = EPSILON;
		  STATE_deterministic(s) = FALSE;
                }
            }
        }
      NET_deterministic(n) = FALSE;
      NET_minimized(n) = FALSE;
      NET_pruned(n) = FALSE;
      NET_epsilon_free(n) = FALSE;
      NET_loop_free(n) = FALSE;
    }
  
  /* find the root net */
  for (i = 0; i < LEX_COUNT; i++)
    {
      n = NV_net(LEX_ARRAY, i);
      net_name = OBJ_str(get_value(n, NETWORKNAME_ATTR));
      
      if (fat_strcmp(net_name, root1) == 0 ||
          fat_strcmp(net_name, root2) == 0)
        {
          root_net = n;
          break;
        }
    }
  /* if no net is named "Root", net 1 is root */
  /* (net 0 is always the final lexicon "#") */
  if ((root_net == NULL) && (LEX_COUNT > 0))
    root_net = NV_net(LEX_ARRAY, 1);
  if (root_net == NULL)
    {
      user_warning("No sublexicons defined!", NULL, TRUE);
      /* clean up and exit */
      free(root1);
      free(root2); 
      free(last_name);
      return(NULL);
    }
  /* mark root net as visited */ 
  NET_client_cell(root_net) = (void *) NULL;
  
  /* merge other nets into root */
  last_state = NULL;
  for (i = 0; i < LEX_COUNT; i++)
    {
      n = NV_net(LEX_ARRAY, i);
      if (NET_arc_label_arity(n) > NET_arc_label_arity(root_net))
          NET_arc_label_arity(root_net) = NET_arc_label_arity(n);
      if (n != root_net)
        {
          NET_labels(root_net) = 
            sigma_union_dont_copy(NET_labels(root_net), NET_labels(n));
          
          /* maintain arc & state count */
          NET_num_arcs(root_net) += NET_num_arcs(n);
          NET_num_states(root_net) += NET_num_states(n);
          
          /* set last_state -- first time only */
          if (last_state == NULL)
            if (NET_states(root_net) != NULL)
              for (last_state = NET_states(root_net);
                   next_state(last_state) != NULL;
                   last_state = next_state(last_state));
          
          /* splice state lists */
          if (last_state == NULL)
            NET_states(root_net) = NET_states(n);
          else
            next_state(last_state) = NET_states(n);
          
          /* find new last_state */
          if (NET_states(n) != NULL)
            for (last_state = NET_states(n);
                 next_state(last_state) != NULL;
                 last_state = next_state(last_state));
        }
    }
  
  /* clean up */
  for (i = 0; i < LEX_COUNT; i++)
    {
      n = NV_net(LEX_ARRAY, i);
      
      /* remove pointer to N from LABEL_data */
      net_name = OBJ_str(get_value(n, NETWORKNAME_ATTR));
      name_to_atom_id(&id, net_name);
      l = id_to_label(id);
      LABEL_data(l) = NULL;       
      
      /* watch for sublexicons defined but not mentioned */
      if (NET_client_cell(n) != NULL)
	{  
	  NET_client_cell(n) = NULL;	  
	  if (warning_given == FALSE)
	    {
	      fresh_line(72, 0);
	      string_out("*** Warning:  Sublexicons defined but not used:");
	      fresh_line_s(78,0,stderr);
	      string_out("      ");
	      warning_given = TRUE;
	    }
	  fprint_sublex_name(NULL, net_name);
	  buffer_out_s(76, 6, stderr);
	}
      
      if (n != root_net)
        {         
          /* Release n & its alphabets.  
             We don't call free_network() here because
             the arcs & states of n end up in the root.
             */
          free_alph(NET_labels(n));
          free_alph(NET_sigma(n));
          free_cell(n, NET_HEAP);
          NV_net(LEX_ARRAY, i) = NULL;
        }
    }
  if (warning_given == TRUE)
    fprintf(stderr, "\n");
  
  free(root1);
  free(root2); 
  free(last_name);

  /* clean up the root_net */
  NET_pruned(root_net) = FALSE;
  NET_minimized(root_net) = FALSE; 
  NET_deterministic(root_net) = FALSE;
  NET_epsilon_free(root_net) = FALSE;
  
  /* The prune call in the minimizer gets rid of 
     does another label check only if net is not fully
     connected and some arcs get eliminated. But that
     is not in general the case. To make sure that
     all the cont class arcs <<<SUBLEX>>> that are
     no longer present get eliminated from NET_labels
     we do a label check here are then create NET_sigma
     is correct at this point.  Create NET_sigma from
     NET_labels when the pruning has been done.
     */

  label_check(root_net);

  if (!INTERACTIVE_P && *fst_IY_VERBOSE())
    {
      printf("Minimizing...");
      fflush(stdout);
    }
  
  min_fsm(root_net, DONT_COPY);
  
  if (!INTERACTIVE_P && *fst_IY_VERBOSE())
    printf("Done!\n");

  free_alph(NET_sigma(root_net));
  NET_sigma(root_net) = label_sigma(root_net);

  /* Check for lost labels. */

  subtract_alph(GLOBAL_SIGMA, NET_sigma(root_net));

  if (ALPH_len(GLOBAL_SIGMA) > 0)
    {
      binary_to_label(GLOBAL_SIGMA);
      fprintf(stderr, "*** Warning: Lost all entries containing");
      fprintf(stderr, " the following symbols:\n    ");
      reset_out_buffer();
      alph_out_s(GLOBAL_SIGMA, 78, 4, stderr);
      putc('\n', stderr);
    }

  /* Add the properties we read as the beginning of the file */
  if (LexProps != NULL)
  {
      PROPptr last_prop, prop;

      for (prop = NET_properties(root_net), last_prop = NULL;
	   prop != NULL;
	   prop = next_prop(prop))
	  last_prop = prop;

      if (last_prop == NULL)
	  NET_properties(root_net) = LexProps;
      else
	  next_prop(last_prop) = LexProps;
  }
  check_for_flags(root_net);
  return (root_net);
}


void begin_lexicons(void)
{
  INFO_STRINGS_SEEN = FALSE;
  PARSE_ERROR_GIVEN = FALSE;
  LINE_COUNT = 0;
  FIRST_ENTRY = TRUE;
  INTERACTIVE_P = (lexicon_in == stdin && isatty(0));
  
  if (!LEX_ARRAY)
    {
      /* This is the first file. Initialization should only be done once.
      	 All files constituting a lexicon have the same parse table.
	  */
      make_lexicon_parse_table();
      clear_label_data();

      PARSE_ERRORS_P = FALSE;
      ERROR_COUNT = 0;

      /* Make an empty alphabet big enought to handle all
         currently known symbols but set the length to
         0 here. The final size will be the size of the
         largest symbol encountered.
         */

      GLOBAL_SIGMA = make_alph(global_alph_size(), BINARY_VECTOR);
      ALPH_len(GLOBAL_SIGMA) = 0;
      
      SD_LEN = 100;
      
      LEX_ARRAY = make_nv(100);
      
      START_DESTS =
	(STATEptr *) calloc ((size_t) SD_LEN, (size_t) sizeof(STATEptr));
      
      FINAL_LEXICON = init_sublexicon(make_sublex_name("#"));
      STATE_final(NET_start_state(FINAL_LEXICON)) = TRUE;
      
      if (!START_DESTS || !LEX_ARRAY || !FINAL_LEXICON)
	not_enough_memory("begin_lexicons");
    }
}

void end_lexicons(void)
{
  fresh_line(72, 0);
  
  /* Don't free the parse table until all
     lexicon files have been read. */
  /*	dispose_lexicon_parse_table(); */
}

static NETptr cleanup_only(int quit_on_fail_p)
{
  FAT_STR net_name;
  NETptr net;
  LABELptr l;
  int i;
  id_type id;
  char *plural = "";

  for (i = 0; i < LEX_COUNT; i++)
    {
      if ((net = NV_net(LEX_ARRAY, i)))
	{
	  /* remove pointer to NET from LABEL_data */
	  if ((net_name = OBJ_str(get_value(net, NETWORKNAME_ATTR))))
	    {
	      name_to_atom_id(&id, net_name);
	      l = id_to_label(id);
	      LABEL_data(l) = NULL;
	    }
	  free_network(net);
	}
    }

  if (ERROR_COUNT > 1)
    plural = "s";

  fprintf(stderr, "*** Compilation aborted because of %ld parse error%s.\n",
	  ERROR_COUNT, plural);

  quit_on_fail_error(quit_on_fail_p);

  return(null_fsm());
}

NETptr link_and_cleanup(int quit_on_fail_p)
{
  NETptr lex;

  if (PARSE_ERRORS_P)
    lex = cleanup_only(quit_on_fail_p);
  else
    lex = extract_lexicon();
  
  free(START_DESTS);
  START_DESTS = NULL;
  free_nv_only(LEX_ARRAY);
  LEX_ARRAY = NULL;
  LEX_COUNT = 0;
  free_alph(GLOBAL_SIGMA);
  GLOBAL_SIGMA = NULL;
  dispose_lexicon_parse_table();
  FINAL_LEXICON = NULL; /* gets freed in extract_lexicon() */
  
  return (lex);
  
}

void begin_lexicon(FAT_STR name)
{
  
  int i;
  
  if (!INTERACTIVE_P && *fst_IY_VERBOSE())
    {
      if (FIRST_ENTRY)
        FIRST_ENTRY = FALSE;
      else 
        {
          string_out(", ");
          exact_buffer_out_s(80, 0, stdout);
        }
      
      fprint_sublex_name(NULL, name);
      string_out("...");
      exact_buffer_out_s(68, 0, stdout);
      fflush(stdout);
    }

  /* set up variables for lexicon */
  for (i = 0; i < SD_LEN; i++)
    START_DESTS[i] = NULL;
  ENTRY_COUNT = 0;
  CUR_NET = init_sublexicon(name);
  CUR_REGEX = NULL;
  FINAL_STATE = (STATEptr) NET_client_cell(CUR_NET);
  
  if (STATE_arc_set(NET_start_state(CUR_NET)) != NULL)
    user_warning("Sublexicon is declared more than once.", name, TRUE);
}

static void careful_align_sigmas(NETptr net1, NETptr net2)
{
  /* We need to be careful to prevent temporary class
     labels from being substituted for OTHER. If OTHER
     is not present, expand_other() is a no-op.
     */

  expand_other(net1, GLOBAL_SIGMA, DONT_COPY);
  expand_other(net2, GLOBAL_SIGMA, DONT_COPY);

  /* If the net contains other, augment its sigma with the
     symbols of the other net to avoid another expansion
     in union_fsm().
     */

  if (sigma_member(NET_sigma(net1), OTHER))
    sigma_union_dont_copy(NET_sigma(net1), NET_sigma(net2));

  if (sigma_member(NET_sigma(net2), OTHER))
    sigma_union_dont_copy(NET_sigma(net2), NET_sigma(net1));
}


void end_lexicon(void)
{

  if (!INTERACTIVE_P && *fst_IY_VERBOSE())
    {
      int_out(ENTRY_COUNT);
      exact_buffer_out_s(78, 0, stdout);
      fflush(stdout);
    }

  if (PARSE_ERRORS_P)
      return;

  cleanup_seq_list_fsm(CUR_NET, START_DESTS, SD_LEN, FINAL_STATE);
  min_fsm(CUR_NET, DONT_COPY);
  if (CUR_REGEX)
    {
      /* Let's not depend on union-fsm preserving property lists
	 and client cells. It currently preserves the properties
	 but not the client cell.
	 L.K. 94-08-26
	 */
      NVtype nv;
      NETptr nets[2];

      void *client_cell = NET_client_cell(CUR_NET);
      PROPptr props = NET_properties(CUR_NET);

      NET_properties(CUR_NET) = NULL;
      NET_client_cell(CUR_NET) = NULL;

      nets[0] = CUR_NET;
      nets[1] = CUR_REGEX;
      NV_len(&nv) = 2;
      NV_nets(&nv) = nets;

      careful_align_sigmas(CUR_NET, CUR_REGEX);

      CUR_NET = union_fsm(&nv, DONT_COPY, DO_MINIMIZE);

      NET_properties(CUR_NET) = props;
      NET_client_cell(CUR_NET) = client_cell;

      CUR_REGEX = NULL;
    }
}

void info_warning(void)
{
  if (INFO_STRINGS_SEEN == FALSE)
    {
      INFO_STRINGS_SEEN = TRUE;
      user_warning("Ignoring info strings.", NULL, FALSE);
    }
}

static void add_class_id_to_input(FAT_STR class)
     /* 
      ** Adds the label id for CLASS to the end of INPUT_SEQ.
      */
{
  id_type id, *seq;
  
  name_to_atom_id(&id, class);
  if (id == ID_NO_SYMBOL)
    handle_error("CLASS has should already be interned as a label",
		 "add_class_to_input", 0);
  for (seq = INPUT_SEQ; *seq != ID_NO_SYMBOL; seq++);
  seq[0] = id;
  seq[1] = ID_NO_SYMBOL;
}

static void parse_error_message(char *word)
{
  fprintf(stderr, "\n*** Warning:  Cannot parse '%s'.", word);

  if (PARSE_ERROR_GIVEN == FALSE)
    {
      fprintf(stderr, 
	      "\nNOTE:  If you declare MULTICHAR_SYMBOLS instead of ALPHABET, all undeclared\n\
	             symbols are parsed as single characters.  If neither set is declared,\n\
	             then all symbols are parsed as single characters.\n");
      PARSE_ERROR_GIVEN = TRUE;
    }
}

static void maintain_sigma(ALPHABETptr sig, id_type *seq)
{
  while(*seq != ID_NO_SYMBOL)
    sigma_add_to(sig, *seq++);

}



void add_simple_entry(char *word, FAT_STR class)
{ 
  int err;
  
  if (ENTRY_COUNT % 10000 == 0 && ENTRY_COUNT > 0)
    {
      
      int_out(ENTRY_COUNT);
      string_out("...");
      exact_buffer_out_s(78,0,stderr);
      fflush(stderr);
    }
  
  err = FST_parse_string(word, LEXICON_PARSE_TABLE, DO_ZERO_TO_EPS,
    	                   DONT_IGNORE_WHITES, DO_ESCAPE,
                           DO_REPORT_ERROR);
  if (err)
    parse_error_message(word);
  else
    {
      ENTRY_COUNT++;
      maintain_sigma(GLOBAL_SIGMA, INPUT_SEQ);
      add_class_id_to_input(class);
      add_seq_to_net(INPUT_SEQ, CUR_NET, FINAL_STATE, &START_DESTS, &SD_LEN, -1);
    }
}

static void copy_seq(id_type *dest, id_type *source)
{
  while (*source != ID_NO_SYMBOL)
    *dest++ = *source++;
  *dest++ = ID_NO_SYMBOL;
}

static int parse_one_side(char *word, id_type *seq)
{
  int err;
  
  err = FST_parse_string(word, LEXICON_PARSE_TABLE, DO_ZERO_TO_EPS,
                     DONT_IGNORE_WHITES, DO_ESCAPE, DO_REPORT_ERROR);
  if (err)
    {
      parse_error_message(word);
      return (0);
    }
  else
    {
      copy_seq(seq, INPUT_SEQ);
      return(1);
    }
}

static void make_pair_seq()
{
  int input_pos = 0, upper_pos = 0, lower_pos = 0, upper, lower;
  id_type lab_id;
  
  while (!(UPPER_SEQ[upper_pos] == ID_NO_SYMBOL &&
	   LOWER_SEQ[lower_pos] == ID_NO_SYMBOL))
    {
      if (UPPER_SEQ[upper_pos] == ID_NO_SYMBOL)
        upper = EPSILON;
      else
        upper = UPPER_SEQ[upper_pos++];
      
      if (LOWER_SEQ[lower_pos] == ID_NO_SYMBOL)
        lower = EPSILON;
      else
        lower = LOWER_SEQ[lower_pos++];
      
      if (upper == lower)
      	INPUT_SEQ[input_pos++] = upper;
      else
	{
	  make_pair_label(&lab_id, upper, lower);
	  INPUT_SEQ[input_pos++] = lab_id;
	}
    }
  INPUT_SEQ[input_pos] = ID_NO_SYMBOL;
}

void add_pair_entry(char *word1, char *word2, FAT_STR class)
{
  if (ENTRY_COUNT % 10000 == 0 && ENTRY_COUNT > 0)
    {
      int_out(ENTRY_COUNT);
      string_out("...");
      exact_buffer_out_s(78,0,stdout);
      fflush(stdout);
    }
  
  if (parse_one_side(word1, UPPER_SEQ) &&
      parse_one_side(word2, LOWER_SEQ))
    {
      ENTRY_COUNT++;
      maintain_sigma(GLOBAL_SIGMA, UPPER_SEQ);
      maintain_sigma(GLOBAL_SIGMA, LOWER_SEQ);
      make_pair_seq();
      add_class_id_to_input(class);
      add_seq_to_net(INPUT_SEQ, CUR_NET, FINAL_STATE,
		     &START_DESTS, &SD_LEN, -1);
    }
}


void add_regex_entry(NETptr net, FAT_STR class)
/****
 *
 * Concat class onto end of NET, which was built from
 * a regex.  Union NET with previous regex's in current
 * sublex.  Store result as CUR_REGEX.
 *
 ****/
{

  id_type id;
  int other_present_p = sigma_member(NET_sigma(net), OTHER);
  
  name_to_atom_id(&id, class);

  if (ENTRY_COUNT % 10000 == 0 && ENTRY_COUNT > 0)
    {
      int_out(ENTRY_COUNT);
      string_out("...");
      exact_buffer_out_s(78,0,stdout);
      fflush(stderr);
    }

  /* Update sigma before the class label is concatenated. */

  sigma_union_dont_copy(GLOBAL_SIGMA, sigma(net));
      
  /* Avoid extending OTHER with the class label in this network. */

  if (other_present_p)
    sigma_add_to(NET_sigma(net), id);

  net = concat_label(net, id, DONT_COPY, DO_MINIMIZE);

  if (CUR_REGEX)
    {
      NVtype nv;
      NETptr nets[2];

      nets[0] = CUR_REGEX;
      nets[1] = net;
      NV_len(&nv) = 2;
      NV_nets(&nv) = nets;

      careful_align_sigmas(CUR_REGEX, net);

      CUR_REGEX = union_fsm(&nv, DONT_COPY, DO_MINIMIZE);
    }
  else
    CUR_REGEX = net;

  ENTRY_COUNT++;
}


void lexicon_error_cleanup(void)
{
  extern FILE *lexicon_in;
  
  /* end alph stuff */
  if (FILE_ALPH)
    free_alph(FILE_ALPH);
  FILE_ALPH = NULL;
  
  PARSE_ERRORS_P = TRUE;
  ERROR_COUNT++;
/*  dispose_lexicon_parse_table(); */
  fflush(stdin);
  fflush(lexicon_in);
}

/* Functions for creating nets from regular expressions */

NETptr lexc_regex(char *regex_string)
{
  NETptr net;
  id_type id;

  /* Replace the trailing > by ; */
  
  net = RE2N_net_from_regex_string(regex_string);

  if (!net)
    {
      FST_intern_literal(&id, "*** regex error ***");
      net = symbol_fsm(id);
      PARSE_ERRORS_P = TRUE;
      ERROR_COUNT++;

      printf("\nline %ld: Regular expression error: '%s'\n",
	     LEXICON_LINE_NUM, regex_string);
    }

  return(net);
}


