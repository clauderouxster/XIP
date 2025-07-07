/* Copyright (c) 2000 by Xerox Research Centre Europe.  All rights reserved. */
/* Created by Tamas Gaal: the former iy-define.c got split, here is the 
   function definition related part */
/* $Id: functions.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

#include <stdlib.h>
#include <string.h>

#include "fst/fsm-package.h"
#include "fst/functions.h"
#include "fst/context.h"
#include "fst/fsm_io.h"

//#if (defined(MAC_OS) || defined(_WIN32)) && !defined(__GNUC__) && !defined(UNDER_CE)
#if (defined(MAC_OS))
#include <cstring>
char *	strdup(const char *str);
#endif

/****************************************************************
  COMMAND: function in regex
 ****************************************************************/
#define FUNC_SEP (-1)
static HASH_TABLEptr fun;
static FILE *func_f;
static int func_prp;

static unsigned int al_hashfn(unsigned char *str)
{
  int i=0;
  while (*str) i=13*i + *str++;
  return i;
}

static void init_funcs()
{
  fun=make_hash_table(61,"Functions");
}

static int func_pr(char *el, char *val)
{
  char *p, *e;
  int len;
#ifdef __GNUC__
  char buf[strlen(val)+4];
#else
  char *buf = (char *)malloc(sizeof(buf[0]) * (strlen(val)+4));
#endif
  func_prp=1;
  fsm_fprintf(func_f, "%s)\t",el);
  strcpy(p=buf,val);
  while ((e = strchr(p, FUNC_SEP)))
    {
      *e = 0;
      fputs(p, func_f);
      fputs("$1",func_f);
      p = e+1;
    }
  len = fsm_fprintf(func_f,"%s\n",p);
#ifndef __GNUC__
  free(buf);
#endif
  return len;
}

char *iy_find_func(char *a)
{
  void *p;
  if (fun && !get_hash(fun,a,&p, (uint32 (*)(void *)) al_hashfn, (int (*)(void *, void *)) strcmp))
    return p;
  else
    return NULL;
}


static int fun_space_needed(char *str, int add)
{
  int i=0;
  char *p;
  p=str;
  while (*p) if (*p++ == FUNC_SEP) ++i;
  return add*i + p-str;
}
char *iy_func_str(char *p, char *par)
{
  if (p)
    {
      int len = strlen(par);
#ifdef __GNUC__
      char buf[fun_space_needed(p,len)+10];
#else
      char *buf = (char *)malloc(sizeof(buf[0]) * (fun_space_needed(p,len)+10));
#endif
      char *new_buf, *bp;
      bp=buf;
      while (*p)
	{
	  if (*p == FUNC_SEP)
	    {
	      strcpy(bp,par);
	      bp += len; p++;
	    }
	  else *bp++ = *p++;
	}
      *bp = '\0';
      new_buf = strdup(buf);
#ifndef __GNUC__
      free(buf);
#endif
      return new_buf;
    }
  else
    return NULL;
}

void iy_save_func(char *a, char *var, char *str)
{
  int i, lvar=strlen(var);
  char *bp, *p;
#ifdef __GNUC__
  char buf[strlen(str)+1000];
#else
  char *buf = (char *)malloc(sizeof(buf[0]) * (strlen(str)+1000));
#endif

  bp=buf;
  while((p=strstr(str,var)))
    {
      strncpy(bp,str,i=p-str);
      bp += i;
      str = p + lvar;
      *bp++ = FUNC_SEP;
    }
  strcpy(bp,str);

  if (str)
    {
      if (!fun) init_funcs();
      if ((p = iy_find_func(a)))
	{
	  free(p);
	  put_hash(fun,a,strdup(buf), (uint32 (*)(void *)) al_hashfn, (int (*)(void *, void *)) strcmp);
	}
      else
	{
	  put_hash(fun,strdup(a),strdup(buf), (uint32 (*)(void *)) al_hashfn, (int (*)(void *, void *)) strcmp);
	}
    }
  else if (fun && (p = iy_find_func(a)))
    {
      free(p);
      del_hash(fun,a, (uint32 (*)(void *)) al_hashfn, (int (*)(void *, void *)) strcmp); /* Note: everything is not freed */
    }
#ifndef __GNUC__
  free(buf);
#endif
}

int iy_list_funcs(FILE *f)
{
  func_prp=0; func_f = f;
  if (fun) map_hash(fun, (int (*)(void *, void *)) func_pr);
  return func_prp;
}
