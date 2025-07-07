/* $Id: re-lex.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1994  by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  RE-LEX.H
 **
 **  Herve POIRIER, RXRC 1994
 **  
 **  Definitions for the REGEX grammar and lexicon C files.
 **
 ******************************************************/

#define yyparse regex_parse
#define yylex regex_lex
#define yyerror regex_error
#define yylval regex_lval
#define yychar regex_char
#define yydebug regex_debug
#define yyin regex_in
#define yyout regex_out
#define yy_init_buffer regex_init_buffer
#define yy_create_buffer regex_create_buffer
#define yy_load_buffer regex_load_buffer
#define yyrestart regex_restart
#define yy_switch_to_buffer regex_switch_to_buffer
#define yy_delete_buffer regex_delete_buffer
#define yy_load_buffer_state regex_load_buffer_state
#define yyleng regexleng
/* #define yynerrs regexnerrs */
extern char * regex_text;
/* IF  %array is used in re.template.lex THEN
   char regex_text[];
   */
/* #define yytext regex_text */
#define yy_scan_string regex_scan_string

int regex_lex(void);

