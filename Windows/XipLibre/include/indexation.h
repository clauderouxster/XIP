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
   filename   : indexation.h
   Date       : 12/05/2002
   Purpose    : Indexing of a semantic disambiguation file
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef indexation_h
#define indexation_h
#include "parsbin.h"

class entreeindex {
 public:

    int num_fichier;
    char* lemme;
    long curseur;
    long ligne;
    char* categorie;
    char* traits;
    
    entreeindex(char* m,char* cat,char* tr,long c,long l, int n)  {
        lemme=DupliqueChaine(m);
        curseur=c;
        num_fichier=n;
        ligne=l;
        categorie=strdup(cat);
        traits=strdup(tr);
    }            

    ~entreeindex() {
        libere(lemme);
        free(categorie);
        free(traits);
    }
};


class indexeur {
 public:

    long lignes;
    ofstream findex;
    
    indexeur(char* nom) : findex(nom) {        
        lignes=0;
    }

    indexeur() {
        lignes=0;
    }

    int LitIndex(GlobalParseur*,
        ifstream& fichier,
        char* tampon,
        char* categorie,
        char* traits,
        Traits** tr,
        long& premier,
        long& ligne,
        char construitindex);
    unsigned char LitCommentaires(ifstream& fichier,unsigned char cc,unsigned char* r);
    void extrait_index(VECTA<char*>& fichiers);
    unsigned char lire(ifstream& fichier);
    char recherche_champ(ifstream& fichier);
    void indexer(GlobalParseur*,char* nom);
    void LitCategorie(ifstream& fichier,char* tampon,char* traits,uchar& c);
    void LitUneCategorie(GlobalParseur*,ifstream& fichier,char* categorie,Traits** traits,uchar& c);
    
    void raz() {
        lignes=0;
    }

    char LireEntreCotes(ifstream& fichier,char* tampon,uchar& k,int& i);
};


#endif

