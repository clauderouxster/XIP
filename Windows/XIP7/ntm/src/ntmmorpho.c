/* $Id: ntmmorpho.c,v 1.2 2007/02/16 16:51:40 ait Exp $ */ 


#include "ntm.h"
#include "ntmextern.h"

#include "pars-tbl.h"
#include "bin-in.h"



extern NormElement *readNormElement(NtmContext* ct, long initpos, NETptr net);



/*****************************************************************
 check the candidate solution and store it if ok
*****************************************************************/

inline void processTempoSol(NtmContext *ct, int configind, Side side) {
    static int length=0, cpos=0, i=0, prev=0;
    int initpos = ct->inputPos;
    MorphoDataStruct *md = &(ct->morphoData);
    MorphoConfigStruct *configs = md->configs;
    MorphoTempoSolStruct *tempoSol= md->tempoSol;
    int tempoSolNb = md->tempoSolNb;
    
    if ( IS_FINAL(configs[configind].normState) ||
         (configs[configind].normState==NULL) ) {
        cpos = configs[configind].cpos;
    } else if (IS_INITIAL(configs[configind].normState) ) {
        cpos = configs[configind].cpos - configs[configind].normlength;
    } else {
        return;
    }

    if (tempoSolNb == MORPHO_TEMPOSOL_MAXSIZE) {
        warningHandler(ct, "Morpho temposol array overflow");
        return;
    } else if ( (length=cpos-initpos) > (INPUTTOKEN_MAXSIZE-1) ) {
        /* recognized token too long, ignore it */ 
        warningHandler(ct, "Token too long.");
        return;
    } else if (length == 0) {
        /* length zero: in case empty words are defined in the
           lexicon */
        return;
    } else {

        /* typos constraints on separators */

		if ( (ct->checkSeparatorConstraint == FALSE) ||
             (side != NONE) ||
             ( ( isSeparator(ct,
                             readInputCharId(ct, &(ct->levels),cpos) ) ||
                 isSeparator(ct,
                             getInputCharId(&(ct->levels),cpos-1)) )  ) ) {

            tempoSol[tempoSolNb].length = length;
            tempoSol[tempoSolNb].configInd = configind;
            tempoSol[tempoSolNb].cstate = configs[configind].cstate;
            tempoSol[tempoSolNb].weight = configs[configind].weight;
            tempoSol[tempoSolNb].side = side;

            if (md->longestSolInd < 0) {
                /* first sol */
                tempoSol[0].next = -1;
                md->longestSolInd = 0;
            } else if ( (tempoSol[tempoSolNb].length >=
                         tempoSol[md->longestSolInd].length) &&
                        (tempoSol[tempoSolNb].weight >=
                         tempoSol[md->longestSolInd].weight) ){
                /* longest than previously found solutions, insert at
                 the top of the list */
                tempoSol[tempoSolNb].next = md->longestSolInd;
                md->longestSolInd = tempoSolNb;
            } else {
                i = md->longestSolInd;
                while ( (tempoSol[tempoSolNb].length < tempoSol[i].length) 
                        && (i>=0) ) {
                    prev = i;
                    i = tempoSol[i].next;
                }
                while ( (tempoSol[tempoSolNb].weight < tempoSol[i].weight)
                        && (i>=0) ) {
                    prev = i;
                    i = tempoSol[i].next;
                }
                tempoSol[prev].next = tempoSolNb;
                tempoSol[tempoSolNb].next = i;
            }
            md->tempoSolNb++;
        }
    }
}


/**********************************************************************
  if no success with the lex fsts, collect the string between spaces 
***********************************************************************/

static inline
NtmToken * collectDefaultSol(NtmContext *ct) {
    int i=0, length=0, ignore_flag=0;
    int  c=0;
    NtmToken * token = NULL;
    id_type *normform=NULL, *features=NULL;

    i = ct->inputPos;
    c = getInputCharId(&(ct->levels), i);
    /* if current is space */
    if ( isSeparator(ct, c) ) {
        if (isSpace(ct, c)) {
            ignore_flag = 1;
            /* this is a space sequence, collect it and set ID_IGNORE */
            while ( (c!=ID_EOT) && isSpace(ct, c) ) {

                length++;
                i++;
                c = readInputCharId(ct, &(ct->levels), i);
            }
        } else {
            /* just one separator */
            length = 1;
        }
    } else {
        if ( (c!=ID_EOT) && (ct->unknownStringMode == BYCHAR) ) {
            length++; i++;
        } else {
            while ( (c!=ID_EOT) && !isSeparator(ct,c) ) {
                length++;
                i++;
                c = readInputCharId(ct, &(ct->levels),i);
            }
        }
    }
    token = createToken(ct,ct->inputPos, length, ignore_flag);

    normform = createIdSeq(ct,length);
    if (normform != NULL) {
        for(i=0; i < length; i++)
            normform[i] = readInputCharId(ct,&(ct->levels),i+ct->inputPos);
        normform[length] = ID_NO_SYMBOL;
    }
    features = createIdSeq(ct,1);
    if (normform != NULL) {
        features[0] = ID_UNKNOWN;
        features[1] = ID_NO_SYMBOL;
    } 
    addEntryToToken(token,
                    createEntry(ct,normform, normform, features));
    return token;
}

/* Same as 'collectDefaultSol" above, except that it reads the whole remaining string
  (including spaces ans separators) as a single unknown token
*********************************************************************************/

NtmToken * getUnknownToken(NtmContext *ct) {

    int i=0, length=0;
    int  c=0;
    NtmToken * token = NULL;
    id_type *normform=NULL, *features=NULL;

    /* i = ct->inputPos; */
    i = ct->inputPos;
    c = getInputCharId(&(ct->levels), i);
    while ( c!=ID_EOT ) {
        length++;
        i++;
        c = readInputCharId(ct, &(ct->levels),i);
    }    
    length--;
    token = createToken(ct,ct->inputPos, length, 0);

    normform = createIdSeq(ct,length);
    if (normform != NULL) {
        for(i=0; i < length; i++)
            normform[i] = readInputCharId(ct, &(ct->levels), i+ct->inputPos);
        normform[length] = ID_NO_SYMBOL;
    }

    features = createIdSeq(ct,1);
    features[0] = ID_UNKNOWN;
    features[1] = ID_NO_SYMBOL;
    addEntryToToken(token,
                    createEntry(ct,normform, normform, features));
    return token;
}


/***********************************************************************
  Collect the final solutions after checking context etc.
************************************************************************/

static inline
NtmToken * collectFinalSol(NtmContext *ct, NETptr net) {
    NtmToken *token=NULL;
    id_type *input_idseq=NULL, *lemma=NULL, *features=NULL; 
    int cind=0, c=0, last_ind=0, i=0, j=0, lg=0, weight=0, tambig=0;
	size_t nLemmaLength, nFeaturesLength;

    long initpos = ct->inputPos;
    MorphoDataStruct *md = &(ct->morphoData);
    MorphoConfigStruct *configs = md->configs;
    MorphoTempoSolStruct *tempoSol= md->tempoSol;

    md->finalSolNb = 0;
    for(i= md->longestSolInd; i >= 0; i = tempoSol[i].next) {
        j = tempoSol[i].length;
        cind = tempoSol[i].configInd;
        if (j >= lg) {
            if (tempoSol[i].weight >= weight) {
                if ( ( tempoSol[i].side == NONE) ||
                     ( (tempoSol[i].side == LEFT) &&
                       (checkLeftContext( ct, &(ct->levels), initpos - 1,
                                          initpos+j,
                                          tempoSol[i].cstate,
                                          net) > 0 ) ) ||
                     ( (tempoSol[i].side == RIGHT) &&
                       (checkRightContext(ct, &(ct->levels), initpos+j,
                                         tempoSol[i].cstate,
                                         net) > 0 ) ) ) {
                    lg = j;
                    weight = tempoSol[i].weight;
                    tambig = configs[cind].tambig;
                    md->finalSol[md->finalSolNb++] = cind;
                }
            } else if (md->finalSolNb > 0) {
                /* for next shorter tokens */
               for(; (i >= 0) && (tempoSol[i].length==lg);
                   i=tempoSol[i].next);
               break;
            }
        } else if (md->finalSolNb > 0) {
            break;
        }
    }

    /* lg is the length of the satisfying token */
    if (md->finalSolNb == 0) return NULL;
    /* for next shorter tokens */
    md->longestSolInd = i;
    for(i=0; i < md->finalSolNb; i++) {
        c = md->finalSol[i];

        if (token == NULL) {
            /* first solution found */
            token = createToken(ct, initpos,
                                lg, 
                                configs[c].ignore);
            if (tambig) tokenSetTambig(token);
        }
        /* collect the norm input id sequence */
        input_idseq = createIdSeq(ct, configs[c].inputLength);
        if (input_idseq != NULL) {
            for(j=configs[c].inputLength-1, cind=c;
                cind > 0;
                cind = configs[cind].previous) {
                if ( !IS_EMPTY(configs[cind].inputId) ) {
                    input_idseq[j] = configs[cind].inputId;
                    j--;
                }
            }
            input_idseq[configs[c].inputLength] = ID_NO_SYMBOL;
        }

        /* collect the outputIdSeq */

        /* Find the length of the features */
	nFeaturesLength = configs[c].featuresLength ;
		
	/* first, the features */
        features = createIdSeq(ct, nFeaturesLength + 1);
        cind = c;
        /* in case of an RC or LC config, skip it: do not collect
           the RC or LC symbol as part of the feature sequence */
        if ( (configs[cind].outputId == ID_RC) ||
             (configs[cind].outputId == ID_LC) )
             cind = configs[cind].previous;
        if (features != NULL) {
            for(j=nFeaturesLength -1;
                  (configs[cind].specialIds);
                  cind = configs[cind].previous) {
                if (configs[cind].outputId != EPSILON)
                    features[j--] = configs[cind].outputId;
            }
            features[nFeaturesLength] = ID_NO_SYMBOL;
        }

        /* then the lemma */
	nLemmaLength = configs[c].lemmaLength ;

        lemma = createIdSeq(ct, nLemmaLength + 1);
        if (lemma != NULL) {
            for(j=nLemmaLength - 1;
                cind > 0;  cind = configs[cind].previous) {
                if ( !IS_EMPTY(configs[cind].outputId) ) {
                    lemma[j] = configs[cind].outputId;
                    j--;
                }
            }
            lemma[nLemmaLength] = ID_NO_SYMBOL;
        }
        addEntryToToken(token,
                        createEntry(ct,input_idseq, lemma, features)); 

    }
    tokenSetWeight(token, weight);
    return token;
}


/****************************************************************** 
 Gets a recognized token and if it's tok-ambiguous, gets the shorter
  token if any, and so on...
*******************************************************************/
NtmToken * getRecognizedToken(NtmContext *ct, NETptr net) {
    NtmToken *firsttoken=NULL, *lasttoken=NULL, *token=NULL;
    token = firsttoken = collectFinalSol(ct, net);
    while (token && token->tambig) {
        token->shorterToken = collectFinalSol(ct, net);
        if (token->shorterToken) {
            token->shorterToken->longerToken = token;
        }
        token = token->shorterToken;
    }
    return firsttoken;
}



/************************************************************************ 
   Creates a new config from current config i, having
   a new state next_state, with lower_id and upper_id as the arc symbol
   ids.
*************************************************************************/

static
inline void createNewConfig( NtmContext *ct, int i,
                     id_type input_id, 
                     id_type upper_id,
                     long cpos,
                     NormState *norm_state,
                     STATEptr next_state ) {
    MorphoDataStruct *md = &(ct->morphoData);
    int configsNb = md->configsNb;
    MorphoConfigStruct *configs = md->configs;

    if (upper_id == OTHER) {
        errorHandler(ct, "Net contains OTHER on upper side!");
        return;
    }
    if ( configsNb == MORPHO_CONFIGS_MAXNB ) {
        warningHandler(ct, "Morpho configs array overflow");
        return;
    } else if (md->newcconfigsNb == MORPHO_CCONFIGS_MAXNB) {
        warningHandler(ct, "Morpho cconfig array overflow");
        return;
    }

    configs[configsNb].previous = i;
    configs[configsNb].inputId = input_id;
    configs[configsNb].outputId = upper_id;
    configs[configsNb].cpos = cpos;
    configs[configsNb].normState = norm_state;
    configs[configsNb].cstate = next_state;
    
    configs[configsNb].normlength = configs[i].normlength;
    configs[configsNb].lemmaLength = configs[i].lemmaLength;
    configs[configsNb].featuresLength = configs[i].featuresLength;
    configs[configsNb].inputLength = configs[i].inputLength;
    configs[configsNb].specialIds = configs[i].specialIds;
    configs[configsNb].ignore = configs[i].ignore;
    configs[configsNb].tambig = configs[i].tambig;
    configs[configsNb].weight = configs[i].weight;
    
    if ( !IS_EMPTY( upper_id ) && (fat_strlen(id_to_atom_name(upper_id)) > 1) ) { /* the upper id is a feature ! */
        configs[configsNb].specialIds = 1;
        if (upper_id == ID_IGNORE) {
            configs[configsNb].ignore = 1;
        } else if (upper_id == ID_TAMBIG) {
            configs[configsNb].tambig = 1;
        } else if (IS_WEIGHT(upper_id)) {
            configs[configsNb].weight = GET_WEIGHT(upper_id);
        }
    }

    if ( !IS_EMPTY( upper_id ) ) {
        if (!configs[configsNb].specialIds) {
            configs[configsNb].lemmaLength++;
        } else { /* no test on if (upper_id != ID_IGNORE) { */
            configs[configsNb].featuresLength++;
        }
    }
        
    if ( !IS_EMPTY( input_id ) )
        configs[configsNb].inputLength++;

    md->newcconfigs[md->newcconfigsNb++] = md->configsNb++; 

    return;
}


/************************************************************************ 
  From the state of the current config, and using input_id, try to transit
   through the out arcs, and for each succesful transition, create a new
   config in pnc with the destination state
   Returns: number of new created configs 
*************************************************************************/

static inline void transitNormal (
        NtmContext *ct,
        NETptr net, /* the net */
        int x /* the pointer to current config */
    ) {
    ARCptr arc=NULL;
    id_type input_id, arc_id, lower_id, upper_id;
    NormState *next_state;
    MorphoDataStruct *md = &(ct->morphoData);
    MorphoConfigStruct *configs = md->configs;
   
    /* for each arc of the current state */
    for(arc = STATE_arc_set (configs[x].cstate); arc; arc = next_arc (arc)) {
        /* get the id of this arc label */
        arc_id = ARC_label(arc);
        /* get the id of the lower symbol of this arc label */
        lower_id = LOWER_MATCH[arc_id];
        /* get the id of the upper symbol of this arc label */
        upper_id = UPPER_MATCH[arc_id];

        /* go through if EPSILON transition */
        if ( (lower_id == EPSILON) ||
             (lower_id == ALTCHAIN_SYMBOL) ||
             (lower_id == ID_LC) || (lower_id == ID_RC) ) {
            if (lower_id == ALTCHAIN_SYMBOL) {
                configs[x].cstate = ARC_destination(arc);
                md->newcconfigs[md->newcconfigsNb++] = x;
            } else {
                createNewConfig( ct, x,
                                 lower_id,
                                 upper_id,
                                 configs[x].cpos,
                                 configs[x].normState,
                                 ARC_destination(arc));
            }
            continue;
        }
        
        if (PATH_END(configs[x].normState)) continue;

        for(next_state = FIRST_NEXT_STATE(configs[x].normState);
            next_state <= LAST_NEXT_STATE(configs[x].normState);
            next_state++) {
            
            input_id = next_state->symbolId;
            if ( (lower_id == OTHER) &&
                 !binary_sigma_member( NET_sigma(net), input_id) &&
                 !IS_TEXTBOUND(input_id)  ) {
                /* the input symbol doesn't belong to the net sigma
                   => it matches the lower_id = OTHER */
                upper_id = input_id;
                createNewConfig( ct, x,
                                 input_id,
                                 upper_id,
                                 configs[x].cpos,
                                 next_state,
                                 ARC_destination(arc));
            } else if ( lower_id == input_id ) {
                createNewConfig( ct, x,
                                 input_id,
                                 upper_id,
                                 configs[x].cpos,
                                 next_state,
                                 ARC_destination(arc));
            }
        } /* end for each input symbol */

    } /*end for each arc*/

    return;
}


/********************************************************************
 From a vectorized state
*********************************************************************/

static inline void transitVectorized(
        NtmContext *ct,
        NETptr net, /* the net */
        int x /* the pointer to current config */
    ) {
    id_type input_id, arc_id, lower_id, upper_id, *range_items;
    RANGEptr range;
    AVptr arc_vector;
    ALPHABETptr low_eps;
    STATEptr dest;
    int i=0, range_len=0;
    NormState *next_state = NULL;
    MorphoDataStruct *md = &(ct->morphoData);
    MorphoConfigStruct *configs = md->configs;


    arc_vector = STATE_arc_vector(configs[x].cstate);

    if (!PATH_END(configs[x].normState)) {

        for(next_state = FIRST_NEXT_STATE(configs[x].normState);
            next_state <= LAST_NEXT_STATE(configs[x].normState);
            next_state++) {
            
            input_id = next_state->symbolId ;
            
            range_len = 0;
            if ( binary_sigma_member( NET_sigma(net), input_id) ) {
                range = NET_uprange_map( net ) + input_id;
                range_len = RANGE_len ( range );
                range_items = RANGE_items ( range );
            } else if (!IS_TEXTBOUND(input_id)) {
                range = NET_uprange_map( net ) + OTHER;
                range_len = RANGE_len ( range );
                range_items = RANGE_items ( range );
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
                    createNewConfig( ct, x,
                                     input_id,
                                     upper_id,
                                     configs[x].cpos,
                                     next_state,
                                     dest);
                }
            }
        } /* for each next_state*/
    } /*if !PATH_END */

    /* anyways, try to go through EPSILON/LC/RC transitions */
    if ( (low_eps = AV_l_eps(arc_vector)) ) {
        /* arcs with EPSILON on the lower side */
        for (i = 0; i < ALPH_len(low_eps); i++) {
            arc_id = ALPH_item(low_eps, i);
            upper_id = UPPER_MATCH[arc_id];
            dest = AV_dest(arc_vector, arc_id);
            createNewConfig( ct, x,
                             EPSILON,
                             upper_id,
                             configs[x].cpos,
                             configs[x].normState,
                             dest);
        }
    }
    /* check if any Left Context constraint */
    if (binary_sigma_member( NET_sigma(net), ID_LC)) {
        range = NET_uprange_map( net ) + ID_LC;
        range_len = RANGE_len ( range );
        range_items = RANGE_items ( range ) ;
        for(i = 0; i < range_len; i++) {
            if (dest = AV_dest(arc_vector, range_items[i]) ) {
                createNewConfig( ct, x,
                                 ID_LC,
                                 ID_LC,
                                 configs[x].cpos,
                                 configs[x].normState,
                                 dest);
                break;
            }
        }
    }
    /* check if any Right Context constraint */
    if (binary_sigma_member( NET_sigma(net), ID_RC)) {
        range = NET_uprange_map( net ) + ID_RC;
        range_len = RANGE_len ( range );
        range_items = RANGE_items ( range ) ;
        for(i = 0; i < range_len; i++) {
            if (dest = AV_dest(arc_vector, range_items[i]) ) {
                createNewConfig( ct, x,
                                 ID_RC,
                                 ID_RC,
                                 configs[x].cpos,
                                 configs[x].normState,
                                 dest);
                break;
            }
        }
    }

    return;

} /*transitVectorized*/



/********************************************************************
 basic token reading and analysing
*********************************************************************/

inline NtmToken *
ntmReadToken(
    NtmContext* ct, /* the ntm context */
    int strategy_ind /* using the norment and lexnet of strategy_ind */
    ) {

    NtmToken *token=NULL;
    int i=0, j=0, k=0;
    id_type in_id= ID_NO_SYMBOL;
    NETptr normnet = ct->strategy[strategy_ind].normnet;
    NETptr lexinet = ct->strategy[strategy_ind].lexnet;
    NormElement *normres=NULL;
    MorphoDataStruct *md = &(ct->morphoData);
    MorphoConfigStruct *configs = md->configs;

    while (token==NULL) {

    if (endOfInput(ct, &(ct->levels), ct->inputPos)) return getLastToken();

    resetMorpho(ct, lexinet);

    normres = readNormElement(ct, configs[0].cpos, normnet);
    configs[0].normState = normres->normState;
    configs[0].normlength = normres->inputLength;
    configs[0].cpos += normres->inputLength;

    while (md->cconfigsNb > 0) {
        md->newcconfigsNb=0;
        /* for each of the current states in the configs */
        for(i=0, k=md->cconfigs[i]; i < md->cconfigsNb ; i++, k=md->cconfigs[i]) {

            /* if this is an LC/RC config then skip it */
            if (configs[k].inputId == ID_LC) {
                continue;
            } else if (configs[k].inputId == ID_RC) {
                continue;
            }
            /* if this normState is final, create a next config that will
               consider reading the next norm seq, add the new config
               into newcconfigs */
            if ( IS_FINAL(configs[k].normState) ) {
                normres = readNormElement(ct,configs[k].cpos, normnet);
                if (normres != NULL) {
                    configs[k].normState = normres->normState;
                    configs[k].cpos += normres->inputLength;
                    configs[k].normlength = normres->inputLength;
                    if ( IS_FINAL(normres->normState) ) {
                        createNewConfig(ct,
                            configs[k].previous,
                            configs[k].inputId,
                            configs[k].outputId,
                            configs[k].cpos,
                            normres->normState,
                            configs[k].cstate);
                    }
                } else {
                    configs[k].normState = NULL;
                }
            }

            if ( STATE_vector_p(configs[k].cstate) ) {
                /* this is a vectorized state */
                transitVectorized(ct, lexinet, k);
            } else {
                /* state is not vectorized */
                transitNormal(ct, lexinet, k);
            }
        } /*end foreach current config state */

        /* for each of the new reached configs states 
           check if they're final  */
        for(j=0; j < md->newcconfigsNb; j++) {
            k = md->newcconfigs[j];
            if ( STATE_final(configs[k].cstate) ) {
                processTempoSol(ct, k, NONE);
            } else if (configs[k].inputId == ID_LC) {
                processTempoSol(ct, k, LEFT);
            } else if (configs[k].inputId == ID_RC) {
                processTempoSol(ct, k, RIGHT);
            }
        }

        md->cconfigsNb = md->newcconfigsNb;
        md->cconfigs = md->newcconfigs;
        md->newcconfigs = (md->newcconfigs==md->cconfigs1) ?
            md->cconfigs2 : md->cconfigs1;
    } /*end while success_match */

    token = getRecognizedToken(ct, lexinet);
    if ( token == NULL ) {
        /* if no token was recognized and current input char is a space
           then skip all spaces and loop again */
        if (isSpace(ct, getInputCharId( &(ct->levels), ct->inputPos))) {
            skipSpaces(ct);
        } else {
            return NULL;
        }
    }
    }
    return token;
}


/********************************************************************
 Reads token by token. If no norm net was provided, process directly
 using lexicon, else, process using typo and lexicon
*********************************************************************/

NtmToken * ntmStrategyReadToken(NtmContext *ct) {
    long strategy_ind=0;
    NtmToken *token=NULL;

    do {
        token = ntmReadToken(ct, strategy_ind);
        strategy_ind++;
        if ( (token==NULL) && (strategy_ind >= ct->strategyNb)) {
            if (isSpace(ct, getInputCharId(&(ct->levels), ct->inputPos))) {
                skipSpaces(ct);
                strategy_ind=0;
            }
        }
    } while ( (token==NULL) && (strategy_ind < ct->strategyNb) );
    
    if (token == NULL) {
        token = collectDefaultSol(ct);
        tokenSetStrategy(token, NTM_LAST_STRATEGY);
    } else {
        tokenSetStrategy(token, strategy_ind-1);
    }
    return token;
}

/********************************************************************
 Structure and utility for handling different paths of tokenization
 when ambiguous
*********************************************************************/

typedef struct {
    long pos;
    long bytepos;
    int numberOfTokens;
    NtmToken *lastToken;
    int cost;
} TokenPathStruct;

void removeSubpath(NtmToken *t, NtmToken** first_token) {
    while ( t && t->previousToken &&
            !t->shorterToken && !t->longerToken ) {
        t = t->previousToken;
        t->nextToken = NULL;
    }
    if (t->shorterToken && t->longerToken) {
        /* middle */
        t->longerToken->shorterToken = t->shorterToken;
        t->shorterToken->longerToken = t->longerToken;
    } else if (!t->shorterToken && t->longerToken) {
        /* tail */
        t->longerToken->shorterToken = t->shorterToken;
    } else if (!t->longerToken && t->shorterToken) {
        /* head */
        t->shorterToken->longerToken = NULL;
        if (*first_token == t) {
            *first_token = t->shorterToken;
        } else {
            /* link to previous */
            t->shorterToken->previousToken = t->previousToken;
            t->previousToken->nextToken = t->shorterToken;
        }
    }
}

/********************************************************************
 Reads net of tokens (possible ambiguity). If no norm net was provided,
process directly using lexicon, else, process using typo and lexicon
*********************************************************************/

NtmToken * ntmReadTokens(NtmContext *ct) {

    NtmToken * token=NULL, *first_token=NULL;
    TokenPathStruct paths[NTM_TOKEN_PATHS_MAXNB];
    int i=0, number_of_paths = 1, unique_endpos=0, min_nbtokens=0;
    long endpos=0;
    int best =-1, cost=-1;
    
    if (ntmError(ct)) return NULL;
    paths[0].pos = ct->inputPos;
    paths[0].bytepos = ct->inputBytePos;
    paths[0].numberOfTokens = 0;
    paths[0].lastToken = NULL;
    paths[0].cost = 0;
    endpos = ct->inputPos+1;

    /* ntmReset(ct); */
    resetNtmMemory(&(ct->memory)); 

    while ( (number_of_paths > 0) &&
            (!unique_endpos) ) {
        unique_endpos = 1;
        for(i=0; i < number_of_paths; i++) {
            if (paths[i].pos >= endpos) continue;
            ct->inputPos = paths[i].pos;
            ct->inputBytePos = paths[i].bytepos;
            token = ntmStrategyReadToken(ct);
            if (first_token==NULL) {
                first_token = token;
            }
            if ( (token == NULL) || ntmIsEofToken(token)) continue;
            if (paths[i].lastToken != NULL) {
                paths[i].lastToken->nextToken = token;
                token->previousToken = paths[i].lastToken;
            }
            paths[i].pos = token->initpos + token->inputLength;
            paths[i].bytepos = token->initBytepos + token->inputByteLength;
            if (paths[i].pos != endpos) {
                unique_endpos = 0;
                if (paths[i].pos > endpos) {
                    endpos = paths[i].pos;
                }
            }
            cost = paths[i].cost;
            paths[i].cost += token->strategy;
            paths[i].numberOfTokens++;
            paths[i].lastToken = token;
        
            while (token->shorterToken != NULL) {
                /* means longest token is tok-ambiguous */
                if (number_of_paths == NTM_TOKEN_PATHS_MAXNB) {
                    warningHandler(ct, "Too many tokenization paths.");
                    break;
                }
                unique_endpos = 0;
                paths[number_of_paths].pos =
                    token->initpos + token->shorterToken->inputLength;
                paths[number_of_paths].bytepos =
                    token->initBytepos + token->shorterToken->inputByteLength;
                paths[number_of_paths].numberOfTokens =
                    paths[i].numberOfTokens;
                paths[number_of_paths].lastToken = token->shorterToken;
                paths[number_of_paths].cost =
                    cost + token->shorterToken->strategy;
                number_of_paths++;
                token = token->shorterToken;
            }
        }
    }
    if (number_of_paths > 0) {
        if (number_of_paths > 1) {
            /* several pathes, select the best ones */
            /* first, look for the best value for strategy */
            for(best=paths[0].cost, i=1; i < number_of_paths; i++) {
                if (paths[i].cost < best) best = paths[i].cost;
            }
            /* then remove pathes with higher value (cost) */
            for(i=0; i < number_of_paths; i++) {
                if (paths[i].cost > best)
                    removeSubpath(paths[i].lastToken, &first_token);
            }
        }
        ct->inputPos = endpos;
        ct->inputBytePos += first_token->inputByteLength;
        return first_token;
    } else {
        return NULL;
    }
}


