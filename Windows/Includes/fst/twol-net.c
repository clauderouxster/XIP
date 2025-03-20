/* $Id: twol-net.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 1990-93  by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#include "fst/struct.h"
#include "fst/sig-prop.h"
#include "fst/eqv-labs.h"
#include "fst/twol-net.h"
#include "fst/fsm_io.h"
#include "fst/types.h"
#include "fst/utf8_fatstr_util.h"

/*************************************************
 *
 *  TWOL-NET.C
 * 
 *  Lauri Karttunen, RXRC
 *  December 1993
 *
 *************************************************/

/* These functions used to be in MODIFY.C" */

static int epsilon_p(FAT_STR fs)
{
  return(CHARNUM(fs) == '0' && CHARNUM(fs + 1) == 0 &&
         CHARSET(fs) == 0 && CHARSET(fs + 1) == 0);
}

static int other_p(FAT_STR fs)
{
  return(CHARNUM(fs) == '?' && CHARNUM(fs + 1) == 0 &&
         CHARSET(fs) == 0 && CHARSET(fs + 1) == 0);
}

static int quoted_p(FAT_STR fs)
{
  /* Percent sign followed by some nonempty string. */
  
  return(CHARNUM(fs) == '%' && CHARNUM(fs + 1) != 0 &&
         CHARSET(fs) == 0 && CHARSET(fs + 1) == 0);
}


static void recode_label(id_type *id, FAT_STR name)
{
  if (epsilon_p(name))
    *id= EPSILON;
  else if (other_p(name))
    *id=OTHER;
  else if (quoted_p(name))
    intern_atomic_label(id, name + 1);
  else
    intern_atomic_label(id, name);  
}

static void expand_lab(id_type *id, id_type compressed_label)
{
  /* Converts the id COMPRESSED_LABEL to label and examines its name.  
     If the label's name has a colon in the middle, the label
     is interpreted as a compressed label of an fstpair.
     In that case, the parameter ID is assigned the label id
     of the fstpair, otherwise the output value is the
     same as the input i.e. ID is assigned COMPRESSED_LABEL
     */
  
  LABELptr label = id_to_label(compressed_label);
  FAT_STR name, fs;
  static FAT_STR upper_name = NULL;
  static int max_len = 0;

  /* By default, compressed_label is supposed to represent an atomic name, 
     (not a compressed fst pair), or an already decompressed tuple */
  *id = compressed_label;

  if (LABEL_arity(label) != 2)
    { /* the label is not already decompressed as a tuple */
      name = LABEL_atom(label);
      fs = name;

      while (CHARNUM(fs) || CHARSET(fs))
	{
      /* If you encounter ' :' that is in the middle of name
         (that is, neither final nor initial character), delete it
         and assign the string up to that spot to UPPER_NAME
         and the rest of the name to LOWER_NAME. This assumes
         that if a symbol name can contain ':' only as the very first or
         as the very last character. We check for this here to
         avoid tripping over this later.
         */
      
	  if (CHARNUM(fs) == ':' && CHARSET(fs) == '\0' && fs != name
	      && !((CHARNUM(fs + 1) == '\0') && CHARSET(fs + 1) == '\0' ))
	    {
	      FAT_STR lower_name = fs + 1;
	      id_type upper_id, lower_id; 
	      int len;
          
	  /* Must copy the upper_name. Otherwise it could
	     match the complex symbol name. Temporarily break
	     the string, then copy and restore.
	     */

	      CHARNUM(fs) = '\0';
	      len = fat_strlen(name);

	      if (len >= max_len)
		{
	      /* Let's allocate 16 characters initially for a static
		 buffer. Extend it later if necesary.
		 */

		  if (max_len == 0 && len < 16)
		    {
		      max_len = 15;
		      upper_name =
			(FAT_STR) malloc((size_t) (16 * sizeof(FAT_CHAR)));
		    }
		  else
		    {
		      max_len = len;

		      if (!upper_name)
			upper_name = (FAT_STR)
			  malloc((size_t) ((len+1) * sizeof(FAT_CHAR)));
		      else
			upper_name = (FAT_STR)
			  realloc(upper_name, (size_t) ((len+1)*sizeof(FAT_CHAR)));
		    }
		}
  
	      fat_strcpy(upper_name, name);

	      CHARNUM(fs) = ':';	  

	      recode_label(&upper_id, upper_name);
	      recode_label(&lower_id, lower_name);
	      make_pair_label(id, upper_id, lower_id);
          
          /* If lower name contains a nonfinal ':', we might
             have made an error making the cut. */
          
            while (CHARNUM(lower_name) || CHARSET(lower_name))
	      {
		if (CHARNUM(lower_name) == ':' && CHARSET(lower_name) == 0
		    && (CHARNUM(lower_name + 1) || CHARSET(lower_name + 1)))
		  {
		    fsm_fprintf(stderr, "*** Warning: '");
		    FST_fprint_fat_string(stderr, name);
		    fsm_fprintf(stderr, "' cannot be unambiguously converted.\n");
		    fsm_fprintf(stderr, "    Upper symbol = '");
		    print_label(upper_id, stderr, DO_ESCAPE);
		    fsm_fprintf(stderr, "', lower symbol = '");
		    print_label(lower_id, stderr, DO_ESCAPE);
		    fsm_fprintf(stderr, "'\n");
		    break;
		  }              	
		lower_name++;  	    
	      } 
	    /* the compressed fst pair has been interned as a tuple 
	       and ID contains the adressed of the uncompressed label */
	    break;
          }
        fs++;
      }
    } /* if LABEL_arity...*/
}

ALPHABETptr decompress_alphabet(ALPHABETptr alph, int copy_p)
{
  /* Returns a new alphabet in label format in which every
     atomic pair labels are replaced by true fstpairs.
     */
  
  ALPHABETptr result_alph;
  id_type *items1, *items2, id;
  int i, len;
  
  binary_to_label(alph);
  
  len = ALPH_len(alph);
  items1 = ALPH_items(alph);
  
  if (copy_p)
    result_alph = make_alph(len, LABEL_VECTOR);
  else
    result_alph = alph;
  
  items2 = ALPH_items(result_alph);
  
  for (i = 0; i < len; i++)
    {
      expand_lab(&id, *items1++);
      *items2++ = id;
    }
  
  return(result_alph);
}

void decompress_net(NETptr net)
{
  /* Replaces the arc labels of NET with a new set of labels obtained
     by expanding any atomic pair labels to true fstpairs.  The label
     and sigma fields of the net are updated and the arity set
     correctly.
     */
  
  static id_type map_len = 0;
  static id_type *map = NULL;
  int i, len, max_id;
  id_type *items1, *items2;
  ALPHABETptr labels1 = NET_labels(net), labels2;
  STATEptr state;
  ARCptr arc;
  
  /* Get sigma into standard format. */

  standardize_sigma(net);

  /* Get a new label set by expanding the original label alphabet. */
  
  labels2 = decompress_alphabet(labels1, DO_COPY);
  
  len =  ALPH_len(labels1);
  items1 = ALPH_items(labels1);
  items2 = ALPH_items(labels2);
  
  /* Make sure that the conversion map has space for all the labels. */
  
  max_id = max_label_item(ALPH_items(labels1), len);
  
  if (map_len <= max_id)
    {
      map_len = max_id + 1;
      if (map == NULL)
        map = (id_type *) malloc((size_t) map_len * (size_t) sizeof(id_type));
      else
        map = (id_type *) realloc(map,
				(size_t) map_len * (size_t) sizeof(id_type));
    }
  
  /* Set map positions corresponding to the old labels to the
     new id values.
     */
  
  for (i = 0; i < len; i++)
    map[*items1++] = *items2++;
  
  /* Replace the old arc labels. */
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
      ARC_label(arc) = map[ARC_label(arc)];
  
  /* Reset LABEL field, update SIGMA. */
  free_alph(labels1);
  NET_labels(net) = labels2;
  
  sigma_augment(NET_sigma(net), labels2);
  
  /* Reset arity. */
  
  for (i = 0, items2 = ALPH_items(labels2); i < len; i++)
    if (LABEL_arity(id_to_label(*items2++)) == 2)
      {
        NET_arc_label_arity(net) = 2;
        break;
      }
}

void standardize_sigma(NETptr net)
{
  /*Converts NET_sigma from TWOL to STANDARD MODE.  Removes fstpairs
     from sigma and adds their constituents, if necessary.
     Resets NET_twol_net flag.
     */
  ALPHABETptr sig;
  id_type *items;
  int i, len;
  LABELptr label;
  
  NET_arc_label_arity(net) = 1;
  
  sig = label_to_binary(decompress_alphabet(NET_sigma(net), DONT_COPY));
  len = ALPH_len(sig);
  items = ALPH_items(sig);

  for (i = 0; i < len; i++, items++)
    {
      if (*items == 1)
        {
          label = id_to_label(i);
          if (LABEL_arity(label) == 2)
            {
              /* Remove fstpair from sigma. */
              
              *items = 0;
              
              /* The constituent ID's of an fstpair are necessarily
                 smaller than the pair id. No danger of overstepping
                 bounds here. Not checking for EPSILON here, just
                 at the end.
                 */

              sigma_add_to(sig, fstpair_upper(label));
              sigma_add_to(sig, fstpair_lower(label));
            }
        }
    }
  NET_twol_net(net) = 0;
}

void convert_twol_nets(NVptr nets, int quiet_p)
     /***
      *
      * Some nets are stored in an "TWOL" format.  Such nets have
      * the following properties:
      *      (1) All tuples are stored as atoms (name = tuple print name).
      *          The real alphabet is stored as the list property "SIGMA".
      *      (2) Arcs are compressed into eqv_classes.
      *          The class definitions are stored as the property "EQV-VECTOR".
      *      (3) Such nets are also marked with the network flag "twol_net".
      *
      * This function restores such nets to the standard format,
      * deletes the properties that are no longer needed, and 
      * resets the twol_net flag.
      *
      * Unless QUIET_P is TRUE, a status message is printed.
      *
      ***/
{
  NETptr net, first_net;
  
  if (!nets || NV_len(nets) == 0)
    return;

  first_net = NV_net(nets, 0);

  if (NET_twol_net(first_net))
    {
      OBJECTptr sigma_prop = get_value(NV_net(nets, 0), SIGMA_ATTR);
      ALPHABETptr labels;
      int i;
      
      if (sigma_prop == NULL || OBJ_type(sigma_prop) != List)
	{
	  fsm_fprintf(stderr, "Net marked as TWOL net but isn't.\n");
	  NET_twol_net(first_net) = FALSE;
	}
      else
	{
	  if (!quiet_p)
	    fsm_printf("Converting nets from TWOL to STANDARD mode.\n");
	  labels = prop_val_to_alph(sigma_prop, LABEL_VECTOR);
	  decompress_alphabet(labels, DONT_COPY);
	  
	  for (i = 0; i < NV_len(nets); i++)
	    {
	      net = NV_net(nets, i);
	      decompress_net(net);
	      restore_eqv_arcs(net, labels);
	      set_net_arity(net);    /* Bug fix. Added by LK 00-03-04 */
	      NET_twol_net(net) = FALSE;
	    }			
	  free_prop(remove_prop(first_net, SIGMA_ATTR));
	  free_alph(labels);
	}
    }
}
#endif /* SLIM_FST */
