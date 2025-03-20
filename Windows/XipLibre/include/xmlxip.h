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
   filename   : xmlxip.h
   Date       : 21/01/2002
   Purpose    : Derivation of GlobalParseur to handle XML input files.
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef xmlxip_h
#define xmlxip_h

class Stocke_Variable {
public:
    Variable* var;
    double val;
    ResultatFonction* rf;
    char* nom;

    Stocke_Variable(Variable* v,double vv,char* n) {
        val=vv;
        var=v;
        rf=NULL;
        nom=DupliqueChaine(n);
    };

    ~Stocke_Variable() {
        libere(nom);
    }
};

#endif

