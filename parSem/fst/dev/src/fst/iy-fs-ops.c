/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: iy-fs-ops.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

/* #include <sys/ddi.h> */
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "fst/fsm-package.h"
#include "fst/comm-def.h"
#include "fst/interface.h"
#include "fst/interface.lex.h"
#include "fst/re.lex.h"
#include "fst/iy-stack.h"
#include "fst/iy-fs-ops.h"
#include "fst/warning.h"
#include "fst/prompt.h"
#include "fst/net-io.h"
#include "fst/str-buf.h"
#include "fst/apply.h"
#include "fst/eqv-labs.h"
#include "fst/fsm_io.h"
#include "fst/twol-net.h"
#include "fst/prop-ed.h"
/* #include "fst/prop-io-lex.h" */
#include "fst/fst-types.h"
#include "fst/prop-io.h"
#include "fst/prolog.h"
#include "fst/bimach.h"
#include "fst/remove_epsilon.h"
#include "fst/flags.h"
#include "fst/define.h"
#include "fst/ambclass.h"
#include "fst/labpaste.h"
#include "fst/com-rep.h"
#include "fst/cfsm.h"
#include "fst/bin-out.h"
#include "fst/vcg-net-io.h"
#include "fst/dot-net-io.h"
#include "fst/lexicon-aux.h"
#include "fst/lexicon.lex.h"
#include "fst/share_arclist.h"
#include "fst/utf8_fatstr_util.h"

#ifndef WORLD
#include "fst/wprolog.h"
#include "fst/fsopt/test-compression.h"
#include "fst/virtual-fsm.h"
#ifdef HAVE_TCL
#include "fst/tk-inspect.h"
#endif /* HAVE_TCL */
#endif /* WORLD */

extern finfo *prolog_file_info;
extern finfo *interface_file_info;
extern int PROP_FILE_LINE_NUM;

#if (defined(MAC_OS) || defined(__MWERKS__))
#include <unix.h>
#include <cstring>
#include "codewarrior.h"

#ifndef _WIN32
#include <unistd.h>
#endif
#endif /* MAC_OS */

/****************************************************************
  DUMMY FUNCTION THAT NEVER SHOULD BE CALLED.
  PLEASE, REMOVE IT IF YOU KNOW HOW TO MODIFY ALL PLACES
  FROM WHERE IT IS CALLED.
  02/03/99 AK
 ****************************************************************/

int unambiguous_p (NETptr net, int input_side)
{
  fsm_printf
    ("Sorry, I don't know if your network is unambiguous on the %s side.\n",
     (input_side == UPPER) ? "upper" : "lower");

  return (FALSE);
}


/****************************************************************
  COMMAND: all fs ????
 ****************************************************************/
void iy_fs_all1_op(NETptr (*func)())
{
  NVptr nv;
  int i;
  if (iy_check_args(1, last_command->command) &&
      iy_can_do_on(STANDARD_NET, last_command->command, -1))
    {
      nv = iy_get_stack();
      i = NV_len(nv);	/* Nets in the vector are processed and pushed back. */
      while (--i >= 0)	/* The vector (or the stack, actually) is here */
	{		/* "upsidedown", and I SHOULD NOT lean on it.... !!! */
	  iy_net_to_stack((*func)(NV_net(nv,i), DONT_COPY, IY_MINIMIZE_P));
	}
      free_nv_only(nv);
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}
/****************************************************************
  COMMAND: fs operations: compose, concat, intersect, shuffle, union
 ****************************************************************/

void iy_fs_stack_op(NETptr (*func)(), FST_CNTXTptr fst_cntxt)
{
  if (iy_check_args(1, last_command->command) &&
      iy_can_do_on(STANDARD_NET, last_command->command, -1))
    {
      /* Note that intersect, shuffle, and compose reclaim the
	 argument nets if they are called with DO_RECLAIM as
	 the second argument whereas union_fsm and concat
	 reclaim the argument nets if the second argument
	 is DONT_COPY. DO_RECLAIM and DONT_COPY now are
	 defined to be the same constant (0). Thus the call
	 below has the effect of reclaiming the argument
	 nets regardless of what function gets called.
      */
      iy_net_to_stack((*func)(iy_get_stack(),
			      DO_RECLAIM, IY_MINIMIZE_P, DONT_CLEANUP,
			      !IY_INORDER, /* This last parameter works only
					      with compose and intersection */
			      /* If the C-compiler does not ignore the last 
				 reduntant argument, this may cause problems */
			      IY_COMPOSE_FLAG_AS_SPECIAL
			      /* This parameter is for compose only */
			      )); 
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}
/****************************************************************
  COMMAND: fs operations: negate, reverse, substring
 ****************************************************************/
void iy_fs_1_op(NETptr (*func)(), FST_CNTXTptr fst_cntxt)
{
  if (iy_check_args(1, last_command->command) &&
      iy_can_do_on(STANDARD_NET, last_command->command, 1))
    {
      NETptr net = iy_stack_pop();
#ifndef WORLD  /* Are we including FSMMORE? */
      if (*func == *virtual_to_real_fsm)
	iy_net_to_stack((*func)(net, IY_MINIMIZE_P, fst_cntxt));
      else
#endif
	iy_net_to_stack((*func)(net, DONT_COPY, IY_MINIMIZE_P, fst_cntxt));
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}


void iy_fs_1_virt_op(NETptr (*func)(), FST_CNTXTptr fst_cntxt)
{
  /* Applies *FUNC to the top network on the stack.
  The convention in (x)fst is that the networks on the stack are
  reclaimable. All virtual calculus operations must specify
  explicitly whether the operand can be reclaimed when the virtual
  net is freed.
  */
  if (iy_check_args(1, last_command->command) &&
      iy_can_do_on(VIRTUAL_NET, last_command->command, 1))
    {
      NETptr net = iy_stack_pop();
#ifndef WORLD 
      if (func == virtual_to_real_fsm)
	iy_net_to_stack((*func)(net, IY_MINIMIZE_P));
      else
#endif
	iy_net_to_stack((*func)(net, DO_RECLAIM));
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}

/****************************************************************
  COMMAND: fs operation: eliminate flag
 ****************************************************************/

NETptr iy_eliminate_flag(NETptr net)
{
  /* In principle two-byte attribute names are OK but the interface
     does not handle them yet. Need to coerce here the simple string
     to a two-byte string. */

  id_type attr_id;

  intern_flag_attribute(&attr_id, FST_thin_to_temp_fat((char *) fs_arg1));

  if (flag_attribute_occurs_in_p(net, attr_id))
    net = eliminate_flag_attribute(net, attr_id, DO_RECLAIM, IY_MINIMIZE_P);
  else
    {
      fsm_printf("Flag attribute '%s' does not occur in the network.\n",
		 fs_arg1);

      quit_on_fail_error(IY_QUIT_ON_FAIL);
    }
  return(net);
}

/****************************************************************
  COMMAND: fs operations: bimachine up/down
 ****************************************************************/
void iy_1_to_n_op(NVptr (*func)())
{
  if (iy_check_args(1, last_command->command) &&
      iy_can_do_on(STANDARD_NET, last_command->command, 1))
    {
      iy_nv_to_stack((*func) (iy_stack_pop()));
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}

void iy_m_to_n_op (NVptr (*func)(), int m)
{
  /* Calls "func" on the m top nets from the stack, and replaces these
     m nets by n nets resulting from "func".
     If m is negative, all nets from the stack are taken.
     NOTE: "func" should always consume the input nets but not the vector,
     and return nets in a vector on success and NULL on failure. */

  NVptr in, out;  int i;  char *error;

  /* take input nets */

  if (m < 0)  /* take all nets from the stack */
    {
      if (! iy_check_args (1, last_command->command))
	{
	  quit_on_fail_error(IY_QUIT_ON_FAIL);
	  return;
	}

      in = iy_get_stack();
    }
  else        /* take m nets from the stack */
    {
      if (! iy_check_args (m, last_command->command))
	{
	  quit_on_fail_error(IY_QUIT_ON_FAIL);
	  return;
	}

      in = make_nv (m);

      for (i=0; i< m; i++)
	NV_net (in, i) = iy_stack_pop ();
    }

  /* compile output nets */

  out = func (in);
  free_nv_only (in);

  if (out)                /* success */
    iy_nv_to_stack (out);
  else                    /* failure */
    {
      out = make_nv (1);
      NV_net (out, 0) = null_fsm ();
      iy_nv_to_stack (out);

      fsm_printf ("ERROR: %s\n",
		  ((error = error_buffer ("all", NULL))) ? error : "");
      quit_on_fail_error(IY_QUIT_ON_FAIL);
    }
}

/****************************************************************
  COMMAND: binary operations: minus, crossproduct,  
           test equivalence, test overlap, test sublanguage
 ****************************************************************/

static void net_comparison (int comp_fn (NETptr n1, NETptr n2))
{
  NVptr NET_STACK = iy_stack(0);
  int i = 1, lim = NV_len(NET_STACK)  ;
  
  while (i < lim)
    if (!comp_fn(nv_get(NET_STACK, 0), nv_get(NET_STACK, i)))
      {
        fsm_printf("0, (0=NO,1=YES)\n");
	if (IY_ASSERT)
	  quit_on_fail_error(IY_QUIT_ON_FAIL);
        break;
      }
    else if (++i == lim)
      fsm_printf("1, (0=NO,1=YES)\n"); 
}

void iy_fs_2_op(NETptr (*func)(), FST_CNTXTptr fst_cntxt)
{
  /* Applies *FUNC to the two top networks on the stack.
     If the function needs additional arguments, they are
     added here. Generates an error if there not at least
     two arguments of the suitable type on the stack.
  */
  NETptr net1, net2;
  if (iy_check_args(2, last_command->command) &&
      iy_can_do_on(STANDARD_NET, last_command->command, 2))
    {
      net1 = iy_stack_pop();
      net2 = iy_stack_pop();

      if (*func == *minus_fsm ) /* hack to add more 2 arg functions. TG */
	iy_net_to_stack((*func)(net1, net2, DO_RECLAIM, DO_RECLAIM,
				IY_MINIMIZE_P));
      else if (*func == *crossproduct_fst )
	iy_net_to_stack((*func)(net1, net2, DO_RECLAIM, IY_MINIMIZE_P));
      else
	{ /* No additional arguments needed */
	  iy_net_to_stack((*func)(net1, net2));
	}
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}


void iy_fs_2_virt_op(NETptr (*func)(), FST_CNTXTptr fst_cntxt)
{
  /* Applies *FUNC to the two top networks on the stack.
  The convention in (x)fst is that the networks on the stack are
  reclaimable. All virtual calculus operations must specify
  explicitly whether the operand can be reclaimed when the virtual
  net is freed.
  */

  NETptr net, net1, net2;
  if (iy_check_args(2, last_command->command) &&
      iy_can_do_on(VIRTUAL_NET, last_command->command, 2))
    {
      net1 = iy_stack_pop();
      net2 = iy_stack_pop();
      
#ifndef WORLD 
      if (func == virtual_priority_union)
	net = virtual_priority_union(net1, net2, LOWER, DO_RECLAIM);
      else
#endif
	net = ((*func)(net1, net2, DO_RECLAIM));
      iy_net_to_stack(net);
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}

void iy_test_2_op(int (*func)())
{
  if (iy_check_args(2, last_command->command) &&
      iy_can_do_on(STANDARD_NET, last_command->command, 2))
    {
      if (*func == *equiv_fsm_p 
	  || *func == *intersect_p 
	  || *func == *sublanguage_p)
	net_comparison (func);
      else
	fsm_fprintf (stderr, "compiler error: in iy_test_2_op()\n");
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}

/****************************************************************
  COMMAND: fs operations: lookup, lookdown
 ****************************************************************/
static APPLYtype APPLY_CONTEXT ;
static STRING_BUFFERptr OUT_BUFFER = NULL;
							
#if NOT_USED
static void iy_report_wrong_bimachine_apply_direction(char *s1, char *s2)
{

	fsm_fprintf(stdout, "%s bimachine cannot be applied %s.\n", s1, s2);
}						
							
static int iy_check_bimachine_apply_direction(NETptr n1, NETptr n2,
					      int input_side)
{
	
  if (input_side == UPPER)
    {
      if (NET_lower_seq(n1) || NET_lower_seq(n2))
	{
	  iy_report_wrong_bimachine_apply_direction("A downward", "upwards");
	  return(1);
	}
		
    }
  else if (NET_upper_seq(n1) || NET_upper_seq(n2))
    {
      iy_report_wrong_bimachine_apply_direction("An upnward", "downwards");
      return(1);
    }
  return(0);
}

static void iy_report_wrong_bimachine_order(char *s1, char *s2, char *s3)
{
  fsm_fprintf(stdout, "The two halves of %s bimachine ", s1);
  fsm_fprintf(stdout, "should be on the stack\n");
  fsm_fprintf(stdout, "in the opposite order: %s, %s.\n", s2, s3);
}

static void iy_check_bimachine_order(NETptr *n1, NETptr *n2, int wrong_p,
				     char *s1, char *s2, char *s3)
{
  if (wrong_p)
    {
      NETptr net = *n2;
		
      iy_report_wrong_bimachine_order(s1, s2, s3);
			
      *n2 = *n1;
      *n1 = net;
    }
}
#endif /*NOT_USED */


void iy_apply_and_print(char *input)
{
  /* Always apply the net or the bimachine on the top of the stack. 
     The non licenseable version can perform a lookup/down on Kaplan
     compressed networks. */

	
  APPLY_CONTEXT.nv_pos = 0;

  if (choose_nets_to_apply(&APPLY_CONTEXT, FALSE))
    return;
  
  if (NET_Kaplan_compressed(APPLY_CONTEXT.net1))
    {
      handle_warning("Unsupported network format", "IY_APPLY_AND_PRINT", 0);
      return ;
    } /* if (NET_Kaplan_compressed(APPLY_CONTEXT.net1)) */
  /* else if (NET_compacted(APPLY_CONTEXT.net1)) */
  /*   { */
  /*    PARSE_TBL ptb = get_parse_table(APPLY_CONTEXT.net1, APPLY_CONTEXT.side); */
  /*       apply_to_string(input, ptb, APPLY_CONTEXT.net1, APPLY_CONTEXT.side, */
  /* 		      APPLY_CONTEXT.out_side, APPLY_CONTEXT.obey_flags_p); */
  /*     } */
  else
    {
        if (has_arc_vectors_p(APPLY_net1(&APPLY_CONTEXT)))
	{
      /* fsm_fprintf(stdout,"Standard apply routine does not work ");*/
      /* fsm_fprintf(stdout,"on vectorized nets.\nUnvectorizing...\n");*/
      /* unvectorize_net(APPLY_net1(&APPLY_CONTEXT)); */
            
            /* Vectorized net detected:
               create the NET_uprange_map (or NET_downrange_map
               depending on the apply side). This is a key step to
               speed up the access to the destination array of the
               vectorized states: RANGE_items(range_map + in_sym)
               contains the list of labels having "in_sym" on the 
               apply input side. */
            make_range_map(APPLY_net1(&APPLY_CONTEXT),APPLY_CONTEXT.side); 
	}

        /* Set up of the global variables INPUT_MATCH and
           OUTPUT_MATCH depending on the value of the input_side. */
        select_match_tables_for_apply(APPLY_CONTEXT.side);

        APPLY_CONTEXT.output_fn = write_to_buffer;
        init_apply_context(&APPLY_CONTEXT);
        init_apply_net(&APPLY_CONTEXT);
        init_apply_to_string(input, &APPLY_CONTEXT);
        
        if (apply_network(&APPLY_CONTEXT))
            quit_on_fail_error(IY_QUIT_ON_FAIL);
        else
            print_string_buffer(OUT_BUFFER, stdout);     
    }
} /* iy_apply_and_print */
 

void iy_compose_apply_and_print(char *input)
{
  IO_SEQ_TABLEptr output_table; /* contains the output of the apply
                                   written as a table. */
  APPLYptr last_context;
  
  STATEptr solution_tree= NULL; /* contains the output of the apply
                                   written as a tree. */

  if (IY_RECURSIVE_APPLY)
    {
      APPVECptr appvec;

      if (recursive_compose_apply_networks(input, &APPLY_CONTEXT))
	quit_on_fail_error(IY_QUIT_ON_FAIL);

      appvec = APPLY_hyper_unit(&APPLY_CONTEXT);
      last_context = APPVEC_last_context(appvec);
      if (last_context->solution_tree != NULL)
          solution_tree=last_context->solution_tree;
      else
          output_table = APPLY_output_table(last_context);
    }
  else
    {
      if (compose_apply_networks(input, &APPLY_CONTEXT))
	quit_on_fail_error(IY_QUIT_ON_FAIL);
      output_table = APPLY_output_table(&APPLY_CONTEXT);
    }

  if (solution_tree != NULL)
  {
      LAB_VECTORptr buffer= make_lab_vector(20);   
      print_solution_tree(solution_tree, buffer, stdout, DONT_ESCAPE);
      reclaim_lab_vector(buffer);
  }
  else     
      print_io_seq_table(output_table, stdout, DONT_ESCAPE);
}


void iy_apply_patterns(ifile *file, char *word, int input_side, char *prompt)
{
  NVptr nv = iy_stack(0);

  if (nv && NV_len(nv) > 0)
    {
      FILE *stream = NULL;

      APPLY_CONTEXT.net_vector = nv; 
      APPLY_CONTEXT.nv_pos = 0;
      APPLY_CONTEXT.side = input_side;
      APPLY_CONTEXT.obey_flags_p = IY_OBEY_FLAGS;
      APPLY_CONTEXT.print_space_p = IY_PRINT_SPACE;
      APPLY_CONTEXT.show_flags_p = IY_SHOW_FLAGS;
      APPLY_CONTEXT.delete_patterns_p = IY_DELETE_PATTERNS;
      APPLY_CONTEXT.extract_patterns_p = IY_EXTRACT_PATTERNS;
      APPLY_CONTEXT.mark_patterns_p = IY_MARK_PATTERNS;
      APPLY_CONTEXT.count_patterns_p = IY_COUNT_PATTERNS;

      APPLY_CONTEXT.need_separators_p = IY_NEED_SEPARATORS;

      if (IY_DELETE_PATTERNS && IY_EXTRACT_PATTERNS && !IY_COUNT_PATTERNS)
	{
	  fsm_fprintf(stderr,
		      "*** When the variables 'delete-patterns' \
and 'extract-patterns' are both ON\n\
'apply patterns up' and 'apply patterns down' produce no output.\n\
unless 'count-patterns' is ON. Aborting...\n");

	  quit_on_fail_error(IY_QUIT_ON_FAIL);
	  return;
	}

      if (choose_nets_to_apply(&APPLY_CONTEXT, FALSE))
	quit_on_fail_error(IY_QUIT_ON_FAIL);
  
      if (NET_reduced_labelset(APPLY_CONTEXT.net1))
	{
	  fsm_fprintf(stderr, "Sorry. This function has not yet been\
 implemented for reduced networks.\n");
	  return;
	}

      if (NET_left_bimachine(APPLY_CONTEXT.net1) ||
	  NET_left_bimachine(APPLY_CONTEXT.net1))
	{
	  fsm_fprintf(stderr, "Sorry. This function has not\
 been implemented for bimachines.\n");
	  return;
	}

     if (input_side == UPPER)
        APPLY_CONTEXT.out_side = LOWER;
      else
	APPLY_CONTEXT.out_side = UPPER;

      APPLY_CONTEXT.output_fn = merge_to_buffer;
      APPLY_CONTEXT.out_stream = stdout;

      init_apply_context(&APPLY_CONTEXT);
      init_apply_net(&APPLY_CONTEXT);

      /* If we are counting patterns, set the tag counts to 0 */

      if (APPLY_CONTEXT.count_patterns_p)
	reset_pattern_match_counts(&APPLY_CONTEXT);

      if (word && *word == '\0')
	{
	  print_a_prompt(prompt);

	  while((word = lookup_line()))
	    {
	      init_apply_to_string(word, &APPLY_CONTEXT);

	      if (!print_prompt)
		fsm_fprintf(stdout, "%s%s\n", prompt, word);

	      apply_patterns(&APPLY_CONTEXT);
	      fsm_fprintf(stdout, "\n");

	      print_a_prompt(prompt);
	    }

	  if (APPLY_CONTEXT.count_patterns_p)
	    print_pattern_match_counts(&APPLY_CONTEXT, stdout);
	}
      else
	{
	  if (word)
	    init_apply_to_string(word, &APPLY_CONTEXT);
	  else if (file)
	    {
	      if (strcmp(file->name, "-") == 0)
		stream = stdin;
	      else if (!(stream = fopen(file->name, "r")))
		{
		  fsm_fprintf(stderr, "Cannot open %s\n", file->name);
		  quit_on_fail_error(IY_QUIT_ON_FAIL);
		  return;
		}

	      init_apply_to_stream(stream, &APPLY_CONTEXT);
	    }
	
	  if (apply_patterns(&APPLY_CONTEXT))
	    quit_on_fail_error(IY_QUIT_ON_FAIL);

	  if (stream && stream != stdin)
	    fclose(stream);
	  
	  if (word)
	    fsm_fprintf(stdout, "\n");

	  if (APPLY_CONTEXT.count_patterns_p)
	    print_pattern_match_counts(&APPLY_CONTEXT, stdout);
	}
    }
  else
    {
      if (IY_VERBOSE)
	warn_badnet();

      quit_on_fail_error(IY_QUIT_ON_FAIL);
    }
}

void iy_fs_apply(ifile *file, char *word, char *prompt, int input_side,
		 void (*apply_fn)(char *))
{
  int output_side;
  NVptr nv = iy_stack(0);

/*   int i=0; */
/*   for (i=0; i< nv->len; i++) */
/*   { */
/*       if (!NET_compacted(nv->nets[i])) */
/*       vectorize_net(nv->nets[i]); */
/*   } */
  
  if (nv && NV_len(nv) > 0)
    {
      APPLY_CONTEXT.net_vector = nv; 
      APPLY_CONTEXT.side = input_side;
      APPLY_CONTEXT.obey_flags_p = IY_OBEY_FLAGS;
      APPLY_CONTEXT.print_space_p = IY_PRINT_SPACE;
      APPLY_CONTEXT.show_flags_p = IY_SHOW_FLAGS;

      if (!OUT_BUFFER)
	{
	  OUT_BUFFER = make_string_buffer(80);
	  APPLY_CONTEXT.out_buffer = OUT_BUFFER;
	}

      if (IY_PRINT_PAIRS)
	output_side = BOTH_SIDES;
      else if (input_side == UPPER)
        output_side = LOWER;
      else
	output_side = UPPER;

      APPLY_CONTEXT.out_side = output_side;
  
      if (word && *word)
	apply_fn(word);
      else
        {
	  finfo buffer, *prev = NULL;
	  if (file && !(prev = new_interface_input_file(file, &buffer, 0)))
	    return;

	  if (!file)
	    print_a_prompt(prompt);

	  while ((word = lookup_line()))
            {
	      /* If we are reading from a file. We print an
		 empty line and the input word but no prompt.
	      */

	      if (file && IY_VERBOSE)
		fsm_fprintf(stdout, "\n%s\n", word);
	    
	      apply_fn(word);

	      if (!file)
		print_a_prompt(prompt);
            }
	  interface_restart_if_necessary();
	  if (file) old_interface_input_file(prev, &buffer);
        }

      /* Free the apply_context_vector pointed by hyper_unit: */
      if (APPLY_CONTEXT.hyper_unit != NULL)
      {
          free_apply_context_vector(APPLY_CONTEXT.hyper_unit);
          APPLY_CONTEXT.hyper_unit= NULL;
      }
    }
  else
    {
      if (IY_VERBOSE)
	warn_badnet();

      quit_on_fail_error(IY_QUIT_ON_FAIL);
    }
}


/****************************************************************
  COMMAND: print arcs
 ****************************************************************/

int iy_tally_sigma_by_words(FILE *f, NETptr net, int n, char * sidestr)
{
  long unsigned wc = 0;
  int tally_size = 0;
  id_type max_tally_id = 0;
  TALLYptr word_tally = NULL;
  int side;

  if ( ! strcmp(sidestr, "upper"))
    side = UPPER;
  else if  ( ! strcmp(sidestr, "lower"))
    side = LOWER;
  else
    side = UPPER_OR_LOWER;

  if (net)
    {
     if (!n) --n;
     word_tally = tally_paths(net, 1, n, side, &wc,  &tally_size,
			      &max_tally_id, IY_QUOTE_SPECIAL,  
			      IY_OBEY_FLAGS);
     print_this_tally(f, word_tally, tally_size,
		      max_tally_id, compare_label_freq_symbols_precede_tags);
	fsm_fprintf(f, "Number of paths = %lu \n", wc);
	free(word_tally);
    }
  else
    {
      if (IY_VERBOSE) warn_badnet();
      quit_on_fail_error(IY_QUIT_ON_FAIL);
    }
  return wc;
}

void iy_print_net(FILE *f, int what, NETptr net)
{
  char *TRUE_COND = "1 (1 = TRUE, 0 = FALSE)\n",
    *FALSE_COND = "0 (1 = TRUE, 0 = FALSE)\n";
  int RM = WIN_LEN - 6, sigma_tally_count = 0 , stl = 0;
  id_type max_sigma_id = 0;
  TALLYptr tally_sig = NULL;

  if (net)
    {        
      /* The only print operation that can be done on virtual
	 networks is printing the sigma. We can inspect a net
	 only with the tcl/tk interpreter.
      */

      if (NET_is_virtual(net) && !(what == 0 || what == 1 || what == 2 ||
				   /* what == 3 || */
				   what == 4 || what==6))
	{
	  not_applicable_to_virtual_net(IY_ASSERT && IY_QUIT_ON_FAIL);
	  return;
	}

      switch (what)
	{
	case 0:

            /* Karttunen compacted and Kaplan compressed networks can't be
               printed:  */
            if (NET_Kaplan_compressed(net))
            {
                fsm_fprintf(stderr, "Can't print a net having this format...\n");
                print_flags_s(net, RM, 0, f);
                return;
            }
#ifdef WORLD
	  if (NET_num_states(net) > WORLD_STATE_LIMIT)
	    {
	      fsm_fprintf(stderr, "This network is too large to be \
displayed in text format by xfst.\n");
	      fsm_fprintf(stderr, "You need to license a professional \
version of this program to print it.\n");
	      return;
	    }
#endif
	  if (IY_SORT_ARCS &&
              !NET_shared_arc_lists(net) &&
              !NET_is_virtual(net) &&
              !NET_compacted(net))
	    iy_sort(net);

	  if (IY_PRINT_SIGMA)
	    {
	      fputs("Sigma: ",f);
	      print_sigma_s(net, RM, 7, f);
	    }
	  print_net(net, f);
	  break;
	case 1:
	  print_net_size(net, f);
	  break;
	case 2:
	  print_sigma_s(net, RM, 0, f);
	  break;	  
	case 3:
	  print_labels_s(net, RM, 0, f);
	  break;
	case 4:
	  net_name_out(net);
	  fresh_line(RM, 7);
	  print_flags_s(net, RM, 0, f);
	  break;
	case 5:

	  tally_labels(net);
	  print_tally(f);
	  free(SYMBOL_TALLY);
	  SYMBOL_TALLY = NULL;
	  break;

	case 6:
#ifndef WORLD
            /* Tcl-Tk does not work on the Mac nor on Windows */
  #ifdef Darwin
	  inspect_net(net, RM, BOTH_SIDES);
  #else
     #ifndef HAVE_TCL
	  inspect_net(net, RM, BOTH_SIDES);
     #else
	  tk_inspect_net(net);
     #endif
  #endif
#else
	  inspect_net(net, RM, BOTH_SIDES);
#endif /* WORLD */
	  break;
	case 7:
            if (NET_compacted(net))
            {
                fsm_fprintf(stderr, "Can't check for epsilon cycles in a compacted network.\n");
                break;
            }
	  if (linearbounded_p(net, LOWER))
	    fsm_fprintf(f, TRUE_COND);
	  else
	    fsm_fprintf(f, FALSE_COND);
	  break;
	case 8:
	  if (linearbounded_p(net, UPPER))
	    fsm_fprintf(f, TRUE_COND);
	  else
	    {
	      fsm_fprintf(f, FALSE_COND);
	      if (IY_ASSERT)
		quit_on_fail_error(IY_QUIT_ON_FAIL);
	    }
	  break;
	case 9: /* lower universal */
	  if (side_universal_p(net, NULL ,LOWER))
	    fsm_fprintf(f, TRUE_COND);
	  else
	    {
	      fsm_fprintf(f, FALSE_COND);
	      if (IY_ASSERT)
		quit_on_fail_error(IY_QUIT_ON_FAIL);
	    }
	  break;
	case 10:
	  if (side_universal_p(net, NULL ,UPPER))  
	    fsm_fprintf(f, TRUE_COND);
	  else
	    {
	      fsm_fprintf(f, FALSE_COND);
	      if (IY_ASSERT)
		quit_on_fail_error(IY_QUIT_ON_FAIL);
	    }
	  break;
#ifndef WORLD  
	case 11:
	  if (unambiguous_p(net, UPPER))  
	    fsm_fprintf(f, TRUE_COND);
	  else
	    {
	      fsm_fprintf(f, FALSE_COND);
	      if (IY_ASSERT)
		quit_on_fail_error(IY_QUIT_ON_FAIL);
	    }
	  break;	  
	case 12:
	  if (unambiguous_p(net, LOWER))  
	    fsm_fprintf(f, TRUE_COND);
	  else
	    {
	      fsm_fprintf(f, FALSE_COND);
	      if (IY_ASSERT)
		quit_on_fail_error(IY_QUIT_ON_FAIL);
	    }
	  break;
#endif /* WORLD */
	case 13: /* tally sigma frequencies according to arc frequency */
	  tally_sig = make_tally_sigma(net, &sigma_tally_count,
				       &max_sigma_id);
	  print_this_tally(f, tally_sig, sigma_tally_count,
			   max_sigma_id, compare_label_freq_symbols_precede_tags);
	  free(tally_sig);
	  tally_sig = NULL; 
	  max_sigma_id = 0;
	  sigma_tally_count = 0;
	  break;
	case 14:
	  if (NET_arc_label_arity(net) > 1)
	    {
	      stl = longest_minimal_string(net, UPPER, TRUE);
	      fsm_fprintf(f, "Longest non-looping upper string has %d character(s).\n", stl);
	    }
	  stl = longest_minimal_string(net, LOWER, TRUE);
	  fsm_fprintf(f, "Longest non-looping lower string has %d character(s).\n", stl);
	  break;
	case 15:
	  if (!null_fsm_p(net))
	    fsm_fprintf(f, TRUE_COND);
	  else
	    {
	      fsm_fprintf(f, FALSE_COND);
	      if (IY_ASSERT)
		quit_on_fail_error(IY_QUIT_ON_FAIL);
	    }
	  break;
	case 16:
	  if (null_fsm_p(net))
	    fsm_fprintf(f, TRUE_COND);
	  else
	    {
	      fsm_fprintf(f, FALSE_COND);
	      if (IY_ASSERT)
		quit_on_fail_error(IY_QUIT_ON_FAIL);
	    }
	  break;
	case 17:
	  if (NET_arc_label_arity(net) > 1)
	    {
	      stl = print_longest_minimal_string(net, UPPER, TRUE, IY_PRINT_SPACE, f);
	      fsm_fprintf(f, "Longest non-looping upper string has %d character(s).\n", stl);
	    }
	  stl = print_longest_minimal_string(net, LOWER, TRUE, IY_PRINT_SPACE, f);
	  fsm_fprintf(f, "Longest non-looping lower string has %d character(s).\n", stl);
	  break;
	case 18:
	  if (IY_SORT_ARCS && !NET_shared_arc_lists(net))
	    iy_sort(net);
#ifndef _WIN32
	  vcg_to_file_or_to_window(f, net, IY_INTERACTIVE, IY_VERBOSE);
#else
          print_vcg_net(net, f);
#endif
          break;
	case 19:
	  if (IY_SORT_ARCS && !NET_shared_arc_lists(net))
	    iy_sort(net);
#ifndef _WIN32
	  dot_to_file_or_to_window(f, net, IY_INTERACTIVE, IY_VERBOSE);
#else
          print_dot_net(net, f);
#endif
	  break;
	default:
	  break;
	}
    }
}

void iy_print_nets(FILE *f, int what)
{
  int i;
  for (i=iy_stack_length()-1; i>=0; i--)
      iy_print_net(f, what, iy_top_stack(i));
}

void iy_print_longest_string(FILE *f, NETptr net)
{


}

/****************************************************************
  COMMAND: print (random) words
 ****************************************************************/
#if NOT_USED
static void adjust_output_buffer_size(NETptr net, int  side, FST_CNTXTptr fst_cntxt)
{
  int ns=0;
  if (net)
    {
      ns = longest_minimal_string( net, side, TRUE);
      if ( ns > FST_output_buffer_size(fst_cntxt))
	{
	  free(FST_output_buffer(fst_cntxt));
	  FST_output_buffer(fst_cntxt) = malloc((size_t) ns * sizeof(char));
	}
    }
}
#endif /* NOT_USED */

static int outwor(char *p, FILE*f)
{
  fputs(p,f);
  return fputs("\n",f);
}

void iy_print_words(FILE *f, NETptr net, int n, int side)
{
  long unsigned wc;
  int err;
  unsigned int count;
  int warn_p = FALSE;

  if (net)
    {
      if (NET_Kaplan_compressed(net) || NET_compacted(net))
        {
	  fsm_printf("Can't process this network format.\n");
	  return;
        }

      err = index_net(net);
      count = (unsigned int) STATE_client_cell(NET_start_state(net));

#ifdef WORLD
      if ((n <= 0 || n > WORLD_PATH_LIMIT) &&
	  !(err == 0 && count < WORLD_PATH_LIMIT))
	{
	  n = WORLD_PATH_LIMIT;
	  warn_p = TRUE;
	}
#else
      if (!n) --n;
#endif

      if (IY_PRINT_SPACE)
	print_paths(net, IY_MAX_STATE_VISITS, n, side, " ", &wc, f,
		    IY_QUOTE_SPECIAL, IY_OBEY_FLAGS, IY_SHOW_FLAGS, outwor);
      else
	print_paths(net, IY_MAX_STATE_VISITS, n, side, NULL, &wc, f,
		    IY_QUOTE_SPECIAL, IY_OBEY_FLAGS, IY_SHOW_FLAGS, outwor);

      if (warn_p && wc == n)
	{
	  /* Warn the user that all paths were not printed. */

	  fsm_fprintf(stderr, "This network is too large to be displayed in \
text format by xfst.\n");
	  fsm_fprintf(stderr, "You need to license a professional \
version of this program to see more.\n");
	}
    }
  else
    {
      if (IY_VERBOSE) warn_badnet();
      quit_on_fail_error(IY_QUIT_ON_FAIL);
    }
}

void iy_print_rand_words(FILE *f, int n, int side)
{
  int warn_p = FALSE;

  if (iy_check_args(1, last_command->command) &&
      iy_can_do_on(STANDARD_NET, last_command->command, 1))
    {
#ifdef WORLD
      if (n > WORLD_PATH_LIMIT)
	{
	  n = WORLD_PATH_LIMIT;
	  warn_p = TRUE;
	}
#endif

      if (IY_PRINT_SPACE)
	random_words_s(iy_top_stack(0), n, side, IY_OBEY_FLAGS, IY_SHOW_FLAGS,
		       f, " ", IY_QUOTE_SPECIAL, outwor);
      else
	random_words_s(iy_top_stack(0), n, side, IY_OBEY_FLAGS, IY_SHOW_FLAGS,
		       f, NULL, IY_QUOTE_SPECIAL, outwor);

      if (warn_p)
	{
	  fsm_fprintf(stderr, "The number of random paths requested was too \
large for xfst.\n");
	  fsm_fprintf(stderr, "You need to license a professional \
version of this program to see more.\n");
	}
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
} 

void iy_do_word_to_numbers(NETptr net, char * word, int side, FILE * ofile)
{
  long int *indices;
  int i = 0;

  if (net)
    {
      if (!word)
	word = get_input("Word:", FALSE, "\n");
    
      if (word && (indices = word_to_numbers(word, net, side)))
	{
	  while (*indices >= 0)
	    {
	      if (i++ > 0)
		fsm_fputs(", ", ofile);  
	      fsm_fprintf(ofile, "%ld", *indices++);
	    }
	  fputc('\n', ofile);
	}
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}

void iy_do_number_to_word(NETptr net, int num, int side, FILE * ofile)
{
  char *word;
  /* long int n; */

  if (net && (num  >= 0) && (word = number_to_word(num, net, side, IY_QUOTE_SPECIAL)))
    fsm_fprintf(ofile, "%s\n", word);
}

/****************************************************************
  COMMAND: read text 
           read prolog
	   write spaced-text, write text
 ****************************************************************/

static void use_control_d_or_z_to_terminate(void)
{
#ifndef _WIN32
	  fsm_printf("Use ^D to terminate input.\n");
#else
	  fsm_printf("Use ^Z to terminate input.\n");
#endif
}

#if NOT_USED
static int compacted_p(NETptr net)
{
  OBJECTptr value = get_value(net, COMPACTED_ATTR);
  
  if (value == NULL)
    return(FALSE);
  
  if (OBJ_type(value) == Symbol &&
      fat_strcmp(IO_SYM_name(OBJ_symbol(value)), T_NAME) == 0)
    return(TRUE);
  else
    return(FALSE);
}
#endif /* NOT_USED */

NETptr network_from_lexc_file(FILE *stream)
{
  extern void parse_lexicon();
  extern void lexicon_restart();
  extern FILE *lexicon_in;
  NETptr net = NULL;
  HASH_TABLEptr fst_defs = DEF_TABLE;

  /* Create a separate table for lexc definitions. */

  DEF_TABLE = make_hash_table(DEF_BUCKETS, "Lexc definitions");

  lexicon_in = stream;
  
  reset_out_buffer();
  
  lexicon_restart(lexicon_in);
      
  parse_lexicon();
      
  net = link_and_cleanup(IY_QUIT_ON_FAIL);
      
  /* Restore DEF_TABLE */

  free_hash_table(DEF_TABLE);
  DEF_TABLE = fst_defs;

  return(net);
}

int iy_read_networks(void  *input_fn(FILE *stream), ifile *file)
     /*****
      *
      * Called in response to the following commands:
      * 'read text', 'read spaced-text', 'read lexc', 'read prolog',
      *  and 'read wprolog', all of which compile a network from
      *  and ASCII representation and take input either from
      *  a file or from stdin. The first three read and return a
      *  single network, 'read prolog' returns a vector of networks.
      *  Because of the difference we have to specify the return
      *  value as (void *).
      *
      *  If the routine fails because of an error of some sort
      *  or because the user has aborted without providing any
      *  input, the result is a null fsm in all cases.
      *
      *****/
{
  FILE *f = NULL;
  NVptr *net_stack = iy_stackptr(0); /* TG */
  NVptr nets;
  NETptr net = NULL;

  if (file)
    {
      switch (file->type)
	{
	case PIPE_IN:
	  f = popen(file->name,"r");
	  break;
	case FILE_IN:
	  /* dash is an alias for stdin */

	  if (strcmp(file->name, "-") == 0)
	    {
	      f = stdin;
	      file->type = STDIN_IN;
	    }
	  else
	    f = fopen(file->name, "r");
	  break;
	case STDIN_IN:
	  f = stdin;
	  break;
	default:
	  assert(0);
	}

      if (f == NULL)
	{
	  fsm_fprintf(stderr, "Cannot lopen '%s'\n", file->name);
	  quit_on_fail_error(IY_QUIT_ON_FAIL);
	  return(ERR_FILE_OPENING);
	}
      else if (f != stdin && IY_VERBOSE)
	fsm_fprintf(stdout, "Reading from '%s'\n", file->name);
    }
  else
    {
      f = stdin;

      if (input_fn == (void* (*)(FILE*)) network_from_text_file)
	{
	  fsm_printf("\tOne word per line. Space between symbols.\n");
	  fsm_printf("\tA pair of adjacent lines is a transducer path.\n");
	  fsm_printf("\tLeave a blank line between paths.\n\t");
	}
      else if (input_fn == (void*(*)(FILE*)) word_file_to_fsm)
	fsm_printf("\tOne word per line. ");
      else if (input_fn == (void* (*)(FILE*)) network_from_lexc_file)
	fsm_printf("\tType a network description in lexc format.\n\t");
      else if (input_fn == (void* (*)(FILE*)) networks_from_prolog_file)
	fsm_printf("\tType one or more networks in prolog format.\n\t");

      use_control_d_or_z_to_terminate();
    }

  if (input_fn ==  (void* (*)(FILE*)) network_from_text_file ||
      input_fn ==  (void* (*)(FILE*)) word_file_to_fsm ||
      input_fn ==  (void* (*)(FILE*)) network_from_lexc_file)
    {
      net = (NETptr) input_fn(f);

      if(net)
	{
	  if (CONTINUE_OR_STOP == CONTINUE)
	    iy_net_to_stack(net);
	  else
	    free_network(net);
	}
    }
  else if (input_fn ==  (void* (*)(FILE*)) networks_from_prolog_file
#ifndef WORLD    
	   || input_fn ==  (void* (*)(FILE*)) networks_from_weighted_prolog_file
#endif
	   )
    {
      if (file)
	{
	  prolog_file_info->file.name = file->name;
	  prolog_file_info->file.type = file->type;
	}
      else
	{
	  prolog_file_info->file.name = interface_file_info->file.name;
	  prolog_file_info->file.type = interface_file_info->file.type;
	}

      nets = (NVptr) input_fn(f);

      if (nets)
	{
	  if (PROLOG_PARSE_ERROR)
	    {
	      free_nv_and_nets(nets);
	      prolog_file_info->line_number++;
	    }
	  else if (NV_len(nets) == 0)
	    free_nv_only(nets);
	  else
	    *net_stack = nv_append(nets, *net_stack);
	}

      if (PROLOG_PARSE_ERROR)
	quit_on_fail_error(IY_QUIT_ON_FAIL);

      if (!file)
	interface_file_info->line_number += prolog_file_info->line_number;
    }
  else
    assert(0);

  if (f != stdin)
    {
      if (IY_VERBOSE)
        fsm_fprintf(stdout, "Closing '%s'\n", file->name);
      fclose(f);
    }
  else if (feof(stdin))
    clearerr(stdin);  

  return(NO_ERROR);
}

void iy_net_to_text_file(void output_fn (NETptr, FILE *),  FILE *ofile )
{
  NVptr NET_STACK = iy_stack(0);
  NETptr net = nv_get(NET_STACK, 0);
  
  if (!net)
    {
      fsm_fputs("No net.\n", stderr);
      quit_on_fail_error(IY_QUIT_ON_FAIL);
      return;
    }

  if (!NET_loop_free(net) && index_net(net) == 2)
    {
      fsm_fputs("This net is circular.  Cannot save it as text.\n",
		stderr);
      quit_on_fail_error(IY_QUIT_ON_FAIL);
      return;
    }

#ifdef WORLD
  /* After the net has been indexed, the client cell constains the
     number of paths. */

  if (index_net(net) == 0 &&
      (int) STATE_client_cell(NET_start_state(net)) > WORLD_PATH_LIMIT)
    {
      fsm_fprintf(stderr, "Sorry. This network is too large to be saved \
as a text file by xfst.\n");
      fsm_fprintf(stderr, "You need to license a professional version of \
this program.\n");
      quit_on_fail_error(IY_QUIT_ON_FAIL);
      return;
    }

#endif
    if ((output_fn == fsm_to_word_file) || (output_fn == utf8_fsm_to_word_file))
      {
	int i, fst_p = FALSE, multi_char_p = FALSE,
	  lim = ALPH_len(binary_to_label(NET_labels(net)));
      
	for (i = 0; i < lim; i++)
	  {
	    LABELptr label = id_to_label(ALPH_item(NET_labels(net), i));
	    if (LABEL_arity(label) > 1)
	      fst_p = TRUE;
	    else
	      {
		FAT_STR fs = LABEL_atom(label);
		if (CHARSET(fs+1) != 0 || CHARNUM(fs+1) != 0)
		  multi_char_p = TRUE;
	      }
	  }
	if (fst_p)
	  {
	    fsm_fputs("Net is a transducer", stderr);
	    if (multi_char_p)
	      fsm_fputs(" with multi-character labels.\n", stderr);
	    else
	      fsm_fputs(". ", stderr);
	  }
	else if (multi_char_p)
	  fsm_fputs("Net contains multi-character labels.\n", stderr);
      
	if (fst_p || multi_char_p)
	  {
	    fsm_fputs("Use 'write spaced-text' instead.\n", stderr);
	    quit_on_fail_error(IY_QUIT_ON_FAIL);
	    if (ofile != stdin && ofile != stdout)
	      fclose(ofile);
	    return;
	  }
      }
  
  if (ofile)
      output_fn(nv_get(NET_STACK, 0), ofile);
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}

/****************************************************************
  Messing up the alphabet
 ****************************************************************/

/*  Not needed. compact_sigma() does the same work more thoroughly.

static NETptr iy_smash_alph(NETptr net)
{
  if (NET_labels(net) && (ALPH_len(NET_labels(net))>0) &&
      !(ALPH_item(NET_labels(net),0) == OTHER))
    {
      if (NET_sigma(net)) free_alph(NET_sigma(net));
      NET_sigma(net) = copy_alphabet(NET_labels(net));
    }
  return net;
}
*/

NETptr iy_compact_sigma(NETptr net)
{
    compact_sigma(net);
    return(net);
}

/****************************************************************
  Replacement of operations
 ****************************************************************/
void *fs_arg1, *fs_arg2, *fs_arg3, *fs_arg4;

NETptr iy_one_plus(NETptr net, int copy_p, int minimize_p)
{
  return one_plus(net,  copy_p, minimize_p);
}

NETptr iy_zero_plus(NETptr net, int copy_p, int minimize_p)
{
  return zero_plus( net, copy_p, minimize_p);
}

NETptr iy_label_net(NETptr net, int cp, int mn)
{
  NETptr res = NULL;
  res = sigma_fsm(NET_labels(net));
  if (net)
     free_network(net);

  return(res);
}

NETptr iy_negate(NETptr net, int cp, int mn)
{
  return negate_fsm(/* iy_smash_alph */ (net),cp,mn);
}
NETptr iy_lower(NETptr net, int cp, int mn)
{
/*  return iy_smash_alph(lower_fsm(net,cp,mn)); */
    return lower_fsm(net, cp, mn);
}
NETptr iy_upper(NETptr net, int cp, int mn)
{
/*  return iy_smash_alph(upper_fsm(net,cp,mn)); */
    return upper_fsm(net, cp, mn);
}

#ifndef WORLD  
NETptr iy_compact_net(NETptr net, int compact_option)
{
    compact_net(net, compact_option);
    return(net);
}

NETptr iy_uncompact_net(NETptr net)
{
    uncompact_net(net);
    return(net);
}


NETptr iy_optimize_arcs(NETptr net)
{
  do_optimize_arcs(net);
  return(net);
}

NETptr iy_unoptimize_arcs(NETptr net)
{
  do_unoptimize_arcs(net);
  return(net);
}

#endif /* WORLD */

NETptr iy_reduce_labelset(NETptr net)
{
  delete_eqv_arcs(net, NULL, TRUE, DO_KEEP);
  return(net);
}

NETptr iy_share_arcs(NETptr net)
{
  SHARE_make_shared_arclists(net, DONT_COPY);
  return(net);
}

NETptr iy_unreduce_labelset(NETptr net)
{
  restore_eqv_arcs(net, NULL);
  return(net);
}

#ifndef WORLD
NETptr iy_unshare_arcs(NETptr net)
{
  return(SHARE_make_unshared_arclists(net, DONT_COPY));
}
#endif /* WORLD */

NETptr iy_cleanup_net(NETptr net)
{
  cleanup_fst(net, DONT_COPY, DO_MINIMIZE);
  return(net);
}

NETptr iy_sort (NETptr net)
{
  if (NET_shared_arc_lists(net))
    {
      fsm_fprintf(stdout, "Cannot reorder arcs when arcs are shared.\n");
      return(net);
    }

  if (NET_left_bimachine(net))
    {
      if (NET_upper_seq(net))
	reorder_arcs(net, down_left_bm_arc_comp_fn);
      else
	reorder_arcs(net, up_left_bm_arc_comp_fn);
    }
  else if (NET_right_bimachine(net))
    {
      if (NET_upper_seq(net))
	reorder_arcs(net, down_right_bm_arc_comp_fn);
      else
	reorder_arcs(net, up_right_bm_arc_comp_fn);
    }
  else
    reorder_arcs(net, default_arc_comp_fn);

  return (net);
}

NETptr iy_tabular(NETptr net, int cp, int mn)
{
  return net; /* TTB_net_to_ttnet(net, DO_RECLAIM); */
}
NETptr iy_reset_props(NETptr net, int cp, int mn)
{
  FILE * stream;
  if ( ! strcmp (fs_arg1, "stdin"))
    {
      stream = stdin;
      propio_restart(stream);
      use_control_d_or_z_to_terminate();
    }
  else
    stream = fopen(fs_arg1, "r");
    
  if (stream )
    {
      propio_restart(stream);
      reset_props_s(net, stream);
      if (stream != stdin)
	fclose(stream);
      else if (feof(stdin))
	clearerr(stdin);
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);

  return net;
}

NETptr iy_add_props(NETptr net, int cp, int mn)
{
  int err = 0;
  extern FILE *propio_in;
  extern FAT_STR propio_attr;
  extern OBJECTptr propio_value;
  extern int propio_eof;  

  if ( ! strcmp (fs_arg1, "stdin"))
    {
      propio_restart(stdin);
      propio_in = stdin;
      propio_eof = FALSE;
      use_control_d_or_z_to_terminate();

      while (!propio_eof)
	{
	  if (propio_parse() == 1)
	    {
	      fsm_printf("Aborting\n");
	      quit_on_fail_error(IY_QUIT_ON_FAIL);
	      return(net);
	    }
	  else
	    if (!propio_eof && (propio_attr != NULL))
	      add_prop(net, propio_attr, propio_value);
	}
      propio_in = NULL;
      propio_eof = TRUE;
      interface_file_info->line_number += prop_io_line_count();
    }
  else 
    {
      if ((propio_in = fopen(fs_arg1, "r")))
	{
	  propio_restart(propio_in);
	  err = add_props_s(net, propio_in);
	  fclose(propio_in);
	}
      else
	quit_on_fail_error(IY_QUIT_ON_FAIL);
    }
  return net;
}

NETptr iy_add_prop(NETptr net, int cp, int mn)
{
  add_string_prop(net, FST_thin_to_fat(fs_arg1), FST_thin_to_fat(fs_arg2));
  return net;
}

/* char * word = NULL; */ /* ?? */
static void edit_props(NETptr net)
{
  char def = '0';
  
  fsm_fprintf (stdout,   "+------------------------------+\n");
  fsm_fprintf (stdout,   "| XFST NETWORK PROPERTY EDITOR |\n");
  fsm_fprintf (stdout,   "+------------------------------+\n");
  while (TRUE)
    {
      PE_menu();
      word = get_input("Prop-edit (0-4) [exit]:", FALSE, "\n");
      if (word == NULL)
        word = &def;
      
      switch (word[0])
        {
        case ('0'):		/* exit */
          return;
          break;
        case ('1'):		/* add */
          PE_add_prop(net);
          break;                
        case ('2'):		/* delete */
          PE_delete_prop(net);
          break;
        case ('3'):		/* modify */
          PE_modify_prop(net);
          break;
        case ('4'): 
          PE_print_prop(net);
          break;
        default:		/* error */
          fsm_fprintf (stderr, "%%%% input out of range (0-4)\n");
          break;
        }
    }
}

NETptr iy_edit_props(NETptr net, int cp, int mn)
{
  edit_props (net);
  return net;
}

static void name_net(NETptr net, char * word)
{
  if (!word)
    word = get_input("XFST: Name for the top network (<CR> = Cancel):", FALSE, "\n");
  
  if (word)
    {
      name_network(net, word);
      print_net_name(net, stdout);
      fsm_putc('\n', stdout);
    }
  else
    {
      quit_on_fail_error(IY_QUIT_ON_FAIL);
      fsm_printf("Cancelled.\n");
    }
}

NETptr iy_name_net(NETptr net, int cp, int mn)
{
  name_net(net, fs_arg1);
  return net;
}

NETptr iy_print_name_old(NETptr net, int cp, int mn, FILE *ofile)
{
  print_net_name(net, ofile);
  fsm_fprintf (stderr, "\n");
  return net;
}

void iy_print_name( FILE * ofile)
{
  if (iy_check_args(1, last_command->command) &&
      iy_can_do_on(STANDARD_NET, last_command->command, 1))
    {
      print_net_name(iy_top_stack(0), ofile);
      fsm_fprintf (ofile, "\n");
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
  return ;
}

NETptr iy_sigma_net(NETptr net, int cp, int mn)
{
  NETptr res = NULL;
  res = sigma_fsm(NET_sigma(net));
  if (net)
     free_network(net);

  return(res);
}

NETptr iy_single_char_sigma_net(NETptr net, int cp, int mn)
{

  NETptr res = NULL;

  res = single_char_sigma_fsm(NET_sigma(net));
  if (net)
     free_network(net);

  return(res);
}


NETptr iy_multi_char_sigma_net(NETptr net, int cp, int mn, FST_CNTXTptr fst_cntxt)
{

  NETptr res = NULL;

  res = multi_char_sigma_fsm(net, fst_cntxt);
  if (net)
     free_network(net);

  return(res);
}


NETptr iy_paste_net_labels(NETptr net, int cp, int mn, FST_CNTXTptr fst_cntxt)
{

  NETptr res = NULL;  int reclaim_p;

  if (cp == DO_COPY)
    reclaim_p = DONT_RECLAIM;
  else
    reclaim_p = DO_RECLAIM;

  if (net)
    res = PLB_paste_net_labels(net, reclaim_p, mn);

  return(res);
}


#ifndef WORLD

/* 28.2.1998 TG test only */
NETptr iy_TEST_1_unicode_parse (NETptr net, char * nonunic_str, FST_CNTXTptr fst_cntxt)
{
  int res = 0;

  res = TEST1_parse_unicode_string (net, nonunic_str, fst_cntxt);
  fsm_fprintf(stdout, "TEST 1 Parse unicode string. Result = %d \n", res);

  return net;
}


NETptr iy_lseq_up(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt)
{
  NETptr lsres=0;

  lsres = local_sequentialize_net(net, LOWER, fst_cntxt);
 
  if (lsres)
    return lsres;

  return net;
}

NETptr iy_lseq_down(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt)
{
  NETptr lsres=0;

  lsres = local_sequentialize_net(net, UPPER, fst_cntxt);
 
  if (lsres)
    return lsres;

  return net;
}

NETptr iy_push_epsilons(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt)
{
  int from_level = (int) fs_arg2,
    over_level = (int) fs_arg3,
    side = (int) fs_arg1,
    algorithm = (int) fs_arg4;
  NETptr lsres=0;

  lsres = local_from_to_sequentialize_net(net, side, from_level, over_level, algorithm, fst_cntxt);
  
  if (lsres)
    return lsres;

  return net;
}

NETptr iy_seq_up(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt)
{
  return(sq_subsequentialize(net, LOWER, DO_RECLAIM, FALSE, 0, fst_cntxt));
}
NETptr iy_seq_down(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt)
{
  return(sq_subsequentialize(net, UPPER, DO_RECLAIM, FALSE, 0, fst_cntxt));
}
NETptr iy_test_seq_up(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt)
{
  if (sq_is_fst_sequentiable(net, LOWER))
    ;
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
  return net;
}
NETptr iy_test_seq_down(NETptr net, int dummy_cp, int dummy_mn, FST_CNTXTptr fst_cntxt)
{
  if (sq_is_fst_sequentiable(net, UPPER))
    ;
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
  return net;
}

int iy_test_unamb_up(NETptr net)
{
  return(unambiguous_p(net, LOWER));
}

int iy_test_unamb_down(NETptr net)
{
  return(unambiguous_p(net, UPPER));
}
#endif

#ifndef WORLD
/****************************************************************
  FACTORIZATION COMMANDS
 ****************************************************************/

static NVptr one_net_to_nv (NETptr net)
{
  NVptr nv;

  if (net)
    {
      nv = make_nv (1);
      NV_net (nv, 0) = net;
    }
  else
    nv = NULL;

  return (nv);
}

/*------------------------------------------------*/


NVptr iy_factorize_down (NVptr nv)
{
  nv = factorize_arbitrary_fst
    (NV_net (nv, 0), UPPER, DONT_COPY, TRUE, TRUE, IY_VERBOSE);
  return (nv);
}

NVptr iy_factorize_up (NVptr nv)
{
  nv = factorize_arbitrary_fst
    (NV_net (nv, 0), LOWER, DONT_COPY, TRUE, TRUE, IY_VERBOSE);
  nv_reverse (nv);
  return (nv);
}

NVptr iy_unfactorize_down (NVptr nv)
{
  NETptr net;

  net = unfactorize_arbitrary_fst (nv, UPPER, DONT_COPY, TRUE, IY_VERBOSE);
  nv = one_net_to_nv (net);
  return (nv);
}

NVptr iy_unfactorize_up (NVptr nv)
{
  NETptr net;

  nv_reverse (nv);
  net = unfactorize_arbitrary_fst (nv, LOWER, DONT_COPY, TRUE, IY_VERBOSE);
  nv = one_net_to_nv (net);
  return (nv);
}

/*------------------------------------------------*/
NVptr iy_factorize_down_infinite_amb (NVptr nv)
{
  nv = factorize_infamb_fst
    (NV_net (nv, 0), UPPER, DONT_COPY, TRUE, TRUE, IY_VERBOSE);
  return (nv);
}

NVptr iy_factorize_up_infinite_amb (NVptr nv)
{
  nv = factorize_infamb_fst
    (NV_net (nv, 0), LOWER, DONT_COPY, TRUE, TRUE, IY_VERBOSE);
  nv_reverse (nv);
  return (nv);
}

NVptr iy_unfactorize_down_infinite_amb (NVptr nv)
{
  NETptr net;

  net = unfactorize_amb_fsts
    (NV_net (nv, 0), NV_net (nv, 1), UPPER, DONT_COPY, IY_VERBOSE);
  nv = one_net_to_nv (net);
  return (nv);
}

NVptr iy_unfactorize_up_infinite_amb (NVptr nv)
{
  NETptr net;

  net = unfactorize_amb_fsts
    (NV_net (nv, 1), NV_net (nv, 0), LOWER, DONT_COPY, IY_VERBOSE);
  nv = one_net_to_nv (net);
  return (nv);
}

/*------------------------------------------------*/
NVptr iy_factorize_down_finite_amb (NVptr nv)
{
  nv = factorize_finamb_fst
    (NV_net (nv, 0), UPPER, DONT_COPY, TRUE, TRUE, IY_VERBOSE);
  return (nv);
}

NVptr iy_factorize_up_finite_amb (NVptr nv)
{
  nv = factorize_finamb_fst
    (NV_net (nv, 0), LOWER, DONT_COPY, TRUE, TRUE, IY_VERBOSE);
  nv_reverse (nv);
  return (nv);
}

NVptr iy_unfactorize_down_finite_amb (NVptr nv)
{
  NETptr net;

  net = unfactorize_amb_fsts
    (NV_net (nv, 0), NV_net (nv, 1), UPPER, DONT_COPY, IY_VERBOSE);
  nv = one_net_to_nv (net);
  return (nv);
}

NVptr iy_unfactorize_up_finite_amb (NVptr nv)
{
  NETptr net;

  net = unfactorize_amb_fsts
    (NV_net (nv, 1), NV_net (nv, 0), LOWER, DONT_COPY, IY_VERBOSE);
  nv = one_net_to_nv (net);
  return (nv);
}

/*------------------------------------------------*/
NVptr iy_factorize_down_short_amb (NVptr nv)
{
  nv = factorize_shortamb_fst
    (NV_net (nv, 0), UPPER, DONT_COPY, TRUE, IY_VERBOSE);
  return (nv);
}

NVptr iy_factorize_up_short_amb (NVptr nv)
{
  nv = factorize_shortamb_fst
    (NV_net (nv, 0), LOWER, DONT_COPY, TRUE, IY_VERBOSE);
  nv_reverse (nv);
  return (nv);
}

NVptr iy_unfactorize_down_short_amb (NVptr nv)
{
  NETptr net;

  net = unfactorize_amb_fsts
    (NV_net (nv, 0), NV_net (nv, 1), UPPER, DONT_COPY, IY_VERBOSE);
  nv = one_net_to_nv (net);
  return (nv);
}

NVptr iy_unfactorize_up_short_amb (NVptr nv)
{
  NETptr net;

  net = unfactorize_amb_fsts
    (NV_net (nv, 1), NV_net (nv, 0), LOWER, DONT_COPY, IY_VERBOSE);
  nv = one_net_to_nv (net);
  return (nv);
}

/*------------------------------------------------*/
NVptr iy_factorize_down_unambiguous (NVptr nv)
{
  nv = factorize_bm_fsts
    (NV_net (nv, 0), UPPER, DONT_COPY, TRUE, TRUE, IY_VERBOSE);
  return (nv);
}

NVptr iy_factorize_up_unambiguous (NVptr nv)
{
  nv = factorize_bm_fsts
    (NV_net (nv, 0), LOWER, DONT_COPY, TRUE, TRUE, IY_VERBOSE);
  nv_reverse (nv);
  return (nv);
}

NVptr iy_unfactorize_down_unambiguous (NVptr nv)
{
  NETptr net;

  net = unfactorize_bm_fsts
    (NV_net (nv, 0), NV_net (nv, 1), UPPER, DONT_COPY, IY_VERBOSE);
  nv = one_net_to_nv (net);
  return (nv);
}

NVptr iy_unfactorize_up_unambiguous (NVptr nv)
{
  NETptr net;

  net = unfactorize_bm_fsts
    (NV_net (nv, 1), NV_net (nv, 0), LOWER, DONT_COPY, IY_VERBOSE);
  nv = one_net_to_nv (net);
  return (nv);
}

/*------------------------------------------------*/
NVptr iy_align_down_finite_amb (NVptr nv)
{
  NETptr net;

  net = align_ambiguity (NV_net (nv, 0), UPPER, DONT_COPY, TRUE);
  nv = one_net_to_nv (net);
  return (nv);
}

NVptr iy_align_up_finite_amb (NVptr nv)
{
  NETptr net;

  net = align_ambiguity (NV_net (nv, 0), LOWER, DONT_COPY, TRUE);
  nv = one_net_to_nv (net);
  return (nv);
}

NVptr iy_unalign_finite_amb (NVptr nv)
{
  NETptr net;

  net = unalign_ambiguity (NV_net (nv, 0), DONT_COPY, TRUE);
  nv = one_net_to_nv (net);
  return (nv);
}

/*------------------------------------------------*/
NVptr iy_reduce_down_intermediate_alph (NVptr nv)
{
  /* Must consume the input nets but preserve the input vector. */

  nv = copy_nv_only (nv);

  nv = reduce_intermediate_alphabet_in_nv
    (nv, NULL, UPPER, DONT_COPY, TRUE, IY_VERBOSE);
  return (nv);
}

NVptr iy_reduce_up_intermediate_alph (NVptr nv)
{
  /* Must consume the input nets but preserve the input vector. */

  nv = copy_nv_only (nv);

  nv_reverse (nv);
  nv = reduce_intermediate_alphabet_in_nv
    (nv, NULL, LOWER, DONT_COPY, TRUE, IY_VERBOSE);
  nv_reverse (nv);

  return (nv);
}

/*------------------------------------------------*/
NVptr iy_test_down_fail_safety (NVptr nv)
{
  char *s;

  if (check_fail_safety (NV_net (nv, 0), UPPER))
    s = "1";
  else
    s = "0";

  fprintf (stderr, "%s, (0=NO,1=YES)\n", s);
  nv = one_net_to_nv (NV_net (nv, 0));
  return (nv);
}

NVptr iy_test_up_fail_safety (NVptr nv)
{
  char *s;

  if (check_fail_safety (NV_net (nv, 0), LOWER))
    s = "1";
  else
    s = "0";

  fprintf (stderr, "%s, (0=NO,1=YES)\n", s);
  nv = one_net_to_nv (NV_net (nv, 0));
  return (nv);
}

#endif  /* WORLD */

/****************************************************************
  COMMAND: substitute
 ****************************************************************/

ALPHABETptr iy_new_alph(void)
{
  ALPHABETptr alph = make_alph(global_alph_size(), BINARY_VECTOR);
  ALPH_len(alph) = 0;
  return(alph);
}

void iy_add_symbol(ALPHABETptr alph, id_type id)
{
  /* Adds ID to alph, even if it is EPSILON. */

  if (id == EPSILON)
    { 
      ALPH_item(alph, EPSILON) = 1;
      if (ALPH_len(alph) == 0)
	ALPH_len(alph) = (EPSILON +1);
    }
  else
    sigma_add_to(alph, id);
}

static void symbol_pair_warning(id_type def_id, id_type label_id)
{

  fsm_fprintf(stdout, "Symbol '");
  print_label(def_id, stdout, TRUE);
  fsm_fprintf(stdout, "' names a transducer and '");
  print_label(label_id, stdout, TRUE);
  fsm_fprintf(stdout, "' occurs in a symbol pair.\n");
  fsm_fprintf(stdout, "Cannot substitute.\n");

  quit_on_fail_error(IY_QUIT_ON_FAIL);
}

static void no_def_warning(id_type id)
{

  fsm_fprintf(stdout, "Symbol '");
  print_label(id, stdout, TRUE);
  fsm_fprintf(stdout, "' is undefined.\n");

  quit_on_fail_error(IY_QUIT_ON_FAIL);
}

static void not_found_warning(id_type id, char *type)
{

  fsm_fprintf(stdout, "No occurrences of %s '", type);
  print_label(id, stdout, TRUE);
  fsm_fprintf(stdout, "' in the network.\n");

  quit_on_fail_error(IY_QUIT_ON_FAIL);
}

NETptr iy_symsubst(NETptr net)
{
  NETptr result;
  id_type id = (id_type) (int) fs_arg1;
  ALPHABETptr alph = (ALPHABETptr) fs_arg2;

  if (sigma_member(NET_sigma(net), id) ||
      (id == EPSILON && occurs_as_label_component_p(id, NET_labels(net))))
    {
      result = substitute_symbol(id, alph, net,DONT_COPY, FALSE);
      free_alph(alph);
    }
  else
    {
      result = net;
      free_alph(alph);
      not_found_warning(id, "symbol");
    }
  
  if (IY_MINIMIZE_P)
    min_fsm(result, DONT_COPY);

  return(result);
}


NETptr iy_labsubst(NETptr net)
{
  NETptr result;
  id_type id = (id_type) (int) fs_arg1;
  ALPHABETptr alph = (ALPHABETptr) fs_arg2;

  if (sigma_member(NET_labels(net), id) ||
      (id == EPSILON && !NET_epsilon_free(net)))
    {
      result = substitute_label(id, alph, net, DONT_COPY, FALSE);
      free_alph(alph);
    }
  else
    {
      result = net;
      free_alph(alph);
      not_found_warning(id, "label");
    }

  if (IY_MINIMIZE_P)
    min_fsm(result, DONT_COPY);

  return(result);
}

NETptr iy_defsubst(NETptr net)
{
  NETptr result = net;
  id_type sym_id = (id_type) (int) fs_arg1;
  id_type def_id = (id_type) (int) fs_arg2;
  NETptr def = get_def_uncopied(def_id);

  if (def)
    {
      if (sigma_member(NET_sigma(net), sym_id))
	{
	  if (!(NET_arc_label_arity(def) == 2 &&
		occurs_as_label_component_p(sym_id, NET_labels(net))))
	    /* The last argument of the call to netsubst() determines
	       whether or not the alphabets are aligned, that is,
	       whether the instances of OTHER in the two networks
	       get expanded with respect to each other.

	       Here we align the insert with respect to the host
	       but not the other way round. Is that right? Should
	       there be a variable setting for this in the interface?
	    */
	    {
	      def = expand_other(def, NET_sigma(net), DO_COPY);
	      result = netsubst(sym_id, def, net, DONT_COPY, DONT_COPY,
				IY_MINIMIZE_P, FALSE);
	    }
	  else
	    symbol_pair_warning(def_id, sym_id);
	}
      else
	not_found_warning(sym_id, "symbol");
    }
  else
    no_def_warning(def_id);
  
  if (IY_MINIMIZE_P)
    min_fsm(result, DONT_COPY);

  return(result);
}

NETptr iy_ambclass(NETptr net)
{
  NETptr result = ambiguity_net(net, DONT_COPY, DO_MINIMIZE);

  if (null_fsm_p(result))
    quit_on_fail_error(IY_QUIT_ON_FAIL);

  return(result);
}

NETptr iy_compile_replace(NETptr net)
{
  NETptr result;
  int direction = (int) fs_arg1;
  int extract_p = (int) fs_arg2;

  int min_p = (IY_MINIMIZE_P ? DO_MINIMIZE : DONT_MINIMIZE);

  if (extract_p)
    result = extract_compile_replace(net, direction, DO_RECLAIM, min_p,
				     IY_RETOKENIZE_P);
  else
    result = compile_and_replace_self(net, direction, DO_RECLAIM, min_p,
				    IY_RETOKENIZE_P);

  if (net == result)
    quit_on_fail_error(IY_QUIT_ON_FAIL);

  return(result);
}

#ifndef WORLD

NETptr iy_removeepsilon (NETptr net)
{
  NETptr result = net;

  remove_epsilon(result);

  if (null_fsm_p(result))
    quit_on_fail_error(IY_QUIT_ON_FAIL);

  return(result);
}

/****************************************************************
  COMMAND: compression aka fsopt
 ****************************************************************/

NETptr iy_tb_insert(NETptr net)
{
  return((NETptr) insert_token_break_do_reclaim((cfsm_fsm) net));
}

NETptr iy_tagsep_insert(NETptr net)
{
  return((NETptr) insert_tag_separator((cfsm_fsm) net));
}

NETptr iy_encode_nonid(NETptr net)
{
  return ( (NETptr) nonid_transformation_for_compression ( (cfsm_fsm) net));
}

NETptr iy_tag_plus_strip_down (NETptr net)
{
  return ( (NETptr) remove_leading_plus_from_tags ( (cfsm_fsm) net));
}

NETptr iy_fsopt_reduce_sigma(NETptr net, 
			     int pointer_prefixes_desired,
			     int save_mapper_fsts, 
			     int fsopt_interactive,
			     char * filler)
{
 cfsm_fsm result=NULL, single_chars_net=NULL, tags_net=NULL;
 int keep_sc=TRUE, keep_tn=TRUE, preceding_escapes=FALSE;
 

  if ( net && (!  ALPH_type(sigma(net)) == BINARY_VECTOR))
    label_to_binary(sigma(net));
  if (binary_sigma_member (sigma(net), OTHER) )
    {
      fsm_fprintf (stderr, "\n\
*** CFSM ERROR *** Please, inform the programmer.\n\
Net contains OTHER.\n");
      quit_on_fail_error(IY_QUIT_ON_FAIL);
      return (NETptr) result;
    }

 if (yes_or_no("* FSOPT - Preceding escapes?", 'n'))
   {
     preceding_escapes=TRUE;
   }

 result=TEST_fsopt_reduce_sigma ((cfsm_fsm) net, 
				 pointer_prefixes_desired, 
				 preceding_escapes,
				 & single_chars_net,
				 & tags_net);
#ifdef DEBUG
 if (single_chars_net)
   keep_sc=yes_or_no("Put mapping net of single_characters onto the \
stack?", 'y');
#endif
 if (single_chars_net && keep_sc)
   {
     name_network((NETptr) single_chars_net, "FSOPT single characters mapping");
     iy_net_to_stack((NETptr) single_chars_net);
   }

#ifdef DEBUG
 if (tags_net)
   keep_tn=yes_or_no("Put mapping net of tags onto the \
stack?", 'y');
#endif
 if (tags_net && keep_tn)
   {
     name_network((NETptr) tags_net, "FSOPT tags mapping");
     iy_net_to_stack((NETptr) tags_net);
   }

#ifdef DEBUG
 if (single_chars_net &&
     yes_or_no("Max escape count in single ch map table?", 'y'))
   { 
     fsm_fprintf(stdout, "\tNumber of escapes in single ch map table=%d\n", 
	     fsopt_max_escape_count (single_chars_net));
   }
 if (tags_net &&
     yes_or_no("Max escape count in tag map table?", 'y'))
   {
     fsm_fprintf(stdout, "\tNumber of escapes in tag map table=%d\n", 
	     fsopt_max_escape_count (tags_net));
   }

 if(single_chars_net && 
    yes_or_no("Show full mapping of single ch map table?", 'y'))
   fsopt_show_full_mapping(single_chars_net, fsopt_max_escape_count (single_chars_net), preceding_escapes, filler);

 if(tags_net && 
    yes_or_no("Show full mapping of tag map table?", 'y'))
   fsopt_show_full_mapping(tags_net, fsopt_max_escape_count (tags_net), preceding_escapes, filler);
#endif

  /* cleanup */
 if (!keep_sc)
   free_network((NETptr) single_chars_net);
 if (!keep_tn)
   free_network((NETptr) tags_net);
 return (NETptr) result;
}

#endif /* WORLD */

NETptr iy_twosided_flag_diacritics(NETptr net)
{
  return ( (NETptr) flag_diacritics_two_sided( (cfsm_fsm) net));
}

/****************************************************************
  COMMAND: add path
 ****************************************************************/

#ifndef WORLD
NETptr iy_add_path_from_line (NETptr fst, char *path)
{

    /* This function pops the top network from the interface stack,
       enriches it with PATH and then pushes the resulting net back on
       the top of the stack.
       The argument PATH is parsed into a string of (multi-)character
       symbol PAIRS. By convention:
        - the string PATH may contain white spaces: they are ignored.
        - the parsing is done according to the net's UPPER parse_table AND
          the LOWER parse table,
        - the character ":" acts as a pair separator only,
        - 0 is treated as epsilon,
        - trailing blanks, tabs and linefeeds are ignored,
        - symbol "%" acts as an escape character,
    */
    
    PARSE_TBL upper_table, lower_table; 
    id_type path_ids[512], id;
    int i;

    if (!fst)
      return NULL;

    /* Retrieve the two parse tables of the net */
    upper_table= get_parse_table(fst, UPPER);
    lower_table= get_parse_table(fst, LOWER);
    
    /* Parsing the PATH into an array of pair id-s */
    i=0;
    while (pair_symbol_from_string(&id, path, &path, upper_table, lower_table, TRUE, TRUE, TRUE) != ERR_BAD_INPUT)
    {
        path_ids[i++]=id;
    }
    path_ids[i]= ID_EOS; /* marks the end of the id sequence */
    
    return(add_path(fst, path_ids));

} /* iy_add_path_from_line */


NETptr iy_add_path_from_file (NETptr fst, ifile *file)
{
    /* This function does the same work as iy_add_path_from_line(),
       but it takes its input from a text file, and enriches the FST
       with a list of paths, instead of adding a single path. */
    FILE *f;
    char LINE[512];
    
    f = fopen(file->name,"rt");
    if (!f)
        printf("Could not open file %s\n", file->name);
    else
    {
        while(fgets(LINE, sizeof(LINE), f))
        {
            if (LINE[strlen(LINE) - 1] == '\n')
            {
                LINE[strlen(LINE) - 1] = '\0';
            }
            
            if (LINE[0] != '\0' )  
                fst= iy_add_path_from_line(fst, LINE);
            else
                continue;
        }
        fclose(f);
    } /* if f = fopen... */
 return(fst);
} /* iy_add_path_from_file */



NETptr iy_add_word_from_line (NETptr fst, char *word, char* model)
{
    /* This function aims at enriching an FST with all the inflected
       forms of a WORD following a Model word whose inflected forms
       are already part of the FST.
       It pops the top network from the interface stack, enrich it with
       the paths resulting from WORD and MODEL, and then push the resulting
       net back on the top of the stack.
       The arguments WORD and MODEL are NOT parsed as symbol pairs but
       only as two distinct "one-sided" strings of (multi-)character symbols.
       By convention:
        - the strings word and model should not contain white spaces,
        - the parsing is done according to the net's UPPER parse_table only,
        - the character ":" does NOT act as a pair separator; it is treated
          as any other character,
        - 0 is not treated as epsilon,
        - trailing blanks, tabs and linefeeds are ignored,
        - symbol "%" does not act as an escape character,
        - Regular expressions are not handled. */
    /* NOT YET IMPLEMENTED ... */
    
    PARSE_TBL upper_table;
    id_type word_ids[512], model_ids[512], id;
    int i;

    /* Retrieve the two parse tables of the net */
    upper_table= get_parse_table(fst, UPPER);
    
    /* Parsing of WORD */
    i=0;
    while (symbol_from_string(&id, word, &word, upper_table, FALSE, TRUE, FALSE) != ERR_BAD_INPUT)
    {
        word_ids[i++]=id;
    }
    word_ids[i]= ID_EOS; /* marks the end of the id sequence */

    /* Parsing of MODEL */
    i=0;
    while (symbol_from_string(&id, model, &model, upper_table, FALSE, TRUE, FALSE) != ERR_BAD_INPUT)
    {
        model_ids[i++]=id;
    }
    model_ids[i]= ID_EOS; /* marks the end of the id sequence */
    
    return( add_word(fst, word_ids, model_ids) ); /* Not yet implemented */
} /* iy_add_word_from_line */


int next(id_type* word, id_type* model, void* user_data)
{
    /*  to be implemented ...  */
    return(0);
}

NETptr iy_add_word_from_file (NETptr fst, ifile *file)
{
    /* to be implemented ... */
    /*  NETptr net; */
    /* net= enrich_network(fst, (void *) file->name, next); */
    
    fsm_printf("add-words < file is not yet implemented ...\n");
    return(fst);
}

#endif /* WORLD */

/****************************************************************
  Character encoding

 ****************************************************************/

int iy_set_char_encoding(int valid_code, char *err_str)
{
  if (err_str)
    {
      fsm_fprintf(stderr, "Cannot set character encoding to '%s'.\n\
Only UTF-8 and ISO-8859-1 are currently supported.\n", err_str);
      quit_on_fail_error(IY_QUIT_ON_FAIL);
      return(1);
    }
  else
    {
      switch(valid_code)
	{
	case CHAR_ENC_UTF_8:
	case CHAR_ENC_ISO_8859_1:
	  IY_CHAR_ENCODING = valid_code;
	  return(init_string_functions(valid_code));
	  break;
	default:
      fsm_fprintf(stderr, "Character encoding code 'd' is not valic.\n\
Only %d (UTF-8) and %d (ISO-8859-1) are currently supported\n",
		  CHAR_ENC_UTF_8, CHAR_ENC_ISO_8859_1);
      quit_on_fail_error(IY_QUIT_ON_FAIL);
      return(1);	  
	  break;
	}
    }
}

#ifndef WORLD
/****************************************************************
  Memory mapping

 ****************************************************************/
void iy_show_use_mmap()
{
    if (IY_USE_MMAP)
        fsm_fprintf(stdout, "%s\n", "ON");
    else
        fsm_fprintf(stdout, "%s\n", "OFF");
}

void iy_set_use_mmap(int use_mmap)
{
    IY_USE_MMAP= use_mmap;
    
    if (IY_USE_MMAP)
        fsm_fprintf(stdout, "use-mmap set to %s.\n", "ON");
    else
        fsm_fprintf(stdout, "use-mmap set to %s.\n", "OFF");
    
    MEMORY_MAP= use_mmap;
}
#endif

/****************************************************************
  COMMANDS to create reclaimable virtual networks.

 ****************************************************************/

#ifndef WORLD  
void iy_show_license_type()
{
  printf ("%s\n", decode_license_type(get_user_license_type()));
}

void iy_set_license_type(int license_type)
{
  if (valid_license_type(license_type))
    {
      set_user_license_type(license_type);
      fsm_fprintf(stdout, "License type set to %s.\n",
		  decode_license_type(license_type));
      IY_LICENSE_TYPE = license_type;
    }
  else
    {
      fsm_fprintf(stderr, "*** ERROR: %d is not a valid license type\n");
      quit_on_fail_error(IY_QUIT_ON_FAIL);
    }
}

#endif /* WORLD */

