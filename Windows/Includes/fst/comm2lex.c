/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: comm2lex.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define INFO(x,y,z) x, y, z
#include "fst/types.h"
#include "fst/fst-types.h"
#include "fst/interface.tab.h"
#include "fst/comm-def.h"

static char *up(char *p)
{
  static char buf[80];
  char *to;
  to=buf;
  while (*p)
    if (!isalpha(*p))
      {
	*to++='_';
	p++;
      }
    else
      *to++=toupper(*p++);
  *to='\0';

  if (!strcmp(buf,"ECHO"))
    strcat(buf,"_");

  return buf;
}

#define special(c)	((!c) || ((c)=='(') || ((c)==')') || ((c)=='|') || ((c)==' ') || ((c) == '?'))

static char *print_rest(char *p)
{
  if (!special(*p))
    {
      printf("(\"%c\"",*p++);
      p=print_rest(p);
      printf(")?");
    }
  return p;
}

#define END_QUOT	if ( quot) { quot=0; printf("\""); }
#define START_QUOT	if (!quot) { quot=1; printf("\""); }


int main()
{
  struct command *c;
  char *p;
  int quot;

  c=commands;
  while (c->command)
    {
      quot=0;
      if (!strncmp(c->command,"variable",8))
	printf("<VARIABLE>");
      else if (!strncmp(c->command,"regex:",6))
	printf("<REG_HELP>");
      else
	printf("<COMMAND>");
      printf("(");
      for (p=c->coding; *p; p++)
	{
	  switch (*p)
	    {
	    case '$':
	      END_QUOT;
	      p=print_rest(p+1);
	      p--;
	      break;
	    case '(':
	      END_QUOT;
	      printf("(");
	      break;
	    case ')':
	      END_QUOT;
	      printf(")");
	      break;
	    case '?':
	      printf("?");
	      break;
	    case '|':
	      END_QUOT;
	      printf(")|(");
	      break;
	    case ' ':
	      END_QUOT;
	      printf("[ \\t]+");
	      break;
	    default:
	      START_QUOT;
	      printf("%c",*p);
	    }
	}
      END_QUOT;
      printf(")");

      printf("/{commend}\t{ %s=&commands[%d]; BEGIN(%s); return(%s); }\n",
	     (strncmp(c->command,"variable",8) ? "last_command" : "last_variable"),
	     c-commands, c->state, up(c->command));
      c++;
    }

  return 0;
}
