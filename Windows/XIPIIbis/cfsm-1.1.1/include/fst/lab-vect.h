/* $Id: lab-vect.h,v 1.18 2002/12/17 16:55:02 grayrat Exp $ */
/******************************************************
 **
 **  LAB-VECT.H
 **
 **  
 **  Lauri Karttunen, RXRC, December 1993
 **  Added LAB_VECTOR structure. February 1999.
 **
 ******************************************************/
 
#ifndef C_FSM_LAB_VECT
 
#define C_FSM_LAB_VECT
 
#include "syst-dep.h"
#include "id_type.h"


#ifdef __cplusplus
extern "C" {
#endif


 void FsmFstExport init_lab_vector(void);
 void free_lab_vector(void);
 id_type lab_vect_len(void);
 id_type FsmFstExport *lab_vect_labs(void);
 void FsmFstExport add_label(id_type i);

typedef struct LAB_VECTOR {
  int length;
  int pos;
  id_type *array;
} LAB_VECTORtype, *LAB_VECTORptr;

extern id_type FsmFstExport *LAB_VECTOR; /* The name is misleading */
extern int FsmFstExport LAB_COUNT_MAX;

#define LAB_VECTOR_length(X)	(X)->length
#define LAB_VECTOR_pos(X)	(X)->pos
#define LAB_VECTOR_array(X)	(X)->array

typedef struct LAB_ENUMERATOR {
  int pos;
  LAB_VECTORptr lab_vector;
} LAB_ENUMtype, *LAB_ENUMptr;

#define LAB_ENUM_pos(X)	        (X)->pos
#define LAB_ENUM_vector(X)	(X)->lab_vector

typedef struct LAB_RING {
  int start_pos;
  int min_free;
  LAB_VECTORptr lab_vector;
} LAB_RINGtype, *LAB_RINGptr;

#define LAB_RING_min_free(X)	(X)->min_free
#define LAB_RING_start_pos(X)	(X)->start_pos
#define LAB_RING_lab_vector(X) 	(X)->lab_vector
#define LAB_RING_cur_pos(X) 	(X)->lab_vector->pos
#define LAB_RING_size(X) 	(X)->lab_vector->length

LAB_RINGptr make_lab_ring(int size, int min_free);

void reclaim_lab_ring(LAB_RINGptr lab_ring);
void reset_lab_ring(LAB_RINGptr lab_ring);
void turn_lab_ring(LAB_RINGptr lab_ring, int dist);
void append_to_lab_ring(id_type id, LAB_RINGptr lab_ring);
int lab_ring_element_at(id_type *lab, int pos, LAB_RINGptr lab_ring);
int count_symbols_in_ring(LAB_RINGptr lab_ring);

typedef struct LAB_RING_CURSOR {
  LAB_RINGptr ring;
} LAB_RING_CURSORtype, LAB_RING_CURSORptr;

#define LAB_RING_CURSOR_cur_label	(X)->lab_vector->array[(X)->lab_vector->pos]

LAB_RING_CURSORptr make_lab_ring_cursor(LAB_RINGptr lab_ring);
void reclaim_lab_ring_cursor(LAB_RING_CURSORptr lab_ring_cursor);
void init_lab_ring_cursor(LAB_RING_CURSORptr lab_ring_cursor);
id_type lab_ring_cursor_forward(LAB_RING_CURSORptr lab_ring_cursor);
id_type lab_ring_cursor_backward(LAB_RING_CURSORptr lab_ring_cursor);

typedef struct LAB_VECTOR_TABLE {
  int length;
  int vector_length;
  int pos;
  LAB_VECTORptr *array;
} LAB_VECTOR_TABLEtype, *LAB_VECTOR_TABLEptr;

#define LAB_VECTOR_TABLE_length(X)	(X)->length
#define LAB_VECTOR_TABLE_vector_length(X)	(X)->vector_length
#define LAB_VECTOR_TABLE_pos(X)	(X)->pos
#define LAB_VECTOR_TABLE_array(X)	(X)->array

LAB_VECTORptr make_lab_vector(int length);
void reclaim_lab_vector(LAB_VECTORptr lab_vect);
void assure_lab_vector_space(int len, LAB_VECTORptr lab_vect);
int FsmFstExport append_to_lab_vector(id_type lab, LAB_VECTORptr lab_vect);
int lab_vector_element_at(id_type *lab, int pos, LAB_VECTORptr lab_vect);
int set_lab_vector_element_at(id_type lab, int pos, LAB_VECTORptr lab_vect);
void increment_lab_vector(LAB_VECTORptr lab_vect);
int decrement_lab_vector(LAB_VECTORptr lab_vect);
int subtract_from_lab_vector(id_type *lab, LAB_VECTORptr lab_vect);
void copy_lab_vector(LAB_VECTORptr to, LAB_VECTORptr from);
int concatenate_lab_vectors(LAB_VECTORptr vect1, LAB_VECTORptr vect2);
int lab_vector_contains(id_type id, LAB_VECTORptr lab_vector);
int lab_vector_size(LAB_VECTORptr lab_vect);
void print_lab_vector(LAB_VECTORptr lab_vect, FILE *stream, int escape_p);
void reset_lab_vector(LAB_VECTORptr lab_vect);
void reverse_lab_vector(LAB_VECTORptr lab_vect);

LAB_ENUMptr create_lab_enumerator(LAB_VECTORptr lab_vect);
void reset_lab_enumerator(LAB_ENUMptr lab_enum);
void free_lab_enumerator(LAB_ENUMptr lab_enum);
int next_lab_vector_element(id_type *id, LAB_ENUMptr lab_enum);

LAB_VECTOR_TABLEptr make_lab_vector_table(int table_length, int vector_length);
void assure_lab_vector_table_space(int len, LAB_VECTOR_TABLEptr table);
int lab_vector_at(LAB_VECTORptr *lab_vect, int pos, LAB_VECTOR_TABLEptr table);
void reclaim_lab_vector_table(LAB_VECTOR_TABLEptr table);
void print_lab_vector_table(LAB_VECTOR_TABLEptr table, FILE *stream, int escape_p);
void reset_lab_vector_table(LAB_VECTOR_TABLEptr table);

#ifdef __cplusplus
}
#endif

#endif  /* C_FSM_LAB_VECT */
