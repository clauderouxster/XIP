/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: comm-def.h,v 1.4 2002/04/03 01:57:49 lk Exp $ */

#ifndef COMM_DEF_H
#define COMM_DEF_H
#define INFO(x,y,z) x, y, z

struct command
{
  char *command;
#ifdef INFO
  char *coding;
  char *state;
#endif
  int class;

  char *usage;
  char *brief;
  char *help;
  int *value;  /* Only meaningful for variables.*/
};

enum {COMM1, COM_S_IO, COM_D, COM_T, COM_NET, VIRT_NET};

extern struct command commands[];
extern struct command *last_command, *last_variable;

extern char * help_header;
extern char *  CommandClasses[];

#endif
