/* $Id: prompt.h,v 1.4 2005/03/04 15:45:25 grondeau Exp $ */
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

extern char FstBaseExport *word; 
extern char FstBaseExport CURRENT_FILE_NAME[];
extern int FstBaseExport FOPEN_ERROR;
extern FILE FstBaseExport *SCRIPT_INFILE;
extern FILE FstBaseExport *SCRIPT_OUTFILE;
extern char FstBaseExport SCRIPT_INFILE_NAME[];
extern char FstBaseExport SCRIPT_OUTFILE_NAME[];

char *get_line(char *prompt);
void flush_get_input_buffer(void);
char FstBaseExport * fget_input(FILE *stream, char *prompt, int insist_p, char *limiters);
#define get_input(P, I, L) fget_input(stdin, (P), (I), (L))
char FstBaseExport * get_more(void);
int FstBaseExport universal_read_line(char **buffer, int buf_len, FILE *stream);
int FstBaseExport yes_or_no (char *question, char default_choice);

FILE FstBaseExport * fquery_fopen(char *mode, FILE *stream, int cr_quit_p);
#define query_fopen(mode) fquery_fopen((mode), stdin, FALSE)
#define maybe_fopen(mode) fquery_fopen((mode), stdin, TRUE)

#ifdef __cplusplus
}
#endif

#endif
