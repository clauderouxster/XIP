/* $Id: recode.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1993 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "fst/error.h"
#include "fst/struct.h"
#include "fst/recode.h"
#include "fst/fsm_io.h"
#include "fst/utf8_fatstr_util.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 


/******************************************************
 **
 ** RECODE.C
 **
 **  Lauri Karttunen, Xerox PARC, July 1993
 **
 **  Utility for converting between Macintosh, ISO 8859,
 **  IBM PC, etc. character encodings.
 **
 ******************************************************/

#define DO_REVERSE 1
#define DONT_REVERSE 0


unsigned char ISOToMacTable[256] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
  0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
  0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
  0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
  0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
  0xA5, 0xAA, 0xAD, 0xB0, 0xB3, 0xB7, 0xBA, 0xBD,
  0xC3, 0xC5, 0xC9, 0xD1, 0xD4, 0xD9, 0xDA, 0xB6,
  0xC6, 0xCE, 0xE2, 0xE3, 0xE4, 0xF0, 0xF6, 0xF7,
  0xF9, 0xFA, 0xFB, 0xFD, 0xFE, 0xFF, 0xF5, 0xC4,
  0xCA, 0xC1, 0xA2, 0xA3, 0xDB, 0xB4, 0xCF, 0xA4,
  0xAC, 0xA9, 0xBB, 0xC7, 0xC2, 0xD0, 0xA8, 0xF8,
  0xA1, 0xB1, 0xD3, 0xD2, 0xAB, 0xB5, 0xA6, 0xE1,
  0xFC, 0xD5, 0xBC, 0xC8, 0xB9, 0xB8, 0xB2, 0xC0,
  0xCB, 0xE7, 0xE5, 0xCC, 0x80, 0x81, 0xAE, 0x82,
  0xE9, 0x83, 0xE6, 0xE8, 0xED, 0xEA, 0xEB, 0xEC,
  0xDC, 0x84, 0xF1, 0xEE, 0xEF, 0xCD, 0x85, 0xD7,
  0xAF, 0xF4, 0xF2, 0xF3, 0x86, 0xA0, 0xDE, 0xA7,
  0x88, 0x87, 0x89, 0x8B, 0x8A, 0x8C, 0xBE, 0x8D,
  0x8F, 0x8E, 0x90, 0x91, 0x93, 0x92, 0x94, 0x95,
  0xDD, 0x96, 0x98, 0x97, 0x99, 0x9B, 0x9A, 0xD6,
  0xBF, 0x9D, 0x9C, 0x9E, 0x9F, 0xE0, 0xDF, 0xD8
  };


unsigned char MacToISOTable[256] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
  0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
  0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
  0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
  0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
  0xC4, 0xC5, 0xC7, 0xC9, 0xD1, 0xD6, 0xDC, 0xE1,
  0xE0, 0xE2, 0xE4, 0xE3, 0xE5, 0xE7, 0xE9, 0xE8,
  0xEA, 0xEB, 0xED, 0xEC, 0xEE, 0xEF, 0xF1, 0xF3,
  0xF2, 0xF4, 0xF6, 0xF5, 0xFA, 0xF9, 0xFB, 0xFC,
  0xDD, 0xB0, 0xA2, 0xA3, 0xA7, 0x80, 0xB6, 0xDF,
  0xAE, 0xA9, 0x81, 0xB4, 0xA8, 0x82, 0xC6, 0xD8,
  0x83, 0xB1, 0xBE, 0x84, 0xA5, 0xB5, 0x8F, 0x85,
  0xBD, 0xBC, 0x86, 0xAA, 0xBA, 0x87, 0xE6, 0xF8,
  0xBF, 0xA1, 0xAC, 0x88, 0x9F, 0x89, 0x90, 0xAB,
  0xBB, 0x8A, 0xA0, 0xC0, 0xC3, 0xD5, 0x91, 0xA6,
  0xAD, 0x8B, 0xB3, 0xB2, 0x8C, 0xB9, 0xF7, 0xD7,
  0xFF, 0x8D, 0x8E, 0xA4, 0xD0, 0xF0, 0xDE, 0xFE,
  0xFD, 0xB7, 0x92, 0x93, 0x94, 0xC2, 0xCA, 0xC1,
  0xCB, 0xC8, 0xCD, 0xCE, 0xCF, 0xCC, 0xD3, 0xD4,
  0x95, 0xD2, 0xDA, 0xDB, 0xD9, 0x9E, 0x96, 0x97,
  0xAF, 0x98, 0x99, 0x9A, 0xB8, 0x9B, 0x9C, 0x9D
  };


SPtype UnixToMacMap[]  = {
  {"A1", "0x87"}, {"E1", "â"}   , {"I1", "0x8a"}, {"O1", "0x8e"},
  {"U1", "ø"},    {"A2", "Ý"}   , {"E2", "0x89"}, {"I2", "0x8d"},
  {"O2", "±"},    {"U2", "0x9f"}, {"A3", "0x85"}, {"E3", "0x86"},
  {"I3", "0x8b"}, {"O3", "0x8f"}, {"U3", "0x7f"}, {"C4", "é"},
  {"A5", "Ç"},    {"E5", "0x88"}, {"I5", "0x8c"}, {"O5", "à"},
  {"U5", "å"},    {"Y5", "ã"},    {"A6", "Þ"},    {"O6", "Í"},
  {"N6", "ä"},    {"A7", "ü"},    {"a1", "ç"},    {"e1", "Ä"},
  {"i1", "Æ"},    {"o1", "ù"},    {"u1", "£"},    {"a2", "ê"},
  {"e2", "Å"},    {"i2", "ô"},    {"o2", "ÿ"},    {"u2", "¥"},
  {"a3", "ë"},    {"e3", "É"},    {"i3", "ö"},    {"o3", "Ö"},
  {"u3", "¤"},    {"c4", "ì"},    {"a5", "è"},    {"e5", "æ"},
  {"i5", "ò"},    {"o5", "Ü"},    {"u5", "0x96"}, {"y5", "Û"},
  {"a6", "ï"},    {"o6", "¢"},    {"n6", "û"},    {"a7", "î"},
  {"s8", "º"}
  };

#define UnixToMacMapSize (sizeof(UnixToMacMap) / sizeof(SPtype))


static long int recode_marked_symbols (NETptr net)
{
  /* Recodes symbols that have the id of a replacement symbol in
     the client cell of the label.
     */
  
  STATEptr state;
  ARCptr arc;
  id_type id, new_id;
  long int count = 0;
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
      {
        id = ARC_label(arc);
        
        /* EPSILON and OTHER must not get recoded. */
        
        if (id != EPSILON && id != OTHER)
	  {
	    new_id = (int) LABEL_data(id_to_label(id));
	    
	    if (new_id)
	      {
		ARC_label(arc) = new_id;
		count++;
	      }
          }
      }
  
  return(count);
}

static void process_label(id_type id, LABELptr lab, id_type *items, id_type *max_id)
{
  if (id > *max_id)
    *max_id = id;
  
  LABEL_data(lab) = (void *) NULL;
  items[id] = 1;
}

static void process_upper_and_lower(LABELptr lab, id_type *items, id_type *max_id)
{
  id_type upper = fstpair_upper(lab), lower = fstpair_lower(lab);
  
  process_label(upper, id_to_label(upper), items, max_id);
  process_label(lower, id_to_label(lower), items, max_id);
}

static void shift_string(char *str, int n)
{
  /* Shifts the string N places to the left if N is negative, N places
     to the right if N is positive. The calling function is responsible
     for making sure that there is enough space for the right shift.
     */

    if (n < 0)
      {
        char *tail1 = str;
        char *tail2 = str - n;
        char *end = str + strlen(str) + 1;
        
        while (tail2 <= end)
          *tail1++ = *tail2++;
      }
    else if (n > 0)
      {
        char *tail1 = str + strlen(str) + 1;
        char *tail2 = tail1 + n;

        while (tail1 >= str)
          *tail2-- = *tail1--;
       }
}

static int recode_symbol(id_type id, RKptr key, int reverse_p, int *work_p)
{
    /* If the symbol name of the label contains characters that need to be
       recoded, create a new symbol name, intern it, and store on the data
       field of the symbol that is being recoded.
       */
    int error= NO_ERROR;
    LABELptr lab = id_to_label(id);
      
    /* If the data field contains something, we have already processed
       the symbol. */
  
    if (LABEL_data(lab))
        return(error);
    else
    { 
        char *old, *new, buff[80], max_len = 79;
        char *name, *tail;
        FAT_STR fs = LABEL_atom(lab);
	int i, old_len, new_len, diff, displ, len = fat_strlen(fs);
        int changed_p = FALSE;
        SPptr  pair;
        uint16 map_len = RK_len(key);
  
        /* Convert the symbol name of the label to a thin string. If the
           length is longer than the allocated storage, we need to allocate
           space dynamically.
           */

        if (len > 79)
        {
            if (!(name = (char *) malloc(((size_t) len + 1) * sizeof(char))))
                handle_error("String too long", "RECODE_SYMBOL", len);
        }
        else
            name = buff;
      
        FST_fat_to_thin(fs, name);
        tail = name;
      
        do {
            pair = RK_map(key);
            for (i = 0; i < map_len; i++, pair++)
            {
                if (reverse_p)
                {
                    old = pair->new;
                    new = pair->old;
                }
                else
                {
                    old = pair->old;
                    new = pair->new;
                }
      
                if (old && new)
                {
                    /* Check if the old string occurs on the net--either as
                       a symbol or as part of a multicharacter symbol.
                       N.B. OLD_ALPH is in binary format here.
                       */
	  
                    old_len = strlen(old);
    	       
                    if (strncmp(tail, old, old_len) == 0)
                    {
                        new_len = strlen(new);
                        diff = new_len - old_len;
    	           
                        /* If new is larger than old, we need more space. */
    	           
                        if (diff > 0)
                        {
                            displ = tail - name;
                            len += diff;
    	                
                            if (len > max_len)
                            {
                                max_len = len; 
                                name = (char *)
                                    realloc((void *) name,
                                            (size_t) ((len +1)*sizeof(char))) ;
                                tail = name + displ;   	                    
                            }
                        }
    	             
                        shift_string(tail, diff);
    	         
                        strncpy(tail, new, new_len);
    	           
                        tail = tail + (new_len - 1);
    	           
                        changed_p = TRUE;
                        break;
                    }
	        }
            }
    	
            tail++;
        } while (*tail);
      
        if (changed_p)
        {
            id_type new_id;
	    error= FST_intern_literal(&new_id, name);
	    if (error != NO_ERROR)
	      return(error);

            LABEL_data(lab) = cast_to_void(new_id);
           
            *work_p = TRUE;
        }
        
        /* If the symbol name exceeded the length of the buffer, we can now
           release the allocated temporary storage.
           */
     	
        if (name != buff)
            free((void *) name);
    }
return(error);
}

static void recode_upper_and_lower(id_type id, RKptr key, int reverse_p,
                                   int *work_p)
{
  recode_symbol(upper_id(id), key, reverse_p, work_p);
  recode_symbol(lower_id(id), key, reverse_p, work_p);
}

static ALPHABETptr update_sigma(ALPHABETptr sig)
{
  /* Return a new recoded sigma and free the original one, which is known
     to be in the binary format. Note that we have to be careful here
     because it may be the case that X is recoded as Y and Z as X,
     so X may belong to the new sigma even though it is recoded.
     */
  
  id_type  new_id;
  int i, len = ALPH_len(sig);
  ALPHABETptr new_sig = make_alph(len, BINARY_VECTOR);
  LABELptr lab;
  
  for (i = 0; i < len; i++)
    if (ALPH_item(sig, i))
      {     
        lab = id_to_label(i);
        new_id = (int) LABEL_data(lab);
	
        if (new_id)
          {
            ALPH_item(sig, i) = 0;
            label_to_sigma(new_sig, new_id);
          }
        else
          label_to_sigma(new_sig, i);
      }
  free_alph(sig);
  return(new_sig);
}


static void update_labels(ALPHABETptr labs, ALPHABETptr sig)
{
  /* Replace a label id by the id of its replacement
     Here labs is known to be in label format and sigma
     in the binary format
     */
  
  int i, new_id, len = ALPH_len(labs);
  LABELptr lab;
  
  for (i = 0; i < len; i++)
    {     
      lab = id_to_label(ALPH_item(labs, i));
      new_id = (int) LABEL_data(lab);
      
      if (new_id)
        ALPH_item(labs,i) = new_id;
    }
}

int recode_net(NETptr net, RKptr key, int reverse_p, long int *count) 
{
  /* Recode symbols in NET using KEY as a recode map. If REVERSE_P is FALSE,
     the first member of a pair of strings in the map is an old symbol
     to be recoded as the second string, otherwise the interpretation
     is reversed. Record in COUNT the number of recoded arcs.
     */
  
  id_type id, max_id = 0;
  int i, j, len;
  int work_p = FALSE;
  ALPHABETptr alph;
  LABELptr lab;
  ALPHABETptr old_alph;
  
  /* Make sure NET and KEY are provided */
  
  if (!net || !key)
    return(1);
  
  /* Sigma assumed to be in binary format. */
  
  alph = label_to_binary(sigma(net));
  len = ALPH_len(alph);
  
  /* Collect all the atomic symbols and fstpairs.
     Note that in TWOL_MODE sigma may contain fstpairs.
     Make sure the client field of the label is NULL.
     */
  
  old_alph = make_alph(LIM_id_count(LABEL_MAP), BINARY_VECTOR);
  
  for (i = 0; i < len; i++)
    if (ALPH_item(alph, i))
      {
	lab = id_to_label(i);
	
	process_label(i, lab, ALPH_items(old_alph), &max_id);
	
	if (LABEL_arity(lab) > 1)
	  process_upper_and_lower(lab, ALPH_items(old_alph), &max_id);
      }
  
  /* Labels assumbed to be in label format. */
  
  alph = binary_to_label(NET_labels(net));
  len = ALPH_len(alph);
  
  for (i = 0; i < len; i++)
    {
      id = ALPH_item(alph, i);
      lab = id_to_label(id);
      
      if (LABEL_arity(lab) == 1)
        {
          /* Should already be marked. */
          if (!(ALPH_item(old_alph, id)))
            handle_error("Unaccounted label", "RECODE_NET", i);
        }
      else
        {
          process_upper_and_lower(lab, ALPH_items(old_alph), &max_id);
          process_label(id, lab, ALPH_items(old_alph), &max_id);    
        }
    }
  
    len = max_id + 1;

    for (j = 0; j < len; j++)
      if (ALPH_item(old_alph, j))
        {
          lab = id_to_label(j);
                
          if (LABEL_arity(lab) > 1)
             recode_upper_and_lower(j, key, reverse_p, &work_p);
           else
             recode_symbol(j, key, reverse_p, &work_p);
       }

  /* If there are no occurrences of OLD, we are done. */

  if (work_p)
    {
      /* Check all fstpair labels in the net and recode the ones that
       have a component that is being recoded.
       */ 
  
       for (i = 0; i <= max_id; i++)
         if (ALPH_item(old_alph, i))
           {
             lab = id_to_label(i);
             if (LABEL_arity(lab) > 1)
               {
                 id_type old_u = fstpair_upper(lab), old_l = fstpair_lower(lab),
                 new_u, new_l;
	    
                 if (!(new_u = (int) LABEL_data(id_to_label(old_u))))
                   new_u = old_u;
	    
                 if (!(new_l = (int) LABEL_data(id_to_label(old_l))))
                   new_l = old_l;
	    
                 /* If one or the other component is recoded,
                    recode the fstpair. */
	    
                 if (old_u != new_u || old_l != new_l)
		   {
		     id_type id_pair;
		     make_fstlabel(&id_pair, new_u, new_l);
                     LABEL_data(lab) = cast_to_void(id_pair);
		   }
             }
           }
  
      *count += recode_marked_symbols(net);
      
      /* Update the sigma and the label fields, the label update can
      	 is done in place. */
      
      NET_sigma(net) = update_sigma(NET_sigma(net));
      update_labels(NET_labels(net), NET_sigma(net));

     /* Clear label data cells and free temporary structures. */
  
      for (i = 0; i < ALPH_len(old_alph); i++)
        if (ALPH_item(old_alph, i))
           LABEL_data(id_to_label(i)) = (void *) NULL;
    }

  free_alph(old_alph);   
  
  return(0);
}


static void table_to_key_map(unsigned char *table, SPptr pairs, char *str)
{
  /* Puts the information in the recoding table to a key map. TABLE must be a
     character array of length 256, such as ISOToMacTable, PAIRS should point to a 256
     array of pairs of string pointers. STR is used to make strings to
     fill the old and slots of a pair. STR should have space for 1024 characters.
     
     If the position i in the recoding table, table[i], contains i,
     then i is not recoded. In that case map[i] is set to NULL. If position i
     contains something else, map[i] contains a pointer to a pair of
     strings: <old, new>, where <old> is the 1-character string constructed
     from i and <new> is the one charater string corresponding to table[i].
     */
  
  int i;
  SPptr pair;
  
  for (i = 0; i < 256; i++)
    {
      pair = &pairs[i];
      
      if (table[i]  != i)
        {
          /* Set the old string of PAIR to the string corresponding to i */
          pair->old = str;
          *str++ = i;
          *str++ = '\0';
	  
          /* Set the new string of PAIR to the string formed with table[i] */
          pair->new = str;     
          *str++ = table[i];
          *str++ = '\0';
        }
      else
        {
          /* No recoding for i. */
          pair->old = NULL;
          pair->new = NULL; 
        }
    }
}


int recode_chars(NETptr net, unsigned char *table, int reverse_p,
		 long int *count)
{
  RKtype key;
  SPtype pairs[256];
  char str[1024];
  int err;
  
  key.len = 256;
  key.map = pairs;
  
  table_to_key_map(table, pairs, str);
  
  err = recode_net(net, &key, reverse_p, count);
  
  return(err);
}

int recode_strings(NETptr net, SPtype *pairs, int pair_count,
		   int reverse_p, long int *count)
{
  RKtype key;
  key.len = pair_count;
  key.map = pairs;
  return(recode_net(net, &key, reverse_p, count));
}

int mac_to_iso(NETptr net, long int *count)
{
  return(recode_chars(net, MacToISOTable, DONT_REVERSE, count));
}

RKptr make_recode_key(FILE *stream)
{
  /* Reads a list of pairs in the format
     old1 new1
     old2 new2
     ...etc.
     and constructs a recode key for RECODE_NET. FREE_RECODE_KEY
     reclaims the space.
     */
  
  char old_buf[80], new_buf[80];
  char *old, *new, s = 's';
  SPptr pair;
  RKptr key;
  CONSptr pairs = NULL;
  int i, err, count = 0;
  
  init_cons_mem();
  
  while ((err = fscanf(stream, "%s %s", old_buf, new_buf)) == 2)
    {
      old = (char *) malloc((size_t)(strlen(old_buf) + 1) * sizeof(char));
      new = (char *) malloc((size_t)(strlen(new_buf) + 1) * sizeof(char));
      if (!(old && new))
	not_enough_memory("READ_MAP_AND_RECODE");
      
      strcpy(old, old_buf);
      strcpy(new, new_buf);
      
      pairs = cons((void *) new, pairs);
      pairs = cons((void *) old, pairs);
      count++;   
    }
  
  if (count == 0 && err == EOF)
    {
      /* Empty file. Nothing to do. */

      free_cons_mem();
      return((RKptr) NULL);
    }
  else if (err == 1)
    fsm_fprintf(stderr,
	    "*** Warning: Last symbol in the list, '%s', has no pair.\n",
	    old_buf);
  
  if (count == 1)
    s = '\0';
  
  fsm_fprintf(stdout, "The map contains %d symbol pair%c.\n", count, s);
  
  
  key = (RKptr) malloc((size_t) sizeof(RKtype));
  pair = (SPptr) malloc((size_t) count * sizeof(SPtype));
  
  if (!(key && pair))
    not_enough_memory("READ_MAP_AND_RECODE");
  
  key->len = count;
  key->map = pair;
  
  for (i = count - 1; pairs; i--, pair++)
    {
      pair->old = (char *) car(pairs);
      pairs = cdr(pairs);
      pair->new = (char *) car(pairs);
      pairs = cdr(pairs);
    }
  
  free_cons_mem();
  
  return(key);
}

void free_recode_key(RKptr key)
{
  /* Only use for keys created by MAKE_RECODE_KEY. */
  
  int i;
  SPptr pair;
  
  if (key)
    {
      /* Free the strings. */
      
      pair = RK_map(key);
      
      if (pair)
        {
	  for (i = 0; i < RK_len(key); i++, pair++)
	    {
	      if (pair->old)
		free(pair->old);
	      if (pair->new)
		free(pair->new);
	    }
	  
	  free(RK_map(key));
        }
      free(key);
    }
  
  
}

int iso_to_mac(NETptr net, long int *count)
{
  return(recode_chars(net, ISOToMacTable, DONT_REVERSE, count));
}

int unix_to_mac(NETptr net, long int *count)
{
  return(recode_strings(net, UnixToMacMap, UnixToMacMapSize,
			DONT_REVERSE, count));
}

int mac_to_unix(NETptr net, long int *count)
{
  return(recode_strings(net, UnixToMacMap, UnixToMacMapSize,
			DO_REVERSE, count));
}

#endif /* SLIM_FST */
