/* $Id: vector.h,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/* Copyright (c) 1999 by the Xerox Corporation.  All rights reserved */

/******************************************************
 **
 **  VECTOR.H
 **
 **  Lauri Karttunen, XRCE, February 1999
 **
 **  Function prototypes and data structures for new lookup routines
 **
 ******************************************************/

#ifndef C_FSM_VECTOR

#define C_FSM_VECTOR

#ifdef __cplusplus
extern "C" {
#endif

/******************
 *  Vector object  *
 ******************/
typedef struct VECTOR {
  int length;
  int pos;
  void **array;
} VECTORtype, *VECTORptr;

#define VECTOR_length(X)	(X)->length
#define VECTOR_pos(X)		(X)->pos
#define VECTOR_array(X)		(X)->array

typedef struct VECTOR_ENUMERATOR {
  int pos;
  VECTORptr vector;
} VECT_ENUMtype, *VECT_ENUMptr;

#define VECT_ENUM_pos(X)	 (X)->pos
#define VECT_ENUM_vector(X)	 (X)->vector


typedef struct VECTOR_TABLE {
  int length;
  int vector_length;
  int pos;
  VECTORptr *array;
} VECTOR_TABLEtype, *VECTOR_TABLEptr;

#define VECTOR_TABLE_length(X)	(X)->length
#define VECTOR_TABLE_vector_length(X)	(X)->vector_length
#define VECTOR_TABLE_pos(X)	(X)->pos
#define VECTOR_TABLE_array(X)	(X)->array

VECTORptr make_vector(int length);
void free_vector(VECTORptr vector1);
void initialize_vector(VECTORptr *vector1, int min_size);
void reset_vector(VECTORptr input_vector);

int vector_size(VECTORptr vector1);
void assure_vector_space(int len, VECTORptr vector);
int append_to_vector(void *object, VECTORptr vector1);
int vector_element_at(void **element, int pos, VECTORptr vector1);
int set_vector_element_at(void *element, int pos, VECTORptr input_vector);
void increment_vector(VECTORptr vector1);
int decrement_vector(VECTORptr vector1);
int subtract_from_vector(void **last_element, VECTORptr vector1);
int concatenate_vectors(VECTORptr vector1, VECTORptr vector2);

VECT_ENUMptr create_vector_enumerator(VECTORptr lab_vect);
void reset_vector_enumerator(VECT_ENUMptr vect_enum);
void free_vector_enumerator(VECT_ENUMptr vect_enum);
int next_vector_element(void **element, VECT_ENUMptr vect_enum);

VECTOR_TABLEptr make_vector_table(int table_length, int vector_length);
void assure_vector_table_space(int len, VECTOR_TABLEptr table);
int vector_at(VECTORptr *vect, int pos, VECTOR_TABLEptr table);
void reclaim_vector_table(VECTOR_TABLEptr table);
void reset_vector_table(VECTOR_TABLEptr table);

#ifdef __cplusplus
}
#endif

#endif /* C_FSM_VECTOR */

