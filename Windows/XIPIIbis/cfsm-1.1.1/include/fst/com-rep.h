/* $Id: com-rep.h,v 1.5 2002/03/19 01:09:11 lk Exp $ */
/* Copyright (c) 1997 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  COM-REP.H
 **
 **  Lauri Karttunen, XRCE, December 1997
 **
 **  Function that compiles a regular expressions in a network and replaces
 **  the original by the result of the compilation. The main intented
 **  application: compiling Arabic lexical forms such as
 **        [ k t p .m>. C V C V V C .<m. i a* ]
 **  to yield a transducer that maps it to the surface form
 **          k i t a a b
 **
 ******************************************************/

NETptr compile_and_replace_self(NETptr net, int side, int reclaim_p, int min_p, int retokenize_p);

NETptr extract_compile_replace(NETptr net, int side, int reclaim_p,
			       int minimize_p, int retokenize_p);
