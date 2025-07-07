/* $Id: subseq.h,v 1.4 2005/03/04 15:45:26 grondeau Exp $ */
/*************************************************************
 *
 *  FILE  "subseq.c"
 *
 *      P-subsequentialize (in the Mohri sense) an fst
 *
 *  Tamas Gaal, RANK XEROX, Grenoble, France
 *  07.../96
 *
 *************************************************************/
 
#ifndef C_FSM_SUBSEQ

#define C_FSM_SUBSEQ

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================
Implementation of Mehryar Mohri's (sub)sequentialization algorithm
as described in JNLE (to appear) (revised July 1996)
================================================================*/

/* --- OUT_CELL for ids (symbols) ----------------------------------- */

typedef struct OUT_CELL {
  id_type outid;
  uint16 refcount;
  struct OUT_CELL  *link;
} OUT_CELLtype, *OUT_CELLptr;

#define OC_id(X)           (X)->outid
#define OC_rc(X)           (X)->refcount
#define OC_l(X)            (X)->link
#define OC_next(X)         (X)->link

#define SQ_MAX_REFCOUNT         ((unsigned) (1<<16) - 1)

/* A state in Q2 is a set of pairs of (st1, id) where st1 is a state
   from T1 and id is a string. The set is represented as a list of
   such pairs. The "real", eventual fst states in T2 will be ordinary
   states (in the fst sense) but - in this *** initial implementation
   *** - certain final states in fst will also have a "final strings"
   list that will represent p-subsequentiability.
   */

typedef struct Q2_LIST {
  STATEptr st; /* from T1 */
  OUT_CELLptr ids;/* chained l -> r */
  int whash;
  /*   OUT_CELLptr lastid; */ /* most recent id: l-r last */
  struct Q2_LIST *next;
} Q2_LISTtype, *Q2_LISTptr;

#define QL_st(X)            (X)->st
#define QL_w(X)             (X)->ids
#define QL_w_head(X)        (X)->ids
#define QL_w_hash(X)        (X)->whash
#define QL_link(X)          (X)->next
#define next_cell(X)        (X)->next

/*
HEAPptr SQ_CONFIG_HEAP;
HEAPptr SQ_OUT_CELL_HEAP;
HEAPptr Q2_LIST_HEAP;
HEAPptr  Q2_QUEL_HEAP;
HEAPptr HASH_CELL_HEAP;
*/
/* Allocate memory for sq_net_epsilon_closures . Net can be NULL. Hash_factor
   is a small integer, usually (can be 0). */
void FstCalcExport allocate_seq_memory(NETptr net, int hash_factor);
void FstCalcExport reclaim_seq_memory(void);

void FstCalcExport reset_visit_marks_and_client_cells_to_zero (NETptr t1);

/* Compute eventual epsilon closures of all fst states and put them into the
   client_cells */
void FstCalcExport sq_net_epsilon_closures (NETptr t1,
			      id_type (* iside_id) (id_type id), 
			      id_type (* oside_id) (id_type id));

int FstCalcExport sq_is_fst_sequentiable (NETptr t1, int input_side);

NETptr FstCalcExport sq_subsequentialize (NETptr T1, 
			    int input_side, 
			    int reclaim_p,
			    int test_of_subsequentiality_needed, 
			    int hash_factor,
			    FST_CNTXTptr fst_cntxt);

NETptr FstCalcExport local_sequentialize_net(NETptr fnet, int input_side, FST_CNTXTptr fst_cntxt);

NETptr FstCalcExport local_from_to_sequentialize_net(NETptr fnet, 
				       int input_side, 
				       int from_level, 
				       int over_level,
				       int marking_algorithm,
				       FST_CNTXTptr fst_cntxt);
#ifdef __cplusplus
}
#endif

#endif /* C_FSM_SUBSEQ */
