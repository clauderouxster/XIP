/* A Bison parser, made from xerox/fst/regex.template.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse regex_parse
#define yylex regex_lex
#define yyerror regex_error
#define yylval regex_lval
#define yychar regex_char
#define yydebug regex_debug
#define yynerrs regex_nerrs
# define	END	257
# define	SEMIC	258
# define	RERROR	259
# define	EOL	260
# define	EOL_NEXT	261
# define	SEMIC_NEXT	262
# define	BAR_NEXT	263
# define	SYMBOL	264
# define	QUOTED_SYMBOL	265
# define	BREAKSYMB	266
# define	FSYMBOL	267
# define	FSTRING	268
# define	STRING	269
# define	CONSTT	270
# define	PAR1	271
# define	PARS	272
# define	LEFT_P	273
# define	RIGHT_P	274
# define	LEFT_B	275
# define	RIGHT_B	276
# define	SOMETHING_ELSE	277
# define	BINARY	278
# define	NOTHING	279
# define	EQUALS	280
# define	PRIORITY_BOTTOM	281
# define	BOTTOM_PLUS_ONE	282
# define	LEFT_A	283
# define	RIGHT_A	284
# define	OR	285
# define	UNION_UPPER	286
# define	UNION_LOWER	287
# define	AND	288
# define	STAR	289
# define	PLUS	290
# define	COLON	291
# define	UP_LEFT_C	292
# define	RIGHT_C	293
# define	UP	294
# define	UP_LEFT_A	295
# define	UP_RIGHT_A	296
# define	B_QUOTE	297
# define	TILDE	298
# define	SLASH	299
# define	B_SLASH	300
# define	IGNORE_INTERN	301
# define	MINUS	302
# define	MINUS_UPPER	303
# define	MINUS_LOWER	304
# define	X_OP	305
# define	O_OP	306
# define	BIG_O_OP	307
# define	DOUBLE_A	308
# define	ZERO	309
# define	QMARK	310
# define	DOLLAR	311
# define	DOLL_QM	312
# define	DOLL_PER	313
# define	PERIOD	314
# define	COMMA	315
# define	LINE	316
# define	REP_SEP	317
# define	L_ARROW	318
# define	R_ARROW	319
# define	D_ARROW	320
# define	MERGE_LEFT	321
# define	MERGE_RIGHT	322
# define	L_ARROW_OPT	323
# define	R_ARROW_OPT	324
# define	D_ARROW_OPT	325
# define	AT_R_ARROW	326
# define	R_ARROW_AT	327
# define	AT_RIGHT_A	328
# define	RIGHT_A_AT	329
# define	THREE_DOTS	330
# define	AT_L_ARROW	331
# define	L_ARROW_AT	332
# define	AT_LEFT_A	333
# define	LEFT_A_AT	334
# define	AT_R_ARROW_OPT	335
# define	R_ARROW_AT_OPT	336
# define	AT_RIGHT_A_OPT	337
# define	RIGHT_A_AT_OPT	338
# define	AT_L_ARROW_OPT	339
# define	L_ARROW_AT_OPT	340
# define	AT_LEFT_A_OPT	341
# define	LEFT_A_AT_OPT	342
# define	L_DBL_ARROW	343
# define	R_DBL_ARROW	344
# define	D_DBL_ARROW	345
# define	IN_CTX	346
# define	IN_CTX_R	347
# define	IN_CTX_L	348
# define	IN_CTX_O	349
# define	CTX_BOUNDARY	350
# define	EPSBR_LEFT	351
# define	EPSBR_RIGHT	352
# define	BIN_ATFILE	353
# define	REGEX_ATFILE	354
# define	TEXT_ATFILE	355
# define	SPACEDTEXT_ATFILE	356
# define	PROLOG_ATFILE	357
# define	RE2N_VAR_TOK	358
# define	UPPER_SIDE	359
# define	LOWER_SIDE	360
# define	INVERT_FST	361
# define	REVERS_FST	362
# define	CONCAT	363

#line 15 "xerox/fst/regex.template.y"

/* #include <stdio.h> * - in fsm-package.h */
#include <stdlib.h>
#include <stddef.h>

#if defined _WINDOWS
#include <windows.h>
#include <io.h>
#ifndef __GNUC__
#include <direct.h>
#endif
#endif

#if (defined(MAC_OS))
#define alloca	malloc		/* Does not get defined otherwise in the code below. */
#endif

#include "fst/fsm-package.h"

#include "fst/re.lex.h"
#include "fst/lex-aux.h"
#include "fst/re-lex.h"
#include "fst/regex.h"
#include "fst/regex-in.h"

#include "fst/functions.h"
#include "fst/warning.h"
#include "fst/calc-aux.h"
#include "fst/re-aux.h"
#include "fst/merge.h"

#include "fst/fsm-fst-x.h"
#include "fst/utf8_fatstr_util.h"

#define yyparse regex_parse
#define yyerror regex_error
#define yychar regex_char
#define yydebug regex_debug
#define yylval regex_lval
#define yytext regex_text
#define YYPARSE_PARAM fst_cntxt

/* static int yyerror(char *);
 */

static NETptr NET;

#line 62 "xerox/fst/regex.template.y"
#ifndef YYSTYPE
typedef union {
  struct NETWORK	*net;
  struct NET_VECTOR	*net_seq;
  struct ALPHABET	*alph;
  struct REPL_STRUCT	*repl_struct;
  char			*string;
  int			integer;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		202
#define	YYFLAG		-32768
#define	YYNTBASE	110

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 363 ? yytranslate[x] : 146)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     3,     4,     6,     9,    12,    14,    17,    19,
      21,    23,    27,    31,    35,    39,    43,    47,    49,    53,
      57,    61,    65,    69,    73,    77,    81,    85,    87,    89,
      93,    96,    99,   101,   103,   105,   107,   111,   114,   118,
     127,   129,   132,   136,   140,   143,   146,   149,   153,   157,
     161,   168,   171,   174,   177,   180,   183,   186,   189,   192,
     196,   198,   200,   202,   204,   207,   208,   210,   212,   214,
     216,   218,   220,   222,   225,   228,   230,   232,   234,   236,
     238,   240,   242,   246,   248,   252,   256,   258,   262,   266,
     270,   274,   278,   282,   286,   290,   292,   296,   299,   303,
     306,   309,   311,   315,   317,   321,   324,   327,   329,   331,
     333,   335,   337,   339,   341,   343,   345,   347,   349,   351,
     353,   355,   357,   359,   361,   363,   365,   367,   369,   371,
     373,   375,   377,   379,   381,   383,   385,   387,   389,   391
};
static const short yyrhs[] =
{
     111,   112,     0,     0,   113,     0,     1,     5,     0,     1,
       3,     0,     3,     0,   114,     4,     0,     4,     0,   115,
       0,   116,     0,   114,    52,   114,     0,   114,    53,   114,
       0,   116,    29,   116,     0,   116,    30,   116,     0,   116,
      51,   116,     0,   116,    54,   116,     0,   130,     0,   116,
      48,   116,     0,   116,    49,   116,     0,   116,    50,   116,
       0,   116,    31,   116,     0,   116,    32,   116,     0,   116,
      33,   116,     0,   116,    34,   116,     0,   116,    67,   116,
       0,   116,    68,   116,     0,   117,     0,   118,     0,   116,
     144,   138,     0,   118,   119,     0,   117,   119,     0,   119,
       0,   120,     0,   121,     0,   123,     0,    21,   114,    22,
       0,    21,    22,     0,    19,   114,    20,     0,    43,    21,
     114,    61,   125,    61,   124,    22,     0,   128,     0,   104,
     126,     0,   119,    45,   119,     0,   119,    47,   119,     0,
      46,   119,     0,   119,    35,     0,   119,    36,     0,   119,
      40,   122,     0,   119,    41,   122,     0,   119,    42,   122,
       0,   119,    38,   122,    61,   122,    39,     0,    44,   119,
       0,    57,   119,     0,    58,   119,     0,    59,   119,     0,
     119,   105,     0,   119,   106,     0,   119,   107,     0,   119,
     108,     0,   119,    37,   119,     0,   126,     0,    55,     0,
     125,     0,    12,     0,   124,   125,     0,     0,   126,     0,
     127,     0,    55,     0,    56,     0,    96,     0,    10,     0,
      11,     0,   129,   127,     0,   129,   126,     0,    99,     0,
     100,     0,   101,     0,   102,     0,   103,     0,   133,     0,
     131,     0,   131,    63,   132,     0,   132,     0,   133,   145,
     138,     0,   133,    61,   134,     0,   134,     0,   135,   140,
     137,     0,   137,   141,   135,     0,   135,   140,   135,     0,
     135,   140,   136,     0,   136,   140,   137,     0,   137,   141,
     136,     0,   136,   140,   135,     0,   136,   140,   136,     0,
     116,     0,    97,   116,    98,     0,    97,    98,     0,   116,
      76,   116,     0,   116,    76,     0,    76,   116,     0,    76,
       0,   138,    61,   139,     0,   139,     0,   116,    62,   116,
       0,   116,    62,     0,    62,   116,     0,    62,     0,   141,
       0,   142,     0,   143,     0,    64,     0,    77,     0,    78,
       0,    79,     0,    80,     0,    69,     0,    85,     0,    86,
       0,    87,     0,    88,     0,    65,     0,    72,     0,    73,
       0,    74,     0,    75,     0,    70,     0,    81,     0,    82,
       0,    83,     0,    84,     0,    66,     0,    71,     0,    90,
       0,    89,     0,    91,     0,    92,     0,    93,     0,    94,
       0,    95,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   157,   167,   168,   169,   170,   171,   172,   173,   174,
     174,   175,   176,   177,   178,   179,   180,   181,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     197,   198,   201,   202,   202,   202,   203,   204,   205,   206,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     230,   231,   233,   234,   237,   238,   240,   252,   264,   265,
     266,   267,   268,   285,   288,   292,   293,   294,   295,   296,
     301,   302,   305,   306,   308,   312,   313,   315,   316,   317,
     318,   319,   320,   321,   322,   324,   326,   328,   331,   332,
     333,   334,   337,   338,   340,   341,   342,   343,   346,   347,
     348,   351,   352,   353,   354,   355,   356,   357,   358,   359,
     360,   363,   364,   365,   366,   367,   368,   369,   370,   371,
     372,   376,   377,   380,   381,   382,   385,   386,   387,   388
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "END", "SEMIC", "RERROR", "EOL", "EOL_NEXT", 
  "SEMIC_NEXT", "BAR_NEXT", "SYMBOL", "QUOTED_SYMBOL", "BREAKSYMB", 
  "FSYMBOL", "FSTRING", "STRING", "CONSTT", "PAR1", "PARS", "LEFT_P", 
  "RIGHT_P", "LEFT_B", "RIGHT_B", "SOMETHING_ELSE", "BINARY", "NOTHING", 
  "EQUALS", "PRIORITY_BOTTOM", "BOTTOM_PLUS_ONE", "LEFT_A", "RIGHT_A", 
  "OR", "UNION_UPPER", "UNION_LOWER", "AND", "STAR", "PLUS", "COLON", 
  "UP_LEFT_C", "RIGHT_C", "UP", "UP_LEFT_A", "UP_RIGHT_A", "B_QUOTE", 
  "TILDE", "SLASH", "B_SLASH", "IGNORE_INTERN", "MINUS", "MINUS_UPPER", 
  "MINUS_LOWER", "X_OP", "O_OP", "BIG_O_OP", "DOUBLE_A", "ZERO", "QMARK", 
  "DOLLAR", "DOLL_QM", "DOLL_PER", "PERIOD", "COMMA", "LINE", "REP_SEP", 
  "L_ARROW", "R_ARROW", "D_ARROW", "MERGE_LEFT", "MERGE_RIGHT", 
  "L_ARROW_OPT", "R_ARROW_OPT", "D_ARROW_OPT", "AT_R_ARROW", "R_ARROW_AT", 
  "AT_RIGHT_A", "RIGHT_A_AT", "THREE_DOTS", "AT_L_ARROW", "L_ARROW_AT", 
  "AT_LEFT_A", "LEFT_A_AT", "AT_R_ARROW_OPT", "R_ARROW_AT_OPT", 
  "AT_RIGHT_A_OPT", "RIGHT_A_AT_OPT", "AT_L_ARROW_OPT", "L_ARROW_AT_OPT", 
  "AT_LEFT_A_OPT", "LEFT_A_AT_OPT", "L_DBL_ARROW", "R_DBL_ARROW", 
  "D_DBL_ARROW", "IN_CTX", "IN_CTX_R", "IN_CTX_L", "IN_CTX_O", 
  "CTX_BOUNDARY", "EPSBR_LEFT", "EPSBR_RIGHT", "BIN_ATFILE", 
  "REGEX_ATFILE", "TEXT_ATFILE", "SPACEDTEXT_ATFILE", "PROLOG_ATFILE", 
  "RE2N_VAR_TOK", "UPPER_SIDE", "LOWER_SIDE", "INVERT_FST", "REVERS_FST", 
  "CONCAT", "kanto", "kclean", "kanto2", "regex", "re3", "re2r", "re2c", 
  "reconc", "re1lab", "re1", "re1a", "re1b", "int", "symfs", "syms", 
  "sym", "str", "qstr", "atfile", "atfile_type", "replacement", 
  "repl_list", "repl", "uplow_list", "uplow", "re2cN", "re2cE", "insert", 
  "ctx_list", "context", "arrow_type", "left_arrow", "right_arrow", 
  "bidir_arrow", "dblarrow_type", "ctxdir", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,   110,   111,   112,   112,   112,   112,   113,   113,   114,
     114,   115,   115,   115,   115,   115,   115,   115,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     117,   117,   118,   119,   119,   119,   120,   120,   120,   120,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     122,   122,   123,   123,   124,   124,   125,   125,   125,   125,
     125,   126,   127,   128,   128,   129,   129,   129,   129,   129,
     130,   130,   131,   131,   132,   133,   133,   134,   134,   134,
     134,   134,   134,   134,   134,   135,   136,   136,   137,   137,
     137,   137,   138,   138,   139,   139,   139,   139,   140,   140,
     140,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   142,   142,   142,   142,   142,   142,   142,   142,   142,
     142,   143,   143,   144,   144,   144,   145,   145,   145,   145
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     2,     0,     1,     2,     2,     1,     2,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     1,     3,
       2,     2,     1,     1,     1,     1,     3,     2,     3,     8,
       1,     2,     3,     3,     2,     2,     2,     3,     3,     3,
       6,     2,     2,     2,     2,     2,     2,     2,     2,     3,
       1,     1,     1,     1,     2,     0,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     3,     3,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     3,     2,     3,     2,
       2,     1,     3,     1,     3,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       2,     0,     0,     6,     8,    71,    72,    63,     0,     0,
       0,     0,     0,    68,    69,     0,     0,     0,   101,    70,
       0,    75,    76,    77,    78,    79,     0,     1,     3,     0,
       9,    95,    27,    28,    32,    33,    34,    35,    62,    66,
      67,    40,     0,    17,    81,    83,    80,    86,     0,     0,
       0,     5,     4,     0,    37,     0,     0,    51,    44,    52,
      53,    54,   100,    97,     0,    41,     7,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    99,   134,   133,   135,     0,    31,    30,    45,
      46,     0,     0,     0,     0,     0,     0,     0,    55,    56,
      57,    58,    74,    73,     0,     0,   136,   137,   138,   139,
       0,   111,   121,   131,   116,   126,   132,   122,   123,   124,
     125,   112,   113,   114,   115,   127,   128,   129,   130,   117,
     118,   119,   120,     0,   108,   109,   110,     0,     0,    38,
      36,     0,    96,    11,    12,    13,    14,    21,    22,    23,
      24,    18,    19,    20,    15,    16,    25,    26,    98,   107,
       0,    29,   103,    59,    61,     0,    60,    47,    48,    49,
      42,    43,    95,    82,     0,    85,    84,    89,    90,    87,
      93,    94,    91,    95,    88,    92,     0,   106,   105,     0,
       0,     0,   104,   102,     0,    65,    50,     0,    39,    64,
       0,     0,     0
};

static const short yydefgoto[] =
{
     200,     1,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,   165,    37,   197,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,   161,   162,
     133,   134,   135,   136,    86,   110
};

static const short yypact[] =
{
  -32768,    78,     8,-32768,-32768,-32768,-32768,-32768,   202,   140,
       2,   427,   427,-32768,-32768,   427,   427,   427,   427,-32768,
     264,-32768,-32768,-32768,-32768,-32768,    24,-32768,-32768,    -3,
  -32768,   445,   427,   427,   502,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,    22,-32768,   -45,-32768,   -49,-32768,   552,   552,
     260,-32768,-32768,    -5,-32768,   -15,   202,    67,-32768,    67,
      67,    67,   373,-32768,   522,-32768,-32768,   202,   202,   427,
     427,   427,   427,   427,   427,   427,   427,   427,   427,   427,
     427,   427,   427,-32768,-32768,-32768,   315,   502,   502,-32768,
  -32768,   427,    12,    12,    12,    12,   427,   427,-32768,-32768,
  -32768,-32768,-32768,-32768,   202,   202,-32768,-32768,-32768,-32768,
     315,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,   202,-32768,-32768,-32768,   202,   332,-32768,
  -32768,   -44,-32768,-32768,-32768,   373,   373,    -2,    -2,    -2,
      -2,    -2,    -2,    -2,   373,   373,-32768,-32768,   373,   427,
      98,   -22,-32768,    40,-32768,    17,-32768,-32768,-32768,-32768,
     168,   361,   201,-32768,   -49,-32768,   -22,-32768,-32768,-32768,
  -32768,-32768,-32768,   373,-32768,-32768,    20,   373,   427,   315,
      12,    30,   373,-32768,    54,-32768,-32768,    18,-32768,-32768,
      96,   110,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,    27,-32768,   -18,-32768,-32768,     9,
  -32768,-32768,   -89,-32768,-32768,  -170,   -23,    56,-32768,-32768,
  -32768,-32768,     7,    19,    21,   -53,   -20,  -123,    15,   -77,
      79,    77,-32768,-32768,-32768,-32768
};


#define	YYLAST		640


static const short yytable[] =
{
      62,    66,    64,    65,   167,   168,   169,   140,    67,    68,
     179,    51,   105,    52,   182,   139,   191,   186,   104,   102,
      57,    58,     5,    56,    59,    60,    61,   199,     5,     6,
       5,     6,     5,     6,     5,    53,    55,    67,    68,   189,
     198,    87,    88,   106,   107,   108,   109,    67,    68,    67,
      68,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,    80,    81,   164,   160,   166,
     166,   166,   166,    13,    14,    13,    14,-32768,   190,     2,
     177,     3,     4,   141,   180,   184,   172,   172,     5,     6,
       7,   195,   160,   196,   143,   144,   201,     8,   103,     9,
     163,   194,    89,    90,    91,   170,   171,    93,    94,    95,
     202,   173,   193,   178,    19,   172,    19,   181,   185,   172,
     183,    10,    11,   174,    12,   176,   175,   138,   137,    71,
      72,    73,    74,    13,    14,    15,    16,    17,     0,     0,
       0,   187,     0,     0,     0,     0,    75,    76,    77,     0,
       5,     6,     7,     0,    18,     0,     0,     0,     0,     8,
     188,     9,    54,     0,     0,    80,    81,   166,     0,     0,
     192,   160,     0,     0,    19,    20,     0,    21,    22,    23,
      24,    25,    26,    10,    11,     0,    12,    83,    84,    85,
       0,     0,     0,     0,     0,    13,    14,    15,    16,    17,
       0,     0,     0,    89,    90,    91,     0,     0,    93,    94,
      95,     0,     5,     6,     7,    97,    18,     0,     0,     0,
       0,     8,     0,     9,     0,     0,     0,     0,     0,     0,
       0,     0,    71,    72,    73,    74,    19,    20,     0,    21,
      22,    23,    24,    25,    26,    10,    11,     0,    12,    75,
      76,    77,     0,     0,     0,     0,     0,    13,    14,    15,
      16,    17,     0,     0,     0,     0,     0,     0,    80,    81,
       0,     0,     0,     0,     5,     6,     7,    82,    18,     0,
       0,     0,     0,     8,     0,     9,     0,     0,     0,     0,
      83,    84,    85,     0,     0,     0,     0,     0,    19,    20,
       0,    21,    22,    23,    24,    25,    26,    10,    11,     0,
      12,     0,     0,     0,     0,     0,     0,     0,     0,    13,
      14,    15,    16,    17,   111,     5,     6,     7,     0,   114,
       0,     0,     0,     0,     8,     0,     9,   121,   122,   123,
     124,     0,     5,     6,     7,   129,   130,   131,   132,     0,
       0,     8,     0,     9,     0,     0,     0,     0,    10,    11,
      19,    12,    63,    21,    22,    23,    24,    25,    26,     0,
      13,    14,    15,    16,    17,    10,    11,   159,    12,     0,
       0,     0,     0,     0,     0,     0,     0,    13,    14,    15,
      16,    17,     0,     0,     0,     0,    89,    90,    91,     0,
       0,    93,    94,    95,    71,    72,    73,    74,-32768,     0,
       0,    19,     0,     0,    21,    22,    23,    24,    25,    26,
       0,    75,    76,    77,     0,     0,     0,     0,    19,    20,
       0,    21,    22,    23,    24,    25,    26,     5,     6,     7,
      80,    81,     0,     0,     0,     0,     8,     0,     9,   -10,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    83,    84,    85,   -10,     0,   -10,     0,     0,
      10,    11,     0,    12,    69,    70,    71,    72,    73,    74,
       0,     0,    13,    14,    15,    16,    17,     0,     0,     0,
       0,     0,     0,    75,    76,    77,    78,   -10,   -10,    79,
       0,     0,     0,     0,     0,     0,   -10,     0,     0,     0,
       0,     0,    80,    81,     0,     0,     0,     0,     0,     0,
       0,    82,     0,    19,     0,     0,    21,    22,    23,    24,
      25,    26,     0,     0,    83,    84,    85,    89,    90,    91,
      92,     0,    93,    94,    95,     0,     0,    96,     0,    97,
       0,     0,     0,    71,    72,    73,    74,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      75,    76,    77,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    80,
      81,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    98,    99,   100,
     101,    83,    84,    85,     0,     0,   111,   112,   113,     0,
     142,   114,   115,   116,   117,   118,   119,   120,     0,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132
};

static const short yycheck[] =
{
      18,     4,    20,    26,    93,    94,    95,    22,    52,    53,
     133,     3,    61,     5,   137,    20,   186,    61,    63,    42,
      11,    12,    10,    21,    15,    16,    17,   197,    10,    11,
      10,    11,    10,    11,    10,     8,     9,    52,    53,    61,
      22,    32,    33,    92,    93,    94,    95,    52,    53,    52,
      53,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    67,    68,    55,    86,    92,
      93,    94,    95,    55,    56,    55,    56,    37,    61,     1,
     133,     3,     4,    56,   137,   138,   104,   105,    10,    11,
      12,    61,   110,    39,    67,    68,     0,    19,    42,    21,
      91,   190,    35,    36,    37,    96,    97,    40,    41,    42,
       0,   104,   189,   133,    96,   133,    96,   137,   138,   137,
     138,    43,    44,   104,    46,   110,   105,    50,    49,    31,
      32,    33,    34,    55,    56,    57,    58,    59,    -1,    -1,
      -1,   159,    -1,    -1,    -1,    -1,    48,    49,    50,    -1,
      10,    11,    12,    -1,    76,    -1,    -1,    -1,    -1,    19,
      62,    21,    22,    -1,    -1,    67,    68,   190,    -1,    -1,
     188,   189,    -1,    -1,    96,    97,    -1,    99,   100,   101,
     102,   103,   104,    43,    44,    -1,    46,    89,    90,    91,
      -1,    -1,    -1,    -1,    -1,    55,    56,    57,    58,    59,
      -1,    -1,    -1,    35,    36,    37,    -1,    -1,    40,    41,
      42,    -1,    10,    11,    12,    47,    76,    -1,    -1,    -1,
      -1,    19,    -1,    21,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    32,    33,    34,    96,    97,    -1,    99,
     100,   101,   102,   103,   104,    43,    44,    -1,    46,    48,
      49,    50,    -1,    -1,    -1,    -1,    -1,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,
      -1,    -1,    -1,    -1,    10,    11,    12,    76,    76,    -1,
      -1,    -1,    -1,    19,    -1,    21,    -1,    -1,    -1,    -1,
      89,    90,    91,    -1,    -1,    -1,    -1,    -1,    96,    97,
      -1,    99,   100,   101,   102,   103,   104,    43,    44,    -1,
      46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      56,    57,    58,    59,    64,    10,    11,    12,    -1,    69,
      -1,    -1,    -1,    -1,    19,    -1,    21,    77,    78,    79,
      80,    -1,    10,    11,    12,    85,    86,    87,    88,    -1,
      -1,    19,    -1,    21,    -1,    -1,    -1,    -1,    43,    44,
      96,    46,    98,    99,   100,   101,   102,   103,   104,    -1,
      55,    56,    57,    58,    59,    43,    44,    62,    46,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,
      -1,    40,    41,    42,    31,    32,    33,    34,    47,    -1,
      -1,    96,    -1,    -1,    99,   100,   101,   102,   103,   104,
      -1,    48,    49,    50,    -1,    -1,    -1,    -1,    96,    97,
      -1,    99,   100,   101,   102,   103,   104,    10,    11,    12,
      67,    68,    -1,    -1,    -1,    -1,    19,    -1,    21,     4,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    90,    91,    20,    -1,    22,    -1,    -1,
      43,    44,    -1,    46,    29,    30,    31,    32,    33,    34,
      -1,    -1,    55,    56,    57,    58,    59,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    -1,
      -1,    -1,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    76,    -1,    96,    -1,    -1,    99,   100,   101,   102,
     103,   104,    -1,    -1,    89,    90,    91,    35,    36,    37,
      38,    -1,    40,    41,    42,    -1,    -1,    45,    -1,    47,
      -1,    -1,    -1,    31,    32,    33,    34,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   105,   106,   107,
     108,    89,    90,    91,    -1,    -1,    64,    65,    66,    -1,
      98,    69,    70,    71,    72,    73,    74,    75,    -1,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 1:
#line 157 "xerox/fst/regex.template.y"
{
		  if (NET && *fst_IY_NAME_NETS() && regex_file_info && regex_file_info->file.name)
		    {
		      char buf[1000];
		      sprintf(buf,"%s:%d",
			      regex_file_info->file.name, regex_file_info->line_number);
		      add_string_prop(NET, SOURCE_ATTR, FST_thin_to_fat(buf));
		    }
		  return yyvsp[0].integer;
		;
    break;}
case 2:
#line 167 "xerox/fst/regex.template.y"
{ if (NET) free_network(NET); NET = NULL; ;
    break;}
case 3:
#line 168 "xerox/fst/regex.template.y"
{ NET = yyvsp[0].net; check_for_flags(NET); yyval.integer = 1; ;
    break;}
case 4:
#line 169 "xerox/fst/regex.template.y"
{ NET = NULL; yyval.integer = (clean_regex() != END); ;
    break;}
case 5:
#line 170 "xerox/fst/regex.template.y"
{ NET = NULL; stop_regex(); yyval.integer = 0;;
    break;}
case 6:
#line 171 "xerox/fst/regex.template.y"
{ NET = NULL; yyval.integer = 0; ;
    break;}
case 7:
#line 172 "xerox/fst/regex.template.y"
{ yyval.net = ((* fst_IY_MINIMIZE_P()) ? min_fsm(yyvsp[-1].net, DONT_COPY) : yyvsp[-1].net); ;
    break;}
case 8:
#line 173 "xerox/fst/regex.template.y"
{ yyval.net = epsilon_fsm();		;
    break;}
case 11:
#line 175 "xerox/fst/regex.template.y"
{ yyval.net = compose(yyvsp[-2].net, yyvsp[0].net);		;
    break;}
case 12:
#line 176 "xerox/fst/regex.template.y"
{ yyval.net = lenient_compose(yyvsp[-2].net, yyvsp[0].net); ;
    break;}
case 13:
#line 177 "xerox/fst/regex.template.y"
{ yyval.net = precedes(yyvsp[-2].net, yyvsp[0].net); ;
    break;}
case 14:
#line 178 "xerox/fst/regex.template.y"
{ yyval.net = follows(yyvsp[-2].net, yyvsp[0].net); ;
    break;}
case 15:
#line 179 "xerox/fst/regex.template.y"
{ yyval.net = product(yyvsp[-2].net, yyvsp[0].net);		;
    break;}
case 16:
#line 180 "xerox/fst/regex.template.y"
{ yyval.net = shuffle(yyvsp[-2].net, yyvsp[0].net); ;
    break;}
case 18:
#line 183 "xerox/fst/regex.template.y"
{ yyval.net = minus(yyvsp[-2].net, yyvsp[0].net);		;
    break;}
case 19:
#line 184 "xerox/fst/regex.template.y"
{ yyval.net = oneside_minus(yyvsp[-2].net, yyvsp[0].net, UPPER); ;
    break;}
case 20:
#line 185 "xerox/fst/regex.template.y"
{ yyval.net = oneside_minus(yyvsp[-2].net, yyvsp[0].net, LOWER); ;
    break;}
case 21:
#line 186 "xerox/fst/regex.template.y"
{ yyval.net = net_union(yyvsp[-2].net, yyvsp[0].net);	;
    break;}
case 22:
#line 187 "xerox/fst/regex.template.y"
{ yyval.net = priority_union(yyvsp[-2].net, yyvsp[0].net, UPPER); ;
    break;}
case 23:
#line 188 "xerox/fst/regex.template.y"
{ yyval.net = priority_union(yyvsp[-2].net, yyvsp[0].net, LOWER); ;
    break;}
case 24:
#line 189 "xerox/fst/regex.template.y"
{ yyval.net = intersect(yyvsp[-2].net, yyvsp[0].net);	;
    break;}
case 25:
#line 190 "xerox/fst/regex.template.y"
{ yyval.net = merge(yyvsp[-2].net, yyvsp[0].net);	;
    break;}
case 26:
#line 191 "xerox/fst/regex.template.y"
{ yyval.net = merge(yyvsp[0].net, yyvsp[-2].net);	;
    break;}
case 27:
#line 192 "xerox/fst/regex.template.y"
{ yyval.net = ((* fst_IY_MINIMIZE_P()) ? min_fsm(yyvsp[0].net, DONT_COPY) : yyvsp[0].net); ;
    break;}
case 29:
#line 195 "xerox/fst/regex.template.y"
{ yyval.net = REAUX_restrict_in_multctx(yyvsp[-2].net, yyvsp[0].net_seq, yyvsp[-1].integer); ;
    break;}
case 30:
#line 197 "xerox/fst/regex.template.y"
{ yyval.net = concat(yyvsp[-1].net, yyvsp[0].net);		;
    break;}
case 31:
#line 198 "xerox/fst/regex.template.y"
{ yyval.net = concat(yyvsp[-1].net, yyvsp[0].net);		;
    break;}
case 36:
#line 203 "xerox/fst/regex.template.y"
{ yyval.net = ((* fst_IY_MINIMIZE_P()) ? min_fsm(yyvsp[-1].net, DONT_COPY) : yyvsp[-1].net); ;
    break;}
case 37:
#line 204 "xerox/fst/regex.template.y"
{ yyval.net = epsilon_fsm();		;
    break;}
case 38:
#line 205 "xerox/fst/regex.template.y"
{ yyval.net = optional(yyvsp[-1].net);		;
    break;}
case 39:
#line 207 "xerox/fst/regex.template.y"
{ yyval.net = sym_subst(yyvsp[-5].net, yyvsp[-3].integer, yyvsp[-1].alph);	;
    break;}
case 40:
#line 209 "xerox/fst/regex.template.y"
{ yyval.net = yyvsp[0].net;	;
    break;}
case 41:
#line 210 "xerox/fst/regex.template.y"
{ yyval.net = RE2N_var_net(yyvsp[0].string); ;
    break;}
case 42:
#line 211 "xerox/fst/regex.template.y"
{ yyval.net = ignore(yyvsp[-2].net, yyvsp[0].net);		;
    break;}
case 43:
#line 212 "xerox/fst/regex.template.y"
{ yyval.net = ignore_internally(yyvsp[-2].net,yyvsp[0].net);	;
    break;}
case 44:
#line 213 "xerox/fst/regex.template.y"
{ yyval.net = other_than(yyvsp[0].net);		;
    break;}
case 45:
#line 214 "xerox/fst/regex.template.y"
{ yyval.net = kleene_star(yyvsp[-1].net);		;
    break;}
case 46:
#line 215 "xerox/fst/regex.template.y"
{ yyval.net = kleene_plus(yyvsp[-1].net);		;
    break;}
case 47:
#line 216 "xerox/fst/regex.template.y"
{ yyval.net = exactly(yyvsp[-2].net, yyvsp[0].integer);		;
    break;}
case 48:
#line 217 "xerox/fst/regex.template.y"
{ yyval.net = less_than(yyvsp[-2].net, yyvsp[0].integer); ;
    break;}
case 49:
#line 218 "xerox/fst/regex.template.y"
{ yyval.net = more_than(yyvsp[-2].net, yyvsp[0].integer); ;
    break;}
case 50:
#line 219 "xerox/fst/regex.template.y"
{ yyval.net = between(yyvsp[-5].net, yyvsp[-3].integer, yyvsp[-1].integer); ;
    break;}
case 51:
#line 220 "xerox/fst/regex.template.y"
{ yyval.net = negate(yyvsp[0].net);		;
    break;}
case 52:
#line 221 "xerox/fst/regex.template.y"
{ yyval.net = contains(yyvsp[0].net);		;
    break;}
case 53:
#line 222 "xerox/fst/regex.template.y"
{ yyval.net = at_most_one(yyvsp[0].net);	;
    break;}
case 54:
#line 223 "xerox/fst/regex.template.y"
{ yyval.net = exactly_one(yyvsp[0].net);	;
    break;}
case 55:
#line 224 "xerox/fst/regex.template.y"
{ yyval.net = upper_fsm(yyvsp[-1].net,DONT_COPY,DO_MINIMIZE); ;
    break;}
case 56:
#line 225 "xerox/fst/regex.template.y"
{ yyval.net = lower_fsm(yyvsp[-1].net,DONT_COPY,DO_MINIMIZE); ;
    break;}
case 57:
#line 226 "xerox/fst/regex.template.y"
{ yyval.net = invert_fst(yyvsp[-1].net,DONT_COPY); ;
    break;}
case 58:
#line 227 "xerox/fst/regex.template.y"
{ yyval.net = reverse_fsm(yyvsp[-1].net,DONT_COPY,DO_MINIMIZE); ;
    break;}
case 59:
#line 228 "xerox/fst/regex.template.y"
{ yyval.net = product(yyvsp[-2].net, yyvsp[0].net); ;
    break;}
case 60:
#line 230 "xerox/fst/regex.template.y"
{ yyval.integer = nat_number(yyvsp[0].string);			;
    break;}
case 61:
#line 231 "xerox/fst/regex.template.y"
{ yyval.integer = 0;				;
    break;}
case 62:
#line 233 "xerox/fst/regex.template.y"
{ yyval.net = symbol_to_fsm(yyvsp[0].integer); ;
    break;}
case 63:
#line 234 "xerox/fst/regex.template.y"
{ yyval.net = FST_string_to_fsm(last_lex_string(),TRUE); ;
    break;}
case 64:
#line 237 "xerox/fst/regex.template.y"
{ yyval.alph = yyvsp[-1].alph; new_sym(yyvsp[-1].alph, yyvsp[0].integer);		;
    break;}
case 65:
#line 238 "xerox/fst/regex.template.y"
{ yyval.alph = new_alph;			;
    break;}
case 66:
#line 240 "xerox/fst/regex.template.y"
{ id_type id;
                                  if (intern_literal(&id, yyvsp[0].string)!= NO_ERROR) 
				    {
				      yyerror("C-fsm error: cannot intern symbol \
in function intern_literal()");
				      yyval.integer= ID_NO_SYMBOL;
                                    }
                                   else 
				    {
				      yyval.integer=id;
                                    }			
                   	        ;
    break;}
case 67:
#line 252 "xerox/fst/regex.template.y"
{ id_type id;
                                  if (intern_unicode(&id, yyvsp[0].string) != NO_ERROR) 
				    {
				      yyerror("C-fsm error: cannot intern symbol \
in function intern_unicode()");
				      yyval.integer= ID_NO_SYMBOL;
                                    }
                                   else 
				    {
				      yyval.integer=id;
                                    }				
	                        ;
    break;}
case 68:
#line 264 "xerox/fst/regex.template.y"
{ yyval.integer = EPSILON;				;
    break;}
case 69:
#line 265 "xerox/fst/regex.template.y"
{ yyval.integer = OTHER;				;
    break;}
case 70:
#line 266 "xerox/fst/regex.template.y"
{ yyval.integer = BOUNDARY_SYMBOL;			;
    break;}
case 71:
#line 267 "xerox/fst/regex.template.y"
{ yyval.string = last_lex_string();		;
    break;}
case 72:
#line 268 "xerox/fst/regex.template.y"
{ yyval.string = last_lex_string();		;
    break;}
case 73:
#line 285 "xerox/fst/regex.template.y"
{  if (yyvsp[-1].integer == 1) {FILE *ff = re_lex_infile(); yyval.net = net_from_atfile(yyvsp[0].string, yyvsp[-1].integer, 1); 
                                              regex_tg_restart(ff, *fst_IY_LAST_ERRORS() ? FALSE : TRUE); }
                                else yyval.net = net_from_atfile(yyvsp[0].string, yyvsp[-1].integer, 1); ;
    break;}
case 74:
#line 288 "xerox/fst/regex.template.y"
{  if (yyvsp[-1].integer == 1) {FILE *ff = re_lex_infile(); yyval.net = net_from_atfile(yyvsp[0].string, yyvsp[-1].integer, 0); 
	                                     regex_tg_restart(ff, *fst_IY_LAST_ERRORS() ? FALSE : TRUE); }
	                        else yyval.net = net_from_atfile(yyvsp[0].string, yyvsp[-1].integer, 0); ;
    break;}
case 75:
#line 292 "xerox/fst/regex.template.y"
{ yyval.integer = 0; ;
    break;}
case 76:
#line 293 "xerox/fst/regex.template.y"
{ yyval.integer = 1; ;
    break;}
case 77:
#line 294 "xerox/fst/regex.template.y"
{ yyval.integer = 2; ;
    break;}
case 78:
#line 295 "xerox/fst/regex.template.y"
{ yyval.integer = 3; ;
    break;}
case 79:
#line 296 "xerox/fst/regex.template.y"
{ yyval.integer = 4; ;
    break;}
case 80:
#line 301 "xerox/fst/regex.template.y"
{ yyval.net = REAUX_uncond_replace(yyvsp[0].repl_struct); ;
    break;}
case 81:
#line 302 "xerox/fst/regex.template.y"
{ yyval.net = REAUX_conditional_replace(yyvsp[0].repl_struct); ;
    break;}
case 82:
#line 305 "xerox/fst/regex.template.y"
{ yyval.repl_struct = REAUX_rs_append(yyvsp[-2].repl_struct, yyvsp[0].repl_struct); ;
    break;}
case 84:
#line 309 "xerox/fst/regex.template.y"
{ yyval.repl_struct = REAUX_crossprod_uplow_leftright(yyvsp[-2].repl_struct,yyvsp[0].net_seq,yyvsp[-1].integer); ;
    break;}
case 85:
#line 312 "xerox/fst/regex.template.y"
{ yyval.repl_struct = REAUX_rs_append(yyvsp[-2].repl_struct, yyvsp[0].repl_struct); ;
    break;}
case 87:
#line 315 "xerox/fst/regex.template.y"
{ yyval.repl_struct= REAUX_rs_upper_lower(yyvsp[-2].net,yyvsp[0].net,yyvsp[-1].integer); ;
    break;}
case 88:
#line 316 "xerox/fst/regex.template.y"
{ yyval.repl_struct= REAUX_rs_upper_lower(yyvsp[-2].net,yyvsp[0].net,yyvsp[-1].integer); ;
    break;}
case 89:
#line 317 "xerox/fst/regex.template.y"
{ yyval.repl_struct= REAUX_rs_upper_lower(yyvsp[-2].net,yyvsp[0].net,yyvsp[-1].integer); ;
    break;}
case 90:
#line 318 "xerox/fst/regex.template.y"
{ yyval.repl_struct= REAUX_rs_upper_lower(yyvsp[-2].net,yyvsp[0].net,yyvsp[-1].integer); ;
    break;}
case 91:
#line 319 "xerox/fst/regex.template.y"
{ yyval.repl_struct= REAUX_rs_upper_lower(yyvsp[-2].net,yyvsp[0].net,yyvsp[-1].integer); ;
    break;}
case 92:
#line 320 "xerox/fst/regex.template.y"
{ yyval.repl_struct= REAUX_rs_upper_lower(yyvsp[-2].net,yyvsp[0].net,yyvsp[-1].integer); ;
    break;}
case 93:
#line 321 "xerox/fst/regex.template.y"
{ yyval.repl_struct= REAUX_rs_upper_lower(yyvsp[-2].net,yyvsp[0].net,yyvsp[-1].integer); ;
    break;}
case 94:
#line 322 "xerox/fst/regex.template.y"
{ yyval.repl_struct= REAUX_rs_upper_lower(yyvsp[-2].net,yyvsp[0].net,yyvsp[-1].integer); ;
    break;}
case 95:
#line 324 "xerox/fst/regex.template.y"
{ yyval.net = REAUX_rs_reset_single_epsilon(yyvsp[0].net); ;
    break;}
case 96:
#line 327 "xerox/fst/regex.template.y"
{ yyval.net= REAUX_rs_set_single_epsilon(yyvsp[-1].net); ;
    break;}
case 97:
#line 329 "xerox/fst/regex.template.y"
{ yyval.net = REAUX_rs_set_single_epsilon(epsilon_fsm()); ;
    break;}
case 98:
#line 331 "xerox/fst/regex.template.y"
{ yyval.net = REAUX_rs_left_right(yyvsp[-2].net, yyvsp[0].net);;
    break;}
case 99:
#line 332 "xerox/fst/regex.template.y"
{ yyval.net = REAUX_rs_left_right(yyvsp[-1].net, NULL);;
    break;}
case 100:
#line 333 "xerox/fst/regex.template.y"
{ yyval.net = REAUX_rs_left_right(NULL, yyvsp[0].net);;
    break;}
case 101:
#line 334 "xerox/fst/regex.template.y"
{ yyval.net = REAUX_rs_left_right(NULL, NULL);;
    break;}
case 102:
#line 337 "xerox/fst/regex.template.y"
{ yyval.net_seq = nv_append(yyvsp[-2].net_seq,yyvsp[0].net_seq); ;
    break;}
case 104:
#line 340 "xerox/fst/regex.template.y"
{ yyval.net_seq = NV_2_nets_to_nv(yyvsp[-2].net,yyvsp[0].net); ;
    break;}
case 105:
#line 341 "xerox/fst/regex.template.y"
{ yyval.net_seq = NV_2_nets_to_nv(yyvsp[-1].net,NULL); ;
    break;}
case 106:
#line 342 "xerox/fst/regex.template.y"
{ yyval.net_seq = NV_2_nets_to_nv(NULL,yyvsp[0].net); ;
    break;}
case 107:
#line 343 "xerox/fst/regex.template.y"
{ yyval.net_seq = NV_2_nets_to_nv(NULL,NULL); ;
    break;}
case 108:
#line 346 "xerox/fst/regex.template.y"
{ yyval.integer = yyvsp[0].integer; ;
    break;}
case 109:
#line 347 "xerox/fst/regex.template.y"
{ yyval.integer = yyvsp[0].integer; ;
    break;}
case 110:
#line 348 "xerox/fst/regex.template.y"
{ yyval.integer = yyvsp[0].integer; ;
    break;}
case 111:
#line 351 "xerox/fst/regex.template.y"
{ yyval.integer = 1; ;
    break;}
case 112:
#line 352 "xerox/fst/regex.template.y"
{ yyval.integer = 7; ;
    break;}
case 113:
#line 353 "xerox/fst/regex.template.y"
{ yyval.integer = 8; ;
    break;}
case 114:
#line 354 "xerox/fst/regex.template.y"
{ yyval.integer = 9; ;
    break;}
case 115:
#line 355 "xerox/fst/regex.template.y"
{ yyval.integer = -1; ;
    break;}
case 116:
#line 356 "xerox/fst/regex.template.y"
{ yyval.integer = 11; ;
    break;}
case 117:
#line 357 "xerox/fst/regex.template.y"
{ yyval.integer = 17; ;
    break;}
case 118:
#line 358 "xerox/fst/regex.template.y"
{ yyval.integer = 18; ;
    break;}
case 119:
#line 359 "xerox/fst/regex.template.y"
{ yyval.integer = 19; ;
    break;}
case 120:
#line 360 "xerox/fst/regex.template.y"
{ yyval.integer = -11; ;
    break;}
case 121:
#line 363 "xerox/fst/regex.template.y"
{ yyval.integer = 0; ;
    break;}
case 122:
#line 364 "xerox/fst/regex.template.y"
{ yyval.integer = 3; ;
    break;}
case 123:
#line 365 "xerox/fst/regex.template.y"
{ yyval.integer = 4; ;
    break;}
case 124:
#line 366 "xerox/fst/regex.template.y"
{ yyval.integer = 5; ;
    break;}
case 125:
#line 367 "xerox/fst/regex.template.y"
{ yyval.integer = 6; ;
    break;}
case 126:
#line 368 "xerox/fst/regex.template.y"
{ yyval.integer = 10; ;
    break;}
case 127:
#line 369 "xerox/fst/regex.template.y"
{ yyval.integer = 13; ;
    break;}
case 128:
#line 370 "xerox/fst/regex.template.y"
{ yyval.integer = 14; ;
    break;}
case 129:
#line 371 "xerox/fst/regex.template.y"
{ yyval.integer = 15; ;
    break;}
case 130:
#line 372 "xerox/fst/regex.template.y"
{ yyval.integer = 16; ;
    break;}
case 131:
#line 376 "xerox/fst/regex.template.y"
{ yyval.integer = 2; ;
    break;}
case 132:
#line 377 "xerox/fst/regex.template.y"
{ yyval.integer = 12; ;
    break;}
case 133:
#line 380 "xerox/fst/regex.template.y"
{ yyval.integer = 0; ;
    break;}
case 134:
#line 381 "xerox/fst/regex.template.y"
{ yyval.integer = 1; ;
    break;}
case 135:
#line 382 "xerox/fst/regex.template.y"
{ yyval.integer = 2; ;
    break;}
case 136:
#line 385 "xerox/fst/regex.template.y"
{ yyval.integer = 0; ;
    break;}
case 137:
#line 386 "xerox/fst/regex.template.y"
{ yyval.integer = 1; ;
    break;}
case 138:
#line 387 "xerox/fst/regex.template.y"
{ yyval.integer = 2; ;
    break;}
case 139:
#line 388 "xerox/fst/regex.template.y"
{ yyval.integer = 3; ;
    break;}
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 391 "xerox/fst/regex.template.y"


/****************************
  ERROR HANDLING for REGEX.y
 ****************************/

int yyerror(char *str)
{

  NET = NULL;
  recover_regex_error();

  if (regex_file_info && regex_file_info->file.name)
      fprintf(stderr,"*** Regex error at %s on line %d in '%s'\n",
	      regex_text, regex_file_info->line_number+1,
              regex_file_info->file.name);
  else
    fprintf(stderr,"*** Regex error (%s) at %s\n",
	    str, regex_text);
  if (++ (*fst_IY_LAST_ERRORS()) > *fst_IY_LEX_ERRORS())
    {
      warn_toomanyerrors(*fst_IY_LAST_ERRORS());
      wait_for_END();
    }
  return(1);
}
/****************************************************************
  AUXILIARY
 ****************************************************************/
NETptr last_regex()
{
  NETptr n;
  n = NET;
  NET = NULL;
  return n;
}
