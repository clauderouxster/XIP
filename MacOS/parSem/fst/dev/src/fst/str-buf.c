/* $Id: str-buf.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "fst/error.h"
#include "fst/str-buf.h"


/******************************************************
 **
 **  STR-BUF.C
 **
 **  Lauri Karttunen, XRCE, February 1999
 **
 **  Small utility for strings.
 **
 ******************************************************/

STRING_BUFFERptr make_string_buffer(int length)
{
  STRING_BUFFERptr str_buf;
  char *string;

 str_buf = (STRING_BUFFERptr) malloc((size_t) sizeof(STRING_BUFFERtype));

 if (!str_buf)
   not_enough_memory("MAKE_STRING_BUFFER");

 string = (char *) malloc((size_t) (length * sizeof(char)));

 if (!string)
   not_enough_memory("MAKE_STRING_BUFFER");

 *string = '\0';

 STRING_BUFFER_string(str_buf) = string;
 STRING_BUFFER_length(str_buf) = length;
 STRING_BUFFER_pos(str_buf) = 0;

 return(str_buf);
}

void free_string_buffer(STRING_BUFFERptr str_buf)
{
  if (str_buf)
    {
      free(STRING_BUFFER_string(str_buf));
      free(str_buf);
    }
}



void initialize_string_buffer(STRING_BUFFERptr str_buf)
{
	STRING_BUFFER_pos(str_buf) = 0;
	
	if (STRING_BUFFER_string(str_buf))
		*(STRING_BUFFER_string(str_buf)) = '\0';

}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

static void assure_space(int len, STRING_BUFFERptr str_buf)
{
  int new_pos = STRING_BUFFER_pos(str_buf) + len + 1;
  int cur_length = STRING_BUFFER_length(str_buf);

  if (new_pos >= cur_length)
    {
      int new_length;
      char *str;

      if (cur_length <= 0)
	new_length = new_pos;
      else
	new_length = 2 * cur_length;

        str = (char *) realloc(STRING_BUFFER_string(str_buf),
			       (size_t) (new_length * sizeof(char)));

      if (!str)
	not_enough_memory("ASSURE_SPACE");

      STRING_BUFFER_length(str_buf) = new_length;
      STRING_BUFFER_string(str_buf) = str;
    }
}


int append_char_to_buffer(char c, STRING_BUFFERptr str_buf)
{
  char *buf;
  int pos = STRING_BUFFER_pos(str_buf);

  assure_space(1, str_buf);
  buf = STRING_BUFFER_string(str_buf);
  
  buf[pos++] = c;
  buf[pos] = '\0';
  STRING_BUFFER_pos(str_buf) = pos;
  return(pos);
}


int append_string_to_buffer(char *str, STRING_BUFFERptr str_buf)
{
  int pos = STRING_BUFFER_pos(str_buf);
  char *buf;

  assure_space(strlen(str), str_buf);
  buf = STRING_BUFFER_string(str_buf);
  
  while (*str)
    buf[pos++] = *str++;

  buf[pos] = '\0';
  STRING_BUFFER_pos(str_buf) = pos;

  return(pos);
}


int print_string_buffer(STRING_BUFFERptr str_buf, FILE *stream)
{
  fputs(STRING_BUFFER_string(str_buf), stream);
  return(STRING_BUFFER_pos(str_buf));
}

#endif /* SLIM_FST */
