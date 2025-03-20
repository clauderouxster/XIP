/* $Id: ntmmemory.c,v 1.7 2004/05/14 09:46:59 ait Exp $ */ 


#include "ntmmemory.h"
#include "ntm.h"
#include "ntmextern.h"

/* Creates a normstate from the normstateHeap and returns a pointer
   to it. If no space is left, returns NULL
****************************************************************/
inline NormState * createNormState (
    NtmContext *ct,
    id_type id,
    ntmbit final,
    NormState *next,
    int nextnb
    ) {
    NtmMemory *mem = &(ct->memory);

    if (mem->normstateNb >= mem->normstateNbMax) {
        warningHandler(ct, "NormStates heap overflow");
        return NULL;
    } else {
        mem->normstateHeap[mem->normstateNb].symbolId = id;
        mem->normstateHeap[mem->normstateNb].isInitial = 0;
        mem->normstateHeap[mem->normstateNb].isFinal = final;
        mem->normstateHeap[mem->normstateNb].nextNb = nextnb;
        mem->normstateHeap[mem->normstateNb].firstNext = next;
        return (mem->normstateHeap + mem->normstateNb++);
    }
}


/* Creates a normelement from the normelementHeap and returns a pointer
   to it. If no space is left, returns NULL
****************************************************************/
inline NormElement * createNormElement (
    NtmContext *ct,
    int initpos,
    int inputlength,
    NormState *normstate
    ) {
    NtmMemory *mem = &(ct->memory);

    if (mem->normelementNb >= mem->normelementNbMax) {
        warningHandler(ct, "normalized level data overflow");
        return NULL;
    } else {
        mem->normelementHeap[mem->normelementNb].initpos = initpos;
        mem->normelementHeap[mem->normelementNb].inputLength = inputlength;
        normstate->isInitial = 1;
        mem->normelementHeap[mem->normelementNb].normState = normstate;
        return (mem->normelementHeap + mem->normelementNb++);
    }
}


/* Creates an Id sequence from the idseqHeap and returns a pointer
   to it. If no space is left, returns NULL
****************************************************************/
extern id_type * createIdSeq(
    NtmContext *ct,
    int size
    ) {
    NtmMemory *mem = &(ct->memory);

    if ( (mem->idseqNb + size + 1) >= mem->idseqNbMax ) {
        warningHandler(ct, "IdSequences array overflow");
        return NULL;
    } else {
        mem->idseqNb += size + 1;
        return (mem->idseqHeap + mem->idseqNb - size - 1);
    }
}


/* Creates an entry from the entryHeap and returns a pointer
   to it. If no space is left, returns NULL
****************************************************************/
inline NtmEntry * createEntry(
    NtmContext *ct,
    id_type *nform,
    id_type *lemma,
    id_type *features
    ) {
    NtmMemory *mem = &(ct->memory);

    if (mem->entryNb >= mem->entryNbMax) {
        warningHandler(ct, "Entry array overflow");
        return NULL;
    } else {
        mem->entryHeap[mem->entryNb].normform = nform;
        mem->entryHeap[mem->entryNb].lemma = lemma;
        mem->entryHeap[mem->entryNb].features = features;
        return (mem->entryHeap + mem->entryNb++);
    }
}


/* Creates a token from the tokenHeap and returns a pointer
   to it. If no space is left, returns NULL
****************************************************************/
extern NtmToken * createToken(
    NtmContext *ct,
    int initpos,
    int inputlength,
    int ignore_flag
    ) {
    int i=0;
    NtmMemory *mem = &(ct->memory);

    if (mem->tokenNb >= mem->tokenNbMax) {
        warningHandler(ct, "Token array overflow");
        return NULL;
    } else {
        mem->tokenHeap[mem->tokenNb].initpos = initpos;
        mem->tokenHeap[mem->tokenNb].inputLength = inputlength;
        mem->tokenHeap[mem->tokenNb].ignore = ignore_flag;
        mem->tokenHeap[mem->tokenNb].weight = 0;
        mem->tokenHeap[mem->tokenNb].tambig = 0;
        mem->tokenHeap[mem->tokenNb].entryNb = 0;
        mem->tokenHeap[mem->tokenNb].entrySeq = NULL;
        mem->tokenHeap[mem->tokenNb].longerToken = NULL;
        mem->tokenHeap[mem->tokenNb].shorterToken = NULL;
        mem->tokenHeap[mem->tokenNb].previousToken = NULL;
        mem->tokenHeap[mem->tokenNb].nextToken = NULL;
        mem->tokenHeap[mem->tokenNb].strategy = 0;
        if (ct->printByteIndices) {
            /* set the offset and inputLenght in terms of Bytes */
            mem->tokenHeap[mem->tokenNb].initBytepos = ct->inputBytePos;
            mem->tokenHeap[mem->tokenNb].inputByteLength = 0;
            for(i=initpos; i < (inputlength+initpos); i++) {
                mem->tokenHeap[mem->tokenNb].inputByteLength +=
                    ct->levels.inputIdLen[getInputCharId(&(ct->levels), i)];
            }
                
        }
        return (mem->tokenHeap + mem->tokenNb++);
    }
}


/* Init the ntm memory
***********************/
inline void initNtmMemory(NtmMemory *mem) {
    mem->normstateNbMax = NORMSTATE_HEAP_INIT_SIZE;
    mem->normstateNb = 0;
    mem->normelementNbMax = NORMELEMENT_HEAP_INIT_SIZE;
    mem->normelementNb = 0;
    mem->idseqNbMax = IDSEQ_HEAP_INIT_SIZE;
    mem->idseqNb = 0;
    mem->entryNbMax = ENTRY_HEAP_INIT_SIZE;
    mem->entryNb = 0;
    mem->tokenNbMax = TOKEN_HEAP_INIT_SIZE;
    mem->tokenNb = 0;
}

/* Resets ntm memory mem
  !!!! Achtung !
  !!!! Next call to the allocating functions will erase the data
  !!!! stored in the heaps
**************************************************************/
inline void FsmNTMExport resetNtmMemory(
    NtmMemory * mem
    ) {

    mem->normstateNb = mem->normelementNb = 0;
    mem->idseqNb = mem->entryNb = mem->tokenNb = 0;
}







