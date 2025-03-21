/* $Id: fsm_io.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

#include "fst/fsm_io.h"

#if defined(_WIN32) || defined(__STDC__)
#include <stdarg.h>
#else /* !__STDC__ */
#include <varargs.h>
#endif /* __STDC__ */

#ifdef SUNOS
/* on SunOS we don't have declaration of *printf* functions */
#if defined(__STDC__)
extern int      vfprintf(FILE *, const char *, va_list);
extern int      vprintf(const char *, va_list);
extern int	fputs(const char *, FILE *);
extern int	puts(const char *);
extern char	*fgets(char *, int, FILE *);
/* extern char	*gets(char *); */
extern int	fgetc(FILE *);
#else
extern int      vfprintf();
extern int      vprintf();
extern int	fputs();
extern int	puts();
extern char	*fgets();
/* extern char	*gets(); */
extern int	fgetc();
#endif


/* getc, getchar, putc and putchar are defined as macros in 
   SunOS => they have to  be wrapped in order to call them 
   as functions */

static int _fsm_getc(FILE* stream)
{
	return getc(stream);
}

static int _fsm_getchar()
{
	return getchar();
}

static int _fsm_putc(int c, FILE* stream)
{
	return putc(c, stream);
}


static int _fsm_putchar(int c)
{
	return putchar(c);
}

#endif


int _fsm_vprintf(const char *format, va_list args)
{
    return vprintf(format, args);
}


int _fsm_vfprintf(FILE * file, const char * format, va_list args)
{
    return vfprintf(file, format, args);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int _fsm_puts(const char * format)
{
    return puts(format);
}
#endif /* SLIM_FST */

int _fsm_fputs(const char * format, FILE * file)
{
    return fputs(format, file);
}


int _fsm_putc(int format, FILE * file)
{
    return putc(format, file);
}

int _fsm_putchar(int format)
{
    return putchar(format);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#define MAX_FSM_IO_GETS_ANSWER_LENGTH 1024

char * _fsm_gets(char * format)
{
  return fgets(format, MAX_FSM_IO_GETS_ANSWER_LENGTH, stdin);
}

char * _fsm_fgets(char * format, int len,  FILE * file)
{
    return fgets(format, len, file);
}
#endif /* SLIM_FST */

int _fsm_fgetc(FILE * file)
{
    return fgetc(file);
}


int _fsm_getc(FILE * file)
{
    return getc(file);
}

int _fsm_getchar()
{
    return getchar();
}

/* default output functions */
static FPRINTF_FUNC pf_fprintf = _fsm_vfprintf;
static PRINTF_FUNC pf_printf = _fsm_vprintf;

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

static PUTS_FUNC pf_puts = _fsm_puts;
#endif /* SLIM_FST */

static FPUTS_FUNC pf_fputs = _fsm_fputs;

#ifdef SUNOS
static PUTC_FUNC pf_putc = _fsm_putc;
static PUTCHAR_FUNC pf_putchar = _fsm_putchar;
#else
static PUTC_FUNC pf_putc = _fsm_putc;
static PUTCHAR_FUNC pf_putchar = _fsm_putchar;
#endif
  
#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/* default input functions */
static GETS_FUNC pf_gets = _fsm_gets;
static FGETS_FUNC pf_fgets = _fsm_fgets;
#endif /* SLIM_FST */

static FGETC_FUNC pf_fgetc = _fsm_fgetc;


#ifdef SUNOS
static GETC_FUNC pf_getc = _fsm_getc;
static GETCHAR_FUNC pf_getchar = _fsm_getchar;
#else
static GETC_FUNC pf_getc = _fsm_getc;
static GETCHAR_FUNC pf_getchar = _fsm_getchar;
#endif


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/************************************************************************************
*	printf substitution
*************************************************************************************/
						   
PRINTF_FUNC set_fsm_printf(PRINTF_FUNC f)
{
	PRINTF_FUNC old = pf_printf;

	pf_printf = f;

	return old;
}
#endif /* SLIM_FST */

int fsm_printf(const char * format, ...)
{
	if (pf_printf)
	{
		int ret;

		va_list ap;					 

#if defined(_WIN32) || defined(__STDC__)
                va_start(ap, format);
#else /* ! __STDC__ */
                va_start(ap);
                                           /* extract the fixed argument */
                format = va_arg(ap, char *);
#endif /* __STDC__ */

		ret = (*pf_printf)(format, ap);

		va_end(ap);

		return ret;
	}
	else
		return 0;
}


#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/************************************************************************************
*	fprintf substitution
*************************************************************************************/

FPRINTF_FUNC set_fsm_fprintf(FPRINTF_FUNC f)
{
	FPRINTF_FUNC old = pf_fprintf;

	pf_fprintf = f;

	return old;
}
#endif /* SLIM_FST */

int fsm_fprintf(FILE *stream, const char * format, ...)
{
	if (pf_fprintf)
	{
		int ret;

		va_list ap;					 

#if defined(_WIN32) || defined(__STDC__)
                va_start(ap, format);
#else /* ! __STDC__ */
                va_start(ap);
				/* extract the fixed argument */
                format = va_arg(ap, char *);
#endif /* __STDC__ */

		ret = (*pf_fprintf)(stream, format, ap);

		va_end(ap);

		return ret;
	}
	else
		return 0;
} /* fsm_fprintf */

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

int fprintf_fflush (FILE *stream, const char *format, va_list ap)
{
  /* Nov 1998 AK */

  int ret;

  ret = vfprintf (stream, format, ap);
  fflush (stream);

  return (ret);
}


/************************************************************************************
*	puts substitution
*************************************************************************************/

PUTS_FUNC set_fsm_puts(PUTS_FUNC f)
{
	PUTS_FUNC old = pf_puts;

	pf_puts = f;

	return old;
}

int fsm_puts(const char * s)
{
	if (pf_puts)
	{
		return (*pf_puts)(s);
	}
	else
		return 1;
}


/************************************************************************************
*	fputs substitution
*************************************************************************************/

FPUTS_FUNC set_fsm_fputs(FPUTS_FUNC f)
{
	FPUTS_FUNC old = pf_fputs;

	pf_fputs = f;

	return old;
}
#endif /* SLIM_FST */

int fsm_fputs(const char * s, FILE *stream)
{
	if (pf_fputs)
	{
		return (*pf_fputs)(s, stream);
	}
	else
		return 1;
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/************************************************************************************
*	putc substitution
*************************************************************************************/

PUTC_FUNC set_fsm_putc(PUTC_FUNC f)
{
	PUTC_FUNC old = pf_putc;

	pf_putc = f;

	return old;
}
#endif /* SLIM_FST */

int fsm_putc(int c, FILE *stream)
{
	if (pf_putc)
	{
		return (*pf_putc)(c, stream);
	}
	else
		return c;
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/************************************************************************************
*	putchar substitution
*************************************************************************************/

PUTCHAR_FUNC set_fsm_putchar(PUTCHAR_FUNC f)
{
	PUTCHAR_FUNC old = pf_putchar;

	pf_putchar = f;

	return old;
}

int fsm_putchar(int c)
{
	if (pf_putchar)
	{
		return (*pf_putchar)(c);
	}
	else
		return c;
}


/************************************************************************************
*	gets substitution
*************************************************************************************/

GETS_FUNC set_fsm_gets(GETS_FUNC f)
{
	GETS_FUNC old = pf_gets;

	pf_gets = f;

	return old;
}

char * fsm_gets(char * buffer)
{
	if (pf_gets)
	{
		return (*pf_gets)(buffer);
	}
	else
		return NULL;
}


/************************************************************************************
*	fgets substitution
*************************************************************************************/

FGETS_FUNC set_fsm_fgets(FGETS_FUNC f)
{
	FGETS_FUNC old = pf_fgets;

	pf_fgets = f;

	return old;
}

char * fsm_fgets(char * buffer, int n, FILE * stream)
{
	if (pf_fgets)
	{
		return (*pf_fgets)(buffer, n, stream);
	}
	else
		return NULL;
}


/************************************************************************************
*	getc substitution
*************************************************************************************/

GETC_FUNC set_fsm_getc(GETC_FUNC f)
{
	GETC_FUNC old = pf_getc;

	pf_getc = f;

	return old;
}

int fsm_getc(FILE * stream)
{
	if (pf_getc)
	{
		return (*pf_getc)(stream);
	}
	else
		return EOF;
}


/************************************************************************************
*	fgetc substitution
*************************************************************************************/

FGETC_FUNC set_fsm_fgetc(FGETC_FUNC f)
{
	FGETC_FUNC old = pf_fgetc;

	pf_fgetc = f;

	return old;
}
#endif /* SLIM_FST */

int fsm_fgetc(FILE * stream)
{
	if (pf_fgetc)
	{
		return (*pf_fgetc)(stream);
	}
	else
		return EOF;
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/************************************************************************************
*	getchar substitution
*************************************************************************************/

GETCHAR_FUNC set_fsm_getchar(GETCHAR_FUNC f)
{
	GETCHAR_FUNC old = pf_getchar;

	pf_getchar = f;

	return old;
}
#endif /* SLIM_FST */

int fsm_getchar()
{
	if (pf_getchar)
	{
		return (*pf_getchar)();
	}
	else
		return EOF;
}
