typedef union {
  WEIGHTED_NETWORK      *network;
  char                  *string;
  int			integer;
  int                   weight;
} YYSTYPE;
#define	ZERO	257
#define	QMARK	258
#define	SYMBOL	259
#define	QUOTED_SYMBOL	260
#define	STRING_QUOTED_SYMBOL	261
#define	W_STRING	262
#define	PL_NETWORK	263
#define	PL_START	264
#define	PL_ARC	265
#define	PL_FINAL	266
#define	PL_SYMBOL	267
#define	PL_SEMIRING	268
#define	LEFT_P	269
#define	RIGHT_P	270
#define	COMMA	271
#define	COLON	272
#define	PERIOD	273
#define	END_OF_FILE	274
#define	LEFT_A_B	275
#define	RIGHT_A_B	276


extern YYSTYPE wprolog_lval;
