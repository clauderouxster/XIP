/* $Id: fsm-fst-x.h,v 1.9 2002/03/20 14:03:41 jmcoursi Exp $ */
/*************************************************************
 *
 *  FILE  "fsm-fst-x.h"
 *
 *      Passing fst-specific variables to fsm libraries
 *  Tamas Gaal, XEROX XRCE, Grenoble, France
 *  12/97
 *
 *************************************************************/

extern int  * (* fst_IY_ASSERT)();
extern int FsmFstExport * (* fst_IY_QUIT_ON_FAIL)();
extern int FsmFstExport * (* fst_IY_COMPOSE_FLAG_AS_SPECIAL)();
extern int FsmFstExport * (* fst_IY_INTERACTIVE)();
extern int FsmFstExport * (* fst_IY_VERBOSE)();
extern int FsmFstExport * (* fst_IY_NAME_NETS)();
extern int FsmFstExport * (* fst_IY_LAST_ERRORS)();
extern int FsmFstExport * (* fst_IY_LEX_ERRORS)();
extern int FsmFstExport * (* fst_IY_RECURSIVE_DEFINE)();
extern int FsmFstExport * def_LAST_ERRORS();
extern int FsmFstExport * (* fst_IY_MINIMIZE_P)();
