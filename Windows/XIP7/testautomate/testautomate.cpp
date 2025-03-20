// testautomate.cpp : Defines the entry point for the console application.
//


#define TailleChaine 512
#define NON 0
#define OUI 1

#include <ostream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;


#include "vecta.h"
#include "automaton.h"
#include "transduct.h"

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

void compileautomaton(string nom,string sauve,int nbarcs);
static VECTA<int> pilearcs;    
static VECTA<int> pileposition;    
static VECTA<etat*> pileetats;
static char* automate_buffer=NULL;
static int tailledebuf=0;

void liretrans(char*);

etat::etat() {
    status=AUTOMATONSTART;
    id=0;
    valeur=NULL;
    marque=0;
    nombre=0;
}

etat::etat(automate* a,char s) {
    nombre=0;
    marque=0;
    valeur=NULL;
    status=s;
    id=a->dernier++;    
    a->etats.ajoute(this,a->etats.taille+a->etats.taille/2);
}
  


char analyseCaractere(unsigned char c) {
    
     if (c<=32 || strchr("<>{}[](),;:.&|!/\\=~#@^?+-*$%\"'_¬£€`",c))
        return AUTOMATONPUNCT;

     if (c>='0' && c<='9')
         return AUTOMATONDIGIT;

     int l=c;
     if (isupper(l))
         return AUTOMATONLETTERUPPER;

     if (isalpha(l))
         return AUTOMATONLETTER;
     
     return AUTOMATONANY;
}

arcautomate TestCharId(unsigned char id) {    
    if (id>='1' && id<='9')
        return AUTOMATONVARIABLE;

    switch(id) {
    case '*':
        return AUTOMATONSTAR;
    case '+':
        return AUTOMATONPLUS;
    case 'c':
        return AUTOMATONLETTER;
    case 'C':
        return AUTOMATONLETTERUPPER;
    case 'd':
        return AUTOMATONDIGIT;
    case 'p':
        return AUTOMATONPUNCT;
    }    
    return AUTOMATONEPSILON;
}

void etat::ajoutearc(arc* a) {    
    //nous trions les arcs    
    if (a->action==AUTOMATONVARIABLE) {
        arcs.insere(0,a);
        return;
    }

    int i;
    for (i=0;i<arcs.dernier;i++) {
        if (a->label>arcs[i]->label)
            break;
    }
    arcs.insere(i,a);
}

void ajouteautomate(automate* principal,automate* nouveau) {
	//Puis on rajoute notre automate dans l'automate courant
	//Tout d'abord, les etats
	int i;
	for (i=0;i<nouveau->etats.dernier;i++) {
		if (nouveau->etats[i]!=NULL) {
			nouveau->etats[i]->id=principal->etats.dernier;
			principal->etats.ajoute(nouveau->etats[i]);
		}
	}
	//On vide ce tableau, pour eviter des destructions intempestives
	nouveau->etats.raz();
	//Ensuite, on effectue notre branchement...
	//Tous les arcs initiaux sont recopies comme arcs initiaux
	for (i=0;i<nouveau->e.arcs.dernier;i++)
		principal->e.ajoutearc(nouveau->e.arcs[i]);
	//remise a zero pour la meme raison que precedemment
	nouveau->e.arcs.raz();
	delete nouveau;
}

char chainecomplexe(unsigned char* chaine,int position, typeautoparse state) {
        
    //Chaine entierement analysee
    if (chaine[position]==0)
        return NON;
    int lettresuivante;
    
    switch (state) {
        //etat initial
    case ARCINITIAL:
        lettresuivante=position;
        switch (chaine[position]) { 
        case '~':
            return chainecomplexe(chaine,position+1,ARCINITIAL);            
        case '\\':
            lettresuivante=position+1;
            break;   
        case '?':
            return OUI;
		case '#':
			return OUI;
        case '%':
            return OUI;
        case '[':
            return OUI;
        case '(':
            return OUI;
        }
        return chainecomplexe(chaine,lettresuivante,ARCSIMPLE);        
    //creation d'un simple arc
    case ARCSIMPLE:        
        //si cet arc est suivi d'un symbole de Kleene: cas particulier
        //il faut aussi ne pas oublier de sauter la * ou le + d'ou la valeur d'increment a 2
        if (chaine[position+1]=='*' || chaine[position+1]=='+') 
            return OUI;

        //on rajoute les arcs a la suite
        return chainecomplexe(chaine,position+1,ARCINITIAL);        
    }
    //On analyse un caractere special et on cree un arc au besoin 
    return NON;
}

char typeautomate(unsigned char* chaine,int position) {
        
    //Chaine entierement analysee
    if (chaine[position]==0)
        return NON;

    int lettresuivante;
    
    lettresuivante=position+1;
    switch (chaine[position]) {         
    case '\\':                
        if (chaine[position+1]==0)
            return NON;
        lettresuivante=position+2;
        break;   
    case '%':
        if (chaine[position+1]==0)
            return NON;
        if (chaine[position+1]>='1' && chaine[position+1]<='9')
            return AUTOMATEPILE;
        lettresuivante=position+2;
        break;
    default:
        lettresuivante=position+1;
    }

    return typeautomate(chaine,lettresuivante);        
}


etat* etat::parse(automate* a,                 
                 unsigned char* chaine,
                 int position, 
                 int& finchaine,
                 unsigned char fin,
                 void* value,
                 etat* debut, 
                 arc* courant,
                 etat* etatfinal,
                 typeautoparse state,
                 char negation) {
    
    if (a->erreur!=AERR_NO_ERROR)
        return NULL;

    arcautomate act=AUTOMATONLABEL;
    unsigned char lab;
    etat* et;
    etat* etatf;
    arc* ar=NULL;    
    arcautomate test; 
    //dans ce cas le caractere est simplement saute
    lab=chaine[position];
    act=AUTOMATONLABEL;
    int increment=1;
    typeautoparse etatsuivant;
    int lettresuivante;
	int val;
    
    //Chaine entierement analysee
    if (chaine[position]==0) {
        status|=AUTOMATONEND;
        if (valeur==NULL)
            valeur=value;
        return this;
    }
    char ajouteetatsortant=NON;
	char fermeture=']';
	char erreur_fermeture=AERR_MISSING_BRACKET;
    switch (state) {
        //etat initial
    case ARCINITIAL:
        lettresuivante=position;
        etatsuivant=ARCSIMPLE;
        switch (chaine[position]) { 
        case '~':
            return parse(a,chaine,position+1,finchaine,fin,value,debut,courant,etatfinal,ARCINITIAL,1-negation);            
        case '\\':                 
            lettresuivante=position+1;
            break;   
		case '?':
			etatsuivant=ARCANY;
			lettresuivante=position;
			break;
		case '#':
			etatsuivant=ARCCODE;
			lettresuivante=position;
			break;
        case '%':
            etatsuivant=ARCCOMPLEXE;
            lettresuivante=position+1;
            break;
        case ',':
            etatsuivant=ARCVIRGULE;
            break;
        case ']':
        case ')':
            etatsuivant=ARCFINCROPAR;
            break;
        case '[':
            etatsuivant=ARCCROCHET;
            lettresuivante=position+1;
            break;
        case '(':
            etatsuivant=ARCPARENTHESE;
            lettresuivante=position+1;            
            break;
        }
        return parse(a,chaine,lettresuivante,finchaine,fin,value,debut,courant,etatfinal,etatsuivant,negation);        
    //creation d'un simple arc
	case ARCANY:
		lab=AUTOMATONANY;
		act=AUTOMATONANY;
		ar=trouvearc(lab,act,negation,0);
        if (ar==NULL) {
            ar=new arc(act,negation,lab);
            et=new etat(a,AUTOMATONNODE);
            ajoutearc(ar);
            ar->e=et;
        }
        else
            et=ar->e;

        //si cet arc est suivi d'un symbole de Kleene: cas particulier
        //il faut aussi ne pas oublier de sauter la * ou le + d'ou la valeur d'increment a 2
        if (chaine[position+1]=='*' || chaine[position+1]=='+') {
            et->parse(a,chaine,position+1,finchaine,fin,value,this,ar,etatfinal,ARCBOUCLE,negation);
            increment=2;
        }

        //on rajoute les arcs a la suite
        return et->parse(a,chaine,position+increment,finchaine,fin,value,debut,courant,etatfinal,ARCINITIAL,0);        
	case ARCCODE:
		val=0;
		lettresuivante=0;
		while (chaine[position+1]>='0' && chaine[position+1]<='9' && lettresuivante<3) {
			val=val*10+chaine[++position]-48;
			lettresuivante++;
		}
		if (val>255) {
			a->initErreur(position,AERR_WRONG_CODE);
			return NULL;
		}
		lab=val;
		if (val==0)
			lab='#';
    case ARCSIMPLE:        
		ar=trouvearc(lab,act,negation,0);
        if (ar==NULL) {
            ar=new arc(act,negation,lab);
            et=new etat(a,AUTOMATONNODE);
            ajoutearc(ar);
            ar->e=et;
        }
        else
            et=ar->e;

        //si cet arc est suivi d'un symbole de Kleene: cas particulier
        //il faut aussi ne pas oublier de sauter la * ou le + d'ou la valeur d'increment a 2
        if (chaine[position+1]=='*' || chaine[position+1]=='+') {
            et->parse(a,chaine,position+1,finchaine,fin,value,this,ar,etatfinal,ARCBOUCLE,negation);
            increment=2;
        }

        //on rajoute les arcs a la suite
        return et->parse(a,chaine,position+increment,finchaine,fin,value,debut,courant,etatfinal,ARCINITIAL,0);        
    //On analyse un caractere special et on cree un arc au besoin 
    case ARCCOMPLEXE:
       test=TestCharId(chaine[position]);        
       switch(test) {
       case AUTOMATONVARIABLE:
           lab=chaine[position]-48;
           act=AUTOMATONVARIABLE;
           if (!a->construitvar(lab,position))
               return NULL;
           break;
       case AUTOMATONPLUS:
       case AUTOMATONSTAR:
           lab=AUTOMATONANY;
           act=AUTOMATONANY;
           break;
	   case AUTOMATONLETTER:
	   case AUTOMATONLETTERUPPER:
	   case AUTOMATONDIGIT:
	   case AUTOMATONPUNCT:
		   lab=test;
		   act=test;
		   break;
       default://cas d'echappement du caractere
           lab=chaine[position];                
           act=AUTOMATONLABEL;
       }

        if (ar==NULL) {
            ar=new arc(act,negation,lab);
            et=new etat(a,AUTOMATONNODE);
            ajoutearc(ar);
            ar->e=et;
        }
        else
            et=ar->e;

        if (test==AUTOMATONPLUS || test==AUTOMATONSTAR)
            et->parse(a,chaine,position,finchaine,fin,value,this,ar,etatfinal,ARCBOUCLE,negation);                    

        //si cet arc est suivi d'un symbole de Kleene: cas particulier
        //il faut aussi ne pas oublier de sauter la * ou le + d'ou la valeur d'increment a 2
        if (chaine[position+1]=='*' || chaine[position+1]=='+') {
            et->parse(a,chaine,position+1,finchaine,fin,value,this,ar,etatfinal,ARCBOUCLE,negation);
            increment=2;
        }

        //on rajoute les arcs a la suite
        return et->parse(a,chaine,position+increment,finchaine,fin,value,debut,courant,etatfinal,ARCINITIAL,0);        
    //On construit une boucle * ou +
    case ARCBOUCLE:        
        //On construit la boucle vers cet etat final
        ar=new arc(courant->action,negation,courant->label);
        ar->e=this;
        arcs.ajoute(ar);

        if (chaine[position]=='*') {
            //L'arc qui permet l'etat vide
            ar=new arc(AUTOMATONEPSILON,0,AUTOMATONEPSILON);
            debut->arcs.ajoute(ar);            
            //Qui pointe vers notre etat final courant
            ar->e=this;
        }
        break;
    //Traitement du crochet
	case ARCPARENTHESE:
		ajouteetatsortant=OUI;
		fermeture=')';
		erreur_fermeture=AERR_MISSING_PARENTHESIS;
    case ARCCROCHET:
        if (negation==1) {
            a->initErreur(position-1,AERR_WRONG_NEGATION_POSITION);
            return NULL;
        }
        //on construit des automates individuels
        et=new etat(a,AUTOMATONNODE);
        ar=new arc(AUTOMATONEPSILON,0,AUTOMATONEPSILON);
        ar->e=et;
        arcs.ajoute(ar);
        etatf=new etat(a,AUTOMATONNODE);
        et->parse(a,chaine,position,finchaine,fermeture,value,this,NULL,etatf,ARCINITIAL,0);
        if (chaine[finchaine]!=fermeture) {
            a->initErreur(finchaine,AERR_MISSING_BRACKET);
            return NULL;
        }
        finchaine++;
        //s'il y a une boucle: cas particulier que l'on traite ici
        if (chaine[finchaine]=='*' || chaine[finchaine]=='+') {
            //tout d'abord on reboucle
            ar=new arc(AUTOMATONEPSILON,0,AUTOMATONEPSILON);
            ar->e=this;
            etatf->arcs.ajoute(ar);
            //dans le cas de * il faut aussi sortir tout de suite
            if (chaine[finchaine]=='*') {
                ar=new arc(AUTOMATONEPSILON,0,AUTOMATONEPSILON);
                ar->e=etatf;
                arcs.ajoute(ar);
				ajouteetatsortant=NON;
            }
            finchaine++;
        }

		if (ajouteetatsortant==OUI) {
			ar=new arc(AUTOMATONEPSILON,0,AUTOMATONEPSILON);
			ar->e=etatf;
			arcs.ajoute(ar);
		}
        return etatf->parse(a,chaine,finchaine,finchaine,fin,value,debut,courant,etatfinal,ARCINITIAL,0);        
    //cas particulier du traitement de la ','
    case ARCVIRGULE:        
        //on construit des automatates individuels
        //On finit d'abord d'accrocher le dernier automate construit avec l'etat final propose
        if (fin==0) {            
            a->initErreur(position,AERR_WRONG_USE_OF_COMMA);
            return NULL;
        }
        ar=new arc(AUTOMATONEPSILON,0,AUTOMATONEPSILON);
        ar->e=etatfinal;
        arcs.ajoute(ar);        

        //Puis on cree un nouvel automate
        et=new etat(a,AUTOMATONNODE);
        ar=new arc(AUTOMATONEPSILON,0,AUTOMATONEPSILON);
        ar->e=et;
        debut->arcs.ajoute(ar);                

        return et->parse(a,chaine,position+1,finchaine,fin,value,debut,NULL,etatfinal,ARCINITIAL,0);        
    //Une fin de parenthese ou de crochet
    case ARCFINCROPAR:
        //On racroche notre automate, a l'etat final fourni
        if (fin!=chaine[position]) {
            //si on attendait un crochet ou une parenthese et qu'on a l'inverse
            //c'est une erreur
            if (fin==')')
                a->initErreur(position,AERR_MISMATCHED_PARENTHESIS);
            else
                a->initErreur(position,AERR_MISMATCHED_BRACKET);
            return NULL;
        }

        ar=new arc(AUTOMATONEPSILON,0,AUTOMATONEPSILON);
        ar->e=etatfinal;
        arcs.ajoute(ar);
        //On enregistre la position de ce crochet ou de cette parenthese fermante
        //pour pouvoir remonter cette position dans l'analyse de la chaine
        finchaine=position;
        break;
    }
    return NULL;
}

void automate::initErreur(int l,autoerreur err){
    if (erreur==AERR_NO_ERROR) {
        erreur=err;
        lieuerreur=l;
    }
}


void automatepile::testordre(char ordre) {
    if (ordre==OUI)
        variables[0]=0;
    else
        variables[0]=100;
}

etat* automate::parse(unsigned char* mot,void* valeur,char ordre) {
    int finchaine=0;
    erreur=AERR_NO_ERROR;
    lieuerreur=0;
    testordre(ordre);
    return e.parse(this,mot,0,finchaine,0,valeur,NULL,NULL,NULL,ARCINITIAL,0);    
}


etat* etat::parsebrute(automate* a,                 
                 unsigned char* chaine,
                 int position,                                   
                 void* value,                 
                 typeautoparse state) {
    
    if (a->erreur!=AERR_NO_ERROR)
        return NULL;

    arcautomate act=AUTOMATONLABEL;
    arc* ar=NULL;    
    //dans ce cas le caractere est simplement saute
	etat* courant=this;
	etat* et;
    unsigned char lab=chaine[0];
	position=1;

	while (lab!=0) {
		ar=courant->trouvearc(lab,act,NON,0);
        if (ar==NULL) {
            ar=new arc(act,NON,lab);
            et=new etat(a,AUTOMATONNODE);
            courant->ajoutearc(ar);
            ar->e=et;
			courant=et;
        }
        else
            courant=ar->e;

		lab=chaine[position++];
	}

	courant->status|=AUTOMATONEND;
    if (courant->valeur==NULL)
        courant->valeur=value;
    return courant;
}


//Dans ce cas, on stocke la chaine sans l'analyser. Il s'agit d'un simple mot et non
//d'une expression reguliere.
etat* automate::parsebrute(unsigned char* mot,void* valeur) {
    int finchaine=0;
    erreur=AERR_NO_ERROR;
    lieuerreur=0;
    testordre(OUI);
    return e.parsebrute(this,mot,0,valeur,ARCINITIAL);    
}

etat* etat::parsebrutelin(automate* a,unsigned char* chaine,void* value) {
    
    if (a->erreur!=AERR_NO_ERROR)
        return NULL;

    arcautomate act=AUTOMATONLABEL;
    arc* ar=NULL;    
    //dans ce cas le caractere est simplement saute
	etat* courant=this;
	etat* et;
    unsigned char lab=chaine[0];
	int position=1;

	while (lab!=0) {
		ar=courant->trouvearc(lab,act,NON,0);
        if (ar==NULL) {
            ar=new arc(act,NON,lab);
            et=new etat(a,AUTOMATONNODE);
            courant->ajoutearc(ar);
            ar->e=et;
			courant=et;
        }
        else
            courant=ar->e;

		lab=chaine[position++];
	}

	courant->status|=AUTOMATONEND;
    if (courant->valeur==NULL)
        courant->valeur=value;
    return courant;
}

etat* automate::parsebrutelin(unsigned char* mot,void* valeur) {
    int finchaine=0;
    erreur=AERR_NO_ERROR;
    lieuerreur=0;
    testordre(OUI);
    return e.parsebrutelin(this,mot,valeur);    
}

void arc::ArcEpsilon(VECTA<etatarc*>& eas) {
    if (marque==1)
        return;
    marque=1;
    e->ArcEpsilon(eas);
}

void etat::ArcEpsilon(VECTA<etatarc*>& eas) {
    if (marque==1)
        return;
    marque=1;
    for (int i=0;i<arcs.dernier;i++) {
        if (arcs[i]->action==AUTOMATONEPSILON) {
            etatarc* ea=new etatarc(this,arcs[i]);
            eas.ajoute(ea);
        }
        arcs[i]->ArcEpsilon(eas);
    }
}


void automate::retireEpsilon(VECTA<arc*>& adetruire) {
    //On retire les epsilons du dernier automate cree
    //Tout d'abord on recherche tous les arcs epsilons
    int i,j,k,l;
    
    VECTA<etatarc*> eas;
    
    arc* ar;
    char boucle=1;
    e.ArcEpsilon(eas);
    demarque();
	etat* courant;
    
    if (eas.dernier==0)
        return;
    
    for (i=0;i<eas.dernier;i++) {
        //on recupere tous les arcs semblables:
        k=i+1;
        courant=eas[i]->ar->e;
        
        
        while (k<eas.dernier && eas[i]->ar->e==eas[k]->ar->e) k++;
        
        //Nous avons dans etatarbre toutes les transitions necessaires
        for (j=k-1;j>=i;j--) {
            etat* ajout=eas[j]->e;            

            //si ajout est un etat final ainsi que courant, on ne fait rien
            //if (ajout->end() && courant->end())
            //    continue;

            if (courant==ajout && eas[i]->ar->action==AUTOMATONEPSILON)
                continue;
            
            //On remplace les arcs epsilon par les arcs suivants            
            //La position de l'arc epsilon que l'on remplace
            //Tous les arcs seront places apres
            
            for (l=0;l<courant->arcs.dernier;l++) {
                courant->arcs[l]->marque=-1;
                arc* trouve=ajout->trouvearc(courant->arcs[l]);
                if (trouve==NULL)
                    ajout->ajoutearc(courant->arcs[l]);
            }
            
            if (!ajout->end()) {
                //on recupere notre transition finale                        
                ajout->status|=courant->status;                                
                ajout->valeur=courant->valeur;
            }
        }

        //On retire les transitions epsilons
        for (j=i;j<k;j++) {
            for (l=eas[j]->e->arcs.dernier-1;l>=0;l--)
                if (eas[j]->e->arcs[l]==eas[j]->ar) {
                    ar=eas[j]->e->arcs.retireElement(l);
					if (ar->marque!=-1) {
						//if (ar->action!=AUTOMATONMARKEDFORDELETION)
						//	adetruire.ajoute(ar);
						ar->manipulation=AUTOMATONMARKEDFORDELETION;
					}
                }
        }

        i=k-1;
    }

    eas.nettoie();
	demarque();
	retirefordeleted(adetruire);
}


void arc::marquedestruction(VECTA<arc*>& lesarcs) {
    if (marque==1)
        return;
    marque=1;
    lesarcs.ajoute(this,lesarcs.taille+lesarcs.taille/2);
    e->marquedestruction(lesarcs);
}

void etat::marquedestruction(VECTA<arc*>& lesarcs) {
    if (marque==1)
        return;
    marque=1;

    for (int i=arcs.dernier-1;i>=0;i--)
        arcs[i]->marquedestruction(lesarcs);
}

VECTA<arc*> lesarcsadetruire(10000);

void automate::marquedestruction() {
    demarque();
	etat* et=&e;
	int i;
	for (i=-1;i<etats.dernier;i++) {
		if (i!=-1)
			et=etats[i];
        if (et!=NULL) {
			for (int j=0;j<et->arcs.dernier;j++) {
                if (et->arcs[j]->marque==1)
					continue;
				et->arcs[j]->marque=1;
				lesarcsadetruire.ajoute(et->arcs[j]);
			}
        }
    }

	for (i=0;i<lesarcsadetruire.dernier;i++)
		delete lesarcsadetruire[i];
	lesarcsadetruire.raz();
}



inline char arc::test(unsigned char c) {
    if (!c)
        return 0;

    
    switch(action) {
    case AUTOMATONANY:
        return 1-negation;
    case AUTOMATONDIGIT:      
        if (analyseCaractere(c)==AUTOMATONDIGIT)
            return 1-negation;            
        return negation;
    case AUTOMATONPUNCT:
        if (analyseCaractere(c)==AUTOMATONPUNCT)
            return 1-negation;            
        return negation;
    case AUTOMATONLETTER:
        if (analyseCaractere(c)==AUTOMATONLETTER)
            return 1-negation;            
        return negation;
    case AUTOMATONLETTERUPPER:
        if (analyseCaractere(c)==AUTOMATONLETTERUPPER)
            return 1-negation;
        return negation;
    case AUTOMATONLABEL:
        if (label==c)
            return 1-negation;        
        return negation;
    }

    return negation;
}
//----------------------------------------------------------------------------------------
//On extrait autant de sous-chaines possible que l'on stocke dans un vecteur
int automate::rechercheavecextractiondansvecteur(unsigned char* m,VECTA<string*>& vect) {    
    int suivant=0;
    int pos=0;

    while (suivant!=-1) {
        string* s=new string;
        suivant=rechercheavecextraction(m+pos,*s);
        if (suivant!=-1)
            vect.ajoute(s);
        else {
            delete s;
            break;
        }
        suivant=maxpos;        
        pos+=suivant;
    }
    return vect.dernier;
}

int automate::rechercheavecextraction(unsigned char* m,string& res) {
    int l=strlen((char*)m);
    initvar();

	if (automate_buffer==NULL) {
		automate_buffer=new char[l+1];
		tailledebuf=l+1;
	}
	else {
		if (tailledebuf<=l) {
			delete[] automate_buffer;
			tailledebuf=l+1;
			automate_buffer=new char[tailledebuf];
		}
	}


    for (int i=0;i<l;i++) {
        maxpos=-1;
        if (e.recherchepartielle(m,this,i)) {
            memcpy(automate_buffer,m+i,maxpos-i);
            automate_buffer[maxpos-i]=0;
            res=automate_buffer;
            return i;
        }
    }
    return -1;
}



int automate::rechercheavecextractionparlafin(unsigned char* m,string& res) {    
    int l=strlen((char*)m)-1;
    initvar();

	if (automate_buffer==NULL) {
		automate_buffer=new char[l+1];
		tailledebuf=l+1;
	}
	else {
		if (tailledebuf<=l) {
			delete[] automate_buffer;
			tailledebuf=l+1;
			automate_buffer=new char[tailledebuf];
		}
	}

    for (int i=l;i>=0;i--) {
        maxpos=-1;
        if (e.recherchepartielle(m+i,this,0)) {
            memcpy(automate_buffer,m+i,maxpos);
            automate_buffer[maxpos]=0;
            res=automate_buffer;
            return i;
        }
    }
    return -1;
}

int automate::recherchejusqualafin(unsigned char* m,string& res) {    
    int l=strlen((char*)m)-1;
    initvar();

	if (automate_buffer==NULL) {
		automate_buffer=new char[l+1];
		tailledebuf=l+1;
	}
	else {
		if (tailledebuf<=l) {
			delete[] automate_buffer;
			tailledebuf=l+1;
			automate_buffer=new char[tailledebuf];
		}
	}

    for (int i=l;i>=0;i--) {
        maxpos=-1;
        if (recherche(m+i)) {
            memcpy(automate_buffer,m+i,l-i+1);
            automate_buffer[l-i+1]=0;
            res=automate_buffer;
            return l;
        }
    }
    return -1;
}



etat* automate::recherchepartielle(unsigned char* m) {
    //L'ancre permet de determiner la position du premier caractere de la chaine
    //a recopier dans la variable
    maxpos=-1;
    initvar();

    return e.recherchepartielle(m,this,0);
}


etat* etat::recherchepartielle(unsigned char* mot,automate* ref,int i) {

    pilearcs.raz();
    pileposition.raz();
    pileetats.raz();
    char erreur=OUI;
    etat* courant=this;
    arc* a;
    int ia=0;
    while (courant!=NULL) {        
        while(ia<courant->arcs.dernier) {
            //cas speciaux
            erreur=OUI;
            a=courant->arcs[ia];            
            switch(a->action) {
            case AUTOMATONVARIABLE:
                //Le label correspond au numero de la variable
                if (ref!=NULL)
                    ref->majvar(a,i);
            case AUTOMATONEPSILON:
                //on passe directement a l'etat suivant
                pilearcs.ajoute(ia);
                pileposition.ajoute(i);
                pileetats.ajoute(courant);
                courant=a->e;      
                ia=0;
                erreur=NON;
                break;
            default:                            
                if (a->test(mot[i])) {
                    pilearcs.ajoute(ia);
                    pileposition.ajoute(i);
                    pileetats.ajoute(courant);
                    courant=a->e;                                    
                    i++;
                    ia=0;
                    erreur=NON;
                }
                else
                    ia++;
            }

            if (courant->end() && erreur==NON) {
                if (ref->maxpos<i)
                    ref->maxpos=i;
                return courant;
            }
        }
        
        courant=pileetats.retire();
        i=pileposition.retire();
        ia=pilearcs.retire()+1;
    }
    return NULL;
}

VECTA<int>* lapiledesargs=NULL;
//----------------------------------------------------------------------------------------
etat* etat::recherchelin(unsigned char* mot,automate* ref,int i) {

	if (lapiledesargs==NULL)
		lapiledesargs=&pilearcs;

    pilearcs.raz();
    pileposition.raz();
    pileetats.raz();
    char erreur=OUI;
    etat* courant=this;
    arc* a;
    int ia=0;
    while (courant!=NULL) {
        while(ia<courant->arcs.dernier) {
            //cas speciaux
            erreur=OUI;
            a=courant->arcs[ia];            
            switch(a->action) {
            case AUTOMATONVARIABLE:
                //Le label correspond au numero de la variable
                if (ref!=NULL)
                    ref->majvar(a,i);
            case AUTOMATONEPSILON:
                //on passe directement a l'etat suivant
                //S'il y a d'autres elements possibles en attente
                //On empile notre position
                if (ia<courant->arcs.dernier-1) {
                    pilearcs.ajoute(ia);
                    pileposition.ajoute(i);
                    pileetats.ajoute(courant);
                }
                courant=a->e;      
                ia=0;
                erreur=NON;
                break;
            default:                            
                if (a->test(mot[i])) {
                    if (ia<courant->arcs.dernier-1) {
                        pilearcs.ajoute(ia);
                        pileposition.ajoute(i);
                        pileetats.ajoute(courant);
                    }
                    courant=a->e;                                    
                    i++;
                    ia=0;
                    erreur=NON;
                }
                else
                    ia++;
            } 
        
            if (!mot[i] && courant->end() && erreur==NON) {
                ref->maxpos=i;
                return courant;
            }
        }
                
        courant=pileetats.retire();
        i=pileposition.retire();        
        ia=pilearcs.retire()+1;
    }
    return NULL;
}

etat* arc::recherche(unsigned char* mot,automate* ref,int i,int max) {
    if (mot[i]==0 && e->end())
        return e;           
    return e->recherche(mot,ref,i,max);
}

etat* etat::recherche(unsigned char* mot,automate* ref,int i,int max) {
    if (i+nombre > max)
        return NULL;

    etat* et=NULL;
    for (int ia=0;ia<arcs.dernier;ia++) {
        //cas speciaux
        switch(arcs[ia]->action) {
        case AUTOMATONIF:
            if (arcs[ia]->recherche(mot,ref,i,max))
                return NULL;
            break;
        case AUTOMATONELSE:
        case AUTOMATONVARIABLE:
            //Le label correspond au numero de la variable
            if (ref!=NULL)
                ref->majvar(arcs[ia],i);
            //on passe directement a l'etat suivant
            et=arcs[ia]->recherche(mot,ref,i,max);
            break;
        case AUTOMATONEPSILON:
            et=arcs[ia]->recherche(mot,ref,i,max);
            break;
        default:
            if (arcs[ia]->test(mot[i]))
                et=arcs[ia]->recherche(mot,ref,i+1,max);
        }
        if (et!=NULL)
            return et;
    }
    return NULL;
}

char arc::retirebrute(unsigned char* mot,automate* ref,int i) {
    if (mot[i]==0 && e->end()) {
        //de deux choses l'une, cet arc est le dernier
        //si cet arc est le dernier
        if (e->arcs.dernier==0) {
            ref->etats.affecte(e->id-1,NULL);                
            delete e;
            return 2;
        }
        else {
            e->status&=~AUTOMATONEND; //sinon, on le marque comme n'etant plus un etat final
            e->valeur=NULL;
            return 1;
        }
    }
                   
    char sts=e->retirebrute(mot,ref,i);    
    if (sts==2) {
        //attention, si les arcs sont vides pour l'etat suivant
        //il faut quand meme verifier qu'il n'est pas un etat final
        if (e->arcs.dernier==0 && !e->end()) {
            ref->etats.affecte(e->id-1,NULL);                
            delete e;
            return 2;
        }
    }
    else {
        if (sts==1) {
            e->status&=~AUTOMATONEND; //sinon, on le marque comme n'etant plus un etat final
            e->valeur=NULL;
            return 1;    
        }
    }
    return 0;
}

char etat::retirebrute(unsigned char* mot,automate* ref,int i) {
    
    char sts;
    for (int ia=0;ia<arcs.dernier;ia++) {
        if (arcs[ia]->test(mot[i])) {
            sts=arcs[ia]->retirebrute(mot,ref,i+1);
            if (sts==2) {
                status|=AUTOMATONEND;                
                //on doit retirer cet arc
                arc* a=arcs.retireElement(ia);
                delete a;
                return 2;                
            }            
            else
                if (sts==1)
                    return 1;
        }
    }
    return 0;
}

//----------------------------------------------------------------------------------------
void arc::recherchetous(unsigned char* mot,VECTA<void*>& res,int i,int max) {
    if (mot[i]==0 && e->end()) {
        for (int j=0;j<res.dernier;j++)
            if (res[j]==e->valeur)
                return;
            res.ajoute(e->valeur);
    }
    e->recherchetous(mot,res,i,max);
}

void etat::recherchetous(unsigned char* mot,VECTA<void*>& res,int i,int max) {
    if (i+nombre > max)
        return;
    
    for (int ia=0;ia<arcs.dernier;ia++) {
        //cas speciaux
        switch(arcs[ia]->action) {
        case AUTOMATONIF:
            if (arcs[ia]->recherche(mot,NULL,i,max))
                return;
            break;
        case AUTOMATONELSE:
        case AUTOMATONEPSILON:
            arcs[ia]->recherchetous(mot,res,i,max);
            break;
        default:
            if (arcs[ia]->test(mot[i]))      
                arcs[ia]->recherchetous(mot,res,i+1,max);
        }    
    }
}

void etat::recherchetouslin(unsigned char* mot,VECTA<void*>& res,int i) {

    pilearcs.raz();
    pileposition.raz();
    pileetats.raz();

    char erreur=NON;
    etat* courant=this;
    arc* a;
    int ia=0;
    while (courant!=NULL) {
        while(ia<courant->arcs.dernier) {
            //cas speciaux
            erreur=OUI;
            a=courant->arcs[ia];            
            switch(a->action) {
            case AUTOMATONEPSILON:
                //on passe directement a l'etat suivant
                //S'il y a d'autres elements possibles en attente
                //On empile notre position
                if (ia<courant->arcs.dernier-1) {
                    pilearcs.ajoute(ia);
                    pileposition.ajoute(i);
                    pileetats.ajoute(courant);
                }
                courant=a->e;      
                ia=0;
                erreur=NON;
                break;
            default:                            
                if (a->test(mot[i])) {
                    if (ia<courant->arcs.dernier-1) {
                        pilearcs.ajoute(ia);
                        pileposition.ajoute(i);
                        pileetats.ajoute(courant);
                    }
                    courant=a->e;                                    
                    i++;
                    ia=0;
                    erreur=NON;
                }
                else
                    ia++;
            }            
            if (erreur==NON && courant->end() && mot[i]==0) {
                char trouve=NON;
                for (int j=0;j<res.dernier;j++) {
                    if (res[j]==courant->valeur) {
                        trouve=OUI;
                        break;
                    }
                    
                }
                if (trouve==NON)
                    res.ajoute(courant->valeur);
            }
        }

        courant=pileetats.retire();
        i=pileposition.retire();
        ia=pilearcs.retire()+1;       
    }
}

//----------------------------------------------------------------------------------------
void arc::compose(arc* a,VECTA<void*>& res) {
    if (a->e->end() && e->end()) {
        for (int i=0;i<res.dernier;i++)
            if (res[i]==e->valeur)
                return;
            res.ajoute(e->valeur);
    }
    e->compose(a->e,res);
}

void etat::compose(etat* e,VECTA<void*>& res) {
    int i;
    for (int ia=0;ia<arcs.dernier;ia++) {
        //cas speciaux
        if (arcs[ia]->action==AUTOMATONEPSILON)
            arcs[ia]->e->compose(e,res);
        else {                    
            for (i=0;i<e->arcs.dernier;i++) {
                if (arcs[ia]->test(e->arcs[i]->label))      
                    arcs[ia]->compose(e->arcs[i],res);
            }
        }
    }
}

//----------------------------------------------------------------------------------------
automate::automate(unsigned char* m,void* ind) {    
    dernier=1;        
    parse(m,ind);    
}

automate::automate() {
    dernier=1;
    erreur=AERR_NO_ERROR;    
}


void automate::duplique(automate* a) {    
    etat* et;
    int i,j;    

    e.status=a->e.status;                
    e.valeur=a->e.valeur;
    //Pour tous les etats, on effectue une duplication
    for (i=0;i<a->etats.dernier;i++) {
        if (a->etats[i]!=NULL) {
            et=new etat(this,a->etats[i]->status);                
            et->valeur=a->etats[i]->valeur;
        }
    }
    
    //Pour chaque etat nouveau on duplique les arcs
    for (i=-1;i<a->etats.dernier;i++) {
        //On prend donc un etat a dupliquer
        if (i==-1)
            et=&a->e;
        else
            et=a->etats[i];
        //Pour tous les arcs de cet etat
        for (j=0;j<et->arcs.dernier;j++) {
            //On cree un arc equivalent
            arc* ar=new arc(et->arcs[j]->action,et->arcs[j]->negation,et->arcs[j]->label);
            //qui pointe vers l'etat suivant
            if (et->arcs[j]->e->id>0)
                ar->e=etats[et->arcs[j]->e->id-1];
            else
                ar->e=&e;
            if (i==-1)
                e.arcs.ajoute(ar);
            else
                etats[i]->arcs.ajoute(ar);
        }
    }
}

int automate::compare(automate* a) {

	static char m1[1000];
	static char m2[1000];

    chaine(m1);
    a->chaine(m2);
    return strcmp(m1,m2);
}

void arc::chaine(char* m) {
    char x[] = {label,0};

    if (strlen(m) >= 950)
        return;

    switch (action) {
    case AUTOMATONIF:
        strcat(m,"IF");
        break;
    case AUTOMATONELSE:
        strcat(m,"LS");
        break;
    case AUTOMATONEPSILON:
        strcat(m,"ES");
        break;
    case AUTOMATONANY:
        strcat(m,"?");
        break;
    case AUTOMATONDIGIT:
        strcat(m,"DG");
        break;
    case AUTOMATONPUNCT:
        strcat(m,"PT");
        break;
    case AUTOMATONLETTER:
        strcat(m,"Le");
        break;
    case AUTOMATONLETTERUPPER:
        strcat(m,"LE");
        break;
    case AUTOMATONLABEL:        
        strcat(m,x);
    }
    e->chaine(m);
}


void etat::chaine(char* m) {
    if (strlen(m) >= 950)
        return;

    strcat(m,"<");
    if (start())
        strcat(m,"s");
    if (end())
        strcat(m,"f");
    strcat(m,">");
    for (int i=0;i<arcs.dernier;i++) {    
        arcs[i]->chaine(m);
    }
}

void automate::chaine(char* m) {
    m[0]=0;
    for (int i=0;i<e.arcs.dernier;i++) {    
        e.arcs[i]->chaine(m);
    }
}

void automate::mot(char* m) {
    int i=0;
    etat* et=&e;    
    while (!et->end()) {
        if (et->arcs[0]->action!=AUTOMATONVARIABLE) 
            m[i++]=et->arcs[0]->label;
        et=et->arcs[0]->e;
    }
    m[i]=0;
}

void automatepile::mot(char* m) {
    int i=0;
    etat* et=&e;    
    while (!et->end()) {
        //dans le cas d'une variable, on l'integre dans la chaine
        if (et->arcs[0]->action==AUTOMATONVARIABLE) {            
            strcpy(m+i,(char*)chaines[et->arcs[0]->label]);
            i=strlen(m);            
        }
        else 
            m[i++]=et->arcs[0]->label;
        et=et->arcs[0]->e;
    }
    m[i]=0;
}

arc* etat::trouvearc(unsigned char l,arcautomate a,char neg,char boucle) {
    for (int i=0;i<arcs.dernier;i++) {
        if (arcs[i]->label==l && arcs[i]->negation==neg && arcs[i]->action==a)
            if (!boucle || arcs[i]->e->id<=id)
                return arcs[i];
    }
    return NULL;
}

arc* etat::trouvearc(arc* a) {
    for (int i=0;i<arcs.dernier;i++) {
        if (arcs[i]->label==a->label && 
            arcs[i]->negation==a->negation && 
            arcs[i]->action==a->action &&
            arcs[i]->e==a->e)            
            return arcs[i];
    }
    return 0;
}

void automate::demarque() {
    int j;
    e.marque=0;
    for (j=0;j<e.arcs.dernier;j++)
        e.arcs[j]->marque=0;

    for (int i=0;i<etats.dernier;i++) {
        if (etats[i]!=NULL) {
            etats[i]->marque=0;
            for (j=0;j<etats[i]->arcs.dernier;j++)
                etats[i]->arcs[j]->marque=0;
        }
    }
}

void etat::print(int profondeur) {
    if (marque==1)
        return;
    marque=1;
    char ch[20];
    char chh[20];
    ch[0]=0;
    if (start())
        strcat(ch,"s");
    if (node())
        strcat(ch,"n");
    if (end())
        strcat(ch,"e");
    sprintf(chh,"%d",id);
    strcat(ch,chh);
    sprintf(chh,"=%d",nombre);
    strcat(ch,chh);
    if (end()) {
        sprintf(chh,"[%d]",valeur);
        strcat(ch,chh);       
    }
    cout<<ch;
    profondeur+=strlen(ch);
    for (int i=0;i<arcs.dernier;i++) {
        arcs[i]->print(id,profondeur);
        if (i<arcs.dernier-1) {
            for (int k=0;k<profondeur;k++)
                cout<<" ";
        }
    }

    if (profondeur==0 || end())
        if (!arcs.dernier || !arcs.fin()->e->end())
            cout<<endl;

}

void arc::print(int id,int profondeur) {

    int taille=1;
    char ch[20];
    char x[]={label,0};

    strcpy(ch,"-(");
    if (negation==1) {
        strcat(ch,"~");
        taille++;
    }
    switch (action) {
    case AUTOMATONIF:
        strcat(ch,"IF");
        break;
    case AUTOMATONELSE:
        strcat(ch,"LS");
        break;
    case AUTOMATONEPSILON:
        strcat(ch,"ES");
        break;
    case AUTOMATONANY:
        strcat(ch,"?");
        break;
    case AUTOMATONDIGIT:
        strcat(ch,"DG");
        break;
    case AUTOMATONPUNCT:
        strcat(ch,"PT");
        break;
    case AUTOMATONLETTER:
        strcat(ch,"Le");
        break;
    case AUTOMATONLETTERUPPER:
        strcat(ch,"LE");
        break;
    case AUTOMATONVARIABLE:
        strcat(ch,"%");
        x[0]+=48;
    case AUTOMATONLABEL:        
        strcat(ch,x);
    }
    strcat(ch,")-");
    cout<<ch;
    if (e->marque==0)
        e->print(profondeur+strlen(ch));    
    else {
        cout<<">";
        if (e->start())
            cout<<ch,"s";
        if (e->node())
            cout<<"n";
        if (e->end())
            cout<<"e";
        cout<<e->id<<endl;                            
    }
}


void automate::print() {
    demarque();
    e.print(0); 
    demarque();
    cout<<endl<<endl;
}

char automate::retirebrute(unsigned char* m) {
    //L'ancre permet de determiner la position du premier caractere de la chaine
    //a recopier dans la variable    
    return e.retirebrute(m,this,0);
}


etat* automate::recherche(unsigned char* m) {
    //L'ancre permet de determiner la position du premier caractere de la chaine
    //a recopier dans la variable
    initvar();    
    maxpos=-1;
    return e.recherchelin(m,this,0);
}



etat* automate::recherche(char* m) {
    initvar();    
    maxpos=-1;
    return e.recherchelin((unsigned char*)m,this,0);    
}

void automate::recherchetouslabels(unsigned char* m,VECTA<void*>& res) {
    initvar();
    e.recherchetouslin(m,res,0);
}

void automate::recherchetouslabels(char* m,VECTA<void*>& res) {
    initvar();
    e.recherchetouslin((unsigned char*)m,res,0);
}

void automate::compose(automate& a,VECTA<void*>& res) {e.compose(&(a.e),res);}


void etat::aupluspres() {
    if (marque==1)
        return;
    marque=1;
    arcs.aupluspres();

    for (int i=0;i<arcs.dernier;i++)
        arcs[i]->aupluspres(id);
}

void arc::aupluspres(int id) {
    if (marque==1)
        return;
    marque=1;
    e->aupluspres();
}

void automate::retirefordeleted(VECTA<arc*>& adetruire) {
	etat* courant;
	arc* ar;
	for (int i=-1;i<etats.dernier;i++) {
		if (i==-1)
			courant=&e;
		else
			courant=etats[i];
		for (int j=courant->arcs.dernier-1;j>=0;j--) {
			ar=courant->arcs[j];
			if (ar->manipulation==AUTOMATONMARKEDFORDELETION || ar->manipulation==AUTOMATONDELETED) {				
				if (ar->manipulation==AUTOMATONMARKEDFORDELETION) {
					adetruire.ajoute(ar);
					ar->manipulation=AUTOMATONDELETED;
				}
				courant->arcs.retireElement(j);
			}
		}
	}
}

void automate::aupluspres() {
	VECTA<arc*> adetruire;
    demarque();
    retireEpsilon(adetruire);
    demarque();
    retireEpsilon(adetruire);
    demarque();
	fusion(adetruire);	
	demarque();
	e.CalculChemin(0);
	for (int i=0;i<adetruire.dernier;i++)
		delete adetruire[i];
}

automate::~automate() {
    marquedestruction();
    etats.nettoie();
}

void automate::nettoie() {
    marquedestruction();
    etats.nettoie(); 
    e.arcs.raz();
    dernier=1;
}

void automatepile::extraitvariables(unsigned char* ch,unsigned char souschaines[10][TailleChaine]) {
    int prem=0;
    char mot[TailleChaine];
    int i;
    for (i=1;i<10;i++)
        souschaines[i][0]=0;
    for (i=1;i<=maxvar;i++) {
        if (variables[i]!=-1) {
            int taille=variables[i]-prem;
            if (taille>TailleChaine)
                taille=TailleChaine-1;
            memcpy(mot,ch+prem,taille);
            mot[taille]=0;
            strcpy((char*)souschaines[i],(char*)mot);
            prem=variables[i];
        }
    }
}

void automatepile::extraitvariables(unsigned char* ch,automate* a) {
    if (a->Type()!=AUTOMATEPILE)
        return;
    automatepile* b=(automatepile*)a;
    int prem=0;
    char mot[TailleChaine];
    int i;
    b->raz();
    for (i=1;i<=maxvar;i++) {
        if (variables[i]!=-1) {
            int taille=variables[i]-prem;
            if (taille>TailleChaine)
                taille=TailleChaine-1;
            memcpy(mot,ch+prem,taille);
            mot[taille]=0;
            strcpy((char*)b->chaines[i],(char*)mot);
            prem=variables[i];
        }
    }
}

void automatepile::extraction(unsigned char* ch) {
    int prem=0;
    char mot[TailleChaine];    
    int i;
    for (i=1;i<10;i++)
        chaines[i][0]=0;
    for (i=1;i<=maxvar;i++) {
        if (variables[i]!=-1) {
            int taille=variables[i]-prem;
            if (taille>TailleChaine)
                taille=TailleChaine-1;
            memcpy(mot,ch+prem,taille);
            mot[taille]=0;
            strcpy((char*)chaines[i],(char*)mot);            
            prem=variables[i];
        }
    }
}


void automatepile::affecte(unsigned char souschaines[10][TailleChaine]) {
    for (size_t i=1;i<10;i++) {
        if (souschaines[i][0])
            strcpy((char*)chaines[i],(char*)souschaines[i]);
    }
}


void automatepile::initvar() {
    maxvar=0;
    for (int i=0;i<10;i++) 
        variables[i]=-1;
}

void automatepile::majvar(arc* a,int i) {
    variables[a->label]=i;
    if (maxvar<a->label)
        maxvar=a->label;
}

char automatepile::construitvar(unsigned char lab,int position) {
    if (variables[0]!=100) {
        if (lab<=variables[0]) {
            initErreur(position,AERR_WRONG_VARIABLE_ORDER);
            return NON;
        }
        variables[0]=lab;
    }
    return OUI;
}

void automatepile::raz() {
    for (size_t i=0;i<10;i++) {
        chaines[i][0]=0;
    }
}


int arc::CalculChemin(int max) {
    if (marque==1)
        return -1;
    marque=1;    
    
    int ret=e->CalculChemin(max);
    
    if (ret==-1)
        return -1;
    
    marque=-1;   
    
    if (action==AUTOMATONEPSILON)    
        return ret;

    return ret+1;
}

int etat::CalculChemin(int max) {
    if (marque==1)
        return -1;
    marque=1;

    char debut=OUI;
    int minarc=0;
    int i;
    char debutcle=OUI;    
    for (i=0;i<arcs.dernier;i++) {
                
        if (arcs[i]->marque==1)
            continue;        
        
        int nb=arcs[i]->CalculChemin(0);
        
        if (nb==-1)
            continue;

        if (debut==OUI || nb<minarc) {
            minarc=nb;
            debut=NON;
            debutcle=NON;
        }
    }           
    

    if (!end())
        nombre=minarc+max;
    else
        nombre=max;

    marque=-1;
    return nombre;
}



void arc::dedoublonne() {
    if (marque==1)
        return;
    marque=1;
    e->dedoublonne();
}


void etat::dedoublonne() {
    if (marque==1)
        return;
    marque=1;
    
    int i=0;
    int j;
    VECTA<int> arcsaretirer;
    //Tout d'abord on retire les arcs dupliques
    for (i=0;i<arcs.dernier;i++) {
        for (j=i+1;j<arcs.dernier;j++) {
            if (arcs[j]->label!=arcs[i]->label || 
                arcs[j]->action!=arcs[j]->action)
                break;
            if (arcs[i]->e==arcs[j]->e) {
                if (arcsaretirer.cherche(j)==-1)
                    arcsaretirer.ajoute(j);
            }
        }
    }
    //On retire nos arcs
    for (i=arcsaretirer.dernier-1;i>=0;i--)
        arcs.retireElement(arcsaretirer[i]);
      
    for (i=0;i<arcs.dernier;i++) 
        arcs[i]->dedoublonne();
}


void arc::fusion(VECTA<arc*>& adetruire) {
    if (marque==1)
        return;
    marque=1;
    e->fusion(adetruire);
}


void etat::fusion(VECTA<arc*>& adetruire) {
    if (marque==1)
        return;
    marque=1;
    
    int i=0;
    int j;
    VECTA<int> arcsaretirer;

    while (i<arcs.dernier) {        
        j=i+1;
        //Nous savons que les noeuds sont deja ordonnes en fonction de leur label
        while (j<arcs.dernier &&
            arcs[j]->label==arcs[i]->label && 
            arcs[j]->action==arcs[i]->action)
            j++;
        //
        //si nous avons au moins deux noeuds semblables
		etat* et=arcs[i]->e;
		int larc;
		for (int k=i+1;k<j;k++) {
			//Nous prenons tous les arcs pointes par k et nous les repartissons sur le noeud 
			//pointe par i
			etat* etbis=arcs[k]->e;
			if (etbis!=NULL) {
				if (arcs[k]->manipulation!=AUTOMATONDELETED)
					arcs[k]->manipulation=AUTOMATONMARKEDFORDELETION;

				for (larc=0;larc<etbis->arcs.dernier;larc++) {                    
					//d'abord on verifie que cet arc n'a pas deja ete copie
					if (et->arcs.cherche(etbis->arcs[larc])==-1)
						et->ajoutearc(etbis->arcs[larc]);
				}
				//cet arc k devra par la suite etre retire
				arcsaretirer.ajoute(k);
				et->status|=etbis->status;
			}
		}
		//Puis, comme il y peut y avoir des arcs partages, on les reinitialise
		for (larc=0;larc<et->arcs.dernier;larc++) {                    
			//d'abord on verifie que cet arc n'a pas deja ete copie
			et->arcs[larc]->manipulation=AUTOMATONNONE;
		}
        i=j;
    }

    //On retire nos arcs
	for (i=arcsaretirer.dernier-1;i>=0;i--) {
		arc* a=arcs.retireElement(arcsaretirer[i]);
		if (a->manipulation==AUTOMATONMARKEDFORDELETION) {
			adetruire.ajoute(a);
			a->manipulation=AUTOMATONDELETED;
		}
	}

    for (i=0;i<arcs.dernier;i++) 
        arcs[i]->fusion(adetruire);
}


void automate::fusion(VECTA<arc*>& adetruire) {
    demarque();
    e.fusion(adetruire);
    demarque();    
	retirefordeleted(adetruire);
}


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

#define uchar unsigned char

int main(int argc, char* argv[]) {

	compileautomaton("C:\\XIP\\Test\\toksrc\\english\\english.bse","C:\\XIP\\Test\\toksrc\\english\\autoeng.ar",2500000);
	//transducteur* a=new transducteur("C:\\XIP\\Test\\toksrc\\French\\lex.trs");	
	//lecteur* l=a->fournir(1,"C:\\XIP\\Test\\toksrc\\French\\lib.txt");
	//lecteur* l=a->fournir(0,"C'est un exemple.");

	//a->lire(l);
	//etat* e;
	//lesautomates.ajoute(new automate);	
	//e=lesautomates[0]->parse((unsigned char*)"a*",(void*)1);
	//lesautomates[0]->print(cout);
	//lesautomates[0]->aupluspres();
	//cout<<endl<<endl;
	//lesautomates[0]->print(cout);
	//e=lesautomates[0]->recherche("");
	//
	//lesautomates.ajoute(new automate);	
	//e=lesautomates[1]->parse((unsigned char*)"[ab]+",(void*)1);
	//lesautomates[1]->print(cout);
	//lesautomates[1]->aupluspres();
	//cout<<endl<<endl;
	//lesautomates[1]->print(cout);
	
	return 0;
}

