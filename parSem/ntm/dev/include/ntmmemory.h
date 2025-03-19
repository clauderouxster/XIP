/* $Id: ntmmemory.h,v 1.1 2005/02/11 17:40:00 grondeau Exp $ */ 


#ifndef NTM_NTMMEMORY_H
#define NTM_NTMMEMORY_H


#include "types.h"
#include "ntmtoken.h"



#define IDSEQ_HEAP_INIT_SIZE 10000
#define TOKEN_HEAP_INIT_SIZE 500
#define AVERAGE_ENTRY_NB 5
#define ENTRY_HEAP_INIT_SIZE (TOKEN_HEAP_INIT_SIZE * AVERAGE_ENTRY_NB) 

#define NORMSTATE_HEAP_INIT_SIZE 20000
#define NORMELEMENT_HEAP_INIT_SIZE 10000



/* STRUCTURE
*************/

typedef struct NtmMemory {
    NormState normstateHeap[NORMSTATE_HEAP_INIT_SIZE];
    long int normstateNbMax;
    long int normstateNb;
    NormElement normelementHeap[NORMELEMENT_HEAP_INIT_SIZE];
    long int normelementNb;
    long int normelementNbMax;
    id_type idseqHeap[IDSEQ_HEAP_INIT_SIZE];
    long int idseqNb;
    long int idseqNbMax;
    NtmEntry entryHeap[ENTRY_HEAP_INIT_SIZE];
    long int entryNb;
    long int entryNbMax;
    NtmToken tokenHeap[TOKEN_HEAP_INIT_SIZE];
    long int tokenNb;
    long int tokenNbMax;
} NtmMemory;





#endif /* NTM_NTMMEMORY_H */
