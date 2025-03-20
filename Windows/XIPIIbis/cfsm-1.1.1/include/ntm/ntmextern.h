/* $Id: ntmextern.h,v 1.6 2004/11/10 16:17:37 ait Exp $ */ 

#include "ntm.h"

/* FUNCTIONS
*************/

/* ntmmemory.c */

extern NormState * createNormState (
    NtmContext *ct,
    id_type id,
    ntmbit final,
    NormState *next,
    int nextnb);

extern NormElement * createNormElement (
    NtmContext *ct,
    int initpos,
    int inputlength,
    NormState *normstate);


extern id_type * createIdSeq(
    NtmContext *ct,
    int size);

extern NtmEntry * createEntry(
    NtmContext *ct,
    id_type *nform,
    id_type *lemma,
    id_type *features
    );

extern NtmToken * createToken(
    NtmContext *ct,
    int initpos,
    int inputlength,
    int ignore_flag);


/***/

extern void loadStrategyScript(NtmContext* ct, char *filename);
extern void initNtmMemory(NtmMemory *mem);
extern void loadStrategyScript(NtmContext* ct, char *filename);
extern int ntmPrintToken(NtmContext* ct, NtmToken* token, char *s, int maxlg);
extern void errorHandler(NtmContext *ct, const char *message, ...);
extern void warningHandler(NtmContext *ct, const char *message, ...);
extern int isSeparator(NtmContext* ct,int c);
extern int isSpace(NtmContext* ct,int c);
extern int checkLeftContext( Levels* lev, long leftpos,
                             long rightpos,
                             STATEptr fromstate,
                             NETptr net);
extern int checkRightContext( Levels *lev, long rightpos,
                              STATEptr fromstate,
                       NETptr net);
extern void tokenSetIgnore(NtmToken *token);
extern void tokenSetWeight(NtmToken *token, int w);
extern void tokenSetTambig(NtmToken *token);
extern void tokenSetStrategy(NtmToken *token, int i);
extern void resetMorpho(NtmContext *ct, NETptr lexinet);
extern void skipSpaces(NtmContext* ct);
extern void tokenSetStrategy(NtmToken *token, int i);
extern void resetTypo(NtmContext* ct, NETptr normnet);
extern NtmToken * getUnknownToken(NtmContext *ct);
extern void loadStrategyScriptFromString(NtmContext* ct, char *scriptstring);
