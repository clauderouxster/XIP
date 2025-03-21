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
        strcpy_s(ERREUR_FICHIER,1000,NOM);

    ERREUR_LIGNES=LIGNESERREUR;
    ERREUR_POSITION_DANS_FICHIER=POSITION_DANS_FICHIER;

    XIPError* xiperror=new XIPError(Message,NOM,asctime(newtime),LIGNESERREUR,POSITION_DANS_FICHIER);
	xip_derniere_erreur=xiperror;

    int ligne;

    ligne=LIGNESERREUR;

    Chaine_traits[0]=0;

#ifdef XIPSTLAPI
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

#ifdef XIPSTLAPI
    *flot_erreur<<erreurstream.str();
    strcpy_s(Chaine_traits,3000,(char*)erreurstream.str().c_str());
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
	string s=s_to_lower((char*)iterateur);
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
    c=(unsigned char*)strdup((char*)m);
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
		mot = (char*)realloc(mot, taille);
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

	unsigned char* GlobalParseur::conversionUTF8VersAscii(unsigned char* contenu) {
		chainetraduction = conversion_utf8_to_latin(contenu);
		return (unsigned char*)chainetraduction.c_str();
	}



	uchar* GlobalParseur::conversionAsciiVersUTF8(uchar* contenu) {
		chainetraduction = conversion_latin_to_utf8(contenu);
		return (unsigned char*)chainetraduction.c_str();
	}

	static map<string, string> lamajuscule;
	static map<string, string> lamajusculeutf8;

	void initlamajuscule() {
		static char init = NON;
		if (init == OUI)
			return;
		init = OUI;
		lamajuscule["a"] = "A";
		lamajuscule["b"] = "B";
		lamajuscule["c"] = "C";
		lamajuscule["d"] = "D";
		lamajuscule["e"] = "E";
		lamajuscule["f"] = "F";
		lamajuscule["g"] = "G";
		lamajuscule["h"] = "H";
		lamajuscule["i"] = "I";
		lamajuscule["j"] = "J";
		lamajuscule["k"] = "K";
		lamajuscule["l"] = "L";
		lamajuscule["m"] = "M";
		lamajuscule["n"] = "N";
		lamajuscule["o"] = "O";
		lamajuscule["p"] = "P";
		lamajuscule["q"] = "Q";
		lamajuscule["r"] = "R";
		lamajuscule["s"] = "S";
		lamajuscule["t"] = "T";
		lamajuscule["u"] = "U";
		lamajuscule["v"] = "V";
		lamajuscule["w"] = "W";
		lamajuscule["x"] = "X";
		lamajuscule["y"] = "Y";
		lamajuscule["z"] = "Z";
		lamajuscule["à"] = "À";
		lamajuscule["á"] = "Á";
		lamajuscule["â"] = "Â";
		lamajuscule["ã"] = "Ã";
		lamajuscule["ä"] = "Ä";
		lamajuscule["å"] = "Å";
		lamajuscule["ç"] = "Ç";
		lamajuscule["è"] = "È";
		lamajuscule["é"] = "É";
		lamajuscule["ê"] = "Ê";
		lamajuscule["ë"] = "Ë";
		lamajuscule["ì"] = "Ì";
		lamajuscule["í"] = "Í";
		lamajuscule["î"] = "Î";
		lamajuscule["ï"] = "Ï";
		lamajuscule["ò"] = "Ò";
		lamajuscule["ó"] = "Ó";
		lamajuscule["ô"] = "Ô";
		lamajuscule["õ"] = "Õ";
		lamajuscule["ö"] = "Ö";
		lamajuscule["ù"] = "Ù";
		lamajuscule["ú"] = "Ú";
		lamajuscule["û"] = "Û";
		lamajuscule["ü"] = "Ü";
		lamajuscule["æ"] = "Æ";
		lamajuscule["ð"] = "Ð";
		lamajuscule["ñ"] = "Ñ";
		lamajusculeutf8["a"] = "A";
		lamajusculeutf8["b"] = "B";
		lamajusculeutf8["c"] = "C";
		lamajusculeutf8["d"] = "D";
		lamajusculeutf8["e"] = "E";
		lamajusculeutf8["f"] = "F";
		lamajusculeutf8["g"] = "G";
		lamajusculeutf8["h"] = "H";
		lamajusculeutf8["i"] = "I";
		lamajusculeutf8["j"] = "J";
		lamajusculeutf8["k"] = "K";
		lamajusculeutf8["l"] = "L";
		lamajusculeutf8["m"] = "M";
		lamajusculeutf8["n"] = "N";
		lamajusculeutf8["o"] = "O";
		lamajusculeutf8["p"] = "P";
		lamajusculeutf8["q"] = "Q";
		lamajusculeutf8["r"] = "R";
		lamajusculeutf8["s"] = "S";
		lamajusculeutf8["t"] = "T";
		lamajusculeutf8["u"] = "U";
		lamajusculeutf8["v"] = "V";
		lamajusculeutf8["w"] = "W";
		lamajusculeutf8["x"] = "X";
		lamajusculeutf8["y"] = "Y";
		lamajusculeutf8["z"] = "Z";
		lamajusculeutf8["Ã "] = "Ã€";
		lamajusculeutf8["Ã¡"] = "Ã";
		lamajusculeutf8["Ã¢"] = "Ã‚";
		lamajusculeutf8["Ã£"] = "Ãƒ";
		lamajusculeutf8["Ã¤"] = "Ã„";
		lamajusculeutf8["Ã¥"] = "Ã…";
		lamajusculeutf8["Ã§"] = "Ã‡";
		lamajusculeutf8["Ã¨"] = "Ãˆ";
		lamajusculeutf8["Ã©"] = "Ã‰";
		lamajusculeutf8["Ãª"] = "ÃŠ";
		lamajusculeutf8["Ã«"] = "Ã‹";
		lamajusculeutf8["Ã¬"] = "ÃŒ";
		lamajusculeutf8["Ã­"] = "Ã";
		lamajusculeutf8["Ã®"] = "ÃŽ";
		lamajusculeutf8["Ã¯"] = "Ã";
		lamajusculeutf8["Ã²"] = "Ã’";
		lamajusculeutf8["Ã³"] = "Ã“";
		lamajusculeutf8["Ã´"] = "Ã”";
		lamajusculeutf8["Ãµ"] = "Ã•";
		lamajusculeutf8["Ã¶"] = "Ã–";
		lamajusculeutf8["Ã¹"] = "Ã™";
		lamajusculeutf8["Ãº"] = "Ãš";
		lamajusculeutf8["Ã»"] = "Ã›";
		lamajusculeutf8["Ã¼"] = "Ãœ";
		lamajusculeutf8["Ã¦"] = "Ã†";
		lamajusculeutf8["Ã°"] = "Ã";
		lamajusculeutf8["Ã±"] = "Ã‘";
	}

	//--------------------------------------------------------------------------------------

	void GlobalParseur::RenvoieMaj(string& s) {
		string pr;
		string prm;
		if (encodage_fst == UTF8_ENCODING) {
			pr = s.substr(0, 2);
			prm = lamajusculeutf8[pr];
			if (prm == "") {
				pr = s.substr(0, 1);
				prm = lamajusculeutf8[pr];
				s.replace(0, 1, prm);
			}
			else
				s.replace(0, 2, prm);
		}
		else {
			pr = s.substr(0, 1);
			prm = lamajuscule[pr];
			if (prm != "")
				s.replace(0, 1, prm);
		}
	}


	//--------------------------------------------------------------------------------------
	void Trim(char* chaine) {
		unsigned char* buf = (unsigned char*)chaine;
		int l, i;
		l = strlen(chaine);
		i = l - 1;
		while (i >= 0 && buf[i] <= 32)
			buf[i--] = 0;
		i = 0;
		while (buf[i] <= 32 && buf[i] != 0)
			i++;
		if (i)
			strcpy(chaine, chaine + i);
	}
	//--------------------------------------------------------------------------------------
	void strcatchr(char* tmp, char c) {
		int l = strlen(tmp);
		tmp[l] = c;
		tmp[l + 1] = 0;
	}

	void strcatchr(GlobalParseur* parseur, int max, char* tmp, char c) {
		int l = strlen(tmp);
		if (l >= max)
			parseur->erreur(parseur->CHAINES[336]);
		tmp[l] = c;
		tmp[l + 1] = 0;
	}


