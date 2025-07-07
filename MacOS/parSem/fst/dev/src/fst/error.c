/* $Id: error.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "fst/error.h"
#include "fst/fsm_io.h"


/**************************************************************************
 *
 *				ERROR.C
 *
 *                       Error handling routine
 *
 **************************************************************************/

int CONTINUE_OR_STOP = CONTINUE;

void (*fsm_error_function)(char *message, char *function_name, int code) = NULL;

void handle_error_default (char *message, char *function_name, int code)
     /***
      *
      * Prints out an error message, exits program with code -1.
      *
      ***/
{
  fsm_fprintf(stderr, "** ERROR: %s in '%s' (%d) **\n",
	  message, function_name, code);
  fsm_printf("Press return to exit.\n");
  fsm_getchar();
  exit(-1);
}

void handle_error (char *message, char *function_name, int code)
{
  if (fsm_error_function != NULL)
    (*fsm_error_function)(message, function_name, code);
  else
    handle_error_default(message, function_name, code);
}



void handle_warning (char *message, char *function_name, int code)
     /***
      *
      * Prints out an warning message.
      *
      ***/
{
  fsm_fprintf(stderr, "%% Warning: %s in '%s' (%d)\n", message,
	  function_name, code);
}

void not_enough_memory(char *function_name)
{
  handle_error("Out of memory", function_name, 0);
}



void exit_with_error(char *message)
{
    fsm_fprintf(stderr, "%s\n", message);
    exit(1);
}

void check_for_negative_or_zero(int length, char *function)
{
  if (length <= 0)
    handle_error("Negative or zero length", function, length);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

void quit_on_fail_error(int quit_p)
{
  if (quit_p)
    CONTINUE_OR_STOP = STOP;
}

#ifdef NEED_STRERROR
char *strerror(int err_num)
{
    static char UnknownError[50];
    extern int sys_nerr;
    extern char *sys_errlist[];

    if (err_num >= sys_nerr)
    {
	sprintf(UnknownError, "Unknown Error: %d", err_num);
	return(UnknownError);
    }
    else
	return(sys_errlist[err_num]);
}
#endif /* NEED_STRERROR */


/**************************************************************
 * Error and comment functions used in bimach.c
 *
 */

void print_comment (int verbose_p, char *format, ...)
{
  va_list ap;

  if (! verbose_p)
    return;

  va_start (ap, format);
  vfprintf (stderr, format, ap);
  fflush (stderr);
  va_end (ap);
}
#endif /* SLIM_FST */


void prog_error (char *func, char *format, ...)
{
  va_list ap;

  fprintf (stderr, "\nPROGRAMMING ERROR in %s() :\n  ", func);

  va_start (ap, format);
  vfprintf (stderr, format, ap);
  va_end (ap);

  fprintf (stderr, "\n\n  ---- Please report ----\n\n");
  exit (1);
}

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

char *error_buffer (char *action, char *format, ...)
{
  /* Action "put" appends an error message to the buffer.
     Action "clear" clears all messages.
     Action "last" retrieves the last message.
     Action "all" retrieves all messages.
     Different messages are separated by newline. */

  static char *line = NULL, *last = NULL;
  static int max = 0;
  char *result, message [400];  int need;  va_list ap;

  if (! action)
    prog_error ("error_buffer", "Missing action");

  /* ACTION "put" : append message */

  if (! strcmp (action, "put"))
    {
      if (! format)
	prog_error ("error_buffer", "Action 'put' requires non-NULL message");

      va_start (ap, format);
      vsprintf (message, format, ap);
      va_end (ap);

      if (line)
	need = strlen (line) + strlen (message) + 2;
      else
	need = strlen (message) + 2;

      if (max < need)
	{
	  need += 200;  /* get more space for next messages */
	  max = need;

	  if (! line)
	    {
	      if (! (line = (char *) malloc (need * sizeof (char))))
		not_enough_memory ("error_buffer");

	      *line = '\0';
	    }
	  else
	    {
	      if (! (line = (char *) realloc (line, need * sizeof (char))))
		not_enough_memory ("error_buffer");
	    }
	}

      last = line + strlen (line);

      strcat (last, message);
      strcat (last, "\n");
      result = last;
    }

  /* ACTION "clear" : clear all messages */

  else if (! strcmp (action, "clear"))
    {
      if (line)
	{
	  free (line);
	  line = NULL;
	  last = NULL;
	}
      result = NULL;
    }

  /* ACTION "last" : return pointer to last message */

  else if (! strcmp (action, "last"))
    result = last;

  /* ACTION "all" : return pointer to first message */

  else if (! strcmp (action, "all"))
    result = line;

  else
    prog_error ("error_buffer", "Bad action '%s'", action);

  return (result);
}


void not_applicable_to_virtual_net(quit_p)
{
  fsm_fprintf(stderr, "This command cannot be applied to a virtual net\n");
  quit_on_fail_error(quit_p);
}

#endif /* SLIM_FST */
