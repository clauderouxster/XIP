/* $Id: ntmtypes.h,v 1.7 2003/01/27 16:55:01 ait Exp $ */ 


#ifndef NTM_NTMTYPES_H
#define NTM_NTMTYPES_H


#include "types.h"
#include "limits.h"


/* General size paramaters
*****************************/

#define NTM_ID_SIZE 16384   /* size of the input alphabet, 16384 for the maximum nb of C-FSM symbols*/
#define NTM_SPACES_SIZE 256		   /* Max nb of characters considered as spaces */
#define NTM_SEPARATORS_SIZE 256		/* Max nb of characters considered as separators */

#define NTM_INPUTLEVEL_SIZE 10000
#define NTM_STRATEGY_MAXSIZE 20
#define NTM_LAST_STRATEGY 21
#define NTM_SYMBOL_MAXSIZE 50
#define NTM_FILENAME_SIZE 500
#define NTM_ALIAS_SIZE 50
#define NTM_NETS_MAXNB 50

#define TYPO_CONFIGS_MAXNB 5000
#define TYPO_CCONFIGS_MAXNB 1000
#define TYPO_TEMPOSOL_MAXNB 1000
#define TYPO_OUTPUTSTRING_MAXSIZE 1000

#define INPUTTOKEN_MAXSIZE 5000
#define MORPHO_CONFIGS_MAXNB 10001
#define MORPHO_CCONFIGS_MAXNB 10001
#define MORPHO_TEMPOSOL_MAXSIZE 1000
#define NTM_TOKEN_PATHS_MAXNB 50





/* Define some special symbols associated with word entries
   in the lexicon. Those symbols are not meant for output, but
   are interepreted in the tokenization and morphological process
*****************************************************************/

#define NTM_SPECIALSYMBOL_NB 20
#define NTM_COST_MAX 5
#define SYMBOL_IGNORE "+Ignore"
#define SYMBOL_WEIGHT0 "+w0"
#define SYMBOL_WEIGHT2 "+w2"

#define SYMBOL_TAMBIG "+Tambig" 
#define SYMBOL_MIN "+min"
#define SYMBOL_MAX "+max"
#define SYMBOL_UNKNOWN "+?"

#define SYMBOL_RC "RC" /* introduction keyword for right context */
#define SYMBOL_LC "LC" /* introduction keyword for left context */
#define SYMBOL_BOT "BOT" /* beginning of text */
#define SYMBOL_EOT "EOT" /* end of text */


#define ID_IGNORE specialIdVect[0]
#define ID_WEIGHT0 specialIdVect[1]
#define ID_WEIGHT2 specialIdVect[2]
#define ID_TAMBIG specialIdVect[3]
#define ID_MIN specialIdVect[4]
#define ID_MAX specialIdVect[5]
#define ID_UNKNOWN specialIdVect[6]

#define ID_RC specialIdVect[7]
#define ID_LC specialIdVect[8]
#define ID_BOT specialIdVect[9]
#define ID_EOT specialIdVect[10]

#define FIRST_SPECIAL_ID ID_IGNORE
#define LAST_SPECIAL_ID ID_EOT

#define IS_SPECIAL_SYMBOL(id) ( (id >= FIRST_SPECIAL_ID) && (id <= LAST_SPECIAL_ID) )
#define IS_EMPTY(id) ( (id==EPSILON) || (id==ALTCHAIN_SYMBOL) || (id==ID_LC) || (id==ID_RC) )

#define IS_NORMALCHAR(id) ( id > LAST_SPECIAL_ID )

#define IS_WEIGHT(id) ( (id >= ID_WEIGHT0) && (id <= ID_WEIGHT2) )
#define GET_WEIGHT(id) ( (id == ID_WEIGHT0) ? 0 : 2 )

#define IS_TEXTBOUND(id) ( (id == ID_BOT) || (id == ID_EOT) )

/* Global Types
******************/

typedef unsigned short ntmbit;
typedef int ntmbool;

typedef enum {BYCHAR, BYTOKEN} NtmUnknownStringMode;

typedef enum {LEFT, RIGHT, NONE} Side;


/* Data structure for the normalized input (as a net of paths) 
***************************************************************/

typedef struct NormState {
    id_type symbolId; 
    ntmbit isInitial : 1;
    ntmbit isFinal : 1;
    ntmbit nextNb : 14;
    struct NormState * firstNext;
} NormState;


typedef struct NormalizedElement {
    int initpos;
    short inputLength;
    NormState *normState;
} NormElement;


#define PATH_END(x) ( (x == NULL) || (x->nextNb == 0) )
#define IS_FINAL(x) ( (x != NULL) && (x->isFinal != 0) )
#define IS_INITIAL(x) ( (x != NULL) && (x->isInitial != 0) )
#define FIRST_NEXT_STATE(x) (x->firstNext)
#define LAST_NEXT_STATE(x) (x->firstNext + x->nextNb-1 )



/* configurations for managing current transitions in Typo norm,
****************************************************************/
typedef struct {
    int previous; /* indice of prev config */
    id_type outputId; /* the output symbol id leading to this */
    long cpos; /* current position in InputBuffer to read input symbols */
    int outputLength; /* length of this output path */
    STATEptr cstate; /* current state of this config */
    ntmbit isFinal; /* initially set to 0 */
    NormState* firstNext; /* initially set to 0 */
    int nextNb; /* initially set to 0 */
} TypoConfigStruct;

typedef struct {
    int configind;
    STATEptr cstate;
    Side side;
} TypoTempoSolStruct;

/* Data for typo norm */
typedef struct {
    TypoConfigStruct configs[TYPO_CONFIGS_MAXNB];
    int configsNb;
    int cconfigs1[TYPO_CCONFIGS_MAXNB];
    int cconfigs2[TYPO_CCONFIGS_MAXNB];
    int cconfigsNb;
    int newcconfigsNb;
    int *cconfigs;
    int *newcconfigs;
    TypoTempoSolStruct tempoSol[TYPO_TEMPOSOL_MAXNB];
    int tempoSolNb;
    int longestInputLength;
    int longestOutputLength;
    int finalSol[TYPO_TEMPOSOL_MAXNB];
    int finalSolNb;
} TypoDataStruct;


/* configurations for managing current transitions in morpho. an,
****************************************************************/

typedef struct {
    short previous; /* indice of prev config */
    id_type inputId; /* the input norm symbol id leading to this config */
    id_type outputId; /* the output symbol id leading to this */
    short inputLength; /* current length of input symbol id seq read */
    short lemmaLength;   /* current length of the out symbols gathered */
    short featuresLength;   /* current length of the out symbols gathered */
    ntmbit specialIds : 1; /* set iff encountred a feature/tag symbol */
    ntmbit ignore : 1; /* set iif token must be ignored */
    ntmbit tambig : 1; /* set iif token has a tokenization ambiguity */
    ntmbit weight : 2; /* weight of the entry */
    ntmbit normlength : 11; /* length of current norm. element */
    long cpos; /* current position in NormBuffer to read input symbols */
    NormState *normState; /* current pos in NormLevel to read input symbols */
    STATEptr cstate; /* current state of this config */
} MorphoConfigStruct;

typedef struct {
    int configInd;
    int length;
    int weight;
    int next;
    STATEptr cstate;
    Side side;
} MorphoTempoSolStruct;

typedef struct {
    MorphoConfigStruct configs[MORPHO_CONFIGS_MAXNB];
    int configsNb;
    int cconfigs1[MORPHO_CCONFIGS_MAXNB];
    int cconfigs2[MORPHO_CCONFIGS_MAXNB];
    int cconfigsNb;
    int newcconfigsNb;
    int* cconfigs;
    int* newcconfigs;
    MorphoTempoSolStruct tempoSol[MORPHO_TEMPOSOL_MAXSIZE];
    int tempoSolNb;
    int longestSolInd;
    int finalSol[MORPHO_TEMPOSOL_MAXSIZE];
    int finalSolNb;
} MorphoDataStruct;

/* Global Variables
********************/
extern id_type inputIdVect[NTM_ID_SIZE+1];
extern id_type specialIdVect[NTM_SPECIALSYMBOL_NB];


/* Functions
*************/



#endif /* NTM_NTMTYPES_H */
