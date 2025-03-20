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
   filename   : divers.cxx
   Date       : 10/01/1999
   Purpose    : string and error handling
   Programmer : Claude ROUX
   Reviewer   : 
*/


#include "parsbin.h"
#include "conversion.h"

extern GlobalParseur* parseur_courant;

extern ostream* flot_erreur;
XIPError* xip_derniere_erreur=NULL;

Exportation XIPError* XipGetLastError() {
	return xip_derniere_erreur;
}

Exportation void XipCleanLastError() {
	if (xip_derniere_erreur!=NULL)
		delete xip_derniere_erreur;
	xip_derniere_erreur=NULL;
}

void Nettoyage();
/*-------------------------------------------------------------------------*/
int ERREUR_LIGNES=-1;
char ERREUR_FICHIER[1000];
long ERREUR_POSITION_DANS_FICHIER=-1;

void Recuperenom(char* nom,char* path) {
	char base[1024];	
	int i;
	int pslash=-1;
	for (i=0;nom[i]!=0;i++) {
		base[i]=nom[i];
		if (nom[i]==SEP)
			pslash=i;
	}
	base[i]=0;
    if (pslash==-1)
        strcpy(path,DEB);
    else {
		for (i=0;i<pslash;i++)
			path[i]=base[i];
		path[pslash]=0;
		for (i=pslash+1;base[i]!=0;i++)
			nom[i-pslash-1]=base[i];
		nom[i-pslash-1]=0;
    }
}

void kif_erreur(string message,int ligne) {
    struct tm *newtime;
    time_t long_time;
    time( &long_time );                /* Get time as long integer. */
    newtime = localtime( &long_time ); /* Convert to local time. */
    
    xip_derniere_erreur=new XIPError(STR(message),"kif",asctime(newtime),ligne,ligne);
}


void lance_erreur(char* message) {
    struct tm *newtime;
    time_t long_time;
    time( &long_time );                /* Get time as long integer. */
    newtime = localtime( &long_time ); /* Convert to local time. */
    
    XIPError* xiperror=new XIPError(message,ERREUR_FICHIER,asctime(newtime),ERREUR_LIGNES,ERREUR_POSITION_DANS_FICHIER);
	xip_derniere_erreur=xiperror;
    throw xiperror;   
}

void GlobalParseur::erreur(char* Message) {
    
    ERREUR_SURVENUE=-1;
    struct tm *newtime;
    time_t long_time;
    time( &long_time );                /* Get time as long integer. */
    newtime = localtime( &long_time ); /* Convert to local time. */

    if (NOM!=NULL)
        strcpy(ERREUR_FICHIER,NOM);

    ERREUR_LIGNES=LIGNESERREUR;
    ERREUR_POSITION_DANS_FICHIER=POSITION_DANS_FICHIER;

    XIPError* xiperror=new XIPError(Message,NOM,asctime(newtime),LIGNESERREUR,POSITION_DANS_FICHIER);
	xip_derniere_erreur=xiperror;

    int ligne;

    ligne=LIGNESERREUR;

    Chaine_traits[0]=0;

#ifdef XIPSTLUSE
    ostringstream erreurstream;
#else
    ostrstream erreurstream(Chaine_traits,3000);
#endif

    if (NOM != NULL) {
        erreurstream <<asctime( newtime )<<"  "<<CHAINES[152] << NOM;
        if (ligne!=-1)
            erreurstream << "("<< CHAINES[153] << ligne <<")";
        erreurstream<<Endl<<Endl;
    }

    erreurstream << Message<<Endl<<Endl;
    *flot_erreur<< Message << Endl <<Endl;    

#ifdef XIPSTLUSE
    *flot_erreur<<erreurstream.str();
    strcpy(Chaine_traits,(char*)erreurstream.str().c_str());
#else
    Chaine_traits[erreurstream.pcount()]=0;
    *flot_erreur<<Chaine_traits;
#endif

    cryptage=encoursCryptage;
    
    throw xiperror;
}
/*-------------------------------------------------------------------------*/
void GlobalParseur::insere_erreur(char* Message) {


    struct tm *newtime;
    time_t long_time;
 
 
    time( &long_time );                /* Get time as long integer. */
    newtime = localtime( &long_time ); /* Convert to local time. */
    
   
}

/*-------------------------------------------------------------------------*/
void MettreEnMinuscule(unsigned char* iterateur) {
	string s=c_Tolower((char*)iterateur);
	strcpy((char*)iterateur,(char*)s.c_str());
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

char* DupliqueChaine(const char* m) {
  char* c;
  int l=strlen(m);
  c= new char[l+1];
  memcpy(c,m,l);
  c[l]=0;
  return c;
}

uchar* DupliqueChaine(uchar* m) {
    return (uchar*)DupliqueChaine((char*)m);
}
/*-------------------------------------------------------------------------*/
char* Majuscule(char* m) {
    int l=strlen(m);
    for (int i=0;i<=l;i++)
        m[i]=toupper(m[i]);
    return m;
}
/*-------------------------------------------------------------------------*/
unsigned char* Majuscule(unsigned char* m) {
    int l=strlen((char*)m);
    for (int i=0;i<=l;i++) {
        if (m[i] >= 'a' && m[i] <='z') 
            m[i]=m[i]-32;
        else 
            m[i]=m[i];
    }
    return m;
}
/*-------------------------------------------------------------------------*/
void AjouteChaine(char* chaine,char* ajout)
 {int i;
  i=chaine[0]+1;
  strcpy(&chaine[i],ajout);
  i+=strlen(ajout);
  chaine[i]='\0';
  chaine[0]=i-1;}
  
/*-------------------------------------------------------------------------*/
char* Concatene(char* tamp1,char* tamp2) {
    static char Nom[1000];
  
    sprintf(Nom,"%s%s",tamp1,tamp2);
    //strcpy(Nom,tamp1);
    //strcat(Nom,tamp2);
    return Nom;
 }
/*-------------------------------------------------------------------------*/
int ChaineVersNombre(char* chiffre)
 {int k=chiffre[0]-48;
  short i=1;
  
  while (chiffre[i]) {k*=10;k+=chiffre[i]-48;i++;}
  return k;}
  
/*---------------------------------------------------------------------------------*/
void NombreVersChaine(char* p,int x)
 {int i,j;
 
  i=0;j=x;
  while (j>=10)
   {i++;j=j/10;}
  j=x;p[i+1]='\x0';
  while (j>=10)
   {p[i]=48+(j % 10);
    i--;
	j=j/10;}
  p[i]=48+(j%10);}
/*-------------------------------------------------------------------------*/

char chaine::egal(char* ch) {return (!compchmin(c,(unsigned char*)ch));}

chaine::chaine(char* m) {	
    c=(unsigned char*)DupliqueChaine(m);
}

chaine::chaine(unsigned char* m) {	
    c=dupch(m);
}

chaine::~chaine()  {
    libere(c);
}

char chaine::superieur(char* co) {  
    int test;
 
    test=compchmin(c,(unsigned char*)co);

    if (test==1)
        return 1;

    return 0;
}

char operator ==(chaine& clef,chaine& co) {
    return (compchmin(clef.c,co.c));
}

char operator <(chaine& clef,chaine& co) {
    int test;
 
    test=compchmin(clef.c,co.c);

    if (test==-1)
        return 1;

    return 0;
}

char operator <=(chaine& clef,chaine& co) {
    int test;

    test=compch(clef.c,co.c);

    if (test==-1 || test==0)
        return 1;
    return 0;
}
 
/*--------------------Lecture d'une chaîne de caractère----------------------------*/



Lecture::Lecture(char* phrase) {tampon=phrase;}
  
ostream& operator<< (ostream& os,Lecture& Chaine)
     {return os << Chaine.tampon;}


/*-------------------------------------------------------------------------*/
void GlobalParseur::Espace(char i)
 {int j;
  for (j=0;j<i;j++) trace << " ";}
/*-------------------------------------------------------------------------*/



 	void word_offset::realloue(int tail) {
		taille=tail;
		char* motnouveau=(char*)malloc(taille);
		memcpy(motnouveau,mot,longueur+1);
		free(mot);
		mot=motnouveau;
	}


    char* word_offset::concatene(char* interne) {			
		int frontiere=strlen(interne)+longueur;
		if (frontiere>=taille)
			realloue(frontiere*2);
        strcpy(mot+longueur,interne);
        longueur=frontiere;
		return mot;
    }

	char* word_offset::concatene(string& interne) {			
		int frontiere=interne.size()+longueur;
		if (frontiere>=taille)
			realloue(frontiere*2);
        strcpy(mot+longueur,(char*)interne.c_str());
        longueur=frontiere;
		return mot;
    }
    
    void word_offset::ajoute(char interne,long offset,long offsetchar) {		
		if ((longueur+1)>=taille)
			realloue(longueur*2);
        mot[longueur++]=interne;
        mot[longueur]=0;
		if (gauche==-1) {
            gauche=offset;
			gauchecar=offsetchar;
			droite=offset;
			droitecar=offsetchar;
		}
        droite++;		
		droitecar++;
    }

	void word_offset::ajoute(char* interne,long offset,long offsetchar) {		
		concatene(interne);
		if (gauche==-1) {
            gauche=offset;
			gauchecar=offsetchar;
			droite=offset;
			droitecar=offsetchar;
		}
        droite+=strlen(interne);
		droitecar++;
    }

