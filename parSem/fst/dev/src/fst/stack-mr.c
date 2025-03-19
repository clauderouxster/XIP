/* $Id: stack-mr.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  stack-mr.c
 **
 **	 Todd Yampol, Xerox PARC, April 1990
 **
 **  Generic stack implementation.
 **
 **
 ******************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "fst/error.h"
#include "fst/stack-mr.h"
#include "fst/fsm_io.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#ifdef __GNUC__
static char rcs_id[] __attribute__ ((unused)) = "$Id: stack-mr.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $";
#endif

#endif /* SLIM_FST */

static STACKptr ALL_STACKS = NULL;


STACKptr create_stack(const char *name)
     /*******
      *
      * Dynamically allocates an unbounded stack.
      *
      *******/
{
  STACKptr stack;
  STACK_BLOCKptr stack_block;
  
  stack = (STACKptr) calloc((size_t) 1, (size_t) sizeof(STACKtype));
  if (stack == NULL)
    not_enough_memory("CREATE_STACK");
  
  stack_block = (STACK_BLOCKptr) calloc((size_t) 1, (size_t)
					sizeof(STACK_BLOCKtype));
  if (stack_block == NULL)
    not_enough_memory("CREATE_STACK");
  
  if (!name)
    name = "Unknown";
  else if (strlen(name) > STACK_NAME_MAX_LEN)
    handle_error("Stack name more than STACK_NAME_MAX_LEN chars", "CREATE_STACK",
      strlen(name));
  else
    strcpy(STACK_name(stack), name);
    
  STACK_current_blocks(stack) = 0;
  STACK_depth(stack) = 0;
  STACK_greatest_depth(stack) = 0;
  STACK_cur_loc(stack) = 0;
  stack_block->block =
    (void **) calloc((size_t)  STACK_BLOCK_SIZE, (size_t) sizeof(void *));
  
  if (stack_block->block == NULL)
    not_enough_memory("CREATE_STACK");
  
  stack_block->previous = (STACK_BLOCKptr) NULL;
  stack_block->next = (STACK_BLOCKptr) NULL;
  STACK_top(stack) = stack_block;
  
  STACK_next(stack) = ALL_STACKS;
  ALL_STACKS = stack;
    
  return (stack);
}

int push (void *obj, STACKptr stack)
     /******
      *
      * Push OBJ onto STACK.
      * Returns 1 if push succeeds, 0 on error.
      *
      *******/
{
  
  STACK_BLOCKptr stack_block;
  void **block;
  
  if (stack == NULL)
    {
      handle_error("can't push onto NULL stack", "push", 0);
      return (0);
    }
  
  STACK_cell(stack, STACK_cur_loc(stack)) = obj;
  STACK_cur_loc(stack)++;
  STACK_depth(stack)++;
  if (STACK_depth(stack) > STACK_greatest_depth(stack))
    STACK_greatest_depth(stack) = STACK_depth(stack);
  
  if (STACK_cur_loc(stack) == STACK_BLOCK_SIZE)  /* at the end of a heap */
    {
      STACK_current_blocks(stack)++;
      STACK_cur_loc(stack) = 0;
      if(STACK_top(stack)->next == (STACK_BLOCKptr) NULL)
	{
	  /* Need to allocate a new stack block */
	  stack_block =
	    (STACK_BLOCKptr) calloc((size_t) 1, (size_t) sizeof(STACK_BLOCKtype));    
	  block = 
	    (void **) calloc((size_t) STACK_BLOCK_SIZE,
			     (size_t) sizeof(void *));
	  
	  if (!block || !stack_block)
	    {
	      handle_error("out of memory, could not allocate new heap",
			   "push", STACK_current_blocks(stack));
	      return (0);
	    }
	  stack_block->block = block;
          /* put into chain */
	  stack_block->previous = STACK_top(stack);
	  STACK_top(stack)->next = stack_block;
	}
      else
	stack_block = STACK_top(stack)->next;
      
      STACK_top(stack) = stack_block;
    }
  return (1);	
}

void *pop (STACKptr stack)
     /*******
      *
      * Pops the top item off the stack.
      * Returns the item if successful, NULL otherwise.
      * Leaves the top item in place on the stack block,
      * just moves the pointer below it. This is important
      * for some routines that want to restore the stack
      * after emptying it. LK. 10.12.94
      *
      *******/
{
  void *cell;
  
  if (stack == NULL ||
      (STACK_cur_loc(stack) == 0 && STACK_current_blocks(stack) == 0))
    {
      return (NULL);
    }
  
  STACK_cur_loc(stack)--;
  if (STACK_cur_loc(stack) == -1) /* we need to wrap backwards here */
    {
      STACK_cur_loc(stack) = STACK_BLOCK_SIZE - 1;
      STACK_top(stack) = STACK_top(stack)->previous;
      STACK_current_blocks(stack)--;
    }
  
  cell = STACK_cell(stack, STACK_cur_loc(stack));
/*  STACK_cell(stack, STACK_cur_loc(stack)) = NULL; */
  STACK_depth(stack)--;
  
  return (cell);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void map_stack(STACKptr stack, void map_fn(void *item))
{
  /* Applies MAP_FN to all items in the stack */

  int loc, lim;
  STACK_BLOCKptr block;
  void **cells;

  if (stack == NULL ||
      (STACK_cur_loc(stack) == 0 && STACK_current_blocks(stack) == 0))
    return;
 
  block = STACK_top(stack);

  while(STACK_BLOCK_previous(block))
    block = STACK_BLOCK_previous(block);
  
  while (block)
    {
      loc = 0;

      if (block == STACK_top(stack))
	lim = STACK_cur_loc(stack);
      else
	lim = STACK_BLOCK_SIZE;

      cells = block->block;
      while (loc < lim)
	{
	  map_fn(cells[loc]);
	  loc++;
	}

      block = STACK_BLOCK_next(block);
    }
}

void revmap_stack(STACKptr stack, void map_fn(void *item))
{
  /* Applies MAP_FN to all items in the stack */

  int loc;
  STACK_BLOCKptr block;
  void **cells;

  if (stack == NULL ||
      (STACK_cur_loc(stack) == 0 && STACK_current_blocks(stack) == 0))
    return;
 
  loc = STACK_cur_loc(stack) - 1;
  block = STACK_top(stack);
  
  while (block)
    {
      cells = block->block;
      while (loc >= 0)
	{
	  map_fn(cells[loc]);
	  loc--;
	}

      block = STACK_BLOCK_previous(block);
      loc = STACK_BLOCK_SIZE -1;
    }
}
#endif /* SLIM_FST */

void *peek (STACKptr stack)
     /*******
      *
      * Returns the last item on the stack without removing it.
      *
      *******/
{
  int loc;
  STACK_BLOCKptr block;
  
  if (stack == NULL ||
      (STACK_cur_loc(stack) == 0 && STACK_current_blocks(stack) == 0))
      return (NULL);
  
  if ((loc = STACK_cur_loc(stack) - 1) < 0)
    {
      /* back to the previous block */
      loc = STACK_BLOCK_SIZE - 1;
      block = STACK_top(stack)->previous;
    }
  else
    block = STACK_top(stack);
  
  return(STACK_BLOCK_cell(block, loc));
}


void free_stack(STACKptr stack)
     /*****
      *
      * Relases all memory associated with STACK.
      *
      *****/
{
  STACK_BLOCKptr stack_block;
  STACKptr s, prev;
  
  while(STACK_top(stack)->next != (STACK_BLOCKptr) NULL)
    STACK_top(stack) = STACK_top(stack)->next;
  
  while(STACK_top(stack) != (STACK_BLOCKptr) NULL)
    {
      stack_block = STACK_top(stack);
      STACK_top(stack) = stack_block->previous;
      free((void *) stack_block->block);
      free((void *) stack_block);
    }
  
  for (s = ALL_STACKS, prev = NULL; s; s = STACK_next(s))
    {
      if (s == stack)
        {
          if (prev)
            STACK_next(prev) = STACK_next(stack);
          else
            ALL_STACKS = STACK_next(stack);
            
          break;
        }
      else
        prev = s;
    
    }
    
  free((void *) stack);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void reset_stack(STACKptr stack)
{
  while(STACK_top(stack)->previous != (STACK_BLOCKptr) NULL)
    STACK_top(stack) = STACK_top(stack)->previous;
    
  STACK_depth(stack) = 0;
  STACK_cur_loc(stack) = 0;
}
#endif /* SLIM_FST */

long stack_size(STACKptr stack)
     /*********
      *
      * Returns the amount of memory associated with STACK.
      *
      *********/
{
  return ((long) sizeof(stack) + 
	  (1 + (long) STACK_current_blocks(stack)) *
	  (sizeof(STACK_BLOCKtype) + (STACK_BLOCK_SIZE * sizeof(void **))));
}


static void tally_stack(STACKptr stack, long *tally, FILE *stream)
{
  if (stack != NULL)
    {
      long size = stack_size(stack);
      int indent = STACK_NAME_MAX_LEN - strlen(STACK_name(stack));

      *tally += size;

      while (indent-- > 0)
	fputc(' ', stream);
	    
      fsm_fprintf(stream, "%s: %5ld max depth, %5ld depth, %5ld bytes (%ldK)\n",
	      STACK_name(stack), STACK_greatest_depth(stack),
	      STACK_depth(stack), size, size/1024);
    }
}

long tally_stacks(FILE *stream)
{
  STACKptr stack;
  int i, indent = STACK_NAME_MAX_LEN;
  long tally = 0;
  
  if (!ALL_STACKS)
    return(tally);
    
  for (stack = ALL_STACKS; stack; stack = STACK_next(stack))
    if ((i = STACK_NAME_MAX_LEN - strlen(STACK_name(stack))) < indent)
      indent = i;
    
  for (i = 0; i < indent; i++)
    fputc(' ', stream);
  fsm_fprintf(stream, "* Stacks *\n");
  
  for (stack = ALL_STACKS; stack; stack = STACK_next(stack))
    tally_stack(stack, &tally, stream);
  
  for (i = 0; i < indent; i++)
    fputc(' ', stream);
	   
  fsm_fprintf(stream, "================================");
  fsm_fprintf(stream, "================================\n");
  
  for (i = 0; i < indent; i++)
    fputc(' ', stream);
	
  fsm_fprintf(stream, "Total: %ld Kbytes.\n", tally/1024);
  return(tally);
}

