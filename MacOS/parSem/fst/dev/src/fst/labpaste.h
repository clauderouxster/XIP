/* $Id: labpaste.h,v 1.4 2005/03/04 15:45:24 grondeau Exp $ */
/* Copyright (c) 1990, 1991 by the Xerox Corporation.  All rights reserved */

/***************************************
 *
 *   FUNCTIONS FOR LABEL PASTING
 *
 *     'a'  'b'  'c'  ->  'abc'
 *
 **************************/


#ifndef LABPASTE_H
#define LABPASTE_H
/*--------------------------------------------------------*/

#define NO_ID                   -1
#define FLAG_start_state(X)    STATE_visit_mark(X)
#define FLAG_handle_state(X)   (X)->userflag2

#define FLAG_handle_arc(X)     (X)->unused

#define PSTLAB_ptr     pasted_labels
#define PSTLAB_buffer  pasted_labels->buffer
#define PSTLAB_size    pasted_labels->buffer_size

NETptr FstCalcExport PLB_paste_net_labels(NETptr net1, int reclaim_p, int minimize_p);

/*--------------------------------------------------------*/
#endif /* LABPASTE_H */

