/* $Id: bin-out.c,v 1.4 2005/03/04 15:45:21 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "fst/bin-out.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#include "fst/machine.h"
#include "fst/fsm_io.h"
#include "fst/utf8_fatstr_util.h"

#ifdef INFL_APPLICATION
#include "fst/infl-utl.h"
#endif

#ifndef TWOLC_APPLICATION
#ifndef XIS_APPLICATION
#include "fst/vectorize.h"
#endif
#endif

#ifdef MAC_OS
#include <unix.h>
#endif /* MAC_OS */

/******************************************************
 **
 **  BIN-OUT.C
 **
 **  Lauri Karttunen, RXRC, October 1992
 **
 **  Binary output functions for FSMs.
 **  
 **
 ******************************************************/

/***************************
 *                         *
 *   global declarations   *
 *                         *
 ***************************/

/* STANDARD ATTRIBUTES */
char *STANDARD_ATTRIBUTES[STANDARD_ATTRIBUTE_COUNT] = 
{"COPYRIGHT",
   "AUTHORS",
   "CHARENCODING",
   "SOURCE",
   "LANGUAGE",
   "CREATION-DATE",
   "VERSION"};
char *STANDARD_ATTRIBUTE_VALUES[STANDARD_ATTRIBUTE_COUNT] = 
{NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/* whether to print status bar when writing or not */
static int DISPLAY_WRITE_STATUS_BAR = 1;/* default on */
static long STATUS_BAR_INCREMENT = 0;

extern char *COPYRIGHT_OWNER;
     

/**********************************
 *                                *
 * EDITOR FOR STANDARD ATTRIBUTES *
 *                                *
 **********************************/

static void print_standard_attributes(void)
{
  int i, indent = 0;
  size_t longest = 0;
  
  for (i = 0; i < STANDARD_ATTRIBUTE_COUNT; i++)
    {
      if (strlen(STANDARD_ATTRIBUTES[i]) > longest)
	longest = strlen(STANDARD_ATTRIBUTES[i]);
    }
  
  for (i = 0; i < STANDARD_ATTRIBUTE_COUNT; i++)
    {
      fsm_printf("%d. %s: ", i+1, STANDARD_ATTRIBUTES[i]);
      
      indent = longest - strlen(STANDARD_ATTRIBUTES[i]);
      while (indent > 0)
	{
	  fsm_printf(" ");
	  indent--;
	}
      
      if (STANDARD_ATTRIBUTE_VALUES[i])
	fsm_printf("%s\n", STANDARD_ATTRIBUTE_VALUES[i]);
      else
	fsm_printf("(none)\n");
    }
}

void edit_standard_attributes(int clear_p)
{
  int num = 0, len;
  char buf[1000];
  
  print_standard_attributes();			
  while (num < 1 || num > STANDARD_ATTRIBUTE_COUNT)
    {
      fsm_printf("Please type a number (1-%d) [cancel]:  ",
		 STANDARD_ATTRIBUTE_COUNT);
      fflush(stdout);
      _fsm_fgets(buf, 999, stdin);
      fflush(stdin);
      if (buf[0] == '\0')
	{
	  fsm_printf("Cancelled.\n");
	  return;
	}
      else
	num = atoi(buf);
    }	
  num--;
  
  if (clear_p == TRUE)
    {
      free(STANDARD_ATTRIBUTE_VALUES[num]);
      STANDARD_ATTRIBUTE_VALUES[num] = NULL;
      fsm_printf("Cleared %s.\n", STANDARD_ATTRIBUTES[num]);		
    }
  else
    {
      fsm_printf("%s selected.\n", STANDARD_ATTRIBUTES[num]);
      fsm_printf("Old value: ");
      if (STANDARD_ATTRIBUTE_VALUES[num])
	fsm_printf("%s\n", STANDARD_ATTRIBUTE_VALUES[num]);
      else
	fsm_printf("(none)\n");
      
      fsm_printf("New value [cancel]: ");
      fflush(stdout);
      _fsm_fgets(buf, 999, stdin);
      fflush(stdin);
      
      len = strlen(buf);
      if (len > 0)
	{
	  STANDARD_ATTRIBUTE_VALUES[num] = (char *)
	    malloc((size_t) sizeof(char) * len + 1);
	  strcpy(STANDARD_ATTRIBUTE_VALUES[num], buf);
	  fsm_printf("Done.\n");
	}
      else
	fsm_printf("Cancelled.\n");
    }
}

void add_standard_attributes(NETptr net)
     /*****
      *
      * Installs the STANDARD_ATTRIBUTES with non-null 
      * values as NET_properties of NET.
      *
      * Call this before writing out a net that should
      * have these properties.
      *
      ****/
{
  int i;
  
  for (i = 0; i < STANDARD_ATTRIBUTE_COUNT; i++)
    {
      if (STANDARD_ATTRIBUTE_VALUES[i])
	{
	  add_string_prop(net, FST_thin_to_fat(STANDARD_ATTRIBUTES[i]), 
			  FST_thin_to_fat(STANDARD_ATTRIBUTE_VALUES[i]));
	}
    }
}



/************************************
 *                                  *
 *   binary i/o utilities: writing  *
 *                                  *
 ************************************/


inline
  static int number_size(long int number)
{
  register int bit_count = 1;
  
  while ((number >>= 1))
    bit_count++;
  
  return bit_count;
}

static void write_number(long int number, FILE *stream,
			 unsigned long *byte_countp)
{
  int size, offset, negative;
  unsigned long int n;
  
  if (number < 0)
    {
      number *= -1;
      negative = 64;
    }
  else
    negative = 0;
  
  size = number_size(number);
  
  if (size > 6)
    {
      offset = 7 * (size  / 7);
      n = number >> offset;
      number ^= n << offset;
      
      write_byte((fst_byte) (include_next + negative + n), stream, byte_countp);
      
      while (offset > 7)
        {
          offset -= 7;
          n = number >> offset;
          number ^= n << offset;
          write_byte((fst_byte) (include_next + n), stream, byte_countp);
        }
      write_byte((fst_byte) number, stream, byte_countp);
    }
  else
    write_byte ((fst_byte) (negative + number), stream, byte_countp);
}


static void write_label(ARCptr arc, FILE *stream, unsigned long *byte_countp)
{
  long int l = ARC_label(arc);
  int flags = 0;
  int size, offset;
  unsigned long int n;

  /* Number of bits needed to recode the label. */

  size = number_size(l);
  
  if (STATE_final(ARC_destination(arc)) == TRUE)
    flags += dest_is_final;
  
  if (next_arc(arc) == NULL)
    flags += last_arc_in_set;
  
  if (size > 4)
    {
      /* If the label size is greater than 4 but less than 12 bits
         we shift it to the right by multiples of 7 bits. The first
         four bits (possibly all zero) are included in the first
         label byte, the next 7 bits in the first continuation byte,
         and so on. This means that label id 0-15 are stored as
         one byte; ids 16-2047 take two bytes; labels whose id is
         between 2048 and 32768 are recorded as three bytes. The
         32768 limit is because label id's are 16 bit positive
         integers.
         */

      offset = 7 * ((size + 2) / 7);
      n = l >> offset;
      l ^= n << offset;
      
      write_byte((fst_byte) (type_is_label + include_next + flags + n), stream,
		 byte_countp);
      
      while (offset > 7)
        {
          offset -= 7;
          n = l >> offset;
          l ^= n << offset;
          write_byte((fst_byte) (include_next + n), stream, byte_countp);
        }
      write_byte((fst_byte) l, stream, byte_countp);
    }
  else
    write_byte ((fst_byte) (type_is_label + flags + l), stream, byte_countp);
}

/*
static void old_write_fat_str(FAT_STR s, FILE *stream,
			      unsigned long *byte_countp)
{
    Formerly called 'write_fat_str', now replaced by the new
    version below. This function was in use up to file-type 21.
    It writes fat strings in clear text format.
    Decommissioned 2002-05-20 by LK to make the file format less
    transparent and more difficult to decypher.

  int l, i;
  
  l = fat_strlen(s);

  write_number((long int) l, stream, byte_countp);
  for (i = 0; i < l; i++)
    {
      write_byte((fst_byte) CHARSET(s + i), stream, byte_countp);
      write_byte((fst_byte) CHARNUM(s + i), stream, byte_countp);
    }
}
*/

static long encrypt_uint16(uint16 code, unsigned long *byte_countp)
{
  int offset = *byte_countp & 127;

  if (offset >= 64)
    return(code - offset);
  else
    return(code + offset);
}

static void write_fat_str(FAT_STR s, FILE *stream, unsigned long *byte_countp,
			  int old_format_p)
{
  /* This function replaces the old one above. Starting with file type
     NEW_STANDARD_FILE_TYPE (== 21), fat strings are written as a
     sequence of variable length integers using a function that
     previously was only used for integers. To make the format more
     difficult to crack, the unsigned 16-bit numbers are masked with
     an offset that varies depending on the byte position in
     the file where the string begins. The offset ranges from 0
     to 127 and it is added to or subtracted from the code depending
     on whether it is above or below 64. On Intel platform, the
     byte order of "fat characters" must be switched before
     encryption.
  */

  int l, i;
  int16 code;
  FAT_CHAR fc[1];

  l = fat_strlen(s);

  if (old_format_p)
    {
      write_number((long int) l, stream, byte_countp);
      for (i = 0; i < l; i++)
	{
	  write_byte((fst_byte) CHARSET(s + i), stream, byte_countp);
	  write_byte((fst_byte) CHARNUM(s + i), stream, byte_countp);
	}
    }
  else
    {
      write_number(encrypt_uint16((uint16)l, byte_countp), stream, byte_countp);

      for (i = 0; i < l; i++)
	{
#if !defined(_M_IX86) && !defined(ARM)    /* Sparc, PowerPC */
	  CHARNUM(fc) = CHARNUM(s + i);
	  CHARSET(fc) = CHARSET(s + i);
#else                                     /* Intel, ARM */
	  CHARSET(fc) = CHARNUM(s + i);
	  CHARNUM(fc) = CHARSET(s + i);
#endif
	  code = encrypt_uint16(*((uint16 *) fc), byte_countp);
	  write_number(code, stream,byte_countp);
	}
    }
}

static void write_byte_block(BYTE_BLOCKptr byte_block, FILE *stream,
	unsigned long *byte_countp)
{
  size_t n;
  unsigned long len = BYTE_BLOCK_len(byte_block);
  
  write_number((long int) len, stream, byte_countp);
   
  n = fwrite(BYTE_BLOCK_block(byte_block), (size_t) 1, (size_t) len, stream);

  *byte_countp += n;
  
  if ((unsigned long) n != len)
    handle_warning("Write error", "WRITE_BLOCK", n);

}


static void write_sequence(SEQUENCEptr s, FILE *stream,
			   unsigned long *byte_countp, int old_format_p);

static void write_object(OBJECTptr obj, FILE *stream,
			 unsigned long *byte_countp, int old_format_p)
{
  write_number(OBJ_type(obj), stream, byte_countp);
  switch (OBJ_type(obj))
    {
      case(Int):
        /* type 0 */
        write_number(OBJ_integer(obj), stream, byte_countp);
      break;
      case(Character):
        /* type 1 */
        write_byte((fst_byte) CHARSET(&OBJ_character(obj)), stream, byte_countp);
	write_byte((fst_byte) CHARNUM(&OBJ_character(obj)), stream, byte_countp);
      break;
      case(String):
        /* type 2 */
        write_fat_str(OBJ_str(obj), stream, byte_countp, old_format_p);
      break;
      case(Symbol):
        /* type 3 */
      write_fat_str(IO_SYM_name(OBJ_symbol(obj)), stream, byte_countp,
		    old_format_p);
      break;
      case(Array):
        /* type 4 */
        write_sequence(OBJ_array(obj), stream, byte_countp, old_format_p);
      break;
      case(List):
        /* type 5 */
        write_sequence(OBJ_list(obj), stream, byte_countp, old_format_p);
      break;
      case(Byte_block):
        /* type 6 */
        write_byte_block(OBJ_byte_block(obj), stream, byte_countp);
        break;
    default:
      handle_error("Illegal object type", "write_object", OBJ_type(obj));
      break;
    }
}

static void write_sequence(SEQUENCEptr s, FILE *stream,
			   unsigned long *byte_countp, int old_format_p)
{
  int i;
  
  write_number(SEQ_len(s), stream, byte_countp);
  write_number(SEQ_type(s), stream, byte_countp);
  
  for (i = 0; i < SEQ_len(s); i++)      /* fill sequence with data */
    {
      switch(SEQ_type(s))
        {
        case (Object):
          /* type -1 */
          write_object(SEQ_objects(s)[i], stream, byte_countp, old_format_p);
          break;                
        case (Int):
          /* type 0 */
          write_number(SEQ_ints(s)[i], stream, byte_countp);
          break;
        case (Character):
          /* type 1 */
          write_byte((fst_byte) CHARSET(&SEQ_characters(s)[i]), stream,
		     byte_countp);
          write_byte((fst_byte) CHARNUM(&SEQ_characters(s)[i]), stream,
		     byte_countp);
          break;
        case (String):
          /* type 2 */
          write_fat_str(SEQ_strings(s)[i], stream, byte_countp, old_format_p);
          break;
        case (Symbol_package):
          /* type -3 */
          write_fat_str(SYM_PACK_sym(SEQ_symbol_package(s), i), stream,
			byte_countp, old_format_p);
          break;
        case (Symbol):
          /* type 3 */
          write_fat_str(IO_SYM_name(SEQ_symbols(s) + i), stream, byte_countp,
			old_format_p);
          break;
        case (Array):
          /* type 4 */
          write_sequence(SEQ_array(s)[i], stream, byte_countp, old_format_p);
          break;
        case (List):
          /* type 5 */
          write_sequence(SEQ_list(s)[i], stream, byte_countp, old_format_p);
          break;
	case (Byte_block):
	  write_byte_block(SEQ_byte_block(s)[i], stream, byte_countp);
	  break;
        default:
          handle_error("Illegal sequence type", "write_sequence", SEQ_type(s));
          /* type 5 */                  
          break;
        }
    }
}


/******************************
 *                            *
 * WRITING AN FSM TO A STREAM *
 *                            *
 ******************************/

TALLYptr SYMBOL_TALLY = NULL;
static int SYMBOL_TALLY_COUNT;
static id_type MAX_LABEL_ID;

/*******************************
 *                             *
 * Prepare net for writing out *
 *                             *
 *******************************/

TALLYptr make_symbol_tally_export(int size)
{
  TALLYptr symb_tally = (TALLYptr) calloc((size_t) size,
					  (size_t) sizeof(TALLYcell));

  if (symb_tally == NULL)
    not_enough_memory("MAKE_TALLY_FILE");

  return symb_tally;
}

static void make_symbol_tally(int size)
{
  /*  
      SYMBOL_TALLY = (TALLYptr) calloc((size_t) size,
      (size_t) sizeof(TALLYcell));
  
      if (SYMBOL_TALLY == NULL)
      not_enough_memory("MAKE_TALLY_FILE");
  */

  /* Free the previous SYMBOL_TALLY if it exists. */

  if (SYMBOL_TALLY)
    free((void*) SYMBOL_TALLY);

  SYMBOL_TALLY = make_symbol_tally_export(size);
  
}

static void add_to_tally(id_type id)
{
  if (TALLY_freq(SYMBOL_TALLY + id) == 0)
    SYMBOL_TALLY_COUNT++;
        
  if (id > MAX_LABEL_ID)
    MAX_LABEL_ID = id;

  TALLY_freq(SYMBOL_TALLY + id)++;
}

void tally_labels(NETptr net)
     /***********
      *
      * Counts states and arcs, tallys the occurences of each symbol.
      * clear_all_marks clears the visit marks left by this function.
      *
      ***********/
{ 
  STATEptr state;
  ARCptr arc;
  AVptr arc_vector;
  int i, tally_size = LIM_id_count(LABEL_MAP);
  
  MAX_LABEL_ID = 0;

  make_symbol_tally(tally_size);
  
  for (i = 0; i < tally_size; i++)
    TALLY_label(SYMBOL_TALLY + i) = i;
  
  SYMBOL_TALLY_COUNT = 0;
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    if (STATE_vector_p(state))
      {
	arc_vector = STATE_arc_vector(state);

	for (i = 0; i < NET_arc_vector_len(net); i++)
	  if (AV_dest(arc_vector, i))
	    add_to_tally((uint16)i);
      }
    else
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
	add_to_tally(ARC_label(arc));
}

TALLYptr make_tally_sigma(NETptr net, 
			  int * sigma_tally_count,
			  id_type * max_sigma_id)
     /***********
      *
      * Counts states and arcs, tallys the occurences of each symbol.
      * clear_all_marks clears the visit marks left by this function.
      *
      * TG: counts upper and lower id-s separately.
      *
      ***********/
{ 
  STATEptr state;
  ARCptr arc;

  int i, tally_size = LIM_id_count(LABEL_MAP);
  id_type id, up, low;
  TALLYptr sigma_tally = make_symbol_tally_export(tally_size);

  *max_sigma_id = 0;

  for (i = 0; i < tally_size; i++)
    TALLY_label(sigma_tally + i) = i;
  
  *sigma_tally_count = 0;
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
      {
	id = ARC_label(arc);

	if (LABEL_arity(id_to_label(id)) == 1)
	  {
	    if (TALLY_freq(sigma_tally + id) == 0)
	      ( * sigma_tally_count )++;
        
	    if (id > (* max_sigma_id))
	      (* max_sigma_id) = id;

	    TALLY_freq(sigma_tally + id)++;
	  }
	else
	  {
	    up = upper_id(id);
	    low = lower_id(id);

	    if (up != EPSILON)
	      {
		if (TALLY_freq(sigma_tally + up) == 0)
		  ( * sigma_tally_count )++;
		if (up > (* max_sigma_id))
		  (* max_sigma_id) = up;
		TALLY_freq(sigma_tally + up)++;
	      }

	    if (low != EPSILON)
	      {
		if (TALLY_freq(sigma_tally + low) == 0)
		  ( * sigma_tally_count )++;	
		if (low > (* max_sigma_id))
		  (* max_sigma_id) = low;
		TALLY_freq(sigma_tally + low)++;	
	      }
	  }
      }
  return sigma_tally;
}

static void prepare_states(NETptr net)
{
  /* Count states and arcs and set state client cells to -1
     as an indication that the state has not been processed.
     */
  
  STATEptr state;
  ARCptr arc;
  
  long arc_count = 0;
  long state_count = 0;
  
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      STATE_client_cell(state) = (void *) -1;
      state_count++;
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
        arc_count++;
    }
  
  if (state_count != NET_num_states(net))
    {
      fsm_fprintf(stderr, "*** Programmer alert: Wrong state count.\n");
      NET_num_states(net) = state_count;
    }
  
  if (arc_count != NET_num_arcs(net))
    {
      fsm_fprintf(stderr, "*** Programmer alert: Wrong arc count.\n");
      NET_num_arcs(net) = arc_count;
    }
  
}

/****************************************************************************
 *                                                                          *
 *                Optimizer for writing out FSMs                            *
 *                                                                          *
 * sorts arc labels so that the most frequent ones have the lowest values.  *
 * low numbers are smaller in the file, thus sorting saves storage space.   *
 *                                                                          *
 ****************************************************************************/

int compare_label_freq (const void *a, const void *b)
{
  /* If two labels have the same frequency, give precedence to
     the label with the smaller id, to guarantee a unique order.
     */
  
  if (TALLY_freq((TALLYcell *) b) == TALLY_freq((TALLYcell *) a))
    {
      if (TALLY_label((TALLYcell *) a) < TALLY_label((TALLYcell *) b))
	return(-1);
      else if (TALLY_label((TALLYcell *) b) == TALLY_label((TALLYcell *) a))
	{
          /*  Perhaps this can happen in some implementation of
              qsort without being an error. */
	  /*
	    handle_error("Duplicate label id!", "COMPARE_LABEL_FREQ",
	    TALLY_label((TALLYcell *) a));
	    */
	  return (0);
	}
      else
	return (1);
    }
  else if (TALLY_freq((TALLYcell *) b) > TALLY_freq((TALLYcell *) a))
    return(1);
  else
    return (-1);
}

/* comparison function for ids - id_type - when tally may contain pairs TG */
int compare_label_freq_symbols (const void *a, const void *b)
{
  /* NOT TESTED YET 30.3.98
     Beware: even if the net is an FSM the tally array may contain
     symbol pairs.
     If two labels have the same frequency, give precedence to
     the label with the smaller id, to guarantee a unique order.
     */
 
  if (TALLY_freq((TALLYcell *) b) == 0 &&
      TALLY_freq((TALLYcell *) a ) !=0)
    return -1;
  if (TALLY_freq((TALLYcell *) a) == 0 &&
      TALLY_freq((TALLYcell *) b) !=0)
    return 1;

  if (LABEL_arity(id_to_label ( (id_type) (TALLY_label((TALLYcell *) b))) ) > 1
      && 
      LABEL_arity(id_to_label ( (id_type) (TALLY_label((TALLYcell *) a)))) == 1)
     return -1;

   if (LABEL_arity(id_to_label ( (id_type) (TALLY_label((TALLYcell *) a)))) > 1
       && 
       LABEL_arity(id_to_label ( (id_type) (TALLY_label((TALLYcell *) b)))) == 1)
     return 1;
   else if 
     (LABEL_arity(id_to_label ( (id_type) (TALLY_label((TALLYcell *) a)))) > 1
      && 
      LABEL_arity(id_to_label ( (id_type) (TALLY_label((TALLYcell *) b)))) > 1)
     return 0;
   else
     return  compare_label_freq(a, b);
}

/* comparison for ids - id_type: symbols come before tags TG */
int compare_label_freq_symbols_precede_tags (const void *a, const void *b)
{
  /* Give precedence to single symbols first rather than labels.
     Beware: even if the net is an FSM the tally array may contain
     symbol pairs.
     If two labels have the same frequency, give precedence to
     the label with the smaller id, to guarantee a unique order.
     */
 
  if (TALLY_freq((TALLYcell *) b) == 0 &&
      TALLY_freq((TALLYcell *) a ) !=0)
    return -1;
  if (TALLY_freq((TALLYcell *) a) == 0 &&
      TALLY_freq((TALLYcell *) b) !=0)
    return 1;

  if (LABEL_arity(id_to_label ( (id_type) (TALLY_label((TALLYcell *) b)))) > 1
      && 
      LABEL_arity(id_to_label ( (id_type) (TALLY_label((TALLYcell *) a)))) == 1)
     return -1;

   if (LABEL_arity(id_to_label ( (id_type) (TALLY_label((TALLYcell *) a)))) > 1
       && 
       LABEL_arity(id_to_label ( (id_type) (TALLY_label((TALLYcell *) b)))) == 1)
     return 1;
   else if 
     (LABEL_arity(id_to_label ( (id_type) (TALLY_label((TALLYcell *) a)))) > 1
      && 
      LABEL_arity(id_to_label ( (id_type) (TALLY_label((TALLYcell *) b)))) > 1)
     return 0;

  if ( id_tag_p ( (id_type) (TALLY_label((TALLYcell *) b)) ) &&
        ( ! id_tag_p ((id_type) (TALLY_label((TALLYcell *) a)) )))
    return -1;
  if  ( id_tag_p ( (id_type) (TALLY_label((TALLYcell *) a)) ) &&
        ( ! id_tag_p ( (id_type) (TALLY_label((TALLYcell *) b)) )))
    return 1;
  else
    return  compare_label_freq(a, b);
}

static void sort_tally(void)
{
  QSORT((void *) SYMBOL_TALLY, (size_t) (MAX_LABEL_ID + 1),
        (size_t) sizeof(TALLYcell), compare_label_freq);
}

void sort_this_tally(TALLYptr tally_sigma, id_type max_sigma_id,
			    int comp_fn (const void *a, const void *b))

{
  QSORT((void *) tally_sigma, (size_t) (max_sigma_id + 1),
        (size_t) sizeof(TALLYcell), comp_fn);
}

void print_tally(FILE * ofile)
{
  int i = 0, j, k, len;
  long int freq = 0;
  
  sort_tally();
  while (i < SYMBOL_TALLY_COUNT)
    {
      for (j = 0; j < 3 && i < SYMBOL_TALLY_COUNT; j++, i++)
        {
          len = 12;
          freq = TALLY_freq(SYMBOL_TALLY + i);
          fsm_fprintf (ofile, "%4d.%6ld ", i, freq);
          len += print_label((id_type) TALLY_label(SYMBOL_TALLY + i), ofile, DO_ESCAPE);
          if (freq == 0)
            break;
          if (j < 2)
            for (k = 25; k > len; k--)
              fsm_putc(' ', ofile);
        }
      fsm_putc('\n', ofile);
      if (freq == 0)
        break;
    }
}

void print_this_tally(FILE * ofile,
		      TALLYptr tally_this, 
		      int this_tally_count,
		      id_type max_this_tally_id,
		      int comp_fn (const void *a, const void *b))
{
  int i = 0, j, k, len;
  long int freq = 0;
  
  sort_this_tally(tally_this, max_this_tally_id,  comp_fn);
  while (i < this_tally_count)
    {
      for (j = 0; j < 3 && i < this_tally_count; j++, i++)
        {
          len = 12;
          freq = TALLY_freq(tally_this + i);
          fsm_fprintf (ofile, "%4d.%6ld ", i, freq);
          len += print_label((id_type) TALLY_label(tally_this + i), ofile, DO_ESCAPE);
	  if (freq == 0)
            break;
          if (j < 2)
            for (k = 25; k > len; k--)
              fsm_putc(' ', ofile);
        }
      fsm_putc('\n', ofile);
      if (freq == 0)
        break;
    }
}

void optimize_labels(NETptr net, ALPHABETptr *optimized_map)
{
  int i, full_tally_count;
  ALPHABETptr map;
  
  /* Always include EPSILON and OTHER in the optimization, even
     if they do not appear as arc labels in NET.
     10.9.92 Why the special treatment of EPSILON and other?
     */
  
  tally_labels(net);
  full_tally_count = SYMBOL_TALLY_COUNT;
  
  if (TALLY_freq(SYMBOL_TALLY + EPSILON) == 0)  
    full_tally_count++; /* add 1 for EPSILON */
  if (TALLY_freq(SYMBOL_TALLY + OTHER) == 0)            
    full_tally_count++; /* add 1 for OTHER   */
  
  sort_tally();
  
  map = make_alph(LIM_id_count(LABEL_MAP), LABEL_VECTOR);       
  
  *optimized_map = map;
  
  for (i = 0; i < LIM_id_count(LABEL_MAP); i++)
    ALPH_item(map, i) = ID_NO_SYMBOL;
  
  for (i = 0; i < full_tally_count; i++)
    ALPH_item(map,TALLY_label(SYMBOL_TALLY + i)) = i;
  
  renumber_arc_labels(net, map);
}

static void restore_tally(NETptr net, ALPHABETptr *optimized_map)
{
  ALPHABETptr map;
  int i, len;
  
  if (NET_compacted(net) && NET_recode_key(net) != NULL)
    make_symbol_tally(LIM_id_count(LABEL_MAP));
  else
    handle_error("NET is not compacted", "RESTORE_TALLY", 0);
  
  if (*NET_arc_block(net) == 99 || *NET_arc_block(net) == 100)
    {
      map = NET_labels(net);
      TALLY_label(SYMBOL_TALLY + ALPH_len(map)) = EPSILON;
      TALLY_label(SYMBOL_TALLY + ALPH_len(map) + 1) = OTHER;
    }
  else
    map = NET_recode_key(net);
  
  len = ALPH_len(map);
  
  for (i = 0; i < len; i++)
    TALLY_label(SYMBOL_TALLY + i) = ALPH_item(map, i);
  
  SYMBOL_TALLY_COUNT = ALPH_len(NET_labels(net));
  
  if (!NET_epsilon_free(net))
    SYMBOL_TALLY_COUNT++;
  
  *optimized_map = NET_recode_key(net);
}

/********************************************
 *                                          *
 * FUNCTIONS FOR WRITING AN FSM TO A STREAM *
 *                                          *
 ********************************************/

static long int network_flags_to_number(NETptr net)
{
  long int b = 0;
  
  if (NET_deterministic(net))
    b |= 1;
  if (NET_pruned(net))
    b |= 2;
  if (NET_completed(net))
    b |= 4;
  if (NET_minimized(net))
    b |= 8;
  if (NET_epsilon_free(net))
    b |= 16;
  if (NET_loop_free(net))
    b |= 32;
  if (NET_twol_net(net))
    b |= 64;
  /* This used to be EQ_LABELS property. */
  if (NET_closed_sigma(net))
    b |= 128;
  /* Don't try to check the finality of the state when
     the net is compacted.
     */
  if (NET_start_state_final(net) ||
      (!NET_compacted(net) && STATE_final(NET_start_state(net))))
    b |= 256;
  if (NET_compacted(net))
    b |= 512;
  if (NET_sorted_arcs(net))
    b |= 1024;
  if (NET_tabular(net))
    b |= 2048;
  if (NET_upper_seq(net))
    b |= 4096;
  if (NET_lower_seq(net))
    b |= 8192;
  if (NET_left_bimachine(net))
    b |= 16384;
  if (NET_right_bimachine(net))
    b |= 32768;
  if (NET_reduced_labelset(net))
    b |= 65536;
  if (NET_is_virtual(net))
    b |= (1<<17);
  if (NET_optimized(net))
    b |= (1<<18);
  if (NET_names_matter(net) && NET_compacted(net))
    b |= (1<<19);
  return b;
} /* network_flags_to_number */


static void write_props(FILE *stream, unsigned long *byte_countp, NETptr net,
			int old_format_p)
{
  PROPptr props;
  int count, i;
  
  for (count = 0, props = NET_properties(net);
       props != NULL;
       count++, props = next_prop(props));
  write_number ((long int) count, stream, byte_countp);
  
  for (i = 0, props = NET_properties(net);
       i < count;
       i++, props = next_prop(props))
    {
      write_fat_str(PROP_attr(props), stream, byte_countp, old_format_p);
      write_object(PROP_val(props), stream, byte_countp, old_format_p);
    }
}

/*
  static void print_type(int type)
  {
  switch(type)
  {
  case (Int):
  fsm_printf("INTEGER");
  break;
  case (Character):
  fsm_printf("CHARACTER");
  break;
  case (Symbol):
  fsm_printf("SYMBOL");
  break;
  case (String): ;
  default:
  fsm_printf("STRING");
  break;
  }
  }
  */

static ALPHABETptr file_to_global_atom_map;
static ALPHABETptr global_to_file_atom_map;

static void write_atoms(FILE *stream, unsigned long *byte_countp, NETptr net,
			int old_format_p)
{
  int atom_count, i, j;
  id_type id, atom_id;
  int arity, net_arity = NET_arc_label_arity(net);
  LABELptr label;

  global_to_file_atom_map =
    make_alph(LIM_id_count(LABEL_MAP), LABEL_VECTOR);
  
  for (i = 0; i < LIM_id_count(LABEL_MAP); i++)
    ALPH_item(global_to_file_atom_map,i) = ID_NO_SYMBOL;
  
  ALPH_item(global_to_file_atom_map,0) = 0;
  /* reserve EPSILON (0) and OTHER (1) */
  ALPH_item(global_to_file_atom_map,1) = 1;     
  atom_count = 2;

  /* Make sure sigma is in the expected form */

  label_to_binary(NET_sigma(net));

  /* enumerate atoms */
    
  for (id = 0; id < ALPH_len(NET_sigma(net)); id++)
    {
      if (ALPH_item(NET_sigma(net), id))
	{
	  label = id_to_label(id);
	  arity = LABEL_arity(label);
	  switch (arity)
	    {
	    case 1:         /* already an atomic label */
	      if (ALPH_item(global_to_file_atom_map,id) == ID_NO_SYMBOL)
		/* if atom hasn't been seen before */
		{
		  ALPH_item(global_to_file_atom_map,id) = atom_count;
		  atom_count++;
		}
	      break;
	    default:  /* a tuple...break into atoms (only in TWOL_MODE). */
	      if (arity > net_arity)
		{
		  handle_warning("NET arity incorrectly set", "WRITE_ATOMS",
				 arity);
		  NET_arc_label_arity(net) = arity;
		  net_arity = arity;
		}
	      for (j = 0; j < (int)LABEL_arity(id_to_label(id)); j++)
		{
		  atom_id = TUPLE_labels(LABEL_tuple(id_to_label(id)))[j];
		  if (ALPH_item(global_to_file_atom_map, atom_id) == ID_NO_SYMBOL)
		    /* if atom hasn't been seen before */
		    {
		      ALPH_item(global_to_file_atom_map,atom_id) = atom_count;
		      atom_count++;
		    }
		}
	      break;
	    }
	}
    }
  
  file_to_global_atom_map = reverse_map(global_to_file_atom_map);
  
  write_number((long) atom_count - 2, stream, byte_countp);
  /* don't count EPSILON & OTHER */
  write_number((long) String, stream, byte_countp);
  
  for (i = 2; i < atom_count; i++)                                      
    {   
      label = id_to_label(ALPH_item(file_to_global_atom_map, i));
      write_fat_str(LABEL_atom(label), stream, byte_countp, old_format_p);
    }

  free_alph(file_to_global_atom_map);
}

static void write_alphabet(FILE *stream, unsigned long *byte_countp,
			   NETptr net, int old_format_p)
{
  int arity, i, j, id;
  LABELptr label;
  
  write_atoms(stream, byte_countp, net, old_format_p);
  
  /* Write out arc labels (tuples, perhaps) */
  arity = NET_arc_label_arity(net);
  
  write_number((long) SYMBOL_TALLY_COUNT, stream, byte_countp);
  write_number((long) arity, stream, byte_countp);
  
  for (i = 0; i < SYMBOL_TALLY_COUNT; i++)
    {
      id = TALLY_label(SYMBOL_TALLY + i);
      switch (arity)
        {
        case (1):       /* FSM */
          write_number(ALPH_item(global_to_file_atom_map,id), stream,
		       byte_countp);
          break;
        default:        /* FST */
          label = id_to_label(id);
          switch (LABEL_arity(label))
            {
            case (1):
              for (j = 0; j < arity; j++)
                write_number(ALPH_item(global_to_file_atom_map,id), stream,
			     byte_countp);
              break;
            default:
              /* Watch for OTHER:OTHER */
              if (get_field(label, UPPER) == OTHER &&
                  get_field(label, LOWER) == OTHER)
                for (j = 0; j < arity; j++)
                  write_number(ONE_SIDED_OTHER, stream, byte_countp);
              else
                for (j = 0; j < arity; j++)
                  write_number(ALPH_item(global_to_file_atom_map,
					 get_field(label, j)),
			       stream, byte_countp);
              break;
            }
          break;
        }
    }
  /* print_tally(); */

  /* SYMBOL_TALLY is NULL if we are writing a virtual net. */

  if (SYMBOL_TALLY)
    {
      free((void *) SYMBOL_TALLY);
      SYMBOL_TALLY = NULL;
    }

  free_alph(global_to_file_atom_map);   
}


int write_arcs_and_states(FILE *stream, unsigned long *byte_countp,
                          NETptr net, unsigned long *arc_count,
                          unsigned long *state_count,
			  unsigned long *file_lengthp,
			  int old_format_p)
{
    /* The last two parameters are unused: this is just to align with the
       "save_func" prototype defined in file "struct.h"
       defined in file "struct.h". */
    
  /* Writes the network in a linear, depth first order. The first arc pointing
     to a destination state is printed without the number of the destination
     unless the destination is empty. Otherwise the format is
     LABEL DESTINATION.
     */
     
  STACKptr arc_stack = create_stack("Arcs to write");
  ARCptr arc;
  STATEptr dest_state;
  unsigned long a_count = 0, s_count = 0;
  
  /* Arc stack holds the arcs yet to be processed. Big nets are printed
     with a status bark marking the progress.
     */
  
  if (DISPLAY_WRITE_STATUS_BAR && (NET_num_arcs(net) >= 2500))
    {
      int i;
      STATUS_BAR_INCREMENT = NET_num_arcs(net)/STATUS_BAR_WIDTH;
      
      fsm_fprintf (stderr, "  ");
      for (i = 0; i < STATUS_BAR_WIDTH; i++)
        fsm_fprintf (stderr, " ");
      
      fsm_fprintf (stderr, "100%%\r0%%");
      fflush(stderr);
    }
  else 
    STATUS_BAR_INCREMENT = -1;
     
 /* Mark the initial state and push its arcs to the stack to
    be processed. */
     
  STATE_client_cell(NET_start_state(net)) = (void *) s_count++;
  push((void *) STATE_arc_set(NET_start_state(net)), arc_stack);
  
  /* Process arcs one by one off the stack. */
  
  while ((arc = (ARCptr) pop(arc_stack)))
    {
      /* Count the arc, maybe print a progress message. */
      
      a_count++;
      
      if (STATUS_BAR_INCREMENT > 0 
	  && a_count > 0 
	  && a_count % STATUS_BAR_INCREMENT == 0)
	{
	  fsm_putc('>', stderr);
	  fflush(stderr);
	}
      
      /* Write the label, get the destination state. */
      
      write_label(arc, stream, byte_countp); 
      dest_state = ARC_destination(arc);
      
      /* Push the sister arcs on the stack, if there are any. */
      
      if (next_arc(arc))
	push((void *) next_arc(arc), arc_stack);
      
      
      /* Check if the destination state is already being processed.
         If the client cell is less than zero (-1), the state is
         yet to be processed. Otherwise just write the state number.
         */
         
      if ((long) STATE_client_cell(dest_state) < 0)
	{
	  /* New state. Put its number into the client cell. */
	  
	  STATE_client_cell(dest_state) = (void *) s_count++;
	  
	  /* If the state has arcs, push them on to the stack to be
	     processed without writing the number. If the state is
	     empty, write the number.
	     */
	     
	  if (STATE_arc_set(dest_state) != NULL)	
	    push((void *) STATE_arc_set(dest_state), arc_stack);
	  else
	    write_number((long) STATE_client_cell(dest_state), stream,
			 byte_countp);
	}
      else
	write_number((long) STATE_client_cell(dest_state), stream,
		     byte_countp);
    }
  
  free_stack(arc_stack);
  *arc_count = a_count;
  *state_count = s_count;
/*  NET_names_smashed(net) = TRUE; */
  NET_names_matter(net) = FALSE;

  return(NO_ERROR);
}

#ifndef TWOLC_APPLICATION
#ifndef LEXC_APPLICATION
#ifndef XIS_APPLICATION
int write_arc_block(FILE *stream, unsigned long *byte_countp,
                    NETptr net, unsigned long *arc_count,
                    unsigned long *state_count, unsigned long *file_lengthp,
		    int old_format_p)
{
    /* The last 4 parameters are unused: this is just to align with the
       "save_func" prototype defined in file "struct.h". */
    
  unsigned char *block = NET_arc_block(net);
  long size = NET_block_size(net);
  
  /* Record the address of the start state. That's the
     number of bytes from the beginning of the arc block to
     the location of the start state.
     */
  write_number((long int) NET_start_loc(net) - (long int) NET_arc_block(net),
  	stream, byte_countp);
  
  /* Record the size of the block. */
  
  write_number(size, stream, byte_countp);
  
  if (fwrite((void *) block, (size_t) 1, (size_t) size, stream) != (size_t)size)
    handle_error("Write failed.\n", "WRITE_ARC_BLOCK", 0); 
  
  *byte_countp += size;

  return(NO_ERROR);

}
#endif
#endif
#endif

/* typedef void *VOIDptr; */
VOIDptr (*transition_table_from_net)(NETptr net) = NULL;
void (*transition_table_to_file)(FILE *stream, NETptr net, VOIDptr ntt) = NULL;

#ifndef WORLD
int write_virtual_net(FILE *stream, unsigned long *byte_countp,
                      NETptr net, unsigned long *arc_count,
                      unsigned long *state_count, unsigned long *file_lengthp,
		      int old_format_p)
{
    /* The 2 parameters "arc_count" and "state_count" are unused: this is
       just to align with the "save_func" prototype defined in file
       "struct.h". */
    
    /*  A virtual net contains no states or arcs. We write
        just the operation and the number of the operands
        (1 or 2).

        The number of bytes in the operand networks are added
        to the file length but not to the byte count of the
        virtual net itself.
    */

  VirtualInfo *vinfo = NET_virtual_info(net);
  VMethods *vmethods;
  int n;

  assert(vinfo);

  vmethods = vinfo->methods;

  if (vinfo->op2)
    n = 2;
  else
    {
      assert(vinfo->op1);
      n = 1;
    }

  /* Record the number of bytes for the operation ID and
     the number of nets in the byte count for the virtual
     net. */

  write_number(vmethods->operation_id, stream, byte_countp);
  write_number(n, stream, byte_countp);

  /* Record the number of bytes in the operand network(s)
     in the global count for the file. */

  network_to_stream(vinfo->op1, stream, file_lengthp, old_format_p);
  if (n == 2)
    network_to_stream(vinfo->op2, stream, file_lengthp, old_format_p);

  return(NO_ERROR);
}
#endif /* WORLD */

static void check_net_arity(NETptr net)
{
  int arity = NET_arc_label_arity(net);
  set_net_arity(net);

  if (arity != NET_arc_label_arity(net))
    fsm_fprintf(stderr, "*** Programmer alert: Wrong arity. \
Marked as %d, should be %d. Fixed.\n", arity, NET_arc_label_arity(net));
}

int network_to_stream(NETptr net, FILE *stream, unsigned long *file_lengthp,
		      int old_format_p)
{
  unsigned long tot_bytes, byte_count = 0;
  int size, tot_size;
  ALPHABETptr optimized_map = NULL, restore_map;
  unsigned long expected_states = 0, expected_arcs = 0;
  unsigned long state_count = 0, arc_count = 0;
  VOIDptr ntt = NULL; /*transition table*/
  int error= 0;

  /* If the net contains a transition table, excorporate it and write
     first the net and then the table to stream. After the excorporation
     the net is not tabular and not compacted any more !!! */

  if (NET_Kaplan_compressed(net))
  {
      error= save_functions[TypeSaveCompress](stream, &byte_count, net,
                                              &arc_count, &state_count,
                                              file_lengthp, FALSE);
      return(error);
  }
  
  check_net_arity(net);

  if (NET_tabular(net))
    {
      if (! transition_table_from_net)     /* function not available */
	fsm_fprintf (stderr, "*** ERROR: network contains a transition table\
which cannot be written to a file in this implementation.\n");

      else                                    /* function available */
	ntt = transition_table_from_net (net);
    }

  /* If the net has been compacted, the labels have
     already been optimized. */
  
#ifndef TWOLC_APPLICATION
#ifndef XIS_APPLICATION  
  if (NET_compacted(net))
    {
      /* Let's assume the values are correct. */
      
      arc_count = expected_arcs = NET_num_arcs(net);
      state_count = expected_states = NET_num_states(net);
      
      restore_tally(net, &optimized_map);
    }
  else
#endif
#endif
    {
      if (NET_is_virtual(net))
	{
	  /* Some of the states of the virtual network
	     may have been realized already but they
	     will not be written into the file.
	     We write the sigma alphabet into the
	     file but no labels because the virtual
	     net has no states or arcs. Must set
	     SYMBOL_TALLY_COUNT to 0 because its
	     value is recorded to file in write_alphabet()
	     below.
	  */
	  
	  expected_states = 0;
	  expected_arcs = 0;
	  SYMBOL_TALLY_COUNT = 0;
	}
      else
	{
	  optimize_labels(net, &optimized_map);
	  /* Also check state and arc counts */
	  prepare_states(net);
	  
	  expected_states = NET_num_states(net);
	  expected_arcs = NET_num_arcs(net);
	}

    }

  write_number(expected_states, stream, &byte_count);
  write_number(expected_arcs, stream, &byte_count);
  write_number(network_flags_to_number(net), stream, &byte_count);
  write_props(stream, &byte_count, net, old_format_p);
  write_alphabet(stream, &byte_count, net, old_format_p);
  
#ifndef TWOLC_APPLICATION
#ifndef LEXC_APPLICATION 
#ifndef XIS_APPLICATION
#ifndef WORLD 
  if (NET_compacted(net))
    error= save_functions[TypeSaveCompact](stream, &byte_count, net,
                                           &arc_count, &state_count,
                                           file_lengthp, old_format_p);
  else if (NET_is_virtual(net))
    error= save_functions[TypeSaveVirtual](stream, &byte_count, net,
                                           &arc_count, &state_count,
                                           file_lengthp, old_format_p);
  else
#endif /* WORLD */
#endif
#endif
#endif
    save_functions[TypeSaveStandard](stream, &byte_count, net, &arc_count, &state_count, file_lengthp, old_format_p);
  
  if (STATUS_BAR_INCREMENT > 0)
    {
      fputc('\n', stderr);
      fflush(stderr);
    }
  
  size = 1 + (number_size((long) byte_count)/7);
  tot_bytes = byte_count + size;
  tot_size = 1 + (number_size(tot_bytes)/7);
  
  if (size < tot_size)
    tot_bytes++;
  
  write_number(tot_bytes, stream, &byte_count);
  *file_lengthp += byte_count;
  
  if (!(NET_compacted(net) || NET_is_virtual(net)))
    {
      restore_map = reverse_map(optimized_map);
      renumber_arc_labels(net, restore_map);
      
      free_alph(restore_map);
      free_alph(optimized_map);
    }
  
  
  if (state_count != expected_states && NET_pruned(net))
    handle_warning("STATE_COUNT mismatch)", 
                 "network_to_stream", (int) expected_states - state_count);
  
  if (arc_count != expected_arcs && NET_pruned(net))
    handle_warning("ARC_COUNT mismatch", 
                 "network_to_stream", (int) expected_arcs - arc_count);

  /* If there was a transition table in the net, we write it now
     to the stream and incorporate it into the net again. */

  if (NET_tabular(net))
    {
      if (! transition_table_to_file)     /* function not available */
	fsm_fprintf (stderr, "*** ERROR: network contains a transition table\
which cannot be written to a file in this implementation.\n");

      else                                    /* function available */
	transition_table_to_file (stream, net, ntt);
    }
  return(error);
}


/******************************************************************
 *                                                                *
 * Top-level functions for writing fsms to a file   *
 *                                                                *
 ******************************************************************/


void real_networks_to_file(NVptr nets, FILE *stream, int net_count,
			   int old_format_p)
     /*************
      *
      *  Don't call this function directly.  Rather, use the macros:
      *
      *         networks_to_file(first_net, stream)
      *         network_to_file(net, stream)
      *
      *  The macros correctly initialize the NET_COUNT argument.
      *
      *************/
{
  unsigned long file_pos = 0;
  int i, error, original_net_count= 0;
  /* int net_location_index= 0; */
      
  STANDARD_HEADERptr header;
  
  if (net_count == 0)
      net_count= NV_len(nets);
  
  if (net_count > NV_len(nets))
    {
      fsm_fprintf(stderr, "Only %d nets present.  Cannot write %d.\n",
              NV_len(nets), net_count);
      return;
      
    }

  header = write_header(stream, 0, 0, NULL, net_count, NV_net(nets,0),
			&file_pos, old_format_p);

  /* The original net count is the number of nets to save a priori: */
  original_net_count= net_count;

  for (i = 0; i < original_net_count; i++)
    {
        HEADER_net_location(header, i) = file_pos;
        error= network_to_stream(NV_net(nets,i), stream, &file_pos,
				 old_format_p);
        if (error == ERROR)
        {
            /* The saving of this network failed:
               decrement the number of networks really saved to the
               stream. */
            net_count--;
            HEADER_net_location(header, i) = 0;
        }
    }

  /* Set the current file length: it is the position in the stream once
     the last networks has been written. */
  HEADER_file_length(header) = file_pos;

  /* Record the updated file length to the stream: */
  /* Set the position in the stream equal to the beginnning of the stream
     and write the file length. */
  rewind(stream);
  file_pos = 0;
  write_four_bytes(HEADER_file_length(header), stream, &file_pos);

  /* Record the start locations of the networks in the reserved space
     at the end of the header:
     - first go at the position in the stream where the net_location
     array starts.
     It is recorded as the temporary value of header_length.
     - Then update the header_length and record it to the stream.
  */

  file_pos = HEADER_header_length(header);
  fseek(stream, file_pos, SEEK_SET);

  for (i = 0; i < original_net_count; i++)
    write_four_bytes(HEADER_net_location(header, i), stream, &file_pos);

  /* Reset header length to the correct value. */
  HEADER_header_length(header) = file_pos;
  
} /* real_networks_to_file */


int write_nets(NVptr nets, char *filename)
{
/* On Macintosh we set the type of binary files to 'XFSN'
   . The default is 'TEXT'. */

  if (nets)
    {
      FILE *stream;
#ifdef MAC_OS
	extern long int _ftype, _fcreator;
	int file_type = _ftype;
	int file_creator = _fcreator;
	_fcreator = 'XFSC';
	_ftype = 'XFSN';
#endif /* MAC_OS */
  
      if (!(stream = fopen(filename, "wb")))
	{
	  fsm_fprintf(stderr, "Cannot open '%s'\n", filename);
#ifdef MAC_OS
	  _ftype = file_type;
#endif /* MAC_OS */
	  return(1);
	}
      else
	{
	  networks_to_file(nets, stream);
	  fclose(stream);
#ifdef MAC_OS
	_ftype = file_type;
	_fcreator = file_creator;
#endif /* MAC_OS */
	  return(0);
	}
    }
  else
    return(1);
}


int write_net(NETptr net, char *filename)
{
  NVtype nv;
  
  nv.len = 1;
  nv.nets = &net;

  return(write_nets(&nv, filename));
}

/* Change Display status bar */  
int display_write_status_bar(int on_off)
{
    int old_mode = DISPLAY_WRITE_STATUS_BAR;
    DISPLAY_WRITE_STATUS_BAR = on_off;

    return old_mode;
}
#endif /* SLIM_FST */
