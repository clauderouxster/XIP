/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003, Xerox Corporation, XRCE-Grenoble.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 3.16
   filename   : xipserveur.cxx
   Date       : 10/01/1999
   Purpose    : Reading of an output of server as XML and transformation into a XipResult
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include <string.h>
#include <iostream.h>
#include <stdlib.h>
#include "xipresult.h"

static char receptacle[10000];
static char manipulateur[10000];

static void transmet(int serveur,char* chaine) {
    sprintf(manipulateur,"%d@%s",strlen(chaine),chaine);
    send(serveur,manipulateur,strlen(manipulateur),0);
}

static void transmet(int serveur,int valeur) {
    char chaine[1000];
    sprintf(chaine,"%d",valeur);
    sprintf(manipulateur,"@%d@%s",strlen(chaine),chaine);
    send(serveur,manipulateur,strlen(manipulateur),0);
}

static void transmet(int serveur,long valeur) {
    char chaine[1000];
    sprintf(chaine,"%d",valeur);
    sprintf(manipulateur,"@%d@%s",strlen(chaine),chaine);
    send(serveur,manipulateur,strlen(manipulateur),0);
}


static int nbCaracteresDansManipulateur;
static int encoreALire;
static int indexDansManipulateur=0;
static InitialisationReception() {
    nbCaracteresDansManipulateur=0;
    encoreALire=0;
    indexDansManipulateur=0;
    manipulateur[0]=0;
}

static void recoit(int serveur,char* chaine) {
    //On cherche le premier index dans la chaine de forme: <%d>@
    char* pt=NULL;
    while (pt==NULL) {
        pt=strchr(manipulateur+indexDansManipulateur,'@');
        if (pt==NULL) {
            nbCaracteresDansManipulateur=read(serveur,manipulateur,9999); //on reinitialise le tableau
            manipulateur[nbCaracteresDansManipulateur]=0;
            indexDansManipulateur=0;
        }
    }
    //Calcul de la position de l'arobase:
    long position= pt-manipulateur;
    //Si la position est superieur au nombre de caracteres lus, cela signifie que la description est dans le champ suivant
    //il faut relire encore une fois.
    if (position >= nbCaracteresDansManipulateur) {        
        nbCaracteresDansManipulateur=read(serveur,manipulateur,9999); //on reinitialise le tableau
        manipulateur[nbCaracteresDansManipulateur]=0;
        pt=strchr(manipulateur,'@');
        *pt=0;
        strcpy(chiffre,manipulateur);
        indexDansManipulateur=pt-manipulateur+1;        
    }
    else {
        indexDansManipulateur=position+1;
        pt=strchr(manipulateur+indexDansManipulateur,'@');
        chiffre[0]=0;
        //on recherche la fin de la structure
        if (pt==NULL) {
            strcpy(chiffre,manipulateur+indexDansManipulateur);
            nbCaracteresDansManipulateur=read(serveur,manipulateur,9999); //on reinitialise le tableau
            manipulateur[nbCaracteresDansManipulateur]=0;
            indexDansManipulateur=0;
        }
        pt=strchr(manipulateur+indexDansManipulateur,'@');
        *pt=0;
        strcat(chiffre,manipulateur+indexDansManipulateur);
        indexDansManipulateur=pt-manipulateur+1;        
    }

            
    //le premier chiffre correspond a la taille de caractere que l'on cherche
    int taille=manipulateur
}
    
    
        

void XipFeature::envoyer(int serveur) {
    sprintf(receptacle,"%s:%s",attribute,value);
    transmet(serveur,receptacle);
}

void XipFeature::recevoir(int serveur) {
    recoit(serveur,receptacle);
    char* pt=strchr(receptacle,':');
    *pt=0;
    attribut=DupliqueChaine(receptacle);
    valeur=DupliqueChaine(pt+1);
}

void XipNode::envoyer(int serveur) {
    int i;

    if (lemma!=NULL)
        sprintf(receptacle,"%s:%s",surface,lemma);
    else
        strcpy(receptacle,surface);

    transmet(serveur,receptacle);
    transmet(serveur,left);
    transmet(serveur,right);
    transmet(serveur,features.dernier);

    for (i=0;i<features.dernier;i++)
        features[i]->envoyer(serveur);

    transmet(serveur,daughters.dernier);

    for (i=0;i<daughters.dernier;i++)
        daughters[i]->envoyer(serveur);
}

void XipNode::recevoir(int serveur) {
    int i;

    recoit(serveur,receptacle);
    char* pt=strchr(receptacle,':');

    transmet(serveur,left);
    transmet(serveur,right);
    transmet(serveur,features.dernier);

    for (i=0;i<features.dernier;i++)
        features[i]->envoyer(serveur);

    transmet(serveur,daughters.dernier);

    for (i=0;i<daughters.dernier;i++)
        daughters[i]->envoyer(serveur);
}

void XipNode::envoyer_daughters(int serveur) {
    
    if (lemma!=NULL)
        os<<surface;
    else {    
        for (register int i=0;i<daughters.dernier;i++) {
            if (i!=0)
                os<<" ";
            daughters[i]->envoyer_daughters(os);
        }
    }    
}

void XipDependency::envoyer(int serveur) {
    int i;
    os<<name;
    if (features.dernier) {
        for (i=0;i<features.dernier;i++)
            os<<"_"<<features[i]->attribute;
    }
    os<<"(";
    for (i=0;i<parameters.dernier;i++) {
        if (i)
            os<<",";
        parameters[i]->envoyer_daughters(os);
    }

    os<<")"<<Endl;
}

void XipUnit::envoyer(int serveur) {
    if (root==NULL)
        return;
    root->envoyer(os);
    os<<Endl<<Endl;
    for (int i=0;i<dependencies.dernier;i++)
        dependencies[i]->envoyer(os);
}

void XipResult::envoyer(int serveur) {
    
    for (int i=0;i<sentences.dernier;i++) {
        os<<"Result: "<<i<<Endl;
        sentences[i]->envoyer(os);
        os<<Endl<<Endl;
    }
}

