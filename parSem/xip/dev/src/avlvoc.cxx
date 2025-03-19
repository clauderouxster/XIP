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
   filename   : avlvoc.cxx
   Date       : 10/01/1999
   Purpose    : Implementation of internal lexicons handling
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"
CREERAMASSE(multimots);
CREERAMASSE(auto_vocabulaire);
CREERAMASSE(avl_vocabulaire);
CREERAMASSE(avl_vocabulaire_semantique);
CREERAMASSE(auto_vocabulaire_semantique);
CREERAMASSE(vocabulaire_terme);
CREERAMASSE(vocabulaire_fragmente);
CREERAMASSE(avl_vocabulaire_terme);
extern GlobalParseur* parseur_courant;
/*-------------------------------------------------------------------------*/
ListeVoc* MotInconnu(char* mot,char,int);
/*-------------------------------------------------------------------------*/
//Semantique
/*-------------------------------------------------------------------------*/

void vocabulaire::detruit() {
    if (initiaux!=NULL)
        delete initiaux;
    initiaux=NULL;
    X=NULL;
    lemme[0]=0;
    surface[0]=0;
	surfacebrute[0]=0;
}

vocabulaire::~vocabulaire() { 
    if (initiaux!=NULL)
        delete initiaux;        
}


avl_vocabulaire_semantique::~avl_vocabulaire_semantique() {
	JETTE(avl_vocabulaire_semantique);
    if (semantique!=NULL)
        delete semantique;
}

avl_vocabulaire_semantique::avl_vocabulaire_semantique(char* lem,Cat* x,long c,long l, int n) : avl(lem) {
	RAMASSE(avl_vocabulaire_semantique);
    semantique=NULL;
    X=x;
    charge=NON;
    curseur=c;
    ligne=l;
    num_fichier=n;
    compteur=-1;
}

avl_vocabulaire_semantique::avl_vocabulaire_semantique(GlobalParseur* p,char* lem,Cat* x,Traits* tr,VectTestFonction* vtf) : avl(lem) {
	RAMASSE(avl_vocabulaire_semantique);
    semantique=vtf;
    X=x;
    traits_mot=p->FactoriseTraits(tr);
    compteur=-1;
    charge=OUI;
}

avl_vocabulaire_semantique* avl_vocabulaire_semantique::recherchecasse(char* mot)
 {
  return (avl_vocabulaire_semantique*)avl_recherchecasse((uchar*)mot);
 }


void avl_vocabulaire_semantique::inserecasse(avl_vocabulaire_semantique** courant,avl_vocabulaire_semantique* ajout)
 {
  avl_inserecasse((avl**)courant,(avl*)ajout);
 }

avl_vocabulaire_semantique* avl_vocabulaire_semantique::Autre() {return (avl_vocabulaire_semantique*)autre;};
/*-------------------------------------------------------------------------*/
auto_vocabulaire_semantique::~auto_vocabulaire_semantique() {
	JETTE(auto_vocabulaire_semantique);
    if (semantique!=NULL)
        delete semantique;
    libere(lemme);
}

auto_vocabulaire_semantique::auto_vocabulaire_semantique(char* lem,Cat* x,long c,long l, int n) {
	RAMASSE(auto_vocabulaire_semantique);
    semantique=NULL;
    X=x;
    charge=NON;
    curseur=c;
    ligne=l;
    num_fichier=n;
    compteur=-1;
    autre=NULL;
    lemme=DupliqueChaine(lem);
}

void auto_vocabulaire_semantique::ajoute(auto_vocabulaire_semantique* vs) {
    auto_vocabulaire_semantique* avs=this;

    while (avs->autre!=NULL) avs=avs->autre;
    avs->autre=vs;
}

auto_vocabulaire_semantique::auto_vocabulaire_semantique(GlobalParseur* p,char* lem,Cat* x,Traits* tr,VectTestFonctionSemantique* vtf) {
	RAMASSE(auto_vocabulaire_semantique);
    semantique=vtf;
    X=x;
    traits_mot=p->FactoriseTraits(tr);
    compteur=-1;
    charge=OUI;
    autre=NULL;
    lemme=DupliqueChaine(lem);
}

auto_vocabulaire_semantique* auto_vocabulaire_semantique::Autre() {
    return autre;
}

/*-------------------------------------------------------------------------*/
//Termes
/*-------------------------------------------------------------------------*/
vocabulaire_terme::~vocabulaire_terme() {
	JETTE(vocabulaire_terme);
    terme->nettoie();
    delete terme;
}

vocabulaire_terme::vocabulaire_terme(Sequence* vtf) {
	RAMASSE(vocabulaire_terme);
    terme = new VectSequence(1);
    terme->ajoute(vtf);    
}
/*-------------------------------------------------------------------------*/
//Fragmentes
/*-------------------------------------------------------------------------*/
vocabulaire_fragmente::~vocabulaire_fragmente() {
	JETTE(vocabulaire_fragmente);
    delete fragmente;
}

vocabulaire_fragmente::vocabulaire_fragmente(unsigned char* m,Sequence* vtf) : a(m,NULL) {
	RAMASSE(vocabulaire_fragmente);
    surface=0;
    fragmente=vtf;
}


/*-------------------------------------------------------------------------*/
avl_vocabulaire_terme::~avl_vocabulaire_terme() {
	JETTE(avl_vocabulaire_terme);
    terme->nettoie();
    delete terme;
}

avl_vocabulaire_terme::avl_vocabulaire_terme(GlobalParseur* p,char* mot,Sequence* vtf) :
        avl(mot) {
	RAMASSE(avl_vocabulaire_terme);
    terme = new VectSequence(1);
    terme->ajoute(vtf);
}

avl_vocabulaire_terme* avl_vocabulaire_terme::recherchecasse(char* mot)
{
    return (avl_vocabulaire_terme*)avl_recherchecasse((uchar*)mot);
}


char avl_vocabulaire_terme::inserecasse(avl_vocabulaire_terme** courant,avl_vocabulaire_terme* ajout) {
    int test;
    char retour=NON;

    if (*courant==NULL) { 
        *courant=ajout;
        return OUI;
    }
    else {
        test= strcmp((char*)ajout->cle.c,(char*)(*courant)->cle.c);
        if (!test) {
            ((*courant)->terme)->ajoute((ajout->terme)->vecteur[0]);
            (ajout->terme)->raz();
            delete ajout;
            return 2;
        }
        else {
            if (test<0) 
                retour=inserecasse((avl_vocabulaire_terme**)&(*courant)->gauche,ajout);
            else 
                retour=inserecasse((avl_vocabulaire_terme**)&(*courant)->droit,ajout);
            hauteur((avl**)courant);
        }
    }
    return retour;
}

avl_vocabulaire_terme* avl_vocabulaire_terme::Autre() {return (avl_vocabulaire_terme*)autre;};

/*-------------------------------------------------------------------------*/
auto_vocabulaire::auto_vocabulaire(GlobalParseur* pars,
                                 char* mot,
                                 char* ch,
                                 Cat* x,
                                 Traits* rt,
                                 Traits* init,
								 Sequence* s) {
    RAMASSE(auto_vocabulaire);
    initiaux=pars->FactoriseTraits(init);
    X=x;
    XNouveau=x;
	gauche=s;
    tr=pars->FactoriseTraits(rt);
    traits_mot=NULL;
    lemme=NULL;
    if (ch!=NULL)
        lemme=DupliqueChaine(ch);    
    ecraser=AJOUTER;
    autre=NULL;
}

auto_vocabulaire::auto_vocabulaire(GlobalParseur* pars,
                                 char* mot,
                                 char* ch,
                                 Cat* x,Cat*xnouv,
                                 Traits* rt,
								 Traits* init,
								 Sequence* s) {

    RAMASSE(auto_vocabulaire);
    initiaux=pars->FactoriseTraits(init);
    X=x;
	gauche=s;
    XNouveau=xnouv;
    tr=pars->FactoriseTraits(rt);
    traits_mot=NULL;
    lemme=NULL;
    if (ch!=NULL)
        lemme=DupliqueChaine(ch);    
    ecraser=AJOUTER;
    autre=NULL;
}

void auto_vocabulaire::ajoute(auto_vocabulaire* a) {
    auto_vocabulaire* av=this;
    while (av->autre!=NULL) av=av->autre;
    av->autre=a;
}

auto_vocabulaire::~auto_vocabulaire() {
	JETTE(auto_vocabulaire);
    if (lemme!=NULL)
        libere(lemme);
}


/*-------------------------------------------------------------------------*/
avl_vocabulaire::avl_vocabulaire(GlobalParseur* pars,
                                 char* mot,
                                 char* ch,
                                 Cat* x,
                                 Traits* rt,
                                 Traits* init): avl(mot) {
	RAMASSE(avl_vocabulaire);
    initiaux=pars->FactoriseTraits(init);
    X=x;
    XNouveau=x;
    tr=pars->FactoriseTraits(rt);
    traits_mot=NULL;
    lemme=ch;    
    ecraser=AJOUTER;
}

avl_vocabulaire::avl_vocabulaire(GlobalParseur* pars,
                                 char* mot,
                                 char* ch,
                                 Cat* x,Cat*xnouv,
                                 Traits* rt,Traits* init): avl(mot) {

	RAMASSE(avl_vocabulaire);
    initiaux=pars->FactoriseTraits(init);
    X=x;
    XNouveau=xnouv;
    tr=pars->FactoriseTraits(rt);
    traits_mot=NULL;
    lemme=ch;    
    ecraser=AJOUTER;
}

avl_vocabulaire::~avl_vocabulaire() {

	JETTE(avl_vocabulaire);
}

avl_vocabulaire* avl_vocabulaire::recherchecasse(char* mot)
 {
  return (avl_vocabulaire*)avl_recherchecasse((uchar*)mot);
 }

void avl_vocabulaire::inserecasse(avl_vocabulaire** courant,avl_vocabulaire* ajout)
 {
  avl_inserecasse((avl**)courant,(avl*)ajout);
 }

avl_vocabulaire* avl_vocabulaire::recherche(char* mot)
 {
  return (avl_vocabulaire*)avl_recherche((uchar*)mot);
 }

void avl_vocabulaire::insere(avl_vocabulaire** courant,avl_vocabulaire* ajout)
 {
  avl_insere((avl**)courant,(avl*)ajout);
 }


avl_vocabulaire* avl_vocabulaire::Autre() {return (avl_vocabulaire*)autre;};
avl_vocabulaire* avl_vocabulaire::Frere() {return (avl_vocabulaire*)frere;};
/*-------------------------------------------------------------------------*/
void Insere(avl_vocabulaire** arbre,avl_vocabulaire* liste,avl_vocabulaire* v) {
    static avl_vocabulaire* AVfin;
 
    if (liste->frere==NULL) 
        AVfin=liste;
    v->frere=AVfin->frere;
    AVfin->frere=v;
    AVfin=v;

    if (*arbre==NULL)
        return;

    (*arbre)->insere(arbre,v);
}

/*-------------------------------------------------------------------------*/

ListeVoc::ListeVoc(void) {	
#ifdef XIPLIBXML
	xmlnoeud=NULL;
#endif
	position=-1;
	reutiliser=NON;
    etat=-1;
    passage=0;
    num_mot=0;
    nbX=0;
    L=NULL;
    borneGauche=0;
    borneDroite=0;
	borneGaucheCar=0;
	borneDroiteCar=0;
    global=NULL;
    surface=NULL;
	surfacebrute=NULL;
	suivant=NULL;
 }

 void ListeVoc::cree(void) {	
#ifdef XIPLIBXML
	xmlnoeud=NULL;
#endif
	reutiliser=NON;
    etat=-1;
    passage=0;
    num_mot=0;
    nbX=0;
    L=NULL;
    borneGauche=0;
    borneDroite=0;
	borneGaucheCar=0;
	borneDroiteCar=0;
    global=NULL;
    surface=NULL;
	surfacebrute=NULL;
	suivant=NULL;
 }
 
ListeVoc::~ListeVoc() {
    int i;
    for (i=0;i<nbX;i++) {
        if (tag[i]!=NULL)
            libere(tag[i]);
        if (lemme[i]!=NULL)
            libere(lemme[i]);    
		if (tr[i]!=NULL)
			delete tr[i];    
    }

    if (surface!=NULL)
        libere(surface);    
    if (surfacebrute!=NULL)
        libere(surfacebrute);    

    if (global!=NULL)
        delete global;
}  

void ListeVoc::raz() {
    int i;
#ifdef XIPLIBXML
	xmlnoeud=NULL;
#endif

    for (i=0;i<nbX;i++) {
        if (tag[i]!=NULL)
            libere(tag[i]);
        if (lemme[i]!=NULL)
            libere(lemme[i]);   
		if (tr[i]!=NULL)
			delete tr[i];    
    }

	reutiliser=NON;
    if (surface!=NULL)
        libere(surface);    
    if (surfacebrute!=NULL)
        libere(surfacebrute);    

    if (global!=NULL)
        delete global;

	tag.raz();
	lemme.raz();
	tr.raz();
	X.raz();
	suivants.raz();
	suivant=NULL;

	etat=-1;
	passage=0;
	
	num_mot=0;
	nbX=0;
	L=NULL;
	borneGauche=0;
	borneDroite=0;
	borneGaucheCar=0;
	borneDroiteCar=0;
	global=NULL;
	surface=NULL;
	surfacebrute=NULL;
}  

void ListeVoc::init(vocabulaire& A,char majuscule) {
	reutiliser=NON;
    etat=-1;
    nbX=1;
    passage=0;
    num_mot=0;

    tr.ajoute(A.initiaux);
    lemme.ajoute(DupliqueChaine(A.lemme));
    surface=DupliqueChaine(A.surface);
	surfacebrute=DupliqueChaine(A.surfacebrute);
    X.ajoute(A.X);
    tag.ajoute(NULL);
    L=NULL;
    borneGauche=A.gauche;
    borneDroite=A.droit;
    borneGaucheCar=A.gauchecar;
    borneDroiteCar=A.droitcar;
    global=NULL;
	suivant=NULL;
#ifdef XIPLIBXML
	xmlnoeud=NULL;
#endif

}


ListeVoc::ListeVoc(vocabulaire& A,char majuscule) {
#ifdef XIPLIBXML
	xmlnoeud=NULL;
#endif
	position=-1;
	reutiliser=NON;
    etat=-1;
    nbX=1;
    passage=0;
    num_mot=0;

	suivant=NULL;
    tr.ajoute(A.initiaux);
    lemme.ajoute(DupliqueChaine(A.lemme));
    surface=DupliqueChaine(A.surface);
	surfacebrute=DupliqueChaine(A.surfacebrute);
    X.ajoute(A.X);
    tag.ajoute(NULL);
    L=NULL;
    borneGauche=A.gauche;
    borneDroite=A.droit;
    borneGaucheCar=A.gauchecar;
    borneDroiteCar=A.droitcar;
    global=NULL;
}


void ListeVoc::Marque()
 {
     for (int i=0;i<nbX;i++)
         garder.affecte(i,NON);
 }

    
void ListeVoc::affiche(GlobalParseur* pars) {
    trace <<  X[0]->nom << ":" << lemme[0] << " ";
}

/*-------------------------------------------------------------------------*/

ListeVoc* GlobalParseur::fournirListeVocVide() {
	ListeVoc* lv=NULL;
	do {
		if (TasDeListeVoc.dernier>= TasDeListeVoc.taille) {
			TasDeListeVoc.ajuste(TasDeListeVoc.taille*2);	
			TasDeListeVoc.dernier++;
			lv=NULL;
			break;
		}
		TasDeListeVoc.dernier++;
		lv=TasDeListeVoc.fin();
	}
	while (lv!=NULL && lv->reutiliser==OUI);

	if (lv==NULL) {
		lv=new ListeVoc;
		TasDeListeVoc.dernier--;
		lv->position=TasDeListeVoc.dernier;
		TasDeListeVoc.ajoute(lv);
		lv->reutiliser=OUI;
		return lv;
	}
	lv->reutiliser=OUI;
	return lv;
}

ListeVoc* GlobalParseur::fournirListeVoc(vocabulaire& A,char majuscule) {
	ListeVoc* lv=NULL;

	do {
		if (TasDeListeVoc.dernier>= TasDeListeVoc.taille) {
			TasDeListeVoc.ajuste(TasDeListeVoc.taille*2);	
			TasDeListeVoc.dernier++;
			lv=NULL;
			break;
		}
		TasDeListeVoc.dernier++;
		lv=TasDeListeVoc.fin();
	}
	while (lv!=NULL && lv->reutiliser==OUI);

	if (lv==NULL) {
		lv=new ListeVoc(A,majuscule);
		TasDeListeVoc.dernier--;
		lv->position=TasDeListeVoc.dernier;
		TasDeListeVoc.ajoute(lv);
		lv->reutiliser=OUI;
		return lv;
	}
	
	lv->init(A,majuscule);
	lv->reutiliser=OUI;
	return lv;
}

void GlobalParseur::nettoyerListeVoc() {
	for (int i = 0; i < TasDeListeVoc.taille; i++) {
		if (TasDeListeVoc.vecteur[i] != NULL)
			TasDeListeVoc.vecteur[i]->raz();
	}
	TasDeListeVoc.dernier = 0;
}
