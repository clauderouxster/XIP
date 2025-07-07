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
   filename   : lire.h
   Date       : 21/09/1992
   Purpose    : Reading of characters from a file when compiling a grammar
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef lire_h
#define lire_h

char lettre(unsigned char c);
char lettreBis(unsigned char c);
char LitCommentaires(ifstream& fichier,unsigned char c,unsigned char*);
char Lettre(unsigned char c);


inline unsigned char estCar(unsigned char c) {
    if (strchr("ιθκο",c))
        return c;
    return isalpha(c);
}

#endif
