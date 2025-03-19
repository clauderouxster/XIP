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
   filename   : lliste.h
   Date       : 10/09/1999
   Purpose    : Allocation of syntactic node (embbeding in a Liste object)
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef lliste_h
#define lliste_h

//-------------------------------------------------------------
//Cette classe est utilisee pour gerer un tas de listes...L'allocation n'a lieu qu'une fois...
//-------------------------------------------------------------

class TasListe : public VECTA<Liste*> {
 public:

    TasListe(size_t t) : VECTA<Liste*>(t) {}
    ~TasListe() {nettoie();}

    //Cette methode fournit un nouvel element initialisee de type Liste
    Liste* fournir(GlobalParseur*,Cat* X, Traits* tr);
    void Raz();
    void RecupereDernier(GlobalParseur* parseur);
};


//-----------------------------------------------------------------------------
//Cette classe est en fait un vecteur d'element de type Liste
//-----------------------------------------------------------------------------
class LListe : public VECTA<Liste*> {
 public:

    
    LListe(int t=25) : VECTA<Liste*>(t) {}

    ~LListe() {}


    void inverse();
    void Affiche(ostream&,size_t,size_t);
    
};

#endif
