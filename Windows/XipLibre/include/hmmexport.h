#ifndef HMMEXPORT_H
#define HMMEXPORT_H

#include <stdio.h>
#include "noeud.h"
#include "lliste.h"

#include "parsbin.h"


extern "C" {
#include "desamb/dstypes.h"

#include "fst/struct.h"
#include "fst/context.h"
#include "fst/pars-tbl.h"

#include "ntm.h"

    int InitDisambText (char *nameHMM);
    void CloseHMM();
    void viterbi_start (VTREEptr vtree);
    void viterbi_step (VTREEptr vtree);
    void viterbi_end (VTREEptr vtree);
    void delete_vcell_list (VCELLptr vcell, VTREEptr vtree);
    void exchange_vit_vectors (VTREEptr vtree);
    void clear_vit_vector_and_lists (VITPOSptr vec, VTREEptr vtree);
    void set_current_class_data (WORDBFptr wbf, VTREEptr vtree);
    void analyse_input_line (char *line, int *num_tokens, int *tag);
    void store_to_word_buffer(WORDBFptr wbf, char *line, int tag, int new_word_p);
    void ntmReset(NtmContext* ct);
    void resetNtmMemory(NtmMemory * mem);
}


#endif

