/* $Id: dot-net-io.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* Copyright (c) 2001 by the Xerox Corporation.  All rights reserved */

/*************************************************************
 *
 * FILE "dot-io.c"
 *
 * Interface for visualizing networks with the DOT tool of
 * Saarbruecken University.  
 *
 * Tamas Gaal, XEROX RESEARCH CENTRE EUROPE, Grenoble, France
 * 2001 Dec
 */

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _WIN32
#include <unistd.h>
#include <strings.h>
#endif

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */

#include "fst/modify.h"
#include "fst/mark.h"
#include "fst/rename.h"
#include "fst/pretty-p.h"
#include "fst/prompt.h"
#include "fst/net-io.h"
#include "fst/fsm_io.h"
#include "fst/dot-net-io.h"
#include "fst/types.h"
#include "fst/cfsm.h"
#include "fst/utf8_fatstr_util.h"

#define DOT_BORDERWIDTH_FINAL 3
#define DOT_BORDERWIDTH_NONFINAL 2
#define DOT_BUFFER_SIZE 1000
#define DOT_RIGHT_MARGIN 999

static void dot_print_buf_escaped_doubled(char * buf,  FILE *stream)
     /* DOT may choke on \ and " so they are escaped as \\ and \"
	Clean buffer afterwards. */
{
  int i = 0;

  while (buf[i])
    {
      switch (buf[i])
	{
	case '\\':
	case '"':
          if (buf[i]=='\\')
            {
              fsm_putc('\\', stream);
              fsm_putc('\\', stream);
            }
	  fsm_putc('\\', stream);
          /* break; */
	default:
	   fsm_putc(buf[i], stream);
	   break;
	}
      i++;
    }
  i = 0;
  while (i < DOT_BUFFER_SIZE)
    buf[i++] = 0;
}

static void print_dot_state_s(STATEptr state, int rm, FILE *stream, char * wbuf)
{
  ARCptr sarc, sarc2;
  long int name=0;
  STATEptr sarc_dest;

  if (! state)
    return;

  name = (long int) STATE_client_cell(state);
  
  if (STATE_final(state))
    fsm_putc('f', stream);
  fsm_putc('s', stream);
  fsm_fprintf(stream, "%d", name);
  if (STATE_final(state))
    {
      fsm_fprintf(stream, " [shape=doublecircle, label=\"fs%d\"]",name);
    }
  fsm_fprintf(stream, ";\n"); 

  for (sarc=STATE_arc_set(state) ; sarc; sarc=next_arc(sarc))
    {
      if (ARC_visit_mark(sarc) == NOT_VISITED)
	{
	  if (STATE_final(state))
	    fsm_putc('f', stream);
	  fsm_putc('s', stream);
	  fsm_fprintf(stream, "%d -> ", name);
	  sarc_dest = ARC_destination(sarc);
	  if (STATE_final(sarc_dest))
	    fsm_putc('f', stream);
	  fsm_putc('s', stream);
	  fsm_fprintf(stream, "%d", (long int) STATE_client_cell(sarc_dest));
	  fsm_fprintf(stream, " [label=\"");
	  if (-1 == FST_copy_label_name(ARC_label(sarc), BOTH_SIDES, wbuf, DOT_BUFFER_SIZE, TRUE, DO_ESCAPE))
	    {
	      fsm_fprintf (stderr, "\nXFST-DOT interface error. Too small buffer=DOT_BUFFER_SIZE.\nPlease report this.\n");
	    }
	   dot_print_buf_escaped_doubled(wbuf, stream);

	  ARC_visit_mark(sarc) = IN_PROCESS;

	  for (sarc2=STATE_arc_set(state) ; sarc2; sarc2=next_arc(sarc2))
	    {
	      if (ARC_visit_mark(sarc2) == NOT_VISITED && 
		  ARC_destination(sarc2) == sarc_dest)
		{
		  fsm_fprintf(stream, "\\n");
		  if (-1 == FST_copy_label_name(ARC_label(sarc2), BOTH_SIDES, wbuf, DOT_BUFFER_SIZE, TRUE, DO_ESCAPE))
		    {
		      fsm_fprintf (stderr, "\nXFST-DOT interface error. Too small buffer=DOT_BUFFER_SIZE.\nPlease report this.\n");
		    }
		  dot_print_buf_escaped_doubled(wbuf, stream);

		  ARC_visit_mark(sarc2) = IN_PROCESS;
		}
	    }
	      
	  fsm_fprintf(stream, "\"];\n"); 
	}
    }
}

static void print_dot_states_s(NETptr net, int rm, FILE *stream, char * wbuf)
{
  STATEptr cur;
  
  for (cur = NET_states(net); cur != NULL; cur = next_state(cur))
    print_dot_state_s(cur, rm, stream, wbuf);
}

static void print_dot_uncompacted_net(NETptr net, int rm, FILE *stream, char * wbuf)
{
  /* rename_states(net); */
  top_sort_net(net);
  reorder_states(net, state_num_less_p);

  print_dot_states_s(net, rm, stream, wbuf);
 fsm_fprintf(stream, "}\n"); 
}

static void print_fat_to_thin_buf_dot_escaped( FAT_STR fs, char * str_buf,  FILE *stream)
{
  str_buf[0] = '\0';
  FST_fat_to_thin(fs, str_buf);
  dot_print_buf_escaped_doubled(str_buf, stream); 
}

static void print_dot_net_s(NETptr net, int rm, FILE *stream)
{
  char *str_buf = (char *) calloc((size_t) DOT_BUFFER_SIZE+1, sizeof(char));
  OBJECTptr name = get_value(net, NETWORKNAME_ATTR);

  clear_arc_marks(net);
  clear_state_marks(net);

  fsm_fprintf(stream, "/* Net: ");
  if (name && OBJ_type(name) == String)
    {
      print_fat_to_thin_buf_dot_escaped(OBJ_str(name), str_buf, stream);
    }
  fsm_fprintf(stream, " */\n");

  fsm_fprintf(stream, "\
digraph ");
  if (name && OBJ_type(name) == String)
    {
      /** print_fat_to_thin_buf_dot_escaped(OBJ_str(name), str_buf, stream); -- dot too senstive to some characters like | < >**/
      fsm_fprintf(stream, "named");
    }
  else
     fsm_fprintf(stream, "no_name");
  fsm_fprintf(stream, " {");
  fsm_fprintf(stream, "\n\
rankdir = LR ;\n\
orientation=landscape ;\n\
ranksep =\".15\" ;\n\
/* default shape for nodes */\n\
node[shape=circle,height=\".25\"] ;\n\
\n");

  if (name && OBJ_type(name) == String)
    {
      fprintf(stream, "netname [shape=polygon, sudes=4, label=\"");
      print_fat_to_thin_buf_dot_escaped(OBJ_str(name), str_buf, stream);
      fprintf(stream, "\"];\n");
    }
  print_dot_uncompacted_net(net, rm, stream, str_buf);
  clear_arc_marks(net);
  free(str_buf);
}

void print_dot_net(NETptr net, FILE *stream)
{
print_dot_net_s(net, DOT_RIGHT_MARGIN, stream);
}

#define DOT_COMMAND_BUFFER_LENGTH 256
#define DOT_TAIL_SECURE_LENGTH 2
#define DOT_COMMAND_NAME_PLUS_ONE 9
#define MAX_DOT_NET_STATES 500
#define DOT_SLEEP_SECONDS 1
#define DOT_TEMP_DIR_DEFAULT "/tmp"

#ifndef _WIN32
void db_dot (NETptr net)
{
  dot_to_file_or_to_window (stdout, net, TRUE, FALSE);
}

void dot_to_file_or_to_window(FILE *f, NETptr net, int interactive, int verbose)
{
  FILE *dot_file;
  char dot_text[DOT_COMMAND_BUFFER_LENGTH]="dot -Tps ", *dot_temp_n=NULL, *dot_temp_dir=NULL;
  int dot_tmp_name_length=0, dot_tmpname_ptr=0, dot_buf_ptr=0, dot_print_go_ahead=TRUE, dot_file_descr=0, ps_tmpname_ptr=0;

  if (f != stdout)
    print_dot_net(net,f);
  else
    {
      if (interactive & (NET_num_states(net) > MAX_DOT_NET_STATES))
	{
	  if (yes_or_no("** Shall DOT display this big net?", 'n'))
	    dot_print_go_ahead = TRUE;
	  else
	    dot_print_go_ahead = FALSE;
	}

      if (dot_print_go_ahead)
	{
	  dot_text[DOT_COMMAND_NAME_PLUS_ONE]=' ';

	  dot_temp_dir=getenv ("TMPDIR");
	  if (!dot_temp_dir)
	    dot_temp_dir=DOT_TEMP_DIR_DEFAULT;
		  
	  if ((dot_temp_n=tempnam(dot_temp_dir,"dot_")))
	    {
	      dot_tmp_name_length = strlen(dot_temp_n);

	      if (dot_tmp_name_length < DOT_COMMAND_BUFFER_LENGTH-DOT_TAIL_SECURE_LENGTH-DOT_COMMAND_NAME_PLUS_ONE)
		{
		  strcpy(dot_text+DOT_COMMAND_NAME_PLUS_ONE, dot_temp_n);
		  dot_buf_ptr = DOT_COMMAND_NAME_PLUS_ONE+dot_tmp_name_length;
		
#ifdef DEBUG
		  print_comment(verbose, "* 1. tmpnam() name (between \"\"): \"%s\".\n", dot_temp_n); /* DEBUG */
#endif

		  dot_file_descr = creat(dot_text+DOT_COMMAND_NAME_PLUS_ONE, 0xffffffff);

		  if (dot_file_descr == -1)
		    {
		      print_comment(verbose, "**  creat() failed for DOT temporary file. Name is\"%s\".\n", dot_text+DOT_COMMAND_NAME_PLUS_ONE);
		      print_comment(verbose, "**  creat() errno=%d.\n", errno);
		    }
		  else
		    {
		      if (!(dot_file = fdopen(dot_file_descr , "w")))
			print_comment(verbose, "**  fdopen() failed for DOT temporary file. Name is\"%s\" errno=%d.\n", dot_text+DOT_COMMAND_NAME_PLUS_ONE, errno);
			    

		      errno = 0; /* ?? */
		      print_dot_net(net, dot_file);

		      if (fsync(dot_file_descr)) /** Try synchronizing output ? **/
			print_comment(verbose, "** Unsuccessful fsync() on DOT temporary file; errno=%d.\n", errno);

		      if (fclose(dot_file))
			print_comment(verbose, "** Unsuccessful DOT temporary file close; errno=%d.\n", errno);
			  
                      dot_text[dot_buf_ptr] = '&'; 
#ifdef DEBUG
		      strncpy(dot_text, "cat      ", 9); 
		      system(dot_text);
		      strncpy(dot_text, "dot -Tps ", 9); 
#endif
                      strncpy(dot_text+dot_buf_ptr, " -o ", 4);
                      dot_buf_ptr+=4;
                      dot_tmpname_ptr=DOT_COMMAND_NAME_PLUS_ONE;
                      strncpy(dot_text+dot_buf_ptr, dot_temp_n, dot_tmp_name_length);
                      ps_tmpname_ptr=dot_buf_ptr;
                      dot_buf_ptr=dot_buf_ptr+dot_tmp_name_length;
                      strncpy(dot_text+dot_buf_ptr, ".ps ; gv ", 9);
                      dot_buf_ptr+=9;
                      strncpy(dot_text+dot_buf_ptr, dot_temp_n, dot_tmp_name_length);
                      dot_buf_ptr+=dot_tmp_name_length;
                      strncpy(dot_text+dot_buf_ptr, ".ps &",5);
#ifdef DEBUG
		      print_comment(verbose, "* System call: \"%s\".\n", dot_text); /* DEBUG */
#endif
		      system(dot_text);
		      dot_text[dot_buf_ptr]=0;
		      sleep(DOT_SLEEP_SECONDS); /* Important. Otherwise remove() removes it too
						   quickly and so the previous system() won't
						   find it! */
                      dot_text[dot_tmpname_ptr+dot_tmp_name_length]=0;
                      dot_text[ps_tmpname_ptr+dot_tmp_name_length+3]=0;
		      remove(dot_text+dot_tmpname_ptr);
                      /* remove(dot_text+ps_tmpname_ptr); */
                      print_comment(verbose, "Please delete: \"%s\" if not needed any more.\n", dot_text+ps_tmpname_ptr);
		      fprintf(stdout,"\n");
		    }
		}
	      else 
		print_comment(verbose, "** DOT temporary file name too long.\n");
	    }
	  else
	    print_comment(verbose, "** Unsuccessful DOT temporary file creation.\n");
	  free(dot_temp_n);
	}
    }
}
#endif

#endif /* SLIM_FST */
