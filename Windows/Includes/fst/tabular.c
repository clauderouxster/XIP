/* $Id: tabular.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/******************************************************
 **
 **  TABULAR.C
 **
 **  Andre Kempe, RXRC Grenoble, November 1995
 **
 **  Functions for networks in tabular format.
 **  
 **
 ******************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "fst/struct.h"
#include "fst/bin-in.h"
#include "fst/bin-out.h"
#include "fst/re-aux.h"
#include "fst/calculus.h"
#include "fst/pars-tbl.h"
#include "fst/tabular.h"
#include "fst/fsm_io.h"
#include "fst/mark.h"
#include "fst/utf8_fatstr_util.h"

/******************************************
 *
 *   BASIC FUNCTIONS
 *
 ******************************************/

id_type *TTB_alloc_idtype_vector (long len)
{
  /* Allocates a vector of id_type. */

  id_type *vec;

  vec = (id_type *) malloc (len * sizeof (id_type));
  if (! vec)
    not_enough_memory("TTB_alloc_idtype_vector");

  return (vec);
}

id_type *TTB_realloc_idtype_vector (id_type *oldvec, long len)
{
  /* Reallocates a vector of id_type. */

  id_type *newvec;

  newvec = (id_type *) realloc (oldvec, len * sizeof (id_type));
  if (! newvec)
    not_enough_memory("TTB_realloc_idtype_vector");

  return (newvec);
}

long *TTB_alloc_long_vector (long len)
{
  /* Allocates a vector of long. */

  long *vec;

  vec = (long *) malloc (len * sizeof (long));
  if (! vec)
    not_enough_memory("TTB_alloc_long_vector");

  return (vec);
}

long *TTB_realloc_long_vector (long *oldvec, long len)
{
  /* Reallocates a vector of long. */

  long *newvec;

  newvec = (long *) realloc (oldvec, len * sizeof (long));
  if (! newvec)
    not_enough_memory("TTB_realloc_long_vector");

  return (newvec);
}

void TTB_init_long_vector (long *items, long num_items, long init_value)
{
  while (num_items > 0)
    {
      *items++ = init_value;
      num_items--;
    }
}

void TTB_init_idtype_vector (id_type *items, long num_items, id_type init_value)
{
  while (num_items > 0)
    {
      *items++ = init_value;
      num_items--;
    }
}

void TTB_init_pointer_vector (void **items, long num_items, void *init_value)
{
  while (num_items > 0)
    {
      *items++ = init_value;
      num_items--;
    }
}


/******************************************
 *
 *   WORK WITH ALPHABETS
 *
 ******************************************/

TTALPHptr TTALPH_alloc_alphabet (int len)
{
  TTALPHptr alph;

  alph = (TTALPHptr) malloc (sizeof (TTALPHtype));
  if (! alph)
    not_enough_memory("TTALPH_alloc_alphabet");

  TTALPH_num (alph) = len;
  TTALPH_items (alph) = TTB_alloc_idtype_vector ((long) len);

  return (alph);
}

TTALPHptr TTALPH_realloc_alphabet (TTALPHptr alph, int len)
{
  TTALPH_num (alph) = len;
  TTALPH_items (alph) =
    TTB_realloc_idtype_vector (TTALPH_items (alph), (long) len);

  return (alph);
}

void TTALPH_free_alphabet (TTALPHptr alph)
{
  free (TTALPH_items (alph));
  free (alph);
}


/******************************************
 *
 *   GET LABEL INFORMATION
 *
 ******************************************/

TTLABptr TTLAB_alloc_label_structure ()
{
  TTLABptr ttlab = (TTLABptr) malloc (sizeof (TTLABtype));
  if (! ttlab)
    not_enough_memory("TTLAB_alloc_label_structure");

  TTLAB_upper_str (ttlab) =
    (char *) malloc ((TTLAB_LABEL_MAXLEN + 1) * sizeof (char));
  if (! TTLAB_upper_str (ttlab))
    not_enough_memory("TTLAB_alloc_label_structure");

  TTLAB_lower_str (ttlab) =
    (char *) malloc ((TTLAB_LABEL_MAXLEN + 1) * sizeof (char));
  if (! TTLAB_lower_str (ttlab))
    not_enough_memory("TTLAB_alloc_label_structure");

  return (ttlab);
}


void TTLAB_free_label_structure (TTLABptr ttlab)
{
  free (TTLAB_upper_str (ttlab));
  free (TTLAB_lower_str (ttlab));
  free (ttlab);
}

int TTLAB_get_label_length (LABELptr label)
{
  int i;

  FAT_STR fstring = LABEL_atom( label );

  for (i=0; i<1000; i++)
    if (! *(fstring [i]))
      return (i);

  fsm_fprintf (stderr, "ERROR: label is too long\n");
  exit (1);
}

void TTLAB_get_label_name (LABELptr label, char *name)
{
  /* Checks whether the label is not too long and writes it to name. */

  if (TTLAB_get_label_length (label) +1 > TTLAB_LABEL_MAXLEN)
    {
      fsm_fprintf (stderr, "ERROR in function 'TTLAB_get_label_name'\n");
      fsm_fprintf (stderr, "    label name is too long\n");
      exit (1);
    }

  FST_fat_to_thin (LABEL_atom(label), name);

}

void TTLAB_correct_one_printname
  (id_type id, char *name, int16 *length, int TTLAB_name_p)
{
  /* Changes printname of EPSILON and OTHER from "0" or "?" to the real
     printnames and vice versa. */

  if ((id != EPSILON) && (id != OTHER))  return;

  switch (TTLAB_name_p)
    {
    case TTLAB_ZERO:      /* put "0" or "?" */
      if (id == EPSILON)
	strcpy (name, "0");
      else
	strcpy (name, "1");
      *length = 1;
      break;
    case TTLAB_EPSILON:    /* put real print names */
      TTLAB_get_label_name (id_to_label (id), name);
      *length = strlen (name);
      break;
    }
}

void TTLAB_correct_printnames (TTLABptr ttlab, int TTLAB_name_p)
{
  /* Changes on the upper and lower side, printnames of EPSILON and OTHER
     from "0" or "?" to the real printnames and vice versa. */

  TTLAB_correct_one_printname
    (TTLAB_upper_id (ttlab), TTLAB_upper_str (ttlab),
     &(TTLAB_upper_len (ttlab)), TTLAB_name_p);

  TTLAB_correct_one_printname
    (TTLAB_lower_id (ttlab), TTLAB_lower_str (ttlab),
     &(TTLAB_lower_len (ttlab)), TTLAB_name_p);
}

void TTLAB_get_label_information (id_type id, TTLABptr ttlab)
{
  /* Looks for information for the ID and writes it to 'ttlab'. */

  TUPLEptr tuple;  LABELptr label;
  uint16 id_count = LIM_id_count (LABEL_MAP);

  if (id >= id_count)
    { fsm_fprintf (stderr, "ERROR: id %hd out of range\n", id);  exit (1); }

  TTLAB_id (ttlab) = id;
  label = id_to_label (id);
 
  switch (LABEL_arity (label))
    {
    case 1:
      TTLAB_simple (ttlab) = TRUE;
      TTLAB_upper_id (ttlab) = id;
      TTLAB_lower_id (ttlab) = id;
      break;
    case 2:
      TTLAB_simple (ttlab) = FALSE;
      tuple = LABEL_tuple (label);
      TTLAB_upper_id (ttlab) = TUPLE_labels (tuple) [UPPER];
      TTLAB_lower_id (ttlab) = TUPLE_labels (tuple) [LOWER];
      break;
    default:
      fsm_fprintf (stderr, "ERROR: id %d has wrong arity %d\n",
	       id, LABEL_arity (label));
      exit (1);
    }

  /* handle upper label */

  label = id_to_label (TTLAB_upper_id (ttlab));

  TTLAB_get_label_name (label, TTLAB_upper_str (ttlab));
  TTLAB_upper_len (ttlab) = strlen (TTLAB_upper_str (ttlab));

  if ((TTLAB_upper_len (ttlab) < 2)
      || (TTLAB_upper_id (ttlab) == EPSILON)
      || (TTLAB_upper_id (ttlab) == OTHER)
      || (! strcmp (TTLAB_upper_str (ttlab), TTALPH_FIN_NAME)))
    TTLAB_single_upper (ttlab) = TRUE;
  else
    TTLAB_single_upper (ttlab) = FALSE;

  /* handle lower label in case of arity one */

  if (TTLAB_simple (ttlab) == TRUE)
    {
      strcpy (TTLAB_lower_str (ttlab), TTLAB_upper_str (ttlab));
      TTLAB_lower_len (ttlab) = TTLAB_upper_len (ttlab);
      TTLAB_single_lower (ttlab) = TTLAB_single_upper (ttlab);
    }

  /* handle lower label in case of arity two */

  if (TTLAB_simple (ttlab) == FALSE)
    {
      label = id_to_label (TTLAB_lower_id (ttlab));

      TTLAB_get_label_name (label, TTLAB_lower_str (ttlab));
      TTLAB_lower_len (ttlab) = strlen (TTLAB_lower_str (ttlab));

      if ((TTLAB_lower_len (ttlab) < 2)
	  || (TTLAB_lower_id (ttlab) == EPSILON)
	  || (TTLAB_lower_id (ttlab) == OTHER)
	  || (! strcmp (TTLAB_lower_str (ttlab), TTALPH_FIN_NAME)))
	TTLAB_single_lower (ttlab) = TRUE;
      else
	TTLAB_single_lower (ttlab) = FALSE;
    }

  /* change print names of EPSILON and OTHER to "0" or "?" */

  TTLAB_correct_printnames (ttlab, TTLAB_ZERO);
}

void TTLAB_get_id_of_label (TTLABptr ttlab)
{
  /* Writes ID, upper ID and lower ID for the given label print name
     to 'ttlab'. */
  id_type id;

  FST_intern_literal (&id, TTLAB_upper_str (ttlab));
  TTLAB_upper_id (ttlab) = id;

  if (TTLAB_simple (ttlab))   /* arity 1 */
    {
      TTLAB_lower_id (ttlab) = TTLAB_upper_id (ttlab);
      TTLAB_id (ttlab) = TTLAB_upper_id (ttlab);
    }
  else     /* arity 2 */
    {
      FST_intern_literal (&id, TTLAB_lower_str (ttlab));
      TTLAB_lower_id (ttlab) = id;
      
      make_pair_label(&id, TTLAB_upper_id (ttlab), TTLAB_lower_id (ttlab));
      TTLAB_id (ttlab) = id;
    }
}


/******************************************
 *
 *   CONVERT NETWORK TO TRANSITION TABLE
 *
 ******************************************/


TTAUXptr NTT_alloc_ttaux (NETptr net, NTTptr ntt)
{
  /* Allocates an auxiliary table for the conversion. */

  TTAUXptr aux;

  aux = (TTAUXptr) malloc (sizeof (TTAUXtype));
  if (! aux)
    not_enough_memory("NTT_alloc_ttaux");

  TTAUX_tvec_len (aux) = TTALPH_num (NTT_glob_labs (ntt));
  TTAUX_tvec (aux) = TTB_alloc_long_vector (TTAUX_tvec_len (aux));

  if (NET_arc_label_arity (net) == 1)   /* simple fsm */
    {
      TTAUX_tvec_upper (aux) = NULL;
      TTAUX_tvec_lower (aux) = NULL;
    }
  else    /* fst */
    {
      TTAUX_tvec_upper (aux) = TTB_alloc_long_vector (TTAUX_tvec_len (aux));
      TTAUX_tvec_lower (aux) = TTB_alloc_long_vector (TTAUX_tvec_len (aux));
    }

  TTAUX_next_pos_vec (aux) = TTB_alloc_long_vector (TTAUX_tvec_len (aux));
  TTB_init_long_vector (TTAUX_next_pos_vec (aux), TTAUX_tvec_len (aux), 0);

  TTAUX_net_states (aux) = NET_num_states (net);
  TTAUX_net_arcs (aux) = NET_num_arcs (net);

  TTAUX_num_states (aux) = 0;
  TTAUX_size_states (aux) = NTT_num_states (ntt);
  TTAUX_num_arcs (aux) = 0;
  TTAUX_size_arcs (aux) = NTT_num_arcs (ntt);
  TTAUX_num_other (aux) = 0;
  TTAUX_size_other (aux) = NTT_other_len (ntt);

  TTAUX_arcs_in_vec (aux) = 0;
  TTAUX_cur_state (aux) = 0;
  TTAUX_final (aux) = FALSE;
  TTAUX_base (aux) = 0;

  return (aux);
}

void NTT_free_ttaux (TTAUXptr aux)
{
  /* Allocates an auxiliary table. */

  if (TTAUX_tvec (aux))  free (TTAUX_tvec (aux));
  if (TTAUX_tvec_upper (aux))  free (TTAUX_tvec_upper (aux));
  if (TTAUX_tvec_lower (aux))  free (TTAUX_tvec_lower (aux));
  if (TTAUX_next_pos_vec (aux))  free (TTAUX_next_pos_vec (aux));
  free (aux);
}

int NTT_id_in_alphabet (id_type id, TTALPHptr alph, int len)
{
  /* Checks whether an ID is already in the alphabet. */

  int i;

  for (i=0; i< len; i++)
    if (TTALPH_item (alph, i) == id)  return (TRUE);

  return (FALSE);
}

id_type NTT_highest_id (id_type *vector, int len)
{
  /* Returns the highest ID form the vector. */

  long i;  
  id_type id, idmax;

  idmax= vector[0];
  for (i=1; i< len; i++)
    {
      id = vector [i];
      if (idmax < id)  
	idmax = id;
    }

  return (idmax);
}

static int NTT_qsort_comp (int *p1, int *p2)
{
  return (*p2 - *p1);
}

void NTT_sort_labels (TTALPHptr alph, NETptr net)
{
  /* Sorts labels beginning with the most frequent one.
     For fst we do not consider pair IDs but the corresponding component
     (i.e. simple label) IDs. */

  TTLSORTptr vector;  TTLABptr ttlab = NULL;
  STATEptr state;  ARCptr arc;
  long i, *freq, numfreqs, numfinal = 0;
  id_type id;

  numfreqs = NTT_highest_id (TTALPH_items (alph), TTALPH_num (alph)) + 1;
  freq = TTB_alloc_long_vector (numfreqs);
  memset (freq, 0, numfreqs * sizeof (long));

  /* make a label structure if the net is an fst */

  if (NET_arc_label_arity (net) == 2)
    ttlab = TTLAB_alloc_label_structure ();

  /* count label IDs and final marks */

  for (state = NET_states (net); state; state = next_state (state))
    {
      for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
	{
	  id = ARC_label (arc);
	  if (id >= numfreqs)
	    {
	      fsm_fprintf (stderr, "ERROR: higher ID than expected\n");
	      exit(1);
	    }

	  if (! ttlab)   /* network is a simple fsm */
	    freq [id] ++;
	  else          /* network is an fst */
	    {
	      TTLAB_get_label_information (id, ttlab);
	      freq [TTLAB_upper_id (ttlab)] ++;
	      freq [TTLAB_lower_id (ttlab)] ++;
	    }
	}

      if (STATE_final (state) == TRUE)  numfinal++;
    }

  /* make vector for quicksort */

  vector = (TTLSORTptr) malloc (TTALPH_num (alph) * sizeof (TTLSORTtype));
  if (! vector)
    not_enough_memory("NTT_sort_labels");

  /* copy label IDs and frequencies to vector */

  for (i=0; i< TTALPH_num (alph); i++)
    {
      vector [i].freq = freq [TTALPH_item (alph, i)];
      vector [i].lab = TTALPH_item (alph, i);
    }

  vector [TTALPH_FINLABEL].freq = numfinal;

  /* quicksort of frequencies beginning with highest one */

  QSORT (vector, TTALPH_num (alph), sizeof (TTLSORTtype),
	 (int (*)(const void *, const void *) ) NTT_qsort_comp);

  /* copy label IDs back to the alphabet */

  for (i=0; i< TTALPH_num (alph); i++)
    TTALPH_item (alph, i) = vector [i].lab;

  free (freq);  free (vector);

  if (ttlab)
    TTLAB_free_label_structure (ttlab);
}

TTALPHptr NTT_make_glob_labels (NETptr net)
{
  /* Makes the vector for conversion from local to global labels. */

  ALPHABETptr netlabels = binary_to_label (NET_labels (net));
  ALPHABETptr netsigma = binary_to_label (NET_sigma (net));
  TTALPHptr globlabs;
  id_type id;
  int i, k, labmax = ALPH_max (netlabels), sigmax = ALPH_max (netsigma);;

  globlabs = TTALPH_alloc_alphabet (labmax + sigmax + TTALPH_1ST_ID);

  /* put special IDs to alphabet */

  TTALPH_item (globlabs, TTALPH_EPSILON) = EPSILON;
  TTALPH_item (globlabs, TTALPH_OTHER) = OTHER;
  FST_intern_literal (&id, TTALPH_FIN_NAME);
  TTALPH_item (globlabs, TTALPH_FINLABEL) = id;
  /* put net labels to alphabet */

  for (i=0, k = TTALPH_1ST_ID; i < labmax; i++)
    {
      id = ALPH_item (netlabels, i);
      if ((id == EPSILON) || (id == OTHER))  continue;

      TTALPH_item (globlabs, k++) = id;
    }

  /* put net sigma to alphabet if no already there */

  for (i=0; i < sigmax; i++)
    {
      id = ALPH_item (netsigma, i);
      if ((id == EPSILON) || (id == OTHER))  continue;
      if (NTT_id_in_alphabet (id, globlabs, k))  continue;

      TTALPH_item (globlabs, k++) = id;
    }

  TTALPH_realloc_alphabet (globlabs, k);  /* actual number of copied IDs */
  NTT_sort_labels (globlabs, net);

  return (globlabs);
}


TTALPHptr NTT_make_local_labels (NTTptr ntt)
{
  /* Makes the vector for conversion from global to local labels. */

  TTALPHptr loclabs, globlabs = NTT_glob_labs (ntt);
  int i, locnum, globnum = TTALPH_num (globlabs);
  id_type id;

  locnum = NTT_highest_id (TTALPH_items (globlabs), globnum) + 1;
  loclabs = TTALPH_alloc_alphabet (locnum);

  /* copy IDs to local labels vector; free IDs not handled */

  for (i=0; i< globnum; i++)
    TTALPH_item (loclabs, TTALPH_item (globlabs, i)) = i;

  NTT_loc_epsilon (ntt) = TTALPH_item (loclabs, EPSILON);
  NTT_loc_other (ntt) = TTALPH_item (loclabs, OTHER);
  FST_intern_literal (&id, TTALPH_FIN_NAME);
  NTT_loc_final (ntt) = TTALPH_item (loclabs, id);

  /* set all items of local labels vector to LOCAL OTHER */

  for (i=0; i< locnum; i++)
    TTALPH_item (loclabs, i) = NTT_loc_other (ntt);

  /* copy again IDs to local labels vector; now free IDs are handled */

  for (i=0; i< globnum; i++)
    TTALPH_item (loclabs, TTALPH_item (globlabs, i)) = i;

  return (loclabs);
}


void NTT_set_table_flags (NETptr net, NTTptr ntt)
{
  /* Sets table flags according to global labels. */

  TTLABptr ttlab = TTLAB_alloc_label_structure ();
  ALPHABETptr netlabels = binary_to_label (NET_labels (net));
  id_type id;
  int   i, len = ALPH_max (netlabels);
    
  NTT_simple_fsm (ntt) = TRUE;
  NTT_single_upper (ntt) = TRUE;
  NTT_single_lower (ntt) = TRUE;
  NTT_max_upper (ntt) = 0;
  NTT_max_lower (ntt) = 0;

  for (i=0; i< len; i++)
    {
      id = ALPH_item (netlabels, i);
      if ((id == EPSILON) || (id == OTHER))  continue;

      TTLAB_get_label_information (id, ttlab);

      if (! (TTLAB_simple (ttlab)))  NTT_simple_fsm (ntt) = FALSE;
      if (! (TTLAB_single_upper (ttlab)))  NTT_single_upper (ntt) = FALSE;
      if (! (TTLAB_single_lower (ttlab)))  NTT_single_lower (ntt) = FALSE;

      if (NTT_max_upper (ntt) < TTLAB_upper_len (ttlab))
	NTT_max_upper (ntt) = TTLAB_upper_len (ttlab);

      if (NTT_max_lower (ntt) < TTLAB_lower_len (ttlab))
	NTT_max_lower (ntt) = TTLAB_lower_len (ttlab);
    }

  /* if all label have length 1, apart of the final label,
     then NTT_max_upper and NTT_max_lower must be 1 */

  if ((NTT_max_upper (ntt) > 0) && (NTT_single_upper (ntt)))
    NTT_max_upper (ntt) = 1;

  if ((NTT_max_lower (ntt) > 0) && (NTT_single_lower (ntt)))
    NTT_max_lower (ntt) = 1;

  TTLAB_free_label_structure (ttlab);
}


void NTT_enumerate_states (NETptr net)
{
  STATEptr state;  long num;  

  NTT_num_of_state (NET_start_state (net)) = 0;

  for (num=1, state = NET_states (net); state; state = next_state (state))
    if (state != NET_start_state (net))
      NTT_num_of_state (state) = num++;
}

void NTT_fsm_state_to_auxtable (STATEptr state, NTTptr ntt, TTAUXptr aux)
{
  /* Writes to the auxiliary table the infomation concerning one state
     in the network: state number, final flag, number of outgoing arcs,
     their IDs and destinations. */

  ARCptr arc;  
  id_type id;
  id_type *local = TTALPH_items (NTT_loc_labs (ntt));
  long globmax = TTALPH_num (NTT_loc_labs (ntt));

  TTAUX_cur_state (aux) = NTT_num_of_state (state);
  TTAUX_final (aux) = STATE_final (state);

  TTB_init_long_vector
    (TTAUX_tvec (aux), TTAUX_tvec_len (aux), NTT_NO_STATE);

  if (TTAUX_final (aux))   /* state is final */
    {                                               /* cyclic final arc */
      TTAUX_tvec (aux) [NTT_loc_final (ntt)] = TTAUX_cur_state (aux);
      TTAUX_arcs_in_vec (aux) = 1;
    }
  else                     /* state is not final */
    TTAUX_arcs_in_vec (aux) = 0;

  for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
    {
      id = ARC_label (arc);
      if (id >= globmax)
	{ fsm_fprintf (stderr, "ERROR: higher ID than expected\n"); exit(1); }

      TTAUX_tvec (aux) [local [id]] =
	NTT_num_of_state (ARC_destination (arc));

      TTAUX_arcs_in_vec (aux)++;
    }
}

long NTT_other_side_to_vector (ARCptr arc, TTAUXptr ttaux, NTTptr ntt)
{
  long newsize, idx, base = TTAUX_num_other (ttaux);

  /* return negative base if no arc list */

  if (! arc)
    return (NTT_NO_STATE);

  /* write arcs to the vectors */

  for (idx = base;  ; idx++, arc = next_arc (arc))
    {
      if (idx >= TTAUX_size_other (ttaux))
	{
	  newsize = (long)(NTT_FACTOR_REALLOC * TTAUX_size_other (ttaux));
	  TTAUX_size_other (ttaux) = newsize;
	  NTT_other_len (ntt) = newsize;

	  NTT_other_labs (ntt) =
	    TTB_realloc_idtype_vector (NTT_other_labs (ntt), newsize);
	  NTT_other_dests (ntt) =
	    TTB_realloc_long_vector (NTT_other_dests (ntt), newsize);
	}

      if (! arc)  /* no more arcs to write to the vectors */
	{
	  NTT_other_lab (ntt, idx) = NTT_NO_ID;
	  NTT_other_dest (ntt, idx) = NTT_NO_STATE;
	  break;
	}

      NTT_other_lab (ntt, idx) = ARC_label (arc);
      NTT_other_dest (ntt, idx) = NTT_num_of_state (ARC_destination (arc));
    }

  TTAUX_num_other (ttaux) = idx + 1;

  return (base);
}

void NTT_free_aux_arc_list (ARCptr arc)
{
  ARCptr next;

  for ( ; arc; arc = next)
    {
      next = next_arc (arc);
      free_arc (arc);
    }
}

void NTT_fst_state_to_auxtable (STATEptr state, NTTptr ntt, TTAUXptr aux)
{
  /* Writes to the auxiliary table the infomation concerning one state
     in the network: state number, final flag, number of outgoing arcs,
     their IDs and destinations. */

  ARCptr arc, upper_arc, lower_arc,
         *up_arc_vec = (ARCptr *) TTAUX_tvec_upper (aux),
         *low_arc_vec = (ARCptr *) TTAUX_tvec_lower (aux);
  id_type id, *local = TTALPH_items (NTT_loc_labs (ntt));
  long globmax = TTALPH_num (NTT_loc_labs (ntt));
  TTLABptr ttlab = TTLAB_alloc_label_structure ();

  TTAUX_cur_state (aux) = NTT_num_of_state (state);
  TTAUX_final (aux) = STATE_final (state);

  TTB_init_long_vector
    (TTAUX_tvec (aux), TTAUX_tvec_len (aux), NTT_NO_STATE);
  TTB_init_pointer_vector
    ((void **)up_arc_vec, TTAUX_tvec_len (aux), NULL);
  TTB_init_pointer_vector
    ((void **)low_arc_vec, TTAUX_tvec_len (aux), NULL);

  /* make aux lists of upper and lower arcs and link them to the arc vectors*/

  for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
    {
      id = ARC_label (arc);
      if (id >= globmax)
	{ fsm_fprintf (stderr, "ERROR: higher ID than expected\n"); exit(1); }

      TTLAB_get_label_information (id, ttlab);

      /* make an upper arc with the lower ID (other_side ID) */

      upper_arc =
	arc_alloc (local [TTLAB_lower_id (ttlab)], ARC_destination (arc));

      next_arc (upper_arc) = up_arc_vec [local [TTLAB_upper_id (ttlab)]];
      up_arc_vec [local [TTLAB_upper_id (ttlab)]] = upper_arc;

      TTAUX_tvec (aux) [local [TTLAB_upper_id (ttlab)]] = 1; /*mark position*/

      /* make a lower arc with the upper ID (other_side ID) */

      lower_arc =
	arc_alloc (local [TTLAB_upper_id (ttlab)], ARC_destination (arc));

      next_arc (lower_arc) = low_arc_vec [local [TTLAB_lower_id (ttlab)]];
      low_arc_vec [local [TTLAB_lower_id (ttlab)]] = lower_arc;

      TTAUX_tvec (aux) [local [TTLAB_lower_id (ttlab)]] = 1; /*mark position*/
    }

  /* add final arcs if required */

  if (TTAUX_final (aux))   /* state is final */
    {                                               /* cyclic final arc */
      upper_arc = arc_alloc (NTT_loc_final (ntt), state);
      next_arc (upper_arc) = NULL;
      up_arc_vec [NTT_loc_final (ntt)] = upper_arc;

      lower_arc = arc_alloc (NTT_loc_final (ntt), state);
      next_arc (lower_arc) = NULL;
      low_arc_vec [NTT_loc_final (ntt)] = lower_arc;

      TTAUX_tvec (aux) [NTT_loc_final (ntt)] = 1; /*mark position*/
    }

  /* write the other-side labels and destinations to the transition table */

  TTAUX_arcs_in_vec (aux) = 0;

  for (id = 0; id < TTAUX_tvec_len (aux); id++)
    {
      if (TTAUX_tvec (aux) [id] == NTT_NO_ID)   /* position not marked */
	{
	  TTAUX_tvec_upper (aux) [id] = NTT_NO_STATE;
	  TTAUX_tvec_lower (aux) [id] = NTT_NO_STATE;
	  continue;
	}

      TTAUX_arcs_in_vec (aux)++;

      upper_arc = up_arc_vec [id];
      lower_arc = low_arc_vec [id];

      TTAUX_tvec_upper (aux) [id] =
	NTT_other_side_to_vector (upper_arc, aux, ntt);

      TTAUX_tvec_lower (aux) [id] =
	NTT_other_side_to_vector (lower_arc, aux, ntt);

      NTT_free_aux_arc_list (upper_arc);
      NTT_free_aux_arc_list (lower_arc);
    }

  TTLAB_free_label_structure (ttlab);
}


void NTT_analyze_1st_arc (long arc_1st, TTAUXptr aux, NTTptr ntt)
{
  /* Analyzes whether the index of the 1st arc can be used. */

  long i, len, arcs_equal = 0, arcs_not_copy = 0;
  long *vec = TTAUX_tvec (aux), *dest = NTT_dests (ntt) + arc_1st;
  id_type *label = NTT_labs (ntt) + arc_1st;

  len = min_of (TTAUX_tvec_len (aux), TTAUX_num_arcs (aux) - arc_1st);
                                          /*remaining length of aux vector*/
  for (i=0; i < len; i++)
    if (label [i] != NTT_NO_ID)     /* any type of arc found in ntt */
      {
	if (label [i] == i)        /* arc belongs to current position */
	  {
	    if (! NTT_simple_fsm (ntt))
	      { TTAUX_confl (aux) = TRUE;  return; }   /* conflicting arc */

	    if (dest [i] != vec [i])
	      { TTAUX_confl (aux) = TRUE;  return; }   /* conflicting arc */

	    arcs_equal++;    /* arcs in ntt and vec are equal */
	  }
	else         /* arc does not belong to current position */
	  {
	    if (vec [i] >= 0)  /* one arc in vec found */
	      arcs_not_copy++;   /* arc cannot be copied from vec to ntt */
	  }
      }

  TTAUX_confl (aux) = FALSE;

  if ((len > 0) && (arcs_equal > 0)) /* some arcs found in rem. used length */
    TTAUX_used (aux) = TRUE;     /* position of 1st arc already used */
  else
    TTAUX_used (aux) = FALSE;     /* position of 1st arc still not used */

  if (TTAUX_used (aux) && (TTAUX_arcs_in_vec (aux) == arcs_equal))
    TTAUX_identical (aux) = TRUE;
  else
    TTAUX_identical (aux) = FALSE;

  if (TTAUX_used (aux))
    TTAUX_remain (aux) = TTAUX_arcs_in_vec (aux) - arcs_equal;
  else
    TTAUX_remain (aux) = arcs_not_copy;       /* number of remaining arcs*/

  if (TTAUX_remain (aux) == TTAUX_arcs_in_vec (aux))
    { TTAUX_useless (aux) = TRUE;  return; }  /* no arcs can be copied */
  TTAUX_useless (aux) = FALSE;
}


long NTT_calc_max_remaining_arcs (TTAUXptr aux)
{
  /* Calulates how manu arcs are allowed to remain in the transition vector
     of the current state. HEURISTICAL FUNCTION. */

  long maxremain;

  maxremain = (long)(TTAUX_arcs_in_vec (aux) * NTT_FACTOR_REMAIN);

  return (maxremain);
}


void NTT_handle_next_pos_vector (TTAUXptr aux)
{
  /* Handles the vector of next positions for the search of 1st arcs. */

  long i;

  for (i=1; i< TTAUX_tvec_len (aux); i++)
    if (TTAUX_next_pos (aux, i) < TTAUX_next_pos (aux, i-1))
      TTAUX_next_pos (aux, i) = TTAUX_next_pos (aux, i-1);
}


long NTT_find_new_1st_arc (TTAUXptr aux, NTTptr ntt)
{
  /* Looks for an index of 1st arc where maximally 'maxremain' arcs
     can not be copied. */

  long arc_1st, start, last_posit, maxremain;

  /* calculate number of arcs that are allowed to remain in the
     transition vector */

  maxremain = NTT_calc_max_remaining_arcs (aux);

  /* take start index from vector TTAUX_next_pos_vec(aux) */

  start = TTAUX_next_pos (aux, TTAUX_arcs_in_vec (aux) - 1);
  last_posit = TTAUX_num_arcs (aux);  /* == 1st position outside the range */

  /* look for a 1st-arc position */

  for (arc_1st = start; arc_1st <= last_posit; arc_1st++)
    {
      NTT_analyze_1st_arc (arc_1st, aux, ntt);

      if (TTAUX_confl (aux) == TRUE)  continue;
      if (TTAUX_useless (aux) == TRUE)  continue;
      if (TTAUX_remain (aux) > maxremain)  continue;

      break;      /* new 1st arc found */
    }

  if (arc_1st > last_posit)
    {
      fsm_fprintf (stderr, "ERROR in function 'NTT_find_new_1st_arc'\n");
      exit (1);
    }

  /* handle vector TTAUX_next_pos_vector(aux) */

  TTAUX_next_pos (aux, TTAUX_arcs_in_vec (aux) - 1) = arc_1st;
  NTT_handle_next_pos_vector (aux);

  /* return position of 1st arc */

  return (arc_1st); 
}


void NTT_check_realloc_ntt (TTAUXptr aux, NTTptr ntt)
{
  /* Checks whether there is enough memory to write the current information
     of the auxiliary table to the transition table. */

  long oldsize, newsize, neededsize, diffsize;

  /* memory needed for states */

  oldsize = TTAUX_size_states (aux);
  neededsize = TTAUX_base (aux) + 1;

  if (oldsize < neededsize)
    {
      newsize = (long)(NTT_FACTOR_REALLOC * neededsize);
      diffsize = newsize - oldsize;

      NTT_defs (ntt) = TTB_realloc_long_vector (NTT_defs (ntt), newsize);
      NTT_bases (ntt) = TTB_realloc_long_vector (NTT_bases (ntt), newsize);

      TTB_init_long_vector
	(NTT_defs (ntt) + oldsize, diffsize, NTT_NO_STATE);
      TTB_init_long_vector
	(NTT_bases (ntt) + oldsize, diffsize, NTT_NO_STATE);

      TTAUX_size_states (aux) = newsize;
    }

  /* memory needed for arcs */

  oldsize = TTAUX_size_arcs (aux);
  neededsize = TTAUX_1st_arc (aux) + 1 + TTAUX_tvec_len (aux);

  if (oldsize < neededsize)
    {
      newsize = (long)(NTT_FACTOR_REALLOC * neededsize);
      diffsize = newsize - oldsize;

      NTT_dests (ntt) = TTB_realloc_long_vector (NTT_dests (ntt), newsize);
      NTT_labs (ntt) = TTB_realloc_idtype_vector (NTT_labs (ntt), newsize);

      if (NTT_simple_fsm (ntt))
	TTB_init_long_vector
	  (NTT_dests (ntt) + oldsize, diffsize, NTT_NO_STATE);

      TTB_init_idtype_vector
	(NTT_labs (ntt) + oldsize, diffsize, NTT_NO_ID);

      TTAUX_size_arcs (aux) = newsize;
    }
}

void NTT_base_and_arcs_to_ntt (TTAUXptr aux, NTTptr ntt)
{
  /* Writes base and as many arcs as possible without conflict from the
     auxiliary table to the transition table. Modifies TTAUX_arcs_in_vec(aux)
     according to the number of arcs which could not be handled. */

  /* handle base */

  long base = TTAUX_base (aux), arc_1st = TTAUX_1st_arc (aux);
  long i, maxarc = -1, len = TTAUX_tvec_len (aux);
  long *dest = NTT_dests (ntt) + arc_1st,
       *upper = NTT_uppers (ntt) + arc_1st,
       *lower = NTT_lowers (ntt) + arc_1st,
       *vec = TTAUX_tvec (aux);
  id_type *label = NTT_labs (ntt) + arc_1st;

  if (TTAUX_num_states (aux) < base + 1)
    TTAUX_num_states (aux) = base + 1;    /* correct state counter */

  NTT_base (ntt, base) = arc_1st;    /* base points to the 1st arc */

  /* handle arcs; case: position identical. */

  if (TTAUX_identical (aux) == TRUE)
    {                      /* delete all arc in vec; copying not required */
      TTB_init_long_vector
	(TTAUX_tvec (aux), TTAUX_tvec_len (aux), NTT_NO_STATE);
      TTAUX_arcs_in_vec (aux) = 0;
      return;
    }

  /* handle arcs; case: position already used but not identical. */

  if (TTAUX_used (aux) == TRUE)
    {
      for (i=0; i< len; i++)
	if (label [i] == i)   /* arc belongs to current position */
	  {
	    if ((! NTT_simple_fsm (ntt)) || (dest [i] != vec [i]))/*conflict*/
	      {
		fsm_fprintf (stderr,
			 "ERROR in function 'NTT_base_and_arcs_to_ntt'\n");
		exit (1);
	      }
	    vec [i] = NTT_NO_STATE;   /* delete arc in vec -> copied */
	    TTAUX_arcs_in_vec (aux)--;
	    maxarc = i;       /* highest arc number */
	  }
    }

  /* handle arcs. case: position still not used. */

  if (TTAUX_used (aux) == FALSE)
    {
      for (i=0; i< len; i++)
	if (label [i] == NTT_NO_ID)       /* empty place found in ntt */
	  if (vec [i] >= 0)      /* arc found in vec */
	    {
	      label [i] = i;               /* write arc label */

	      if (NTT_simple_fsm (ntt))
		dest [i] = vec [i];       /* copy arc from vec to ntt */
	      else
		{
		  upper [i] = TTAUX_tvec_upper (aux) [i];
		  lower [i] = TTAUX_tvec_lower (aux) [i];
		}

	      vec [i] = NTT_NO_STATE;      /* delete arc in vec */
	      TTAUX_arcs_in_vec (aux)--;
	      maxarc = i;       /* highest arc number */
	    }
    }

  /* handle total number of arcs in ntt */

  if (TTAUX_num_arcs (aux) < arc_1st + maxarc + 1)
    TTAUX_num_arcs (aux) = arc_1st + maxarc + 1;  /* correct arc counter */
}

long NTT_find_new_base (TTAUXptr aux, NTTptr ntt)
{
  if (TTAUX_num_states (aux) < TTAUX_net_states (aux))
    return (TTAUX_net_states (aux));
  else
    return (TTAUX_num_states (aux));
}

void NTT_auxtable_to_ntt (TTAUXptr aux, NTTptr ntt)
{
  /* Writes the complete information concerning one network states form
     the  auxiliary table to the transition table. */

  long base, arc_1st;

  base = TTAUX_cur_state (aux);  /* 1st base is current state number */
  TTAUX_used (aux) = FALSE;

  if (TTAUX_arcs_in_vec (aux) <= 0)  /* state has no arcs */
    { NTT_base (ntt, base) = NTT_NO_STATE;  return; }

  while (TRUE)
    {
      TTAUX_base (aux) = base;
      arc_1st = NTT_find_new_1st_arc (aux, ntt); /*returns index of 1st arc*/
      TTAUX_1st_arc (aux) = arc_1st;

      NTT_check_realloc_ntt (aux, ntt);
      NTT_base_and_arcs_to_ntt (aux, ntt);

      if (TTAUX_arcs_in_vec (aux) <= 0)  return;

      base = NTT_find_new_base (aux, ntt);  /* returns index of base */
      NTT_def (ntt, TTAUX_base (aux)) = base;
    }
}

void NTT_final_work_on_ntt (TTAUXptr aux, NTTptr ntt)
{
  /* Reduces the size of the transition table to what actually is needed.
     Clears some items of the defaults vector. Sets variables. */

  NTT_num_states (ntt) = TTAUX_num_states (aux);
  NTT_num_arcs (ntt) = TTAUX_num_arcs (aux);
  NTT_other_len (ntt) = TTAUX_num_other (aux);

  NTT_defs (ntt) =
    TTB_realloc_long_vector (NTT_defs (ntt), NTT_num_states (ntt));
  NTT_bases (ntt) =
    TTB_realloc_long_vector (NTT_bases (ntt), NTT_num_states (ntt));
  NTT_labs (ntt) =
    TTB_realloc_idtype_vector (NTT_labs (ntt), NTT_num_arcs (ntt));

  if (NTT_simple_fsm (ntt))
    {
      NTT_dests (ntt) =
	TTB_realloc_long_vector (NTT_dests (ntt), NTT_num_arcs (ntt));
    }
  else
    {
      NTT_uppers (ntt) =
	TTB_realloc_long_vector (NTT_uppers (ntt), NTT_num_arcs (ntt));
      NTT_lowers (ntt) =
	TTB_realloc_long_vector (NTT_lowers (ntt), NTT_num_arcs (ntt));
      NTT_other_labs (ntt) =
	TTB_realloc_idtype_vector (NTT_other_labs (ntt), NTT_other_len (ntt));
      NTT_other_dests (ntt) =
	TTB_realloc_long_vector (NTT_other_dests (ntt), NTT_other_len (ntt));
    }
}


void NTT_conversion_print_begin (TTAUXptr aux)
{
  /* Begins the conversion-print. */

  if (TTAUX_net_states (aux) < NTT_PRINT_FACTOR)  return;

  TTAUX_print (aux) = TTAUX_net_states (aux) / NTT_PRINT_FACTOR;
  fsm_fprintf (stderr, "%s\n", NTT_PRINT_HEAD);
}


void NTT_conversion_print (TTAUXptr aux)
{
  /* Prints a NTT_PRINT_SYMBOL after every n handled states. */

  if (TTAUX_net_states (aux) < NTT_PRINT_FACTOR)  return;

  if ((TTAUX_cur_state (aux) > 0)
      && (TTAUX_cur_state (aux) % TTAUX_print (aux) == 0))
    { fsm_fprintf (stderr, "%c", NTT_PRINT_SYMBOL);  fflush (stderr); }
}


void NTT_convert_net_to_table (NETptr net, NTTptr ntt)
{
  /* Converts a network to a transition table. */

  TTAUXptr aux;  STATEptr state;

  aux = NTT_alloc_ttaux (net, ntt);
  NTT_enumerate_states (net);
  NTT_conversion_print_begin (aux);

  for (state = NET_states (net); state; state = next_state (state))
    {
      if (NTT_simple_fsm (ntt))
	NTT_fsm_state_to_auxtable (state, ntt, aux);
      else                              /* net is an fst */
	NTT_fst_state_to_auxtable (state, ntt, aux);

      NTT_auxtable_to_ntt (aux, ntt);
      NTT_conversion_print (aux);  /* print how many states are converted */
    }

  fsm_fprintf (stderr, "\n");  /* end of conversion-print */

  NTT_final_work_on_ntt (aux, ntt);

  clear_client_cells (net);    /* delete state numbers */
  NTT_free_ttaux (aux);
}


NTTptr NTT_alloc_transition_table (NETptr net)
{
  /* Allocates and initializes a transition table for a network. */

  NTTptr ntt;

  ntt = (NTTptr) malloc (sizeof (NTTtype));
  if (! ntt)
    not_enough_memory("NTT_alloc_transition_table");

  NTT_set_table_flags (net, ntt);

  NTT_glob_labs (ntt) = NTT_make_glob_labels (net);
  NTT_loc_labs (ntt) = NTT_make_local_labels (ntt);

  NTT_net_states (ntt) = NET_num_states (net);
  NTT_net_arcs (ntt) = NET_num_arcs (net);

  NTT_num_states (ntt) = (long)(NTT_FACTOR_STATES * NET_num_states (net));
  NTT_num_arcs (ntt) = (long)(NTT_FACTOR_ARCS * NET_num_arcs (net));
  NTT_other_len (ntt) = NTT_num_arcs (ntt);

  NTT_defs (ntt) = TTB_alloc_long_vector (NTT_num_states (ntt));
  NTT_bases (ntt) = TTB_alloc_long_vector (NTT_num_states (ntt));
  NTT_labs (ntt) = TTB_alloc_idtype_vector (NTT_num_arcs (ntt));

  TTB_init_long_vector
    (NTT_defs (ntt), NTT_num_states (ntt), NTT_NO_STATE);
  TTB_init_long_vector
    (NTT_bases (ntt), NTT_num_states (ntt), NTT_NO_STATE);
  TTB_init_idtype_vector
    (NTT_labs (ntt), NTT_num_arcs (ntt), NTT_NO_ID);

  if (NTT_simple_fsm (ntt))
    {
      NTT_dests (ntt) = TTB_alloc_long_vector (NTT_num_arcs (ntt));
      NTT_uppers (ntt) = NULL;
      NTT_lowers (ntt) = NULL;
      NTT_other_labs (ntt) = NULL;
      NTT_other_dests (ntt) = NULL;

      TTB_init_long_vector
	(NTT_dests (ntt), NTT_num_arcs (ntt), NTT_NO_STATE);
    }
  else
    {
      NTT_dests (ntt) = NULL;
      NTT_uppers (ntt) = TTB_alloc_long_vector (NTT_num_arcs (ntt));
      NTT_lowers (ntt) = TTB_alloc_long_vector (NTT_num_arcs (ntt));
      NTT_other_labs (ntt) = TTB_alloc_idtype_vector (NTT_other_len (ntt));
      NTT_other_dests (ntt) = TTB_alloc_long_vector (NTT_other_len (ntt));

      TTB_init_long_vector
	(NTT_uppers (ntt), NTT_num_arcs (ntt), NTT_NO_STATE);
      TTB_init_long_vector
	(NTT_lowers (ntt), NTT_num_arcs (ntt), NTT_NO_STATE);
      TTB_init_long_vector
	(NTT_other_dests (ntt), NTT_other_len (ntt), NTT_NO_STATE);
    }

  return (ntt);
}


NTTptr NTT_make_transition_table_from_net (NETptr old_net, int reclaim_p)
{
  /* Creates a transition table from a deterministic network. */

  NTTptr ntt;  NETptr new_net;

  if (reclaim_p == DONT_RECLAIM)
    new_net = copy_fsm (old_net);
  else
    new_net = old_net;

  /* prune, minimize and determinize the network first */

  NET_pruned (new_net) = FALSE;
  NET_minimized (new_net) = FALSE;
  NET_deterministic (new_net) = FALSE;

  new_net = prune_fsm (new_net, DONT_COPY);
  new_net = min_fsm (new_net, DONT_COPY);
  new_net = determinize_fsm (new_net, DONT_COPY);

  /* make the conversion */

  ntt = NTT_alloc_transition_table (new_net);
  NTT_convert_net_to_table (new_net, ntt);

  free_network (new_net);

  return (ntt);
}


void NTT_free_transition_table (NTTptr ntt)
{
  /* Frees a transition table. */

  TTALPH_free_alphabet (NTT_glob_labs (ntt));
  TTALPH_free_alphabet (NTT_loc_labs (ntt));

  free (NTT_defs (ntt));
  free (NTT_bases (ntt));
  free (NTT_labs (ntt));

  if (NTT_simple_fsm (ntt))
    {
      free (NTT_dests (ntt));
    }
  else
    {
      free (NTT_uppers (ntt));
      free (NTT_lowers (ntt));
      free (NTT_other_labs (ntt));
      free (NTT_other_dests (ntt));
    }

  free (ntt);
}


/******************************************
 *
 *   CONVERT TRANSITION TABLE TO NETWORK
 *
 ******************************************/

int NTT_is_arclabel_in_arclist (id_type id, ARCptr list)
{
  for ( ; list; list = next_arc (list))
    if (id == ARC_label (list))
      return (TRUE);       /* arc with same label found */

  return (FALSE);   /* not found */
}

void NTT_arc_vector_to_arc_list
  (id_type lab_this_side, int this_side_p, long idx_other_side, NTTptr ntt,
   STATEptr *state_vec, STATEptr state, NETptr net)
{
  ARCptr arc; /* YM, 2/23/96 local var. not used , prev_arc; */
  long *dest_other;
  id_type *lab_other, upper_id, lower_id, pair_id;

  if (idx_other_side < 0)  return;    /* no index */

  lab_other = NTT_other_labs (ntt) + idx_other_side;
  dest_other = NTT_other_dests (ntt) + idx_other_side;

  for ( ; *lab_other != NTT_NO_ID; lab_other++, dest_other++)
    {
      if (this_side_p == TTL_SD_UPPER)
	{
	  upper_id = TTLOOK_local_to_global_id (lab_this_side, ntt);
	  lower_id = TTLOOK_local_to_global_id (*lab_other, ntt);
	}
      else
	{
	  lower_id = TTLOOK_local_to_global_id (lab_this_side, ntt);
	  upper_id = TTLOOK_local_to_global_id (*lab_other, ntt);
	}

      if (upper_id == lower_id)
	pair_id = upper_id;
      else
	make_pair_label (&pair_id, upper_id, lower_id);

      if (! NTT_is_arclabel_in_arclist (pair_id, STATE_arc_set (state)))
	{
	  arc = make_arc (pair_id, state_vec [*dest_other], net);
	  push_arc (arc, state);
	}
    }
}

void NTT_add_arcs_to_net_state
  (NTTptr ntt, STATEptr *states, long state_idx, NETptr net)
{
  id_type glob_label, *label;
  int   num_labels = TTALPH_num (NTT_glob_labs (ntt));
  long i, len, base, arc_1st, *dest, *upper, *lower;
  STATEptr glob_dest, cur_state = states [state_idx];
  ARCptr arc;

  for (base = state_idx; base >= 0; base = NTT_def (ntt, base))
    {
      arc_1st = NTT_base (ntt, base);
      if (arc_1st < 0)
	return;

      label = NTT_labs (ntt) + arc_1st;
      dest = NTT_dests (ntt) + arc_1st;
      upper = NTT_uppers (ntt) + arc_1st;
      lower = NTT_lowers (ntt) + arc_1st;

      len = min_of (NTT_num_arcs (ntt) - arc_1st, num_labels);

      for (i=0; i< len; i++)
	if (label [i] == i)        /* arc belongs to current position */
	  {
	    if (i == NTT_loc_final (ntt))        /* arc is final */
	      { STATE_final (cur_state) = TRUE;  continue; }

	    if (NTT_simple_fsm (ntt))
	      {
		glob_label = TTALPH_item (NTT_glob_labs (ntt), i);
		glob_dest = states [dest [i]];

		arc = make_arc (glob_label, glob_dest, net);
		push_arc (arc, cur_state);
	      }
	    else    /* net is an fst */
	      {
		NTT_arc_vector_to_arc_list
		  (i, TTL_SD_UPPER, upper [i], ntt, states, cur_state, net);

		NTT_arc_vector_to_arc_list
		  (i, TTL_SD_LOWER, lower [i], ntt, states, cur_state, net);
	      }
	  }
    }
}

void NTT_convert_table_to_net (NTTptr ntt, NETptr net)
{
  /* Converts a transition table to a network. */

  long i, num_states = NTT_net_states (ntt);
  STATEptr *states;

  /* make all states and set pointers to them */

  states = (STATEptr *) malloc (num_states * sizeof (STATEptr));
  if (! states)
    not_enough_memory("NTT_convert_table_to_net");

  for (i=0; i< num_states; i++)
    states [i] = make_state (NULL, net);

  NET_start_state (net) = states [0];    /* define start state */

  /* add arcs to every state */

  for (i=0; i< num_states; i++)
    NTT_add_arcs_to_net_state (ntt, states, i, net);

  /* check number of arcs */

  if (NET_num_arcs (net) != NTT_net_arcs (ntt))
    fsm_fprintf (stderr, "ERROR: wrong number of arcs\n");

  free (states);
}

void NTT_generate_net_alphabets (NTTptr ntt, NETptr net)
{
  ALPHABETptr alph;
  id_type id;
  int i, alph_len, new_len;

  /* The net labels are genereted by looking at every arc in the network */

  update_label_alphabet (net);

  /* The net sigma is genereted from the global alph of the table and not
     from the labels of the network since sigma can contain items that are
     excluded from the labels */

  alph_len = TTALPH_num (NTT_glob_labs (ntt));
  alph = make_alph (alph_len, LABEL_VECTOR);

  for (i=0, new_len = 0; i< alph_len; i++)    /* copy all ID of arity 1 */
    {
      if (i == NTT_loc_epsilon (ntt))  continue;  /* do not copy EPSILON */
      if (i == NTT_loc_final (ntt))  continue;  /* do not copy FINAL label */

      id = TTALPH_item (NTT_glob_labs (ntt), i);
      if (LABEL_arity (id_to_label (id)) != 1)  continue;

      ALPH_item (alph, new_len++) = id;
    }

  ALPH_len (alph) = new_len;            /* number of items */
  alph = binary_to_label (label_to_binary (alph));
                                     /* sort items and adjust length */
  free_alph (NET_sigma (net));
  NET_sigma (net) = alph;
}


NETptr NTT_make_net_from_transition_table (NTTptr ntt, int reclaim_p)
{
  /* Creates a deterministic network from a transition table. */

  NETptr net = make_network ();

  NTT_convert_table_to_net (ntt, net);
  NTT_generate_net_alphabets (ntt, net);

  if (reclaim_p == DO_RECLAIM)  NTT_free_transition_table (ntt);

  NET_pruned (net) = FALSE;
  NET_deterministic (net) = FALSE;
  NET_minimized (net) = FALSE;

  net = prune_fsm (net, DONT_COPY);
  net = determinize_fsm (net, DONT_COPY);
  net = min_fsm (net, DONT_COPY);

  return (net);
}


/******************************************
 *
 *   EVALUATION OF A TRANSITION TABLE
 *
 ******************************************/

NTTptr NTT_evaluate_table (NTTptr ntt)
{
  /* Counts states and arcs in a transition table and calculates the
     'density' of items in the corresopnding vectors. */

  long total, num_labs = 0, num_bases = 0, num_defs = 0;
  long i;

  for (i=0; i< NTT_num_arcs (ntt); i++)
    if (NTT_lab (ntt, i) != NTT_NO_ID)  num_labs++;

  for (i=0; i< NTT_num_states (ntt); i++)
    {
      if (NTT_base (ntt, i) >= 0)  num_bases++;
      if (NTT_def (ntt, i) >= 0)  num_defs++;
    }

  fsm_fprintf (stderr, "\nITEM\t\t\tIN_NET\tIN_VEC\tVEC_LEN\tDENSITY\n");

  fsm_fprintf (stderr, "arcs\t\t\t%li\t%li\t%li\t%li %%\n",
	   NTT_net_arcs (ntt), num_labs, NTT_num_arcs (ntt),
	   ((100 * num_labs) / NTT_num_arcs (ntt)));

  if (! NTT_simple_fsm (ntt))
    fsm_fprintf (stderr, "other-side labels\t--\t%li\t%li\t100 %%\n",
	     NTT_other_len (ntt), NTT_other_len (ntt));

  fsm_fprintf (stderr, "states (base)\t\t%li\t%li\t%li\t%li %%\n",
	   NTT_net_states (ntt), num_bases, NTT_num_states (ntt),
	   ((100 * num_bases) / NTT_num_states (ntt)));

  fsm_fprintf (stderr, "states (deflt)\t\t  \t%li\t%li\t%li %%\n",
	   num_defs, NTT_num_states (ntt),
	   ((100 * num_defs) / NTT_num_states (ntt)));

  if (NTT_simple_fsm (ntt))
    total =
      sizeof (struct NET_TRANSITION_TABLE)
	+ 2 * sizeof (struct TRANS_TABLE_ALPHABET)
	  + TTALPH_num (NTT_glob_labs (ntt)) * sizeof (id_type)
	    + TTALPH_num (NTT_loc_labs (ntt)) * sizeof (id_type)
	      + NTT_num_arcs (ntt) * (sizeof (id_type) + sizeof (long))
		+ NTT_num_states (ntt) * (2 * sizeof (long));
  else
    total =
      sizeof (struct NET_TRANSITION_TABLE)
	+ 2 * sizeof (struct TRANS_TABLE_ALPHABET)
	  + TTALPH_num (NTT_glob_labs (ntt)) * sizeof (id_type)
	    + TTALPH_num (NTT_loc_labs (ntt)) * sizeof (id_type)
	      + NTT_num_arcs (ntt) * (sizeof (id_type) + 2 * sizeof (long))
		+ NTT_other_len (ntt) * (sizeof (id_type) + sizeof (long))
		  + NTT_num_states (ntt) * (2 * sizeof (long));

  fsm_fprintf (stderr, "\nMEM SIZE OF TABLE:  %li Bytes\n\n", total);

  return (ntt);
}


/******************************************
 *
 *   SAVE AND READ TRANSITION TABLE
 *
 ******************************************/

int NTT_write_vector (void *vec, long bytes, FILE *file)
{
  /* Writes a vector to the file. */

  if ((fwrite (vec, 1, bytes, file) != bytes) || (ferror (file)))
    return (FALSE);

  fflush (file);
  if (ferror (file))  return (FALSE);

  return (TRUE);    /* writing correct */
}

int NTT_write_string (char *str, FILE *file)
{
  /* Writes a string (and '\0' at its end) to the file. */

  return (NTT_write_vector (str, strlen (str) + 1, file));
}

int NTT_write_label (TTLABptr ttlab, FILE *file)
{
  /* Writes the print name of one label to the file. Uses information
     about the label that is in 'ttlab'. */

  int16 arity;

  /* write arity */

  if (TTLAB_simple (ttlab) == TRUE)  arity = 1;
  else  arity = 2;

  if (! NTT_write_vector (&arity, sizeof (int16), file))  return (FALSE);

  /* write print name */

  if (! NTT_write_string (TTLAB_upper_str (ttlab), file))  return (FALSE);

  if (arity == 2)
    if (! NTT_write_string (TTLAB_lower_str (ttlab), file))  return (FALSE);

  return (TRUE);    /* writing correct */
}

int NTT_write_alphabet (TTALPHptr alph, FILE *file)
{
  /* Writes the global alphabet to the file. */

  uint16 i; 
  int len = TTALPH_num (alph);
  TTLABptr ttlab = TTLAB_alloc_label_structure ();

  /* first write the number of items */

  if (! NTT_write_vector (&len, sizeof (uint16), file))  return (FALSE);

  /* then write all item */

  for (i=0; i< len; i++)
    {
      TTLAB_get_label_information (TTALPH_item (alph, i), ttlab);
      TTLAB_correct_printnames (ttlab, TTLAB_EPSILON);

      if (! NTT_write_label (ttlab, file))  return (FALSE);
    }

  TTLAB_free_label_structure (ttlab);
  return (TRUE);    /* writing correct */
}

int NTT_write_table (NTTptr ntt, FILE *file)
{
  /* Writes a transition table to a file. */

  if (! NTT_write_vector (ntt, sizeof (NTTtype), file))  return (FALSE);

  /* write global alphabet only; the local one can be derived from it */

  if (! NTT_write_alphabet (NTT_glob_labs (ntt), file))  return (FALSE);

  /* write vectors */

  if (! NTT_write_vector (NTT_defs (ntt), NTT_num_states (ntt) *sizeof (long),
			  file))  return (FALSE);
  if (! NTT_write_vector (NTT_bases (ntt), NTT_num_states (ntt) *sizeof (long),
			  file))  return (FALSE);
  if (! NTT_write_vector (NTT_labs (ntt), NTT_num_arcs (ntt) *sizeof (id_type),
			  file))  return (FALSE);

  if (NTT_simple_fsm (ntt))
    {
      if (! NTT_write_vector (NTT_dests (ntt),
			      NTT_num_arcs (ntt) *sizeof (long), file))
	return (FALSE);
    }
  else
    {
      if (! NTT_write_vector (NTT_uppers (ntt),
			      NTT_num_arcs (ntt) *sizeof (long), file))
	return (FALSE);
      if (! NTT_write_vector (NTT_lowers (ntt),
			      NTT_num_arcs (ntt) *sizeof (long), file))
	return (FALSE);
      if (! NTT_write_vector (NTT_other_labs (ntt),
			      NTT_other_len (ntt) *sizeof (id_type), file))
	return (FALSE);
      if (! NTT_write_vector (NTT_other_dests (ntt),
			      NTT_other_len (ntt) *sizeof (long), file))
	return (FALSE);
    }

  if (ferror (file))
    return (FALSE);
  else
    return (TRUE);    /* writing correct */
}


void *NTT_read_vector (long bytes, FILE *file)
{
  /* Reads a vector from a file. */

  void *vec;

  if (! (vec = (void *) malloc (bytes)))
    not_enough_memory("NTT_read_vector");

  if ((fread (vec, 1, bytes, file) != bytes) || (ferror (file)))
    { free (vec);  return (NULL); }

  return (vec);    /* reading correct */
}

int NTT_read_string (char *str, int maxlen, FILE *file)
{
  /* Reads a string (and '\0' at its end) from a file. */

  int i;

  for (i=0; i< maxlen; i++, str++)
    {
      *str = (unsigned char) fsm_fgetc (file);

      if ((feof (file)) || (ferror (file)))  return (FALSE);
      if (*str == '\0')  return (TRUE);    /* reading correctly finished */
    }

  return (FALSE);    /* string is too long */
}

int NTT_read_label (TTLABptr ttlab, FILE *file)
{
  /* Reads the print name of one label from the file. Puts information
     about the label into 'ttlab'. */

  int16 arity;

  /* read arity */

  if ((fread (&arity, 1, sizeof (int16), file) != sizeof (int16))
      || (ferror (file)))
    return (FALSE);

  /* read print name */

  if (arity == 1)
    {
      TTLAB_simple (ttlab) = TRUE;
      if (! NTT_read_string (TTLAB_upper_str (ttlab),
			     TTLAB_LABEL_MAXLEN - 1, file))  return (FALSE);
      strcpy (TTLAB_lower_str (ttlab), TTLAB_upper_str (ttlab));
    }
  else  /* arity == 2 */
    {
      TTLAB_simple (ttlab) = FALSE;
      if (! NTT_read_string (TTLAB_upper_str (ttlab),
			     TTLAB_LABEL_MAXLEN - 1, file))  return (FALSE);
      if (! NTT_read_string (TTLAB_lower_str (ttlab),
			     TTLAB_LABEL_MAXLEN - 1, file))  return (FALSE);
    }

  return (TRUE);    /* reading correct */
}

TTALPHptr NTT_read_alphabet (FILE *file)
{
  /* Allocates and reads the global alphabet from the file. */

  TTALPHptr alph;
  uint16 i;
  int len;
  TTLABptr ttlab = TTLAB_alloc_label_structure ();

  /* first read the number of items */

  if ((fread (&len, 1, sizeof (uint16), file) != sizeof (uint16))
      || (ferror (file)))
    return (NULL);

  /* allocate the alphabet */

  alph = (TTALPHptr) malloc (sizeof (TTALPHtype));
  if (! alph)
    not_enough_memory("NTT_read_alphabet");

  TTALPH_items (alph) = (id_type *) malloc (len * sizeof (id_type));
  if (! TTALPH_items (alph))
    not_enough_memory("NTT_read_alphabet");

  TTALPH_num (alph) = len;

  /* read all items and write them to the alphabet */

  for (i=0; i< len; i++)
    {
      if (! NTT_read_label (ttlab, file))  return (NULL);

      TTLAB_get_id_of_label (ttlab);
      TTALPH_item (alph, i) = TTLAB_id (ttlab);
    }

  TTLAB_free_label_structure (ttlab);
  return (alph);    /* reading correct */
}


NTTptr NTT_read_table (FILE *file)
{
  /* Reads a transition table from a file. */

  NTTptr ntt;

  /* allocate and read table */

  if (! (ntt = (NTTptr) NTT_read_vector (sizeof (NTTtype), file)))
    return (NULL);

  /* allocate and read global alphabet */

  if (! (NTT_glob_labs (ntt) = NTT_read_alphabet (file)))  return (NULL);

  /* derive local alphabet from the global alphabet */

  NTT_loc_labs (ntt) = NTT_make_local_labels (ntt);

  /* allocate and read vectors */

  if (! (NTT_defs (ntt) = (long *)
	 NTT_read_vector (NTT_num_states (ntt) * sizeof (long), file)))
    return (NULL);

  if (! (NTT_bases (ntt) = (long *)
	 NTT_read_vector (NTT_num_states (ntt) * sizeof (long), file)))
    return (NULL);

  if (! (NTT_labs (ntt) = (id_type *)
	 NTT_read_vector (NTT_num_arcs (ntt) * sizeof (id_type), file)))
    return (NULL);

  if (NTT_simple_fsm (ntt))
    {
      if (! (NTT_dests (ntt) = (long *)
	     NTT_read_vector (NTT_num_arcs (ntt) * sizeof (long), file)))
	return (NULL);
    }
  else
    {
      if (! (NTT_uppers (ntt) = (long *)
	     NTT_read_vector (NTT_num_arcs (ntt) * sizeof (long), file)))
	return (NULL);
      if (! (NTT_lowers (ntt) = (long *)
	     NTT_read_vector (NTT_num_arcs (ntt) * sizeof (long), file)))
	return (NULL);
      if (! (NTT_other_labs (ntt) = (id_type *)
	     NTT_read_vector (NTT_other_len (ntt) * sizeof (id_type), file)))
	return (NULL);
      if (! (NTT_other_dests (ntt) = (long *)
	     NTT_read_vector (NTT_other_len (ntt) * sizeof (long), file)))
	return (NULL);
    }

  if (ferror (file))
    return (NULL);
  else
    return (ntt);    /* reading correct */
}


/******************************************
 *
 *   LOOKDOWN / LOOKUP IN A TRANSITION TABLE
 *
 ******************************************/

id_type TTLOOK_get_first_id_from_string
  (char *str, char *buffer, int maxlen, int *lablen)
{
  /* Returns the first ID from a string and informs about the length of
     the first label 'lablen'. */

  FAT_STR fs;  
  id_type id;  
  int len;

  len = strlen (str);
  if (len > maxlen)  len = maxlen;

  memcpy (buffer, str, len);     /* longest possible label to buffer */

  /* search for a label longer than 1 */

  for (; len > 1; len--)
    {
      buffer [len] = '\0';
      fs = FST_thin_to_temp_fat (buffer);
      atomic_label_to_id (&id, fs);

      if (id == ID_NO_SYMBOL)  continue;    /* label does not exist */

      *lablen = len;       /* real length of label */

      return (id);     /* ID found and ok */
    }

  /* if label longer than 1 not found then get a label of length 1 */

  buffer [1] = '\0';
  fs = FST_thin_to_temp_fat (buffer);
  intern_atomic_label (&id, fs);
  *lablen = 1 ;       /* real length of label */

  return (id);     /* ID found or created */
}

id_type *TTLOOK_convert_string_to_vector (char *str, int side_in, NTTptr ntt)
{
  /* Allocates an id_type-vector and fills it with an ID sequence
     corresponding to the string. */

  id_type *vec, *next_id, id;  
  char *buffer;  
  int maxlen, len;

  /* init variables, alloc memory */

  switch (side_in)      /* input side */
    {
    case TTL_SD_UPPER:  maxlen = NTT_max_upper (ntt);  break;
    case TTL_SD_LOWER:  maxlen = NTT_max_lower (ntt);  break;
    default:
      fsm_fprintf (stderr, "ERROR: wrong input side parameter %d\n", side_in);
      fsm_fprintf (stderr, "in function 'TTLOOK_convert_string_to_vector'\n");
      exit (1);
    }

  vec = TTB_alloc_idtype_vector (strlen (str) + 1);
  buffer = (char *) malloc ((maxlen + 1) * sizeof (char));
  if (! buffer)
    not_enough_memory("TTLOOK_convert_string_to_vector");

  /* convert string to vector */

  for (next_id = vec; *str; str += len)
    {
      id = TTLOOK_get_first_id_from_string (str, buffer, maxlen, &len);

      *next_id++ = id;     /* put ID to vector */
    }

  *next_id++ = TTLOOK_NO_ID;     /* mark end of vector */

  len = next_id - vec;       /* real length of vector */
  vec = TTB_realloc_idtype_vector (vec, len);

  free (buffer);

  return (vec);
}

long TTLOOK_get_next_fsm_state (id_type id, long state, NTTptr ntt)
{
  /* Returns the state (if there is one) that can be reached from the
     current state in the transition table on an arc labeled with the
     given ID. */

  long base, arc;

  for ( ; ; state = NTT_def (ntt, state))
    {
      if (state < 0)                      /* state does not exist */
	break;

      if ((base = NTT_base (ntt, state)) < 0)  /* state has no arcs */
	break;

      if ((arc = base + id) >= NTT_num_arcs (ntt))  /* arc out of range */
	continue;

      if (NTT_lab (ntt, arc) == id)     /* arc found */
	return (NTT_dest (ntt, arc));
    }

  return ((long) NTT_NO_STATE);
}

STATEptr TTLOOK_generate_tail_fsm
  (id_type *inp, int side_out, long nttstate, NTTptr ntt)
{
  /* Maps the remaining IDs in the input vector to the transition table
     which is a simple fsm, beginning with 'nttstate' and generates if
     possible and required the tail of the corresponding network path.
     EPSILON is not handled since a simple fsm is supposed to be EPS-free. */

  STATEptr tail;  ARCptr arc;
  long next_nttstate;  
  id_type id_in, id_out;

  /* get local input ID */

  if (*inp == TTLOOK_NO_ID)
    id_in = NTT_loc_final (ntt);    /* ID of FINAL arc */
  else
    id_in = TTLOOK_global_to_local_id (*inp, ntt);

  /* map the current ID to the transition table */

  next_nttstate = TTLOOK_get_next_fsm_state (id_in, nttstate, ntt);
  if (next_nttstate < 0)  return (NULL);   /* no path for current ID */

  /* if FINAL arc found then return a final state (if required) */

  if (id_in == NTT_loc_final (ntt))
    {
      if (side_out == TTL_SD_NONE)   /* generation of network not required */
	return ((STATEptr) 1);     /* non-NULL pointer */

      tail = alloc_state();          /* not included in any network */
      STATE_type_bit (tail) = 1;   /* Arcs = 0, States = 1, for ALTCHAINS */
      STATE_deterministic (tail) = TRUE;
      STATE_final (tail) = TRUE;

      return (tail);
    }

  /* try to generate a tail for the current ID */

  tail = TTLOOK_generate_tail_fsm
    (inp+1, side_out, next_nttstate, ntt);
  if (! tail)  return (NULL);     /* no tail for current ID */

  /* find the current output ID */

  if (side_out == TTL_SD_NONE)
    return ((STATEptr) 1);   /* return non-NULL pointer; no output required */

  id_out = TTALPH_item (NTT_glob_labs (ntt), id_in); /* global output ID */
  if (id_out == OTHER)
    id_out = *inp;       /* OTHER should not be written to the output */

  /* make arc and state for the tail */

  arc = arc_alloc (id_out, tail);    /* not included in any network */

  tail = alloc_state();          /* not included in any network */
  STATE_type_bit (tail) = 1;   /* Arcs = 0, States = 1, for ALTCHAINS */
  STATE_final (tail) = FALSE;

  STATE_arc_set (tail) = arc;

  if (! next_arc (arc))  /* a state with less than 2 arcs is deterministic */
    STATE_deterministic (tail) = TRUE;

  return (tail);    /* tail for current input ID */
}

long TTLOOK_get_next_fst_index (id_type id, int side_in, long state, NTTptr ntt)
{
  /* Returns the upper/lower-side index of the arc list (if there is one)
     that can be reached with the given ID, from the current state in the
     transition table. */

  long base, arc;

  for ( ; ; state = NTT_def (ntt, state))
    {
      if (state < 0)                      /* state does not exist */
	break;

      if ((base = NTT_base (ntt, state)) < 0)  /* state has no arcs */
	break;

      if ((arc = base + id) >= NTT_num_arcs (ntt))  /* arc out of range */
	continue;

      if (NTT_lab (ntt, arc) == id)     /* arc found */
	{
	  if (side_in == TTL_SD_UPPER)
	    return (NTT_upper (ntt, arc));
	  else
	    return (NTT_lower (ntt, arc));
	}
    }

  return ((long) NTT_NO_STATE);
}

STATEptr TTLOOK_generate_tail_fst
  (id_type *inp, int side_in, int side_out, int eps_recurs,
   long nttstate, NTTptr ntt)
{
  /* Maps the remaining IDs in the input vector to the transition table
     which is a transducer, beginning with 'nttstate' and generates
     if possible and required the tail of the corresponding network path.
     Between all IDs recursive insertion of EPSILON up to a predefined
     maximum is allowed. */

  STATEptr tail;  ARCptr arc, arclist;
  long idx_other, *dest;
  int16 offset, final_flag; 
  id_type id_out, id_in, *lab;

  /* if EPSILON recursion too deep then return NULL */

  if (eps_recurs > TTLOOK_MAX_EPSREC)
    {
      fsm_fprintf (stderr, "EPSILON recursion exceeds limit of %d\n",
	       TTLOOK_MAX_EPSREC);
      return (NULL);
    }

  /* if end of input then look for FINAL arc otherwise look for input ID */

  if (*inp == TTLOOK_NO_ID)
    id_in = NTT_loc_final (ntt);             /* local ID of FINAL arc */
  else
    id_in = TTLOOK_global_to_local_id (*inp, ntt);  /* local input ID */

  /* 1st: look for input ID, 2nd: look for EPSILON */

  arclist = NULL;  final_flag = FALSE;

  for (offset = 1; offset >= 0; offset--, id_in = NTT_loc_epsilon (ntt))
    {
      /* get start index of alternative output IDs */

      if ((idx_other =
	   TTLOOK_get_next_fst_index (id_in, side_in, nttstate, ntt)) <0)
	continue;      /* input ID cannot be mapped */

      if (*(lab = NTT_other_labs (ntt) + idx_other) == NTT_NO_ID)  /*other-side labels*/
	continue;      /* no other-side labels */

      dest = NTT_other_dests (ntt) + idx_other;   /* start of destinations */

      /* if final arc found then set final_flag (if output required) */

      if (id_in == NTT_loc_final (ntt))    /* local ID of FINAL arc */
	{
	  if (side_out == TTL_SD_NONE)  /* output network not required */
	    return ((STATEptr) 1);     /* non-NULL pointer */

	  final_flag = TRUE;
	  break;    /* do not generate a tail after FINAL arc */
	}

      /* check pathes for all alternative IDs in id_ntt */

      for ( ; ; lab++, dest++)
	{
	  if (*lab == NTT_NO_ID)     /* end of output label sequence */
	    break;

	  /* try to generate a tail for the current output label */

	  if (offset == 0)                     /* working with EPSILON */
	    tail = TTLOOK_generate_tail_fst
	      (inp, side_in, side_out, eps_recurs+1, *dest, ntt);
	  else                             /* working with real input ID */
	    tail = TTLOOK_generate_tail_fst
	      (inp+1, side_in, side_out, eps_recurs, *dest, ntt);

	  if (! tail)  continue;     /* no tail for current output label */

	  /* tail exists; make ID for current output arc */

	  switch (side_out)
	    {
	    case TTL_SD_NONE:
	      return ((STATEptr) 1); /* return non-NULL; no output required */
	    case TTL_SD_BOTH:
	      if (side_in == TTL_SD_UPPER)
		  make_pair_label (&id_out, TTLOOK_local_to_global_id (id_in, ntt),
				   TTLOOK_local_to_global_id (*lab, ntt));
	      else
		  make_pair_label (&id_out, TTLOOK_local_to_global_id (*lab, ntt),
				   TTLOOK_local_to_global_id (id_in, ntt));
	      break;
	    default:
	      if (side_out == side_in)
		id_out = TTLOOK_local_to_global_id (id_in, ntt);
	      else
		id_out = TTLOOK_local_to_global_id (*lab, ntt);
	      break;
	    }

	  if (id_out == OTHER)
	    id_out = *inp;

	  arc = arc_alloc (id_out, tail);  /* not included in any network */
	  next_arc (arc) = arclist;
	  arclist = arc;            /* put tail of current ID to arclist */
	}
    }

  /* make state for alternative tails */

  if ((! arclist) && (final_flag == FALSE))
    return (NULL);            /* no tails existing and state not final */

  tail = alloc_state();          /* not included in any network */
  STATE_type_bit (tail) = 1;   /* Arcs = 0, States = 1 */
  STATE_final (tail) = final_flag;

  STATE_arc_set (tail) = arclist;

  if (!arclist || !next_arc(arclist))   /* state with < 2 arcs is determ. */
    STATE_deterministic (tail) = TRUE;

  return (tail);    /* all tails for current input ID */
}

void TTLOOK_make_outnet_statelist (STATEptr state, NETptr net)
{
  /* Puts the current state and all states that can be reached form here
     to the state list of the network and counts them and their arcs. */

  ARCptr arc;

  NET_num_states (net)++;        /* count current state */

  next_state (state) = NET_states (net); /* put current state to state list */
  NET_states (net) = state;

  for (arc = STATE_arc_set (state); arc; arc = next_arc (arc))
    {
      NET_num_arcs (net)++;        /* count current arc */
      TTLOOK_make_outnet_statelist (ARC_destination (arc), net);
    }                              /* handle following states and arcs */
}

void TTLOOK_make_outnet_alphabets (NETptr net, NTTptr ntt)
{
  /* Makes the alphabets of the output network. */

  STATEptr state;  ARCptr arc;  TUPLEptr tuple;
  int i, id, other_flag = FALSE, net_arity = 1;

  /* get labels by looking at all arc labels */

  NET_labels (net) = make_alph (global_alph_size(), BINARY_VECTOR);

  for (state = NET_states(net); state; state = next_state(state))
    for (arc = STATE_arc_set(state); arc; arc = next_arc(arc))
      {
	id = ARC_label(arc);
	if (id == EPSILON)  continue;  /* do not add EPSILON */
	if (id == OTHER)  other_flag = TRUE;

	ALPH_item (NET_labels (net), id) = 1;    /* add ID to labels */

	if (LABEL_arity (id_to_label (id)) == 2)
	  {
	    net_arity = 2;
	    tuple = LABEL_tuple (id_to_label (id));

	    if ((TUPLE_labels (tuple) [UPPER] == OTHER)
		|| (TUPLE_labels (tuple) [LOWER] == OTHER))
	      other_flag = TRUE;
	  }
      }

  binary_to_label (NET_labels (net));

  /* case: labels contain OTHER --> get sigma from transition table */

  if (other_flag == TRUE)
    {
      NET_sigma (net) = make_alph (global_alph_size(), BINARY_VECTOR);

      for (i=0; i< TTALPH_num (NTT_glob_labs (ntt)); i++)
	{
	  if (i == NTT_loc_epsilon (ntt))  continue;
	  if (i == NTT_loc_final (ntt))  continue;

	  id = TTALPH_item (NTT_glob_labs (ntt), i);

	  if (LABEL_arity (id_to_label (id)) == 1)
	    ALPH_item (NET_sigma (net), id) = 1;    /* add ID to sigma */
	}
    }

  /* case: labels do not contain OTHER --> get sigma from labels */

  if (other_flag == FALSE)
    NET_sigma (net) = label_sigma (net);

  /* final steps */

  binary_to_label (NET_sigma (net));
  NET_arc_label_arity (net) = net_arity;
}

NETptr TTLOOK_lookup_vector_to_net
  (id_type *vec_in, int side_in, int side_out, NTTptr ntt)
{
  /* Maps the IDs in the vector to the transition table and generates
     a network if possible and required. */

  NETptr net_out;  STATEptr seq_out;

  /* make output sequence of states and arcs */

  if (NTT_simple_fsm (ntt))
    seq_out = TTLOOK_generate_tail_fsm (vec_in, side_out, (long) 0, ntt);
  else
    seq_out = TTLOOK_generate_tail_fst
      (vec_in, side_in, side_out, 0, (long) 0, ntt);

  if (! seq_out)  return (NULL);    /* input could not be mapped */

  if (side_out == TTL_SD_NONE)
    return ((NETptr) 1);   /* non-NULL pointer; no output required */

  /* include the output sequence into network; update everything */

  net_out = make_network ();
  NET_start_state (net_out) = seq_out;
                         /* update statelist and number of states and arcs */
  TTLOOK_make_outnet_statelist (seq_out, net_out);
  TTLOOK_make_outnet_alphabets (net_out, ntt);

  /* prune, determinize and minimize the resulting network */

  NET_pruned (net_out) = FALSE;
  NET_deterministic (net_out) = FALSE;
  NET_minimized (net_out) = FALSE;

  net_out = prune_fsm (net_out, DONT_COPY);
  net_out = determinize_fsm (net_out, DONT_COPY);
  net_out = min_fsm (net_out, DONT_COPY);

  return (net_out);
}

/******************************************
 *
 *   LOOK FOR LONGEST MATCH OF A STRING PREFIX
 *
 ******************************************/

int TTLOOK_continue_longest_match_fsm
  (char *str, char *buffer, int maxlabel, int cur_prefix,
   long nttstate, NTTptr ntt)
{
  /* Matches the beginning of 'str' to the trans. table of a simple fsm. */

  id_type id;  
  int len_label, new_prefix = -1;  long next_nttstate;

  /* case: string is finished */

  if (! *str)
    {
      if (TTLOOK_get_next_fsm_state (NTT_loc_final (ntt), nttstate, ntt) >= 0)
	return (cur_prefix);
      else
	return (-1);  /* state is final */
    }

  /* continue matching */

  id = TTLOOK_get_first_id_from_string (str, buffer, maxlabel, &len_label);
  id = TTLOOK_global_to_local_id (id, ntt);

  next_nttstate = TTLOOK_get_next_fsm_state (id, nttstate, ntt);

  if (next_nttstate >= 0)
    new_prefix = TTLOOK_continue_longest_match_fsm
      (str + len_label, buffer, maxlabel, cur_prefix + len_label,
       next_nttstate, ntt);

  if (new_prefix >= 0)
    return (new_prefix);

  /* if there was no longer prefix, try whether the current position
     could be a prefix end. */

  if (TTLOOK_get_next_fsm_state (NTT_loc_final (ntt), nttstate, ntt) >= 0)
    return (cur_prefix);
  else
    return (-1);  /* state is final */
}

int TTLOOK_continue_longest_match_fst
  (char *str, char *buffer, int maxlabel, int cur_prefix, int side,
   int eps_recurs, long nttstate, NTTptr ntt)
{
  /* Matches the beginning of 'str' to the trans. table of a transducer. */

  long *dest, idx;
  int16 i;
  id_type id;
  int len_label = 0, prefix, new_prefix = -1;

  /* case: EPSILON recursion too deep */

  if (eps_recurs > TTLOOK_MAX_EPSREC)  return (NTT_NO_STATE);

  /* continue matching */

  if (! *str)              /* case: string is finished */
    id = NTT_loc_final (ntt);
  else               /* get next ID */
    {
      id =TTLOOK_get_first_id_from_string (str, buffer, maxlabel, &len_label);
      id = TTLOOK_global_to_local_id (id, ntt);
    }

  /* 1st: look for input ID, 2nd: look for EPSILON */

  for (i=0; i<2; i++, id = NTT_loc_epsilon (ntt))
    {
      /* get sequence of alternative destination states */

      if ((idx = TTLOOK_get_next_fst_index (id, side, nttstate, ntt)) < 0)
	continue;    /* id cannot be mapped */

      /* check path for all alternative destination states */

      for (dest = NTT_other_dests (ntt) + idx ; *dest >= 0; dest++)
	{
	  /* case: FINAL arc required and found */

	  if (id == NTT_loc_final (ntt))
	    return (cur_prefix);

	  /* continue matching with the current ID */

	  if (i == 1)                     /* working with EPSILON */
	    prefix = TTLOOK_continue_longest_match_fst
	      (str, buffer, maxlabel, cur_prefix,
	       side, eps_recurs + 1, *dest, ntt);
	  else                             /* working with real input ID */
	    prefix = TTLOOK_continue_longest_match_fst
	      (str + len_label, buffer, maxlabel, cur_prefix + len_label,
	       side, eps_recurs, *dest, ntt);

	  new_prefix = max_of (new_prefix, prefix);
	}
    }

  if (new_prefix >= 0)
    return (new_prefix);

  /* if there was no longer prefix, try whether the current position
     could be a prefix end. */

  id = NTT_loc_final (ntt);

  if ((idx = TTLOOK_get_next_fst_index (id, side, nttstate, ntt)) < 0)
    return (-1);     /* state is not final */

  if (*(dest = NTT_other_dests (ntt) + idx)  < 0)
    return (-1);     /* state is not final */

  return (cur_prefix);       /* state is final */
}

int TTLOOK_get_longest_match (char *str, int side, NTTptr ntt)
{
  /* Returns the length of the longest match of the prefix of 'str'
     to the transition table. */

  char *buffer;  int maxlabel, len_prefix;

  /* init variables, alloc memory */

  switch (side)      /* input side */
    {
    case TTL_SD_UPPER:  maxlabel = NTT_max_upper (ntt);  break;
    case TTL_SD_LOWER:  maxlabel = NTT_max_lower (ntt);  break;
    default:
      fsm_fprintf (stderr, "ERROR: wrong input side parameter %d\n", side);
      fsm_fprintf (stderr, "in function 'TTLOOK_find_longest_match'\n");
      exit (1);
    }

  buffer = (char *) malloc ((maxlabel + 1) * sizeof (char));
  if (! buffer)
    not_enough_memory("TTLOOK_get_longest_match");

  if (NTT_simple_fsm (ntt))
    len_prefix = TTLOOK_continue_longest_match_fsm
      (str, buffer, maxlabel, 0, 0, ntt);
  else
    len_prefix = TTLOOK_continue_longest_match_fst
      (str, buffer, maxlabel, 0, side, 0, 0, ntt);

  if (len_prefix < 0)  len_prefix = 0;

  free (buffer);
  return (len_prefix);
}

/******************************************
 *
 *   INCLUSION OF THE TRANSITION TABLE STRUCTURE
 *   INTO THE NETWORK STRUCTURE
 *
 ******************************************/

void TTB_incorporate_ttable_in_ttnet (NTTptr ntt, NETptr ttnet)
{
  /* Incorporates the transition table into the network. Frees the
     states and arcs of the network. */

  STATEptr state, next_state;  ARCptr arc, next_arc;

  /* free states and arcs of the net */

  for (state = NET_states (ttnet); state != NULL; state = next_state)
    {
      next_state = next_state (state);
      if (STATE_vector_p (state))
	{
	  free_arc_vector (STATE_arc_vector(state), ttnet);
	}
      else
	for (arc = STATE_arc_set (state); arc != NULL; arc = next_arc)
	  {
	    next_arc = next_arc (arc);
	    free_arc (arc);
	  }
      free_state (state);
    }

  /* incorporates the net and set variables */

  NET_tabular (ttnet) = TRUE;
  NET_compacted (ttnet) = TRUE;
  NET_states (ttnet) = NULL;
  NET_start_state (ttnet) = NULL;
  NET_num_states (ttnet) = NTT_num_states (ntt);
  NET_num_arcs (ttnet) = NTT_num_arcs (ntt);
  NET_states(ttnet) = (STATEptr) ntt;
}

NTTptr TTB_excorporate_ttable_from_ttnet (NETptr ttnet)
{
  /* Returns the transition table from ttnet and puts an epsilon network
     into ttnet. */

  NTTptr ntt;  STATEptr state;

  /* take table out of the net */

  ntt = TTB_ttable (ttnet);
  NET_states(ttnet) = NULL;

  /* put an epsilon network into the net and set variables */

  NET_tabular (ttnet) = FALSE;
  NET_compacted (ttnet) = FALSE;
  NET_num_states (ttnet) = 0;
  NET_num_arcs (ttnet) = 0;

  state = make_state (NULL, ttnet);
  STATE_final (state) = TRUE;

  NET_states (ttnet) = state;
  NET_start_state (ttnet) = state;

  return (ntt);
}

NETptr TTB_net_to_ttnet (NETptr net, int reclaim_p)
{
  NETptr ttnet, auxnet;  STATEptr statelist, startstate;
  NTTptr ntt;  long numstates, numarcs;

  if (NET_tabular (net) == TRUE)
    return (net);      /* it is already a transition table */
  
  /* save temporarily some net data */

  statelist = NET_states (net);
  startstate = NET_start_state (net);
  numstates = NET_num_states (net);
  numarcs = NET_num_arcs (net);

  /* replace net states temporarily by an epsilon net and copy the net */

  auxnet = epsilon_fsm();

  NET_states (net) = NET_states (auxnet);
  NET_start_state (net) = NET_start_state (auxnet);
  NET_num_states (net) = NET_num_states (auxnet);
  NET_num_arcs (net) = NET_num_arcs (auxnet);

  ttnet = copy_fsm (net);  /* copy without original states and arcs */
  free_network (auxnet);

  /* put original data back to the net */

  NET_states (net) = statelist;
  NET_start_state (net) = startstate;
  NET_num_states (net) = numstates;
  NET_num_arcs (net) = numarcs;

  NET_sorted_arcs(net) = FALSE;

  /* make table and incorporate it into the network */

  ntt = NTT_make_transition_table_from_net (net, reclaim_p);
  TTB_incorporate_ttable_in_ttnet (ntt, ttnet);

  return (ttnet);
}

NETptr TTB_ttnet_to_net (NETptr ttnet, int reclaim_p)
{
  NETptr net;  NTTptr ntt = TTB_ttable (ttnet);

  if (NET_tabular (ttnet) == FALSE)
    return (ttnet);      /* it is not a transition table */

  net = NTT_make_net_from_transition_table (ntt, DONT_RECLAIM);

  /* copy flags etc. */

  TTB_flags (net) = TTB_flags (ttnet);
  NET_tabular (net) = FALSE;
  NET_compacted (net) = FALSE;

  NET_arc_label_arity (net) = NET_arc_label_arity (ttnet);
  NET_range_len (net) = NET_range_len (ttnet);

  NET_arc_vector_len (net) = NET_arc_vector_len (ttnet);
  NET_block_size (net) = NET_block_size (ttnet);

  /* smash network if required */

  if (reclaim_p == DO_RECLAIM)
    free_network (ttnet);

  return (net);
}

NETptr TTB_lookup_ttnet_make_net
  (char *word, int input_side, int output_side, NETptr ttnet)
{
  /* Returns a network or NULL as result of the lookup. */

  NTTptr ntt;  NETptr result;  
  id_type *vec;

  if (! NET_tabular (ttnet))
    {
      fsm_fprintf (stderr, "ERROR in 'TTB_lookup_ttnet_make_net':\n");
      fsm_fprintf (stderr, "    Net not tabular.\n");
      exit (1);
    }

  ntt = TTB_ttable (ttnet);

  switch (input_side)
    {
    case UPPER:  input_side = TTL_SD_UPPER;  break;
    case LOWER:  input_side = TTL_SD_LOWER;  break;
    default:
      fsm_fprintf (stderr, "ERROR in 'TTB_lookup_ttnet_make_net':\n");
      fsm_fprintf (stderr, "    Wrong input side: %d.\n", input_side);
      exit (1);
    }

  switch (output_side)
    {
    case UPPER:  output_side = TTL_SD_UPPER;  break;
    case LOWER:  output_side = TTL_SD_LOWER;  break;
    case BOTH_SIDES:  output_side = TTL_SD_BOTH;  break;
    default:
      fsm_fprintf (stderr, "ERROR in 'TTB_lookup_ttnet_make_net':\n");
      fsm_fprintf (stderr, "    Wrong output side: %d.\n", output_side);
      exit (1);
    }

  vec = TTLOOK_convert_string_to_vector (word, input_side, ntt);

  result =
    TTLOOK_lookup_vector_to_net (vec, input_side, output_side, ntt);

  free (vec);  /* free ID vector */

  return (result);
}

/******************************************
 *
 *   CONNECTION OF TABULAR FUNCTIONS WITH OTHER FUNCTIONS
 *      (e.g. read, write, lookup, etc.)
 *
 ******************************************/

void TABULAR_table_from_stream (FILE *stream, NETptr net)
{
  /* This function should not be called directly. See
       void TABULAR_make_all_function_assignments ()  */

  NTTptr ntt; /*transition table*/

  if (! (ntt = NTT_read_table (stream)))
    handle_error("Read failed.\n", "NTT_read_table", 0);

  TTB_incorporate_ttable_in_ttnet (ntt, net);
}

NTTptr TABULAR_table_from_net (NETptr net)
{
  /* This function should not be called directly. See
       void TABULAR_make_all_function_assignments ()  */

  NTTptr ntt; /*transition table*/

  ntt = TTB_excorporate_ttable_from_ttnet (net);
  NET_tabular (net) = TRUE;   /* net must remain tabular !!! */

  return (ntt);
}

void TABULAR_table_to_file (FILE *stream, NETptr net, void *vntt)
{
  /* This function should not be called directly. See
       void TABULAR_make_all_function_assignments ()  */

  NTTptr ntt; /*transition table*/

  ntt = (NTTptr) vntt;

  if (! NTT_write_table (ntt, stream))
    handle_error("Write failed.\n", "NTT_write_table", 0);

  TTB_incorporate_ttable_in_ttnet (ntt, net);
}

void TABULAR_free_included_table (NETptr net)
{
  /* This function should not be called directly. See
       void TABULAR_make_all_function_assignments ()  */

  NTTptr ntt; /*transition table*/

  ntt = TTB_excorporate_ttable_from_ttnet (net);
  NTT_free_transition_table (ntt);
  free_state (NET_states (net));  /* free the only state */
}

void TABULAR_make_all_function_assignments ()
{
  transition_table_from_stream = TABULAR_table_from_stream;
  transition_table_from_net = (void * (*)(NETptr) ) TABULAR_table_from_net;
  transition_table_to_file = TABULAR_table_to_file;
  free_included_table = TABULAR_free_included_table;
}


#include <time.h>
#include "fst/look.h"
#include "fst/pretty-p.h"

extern id_type OUTPUT_SEQ[WORD_STRING_SIZE + 1];
extern int OUTPUT_SIDE;
extern int RESULT_COUNT;
extern int TEST_P;
extern ALPHABETptr SIGMA;

static void TEST_get_answer (char *question, char *answer)
{
  fsm_fprintf (stdout, "%s", question);  fflush (stdout);
  fflush (stdin);  fsm_gets (answer);
}

void TEST_ntt_one_word_apply (NETptr net)
{
  /* Applies one word many times to the lower side of a network or table. */

  char word [100];  long i, num;  
  id_type *vec = NULL;
  NTTptr ntt = NULL;
  time_t START_TIME, END_TIME;
  PARSE_TBL parse_table;

  /* Preparation */

  TEST_get_answer ("number of repetitions : ", word);
  if (! *word)  return;
  num = atol (word);

  TEST_get_answer ("word                  : ", word);
  if (! *word)  return;

  fsm_fprintf (stdout, "\n'%s' will be mapped %li times\n", word, num);
  fflush (stdout);

  if (NET_tabular (net))
    {
      ntt = TTB_ttable (net);
      vec = TTLOOK_convert_string_to_vector (word, TTL_SD_LOWER, ntt);
    }
  else
    {
      if (!(parse_table = NET_lower_parse_table(net)))
	parse_table = make_parse_table(net, NULL, LOWER);

      TEST_P = TRUE;
      MATCH_TABLE = LOWER_MATCH;
      OUTPUT_SIDE = UPPER;

      if (FST_parse_string(word, parse_table, DONT_ZERO_TO_EPS,
		       DONT_IGNORE_WHITES, DONT_ESCAPE,
		       DO_REPORT_ERROR))
	return;

      SIGMA = label_to_binary (NET_sigma(net));
    }

  RESULT_COUNT = 0;

  /* TEST */

  fsm_fprintf (stdout, "Starting test.\n");
  START_TIME = time(NULL);

  for (i = 0; i < num; i++)
    {
      if (NET_tabular (net))
	{
	  if (TTLOOK_lookup_vector_to_net
	      (vec, TTL_SD_LOWER, TTL_SD_NONE, ntt))
	    RESULT_COUNT = 1;  /* at least 1 result */
	}
      else if (NET_compacted (net))
	{
	  RESULT_COUNT = 0;
	  do_compact_look(net, print_result);
	}
      else
	{
	  RESULT_COUNT = 0;
	  look(INPUT_SEQ, NET_start_state(net),
	       OUTPUT_SEQ, print_result, FALSE);
	}
    }

  /* Final steps */

  END_TIME = time(NULL);

  if (RESULT_COUNT > 0)
    fsm_fprintf (stdout, "Mapping OK\n");
  else
    fsm_fprintf (stdout, "Mapping failed\n");

  print_time (START_TIME, END_TIME);

  if (END_TIME - START_TIME > 0)
    fsm_fprintf(stdout, "%li words/sec.\n", num/(END_TIME - START_TIME));

  if (NET_tabular (net))
    free (vec);
}



