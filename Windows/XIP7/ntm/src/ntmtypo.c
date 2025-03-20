/* $Id: ntmtypo.c,v 1.2 2007/02/16 16:51:40 ait Exp $ */ 

#include "types.h"
#include "struct.h"
#include "pars-tbl.h"

#include "ntm.h"
#include "ntmextern.h"

#ifdef _WINDOWS
#  include <string.h>
#  define snprintf _snprintf
#endif
#include <assert.h>
/* Store a config as a temporary solution
******************************************/

static inline void addTempoSol(NtmContext *ct,
                               int config_ind,
                               STATEptr state,
                               Side side) {
    TypoDataStruct* typodata = &(ct->typoData);

    if (typodata->tempoSolNb >= TYPO_TEMPOSOL_MAXNB) {
        warningHandler(ct, "Typo temporary sol array overflow");
        return;
    }
    typodata->tempoSol[typodata->tempoSolNb].configind = config_ind;
    typodata->tempoSol[typodata->tempoSolNb].cstate = state;
    typodata->tempoSol[typodata->tempoSolNb++].side = side;
}

/* Collect all the longest tempo solutions as the final sols
************************************************************/

static inline NormElement *
collectFinalSol( NtmContext* ct, 
                 long initpos,
                 NETptr net) {
    TypoDataStruct* typodata = &(ct->typoData);
    TypoConfigStruct *configs = typodata->configs;
    int i=0, j=0, ind=0, clength=0;
    int found_sol = 0; id_type symbolid = ID_NO_SYMBOL;
    NormState *normstate=NULL;

    /* select the final sol : check left and right constraints */
    typodata->finalSolNb = typodata->longestInputLength = 0;
    typodata->longestOutputLength = 0;
    for(i=typodata->tempoSolNb-1; i>=0; i--) {
        ind = typodata->tempoSol[i].configind;
        if ( (j = configs[ind].cpos - initpos) == 0) continue;
        if (j >= typodata->longestInputLength) {
            if ( (typodata->tempoSol[i].side == NONE) ||
                 ( (typodata->tempoSol[i].side == LEFT) &&
                   (checkLeftContext( ct, &(ct->levels), initpos - 1,
                                     configs[ind].cpos,
                                     typodata->tempoSol[i].cstate,
                                     net) > 0 ) ) ||
                 (checkRightContext(ct, &(ct->levels), configs[ind].cpos,
                                      typodata->tempoSol[i].cstate,
                                      net) > 0 ) ) {
                if (j > typodata->longestInputLength) {
                    typodata->finalSolNb = 0;
                    typodata->longestInputLength = j;
                }
                if (configs[ind].outputLength >
                    typodata->longestOutputLength) {
                    typodata->longestOutputLength =
                        configs[ind].outputLength;
                }
                typodata->finalSol[typodata->finalSolNb++] = ind;
            }
        }
    }

    clength = typodata->longestOutputLength;
    /* collect the final solutions */
    while ( typodata->finalSolNb > 0 ) {
        found_sol = 1;
        for(i=0, j=0; i < typodata->finalSolNb; i++) {
            ind = typodata->finalSol[i];
            if (clength == typodata->longestOutputLength) {
                /* first time in this loop */
                configs[ind].isFinal = 1;
            }     
            if (configs[ind].outputLength < clength) {
                /* this path is not enough long, process it later */
                if (j == TYPO_TEMPOSOL_MAXNB) {
                    warningHandler(ct, "finalSol array overflow");
                    return NULL;
                }
                typodata->finalSol[j++] = ind;
                continue;
            } else if ( (configs[ind].outputId == EPSILON) &&
                        (configs[ind].previous >= 0) ) {
                /* skip this EPSILON arc by storing previous */
                if (j == TYPO_TEMPOSOL_MAXNB) {
                    warningHandler(ct, "finalSol array overflow");
                    return NULL;
                }
                typodata->finalSol[j++] = configs[ind].previous;

                if (configs[configs[ind].previous].nextNb == 0) {
                    configs[configs[ind].previous].firstNext =
                        configs[ind].firstNext;
                }
                configs[configs[ind].previous].nextNb +=
                    configs[ind].nextNb;
                if (configs[ind].isFinal) {
                    configs[configs[ind].previous].isFinal = 1;
                }
                continue;
            }
            normstate = createNormState( ct, configs[ind].outputId,
                                         configs[ind].isFinal,
                                         configs[ind].firstNext,
                                         configs[ind].nextNb);
            
            if ( (ind = configs[ind].previous) >= 0 ) {
                /* has a previous state */
                if ( configs[ind].firstNext == NULL) {
                    /* previous state not yet stored */
                    /* set the next pointers to this */
                   typodata-> configs[ind].firstNext = normstate;
                    configs[ind].nextNb = 1;
                    /* add previous state to the new list of states to 
                       be stored */
                    if (j == TYPO_TEMPOSOL_MAXNB) {
                        warningHandler(ct, "finalSol array overflow");
                        return NULL;
                    }
                    typodata->finalSol[j++] = ind;
                } else {
                    configs[ind].nextNb++;
                }
            }
        }
        typodata->finalSolNb = j;
        clength--;
    }
    if (found_sol == 0) {
        /* no output from this typo */
        /* no output from typo, take the input as it is */
        symbolid = readInputCharId(ct, &(ct->levels), initpos);
        if (symbolid == ID_EOT) return NULL;
        normstate = createNormState( ct, EPSILON,
                                     0,
                            createNormState(ct, symbolid, 1, NULL, 0),
                                     1);
        if (normstate == NULL) return NULL;
        typodata->longestInputLength = 1;
    }

    return createNormElement(ct, initpos,
                             typodata->longestInputLength, normstate);
}


/* Creates a new config  from current configs[i], having
   a new state next_state, with lower_id and upper_id as the arc symbol
   ids.
***************************************************************/

inline void createNewConfig(NtmContext *ct, int i, id_type input_id,
                     id_type lower_id, id_type upper_id,
                     STATEptr next_state) {
    TypoDataStruct *td = &(ct->typoData);
    /* there was a successful transition with this arc
       process the out symbol */
    if (upper_id == OTHER) {
        errorHandler(ct,"Net contains OTHER on upper side!");
        return;
    }
    /* if ( (next_state == td->configs[i].cstate) &&
         (upper_id == td->configs[i].outputId) ) {
        /* there's a loop on a state with same output 
        if ( (lower_id != EPSILON) &&
             (lower_id != ALTCHAIN_SYMBOL) ) td->configs[i].cpos++;
        td->newcconfigs[td->newcconfigsNb++] = i;
        return;
    } */

    if ( td->configsNb == TYPO_CONFIGS_MAXNB ) {
        warningHandler(ct, "Typo configs array overflow");
        return;
    } else if (td->newcconfigsNb == TYPO_CCONFIGS_MAXNB) {
        warningHandler(ct, "Typo cconfigs overflow");
        return;
    }

    td->configs[td->configsNb].previous = i;
    td->configs[td->configsNb].outputId = upper_id;

    td->configs[td->configsNb].cstate = next_state;
    td->configs[td->configsNb].outputLength = td->configs[i].outputLength+1;
    td->configs[td->configsNb].isFinal = 0;
    td->configs[td->configsNb].firstNext = NULL;
    td->configs[td->configsNb].nextNb = 0;

    if ( (lower_id == EPSILON) ||
         (lower_id == ALTCHAIN_SYMBOL) ) {
            /* go on without consuming the input */
        td->configs[td->configsNb].cpos = td->configs[i].cpos;
    } else {
        td->configs[td->configsNb].cpos = td->configs[i].cpos+1;
    }

    if ( STATE_final(td->configs[td->configsNb].cstate) ) {
        /* successful path ! */
        addTempoSol(ct, td->configsNb,
                    td->configs[td->configsNb].cstate, NONE);
    }

    td->newcconfigs[td->newcconfigsNb++] = td->configsNb++;

    return;
}

/* returns 1 if there is a string starting from
 initpos to side and that was matched by a net path starting
 from state fromstate.
 If no match at all, it returns -1
 */
inline int checkRightContext( NtmContext* ct, Levels *lev, long rightpos,
                              STATEptr fromstate,
                              NETptr net) {
    id_type cid=ID_NO_SYMBOL, clabel=ID_NO_SYMBOL, *range_items=NULL;
    int i=0, range_len=0; long cpos = rightpos;
    STATEptr cstate=NULL;
    RANGEptr range=NULL;
    ARCptr arc=NULL;
    AVptr arc_vector=NULL;

    cstate = fromstate;
    while (!STATE_final(cstate)) {
        cid = readInputCharId(ct, lev, cpos);
        if ( STATE_vector_p(cstate) ) {
            /* vectorized state */
            arc_vector = STATE_arc_vector(cstate);
            cstate = NULL;
            range_len = 0 ;
            if (binary_sigma_member( NET_sigma(net), cid)) {
                range = NET_uprange_map( net ) + cid ;
                range_len = RANGE_len ( range ) ;
                range_items = RANGE_items ( range ) ;
            } else if (!IS_TEXTBOUND(cid)) {
                range = NET_uprange_map( net ) + OTHER ;
                range_len = RANGE_len ( range ) ;
                range_items = RANGE_items ( range ) ;
            }
            for(i=0; i < range_len; i++) {
                if ( cstate = AV_dest(arc_vector, range_items[i])  ) {
                    /* match ! */
                     break;
                }
            }
        } else {
            /* normal state */
            for (arc = STATE_arc_set(cstate), cstate=NULL;
                 arc; arc = next_arc(arc)) {
                clabel = ARC_label(arc);
                if ( cid == clabel ) {
                    /* MATCHED */
                    cstate = ARC_destination(arc);
                } else if ( (clabel == OTHER) &&
                            !IS_TEXTBOUND(cid) &&
                            (!sigma_member( NET_sigma(net), cid)) ) {
                    cstate = ARC_destination(arc);
                }
                if (cstate) break;
            }
        }
        if (cstate==NULL) {
            /* FAILURE AT CURRENT STATE cstate
               can't advance any longer !! */
            return -1 ;
        } else {
            cpos++;
            if (cid == ID_EOT) {
                break;
            }
        }
    }
    return (STATE_final(cstate)) ? 1 : -1;
}

inline int checkLeftContext( NtmContext *ct, Levels* lev, long leftpos,
                             long rightpos,
                             STATEptr fromstate,
                             NETptr net) {

    id_type cid=ID_NO_SYMBOL, clabel=ID_NO_SYMBOL, *range_items=NULL;
    int i=0, range_len=0; long cpos = leftpos;
    STATEptr cstate=NULL;
    RANGEptr range=NULL;
    ARCptr arc=NULL;
    AVptr arc_vector=NULL;

    cstate = fromstate;
    cid = getBackInputCharId(lev, cpos);
    while ( cstate && !STATE_final(cstate) ) {
        if ( STATE_vector_p(cstate) ) {
            /* vectorized state */
            arc_vector = STATE_arc_vector(cstate);
            cstate = NULL;
            /* Right context constraints ? */
            if ( binary_sigma_member( NET_sigma(net), ID_RC) ) {
                range = NET_uprange_map( net ) + ID_RC ;
                range_len = RANGE_len ( range ) ;
                range_items = RANGE_items ( range ) ;
                for(i=0; i < range_len; i++) {
                    if ( cstate = AV_dest(arc_vector, range_items[i])  ) {
                        /* match with RC ! */
                        return checkRightContext(ct, lev, rightpos,
                                             /* ARC_destination(arc),*/
                                             cstate,
                                            net) ;
                    }
                }
            }
            range_len = 0 ;
            if ( binary_sigma_member( NET_sigma(net), cid) ) {
                range = NET_uprange_map( net ) + cid ;
                range_len = RANGE_len ( range ) ;
                range_items = RANGE_items ( range ) ;
            } else if (!IS_TEXTBOUND(cid)) {
                /* Match other ONLY IF cid not in Sigma AND cid
                   is not a text boundary (BOT or EOT) */
                range = NET_uprange_map( net ) + OTHER;
                range_len = RANGE_len ( range ) ;
                range_items = RANGE_items ( range ) ;
            }
            for(i=0; i < range_len; i++) {
                if ( cstate = AV_dest(arc_vector, range_items[i])  ) {
                    /* match ! */
                     break;
                }
            }
        } else {
            /* normal state */
            for (arc = STATE_arc_set(cstate), cstate=NULL;
                 arc; arc = next_arc(arc)) {
                clabel = ARC_label(arc);
                if ( cid == clabel ) {
                    /* MATCHED */
                    cstate = ARC_destination(arc);
                } else if ( (clabel == OTHER) && !IS_TEXTBOUND(cid) &&
                            (!sigma_member( NET_sigma(net), cid)) ) {
                    cstate = ARC_destination(arc);
                } else if (clabel == ID_RC) {
                    /* Right context constraint begins here */
                    return checkRightContext(ct, lev, rightpos,
                                             ARC_destination(arc),
                                             net) ;
                }
                if (cstate) break;
            }
        }
        cpos--;
        cid = getBackInputCharId(lev, cpos);
    }
    if ( cstate && STATE_final(cstate))
        return 1;
    else
        return -1;    
}


/* From the state of the current config, and using input_id, try to transit
   through the out arcs, and for each succesful transition, create a new
   config with the destination state
***********************************************************************/
inline void transitFromNormalState(
         NtmContext *ct,
         NETptr net, /* the net */
         int x, /* the pointer to current config */
         id_type input_id /* the input symbol id */
    ) {
    ARCptr arc=NULL;
    id_type arc_id, lower_id, upper_id;
    TypoDataStruct *td = &(ct->typoData);
    
    /* for each arc of the current state */
    for(arc=STATE_arc_set (td->configs[x].cstate); arc; arc = next_arc (arc)) {
        /* get the id of this arc label */
        arc_id = ARC_label(arc);
        /* get the id of the lower symbol of this arc label */
        lower_id = LOWER_MATCH[arc_id];
        /* get the id of the upper symbol of this arc label */
        upper_id = UPPER_MATCH[arc_id];
        
        if ( (lower_id == OTHER) &&
             !binary_sigma_member( NET_sigma(net), input_id) &&
             !IS_TEXTBOUND(input_id) ) {
            /* the input symbol doesn't belong to the net sigma
               => it matches the lower_id = OTHER */
            upper_id = input_id;
            createNewConfig(ct, x, input_id, lower_id,
                                   upper_id, ARC_destination(arc));
        } else if ( (lower_id == EPSILON) ||
                    (lower_id == ALTCHAIN_SYMBOL) ||
                    ( lower_id == input_id ) ) {
            createNewConfig(ct, x, input_id,lower_id,
                                   upper_id, ARC_destination(arc));
        } else if (lower_id == ID_LC) {
            addTempoSol(ct, x, ARC_destination(arc), LEFT);
        } else if (lower_id == ID_RC) {
            addTempoSol(ct, x, ARC_destination(arc), RIGHT);
        }
    } /*end for each arc*/
    return;
}


/* Same as above but from a vectorized state
********************************************/

inline void transitFromVectorizedState(
        NtmContext *ct,
        NETptr net, /* the net */
        int x, /* the pointer to current config */
        id_type input_id /* the input symbol id */
    ) {
    TypoDataStruct *td = &(ct->typoData);
    id_type arc_id, lower_id, upper_id, *range_items;
    RANGEptr range;
    AVptr arc_vector;
    ALPHABETptr low_eps;
    STATEptr dest;
    int i=0;
    int range_len;


    arc_vector = STATE_arc_vector(td->configs[x].cstate);

    /* check if any Left Context constraint */
    if (binary_sigma_member( NET_sigma(net), ID_LC)) {
        range = NET_uprange_map( net ) + ID_LC;
        range_len = RANGE_len ( range );
        range_items = RANGE_items ( range ) ;
        for(i = 0; i < range_len; i++) {
            if (dest = AV_dest(arc_vector, range_items[i]) ) {
                addTempoSol(ct, x, dest, LEFT);
                break;
            }
        }
    }
    if (binary_sigma_member( NET_sigma(net), ID_RC)) {
    /* check if any Right Context constraint */
        range = NET_uprange_map( net ) + ID_RC;
        range_len = RANGE_len ( range );
        range_items = RANGE_items ( range ) ;
        for(i = 0; i < range_len; i++) {
            if (dest = AV_dest(arc_vector, range_items[i]) ) {
                addTempoSol(ct, x, dest, RIGHT);
                break;
            }
        }
    }

    if (input_id != EPSILON) {
        range_len = 0;
        if ( binary_sigma_member( NET_sigma(net), input_id) ) {
            range = NET_uprange_map( net ) + input_id ;
            range_len = RANGE_len ( range );
            range_items = RANGE_items ( range );
        } else if (!IS_TEXTBOUND(input_id)) {
            range = NET_uprange_map( net ) + OTHER;
            range_len = RANGE_len ( range );
            range_items = RANGE_items ( range ) ;
        }
        
        for(i = 0; i < range_len; i++) {
            arc_id = range_items[i];
            if ( (dest = AV_dest(arc_vector, arc_id) ) ) {
                /* successful match */
                lower_id = LOWER_MATCH[arc_id];
                upper_id = UPPER_MATCH[arc_id];
                /* If we are matching OTHER, use the text symbol
                   itself. */
                if (upper_id == OTHER && RANGE_id(range) == OTHER) {
                    upper_id = input_id;
                }
                createNewConfig(ct, x, input_id, lower_id, upper_id, dest);
            }
        }
    }

    if ( (low_eps = AV_l_eps(arc_vector)) ) {
        /* arcs with EPSILON on the lower side */
        for (i = 0; i < ALPH_len(low_eps); i++) {
            arc_id = ALPH_item(low_eps, i);
            upper_id = UPPER_MATCH[arc_id];
            dest = AV_dest(arc_vector, arc_id);
            createNewConfig(ct, x, EPSILON, EPSILON, upper_id, dest);
        }
    }
    return;
} /*transitFromVectorizedState*/




/* Reads file and process
*************************/

inline NormElement *readNormElement2(NtmContext *ct, int initpos, NETptr net) {

    int i=0, j=0, k=0, new_j=0;
    id_type in_id=ID_NO_SYMBOL;
    TypoDataStruct *td = &(ct->typoData);
    TypoConfigStruct *configs = td->configs;

    td->tempoSolNb = 0;
    if (net == NULL) return collectFinalSol(ct, initpos, net);
    resetTypo(ct, net);

    td->configs[0].cpos = initpos;

    while ( (td->cconfigsNb > 0) ) {
        td->newcconfigsNb = 0;
        /* for each of the current states in the configs */
        for(i=0, k=td->cconfigs[i]; i < td->cconfigsNb && !ntmError(ct); i++, k=td->cconfigs[i]) {
            /* read the next input for this config */
            in_id = readInputCharId(ct, &(ct->levels), configs[k].cpos);
            if (in_id == ID_EOT) {
                /* end of input buffer, consider EPSILON end transitions */
                in_id = EPSILON;
            }
            if ( STATE_vector_p(configs[k].cstate) ) {
                /* this is a vectorized state */
                transitFromVectorizedState(ct, net, k, in_id);
            } else {
                /* state is not vectorized */
                transitFromNormalState(ct, net, k, in_id);
            }
        } /*end foreach current state */

        td->cconfigsNb = td->newcconfigsNb;
        td->cconfigs = td->newcconfigs;
        td->newcconfigs = (td->newcconfigs==td->cconfigs1) ?
            td->cconfigs2 : td->cconfigs1;
    } /*end while success_match */

    return collectFinalSol(ct, initpos, net);
}

/*************************************************************
 Printing
*************************************************************/

int snprintPaths2(char *s, int max, NormState *normstate,
                 id_type *pref, int lg, int maxlg) {
    int i=0, ind=0;
    if (normstate->isFinal > 0) {
        s[ind++] = '|';
        pref[lg] = ID_NO_SYMBOL;
        ind += ntmSnprintIdSeq(s+ind, max-ind, pref);
        s[ind++] = '|';
        s[ind++] = ',';
        s[ind++] = ' ';
        s[ind++] = ' ';
    }
    if ( (lg >= maxlg) || (normstate->nextNb == 0) ) return ind;
    i = normstate->nextNb;
    normstate = normstate->firstNext;
    while (i > 0) {
        if (normstate->symbolId != EPSILON) {
            pref[lg] = normstate->symbolId;
            ind += snprintPaths2(s+ind, max-ind, normstate, pref, lg+1, maxlg);
        } else {
            ind += snprintPaths2(s+ind, max-ind, normstate, pref, lg, maxlg);
        }
        normstate++; i--;
    }
    return ind;
}

int snprintPaths(char *s, int max, NormState * normstate) {
    id_type idseq[TYPO_OUTPUTSTRING_MAXSIZE];
    return snprintPaths2(s, max, normstate, idseq, 0,TYPO_OUTPUTSTRING_MAXSIZE);
}

void debugPaths(NormState * normstate) {
    char s[200];
    fputs("=============Begin\n", stdout );
    snprintPaths(s, 200, normstate);
    fputs(s, stdout);
    fputs("\n=============End\n", stdout );
}


void FsmNTMExport snprintNorm(Levels *lev, char *s, int max, NormElement *normres) {
    long ind=0, pos=0, i=0;
    char ch[NTM_SYMBOL_MAXSIZE];

    ind += snprintf(s+ind, max-ind,"%d-%d:|", normres->initpos,
                    normres->initpos+normres->inputLength);
    for(pos=normres->initpos, i=0; i < normres->inputLength; pos++, i++) {
        FST_fat_to_thin( id_to_atom_name(getInputCharId(lev, pos)), ch);
        ind += snprintf(s+ind, max-ind,"%s", ch);
    }
    ind += snprintf(s+ind, max-ind, "| ===> ");
    ind += snprintPaths(s+ind, max-ind, normres->normState);
    s[ind++] = '\n';
    s[ind] = 0;
}



