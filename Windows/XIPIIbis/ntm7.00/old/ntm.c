/* $Id: ntm.c,v 1.15 2004/10/13 13:42:52 ait Exp $ */ 


#include <stdarg.h>

#include "ntm.h"
#include "ntmextern.h"
#include "version.h"
#include <string.h>

#ifdef _WINDOWS
#endif
#include <assert.h>
/* id_type relates C-FSM IDs and the UTF-16 encoding of a single character. */
id_type inputIdVect[NTM_ID_SIZE+1];

id_type specialIdVect[NTM_SPECIALSYMBOL_NB];


/* Handles errors and warnings
*******************/
void errorHandler(NtmContext *ct, const char *message, ...) {
    int ind = 0;
    va_list ap;
    ct->error = 1;
    ind += sprintf(ct->message, "NTM: Error at pos %d: ", ct->inputPos);
    va_start(ap, message);
    vsprintf(ct->message+ind, message, ap);
}

void warningHandler(NtmContext *ct, const char *message, ...) {
    int ind = 0;
    va_list ap;
    ct->warning = 1;
    ind += sprintf(ct->message, "NTM: Warning: Result may be incorrect at position %d. ", ct->inputPos);
    va_start(ap, message);
    vsprintf(ct->message+ind, message, ap);
}

void ntmCopyright() {
  fprintf(stderr, "Copyright © Xerox Corporation 2000-2001\n\
NTM (Normalization, Tokenization and Morphological analysis), version %s\n\
Created by Salah Aït-Mokhtar, Xerox Research Centre Europe\n\n",
NTM_VERSION_STRING);
  
}

void ntmCopyrightString(char* version) {
  sprintf(version, "Copyright © Xerox Corporation 2000-2001\n\
NTM (Normalization, Tokenization and Morphological analysis), version %s\n\
Created by Salah Aït-Mokhtar, Xerox Research Centre Europe\n\n",
NTM_VERSION_STRING);
  
}

/* Init of the Ids Vector for input chars
*****************************************/

inline void ntmInitSymbolIds() {

    /* the Id vector of the input level */
    /* get Ids for particular symbols */
    intern_literal(&ID_IGNORE, SYMBOL_IGNORE );
    intern_literal(&ID_WEIGHT0, SYMBOL_WEIGHT0 );
    intern_literal(&ID_WEIGHT2, SYMBOL_WEIGHT2 );
    intern_literal(&ID_TAMBIG, SYMBOL_TAMBIG );
    intern_literal(&ID_MIN, SYMBOL_MIN );
    intern_literal(&ID_MAX, SYMBOL_MAX );
    intern_literal(&ID_UNKNOWN, SYMBOL_UNKNOWN );
    intern_literal(&ID_RC, SYMBOL_RC );
    intern_literal(&ID_LC, SYMBOL_LC );
    intern_literal(&ID_BOT, SYMBOL_BOT );
    intern_literal(&ID_EOT, SYMBOL_EOT );

}

inline void ntmSetDefaultSepSpaces(NtmContext* ct) {
    ntmUndefineSpaces(ct);
    ntmUndefineSeparators(ct);
    ntmSetSpace(ct,0);
    ntmSetSpace(ct,9);
    ntmSetSpace(ct,10);
    ntmSetSpace(ct,13);
    ntmSetSpace(ct,32);
    ntmSetSeparator(ct,',');
    ntmSetSeparator(ct,';');
    ntmSetSeparator(ct,'.');
    ntmSetSeparator(ct,':');
    ntmSetSeparator(ct,'!');
    ntmSetSeparator(ct,'?');
    ntmSetSeparator(ct,'-');
    ntmSetSeparator(ct,'_');
    ntmSetSeparator(ct,'"');
    ntmSetSeparator(ct,'\'');
    ntmSetSeparator(ct,'`');
    ntmSetSeparator(ct,'(');
    ntmSetSeparator(ct,')');
    ntmSetSeparator(ct,'[');
    ntmSetSeparator(ct,']');
    ntmSetSeparator(ct,'{');
    ntmSetSeparator(ct,'}');
    ntmSetSeparator(ct,'^');
    ntmSetSeparator(ct,'=');
    ntmSetSeparator(ct,'/');
    ntmSetSeparator(ct,'\\');
    ntmSetSeparator(ct,'|');
    ntmSetSeparator(ct,'*');
    ntmSetSeparator(ct,'+');
    ntmSetSeparator(ct,'%');
    ntmSetSeparator(ct,'$');
    ntmSetSeparator(ct,'#');
    ntmSetSeparator(ct,'<');
    ntmSetSeparator(ct,'>');
    ntmSetSeparator(ct,'~');
    ntmSetSeparator(ct,'&');
}

/* reset & initialization
******************/
inline void resetTypo(NtmContext* ct, NETptr normnet) {
    if (normnet != NULL) {
        ct->typoData.configs[0].cstate =
            NET_start_state(normnet);
    }
    ct->typoData.longestInputLength = 0;
    ct->typoData.longestOutputLength = 0;
    ct->typoData.configsNb = 1;
    ct->typoData.configs[0].isFinal = 0;
    ct->typoData.configs[0].firstNext = NULL;
    ct->typoData.configs[0].nextNb = 0;
    ct->typoData.cconfigs[0] = 0;
    ct->typoData.cconfigsNb = 1;
    ct->typoData.tempoSolNb = 0;
}

inline void resetMorpho(NtmContext *ct, NETptr lexinet) {
    resetNormLevel(&(ct->levels), ct->inputPos);

    ct->morphoData.configs[0].cstate =  NET_start_state(lexinet);
    ct->morphoData.cconfigs = ct->morphoData.cconfigs1;
    ct->morphoData.newcconfigs = ct->morphoData.cconfigs2;
    ct->morphoData.longestSolInd = -1;
    ct->morphoData.tempoSolNb = 0;
    ct->morphoData.configsNb = 1;
    ct->morphoData.configs[0].cpos = ct->inputPos;
    ct->morphoData.cconfigs[0] = 0;
    ct->morphoData.cconfigsNb = 1;   
}


void ntmResetWarning(NtmContext* ct) {
    ct->warning = 0;
}

void ntmReset(NtmContext* ct) {
    resetNtmMemory(&(ct->memory)); 
    ct->error = ct->warning = 0;
    ct->message[0] = 0;
    ct->inputPos = 0;
    ct->inputBytePos = 0;

    initLevels(&ct->levels);
}


/* test spaces/seprators
*************************/

inline int isSpace(NtmContext* ct,int c) {return (unsigned int) ct->spaces[(unsigned) c];}
inline int isSeparator(NtmContext* ct,int c) {
    return (unsigned int) ct->separators[(unsigned) c];}

/******************************************************************
 Utility: skips spaces from initpos forward, and then return the new
 position pointing on the first non space
******************************************************************/
inline void skipSpaces(NtmContext* ct) {
    int c = getInputCharId(&(ct->levels), ct->inputPos);
    while ( ( c != ID_EOT) && isSpace(ct,c) ) {
        ct->inputPos++;
        ct->inputBytePos += ct->levels.inputIdLen[c];
        c = readInputCharId(&(ct->levels), ct->inputPos);
    }
}


/*******************************************************************
 *
 * THE API FUNCTIONS
 *
 *******************************************************************/


/* Init ntm context
*******************/

void ntmInit(NtmContext* ct) {

    initNtmMemory(&(ct->memory));
    ct->error = ct->warning = 0;
    ct->message[0] = 0;
    ct->strategyNbMax = NTM_STRATEGY_MAXSIZE;
    ct->strategyNb = 0;
    ct->inputPos = 0;
    ct->inputBytePos = 0;

    /* intern symbol ids */
    ntmInitSymbolIds();
    /* init some default input char classes */
    ntmSetDefaultSepSpaces(ct);
    /**** initLevels() ***/

	    initLevels(&ct->levels);

    /***** initTypo() instructions *****/
    ct->typoData.configs[0].previous = -1;
    ct->typoData.configs[0].outputId = EPSILON;
    ct->typoData.configs[0].outputLength = 0;
    ct->typoData.cconfigs = ct->typoData.cconfigs1;
    ct->typoData.newcconfigs = ct->typoData.cconfigs2;
    ct->typoData.tempoSolNb = 0;
    ct->typoData.longestInputLength = ct->typoData.longestOutputLength = 0;
    ct->typoData.finalSolNb = 0;
    /* initMorpho() instructions */
    ct->morphoData.configs[0].previous = -1;
    ct->morphoData.configs[0].normState = NULL;
    ct->morphoData.configs[0].inputId = EPSILON;
    ct->morphoData.configs[0].outputId = EPSILON;
    ct->morphoData.configs[0].inputLength = 0;
    ct->morphoData.configs[0].lemmaLength =
        ct->morphoData.configs[0].featuresLength = 0;
    ct->morphoData.configs[0].specialIds = 0;
    ct->morphoData.configs[0].ignore = 0;
    ct->morphoData.configs[0].tambig = 0; 
    ct->morphoData.configs[0].weight = 1; /* default weight */

    /* default values for parameters */
    ct->vectorizeNumber = 19;
    ct->unknownStringMode = BYTOKEN;
    ct->checkSeparatorConstraint = TRUE;
    ct->deterministicOutput = FALSE;
    ct->printIndices = FALSE;
    ct->printByteIndices = FALSE;
    ct->xmlOutput = FALSE;
}


/* the set of spaces is set to {} (empty)
****************************************/
inline void ntmUndefineSpaces(NtmContext* ct) {
    memset(ct->spaces, 0, NTM_SPACES_SIZE);
    ct->spaces[ID_EOT] = 1;
    ct->separators[ID_EOT] = 1;
    ntmSetSpace(ct, 0);
}

/* the set of separators is set to {} (empty)
********************************************/
inline void ntmUndefineSeparators(NtmContext* ct) {
    memset(ct->separators, 0, NTM_SEPARATORS_SIZE);
    ct->separators[ID_EOT] = 1;
    ntmSetSeparator(ct, 0);
}

inline void ntmSetSpace(NtmContext* ct, int char_code) { 
    id_type id;
    intern_char(&id, char_code );
    ct->spaces[id] = 1;
    ct->separators[id] = 1;	/* Spaces are separators */
}

inline void ntmSetSeparator(NtmContext* ct, int char_code) { 
    id_type id;
    intern_char(&id, char_code );
    ct->separators[id] = 1;
}

inline void ntmSetVectorizeNumber(NtmContext* ct, int n) {
    ct->vectorizeNumber = n;
}

inline void ntmSetUnknownStringMode(NtmContext* ct, NtmUnknownStringMode m) {
    ct->unknownStringMode = m;
}

inline void ntmSetSeparatorConstraint(NtmContext* ct, ntmbool value) {
    ct->checkSeparatorConstraint = value;
}

inline void ntmSetDeterministicOutput(NtmContext* ct, ntmbool value) {
    ct->deterministicOutput = value;
}

inline void ntmSetPrintIndices(NtmContext* ct, ntmbool value) {
    ct->printIndices = value;
}

inline void ntmSetPrintByteIndices(NtmContext* ct, ntmbool value) {
    ct->printByteIndices = value;
}

inline void ntmSetXmlOutput(NtmContext* ct, ntmbool value) {
    ct->xmlOutput = value;
}


/* adds <normnet,lexnet> as a strategy
***************************************/
inline void ntmAddStrategy(NtmContext* ct,
                           NETptr normnet, NETptr lexnet) {
    if (ct->strategyNb == 0) {
        /* first setting */
        ct->strategyNbMax = NTM_STRATEGY_MAXSIZE;
    }
    if (ct->strategyNb >= ct->strategyNbMax) {
        /* incorrect setting of strategy: too many strategy levels */
        errorHandler(ct, "incorrect setting of strategy: too many strategy levels (should be < %d)", ct->strategyNbMax);
    } else {
        ct->strategy[ct->strategyNb].normnet = normnet;
        ct->strategy[ct->strategyNb++].lexnet = lexnet;
    }
}

inline void ntmResetStrategies(NtmContext* ct) {
    ct->strategyNb = 0;
}


/* reads the classes of spaces and separators and the cascade of
   strategies from a script file and uses them to initialize ct 
****************************************************************/

inline void ntmLoadScript(NtmContext* ct, char *filename) {
    /* first, reset ct data */
    ntmResetStrategies(ct);
    loadStrategyScript(ct, filename);
}


/* check if an error has occurred in an NTM context
****************************************************/

inline int ntmError(NtmContext* ct) {
    return ct->error;
}

inline int ntmWarning(NtmContext* ct) {
    return ct->warning;
}


/* returns the ntm message associated with the last error or warning
*********************************************************************/
inline char *ntmMessage(NtmContext* ct) {
    return ct->message;
}

/* print token(s) into a char buffer
*************************************/

int ntmPrintTokens(NtmContext* ct, NtmToken* token, char *s, int maxlg) {
    int ind=0;
    ind = ntmPrintToken(ct, token, s, maxlg);
    if (token->nextToken != NULL)
        ind += ntmPrintTokens(ct, token->nextToken, s+ind, maxlg-ind);
    if (token->shorterToken != NULL)
        ind += ntmPrintTokens(ct, token->shorterToken, s+ind, maxlg-ind);
    return ind;
}


/**************************************
 FOR DEBUGGING ONLY
**************************************/

void idc(id_type id) {
    char s[NTM_SYMBOL_MAXSIZE];
    FST_fat_to_thin( id_to_atom_name(id), s);
    printf("\n%d=>|%s|\n", id, s);
}

