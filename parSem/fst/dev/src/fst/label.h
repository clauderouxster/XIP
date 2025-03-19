/* $Id: label.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990-94 Xerox Corporation. All rights reserved. */
/*************************
 *
 *  LABEL.H
 *
 ***************************/

#ifndef C_FSM_LABEL

#define C_FSM_LABEL

#include "fst/syst-dep.h"
#include "fst/hash-tbl.h"
#include "fst/fat-str.h"
#include "fst/id_type.h"

#ifdef __cplusplus
extern "C" {
#endif

  /* Redefined in types.h */

/* These Lisp-derived distinctions are now obsolete. 1999-05-31. LK

typedef struct ATOM {
	int type;
	FAT_STR package;
	FAT_STR name;
} ATOMtype, *ATOMptr;

#define ATOM_type(X) 	(X)->type
#define ATOM_package(X) (X)->package
#define ATOM_name(X) 	(X)->name
#else
#define ATOM_type(X) 	X->error
#define ATOM_package(X) X->error
#define ATOM_name(X) 	X->error

Removed by Caroline Privault and Lauri Karttunen.

*/



typedef struct TUPLE {
	id_type *labels;
	/* a vector of label IDs */
	id_type inverse;
	/* it is equal to ID_NO_SYMBOL for labels of arity 1 and for tuples whose inverse
	   has not been computed; otherwise it contains the unique ID of the inverse
	   tuple. */
} TUPLEtype, *TUPLEptr;

#define TUPLE_labels(X)		(X)->labels
#define TUPLE_inverse(X)	(X)->inverse

enum {UPPER = 0, LOWER = 1, BOTH_SIDES = 2, UPPER_OR_LOWER = 3};
/* Flag diacritics */
enum {NO_ACTION = 0, CLEAR_SETTING = 1, POSITIVE_SETTING = 2,
      NEGATIVE_SETTING = 3, UNIFY_TEST = 4, DISALLOW_TEST = 5,
      REQUIRE_TEST = 6, FAIL_ACTION = 7, INSERT_SUBNET = 8,
      SET_TO_ATTR = 9, EQUAL_ATTR_TEST = 10};
#define UNDEFINED_FLAG_ATTRIBUTE 0
#define UNDEFINED_FLAG_VALUE 0

typedef struct FLAG_DIACRITIC {
  int action;
  id_type attribute;
  id_type value;
} FLAG_DIACRtype, *FLAG_DIACRptr;

#define FLAG_DIACR_action(X)	(X)->action
#define FLAG_DIACR_attribute(X)	(X)->attribute
#define FLAG_DIACR_value(X)	(X)->value

typedef struct LABEL {
	id_type id;
	/* unique ID for label, as returned by INTERN... */
	short arity;
      /* 1 for atomic label, 2 or more for tuples */
	void *data;
	/* a cache for storing some information about the label
	   such as type or print name */
        FLAG_DIACRptr flag;
	union {
	  FAT_STR atom;
	  TUPLEptr tuple;
	} content;
} LABELtype, *LABELptr;

#define LABEL_id(X) 	(X)->id
#define LABEL_arity(X) 	(X)->arity
#define LABEL_data(X)	(X)->data
#define LABEL_flag(X)	(X)->flag
#define LABEL_atom(X) 	(X)->content.atom
#define LABEL_tuple(X) 	(X)->content.tuple

#define LABEL_flag_action(X) \
       (LABEL_flag(X) ? FLAG_DIACR_action(LABEL_flag(X)) : 0)
#define LABEL_flag_attrib(X) \
       (LABEL_flag(X) ? FLAG_DIACR_attribute(LABEL_flag(X)) : 0)
#define LABEL_flag_value(X) \
       (LABEL_flag(X) ? FLAG_DIACR_value(LABEL_flag(X)) : 0)

#define fstpair_upper(X)	TUPLE_labels(LABEL_tuple(X))[UPPER]
#define fstpair_lower(X)	TUPLE_labels(LABEL_tuple(X))[LOWER]
#define nth_tuple_id(X, Y)	TUPLE_labels(LABEL_tuple(X))[Y]

typedef struct LABEL_ID_MAP {
	HASH_TABLEptr hash_table;
	uint16 id_count;  /* current number of label ids */
	uint16 id_max;	/* current size of ID_TO_ELEMENT_MAP */
	int alloc_size; /* size of increments to MAX_ELEMENTS */
	LABELptr *map;
} LABEL_ID_MAPtype, *LABEL_ID_MAPptr;

#define LIM_hash_table(X) (X)->hash_table
#define LIM_id_count(X) (X)->id_count
#define LIM_alloc_size(X) (X)->alloc_size
#define LIM_id_max(X) (X)->id_max
#define LIM_map(X) (X)->map

extern FstBaseExport LABEL_ID_MAPptr LABEL_MAP;
extern HASH_TABLEptr LABEL_TABLE;
/* declared in label.c */


/*
 Note that the maximum label id is limited to being less than
1<<15 because (1) label id's are two byte integers and (2) negative
id, such as -1, are used to indicate termination of label id vectors
*/

#define MAX_LV 16 /*  -- max of 2^16 labels: for constructing the heaps 
                         of label vectors in LV_HEAPS */
/* #define MAX_LABEL 32767 */
/* #define MAX_LABEL ((unsigned) (1<<15) - 1)  */
/* Id extension: 
   MAX_LABEL was 32767. With the new id type "uint16"
   it should be extended to 65535, however the greatest 
   value is dedicated to special reserved id-s such as 
   ID_EOS or ID_NO_SYMBOL. Hence MAX_LABEL should be 65534 */
#define MAX_LABEL ( (uint16) -2 )

#define LABEL_BUCKETS 257
	/* LABEL_MAP hash keys range from 0 to LABEL_BUCKETS-1 */
#define LABEL_ALLOC 100
	/* LABEL_MAP's ID_TO_ELEMENT_MAP starts at LABEL_ALLOC
	   and grows by LABEL_ALLOC as needed. */
							
uint32 hash_fatstr (void *str);
uint32 label_hashfn(void *label);
int label_comparefn(void *lab1, void *lab2);

LABEL_ID_MAPptr make_symbol_map(int alloc_size, char *name);
int init_label_table(void);
void free_label_table(void);

int FstBaseExport atomic_label_to_id(id_type *id, FAT_STR name);
int FstBaseExport symbol_to_id(id_type *id, FAT_STR name, LABEL_ID_MAPptr symbol_table);
int FstBaseExport make_pair_label(id_type *id_tuple, id_type upper_id, id_type lower_id);
int FstBaseExport make_fstlabel(id_type *tuple_id, id_type upper_id, id_type lower_id);
int FstBaseExport intern_symbol_name(id_type *id, FAT_STR name, LABEL_ID_MAPptr symbol_table);
int FstBaseExport intern_atomic_label(id_type *id, FAT_STR name);
int FstBaseExport intern_tuple_label(id_type *tuple_id, id_type *labels, int16 arity);
int FstBaseExport intern_literal(id_type *id, char *name);
int FstBaseExport utf8_intern_literal(id_type *id, char *name);
int FstBaseExport intern_unicode(id_type *id, char *name);
int FstBaseExport utf8_intern_unicode(id_type *id, char *name);
int FstBaseExport intern_unicode_symbol(id_type *id, FAT_STR name);
int FstBaseExport intern_symbol(id_type *id, char *name);
void FstBaseExport intern_char(id_type *new_id, char c);
void intern_int(id_type *id, int i);
int FstBaseExport intern_flag_value(id_type *id, FAT_STR name);
int FstBaseExport intern_flag_attribute(id_type *id, FAT_STR name);

#define thin_name_to_id(X, Y)	name_to_atom_id(X, FST_thin_to_temp_fat(Y))

#define id_to_label(X) (LABELptr) LIM_map(LABEL_MAP)[(X)]
#define global_alph_size()	LIM_id_count(LABEL_MAP)

#define tuple_id_p(X) (LABEL_arity(id_to_label(X)) > 1)

/* The macro below is only well defined for atomic labels (not tuple-labels) */

#define id_to_atom_name(X) (LABEL_atom(id_to_label((X))))
#define id_to_first_ch(X) (CHARNUM(id_to_atom_name(X)))


/* The macros below are well defined only for tuple labels */
#define id_to_upper_id(X) (TUPLE_labels(LABEL_tuple(id_to_label(X)))[UPPER])
#define id_to_lower_id(X) (TUPLE_labels(LABEL_tuple(id_to_label(X)))[LOWER])
#define id_to_upper_name(X) (id_to_atom_name(id_to_upper_id(X)))
#define id_to_lower_name(X) (id_to_atom_name(id_to_lower_id(X)))
#define id_to_first_uch(X) CHARNUM(id_to_upper_name(X))
#define id_to_first_lch(X) CHARNUM(id_to_lower_name(X))

extern id_type FstBaseExport EPSILON;
extern id_type FstBaseExport OTHER;
extern id_type FstBaseExport DUMMY_LABEL;
extern id_type FstBaseExport BOUNDARY_SYMBOL;
extern id_type FstBaseExport ALTCHAIN_SYMBOL;
extern id_type FstBaseExport SPACE_SYMBOL;
extern id_type FstBaseExport REPEAT_SYMBOL;
extern id_type FstBaseExport DET_EPSILON;
extern id_type FIRST_FREE_LABEL;

extern FAT_STR KEYWORD_PACKAGE;

#define REPEAT_SYMBOL_CHAR "*"
#define SPACE_SYMBOL_CHAR " "

void remove_escapes(char *str);
int FstBaseExport name_to_atom_id(id_type *id, FAT_STR name);

int FstBaseExport print_label(id_type id, FILE *stream, int escape_p);
int FstBaseExport fat_to_thin_name(char *buffer, FAT_STR name, int max, int escape_p);
int FstBaseExport utf8_fat_to_thin_name(char *buffer, FAT_STR name, int max, int escape_p);

int FstBaseExport copy_label_name(id_type id, int side, char *buffer, int max, int eps_to_zero_p, int escape_p);
int FstBaseExport utf8_copy_label_name(id_type id, int side, char *buffer, int max, int eps_to_zero_p, int escape_p);
void FstBaseExport print_global_labels(FILE *stream);

id_type FstBaseExport upper_id(id_type id);
id_type FstBaseExport lower_id(id_type id);

void FstBaseExport clear_label_data(void);
int FstBaseExport id_tag_p (id_type id);

int flag_diacritic_name_p(FAT_STR name, int error_p);
void print_flag_diacritic(FLAG_DIACRptr flag_diacr, FILE *stream);
void print_label_and_flag_diacritic(id_type id);

FAT_STR id_to_flag_attribute_name(id_type id);
FAT_STR id_to_flag_value_name(id_type id);
int flag_attribute_count(void);
int flag_value_count(void);

#define flag_diacritic_p(X)         (LABEL_flag(id_to_label(X)) != NULL)
#define flag_diacritic(X)           (LABEL_flag(id_to_label(X)))
#define flag_diacritic_action(X)    (LABEL_flag_action(id_to_label(X)))
#define flag_diacritic_attrib(X)    (LABEL_flag_attrib(id_to_label(X)))
#define flag_diacritic_value(X)     (LABEL_flag_value(id_to_label(X)))

enum escape_p {DONT_ESCAPE, DO_ESCAPE};

#ifdef __cplusplus
}
#endif

#endif
