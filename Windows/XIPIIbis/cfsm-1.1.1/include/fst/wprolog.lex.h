/* Copyright (c) 2001 by Xerox Research Centre.  All rights reserved. */
/* Created by Tamas Gaal */
/* $Id: wprolog.lex.h,v 1.1 2001/11/19 16:34:48 gaal Exp $ */


#ifdef __cplusplus
extern "C" {
#endif

   void wprolog_total_restart();
   int wprolog_lex(void);
   int wprolog_error(char *str);
   char* wprolog_text;

#ifdef __cplusplus
}
#endif
