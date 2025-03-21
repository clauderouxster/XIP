/* $Id: ntmapp.h,v 1.1 2005/02/11 17:40:00 grondeau Exp $ */ 


#ifndef NTM_NTMAPP
#define NTM_NTMAPP



#include "types.h"
#include "ntm.h"

/* Defines
***********/


#define APPLICATION "NTM"
#define FILENAME_SIZE 100


/* Global Types
******************/


typedef enum {NORM, MORPHOTOKEN} NtmOutputType;


typedef struct ApplicationParamaterStruct {
    /* complete name of the script file, if any */
    char scriptFilename[FILENAME_SIZE];
    /* complete name of the text file to process, if any */
    char textFilename[FILENAME_SIZE];
    /* stream of the text to process, defaults to stdin */
    FILE *textStream;

    char normFilename[FILENAME_SIZE];
    char lexFilename[FILENAME_SIZE];

    /* min number of outgoing arcs for dense state that must be
       vectorized, defaults to  ?? */
    int vectorizeMin;

    /* type of output: normalization, tokenization, or morphological
       analysis, defaults to morpho. ana. */
    NtmOutputType outputType;


    /* if noTok, ntm does not perform tokenization, but only normalization and
       morphological analysis. It assumes the input file is already tokenized,
       each line being a (single) token (without the end-of-line char).
    */
    ntmbool noTok;

    /* if this is set, check separator constraints before validating
       the analysis of tokens, defaults to TRUE */
    ntmbool checkSepConstraint;

    /* determine wether unknown strings should be printed as tokens
       delimited with separators, or as independent characters. Defaults
       to BYTOKEN, but BYCHAR is necessary for asian languages where
       there is no separators */
    NtmUnknownStringMode unknownStringOutput;

    /* if this is set, print out the indices of tokens in input text,
       defaults to FALSE */
    ntmbool printIndices;

    /* same as previous, but the numbers concern BYTES and not characters */
    ntmbool printByteIndices;

    /* if this is set, print out an XML-based output,
       defaults to FALSE */
    ntmbool xmlOutput;

	/* if this is set, the input data is UTF-8
	   defaults to FALSE */
	ntmbool utf8Data;
} ApplicationParamaterStruct;


/* Functions
*************/

/* c-fsm functions */
extern int std_file_header_p(FILE* filestream);
extern NVptr networks_from_file(FILE* filestream);
extern ALPHABETptr lower_sigma_tg(NETptr net);


#endif /* NTM_NTMAPP */
