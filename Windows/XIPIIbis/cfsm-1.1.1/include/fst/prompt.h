/* $Id: prompt.h,v 1.4 2002/04/24 12:23:27 jmcoursi Exp $ */
/******************************************************
 **
 **  PROMPT.H
 **
 **  Todd Yampol, Xerox PARC, November 1992
 **
 ******************************************************/

#ifndef C_FSM_PROMPT

#define C_FSM_PROMPT

#ifdef __cplusplus
extern "C" {
#endif

extern char FsmFstExport *word; 
extern char FsmFstExport CURRENT_FILE_NAME[];
extern int FsmFstExport FOPEN_ERROR;
extern FILE FsmFstExport *SCRIPT_INFILE;
extern FILE FsmFstExport *SCRIPT_OUTFILE;
extern char FsmFstExport SCRIPT_INFILE_NAME[];
extern char FsmFstExport SCRIPT_OUTFILE_NAME[];

char *get_line(char *prompt);
void flush_get_input_buffer(void);
char FsmFstExport * fget_input(FILE *stream, char *prompt, int insist_p, char *limiters);
#define get_input(P, I, L) fget_input(stdin, (P), (I), (L))
char FsmFstExport * get_more(void);

int FsmFstExport yes_or_no (char *question, char default_choice);

FILE FsmFstExport * fquery_fopen(char *mode, FILE *stream, int cr_quit_p);
#define query_fopen(mode) fquery_fopen((mode), stdin, FALSE)
#define maybe_fopen(mode) fquery_fopen((mode), stdin, TRUE)

#ifdef __cplusplus
}
#endif

#endif
