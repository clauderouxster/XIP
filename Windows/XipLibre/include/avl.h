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
   filename   : avl.h
   Date       : 10/01/1999
   Purpose    : This object is base class of binary balanced tree handling
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef avl_h
#define avl_h

class avl {
    int haut;
 
 public:

    avl* gauche;
    avl* droit;
    avl* autre;
    avl* frere;

    chaine cle;

 
    avl(char*);
    avl(unsigned char*);
    virtual ~avl() {};

    void typage(avl**,int*);
    void reconstruit(avl**,int);
    void hauteur(avl**);
    void affiche(avl*,int);
    avl* suivant(void);
    virtual void avl_insere(avl**,avl*);
    avl* avl_recherche(char* mot);
    void avl_inserecasse(avl**,avl*);
    avl* avl_recherchecasse(char* mot);

    avl* avl_recherche(unsigned char* mot);
    avl* avl_recherchecasse(unsigned char* mot);
};

#endif

