/* $Id: test-compression.c,v 1.4 2005/03/04 15:45:30 grondeau Exp $ */
/* Copyright (c) 1998 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  test-compression.c
 **
 ** Tamas Gaal, XRCE, March 1998
 **
 ** Kaplan-compression tests, preparatory steps for
 ** the compression and
 ** ancillary functions for the c-fsm library.
 **
 ******************************************************/


#include "fst/cfsm.h"
#include "fst/calculus.h"
#include "fst/varstack.h"
#include "fst/regex-in.h"

#include "fst/bin-out.h"
#include "fst/flags.h"
#include "fst/fsm_io.h"
#include "fst/label.h"
#include "fst/look.h"
#include "fst/pars-tbl.h"
#include "fst/pretty-p.h"
#include "fst/prompt.h"
#include "fst/subseq.h"
#include "fst/context.h"
#include "fst/types.h"
#include "fst/mark.h"

#include "fst/fsopt/test-compression.h"



/* Here are functions to convert between cfsm.h labels and id_types, and
   between symbols and id_types. */

static cfsm_label convert_id_to_label(id_type id) {
    if (id == ID_NO_SYMBOL)
        return cfsm_the_invalid_label();
    return cfsm_number_to_label(id);
}
static cfsm_symbol convert_id_to_symbol(id_type id) {
    if (id == ID_NO_SYMBOL)
        return cfsm_the_invalid_symbol();
    return cfsm_number_to_symbol(id);
}
static id_type convert_label_to_id(cfsm_label l) {
    if (cfsm_valid_label(l))
        return cfsm_label_number(l);
    return ID_NO_SYMBOL;
}
static id_type convert_symbol_to_id(cfsm_symbol s) {
    if (cfsm_valid_symbol(s))
        return cfsm_symbol_number(s);
    return ID_NO_SYMBOL;
}



CFSM_VARptr create_cfsm_var( char * name, cfsm_fsm net)
{
  CFSM_VARptr new_var  =   (CFSM_VARptr) malloc( (size_t) sizeof (CFSM_VARtype));
  if (! new_var)
    not_enough_memory ("CREATE_CFSM_VAR");
  CFSM_VAR_name(new_var) = name;
  CFSM_VAR_fst(new_var) = net;
  CFSM_VAR_link(new_var) = NULL;
  return (new_var);
}

void free_cfsm_var (CFSM_VARptr cv, int free_the_string, int free_the_fst)
{
  if (cv)
    {
      if (free_the_string)
	free (CFSM_VAR_name(cv));
      if (free_the_fst)
	free_network ((NETptr) CFSM_VAR_fst(cv));
      free(cv);
    }
  return;
}

void free_cfsm_varlist (CFSM_VARptr vl, int free_the_string, int free_the_fst)
{
  CFSM_VARptr prev = NULL;
  while (vl)
    {
      prev = CFSM_VAR_link(vl);
      free_cfsm_var(vl, free_the_string, free_the_fst );
      vl = prev;
    }
  return;
}

CFSM_VARptr create_and_prepend_cfsm_var( char * name, cfsm_fsm net, CFSM_VARptr list)
{
  CFSM_VARptr new_v =  create_cfsm_var( name, net);
  CFSM_VAR_link( new_v) = list;
  return(new_v);
}

/* cfsm_regex creates a network from a string that contains a regular expression.
   "regex_string" is a string that may contain internal regex variables in this
   syntax:  $(variable_name_i) . The variables must be enumerated in
   "varlist" as <name, transducer> pairs. 
   */

cfsm_fsm cfsm_regex (char * regex_string, CFSM_VARptr varlist)
{
  RE2N_VSTptr vst = NULL;  
  NETptr net = NULL;
  CFSM_VARptr var = NULL;

  vst = RE2N_create_var_stack ();

  if (varlist)
    for ( var = varlist ; var ; var =  CFSM_VAR_link(var) )
      {
	if (CFSM_VAR_name(var) &&  CFSM_VAR_fst(var))
	    RE2N_save_var (vst, CFSM_VAR_name(var), (NETptr) CFSM_VAR_fst(var));
	else
	  {
	   fsm_fprintf (stderr, "\n\
*** CFSM INTERNAL PROGRAMMING ERROR ***   Please, inform the programmer.\n\
Attempt to use a NULL name : %s or NULL fst %d\n", CFSM_VAR_name(var),
		 (int)   CFSM_VAR_fst(var));
	   exit (1);
	  }
      }
 
  if ( vst )
    {
      net = RE2N_create_net (vst, regex_string);
      RE2N_remove_var_stack (vst, FALSE);
    }
  return (cfsm_fsm) net;
}

void fsopt_substitute_single_symbol(cfsm_fsm net, cfsm_symbol old, cfsm_symbol new)
{
  cfsm_arc ari;
  cfsm_label li, new_li;
  cfsm_state sti;
  cfsm_symbol up, low;
  int change=FALSE;

  for (sti = cfsm_state_list(net) ; sti ; sti = cfsm_next_state(sti, net), new_li = 0)
    {
      for (ari = cfsm_first_arc(sti, net) ; ari ; ari = cfsm_next_arc(ari, net))
	{
	  li = cfsm_arc_label( ari, net);
	  if ( cfsm_label_arity(li, net) > 1)
	    {	     
	      up = cfsm_label_upperside( li, net) ;
	      low = cfsm_label_lowerside( li, net) ;
	      
	      if (cfsm_symbol_eq(low, old))
		{
		  new_li = cfsm_make_fst_label (up, new);
		  ARC_label( (ARCptr) ari) =  convert_label_to_id(new_li);
		  change = TRUE;
		}
	      else if (cfsm_symbol_eq(up, old))
		{
		  new_li = cfsm_make_fst_label (new, low);
		  ARC_label( (ARCptr) ari) =  convert_label_to_id(new_li);
		  change = TRUE;
		}
	    }
	  else if (cfsm_symbol_eq(cfsm_label_lowerside(li,net), old))
	    {
	      ARC_label( (ARCptr) ari) = convert_symbol_to_id(new);
	      change = TRUE;
	    }
	}
    }
  if (change)
    cfsm_normal_form_net(net);
}

static cfsm_fsm insert_end_of_string(cfsm_fsm net, int reclaim_p)
/* 
   Mark end of lower-side strings by special symbols called
   "*EOS*". The *EOS*  must be to the left of any lower-side EPSILON-s on 
   the right-hand end of the path.*EOS* means end-of-symbol (former
   TB or token break).
   Ignore ALTCHAIN-s and flag diacritics (EOS must precede them if there is no
   further other symbol (then EPSILON or ALTCHAIN or flag diacritics).

   If reclaim_p is DO_RECLAIM then the original net is freed.
   */
{
  int error;
  ALPHABETptr epsalph=NULL, flagdalp=NULL, siga=NULL;
  CFSM_VARptr prev=NULL, varlist=NULL;
  cfsm_state newstart=NULL, newstart_next=NULL;
  FLAG_DIACRptr flag_diacr;
  int i, must_subst=FALSE, null_lang_added=FALSE;
  id_type epsid, nullepsid;
  NETptr altchainnet=NULL, flagdnet = NULL, realsigmanet=NULL, sigmanet=NULL, 
    EOSizer=NULL, EOSnet=NULL ;
 
  if (!net)
    return (cfsm_fsm) EOSnet;
 
  /* Hacking [] subtitution by *EPS*:
     it should be done by EOSizer but fst 6.6.1 is unable to do it. */

  siga =  sigma((NETptr) net);

  error = intern_symbol (&epsid, "*EPS*"); /* **  cfsm_make_symbol() ? */
  error = intern_symbol (&nullepsid, "*EPS_NULL*"); 
  if (cfsm_state_final(cfsm_start_state(net), net))
    {
      newstart = cfsm_add_state(FALSE, net);
      newstart_next = cfsm_add_state(TRUE, net);
      cfsm_add_arc( convert_id_to_label(nullepsid), newstart, newstart_next, net);
      cfsm_add_arc( convert_id_to_label(EPSILON), newstart, cfsm_start_state(net), net);
      sigma_add_to(siga, nullepsid);
      cfsm_set_start_state(net, newstart);
      null_lang_added = TRUE;
    }
  
  /* Sigma as a net */

  sigmanet = sigma_fsm(siga);
  varlist = create_cfsm_var("SIGMA", (cfsm_fsm) sigmanet); 

  /* net of "*ALTCHAIN*-s (as a language)
   */
  altchainnet = (NETptr) cfsm_regex ("[ \"*ALTCHAIN*\" ]+", varlist);
  varlist = create_and_prepend_cfsm_var("ALTCHAINNET", (cfsm_fsm) altchainnet, varlist);

  /* net of flag diacritics */
  flagdalp=make_alph(global_alph_size(), BINARY_VECTOR);
  ALPH_len(flagdalp) =0;
  siga = label_to_binary(siga);

  for (i = 0; i < ALPH_len(siga); i++)
    if (ALPH_item(siga, i) && (flag_diacr = LABEL_flag(id_to_label((id_type) i))))
      sigma_add_to(flagdalp, (id_type) i);

  flagdnet = sigma_fsm(flagdalp);
  varlist = create_and_prepend_cfsm_var("FLAGDNET",  (cfsm_fsm) flagdnet, varlist);
  free_alph(flagdalp);

  /* Subtract *ALTCHAIN*-s and Flag Diacritics from SIGMA ! VERY important.
     Allow empty string.
   */
  realsigmanet = (NETptr) cfsm_regex ("[] | $(SIGMA) - $(ALTCHAINNET) - $(FLAGDNET)", varlist);
  varlist = create_and_prepend_cfsm_var("RSIG", (cfsm_fsm) realsigmanet, varlist); 

  /* Define the ancillary transducer wrt the restricted sigma (no ALTCHAIN, no Flag Diacritics) of the original one. 
   */
  EOSizer = (NETptr) cfsm_regex
    (" [. .] -> \"*EOS*\" \\/ $(RSIG) _ [ $(FLAGDNET) | \"*ALTCHAIN*\" | \"*EPS*\" ]* .#.", varlist);

  varlist = create_and_prepend_cfsm_var("EOSIZER", (cfsm_fsm) EOSizer, varlist);

  /* Substitute real EPSILON-s with something else. 
   */

  epsalph = make_alph(global_alph_size(), BINARY_VECTOR);
  ALPH_len(epsalph) = 0;
  sigma_add_to(epsalph, epsid);
  if (( must_subst = occurs_as_label_component_p(EPSILON,
						  NET_labels((NETptr) net))))
       net =  (cfsm_fsm) substitute_symbol(EPSILON, epsalph, (NETptr) net, DONT_COPY, FALSE);
  free_alph(epsalph);
  varlist = create_and_prepend_cfsm_var("NET", (cfsm_fsm) net, varlist); 
 
  /* perform EOS append via composition.
   */
  EOSnet = (NETptr) cfsm_regex ("$(NET) .o. $(EOSIZER)", varlist);

  /* re-substitute real EPSILON-s  (0-s) in place of *EPS*-s - or *EPS_NULL*. 
   */
  if (must_subst || null_lang_added)
    {
      epsalph = make_alph(global_alph_size(), BINARY_VECTOR);
      ALPH_len(epsalph) = 0;
      ALPH_item(epsalph, EPSILON) = 1;
      if (ALPH_len(epsalph) == 0)
	ALPH_len(epsalph) = (EPSILON +1);
      EOSnet = substitute_symbol(epsid, epsalph, EOSnet, DONT_COPY, FALSE);
      if (null_lang_added)
	EOSnet = substitute_symbol(nullepsid, epsalph, EOSnet, DONT_COPY, FALSE);
      free_alph(epsalph);
    }

  prev = NULL;
  while (varlist)
    {
      prev = CFSM_VAR_link(varlist);
      if ( CFSM_VAR_fst(varlist) != net)
	free_cfsm_var(varlist, FALSE, TRUE );
      else
	{
	  if (reclaim_p == DO_RECLAIM)
	    free_cfsm_var(varlist, FALSE, TRUE );
	  else
	    free_cfsm_var(varlist, FALSE, FALSE );
	}
      varlist = prev;
    }

  /* minimize net, sort arcs - *ALTCHAIN*-s be the last  */
  cfsm_normal_form_net((cfsm_fsm) EOSnet);

  /*  Hack: if the null language was part of the original net then this path
      has been suffixed by an EOS already so the start state must not be
      final. */
  if (EOSnet && NET_start_state(EOSnet) && null_lang_added)
    STATE_final(NET_start_state(EOSnet)) = FALSE;

  return (cfsm_fsm) EOSnet;
}

cfsm_fsm insert_token_break(cfsm_fsm net)
{
  return(insert_end_of_string(net, DONT_RECLAIM));
}

cfsm_fsm insert_token_break_do_reclaim(cfsm_fsm net)
     /* Do reclaim the input net (will be freed!)*/
{
  return(insert_end_of_string(net, DO_RECLAIM));
}

cfsm_fsm insert_tag_separator(cfsm_fsm net)
     /* prepares a transducer for compression by inserting a transition labelled
      *TAGSEP* between ordinary symbols and tags. */
{
  NETptr sigmanet = NULL, scs1 = NULL, scs2 = NULL, tagsfst = NULL,  notagsfst = NULL, 
    tagsep = NULL, tagsepnet = NULL, altchainnet = NULL;  
  CFSM_VARptr varlist = NULL ;

  /* net of sigma 
   */
  sigmanet = sigma_fsm (sigma((NETptr) net));
  varlist = create_cfsm_var("SIGMA", (cfsm_fsm) sigmanet);

  /* net of extended set of single-character symbols 
   */
  scs1 = single_char_sigma_fsm (sigma((NETptr) net));
  scs2 = single_char_sigma_fsm (sigma(scs1));
  free_network (scs1);
  varlist = create_and_prepend_cfsm_var("SCS2", (cfsm_fsm) scs2, varlist); 

  /* net of "*ALTCHAIN* -s as a language 
   */
  altchainnet = (NETptr) cfsm_regex ("[ \"*ALTCHAIN*\" ]+", varlist);
  varlist = create_and_prepend_cfsm_var("ALTCHAINNET", (cfsm_fsm) altchainnet, varlist); 

  /* net of multi-character symbols (approximate tags) without (!) *ALTCHAIN*
   */
  tagsfst = (NETptr) cfsm_regex ("$(SIGMA) - $(SCS2) - $(ALTCHAINNET)", varlist);
  varlist = create_and_prepend_cfsm_var("TAGS", (cfsm_fsm) tagsfst, varlist);

  /*  net of real set of single-character symbols 
   */
  notagsfst = (NETptr) cfsm_regex ("$(SIGMA) - $(TAGS) - $(ALTCHAINNET)", varlist);
  varlist = create_and_prepend_cfsm_var("NOTAGS", (cfsm_fsm) notagsfst, varlist);

  /*
  tagsep = cfsm_regex ("[. .] -> [  \"*TAGSEP*\" ] || $(NOTAGS) _ $(TAGS), $(TAGS) _ $(NOTAGS)",
		       varlist) ; */
  tagsep =  (NETptr) cfsm_regex ("[. .] -> [\"*TAGSEP*\"] || .#. _ $(TAGS), [$(NOTAGS)/$(ALTCHAINNET)] _ $(TAGS), [$(TAGS)/$(ALTCHAINNET)] _ $(NOTAGS)", varlist);

  varlist = create_and_prepend_cfsm_var("TAGSEP", (cfsm_fsm) tagsep, varlist);
  varlist = create_and_prepend_cfsm_var("FST", net, varlist);

  tagsepnet = (NETptr) cfsm_regex ("$(TAGSEP).i .o. $(FST) .o. $(TAGSEP)", varlist) ;

  /* delete all networks including the call parameter "net" here */
  free_cfsm_varlist(varlist, FALSE, TRUE);

  /* minimize tagsepnet sort arcs - *ALTCHAIN*-s be the last  */
  cfsm_normal_form_net((cfsm_fsm) tagsepnet);
  return (cfsm_fsm) tagsepnet;
}

#define escape_label "*ESCAPE*"
#define eos_label "*EOS*"
#define instruction_label "*INSTRUCTION*"
#define nonid_label "*NONID*"
#define one_sided_epsilon "*1SIDEDEPSILON*"
#define table_label "*TABLE*"
#define tagsep_label "*TAGSEP*"

static ALPHABETptr create_diacritic_transformation_exceptions(void)
     /* Create an alphabet with special diacritics that mustn't be
	transformed from
	*DIACRITIC* to *1SIDEDEPSILON* *1SIDEDEPSILON*
	in nonid_transformation_for_compression() */
{
  id_type new_id;
  ALPHABETptr result = make_alph(global_alph_size(), BINARY_VECTOR);

  intern_symbol(&new_id, escape_label);
  sigma_add_to(result, new_id);

  intern_symbol(&new_id, eos_label);
  sigma_add_to(result, new_id);

  intern_symbol(&new_id, tagsep_label);
  sigma_add_to(result, new_id);

  intern_symbol(&new_id, nonid_label);
  sigma_add_to(result, new_id);

  intern_symbol(&new_id, table_label);
  sigma_add_to(result, new_id);

  intern_symbol(&new_id, instruction_label);
  sigma_add_to(result, new_id);

  sigma_add_to(result, OTHER);
  sigma_add_to(result, BOUNDARY_SYMBOL);
  sigma_add_to(result, ALTCHAIN_SYMBOL);
  sigma_add_to(result, DUMMY_LABEL);
  if (! (ALPH_type(result) == BINARY_VECTOR))
    label_to_binary(result);
  return result;
}

inline
static int forbidden_diacritic(id_type id, ALPHABETptr exceptions)
     /* exceptions alphabet must be in binary form! */
{
  if (binary_sigma_member(exceptions, id))
    return TRUE;
  return FALSE;
}

static int cfsm_is_epsilon_diacritic(cfsm_symbol sym, ALPHABETptr exceptions)
     /* determine whether this is an epsilon diacritic or not:
	"*diacritic*" strings are accpeted if not in the exceptions set */
{
  FAT_STR str=LABEL_atom(id_to_label(convert_symbol_to_id(sym)));
  int len = cfsm_symbol_length(sym);

  if (forbidden_diacritic(convert_symbol_to_id(sym), exceptions))
    return FALSE;

  len = fat_strlen(str);
  if ( len > 2  && CHARNUM(str) == '*' && CHARNUM(str+len-1) == '*' )
    return TRUE;
  else 
    return FALSE;
}

cfsm_fsm nonid_transformation_for_compression(cfsm_fsm net)
     /* Try using cfsm interface functions of M. Wilkens */
{
  /* Convert an arc with a label <a:b> into a sequence of arcs and states:
     b *NONID* a (sic!)
     Convert <a:0> to  *1SIDEDEPSILON* a
     Convert <0:a> to  a *NONID* *1SIDEDEPSILON*
     and replace the arc with the new sequence.
     Convert 
	*DIACRITIC* ==> *1SIDEDEPSILON* *1SIDEDEPSILON*
	*DIACRITIC*:a ==> a *NONID* *1SIDEDEPSILON*
	a:*DIACRITIC* ==> *1SIDEDEPSILON* a
     Except for the following - do not convert:
        fst:
	  *ALTCHAIN*, **EPSILON**, 
	  **OTHER**, **DUMMY_SYMBOL**, **BOUNDARY_SYMBOL** - these latter three
	  won't occur normally in input.   
	fsopt:
	  *EOS*, *TAGSEP*, *ESCAPE*,
	  *INSTRUCTION*, *TABLE*, *NONID* - these latter three won't occur
	  normally in input.

     */
  ALPHABETptr nonepsdcrs=create_diacritic_transformation_exceptions();
  cfsm_state sti, st1, st2 ;
  cfsm_arc ari;
  cfsm_label li;
  cfsm_symbol nonidl = 0, up, low, xch;
  int first = TRUE;

  for (sti = cfsm_state_list(net), first = TRUE ; sti ; sti = cfsm_next_state(sti, net))
    {
      for (ari = cfsm_first_arc(sti, net) ; ari ; ari = cfsm_next_arc(ari, net))
	{
	  if ( cfsm_label_arity( cfsm_arc_label (ari, net), net) > 1)
	    {
	      li = cfsm_arc_label( ari, net);
	      if (first)
		{
		  first = FALSE;
		  nonidl = cfsm_make_symbol (nonid_label, net);
		}
	      up =  cfsm_label_upperside( li, net) ;
	      low = cfsm_label_lowerside( li, net) ;
	      
	      if ( cfsm_symbol_eq( low, cfsm_epsilon_symbol()) ||
		  cfsm_is_epsilon_diacritic(low, nonepsdcrs) )
		{
		  /* <a:0>  ==>  *1SIDEDEPSILON* a
		     <a:*DIACRITIC*> ==> *1SIDEDEPSILON* a
		   */		    
		  low = cfsm_make_symbol( one_sided_epsilon, net);
		  xch = low;
		  if ( ! ( cfsm_symbol_eq(up, cfsm_epsilon_symbol()) ||
			   cfsm_is_epsilon_diacritic(up,  nonepsdcrs) ))
		    low = up;
		  up = xch;
		  st1 = cfsm_add_state(FALSE, net);
		  cfsm_add_arc(cfsm_make_label(low), st1, cfsm_arc_destination (ari, net), net);

		  /* Next two lines do NOT conform to cfsm yet. 98-3-10 TG */
		  ARC_label( (ARCptr) ari) =  convert_symbol_to_id(up);
		  ARC_destination( (ARCptr) ari) = (STATEptr) st1;
		}
	      else
		{
		  if ( cfsm_symbol_eq( up, cfsm_epsilon_symbol()) ||
		       cfsm_is_epsilon_diacritic(up, nonepsdcrs))
		    {
		      /* <0:a>  ==>  a *NONID* *1SIDEDEPSILON*
			 <*DIACRITIC*:a> ==> a *NONID* *1SIDEDEPSILON*
			*/
		      up = cfsm_make_symbol( one_sided_epsilon, net);
		    }
		  st1 = cfsm_add_state(FALSE, net);
		  st2 = cfsm_add_state(FALSE, net);
		  cfsm_add_arc(cfsm_make_label(nonidl), st1, st2, net);
		  cfsm_add_arc(cfsm_make_label(up), st2, cfsm_arc_destination (ari, net), net);

		  /* Next two lines do NOT conform to cfsm yet. 98-3-9 TG  */
		  ARC_label( (ARCptr) ari) =  convert_symbol_to_id(low);
		  ARC_destination( (ARCptr) ari) = (STATEptr) st1;
		}
	    }
	  else if (convert_symbol_to_id
                   (cfsm_label_lowerside(cfsm_arc_label(ari,net), net)) != ALTCHAIN_SYMBOL &&
		   cfsm_is_epsilon_diacritic
                   (cfsm_label_lowerside(cfsm_arc_label (ari,net), net), nonepsdcrs))
	    /* *DIACRITIC* ==> *1SIDEDEPSILON* *1SIDEDEPSILON* */
	    {
	      if (first)
		{
		  first = FALSE;
		  nonidl = cfsm_make_symbol (nonid_label, net);
		}
	      up =  cfsm_make_symbol( one_sided_epsilon, net);
	      low = up;
	      st1 = cfsm_add_state(FALSE, net);
	      cfsm_add_arc(cfsm_make_label(low), st1, cfsm_arc_destination (ari, net), net);
	      ARC_label( (ARCptr) ari) =  convert_symbol_to_id(low);
	      ARC_destination( (ARCptr) ari) = (STATEptr) st1;
	    }
	}
    }
  if (! first)
    {
      cfsm_normal_form_net(net);
    }

  free_alph(nonepsdcrs);
  return net;
}

#define CFSM_SUBST_BUFFER_LENGTH 256

static int cfsm_subst_arclab (cfsm_fsm net, cfsm_label oldlab,  cfsm_label newlab, cfsm_arc arc )
     /* newlab must not be part of the sigma (?) */
{
  int result = 0;
  if (! binary_sigma_member (NET_labels ((NETptr) net), convert_label_to_id(oldlab)))
    ARC_label( (ARCptr) arc) =  convert_label_to_id(newlab);
  else
    {
      fsm_fprintf (stderr, "\n\
*** CFSM INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
label clash: old = %d new = %d .\n", (int) oldlab, (int) newlab);
      result = 1;
    }
  return result;      
}

static cfsm_symbol cfsm_make_substitute_symbol (cfsm_symbol old, cfsm_fsm net)
     /* If this tag (== multi-character symbol) has more than two characters
	and the first character is a + sign
	and the remaining can't be conceived as a diacritic (like "+*diacritic*")
	or a flag diacritic (like "+@U.A1.V1@")
	then strip the leading + and create a new cfsm symbol (a label id).
	Raise and exception when the new symbol can be conceived as a
	flag diacritic or an epsilon diacritic.
	*/
{
  cfsm_symbol  result = 0;
  char tagbuf[CFSM_SUBST_BUFFER_LENGTH] = {0};
  if ( cfsm_symbol_tag_p (old) &&
       (CFSM_NO_ERROR ==  cfsm_symbol_name(old, tagbuf,
					   CFSM_SUBST_BUFFER_LENGTH)) &&
       tagbuf[0] == '+' && tagbuf[2] )
    {
      if ((tagbuf[1] == '*') || (tagbuf[1] == '@'))
	{
	  fsm_fprintf (stderr, "\n\
*** CFSM INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
Incorrect tag: ");
	  print_label(convert_symbol_to_id(old), stderr, DONT_ESCAPE);
	  fsm_fputs("\n",stderr);
	  exit(1);
	}	
      else
	result = cfsm_make_symbol( tagbuf+1, net);
    }
  return result;
}

cfsm_fsm remove_leading_plus_from_tags (cfsm_fsm net)
{
  /* Following a convention from Xerox PARC, +Tag names are rewritten as Tag
     if the remaining string (Tag) has more than one character and
     remains a tag, that is, a multi-character symbol which is neither
     a flag diacritic (aka instruction) nor an epsilon diacritic.
   */
     
  cfsm_state sti = NULL ;
  cfsm_arc ari;
  cfsm_label li, new_li;
  cfsm_symbol up, low, new_up, new_low, li_sy;
  int was_subst = FALSE;
 
  for (sti = cfsm_state_list(net); sti ; sti = cfsm_next_state(sti, net))
    {
      for (ari = cfsm_first_arc(sti, net) ; ari ; ari = cfsm_next_arc(ari, net))
	{
	  li = cfsm_arc_label( ari, net);
	  if ( cfsm_label_arity( li, net) > 1)
	    {
	      new_up = 0;
	      new_low = 0;
	      up = cfsm_label_upperside( li, net) ;
	      low = cfsm_label_lowerside( li, net) ;
	      new_up = cfsm_make_substitute_symbol (up, net);
	      new_low =  cfsm_make_substitute_symbol (low, net);
	      if (new_up || new_low)
		{
		  if (new_up)
		    {
		      up = new_up;
		      if (new_low)
			low = new_low;
		    } 
		  else if (new_low)
		    {
		      low = new_low;
		      if (new_up)
			up = new_up;
		    }
		 new_li =  cfsm_make_fst_label (up, low);
		 cfsm_subst_arclab (net, li, new_li, ari);
		 was_subst = TRUE;
		}   
	    }
	  else
	    {
	      li_sy = cfsm_label_symbol (li, net);
	      new_li = cfsm_make_label(cfsm_make_substitute_symbol (li_sy, net));
	      if (new_li)
		{
		  cfsm_subst_arclab (net, li, new_li, ari);
		  was_subst = TRUE;
		}
	    }	    
	}
    }
  if (was_subst)
    {
      cfsm_normal_form_net(net);
    }
  return net;
}


#define BASECHAR_MIN 1
#define BASECHAR_DEFAULT 30
#define BASECHAR_MAX 63
#define DICTCODE_MAX 256
#define POINTER_PREFIX_PER_BASECHAR 4

void print_fsopt_preprocess_result (cfsm_preprocess_resultptr ppr)
{
  fsm_fprintf(stdout, "Tags on upper side: %d.\n",  
	      CfsmFsoptPrepr_tags_on_upper_side(ppr));
  fsm_fprintf(stdout, "Tags on lower side: %d.\n",
	      CfsmFsoptPrepr_tags_on_lower_side(ppr));
  fsm_fprintf(stdout, "Sigma needs to be reduced: %d.\n",
	      CfsmFsoptPrepr_sigma_to_be_reduced(ppr));
  fsm_fprintf(stdout, "One-sided epsilon insertion needed %d.\n",
	      CfsmFsoptPrepr_one_sided_epsilon_needed(ppr));
  fsm_fprintf(stdout, "Nonid-recoding needed: %d.\n",
	      CfsmFsoptPrepr_nonid_recoding_needed(ppr));
  fsm_fprintf(stdout, "Instruction symbol neded: %d.\n",
	      CfsmFsoptPrepr_instruction_symbol_neded(ppr));
  fsm_fprintf(stdout, "End of String symbol needed: %d.\n",
	      CfsmFsoptPrepr_EOS_symbol_needed(ppr));
  fsm_fprintf(stdout, "Real number of available pointer prefixes: %d.\n",
	      CfsmFsoptPrepr_number_of_pointer_prefixes(ppr));
  fsm_fprintf(stdout, "Number of basechars: %d.\n",
	       CfsmFsoptPrepr_number_of_basechars(ppr));
}

void fsopt_preprocess_fsm (cfsm_fsm net, 
			   int pointer_prefixes_desired,
			   cfsm_preprocess_resultptr ppres)
{
  /*
    Preprocess the transducer. Fill in the fields in the result array.
   */
  ALPHABETptr labelsa = NULL, sigmaa = NULL;
  int i, basechar_add = 0, tus = FALSE, tls = FALSE, osen = FALSE;
  id_type id, eossymbol, up, lo ;

  if (! ( net && ppres ))
    return;
  if (! ( ( POINTER_PREFIX_PER_BASECHAR * BASECHAR_MIN <=
	    (DICTCODE_MAX - pointer_prefixes_desired))
	    &&  ( pointer_prefixes_desired< DICTCODE_MAX)))
    {
      fsm_fprintf (stderr, "\n\
*** CFSM INTERNAL PROGRAMMING ERROR ***   Please, inform the programmer.\n\
Incorrect input pointer_prefix number value: %d.\n", pointer_prefixes_desired);
      exit (1) ;
    }

  /* FST or FSM ? */
  CfsmFsoptPrepr_nonid_recoding_needed(ppres) = FALSE;
  if (NET_arc_label_arity((NETptr) net) >1)
    {
      CfsmFsoptPrepr_nonid_recoding_needed(ppres) = TRUE;
      basechar_add++;
    }

  /*  Presence of Flag diacritics, aka Instructions. */
  if (has_flag_diacritics_p ((NETptr) net))
    {
      CfsmFsoptPrepr_instruction_symbol_neded(ppres) = TRUE;
      basechar_add++;
    }
  else
    CfsmFsoptPrepr_instruction_symbol_neded(ppres) = FALSE;

  /* Presence of tags (on both sides). 
     Presence of EPSILON in a symbol pair.
     */
  sigmaa = sigma ((NETptr) net);
  labelsa = NET_labels((NETptr) net);

  CfsmFsoptPrepr_tags_on_upper_side(ppres) = FALSE;
  CfsmFsoptPrepr_tags_on_lower_side(ppres) = FALSE;
  CfsmFsoptPrepr_one_sided_epsilon_needed(ppres) = FALSE;

  if (!  ALPH_type(labelsa) ==  LABEL_VECTOR)
     binary_to_label(labelsa);
  for ( i= 0 ; i < ALPH_len(labelsa) ; i++)
    {
      id = ALPH_item(labelsa, i);
      if (  LABEL_arity(id_to_label(id)) > 1)
	{
	  up = upper_id (id);
	  lo = lower_id (id);
	  if (up == EPSILON || lo == EPSILON)
	    osen = TRUE;
	 if (cfsm_symbol_tag_p (convert_id_to_symbol(up)))
	   tus = TRUE;
	 if (cfsm_symbol_tag_p (convert_id_to_symbol(lo)))
	   tls = TRUE;
	}
    }
  if (osen)
    {
      CfsmFsoptPrepr_one_sided_epsilon_needed(ppres) = osen;
      basechar_add++;
    }
  if (tus)
    {
      CfsmFsoptPrepr_tags_on_upper_side(ppres) = tus;
      if (! tls)
	basechar_add++;
    }
  if (tls)
    {
      CfsmFsoptPrepr_tags_on_lower_side(ppres) = tls;
      basechar_add++;
    }

  /* EOS symbol needed or not.
  *** NOT YET IMPLEMENTED. 31.3.98 TG ***
  */
 
   {
    CfsmFsoptPrepr_EOS_symbol_needed(ppres) = TRUE;
    intern_symbol(&eossymbol, eos_label);
    if (! (ALPH_type(sigmaa) == BINARY_VECTOR))
      label_to_binary(sigmaa);
    if (! binary_sigma_member (sigmaa, eossymbol))
      basechar_add++;
   }

  /* Check whether sigma recoding is needed or not.
     Determine the real number of available pointer prefixes.
     Determine the number of available basechars.
     */
  if (!  ALPH_type(sigmaa) == LABEL_VECTOR)
     binary_to_label(sigmaa);
  
  CfsmFsoptPrepr_number_of_pointer_prefixes(ppres) = pointer_prefixes_desired;
  if ( ( (ALPH_len(sigmaa) + basechar_add) *POINTER_PREFIX_PER_BASECHAR  
	 + pointer_prefixes_desired) < DICTCODE_MAX )
    {
      CfsmFsoptPrepr_sigma_to_be_reduced(ppres) = FALSE;
      CfsmFsoptPrepr_number_of_basechars(ppres) = ALPH_len(sigmaa)+ basechar_add;
      CfsmFsoptPrepr_number_of_pointer_prefixes(ppres) = DICTCODE_MAX -
	POINTER_PREFIX_PER_BASECHAR*(CfsmFsoptPrepr_number_of_basechars(ppres));
    }
  else
    {
      CfsmFsoptPrepr_sigma_to_be_reduced(ppres) = TRUE;
      CfsmFsoptPrepr_number_of_basechars(ppres) = (DICTCODE_MAX -
	CfsmFsoptPrepr_number_of_pointer_prefixes(ppres)) /
	POINTER_PREFIX_PER_BASECHAR;
      CfsmFsoptPrepr_number_of_pointer_prefixes(ppres) = DICTCODE_MAX - 
	POINTER_PREFIX_PER_BASECHAR* CfsmFsoptPrepr_number_of_basechars(ppres);
    }
  return;
}

void TEST_fsopt_preprocess (cfsm_fsm net, int pointer_prefixes_desired)
{
  cfsm_preprocess_resulttype  ppres;
  fsm_fprintf(stdout, "FSOPT PREPROCESS. Asked for: %d pointer prefixes.\n",
	     pointer_prefixes_desired);
  fsopt_preprocess_fsm ( net, 
			pointer_prefixes_desired,
			&ppres);
  print_fsopt_preprocess_result(&ppres); 
}

static cfsm_symbol make_base_symbol(cfsm_fsm net,
				    char * lbl, 
				    ALPHABETptr * basealph,
				    int * basct)
{
  cfsm_symbol res = cfsm_make_symbol (lbl, net);
  if (*basct > 0)
    (*basct)--;
  else
    {
      fsm_fprintf (stderr, "\n\
*** CFSM FSOPT INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
Base char number exhausted.  base =  %d .\n", *basct);
      exit (1) ;
    }

  if ( ! (*basealph))
    *basealph = make_alph(0, LABEL_VECTOR);
  sigma_add_to(*basealph, convert_symbol_to_id(res));
  return res;
}

static cfsm_symbol make_immune_symbol(cfsm_fsm net,
				      char * lbl, 
				      ALPHABETptr * basealph,
				      ALPHABETptr * immalp, 
				      int * basct, 
				      int * imct)
{
  cfsm_symbol res = make_base_symbol(net, lbl, basealph, basct);
  (*imct)++;
 
  if ( ! (*immalp))
    *immalp = make_alph(0, LABEL_VECTOR);
  sigma_add_to(*immalp, convert_symbol_to_id(res));

  return res;
}

static int number_of_single_chars_in_vector(TALLYptr symbol_frequency_vector,
					    int sigma_tally_count)
     /* Returns the number of single characters.
     ***** May have to be refined ! 12.698 */
{
  int i, result = 0;

  for (i = 0 ; i < sigma_tally_count; i++)
    {
      result = i+1;
      if (cfsm_symbol_tag_p (convert_id_to_symbol((TALLY_label(symbol_frequency_vector+i)))))
	{
	  result--;
	  break;
	}
    }
  return result;
}

static int next_label_in_alph_label(ALPHABETptr thisalph, int *indx, cfsm_symbol *next)
{
   int ln;
   *next = 0;

   if (! thisalph)
     return FALSE;

   if (!  ALPH_type(thisalph) == LABEL_VECTOR)
     binary_to_label(thisalph);
   if (! (thisalph && (ln = ALPH_len(thisalph))))
     return FALSE;
 
   if ((*indx)+1 <= ln)
     {
       *next = convert_id_to_symbol(ALPH_item(thisalph, *indx));
       (*indx)++;
     }
   else
     {
       *indx=0;
       *next = convert_id_to_symbol(ALPH_item(thisalph, *indx));
       (*indx)++;
       return FALSE;
     }
  return TRUE;
}


static void create_new_stringmap(cfsm_symbol upper_ext_symbol, 
				 cfsm_symbol lower_basechar, 
				 int escape_cnt, 
				 int preceding_escapes,
				 cfsm_fsm * scs_net)
     /* 
	A transducer is created if it doesn't exist.
	A mapping between the two strings, in pseudo-Xerox notation:
	[ [ UPPER_EXT_SYMBOL ] .x. 
	        [ LOWER_BASECHAR [escape_label]^ESCAPE_CNT ] ]
	is done, that is, in the lower string, lower_basechar is followed by
	ESCAPE_CNT number of escape_label-s. 
	If  PRECEDING_ESCAPES is TRUE then escapes precede, rather than follow,
	LOWER_BASECHAR:  
	[ [ UPPER_EXT_SYMBOL ] .x. 
	        [ [escape_label]^ESCAPE_CNT LOWER_BASECHAR ] ]
	*/
{
  cfsm_label esc_l=NULL, last_l, nle=NULL;
  register int i;
  cfsm_state start_state=NULL, fin_dest_state=NULL, statefrom=NULL, dest_state=NULL; 

  if (! (upper_ext_symbol && lower_basechar))
    return;

  if (! *scs_net)
    *scs_net = (cfsm_fsm) null_fsm();
  start_state = (cfsm_state) NET_start_state((NETptr) *scs_net);
  fin_dest_state = cfsm_add_state(TRUE, *scs_net);
  sigma_add_to(NET_sigma((NETptr) *scs_net), convert_symbol_to_id(upper_ext_symbol));
  sigma_add_to(NET_sigma((NETptr) *scs_net), convert_symbol_to_id(lower_basechar));
 
  statefrom = start_state;
  if (!preceding_escapes)
    {
      nle = cfsm_make_fst_label(upper_ext_symbol, lower_basechar);
      if (escape_cnt)
	{
	  esc_l = cfsm_make_fst_label(cfsm_make_symbol ("", *scs_net),
				      cfsm_make_symbol (escape_label, *scs_net));
	}
      for ( i = 0; i < escape_cnt ; i++)
	{
	  dest_state =  cfsm_add_state (FALSE, *scs_net); 
	  cfsm_add_arc(nle, statefrom, dest_state, *scs_net);
	  statefrom = dest_state; 
	  nle = esc_l;
	}
      cfsm_add_arc(nle, statefrom, fin_dest_state, *scs_net);
    }
  else
    {
      if (escape_cnt)
	{
	  nle=esc_l=cfsm_make_fst_label(cfsm_make_symbol ("", *scs_net),
					cfsm_make_symbol (escape_label, *scs_net));
	  last_l=cfsm_make_fst_label(upper_ext_symbol,
				     lower_basechar);
	}
      else
	nle=last_l=cfsm_make_fst_label(upper_ext_symbol,
				       lower_basechar);

      for ( i = 0; i < escape_cnt ; i++)
	{
	  dest_state =  cfsm_add_state (FALSE, *scs_net); 
	  cfsm_add_arc(nle, statefrom, dest_state, *scs_net);
	  statefrom = dest_state; 
	  nle = esc_l;
	}
      cfsm_add_arc(last_l, statefrom, fin_dest_state, *scs_net);
      }
}
/****/

static id_type side_id(id_type id, int side)
{
  return ((side == UPPER) ? upper_id(id) : lower_id(id));
}

static int longest_string_from_state_by_id(STATEptr state, int side, id_type id, int level,
				     int *max_depth, int obey_flags_p)
{
  /* Returns the longest substring containing the most id-s on SIDE
     ignoring epsilons. */ 

  int longest_string = 0, this = 0;
  
  if (level > *max_depth)
    *max_depth = level;
    
  if ((int) STATE_client_cell(state) > 0)
    longest_string = (int) STATE_client_cell(state);
  else if (STATE_client_cell(state) == NULL)
    {
      int len;
      ARCptr a;
      
      STATE_client_cell(state) = (void *) -1; /* state is being processed */
      
      for (a = STATE_arc_set(state); a != NULL; a = next_arc(a))
	{
	  if (side_id (ARC_label(a), side) == id)
	    this=1;
	  else
	    this = 0;
	  len = longest_string_from_state_by_id(ARC_destination(a), side, id, level+1,
						max_depth, obey_flags_p)
	    + this;
	  
	  if (len > longest_string)
	    longest_string = len ;
	}
      STATE_client_cell(state) = (void *) longest_string;
    }

  return(longest_string);
}


static int most_id_minimal_string(NETptr net, int side, id_type id, int obey_flags_p)
     /*****
      *
      * Returns the number of id-s in the "minimal" most id-containing string in
      * NET, that is, the string from the start state to a final state containing
      * the greatest number of id-s (the mappins of id-s) that doesn't loop.
      * This function sums the numbers of "id" arc labels on the path.
      *
      * N.B. Epsilons are ignored.
      *
      *****/
{	
  int most_epsilons_containing_string, max_depth = 0; 
  
  clear_client_cells(net);

  most_epsilons_containing_string =
    longest_string_from_state_by_id(NET_start_state(net), side, id, 1,
				    &max_depth, obey_flags_p);
  clear_client_cells(net);

  return (most_epsilons_containing_string);
}

/***/

int fsopt_max_escape_count(fsopt_sigma_reduction s)
{
  /*** return ((longest_minimal_string ((NETptr) s, LOWER,
    TRUE)-1)/(sizeof(escape_label)-1)) ; ****/

  id_type eps;

  intern_symbol(&eps, escape_label);

  return (most_id_minimal_string((NETptr) s, LOWER, eps, TRUE));
}


#if NOT_USED
static STATEptr match_id_on_arc(ARCptr arc, id_type sid, id_type *uid, int escapes)
{
  STATEptr dest=NULL;
  ARCptr sarc;
  *uid=0;
  if (arc)
    if ( (sid == lower_id(ARC_label(sarc))))
      {
	dest = (ARC_destination(sarc));
	if ((! escapes && STATE_final(dest)) ||
	    (escapes  && ! STATE_final(dest)))
	  {
	    *uid = upper_id(ARC_label(sarc));
	  }
	else
	  dest=NULL;
      }
  return dest;
}
#endif /* NOT_USED */

static STATEptr match_id_on_state(STATEptr systat, id_type sid, id_type *uid)
{
  STATEptr dest=NULL;
  ARCptr sarc, narc;
  *uid=0;
  if (systat)
    for (sarc = STATE_arc_set(systat) ; sarc; sarc = narc)
      {
	narc = next_arc(sarc);
	if ( (sid == lower_id(ARC_label(sarc))))
	  {
	    dest = (ARC_destination(sarc));
	    *uid = upper_id(ARC_label(sarc));
	  }
      }
  return dest;
}


/**** Experimenting. Not yet working ****/
fsopt_string_16bit fsopt1_code_external_char_name(cfsm_label basechar, int escapes,
						 fsopt_sigma_reduction s)
 /* S is a special transducer mapping a single "external character" to 
       basechar
    or
       basechar *ESCAPE* 
    or
       basechar *ESCAPE* *ESCAPE* ...
    sequences. This property is used in this lookup.
    Should work for "preceding escapes", too:
       *ESCAPE* *ESCAPE* ... basechar
    */
{
  int i;
  id_type esc_l=0, loid=0, uid=0, result=0;
  STATEptr fin_st=NULL, next=NULL, sti;

  if (escapes)
    {
      intern_symbol(&esc_l, escape_label);
      if (! binary_sigma_member (sigma((NETptr) s), esc_l))
	return result;
      /* try if preceding escapes */
      for ( sti=NET_start_state((NETptr) s), i=0 ; i < escapes && sti ; sti=next, i++)
	{
	  next=match_id_on_state(sti, esc_l, &uid);
	  if (!next)
	    break;
	}
      if (next)
	{
	  fin_st=match_id_on_state(next, convert_label_to_id(basechar), &result);
	  if (fin_st && STATE_final(fin_st))
	    return result;
	}
      /* try if preceding basechar */
      sti=match_id_on_state(NET_start_state((NETptr) s), convert_label_to_id(basechar), &result);
      loid=convert_label_to_id(basechar);
      for ( i=0 ; i < escapes && sti ; sti=next, i++)
	{
	  next=match_id_on_state(sti, esc_l, &uid);
	}
      if (! (next &&  STATE_final(next)))
	result=0;
    }

  else
    sti=match_id_on_state(NET_start_state((NETptr) s), convert_label_to_id(basechar), &result);

  return result;
}

extern id_type OUTPUT_SEQ[WORD_STRING_SIZE + 1];
extern int OUTPUT_SIDE;

#define ALTCHAIN_NAME_STRING_LENGTH 11
#define FSOPT_ID_SEQUENCE_LIMIT 256
#define MAX_LINE_SIZE 4096
id_type FSOPT_OUTPUT_SEQ[FSOPT_ID_SEQUENCE_LIMIT + 1];


static void get_look_result_upper(id_type *tail)
/* look() uses a special callback format.
   Its result is copied into FSOPT_OUTPUT_SEQ. */
{
  id_type label, *head;
  int herept=0;

  for (head = OUTPUT_SEQ; head != tail; head++)
    {
      label = *head;
      if (upper_id(label))
	{
	  FSOPT_OUTPUT_SEQ[herept]=label;
	  herept++;
 	  if (herept > FSOPT_ID_SEQUENCE_LIMIT)
	    {
	      fsm_fprintf (stderr, "\n\
*** CFSM FSOPT INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
FSOPT_ID_SEQUENCE_LIMIT = %d .\n", herept);
	      exit (1);
	    }
	}
    }
  FSOPT_OUTPUT_SEQ[herept]=ID_EOS;
}

fsopt_string_16bit fsopt_code_external_char_name(cfsm_label basechar, int escapes,
						 fsopt_sigma_reduction s)
 /* S is a special, sequential transducer mapping a single "external
    character" to [basechar [*ESCAPE*]*] sequences.
    This property is used in this lookup.
    Works for "preceding escapes", too: [[*ESCAPE*]* basechar] .
    */
{
  ALPHABETptr SIGMA;
  int i;
  id_type esc_l=0, *idseq=NULL, result=0;
  PARSE_TBL ptb;

  if (escapes)
    intern_symbol(&esc_l,escape_label);

  idseq = (id_type *) malloc( (escapes+2)* sizeof (id_type));
  if (!idseq)
    {
      fsm_fprintf (stderr, "\n\
*** CFSM FSOPT INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
Storage allocation failed.  asked =  %d .\n", (escapes+2)* sizeof (id_type));
      exit (1) ;
    }

  SIGMA = label_to_binary(NET_sigma((NETptr) s));
  if (! NET_lower_parse_table((NETptr) s))
    ptb = make_parse_table((NETptr) s, NULL, LOWER);
  MATCH_TABLE = LOWER_MATCH;
  OUTPUT_SIDE = UPPER;
  FSOPT_OUTPUT_SEQ[0] = ID_EOS;
 
  {
    idseq[0]=convert_label_to_id(basechar);
    for (i=0; i <escapes; i++)
      idseq[i+1]=esc_l;
    idseq[escapes+1]= ID_EOS;

    look(idseq, NET_start_state((NETptr) s), OUTPUT_SEQ, get_look_result_upper,
	 FALSE);
    if (FSOPT_OUTPUT_SEQ[0] != ID_EOS)
      result = upper_id(FSOPT_OUTPUT_SEQ[0]);
  }
  /* try preceding escapes */
  if (!result)
    {
      for (i=0; i <escapes; i++)
	idseq[i]=esc_l;
      idseq[escapes]=convert_label_to_id(basechar);
      idseq[escapes+1]= ID_EOS;

      look(idseq, NET_start_state((NETptr) s), OUTPUT_SEQ,
	   get_look_result_upper, FALSE);
      if (FSOPT_OUTPUT_SEQ[0] != ID_EOS)
	result = upper_id(FSOPT_OUTPUT_SEQ[0]);
    }

  free(idseq);
  return result;
}

fsopt_string_16bit fsopt_code_external_tag_name(cfsm_label basechar, int escapes,
						fsopt_sigma_reduction s) 
 /* See fsopt_code_external_char_name() */
{
  return fsopt_code_external_char_name(basechar, escapes, s);
}

/******  Not yet working ***/
fsopt_string_16bit fsopt_code_external_diacritic_name(cfsm_label basechar, int escapes,
						      fsopt_sigma_reduction s)
{
  id_type result=0; /* Is RESULT an id ? */
                    /* changed the type from int16 to id_type */
  return result;
}

static ALPHABETptr set_available_alph(ALPHABETptr * basealph, int basechar_no,  
				      TALLYptr symbol_frequency_vector, int sigma_tally_count,
				      int single_ch)
{
 int bsln, i, size;
 id_type si;
 ALPHABETptr result = NULL;

 if (! basechar_no)
   return result;
 
 if (! (* basealph))
   {
     * basealph = make_alph (0, LABEL_VECTOR);
   }

 if (!  ALPH_type(* basealph) == LABEL_VECTOR)
   binary_to_label(* basealph);
 bsln = ALPH_len(* basealph);
 label_to_binary(* basealph);

 if (basechar_no <= (bsln + single_ch))
   /* then this is easy, just use what the net contains */
   {
     size = basechar_no-bsln; /* already reserved */
     if ( !(size > 0))
       {
	 fsm_fprintf (stderr, "\n\
*** CFSM INTERNAL PROGRAMMING ERROR ***   Please, inform the programmer.\n\
Impossible available alphabet size: %d.\n", size);
	 exit (1);
       }
     result = make_alph (0, LABEL_VECTOR);
   
     for (i = 0 ; i < size; i++)
       {
	 si=TALLY_label(symbol_frequency_vector+i);
	 if (!  ALPH_type(* basealph) == BINARY_VECTOR)
	   label_to_binary(* basealph);
	 if (! (binary_sigma_member (*basealph, si) ||
		(si == ALTCHAIN_SYMBOL)))
	   sigma_add_to(result, si);
       }
   }
 else
   {
     /*** set up somehow an alphabet from an initial default one ***/
   }
 return result;
}

static int remove_arc(NETptr net, STATEptr sti, ARCptr thisarc)
     /* Destructive operation to remove an arc from a state.
	Looping on the state is necessary. */
{
  int result = FALSE;
  ARCptr arci, nextarc, prevarc;
  if (! (net && sti && thisarc))
    return result;

  for (arci = STATE_arc_set(sti), prevarc = NULL; arci; arci =  nextarc)
    {
      nextarc = next_arc(arci);
      if (arci == thisarc)
	{
	  if (prevarc)
	    next_arc(prevarc) = nextarc;
	  else
	    STATE_arc_set(sti) = nextarc;
	  free_arc (thisarc);
	  (NET_num_arcs(net))--;
	  result = TRUE;
	  break;
	}
      prevarc = arci;
    }
  return result;
}

static cfsm_fsm recode_net_by_sigma_of_mapping_fst (cfsm_fsm net, 
						    cfsm_fsm singlech_map_fst,
						    cfsm_fsm tag_map_fst)
     /* The upper side of SINGLECH_MAP_FST contains the original single character
	symbols in the sigma of the original NET (which is an fst), the lower side
	the new (reduced) mapping. 
	The original net is recoded using this information.
	The same happens for tags using TAG_MAP_FST. 
	ALTCHAIN arcs are handled specially.*/
{
  ALPHABETptr upmapsc = NULL, upmaptg = NULL ;
  id_type idseq[2]= {0, ID_EOS};
  NETptr insert=NULL, insertlow=NULL, insertup=NULL;
  PARSE_TBL ptb;
  
  cfsm_arc ari, nextarc;
  cfsm_label lai;
  cfsm_state sti;
  cfsm_symbol lowi, upi; 

  if (singlech_map_fst)
    upmapsc = upper_sigma ((NETptr) singlech_map_fst);
  if (tag_map_fst)
    upmaptg = upper_sigma ((NETptr) tag_map_fst);
  if (singlech_map_fst && (!(ptb = NET_upper_parse_table((NETptr) singlech_map_fst))))
    {
      ptb = make_parse_table((NETptr) singlech_map_fst, NULL, BOTH_SIDES);
    }
  
  if (tag_map_fst && (!(ptb = NET_upper_parse_table((NETptr) tag_map_fst))))
    {
      ptb = make_parse_table((NETptr) tag_map_fst, NULL, BOTH_SIDES);
    }
  MATCH_TABLE = UPPER_MATCH;

  for (sti = cfsm_state_list(net) ; sti ; sti = cfsm_next_state(sti, net))
    {
      for (ari = cfsm_first_arc(sti, net) ; ari ; ari = nextarc)
	{
	  nextarc = cfsm_next_arc(ari, net);
	  lai = cfsm_arc_label(ari, net); 
	  upi = cfsm_label_upperside(lai, net) ;
	  lowi = cfsm_label_lowerside(lai, net) ;
	  idseq[0] = convert_symbol_to_id(upi);
	  if (lai != convert_id_to_label(ALTCHAIN_SYMBOL))
	    {
	      if (upi)
		insertup = map_seq_to_net (idseq, cfsm_symbol_tag_p(upi)?
					   ((NETptr) tag_map_fst):((NETptr) singlech_map_fst),
					   UPPER, LOWER, TRUE);
	      else
		insertup = epsilon_fsm();
	      idseq[0] = convert_symbol_to_id(lowi);
	      if (lowi)
		insertlow = map_seq_to_net (idseq, cfsm_symbol_tag_p(lowi)?
					    ((NETptr) tag_map_fst):((NETptr) singlech_map_fst),
					    UPPER, LOWER, TRUE);
	      else
		insertlow = epsilon_fsm();
	      insert = crossproduct_fst (insertup, insertlow, DO_RECLAIM, TRUE);
	 
	      STATE_deterministic((STATEptr) sti) = FALSE;
	      splice_fsm ((NETptr) net, (STATEptr) sti, 
		      (STATEptr) cfsm_arc_destination(ari, net), 
		      insert, FALSE, ID_NO_SYMBOL);
	      remove_arc((NETptr) net, (STATEptr) sti, (ARCptr)ari);
	    }

	  /*
#ifdef DEBUG
	  print_net_size((NETptr) net, stderr);
	  if (! (yes_or_no("Continue?", 'y')))
	    exit (1);
#endif
*/
	}
    }

  free_alph(upmapsc);
  free_alph(upmaptg);
  cfsm_normal_form_net(net);
  return net; 
}

void fsopt_show_full_mapping(cfsm_fsm mapper_net, int escapes, int preceding_escapes,
		       char * filler)
{
  ALPHABETptr SIGMA=0;
  int i, j, k, match=0, sln=0;
  id_type esc_l=0, lab;

 if (escapes)
   intern_symbol(&esc_l, escape_label);
 SIGMA=copy_alphabet(binary_to_label(NET_sigma((NETptr) mapper_net)));
 sln=ALPH_len(SIGMA);

 if (!(SIGMA && sln))
   return;
 if (!  ALPH_type(SIGMA) ==  LABEL_VECTOR)
   binary_to_label(SIGMA);
 free_parse_table(NET_lower_parse_table((NETptr) mapper_net));
 NET_lower_parse_table((NETptr) mapper_net)=NULL;
 fsm_fprintf(stdout,"* FSOPT mapping:\n");
 for (i = 0 ; i < sln; i++)
   {
     for (j = 0; j <= escapes ; j++)
       {
	 if ((lab=fsopt_code_external_char_name(convert_id_to_label(ALPH_item(SIGMA, i)),
						j, mapper_net)))
	   {
	     match++;
	     if (!preceding_escapes)
	       {
		 print_label(ALPH_item(SIGMA, i), stdout, DONT_ESCAPE);
		 fsm_fprintf(stdout,"%s",filler);
	       }
	     for (k = 0; k < j ; k++)
	       {
		 
		 print_label(esc_l, stdout, DONT_ESCAPE);
		 fsm_fprintf(stdout,"%s",filler);
	       }
	     if (preceding_escapes)
	       print_label(ALPH_item(SIGMA, i), stdout, DONT_ESCAPE);
	     fsm_fprintf(stdout,"%s",filler);
	     fsm_fprintf(stdout,"<->");
	     fsm_fprintf(stdout,"%s",filler);
	     print_label(lab, stdout, DONT_ESCAPE);
	     fsm_fprintf(stdout,"\n");
	   }
       }
   
   }  
 if(match)
   fsm_fprintf(stdout,"\tNumber of maps: %d\n", match);
 fsm_fprintf(stdout,"\n");
 free_alph(SIGMA);
}

#if NOT_USED
static void print_label_sequence(id_type *ls)
{
  int i=0;
  if (ls)
    {
     while (*ls != ID_EOS)
      {
	label_out(ls[i], DONT_ESCAPE) ;
	i++;
      }
    }
}
#endif /* NOT_USED */

static void create_trivial_mappings(cfsm_fsm *net, ALPHABETptr basealph,
				    cfsm_symbol escape)
     /* create a net with from basealph, except from *ESCAPE* that doesn't have
	its own right.
	*/
{
  int aln, i;
  cfsm_state fromst, tost;

  if (! basealph)
    return;
  
  *net=(cfsm_fsm) null_fsm();
  fromst = (cfsm_state) NET_start_state((NETptr) *net);
  tost = cfsm_add_state (TRUE, *net);
  binary_to_label(basealph);
  aln = ALPH_len(basealph);

  for (i = 0; i < aln; i++)
    {
      if (! (ALPH_type(basealph) ==  LABEL_VECTOR))
	binary_to_label(basealph);
      if (! (escape == convert_id_to_symbol(ALPH_item(basealph, i))))
	cfsm_add_arc(convert_id_to_label(ALPH_item(basealph, i)), fromst, tost, *net);
    }
  cfsm_normal_form_net(*net);
  if (! NET_num_arcs( (NETptr) *net))
    *net = NULL;
}

static void norm_add_trivials(cfsm_fsm *net, cfsm_fsm trivial)
{
  if (trivial)
    {
      if (*net)
	{ 
	  cfsm_normal_form_net (*net);
	  *net = (cfsm_fsm) net_union((NETptr) *net,
				      copy_fsm((NETptr) trivial));
	}
      else
	*net = (cfsm_fsm) net_union((NETptr) epsilon_fsm(),
				    copy_fsm((NETptr) trivial)); 
    }
}

cfsm_fsm reduce_sigma(cfsm_fsm net,
		      cfsm_preprocess_resultptr prepr_res,
		      int escape_symbol_needed,
		      int table_symbol_needed,
		      int is_fst,
		      int preceding_escapes,
		      int speed_true_space_false_opt,
		      cfsm_TALLYptr symbol_frequency_vector,
		      int sigma_tally_count,
		      cfsm_fsm *single_chars_net,
		      cfsm_fsm *tags_net)
     /* The sigma of the NET is "reduced", that is, its symbols are
	mapped to [basechar [ "*ESCAPE*" ]*] sequences and then the
	net is recoded using the recoding alphabet. 	
	The resulting net is returned and two auxiliary,
	deterministic, sequential networks:
	SINGLE_CHARS_NET and TAGS_NET
	that represent respective (symbol-to-symbol-sequence) mappings
	are also returned as help to find the mappings. 
	The frequency statistics that govern recoding are given in the
	next arguments:
	SYMBOL_FREQUENCY_VECTOR,  SIGMA_TALLY_COUNT
	where SYMBOL_FREQUENCY_VECTOR is a sorted vector of symbol
	id-s - first single character symbols and diacritics then
	multi-character symbols - and SIGMA_TALLY_COUNT is the size of
	this vector.
	Disclaimer: The OTHER symbol is not recoded correctly since it
	is not expected in a reduced net.
	*/
{
  cfsm_fsm result=NULL, trivials_net=NULL;
  cfsm_symbol esl=0, isl=0, next_a=0, nonidl=0, osel=0, tabl=0, this=NULL;
  int aind, escape_cnt=0, i, immune=0, rsbase=0, singlech=0, tags=0;
  int16 eos_in=0;
  id_type eossymbol=0;
  ALPHABETptr  availablealph=NULL, basealph=NULL, diffalph=NULL, immunealph=NULL;

  *single_chars_net = NULL;
  *tags_net = NULL;

  if (! ((BASECHAR_MIN <= CfsmFsoptPrepr_number_of_basechars(prepr_res)) 
	 && (CfsmFsoptPrepr_number_of_basechars(prepr_res) < BASECHAR_MAX)))
    {
      fsm_fprintf (stderr, "\n\
*** CFSM INTERNAL PROGRAMMING ERROR ***   Please, inform the programmer.\n\
Incorrect basechar number value: %d.\n", CfsmFsoptPrepr_number_of_basechars(prepr_res));
      exit (1);
    }
  
  singlech = number_of_single_chars_in_vector((TALLYptr) symbol_frequency_vector, sigma_tally_count);
  tags = sigma_tally_count - singlech;

  /* calculate the available number of base characters */
  rsbase = CfsmFsoptPrepr_number_of_basechars(prepr_res); 
  if (is_fst)
    nonidl = make_immune_symbol(result, nonid_label, &basealph, &immunealph, &rsbase, &immune);
  if (table_symbol_needed)
    tabl = make_immune_symbol(result, table_label, &basealph, &immunealph, &rsbase, &immune);
  if (CfsmFsoptPrepr_one_sided_epsilon_needed( prepr_res))
    osel = make_immune_symbol(result, one_sided_epsilon, &basealph, &immunealph, &rsbase, &immune);
  if (CfsmFsoptPrepr_instruction_symbol_neded( prepr_res))
    isl = make_immune_symbol(result, instruction_label, &basealph, &immunealph,
			     &rsbase, &immune);

  if ( net && (!  ALPH_type(sigma((NETptr) net)) == BINARY_VECTOR))
    label_to_binary(sigma((NETptr) net));
  if (binary_sigma_member (sigma((NETptr) net), OTHER) )
    {
      fsm_fprintf (stderr, "\n\
*** CFSM INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
Net contains OTHER.\n");
      return result;
    }

  if (! (ALPH_type(sigma((NETptr) net)) == BINARY_VECTOR))
    label_to_binary(NET_sigma((NETptr) net));
  intern_symbol(&eossymbol, eos_label);
  if  (binary_sigma_member ((NET_sigma((NETptr) net)), eossymbol))
    eos_in=1;
  if (CfsmFsoptPrepr_EOS_symbol_needed( prepr_res))		      
    make_base_symbol(result, eos_label, &basealph,  &rsbase);
  if (tags)
    make_base_symbol(result, tagsep_label, &basealph,  &rsbase);

  if (basealph && (! (ALPH_type(basealph) == LABEL_VECTOR)))
    binary_to_label(basealph);
  if (immunealph && (! (ALPH_type(immunealph) == LABEL_VECTOR)))
    binary_to_label(immunealph);
 
  if (escape_symbol_needed ||
      ((rsbase+eos_in) <= singlech) ||
      ((rsbase+eos_in) <= tags))
    esl = make_immune_symbol(result, escape_label, &basealph, &immunealph,
			     &rsbase, &immune);
 
#ifdef DEBUG
  if (basealph)
    {
      fsm_fprintf(stderr, "\n** BASEALPH:\n");
      print_alphabet_s(basealph,  60, 0, TRUE, stderr);
    }
  if (immunealph)
    {
      fsm_fprintf(stderr, "\n** IMMUNEALPH:\n");
      print_alphabet_s(immunealph,  60, 0, TRUE, stderr);
    }
#endif
  /* if no escape is needed then recode tags to symbols.
   */

  /* Initial condition of solvability */
  if ( ! ( CfsmFsoptPrepr_number_of_basechars(prepr_res) > 0 ||
	  (basealph && (( ALPH_len(binary_to_label(basealph)) - ALPH_len(binary_to_label(immunealph))) > 0)))
       )
    {
      free_alph(basealph);
      free_alph(immunealph);
#ifdef DEBUG
      fsm_fprintf(stderr, "*** fsopt: not enough basechar %d\n", CfsmFsoptPrepr_number_of_basechars(prepr_res));
#endif
      return result;
    }
   
  if (basealph && immunealph)
    {
      diffalph = copy_alphabet(basealph);
      diffalph = binary_to_label(subtract_alph(diffalph, immunealph));
    }

   availablealph = set_available_alph (& basealph, CfsmFsoptPrepr_number_of_basechars(prepr_res), 
				       (TALLYptr) symbol_frequency_vector, sigma_tally_count, singlech);
#ifdef DEBUG
  if (availablealph && yes_or_no("Ask for availablealph?", 'y'))
    {
      fsm_fprintf(stderr, "\n** AVAILABLE for reduction ALPH:\n");
      print_alphabet_s(availablealph,  60, 0, TRUE, stderr);
    }
#endif 

  /* single char symbols */
  for ( i = 0, escape_cnt = 0, aind = 0 ; i < singlech ; i++)
    {
      this = convert_id_to_symbol(TALLY_label(((TALLYptr) symbol_frequency_vector) + i));
      if (! (ALPH_type(basealph) == BINARY_VECTOR))
	label_to_binary(basealph);
      if ( (! binary_sigma_member (basealph, convert_symbol_to_id(this))) &&
	   (this != convert_id_to_symbol(ALTCHAIN_SYMBOL)))
	{
	  next_a = 0;
	  if (! next_label_in_alph_label (availablealph, &aind, &next_a))
	    escape_cnt++;
	  create_new_stringmap(this, next_a, escape_cnt, preceding_escapes,
			       single_chars_net);
	}
    }

  /* ADD trivial mappings like *TABLE* <-> *TABLE* to mapper fst !*/
  create_trivial_mappings(&trivials_net, basealph, esl);

  norm_add_trivials(single_chars_net, trivials_net);

#ifdef DEBUG
  if (*single_chars_net && NET_num_arcs( (NETptr) *single_chars_net) &&
      yes_or_no("Ask for single chars mapping net?", 'n'))
  {
    fsm_fprintf(stderr, "\n** single chars to sequences fst (*single_chars_net) **\nSigma:\n");
    print_sigma_s((NETptr) *single_chars_net, 60, 0, stderr);  
    print_net ((NETptr) *single_chars_net, stderr);
  }
#endif

  /* tags */
  for ( i = 0, escape_cnt = 0, aind = 0 ; i < tags ; i++)
    {  
      this = convert_id_to_symbol(TALLY_label(((TALLYptr) symbol_frequency_vector)+(singlech+i)));
      if (! (ALPH_type(basealph) == BINARY_VECTOR))
	label_to_binary(basealph);
      if (! binary_sigma_member (basealph, convert_symbol_to_id(this)))
	{
	  next_a = 0;
	  if (! next_label_in_alph_label (availablealph, &aind, &next_a))
	    escape_cnt++;
	  create_new_stringmap(this, next_a, escape_cnt, preceding_escapes,
			       tags_net);
	}
    }
  
  norm_add_trivials(tags_net, trivials_net);

#ifdef DEBUG
  if (*tags_net && NET_num_arcs( (NETptr) *tags_net) &&
      yes_or_no("Ask for tags mapping net?", 'n'))
    {
      fsm_fprintf(stderr, "\n** *tags_net **\nSigma:\n");
      print_sigma_s((NETptr) *tags_net, 60, 0, stderr);  
      print_net ((NETptr) *tags_net, stderr);

    }
#endif

  /* transform the input NET into an equivalent, recoded one using SCS_NET encoding
     for single characters and TAGS_NET encoding for tags. */

  if (! *single_chars_net )
    *single_chars_net = (cfsm_fsm) epsilon_fsm();
  if (!  *tags_net)
     *tags_net = (cfsm_fsm) epsilon_fsm();
  recode_net_by_sigma_of_mapping_fst(net, *single_chars_net, *tags_net);

  /* cleanup */

 free_alph(availablealph);
 free_alph(basealph);
 free_alph(immunealph); /* may not exist - and must not exist any more */
 free_alph(diffalph);
 free_network((NETptr) trivials_net);

 return result;
}

#ifdef DEBUG
static void interactive_mapping(cfsm_fsm reduced_mapping_net)
{
  char *basecharstr, def='0', *escapesch=NULL;
  int escapes=0;
  id_type basechar, map=0;

  basecharstr = (char *) malloc( (size_t) MAX_LINE_SIZE*sizeof(char));
  basecharstr[0]=0;
  fsm_fprintf(stdout,"Basechar?\n");
  if 
    (scanf( "%s", basecharstr) == EOF)
    basecharstr[0]=0;
  intern_symbol(&basechar, basecharstr);
  escapesch= get_input("Number of escapes?", FALSE, "\n");
  
  if (escapesch==NULL)
    escapesch = &def;
  switch (escapesch[0])
    {
    case ('0'):
      escapes=0;
      break; 
    default:
      if 
	(sscanf(escapesch, "%u", &escapes) == EOF)
	escapes=0;
      break; 
    }
  fsm_fprintf(stdout,"\tAsked for %d escapes\n", escapes);    
  map=fsopt_code_external_char_name(convert_id_to_label(basechar), escapes, reduced_mapping_net);
  if (map)
    {
      print_label(map, stdout, DONT_ESCAPE); 
      fsm_fprintf(stdout,"\n");
    }
  free(basecharstr);
}
#endif /* DEBUG */

cfsm_fsm TEST_fsopt_reduce_sigma (cfsm_fsm net, 
				  int pointer_prefixes_desired,
				  int preceding_escapes,
				  cfsm_fsm *single_chars_net,
				  cfsm_fsm *tags_net)
{
  cfsm_fsm result=NULL;
  cfsm_preprocess_resulttype ppres;
  int sigma_tally_count =0;
  id_type max_sigma_id = 0;
  TALLYptr tally_sig=NULL;

  if (!net)
    return result;
 
  if (*single_chars_net || *tags_net)
    {
      fsm_fprintf (stderr, "\n\
*** CFSM INTERNAL PROGRAMMING ERROR ***   Please, inform the programmer.\n\
Preexisting single character or tag mapper fst(s).\n");
      exit (1); 
    }

  fsm_fprintf(stdout, "FSOPT PREPROCESS. Asked for: %d pointer prefixes.\n",
	      pointer_prefixes_desired);
  fsopt_preprocess_fsm (net, 
			pointer_prefixes_desired,
			&ppres);

  /* CfsmFsoptPrepr_EOS_symbol_needed(&ppres) = FALSE; */
  print_fsopt_preprocess_result(&ppres); 

  fsm_fprintf(stdout, "\nFSOPT Reduce Sigma.\n");

  tally_sig = make_tally_sigma((NETptr) net, &sigma_tally_count, &max_sigma_id);
  sort_this_tally(tally_sig, max_sigma_id, compare_label_freq_symbols_precede_tags); 

  result=reduce_sigma (net, 
		       &ppres,
		       /* TRUE, */ FALSE, 
		       FALSE,
		       (NET_arc_label_arity( (NETptr) net)>1)?TRUE:FALSE,
		       preceding_escapes,
		       FALSE,
		       (cfsm_TALLYptr) tally_sig,
		       sigma_tally_count,
		       single_chars_net,
		       tags_net);
#ifdef DEBUG
  do
    {
      if (* single_chars_net)
	while (yes_or_no("Ask for mapping of single characters?", 'n'))
	  interactive_mapping(* single_chars_net);
      if (* tags_net)
	while (yes_or_no("Ask for mapping of tags?", 'n'))
	  interactive_mapping(* tags_net);
    }
  while ((* single_chars_net || * tags_net) && 
	 yes_or_no("Restart interactive mapping inquiry of single characters or tags?"
		   , 'n'));
#endif
  free(tally_sig);
  return result;
}


cfsm_fsm compression_push_epsilons(cfsm_fsm net, 
				   int input_side, 
				   int from_level, 
				   int upto_level,
				   int marking_algorithm,
				   void* fst_cntxt)
     /* Push epsilons, using local sequentialization. */
{
  return (cfsm_fsm) local_from_to_sequentialize_net((NETptr) net, input_side, 
						    from_level, upto_level, 
						    marking_algorithm, 
						    (FST_CNTXTptr)fst_cntxt);
}

cfsm_fsm fsopt_sequentialize_to_level(cfsm_fsm fsm,
				      int lowerside, /* "seq. w.r.t. the lower side" */
				      int from_level,
				      int over_level,
				      int marking_algorithm,
				      void* context)
{
  cfsm_fsm result = (cfsm_fsm) local_from_to_sequentialize_net
      ((NETptr)fsm,
       (lowerside ? LOWER : UPPER),
       from_level,
       over_level,
       marking_algorithm,
       (FST_CNTXTptr) context);
  return (result == NULL) ? fsm : result;
}
