/* $Id: config.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 1994 by the Xerox Corporation.  All rights reserved */

/* functions to read an .fsmrc file */

#include <stdio.h>
#include <stdlib.h>

#if defined _WINDOWS
#include <windows.h>
#endif

#ifdef MAC_OS
#include <unix.h>
/*
typedef int uid_t;		Must be obsolete. LK 98-09-25.
*/
#else	 
#ifndef _WIN32
#include <unistd.h>
#include <pwd.h>
#endif
#endif

#include <string.h>

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */
#include "fst/types.h"
#include "fst/error.h"
#include "fst/config.h"
#include "fst/fsm_io.h"
#include "fst/site-dep.h"

#define RCFILENAME "fsmrc"

#if defined(_WIN32) && ! defined(__GNUC__) && ! defined(UNDER_CE)
#if 0
int isatty( int handle )
{
    return _isatty(handle);
}

char *strdup( const char *strSource )
{
    return _strdup(strSource);
}

void *alloca( size_t size )
{
    return _alloca(size);
}

int fileno( FILE *stream )
{
    return _fileno(stream);
}

int chdir( const char *dirname )
{
    return _chdir(dirname);
}
#endif

#endif /* WIN32 */

#if defined(MAC_OS)
static char *strdup(char *str)
{
  return(strcpy((char *) malloc((size_t) strlen(str) + 1), str));
}

static int strncasecmp(char *str1, char *str2, int len)
{
  while(len > 0)
  {
      if (!(*str1 == *str2 || (*str1 + ' ') == *str2 || *str1 == (*str2 + ' ')))
        return(1);
      str1++;
      str2++;
      len--;
  }
 return(0);
}
#endif

#ifdef MAC_OS
typedef struct passwd {
    char *pw_dir;
} PWtype, *PWptr;

static PWptr getpwuid(uid_t uid)
{
  static PWtype pw;
  return(&pw);
}
#endif


static FsmConfig config = NULL;

void fsm_config_init()
{
#ifndef _WIN32
    struct passwd *pw;
    uid_t uid = getuid();
#endif

    if (config != NULL)
	return;

    config = (FsmConfig)calloc(1, sizeof(FsmConfigRec));
    if (config == NULL)
	not_enough_memory("CONFIG_INIT");

    config->state = ConfigEmpty;

#ifdef _WIN32
    config->home = strdup("C:\\");
    config->copyright = strdup(DEFAULT_COPYRIGHT_OWNER);
    config->state = ConfigInitialized;  /* Don't try to read an rcfile. */
#else
    pw = getpwuid(uid);

    if (pw && pw->pw_dir)
	config->home = strdup(pw->pw_dir);
    else
    {
	if (getenv("HOME") != NULL)
	    config->home = strdup(getenv("HOME"));
	else
	    config->home = strdup("");
    }

    if (getenv("COPYRIGHT_OWNER") != NULL)
      config->copyright = strdup(getenv("COPYRIGHT_OWNER"));
    else
      config->copyright = strdup(DEFAULT_COPYRIGHT_OWNER);

#endif
    config->completion = FALSE;
}


static int StringToBoolean(char *value, char *filename, int linenum)
{
    if ((strncasecmp(value, "on", 2) == 0) || (strncasecmp(value, "true", 4) == 0))
	return(TRUE);
    else if ((strncasecmp(value, "off", 3) == 0) || (strncasecmp(value, "false", 5) == 0))
	return(FALSE);
    else
	fsm_printf("\"%s\", line %d: Invalid Boolean keyword '%s': must be one of 'on', 'off', 'true' or 'false'\n", filename, linenum, value);
	return(-1);
}

void fsm_read_rcfile()
{
#define MAX_FILENAME_LEN 512
    char filename[MAX_FILENAME_LEN];
#define MAX_LINE_LEN 512
    char line[MAX_LINE_LEN];
    FILE *fd;
    int linenum;

    if (config == NULL)
	fsm_config_init();

    /* With #ifdef _WIN32, config->state is ConfigInitialized at this point. */

    if (config->state == ConfigInitialized)
	return;


#ifndef UNDER_CE
    /* look for an .fsmrc file in the HOME dir */
    sprintf(filename, "%s/.%s", config->home, RCFILENAME);
    if ((fd = fopen(filename, "r")) == NULL)
    {
	/* try in $FSMTOP/lib/ */
	char *fsm_home = getenv("FSMHOME");
	if (fsm_home == NULL)
	{
	    /* try in /usr/local/ ? */
	    sprintf(filename, "/usr/local/lib/%s", RCFILENAME);
	    fd = fopen(filename, "r");
	}
	else
	{
	    sprintf(filename, "%s/lib/%s", fsm_home, RCFILENAME);
	    fd = fopen(filename, "r");
	}
	if (fd == NULL) /* Can't find a valid file */
          {
            config->state = ConfigInitialized;
            return;
          }
    }
    linenum = 0;
    while (fsm_fgets(line, MAX_LINE_LEN, fd) != NULL)
    {
	linenum++;
	if (line[0] != '#')
	{
	    char *tok = line;
	    char *value = NULL;
	    char *pt = line;
	    int line_len = strlen(line);

				/* remove last NL */
	    if (line_len > 0 && line[line_len - 1] == '\n')
		line[line_len - 1] = '\0';

	    while ((*pt == ' ') || (*pt == '\t'))
		pt++;
	    if (*pt != '\0')
	    {
		for (; *pt != '\0'; pt++)
		    if ((*pt == ' ') || (*pt == '\t') || (*pt == ':') || (*pt == '='))
		    {
			char *end_tok = pt;
				/* found end of keyword, find value */
			while ((*pt == ' ') || (*pt == '\t'))
			    pt++;

			if ((*pt == ':') || (*pt == '='))
			{
			    pt++;
			    while ((*pt == ' ') || (*pt == '\t'))
				pt++;
			    *end_tok = '\0';
			    value = pt;
			    break;
			}
		    }
		if (value == NULL)
		    fsm_printf("\"%s\", line %d: syntax error at : %s\n", filename, linenum, line);
		else
		{
		    if (strcmp(tok, "copyright") == 0)
		    {
			if (config->copyright != NULL)
			    free((void *)config->copyright);
			config->copyright = strdup(value);
		    }
		    else if (strcmp(tok, "completion") == 0)
		    {
			int newval = StringToBoolean(value, filename, linenum);
			if (newval != -1)
			    config->completion = newval;
		    }
/* Don't warn anymore
		    else
                    fsm_printf("\"%s\", line %d: Invalid keyword '%s': must be one of 'copyright' or 'completion'\n", filename, linenum, tok);
                    */
		    
/*	else if (strcmp(tok, "") == 0)
	config-> = strdup(value);
	*/
		}
	    }
	}
    }
    config->state = ConfigInitialized;
#else
    config->state = ConfigInitialized;
	return;
#endif
}

char *fsm_copyright(char *new_owner)
{
  /* When #ifdef _WIN32, fsm_read_rcfile() just initializes the config
     structure without attempting to read an rcfile. */

    if (config == NULL || config->state == ConfigEmpty)
      fsm_read_rcfile();

    if (new_owner)
      {
        free(config->copyright);
        config->copyright = strdup(new_owner);
      }

    return(config->copyright);
}
#endif /* SLIM_FST */
