/* $Id: tk-inspect.c,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* (c) 1996-2001 by the Xerox Corporation.  All rights reserved */

/* Most of this code comes from John Maxwell's inspector for the XLE data
   structures. I preserved all the code that is relevant for the c-fsm
   structures and removed everything else. The get_tcl_interpreter() routine
   is a stripped-down version of Tcl_Main() in tclMain.c.  LK  2001-02-16
*/
#ifdef HAVE_TCL /* ignore this file on Windows */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <tcl.h>
#include <tk.h>
#include "fst/types.h"
#include "fst/struct.h"
#include "fst/virtual-fsm.h"
#include "fst/virtual-base.h"
#include "fst/tk-inspect.h"
#include "fst/tk-commands.h"
#include "fst/utf8_fatstr_util.h"

/* ------------------------------------------------------------------------- */

/* The following macros are used to make it easier to translate between C */
/* pointers and their C-FSM representation: (Type)address.  They assume that */
/* there are locally defined variables named type_str, interp, and buffer, */
/* where type_str holds the Type string, interp is the Tcl_Interpreter, and */
/* buffer is a scratch string buffer. */

#define TEQ(name) (strcmp(type_str, (name)) == 0)
#define SETSTRING(value) Tcl_SetResult(interp, (value), TCL_VOLATILE)
#define SETBOOL(condition) strcpy(interp->result, (condition) ? "T" : "")
#define SETINT(value) sprintf(interp->result, "%i", (value))
#define APPENDSTR(str) Tcl_AppendElement(interp, (str))
#define HISTSIZE 100
#define APPENDPTR(type, ptr) AppendPtr(interp, (type), (ptr), buffer)

/* APPENDPTR calls AppendPtr so that it expands to an expression  */
/* rather than a statement.  This allows a uniform use of ; after */
/* APPENDPTR. */

#define FEQ(name) (strcmp(argv[2], (name)) == 0)

/* ------------------------------------------------------------------------- */
static char *tk_coms[] = {INSPECT, INSPECT_NETWORK, INSPECT_STATE,
			  REALIZE_STATE, INSPECT_RAW, FOCUS_WINDOW,
			  INSPECTOR_PANEL, RECOMPUTE_INSPECTOR, DISPLAY_FIELD,
			  DISPLAY_REST, TYPE, GET_TOPLEVEL, GET_ADDRESS,
			  GET_LABEL, TRUNCATE, INSPECT_BUTTON,
			  INSPECT_BUTTON_DOC, INSPECT_AND_DESTROY,
			  BREAKPOINT, SHOW, KILL_ALL};
static int num_tk_coms = 21;

static char *initial_tcl_command = NULL;

static Tcl_Interp *get_tcl_interpreter(Tcl_AppInitProc *appInitProc)
{
   /* Application-specific initialization
    * procedure to call after most
    * initialization but before starting
    * to execute commands. */

    Tcl_Interp *interp = Tcl_CreateInterp();
#ifdef TCL_MEM_DEBUG
    Tcl_InitMemory(interp);
#endif

    /*
     * Set the "tcl_interactive" variable.
     */

    Tcl_SetVar(interp, "tcl_interactive", "0",  TCL_GLOBAL_ONLY);

    /*
     * Invoke application-specific initialization.
     */

    if ((*appInitProc)(interp) != TCL_OK) {
	strcpy(interp->result, "Application initialization failed");
    }

    return(interp);
}

static void *parse_typed_pointer(char *typed_pointer, char *type) {
  unsigned int ptr = 0;
  int i;
  
  type[0] = '\0';
  if (typed_pointer[0] == '(') {
    for (i=1; typed_pointer[i] != ')' && i < 30; i++)
      type[i-1] = typed_pointer[i];
    type[i-1] = '\0';
    if (typed_pointer[i] == ')') sscanf(typed_pointer+i+1, "%x", &ptr);
  }
  return((void *) ptr);
}

static int RealizeState(ClientData clientData, Tcl_Interp *interp,
		 int argc, char *argv[]) {
  void *ptr;
  char type_str[30]; /* used by TEQ */
  STATEptr state = NULL;
  /* ---------------------- */
  /* Process the arguments. */
  /* ---------------------- */
  if (argc > 1) {
    ptr = parse_typed_pointer(argv[1], type_str);
    if (TEQ("STATE")) {
      state = ptr;
    }
  }
  if (!state) {
    fprintf(stderr, "No state given to realize_state.\n");
    return TCL_OK;
  }
  virtual_arc_set(state);
  return TCL_OK;
}

static char *label_string(id_type label)
{
  static char name[101];
  FST_copy_label_name(label, BOTH_SIDES, name, 100, 1, DONT_ESCAPE);
  return name;
}

static void AppendPtr(Tcl_Interp *interp, char *type, void *ptr, char *buffer) {
  if (ptr != NULL) {
    sprintf(buffer, "(%s)%x" , type, (unsigned int) ptr);
    Tcl_AppendElement(interp, buffer);
  }
}

static int GetField(ClientData clientData, Tcl_Interp *interp,
		 int argc, char *argv[])
{
  void *ptr;
  char type_str[30]; /* used by TEQ */
  static char buffer[100]; /* used by APPENDPTR */
  
  ptr = parse_typed_pointer(argv[1], type_str);

  if (ptr == NULL && !FEQ("*fields*")) {
    return TCL_OK;}

  if TEQ("STATE") {
    STATEptr state = ptr;
    ARCptr arc;
    if FEQ("*fields*") {
      if (STATE_is_virtual(state)) {
	SETSTRING("virtual virtual_info state_data");
      } else {
	SETSTRING("*final* arcs");
      }
    }
    else if FEQ("virtual") SETBOOL(STATE_is_virtual(state));
    else if FEQ("virtual_info") APPENDPTR("VirtualInfo", 
					  STATE_virtual_info(state));
    else if FEQ("state_data") {
      if (STATE_is_virtual(state)) {
	VirtualInfo *vinfo = STATE_virtual_info(state);
	switch (vinfo->methods->data_type) {
	case DATA_STATE: {
	  APPENDPTR("STATE",(STATEptr)STATE_state_data(state) );
	  break;
	}
	case STATE_PAIR: {
	  StatePair *pair = (StatePair *)STATE_state_data(state);
	  APPENDPTR("STATE", pair->state1);
	  APPENDPTR("STATE", pair->state2);
	  break;
	}
	case STATE_LIST: {
	  CONSptr tail, list = (CONSptr)STATE_state_data(state);
	  for (tail = list; tail; tail = cdr(tail)) {
	    STATEptr carstate = (STATEptr)car(tail);
	    APPENDPTR("STATE", carstate);
	  }
	  break;
	}
	}
      }
    }
    else if FEQ("*final*") {
      if (!STATE_is_virtual(state)) SETBOOL(state->final);
    }
    else if FEQ("arcs") {
      for (arc = STATE_arc_set(state); arc; arc = next_arc(arc)) {
	APPENDPTR("ARC", arc);
      }
    }
  }

/* ------------------------------------------------------------------------- */
  
  else if TEQ("ARC") {
    ARCptr arc = ptr;
    if FEQ("*fields*") {
      SETSTRING("label destination");
    }
    else if FEQ("*label*") {
      APPENDSTR(label_string(arc->label));
    }
    else if FEQ("label") {
      APPENDSTR(label_string(arc->label));
    }
    else if FEQ("destination") {
      APPENDPTR("STATE", ARC_destination(arc));
    }
  }

/* ------------------------------------------------------------------------- */
  
  else if TEQ("ALPHABET") {
    ALPHABETptr original = ptr;
    if FEQ("*fields*") SETSTRING("len max_id items");
    else if FEQ("len") {
      ALPHABETptr sigma = copy_alphabet(original);
      binary_to_label(sigma);
      SETINT(ALPH_len(sigma));
      free_alph(sigma);
    }
    else if FEQ("max_id") {
      ALPHABETptr sigma = copy_alphabet(original);
      int i, length, max = 0;
      binary_to_label(sigma);
      length = ALPH_len(sigma);
      for (i = 0; i < length; i++) {
	if (ALPH_item(sigma, i) > max) {
	  max = ALPH_item(sigma, i);
	}
      }
      SETINT(max);
      free_alph(sigma);
    }
    else if FEQ("items") {
      ALPHABETptr sigma = copy_alphabet(original);
      int i, length;
      binary_to_label(sigma);
      length = ALPH_len(sigma);
      for (i = 0; i < length; i++) {
	APPENDSTR(label_string(ALPH_item(sigma, i)));
      }
      free_alph(sigma);
    }
  }

/* ------------------------------------------------------------------------- */
  
  else if TEQ("NET") {
    NETptr net = ptr;
    STATEptr state;
    if FEQ("*fields*")
       SETSTRING("start sigma labels virtual_info states epsilon_free");
    else if FEQ("start") APPENDPTR("STATE", net->start.state);
    else if FEQ("epsilon_free") SETBOOL(NET_epsilon_free(net));
    else if FEQ("sigma") APPENDPTR("ALPHABET", NET_sigma(net));
    else if FEQ("labels") APPENDPTR("ALPHABET", NET_labels(net));
    else if FEQ("states") {
      for (state = NET_states(net); state; state = next_state(state)) { 
      APPENDPTR("STATE", state);
      }
    }
    else if FEQ("virtual_info") {
      if (NET_is_virtual(net)) {
	APPENDPTR("VirtualInfo", NET_virtual_info(net));
      }
    }
  }

/* ------------------------------------------------------------------------- */
  
  else if TEQ("VirtualInfo") {
    VirtualInfo *vinfo = ptr;
    if FEQ("*fields*") SETSTRING("operation op1 op2 methods network");
    else if FEQ("op1") APPENDPTR("NET", vinfo->op1);
    else if FEQ("op2") APPENDPTR("NET", vinfo->op2);
    else if FEQ("methods") APPENDPTR("VMethods", vinfo->methods);
    else if FEQ("operation") SETSTRING(vinfo->methods->operation);
    else if FEQ("network") APPENDPTR("NET", vinfo->network);
  }

  return TCL_OK;
}

static int FST_AppInit(Tcl_Interp *interp) {
  int i;

  if (Tcl_Init(interp) == TCL_ERROR) {
    return TCL_ERROR;
  }
  if (Tk_Init(interp) == TCL_ERROR) {
    return TCL_ERROR;
  }
  /* --------------------------------------------------------- */
  /* Register TCL commands implemented in C.                   */
  /* (The naming convention for these commands uses underscore */
  /* rather than dash so that it is easier to search for the   */
  /* commands across .c and .tcl file.)                        */
  /* --------------------------------------------------------- */

  Tcl_CreateCommand(interp, "get_field", GetField,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "realize_state", RealizeState,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  /* Process command definitions in tk-commands.h. */

  for (i = 0; i < num_tk_coms; i++)
    if (Tcl_Eval(interp, tk_coms[i]) != TCL_OK)
      {
	fprintf(stderr, "Error at tk_coms[%d]: %s\n", i, interp->result);
	return TCL_ERROR;
      } 
  /* ------------------------------------------------------------------ */
  /* execute an initialization script, if a -exec argument is specified */
  /* ------------------------------------------------------------------ */

  if (initial_tcl_command != NULL) {
    if (Tcl_Eval(interp, initial_tcl_command) != TCL_OK) {
      fprintf(stderr, "%s\n", interp->result);
    }
  }

  return TCL_OK;
}

static void tk_inspect(char *cmd) {
  static Tcl_Interp *TCL_interp = NULL;
  char *debug;
  int done = 0;

  if (!TCL_interp)
    TCL_interp = get_tcl_interpreter(FST_AppInit);
  Tcl_Eval(TCL_interp, cmd);

  while (!done) {
    Tk_DoOneEvent(0);

    /* We will stop if the user has killed all the browser
       windows or if he has clicked on 'Done'.*/

    Tcl_Eval(TCL_interp, "winfo children .");
    debug = Tcl_GetVar(TCL_interp, "debugging", 1);

    if ((debug && strcmp(debug, "done") == 0) ||
	strcmp(Tcl_GetStringResult(TCL_interp), "") == 0)
      done = 1;
  }

  Tcl_SetVar(TCL_interp, "debugging", "", 1); 
  Tcl_Eval(TCL_interp, "kill-all");
}

void tk_inspect_net(NETptr net)
{
  char cmd[100];
  sprintf(cmd, "inspect (%s)%x", "NET", (unsigned int) net);
  tk_inspect(cmd);
}

void tk_inspect_state(STATEptr state)
{
  char cmd[100];
  sprintf(cmd, "inspect (%s)%x", "STATE", (unsigned int) state);
  tk_inspect(cmd);
}

void tk_inspect_alph(STATEptr alph)
{
  char cmd[100];
  sprintf(cmd, "inspect (%s)%x", "ALPHABET", (unsigned int) alph);
  tk_inspect(cmd);
}
#endif /* HAVE_TCL */
