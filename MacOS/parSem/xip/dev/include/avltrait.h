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
   filename   : avltrait.h
   Date       : 16/09/1992
   Purpose    : The classes to handle features
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef avltrait_h
#define avltrait_h
#include "parsbin.h"

class GlobalParseur;
class VariableString;


class arbretrait {
public:

    string nom;
    VECTA<arbretrait*> fils;
    arbretrait* pere;

    arbretrait(GlobalParseur*);

    void ajoute(arbretrait* a) {
        a->pere=this;
        fils.ajoute(a);
    }
	
	virtual char type();    
};

class featureroot : public arbretrait {
public:
    string filename;
	featureroot(GlobalParseur* parseur, char* f, char* n) : arbretrait(parseur), filename(f) {
		nom = n;
	}
    char type();	
};


class trait : public avl {

 public:

  RAMASSAGE(trait);

  Vecteur* valeur;
  typeTrait type;
  int indice;
  char permanent;


  arbretrait* freretrait;
 
  trait(GlobalParseur*,const char* mot,int i,typeTrait ty);
  ~trait();
 
  trait* recherche(const char*);
  void insere(trait** ATR,trait* tr);
  trait* Autre();
  trait* Frere();
};


/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
class Traits {
 public:

  //RAMASSAGE(Traits);
  
  Vecteur* valeur;
  Vecteur* bloquage;
  Vecteur* filtre;
  Vecteur* libre;
  Vecteur* negation;  
  Vecteur* present;
  Vecteur* lpresent;
  Vecteur* efface;
  Vecteur* permanent;

  automate* lemme;  
  automate* surface;  

  TestFormule* varlemme;
  TestFormule* varsurface;

  typeManip maniplemme;
  typeManip manipsurface;
  
  VECTA<ontologie*>* onto;
  
  Traits(Traits* tr);
  Traits(Traits& tr);
  Traits(Traits* tr,short* calcul);
  Traits(GlobalParseur*);
  Traits(int);
  ~Traits();
  void etnon(Traits*);
  void et(Traits*);
  void etnon(trait*);
  void etnon(Categorie*);
  void raz();
  void razp();
  void ou(Traits*); 
  void ou(Traits&);
  void oup(Traits*); 
  void oup(Traits&);
  void xou(Traits*);
  void ou(Categorie*);
  void oup(Categorie*);
  void ou(trait*);  
  void oup(trait*);  
  char contient(Traits*);
  void recopie(Traits*);
  void recopie(Traits&);
  char egal(Traits*);
  char vide();
  void TraitsEnChaine(GlobalParseur*,char* resultat);
  void TraitsEnMap(GlobalParseur*, map<string,string>& m);
  void TraitsEnChaineString(GlobalParseur* parseur,string& chaine_traits,Vecteur* filtre=NULL);
  char egaux(Traits* tr);
  Vecteur* Libre() {if (libre!=NULL && !libre->Zero()) return libre;return NULL;}
  void AfficheValeursXML(GlobalParseur* pars,ostream& os,Vecteur* filtre);
#ifdef XIPLIBXML
  void ConstruitXML(GlobalParseur* parseur,xmlNodePtr node,Vecteur* filtre);
#endif
  ontologie* Onto(actionOntologie a) {
	  if (onto!=NULL)
		  return (*onto)[a];
	  return NULL;
  }
  TVecteur ClefHachage(int tailleTable);
  void inittrait(trait*,trait*);
};
  

class abreviation {
public:

 char* Nom;
 Traits* traits;
 abreviation* suivant;
 
 abreviation(char* n,Traits* tr);
 ~abreviation();
   
 void insere(GlobalParseur*,char*,Traits*);
 Traits* recherche(GlobalParseur*,char*);
 void nom(Traits* TR,char*);
 char* alias(Traits*);
};


/*-------------------------------------------------------------------------------------*/
/*Cette classe est conçue pour enregistrer dans une liste, les structures lues dans lesfichiers textes...*/ 

class ListeDefaut {
 public:
  
  Vecteur* valeur;
  Vecteur* contexte;
  Vecteur* non;
  int reponse;
  int un;
  int deux;
  
  ListeDefaut* suivant;
  ListeDefaut* courant;
  
  ~ListeDefaut()
    {
	 if (contexte!=NULL) delete contexte;
	 if (non!=NULL) delete non;
	 if (suivant!=NULL) delete suivant;
    }
  
  ListeDefaut(Vecteur* v,Traits* tr,int i,int j, int k);
  void insere(Vecteur* v,Traits* tr,int i,int j,int k);
};
    
 
 /*-------------------------------------------------------------------------------------*/
//Cette classe est utilisee pour la factorisation des traits entre les regles...
class ChaineTraits {

 public:

    Traits* traits;
    ChaineTraits* suivant;
    ChaineTraits* autre;
    char dejaLu;
    int arg;
    Cat* X;
    char etoile;
    char testLecture;

    ChaineTraits(Traits* traits,Cat*,char etoileKleene,int argu=-1,char testLecture=NON);
    ChaineTraits(ChaineTraits* traits,short& i);
    ~ChaineTraits();

    void insere(ChaineTraits* tr);
    void initialise();

    short val() {
        if (X!=NULL)
            return X->val;
        return -1;
    }
};

/*-----------------------------------------------------------------------*/
class CatTraits {
 public:
    
	RAMASSAGE(CatTraits);
    Cat* X;
    Traits* traits;
    
    CatTraits(Cat* x, Traits* tr) {
		RAMASSE(CatTraits);
        traits=tr;
        X=x;
    }
	~CatTraits() {
		JETTE(CatTraits);
	}
};

/*-----------------------------------------------------------------------*/

class DefinitionCategorie {
public:
	Cat* X;
	Traits* traitstest;
	Traits* traitsaffecter;
	DefinitionCategorie(Cat* x,Traits* tr,Traits* trc) {
		X=x;
		traitstest=tr;
		traitsaffecter=trc;
	}
};

#endif
