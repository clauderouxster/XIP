/* A Bison parser, made from xerox/fst/interface.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse interface_parse
#define yylex interface_lex
#define yyerror interface_error
#define yylval interface_lval
#define yychar interface_char
#define yydebug interface_debug
#define yynerrs interface_nerrs
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
# define	ONE_LINE	280
# define	ATOMI_ON	281
# define	ATOMI_OFF	282
# define	ATOMI_YES	283
# define	ATOMI_NO	284
# define	ATOMI_ALL	285
# define	ATOMI_INFO	286
# define	ATOMI_FOR	287
# define	FILE_NAM	288
# define	READ_FILE	289
# define	WRIT_FILE	290
# define	READ_PIPE	291
# define	NUMBER	292
# define	READ_LEXC	293
# define	ALL	294
# define	QMARK_SYM	295
# define	ZERO_SYM	296
# define	COLON_SYM	297
# define	HELP	298
# define	APROPOS	299
# define	QUIT	300
# define	QUIT2	301
# define	SOURCE	302
# define	SYSTEM	303
# define	ECHO_	304
# define	READ_REGEX	305
# define	DEFINE	306
# define	UNDEFINE	307
# define	ALIAS	308
# define	ALIASUBST	309
# define	ALIASTOP	310
# define	APPLY_UP	311
# define	APPLY_DOWN	312
# define	MULTI_CHAR_SIGMA_NET	313
# define	SIGMA_NET	314
# define	SINGLE_CHAR_SIGMA_NET	315
# define	PASTE_NET_LABELS	316
# define	LABEL_NET	317
# define	SUBSTITUTE_DEFINED	318
# define	ADD_PROPERTIES	319
# define	EDIT_PROPERTIES	320
# define	NAME_NET	321
# define	PRINT_NAME	322
# define	READ_PROPERTIES	323
# define	READ_SPACED_TEXT	324
# define	READ_TEXT	325
# define	SUBSTITUTE_SYMBOL	326
# define	SUBSTITUTE_LABEL	327
# define	COMPOSE_NET	328
# define	CONCATENATE_NET	329
# define	INTERSECT_NET	330
# define	SHUFFLE_NET	331
# define	UNION_NET	332
# define	CROSSPRODUCT_NET	333
# define	MINUS_NET	334
# define	TEST_EQUIVALENT	335
# define	TEST_OVERLAP	336
# define	TEST_SUBLANGUAGE	337
# define	FS_REPEAT	338
# define	TABULARIZE_NET	339
# define	COMPACT_SIGMA	340
# define	OPTIMIZE_NET	341
# define	PUSH_DEFINED	342
# define	CLEANUP_NET	343
# define	ELIMINATE_FLAG	344
# define	UNOPTIMIZE_NET	345
# define	INVERT_NET	346
# define	LOWER_SIDE_NET	347
# define	UPPER_SIDE_NET	348
# define	REDUCE_LABELSET	349
# define	UNREDUCE_LABELSET	350
# define	COMPOSE_APPLY_UP	351
# define	COMPOSE_APPLY_DOWN	352
# define	NEGATE_NET	353
# define	ONE_PLUS_NET	354
# define	ZERO_PLUS_NET	355
# define	REVERSE_NET	356
# define	SUBSTRING_NET	357
# define	COMPLETE_NET	358
# define	DETERMINIZE_NET	359
# define	EPSILON_REMOVE_NET	360
# define	MINIMIZE_NET	361
# define	PRUNE_NET	362
# define	SORT_NET	363
# define	AMBIGUITY_NET	364
# define	COMPILE_REPLACE_LOWER	365
# define	COMPILE_REPLACE_UPPER	366
# define	TWOSIDED_FLAG_DIACRITICS	367
# define	COMPACT_NET	368
# define	UNCOMPACT_NET	369
# define	EXTRACT_COMPILE_REPLACE_LOWER	370
# define	EXTRACT_COMPILE_REPLACE_UPPER	371
# define	ADD_PATH	372
# define	ADD_WORD	373
# define	LOCAL_SEQUENTIALIZE_UP	374
# define	LOCAL_SEQUENTIALIZE_DOWN	375
# define	PUSH_EPSILONS	376
# define	REMOVE_EPSILON	377
# define	SEQUENTIALIZE_UP	378
# define	SEQUENTIALIZE_DOWN	379
# define	TEST_SEQ_UP	380
# define	TEST_SEQ_DOWN	381
# define	TB_INSERT	382
# define	TAGSEP_INSERT	383
# define	ENCODE_NONID	384
# define	TAG_PLUSSTRIP	385
# define	TEST_UNAMBIGUOUS_UP	386
# define	TEST_UNAMBIGUOUS_DOWN	387
# define	TEST_PREPROCESS	388
# define	TEST_REDUCESIGMA	389
# define	TEST_UNICODE_PARSING	390
# define	SHARE_ARCS	391
# define	UNSHARE_ARCS	392
# define	PRINT_STORAGE	393
# define	LIST	394
# define	UNLIST	395
# define	PRINT_LISTS	396
# define	FACTORIZE_DOWN	397
# define	FACTORIZE_UP	398
# define	UNFACTORIZE_DOWN	399
# define	UNFACTORIZE_UP	400
# define	FACTORIZE_DOWN_INFINITE_AMBIGUITY	401
# define	FACTORIZE_UP_INFINITE_AMBIGUITY	402
# define	UNFACTORIZE_DOWN_INFINITE_AMBIGUITY	403
# define	UNFACTORIZE_UP_INFINITE_AMBIGUITY	404
# define	FACTORIZE_DOWN_FINITE_AMBIGUITY	405
# define	FACTORIZE_UP_FINITE_AMBIGUITY	406
# define	UNFACTORIZE_DOWN_FINITE_AMBIGUITY	407
# define	UNFACTORIZE_UP_FINITE_AMBIGUITY	408
# define	FACTORIZE_DOWN_SHORT_AMBIGUITY	409
# define	FACTORIZE_UP_SHORT_AMBIGUITY	410
# define	UNFACTORIZE_DOWN_SHORT_AMBIGUITY	411
# define	UNFACTORIZE_UP_SHORT_AMBIGUITY	412
# define	FACTORIZE_DOWN_UNAMBIGUOUS_NET	413
# define	FACTORIZE_UP_UNAMBIGUOUS_NET	414
# define	UNFACTORIZE_DOWN_UNAMBIGUOUS_NET	415
# define	UNFACTORIZE_UP_UNAMBIGUOUS_NET	416
# define	ALIGN_DOWN_FINITE_AMBIGUITY	417
# define	ALIGN_UP_FINITE_AMBIGUITY	418
# define	UNALIGN_FINITE_AMBIGUITY	419
# define	REDUCE_DOWN_INTERMEDIATE_ALPHABET	420
# define	REDUCE_UP_INTERMEDIATE_ALPHABET	421
# define	TEST_DOWN_FAIL_SAFETY	422
# define	TEST_UP_FAIL_SAFETY	423
# define	PRINT_LONGEST_STRING	424
# define	PRINT_LONGEST_STRING_SIZE	425
# define	PRINT_NET	426
# define	PRINT_SIZE	427
# define	PRINT_SIGMA	428
# define	PRINT_LABELS	429
# define	INSPECT_NET	430
# define	PRINT_VCG_NET	431
# define	PRINT_DOT_NET	432
# define	TEST_LOWER_BOUNDED	433
# define	TEST_UPPER_BOUNDED	434
# define	TEST_LOWER_UNIVERSAL	435
# define	TEST_UPPER_UNIVERSAL	436
# define	TEST_NULL	437
# define	TEST_NON_NULL	438
# define	READ_PROLOG	439
# define	WRITE_PROLOG	440
# define	WRITE_TEXT	441
# define	WRITE_SPACED_TEXT	442
# define	READ_WEIGHTED_PROLOG	443
# define	WRITE_WEIGHTED_PROLOG	444
# define	CONTINUE_SCRIPT	445
# define	INTERRUPT_SCRIPT	446
# define	PRINT_NTH_LOWER	447
# define	PRINT_NTH_UPPER	448
# define	PRINT_NUM_LOWER	449
# define	PRINT_NUM_UPPER	450
# define	PRINT_WORDS	451
# define	PRINT_LOWER_WORDS	452
# define	PRINT_UPPER_WORDS	453
# define	PRINT_RANDOM_LOWER	454
# define	PRINT_RANDOM_UPPER	455
# define	PRINT_RANDOM_WORDS	456
# define	PRINT_EQV_LABELS	457
# define	PRINT_STACK	458
# define	PRINT_DEFINED	459
# define	PRINT_ALIASES	460
# define	PRINT_FILE_INFO	461
# define	PRINT_FLAGS	462
# define	PRINT_LABEL_TALLY	463
# define	PRINT_SIGMA_TALLY	464
# define	PRINT_SIGMA_WORD_TALLY	465
# define	WRITE_PROPERTIES	466
# define	SAVE_STACK	467
# define	LOAD_STACK	468
# define	SAVE_DEFINED	469
# define	LOAD_DEFINED	470
# define	CLEAR_STACK	471
# define	TURN_STACK	472
# define	ROTATE_STACK	473
# define	POP_STACK	474
# define	STACK_TOP	475
# define	PRINT_DIRECTORY	476
# define	SET	477
# define	SHOW	478
# define	APPLY_PATTERNS_DOWN	479
# define	APPLY_PATTERNS_UP	480
# define	VARIABLE_COUNT_PATTERNS	481
# define	VARIABLE_DELETE_PATTERNS	482
# define	VARIABLE_EXTRACT_PATTERNS	483
# define	VARIABLE_MARK_PATTERNS	484
# define	VARIABLE_ASSERT	485
# define	VARIABLE_CURRENT_STACK	486
# define	VARIABLE_DIRECTORY	487
# define	VARIABLE_MAX_REGEX_ERRORS	488
# define	VARIABLE_FLAG_IS_EPSILON	489
# define	VARIABLE_PROCESS_IN_ORDER	490
# define	VARIABLE_VERBOSE	491
# define	VARIABLE_COMPLETION	492
# define	VARIABLE_SORT_ARCS	493
# define	VARIABLE_PRINT_SIGMA	494
# define	VARIABLE_PRINT_SPACE	495
# define	VARIABLE_PRINT_PAIRS	496
# define	VARIABLE_READ_LEFT_SIDE	497
# define	VARIABLE_RECURSIVE_DEFINE	498
# define	VARIABLE_NAME_NETS	499
# define	VARIABLE_RANDOM_SEED	500
# define	VARIABLE_COMPRESS_WORDS	501
# define	VARIABLE_UNICODE	502
# define	VARIABLE_MINIMAL	503
# define	VARIABLE_OBEY_FLAGS	504
# define	VARIABLE_QUIT_ON_FAIL	505
# define	VARIABLE_QUOTE_SPECIAL	506
# define	VARIABLE_MARK_VERSION	507
# define	VARIABLE_RETOKENIZE	508
# define	VARIABLE_RECURSIVE_APPLY	509
# define	VARIABLE_SHOW_FLAGS	510
# define	VARIABLE_CHAR_ENCODING	511
# define	ENCODING_UTF_8	512
# define	ENCODING_ISO_8859_1	513
# define	VARIABLE_COPYRIGHT_OWNER	514
# define	VARIABLE_USE_MMAP	515
# define	VARIABLE_SEQ_FINAL_ARCS	516
# define	VARIABLE_SEQ_INTERN_ARCS	517
# define	VARIABLE_SEQ_STRING_ONELONG	518
# define	VARIABLE_NEED_SEPARATORS	519
# define	VARIABLE_MAX_STATE_VISITS	520
# define	REGEX__BRACES	521
# define	REGEX__BRACKETS	522
# define	REGEX__CONTAINS	523
# define	VARIABLE_LICENSE_TYPE	524
# define	LICENSE_TYPE_XEROX	525
# define	LICENSE_TYPE_PUBLIC	526
# define	LICENSE_TYPE_LICENSED	527
# define	LICENSE_TYPE_RESERVED	528
# define	LICENSE_TYPE_NONE	529
# define	REGEX__CONCATENATE	530
# define	REGEX__COMPOSE	531
# define	REGEX__CROSSPRODUCT	532
# define	REGEX__INTERSECT	533
# define	REGEX__INVERT	534
# define	REGEX__LOWER_SIDE	535
# define	REGEX__MINUS	536
# define	REGEX__NEGATE	537
# define	REGEX__ONE_PLUS	538
# define	REGEX__READ_PROLOG	539
# define	REGEX__READ_REGEX	540
# define	REGEX__READ_SPACED_TEXT	541
# define	REGEX__READ_TEXT	542
# define	REGEX__REVERSE	543
# define	REGEX__UNION	544
# define	REGEX__UPPER_SIDE	545
# define	REGEX__ZERO_PLUS	546
# define	REGEX__TERM_COMPLEMENT	547
# define	REGEX__OPTIONAL	548
# define	REGEX__ITERATION	549
# define	REGEX__LOAD_NETWORK	550
# define	REGEX__OPERATOR_PRECEDENCE	551
# define	REGEX__PRECEDES	552
# define	REGEX__FOLLOWS	553
# define	REGEX__PRIORITY_UNION_LOWER	554
# define	REGEX__PRIORITY_UNION_UPPER	555
# define	REGEX__REPLACE	556
# define	REGEX__RESTRICT	557
# define	REGEX__SHUFFLE	558
# define	REGEX__SUBSTITUTE	559
# define	VIRTUAL_CONCATENATE	560
# define	VIRTUAL_INTERSECT	561
# define	VIRTUAL_OPTION	562
# define	VIRTUAL_MINUS	563
# define	VIRTUAL_NEGATE	564
# define	VIRTUAL_UNION	565
# define	VIRTUAL_ONE_PLUS	566
# define	VIRTUAL_ZERO_PLUS	567
# define	VIRTUAL_DETERMINIZE	568
# define	VIRTUAL_EPS_REMOVE	569
# define	VIRTUAL_COPY	570
# define	VIRTUAL_INVERT	571
# define	VIRTUAL_LOWER	572
# define	VIRTUAL_UPPER	573
# define	VIRTUAL_PRIORITY_UNION	574
# define	VIRTUAL_COMPOSE	575
# define	VIRTUAL_TAGS	576
# define	VIRTUAL_NON_TAGS	577
# define	VIRTUAL_TO_REAL	578
# define	VIRTUAL_NETWORK_P	579
# define	FREE_VIRTUAL_NET	580
# define	SYNTAX_ERROR	581

#line 16 "xerox/fst/interface.y"


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
    
#if defined _WINDOWS
#include <windows.h>
#include <io.h>
#ifndef __GNUC__
#include <direct.h>
#endif
#endif

#ifndef _WINDOWS
#include <unistd.h>
#endif

#include <string.h>
#include <ctype.h>
#include <errno.h>
#if (defined(MAC_OS) || defined(__MWERKS__))  /* Needed only if prototypes are obligatory. */
#include "codewarrior.h"
#define alloca	malloc	/* Does not get defined otherwise in the code below. */
#endif /* MAC_OS */
/***** FSM *****/
#include "fst/fsm-package.h"

#ifdef _WIN32
int yyerror(char *str);		/* Need this only if headers are obligatory. */
#endif

/**** local ****/
#include "fst/types.h"
#include "fst/fsm_io.h"
#include "fst/fst-prompt.h"
#include "fst/define.h"
#include "fst/comm-def.h"
#include "fst/iy-stack.h"
#include "fst/iy-fs-ops.h"
#include "fst/iy-define.h"
#include "fst/iy-help.h"
#include "fst/interface-aux.h"
#include "fst/interface.h"
#include "fst/re.lex.h"
#include "fst/interface.lex.h"
#include "fst/regex.h"
#include "fst/warning.h"
#include "fst/prolog.lex.h"
#include "fst/prolog.h"
#include "fst/fat-str.h"
#include "fst/config.h"
#include "fst/header.h"

#include "fst/cfsm.h"
#include "fst/apply.h"
#include "fst/wprolog.lex.h"
#include "fst/wprolog.h"
#ifdef MAC_OS
#include "fst/fsopt/test-compression.h"
#else
#include "fst/fsopt/test-compression.h"
#endif /* MAC_OS */

#include "fst/utf8_fatstr_util.h"

/* #define yyparse	command_parse
#define yyerror	command_error
#define yychar	command_char
#define yydebug	command_debug
#define yylval	command_lval
#define yynerrs	command_nerrs
*/
#define YYPARSE_PARAM fst_cntxt

static FILE *pipe_file;
FST_CNTXTptr fst_cntxt ;
finfo * old_regex_finfo_ptr;

extern struct STRIntPar interface_parameters;  /* Definel in comm-def.c */
extern finfo *interface_file_info; /* Defined in interface.template.lex. */

enum partype { luku, merkkijono, binaarinen };

/****************************************************************
  Local function declarations
 ****************************************************************/
static int yyerror(char *);

static void command_error_detected(struct command *, struct command *,
				   char *junk);
static void show_last_var(int, int *);

static void regex_to_stack(FST_CNTXT *);
static void regex_from_file(ifile *, FST_CNTXT *);

static void call_system(char *);
static void cd(char *);
static void pwd(void);
static void dir(char *);

static char *strip_yytext(void);
#if NOT_USED
static char *store_yytext(void);
#endif /* NOT_USED */
static char *store_text(char *);
static void fputs_interface_text(void);

static FILE *open_w_file(char *);
static void loclose(FILE *);

#define YY_BUF_LEN 1024
static unsigned char yy_buf[2][YY_BUF_LEN];
static int yy_buf_n=0;

static NETptr invert2(NETptr net, int a, int b) { return invert_fst(net,a); }

static ifile file_par;


/* char *FST_PROMPT = "fst"; */

/* #ifdef WORLD */
/* char *FST_PROMPT = "xfst"; */
/* #endif */

#ifdef WORLD
char *FST_PROMPT = "xfst";
#else
char *FST_PROMPT = "fst";
#endif 

/****************************************************************
  Communication between interface and regex to handle lexer buffer
  exchange. 
****************************************************************/  
static int regex_on_command_line = FALSE;
int regex_cl_length_processed = 0;
/* is it "read regex < my.file" ?*/
int regex_file_symbol_length=0; /* What is the length of "< my.file" on the previous line?" */


#line 158 "xerox/fst/interface.y"
#ifndef YYSTYPE
typedef union {
  int integer;
  int *int_p;
  char *string;
  NETptr net;
  NETptr (*fs_func)();
  void (*func)();
  FILE *file;
  ifile *filename;
  int  (*test_func)();
  ALPHABETptr alph;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		559
#define	YYFLAG		-32768
#define	YYNTBASE	328

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 581 ? yytranslate[x] : 391)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const short yytranslate[] =
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
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     5,     7,    10,    13,    15,    17,    18,
      24,    27,    30,    32,    33,    37,    41,    45,    49,    53,
      54,    59,    63,    67,    71,    73,    75,    77,    79,    81,
      84,    86,    88,    90,    92,    94,    96,    98,   100,   101,
     103,   105,   107,   108,   110,   112,   114,   116,   118,   120,
     122,   124,   126,   127,   129,   130,   132,   133,   135,   136,
     138,   140,   142,   144,   146,   148,   150,   153,   155,   157,
     159,   163,   166,   168,   170,   174,   177,   180,   183,   186,
     189,   193,   197,   201,   205,   209,   212,   214,   216,   218,
     220,   222,   224,   226,   229,   231,   233,   235,   237,   239,
     242,   244,   246,   248,   250,   252,   254,   256,   258,   261,
     264,   266,   268,   270,   272,   273,   279,   280,   286,   287,
     293,   295,   297,   299,   301,   303,   305,   307,   309,   311,
     313,   315,   317,   319,   321,   323,   326,   330,   335,   341,
     343,   345,   347,   349,   351,   353,   355,   357,   359,   361,
     363,   365,   367,   369,   371,   373,   375,   377,   379,   381,
     383,   385,   387,   389,   391,   393,   395,   397,   399,   401,
     403,   405,   407,   409,   411,   413,   415,   417,   421,   425,
     428,   431,   435,   439,   442,   445,   449,   453,   455,   457,
     459,   461,   463,   465,   467,   469,   471,   473,   475,   477,
     479,   481,   483,   485,   487,   489,   491,   493,   495,   497,
     499,   501,   503,   505,   507,   511,   514,   517,   521,   525,
     529,   533,   536,   540,   544,   549,   553,   557,   561,   566,
     571,   575,   579,   584,   589,   594,   600,   606,   612,   618,
     624,   629,   635,   640,   646,   651,   656,   659,   664,   668,
     672,   676,   680,   683,   687,   692,   696,   698,   700,   702,
     704,   706,   708,   710,   712,   714,   716,   718,   720,   722,
     724,   726,   728,   730,   732,   734,   736,   738,   740,   742,
     743,   747,   750,   754,   757,   760,   763,   767,   771,   774,
     778,   781,   785,   787,   789,   791,   793,   795,   797,   800,
     803,   806,   809,   812,   815,   818,   821,   824,   827,   830,
     833,   836,   839,   841,   843,   845,   847,   849,   851,   853,
     855,   857,   859,   861,   863,   865,   867,   869,   871,   873,
     875,   877,   879,   881,   883,   885,   887,   889,   891,   893,
     895,   897,   899,   901,   903,   905,   907,   910,   913,   915,
     918,   919,   923,   925,   928,   931,   934,   936,   938,   940,
     942,   945,   947,   949,   951,   953,   956,   959,   962,   964,
     966,   968,   970,   972
};
static const short yyrhs[] =
{
     390,     0,    54,   383,     0,   385,     0,    45,   345,     0,
      52,   380,     0,   191,     0,   192,     0,     0,   140,   329,
     354,   355,   389,     0,    50,   333,     0,    44,   388,     0,
     334,     0,     0,    51,   330,   386,     0,   223,   379,   389,
       0,   224,   378,   389,     0,    48,   340,   390,     0,    49,
     338,   390,     0,     0,   141,   331,   354,   389,     0,   141,
      31,   389,     0,    53,   332,   389,     0,    53,    31,   389,
       0,   373,     0,   358,     0,   369,     0,   368,     0,     1,
       0,   332,   342,     0,   342,     0,    26,     0,    46,     0,
      47,     0,    34,     0,    36,     0,    35,     0,    37,     0,
       0,   340,     0,   337,     0,   338,     0,     0,   336,     0,
       9,     0,    10,     0,    10,     0,    10,     0,    26,     0,
      38,     0,   342,     0,   347,     0,     0,   346,     0,     0,
     346,     0,     0,   346,     0,     0,    27,     0,    28,     0,
      25,     0,    10,     0,    11,     0,    42,     0,    41,     0,
     355,   354,     0,   354,     0,   353,     0,   354,     0,   354,
      43,   354,     0,   357,   356,     0,   356,     0,   353,     0,
      40,   359,   390,     0,   359,   390,     0,   364,   390,     0,
     367,   390,     0,   363,   390,     0,   365,   390,     0,    39,
     339,   390,     0,    71,   339,   390,     0,    70,   339,   390,
       0,   185,   339,   390,     0,   189,   339,   390,     0,    65,
     337,     0,    65,     0,    84,     0,    89,     0,    86,     0,
     104,     0,   105,     0,    66,     0,    90,   342,     0,   106,
       0,    92,     0,    63,     0,    93,     0,   107,     0,    67,
     342,     0,    99,     0,   100,     0,    59,     0,    62,     0,
      87,     0,    95,     0,    61,     0,   108,     0,    69,   337,
       0,    69,   345,     0,    69,     0,   102,     0,    60,     0,
     109,     0,     0,    64,   360,   354,    33,   354,     0,     0,
      72,   361,   355,    33,   354,     0,     0,    73,   362,   357,
      33,   356,     0,   103,     0,    94,     0,   101,     0,   110,
       0,   111,     0,   112,     0,   116,     0,   117,     0,    91,
       0,    96,     0,    85,     0,   123,     0,   120,     0,   121,
       0,   122,     0,   122,   346,     0,   122,   346,   346,     0,
     122,   346,   346,   346,     0,   122,   346,   346,   346,   346,
       0,   124,     0,   125,     0,   137,     0,   126,     0,   127,
       0,   128,     0,   129,     0,   138,     0,   130,     0,   131,
       0,   113,     0,   308,     0,   310,     0,   313,     0,   312,
       0,   314,     0,   315,     0,   316,     0,   317,     0,   318,
       0,   319,     0,   324,     0,    80,     0,    79,     0,   306,
       0,   307,     0,   311,     0,   309,     0,   320,     0,   321,
       0,    81,     0,    82,     0,    83,     0,    74,     0,    75,
       0,    76,     0,    77,     0,    78,     0,   214,   337,   390,
       0,   213,   336,   390,     0,   217,   390,     0,   220,   390,
       0,    88,   342,   390,     0,   221,   346,   390,     0,   218,
     390,     0,   219,   390,     0,   216,   337,   390,     0,   215,
     336,   390,     0,   143,     0,   144,     0,   145,     0,   146,
       0,   147,     0,   148,     0,   149,     0,   150,     0,   151,
       0,   152,     0,   153,     0,   154,     0,   155,     0,   156,
       0,   157,     0,   158,     0,   159,     0,   160,     0,   161,
       0,   162,     0,   163,     0,   164,     0,   165,     0,   166,
       0,   167,     0,   168,     0,   169,     0,   114,   346,   390,
       0,   114,   390,     0,   115,   390,     0,   206,   341,   390,
       0,   205,   341,   390,     0,   142,   341,   390,     0,   203,
     341,   390,     0,   222,   345,     0,   207,   341,   390,     0,
      68,   341,   390,     0,   212,   371,   341,   390,     0,   186,
     341,   390,     0,   187,   341,   390,     0,   188,   341,   390,
       0,   204,   348,   341,   390,     0,   204,    31,   341,   390,
       0,   190,   341,   390,     0,   139,   341,   390,     0,   200,
     350,   341,   390,     0,   201,   350,   341,   390,     0,   202,
     350,   341,   390,     0,   197,   372,   349,   341,   390,     0,
     198,   372,   349,   341,   390,     0,   199,   372,   349,   341,
     390,     0,   193,   372,   349,   341,   390,     0,   194,   372,
     349,   341,   390,     0,   195,   343,   341,   390,     0,   195,
     343,   344,   341,   390,     0,   196,   343,   341,   390,     0,
     196,   343,   344,   341,   390,     0,   370,   371,   341,   390,
       0,    40,   370,   341,   390,     0,   366,   390,     0,   211,
     343,   341,   390,     0,   211,   341,   390,     0,   134,   351,
     390,     0,   135,   351,   390,     0,   136,   343,   390,     0,
     118,   345,     0,   118,   340,   390,     0,   119,   343,   344,
     390,     0,   119,   340,   390,     0,   172,     0,   173,     0,
     174,     0,   175,     0,   208,     0,   209,     0,   176,     0,
     179,     0,   180,     0,   181,     0,   182,     0,   132,     0,
     133,     0,   177,     0,   178,     0,   210,     0,   171,     0,
     170,     0,   184,     0,   183,     0,   372,     0,   346,     0,
     342,     0,     0,    58,   339,   390,     0,    58,   345,     0,
      57,   339,   390,     0,    57,   345,     0,    97,   345,     0,
      98,   345,     0,    97,   339,   390,     0,    98,   339,   390,
       0,   226,   345,     0,   226,   339,   390,     0,   225,   345,
       0,   225,   339,   390,     0,   232,     0,   233,     0,   257,
       0,   260,     0,   261,     0,   270,     0,   232,   347,     0,
     233,   335,     0,   246,   346,     0,   257,   258,     0,   257,
     259,     0,   257,   342,     0,   260,   335,     0,   261,   352,
       0,   270,   271,     0,   270,   274,     0,   270,   273,     0,
     270,   272,     0,   270,   275,     0,   270,    38,     0,   234,
       0,   266,     0,   253,     0,   249,     0,   235,     0,   250,
       0,   245,     0,   242,     0,   240,     0,   241,     0,   244,
       0,   255,     0,   236,     0,   251,     0,   256,     0,   231,
       0,   252,     0,   243,     0,   254,     0,   265,     0,   227,
       0,   228,     0,   229,     0,   230,     0,   239,     0,   262,
       0,   263,     0,   264,     0,   248,     0,   237,     0,   238,
       0,   376,     0,   377,     0,   374,     0,   376,   346,     0,
     377,   352,     0,   375,     0,   382,   390,     0,     0,   382,
     381,   387,     0,   342,     0,   341,   390,     0,   384,   390,
       0,   384,    23,     0,   342,     0,     3,     0,    23,     0,
     390,     0,   340,   390,     0,   387,     0,    23,     0,    38,
       0,   390,     0,    44,   390,     0,    51,   390,     0,    51,
      23,     0,     1,     0,   390,     0,     1,     0,     6,     0,
       7,     0,     8,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   346,   347,   348,   349,   350,   352,   353,   355,   355,
     356,   357,   358,   359,   359,   363,   364,   365,   366,   367,
     367,   368,   369,   370,   371,   372,   373,   374,   375,   380,
     381,   384,   386,   387,   402,   403,   404,   405,   406,   406,
     408,   409,   410,   410,   411,   427,   428,   431,   434,   435,
     436,   437,   437,   438,   438,   439,   439,   441,   441,   443,
     443,   444,   447,   458,   468,   471,   476,   480,   483,   488,
     489,   501,   506,   510,   516,   517,   518,   519,   521,   522,
     524,   526,   528,   530,   535,   542,   543,   544,   545,   546,
     547,   548,   549,   550,   551,   552,   553,   554,   555,   556,
     557,   558,   560,   561,   562,   563,   564,   566,   567,   568,
     570,   571,   572,   573,   574,   574,   580,   580,   586,   586,
     592,   593,   594,   595,   596,   598,   601,   603,   605,   606,
     607,   608,   609,   610,   611,   612,   613,   614,   615,   616,
     617,   618,   619,   620,   621,   622,   623,   624,   625,   627,
     631,   632,   633,   634,   635,   636,   637,   638,   639,   640,
     641,   646,   647,   651,   652,   653,   654,   655,   656,   661,
     662,   663,   665,   666,   667,   668,   669,   672,   673,   674,
     675,   676,   677,   678,   679,   680,   681,   686,   687,   688,
     689,   690,   691,   692,   693,   694,   695,   696,   697,   698,
     699,   700,   701,   702,   703,   704,   705,   706,   707,   708,
     709,   710,   711,   712,   713,   717,   721,   729,   730,   731,
     733,   735,   736,   738,   739,   740,   741,   747,   754,   755,
     758,   759,   762,   764,   765,   766,   767,   768,   771,   772,
     773,   775,   786,   788,   802,   806,   807,   808,   810,   813,
     814,   816,   820,   824,   828,   833,   842,   843,   844,   845,
     846,   847,   848,   849,   850,   851,   852,   854,   855,   856,
     857,   859,   860,   861,   862,   863,   866,   867,   869,   875,
     878,   879,   880,   881,   882,   883,   884,   885,   887,   888,
     889,   890,   895,   896,   897,   898,   900,   901,   905,   906,
     907,   908,   912,   916,   918,   922,   924,   926,   928,   930,
     932,   934,   938,   940,   944,   945,   946,   947,   948,   949,
     950,   951,   952,   953,   954,   955,   956,   957,   958,   959,
     960,   962,   963,   964,   965,   966,   968,   970,   971,   972,
     975,   976,   977,   980,   981,   982,   984,   985,   992,   995,
     996,   996,  1014,  1017,  1018,  1019,  1020,  1022,  1025,  1043,
    1048,  1057,  1059,  1071,  1087,  1088,  1089,  1090,  1094,  1130,
    1131,  1143,  1143,  1144
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
  "ONE_LINE", "ATOMI_ON", "ATOMI_OFF", "ATOMI_YES", "ATOMI_NO", 
  "ATOMI_ALL", "ATOMI_INFO", "ATOMI_FOR", "FILE_NAM", "READ_FILE", 
  "WRIT_FILE", "READ_PIPE", "NUMBER", "READ_LEXC", "ALL", "QMARK_SYM", 
  "ZERO_SYM", "COLON_SYM", "HELP", "APROPOS", "QUIT", "QUIT2", "SOURCE", 
  "SYSTEM", "ECHO_", "READ_REGEX", "DEFINE", "UNDEFINE", "ALIAS", 
  "ALIASUBST", "ALIASTOP", "APPLY_UP", "APPLY_DOWN", 
  "MULTI_CHAR_SIGMA_NET", "SIGMA_NET", "SINGLE_CHAR_SIGMA_NET", 
  "PASTE_NET_LABELS", "LABEL_NET", "SUBSTITUTE_DEFINED", "ADD_PROPERTIES", 
  "EDIT_PROPERTIES", "NAME_NET", "PRINT_NAME", "READ_PROPERTIES", 
  "READ_SPACED_TEXT", "READ_TEXT", "SUBSTITUTE_SYMBOL", 
  "SUBSTITUTE_LABEL", "COMPOSE_NET", "CONCATENATE_NET", "INTERSECT_NET", 
  "SHUFFLE_NET", "UNION_NET", "CROSSPRODUCT_NET", "MINUS_NET", 
  "TEST_EQUIVALENT", "TEST_OVERLAP", "TEST_SUBLANGUAGE", "FS_REPEAT", 
  "TABULARIZE_NET", "COMPACT_SIGMA", "OPTIMIZE_NET", "PUSH_DEFINED", 
  "CLEANUP_NET", "ELIMINATE_FLAG", "UNOPTIMIZE_NET", "INVERT_NET", 
  "LOWER_SIDE_NET", "UPPER_SIDE_NET", "REDUCE_LABELSET", 
  "UNREDUCE_LABELSET", "COMPOSE_APPLY_UP", "COMPOSE_APPLY_DOWN", 
  "NEGATE_NET", "ONE_PLUS_NET", "ZERO_PLUS_NET", "REVERSE_NET", 
  "SUBSTRING_NET", "COMPLETE_NET", "DETERMINIZE_NET", 
  "EPSILON_REMOVE_NET", "MINIMIZE_NET", "PRUNE_NET", "SORT_NET", 
  "AMBIGUITY_NET", "COMPILE_REPLACE_LOWER", "COMPILE_REPLACE_UPPER", 
  "TWOSIDED_FLAG_DIACRITICS", "COMPACT_NET", "UNCOMPACT_NET", 
  "EXTRACT_COMPILE_REPLACE_LOWER", "EXTRACT_COMPILE_REPLACE_UPPER", 
  "ADD_PATH", "ADD_WORD", "LOCAL_SEQUENTIALIZE_UP", 
  "LOCAL_SEQUENTIALIZE_DOWN", "PUSH_EPSILONS", "REMOVE_EPSILON", 
  "SEQUENTIALIZE_UP", "SEQUENTIALIZE_DOWN", "TEST_SEQ_UP", 
  "TEST_SEQ_DOWN", "TB_INSERT", "TAGSEP_INSERT", "ENCODE_NONID", 
  "TAG_PLUSSTRIP", "TEST_UNAMBIGUOUS_UP", "TEST_UNAMBIGUOUS_DOWN", 
  "TEST_PREPROCESS", "TEST_REDUCESIGMA", "TEST_UNICODE_PARSING", 
  "SHARE_ARCS", "UNSHARE_ARCS", "PRINT_STORAGE", "LIST", "UNLIST", 
  "PRINT_LISTS", "FACTORIZE_DOWN", "FACTORIZE_UP", "UNFACTORIZE_DOWN", 
  "UNFACTORIZE_UP", "FACTORIZE_DOWN_INFINITE_AMBIGUITY", 
  "FACTORIZE_UP_INFINITE_AMBIGUITY", 
  "UNFACTORIZE_DOWN_INFINITE_AMBIGUITY", 
  "UNFACTORIZE_UP_INFINITE_AMBIGUITY", "FACTORIZE_DOWN_FINITE_AMBIGUITY", 
  "FACTORIZE_UP_FINITE_AMBIGUITY", "UNFACTORIZE_DOWN_FINITE_AMBIGUITY", 
  "UNFACTORIZE_UP_FINITE_AMBIGUITY", "FACTORIZE_DOWN_SHORT_AMBIGUITY", 
  "FACTORIZE_UP_SHORT_AMBIGUITY", "UNFACTORIZE_DOWN_SHORT_AMBIGUITY", 
  "UNFACTORIZE_UP_SHORT_AMBIGUITY", "FACTORIZE_DOWN_UNAMBIGUOUS_NET", 
  "FACTORIZE_UP_UNAMBIGUOUS_NET", "UNFACTORIZE_DOWN_UNAMBIGUOUS_NET", 
  "UNFACTORIZE_UP_UNAMBIGUOUS_NET", "ALIGN_DOWN_FINITE_AMBIGUITY", 
  "ALIGN_UP_FINITE_AMBIGUITY", "UNALIGN_FINITE_AMBIGUITY", 
  "REDUCE_DOWN_INTERMEDIATE_ALPHABET", "REDUCE_UP_INTERMEDIATE_ALPHABET", 
  "TEST_DOWN_FAIL_SAFETY", "TEST_UP_FAIL_SAFETY", "PRINT_LONGEST_STRING", 
  "PRINT_LONGEST_STRING_SIZE", "PRINT_NET", "PRINT_SIZE", "PRINT_SIGMA", 
  "PRINT_LABELS", "INSPECT_NET", "PRINT_VCG_NET", "PRINT_DOT_NET", 
  "TEST_LOWER_BOUNDED", "TEST_UPPER_BOUNDED", "TEST_LOWER_UNIVERSAL", 
  "TEST_UPPER_UNIVERSAL", "TEST_NULL", "TEST_NON_NULL", "READ_PROLOG", 
  "WRITE_PROLOG", "WRITE_TEXT", "WRITE_SPACED_TEXT", 
  "READ_WEIGHTED_PROLOG", "WRITE_WEIGHTED_PROLOG", "CONTINUE_SCRIPT", 
  "INTERRUPT_SCRIPT", "PRINT_NTH_LOWER", "PRINT_NTH_UPPER", 
  "PRINT_NUM_LOWER", "PRINT_NUM_UPPER", "PRINT_WORDS", 
  "PRINT_LOWER_WORDS", "PRINT_UPPER_WORDS", "PRINT_RANDOM_LOWER", 
  "PRINT_RANDOM_UPPER", "PRINT_RANDOM_WORDS", "PRINT_EQV_LABELS", 
  "PRINT_STACK", "PRINT_DEFINED", "PRINT_ALIASES", "PRINT_FILE_INFO", 
  "PRINT_FLAGS", "PRINT_LABEL_TALLY", "PRINT_SIGMA_TALLY", 
  "PRINT_SIGMA_WORD_TALLY", "WRITE_PROPERTIES", "SAVE_STACK", 
  "LOAD_STACK", "SAVE_DEFINED", "LOAD_DEFINED", "CLEAR_STACK", 
  "TURN_STACK", "ROTATE_STACK", "POP_STACK", "STACK_TOP", 
  "PRINT_DIRECTORY", "SET", "SHOW", "APPLY_PATTERNS_DOWN", 
  "APPLY_PATTERNS_UP", "VARIABLE_COUNT_PATTERNS", 
  "VARIABLE_DELETE_PATTERNS", "VARIABLE_EXTRACT_PATTERNS", 
  "VARIABLE_MARK_PATTERNS", "VARIABLE_ASSERT", "VARIABLE_CURRENT_STACK", 
  "VARIABLE_DIRECTORY", "VARIABLE_MAX_REGEX_ERRORS", 
  "VARIABLE_FLAG_IS_EPSILON", "VARIABLE_PROCESS_IN_ORDER", 
  "VARIABLE_VERBOSE", "VARIABLE_COMPLETION", "VARIABLE_SORT_ARCS", 
  "VARIABLE_PRINT_SIGMA", "VARIABLE_PRINT_SPACE", "VARIABLE_PRINT_PAIRS", 
  "VARIABLE_READ_LEFT_SIDE", "VARIABLE_RECURSIVE_DEFINE", 
  "VARIABLE_NAME_NETS", "VARIABLE_RANDOM_SEED", "VARIABLE_COMPRESS_WORDS", 
  "VARIABLE_UNICODE", "VARIABLE_MINIMAL", "VARIABLE_OBEY_FLAGS", 
  "VARIABLE_QUIT_ON_FAIL", "VARIABLE_QUOTE_SPECIAL", 
  "VARIABLE_MARK_VERSION", "VARIABLE_RETOKENIZE", 
  "VARIABLE_RECURSIVE_APPLY", "VARIABLE_SHOW_FLAGS", 
  "VARIABLE_CHAR_ENCODING", "ENCODING_UTF_8", "ENCODING_ISO_8859_1", 
  "VARIABLE_COPYRIGHT_OWNER", "VARIABLE_USE_MMAP", 
  "VARIABLE_SEQ_FINAL_ARCS", "VARIABLE_SEQ_INTERN_ARCS", 
  "VARIABLE_SEQ_STRING_ONELONG", "VARIABLE_NEED_SEPARATORS", 
  "VARIABLE_MAX_STATE_VISITS", "REGEX__BRACES", "REGEX__BRACKETS", 
  "REGEX__CONTAINS", "VARIABLE_LICENSE_TYPE", "LICENSE_TYPE_XEROX", 
  "LICENSE_TYPE_PUBLIC", "LICENSE_TYPE_LICENSED", "LICENSE_TYPE_RESERVED", 
  "LICENSE_TYPE_NONE", "REGEX__CONCATENATE", "REGEX__COMPOSE", 
  "REGEX__CROSSPRODUCT", "REGEX__INTERSECT", "REGEX__INVERT", 
  "REGEX__LOWER_SIDE", "REGEX__MINUS", "REGEX__NEGATE", "REGEX__ONE_PLUS", 
  "REGEX__READ_PROLOG", "REGEX__READ_REGEX", "REGEX__READ_SPACED_TEXT", 
  "REGEX__READ_TEXT", "REGEX__REVERSE", "REGEX__UNION", 
  "REGEX__UPPER_SIDE", "REGEX__ZERO_PLUS", "REGEX__TERM_COMPLEMENT", 
  "REGEX__OPTIONAL", "REGEX__ITERATION", "REGEX__LOAD_NETWORK", 
  "REGEX__OPERATOR_PRECEDENCE", "REGEX__PRECEDES", "REGEX__FOLLOWS", 
  "REGEX__PRIORITY_UNION_LOWER", "REGEX__PRIORITY_UNION_UPPER", 
  "REGEX__REPLACE", "REGEX__RESTRICT", "REGEX__SHUFFLE", 
  "REGEX__SUBSTITUTE", "VIRTUAL_CONCATENATE", "VIRTUAL_INTERSECT", 
  "VIRTUAL_OPTION", "VIRTUAL_MINUS", "VIRTUAL_NEGATE", "VIRTUAL_UNION", 
  "VIRTUAL_ONE_PLUS", "VIRTUAL_ZERO_PLUS", "VIRTUAL_DETERMINIZE", 
  "VIRTUAL_EPS_REMOVE", "VIRTUAL_COPY", "VIRTUAL_INVERT", "VIRTUAL_LOWER", 
  "VIRTUAL_UPPER", "VIRTUAL_PRIORITY_UNION", "VIRTUAL_COMPOSE", 
  "VIRTUAL_TAGS", "VIRTUAL_NON_TAGS", "VIRTUAL_TO_REAL", 
  "VIRTUAL_NETWORK_P", "FREE_VIRTUAL_NET", "SYNTAX_ERROR", "command", 
  "@1", "@2", "@3", "undef_list", "echo", "quit", "filename", "wf_name", 
  "rf_name", "rp_name", "readfile_opt", "readfile", "wf", "atom", "atom1", 
  "atom2", "one_line", "nro", "stack_nro", "opt_stack_nro", "nro0", 
  "nro15", "nro30", "on_off", "nothing", "sym_id", "sym_list", "lab_id", 
  "lab_list", "fsops", "fs1op", "@4", "@5", "@6", "fs1virtop", "fs2op", 
  "fs2virtop", "test2op", "fsstop", "stacks", "print", "print_net", 
  "get_nro_net", "get_net", "apply", "var_spec_show", "var_spec_set", 
  "var_int", "var_off", "show", "set", "define", "@7", "def1a", "alias", 
  "alias1", "try_alias", "regex", "regex2", "help", "eol", "real_eol", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,   328,   328,   328,   328,   328,   328,   328,   329,   328,
     328,   328,   328,   330,   328,   328,   328,   328,   328,   331,
     328,   328,   328,   328,   328,   328,   328,   328,   328,   332,
     332,   333,   334,   334,   335,   336,   337,   338,   339,   339,
     340,   340,   341,   341,   341,   342,   343,   344,   345,   346,
     347,   348,   348,   349,   349,   350,   350,   351,   351,   352,
     352,   353,   354,   354,   354,   354,   355,   355,   355,   356,
     356,   357,   357,   357,   358,   358,   358,   358,   358,   358,
     358,   358,   358,   358,   358,   359,   359,   359,   359,   359,
     359,   359,   359,   359,   359,   359,   359,   359,   359,   359,
     359,   359,   359,   359,   359,   359,   359,   359,   359,   359,
     359,   359,   359,   359,   360,   359,   361,   359,   362,   359,
     359,   359,   359,   359,   359,   359,   359,   359,   359,   359,
     359,   359,   359,   359,   359,   359,   359,   359,   359,   359,
     359,   359,   359,   359,   359,   359,   359,   359,   359,   359,
     363,   363,   363,   363,   363,   363,   363,   363,   363,   363,
     363,   364,   364,   365,   365,   365,   365,   365,   365,   366,
     366,   366,   367,   367,   367,   367,   367,   368,   368,   368,
     368,   368,   368,   368,   368,   368,   368,   368,   368,   368,
     368,   368,   368,   368,   368,   368,   368,   368,   368,   368,
     368,   368,   368,   368,   368,   368,   368,   368,   368,   368,
     368,   368,   368,   368,   368,   368,   368,   369,   369,   369,
     369,   369,   369,   369,   369,   369,   369,   369,   369,   369,
     369,   369,   369,   369,   369,   369,   369,   369,   369,   369,
     369,   369,   369,   369,   369,   369,   369,   369,   369,   369,
     369,   369,   369,   369,   369,   369,   370,   370,   370,   370,
     370,   370,   370,   370,   370,   370,   370,   370,   370,   370,
     370,   370,   370,   370,   370,   370,   371,   371,   372,   372,
     373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
     373,   373,   374,   374,   374,   374,   374,   374,   375,   375,
     375,   375,   375,   375,   375,   375,   375,   375,   375,   375,
     375,   375,   376,   376,   377,   377,   377,   377,   377,   377,
     377,   377,   377,   377,   377,   377,   377,   377,   377,   377,
     377,   377,   377,   377,   377,   377,   377,   377,   377,   377,
     377,   377,   377,   378,   378,   378,   379,   379,   379,   380,
     381,   380,   382,   383,   383,   383,   384,   385,   385,   386,
     386,   386,   387,   387,   388,   388,   388,   388,   388,   389,
     389,   390,   390,   390
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     2,     1,     2,     2,     1,     1,     0,     5,
       2,     2,     1,     0,     3,     3,     3,     3,     3,     0,
       4,     3,     3,     3,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     1,
       1,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     0,     1,     0,     1,     0,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       3,     2,     1,     1,     3,     2,     2,     2,     2,     2,
       3,     3,     3,     3,     3,     2,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     1,     1,     1,     0,     5,     0,     5,     0,     5,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     3,     4,     5,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     2,
       2,     3,     3,     2,     2,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     2,     2,     3,     3,     3,
       3,     2,     3,     3,     4,     3,     3,     3,     4,     4,
       3,     3,     4,     4,     4,     5,     5,     5,     5,     5,
       4,     5,     4,     5,     4,     4,     2,     4,     3,     3,
       3,     3,     2,     3,     4,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       3,     2,     3,     2,     2,     2,     3,     3,     2,     3,
       2,     3,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     1,     2,
       0,     3,     1,     2,     2,     2,     1,     1,     1,     1,
       2,     1,     1,     1,     1,     2,     2,     2,     1,     1,
       1,     1,     1,     1
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,    28,   357,   371,   372,   373,   358,    38,     0,     0,
       0,    32,    33,     0,     0,     0,    13,     0,     0,    42,
      38,    38,   102,   112,   106,   103,    96,   114,    86,    92,
       0,    42,   110,    38,    38,   116,   118,   172,   173,   174,
     175,   176,   162,   161,   169,   170,   171,    87,   130,    89,
     104,     0,    88,     0,   128,    95,    97,   121,   105,   129,
      38,    38,   100,   101,   122,   111,   120,    90,    91,    94,
      98,   107,   113,   123,   124,   125,   149,     0,     0,   126,
     127,     0,     0,   132,   133,   134,   131,   139,   140,   142,
     143,   144,   145,   147,   148,   267,   268,    58,    58,     0,
     141,   146,    42,     8,    19,    42,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   273,   272,   256,   257,   258,   259,   262,
     269,   270,   263,   264,   265,   266,   275,   274,    38,    42,
      42,    42,    38,    42,     6,     7,   279,   279,     0,     0,
     279,   279,   279,    56,    56,    56,    42,    52,    42,    42,
      42,   260,   261,   271,    42,   279,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    38,    38,
     163,   164,   150,   166,   151,   165,   153,   152,   154,   155,
     156,   157,   158,   159,   167,   168,   160,    12,    25,     0,
       0,     0,     0,     0,     0,    27,    26,   279,    24,     3,
       1,    36,    37,    40,    41,     0,    39,     0,    42,   368,
       0,     0,    11,   364,    48,     4,     0,     0,    31,    10,
       0,    45,   352,     5,   350,     0,     0,    30,    44,    35,
      43,     0,   356,     2,     0,     0,   283,     0,   281,     0,
      85,    99,     0,   108,   109,     0,     0,     0,     0,     0,
      93,     0,   284,     0,   285,    49,     0,   215,   216,     0,
     252,    46,     0,     0,   135,    57,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     278,    54,    54,    42,    42,    54,    54,    54,    55,    42,
      42,    42,     0,    42,    50,    51,    42,     0,     0,     0,
       0,    42,   277,    42,   276,     0,     0,     0,     0,   179,
     183,   184,   180,     0,   221,   332,   333,   334,   335,   327,
       0,     0,   312,   316,   324,   341,   342,   336,   320,   321,
     319,   329,   322,   318,     0,   340,   315,   317,   325,   328,
     314,   330,   323,   326,     0,     0,     0,   337,   338,   339,
     331,   313,     0,   348,     0,     0,     0,   292,   293,   294,
     295,   296,   297,   345,   343,   344,     0,     0,   290,     0,
     288,    75,    78,    76,    79,   246,    77,    42,    80,    74,
       0,   365,   367,   366,    17,    18,   362,   363,     0,    14,
     361,   359,     0,   349,   370,    23,   369,    29,    22,   353,
     355,   354,   282,   280,    62,    63,    65,    64,     0,   223,
      82,    81,    61,    68,    67,     0,    73,    69,    72,     0,
     181,   286,   287,   214,   253,   255,    47,     0,   136,   249,
     250,   251,   231,     0,    21,     0,   219,    83,   225,   226,
     227,    84,   230,    53,    42,    42,     0,    42,     0,    42,
      42,    42,    42,     0,     0,     0,   220,     0,     0,   218,
     217,   222,   248,     0,     0,   178,   177,   186,   185,   182,
     298,    34,   299,   300,   301,   302,   303,   304,    59,    60,
     305,   311,   306,   309,   308,   307,   310,   346,   347,    15,
      16,   291,   289,     0,   245,   360,   351,     0,     0,    66,
       0,     0,    71,   254,   137,     0,    20,     0,     0,   240,
       0,   242,     0,     0,     0,     0,   232,   233,   234,   229,
     228,   247,   224,   244,   115,   117,    70,   119,   138,     9,
     238,   239,   241,   243,   235,   236,   237,     0,     0,     0
};

static const short yydefgoto[] =
{
     557,   290,   240,   292,   246,   239,   207,   492,   250,   223,
     224,   225,   226,   251,   300,   283,   447,   235,   463,   315,
     316,   464,   309,   286,   500,   433,   437,   435,   438,   439,
     208,   209,   259,   267,   268,   210,   211,   212,   213,   214,
     215,   216,   217,   323,   324,   218,   383,   373,   374,   375,
     386,   376,   243,   412,   244,   253,   254,   219,   409,   410,
     232,   415,   416
};

static const short yypact[] =
{
     497,-32768,-32768,-32768,-32768,-32768,-32768,    -3,   763,    48,
       4,-32768,-32768,    -3,    28,    41,-32768,    60,    27,    17,
     108,   108,-32768,-32768,-32768,-32768,-32768,-32768,    49,-32768,
      60,    15,    12,    -3,    -3,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,    60,-32768,    60,-32768,-32768,-32768,-32768,-32768,-32768,
     108,   108,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,    55,   159,-32768,
  -32768,   108,    29,-32768,-32768,    38,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,    38,    38,    92,
  -32768,-32768,    15,-32768,    81,    15,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    -3,    15,
      15,    15,    -3,    15,-32768,-32768,    60,    60,    92,    92,
      60,    60,    60,    38,    38,    38,    15,    69,    15,    15,
      15,-32768,-32768,-32768,   114,     5,   101,    49,   101,    49,
     159,   159,   159,   159,    38,     4,   498,   747,   108,   108,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   159,
     159,   159,   159,   159,   159,-32768,-32768,     5,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,   159,-32768,   159,    15,-32768,
     159,   121,-32768,-32768,-32768,-32768,   159,   159,-32768,-32768,
      34,-32768,-32768,-32768,   159,   163,   184,-32768,-32768,-32768,
  -32768,   159,-32768,-32768,   153,   159,-32768,   159,-32768,    99,
  -32768,-32768,   159,-32768,-32768,   159,   159,    76,    76,   159,
  -32768,   159,-32768,   159,-32768,-32768,   159,-32768,-32768,   159,
  -32768,-32768,   159,   104,    38,-32768,   159,   159,   159,   159,
      99,   163,    99,   159,   159,   159,   159,   159,   159,   159,
  -32768,    38,    38,   116,   116,    38,    38,    38,-32768,    15,
      15,    15,   159,    15,-32768,-32768,    15,   159,   159,   159,
     159,    15,-32768,    15,-32768,   159,   159,   159,   159,-32768,
  -32768,-32768,-32768,   159,-32768,-32768,-32768,-32768,-32768,-32768,
      60,    82,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,    38,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,    -9,    82,   103,-32768,-32768,-32768,
  -32768,-32768,    35,-32768,    38,   103,   163,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,   163,   159,-32768,   159,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,    15,-32768,-32768,
     159,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   159,-32768,
  -32768,-32768,    -2,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   100,-32768,
  -32768,-32768,-32768,-32768,-32768,    78,-32768,   106,-32768,    80,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,   159,    38,-32768,
  -32768,-32768,-32768,    76,-32768,   163,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,    15,    15,   159,    15,   159,    15,
      15,    15,    15,   159,   159,   159,-32768,   159,   159,-32768,
  -32768,-32768,-32768,   159,   159,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,   159,-32768,-32768,-32768,    99,    99,-32768,
      99,    99,-32768,-32768,    38,    97,-32768,   159,   159,-32768,
     159,-32768,   159,   159,   159,   159,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,   135,   142,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,  -214,  -126,   -21,
     141,   -16,    -7,    52,    -5,   -77,  -157,    -1,   -69,  -172,
  -32768,  -107,    10,    79,  -196,   -82,  -257,  -264,  -436,-32768,
  -32768,   185,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,   189,   -13,    86,-32768,-32768,-32768,    19,    20,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -204,
  -32768,  -223,     0
};


#define	YYLAST		1017


static const short yytable[] =
{
     220,   241,   428,   522,   255,   257,   236,   260,   276,   233,
     434,   263,   242,   247,   252,   241,   284,   265,   266,   256,
     258,   406,   288,   418,   248,   261,   248,   241,   285,   285,
     234,   264,   221,   453,   222,   455,   407,   241,   234,   281,
       3,     4,     5,   275,   271,   273,   269,   221,   270,   229,
     325,   249,   327,   249,     3,     4,     5,   406,   245,   272,
     274,     3,     4,     5,   221,   222,   222,   238,   454,   221,
     241,   222,   407,   501,   279,   282,   275,   277,   278,   241,
     280,   303,   304,   262,   221,   547,   424,   425,   424,   425,
     424,   425,   230,   275,   308,   308,   308,   321,   414,   231,
     313,   432,   281,     3,     4,     5,   322,   424,   425,   424,
     425,   518,   291,   521,   446,   333,   491,   426,   427,   426,
     427,   426,   427,   248,   281,   248,   446,     3,     4,     5,
     498,   499,   294,   517,   234,   558,   298,   249,   426,   427,
     426,   427,   559,   221,   402,   222,   467,   469,   322,   520,
     249,   497,   249,   509,   289,   237,   326,   293,   328,     3,
       4,     5,   314,   510,   414,     3,     4,     5,   490,     3,
       4,     5,   387,   389,   310,   311,   420,   287,   519,   508,
     329,   330,   331,   332,   334,   414,   436,   388,   390,   525,
       3,     4,     5,   227,   241,   465,   434,   228,   470,   471,
     472,   295,   296,   297,   397,   299,   384,   385,   516,   391,
     392,   393,   394,   395,   396,   448,     0,     0,   312,     0,
     317,   318,   319,     0,     0,   398,   320,   399,     0,     0,
     401,   403,   526,   408,     0,     0,   404,   405,     0,     0,
     411,   417,   301,   302,   413,     0,   305,   306,   307,   494,
     495,   419,     0,     0,   421,   422,     0,   423,     0,     0,
     544,   545,   429,   546,     0,   430,   431,     0,   519,   440,
       0,   441,     0,   442,     0,     0,   443,     0,     0,   444,
     400,     0,   445,     0,     0,   493,   449,   450,   451,   452,
       0,     0,     0,   456,   457,   458,   459,   460,   461,   462,
       0,     0,   549,     0,     0,   507,   502,   503,   504,   505,
     506,     0,   476,     0,     0,     0,     0,   479,   480,   481,
     482,     0,     0,     0,     0,   485,   486,   487,   488,     0,
       0,     0,     0,   489,     0,   314,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   466,   468,     0,     0,   496,
       0,   473,   474,   475,     0,   477,     0,     0,   478,     0,
       0,     0,     0,   483,     0,   484,     0,     0,     0,   524,
       0,     0,     0,     0,     0,     0,     0,   511,     0,   512,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     514,     0,     0,     0,     0,     0,     0,     0,   515,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   523,     0,   513,
       0,     0,     0,     0,     0,   548,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   529,     0,   531,     0,
       0,     0,     0,   536,   537,   538,     0,   539,   540,     0,
       0,     0,     0,   541,   542,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     1,     0,
       2,     0,     0,     3,     4,     5,     0,     0,     0,     0,
       0,     0,     0,   543,     0,     0,   527,   528,     0,   530,
       6,   532,   533,   534,   535,     0,     0,   550,   551,     0,
     552,     0,   553,   554,   555,   556,     7,     8,     0,     0,
       0,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,     0,     0,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,     0,   335,   336,   337,   338,   339,
     340,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     350,   351,   352,   353,   354,     0,   355,   356,   357,   358,
     359,   360,   361,   362,   363,   364,     0,     0,   365,   366,
     367,   368,   369,   370,   371,     0,     0,     0,   372,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,     0,
       0,   206,    22,    23,    24,    25,    26,    27,    28,    29,
      30,     0,    32,     0,     0,    35,    36,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    47,    48,    49,
      50,     0,    52,    53,    54,    55,    56,    57,    58,    59,
       0,     0,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,     0,     0,    79,
      80,     0,     0,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,     0,     0,
     100,   101,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   171,   172,   173,   335,   336,   337,   338,   339,   377,
     378,   342,   343,   344,   345,   346,   347,   348,   349,   350,
     351,   352,   353,     0,     0,   355,   356,   357,   358,   359,
     360,   361,   362,   363,   379,     0,     0,   380,   381,   367,
     368,   369,   370,   371,     0,     0,     0,   382
};

static const short yycheck[] =
{
       0,    10,   259,   439,    20,    21,    13,    28,    77,     9,
     267,    32,    17,    18,    19,    10,    85,    33,    34,    20,
      21,    23,    99,   246,     9,    30,     9,    10,    97,    98,
      26,    32,    35,   290,    37,   292,    38,    10,    26,    10,
       6,     7,     8,    38,    60,    61,    51,    35,    53,     1,
     176,    36,   178,    36,     6,     7,     8,    23,    31,    60,
      61,     6,     7,     8,    35,    37,    37,    26,   291,    35,
      10,    37,    38,    38,    81,    82,    38,    77,    78,    10,
      81,   158,   159,    31,    35,   521,    10,    11,    10,    11,
      10,    11,    44,    38,   163,   164,   165,   174,     1,    51,
      31,    25,    10,     6,     7,     8,   175,    10,    11,    10,
      11,    33,    31,    33,    10,   184,    34,    41,    42,    41,
      42,    41,    42,     9,    10,     9,    10,     6,     7,     8,
      27,    28,   148,    33,    26,     0,   152,    36,    41,    42,
      41,    42,     0,    35,    23,    37,   303,   304,   217,    43,
      36,   365,    36,   376,   102,    14,   177,   105,   179,     6,
       7,     8,   167,   386,     1,     6,     7,     8,   340,     6,
       7,     8,   188,   189,   164,   165,    23,    98,   435,   375,
     180,   181,   182,   183,   185,     1,   268,   188,   189,   453,
       6,     7,     8,     8,    10,   302,   453,     8,   305,   306,
     307,   149,   150,   151,   217,   153,   187,   187,   412,   209,
     210,   211,   212,   213,   214,   284,    -1,    -1,   166,    -1,
     168,   169,   170,    -1,    -1,   225,   174,   227,    -1,    -1,
     230,   231,   455,   240,    -1,    -1,   236,   237,    -1,    -1,
     240,   246,   156,   157,   244,    -1,   160,   161,   162,   258,
     259,   251,    -1,    -1,   254,   255,    -1,   257,    -1,    -1,
     517,   518,   262,   520,    -1,   265,   266,    -1,   525,   269,
      -1,   271,    -1,   273,    -1,    -1,   276,    -1,    -1,   279,
     228,    -1,   282,    -1,    -1,   354,   286,   287,   288,   289,
      -1,    -1,    -1,   293,   294,   295,   296,   297,   298,   299,
      -1,    -1,   525,    -1,    -1,   374,   271,   272,   273,   274,
     275,    -1,   312,    -1,    -1,    -1,    -1,   317,   318,   319,
     320,    -1,    -1,    -1,    -1,   325,   326,   327,   328,    -1,
      -1,    -1,    -1,   333,    -1,   340,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   303,   304,    -1,    -1,   364,
      -1,   309,   310,   311,    -1,   313,    -1,    -1,   316,    -1,
      -1,    -1,    -1,   321,    -1,   323,    -1,    -1,    -1,   448,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   387,    -1,   389,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     400,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   408,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   447,    -1,   397,
      -1,    -1,    -1,    -1,    -1,   524,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   466,    -1,   468,    -1,
      -1,    -1,    -1,   473,   474,   475,    -1,   477,   478,    -1,
      -1,    -1,    -1,   483,   484,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,
       3,    -1,    -1,     6,     7,     8,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   513,    -1,    -1,   464,   465,    -1,   467,
      23,   469,   470,   471,   472,    -1,    -1,   527,   528,    -1,
     530,    -1,   532,   533,   534,   535,    39,    40,    -1,    -1,
      -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    -1,    -1,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,    -1,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   245,   246,    -1,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,    -1,    -1,   260,   261,
     262,   263,   264,   265,   266,    -1,    -1,    -1,   270,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,    -1,
      -1,   324,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    -1,    69,    -1,    -1,    72,    73,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,
      87,    -1,    89,    90,    91,    92,    93,    94,    95,    96,
      -1,    -1,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,    -1,    -1,   116,
     117,    -1,    -1,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,    -1,    -1,    -1,
     137,   138,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   208,   209,   210,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,    -1,    -1,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,    -1,    -1,   260,   261,   262,
     263,   264,   265,   266,    -1,    -1,    -1,   270
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
#line 346 "xerox/fst/interface.y"
{ return(EOL); ;
    break;}
case 2:
#line 347 "xerox/fst/interface.y"
{ return(ALIAS); ;
    break;}
case 3:
#line 348 "xerox/fst/interface.y"
{ return(ALIASUBST); ;
    break;}
case 4:
#line 349 "xerox/fst/interface.y"
{ iy_print_apropos(yyvsp[0].string,1); return(APROPOS); ;
    break;}
case 5:
#line 350 "xerox/fst/interface.y"
{ return(DEFINE); ;
    break;}
case 6:
#line 352 "xerox/fst/interface.y"
{ return(CONTINUE_SCRIPT);;
    break;}
case 7:
#line 353 "xerox/fst/interface.y"
{ return(INTERRUPT_SCRIPT);;
    break;}
case 8:
#line 355 "xerox/fst/interface.y"
{wait_for_sym();;
    break;}
case 9:
#line 355 "xerox/fst/interface.y"
{iy_list(yyvsp[-2].integer, yyvsp[-1].alph); return(LIST); ;
    break;}
case 10:
#line 356 "xerox/fst/interface.y"
{ return(ECHO_); ;
    break;}
case 11:
#line 357 "xerox/fst/interface.y"
{ return(HELP); ;
    break;}
case 12:
#line 358 "xerox/fst/interface.y"
{ return(yyvsp[0].integer); ;
    break;}
case 13:
#line 359 "xerox/fst/interface.y"
{
                old_regex_finfo_ptr=read_regex_immediate_action(&regex_on_command_line, fst_cntxt, 0, &regex_file_symbol_length);
              ;
    break;}
case 14:
#line 362 "xerox/fst/interface.y"
{ read_regex_post_action(regex_cl_length_processed, fst_cntxt, old_regex_finfo_ptr, regex_file_info);  return(READ_REGEX); ;
    break;}
case 15:
#line 363 "xerox/fst/interface.y"
{ return(SET); ;
    break;}
case 16:
#line 364 "xerox/fst/interface.y"
{ return(SHOW); ;
    break;}
case 17:
#line 365 "xerox/fst/interface.y"
{ iy_source(yyvsp[-1].filename, fst_cntxt); return(SOURCE); ;
    break;}
case 18:
#line 366 "xerox/fst/interface.y"
{ call_system(yyvsp[-1].string); return(SYSTEM); ;
    break;}
case 19:
#line 367 "xerox/fst/interface.y"
{wait_for_sym();;
    break;}
case 20:
#line 367 "xerox/fst/interface.y"
{ iy_unlist(yyvsp[-1].integer); return UNLIST; ;
    break;}
case 21:
#line 368 "xerox/fst/interface.y"
{ iy_unlist(0); return UNLIST; ;
    break;}
case 22:
#line 369 "xerox/fst/interface.y"
{ return(UNDEFINE); ;
    break;}
case 23:
#line 370 "xerox/fst/interface.y"
{ iy_undef_all(); return(UNDEFINE); ;
    break;}
case 24:
#line 371 "xerox/fst/interface.y"
{ return(APPLY_UP); ;
    break;}
case 25:
#line 372 "xerox/fst/interface.y"
{ return(NEGATE_NET); ;
    break;}
case 26:
#line 373 "xerox/fst/interface.y"
{ return(PRINT_NET); ;
    break;}
case 27:
#line 374 "xerox/fst/interface.y"
{ return(POP_STACK); ;
    break;}
case 28:
#line 375 "xerox/fst/interface.y"
{ command_error_detected(last_command,
						 last_variable,NULL);
			  return(SYNTAX_ERROR); ;
    break;}
case 29:
#line 380 "xerox/fst/interface.y"
{ iy_undefine(yyvsp[0].string); ;
    break;}
case 30:
#line 381 "xerox/fst/interface.y"
{ iy_undefine(yyvsp[0].string); ;
    break;}
case 31:
#line 384 "xerox/fst/interface.y"
{ fputs_interface_text(); ;
    break;}
case 32:
#line 386 "xerox/fst/interface.y"
{ yyval.integer = QUIT; ;
    break;}
case 33:
#line 387 "xerox/fst/interface.y"
{ 
          if (IY_INTERACTIVE)
	    {
	      print_wanna_stop();
	      interface_lex_total_restart();
	      if (feof(stdin))
		clearerr(stdin);

	      if (wait_for_confirm() == ATOMI_YES)
		yyval.integer = QUIT;
	      else
		yyval.integer = EOL;
	    }
	  else yyval.integer = QUIT;
	;
    break;}
case 34:
#line 402 "xerox/fst/interface.y"
{ yyval.string=strip_yytext(); ;
    break;}
case 35:
#line 403 "xerox/fst/interface.y"
{ yyval.string=strip_yytext(); ;
    break;}
case 36:
#line 404 "xerox/fst/interface.y"
{ yyval.string=strip_yytext(); ;
    break;}
case 37:
#line 405 "xerox/fst/interface.y"
{ yyval.string=strip_yytext(); ;
    break;}
case 38:
#line 406 "xerox/fst/interface.y"
{ yyval.filename = NULL; ;
    break;}
case 40:
#line 408 "xerox/fst/interface.y"
{ file_par.name = yyvsp[0].string; file_par.type = FILE_IN; yyval.filename = &file_par; ;
    break;}
case 41:
#line 409 "xerox/fst/interface.y"
{ file_par.name = yyvsp[0].string; file_par.type = PIPE_IN; yyval.filename = &file_par; ;
    break;}
case 42:
#line 410 "xerox/fst/interface.y"
{ yyval.file=stdout;	;
    break;}
case 43:
#line 410 "xerox/fst/interface.y"
{ yyval.file = open_w_file(yyvsp[0].string); ;
    break;}
case 44:
#line 411 "xerox/fst/interface.y"
{ char *p;
			  if (read_bar_command() && ((p = strip_yytext())))
			    {
			      if (!(yyval.file = pipe_file = popen(p,"w")))
				{
				  perror("fst:open_pipe");
				  return(SYNTAX_ERROR);
				}
			    }
			  else
			    {
			      nopipecomm();
			      yyval.file = NULL;
			    }
			;
    break;}
case 45:
#line 427 "xerox/fst/interface.y"
{ yyval.string=store_text(interface_last_lex_string()); ;
    break;}
case 46:
#line 428 "xerox/fst/interface.y"
{ yyval.string=strdup(interface_last_lex_string()); fs_arg1 = yyval.string; ;
    break;}
case 47:
#line 431 "xerox/fst/interface.y"
{ yyval.string=strdup(interface_last_lex_string()); fs_arg2 = yyval.string; ;
    break;}
case 48:
#line 434 "xerox/fst/interface.y"
{ yyval.string=strip_yytext();		;
    break;}
case 49:
#line 435 "xerox/fst/interface.y"
{ yyval.integer=atoi(interface_text);		;
    break;}
case 50:
#line 436 "xerox/fst/interface.y"
{ yyval.integer = iy_find_stack(yyvsp[0].string);	;
    break;}
case 52:
#line 437 "xerox/fst/interface.y"
{ yyval.integer = IY_STACK_NUMBER; ;
    break;}
case 54:
#line 438 "xerox/fst/interface.y"
{ yyval.integer=0;	;
    break;}
case 56:
#line 439 "xerox/fst/interface.y"
{ yyval.integer=15;	;
    break;}
case 58:
#line 441 "xerox/fst/interface.y"
{ yyval.integer=30;	;
    break;}
case 59:
#line 443 "xerox/fst/interface.y"
{ yyval.integer=1; ;
    break;}
case 60:
#line 443 "xerox/fst/interface.y"
{ yyval.integer=0;	;
    break;}
case 62:
#line 448 "xerox/fst/interface.y"
{
        id_type id;
        if (intern_symbol(&id, interface_last_lex_string()) != NO_ERROR) {
            yyerror("C-fsm error: cannot intern symbol in function intern_symbol()");
            yyval.integer= ID_NO_SYMBOL;
        }
        else {
            yyval.integer = id;
        }
    ;
    break;}
case 63:
#line 458 "xerox/fst/interface.y"
{
        id_type id;
        if (intern_unicode(&id, interface_last_lex_string())  != NO_ERROR) {
            yyerror("C-fsm error: cannot intern symbol in function intern_unicode()");
            yyval.integer= ID_NO_SYMBOL;
        }
        else {
            yyval.integer = id;
        }
    ;
    break;}
case 64:
#line 468 "xerox/fst/interface.y"
{
        yyval.integer = EPSILON;
    ;
    break;}
case 65:
#line 471 "xerox/fst/interface.y"
{
        yyval.integer = OTHER;
    ;
    break;}
case 66:
#line 477 "xerox/fst/interface.y"
{
        yyval.alph = yyvsp[-1].alph; iy_add_symbol(yyvsp[-1].alph, yyvsp[0].integer);
    ;
    break;}
case 67:
#line 480 "xerox/fst/interface.y"
{
        yyval.alph = iy_new_alph(); iy_add_symbol(yyval.alph, yyvsp[0].integer);
    ;
    break;}
case 68:
#line 483 "xerox/fst/interface.y"
{
        yyval.alph = NULL;
    ;
    break;}
case 69:
#line 488 "xerox/fst/interface.y"
{yyval.integer = yyvsp[0].integer;;
    break;}
case 70:
#line 489 "xerox/fst/interface.y"
{id_type id;
	                           if (make_fstlabel(&id, yyvsp[-2].integer, yyvsp[0].integer)  != NO_ERROR)
			             {
			               yyerror("C-fsm error: cannot intern symbol in function make_fstlabel()");
			                yyval.integer= ID_NO_SYMBOL;
				     }
				   else
				     {
				       yyval.integer = id;
				     } 
	                           ;
    break;}
case 71:
#line 502 "xerox/fst/interface.y"
{
        yyval.alph = yyvsp[-1].alph;
        iy_add_symbol(yyvsp[-1].alph, yyvsp[0].integer);
    ;
    break;}
case 72:
#line 506 "xerox/fst/interface.y"
{
        yyval.alph = iy_new_alph();
        iy_add_symbol(yyval.alph, yyvsp[0].integer);
    ;
    break;}
case 73:
#line 510 "xerox/fst/interface.y"
{
        yyval.alph = NULL;
    ;
    break;}
case 74:
#line 516 "xerox/fst/interface.y"
{ iy_fs_all1_op(yyvsp[-1].fs_func); ;
    break;}
case 75:
#line 517 "xerox/fst/interface.y"
{ iy_fs_1_op(yyvsp[-1].fs_func, fst_cntxt); ;
    break;}
case 76:
#line 518 "xerox/fst/interface.y"
{ iy_fs_2_op(yyvsp[-1].fs_func, fst_cntxt); ;
    break;}
case 77:
#line 519 "xerox/fst/interface.y"
{ iy_fs_stack_op(yyvsp[-1].fs_func, fst_cntxt); ;
    break;}
case 78:
#line 521 "xerox/fst/interface.y"
{ iy_fs_1_virt_op(yyvsp[-1].fs_func, fst_cntxt); ;
    break;}
case 79:
#line 522 "xerox/fst/interface.y"
{ iy_fs_2_virt_op(yyvsp[-1].fs_func, fst_cntxt); ;
    break;}
case 80:
#line 524 "xerox/fst/interface.y"
{
	  iy_read_networks((void* (*)(FILE*)) network_from_lexc_file, yyvsp[-1].filename);;
    break;}
case 81:
#line 526 "xerox/fst/interface.y"
{
      iy_read_networks((void* (*)(FILE*)) word_file_to_fsm, yyvsp[-1].filename); ;
    break;}
case 82:
#line 528 "xerox/fst/interface.y"
{
      iy_read_networks((void* (*)(FILE*)) network_from_text_file ,yyvsp[-1].filename); ;
    break;}
case 83:
#line 530 "xerox/fst/interface.y"
{
      if (iy_read_networks((void* (*)(FILE*)) networks_from_prolog_file, yyvsp[-1].filename) !=
	  ERR_FILE_OPENING)
	prolog_total_restart(); ;
    break;}
case 84:
#line 535 "xerox/fst/interface.y"
{
  iy_read_networks((void* (*)(FILE*)) networks_from_weighted_prolog_file, yyvsp[-1].filename);
  wprolog_total_restart(); ;
    break;}
case 85:
#line 542 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_add_props; fs_arg1 = yyvsp[0].string;;
    break;}
case 86:
#line 543 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_add_props;  fs_arg1 = "stdin"; ;
    break;}
case 87:
#line 544 "xerox/fst/interface.y"
{ yyval.fs_func = &zero_plus;	;
    break;}
case 88:
#line 545 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_cleanup_net; ;
    break;}
case 89:
#line 546 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_compact_sigma; ;
    break;}
case 90:
#line 547 "xerox/fst/interface.y"
{ yyval.fs_func = &complete_fsm; ;
    break;}
case 91:
#line 548 "xerox/fst/interface.y"
{ yyval.fs_func = &determinize_fsm; ;
    break;}
case 92:
#line 549 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_edit_props; ;
    break;}
case 93:
#line 550 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_eliminate_flag; fs_arg1 = yyvsp[0].string; ;
    break;}
case 94:
#line 551 "xerox/fst/interface.y"
{ yyval.fs_func = &epsilon_remove; ;
    break;}
case 95:
#line 552 "xerox/fst/interface.y"
{ yyval.fs_func = &invert2;	;
    break;}
case 96:
#line 553 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_label_net ;;
    break;}
case 97:
#line 554 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_lower;	;
    break;}
case 98:
#line 555 "xerox/fst/interface.y"
{ yyval.fs_func = &min_fsm; ;
    break;}
case 99:
#line 556 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_name_net; fs_arg1 = yyvsp[0].string; ;
    break;}
case 100:
#line 557 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_negate;	;
    break;}
case 101:
#line 558 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_one_plus; ;
    break;}
case 102:
#line 560 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_multi_char_sigma_net; ;
    break;}
case 103:
#line 561 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_paste_net_labels; ;
    break;}
case 104:
#line 562 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_optimize_arcs; ;
    break;}
case 105:
#line 563 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_reduce_labelset; ;
    break;}
case 106:
#line 564 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_single_char_sigma_net; ;
    break;}
case 107:
#line 566 "xerox/fst/interface.y"
{ yyval.fs_func = &prune_fsm; ;
    break;}
case 108:
#line 567 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_reset_props; fs_arg1 = yyvsp[0].string; ;
    break;}
case 109:
#line 568 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_reset_props;
			     fs_arg1 = ""; fs_arg2 = yyvsp[0].string; ;
    break;}
case 110:
#line 570 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_reset_props; fs_arg1 = "stdin"; ;
    break;}
case 111:
#line 571 "xerox/fst/interface.y"
{ yyval.fs_func = &reverse_fsm;	;
    break;}
case 112:
#line 572 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_sigma_net ;;
    break;}
case 113:
#line 573 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_sort ;;
    break;}
case 114:
#line 574 "xerox/fst/interface.y"
{wait_for_sym();;
    break;}
case 115:
#line 577 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_defsubst;
                                   fs_arg1 = (void *) yyvsp[0].integer;
                                   fs_arg2 = (void *) yyvsp[-2].integer ; ;
    break;}
case 116:
#line 580 "xerox/fst/interface.y"
{wait_for_sym();;
    break;}
case 117:
#line 583 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_symsubst;
                                   fs_arg1 = (void *) yyvsp[0].integer;
                                   fs_arg2 = (void *) yyvsp[-2].alph ; ;
    break;}
case 118:
#line 586 "xerox/fst/interface.y"
{wait_for_sym();;
    break;}
case 119:
#line 589 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_labsubst;
                                   fs_arg1 = (void *) yyvsp[0].integer;
                                   fs_arg2 = (void *) yyvsp[-2].alph ; ;
    break;}
case 120:
#line 592 "xerox/fst/interface.y"
{ yyval.fs_func = &substring_fsm;	;
    break;}
case 121:
#line 593 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_upper;	;
    break;}
case 122:
#line 594 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_zero_plus; ;
    break;}
case 123:
#line 595 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_ambclass; ;
    break;}
case 124:
#line 596 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_compile_replace; fs_arg1=(void*) LOWER;
		          fs_arg2 = (void *) 0;;
    break;}
case 125:
#line 598 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_compile_replace; fs_arg1=(void*) UPPER;
		          fs_arg2 = (void *) 0; ;
    break;}
case 126:
#line 601 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_compile_replace;
				  fs_arg1=(void*) LOWER; fs_arg2 = (void *) 1;;
    break;}
case 127:
#line 603 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_compile_replace;
				  fs_arg1=(void*) UPPER; fs_arg2= (void*) 1;;
    break;}
case 128:
#line 605 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_unoptimize_arcs; ;
    break;}
case 129:
#line 606 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_unreduce_labelset; ;
    break;}
case 130:
#line 607 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_tabular;	;
    break;}
case 131:
#line 608 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_removeepsilon; ;
    break;}
case 132:
#line 609 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_lseq_up; ;
    break;}
case 133:
#line 610 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_lseq_down; ;
    break;}
case 134:
#line 611 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_push_epsilons; fs_arg1 = fs_arg2 = fs_arg3 = fs_arg4 = 0;;
    break;}
case 135:
#line 612 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_push_epsilons; fs_arg1 = (void *) yyvsp[0].integer; fs_arg2 = fs_arg3 = fs_arg4 = 0 ;;
    break;}
case 136:
#line 613 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_push_epsilons; fs_arg1 = (void *) yyvsp[-1].integer; fs_arg2 = (void *) yyvsp[0].integer; fs_arg3 = fs_arg4 = 0;;
    break;}
case 137:
#line 614 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_push_epsilons; fs_arg1 = (void *) yyvsp[-2].integer; fs_arg2 = (void *) yyvsp[-1].integer; fs_arg3 = (void *) yyvsp[0].integer; fs_arg4 = 0;;
    break;}
case 138:
#line 615 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_push_epsilons; fs_arg1 = (void *) yyvsp[-3].integer; fs_arg2 = (void *) yyvsp[-2].integer; fs_arg3 = (void *) yyvsp[-1].integer; fs_arg4 = (void *) yyvsp[0].integer;;
    break;}
case 139:
#line 616 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_seq_up; ;
    break;}
case 140:
#line 617 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_seq_down; ;
    break;}
case 141:
#line 618 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_share_arcs; ;
    break;}
case 142:
#line 619 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_test_seq_up; ;
    break;}
case 143:
#line 620 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_test_seq_down;;
    break;}
case 144:
#line 621 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_tb_insert;;
    break;}
case 145:
#line 622 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_tagsep_insert;;
    break;}
case 146:
#line 623 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_unshare_arcs; ;
    break;}
case 147:
#line 624 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_encode_nonid;;
    break;}
case 148:
#line 625 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_tag_plus_strip_down;;
    break;}
case 149:
#line 627 "xerox/fst/interface.y"
{ yyval.fs_func = &iy_twosided_flag_diacritics;;
    break;}
case 150:
#line 631 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_option_fsm; ;
    break;}
case 151:
#line 632 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_negate_fsm; ;
    break;}
case 152:
#line 633 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_zero_plus; ;
    break;}
case 153:
#line 634 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_one_plus; ;
    break;}
case 154:
#line 635 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_determinize; ;
    break;}
case 155:
#line 636 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_eps_remove; ;
    break;}
case 156:
#line 637 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_copy_fsm; ;
    break;}
case 157:
#line 638 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_invert_fst; ;
    break;}
case 158:
#line 639 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_lower_fsm; ;
    break;}
case 159:
#line 640 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_upper_fsm; ;
    break;}
case 160:
#line 641 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_to_real_fsm; ;
    break;}
case 161:
#line 646 "xerox/fst/interface.y"
{ yyval.fs_func = &minus_fsm;	;
    break;}
case 162:
#line 647 "xerox/fst/interface.y"
{ yyval.fs_func = &crossproduct_fst; ;
    break;}
case 163:
#line 651 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_concat_fsm; ;
    break;}
case 164:
#line 652 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_intersect_fsm; ;
    break;}
case 165:
#line 653 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_union_fsm; ;
    break;}
case 166:
#line 654 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_minus_fsm; ;
    break;}
case 167:
#line 655 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_priority_union; ;
    break;}
case 168:
#line 656 "xerox/fst/interface.y"
{ yyval.fs_func = &virtual_compose_fst; ;
    break;}
case 169:
#line 661 "xerox/fst/interface.y"
{ yyval.test_func = &equiv_fsm_p; ;
    break;}
case 170:
#line 662 "xerox/fst/interface.y"
{ yyval.test_func = &intersect_p; ;
    break;}
case 171:
#line 663 "xerox/fst/interface.y"
{ yyval.test_func = &sublanguage_p; ;
    break;}
case 172:
#line 665 "xerox/fst/interface.y"
{ yyval.fs_func = &compose_fst;	;
    break;}
case 173:
#line 666 "xerox/fst/interface.y"
{ yyval.fs_func = &concat_fsm;	;
    break;}
case 174:
#line 667 "xerox/fst/interface.y"
{ yyval.fs_func = &intersect_fsm;	;
    break;}
case 175:
#line 668 "xerox/fst/interface.y"
{ yyval.fs_func = &shuffle_fsm;	;
    break;}
case 176:
#line 669 "xerox/fst/interface.y"
{ yyval.fs_func = &union_fsm; ;
    break;}
case 177:
#line 672 "xerox/fst/interface.y"
{ iy_stack_load(yyvsp[-1].string,USE_CURRENT_STACK);;
    break;}
case 178:
#line 673 "xerox/fst/interface.y"
{ iy_stack_save(yyvsp[-1].string,NULL);	;
    break;}
case 179:
#line 674 "xerox/fst/interface.y"
{ iy_stack_clear();		;
    break;}
case 180:
#line 675 "xerox/fst/interface.y"
{ iy_stack_drop(0);		;
    break;}
case 181:
#line 676 "xerox/fst/interface.y"
{ iy_stack_push_defined(yyvsp[-1].string); ;
    break;}
case 182:
#line 677 "xerox/fst/interface.y"
{ iy_stack_totop(yyvsp[-1].integer);		;
    break;}
case 183:
#line 678 "xerox/fst/interface.y"
{ iy_stack_reverse();		;
    break;}
case 184:
#line 679 "xerox/fst/interface.y"
{ iy_stack_rotate();		;
    break;}
case 185:
#line 680 "xerox/fst/interface.y"
{ iy_stack_to_defs(iy_stack_load(yyvsp[-1].string,CREATE_NEW_STACK));;
    break;}
case 186:
#line 681 "xerox/fst/interface.y"
{ NVptr nets;
			          iy_stack_save(yyvsp[-1].string, nets=iy_defs_to_stack());
			          iy_stack_to_defs(nets);       ;
    break;}
case 187:
#line 686 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_factorize_down,1);;
    break;}
case 188:
#line 687 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_factorize_up,1);;
    break;}
case 189:
#line 688 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_unfactorize_down,4);;
    break;}
case 190:
#line 689 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_unfactorize_up,4);;
    break;}
case 191:
#line 690 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_factorize_down_infinite_amb,1);;
    break;}
case 192:
#line 691 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_factorize_up_infinite_amb,1);;
    break;}
case 193:
#line 692 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_unfactorize_down_infinite_amb,2);;
    break;}
case 194:
#line 693 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_unfactorize_up_infinite_amb,2);;
    break;}
case 195:
#line 694 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_factorize_down_finite_amb,1);;
    break;}
case 196:
#line 695 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_factorize_up_finite_amb,1);;
    break;}
case 197:
#line 696 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_unfactorize_down_finite_amb,2);;
    break;}
case 198:
#line 697 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_unfactorize_up_finite_amb,2);;
    break;}
case 199:
#line 698 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_factorize_down_short_amb,1);;
    break;}
case 200:
#line 699 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_factorize_up_short_amb,1);;
    break;}
case 201:
#line 700 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_unfactorize_down_short_amb,2);;
    break;}
case 202:
#line 701 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_unfactorize_up_short_amb,2);;
    break;}
case 203:
#line 702 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_factorize_down_unambiguous,1);;
    break;}
case 204:
#line 703 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_factorize_up_unambiguous,1);;
    break;}
case 205:
#line 704 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_unfactorize_down_unambiguous,2);;
    break;}
case 206:
#line 705 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_unfactorize_up_unambiguous,2);;
    break;}
case 207:
#line 706 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_align_down_finite_amb,1);;
    break;}
case 208:
#line 707 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_align_up_finite_amb,1);;
    break;}
case 209:
#line 708 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_unalign_finite_amb,1);;
    break;}
case 210:
#line 709 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_reduce_down_intermediate_alph,-1);;
    break;}
case 211:
#line 710 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_reduce_up_intermediate_alph,-1);;
    break;}
case 212:
#line 711 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_test_down_fail_safety,1);;
    break;}
case 213:
#line 712 "xerox/fst/interface.y"
{iy_m_to_n_op(iy_test_up_fail_safety,1);;
    break;}
case 214:
#line 713 "xerox/fst/interface.y"
{ NETptr net;
                             net= iy_stack_pop();
                             net= iy_compact_net(net, yyvsp[-1].integer);
                             iy_net_to_stack(net);;
    break;}
case 215:
#line 717 "xerox/fst/interface.y"
{ NETptr net;
                             net= iy_stack_pop();
                             net= iy_compact_net(net, 1);
                             iy_net_to_stack(net);;
    break;}
case 216:
#line 721 "xerox/fst/interface.y"
{ NETptr net;
                             net= iy_stack_pop();
                             net= iy_uncompact_net(net);
                             iy_net_to_stack(net);;
    break;}
case 217:
#line 729 "xerox/fst/interface.y"
{ if (yyvsp[-1].file) {iy_list_aliases(yyvsp[-1].file); loclose(yyvsp[-1].file);};
    break;}
case 218:
#line 730 "xerox/fst/interface.y"
{ if (yyvsp[-1].file) {iy_print_defs(yyvsp[-1].file);loclose(yyvsp[-1].file);};
    break;}
case 219:
#line 731 "xerox/fst/interface.y"
{ if (yyvsp[-1].file) {iy_print_lists(yyvsp[-1].file); loclose(yyvsp[-1].file);};
    break;}
case 220:
#line 733 "xerox/fst/interface.y"
{ if (yyvsp[-1].file) {iy_print_eqv_labels (yyvsp[-1].file); loclose(yyvsp[-1].file); };
    break;}
case 221:
#line 735 "xerox/fst/interface.y"
{ dir(yyvsp[0].string);		;
    break;}
case 222:
#line 736 "xerox/fst/interface.y"
{ if (yyvsp[-1].file) {print_file_info(yyvsp[-1].file);loclose(yyvsp[-1].file);} 
                                  else printf ("No files have been loaded or saved yet.");;
    break;}
case 223:
#line 738 "xerox/fst/interface.y"
{ if (yyvsp[-1].file) {iy_print_name(yyvsp[-1].file); loclose(yyvsp[-1].file);};
    break;}
case 224:
#line 739 "xerox/fst/interface.y"
{ if (yyvsp[-1].file) {save_props_s(yyvsp[-2].net,yyvsp[-1].file);loclose(yyvsp[-1].file);};
    break;}
case 225:
#line 740 "xerox/fst/interface.y"
{ if (yyvsp[-1].file) {iy_stack_write(networks_to_prolog_file, yyvsp[-1].file, IY_SORT_ARCS);loclose(yyvsp[-1].file);};
    break;}
case 226:
#line 741 "xerox/fst/interface.y"
{ if (yyvsp[-1].file)
                               {
                                 iy_net_to_text_file (FST_fsm_to_word_file, yyvsp[-1].file);
                                 loclose(yyvsp[-1].file);
                                }
                           ;
    break;}
case 227:
#line 748 "xerox/fst/interface.y"
{ if (yyvsp[-1].file)
                              {
                                iy_net_to_text_file (FST_network_to_text_file, yyvsp[-1].file);
				loclose(yyvsp[-1].file);
                               }
                            ;
    break;}
case 228:
#line 754 "xerox/fst/interface.y"
{ if (yyvsp[-1].file) {iy_print_stack(yyvsp[-1].file,yyvsp[-2].integer);loclose(yyvsp[-1].file);};
    break;}
case 229:
#line 755 "xerox/fst/interface.y"
{ if (yyvsp[-1].file) {iy_print_stacks(yyvsp[-1].file);loclose(yyvsp[-1].file);};
    break;}
case 230:
#line 758 "xerox/fst/interface.y"
{ if (yyvsp[-1].file) {iy_stack_write(networks_to_weighted_prolog_file, yyvsp[-1].file, IY_SORT_ARCS);loclose(yyvsp[-1].file);};
    break;}
case 231:
#line 759 "xerox/fst/interface.y"
{ if(yyvsp[-1].file) {print_storage_info(yyvsp[-1].file);loclose(yyvsp[-1].file);};
    break;}
case 232:
#line 763 "xerox/fst/interface.y"
{ if(yyvsp[-1].file) {iy_print_rand_words(yyvsp[-1].file,yyvsp[-2].integer, LOWER); loclose(yyvsp[-1].file);};
    break;}
case 233:
#line 764 "xerox/fst/interface.y"
{ if(yyvsp[-1].file) {iy_print_rand_words(yyvsp[-1].file,yyvsp[-2].integer, UPPER); loclose(yyvsp[-1].file);};
    break;}
case 234:
#line 765 "xerox/fst/interface.y"
{ if(yyvsp[-1].file) {iy_print_rand_words(yyvsp[-1].file,yyvsp[-2].integer, BOTH_SIDES); loclose(yyvsp[-1].file);};
    break;}
case 235:
#line 766 "xerox/fst/interface.y"
{ if(yyvsp[-1].file) {iy_print_words(yyvsp[-1].file,yyvsp[-3].net,yyvsp[-2].integer, BOTH_SIDES); loclose(yyvsp[-1].file);};
    break;}
case 236:
#line 767 "xerox/fst/interface.y"
{ if(yyvsp[-1].file) {iy_print_words(yyvsp[-1].file,yyvsp[-3].net,yyvsp[-2].integer, LOWER); loclose(yyvsp[-1].file);};
    break;}
case 237:
#line 768 "xerox/fst/interface.y"
{ if(yyvsp[-1].file) {iy_print_words(yyvsp[-1].file,yyvsp[-3].net,yyvsp[-2].integer, UPPER); loclose(yyvsp[-1].file);};
    break;}
case 238:
#line 771 "xerox/fst/interface.y"
{ if(yyvsp[-1].file) { iy_do_number_to_word(yyvsp[-3].net, yyvsp[-2].integer, LOWER, yyvsp[-1].file);loclose(yyvsp[-1].file); };
    break;}
case 239:
#line 772 "xerox/fst/interface.y"
{ if(yyvsp[-1].file) { iy_do_number_to_word(yyvsp[-3].net, yyvsp[-2].integer, UPPER, yyvsp[-1].file);loclose(yyvsp[-1].file); };
    break;}
case 240:
#line 773 "xerox/fst/interface.y"
{ if(yyvsp[-1].file) { NETptr net; net = iy_top_stack(0); 
                                                    iy_do_word_to_numbers(net, yyvsp[-2].string, LOWER, yyvsp[-1].file);loclose(yyvsp[-1].file); } free (fs_arg1); ;
    break;}
case 241:
#line 775 "xerox/fst/interface.y"
{
  if(yyvsp[-1].file)
    { NETptr net;    
    id_type id; 
    if (FST_intern_literal(&id, fs_arg1) != NO_ERROR)
      quit_on_fail_error(IY_QUIT_ON_FAIL);
    net = get_def_uncopied(id);
    iy_do_word_to_numbers(net, yyvsp[-2].string, LOWER, yyvsp[-1].file);
    loclose(yyvsp[-1].file); } 
  free (fs_arg1); 
  free (fs_arg2); ;
    break;}
case 242:
#line 786 "xerox/fst/interface.y"
{ if(yyvsp[-1].file) { NETptr net; net = iy_top_stack(0); 
                                                    iy_do_word_to_numbers(net, yyvsp[-2].string, UPPER, yyvsp[-1].file);loclose(yyvsp[-1].file); } free (fs_arg1); ;
    break;}
case 243:
#line 789 "xerox/fst/interface.y"
{
  if(yyvsp[-1].file)
    { 
      NETptr net; 
      id_type id;  
      if (FST_intern_literal(&id, fs_arg1) != NO_ERROR)
	quit_on_fail_error(IY_QUIT_ON_FAIL);
      net = get_def_uncopied(id);
      iy_do_word_to_numbers(net, yyvsp[-2].string, UPPER, yyvsp[-1].file);
      loclose(yyvsp[-1].file); } 
  free (fs_arg1);
  free (fs_arg2); ;
    break;}
case 244:
#line 802 "xerox/fst/interface.y"
{ if(yyvsp[-1].file) {iy_print_net(yyvsp[-1].file,yyvsp[-3].integer,yyvsp[-2].net);loclose(yyvsp[-1].file);};
    break;}
case 245:
#line 806 "xerox/fst/interface.y"
{ if (yyvsp[-1].file) {iy_print_nets(yyvsp[-1].file,yyvsp[-2].integer);loclose(yyvsp[-1].file);};
    break;}
case 246:
#line 807 "xerox/fst/interface.y"
{ iy_test_2_op(yyvsp[-1].test_func); ;
    break;}
case 247:
#line 808 "xerox/fst/interface.y"
{  if(yyvsp[-1].file) { NETptr net; net = iy_top_stack(0);  
                                                               iy_tally_sigma_by_words(yyvsp[-1].file, net, 0, yyvsp[-2].string);loclose(yyvsp[-1].file); }; free (fs_arg1); ;
    break;}
case 248:
#line 810 "xerox/fst/interface.y"
{  if(yyvsp[-1].file) { NETptr net; net = iy_top_stack(0);  
                                                               iy_tally_sigma_by_words(yyvsp[-1].file, net, 0, "both");loclose(yyvsp[-1].file); }; ;
    break;}
case 249:
#line 813 "xerox/fst/interface.y"
{ NETptr net; net = iy_top_stack(0); TEST_fsopt_preprocess((cfsm_fsm) net, yyvsp[-1].integer); ;
    break;}
case 250:
#line 814 "xerox/fst/interface.y"
{ NETptr net; net = iy_top_stack(0); 
                                                   iy_fsopt_reduce_sigma (net, yyvsp[-1].integer, 1, 1, IY_PRINT_SPACE?" ":""); ;
    break;}
case 251:
#line 816 "xerox/fst/interface.y"
{  NETptr net; net = iy_top_stack(0); iy_TEST_1_unicode_parse (net, yyvsp[-1].string, fst_cntxt);;
    break;}
case 252:
#line 820 "xerox/fst/interface.y"
{ NETptr net;
                                        net= iy_stack_pop();
                                        net= iy_add_path_from_line(net, yyvsp[0].string);
                                        iy_net_to_stack(net);;
    break;}
case 253:
#line 824 "xerox/fst/interface.y"
{ NETptr net;
                                        net= iy_stack_pop();
                                        net= iy_add_path_from_file(net, yyvsp[-1].filename);
                                        iy_net_to_stack(net);;
    break;}
case 254:
#line 828 "xerox/fst/interface.y"
{ NETptr net;
                                        net= iy_stack_pop();
                                        net= iy_add_word_from_line(net, yyvsp[-2].string, yyvsp[-1].string);
                                        iy_net_to_stack(net);
                                       /*  free (fs_arg1); free (fs_arg2);  */;
    break;}
case 255:
#line 833 "xerox/fst/interface.y"
{ NETptr net;
                                        net= iy_stack_pop();
                                        net= iy_add_word_from_file(net, yyvsp[-1].filename);
                                        iy_net_to_stack(net);
                                       /*  free (fs_arg1); free (fs_arg2); */ ;
    break;}
case 256:
#line 842 "xerox/fst/interface.y"
{ yyval.integer = 0; ;
    break;}
case 257:
#line 843 "xerox/fst/interface.y"
{ yyval.integer = 1; ;
    break;}
case 258:
#line 844 "xerox/fst/interface.y"
{ yyval.integer = 2; ;
    break;}
case 259:
#line 845 "xerox/fst/interface.y"
{ yyval.integer = 3; ;
    break;}
case 260:
#line 846 "xerox/fst/interface.y"
{ yyval.integer = 4; ;
    break;}
case 261:
#line 847 "xerox/fst/interface.y"
{ yyval.integer = 5; ;
    break;}
case 262:
#line 848 "xerox/fst/interface.y"
{ yyval.integer = 6; ;
    break;}
case 263:
#line 849 "xerox/fst/interface.y"
{ yyval.integer = 7; ;
    break;}
case 264:
#line 850 "xerox/fst/interface.y"
{ yyval.integer = 8; ;
    break;}
case 265:
#line 851 "xerox/fst/interface.y"
{ yyval.integer = 9; ;
    break;}
case 266:
#line 852 "xerox/fst/interface.y"
{ yyval.integer = 10; ;
    break;}
case 267:
#line 854 "xerox/fst/interface.y"
{ yyval.integer = 11; ;
    break;}
case 268:
#line 855 "xerox/fst/interface.y"
{ yyval.integer = 12; ;
    break;}
case 269:
#line 856 "xerox/fst/interface.y"
{ yyval.integer = 18;;
    break;}
case 270:
#line 857 "xerox/fst/interface.y"
{ yyval.integer = 19;;
    break;}
case 271:
#line 859 "xerox/fst/interface.y"
{ yyval.integer = 13; ;
    break;}
case 272:
#line 860 "xerox/fst/interface.y"
{ yyval.integer = 14;;
    break;}
case 273:
#line 861 "xerox/fst/interface.y"
{ yyval.integer = 17;;
    break;}
case 274:
#line 862 "xerox/fst/interface.y"
{ yyval.integer = 15;;
    break;}
case 275:
#line 863 "xerox/fst/interface.y"
{ yyval.integer = 16;;
    break;}
case 277:
#line 867 "xerox/fst/interface.y"
{ yyval.net = iy_top_stack(yyvsp[0].integer); ;
    break;}
case 278:
#line 870 "xerox/fst/interface.y"
{ 
  id_type id;
  if (FST_intern_literal(&id, yyvsp[0].string) != NO_ERROR)
    quit_on_fail_error(IY_QUIT_ON_FAIL);
  yyval.net = get_def_uncopied(id); ;
    break;}
case 279:
#line 875 "xerox/fst/interface.y"
{ yyval.net = iy_top_stack(0); ;
    break;}
case 280:
#line 878 "xerox/fst/interface.y"
{ iy_fs_apply(yyvsp[-1].filename,NULL, "apply down> ", UPPER, iy_apply_and_print); ;
    break;}
case 281:
#line 879 "xerox/fst/interface.y"
{ iy_fs_apply(NULL, yyvsp[0].string, "apply down> ", UPPER, iy_apply_and_print); ;
    break;}
case 282:
#line 880 "xerox/fst/interface.y"
{ iy_fs_apply(yyvsp[-1].filename,NULL, "apply up> ", LOWER, iy_apply_and_print); ;
    break;}
case 283:
#line 881 "xerox/fst/interface.y"
{ iy_fs_apply(NULL, yyvsp[0].string, "apply up> ", LOWER, iy_apply_and_print); ;
    break;}
case 284:
#line 882 "xerox/fst/interface.y"
{ iy_fs_apply(NULL, yyvsp[0].string, "compose-apply up> ", LOWER, iy_compose_apply_and_print); ;
    break;}
case 285:
#line 883 "xerox/fst/interface.y"
{ iy_fs_apply(NULL, yyvsp[0].string, "compose-apply down> ", UPPER, iy_compose_apply_and_print); ;
    break;}
case 286:
#line 884 "xerox/fst/interface.y"
{ iy_fs_apply(yyvsp[-1].filename, NULL, "compose-apply up> ", LOWER, iy_compose_apply_and_print); ;
    break;}
case 287:
#line 885 "xerox/fst/interface.y"
{ iy_fs_apply(yyvsp[-1].filename, NULL, "compose-apply down> ", UPPER, iy_compose_apply_and_print); ;
    break;}
case 288:
#line 887 "xerox/fst/interface.y"
{ iy_apply_patterns(NULL, yyvsp[0].string, LOWER, "apply patterns up> ") ;;
    break;}
case 289:
#line 888 "xerox/fst/interface.y"
{ iy_apply_patterns(yyvsp[-1].filename, NULL, LOWER, "apply patterns up> "); ;
    break;}
case 290:
#line 889 "xerox/fst/interface.y"
{ iy_apply_patterns(NULL, yyvsp[0].string, UPPER, "apply patterns down> "); ;
    break;}
case 291:
#line 890 "xerox/fst/interface.y"
{ iy_apply_patterns(yyvsp[-1].filename, NULL, UPPER, "apply patterns down> "); ;
    break;}
case 292:
#line 895 "xerox/fst/interface.y"
{ yyval.string=iy_current_stack_name();	;
    break;}
case 293:
#line 896 "xerox/fst/interface.y"
{ pwd(); yyval.string = NULL; ;
    break;}
case 294:
#line 897 "xerox/fst/interface.y"
{ yyval.string = decode_char_encoding(*last_variable->value); ;
    break;}
case 295:
#line 898 "xerox/fst/interface.y"
{  yyval.string = fsm_copyright(NULL); ;
    break;}
case 296:
#line 900 "xerox/fst/interface.y"
{ iy_show_use_mmap(); ;
    break;}
case 297:
#line 901 "xerox/fst/interface.y"
{ iy_show_license_type(); ;
    break;}
case 298:
#line 905 "xerox/fst/interface.y"
{ if (iy_change_stack(yyvsp[0].integer,stdout)) IY_STACK_NUMBER=yyvsp[0].integer; ;
    break;}
case 299:
#line 906 "xerox/fst/interface.y"
{ cd(yyvsp[0].string); ;
    break;}
case 300:
#line 907 "xerox/fst/interface.y"
{ srand(yyvsp[0].integer); ;
    break;}
case 301:
#line 909 "xerox/fst/interface.y"
{iy_set_char_encoding(CHAR_ENC_UTF_8, NULL);
		   if (IY_VERBOSE) show_last_var(merkkijono,
						 (int *)"UTF-8");;
    break;}
case 302:
#line 913 "xerox/fst/interface.y"
{iy_set_char_encoding(CHAR_ENC_ISO_8859_1, NULL);
		   if (IY_VERBOSE)
		     show_last_var(merkkijono, (int *) "ISO-8859-1");;
    break;}
case 303:
#line 917 "xerox/fst/interface.y"
{iy_set_char_encoding(-1, yyvsp[0].string);;
    break;}
case 304:
#line 918 "xerox/fst/interface.y"
{ fsm_copyright(yyvsp[0].string);
		   if (IY_VERBOSE)
		     show_last_var(merkkijono, (int *) fsm_copyright(NULL));;
    break;}
case 305:
#line 923 "xerox/fst/interface.y"
{ iy_set_use_mmap(yyvsp[0].integer);;
    break;}
case 306:
#line 925 "xerox/fst/interface.y"
{ iy_set_license_type(FILE_TYPE_LICENSE_PRIVATE);;
    break;}
case 307:
#line 927 "xerox/fst/interface.y"
{ iy_set_license_type(FILE_TYPE_LICENSE_RESERVED);;
    break;}
case 308:
#line 929 "xerox/fst/interface.y"
{ iy_set_license_type(FILE_TYPE_LICENSE_LICENSED);;
    break;}
case 309:
#line 931 "xerox/fst/interface.y"
{ iy_set_license_type(FILE_TYPE_LICENSE_PUBLIC); ;
    break;}
case 310:
#line 933 "xerox/fst/interface.y"
{ iy_set_license_type(FILE_TYPE_LICENSE_NONE); ;
    break;}
case 311:
#line 934 "xerox/fst/interface.y"
{ iy_set_license_type(atoi(interface_text)); ;
    break;}
case 312:
#line 938 "xerox/fst/interface.y"
{ yyval.int_p=&IY_LEX_ERRORS; ;
    break;}
case 313:
#line 940 "xerox/fst/interface.y"
{ yyval.int_p=&IY_MAX_STATE_VISITS ;
    break;}
case 314:
#line 944 "xerox/fst/interface.y"
{ yyval.int_p=&IY_MARK_VERSION;  ;
    break;}
case 315:
#line 945 "xerox/fst/interface.y"
{ yyval.int_p=&IY_MINIMIZE_P;    ;
    break;}
case 316:
#line 946 "xerox/fst/interface.y"
{ yyval.int_p=&IY_COMPOSE_FLAG_AS_SPECIAL;    ;
    break;}
case 317:
#line 947 "xerox/fst/interface.y"
{ yyval.int_p=&IY_OBEY_FLAGS;    ;
    break;}
case 318:
#line 948 "xerox/fst/interface.y"
{ yyval.int_p=&IY_NAME_NETS;	;
    break;}
case 319:
#line 949 "xerox/fst/interface.y"
{ yyval.int_p=&IY_PRINT_PAIRS;	;
    break;}
case 320:
#line 950 "xerox/fst/interface.y"
{ yyval.int_p=&IY_PRINT_SIGMA;	;
    break;}
case 321:
#line 951 "xerox/fst/interface.y"
{ yyval.int_p=&IY_PRINT_SPACE;	;
    break;}
case 322:
#line 952 "xerox/fst/interface.y"
{ yyval.int_p=&IY_RECURSIVE_DEFINE;	;
    break;}
case 323:
#line 953 "xerox/fst/interface.y"
{ yyval.int_p=&IY_RECURSIVE_APPLY;  ;
    break;}
case 324:
#line 954 "xerox/fst/interface.y"
{ yyval.int_p=&IY_INORDER;	;
    break;}
case 325:
#line 955 "xerox/fst/interface.y"
{ yyval.int_p=&IY_QUIT_ON_FAIL;  ;
    break;}
case 326:
#line 956 "xerox/fst/interface.y"
{ yyval.int_p=&IY_SHOW_FLAGS;    ;
    break;}
case 327:
#line 957 "xerox/fst/interface.y"
{ yyval.int_p=&IY_ASSERT;  ;
    break;}
case 328:
#line 958 "xerox/fst/interface.y"
{ yyval.int_p=&IY_QUOTE_SPECIAL; ;
    break;}
case 329:
#line 959 "xerox/fst/interface.y"
{ yyval.int_p=&IY_READ_LEFT;	;
    break;}
case 330:
#line 960 "xerox/fst/interface.y"
{ yyval.int_p=&IY_RETOKENIZE_P; ;
    break;}
case 331:
#line 962 "xerox/fst/interface.y"
{ yyval.int_p=&IY_NEED_SEPARATORS; ;
    break;}
case 332:
#line 963 "xerox/fst/interface.y"
{ yyval.int_p=&IY_COUNT_PATTERNS ;;
    break;}
case 333:
#line 964 "xerox/fst/interface.y"
{ yyval.int_p=&IY_DELETE_PATTERNS ;;
    break;}
case 334:
#line 965 "xerox/fst/interface.y"
{ yyval.int_p=&IY_EXTRACT_PATTERNS ;;
    break;}
case 335:
#line 966 "xerox/fst/interface.y"
{ yyval.int_p=&IY_MARK_PATTERNS ;;
    break;}
case 336:
#line 968 "xerox/fst/interface.y"
{ yyval.int_p=&IY_SORT_ARCS;	;
    break;}
case 337:
#line 970 "xerox/fst/interface.y"
{ yyval.int_p=&SQ_FINAL_STRINGS_ARCS;	;
    break;}
case 338:
#line 971 "xerox/fst/interface.y"
{ yyval.int_p=&SQ_INTERN_STRINGS_ARCS;	;
    break;}
case 339:
#line 972 "xerox/fst/interface.y"
{ yyval.int_p=&SQ_STRING_ONELONG;	;
    break;}
case 340:
#line 975 "xerox/fst/interface.y"
{ yyval.int_p=&IY_UNICODE;	;
    break;}
case 341:
#line 976 "xerox/fst/interface.y"
{ yyval.int_p=&IY_VERBOSE;	;
    break;}
case 342:
#line 977 "xerox/fst/interface.y"
{ yyval.int_p=&IY_COMPLETION;	;
    break;}
case 343:
#line 980 "xerox/fst/interface.y"
{ show_last_var(luku, yyvsp[0].int_p); ;
    break;}
case 344:
#line 981 "xerox/fst/interface.y"
{ show_last_var(binaarinen, yyvsp[0].int_p); ;
    break;}
case 345:
#line 982 "xerox/fst/interface.y"
{ show_last_var(merkkijono, (int *) yyvsp[0].string); ;
    break;}
case 346:
#line 984 "xerox/fst/interface.y"
{ *yyvsp[-1].int_p = yyvsp[0].integer; if (IY_VERBOSE) show_last_var(luku,yyvsp[-1].int_p); ;
    break;}
case 347:
#line 985 "xerox/fst/interface.y"
{ *yyvsp[-1].int_p = yyvsp[0].integer;
/*                          if ($1 == &IY_QUIT_ON_FAIL)
                            set_quit_on_fail_p($2);
*/
if (yyvsp[-1].int_p == &IY_OBEY_FLAGS)
  IY_OBEY_FLAGS = FST_obey_flags_p((FST_CNTXTptr) fst_cntxt) = yyvsp[0].integer;
                          if (IY_VERBOSE) show_last_var(binaarinen,yyvsp[-1].int_p);;
    break;}
case 348:
#line 992 "xerox/fst/interface.y"
{ /* We cannot set a value here.  */ ;
    break;}
case 349:
#line 995 "xerox/fst/interface.y"
{ iy_define(yyvsp[-1].string,iy_stack_pop(), IY_VERBOSE); ;
    break;}
case 350:
#line 996 "xerox/fst/interface.y"
{ 
           /*** Try changing the lookahead token ***/
           int ttc=interface_yacc_lex__change_token_for_define();
           switch_verbose(0); /* switch off verbosity, temporarily */
	   /* intercept the interface lex buffer, pass its valuable part to regex,
	      and then return to interface; needs a post cleanup/re-change
	      action. Save the actual state for recursive calls. */
           old_regex_finfo_ptr=read_regex_immediate_action(&regex_on_command_line, fst_cntxt, ttc, &regex_file_symbol_length); 
        ;
    break;}
case 351:
#line 1005 "xerox/fst/interface.y"
{ /* Clean interface lex buffers and get back their contents,
			      deducing the chunk that regex processed. */
                          read_regex_post_action(regex_cl_length_processed, fst_cntxt, old_regex_finfo_ptr, regex_file_info);
	                  switch_verbose(1); /* switch verbosity back */
                          if (!IY_LAST_ERRORS)
			    iy_define(yyvsp[-2].string,iy_stack_pop(), TRUE); /* real define handling, enfin */
			  else cannot_def(yyvsp[-2].string);
			;
    break;}
case 352:
#line 1014 "xerox/fst/interface.y"
{ yyval.string=yyvsp[0].string; wait_for_parameter(); ;
    break;}
case 353:
#line 1017 "xerox/fst/interface.y"
{ if (yyvsp[-1].file) {iy_list_aliases(yyvsp[-1].file); loclose(yyvsp[-1].file);};
    break;}
case 354:
#line 1018 "xerox/fst/interface.y"
{ iy_save_alias(yyvsp[-1].string,NULL); ;
    break;}
case 355:
#line 1019 "xerox/fst/interface.y"
{ iy_save_alias(yyvsp[-1].string,read_line()); ;
    break;}
case 356:
#line 1020 "xerox/fst/interface.y"
{ yyval.string=yyvsp[0].string; wait_for_EOL(); ;
    break;}
case 357:
#line 1022 "xerox/fst/interface.y"
{ return ALIASTOP; /* This should occur when
					      alias is interrupted */
			;
    break;}
case 358:
#line 1025 "xerox/fst/interface.y"
{ char *p, *x;
                          x = read_alias_name(); 
			  if ((p = iy_find_alias(x)))
			    {
			      int n = iy_alias_pars(p);
			      char *pars[11];
			      iy_execute_alias(p, pars, n,
					       read_parameters(pars, n));
			      interface_fake_eol = 1;
			      if (too_many_aliases) return ALIASTOP;
			    }
			  else
			    {
			      command_error_detected(NULL, NULL, x);
			      return SYNTAX_ERROR;
			    }
			;
    break;}
case 359:
#line 1043 "xerox/fst/interface.y"
{ one_liner=0; one_regex=1; remember_to_clean=1;
			  regex_to_stack(fst_cntxt);
			  interface_restart_if_necessary();
			  remember_to_clean=0;
			;
    break;}
case 360:
#line 1048 "xerox/fst/interface.y"
{ regex_from_file(yyvsp[-1].filename, fst_cntxt);
                        /* It seems that the trailing EOL gets processed
                           twice. Have to subtract to keep the
                           line count correctly. 2002-04-30 LK */
    /*
                          if (interface_file_info->file.type == STDIN_IN)
                            (interface_file_info->line_number--);
    */
                         ;
    break;}
case 362:
#line 1059 "xerox/fst/interface.y"
{ one_liner=0 ; one_regex=1; remember_to_clean=1;

			  /* The line contains a regular expression.
			   * It is pushed into the stack.
			   * Regex_to_stack is supposed to read the
			   * new line character in the end of the
			   * line.  */

			  regex_to_stack(fst_cntxt);
			  one_liner=0; one_regex=1; remember_to_clean=0;
			  interface_restart_if_necessary();
			;
    break;}
case 363:
#line 1071 "xerox/fst/interface.y"
{ one_liner=0 ; one_regex=1; remember_to_clean=1;
			  /* **  Hack to mimic LEX action for SOMETHING_ELSE */
                          if (!regex_on_command_line) /** otherwise try to read a first (!) "regex 1 \nl" 99-5-1 */
                           regex_lex_yyless(0); 
			  /* The line contains a regular expression.
			   * It is pushed into the stack.
			   * Regex_to_stack is supposed to read the
			   * new line character in the end of the
			   * line.  */

			  regex_to_stack(fst_cntxt);
			  one_liner=0; one_regex=1; remember_to_clean=0;
			  interface_restart_if_necessary();
			;
    break;}
case 364:
#line 1087 "xerox/fst/interface.y"
{ iy_print_brief(); ;
    break;}
case 365:
#line 1088 "xerox/fst/interface.y"
{ iy_print_help(last_command); ;
    break;}
case 366:
#line 1089 "xerox/fst/interface.y"
{ iy_print_help(last_command); ;
    break;}
case 367:
#line 1090 "xerox/fst/interface.y"
{ read_regex_help();
			  iy_print_help(last_command);
			  interface_clean_inputline();
			;
    break;}
case 368:
#line 1094 "xerox/fst/interface.y"
{ if (strcmp(last_command->command,"help"))
			    {

			      /* We do not want to do this in the
			       * standard way because we should
			       * introduce a new entry for each
			       * command (i.e. the parameter of
			       * 'help').  So we just catch everything
			       * here (except commands 'help help' and
			       * 'help regex') and try to resolve the
			       * command name that was given as a
			       * parameter.  */

			      iy_print_help(last_command);
			      interface_clean_inputline();
			    }
			  else if (read_variable())
			    {
			      iy_print_help(last_variable);
			      interface_clean_inputline();
			    }
			  else if (read_regex_help())
			    {
			      iy_print_help(last_command);
			      interface_clean_inputline();
			    }
			  else
			    { char *p;
			      p=read_line();
			      if (!iy_print_apropos(p,0))
                                  print_no_help((p ? p : "that"));
                                    
			    }
			;
    break;}
case 370:
#line 1131 "xerox/fst/interface.y"
{ char *p;
			  p=more_inputline();
			  warn_toomany((p ? p : ""));

			  /* We have read more arguments than
			   * expected.  To resolve this we remove the
			   * rest of the line.  */

			  iy_print_usage(last_command);
			  quit_on_fail_error(IY_QUIT_ON_FAIL);
			;
    break;}
case 372:
#line 1143 "xerox/fst/interface.y"
{ interface_clean_inputline(); ;
    break;}
case 373:
#line 1144 "xerox/fst/interface.y"
{ read_semic_as_eol(); ;
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
#line 1146 "xerox/fst/interface.y"


/****************************************************************
  YYERROR AND YYWRAP are needed for yacc
 ****************************************************************/
static int yyerror(char *str)
{
  return(1);
}

/*
static int yywrap(void)
{
  return(1);
}
*/

/****************************************************************
  COMMAND: not recognised
 ****************************************************************/
static void command_error_detected(struct command *comm, struct command *var,
				   char *junk)
{
  if (comm)
    {
      warn_command_parameters(interface_text);

      /* We have read a command but there is something wrong with the
       * parameters.  */

      iy_print_usage(comm);
      if (var) iy_print_usage(var);

      quit_on_fail_error(IY_QUIT_ON_FAIL);

    }
  else
    {
      warn_command_not_found(junk);

      quit_on_fail_error(IY_QUIT_ON_FAIL);
    }

  /* Here we have not recognised the last command.  We resolve
   * this by removing everything until the next newline character.
   * */

  interface_clean_inputline();
  fputs("\n",stderr); /* To clear things...
			 It is not very clear what we have printed so far... */
}

/****************************************************************
  COMMAND: show
 ****************************************************************/
static char *ONF(int i)
{
  return (i ? "ON" : "OFF");
}

static void show_last_var(int t, int *n)
{
  switch (t)
    {
    case luku:
      printf("%s = %d\n", last_variable->command, *n);
      break;
    case binaarinen:
      n = (int *) ONF(*n);
    case merkkijono:
      if (n) printf("%s = %s\n", last_variable->command, (char *) n);
      break;
    }

  /* Because we do not know the state of LEX after reading the
   * variable (it depends on the type of the variable), we force LEX
   * to read next the NewLine character.  */

  wait_for_EOL();
}

/****************************************************************
  COMMAND: source -- execute commands
 ****************************************************************/
static void command_prompt()
{
  if ((print_prompt>0) && !interface_fake_eol) print_stack_prompt(FST_PROMPT);
  interface_fake_eol=0;
  last_command=NULL;
  last_variable=NULL;
  wait_for_command();
}



static char *LAST_SCRIPT_FILE = NULL;
static long LAST_SCRIPT_FILE_LINE = 0;

static void discard_any_old_script(void)
{
  if (LAST_SCRIPT_FILE)
    {
      fsm_fprintf(stderr, "Discarding a previously interrupted script: \
'%s'\n", LAST_SCRIPT_FILE);
      free(LAST_SCRIPT_FILE);
      LAST_SCRIPT_FILE = NULL;
    }
}

static int interrupt_script(FST_CNTXTptr fst_cntxt)
{
  if (interface_file_type() != FILE_IN)
    {
      fsm_fprintf(stderr, "Nothing to interrupt. Not sourcing a script.\n");
      quit_on_fail_error(IY_QUIT_ON_FAIL);
      interface_fake_eol = 1;
      return(ERROR);
    }
  else
    {
      discard_any_old_script();

      LAST_SCRIPT_FILE = strdup(interface_file_name());
      LAST_SCRIPT_FILE_LINE = interface_file_line();

      fsm_fprintf(stderr, "Interrupting script '%s' on line %ld.\n",
		  LAST_SCRIPT_FILE, LAST_SCRIPT_FILE_LINE);

    }
  return(NO_ERROR);
}

static void continue_script(FST_CNTXTptr fst_cntxt)
{
  finfo buffer, *prev;
  ifile file;

  if (!LAST_SCRIPT_FILE)
    {
      fsm_fprintf(stderr, "No interrupted script. Cannot continue\n");
      quit_on_fail_error(IY_QUIT_ON_FAIL);
      interface_fake_eol = 1;
      return;
    }

  fsm_fprintf(stderr, "Continuing script '%s' from line %ld.\n",
	      LAST_SCRIPT_FILE, LAST_SCRIPT_FILE_LINE+1);


  file.name = LAST_SCRIPT_FILE;
  file.type = FILE_IN;

  /* Set LAST_SCRIPT_FILE to NULL to allow a later interrupt
     command in the same resumed script file. */

  LAST_SCRIPT_FILE = NULL;

  if ((prev = new_interface_input_file(&file, &buffer,
				       LAST_SCRIPT_FILE_LINE)))
    {
      execute_commands(fst_cntxt);
      
      old_interface_input_file(prev, &buffer);

      interface_fake_eol = 1;
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);

  /* Reclaim the name string. */

  free(file.name);
}



static void script_error(void)
{
  /* If the script was launced with the 'source script' command or
     with -l or -f options on the command line, we know the name
     of the file. However, if the script was launched from the
     Unix command line with 'fst < scriptfile', we don't know
     the name of the file but we can print the line number
     where the error occurs.
  */

  if (interface_file_name())
     fsm_fprintf(stderr, "*** Error on line %d in file '%s'.\n",
		 interface_file_line(), interface_file_name());
  else
    fsm_fprintf(stderr, "*** Error on line %d in the script\n",
		interface_file_line());

  if (!IY_INTERACTIVE)
    {
      if (interface_file_type() == STRING_IN)
	return;
      else
	exit_with_error("Aborting the script because quit-on-fail is ON.");
    }
  else
    CONTINUE_OR_STOP = CONTINUE;
}



void execute_commands(FST_CNTXTptr fst_cntxt)
{
  int command;

  for (;;)
    {
      command_prompt();

      /* If the user has terminated some command such as 'read text'
	 'add properties' with a control-D, as instructed, the
	 the end-of-file indicator on stdin is set. This seems not
	 to cause any problems on Solaris and Linux systems, probably
	 because they automatically call clearr() in that case.
	 However, on MacOS X (a.k.a Darwin) This has to be done
	 explicitly to avoid the program from exiting abruptly. */

#ifdef Darwin
      if (interface_file_type() == STDIN_IN && IY_INTERACTIVE && feof(stdin))
	clearerr(stdin);
#endif
      command = yyparse(fst_cntxt);

      switch (command)
	{
	case SYNTAX_ERROR:
	  if (interface_file_type() == FILE_IN ||
	      (interface_file_type() == STDIN_IN && !IY_INTERACTIVE))
	    {
	      script_error();
	      return;
	    }
	  else
	    {
	      fputs("Error - command not executed\n",stderr);
	      CONTINUE_OR_STOP = CONTINUE;
	    }
	  break;

	case INTERRUPT_SCRIPT:
	  /* Have to increase the line count 'manually' in this case. */
	  increment_interface_line_count();
	  if(interrupt_script(fst_cntxt) == NO_ERROR)
	    return;
	  break;
	case CONTINUE_SCRIPT:
	  continue_script(fst_cntxt);
	  break;

	case ALIASTOP:
	  wait_for_END(); /* this is set on/off in iy_execute_alias */
	  break;
	case QUIT:
	  return;
	  break;
	default:
	  if (CONTINUE_OR_STOP == STOP)
	    {
	      /* If we are sourcing a script in a non-interactive
		 mode or executing a string of commands from the
		 command line, we abort on error. In the former case
		 we print the number of the line where the error
		 occurs and the name of the file (if we know it).

		 If we are in an interactive mode, we continue
		 in spite of the error.
	      */
	      switch (interface_file_type())
		{
		case FILE_IN:
		case PIPE_IN:
		  script_error();
		  return;
		  break;
		case STDIN_IN:
		  if (IY_INTERACTIVE)
		    CONTINUE_OR_STOP = CONTINUE;
		  else
		    {
		      script_error();
		      return;
		    }
		  break;
		case STRING_IN:
		  return;
		  break;
		default:
		  break;
		}
	    }
	  break;
	}
    }
}

void execute_string(char *p, FST_CNTXTptr fst_cntxt)
{
  finfo buffer, *prev;
  ifile file;

  file.name = p;
  file.type = STRING_IN;

  if ((prev = new_interface_input_file(&file, &buffer, 0)))
    {
      execute_commands(fst_cntxt);
      old_interface_input_file(prev, &buffer);
      if (too_many_aliases) wait_for_END(); /* this is set on/off
					       in iy_execute_alias */
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}

void iy_source(ifile *file, FST_CNTXTptr fst_cntxt)
{
  finfo buffer, *prev;


  discard_any_old_script();


  if ((prev = new_interface_input_file(file, &buffer, 0)))
    {
      execute_commands(fst_cntxt);

      old_interface_input_file(prev, &buffer);
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}


int interface_yacc_lex__change_token_for_define(void)
/* Hack to force the parser of interface (aka command) to forget about a possible
   look-ahead symbol, in the context of "define" processing. Otherwise it would
   not pass on strings (aimed to be regular expressions), that look like xfst
   commands,  to its slave, the regular expression handler (parser and lexer) but
   rather it would run into a parsing error.
   */
{
  int token_type_changed=0;
  if (yychar != EOL)
    {
      if (yychar != SOMETHING_ELSE)
	{
	  token_type_changed=yychar;
	  /* fprintf (stderr, "DEBUG interface: command_char changed from %d to:", (int) command_char); **/
	  yychar = SOMETHING_ELSE; /* Forcing it to take anything as regexp */
	  /* fprintf (stderr, "%d.\n", (int) yychar); 
	  fflush(stderr); */
	}
    }
  return token_type_changed;
}

/****************************************************************
  COMMAND: regex
 ****************************************************************/

static void regex_to_stack(FST_CNTXTptr fst_cntxt)
{
  NETptr net;
  char name[1024] = "";
  int i;
  /* int debug_iy_name_nets = IY_NAME_NETS; */
  /* regex_name(((print_prompt > 0) || IY_NAME_NETS ? name : NULL)); */
  regex_name(( IY_NAME_NETS ? name : NULL));
  /* name nets if... */

  IY_LAST_ERRORS = 0;

 if (regex_on_command_line)
   {
     set_unput_allowed (FALSE);  /** 99-5-20 */
     wait_for_regex();
     /* switch to input from memory for regex (F)LEX: the intercepted
	interface/command buffer is processed first.
	Then switch back to stdin. */
   
     regex_parse(fst_cntxt);
     set_unput_allowed (TRUE); /** 99-5-20 */
     net = last_regex();

     if (net && *name)
       {
	 if (IY_MINIMIZE_P)
	   min_fsm (net, DONT_COPY); /** was:** cleanup_fst(net, DONT_COPY, DO_MINIMIZE); */

	 /* Remove the trailing semicolon if any */

	 i = strlen(name);
	 while (i > 0)
	   {
	     i--;
	     if (!isspace((int) name[i]))
	       {
		 if (name[i] == ';')
		   name[i] = '\0';
		 break;
	       }
               
	   }
	 name_network(net, name);
	 *name = '\0';
       }

     iy_net_to_stack(net);
     regex_cl_length_processed = regex_length_processed();

     if (regex_cl_length_processed == 0) /*** 2001-7-4 was it a read from file: " < my.regex.file"  */
       {
	 regex_cl_length_processed = regex_file_symbol_length; 
	 regex_file_symbol_length = 0;
       }
     else
       interface_file_info->line_number = regex_file_info->line_number;

	 
     regex_tg_flush_buffer();
     regex_on_command_line = FALSE;
   }
 else
   {
     wait_for_regex();

     while (regex_parse(fst_cntxt))
       {
	 net = last_regex();
	 interface_file_info->line_number = regex_file_info->line_number;
	 if (net && *name)
	   {
	      if (IY_MINIMIZE_P)
		min_fsm (net, DONT_COPY); /** was:** cleanup_fst(net, DONT_COPY, DO_MINIMIZE); */

	     /* Remove the trailing semicolon if any */

	     i = strlen(name);
	     while (i > 0)
	       {
		 i--;
		 if (!isspace((int) name[i]))
		   {
		     if (name[i] == ';')
		       name[i] = '\0';
		     break;
		   }
               
	       }
	     name_network(net, name);
	     *name = '\0';
	   }
	 iy_net_to_stack(net);
       }
   }
  if (IY_LAST_ERRORS)
    {
      warn_errors(IY_LAST_ERRORS);
      quit_on_fail_error(IY_QUIT_ON_FAIL);
    }

  regex_name(NULL); /* not to confuse reader when "name[]" disappears */
}

static void regex_from_file(ifile *file, FST_CNTXTptr fst_cntxt)
{
  finfo buffer, *prev;
  int num_nets;
  int result;

  one_liner=one_regex=0;

  if ((prev=new_re_input_file(file, &buffer)))
    {
      IY_LAST_ERRORS = 0;
      wait_for_regex();
      
      do {
	/* If the file contains more than one regular expression,
	   compile all of them but stop on error.
	*/
	num_nets = iy_stack_length();

	result = regex_parse(fst_cntxt);

	if (IY_LAST_ERRORS)
	  quit_on_fail_error(IY_QUIT_ON_FAIL);
	else if (result)
	  {
	    iy_net_to_stack(last_regex());

	    interface_file_info->line_number =
	      regex_file_info->line_number;

	  }
      } while (iy_stack_length() > num_nets);

      regex_cl_length_processed = regex_file_symbol_length; 
      regex_file_symbol_length = 0;

      regex_tg_flush_buffer();

      old_re_input_file(prev, &buffer);
    }
  else
    quit_on_fail_error(IY_QUIT_ON_FAIL);
}

/****************************************************************
  COMMAND: system, cd, dir
 ****************************************************************/
static void call_system(char *line)
{
  if (system(line) && errno)
  {
      perror(line);
      quit_on_fail_error(IY_QUIT_ON_FAIL);
  }
}
static void pwd()
{
  if (system("pwd") && errno)
  {
      perror(last_command->command);
      quit_on_fail_error(IY_QUIT_ON_FAIL);
  }
}
static void cd(char *newdir)
{
  if (chdir(newdir))
  {
      perror(newdir);
      quit_on_fail_error(IY_QUIT_ON_FAIL);
  }
  else if (IY_VERBOSE) pwd();
}
static void dir(char *line)
{
  char buf[200];
  strcpy(buf,"ls -F ");
  strncpy(buf+6,line,190);
  system(buf);
}

/****************************************************************
  COMMAND: no command; auxiliary
 ****************************************************************/
/****************************************************************
  STORE STRING
 ****************************************************************/
static char *store_text(char *str)
{
  yy_buf_n = 1 - yy_buf_n;
  strcpy((char *) yy_buf[yy_buf_n],str);
  return (char *) yy_buf[yy_buf_n];
}
#if NOT_USED
static char *store_yytext()
     /* This is tricky; we use here interface_text (yytext) and interface_leng (yyleng).
	Be sure that they have appropriate values.  */
{
  yy_buf_n = 1 - yy_buf_n;
  if (interface_leng >= YY_BUF_LEN)
    {
      strncpy((char *) yy_buf[yy_buf_n], interface_text, YY_BUF_LEN-1);
      yy_buf[yy_buf_n][YY_BUF_LEN-1]='\0';
    }
  else
    {
      strcpy((char *) yy_buf[yy_buf_n], interface_text);
    }
  return (char *) yy_buf[yy_buf_n];
}
#endif /* NOT_USED */

/* Hack to perform real iso_latin_1 printable character checking.
   Languages like French, German, Swedish and others need this! TG */
/* #define	is_iso_latin_1_upperhalf(c) ((c) > 160) */
/***
#define	is_iso_latin_1_upperhalf(c) ( ( (c ) >>7 ) && ( (c) >> 5))
#define	isgraph_iso_latin_1(c)       (isgraph(c) || is_iso_latin_1_upperhalf(c))
***/

static char *strip_yytext()
{
  unsigned char *to, *from;
  int len;

  yy_buf_n = 1 - yy_buf_n;
  from=(unsigned char *)interface_text; to=yy_buf[yy_buf_n]; len=YY_BUF_LEN-1;

  /* Remove leading non-graphical characters. */
  while (*from && !isgraph_iso_latin_1(*from)) from++;

  /* Copy interface_text & remove %-signs. */
  while (*from && (--len>=0))
    {
      /* if ((from[0] == '%') && from[1]) from++; */
      *to++ = *from++;
    }
  *to = '\0';

  /* Remove trailing non-graphical characters */
  while ((--to >= yy_buf[yy_buf_n]) && !isgraph_iso_latin_1(*to)) *to = '\0';

  /* Handle ""-marks */
  if ((yy_buf[yy_buf_n][0] == '"') && (*to == '"'))
    {
      *to = '\0';
      return (char *) yy_buf[yy_buf_n]+1;
    }
  else
    return (char *) yy_buf[yy_buf_n];
}

void fputs_interface_text(void)
{
  /* Make sure the text ends with a linefeed */

  int len = strlen(interface_text);
  char c  = interface_text[len-1];
  if (c == '\r')
    {
      interface_text[len-1] = '\n';
      fputs(interface_text,stdout);
      interface_text[len-1] = '\r';
    }
  else
    fputs(interface_text,stdout);
}


/***************************************************************
  OPEN/CLOSE FILE
 ****************************************************************/
static char *open_file;

static FILE *open_w_file(char *name)
{
  FILE *f;
  if (!(f=fopen(name,"wb")))
    perror(name);
  else
    open_file = name;

  if (IY_VERBOSE)
    fsm_fprintf(stdout, "Opening '%s'\n", open_file);

  return f;
}
static void loclose(FILE *f)
{
  if (f == pipe_file)
    {
      pclose(f);
      pipe_file = NULL;
    }
  else if (!((f == stdin) ||
	     (f == stdout) ||
	     (f == stderr) ||
	     (f == NULL)))
    {
      if (IY_VERBOSE)
	fsm_fprintf(stdout, "Closing '%s'\n", open_file);

    fclose(f);
}
}

void yacc_restart()
{
  if (pipe_file)
    {
      pclose(pipe_file);
      pipe_file = NULL;
    }
}

/****************************************************************
  END.
 ****************************************************************/
