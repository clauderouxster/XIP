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
   filename   : avlcat.cxx
   Date       : 10/01/1999
   Purpose    : Implementation of the compiling of categories
   Programmer : Claude ROUX
   Reviewer   :
   */

#include "parsbin.h"
#include "kif.h"
/*-----------------------La liste des noms de catégorie---------------------*/
char existeFonction(char* nom);


#ifndef WIN32
char* strupr(char* n) {
	unsigned int longueur=strlen(n);

	for (int i=0;i<longueur;i++)
		n[i]=toupper(n[i]);
	return n;
}
#endif

CREERAMASSE(Categorie);

Categorie::Categorie(GlobalParseur* grammaire, char* mot) {
	RAMASSE(Categorie);

	int i = grammaire->iCategorie;
	X.cacher = NON;
	fichier = NULL;

	if (i >= NbCats)
		grammaire->erreur(grammaire->CHAINES[141]);
	else {
		traits = NULL;
		X.nom = strupr(DupliqueChaine(mot));

		X.val = i;
		X.Val = Un << i;
		X.index = 0;
		X.type = C_CATEGORIE;

		etoile = NON;
		grammaire->MaxIndiceCat = i;
		grammaire->TableauDesNoms[i] = X.nom;
		grammaire->iCategorie = i + 1;
	}
}

Categorie::Categorie(GlobalParseur* grammaire, char* mot, int index) {
	RAMASSE(Categorie);

	X.val = index;
	X.cacher = NON;
	X.type = C_FONCTION;
	fichier = NULL;

	if (index >= NbCats)
		X.Val = 0;
	else
		X.Val = Un << index;

	X.nom = strupr(DupliqueChaine(mot));

	etoile = NON;
	traits = NULL;
	grammaire->MaxIndiceFonc = index;
}


Categorie::Categorie() {
	RAMASSE(Categorie);
	fichier = NULL;
	X.val = 0;
	X.Val = 0;
	X.index = 0;
	X.nom = DupliqueChaine((char*)ANYSYMBOL);
	X.cacher = NON;
	traits = NULL;
}



Categorie::~Categorie() {
	JETTE(Categorie);
	libere(X.nom);
	if (fichier != NULL)
		libere(fichier);
}

void Categorie::TraitEnChaine(GlobalParseur* grammaire, char* res) {
	res[0] = 0;
	if (traits != NULL)
		traits->TraitsEnChaine(grammaire, res);
}

/*-------------------Cette procédure permet l'insertion d'une nouvelle Catégorie dans la liste*/

Categorie* CreeCatVide(GlobalParseur* grammaire, char* symbole) {

	Categorie* cc = new Categorie(grammaire, symbole);
	grammaire->Categories.ajoute(cc);
	cc->X.index = 0;
	cc->X.val = 0;
	cc->X.Val = 0;
	cc->X.index = 0;
	cc->X.courant = grammaire;
	cc->traits = new Traits(grammaire);

	return cc;
}


Categorie* GlobalParseur::CreeCat(char* mot, char* fichier) {
	Categorie* cc;


	if (mot[0] == '?')
		return CatVide;

	for (int i = 0; i < Categories.dernier; i++)
	if (!strcmpi(Categories[i]->X.nom, mot))
		return Categories[i];

	cc = new Categorie(this, mot);
	if (fichier != NULL)
		cc->fichier = DupliqueChaine(fichier);
	cc->X.type = C_CATEGORIE;
	cc->X.courant = this;
	Categories.ajoute(cc);
	cc->X.index = Categories.dernier - 1;
	return cc;
}


Categorie* GlobalParseur::ChercheCat(char* mot) {

	if (mot[0] == '?')
		return CatVide;

	for (int i = 0; i < Categories.dernier; i++)
	if (!strcmpi(Categories[i]->X.nom, mot))
		return Categories[i];


	char chaine[1000];
	strcpy_s(chaine, 1000, mot);
	strcat_s(chaine, 1000 - strlen(chaine), "   ");
	strcat_s(chaine, 1000, CHAINES[193]);
	erreur(chaine);

	return NULL;
}

//Creation de fonctions syntaxiques...
Categorie* GlobalParseur::CreeFonc(char* mot, int index, Traits* tr, char* fichier) {
	Categorie* cc;

	for (int i = 0; i < Fonctions.dernier; i++)
	if (!strcmpi(Fonctions[i]->X.nom, mot))
		return Fonctions[i];


	cc = new Categorie(this, mot, index);
	if (fichier != NULL)
		cc->fichier = DupliqueChaine(fichier);
	cc->traits = tr;
	cc->X.type = C_FONCTION;
	cc->X.courant = this;
	Fonctions.ajoute(cc);
	cc->X.index = Fonctions.dernier - 1;
	return cc;
}


KifElement* ChercheDansKif(KifCode* kifcode, string name) {
	if (kifcode == NULL)
		return NULL;
	return kifcode->Find(name);
}


//recherche dans le tableau des noms de fonctions et des procedures
Categorie* GlobalParseur::ChercheFonc(char* mot) {
	int i;
	if (mot[0] == '?')
		return CatVide;

	for (i = 0; i < Fonctions.dernier; i++) {
		if (!strcmpi(Fonctions[i]->X.nom, mot))
			return Fonctions[i];
	}

	for (i = 0; i < Procedures.dernier; i++) {
		if (!strcmpi(Procedures[i]->X.nom, mot))
			return Procedures[i];
	}

	//Si cette methode a ete declaree en tant que KIF, on construit une categorie a la volee
	string original = mot;
	if (nomsoriginaux.find(original) != nomsoriginaux.end())
		original = nomsoriginaux[mot];

	if (ChercheDansKif(kifloader, original) != NULL) {
		Categorie* cc = new Categorie(this, STR(original), Procedures.dernier - 1);
		strcpy(cc->X.nom, STR(original));
		cc->X.type = C_KIF;
		Procedures.ajoute(cc);
		cc->X.Val = 0;
		cc->X.index = Procedures.dernier - 1;
		cc->X.courant = this;
		return cc;
	}

	char chaine[1000];
	strcpy_s(chaine, 1000, mot);
	strcat_s(chaine, 1000 - strlen(chaine), "   ");
	strcat_s(chaine, 1000, CHAINES[193]);
	erreur(chaine);

	return NULL;
}


//Creation de procedures...
Categorie* GlobalParseur::CreeProcedure(char* mot) {
	Categorie* cc;


	if (existeFonction(mot) != -1)
		erreur(CHAINES[253]);

	for (int i = 0; i < Procedures.dernier; i++) {
		if (!strcmpi(Procedures[i]->X.nom, mot)) {
			if (Procedures[i]->X.type == C_PROCEDUREDECLAREE)
				return Procedures[i];
			erreur(CHAINES[253]);
		}
	}

	cc = new Categorie(this, mot, Procedures.dernier - 1);
	cc->X.type = C_PROCEDURE;
	Procedures.ajoute(cc);
	cc->X.Val = 0;
	cc->X.index = Procedures.dernier - 1;
	cc->X.courant = this;
	return cc;
}

//recherche dans le tableau des noms de fonctions et des procedures
Categorie* GlobalParseur::RetrouveFonc(char* mot) {
	int i;
	if (mot[0] == '?')
		return CatVide;

	for (i = 0; i < Fonctions.dernier; i++) {
		if (!strcmpi(Fonctions[i]->X.nom, mot))
			return Fonctions[i];
	}

	for (i = 0; i < Procedures.dernier; i++)
	if (!strcmpi(Procedures[i]->X.nom, mot))
		return Procedures[i];

	return NULL;
}

Categorie* GlobalParseur::RetrouveCat(char* mot) {

	if (mot[0] == '?')
		return CatVide;

	for (int i = 0; i < Categories.dernier; i++)
	if (!strcmpi(Categories[i]->X.nom, mot))
		return Categories[i];

	return NULL;
}



