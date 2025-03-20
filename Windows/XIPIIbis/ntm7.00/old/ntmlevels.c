/* $Id: ntmlevels.c,v 1.8 2004/05/12 10:03:39 jmcoursi Exp $ */ 


#include <string.h>

#include "ntmlevels.h"
#include "ntm.h"
#include "ntmstdinputobject.h"

inline id_type readInputCharId(Levels* lev, long pos);
#include "ntmstdinputobject.h"


/* Utility
***********/

inline long relPos(Levels* lev, long pos) {
    return (pos % lev->inputLevelSize);
}



/* Init levels
***************/

void initLevels(Levels* lev) {

	
	/* init the input level */
    lev->lastInputPos = -1;
    lev->inputEnd = 0;
    lev->inputStart = 0;
    lev->inputLevelSize = NTM_INPUTLEVEL_SIZE;
    lev -> closeInputObject = (int (*)(void *)) NULL;	
    lev -> readInputObject = (int (*)(void *, int *)) NULL;	
    lev-> getInputCodePointLen = (int (*) (int)) NULL;
    lev -> pInputObject = NULL;
    
    /* init the normalized level */
    lev->lastNormalizedPos = -1;
    lev->normalizedEnd = 0;
}


/* reset norm level
********************/

inline void resetNormLevel(Levels* lev, long pos) {
    lev->lastNormalizedPos = pos-1;
    lev->normalizedEnd = relPos(lev, pos);
}


/*====================*/
/* getInputId_NoCheck */
/*====================*/
/* Returns the ID of a character from its position.
   It assumes that the character has already been read.
*/
id_type getInputId_NoCheck(Levels *lev, long pos) {
    return lev->inputLevel[relPos(lev,pos)];
} /* getInputCharId_NoCheck */

inline int endOfInput(Levels* lev, long pos) {
    return (readInputCharId(lev, pos) == ID_EOT);
}

/*====================*/
/* getInputId_Check */
/*====================*/
/* Returns the ID of a character from its position.
   If the character has not already been read, it reads
   it by calling lev->readId)(
*/
int getInputId_Check(Levels* lev, long pos, id_type *pId) {

    if (pos > lev->lastInputPos) {
        /* The character requested has not been read yet, get it 
           via the input function for the level */

        if ( EOF == readId(lev, pId)) {
            /* End of File convention : the ID_EOT (end of text) */
            lev->inputLevel[lev->inputEnd] = ID_EOT;
        } else { 
            lev->inputLevel[lev->inputEnd] = *pId;
        }
        /* Move the pointers */
        lev->lastInputPos++;
        lev->inputEnd = relPos(lev, lev->inputEnd+1);
        if (pos > lev->inputLevelSize) {
            lev->inputStart = relPos(lev, lev->inputEnd+1);
        }
    } else {
        /* The character has been read : just get it from the buffer. */
        *pId = lev->inputLevel[relPos(lev,pos)];
    }

	if(ID_EOT == *pId) 
	{
		return ID_EOT;
	}
	else
	{
		return 0;
	}
}



inline id_type getBackInputCharId(Levels* lev, long pos) {
    if ( (pos < 0) && (lev->lastInputPos < lev->inputLevelSize) ) {
        if (pos == -1)
            return ID_BOT;
        else
            return ID_NO_SYMBOL;
    } else {
        if ( (pos > lev->inputLevelSize) &&
             (pos < (lev->lastInputPos - lev->inputLevelSize + 1) ) ) {    
            return ID_NO_SYMBOL;
        } else {
	  return lev->inputLevel[relPos(lev,pos)];
        }
    }
}

/* gets char Id at position pos of input level without any checking
****************************************************************/
inline id_type getInputCharId(Levels* lev, long pos) {
	return lev->inputLevel[relPos(lev,pos)];
}

/* same but checks if pos is valid and reads the char if necessary
*****************************************************************/
inline id_type readInputCharId(Levels* lev, long pos) {
	id_type id;
	getInputId_Check(lev, pos, &id);
	return id;
}


/* read one normalized element at position pos using net norm_net
******************************************************************/
extern NormElement *readNormElement2(NtmContext *ct, int initpos, NETptr net);

inline FsmNTMExport NormElement *readNormElement(NtmContext *ct, long pos, NETptr norm_net) {
    NormElement *normel=NULL; Levels *lev = &(ct->levels);
    int i=0;

    if (pos > lev->lastNormalizedPos) {
        if (readInputCharId(lev, pos) == ID_EOT) {
	  /* End of File convention :
	   */
	  return NULL;
        } else {
	  while (lev->lastNormalizedPos < pos) {
	    normel = readNormElement2(ct, lev->lastNormalizedPos+1,
				      norm_net );
	    if (normel==NULL) return NULL;
	    for(i=normel->inputLength; i > 0; i--) {
	      lev->normalizedLevel[lev->normalizedEnd] = normel;
	      lev->normalizedEnd = relPos(lev, lev->normalizedEnd+1);
	    }
	    lev->lastNormalizedPos += normel->inputLength;
	  }
	  return normel;
        }
    } else {
      return (lev->normalizedLevel[relPos(lev,pos)]);
    }
}

