/* $Id: ntmapp.c,v 1.12 2004/05/14 09:46:59 ait Exp $ */ 

#include <string.h>
#include <stdarg.h>

#include "struct.h"
#include "context.h"
#include "pars-tbl.h"
#include "types.h"

#include "ntm.h"
#include "ntmapp.h"
#include "modify.h" /* For init_save_functions and init_restore_functions */

#ifdef _WINDOWS
#  include <stdlib.h>
#endif


ApplicationParamaterStruct AppParams;

/* id_type SpecialIdVect[NTM_SPECIALSYMBOL_NB]; */


/* Displays online help on options
**********************************/
void displayHelp() {

    printf("\nNTM options:\n\n\
-t <name>\n\
       <name> is the filename of the normalizing transducer to use\n\
-l <name>\n\
       <name> is the filename of the lexical transducer to use\n\
-f <name>\n\
       <name> is the filename of the ntm script (which defines the\n\
       space and separator classes, and the lookup strategy)\n\
-i <name>\n\
       <name> is the filename of the input text to be processed\n\
       (if not specified, ntm reads from the standard input)\n\
-nsc\n\
       Do not apply the separator constraint by default (do not require\n\
       by default that any token must be delimited with separators)\n\
-unknownbychars\n\
       In case of unknown string, print only one char as an unknown\n\
       token and proceed ahead starting again from the next char.\n\
       When this option is not set (default), ntm returns a string\n\
       delimited with separators as an unknown token.\n\
-norm\n\
       Only output the result of the normalization (as defined by\n\
       the -t option, or by the first level of the strategy in the\n\
       script file of the -f option).\n\
-indices\n\
       Print out the indices (in the input text) of tokens\n\
-bytes\n\
       Print out the indices in terms of BYTES (in the input text) of tokens\n\
-xml\n\
       Print out in xml format\n\
-utf8\n\
       Expect UTF-8 input data\n\n"

    );
}



/* Initialisation of parameters
********************************/

void initParams(int argn, char **argv) {
    
    int i=0;
    enum {NONE, TEXTFILE, TYPOFILE, LEXIFILE, SCRIPTFILE} next_param; 

    /* defaults */
    AppParams.scriptFilename[0] = 0;
    AppParams.textFilename[0] = 0;
    AppParams.checkSepConstraint = TRUE;
    AppParams.unknownStringOutput = BYTOKEN;
    AppParams.vectorizeMin = 29;
    AppParams.outputType = MORPHOTOKEN;
    AppParams.printIndices = FALSE;
    AppParams.utf8Data = FALSE;
    /* COMMENTS ON THE OPTIONS:
       -t  use the following string as the filename of the
       normalizing transducer
       -l  use the following string as the filename of the
       lexical transducer
       -f  use the following string as the filename of the
       ntm script (contains specification on the strategy,
       seprators and spaces)
       -i  use the following string as the filename of the
       input text to be processed
       -nsc no separator constraint (do not require by default that
       must be delimited with separators)
       -unknownbychars in case of unknown string, print only one char
       as an unknown token and go ahead by one char
       -norm output of ntm is only that of the normalization
       -indices print out the indices (in the text) of tokens
       
    */

    next_param = NONE;
    for(i=1; i < argn; i++) {
        if (!strcmp(argv[i], "-t")) {
            next_param = TYPOFILE;
        } else if (!strcmp(argv[i], "-l")) {
            next_param = LEXIFILE;
        } else if (!strcmp(argv[i], "-f")) {
            next_param = SCRIPTFILE;
        } else if (!strcmp(argv[i], "-i")) {
            next_param = TEXTFILE;
        } else if (!strcmp(argv[i], "-nsc")) {
            AppParams.checkSepConstraint = FALSE;
        } else if (!strcmp(argv[i], "-unknownbychars")) {
            AppParams.unknownStringOutput = BYCHAR;
        }  else if (!strcmp(argv[i], "-norm")) {
            AppParams.outputType = NORM;
        } else if (!strcmp(argv[i], "-indices")) {
            AppParams.printIndices = TRUE;
        } else if (!strcmp(argv[i], "-bytes")) {
            AppParams.printByteIndices = TRUE;
        } else if (!strcmp(argv[i], "-xml")) {
            AppParams.xmlOutput = TRUE;
        } else if (!strcmp(argv[i], "-utf8")) {
            AppParams.utf8Data = TRUE;
        } else if (!strcmp(argv[i], "-h")) {
            displayHelp();
            exit(0);
        } else {
            if (next_param == TYPOFILE) {
                strcpy(AppParams.normFilename,
                       argv[i]);
            } else if (next_param == LEXIFILE) {
                strcpy(AppParams.lexFilename,
                       argv[i]);
            } else if (next_param == SCRIPTFILE) {
                strcpy(AppParams.scriptFilename, argv[i]);
            } else if (next_param == TEXTFILE) {
                strcpy(AppParams.textFilename, argv[i]);
            } else {
                fprintf(stderr, "NTM: Unknown option: '%s'\n\n", argv[i]);
                exit(1);
            }
            next_param = NONE;
        }
    }

    /* set the input stream */

    if (AppParams.textFilename[0] == 0) {
        AppParams.textStream = stdin;
    } else {
        /* open input text file */
        AppParams.textStream = fopen(AppParams.textFilename, "r");
        if (AppParams.textStream == NULL) {
            fprintf(stderr, "NTM: Unable to read text from '%s'\n\n",
                         AppParams.textFilename);
            exit(1);
        }
    }

    /* set the paramaters (lexicons, etc.) */
    if ( (AppParams.scriptFilename[0] == 0) &&
         (AppParams.lexFilename[0] == 0) ) {
        fprintf(stderr, "NTM: No lexicon/strategy was provided!\n");
        exit(1);
    } else if (AppParams.lexFilename[0] != 0) {
        if (AppParams.scriptFilename[0] != 0) {
            fprintf(stderr, "NTM: option -t or -l is incompatible with -f\n");
            exit(1);
        }
    }
}



/* main analysis function. call actual processing function depending
   on the typo nets. If no typo net was provided, process directly
   using lexicon, else, process using typo and lexicon
***********************************************************/

char sout[5002];



void ntm(NtmContext* ct) {

    long initpos = 0, strategy_ind=0;
    NtmToken *token=NULL, *t=NULL;
    char sout[5002];

    token = ntmReadTokens(ct);
    while ( !ntmError(ct) && !ntmIsEofToken(token) ) {
        if (ntmWarning(ct)) {
            fprintf(stderr, "\n####%s\n", ntmMessage(ct));
            ntmResetWarning(ct);
            /* exit(1); */
        }
        if (!token->ignore) {
            ntmPrintTokens(ct, token, sout, 5000);
            fputs(sout, stdout);
        }
        token = ntmReadTokens(ct);
    }
    if (ntmError(ct)) {
        fprintf(stderr, "\n%s\n\n", ntmMessage(ct));
    }
}



/* Performs typo normalization and prints out the normalized items
*******************************************************************/
extern NormElement *readNormElement(NtmContext* ct, long initpos, NETptr net);

void normalize(NtmContext* ct) {

    long initpos=0, i=0, pos=0;
    NormElement *normres=NULL;
    char sout[5000];

    resetNtmMemory(&(ct->memory)); 
    normres = readNormElement(ct, initpos, ct->strategy[0].normnet);
    while (normres != NULL) {
        snprintNorm(&(ct->levels), sout, 5000, normres);
        fputs(sout, stdout);
        initpos += normres->inputLength;
        resetNtmMemory(&(ct->memory)); 
       normres = readNormElement(ct, initpos, ct->strategy[0].normnet);
    }
}


/* Main function
*****************/
int main(int argn, char **argv) {
    int i=0;
    NtmContext ct;
    set_default_context();
    init_fsm_memory(get_default_context());
    init_restore_functions();
    init_save_functions();

    ntmCopyright();

    initParams(argn, argv);

    ntmInit(&ct);

	if (TRUE == AppParams.utf8Data)
	{
		ntmSetInputUTF8Stream(&ct, AppParams.textStream);
		init_string_functions(CHAR_ENC_UTF_8);
	}
	else
	{
		ntmSetInputTextStream(&ct, AppParams.textStream);
		init_string_functions(CHAR_ENC_ISO_8859_1);
	}

/*
	ntmSetInputTextString(&ct,"UTF-8 accents : Même, chanté, grèle, où, hôtel, aïgüe.");
*/


    ntmSetUnknownStringMode(&ct, AppParams.unknownStringOutput);
    ntmSetSeparatorConstraint(&ct, AppParams.checkSepConstraint);
    ntmSetPrintIndices(&ct, AppParams.printIndices);
    ntmSetPrintByteIndices(&ct, AppParams.printByteIndices);
    ntmSetXmlOutput(&ct, AppParams.xmlOutput);
    ntmSetVectorizeNumber(&ct, 25);

    if (AppParams.scriptFilename[0] != 0) {
        ntmLoadScript(&ct, AppParams.scriptFilename);
    } else {
        ntmLoadStrategy(&ct, AppParams.normFilename, AppParams.lexFilename);
    }
    if (ntmError(&ct)) {
        fprintf(stderr, "\n%s\n\n", ntmMessage(&ct));
        exit(1);
    }        

    if (AppParams.outputType == NORM) {
        normalize(&ct);
    } else {
        ntm(&ct);
    }
	ntmClearInputObject(&ct);
	if (AppParams.textStream) fclose(AppParams.textStream);
	unset_default_context();
    release_fsm_memory();


    return 0;
}
