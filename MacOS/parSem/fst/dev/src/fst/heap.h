/* $Id: heap.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  heap.h
 **
 **	 Todd Yampol, Xerox PARC, May 1990
 **
 ******************************************************/

#ifndef C_FSM_HEAP

#define C_FSM_HEAP

#include "fst/fst_windefs.h"
#include <stdio.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* set the "fst_debug_heap_cell" global variable to actually debug the heap
   cell allocation:
   - FstDebugHeapCellNoDebugMode: Does not debug the heap cell allocation.
   - FstDebugHeapCellMallocMode: replace cell_alloc() by calloc() and
         cell_free() by free(). This is useful when used with purify to
         find leaks. Purify can give a longer call stack for each unfreed
         cell.
   - FstDebugHeapCellInternalMode: Add the FstDebugHeapCell structure to each 
         cell so that we can track cell allocation problems.
*/
typedef enum _FstDebugHeapCellMode { FstDebugHeapCellNoDebugMode,
               FstDebugHeapCellMallocMode,
               FstDebugHeapCellInternalMode } FstDebugHeapCellMode ;
extern FstDebugHeapCellMode FstBaseExport fst_debug_heap_cell_mode;

#ifdef DEBUG
/* this is the structure that is attach the cell itself.
   It can be done only if the cell_size is a multiple of sizeof(char *) */
typedef struct _FstDebugHeapCell
{
    unsigned short magic;       /* a magic number to be sure that this is good cell! */
    unsigned short linenum;     /* the line number in "filename" */
    char *filename;             /* the file name where this cell was allocated or freed */
    unsigned int in_use:1;      /* one bit set to 1 if cell is allocated. */
    unsigned int flag:1;        /* one bit used by check cell in use */
    unsigned int reuse_count:30; /* The number of time this cell was reused */
} FstDebugHeapCell;

#endif

typedef struct BLOCK {
	void *cells;			/* vector of cells */
	struct BLOCK *next;		/* blocks are linked together */
} BLOCKtype, *BLOCKptr;

#define BLOCK_cells(X) (X)->cells
#define BLOCK_next(X) (X)->next
#define HEAP_NAME_MAX_LEN 23

typedef struct HEAP {
    char name[HEAP_NAME_MAX_LEN +1];
	BLOCKptr first;
	BLOCKptr cur;
	void *cur_cell;
	void *free_list;
	size_t cell_size;
	size_t block_size;
	long cur_loc;
	struct HEAP *next_heap;
} HEAPtype, *HEAPptr;

#define HEAP_name(X)		(X)->name
#define HEAP_first(X) 		(X)->first
#define HEAP_cur(X) 		(X)->cur
#define HEAP_cur_cell(X) 	(X)->cur_cell
#define HEAP_free_list(X) 	(X)->free_list
#ifdef DEBUG
/* in debug mode, add 2 fields to the cell: filename and linenum */
int FstBaseExport HEAP_cell_size(HEAPptr heap);
#else
#define HEAP_cell_size(X) 	(X)->cell_size
#endif
#define HEAP_block_size(X) 	(X)->block_size
#define HEAP_cur_loc(X) 	(X)->cur_loc
#define HEAP_next_heap(X)	(X)->next_heap

extern HEAPptr FstBaseExport ALL_HEAPS;

HEAPptr FstBaseExport init_heap(size_t cell_size, size_t block_size, char *name);
long int FstBaseExport heap_items(HEAPptr heap, int total_p);
long int FstBaseExport heap_size(HEAPptr heap, int total_p);
long int FstBaseExport free_list_size(HEAPptr heap);

#ifdef DEBUG
void FstBaseExport *FstDebug_cell_alloc(HEAPptr heap, char *filename, int linenum);
#define cell_alloc(h) FstDebug_cell_alloc(h, __FILE__, __LINE__)
#else
void FstBaseExport *cell_alloc(HEAPptr heap);
#endif
void *debug_cell_alloc(HEAPptr heap, long int *cell_id);

#ifdef DEBUG
void FstBaseExport FstDebug_free_cell(void *cell, HEAPptr heap, char *filename, int linenum);
#define free_cell(cell, heap) FstDebug_free_cell(cell, heap, __FILE__, __LINE__)
#else
void FstBaseExport free_cell(void *cell, HEAPptr heap);
#endif

void free_cell_debug(void *cell, HEAPptr heap);
void FstBaseExport free_heap(HEAPptr heap);
void free_all_heaps(void);
void FstBaseExport reset_heap(HEAPptr heap);
void debug_reset_heap(HEAPptr heap);
void map_heap (HEAPptr heap, void map_fn(void *cell));
void heap_info(HEAPptr h, long *in_use, long *total, long *size);
long FstBaseExport tally_heaps(FILE *stream);
void tally_heap(HEAPptr h, long *tally, FILE *stream);

#ifdef DEBUG
  /* check that all cells have been freed */
void FstDebugCheckHeapCellInUse(HEAPptr heap);
void FstDebugHeapCellReset(HEAPptr heap);
void FstDebugHeapCellResetBlock(HEAPptr heap, BLOCKptr block);
#endif

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_HEAP */
