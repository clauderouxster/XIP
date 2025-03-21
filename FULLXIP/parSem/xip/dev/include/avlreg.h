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
   filename   : avlreg.h
   Date       : 08/09/1992
   Purpose    : Regle defines a ID-rule. The cle in Regle is used to store ID-rule in a 
                binary balanced tree to find which rule to apply on a set of nodes.
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef avlreg_h
#define avlreg_h
#include "avltrait.h"

class Sequence;
class ListeTestArbre;
class GlobalParseur;
class baseregle;

class Doublon {
 public:

    Cat* X;
    int nb;
    Doublon* suivant;

    Doublon(GlobalParseur* pars,Cat* x,char etoile);

    Doublon(Doublon*);

    ~Doublon() {
        if (suivant!= NULL)
            delete suivant;
    }

    char insere(GlobalParseur* pars,Cat*,char);
    Doublon* recherche(Cat*);
};

class Regle : public baseregle {
   public:

    TVecteur  cle;
    short Nombre;
    Cat* Sx; /* Sx-> x,Spec(x),..*/
    //si une categorie est marquee avec le trait last (ou first), nous enregistrons le type de cette categorie ici...
    Cat* last;
    Cat* first;
    int IDoublon;
    Doublon* doublon;
    char passageLexical;
    ChaineTraits* traits;
    ChaineTraits* TeteTraits;
    Regle* suivant;
    Regle* courant;
	Regle* frere;
    Sequence* gauche;
    Sequence* droit;
    char detfonc;
	int borneG;
	int borneD;

    ListeTestArbre* listes;

    Regle(Cat*);
    Regle(Cat*,Traits*);
    Regle(Regle*);
    ~Regle();
    void detruit();
    void detruitNoeud();
    char egalite(Regle* r);
    void affiche(char);
    char retire(Cat*);
    char Contient(Regle*);
    void insere(Regle* r);
    void insere(GlobalParseur*,Cat*);
    void insere(GlobalParseur* pars,Cat*,Traits*,char etoileKleene,int arg,char testLecture);
    char testDerniereCat(Cat* X);
    char testPremiereCat(Cat* X);
    char TestExpressionBooleen(GlobalParseur* parseur,VECTA<Noeud*>& vect_noeuds);
    char TestContexte(GlobalParseur*,
                      LListe& Base, 
                      size_t ipremier,
                      size_t idernier,
                      VECTA<Noeud*>& vect_noeuds);
};

/*-------------------------------------------------------------------------*/

class avl_Regle {
    //int haut;
    
public:
    
    //avl_Regle* gauche;
    //avl_Regle* droit;
    
	Regle* noeud;
	avl_Regle* autre;
    
        
    avl_Regle(Regle*);
    ~avl_Regle();
    avl_Regle* recherche(Regle*);
    avl_Regle* recherche(TVecteur);
    
    //void typage(avl_Regle**,int*);
    //void reconstruit(avl_Regle**,int);
    //void hauteur(avl_Regle**);
    //void affiche(avl_Regle*,int);
    //avl_Regle* suivant(void);
    //void insere(avl_Regle**,avl_Regle*);    
};

/*-------------------------------------------------------------------------*/
/* 
 Cette classe est fondamentale, en effet c'est dans un élément de cette classe que l'on 
enregistre
chacune des catégories lues... C'est dans un élément de cette classe que «traits» s'initialise 
pour
la valeur Cat...*/

 class ChaineCat {

 public:

  RAMASSAGE(ChaineCat);

  char Code;
  char etoile;
  Cat* X;
  Traits* traits;
  char testLecture;
  int arg;

  ChaineCat(GlobalParseur*,char code,char etoileKleene,Cat* x,Traits* ts,char tl) ;
  ~ChaineCat();
 };
/*-------------------------------------------------------------------------*/
/* Grace à cette classe on peut enregistrer des PL_règles ou bien des V_règles.

 - valeur[0],négation[0] contient la partie gauche de la VRègle ou de la PL-règle.
 - valeur[1],négation[1],traits contiennent la partie droite de la Vrègle ou de la PL-règle.*/

 class RegleTraits : public baseregle {
public:

  Traits* test;
  Traits* affecte;
  Cat* X;

  RegleTraits(Traits* p,Traits* s,Cat* c);
  ~RegleTraits() {
      delete test;
      delete affecte;
  }
  void affiche(GlobalParseur*) {}
  void affichePL(GlobalParseur* pars) {}  
  char Test(GlobalParseur*,Traits*,Cat*,char);
};
/*-------------------------------------------------------------------------*/
    
class ChercheRegle {
 public:

    TVecteur cle;

    VECTA<Doublon*> doublon;

    ChercheRegle(GlobalParseur* pars) : doublon(15) {
        cle=0; 
        for (int i=0;i<15;i++)
            doublon.affecte(i,new Doublon(pars,NULL,NON));
        doublon.dernier=0;
    }

    ChercheRegle(GlobalParseur* pars,Cat* X) : doublon(15) {
        cle=X->Val;
        for (int i=0;i<15;i++)
            doublon.affecte(i,new Doublon(pars,NULL,NON));
        doublon.dernier=0;
    }

    ~ChercheRegle() {
        doublon.nettoie();
    }

    void retire(Cat* X);
    void insere(GlobalParseur* pars,Cat* X);
    void raz();
};


/*-------------------------------------------------------------------------*/
Regle* ConsUneRegle(Regle* rg,Regle* rd,Regle* r);
/*-------------------------------------------------------------------------*/

void GenereDi();
/*-------------------------------------------------------------------------*/


#endif
