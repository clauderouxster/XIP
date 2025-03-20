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
   filename   : xmlxip.cxx
   Date       : 10/01/1999
   Purpose    : Basic implementation of XML input parsing
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"

void majvariablelemme(VariableLemmeIndex* vlemme,Noeud* n,uchar* chaine,int pos,string,double res);    
extern ostream* flot_erreur;

char TestChaineVide(unsigned char* chaine) {
    size_t l=strlen((char*)chaine);
    for (size_t i=0;i<l;i++) {
        if (chaine[i]>32)
            return NON;
    }

    return OUI;
}


//Le fichier contient alors des entrees de la forme:
/*
 <LUNIT>

 <NODE tag=v, start=v, end=v> //can contain embedded nodes
     <features>    //optional (global features)
     </features>
     </mainreading lemma=v,pos=v>
     <reading lemma=v, pos=v>
        <features> //local features
          attr=v.
          attr=v.
        </features>
     </reading> //could be other readings.
 </NODE>
 </LUNIT>
*/
char rechercheNomFonction(char* nom);


Stocke_Variable* RechercheVariable(VECTA<Variable*>& Var,char* nom, char* valeur) {
    char c;

    double vf=atof(valeur);
    char* pt=strstr(nom,"##");
    if (pt!=NULL) {
        c=pt[2];
        pt[2]=0;
    }
    
    for (int i=0;i<Var.dernier;i++) {
        if (!strcmp(nom,Var[i]->nom)) {
            if (pt!=NULL)
                pt[2]=c;
            Stocke_Variable* sv=new Stocke_Variable(Var[i],vf,nom);
            return sv;
        }
    }

    if (pt!=NULL)
        pt[2]=c;

    return NULL;
}

char NettoieUneChaine(char* chaine,char* resul,const char* trouve,const char* remplace) {

    char* debut;
    char* courant=chaine;
    char trouver=NON;

    resul[0]=0;
    while ((debut=strstr(courant,trouve))) {
        *debut=0;
        strcat(resul,courant);
        strcat(resul,remplace);
        courant=debut+strlen(trouve);
        trouver=OUI;
    }        
    
    strcat(resul,courant);
    return trouver;
}


void GlobalParseur::NettoieLaChaine(char* chaine,char* resul) {
    char buffer[1000]; 
    
    strcpy(resul,chaine);
    for (int i=0;i<caractereXML.dernier;i++) {
        if (NettoieUneChaine(chaine,buffer,codageXML[i],caractereXML[i]))
            strcpy(resul,buffer);
    }
}


    





    
    

        




