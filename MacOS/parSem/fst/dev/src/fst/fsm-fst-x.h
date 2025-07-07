/* $Id: fsm-fst-x.h,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
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
extern int FstCalcExport * (* fst_IY_QUIT_ON_FAIL)();
extern int FstCalcExport * (* fst_IY_COMPOSE_FLAG_AS_SPECIAL)();
extern int FstCalcExport * (* fst_IY_INTERACTIVE)();
extern int FstCalcExport * (* fst_IY_VERBOSE)();
extern int FstCalcExport * (* fst_IY_NAME_NETS)();
extern int FstCalcExport * (* fst_IY_LAST_ERRORS)();
extern int FstCalcExport * (* fst_IY_LEX_ERRORS)();
extern int FstCalcExport * (* fst_IY_RECURSIVE_DEFINE)();
extern int FstCalcExport * def_LAST_ERRORS();
extern int FstCalcExport * (* fst_IY_MINIMIZE_P)();
