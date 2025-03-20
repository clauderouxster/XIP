#ifndef XLE_INTERSECT_H_INCLUDED
#define XLE_INTERSECT_H_INCLUDED

/* XLE needs to know the size of these objects, because it allocates
 * an array of them for intersection.
 */ 

typedef struct CONTENTS {
  STATEptr state;
  void *user_ptr;
  struct CONTENTS *next;
} CONTENTStype, *CONTENTSptr;
 
typedef struct DEST2_CELL {
  byte type;     /* 0 = empty, 1 = single state, 2 = list of states */
  byte mark;     /* 0 = unmarked, 1 = visited */
  CONTENTSptr content_ptr;
} DEST2type, *DEST2ptr;

NETptr new_intersect_fsm(NVptr nets, int reclaim_p, int minimize_p,
                       DEST2ptr (*make_dest_array)(NETptr, NETptr),
                       DEST2ptr (*index_dest_array)(ARCptr, DEST2ptr, void **),
                       id_type (*get_new_label)(ARCptr, void *));
 
#endif /* XLE_INTERSECT_H_INCLUDED */
