/* Copyright (c) 2001 by Xerox Research Centre.  All rights reserved. */
/* Created by Tamas Gaal */
/* $Id: wprolog.lex.h,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */


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
