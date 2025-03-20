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
   filename   : avl.cxx
   Date       : 10/01/1999
   Purpose    : Implementation of the base class avl methods.
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"


avl::avl(char* mot): cle(mot) {
  autre=NULL;
  gauche=NULL;
  droit=NULL;
  haut=0;
  frere=NULL;   
}

avl::avl(unsigned char* mot): cle(mot) {
  autre=NULL;
  gauche=NULL;
  droit=NULL;
  haut=0;
  frere=NULL;   
}


void avl::avl_insere(avl** courant,avl* ajout) {
    char test;

    if (*courant==NULL) 
        *courant=ajout;
    else {
        test= compchmin(ajout->cle.c,(*courant)->cle.c);
        if (!test) {
            avl* dernier=*courant;
            //On va chercher le dernier element de la liste
            while (dernier->autre!=NULL)
                dernier=dernier->autre;
            dernier->autre=ajout;
            //ajout->autre=(*courant)->autre;
            //(*courant)->autre=ajout;
        }
        else {
            if (test<0) 
                avl_insere(&(*courant)->gauche,ajout);
            else 
                avl_insere(&(*courant)->droit,ajout);
            hauteur(courant);
        }
    }
}

void avl::avl_inserecasse(avl** courant,avl* ajout) {
    int test;

    if (*courant==NULL) 
        *courant=ajout;
    else {
        test= compch(ajout->cle.c,(*courant)->cle.c);
        if (!test) {
            avl* dernier=*courant;
            //On va chercher le dernier element de la liste
            while (dernier->autre!=NULL)
                dernier=dernier->autre;
            dernier->autre=ajout;
            //ajout->autre=(*courant)->autre;
            //(*courant)->autre=ajout;
        }
        else {
            if (test<0) 
                avl_inserecasse(&(*courant)->gauche,ajout);
            else 
                avl_inserecasse(&(*courant)->droit,ajout);
            hauteur(courant);
        }
    }
}

void avl::typage(avl** courant,int *type) {
    int hg,hd;
    avl* element;

    element=*courant;
    if (*type<11 && element !=NULL) {
        if (element->gauche !=NULL) 
            hg=(element->gauche)->haut;
        else 
            hg=-1;
        if (element->droit !=NULL) 
            hd=(element->droit)->haut;
        else 
            hd=-1;
      if (hg>hd) {
          *type=*type*10+1;
          typage(&element->gauche,type);
      }
      else
          if (hg<hd || (hg==hd && hd!=-1)) {
              *type=*type*10+2;
              typage(&element->droit,type);
          }
    }
}

void avl::hauteur(avl** courant) {
    int hd,hg,type;
    avl* element;

    element=*courant;
    if (element->gauche !=NULL) 
        hg=1+(element->gauche)->haut;
    else 
        hg=0;
    if (element->droit !=NULL) 
        hd=1+(element->droit)->haut;
    else
        hd=0;
    if (hg==hd || hg==hd+1) 
        element->haut=hg;
    else 
        if (hd==hg+1) 
            element->haut=hd;
        else {
            type=0;
            if (hg>hd) 
                element->haut=hg;
	    else
                element->haut=hd;
            typage(courant,&type);
            reconstruit(courant,type);
        }
}


void avl::reconstruit(avl** courant,int type) {
    avl* element;
    avl* e;
    avl* eg;
    avl* ed;

    element=*courant;
    if (type>10) {
        switch(type) {
        case 11:
            e=(element->gauche)->droit;
            (element->gauche)->droit=element;
            *courant=element->gauche;
            ((*courant)->droit)->gauche=e;
            hauteur(&(*courant)->droit);
            hauteur(courant);
            break;
        case 12:
            eg=((element->gauche)->droit)->gauche;
            ed=((element->gauche)->droit)->droit;
            ((element->gauche)->droit)->gauche=element->gauche;
            ((element->gauche)->droit)->droit=element;
            *courant=(element->gauche)->droit;
            ((*courant)->gauche)->droit=eg;
            ((*courant)->droit)->gauche=ed;
            hauteur(&(*courant)->gauche);
            hauteur(&(*courant)->droit);
            hauteur(courant);
            break;
        case 21:
            eg=((element->droit)->gauche)->gauche;
            ed=((element->droit)->gauche)->droit;
            ((element->droit)->gauche)->droit=element->droit;
            ((element->droit)->gauche)->gauche=element;
            *courant=(element->droit)->gauche;
            ((*courant)->gauche)->droit=eg;
            ((*courant)->droit)->gauche=ed;
            hauteur(&(*courant)->gauche);
            hauteur(&(*courant)->droit);
            hauteur(courant);
            break;
        case 22: 
            e=(element->droit)->gauche;
            (element->droit)->gauche=element;
            *courant=element->droit;
            ((*courant)->gauche)->droit=e;
            hauteur(&(*courant)->gauche);
            hauteur(courant);
        }
    }
}

avl* avl::suivant() {return gauche;}

avl* avl::avl_recherche(char* mot) {
    char test;
    avl* e=this;
    
    while (e!=NULL) {
        test = compchmin((uchar*)mot,e->cle.c);
        if (test==0)
            return e;
        if (test<0)
            e=e->gauche;
        else
            e=e->droit;
    }
    return NULL;        
}


avl* avl::avl_recherchecasse(char* mot) {
    int test;
    avl* e=this;
    
    while (e!=NULL) {
        test = compch((uchar*)mot,e->cle.c);
        if (test==0)
            return e;
        if (test<0)
            e=e->gauche;
        else
            e=e->droit;
    }
    return NULL;
        
}

avl* avl::avl_recherche(unsigned char* mot) {
    char test;
    avl* e=this;
    
    while (e!=NULL) {
        test = compchmin(mot,e->cle.c);
        if (test==0)
            return e;
        if (test<0)
            e=e->gauche;
        else
            e=e->droit;
    }
    return NULL;        
}


avl* avl::avl_recherchecasse(unsigned char* mot) {
    int test;
    avl* e=this;
    
    while (e!=NULL) {
        test = compch(mot,e->cle.c);
        if (test==0)
            return e;
        if (test<0)
            e=e->gauche;
        else
            e=e->droit;
    }
    return NULL;
        
}

