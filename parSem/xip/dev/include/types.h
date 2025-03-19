/* $Id: types.h,v 1.4 2007/02/13 12:54:04 roux Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

#ifndef C_FSM_TYPES

#define C_FSM_TYPES

#include "id_type.h"
#include "label.h"
#include "cons.h"
#include "stack-mr.h"
#include "str-buf.h"
#include "lab-vect.h"
#include "vector.h"


/******************************************************
 **
 **  types.h
 **
 **  Todd Yampol, Xerox PARC
 **  January, 1989
 **  January, 1993 (ARC & STATE declarations unified for all applications)
 **
 **  Structure definitions for finite state machines
 **
 *******************************************************/

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned short bit;
typedef unsigned char byte;


#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif

#ifndef max_of
#define max_of(A,B)    ((A)>(B))?(A):(B)
#define min_of(A,B)    ((A)<(B))?(A):(B)
#endif

#define opposite_side(S)    (S==UPPER)?LOWER:UPPER

#define STANDARD_MODE 0
#define TWOL_MODE 1

#ifndef __cplusplus
/* DON'T even think of changing this definition with C++, or you may have some serious problems... */
#define inline
#endif

/* N.B. DO_RECLAIM == DONT_COPY and DONT_RECLAIM == DO_COPY  */

enum visit_marks {NOT_VISITED = 0, IN_PROCESS = 1, DONE = 2};
enum copy_p {DONT_COPY, DO_COPY, COPY_IF_ALTERED};
enum reclaim_p {DO_RECLAIM, DONT_RECLAIM};
enum minimize_p {DONT_MINIMIZE, DO_MINIMIZE};
enum obey_flags_p {DONT_OBEY, DO_OBEY};
enum flags_special_p {FLAGS_NOT_SPECIAL, FLAGS_SPECIAL};
enum cleanup_p {DONT_CLEANUP, DO_CLEANUP};
enum compose_strategy {SEQUENTIALLY, PAIRWISE};
enum zero_to_epsilon_p {DONT_ZERO_TO_EPS, DO_ZERO_TO_EPS};
enum ignore_whitespace_p {DONT_IGNORE_WHITES, DO_IGNORE_WHITES};




/*****************
 *  ARC  object  *
 *****************/
typedef struct ARC {
#ifndef SMALL_FSM
    bit type_bit : 1; /* must be FIRST for ALTCHAINS: Arcs=0, States=1 */
    bit userflag1 : 1;
    bit visit_mark : 2;
    bit big_arc_flag : 1; /* 0 in ARC objects, 1 in BIG_ARC objects */
    bit userflag2 : 2; /* New, for local sequentialization. 1999 Jan. TG */
    bit unused : 9;   /* pad to 16 bits for alignment - use as needed */
#endif
    id_type label; /* index into label table -- use id_to_label() to access print name */
    struct STATE *destination;
    struct ARC *next;
#ifdef FULL_FSM
    void *user_pointer; /* only ALTCHAINS uses this field at the moment */
#endif
} ARCtype, *ARCptr;


#ifndef SMALL_FSM
/*********************
 *  BIG_ARC  object  *
 *********************
 This is used to replicate the user_pointer facility of the
 ARC object without need to compile with FULL_FSM flags on;
 the only noticeable difference from the definition of the ARC
 object is that the field user_pointer is always included.
 NOTE: the BIG_ARCs don't exist in SMALL_FSM
 */
typedef struct BIG_ARC {
    bit type_bit : 1; /* must be FIRST for ALTCHAINS: Arcs=0, States=1 */
    bit userflag1 : 1;
    bit visit_mark : 2;
    bit big_arc_flag : 1; /* 0 in ARC objects, 1 in BIG_ARC objects */
    bit unused : 11;   /* pad to 16 bits for alignment - use as needed */
    id_type label; /* index into label table -- use id_to_label() to access print name */
    struct STATE *destination;
    struct BIG_ARC *next;
    void *user_pointer; /* only ALTCHAINS uses this field at the moment */
} BIG_ARCtype, *BIG_ARCptr;
#endif

#ifndef SMALL_FSM
#define ARC_type_bit(X)			(X)->type_bit
#define ARC_userflag1(X)		(X)->userflag1
#define ARC_visit_mark(X) 		(X)->visit_mark
#define ARC_big_arc_flag(X)                  (X)->big_arc_flag
#define ARC_userflag2(X)		        (X)->userflag2
#endif

#ifdef FULL_FSM
#define ARC_user_pointer(X)		(X)->user_pointer
#endif

#define ARC_label(X) 			(X)->label
#define ARC_destination(X) 		(X)->destination
#define next_arc(X)			(X)->next
  
/***********************
 *  ARC_VECTOR object  *
 ***********************/
typedef struct ARC_VECTOR {
	struct STATE **dest_array; 	/* pointer to the ARC array */
	struct ALPHABET *u_eps;
	struct ALPHABET *l_eps;
	struct ALPHABET *u_flag_diacr;  /* Flag diacritics: 10.10.97 TG */
	struct ALPHABET *l_flag_diacr;
} AVtype, *AVptr;

#define AV_dest_array(X) (X)->dest_array
#define AV_u_eps(X) 	 (X)->u_eps
#define AV_l_eps(X)	 (X)->l_eps
#define AV_dest(X, Y)    (X)->dest_array[(Y)]
#define AV_u_fld(X) 	 (X)->u_flag_diacr
#define AV_l_fld(X)	 (X)->l_flag_diacr

/******************
 *  STATE object  *
 ******************/
typedef struct STATE {
        bit type_bit : 1; /* must be FIRST for ALTCHAINS. Arcs=0, States=1*/
	bit final : 1;
	bit deterministic : 1;
	bit vector_p : 1;
        bit visit_mark : 8;	/* 8 bits because of byte-fetch */
        bit userflag2 : 2;
        bit virtuel: 1;
	bit unused : 1;	   /* pad to 16 bits for alignment - use as needed */
	union { 
		struct ARC *set;
		struct ARC_VECTOR *vector;
	} arc;
#ifndef SMALL_FSM
	void *client_cell;
  struct STATE *next;
#endif
} STATEtype, *STATEptr;

#define STATE_type_bit(X)	(X)->type_bit
#define STATE_visit_mark(X) 	(X)->visit_mark
#define STATE_final(X)		(X)->final
#define STATE_deterministic(X) 	(X)->deterministic
#define STATE_vector_p(X)	(X)->vector_p
#define STATE_arc_set(X)	(X)->arc.set
#define STATE_arc_vector(X) 	(X)->arc.vector
#define STATE_userflag2(X)	(X)->userflag2
#define STATE_virtuel(X)	(X)->virtuel

#ifndef SMALL_FSM
#define STATE_client_cell(X)	(X)->client_cell
#define next_state(X)			(X)->next
#else
typedef struct {
  bit visit_mark : 2;
  void *client_cell;
  struct STATE *next;
} DUMMYtype;

extern DUMMYtype DUMMY;

#define ARC_visit_mark(X) 		DUMMY.visit_mark
#define STATE_client_cell(X)	DUMMY.client_cell
#define next_state(X)			DUMMY.next
#endif 
  
/* RANGE_RECORD object */
typedef struct RANGE_RECORD {
  id_type id;
  int len;
  id_type *items;
} RANGEtype, *RANGEptr;

#define RANGE_id(X)		(X)->id
#define RANGE_len(X)	        (X)->len
#define RANGE_items(X)	        (X)->items
#define RANGE_item(X, Y)	((X)->items)[Y]

/* RANGE_VECTOR object */
typedef struct RANGE_VECTOR {
  int len;
  RANGEptr vector;
} RANGE_VECTORtype, *RANGE_VECTORptr;

#define RANGE_VECTOR_len(X)      (X)->len
#define RANGE_VECTOR_vector(X)   (X)->vector

/* MATCH_TABLE object */
typedef struct MATCH_TABLE {
  int len;
  RANGE_VECTORptr *match_table;
  RANGEptr eqv_classes;
} MATCH_TABLEtype, *MATCH_TABLEptr;

#define MATCH_TABLE_len(X)      (X)->len
#define MATCH_TABLE_table(X)    (X)->match_table
#define MATCH_TABLE_class(X)    (X)->eqv_classes

typedef struct PARSE {
  int max_size;
  id_type *result;
} PARSEtype, *PARSEptr;

#define PARSE_max_size(X)	(X)->max_size
#define PARSE_result(X)		(X)->result


  
/***********************************
 *  PROPERTY LIST generic objects  *
 ***********************************/
/* IO_SYMBOLs, IO_SYMBOL_PACKAGEs, OBJECTs and SEQUENCEs */ 
typedef struct IO_SYMBOL {
  FAT_STR	sym_name;
} IO_SYMBOLtype, *IO_SYMBOLptr;

#define IO_SYM_pack(X) (X)->error  /* To catch old pre-ATOM_REMOVE code */

#define IO_SYM_name(X) (X)->sym_name
  
typedef struct IO_SYMBOL_PACKAGE {
  FAT_STR *sym_names;
} IO_SYMBOL_PACKAGEtype, *IO_SYMBOL_PACKAGEptr;

#define SYM_PACK_name(X)   (X)->error  /* To catch old pre-ATOM_REMOVE code */
  
#define SYM_PACK_sym_names(X) (X)->sym_names
#define SYM_PACK_sym(X, Y)    (X)->sym_names[(Y)]
  
typedef struct BYTE_BLOCK {
  unsigned long length;
  void *block;
} BYTE_BLOCKtype, *BYTE_BLOCKptr;

#define BYTE_BLOCK_len(X)	(X)->length
#define BYTE_BLOCK_block(X)	(X)->block
  
typedef struct OBJECT {
  int type;
  union {
    long int integer;
    FAT_CHAR character;
    FAT_STR str;
    IO_SYMBOLptr symbol;
    struct SEQUENCE *array;
    struct SEQUENCE *list;
    struct BYTE_BLOCK *byte_block;
  } data;
} OBJECTtype, *OBJECTptr;

#define OBJ_type(X)      (X)->type
#define OBJ_integer(X)	 (X)->data.integer
#define OBJ_character(X) (X)->data.character
#define OBJ_str(X)       (X)->data.str
#define OBJ_symbol(X)    (X)->data.symbol
#define OBJ_array(X)     (X)->data.array
#define OBJ_list(X)      (X)->data.list
#define OBJ_byte_block(X)   (X)->data.byte_block
  
typedef struct SEQUENCE {
  int len;
  int type;
  union {
    OBJECTptr *objects;
    long int *ints;
    FAT_CHAR *characters;
    FAT_STR *strings;
    IO_SYMBOL_PACKAGEptr symbol_package;
    IO_SYMBOLptr symbols;
    struct SEQUENCE **array;
    struct SEQUENCE **list;
    struct BYTE_BLOCK **byte_block;
  } data;
} SEQUENCEtype, *SEQUENCEptr;

#define SEQ_len(X)	(X)->len
#define SEQ_type(X)	(X)->type
  
#define SEQ_objects(X)        (X)->data.objects
#define SEQ_ints(X)           (X)->data.ints
#define SEQ_characters(X)     (X)->data.characters
#define SEQ_strings(X)        (X)->data.strings
#define SEQ_symbol_package(X) (X)->data.symbol_package
#define SEQ_symbols(X)        (X)->data.symbols
#define SEQ_array(X)          (X)->data.array
#define SEQ_list(X)           (X)->data.list
#define SEQ_byte_block(X)     (X)->data.byte_block
  
enum OBJECT_AND_SEQUENCE_TYPES 
	   {Object         = -1, 
	    Int            =  0,
	    Character      =  1, 
	    String         =  2, 
	    Symbol_package = -3,
	    Symbol         =  3,
	    Array          =  4,
	    List           =  5,
	    Byte_block     =  6};

typedef struct PROP {
  FAT_STR attribute;
  OBJECTptr value;
  struct PROP *next;
} PROPtype, *PROPptr;

#define PROP_attr(X)	(X)->attribute
#define PROP_val(X)     (X)->value
#define next_prop(X)	(X)->next

/**********************
 *  ALPHABET  object  *
 **********************/

  /*****
   * Notes on the ALPHABET object:
   *
   * An ALPHABET has two forms, LABEL and BINARY.  
   *
   * In the LABEL form, the alphabet is a vector
   * of label ids.  
   *
   * In the BINARY form, the alphabet
   * is a vector of boolean values (0 and 1).  If 
   * alphabet item N is 1, then label N is in the
   * alphabet.  If the item is 0, then N is not
   * in the alphabet.  Also, if N is larger than 
   * the length of the alphabet, then N is not
   * in the alphabet.
   *
   * Thus for the alphabet 0 1 2 5, the LABEL form
   * alphabet would be:  
   * type = LABEL_VECTOR, len = 4, items = [0, 1, 2, 5].
   *
   * The BINARY form alphabet would be:
   * type = BINARY_VECTOR, len = 6, items = [1, 1, 1, 0, 0, 1].
   *
   * Each representation has advantages and disadvantages.  Membership
   * is unit time for BINARY, and linear for LABEL.  However, the
   * actual values of the labels are immediately availible in LABEL, but
   * must be computed from the BINARY.
   *
   * The functions binary_to_label() and label_to_binary() convert
   * between the two forms.
   *
   *****/

typedef struct ALPHABET {
  int16 type;
  int len;	/* # of ALPH_items positions in use */
  int max;  /* actual size of ALPH_items */
  id_type *items;
} ALPHABETtype, *ALPHABETptr;

enum alph_types {BINARY_VECTOR, LABEL_VECTOR};

#define ALPH_type(X)	(X)->type
#define ALPH_items(X)	(X)->items
#define ALPH_item(X,Y)	(X)->items[(Y)]
#define ALPH_len(X)		(X)->len
#define ALPH_max(X)		(X)->max
  


/*************************
 *  ALPH_VECTOR  object  *
 *************************/

/*  ALPH_VECTOR is a collection of alphabets.  It is used to
    represent partions of the global alphabet to equivalence
    classes.
    */

typedef struct ALPH_VECTOR {
    int length;
    ALPHABETptr *alphabets;
    struct ALPH_VECTOR *next;
  } ALPH_VECTORtype, *ALPH_VECTORptr;

#define av_len(X)					(X)->length
#define av_alphs(X)					(X)->alphabets
#define av_alph(X,Y)				(X)->alphabets[(Y)]
#define next_av(X)					(X)->next

/*********************
 *  CH_NODE  object  *
 *********************/

typedef struct CH_NODE {
    id_type id;
    struct CH_NODE **next;
} CH_NODEtype, *CH_NODEptr, **PARSE_TBL;

#define CH_NODEid(X)	(X)->id
#define CH_NODEnext(X)	(X)->next


/*********************
 *  NETWORK  object  *
 *********************/
typedef struct NETWORK {
  struct {
    bit deterministic:1;
    bit pruned:1;
    bit completed:1;
    bit minimized:1;
    bit epsilon_free:1;
    bit sorted_states:1;
    bit loop_free:1;
    bit twol_net:1; /* NET created in TWOL_MODE */
    bit visit_marks_dirty:1;
    bit names_matter:1;
    bit shared_arc_lists:1;
    bit has_arc_user_pointer:1;
    bit closed_sigma:1;
    bit start_state_final:1;
    bit tabular:1;
    bit compacted:1;
    bit upper_seq:1;
    bit lower_seq:1;
    bit mark:1;
    bit u_flag_diacr:1;
    bit l_flag_diacr:1;
    bit left_bimachine:1;
    bit right_bimachine:1;
    bit sorted_arcs:1;
    bit reduced_labelset:1;
    bit Kaplan_compressed:1;
    bit virtuel:1;
    bit unused: 5;
  } flags;

  int16 arc_label_arity;
  int16 range_len;
  RANGEptr uprange_map;
  RANGEptr downrange_map;
  MATCH_TABLEptr upmatch_table;
  MATCH_TABLEptr downmatch_table;
  ALPHABETptr labels;
  ALPHABETptr sigma;
  ALPHABETptr recode_key;
  ALPHABETptr unreduce_key;
  union {
    STATEptr state;
    unsigned char *loc;
  } start;
  union {
    STATEptr states;
    unsigned char *block;
    } body;
  HEAPptr arc_vector_heap;
  int arc_vector_len;
  PROPptr networkprops;
  PARSE_TBL upper_parse_table;
  PARSE_TBL lower_parse_table;
  long num_states;
  long num_arcs;
  long block_size;
  ALPHABETptr flag_register;
  void *client_cell;
} NETtype, *NETptr;


#define NET_deterministic(X) 		(X)->flags.deterministic
#define NET_pruned(X) 				(X)->flags.pruned
#define NET_completed(X) 			(X)->flags.completed
#define NET_minimized(X) 			(X)->flags.minimized
#define NET_epsilon_free(X) 		(X)->flags.epsilon_free
#define NET_sorted_states(X) 		(X)->flags.sorted_states
#define NET_loop_free(X) 			(X)->flags.loop_free
#define NET_visit_marks_dirty(X) 	(X)->flags.visit_marks_dirty
#define NET_names_matter(X) 		(X)->flags.names_matter
#define NET_shared_arc_lists(X) 	(X)->flags.shared_arc_lists
#define NET_has_arc_user_pointer(X)	(X)->flags.has_arc_user_pointer
#define NET_closed_sigma(X) 		(X)->flags.closed_sigma
#define NET_start_state_final(X)	(X)->flags.start_state_final
#define NET_tabular(X)		        (X)->flags.tabular
#define NET_twol_net(X)			(X)->flags.twol_net
#define NET_compacted(X)		(X)->flags.compacted
#define NET_upper_seq(X)		(X)->flags.upper_seq
#define NET_lower_seq(X)	        (X)->flags.lower_seq
#define NET_mark(X)			(X)->flags.mark
#define NET_u_flag_diacr(X)		(X)->flags.u_flag_diacr
#define NET_l_flag_diacr(X)		(X)->flags.l_flag_diacr
#define NET_left_bimachine(X)		(X)->flags.left_bimachine
#define NET_right_bimachine(X)		(X)->flags.right_bimachine
#define NET_sorted_arcs(X)		(X)->flags.sorted_arcs
#define NET_reduced_labelset(X)		(X)->flags.reduced_labelset
#define NET_Kaplan_compressed(X)	(X)->flags.Kaplan_compressed
#define NET_virtuel(X)			(X)->flags.virtuel

#define NET_arc_label_arity(X)  	(X)->arc_label_arity
#define NET_labels(X) 				(X)->labels
#define NET_sigma(X)				(X)->sigma
#define NET_recode_key(X)			(X)->recode_key
#define NET_unreduce_key(X)			(X)->unreduce_key
#define NET_start_state(X) 			(X)->start.state
#define NET_start_loc(X) 			(X)->start.loc
#define NET_states(X) 				(X)->body.states
#define NET_arc_block(X) 			(X)->body.block
#define NET_arc_vector_heap(X)		(X)->arc_vector_heap
#define NET_arc_vector_len(X)		(X)->arc_vector_len
#define NET_range_len(X)			(X)->range_len
#define NET_properties(X)			(X)->networkprops
#define NET_upper_parse_table(X)	(X)->upper_parse_table
#define NET_lower_parse_table(X)	(X)->lower_parse_table
#define NET_uprange_map(X)			(X)->uprange_map
#define NET_downrange_map(X)		(X)->downrange_map
#define NET_upmatch_table(X)		(X)->upmatch_table
#define NET_downmatch_table(X)		(X)->downmatch_table


#define NET_num_states(X) 		(X)->num_states
#define NET_num_arcs(X) 		(X)->num_arcs
#define NET_block_size(X)		(X)->block_size
#define NET_client_cell(X)		(X)->client_cell
#define NET_flag_register(X)		(X)->flag_register

#define has_arc_vectors_p(X)	(X)->arc_vector_heap

/******************************
 *  TYPES FOR COMPACTED NETS  *
 ******************************/

typedef struct COMPACT_STATE {
  unsigned char *state_loc;
  long int addr;
  int reclaimable;
  int final;
  int visit_mark;
} CStype, *CSptr;

#define CS_loc(X)			(X)->state_loc
#define CS_mark(X)			(X)->visit_mark
#define CS_final(X)			(X)->final
#define CS_addr(X)			(X)->addr
#define CS_reclaimable(X)       	(X)->reclaimable

#ifdef MAC_OS

/* Useless on Sparcstations because size is not divisible by 4 */

typedef struct COMPACT_ARC {
  id_type label;
  struct COMPACT_ARC *next;
} CAtype, *CAptr;

#define CA_label(X)			(X)->label
#define CA_next(X)			(X)->next

#endif

typedef struct MEDIUM_ARC {
  unsigned char last_arc;
  unsigned char dest_final; 
  unsigned short label;
  struct MEDIUM_ARC *next;
} MAtype, *MAptr;

#define MA_last_arc(X)		(X)->last_arc
#define MA_dest_final(X)	(X)->dest_final
#define MA_label(X)			(X)->label
#define MA_next(X)			(X)->next

#define CS_HASH_SIZE 128


typedef struct NET_VECTOR {
	int len;
	NETptr *nets;
} NVtype, *NVptr;

#define NV_len(X)   (X)->len
#define NV_nets(X)  (X)->nets
#define NV_net(X,Y) (X)->nets[(Y)]


/************************
 *  NET-VECTOR VECTORS  *
 ************************/

typedef struct NET_VECTOR_VECTOR {
	int len;
	NVptr *net_vectors;
} NVVtype, *NVVptr;

#define NVV_len(X)     (X)->len
#define NVV_nvs(X)     (X)->net_vectors
#define NVV_nv(X,Y)    (X)->net_vectors[(Y)]
#define NVV_nvlen(X,Y) NV_len( NVV_nv(X,Y))
#define NVV_nets(X,Y)  NV_nets( NVV_nv(X,Y))
#define NVV_net(X,Y,Z) NV_net( NVV_nv(X,Y), Z)


/************************************
 *  TYPES FOR PARALLEL REPLACEMENT  *
 ************************************/

/* A replacement structure contains all arguments for 
   parallel replacement.
   */

typedef struct REPL_STRUCT {
  NVptr nv;        /* net vektor of UPPER LOWER and/or LEFT RIGHT */
  int arrow;       /* arrow type:  ->  <-  <->  (->)  (<-)  (<->) */
  int ctxdir;      /* context direction:  ||  //  \\  \/  */
} RStype, *RSptr;

#define RS_nv(X)     (X)->nv
#define RS_len(X)    (X)->nv->len
#define RS_net(X,Y)  (X)->nv->nets[(Y)]
#define RS_arrow(X)  (X)->arrow
#define RS_ctxdir(X) (X)->ctxdir


/**************************
 *  STANDARD FILE HEADER  *
 **************************/

/* The standard file header for binary fsm files consists of a
   fixed 20 byte field (FILE_STAMP) with the following fields:
   LENGTH (4), TYPE (1), VERSION(1), COPYRIGHT_LOCATION (1),
   CREATION_DATE (4), HEADER_LENGTH (2), NET_COUNT (3),
   and 4 unused reserved zero bytes.  After the fixed-length
   file stamp come two variable length fields that contain ASCII
   characters: CREATION_DATE and COPYRIGHT_NOTICE.  The STANDARD_
   HEADER structure contains the information written to the file
   as well as the name of the file and the whether the file was
   read or saved.
   */

typedef struct STANDARD_HEADER {
	unsigned long int file_length;   /* 32 bits */
	unsigned int file_type:8;
	unsigned int file_version:8;
	unsigned int copyright_loc:8;
	unsigned long int creation_date; /* 32 bits */
	unsigned int header_length:16;
	unsigned long int net_count;	 /* 24 bits */
	unsigned long int reserved;	 /* 4 zero bytes */
	char *ascii_date;
	int date_max;
	char *copyright_notice;
	int copyright_max;
	char *file_name;
	int file_name_max;
	int output_p;
	int max_net_count;
	unsigned long *start_locations;  
} STANDARD_HEADER, *STANDARD_HEADERptr;

#define HEADER_file_length(X)		(X)->file_length
#define HEADER_file_type(X)			(X)->file_type
#define HEADER_file_version(X)		(X)->file_version
#define HEADER_copyright_loc(X)		(X)->copyright_loc
#define HEADER_creation_date(X)		(X)->creation_date
#define HEADER_header_length(X)		(X)->header_length
#define HEADER_net_count(X)			(X)->net_count
#define HEADER_reserved(X)			(X)->reserved
#define HEADER_ascii_date(X)		(X)->ascii_date
#define HEADER_date_max(X)			(X)->date_max
#define HEADER_copyright_notice(X)	(X)->copyright_notice
#define HEADER_copyright_max(X)		(X)->copyright_max
#define HEADER_file_name(X)	        (X)->file_name
#define HEADER_file_max(X)	        (X)->file_name_max
#define HEADER_output_p(X)	        (X)->output_p
#define HEADER_max_net_count(X)		(X)->max_net_count
#define HEADER_start_locations(X)	(X)->start_locations
#define HEADER_net_location(X, Y)       ((X)->start_locations)[Y]
#define FILE_STAMP_LENGTH 20

typedef struct C_FSM_CONTEXT {      /* Current global variables .*/
  int mode ;
  char *copyright_string ;          /* COPYRIGHT_OWNER */
  int compose_strategy ;
  struct flag_parameters
  {
    bit copy_p ;
    bit determinize_p ;
    bit interactive_p ;            /* fst_IY_INTERACTIVE */
    bit last_errors_p ;            /* fst_LAST_ERRORS */
    bit lex_errors_p ;             /* fst_LAST_ERRORS */
    bit minimize_p ;
    bit name_nets_p ;              /* fst_IY_NAME_NETS */
    bit obey_flags_p ;
    bit prune_p ;
    bit quit_on_fail_p ;           /* fst_IY_QUIT_OF_FAIL */
    bit reclaim_p ;
    bit recursive_define_p ;       /* fst_IY_RECURSIVE_DEFINE */
    bit sq_final_strings_arcs;     /* SQ_FINAL_STRINGS_ARCS */
    bit sq_intern_strings_arcs;    /* SQ_INTERN_STRINGS_ARCS */
    bit sq_string_onelong;         /* SQ_STRING_ONELONG */
    bit unicode_p ;                /* UNICODE_P */
    bit verbose_p ;                /* fst_IY_VERBOSE */
    bit embedded_command_p;        /* for interface->regex flex-ers 99-6-15 TG */
  } flags ;
  struct pretty_print_parameters
  {
    int cur_pos ;                 /* cur_pos */
    int indent ;
    int line_pos ;
    char *output_buffer ;         /* output_buffer */
    int output_buffer_size ;      /* OUTPUT_BUFFER_SIZE */
    int right_margin ;
  } pretty_print ;
  struct path_index_data
  {
    int max_path_index_pos ;         /* MAX_PATH_INDEX_POS */
    int path_index_incr ;            /* PATH_INDEX_INCR */
    int path_index_pos ;             /* PATH_INDEX_POS */
    long int *path_index_vector ;    /* PATH_INDEX_VECTOR */
  } index ;
  struct parse_parameters_and_data
  {
    bit ignore_white_space_p ;
    bit zero_to_epsilon_p ;
    int input_seq_size ;            /* WORD_STRING_SIZE */
    id_type *input_seq ;              /* INPUT_SEQ */
    id_type *lower_match ;            /* LOWER_MATCH */
    id_type *match_table ;            /* MATCH_TABLE */
    id_type *upper_match ;            /* UPPER_MATCH */
  } parse;
  struct bin_io_parameters_and_data
  {
    bit altchain_p ;                /* ALTCHAIN_P */
    bit status_bar_p ;              /* DISPLAY_READ_STATUS_BAR */
    long int status_bar_increment ; /* STATUS_BAR_INCREMENT */
    unsigned long arc_count ;       /* ARC_COUNT */
    byte cur_byte ;                 /* CUR_BYTE */
    STANDARD_HEADERptr last_header; /* LAST_HEADER */
    STANDARD_HEADERptr next_header; /* NEXT_HEADER */
    STATEptr  cur_state ;           /* CUR_STATE */
    STATEptr  *state_stack ;        /* STATE_STACK */
    char **attributes ;             /* STANDARD_ATTRIBUTES */
    int attribute_count ;           /* STANDARD_ATTRIBUTE_COUNT */
  } bin_io ;
  struct define_data
  {
    HASH_TABLEptr net_table;        /* DEF_TABLE */
    HASH_TABLEptr set_table;
  } define;

  } FST_CNTXT, *FST_CNTXTptr;

#define FST_mode(X)                   (X)->mode

#define FST_copyright_string(X)       (X)->copyright_string

#define FST_compose_strategy(X)       (X)->compose_strategy

#define FST_copy_p(X)                 (X)->flags.copy_p
#define FST_determinize_p(X)          (X)->flags.determinize_p
#define FST_interactive_p(X)          (X)->flags.interactive_p
#define FST_last_errors_p(X)          (X)->flags.last_errors_p
#define FST_lex_errors_p(X)           (X)->flags.lex_errors_p
#define FST_minimize_p(X)             (X)->flags.minimize_p
#define FST_name_nets_p(X)            (X)->flags.name_nets_p
#define FST_obey_flags_p(X)           (X)->flags.obey_flags_p
#define FST_prune_p(X)                (X)->flags.prune_p
#define FST_quit_on_fail_p(X)         (X)->flags.prune_p
#define FST_sq_final_strings_arcs(X)  (X)->flags.sq_final_strings_arcs
#define FST_sq_intern_strings_arcs(X) (X)->flags.sq_intern_strings_arcs
#define FST_sq_string_onelong(X)      (X)->flags.sq_string_onelong
#define FST_reclaim_p(X)              (X)->flags.reclaim_p
#define FST_unicode_p(X)              (X)->flags.unicode_p
#define FST_verbose_p(X)              (X)->flags.verbose_p
#define FST_embedded_command_p(X)     (X)->flags.embedded_command_p

#define FST_cur_pos(X)                (X)->pretty_print.cur_pos
#define FST_indent(X)                 (X)->pretty_print.indent
#define FST_line_pos(X)               (X)->pretty_print.line_pos
#define FST_output_buffer(X)          (X)->pretty_print.output_buffer
#define FST_output_buffer_size(X)     (X)->pretty_print.output_buffer_size
#define FST_right_margin(X)           (X)->pretty_print.right_margin

#define FST_max_path_index_pos(X)     (X)->index.max_path_index_pos
#define FST_path_index_pos(X)         (X)->index.path_index_pos
#define FST_path_index_vector(X)      (X)->index.path_index_vector

#define FST_ignore_white_space_p(X)   (X)->parse.ignore_white_space_p
#define FST_zero_to_epsilon_p(X)      (X)->parse.zero_to_epsilon_p
#define FST_input_seq_size(X)         (X)->parse.input_seq_size
#define FST_input_seq(X)              (X)->parse.input_seq
#define FST_lower_match(X)            (X)->parse.lower_match
#define FST_match_table(X)            (X)->parse.match_table
#define FST_upper_match(X)            (X)->parse.upper_match

#define FST_altchain_p(X)             (X)->bin_io.altchain_p
#define FST_status_bar_p(X)           (X)->bin_io.status_bar_p
#define FST_status_bar_increment(X)   (X)->bin_io.status_bar_increment
#define FST_arc_count(X)              (X)->bin_io.arc_count
#define FST_cur_byte(X)               (X)->bin_io.cur_byte
#define FST_last_header(X)            (X)->bin_io.last_header
#define FST_next_header(X)            (X)->bin_io.next_header
#define FST_cur_state(X)              (X)->bin_io.cur_state
#define FST_state_stack(X)            (X)->bin_io.state_stack
#define FST_attributes(X)             (X)->bin_io.attributes
#define FST_attribute_count(X)        (X)->bin_io.attribute_count

#define FST_defined_nets(X)           (X)->define.net_table
#define FST_defined_sets(X)           (X)->define.set_table

/**************************
 *  Data structure for arc label statistics  *
 *  (Moved here from bin-out.h -- LK 98-09-25)
 **************************/

typedef struct TALLY {
  id_type label;
  long freq;
} TALLYcell, *TALLYptr;

#define TALLY_label(X) (X)->label
#define TALLY_freq(X)  (X)->freq

/* -------------------------------------------------------------------- */
/* The VirtuelInfo has information about the virtuel operation being    */
/* simulated, including methods for computing the arc set and the final */
/* bit.  It is stored as a network property and in each virtuel state.  */
/* -------------------------------------------------------------------- */

/* ------------------------------------------------------------------------ */
/* DATA TYPES AND ACCESS FUNCTIONS FOR VIRTUEL NETWORKS                     */
  /* Moved here from John Maxwell's virtuel-fsm.h.                          */
/* ------------------------------------------------------------------------ */

typedef struct VirtuelInfo VirtuelInfo;
typedef struct StatePair StatePair;
typedef void StateData;
//typedef enum StateDataType StateDataType;
typedef struct VMethods VMethods;
typedef int (*FinalPFN)(STATEptr vstate);
typedef ARCptr (*ArcSetFN)(STATEptr vstate, VirtuelInfo *vinfo);
typedef void (*ExpandOtherFN)(ALPHABETptr new_sigma, NETptr vnetwork,
			      VirtuelInfo *vinfo);

struct VirtuelInfo {
  VMethods *methods;
  NETptr op1;     /* All n-ary operations must be converted to 
		     binary operations. */
  NETptr op2;     /* If op2 is NULL, this is a unary operation. */
  void *btree;             /* For state construction */
  void *btree_heap;        /* For state construction */
  void *extra_info;        /* Operation-specific info */
  NETptr network;          /* A pointer back to the virtuel network */
  int n_dead_states;       /* Possibly useful for pruning */
};

/* -------------------------------------------------------------------- */
/* The data type of the state data varies from operation to operation,  */
/* but is fixed for a particular operation.  For simplicity we store    */
/* the data type in the VMethods, which is the same for each operation. */
/* -------------------------------------------------------------------- */
enum StateDataType {DATA_STATE, /* a single state (for unary ops) */
		    STATE_PAIR, /* a pair of states (for binary ops) */
		    STATE_LIST  /* a state list (for determinize) */
};

struct VMethods {
  char *operation;     /* The name of the operation */
  StateDataType data_type;  /* The data type of the state data */
  FinalPFN final_p_fn; /* Determines whether a virtuel state is final. */
  ArcSetFN arc_set_fn; /* Returns the arcs for a virtuel state. */
  ExpandOtherFN expand_other_fn;
};

struct StatePair {
  STATEptr state1;
  STATEptr state2;
};

#define NET_virtuel_info(X)   (X)->client_cell
#define STATE_state_data(X)   (X)->client_cell
/* The virtuel info is not needed once the arcset is constructed, 
   hence can temporarily reside in that field. */
#define STATE_virtuel_info(X) (VirtuelInfo *)(X)->arc.set



#ifdef __cplusplus
}
#endif
#endif
