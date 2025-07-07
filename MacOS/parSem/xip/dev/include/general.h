#ifndef general_h
#define general_h

#define OUI 1
#define NON 0
#define CHAMP 2
#define CONCEPT 1
#define RELATION 0
#define ERREUR -1
#define TOUS -1
#define GROUPE 3
#define EQUIVALENT 2
#define IMPLICATION 0
#define UTILISE 1
#include "noeud.h"

char lettre(unsigned char c);

#ifdef principal_v

int CHIFFRES[11];
#else
extern int CHIFFRES[11];
#endif

#endif