/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: iy-help.h,v 1.1 2002/05/16 02:06:15 lk Exp $ */

void iy_print_brief(void);
void iy_print_usage(struct command *);
void iy_print_help(struct command *);
int iy_print_apropos(char *, int);
struct command *iy_find_variable(char *name);
int iy_set_variable(char *name, void *value);
