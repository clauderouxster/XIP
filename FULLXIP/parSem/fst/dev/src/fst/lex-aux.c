/* Copyright (c) 1996 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Lauri Karttunen */
/* $Id: lex-aux.c,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "fst/types.h"
#include "fst/lex-aux.h"
#include "fst/fsm_io.h"
#include "fst/error.h"
#include "fst/fsm-fst-x.h"
#include "fst/utf8_fatstr_util.h"
#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FSTis defined. */

/****************************************************************
  Functions handle escape characters: % outside double quotes,
  \ in  doubly quoted strings. Conversions from hex and octal
  numbers.
 ****************************************************************/

static unsigned char buf[1024];

unsigned char *remove_percent(unsigned char *str, int strip)
{
  /* static unsigned char buf[1024]; */

  unsigned char *to;
  to = buf;
  if (strip) str++;
  do
    {
      if (*str == '%') str++;
      *to++ = *str;
    } while (*str++);
  if (strip && (to-buf > 2)) to[-2] = '\0';
  return buf;
}

unsigned char *remove_braces(unsigned char *str)
{
  /* static unsigned char buf[1024];  */

  int len = strlen(str);

  if (!(*str == '{' && str[len-1] == '}'))
    fsm_fprintf(stderr,
		"*** Error: Expecting an initial { and final } in %s\n", str);

  if (len > 1024)
    len = 1024;

  strncpy(buf, str+1, len-2);
  buf[len-2] = '\0';
  return buf;
}

static int ctonum(unsigned char c)
{
    if (isdigit(c))
        return((int) c - (int) '0');
    else
        switch (c)
        {
        case 'A':
        case 'a':
            return(10);
            break;
        case 'B':
        case 'b':
            return(11);
            break;
        case 'C':
        case 'c':
            return(12);
            break;
        case 'D':
        case 'd':
            return(13);
            break;
        case 'E':
        case 'e':
            return(14);
            break;
        case 'F':
        case 'f':
            return(15);
            break;
        default:
            fsm_fprintf(stderr, "*** Bad character in atomnum: %c\n", c);
            return(0);
            break;
        }
}

static unsigned char hex2char(unsigned char *str)
{
    return((char) ((ctonum(*str) * 16) + ctonum(*(str+1))));
}


static int isoctdigit(char c)
{
    return(isdigit((int) c) && c != '8' && c != '9');
}

static unsigned char oct2char(unsigned char **str)
{
    int i = 0, k = 1;
    unsigned char *oct, *sym = *str;

    (*str)++;

    /* First find the least significant digit. */

    do {
    (*str)++;
    } while (isoctdigit(**str));

    /* Process the digits from right to left. */

    oct = (*str) - 1;

    do{
    i += k * (*oct-- - '0');
    k = k * 8;
    } while (oct > sym && i < 256);

    /* Print a warning if the octal number is invalid. */

    if (i > 255)
    {
        char c = **str;

        **str = '\0';
        fsm_fprintf(stderr, "*** Error: \"%s\" is too large for an octal ", sym);
        fsm_fprintf(stderr, "number. Max is \"\\377\".\n");
        **str = c;
    }

    return((unsigned char) i);
}

void null_char_error(char *str)
{
  fsm_fprintf(stderr,
	      "*** Error: '%s' contains an illegal NULL character.\n", str);
  quit_on_fail_error(*fst_IY_QUIT_ON_FAIL());
}


unsigned char *decode_quoted(unsigned char *in_str)
{

  /* static unsigned char buf[1024]; */
  unsigned char *to = buf, *str = in_str;

  /* Eliminate initial double quote */

  if (*str != '\"')
    fsm_fprintf(stderr, "*** Error: Expecting an initial \" in %s\n", str);

  str++;
    
  while (*str && *str != '\"')
    {
      if (*str == '\\')
        {
	  if (*(str+1) == 'x' && isxdigit(*(str+2)) && isxdigit(*(str+3)))
            {
	      *to = hex2char(str + 2);
	      str = str + 4;

	      /* The null character '\x00' is not allowed. */

	      if (*to)
		to++;
	      else
		null_char_error(in_str);
            }
	  else if (isoctdigit(*(str+1)))
            {
	      *to = oct2char(&str);

	      /* Then null character '\000' is not allowed. */

	      if (*to)
		to++;
	      else
		null_char_error(in_str);
            }
	  else
            {
	      /* Skip over escape character. */

	      str++;

	      /* Follow Unix conventions. */

	      switch (*str)
                {
                case 'n':
		  *to++ = '\n';
		  break;
                case 't':
		  *to++ = '\t';
		  break;
                case 'v':
		  *to++ = '\v';
		  break;
                case 'b':
		  *to++ = '\b';
		  break;
                case 'r':
		  *to++ = '\r';
		  break;
                case 'f':
		  *to++ = '\f';
		  break;
                case 'a':
		  *to++ = '\a';
		  break;
		  /* Don't need a special case for '\' and '"' */
                default: *to++ = *str;
		  break;
                }
	      str++;
            }
        }
      else
	*to++ = *str++;
    }

  /* Close before final double quote */

  *to = '\0';

  if (*str++ != '\"')
    fsm_fprintf(stderr, "*** Error: Expecting a final \" in \"%s\n", buf );

  /*
    if (to > buf && *str != '\"')
    to[-1] = '\0';
    else
    *to = '\0';
    */
  return(buf);
}


unsigned char *expand_quoted(unsigned char *in_str)
{
  /* Like decode_quoted except that produces a Unicode string
     (= a FAT_STR where the odd-numbered bytes are not
     necessarily zeros.
  */

  /* static unsigned char buf[1024]; */
  unsigned char *to = buf, *str = in_str;
  int utf8_mode = IS_UTF8_MODE;
  char *str_end = str + strlen(str);
  FAT_CHAR fc;
  /* Eliminate initial double quote */

  if (*str != '\"')
    fsm_fprintf(stderr, "*** Error: Expecting an initial \" in %s\n", str);

  str++;
    
  while (*str && *str != '\"')
    {
      if (*str == '\\')
        {
	  if (*(str+1) == 'u' && isxdigit(*(str+2)) && isxdigit(*(str+3))
	      && isxdigit(*(str+4)) && isxdigit(*(str+5)))
            {
	      /* two-byte unicode character */

	      *to++ = hex2char(str + 2);
	      *to = hex2char(str + 4);
	      str = str + 6;

	      /* The null character  '\u0000' is not allowed
		 because it is the Unix string terminator! */

	      if (*(to) || *(to - 1))
		to++;
	      else
		{
		  null_char_error(in_str);
		  to--;
		}
            }
	  else if (*(str+1) == 'x' && isxdigit(*(str+2)) &&
		   isxdigit(*(str+3)))
            {
	      /* one byte hex character */

	      *to++ = '\0';                    /* padding */
	      *to = hex2char(str + 2);
	      str = str + 4;

	      /* The null character '\x00' is not allowed. */

	      if (*to)
		to++;
	      else
		null_char_error(in_str);
            }
	  else if (isoctdigit(*(str+1)))
            {
	      /* one byte octal character */

	      *to++ = '\0';                    /* padding */
	      *to = oct2char(&str);

	      /* The null character '\000' is not allowed. */

	      if (*to)
		to++;
	      else
		null_char_error(in_str);
            }
	  else
            {
	      /* one byte regular character */

	      *to++ = '\0';                    /* padding */

	      /* Follow Unix conventions. */

	      str++;                           /* skip over '\' */
	      switch (*str)
                {
                case 'n':
		  *to++ = '\n';
		  break;
                case 't':
		  *to++ = '\t';
		  break;
                case 'v':
		  *to++ = '\v';
		  break;
                case 'b':
		  *to++ = '\b';
		  break;
                case 'r':
		  *to++ = '\r';
		  break;
                case 'f':
		  *to++ = '\f';
		  break;
                case 'a':
		  *to++ = '\a';
		  break;
		  /* Don't need a special case for '\' and '"' */
                default: *to++ = *str;
		  break;
                }
	      str++;
            }
        }
      else if (utf8_mode)
        {
          if (ConvertUTF8toFAT_CHAR(&str, str_end, &fc, lenientConversion) ==
              conversionOK)
            {
              *to++ = CHARSET(&fc);
              *to++ = CHARNUM(&fc);
            }
          else /* No conversion) */
            {
              *to++ = '\0';
              *to++ = *str++;
            }
        }
      else
        {
	  *to++ = '\0';
	  *to++ = *str++;
        }
    }

  /* Close before final double quote */

  *to++ = '\0';
  *to = '\0';

  if (*str++ != '\"')
    fsm_fprintf(stderr, "*** Error: Expecting a final \" in \"%s\n", buf );

  return(buf);
}

char *last_lex_string()
{
  return (char *) buf;
}

#endif /* SLIM_FST */
