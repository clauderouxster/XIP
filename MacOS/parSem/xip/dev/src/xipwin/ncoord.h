#ifndef NCoord_h
#define NCoord_h


#include "xipstlres.h"

class NCoord {
public:
  
  CString nom;
  long xNom;
  long yNom;
  long yHautCentre;
  long xCentreNom;
  long yBasCentre;
  long longueur;
  
  NCoord* fils;
  NCoord* suivant;
   
  NCoord(CDC* pDC,XipNode* N,long* X,long* Y,long y,XipNode*,int pos);

  ~NCoord() {
      if (suivant!=NULL)
          delete suivant;
    if (fils!=NULL)
        delete fils;
  }
    
};

#endif;
