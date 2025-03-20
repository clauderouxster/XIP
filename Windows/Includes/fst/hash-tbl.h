/* $Id: hash-tbl.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

#ifndef C_FSM_HASH_TBL

#define C_FSM_HASH_TBL

#include "fst/syst-dep.h"

#include "fst/heap.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CELL {
  void *element;
  void *content;
  struct CELL *next;
} CELLtype, *CELLptr;

#define CELL_element(X) (X)->element
#define CELL_content(X) (X)->content
#define CELL_next(X) (X)->next
  
typedef struct HASH_TABLE {
  HEAPptr cell_heap;
  CELLptr *buckets;
  int bucket_count; /* size of BUCKETS (fixed) */
} HASH_TABLEtype, *HASH_TABLEptr;

#define HT_cell_heap(X) (X)->cell_heap
#define HT_buckets(X) (X)->buckets
#define HT_bucket(X,Y) (X)->buckets[(Y)]
#define HT_bucket_count(X) (X)->bucket_count
  
HASH_TABLEptr FstBaseExport make_hash_table(size_t bucket_count, char *name);
void FstBaseExport free_hash_table(HASH_TABLEptr h);
int FstBaseExport put_hash(HASH_TABLEptr table, void *element, void *value,
	     uint32 (*hashfn)(void *), int (*comparefn)(void *, void *));
int FstBaseExport get_hash(HASH_TABLEptr table, void *element, void **value,
	     uint32 (*hashfn)(void *), int (*comparefn)(void *, void *));
int FstBaseExport del_hash(HASH_TABLEptr table, void *element, uint32 (*hashfn)(void *),
	     int (*comparefn)(void *, void *));
long int FstBaseExport hash_table_size(HASH_TABLEptr table, int total_p);
int FstBaseExport update_hash(HASH_TABLEptr table, void *element, void **pvalue,
	     uint32 hashfn(void *), int comparefn(void *, void *),
		void *combinefn(void *, void *) );
void FstBaseExport map_hash(HASH_TABLEptr table, int (*map_fn)(void *, void *));

#ifdef __cplusplus
}
#endif

#endif  /* C_FSM_HASH_TBL */
