/* $Id: fsm_io.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1994 by the Xerox Corporation.  All rights reserved */

/*********************************************************************
 *                                                                   *
 *  OUTPUT.H
 *                                                                   *
 *********************************************************************/

#ifndef _FsmOutput_h
#define _FsmOutput_h

#include "fst/fst_windefs.h"

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
int FstBaseExport fsm_printf(const char *, ...);

typedef int (*FPRINTF_FUNC)(FILE *, const char *, va_list);
FstBaseExport FPRINTF_FUNC set_fsm_fprintf(FPRINTF_FUNC f);
int FstBaseExport fsm_fprintf(FILE *, const char *, ...);
int FstBaseExport fprintf_fflush (FILE *stream, const char *format, va_list ap);

typedef int (*PUTS_FUNC)(const char *);
PUTS_FUNC set_fsm_puts(PUTS_FUNC f);
int FstBaseExport fsm_puts(const char *);

typedef int (*FPUTS_FUNC)(const char *, FILE *);
FPUTS_FUNC set_fsm_fputs(FPUTS_FUNC f);
int FstBaseExport fsm_fputs(const char *, FILE *);

typedef int (*PUTC_FUNC)(int, FILE *);
PUTC_FUNC set_fsm_putc(PUTC_FUNC f);
int FstBaseExport fsm_putc(int, FILE *);

typedef int (*PUTCHAR_FUNC)(int);
PUTCHAR_FUNC set_fsm_putchar(PUTCHAR_FUNC f);
int FstBaseExport fsm_putchar(int);

/* input functions */

char FstBaseExport * _fsm_fgets(char * format, int len,  FILE * file);

typedef char * (*GETS_FUNC)(char *);
GETS_FUNC set_fsm_gets(GETS_FUNC f);
char FstBaseExport * fsm_gets(char *);

typedef char * (*FGETS_FUNC)(char *, int, FILE *);
FGETS_FUNC set_fsm_fgets(FGETS_FUNC f);
char FstBaseExport * fsm_fgets(char *, int, FILE *);

typedef int (*GETC_FUNC)(FILE *);
GETC_FUNC set_fsm_getc(GETC_FUNC f);
int FstBaseExport fsm_getc(FILE *);

typedef int (*FGETC_FUNC)(FILE *);
FGETC_FUNC set_fsm_fgetc(FGETC_FUNC f);
int FstBaseExport fsm_fgetc(FILE *);

typedef int (*GETCHAR_FUNC)();
GETCHAR_FUNC set_fsm_getchar(GETCHAR_FUNC f);
int FstBaseExport fsm_getchar();


#ifdef __cplusplus
}
#endif

#endif /* _FsmOutput_h */

