/* Copyright (c) 1996-1999 by Xerox Research Centre Europe.  All rights reserved. */
/* Created by Tamas Gaal (copied from lex-aux.c of Lauri Karttunen). */
/* $Id: interface-aux.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "fst/interface-aux.h"


/****************************************************************
  Functions handle escape characters: % outside double quotes,
  \ in  doubly quoted strings. Conversions from hex and octal
  numbers.
 ****************************************************************/

static unsigned char buf[1024];

unsigned char *interface_remove_percent(unsigned char *str, int strip)
{
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
            fprintf(stderr, "*** Bad character in atomnum: %c\n", c);
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
    char *oct, *sym =  (char *) *str;

    (*str)++;

    /* First find the least significant digit. */

    do {
    (*str)++;
    } while (isoctdigit(**str));

    /* Process the digits from right to left. */

    oct = (char *)(*str) - 1;

    do{
    i += k * (*oct-- - '0');
    k = k * 8;
    } while (oct > sym && i < 256);

    /* Print a warning if the octal number is invalid. */

    if (i > 255)
    {
        char c = **str;

        **str = '\0';
        fprintf(stderr, "*** Error: \"%s\" is too large for an octal ", sym);
        fprintf(stderr, "number. Max is \"\\377\".\n");
        **str = c;
    }

    return((unsigned char) i);
}


unsigned char *interface_decode_quoted(unsigned char *str)
{
    unsigned char *to;
    to = buf;

    /* Eliminate initial double quote */

    if (*str != '\"')
      fprintf(stderr, "*** Error: Expecting an initial \" in %s\n", str);

    str++;
    
    while (*str && *str != '\"')
    {
        if (*str == '\\')
        {
            if (*(str+1) == 'x' && isxdigit(*(str+2)) && isxdigit(*(str+3)))
            {
                *to = hex2char(str + 2);
                str = str + 4;

                /* Treat '\x00' as an epsilon, not a Unix string terminator! */

                if (*to)
                    to++;
            }
            else if (isoctdigit(*(str+1)))
            {
                *to = oct2char(&str);

                /* Treat '\000' as an epsilon, not a Unix string terminator! */

                if (*to)
                    to++;
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
      fprintf(stderr, "*** Error: Expecting a final \" in \"%s\n", buf );

/*
    if (to > buf && *str != '\"')
        to[-1] = '\0';
    else
        *to = '\0';
*/
    return(buf);
}


unsigned char *interface_expand_quoted(unsigned char *str)
{
    /* Like decoce quoted except that produces a Unicode string
       (= a FAT_STR where the odd-numbered bytes are not
          necessarily zeros.
          */


    unsigned char *to;
    to = buf;

    /* Eliminate initial double quote */

    if (*str != '\"')
      fprintf(stderr, "*** Error: Expecting an initial \" in %s\n", str);

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

                /* Treat '\x00' as an epsilon,
                   not as a Unix string terminator! */

                if (*(to) || *(to - 1))
                    to++;
                else
                    to--;
            }
            else if (*(str+1) == 'x' && isxdigit(*(str+2)) &&
                     isxdigit(*(str+3)))
            {
                /* one byte hex character */

                *to++ = '\0';                    /* padding */
                *to = hex2char(str + 2);
                str = str + 4;

                /* Treat '\x00' as an epsilon,
                   not as a Unix string terminator! */

                if (*to)
                    to++;
            }
            else if (isoctdigit(*(str+1)))
            {
                /* one byte octal character */

                *to++ = '\0';                    /* padding */
                *to = oct2char(&str);

                /* Treat '\000' as an epsilon,
                   not a Unix string terminator! */

                if (*to)
                    to++;
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
      fprintf(stderr, "*** Error: Expecting a final \" in \"%s\n", buf );

    return(buf);
}

char *interface_last_lex_string()
{
  return (char *) buf;
}
