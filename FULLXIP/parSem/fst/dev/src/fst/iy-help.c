/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: iy-help.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "fst/fst-types.h"
#include "fst/comm-def.h"
#include "fst/context.h"
#include "fst/iy-help.h"
#include "fst/warning.h"
#include "fst/interface.h"
#include "fst/header.h"
#include "fst/config.h"

#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif


extern char * help_header;
extern char *  CommandClasses[];

enum value_types{OnOff = 0, Integer =1, Filename=2, License=3,
		 CharEncoding=4, Copyright=5};

static int value_type(struct command *command)
{
  if (strstr(command->state, "ON_OR_OFF"))
    return(OnOff);
  else if (strstr(command->state, "NRO"))
    return(Integer);
  else if (strstr(command->state, "FILENAME"))
    {
      if (strstr(command->brief, "copyright owner"))
        return(Copyright);
      else
        return(Filename);
    }
  else if (strstr(command->state, "LICENSE_TYPE"))
    return(License);
  else if (strstr(command->state, "CHAR_ENCODING"))
    return(CharEncoding);
  else
    assert(0);
  return(0);
}

static char *on_off(struct command *command)
{
  return(*command->value ? "ON" : "OFF");
}

void iy_print_brief()
{
  int c = 0, cold, cc, first_command_of_class = TRUE;
  struct command *comm;

  comm=commands;
  printf ("%s",help_header);

#ifndef WORLD /* VIRT_NET commands are not available to WORLD */
  for ( cc = COMM1 ; cc <= VIRT_NET ; cc++ )
#else
  for ( cc = COMM1 ; cc <= COM_NET ; cc++ )
#endif /* WORLD */
    {
       printf("\n\nCOMMAND CLASS: %s\n\n", CommandClasses[cc]);
       comm=commands;
       c = 0;
       first_command_of_class = TRUE;
       while  (comm->command)
	 {
	   cold = c;

	   if ( comm->class == cc )
	     {
	       if ( ! first_command_of_class && (c != 0))
		 c+=printf(", ");
	       first_command_of_class = FALSE;
	       c+=printf("%s",comm->command);
	     }
	   comm++;
	   /* if ((cold != c) && comm->command) c+=printf(", "); */
	   if (c > (WIN_LEN-20))
	     {
	       if ( ! first_command_of_class && (c != 0))
		 c+=printf(",");
	       printf("\n");
	       c=0;
	     }
	 }
    }
  printf("\n\n");
}

void iy_print_usage(struct command *comm)
{
  char *p, buf[1000]; 
#ifdef INFO
  char *al_b;	  
#endif	  

  if (comm)
    {
      if (!strncmp(comm->command,"variable",8))
	{
	  printf("%s == %s\n",comm->command,comm->usage);
	}
      else if (!strncmp(comm->command,"regex:",6))
	{
	  printf("\t%s (%s)\n",comm->command,comm->usage);
	}
      else
	{
	  printf("usage");
	  strcpy(buf,comm->usage);
#ifdef INFO
	  if ( comm && comm->command && comm->coding &&
	       ( al_b = strpbrk (comm->coding, "|")))
	    {
	      printf(" (alias:\t");
	      for ( al_b++ ; *al_b != 0; al_b++)
		{
		  if (*al_b == '|')
		    printf(", ");
		  else
		  if (*al_b != '$')
		    putc (*al_b, stdout);
		}
	       printf(")");
	    }	       
#endif	  
	  printf(":\n");
	  p=strtok(buf,"|");
	  do {
	    printf("\t%s %s\n",comm->command,(p ? p : ""));
	    p=strtok(NULL,"|");
	  } while (p);
	}
    }
}

void iy_print_help(struct command *command)
{
  if (command)
    {
      iy_print_usage(command);
      if (command->help)
	{
	  if (command->value)
	    {
	      printf("\n%s Current value is ", command->help);
	      switch(value_type(command))
		{
		case OnOff:
		  printf("%s.", on_off(command));
		  break;
		case Integer:
		printf("%d.", *command->value);
		  break;
		case License:
		  printf("%s.", decode_license_type(*command->value));
		  break;
		case CharEncoding:
		  printf("%s.", decode_char_encoding(*command->value));
		  break;
                case Copyright:
                  printf("\"%s\".", fsm_copyright(NULL));
                  break;
		default: /* Filename */
		system("pwd");
		  break;
		}

	      printf("\n\n");
	    }
	  else
	    printf("\n%s\n\n",command->help);
	}
    }
  else
    {
      print_no_help("that command");
    }
}

int iy_print_apropos(char *p, int mess_p)
{
  int i = 0;
  int terse_p;
  int variable_p;
  struct command *c;

  if (p)
    {
      /* To allow for a common error. */

      if (strcmp(p, "variables") == 0)
	{
	  p = "variable";
	  terse_p = TRUE;
	}
      else 
	terse_p = strcmp(p, "variable") == 0;
      
      c=commands;
      while(c->command)
	{
	  if (c && (c->brief) &&
	      (strstr(c->command,p) || (mess_p && strstr(c->brief,p))))
	    {
	      if (c->brief)
		{
                  int len;
                  char *name;

		  if (terse_p)
		    {
		      
		      /*  Omit redundant 'variable' in responding to
			  'help variable, or 'apropos <variable>' */

		      if (strncmp("variable ", c->command, 9) == 0)
			{
			  name = (c->command + 9);
			  variable_p = TRUE;
			}
		      else
			{
			  name = c->command;
			  variable_p = FALSE;
			}

		      len = strlen(name);

		      if (variable_p && c->value)
			{
			  switch(value_type(c))
			    {
			    case OnOff:
			      if (len < 8)
				printf("%s\t\t :  %s\t: %s\n", name,
				       on_off(c), c->brief);
			      else if (len < 16)
				printf("%s\t :  %s\t: %s\n", name,
				       on_off(c), c->brief);
			      else
				printf("%s :  %s\t: %s\n", name,
				       on_off(c), c->brief);
			      break;
			    case Integer:
				{
				  if (len < 8)
				    printf("%s\t\t :  %d\t: %s\n", name,
					   *c->value, c->brief);
				  else if (len < 16)
				    printf("%s\t %d\t : %s\n", name,
					   *c->value, c->brief);
				  else
				    printf("%s :  %d\t: %s\n", name,
					   *c->value, c->brief);
				}
			      break;
			    case License:
			      printf("%s\t : %s : %s\n", name,
				     decode_license_type(*c->value),
				     c->brief);
			      break;
			    case CharEncoding:
			      printf("%s\t : %s : %s\n", name,
				     decode_char_encoding(*c->value),
				     c->brief);
			      break;
                            case Copyright:
			      printf("%s\t : %s\n", name, fsm_copyright(NULL));
                              break;
			    default: /* Filename */
			      printf("%s\t :  ", name);
			      system("pwd");
			      break;
			    }
			}
		      else if (len < 8)
			printf("%s\t\t\t:  %s\n", name ,c->brief);
		      else if (len < 16)
			printf("%s\t\t:  %s\n", name ,c->brief);
		      else
			printf("%s\t:  %s\n", name ,c->brief);
		    }
		  else
                    {
                      if (strncmp("regex:", c->command, 6) == 0)
                        {
                          name = c->command;
                          len = strlen(name);
                          
                          if (len < 16)
                            printf("%s\t\t:  %s\n",c->command,c->brief);
                          else
                            printf("%s\t:  %s\n",c->command,c->brief);
                        }
                      else
                        printf("%s\t:  %s\n",c->command,c->brief);
                    }
		
		  i++;
		}
	    }
	  c++;
	}
      return i;
    }
  else
    {
      iy_print_brief();
      return -1;
    }
}


struct command *iy_find_variable(char *name)
{
  struct command *c;
  char buf[50];
  char *var_name;

  /* Variable names all start with the string "variable"
     in the command structures. If the function is
     called with a short name such as "print-space"
     we first make it "variable print-space".
  */

  if (strstr(name, "variable"))
    var_name = name;
  else
    {
      strcpy(buf, "variable ");
      strncpy(buf+9, name, (size_t) 40);
      var_name = buf;
    }

 c = commands;
  while  (c && c->command)
    {
	   if (strcmp(c->command, var_name) == 0)
	     return(c);
	   c++;
    }

  return(NULL);
}

int iy_set_variable(char *name, void *value)
{
  /* Return values:
     0 == success
     1 == command not found
     2 == value of wrong type
  */

  struct command *c = iy_find_variable(name);

  if (c == NULL)
    return(1);
  
  switch(value_type(c))
    {
    case OnOff:
      if (strncasecmp((char *) value, "ON",3) == 0)
	*c->value = 1;
      else if (strncasecmp((char *) value, "OFF",4) == 0)
	*c->value = 0;
      else
	return(2);
      break;
    case Integer:
      *c->value = *((int *) value);
      break;
    case Filename:
      *c->value = *((char *) value);
      break;
    case Copyright:
      fsm_copyright((char *) value);
      break;
    default:
      assert(0);
      break;
    }

  return(0);
}
