/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: fst.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __MWERKS__
#include <unix.h>
#endif /* __MWERKS__ */

#if defined _WINDOWS
#include <windows.h>
#include <io.h>
#elif !defined UNDER_CE
#include <sys/mman.h>
#endif

#include "fst/fsm-package.h"
#include "fst/fsm-version.h"

#include "fst/comm-def.h"
#include "fst/context.h"
#include "fst/calc-aux.h"
#include "fst/re-aux.h"
#include "fst/re.lex.h"
#include "fst/interface.h"
#include "fst/iy-stack.h"
#include "fst/interface.tab.h"
#include "fst/interface.lex.h"
#include "fst/replace.h"
#include "fst/fsm-fst-x.h"
#include "fst/fsm_io.h"
#include "fst/fst-prompt.h"
#include "fst/cfsm.h"
#include "fst/error.h"
#include "fst/iy-fs-ops.h"


/****************************************************************
  Global variables.
 ****************************************************************/

static struct command_line 
{
  int quiet;
  int obey_ctrl_c;
} command_line_parameters = { 0, 0};


int * Fst_IY_QUIT_ON_FAIL()
{
  return ( & IY_QUIT_ON_FAIL);
}

int * Fst_IY_COMPOSE_FLAG_AS_SPECIAL()
{
  return ( & IY_COMPOSE_FLAG_AS_SPECIAL);
}

int * Fst_IY_INTERACTIVE()
{
  return ( & IY_INTERACTIVE);
}

int * Fst_IY_VERBOSE()
{
  return ( & IY_VERBOSE);
}

int * Fst_IY_NAME_NETS()
{
  return ( & IY_NAME_NETS);
}

int * Fst_IY_LAST_ERRORS()
{
  return ( & IY_LAST_ERRORS);
}

int * Fst_IY_LEX_ERRORS()
{
  return ( & IY_LEX_ERRORS);
}

int * Fst_IY_RECURSIVE_DEFINE()
{
  return ( &IY_RECURSIVE_DEFINE);
}

int * Fst_IY_MINIMIZE_P()
{
  return ( &IY_MINIMIZE_P);
}

static void init_fsm_fst_global_variables()
{
  fst_IY_QUIT_ON_FAIL = & Fst_IY_QUIT_ON_FAIL;
  fst_IY_COMPOSE_FLAG_AS_SPECIAL = & Fst_IY_COMPOSE_FLAG_AS_SPECIAL;
  fst_IY_INTERACTIVE = Fst_IY_INTERACTIVE;
  fst_IY_VERBOSE = Fst_IY_VERBOSE;
  fst_IY_NAME_NETS =  Fst_IY_NAME_NETS;
  fst_IY_LAST_ERRORS = Fst_IY_LAST_ERRORS;
  fst_IY_LEX_ERRORS = Fst_IY_LEX_ERRORS ;
  fst_IY_RECURSIVE_DEFINE = Fst_IY_RECURSIVE_DEFINE;
  fst_IY_MINIMIZE_P = Fst_IY_MINIMIZE_P;
}

/*******************
  static functions
 *******************/

static void command_line_help(void)
{
  char *app = FST_PROMPT;
  char *APP;

  if (strcmp(FST_PROMPT, "xfst") == 0)
    APP = "XFST";
  else
    APP = "FST";

  /* Copy the prompt into upper case */

 fsm_fprintf(stdout, "\n%s COMMAND-LINE OPTIONS:\n", APP);
 fsm_fprintf(stdout, "-------------------------\n");

 fsm_fprintf(stdout, "-e \"command\"\t\
execute the command before starting the application.\n\t\t\
If more than one '-e' option is specified, the commands\n\t\t\
are executed in the specified order. For example:\n\t\t\
'%s -e \"load foo\" -e \"reverse net\" -e \"save fie\"'\n", app);
 fsm_fprintf(stdout,"\t\t\
After the commands have been executed, %s starts in\n\t\t\
the interactive mode unless the option '-stop' follows.\n", app);

 fsm_fprintf(stdout, "-f scriptfile\t\
execute the commands in 'scriptfile' and exit.\n");

 fsm_fprintf(stdout, "-flush\t\t\
show output immediately without waiting for a carriage\n\t\t\
return.\n");

 fsm_fprintf(stdout, "-h\t\tprint a cryptic 'usage' message and exit.\n");

 fsm_fprintf(stdout, "-help\t\tprint this help message and exit.\n");

 fsm_fprintf(stdout, "-l startscript\t\
execute the commands in 'startscript' and start %s\n\t\t\
in the interactive mode.\n", app);

 fsm_fprintf(stdout, "-pipe\t\t\
execute without prompting for commands.\n");

 fsm_fprintf(stdout, "-q\t\t\
operate quietly. Don't print unnecessary messages.\n");

 fsm_fprintf(stdout, "-s binaryfile\t\
load 'binaryfile' and start %s in the interactive mode\n\t\t\
The file must have been created with the 'save' command.\n", app);

 fsm_fprintf(stdout, "-stop\t\t\
stop %s without entering into the interactive mode.\n\t\t\
Any option following '-stop' is ignored.\n", app);

 fsm_fprintf(stdout, "-v\t\tprint %s version number and exit.\n", app);
}


static void parse_command_line(int argc, char *argv[], FST_CNTXTptr cntxt)
{
  int i;
  /*
  int FST_OUTPUT_BUFFER_SIZE=0;
  void * newbuf;
  */
  if (!isatty(0))
    {
      command_line_parameters.obey_ctrl_c = 1;
      IY_INTERACTIVE = 0;
    }
  for (i=1; i<argc; i++)
    if (CONTINUE_OR_STOP == STOP &&
	(!strcmp(argv[i], "-e") || !strcmp(argv[i], "-l") ||
	 !strcmp(argv[i], "-f") || !strcmp(argv[i], "-s")))
      {
        char* cmd = argv[i];
        char* arg = "";
        if (i+1 < argc)
          {
            i++;
            arg = argv[i];
          }    
        fprintf(stderr, "Command %s \"%s\" not executed\n", cmd, arg);
      }
    else if (!strcmp(argv[i],"-e") && (i+1 < argc))
      {
	/* Execute one command */

#ifdef __GNUC__
	char buf[strlen(argv[++i])+5];
#else
        char *buf = (char *)malloc(strlen(argv[++i])+5);
#endif /* __GNUC__ */
	strcpy(buf,argv[i]);
	strcat(buf,"\n");
	FST_embedded_command_p(cntxt) = TRUE; 
	IY_INTERACTIVE = 0;
	if (CONTINUE_OR_STOP == CONTINUE)
	  {
	    execute_string(buf, cntxt);
	    FST_embedded_command_p(cntxt) = FALSE;
	    if (CONTINUE_OR_STOP == STOP)
	      fsm_fprintf(stderr, "Because quit-on-fail is ON, any remaining \
commands are not executed.\n");
	  }
#ifndef __GNUC__
        free(buf);
#endif /* ! __GNUC__ */
	IY_INTERACTIVE = 1;
      }
    else if (!strcmp(argv[i],"-f") && (i+1 < argc))
      {
	/* Execute only a script. */

	command_line_parameters.obey_ctrl_c = 1;
	IY_INTERACTIVE = 0;
	if (!freopen(argv[++i],"r",stdin))
	  {
	    fsm_fprintf(stderr,"%s: file not found %s\n",argv[0],argv[i]);
	    exit(-1);
	  }
	interface_file_info->file.name = argv[i];
      }
    else if (!strcmp(argv[i],"-flush"))
      { /* sets the print default to 'fprintf_fflush' */
	set_fsm_fprintf ((FPRINTF_FUNC) fprintf_fflush);
      }
    else if (!strcmp(argv[i], "-help"))
      {
	command_line_help();
	exit(0);
      }
    else if (!strcmp(argv[i],"-s") && (i+1 < argc))
      {
	 /* Load a binary file */

	  iy_stack_load(argv[++i], USE_CURRENT_STACK);
      }
    else if (!strcmp(argv[i],"-stop"))
      {
	exit(CONTINUE_OR_STOP);
      }
    else if (!strcasecmp(argv[i], "-utf8") ||
	     !strcasecmp(argv[i], "-utf-8"))
      {
	iy_set_char_encoding(CHAR_ENC_UTF_8, NULL);
      }
    else if (!strcmp(argv[i],"-l") && (i+1 < argc))
      {
	/* Execute a script and go to interactive mode on error
	   or at the end of the script. */

	int cc;
	ifile file;
	file.name = argv[++i];
	file.type = FILE_IN;
	cc = command_line_parameters.obey_ctrl_c;
	command_line_parameters.obey_ctrl_c = 1;
	IY_INTERACTIVE = 1;
	iy_source(&file, cntxt);
	command_line_parameters.obey_ctrl_c = cc;
      }
    else if (!strcmp(argv[i],"-q")) /* Do not print anything unnecessary. */
      {
	command_line_parameters.quiet = 1;
	IY_VERBOSE = 0;
	display_read_status_bar(0);
      }
    else if (!strcmp(argv[i],"-pipe"))
      {
	command_line_parameters.obey_ctrl_c = 1;
	IY_INTERACTIVE = 0;
      }
  /*   Decommissioning this obsolete option  2002-05-08 L.K.  */
  /*
    else if (!strcmp(argv[i],"-o") && (i+1 < argc))
      {
	if ( (sscanf(argv[++i],"%u", &FST_OUTPUT_BUFFER_SIZE)!= EOF))
	  {
	    if (FST_output_buffer_size(cntxt) < FST_OUTPUT_BUFFER_SIZE)
	      {
		if ( (newbuf = malloc((size_t) FST_OUTPUT_BUFFER_SIZE * sizeof(char))))
		  {
		    free(FST_output_buffer(cntxt));
		    FST_output_buffer(cntxt) = newbuf;
		    FST_output_buffer_size(cntxt) = FST_OUTPUT_BUFFER_SIZE;
		  }
		else
		  {
		    fprintf(stderr, "-o: Unable to allocate %d bytes. Try less.\n", FST_OUTPUT_BUFFER_SIZE);
		    exit(-1);
		  }
	      }
	  }
	else
	  {
	    fprintf(stderr, "Usage: -o <#output_buffer_size_in_bytes>\n");
	    exit(-1);
	  }
      }
  */
    else if (!strcmp(argv[i],"-v"))
      {
	fsm_fprintf(stderr,"%s %s \n", argv[0],
                FSM_VERSION_STRING);
	if (argc == 2) exit(0); /* The only argument */
      }
    else					/* Not a proper command line */
      {
	fsm_fprintf(stderr,"usage: %s [-utf8] [-e \"command\"] [-f scriptfile] \
[-flush] [-h] [-help] [-l startscript] [-pipe] [-s binaryfile]  [-stop] [-q] [-v]\n",argv[0]);
	exit(-1);
      }
}

/****************************************************************
  Interruption (means: type <CTRL>-C)
 ****************************************************************/
static jmp_buf buffff;

void volatile restart_fst(int n)
{
  interface_lex_total_restart();
  reset_out_buffer();
  longjmp(buffff, n);
}

static void handle_interrupt(int n)
{
  if (!command_line_parameters.obey_ctrl_c)
    {
      restart_fst(n);
    }
  else
    {
      fputs("Interrupted\n",stderr);
      exit(n);
    }
}

#if (!defined(MAC_OS) && !defined(_WIN32))
static void handle_broken_pipe(int n)
{
  if (IY_INTERACTIVE)
    {
      restart_fst(n);
    }
  else
    {
      fputs("Broken pipe\n",stderr);
      exit(n);
    }
}
#endif /* !MAC_OS && !_WIN32 */

/****************************************************************
  main function
 ****************************************************************/

static void run_commands(FST_CNTXTptr fst_cntxt)
{
  switch (setjmp(buffff))
    {
    case SIGINT:
      fputs("\nDo you want to exit the program (yes/no/info)? ",stdout);
      switch (wait_for_confirm2())
	{
	case ATOMI_YES:  return;
	case ATOMI_INFO: print_storage_info(stderr);
	case ATOMI_NO:
	  recover_temp_memory();
	  break;
	}

#if (!defined(MAC_OS) && !defined(_WIN32))
    case SIGPIPE:
#endif
    case 0:
      signal(SIGINT, handle_interrupt);
#if (!defined(MAC_OS) && !defined(_WIN32))
      signal(SIGPIPE, handle_broken_pipe);
#endif
      execute_commands(fst_cntxt);
    }
}

static char *current_year(void)
{
  static char year[5];
  time_t cur_time = time(NULL);

  strftime(year, 5, "%Y", localtime(&cur_time));

  return(year);
}

static char *copyright_symbol(void)
{
  /*
  static unsigned char sym[3];

  if (IS_UTF8_MODE)
    {
      sym[0] = (unsigned char) 194;
      sym[1] = (unsigned char) 169;
      sym[2] = '\0';
    }
  else
    {
      sym[0] = (unsigned char) 169 ;
      sym[1] = '\0';
    }
  return (char *) sym;
*/

  /* The only safe solution to make a legally binding Copyright
     declaration. If the user's terminal is not in sync with
     the declaration, the fancy copyright symbol does not print
     correctly. */

  return("(c)");
}

int main(int argc, char *argv[])
{
	FST_CNTXTptr cntxt;

  IY_LICENSE_TYPE = get_user_license_type();

  cfsm_init();
  init_fsm_fst_global_variables();
  cntxt = get_default_context();

  start_flex(NULL);
  
  start_regex_flex(NULL);

/* Global variables for file creator and type on Macintosh OS */
#ifdef MAC_OS
_fcreator = 'ttxt';
_ftype = 'TEXT';
#endif
 
  parse_command_line(argc,argv,cntxt);	/* Read given parameters */

  if (IY_INTERACTIVE)	/* This is set by parse_command_line */
    {
      print_prompt = 1;
      /* This is useful but it may cause troubles in some systems. */
      setvbuf(stdout, NULL, _IONBF, 1);
    }
  if (!command_line_parameters.quiet && IY_INTERACTIVE)
    {
      fsm_fprintf(stderr, "Copyright %s Xerox Corporation 1997-%s\n",
		  copyright_symbol(), current_year());
      fsm_fprintf(stderr,"Xerox Finite-State Tool, version %s \n",
	      FSM_VERSION_STRING);
/*
      puts("\n\
           **** UNDER MODIFICATION ****\n\
            **** DRIFTING TO \"FST\" ****\n\
\n\
\n\
 *******     ***      ***  *******        *****\n\
 ********     ***    ***   ********      *******\n\
 **    ***     ***  ***    **    ***    ***\n\
 **     ***     ******     **     ***  ***\n\
 **    ***       ****      **    ***   **\n\
 ********        ****      ********    **\n\
 *******        ******     *******     ***\n\
 **   ***      ***  ***    **   ***     ***\n\
 **    ***    ***    ***   **    ***     *******\n\
 **     ***  ***      ***  **     ***     *****\n\
    ___ _     _ _        ___ _       _\n\
   | __(_)__ (_| |_____ / __| |____ | |_____\n\
   | _|| |  \\| |  _/ -_)\\__ \\  _/ _\\|  _/ -_)\n\
   |_| |_|_|_|_|\\__\\__/ |___/\\__\\___|\\__\\__/\n\
          ___               _ _\n\
         / __|___  ___  ___(_| |___ ___\n\
        | (__/ _ \\/   \\/ _ | | / -_)  _)\n\
         \\___\\___/_|_|_| __|_|_\\__/|_|\n\
                       |/\n\
Created by Pasi Tapanainen\n\
*/
      puts("\n\
Type \"help\" to list all commands available or \"help help\" for further help.\n");
    }

  run_commands(cntxt);

  if (IY_VERBOSE) fputs("bye.\n",stdout);
  release_fsm_memory();
  unset_default_context();
  return 0;
}
