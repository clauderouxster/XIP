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
   filename   : lliste.cxx
   Date       : 10/01/1999
   Purpose    : Handling of syntactic node allocation
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"

//Cette methode fournit un nouvel element initialisee de type Liste
Liste* TasListe::fournir(GlobalParseur* parseur,Cat* X, Traits* tr) {
    if (dernier >= taille)
        ajuste(taille+ (taille<<2));

    if (vecteur[dernier]==NULL) {
        vecteur[dernier]=new Liste;
        vecteur[dernier]->traitsbase=new Traits(parseur);
        vecteur[dernier]->traitsactif=new Traits(parseur);
    }

    Liste* l= vecteur[dernier];
    dernier++;
    l->initialise(X,tr);
    l->position=dernier-1;

	parseur->NoeudsTemporaires.affecte(parseur->dernierNumeroNoeud,l);
    l->N.num_noeud=parseur->dernierNumeroNoeud++;    
    return l;
}

void TasListe::RecupereDernier(GlobalParseur* parseur) {      
    dernier--;
    if (dernier<0) {
		dernier=0;
        return;
	}
    if (vecteur[dernier]->N.num_noeud==parseur->dernierNumeroNoeud-1)
        parseur->dernierNumeroNoeud--;
} 


void TasListe::Raz() {
    for (int i=0;i<taille;i++) {
        if (vecteur[i]!=NULL)
            vecteur[i]->N.initialise(NULL,NULL);
    } 
    dernier=0;
}


//inverse une liste: dcba devient abcd        
void LListe::inverse() {

    size_t moitie=dernier/2;
    Liste* inter=NULL;

    for (size_t i=0; i<moitie;i++) {
        inter=vecteur[i];
        vecteur[i]=vecteur[dernier-i-1];
        vecteur[dernier-i-1]=inter;
    }
}


void LListe::Affiche(ostream& os,size_t debut,size_t fin) {
    Liste* l;


    for (size_t i=0;i<(size_t)dernier;i++) {
        l=vecteur[i];   
        if (i)
            os << ",";
        if (i==debut)
            os<<" <";
        os << (l->X)->nom;
        if (i==fin) 
            os << ">";
    }
    os << ".\n";
}



