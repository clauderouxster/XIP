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
   filename   : readxml.cxx
   Date       : 
   Purpose    : Reading XML characters
   Programmer : Claude ROUX
   Reviewer   : 
*/


#include "parsbin.h"

/****************************************************************************************************/
#define ReadACar(x) x=filename.get();
/****************************************************************************************************/
char aspace(unsigned char c) {
    if (c==' ' || c== '\t' || c=='\n' || c==13)
        return 1;
    return 0;
}

/*****************************************************************************************************/
char validLetter(unsigned char c) {

    if (c<=32 || strchr("<>{}[](),;:.&|!/\\=~#@^",c))
        return 0;

    return tolower(c);
}

/****************************************************************************************************/

/* Cette fonction lit les commentaires...*/
char readComments(ifstream& filename,unsigned char cc,unsigned char* r) {
    char c=cc;

    if (c=='/') {
        ReadACar(c);

        //C'est la lecture d'une ligne
        if (c=='/') {
            while (!filename.eof() && c!='\n') {ReadACar(c);}
        }
        else
            while (!filename.eof() && c!='\\') {ReadACar(c);}

        ReadACar(c);
        while (!filename.eof() && aspace(c)) {ReadACar(c);}
        *r=c;
        return 1;
    }

    *r=c;
    return 0;
}
/****************************************************************************************************/
/*On lit certaines chaines de caractères dont on se servira pour 
construire 
les attributs, les valeurs etc...*/
char readString(ifstream& filename,char *buffer) {
    int i=0;
    unsigned char c,k;
   
    ReadACar(c);

    while (!filename.eof() && aspace(c)) {ReadACar(c);}

    unsigned char cc=c;
    while (readComments(filename,c,&cc)) {
        c=cc;
    }

    //Sequence d'echappement
    if (cc=='\\') {
        ReadACar(k);    
        buffer[i++]=k;
        ReadACar(cc);    
    }

    k=validLetter(cc);

    while (!filename.eof() && k) {
        buffer[i] = k;
        ReadACar(c);

        //Sequence d'echappement
        if (c=='\\') {
            ReadACar(k);
            buffer[++i]=k;
            ReadACar(c);
        }

        k=validLetter(c);
        i++;
    }
    buffer[i]='\0';
    if (c==32 || c=='\n') 
        while (!filename.eof() && aspace(c)) {
            
            ReadACar(c);
        }

    if (filename.eof()) return ':';
    return c;
}

//------------------------------------------------------------------------------
char readLeft(ifstream& filename,unsigned char *buffer,char stop) {
    int i=0;
    unsigned char c,k;

    ReadACar(c);

    while (!filename.eof() && aspace(c)) {ReadACar(c);}

    unsigned char cc=c;
    while (readComments(filename,c,&cc)) {
        c=cc;
    }

    //Sequence d'echappement
    if (cc=='\\') {
        ReadACar(k);    
    }
    else
        k=cc;

    while (!filename.eof()) {
        buffer[i] = k;
        ReadACar(k);
        i++;        
        if (k==stop)
            break;
        if (k=='\\') {
            ReadACar(k);
        }
    }
 
    buffer[i]='\0';

    if (k==':')
        return k;

    while (!filename.eof() && aspace(k)) {
        ReadACar(k);
    }
    if (filename.eof()) 
        return ':';
    return k;
}

void readLine(ifstream& filename,char *tampon) {
    int i=0;
    unsigned char c,k;

    ReadACar(c);

    while (!filename.eof() && aspace(c)) {ReadACar(c);}

    unsigned char cc=c;
    while (readComments(filename,c,&cc)) {
        c=cc;
    }

    k=cc;
    while (!filename.eof() && k!='\n') {
        tampon[i] = k;
        ReadACar(k);
        i++;        
    }
 
    tampon[i]='\0';
}

