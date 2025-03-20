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
   filename   : divers.h
   Date       : 10/01/1999
   Purpose    : String methods
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef divers_h
#define divers_h

#include <ctype.h>
#include <string.h>

class GlobalParseur;

char* DupliqueChaine(char* m);
char* DupliqueChaine(const char* m);
unsigned char* DupliqueChaine(unsigned char* m);
char* Majuscule(char* m);
unsigned char* Majuscule(unsigned char* m);
char* Concatene(char* tamp1,char* tamp2);
int ChaineVersNombre(char* chiffre);
void NombreVersChaine(char* p,int x);
char* ConversionChaine(GlobalParseur*,char* chaine);

#define uchar unsigned char

class chaine
{public:
 unsigned char *c;

  chaine(char*);
  chaine(unsigned char*);
  ~chaine();
  char egal(char* ch);
  char superieur(char*);
 friend char operator <(chaine&,chaine&);
 friend char operator <=(chaine&,chaine&);
 friend char operator ==(chaine& clef,chaine& co);
};


#ifdef UNIX
#ifndef sprintf_s
#define sprintf_s snprintf
#define vsprintf_s vsnprintf
#endif
#endif

/*--------------------Lecture d'une chaîne de caractère----------------------------*/


class Lecture {
 public:
  char* tampon;
  
  Lecture(char* phrase);
  
      
  friend ostream& operator<< (ostream& os,Lecture& Chaine);


  friend ifstream& operator>> (ifstream&,Lecture&);
};
  
void Espace(char i);

//compare deux chaines de caracteres sans tenir compte de la casse
inline char compchmin(unsigned char* mot1, unsigned char* mot2) {
    unsigned char c1=tolower(*mot1++);
    unsigned char c2=tolower(*mot2++);
    
    while (c1==c2 && c1 && c2) {
        c1=tolower(*mot1++);
        c2=tolower(*mot2++);
    }
    
    if (c1 < c2)
        return -1;
    
    if (c1 > c2)
        return 1;
    
    return 0;
}

//compare deux chaines de caracteres sans tenir compte de la casse
inline char compchmin(int* mot1, unsigned char* mot2) {
    unsigned char c1=tolower(255&*mot1++);
    unsigned char c2=tolower(*mot2++);
    
    while (c1==c2 && c1 && c2) {
        c1=tolower(255&*mot1++);
        c2=tolower(*mot2++);
    }
    
    if (c1 < c2)
        return -1;
    
    if (c1 > c2)
        return 1;
    
    return 0;
}


inline char compch(unsigned char* mot1, unsigned char* mot2) {
    unsigned char c1=*mot1++;
    unsigned char c2=*mot2++;
    
    while (c1==c2 && c1 && c2) {
        c1=*mot1++;
        c2=*mot2++;
    }
    
    if (c1 < c2)
        return -1;
    
    if (c1 > c2)
        return 1;
    
    return 0;
}

inline char compchsign(char* mot1,char* mot2) {
    unsigned char c1=(uchar)*mot1++;
    unsigned char c2=(uchar)*mot2++;
    
    while (c1==c2 && c1 && c2) {
        c1=(uchar)*mot1++;
        c2=(uchar)*mot2++;
    }
    
    if (c1 < c2)
        return -1;
    
    if (c1 > c2)
        return 1;
    
    return 0;
}


inline void MetEnMinuscule(unsigned char* iterateur) {
    while (*iterateur!=0) {
		*iterateur=tolower(*iterateur);
        ++iterateur;
    }
}

#define STR(x) (char*)x.c_str()
#define USTR(x) (uchar*)x.c_str()
#define XSTR(x) (xmlChar*)x.c_str()


#endif
