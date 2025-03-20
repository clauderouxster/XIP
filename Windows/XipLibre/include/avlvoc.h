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
   filename   : avlvoc.h
   Date       : 15/09/1999
   Purpose    : Definition of a word reading (ListeVoc), and handling of internal lexicons.
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef avlvoc_h
#define avlvoc_h
#include "avl.h"
#include "vecteur.h"
#include "avltrait.h"

/*-------------------------------------------------------------------------*/
class chaine;
class Liste;
struct Cat;
class Traits;
class GlobalParseur;
class VectTestFonction;
class VectTestFonctionSemantique;
class Sequence;
class baseregle;

/*-------------------------------------------------------------------------*/
class multimots : public baseregle {
public:

	RAMASSAGE(multimots);
    Cat* X;
    Traits* tr;
	Sequence* gauche;
	Sequence* droit;
	ListeTestArbre* listes;
    
    multimots(Cat* x,Traits* t,Sequence* g,Sequence* d,ListeTestArbre* l) {
		RAMASSE(multimots);
		gauche=g;
		droit=d;
		listes=l;
        tr=t;
        X=x;
    }
	~multimots() {
		JETTE(multimots);
	}
};
/*-------------------------------------------------------------------------*/
class auto_vocabulaire {
 public:

	RAMASSAGE(auto_vocabulaire);
    Cat* X;
    Cat* XNouveau;
    Traits* tr;
    Traits* initiaux;
    Traits* traits_mot;
    char* lemme;
    typeVoc ecraser;
    auto_vocabulaire* autre;
	Sequence* gauche;
     

    auto_vocabulaire(GlobalParseur*,char*,char*,Cat*,Traits*,Traits*,Sequence* s=NULL);
    auto_vocabulaire(GlobalParseur*,char*,char*,Cat*,Cat*,Traits*,Traits*,Sequence* s=NULL);
    ~auto_vocabulaire();
    auto_vocabulaire* recherche(char* mot);    
    auto_vocabulaire* recherchecasse(char* mot);    
    auto_vocabulaire* Autre() {return autre;}
    void ajoute(auto_vocabulaire* a);
};


class avl_vocabulaire : public avl {
 public:

    RAMASSAGE(avl_vocabulaire);
    Cat* X;
    Cat* XNouveau;
    Traits* tr;
    Traits* initiaux;
    Traits* traits_mot;
    char* lemme;
    typeVoc ecraser;


    avl_vocabulaire(GlobalParseur*,char*,char*,Cat*,Traits*,Traits*);
    avl_vocabulaire(GlobalParseur*,char*,char*,Cat*,Cat*,Traits*,Traits*);
    ~avl_vocabulaire();
    avl_vocabulaire* recherche(char* mot);
    virtual void insere(avl_vocabulaire**,avl_vocabulaire*);
    avl_vocabulaire* recherchecasse(char* mot);
    void inserecasse(avl_vocabulaire**,avl_vocabulaire*);
    avl_vocabulaire* Autre();
    avl_vocabulaire* Frere();
};

class avl_vocabulaire_semantique : public avl {
 public:
    
    RAMASSAGE(avl_vocabulaire_semantique);
    VectTestFonction* semantique;
    Cat* X;
    Traits* traits_mot;

    int num_fichier;
    int compteur;
    long curseur;
    long ligne;
    char charge;

    avl_vocabulaire_semantique(char* lem,Cat*,long c,long, int n);
    avl_vocabulaire_semantique(GlobalParseur* p,char* lem,Cat* x,Traits* tr,VectTestFonction* vtf);
    
    ~avl_vocabulaire_semantique();
    
    void inserecasse(avl_vocabulaire_semantique**,avl_vocabulaire_semantique*);
    avl_vocabulaire_semantique* recherchecasse(char* mot);
    avl_vocabulaire_semantique* Autre();
};

class auto_vocabulaire_semantique {
 public:
    
    RAMASSAGE(auto_vocabulaire_semantique);
    VectTestFonctionSemantique* semantique;
    Cat* X;
    Traits* traits_mot;
    char* lemme;
    int num_fichier;
    int compteur;
    long curseur;
    long ligne;
    char charge;
    auto_vocabulaire_semantique* autre;

    auto_vocabulaire_semantique(char* lem,Cat*,long c,long, int n);
    auto_vocabulaire_semantique(GlobalParseur* p,char* lem,Cat* x,Traits* tr,VectTestFonctionSemantique* vtf);
    
    ~auto_vocabulaire_semantique();
    
    void ajoute(auto_vocabulaire_semantique*);
    auto_vocabulaire_semantique* Autre();
};

//---------------------------------------------------------------------------------------------
class avl_vocabulaire_terme : public avl {
 public:
    
	RAMASSAGE(avl_vocabulaire_terme);
    VectSequence* terme;

    avl_vocabulaire_terme(GlobalParseur* p,char* mot,Sequence* vtf);
    ~avl_vocabulaire_terme();

    char inserecasse(avl_vocabulaire_terme**,avl_vocabulaire_terme*);
    avl_vocabulaire_terme* recherchecasse(char* mot);
    avl_vocabulaire_terme* Autre();
};


class vocabulaire_terme {
 public:
    
	RAMASSAGE(vocabulaire_terme);
    VectSequence* terme;

    vocabulaire_terme(Sequence* vtf);
    ~vocabulaire_terme();
};

class vocabulaire_fragmente {
 public:
    
	RAMASSAGE(vocabulaire_fragmente);
    Sequence* fragmente;
    automatepile a;
    char surface;

    vocabulaire_fragmente(unsigned char*,Sequence* vtf);
    ~vocabulaire_fragmente();
};

/*-------------------------------------------------------------------------*/

class ListeVoc {
 public:

 
  VECTA<Traits*> tr;
  VECTA<char*> lemme;
  VECTA<Cat*> X; 
  VECTA<char*> tag;
  VECTA<char> garder;
  VECTA<ListeVoc*> suivants;

  char* surface;
  char* surfacebrute;

  int nbX;
  char passage;
  long num_mot;
  int motGauche,motDroit;
  long borneGauche,borneDroite;
  long borneGaucheCar, borneDroiteCar;
  Traits* global;
  int etat;
  Liste* L;
  ListeVoc* suivant;
  char reutiliser;
  int position;
  int idx;

  TVecteur CalculIndex() {
      TVecteur val=0;
      for (int i=0;i<nbX;i++)
          val|=X[i]->Val;
      return val;
  }
 
  ListeVoc(vocabulaire&,char maj=NON);
  ListeVoc(void);
  void init(vocabulaire&,char maj=NON);
  void cree(void);
  void raz();

  
  void Marque();
  void affiche(GlobalParseur*);
  ~ListeVoc();
};

void RemplaceLesMots(char*);
void Insere(avl_vocabulaire**,avl_vocabulaire* ,avl_vocabulaire*);
#endif

