/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: comm-def.c,v 1.1 2006/06/09 12:53:25 grondeau Exp $ */

#include <stdlib.h>
#include <stdio.h>

#include "fst/fst_windefs.h"
#include "fst/fst-types.h"
#include "fst/context.h"
#include "fst/interface.h"
#include "fst/comm-def.h"

#ifndef INFO
#define INFO(x,y,z) z
#endif

#ifndef WORLD
extern int SQ_FINAL_STRINGS_ARCS;
extern int SQ_INTERN_STRINGS_ARCS;
extern int SQ_STRING_ONELONG ;
#endif /* WORLD */

int NOT_BINARY = -1; /* To fill the value slot of non-binary variables */

struct command *last_command, *last_variable;

/****************************************************************
  Local interface administration
 ****************************************************************/
struct STRIntPar interface_parameters =
  {{0, 10},	 /* regex */
   {0},		 /* idiom: space */
   {0, 1, 0, 1}, /* alphabet: print_pairs, print_left, read_left, unicode */
   {1, 1, 0,     /* general: sort_arcs, verbose, completion *AK*,         */
    0, 0, 1, 1,  /*          stack, namenets, minimal, quit_on_fail,      */
    0, 0, 0,     /*          assert, show_escape, sq_final_arcs,          */
    0, 0,        /*          sq_intern_arcs, recursive_define,            */
    0, 0,        /*          recursive_apply, flag_is_epsilon             */
    1},          /*          need_separators                              */
   {0},		 /* optimisation: in_order */
   {20, 0, 10,	 /* parsing: maxresult, maxerrors, maxheurs */
      1000000},  /* maximum workspace */
   {1, 0, 1,     /* io: print_sigma_also, print_space, obey_flags, */
    0, 1, 0,     /*     mark_version, retokenize, show_flags, */
    1, 0,        /*     max_state_visits, count_patterns */
    0, 0, 1,     /*     delete_patterns, extract_patterns, mark_patterns */
    -1,          /*     license_type -- sync with user_license_type      */
    DEFAULT_CHAR_ENCODING,  /*  1 = ISO-8859-1, 2 = UTF-8 */
    1},          /*     for compacted nets use memory mapping by default */
   {1},		 /* parameters: interactive */
   {1}		 /* lexicon: compress words */
 };

/****************************************************************
 * Add here command names and functions to be called.
 * FIELDS:
 *	the name of the command
 *	typing instructions
 *	start state in lex for reading possible parameters (be careful with)
 *	command class (stack-operation, fs-operation, ...)
 *	usage information
 *	brief message
 *	long help
*/

/* enum { COMM1, COM_A, COM_FS, COM_LX, COM_P, COM_S, COM_T, COM_X }; */

#define VARI	100
#define	REGE	200

char * help_header = {"\n\
General:\n\
        apropos <text>\n\
will help in finding the command related to text. Then use\n\
        help <full command name>\n\
Some commands have built-in aliases. They are equivalent, that is,\n\
        help <built-in-alias name>\n\
will give the same result.  Some full and built-in alias command names\n\
can be abbreviated. When in doubt, ask `help' to list and explain\n\
valid abbreviations!\n\
        help variable\n\
will print out a list of variables that control the behavior of\n\
the interface and some finite-state operations.\n\
        show <variable name>\n\
shows the current value of the variable. The 'set' command can be\n\
used to set a variable to a new value."};

char *  CommandClasses [VIRT_NET+1] = { 
    "System commands",

    "Input/output and stack commands",

    "Display commands",

    "Tests of network properties",

    "Operations on networks",

    "Operations on virtual networks"};

struct command commands[] = {

#ifndef WORLD
    { "add path", INFO("add$ p$ath|ap", "IN_LINE_OR_FILE", COM_NET),
      " |line|< source-file",

      "add a path to the top network",

      "add a path to the top network", 0},

    { "add word", INFO("add$ w$ord|ad", "ATOMI", COM_NET),
      " |line|< source-file",

      "add a word (according to a model) to the top network",

      "add a word (according to a model) to the top network", 0},
#endif

    
  { "add properties", INFO("ad$d prop$erties|add", "ANY_PARAMETER", COM_NET),
      " |< source-file",

      "adds attribute-value pairs to the network's property list",
#ifndef _WIN32
      "reads attribute-value pairs from a text file or from the console\n\
and adds them to the property list of the top network on the stack.\n\
Each line in the source file must contain one attribute-value\n\
pair. For example,\n\
COPYRIGHT: \"Xerox Corporation\"\n\
DATE:   \"June 10, 1997\"\n\
VERSION: 9\n\
The attributes, each terminated by a colon, must be strings (upper\n\
case recommended). The values may be strings or integers. String\n\
values must be enclosed in double quotes: \"9\" and 9 are considered as\n\
different values. If an attribute to be added already exists on the\n\
property list of the network, 'add properties' will reset it to a new\n\
value. When reading from the console, use Control-D to terminate\n\
the input. See also: 'read properties', 'write-properties'.",
#else
      "reads attribute-value pairs from a text file or from the console\n\
and adds them to the property list of the top network on the stack.\n\
Each line in the source file must contain one attribute-value\n\
pair. For example,\n\
COPYRIGHT: \"Xerox Corporation\"\n\
DATE:   \"June 10, 1997\"\n\
VERSION: 9\n\
The attributes, each terminated by a colon, must be strings (upper\n\
case recommended). The values may be strings or integers. String\n\
values must be enclosed in double quotes: \"9\" and 9 are considered as\n\
different values. If an attribute to be added already exists on the\n\
property list of the network, 'add properties' will reset it to a new\n\
value. When reading from the console, use Control-Z to terminate\n\
the input. See also: 'read properties', 'write-properties'.",
#endif
 0},

  { "alias", INFO("ali$as", "ATOMI", COMM1),
      " |name|name command sequence|",

      "define a word to be a shorthand notation",


      "'alias <name> <command-sequence>' defines <name> as a shorthand\n\
for the given sequence of XFST commands. For instance, the new command\n\
'ls' can be created in the following way:\n\
\n\
alias ls print directory\n\
\n\
Alias may contain several commands.  A new command 'lexr', for\n\
reading a lexicon, can be created in the following way:\n\
\n\
fst[0]: alias lexr\n\
alias> load lexicon.fst\n\
alias> print properties\n\
alias> END;\n\
\n\
A multi-command alias may also be defined on one line as follows:\n\
\n\
alias lexr load lexicon.fst; print properties\n\
\n\
See also: 'print aliases'.", 0},

#ifndef WORLD
  { "ambiguity net", INFO("ambig$uity n$et|amb$net", "ANY_PARAMETER", COM_NET),
      "",

      "converts the top network to an ambiguity class transducer",

      "converts the lexical transducer on the top of the stack into a\n\
transducer that maps each word on the lower-side of the network to\n\
itself and a sequence of alternative tags that the word has in the\n\
original transducer. Limitations: (1) the last symbol on each path is\n\
interpreted as a tag, (2) only one tag per word is allowed, \n\
(3) the network must be acyclic.", 0},
#endif /* WORLD */

  { "apply up", INFO("app$ly u$p|up", "IN_LINE_OR_FILE", COM_D),
      " |line|< source-file",

      "applies the input line to the lower side of the top network",

#ifndef _WIN32
      "Simulates the composition of the input string with the lower\n\
side of the top network on the stack and extracts the result from the\n\
upper side. If the command `apply up' is followed by a carriage\n\
return,the program prompts the user with apply>, prints the result and\n\
gives a prompt for a new input. Control-D exits from this mode.\n\
You can also input END; to exit. See also 'print-pairs', 'print-space'\n\
'show-flags' for output options.",
#else
      "Simulates the composition of the input string with the lower\n\
side of the top network on the stack and extracts the result from the\n\
upper side. If the command `apply up' is followed by a carriage\n\
return,the program prompts the user with apply>, prints the result and\n\
gives a prompt for a new input. Control-Z exits from this mode.\n\
You can also input END; to exit. See also 'print-pairs', 'print-space'\n\
'show flags' for output options .",
#endif

 0},

  { "apply down", INFO("app$ly do$wn|down", "IN_LINE_OR_FILE", COM_D),
      " |line|< source-file",

      "applies the input line to the upper side of the top network",

#ifndef _WIN32
      "Simulates the composition of the input string with the upper\n\
side of the top network on the stack and extracts the result from the\n\
lower side. If the command `apply down' is followed by a carriage\n\
return, the program prompts the user with apply>, prints the result\n\
and gives a prompt for a new input. Control-D exits from this mode.\n\
You can also input END; to exit. See also 'print-pairs', 'print-space'\n\
'show-flags' for output options.",
#else
      "Simulates the composition of the input string with the upper\n\
side of the top network on the stack and extracts the result from the\n\
lower side. If the command `apply down' is followed by a carriage\n\
return, the program prompts the user with apply>, prints the result\n\
and gives a prompt for a new input. Control-Z exits from this mode.\n\
You can also input END; to exit. See also 'print-pairs', 'print-space',\n\
'show-flags' for output options.",
#endif
 0},

#ifndef WORLD

 { "apply patterns down", INFO("app$ly pat$terns d$own", "IN_LINE_OR_FILE",
			       COM_D),
  " |line|< source-file",
  "applies a pattern network downward to continuous input",
  "Applies a pattern network downward continuously under a left-to-right\n\
longest-match regimen. Output is provided at every final state\n\
when there is no longer path to final state. See also the\n\
variables 'need-separators', 'delete-patterns', 'extract-patterns,\n\
and 'mark-patterns'."
 , 0},

 { "apply patterns up", INFO("apply pat$terns u$p", "IN_LINE_OR_FILE", COM_D),
  " |line|< source-file",
  "applies a pattern network upward to continuous input",
  "Applies a pattern network upword continuously under a left-to-right\n\
longest-match regimen. Output is provided at every final\n\
state when there is no longer path to final state. See also the\n\
variables 'need-separators', 'delete-patterns', 'extract-patterns,\n\
and 'mark-patterns'."
 , 0},

#endif /* WORLD */

  { "apropos", INFO("apr$opos", "READ_LINE", COM_D),
      "text",

      "print brief help information about the relating commands",

      "'apropos text' prints brief information about the commands\n\
relating to the given text.  If you need further information about a\n\
command, use the command 'help command-name'.", 0},


  { "cleanup net", INFO("cl$eanup n$et|cleanup", "ANY_PARAMETER", COM_NET ),
      "",

      "merges X:0 0:Y sequences to X:Y, if possible",

      "merges X:0 0:Y and 0:Y X:0 sequences to X:Y, if possible", 0},



  { "clear stack", INFO("cl$ear s$tack|clear", "ANY_PARAMETER", COM_S_IO),
      "",

      "removes all networks on the stack",

      "Removes all networks on the stack.", 0},

#ifndef WORLD
    { "compact net", INFO("comp$act n$et", "NRO", COM_NET),
      " |compaction_option",

      "compact the top network",
      "compact the top network using the Karttunen compaction scheme.\n\
Compaction options are: 1 = smallest, 2 = small, 3 = fast, 4 = fastest.\n\
      (default is 1)",
      0},
    
    { "uncompact net", INFO("uncomp$act n$et", "NRO", COM_NET),
      "",

      "uncompact the top network",
      "uncompact the top network according to the Karttunen compaction\
 scheme.",
      0},
#endif

  { "compact sigma", INFO("comp$act s$igma", "ANY_PARAMETER",
                          COM_NET),
      "",

      "eliminate redundant symbols and arcs",

      "all symbols that have the same distribution as the unknown\n\
symbol are eliminated from the alphabet and the corresponding\n\
redundant arcs are removed from the network.", 0},

  { "compile-replace lower", INFO("compile-re$place l$ower|com-rep l$ower",
				  "ANY_PARAMETER",

                         COM_NET),
      "",

      "compile and replace regular expressions in the net",

      "compiles every regular expression on the lower side of the network and\n\
replaces the original expression by the result of the compilation.\n\
See also 'retokenize', 'compile-replace upper'.", 0},

  { "compile-replace upper", INFO("compile-re$place u$pper|com-rep u$per",
				  "ANY_PARAMETER",
                         COM_NET),
      "",

      "compile and replace regular expressions in the net",

      "compiles every regular expression on the upper side of the network and\n\
replaces the original expression by the result of the compilation.\n\
See also 'retokenize', 'compile-replace lower'.", 0},

  { "complete net", INFO("compl$ete n$et|compl$ete", "ANY_PARAMETER",

                         COM_NET),
      "",

      "complete the simple automaton",

      "extends the top network until it has a transition for every\n\
symbol in sigma in every state. All new transitions lead to a failure\n\
state, thus the language of the network is preserved.Complete is\n\
defined only for simple networks (arity = 1), not for transducers.", 0},

  { "compose net", INFO("com$pose n$et|compo$se", "ANY_PARAMETER", COM_NET),
      "",

      "compose the finite-state networks in the stack",

    "replaces the stack with the composition of all networks currently\n\
on the stack. If the stack consists of two networks, defined as A and\n\
B, with A on top of the stack, the result is equivalent to compiling\n\
the regular expression [A .o. B].", 0},

#ifndef WORLD

  { "compose-apply up", INFO("compose-ap$ply u$p", "IN_LINE_OR_FILE", COM_D),
      " |line|< source-file",

      "applies the input line to the stack composing the results upwards",

#ifndef _WIN32
      "Simulates the composition of the input string with all the networks\n\
on the stack. The input is composed with the lower side of the network\n\
on the bottom of the stack and the result is extracted from the upper side\n\
of the network on the top of the stack.\n\
If the command `apply up' is followed by a carriage\n\
return,the program prompts the user with apply>, prints the result and\n\
gives a prompt for a new input. Control-D exits from this mode. See\n\
also the variables 'print-pairs', 'extract-patterns', 'mark_patterns'.",
#else
      "Simulates the composition of the input string with all the networks\n\
on the stack. The input is composed with the lower side of the network\n\
on the bottom of the stack and the result is extracted from the upper side\n\
of the network on the top of the stack.\n\
If the command `apply up' is followed by a carriage\n\
return,the program prompts the user with apply>, prints the result and\n\
gives a prompt for a new input. Control-Z exits from this mode. See\n\
also the variables 'print-pairs', 'extract-patterns', 'mark_patterns'.",
#endif
0},

  { "compose-apply down", INFO("compose-ap$ply d$own", "IN_LINE_OR_FILE",
			       COM_D),
      " |line|< source-file",

      "applies the input line to the stack composing the results downwards",

#ifndef _WIN32
      "Simulates the composition of the input string with all the networks\n\
on the stack. The input is composed with the upper side of the network\n\
on the top of the stack and the result is extracted from the lower side\n\
of the network on the bottom of the stack.\n\
If the command `apply up' is followed by a carriage\n\
return,the program prompts the user with apply>, prints the result and\n\
gives a prompt for a new input. Control-D exits from this mode. See\n\
also the variable 'print-pairs', 'extract-patterns', and 'mark-patterns'.",
#else
      "Simulates the composition of the input string with all the networks\n\
on the stack. The input is composed with the upper side of the network\n\
on the top of the stack and the result is extracted from the lower side\n\
of the network on the bottom of the stack.\n\
If the command `apply up' is followed by a carriage\n\
return,the program prompts the user with apply>, prints the result and\n\
gives a prompt for a new input. Control-Z exits from this mode. See\n\
also the variable 'print-pairs', 'extract-patterns', and 'mark-patterns'.",
#endif
 0},

#endif /* WORLD */

  { "concatenate net", INFO("con$catenate n$et|concat$enate", "ANY_PARAMETER",

                            COM_NET),
      "",

      "concatenate the finite-state networks in the stack",

    "replaces the stack with the concatenation of all networks\n\
currently on the stack. If the consists of two networks, defined as A\n\
and B, with A on top of the stack, the result is equivalent to\n\
compiling the regular expression [A B]. In regular expressions,\n\
concatenation is indicated simply by whitespace.", 0},

#ifndef WORLD

  { "continue script", INFO("cont$inue s$cript|continue",
			     "ANY_PARAMETER", COMM1),
      "",

      "resumes the processing of an interrupted script",

      "resumes the processing of a script that was halted by\n\
an 'interrupt script' command contained in the script itself.\n\
The processing will continue from the interrupt point.", 0},

#endif /* WORLD */

  { "crossproduct net", INFO("cr$ossproduct n$et|cross$product",
                             "ANY_PARAMETER", COM_NET),
      "",

      "return the crossproduct of the top two automata on the stack",

    "returns the crossproduct of the first two networks on the\n\
stack. They must not be transducers. If the stack consists of two\n\
networks, defined as A and B, with A on top of the stack, the result\n\
is equivalent to compiling the regular expression [A .x. B].", 0},

  { "define", INFO("def$ine", "ATOMI", COM_S_IO),
      "symbol|symbol regular expression",

      "bind the given symbol to a network compiled from a regular expression",

    "binds the given symbol to a network compiled from the regular\n\
expression. Every subsequent occurrence of the symbol in a regular\n\
expression is interpreted as the name of the corresponding language or\n\
relation. To unbind the symbol use 'undefine <symbol>'. If the regular\n\
expression is omitted, i.e. if the command is 'define <symbol>', the\n\
symbol is bound to the top net on the stack. The net is removed from the\n\
stack. \n\
See also: 'load defined', 'save defined', 'print defined', 'read regex',\n\
'undefine', and 'recursive-define'.", 0},

  { "determinize net", INFO("det$erminize n$et|det$erminize",

                            "ANY_PARAMETER", COM_NET),
      "",

      "determinize the transducer",

      "replaces the top network with an equivalent deterministic\n\
network.  A deterministic net has at most one transition per label\n\
from each state.", 0},

  { "echo", INFO("echo", "READ_LINE", COM_D),
      "text",

      "prints the text",

      "prints the text, followed by a newline.", 0},

  { "edit properties", INFO("ed$it prop$erties|ed$it", "ATOMI", COM_NET),
      "",

      "edits the property list of the top network",

      "'edit properties' allows the user to edit the property list of\n\
the top network on the stack.  Properties may be added, deleted,\n\
and modified.  Values must be integers or strings.", 0},

  { "eliminate flag", INFO("eliminate$ flag", "ATOMI", COM_NET),
      "",

      "eliminate the given flag attribute symbols from the network",

   "replaces the top network on the stack with an equivalent network\n\
that encodes directly the constraints expressed by the given flag \n\
attribute.", 0},

#ifndef WORLD

  { "encode nonid", INFO("enc$ode n$onid|nid|enid", "ANY_PARAMETER", COM_NET),
      "",

      "change fst to fsm using NONID encoding",

      "Arcs with composite labels like <a:b> are substituted by\n\
b *NONID* a . One-sided epsilons handled differently. Compression-related.", 0},

#endif /*WORLD*/


  { "epsilon-remove net", INFO("eps$ilon-remove n$et|eps$ilon-remove",

                               "ANY_PARAMETER", COM_NET),
      "",

      "epsilon-remove the transducer",

      "replaces the top network with an equivalent one that has no\n\
epsilon transitions.", 0},

#ifndef WORLD
  { "extract-compile-replace lower", INFO("extract-com$pile-replace l$ower|extract-com-rep l$ower",
				  "ANY_PARAMETER",

                         COM_NET),
      "",

      "compile and replace regular expressions in the net",

      "compiles every regular expression on the lower side of the network and\n\
replaces the original expression by the result of the compilation.\n\
More efficient on large networks than the standard compile-replace.\n\
See also 'retokenize', 'compile-replace upper'.", 0},

  { "extract-compile-replace upper", INFO("extract-compile-re$place u$pper|extract-com-rep u$per",
				  "ANY_PARAMETER",
                         COM_NET),
      "",

      "compile and replace regular expressions in the net",

      "compiles every regular expression on the upper side of the network and\n\
replaces the original expression by the result of the compilation.\n\
More efficient on large networks than the standard compile-replace.\n\
See also 'retokenize', 'compile-replace lower'.", 0},

#endif /* WORLD */

/**************************************************************
 *
 *  (UN-)FACTORIZATION AND RELATED COMMANDS
 */

#ifndef WORLD
    { "factorize down",
      INFO("fa$ctorize d$own|fd", "ANY_PARAMETER", COM_NET),
      "",
      "factorizes downwards an arbitrary FST into four FSTs",

      "Factorizes downwards an arbitrary FST, from the top of the stack,\n\
into four FSTs that are put onto the stack in the order:\n\
[1] a left-sequential FST, [2] a right-sequential FST,\n\
[3] a finitely ambiguous FST, and [4] an infinitely ambiguous FST.\n\
This command is equivalent to the successive application of more\n\
specialized factorization algorithms, namely,\n\
'factorize down infinite-ambiguity', 'factorize down finite-ambiguity',\n\
and 'factorize down unambiguous-net'.\n\
All intermediate alphabets are reduced\n\
(see 'reduce down intermediate-alphabet').\n\
When applied to an input string, the factors (parts) resulting from\n\
any factorization operate as a cascade, and generate the same result\n\
as the original FST.\n\
Every factorization can be undone by the corresponding\n\
unfactorization command.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "factorize up",
      INFO("fa$ctorize u$p|fu", "ANY_PARAMETER", COM_NET),
      "",
      "factorizes upwards an arbitrary FST into four FSTs",

      "Factorizes upwards an arbitrary FST, from the top of the stack,\n\
into four FSTs that are put onto the stack in the order:\n\
[1] an infinitely ambiguous FST, [2] a finitely ambiguous FST,\n\
[3] a right-sequential FST, and [4] a left-sequential FST.\n\
This command is equivalent to the successive application of more\n\
specialized factorization algorithms, namely,\n\
'factorize up infinite-ambiguity', 'factorize up finite-ambiguity',\n\
and 'factorize up unambiguous-net'.\n\
All intermediate alphabets are reduced\n\
(see 'reduce up intermediate-alphabet').\n\
When applied to an input string, the factors (parts) resulting from\n\
any factorization operate as a cascade, and generate the same result\n\
as the original FST.\n\
Every factorization can be undone by the corresponding\n\
unfactorization command.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "unfactorize down",
      INFO("unf$actorize d$own|ufd", "ANY_PARAMETER", COM_NET),
      "",
      "unfactorizes (merges) downwards four FSTs into one FST",

      "Unfactorizes (merges) downwards four FSTs, from the top of the stack,\n\
into one FST that is put onto the stack. The four FSTs must be in the order:\n\
[1] a left-sequential FST, [2] a right-sequential FST,\n\
[3] a finitely ambiguous FST, and [4] an infinitely ambiguous FST.\n\
Inverse operation to 'factorize down'.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "unfactorize up",
      INFO("unf$actorize u$p|ufu", "ANY_PARAMETER", COM_NET),
      "",
      "unfactorizes (merges) upwards four FSTs into one FST",

      "Unfactorizes (merges) upwards four FSTs, from the top of the stack,\n\
into one FST that is put onto the stack. The four FSTs must be in the order:\n\
[1] an infinitely ambiguous FST, [2] a finitely ambiguous FST,\n\
[3] a right-sequential FST, and [4] a left-sequential FST.\n\
Inverse operation to 'factorize up'.", 0},
#endif /* WORLD */

/*------------------------------------------------*/

#ifndef WORLD
    { "factorize down infinite-ambiguity",
      INFO("fa$ctorize d$own i$nfinite-ambiguity|fdi", "ANY_PARAMETER", COM_NET),
      "",
      "factorizes downwards an infinitely ambiguous FST into two FSTs",

      "Factorizes downwards an infinitely ambiguous FST, from the top\n\
of the stack, into two FSTs that are put onto the stack in the order:\n\
[1] a finitely ambiguous FST, and [2] an infinitely ambiguous FST.\n\
The intermediate alphabet is reduced (see 'reduce down intermediate-alphabet').\n\
When applied to an input string, the factors (parts) resulting from\n\
any factorization operate as a cascade, and generate the same result\n\
as the original FST.\n\
Every factorization can be undone by the corresponding\n\
unfactorization command.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "factorize up infinite-ambiguity",
      INFO("fa$ctorize u$p i$nfinite-ambiguity|fui", "ANY_PARAMETER", COM_NET),
      "",
      "factorizes upwards an infinitely ambiguous FST into two FSTs",

      "Factorizes upwards an infinitely ambiguous FST, from the top\n\
of the stack, into two FSTs that are put onto the stack in the order:\n\
[1] an infinitely ambiguous FST, and [2] a finitely ambiguous FST.\n\
The intermediate alphabet is reduced (see 'reduce up intermediate-alphabet').\n\
When applied to an input string, the factors (parts) resulting from\n\
any factorization operate as a cascade, and generate the same result\n\
as the original FST.\n\
Every factorization can be undone by the corresponding\n\
unfactorization command.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "unfactorize down infinite-ambiguity",
      INFO("unf$actorize d$own i$nfinite-ambiguity|ufdi", "ANY_PARAMETER", COM_NET),
      "",
      "unfactorizes (merges) downwards two FSTs into one FST",

      "Unfactorizes (merges) downwards two FSTs, from the top of the stack,\n\
into one FST that is put onto the stack. The two FSTs must be in the order:\n\
[1] a finitely ambiguous FST, and [2] an infinitely ambiguous FST.\n\
Inverse operation to 'factorize down infinite-ambiguity'.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "unfactorize up infinite-ambiguity",
      INFO("unf$actorize u$p i$nfinite-ambiguity|ufpi", "ANY_PARAMETER", COM_NET),
      "",
      "unfactorizes (merges) upwards two FSTs into one FST",

      "Unfactorizes (merges) upwards two FSTs, from the top of the stack,\n\
into one FST that is put onto the stack. The two FSTs must be in the order:\n\
[1] an infinitely ambiguous FST, and [2] a finitely ambiguous FST.\n\
Inverse operation to 'factorize up infinite-ambiguity'.", 0},
#endif /* WORLD */

/*------------------------------------------------*/

#ifndef WORLD
    { "factorize down finite-ambiguity",
      INFO("fa$ctorize d$own f$inite-ambiguity|fdf", "ANY_PARAMETER", COM_NET),
      "",
      "factorizes downwards a finitely ambiguous FST into two FSTs",

      "Factorizes downwards a finitely ambiguous FST, from the top\n\
of the stack, into two FSTs that are put onto the stack in the order:\n\
[1] an unambiguous FST, and [2] a finitely ambiguous FST.\n\
The intermediate alphabet is reduced (see 'reduce down intermediate-alphabet').\n\
When applied to an input string, the factors (parts) resulting from\n\
any factorization operate as a cascade, and generate the same result\n\
as the original FST.\n\
Every factorization can be undone by the corresponding\n\
unfactorization command.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "factorize up finite-ambiguity",
      INFO("fa$ctorize u$p f$inite-ambiguity|fuf", "ANY_PARAMETER", COM_NET),
      "",
      "factorizes upwards a finitely ambiguous FST into two FSTs",

      "Factorizes upwards a finitely ambiguous FST, from the top\n\
of the stack, into two FSTs that are put onto the stack in the order:\n\
[1] a finitely ambiguous FST, and [2] an unambiguous FST.\n\
The intermediate alphabet is reduced (see 'reduce up intermediate-alphabet').\n\
When applied to an input string, the factors (parts) resulting from\n\
any factorization operate as a cascade, and generate the same result\n\
as the original FST.\n\
Every factorization can be undone by the corresponding\n\
unfactorization command.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "unfactorize down finite-ambiguity",
      INFO("unf$actorize d$own f$inite-ambiguity|ufdf", "ANY_PARAMETER", COM_NET),
      "",
      "unfactorizes (merges) downwards two FSTs into one FST",

      "Unfactorizes (merges) downwards two FSTs, from the top of the stack,\n\
into one FST that is put onto the stack. The two FSTs must be in the order:\n\
[1] an unambiguous FST, and [2] a finitely ambiguous FST.\n\
Inverse operation to 'factorize down finite-ambiguity'.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "unfactorize up finite-ambiguity",
      INFO("unf$actorize u$p f$inite-ambiguity|ufuf", "ANY_PARAMETER", COM_NET),
      "",
      "unfactorizes (merges) upwards two FSTs into one FST",

      "Unfactorizes (merges) upwards two FSTs, from the top of the stack,\n\
into one FST that is put onto the stack. The two FSTs must be in the order:\n\
[1] an unambiguous FST, and [2] a finitely ambiguous FST.\n\
Inverse operation to 'factorize up finite-ambiguity'.", 0},
#endif /* WORLD */

/*------------------------------------------------*/

#ifndef WORLD
    { "factorize down unambiguous-net",
      INFO("fa$ctorize d$own u$nambiguous-net|fdu", "ANY_PARAMETER", COM_NET),
      "",
      "factorizes downwards an unambiguous FST into two FSTs (bimachine)",

      "Factorizes downwards an unambiguous FST, from the top of the stack,\n\
into two FSTs (bimachine) that are put onto the stack in the order:\n\
[1] a left-sequential FST, and [2] a right-sequential FST.\n\
An input string is first processed, from left to right, by the left-sequential\n\
FST and then, from right to left, by the right-sequential FST.\n\
Every factorization can be undone by the corresponding\n\
unfactorization command.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "factorize up unambiguous-net",
      INFO("fa$ctorize u$p u$nambiguous-net|fuu", "ANY_PARAMETER", COM_NET),
      "",
      "factorizes upwards an unambiguous FST into two FSTs (bimachine)",

      "Factorizes upwards an unambiguous FST, from the top of the stack,\n\
into two FSTs (bimachine) that are put onto the stack in the order:\n\
[1] a right-sequential FST, and [2] a left-sequential FST.\n\
An input string is first processed, from left to right, by the left-sequential\n\
FST and then, from right to left, by the right-sequential FST.\n\
Every factorization can be undone by the corresponding\n\
unfactorization command.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "unfactorize down unambiguous-net",
      INFO("unf$actorize d$own u$nambiguous-net|ufdu", "ANY_PARAMETER", COM_NET),
      "",
      "unfactorizes (merges) downwards two FSTs into one FST",

      "Unfactorizes (merges) downwards two FSTs, from the top of the stack,\n\
into one FST that is put onto the stack. The two FSTs must be in the order:\n\
[1] a left-sequential FST, and [2] a right-sequential FST.\n\
Inverse operation to 'factorize down unambiguous-net'.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "unfactorize up unambiguous-net",
      INFO("unf$actorize u$p u$nambiguous-net|ufuu", "ANY_PARAMETER", COM_NET),
      "",
      "unfactorizes (merges) upwards two FSTs into one FST",

      "Unfactorizes (merges) upwards two FSTs, from the top of the stack,\n\
into one FST that is put onto the stack. The two FSTs must be in the order:\n\
[1] a right-sequential FST, and [2] a left-sequential FST.\n\
Inverse operation to 'factorize up unambiguous-net'.", 0},
#endif /* WORLD */

/*------------------------------------------------*/

#ifndef WORLD
    { "factorize down short-ambiguity",
      INFO("fa$ctorize d$own s$hort-ambiguity|fds", "ANY_PARAMETER", COM_NET),
      "",
      "factorizes downwards an arbitrary FST into two FSTs",

      "Factorizes downwards an arbitrary FST, from the top of the stack,\n\
into two FSTs that are put onto the stack in the order:\n\
[1] an FST containing most of the original FST, and [2] an FST containing\n\
the short ambiguity (one arc long) of the original FST.\n\
When applied to an input string, the factors (parts) resulting from\n\
any factorization operate as a cascade, and generate the same result\n\
as the original FST.\n\
Every factorization can be undone by the corresponding\n\
unfactorization command.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "factorize up short-ambiguity",
      INFO("fa$ctorize u$p s$hort-ambiguity|fus", "ANY_PARAMETER", COM_NET),
      "",
      "factorizes upwards an arbitrary FST into two FSTs",

      "Factorizes upwards an arbitrary FST, from the top of the stack,\n\
into two FSTs that are put onto the stack in the order:\n\
[1] an FST containing the short ambiguity (one arc long) of the original FST,\n\
and [2] an FST containing most of the original FST.\n\
When applied to an input string, the factors (parts) resulting from\n\
any factorization operate as a cascade, and generate the same result\n\
as the original FST.\n\
Every factorization can be undone by the corresponding\n\
unfactorization command.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "unfactorize down short-ambiguity",
      INFO("unf$actorize d$own s$hort-ambiguity|ufds", "ANY_PARAMETER", COM_NET),
      "",
      "unfactorizes (merges) downwards two FSTs into one FST",

      "Unfactorizes (merges) downwards two FSTs, from the top of the stack,\n\
into one FST that is put onto the stack. The two FSTs must be in the order:\n\
[1] an FST containing most of the original FST, and [2] an FST containing\n\
the short ambiguity (one arc long) of the original FST.\n\
Inverse operation to 'factorize down short-ambiguity'.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "unfactorize up short-ambiguity",
      INFO("unf$actorize u$p s$hort-ambiguity|ufus", "ANY_PARAMETER", COM_NET),
      "",
      "unfactorizes (merges) upwards two FSTs into one FST",

      "Unfactorizes (merges) upwards two FSTs, from the top of the stack,\n\
into one FST that is put onto the stack. The two FSTs must be in the order:\n\
[1] an FST containing most of the original FST, and [2] an FST containing\n\
the short ambiguity (one arc long) of the original FST.\n\
Inverse operation to 'factorize up short-ambiguity'.", 0},
#endif /* WORLD */

/*------------------------------------------------*/

#ifndef WORLD
    { "align down finite-ambiguity",
      INFO("al$ign d$own f$inite-ambiguity|adf", "ANY_PARAMETER", COM_NET),
      "",
      "aligns downwards alternative arcs of a finitely ambiguous FST",

      "Aligns downwards alternative arcs of a finitely ambiguous FST,\n\
from the top of the stack, by introducing deterministic espilons.\n\
The algorithm is a precondition to the factorization of both\n\
finitely ambiguous and unambiguous FSTs.\n\
It is automatically called when those commands are executed.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "align up finite-ambiguity",
      INFO("al$ign u$p f$inite-ambiguity|auf", "ANY_PARAMETER", COM_NET),
      "",
      "aligns upwards alternative arcs of a finitely ambiguous FST",

      "Aligns upwards alternative arcs of a finitely ambiguous FST,\n\
from the top of the stack, by introducing deterministic espilons.\n\
The algorithm is a precondition to the factorization of both\n\
finitely ambiguous and unambiguous FSTs.\n\
It is automatically called when those commands are executed.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "unalign finite-ambiguity",
      INFO("una$lign f$inite-ambiguity|uaf", "ANY_PARAMETER", COM_NET),
    "",
    "removes the alignment of alternative arcs",

    "Removes the alignment of alternative arcs in the FST on top of the stack.\n\
Replaces all deterministic espilons by ordinary ones, and minimizes the FST.\n\
Inverse operation to both 'align down finite-ambiguity' and\n\
'align up finite-ambiguity'.", 0},
#endif /* WORLD */

/*------------------------------------------------*/

#ifndef WORLD
    { "reduce down intermediate-alphabet",
      INFO("re$duce d$own i$ntermediate-alphabet|rdia", "ANY_PARAMETER", COM_NET),
      "",
      "reduces downwards the intermediate alphabets in a cascade of FSTs",

      "Reduces downwards the intermediate alphabets in a cascade consisting\n\
of all FSTs on the stack. All redundant intermediate symbols are removed.\n\
This command is automatically called after every type of factorization.\n\
There is no inverse operation.", 0},
#endif /* WORLD */

#ifndef WORLD
    { "reduce up intermediate-alphabet",
      INFO("re$duce u$p i$ntermediate-alphabet|ruia", "ANY_PARAMETER", COM_NET),
      "",
      "reduces upwards the intermediate alphabets in a cascade of FSTs",

      "Reduces upwards the intermediate alphabets in a cascade consisting\n\
of all FSTs on the stack. All redundant intermediate symbols are removed.\n\
This command is automatically called after every type of factorization.\n\
There is no inverse operation.", 0},
#endif /* WORLD */

/*------------------------------------------------*/

#ifndef WORLD
    { "test down fail-safety",
      INFO("t$est d$own f$ail-safety|tdf", "ANY_PARAMETER", COM_NET),
      "",
      "tests downwards the fail-safety",

      "Tests downwards the fail-safety of the net on top of the stack.\n\
The net is fail-safe if none of the strings in the input language\n\
of the net fails on any path.\n\
NOTE: This test works correctly only on an FST with aligned ambiguity\n\
(see 'align down finite-ambiguity').", 0},
#endif /* WORLD */

#ifndef WORLD
    { "test up fail-safety",
      INFO("t$est u$p f$ail-safety|tuf", "ANY_PARAMETER", COM_NET),
      "",
      "tests upwards the fail-safety",

      "Tests upwards the fail-safety of the net on top of the stack.\n\
The net is fail-safe if none of the strings in the input language\n\
of the net fails on any path.\n\
NOTE: This test works correctly only on an FST with aligned ambiguity\n\
(see 'align up finite-ambiguity').", 0},
#endif /* WORLD */


/**************************************************************
 */

  { "help", INFO("\\?|he$lp|desc$ribe", "COMMAND", COM_D),
      " |command-name",

      "print this message",

      "'help' lists the commands and the command classes.\n\
\n\
To get more information about a command, use the command 'help\n\
command'.  It gives a longer explanations about the command and its\n\
usage.  All possible arguments are listed and explained.  For\n\
instance, the command 'help' (see above) may be used without arguments\n\
or it may have one argument that is a name of a command.\n\
\n\
The command name can be a name of the class of commands, for instance,\n\
'help stack' prints all stack commands.  If the given command name\n\
is not recognised, command help works like command 'apropos'.", 0},

  { "inspect net", INFO("ins$pect n$et|ins$pect", "ATOMI", COM_D),
      "",

      "inspect interactively the top network on the stack",

      "invokes a mode for interactively inspecting the top network on\n\
the stack.", 0},

#ifndef WORLD

  { "interrupt script", INFO("int$errupt s$cript|interrupt",
			     "ANY_PARAMETER", COMM1),
      "",

      "interrupts a script in the middle of processing it",

      "interrupts the script in process and returns the user to the\n\
fst command line. The interrupted script may be resumed with the\n\
command 'continue script'.", 0},

#endif /* WORLD */

  { "intersect net", INFO("int$ersect n$et|intersect", "ANY_PARAMETER",

         COM_NET),
      "",

      "intersect finite-state networks in the stack",

      "replaces the stack with the intersection of all networks\n\
currently on the stack. If the stack consists of two networks, defined\n\
as A and B, regardless of the order, the result is equivalent to\n\
compiling the regular expression [A & B].", 0},

  { "invert net", INFO("inv$ert n$et|inv$ert", "ANY_PARAMETER", COM_NET),
      "",

      "invert upper and lower side of a transducer",

      "exchanges the two sides of the top network on the stack.  If\n\
the network is defined as A, the result is equivalent to compiling the\n\
regular expression [A.i].  'invert net' has no effect on simple\n\
networks.", 0},

  { "label net", INFO("lab$el n$et", "ANY_PARAMETER",

                          COM_NET),
      "",

      "replaces the top network with a network of its label alphabet.",

      "replaces the top network on the stack with a network that\n\
represents the language or relation that includes all the strings\n\
and string pairs in the label alphabet of the original network.\n\
See also 'print labels', 'sigma net'.", 0},

  { "list", INFO("list", "ATOMI", COM_S_IO),
      "symbol list-of-symbols",

      "bind the given symbol to a list of symbols",

    "associates the given symbol with a set of symbols. Only useful\n\
in connection with the 'compile-replace upper' and 'compile-replace lower'\n\
commands. List definitions have no effect elsehere. For example,\n\
'list Vow a e i o u' assigns to 'Vow' the symbols 'a', 'e', 'i', o', \n\
and 'u'. See also 'unlist', 'print lists'.", 0},

  { "load defined", INFO("l$oad def$ined|loadd", "IN_FILE", COM_S_IO),
      "source-file",

      "loads and binds symbol definitions from a binary file",

      "restores the definitions from a binary source file. The\n\
networks are assigned to the chosen names directly, without pushing\n\
them onto the stack. The file must be in the binary format, as\n\
produced by the 'save defined' command.", 0},

  { "load stack", INFO("lo$ad s$tack|lo$ad", "IN_FILE", COM_S_IO),
      "< source-file",

      "reads finite state automata from the file",

      "reads the networks in the given file, and pushes them onto the\n\
the stack.  The file must be in the binary format, as produced by the\n\
'save stack' command. If the file only contains a single network, it\n\
can also be loaded with the command 'read regex [@\"source-file\"];'.", 0},

  { "lower-side net", INFO("low$er-side n$et|lower-side", "ANY_PARAMETER",

                           COM_NET),
      "",

      "extract the lower side of the transducer on the top of the stack",

    "extracts the lower language of the top network on the stack. If\n\
the network is defined as A, the result is equivalent to compiling the\n\
regular expression [A.l].  'lower-side net' has no effect on simple\n\
networks. ", 0},

  { "minimize net", INFO("mini$mize n$et|mini$mize", "ANY_PARAMETER",

                         COM_NET),
      "",

      "minimize the number of states in the network",

   "replaces the top network on the stack with an equivalent network\n\
that has a minimal number of states.", 0},

  { "minus net", INFO("minu$s n$et|minu$s", "ANY_PARAMETER", COM_NET),
      "",

      "return net that accepts the strings of the top net on the stack\n\
minus the strings of the second net",

    "replaces the top two networks on the stack with a network\n\
containing all strings in the first network that are not in the\n\
second. If the two networks are defined as A and B, with A on top of\n\
the stack, the result is equivalent to compiling the regular\n\
expression [A - B].", 0},

#ifndef WORLD
{ "multi char sigma net", INFO("mu$lti c$har sig$ma n$et|mcs", "ANY_PARAMETER",

                          COM_NET),
      "",

      "replaces the top network with the network of its multi character words",

      "replaces the top network on the stack with a network that\n\
represents the language of the words of the original network. \n\
The words are composed into multi-character symbols. See also \n\
'single char sigma net', 'print sigma', 'label net'\n\
'sigma net'.", 0},
#endif /*WORLD */

  { "name net", INFO("na$me n$et|na$me", "ATOMI", COM_NET),
      "symbol",

      "gives a name to the top network on the stack",

      "assigns the given name to the top network on the stack.", 0},

  { "negate net", INFO("neg$ate n$et|neg$ate", "ANY_PARAMETER", COM_NET),
      "",

      "calculate a negation of the top net of the stack",

    "replaces the top network on the stack with a network that accepts\n\
all and only those strings rejected by the original. If the network is\n\
defined as A, the result is equivalent to compiling the regular\n\
expression [~A].", 0},

  { "one-plus net", INFO("on$e-plus n$et|on$e-plus", "ANY_PARAMETER",

                         COM_NET),
      "",

      "concatenate with itself the top net of the stack",

    "concatenates the top network any number of times with itself. If\n\
the network is defined as A, the result is equivalent to compiling the\n\
regular expression [A+].", 0},

#ifndef WORLD
  { "optimize net", INFO("opt$imize n$et|opt$imize", "ANY_PARAMETER",

                         COM_NET),
      "",

      "tries to reduce the number of arcs at the cost of adding more states",

    "runs a heuristic algorithm that tries to reduce the number of\n\
arcs, possibly at the cost of introducing more states.", 0},

  { "paste net labels", INFO("pa$ste n$et l$abels|pnl", "ANY_PARAMETER",

			     COM_NET),
    "",

    "pastes the labels of the top network together. Handles loops",

    "replaces the top network on the stack with a network that\n\
represents the same language as the original network.\n\
Along every path, symbols (labels) are pasted together into\n\
as few as possible multi-character symbols. Loops and branching\n\
are handled correctly.\n\
See also 'multi char sigma net' 'single char sigma net',\n\
'print sigma', 'label net', 'sigma net'.", 0},
#endif /* WORLD */

  { "pop stack", INFO("po$p s$tack|po$p", "ANY_PARAMETER", COM_S_IO),
      "",

      "remove the top network on the stack",

      "removes the top network on the stack.", 0},

  { "push defined", INFO("pu$sh def$ined|pu$sh", "ATOMI", COM_S_IO),
      "",

      "push a defined network onto the stack",

    "makes a copy of the network named by a previous 'define' command\n\
and pushes it onto the stack. 'push defined SYMBOL' has the same effect\n\
as 'read regex SYMBOL ;'.", 0},

  { "print aliases", INFO("pr$int a$liases|aliases", "ANY_PARAMETER", COM_D),
      " |> target-file",

      "print the aliases and their values",

      "prints each alias and the command sequence it encodes.", 0},

  { "print defined", INFO("pr$int de$fined|pdef$ined", "ANY_PARAMETER",

                          COM_D),
      " |> target-file",

      "print defined symbols (see define)",

      "prints each defined symbol and the size of the network it stands for.", 0}
  ,

  { "print directory", INFO("pr$int di$rectory|dir$ectory", "READ_LINE",

                            COM_D),
      " |filenames |[filenames] > target-file",

      "print the content of directory",

      "prints the content of the current directory in its entirety or\n\
the files that match the given pattern, such as '*.txt'.", 0},

#ifndef WORLD

  { "print eqv-labels", INFO("pr$int eq$v-labels|eqv$-labels",

                             "ANY_PARAMETER", COM_D),
      " |> target-file",

      "print label equivalence classes",

      "prints the label equivalence classes of the top network on the\n\
stack. Labels that belong to the same equivalence class always appear\n\
together on arcs that lead to the same destination state.", 0},

  { "print dot net", INFO("pr$int dot n$et|pdo$t|dot", "ATOMI", COM_D),
      " |symbol|        > target-file",

      "prints a DOT representation of the top network on the stack",

      "prints a  DOT (textual) representation for visualization\n\
of the top network on the stack (by default) or the given one (the \n\
ordinal number of the network).\n\
Without output file redirection, the command will attempt to \n\
visualize the file in a separate window. A temporary PostScript file\n\
will be created, to be deleted by the user.\n\
The output is directed to a file by adding a greater-than symbol, >,\n\
and a name of a target file, to the end of the command. For example,\n\
`print dot net > myfst.dot'. The file can then be processed by dot,\n\
for example, `dot -Tps myfst.dot -o myfst.ps' and the result can be\n\
visualized (in the example, by a PostScript viewer like `gv').\n\
Temporary files go to TMPDIR (or to /tmp or /var/tmp) then they will\n\
be deleted automatically (except for the online .ps).", 0},

#endif /* WORLD */

  { "print file-info", INFO("pr$int f$ile-info|file$-info", "ANY_PARAMETER",

                            COM_D),
      " |> target-file",

      "displays info about the last opened (fst) file",

      "displays info about the last opened binary file.", 0},

  { "print flags", INFO("pr$int fl$ags|flags", "ANY_PARAMETER", COM_D),
      " |> target-file",

      "displays info about the top network on the stack",

      "'print flags' prints information about the top network on the stack.", 0},

  { "print labels", INFO("pr$int la$bels|lab$els", "ATOMI", COM_D),
    " |symbol|  > target-file",

      "prints the alphabet of the network",

       "prints the label alphabet of the top network on the stack (by\n\
default) or of the given defined network.  The label alphabet includes\n\
all the symbols and symbol pairs that occur as arc labels in the\n\
network.  See also 'print sigma', 'label net'." , 0},

  { "print label-tally", INFO("pr$int label-t$ally|label-t$ally|ta$lly",

                              "ANY_PARAMETER", COM_D),
      " |> target-file",

      "displays info about label frequencies of the top network on the stack",

      "tallies label frequencies in the top network on the stack.", 0},

  { "print lists", INFO("pr$int list$s", "ANY_PARAMETER",

                          COM_D),
      " |> target-file",

      "prints the symbols defined by a list (see list)",

    "print all the symbols defined by a list command and the members \n\
of each set. See also 'list', 'unlist'.", 0},


  { "print longest-string", INFO("pr$int lo$ngest-string|lon$gest-string|pls", "ANY_PARAMETER", COM_D),
  
    " |> target-file",

      "prints the longest string in the net",

      "prints the longest path and its length of the top net.", 0},

 { "print longest-string-size", INFO("pr$int longest-string-size|longest-string-size|psl", "ANY_PARAMETER", COM_D),
  
    " |> target-file",

      "prints the size of the longest string in the net",

      "prints the length of the longest path in the top net.", 0},

  { "print lower-words", INFO("pr$int lo$wer-words|low$er-words", "ATOMI",

                              COM_D),
      " |name|number|name number|       > target-file",

      "displays the words accepted by the lower side of the top network",

      "displays the words in the lower side language of the top\n\
network or in the network assigned to the defined symbol. If a number\n\
is given, only that many words are printed, otherwise every word is\n\
printed but not looping on any path more than once. The output can be\n\
directed to a file by adding a greater-than symbol, >, and a name of a\n\
target file to the end of the command. For example, 'print lower-words\n\
> lower-words.txt'.  If the network is a simple network rather than a\n\
transducer, 'print lower-words', 'print words', and 'print\n\
upper-words' give the same output.  See also the variables\n\
'print-space', 'show-flags', and 'quote-special'.", 0},

  { "print name", INFO("pr$int na$me|pna$me", "ATOMI", COM_D),
      " |> target-file",

      "prints the name of the top network on the stack",

      "prints the name of the top network on the stack. If the net is\n\
unnamed, the hex address of the net is printed instead. Use the\n\
command 'name net' to give a name to the top network.", 0},

  { "print net", INFO("pr$int n$et|net", "ATOMI", COM_D),
      " |symbol|        > target-file",

      "prints a text representation of the top network on the stack",

      "prints a text representation of the top network on the stack\n\
(by default) or the given definition.  The output is directed to a\n\
file by adding a greater-than symbol, >, and a name of a target file\n\
to the end of the command. See 'print-pairs'",
 0},

#ifndef WORLD

  { "print nth-lower", INFO("pr$int nth-l$ower|nth-l$ower", "ATOMI", COM_D),
      " |name|number|name number|       > target-file",

      "displays the word of the nth path of the lower part of the top\n\
network or defined symbol",

      "displays the word on the lower side of the nth path in the top\n\
level network or in the network assigned to the defined symbol. This\n\
command only works on acyclic networks. The inverse command is 'print\n\
num-lower'.", 0},

  { "print nth-upper", INFO("pr$int nth-u$pper|nth-u$pper", "ATOMI", COM_D),
      " |name|number|name number|       > target-file",

      "displays the word of the nth path of the upper part of the top\n\
network or defined symbol",

      "displays the word on the upper side of the nth path in the top\n\
level network or in the network assigned to the defined symbol.  This\n\
command only works on acyclic networks.  The inverse command is 'print\n\
num-upper'.", 0},


  { "print num-lower", INFO("pr$int num-l$ower|num-l$ower", "ATOMI", COM_D),
      "word||name word| > target-file",

      "displays the numbers of all paths having word in the lower part\n\
of the top network",

      "displays the numbers of all paths in the top level network that\n\
have word on the lower side. This command only works on acyclic\n\
networks. The inverse command is 'print nth-lower'.", 0},

  { "print num-upper", INFO("pr$int num-u$pper|num-u$pper", "ATOMI", COM_D),
      "word|name word|  > target-file",

      "displays the numbers of all paths having word in the upper part\n\
of the top network",

      "displays the numbers of all paths in the top level network that\n\
have word on the upper side. This command only works on acyclic\n\
networks. The inverse command is 'print nth-upper'.", 0},

#endif /* WORLD */

  { "print random-lower", INFO("pr$int random-l$ower|random-l$ower", "ATOMI",

                               COM_D),
      " |number|        > target-file",

      "displays a selection of the words accepted by the lower part of\n\
the top network",

    "generates 15 random words (by default) from the top network on\n\
the stack or any number specified in the command. The words are from\n\
the lower side language of the net. If the network is a simple\n\
automaton, rather than a transducer, 'print random-lower' gives the\n\
same output as 'print random' and 'print random-upper'. See also the\n\
variable 'print-space'.", 0},

  { "print random-upper", INFO("pr$int random-u$pper|random-u$pper", "ATOMI",

                               COM_D),
      " |number|        > target-file",

      "displays a selection of the words accepted by the upper part of\n\
the top network",

    "generates 15 random words (by default) from the top network on\n\
the stack, or any number specified in the command. The words are from\n\
the upper side language of the net. If the network is a simple\n\
automaton, rather than a transducer, 'print random-upper' gives the\n\
same output as 'print random' and 'print random-lower'. See also the\n\
variable 'print-space'.", 0},

  { "print random-words", INFO("pr$int random-w$ords|random-w$ords", "ATOMI",

                               COM_D),
    " |number|        > target-file",

    "displays a selection of the words accepted by the top network",

    "generates 15 random words (by default) from the top network on the\n\
stack or any number specified in the command. If the network is a\n\
transducer, the output may contain symbol pairs. If the network is a\n\
simple automaton, rather than a transducer, 'print random-words' gives\n\
the same output as 'print random-lower' and 'print random-upper'. See\n\
also the variable 'print-space'.", 0},

  { "print sigma", INFO("pr$int s$igma|sig$ma", "ATOMI", COM_D),
      " |symbol|        > target-file",

      "prints the alphabet of the network",

      "prints the sigma alphabet of the top network on the stack (by\n\
default) or the given defined network. The sigma alphabet includes all the\n\
symbols that appear in the network, either as such or as a constituent\n\
of a symbol pair, and any symbols that have been excluded from the\n\
network by subtraction or complementation. See also 'print labels',\n\
'sigma net'.", 0},

 { "print sigma-tally", INFO("pr$int sigma-t$ally|sigma-t$ally|sit$ally",

                              "ANY_PARAMETER", COM_D),
      " |> target-file",

      "displays info about sigma frequencies of the top network on the stack",

      "tallies sigma arc frequencies in the top network on the stack.", 0},

{ "print sigma-word-tally", INFO("pr$int sigma-w$ord-tally|swt|pswt", "ATOMI", COM_D),

      " |upper|lower|> target-file",

      "displays info about sigma frequencies of the top network on the stack",

      "tallies sigma frequencies by path in the top network on the stack.", 0},

  { "print size", INFO("pr$int siz$e|siz$e", "ATOMI", COM_D),
      " |symbol|        > target-file",

      "prints the size of the network",

      "prints the size of the top network on the stack (by default) or\n\
the given definition.", 0},


  { "print stack", INFO("pr$int sta$ck|sta$ck", "ATOMI", COM_D),
      " |        > target-file",

      "displays the content of the stack",

      "displays the content of the stack.", 0},

#ifndef WORLD
  { "print storage", INFO("pr$int sto$rage|sto$rage", "ANY_PARAMETER", COM_D),
      " |        > target-file",

      "displays memory usage",

      "displays memory usage", 0},
#endif /* WORLD */
  { "print upper-words", INFO("pr$int upp$er-words|upp$er-words", "ATOMI",

                COM_D),
      " |name|number|name number|        > target-file",

      "displays the words accepted by the upper side of the top network",

      "displays the words in the upper side language of the top\n\
network or in the network assigned to the defined symbol. If a number\n\
is given, only that many words are printed, otherwise every word is\n\
printed but not looping on any path more than once. The output can be\n\
directed to a file by adding a greater-than symbol, >, and a name of a\n\
target file to the end of the command. For example, 'print upper-words\n\
> upper-words.txt'. If the network is a simple network rather than a\n\
transducer, 'print lower-words', 'print words', and 'print\n\
upper-words' give the same output. See also the variables\n\
'print-space', 'show-flags', and 'quote-special'.", 0},

#ifndef WORLD
  { "print vcg net", INFO("pr$int vcg n$et|pvc$g|vcg", "ATOMI", COM_D),
      " |symbol|        > target-file",

      "prints a VCG/GDL representation of the top network on the stack",

      "prints a  VCG/GDL (textual) representation for visualization\n\
of the top network on the stack (by default) or the given one (the \n\
ordinal number of the network).\n\
Without output file redirection, the `xvcg' program will attempt to \n\
visualize the file in a separate window (see `man xvcg').\n\
The output is directed to a file by adding a greater-than symbol, >,\n\
and a name of a target file, to the end of the command. For example,\n\
`print vcg net > myfst.gdl'. The file can then be visualized by the\n\
`xvcg' tool. Temporary files go to TMPDIR (or /tmp or /var/tmp) then\n\
they will be deleted automatically.", 0},
#endif /* WORLD */

  { "print words", INFO("pr$int w$ords|word$s", "ATOMI", COM_D),
       " |name|number|name number",

      "displays the words accepted by the top network",

      "displays the paths in the top network or in the network\n\
assigned to the defined symbol. If a number is given, only that many\n\
paths are printed, otherwise every word is printed but not looping on\n\
any path more than once. The output can be directed to a file by\n\
adding a greater-than symbol, >, and a name of a target file to the\n\
end of the command. For example, 'print words > words.txt' If the\n\
network is a transducer, the output may contain symbol pairs.\n\
Otherwise, 'print lower-words', 'print words', and 'print upper-words'\n\
give the same output. See also the variables 'print-space', 'show-flags',\n\
and 'quote-special'.", 0},

  { "prune net", INFO("pru$ne n$et|pru$ne", "ANY_PARAMETER", COM_NET),
      "",

      "prune the network",

      "removes all paths leading to non-final states in the top\n\
network on the stack.", 0},

#ifndef WORLD

  { "push epsilons", INFO("pu$sh e$psilons|pe$psi", "ANY_PARAMETER", COM_NET),
      " | side from_level over_level algorithm",

      "Push epsilons further to the right on all paths.",

      "Compression and for other transducer speedup. Pushes epsilons, on\n\
the given SIDE (0 = UPPER, 1 = LOWER, default is UPPER), from FROM_LEVEL\n\
over OVER_LEVEL (if specified, otherwise traverses the whole net) from\n\
left to right, breadth-first, using \"local sequentialization\". See also\n\
the variable \"seq-string-onelong\" (and, for more, \"apropos seq\").\n\
The ALGORITHM (of initial selection of the arcs of a given state to be\n\
sequentialized) can be chosen. Then the closures of such arcs is\n\
construed as a network - until the allowed level - and sequentialized:\n\
\t0 (default) selects arcs labeled by epsilons on the input side;\n\
\t1 selects all arcs of a state;\n\
\t2 gives a partial selection of the arcs of the state (based on\n\
the presence of epsilons and/or non-deterministic ambiguities) by\n\
looking at both sides;\n\
\t3 is like 2 but on the input side only.\n\
The resulting network replaces the original on the stack.", 0},

#endif /* WORLD */ 

  { "quit", INFO("quit|exit|bye|stop|au revoir|hyvästi|visz$lát|auf wiedersehen|hasta la vista|arrivederci", "ANY_PARAMETER", COMM1),
      "",

      "exit program",

      "Exits the program.", 0},

  { "read lexc",  INFO("r$ead lexc", "IN_FILE", COM_S_IO),
    " |source-file",

    "reads a lexicon file and puts the resulting network on the stack",

#ifndef _WIN32
    "reads from a source file or from the console in the format of the\n\
lexc compiler, assembles all the sublexicons into a single network and\n\
pushes the result on the stack. When reading from the console use\n\
Control-D or END; to terminate the input.",
#else
    "reads from a source file or from the console in the format of the\n\
lexc compiler, assembles all the sublexicons into a single network and\n\
pushes the result on the stack. When reading from the console use\n\
Control-Z or END; to terminate the input.",
#endif
 0},

  { "read prolog", INFO("r$ead prol$og|rpl", "IN_FILE", COM_S_IO),
      " |source-file",

      "reads in Prolog-clauses and puts network(s) on the stack",
#ifndef _WIN32
      "reads the network(s) contained in the file and pushes them onto\n\
the stack.  The file must be in the format produced by the 'write\n\
prolog' command.  The result will be neither determinized nor pruned.\n\
The same effect can be produced with the command\n\
'read regex [@pl \"prolog file\"];'\n\
When reading from the console, use Control-D to terminate the input",
#else
      "reads the network(s) contained in the file and pushes them onto\n\
the stack.  The file must be in the format produced by the 'write\n\
prolog' command.  The result will be neither determinized nor pruned.\n\
The same effect can be produced with the command\n\
'read regex [@pl \"prolog file\"];'\n\
When reading from the console, use Control-Z to terminate the input",
#endif
 0},

  { "read properties", INFO("r$ead prop$erties|rprops", "IN_FILE", COM_NET),
      " |< source-file",

      "resets the properties of the top network from a file",
#ifndef _WIN32
      "reads a set of attribute-value pairs from a text file or from the\n\
console and resets the property list of the top network on the stack.\n\
Example:\n\
LANGUAGE: \"English\"\n\
CHARENCODING: \"ISO 8859\"\n\
VERSION: 7.2\n\
When reading from the console, use Control-D to terminate the input.\n\
See also: 'add properties', 'write properties'.",
#else
      "reads a set of attribute-value pairs from a text file or from the\n\
console and resets the property list of the top network on the stack.\n\
Example:\n\
LANGUAGE: \"English\"\n\
CHARENCODING: \"ISO 8859\"\n\
VERSION: 7.2\n\
When reading from the console, use Control-Z to terminate the input.\n\
See also: 'add properties', 'write properties'.",
#endif
 0},

  { "read regex", INFO("r$ead r$egex|reg$ex", "ANY_PARAMETER", COM_S_IO),
      " |< source-file|regular expression ;",

      "transform a regular expression into a finite state network",

      "creates a network from a regular expression and and pushes it\n\
onto the stack.  The file should contain one regular expression. If no\n\
filename is given the input is read from the console.  The input\n\
expression must be terminated by a semicolon.\n\n\
The command 'help regex:' gives the complete list of regular expression\n\
operators and their precedences. Commands such as 'help regex: shuffle'\n\
give help on a particular operator.", 0},

  { "read spaced-text", INFO("r$ead s$paced-text|rs", "IN_FILE", COM_S_IO),
      " | < source-file",

      "reads the net as text from a file",
#ifndef _WIN32
      "reads a transducer or a net with multicharacter symbols from a file\n\
or from the console. Separate paths with an empty line. Leave space\n\
between symbols. Zero is epsilon. %  is space. Example:\n\
l e a v e +Verb +Past\n\
l e 0 f t\n\
\n\
a t t o r n e y 0 %  g e n e r a l +Noun +Pl\n\
a t t o r n e y s %  g e n e r a l\n\
When reading from the console, use Control-D to terminate the input.\n\
See also 'read text', 'write text', 'write spaced-text'.",
#else
      "reads a transducer or a net with multicharacter symbols from a file\n\
or from the console. Separate paths with an empty line. Leave space\n\
between symbols. Zero is epsilon. %  is space. Example:\n\
l e a v e +Verb\n\
l e f 0 t\n\
\n\
a t t o r n e y 0 %  g e n e r a l +Noun +Pl\n\
a t t o r n e y s %  g e n e r a l\n\
When reading from the console, use Control-Z to terminate the input.\n\
See also 'read text', 'write text', 'write spaced-text'.",
#endif
 0},

  { "read text", INFO("r$ead t$ext|rt", "IN_FILE", COM_S_IO),
      " | < source-file",

      "reads the net as text from a file",

#ifndef _WIN32
     "reads a net as text from a file or from the console. One word per line\n\
without empty lines. Space is taken literally. Example:\n\
New York\n\
Toronto\n\
San Francisco\n\
When reading from the console, use Control-D to terminate the input.\n\
See also 'write text', 'read spaced-text', 'write spaced-text'.",
#else
     "reads a net as text from a file or from the console. One word per line\n\
without empty lines. Space is taken literally. Example:\n\
New York\n\
Toronto\n\
San Francisco\n\
When reading from the console, use Control-Z to terminate the input.\n\
See also 'write text', 'read spaced-text', 'write spaced-text'.",
#endif
 0},

#ifndef WORLD

  { "read weighted prolog", INFO("r$ead w$eighted prol$og|rwp|rwpl", "IN_FILE", COM_S_IO),
      " |source-file",

      "reads in weighted Prolog-clauses and puts network(s) on the stack",

      "reads the weighted network(s) contained in the file and pushes them\n\
onto the stack.  The file must be in the format produced by the 'write\n\
weighted prolog' command.", 0},


  { "reduce labelset", INFO("red$uce lab$elset|reduce", "ANY_PARAMETER",
			    COM_NET),
      "",

      "eliminates redundant arcs",

     "partitions the label alphabet into equivalence classes and eliminates\n\
all arcs that are not labeled by the first symbol of some equivalence\n\
class.\n\
See also 'unreduce labelset'.", 0},

  { "remove epsilon", INFO("r$emove eps$ilon|rme", "IN_FILE", COM_S_IO),
      " |source-file",

      "removes transitions with epsilon in the upperside",

      "computes transitive closure of transitions with epsilon in the \n\
upperside and removes them.  First step of network sequentialization. \n\
The computation is completed if no cycles appear in the closure.", 0},

#endif /* WORLD */ 

  { "reverse net", INFO("rev$erse n$et|rev$erse", "ANY_PARAMETER", COM_NET),
      "",

      "reverse the top net of the stack",

      "replaces the top net on the stack with one that accepts the\n\
reverse language.", 0},

  { "rotate stack", INFO("rot$ate st$ack|rot$ate", "ANY_PARAMETER", COM_S_IO),
      "",

      "pushes the top element of the stack to the bottom",

      "Pushes the top element of the stack to the bottom.", 0},   

  { "save defined", INFO("sa$ve def$ined|saved", "OUT_FILE", COM_S_IO),
      "target-file",

      "saves the definitions to a file",

      "stores the networks for all defined symbols into a binary file.\n\
Use 'load defined' to restore the definitions.", 0},

  { "save stack", INFO("sa$ve st$ack|save|ss", "OUT_FILE", COM_S_IO),
      "target-file",

      "saves the stack of finite state automata to the file",

      "stores all the networks on the stack into the given file in a\n\
binary format.  Use 'load stack' to load the file.", 0},

  { "set", INFO("se$t var$iable|set","VARIABLE", COMM1),
      "variable value",

      "sets a new value to the variable",

      "sets a new value to the variable.  The value may be an integer,\n\
ON or OFF, or a string.  See variable for details. See also:\n\
'show'.", 0},

#ifndef WORLD

  { "share arcs", INFO("share$ arcs|share", "ANY_PARAMETER", COM_NET),
      "",
      "eliminates ALTCHAIN arcs",

      "eliminates ALTCHAIN arcs and physically shares arcs between states.\n\
Has no effect on an ordinary network. See also 'optimize net' and\n\
'unshare arcs'.\n", 0},

#endif /* WORLD */



  { "show", INFO("sho$w var$iable|show","VARIABLE", COMM1),
      "variable",

      "show the value of the variable",

      "shows the value of the variable.  See also 'set'.", 0},

  { "shuffle net", INFO("shu$ffle n$et|shu$ffle", "ANY_PARAMETER", COM_NET),
      "",

      "shuffle finite-state networks in the stack",

      "replaces the stack with a single network that accepts every\n\
string formed by shuffling together (interdigitating) one string from\n\
each of the input languages.  For instance, if Net1 accepts ab and\n\
Net2 accepts xy, then the shuffle of these nets will accept the\n\
following six strings: abxy axby axyb xaby xayb xyab.", 0},

  { "sigma net", INFO("sig$ma n$et", "ANY_PARAMETER",

                          COM_NET),
      "",

      "replaces the top network with network of its sigma alphabet",

      "replaces the top network on the stack with a network that\n\
represents the language of the strings in the sigma alphabet of the\n\
original network. See also 'print sigma', 'label net'.", 0},

#ifndef WORLD
{ "single char sigma net", INFO("sin$gle c$har sig$ma n$et|scs", "ANY_PARAMETER",

                          COM_NET),
      "",

      "replaces the top network with the network of its single character sigma alphabet",

      "replaces the top network on the stack with a network that\n\
represents the language of the strings in the sigma alphabet of the\n\
original network. Multi-character symbols are decomposed into single\n\
character ones. See also 'print sigma', 'label net' 'sigma net'.", 0},
#endif /* WORLD */

  { "sort net", INFO("so$rt n$et|sor$t", "ANY_PARAMETER", COM_NET),
      "",

      "sort the transducer",

      "Reorders the arcs of the top network in a canonical order.", 0},

  { "source", INFO("sou$rce", "IN_FILE", COMM1),
      "command file",

      "execute a file of commands",

      "executes a file of XFST commands.  For example, 'source\n\
xfst.script' executes the commands in xfst.script.", 0},

  { "substitute defined", INFO("subs$titute def$ined", "ANY_PARAMETER",

                          COM_NET),
      "NAME for LABEL",

      "replaces the given label by the network associated with the name",

      "replaces the top network on the stack with a network derived by\n\
splicing in the network associated with the given name for each arc that\n\
contains the label. If the network is a transducer, LABEL must not occur\n\
in a symbol pair.", 0},

  { "substitute label", INFO("subs$titute lab$el", "ANY_PARAMETER",

                          COM_NET),
      "LIST-OF-LABELS for LABEL",

      "substitutes one or more labels for the given label",

      "replaces the top network on the stack with a network derived by\n\
replacing every arc with the given label by an arc or arcs, each labeled\n\
with one of the substitute labels. If the list consists of the keyword\n\
NOTHING, all paths containing the given label are removed. The label and\n\
the substitutes may be single symbols or a symbol pairs.\n\
See also 'substitute symbol'.", 0},

  { "substitute symbol", INFO("subs$titute sym$bol", "ANY_PARAMETER",

                          COM_NET),
      "LIST-OF-SYMBOLS for SYMBOL",

      "substitutes one or more symbols for the given symbol",

      "replaces the top network on the stack with a network derived by\n\
replacing every arc whose label contains the given symbol by an arc or\n\
arcs whose label instead contains one of the substitute symbols. If the\n\
list consists of the keyword NOTHING, all paths containing the given\n\
symbol are removed. The symbol and the substitutes must be single symbols,\n\
not symbol pairs. See also 'substitute label'.", 0},

  { "substring net", INFO("subs$tring n$et|subs$tring", "ANY_PARAMETER",

                          COM_NET),
      "",

      "collects all substrings of the top net of the stack",

      "replaces the top network on the stack with a network that\n\
accepts every string that is a substring of some string in the\n\
language of the original network.", 0},

  { "system", INFO("sy$stem", "PFILE", COMM1),
      "system-command",

      "execute a system command",

      "'system command' executes the given operating system\n\
command(s), for instance, 'system ls'.", 0},

#ifndef WORLD

  { "tag plusstrip", INFO("tag p$lusstrip|tpst|tst", "ANY_PARAMETER", COM_NET),
      "",

      "strip leading + from tag names",

      "Remove leading + from tag names.\n\
Compression-related.", 0},

  { "tagsep insert", INFO("tagsep i$nsert|tsi", "ANY_PARAMETER", COM_NET),
      "",

      "insert *TAGSEP*-s around tags",

      "Mark beginning and end of tags by *TAGSEP* (tag separator).\n\
Compression-related.", 0},

 { "tb insert", INFO("tb i$nsert|tbi|eos", "ANY_PARAMETER", COM_NET),
      "",

      "Insert EOS-s at end of lower-side strings",

      "Mark end of lower-side strings by *EOS*. *EOS* will be to\n\
the left of any lower EPSILON-s. Also called token break (tb).\n\
Compression-related.", 0},

#endif /*WORLD*/


  /*
    Tests of network properties
    */

  { "test equivalent", INFO("t$est e$quivalent|equ$ivalent|te", "ATOMI",

                            COM_T),
      "",

      "TRUE if the languages of the networks on the stack are the same.",

      "returns 1 (TRUE) if all networks on the stack contain the same\n\
language", 0},

  { "test lower-bounded", INFO("t$est lower-b$ounded|lower-b$ounded|tlb",

                               "ATOMI", COM_T),
      "",

      "TRUE if no lower epsilon cycles in the top network on the stack",

      "returns 1 (TRUE) if the lower side of the top level network has\n\
no epsilon cycles.", 0},

  { "test lower-universal",

    INFO("t$est lower-u$niversal|lower-u$niversal|tlu", "ATOMI", COM_T),
    "",

    "TRUE if the lower language of the top network on the stack is universal",

    "returns 1 (TRUE) if the lower side of the top level network\n\
represents the universal language.", 0},

 { "test non-null", INFO("t$est n$on-null|tnn", "ATOMI", COM_T),
      "",

      "TRUE if the top network on the stack is a non-null fsm",

      "returns 1 (TRUE) if the top network on the stack is not\n\
a null fsm.", 0},

 { "test null", INFO("t$est nu$ll|tnu", "ATOMI", COM_T),
      "",

      "TRUE if the top network on the stack is a null fsm",

      "returns 1 (TRUE) if the top network on the stack is a null\n\
fsm.", 0},

  { "test overlap", INFO("t$est o$verlap|ov$erlap|to", "ATOMI",

                         COM_T),
    "",

    "TRUE if the languages overlap (of all networks on the stack)",

    "returns 1 (TRUE) if the languages of all networks on the stack\n\
have a non-empty intersection.", 0},

#ifndef WORLD
 { "test preprocess", INFO("t$est pre$process|tpp", "ATOMI",
				  COM_D),
      " |PointerNo",

      "TEST preprocess fsm",

      "TEST preprocess values of top net. Compression-related.", 0},

 { "test reducesigma", INFO("t$est r$educesigma|trs", "ATOMI",
				  COM_D),
      " |PointerNo",

      "TEST sigma reduction",

      "TEST. Reduce sigma. Stack will contain mapper fst-s of \n\
single characters and/or tags (if any). Compression-related.", 0},
#endif /* WORLD */


  { "test sublanguage", INFO("t$est subl$anguage|subl$anguage|ts", "ATOMI",

                             COM_T),
      "",

      "TRUE if the language of a network is a sublanguage of the next",

      "returns 1 (TRUE) if the language of the i-th network is a\n\
sublanguage of the next (i+1-th) one on the stack. Starting at the\n\
topmost network (i=0). See also 'print stack' and 'turn stack'.", 0},

#ifndef WORLD
  { "test unambiguous-down", INFO("t$est unamb$iguous-down|tunambd", "ATOMI",
				  COM_T),
      "",

      "TRUE if the transducer is unambiguous in the downward direction",

      "returns 1 (TRUE) if the transducer is unambiguous in the downward\n\
direction", 0},

  { "test unambiguous-up", INFO("t$est unamb$iguous-up|tuambu", "ATOMI",
				COM_T),
      "",

      "TRUE if the transducer is unambiguous in the upward direction",

      "returns 1 (TRUE) if the transducer is unambiguous in the upward\n\
direction", 0},

 { "test unicode parsing", INFO("t$est uni$code p$arsing|tup", "ATOMI",
				  COM_D),
      "word",

      "TEST unicode parsing string",

      "TEST unicode parsing on a non-unicode string. The top net \n\
(which is the base of the parse) must be a Unicode one.", 0},


#endif /* WORLD */


  { "test upper-bounded", INFO("t$est upper-b$ounded|upper-b$ounded|tub",

                               "ATOMI", COM_T),
      "",

      "TRUE if no upper epsilon cycles in the top network on the stack",

      "returns 1 (TRUE) if the upper side of the top level network has\n\
no epsilon cycles.", 0},

  { "test upper-universal",

    INFO("t$est upper-u$niversal|upper-u$niversal|tuu", "ATOMI", COM_T),
    "",

    "TRUE if the upper language is universal in the top network on the stack",

    "returns 1 (TRUE) if the upper side of the top level network\n\
contains the universal language.", 0},

  { "turn stack", INFO("tu$rn s$tack|tu$rn", "ANY_PARAMETER", COM_S_IO),
      "",

      "reverses the order of the networks on the stack",

     "reverses the order of the networks on the the stack.  NOTE:  Use the\n\
command 'reverse net' to create the reverse language of the top net.", 0},

  { "undefine", INFO("und$efine", "ATOMI", COM_S_IO),
      "list of symbols",

      "unbind a symbol or symbols that were bound by 'define'",

      "unbinds the symbol that was bound by the 'define'\n\
command. 'undefine symbol1 symbol2 symbol3 ...' etc. undefines all the\n\
listed symbols. 'undefine ALL' removes all the definitions.", 0},

  { "twosided flag-diacritics", INFO("tw$osided f$lag-diacritics|tfd", "ANY_PARAMETER", COM_NET),
      "",

      "change flag diacritics to identity pairs",

      "Flag diacritics (aka instructions) in symbol pairs are changed,\n\
for example, @U.CAP.Y@:a ==> 0:a @U.CAP.Y@ .", 0},


  { "unlist", INFO("unlist", "ATOMI", COM_S_IO),
      "symbol",

      "disassociates a symbol or symbols from an earlier 'list' set",

      "disassociates the given symbol from the set that was bound to it\n\
by a previous 'list' definition. See also 'list', 'print lists'.", 0},


  { "union net", INFO("uni$on n$et|uni$on", "ANY_PARAMETER", COM_NET),
      "",

      "makes union of finite-state networks in the stack",

      "replaces the stack with the union of all networks currently on\n\
the stack.", 0},

#ifndef WORLD

  { "unoptimize net", INFO("unopt$imize n$et|unopt$imize", "ANY_PARAMETER",

                         COM_NET),
      "",

      "reverses the effect of 'optimize net'",

    "reverses the effect of 'optimize net'", 0},

  { "unreduce labelset", INFO("unred$uce lab$elset|unreduce", "ANY_PARAMETER",
			      COM_NET),
      "",

      "undoes the work of 'reduce labelset'",

     "restores the deleted arcs that are represented by the first member\n\
of their equivalence class.\n\
See also 'reduce labelset'.", 0},

  { "unshare arcs", INFO("unsha$re arcs|unshare","ANY_PARAMETER", COM_NET),
      "",
      "restores eliminated ALTCHAIN arcs",

      "restores eliminated ALTCHAIN arcs. The arcs are not shared\n\
between states. See also 'share arcs', 'optimize net'.\n", 0},

#endif /* WORLD */

  { "upper-side net", INFO("upp$er-side n$et|upper-side",

                           "ANY_PARAMETER", COM_NET),
      "",

      "extracts the upper side of the transducer on the top of the stack",

      "extracts the upper language from the network on the top of the\n\
stack. If the network is defined as A, the result is equivalent to\n\
compiling the regular expression [A.l].  'upper-side net' has no\n\
effect on simple networks.", 0},

  { "write prolog", INFO("w$rite prol$og|wpl", "ANY_PARAMETER", COM_S_IO),
    " |> target-file",

    "writes the network as Prolog-clauses",

    "writes the network as Prolog-clauses. See also: 'read prolog'.", 0},

  { "write properties", INFO("w$rite prop$erties|props", "ATOMI", COM_D),
    " |symbol|        > target-file",

    "prints the property list of the network",

    "prints the attribute-value pairs on the property list of the\n\
network. See also 'add properties', 'read properties'.", 0},

  { "write spaced-text", INFO("w$rite s$paced-text|ws$paced-text",

                              "ANY_PARAMETER", COM_S_IO),
    " |> target-file",

    "writes the top net of the stack as text to a file", 

    "Stores the top network in a text format.  This format is intended\n\
for transducers and networks with multi-character labels.  Use\n\
'read spaced-text' to read files in this format.  Use 'write text'\n\
for simple networks with single-character labels.  Use 'save stack'\n\
to store in a compact binary format.  If no target file is specified,\n\
output will be sent to standard output (usually the terminal).", 0},

  {"write text", INFO("w$rite t$ext|wt", "ANY_PARAMETER", COM_S_IO),
   "|> target-file",

   "writes the top net of the stack as text to a file",

   "stores the words in the top network, one per line.  This format\n\
cannot be used for transducers or networks with multi-character\n\
labels.  For such networks, use 'write spaced-text' instead.  Use\n\
'save stack' to store in a compact binary format. If no target file is\n\
specified, output will be sent to standard output (usually the\n\
terminal).", 0},

#ifndef WORLD

 { "write weighted prolog", INFO("w$rite w$eighted prol$og|wwp|wwpl", "ANY_PARAMETER", COM_S_IO),
    " |> target-file",

    "writes the network as weighted Prolog-clauses",

    "writes the network as weighted Prolog-clauses. See also: 'read weighted prolog'.", 0},

#endif /* WORLD */ 

  { "zero-plus net", INFO("z$ero-plus n$et|ze$ro-plus", "ANY_PARAMETER",

                          COM_NET),
      "",

    "concatenates the top network any number of times with itself and\n\
unions it with the language consisting of the empty string. If the\n\
network is defined as A, the result is equivalent to compiling the\n\
regular expression [A*].", 0},

#ifndef WORLD
  { "stack top", INFO("st$ack top|top", "NRO", COM_S_IO),
      "index",

      "places the net of the index to the top of the stack",

      "places the net of the index to the top of the stack", 0},

 { "all", INFO("all", "COMMAND", COMM1),
      "unary command",

      "execute the given unary command with all the nets in the stack.",

      "'all <command>' executes the given unary command with all\n\
the nets in the stack.  For instance, 'all negate' does negation\n\
for each net.", 0},

  { "tabularize net", INFO("tab$ularize n$et|tab$ular", "ANY_PARAMETER", COM_NET),
      "",

      "transforms the network into a transition table presentation",

      "transforms the network into a transition table presentation", 0},


  { "local sequentialize down", INFO("l$ocal seq$uentialize d$own|l-seq-d$own|lsd", "ANY_PARAMETER", COM_NET),
      "",

      "locally sequentialize downwards the transducer (its UPPER side) on\n\
the top of the stack",

      "Experimental; use \"push epsilons\" instead.\n\
Possibly sequentiable subnets are identified and sequentialized according\n\
to a heuristic algorithm. The resulting network replaces the original on\n\
the stack.", 0},

  { "local sequentialize up", INFO("l$ocal seq$uentialize u$p|l-seq-u$p|lsu", "ANY_PARAMETER", COM_NET),
      "",


      "locally sequentialize upwards the transducer (its LOWER side) on\n\
the top of the stack",

      "Experimental; use \"push epsilons\" instead.\n\
Possibly sequentiable subnets are identified and sequentialized according\n\
to a heuristic algorithm. The resulting network replaces the original on\n\
the stack.", 0},

  { "sequentialize down", INFO("seq$uentialize d$own|seq-d$own", "ANY_PARAMETER", COM_NET),
      "",

      "sequentialize downwards the transducer (its UPPER side) on\n\
the top of the stack",

      "if possible, eliminates all epsilon transitions and ambiguities\n\
from the upper side of the toplevel network. The resulting network replaces\n\
the original on the stack.", 0},

  { "sequentialize up", INFO("seq$uentialize u$p|seq-u$p", "ANY_PARAMETER", COM_NET),
      "",


      "sequentialize upwards the transducer (its LOWER side) on\n\
the top of the stack",

      "if possible, eliminates all epsilon transitions and ambiguities\n\
from the lower side of the toplevel network. The resulting network replaces\n\
the original on the stack.", 0},

  { "test seq-down", INFO("test seq-d$own|tsd", "ANY_PARAMETER", COM_NET),
      "",

      "tests if the toplevel network is (sub)sequentiable downwards",

      "tests if the transducer on the top  of the stack (i.e. its\n\
upper side) is (sub)sequentiable downwards.", 0},

  { "test seq-up", INFO("test seq-u$p|tsu", "ANY_PARAMETER", COM_NET),
      "",

      "tests if the toplevel network is (sub)sequentiable upwards",

      "'test seq-up' is a unary operation that tests if the transducer\n\
(its LOWER side) is (sub)sequentiable upwards.", 0},

#endif /*WORLD*/

#ifndef WORLD
    /* Commands for virtual networks. */

{ "virtual compose",
 INFO("virt$ual com$pose|vcom$pose",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually composes two relations",
 "'virtual compose' replaces the two top networks on the stack\n\
by a virtual network representing the composition of the\n\
two relations if at least one of the networks is a transducer.\n\
If both networks are simple automata, the result represents\n\
the intersection of the two languages.", 0},

{ "virtual concatenate",
 INFO("virt$ual concat$enate|vconcat",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually concatenates two networks",
 "'virtual concatenate' replaces the two top networks on the stack\n\
by a virtual network representing their concatenation.", 0},

{ "virtual copy",
 INFO("virt$ual cop$y|vcop$y",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually copies a network",
 "'virtual copy' replaces the top network on the stack\n\
by its virtual copy. (An uninteresting operation.)", 0},

{ "virtual determinize",
 INFO("virt$ual det$erminize|vdet$erminize",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually determinizes a network",
 "'virtual determinize' replaces the top network on the stack\n\
by its virtual deterministic counterpart.", 0},

{ "virtual intersect",
 INFO("virt$ual inters$ect|vinters$ect",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually intersect the languages of the two top networks on the stack",
 "'virtual intersect' replaces the two top networks on the stack\n\
by a network that contains all the strings they have in common.", 0},

{ "virtual lower",
 INFO("virt$ual low$er|vlow$er",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually extracs the lower language of a transducer",
 "'virtual lower' replaces the top network on the stack\n\
by a virtual network representing the lower language, if\n\
the original net is a transducer; otherwise, by its virtual copy.", 0},

{ "virtual minus",
 INFO("virt$ual min$us|vmin$us",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually subtracts from the top network the language of the second net",
 "'virtual minus' replaces the two top networks on the stack\n\
by a virtual network that contains all the strings of the top\n\
network that are not in the language of the second network.", 0},

{ "virtual negate",
 INFO("virt$ual neg$ate|vneg$ate",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually complements the network",
 "'virtual negate' replaces the top network on the stack\n\
by a virtual network representing the complement language.", 0},

{ "virtual one-plus",
 INFO("virt$ual one-p$lus|vone$-plus",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually concatenates the network with itself one or more times",
 "'virtual one-plus' replaces the top network on the stack\n\
by a virtual network representing one or more concatenations\n\
of the original network with itself.", 0},

{ "virtual option",
 INFO("virt$ual opt$ion|vopt$ion",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually unions the network with an epsilon net",
 "'virtual option' replaces the top network on the stack\n\
by a virtual network representing the union of the original network\n\
with a network for the empty-string language.", 0},

{ "virtual priority-union",
 INFO("virt$ual pr$iority-union|vpr$iority",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually constructs the priority-union of two relations",
 "'virtual priority-union' replaces the two top networks on the stack\n\
by a virtual network representing their priority-union.\n\
The result contains all pairs of strings of the first network\n\
and any pair of strings x:y from the second network in which\n\
x does not belong to the upper language of the first network.", 0},

{ "virtual union",
 INFO("virt$ual uni$on|vunion",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually unions two networks",
 "'virtual union' replaces the two top networks on the stack\n\
by a virtual network representing their union.", 0},

{ "virtual upper",
 INFO("virt$ual up$per|vup$per",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually extracs the upper language of a transducer",
 "'virtual upper' replaces the top network on the stack\n\
by a virtual network representing the upper language, if\n\
the original net is a transducer; otherwise, by its virtual copy.", 0},

{ "virtual zero-plus",
 INFO("virt$ual zero-p$lus|vzero$-plus",
"ANY_PARAMETER", VIRT_NET), "",
 "virtually concatenates the network with itself zero or more times",
 "'virtual zero-plus' replaces the top network on the stack\n\
by a virtual network representing zero or more concatenations\n\
of the original network with itself.", 0},

{ "virtual-to-real",
 INFO("virtual-to-real|v2real",
"ANY_PARAMETER", VIRT_NET), "",
 "converts a virtual network to a real one",
 "'virtual-to-real' replaces a virtual network on top the stack\n\
by the corresponding real network. If the argument is not a\n\
virtual network it does nothing.", 0},


#endif /* WORLD */


  /*
    VARIABLES
    */

  { "variable assert", INFO("(var$iable )?as$sert","ON_OR_OFF", VARI),
      "ON|OFF",

      "quit if a test fails and quit-on-fail is ON",

      "When the variable 'assert' and 'quit-on-fail' are both ON,\n\
the execution is terminated if the test result is 0 (NO).\n\
The default is OFF.", &IY_ASSERT},

  { "variable char-encoding", INFO("(var$iable )?char-enc$oding",
				   "CHAR_ENCODING", VARI),
      "UTF-8|ISO-8859-1",

      "set character encoding",

      "\
When the variable 'char-encoding' is set to ISO-8859-1 (default)\n\
the input to xfst is assumed to be in ISO-8859-1 (alias Latin-1) \n\
and the text output produced by xfst is also in ISO-8859-1. If the\n\
variable is set to UTF-8 the input and output text is in Unicode.\n\
The internal representation of the alphabet is the same in both\n\
cases. The supported part of Unicode includes all the codes that\n\
can be represented by two 8-bit bytes. The UTF-8 mode can also be set\n\
by calling fst with the optional -utf8 flag on the command line.\n",
      &IY_CHAR_ENCODING},

#ifndef WORLD  

  { "variable completion", INFO("(var$iable )?com$pletion","ON_OR_OFF", VARI),
      "ON|OFF",

      "command completion and history is available",

      "When ON command completion and history is available.", &IY_COMPLETION},
#endif

{ "variable copyright-owner", INFO("(var$iable )?copyright$-owner",
                                   "FILENAME", VARI),
    "TEXT",
    "copyright owner",
    "When a file is created with the 'save stack' or 'save defined' command, the\n\
current date and the copyright owner are recorded in the beginning of the\n\
file in text format. The default copyright-owner is \"the Xerox Corporation\".\n\
On Solaris, Linux, and MacOS X platforms, the default can be changed by\n\
setting the environment variable COPYRIGHT_OWNER (N.B. _) to the desired\n\
value. On all systems, the value of 'copyright-owner' can be changed with\n\
the 'set' command to any string that fits on one input line. In order to\n\
be legible in both ISO-8859-1 and UTF-8 modes, the string should not contain\n\
any accented characters.",
&NOT_BINARY
}
,

#ifndef WORLD
  { "variable count-patterns",

     INFO("(var$iable )?count-pat$terns", "ON_OR_OFF", VARI),
      "ON|OFF",

      "count all pattern matches",

      "When ON, 'apply patterns up' and 'apply patterns down' commands count\n\
and report the number of matches for each pattern. Default is OFF.\n\
See also 'delete-patterns', 'extract-patterns', and 'mark-patterns'.\n",
&IY_COUNT_PATTERNS},

  { "variable delete-patterns",

     INFO("(var$iable )?delete-pat$terns", "ON_OR_OFF", VARI),
      "ON|OFF",

      "delete anything that matches a pattern",

      "When ON, 'apply patterns up' and 'apply patterns down' commands\n\
delete all the text that matches some pattern. Default is OFF.\n\
See also 'count-patterns', 'extract-patterns', and mark-patterns'.\n",
&IY_DELETE_PATTERNS},

#endif /*WORLD*/

{ "variable directory", INFO("(var$iable )?dir$ectory", "FILENAME", VARI),
      "filename",

      "the working directory",

      "The working directory.", &NOT_BINARY},

#ifndef WORLD

  { "variable extract-patterns",

     INFO("(var$iable )?extract-pat$terns", "ON_OR_OFF", VARI),
      "ON|OFF",

      "delete anything that does not match patterns",

      "When ON, 'apply patterns up' and 'apply patterns down' commands\n\
delete all the text that is not matched by any pattern. Default is OFF.\n\
See also 'count-patterns', 'delete-patterns', and 'mark-patterns'.\n",
&IY_EXTRACT_PATTERNS},

#endif /*WORLD*/

  { "variable flag-is-epsilon", INFO("(var$iable )?flag-is-eps$ilon|compose-flags-as-special","ON_OR_OFF", VARI),
      "ON|OFF",

      "treat flag diacritics as epsilons in composition",

      "When ON, flag diacritics are treated in a special way\n\
composition treats flag diacritics as special kind of\n\
of epsilon symbols. Flag arcs are like epsilon arcs in that they\n\
are not matched against any symbol in the other network but, unlike\n\
true epsilons, they are retained in the resulting network. When OFF\n\
flag diacritics are treated as ordinary symbols.\n\
Default is OFF.", 
      &IY_COMPOSE_FLAG_AS_SPECIAL},

#ifndef WORLD
  { "variable license-type", INFO("(var$iable )?lic$ense-type","LICENSE_TYPE",
				  VARI),
      "XEROX|LICENSED|PUBLIC|NONE",

      "license type for saved networks",

      "\
A flag that defines what type of privilege is required to load networks\n\
created by the application. Valid values are: XEROX, LICENSED, PUBLIC,\n\
and NONE. If the value is NONE, the created files can be loaded by any\n\
fst application. Default is the application's own privilege level.\n",
&IY_LICENSE_TYPE},


  { "variable mark-patterns",

     INFO("(var$iable )?mark-pat$terns", "ON_OR_OFF", VARI),
      "ON|OFF",

      "mark matching strings with HTML tags",

      "When ON, 'apply patterns up' and 'apply patterns down' commands\n\
insert HTML tags around strings that match a pattern. Default is ON.\n\
See also 'count-patterns', 'delete-patterns', and 'extract-patterns'.\n",
&IY_MARK_PATTERNS},

  { "variable mark-version", INFO("(var$iable )?mark-v$ersion","ON_OR_OFF",
				  VARI),
      "ON|OFF",

      "record the version number when a network is saved",

      "When ON, the current version of the program is saved on the property\n\
list of the network under the attribute TOOLVERSIONS when the network is\n\
saved into a file. Default is OFF.", &IY_MARK_VERSION},

  /*  This seems to be obsolete.  2002-04-02.  LK
{ "variable max-regex-errors", INFO("(var$iable )?max-r$egex-errors","NRO",

                                    VARI),
      "integer",

      "the maximum number of regex errors",

      "The maximum number of errors for regular expressions.  When it\n\
is exceeded the rest of the input file is discarded.\n\
Default is 0.", 
    &IY_LEX_ERRORS},
*/
{ "variable max-state-visits", INFO("(var$iable )?max-st$ate-visits","NRO",

                                    VARI),
      "integer",

      "maximum number of state visits along a path",

      "The maximum number of visits to the same state when printing.\n\
Print commands such as 'print lower-words' and 'print upper-words' avoid\n\
getting into an infinite loop by keeping track of the states on the\n\
path. If variable 'max-state-visits' is 1 (default), each state can be\n\
entered only once. If the value is higher, any loop is traversed\n\
'max-state-visits'-1 times and the loop exits are marked with two dots.\n\
For example, 'print lower-words' on the network compiled from a b* c\n\
yields ac on the default setting. If 'max-state-visits' is set to 2,\n\
the output is ac and ab..c. Default is 1.", &IY_MAX_STATE_VISITS},
#endif /* WORLD */
  { "variable minimal", INFO("(var$iable )?minima$l","ON_OR_OFF", VARI),
      "ON|OFF",

      "minimize the result of calculus operations",

      "When ON, network operations minimize the result. Minimization\n\
includes epsilon removal and determinization.\n\
 Default is ON.",
      &IY_MINIMIZE_P},

  { "variable name-nets", INFO("(var$iable )?nam$e-nets","ON_OR_OFF", VARI),
      "ON|OFF",

      "use regular expressions as network names",

      "When ON, 'read regex' stores the regular expression on the\n\
property list of the result as the value of NETWORKNAME.\n\
Default is OFF.", &IY_NAME_NETS},

#ifndef WORLD

  { "variable need-separators", INFO("(var$iable )?need-sep$arators","ON_OR_OFF", VARI),
      "ON|OFF",

      "check for separators in 'apply patterns up/down'",

      "When ON, 'apply patterns up/down' requires a white-space or\n\
some other separator on both sides of the the matching string.\n\
A pattern is not accepted if the match in string is preceded\n\
or followed by an alphanumeric symbol. Default is ON." , 
      &IY_NEED_SEPARATORS},

#endif /*WORLD */

  { "variable obey-flags", INFO("(var$iable )?obey$-flags","ON_OR_OFF",
				VARI),
      "ON|OFF",

      "enforce flag diacritics",

      "When ON, the constraints expressed as flag diacritics are\n\
taken into account by 'print [upper|lower]-words' and\n\
'print random-[upper|lower]' commands. When OFF, flag diacritics\n\
are treated as ordinary symbols in listing the contents of a\n\
network. Default is ON.", &IY_OBEY_FLAGS},

  { "variable print-pairs", INFO("(var$iable )?(pr$int)-p$airs","ON_OR_OFF",

                                 VARI),
      "ON|OFF",

      "show both sides of labels in apply",

      "When ON,'apply up' and 'apply down' show both (UPPER and LOWER)\n\
sides of labels. Default is OFF.", &IY_PRINT_PAIRS},

  { "variable print-sigma", INFO("(var$iable )?(pr$int)-sigma","ON_OR_OFF",

                                 VARI),
      "ON|OFF",

      "show the sigma when a network is printed",

      "When ON, the sigma is shown when a network is printed.\n\
 Default is ON.",
      &IY_PRINT_SIGMA},

  { "variable print-space", INFO("(var$iable )?(pr$int)-space","ON_OR_OFF",

                                 VARI),
      "ON|OFF",

      "insert a space between symbols in printing words",

      "When ON, 'print word' and 'print random-word' insert a space\n\
between symbols. Default is OFF.", &IY_PRINT_SPACE},

#ifndef WORLD
  { "variable process-in-order", INFO("(var$iable )?pro$cess-in-order",

                                      "ON_OR_OFF", VARI),
      "ON|OFF",

      "process networks in the order they appear",

      "When ON, intersection and composition operations process networks\n\
in the same order as they appear in the stack.\n\
Default is OFF.", 
      &IY_INORDER},

#endif /* WORLD */

  { "variable quit-on-fail", INFO("(var$iable )?quit$-on-fail","ON_OR_OFF",
                                  VARI),
      "ON|OFF",

      "quit scripts abruptly on any error",

      "When ON, the application quits abruptly with an error if it is\n\
running a script and encounters a command that is incorrectly\n\
spelled or cannot be executed because of a missing file\n\
or some other error. Default is ON.", &IY_QUIT_ON_FAIL},

 { "variable quote-special", INFO("(var$iable )?quote-$special","ON_OR_OFF",
				  VARI),
      "ON|OFF",

      "print special characters in double quotes",

      "When ON literal instances of special characters are enclosed in\n\
double quotes: zero and question mark are printed as \"0\" and \"?\"\n\
to distinguish them from epsilon and the unknown symbol. Space, tab,\n\
and newline are printed as \" \", \"\\t\", and \"\\n\", respectively,\n\
to distinguish them from each other. The affected commands are\n\
'print words', 'print lower-words', and 'print upper-words'.\n\
Default is OFF.", &IY_QUOTE_SPECIAL},

  { "variable random-seed", INFO("(var$iable )?ran$dom-seed","NRO", VARI),
      "integer",

      "seed of the random number generator",

      "Seed of the random number generator.  It cannot be inspected.",
      0},

#ifndef WORLD
  {"variable recursive-apply", INFO("(var$iable)?recursive-app$ly",
				    "ON_OR_OFF", VARI),
     "ON|OFF",
     "work depth-first in 'compose-apply'",
     "When ON, compose-apply follows a depth-first strategy in the same\n\
manner as lookup in its depth-first mode.\n\
Default is (temporarily) OFF."
  , &IY_RECURSIVE_APPLY},

#endif /* WORLD */

  { "variable recursive-define", INFO("(var$iable )?recursive-def$ine",

                                      "ON_OR_OFF", VARI),
      "ON|OFF",

      "allow self-reference in definitions",

      "When ON, 'define <sym> <regex>;' treats instances of <sym> in <regex>\n\
as referring to what is being defined. 'define A [a | A b];' yields\n\
the left-recursive language [a b*]. The right-recursive definition\n\
'define B [a B | b];' assigns to B the language [a* b].\n\
'define C [a C b | a b];' makes C equivalent to [a+ b+], a weak\n\
approximation of the context-free language C -> a C b, C -> a b.\n\
Default is OFF.", &IY_RECURSIVE_DEFINE},

  { "variable retokenize", INFO("(var$iable )?retok$enize","ON_OR_OFF",
                                  VARI),
      "ON|OFF",

      "retokenize regular expressions in 'compile-replace'",

      "When ON, the 'compile-replace' command requires token boundaries to be\n\
explicitly present in compiling a regular expression path. When OFF, the\n\
present token boundaries are preserved. Default is ON.", 
      &IY_RETOKENIZE_P},

#ifndef WORLD 
    /*  This seems to be obsolete. 2002-03-25  LK
  { "variable read-left-side", INFO("(var$iable )?read-l$eft-side",

                                    "ON_OR_OFF", VARI),
      "ON|OFF",

      "which side of the alphabet of a transducer is read",

      "Which side of the alphabet of a transducer is read.", 0},
    */
#endif /* WORLD */

#ifndef WORLD

  { "variable seq-final-arcs", INFO("(var$iable )?seq-f$inal-arcs",

                                    "ON_OR_OFF", VARI),
      "ON|OFF",

      "make final strings be arc sequences",

      "Determines whether final strings be states and arcs (ON) or\n\
multi-char labels in sequentialization. Default is OFF.",
      &SQ_FINAL_STRINGS_ARCS },

  { "variable seq-intern-arcs", INFO("(var$iable )?seq-i$ntern-arcs",

                                     "ON_OR_OFF", VARI),
      "ON|OFF",

      "make internal strings be arc sequences",

      "Determines whether internal strings be states and arcs (ON) or\n\
multi-char labels in sequentialization. Default is OFF.", 
      &SQ_INTERN_STRINGS_ARCS },

{ "variable seq-string-onelong", INFO("(var$iable )?seq-s$tring-onelong",

                                     "ON_OR_OFF", VARI),
      "ON|OFF",

      "use single symbols in sequentialization",

      "The \"delayed string\" in sequentialization may be a real string\n\
of several characters (OFF) or just a single symbol (ON). The latter gives\n\
better 'epsilon-pushing' properties at the price of perhaps more states.\n\
Default is OFF.", 
    &SQ_STRING_ONELONG},

#endif /*WORLD*/

  {"variable show-flags", INFO("(var$iable)?show-fl$ags",
				    "ON_OR_OFF", VARI),
     "ON|OFF",
     "show flag diacritics when printing",
     "When ON, print commands show the position of flag diacritics.\n\
If the setting is OFF, flag diacritics (like epsilons) are not shown\n\
Default is OFF."
  , &IY_SHOW_FLAGS},

  { "variable sort-arcs", INFO("(var$iable )?sor$t-arcs","ON_OR_OFF", VARI),
      "ON|OFF",

      "sort the arcs before printing a network",

      "When ON, the arcs of a network are sorted before executing\n\
a 'print net' command. Default is ON.",
      &IY_SORT_ARCS},
      
#ifndef WORLD
      
  { "variable use-mmap", INFO("(var$iable )?use-mm$ap",
				   "ON_OR_OFF", VARI),
      "ON|OFF",

      "map the network file in memory if it is Karttunen compacted",

      "When the variable 'use_mmap' is set to ON (default)\n\
compacted networks when loaded in memory are not copied but mapped on \n\
the disk.\n\
The process will access the arcs and states directly on the disk.\n\
If the variable is set to OFF, the whole network will be copied in memory\n\
as usual when loaded from a binary file.",
      &IY_USE_MMAP},

#endif /*WORLD*/
      
  /* Not ready for prime-time. The default set in struct.c is TRUE

  { "variable unicode", INFO("(var$iable )?uni$code","ON_OR_OFF", VARI),
      "ON|OFF",

      "print two-byte characters with a \\u prefix.",

      "If the value is ON, two-byte characters are printed as four hex\n\
integers preceded by \\u, if the value is OFF we try to display 16-bit\n\
characters directly. Default is ON.",
&IY_UNICODE},
  */

  { "variable verbose", INFO("(var$iable )?ver$bose","ON_OR_OFF", VARI),
      "ON|OFF",

      "print messages",

      "When ON more information is printed. Default is ON.", &IY_VERBOSE},
    /*
      REGEX
      */
  { "regex: braces", INFO("(r$egex: )?brace$s","ANY_PARAMETER", REGE),
      "unary",

      "regular expression [{string}]",

      "[{word}] splits the string ''word'' into a sequence of one\n\
letter symbols: w o r d .", 0},

  { "regex: brackets", INFO("(r$egex: )?bra$ckets","ANY_PARAMETER", REGE),
      "unary",

      "regular expression [ [A] ]",

      "[ [A] ] is equivalent to A.", 0},

  { "regex: compose", INFO("(r$egex: )?com$pose","ANY_PARAMETER",

                               REGE),
      "binary",

      "regular expression [A .o. B]",

      "[A .o. B] denotes the composition of the relations A and B .", 0},

  { "regex: concatenate", INFO("(r$egex: )?con$catenate",

                                   "ANY_PARAMETER", REGE),
      "binary",

      "regular expression [A B]",

      "[A B] denotes the concatenation of all strings in A with all\n\
strings in B.", 0},

  { "regex: contains", INFO("(r$egex: )?con$tains","ANY_PARAMETER", REGE),
      "unary",

      "regular expression [$A]",

      "[$A] is equivalent to [?* A ?*] i.e. it accepts everything that\n\
contains a substring which A accepts.", 0},

  { "regex: crossproduct", INFO("(r$egex: )?cr$ossproduct",

                                    "ANY_PARAMETER", REGE),
      "binary",

      "regular expression [A .x. B]",


      "[A .x. B] denotes the relation that maps every string of A to\n\
every string of B.", 0},

  { "regex: iteration", INFO("(r$egex: )?iter$ation",

                                    "ANY_PARAMETER", REGE),
      "unary",

      "family of regular expressions\n\
\t\t\t\t[A^i]\n\
\t\t\t\t[A^<i]\n\
\t\t\t\t[A^>i]\n\
\t\t\t\t[A^{i,j}]",

"[A^i] denotes a language or a relation consisting i concatenations of A\n\
with itself. ^<i means 'less than i', ^>i 'more than i', ^{i,j}\n\
'from i to j' concatenations.", 0},

  { "regex: follows", INFO("(r$egex: )?follow$s","ANY_PARAMETER",

                            REGE),
      "binary",

      "regular expression [A > B]",

      "[A > B] denotes the infinite language or relation in which any member\n\
of B comes before any member of A. For example [a > b] contains strings\n\
such as \"xyz\", \"back\" and \"dad\" but no strings such as \"cab\".",
 0},
  { "regex: intersect", INFO("(r$egex: )?int$ersect",

                                 "ANY_PARAMETER", REGE),
      "binary",

      "regular expression [A & B]",

      "[A & B] denotes the set of strings that belong to both A and B.", 0},

  { "regex: invert", INFO("(r$egex: )?inv$ert","ANY_PARAMETER",

                              REGE),
      "unary",  
      "regular expression [A.i]",

      "[A.i] denotes the relation that is the inverse of A; that is\n\
the upper and lower languages set of strings that belong to both A and\n\
B.", 0},

  { "regex: load network", INFO("(r$egex: )?load","ANY_PARAMETER",

                              REGE),
      "unary",

      "regular expression [@\"binary file\"]",

      "[@\"binary file\"] loads the topmost network in the file.\n\
Note that @ only retrieves one network whereas the command\n\
'load stack' pushes onto the stack all the networks in the file.\n\
[@bin\"binary file\"] is synonymous with [@\"binary file\"]", 0},

  { "regex: lower-side", INFO("(r$egex: )?l$ower-side",

                                  "ANY_PARAMETER", REGE),
      "unary",

      "regular expression [A.l]",

      "[A.l] denotes the lower language of the relation A, synonymous with\n\
[A.2]. See also 'regex: upper-side'.", 0},

  { "regex: minus", INFO("(r$egex: )?min$us","ANY_PARAMETER", REGE),
      "binary",

      "regular expression [A - B]",

      "[A - B] denotes the set of strings that are in A but not in B.", 0},

  { "regex: negate", INFO("(r$egex: )?n$egate","ANY_PARAMETER",

                              REGE),
      "unary",

      "regular expression [~A]",

      "[~A] denotes the set of all strings that are not in A.", 0},

  { "regex: one-plus", INFO("(r$egex: )?one$-plus","ANY_PARAMETER",

                                REGE),
      "unary",

      "regular expression [A+]",

      "[A+] denotes all the strings that are composed by concatenating\n\
strings of A with each other any number of times.", 0},

  { "regex: optional", INFO("(r$egex: )?opt$ional","ANY_PARAMETER",

                            REGE),
      "unary",

      "regular expression [(A)]",

      "[(A)] is equivalent to [A | []] i.e. it adds the empty string to A.", 0},

  { "regex: operator precedence", INFO("(r$egex: )?op$erator pre$cedence",
                                       "ANY_PARAMETER",
                            REGE),
      "",

      "\t\\ `\n\
\t\t\t\t\t:\n\
\t\t\t\t\t+ * ^ .1 .2 .u .l .i .r\n\
\t\t\t\t\t~ $ $. $?\n\
\t\t\t\t\t/\n\
\t\t\t\t\tconcatenation\n\
\t\t\t\t\t< >\n\
\t\t\t\t\t| & -\n\
\t\t\t\t\t=> -> @->\n\
\t\t\t\t\t<>\n\
\t\t\t\t\t.x. .o.",

      "\\ `\t\t\tTerm complement, Substitution\n\
:\t\t\tHigh-precedence crossproduct\n\
+ * ^ .1 .2 .u .l .i .r\tIteration, Projection, Inverse, Reverse\n\
~ $ $.$?\t\tComplement, Containment\n\
/\t\t\tIgnore\n\
\t\t\tConcatenation\n\
> <\t\t\tFollow, Precede\n\
| & -\t\t\tUnion, Intersect, Minus\n\
<>\t\t\tShuffle\n\
=> -> @->\t\tRestriction, Replacement\n\
.x. .o.\t\t\tLow-precedence crossproduct, Compose",
 0},
  { "regex: precedes", INFO("(r$egex: )?preced$es","ANY_PARAMETER",

                            REGE),
      "binary",

      "regular expression [A < B]",

      "[A < B] denotes the infinite language or relation in which any member\n\
of A comes before any member of B. For example [a < b] contains strings\n\
such as \"xyz\", \"cab\" and \"big\" but no strings such as \"back\".",
 0},
  { "regex: priority-union lower", INFO("(r$egex: )?priority-union lower",
                                      "ANY_PARAMETER",

                               REGE),
      "binary",

      "regular expression [A .p. B]",

      "denotes the relation containing all the pairs <x:y> in A and all the\n\
pairs <z,w> in B where w is not equal to any y on the lower side of A.\n\
This operation cannot be invoked from the command line.",
      0},
{ "regex: priority-union upper", INFO("(r$egex: )?priority-union lupper",
                                      "ANY_PARAMETER",

                               REGE),
      "binary",

      "regular expression [A .P. B]",

      "denotes the relation containing all the pairs <x:y> in A and all the\n\
pairs <z,w> in B where z is not equal to any x on the upper side of A.\n\
This operation cannot be invoked from the command line.",
      0},

  { "regex: read prolog", INFO("(r$egex: )?read prol$og","ANY_PARAMETER",

                               REGE),
      "unary",

      "regular expression [@pl\"text file\"]",

      "@pl\"text file\" denotes the language or relation compiled from the\n\
file. The file must be in the format produced by the 'write prolog'\n\
command.",
      0},

  { "regex: read regex", INFO("(r$egex: )?read reg$ex","ANY_PARAMETER", REGE),
      "unary",

      "regular expression [@re\"text file\"]",

      "@re\"text file\" denotes the language or relation compiled from\n\
the regular expression in the file.", 0},

  { "regex: read spaced-text", INFO("(r$egex: )?read spaced$-text",
                                    "ANY_PARAMETER", REGE),
      "unary",

      "regular expression [@stx\"text file\"]",

      "@stxt\"text file\" denotes the relation compiled from the text\n\
file. The file must be in the format produced by the command\n\
'write spaced-text'.", 0},

  { "regex: read text", INFO("(r$egex: )?read t$ext","ANY_PARAMETER", REGE),
      "unary",

      "regular expression [@txt\"text file\"]",

      "@txt\"text file\" denotes the language compiled from the text file.\n\
The source must be in the format produced by the command 'write text'.", 0},

  { "regex: replace", INFO("(r$egex: )?repl$ace","ANY_PARAMETER",
                               REGE),
      "complex",

      "family of regular expressions\n\
\t\t\t\t[A -> B || X _ Y]\n\
\t\t\t\t[A -> P ... S || L _ R]\n\
\t\t\t\t[A -> B, C -> D]\n\
\t\t\t\t[A @-> B || L _ R] etc.",

      "[A -> B || L _ R] denotes the relation in which any string in A\n\
occurring in the context L _ R is mapped onto every string in B. For\n\
example, [a -> b || c _ d] denotes an infinite relation containing pairs\n\
such as <\"cad\", \"cbb\"> where a replacement occurs, as well as pairs such\n\
as <\"cab\", \"cab\"> and <\"zig\", \"zig\"> that do not contain any\n\
replacement. There are many different types of replace expressions. Consult\n\
section 2.4 of the Beesley and Karttunen book for details.",
      0},
  { "regex: restrict", INFO("(r$egex: )?rest$rict","ANY_PARAMETER",

                               REGE),
      "complex",

      "family of regular expressions [A => B _ C] etc.",

      "[A => B _ C] denotes the language in which any string in A is\n\
immediately preceded by some string in B and immediately followed by\n\
some string in C. For example, [a => b _ c] includes strings such as\n\
\"back-to-back\" and \"front\" but no strings such as \"bat\" or \"sack\".\n\
Consult section 2.4 of the Beesley and Karttunen book for additional\n\
information about restrictions with multiple contexts.",
 0},

  { "regex: reverse", INFO("(r$egex: )?rev$erse","ANY_PARAMETER",

                               REGE),
      "unary",

      "regular expression [A.r]",

      "[A.r] denotes the language or relation that is the reverse of A.", 0},

  { "regex: shuffle", INFO("(r$egex: )?shuffle$","ANY_PARAMETER",

                               REGE),
      "binary",

      "regular expression [A <> B]",

      "[A <> B] denotes the language or relation that results from shuffling\n\
(interdigitating) every member of A with every member of B. For example,\n\
if A includes xy and B includes ab, then [A <> B] includes abxy, axby,\n\
axyb, xaby, xayb, xyab.", 0},

  { "regex: substitute", INFO("(r$egex: )?subs$titute",

                                   "ANY_PARAMETER", REGE),
      "ternary",

      "regular expression [`[[A], S, L]",

      "[`[[A], S, L]] denotes the result of replacing every instance of\n\
symbol S in network A with the list of symbols in L. For example,\n\
`[[a:b], b, c d] is equivalent to [a:c | a:d]. If the list L is empty,\n\
all paths where symbol S occurs are eliminated from the network.", 0},


  { "regex: term complement", INFO("(r$egex: )?ter$m comp$lement",

                                   "ANY_PARAMETER", REGE),
      "unary",

      "regular expression [\\A]",

      "[\\A] is equivalent to [~A & ?] i.e. it denotes all single\n\
symbol strings that do not belong to A.", 0},

  { "regex: union", INFO("(r$egex: )?uni$on","ANY_PARAMETER", REGE),
      "binary",

      "regular expression [A | B]",

      "[A | B] denotes the union of expressions A and B.  All the\n\
strings that belong to either A or B (or both) also belong to the\n\
union.", 0},

  { "regex: upper-side", INFO("(r$egex: )?upper$-side",

                                  "ANY_PARAMETER", REGE),
      "unary",

      "regular expression [A.u]",

      "[A.u] denotes the upper language of the relation A, synonymous with\n\
[A.1]. See also 'regex: lower-side'", 0},

  { "regex: zero-plus", INFO("(r$egex: )?zero%-plus",

                                 "ANY_PARAMETER", REGE),
      "unary",

      "regular expression [A*]",

      "[A*] denotes the set of strings that consists of zero or more\n\
concatenations of strings taken from A. [A*] is like [A+] except that\n\
it always includes the empty string.", 0},

  { 0, 0, 0, 0, 0, 0 }};
