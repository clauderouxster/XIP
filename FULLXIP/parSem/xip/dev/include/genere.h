#ifndef genere_h
#define genere_h

#include <malloc.h>


class generation
{
public:

 char* concept;
 char* relation;
 char* valeur;
 char test;
 generation* suivant;
 generation* dernier;
 generation* fils;
 generation* Dfils;
 generation* Dfrere;
 generation* frere;

 generation() 
   {concept=NULL;relation=NULL;valeur=NULL;
    fils=NULL;frere=NULL;Dfils=this;Dfrere=this;
	suivant=NULL;dernier=this;test=0;
   }

 ~generation()
  {if (concept!=NULL) free(concept);
   if (relation!=NULL) free(relation);
   if (valeur!=NULL) free(valeur);
   if (suivant!=NULL) delete suivant;
   if (fils!=NULL) delete fils;
   if (frere!=NULL) delete frere;
  }
 
 void Fils(generation* g)
  {
   Dfils->fils=g;
   Dfils=g;
  }

 void Frere(generation* g)
  {
   Dfrere->frere=g;
   Dfrere=g;
  }
 
 void Suivant(generation* g)
  {
   dernier->suivant=g;
   dernier=g;
  }
};


#endif
