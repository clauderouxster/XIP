/* $Id: test-compression.h,v 1.4 2005/03/04 15:45:30 grondeau Exp $ */
/* Copyright (c) 1998 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  test-compression.c
 **
 ** Tamas Gaal, XRCE, March 1998
 **
 ** Kaplan-compression tests and ancillary functions 
 ** for the c-fsm library.
 **
 ******************************************************/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CFSM_VAR {
  char * name;
  cfsm_fsm fst;
  struct CFSM_VAR * link;
} CFSM_VARtype, * CFSM_VARptr;

#define CFSM_VAR_name(X)          (X)->name
#define CFSM_VAR_fst(X)           (X)->fst
#define CFSM_VAR_link(X)          (X)->link

typedef  cfsm_fsm fsopt_sigma_reduction;

/* Since this header file serves as an interface file to fsoptimize, we cannot
   use datatypes internal to the C-FSM library--such as id_type. */
typedef unsigned short fsopt_string_16bit;

/* The size of cfsm_preprocess_resultType can be reduced: a lot of fields
   are binary logical values only */
typedef struct CFSM_PREPROCESS_RESULT {
  int tags_on_upper_side;
  int tags_on_lower_side;
  int sigma_to_be_reduced;
  int one_sided_epsilon_needed;
  int nonid_recoding_needed;
  int instruction_symbol_neded;
  int EOS_symbol_needed;
  int number_of_pointer_prefixes;
  int number_of_basechars;
} cfsm_preprocess_resulttype, *cfsm_preprocess_resultptr;

#define CfsmFsoptPrepr_tags_on_upper_side(X)   (X)->tags_on_upper_side
#define CfsmFsoptPrepr_tags_on_lower_side(X)   (X)->tags_on_lower_side
#define CfsmFsoptPrepr_sigma_to_be_reduced(X)  (X)->sigma_to_be_reduced
#define CfsmFsoptPrepr_one_sided_epsilon_needed(X) (X)->one_sided_epsilon_needed
#define CfsmFsoptPrepr_nonid_recoding_needed(X)  (X)->nonid_recoding_needed
#define CfsmFsoptPrepr_instruction_symbol_neded(X) (X)->instruction_symbol_neded
#define CfsmFsoptPrepr_EOS_symbol_needed(X)    (X)->EOS_symbol_needed
#define CfsmFsoptPrepr_number_of_pointer_prefixes(X) (X)->number_of_pointer_prefixes
#define CfsmFsoptPrepr_number_of_basechars(X)  (X)->number_of_basechars



cfsm_fsm FstCalcExport cfsm_regex (char * regex_string, CFSM_VARptr varlist);
  /* Creates a network from a string that contains a A Xerox regular
     expression.  "regex_string" is a string that may contain internal
     regex variables in the following syntax:
     $(variablename) .
     The "varlist" parameter must contain (variablename, net) pairs.
     */

cfsm_fsm FstCalcExport insert_token_break(cfsm_fsm net);
  /* Mark end of lower-side strings by special symbols called
     "*EOS*". The *EOS*  must be to the left of any lower-side EPSILON-s on 
     the right-hand end of the path.*EOS* means end-of-symbol (former
     TB or token break).
     Ignore ALTCHAIN-s and flag diacritics (EOS must precede them if there is no
     further other symbol (then EPSILON or ALTCHAIN or flag diacritics).
     The original net is retained (not freed).
     */

cfsm_fsm FstCalcExport insert_token_break_do_reclaim(cfsm_fsm net); 
  /* See insert_token_break() above.

     The original net is freed.
       */

cfsm_fsm FstCalcExport insert_tag_separator(cfsm_fsm net);
  /* Prepares a transducer for compression by inserting a transition
     labelled *TAGSEP* between ordinary symbols and tags.      
     The returned result will be the so transformed new net.
     */

cfsm_fsm FstCalcExport nonid_transformation_for_compression(cfsm_fsm net);
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
      The returned result will be the so transformed new net.
     */

cfsm_fsm FstCalcExport remove_leading_plus_from_tags(cfsm_fsm net);
 /* Following a convention from Xerox PARC, "+Tag" names -
    multi-character symbols with a leading "+" sign - are rewritten as
    "Tag" provided the new tag, "Tag", remains a tag - that is, a
    multi-character symbol, which is neither a flag diacritic
    (aka instruction) nor an epsilon diacritic.  
    The returned result will be the so transformed new net.
    */

void FstCalcExport fsopt_preprocess_fsm(cfsm_fsm net, 
			  int pointer_prefixes_desired,
			  cfsm_preprocess_resultptr ppres);
 /* Gathers information about the transducer for Kaplan compression. 

    Input arguments: 
      net - a transducer (remains unchanged)
      pointer_prefixes_desired - a number between 4 and 252. The
    computed "pointer_prefixes" number may differ and is put into the
    result:

    Output argument: 
      ppres - i it is a C structure telling the properties of the net in
    the members of the structure.
    All values (see cfsm_preprocess_resultptr above) are effectively
    computed but for 
    EOS_symbol_needed: it is NOT IMPLEMENTED yet (9 July 1998) so
    the user must set it.
    The number_of_pointer_prefixes and  number_of_basechars fields
    sum up the "dictcode space" (256 bytes).
    */

void FstCalcExport TEST_fsopt_preprocess(cfsm_fsm net, int pointer_prefixes_desired);
  /* Calls fsopt_preprocess_fsm() and prints the result to stdout */

void FstCalcExport print_fsopt_preprocess_result (cfsm_preprocess_resultptr ppr);
  /* Prints the argument to stdout */

typedef struct cfsm_TALLY {
  int cfsm_label;
  long freq;
} cfsm_TALLYcell, *cfsm_TALLYptr;

#define cfsm_TALLY_label(X) (X)->cfsm_label
#define cfsm_TALLY_freq(X)  (X)->freq

cfsm_fsm FstCalcExport reduce_sigma(cfsm_fsm net,
		      cfsm_preprocess_resultptr prepr_res,
		      int escape_symbol_needed,
		      int table_symbol_needed,
		      int is_fst,
		      int preceding_escapes,
		      int speed_true_space_false_opt,
		      cfsm_TALLYptr symbol_frequency_vector,
		      int sigma_tally_count,
		      cfsm_fsm *single_chars_net,
		      cfsm_fsm *tags_net);
  /* 
     The "net" argument - a transducer - is transformed  and copied to the
     returned result transducer. Two more results are returned in the
     out-parameter arguments single_chars_net and tags_net.
 
     Input arguments:
        prepr_res - result of fsopt_preprocess_fsm()
	escape_symbol_needed - *ESCAPE* will be used (both optional and	computed)
	table_symbol_needed - whether final index file needs *TABLE*-s
	is_fst - transducer or simpe automaton
	preceding_escapes - ScanSoft convention, *ESCAPE*-s precede basechars.
	                    default is suffixing *ESCAPE*-s
        speed_true_space_false_opt - optimize for speed (NOT IMPLEMENTED YET)
	symbol_frequency_vector - vector of (cfsm_label, frequency_of_this_label)
	    pairs showing relative frequency of labels. Actual implementation
	    expects tags sorted after other (single character and diacritic)
	    symbols. 
        sigma_tally_count - size (number of elements) of the previous vector.
	
      Output arguments:
      
	single_chars_net - a mapping transducer between an Upper language that
	represents the original single character symbols and diacritics of the
	(sigma alphabet of the) input net and a Lower language that represents
	the recoded (reduced) alphabet.  Since this is a special transducer,
	ordinary CFSM tools (lookup) can be used to query the mapping.

	tags_net -  a mapping transducer between an Upper language that
	represents the original tag set  of the input net and a Lower language
	that represents the recoded (reduced) alphabet.  Since this is a special
	transducer, ordinary CFSM tools (lookup) can be used to query the
	mapping.

      Disclaimer: The input net must not contain the OTHER symbol.  */

cfsm_fsm FstCalcExport TEST_fsopt_reduce_sigma (cfsm_fsm net, 
				  int pointer_prefixes_desired,
				  int preceding_escapes,
				  cfsm_fsm *single_chars_net,
				  cfsm_fsm *tags_net);
  /* An application of fsopt_reduce_sigma():
     net is transformed into an equvalent net (returned as result) that has the
     same or smaller label set. The extent of the reduction is governed by the
     "pointer_prefixes_desired" argument. The recoding (in pseudo-Xerox
     regular expression notation) is :
     	[ [ UPPER_EXT_SYMBOL ] .x. 
	        [ LOWER_BASECHAR [escape_label]^ESCAPE_CNT ] ]

     or, if "preceding_escapes" is on, 
		[ [ UPPER_EXT_SYMBOL ] .x. 
	        [ [escape_label]^ESCAPE_CNT LOWER_BASECHAR ] ]

     Output arguments are: single_chars_net, tags_net.
     These are special, deterministic, non-circular transducers that represent
     the above sigma-reduction mapping for not-tags and tags, respectively.
     See also at fsopt_reduce_sigma() above.
   */

void FstCalcExport fsopt_show_full_mapping(cfsm_fsm mapper_net, int escapes, int preceding_escapes,
			     char * filler);
  /* Prints the mapping between the words of the uppe rnad the lower language.
     See fsopt_reduce_sigma() and TEST_fsopt_reduce_sigma() above.
     
     Input arguments:
          mapper_net - must be a sequential, non-circular transducer.
	  escapes - number of *ESCAPE*-s in the reduced character.
	  preceding_escapes - when *ESCAPE*(-s) precede basechars.
	  filler - character between symbols (default is "", that is, nothing).     
 */

int FstCalcExport fsopt_max_escape_count(fsopt_sigma_reduction s);
  /* Returns the maximal number of *ESCAPE* symbols in the words of mapper_net.
     This must be a sequential, non-circular transducer.
     See also fsopt_reduce_sigma() and TEST_fsopt_reduce_sigma() above.*/

fsopt_string_16bit FstCalcExport fsopt_code_external_char_name(cfsm_label basechar, int escapes,
						 fsopt_sigma_reduction s);
  /* Shows the mapping between a basechar followed (or preceded, depending on the
     convention used) by "escapes" number of *ESCAPE* sequences and a non-tag
     external symbol.
     The mapping is described by a transducer, s. This must be sequential and
     non-circular, the lower language is the input side, the upper one is the
     result side.
     */

fsopt_string_16bit FstCalcExport fsopt_code_external_tag_name(cfsm_label basechar, int escapes,
						fsopt_sigma_reduction s);
  /* Shows the mapping between a basechar followed (or preceded, depending on the
     convention used) by "escapes" number of *ESCAPE* sequences and a tag
     external symbol.
     The mapping is described by a transducer, s. This must be sequential and
     non-circular, the lower language is the input side, the upper one is the
     result side.
     */

cfsm_fsm FstCalcExport compression_push_epsilons(cfsm_fsm net, 
				   int input_side, 
				   int from_level, 
				   int upto_level,
				   int marking_algorithm,
				   void* fst_cntxt);
  /* 
     Push epsilons on the input_side of the net, from from_level to
     upto_level. The latter is the boundary for "local sequentialization".
     If upto_level is 0 then there is no upper boundary.
     Local sequentialization is done state-wise: arcs of the state are selected
     first, then the closures of the selected arcs until upto_level, then the
     so marked subnet is extracted, sequentialized (if possible) and reinserted.

     Values of marking_algorithm:
         0: mark arcs labeled by epsilon-s on the input side;
	 1: mark all arcs of a state;
	 2: mark arcs of the state according to the the presence of
	    epsilons and/or non-deterministic ambiguities on both sides;
	 3: mark epsilon arcs and ambiguous arcs on the input side.

     fst_cntxt is the object needed by init_fsm_memory();
*/

/* Marking algorithms: */
#define CFSM_SEQ_EPSILON_ARCS 0
#define CFSM_SEQ_ALL_ARCS 1
#define CFSM_SEQ_MIXED 2

cfsm_fsm FstCalcExport fsopt_sequentialize_to_level(cfsm_fsm fsm,
				      int lowerside, /* "seq. w.r.t. the lower side" */
				      int from_level,
				      int over_level,
				      int marking_algorithm,
				      void* context);

#ifdef __cplusplus
}
#endif
