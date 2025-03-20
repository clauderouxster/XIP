/* $Id: fsm_io.h,v 1.8 2002/12/20 10:18:43 grayrat Exp $ */
/* Copyright (c) 1994 by the Xerox Corporation.  All rights reserved */

/*********************************************************************
 *                                                                   *
 *  OUTPUT.H
 *                                                                   *
 *********************************************************************/

#ifndef _FsmOutput_h
#define _FsmOutput_h

#include "fst_windefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#if defined  __STDC__ || defined _WINDOWS
#include <stdarg.h>
#else /* !__STDC__ */
#include <varargs.h>
#endif /* __STDC__ */


/* output functions */

typedef int (*PRINTF_FUNC)(const char *, va_list);
PRINTF_FUNC set_fsm_printf(PRINTF_FUNC f);
int fsm_printf(const char *, ...);

typedef int (*FPRINTF_FUNC)(FILE *, const char *, va_list);
FsmFstExport FPRINTF_FUNC set_fsm_fprintf(FPRINTF_FUNC f);
int FsmFstExport fsm_fprintf(FILE *, const char *, ...);
FsmFstExport int fprintf_fflush (FILE *stream, const char *format, va_list ap);

typedef int (*PUTS_FUNC)(const char *);
PUTS_FUNC set_fsm_puts(PUTS_FUNC f);
int fsm_puts(const char *);

typedef int (*FPUTS_FUNC)(const char *, FILE *);
FPUTS_FUNC set_fsm_fputs(FPUTS_FUNC f);
int fsm_fputs(const char *, FILE *);

typedef int (*PUTC_FUNC)(int, FILE *);
PUTC_FUNC set_fsm_putc(PUTC_FUNC f);
int fsm_putc(int, FILE *);

typedef int (*PUTCHAR_FUNC)(int);
PUTCHAR_FUNC set_fsm_putchar(PUTCHAR_FUNC f);
int fsm_putchar(int);

/* input functions */

char * _fsm_fgets(char * format, int len,  FILE * file);

typedef char * (*GETS_FUNC)(char *);
GETS_FUNC set_fsm_gets(GETS_FUNC f);
char * fsm_gets(char *);

typedef char * (*FGETS_FUNC)(char *, int, FILE *);
FGETS_FUNC set_fsm_fgets(FGETS_FUNC f);
char * fsm_fgets(char *, int, FILE *);

typedef int (*GETC_FUNC)(FILE *);
GETC_FUNC set_fsm_getc(GETC_FUNC f);
int fsm_getc(FILE *);

typedef int (*FGETC_FUNC)(FILE *);
FGETC_FUNC set_fsm_fgetc(FGETC_FUNC f);
int fsm_fgetc(FILE *);

typedef int (*GETCHAR_FUNC)();
GETCHAR_FUNC set_fsm_getchar(GETCHAR_FUNC f);
int fsm_getchar();


#ifdef __cplusplus
}
#endif

#endif /* _FsmOutput_h */

