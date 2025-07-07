/* $Id: ntmlevels.h,v 1.3 2007/02/16 16:51:52 ait Exp $ */ 


#ifndef NTM_LEVELS_H
#define NTM_LEVELS_H

#include <stdio.h>

#include "ntmtypes.h"

typedef struct Levels {

	int (* closeInputObject)(void *pInputObject);	
	int (* readInputObject)(void *pInputObject, int *p);	
	int (* getInputCodePointLen)(int ic);
	void * pInputObject;
    /* =================================== */

    long inputLevelSize;
    long lastInputPos;
    long inputEnd;
    long inputStart;
/* the contents of the input level */ 
    id_type inputLevel[NTM_INPUTLEVEL_SIZE];

/* Length, in bytes, of the UTF-8 string representing each Id */
	int inputIdLen[NTM_ID_SIZE+1];
	id_type inputIdVect[NTM_ID_SIZE+1];

/* indice of last available element in normalized level */
    long lastNormalizedPos;
    long normalizedEnd;
/* the contents of the normalized level */ 
    NormElement* normalizedLevel[NTM_INPUTLEVEL_SIZE];
} Levels;



/* Functions */



/* extern void initLevels(Levels *lev, FILE *in, char* s, id_type * pId); */

extern void initLevels(Levels *lev);

extern void resetNormLevel(Levels* levels, long pos);

extern id_type getInputCharId(Levels* levels, long pos);

extern id_type getBackInputCharId(Levels* levels, long pos);


#endif /* NTM_LEVELS_H */

