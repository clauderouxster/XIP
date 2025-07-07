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
   filename   : vecteur.h
   Date       : 10/09/1992
   Purpose    : Binary vector implementation to handle features.
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef vecteur_h
#define vecteur_h
#include "defun.h"

class GlobalParseur;
class XipFeature;

#ifdef XIPSTLAPI
#include <vector>
#endif



class Vecteur {

 public:
    
    TVecteur* vecteur;
    short TailleVect;

    Vecteur(short taille);
    Vecteur(Vecteur*);
    void un(GlobalParseur*,int);
    void Max();
    
    char contient(Vecteur*);
    char NeContientPas(Vecteur*);
    char contientNegation(Vecteur* neg);
   
    void xou(Vecteur*);
    void zero(GlobalParseur*,int);
    char Zero();

    void raz();
	void un();
    void ou(Vecteur* v);
    void etnon(Vecteur* v);
    char Test(int i);

    void et(Vecteur*);
    char egal(Vecteur*);
    void affiche(GlobalParseur* pars,short);
    void affichev(GlobalParseur* pars,Vecteur*,Vecteur*);
    void afficheTrace(GlobalParseur* pars);
    void Affiche(GlobalParseur* pars,Vecteur*,Vecteur*);
    void recopie(Vecteur*);
    void Chaine(GlobalParseur*,char*);
    void ChaineAttributs(GlobalParseur*,char*);
    void TraitsEnChaine(GlobalParseur* pars,char* resultat,Vecteur* Li,Vecteur* pres,Vecteur* In,Vecteur* Ne);
	void TraitsEnMap(GlobalParseur* pars, map<string,string>& m, Vecteur* Li, Vecteur* pres, Vecteur* In, Vecteur* Ne);

    void TraitsEnChaineString(GlobalParseur* pars,
                        string& resultat,
                        Vecteur* Li,
                        Vecteur* pres,
                        Vecteur* In,
                        Vecteur* Ne);

    void ConcateneAttributs(GlobalParseur* pars,char* chaine,Vecteur*,char* separateur=(char*)SEPARATEUR);
    void ConcateneValeurs(GlobalParseur* pars,char* chaine,Vecteur*,char* separateur=(char*)SEPARATEUR,char posSep=NON);
	void ConcateneValeursString(GlobalParseur* pars,string& chaine,Vecteur*,char* separateur=(char*)SEPARATEUR,char posSep=NON);

    void AjouteTraits(GlobalParseur* pars,VECTA<XipFeature*>& xip,Vecteur* filtre,Vecteur* libre);

#ifdef XIPSTLAPI
    void AjouteTraitsSTL(GlobalParseur* pars,VXipFeature& xip,Vecteur* filtre,Vecteur* libre);
#endif

    void AfficheValeursXML(GlobalParseur* pars,ostream& os,Vecteur* filtre);
    void Liste(VECTA<int>& liste);
    ~Vecteur();
    char Valeur(GlobalParseur* pars,char* chaine,int);
    char Attribut(GlobalParseur* pars,char* chaine);
#ifdef XIPLIBXML
    void ConstruitXML(GlobalParseur*,xmlNodePtr,Vecteur* filtre);
#endif    
};


inline void Vecteur::raz() {
    memset(vecteur,0,sizeof(TVecteur)*TailleVect);
}

inline void Vecteur::un() {
    memset(vecteur,-1,sizeof(TVecteur)*TailleVect);
}

inline void  Vecteur::ou(Vecteur* v) {
    TVecteur* v1=vecteur;
    TVecteur* v2=v->vecteur;
    for (register int i=0;i<TailleVect;i++) {*v1|=*v2;++v1;++v2;}
}

inline void  Vecteur::etnon(Vecteur* v) { 
    TVecteur* v1=vecteur;
    TVecteur* v2=v->vecteur;
    for (register int i=0;i<TailleVect;i++) {
        *v1&=~*v2;
        ++v1;++v2;
    }
}

inline char  Vecteur::Test(int i) {
    int n;
    STVecteur v;
    
    n= i >> gBitVectDivise; //division par racine(gBitVect)
    v = (STVecteur)vecteur[n];
    //On deplace le bit correspondant vers la gauche de TAILLEVECTEUR - reste, en position signe
    //Le reste se calcule par la formule: i- (i/gBitVect)*gBitVect
    n = gBitVect - (i - (n << gBitVectDivise)); 
    v<<= n;
    //Si v est negatif c'est que le bit etait a 1
    if (v<0)
        return OUI;
    return NON;
}

#endif
