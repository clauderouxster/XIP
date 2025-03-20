/* $Id: struct.h,v 1.40 2003/02/06 02:11:06 lk Exp $ */
/* Copyright (c) 1990-94 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  struct.h
 **
 **	 Todd Yampol, Xerox PARC, January 1990
 **
 **  function prototypes for construction fsm datatypes
 **  (network, state, arc, alphabet)
 **
 ******************************************************/

#ifndef C_FSM_STRUCT

#define C_FSM_STRUCT

#include "fst_windefs.h"
#include "types.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

/* #define MAX_LV 16                             now defined in label.h  */
/* #define MAX_LABEL ((unsigned) (1<<15) - 1)    now defined in label.h  */
#define ALPH_HEAP LV_HEAPS[0]
/* in binary_sigma_member, X is an ALPHABET, Y is an integer (label id) */
#define binary_sigma_member(X,Y) (ALPH_len((X)) > (Y) && (ALPH_item((X), (Y)) == 1))

/****************
 *
 * XFST LIMITS
 *
 ***************/
  /* When the code is compiled with -DWORLD flag, the user can build
     networks of any size and save them in binary format. However,
     only nets that are smaller than the limits given here can be
     written out as text files. These limits affect the following
     xfst commands: 'write text', 'write spaced-text', 'write prolog',
     'print words', 'print upper-words', 'print lower-words',
     'print random-upper', 'print random-lower', print nth-lower',
     and 'print nth-upper'.
  */

#define WORLD_STATE_LIMIT 500
#define WORLD_PATH_LIMIT 500


/********************
 *
 * GLOBAL VARIABLES
 *
 ********************/

/* NET LOADING FEATURES:
   --------------------- */
enum RestoreFunctionType {TypeRestoreCompact,/* index of the compact net loading function */
                          TypeRestoreVirtual,/* index of the virtual net loading function */
                          TypeRestoreStandard,/*index of the standard net loading function*/
                          TypeRestoreCompress,/*index of the compressed net loading function*/
                          LastRestoreFunctionType/* last index for next loading functions */
};

/* Definition of the type "pointer to a network loading function": */
    typedef NETptr (*restore_func)(char * /* filename */,
                                   byte* /* cur_byte */,
                                   long int /* num_states */,
                                   long int /* num_arcs */,
                                   NETptr /* net */,
                                   FILE * /* stream */,
                                   unsigned long * /* byte_countp */,
                                   int /* file_type */);

    
/* Global array of pointers to all the net loading functions in use;
   call function init_restore_functions() in your main program for filling this
   table with the approriate functions. */
extern restore_func FsmFstExport restore_functions[LastRestoreFunctionType];


/* NET SAVING FEATURES:
   -------------------- */
enum SaveFunctionType {TypeSaveCompact,/* index of the compact net saving function */
                       TypeSaveVirtual,/* index of the virtual net saving function */
                       TypeSaveStandard,/*index of the standard net saving function*/
                       TypeSaveCompress,/*index of the compressed net saving function*/
                       LastSaveFunctionType/* last index for next saving functions */};
    
/* Definition of the type "pointer to a network saving function": */
    typedef int (*save_func)(FILE * /* stream */,
                             unsigned long * /* byte_countp */,
                             NETptr /* net */,
                             unsigned long * /* arc_count */,
                             unsigned long * /* state_count */,
                             unsigned long * /* file_lengthp */,
                             int /* old_format_p */);

/* Global array of pointers to all the net saving functions in use;
   call function init_save_functions() in your main program for filling 
   this table with the appropriate functions. */
extern save_func FsmFstExport save_functions[LastSaveFunctionType];


/* EXTERNAL LIBRARY DEPENDENCIES:
   ------------------------------ */
    
/* None in WORLD mode. */


/* MEMORY ALLOCATION FEATURES:
   --------------------------- */
extern HEAPptr FsmFstExport STATE_HEAP;
extern HEAPptr FsmFstExport ARC_HEAP;
extern HEAPptr ARC_VECTOR_HEAP;
extern HEAPptr FsmFstExport BIG_ARC_HEAP;
extern HEAPptr FsmFstExport NET_HEAP;
extern HEAPptr FsmFstExport LV_HEAPS[MAX_LV+1];
extern HEAPptr FsmFstExport CH_VECTOR_HEAP, CH_NODE_HEAP, PARSE_TBL_HEAP;

void FsmFstExport register_common_fs(FAT_STR fs);
void FsmFstExport init_fsm_memory(FST_CNTXTptr fst_cntxt);
void FsmFstExport print_storage_info(FILE *stream);
void print_net_name(NETptr net, FILE *stream);
void FsmFstExport release_fsm_memory(void);
NETptr FsmFstExport make_network (void);
void name_network(NETptr network, char *name);

NETptr FsmFstExport null_fsm(void);
NETptr FsmFstExport epsilon_fsm (void);

ARCptr arc_alloc(id_type label, STATEptr dest);
ARCptr FsmFstExport make_arc (id_type label, STATEptr dest, NETptr net);
void FsmFstExport push_arc(ARCptr arc, STATEptr state);
void FsmFstExport add_arc (ARCptr arc, STATEptr state, NETptr net);
#ifdef FULL_FSM
void FsmFstExport add_new_arc (NETptr net, STATEptr start, id_type l,
					STATEptr dest, void *user_pointer);
#else
void FsmFstExport add_new_arc (NETptr net, STATEptr start, id_type l, STATEptr dest);
#endif

int FsmFstExport num_arcs(STATEptr state);

id_type get_field (LABELptr label, int field);

STATEptr alloc_state(void);
STATEptr FsmFstExport make_state (ARCptr arcs, NETptr net);

STATEptr delete_state_from_list (STATEptr s, STATEptr states);
STATEptr delete_states(NETptr net, int keep_pred(STATEptr state));

BYTE_BLOCKptr FsmFstExport make_byte_block(unsigned long len);

/* 
These macros to test whether the argument is an arc or a state.
They depend upon the TYPE_BIT being in the first position of
each record.  This may not be portable.
*/
#define STATE_P(X) ((X) && (int) ARC_type_bit((ARCptr) (X)))
#define ARC_P(X)   ((X) && (int) !ARC_type_bit((ARCptr) (X)))

ALPHABETptr FsmFstExport reverse_map(ALPHABETptr map);
void renumber_arc_labels(NETptr net, ALPHABETptr key);

id_type *make_alph_items(int len, int *max);
ALPHABETptr FsmFstExport make_alph(int len, int type);
ALPHABETptr FsmFstExport copy_alphabet(ALPHABETptr old_alph);
ALPHABETptr FsmFstExport labels_to_alph(id_type *symbols, int len, int type);
ALPHABETptr FsmFstExport adjust_alph_size(ALPHABETptr alph, int new_len);
ALPHABETptr FsmFstExport binary_to_label (ALPHABETptr a);
ALPHABETptr FsmFstExport label_to_binary (ALPHABETptr a);
ALPHABETptr expand_sigma(ALPHABETptr sig, int newsize);
ALPHABETptr FsmFstExport sigma_add_to (ALPHABETptr sig, id_type newsym);
ALPHABETptr FsmFstExport sigma_remove_from (ALPHABETptr sig, id_type sym);
ALPHABETptr FsmFstExport label_remove_from(ALPHABETptr labels, id_type lab);
void FsmFstExport label_to_sigma(ALPHABETptr sigma, id_type id);
ALPHABETptr FsmFstExport label_sigma(NETptr net);
ALPHABETptr FsmFstExport sigma(NETptr net);
ALPHABETptr FsmFstExport update_label_alphabet(NETptr net);
ALPHABETptr side_sigma (NETptr net, int side);
ALPHABETptr upper_sigma(NETptr net);
ALPHABETptr FsmFstExport lower_sigma(NETptr net);
int FsmFstExport sigma_member(ALPHABETptr a, id_type item);
void FsmFstExport sigma_augment(ALPHABETptr sigma, ALPHABETptr alph);
void FsmFstExport label_augment(ALPHABETptr labels, ALPHABETptr alph);

ALPHABETptr excluded_symbols (NETptr net);
void update_net_labels_and_sigma (NETptr net);
void add_binary_alph(id_type *target, id_type *source, int len);
void FsmFstExport add_label_alph(id_type *target, id_type *source, int len);
void subtract_binary_alph(id_type *target, id_type *source, int len);
void subtract_label_alph(id_type *target, id_type *source, int len);
id_type max_binary_item(id_type *items, int len);
id_type FsmFstExport max_label_item(id_type *items, int len);
id_type FsmFstExport max_label_component(ALPHABETptr alph, int side);
int other_in_alph_p(ALPHABETptr alph);
ALPHABETptr label_components(ALPHABETptr alph);
ALPHABETptr FsmFstExport sigma_union(ALPHABETptr s1, ALPHABETptr s2);
ALPHABETptr FsmFstExport sigma_union_dont_copy(ALPHABETptr s1, ALPHABETptr s2);
ALPHABETptr FsmFstExport sigma_difference(ALPHABETptr s1, ALPHABETptr s2);
ALPHABETptr sigma_intersect(ALPHABETptr s1, ALPHABETptr s2);

int alph_order(register id_type id1, register id_type id2);
int FsmFstExport default_alph_order(const void *id1, const void *id2);
int default_label_order(id_type id1, id_type id2);
void FsmFstExport sort_alph(ALPHABETptr alph);

void set_net_arity(NETptr net); /* displaced from modify.h 2000-4-21 TG */

ALPHABETptr make_identity_map(NETptr net);

int common_fat_string_p(FAT_STR name);
int symbol_name_p(FAT_STR attr_name);

#define free_state(X) free_cell((void *) (X), STATE_HEAP)

#ifdef SMALL_FSM
#define free_arc(X) free_cell ((void *) (X), ARC_HEAP)
#else
#define free_arc(X) \
 if((X)->big_arc_flag == 0) free_cell((void *) (X), ARC_HEAP); \
 else free_cell((void*) (X), BIG_ARC_HEAP)
#endif /* SMALL_FSM */

void free_shared_arc_lists (NETptr net);
void free_arc_set(STATEptr state, NETptr net);
void FsmFstExport free_state_and_arcs(STATEptr state, NETptr net);
void free_arc_vector(AVptr av, NETptr net);
void FsmFstExport free_parse_table(PARSE_TBL ch);
void free_net_parse_tables(NETptr net);
void free_range_map(RANGEptr range_map, int len);
void free_range_vector(RANGE_VECTORptr range_vector);
void FsmFstExport free_match_table(MATCH_TABLEptr range_table);
void free_net_and_periphery(NETptr net);
void free_virtual_network_storage(NETptr net);

extern void (*free_included_table)(NETptr net);
void FsmFstExport free_network(NETptr net);

void FsmFstExport free_prop(PROPptr prop);
void FsmFstExport free_props(NETptr net);
void free_object(OBJECTptr obj);
void free_alph_items(ALPHABETptr a);
void FsmFstExport free_alph(ALPHABETptr a);
void free_sequence(SEQUENCEptr seq);
void free_byte_block(BYTE_BLOCKptr byte_block);
void free_tabular_table (NETptr net);

ALPH_VECTORptr FsmFstExport make_alph_vector(int len);
void FsmFstExport free_alph_vector(ALPH_VECTORptr av);

NVptr FsmFstExport make_nv(int len);
void FsmFstExport expand_nv(NVptr nv, int num);
NVptr net2nv(NETptr net);
void nv_push(NETptr net, NVptr nv);
void FsmFstExport nv_add(NETptr net, NVptr nv);
NVptr nv_insert (NETptr net, NVptr nets, int pos);
NETptr FsmFstExport nv_get(NVptr nets, int pos);
NETptr FsmFstExport nv_extract(NVptr nets, int pos);
NVptr FsmFstExport nv_append(NVptr nv1, NVptr nv2);
NVptr FsmFstExport nv_reverse(NVptr nv);
NVptr copy_nv_only(NVptr old);
/* NVptr copy_nv_and_nets(NVptr old); <-- Defined in fsm-basics.c */
void FsmFstExport free_nv_only(NVptr nv);
void FsmFstExport free_nets_only(NVptr nv);
void FsmFstExport free_nv_and_nets(NVptr nv);

extern FAT_STR FsmFstExport AUTHOR_ATTR,
               CHARENCODING_ATTR,
               CHARNAMES_ATTR,
               COMPACTED_ATTR,
               COMPOUNDCHAR_ATTR,
               COPYRIGHT_ATTR,
               DATE_ATTR,
               DIACRITICS_ATTR,
               EQV_VECTOR_ATTR,
               FUNCTION_ATTR,
               LANGUAGE_ATTR,
               LITERALNAMES_ATTR;
extern FAT_STR FsmFstExport NETWORKNAME_ATTR;
extern FAT_STR FsmFstExport NEUTRALLINKINGCHAR_ATTR,
               OPERATION_ATTR,
               POSTFIXLINKINGCHAR_ATTR,
               PREFIXLINKINGCHAR_ATTR,
               SAVED_SIGMA_ATTR,
               SIGMA_ATTR,
               SOURCE_ATTR,
               STATUS_ATTR,
               TAGS_ATTR,
               TOOLVERSIONS_ATTR,
               TRANSCRIPT_ATTR,
               TYPE_ATTR,
               VECTORIZE_N_ATTR;
extern FAT_STR FsmFstExport VERSIONSTRING_ATTR;
extern FAT_STR WORD_TO_FILEPOS_MAP_ATTR; /* 28  pointers to ..._ATTR */

extern FAT_STR FsmFstExport EPSILON_NAME,
               OTHER_NAME;

extern FAT_STR IL_PACKAGE,
               LISP_PACKAGE;

extern FAT_STR FAT_OTHER,
               NIL_NAME,
               T_NAME;

extern int NOT_INITIALIZED_P;
extern int FsmFstExport FSM_MODE;
extern char *COPYRIGHT_OWNER;


/* NET-VECTOR VECTORS */

NVVptr FsmFstExport make_nvv (int len);
NVVptr append_nvv (NVVptr nvv1, NVVptr nvv2);
NVVptr expand_nvv (NVVptr nvv, int num);
NVVptr nv_to_nvv (NVptr nv);
void free_nvv_only (NVVptr nvv);
void FsmFstExport free_nvv_and_nvs_only (NVVptr nvv);
void free_nvv_and_nvs_and_nets (NVVptr nvv);

/* REPLACEMENT STRUCTURES */

RSptr make_rs (int length, int arrow, int ctxdir);
void free_rs (RSptr rs);
RSptr empty_rs();

int rs_compair_and_copy_int (int *n1, int *n2);
void rs_handle_int_comparison_error
            (char *operation, char *xerror, int n1, int n2);
RSptr rs_append (RSptr rs1, RSptr rs2);

/* Property lists */

void FsmFstExport add_prop(NETptr net, FAT_STR attr, OBJECTptr value);
void add_integer_prop(NETptr net, FAT_STR attr, int value);
void FsmFstExport add_character_prop(NETptr net, FAT_STR attr, char value);
void FsmFstExport add_string_prop(NETptr net, FAT_STR attr,  FAT_STR value);
void add_symbol_prop(NETptr net, FAT_STR attr, IO_SYMBOLptr value);
void FsmFstExport add_array_prop(NETptr net, FAT_STR attr, SEQUENCEptr value);
void add_list_prop(NETptr net, FAT_STR attr, SEQUENCEptr value);
void FsmFstExport add_byte_block_prop(NETptr net, FAT_STR attr, BYTE_BLOCKptr value);

OBJECTptr FsmFstExport get_value(NETptr net, FAT_STR attr);
PROPptr FsmFstExport remove_prop(NETptr net, FAT_STR attr);

OBJECTptr make_object(int type);
SEQUENCEptr make_sequence(int len, int type);

int mark_toolversion(NETptr net, char *tool, char *version);
int nv_mark_version(NVptr nv, char *tool, char *version);

/* Only UTF-8 and ISO-8859-1 are currently supported */

int FsmFstExport init_string_functions(int char_encoding);
char *decode_char_encoding(int char_encoding);

/* Hacking around qsort problem on Sun Sparcstations. */

#ifdef sun
#ifdef sparc
#define QSORT qsort_emacs_19_31
#include "qsort-emacs.h"
#else
#define QSORT qsort
#endif
#else
#define QSORT qsort
#endif

#ifdef __cplusplus
}
#endif
#endif
/* DON'T ADD ANYTHING AFTER THIS #endif */


