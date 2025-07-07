/* $Id: compact.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1992 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  COMPACT.C
 **
 **  Lauri Karttunen, Xerox PARC, October 1992
 **
 **  Conversion between standard and compact network formats.
 **
 ******************************************************/

#include <stddef.h>
#include <stdlib.h> 
#include <stdio.h>
#include <limits.h>
/* #include <memory.h> */

#if defined _WINDOWS
#include <windows.h>
#elif !defined UNDER_CE
#include <sys/mman.h>
#endif

#ifdef MAC_OS
#include <cstring>
#else
#include <string.h>
#endif /* MAC_OS */

#include "fst/syst-dep.h"
#include "fst/error.h"
#include "fst/struct.h"
#include "fst/dbl-link.h"
#include "fst/btree.h"
#include "fst/modify.h"

#include "fst/net-io.h"
#include "fst/bin-in.h"
#include "fst/bin-out.h"
#include "fst/rename.h"
#include "fst/cmp-net.h"
#include "fst/compact.h"
#include "fst/fsm_io.h"
#include "fst/pretty-p.h"


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 


#ifndef WORLD /* No arc_optimization available in Xfst-WORLD version */


/*extern unsigned char *ARC_BLOCK, *START_LOC, *DEST_ADDR, *ADDR_LIMIT;
extern id_type LABEL_ID;
extern int LABEL_LIMIT;
extern int LAST_ARC_P, DEST_FINAL_P;
extern unsigned char *(*DECODE_LABEL)(unsigned char *loc);
extern unsigned char *(*DECODE_ADDR)(unsigned char *loc);
extern unsigned char *(*DECODE_DATA)(STATEptr state, unsigned char *loc);
extern unsigned char *(*SKIP_DATA)(unsigned char *loc);
*/

typedef struct STATE_DATA {
  int arcs_out;
  long arcs_in;
  uint32 label_bytes;
  uint32 address_bytes;
  uint32 client_bytes;
  uint32 state_id;
  uint32 location;
  void *client_data;
} SDcell, *SDptr;

#define SD_out(X) (X)->arcs_out
#define SD_in(X) (X)->arcs_in
#define SD_lbytes(X)  (X)->label_bytes
#define SD_abytes(X)  (X)->address_bytes
#define SD_cbytes(X)  (X)->client_bytes
#define SD_id(X)  (X)->state_id
#define SD_loc(X)  (X)->location
#define SD_client_data(X)  (X)->client_data
  SDptr STATE_DATA_CELLS = NULL;


/* HEAPptr CS_HEAP = NULL;
HASH_TABLEptr CS_HASH = NULL; */
#endif /* WORLD */


#ifndef WORLD /* No arc_optimization available in Xfst-WORLD version */

inline
  static int bit_count (long int number)
{
  register int bit_count = 1;
  
  while ((number >>= 1))
    bit_count++;
  
  return(bit_count);
}

  /* return the least significant 7 bits,
     ta number in the range 0-127.
     */
/* static unsigned int loc_to_int(void *loc)
 * {
 *  return((unsigned int) ((unsigned int) loc & 127));
 * }
 */

/* static int compare_loc(void *loc1, void *loc2)
 * {
 *   if ((uint32) loc1 < (uint32) loc2)
 *     return(-1);
 *   else if ((uint32) loc1 == (uint32) loc2)
 *     return(0);
 *   else
 *     return(1);
 * }
 */

static int more_arcs_in_p(const void *s1, const void *s2)
{
  /* return -1, 0, or 1, depending on whether the S1 has more,
   * as many, or fewer incoming arcs than the contents of the
   * client cell of S2. If the number of incoming arcs is the
   * same, give precedence to the state that has a more compact
   * arc set.
   */
  
  SDptr sd1 = (SDptr) STATE_client_cell(* (STATEptr *) s1);
  SDptr sd2 = (SDptr) STATE_client_cell(* (STATEptr *) s2);
  
  if (SD_in(sd1) == SD_in(sd2))
    {
      int sd1_size = SD_lbytes(sd1) + SD_abytes(sd1);
      int sd2_size = SD_lbytes(sd2) + SD_abytes(sd2);
      
      if (sd1_size == sd2_size)
	return(0);
      else if (sd2_size < sd1_size)
	return(1);
      else
	return(-1);
    }
  else if (SD_in(sd2) > SD_in(sd1))
    return(1);
  else
    return(-1);
}


/* Prints up to MAX_OUT (> 0) states that have the greatest number
   of incoming arcs. If MAX_OUT is less than one, statistics is
   printed for all states in NET.
   */

/*  
  static void print_statistics(NETptr net, long int max_out)
  {
  STATEptr state;
  long int *limit, i = 0;
  
  if (max_out > 0)
  limit = &max_out;
  else
  limit = &i;
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
  {
  SDptr cell = (SDptr) STATE_client_cell(state);
  
  if (++i > *limit)
  break;
  
  fsm_fputs("========================================================\n",
  stdout);
  fsm_fprintf(stdout, "%ld. State #%ld, Arcs out: %d, Arcs in: %ld\n",
  i, SD_id(cell), SD_out(cell), SD_in(cell));
  fsm_fprintf(stdout,
  "Location: %ld, Label bytes: %ld, Address bytes: %ld\n",
  (long) SD_loc(cell), SD_lbytes(cell), SD_abytes(cell));
  }
  }
  */


static int data_size (long int data, int first_bits)
{
  /* Returns the numbers of bytes required to represent DATA
     distributed so that the first byte contains FIRST_BITS
     of data and subsequent bits 7 bits of data each.
     */
  
  int bits = bit_count(data);
  
  if (bits > first_bits)
    {
      if (bits < 7)
	return(2);
      else
        return((bits / 7) + 1);
    }
  else 
    return(1);
}

static void print_sizes(uint32 *sizes, int max_size, char *type)
{
  int i, lim = 0;
  
  for (i = 1; i < max_size; i++)
    if (sizes[i] > 0)
      lim = i;
  
  fsm_fprintf(stderr, "\n  %s:", type);
  for (i = 1; i <= lim; i++)
    {
      if (i > 1)
	fputc(',', stderr);
      
      fsm_fprintf(stderr, " %d: %ld", i, (long)sizes[i]);
    }
  fputc('\n', stderr);
}


static int get_start_offset(int option)
{
  /* If compaction optimization is SMALLEST, or SMALL, the first arc begins
     at byte 1. If the option is FAST, the arcs begin at 2.  If the option
     is FASTEST, we start at byte 4.  These conventions are necessary because
     on the Sparcstations, and other machines not including Macs, two byte
     integers must begin at an even numbered byte and four byte integegers
     at a byte divisible by four.
     */
  
  switch (option)
    {
      case(FASTEST):
	return(4);
      break;
      case(FAST):
	return(2);
      break;
    default:
      return(1);
      break;
    }
}

static int decode_start_offset(int first_byte)
{
  switch (first_byte)
    {
    case (100):
      return(get_start_offset(FASTEST));
      break;
    case (99):
      return(get_start_offset(FAST));
      break;
    default:
      return(get_start_offset(SMALL));
      break;
    }
}


/* Sub-routine for function sort_arcs_by_increasing_label(). */
static int compact_label_cmpfn (const void* arc1, const void* arc2)
{
  /* Returns -1 if lower-label of arc1 precedes lower-label of arc2,
     0 if they are the same, otherwise 1. */

    id_type label1 = ARC_label(*(ARCptr *) arc1);
    id_type label2 = ARC_label(*(ARCptr *) arc2);

    if (lower_id(label1) < lower_id(label2))
        return (-1);
    else if (lower_id(label1) == lower_id(label2))
        return(0);
    else
        return(1);
    
} /* compact_label_cmpfn */


/* Sorts arcs by increasing lower label values.
   (speeds up the apply).  */
void sort_arcs_lower_label(NETptr net)

{
    int client_cells_have_content= NET_names_matter(net);

#ifdef DEBUG
    fsm_fprintf(stderr, "\nSorting arcs by increasing lower-label value... (May take a while)...\n");
#endif

  reorder_arcs(net, compact_label_cmpfn);

  /* reorder_arcs turns systematically the flag names_matter to FALSE:
     restore it if the net had state_client_cell contents: */
  if (client_cells_have_content)
      NET_names_matter(net)= TRUE;

} /* sort_arcs_lower_label */



unsigned long int collect_state_statistics (NETptr net, int option,
					    int *addr_size)
{
  SDptr cell;
  STATEptr state;
  int i, changed_p, fixed_size = 0, max_size = sizeof(void *) + 2;
  uint32 next_loc, prev_size, *sizes, label_byte_count = 0;
  int dummy_label = ALPH_len(NET_labels(net)) + 2;
  int start_offset = get_start_offset(option);
  ALPHABETptr optimized_map = NULL;
  
  sizes = (uint32 *) malloc((size_t) max_size * (size_t) sizeof(uint32));
  
  STATE_DATA_CELLS =
    (SDptr) malloc((size_t) NET_num_states(net) * (size_t) sizeof(SDcell));
  
  cell = STATE_DATA_CELLS;
  
  if (cell == NULL)
    {
      fsm_fprintf(stderr, "Not enough memory for compaction.\n");
      return(0);
    }
  
  
  /* Create STATE DATA records for all states before calling RENAME_STATES
     so that the original contents of client cells is preserved.  */
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      /* Initialize fields to zero */
      SD_out(cell) = 0;
      SD_in(cell) = 0;
      SD_lbytes(cell) = 0;	/* Bytes for arc labels */
      SD_abytes(cell) = 0;	/* Bytes for destination addresses */
      SD_cbytes(cell) = 0;   /* Bytes for client data */
      SD_loc(cell) = 0;
      SD_client_data(cell) = STATE_client_cell(state);
      cell++;
    }

  /* Number the states in their order of reachability from the start
     state, and assign this number to each state client cells.  */
  rename_states(net);
  
  /* recode labels when option is FAST: */
  if (option < FAST)
  {
      /* Rename labels by frequency */
      
      fsm_fprintf(stderr, "\nCollecting data...");
      fflush(stderr);

      sort_arcs_lower_label(net);
      
      optimize_labels(net, &optimized_map);
      
      /* Empty state is indicated by a "dummy" label whose id equals
	 ID_NO_SYMBOL that is outside the range of valid labels. 
	 The length of the recode array (one larger than the largest 
	 recoded label) serves that purpose.
	 */
      
      NET_recode_key(net) = (void *) reverse_map(optimized_map);
      
      free_alph(optimized_map);
      
      free(SYMBOL_TALLY);
      SYMBOL_TALLY = NULL;
    }
  else      
    NET_recode_key(net) = make_identity_map(net);     

  /* Now put in the state id numbers and store the STATE data record
     of the state in its client cell.  If the original information in
     the client cell is significant count the number of bytes
     required to store it.   */  
  cell = STATE_DATA_CELLS;
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      SD_id(cell) = (long) STATE_client_cell(state);
      STATE_client_cell(state) = (void *) cell;
      if (NET_names_matter(net))
	{
	  if (option == FAST)
	    SD_cbytes(cell) = 4;
	  else
	    SD_cbytes(cell) =
	      data_size((long) SD_client_data(cell), first_addr_size);
	}
      cell++;
    }

  dummy_label = ALPH_len(NET_recode_key(net));
  for (i = 0; i < max_size; i++)
    sizes[i] = 0;
  
  /* Count the label sizes for each state and enumerate incoming
     arcs.     */
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      ARCptr arc = STATE_arc_set(state);
      int arc_count = 0;
      long int size, label_count = 0;
      cell = (SDptr) STATE_client_cell(state);
      
      if (arc == NULL)
	{
	  /* Empty state is indicated by a dummy label whose id is
	     outside the range of valid labels and a 0 address.
	     */
	  if (option == FAST)
	    label_count += 2;
	  else if (option == FASTEST)
	    label_count += 4;
	  else
	    label_count += data_size(dummy_label, first_label_size);
	}
      else
	while (arc != NULL)
	  {
	    arc_count++;
	    if (option == FASTEST)
	      size = 4;
	    else if (option == FAST)
	      size = 2;
	    else
	      size = data_size(ARC_label(arc), first_label_size);
	    SD_in((SDptr)STATE_client_cell(ARC_destination(arc)))++;
	    sizes[size]++;
	    label_count += size;
	    arc = next_arc(arc);
	  }
      SD_out(cell) = arc_count;
      SD_lbytes(cell) = label_count;
      label_byte_count += label_count;
    }
  
  print_sizes(sizes, max_size, "Label sizes");
  
  if (option == SMALLEST)
    {
      /* Addresses recoded as variable length fields. */
      
      *addr_size = 0;
      
      fsm_fprintf(stderr, "Sorting states (may take a while) ...");
      fflush(stderr);
      
      /* Order states by the number of incoming arcs. */
      reorder_states(net, more_arcs_in_p);
      /*
	fputc('\n', stderr);
	print_statistics(net, 5);
	*/
    }
  else if (option == SMALL)
    {
      /* Addresses recorded as minimal fixed length fields. */
      unsigned long int limit = 1;
      next_loc = label_byte_count + 1;
      next_loc += NET_num_arcs(net);
      
      for (i = 1; i <= sizeof(void *) + 1; i++)
        {
	  limit <<= 8;
	  
	  if (next_loc <= limit)
	    break;
	}
      if (i > sizeof(void *) + 1)
	{
	  fsm_fprintf(stderr, "Net requires %d byte addresses. Too large.\n", i);
	  return(0);
	}
      fixed_size = i;
      *addr_size = fixed_size;
      
    }
  else if (option == FAST || option == FASTEST)
    {
      /* Two byte addresses. */
      
      fixed_size = 4;
      *addr_size = fixed_size;
    }
  else
    {
      fsm_fprintf(stderr, "*** Unknown OPTION: %ld\n", (long)option);
      return(0);
    }
  
  fsm_fprintf(stderr, "Locating states .");
  fflush(stderr);
  
  for (i = 0; i < max_size; i++)
    sizes[i] = 0;
  
  /* Get start location. */
  
  next_loc = start_offset;
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      SDptr dest_cell;
      ARCptr arc;
      long int size, addr_count = 0;
      
      cell = (SDptr) STATE_client_cell(state); 
      SD_loc(cell) = next_loc;
      /* If the state has no arcs, it has a dummy label and a NULL address.
       */
      if (STATE_arc_set(state) == NULL)
	{
	  if (option == SMALLEST)
	    addr_count += 1;
	  else
	    addr_count += fixed_size;
	}
      else
	for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
	  {
	    dest_cell = (SDptr) STATE_client_cell(ARC_destination(arc));
	    if (option == SMALLEST)
	      {
		if (SD_loc(dest_cell) > 0)
		  size = data_size(SD_loc(dest_cell), first_addr_size);
		else
		  size = sizeof(void *) + 1;
	      }
	    else
	      size = fixed_size;
	    sizes[size]++;
	    addr_count += size;
	  }
      
      SD_abytes(cell) = addr_count;
      next_loc += SD_cbytes(cell) + SD_lbytes(cell) + addr_count;
    }
  
  /*
    print_sizes(sizes, max_size, "Address sizes");
    print_statistics(net, 5);
    */
  
  do {
    fputc('.', stderr);
    fflush(stderr);
    
    prev_size = next_loc;
    changed_p = FALSE;
    
    next_loc = start_offset;
    
    for (i = 0; i < max_size; i++)
      sizes[i] = 0;
    
    for (state = NET_states(net); state != NULL; state = next_state(state))
      {
	SDptr dest_cell;
	ARCptr arc;
	long int size = 0, addr_count = 0;
	cell = (SDptr) STATE_client_cell(state);
	
	if (SD_loc(cell) != next_loc)
	  {
	    SD_loc(cell) = next_loc;
	    changed_p = TRUE;
	  }
	
	if (STATE_arc_set(state) == NULL)
	  {
	    if (option == SMALLEST)
	      addr_count += 1;
	    else
	      addr_count += fixed_size;
	  }
	else
	  for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
	    {
	      dest_cell = (SDptr) STATE_client_cell(ARC_destination(arc));
	      if (SD_loc(dest_cell) > 0)
		{
		  if (option == SMALLEST)
		    size = data_size(SD_loc(dest_cell), first_addr_size);
		  else
		    size = fixed_size;
		}
	      else
		handle_error("No destination cell", "FOO", 0);
	      
	      sizes[size]++;
	      addr_count += size;	      
	    } 
	
	SD_abytes(cell) = addr_count;
	next_loc += SD_cbytes(cell) + SD_lbytes(cell) + addr_count;
      }
    /*
      fsm_fprintf(stderr, "\nRound %d, Previous size: %ld, New size %ld",
      iteration_count, prev_size, next_loc);
      
      print_sizes(sizes, max_size, "Address sizes");
      */
  } while (changed_p);
  
  fputc('\n',stderr);
  
  if (option == SMALLEST)
    print_sizes(sizes, max_size, "Address sizes");
  else
    fsm_fprintf(stderr, "  %d byte addresses.\n", fixed_size);
  
  fsm_fprintf(stderr, "  Block size: %ld\n", (long)next_loc);
  free((void *) sizes);
  return(next_loc);
} /* collect_state_statistics */


static unsigned char *set_bytes (unsigned long data, int first_size,
				 int flags, unsigned char *loc)
{
  int size, offset;
  unsigned long int n;
  
  size = bit_count(data);
  
  if (size > first_size)
    {
      if (size < 7) size += 7;
      
      offset = 7 * (size / 7);
      n = data >> offset;
      data ^= n << offset;    /* Xor out the shifted bits. */
      
      *loc++ = (unsigned char)(include_next_mask + flags + n); /* YM, 2/23/96, unsigned char cast added */
      
      while (offset > 7)
	{
	  offset -= 7;
	  n = data >> offset;
	  data ^= n << offset;
	  *loc++ = (unsigned char)(include_next_mask + n); /* YM, 2/23/96, unsigned char cast added */
	}
      *loc++ = (unsigned char)data;	/* YM, 2/23/96, unsigned char cast added */
    }
  else
    *loc++ = (unsigned char)(data + flags); /* YM, 2/23/96, unsigned char cast added */
  
  return(loc);
}

static unsigned char *set_two_bytes(uint16 num, unsigned char *loc)
{
  *(uint16 *) loc = num;
  return(loc + 2);
}

static unsigned char *set_four_bytes(uint32 num, unsigned char *loc)
{
  if ((uint32) loc % 4)
    {
      *(uint16 *) loc = num >> 16;
      *(uint16 *) (loc + 2) = num & USHRT_MAX; 
    }
  else
    *(uint32 *) loc = num;
  
  return(loc + 4);
}

static unsigned char *set_fastest_label(ARCptr arc, unsigned char *loc)
{
  /* Four byte label: LAST_ARC_P, DEST_FINAL_P and a two byte LABEL_ID. */
  
  if (next_arc(arc) == NULL)
    *loc++ = 1;
  else 
    *loc++ = 0;
  
  if (STATE_final(ARC_destination(arc)) == TRUE)
    *loc++ = 1;
  else
    *loc++ = 0;
  
  return(set_two_bytes((uint16) ARC_label(arc), loc));
}

static unsigned char *set_fast_label(ARCptr arc, unsigned char *loc)
{
  /* Two byte label, last two bits encode LAST_ARC_P and DEST_FINAL_P. */
  
  int flags;
  
  if (STATE_final(ARC_destination(arc)) == TRUE)
    flags = 1;
  else
    flags = 0;
  
  if (next_arc(arc) == NULL)
    flags += 2;
  
  return(set_two_bytes((((uint16) ARC_label(arc) << 2) | flags), loc));
}

static unsigned char *set_var_label(ARCptr arc, unsigned char *loc)
{
  int flags = 0;
  
  if (STATE_final(ARC_destination(arc)) == TRUE)
    flags += final_dest_mask;
  
  if (next_arc(arc) == NULL)
    flags += last_mask;
  
  return(set_bytes((unsigned long) ARC_label(arc), first_label_size,
		   flags, loc));
}


static unsigned char *set_var_address(uint32 addr, unsigned char *loc)
{
  return(set_bytes(addr, first_addr_size, 0, loc));
}

static unsigned char *set_fixed_address(uint32 addr, unsigned char *loc,
					int size)
{
  int i, offset = (8 * size);
  long int n, data = addr;
  
  for (i = 0; i < size; i ++)
    {
      offset -= 8;
      n = data >> offset;
      data ^= n << offset;
      *loc++ = (fst_byte) n; 
    }  
  return(loc);
}


static int compact_states_and_arcs(NETptr net, uint32 block_size,
				   int addr_size, int option)
{
  unsigned char *loc, *block = (unsigned char *) malloc((size_t) block_size);
  STATEptr next, state, start_state = NET_start_state(net);
  
  if (block == NULL)
    {
      fsm_fprintf(stderr, "Not enough memory for compaction\n");
      return(1);
    }
  
  loc = block;
  
  /* The first state starts at location 1 or 2 rather than 0.
     OPTION indicates the chosen optimization. If the first byte is
     0, it means means that arc destinations are variable length fields.
     1,2,3,4 means the size of a fixed length address pointer.
     If the first byte is 99, both labels (2 bytes) and addresses (4 bytes)
     are fixed length fields and the labels have not been recoded.
     */
  
  if (option > SMALL)
    {
      
      /* Convert relative addresses to absolute locations within the block */
      for (state = NET_states(net); state != NULL; state = next_state(state))
	{
	  SDptr sd = (SDptr) STATE_client_cell(state);
	  SD_loc(sd) += (uint32) block;
	}
      if (option == FASTEST)
	*loc = 100;
      else if (option == FAST)
	*loc = 99;
      else
	handle_error("Unknown option", "COMPACT_NET", option);
    }
  else if (option == SMALLEST)
    *loc = 0;
  else
    *loc = addr_size;
  
  /* Get starting location */
  
  loc = block + get_start_offset(option);
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    {
      ARCptr free_arc, arc = STATE_arc_set(state);
      uint32 addr;
      SDptr sd = (SDptr) STATE_client_cell(state);
      
      /* If the information in the original client cell is to be recorded,
         write it out first as a variable length as a four byte fixed length
         number depending on the option.
         */
      
      
      if (NET_names_matter(net))
	{
	  if (option == SMALLEST)
	    loc = set_var_address((uint32) SD_client_data(sd), loc);
	  else
	    loc = set_fixed_address((uint32) SD_client_data(sd), loc, 4);
	}
      
      /* An arc can be freed as soon as it has been processed but
         states must be kept until we are done.
         */
      
      if (arc == NULL)
        {
          /* An empty state is indicated by a dummy label, one
             larger than the largest label in the net (We use the
             ALPH_len(NET_recode_key(net)) as that label_id) and
             a NULL address.
	     */
          uint32 dummy_label_id = ALPH_len(NET_recode_key(net));
	  
	  switch (option)
	    {
	      case(FASTEST):
		loc = set_four_bytes(dummy_label_id, loc);
	      loc = set_four_bytes(0, loc);
	      break;
	      case(FAST):
		loc = set_two_bytes(dummy_label_id << 2, loc);
	      loc = set_fixed_address(0, loc, addr_size);
	      break;
	      case(SMALL):
		loc = set_bytes(dummy_label_id, first_label_size, 0, loc);
	      loc = set_fixed_address(0, loc, addr_size);
	      break;
	      case(SMALLEST):
		loc = set_bytes(dummy_label_id, first_label_size, 0, loc);
	      loc = set_var_address(0, loc);
	      break;
	    default:
	      handle_error("Unknown option", "COMPACT_NET", option);
	      break;
	    }
        }
      else
      	do {
	  addr = SD_loc((SDptr) STATE_client_cell(ARC_destination(arc)));
	  
	  switch (option)
	    {
	      case(FASTEST):
		loc = set_fastest_label(arc, loc);
	      loc = set_fixed_address(addr, loc, addr_size);
	      break;
	      case(FAST):
		loc = set_fast_label(arc, loc);
	      loc = set_fixed_address(addr, loc, addr_size);
	      break;
	      case(SMALL):
		loc = set_var_label(arc, loc);
	      loc = set_fixed_address(addr, loc, addr_size);
	      break;
	      case(SMALLEST):
		loc = set_var_label(arc, loc);
	      loc = set_var_address(addr, loc);
	      break;
	    default:
	      handle_error("Unknown option", "COMPACT_NET", option);
	      break;
	    }
	  
	  free_arc = arc;
	  arc = next_arc(arc);
	  free_arc(free_arc);
	  
      	} while(arc != NULL);
      
      next = next_state(state);
    }
  
  /* NET_start_state becomes NET_start_loc. This is an absolute address
     regardless of the optimization setting.
     */
  
  if (option > SMALL)
    NET_start_loc(net) =
      (unsigned char *) SD_loc((SDptr) STATE_client_cell(start_state));
  else
    NET_start_loc(net) =  block +
      SD_loc((SDptr) STATE_client_cell(start_state)); 
  
  /* Now free the states and the state data cells. */
  
  free((void *) STATE_DATA_CELLS);
  for (state = NET_states(net); state != NULL; state = next)
    {
      next = next_state(state);
      free_state(state);
    }
  
  /* NET_states becomes NET_arc_block */
  
  NET_arc_block(net) = block;
  
  if (loc == (unsigned char *) block + block_size)
    return(0);
  else
    {
      handle_error("Bad byte count\n", "COMPACT_STATES_AND_ARCS", 0);
      return(1);
    }
}

NETptr compact_net(NETptr net, int option)
{
  /* Replaces state and arc structures by a continuous block of label id's
     and destination addresses.  If OPTION is SMALLEST, labels and addresses
     are written as variable length integers. If OPTION is SMALL,
     labels are written as variable and addresses as minimal-sized
     fixed-length integers. If OPTION is FAST, addresses are written as
     pointer sized integers. When OPTION is SMALLEST or SMALL,
     the addresses are relative locations with respect to the beginning
     of the block. When OPTION is FAST, the addresses encode the locations
     directly.
     */
  
  uint32 block_size;
  
  int addr_size;
  
  if (NET_compacted(net))
    return(net);

  if (has_arc_vectors_p(net))
  {
      fsm_fprintf(stderr, "Can't compact a vectorized network\n.");
      return(net);
  }

  block_size = collect_state_statistics (net, option, &addr_size);
  
  /* print_statistics(net, 1000); */
  
  if (block_size == 0)
    return(net);
  
  NET_start_state_final(net) = STATE_final(NET_start_state(net));
  
  /* If OPTION is SMALLEST, ADDR_SIZE is 0, otherwise the number of bytes
     in a fixed length destination address.
     */
  compact_states_and_arcs(net, block_size, addr_size, option);
  
  NET_compacted(net) = TRUE;
  NET_block_size(net) = block_size;
  /* If the net's arcs were previously sorted, this arc ordering is lost. */
  NET_sorted_arcs(net) = 0;
  
  return(net);
}


static int loc_compare_fn(register void *loc1, register void *loc2)
{
  if (loc1 == loc2)
    return (0);
  else if (loc1 < loc2)
    return (1);
  else
    return (-1);
}


static int recode_init(char first_byte, unsigned char **locp)
{
  /* Compensate for the difference between two byte (99) and
     four byte (100) labels.
     */
  
  LAST_ARC_P = TRUE;
  
  if (first_byte == 100)
    {
      DECODE_LABEL = four_byte_label;
      *locp = ARC_BLOCK + get_start_offset(FASTEST);
      return(2);
    }
  else if (first_byte == 99)
    {
      DECODE_LABEL = two_byte_label;
      *locp = ARC_BLOCK + get_start_offset(FAST);
      return(0);
    }
  
  handle_error("First byte should be 99 or 100", "RECODE_INIT", first_byte);
  return(-1);
}

static void recode_label(unsigned char *start, unsigned char *end,
			 id_type *key, int offset)
{
  /* If the type is 99, must shift to reconstitute the label. */
  
  if (offset == 0)
    LABEL_ID = (key[LABEL_ID] << 2) | LAST_ARC_P | DEST_FINAL_P;
  else
    LABEL_ID = key[LABEL_ID];
  
  if (set_two_bytes((uint16) LABEL_ID, start) != end)
    handle_error("Lost in the bytes", "RECODE_LABEL", 1);
}

static unsigned char *recode_empty_state(unsigned char *start,
					 unsigned char *loc,
					 int new_label, int offset)
{
  /* Recode the dummy label that marks an empty state as an
     integer one larger than any real label id in the net.
     Check that it is followed by a zero address.
     */
  
  if (offset == 0)
    LABEL_ID = new_label << 2;
  else
    LABEL_ID = new_label;
  
  if (set_two_bytes((uint16) LABEL_ID, start) != loc)
    handle_error("Lost in the bytes", "RECODE_EMPTY_STATE", 0);
  
  loc = four_byte_absolute_addr(loc);
  if (DEST_ADDR != 0 || DEST_ADDR == ARC_BLOCK)
    handle_error("Lost in the bytes", "RECODE_EMPTY_STATE", 1);
  
  LAST_ARC_P = TRUE;
  return(loc);
}


void abs_to_rel_block(NETptr net)
{
  /* Replace fixed length unencoded labels by their positions in
     the label table and substitute relative locations for
     absolute addresses. */
  
  unsigned char *loc, *last_loc;
  int addr_size = sizeof(void *);
  int skip_data_p = NET_names_matter(net);
  id_type *labs = ALPH_items(NET_labels(net));
  int i,  max_id, len = ALPH_len(NET_labels(net));
  id_type *key = ALPH_items(NET_recode_key(net));
  int offset;
  
  ARC_BLOCK = NET_arc_block(net);
  
  offset = recode_init(*ARC_BLOCK, &loc);
  
  LABEL_LIMIT = ALPH_len(NET_recode_key(net));
  ADDR_LIMIT =  ARC_BLOCK + NET_block_size(net);
  
  max_id = LABEL_LIMIT - 3;
  
  /* Prepare to swap label id's with their positions
     in the label list.
     */
  for (i = 0; i < len; i++)
    key[*labs++] = i;
  
  /* EPSILON and OTHER are exceptional */
  key[EPSILON] = len;
  key[OTHER] = len + 1;
  
  while (loc < ADDR_LIMIT)
    {
      if (skip_data_p && LAST_ARC_P)
        loc = skip_four_bytes(loc);
      /* Ignore the first two bytes of a four byte label */
      last_loc = loc + offset;
      loc = DECODE_LABEL(loc);
      
      if (LABEL_ID < LABEL_LIMIT)
        {
          recode_label(last_loc, loc, key, offset);
	  
          last_loc = loc;
          loc = four_byte_absolute_addr(loc);
          if (set_fixed_address((uint32) (DEST_ADDR - ARC_BLOCK),
				last_loc, addr_size) != loc)
	    handle_error("Lost in the bytes", "ABS_TO_REL_BLOCK", 2);
        }
      else
        loc = recode_empty_state(last_loc, loc, LABEL_LIMIT, offset);
    }
  /* Restore identity map. */
  for (i = 0; i <= max_id; i++)
    key[i] = i;
}

NETptr uncompact_net(NETptr net)
{
  long n;
  STATEptr state, dest_state;
  ARCptr arc, last_arc;
  BTREEptr node, btree = NULL;
  HEAPptr bt_heap;
  ALPHABETptr restore_map = NET_recode_key(net);
  int *label_array;
  int max_id, start_offset;
  long state_count = NET_num_states(net);
  long arc_count = NET_num_arcs(net);
  unsigned char *loc, *start_loc;
  long int byte_count;
  int read_data_p = NET_names_matter(net);
  
  unsigned char *old_loc;
  
  if (!NET_compacted(net))
    return(net);
  
  /* Dummy label marking an empty state. */
  
  LABEL_LIMIT = ALPH_len(restore_map);
  
  if (restore_map == NULL)
    handle_error("No recode map", "UNCOMPACT_NET", 0);
  
  bt_heap = make_btree_heap();
  
  /* RESTORE_MAP maps the optimized labels back to originals. */
  binary_to_label(restore_map);
  
  /* Array of labels ids to check for determinism. */
  
  max_id = max_label_item(ALPH_items(restore_map), LABEL_LIMIT);
  label_array = (int *) calloc((size_t) max_id + 1, (size_t) sizeof(int));
  
  if (label_array == NULL)
    {
      fsm_fprintf(stderr, "Not enough space\n.");
      return(net);
    }
  
  ARC_BLOCK = NET_arc_block(net);
  start_loc = NET_start_loc(net);
  
  NET_start_state(net) = NULL;
  NET_states(net) = NULL;
  
  NET_num_states(net) = 0;
  NET_num_arcs(net) = 0;
  
  /* Select address decoding function 1. */
  
  set_decoding_fns(*ARC_BLOCK);
  
  /* Get start location. */
  
  start_offset = decode_start_offset(*ARC_BLOCK);
  
  loc = ARC_BLOCK + start_offset;
  byte_count = loc - ARC_BLOCK;
  
  for (n = 0; n < state_count; n++)
    {
      
      node = btree_insert((void *) loc, btree, bt_heap, loc_compare_fn);
      state = (STATEptr) BT_value(node);
      last_arc = NULL;
      
      if (state == NULL)
        {
	  state = make_state(NULL, net);
	  BT_value(node) = (void *) state;
	  
	  if (btree == NULL)
	    btree = node;
        }
      
      if (loc == start_loc)
	{
	  NET_start_state(net) = state;
	  STATE_final(state) = NET_start_state_final(net);
	}
      
      
      if (read_data_p)
	{
	  /* Read client cell information.
	   */
	  
	  old_loc = loc;
	  loc = DECODE_DATA(state, loc);
	  byte_count += loc - old_loc;
	}
      
      /* Get arcs. */
      
      do {
	old_loc = loc;
	loc = DECODE_LABEL(loc);
	byte_count += loc - old_loc;
	
	if (LABEL_ID >= LABEL_LIMIT)	/* The state has no arcs.  */
	  {
	    loc = DECODE_ADDR(loc);
	    /* The dummy label is followed by a 0 address.  If all
	       is well DEST_ADDR should now be 0 or equal to ARC_BLOCK
	       depending on whether addresses are relative or absolute.
	       */
	    
	    if (!(DEST_ADDR == 0 || DEST_ADDR == ARC_BLOCK))
	      handle_error("Lost in the bytes", "UNCOMPACT_NET", 0);
	    
	    break;
	  }
	
	old_loc = loc;
	loc = DECODE_ADDR(loc);
	node = btree_insert((void *) DEST_ADDR, btree, bt_heap, loc_compare_fn);
	byte_count += loc - old_loc;
	
	dest_state = (STATEptr) BT_value(node);
	if (dest_state == NULL)
	  {
	    dest_state = make_state(NULL, net);
	    BT_value(node) = (void *) dest_state;
	  }
	
	STATE_final(dest_state) = (int) (DEST_FINAL_P > 0) ;
	
	/* Restore original label id. */
	
	LABEL_ID = ALPH_item(restore_map, LABEL_ID);
	
	arc = make_arc(LABEL_ID, dest_state, net);
	label_array[LABEL_ID]++;	  
	
	if (last_arc == NULL)
	  STATE_arc_set(state) = arc;
	else
	  next_arc(last_arc) = arc;
	
	last_arc = arc;
	
      } while (!LAST_ARC_P);
      
      /* Check for determinism and clear the array for next round. */
      for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
	{
	  LABEL_ID = ARC_label(arc);
	  if (label_array[LABEL_ID] > 1)
	    {
	      STATE_deterministic(state) = FALSE;
	      NET_deterministic(net) = FALSE;
	    }
	  if (LABEL_ID == EPSILON)
	    {
	      STATE_deterministic(state) = FALSE;
	      NET_deterministic(net) = FALSE;
	      NET_epsilon_free(net) = FALSE;
	    }
	  label_array[LABEL_ID] = 0;
	}
    }
  
  if (NET_start_state(net) == NULL)
    handle_error("No start state", "UNCOMPACT_NET", 0);
  if (state_count != NET_num_states(net))
    handle_error("Bad state count", "UNCOMPACT_NET", 0);
  if (arc_count != NET_num_arcs(net))
    handle_error("Bad arc count", "UNCOMPACT_NET", 0);

  /* Clean memory:
     --------------------------------------------------------- */
  
  /* Freeing the temporary array of labels: */
  free((void *) label_array);

  /* Freeing the ARC_BLOCK:
     be aware to free this memory block only if it has been allocated
     i.e. if we are NOT in MEMORY_MAP use; otherwise ARC_BLOCK just
     points directly to the net file, and the call to free will cause
     a crash.  In this case we should rather call munmap(). */
  if (net->mmap_handle == NULL){
      /* The net has not been mapped into memory
         => you can free the allocated block of arcs. */
      free((void *) ARC_BLOCK);
  }
  else
  {
      /* The net has a memory-map handle, which indicates that it has
         been mapped in memory ->unmap it. */
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
          else
          {
              net->mmap_handle= NULL;
              net->mmap_size= 0;
          }
  }
  
  free_heap(bt_heap);
  
  free_alph(restore_map);
  NET_recode_key(net) = NULL;
  
  NET_compacted(net) = FALSE;
  return(net);
} /*  uncompact_net */

#endif /* WORLD */

#endif /* SLIM_FST */

