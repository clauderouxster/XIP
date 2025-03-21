#ifndef BISON_PROLOG_TAB_H
# define BISON_PROLOG_TAB_H

#ifndef YYSTYPE
typedef union {
  struct NETWORK        *network;
  char                  *string;
  int			integer;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	ZERO	257
# define	QMARK	258
# define	SYMBOL	259
# define	QUOTED_SYMBOL	260
# define	PL_NETWORK	261
# define	PL_START	262
# define	PL_ARC	263
# define	PL_FINAL	264
# define	PL_SYMBOL	265
# define	LEFT_P	266
# define	RIGHT_P	267
# define	COMMA	268
# define	COLON	269
# define	PERIOD	270
# define	END_OF_FILE	271


extern YYSTYPE prolog_lval;

#endif /* not BISON_PROLOG_TAB_H */
