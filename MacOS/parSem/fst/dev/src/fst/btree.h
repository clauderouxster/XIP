/* $Id: btree.h,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

/******************************************************************************
 **
 **  BTREE.H
 **
 **	 Todd Yampol, Xerox PARC, June 1990
 **
 **  Type declarations and function prototype for binary tree type.
 **
 *****************************************************************************/

#ifndef C_FSM_BTREE

#define C_FSM_BTREE

#include "fst/heap.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BTREE {
  void *node;
  struct BTREE *left;
  struct BTREE *right;
  void *value;
  /* For use by clients.  Set to NULL when node is created. */
} BTREEtype, *BTREEptr;

#define BT_node(X)		(X)->node
#define BT_left(X)		(X)->left
#define BT_right(X)		(X)->right
#define BT_value(X)		(X)->value

extern HEAPptr BTREE_HEAP;
  
  /* X is a size arg provided to shadow malloc -- it is ignored here */
#define btalloc(X)  (BTREEptr) cell_alloc(BTREE_HEAP)

void FstBaseExport init_btree_mem(void);
HEAPptr FstBaseExport make_btree_heap(void);

void FstBaseExport free_btree_mem(void);
BTREEptr FstBaseExport bt_insert(void *new_node, BTREEptr tree, int compare_fn(void*, void*)); /* YM, 2/9/96 */
BTREEptr FstBaseExport bt_store(void *new_node, BTREEptr tree, int compare_fn(void*, void*));

BTREEptr FstBaseExport btree_insert(void *item, BTREEptr tree, HEAPptr bt_heap, int compare_fn(void *, void *));

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_BTREE */
