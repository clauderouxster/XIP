/* $Id: eqv-labs.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1990-93 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "fst/struct.h"
#include "fst/eqv-labs.h"
#include "fst/error.h"
#include "fst/fsm_io.h"
#include "fst/sig-prop.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
#include "fst/id_type.h"
#include "fst/lab-vect.h"

#ifdef DEBUG
#include "fst/pretty-p.h"
#endif

/**************************************
 *
 *  EQV-LABS.C
 *
 *  Lauri Karttunen, RXRC
 *  December 1993
 *
 ***************************************/

/* These functions used to be in BASICS.C */

/* Support for EQV_CLASSES, DELETE_EQV_ARCS and RESTORE_EQV_ARCS */


int label_class_order (const void *x, const void *y)
{
  /* Orders two alphabets according to the DEFAULT_LABEL_ORDER
     of their first members.  It is assumed that the same
     ordering has already been applied within the
     alphabets. (Calls to qsort from within qsort are not
     allowed by some implementations.)
     */
  
  ALPHABETptr sig1 = *(ALPHABETptr *) x, sig2 = *(ALPHABETptr *) y;
  if (ALPH_len(sig1) > 0)
    {
      if (ALPH_len(sig2) > 0)
	{
	  id_type id1 = ALPH_item(sig1, 0), id2 = ALPH_item(sig2, 0);
	  return(default_label_order(id1, id2));
	}
      else
	return(1);
    }
  else
    return(-1);
}


ALPH_VECTORptr eqv_classes (NVptr nets)
{
  /* Partitions the label set of NETs to equivalence classes.
     Labels that belong to the same equivalence class always
     appear together and never lead to different destinations.
     Returns an alphabet vector whose length is the number
     of equivalence classes (less or equal to the number of
     labels).  Each position in the vector contains an
     ALPHABET object of the type LABEL_VECTOR which holds
     a set of labels that are equivalent.  The labels within
     an equivalence class are ordered by DEFAULT_LABEL_ORDER
     and the classes themselves are ordered according
     to their first member.
     */
  
  ALPH_VECTORptr eqv_partition;
  ALPHABETptr labs, *classes;
  NETptr net;
  
  CONSptr multi_mem_classes = NULL,
  single_mem_classes = NULL, class = NULL,
  cur, prev, no_dest,
  split_classes, prev_cell, cell;
  
  STATEptr state, head_dest, dest, *dest_vector;
  ARCptr arc;
  int net_count = NV_len(nets);
  int32 i;
  id_type id;
  int j, lab_count = 0;
  ALPHABETptr all_labs = make_alph(LIM_id_count(LABEL_MAP), BINARY_VECTOR);
  
  if (all_labs == NULL)
    not_enough_memory("EQV_CLASSES");
  
  /* Make an inventory of all labels and count them. */
  
  for (i = 0; i < net_count; i++)
    {
      net = nv_get(nets, i);

      labs = binary_to_label(NET_labels(net));
      for (j = 0; j < ALPH_len(labs); j++)
        sigma_add_to(all_labs, ALPH_item(labs, j));
    }
  
  binary_to_label(all_labs);
  
  lab_count = ALPH_len(all_labs);
  
  /* Construct an equivalence class consisting of all the labels */
  
  init_cons_mem();
  
  for (i = lab_count - 1; i >= 0; i--)
    class = cons(cast_to_void(ALPH_item(all_labs, i)), class);
  
  free_alph(all_labs);
  
  dest_vector = (STATEptr *) calloc((size_t) LIM_id_count(LABEL_MAP),
                                    (size_t) sizeof(STATEptr));
  
  if (dest_vector == NULL)
    not_enough_memory("EQV_CLASSES");
  
  if (lab_count > 1)
    multi_mem_classes = cons((void *) class, NULL);
  else if (lab_count == 1)
    single_mem_classes = cons((void *) class, NULL);
  
  for (i = 0; i < net_count; i++)
    for (state = NET_states(NV_net(nets, i));
         state != NULL && multi_mem_classes != NULL;
         state = next_state(state))
      {
        /* First pass. Fill dest_vector with destinations */
        
        for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
          {
            dest = ARC_destination(arc);
            STATE_client_cell(dest) = NULL;
            dest_vector[ARC_label(arc)] = dest;
          }
        
        /* Run through all the classes in the remaining set of multi-member
           classes, see if any of them get split. */
        
        prev_cell = NULL;
        
        for (cell = multi_mem_classes; cell != NULL; cell = cdr(cell))
          { 
            class = (CONSptr) car(cell);
            prev = class;
            no_dest = NULL;
            
            head_dest = dest_vector[(int) car(class)];
            
            for (cur = cdr(class); cur != NULL; cur = cdr(prev))
              {
                id = (id_type) ((int) car(cur));
                dest = dest_vector[id];
                
                if (dest == head_dest)
                  {
                    /* current label agrees with the head label */
                    prev = cur;
                  }
                else
                  {
                    /* remove the label from its current class */
                    
                    cdr(prev) = cdr(cur);
                    
                    if (dest == NULL)
                      {
                        /* no arc for the current label */
                        no_dest = cons(cast_to_void(id), no_dest);
                        
                      }
                    else
                      {
                        /* arc for the current label leads to a state that
                           the head label does not lead to.  See if some
                           other label in the current class has already caused
                           the class to split at the destination state. */
                        
                        split_classes = (CONSptr) STATE_client_cell(dest);
                        
                        if (split_classes == NULL ||
                            ((CONSptr) car((CONSptr) car(split_classes))) !=
                            class)
                          /* new split */
                          STATE_client_cell(dest) =
                            cons((void *) cons((void *) class,
                                               cons(cast_to_void(id), NULL)),
                                 (CONSptr) STATE_client_cell(dest));
                        else
                          /* previous split */
                          cdr((CONSptr) car(split_classes)) =
                            cons(cast_to_void(id),
				 cdr((CONSptr) car(split_classes)));
                      }
                  }      
              }
            
            if (cdr(class) == NULL)
              {
                /* Only one member remains, no further processing needed. */
                
                single_mem_classes = cons((void *) class, single_mem_classes);
                
                if (prev_cell == NULL)
                  multi_mem_classes = cdr(multi_mem_classes);
                else
                  cdr(prev_cell) = cdr(cell);
              }
            else
              prev_cell = cell;
            
            /* Move the no_dest class, if any, to the appropriate place */
            
            if (no_dest != NULL)
	      {
		if (cdr(no_dest) == NULL)
		  single_mem_classes = cons((void *) no_dest, single_mem_classes);
		else
		  {
		    multi_mem_classes = cons((void *) no_dest, multi_mem_classes);
		    if (prev_cell == NULL)
		      prev_cell = multi_mem_classes;
		  }
	      }
          }
        
        /* Second pass. Clear dest_vector with destinations, collect the new
           classes if any, and sort them to single and multi_member classes
           */
        
        for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
          {
            dest = ARC_destination(arc);
            dest_vector[ARC_label(arc)] = NULL;
            
            for(cell = (CONSptr) STATE_client_cell(dest);
                cell != NULL;
                cell = cdr(cell))
              {
                /* discard the parent class */
                
                class = cdr((CONSptr) car(cell));
                
                if (cdr(class) == NULL)
                  single_mem_classes =
                    cons((void *) class, single_mem_classes);
                else
                  {
                    multi_mem_classes =
                      cons((void *) class, multi_mem_classes);
                  }
                STATE_client_cell(dest) = NULL;  
              }
          }
      }
  /* Construct an ordered label vector for each multi-member class
     (I counts the total number of eqv-classes).
     */
  
  i = 0;
  
  for (cell = multi_mem_classes; cell != NULL; cell = cdr(cell))
    { 
      ALPHABETptr eqv_cl =
        make_alph(LIM_id_count(LABEL_MAP), BINARY_VECTOR);  
      
      i++;
      
      for (class = (CONSptr) car(cell); class != NULL; class = cdr(class))
        ALPH_item(eqv_cl, (int) car(class)) = 1;
      
      binary_to_label(eqv_cl);
      
      sort_alph(eqv_cl);
      
      car(cell) = (void *) eqv_cl;
    }
  
  for (cell = single_mem_classes; cell != NULL; cell = cdr(cell))
    {
      ALPHABETptr eqv_cl = make_alph(1, LABEL_VECTOR);
      i++;
      
      ALPH_item(eqv_cl, 0) = (int) car((CONSptr) car(cell));
      
      car(cell) = (void *) eqv_cl;    
    }
  
  eqv_partition = make_alph_vector(i);
  classes = av_alphs(eqv_partition);
  
  i = 0;
  
  for (cell = multi_mem_classes; cell != NULL; cell = cdr(cell))
    classes[i++] = (ALPHABETptr) car(cell);
  
  for (cell = single_mem_classes; cell != NULL; cell = cdr(cell))
    classes[i++] = (ALPHABETptr) car(cell); 
  
  QSORT((void *) classes, (size_t) i, (size_t) sizeof(ALPHABETptr),
        label_class_order);
  
  free_cons_mem();
  free( dest_vector);
  
  /* As we used temporarily the client_cells (and smashed the possible
     original values, except the one of the net start state), we need
     to specify to the caller that one can't rely on the client_cells
     of the input nets now. */
  for (i = 0; i < net_count; i++)
      NET_names_matter(NV_net(nets, i))= 0; 
  
  return(eqv_partition);
}


ALPH_VECTORptr net_eqv_classes(NETptr net)
{
  /* Returns the equivalence class partition for a
     single net.
     */
  
  NVtype nv;
  NETptr nets[1];
  
  nv.len = 1;
  nv.nets = nets;
  nets[0] = net;
  
  return(eqv_classes(&nv));
}

void inforce_singleton_classes
  (ALPH_VECTORptr eqv_partition, ALPHABETptr singletons)
{
  /* Forces all IDs that are in "singletons" into singleton classes.
     If "singletons"==NULL than all IDs are allowed to occur in
     non-singleton classes (default case).
     "eqv_partition" will be modified, "singletons" won't be.
     */

  ALPHABETptr singl_found, class;  
  int i, j, len, oldlen; 
  id_type id;

  if (! singletons)
    return;

  singletons = label_to_binary (copy_alphabet (singletons));
  singl_found = make_alph (ALPH_len (singletons), BINARY_VECTOR);

  /* move singleton IDs form all classes to "singl_found"
     only if they are no already in a singleton class */

  for (i=0;  i< av_len (eqv_partition);  i++)
    {
      class = av_alph (eqv_partition, i);

      if (ALPH_len (class) == 1)  /* singleton class */
	continue;

      for (j=0, len=0;  j< ALPH_len (class);  j++)
	{
	  id = ALPH_item (class, j);

	  if (sigma_member(singletons, id))  /* singleton ID */
	    ALPH_item (singl_found, id) = TRUE;
	  else
	    ALPH_item (class, len++) = id;
	}

      ALPH_len (class) = len;
    }

  singl_found = binary_to_label (singl_found);

  /* remove classes that now are empty */

  for (i=0, len=0;  i< av_len (eqv_partition);  i++)
    {
      class = av_alph (eqv_partition, i);

      if (! ALPH_len (class))  /* empty class */
	free_alph (class);
      else
	av_alph (eqv_partition, len++) = class;
    }

  av_len (eqv_partition) = len;

  /* enhance alphabet vector of "eqv_partition" */

  oldlen = av_len (eqv_partition);
  len = oldlen + ALPH_len (singl_found);

  av_alphs (eqv_partition) = (ALPHABETptr *)
    realloc (av_alphs (eqv_partition), len * sizeof (ALPHABETptr));

  if (! av_alphs (eqv_partition))
    not_enough_memory ("inforce_singleton_classes");

  av_len (eqv_partition) = len;

  /* add new singleton classes to "eqv_partition" */

  for (i=0;  i< ALPH_len (singl_found);  i++)
    {
      class = make_alph (1, LABEL_VECTOR);
      ALPH_item (class, 0) = ALPH_item (singl_found, i);
      av_alph (eqv_partition, oldlen + i) = class;
    }

  /* finish */

  QSORT((void *) av_alphs (eqv_partition), (size_t) av_len (eqv_partition),
	(size_t) sizeof(ALPHABETptr), label_class_order);

  free_alph (singletons);
  free_alph (singl_found);
}

long int *make_eqv_vector(ALPHABETptr alph, ALPH_VECTORptr eqv_partition,
                          int *vector_size)
{
  /* Returns a vector that maps each label position in ALPH to the
     number of the equivalence class (counting from 1 upwards) to which
     the label belongs in EQV_PARTITION.  VECTOR_SIZE is set to the
     length of the vector.  To restore EQV_PARTITION from a vector
     and the alphabet, use MAKE_EQV_CLASSES
     */
  
  ALPHABETptr *class;
  uint16  i, class_count, class_id; 
  id_type *labels, *labs;
  int  j;
  uint16  label_count;
  long int *eqv_vector;
  
  if (alph == NULL || eqv_partition == NULL)
    handle_error("Bad call", "MAKE_EQV_VECTOR", -1);
  
  /* Make sure that ALPH is in LABEL format and properly sorted. */
  
  if (ALPH_type(alph) != LABEL_VECTOR)
    {
      binary_to_label(alph);
      sort_alph(alph);
    }
  
  label_count = ALPH_len(alph);
  labels = ALPH_items(alph);
  
  class = av_alphs(eqv_partition);
  class_count = av_len(eqv_partition);
  
  eqv_vector =
    (long int *) calloc((size_t)label_count, (size_t) sizeof(long int));
  
  if (eqv_vector == NULL)
    not_enough_memory("MAKE_EQV_LABEL_SEQ");
  
  /* Go through all classes in EQV_PARTITION and mark the positions
     in EQV_VECTOR that are occupied by members of the class. Classes
     are counted from 1 upwards.  Positions in EQV_VECTOR that remain
     0 represent labels in ALPH that do not occur in any partition.
     That is anomalous, perhaps, but not reported as an error here. 
     On the other hand, it is an error to find a label in the
     equivalence partition that does not occur in ALPH.
     */
  
  for (i = 0; i < class_count; i++, class++)
    {
      class_id = i + 1;
      for (j = 0; j < ALPH_len(*class); j++)
        {
          int pos = -1, k;
	  id_type id = ALPH_item(*class, j);
          
          for (k = 0, labs = labels; k < label_count; k++)
            if (*labs++ == id)
              {
                pos = k;
                break;
              }
          
          if (pos < 0)
            handle_error("Label not found", "EQV_LABELS", id);
          
          eqv_vector[pos] = (long) class_id;
        }
    }
  *vector_size = label_count;
  return(eqv_vector);
}

ALPH_VECTORptr make_eqv_partition(ALPHABETptr alph, long int *eqv_vector,
                                  int vector_size)
{
  /* The converse of MAKE_EQV_VECTOR.  Constructs an equivalence partition
     from a label alphabet and a vector that maps the symbols of the alphabet
     to the corresponding equivalence classes.  Equivalence classes are
     counted from 1 upwards.
     */
  
  int i, j, count = 0;
  ALPHABETptr *classes, class;
  ALPH_VECTORptr eqv_partition;
  
  if (ALPH_type(alph) != LABEL_VECTOR)
    handle_error("Bad alphabet type", "MAKE_EQV_PARTITION", ALPH_type(alph));
  
  if (ALPH_len(alph) != vector_size)
    handle_error("Unequal lengths", "MAKE_EQV_PARTITION", ALPH_len(alph));
  
  /* Count the number of cells in the partitition. N.B This count ignores
     cells in EQV_VECTOR that contain 0.  Normally there should not be
     any zeros in the vector but it could happen in case ALPH contains symbols
     that do not occur at all in the network in question.
     */
  
  for (i = 0; i < vector_size; i++)
    if (count < (int)eqv_vector[i])
      count = (int) eqv_vector[i];
  
  eqv_partition = make_alph_vector(count);
  classes = av_alphs(eqv_partition);
  
  for (i = 0; i < count; i++)
    {
      class = make_alph(0, LABEL_VECTOR);
      if (class == NULL)
        not_enough_memory("MAKE_EQV_PARTITION");  
      classes[i] = class;
    }
  
  /* Count the number of items in each cell. Watch out for and ignore zero
     cells in EQV_VECTOR. Only act if EQ_VECTOR[i] is greater than 0, that
     is, J, which is one less than it, must be at least 0.
     */
  for (i = 0; i < vector_size; i++)
    {
      j = (int) eqv_vector[i] - 1;
      if (j >= 0)
        {
          class = classes[j];
          ALPH_len(classes[j])++;
        }
    }
  
  /* Make space for the labels and reset the counts back to 0. */
  
  for (j = 0; j < count; j++)
    {
      class = classes[j];
      ALPH_items(class) = (id_type *)
        make_alph_items(ALPH_len(class), &ALPH_max(class));
      ALPH_len(class) = 0;
    }
  
  /* Last pass.  Add the items to the proper classes. Watch out for
     possible zeros again. */
  for (i = 0; i < vector_size; i++)
    {
      j = (int) eqv_vector[i] - 1;
      if (j >= 0)
        {
          class = classes[j];
          ALPH_item(class, ALPH_len(class)++) = ALPH_item(alph, i);
          if (ALPH_len(class) > ALPH_max(class))
            handle_error("ALPH_len bigger than ALPH_max", "MAKE_EQV_PARTITION",
                         ALPH_len(class));
        }
    }
  return(eqv_partition);
}

SEQUENCEptr make_eqv_label_seq(ALPHABETptr labels, ALPH_VECTORptr eqv_partition)
{
  /* Creates a sequence that maps the positions in LABELS to
     equivalence classes (numbered from 1 upwards).  Returns
     a SEQUENCE pointer. If some label does not belong to any cell in the
     partition, a warning is posted.
     */
  
  SEQUENCEptr seq;
  
  binary_to_label(labels);
  
  seq = (SEQUENCEptr) malloc((size_t) sizeof(SEQUENCEtype)); 
  if (seq == NULL)
    not_enough_memory("MAKE_EQV_LABEL_SEQ");
  SEQ_type(seq) = Int;
  SEQ_ints(seq) =
    make_eqv_vector(labels, eqv_partition, &SEQ_len(seq));
  return(seq);
}


void reduce_arcs (NETptr net, int *keep_map, int keep_old_sigma_p)
{
  /* KEEP_MAP is a function from label IDs to TRUE and FALSE.  Arcs
     whose label is mapped to FALSE are removed from the net.
     The label alphabet for the reduced net contains only labels
     that actually occur in the network. If the KEEP_OLD_SIGMA_P
     flag is non-NULL, the original sigma alphabet is retained.
     This simplifies the construction of parse tables and
     the interpretation of OTHER for the apply functions.
     If KEEP_OLD_SIGMA_P is NULL, the labels of the suppressed
     will be eliminated from the sigma. The two-level rule
     compiler needs to truncate the sigma to prevent an
     unwanted expansion of OTHER in the calculus functions.
     1999-05-27 LK.
     */
  
  STATEptr state = NET_states(net);
  ARCptr arc, next, prev;
  uint16 i;
  id_type *lab_items;

  int  lab_lim;
  
  /* If the sigma is to be reduced, we need to keep track
     of symbols that are in the sigma without occurring
     as a label in the net.
     */

  while (state != NULL)
    {
      prev = NULL;
      
      for (arc = STATE_arc_set(state); arc != NULL; arc = next)
        {
          next = next_arc(arc);
          if (keep_map[ARC_label(arc)])
            prev = arc;
          else
            {
              if (prev == NULL)
                STATE_arc_set(state) = next_arc(arc);
              else
                next_arc(prev) = next_arc(arc);
              free_arc(arc);
              NET_num_arcs(net)--;
            }
        }
      state = next_state(state);
    }
  
  /* Remove eliminated symbols from the LABELS alphabets
     of the net.
     */
  
  label_to_binary(NET_labels(net));
  lab_lim = ALPH_len(NET_labels(net));
  lab_items = ALPH_items(NET_labels(net));
  
  for (i = 0; i < lab_lim; i++, lab_items++)
    if (!(keep_map[i]))
      {
	*lab_items = 0;
	
	/* Adjust the sigma alphabet too? */

	if (keep_old_sigma_p == DONT_KEEP)
	  {
	    if (FSM_MODE == TWOL_MODE || !(tuple_id_p(i)))
	      sigma_remove_from(NET_sigma(net), i);
	    else
	      {
		sigma_remove_from(NET_sigma(net), id_to_upper_id(i));
		sigma_remove_from(NET_sigma(net), id_to_lower_id(i));
	      }
	  }
      }

  binary_to_label(NET_labels(net));
}

NETptr delete_eqv_arcs
  (NETptr net, ALPHABETptr singletons, int record_alphabet_p,
   int keep_old_sigma_p)
{
  /* Calls MAKE_EQV_LABEL_SEQ to partition the labels to
     equivalence classes.  Retains arcs that are labeled
     by the first member of an equivalence class and
     deletes the rest.  If every label is in its own
     equivalence class, no action is take.  If some arcs
     are deleted, the mapping from the labels to the equivalence
     classes they belong is retained as a network property under
     the attribute EQV-VECTOR. RESTORE_EQV_ARCS restores the arcs.

     All IDs that are in SINGLETONS are forced into singleton
     classes. If SINGLETONS==NULL than all IDs are allowed to
     occur in non-singleton classes (default case).
     
     N.B. If RECORD_ALPHABET_P is nonzero, the NET_labels field is
     saved as a network property (confusingly but for backwards
     compatibility) under the attribute SIGMA.  If the net is one
     of many nets to be reduced all sharing the same alphabet (as is
     the case with nets produced by the two-level rule compiler),
     it may be sufficient to record the label set only once.

     If KEEP_OLD_SIGMA_P is not NULL, the original unreduced
     sigma is retained. Otherwise the labels of the surpressed
     arcs are removed from the sigma.

     */
  
  SEQUENCEptr seq;
  int i, id, class, seq_len, work_to_do_p = FALSE;
  long int *class_map;
  int *keep_map, *check_mark;
  ALPHABETptr labels;
  ALPH_VECTORptr eqv_partition;
  
  if (NET_reduced_labelset(net))
    {
      fsm_fprintf(stderr, "The labelset has already been reduced.\n");
      return(net);
    }
  
  eqv_partition = net_eqv_classes(net);
  inforce_singleton_classes (eqv_partition, singletons);  /*AK,30/07/99*/

  labels = NET_labels(net);
  sort_alph(labels);
  
  /* Sort the alphabet so that the representative of each class
     is the first symbol in the class in the default ordering.
     */
  
  seq = make_eqv_label_seq(labels, eqv_partition);
  seq_len = SEQ_len(seq);
  class_map = SEQ_ints(seq);
  
  /* Add 1 to SEQ_LEN to add one extra slot to CHECK_MARK array. It is
     needed because the smallest class is 1 rather than 0.
     */
  
  check_mark = (int *) calloc((size_t) seq_len + 1, (size_t) sizeof(int));
  
  keep_map = (int *) calloc((size_t) LIM_id_count(LABEL_MAP),
                            (size_t) sizeof(int));
  
  if (keep_map == NULL || check_mark == NULL)
    not_enough_memory("DELETE_EQV_ARCS");
  
  /* KEEP_MAP maps only the first member of each equivalence class (numbered
     from 1 upwards), to TRUE and maps other labels in the same set to FALSE.
     */
  
  for (i = 0; i < seq_len; i++)
    {
      class = (int) class_map[i] - 1;
      id = ALPH_item(labels, i);
      if (check_mark[class] == 0)
        {
          check_mark[class] = 1;
          keep_map[id] = TRUE;
        }
      else
        {
          keep_map[id] = FALSE;
          work_to_do_p = TRUE;
        }
    }
  
  if (work_to_do_p)
    { 
      /* Record the full alphabet before if gets reduced */
      if (record_alphabet_p)
        {
          sort_alph(labels);
          add_prop(net, SIGMA_ATTR, alph_to_prop_val(labels));
        }
      
      add_array_prop(net, EQV_VECTOR_ATTR, seq);
      reduce_arcs(net, keep_map, keep_old_sigma_p);

      NET_reduced_labelset(net) = TRUE;

      /* If the net's arcs were previously sorted this arc ordering is lost. */
      NET_sorted_arcs(net)= 0;
      }
  else
    {
      /* Nothing to do.  All equivalence classes are
	 singleton sets.
      */
      fsm_fprintf(stdout, "Does not reduce: all equivalence classes");
      fsm_fprintf(stdout, " are singleton sets.\n");
    free_sequence(seq);
    }
  free((void *) keep_map);
  free((void *) check_mark);
  free_alph_vector(eqv_partition);
  
  return(net);
}
#endif /* SLIM_FST */

int get_labels_from_prop_list(NETptr net, ALPHABETptr *labels)
{
  OBJECTptr value = get_value(net, SIGMA_ATTR);
      
 if (value == NULL || OBJ_type(value) != List)
   {
     fsm_fprintf(stderr, "Cannot find the full label alphabet.\n");
     return(1);
   }
   else
     *labels = prop_val_to_alph(value, LABEL_VECTOR);
     
  return(0);
} /* get_labels_from_prop_list */


int get_eqv_map_from_prop_list(NETptr net, SEQUENCEptr *seq)
{
  OBJECTptr value = get_value(net, EQV_VECTOR_ATTR);
  
  if (value == NULL || OBJ_type(value) != Array ||
      SEQ_type(OBJ_array(value)) != Int)
    {
      fsm_fprintf(stderr, "Cannot find the equivalence class map.\n");
      return(1);
    }
  else
  	*seq = OBJ_array(value);
  	
  return(0);
}



ALPHABETptr get_unreduce_key(NETptr net)
{
  /* Makes a binary alphabet in which the positions correspond to the
     label ids of the unreduced network and the id in each position
     is the one that represents the label in the reduced net.
     The network must contain on its property list of all labels and
     and a parallel array of integers that refer, by position, to the
     labels on the list. If the nth position contains the integer k,
     it means that the the nth label in the reduced net is represented
     by the kth label. The positions are numbered from 1 upwards.
  */
  
  SEQUENCEptr seq;
  ALPHABETptr labels, unreduce_key;
  int i, seq_len;
  long int *key;
  int k, n;
  id_type id;
  LAB_VECTORptr sym_vector = make_lab_vector(80);
  
  if (!NET_reduced_labelset(net))
    {
      fsm_fprintf(stderr, "The labelset has not been reduced.\n");
      return(NULL);
    }
  
  if (NET_unreduce_key(net))
    return(NET_unreduce_key(net));
  	
  get_labels_from_prop_list(net, &labels);
  get_eqv_map_from_prop_list(net, &seq);
  
  seq_len = SEQ_len(seq);
  key = SEQ_ints(seq);
  
  if (seq_len != ALPH_len(labels))
    handle_error("Bad length for EQV_VECTOR", "RESTORE_LABEL_SET", seq_len);
    
  unreduce_key = copy_alphabet(labels);
  label_to_binary(unreduce_key);

  
  /* We substract 1 from key[i] because the label positions in
     the labels start at 0 and not at 1. The variable n counts
     the number of labels that will be eliminated. An example:

        labels: 1 8 7 9 10 11 23 24
	key:    1 2 3 4 1  1  5   6

	unreduce_key:     1                 7  8      1   1     23  24
	               0  1  2  3  4  5  6  7  8  9  10  11 ... 23  24
       
	In the loop below, n counts the labels that are mapped
	to some other label, sym_vector collects the labels that
	are mapped to themselves, that is, the heads of the
	equivalence classes.
     */

  n = 0;
  for (i = 0; i < seq_len; i++)
    {
      k = key[i] - 1;
      
      if (k >= LAB_VECTOR_pos(sym_vector))
      	append_to_lab_vector(ALPH_item(labels, k+n), sym_vector);
      else
      	n++;
      	
      lab_vector_element_at(&id, k, sym_vector);

      ALPH_item(unreduce_key, ALPH_item(labels, i)) = id;
    }
  
  free_alph(labels);
  reclaim_lab_vector(sym_vector);
  
  NET_unreduce_key(net) = unreduce_key;
  
  return(unreduce_key);
} /* get_unreduced_key */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

ALPH_VECTORptr get_reduced_to_unreduced_map(NETptr rlnet)
     /* Makes a vector of ALPHABETS indexable by labels of the (reduced!) label
	alphabet of a network (with reduced labelset id-s). The elements - map[id] -
	are ALPHABET-s containing all the corresponding id-s that id
	stands for, sorted so that ALTCHAIN-s be the last.
	This is the "inverse" mapping of the result of
	get_unreduce_key().
	Example: id 21 substitutes the ids  22, 23 and 24. 22 and 27 stand for
	themselves. Id 26 is not present in the unreduced (nor the reduced)
	labels alphabet. 27 is the greatest label id so the size of the
	corresponding ALPH_VECTOR is 28.
	...
	21. -> 3 (size) -> 23,24,25
	22. -> 1 (size) -> 22
	23.  0
	24.  0
	25.  0
	26.  0
	27. -> 1 (size) -> 27
	*/
{
  ALPH_VECTORptr rum=NULL;
  int i;

  if (!(rlnet && NET_reduced_labelset(rlnet)))
    return rum;
  
  if (! NET_unreduce_key(rlnet))
    get_unreduce_key(rlnet);

  /* allocate range vector with the length of the greatest label */

  if (!NET_labels(rlnet))
    update_label_alphabet(rlnet);
  label_to_binary(NET_labels(rlnet));
  rum = calloc( (size_t) 1, (size_t) sizeof(ALPH_VECTORtype));
  av_alphs(rum) = calloc( (size_t) (ALPH_len(NET_labels(rlnet))), (size_t) sizeof(ALPHABETptr));
  av_len(rum) = ALPH_len(NET_labels(rlnet));
  next_av(rum) = NULL;

  for (i=0 ; i < ALPH_len(NET_unreduce_key(rlnet)) ; i++ )
    if (ALPH_item(NET_unreduce_key(rlnet), i))
      {
	/* if element (alphabet) doesn't exist, create with length 0) */
	assert (ALPH_item(NET_unreduce_key(rlnet), i) <= av_len(rum));
	if (!av_alph(rum, ALPH_item(NET_unreduce_key(rlnet), i)))
	  av_alph(rum, ALPH_item(NET_unreduce_key(rlnet), i)) = make_alph(0, BINARY_VECTOR);
	/* add the real label that this reduced label stands for */
	sigma_add_to(av_alph(rum, ALPH_item(NET_unreduce_key(rlnet), i)), (uint16)i);
      }
  
  /* change alphabets to label representation */
  for (i = 0; i < av_len(rum) ; i++)
    {
      if (av_alph(rum, i))
	{
	  binary_to_label(av_alph(rum, i));
	  sort_alph(av_alph(rum, i));
	}
    }
  
  return rum;
}
#endif /* SLIM_FST */

void free_reduced_to_unreduced_map(ALPH_VECTORptr *rum)
{
  int i;
  if (! *rum)
    return;
  for (i = 0; i < av_len(*rum) ; i++)
    if (av_alph(*rum, i))
      free_alph(av_alph(*rum, i));
  free(*rum);
  *rum = 0;
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#ifdef DEBUG
void TEST_print_alphabet(ALPHABETptr aa)
{
  int i, j, savetype=0;

  if (!(aa && ALPH_len(aa)))
    return;

  savetype = ALPH_type(aa);

  if (ALPH_type(aa) == LABEL_VECTOR)
    label_to_binary(aa);

  for (i = 0, j=0; i <  ALPH_len(aa) ; i++)
    {
      if (ALPH_item(aa, i))
	{
	  fsm_fprintf (stderr, " %d=", i);
	  print_label(i, stderr, DO_ESCAPE);
	  j++;
	}		   
    }

  fsm_fprintf (stderr, "\n Size=%d\n", j);

  if (savetype != ALPH_type(aa))
    binary_to_label(aa);  
}

void TEST_rum(ALPH_VECTORptr rum)
{
  int i, j;

  if (! rum)
    {
      fsm_fprintf (stderr, "** TEST_rum: No reduced-to-unreduced mapping vector.\n");
      return;
    }

   fsm_fprintf (stderr, "* TEST_rum: rum=0x%x; length= %d; next= %x\n", rum, av_len(rum), next_av(rum));
   for (i=0, j=1 ; i < av_len(rum) ; i++)
     if  (av_alph(rum, i))
       {
	 fsm_fprintf (stderr, "%d. reduced label (id=%d)=", j++, i);
	 print_label(i, stderr, DO_ESCAPE);
	 fsm_fprintf (stderr, " stands for:\n");
	 TEST_print_alphabet(av_alph(rum, i));
	 /* print_alphabet(av_alph(rum, i), 80, 8, 0 ); */
       }
}
#endif



ALPHABETptr get_unreduced_labels(NETptr rlnet, ALPH_VECTORptr *rum)
     /* Restore the original labels ALPHABET from the reduced labels alphabet
	using a reduced-to-unreduced map vector (of alphabets). If the latter
	doesn't exist, create it. 
	Could be done in a simpler way by using an "unreduce key" alphabet but
	the reduced-to-unreduced map will be needed anyway for arc vectorization
	(where this will be used). */
{
  ALPHABETptr full_labels_alph = NULL;
  int i, j;
 
  if (!(rlnet && NET_reduced_labelset(rlnet)))
    return full_labels_alph;

  if (!NET_labels(rlnet))
    update_label_alphabet(rlnet);

  if (! *rum)
    *rum = get_reduced_to_unreduced_map(rlnet);

  full_labels_alph = make_alph(0, BINARY_VECTOR);
 
  binary_to_label(NET_labels(rlnet));

  for ( i = 0; i <  ALPH_len(NET_labels(rlnet)) ; i++)
    if (av_alph(*rum, ALPH_item(NET_labels(rlnet), i)))
      {
	if ( ALPH_type(av_alph(*rum, ALPH_item(NET_labels(rlnet), i))) != LABEL_VECTOR)
	  binary_to_label(av_alph(*rum, ALPH_item(NET_labels(rlnet), i)));

	for (j=0 ; j < ALPH_len((av_alph(*rum, ALPH_item(NET_labels(rlnet), i)))); j++)
	  {
	    sigma_add_to(full_labels_alph, ALPH_item(av_alph(*rum, ALPH_item(NET_labels(rlnet), i)), j));
	  }
      }

  return full_labels_alph;  
} /* get_unreduced_labels */


NETptr restore_eqv_arcs(NETptr net, ALPHABETptr alph)
{
  /* Restores a network reduced by a previous call to DELETE_EQV_ARCS
     using the network property EQV_VECTOR and ALPH. EQV_VECTOR maps from
     positions in ALPH to equivalence classes.  The first member of each class
     (numbered from 1 upwards) represents the whole class.  Whereever
     there is an arc labeled by the head of the class, new arcs to the
     same destination are added for the remaining labels.  If ALPH is
     NULL, the net must contain it as a network property SIGMA (confusingly
     named so for backwards compatibility).
  */
  
  OBJECTptr value;
  SEQUENCEptr seq;
  ALPHABETptr class, *label_map, labels = alph, net_labels = NET_labels(net);
  STATEptr state;
  ARCptr arc, new_arc;
  long int *key;
  int i, seq_len;
  id_type id, *class_map; 
  uint16 label_count, *class_size; 
  
  if (labels == NULL)
    {
      value = get_value(net, SIGMA_ATTR);
      
      if (value == NULL || OBJ_type(value) != List)
        {
	  if (NET_reduced_labelset(net))
	    fsm_fprintf(stderr, "Cannot restore without the full alphabet\n");
	  else
	    {
	      fsm_fprintf(stderr, "Nothing to do. The labelset has ");
	      fsm_fprintf(stderr, "not been reduced.\n");
	    }

          return(net);
        }
      else
        labels = prop_val_to_alph(value, LABEL_VECTOR);
    }
  
  value = get_value(net, EQV_VECTOR_ATTR);
  
  if (value == NULL || OBJ_type(value) != Array ||
      SEQ_type(OBJ_array(value)) != Int)
    {
      fsm_fprintf(stderr, "No EQV_VECTOR.  Nothing to restore.\n");
      return(net);
    }
  
  seq = OBJ_array(value);
  seq_len = SEQ_len(seq);
  key = SEQ_ints(seq);
  
  if (seq_len != ALPH_len(labels))
    handle_error("Bad length for EQV_VECTOR", "RESTORE_LABEL_SET", seq_len);
  
  label_count = LIM_id_count(LABEL_MAP);
  label_map = (ALPHABETptr *) calloc((size_t) label_count,
                                     (size_t) sizeof(ALPHABETtype));
  
  /* Find out the number of classes (CLASS_COUNT), who represents
     each class (CLASS_MAP) and the size of the class (CLASS_SIZE).
     Make arrays one longer than SEQ_LEN.
  */
  
  class_map = (id_type *) calloc((size_t) seq_len+1, (size_t) sizeof(id_type));
  class_size = (uint16 *) calloc((size_t) seq_len+1, (size_t) sizeof(uint16));
  
  if (label_map == NULL || class_map == NULL || class_size == NULL)
    not_enough_memory("RESTORE_EQV_ARCS");
  
  for (i = 0; i < seq_len; i++)
    if (class_size[(uint16) key[i]] == 0)
      {
        class_map[(uint16) key[i]] = ALPH_item(labels, i);
        class_size[(uint16) key[i]] =1;
      }
    else
      class_size[(uint16) key[i]]++;
  
  /* Construct a ALPHABET vector containing the eclipsed labels
     for each class representative.
  */
  
  for (i = 0; i < seq_len; i++)
    {
      id = class_map[(uint16) key[i]];
      
      if (label_map[id] == NULL)
        {
          if  (class_size[(uint16) key[i]] > 1)
            {
              class = make_alph(label_count, BINARY_VECTOR);
              label_map[id] = class;
            }
        }
      else
        sigma_add_to(label_map[id], ALPH_item(labels,i));
    }
  
  for (i = 0; i < SEQ_len(seq); i++)
    {
      class = label_map[ALPH_item(labels, i)];
      if (class != NULL)
        binary_to_label(class);
    }
  
  /* Add the missing arcs. */
  
  for (state = NET_states(net); state != NULL; state = next_state(state))
    for (arc = STATE_arc_set(state); arc != NULL; arc = next_arc(arc))
      {
        class = label_map[ARC_label(arc)];
        if (class != NULL)
          for (i = 0; i < ALPH_len(class); i++)
            {
	      id = ALPH_item(class, i);
              new_arc = make_arc(id, ARC_destination(arc), net);
              push_arc(new_arc, state);
	      sigma_add_to(net_labels, id);
	      /* The current version of delete_eqv_arcs() does not
		 truncate the sigma if it is called with the
		 keep_old_sigma flag set to DO_KEEP. Previous
		 versions always deleted the sigma. Because we
		 in general don't know whether the sigma is complete
		 we need the following line. 1999-05-27 LK.
		 */
	      label_to_sigma(NET_sigma(net), id);
            }
      }
  
  /* Free workspace */
  
  for (i = 0; i < seq_len; i++)
    if (label_map[ALPH_item(labels, i)] != NULL)
      free_alph(label_map[ALPH_item(labels, i)]);
  
  free((void *) label_map);
  free((void *) class_map);
  free((void *) class_size);
  
  /* remove SIGMA and EQV_VECTOR_ATTR properites & free them */
  
  free_prop(remove_prop(net, SIGMA_ATTR));
  free_prop(remove_prop(net, EQV_VECTOR_ATTR));
   
  /* Update SIGMA and LABEL fields of the net. */
  
  binary_to_label(net_labels);

  if (alph == NULL)
    free_alph(labels);
  	
  /* Reclaim match tables (if any) and unreduce_key */
 
  free_match_table(NET_upmatch_table(net));
  NET_upmatch_table(net) = NULL;

  free_match_table(NET_downmatch_table(net));
  NET_downmatch_table(net) = NULL;
 
  free_alph(NET_unreduce_key(net));
  NET_unreduce_key(net) = NULL;
 	
  NET_reduced_labelset(net) = FALSE;
  
  return(net);
}


/**************************************
 *
 *  SIMPLE WRAPPERS FOR SOME ABOVE FUNCTIONS
 *
 *  Andre Kempe, XRCE, August 1999
 *
 */

static ALPHABETptr build_arc_remove_alph
  (NETptr net, ALPH_VECTORptr eqv_classes)
{
  /* Builds a binary vector of arcs to be removed. These are are labelled
     with an ID that is not the first one in an equivalence class. */

  ALPHABETptr remove, class, labels = NET_labels (net);
  uint16 i, j;
  id_type id, max_id;

  for (i=0, max_id = 0;  i< ALPH_len (labels);  i++)
    max_id = max_of (max_id, ALPH_item (labels, i));

  remove = make_alph (max_id + 1, BINARY_VECTOR);

  for (i=0;  i< av_len (eqv_classes);  i++)
    {
      class = av_alph (eqv_classes, i);

      for (j=0;  j< ALPH_len (class);  j++)
	{
	  if ((id = ALPH_item (class, j)) > max_id)
	    prog_error ("build_arc_remove_alph", "inconsistent arguments");

	  if (j > 0)
	    ALPH_item (remove, id) = TRUE;
	}
    }

  return (remove);
}

NETptr reduce_eqv_arcs (NETptr net, ALPH_VECTORptr eqv_classes)
{
  /* Removes all arcs labelled with an ID that is not the first one
     in an equivalence class. Handles the label and sigma alphabet.
     Note, no trace about label reduction (e.g. flags or recode vector)
     is put into the reduced net.
     The argument "net" is mofified if copy_p==FALSE, but "eqv_classes"
     is never modified. */

  STATEptr state;  ARCptr arc, nextarc, new_arcs;  ALPHABETptr remove;

  remove = build_arc_remove_alph (net, eqv_classes);

  /* remove arcs */

  for (state = NET_states (net);  state;  state = next_state (state))
    {
      new_arcs = NULL;

      for (arc = STATE_arc_set (state);  arc;  arc = nextarc)
	{
	  nextarc = next_arc (arc);

	  if (ALPH_item (remove, ARC_label (arc)))
	    {
	      free_arc (arc);
	      NET_num_arcs (net) --;
	    }
	  else
	    {
	      next_arc (arc) = new_arcs;
	      new_arcs = arc;
	    }
	}

      STATE_arc_set (state) = new_arcs;
    }

  /* finish */

  update_net_labels_and_sigma (net);
  free_alph (remove);

  return (net);
}

#if NOT_USED
static ALPH_VECTORptr OLD_build_eqv_mapping /* TO BE REMOVED. AK */
  (NETptr net, ALPH_VECTORptr eqv_classes)
{
  ALPH_VECTORptr eqv_map;
  ALPHABETptr class, labels = NET_labels (net);
  uint16 i, j;
  id_type id, max_id;

  for (i=0, max_id = 0;  i< ALPH_len (labels);  i++)
    max_id = max_of (max_id, ALPH_item (labels, i));

  eqv_map = make_alph_vector (max_id + 1);
  memset (av_alphs (eqv_map), 0, av_len (eqv_map) * sizeof (ALPHABETptr));

  for (i=0;  i< av_len (eqv_classes);  i++)
    {
      class = av_alph (eqv_classes, i);

      if (ALPH_len (class) < 1)
	prog_error ("build_eqv_mapping", "empty equivalence class found");

      for (j=0;  j< ALPH_len (class);  j++)
	{
	  if ((id = ALPH_item (class, j)) > max_id)
	    prog_error ("build_eqv_mapping", "inconsistent arguments");

	  if ((j == 0) && (ALPH_len (class) > 1))
	    av_alph (eqv_map, id) = class;/* 1st ID of non-singleton class */
	}
    }

  return (eqv_map);
}
#endif /* NOT_USED */

static ALPH_VECTORptr build_eqv_mapping
  (NETptr net, ALPH_VECTORptr eqv_classes)
{
  ALPH_VECTORptr eqv_map;
  ALPHABETptr class, labels = NET_labels (net);
  uint16 i;
  id_type id, max_id;

  for (i=0, max_id = 0;  i< ALPH_len (labels);  i++)
    max_id = max_of (max_id, ALPH_item (labels, i));

  eqv_map = make_alph_vector (max_id + 1);
  memset (av_alphs (eqv_map), 0, av_len (eqv_map) * sizeof (ALPHABETptr));

  for (i=0;  i< av_len (eqv_classes);  i++)
    {
      class = av_alph (eqv_classes, i);

      if (ALPH_len (class) < 1)
	prog_error ("build_eqv_mapping", "empty equivalence class found");

      if ((ALPH_len (class) > 1)  /* non-singleton class */
	  && ((id = ALPH_item (class, 0)) <= max_id))  /* relevant ID */
	av_alph (eqv_map, id) = class;  /* set recode pointer */
    }

  return (eqv_map);
}

NETptr unreduce_eqv_arcs (NETptr net, ALPH_VECTORptr eqv_classes)
{
  /* Inverse function to reduce_eqv_arcs().
     Restores all arcs labelled with an ID that is not the first one
     in an equivalence class. Handles the label and sigma alphabet.
     Note, no trace about label unreduction (e.g. flags)
     is put into the unreduced net.
     The argument "net" is mofified if copy_p==FALSE, but "eqv_classes"
     is never modified. */

  ALPH_VECTORptr eqv_map;  ALPHABETptr class;  int i;
  STATEptr state;  ARCptr arc, nextarc, new_arcs, eqv_arc;

  eqv_map = build_eqv_mapping (net, eqv_classes);

  /* rebuild arcs */

  for (state = NET_states (net);  state;  state = next_state (state))
    {
      new_arcs = NULL;

      for (arc = STATE_arc_set (state);  arc;  arc = nextarc)
	{
	  nextarc = next_arc (arc);
	  next_arc (arc) = new_arcs;
	  new_arcs = arc;

	  if ((class = av_alph (eqv_map, ARC_label (arc))))
	    for (i=1;  i< ALPH_len (class);  i++)
	      {
		eqv_arc = make_arc
		  (ALPH_item (class, i), ARC_destination (arc), net);
		next_arc (eqv_arc) = new_arcs;
		new_arcs = eqv_arc;
	      }
	}

      STATE_arc_set (state) = new_arcs;
    }

  /* finish */

  update_net_labels_and_sigma (net);

  memset (av_alphs (eqv_map), 0, av_len (eqv_map) * sizeof (ALPHABETptr));
  free_alph_vector (eqv_map);

  return (net);
}

#endif /* SLIM_FST */
