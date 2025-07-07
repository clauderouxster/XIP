/* $Id: stack-mr.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

 /******************************************************
 **
 **  STACK-MR.H
 **
 **	 Todd Yampol, Xerox PARC, April 1990
 **
 **  defstructs and function prototypes for STATE and ARC stacks.
 **
 ******************************************************/

#ifndef C_FSM_STACK_MR

#define C_FSM_STACK_MR

#ifdef __cplusplus
extern "C" {
#endif

#define STACK_BLOCK_SIZE 50

//enum STACK_TYPES {STACK_TYPE_STATE = 1, STACK_TYPE_ARC = 2};
 
typedef struct STACK_BLOCK {
  void **block;
  struct STACK_BLOCK *previous;
  struct STACK_BLOCK *next;
} STACK_BLOCKtype, *STACK_BLOCKptr;

#define STACK_BLOCK_previous(X) (X)->previous
#define STACK_BLOCK_next(X)     (X)->next
#define STACK_BLOCK_cell(X, Y)    (X)->block[(Y)]

#define STACK_NAME_MAX_LEN 39

typedef struct STACK {
    char name[STACK_NAME_MAX_LEN + 1];
	long depth;
	long greatest_depth;
	int current_blocks;
	int cur_loc;		/* cur index into cur heap */
	STACK_BLOCKptr top;
	struct STACK *next;
} STACKtype, *STACKptr;

#define STACK_name(X)		(X)->name
#define STACK_depth(X) 		(X)->depth
#define STACK_greatest_depth(X) (X)->greatest_depth
#define STACK_cur_loc(X)  	(X)->cur_loc
#define STACK_top(X)    	(X)->top
#define STACK_current_blocks(X) (X)->current_blocks
#define STACK_cell(X, Y)    (X)->top->block[(Y)]
#define STACK_next(X)		(X)->next

STACKptr FstBaseExport create_stack(const char *name);

int FstBaseExport push (void *obj, STACKptr stack);
void FstBaseExport *pop(STACKptr stack);
void FstBaseExport *peek (STACKptr stack);

void FstBaseExport reset_stack(STACKptr stack);
void FstBaseExport map_stack(STACKptr stack, void map_fn(void *item));
void revmap_stack(STACKptr stack, void map_fn(void *item));

void FstBaseExport free_stack(STACKptr stack);
long FstBaseExport stack_size(STACKptr stack);
long tally_stacks(FILE *stream);

#ifdef __cplusplus
}
#endif

#endif
