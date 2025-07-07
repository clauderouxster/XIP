/* Copyright (c) 1994 by Rank Xerox Research Centre.  All rights reserved. */
/* Created by Pasi Tapanainen. */
/* $Id: re.lex.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */

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
char *last_lex_string(void);

void recover_regex_error(void);
int clean_regex(void);
void stop_regex(void);
void re_lex_total_restart(void);
void regex_restart_if_necessary(void);
int lex_read_from_string(char *);
int regex_lex_tg_read_from_string(char *str);
void old_re_lex_buffer(finfo * old_file_info_buffer, finfo * actual_file_info_buffer);
finfo *new_re_lex_buffer(finfo *fi, FILE *f, ifile *file, int line_offset);
finfo *new_re_input_file(ifile *, finfo *);
int old_re_input_file(finfo * buffer, finfo * actual_buffer);
finfo *new_re_string_buffer(finfo *fi, ifile *file);
void regex_name(char *);

void wait_for_regex(void);
void FstCalcExport wait_for_END(void);

int read_regex_help(void);

extern finfo *regex_file_info;
extern int one_liner;
extern int one_regex;
extern FstCalcExport int print_prompt;
extern int remember_to_clean;
extern int regex_fake_eol;

/* char *regex_text; */
/* #define yytext regex_text */

/* extern char *yytext; */
extern int yyleng;
extern char lost_char;

FILE * re_lex_infile();
void regex_tg_restart(FILE * ff, int from_initial_regex_state);
void regex_tg_restart_with_init(FILE * ff, void * interface_master_current_buffer);

void regex_lex_yyless(int n);
void change_lex_state_to_REGEXP (void);

void re_nl (void);
int set_unput_allowed(int true_or_false); /*returns the old value */
void set_warn_missing_semic_EOF_allowed(int true_or_false);

void FstCalcExport print_a_prompt(char *);

void regex_lex_buf_restart(char * buf);
void regex_tg_flush_buffer();
FstCalcExport void start_regex_flex(FILE *f);
int regex_length_processed(void);
/* finfo *regex_file_info; */
char * re_yy_current_buffer__yy_ch_buf();

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif
