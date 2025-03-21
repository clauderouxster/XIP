/* $Id: str-buf.h,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */

#include "fst/fat-str.h"

/******************************************************
 **
 **  STR-BUF.H
 **
 **  Lauri Karttunen, XRCE, February 1999
 **
 **  Utility for managing strings.
 **
 ******************************************************/

#ifndef C_FSM_STR_BUF

#define C_FSM_STR_BUF

#ifdef __cplusplus
extern "C" {
#endif

/******************
 *  String buffer object  *
 ******************/

typedef struct STRING_BUFFER {
  int length;
  int pos;
  char *string;
} STRING_BUFFERtype, *STRING_BUFFERptr;

#define STRING_BUFFER_length(X)	(X)->length
#define STRING_BUFFER_pos(X)	(X)->pos
#define STRING_BUFFER_string(X)	(X)->string

STRING_BUFFERptr FstBaseExport make_string_buffer(int length);
void FstBaseExport free_string_buffer(STRING_BUFFERptr str_buf);
void FstBaseExport initialize_string_buffer(STRING_BUFFERptr str_buf);

int FstBaseExport append_char_to_buffer(char c, STRING_BUFFERptr str_buf);
int FstBaseExport append_string_to_buffer(char *str, STRING_BUFFERptr str_buf);
int FstBaseExport print_string_buffer(STRING_BUFFERptr str_buf, FILE *stream);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_STR_BUF */
