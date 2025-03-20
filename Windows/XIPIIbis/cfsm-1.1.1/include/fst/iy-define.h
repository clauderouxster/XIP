/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: iy-define.h,v 1.8 2003/01/10 08:36:06 jmcoursi Exp $ */


#define STR_TO_LABEL(id_ptr, str) intern_atomic_label(id_ptr, FST_thin_to_temp_fat(str))

extern int too_many_aliases;

void switch_verbose(int restore);
void iy_define(char *, NETptr, int);
void iy_undefine(char *);
void iy_undef_all(void);
void iy_print_defs(FILE *);
NVptr iy_defs_to_stack(void);
int iy_stack_to_defs(NVptr);

void iy_list_aliases(FILE *);
char *iy_find_alias(char *);
void iy_save_alias(char *, char *);
int iy_alias_pars(char *);
void iy_execute_alias(char *, char *pars[], int, int);

void iy_list(id_type id, ALPHABETptr sym_list);
void iy_unlist(id_type id);
void iy_print_lists(FILE *f);
