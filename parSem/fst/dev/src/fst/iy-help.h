/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: iy-help.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

void iy_print_brief(void);
void iy_print_usage(struct command *);
void iy_print_help(struct command *);
int iy_print_apropos(char *, int);
struct command *iy_find_variable(char *name);
int iy_set_variable(char *name, void *value);
