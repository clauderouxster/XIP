/* $Id: readtext_unsorted.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1991-94 Xerox Corporation. All rights reserved. */

/********************************************************************************
 **
 **  readtext_unsorted.c
 **
 **	 Florent NICART, Franck GUINGNE, Xerox XRCE Grenoble, June 2001
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
#include "fst/types.h"
#include "fst/readtext_unsorted.h"

/*#include "assert.h"*/
 #define assert(x) 




/* ***************************************************************************************************
 * nb_child : return the number of  outgoing arcs of the state.
 * Input :
 * 	st : the state we count the children.
 * Output :
 * 	none.
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

	
	assert(stA);
	assert(stB);
	
	/* If the number of childs are different, the right languages are too */
	if (nb_child(stA)!=nb_child(stB)) return FALSE;
	
	/* If the state types are differents, the right languages are too */
	if (STATE_final(stA)!=STATE_final(stB)) return FALSE;
	
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
	

	/* rebranch the incoming arc : */
	ARC_destination(rebranch)=stMerge;
	
	/* delete the merged state and its outgoing arcs */
	free_arc_set(stRemove, net);
	NET_states(net)=delete_state_from_list(stRemove, NET_states(net));
	NET_num_states(net)--;
}




/* ***************************************************************************************************
 * state_by : return the state accessible from another given a label.
 * Input :
 * 	from : the state we come from.
 *	id_label : the label of the arc that go out..
 * Output :
 * 	return the state found, NULL if not..
 * ***************************************************************************************************
 */
STATEptr state_by(STATEptr from, id_type label) {
	ARCptr ascan;
	
	for (ascan=STATE_arc_set(from); (ascan && (ARC_label(ascan) != label)); ascan=next_arc(ascan));
	
	if (ascan) return ARC_destination(ascan);
	else return NULL;
}




/* ***************************************************************************************************
 * longest_prefix : return the length of the longest suffix of seq found of in the network net.
 * At return, *lastState contain the address of the last state of the prefix.
 * Input :
 * 	net : the network in which we are looking for the prefix.
 * 	seq : the sequence of type_id.
 * Output :
 *	commonPrefix : pointer to an array to feed with the states of the prefix.
 * 	return the lenght of the prefix.
 * ***************************************************************************************************
 */
int longest_prefix(NETptr net, id_type* seq, STATEptr* commonPrefix) {
	STATEptr currentState;	/* The current state in the path */
	int len=0;			/* the length of the current prefix found */
	
	assert(net);
	assert(seq);

	currentState = NET_start_state(net);
	assert(currentState); /* The network must have a startstat ! */
	
	/* register the initial state */
	commonPrefix[0] = currentState;
	
	/* go through and register the common prefix */
	while ((*seq != ID_EOS) && (currentState = state_by(currentState, *seq))) {
		seq++;		/* next symbol */
		len++;		/* Increment the length of the prefix found at this state */
		
		commonPrefix[len] = currentState;
	}
	
	return len;
}





/* ***************************************************************************************************
 * build_arcs : create an arc an its reverse arc between two states given a label.
 * Input :
 * 	net : the net we are working on.
 *	source : the origin state of the arc (target of the reverse arc).
 *	target : the target state of the arc (origin of the reverse arc).
 *	label : the label of the two arcs.
 * Output :
 * 	none.
 * ***************************************************************************************************
 */
void build_arcs(NETptr net, STATEptr source, STATEptr target, id_type label) {
	ARCptr newArc;
	
	/* Branch the target on the source : */
	newArc=make_arc(label, target, net);
	next_arc(newArc) = STATE_arc_set(source);
	STATE_arc_set(source) = newArc;
	
	/* Create the corresponding inverted arc) : */
	newArc = arc_alloc(label, source);
	assert(newArc);
	next_arc(newArc) = STATE_client_cell(target);
	STATE_client_cell(target) = newArc;
}




/* ***************************************************************************************************
 * OnlyOneArc : Indicate if a state has only one arc.
 * Input :
 *	state : the state we check for divergence.
 * Output :
 * 	return TRUE if the state have one or less arc, else FALSE.
 * ***************************************************************************************************
 */
BOOL OnlyOneArc(STATEptr state) {
	if (STATE_arc_set(state)) {
		return (next_arc(STATE_arc_set(state)) == NULL);
	}
	
	return FALSE;
}




/* ***************************************************************************************************
 * SearchEqSfx : search an equivalent state while building the suffix. This function use the property 
 * that states in the suffix have at most one arc, so the equivalent ones have two.
 * Input :
 * 	CurrentInNet : the state used by the previous minimisation.
 *	id_label : the arc label leading to the state we searching an equivalent for (in reverse order).
 * Output :
 * 	Return the equivalent state.
 * ***************************************************************************************************
 */
STATEptr SearchEqSfx(STATEptr CurrentInNet, id_type label) {
	ARCptr ascan;
	STATEptr sscan;
	
	ascan = STATE_client_cell(CurrentInNet);
	
	while (ascan) {
		if (ARC_label(ascan) == label) {
			sscan=ARC_destination(ascan);
			
			if ((OnlyOneArc(sscan))  &&  (STATE_final(sscan) == FALSE)) return sscan;
		}
		
		ascan = next_arc(ascan);
	}

	return NULL;
}




/* ***************************************************************************************************
 * add_suffix : add the specified suffix to a given state in a given network.
 * Input :
 * 	net : the network in which we are adding the suffix.
 *	lastState : the state on which we are adding the suffix.
 * 	seq : suffix to add.
 *	finalState : address of a pointer to "the final state" of the net. That is, the only one state
 *	with no outgoing arc.
 *	cloned : indicate whether the anchor state is a clone or not.
 * Output :
 * 	return the state which has been minimized with the first state of the path, NULL else.
 * ***************************************************************************************************
 */
STATEptr add_suffix(NETptr net, STATEptr lastState, id_type *seq, STATEptr *finalState, BOOL cloned) {
	int seqLen;		/* The length of the sequence Minus the ID_EOS. */
	int i;			/* Index in the sequence. */	
	STATEptr CurrentState;
	STATEptr CurrentStateInNet;
	STATEptr newState;
	ARCptr newArc;
	assert(net);
	assert(lastState);
	assert(seq);
	
	
	/* Determine the length of the Suffix : */
	for (seqLen=0; seq[seqLen] != ID_EOS; seqLen++) {
		binary_labels_add_to(NET_labels(net), seq[seqLen]);
	}

	/* If empty suffix, finalize the given lastState : */
	if (!seqLen) {
		STATE_final(lastState) = TRUE;
		
		assert(STATE_arc_set(lastState));
		return ARC_destination(STATE_arc_set(lastState));
	}
	
	/* If the anchor state is THE final state, then create a new one, */
	/* no cloning is necessary since there is no outgoing arcs */
	if (lastState == *finalState) {
		CurrentState=make_state(NULL, net);
		STATE_client_cell(CurrentState) = NULL;
		
		STATE_final(CurrentState) = TRUE;
		CurrentStateInNet = NULL;
		
		/* the final state of the network is the new state : */
		*finalState = CurrentState;
	}
	else {
		CurrentState = *finalState;
		CurrentStateInNet = *finalState;
	}

	
	/* create the suffix : */
	for(i=seqLen-1; i>=1; i--) {
		/* look for an equivalent State : */
		if (CurrentStateInNet) {
			CurrentStateInNet=SearchEqSfx(CurrentStateInNet, seq[i]);

			/* must not go through the lastState or the Initial state : */
			if ((CurrentStateInNet == NET_start_state(net)) || (CurrentStateInNet == lastState)) CurrentStateInNet=NULL;
		}
		
		if (!CurrentStateInNet) {
			/* Create a new state */
			newState=make_state(NULL, net);
			STATE_client_cell(newState) = NULL;
			
			/* Branch the new state on the currentState : */
			build_arcs(net, newState, CurrentState, seq[i]);
			
			CurrentState=newState;
		}
		else CurrentState=CurrentStateInNet;
	}
	
	/* Branch the suffix on the lastState : (reverse arc will be constructed by a call to replace or register) */
	if (cloned) {
		newArc=make_arc(seq[0], CurrentState, net);
		next_arc(newArc) = STATE_arc_set(lastState);
		STATE_arc_set(lastState) = newArc;
	}
	else build_arcs(net, lastState, CurrentState, seq[0]);

	return CurrentStateInNet;
}




/* ***************************************************************************************************
 * replace_or_register : delete the current state or create the reverse arcs form its target (in order
 * to add it in the register) whether it has an equivalent state in the register or not.
 * Input :
 * 	net : the network we are building.
 *	currentInPath : the state to minimize.
 * 	currentInNet : the last state in the register that has been minimized. NULL = no minimization !
 * Output :
 * 	return the the resulting state.
 * RQUE : the state must not be in the register ie it must not have reverse arcs !
 * ***************************************************************************************************
 */
STATEptr replace_or_register(NETptr net, STATEptr currentInPath, STATEptr *currentInNet) {
	ARCptr aprev, a_clean, newArc;
	STATEptr sprev;
	int Found=FALSE;
	
	
	if (*currentInNet) {
		/* look for a equivalent state in the currentInNet previous state list : */
		for (aprev=STATE_client_cell(*currentInNet); aprev && !Found; aprev=next_arc(aprev)) {
			sprev=ARC_destination(aprev);
			
			/* compare the two states */
			Found = equal_states(currentInPath, sprev);
			assert(!(Found && (sprev == NET_start_state(net))));
		}	
	}
	
	if (Found) {
		assert(sprev!=currentInPath);
		/* Update the current in Net */
		*currentInNet = sprev;
	
		/* delete the outgoing arcs of the state : */
		free_arc_set(currentInPath, net);
		NET_states(net)=delete_state_from_list(currentInPath, NET_states(net));
		NET_num_states(net)--;
		
		return sprev;
	}
	else {
		/* no more minimization : */
		*currentInNet = NULL;
		
		/* Add the state in the register by building its reverse arcs */
		for (a_clean = STATE_arc_set(currentInPath); a_clean; a_clean = next_arc(a_clean)) {
			/* create the reverse arc */
			newArc=arc_alloc(ARC_label(a_clean), currentInPath);
			
			/* Insert the arc in the target list */
			sprev=ARC_destination(a_clean);
			next_arc(newArc) = STATE_client_cell(sprev);
			STATE_client_cell(sprev) = newArc;
		}
		
		
		return currentInPath;
	}
}





/* ***************************************************************************************************
 * isConvergent : Indicate if a state is confluent.
 * Input :
 *	state : the state we check for confluence.
 * Output :
 * 	return TRUE if the state have one or less arc, else FALSE.
 * ***************************************************************************************************
 */
BOOL isConvergent(STATEptr state) {
	ARCptr carc=STATE_client_cell(state);

	assert(carc);
	
	
	return (next_arc(carc) != NULL);
	
	return FALSE;
}




/* ***************************************************************************************************
 * first_state : return the position of the first confluence state in the comon prefix.
 * Input :
 *	commonPrefix : The sequence of states of the common prefix.
 *	length : the length of the common prefix.
 * Output :
 * 	return the position of the firstState, 0 if none.
 * ***************************************************************************************************
 */
int first_state(STATEptr *commonPrefix, int length) {
	int i;			/* the length of the path to the firtState */
	
	assert(commonPrefix);

	/* look for the first confluent state in the prefix */
	for(i=1; (i<=length) && !isConvergent(commonPrefix[i]); i++);

	if (i<=length) return i;
	else return 0;
}




/* ***************************************************************************************************
 * clone_state : duplicate a state with its arcs. (the reversed arcs are no copied).
 * Input :
 *	net : the net on which we are working.
 *	ost : the state to be cloned.
 * Output :
 * 	return return the clone.
 * ***************************************************************************************************
 */
STATEptr clone_state(NETptr net, STATEptr ost) {
	STATEptr newState;
	ARCptr ascan;
	ARCptr newArc;
	
	assert(ost);
	
	/* create the new state : */
	newState=make_state(NULL, net);
	STATE_client_cell(newState) = NULL;
	STATE_final(newState) = STATE_final(ost);
	
	assert(newState);
	
	/* and copy the outgoing arcs from the original state to the clone : */
	ascan = STATE_arc_set(ost);
	while (ascan) {
		/* clone the arc : */
		newArc=make_arc(ARC_label(ascan), ARC_destination(ascan), net);
		assert(newArc);
	
		/* insert the arc in the arclist of the new state : */
		next_arc(newArc) = STATE_arc_set(newState);
		STATE_arc_set(newState) = newArc;
		
		/*  next arc : */
		ascan = next_arc(ascan);
	}
	
	/* return the clone : */
	return newState;
}




/* ***************************************************************************************************
 * rebranch : rebranch a state to another through a given label. An arc with this label must exists !
 * Input :
 *	source : the state to rebranch.
 *	newTarget : the new target state.
 *	label : the label of the arc to rebranch.
 * Output :
 * 	return TRUE if the state have one or less arc, else FALSE.
 * ***************************************************************************************************
 */
void rebranch(STATEptr source, STATEptr newTarget, id_type label) {
	STATEptr oldTarget;
	ARCptr c_ascan;
	ARCptr darc;
	ARCptr *cc_ascan;

	/* look for the arc to rebranch : */
	for (c_ascan=STATE_arc_set(source); c_ascan && (ARC_label(c_ascan) != label); c_ascan = next_arc(c_ascan));
	assert(c_ascan);
	
	oldTarget = ARC_destination(c_ascan);
	
	/* look for the reverse arc to remove : */
	for (cc_ascan = (ARCptr*)(&oldTarget->client_cell); 
		*cc_ascan && ((ARC_label(*cc_ascan) != label) || (ARC_destination(*cc_ascan)!=source));
		cc_ascan = &next_arc(*cc_ascan));
	assert(*cc_ascan);
	
	/* remove the reverse arc */
	darc=*cc_ascan;
	*cc_ascan=next_arc(darc);


	/* branch the arc to the new target : */
	ARC_destination(c_ascan) = newTarget;
	
	/* use the old reverse arc for the new target : */
	next_arc(darc) = STATE_client_cell(newTarget);
	STATE_client_cell(newTarget) = darc;
}





/* ***************************************************************************************************
 * update_register : delete the current state or create the reverse arcs form its target (in order
 * to add it in the register) whether it has an equivalent state in the register or not.
 * Input :
 * 	net : the network we are building.
 *	currentInPath : the state to minimize.
 * 	currentInNet : the last state in the register that has been minimized. NULL = no minimization !
 * Output :
 * 	return the the resulting state.
 * RQUE : the state must not be in the register ie it must not have reverse arcs !
 * ***************************************************************************************************
 */
STATEptr update_register(NETptr net, STATEptr currentInPath, STATEptr *currentInNet) {
	ARCptr aprev, ascan, *iscan;
	STATEptr sprev, trg;
	int Found=FALSE;
	
	assert(currentInPath);
	
	if (*currentInNet) {
		/* look for a equivalent state in the currentInNet previous state list : */
		for (aprev=STATE_client_cell(*currentInNet); aprev && !Found; aprev=next_arc(aprev)) {
			sprev=ARC_destination(aprev);
			
			/* compare the two states */
			Found = (sprev!=currentInPath) && equal_states(currentInPath, sprev);
			assert(!(Found && (sprev == NET_start_state(net))));
		}	
	}
	
	
	if (Found) {
		assert(sprev!=currentInPath);
		
		/* Update the current in Net */
		*currentInNet = sprev;
		/* clear the targets' client_cells : */
		for (ascan=STATE_arc_set(currentInPath); ascan; ascan=next_arc(ascan)) {
			trg = ARC_destination(ascan);
			
			for (iscan=(ARCptr*)&(STATE_client_cell(trg)); *iscan; ) {
				if (ARC_destination(*iscan) == currentInPath) {
					*iscan = next_arc(*iscan);
				}
				else iscan=&(next_arc(*iscan));
			}
		
		}

		/* verify that there is only one incoming arc : */
		ascan=STATE_client_cell(currentInPath);
		assert(next_arc(ascan) == NULL );
		
		/* rebranch the previous state to the equivalent one */
		trg = ARC_destination(ascan);
		for (ascan=STATE_arc_set(trg); ascan; ascan=next_arc(ascan)) {
			if (ARC_destination(ascan) == currentInPath) {
				rebranch(trg, sprev, ARC_label(ascan));
			}
		}
		
		
		/* delete the outgoing arcs of the state : */
		free_arc_set(currentInPath, net);
		NET_states(net)=delete_state_from_list(currentInPath, NET_states(net));
		NET_num_states(net)--;
		
		return sprev;
	}
	else {
		/* no more minimization : */
		*currentInNet = NULL;
		
		return currentInPath;
	}
}





/* ***************************************************************************************************
 * add_to_net : adds a word to a net.
 * Input :
 *	net : the state we add the word to.
 *	finalState : the address of a pointer to the finalState (can be changed by add_suffix)
 *	seq : the sequence of id describing the word to add.
 * Output :
 * 	none.
 * ***************************************************************************************************
 */
void add_to_net(NETptr net, STATEptr *finalState, id_type* seq) {
	int i;
	int len;				/* length of the word */
	int plen;				/* The length of the prefix */
	STATEptr *commonPrefix;	/* array with pointer to the states in the prefix. */
	STATEptr lastState;		/* Pointer to the lastState of the prefix */
	STATEptr currentState;	/* Pointer to the lastState of the prefix */
	int firstState;		/* index of the firstState in the prefix */
	STATEptr currentInNet;	/* The state in the net the currentState has been mininized with */
	id_type *currentSuffix;	/* Pointer to the suffix of the word to add. */
	ARCptr ascan;

	/* compute the length of the word to add */
	for (len=0; seq[len]!=ID_EOS; len++);
	
	/* allocate the array for the prefix : */
	commonPrefix = malloc((len+1)*sizeof(STATEptr));

	/* retrieve the length of the common prefix and the lastState : */
	plen = longest_prefix(net, seq, commonPrefix);
	lastState = commonPrefix[plen];
	
	if (STATE_final(lastState) && (len==plen)) {
		free(commonPrefix);
		return;
	}
	
	/* point to the suffix in the sequence : */
	currentSuffix=seq;
	currentSuffix+=plen;

	firstState = first_state(commonPrefix, plen);
	
	if (firstState) {
		lastState=clone_state(net, lastState);
	}

	/* add the suffix to the lastState : */
	currentInNet = add_suffix(net, lastState, currentSuffix, finalState, firstState);

	if (firstState) {
		/* replace lastState by an equivalemt state, or create reverse arcs : */
		lastState = replace_or_register(net, lastState, &currentInNet);

		/* has the firstState been changed by the adding of the suffix? :*/
		firstState = first_state(commonPrefix, plen);
		assert(firstState);
		
		for (i=plen-1; i>=firstState; i--) {
			/* clone the previous state : */
			currentState = clone_state(net, commonPrefix[i]);

			/* look for the arc to rebranch : */
			for(ascan=STATE_arc_set(currentState); ascan && (ARC_label(ascan)!=seq[i]); ascan=next_arc(ascan));
			assert(ascan);
			ARC_destination(ascan) = lastState;
			
			
			lastState = replace_or_register(net, currentState, &currentInNet);
		}
		
		rebranch(commonPrefix[i], lastState, seq[i]);
	}
	else i=plen;

	while (currentInNet && (i>0)) {
		update_register(net, commonPrefix[i], &currentInNet);
		
		i--;
	}

	free(commonPrefix);
}




void Clean_CC(STATEptr stc) {
	ARCptr adel;
	
	while (adel = STATE_client_cell(stc)) {
		STATE_client_cell(stc) = next_arc(adel);
		
		free_arc(adel);
	}

}



/* ***************************************************************************************************
 * seqs_to_net : the function called by read text.
 * Input :
 *	seq_form : pointer to a function that read word.
 * Output :
 * 	return the network build.
 * ***************************************************************************************************
 */
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
	NETptr net = null_fsm();	/* The create a NEW network */
	id_type* seq;			/* Pointer to a symbol id in the sequence */
	STATEptr finalState;		/* the final state of the minimized part */
	STATEptr cState;		/* the final state of the minimized part */
	ARCptr ascan, rscan;
	FILE *arcin, *arcout;
  
	NET_minimized(net) = FALSE;
	label_to_binary(NET_labels(net));

	printf("start\n");

	finalState = make_state(NULL, net);
	STATE_client_cell(finalState) = NULL;
	STATE_final(finalState)=TRUE;



	while ((seq = seq_form()) && (*seq != ID_EOS)) {
		add_to_net(net, &finalState, seq);
	}

	assert(!(STATE_client_cell(NET_start_state(net))));


	printf("\nCleaning Client Cell\n");
	for (cState=net->body.states; cState; cState=cState->next) { Clean_CC(cState); }

	printf("\nstop\n");
	
	binary_to_label(NET_labels(net));
	NET_sigma(net) = label_sigma(net);
	return (net);
}




