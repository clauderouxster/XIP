#ifndef BISON_XEROX_FST_REGEX_TAB_H
# define BISON_XEROX_FST_REGEX_TAB_H

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


extern YYSTYPE regex_lval;

#endif /* not BISON_XEROX_FST_REGEX_TAB_H */
