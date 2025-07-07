/* $Id: error.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

/****
 *
 *	ERROR.H
 *
 ****/

#ifndef C_FSM_ERROR

#define C_FSM_ERROR

#include "fst/fst_windefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*  LIST of ERROR CODES RETURNED by FUNCTIONS  */
/*  CP: 30/04/99: LABELMAP EXTENSION           */
/* ******************************************* */

/* Removes MS-VC warning */
#ifdef NO_ERROR
#undef NO_ERROR
#endif
#ifdef ERROR
#undef ERROR
#endif

#define NO_ERROR 0
#define ERROR 1
#define ERR_NOLABEL 2
#define ERR_BAD_INPUT 3
#define ERR_IN_PROPERTY 4
#define ERR_NO_SPACE_LEFT 5
#define ERR_NO_SPACE_IN_HASHTABLE 6
#define ERR_MAX_LABELS_REACHED 7
#define ERR_ID_FLAG_VALUE_TOO_BIG 8
#define ERR_FILE_OPENING 9
#define ERR_WRONG_FILE_TYPE 10
#define ERR_OPERATION_NOT_ALLOWED 11
#define ERR_INVALID_LICENSE_TYPE 12

/* ****************************************** */


enum report_error_p {DONT_REPORT_ERROR, DO_REPORT_ERROR};
extern void (*fsm_error_function)(char *message, char *function_name, int code);

void FstBaseExport handle_error (char *message, char *function_name, int code);
void FstBaseExport handle_warning (char *message, char *function_name, int code);
void FstBaseExport not_enough_memory(char *function_name);
void FstBaseExport exit_with_error(char *message);
void FstBaseExport quit_on_fail_error(int quit_p);
void FstBaseExport check_for_negative_or_zero(int length, char *function);
void FstBaseExport not_applicable_to_virtual_net(int quit_p);
#ifdef NEED_STRERROR
extern char *strerror(int err_num);
#endif /* NEED_STRERROR */

void FstBaseExport print_comment (int verbose_p, char *format, ...);
void FstBaseExport prog_error (char *func, char *format, ...);
char FstBaseExport *error_buffer (char *action, char *format, ...);

enum continue_or_stop {CONTINUE, STOP};
extern int FstBaseExport CONTINUE_OR_STOP;


#ifdef __cplusplus
}
#endif

#endif /* C_FSM_ERROR */

