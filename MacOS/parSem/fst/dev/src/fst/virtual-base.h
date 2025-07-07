/* $Id: virtual-base.h,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */ 
/* Used to be called virtual-fsm-base.h Name shortened to stay within
   the 15 character limit on filenames that affect the 'ar' command
   in some systems such as Solaris where 'ar -xv libfsmbase.a' doesn't
   work on longer filenames. */
/* Copyright (c) 2001 by the Xerox Corporation.  All rights reserved */ 

/****************************************************** 
** 
**  virtual-base.h
** 
**  Caroline Privault, XRCE, January 2002
** 
**  Low level functions (from J. Maxwell: moved from 
**  file "virtual-fsm.h") that are needed in the BASE 
**  library, in both WORLD and NON-WORLD mode.
** 
******************************************************/ 


#ifndef C_FSM_VIRTUAL_FSM_BASE
#define C_FSM_VIRTUAL_FSM_BASE


#ifdef __cplusplus 
extern "C" { 
#endif 

int FstBaseExport dead_state_p(STATEptr state);
    
/* RMK: Rewriting any algorithm (lookup, lookdown...) that fetches 
   a state's arcset field so that it calls VARCSET instead will make 
   that algorithm also work on virtual machines. */
ARCptr FstBaseExport virtual_arc_set(STATEptr vstate);


/* RMK: Rewriting any algorithm (lookup, lookdown...) that fetches a 
   state's final field so that it calls virtual_final_p instead will make 
   that algorithm also work on virtual machines.   Note that is OK 
   to fetch final directly if you are sure that virtual_arc_set or 
   virtual_final_p were previously called. */
int FstBaseExport virtual_final_p(STATEptr vstate);



#ifdef __cplusplus 
} 
#endif 

#endif /* C_FSM_VIRTUAL_FSM_BASE */
