/* $Id: vectorize.c,v 1.5 2015/11/19 09:24:27 roux Exp $
 * 
 * Copyright (C) 1994-2004, Xerox Corporation. All rights reserved.
 * 
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "fst/struct.h"
#include "fst/heap.h"
#include "fst/vectorize.h"
#include "fst/fsm_io.h"
#include "fst/eqv-labs.h"
#include "fst/utf8_fatstr_util.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/*******************************************************
 **
 **  vectorize.c
 **
 **	 Todd Yampol, Xerox PARC, January 1993.
 **
 **  Conversion between linked arc sets & arc vectors.
 ** 
 ********************************************************/

/* NOTE:  If STANDARD_VECTORIZE_N is low and the network has a large
   alphabet, but very few arcs per state, then a lot of memory can be
   wasted on sparse arrays.

   If STANDARD_VECTORIZE_N == 1, then all states will be vectorized.
*/

/* minimum number of arcs for vectorization of a state. 
   Integer network property VECTORIZE_N overrides this default.*/

#define STANDARD_VECTORIZE_N 1 

int arc_vector_size(NETptr net)
{
  ALPHABETptr externally_unreduced_net_labels=NULL;
  ALPH_VECTORptr reduced_to_unreduced_arcs_map=NULL;

  if (has_arc_vectors_p(net))
    {
      /* compute from arc vector heap cell size */
      return (HEAP_cell_size(NET_arc_vector_heap(net))/sizeof(STATEptr));
    }
  else
    {
      id_type *items, n=0;
      int i;
      ALPHABETptr net_labels;

      externally_unreduced_net_labels = get_unreduced_labels(net, &reduced_to_unreduced_arcs_map);
  
      if (! externally_unreduced_net_labels)
	net_labels = NET_labels(net);
      else 
	net_labels = externally_unreduced_net_labels;
	  
      /*  If alphabet type is BINARY, just take the length of the
	  vector.  Otherwise, find the highest label id and add 1.
	  */
      if (ALPH_type(net_labels) == 0)
	n = ALPH_len(net_labels);
      else
	{
	  items = ALPH_items(net_labels);
	      
	  for (i = 0 ; i < ALPH_len(net_labels) ; i++)
	    if (items[i] > n)
	      n = items[i];

	  n++;
	}

      free_reduced_to_unreduced_map(&reduced_to_unreduced_arcs_map);
      free_alph(externally_unreduced_net_labels);
      return(n);
    }
}

static ALPHABETptr alph_merge(ALPHABETptr target, ALPHABETptr source)
{
    /* If source is not NULL, copy the labels to target. One or
       both arguments may be NULL. If they exist, they are known
       to be in LABEL_VECTOR format. The label ids are known
       to be distinct.
       */
    if (!source)
        return(target);
    else if (!target)
        return(copy_alphabet(source));
    else
    {
        id_type  *items;
	int i, source_len = ALPH_len(source),
        orig_target_len = ALPH_len(target);

        adjust_alph_size(target, orig_target_len + source_len);
        items = ALPH_items(target) + orig_target_len;

        for (i = 0; i < source_len; i++)
            *items++ = ALPH_item(source, i);
        
        return(target);
    }
}

void make_vector_state (STATEptr state, NETptr net, int vector_size)
{
  AVptr arc_vector;
  ARCptr arc, temp;
  STATEptr *dest_array;
  LABELptr label;
  id_type label_id, upper_id, lower_id ;
  int /*len, i,*/ l_eps_count = 0, u_eps_count = 0,
    l_fld_count = 0, u_fld_count = 0;
  ALPHABETptr l_eps_alph = NULL, u_eps_alph = NULL,
    u_fld_alph = NULL, l_fld_alph = NULL;
  
  if (STATE_vector_p(state) == FALSE)
    {
        arc_vector = (AVptr) cell_alloc(ARC_VECTOR_HEAP);
        dest_array = (STATEptr *) cell_alloc(NET_arc_vector_heap(net));

        if (!(arc_vector && dest_array))
            not_enough_memory("make_vector_state");

        AV_dest_array(arc_vector) = dest_array;

        for (arc = STATE_arc_set(state) ; arc != NULL ; arc = next_arc(arc))
	{
            /* If the arc label is the ALTCHAIN_SYMBOL, we ignore the
               arc and go to its destination state. If the destination
               has already been vectorized, we copy the destinations from
               its destination vector to the current state. If the
               destination has not yet been vectorized, we process its
               arcs as if they belonged to the current state.
               */

            label_id = ARC_label(arc);

            if (label_id == ALTCHAIN_SYMBOL)
            {
                if (next_arc(arc))
                    fsm_fprintf(stdout, "*** Nonfinal ALTCHAIN arc\n");
                else
                {
                    STATEptr dest = ARC_destination(arc);
                    
                    /* No looping ALTCHAIN_SYMBOLS! */

                    assert(state != dest);

                    if (STATE_vector_p(dest))
                    {
                        /* Destination state has already been vectorized.
                           Copy the destination pointers and epsilon
                           alphabets.
                           */
                            
                        int i;
                        AVptr dest_av = STATE_arc_vector(dest);
                        STATEptr *dest_dest_array = AV_dest_array(dest_av);

                        for (i = 0; i < vector_size; i++)
                          if (dest_dest_array[i])
                          {
                              assert(!(dest_array[i]));
                              dest_array[i] = dest_dest_array[i];
                          }

                        /* Note: one or both of the arguments to
                           alph_merge() might be NULL. */

                        u_eps_alph = alph_merge(u_eps_alph, AV_u_eps(dest_av));

                        if (u_eps_alph)
                            u_eps_count = ALPH_len(u_eps_alph);

                        l_eps_alph = alph_merge(l_eps_alph, AV_l_eps(dest_av));

                        if (l_eps_alph)
                            l_eps_count = ALPH_len(l_eps_alph);

			/* Does flag diacritics need this? TG */
			u_fld_alph = alph_merge(u_fld_alph, AV_u_fld(dest_av));

                        if (u_fld_alph)
                            u_fld_count = ALPH_len(u_fld_alph);

                        l_fld_alph = alph_merge(l_fld_alph, AV_l_fld(dest_av));

                        if (l_fld_alph)
                            l_fld_count = ALPH_len(l_fld_alph);
                    }
                    else
                    {
                        arc = STATE_arc_set(ARC_destination(arc));
                        label_id = ARC_label(arc);
                    }
                }
            } /* if label_id == ALTCHAIN_SYMBOL */

            /* If LABEL_ID is the ALTCHAIN_SYMBOL, the arc was pointing
               to a state that had already been vectorized. Nothing
               more to do in that case.
               */
                            
            if (label_id != ALTCHAIN_SYMBOL)
            {

                /* If the position is already filled, the network
                   is not deterministic.
                   */

                assert(!(dest_array[label_id]));

                dest_array[label_id] = ARC_destination(arc);

                label = id_to_label(label_id);
                            
                if (LABEL_arity(label) == 2)
                {
                    upper_id = TUPLE_labels(LABEL_tuple(label))[UPPER];
                    lower_id = TUPLE_labels(LABEL_tuple(label))[LOWER];
                }
                else
                {
                    upper_id = label_id;
                    lower_id = label_id;
                }
                            
                if (upper_id == EPSILON)
                {
                    if (!u_eps_alph)
                    {
                        u_eps_alph = make_alph(1, LABEL_VECTOR);
                        if (!u_eps_alph)
                            not_enough_memory("make_vector_state");
                    }
                    else
                        adjust_alph_size(u_eps_alph, u_eps_count+1);
                            
                    ALPH_item(u_eps_alph, u_eps_count++) = label_id;
                }
                else if (lower_id == EPSILON)
                {
                    if (!l_eps_alph)
                    {
                        l_eps_alph = make_alph(1, LABEL_VECTOR);
                        if (!l_eps_alph)
                            not_enough_memory("make_vector_state");
                    }
                    else
                        adjust_alph_size(l_eps_alph, l_eps_count+1);
                            
                    ALPH_item(l_eps_alph, l_eps_count++) = label_id;
                }
		/* Flag diacritics 10.10.97  TG */
		if (NET_u_flag_diacr(net) || NET_l_flag_diacr(net))
		  {
		    if (flag_diacritic_p(upper_id))
		      {
			if (!u_fld_alph)
			  {
			    u_fld_alph = make_alph(1, LABEL_VECTOR);
			    if (!u_fld_alph)
			      not_enough_memory("make_vector_state");
			  }
			else
			  adjust_alph_size(u_fld_alph, u_fld_count+1);
			
			ALPH_item(u_fld_alph, u_fld_count++) = label_id;
		      }
		    if (flag_diacritic_p(lower_id))
		      {
			if (!l_fld_alph)
			  {
			    l_fld_alph = make_alph(1, LABEL_VECTOR);
			    if (!l_fld_alph)
			      not_enough_memory("make_vector_state");
			  }
			else
			  adjust_alph_size(l_fld_alph, l_fld_count+1);
			
			ALPH_item(l_fld_alph, l_fld_count++) = label_id;
		      }
		  }
            }
        }               
        AV_u_eps(arc_vector) = u_eps_alph;
        AV_l_eps(arc_vector) = l_eps_alph;
	AV_u_fld(arc_vector) = u_fld_alph;
	AV_l_fld(arc_vector) = l_fld_alph;
                            
        for (arc = STATE_arc_set(state); arc != NULL; 
             arc = temp)
        {
            temp = next_arc(arc);
            free_arc(arc);
        }
		
        STATE_arc_vector(state) = arc_vector;
        STATE_vector_p(state) = TRUE;	
    }
}

void unmake_vector_state (STATEptr state, NETptr net, int vector_size)
{
  /*******
   *
   * Restores the original arc set of STATE and frees the cells of the array.
   * holding the arcs. 
   *
   *******/

  AVptr arc_vector;
/*  ARCptr new_arc;*/
  STATEptr *array;
  int i;
  
  if (STATE_vector_p(state) == TRUE)
    {
      arc_vector = STATE_arc_vector(state);
      array = AV_dest_array(arc_vector);
      vector_size = arc_vector_size(net);
      STATE_arc_set(state) = NULL;
		
      /* cycle through vector backwards so that arcs end up in 
	 numerical order after "push".
	 */
      for (i = vector_size - 1; i >= 0; i--)
	{
	  if (array[i])
	    push_arc(make_arc((id_type) i, array[i], net),  state);
	}
		
      free_arc_vector(arc_vector, net);
      STATE_vector_p(state) = FALSE;
		
    }  
}

static int count_arcs(STATEptr s)
{
  int count;
  ARCptr a;
	
  for (count = 0, a = STATE_arc_set(s);
       a != NULL;
       count++, a = next_arc(a));
	
  return (count);
}

void vectorize_net_OLD(NETptr net)
{
  FAT_STR vectorize_prop = FST_thin_to_temp_fat("VECTORIZE-N");
  OBJECTptr v_prop = get_value(net, vectorize_prop);
  int vectorize_n, vector_size;
  STATEptr s;
    
  if (!NET_deterministic(net))
    {
      handle_warning("Can't vectorize a non-deterministic network.\n",
		     "vectorize_net", 0);
      return;
    }
    
  if (!has_arc_vectors_p(net))
    {
      if (v_prop != NULL)
	vectorize_n = OBJ_integer(v_prop);
      else
	vectorize_n = STANDARD_VECTORIZE_N;
	    
      NET_arc_vector_len(net) = vector_size = arc_vector_size(net);
	    
      /* NOTE:  If you ever change the  first arg (cell size),
	 you must also alter the function arc_vector_size(),
	 which depends upon the cell_size declared here.
	 */
      NET_arc_vector_heap(net) = init_heap(vector_size * sizeof(STATEptr), 
					   NET_num_states(net)/8+1,
					   "Net arc vectors");
      for (s = NET_states(net); s != NULL; s = next_state(s))
	{
	  if (!STATE_vector_p(s) && count_arcs(s) >= vectorize_n)
	    make_vector_state(s, net, vector_size);
	}
    }
}

void make_vector_state_from_reduced (STATEptr state, 
				     NETptr net,
				     int vector_size, 
				     ALPH_VECTORptr reduced_to_unreduced_map)
     /* 
	For Reduced Labelset nets only. 
	This is similar to make_vector_state() but it works on networks with
	reduced labelsets: if an arc stands for several other arcs then all
	these arcs will be taken into consideration when computing the
	destination array of the state. VECTOR_SIZE must be big enough to be
	indexable by the ORIGINAL (not reduced) id-s.
	*/
{
  AVptr arc_vector;
  ARCptr arc, temp;
  STATEptr *dest_array;
  LABELptr label;
  id_type label_id, upper_id, lower_id ;
  int /*len,*/ i, l_eps_count = 0, u_eps_count = 0,
    l_fld_count = 0, u_fld_count = 0;
  ALPHABETptr arc_unreduced_arcs_alph, l_eps_alph = NULL, u_eps_alph = NULL,
    u_fld_alph = NULL, l_fld_alph = NULL;
  
  assert(reduced_to_unreduced_map);

  if (STATE_vector_p(state) == FALSE)
    {
        arc_vector = (AVptr) cell_alloc(ARC_VECTOR_HEAP);
        dest_array = (STATEptr *) cell_alloc(NET_arc_vector_heap(net));

        if (!(arc_vector && dest_array))
            not_enough_memory("make_vector_state_from_reduced");

        AV_dest_array(arc_vector) = dest_array;

        for (arc = STATE_arc_set(state) ; arc != NULL ; arc = next_arc(arc))
	{
            /* If the arc label is the ALTCHAIN_SYMBOL, we ignore the
               arc and go to its destination state. If the destination
               has already been vectorized, we copy the destinations from
               its destination vector to the current state. If the
               destination has not yet been vectorized, we process its
               arcs as if they belonged to the current state.
               */

	  /** Reduced labelset fst-s: an arc may substitute several. We iterate on them. */

            label_id = ARC_label(arc);
	    arc_unreduced_arcs_alph = av_alph(reduced_to_unreduced_map, label_id);
	    assert (arc_unreduced_arcs_alph && ALPH_len(arc_unreduced_arcs_alph) > 0 );

	    for (i=0; i < ALPH_len(arc_unreduced_arcs_alph) ; i++)
	      {
		label_id = ALPH_item(arc_unreduced_arcs_alph, i);

		if (label_id == ALTCHAIN_SYMBOL)
		  {
		    if (next_arc(arc) || ( i < (ALPH_len(arc_unreduced_arcs_alph)-1))  )
		      fsm_fprintf(stdout, "***make_vector_state_from_reduced(): Nonfinal ALTCHAIN arc\n");
		    else
		      {
			STATEptr dest = ARC_destination(arc);
                    
			/* No looping ALTCHAIN_SYMBOLS! */

			assert(state != dest);

			if (STATE_vector_p(dest))
			  {
			    /* Destination state has already been vectorized.
			       Copy the destination pointers and epsilon
			       alphabets.
			       */
                            
			    int i;
			    AVptr dest_av = STATE_arc_vector(dest);
			    STATEptr *dest_dest_array = AV_dest_array(dest_av);

			    for (i = 0; i < vector_size; i++)
			      if (dest_dest_array[i])
				{
				  assert(!(dest_array[i]));
				  dest_array[i] = dest_dest_array[i];
				}

			    /* Note: one or both of the arguments to
			       alph_merge() might be NULL. */

			    u_eps_alph = alph_merge(u_eps_alph, AV_u_eps(dest_av));

			    if (u_eps_alph)
			      u_eps_count = ALPH_len(u_eps_alph);

			    l_eps_alph = alph_merge(l_eps_alph, AV_l_eps(dest_av));

			    if (l_eps_alph)
			      l_eps_count = ALPH_len(l_eps_alph);

			    /* Does flag diacritics need this? TG */
			    u_fld_alph = alph_merge(u_fld_alph, AV_u_fld(dest_av));

			    if (u_fld_alph)
			      u_fld_count = ALPH_len(u_fld_alph);

			    l_fld_alph = alph_merge(l_fld_alph, AV_l_fld(dest_av));

			    if (l_fld_alph)
			      l_fld_count = ALPH_len(l_fld_alph);
			  }
			else
			  {
			    arc = STATE_arc_set(ARC_destination(arc));
			    label_id = ARC_label(arc);
			  }
		      }
		  }
	      }

		/* If LABEL_ID is the ALTCHAIN_SYMBOL, the arc was pointing
		   to a state that had already been vectorized. Nothing
		   more to do in that case.
		   */
                            
	    if (label_id != ALTCHAIN_SYMBOL)
	      {
		/** Reduced labelset fst-s: an arc may substitute several. We iterate on them. */

		label_id = ARC_label(arc);
		arc_unreduced_arcs_alph = av_alph(reduced_to_unreduced_map, label_id);
		assert (arc_unreduced_arcs_alph && ALPH_len(arc_unreduced_arcs_alph) > 0 );

		for (i=0; i < ALPH_len(arc_unreduced_arcs_alph) ; i++)
		  {
		    label_id = ALPH_item(arc_unreduced_arcs_alph, i);

		    /* If the position is already filled, the network
		       is not deterministic.
		       */

		    assert(!(dest_array[label_id]));

		    dest_array[label_id] = ARC_destination(arc);

		    label = id_to_label(label_id);
                            
		    if (LABEL_arity(label) == 2)
		      {
			upper_id = TUPLE_labels(LABEL_tuple(label))[UPPER];
			lower_id = TUPLE_labels(LABEL_tuple(label))[LOWER];
		      }
		    else
		      {
			upper_id = label_id;
			lower_id = label_id;
		      }
                            
		    if (upper_id == EPSILON)
		      {
			if (!u_eps_alph)
			  {
			    u_eps_alph = make_alph(1, LABEL_VECTOR);
			    if (!u_eps_alph)
			      not_enough_memory("make_vector_state_from_reduced");
			  }
			else
			  adjust_alph_size(u_eps_alph, u_eps_count+1);
                            
			ALPH_item(u_eps_alph, u_eps_count++) = label_id;
		      }
		    else if (lower_id == EPSILON)
		      {
			if (!l_eps_alph)
			  {
			    l_eps_alph = make_alph(1, LABEL_VECTOR);
			    if (!l_eps_alph)
			      not_enough_memory("make_vector_state_from_reduced");
			  }
			else
			  adjust_alph_size(l_eps_alph, l_eps_count+1);
                            
			ALPH_item(l_eps_alph, l_eps_count++) = label_id;
		      }
		    /* Flag diacritics 10.10.97  TG */
		    if (NET_u_flag_diacr(net) || NET_l_flag_diacr(net))
		      {
			if (flag_diacritic_p(upper_id))
			  {
			    if (!u_fld_alph)
			      {
				u_fld_alph = make_alph(1, LABEL_VECTOR);
				if (!u_fld_alph)
				  not_enough_memory("make_vector_state_from_reduced");
			      }
			    else
			      adjust_alph_size(u_fld_alph, u_fld_count+1);
			
			    ALPH_item(u_fld_alph, u_fld_count++) = label_id;
			  }
			if (flag_diacritic_p(lower_id))
			  {
			    if (!l_fld_alph)
			      {
				l_fld_alph = make_alph(1, LABEL_VECTOR);
				if (!l_fld_alph)
				  not_enough_memory("make_vector_state_from_reduced");
			      }
			    else
			      adjust_alph_size(l_fld_alph, l_fld_count+1);
			
			    ALPH_item(l_fld_alph, l_fld_count++) = label_id;
			  }
		      }
		  }
	      }
        }    
        AV_u_eps(arc_vector) = u_eps_alph;
        AV_l_eps(arc_vector) = l_eps_alph;
	AV_u_fld(arc_vector) = u_fld_alph;
	AV_l_fld(arc_vector) = l_fld_alph;
                            
        for (arc = STATE_arc_set(state); arc != NULL; 
             arc = temp)
        {
            temp = next_arc(arc);
            free_arc(arc);
        }
		
        STATE_arc_vector(state) = arc_vector;
        STATE_vector_p(state) = TRUE;	
    }
}

void vectorize_net(NETptr net)
/* NEW 99-4-22 TG
If the net has a Reduced Labelset, it is extended to the original
size and the net is vectorized as if it were unreduced.
To do that, two changes were needed: the original labelset is
restored temporarily to find the unreduced size (of the arc vector)
- the new arc_vector_size() knows that; and make_vector_state() has
a new variant, make_vector_state_from_reduced()  that iterates on
all the arcs that the actual arcs substitutes.
Otherwise it works as before.
*/
{
	ALPH_VECTORptr reduced_to_unreduced_arcs_map = NULL;
	FAT_STR vectorize_prop = FST_thin_to_temp_fat("VECTORIZE-N");
	OBJECTptr v_prop = get_value(net, vectorize_prop);
	int vectorize_n, vector_size;
	int number_vectorized_states = 0;
	STATEptr s;

	if (!NET_deterministic(net))
	{
		handle_warning("Can't vectorize a non-deterministic network.\n",
			"vectorize_net", 0);
		return;
	}


	if (!has_arc_vectors_p(net))
	{
		/* If  ARC_VECTOR_HEAP has not yet been built, we have to allocate it: */
		if (ARC_VECTOR_HEAP == NULL)
			ARC_VECTOR_HEAP = init_heap(sizeof(AVtype), 32768 / sizeof(AVtype),
			"Arc vectors");
		if (v_prop != NULL)
			vectorize_n = OBJ_integer(v_prop);
		else
			vectorize_n = STANDARD_VECTORIZE_N;

		reduced_to_unreduced_arcs_map = get_reduced_to_unreduced_map(net);
		NET_arc_vector_len(net) = vector_size = arc_vector_size(net);

		/* NOTE:  If you ever change the  first arg (cell size),
	   you must also alter the function arc_vector_size(),
	   which depends upon the cell_size declared here.
	   */
		NET_arc_vector_heap(net) = init_heap(vector_size * sizeof(STATEptr),
			NET_num_states(net) / 8 + 1,
			"Net arc vectors");
		for (s = NET_states(net); s != NULL; s = next_state(s))
		{
			if (!STATE_vector_p(s) && count_arcs(s) >= vectorize_n)
			{
				number_vectorized_states++;

				if (NET_reduced_labelset(net) && reduced_to_unreduced_arcs_map)
					make_vector_state_from_reduced(s, net, vector_size,
					reduced_to_unreduced_arcs_map);
				else
					make_vector_state(s, net, vector_size);
			}
		}

		if (number_vectorized_states == 0)
		{
			/* No state has been vertorized:
			   release the "Net arc vectors" heap otherwise the net
			   may later be wrongly considered as vectorized because
			   of the macro has_arc_vectors_p(). */
			free_heap(NET_arc_vector_heap(net));
			NET_arc_vector_heap(net) = NULL;
			NET_arc_vector_len(net) = 0;
		}
		else
			/* If the net's arcs were previously sorted,
			   this arc ordering is lost. */
			   NET_sorted_arcs(net) = 0;
	} /* if (!has_arc_vectors_p(net)) */

	free_reduced_to_unreduced_map(&reduced_to_unreduced_arcs_map);
}

int vectorize_dense_states_of_net_OLD (NETptr net, int min_vectorize)
{
  STATEptr s;  int vector_size, num_vec_states, n;
    
  if (!NET_deterministic(net))
    {
      handle_warning("Can't vectorize a non-deterministic network.\n",
		     "vectorize_net", 0);
      return(0);
    }
    
  if (has_arc_vectors_p(net))
    return(0);
 
  NET_arc_vector_len (net) = vector_size = arc_vector_size (net);

  /* we calculate the number of states to be vectorized, in order to
     allocate not too much memory for the arc vectors. */

  for (num_vec_states = 0, s = NET_states(net); s; s = next_state(s))
    if ((! STATE_vector_p(s)) && (count_arcs(s) >= min_vectorize))
      num_vec_states++;

  /* NOTE:  If you ever change the  first arg (cell size),
     you must also alter the function arc_vector_size(),
     which depends upon the cell_size declared here.
     */
/*  NET_arc_vector_heap(net) =
    init_heap (vector_size * sizeof(STATEptr), 
	       NET_num_states(net)/8+1,
	       "Net arc vectors"); */

  NET_arc_vector_heap(net) =
    init_heap (vector_size * sizeof(STATEptr), num_vec_states,
	       "Net arc vectors");

  for (n = 0, s = NET_states(net); s; s = next_state(s))
    if ((! STATE_vector_p(s)) && (count_arcs(s) >= min_vectorize))
      {
	make_vector_state (s, net, vector_size);
	n++;
      }

  if (num_vec_states != n)  /* this never should happen */
    {
      fsm_fprintf (stderr, " *** ERROR ***\ncounting error in vectorization of dense states\n");
      exit (1);
    }

  return (num_vec_states);
}

int vectorize_dense_states_of_net (NETptr net, int min_vectorize)
     /* 99-4-26: handles nets with Reduced Labelsets, too */
{
  ALPH_VECTORptr reduced_to_unreduced_arcs_map=NULL;
  STATEptr s;  int vector_size, num_vec_states, n;
    
  if (!NET_deterministic(net))
    {
      handle_warning("Can't vectorize a non-deterministic network.\n",
		     "vectorize_net", 0);
      return(0);
    }
    
  if (has_arc_vectors_p(net) || NET_compacted(net))
    return(0);

  /* If  ARC_VECTOR_HEAP has not yet been built, we have to allocate it: */
  if (ARC_VECTOR_HEAP == NULL)
      ARC_VECTOR_HEAP= init_heap(sizeof(AVtype), 32768/sizeof(AVtype),
                                 "Arc vectors");
  
  /* Calculate the number of states to be vectorized, in order to
     allocate not too much memory for the arc vectors. */
  for (num_vec_states = 0, s = NET_states(net); s; s = next_state(s))
    if ((! STATE_vector_p(s)) && (count_arcs(s) >= min_vectorize))
      num_vec_states++;

  /* NOTE:  If you ever change the  first arg (cell size),
     you must also alter the function arc_vector_size(),
     which depends upon the cell_size declared here.
     */
/*  NET_arc_vector_heap(net) =
    init_heap (vector_size * sizeof(STATEptr), 
	       NET_num_states(net)/8+1,
	       "Net arc vectors"); */

  /* If num_vec_states is non NULL, at least one state needs to be
     vectorized and hence a dest_array heap can be allocated for
     that network. */
  if (num_vec_states > 0)
  {
      NET_arc_vector_len (net) = vector_size = arc_vector_size (net);

      NET_arc_vector_heap(net) =
          init_heap (vector_size * sizeof(STATEptr), num_vec_states,
                     "Net arc vectors");
      
      /* In case the net would be reduced:
         make a vector of ALPHABETS indexable by labels of the reduced
         alphabet. */
      reduced_to_unreduced_arcs_map = get_reduced_to_unreduced_map(net);

      /* Do the work now: replace the state arc-lists by arrays: */
      for (n = 0, s = NET_states(net); s; s = next_state(s))
          if ((! STATE_vector_p(s)) && (count_arcs(s) >= min_vectorize))
          {
              if (NET_reduced_labelset(net) && reduced_to_unreduced_arcs_map)
                  make_vector_state_from_reduced(s, net, vector_size,
                                                 reduced_to_unreduced_arcs_map);
              else
                  make_vector_state (s, net, vector_size);
              n++;
          }

      if (num_vec_states != n)  /* this never should happen */
      {
          fsm_fprintf (stderr, " *** ERROR ***\ncounting error in vectorization of dense states\n");
          exit (1);
      }

      free_reduced_to_unreduced_map(&reduced_to_unreduced_arcs_map);

  } /* num_vec_states > 0 */

  return (num_vec_states);
}

void unvectorize_net(NETptr net)
{	
  STATEptr s;
  int vector_size = arc_vector_size(net);
	
  if (has_arc_vectors_p(net))
    {
      /* Reset the arc counter to 0 because it will be
         re-incremented by the push_arc() function: */
      NET_num_arcs(net)= 0;
        
      for (s = NET_states(net); s != NULL; s = next_state(s))
	{
	  unmake_vector_state(s, net, vector_size);
	}
      free_heap(NET_arc_vector_heap(net));
      NET_arc_vector_heap(net) = NULL;
      NET_arc_vector_len(net) = 0;
    }
}

void vectorize_nets(NVptr nets)
{
  int i;
	
  for (i = 0; i < NV_len(nets); i++)
    vectorize_net(NV_net(nets, i));
}

void unvectorize_nets(NVptr nets)
{
  int i;
	
  for (i = 0; i < NV_len(nets); i++)
    unvectorize_net(NV_net(nets, i));
}

#endif /* SLIM_FST */
