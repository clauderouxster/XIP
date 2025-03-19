/* $Id: vcg-net-io.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */

/*************************************************************
 *
 * FILE "vcg-io.c"
 *
 * Interface for visualizing networks with the VCG tool of
 * Saarbruecken University.  
 *
 * Tamas Gaal, XEROX RESEARCH CENTRE EUROPE, Grenoble, France
 * 1999-03 
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
#include "fst/vcg-net-io.h"
#include "fst/types.h"
#include "fst/utf8_fatstr_util.h"

#define VCG_BORDERWIDTH_FINAL 3
#define VCG_BORDERWIDTH_NONFINAL 2
#define VCG_BUFFER_SIZE 1000
#define VCG_RIGHT_MARGIN 999

static void vcg_print_buf_escaped_doubled(char * buf,  FILE *stream)
     /* VCG chokes on \ and " so they are escaped as \\ and \"
	Clean buffer afterwards. */
{
  int i = 0;

  while (buf[i])
    {
      switch (buf[i])
	{
	case '\\':
	case '"':
	  fsm_putc('\\', stream);
	default:
	   fsm_putc(buf[i], stream);
	   break;
	}
      i++;
    }
  i = 0;
  while (i < VCG_BUFFER_SIZE)
    buf[i++] = 0;
}

static void print_vcg_state_s(STATEptr state, int rm, FILE *stream, char * wbuf)
{
  ARCptr sarc, sarc2;
  long int name=0;
  STATEptr sarc_dest;

  if (! state)
    return;

  name = (long int) STATE_client_cell(state);
  
  fsm_fprintf(stream, "\tnode: {borderwidth: ");
  if (STATE_final(state))
    fsm_fprintf(stream, "%d", VCG_BORDERWIDTH_FINAL);
  else
      fsm_fprintf(stream, "%d", VCG_BORDERWIDTH_NONFINAL);
  fsm_fprintf(stream, " title: \"");
  if (STATE_final(state))
    fsm_putc('f', stream);
  fsm_putc('s', stream);
  fsm_fprintf(stream, "%d", name);
  fsm_fprintf(stream, "\"}\n"); 

  for (sarc=STATE_arc_set(state) ; sarc; sarc=next_arc(sarc))
    {
      if (ARC_visit_mark(sarc) == NOT_VISITED)
	{
	  fsm_fprintf(stream, "\tedge: {sourcename: \"");
	  if (STATE_final(state))
	    fsm_putc('f', stream);
	  fsm_putc('s', stream);
	  fsm_fprintf(stream, "%d", name);
	  fsm_fprintf(stream, "\" targetname: \"");
	  sarc_dest = ARC_destination(sarc);
	  if (STATE_final(sarc_dest))
	    fsm_putc('f', stream);
	  fsm_putc('s', stream);
	  fsm_fprintf(stream, "%d", (long int) STATE_client_cell(sarc_dest));
	  fsm_fprintf(stream, "\" label: \"");
	  if (-1 == FST_copy_label_name(ARC_label(sarc), BOTH_SIDES, wbuf, VCG_BUFFER_SIZE, TRUE, DO_ESCAPE))
	    {
	      fsm_fprintf (stderr, "\nXFST-VCG interface error. Too small buffer=VCG_BUFFER_SIZE.\nPlease report this.\n");
	    }
	  vcg_print_buf_escaped_doubled(wbuf, stream);

	  ARC_visit_mark(sarc) = IN_PROCESS;

	  for (sarc2=STATE_arc_set(state) ; sarc2; sarc2=next_arc(sarc2))
	    {
	      if (ARC_visit_mark(sarc2) == NOT_VISITED && 
		  ARC_destination(sarc2) == sarc_dest)
		{
		  fsm_fprintf(stream, "\\n");
		  if (-1 == FST_copy_label_name(ARC_label(sarc2), BOTH_SIDES, wbuf, VCG_BUFFER_SIZE, TRUE, DO_ESCAPE))
		    {
		      fsm_fprintf (stderr, "\nXFST-VCG interface error. Too small buffer=VCG_BUFFER_SIZE.\nPlease report this.\n");
		    }
		  vcg_print_buf_escaped_doubled(wbuf, stream);

		  ARC_visit_mark(sarc2) = IN_PROCESS;
		}
	    }
	      
	  fsm_fprintf(stream, "\"}\n"); 
	}
    }
}

static void print_vcg_states_s(NETptr net, int rm, FILE *stream, char * wbuf)
{
  STATEptr cur;
  
  for (cur = NET_states(net); cur != NULL; cur = next_state(cur))
    print_vcg_state_s(cur, rm, stream, wbuf);
}

static void print_vcg_uncompacted_net(NETptr net, int rm, FILE *stream, char * wbuf)
{
  /* rename_states(net); */
  top_sort_net(net);
  reorder_states(net, state_num_less_p);

  print_vcg_states_s(net, rm, stream, wbuf);
}

static void print_fat_to_thin_buf_vcg_escaped( FAT_STR fs, char * str_buf,  FILE *stream)
{
  str_buf[0] = '\0';
  FST_fat_to_thin(fs, str_buf);
  vcg_print_buf_escaped_doubled(str_buf, stream); 
}

static void print_vcg_net_s(NETptr net, int rm, FILE *stream)
{
  char *str_buf = (char *) calloc((size_t) VCG_BUFFER_SIZE+1, sizeof(char));
  OBJECTptr name = get_value(net, NETWORKNAME_ATTR);

  clear_arc_marks(net);
  clear_state_marks(net);

  fsm_fprintf(stream, "/* Net: ");
  if (name && OBJ_type(name) == String)
    {
      print_fat_to_thin_buf_vcg_escaped(OBJ_str(name), str_buf, stream);
    }
  fsm_fprintf(stream, " */\n");

  fsm_fprintf(stream, "\
graph:\n\
{title: \"");
  if (name && OBJ_type(name) == String)
    {
      print_fat_to_thin_buf_vcg_escaped(OBJ_str(name), str_buf, stream);
    }
  fsm_fprintf(stream, "\"\n\
splines: yes\n\
display_edge_labels: yes\n\
orientation: left_to_right\n\
layoutalgorithm: mindepth\n\
crossing_weight: medianbary\n\
crossing_phase2: yes\n\
layout_nearfactor: 40\n\
late_edge_labels: yes\n");

  fsm_fprintf(stream, " node.shape: ellipse\n");
  fsm_fprintf(stream, "	edge.thickness: 2\n edge.arrowstyle: line\n");

  if (name && OBJ_type(name) == String)
    {
      fprintf(stream, "node: {borderwidth: 1 title: \"");
      print_fat_to_thin_buf_vcg_escaped(OBJ_str(name), str_buf, stream);
      fprintf(stream, "\" shape: box}\n");
    }
  print_vcg_uncompacted_net(net, rm, stream, str_buf);
  fsm_fprintf(stream, "}\n");
  clear_arc_marks(net);
  free(str_buf);
}

void print_vcg_net(NETptr net, FILE *stream)
{
print_vcg_net_s(net, VCG_RIGHT_MARGIN, stream);
}

#define VCG_COMMAND_BUFFER_LENGTH 40
#define VCG_TAIL_SECURE_LENGTH 2
#define VCG_COMMAND_NAME_PLUS_ONE 5
#define MAX_VCG_NET_STATES 500
#define VCG_SLEEP_SECONDS 1
#define VCG_TEMP_DIR_DEFAULT "/tmp"

#ifndef _WIN32
void vcg_to_file_or_to_window(FILE *f, NETptr net, int interactive, int verbose)
{
  FILE *vcg_file;
  char vcg_text[VCG_COMMAND_BUFFER_LENGTH]="xvcg ", *vcg_temp_n=NULL, *vcg_temp_dir=NULL;
  int vcg_tmp_name_length=0, vcg_print_go_ahead=TRUE, vcg_file_descr=0;

  if (f != stdout)
    print_vcg_net(net,f);
  else
    {
      if (interactive & (NET_num_states(net) > MAX_VCG_NET_STATES))
	{
	  if (yes_or_no("** Shall VCG display this big net?", 'n'))
	    vcg_print_go_ahead = TRUE;
	  else
	    vcg_print_go_ahead = FALSE;
	}

      if (vcg_print_go_ahead)
	{
	  vcg_text[VCG_COMMAND_NAME_PLUS_ONE]='x';

	  vcg_temp_dir=getenv ("TMPDIR");
	  if (!vcg_temp_dir)
	    vcg_temp_dir=VCG_TEMP_DIR_DEFAULT;
		  
	  if ((vcg_temp_n=tempnam(vcg_temp_dir,"vcg_")))
	    {
	      vcg_tmp_name_length = strlen(vcg_temp_n);

	      if (vcg_tmp_name_length < VCG_COMMAND_BUFFER_LENGTH-VCG_TAIL_SECURE_LENGTH-VCG_COMMAND_NAME_PLUS_ONE)
		{
		  strcpy(vcg_text+VCG_COMMAND_NAME_PLUS_ONE, vcg_temp_n);
		  vcg_tmp_name_length = VCG_COMMAND_NAME_PLUS_ONE+vcg_tmp_name_length;
		
#ifdef DEBUG
		  print_comment(verbose, "* 1. tmpnam() name (between \"\"): \"%s\".\n", vcg_temp_n); /* DEBUG */
#endif
		  /** There are 2 versions:
		    1. Use only streams - FILE * - fopen(), fclose() -
		    2. use numbered (Int) descriptors, too -  creat(), fdopen(), fsync()
		    (now the 2. is used but 1. is left here as a comment).
		    
		    if (!(vcg_file = fopen(vcg_text+VCG_COMMAND_NAME_PLUS_ONE, "w")))
		    print_comment(verbose, "**  fopen() failed for VCG temporary file. Name is\"%s\".\n", vcg_text+VCG_COMMAND_NAME_PLUS_ONE);
		    **/

		  vcg_file_descr = creat(vcg_text+VCG_COMMAND_NAME_PLUS_ONE, 0xffffffff);

		  if (vcg_file_descr == -1)
		    {
		      print_comment(verbose, "**  creat() failed for VCG temporary file. Name is\"%s\".\n", vcg_text+VCG_COMMAND_NAME_PLUS_ONE);
		      print_comment(verbose, "**  creat() errno=%d.\n", errno);
		    }
		  else
		    {
		      if (!(vcg_file = fdopen(vcg_file_descr , "w")))
			print_comment(verbose, "**  fdopen() failed for VCG temporary file. Name is\"%s\" errno=%d.\n", vcg_text+VCG_COMMAND_NAME_PLUS_ONE, errno);
			    

		      errno = 0; /* ?? */
		      print_vcg_net(net, vcg_file);

		      if (fsync(vcg_file_descr)) /** Try synchronizing output ? **/
			print_comment(verbose, "** Unsuccessful fsync() on VCG temporary file; errno=%d.\n", errno);

		      if (fclose(vcg_file))
			print_comment(verbose, "** Unsuccessful VCG temporary file close; errno=%d.\n", errno);
			  
		      vcg_text[vcg_tmp_name_length] = '&';
#ifdef DEBUG
		      strncpy(vcg_text, "cat ", 4); 
		      system(vcg_text);
		      strncpy(vcg_text, "xvcg", 4); 
		      print_comment(verbose, "* System call: \"%s\".\n", vcg_text); /* DEBUG */
#endif
		      system(vcg_text);
		      vcg_text[vcg_tmp_name_length]=0;
		      sleep(VCG_SLEEP_SECONDS); /* Important. Otherwise remove() removes it too
						   quickly and so the previous system() won't
						   find it! */							  
		      remove(vcg_text+VCG_COMMAND_NAME_PLUS_ONE);
		      fprintf(stdout,"\n");
		    }
		}
	      else 
		print_comment(verbose, "** VCG temporary file name too long.\n");
	    }
	  else
	    print_comment(verbose, "** Unsuccessful VCG temporary file creation.\n");
	  free(vcg_temp_n);
	}
    }
}
#endif

#endif /* SLIM_FST */
