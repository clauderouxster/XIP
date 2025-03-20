/* $Id: cmp-net.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1992-93 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  CMP-NET.C
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

#include "fst/cmp-net.h"
#include "fst/struct.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#include "fst/syst-dep.h"
#include "fst/fsm_io.h"

unsigned char *ARC_BLOCK, *START_LOC, *DEST_ADDR, *ADDR_LIMIT;
id_type LABEL_ID;
int LABEL_LIMIT;
int LAST_ARC_P, DEST_FINAL_P;
unsigned char *(*DECODE_LABEL)(unsigned char *loc);
unsigned char *(*DECODE_ADDR)(unsigned char *loc);
unsigned char *(*DECODE_DATA)(STATEptr state, unsigned char *loc);
unsigned char *(*SKIP_DATA)(unsigned char *loc);

int SKIP_BYTES, SKIP_DATA_P;
long int STATE_NAME;
id_type *RECODE_KEY;

HEAPptr CS_HEAP = NULL;
HASH_TABLEptr CS_HASH = NULL;

/* 
 * inline
 *   static int bit_count (long int number)
 * {
 *   register int bit_count = 1;
 *   
 *   while (number >>= 1)
 *     bit_count++;
 *   
 *   return(bit_count);
 * }
 */

static uint32 loc_to_int(void *loc)
{
  /* return the least significant 7 bits,
     ta number in the range 0-127.
     */
  return((uint32) ((uint32) loc & 127));
}


static int compare_loc(void *loc1, void *loc2)
{
  if ((uint32) loc1 < (uint32) loc2)
    return(-1);
  else if ((uint32) loc1 == (uint32) loc2)
    return(0);
  else
    return(1);
}

void init_cs_mem(void)
{
  if (CS_HEAP == NULL)
    {
      CS_HEAP = init_heap((int) sizeof(CStype), CS_HASH_SIZE,
      	"Compact states");
      CS_HASH = make_hash_table(CS_HASH_SIZE, "Compact states");
    }
  else
    clear_cs_mem();
}

void free_cs_mem(void)
{
  /*
    fsm_fprintf(stdout, "State data allocated: %ld bytes\n",
    heap_size(HT_cell_heap(CS_HASH), TRUE));
    fsm_fprintf(stdout, "State data in use: %ld bytes\n",
    heap_size(HT_cell_heap(CS_HASH), FALSE));
    fsm_fprintf(stdout, "State hash table size: %ld bytes\n",
    hash_table_size(CS_HASH, FALSE));
    */
  
  free_hash_table(CS_HASH);
  free_heap(CS_HEAP);
  
  CS_HASH = NULL;
  CS_HEAP = NULL;
}

void clear_cs_mem(void)
{
  int i, lim = HT_bucket_count(CS_HASH);
  HEAPptr cell_heap = HT_cell_heap(CS_HASH);
  CELLptr cell, next;
  
  for (i = 0; i < lim; i++)
    {
      for (cell = HT_bucket(CS_HASH, i); cell != NULL; cell = next)
	{
	  next = CELL_next(cell);
	  free_cell((void *) CELL_content(cell), CS_HEAP);
	  free_cell((void *) cell, cell_heap);
	}
      HT_bucket(CS_HASH, i) = NULL;
    }
}


CSptr get_dest_state(void)
{
  /* Makes a compact CS structure for the state whose arcs
     start at the location DEST_ADDR.  The calling function
     must set this properly.
     */
  
  void *value;
  
  if (get_hash(CS_HASH, (void *) DEST_ADDR, &value,
	       loc_to_int, compare_loc) != 0)
    {
      CSptr cs = (CSptr) cell_alloc(CS_HEAP);
      
      CS_addr(cs) = DEST_ADDR - ARC_BLOCK;
      CS_loc(cs) = DEST_ADDR;
      if (DEST_FINAL_P > 0)
        CS_final(cs) = TRUE;
      else
	CS_final(cs) = FALSE;
      CS_mark(cs) = 0;
      /* Don't reclaim states with a low address because they have
	 a high number of incoming arcs.
	 */
      CS_reclaimable(cs) = (uint32) CS_addr(cs) > 127;
      
      if (put_hash(CS_HASH, (void *) DEST_ADDR,
		   (void *) cs, loc_to_int, compare_loc) != 0)
	not_enough_memory("GET_DEST_STATE");
      return(cs);
    }
  else
    return((CSptr) value);
}

void free_cs(CSptr cs)
{
  if (del_hash(CS_HASH, (void *) CS_loc(cs), loc_to_int, compare_loc) != 0)
    handle_error("Cell not found", "FREE_CS", 0);
  
  free_cell((void *) cs, CS_HEAP);
} 


/*
 * static int get_start_offset(int option)
 * {
 */
  /* If compaction optimization is SMALLEST, or SMALL, the first arc begins
     at byte 1. If the option is FAST, the arcs begin at 2.  If the option
     is FASTEST, we start at byte 4.  These conventions are necessary because
     on the Sparcstations, and other machines not including Macs, two byte
     integers must begin at an even numbered byte and four byte integegers
     at a byte divisible by four.
     */
/*  
 *   switch (option)
 *     {
 *       case(FASTEST):
 * 	return(4);
 *       break;
 *       case(FAST):
 * 	return(2);
 *       break;
 *     default:
 *       return(1);
 *       break;
 *     }
 * }
 */

/*
 * static int decode_start_offset(int first_byte)
 * {
 *   switch (first_byte)
 *     {
 *     case (100):
 *       return(get_start_offset(FASTEST));
 *       break;
 *     case (99):
 *       return(get_start_offset(FAST));
 *       break;
 *     default:
 *       return(get_start_offset(SMALL));
 *       break;
 *     }
 * }
 */

inline
  unsigned char *four_byte_label(unsigned char *loc)
{
  LAST_ARC_P = *loc;
  DEST_FINAL_P = *(loc + 1);
  LABEL_ID = *(uint16 *)(loc + 2);
  return(loc + 4);
}

inline
  unsigned char *two_byte_label(unsigned char *loc)
{
  /* Reads a two-byte label starting from LOC and sets
     the values of LABEL_ID, DEST_FINAL_P, LAST_ARC_P.
     Because the last two bits are used for flags,
     the LABEL_ID part of the label consists of
     14 bits.
     */
  
  LABEL_ID = *(uint16 *) loc;
  
  /* Set label flags. */
  
  DEST_FINAL_P = LABEL_ID & 1;
  LAST_ARC_P = LABEL_ID & 2;
  
  /* Now shift over */
  
  LABEL_ID = LABEL_ID >> 2;
  
  return(loc + 2);
}

inline
  unsigned char *decode_var_label(unsigned char *loc)
{
  /* Reads the label starting from LOC and sets the values
     of LABEL_ID, DEST_FINAL_P, LAST_ARC_P. Returns the
     next unread byte.
     */
  
  unsigned char byte = *loc++;
  
  /* Set label flags. */
  
  DEST_FINAL_P = byte & final_dest_mask;
  LAST_ARC_P = byte & last_mask;
  
  /* Get label id. */
  
  LABEL_ID = byte & first_label_mask;
  
  while (more_to_come_p(byte))
    {
      byte = *loc++;
      LABEL_ID = (LABEL_ID << 7) | (byte & next_data_mask);
    }
  /*
    if (LABEL_ID < 0)
    handle_error("Label underflow", "DECODE_VAR_LABEL", LABEL_ID);
    if (LABEL_ID >= (LABEL_LIMIT + 2))
    handle_error("Label overflow", "DECODE_VAR_LABEL", LABEL_ID);
    */
  return(loc);
}

inline
  unsigned char *decode_var_addr(unsigned char *loc)
{
  /* Reads the address starting at LOC sets the value of DEST_ADDR
     Returns the address of the next unread byte.
     */
  
  unsigned char byte = *loc++;
  unsigned long int incr;
  
  incr = (byte & first_addr_mask);
  
  while (more_to_come_p(byte))
    {
      byte = *loc++;
      incr = (incr << 7) | (byte & next_data_mask);
    }
  /*    
    if (addr < 0)
    handle_error("Address underflow", "DECODE_ARC", addr);
    if (DEST_ADDR >= ADDR_LIMIT)
    handle_error("Address overflow", "DECODE_ARC", addr);
    */
  
  DEST_ADDR = ARC_BLOCK + incr;
  return(loc);
}

inline
  static uint32 decode_four_bytes(unsigned char *loc)
{
#ifdef MAC_OS
  return(*(uint32 *) loc);
#else
  if ((unsigned long) loc & 1) /* Combine 8, 16, and 8 bit integers. */
    return((uint32)
	   (((uint32) *loc << 24) | ((uint32) *(uint16 *)(loc + 1) << 8) |
	    *(loc + 3)));
  else if ((unsigned long) loc & 2)  /* Combine two 16 bit integers. */
    return((uint32)
	   ((((uint32) *(uint16 *) loc) << 16) | *(uint16 *)(loc + 2)));
  else  /* LOC divisible by 4, a simple cast to 32 bit integer is OK. */
    return(*(uint32 *) loc);
#endif
}

unsigned char *four_byte_data(STATEptr state, unsigned char *loc)
{
  STATE_client_cell(state) = (void *) decode_four_bytes(loc);
  return(loc + 4);
}

unsigned char *decode_var_data(STATEptr state, unsigned char *loc)
{
  /* Like DECODE_VAR_ADDR except that does not add ARC_BLOCK.
   */
  
  unsigned char byte = *loc++;
  unsigned long int data;
  
  data = (byte & first_addr_mask);
  
  while (more_to_come_p(byte))
    {
      byte = *loc++;
      data = (data << 7) | (byte & next_data_mask);
    }  
  STATE_client_cell(state) = (void *) data;
  return(loc);
}


unsigned char *skip_four_bytes(unsigned char *loc)
{
  return(loc + 4);
}

unsigned char *skip_var_data(unsigned char *loc)
{
  /* Skips over one variable length sequence of bytes.
   */ 
  while (more_to_come_p(*loc++))
    ;
  
  return(loc);
}

/* On the Sun, 16 bit integers are aligned to start at even-numbered
   address locations and 32 bit integers start at locations divisible
   by 4. On the Mac the same constraints don't seem to hold, which
   makes it easier to cast sequences of bytes to 2, 3, and 4 byte
   integers on the Mac than on the Sun.
   (Is this true of all Macs or just MacII's?)
   */

unsigned char *one_byte_addr(unsigned char *loc)
{
  DEST_ADDR = ARC_BLOCK + *loc;
  return(loc + 1);
}

unsigned char *two_byte_addr(unsigned char *loc)
{
#ifdef MAC_OS
  DEST_ADDR = ARC_BLOCK + (uint16) *((uint16 *) loc);
#else
  if ((uint32) loc & 1)  /* LOC is odd, must shift and add. */
    DEST_ADDR = ARC_BLOCK + (uint16) (((uint16) *loc << 8) | *(loc + 1));
  else                  /* LOC is even, we can cast to a 16 bit integer. */
    DEST_ADDR = ARC_BLOCK + (uint16) *((uint16 *) loc);
#endif
  return(loc + 2);
}

unsigned char *three_byte_addr(unsigned char *loc)
{
#ifdef MAC_OS
  DEST_ADDR = ARC_BLOCK + ((((uint32) (*(uint16 *) loc)) << 8) | *(loc + 2));
#else
  if ((uint32) loc & 1)    /* LOC is odd: combine 8 and 16 bit integers. */
    DEST_ADDR = ARC_BLOCK + ((((uint32) *loc) << 16) | *(uint16 *)(loc + 1));
  else                     /* LOC is even: combine 16 and 8 bit integers. */
    DEST_ADDR = ARC_BLOCK + ((((uint32) (*(uint16 *) loc)) << 8) | *(loc + 2));
#endif
  
  return(loc + 3);
}

unsigned char *four_byte_addr(unsigned char *loc)
{  
  DEST_ADDR = ARC_BLOCK + decode_four_bytes(loc);
  return(loc + 4);
}

unsigned char *four_byte_absolute_addr(unsigned char *loc)
{  
  DEST_ADDR = (unsigned char *) decode_four_bytes(loc);
  return(loc + 4);
}

void set_decoding_fns(char first_byte)
{
  /* The first byte of the arc block indicates how labels and
     destination addresses are  represented:
     0 = variable length label, variable length relative address;
     1 = variable length label, one byte relative address;
     2 = variable length label, two byte relative address;
     3 = variable length label, three byte relative address;
     4 = variable length label, four byte relative address;
     99 = two byte label, four byte absolute address;
     100 = four byte label, four byte absolute address;
     N.B. variable length labels must be interpreted using the
     recode alphabet of the net.
     */
  switch ((int) first_byte)
    {
    case 0:
      DECODE_LABEL = decode_var_label;
      DECODE_ADDR = decode_var_addr;
      DECODE_DATA = decode_var_data;
      SKIP_DATA = skip_var_data;
      SKIP_BYTES = 0;
      break;
    case 1:
      DECODE_LABEL = decode_var_label;
      DECODE_ADDR = one_byte_addr;
      DECODE_DATA = decode_var_data;
      SKIP_DATA = skip_var_data;
      SKIP_BYTES = 1;
      break;
    case 2:
      DECODE_LABEL = decode_var_label;
      DECODE_ADDR = two_byte_addr;
      DECODE_DATA = decode_var_data;
      SKIP_DATA = skip_var_data;
      SKIP_BYTES = 2;
      break;
    case 3:
      DECODE_LABEL = decode_var_label;
      DECODE_ADDR = three_byte_addr;
      DECODE_DATA = decode_var_data;
      SKIP_DATA = skip_var_data;
      SKIP_BYTES = 3;
      break;
    case 4:
      DECODE_LABEL = decode_var_label;
      DECODE_ADDR = four_byte_addr;
      DECODE_DATA = decode_var_data;
      SKIP_DATA = skip_var_data;
      SKIP_BYTES = 4;
      break;
    case 99:
      DECODE_LABEL = two_byte_label;
#ifdef WINSOFT
      DECODE_ADDR = four_byte_addr;
#else
       DECODE_ADDR = four_byte_absolute_addr;
#endif
      DECODE_DATA = four_byte_data;
      SKIP_DATA = skip_four_bytes;
      SKIP_BYTES = 4;
      break;
    case 100:
      DECODE_LABEL = four_byte_label;
#ifdef WINSOFT
      DECODE_ADDR = four_byte_addr;
#else
       DECODE_ADDR = four_byte_absolute_addr;
#endif
      DECODE_DATA = four_byte_data;
      SKIP_DATA = skip_four_bytes;
      SKIP_BYTES = 4;
      break; 
    default:
      handle_error("Bad first byte", "INIT_COMPACT_NET_GLOBALS", *ARC_BLOCK);
    }
}
#endif /* SLIM_FST */
