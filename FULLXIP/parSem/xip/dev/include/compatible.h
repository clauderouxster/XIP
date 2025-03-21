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
   filename   : compatible.h
   Date       : 16/12/1999
   Purpose    : This file defines the Signature object that is used to check the compatibility between
                dependencies
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef compatible_h
#define compatible_h

#include "avltemplate.h"
class ResultatFonction;

//Cette classe est utilisee pour gerer les incompatiblilites de dependances
//Elle est principalement utilisee dans ResultatFonction et fonctionne de pair avec
//CompteurIncompatible dans GlobalParseur.
class Signature {
public:

    VECTA<TVecteur> signature;
    VECTA<TVecteur> filtre;    

    Signature(int s=1,int f=2);
    Signature(Signature& s);

    void initialise(int,int,int);
    void fusion(Signature& s);
    char compare(Signature& s);
    char compare(ResultatFonction*);
    char global(VECTA<TVecteur>& filtreglobal,Signature& s);
    void retire(Signature& s);
    void raz();
    void base(Signature& s);
    void interdit(Signature& s);
    void remplace(Signature& s);        
    void famille(int borne,VECTA<int>& arbre);
};

#endif
