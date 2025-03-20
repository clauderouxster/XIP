typedef union {
  NETptr                net;
  char			*string;
  FAT_STR               fstring;
  int			integer;
} YYSTYPE;
#define	DEFINITIONS	257
#define	EQUALS	258
#define	QMARK	259
#define	ZERO	260
#define	COLON	261
#define	LEFT_A	262
#define	RIGHT_A	263
#define	SYMBOLS_TOKEN	264
#define	LEXICON	265
#define	END	266
#define	SYMBOL	267
#define	RE_STRING	268
#define	QUOTED_SYMBOL	269
#define	SEMIC	270


extern YYSTYPE lexicon_lval;
