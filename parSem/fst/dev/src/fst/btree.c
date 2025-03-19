/* $Id: btree.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

/******************************************************************************
 **
 **  BTREE.C
 **
 **	 Todd Yampol, Xerox PARC, December 1990
 **
 **  Implements a generic binary tree.  Used by fsm_intersect and other 
 **  algorithms that must pair the states from two machines.
 **
 *****************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "fst/heap.h"
#include "fst/error.h"
#include "fst/btree.h"
#include "fst/fsm_io.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#define BTNODE_BLOCK_SIZE 50

HEAPptr BTREE_HEAP = NULL;

void init_btree_mem(void)
{
  if (BTREE_HEAP != NULL)
    handle_error("BTREE_HEAP cannot be reinitialized", "init_btree_mem", 0);

  BTREE_HEAP = init_heap((int) sizeof(BTREEtype), BTNODE_BLOCK_SIZE,
			 "Binary trees");
  
  if (BTREE_HEAP == NULL)
    not_enough_memory("INIT_BTREE_MEM");
}

void free_btree_mem(void)
{
  /*
    static long int max_size = 0;
    long int size = heap_size(BTREE_HEAP, TRUE);

    if (size > max_size)
    {
    max_size = size;
    fsm_fprintf(stdout, "Max size of BTREE_HEAP: %ld\n", max_size);
    }
    */
  if (BTREE_HEAP)
    free_heap(BTREE_HEAP);
  BTREE_HEAP = NULL;
}


HEAPptr make_btree_heap(void)
{
  return(init_heap((int) sizeof(BTREEtype), BTNODE_BLOCK_SIZE,
		   "Binary trees"));
}


BTREEptr bt_insert(void *item, BTREEptr tree, int compare_fn(void *, void *))
     /*****
      *
      * Inserts ITEM into TREE if it is not already present.
      * Returns subtree containing ITEM in its root.
      *
      * COMPAREFN takes two elements, returning 0 if they are the same, 
      * a positive integer if the first is greater, and a negative integer if
      * the second is greater. 
      * 
      *****/
{
  BTREEptr prev = NULL;
  int cmp = 0;
  
  
  while (tree != NULL)
    {
      cmp = compare_fn(item, BT_node(tree));
      
      if (cmp == 0)
        return(tree);

      prev = tree;
      
      if (cmp < 0)
        tree = BT_left(tree);
      else
        tree = BT_right(tree);
    }
  
  tree = btalloc((size_t) sizeof(BTREEtype));
  if (tree == NULL)
    not_enough_memory("BT_INSERT");
  
  if (prev != NULL)
    {
      if (cmp < 0)
	BT_left(prev) = tree;
      else
	BT_right(prev) = tree;
    }
  
  BT_node(tree) = (void *) item;
  BT_value(tree) = (void *) NULL;
  BT_left(tree) = (BTREEptr) NULL;
  BT_right(tree) = (BTREEptr) NULL;
  
  return(tree);
}


BTREEptr btree_insert(void *item, BTREEptr tree, HEAPptr bt_heap,
		      int compare_fn(void *, void *))
     /*****
      *
      * Inserts ITEM into TREE if it is not already present.
      * Returns subtree containing ITEM in its root.
      *
      * COMPAREFN takes two elements, returning 0 if they are the same, 
      * a positive integer if the first is greater, and a negative integer if
      * the second is greater. 
      * 
      *****/
{
  BTREEptr prev = NULL;
  int cmp = 0;
  
  
  while (tree != NULL)
    {
      cmp = compare_fn(item, BT_node(tree));
      
      if (cmp == 0)
        return(tree);

      prev = tree;
      
      if (cmp < 0)
        tree = BT_left(tree);
      else
        tree = BT_right(tree);
    }
  
  tree = cell_alloc(bt_heap);
  if (tree == NULL)
    not_enough_memory("BTREE_INSERT");
  
  if (prev != NULL)
    {
      if (cmp < 0)
	BT_left(prev) = tree;
      else
	BT_right(prev) = tree;
    }
  
  BT_node(tree) = (void *) item;
  BT_value(tree) = (void *) NULL;
  BT_left(tree) = (BTREEptr) NULL;
  BT_right(tree) = (BTREEptr) NULL;
  
  return(tree);
}

#endif /* SLIM_FST */
