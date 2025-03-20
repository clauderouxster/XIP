/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.64
   filename   : generation.h
   Date       : 05/03/2009
   Purpose    : Generation
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef generation_h
#define generation_h

//Nous creeons la structure pour la generation
//Il s'agit d'un arbre

class genereGraphe {
public:

	ResultatFonction* noeud;
	genereGraphe* parent;
	genereGraphe* parentancien;
	VECTA<genereGraphe*> enfants;
	string mot;
	int pos;
	int posancien;
	int tokenpos;
	char formecalcule;
	char maj;

	genereGraphe(ResultatFonction* r=NULL);

	~genereGraphe();

	int dernier() {
		return enfants.dernier;
	}

	void ajoute(genereGraphe* g,int p);
	void retire(int p);

	genereGraphe* recupere(ResultatFonction* r);
	void affiche(ostream&);
	void nettoie();
	void profondeur(int p,int n);
	char retire();
};


class genere_traits {
public:
	Cat* X;
	Traits* tr;
	string lestraits;

	genere_traits(Cat* x, Traits* t,char* s) : lestraits(s) {
		X=x;
		tr=t;
	}
};

#endif
