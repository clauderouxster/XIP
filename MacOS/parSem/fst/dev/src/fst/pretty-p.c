/* $Id: pretty-p.c,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990-93 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "fst/pretty-p.h"
#include "fst/fsm_io.h"

#ifndef SLIM_FST  /* Only a minimal set of functions is
                     compiled when SLIM_FST is defined. */
#include "fst/struct.h"
#include "fst/pars-tbl.h"
#include "fst/flags.h"
#include "fst/context.h"
#include "fst/utf8_fatstr_util.h"

/******************************************************
 **
 **  PRETTY-P.C
 **
 **	 Todd Yampol, Xerox PARC, January 1990
 **  Lauri Karttunen, Xerox PARC, April 1991
 **  Lauri Karttunen, RXRC, December 1993
 **
 **  Ascii OUTPUT functions for simple FSM data types
 **
 ******************************************************/

#endif /* SLIM_FST */

char output_buffer[OUTPUT_BUFFER_SIZE];
int line_pos = 0;
int cur_pos = 0;
extern int IY_UNICODE;

int UNICODE_P;

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
/****************************************
 *                                      *
 *   reordering states and arcs         *
 *                                      *
 ****************************************/

int state_num_less_p(const void *s1, const void *s2)
{
  /* return -1, 0, or 1, depending on whether the integer in the
   * client cell of S1 is smaller, equal, or larger than the
   * contents of the client cell of S2
   */
  
  if (STATE_client_cell(* (STATEptr *) s2) ==
      STATE_client_cell(* (STATEptr *) s1))
    return(0);
  else if (STATE_client_cell(* (STATEptr *) s2) <
	   STATE_client_cell(* (STATEptr *) s1))
    return(1);
  else
    return(-1);
}

NETptr reorder_states (NETptr net, int sort_fn(const void*, const void*))
{
  /* create a temporary state vector, sort it by an increasing order of
   * client_cell contents, and relink states in the sorted order
   */
  
  STATEptr s, *state_vector;
  long ln;
  
  /* Don't bother unless there are at least two states */
  
  if (next_state(NET_states(net)) != NULL)
    {
      long count = NET_num_states(net), last;
      state_vector = (STATEptr *) calloc((size_t) count,
					 (size_t) sizeof (STATEptr));
      
      if (state_vector == NULL)
	{
	  handle_warning("Too little space to reorder", "REORDER_STATES", 0);
	  return(net);
	}
      
      for (s = NET_states(net), ln = 0;
	   s != NULL;
	   s = next_state(s), ln++)
	{
	  if (ln > count)
	    {
	      STATEptr s2;

	      for (s2 = s; s2; s2 = next_state(s2))
		count++;

	      handle_warning("State count incorrect. Too low.",
			     "REORDER_STATES",
			     (int) (NET_num_states(net) - count));

	      NET_num_states(net) = count;
	      state_vector = (STATEptr *)
		realloc(state_vector, (size_t) sizeof(STATEptr));
	    }

	  state_vector[ln] = s;
	}
      
      if (ln < count)
	{
	  handle_warning("State count incorrect. Too high.", "REORDER_STATES",
			 (int) count - ln);
	  NET_num_states(net) = count = ln;
	}

      last = count - 1;

      QSORT((void *) state_vector, (size_t) count,
	    (size_t) sizeof(STATEptr), sort_fn);
      
      for (ln = 0; ln < last; ln++)
	next_state(state_vector[ln]) = state_vector[ln+1];
      
      next_state(state_vector[last]) = NULL;
      NET_states(net) = state_vector[0];
      free(state_vector);
    }
  return(net);
}

 



/***********
 *
 * X_OUT functions know how to print one X nicely, BUFFER_OUT takes care
 * of right and left margins adjusting LINE_POS and adding carriage
 * returns. FRESH_LINE empties the remaining output from the buffer.
 *
 ************/

/* modified 7 April 1992 by Ken Beesley to output to different streams
 * macros in print_macros.h expand existing calls to these renamed
 * functions, adding stdout by default
 */

void reset_out_buffer(void)
{
  line_pos = 0;
  cur_pos = 0;
  *output_buffer = '\0';
}


void exact_buffer_out_s(int rm, int indent, FILE *stream)
/****
 *
 * Doesn't add extra space.
 *
 *****/
{
  if (rm > 0)
    {
      if (line_pos + cur_pos > rm)
	{
	  fsm_putc('\n', stream);
	  
	  for (line_pos = 0; line_pos < indent; line_pos++)
	    fsm_putc(' ', stream);
	}
 
 /*     else if (line_pos > 0)
	{
	  fsm_putc(' ', stream);
	  line_pos++;
	}
*/	
      output_buffer[cur_pos] = '\0';
      fsm_fputs(output_buffer, stream);
      line_pos = line_pos + cur_pos;
    }
  cur_pos = 0;
}
#endif /* SLIM_FST */

void buffer_out_s(int rm, int indent, FILE *stream)
{
  if (rm > 0)
    {
      if (line_pos + cur_pos > rm)
	{
	  fsm_putc('\n', stream);
	  
	  for (line_pos = 0; line_pos < indent; line_pos++)
	    fsm_putc(' ', stream);
	}
      else if (line_pos > 0)
	{
	  fsm_putc(' ', stream);
	  line_pos++;
	}	
      output_buffer[cur_pos] = '\0';
      fsm_fputs(output_buffer, stream);
      line_pos = line_pos + cur_pos;
    }
  cur_pos = 0;
}/* buffer_out_s */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void fresh_line_s(int rm, int indent, FILE *stream)
{
  if (cur_pos > 0)
    buffer_out_s(rm, indent, stream);
  cur_pos = 0;
  
  if (line_pos > 0) 
    fsm_putc('\n', stream);
  line_pos = 0;
}

/* no need for _s version, just puts char in the buffer */

static void AddCharToBuffer(char c)
{
    static int add_size = 0;

    if (cur_pos < OUTPUT_BUFFER_SIZE)
        output_buffer[cur_pos++] = c;
    else
    {
        if ((add_size & 0xFF) == 0)       /* each 256 call  */
            fsm_fprintf(stderr, "Output buffer full, please increase OUTPUT_BUFFER_SIZE to at least %d\n", OUTPUT_BUFFER_SIZE + add_size + 256);
        add_size++;
    }
}

static void AddStringToBuffer(char *str)
{
  while (*str)
    AddCharToBuffer(*str++);
}


void char_out(char c)
{
    AddCharToBuffer(c);
}

/* no need for _s version, just puts spaces in the buffer */

void spaces_out(int n)
{
  while (n-- > 0)
      AddCharToBuffer(' ');
}

/* no need for _s version, just puts string in the buffer */

void string_out(char *str)
{
  while (*str != '\0')
      AddCharToBuffer(*str++);
}

void word_out_s(char *str, int rm, int indent, FILE *stream)
{
  string_out(str);
  buffer_out_s(rm, indent, stream);
}

void string_array_out_s(int len, char *array[], int rm, 
			int indent, FILE *stream)
{
  int i;
  for (i = 0; i < len; i++)
    {
      string_out(array[i]);
      if (i + 1 < len)
	char_out(',');
      else
	char_out('.');
      buffer_out_s(rm, indent, stream);
    }
  fresh_line_s(rm, indent, stream);
}


static void int_to_str (long i, char *str)
{
  /* Converts an integer to a decimal numeral. Digits are generated
     in reverse order, then reversed in place in the internal buffer.
     (cf. Kernighan&Ritchie's 'itoa' and 'reverse')
     */
  
  int j,k;
  char c;
  
  j = 0;
  
  do {
    AddCharToBuffer((char) (i % 10 + '0'));
    j++;
  } while ((i /= 10) > 0);
  
  str[j] = '\0';

  for (k = 0, j--; k < j; k++, j--)
    {
      c = str[k];
      str[k] = str[j];
      str[j] = c;
    }
}

/* no need for _s version, just writes to internal buffer */

void int_out (long i)
{
  if (i < 0)
    {
        AddCharToBuffer('-');
      
      /* If 'labs' is not defined, we assume that the argument
	 to 'abs' can be long.
	 */
      
#ifndef labs
      i = abs(i);
#else
      i = labs(i);
#endif
    }
  
  int_to_str(i, output_buffer + cur_pos);
}

void hex_out_byte(unsigned char b)
{
  unsigned char n, j = b;
  short k;
  for (k = 0; k < 8; k = k + 4)
    {
      n = ((unsigned char) (j << k)) >> 4;   /* cast needed!  09-06-26 LK */
      switch(n)
	{
	case 15: char_out('F');
	  break;
	case 14: char_out('E');
	  break;
	case 13: char_out('D');
	  break;
	case 12: char_out('C');
	  break;
	case 11: char_out('B');
	  break;
	case 10: char_out('A');
	  break;
	case 9: char_out('9');
	  break;
	case 8: char_out('8');
	  break;
	case 7: char_out('7');
	  break;
	case 6: char_out('6');
	  break;
	case 5: char_out('5');
	  break;
	case 4: char_out('4');
	  break;
	case 3: char_out('3');
	  break;
	case 2: char_out('2');
	  break;
	case 1: char_out('1');
	  break;
	case 0:
	  char_out('0');
	  break;
	}
    }
}

void hex_out(long i)
{
  unsigned long j = (unsigned long) i;
  int print_zero_p = FALSE;
  short k, n;
  for (k = 0; k < 32; k = k + 4)
    {
      n = (short)((j << k) >> 28); /* YM, 2/23/96, (short) cast added */
      switch(n)
	{
	case 15: char_out('F');
	  break;
	case 14: char_out('E');
	  break;
	case 13: char_out('D');
	  break;
	case 12: char_out('C');
	  break;
	case 11: char_out('B');
	  break;
	case 10: char_out('A');
	  break;
	case 9: char_out('9');
	  break;
	case 8: char_out('8');
	  break;
	case 7: char_out('7');
	  break;
	case 6: char_out('6');
	  break;
	case 5: char_out('5');
	  break;
	case 4: char_out('4');
	  break;
	case 3: char_out('3');
	  break;
	case 2: char_out('2');
	  break;
	case 1: char_out('1');
	  break;
	case 0: if (print_zero_p || k >= 24)
	  char_out('0');
	}
      if (n != 0)
	print_zero_p = TRUE;
    }
}

/* no need for _s version, just writes to the buffer */

void fat_char_out (FAT_STR fc)
{
  if (CHARSET(fc) == 0)
    {
        AddCharToBuffer((char) CHARNUM(fc));
    }
  else
    {
      if (IY_UNICODE)
        {
          AddCharToBuffer('\\'); 
          AddCharToBuffer('u');
          hex_out(CHARSET(fc));
          hex_out(CHARNUM(fc));
        }
      else
        {
          AddCharToBuffer((char) CHARSET(fc));
          AddCharToBuffer((char) CHARNUM(fc));
        }
    }
}

/* This version writes to the output buffer */
void utf8_fat_char_out (FAT_STR fc)
{
	char buffer[7];
	int i;

  if ((CHARSET(fc) == 0) && (127 >= CHARNUM(fc)))	// Under 128, no need do decode
    {
        AddCharToBuffer((char) CHARNUM(fc));
    }
  else
    {
	
	  ConvertFatChartoUTF8 (fc, (UTF8 *) buffer, (UTF8*) buffer + 7, lenientConversion);
	  for (i=0; ( i < 7 ) && buffer[i] ; i++) 
	  {
		  AddCharToBuffer(buffer[i]);
	  }

	}
}

static void fat_char_out_print_esc (FAT_STR fc)
     /* Aimed for printing out visible characters */
{
  if (CHARSET(fc) == 0)
    {
       if (is_character_printable(CHARNUM(fc)))
	 AddCharToBuffer((char) CHARNUM(fc));
       else
	 {
	   AddCharToBuffer('\\');
	   AddCharToBuffer('x');
	   hex_out(CHARNUM(fc));
	 }
    }
  else
    {
      if (IY_UNICODE)
	{
	  AddCharToBuffer('\\'); 
	  AddCharToBuffer('u');
	  hex_out(CHARSET(fc));
	  hex_out(CHARNUM(fc));
	}
      else
	{
	  AddCharToBuffer((char) CHARSET(fc));
	  AddCharToBuffer((char) CHARNUM(fc));
	}
    }
}

void utf8_fat_char_out_print_esc (FAT_STR fc)
     /* Aimed for printing out visible characters, the others are printed as \x<hex code> */
{

	char buffer[7];
	int i;

	if (utf8_is_character_printable(fc)){
		ConvertFatChartoUTF8 (fc, (UTF8 *) buffer, (UTF8*) buffer + 7, lenientConversion);
		for (i=0; ( i < 7 ) && buffer[i] ; i++) 
		{
			AddCharToBuffer(buffer[i]);
		}
	}
	else
	{
	   AddCharToBuffer('\\');
	   AddCharToBuffer('x');
	   hex_out(CHARNUM(fc));
	}
}

void fat_char_out_esc (FAT_STR fc , char *esc_codes)
{
  if ((esc_codes != NULL) && (CHARSET(fc) == '\0'))
    {
      if (((CHARNUM(fc)) == '%') || (strchr(esc_codes, (CHARNUM(fc))) != NULL))
	switch (CHARNUM(fc))
	  {
	  case '\t':
	    AddStringToBuffer("\"\\t\"");
	    break;
	  case '\n':
	    AddStringToBuffer("\"\\n\"");
	    break;
	  case ' ':
	    AddStringToBuffer("\" \"");
	    break;
	  case '\r':
	    AddStringToBuffer("\"\\r\"");
	    break;
	  case '\v':
	    AddStringToBuffer("\"\\v\"");
	    break;
	  case '\b':
	    AddStringToBuffer("\"\\b\"");
	    break;
	  case '\f':
	    AddStringToBuffer("\"\\f\"");
	    break;
	  default:
	    AddCharToBuffer('"');
	    FST_fat_char_out(fc);
	    AddCharToBuffer('"');
	  }
      else
	FST_fat_char_out_print_esc(fc);
    }
  else
    FST_fat_char_out_print_esc(fc);
}

void utf8_fat_string_out (FAT_STR fs)
{ 
  for (; !(CHARSET(fs) == 0 && CHARNUM(fs) == 0);)
    utf8_fat_char_out(fs++);
}

void fat_string_out (FAT_STR fs)
{ 
  for (; !(CHARSET(fs) == 0 && CHARNUM(fs) == 0);)
    FST_fat_char_out(fs++);
}

void fat_string_out_esc (FAT_STR fs, char *esc)
{ 
  for (; !(CHARSET(fs) == 0 && CHARNUM(fs) == 0);)
    fat_char_out_esc(fs++, esc);
}


void symbol_out(FAT_STR name)
{
  /* Space, zero, newline, question mark, percent sign, tab */
  fat_string_out_esc(name, " 0\n?%	");
}

static void single_out(int id, LABELptr label, int escape_p)
{
  if (id == EPSILON)
    char_out('0');
  else if (id == OTHER)
    char_out('?');
  else if (id == BOUNDARY_SYMBOL)
    string_out(".#.");
  else
    {
      if (escape_p == DO_ESCAPE && fat_strlen(LABEL_atom(label)) == 1)
	fat_string_out_esc(LABEL_atom(label), " 0\n?%	\t\r\v\b\f");
      else
	FST_fat_string_out(LABEL_atom(label));
    }
}

static void tuple_out(TUPLEptr tuple, int16 arity, int escape_p)
{
  int16 i;
  id_type *labels = TUPLE_labels(tuple);
  
  if (FSM_MODE == TWOL_MODE)
    {
      if (labels[UPPER] != OTHER)
	single_out(labels[UPPER], id_to_label(labels[UPPER]), escape_p);
      AddCharToBuffer(':');
      if (labels[LOWER] != OTHER)
	single_out(labels[LOWER], id_to_label(labels[LOWER]), escape_p);
    }
  else
    {
      AddCharToBuffer('<');
      for (i = 0; i <  arity; i++)
	{
	  single_out(labels[i], id_to_label(labels[i]), escape_p);
	  if (i + 1 < arity)
	    AddCharToBuffer(':');
	}
      AddCharToBuffer('>');
    }
}

void label_out(id_type id, int escape_p)
{
  LABELptr label = id_to_label(id);
  
  switch (LABEL_arity(label))
    {
    case 1:
      single_out(id, label, escape_p);
      break;
    default:
      /* tuple */
      tuple_out(LABEL_tuple(label), LABEL_arity(label), escape_p);
      break;
    }
}


void one_side_out(id_type id, int side, int skip_epsilon_p, int escape_p)
{
  /* Prints the one side of a tuple label unless it is an EPSILON
     and SKIP_EPSILON_P is TRUE.
     */
  
  if (!(skip_epsilon_p && id == EPSILON))
    {
      LABELptr label = id_to_label(id);
      
      switch (LABEL_arity(label))
	{
	case 1:
	  single_out(id, label, escape_p);
	  break;
	default:
	  /* tuple */
	  id = TUPLE_labels(LABEL_tuple(label))[side];
	  if (!(skip_epsilon_p && id == EPSILON))
	    single_out(id, id_to_label(id), escape_p);
	  break;
	}
    }
}


void state_label_out(long int name, int final_p)
{
  if (final_p)
    char_out('f');
  char_out('s');
  int_out(name);
}



/***********************************
 *
 *  Printing alphabets
 *
 ************************************/

static void sigma_out_s(ALPHABETptr sig, int rm, int indent, 
			FILE *stream)
{
  if (ALPH_len(sig) > 0)
    {
      /* Check symbol packages, ignoring EPSILON, OTHER */
      int i, same_p = TRUE;
      id_type *items = ALPH_items(sig);
      
      for (i = 0 ; i < ALPH_len(sig); i++)
	if (items[i] == EPSILON || items[i] == OTHER)
	  {
	    label_out(items[i], DO_ESCAPE);
	    buffer_out_s(rm, indent, stream);
	  }
	else
	  {
	    if (same_p == TRUE)
	      label_out(items[i], DO_ESCAPE);
	    else
	      {
		symbol_out(LABEL_atom(id_to_label(items[i])));
	      }
	    buffer_out_s(rm, indent, stream);
	  }
      
      fresh_line_s(rm, indent, stream);
      
      /* If type has not been set, the alphabet is either empty
	 or consists of OTHER.
	 */
/*  Disconnected the printing of obsolete TYPE and PACKAGE information.
      if (type_id >= 0)
	{
	  word_out_s("Type:", rm, indent, stream);
	  string_out(type);
	  char_out(',');
	  buffer_out_s(rm, indent, stream);
	  if (type_id == Symbol)
	    {
	      word_out_s("Package:", rm, indent, stream);
	      fat_string_out(package);
	      char_out(',');
	    }
	  buffer_out_s(rm, indent, stream);
	}
*/        
    }
}

void alph_out_s(ALPHABETptr alph, int rm, int indent, FILE *stream)
{
  int i, len = ALPH_len(alph);
  id_type *items = ALPH_items(alph);
  
  if (ALPH_type(alph) == LABEL_VECTOR)
    for (i = 0; i < len; i++)
      {
	label_out(*items++, DO_ESCAPE);
	buffer_out_s(rm, indent, stream);
      }
  else
    for (i = 0; i < len; i++)
      if (*items++)
	{
	  label_out((id_type) i, DO_ESCAPE);
	  buffer_out_s(rm, indent, stream);
	}
}

void print_alphabet_s(ALPHABETptr alph, int rm, int indent, 
		      int sigma_p, FILE *stream)
{
  
  alph = copy_alphabet(alph);
  
  sort_alph(alph);
  
  if (sigma_p == TRUE)
    sigma_out_s(alph, rm, indent, stream);
  else
    alph_out_s(alph, rm, indent, stream);
  
  fresh_line_s(rm, indent, stream);
  word_out_s("Size:", rm, indent, stream);
  int_out((long) ALPH_len(alph));
  fresh_line_s(rm, indent, stream);
  free_alph(alph);
}

void print_labels_s(NETptr net, int rm, int indent, FILE *stream)
{
  print_alphabet_s(NET_labels(net), rm, indent, FALSE, stream);
}

void print_sigma_s(NETptr net, int rm, int indent, FILE *stream)
{ 
  int count;
  print_alphabet_s(sigma(net), rm, indent, TRUE, stream);

  count = print_net_flag_diacritics(net, stream);
  if (count > 0)
    {
      char *s = "s";

      if (count == 1)
	s = "";

      fsm_fprintf(stream, "%d flag diacritic%s\n", count, s);
    }
}

void print_alph_vector_s(ALPH_VECTORptr alph_vector, int rm, 
			 int indent, FILE *stream)
{ 
  int i, j, k, len = av_len(alph_vector);
  ALPHABETptr alph, *alphabets = av_alphs(alph_vector);
  buffer_out_s(rm, indent, stream);
  char_out('(');
  for (i = 0; i < len; i++)
    {
      alph = alphabets[i];
      k = ALPH_len(alph) - 1;
      if (i > 0)
	buffer_out_s(rm, indent, stream);
      char_out('(');
      for (j = 0; j <= k; j++)
	{
	  label_out(ALPH_item(alph,j), DO_ESCAPE);
	  if (j < k)
	    buffer_out_s(rm, indent, stream);
	  else
	    char_out(')');
	}
    }
  char_out(')');
  buffer_out_s(rm, indent, stream);
  fresh_line_s(rm, indent, stream);
}

/***********************************
 *
 *  Printing uncompacted nets
 *
 ************************************/

/*
 * static void print_arc_s(ARCptr arc, int rm, int indent, FILE *stream)
 * {
 *   STATEptr dest_state = ARC_destination(arc);
 *   long int dest_name = (long int) STATE_client_cell(dest_state);
 *   label_out(ARC_label(arc));
 *   string_out(" -> ");
 *   state_label_out(dest_name, STATE_final(dest_state));
 *   
 *   if (next_arc(arc) != NULL)
 *     char_out(',');
 *   else 
 *     char_out('.');
 *   
 *   buffer_out_s(rm, indent, stream);
 * }
 */

/*
 * static void print_arcs_s(ARCptr arcs, int rm, int indent, FILE *stream)
 * {
 *   ARCptr cur;
 *   
 *   if (arcs == NULL)
 *     word_out_s("(no arcs)", rm, indent, stream);
 *   else
 *     {
 *       for (cur = arcs; cur != NULL; cur = next_arc(cur))
 * 	print_arc_s(cur, rm, indent, stream);
 *     }
 *   
 *   fresh_line_s(rm, indent,stream);
 * }
 */

/***********************************
 *
 *  Printing timing statistics
 *
 ************************************/

void print_time(long start, long end)
{
  long sec, min;
  
  sec = end - start;
  min = sec / 60;
  
  if (min > 0)
    fsm_printf("%d min %d sec\n", (int) min, (int) (sec % 60));
  else
    fsm_printf("%d sec\n", (int) sec);
}


/***********************************
 *
 *  Printing flags
 *
 ************************************/

static void flag_out(char *flag, int rm, int indent, FILE *stream, int *count)
{
   (*count)--;
   
   if (*count > 0)
     {
       string_out(flag);
       word_out_s(",", rm, indent, stream);
     }
   else
     word_out_s(flag, rm, indent, stream);
}


void print_flags_s(NETptr net, int rm, int indent, FILE *stream)
{
  int count;
  
  count = NET_deterministic(net) + NET_pruned(net) + NET_completed(net) +
      NET_minimized(net) + NET_epsilon_free(net) + 
      NET_sorted_states(net) + NET_loop_free(net) +
      NET_visit_marks_dirty(net) + NET_names_matter(net) +
      NET_shared_arc_lists(net) + NET_has_arc_user_pointer(net) +
      NET_closed_sigma(net) + NET_compacted(net)+
      NET_upper_seq(net)+ NET_lower_seq(net) + NET_optimized(net) +
      NET_is_virtual(net) + NET_Kaplan_compressed(net);

  if (NET_deterministic(net))
    flag_out("deterministic", rm, indent, stream, &count);
  if (NET_pruned(net))
    flag_out("pruned", rm, indent, stream, &count);
  if (NET_completed(net))
    flag_out("completed", rm, indent, stream, &count);
  if (NET_minimized(net))
    flag_out("minimized", rm, indent, stream, &count);
  if (NET_epsilon_free(net))
    flag_out("epsilon_free", rm, indent, stream, &count);
  if (NET_sorted_states(net))
    flag_out("sorted_states", rm, indent, stream, &count);
  if (NET_loop_free(net))
    flag_out("loop_free", rm, indent, stream, &count);
  if (NET_visit_marks_dirty(net))
    flag_out("visit_marks_dirty", rm, indent, stream, &count);
  if (NET_shared_arc_lists(net))
    flag_out("shared_arc_lists", rm, indent, stream, &count);
  if (NET_has_arc_user_pointer(net))
    flag_out("has_arc_user_pointer", rm, indent, stream, &count);
  if (NET_closed_sigma(net))
    flag_out("closed_sigma", rm, indent, stream, &count);
  if (NET_compacted(net))
    flag_out("compacted", rm, indent, stream, &count);
  if (NET_upper_seq(net))
    flag_out("upper_sequentialized", rm, indent, stream, &count);
  if (NET_lower_seq(net))
    flag_out("lower_sequentialized", rm, indent, stream, &count);
  if (NET_left_bimachine(net))
    flag_out("left_bimachine", rm, indent, stream, &count);
  if (NET_right_bimachine(net))
    flag_out("right_bimachine", rm, indent, stream, &count);
  if (NET_reduced_labelset(net))
    flag_out("reduced_labelset", rm, indent, stream, &count);
  if (NET_is_virtual(net))
    flag_out("virtual", rm, indent, stream, &count);
  if (NET_Kaplan_compressed(net))
    flag_out("Kaplan compressed", rm, indent, stream, &count);
  if (NET_optimized(net))
    flag_out("optimized", rm, indent, stream, &count);
  
  fresh_line_s(rm, indent, stream);
  
  string_out("Arity:");
  buffer_out_s(rm, indent, stream);
  int_out(NET_arc_label_arity(net));
  fresh_line_s(rm, indent, stream);
}


/***********************************
 *
 *  Printing size
 *
 ************************************/

/*
 * static char *s_or_null(unsigned long ln)
 * {
 *   if (ln == (unsigned long) 1)
 *     return("");
 *   else
 *     return("s");
 * }
 */

/***********************************
 *
 *  Printing objects on property lists
 *
 ************************************/

void byte_block_out(BYTE_BLOCKptr byte_block, int rm, int indent, FILE *stream)
{
  unsigned long i, len = BYTE_BLOCK_len(byte_block);
  char *block = (char *) BYTE_BLOCK_block(byte_block);
  
  for (i = 0; i < len; i++, block++)
    {
      hex_out_byte(*block);
      exact_buffer_out_s(rm, indent, stream);
    }
}


void object_out_s(OBJECTptr obj, int rm, int indent, FILE *stream)
{
  switch(OBJ_type(obj))
    {
    case(Int):
      /* type 0 */
      int_out((long) OBJ_integer(obj));
      break;
    case(Character):
      /* type 1 */
      char_out('\'');
      FST_fat_char_out(&OBJ_character(obj));
      char_out('\'');
      break;
    case(String):
      /* type 2 */
      char_out('\"');
      fat_string_out(OBJ_str(obj));
      char_out('\"');
      break;
    case(Symbol):
	/* type 3 */
	symbol_out(IO_SYM_name(OBJ_symbol(obj)));
	break;
    case(Array):
	/* type 4 */
	AddCharToBuffer('<');
	sequence_out_s(OBJ_array(obj), rm, indent, stream);
	AddCharToBuffer('>');
	break;
    case(List):
	/* type 5 */
	AddCharToBuffer('(');
	sequence_out_s(OBJ_list(obj), rm, indent, stream);
	AddCharToBuffer(')');
	break;
    case(Byte_block):
      string_out("\\x");
      byte_block_out(OBJ_byte_block(obj), rm, indent, stream);
      break;
    default:
      string_out("<unprintable object of type ");
      int_out((long) OBJ_type(obj));
      char_out('>');		
      break;
    }
  buffer_out_s(rm, indent, stream);
}

void print_object_s(OBJECTptr obj, int rm, int indent, FILE *stream)
{
  object_out_s(obj, rm, indent, stream);
  fresh_line_s(rm, indent, stream);
}

void sequence_out_s(SEQUENCEptr s, int rm, int indent, FILE *stream)
{
  int i, last = SEQ_len(s) -1;
  
  for (i = 0; i <= last; i++)
    {
      switch(SEQ_type(s))
	{
	case (Object):
	  /* type -1 */
	  object_out_s(SEQ_objects(s)[i], rm, indent, stream);
	  break;	
	case (Int):
	  /* type 0 */
	  int_out((long) SEQ_ints(s)[i]);
	  break;
	case (Character):
	  /* type 1 */
	  FST_fat_char_out(&SEQ_characters(s)[i]);
	  break;
	case (String):
	  /* type 2 */
	  fat_string_out(SEQ_strings(s)[i]);
	  break;
	case (Symbol_package):
	  /* type -3 */				
   	  string_out("::");
	  fat_string_out(SYM_PACK_sym(SEQ_symbol_package(s), i));
	  break;
	case (Symbol):
	  /* type 3 */
	  symbol_out(IO_SYM_name(SEQ_symbols(s) + i));
   	  break;
	case (Array):
	  /* type 4 */
	  AddCharToBuffer('<');
	  sequence_out_s(SEQ_array(s)[i], rm, indent, stream);
	  AddCharToBuffer('>');
	  break;
	case (List):
	  /* type 5 */
	  AddCharToBuffer('(');
	  sequence_out_s(SEQ_list(s)[i], rm, indent, stream);
	  AddCharToBuffer(')');
	  break;
   case (Byte_block):
      /* type 6 */
      string_out("\\x");
      byte_block_out(SEQ_byte_block(s)[i], rm, indent, stream);
      break;      
	default:
	  handle_error("Illegal sequence type", "sequence_out", SEQ_type(s));
	}
      if (i < last)
	buffer_out_s(rm, indent, stream);
    }
}

void net_name_out(NETptr net)
{
  OBJECTptr name = get_value(net, NETWORKNAME_ATTR);

  if (name != NULL && OBJ_type(name) == String)
    fat_string_out(OBJ_str(name));
}

void print_props_s(NETptr net, int rm, int indent, FILE *stream)
{
  PROPptr prop;
  int new_indent;

  net_name_out(net);
  fresh_line_s(rm, indent, stream);

  for (prop = NET_properties(net); prop != NULL; prop = next_prop(prop))
    {
      fat_string_out(PROP_attr(prop));
      char_out(':');
      new_indent = indent + cur_pos + 2;
      buffer_out_s(rm, new_indent, stream);
      object_out_s(PROP_val(prop), rm, new_indent, stream);
      fresh_line_s(rm, indent, stream);
    }
}
#endif /* SLIM_FST */
