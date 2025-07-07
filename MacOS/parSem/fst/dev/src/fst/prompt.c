/* $Id: prompt.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1992 by the Xerox Corporation.  All rights reserved */


#include <stddef.h>
#include <string.h>
#include <stdio.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#ifdef MAC_OS
#include <unix.h>
#endif /* MAC_OS */
#endif /* WIN32 */

#include "fst/error.h"
#include "fst/prompt.h"
#include "fst/fsm_io.h"
/*
#ifdef MAC_OS
#include <Files.h>
#include "mac-file.h"
#endif
*/

/******************************************************
 **
 **  PROMPT.C
 **
 **  Todd Yampol, Xerox PARC, November 1992
 **
 **  Prompt i/o for fsm applications.
 **		-Functions that prompt the user for keyboard input
 **		-Functions for user-prompted fsm file I/O
 **
 ******************************************************/

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifdef MAC_OS_OBSOLETE
char input_buffer[512];
#else
char input_buffer[5192];
#endif

char CURRENT_FILE_NAME[256];
/* declare & allocate char *word in the main application file */
int FOPEN_ERROR = FALSE;
char *word = NULL;

/* Variables for script files */
FILE *SCRIPT_INFILE = NULL, *SCRIPT_OUTFILE = NULL;
char SCRIPT_INFILE_NAME[256], SCRIPT_OUTFILE_NAME[256];


int universal_read_line(char **buffer, int buf_len, FILE *stream)
{
  /* Reads Unix, Mac, and DOS lines. Reads a line from stream and
     stores it with a terminating '\0' into buffer. Returns
     NO_ERROR on reading a line (possibly empty),  ERR_BAD_INPUT if the
     buffer overflows before a terminating "\r", "\n", or "\r\n" is found,
     and EOF when the file is exhausted.
  */
  int c, next;
  char *str = *buffer;
  int lim = buf_len-1;
  int error = NO_ERROR;

  do {
    c = fsm_fgetc(stream);
    if (c == '\r')
      {
	/* Mac or DOS end-of-line */

	if ((next = fsm_fgetc(stream)) != '\n')
	  ungetc(next, stream);
      }
    else if (c == EOF)
      {
	if (str == *buffer)
	  error = EOF;
	break;
      }
    else if (c != '\n')
      {
	if (lim > 0)
	  {
	    *str++ = (char) c;
	    lim--;
	  }
	else
	  {
	    fsm_fprintf(stderr, "String buffer overflow at size %d\n", buf_len);
	    error = ERR_BAD_INPUT;;
	    break;
	  }
      }
  } while (c != '\n' && c != '\r');
	       
  *str = '\0';
    
  return(error);
}


/****************************************
 *                                      *
 *  Console input                       *
 *  Uses strtok on stdin                *
 *                                      *
 ****************************************/

char *get_line(char *prompt)
{
  fsm_fprintf(stdout, "%s: ", prompt);
  fflush(stdin);
#ifdef MAC_OS_OBSOLETE
  fsm_fgets(input_buffer, 512, stdin);
#else
  fsm_fgets(input_buffer, 5192, stdin);
#endif
  return(input_buffer);
}

void flush_get_input_buffer(void)
{
  *input_buffer = '\0';
}

/* This macro just improves the readbility of fget_input().
   Don't use elsewhere! */
#define SCRIPT_P (stream != stdin)
char *fget_input(FILE *stream, char *prompt, int insist_p, char *limiters)
{
  /* Get an input line from the user in response to PROMPT. */
  
  int continue_p = TRUE;
  char *wrd, *line;

#ifdef _WIN32 /* YM, 2/12/96 */
  static int bFirstCommand = TRUE;
  if (bFirstCommand)
  {
	bFirstCommand = FALSE;
  	wrd = NULL;
  }
  else
  {
    wrd = strtok(NULL, limiters);
  }
#else

#ifdef LINUX /* TG, 98-5-26 */
  static int bFirstCommand = TRUE;
  if (bFirstCommand)
    {
      bFirstCommand = FALSE;
      wrd = NULL;
    }
  else
    {
      wrd = strtok(NULL, limiters);
    }
#else
  wrd = strtok(NULL, limiters);
#endif    
#endif  

  while (wrd == NULL && continue_p == TRUE)
    {
      if (!SCRIPT_P)
	{
#ifndef _WIN32
	  fflush(stream);
#endif
	  fsm_fprintf(stdout, "%s ", prompt);
	}
#ifdef MAC_OS_OBSOLETE
      line = fsm_fgets(input_buffer, 512, stream);
#else
      line = fsm_fgets(input_buffer, 5192, stream);
#endif
      if (line == NULL)
	{
	  if (SCRIPT_P)
	    {	
	      return (NULL);  /* end of script file */
	    }
	  else
	    {
	      clearerr(stream);
	      fflush(stream);
	      return(NULL);
	    }
	}
      else
	{
	  /* echo script line -- skip blank lines */
	  if (line[0] != '\n')
	    {
	      if (SCRIPT_P)
		fsm_printf(">>> SCRIPT: %s", line);
	      else if (!isatty(fileno(stdin)))
		fsm_printf("%s\n", line);
	    }
	  
	  wrd = (void *) strtok(line, limiters);		
	}
      
      if (SCRIPT_P)
	continue_p = !wrd; /* continue after blank input line */
      else 
	continue_p = insist_p;
    }
  return(wrd);
}


char *get_more(void)
{
#if ( defined(LINUX) || defined(_WIN32))  /* TG, 98-5-26, JMC 15/04/02 */
  static int bFirstCommand = TRUE;
  if (bFirstCommand)
    {
      bFirstCommand = FALSE;
      return NULL;
    }
  else
    {
      return((char *) strtok(NULL, " \t\n"));
    }
#else
      return((char *) strtok(NULL, " \t\n"));
#endif
}


int yes_or_no (char *question, char default_choice)
/* Returns TRUE is answer is yes, FALSE if answer is no.
   Prompts again if answer is inapproprite.  DEFAULT_CHOICE
   is 'y' or 'n'.  The default is selected if the user
   hits return.  The prompt alerts the user of the
   default.
*/
{
	char ch;
	
	fsm_printf("%s (y/n) [%c]: ", question, default_choice);
	ch = fsm_getchar();
	fflush(stdin);
	
	switch(ch)
	{
		case ('y'):
		case ('Y'):
			return (TRUE);
			break;
		case ('n'):
		case ('N'):
			return (FALSE);
		case ('\n'):
			if (default_choice == 'y' || default_choice == 'Y')	
				return (TRUE);
			else
				return (FALSE);
			break;
		default:
			fsm_printf("Please answer 'y' or 'n'.\n");
			return (yes_or_no(question, default_choice));
			break;
	}
}

/******************************************
 *                                        *
 *  Functions for opening fsm data files  *
 *		                          *
 ******************************************/

/****
  NOTES ABOUT OPENING IFSM DATA FILE (read and write):  
  
  To preserve the consistency of the user interface,
  please only use the functions query_fopen() for user-directed
  opening of fsm data files.
  
  The function prompts the user for a filename, if none was 
  provided on the command line.  We adhere to the following conventions:
  
  (1)  For text files, '-' means stdin or stdout (depending on mode)
  For binary files, '-' has no special meaning.
  
  (2)  On the Mac, the filename ';' will always bring up
  the appropriate dialog box.  The dialog box is activated
  in other ways as well.
  
  (3)  The default value (result of typing <CR>) is given in
  square brakets at all times.
  
  (4)  If no file is opened (for whatever reason),
  all of these functions return NULL.  
  
  YOU MUST CHECK FOR THE POSSIBLITY OF A NULL VALUE
  WHENEVER YOU CALL ANY OF THESE FUNCTIONS!!!
  
  correct:   if (infile = query_fopen()) 
  { ... }
  
  incorrect: infile = query_fopen()
  ...
  
  incorrect (for IFSM):     
  infile = fopen(....)
  ...
  
  (5)  The calling function is responsible for closing the
  file.
  
  NOTE:  For code clarity, please do not call directly the functions 
  text_input_fopen().  Use query_fopen() to access these 
  functions.  If additional capabilities are desired, write
  additional functions on the model of text_input_fopen(), and
  modify query_fopen() appropriately.
  
  %%%TSY 11/12/92
  *****/           

static FILE *text_input_fopen(FILE *instream, int cr_quit_p)
{
  FILE *infile;
  
  word = get_more();
  
  if (word == NULL)
    {
#ifdef MAC_OS
      if (cr_quit_p)
	word = fget_input(instream,
			  "Input file (- = stdin, ; = dialog box) [cancel]:",
			  FALSE, " \t\n");
      else
	word = fget_input(instream, "Input file (; = dialog box) [stdin]:",
			  FALSE, " \t\n");
#else
      if (cr_quit_p)
	word = fget_input(instream, "Input file (- = stdin) [cancel]:",
			  FALSE, " \t\n");
      else
	word = fget_input(instream, "Input file [stdin]:", FALSE, " \t\n");
#endif
    }
  
  if (word == NULL && cr_quit_p)
    {
      fsm_fprintf(stdout, "Cancelled.\n");
      return (NULL);
    }
  
  if ((word == NULL  && !cr_quit_p) || (word[0] == '-' && word[1] == '\0'))
    {
      strcpy(CURRENT_FILE_NAME, "stdin");
      return (stdin);
    }
  
#ifdef MAC_OS_OBSOLETE
  else if (word[0] == ';' && word[1] == '\0')
  {
    word = mac_get_fname(CURRENT_FILE_NAME, 'r');
  
  	if (!word)
  	{
  		fsm_fprintf(stdout, "Cancelled.\n");
  		return (NULL);
  	}
  }
#endif
  
  infile = fopen(word, "r");
  if (infile == NULL)
    {
#ifdef MAC_OS_OBSOLETE
      fsm_fprintf(stderr, "%s: No such file.\n", word);
#else
      perror(word);
#endif     
      return (NULL);
    }
  
/*  fsm_fprintf(stdout, "opening \"%s\"\n", word); */
  strcpy(CURRENT_FILE_NAME, word);
  return (infile);
}

#ifdef MAC_OS_OBSOLETE
#ifdef Power_PC
static unsigned char *CtoPstr (char *str)
{
    /* Converts a C string to a Pascal string */
    if (str)
    {
        short len = strlen(str);
        short i = len;
	  	
       /* assert (len <= 255); */
        while (i > 0)
        {
            str[i] = str[i-1];
            i--;
        }
        str[0] = len;
    }
}
#endif
#endif

static FILE *binary_input_fopen(FILE *instream)
{
  FILE *infile;

  word = get_more();
  
  if (word == NULL)
    {
#ifdef MAC_OS_OBSOLETE
      
#ifdef INFL_APPLICATION
      word = fget_input(instream, "Language [dialog box]:", FALSE, " \t\n");
      if (!word)
	word = mac_get_fname(CURRENT_FILE_NAME, 'r');
#else
      word = mac_get_fname(CURRENT_FILE_NAME, 'r');
#endif
      
#else
      
#ifdef INFL_APPLICATION
      word = fget_input(instream, "Language [cancel]:", FALSE, " \t\n");
#else
      word = fget_input(instream, "Input file [cancel]:", FALSE, " \t\n");
#endif
      
#endif
    }
  
  if (word == NULL)
    {
      fsm_fprintf(stdout, "Cancelled.\n");
      return (NULL);
    }

  infile = fopen(word, "rb");
  if (infile == NULL)
    {
#ifdef MAC_OS_OBSOLETE
      fsm_fprintf(stderr, "%s: No such file.\n", word);
#else
      perror(word);
#endif 
#ifdef INFL_APPLICATION
      return (binary_input_fopen(instream));
#else
      return(NULL);
#endif  
    }

/*  fsm_fprintf(stdout, "opening \"%s\"\n", word); */
  strcpy(CURRENT_FILE_NAME, word);
  return (infile);
}

static FILE *text_output_fopen(FILE *instream)
{
  FILE *outfile;
#ifdef MAC_OS_OBSOLETE
  char mac_buf[80];
#endif
  
  word = get_more();
  
  if (word == NULL)
#ifdef MAC_OS_OBSOLETE
    word = fget_input(instream,
		      "Output file (- = stdout, ; = dialog box) [cancel]:",
		      FALSE, " \t\n");
#else
  word = fget_input(instream, "Output file (- = stdout) [cancel]:",
		    FALSE, " \t\n");
#endif
  
  if (word == NULL)
    {
      fsm_printf("Cancelled.\n");
      return (NULL);
    }
  
  else if (word[0] == '-' && word[1] == '\0')
    {
      strcpy(CURRENT_FILE_NAME, "stdout");
      return (stdout);
    }
  
#ifdef MAC_OS_OBSOLETE
  else if (word[0] == ';' && word[1] == '\0')
    word = mac_get_fname(mac_buf, 'w');
#endif
  
  if (word != NULL) /* operation might have been cancelled! */
    {	
      outfile = fopen(word, "w");
      if (outfile == NULL)
	{
#ifdef MAC_OS_OBSOLETE
      fsm_fprintf(stderr, "%s: No such file.\n", word);
#else
      perror(word);
#endif 
	  return (NULL);
	}
      if (outfile != stdout)
/*	fsm_fprintf(stdout, "opening \"%s\"\n", word); */
      strcpy(CURRENT_FILE_NAME, word);
      return (outfile);
    }
  else
    {
      fsm_fprintf(stdout, "Cancelled.\n");
      return (NULL);
    }
}

static FILE *binary_output_fopen(FILE *instream)
{
  FILE *outfile;
#ifdef MAC_OS_OBSOLETE
  char mac_buf[80];
#endif
  
  word = get_more();
  
  if (word == NULL)
#ifdef MAC_OS_OBSOLETE
    word = mac_get_fname(mac_buf, 'w');
#else
  word = fget_input(instream, "Output file [cancel]:", FALSE, " \t\n");
#endif
  
  if (word != NULL) /* operation might be cancelled! */
    {
      outfile = fopen(word, "wb");
      if (outfile == NULL)
	{
#ifdef MAC_OS_OBSOLETE
      fsm_fprintf(stderr, "%s: No such file.\n", word);
#else
      perror(word);
#endif 
	  return (NULL);
	}
      if (outfile != stdout)
	{
#ifdef MAC_OS_OBSOLETE
	  FInfo myFInfo;
	  Str255 fname;
	  
	  /* Set file type & creator on the Mac */			
	  strcpy((char*)fname, word);
	  CtoPstr((char*)fname);
	  GetFInfo(fname, 0, &myFInfo); 
	  myFInfo.fdType = 'XFSN';
#ifdef LEXC_APPLICATION
	  myFInfo.fdCreator = 'XLXC';
#else
	  myFInfo.fdCreator = 'XFSC';			
#endif
	  SetFInfo(fname, 0, &myFInfo);
#endif
/*	  fsm_fprintf(stdout, "opening \"%s\"\n", word); */
	  strcpy(CURRENT_FILE_NAME, word);
	}
      return (outfile);
    }
  else
    {
      fsm_fprintf(stdout, "Cancelled.\n");
      return (NULL);
    }
}

FILE *fquery_fopen(char *mode, FILE *stream, int cr_quit_p)
     /***
       Prompts the user to select a file.  Prompt scheme depends
       upon mode & platform.  
       
       Opens the file.  Returns a pointer to the open file.
       
       Mode is a subset of those for fopen():
       r or rt for READ TEXT
       w or wt for WRITE TEXT
       rb for READ BINARY
       wb for WRITE BINARY
       ****/
{
  FILE *file;
  
  FOPEN_ERROR = FALSE; /* if no file is opened, this is set to 1 */
  
  switch (mode[0])
    {
    case ('r'):
      switch (mode[1])
	{
	case ('b'):
	  file = binary_input_fopen(stream);
	  break;
	case ('t'):
	case ('\0'):
	  file = text_input_fopen(stream, cr_quit_p);
	  break;
	default:
	  handle_error("Improper mode", "query_fopen", (int) mode[1]);
	  return(NULL);
	}
      break;
    case ('w'):
      switch (mode[1])
	{
	case ('b'):
	  file = binary_output_fopen(stream);
	  break;
	case ('t'):
	case ('\0'):
	  file = text_output_fopen(stream);
	  break;
	default:
	  handle_error("Improper mode", "query_fopen", (int) mode[1]);
	  return(NULL);
	}
      break;
    default:
      handle_error("Improper mode", "query_fopen", (int) mode[0]);
      return(NULL);
    }
  
  if (file == NULL)
    {
      FOPEN_ERROR = TRUE;
    }
  else if (SCRIPT_OUTFILE)
    {
      /* If a script is being recorded,
	 print the file name in the script file. */
      if (file == stdin || file == stdout)
	{
	  fsm_fprintf (SCRIPT_OUTFILE, " -");
	  fsm_fprintf(stderr,
		  "Warning:  Script will not contain stdin keystrokes.\n");
	}
      else
	fsm_fprintf (SCRIPT_OUTFILE, " %s", CURRENT_FILE_NAME);
    }
  
  return (file);
}

#endif /* SLIM_FST */
