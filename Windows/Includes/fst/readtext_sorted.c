/* $Id: readtext_sorted.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1991-94 Xerox Corporation. All rights reserved. */

/********************************************************************************
 **
 **  readtext_sorted.c
 **
 **	 Florent NICART, Franck GUINGNE, Xerox XRCE Grenoble, April 2001
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
#include "fst/assert.h"
#include "fst/types.h"
#include "fst/readtext_sorted.h"

/* #define assert(x) */

int nb_repreg=0;
int nb_merge=0;
int nb_addsuf=0;
int nb_eqst=0;
int nb_eqst2=0;
int nb_eqst3=0;
int nb_addreg=0;



int nb_prev(STATEptr st) {
	ARCptr arc;
	int nb=0;
	
	assert(st);
	arc=STATE_client_cell(st);

	while (arc) {
		nb++;
		arc=next_arc(arc);
	}

	return nb;
}





/* ***************************************************************************************************
 * last_child : return the last (lexicographically) outgoing arc of the state.
 * WARNING : it rely on the fact that the arcs list of st is sorted in the
 * inverted lexigraphic order
 * ***************************************************************************************************
 */
ARCptr last_child(STATEptr st) {
	assert(st);
	return (STATE_arc_set(st));
}



/* ***************************************************************************************************
 * nb_child : return the number of  outgoing arcs of the state.
 * ***************************************************************************************************
 */
int nb_child(STATEptr st) {
	ARCptr arc;
	int nb=0;
	
	assert(st);
	arc=STATE_arc_set(st);

	while (arc) {
		nb++;
		arc=next_arc(arc);
	}

	return (nb);
}



/* ***************************************************************************************************
 * equal_states : compare the right languages of two states..
 * ***************************************************************************************************
 */
int equal_states(STATEptr stA, STATEptr stB) {
	ARCptr aA, aB;

nb_eqst++;
	
	assert(stA);
	assert(stB);
	
	/* If the number of childs are different, the right languages are too */
	if (nb_child(stA)!=nb_child(stB)) return FALSE;
nb_eqst2++;
	
	/* If the state types are differents, the right languages are too */
	if (STATE_final(stA)!=STATE_final(stB)) return FALSE;
nb_eqst3++;
	
	aA=STATE_arc_set(stA);
	while (aA) {
		/* start with the first arc of the second state */
		aB=STATE_arc_set(stB);
		
		/* Look for an arc with the same label */
		while ((aB) && (ARC_label(aA) != ARC_label(aB))) {
			aB=next_arc(aB);
		}
		
		/* if not found, not same right languages */
		if (!aB) return FALSE;
		
		/* if not same destination, not same right languages */
		if (ARC_destination(aA)!=ARC_destination(aB)) return FALSE;
		
		/* proceed with the next arc from the first state */
		aA=next_arc(aA);
	} 

	return (TRUE);
}




/* ***************************************************************************************************
 * delete from_client_cell : remove an arc from the client cell.
 * ***************************************************************************************************
 */
/*
 * void delete_from_client_cell(STATEptr stTarget, STATEptr stSource) {
 * 	ARCptr* cc;
 * 	ARCptr to_delete;
 * 	
 * 	assert(stTarget);
 * 	assert(stSource);
 * 	
 * 	cc=&STATE_client_cell(stTarget);
 * 	
 * 	while (*cc) {
 * 		if (arc_destination(*cc)==stSource) {
 * 			to_delete=*cc;
 * 			*cc=next_arc(to_delete);
 * 			
 * 			free(to_delete);	
 * 		}
 * 		cc = &next_arc(*cc);
 * 	}
 * }
 */



/* ***************************************************************************************************
 * clean_client_cell : remove arcs from the client cell that point to the state to remove.
 * ***************************************************************************************************
 */
/*
 * void clean_client_cell(STATEptr stClean) {
 * 	ARCptr source;
 * 	STATEptr target;
 * 	assert(stClean);
 * 
 * 	source=STATE_arc_set(stClean);
 * 	
 * 	while (source) {
 * 		
 * 		target=arc_destination(source);
 * 		
 * 		
 * 		delete_from_client_cell(target, stClean);
 * 		
 * 		source=next_arc(source);
 * 	}
 * }
 * 
 */



/* ***************************************************************************************************
 * add_to_registry : add a state in the registry.
 * Input :
 * 	stAdd : the state to add.
 * Output :
 * 	none.
 * ***************************************************************************************************
 */
void add_to_registry(STATEptr stAdd) {
	ARCptr newArc, arc;
	STATEptr target;

nb_addreg++;
	
	assert(stAdd);

	/* the target list */
	arc = STATE_arc_set(stAdd);
	assert(arc);		/* at least one */
		
	while (arc) {
		/* Retrieve the target state */
		target=ARC_destination(arc);
		assert(target);
		
		/* Create a new arc for this state */
		newArc = arc_alloc(ARC_label(arc), stAdd);
		assert(newArc);
	
		/* Add the new arc to the target arc list on client_cell */
		next_arc(newArc) = STATE_client_cell(target);
		STATE_client_cell(target) = newArc;
		
		/* the next one */
		arc = next_arc(arc);
	}
}



/* ***************************************************************************************************
 * merge_states : merge two states..
 * Input :
 * 	stMerge : the state to merge with (registered).
 *	stRemove : the state to be merged.
 *	rebranch : the arc to be rebranched.
 * Output :
 * 	none.
 * ***************************************************************************************************
 */
void merge_states(NETptr net, STATEptr stMerge, STATEptr stRemove, ARCptr rebranch) {
	assert(stMerge);
	assert(stRemove);
	assert(stMerge!=stRemove);
	assert(rebranch);
	
nb_merge++;

	/* rebranch the incoming arc : */
	ARC_destination(rebranch)=stMerge;
	
	/* delete the merged state and its outgoing arcs */
	free_arc_set(stRemove, net);
	NET_states(net)=delete_state_from_list(stRemove, NET_states(net));
	NET_num_states(net)--;
/*	printf("%d \n", NET_num_states(net));*/
	
/*	free_state_and_arcs(stRemove, net);*/
}




/* ***************************************************************************************************
 * longest_prefix : return the length of the longest suffix of seq found of in the network net.
 * At return, *lastState contain the address of the last state of the prefix.
 * Input :
 * 	net : the network in which we are looking for the prefix.
 * 	seq : the sequence of type_id.
 * Output :
 * 	lastPrefix : the address of pointer that will receive the address of the last state.
 * 	return the lenght of the prefix.
 * ***************************************************************************************************
 */
int longest_prefix(NETptr net, id_type* seq, STATEptr* lastState) {
	STATEptr CurrentState;  /* The current state in the path */
	ARCptr lastArcChild;	/* The last arc child of the current state */
	int len=0;		/* the length of the current prefix found */
	
	assert(net);
	assert(seq);
	assert(lastState);

	CurrentState = NET_start_state(net);
	assert(CurrentState); /* The network must have a startstat ! */
	
	
	while ((*seq != ID_EOS) && (lastArcChild=last_child(CurrentState)) && (ARC_label(lastArcChild)==*seq)) {
		seq++;		/* next symbol */
		len++;		/* Increment the length of the prefix found at this state */
		CurrentState = ARC_destination(lastArcChild); /* the next state in the prefix */
	}
	
	*lastState = CurrentState;
	return (len);
}




/* ***************************************************************************************************
 * add_suffix : add the specified suffix to a given state in a given network.
 * Input :
 * 	net : the network in which we are adding the suffix.
 *	lastState : the state on which we are adding the suffix.
 * 	seq : suffix to add.
 * Output :
 * 	none.
 * ***************************************************************************************************
 */
void add_suffix(NETptr net, STATEptr lastState, id_type *seq) {
	STATEptr CurrentState, newState;
	ARCptr newArc;
	assert(net);
	assert(lastState);
	assert(seq);

nb_addsuf++;
	
	CurrentState=lastState;
	while (*seq != ID_EOS) {
binary_labels_add_to(NET_labels(net), *seq);
		newState=make_state(NULL, net);
		newArc=make_arc(*seq, newState, net);

		/* Add the new arc at the beginning of the arcs list : */
		next_arc(newArc) = STATE_arc_set(CurrentState);
		STATE_arc_set(CurrentState) = newArc;
		
		seq++;
		CurrentState = newState;
	}
	
	STATE_final(CurrentState) = TRUE;
}




STATEptr replace_or_register(NETptr net, STATEptr currentInPath, STATEptr initialState, STATEptr finalState, ARCptr incoming) {
	ARCptr lastc, aprev;
	STATEptr sprev;
	int EqualNotFound=TRUE;
	STATEptr currentInNet;
	
	assert(currentInPath);
	assert(finalState);
	assert(finalState!=initialState);
	assert(incoming);

nb_repreg++;

	/* If we are on the last state of the path, simply merge with the finalstate */
	if (!(lastc=last_child(currentInPath))) {
		/* Merge the two "final" states */
		currentInNet=finalState;
		merge_states(net, currentInNet, currentInPath, incoming);
		
		return (currentInNet);
	}
	
	/* if currentState has a child, recursively call the function */
	currentInNet = replace_or_register(net, ARC_destination(lastc), initialState, finalState, lastc);

	if (!currentInNet) {
		currentInNet=ARC_destination(lastc);
		
		/* Add the current state in the registry */	
		add_to_registry(currentInPath);
	
		return(NULL); /* currentInPath */
	}

	/* Check for previous equivalent state */
	aprev = STATE_client_cell(currentInNet);
	
	while (EqualNotFound && (aprev)) {
		sprev = ARC_destination(aprev);
		assert(sprev);
		
		if (ARC_label(lastc) == ARC_label(aprev)) {
			EqualNotFound = !equal_states(currentInPath, sprev);
		}
		
		aprev = next_arc(aprev);
	}
	
	if (EqualNotFound) {
		add_to_registry(currentInPath);
		
		return NULL;
	}
	

	/* Merge the two states */
	merge_states(net, sprev, currentInPath, incoming);

	return sprev;
}



void rec_count(STATEptr st, int* stats, int* sums, int level) {
	int nb;
	ARCptr child;

	nb = nb_prev(st);
	
	if (nb > stats[level]) stats[level] = nb;
	sums[level] += nb;
	
	child = STATE_client_cell(st);
	
	while (child) {
		rec_count(ARC_destination(child), stats, sums, level+1);
	
		child = next_arc(child);
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
	int stats[200];
	int sums[200];


	NETptr net = null_fsm();	/* The create a NEW network */
	int len;			/* The length of the prefix */
	STATEptr lastState;		/* Pointer to the lastState of the prefix */
	id_type* seq;			/* Pointer to a symbol id in the sequence */
  	STATEptr* registry;		/* The vector of already registered states */
	STATEptr finalState;		/* the final state of the minimized part */
	int regSize;			/* The effective size of the register */
	id_type minId;			/* the minimum state id for this network */
	ARCptr lastc;
  
	NET_minimized(net) = FALSE;
	label_to_binary(NET_labels(net));

	printf("start\n");
	memset(stats, 0, sizeof(stats));
	memset(sums, 0, sizeof(sums));
	/* Initialize the register with 100 entry : */
/*
 * 	regSize = 100;
 * 	start_destinations = (STATEptr *) calloc((size_t) regSize, (size_t) sizeof(STATEptr));
 */

	finalState = make_state(NULL, net);
	STATE_final(finalState)=TRUE;

	while ((seq = seq_form()) && (*seq != ID_EOS)) {
  		len = longest_prefix(net, seq, &lastState);
		seq+=len;

		/* replace or register the previous suffix : */
		lastc=last_child(lastState);
		if (lastc) {
			assert(ARC_destination(lastc));
			replace_or_register(net, ARC_destination(lastc), NET_start_state(net), finalState, lastc);
		}

		/* Add the current suffix */
		add_suffix(net, lastState, seq);
	}
	replace_or_register(net, ARC_destination(last_child(NET_start_state(net))), NET_start_state(net),
finalState, last_child(NET_start_state(net)));

	printf("stop\n");

	printf("nb_repreg : %d\n nb_merge : %d\n nb_addsuf : %d\n nb_eqst : %d\n nb_eqst2 : %d\n nb_eqst3 : %d\n nb_addreg : %d\n",
	nb_repreg,nb_merge,nb_addsuf,nb_eqst,nb_eqst2,nb_eqst3,nb_addreg);



/*	nb_prev(finalState);
	printf("nb pred : %d\n", nb);
	lastc=STATE_client_cell(finalState);
	len=0;
	while (lastc) {
		len+=nb_prev(ARC_destination(lastc));
		
		lastc = next_arc(lastc);
	}	
	printf("total : %d", len);
*/	

	rec_count(finalState, stats, sums, 0);
	for (len=0; len<200; len++) printf("%d : %d/%d\n", len, stats[len], sums[len]);
	
	binary_to_label(NET_labels(net));
	NET_sigma(net) = label_sigma(net);
	return (net);
}




