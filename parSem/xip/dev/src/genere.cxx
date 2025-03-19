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
   filename   : genere.cxx
   Date       : 10/01/1999
   Purpose    : C Rule Generation
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"
#include <locale.h>

#ifdef XIPLIBXML
#include "varlibxml.h"
#endif




//Cette methode permet de generer l'equivalent d'une fonction C a partir d'une regle
//XIP
//etatfin a trois valeurs:
// etatfin=0  transition normale
// etatfin=1  final
// etatfin=2  
void EtatArbre::generation(ostream& os,int id,int profondeur) {
    marque=1;

    switch(etatfin) {
    case 0:
        strcat(ch,"<n");
        break;
    case 1:    
        strcat(ch,"<e");
        break;
    case 2:
        strcat(ch,"<x");
    }

    sprintf(chh,"%d>",etat);
    strcat(ch,chh);
    profondeur+=strlen(ch);
    os<<ch;
    int k;
    if (fils) {
        os<<Endl;
        for (k=0;k<profondeur;k++)
            os<<" ";        
        os<<"{";        
        profondeur;
        fils->print(os,profondeur+1);        
        for (k=0;k<profondeur;k++)
            os<<" ";
        os<<"}"<<Endl;
        for (k=0;k<profondeur;k++)
            os<<" ";
    }
    
    int cr=1;
    for (int i=0;i<arcs.dernier;i++) {
        arcs[i]->print(os,etat,profondeur);
        if (arcs[i]->etatarbre->etatfin==1 || arcs[i]->etatarbre->marque==1)
            cr=0;
        if (i<arcs.dernier-1) {
            for (k=0;k<profondeur;k++)
                os<<" ";
        }
    }
    

    if (cr==1 && (profondeur==0 || etatfin!=0))
        os<<Endl;

}


void generetraits(ostream& os,Traits* tr,char* trch) {
    TVecteur code=0;
    for (i=0;i<iTailleVect;i++)
        code^=(ts->valeur)->vecteur[i];
    
    code=code%100;
    
    for (i=0;i<ListeDesTraits[code].dernier;i++) {
        if (ListeDesTraits[code][i]->egaux(ts)) {
            delete ts;
            return ListeDesTraits[code][i];
        }
    }
    
}

void ArcArbre::generation(ostream& os,int id,int profondeur) {  

    char prev=marque;
    marque=1;

    static char trch[100];
    if (traits!=NULL) {
        os<<"static Traits tr"<<num<<";";
        sprintf_s(trch,100,"tr%d",num);
        generetraits(traits,trch);


    strcpy_s(ch,100,"-(");
    if (negation==1) {
        strcat_s(ch,100,"~");
        taille++;
    }
    strcat_s(ch,100,X->nom);
    ch[taille+5]=0;
    sprintf(chh,"%d",num);
    strcat_s(ch,100,chh);
    if (argument!=-1) {
        sprintf(chh,"#%d",argument);
        strcat_s(ch,100,chh);
    }
    if (type==2)
        strcat_s(ch,100,"^");
    strcat_s(ch,100,")-");
    os<<ch;
    if (etatarbre->marque==-1)
        etatarbre->print(os,profondeur+strlen(ch));    
    else {
        if (type==1)
            os<<"<b";
        else
            os<<"<g";

        switch (etatarbre->etatfin) {
        case 0:
            os<<etatarbre->etat<<">";
            break;
        case 1:
            os<<"e"<<etatarbre->etat<<">";
            break;
        case 2:
            os<<"x"<<etatarbre->etat<<">";
        }            
        os<<Endl;
    }
}


void EtatArc::generation(ostream& os,int id,int profondeur) {

}

void EtatArbre::genere(GlobalParseur* parseur,ostream& os,int profondeur) {
    marque=1;

    switch(etatfin) {
    case 0:
        break;
    case 1:    
        break;
    case 2:
        break;
    }

    
    if (fils) {        
        fils->genere(parseur,os,profondeur+1);        
    }
        
    for (int i=0;i<arcs.dernier;i++)
        arcs[i]->genere(parseur,os,etat,profondeur);
}

void ArcArbre::genere(GlobalParseur* parseur,ostream& os,int id,int profondeur) {
    int taille=2;
    char ch[20];
    char chh[20];
    
    char prev=marque;
    marque=1;    
    os<<"Phrase[index+"<<profondeur<<"]->X";
    if (negation==1)
        os<<"!";

    os<<"=TableauDesNoms["<<X->val<<"]";
    if (argument!=-1) {
        sprintf_s(chh,20,"#%d",argument);
        strcat_s(ch,20,chh);
    }
    if (type==2)
        strcat_s(ch,20,"^");
    strcat_s(ch,20,")-");
    os<<ch;
    if (etatarbre->marque==-1)
        etatarbre->genere(parseur,os,profondeur+strlen(ch));    
    else {
        if (type==1)
            os<<"<b";
        else
            os<<"<g";

        switch (etatarbre->etatfin) {
        case 0:
            os<<etatarbre->etat<<">";
            break;
        case 1:
            os<<"e"<<etatarbre->etat<<">";
            break;
        case 2:
            os<<"x"<<etatarbre->etat<<">";
        }            
        os<<Endl;
    }
}


void EtatArbre::print(ostream& os,int profondeur) {
    char ch[20];
    char chh[20];
    ch[0]=0;
    chh[0]=0;
    marque=1;

    switch(etatfin) {
    case 0:
        strcat_s(ch,20,"<n");
        break;
    case 1:    
        strcat_s(ch,20,"<e");
        break;
    case 2:
        strcat_s(ch,20,"<x");
    }

    sprintf_s(chh,20,"%d>",etat);
    strcat_s(ch,20,chh);
    profondeur+=strlen(ch);
    os<<ch;
    int k;
    if (fils) {
        os<<Endl;
        for (k=0;k<profondeur;k++)
            os<<" ";        
        os<<"{";        
        profondeur;
        fils->print(os,profondeur+1);        
        for (k=0;k<profondeur;k++)
            os<<" ";
        os<<"}"<<Endl;
        for (k=0;k<profondeur;k++)
            os<<" ";
    }
    
    int cr=1;
    for (int i=0;i<arcs.dernier;i++) {
        arcs[i]->print(os,etat,profondeur);
        if (arcs[i]->etatarbre->etatfin==1 || arcs[i]->etatarbre->marque==1)
            cr=0;
        if (i<arcs.dernier-1) {
            for (k=0;k<profondeur;k++)
                os<<" ";
        }
    }
    

    if (cr==1 && (profondeur==0 || etatfin!=0))
        os<<Endl;

}

void ArcArbre::print(ostream& os,int id,int profondeur) {
    int taille=2;
    char ch[20];
    char chh[20];
    
    char prev=marque;
    marque=1;
    strcpy_s(ch,20,"-(");
    if (negation==1) {
        strcat_s(ch,20,"~");
        taille++;
    }
    strcat_s(ch,20,X->nom);
    ch[taille+5]=0;
    sprintf_s(chh,20,"%d",num);
    strcat_s(ch,20,chh);
    if (argument!=-1) {
        sprintf_s(chh,20,"#%d",argument);
        strcat_s(ch,20,chh);
    }
    if (type==2)
        strcat_s(ch,20,"^");
    strcat_s(ch,20,")-");
    os<<ch;
    if (etatarbre->marque==-1)
        etatarbre->print(os,profondeur+strlen(ch));    
    else {
        if (type==1)
            os<<"<b";
        else
            os<<"<g";

        switch (etatarbre->etatfin) {
        case 0:
            os<<etatarbre->etat<<">";
            break;
        case 1:
            os<<"e"<<etatarbre->etat<<">";
            break;
        case 2:
            os<<"x"<<etatarbre->etat<<">";
        }            
        os<<Endl;
    }
}


