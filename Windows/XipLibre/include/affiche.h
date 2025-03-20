/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.60
   filename   : affiche.h
   Date       : 10/01/1999
   Purpose    : Node object that is used when displaying the text tree-like structure on screen
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef NCoord_h
#define NCoord_h

class GlobalParseur;
class genereGraphe;

class NCoord {
 public:
  
    string nom;
    float xNom;
    float yNom;
    float yHautCentre;
    float xCentreNom;
    float yBasCentre;
    unsigned int longueur;
  
    NCoord* fils;
    NCoord* suivant;

  
  
    NCoord(Noeud* N,float* X,float* Y,float y);
	NCoord(genereGraphe*,float* X,float* Y,float y);

    ~NCoord() {
        if (fils) 
            delete fils; 
        if (suivant) 
            delete suivant;
    }
    void Dessine(GlobalParseur*);
};

#endif

