#ifndef NTM_LEVELS_H
#define NTM_LEVELS_H

#include <stdio.h>

#include "ntmtypes.h"

typedef struct Levels {
    FILE *inputStream;
    char *inputString;
    long inputLevelSize;
    long lastInputPos;
    long inputEnd;
    long inputStart;
/* the contents of the input level */ 
    unsigned char inputLevel[NTM_INPUTLEVEL_SIZE];
/* indice of last available element in normalized level */
    long lastNormalizedPos;
    long normalizedEnd;
/* the contents of the normalized level */ 
    NormElement* normalizedLevel[NTM_INPUTLEVEL_SIZE];
} Levels;



/* Functions */



extern void initLevels(Levels *lev, FILE *in, char* s);
extern void resetNormLevel(Levels* levels, long pos);

extern int endOfInput(Levels* levels, long pos);

extern unsigned char getInputChar(Levels* levels, long pos);
extern unsigned char readInputChar(Levels* levels, long pos);
extern id_type getInputCharId(Levels* levels, long pos);
extern id_type readInputCharId(Levels* levels, long pos);
extern id_type getBackInputCharId(Levels* levels, long pos);


#endif /* NTM_LEVELS_H */
