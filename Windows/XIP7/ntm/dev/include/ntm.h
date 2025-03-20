/* $Id: ntm.h,v 1.2 2005/04/14 13:00:25 ait Exp $ */ 


#ifndef NTM_NTM_H
#define NTM_NTM_H

#ifdef _WINDOWS
#include "ntm_windefs.h"
#endif

#include "ntmtypes.h"
#include "ntmlevels.h"
#include "ntmtoken.h"
#include "ntmmemory.h"

/* DATA structures 
********************/

/* Struct for the nets to be loaded and prepared */
typedef FsmNTMExport struct {
    NETptr net;
    char filename[NTM_FILENAME_SIZE];
    char alias[NTM_ALIAS_SIZE];
} NtmNetStruct;


/* CONTEXT */


typedef FsmNTMExport struct {
    /* Parameters */
    struct {NETptr normnet; NETptr lexnet;} strategy[NTM_STRATEGY_MAXSIZE];
    int strategyNb;
    int strategyNbMax;
    long inputPos;
    long inputBytePos;
    int vectorizeNumber;
    NtmUnknownStringMode unknownStringMode;
    ntmbool checkSeparatorConstraint;
    ntmbool deterministicOutput;
    ntmbool printIndices;
    ntmbool printByteIndices;
    ntmbool xmlOutput;
    unsigned char spaces[NTM_SPACES_SIZE+1];
    unsigned char separators[NTM_SEPARATORS_SIZE+1];

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

extern void FsmNTMExport ntmCfsmInit();
extern void FsmNTMExport ntmCfsmRelease();
extern void FsmNTMExport ntmInit(NtmContext* ct);
extern void FsmNTMExport resetNtmMemory(NtmMemory * mem);


/* set context parameters */
extern void FsmNTMExport ntmUndefineSpaces(NtmContext* c);
extern void FsmNTMExport ntmUndefineSeparators(NtmContext* c);
extern void FsmNTMExport ntmSetSpace(NtmContext* c, int code);
extern void FsmNTMExport ntmSetSeparator(NtmContext* c, int code);
extern void FsmNTMExport ntmSetDefaultSepSpaces(NtmContext* c);

extern void FsmNTMExport ntmSetVectorizeNumber(NtmContext* c, int n);
extern void FsmNTMExport ntmSetUnknownStringMode(NtmContext* c, NtmUnknownStringMode m);
extern void FsmNTMExport ntmSetSeparatorConstraint(NtmContext* c, ntmbool value);
extern void FsmNTMExport ntmSetDeterministicOutput(NtmContext* c, ntmbool value);
extern void FsmNTMExport ntmSetPrintIndices(NtmContext* c, ntmbool value);
extern void FsmNTMExport ntmSetPrintByteIndices(NtmContext* c, ntmbool value);
extern void FsmNTMExport ntmSetXmlOutput(NtmContext* c, ntmbool value);

extern void FsmNTMExport ntmResetStrategies(NtmContext* ct);
extern void FsmNTMExport ntmLoadAndPrepareNets(NtmContext *ct, NtmNetStruct *nets, int netsNb);
extern void FsmNTMExport ntmAddStrategy(NtmContext* c, NETptr normnet, NETptr lexnet);
extern void FsmNTMExport ntmLoadStrategy(NtmContext* c, char* normfile, char*lexfile);
extern void FsmNTMExport ntmLoadScript(NtmContext* ct, char *filename);
extern void FsmNTMExport ntmReplaceNet(NtmContext* ct, NtmNetStruct *nets, int netnum, char *filename);
extern void FsmNTMExport ntmReloadNet(NtmContext* ct, NtmNetStruct *nets, int netnum);
extern void FsmNTMExport ntmLoadNewNetLast(NtmContext* ct, NtmNetStruct *nets, int netsNb);
extern FsmNTMExport NtmNetStruct *ntmLoadNewNet(NtmContext* ct, NtmNetStruct *nets, int currentSize, char *filename);

extern void FsmNTMExport ntmSetInputTextStream(NtmContext* ct, FILE *stream);
extern void FsmNTMExport ntmSetInputTextString(NtmContext* ct, unsigned char* s);
extern void FsmNTMExport ntmSetInputUTF8Stream(NtmContext* ct, FILE* stream);
extern void FsmNTMExport ntmSetInputUTF8String(NtmContext* ct, unsigned char* s);
extern void FsmNTMExport ntmClearInputObject(NtmContext* ct);

extern void FsmNTMExport ntmSetInputIdString(NtmContext* ct, id_type * s);

extern int FsmNTMExport ntmError(NtmContext* ct);
extern int FsmNTMExport ntmWarning(NtmContext* ct);
extern void FsmNTMExport ntmResetWarning(NtmContext* ct);
extern FsmNTMExport char *ntmMessage(NtmContext* ct);
extern void FsmNTMExport ntmCopyright();
extern FsmNTMExport char *ntmWhoami();

/* the main function */
extern FsmNTMExport  NtmToken* ntmReadTokens(NtmContext* ct);

extern FsmNTMExport NtmToken* ntmAnalyzeThisToken(NtmContext* ct, unsigned char* s);
extern FsmNTMExport NtmToken* ntmAnalyzeThisTokenUTF8(NtmContext* ct, unsigned char* s);

extern int FsmNTMExport ntmIsEofToken(NtmToken* t);
extern int FsmNTMExport ntmIsUnknownToken(NtmToken* t);

/* prints token t into string s */
extern int FsmNTMExport ntmPrintTokens(NtmContext* ct, NtmToken* token, char *s, int maxlg);
extern int FsmNTMExport ntmSnprintIdSeq(char *s, int maxlg, id_type *idseq);
extern void FsmNTMExport snprintNorm(Levels *lev, char *s, int max, NormElement *normres);
extern FsmNTMExport int snprintInputForm(Levels* lev, char *s, int maxlg, NtmToken* token);
extern FsmNTMExport void loadStrategyScriptFromString(NtmContext* ct, char *scriptstring);
#endif /* NTM_NTM_H */
