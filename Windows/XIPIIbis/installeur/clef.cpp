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
   filename   : clef.cxx
   Date       : 10/01/1999
   Purpose    : Compute the value of the encryption key
   Programmer : Claude ROUX
   Reviewer   : 
*/
#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "xipvecta.h"
#include <ctype.h>
#include <time.h>

#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int LIGNES;
int LIGNESERREUR;
int NBCARS;

#define OUI 1
#define NON 0

void erreur(char* message) {
    cerr<<message<<endl;
    exit(-1);
}
/*-------------------------------------------------------------------------*/
void Trim(char* chaine) {
    unsigned char* buf=(unsigned char*)chaine;
    int l,i;
    l=strlen(chaine);
    i=l-1;    
    while (i>=0 && buf[i]<=32)
        buf[i--]=0;
    i=0;
    while (buf[i]<=32 && buf[i]!=0)
        i++;
    if (i)
        strcpy(chaine,chaine+i);
}

/*-------------------------------------------------------------------------*/
char* DupliqueChaine(char* m) {
  char* c;
  int l=strlen(m);
  c= new char[l+1];
  memcpy(c,m,l);
  c[l]=0;
  return c;
}


/*--------------------------------------------------------------------------*/
void libere(char* c) {
    delete[] c;
}

/*-------------------------------------------------------------------------*/
char VerifieEtCompare(char* tampon,CString& message) {
    struct tm *newtime;
    time_t long_time;
    
    time( &long_time );                /* Get time as long integer. */
    newtime = localtime( &long_time ); /* Convert to local time. */

    int anneE;
    int moiS;
    int jouR;
    char ch[100];
    //On recherche le premier chiffre
    int i=0;
    while (tampon[i]<'0' && tampon[i]>'9') i++;
    char* pt=strchr(tampon+i,'/');
    if (pt==NULL) {
        message="Bad date format: error";
        return 0;
    }
    anneE=atoi(tampon+i)-1900; //la valeur de l'annee est 100 pour 2000...
    pt++;
    char* suite=strchr(pt,'/');
    *suite=0;
    moiS=atoi(pt)-1;
    jouR=atoi(suite+1);
    
    if (anneE > newtime->tm_year)
        return 1;
    if (anneE == newtime->tm_year) {
        if (moiS > newtime->tm_mon)
            return 1;
        if (moiS == newtime->tm_mon) {
            message="License will expired in: ";
            sprintf(ch,"%d",jouR-newtime->tm_mday);
            message+=ch;
            message+=". Please contact Xerox to renew...";
            if (jouR >= newtime->tm_mday)
                return 1;
        }
    }
    message="License has Expired...";
    return 0;
}
    
/****************************************************************************************************/
#define LitUnCar(x) x=fichier.get();NBCARS++;if (x=='\n') LIGNES++;
/****************************************************************************************************************/
char lespace(unsigned char c) {
    if (c==' ' || c== '\t' || c=='\n')
        return OUI;
    return NON;
}

/*****************************************************************************************************/
char lettreBis(unsigned char c)
 {
     if (c<=32 || strchr("<>{}[](),;:.&|!/\\=~#@^?+-*$%'",c))
        return NON;

     return c;
 }

/*****************************************************************************************************/
char lettre(unsigned char c) {

    if (c<=32 || strchr("<>{}[](),;:.&|!/\\=~#@^",c))
        return NON;

    return tolower(c);
}
/****************************************************************************************/

/* Cette fonction lit les commentaires...*/
char LitCommentaires(ifstream& fichier,unsigned char cc,unsigned char* r) {
    char c=cc;

    if (c=='/') {
        LitUnCar(c);

        //C'est la lecture d'une ligne
        if (c=='/') {
            while (!fichier.eof() && c!='\n') {LitUnCar(c);}
        }
        else
            while (!fichier.eof() && c!='\\') {LitUnCar(c);}

        LitUnCar(c);
        while (!fichier.eof() && lespace(c)) {LitUnCar(c);}
        *r=c;
        return OUI;
    }

    *r=c;
    return NON;
}


/****************************************************************************************************/
/*On lit certaines chaines de caractères dont on se servira pour 
construire 
les attributs, les valeurs etc...*/
char lit(ifstream& fichier,char *tampon) {
    int i=0;
    unsigned char c,k;
   
    LitUnCar(c);

    while (!fichier.eof() && lespace(c)) {LitUnCar(c);}

    unsigned char cc=c;
    while (LitCommentaires(fichier,c,&cc)) {
        c=cc;
    }

    LIGNESERREUR=LIGNES+1;

    //Sequence d'echappement
    if (cc=='\\') {
        LitUnCar(k);    
        tampon[i++]=k;
        LitUnCar(cc);
    }
    k=lettre(cc);

    while (!fichier.eof() && k) {
        tampon[i] = k;
        LitUnCar(c);
        //Sequence d'echappement
        if (c=='\\') {
            LitUnCar(k);
            tampon[++i]=k;
            LitUnCar(c);
        }

        k=lettre(c);
        i++;
    }
    tampon[i]='\0';
    if (c==32 || c=='\n') 
        while (!fichier.eof() && lespace(c)) {
            
            LitUnCar(c);
        }

    if (fichier.eof()) return ':';
    return c;
}
/****************************************************************************************************/
char alettre(unsigned char c) {
    if (c<=32 || strchr("<>{}[](),;:.&|!/\\=~#@^$%",c))
        return NON;

    return c;
}


/****************************************************************************************************/
char litSans(ifstream& fichier,char *tampon) {
    int i=0;
    unsigned char c,k;
   
    LitUnCar(c);

    while (!fichier.eof() && lespace(c)) {LitUnCar(c);}
  
    unsigned char cc=c;
    while (LitCommentaires(fichier,c,&cc)) {
        c=cc;
    }
    
    LIGNESERREUR=LIGNES+1;

    //Sequence d'echappement
    if (cc=='\\') {
        LitUnCar(k);    
        tampon[i++]=k;
        LitUnCar(cc);
    }

    k=alettre(cc);

    while (!fichier.eof() && k) {
        tampon[i] = k;
        LitUnCar(c);
        //Sequence d'echappement

        if (c=='\\') {
            LitUnCar(k);
            tampon[++i]=k;
            LitUnCar(c);
        }

        k=alettre(c);
        i++; 
    }
    
    tampon[i]='\0';
    if (c==32 || c=='\n') 
        while (!fichier.eof() && lespace(c)) {LitUnCar(c);}
    if (fichier.eof()) return ':';
    return c;
}
/****************************************************************************************************/
char Llit(ifstream& fichier,char *tampon) {
    int i=0;
    unsigned char c,k;

    LitUnCar(c);
   
    while (!fichier.eof() && lespace(c)) {LitUnCar(c);}

    unsigned char cc=c;
    while (LitCommentaires(fichier,c,&cc)) {
        c=cc;
    }

    LIGNESERREUR=LIGNES+1;

    //Sequence d'echappement
    if (cc=='\\') {
        LitUnCar(k);    
        tampon[i++]=k;
        LitUnCar(cc);
    }

    k=lettre(cc);


    while (!fichier.eof() && k) {
        tampon[i] = k;
        LitUnCar(c);    
        //Sequence d'echappement
        if (c=='\\') {
            LitUnCar(k);
            tampon[++i]=k;
            LitUnCar(c);  
        }
    
        k=lettre(c);
        i++;
    }

    tampon[i]='\0';
    if (fichier.eof()) 
        return ':';
    return c;
}

/****************************************************************************************************/

char Lettre(unsigned char c) {
    
    if (c<=32 || strchr("<>{}[](),;:.&|!/\\=~#@^?+*%",c))
        return NON;


    return tolower(c);
}

/****************************************************************************************************/
/* On lit dans une chaine de caractère le mot suivant*/
char lit(char *tampon,char* laPhrase,int* i) {
    int j=0;
    char k;

    while (laPhrase[*i]!='\x0' && (!Lettre(laPhrase[*i]) || lespace(laPhrase[*i]))) ++*i;
    k=Lettre(laPhrase[*i]);

    while (laPhrase[*i]!='\x0' && laPhrase[*i]!='.' && k) {
        tampon[j] = k;
        j++;
        ++*i;
        k=Lettre(laPhrase[*i]);
    }

    tampon[j]='\0';

    if (laPhrase[*i]=='\0') 
        return OUI;

    return NON;
}
        
/****************************************************************************************************/
/****************************************************************************************************/
void LitLigne(ifstream& fichier,char *tampon) {
    int i=0;
    unsigned char c,k;
   
    LitUnCar(c);

    while (!fichier.eof() && lespace(c)) {LitUnCar(c);}

    unsigned char cc=c;
    while (LitCommentaires(fichier,c,&cc)) {
        c=cc;
    }

    LIGNESERREUR=LIGNES+1;

    k=cc;
    while (!fichier.eof() && k!='\n') {
        tampon[i] = k;
        LitUnCar(k);
        i++;        
    }
 
    tampon[i]='\0';
}
/****************************************************************************************************/
void CheckCRC(VECTA<char*>& contents,CString& clefresultat)
{
    const size_t size = 16;

    const size_t contents_length = contents.dernier;
    const size_t number_of_row = (contents_length%size);

    unsigned char colBuffer[size] = "INTERNAL ERROR ";

    unsigned long rowBuffer = 0; 
    // the columns
    size_t num_row = 0;
    size_t i;

    for (i = 0; i < contents_length; i++)
    {

        for (size_t lettre=0;lettre < strlen(contents[i]);lettre++) {
        
            size_t i_col = (i+lettre)%size;

            if (i_col == 0 && i > 0) 
            {
                unsigned char sum_line = (rowBuffer << 4) + (rowBuffer >> 4);
                // start of a new line
                for (size_t j = 0; j < size; j++)
                    colBuffer[j] += sum_line;
                rowBuffer = 0;
            }
            
            colBuffer[i_col] += contents[i][lettre];
            rowBuffer += contents[i][lettre];
        }
    }

    char res[1000];
    res[0]=0;
    for (i = 0; i < 8; i++)
    {
        const size_t base = 41;
        unsigned short s = (((unsigned short)colBuffer[2 * i]) << 8) | colBuffer[(2 * i) + 1];
        char conv[4] = "000";
        size_t j = 2;
        while (s != 0)
        {
            char c = s % base;
            if (c < 26)
                c += 'A';
            else
                c += 'a' - 26;
            conv[j] = c;
            j--;
            s /= base;
        }
        conv[3] = '\0';
        strcat(res,conv);
    }

    clefresultat=res;

}

char ChainagePlusieurs(ifstream& fichier,
                       char* ChaineTest,
                       char* tampon,
                       char& k,
                       char& ajouteFichier,
                       VECTA<char*>& noms,
                       char* Utiliselimite) {

    char nomFichier[255];
     
    if (tampon[0]==0)
        k=Llit(fichier,tampon);
    
    if (tampon[0]==0 && k==':')
        return 0;

    if (strcmp(tampon,ChaineTest) || k!=':')
        erreur(tampon);

    //Une liste se finit par un ";"
 
    while (k!=';') {    
        k=lit(fichier,tampon);

        strcpy(nomFichier,tampon);
        //Then there should be a number
        if (!memcmp(nomFichier,(char*)"limited",7) || !strcmp(nomFichier,"noparameter"))
            ajouteFichier=OUI;
            

        if (!memcmp(nomFichier,(char*)"limited",7)) {
           if (k!=':')
               return 0;
           strcat(nomFichier,":");
           k=lit(fichier,tampon);
           strcat(nomFichier,tampon);
        }


        Trim(nomFichier);
        noms.ajoute(DupliqueChaine(nomFichier));
        unsigned char cc=k;
        while (LitCommentaires(fichier,k,&cc)) {
            k=cc;
        }

        if (k==':') {
            tampon[0]=0;
            return 1;
        }

        if (k!=',') {
            tampon[0]=tolower(k);
            k=Llit(fichier,tampon+1);
            return 1;
        }

    }
    tampon[0]=0;
    return 1;
}



int ExtraitClef(char* nomfichier,int& debut,int& fin,VECTA<char*>& CalculClef,char* Utiliselimite) {
    
    /*-------------------------------------------------------------------------*/
    char k,tampon[300];
    
    ifstream fichier(nomfichier,ios::in);
    
    LIGNES=0;
    LIGNESERREUR=0;
    NBCARS=0;
    
    if (fichier.fail() )
        return -1;
    
    
    
    
    int rec=strlen(nomfichier)-1;
    while (rec>=0 && nomfichier[rec]!='/' && nomfichier[rec]!='\\') rec--;
    strcpy(Utiliselimite,nomfichier+rec+1);
    debut=NBCARS;
    Trim(Utiliselimite);

    k=Llit(fichier,tampon); 
    debut=NBCARS-9;
    if (strcmp(tampon,"license"))
        return 0;
    
    
    //On lit le reste de la ligne
    LitLigne(fichier,tampon);
    Trim(tampon);
    CalculClef.ajoute(DupliqueChaine(tampon));
    
    tampon[0]=0;
    k=0;
    char ajoute=NON;
    if (!ChainagePlusieurs(fichier,"modules",tampon,k,ajoute,CalculClef,Utiliselimite))
        return 0;
    
    //On lit maintenant la date d'expiration en format americain: Annee/mois/jour
    if (strcmp(tampon,"expiration"))
        return 0;

    LitLigne(fichier,tampon);    
    Trim(tampon);
    if (ajoute==1)    
        CalculClef.ajoute(DupliqueChaine(Utiliselimite));             

    CalculClef.ajoute(DupliqueChaine(tampon));             

    LitLigne(fichier,tampon);
    fin=NBCARS;

    return OUI;
}

char InsereClef(char* nomfichier, CString clef,int debut,int fin,VECTA<char*>& CalculClef,char* Utiliselimite) {
    char k;
  
    ifstream fichier(nomfichier,ios::in);
    ostringstream sortie;
    LIGNES=0;
    LIGNESERREUR=0;
    NBCARS=0;
    while (NBCARS<=debut) {
        LitUnCar(k);
        sortie<<k;
    }

    sortie<<"License: "<<CalculClef[0]<<endl;
    sortie<<"Modules: ";
    char virg=0;
    for (int i=1;i<CalculClef.dernier-1;i++) {
        if (!strcmp(CalculClef[i],Utiliselimite))
            continue;
        if (virg==1)
            sortie<<",";
        virg=1;
        sortie<<CalculClef[i];
    }
    sortie<<endl;
    sortie<<"Expiration: "<<CalculClef.fin()<<endl;
    
    sortie<<"code: "<<LPCTSTR(clef)<<endl;
    while (NBCARS<fin) {
        LitUnCar(k);
    }

    char c=fichier.get();
    while (!fichier.eof()) {        
        sortie<<c;
        c=fichier.get();
    }
    fichier.close();
    ofstream fsortie(nomfichier,ios::binary);
    fsortie<<sortie.str().c_str();
    return 1;
}
