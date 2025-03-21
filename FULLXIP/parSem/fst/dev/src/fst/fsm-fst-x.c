/* $Id: fsm-fst-x.c,v 1.4 2005/03/04 15:45:23 grondeau Exp $ */
/*************************************************************
 *
 *  FILE  "fsm-fst-x.c"
 *
 *      Passing fst-specific variables to fsm libraries
 *  Tamas Gaal, XEROX XRCE, Grenoble, France
 *  12/97
 *
 *************************************************************/

#include "fst/types.h"
#include "fst/fsm-fst-x.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 

int * def_ASSERT()
{
  static int Default_ASSERT = FALSE;
  return ( & Default_ASSERT);
}

int * def_QUIT_ON_FAIL()
{
  static int Default_QUIT_ON_FAIL = FALSE;
  return ( & Default_QUIT_ON_FAIL);
}

int * def_COMPOSE_FLAG_AS_SPECIAL()
{
  static int Default_COMPOSE_FLAG_AS_SPECIAL = FALSE;
  return ( & Default_COMPOSE_FLAG_AS_SPECIAL);
}

int * def_INTERACTIVE()
{
  static int Default_INTERACTIVE = TRUE;
  return ( & Default_INTERACTIVE);
}

int * def_VERBOSE()
{
  static int Default_VERBOSE = TRUE;
  return ( & Default_VERBOSE);
}

int * def_NAME_NETS()
{
  static int Default_NAME_NETS = FALSE;
  return ( & Default_NAME_NETS);
}

int * def_LAST_ERRORS()
{ 
  static int Default_LAST_ERRORS = 0;
  return ( & Default_LAST_ERRORS);
}

int * def_LEX_ERRORS()
{ 
  static int Default_LEX_ERRORS = 10;
  return ( & Default_LEX_ERRORS);
}

int * def_RECURSIVE_DEFINE()
{
  static int Default_RECURSIVE_DEFINE = 0;
  return ( & Default_RECURSIVE_DEFINE);
}

static int * def_MINIMIZE_P()
{
  static int Default_MINIMIZE_P = TRUE;
  return ( & Default_MINIMIZE_P);
}

int  * (* fst_IY_ASSERT)() = def_ASSERT ;
int  * (* fst_IY_QUIT_ON_FAIL)() = def_QUIT_ON_FAIL ;
int  * (* fst_IY_COMPOSE_FLAG_AS_SPECIAL)() = def_COMPOSE_FLAG_AS_SPECIAL ;
int  * (* fst_IY_INTERACTIVE)() = def_INTERACTIVE ;
int  * (* fst_IY_VERBOSE)() = def_VERBOSE ;
int  * (* fst_IY_NAME_NETS)() = def_NAME_NETS ;
int  * (* fst_IY_LAST_ERRORS)() = def_LAST_ERRORS ;
int  * (* fst_IY_LEX_ERRORS)() = def_LEX_ERRORS ;
int  * (* fst_IY_RECURSIVE_DEFINE)() = def_RECURSIVE_DEFINE ;
int  * (* fst_IY_MINIMIZE_P)() = def_MINIMIZE_P ;

#endif /* SLIM_FST */
