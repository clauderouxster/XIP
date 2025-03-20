/* $Id: regex-in.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1993-94 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  REGEX-IN.C
 **
 **  André Kempe, RXRC, January 1997
 **
 **  Functions for generating networks from:
 **
 **  - regular expression files
 **  - text files (lists of words)
 **  - spaced text files (lists of spaced word pairs)
 **
 **  - regex strings with variables
 **
 ******************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "fst/types.h"
#include "fst/fst-types.h"
#include "fst/regex-in.h"
#include "fst/bin-in.h"
#include "fst/re.lex.h"
#include "fst/re-aux.h"
#include "fst/regex.h"
#include "fst/warning.h"
#include "fst/error.h"
#include "fst/prolog.h"
#include "fst/text-io.h"
#include "fst/label.h"
#include "fst/copy-fsm.h"
#include "fst/varstack.h"
#include "fst/fsm-fst-x.h"
#include "fst/fsm_io.h"
#include "fst/interface.h"
#include "fst/utf8_fatstr_util.h"

#ifdef UNDER_CE
FILE* tmpfile()
{
	return NULL;
}
#endif
#if  (defined(MAC_OS) || defined(_WIN32))
int regex_parse(void *);		/* Needed only if prototypes are obligatory. */
#endif

/******************************************************************
  Basic functions */

static INPUT_HIST_CELLptr input_history = NULL;

static int file_in_input_history (char *filename)
{
  /* Checks whether the file is already opened on a previous
     level of recursion. */

  INPUT_HIST_CELLptr ihcell;

  for (ihcell = input_history;
       (ihcell) && (ihcell->input->file.type == FILE_IN);
       ihcell = ihcell->prev)
    if (! strcmp (filename, ihcell->input->file.name))
      return (TRUE);   /* file name found in history */

  return (FALSE);   /* file name not found */
}

static FILE *fopen_atfile (char *filename)
{
    FILE *stream;

    if (! (stream = fopen (filename, "r")))
    {
        fsm_fprintf (stderr, "File '%s' was called by a regular expression.\n\
It could not be opened.\n", filename);
	regex_error("File may not exist and is not opened.");
	quit_on_fail_error(*fst_IY_QUIT_ON_FAIL());
    }
    return (stream);
}

static finfo *set_new_atfile (ifile *file, FILE *stream, finfo *buffer)
{
  finfo *old;
  LexEOF = 0;

  print_prompt--;
  old = new_re_lex_buffer(buffer, stream, file, 1);
  return old;
}

static int restore_old_atfile (finfo *buffer, finfo *actual_buffer)
{
 /* Resetting an old stream and buffer for reading. */
  LexEOF=0;

  if (buffer)
    {
      print_prompt++;
      old_re_lex_buffer (buffer, actual_buffer);
      return 1;
    }
  else
    {
      /* We can only hope that IF lex is ever called again, it is
       * now restarted properly.  */
      re_lex_total_restart();
      regex_file_info = NULL;
      return 0;
    }
}

static NETptr net_from_regex_atfile (char *filename)
{
  NETptr net;  NVptr nv;  FILE *stream;
  INPUT_HIST_CELLtype inhist;

  inhist.input = regex_file_info;
  inhist.prev = input_history;
  input_history = &inhist;

  if (file_in_input_history (filename))
  {
      fsm_fprintf (stderr, "File '%s' was called by a regular expression.\n\
It was already opened on a higher level.\n\
This kind of recursion is not allowed.\n", filename);
      quit_on_fail_error(*fst_IY_QUIT_ON_FAIL());
      return (null_fsm());
  }

  if (strcmp(filename, "-") == 0)
    stream = stdin;
  else if (! (stream = fopen_atfile (filename)))
    return (null_fsm());

  nv = regex_from_atfile (filename, stream, -1);

  input_history = inhist.prev;

  /* check if there was exactly one net in the file */

  if ((! nv) || (! NV_len(nv)))
  {
      fsm_fprintf (stderr, "File '%s' was called by a regular expression.\n\
It does not contain any regular expressions.\n", filename);
      quit_on_fail_error(*fst_IY_QUIT_ON_FAIL());
      return (null_fsm());
  }

  if (NV_len(nv) > 1)
    fsm_fprintf (stderr, "*** WARNING:\n\
File '%s' was called by a regular expression.\n\
It contains %d regular expressions.\n\
Only the first expression was used to create a network !!!",
	     filename, NV_len(nv));

  /* take the 1st network */

  net = NV_net (nv, 0);
  NV_net (nv, 0) = NULL;
  free_nv_and_nets (nv);

  if (stream != stdin)
    fclose (stream);

  return (net);
}

/******************************************************************
  Reading networks inside regular expressions
  from other files than regex files */

static NETptr net_from_binary_atfile (char *filename)
{
  NETptr net;  NVptr nv;

  nv = read_nets (filename);

  /* check if there was exactly one net in the file */

  if ((! nv) || (! NV_len(nv)))
  {
      fsm_fprintf (stderr, "File '%s' was called by a regular expression.\n\
It does not contain any binary network.\n", filename);
      quit_on_fail_error(*fst_IY_QUIT_ON_FAIL());
      return (null_fsm());
  }

  if (NV_len(nv) > 1)
    fsm_fprintf (stderr, "*** WARNING:\n\
File '%s' was called by a regular expression.\n\
It contains %d binary networks.\n\
Only the first network was used !!!", filename, NV_len(nv));

  /* take the 1st network */

  net = NV_net (nv, 0);
  NV_net (nv, 0) = NULL;
  free_nv_and_nets (nv);

  return (net);
}

static NETptr net_from_text_atfile (char *filename)
{
  NETptr net;  FILE *stream;

  if (strcmp(filename, "-") == 0)
    stream = stdin;
  else if (! (stream = fopen_atfile (filename)))
    return (null_fsm());

  net = word_file_to_fsm (stream);
  if (stream != stdin)
    fclose (stream);

  return (net);
}

static NETptr net_from_spacedtext_atfile (char *filename)
{
  NETptr net;  FILE *stream;

  if (strcmp(filename, "-") == 0)
    stream = stdin;
  else if (! (stream = fopen_atfile (filename)))
    return (null_fsm());

  net = network_from_text_file (stream);

  if (stream != stdin)
    fclose (stream);

  return (net);
}

static NETptr net_from_prolog_atfile (char *filename)
{
  NETptr net;  NVptr nv;  FILE *stream;
#ifndef UNDER_CE

  if (strcmp(filename, "-") == 0)
    stream = stdin;
  else if (! (stream = fopen_atfile (filename)))
    return (null_fsm());

  nv = networks_from_prolog_file (stream);

  /* check if there was exactly one net in the file */

  if ((! nv) || (! NV_len(nv)))
  {
      fsm_fprintf (stderr, "File '%s' was called by a regular expression.\n\
It does not contain any prolog network.\n", filename);
      quit_on_fail_error(*fst_IY_QUIT_ON_FAIL());
      return (null_fsm());
  }

  if (NV_len(nv) > 1)
    fsm_fprintf (stderr, "*** WARNING:\n\
File '%s' was called by a regular expression.\n\
It contains %d prolog networks.\n\
Only the first network was used !!!", filename, NV_len(nv));

  /* take the 1st network */

  net = NV_net (nv, 0);
  NV_net (nv, 0) = NULL;
  free_nv_and_nets (nv);

  if (stream != stdin)
    fclose (stream);

#endif
  return (net);
}

/******************************************************************
 *                                                                *
 * Top-level functions for reading fsms from a regex file         *
 *                                                                *
 ******************************************************************/

NVptr regex_from_atfile (char *filename, FILE *stream, int num_nets)
{
  ifile *file, filebf;
  NETptr net;  NVptr nv;
  finfo buffer, *prev_input;
  int i, prev_one_liner, prev_one_regex;

  prev_one_liner = one_liner;  one_liner = 0;
  prev_one_regex = one_regex;  one_regex = 0;

  file = &filebf;
  file->name = filename;
  file->type = FILE_IN;
  nv = make_nv (0);

  prev_input = set_new_atfile (file, stream, &buffer);
  *fst_IY_LAST_ERRORS() = 0;

  for (i=0; (i< num_nets) || (num_nets <0); i++)
    {
      wait_for_regex();
      if (regex_parse(NULL))      /* net ok */
	net = last_regex();
      else if (LexEOF)       /* end of regex file reached */
	break;
      else               /* parsing error; but do not stop reading */
	net = NULL;

      nv_add (net, nv);
    }

  if (*fst_IY_LAST_ERRORS())
    {
      fsm_fprintf (stderr, "No network(s) created\n");
      free_nv_and_nets (nv);
      nv = NULL;
    }

  restore_old_atfile (prev_input, &buffer);
  one_liner = prev_one_liner;
  one_regex = prev_one_regex;

  return (nv);
}

NETptr net_from_atfile (char *filename, int filetype, int fatstr_p)
{
  char fname [502];  NETptr net;

  if (fatstr_p)
    {
      FST_fat_to_thin_name (fname, (FAT_STR)filename, 500, FALSE);
      filename = fname;
    }

  switch (filetype)
    {
    case 0:  /* BIN_ATFILE */
      net = net_from_binary_atfile (filename);
      break;
    case 1:  /* REGEX_ATFILE */
      net = net_from_regex_atfile (filename);
      break;
    case 2:  /* TEXT_ATFILE */
      net = net_from_text_atfile (filename);
      break;
    case 3:  /* SPACEDTEXT_ATFILE */
      net = net_from_spacedtext_atfile (filename);
      break;
    case 4:  /* PROLOG_ATFILE */
      net = net_from_prolog_atfile (filename);
      break;
    default:
      fsm_fprintf (stderr, "File '%s' was called by a regular expression.\n\
It has a wrong type specification after '@'.\n", filename);
      quit_on_fail_error(*fst_IY_QUIT_ON_FAIL());
      net = NULL;
      break;
    }

  if (! net)
    net = null_fsm();

  return (net);
}


/******************************************************
 *
 *   FUNCTIONS FOR GENERATING NETWORKS FROM
 *
 *   REGEX STRINGS WITH VARIABLES
 *
 ******************************************************/

/*******************
 * Basic functions for variable stack handling
 */

static RE2N_VSTptr RE2N_CURRENT_VAR_STACK;

NETptr RE2N_var_net (char *num_str)
{
  NETptr net;  int num;

  num = atol (num_str);
  net = copy_fsm (NV_net (RE2N_CURRENT_VAR_STACK->stack, num));
  return (net);
}

/*******************
 * Basic (static) functions for regex string parsing
 */

NETptr RE2N_net_from_regex_string (char *regex_str)
{
  /* Important change:  (from re.template.lex) calls "unput()"
     at a place where it is not justified. 
     Here it is circumvented temporarily by setting and resetting:
     set_unput_allowed (FALSE) cf. TRUE
     */

  /* This function should create a network from a regex string.
     Unfortunately, it crashes. If you have the time and know how to do,
     you may debug it (cf. re.lex).
     Currently, we use "RE2N_net_from_regex_string()" instead,
     which is a horrible hack with a temporary file. */

  NETptr net = NULL;
  ifile file;
  finfo buffer = { NULL, NULL, 0, { NULL, 0}}, *prev;
  int prev_one_liner = one_liner;
  int prev_one_regex = one_regex;
  int save_unput_flag=0;
  one_liner = one_regex = 0;

  file.name = regex_str;
  file.type = STRING_IN;
  prev = new_re_input_file (&file, &buffer);

   save_unput_flag = set_unput_allowed (FALSE); /* ** 97-12-1 TG */
  set_warn_missing_semic_EOF_allowed (FALSE);

  *fst_IY_LAST_ERRORS() = 0;
  wait_for_regex();

  if (regex_parse(NULL) && !(*fst_IY_LAST_ERRORS()))  /* parsing error ; no net created */
    net = last_regex();
  else
    net = NULL;

  old_re_input_file (prev, &buffer);    /* in re.lex ; prev allowed to be NULL */
  one_liner = prev_one_liner;
  one_regex = prev_one_regex;

  set_unput_allowed (save_unput_flag);  /* 99-6-7; 97-12-1 TG */
  set_warn_missing_semic_EOF_allowed (TRUE);

  return (net);
}

#if NOT_USED
static NETptr old_RE2N_net_from_regex_string (char *regex_str)
{
  /* This function returns a network created from a regex string.
     In case of syntax errors or other problems a message is
     printed and NULL is returned.

     This function is a horrible hack with a temporary file.
     We would prefer using "FUTURE_RE2N_net_from_regex_string()"
     instead, which currently crashes. If you have the time and
     know how to do, you may debug it. 06/02/97 A.K. */

  NETptr net;  NVptr nv;  FILE *tfile;

  if (! (tfile = tmpfile()))
    {
      fsm_fprintf (stderr, "\n\
**** PROGRAMMING ERROR ****   Please, inform the programmer.\n\
Temporary file could not be opened, to parse the string:\n\
%s\n\n", regex_str);
      return (NULL);
    }

  fsm_fprintf (tfile, "%s ;\n", regex_str);
  rewind (tfile);

  if (! (nv = regex_from_atfile ("RE2N_TMPFILE", tfile, 1)))
    {
      fsm_fprintf (stderr, "\n\
**** PROGRAMMING ERROR ****   Please, inform the programmer.\n\
Syntax error found in regular expression from string:\n\
%s\n\n", regex_str);
      net = NULL;
    }
  else         /* regex ok ; net was created */
    {
      net = NV_net (nv, 0);
      free_nv_only (nv);
    }

  fclose (tfile);
  return (net);
}
#endif /* NOT_USED */

static int RE2N_find_next_var
  (RE2N_VSTptr varstack, char **beg, char **end, char *substit, int *error_p)
{
  /* Looks for the next occurence (starting at *end) of a variable.
     Places *beg on the beginning and *end behind the end of the occurence.
     Copies the variable name substitution into substit[].
     If an error occurs, a message is printed and *error_p is set to
     TRUE (otherwise it is FALSE).
     The function returns TRUE if a variable occurence is found,
     and FALSE otherwise. */

  int len, num;

  /* look for beginning and end */

  *error_p = FALSE;  /* no error */

  if (! (*beg = strstr (*end, RE2N_VAR_BEGIN)))
    return (FALSE);          /* no more occurences of $( */

  if (! (*end = strstr ((*beg)+1, RE2N_VAR_END)))
    {                              /* no more occurences of ) */
      fsm_fprintf (stderr, "\n\
**** PROGRAMMING ERROR ****   Please, inform the programmer.\n\
Syntax error in variables of a regular expression.\n");

      *error_p = TRUE;  /* error occured */
      return (FALSE);
    }

  *end += strlen (RE2N_VAR_END);   /* set behind the variable end */

  /* make substitution */

  len = ((*end) - (*beg)) -strlen (RE2N_VAR_BEGIN) -strlen (RE2N_VAR_END);
  memcpy (substit, (*beg) +strlen (RE2N_VAR_BEGIN),
	  len * sizeof (char));    /* original variable name */
  substit [len] = '\0';

  if ((num = RE2N_get_var_number (varstack, substit)) < 0)  /* not found */
    {                              /* no more occurences of ) */
      fsm_fprintf (stderr, "\n\
**** PROGRAMMING ERROR ****   Please, inform the programmer.\n\
Variable '%s' of a regular expression not found in stack.\n",
	       substit);

      *error_p = TRUE;  /* error occured */
      return (FALSE);
    }

  if (num == 0)  /* 0 must be escaped, otherwise it means epsilon */
    sprintf (substit, " %s %%%d ", RE2N_VAR_TOKEN, num);
  else
    sprintf (substit, " %s %d ", RE2N_VAR_TOKEN, num);

  return (TRUE);
}

static char *RE2N_substitute_vars (RE2N_VSTptr varstack, char *regex_str)
{
  /* Substitutes variables of type $(my_var_4) by type .RE2N_VAR.12
     using the variable stack.
     Returns a substituted regex string or NULL if it fails. */

  char *new_regex, *beg, *end, *source, *dest, substit [100];
  int error_p, len, lendiff = 0;

  /* determine the required length of new_regex[] */

  beg = end = regex_str;
  while (RE2N_find_next_var (varstack, &beg, &end, substit, &error_p))
    lendiff += strlen (substit) - (end - beg);  /* new_len - old_len */

  if (error_p)
    {
      fsm_fprintf (stderr, "Error in regular expression:\n%s\n\n", regex_str);
      return (NULL);
    }

  /* make new_regex[] */

  len = strlen (regex_str) + 1 + lendiff;
  if (! (new_regex = (char *) malloc (len * sizeof (char))))
    not_enough_memory ("RE2N_substitute_vars");

  source = beg = end = regex_str;
  dest = new_regex;
  while (RE2N_find_next_var (varstack, &beg, &end, substit, &error_p))
    {
      len = beg - source;
      memcpy (dest, source, len * sizeof (char));   /* before var */
      dest += len;

      len = strlen (substit);
      memcpy (dest, substit, len * sizeof (char));  /* new var name */
      dest += len;
      source = end;
    }

  strcpy (dest, source);   /* behind last variable */
  return (new_regex);
}

/*******************
 * High level functions for regex string parsing
 */

NETptr RE2N_create_net (RE2N_VSTptr varstack, char *regex_str)
{
  /* Returns a net from the regex string using the variable stack.
     If it fails, we exit. */

  NETptr net;  char *new_regex;

  if (! (new_regex = RE2N_substitute_vars (varstack, regex_str)))
    exit (1);

  RE2N_CURRENT_VAR_STACK = varstack;

  if (! (net = RE2N_net_from_regex_string (new_regex)))
    exit (1);

  RE2N_CURRENT_VAR_STACK = NULL;

  free (new_regex);
  return (net);
}

void RE2N_def_var (RE2N_VSTptr varstack, char *name, char *regex_str)
{
  /* Creates a variable net from the regex string and puts it on the
     stack. Note, variables in the regex and final variable will be
     on the same stack. */

  NETptr net;

  net = RE2N_create_net (varstack, regex_str);

  if (! RE2N_save_var (varstack, name, net))
    exit (1);
}

/*******************
 * TEST FUNCTIONS for regex string parsing and variable stack handling
 */

void RE2N_TEST_1 ()
{
  RE2N_VSTptr vst;  NETptr net;
  int i, op;  char a[15], re[80];

  for (;;)
    {
      op = 0;  /* operation: quit */

      switch (op)
	{
	case 0:  /* quit */
	  return;
	case 1:
	  net = RE2N_var_net (a);
	  break;
	case 2:
	  vst = RE2N_create_var_stack ();
	  break;
	case 3:
	  RE2N_remove_var_stack (vst, TRUE);
	  break;
	case 4:
	  i = RE2N_save_var (vst, a, net);
	  break;
	case 5:
	  i = RE2N_remove_var (vst, a);
	  break;
	case 6:
	  net = RE2N_copy_var (vst, a);
	  break;
	case 7:
	  net = RE2N_create_net (vst, re);
	  break;
	case 8:
	  RE2N_def_var (vst, a, re);
	  break;
	}
    }
}


/*** BEGIN testing regex from string 1 Dec .97 TG ***/
static NETptr tg_net_from_regex_string(char *regex_str)
{
  NETptr net = NULL;
  FILE *ff = re_lex_infile(); 
  net = RE2N_net_from_regex_string(regex_str);
  regex_tg_restart(ff, FALSE);
  return (net);
}

NETptr test1_tg_net_from_regex_string ()
{
  char *t1= "AA | BB";
  
  return tg_net_from_regex_string(t1);
}

NETptr test2_tg_net_from_regex_string ()
{
  char *t1= "B B | C C C -";

  return RE2N_net_from_regex_string(t1);
}

/*** END testing regex from string 1 Dec .97 TG ***/
