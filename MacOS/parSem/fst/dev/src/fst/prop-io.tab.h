#ifndef BISON_PROP_IO_TAB_H
# define BISON_PROP_IO_TAB_H

#ifndef YYSTYPE
typedef union {
    int integer;
/*    char character;
    char *string;
*/
    PROPptr property;
    OBJECTptr object;
    IO_SYMBOLptr symbol;
    SEQUENCEptr array;
    SEQUENCEptr list;
    SEQUENCEptr sequence; 
    FAT_STR fat_string;
    FAT_CHAR fat_character;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	PR_PROPNAME	257
# define	STRING_ERROR	258
# define	CHARACTER_ERROR	259
# define	SYMBOL_ERROR	260
# define	UNKNOWN_ERROR	261
# define	INTEGER	262
# define	CHARACTER	263
# define	STRING	264
# define	SYMBOL_PACKAGE	265
# define	SYMBOL_NAME	266
# define	PR_START_LIST	267
# define	PR_END_LIST	268
# define	PR_START_ARRAY	269
# define	PR_END_ARRAY	270
# define	PR_SEQ_SEPARATOR	271
# define	PR_ERROR	272
# define	QUIT	273


extern YYSTYPE propio_lval;

#endif /* not BISON_PROP_IO_TAB_H */
