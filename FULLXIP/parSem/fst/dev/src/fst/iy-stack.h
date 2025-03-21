/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: iy-stack.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */

enum stacks { WORK_STACK, LEXICON_STACK, TOKEN_STACK,
	      RULE_STACK, PRULE_STACK, HRULE_STACK, SENTENCE_STACK, USER_STACKS };
enum which_stack{ USE_CURRENT_STACK, CREATE_NEW_STACK };

void print_stack_prompt(char *);  /* Probably in a wrong place... */

NVptr iy_stack(int);
NVptr * iy_stackptr(int n); /* TG */
void iy_set_stack(int n, NVptr newstack); /* TG */
void iy_net_to_stack(NETptr);
void iy_nv_to_stack(NVptr nv);
NETptr iy_net_from_a_stack(int, int);
int iy_stack_length(void);
NVptr iy_get_stack(void);

char *iy_current_stack_name(void);
unsigned int iy_find_stack(char *);
int iy_change_stack(unsigned int, FILE *);

int iy_check_args(int, char*);
int iy_can_do_on(int, char *, int);

void iy_print_eqv_labels (FILE *);

void iy_print_stacks(FILE *);
void iy_print_stack(FILE *, int);

NVptr FstCalcExport iy_stack_load(char *, int);
NVptr iy_stack_load_from_stream(FILE *stream, int method);
void iy_stack_save(char *, NVptr);
void iy_stack_write(void write_fn(NVptr nv, FILE *stream, int sort_states_and_arcs), FILE *stream, int sort_states_and_arcs);
void iy_stack_drop(int);
void iy_stack_dup(int);
void iy_stack_totop(int);
void iy_stack_swap(int, int);
NETptr iy_stack_pop(void);
NETptr iy_stack_push_defined(char *);
NETptr iy_top_stack(int);
void iy_stack_clear(void);
void iy_stack_reverse(void);
void iy_stack_rotate(void);

#ifndef max
#define max(a, b)       ((a) < (b) ? (b) : (a))
#endif
