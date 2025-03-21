/* $Id: prop-io-lex.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
/* Copyright (c) 1994  by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  PROP-IO-LEX.H
 **
 **  Herve POIRIER RXRC 1994
 **  
 ** Include file for the prop-io grammar and lexicon files.
 **
 ******************************************************/

#define yyparse propio_parse
#define yylex propio_lex
#define yyerror propio_error
#define yylval propio_lval
#define yychar propio_char
#define yydebug propio_debug
#define yyin propio_in
#define yyout propio_out
#define yy_init_buffer propio_init_buffer
#define yy_create_buffer propio_create_buffer
#define yy_load_buffer propio_load_buffer
#define yyrestart propio_restart
#define yy_switch_to_buffer propio_switch_to_buffer
#define yy_delete_buffer propio_delete_buffer
#define yy_load_buffer_state propio_load_buffer_state
#define yyleng propioleng
#define yynerrs propionerrs
#define yytext propiotext
#define yylloc propiolloc

int propio_lex(void);
