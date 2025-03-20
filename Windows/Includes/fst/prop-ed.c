/* $Id: prop-ed.c,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1994  by the Xerox Corporation.  All rights reserved */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "fst/struct.h"
#include "fst/prompt.h"
#include "fst/pretty-p.h"
#include "fst/prop-ed.h"
#include "fst/fsm_io.h"
#include "fst/types.h"
#include "fst/utf8_fatstr_util.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

/******************************************************
 **
 **  PROP-ED.C
 **
 **  Lauri Karttunen, RXRC 1993
 **  
 **  Functions for interactive editing network properties
 **
 ******************************************************/
 
/**********************************
 *                                *
 *  Network Property List Editor  *
 *                                *
 **********************************/

void print_prop_names(NETptr net)
{
  PROPptr prop;
  int i;
  
  if (NET_properties(net) == NULL)
    fsm_fprintf(stdout, "(no props)\n");
  else for (prop = NET_properties(net), i = 1; 
            prop != NULL; 
            prop = next_prop(prop), i++)
    {
      fsm_printf("%d. ", i);
      print_fat_string(PROP_attr(prop));
      fsm_printf("\n");
    }
}

int PE_check_range(NETptr net, int num)
{
  PROPptr prop;
  int count;
  
  for (prop = NET_properties(net), count = 1; 
       prop != NULL; 
       prop = next_prop(prop), count++);
  
  if (num >= 1 && num < count)
    return (1);
  else
    {
      fsm_fprintf(stderr, "%%%% number out of range [1..%d]\n", count-1);
      return (0);
    }
}

static int PE_no_attr_conflict(NETptr net, FAT_STR attr)
{
  PROPptr prop;
  int count;
  
  for (prop = NET_properties(net), count = 1; 
       prop != NULL; 
       prop = next_prop(prop), count++)
    {
      if (fat_strcmp(PROP_attr(prop), attr) == 0)
        {
          fsm_fprintf(stderr, "%%%% That name already exists. ");
          fsm_fprintf(stderr, "Choose another name or try MODIFY.\n");
          return (0);
        }
    }
  return (1);
  
}

static int is_int(char *str)
{
  if (*str == '-') /* allow for negative ints */
    str++;
  
  for (; *str; str++)
    {
      if (!isdigit((int) *str))
        return (0);
    }
  return (1);
}

static FAT_STR get_attr(NETptr net, int num)
{
  PROPptr prop;
  int i;
  
  for (prop = NET_properties(net), i = 1;
       prop != NULL;
       prop = next_prop(prop), i++)
    {
      if (i == num)
        return(PROP_attr(prop));
    }
  return(NULL);
}

void PE_menu(void)
{
  fsm_fprintf (stdout, "\nChoose one of the following:\n");
  fsm_fprintf (stdout, "    1. ADD prop\n");
  fsm_fprintf (stdout, "    2. DELETE prop\n");
  fsm_fprintf (stdout, "    3. MODIFY prop\n");
  fsm_fprintf (stdout, "    4. PRINT prop\n");
  fsm_fprintf (stdout, "    0. EXIT prop editor\n\n");
}

void PE_add_prop (NETptr net)
{
  FAT_STR attr;
  char def = '1';
  
  word = get_input("Type name of new property [cancel]:", FALSE, "\n");
  if (word == NULL)
    return;
  else
    {
      attr = FST_thin_to_fat(word);
      if (PE_no_attr_conflict(net, attr))
        {
          word = get_input("Value type (1 = string, 2 = integer) [string]:",
			   FALSE, "\n");
          if (word == NULL)
            word = &def;
          
          switch (word[0])
            {
            case ('1'):
              word = get_input("Type string value:", TRUE, "\n");
              add_string_prop(net, attr, FST_thin_to_fat(word));
              break;
            case ('2'):
              word = get_input ("Type integer value:", TRUE, "\n");
              if (is_int(word))
		add_integer_prop(net, attr, atoi(word));
              else
                {
                  fsm_fprintf(stderr, "%%%% Value is not an integer!\n");
                  return;
                }
              break;
            default:
              fsm_fprintf (stderr, "%%%%invalid type\n");
              return;
            }
          fsm_printf("Done!\n");
        }
    }
}


void PE_delete_prop (NETptr net)
{
  int num;
  
  if (NET_properties(net) == NULL)
    {
      fsm_fprintf(stdout, "No props to delete!\n");
      return;
    }
  
  print_prop_names(net);
  word = get_input("Number of property to delete [cancel]:", FALSE, "\n");
  if (word == NULL)
    return;
  else
    {
      num = atoi(word);
      if (PE_check_range(net, num))
        {
          word = get_input("Are you sure? (y/n) [y]:", FALSE, "\n");
          if (word == NULL || word[0] != 'n')
            {
              remove_prop(net, get_attr(net, num));
              fsm_printf("Done!\n");
            }
          else
            fsm_fprintf(stderr, "Cancelled.\n");
        }
    }
}

void PE_modify_prop (NETptr net)
{
  int num, int_value;
  FAT_STR attr, str_value;
  char def = '1';
  
  if (NET_properties(net) == NULL)
    {
      fsm_fprintf(stdout, "No props to modify!\n");
      return;
    }
  
  print_prop_names(net);
  word = get_input("Number of property to modify [cancel]:", FALSE, "\n");
  if (word == NULL)
    return;
  else
    {
      num = atoi(word);
      if (PE_check_range(net, num))
        {
          attr = get_attr(net, num);
          
          word = get_input("Value type (1 = string, 2 = integer) [string]:",
			   FALSE, "\n");
          if (word == NULL)
            word = &def;
          
          switch (word[0])
            {
            case ('1'):
              word = get_input("string value:", TRUE, "\n");
              str_value = FST_thin_to_fat(word);
              word = get_input("Are you sure? (y/n) [y]:", FALSE, "\n");
              if (word == NULL || word[0] != 'n')
                {       
                  remove_prop(net, attr);
                  add_string_prop(net, attr, str_value);
                }
              else
                fsm_fprintf(stderr, "Cancelled.\n");
              break;
              break;
            case ('2'):
              word = get_input ("integer value", TRUE, "\n");
              if (is_int(word))
                {
                  int_value = atoi(word);
                  word = get_input("Are you sure? (y/n) [y]:", FALSE, "\n");
                  if (word == NULL || word[0] != 'n')
                    {
                      remove_prop(net, attr);
                      add_integer_prop(net, attr, int_value);
                    }
                  else
                    fsm_fprintf(stderr, "Cancelled.\n");
                }
              else
                {
                  fsm_fprintf(stderr, "%%%% Value is not an integer!\n");
                  return;
                }
              break;
            default:
              fsm_fprintf (stderr, "%%%%invalid type\n");
              return;
            }
          fsm_printf("Done!\n");
        }
    }
}

void PE_print_prop (NETptr net)
{
  int num;
  FAT_STR attr;
  
  if (NET_properties(net) == NULL)
    {
      fsm_fprintf(stdout, "No props to print!\n");
      return;
    }
  
  print_prop_names(net);
  word = get_input("Number of property to print [cancel]:", FALSE, "\n");
  if (word == NULL)
    return;
  
  num = atoi(word);
  if (PE_check_range(net, num))
    {
      attr = get_attr(net, num);
      print_fat_string(attr);
      fsm_fprintf(stdout, ":");
      object_out (get_value(net, attr), 78, 0);
    }
}

#endif /* SLIM_FST */
