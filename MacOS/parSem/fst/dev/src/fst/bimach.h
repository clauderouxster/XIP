/* $Id: bimach.h,v 1.4 2005/03/04 15:45:21 grondeau Exp $ */
/*************************************************************
 *
 *  FILE  "bimach.h"
 *
 *  Conversion of an unambiguou FST
 *  first into a bimachine (Schuetzenberger, 1961)
 *  and then into a left-sequential and a right-sequential FST,
 *  based on an enhanced version of Roche and Schabes'
 *  algorithm (1997).
 *
 *  March 99, Andre Kempe, MLTT, XRCE, Xerox Corporation
 *
 *************************************************************/


#ifndef BIMACH_H
#define BIMACH_H
/*-----------------------------------------------------------*/

/* ---- cell for list of state pointers (from heap) */
typedef struct STATE_LIST_TYPE *STATELISTptr;
typedef struct STATE_LIST_TYPE {
  STATEptr state;         /* state pointer */
  STATELISTptr next;      /* next cell in list */
}
STATELISTtype;

#define STATE_LIST_BLOCK_SIZE      1000    /* block size in state list heap */

/* ---- vector of state pointers */
typedef struct STATE_LIST_VECTOR_TYPE {
  ALPHABETptr id2idx;   /* conversion:  label ID --> index */
  long max;             /* maximal length == allocated vector length */
  long len;             /* vector length currently used */
  STATELISTptr *vec;    /* vector */
}
SLVECtype, *SLVECptr;

/* ---- vector of state pointers */
typedef struct STATE_VECTOR_TYPE {
  long max;             /* maximal length == allocated vector length */
  long len;             /* vector length currently used */
  STATEptr *vec;        /* vector */
}
STATEVECtype, *STATEVECptr;

#define fSc_INDIR_FACTO     "INDIR_FACTO"
#define fSc_split_arc(A)    (A)->userflag1

/*-----------------------------------------------------------*/

/* ---- cell for binary tree of deterministic states */
typedef struct STATE_BINTREE_TYPE *SBTREEptr;
typedef struct STATE_BINTREE_TYPE {
  STATEptr state;   /* state of deterministic network */
  SBTREEptr prev;   /* subtree of previous states */
  SBTREEptr next;   /* subtree of next states */
}
SBTREEtype;

#define STATE_BINTREE_BLOCK_SIZE      1000    /* block size in state bin.tree heap */

/*-----------------------------------------------------------*/

/* ---- cell for list of pair state pointers (from heap) */
typedef struct PAIR_STATE_LIST_TYPE *PRSTLISTptr;
typedef struct PAIR_STATE_LIST_TYPE {
  STATEptr state1;         /* 1st state pointer */
  STATEptr state2;         /* 2nd state pointer */
  PRSTLISTptr next;        /* next cell in list */
}
PRSTLISTtype;

#define PAIR_STATE_LIST_BLOCK_SIZE     500  /* block size in cell heap */

/*-----------------------------------------------------------*/

/* ---- cell for list of IDs (from heap) */
typedef struct ID_LIST_TYPE *IDLISTptr;
typedef struct ID_LIST_TYPE {
  id_type id;             /* ID */
  IDLISTptr next;       /* next cell in list */
}
IDLISTtype;

/* ---- vector item for arc chains */
typedef struct ARC_CHAIN_ITEM_TYPE {
  STATEptr source;            /* real source state of the arc chain */
  STATEptr dest;              /* real destination state of the arc chain */
  int len;                    /* length of the arc chain */
  IDLISTptr chain;            /* ID list of the arc chain */
}
ARCCHAINtype, *ARCCHAINptr;

/* ---- vector of arc chains */
typedef struct ARC_CHAIN_VECTOR_TYPE {
  long max;
  long len;
  ARCCHAINptr vec;
  HEAPptr id_cell_heap;
}
ARCCHVECtype, *ARCCHVECptr;

#define ARC_CHAIN_VECTOR_LENGTH        200
#define ARC_CHAIN_VECTOR_REALLOC       1.5
#define ARC_CHAIN_VECTOR_HEAP_BLOCK    500

/*-----------------------------------------------------------*/

/* ---- state number list */
typedef struct NUMBER_LIST_TYPE *NUMLISTptr;
typedef struct NUMBER_LIST_TYPE {
  int number;
  NUMLISTptr next;
}
NUMLISTtype;

#define NUMBER_LIST_HEAP_BLOCK       2000

typedef struct TWO_NUMBER_SETS_TYPE {
  NUMLISTptr A1_states;    /* list of all A1 states related to the FST state */
  NUMLISTptr A2_states;    /* list of all A2 states related to the FST state */
}
NUM2SETtype, *NUM2SETptr;

#define TWO_NUMBER_SETS_HEAP_BLOCK    500

/*-----------------------------------------------------------*/

/* ---- item in dense transition matrix */
typedef struct DELTA_DENSE_ITEM_TYPE {
  long A1_num;        /* number of corresponding state in A1 */
  long A2_num;        /* number of corresponding state in A2 */
  id_type pair_label;
}
DLDENSEtype, *DLDENSEptr;

/* ---- delta matrix of bimachine */
typedef struct DELTA_MATRIX_TYPE {
  struct {
    NETptr fst;          /* original FST */
    NETptr A1;           /* 1st automaton of the bimachine */
    NETptr A2;           /* 2nd automaton of the bimachine */
    NETptr T1;           /* 1st FST derived from the bimachine */
    NETptr T2;           /* 2nd FST derived from the bimachine */
  }
  net;
  struct {
    STATEVECptr A1;   /* vector of pointers to all states of A1 */
    STATEVECptr A2;   /* vector of pointers to all states of A2 */
    STATEVECptr T1;   /* vector of pointers to all states of T1 */
    STATEVECptr T2;   /* vector of pointers to all states of T1 */
  }
  state_vector;
  struct {
    long max;
    long len;
    DLDENSEptr items;
    int sorted_p;   /* flag: all items sorted [A2][A1] */
  }
  dense_matrix;
  uint16 *label_index;  /* vector[A1]: label indices occur.with input_label */
  struct {
    HEAPptr number_list;
    HEAPptr num2_set;
  }
  heap;
}
DELTAMATRIXtype, *DELTAMATRIXptr;

#define BM_DELTA_DENSE_REALLOC_FACTOR    1.6
#define BM_DIACRITIC_PATTERN            ".%s.%hu_"
#define INDIR_FACTORIZE_DIACR_PATTERN   "_INDIR_FACTORIZE.%hu_"

/*-----------------------------------------------------------*/

#define BIMACH_EPSILON_RECURSE_LIMIT    1000

/*-----------------------------------------------------------*/

/* ---- cell for state info in factorization of fin.amb. FSTs */
typedef struct FCT2_STATE_INFO_TYPE {
  long state_num;         /* number of corresponding state in input FST */
  STATEptr det_right;     /* hyper state in right-det. input automaton */
  STATEptr fctfst_state;
  STATEptr ambfst_state;
}
FCT2_STATEINFOtype, *FCT2_STATEINFOptr;

#define FCT2_STATE_INFO_BLOCK_SIZE  500    /* block size in state info heap */

/* ---- cell for arc info in factorization of fin.amb. FSTs */
typedef struct FCT2_ARC_TYPE {
  STATEptr source;      /* source state of the arc */
  STATEptr dest;        /* destination state of the arc */
  long source_num;      /* source state number wrt. input FST */
  long dest_num;        /* destination state number wrt. input FST */
  STATEptr hyper_dest;  /* hyper destination state of the arc */
  id_type pair_id;
  id_type in_id;
  id_type out_id;
}
FCT2_ARCtype, *FCT2_ARCptr;

typedef struct FCT2_ARC_VECTOR_TYPE {
  long max;
  long len;
  FCT2_ARCptr vector;        /* full vector of arcs */
  struct {
    long len;     /* length of current partition */
    FCT2_ARCptr vector;  /* current partition */
    struct {
      short amb;    /* current partition is ambiguous */
      short beg;    /* ambiguity begins with current partition */
      short end;    /* ambiguity ends with current partition */
      short fork;   /* fork in current partition */
      short other;  /* OTHER or OTHER:OTHER in current partition */
    }
    flags;
    struct {
      long beg;     /* index of ambiguity area, at begin */
      long mid;     /* index inside an ambiguity area */
    }
    index;
  }
  sub;                      /* partition of arcs */
}
FCT2_ARCVECtype, *FCT2_ARCVECptr;

#define FCT2_ARC_VECTOR_INIT_LENGTH    200
#define FCT2_ARC_VECTOR_REALLOC        1.5

#define FCT2_AMB_BEGIN    "_ABEG_%d_"
#define FCT2_AMB_MIDDLE   "_AMID_%d_"

#define FCT2_TOTAL_DOTS            50
#define FCT2_MIN_STATES_PER_DOT    10

/*-----------------------------------------------------------*/

typedef struct ARC_PATH_TYPE {
  long max;
  long len;
  ARCptr *arcs;
}
ARC_PATHtype, *ARC_PATHptr;

#define FCT2_ARC_PATH_INIT_LENGTH        200
#define FCT2_ARC_PATH_REALLOC            1.5

/*--------------------------------*/
typedef struct ARC_LOOP_TYPE *ARC_LOOPptr;
typedef struct ARC_LOOP_TYPE {
  long len;            /* length of the loop */
  ARCptr *arcs;        /* arcs of the loop */
  STATEptr state;      /* first state of the loop */
  ARC_LOOPptr next;    /* next loop that begins at the same state */
}
ARC_LOOPtype;

typedef struct STATE_LOOP_TYPE {
  STATEptr state;      /* first state of a loop */
  STATEptr auxstate;   /* auxiliary state for redirecting incomming arcs */
  ARC_LOOPptr loops;   /* first loop of a state (= first list item) */
  id_type diacritic;   /* diacritic representing all loops of the state */
}
STATE_LOOPtype, *STATE_LOOPptr;

typedef struct LOOP_VECTOR_TYPE {
  long num_loops;             /* total number of loops (= length of loop vector) */
  ARC_LOOPptr loops;          /* loops (vector of ...) */
  ARC_PATHptr path_buffer;    /* buffer of loop paths */
  long num_lpstates;    /* number of states with loops (= length of state vector)*/
  STATE_LOOPptr lpstates;     /* states with loops (vector of ...) */
}
LOOP_VECTORtype, *LOOP_VECTORptr;

#define FCT2_EPS_DIACRITIC       "_INFAMB_%d_"
#define FCT2_TMPEPS_DIACRITIC    "_TMPEPS_%d_"

/*-----------------------------------------------------------*/
typedef struct SYMBOL_TRANSDUCTION_TYPE {
  id_type in_symbol;          /* input symbol, e.g., a diacritic */
  ALPHABETptr out_symbols;    /* alternative output symbols */
}
SYMTRANStype, *SYMTRANSptr;

typedef struct ALPHABET_TRANSDUCTION_TYPE {
  int max;
  int len;
  SYMTRANSptr symbols;
}
ALPHTRANStype, *ALPHTRANSptr;

#define FCT2_SHORT_AMB_DIACRITIC         "_SHORTAMB_%d_"
#define FCT2_TRANSDUCTION_INIT_LENGTH        200
#define FCT2_TRANSDUCTION_REALLOC            1.5

/*-----------------------------------------------------------*/

void FstCalcExport set_bimachine_flags (NETptr left, NETptr right, int side_p);
void FstCalcExport sort_bimachine_fst_arcs (NETptr net, int side_p, int eps_1st_p);

NETptr FstCalcExport align_ambiguity (NETptr fst, int side_p, int copy_p, int compact_p);
NETptr FstCalcExport unalign_ambiguity (NETptr fst, int copy_p, int compact_p);

NVptr FstCalcExport factorize_bm_fsts
  (NETptr fst, int side_p, int copy_p, int compact_p, int alphreduce_p,
   int verbose_p);
NETptr FstCalcExport unfactorize_bm_fsts
  (NETptr left, NETptr right, int side_p, int copy_p, int verbose_p);

NVptr FstCalcExport factorize_shortamb_fst
  (NETptr fst, int side_p, int copy_p, int compact_p, int verbose_p);

int FstCalcExport check_fail_safety (NETptr fst, int side_p);

NVptr FstCalcExport factorize_finamb_fst
  (NETptr fst, int side_p, int copy_p, int compact_p, int alphreduce_p,
   int verbose_p);

NVptr FstCalcExport factorize_infamb_fst
  (NETptr fst, int side_p, int copy_p, int compact_p, int alphreduce_p,
   int verbose_p);

NETptr FstCalcExport unfactorize_amb_fsts
  (NETptr fa1, NETptr fa2, int side_p, int copy_p, int verbose_p);
NETptr FstCalcExport unfactorize_cascade
  (NETptr fa1, NETptr fa2, int side_p, int copy_p, int verbose_p);

NVptr FstCalcExport reduce_intermediate_alphabet_in_pair
  (NVptr nv, ALPHABETptr midsyms, int side_p, int copy_p, int compact_p,
   int verbose_p);
NVptr FstCalcExport reduce_intermediate_alphabet_in_nv
  (NVptr nv, ALPHABETptr midsyms, int side_p, int copy_p, int compact_p,
   int verbose_p);

NVptr FstCalcExport factorize_with_S_covering
  (NETptr fst, int side_p, int copy_p, int compact_p, int alphreduce_p,
   int verbose_p);

NVptr FstCalcExport factorize_arbitrary_fst
  (NETptr fst, int side_p, int copy_p, int compact_p, int alphreduce_p,
   int verbose_p);
NETptr FstCalcExport unfactorize_arbitrary_fst
  (NVptr nv, int side_p, int copy_p, int compact_p, int verbose_p);


#endif /* BIMACH_H */

