/* Copyright (c) 1998-99 by Xerox Research Centre Europe.  All rights reserved. */
/* Created by Tamas Gaal. */
/* $Id: interface.lex.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */

/*
#define	AND	'&'
#define	AT_SIGN	'@'
#define	B_QUOTE	'`'
#define	B_SLASH	'\\'
#define	COLON	':'
#define	COMMA	','
#define	EQUALS	'='
#define	LEFT_A	'<'
#define	LEFT_B	'['
#define	LEFT_C	'{'
#define	LEFT_P	'('
#define	LINE	'_'
#define	OR	'|'
#define	PERIOD	'.'
#define	PLUS	'+'
#define	QMARK	'?'
#define	RIGHT_A	'>'
#define	RIGHT_B	']'
#define	RIGHT_C	'}'
#define	RIGHT_P	')'
#define	SEMIC	';'
#define	SLASH	'/'
#define	STAR	'*'
#define	TILDE	'~'
#define	UP	'^'
#define	ZERO	'0'
*/

int yylex(void);
char *interface_last_lex_string(void);

void FstCalcExport interface_lex_total_restart();
finfo *interface_file_info;
void wait_for_command(void);
void wait_for_parameter(void);
void wait_for_EOL(void);

void wait_for_sym(void);

int wait_for_confirm(void);
int FstCalcExport wait_for_confirm2(void);
int read_variable(void);
void read_semic_as_eol(void);
int read_bar_command(void);
char *read_alias_name(void);
int read_parameters(char *pars[], int);

char *lookup_line(void);
char *read_line(void);
char *more_inputline(void);
void interface_clean_inputline(void);

#define yylex interface_lex
/* #define yytext interface_text */
/* #define yyleng interface_leng */
/****
extern char *yytext;
extern int yyleng;
****/

char * interface_lex_buffer();
char * interface_lex_char_buffer();

void FstCalcExport start_flex(FILE *);
int interface_lex_read_from_string(char *str);
int interface_lex_consume(char *str,  int le);
void interface_unput (char value);
void interface_teminate();
void interface_debug_clean_lex_buffer();
void interface_debug_clean_buf_string(char *str);


finfo * read_regex_immediate_action(int * regex_on_command_line, 
				    FST_CNTXTptr fst_cntxt, 
				    int token_type_changed_for_define, 
				    int * length_of_read_regex_from_file_string);
void read_regex_post_action(int regexp_leng_processed, 
			    FST_CNTXTptr fst_cntxt, 
			    finfo *re_old_buffer, 
			    finfo *re_actual_buffer);
extern int interface_fake_eol;

void interface_restart_if_necessary();
int interface_lex(void);

finfo *new_interface_input_file(ifile *, finfo *, long);
int old_interface_input_file(finfo * to_be_restored_finfo, finfo * actual_finfo);

long interface_file_line(void);
char *interface_file_name(void);
int interface_file_type(void);
void increment_interface_line_count(void);
int interface_yacc_lex__change_token_for_define(void);

