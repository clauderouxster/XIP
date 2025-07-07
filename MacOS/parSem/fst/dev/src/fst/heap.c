/* $Id: heap.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  heap.c
 **
 **	 Todd Yampol, Xerox PARC, May 1990
 **
 **  Heap memory allocation, used for states, arcs, etc.
 **  For large blocks of uniform size elements, this is substantially
 **  faster than malloc().
 **
 **  >> Free list capability added, 4/92, TSY, 
 **  >> cell_size must be at least the size of a pointer to use free_list
 **
 ******************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>	/* YM, 2/9/96 */

#include "fst/error.h"
#include "fst/heap.h"
#include "fst/fsm_io.h"

HEAPptr ALL_HEAPS = NULL;


FstDebugHeapCellMode fst_debug_heap_cell_mode = FstDebugHeapCellNoDebugMode;

#ifdef DEBUG
#define DEBUG_CELL_MAGIC_NUMBER 0x55AA
FstDebugHeapCell *FstDebugGetDebugCell(HEAPptr heap, void *cell)
{
  FstDebugHeapCell *debug_cell = (FstDebugHeapCell *)((char *)cell + heap->cell_size);
  return debug_cell;
}



/* in debug mode, add 2 fields to the cell: filename and linenum */
int HEAP_cell_size(HEAPptr heap)
{
  if (fst_debug_heap_cell_mode == FstDebugHeapCellInternalMode)
    return heap->cell_size + sizeof(FstDebugHeapCell);
  else
    return heap->cell_size;
}
#endif



void heap_info(HEAPptr h, long *in_use, long *total, long *size)
{
  if (h != NULL)
    {
      *total += heap_items(h, 1); 
      *in_use += heap_items(h, 0);
      *size += heap_size(h, 1);
    }
}


void tally_heap(HEAPptr h, long *tally, FILE *stream)
{
  if (h != NULL)
    {
      long in_use = 0, total = 0, size = 0;
      int indent = HEAP_NAME_MAX_LEN - strlen(HEAP_name(h));
      
      heap_info(h, &in_use, &total, &size);

      *tally += size;

	  while (indent-- > 0)
	    fputc(' ', stream);
	    
      fsm_fprintf(stream, "%s: %4ld bytes, %7ld in use, %7ld total (%ldK)\n",
	      HEAP_name(h), (long)HEAP_cell_size(h), in_use, total, size/1024);
    }
} /* tally_heap */

long tally_heaps(FILE *stream)
{
  HEAPptr heap;
  int i, indent = HEAP_NAME_MAX_LEN;
  long tally = 0;
  
  if (!ALL_HEAPS)
    return(tally);
  
  for (heap = ALL_HEAPS; heap; heap = HEAP_next_heap(heap))
    if ((i = HEAP_NAME_MAX_LEN - strlen(HEAP_name(heap))) < indent)
       indent = i;
    
  for (i = 0; i < indent; i++)
	fputc(' ', stream);
  fsm_fprintf(stream, "* Heaps *\n");
  
  for (heap = ALL_HEAPS; heap; heap = HEAP_next_heap(heap))
    tally_heap(heap, &tally, stream);
  
  for (i = 0; i < indent; i++)
	fputc(' ', stream);
	   
  for (i = 0; i < 70; i++)
	fputc('=', stream);
	
  fputc('\n', stream);
  
  for (i = 0; i < indent; i++)
	fputc(' ', stream);
	
  fsm_fprintf(stream, "Total: %ld Kbytes.\n", tally/1024);
  return(tally);
} /* tally_heaps */


/* Create a heap with given parameters:
   - "name"= heap name for future identification.
   - "block_size"= number of contiguous cells in a block.
   - "cell_size"= size of each cell pointed by a block.

   The first block structure is allocated but it is empty i.e. its
   field BLOCK_cells is NULL; it is the first call to cell_alloc() on
   that heap that will allocate the first package of cells.  */
HEAPptr init_heap(size_t cell_size, size_t block_size, char *name)
{
  HEAPptr new;
  
  if (strlen(name) > HEAP_NAME_MAX_LEN)
    handle_error("Heap name longer than HEAP_NAME_MAX_LEN characters",
    	"INIT_HEAP", strlen(name));
    
  new = (HEAPptr) calloc((size_t) 1, (size_t) sizeof(HEAPtype));
  
  if (!new)
    not_enough_memory("INIT_HEAP");
    
  strcpy(HEAP_name(new), name);

  /* don't use HEAP_cell_size(new) macro: it is not assignable in DEBUG mode */
  new->cell_size = cell_size;
  HEAP_block_size(new) = block_size;
  HEAP_cur(new) = HEAP_first(new) =
    (BLOCKptr) calloc((size_t) 1, (size_t) sizeof(BLOCKtype));
#ifdef DEBUG
  if (fst_debug_heap_cell_mode == FstDebugHeapCellInternalMode)
      /* Check that cell_size is a multiple of sizeof(char *) */
      if ((cell_size % sizeof(char *) ) != 0)
      {
          handle_error("Cell size not a multiple of sizeof(FstDebugHeapCell.filename)",
                       "INIT_HEAP", cell_size);
      }
#endif
  
/* Don't allocate cells for the moment: it will be done later
   on-demand by cell_alloc() when needed. */
  BLOCK_cells(HEAP_cur(new)) = NULL;
  HEAP_cur_cell(new)= NULL;
  
#ifdef DEBUG
  FstDebugHeapCellResetBlock(new, HEAP_first(new));
#endif
  
  HEAP_free_list(new) = NULL;
  HEAP_cur_loc(new) = 0;
  BLOCK_next(HEAP_cur(new)) = NULL;
  
  HEAP_next_heap(new) = ALL_HEAPS;
  ALL_HEAPS = new;
  
  return (new);
} /* init_heap */


long int heap_items(HEAPptr heap, int total_p)
{
  /* When TOTAL_P is TRUE, returns the total number of items in the
     heap, otherwise the number of items that are currently in use.
     */
  
  BLOCKptr block = HEAP_first(heap);
  long int item_count = 0, incr = (long) HEAP_block_size(heap);
  void **free_cell = HEAP_free_list(heap);
  
  while (block != NULL && (total_p || block != HEAP_cur(heap)))
    {
      item_count += incr;
      block = BLOCK_next(block);
    }
  
  if (!total_p)
    {
      /* Add the items in the current block. */
      item_count += (long) HEAP_cur_loc(heap);
      
      /* subtract any items on the free list. If the count
	 becomes negative, there is an error somewhere.
	 Most likely, some element has been freed more
	 than once. */
      
      while (free_cell != NULL)
	{
	  item_count--;

	  if (item_count < 0)
	    {
	      handle_error("Loop in the free list", "HEAP_ITEMS", item_count);
	      return(0);
	    }

	  free_cell = (void **) *free_cell;
	}
    }
  return(item_count);
}


long int heap_size(HEAPptr heap, int total_p)
{
  /* When TOTAL_P is TRUE, returns the total number of bytes allocated
     to HEAP, otherwise the number of bytes that are currently in use.
     */
  
  BLOCKptr block = HEAP_first(heap);
  long int byte_count = (long) sizeof(HEAPtype);
  long int incr = sizeof(BLOCKtype)
                  + ((long) HEAP_block_size(heap) * HEAP_cell_size(heap));
  void **free_cell = HEAP_free_list(heap);

  /* Check if the heap is not empty: i.e. first block has no cells yet.*/
  if (BLOCK_cells(block) != NULL)
  {  
      while (block != NULL && (total_p || block != HEAP_cur(heap)))
      {
          byte_count += incr;
          block = BLOCK_next(block);
      }
  
      if (!total_p)
      {
          /* Add the cells in the current block. */
          byte_count += (long) HEAP_cur_loc(heap) * HEAP_cell_size(heap);
      
          /* subtract any cells on the free list */
      
          while (free_cell != NULL)
          {
              byte_count -= HEAP_cell_size(heap);
              free_cell = (void **) *free_cell;
          }
      }
  }
  return(byte_count);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

long free_list_size(HEAPptr heap)
{
  long cell_count;
  void **f;

  for (cell_count = 0, f = HEAP_free_list(heap); 
       f != NULL;
       cell_count++, f = (void **) *f);

  return (cell_count);
}
#endif /* SLIM_FST */

static BLOCKptr add_block(HEAPptr heap)
{
  BLOCKptr next = (BLOCKptr) calloc((size_t) 1, (size_t) sizeof(BLOCKtype));
  
  if (next == NULL)
    not_enough_memory("ADD_BLOCK");
  
  BLOCK_next(HEAP_cur(heap)) = next;
  
  BLOCK_cells(next) = (void *) calloc((size_t) HEAP_block_size(heap),
				      (size_t) HEAP_cell_size(heap));
#ifdef DEBUG
  FstDebugHeapCellResetBlock(heap, next);
#endif
  
  if (BLOCK_cells(next) == NULL)
    {
      long tally;
      
      extern long tally_stacks(FILE *stream);
      
      fsm_fprintf(stderr, "*** ERROR: Not enough memory. Cannot continue.\n");
      fsm_fprintf(stderr, "    This is the situation:\n");  
      tally = tally_stacks(stderr); 
      fputc(' ', stderr);
      tally += tally_heaps(stderr);
      fsm_fprintf(stderr, "Total %ld bytes in use.\n", tally);
#ifdef MAC_OS
      handle_error("couldn't allocate memory for cells", "add_block", 0);
#else
	  exit(1);
#endif
    }
  
  return(next);
}

static void next_block(HEAPptr heap)
{
  BLOCKptr next = BLOCK_next(HEAP_cur(heap));
  
  if (next == NULL)
    next = add_block(heap);
  
  HEAP_cur(heap) = next;
  HEAP_cur_cell(heap) = BLOCK_cells(next);
  HEAP_cur_loc(heap) = 0;
  
}

/* Cell allocation:
   returns the current available cell, either from the heap free_list
   or from the current block. Allocate a new block if needed.
   Allocates cells for the first block.  */
#ifdef DEBUG
void *FstDebug_cell_alloc(HEAPptr heap, char *filename, int linenum)
#else
void *cell_alloc(HEAPptr heap)
#endif
{
  void *cell;
  
  if (heap == NULL)
    {
      handle_error("can't allocate from null heap", "cell_alloc", 0);
      return (NULL);
    }
  else if (HEAP_cur_cell(heap) == NULL)
    {  
      /* The heap is empty: the first and unique block does not point
         to any allocated bulk of cells yet: allocate cells. */
        BLOCK_cells(HEAP_cur(heap)) 
            = (void*) calloc((size_t) HEAP_block_size(heap),
                             (size_t) HEAP_cell_size(heap));

        if (BLOCK_cells(HEAP_cur(heap)) == NULL)
            handle_error("couldn't allocate memory for cells", "INIT_HEAP",0);

        HEAP_cur_cell(heap)= BLOCK_cells(HEAP_cur(heap));

#ifdef DEBUG
        FstDebugHeapCellResetBlock(heap, HEAP_first(heap));
#endif
    }

#ifdef DEBUG
  /* DEBUG mode: either in NoDebug, Malloc or Internal mode */
  if (fst_debug_heap_cell_mode == FstDebugHeapCellMallocMode)
  {
      /* malloc mode; simply allocate cell with calloc */
      cell = calloc(1, HEAP_cell_size(heap));
  }
  else
  {
      /* either in non-debug mode or in Internal mode */
      /* Warning: the closing brace is in the next #ifdef DEBUG part */
#endif
      if (HEAP_free_list(heap) == NULL)
      {
          if (HEAP_cur_loc(heap) == (long)HEAP_block_size(heap))
              next_block(heap);
	  
          cell = HEAP_cur_cell(heap);
          HEAP_cur_cell(heap) =
              (void *) ((char *) HEAP_cur_cell(heap)+HEAP_cell_size(heap));
          
          HEAP_cur_loc(heap)++;
      }
      else
      {
          cell = HEAP_free_list(heap);
          HEAP_free_list(heap) = *(void **) cell;
          
          /* Clear the initial bytes just in case the caller
             forgets to initialize properly.  */
          *(void **) cell = (void *) NULL;
      }
#ifdef DEBUG
  }
  
  if (fst_debug_heap_cell_mode == FstDebugHeapCellInternalMode)
  {
      /* store our infos */
      FstDebugHeapCell *debug_cell = FstDebugGetDebugCell(heap, cell);
      /* set magic field to the magic number */
      debug_cell->magic = DEBUG_CELL_MAGIC_NUMBER;
      /* Set in_use field to 1 */
      debug_cell->in_use = 1;
          /* increment reuse count */
      debug_cell->reuse_count++;
      debug_cell->filename = filename;
      debug_cell->linenum = (unsigned short)linenum;
  }
#endif
  return (cell);

} /* cell_alloc */


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */


/*
void *debug_cell_alloc(HEAPptr heap, long int *cell_id)
{
  void *cell = cell_alloc(heap);
  
  *cell_id = 0;
  
  if (cell != NULL)
    {
	   BLOCKptr prev = NULL, block = HEAP_first(heap);
	   void *cur_cell;
	   long int loc;

	   do {
	     cur_cell = BLOCK_cells(block);
	     loc = 0;
	       
	     while (loc < HEAP_block_size(heap))
	       {
	         if (cur_cell == cell)
			   return(cell);
				 
			 cur_cell = (void *) ((char *) cur_cell + HEAP_cell_size(heap));
			 loc++;
			 *cell_id = *cell_id + 1; 
	         }
	       block = BLOCK_next(block);
	       
	     } while (block != NULL); 
    }
    
  handle_error("Cell not found", "DEBUG_CELL_ALLOC", 0);
  
  return (cell);
}
*/

static void debug1_free_cell(void *cell, HEAPptr heap)
{
  /* Checks whether the cell is already in the free list. */

  void *freed_cell;

  for (freed_cell = HEAP_free_list (heap);
       freed_cell;
       freed_cell = *(void **) freed_cell)
    if (freed_cell == cell)
      {
	fsm_fprintf (stderr, "ERROR: cell %p already freed\n", cell);
	break;  /* SET BREAKPOINT ON THIS LINE !!! */
      }
}
#endif /* SLIM_FST */

#ifdef DEBUG
/* check cell magic number. Return TRUE is magic number is ok, otherwise FALSE. */
int FstDebugCheckCellMagic(void *cell, HEAPptr heap)
{
  if (fst_debug_heap_cell_mode == FstDebugHeapCellInternalMode)
    {
      FstDebugHeapCell *debug_cell = FstDebugGetDebugCell(heap, cell);
      if (debug_cell->magic != DEBUG_CELL_MAGIC_NUMBER)
        {
          fsm_fprintf(stderr, "[HEAP \"%s\"] Bab magic number for cell at 0x%x: ", heap->name, cell);
          return 0;
        }
    }
  return 1;
}


void FstDebugFreeCell(void *cell, HEAPptr heap, char *filename, int linenum)
{
  if (fst_debug_heap_cell_mode != FstDebugHeapCellInternalMode)
    return;

      /* check magic number */
  if (FstDebugCheckCellMagic(cell, heap))
    {
      FstDebugHeapCell *debug_cell = FstDebugGetDebugCell(heap, cell);

      /* Check that this block waasn't already freed */
      if (debug_cell->in_use == 0)
        {
          fsm_fprintf(stderr, "[HEAP \"%s\"] Freeing unallocated cell:\n", heap->name);
          if (debug_cell->reuse_count == 0)
            {
              fsm_fprintf(stderr, "This cell was never allocated\n");
            }
          else
            {
              fsm_fprintf(stderr,
                          "-> cell of size %d bytes allocated from\n"
                          "   \"%s\" line %d after %d previous free(s)\n",
                          heap->cell_size, debug_cell->filename,
                          debug_cell->linenum, debug_cell->reuse_count - 1);
            }
        }
      else
        {
          debug_cell->in_use = 0;
          debug_cell->linenum = linenum;
          debug_cell->filename = filename;
        }
    }
}
#endif



#ifdef DEBUG
void FstDebug_free_cell(void *cell, HEAPptr heap, char *filename, int linenum)
#else
void free_cell(void *cell, HEAPptr heap)
#endif
     /* puts CELL at the front of HEAP's free list */
{
  if (HEAP_cell_size(heap) < sizeof(void *))
  	handle_error("cell_size must be larger than a pointer", "free_cell", 
  	             (int) HEAP_cell_size(heap));

#ifdef DEBUG
  /* DEBUG mode: either in NoDebug, Malloc or Internal mode */
  if (fst_debug_heap_cell_mode == FstDebugHeapCellMallocMode)
    {
      /* malloc mode; simply free cell */
      free(cell);
    }
  else
    {
      if (fst_debug_heap_cell_mode == FstDebugHeapCellInternalMode)
        {
          /* Internal mode: check cell is ok */
          FstDebugFreeCell(cell, heap, filename, linenum);
        }
      /* Warning: the closing brace is in the next #ifdef DEBUG part */
#endif /* DEBUG */

      /* debug1_free_cell(cell, heap);
         checks whether the cell is already in the free list.
         SET BREAKPOINT AT THE END OF THAT DEBUG FUNCTION !!! */
      *(void **) cell = HEAP_free_list(heap);
      HEAP_free_list(heap) = cell;
#ifdef DEBUG
    }
#endif

} /* free_cell */


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void free_cell_debug (void *cell, HEAPptr heap)
     /* puts CELL at the front of HEAP's free list */
{
  if (HEAP_cell_size(heap) < sizeof(void *))
  	handle_error("cell_size must be larger than a pointer", "free_cell", 
  	             (int) HEAP_cell_size(heap));

   debug1_free_cell(cell, heap);

   /*  checks whether the cell is already in the free list.
     SET BREAKPOINT AT THE END OF THAT DEBUG FUNCTION !!! */

  *(void **) cell = HEAP_free_list(heap);
  HEAP_free_list(heap) = cell;
}

#endif /* SLIM_FST */


static void release_heap(HEAPptr heap)
/* Release memory for one heap.  HEAP must be non-null. */
{
  BLOCKptr temp, b;

#ifdef DEBUG
  if (fst_debug_heap_cell_mode == FstDebugHeapCellInternalMode)
    FstDebugCheckHeapCellInUse(heap);
#endif

  b = HEAP_first(heap);
  while (b != NULL)
    {
      temp = b;
      b = BLOCK_next(b);
      free(BLOCK_cells(temp));
      free(temp);
    }
  free(heap);
}

void free_heap(HEAPptr heap)
/* Free one heap.  Remove from ALL_HEAPS chain. */
{
  HEAPptr cur, prev;
	
  if (heap != NULL)
    {
      /* remove HEAP from ALL_HEAPS chain */
      for (prev = NULL, cur = ALL_HEAPS; 
           cur != NULL; 
           prev = cur, cur = HEAP_next_heap(cur))
        {
          if (heap == cur)
            {
              if (prev)
                HEAP_next_heap(prev) = HEAP_next_heap(cur);
              else
                ALL_HEAPS = HEAP_next_heap(cur); /* first in chain */
              break;
            }
        }
      release_heap(heap);	
    }
} /* free_heap */


void free_all_heaps(void)
/* Free every heap that has been created. */
{
  HEAPptr next;
	
  while (ALL_HEAPS)
    {
      next = HEAP_next_heap(ALL_HEAPS);
      release_heap(ALL_HEAPS);
      ALL_HEAPS = next; 
    }
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void reset_heap(HEAPptr heap)
{
  if (heap == NULL)
    return;

#ifdef DEBUG
  if (fst_debug_heap_cell_mode == FstDebugHeapCellInternalMode)
    {
      FstDebugCheckHeapCellInUse(heap);
      FstDebugHeapCellReset(heap);
    }
#endif

  HEAP_cur(heap) = HEAP_first(heap);
  HEAP_cur_cell(heap) = BLOCK_cells(HEAP_first(heap));
  HEAP_cur_loc(heap) = 0;
  HEAP_free_list(heap) = NULL;
}
#endif /* SLIM_FST */

/*
void debug_reset_heap(HEAPptr heap)
{

   if (heap == NULL)
     return;
   else
     {
   void *next, *cell = HEAP_free_list(heap);
   BLOCKptr prev = NULL, block = HEAP_first(heap);
   long int loc, cell_id = 0;
   
   if (block == HEAP_cur(heap) && HEAP_cur_loc(heap) == 0)
     return;
   
   while (cell != NULL)
     {
        next = *(void **) cell;
        *(void **)cell = (void *) 0xB747;
        cell = next;
     }
   
   HEAP_free_list(heap) = NULL;

  fsm_fprintf(stderr, "\nCells in use: ");
  do {
	   cell = BLOCK_cells(block);
	   loc = 0;
	       
	   while (loc < HEAP_block_size(heap) &&
	          (block != HEAP_cur(heap) || loc < HEAP_cur_loc(heap)))
	     {
	          
	       if (*(void **) cell == (void *) 0xB747)
			 free_cell(cell, heap);
	       else
			 fsm_fprintf(stderr, "%ld ", cell_id);
				 
			cell = (void *) ((char *) cell + HEAP_cell_size(heap));
			loc++;
			cell_id++; 
	     }
	    block = BLOCK_next(block);
	       
  } while (block != NULL); 

  fsm_fprintf(stderr, "\n");
 }
}
*/

#ifdef DEBUG

void FstDebugHeapCellResetBlock(HEAPptr heap, BLOCKptr block)
{
  if (fst_debug_heap_cell_mode != FstDebugHeapCellInternalMode)
    return;

  if ((block == NULL) || BLOCK_cells(block) == NULL)
    return;
  else
    {
      long loc;

      for (loc = 0; loc < HEAP_block_size(heap); loc++)
        {
          void *cell = (char *)BLOCK_cells(block) + (loc * HEAP_cell_size(heap));
          FstDebugHeapCell *debug_cell = FstDebugGetDebugCell(heap, cell);

          debug_cell->magic = DEBUG_CELL_MAGIC_NUMBER;
          debug_cell->flag = 0;
          debug_cell->in_use = 0;
          debug_cell->filename = NULL;
          debug_cell->linenum = 0;
        }
    }
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void FstDebugHeapCellReset(HEAPptr heap)
{
  if (fst_debug_heap_cell_mode != FstDebugHeapCellInternalMode)
    return;

  if (heap == NULL)
    return;
  else
    {
      BLOCKptr block;

      for (block = HEAP_first(heap); block != NULL; block = BLOCK_next(block))
          FstDebugHeapCellResetBlock(heap, block);
    }
}
#endif /* SLIM_FST */

void FstDebugHeapCellResetUserFlag(HEAPptr heap)
{
  if (fst_debug_heap_cell_mode != FstDebugHeapCellInternalMode)
    return;

  if ((heap == NULL) || BLOCK_cells(HEAP_first(heap)) == NULL)
      /* no heap, or heap empty: */
    return;
  else
    {
      BLOCKptr block;

      /* clear in_use flag */
      for (block = HEAP_first(heap); block != NULL; block = BLOCK_next(block))
        {
          long loc;

          for (loc = 0; loc < HEAP_block_size(heap); loc++)
            {
              void *cell = (char *)BLOCK_cells(block) + (loc * HEAP_cell_size(heap));
              if (FstDebugCheckCellMagic(cell, heap))
                {
                  FstDebugHeapCell *debug_cell = FstDebugGetDebugCell(heap, cell);
                  debug_cell->flag = 0;
                }
            }
        }
    }
}


/* check all cell in use that were allocated from "filename" at line
   "linenum". Set flag to 1 and returns the number of cell found. */

int FstDebugCheckHeapCellInUseFromLocation(HEAPptr heap, char *filename, int linenum)
{
  int num = 0;
  BLOCKptr block;

  /* check cell in use */
  for (block = HEAP_first(heap); block != NULL; block = BLOCK_next(block))
    {
      long loc;

      for (loc = 0; loc < HEAP_block_size(heap); loc++)
        {
          void *cell = (char *)BLOCK_cells(block) + (loc * HEAP_cell_size(heap));
          if (FstDebugCheckCellMagic(cell, heap))
            {
              FstDebugHeapCell *debug_cell = FstDebugGetDebugCell(heap, cell);

              if ((debug_cell->in_use == 1) && (debug_cell->flag == 0) &&
                  (debug_cell->filename == filename) &&
                  (debug_cell->linenum == linenum))
                {
                  num++;
                  debug_cell->flag = 1;
                }
            }
        }
    }
  return num;
}


void FstDebugCheckHeapCellInUse(HEAPptr heap)
{
  int header_printed = 0;

  if (fst_debug_heap_cell_mode != FstDebugHeapCellInternalMode)
    return;

  if ((heap == NULL) || BLOCK_cells(HEAP_first(heap)) == NULL)
      /* no heap, or heap empty: */
    return;
  else
    {
      BLOCKptr block;

      /* clear flags */
      FstDebugHeapCellResetUserFlag(heap);

      /* check cell in use */
      for (block = HEAP_first(heap); block != NULL; block = BLOCK_next(block))
        {
          long loc;

          for (loc = 0; loc < HEAP_block_size(heap); loc++)
            {
              void *cell = (char *)BLOCK_cells(block) + (loc * HEAP_cell_size(heap));

              if (FstDebugCheckCellMagic(cell, heap))
                {
                  FstDebugHeapCell *debug_cell = FstDebugGetDebugCell(heap, cell);

                  if (debug_cell->flag == 0)
                    {
                      /* not already printed */
                      if (debug_cell->in_use == 1)
                        {
                          int num;
                          num = FstDebugCheckHeapCellInUseFromLocation(
                                    heap, debug_cell->filename, debug_cell->linenum);

                          if (header_printed == 0)
                            {
                              int num_blocks = 0;
                              BLOCKptr b;
                              /* count number of blocks */
                              for (b = HEAP_first(heap); b != NULL; b = BLOCK_next(b))
                                num_blocks++;

                                /* print header only once */
                              fsm_fprintf(stderr,
                                          "List of cells in use in heap \"%s\" (Total: %d):\n",
                                          heap->name, num_blocks * HEAP_block_size(heap));
                              header_printed = 1;
                            }
                          fsm_fprintf(stderr,
                                      "-> %d cell(s) of size %d bytes allocated from\n"
                                      "   \"%s\" line %d after %d previous free(s)\n",
                                      num, heap->cell_size,
                                      debug_cell->filename,
                                      debug_cell->linenum,
                                      debug_cell->reuse_count - 1);
                        }
                    }
                }
            }
        }
    }
}
#endif /* DEBUG */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
void map_heap (HEAPptr heap, void map_fn(void *cell))
{
  /* Applies MAP_FN to all cells in the order they were created.
     without checking whether it has been freed or not. (Be
     careful!) The calling function must cast the cell
     to the appropriate data structure.
     */
  
  size_t block_size = HEAP_block_size(heap), cell_size = HEAP_cell_size(heap);
  BLOCKptr cur_block = HEAP_first(heap), last_block = HEAP_cur(heap);
  long cur_loc = 0, last_loc = HEAP_cur_loc(heap);
  void *cell = BLOCK_cells(cur_block);
  
  if (HEAP_free_list(heap))
    handle_error("Freelist is not empty", "MAP_HEAP", 0);

  while (!(cur_block == last_block && cur_loc == last_loc))
    {
      map_fn(cell);
      cur_loc++;
      
      if (cur_loc == (long)block_size)
        {
          cur_block = BLOCK_next(cur_block);
          cur_loc = 0;
          cell = BLOCK_cells(cur_block);
        }
      else
        cell = (void *)(((char *) cell) + cell_size);
    }
}

#endif /* SLIM_FST */
