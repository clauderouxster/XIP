#ifndef NTM_NTM_H
#define NTM_NTM_H

#include "ntmtypes.h"
#include "levels.h"
#include "token.h"
#include "ntmmemory.h"

/* DATA structures 
********************/

/* Struct for the nets to be loaded and prepared */
typedef struct {
    NETptr net;
    char filename[NTM_FILENAME_SIZE];
    char alias[NTM_ALIAS_SIZE];
} NtmNetStruct;


/* CONTEXT */


typedef struct {
    /* Paramaters */
    struct {NETptr normnet; NETptr lexnet;} strategy[NTM_STRATEGY_MAXSIZE];
    int strategyNb;
    int strategyNbMax;
    long inputPos;
    int vectorizeNumber;
    NtmUnknownStringMode unknownStringMode;
    ntmbool checkSeparatorConstraint;
    ntmbool deterministicOutput;
    ntmbool printIndices;
    unsigned short spaces[NTM_INPUTALPHABET_SIZE];
    unsigned short separators[NTM_INPUTALPHABET_SIZE];
    /* Private (internal) data */
    Levels levels;
    TypoDataStruct typoData;
    MorphoDataStruct morphoData;
    NtmMemory memory;
    /* Errors */
    int error; /* = 0 when NO ERROR */
    int warning; /* = 0 when NO Warning */
    char message[500];
} NtmContext;



/* API functions
*****************/

extern void ntmInit(NtmContext* ct);


/* set context paramaters */
extern void ntmUndefineSpaces(NtmContext* c);
extern void ntmUndefineSeparators(NtmContext* c);
extern void ntmSetSpace(NtmContext* c, int code);
extern void ntmSetSeparator(NtmContext* c, int code);
extern void ntmSetDefaultSepSpaces(NtmContext* c);

extern void ntmSetVectorizeNumber(NtmContext* c, int n);
extern void ntmSetUnknownStringMode(NtmContext* c, NtmUnknownStringMode m);
extern void ntmSetSeparatorConstraint(NtmContext* c, ntmbool value);
extern void ntmSetDeterministicOutput(NtmContext* c, ntmbool value);
extern void ntmSetPrintIndices(NtmContext* c, ntmbool value);

extern void ntmResetStrategies(NtmContext* ct);
extern void ntmLoadAndPrepareNets(NtmContext *ct, NtmNetStruct *nets, int netsNb);
extern void ntmAddStrategy(NtmContext* c, NETptr normnet, NETptr lexnet);
extern void ntmLoadStrategy(NtmContext* c, char* normfile, char*lexfile);
extern void ntmLoadScript(NtmContext* ct, char *filename);

extern void ntmSetInputTextStream(NtmContext* ct, FILE *stream);
extern void ntmSetInputTextString(NtmContext* ct, char* s);

extern int ntmError(NtmContext* ct);
extern int ntmWarning(NtmContext* ct);
extern void ntmResetWarning(NtmContext* ct);
extern char *ntmMessage(NtmContext* ct);
extern void ntmCopyright();

/* the main function */
extern Token* ntmReadTokens(NtmContext* ct);

extern int ntmIsEofToken(Token* t);
extern int ntmIsUnknownToken(Token* t);

/* prints token t into string s */
extern int ntmPrintTokens(NtmContext* ct, Token* token, char *s, int maxlg);
extern int ntmSnprintIdSeq(char *s, int maxlg, id_type *idseq);

#endif /* NTM_NTM_H */
