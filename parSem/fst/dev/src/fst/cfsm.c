/* $Id: cfsm.c,v 1.4 2005/03/04 15:45:22 grondeau Exp $ */
/* C-FSM Interface Library

   Collect and standardize the C-FSM function library.
   April, 1995 Mike Wilkens */

/* C-FSM objects.

   Symbols are objects used to label arcs in finite-state machines.
   Symbols are character strings which are the same if the character
   strings have the same characters in the same order.  Symbols
   are usable with any finite-state machine -- they are not interned
   with respect to a particular fsm.  Once interned (see make_symbol),
   symbols may be compared for equality using the C == operator.
   Symbols from one fsm may be incorporated into labels used by
   another fsm, and may be compared for equality with symbols
   from another fsm.

   Labels are used to put information on arcs.  Labels contain
   one or more symbols.  For a strict one-sided FSM, a label
   contains one symbol.  For two-sided FSTs, labels contain 2
   symbols, an "upperside" symbol and a "lowerside" symbol.
   Labels from one machine may be incorprated into arcs belonging
   to another machine.

   Arcs connect states and carry a label.  Arcs may not be
   transferred between machines (?).  For an arc to be attached
   to a machine, it must either be newly created with respect
   to that machine (?), or must already belong to that machine (?).

   States.  States must be explicitly added to and subtracted from
   fsms.  A state belonging to one machine cannot be used in
   another.

   Finite-state machines (FSMs) are sets of states connected by
   arcs.   mjw */



#include <string.h>

#include "fst/types.h"
#include "fst/stack-mr.h"
#include "fst/context.h"

#include "fst/cfsm.h"

#include "fst/copy-fsm.h"
#include "fst/eqv-labs.h"
#include "fst/struct.h"
#include "fst/sig-prop.h"

#include "fst/bin-in.h"
#include "fst/header.h"
#include "fst/fsm_io.h"

#include "fst/bin-out.h"
#include "fst/cons.h"
#include "fst/calculus.h"
#include "fst/look.h"
#include "fst/pretty-p.h"
#include "fst/net-io.h"
#include "fst/pars-tbl.h"
#include "fst/text-io.h"
#include "fst/site-dep.h"
#include "fst/re-aux.h"
/* #include "fst/command.h" */
#include "fst/cmp-net.h"
#include "fst/compact.h"
#include "fst/prolog.h"
#include "fst/prompt.h"
#include "fst/recode.h"
#include "fst/subseq.h"
#include "fst/mark.h"
#include "fst/arc-optimization.h"

#include <stdlib.h>
#include <stdio.h>
#include "fst/utf8_fatstr_util.h"

#ifndef SLIM_FST /* Only a minimal set of functions is
                    compiled when SLIM_FST is defined. */ 


#define CFSM_NO_ERROR 0
#define CFSM_WRITE_ERROR -1
#define CFSM_BUFFER_OVERFLOW -2
#define CFSM_ERROR -3


/* typedef NETptr cfsm_fsm;
typedef NVptr cfsm_fsm_sequence; */

#define TRUE 1
#define FALSE 0


/* Labels and symbols are both labelid's. */

/* typedef int	cfsm_label;
typedef int	cfsm_symbol; */

#define INVALID_LABEL (-1)


/* typedef ARCptr	cfsm_arc;
typedef STATEptr cfsm_state;

typedef OBJECTptr cfsm_object;

typedef unsigned short cfsm_fat_char; */

void* cfsm_error_object = NULL;

/* const int CFSM_UPPERSIDE_SYMBOL;
const int CFSM_LOWERSIDE_SYMBOL; */



/* Return the integer number for the given label: */
int cfsm_label_number(cfsm_label l) {
    return l;
}

cfsm_label cfsm_number_to_label(int n) {
    return n;
}

cfsm_label cfsm_the_invalid_label(void) {
    return INVALID_LABEL;
}

int cfsm_valid_label(cfsm_label l) {
    return (l != INVALID_LABEL);
}

/* Return the integer number for the given symbol: */
int cfsm_symbol_number(cfsm_symbol s) {
    return s;
}

cfsm_symbol cfsm_number_to_symbol(int n) {
    return n;
}

int cfsm_valid_symbol(cfsm_symbol sym) {
    return (sym != INVALID_LABEL);
}

cfsm_symbol cfsm_the_invalid_symbol(void) {
    return INVALID_LABEL;
}



/* Here are functions to convert between cfsm.h labels and id_types, and
   between symbols and id_types. */

static cfsm_label convert_id_to_label(id_type id) {
    if (id == ID_NO_SYMBOL)
        return cfsm_the_invalid_label();
    return cfsm_number_to_label(id);
}
static cfsm_symbol convert_id_to_symbol(id_type id) {
    if (id == ID_NO_SYMBOL)
        return cfsm_the_invalid_symbol();
    return cfsm_number_to_symbol(id);
}
static id_type convert_label_to_id(cfsm_label l) {
    if (cfsm_valid_label(l))
        return cfsm_label_number(l);
    return ID_NO_SYMBOL;
}
static id_type convert_symbol_to_id(cfsm_symbol s) {
    if (cfsm_valid_symbol(s))
        return cfsm_symbol_number(s);
    return ID_NO_SYMBOL;
}




/* C-FSM is in the process of being changed to be reentrant, and to use
   context objects instead of global variables.  For now, we store the
   context object in a global variable, for functions which need it. */

void FstCalcExport cfsm_init(void) {
    set_default_context();
    init_fsm_memory(default_context_object);
    init_restore_functions();
    init_save_functions();
}



void cfsm_free(void) {
    release_fsm_memory();
    if (default_context_object != NULL)
    {
      unset_default_context();
      default_context_object= NULL;
    }
}




int cfsm_fsm_sequence_length (cfsm_fsm_sequence s)
{
    return NV_len (s);
}


cfsm_fsm cfsm_fsm_sequence_ref (cfsm_fsm_sequence s, int i)
{
    return NV_net (s, i);
}





cfsm_fsm_sequence cfsm_read_fsm_sequence (const char* filename)
{
    FILE* input_stream;
    cfsm_fsm_sequence net_list;

    input_stream = fopen (filename, "rb");

    if (input_stream == NULL)
	return cfsm_error_object;

    if (!std_file_header_p (input_stream))
    {
	fclose (input_stream);
	return cfsm_error_object;
    }

    net_list = networks_from_file (input_stream);

    fclose (input_stream);

    if (net_list == NULL)
	return cfsm_error_object;

    return net_list;
}



cfsm_fsm cfsm_read_fsm (const char* filename)
{
    cfsm_fsm_sequence fsm_seq = cfsm_read_fsm_sequence (filename);
    int number_of_nets;

    if (fsm_seq == cfsm_error_object)
	return cfsm_error_object;

    number_of_nets = cfsm_fsm_sequence_length (fsm_seq);

    if (number_of_nets < 1)
    {
	printf ("cfsm: \"%s\" contains no machines.\n", filename);
	return cfsm_error_object;
    }
    if (number_of_nets > 1)
    {
	printf ("cfsm: warning: \"%s\" contains more than one machine.\n",
		filename);
    }
    return cfsm_fsm_sequence_ref (fsm_seq, 0);
}




/* cfsm_update_sigma
   Updates the label alphabet by examining all the arcs in the
   machine and building a new sigma table.  This function must
   be called before writing out an fsm, before pruning, and
   before minimization, says Andre Kemp (Xerox France).  In general,
   before any large operation. */


void cfsm_update_sigma(cfsm_fsm fsm)
{
  /* free label alphabet if it exists and make a new one by looking at
     the arcs in the network */

  update_label_alphabet(fsm);

  /* free sigma alphabet if it exists and make a new one by looking at
     the label alphabet */

  free_alph(NET_sigma(fsm));
  NET_sigma(fsm) = label_sigma(fsm);

  /* Scan the label alphabet and set net arity value appropriately. */
  set_net_arity(fsm);
}



int cfsm_write_fsm (cfsm_fsm fsm, const char* filename)
{
    int rv;

    cfsm_update_sigma (fsm);

    rv = write_net (fsm, (char *)filename);

    if (rv != 0)
	return CFSM_WRITE_ERROR;
    return CFSM_NO_ERROR;
}




int cfsm_state_count(cfsm_fsm fsm) {
    return (int)NET_num_states(fsm);
}


int cfsm_arc_count(cfsm_fsm fsm) {
    return (int)NET_num_arcs(fsm);
}




/* States */


cfsm_state cfsm_the_invalid_state(void) {
    return NULL;
}

int cfsm_valid_state(cfsm_state s) {
    return (s != NULL);
}

cfsm_state cfsm_start_state (cfsm_fsm fsm)
{
    return NET_start_state(fsm);
}


void cfsm_set_start_state (cfsm_fsm fsm, cfsm_state s)
{
    NET_start_state(fsm) = s;
}


cfsm_state cfsm_state_list (cfsm_fsm fsm)
{
    return NET_states(fsm);
}


cfsm_state cfsm_next_state (cfsm_state state, cfsm_fsm fsm)
{
    return next_state(state);
}


/* In the functions below, pass in the fsm as well as the
   state, to support implementations that represent states
   as indexes into arc vectors. */


int cfsm_state_visit_mark(cfsm_state state, cfsm_fsm fsm) {
    return STATE_visit_mark(state);
}


void cfsm_set_state_visit_mark(cfsm_state state, int value, cfsm_fsm fsm) {
    STATE_visit_mark(state) = value;
}







/* cfsm_add_state makes a new state with no outgoing arcs,
   and adds it to the state list of the given fsm. */


cfsm_state cfsm_add_state (int final, cfsm_fsm fsm)
{
    cfsm_state s = make_state (NULL, fsm);
    STATE_final(s) = final;
    return s;
}



/* cfsm_remove_state removes a state from the fsm.  After
   removing a state, the state may not be used in any way.
   Explicit removal of states is usually not necessary;
   cleanup of unreachable states and arcs is normally
   performed by cfsm_prune.  cfsm_remove_state is included
   here mainly so that prune can be implemented using this
   low-level cfsm library interface.  */




void* cfsm_state_name (cfsm_state s, cfsm_fsm fsm)
{
    return STATE_client_cell(s);
}

void cfsm_set_state_name (cfsm_state s, const void* val, cfsm_fsm fsm)
{
    STATE_client_cell(s) = (void *)val;
}


/* cfsm_number_the_states assigns a number to each state, storing the
   number in the state's client cell.  One must not change the FSM
   after asking for a numbering.  Also if the client cells are changed,
   that will destory the numbering. */

void cfsm_number_the_states(cfsm_fsm m) {
    /* It is an error for a state number to exceed cfsm_state_count(m)-1. */
    cfsm_state s;
    int num = 0;
    for (s = NET_states(m); s != NULL; s = next_state(s)) {
	STATE_client_cell(s) = (void*)num;
	++num;
    }
    if (num != cfsm_state_count(m)) {
	fprintf(stderr, "CFSM-ERROR: state numbering finds that the number of states\n"
		"in the state list does not match the state count.\n");
    }
}


/* cfsm_assert_state_numbering checks that the states are numbered correctly.
   If so it returns 1, else 0. */

int cfsm_assert_state_numbering(cfsm_fsm m) {
    cfsm_state s;
    int num = 0;
    for (s = NET_states(m); s != NULL; s = next_state(s)) {
	if ((int)(STATE_client_cell(s)) != num)
	    return 0;
	++num;
    }
    return (num == cfsm_state_count(m)) ? 1 : 0;
}


/* cfsm_state_number returns the number assigned to the state
   by cfsm_number_the_states, which is a number from 0..state_count-1. */

int cfsm_state_number(cfsm_state s) {
    return (int)STATE_client_cell(s);
}


int cfsm_state_final (cfsm_state s, cfsm_fsm fsm)
{
    return (STATE_final(s) != 0);
}


void cfsm_set_state_final (cfsm_state s, int b, cfsm_fsm fsm)
{
    STATE_final(s) = b;
}




/* Arcs */



int cfsm_valid_arc(cfsm_arc arc) {
    return (arc != NULL);
}

cfsm_arc cfsm_the_invalid_arc(void) {
    return NULL;
}


cfsm_arc cfsm_first_arc (cfsm_state s, cfsm_fsm fsm)
{
    return STATE_arc_set(s);
}


cfsm_arc cfsm_next_arc (cfsm_arc a, cfsm_fsm fsm)
{
    return next_arc(a);
}


cfsm_state cfsm_arc_destination (cfsm_arc arc, cfsm_fsm fsm)
{
    return ARC_destination(arc);
}



void cfsm_add_arc(cfsm_label label, cfsm_state from, cfsm_state to, cfsm_fsm fsm) {
    add_new_arc(fsm, from, label, to);
}



/* Unlink the given arc from the given state, but do not free the arc or remove
   it from the FSM.  Used by top_arc and remove_arc. */

static int unlink_arc(cfsm_arc a, cfsm_state s, cfsm_fsm m) {
    ARCptr prev = NULL;
    ARCptr curr = STATE_arc_set(s);
    while (curr != NULL) {
	if (curr == a) {
	    if (prev == NULL)
		STATE_arc_set(s) = next_arc(a);
	    else
		next_arc(prev) = next_arc(a);
	    return 1;
	}
	prev = curr;
	curr = next_arc(curr);
    }
    fprintf(stderr, "Warning: cfsm_remove_arc: arc not found\n");
    return 0;
}


void cfsm_remove_arc(cfsm_arc a, cfsm_state s, cfsm_fsm m) {
    if (unlink_arc(a, s, m)) {
	--NET_num_arcs(m);
	free_arc(a);
    }
}


/* Make the given arc the first arc at the given state.  The state must
   already contain the arc. */

void cfsm_top_arc(cfsm_arc a, cfsm_state s, cfsm_fsm m) {
    unlink_arc(a, s, m);
    next_arc(a) = STATE_arc_set(s);
    STATE_arc_set(s) = a;
}



cfsm_label cfsm_arc_label (cfsm_arc arc, cfsm_fsm fsm) {
    return (arc == NULL ? INVALID_LABEL : ARC_label(arc));
}




/* Properties */


/* Properties are set and retrieved from fsms as strings or sequences
   of strings.  Symbols, strings, and lists are supported.  Symbols
   are represented as normal strings, except that some special characters
   are quoted.  Strings begin and end with ".  Lists are represented
   as a sequence of strings, starting with a ( string and ending in
   a ) string.

   Quote ("), open ((), close ()), and backslash (\) are special
   characters and are quoted when they occur within property strings
   and symbols.  Since " is used to tell a string from a symbol,
   literal occurences of " in the symbol or string are preceded by
   a backslash.  Since open and close are used to identify a list,
   occurences of these are also escaped.  For example, the string
   (Well\n"guys") would be represented as the string:

   "(Well\\n\"guys\""

   Use cfsm_unquote_string to undo the escapification.  */



static int write_fat_string

(FAT_STR str, char* buffer, size_t buffer_size, int escape)

{
    cfsm_fat_char ch;
    int i = 0;

    while ((ch = CHARNUM(str++)) != 0 && i < buffer_size - 1)
    {
	if (escape &&
	    (ch == '"' || ch == '\\' ||
	     ch == '(' || ch == ')'))
	    buffer[i++] = '\\';
	buffer[i++] = (char)ch;
    }

    if (i - 1 >= buffer_size)
	return CFSM_BUFFER_OVERFLOW;

    buffer[i] = '\0';
    return CFSM_NO_ERROR;
}




int cfsm_escapify (const char* str, char* buffer, size_t buffer_size, int string)
{
    char ch;
    int i = 0;

    if (string)
    {
	if (buffer_size < 3)
	    return CFSM_BUFFER_OVERFLOW;
	buffer[i++] = '"';
    }

    /* Since we may write two chars (backslash plus one more),
       plus add " and then \0, we allow for four chars below. */

    while ((ch = *str++) != '\0' && i < buffer_size - 3)
    {
	if (ch == '"' || ch == '\\' || ch == '(' || ch == ')')
	    buffer[i++] = '\\';
	buffer[i++] = (char)ch;
    }

    if (string)
	buffer[i++] = '"';
    buffer[i] = '\0';

    return CFSM_NO_ERROR;
}




/* cfsm_fsm_props
   lists all the attributes this fsm has defined.
   The number of attributes is returned, and the
   names of the attributes are written in sequence
   to the buffer. */


int cfsm_fsm_props (cfsm_fsm fsm, char* buffer, size_t buffer_size)
{
    PROPptr prop;
    int i = 0;

    for (prop = NET_properties(fsm); 
	 prop != NULL; 
	 prop = next_prop(prop))
    {
	int ec = write_fat_string
	    (PROP_attr(prop), buffer + i, buffer_size - i, FALSE);

	if (ec < CFSM_NO_ERROR)
	    return ec;

	i += strlen (buffer + i) + 1;
    }
    return CFSM_NO_ERROR;
}



static int write_object_list (SEQUENCEptr obj, char* buffer, size_t buffer_size);


static int write_object (OBJECTptr obj, char* buffer, size_t buffer_size)
{
    int rv;
    int i = 0;

    switch(OBJ_type(obj))
    {

    case String:

	if (i >= buffer_size) return CFSM_BUFFER_OVERFLOW;

	buffer[i++] = '"';
	rv = write_fat_string
	    ((OBJ_str(obj)), buffer + i, buffer_size - i, TRUE);

	if (rv < CFSM_NO_ERROR)
	    return rv;

	i += strlen (buffer + i);

	if (i >= buffer_size - 1) return CFSM_BUFFER_OVERFLOW;

	buffer[i++] = '"';
	buffer[i] = '\0';

	return 1;


    case Symbol:

	rv = write_fat_string
	    (IO_SYM_name(OBJ_symbol(obj)), buffer + i, buffer_size - i, TRUE);

	if (rv < CFSM_NO_ERROR)
	    return rv;

	return 1;


    case List:

	return write_object_list
	    (OBJ_list(obj), buffer + i, buffer_size - i);

    case Int:
    case Character:
    case Array:
    case Byte_block:
    default:

	puts ("cfsm_get_prop: unsupported property type");
	return CFSM_ERROR;
    }
}



static int isadv (char* buffer, int i, int cnt)
{
    while (cnt--)
	i += strlen (buffer + i) + 1;

    return i;
}



static int write_object_list (SEQUENCEptr s, char* buffer, size_t buffer_size)
{
    int i = 0;			/* index into buffer */
    int j;			/* index into sequence */
    int len = SEQ_len(s);
    int total_len = 0;
    int rv;

    if (i >= buffer_size - 1) return CFSM_BUFFER_OVERFLOW;
	     
    buffer[i++] = '(';
    buffer[i++] = '\0';


    for (j = 0; j < len; j++)
    {
	switch(SEQ_type(s))
	{

	case Object:
	    
	    rv = write_object (SEQ_objects(s)[j], buffer + i, buffer_size - i);

	    if (rv < CFSM_NO_ERROR)
		return rv;

	    i = isadv (buffer, i, rv);

	    total_len += rv;

	    break;


	case String:

	    if (i >= buffer_size) return CFSM_BUFFER_OVERFLOW;

	    buffer[i++] = '"';
	    rv = write_fat_string
		(SEQ_strings(s)[j], buffer + i, buffer_size - i, TRUE);

	    if (rv < CFSM_NO_ERROR)
		return rv;

	    i += strlen (buffer + i);

	    if (i >= buffer_size - 1) return CFSM_BUFFER_OVERFLOW;

	    buffer[i++] = '"';
	    buffer[i++] = '\0';

	    total_len += 1;

	    break;


	case Symbol:

	    rv = write_fat_string
		(IO_SYM_name(SEQ_symbols(s) + j),
		 buffer + i, buffer_size - i, TRUE);

	    if (rv < CFSM_NO_ERROR)
		return rv;

	    i += strlen (buffer + i) + 1;

	    total_len += 1;

	    break;


	case Int:
	case Character:
	case Symbol_package:
	case Array:
	case List:
	case Byte_block:
	default:

	    puts ("cfsm_get_prop: unsupported property type");
	    return CFSM_ERROR;
	}
    }
    if (i >= buffer_size - 1) return CFSM_BUFFER_OVERFLOW;

    buffer[i++] = ')';
    buffer[i++] = '\0';

    return total_len + 2;
}




/* cfsm_get_prop
   copies the value of the given attribute into buffer as a
   sequence of strings.  The number of strings is returned.
   0 is returned if the attribute is not defined. */


int cfsm_get_prop (const char*		attribute,
                   char*		buffer,
                   size_t		buffer_size,
                   cfsm_fsm	fsm)
{
    OBJECTptr obj = get_value(fsm, FST_thin_to_fat (attribute));
    return (obj == NULL) ? 0 : write_object (obj, buffer, buffer_size);
}










/* cfsm_unescapify
   removes the quotes and escaped characters from the string
   contained in buffer.  Buffer is changed to the result string.

   This function works by removing start and end quotes, and
   undoing the escapification of "()\  */


void cfsm_unescapify (char* buffer)
{
    int i = 0;
    int j = 0;
    char ch;
    int escape = FALSE;

    while ((ch = buffer[i++]) != '\0')
    {
	if (escape)
	{
	    buffer[j++] = ch;
	    /* was ... = (ch == 't') ? '\t' : ((ch == 'n') ? '\n' : ch); */
	    escape = FALSE;
	}
	else
	{
	    if (ch == '\\')
		escape = TRUE;
	    else if (ch != '"')	/* remove unescaped " from output */
		buffer[j++] = ch;
	}
    }
    buffer[j] = '\0';
}
	    



/* Setting properties */




/* Make an object struct and set the type field; the value
   field is left uninitialized. */


/* Duplicates a function in struct.c */
/*
static OBJECTptr make_object (int type)
{
    OBJECTptr object = (OBJECTptr)malloc(sizeof(OBJECTtype));

    if (object == NULL)
	return NULL;

    OBJ_type(object) = type;
    return(object);
}
*/

static void free_obj (OBJECTptr obj)
{
    free (obj);
}




/* Make a sequence struct, set the type field to Object, and
   set the length field to len.  Allocate the actual sequence
   as well.  Only sequences containing objects are supported. */

/* Duplicates a function in struct.c */
/*
static SEQUENCEptr make_sequence (int len)
{
    SEQUENCEptr sequence = (SEQUENCEptr)malloc(sizeof(SEQUENCEtype));

    if (sequence == NULL)
	return NULL;

    SEQ_type(sequence) = Object;
    SEQ_len(sequence) = len;

    SEQ_objects(sequence) = (OBJECTptr*) malloc(len * sizeof(OBJECTptr));

    if (SEQ_objects(sequence) == NULL)
    {
	free (sequence);
	return NULL;
    }
    return(sequence);
}
*/

static void free_seq (SEQUENCEptr seq)
{
    free (SEQ_objects(seq));
    free (seq);
}



static OBJECTptr make_sequence_object (int len)
{
    SEQUENCEptr seq = make_sequence (len, Object);
    OBJECTptr obj = make_object (List);
    if (seq == NULL || obj == NULL)
    {
	if (seq != NULL)
	    free_seq (seq);
	if (obj != NULL)
	    free_obj (obj);
    }
    OBJ_list(obj) = seq;
    return obj;
}


static void free_sequence_object (OBJECTptr obj)
{
    free_seq (OBJ_list(obj));
    free_obj (obj);
}



/* Allocate a fat string and copy the string to the fat string.
   Unescapify the string. */

static FAT_STR unescapify_fat (char* name)
{
    int escape = FALSE;
    char ch;
    FAT_STR fat = malloc ((strlen (name) + 1) * sizeof(FAT_CHAR));
    FAT_STR f = fat;

    if (fat == NULL)
	return NULL;

    while ((ch = *name++) != '\0')
    {
	if (escape)
	{
	    CHARNUM(f) = (cfsm_fat_char)ch;
	    CHARSET(f++) = 0;
	    escape = FALSE;
	}
	else
	{
	    if (ch == '\\')
		escape = TRUE;
	    else if (ch != '"')	/* remove unescaped " from output */
	    {
		CHARNUM(f) = (cfsm_fat_char)ch;
		CHARSET(f++) = 0;
	    }
	}
    }
    CHARSET(f) = 0;
    CHARNUM(f++) = 0;
    return fat;
}


/* Make a symbol struct, plus a fat string name */

static IO_SYMBOLptr make_symbol (char* name)
{
    IO_SYMBOLptr sym = (IO_SYMBOLptr)malloc (sizeof(IO_SYMBOLtype));
    FAT_STR fat = unescapify_fat (name);
    if (sym == NULL || fat == NULL)
      {
	if (fat != NULL)
	    free (fat);
	if (sym != NULL)
	    free (sym);
	return NULL;
      }
    IO_SYM_name(sym) = fat;
    return sym;
}

static void free_symbol (IO_SYMBOLptr sym)
{
    free (IO_SYM_name(sym));
    free (sym);
}



/* Make an object out of a symbol name string.  Make a symbol
   struct and an object struct. */


static OBJECTptr make_symbol_object (char* name)
{
    IO_SYMBOLptr sym = make_symbol (name);
    OBJECTptr obj = make_object (Symbol);
    if (sym == NULL || obj == NULL)
    {
	if (sym != NULL)
	    free_symbol (sym);
	if (obj != NULL)
	    free_obj (obj);
	return NULL;
    }
    OBJ_symbol(obj) = sym;
    return obj;
}





/* Make an object struct, plus a fat string name.  */


static OBJECTptr make_string_object (char* name)
{
    OBJECTptr obj = make_object (String);
    FAT_STR fat = unescapify_fat (name);
    if (obj == NULL || fat == NULL)
    {
	if (obj != NULL)
	    free (obj);
	if (fat != NULL)
	    free (fat);
	return NULL;
    }
    OBJ_str(obj) = fat;
    return obj;
}


static int isopen (char* str)
{
    return (str[0] == '(' && str[1] == '\0');
}


static int isclose (char* str)
{
    return (str[0] == ')' && str[1] == '\0');
}



static char* sadv (char* str)
{
    return str + strlen (str) + 1;
}


static int list_size (char* str)
{
    int len = 1;
    int depth = 1;

    if (!isopen (str))
	return CFSM_ERROR;

    str = sadv(str);
    while (depth > 0)
    {
	depth += isopen (str) - isclose (str);
	++len;
	str = sadv(str);
    }
    return len;
}


static int list_length (char* str)
{
    int len = 0;
    int depth = 1;

    if (!isopen (str))
	return CFSM_ERROR;

    str = sadv(str);
    while (depth > 0)
    {
	depth += isopen (str) - isclose (str);
	if (depth == 1)
	    ++len;		/* count closing parens (exc. last) */
	str = sadv(str);
    }
    return len;
}



static char* sref (char* strs, int n)
{
    int i;
    for (i = 0; i < n; ++i)
	strs = sadv (strs);
    return strs;
}


static char* next_object (char* buffer)
{
    return isopen (buffer) ?
	sref (buffer, list_size (buffer)) :
	    sadv (buffer);
}
    


/* read_prop is a recursive function that reads an arbitrary property
   from the given buffer.  The buffer holds a sequence of one or
   more tokens. */


static OBJECTptr read_object (char* buffer)
{
    if (isopen (buffer))	/* list? */
    {
	int i;
	int len = list_length (buffer);
	OBJECTptr list = make_sequence_object (len);

	if (list == NULL)
	    return NULL;

	buffer = sadv(buffer);	/* advance past ( */
	for (i = 0; i < len; ++i)
	{
	    OBJECTptr obj;

	    obj = read_object (buffer);
	    if (obj == NULL)
	    {
		puts ("cfsm.read_object: error reading list");
		free_sequence_object (list);
		return NULL;
	    }
	    SEQ_objects(OBJ_list(list))[i] = obj;
	    buffer = next_object (buffer);
	}
	return list;
    }
    if (buffer[0] == '"')	/* string? */
	return make_string_object (buffer);
    /* symbol */
    return make_symbol_object (buffer);
}





/* cfsm_set_prop sets the given attribute to the given value.
   The string is converted to a fat string, because the
   underlying library wants it that way.  Thin_to_fat simply
   mallocs a new string, which is never freed.

   sets the property according to the contents of buffer
   which is a sequence of "tokens" strings.  If the
   attribute is already defined, it is redefined.

	List: (\0token\0...\0)\0
	String: "abc\"abc"\0
	Symbol: a\"bc\0  */



void cfsm_set_prop (const char* attribute, const char* buffer, cfsm_fsm fsm)
{
    OBJECTptr val = read_object ((char *)buffer);
    if (val == NULL)
	printf ("cfsm: cfsm_set_prop: error reading property value\n");
    else
	add_prop (fsm, FST_thin_to_fat (attribute), val);
}



void cfsm_unset_prop (const char* attribute, cfsm_fsm fsm)
{
    (void) remove_prop (fsm, FST_thin_to_fat (attribute));
}






/* Labels */


cfsm_label cfsm_nth_label(cfsm_fsm fsm, int n) {
    ALPHABETptr alph = NET_labels(fsm);
    int16 type = ALPH_type(alph);
    int len = ALPH_len(alph);
    id_type* array = ALPH_items(alph);

    if (n < 0)
	return INVALID_LABEL;
    if (type == LABEL_VECTOR) {
	if (n >= len || array[n] == ID_NO_SYMBOL)
	    return INVALID_LABEL;
	return convert_id_to_label(array[n]);
    }
    else { /* BINARY_VECTOR */
	/* Find the position of the nth 1 in the bit vector: */
	int i;
	cfsm_label label = -1;
        /* assume n >= 0 */
	for (i = 0; i <= n; ++i) {
            ++label;
	    while (label < len && !array[label])
		++label;
	    if (label == len)
		return INVALID_LABEL;
	}
	return label;
    }
}



static int alph_size(ALPHABETptr alph) {
    if (ALPH_type(alph) == LABEL_VECTOR)
	return ALPH_len(alph);
    else {			/* a binary vector -- count the 1 bits */
	int i;
	int n = 0;
	for (i = 0; i < ALPH_len(alph); ++i)
	    if (ALPH_items(alph)[i] != 0)
		++n;
	return n;
    }
}


int cfsm_label_set_size(cfsm_fsm fsm) {
    cfsm_update_sigma(fsm);
    return alph_size(NET_labels(fsm));
}


int cfsm_symbol_set_size(cfsm_fsm fsm) {
    cfsm_update_sigma(fsm);
    return alph_size(NET_sigma(fsm));
}


/* cfsm_label_set stores the FSM's set of labels in the given array.  It is
   assumed that the array is large enough to hold cfsm_label_set_size()
   items. */

static void get_alph(ALPHABETptr alph, cfsm_label* out) {
    id_type* array;

    array = ALPH_items(alph);

    if (ALPH_type(alph) == LABEL_VECTOR) {
	int i;
	for (i = 0; i < ALPH_len(alph); ++i)
	    out[i] = convert_id_to_label(array[i]);
    }
    else { /* BINARY_VECTOR */
	int i;
	int next = 0;
	for (i = 0; i < ALPH_len(alph); ++i) {
	    if (array[i] != 0)
		out[next++] = cfsm_number_to_label(i);
	}
    }
}


void cfsm_label_set(cfsm_fsm fsm, cfsm_label* out) {
    cfsm_update_sigma(fsm);
    get_alph(NET_labels(fsm), out);
}




/* cfsm_symbol_set stores the FSM's set of symbols in the given array.  It is
   assumed that the array is large enough to hold cfsm_symbol_set_size()
   items. */

void cfsm_symbol_set(cfsm_fsm fsm, cfsm_symbol* out) {
    cfsm_update_sigma(fsm);
    get_alph(NET_sigma(fsm), out);
}



#if 0
/* Return the maximum arity of all labels in the FSM.  Assumes
   that the sigma is up-to-date: */
static int max_label_arity(cfsm_fsm m) {
    int arity = 0;
    int n = cfsm_label_set_size(m);
    cfsm_label* labelset = malloc(n * sizeof(cfsm_label));
    int i;
    cfsm_label_set(m, labelset);
    for (i = 0; i < n; ++i) {
	int a = cfsm_label_arity(labelset[i], m);
	if (a > arity)
	    arity = a;
    }
    free(labelset);
    return arity;
}
#endif


int cfsm_is_fst(cfsm_fsm fsm) {
    cfsm_update_sigma(fsm);
    return (NET_arc_label_arity(fsm) > 1);
}



cfsm_label cfsm_make_label(cfsm_symbol sym) {
    return sym;
}


cfsm_label cfsm_make_fst_label(cfsm_symbol upper, cfsm_symbol lower) 
{
  id_type tuple_id;

  if (make_fstlabel(&tuple_id, upper, lower) != NO_ERROR)
    return INVALID_LABEL;
  else
    return convert_id_to_label(tuple_id);
}



int cfsm_label_arity (cfsm_label l, cfsm_fsm fsm)
{
    return LABEL_arity(id_to_label (l));
}

cfsm_symbol cfsm_the_invalid_symbol(void);

cfsm_symbol cfsm_label_symbol (cfsm_label l, cfsm_fsm fsm) {
    if (!cfsm_valid_label(l))
	return cfsm_the_invalid_symbol();
    if (cfsm_label_arity(l, fsm) != 1) {
	fprintf(stderr, "CFSM-ERROR: cfsm_label_symbol label has more than one symbol\n");
    }
    return l;
}





/* cfsm_label_ref returns the ith symbol of the given label.
   If the label is of arity 1, then any ref will return
   that symbol: a is interpreted as a:...:a. */



#define CFSM_UPPERSIDE_SYMBOL	UPPER
#define CFSM_LOWERSIDE_SYMBOL	LOWER



cfsm_symbol cfsm_label_ref(cfsm_label l, int i, cfsm_fsm fsm) {
    if (cfsm_label_arity(l, fsm) == 1)
	return l;

    /* Label is an array of labelid's: */

    return TUPLE_labels(LABEL_tuple(id_to_label(l)))[i];
}


cfsm_symbol cfsm_label_upperside(cfsm_label l, cfsm_fsm fsm) {
    return cfsm_label_ref(l, UPPER, fsm);
}

cfsm_symbol cfsm_label_lowerside(cfsm_label l, cfsm_fsm fsm) {
    return cfsm_label_ref(l, LOWER, fsm);
}


/* Return the highest label number plus one.  This is not the same as the
   number of labels in the FSM: */
int cfsm_label_number_limit(cfsm_fsm f) {
    return global_alph_size();
}


/* Symbols */

/* Return the symbol associated with the given name; if there exists no
   symbol by that name, return the invalid symbol: */
cfsm_symbol cfsm_find_symbol(const char* str, cfsm_fsm fsm) {
    id_type id;
	if (!*str) {
		return EPSILON;
	}
	else
	{
        if (atomic_label_to_id(&id, FST_thin_to_temp_fat((char*)str)) != NO_ERROR)
		{
			return INVALID_LABEL;
		}
		return convert_id_to_symbol(id);
	}
}


/* Return the label for the given symbol.  We assume that the singleton sym
   appears in the label set: */
cfsm_label cfsm_symbol_label(cfsm_symbol sym) {
    return sym;
}




cfsm_symbol cfsm_make_symbol(const char* str, cfsm_fsm fsm) {
    id_type sym;
    if (FST_intern_literal(&sym, (char*)str) != NO_ERROR)
      /* No space left */
      return INVALID_LABEL;

    /* Add new symbol to network's alphabet table:
    label_to_sigma(NET_sigma(fsm), sym);
    binary_to_label(sigma_add_to(NET_labels(fsm), sym)); */
    return convert_id_to_symbol(sym);
}


/* Make a symbol from the given string of 16-bit characters.  The
   string is terminated with 16-bit 0.  The empty string is
   interpreted as EPSILON. */

cfsm_symbol cfsm_make_unicode_symbol(const cfsm_fat_char* str, cfsm_fsm fsm) 
{
  id_type sym;
  
  intern_unicode(&sym, (char*)str);
  return convert_id_to_symbol(sym);
}

cfsm_symbol cfsm_epsilon_symbol (void)
{
    return EPSILON;
}


cfsm_symbol cfsm_other_symbol (void)
{
    return OTHER;
}



int cfsm_symbol_eq (cfsm_symbol s1, cfsm_symbol s2)
{
    return (s1 == s2);
}


int cfsm_symbol_eq_string(cfsm_symbol sym, const char* str) {
    LABELptr lab = id_to_label(sym);
    FAT_STR name;
    char ch;
    unsigned char fch = 0;

    if (LABEL_arity(lab) != 1) {
	fprintf(stderr, "CFSM-ERROR: symbol found not to be arity 1\n");
	return 0;
    }
    name = LABEL_atom(lab);

    while ((ch = *str++) != '\0' && (fch = CHARNUM(name++)) != 0) {
      if ((unsigned char)ch != fch)
	return 0;
    }
    return ((unsigned char)ch == fch) ? 1 : 0;
}


int cfsm_epsilon_p (cfsm_symbol s)
{
    return (s == EPSILON);
}


int cfsm_other_p (cfsm_symbol s)
{
    return (s == OTHER);
}






static int cfsm_extract_string (FAT_STR fs, char* buffer, size_t buffer_size)
{
    size_t i = 0;
    cfsm_fat_char ch;

    while ((ch = CHARNUM(fs++)) != '\0' && i < buffer_size)
	buffer[i++] = ch;

    if (i == buffer_size)
	return CFSM_BUFFER_OVERFLOW;

    buffer[i] = '\0';
    return CFSM_NO_ERROR;
}



/* Return the nth character in the given symbol: */
int cfsm_symbol_ref(cfsm_symbol sym, int n) {
    if (cfsm_other_p(sym) || cfsm_epsilon_p(sym))
	return 0;
    else {
      FAT_STR str = LABEL_atom(id_to_label(sym));

      return (CHARNUM(str+n) + 256*CHARSET(str+n));
    }
}

/* Return the length of the name of the given symbol: */
int cfsm_symbol_length(cfsm_symbol sym) {
    if (cfsm_other_p(sym) || cfsm_epsilon_p(sym))
	return 0;
    else {
      FAT_STR str = LABEL_atom(id_to_label(sym));

      int len = 0;
      while (CHARNUM(str) != 0 || CHARSET(str) != 0) {
	++str;
	++len;
      }
      return len;
    }
}



int cfsm_symbol_name (cfsm_symbol sym, char* buffer, size_t buffer_size)
{
  FAT_STR fs = LABEL_atom(id_to_label (sym));

  return cfsm_extract_string (fs, buffer, buffer_size);
}




int cfsm_fat_symbol_name (cfsm_symbol sym,
			  cfsm_fat_char* buffer, size_t buffer_size)
{
  FAT_STR fs = LABEL_atom(id_to_label (sym));

  size_t i = 0;
  cfsm_fat_char ch;

  while ((ch = CHARNUM(fs++)) != '\0' &&
	   i < buffer_size)
    buffer[i++] = ch;

  if (i == buffer_size)
    return CFSM_BUFFER_OVERFLOW;

  buffer[i] = '\0';
  return CFSM_NO_ERROR;
}


/* Return true if the given symbol name consists of a single
   character */

int cfsm_symbol_char_p (cfsm_symbol sym)
{
    if (cfsm_other_p(sym) || cfsm_epsilon_p(sym))
	return 0;
    else {
      FAT_STR fs = LABEL_atom(id_to_label (sym));

      return (CHARNUM(fs) != 0 && CHARNUM(fs+1) == 0);
    }
}    


int cfsm_symbol_multichar_p (cfsm_symbol sym)
{
    if (cfsm_other_p(sym) || cfsm_epsilon_p(sym))
	return 0;
    else {
      FAT_STR fs = LABEL_atom(id_to_label (sym));

      return (CHARNUM(fs) != 0 && CHARNUM(fs+1) != 0);
    }
}


/* A tag is a multichar symbol that does not start with
   * or @.  */


int cfsm_symbol_tag_p (cfsm_symbol sym)
{
    if (cfsm_other_p(sym) || cfsm_epsilon_p(sym))
	return 0;
    else {
      FAT_STR fs = LABEL_atom(id_to_label (sym));

      return (CHARNUM(fs) != 0 &&
	      CHARNUM(fs) != '*' &&
	      CHARNUM(fs) != '@' &&
	      CHARNUM(fs+1) != 0);
    }
}


int cfsm_symbol_char (cfsm_symbol sym, cfsm_fsm fsm)
{
    if (cfsm_other_p(sym) || cfsm_epsilon_p(sym))
	return 0;
    else {
      FAT_STR fs = LABEL_atom(id_to_label (sym));

      return CHARNUM(fs);
    }
}



/* Sigma */


/* Store the symbol set as a sequence of strings in buffer, and
   return the number of symbols.  We appear to be assuming that
   a) all the symbols are in the same package, and b) that they are
   all symbols, and not chars or integers. */


int cfsm_string_symbol_set (cfsm_fsm fsm, char* buffer, size_t buffer_size)
{
    ALPHABETptr alpha;
    int i;
    int j = 0;
    id_type* label_vector;

    cfsm_update_sigma (fsm);

    if (ALPH_len(sigma(fsm)) == 0)
	return 0;

    alpha = copy_alphabet(sigma(fsm));
    sort_alph(alpha);


    label_vector = ALPH_items(alpha);

    for (i = 0; i < ALPH_len(alpha); ++i)
    {
	id_type id = label_vector[i];
	if (id != OTHER && id != EPSILON)
	{
	    int error_code = cfsm_symbol_name
		(convert_id_to_symbol(id), buffer + j, buffer_size - j);

	    if (error_code < CFSM_NO_ERROR)
		return error_code;

	    j += strlen (buffer + j) + 1;
	}
    }
    free_alph(alpha);

    return i;
}





/* Machines */


/* cfsm_make_empty_fsm makes a new, empty fsm which recognizes
   no string.  It does have a start state, but the start state
   is not final and has no arcs. */


cfsm_fsm cfsm_make_empty_fsm (void)
{
    cfsm_fsm fsm = make_network ();
    NET_start_state(fsm) = make_state (NULL, fsm);
/*    NET_labels(fsm) = make_alph (0, LABEL_VECTOR);
    NET_sigma(fsm) = make_alph (0, BINARY_VECTOR); */
    return fsm;
}


void cfsm_set_dirty_visit_marks (int b, cfsm_fsm fsm)
{
    NET_visit_marks_dirty(fsm) = b;
}


void cfsm_init_visit_marks(int val, cfsm_fsm fsm) {
    prepare_visit_marks(fsm, val);
}


void cfsm_prune (cfsm_fsm fsm)
{
    cfsm_update_sigma(fsm);
    (void) prune_fsm(fsm, DONT_COPY);
}


void cfsm_minimize (cfsm_fsm fsm)
{
    cfsm_update_sigma(fsm);
    (void) min_fsm(fsm, DONT_COPY);
}



cfsm_fsm cfsm_union(const cfsm_fsm* fsms, int length) {
  NVtype nv;
  int i;
  nv.len = length;
  nv.nets = (NETptr*)fsms;
  for (i = 0; i < length; ++i)
      cfsm_update_sigma(fsms[i]);
  return union_fsm(&nv, DO_COPY, DO_MINIMIZE);
}


#ifndef WORLD
static int valid_label_count(cfsm_label* initial_labels, int num_initial_labels) {
    int i;
    int j = 0;
    for (i = 0; i < num_initial_labels; ++i) {
        cfsm_label l = initial_labels[i];
        if (cfsm_valid_label(l)) {
            ++j;
        }
    }
    return j;
}


static void label_list_to_alphabet(cfsm_label* initial_labels,
                                   int num_initial_labels,
                                   ALPHABETptr alph) {
      int i;
      int j = 0;
      for (i = 0; i < num_initial_labels; ++i) {
          cfsm_label l = initial_labels[i];
          if (cfsm_valid_label(l)) {
              ALPH_item(alph, j++) = convert_label_to_id(l);
          }
      }
}

/* 2002-5-30 following changes in compact.[ch] rendering
   arc-optimization not being public. TG */ 
void cfsm_optimize_arcs(cfsm_fsm net,
			cfsm_label* initial_labels,
			int num_initial_labels,
			int reorder_p) {
  ALPHABETptr initials = NULL;
  ALPHABETptr finals = NULL;

  if (!BIG_ARC_HEAP)
      BIG_ARC_HEAP = init_heap(sizeof(BIG_ARCtype), 32768/sizeof(BIG_ARCtype), "Big_Arcs");

  make_arcs_big(net);

  /* Copy the list of initials to an alphabet.  Copy only valid
     labels. */
  if (initial_labels != NULL && num_initial_labels > 0) {
      int valid_labels = valid_label_count(initial_labels, num_initial_labels);
      initials = make_alph(valid_labels, LABEL_VECTOR);
      label_list_to_alphabet(initial_labels, num_initial_labels, initials);
  }

  if (reorder_p)
      sort_arcs_by_arc_indegree(net, initials, finals);

  optimize_arc_chains
      (net, sigma_union_dont_copy(initials, finals), -1, -1, reorder_p);

  make_arcs_small(net);
  free_heap(BIG_ARC_HEAP);
  BIG_ARC_HEAP = NULL;

  free_alph(initials);
  free_alph(finals);
  cfsm_update_sigma(net);
}
#endif /* WORLD */


void cfsm_remove_arc_optimization(cfsm_fsm fsm) {
    do_unoptimize_arcs(fsm);
    cfsm_update_sigma(fsm);
}



cfsm_fsm cfsm_upper_side(cfsm_fsm m, int inplace, int minimize) {
    return upper_fsm(m, (inplace ? 0 : 1) /* copy_p */, (minimize ? 1 : 0));
}

cfsm_fsm cfsm_lower_side(cfsm_fsm m, int inplace, int minimize) {
    return lower_fsm(m, (inplace ? 0 : 1) /* copy_p */, (minimize ? 1 : 0));
}



/*
	NETptr local_from_to_sequentialize_net(NETptr fnet, 
					       int input_side, 
					       int from_level, 
					       int over_level,
					       int marking_algorithm,
					       FST_CNTXTptr fst_cntxt);


	input_side: UPPER or LOWER
	from_level: 0 or any level
	over_level: 0 (meaning go as long as you can) or any level greater
			than from_level;
	marking_algorithm:
			0: epsilon arcs
			1: all arcs
			2: mixed strategy for epsilons and ambiguities.
	fst_cntxt: the same FST_CNTXTptr that init_fsm_memory() needs, too.
*/




void cfsm_free_fsm (cfsm_fsm fsm) {
    free_network (fsm);
}


void cfsm_erase_state_names (cfsm_fsm fsm) {
    cfsm_state state;

    for (state = NET_states(fsm); state != NULL; state = next_state(state))
	cfsm_set_state_name (state, NULL, fsm);
}


void cfsm_set_state_names_smashed (int val, cfsm_fsm fsm) {
  /*    NET_names_smashed(fsm) = val; */
  /* This flag has been retired.  24.07.96 LK */
}

void clean_state_fields(cfsm_fsm net)
{
  cfsm_state next_st, sti;
  for (sti = cfsm_state_list(net) ; sti ; sti = next_st)
    {
      next_st = cfsm_next_state(sti, net);
      if (cfsm_first_arc(sti, net) && cfsm_next_arc(cfsm_first_arc(sti, net), net))
	  STATE_deterministic((STATEptr) sti) = FALSE;
      STATE_visit_mark((STATEptr) sti) = FALSE;
      STATE_client_cell((STATEptr) sti) = NULL;
      STATE_type_bit((STATEptr) sti) = FALSE;
    }
}

void cfsm_normal_form_net(cfsm_fsm net)
{
  if (net)
    {
      /**
#ifdef DEBUG
      if (  NET_sigma((NETptr) net))
	{
	  fsm_fprintf(stderr, "\n*** DEBUG NET_sigma before normal form:\n");
	  print_alphabet_s( NET_sigma((NETptr) net),  60,  0, TRUE, stderr);
	  print_net_size((NETptr) net, stderr);
	}
#endif
      clean_state_fields(net);
      **/
      cfsm_update_sigma (net);
      set_net_arity((NETptr) net);
      /**
#ifdef DEBUG
      if (  NET_sigma((NETptr) net))
	{
	  fsm_fprintf(stderr, "\n*** DEBUG NET_sigma AFTER normal form:\n");
	  print_alphabet_s( NET_sigma((NETptr) net),  60, 0, TRUE, stderr);
	}
#endif
**/

      /* Next lines do NOT conform to cfsm. TG */
      NET_deterministic( (NETptr) net) = 0 ;  /* May happen */
      NET_minimized( (NETptr) net) = 0 ; 
      clean_state_fields(net);
      reorder_arcs( (NETptr) net, default_arc_comp_fn);

      NET_deterministic( (NETptr) net) = 0 ;
      NET_pruned( (NETptr) net) = 0 ;
      NET_minimized( (NETptr) net) = 0 ;
      NET_epsilon_free( (NETptr) net) = 0 ;
      net = (cfsm_fsm) min_fsm( (NETptr) net, DONT_COPY) ;
      reorder_arcs( (NETptr) net, default_arc_comp_fn);
    }
}

cfsm_fsm flag_diacritics_two_sided(cfsm_fsm net)
{
 /* 
   Process instructions.  Convert any one-sided instructions to sequences
  such that all instructions are thereafter two-sided.  For example,
  convert:

	@U.CAP.Y@:a ==> 0:a @U.CAP.Y@
*/
     
  cfsm_state sti = NULL, st1 =  NULL;
  cfsm_arc ari;
  cfsm_label li, new_li;
  cfsm_symbol up, low;
  int was_subst = FALSE;
 
 if (! has_flag_diacritics_p((NETptr) net))
     return net;

  for (sti = cfsm_state_list(net); sti ; sti = cfsm_next_state(sti, net))
    {
      for (ari = cfsm_first_arc(sti, net) ; ari ; ari = cfsm_next_arc(ari, net))
	{
	  li = cfsm_arc_label( ari, net);
	  if ( cfsm_label_arity( li, net) > 1)
	    {
	      up = cfsm_label_upperside(li, net) ;
	      low = cfsm_label_lowerside(li, net) ;
	      /* up and low are (and MUST be) different due to implementation */
	      if (flag_diacritic_p (convert_symbol_to_id(up)))
		{
		  if (flag_diacritic_p (convert_symbol_to_id(low)))
		    {
		      /*  @U.FL1.v1@:@U.FL2.v2@ ==> @U.FL1.v1@ @U.FL2.v2@ */
		      fsm_fprintf (stderr, "\n\
*** CFSM FSOPT INTERNAL PROGRAMMING ERROR *** Please, inform the programmer.\n\
Flag diacritics (instructions) on both sides of a label pair.\n");
		      exit (1);
		    }
		  else
		    {
		      /*  @U.CAP.Y@:a ==> 0:a @U.CAP.Y@ */
		      new_li =  cfsm_make_fst_label (cfsm_epsilon_symbol (), low);
		      st1 = cfsm_add_state(FALSE, net);
		      cfsm_add_arc( cfsm_make_label(up), st1, cfsm_arc_destination (ari, net), net);
		      /* Next two lines do NOT conform to cfsm . 98-9-28 TG */
		      ARC_label( (ARCptr) ari) =  convert_label_to_id(new_li);
		      ARC_destination( (ARCptr) ari) = (STATEptr) st1;
		      was_subst = TRUE;
		    }
		}
	      else if (flag_diacritic_p (convert_symbol_to_id(low)))
		{
		  /*  b:@U.CAP.Y@ ==> b:0 @U.CAP.Y@ */
		  new_li =  cfsm_make_fst_label (up, cfsm_epsilon_symbol ());
		  st1 = cfsm_add_state(FALSE, net);
		  cfsm_add_arc( cfsm_make_label(low), st1, cfsm_arc_destination (ari, net), net);
		  /* Next two lines do NOT conform to cfsm . 98-9-28 TG */
		  ARC_label( (ARCptr) ari) =  convert_label_to_id(new_li);
		  ARC_destination( (ARCptr) ari) = (STATEptr) st1;
		  was_subst = TRUE;
		}
	    }
	}
    }

  if (was_subst)
    {
      cfsm_normal_form_net(net);
    }
  return net;
}

#endif /* SLIM_FST */

/* end of file */
