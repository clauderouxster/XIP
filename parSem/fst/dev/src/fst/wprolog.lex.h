/* Copyright (c) 2001 by Xerox Research Centre.  All rights reserved. */
/* Created by Tamas Gaal */
/* $Id: wprolog.lex.h,v 1.5 2006/06/09 12:54:59 grondeau Exp $ */


#ifdef __cplusplus
extern "C" {
#endif

   void wprolog_total_restart();
   int wprolog_lex(void);
   int wprolog_error(char *str);
   extern char* wprolog_text;

#ifdef __cplusplus
}
#endif
