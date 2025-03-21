/* $Id: context.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1998 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "fst/types.h"
#include "fst/context.h"

#include "fst/label.h"
#include "fst/pars-tbl.h"
#include "fst/pretty-p.h"
#include "fst/utf8_fatstr_util.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#include "fst/error.h"

#endif /* SLIM_FST */


/******************************************************
 **
 **  context.c
 **
 **  Lauri Karttunen, XRCE January 1998
 **  Setting general c-fsm parameters
 **
 ******************************************************/

void* default_context_object;

static FST_CNTXTptr make_fst_context(void)
{
  FST_CNTXTptr fst_cntxt = (FST_CNTXTptr) malloc(sizeof(FST_CNTXT));

  if (!fst_cntxt)
    not_enough_memory("MAKE_FST_CONTEXT");

  return(fst_cntxt);    
} /* make_fst_context */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

FST_CNTXTptr copy_fst_context(FST_CNTXTptr fst_cntxt)
{
  FST_CNTXTptr new_cntxt = make_fst_context();
  
  memcpy((void *) new_cntxt, (void *) fst_cntxt, sizeof(FST_CNTXT));

  return(new_cntxt);
}

#endif /* SLIM_FST */


static STANDARD_HEADERptr make_default_header(void)
{
  STANDARD_HEADERptr header = malloc((size_t) sizeof(STANDARD_HEADER));

  if (!header)
    not_enough_memory("MAKE_DEFAULT_HEADER");

  return(header);
}/* make_default_header */


static char **make_default_attributes(int n)
{
  char **array = malloc((size_t) n * sizeof(char *));
  array[0] = "COPYRIGHT";
  array[1] = "AUTHORS";
  array[2] = "CHARENCODING";
  array[3] = "SOURCE";
  array[4] = "LANGUAGE";
  array[5] = "CREATION-DATE";
  array[6] = "VERSION";

  return(array);
} /* make_default_attributes */


FST_CNTXTptr make_default_context(void)
{
  FST_CNTXTptr fst_cntxt = make_fst_context();

  FST_mode(fst_cntxt) = STANDARD_MODE;

  FST_copyright_string(fst_cntxt) = "Xerox Corporation";

  FST_compose_strategy(fst_cntxt) = PAIRWISE;

  FST_copy_p(fst_cntxt) = DONT_COPY;
  FST_determinize_p(fst_cntxt) = TRUE;
  FST_interactive_p(fst_cntxt) = FALSE;
  FST_last_errors_p(fst_cntxt) = FALSE;
  FST_lex_errors_p(fst_cntxt) = FALSE;
  FST_minimize_p(fst_cntxt) = DO_MINIMIZE;
  FST_name_nets_p(fst_cntxt) = FALSE;
  FST_obey_flags_p(fst_cntxt) = DO_OBEY;
  FST_prune_p(fst_cntxt) = TRUE;
  FST_quit_on_fail_p(fst_cntxt) = FALSE;
  FST_reclaim_p(fst_cntxt) = DO_RECLAIM;
  FST_sq_final_strings_arcs(fst_cntxt) = TRUE;
  FST_sq_intern_strings_arcs(fst_cntxt) = TRUE;
  FST_sq_string_onelong(fst_cntxt) = FALSE;

  FST_unicode_p(fst_cntxt) = FALSE ;
  FST_verbose_p(fst_cntxt) = TRUE ;

  FST_embedded_command_p(fst_cntxt) = FALSE;

  FST_cur_pos(fst_cntxt) = 0 ;
  FST_indent(fst_cntxt) = 0 ;
  FST_line_pos(fst_cntxt) = 0 ;
  FST_output_buffer_size(fst_cntxt) = 10000;
  FST_output_buffer(fst_cntxt) = malloc((size_t) 10000 * sizeof(char));
  FST_right_margin(fst_cntxt) = 72 ;

  FST_max_path_index_pos(fst_cntxt) = 0;
  FST_path_index_pos(fst_cntxt) = 0;
  FST_path_index_vector(fst_cntxt) = NULL; 

  FST_ignore_white_space_p(fst_cntxt) = DONT_IGNORE_WHITES;
  FST_zero_to_epsilon_p(fst_cntxt) = DONT_ZERO_TO_EPS;
  FST_input_seq_size(fst_cntxt) = 10000;
  FST_input_seq(fst_cntxt) = malloc((size_t) 10000 * sizeof(id_type));
  FST_lower_match(fst_cntxt) = NULL;
  FST_match_table(fst_cntxt) = NULL;
  FST_upper_match(fst_cntxt) = NULL;

  FST_altchain_p(fst_cntxt) = FALSE;
  FST_status_bar_p(fst_cntxt) = TRUE;
  FST_status_bar_increment(fst_cntxt) = 0;
  FST_arc_count(fst_cntxt) = 0;
  FST_cur_byte(fst_cntxt) = 0;

  FST_last_header(fst_cntxt) = make_default_header();
  FST_next_header(fst_cntxt) = make_default_header();
  FST_cur_state(fst_cntxt) = NULL;
  FST_state_stack(fst_cntxt) = NULL;
  FST_attribute_count(fst_cntxt) = 7;
  FST_attributes(fst_cntxt) = make_default_attributes(7);
  set_io_char_encoding(fst_cntxt,DEFAULT_CHAR_ENCODING);
  return(fst_cntxt);
} /* make_default_context */

FST_CNTXTptr set_default_context (void)
{
  default_context_object = make_default_context();
  return(default_context_object);
}

void set_io_char_encoding(FST_CNTXTptr fst_cntxt, int io_char_encoding)
{
  switch (io_char_encoding) 
  {
  case CHAR_ENC_UTF_8 :
	fst_cntxt->io_character_encoding = CHAR_ENC_UTF_8;
	break;
  case CHAR_ENC_ISO_8859_1 :
	fst_cntxt->io_character_encoding = CHAR_ENC_ISO_8859_1;
	break;
  default:
	fst_cntxt->io_character_encoding = DEFAULT_CHAR_ENCODING;
  }
}

void free_default_context(FST_CNTXTptr fst_cntxt)
{
  if (fst_cntxt)
    {
      free(FST_last_header(fst_cntxt));
      free(FST_next_header(fst_cntxt));
      free(FST_attributes(fst_cntxt));
      free(FST_output_buffer(fst_cntxt));
      free(FST_input_seq(fst_cntxt));
      free(fst_cntxt);
    }
}

/* unset_default_context must be used when the default context was
   established by using cfsm_set_default_context(). The difference with 
   free_default_context() is that it does not try to free() the 
   default context itself (which is a global)
*/
void unset_default_context()
{
  FST_CNTXTptr fst_cntxt;
  fst_cntxt = get_default_context();
  if (fst_cntxt)
    {
      free(FST_last_header(fst_cntxt));
      free(FST_next_header(fst_cntxt));
      free(FST_attributes(fst_cntxt));
      free(FST_output_buffer(fst_cntxt));
      free(FST_input_seq(fst_cntxt));
    }
}

/* In order to access other low-level APIs (like the fsopt compression
   functions), it is necessary to retrieve the current context. */

void * get_default_context(void) {
    return default_context_object;
}
