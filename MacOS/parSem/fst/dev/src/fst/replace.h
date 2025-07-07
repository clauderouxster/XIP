/* $id: replace.h,v 1.4 1998/12/03 10:41:42 gaal Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/****************************************
 *
 *  FUNCTIONS TO HANDLE CONTEXT BOUNDARIES IN REPLACEMENT OPERATIONS
 *
 *    e.g.:  a -> b || # _ y ;
 *
 ***********************************/


#ifndef REPLACE_H
#define REPLACE_H
/*--------------------------------------------------------*/

extern void (*applic_preprocess_uplow)(RSptr uplow);
extern void (*applic_preprocess_leftright)(NVptr leftright);

NETptr simple_restriction(NETptr center, NETptr left, NETptr right,
                          int arrow_type, int copy_p);

NETptr simple_replacement(NETptr upper, NETptr lower, int copy_p,
			  int optional_p);

/*--------------------------------------------------------*/

RSptr rs_upper_lower (NETptr upper, NETptr lower, int arrow);
NETptr rs_left_right(NETptr left, NETptr right);
RSptr crossprod_uplow_leftright
  (RSptr uplow, NVptr leftright, int ctxdir, int reclaim_p);

#define NET_single_epsilon(N)   NET_mark(N)

NETptr rs_set_single_epsilon (NETptr net);
NETptr rs_reset_single_epsilon (NETptr net);

#define UPPER_NV(X)      NVV_nv(X, 0)
#define LOWER_NV(X)      NVV_nv(X, 1)
#define LEFT_CTX_NV(X)   NVV_nv(X, 2)
#define RIGHT_CTX_NV(X)  NVV_nv(X, 3)
#define LEFT_BR_NV(X)    NVV_nv(X, 4)
#define RIGHT_BR_NV(X)   NVV_nv(X, 5)

#define LEFT_BR_IDS(X)   (X)[0]
#define RIGHT_BR_IDS(X)  (X)[1]
#define LEFT_BR_ID(X,Y)  (X)[0][(Y)]
#define RIGHT_BR_ID(X,Y) (X)[1][(Y)]

NETptr uncond_repl_unidir(NVptr upper, NVptr lower, int optional);
NETptr uncond_repl_bidir(NVptr upper, NVptr lower, int optional);
NETptr uncond_replace(RSptr uplow, int reclaim_p);

NETptr cond_repl_unidir (RSptr repl_rs, int upwards_p, int depth);

NETptr conditional_replace (RSptr repl_rs, int reclaim_p);

/*
NETptr CPDR_cond_determ_replace (RSptr repl_rs);
*/

#define EPS_SINGLE_NAME    "**EPS-SINGLE**"
#define EPS_MULTI_NAME     "**EPS-MULTIPLE**"
#define STRING_BEGIN_NAME  "**STR-BEGIN**"
#define STRING_END_NAME    "**STR-END**"

#define APPLIC_LEFT        "**APPLIC-LEFT**"
#define APPLIC_RIGHT       "**APPLIC-RIGHT**"

#define MAX_RECURSION      20

#define INSERT_STRING "..."

NETptr RST_restrict_in_multctx
  (NETptr center, NVptr ctxlist, int arrow, int reclaim_p);

/*--------------------------------------------------------*/
#endif /* REPLACE_H */

