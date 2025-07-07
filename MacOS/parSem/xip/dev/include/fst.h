#ifndef Connexion_fst
#define Connexion_fst

#ifdef FST_DLL
#include "resultat.h"

extern "C" {
int InitSession();
int Execute(char* texte, char* langin,char* langout,ExportResult**,int);
int Desambiguise(char* texte, char* langin,ExportResult** re);
void CloseSession();
void Clear(ExportResult*);
}


#endif


#endif
