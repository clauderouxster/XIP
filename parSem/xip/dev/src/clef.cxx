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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "xipvecta.h"
#include <iostream>
#include <fstream>


using namespace std;

int LIGNES;
int LIGNESERREUR;
#define OUI 1
#define NON 0

void erreur(char* message) {
    cerr<<message<<endl;
    exit(-1);
}
/*-------------------------------------------------------------------------*/
void Trim(char* chaine) {
    unsigned char* buf=(unsigned char*)chaine;
    size_t l,i;
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
  size_t l=strlen(m);
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
void VerifieEtCompare(char* tampon) {
    struct tm *newtime;
    time_t long_time;
    
    time( &long_time );                /* Get time as long integer. */
    newtime = localtime( &long_time ); /* Convert to local time. */

    int anneE;
    int moiS;
    int jouR;
    //On recherche le premier chiffre
    int i=0;
    while (tampon[i]<'0' && tampon[i]>'9') i++;
    char* pt=strchr(tampon+i,'/');
    if (pt==NULL)
        erreur("Bad date format: error");
    anneE=atoi(tampon+i)-1900; //la valeur de l'annee est 100 pour 2000...
    pt++;
    char* suite=strchr(pt,'/');
    *suite=0;
    moiS=atoi(pt)-1;
    jouR=atoi(suite+1);
    
    if (anneE > newtime->tm_year)
        return;
    if (anneE == newtime->tm_year) {
        if (moiS > newtime->tm_mon)
            return;
        if (moiS == newtime->tm_mon) {
            cerr<<"License will expired in: "<< jouR-newtime->tm_mday<<". Please contact Xerox to renew..."<<endl;
            if (jouR >= newtime->tm_mday)
                return;
        }
    }
    cerr<<"License has Expired..."<<endl;
}
    
/****************************************************************************************************/
#define LitUnCar(x) x=fichier.get();if (x=='\n') LIGNES++;
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
/*On lit certaines chaines de caract�res dont on se servira pour 
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
/* On lit dans une chaine de caract�re le mot suivant*/
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
inline void CheckCRC(VECTA<char*>& contents)
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

    cerr<<"Code= "<<res<<endl;

}

void ChainagePlusieurs(ifstream& fichier,
                       char* ChaineTest,
                       char* tampon,
                       char& k,
                       char& ajouteFichier,
                       VECTA<char*>& noms) {

    char nomFichier[255];
     
    if (tampon[0]==0)
        k=Llit(fichier,tampon);
    
    if (tampon[0]==0 && k==':')
        return;

    if (strcmp(tampon,ChaineTest) || k!=':')
        erreur(tampon);

    //Une liste se finit par un ";"
 
    while (k!=';') {    
        k=lit(fichier,tampon);

        strcpy_s(nomFichier,255,tampon);
        if (!memcmp(nomFichier,(char*)"limited",7) || !strcmp(nomFichier,"noparameter"))
            ajouteFichier=OUI;

        Trim(nomFichier);
        noms.ajoute(DupliqueChaine(nomFichier));
        unsigned char cc=k;
        while (LitCommentaires(fichier,k,&cc)) {
            k=cc;
        }

        if (k==':') {
            tampon[0]=0;
            return;
        }

        if (k!=',') {
            tampon[0]=tolower(k);
            k=Llit(fichier,tampon+1);
            return;
        }

    }
    tampon[0]=0;
}


int main(int argc, char *argv[]) {

/*-------------------------------------------------------------------------*/
    char k,tampon[500];

    if (argc < 2) {
        cerr<<"Erreur pas de nom de fichier sur la ligne de commande..."<<endl;
        exit(-1);
    }
    ifstream fichier(argv[1],ios::in|ios::binary);

    LIGNES=0;
    LIGNESERREUR=0;
    
    if (fichier.fail() ) {
        strcpy_s(tampon,500,argv[1]);
        erreur(tampon);
    }
    else {
     
        VECTA<char*> CalculClef;

        char Utiliselimite[300];
        int rec=strlen(argv[1])-1;
        while (rec>=0 && argv[1][rec]!='/' && argv[1][rec]!='\\') rec--;
        strcpy_s(Utiliselimite,300,argv[1]+rec+1);

        k=Llit(fichier,tampon);    
        if (strcmp(tampon,"license"))
            erreur("No license found... Error..");
        //On lit le reste de la ligne
        LitLigne(fichier,tampon);
        Trim(tampon);
        CalculClef.ajoute(DupliqueChaine(tampon));

        tampon[0]=0;
        k=0;
        char ajoute=NON;
        ChainagePlusieurs(fichier,"modules",tampon,k,ajoute,CalculClef);
        if (ajoute==OUI) {
            cerr<<"On rajoute:"<<Utiliselimite<<endl;
            Trim(Utiliselimite);
            CalculClef.ajoute(DupliqueChaine(Utiliselimite));
        }

        int imo;
                                     
        //On lit maintenant la date d'expiration en format americain: Annee/mois/jour
        if (strcmp(tampon,"expiration"))
            erreur("No expiration date found... Error..");     
        LitLigne(fichier,tampon);
        Trim(tampon);
        CalculClef.ajoute(DupliqueChaine(tampon));     
        VerifieEtCompare(tampon);
        CheckCRC(CalculClef);

        //On nettoie cette structure...
        for (imo=0;imo<CalculClef.dernier;imo++)
            libere(CalculClef[imo]);
        CalculClef.raz();
    }
    return 0;
}
