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
   filename   : lire.cxx
   Date       : 10/01/1999
   Purpose    : Reading of a character (along with the decyphering if necessary)
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"

extern ostream* flot_erreur;
extern char tamponlecturetrace[5000];
extern int positiontamponlecturetrace;
void strcatchr(char* tmp,char c);


static unsigned char ecry(unsigned char c,unsigned long boucle) {
    uchar val= ((~boucle)<<2)%256;
    c+=(char)val;
    return c;
}


/****************************************************************************************************/
void GlobalParseur::RemetCaractere(unsigned char c) {SUITE=c;}
/****************************************************************************************************/
unsigned char GlobalParseur::LectCrypte(istream& fichier) {
    unsigned char c;
    char nePasCrypter=NON;
    /*
    static char car=8;
    static char curseur='/';

    cout<<curseur<<car;
    switch (curseur) {
    case '/':
        curseur='-';
        break;
    case '-':
        curseur='\\';
        break;
    case '\\':
        curseur='/';
    }
    */

    if (SUITE==-1) {
        if (enAttente[0]!=0) {
            int l=strlen(enAttente);
            c=enAttente[l-1];
            enAttente[l-1]=0;
            nePasCrypter=OUI;
        }
        else {
            c=fichier.get();
            POSITION_DANS_FICHIER++;
			if (c==255 && fichier.eof())
				return c;
        }
    }
    else {
        c=SUITE;
        SUITE=-1;
        return c;
    }

    if (cryptage==NON || nePasCrypter==OUI) {
        if (TRACEREGLE==2 && Trace!=NULL)
            *Trace<<c;
        if (c=='\n') LIGNES++;        
        return c;   
    }

    bouclage++;    
    uchar val= ((~bouclage)<<2)%256;

    if (cryptage==OUI)
        c-=(char)val;
    else
        c+=(char)val;

    if (c=='\n') LIGNES++;
    return c;
}

void putback(GlobalParseur* parseur,istream& fichier, uchar k) {
	if (parseur->cryptage!=NON)
		fichier.putback(ecry(k,parseur->bouclage--));
	else
		fichier.putback(k);
}

#define PUTBACK(k) putback(this,fichier,k)

void remetCaracteres(istream& fichier,char* enAttente,char cryptage,int bouclage) {
    unsigned int bouc=bouclage;
    int l=strlen(enAttente);
    for (int i=l-1;i>=0;i--) {
        if (cryptage==NON) {
            fichier.putback(enAttente[i]);
            *flot_erreur<<"#"<<enAttente[i];
        }
        else
            fichier.putback(ecry(enAttente[i],bouc--));
    }    
    enAttente[0]=0;    
}

/****************************************************************************************************/
char GlobalParseur::LireEntreCotes(istream& fichier,char* tampon,uchar& k,int& i,const char* guillemet) {
    LuEntreCotes=NON;

    if (!strchr(guillemet,k))
        return NON;
    
    
    k=LectCrypte(fichier);
    //Nous mettons une limite a 100 caracteres
	while (!fichier.eof() && !strchr(guillemet, k) && i < XIPBUFFERMAX) {
        if (k=='\\') {
            k=LectCrypte(fichier);
            switch(k) {
            case 'n':
                tampon[i++]='\n';
                break;
            case 't':
                tampon[i++]='\t';
                break;
            case 'r':
                tampon[i++]='\r';
                break;
            default:
                tampon[i++]=k;
            }
        }
        else
            tampon[i++]=k;
        k=LectCrypte(fichier);
    }
    
	if (i >= XIPBUFFERMAX || fichier.eof())
        //ce n'etait pas la chaine que nous cherchions
        //dans ce cas nous considerons qu'il s'agissait d'une erreur
        erreur(CHAINES[227]);    

    LuEntreCotes=OUI;
    k=LectCrypte(fichier);
    return OUI;
}
/****************************************************************************************************/
//Lecture sans traitement des echappements (sauf pour le " )
char GlobalParseur::LireEntreCotesBrute(istream& fichier,char* tampon,uchar& k,int& i,const char* guillemet) {
    LuEntreCotes=NON;

    if (!strchr(guillemet,k))
        return NON;
    
    
    k=LectCrypte(fichier);
    //Nous mettons une limite a 100 caracteres
	while (!fichier.eof() && !strchr(guillemet, k) && i < XIPBUFFERMAX) {
        if (k=='\\') {
            k=LectCrypte(fichier);
            if (k=='"')
                tampon[i++]='"';
            else {
                //dans ce cas on garde le caractere d'echappement
                tampon[i++]='\\';
                tampon[i++]=k;
            }
        }
        else
            tampon[i++]=k;
        k=LectCrypte(fichier);
    }
    
	if (i >= XIPBUFFERMAX || fichier.eof())
        //ce n'etait pas la chaine que nous cherchions
        //dans ce cas nous considerons qu'il s'agissait d'une erreur
        erreur(CHAINES[227]);    

    LuEntreCotes=OUI;
    k=LectCrypte(fichier);
    return OUI;
}

/****************************************************************************************************/
//#define LitUnCar(x) x=LectCrypte(fichier);if (x=='\n') LIGNES++;
/****************************************************************************************************/
char lespace(unsigned char c) {
    if (c==' ' || c== '\t' || c=='\n' || c=='\r' || c==0)
        return OUI;
    return NON;
}


/*****************************************************************************************************/
char lettre(unsigned char c) {

    if (c<=32 || strchr("<>{}[](),;:.&|!/\\=~#@^\"",c))
        return NON;

    return tolower(c);
}
/****************************************************************************************************/

char minlettre(unsigned char c) {

    if (c<=32 || strchr("<>{}[](),;:.&|!/\\=~#@^\"",c))
        return c;

    return tolower(c);
}
/****************************************************************************************************/
char alettre(unsigned char c) {
    if (c<=32 || strchr("<>{}[](),;:.&|!/\\=~#@^$%\"",c))
        return NON;

    return c;
}

/****************************************************************************************************/

char Lettre(unsigned char c) {
    
    if (c<=32 || strchr("<>{}[](),;:.&|!/\\=~#@^+*%\"",c))
        return NON;


    return tolower(c);
}
/*****************************************************************************************************/
char lettreBis(unsigned char c) {
    
    if (c<=32 || strchr("<>{}[](),;:.&|!/\\=~#@^?+-*$%\"'",c))
        return NON;
    
    return c;
}
/****************************************************************************************************/
//Nous lisons des instructions de compilation
// Elles sont de la forme /@, a raison d'une par ligne
//Les instructions sont:
// @rulespace=  ... a string
void GlobalParseur::LitInstructions(istream& fichier,unsigned char cc,unsigned char& r) {
	char buff[XIPBUFFERSIZE];
	char k;
	int i;

	k=litSans(fichier,buff);
	rulespace* courant=NULL;
	if (k=='=') {
		if (!strcmp(buff,"comment")) {
			buff[0]=0;
			i=0;
			k=LectCrypte(fichier);
			while (!fichier.eof() && i<1023) {
				if (k=='\\')
					k=LectCrypte(fichier);
				else
					if (k=='@')
						break;
				buff[i++]=k;
				k=LectCrypte(fichier);
			}
			buff[i]=0;
			commentaire_regle=buff;
			k=LectCrypte(fichier);
			while (!fichier.eof() && lespace(k)) {
				k=LectCrypte(fichier);
			}
			r=k;
			return;
		}

		if (!strcmp(buff,"rulespace")) {
			k=litSans(fichier,buff);
			if (spacename==NULL)
				spacename=new rulespace(buff);
			else
				if (spacename->nom!=buff)
					erreur(CHAINES[376]);
			courant=spacename;
			currentspacename=courant;
			while (k=='.') {
				k=litSans(fichier,buff);
				char trouve=NON;
				for (i=0;i<courant->fils.dernier;i++) {
					if (courant->fils[i]->nom == buff) {
						courant=courant->fils[i];
						trouve=OUI;
						break;
					}
				}
				if (trouve==NON) {
					rulespace* edn=new rulespace(buff);
					courant->fils.ajoute(edn);
					edn->pere=courant;
					courant=edn;					
				}
				currentspacename=courant;
			}
			r=k;
			return;
		}
		static char bufferr[1000];
		sprintf(bufferr,"%s:%s",CHAINES[377],buff);
		erreur(bufferr);
	}	
	
}

/****************************************************************************************************/
//Nous lisons une definition
void GlobalParseur::LitDefinition(istream& fichier,unsigned char cc,unsigned char& r) {
    char c=LectCrypte(fichier);
	char buff[XIPBUFFERSIZE];
    int i=0;    
    int val=0;
    char operateur=0;
    int recursion=0;
    
	while (c > 32 && c != '\\' && c != '(' && i < XIPBUFFERMAX) {
        buff[i++]=c;
        c=LectCrypte(fichier);
    }
    buff[i]=0;
    if (!strcmp(buff,"ifdef")) {
        //on doit lire le test
        if (c!='(')
            while (!fichier.eof() && c!='(') {c=LectCrypte(fichier);}
        if (c!='(')
            erreur(CHAINES[330]);
		while (c != ')' && i < XIPBUFFERMAX) {
            i=0;
            char negation=0;
            c=LectCrypte(fichier);
            if (c<=32) {
                while (c<=32) c=LectCrypte(fichier);
            }
            if (c=='~') {
                negation=1;
                c=LectCrypte(fichier);
                if (c<=32) {
                    while (c<=32) c=LectCrypte(fichier);
                }
            }
            while (c>32 && c!='|' && c!='&' && c!=')') {
                buff[i++]=c;
                c=LectCrypte(fichier);
            }
            buff[i]=0;
            if (c<=32) {
                while (c<=32) c=LectCrypte(fichier);
            }

            char existe=negation;
            for (int j=0;j<definition_var.size();j++) {
                if (definition_var[j]==buff) {
                    existe=1-negation;
                    break;
                }
            }
            switch (operateur) {
            case 0:
                val=existe;
                break;
            case '|':
                val|=existe;
                break;
            case '&':
                val&=existe;
                break;
            }
            operateur=c;
        }

        //si le test est positif, on lit la suite de l'analyse
        if (val==1) {
            c=LectCrypte(fichier);
            while (!fichier.eof() && lespace(c))
                c=LectCrypte(fichier);
            
			if (!fichier.eof() && c=='\\')
                c=LectCrypte(fichier);
            
			while (!fichier.eof() && lespace(c))
				c=LectCrypte(fichier);
            
            r=c;
            return;        
        }
        //sinon, on va jusqu'au else, s'il existe, ou jusqu'au endif
        while (!fichier.eof()) {
            while (!fichier.eof() && c!='/') c=LectCrypte(fichier);
            c=LectCrypte(fichier);
            if (c!='#') {
                c=LectCrypte(fichier);
                continue;
            }
            i=0;
            c=LectCrypte(fichier);
			while (!fichier.eof() && c>32 && c != '\\' && i < XIPBUFFERMAX) {
                buff[i++]=c;
                c=LectCrypte(fichier);
            }
            buff[i]=0;
            if (!strcmp(buff,"ifdef")) {
                //appel recursif
                recursion++;
                continue;
            }
            
            if (!strcmp(buff,"endif") ||
                !strcmp(buff,"else")) {
                
                if (recursion>=1) {
                    if (!strcmp(buff,"endif"))
                        recursion--;
                    continue;
				}

				while (!fichier.eof() && lespace(c))
					c=LectCrypte(fichier);

				if (!fichier.eof() && c=='\\')
					c=LectCrypte(fichier);

				while (!fichier.eof() && lespace(c))
					c=LectCrypte(fichier);
				r=c;
				return;        
            }
        }
        erreur(CHAINES[331]);
    }

    if (!strcmp(buff,"else")) {
        //sinon, on va jusqu'au endif
        while (!fichier.eof()) {
            while (!fichier.eof() && c!='/') c=LectCrypte(fichier);
            c=LectCrypte(fichier);
            if (c!='#') {
                c=LectCrypte(fichier);
                continue;
            }
            i=0;
            c=LectCrypte(fichier);
			while (!fichier.eof() && c>32 && c != '\\'&& i < XIPBUFFERMAX) {
                buff[i++]=c;
                c=LectCrypte(fichier);
            }
            buff[i]=0;
            if (!strcmp(buff,"ifdef")) {
                //appel recursif
                recursion++;
                continue;
            }
            
            if (!strcmp(buff,"else")) {
                if (recursion>=1) 
                    continue;
                erreur(CHAINES[333]);
            }

            if (!strcmp(buff,"endif")) {
                if (recursion>=1) {
                    recursion--;
                    continue;
                }

				while (!fichier.eof() && lespace(c))
					c=LectCrypte(fichier);

				if (!fichier.eof() && c=='\\')
					c=LectCrypte(fichier);

				while (!fichier.eof() && lespace(c))
					c=LectCrypte(fichier);

				r=c;
				return;        
            }
        }
        erreur(CHAINES[332]);
    }

    if (!strcmp(buff,"endif")) {
        while (!fichier.eof() && c!='\n') {c=LectCrypte(fichier);}
    }
    else
        while (!fichier.eof() && c!='\\') {c=LectCrypte(fichier);}
    
    c=LectCrypte(fichier);
    while (!fichier.eof() && lespace(c)) {
        c=LectCrypte(fichier);
    }
    r=c;
}

char* GlobalParseur::lireRulespaceLabel(istream& fichier,char* tampon,unsigned char& k,char blanc) {
	char* etiquette=NULL;
	static char tampon_etiquette[XIPBUFFERSIZE];
	tampon_etiquette[0]=0;
	if (k=='#') {//lecture etiquette
		k=lit(fichier,tampon);
		if (k=='@') {
			LitInstructions(fichier,'@',k);
			if (k=='#')
				etiquette=lireRulespaceLabel(fichier,tampon,k,blanc);
			else {
				RemetCaractere(k);
				if (blanc==NON)
					k=lit(fichier,tampon);
			}
		}
		else {
			strcpy(tampon_etiquette,tampon);
			etiquette=tampon_etiquette;
			RemetCaractere(k);
			if (blanc==OUI)
				LectureBlancs(fichier,k);
			else
				k=lit(fichier,tampon);            
		}
	}
	
	return etiquette;
}

/****************************************************************************************************/
/* Cette fonction lit les commentaires...*/
char GlobalParseur::LitCommentaires(istream& fichier,unsigned char cc,unsigned char& r) {
    char c=cc;
	int ligne;
	
	if (fichier.eof())
		return NON;

	if (c=='/') {
		c=LectCrypte(fichier);

		switch(c) {
		case '/':
		//C'est la lecture d'une ligne
			while (!fichier.eof() && c!='\n') {c=LectCrypte(fichier);}
			break;
		case '#':
			LitDefinition(fichier,cc,r);
			return OUI;
		default:
			ligne=LIGNES+1;
			while (!fichier.eof() && c!='\\') {
				c=LectCrypte(fichier);
			}
			//dans le cas ou le caractere fermant n'a pas ete trouve, on renvoie une erreur
			if (c!='\\') {
				LIGNESERREUR=ligne;
				erreur(CHAINES[378]);
			}
		}
        c=LectCrypte(fichier);
        while (!fichier.eof() && lespace(c)) {
            c=LectCrypte(fichier);
        }
        r=c;
        return OUI;
    }

    r=c;
    return NON;
}
/****************************************************************************************************/
/*On lit certaines chaines de caractères dont on se servira pour 
construire 
les attributs, les valeurs etc...*/
char GlobalParseur::lit(istream& fichier,char *tampon) {
    int i=0;
    unsigned char c,k;
   
    LuEntreCotes=NON;

//    if (enAttente[0]!=0)
//        remetCaracteres(fichier,enAttente,cryptage,bouclage);
        
	tampon[0]=0;
	c=LectCrypte(fichier);

    while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);}

    while (LitCommentaires(fichier,c,k)) {
        c=k;
    }

    if (fichier.eof()) 
        return ':';


    LIGNESERREUR=LIGNES+1;
	bool conversion=false;
    //Sequence d'echappement
	while (c == '\\' && i < XIPBUFFERMAX) {
        k=LectCrypte(fichier);    
        tampon[i++]=k;
        c=LectCrypte(fichier);
    }

    LireEntreCotes(fichier,tampon,c,i);
	if (c > 127) {
		k = c;
		conversion = true;
	}
	else
		k = lettre(c);

	while (!fichier.eof() && k && i < XIPBUFFERMAX) {
        tampon[i] = k;
        c=LectCrypte(fichier);
        //Sequence d'echappement
        if (c=='\\') {
            k=LectCrypte(fichier);    
            tampon[++i]=k;
            c=LectCrypte(fichier);
        }
		if (c > 127) {
			k = c;
			conversion = true;
		}
		else
			k = lettre(c);
		i++;
    }
    tampon[i]='\0';
	if (i && conversion) {
		string res = conversion_utf8_to_latin(s_to_lower(tampon));
		strcpy(tampon, STR(res));
	}

	if (lespace(c)) {
		while (!fichier.eof() && lespace(c)) {
			char kc=LectCrypte(fichier);
			if (kc=='/') {
				PUTBACK(kc);
				break;
			}
			c=kc;
		}
	}

    if (fichier.eof()) return ':';
    return c;
}

/****************************************************************************************************/
char GlobalParseur::litSans(istream& fichier,char *tampon) {
    int i=0;
    unsigned char c,k;

    LuEntreCotes=NON;

//     if (enAttente[0]!=0)
//         remetCaracteres(fichier,enAttente,cryptage,bouclage);

    c=LectCrypte(fichier);
    tampon[0]=0;
    while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);}
  
    while (LitCommentaires(fichier,c,k)) {
        c=k;
    }

    if (fichier.eof()) 
        return ':';

    
    LIGNESERREUR=LIGNES+1;

    //Sequence d'echappement
	while (c == '\\' && i < XIPBUFFERMAX) {
        k=LectCrypte(fichier);    
        tampon[i++]=k;
        c=LectCrypte(fichier);
    }

    LireEntreCotes(fichier,tampon,c,i);

    k=alettre(c);
	while (!fichier.eof() && k && i < XIPBUFFERMAX) {
        tampon[i] = k;
        c=LectCrypte(fichier);

        //Sequence d'echappement
        if (c=='\\') {
            k=LectCrypte(fichier);
            tampon[++i]=k;
            c=LectCrypte(fichier);
        }

        k=alettre(c);
        i++; 
    }
    
    tampon[i]='\0';
	if (lespace(c)) {
		while (!fichier.eof() && lespace(c)) {
			char kc=LectCrypte(fichier);
			if (kc=='/') {
				PUTBACK(kc);
				break;
			}
			c=kc;
		}
	}

    if (fichier.eof()) return ':';
    return c;
}
/****************************************************************************************************/
char GlobalParseur::Llit(istream& fichier,char *tampon) {
    int i=0;
    unsigned char c,k;
    
    LuEntreCotes=NON;

//     if (enAttente[0]!=0)
//         remetCaracteres(fichier,enAttente,cryptage,bouclage);

    c=LectCrypte(fichier);
    tampon[0]=0;
    while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);}

    while (LitCommentaires(fichier,c,k)) {
        c=k;
    }

    LIGNESERREUR=LIGNES+1;

    //Sequence d'echappement
	while (c == '\\' && i < XIPBUFFERMAX) {
        k=LectCrypte(fichier);    
        tampon[i++]=k;
        c=LectCrypte(fichier);
    }

	bool conversion = false;
	LireEntreCotes(fichier, tampon, c, i);
	if (c > 127) {
		conversion = true;
		k = c;
	}
	else
		k = lettre(c);

	while (!fichier.eof() && k && i < XIPBUFFERMAX) {
        tampon[i] = k;
        c=LectCrypte(fichier);    

        //Sequence d'echappement
        if (c=='\\') {
            k=LectCrypte(fichier);
            tampon[++i]=k;
            c=LectCrypte(fichier);
        }

		if (c > 127) {
			conversion = true;
			k = c;
		}
		else
			k = lettre(c);
		i++;
    }

    tampon[i]='\0';
	if (i && conversion) {
		string res = conversion_utf8_to_latin(s_to_lower(tampon));
		strcpy(tampon, STR(res));
	}
    if (fichier.eof()) 
        return ':';
    return c;
}
/****************************************************************************************************/
char GlobalParseur::litbrut(istream& fichier, char *tampon) {
	int i = 0;
	unsigned char c, k;

	LuEntreCotes = NON;

	//     if (enAttente[0]!=0)
	//         remetCaracteres(fichier,enAttente,cryptage,bouclage);

	c = LectCrypte(fichier);
	tampon[0] = 0;
	while (!fichier.eof() && lespace(c)) { c = LectCrypte(fichier); }

	while (LitCommentaires(fichier, c, k)) {
		c = k;
	}

	LIGNESERREUR = LIGNES + 1;

	//Sequence d'echappement
	while (c == '\\' && i < XIPBUFFERMAX) {
		k = LectCrypte(fichier);
		tampon[i++] = k;
		c = LectCrypte(fichier);
	}

	bool conversion = false;
	LireEntreCotes(fichier, tampon, c, i);
	if (c > 127) {
		conversion = true;
		k = c;
	}
	else
		k = alettre(c);

	while (!fichier.eof() && k && i < XIPBUFFERMAX) {
		tampon[i] = k;
		c = LectCrypte(fichier);

		//Sequence d'echappement
		if (c == '\\') {
			k = LectCrypte(fichier);
			tampon[++i] = k;
			c = LectCrypte(fichier);
		}

		if (c > 127) {
			conversion = true;
			k = c;
		}
		else
			k = alettre(c);
		i++;
	}

	tampon[i] = '\0';
	if (i && conversion) {
		string res = conversion_utf8_to_latin(tampon);
		strcpy(tampon, STR(res));
	}
	if (fichier.eof())
		return ':';
	return c;
}
/****************************************************************************************************/
void GlobalParseur::LitUneLigne(istream& fichier,char *tampon,char premier) {
    int i=0;
    unsigned char c;
    
    LuEntreCotes=NON;

	tampon[0]=0;

	c=LectCrypte(fichier);    
	if (premier==OUI) {
		while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);}
	}
	else {
		//sinon, il ne peut y avoir qu'un seul retour chariot
		if (c==10)
			c=LectCrypte(fichier);
		if (c==13)
			c=LectCrypte(fichier);
	}

    if (LireEntreCotes(fichier,tampon,c,i)==OUI)
		return;

	while (!fichier.eof() && c != 13 && c != 10 && i < XIPBUFFERMAX) {
        tampon[i] = c;
        c=LectCrypte(fichier);    
        i++;
    }

    tampon[i]='\0';
}
/****************************************************************************************************/
char GlobalParseur::litChemin(istream& fichier,char *tampon) {
    int i=0;
    unsigned char c;

    LuEntreCotes=NON;

//     if (enAttente[0]!=0)
//         remetCaracteres(fichier,enAttente,cryptage,bouclage);

    c=LectCrypte(fichier);
    tampon[0]=0;
    while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);}
	unsigned char cc;
	
	while (!fichier.eof() && c == '/' && i < XIPBUFFERMAX) {
		c = LectCrypte(fichier);
		if (c == '/') {
			//C'est la lecture d'une ligne de commentaire...
			while (!fichier.eof() && c != '\n') { c = LectCrypte(fichier); }
			while (!fichier.eof() && lespace(c)) { c = LectCrypte(fichier); }
		}
		else {
			if (c == '#') {
				LitDefinition(fichier, c, cc);
				c = cc;
				break;
			}
			tampon[i++] = '/';
			break;
		}
	}

	if (c == '+' && i < XIPBUFFERMAX) {
        tampon[i++]=c;
        c=LectCrypte(fichier);
        while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);}
    }

	while (!fichier.eof() && c != ',' && !lespace(c) && i < XIPBUFFERMAX) {
        tampon[i++] = c;
        c=LectCrypte(fichier);
    }

    tampon[i]='\0';
    while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);} 
    if (c!=',') {
        cc=c;
        while (LitCommentaires(fichier,c,cc)) {
            c=cc;
        }
        if (fichier.eof())
            return ':';
        return lettre(c);
    }

    return c;
}
/****************************************************************************************************/
char GlobalParseur::LitEgal(istream& fichier,unsigned char *tampon) {
    int i=0;
    unsigned char c,k;

    LuEntreCotes=NON;

    c=LectCrypte(fichier);
    tampon[0]=0;
    while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);}

    while (LitCommentaires(fichier,c,k)) {
        c=k;
    }
    if (fichier.eof()) 
        return ':';

    LIGNESERREUR=LIGNES+1;

    //Sequence d'echappement
    if (c=='\\') {
        while (c=='\\') {
            k=LectCrypte(fichier);    
            tampon[i++]=k;
            c=LectCrypte(fichier);
        }
    }
    else
        //Cas particulier, c'est le premier caractere et il correspond a un caractere d'arret
        if (c==':' || c=='=' || c=='[') {
            tampon[i++]=c;
            c=LectCrypte(fichier);
        }

    k=c;
    if (!LireEntreCotes(fichier,(char*)tampon,k,i)) {
        while (!fichier.eof()) {          
            if (k==':' || k=='=' || k=='[')
                break;
            if (lespace(k))
                break;
            tampon[i] = k;
            i++;              
            k=LectCrypte(fichier);
        
            if (k=='\\') {
                k=LectCrypte(fichier);
                tampon[i++]=k;
                k=LectCrypte(fichier);
            }           
        }
    }
 
    tampon[i]='\0';

    if (k==':')
        return k;

    while (!fichier.eof() && lespace(k)) {
        k=LectCrypte(fichier);
    }
    if (fichier.eof()) 
        return ':';
    return k;
}/****************************************************************************************************/
//Lecture sans sequence d'echappement (sauf pour le signe egal)
char GlobalParseur::LitEgalBrut(istream& fichier,unsigned char *tampon,char type_lecture) {
    int i=0;
    unsigned char c,k;

    LuEntreCotes=NON;

    c=LectCrypte(fichier);
    tampon[0]=0;
    while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);}

    while (LitCommentaires(fichier,c,k)) {
        c=k;
    }

    if (fichier.eof()) 
        return ':';

    LIGNESERREUR=LIGNES+1;

    //Sequence d'echappement
    if (c=='\\') {
        while (c=='\\') {
            k=LectCrypte(fichier);  
            //Un seul cas: "
            if (k!='"') 
                tampon[i++]='\\';            
            tampon[i++]=k;
            c=LectCrypte(fichier);
        }
    }
	else {
		//Premier cas particulier, presence d'un contexte
		if (c=='|') 
			return c;
        //Cas particulier, c'est le premier caractere et il correspond a un caractere d'arret
        if (c==':' || c=='=' || c=='[' || (type_lecture==OUI && c=='>')) {
            tampon[i++]=c;
            c=LectCrypte(fichier);
        }
	}

    k=c;
    if (!LireEntreCotesBrute(fichier,(char*)tampon,k,i)) {
        while (!fichier.eof()) {          
            if (k==':' || k=='=' || k=='[' || (type_lecture==OUI && k=='>'))
                break;
            if (lespace(k))
                break;
            tampon[i] = k;
            i++;              
            k=LectCrypte(fichier);
        
            if (k=='\\') {
                k=LectCrypte(fichier);
                tampon[i++]=k;
                k=LectCrypte(fichier);
            }           
        }
    }
 
    tampon[i]='\0';

    if (k==':')
        return k;

    while (!fichier.eof() && lespace(k)) {
        k=LectCrypte(fichier);
    }
    if (fichier.eof()) 
        return ':';
    return k;
}
/****************************************************************************************************/
char GlobalParseur::litBis(istream& fichier,char *tampon) {
    int i=0;
    unsigned char c,k;

    LuEntreCotes=NON;

//     if (enAttente[0]!=0)
//         remetCaracteres(fichier,enAttente,cryptage,bouclage);

    c=LectCrypte(fichier);
   
    while (!fichier.eof() && lespace(c)) {
        if (c=='\n') 
            LIGNES++;
        c=LectCrypte(fichier);
    }

    while (LitCommentaires(fichier,c,k)) {
        c=k;
    }

    if (fichier.eof()) 
        return ':';

    LIGNESERREUR=LIGNES+1;
    LireEntreCotes(fichier,tampon,c,i);

    k=lettreBis(c);
	while (!fichier.eof() && k && i < XIPBUFFERMAX) {
        tampon[i] = k;
        c=LectCrypte(fichier);
        i++;
        k=lettreBis(c);
    }
 
    tampon[i]='\0';
    if (fichier.eof())
        return ':';
    return c;
}

        
/****************************************************************************************************/
/****************************************************************************************************/
void GlobalParseur::LitLigne(istream& fichier,char *tampon) {
    int i=0;
    unsigned char c,k;

    LuEntreCotes=NON;

//     if (enAttente[0]!=0)
//         remetCaracteres(fichier,enAttente,cryptage,bouclage);

    c=LectCrypte(fichier);
    tampon[0]=0;
    while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);}

    while (LitCommentaires(fichier,c,k)) {
        c=k;
    }

    LIGNESERREUR=LIGNES+1;

    k=c;
	while (!fichier.eof() && k != 10 && k != 13 && i < XIPBUFFERMAX) {
        tampon[i] = k;
        k=LectCrypte(fichier);
        i++;        
    }
 
    tampon[i]='\0';
}
//------------------------------------------------------------------------------
char GlobalParseur::LitJusque(istream& fichier,unsigned char *tampon,char stop) {
    int i=0;
    unsigned char c,k;

    LuEntreCotes=NON;

//     if (enAttente[0]!=0)
//         remetCaracteres(fichier,enAttente,cryptage,bouclage);

    c=LectCrypte(fichier);
    tampon[0]=0;
    while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);}

    while (LitCommentaires(fichier,c,k)) {
        c=k;
    }
    if (fichier.eof()) 
        return ':';

    LIGNESERREUR=LIGNES+1;

    //Sequence d'echappement
    if (c=='\\') {
        while (c=='\\') {
            k=LectCrypte(fichier);    
            tampon[i++]=k;
            c=LectCrypte(fichier);
        }
    }
    else
        if (c==stop) {
            tampon[i++]=c;
            c=LectCrypte(fichier);
        }
    
    k=c;
    if (!LireEntreCotes(fichier,(char*)tampon,k,i)) {
        while (!fichier.eof()) {
            if (k==stop)
                break;

            tampon[i] = k;
            i++;        
            k=LectCrypte(fichier);
        
            if (k=='\\') {
                k=LectCrypte(fichier);
                tampon[i++]=k;
                k=LectCrypte(fichier);
            }
        }
    }

    tampon[i]='\0';

    if (k==':')
        return k;

    while (!fichier.eof() && lespace(k)) {
        k=LectCrypte(fichier);
    }
    if (fichier.eof()) 
        return ':';
    return k;
}

/*****************************************************************************************************/
char lettreformule(unsigned char c) {

    if (c<=32 || strchr("<>{}[](),;:&|!/\\=~^*+-%\"",c))
        return NON;

    return tolower(c);
}

//--------------------------------------------------------------------------------------------
char GlobalParseur::litFormule(istream& fichier,char *tampon,char stop) {
    int i=0;
    unsigned char c,k;
   
    LuEntreCotes=NON;

//     if (enAttente[0]!=0)
//         remetCaracteres(fichier,enAttente,cryptage,bouclage);

    c=LectCrypte(fichier);
    tampon[0]=0;
    while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);}

    while (LitCommentaires(fichier,c,k)) {
        c=k;
    }
    if (fichier.eof()) 
        return ':';

    LIGNESERREUR=LIGNES+1;

    char signe=NON;
    //Sequence d'echappement
	while (c == '\\' && i < XIPBUFFERMAX) {
        k=LectCrypte(fichier);    
        tampon[i++]=k;
        c=LectCrypte(fichier);
    }

    LireEntreCotes(fichier,tampon,c,i);
    //cas particulier si le premier caractere est un signe + ou -
    //Alors la suite doit immédiatement être un chiffre. Sinon, il s'agit d'un nombre
    if (c=='+' || c=='-') {
        k=c;
        signe=OUI;
    }
    else
        k=lettreformule(c);

    char ajoute=OUI;
	while (!fichier.eof() && k && i < XIPBUFFERMAX) {
        if (ajoute==OUI)
            tampon[i] = k;
        c=LectCrypte(fichier);
        //Sequence d'echappement
        if (c=='\\') {
            k=LectCrypte(fichier);
            tampon[++i]=k;
            c=LectCrypte(fichier);
        }

        k=lettreformule(c);

        //si le signe n'est pas suivi d'un caractere valide
        //alors on saute ce caractere et on continue
        if (!signe || k) {
            signe =NON;
            ajoute=OUI;
            i++;
        }
        else {
            ajoute=NON;
            k=255;
        }
    }
    tampon[i]='\0';
	if (lespace(c)) {
        if (stop==OUI)
            return -1;
		while (!fichier.eof() && lespace(c)) {
			char kc=LectCrypte(fichier);
			if (kc=='/') {
				PUTBACK(kc);
				break;
			}
			c=kc;
		}
	}

    if (fichier.eof()) return ':';
    return c;
}


char GlobalParseur::litOperateur(istream& fichier) {
    int i=0;
    unsigned char c;       

    LuEntreCotes=NON;

    c=LectCrypte(fichier);
    
    while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);}
    return c;
}


//----------------------------------------------------------------------------
char GlobalParseur::LitTantQue(istream& fichier,unsigned char *tampon,const char* stop,const char* guillemet) {
    int i=0;
    unsigned char c,k;

    LuEntreCotes=NON;

//     if (enAttente[0]!=0)
//         remetCaracteres(fichier,enAttente,cryptage,bouclage);

    c=LectCrypte(fichier);
    tampon[0]=0;
    while (!fichier.eof() && lespace(c)) {c=LectCrypte(fichier);}


    LIGNESERREUR=LIGNES+1;

    //Sequence d'echappement
    if (c=='\\') {
        while (c=='\\') {
            k=LectCrypte(fichier);    
            tampon[i++]=k;
            c=LectCrypte(fichier);
        }
    }
    else
        if (strchr(stop,c)) {
            tampon[0]=0;
            return c;
        }
    
    k=c;
    if (!LireEntreCotes(fichier,(char*)tampon,k,i,guillemet)) {
        while (!fichier.eof()) {
            if (strchr(stop,k))
                break;

            tampon[i] = k;
            i++;        
            k=LectCrypte(fichier);
        
            if (k=='\\') {
                k=LectCrypte(fichier);
                tampon[i++]=k;
                k=LectCrypte(fichier);
            }
        }
    }

    tampon[i]='\0';

    if (k==':')
        return k;

    while (!fichier.eof() && lespace(k)) {
        k=LectCrypte(fichier);
    }
    if (fichier.eof()) 
        return ':';
    return k;
}

//----------------------------------------------------------------------------------
void GlobalParseur::LectureBlancs(istream& fichier,uchar& k,char comment) {
    uchar debut=k;
    uchar c;
    while ((k<=32 || k==comment) && !fichier.eof()) {
        k=minlettre(LectCrypte(fichier));
        if (k==comment) {
            if (debut==comment) {
                enAttente[0]=debut;
                enAttente[1]=0;
            }
            while (LitCommentaires(fichier,k,c)) {
                k=c;
            }
        }
    }
}

//----------------------------------------------------------------------------------
void GlobalParseur::LectureBruteBlancs(istream& fichier,uchar& k,char comment) {
    uchar debut=k;
    uchar c;
    while ((k<=32 || k==comment) && !fichier.eof()) {
        k=LectCrypte(fichier);
        if (k==comment) {
            if (debut==comment) {
                enAttente[0]=debut;
                enAttente[1]=0;
            }
            while (LitCommentaires(fichier,k,c)) {
                k=c;
            }
        }
    }
}

//----------------------------------------------------------------------------------
//Lecture d'un argument
void GlobalParseur::lireunevariable(istream& fichier,uchar& k,char *tampon) {
    k=LectureMin(fichier);
    int i=0;

    while (Lettre(k)) {
        tampon[i++]=k;
        k=LectureMin(fichier);
    }

    tampon[i]=0;
    LectureBlancs(fichier,k,NON);
}

//-----------------------------------------------------------------------------------
//On stocke la chaine a l'envers
void GlobalParseur::MetEnAttenteLecture(char k,char* tampon) {
    //cas particulier, si k est une lettre de l'alphabet et tampon n'est pas vide                  
    if (lettre(k) && tampon[0]!=0)
        //On rajoute un separateur dans tampon
        strcatchr(tampon,32);

    int len=strlen(enAttente);
    enAttente[len]=k;
    int l=strlen(tampon);
    for (int i=l-1;i>=0;i--)
        enAttente[len+l-i]=tampon[i];
    enAttente[len+l+1]=0;
}
