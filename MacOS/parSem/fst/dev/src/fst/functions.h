/* Copyright (c) 2000 by Xerox Research Centre Europe.  All rights reserved. */
/* Created by Tamas Gaal: the former iy-define.h got split, here is the 
   function definition related part */
/* $Id: functions.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

char *iy_find_func(char *);
char *iy_func_str(char *, char *);
void iy_save_func(char *, char *, char *);
int FstCalcExport iy_list_funcs(FILE *f);

