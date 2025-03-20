/* $Id: text-io.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 1991-94 Xerox Corporation. All rights reserved. */

/******************************************************************************
 **
 **  text-io.c
 **
 **      Todd Yampol, Xerox PARC, September 1991
 **
 **
 *****************************************************************************/
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <ctype.h>

#include "fst/struct.h"
#include "fst/modify.h"
#include "fst/calculus.h"
#include "fst/text-io.h"
#include "fst/net-io.h"
#include "fst/fsm_io.h"
#include "fst/flags.h"
#include "fst/utf8_fatstr_util.h"
#include "fst/prompt.h"


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

#define WORD_BUF_SIZE 10000
#define SEQ_BUF_SIZE 10000
#define NUM_STATES_BEFORE_COMPACTION 30000

FILE *WORD_FILE;
char *WORD_BUF;
id_type *SEQ_BUF, *UPPER_BUF, *LOWER_BUF;
STACKptr STACK;
FILE *OUT_STREAM;


static STATEptr vector_ref(id_type index, STATEptr **vector, int *vlen)
     /*****
      *
      * Return the cell of VECTOR indicated by INDEX.  If 
      * VECTOR is shorter than index, expand accordingly,  
      * adjust length, and return NULL.
      *
      *****/
{
  if (index < *vlen)
    {
      return ((*vector)[index]);
    }
  else
    {
      /* expand vector, clear new cells, return null */
      /* expand by 100 over INDEX so we don't have to do this very often */
      *vector = (STATEptr *) realloc(*vector,
                                     (size_t) (index+100) * sizeof(STATEptr));
      if (*vector == NULL)
        not_enough_memory("VECTOR_REF");
      
      for (; *vlen < index+100; (*vlen)++)
        (*vector)[*vlen] = NULL;
      return (NULL);
    }
}

void add_seq_to_net (id_type *seq, NETptr net, STATEptr final_state,
                     STATEptr **start_destinations, int *sd_len, 
                     int limit)
     /*****
      *
      * Adds one sequence to NET.  LIMIT, if non-negative, is the longest
      * sequence to add. If negative, length is not limited.  
      *
      * An empty sequence (starts with ID_EOS) or a sequence that consists 
      * of EPSILONs only makes the start state final. Otherwise, 
      * EPSILONs are ignored.
      *
      *****/
{
  STATEptr state, new_state;
  id_type first_label, label, l, arc_label;
  id_type prev_label = ID_EOS;
  int state_type;
  ARCptr last_arc = NULL, new_arc, prev, atail;
  
  if (limit == 0)
    return;
  
  /* Empty seq now makes the start state final. 3/16/93, TSY */

  /* Print a message for now.  Delete this later. */
  if (*seq == ID_EOS) /* seq is empty */
    fsm_fprintf(stderr, "NOTE:  Making start state final on empty sequence.\n");

  while(*seq == EPSILON)
    seq++;
      
  if (*seq == ID_EOS)
    STATE_final(NET_start_state(net)) = TRUE;
  /* end of changes TSY */
  
  first_label = *seq++;
  limit--;
  
  if (first_label == ID_EOS)
    return;
  
  state = vector_ref(first_label, start_destinations, sd_len);
  if (state == NULL)
    {
      binary_labels_add_to(NET_labels(net), first_label);
      state = (*start_destinations)[first_label] = make_state(NULL, net);
    }
  
  /* begin the real processing */
  state_type = 1;
  
  while (TRUE)
    {
      /* LAST_ARC is the last arc visited/created, whose
         destination is STATE.  If STATE is NULL, then the 
         creation of the destination was deferred to see if
         it should be final or even the FINAL_STATE.  If the 
         destination is a label (ARC_visit_mark(last_arc) == 1),
         then creation of a non-final state with 1 arc labelled
         that way to the final state was deferred to see if it 
         should come from the PENULT_DESTS array.
         */
      
      /* Ignore epsilons. */
      
      do {
        label = *seq++;
      } while (label == EPSILON);
      
      
      if (label == ID_EOS || limit == 0)
        {       /* end of seq */
          if (state == NULL && state_type == 1)
            {
              if (prev_label != ID_EOS)
                {
                  ARC_destination(last_arc) = (STATEptr)(int) prev_label;
                  ARC_visit_mark(last_arc) = 1; /* mark destination as label */
                }
              else
                {
                  ARC_destination(last_arc) = final_state;
                  ARC_visit_mark(last_arc) = 0;
                }
            }
          else 
            {
              switch (state_type)
                {
                case (1): /* state is just a state */
                  STATE_final(state) = TRUE;
                  break;
                case (2): /* state is a label */
                  l = (int) state;
                  new_arc = make_arc(l, final_state, net);
                  state = make_state(new_arc, net);
                  state_type = 1;
                  STATE_final(state) = TRUE;
                  STATE_client_cell(state) = (void *) new_arc;
                  ARC_destination(last_arc) = state;
                  ARC_visit_mark(last_arc) = 0;
                  break;
                }
            }
          return;
        }
      /* Count down */
      
      limit--;
      
      /* maintain label field */
      binary_labels_add_to(NET_labels(net), label);
      if (state == NULL && state_type == 1)
        {
          /* we are adding an arc to the dummy non-final state.
             That state is not a non-final penult for the label L.
             We do nothing else now, waiting to see what the next
             label brings.
             */
          if (prev_label != ID_EOS)
            {
              ARC_destination(last_arc) = new_state = make_state(NULL, net);
              ARC_visit_mark(last_arc) = 0;
              last_arc = make_arc(prev_label, NULL, net);
              STATE_arc_set(new_state) = last_arc;
              STATE_client_cell(new_state) = (void *) last_arc;
            }
          prev_label = label;
        }
      else
        {
          switch (state_type)
            {
            case (1):  /* STATE is a real state */
              if (state == final_state)
                {
                  /* we are adding an arc to the distinguished
                     final state, must split
                     */
                  state = make_state(NULL, net);
                  state_type = 1;
                  STATE_final(state) = TRUE;
                  ARC_destination(last_arc) = state;
                  ARC_visit_mark(last_arc) = 0;
                }
              break;
            case (2):  /* STATE is a label */
              /* STATE is a label, so the deferred
                 destination must now be instantiated with an
                 arc for the previous label going to the 
                 final state.  We know that it is final, cause
                 we are now on a new seq.
                 */
              l = (int) state;
              new_arc = make_arc(l, final_state, net);
              state = make_state(new_arc, net);
              state_type = 1;
              STATE_client_cell(state) = (void *) new_arc;
              ARC_destination(last_arc) = state;
              ARC_visit_mark(last_arc) = 0;
              break;
            }
          
          /* most recent match is stored in client cell */
          last_arc = (ARCptr) STATE_client_cell(state);
          if (last_arc != NULL && ARC_label(last_arc) == label)
            {
              state = ARC_destination(last_arc);
              if (ARC_visit_mark(last_arc) == 1)
                state_type = 2;
              else
                state_type = 1;
            }
          else /* insert arc into state's arc set */
            {
              for (prev = NULL, atail = STATE_arc_set(state);
                   ;
                   prev = atail, atail = next_arc(atail))
                {
                  /* arcs are sorted in reverse order by label number */
                  if (atail == NULL || ARC_label(atail) < label) 
                    {
                      last_arc = make_arc(label, NULL, net);
                      STATE_client_cell(state) = (void *) last_arc;
                      if (prev != NULL)
                        {
                          next_arc(last_arc) = next_arc(prev);
                          next_arc(prev) = last_arc;    
                        }
                      else
                        {
                          next_arc(last_arc) = STATE_arc_set(state);
                          STATE_arc_set(state) = last_arc;
                        }
                      state = NULL;
                      state_type = 1;
                      break; /* for loop */
                    }
                  else
                    {
                      last_arc = atail;
                      arc_label = ARC_label(last_arc);
                      if (label == arc_label)
                        {
                          STATE_client_cell(state) = (void *) last_arc;
                          state = ARC_destination(last_arc);
                          if (ARC_visit_mark(last_arc) == 1)
                            state_type = 2;
                          else
                            state_type = 1;
                          break; /* for loop */ 
                        }       
                    }
                }
            }
        }
    }
}

NETptr cleanup_seq_list_fsm (NETptr net, STATEptr *start_destinations,
                             int sd_len, STATEptr final_state)
     /*****
      *
      * Extract real network from intermediate form.
      *
      *****/
{
  STATEptr *penult_destinations, state, dest;
  int pd_len;
  id_type label;
  int i;
  LABELptr lab;
  ARCptr arc;
  
  /* fill in penult destinations */
  pd_len = 100;
  penult_destinations =
    (STATEptr *) calloc((size_t) pd_len, (size_t) sizeof(STATEptr));
  
  if (!penult_destinations)
    not_enough_memory("SEQS_TO_NET");
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      STATE_client_cell(state) = NULL; /* clear client cell */
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
        if (ARC_visit_mark(arc) == 1)
          {     /* destination is still a label */
            ARC_visit_mark(arc) = 0;
            label = (int) ARC_destination(arc);
            ARC_destination(arc) =
              vector_ref(label, &penult_destinations, &pd_len);
            if (ARC_destination(arc) == NULL)
              ARC_destination(arc) = penult_destinations[label] =
                make_state(make_arc(label, final_state, net), net);
          }
    }
  
  free((void *) penult_destinations);
  
  /* create start destination states & arcs */
  for (label = 0; label < sd_len; label++)
    {
      dest = start_destinations[label];
      if (dest != NULL)
        {
          start_destinations[label] = NULL;
          if (STATE_arc_set(dest) == NULL && STATE_final(dest))
            {
              /* This is the case where, for eg, 'a' is in the machine, but
                 not any other strings beginning with 'a'.   In this
                 case, just make an arc from the start state to the final state.
                 It is safe to free the old dest now because nothing else
                 is pointing to it, and it has no arcs.  This happens in
                 the call to "delete_states" below.  It is ok to use the
                 STATE_visit_mark because it isn't used otherwise by this
                 algorithm.  All states marked 1 get deleted below.  The 
                 result network has clean state marks (all 0).
                 %%%TSY, 12/8/92
                 */
              push_arc(make_arc(label, final_state, net),
		       NET_start_state(net));
              STATE_visit_mark(dest) = 1;
              NET_pruned(net) = FALSE;
            }
          else
            {
              push_arc(make_arc(label, dest, net), NET_start_state(net));
            }
        }
    }
  
  if (!NET_pruned(net))
    {
      /* NET_pruned(net) was set to FALSE immediately
         above.  Thus, this call to delete states only prunes
         states in the special case discussed above.
         %%%TSY 12/8/92
         */
      delete_states(net, state_mark_zero_p);
      NET_pruned(net) = TRUE;
    }
  
  
  binary_to_label(NET_labels(net));
  
  /* calculate net arity from labels: highest wins */
  for (i = 0; i < ALPH_len(NET_labels(net)); i++)
    {
      lab = id_to_label(ALPH_item(NET_labels(net), i));
      if ((int16)LABEL_arity(lab) > NET_arc_label_arity(net))
        NET_arc_label_arity(net) = LABEL_arity(lab);
    }
  
  return(net);
}

static void compress_intermediate_nets(NETptr *cur_result,
                                       STATEptr *final_state, 
                                       NVptr prev_results, int result_count, 
                                       STATEptr start_destinations[],
                                       int sd_len, int last_p)
     /*****
      *
      * Merge nets on a binary counting pattern to save memory
      * (and perhaps time due to better paging performance) while
      * doing seqs_to_net (i.e. make_word_list_fsm).
      *
      *****/
{
  int i, len;
  NVptr temp;
  
  cleanup_seq_list_fsm(*cur_result, start_destinations, sd_len, *final_state);
  nv_add(*cur_result, prev_results);
  temp = make_nv(2);
  
  while (result_count > 0 && result_count % 2 == 0)
    {
      len = NV_len(prev_results);
      NV_net(temp, 0) = NV_net(prev_results, len - 1);
      NV_net(temp, 1) = NV_net(prev_results, len - 2);
      
      NV_net(prev_results, len - 1) = NULL;
      NV_net(prev_results, len - 2) = union_fsm(temp, DONT_COPY, DO_MINIMIZE);
      NV_len(prev_results)--;
      result_count /= 2;
    }
  free_nv_only(temp);
  
  if (last_p)
    {
      *cur_result = union_fsm(prev_results, DONT_COPY, DO_MINIMIZE);
    }
  else
    {
      /* prepare for next round */
      *cur_result = null_fsm();
      label_to_binary(NET_labels(*cur_result));
      *final_state = make_state(NULL, *cur_result);
      STATE_final(*final_state) = TRUE;
      
      /* clear start_destinations */
      for (i = 0; i < sd_len; i++)
        start_destinations[i] = NULL;
      
    }
}


NETptr seqs_to_net(id_type *(*seq_form)(void))
     /*****
      *
      * Each call to SEQ_FORM returns a vectors of arc labels terminated by ID_EOS.
      * Returns a minimal network containing a path for every sequence
      * terminating in a final state.  Terminates if SEQ is NULL.
      * Empty sequences (beginning with ID_EOS) are ignored.  Sequences
      * of EPSILON make the start state final.  Otherwise, EPSILON
      * is ignored.
      *
      *****/
{
  NETptr net;
  NVptr nets = make_nv(0);
  id_type *seq;
  int sd_len, result_count = 0;
  STATEptr final_state, *start_destinations;
  long int line_count = 0;
  
  net = null_fsm();
  NET_minimized(net) = FALSE;
  label_to_binary(NET_labels(net));
  
  seq = seq_form();
  
  if (!seq)
    {
      NET_minimized(net) = TRUE;
      return(net);
    }
  
  final_state = make_state(NULL, net);
  STATE_final(final_state) = TRUE;
  
  sd_len = 100;
  start_destinations =
    (STATEptr *) calloc((size_t) sd_len, (size_t) sizeof(STATEptr));
  
  if (!start_destinations)
    not_enough_memory("SEQS_TO_NET");
  
  while (seq)
    {
      add_seq_to_net(seq, net, final_state, &start_destinations, &sd_len, -1);
      
      line_count++;
      if (line_count % 100 == (long) 0)
        {
          fputc('.', stderr);
          fflush(stderr);
        }
      
      if (NET_num_states(net) >= NUM_STATES_BEFORE_COMPACTION)
        {
          result_count++;
          fsm_fprintf(stderr,
                  "\n%ld words so far. Compressing intermediate nets.\n",
                  line_count);
          fflush(stderr);
          compress_intermediate_nets(&net, &final_state, nets, result_count,
                                     start_destinations, sd_len, 0);
          fputc('\n', stderr);
          fflush(stderr);
        }
      seq = seq_form();
    }
  fputc('\n', stderr);
  result_count++;
  compress_intermediate_nets(&net, &final_state, nets, result_count,
                             start_destinations, sd_len, 1);
  
  /* NOTE: most of the time, NET will already be minimized */
  binary_to_label(NET_labels(net));
  free_alph(NET_sigma(net));
  NET_sigma(net) = label_sigma(net);
  check_for_flags(net);
  return (min_fsm(net, DONT_COPY));
}

static id_type *collect_sequence (char *word, int literal_p)
     /*****
      *
      * Convert word (a string terminated by '\0' or '\n') into
      * a sequence of arc labels.  Create new labels as necessary.
      * Stores the newly formed sequence in the global buffer SEQ_BUF.  
      * Returns a pointer to SEQ_BUF.  Caller must initialize SEQ_BUF.
      * Special characters (0 = EPSILON, ? = OTHER, % = ESCAPE, ! = COMMENT)
      * are assumed to have '%' as prefix when used as ordinary symbols.
      * If literal_p is TRUE, there are no special characters and
      * everything is taken literally.
      *
      *****/
{
  id_type *seq = SEQ_BUF, id;
  static FAT_CHAR fat[2];
  int error;
  char *limit = word + WORD_BUF_SIZE;
  
  if (word == NULL)
    return(NULL);
  
  CHARSET(fat) = CHARSET((fat + 1)) = CHARNUM((fat + 1)) = '\0';

  if (literal_p)
    while (*word != '\0' && *word != '\n' && word < limit)
      intern_char(seq++, *word++);
  else
    while (*word != '\0' && *word != '\n' && word < limit)
      {
	if (word[0] == '0' && word[1] == '\0')
	  *seq++ = EPSILON;
	else if (word[0] == '?' && word[1] == '\0')
	  *seq++ = OTHER;
	else if (*word == '!')
	  break;
	else
	  {
	    if (*word == '%' && (word[1] != '\0' || word[1] != '\n'))
	      word++;
	    CHARNUM(fat) = *word;
	    error= name_to_atom_id(&id, fat);
	    if (error == ERR_NOLABEL)
	      {
		intern_atomic_label(&id, fat);
		*seq++ = id;
	      }
	    else
	      *seq++ = id;
	  }
	word++;
      }

  *seq = ID_EOS; /* terminate sequence with ID_EOS */
  
  return (SEQ_BUF);
}

static id_type *utf8_collect_sequence (char *word, int literal_p)
     /*****
      *
      * Convert word (a string terminated by '\0' or '\n') into
      * a sequence of arc labels.  Create new labels as necessary.
      * Stores the newly formed sequence in the global buffer SEQ_BUF.  
      * Returns a pointer to SEQ_BUF.  Caller must initialize SEQ_BUF.
      * Special characters (0 = EPSILON, ? = OTHER, % = ESCAPE, ! = COMMENT)
      * are assumed to have '%' as prefix when used as ordinary symbols.
      * If literal_p is TRUE, there are no special characters and
      * everything is taken literally.
      *
      *****/
{
  id_type *seq = SEQ_BUF, id;
  static FAT_CHAR fat[2];
  int error;
  char *limit = word + WORD_BUF_SIZE;
  
  if (word == NULL)
    return(NULL);
  CHARSET(fat) = CHARSET((fat + 1)) = CHARNUM((fat + 1)) = '\0';
  
  if (literal_p)
    {
      while (*word != '\0' && *word != '\n' && word < limit)
	{
	  
	  ConvertUTF8toFAT_CHAR ((UTF8**)&word, 0, fat, lenientConversion);
	  
	  error= name_to_atom_id(&id, fat);
	  if (error == ERR_NOLABEL)
	    {
	      intern_atomic_label(&id, fat);
	      *seq++ = id;
	    }
	  else 
	    {
	      *seq++ = id;
	    }
	  
	  //		utf8_intern_char(seq++, *word++);
	}
    }
  
  else
    {
      while (*word != '\0' && *word != '\n' && word < limit)
	  {
	  if (word[0] == '0' && word[1] == '\0')
	    {
	      *seq++ = EPSILON;
	    }
	  else if (word[0] == '?' && word[1] == '\0')
	    {
	      *seq++ = OTHER;
	    }
	  else if (*word == '!')
	    {
	      break;
	    }
	  else
	    {
	      if (*word == '%' && (word[1] != '\0' || word[1] != '\n'))
		{
		  word++;
		}
	      
	      ConvertUTF8toFAT_CHAR ((UTF8 **)&word, (UTF8 *)word+1, fat, lenientConversion);
	      
	      error= name_to_atom_id(&id, fat);
	      if (error == ERR_NOLABEL)
		{
		  intern_atomic_label(&id, fat);
		  *seq++ = id;
		}
	      else
		{
		  *seq++ = id;
		}
	    }
	  word++;
	  }
    }
  
  *seq = ID_EOS; /* terminate sequence with ID_EOS */
  return (SEQ_BUF);
}


static id_type *read_line_and_convert(void)
/*****
 *
 * Skip blank lines in WORD_FILE.
 * Read first non-blank line, convert into an array of label ids (SEQ_BUF).
 * No characters are special.
 * Return NULL on EOF.
 *
 *****/
{	
  int err;

  /* skip blank lines */
  do {
    err = universal_read_line(&WORD_BUF, WORD_BUF_SIZE, WORD_FILE);
  } while (err == NO_ERROR && *WORD_BUF == '\0');
	
  if (err == NO_ERROR)	
    return (collect_sequence(WORD_BUF, TRUE));
  else
    return (NULL);
}

static id_type *utf8_read_line_and_convert(void)
/*****
 *
 * Skip blank lines in WORD_FILE.
 * Read first non-blank line, convert into an array of label ids (SEQ_BUF).
 * No characters are special.
 * Return NULL on EOF.
 *
 *****/

{	
  int err ;

  /* skip blank lines */
  do {
    err = universal_read_line(&WORD_BUF, WORD_BUF_SIZE, WORD_FILE);
  } while (err == NO_ERROR && *WORD_BUF == '\0');
	
  if (err == NO_ERROR)	
    return (utf8_collect_sequence(WORD_BUF, TRUE));
  else
    return (NULL);
}


static int starts_with_a_comment(FILE *stream)
{
  int ch = getc(stream);
  
  ungetc(ch, stream);

  return(ch == '#' || ch == '!');
}


int check_for_char_encoding(int *char_encoding, FILE *stream)
{
  /* Checks the first line of the file. If the first character
     is a comment, # or ! and if the line continues with "-*-"
     and contains the attribute "coding", the following value
     is interpreted as the character encoding of the file.
     Acceptable values are UTF-8 and ISO-8859-1. Returns NO_ERROR
     if either the first line does not conform to this format
     or if the recognized value is one of the two accepted
     encodings. Otherwise returns ERR_BAD_INPUT. The char_encoding
     parameter is set to CHAR_ENCODING_UNKNOWN if the first
     line provides no information or to one of the two accepted
     encodings if it does. Note the

     # -*- coding: utf-8 -*-

     idiom is an Emacs convention.
  */
     
  int error = NO_ERROR;
  long file_pos = ftell(stream);
  *char_encoding = CHAR_ENC_UNKNOWN;

  if (starts_with_a_comment(stream))
    {
      char *buf;
      char line[512];
      char *delim, *attr, *val = NULL;
      
      buf = line;

      if (universal_read_line(&buf, 100, stream) == NO_ERROR) {

	/* Ignore initial # marks. */

	delim = strtok(line, " \t:");

	while (delim && strcmp(delim, "#") == 0 && strcmp(delim, "!")) {
	  delim = strtok(NULL, " \t:");
	}

	if (strcmp(delim, "-*-") == 0) {
	  do {
	    attr = strtok(NULL, " \t:");
	    if (strcasecmp(attr, "coding") == 0)
	      {
		val = strtok(NULL, " \t:");
		break;
	      }
	  } while (attr && strcmp(attr, "-*-") != 0);
	}

	if (val) {
	  if (strcasecmp(val, "utf-8") == 0 ||
	      strcasecmp(val, "utf8") == 0)
	    *char_encoding = CHAR_ENC_UTF_8;
	  else if (strcasecmp(val, "iso-8859-1") == 0 ||
		   strcasecmp(val, "iso88591") == 0 ||
		   strcasecmp(val, "iso8859-1") == 0 ||
		   strcasecmp(val, "iso-8859/1") == 0 ||
		   strcasecmp(val, "iso8859/1") == 0 ||
		   strcasecmp(val, "latin-1") == 0 ||
		   strcasecmp(val, "latin1") == 0)
	    *char_encoding = CHAR_ENC_ISO_8859_1;
	  else
	    {
	      fprintf(stderr, "*** Unknown character encoding: %s\n", val);
	      error = ERR_BAD_INPUT;
	    }
	}
      }
    }

  fseek(stream, file_pos, SEEK_SET);

  return(error);
}


NETptr word_file_to_fsm(FILE *f)
{       
  NETptr net;
  WORD_FILE = f;
  
  SEQ_BUF = (id_type *) calloc((size_t) SEQ_BUF_SIZE, (size_t) sizeof(id_type));
  WORD_BUF = (char *) calloc((size_t) WORD_BUF_SIZE, (size_t) sizeof(char));
  
  if (!SEQ_BUF || !WORD_BUF)
    not_enough_memory("WORD_FILE_TO_FSM");
  
  net = seqs_to_net(FST_read_line_and_convert);
  
  WORD_FILE = NULL;
  free((void *) SEQ_BUF);
  free((void *) WORD_BUF);
  
  return (net);
}

static void line_to_labels (id_type *id_buf)
     /*****
      *
      * Convert a line (a string terminated by '\0') into
      * a sequence of symbol ids.  Symbols are separated by blanks.
      * Create new labels as necessary.
      * Stores the newly formed sequence in ID_BUF.  
      *
      *****/
{
  id_type id, *seq = id_buf;
  int no_quote_p;
  char ch, *sym, *wrd = WORD_BUF;
  FAT_STR fat;
  int error;
  
  /* Process all characters in WORD_BUF up to NEWLINE or END-OF-FILE */
  
  while(*wrd != '\0')
    {
      /* Skip initial white space */
      
      while(*wrd == ' ' || *wrd == '\t')
        wrd++;
      
      /* After skipping white space, check again for END-OF-LINE */
      
      if (*wrd == '\0')
        break;
      
      /* SYM is the span of characters in WORD_BUF up to
         the next BLANK, TAB, NEWLINE, RETURN, or END-OF-FILE not including
         an initial escape character.
         */
      
      sym = wrd;
      if (*wrd == '%' && *(wrd + 1) != '\0')
        {
          wrd++;
          no_quote_p = FALSE;
        }
      else
        no_quote_p = TRUE;
      
      sym = wrd++;
      while(*wrd != ' ' && *wrd != '\t' && *wrd != '\0')
        wrd++;
      
      /* Save space character and terminate string. */
      ch = *wrd;
      *wrd = '\0';
      
      /* Check for EPSILON, OTHER, and COMMENT */
      
      if (*sym == '0' && wrd == (sym + 1) && no_quote_p)
        *seq++ = EPSILON;
      else if (*sym == '?' && wrd == (sym + 1) && no_quote_p)
        *seq++ = OTHER;
      else if ((*sym == '!' || *sym == '#' ) && no_quote_p)
        break;
      else         
        {
          fat = FST_thin_to_temp_fat(sym);
          error = name_to_atom_id(&id, fat);
          if (error != NO_ERROR)
	    {
	      intern_atomic_label(&id, fat);
	    }
	  *seq++ = id;
        }
      
      /* Restore space character (it might be ' ', '\t' or'\0'). */
      *wrd = ch;
    }
  *seq = ID_EOS; /* terminate sequence with ID_EOS */
}

static id_type *two_lines_to_labels(void)
{
  /* Skips white space and comments.  Converts the first content line
     to a label sequence.  If the next line is empty or if END-OF-FILE
     has been reached, the first label sequence is returned as such.
     
     Otherwise the next line is converted to a second label sequence and
     merged with the first line to a sequence of fstpairs.  Trailing EPSILONs
     are added, if necessary to make the sequences equally long.
     */
  
  uint16 u_pos, l_pos;
  id_type tuple_id;
  int err;

  do {

    /* If an empty sequence is received, the input is a comment
       line.  Keep reading until something else appears.
    */
    do {
      err = universal_read_line(&WORD_BUF, WORD_BUF_SIZE, WORD_FILE);
    } while (err == NO_ERROR &&
	     (*WORD_BUF == '\0' || *WORD_BUF == '!' || *WORD_BUF == '#'));
    
    if (err != NO_ERROR)
      return(NULL);
    
    line_to_labels(UPPER_BUF);
  } while (*UPPER_BUF == ID_EOS);
  
  /* If an empty line follows, don't build fstpairs. */
  
  err = universal_read_line(&WORD_BUF, WORD_BUF_SIZE, WORD_FILE);
  
  if (err != NO_ERROR || *WORD_BUF == '\0')
    return(UPPER_BUF);
  
  line_to_labels(LOWER_BUF);
  
  u_pos = 0;
  l_pos = 0;
  while (UPPER_BUF[u_pos] != ID_EOS && LOWER_BUF[l_pos] != ID_EOS)
    {     
      make_fstlabel(&tuple_id, UPPER_BUF[u_pos], LOWER_BUF[l_pos++]);
      UPPER_BUF[u_pos] = tuple_id;
      u_pos++;
    }
  
  /* If one side is shorter than the other, fill with EPSILONs at the end */
  
  if (UPPER_BUF[u_pos] != ID_EOS)
    do {
      make_fstlabel(&tuple_id, UPPER_BUF[u_pos], EPSILON);
      UPPER_BUF[u_pos] = tuple_id ;
      u_pos++;
    } while (UPPER_BUF[u_pos] != ID_EOS);
  else if (LOWER_BUF[l_pos] != ID_EOS)
    do {
      make_fstlabel(&tuple_id, EPSILON, LOWER_BUF[l_pos++]);
      UPPER_BUF[u_pos] = tuple_id;
      u_pos++;
    } while (LOWER_BUF[l_pos] != ID_EOS);
  
  UPPER_BUF[u_pos] = ID_EOS;
  return(UPPER_BUF);
}

NETptr network_from_text_file(FILE *stream)
{       
  NETptr net;
  
  WORD_FILE = stream;
  
  UPPER_BUF = (id_type *) calloc((size_t) SEQ_BUF_SIZE, (size_t) sizeof(id_type));
  LOWER_BUF = (id_type *) calloc((size_t) SEQ_BUF_SIZE, (size_t) sizeof(id_type));
  
  WORD_BUF = (char *) calloc((size_t) WORD_BUF_SIZE, (size_t) sizeof(char));
  
  if (!UPPER_BUF || !LOWER_BUF || !WORD_BUF)
    not_enough_memory("NETWORK_FROM_TEXT_FILE");
  
  net = seqs_to_net(two_lines_to_labels);
  
  WORD_FILE = NULL;
  free((void *) UPPER_BUF);
  free((void *) LOWER_BUF);
  free((void *) WORD_BUF);
  return (net);
}

static int16 utf8_print_level (int i, int careful_p)
{
  /* If CAREFUL_P is TRUE, we print EPSILON as 0, OTHER as ?,
     and we omit trailing epsilons.  If CAREFUL_P is FALSE,
     symbol names are printed as they are without separators.
     The noncareful mode should only be used for simple
     word files. */

  id_type id;
  int16 arity, max_arity = 1;
  int j, loc, len, max_len, skip, pos = 0, end = 0;
  ARCptr arc;
  LABELptr label;
  FAT_STR fs;
  int limit;
  STACK_BLOCKptr block = STACK_top(STACK);
  STACK_BLOCKptr off_limits = STACK_BLOCK_next(block);

	UTF8 utf8_buffer[7];
	size_t ibuf;


  /* Find the bottom of the stack */

  while (STACK_BLOCK_previous(block))
    block = STACK_BLOCK_previous(block);
  
  /* Process the blocks in reverse order up to the top
     block of the stack but not beyond it */

  while (block != off_limits)
    {
      if (block == STACK_top(STACK))
	limit = STACK_cur_loc(STACK);
      else
	limit = STACK_BLOCK_SIZE;

      for (loc = 0; loc < limit; loc++)
	{
	  arc = (ARCptr) STACK_BLOCK_cell(block, loc);
	  id = ARC_label(arc);
	  label = id_to_label(ARC_label(arc));
	  arity = LABEL_arity(label);
	  max_len = (int) LABEL_data(label);
      
	  if (arity > 1)
	    {
	      /* tuple, get the label from the relevant level */
          
	      id = TUPLE_labels(LABEL_tuple(label))[i];
	      label = id_to_label(id);
	      len = (int) LABEL_data(label);
          
	      if (max_arity < arity)
		max_arity = arity;
	    }
	  else
	    len = max_len;
      
	  if ((loc > 0 || STACK_BLOCK_previous(block)) && careful_p)
	    max_len++;
      
	  skip = max_len - len;
      
	  for (j = 0; careful_p && j < skip; j = j + 2)
	    WORD_BUF[pos++] = ' ';
      
      
		if (id == EPSILON)
			WORD_BUF[pos++] = '0';
		else if (id == OTHER)
			WORD_BUF[pos++] = '?';
		else
	    {
			fs = LABEL_atom(label);

			/* Prefix with the ESCAPE character if the symbol
			name begins with a special character.
			*/

			if (careful_p && len == 2 &&
			strchr(QUOTED_AS_SINGLE, CHARNUM(fs)) != NULL)
				WORD_BUF[pos++] = '%';

			while (CHARNUM(fs) || CHARSET(fs))
			{
				if (careful_p && strchr(QUOTED_ANYWHERE, CHARNUM(fs)) != NULL)
					WORD_BUF[pos++] = '%';

				if ( (unsigned int)(256U * CHARSET(fs)+ CHARNUM(fs)) > 127U ){
					ConvertFatChartoUTF8 (fs, utf8_buffer, utf8_buffer + 7, lenientConversion);
					for (ibuf=0; ( ibuf < 7 ) && utf8_buffer[ibuf] ; ibuf++) 
					{
						WORD_BUF[pos++] = utf8_buffer[ibuf];
					}
				}
				else
				{
				  /* No need to translate, it's ASCII */
					WORD_BUF[pos++] = CHARNUM(fs);
				}
				fs++;
			}
	    }
	  for (j = 1; careful_p && j < skip; j = j + 2)
	    WORD_BUF[pos++] = ' ';
	  end = pos;
	}

	block = STACK_BLOCK_next(block);
    }
  WORD_BUF[end] = '\0';
  fsm_fputs(WORD_BUF, OUT_STREAM);
  fputc('\n', OUT_STREAM);
  return(max_arity);
}

static int16 print_level (int i, int careful_p)
{
  /* If CAREFUL_P is TRUE, we print EPSILON as 0, OTHER as ?,
     and we omit trailing epsilons.  If CAREFUL_P is FALSE,
     symbol names are printed as they are without separators.
     The noncareful mode should only be used for simple
     word files. */

  id_type id;
  int16 arity, max_arity = 1;
  int j, loc, len, max_len, skip, pos = 0, end = 0;
  ARCptr arc;
  LABELptr label;
  FAT_STR fs;
  int limit;
  STACK_BLOCKptr block = STACK_top(STACK);
  STACK_BLOCKptr off_limits = STACK_BLOCK_next(block);
  
  /* Find the bottom of the stack */

  while (STACK_BLOCK_previous(block))
    block = STACK_BLOCK_previous(block);
  
  /* Process the blocks in reverse order up to the top
     block of the stack but not beyond it */

  while (block != off_limits)
    {
      if (block == STACK_top(STACK))
	limit = STACK_cur_loc(STACK);
      else
	limit = STACK_BLOCK_SIZE;

      for (loc = 0; loc < limit; loc++)
	{
	  arc = (ARCptr) STACK_BLOCK_cell(block, loc);
	  id = ARC_label(arc);
	  label = id_to_label(ARC_label(arc));
	  arity = LABEL_arity(label);
	  max_len = (int) LABEL_data(label);
      
	  if (arity > 1)
	    {
	      /* tuple, get the label from the relevant level */
          
	      id = TUPLE_labels(LABEL_tuple(label))[i];
	      label = id_to_label(id);
	      len = (int) LABEL_data(label);
          
	      if (max_arity < arity)
		max_arity = arity;
	    }
	  else
	    len = max_len;
      
	  if ((loc > 0 || STACK_BLOCK_previous(block)) && careful_p)
	    max_len++;
      
	  skip = max_len - len;
      
	  for (j = 0; careful_p && j < skip; j = j + 2)
	    WORD_BUF[pos++] = ' ';
      
      
	  if (id == EPSILON)
	    WORD_BUF[pos++] = '0';
	  else if (id == OTHER)
	    WORD_BUF[pos++] = '?';
	  else
	    {
	      fs = LABEL_atom(label);
             
	      /* Prefix with the ESCAPE character if the symbol
		 name begins with a special character.
		 */
          
	      if (careful_p && len == 2 &&
		  strchr(QUOTED_AS_SINGLE, CHARNUM(fs)) != NULL)
		WORD_BUF[pos++] = '%';
          
	      while (CHARNUM(fs) || CHARSET(fs))
		{
		  if (careful_p && strchr(QUOTED_ANYWHERE, CHARNUM(fs)) != NULL)
		    WORD_BUF[pos++] = '%';
              
		  WORD_BUF[pos++] = CHARNUM(fs);
		  fs++;
		}
	    }
	  for (j = 1; careful_p && j < skip; j = j + 2)
	    WORD_BUF[pos++] = ' ';
	  end = pos;
	}

	block = STACK_BLOCK_next(block);
    }
  WORD_BUF[end] = '\0';
  fsm_fputs(WORD_BUF, OUT_STREAM);
  fputc('\n', OUT_STREAM);
  return(max_arity);
}

static void print_fst_path (void)
{
  int i, max_arity;
  
  /* Make the first pass.  Get maximal arity. */
  
  max_arity = print_level(0, TRUE);
  
  for (i = 1; i < max_arity; i++)
    print_level(i, TRUE);
  
  fputc('\n', OUT_STREAM);
}

static void utf8_print_fst_path (void)
{
  int i, max_arity;
  
  /* Make the first pass.  Get maximal arity. */
  
  max_arity = utf8_print_level(0, TRUE);
  
  for (i = 1; i < max_arity; i++)
    utf8_print_level(i, TRUE);
  
  fputc('\n', OUT_STREAM);
}

static void print_fsm_path (void)
{ 
  if (print_level(0, FALSE) > 1)
    handle_warning("Not expecting an FSTLABEL.",
                   "PRINT_FSM_PATH", 0);
}

static void utf8_print_fsm_path (void)
{ 
  if (utf8_print_level(0, FALSE) > 1)
    handle_warning("Not expecting an FSTLABEL.",
                   "PRINT_FSM_PATH", 0);
}


void network_to_text_file(NETptr net, FILE *stream)
{
  network_to_text(net, stream, print_fst_path);
}

void fsm_to_word_file(NETptr net, FILE *stream)
{
  network_to_text(net, stream, print_fsm_path);
}

void utf8_network_to_text_file(NETptr net, FILE *stream)
{
  network_to_text(net, stream, utf8_print_fst_path);
}

void utf8_fsm_to_word_file(NETptr net, FILE *stream)
{
  network_to_text(net, stream, utf8_print_fsm_path);
}


void clear_labels(ALPHABETptr labels)
{
  int i, j, len = ALPH_len(labels);
  LABELptr label, lab;
  
  for (i = 0; i < len; i++)
    {
      label = id_to_label(ALPH_item(labels, i)) ;
      
      LABEL_data(label) = (void *) NULL;
      
      if (LABEL_arity(label) > 1)
        for (j = 0; j < (int)LABEL_arity(label); j++)
          {
            lab = id_to_label(TUPLE_labels(LABEL_tuple(label))[j]);
            LABEL_data(lab) = (void *) NULL;
          }
      
    }
}

static int label_length(LABELptr label)
{
  /* Returns the length of elements in a fat string under the
     assumption that we are only interested in what is in
     CHARNUM.  Adds 1 to the length of a string consisting of
     a single fat char, if the second part is in QSINGLE
     (normally, "0?"). Adds 1 for every character in the
     string that is in QANY (normally "%!$*+-,.:;^{}[]<>@~\&|/ ").
     This is useful when the output of a printing function has
     to be read back in by a function for which these characters
     are special.
     */
  
  FAT_STR fs = LABEL_atom(label);
     
  if (strchr(QUOTED_AS_SINGLE, CHARNUM(fs)) != NULL && CHARNUM((fs + 1)) == 0 &&
      CHARSET((fs + 1)) == 0)
    return(2);
  else
    {
      int i = 0;
      
      while (CHARNUM(fs) || CHARSET(fs))
        {
          if (strchr(QUOTED_ANYWHERE, CHARNUM(fs)) != NULL)
            i++;
          
          fs++;
          i++;
        }
      
      return (i);
    }
}

void mark_length(ALPHABETptr labels)
{
  int i, j, id, ln, max_len, len = ALPH_len(labels);
  
  LABELptr label, lab;
  
  for (i = 0; i < len; i++)
    { 
      id = ALPH_item(labels, i);
      label = id_to_label(id);
      
      if (LABEL_arity(label) == 1)
	{
	  if (id == EPSILON || id == OTHER)
	    max_len = 1;
	  else
	    max_len = label_length(label);
	}
      else
        {
          max_len = 0;
          
          for (j = 0; j < (int)LABEL_arity(label); j++)
            {
              id = TUPLE_labels(LABEL_tuple(label))[j];
              lab = id_to_label(id);
              
              if (id == EPSILON || id == OTHER)
                ln = 1;
              else
                ln = label_length(lab);
              
              LABEL_data(lab) = (void *) ln;
              
              if (ln > max_len)
                max_len = ln;
            }
        }
      LABEL_data(label) = (void *) max_len;
    }
  
}

void network_to_text (NETptr net, FILE *stream, void print_path_fn(void))
{
  /* Print noncircular net as simple text file.
     Prefix special characters (0, ?, !, and %)
     with '%' if they occur as the first character
     of a symbol.  Also, watch out for ' '.
     */
  
  STATEptr state = NET_start_state(net);
  ARCptr arc = STATE_arc_set(state), old_arc = NULL;
  
  index_net(net);
  
  if (!NET_loop_free(net))
    {
      fsm_fputs("*** Aborting.  Network is circular.\n", stderr);
      return;
    }
  
  if ( (print_path_fn == print_fst_path) || (print_path_fn == utf8_print_fst_path) )
    mark_length(NET_labels(net));
  
  STACK = create_stack("Arcs to write");
  
  OUT_STREAM = stream;
  WORD_BUF = (char *) calloc((size_t) WORD_BUF_SIZE, (size_t) sizeof(char));
  
  if (!WORD_BUF)
    not_enough_memory("NETWORK_TO_TEXT_FILE");
  
  /* Final start state needs special handling.
     We fake a looping EPSILON arc leading back
     to the initial state to get something printed
     that preserves the information about the finality.
     (see ADD_SEQ_TO_NET).
     */
  
  if (STATE_final(state))
    {
      ARCptr dummy_arc = (ARCptr) malloc((size_t) sizeof(ARCtype));
      if (!dummy_arc)
        not_enough_memory("NETWORK_TO_TEXT_FILE");
      
      ARC_label(dummy_arc) = EPSILON;
      ARC_destination(dummy_arc) = state;
      push((void *) dummy_arc, STACK);
      print_path_fn();
      pop(STACK);
      free((void *) dummy_arc);
    }
  
  while (arc != NULL)
    {
      push((void *) arc, STACK);
      state = ARC_destination(arc);
      
      old_arc = arc;
      
      if (STATE_final(state) == TRUE)
        print_path_fn();
      
      arc = STATE_arc_set(state);
      
      while (old_arc != NULL && arc == NULL)
        {
          old_arc = (ARCptr) pop(STACK);
          if (old_arc != NULL)
            arc = next_arc(old_arc);
        }
    }
  
  free_stack(STACK);
  OUT_STREAM = NULL;
  free((void *) WORD_BUF);
  
  if ((print_path_fn == print_fst_path) || (print_path_fn == utf8_print_fst_path))
    clear_labels(NET_labels(net));
  
}

#endif /* SLIM_FST */
