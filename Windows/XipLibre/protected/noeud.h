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
   filename   : noeud.h
   Date       : 10/09/1992
   Purpose    : Syntactic Node Object embedded in a Liste object
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef noeud_h
#define noeud_h

class Traits;
struct Cat;
class Vecteur;
class ListeVoc;
class Cornes;
class Regle;
class ResultatSequence;
class ResultatFonction;
class Liste;
class GlobalParseur;
class XipNode;
class XipUnit;
class Variable;

char TestADroite(Cat*,Cat*);


/*-------------------------------------------------------------------------*/
char TestAttente(Cat*);
char TestRetire(Cat*);
/*-------------------------------------------------------------------------*/



class Noeud {

public:

 Cat* X;

 Noeud* frere;
 Noeud* fils;
 Noeud* dernier;
 Noeud* precedent;
 Noeud* pere;
 char* surface;
 char* lemme;
 int num_mot;
 int num_noeud;
 long gauche,droite;
 long gauchecar,droitecar;
 int num_gauche,num_droite;
 TVecteur clef;
 TVecteur clef_suivant;
 int nombre;
 int numero;
 int index;
 TVecteur clefsurindex;

 Liste* source;

 XipNode* soeur;
 int etat;
 int hauteur;
 char marquage;
 char modifie;
 char fictif;
 char sousnoeud;

 string infos;
 Noeud(); 
 Noeud(Cat*,Liste*);
 ~Noeud();
 
 void initialise(Cat* x,Liste* L);
 void copie(Noeud* n);

 TVecteur ConstruitIndex(GlobalParseur*,Noeud*,char,int hauteur); 
 int AfficheMot(GlobalParseur*,ostream&);
 void AfficheMotPF(GlobalParseur* pars);
 void affiches(GlobalParseur* pars,int);
 void SauveResultat(GlobalParseur*,ostream&);
 void SauveResultatXML(GlobalParseur*,ostream&);
 void annule();
 void retire();
 int TraitsEnChaine(GlobalParseur*,char*);
 XipNode* ConstruitResultat(GlobalParseur*,XipUnit*);
 void  AutomateLemme(string& chaine);
 void ListeLemme(vector<string>& vs);

 int distance(Noeud* n) {
	 return abs(n->num_mot - num_mot);
 }

 int distancearbre(Noeud* n);

 Noeud* voisin(int increment) {
     if (increment==-1)
         return precedent;
     return frere;
 }

 Traits* lesTraits();
 Traits* lesTraitsAffiche();

 char terminal() {
     if (lemme!=NULL)
         return OUI;
     return NON;
 }

 char filsTerminal() {
     if (fils!=NULL)
         if (fils->lemme!=NULL)
             return OUI;
     
     return NON;
 }

 void RetTete();

 void insere(Noeud* n);
 void Lemme(unsigned char*,size_t);
 void Lemme(string&);
 void Surface(string&,char ajouteblanc=OUI,long gauche=0,long droite=-1);
 void Surface(unsigned char*,size_t,char ajouteblanc=OUI,long gauche=0,long droite=-1);
#ifdef XIPLIBXML
 /** Start MCR added code for passage XML output **/
 void ConstruitNoeudXML(GlobalParseur*,xmlNodePtr);
 void ConstruitNoeudXMLStandard(GlobalParseur*,xmlNodePtr);
 void ConstruitNoeudXMLPassageTokens(GlobalParseur* parseur,xmlNodePtr noeudParent);
 void ConstruitNoeudXMLPassageGroupes(GlobalParseur* parseur,xmlNodePtr noeudParent, int& numGroupe);
 char* idPassage;
 /** End MCR added code for passage XML output **/
#endif
 int TraitsEnChaineString(GlobalParseur* parseur,string& chaine_traits);
 void ConstruitUnNoeud(GlobalParseur* pars, 
     string& rpos,
     string& rsurface,
     string& rlemme,
     string& rtraits,
	 string& initialfeatures,
     int& rpere,
     int& rfils,
     int& rdernier,
     int& rsuivant,
     int& rprecedent);
 void annulereconstruction();
 void inseresous(Noeud* n);
 void insertion(Noeud* n);
};

/*-------------------------------------------------------------------------*/


class Liste {

public:

 ListeVoc* voc;
 Cat* X;
 Noeud N;

 Traits* traitsactif;
 Traits* traitsbase;
 Traits* aSelectionner;
 Traits* instanciateur;
 Vecteur* blocfiltrage;

 TVecteur index;
 int choisie;
 int position;
 int indexCat;
 int iphrase;
 char debut,fin,couvert;
 VECTA<Variable*> variables;

 Liste(Cat* x=NULL);

 ~Liste();
 void copie(Liste&);
 void StabiliseTraits(char);
 char Attente();
 void choixCategorie(GlobalParseur*,int i);
 void choixCategorieTestNoeud(GlobalParseur*,int i);
 void choixCatTraits(int i);
 void RetrouveTraits();

 void SauveTraits();

 void initialise(Cat* x,Traits* tr);

 void affiche(Liste*);
 void affiches();
 void affichet(GlobalParseur*,char*);
 void affiche();
 void Affiche(); 
 void SauveResultat(GlobalParseur* p) {N.SauveResultat(p,cout);}
 void Affiche(Liste*,Liste*,Liste*);
 ResultatSequence* ResFonc(int laCouche);

 char TestEnCouche(GlobalParseur*,int profondeur,int&,char&);
};

//-----------------------------------------------------------------------------

char AppPl(int laCouche,LListe&,size_t,size_t);
void Initialisations(Traits* trait,Vecteur* neg);
char Test(int i,TVecteur  vecteur);
void AppDecib(Traits* traits);

extern char TestTetes(Vecteur* v);
void affiche(short i,TVecteur v);

#endif
