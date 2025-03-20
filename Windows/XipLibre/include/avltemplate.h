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
   filename   : avltemplate.h
   Date       : 10/01/1999
   Purpose    : A template to handle binary balanced tree
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef avltemplate_h
#define avltemplate_h
#include "xipvecta.h"

template <class Z> class AVL {
    int haut;
 
 public:

    AVL<Z>* gauche;
    AVL<Z>* droit;
    AVL<Z>* autre;

    unsigned char* cle;
    Z element;
	char detruit;

    ~AVL() {
        libere(cle);
        if (element!=NULL)
            delete element;
        if (gauche!=NULL)
            delete gauche;
        if (droit!=NULL)
            delete droit;
        if (autre!=NULL)
            delete autre;
    }

    AVL(unsigned char* mot,Z z)  {
		detruit=NON;
        cle=DupliqueChaine(mot);
        gauche=NULL;
        droit=NULL;
        autre=gauche;
        haut=0;
        element=z;
    }

    void Detruit(VECTA<AVL<Z>*>& liste) {   
		if (detruit==OUI)
			return;

        if (gauche!=NULL)
            gauche->Detruit(liste);
        if (droit!=NULL)
            droit->Detruit(liste);
        if (autre!=NULL)
            autre->Detruit(liste);
        //On ne detruit pas l'element courant qui peut etre partage
		if (detruit==NON) {
			gauche=NULL;
			droit=NULL;
			autre=NULL;
			element=NULL;
			detruit=OUI;
			liste.ajoute(this);
		}
    }


    void insere(AVL<Z>** courant,AVL<Z>* ajout) {
        char test;



        if (*courant==NULL) 
            *courant=ajout;
        else {
            test= compchmin(ajout->cle,(*courant)->cle);
            if (!test) {
                AVL<Z>* dernier=*courant;
                //On va chercher le dernier element de la liste
                while (dernier->autre!=NULL)
                    dernier=dernier->autre;
                dernier->autre=ajout;
            }
            else {
                if (test<0) 
                    insere(&(*courant)->gauche,ajout);
                else 
                    insere(&(*courant)->droit,ajout);
                hauteur(courant);
            }
        }
    }
    
    AVL<Z>* insereUnique(AVL<Z>** courant,AVL<Z>* ajout) {
        char test;
        AVL<Z>* z=this;

       
        if (*courant==NULL) {
            *courant=ajout;
            return ajout;
        }
        else {
            test= compchmin(ajout->cle,(*courant)->cle);
            if (!test) {
                delete ajout;
                return this;
            }
            else {
                if (test<0) 
                    z=insereUnique(&(*courant)->gauche,ajout);
                else 
                    z=insereUnique(&(*courant)->droit,ajout);
                hauteur(courant);
            }
        }
        return z;
    }

    AVL<Z>* insereErreur(AVL<Z>** courant,AVL<Z>* ajout) {
        char test;
        AVL<Z>* z=this;

       
        if (*courant==NULL) {
            *courant=ajout;
            return ajout;
        }
        else {
            test= compchmin(ajout->cle,(*courant)->cle);
            if (!test)
                return NULL;
            else {
                if (test<0) 
                    z=insereErreur(&(*courant)->gauche,ajout);
                else 
                    z=insereErreur(&(*courant)->droit,ajout);
                if (z!=NULL)
                    hauteur(courant);
            }
        }
        return z;
    }

    void typage(AVL<Z>** courant,int *type) {
        int hg,hd;
        AVL<Z>* element;
        
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

    void hauteur(AVL<Z>** courant) {
        int hd,hg,type;
        AVL<Z>* element;
        
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


    void reconstruit(AVL<Z>** courant,int type) {
        AVL<Z>* element;
        AVL<Z>* e;
        AVL<Z>* eg;
        AVL<Z>* ed;

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
    
 AVL<Z>* suivant() {return gauche;}

 Z recherche(unsigned char* mot) {
     char test=compchmin(cle,mot);
     
     if (!test) 
         return element;
     else 
         if (test>0)
             if (gauche) 
                 return(gauche->recherche(mot)); 
             else
                 return NULL;
         else
             if (droit) 
                 return(droit->recherche(mot)); 
             else 
                 return NULL;
 }

 AVL<Z>* Recherche(unsigned char* mot) {
     char test=compchmin(cle,mot);
     
     if (!test) 
         return this;
     else 
         if (test>0)
             if (gauche) 
                 return(gauche->Recherche(mot)); 
             else
                 return NULL;
         else
             if (droit) 
                 return(droit->Recherche(mot)); 
             else 
                 return NULL;
 }
 
};

#endif

