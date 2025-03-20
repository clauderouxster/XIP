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
   filename   : avlcat.h
   Date       : 16/09/1992
   Purpose    : Definition of a cat structure (that stores the bit index of a category and its name).
                Categorie defines a categorie together with its features
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef avlcat_h
#define avlcat_h
/*-----------------------La liste des noms de catégorie---------------------*/
class Traits;
class GlobalParseur;

#include "divers.h"
/*-------------------------Codage des catégories----------------------------
			      Val = 2^val */

struct Cat {short index;char cacher;typeCat type;short val; TVecteur  Val; char* nom;GlobalParseur* courant;};

//--------------------------------------------------------------------------

class Categorie {

 public:
   
	
   RAMASSAGE(Categorie);

   Cat X;
   Traits* traits;
   char etoile;
   char* fichier;

   Categorie();   
   Categorie(GlobalParseur* pars,char*); 
   Categorie(GlobalParseur* pars,char*,int);
   ~Categorie();
   void TraitEnChaine(GlobalParseur*,char* res);
   Cat* Categ() {return &X;}
};

Categorie* CreeCatVide(GlobalParseur*,char* symbole=(char*)ANYSYMBOL);

#endif

