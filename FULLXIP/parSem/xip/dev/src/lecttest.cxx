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
filename   : lecttest.cxx
Date       : 10/01/1999
Purpose    : Compiling and parsing of a test after a "if"
Programmer : Claude ROUX
Reviewer   : 
*/

#include "parsbin.h"

extern char afficheautomate;
int rechercheNomFonction(char* nom,const char* table[]);
extern const char* TableOperationChaine[];
/*-------------------------------------------------------------------------*/
/* Petite explication de l'utilisation des champs dans Traits:
valeur: contient la valeur que l'on doit affecter ou trouver: tr=+
filtre: fonctionne de pair avec valeur. Il contient l'ensemble des valeurs possibles pour un attribut
bloquage: il s'agit du bloquage d'un trait libre: tr:!
libre:   traits qui doivent etre liberes: !tr:!
negation: traits dont on doit pas trouver la valeur: tr:~
present:    traits (a l'extremite de l'arbre des traits) qui doivent etre present: tr:x
lpresent:   traits de traits qui doivent etre present
efface: traits qui doivent etre effaces: tr=~
permanent: traits qui ne peuvent etre effaces: tr:=..
*/
char testliste(trait* tr) {
	if (tr->type==LISTEDUPLICAT || tr->type==LISTE)
		return OUI;
	return NON;
}

#define PERMANENT 100
Traits* GlobalParseur::BatitTraits(istream& fichier,char finCaractere,VECTA<char*>* pile,char* clearStack) {
    Traits* resultat=new Traits(this); 
    trait* vattribut; 
	char tampon[XIPBUFFERSIZE];
    char k=' ',test,affecter,option,libre;

    //par defaut finCaractere==']'
    while (k!=finCaractere) {
        k=lit(fichier,tampon);
        if (k=='(') {
            option=OUI;
            k=lit(fichier,tampon);
        } 
        else
            option=NON;
        
        if (!strcmp(tampon,"$stack")) {
            if (k!='=' && k!=':') {
                if (clearStack==NULL)
                    erreur(CHAINES[135]);
                else
                    *clearStack=STACKNONVIDE;
                continue;
            }
            char operateur=k;
            k=lit(fichier,tampon);
            if (pile!=NULL) {
                char neg=NON;
                if (k=='~') {
                    k=lit(fichier,tampon);
                    if (tampon[0]==0) {
                        if (clearStack!=NULL)
                            *clearStack=OUI;
                        continue;
                    }
                    neg=OUI;
                }
                
                if (tampon[0]=='-' && tampon[1] >= '1' && tampon[1]<='9') {
                    if (clearStack!=NULL)
                        *clearStack=48-tampon[1];
                }
                else {
                    string suite="\"";
                    //Le code est le suivant dans suite: 
                    //   suite[0]=!  --> c'est peut-etre une variable
                    //   suite[0]=~  --> c'est une negation
                    //   suite[0]="  --> c'est une chaine ne rien faire
                    
                    suite="\"";
                    if (neg==OUI)
                        suite="~";
                    else
                        if (LuEntreCotes==0)
                            suite="!";

                    suite+=tampon;   
                    pile->ajoute(DupliqueChaine(STR(suite)));
                }
            }
            continue;
        }
        

        test=2;
        libre=NON;
        if (k=='!') {
            test=3;
            libre=OUI;
            k=lit(fichier,tampon);
        }
    
		//lecture d'un chemin d'ontologie
        if (k=='@') {            
            ParseOntologie(fichier,resultat,k,libre);                        
            continue;
        }

        //Cas particulier, nous nous interessons au seul filtre
        //seul cas d'utilisation dans la comparaison entre noeud
        //dans les regles d'extraction: #1[genre]:#2[genre]
        if (k==',' || k==finCaractere) {
            if (LEMME!= NULL && !strcmp(tampon,LEMME)) {
                if (resultat->lemme==NULL)
                    resultat->lemme=new automate((uchar*)"\\*",0);
                else {
                    delete resultat;
                    erreur(CHAINES[136]);
                }
                continue;
            }
            
            if (SURFACE!= NULL && !strcmp(tampon,SURFACE)) {
                if (resultat->surface==NULL)
                    resultat->surface=new automate((uchar*)"\\*",0);
                else {
                    delete resultat;
                    erreur(CHAINES[136]);
                }
                continue;
            }
            
            //recherche du nom de l'attribut
            vattribut=ArbTraits->recherche(tampon);
            if (!vattribut) {
                delete resultat;
                erreur(Concatene(tampon,CHAINES[124]));
            }
            
            //Si vattribut->type==LISTE, il s'agit d'une liste d'attribut
            if (testliste(vattribut)) {
                if (resultat->lpresent)
                    erreur(CHAINES[45]);
                if (!resultat->lpresent) 
                    resultat->lpresent=new Vecteur(iTailleVect);
                (resultat->lpresent)->ou(vattribut->valeur);                
            }
            else {
                if (!resultat->present) 
                    resultat->present=new Vecteur(iTailleVect);
                (resultat->present)->ou(vattribut->valeur);                
            }
            continue;
        }
        
        if (k!=':' && k!='=') {
            delete resultat;
            erreur(CHAINES[135]);
        }
        
        //Deux possibilites: attribut:valeur ou attribut=valeur
        affecter=NON;
        if (k=='=') 
            affecter=OUI; 

        static VECTA<char> variables_utilisees;
        string inter;
        etat* e;

        if (LEMME!= NULL && !strcmp(tampon,LEMME)) {                
            k=litSans(fichier,tampon);  
            if (k==NEGATION) {
                if (affecter==OUI)
                    erreur(CHAINES[274]);
                resultat->maniplemme=NEG;
                k=litSans(fichier,tampon);
            }
            
            if (resultat->lemme==NULL) {
                if (LuEntreCotes==NON) {
                    inter=tampon;
                    MetEnMinuscule(USTR(inter));
                    e=VariablesNoms.recherche(STR(inter));  
                    if (e!=NULL || rechercheNomFonction(tampon,TableOperationChaine)!=-1) {
                        //On verifie s'il s'agit d'une formule    
                        MetEnAttenteLecture(k,tampon);
                        resultat->varlemme=LireUneFormule(fichier,k,variables_utilisees,2);
                    }
                }

                if (resultat->varlemme!=NULL) {
                    if (resultat->maniplemme==NEG)
                        resultat->maniplemme=VARNEG;
                    else {
                        if (affecter==OUI)
                            resultat->maniplemme=VARECRASE;
                        else
                            resultat->maniplemme=VARCOMPARE;
                    }
                    
                    continue;                  
                }
                
                //cas general
                if (typeautomate((uchar*)tampon,0)==AUTOMATEPILE)
                    resultat->lemme=new automatepile;
                else
                    resultat->lemme=new automate;

				if (tablelatin) {
					agnostring m(tampon);
					m = m.latintoutf8(tablelatin);
					strcpy_s(tampon,1000, STR(m));
				}

                if (resultat->lemme->parse((uchar*)tampon,NULL,NON)==NULL)
                    RenvoieErreurAutomate(resultat->lemme->erreur);
            }
            else {
                delete resultat;
                erreur(CHAINES[136]);
            }
            
            if (affecter==OUI)
                resultat->maniplemme=ECRASE;
            
            continue;
        }
        
        
        if (SURFACE!= NULL && !strcmp(tampon,SURFACE)) {
            k=litSans(fichier,tampon);      
            if (k==NEGATION) {
                if (affecter==OUI)
                    erreur(CHAINES[274]);
                resultat->manipsurface=NEG;
                k=litSans(fichier,tampon);
            }
            
            if (resultat->surface==NULL) {
                if (LuEntreCotes==NON) {
                    inter=tampon;
                    MetEnMinuscule(USTR(inter));
                    e=VariablesNoms.recherche(STR(inter));  
                    //On verifie s'il s'agit d'une formule    
                    if (e!=NULL || rechercheNomFonction(tampon,TableOperationChaine)!=-1) {
                        MetEnAttenteLecture(k,tampon);
                        resultat->varsurface=LireUneFormule(fichier,k,variables_utilisees,2);
                    }
                }
                
                if (resultat->varsurface!=NULL) {
                    if (resultat->manipsurface==NEG)
                        resultat->manipsurface=VARNEG;
                    else {
                        if (affecter==OUI)
                            resultat->manipsurface=VARECRASE;
                        else
                            resultat->manipsurface=VARCOMPARE;
                    }
                    
                    continue;                                    
                }

                if (typeautomate((uchar*)tampon,0)==AUTOMATEPILE)
                    resultat->surface=new automatepile;
                else
                    resultat->surface=new automate;        

				if (tablelatin) {
					agnostring m(tampon);
					m = m.latintoutf8(tablelatin);
					strcpy_s(tampon,1000, STR(m));
				}

                if (resultat->surface->parse((uchar*)tampon,NULL,NON)==NULL)
                    RenvoieErreurAutomate(resultat->surface->erreur);
            }
            else {
                delete resultat;
                erreur(CHAINES[136]);
            }
            
            if (affecter==OUI)
                resultat->manipsurface=ECRASE;
            
            continue;
        }
        
        //recherche du nom de l'attribut
        vattribut=ArbTraits->recherche(tampon);
        if (!vattribut) {
            delete resultat;
            erreur(Concatene(tampon,CHAINES[124]));
        }
        
        //lecture de la valeur du trait: les caracteres suivants
        //une negation: valeur:~
        //une virgule ou un !
        k=lit(fichier,tampon);
		if (k=='=' && affecter==NON) {
			affecter=PERMANENT;
			k=lit(fichier,tampon);
		}

        if (k!=',' && k!=finCaractere && k!=NEGATION && k!='!' && k!='#') {
            delete resultat;
            erreur(CHAINES[134]);
        }
        
        
        //le caractere suivant est un !
        //  !trait:! liberation de trait
        //  trait:! bloquage du trait
        if (k=='!') {
            if (affecter) {
                delete resultat;
                erreur(CHAINES[63]);
            }
            if (libre) {
                if (!resultat->libre) 
                    resultat->libre=new Vecteur(iTailleVect);
                (resultat->libre)->ou(vattribut->valeur);
            }
            else {
                if (!resultat->bloquage) 
                    resultat->bloquage=new Vecteur(iTailleVect);
                (resultat->bloquage)->ou(vattribut->valeur);
            }
            k=lit(fichier,tampon);
        }
        else {
			if (vattribut->permanent==OUI && affecter==NON)
				affecter=PERMANENT;
            //Test sur la presence d'un trait
            if (k=='#') {
                (resultat->valeur)->ou(vattribut->valeur);
                k=lit(fichier,tampon);
                continue;
            }
            
            //Test sur le trait universel: trait:?
            if (k!=NEGATION && (!affecter || tampon[0]=='?')) {
				if (affecter==PERMANENT) {
                    if (!resultat->permanent) 
                        resultat->permanent=new Vecteur(iTailleVect);
                    (resultat->permanent)->ou(vattribut->valeur);
				}
				else {
					if (testliste(vattribut)) {
						if (resultat->lpresent)
							erreur(CHAINES[45]);
						if (!resultat->lpresent) 
							resultat->lpresent=new Vecteur(iTailleVect);
						(resultat->lpresent)->ou(vattribut->valeur);                
					}
					else {
						if (!resultat->present) 
							resultat->present=new Vecteur(iTailleVect);
						(resultat->present)->ou(vattribut->valeur);
					}
				}
				if (tampon[0]=='?') 
					continue;
			}
            
            //Cas d'une negation trouvee
            if (k==NEGATION) {
                k=lit(fichier,tampon);
                //Premier cas, si le signe egal etait mentionne, 
                //c'est un effacement: attribut=NEGATION.
                //Tous les traits sont effaces...
                if (affecter) {
                    if (!resultat->efface)
                        resultat->efface=new Vecteur(iTailleVect);
                    test=0;
                }
                else {
                    if (!resultat->negation)
                        resultat->negation=new Vecteur(iTailleVect);
                    if (tampon[0]!='\0') 
                        test=1; 
                    else 
                        test=0;
                }
            }
            
            if (test) {
                int indice = RetrouveTrait(vattribut,tampon,OUI);
                
                if ((resultat->filtre)->contient(vattribut->valeur)) {
                    delete resultat;
                    erreur(CHAINES[68]);
                }
                if (test>=2) {
                    resultat->valeur->un(this,indice);
					//Pour que cette valeur soit permanente
					if (affecter==PERMANENT) {
						if (!resultat->permanent) 
							resultat->permanent=new Vecteur(iTailleVect);
						resultat->permanent->un(this,indice);
					}
                    
                    if (libre) {
                        if (!resultat->libre) 
                            resultat->libre=new Vecteur(iTailleVect);
                        (resultat->libre)->ou(vattribut->valeur);
                    }
                    (resultat->filtre)->ou(vattribut->valeur);
                }
                else {
                    resultat->negation->un(this,indice);
                }
            }
            else {
                //dans le cas de la regle: attribut=NEGATION
                if (affecter)
                    (resultat->efface)->ou(vattribut->valeur);
                else
                    (resultat->negation)->ou(vattribut->valeur);
            }
        }
    }
    return resultat;
}

Traits* GlobalParseur::TraduitChaineTraits(string& traits) {
	if (traits[0]=='[')
		traits=traits.substr(1,traits.size()-1);
	istringstream is(traits);
	Traits* tr=NULL;
	try {
		tr=BatitTraits(is,']');
	}
	catch(XIPError* lerreur) {
		return NULL;
	}
	return tr;
}

ArcArbre* GlobalParseur::ConstruitArc(istream& fichier,
                                      char* tampon,
                                      char& k,
                                      typeRegle regle,
                                      char negation,
                                      char& optionnel,
                                      char& modification,
                                      EtatArbre* lesuivant,
                                      char caracterefinal) {
    
    char neg=negation;    
    char desamb=NON;
    int iter=0;
    int num_arg=-1;
    Traits* trcourant=NULL;
    ArcArbre* element=NULL;
    char enProfondeur=NON;
    Categorie* c;
	char suite[XIPBUFFERSIZE];
    Traits* selection=NULL;
    Traits* selectiontous=NULL;
    
    optionnel=NON;
    char relire=NON;
    
    suite[0]=0;
    if (k==NEGATION) {
        if (tampon[0]==0) {
            //Negation
            neg=1-negation;
            k=lit(fichier,tampon);
            modification=OUI;
        }
        else
            relire=OUI;
    }
    
    //Il s'agit d'un caractere de controle
    //on repart
    if (tampon[0]==0 && k!='#')
        return NULL;
    
    if (k=='^') {
        enProfondeur=OUI;
        k=lit(fichier,suite);
        modification=OUI;
    }
    
    int l=strlen(tampon);
    desamb=NON;
	char chiffre=NON;
    int pos=-1;
    for (int i=0;i<l;i++) {
		if (strchr("+*%-",tampon[i])==NULL) {
			if (pos==-1)
				continue;
			if (chiffre==OUI && isdigit(tampon[i])) {
				chiffre=NON;
				continue;
			}
			sprintf_s(suite,1000,"%s: %s",CHAINES[193],tampon);
			erreur(suite);
		}
        if (tampon[i]=='*' || tampon[i]=='+' || tampon[i]=='-') {
            if (pos==-1)
                pos=i;
            optionnel=tampon[i];
        }
        else
            if (tampon[i]=='%') {
                if (pos==-1)
                    pos=i;
                //On verifie la presence d'un chiffre apres le %
				if (tampon[i+1]>='1' && tampon[i+1]<='9') {
                    desamb=tampon[i+1]-48;
					chiffre=OUI;
				}
                else
                    desamb=OUI;
            }
    }    
    
    
	if (pos!=-1)
		tampon[pos]=0;

    if (k=='#') {
        //nous lisons le numero de l'argument...
        k=lit(fichier,suite);
        modification=OUI;
        //Une variable
        num_arg=atoi(suite);
		suite[0]=0;
    }
    
    if (k=='<') {
        k=litBis(fichier,suite);
        modification=OUI;
        //S'il s'agit d'un selectiontous
        if (k=='*') {
            selectiontous=BatitTraits(fichier,'>');
            k=lit(fichier,suite);
            if (k=='<') {
                selection=BatitTraits(fichier,'>');
                k=lit(fichier,suite);
            }
        }                
        else{
            MetEnAttenteLecture(k,suite);
            selection=BatitTraits(fichier,'>');            
            k=lit(fichier,suite);
        }
    }
    
    
    if (k=='[') {
        modification=OUI;
        trcourant=BatitTraits(fichier);            
        k=lit(fichier,suite);
    }
    
    /*
    if (!strchr(",.;|(){}=~",k)) {
        sprintf_s(suite,1000,"%s %c",CHAINES[36],k);
        erreur(suite);
    }
    */

	if (suite[0]!=0) {
		sprintf_s(tampon,1000,"%s: %s",CHAINES[193],suite);
		erreur(tampon);
	}

    if (!strchr(",;.{(~=|",k) && k!=caracterefinal) {
        sprintf_s(suite,1000,"%s %c",CHAINES[36],k);
        erreur(suite);
    }

    c=CatVide;
    if (tampon[0])
        c=ChercheCat(tampon);        
    
    if (regle==REGLES_RECONSTRUCTION_MODELE && c!=CatVide) {
        //Il s'agit d'une modification d'un noeud
        //dans le cadre d'une regle de reconstruction
        //pour lui donner une nouvelle categorie
        if (trcourant==NULL)
            trcourant=new Traits(c->traits);
        else
            trcourant->ou(c->traits);
    }
    
    trcourant=FactoriseTraits(trcourant);

    //si une queue de regle a deja ete calcule, il n'est pas besoin d'en
    //generer une nouvelle version
    element=listeEtatArbre.allouearc(&c->X,trcourant,num_arg);
    element->etatarbre=lesuivant;
        
    element->selection=FactoriseTraits(selection);
    element->selectiontous=FactoriseTraits(selectiontous);
    element->negation=neg;
    
    //dans le cas ou il s'agit d'une exploration en profondeur non bornee
    //on conserve le pere du sous-arbre dans le champ suivantbis...
    //if (sous!=NULL && sous->profondeurNonBornee==OUI)
    //  sous->suivantbis=element;
    
    element->profondeurNonBornee=enProfondeur;    
    element->desamb=desamb;
    //On reinitialise le tampon, pour d'autres lectures
    tampon[0]=0;
    if (relire==OUI) {
        k=lit(fichier,tampon);
        modification=OUI;
    }
    
    return element;
}


void TraitementArcArgument(GlobalParseur* parseur,ArcArbre* element,typeRegle regle,SeqFonc* fonc) {
    int num_arg=element->argument;
    int arg; 
    Fonction* fFonc=NULL;
    
    if (num_arg==-1)
        return;
    
    
    if (fonc->type()!=REGLES_SEQUENCE)
        fFonc=(Fonction*)fonc;
    
    //Nous avons un argument lie avec une variable de type #num.
    
    arg=-1;
    
    //Nous recherchons la position de cet argument dans la
    //liste deja construite
    //de la fonction syntaxique. 
    //si l'argument est absent
    for (int i=0;i<fonc->nb_args;i++)
        if (fonc->arg[i]==(size_t)num_arg) {
            arg=i;
            if (fFonc!=NULL)                            
                fFonc->marque.affecte(i,1);
            break;
        }
        
        if (arg==-1) {
            //nous n'avons pas trouve cet argument, dans ce cas,
            //il ne s'agit pas d'une erreur
            //mais simplement d'un argument supplementaire que l'on 
            //enregistre a la fin de la liste
            //des arguments de la fonction... Une marque speciale est 
            //enregistree
            //pour s'en souvenir
            //les valeurs sont pour marque: 
            // 1 argument normal
            // 2 argument supplementaire a ne pas afficher...
            if (regle==VIDE || regle==REGLES_DEDUCTION || regle==REGLES_EXTRACTION) {
                if (regle!=REGLES_DEDUCTION)
                    arg=fonc->nb_args;
                else
                    arg=num_arg;
                if (fFonc!=NULL)
                    fFonc->marque.affecte(arg,2);
                fonc->arg.affecte(arg,num_arg); 
                //on initialise la valeur avec num_arg...
                ++fonc->nb_args; //on incremente le nombre d'argument...
            }
            //dans le cas des regles de simplification, on rajoute quelques contraintes:
            //a) la valeur #1 est par defaut la valeur du noeud que l'on simplifie, elle ne
            //peut donc etre utilise pour marquer un element du contexte.
            //b) la variable associee n'a aucun lien avec des arguments inexistants d'une fonction
            //il s'agit forcement d'un contexte dans le cas d'une regle de simplification
            else {
                arg=num_arg;
                if (regle==REGLES_RECONSTRUCTION || 
                    regle==REGLES_RECONSTRUCTION_MODELE &&
                    fFonc!=NULL)
                    fFonc->marque.affecte(arg,OUI);
                if (regle==REGLES_SIMPLIFICATION && arg==0)
                    parseur->erreur(parseur->CHAINES[104]);
            }                        
        }
        
        element->argument=arg;
}

void GlobalParseur::LireAutomate(istream& fichier,                                                                                        
                                 char fin,
                                 SeqFonc* fonc,
                                 typeRegle regle,
                                 VECTA<ArcArbre*>& liste,
                                 char sens) {
	char k, tampon[XIPBUFFERSIZE];
    ArcArbre* element=NULL;
    ArcArbre* fictif=NULL;
    int i;
    char disjonction=NON;
    char neg=NON;
    char ajoutelement=OUI;
    
    Sequence* seq=NULL;
    Fonction* fFonc=NULL;
    
    if (fonc->type()==REGLES_SEQUENCE)
        seq=(Sequence*)fonc;
    else
        fFonc=(Fonction*)fonc;
    
    k=0;
    VECTA<char> pile;
    pile.ajoute(fin);
    k=lit(fichier,tampon);    
    
    if (seq!=NULL && seq->passageLexical!=-1 && k=='~') {
        neg=OUI;
        k=lit(fichier,tampon);
    }
    
    if (fin=='|' && k=='@') {
        fonc->deterministe=longue;
        k=lit(fichier,tampon);
    }
    
    if (fin=='|' && k=='!') {
        fonc->deterministe=courte;
        k=lit(fichier,tampon);
    }

    if (seq!=NULL && seq->passageLexical!=-1 && k=='|' && tampon[0]==0) {
        seq->gauche=LitUneSequence(fichier,seq->Fx,neg,-1,regle);        
        k=lit(fichier,tampon);
    }
    
    VECTA<int> pilepos;
    

	while (!fichier.eof()) {
		neg=NON;
        if (k==NEGATION)
            neg=OUI;
        char negation=NON;
        char optionnel=0;
        char modification;

        if (regle==REGLES_SEQUENCE || regle==REGLES_EXTRACTION || regle==REGLES_RECONSTRUCTION_MODELE) {
            if (!strcmp(tampon,"where") && k=='(') {
                TestArbre* tarbre=NULL;
                static VECTA<char> vari(5);
                //lecture du test... Il s'agit d'un test en debut de definition            
                tarbre=new TestArbre;
                k=' ';
                LectureTest(fichier,k,tampon,NON,vari,&tarbre);
                TVecteur clefArbre;
                if (fonc->listes!=NULL)
                    erreur(CHAINES[10]);
                fonc->listes=new ListeTestArbre;
                tarbre->direct=tarbre->LineariseDirect(this,clefArbre,*fonc->listes);
                delete tarbre;                

                //On retire alors la virgule qui precede le where
                if (sens==-1) {
                    if (liste[0]->X==NULL)
                        liste.retireElement(0);
                }
                else {
                    if (liste.fin()->X==NULL)
                        liste.retire();
                    }
                break;
            }
        }

        element=ConstruitArc(fichier,tampon,k,regle,negation,optionnel,modification,NULL,pile.fin());

		if (k==NEGATION) {
			neg=OUI;
			k=lit(fichier,tampon);
		}

        if (element!=NULL) {
            TraitementArcArgument(this,element,regle,fonc);       
            if (sens==-1)
                liste.insere(0,element);
            else
                liste.ajoute(element);
        }
        else {
            if (neg==OUI) {
                fictif=listeEtatArbre.allouearc(NULL,NULL,'~');
                if (sens==-1)
                    liste.insere(0,fictif);
                else
                    liste.ajoute(fictif);
            }
        }

        if (optionnel!=0) {
            fictif=listeEtatArbre.allouearc(NULL,NULL,optionnel);
            if (sens==-1)
                liste.insere(1,fictif);
            else
                liste.ajoute(fictif);
        }
        char carac;                       
        if (k=='{' || k=='(' || k==')' || k=='}' || k=='[' || k==']' || k=='<' || k=='>') {
            int icourant=-1;
            VECTA<ArcArbre*> sousliste;
            fictif=NULL;
            carac=k;
            //On inverse le sens des parentheses de toute sorte            
            switch(k) {                
            case '{':
                fictif=listeEtatArbre.allouearc(NULL,NULL,'{');
                LireAutomate(fichier,'}',fonc,regle,sousliste,1);
                break;
            case '(':
                pilepos.ajoute(liste.dernier);
                pile.ajoute(')');
                if (sens==-1)
                    carac=')';
                fictif=listeEtatArbre.allouearc(NULL,NULL,carac);
                break;
            case '<':
                pilepos.ajoute(liste.dernier);
                pile.ajoute('>');
                if (sens==-1)
                    carac='>';
                fictif=listeEtatArbre.allouearc(NULL,NULL,carac);
                break;
            case '[':
                pilepos.ajoute(liste.dernier);
                pile.ajoute(']');
                if (sens==-1)
                    carac=']';
                fictif=listeEtatArbre.allouearc(NULL,NULL,carac);
                break;
            case ')':
                if (sens==-1)
                    carac='(';
                fictif=listeEtatArbre.allouearc(NULL,NULL,carac);                
                pile.retire();
                icourant=liste.dernier-pilepos.retire()+1;
                break;
            case '>':
                if (sens==-1)
                    carac='<';
                fictif=listeEtatArbre.allouearc(NULL,NULL,carac);                
                pile.retire();
                icourant=liste.dernier-pilepos.retire()+1;
                break;
            case ']':
                if (sens==-1)
                    carac='[';
                fictif=listeEtatArbre.allouearc(NULL,NULL,carac);                
                pile.retire();
                icourant=liste.dernier-pilepos.retire()+1;
                break;
            }                        

            if (k=='{') {
                if (sens==-1) {
                    for (i=sousliste.dernier-1;i>=0;i--)
                        liste.insere(1,sousliste[i]);
                    liste.insere(1,fictif);
                }
                else {
                    liste.ajoute(fictif);
                    for (i=0;i<sousliste.dernier;i++)
                        liste.ajoute(sousliste[i]);
                }
            }
            else {
                if (fictif!=NULL) {
                    if (sens==-1)
                        liste.insere(0,fictif);            
                    else 
                        liste.ajoute(fictif);            
                }
            }
        
            if (k==')' || k==']' || k=='>') {
                k=lit(fichier,tampon);            
                if (tampon[0]=='*' || tampon[0]=='+' || tampon[0]=='-' && icourant!=-1) {
                    fictif=listeEtatArbre.allouearc(NULL,NULL,tampon[0]);
                    if (sens==-1)
                        liste.insere(icourant,fictif);                                
                    else
                        liste.ajoute(fictif);
                }
                else
                    continue;
            }
        }

        if (seq!=NULL && seq->passageLexical!=-1 && k=='|') {
            seq->droit=LitUneSequence(fichier,seq->Fx,neg,1,regle);
            k=lit(fichier,tampon);
        }

        if (k==',' || k==';') {
            fictif=listeEtatArbre.allouearc(NULL,NULL,k);
            if (sens==-1)
                liste.insere(0,fictif);
            else
                liste.ajoute(fictif);
        }

        if (k==fin)
            break;             
		k=lit(fichier,tampon);        
    }    
    fictif=listeEtatArbre.allouearc(NULL,NULL,fin);
    liste.ajoute(fictif);
}
                                            

EtatArbre* GlobalParseur::ConstruitAutomate(VECTA<ArcArbre*>& liste,
                                            int& iliste,
                                            SeqFonc* fonc,
                                            char negation,
                                            typeRegle regle,
                                            char fin,
                                            int debutetat,
                                            int debutarc,
                                            EtatArbre** queue) {
    
    EtatArbre* cle=NULL;
    EtatArbre* debutcle=NULL;
    EtatArbre* lesuivant=NULL;
    ArcArbre* element=NULL;
    ArcArbre* fictif=NULL;    
    EtatArbre* etatfictif=NULL;
    char disjonction=NON;
    char neg=NON;
    char ajoutelement=OUI;
    EtatArbre* etbis;

    Sequence* seq=NULL;
    Fonction* fFonc=NULL;
    
    if (fonc->type()==REGLES_SEQUENCE)
        seq=(Sequence*)fonc;
    else
        fFonc=(Fonction*)fonc;
    
    
    char utilise=-1;
    EtatArbre* finqueue=NULL;                
    EtatArbre* deb=NULL;
    int iarc;

    lesuivant=listeEtatArbre.alloue();
    
    int nbfils;    
    cle=listeEtatArbre.alloue();
    debutcle=cle;
    *queue=lesuivant;
    char debut=OUI;    
    char determ=fonc->deterministe;
    while (iliste<liste.dernier) {
        char avaltout=NON;
                
        char optionnel=NON;
        char modification=NON;
        ajoutelement=OUI;
        neg=negation;
        deb=NULL;
        element=liste[iliste];
        if (element->X==NULL) {            
            if (element->argument=='~') {
                neg=1-negation;
                element=liste[iliste++];
            }
        }
        else {
            element->etatarbre=lesuivant;
            utilise=OUI;
            iliste++;
        }

        if (liste[iliste]->X==NULL) {
            if (liste[iliste]->argument=='*' || 
                liste[iliste]->argument=='+' || 
                liste[iliste]->argument=='-') {
                optionnel=(char)liste[iliste]->argument;
                iliste++;
            }
            
            if (liste[iliste]->argument=='{') {
                finqueue=NULL;                         
                fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);
                fictif->etatarbre=lesuivant;
                etatfictif=listeEtatArbre.alloue();
                etatfictif->ajoutearc(this,fictif,XCatEpsilon,fonc->deterministe);
                element->etatarbre=etatfictif;
                //On rajoute un niveau dans l'arbre pour l'etat inferieur
                iliste++;
                etatfictif->fils=ConstruitAutomate(liste,iliste,fonc,negation,regle,'}',debutetat,debutarc,&finqueue);   
                etatfictif->ConstruitClef(XCatEpsilon,nbfils);
                iliste++;
            }
            else {
                if (liste[iliste]->argument=='(' || 
                    liste[iliste]->argument=='[' ||
                    liste[iliste]->argument=='<') { 
                    if (neg==OUI)
                        erreur(CHAINES[282]);

                    char caracterefinal=')';
                    debut=NON;
                    if (liste[iliste]->argument=='[')
                        caracterefinal=']';
                    else
                        if (liste[iliste]->argument=='<')
                            caracterefinal='>';
                    
                    iliste++;

                    deb=ConstruitAutomate(liste,iliste,fonc,negation,regle,caracterefinal,debutetat,debutarc,&finqueue);
                    
                    VECTA<ArcArbre*> lesderniers;
                    iliste++;
                    EtatArbre* courant=cle;
                    //Dans le cas d'un ?-, on est alors en analyse longue
                    if (liste[iliste]->argument=='-' || caracterefinal=='>') {
                        determ=longue;
                        avaltout=OUI;
                    }
                    else
                        determ=fonc->deterministe;
                    
                    //Si on lit des parentheses imbriquees, on rajoute un niveau
                    if (fin==')' || fin==']') {
                        fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);
                        fictif->etatarbre=deb;
                        cle->ajoutearc(this,fictif,XCatEpsilon,determ);
                        finqueue->etatfin=0;
                        //Puis on cree un epsilon vers l'etat suivant
                        if (avaltout==NON) {
                            fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);
                            fictif->etatarbre=lesuivant;                                            
                            finqueue->ajoutearc(this,fictif,XCatEpsilon,determ);
                            finqueue=lesuivant;
                        }
                        else {
                            fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);                                            
                            etbis=listeEtatArbre.alloue();
                            etbis->etatfin=2;
                            fictif->etatarbre=etbis;
                            finqueue->ajoutearc(this,fictif,XCatEpsilon,determ);                        
                        }
                        courant=deb;                           
                    }
                    else {
                        //Tout d'abord finqueue devient l'etat final pour tous les elements de cle qui branchait
                        //vers le suivant
                        if (avaltout==NON) {
                            cle->arcvers(lesderniers,lesuivant);          
                            listeEtatArbre.demarquage(debutetat,debutarc);
                        }
                        
                        //On cree un niveau supplementaire pour l'integrer dans l'automate
                        fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);
                        fictif->etatarbre=deb;
                        cle->ajoutearc(this,fictif,XCatEpsilon,determ);                                        
                        finqueue->etatfin=0;                      
                        if (avaltout==NON) {
                            listeEtatArbre.detruit(lesuivant);
                            lesuivant=finqueue;
                        }
                        else {
                            fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);                                            
                            etbis=listeEtatArbre.alloue();
                            etbis->etatfin=2;
                            fictif->etatarbre=etbis;
                            finqueue->ajoutearc(this,fictif,XCatEpsilon,determ);                        
                        }                        
                        courant=cle;
                    }    
                    switch (liste[iliste]->argument) {
                    case '-':
                    case '*':
                        if (avaltout==NON) {
                            //Tout d'abord on cree un nouveau noeud final
                            fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);    
                            lesuivant=listeEtatArbre.alloue();
                            //vers lequel on branche notre dernier noeud bouclant
                            fictif->etatarbre=lesuivant;
                            finqueue->ajoutearc(this,fictif,XCatEpsilon,fonc->deterministe);
                            //Puis on cree un chemin vide vers ce nouveau etat final
                            fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);    
                            fictif->etatarbre=lesuivant;
                            fictif->type=2;
                            courant->ajoutearc(this,fictif,XCatEpsilon,fonc->deterministe);   
                        }
						//On boucle alors vers le debut
                        fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);    
                        fictif->etatarbre=deb;
                        fictif->type=1;
                        finqueue->ajoutearc(this,fictif,XCatEpsilon,determ);
                        iliste++;
                        break;
                    case '+':
						//Tout d'abord on cree un nouveau noeud final
						fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);    
						lesuivant=listeEtatArbre.alloue();
						//vers lequel on branche notre dernier noeud bouclant
						fictif->etatarbre=lesuivant;
						finqueue->ajoutearc(this,fictif,XCatEpsilon,fonc->deterministe);
                        //On boucle alors vers le debut
                        fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);    
                        fictif->etatarbre=deb;
                        fictif->type=1;
                        finqueue->ajoutearc(this,fictif,XCatEpsilon,determ);
                        iliste++;
                        break;
                    default:
                        if (caracterefinal==')') {
                            //Tout d'abord on cree un nouveau noeud final
                            fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);    
                            lesuivant=listeEtatArbre.alloue();
                            //vers lequel on branche notre dernier noeud bouclant
                            fictif->etatarbre=lesuivant;
                            finqueue->ajoutearc(this,fictif,XCatEpsilon,fonc->deterministe);
                            //Puis on cree un chemin vide vers ce nouveau etat final
                            fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);    
                            fictif->etatarbre=lesuivant;
                            fictif->type=2;
                            courant->ajoutearc(this,fictif,XCatEpsilon,fonc->deterministe);                            
                        }
                    }
                    
                    //On rebranche tous les noeuds vers le dernier element
                    for (iarc=0;iarc<lesderniers.dernier;iarc++)                    
                        lesderniers[iarc]->etatarbre=lesuivant;
                    
                    *queue=lesuivant;
                    //Puis on lit l'etat final avec l'etat suivant                
                    optionnel=0;
                    ajoutelement=NON;
                    utilise=OUI;
                }
            }
        }
        if (element==NULL)
            ajoutelement=NON;
        
        debut=NON;
        char disjonctionPrecedente=disjonction;
        if (liste[iliste]->argument!=',' && liste[iliste]->argument!=';' && liste[iliste]->argument!=fin)
            erreur(CHAINES[280]);
        
        char k=0;
        if (liste[iliste]->argument==',' || liste[iliste]->argument==';') {
            k=liste[iliste]->argument;            
            if (k==';')
                disjonction=OUI;
            else
                disjonction=NON;
        }
        
        if (ajoutelement==OUI) {
            if (optionnel) {                    
                if (optionnel=='-') {
                    determ=longue;
                    avaltout=OUI;
                }
                else
                    determ=fonc->deterministe;
                
                element->type=1;
                //Tout d'abord on cree un nouveau noeud final
                fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);    
                etatfictif=listeEtatArbre.alloue();
                //vers lequel on branche notre dernier noeud bouclant
                fictif->etatarbre=etatfictif;
                
                if (disjonctionPrecedente==NON) {
                    lesuivant->ajoutearc(this,fictif,XCatEpsilon,determ);                                    
                    if (avaltout==OUI)
                        etatfictif->etatfin=2;
                    lesuivant=etatfictif;                
                    *queue=lesuivant;                                    
                }
                else {
                    element->etatarbre=etatfictif;   
                    fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);                                            
                    if (avaltout==OUI) {           
                        etbis=listeEtatArbre.alloue();
                        etbis->etatfin=2;
                        fictif->etatarbre=etbis;
                        etatfictif->ajoutearc(this,fictif,XCatEpsilon,determ);
                        etatfictif=etbis;
                    }
                    else {                        
                        fictif->etatarbre=lesuivant;                     
                        etatfictif->ajoutearc(this,fictif,XCatEpsilon,determ);
                    }
                }
                
                element->etatarbre->ajoutearc(this,element,XCatEpsilon,determ);                
                if (optionnel!='+') {
                    //Puis on cree un chemin vide vers ce nouveau etat final
                    fictif = listeEtatArbre.allouearc(XCatEpsilon,NULL,-1);    
                    fictif->type=2;
                    fictif->etatarbre=etatfictif;                    
                    cle->ajoutearc(this,fictif,XCatEpsilon,fonc->deterministe);                                        
                }
            }
            cle->ajoutearc(this,element,XCatEpsilon,determ);    
        } 
        
        if (disjonction==NON) {
            //dans le cas d'une boucle
            
            //dans le cas d'une disjonction, on reste sur l'element courant                                
            //On fusionne alors le noeud courant
            cle->FusionneArc(this);
            cle=lesuivant;    
            if (k!=fin) {
                lesuivant=listeEtatArbre.alloue();                
                utilise=NON;
            }
        }
        
        if (liste[iliste]->argument==fin)
            break;
        *queue=lesuivant;
        iliste++;        
    }
    
    if (utilise==NON)
        listeEtatArbre.detruit(lesuivant);
    
    if (utilise==-1)            
        debutcle->etatfin=OUI;
    
    if ((*queue)->etatfin==0)
        (*queue)->etatfin=OUI;

    debutcle->ConstruitClefSuivant(this,XCatEpsilon);
    listeEtatArbre.demarquage(debutetat,debutarc);
    return debutcle;
}

EtatArbre* GlobalParseur::ConstructionAutomate(istream& fichier,
                                               SeqFonc* fonc,                                            
                                               char negation,
                                               char fin,
                                               char optio,
                                               int increment,                                         
                                               typeRegle regle,
                                               EtatArbre** queue) {

    int debutetat=listeEtatArbre.etats.dernier;
    int debutarc=listeEtatArbre.arcs.dernier;
    EtatArbre* cle=NULL;
    VECTA<ArcArbre*> liste;
    LireAutomate(fichier,fin,fonc,regle,liste,increment);  
    int iliste=0;
    
	fonc->aplat=0;
	
	if (liste.dernier>1) {		
		if (regle==REGLES_DEDUCTION) {
			for (int il=0;il<liste.dernier;il++) {
				if (liste[il]->X!=NULL && liste[il]->negation==NON && liste[il]->X!=XCatVide) {
					if (liste[il+1]==NULL || liste[il+1]->argument!='*')
						fonc->aplat|=liste[il]->X->Val;
				}
				else {
					if (liste[il]->X==NULL) {
						if (liste[il]->argument==';' || liste[il]->argument=='(') {
							fonc->aplat=0;
							break;
						}
					}
				}
			}
		}
		cle=ConstruitAutomate(liste,iliste,fonc,NON,regle,fin,debutetat,debutarc,queue);
	}
    if (cle!=NULL) {


        listeEtatArbre.retireEpsilon(cle,XCatEpsilon,debutetat,debutarc,fonc->deterministe);
        
        cle->typer(XCatEpsilon);   
        listeEtatArbre.demarquage(debutetat,debutarc);

        fonc->clef=0; 
        fonc->nombre=cle->CheminCourt(fonc->clef,0,XCatEpsilon);        

        listeEtatArbre.demarquage(debutetat,debutarc);

        if (afficheautomate==OUI) {
            ostream* os=&cout;
            if (OS!=NULL)
                os=OS;
            
            *os<<Endl;
			if (fonc->negation==OUI)
				*os<<"~";
            cle->print(*os,0);
            listeEtatArbre.demarquage(debutetat,debutarc);
        }
               

        listeEtatArbre.destruction(debutetat,debutarc);
    }
    return cle;
}




