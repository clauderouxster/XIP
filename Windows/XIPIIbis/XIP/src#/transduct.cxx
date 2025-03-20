/* $Id: tok-aux.c,v 1.47 2004/02/03 01:18:43 lk Exp $ */

/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.63
   filename   : transduct.cxx
   Date       : 
   Purpose    : TRANSDUCTION XIPTRANS IMPLEMENTATION
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"
#include "ntmhmm.h"
#include "transduct.h"

void compileautomaton(string nom,string sauve,int taille,char);
int retournelabel(string c);

extern "C" {
	void NormalisationNomFichier(char* nomFichier,char* tampon);
}

const int tablequi[]= {
	8211,'-', 8212,'-', 8216,'`',8217,'\'',
	8218,'\'',8220,'"',8221,'"',8222,'"',8226,'.',8230,'.',
	8240,'%',8242,'\'',8243,'\'',8249,'<',8250,'>',8254,'-',8260,'/',8364,128,9001,'<',9002,'>',-1};

void carac_equivalent(int code,uchar& c) {
    if (code>=8211 && code<=9002) {
        for (int kl=0;tablequi[kl]!=-1;kl+=2) {
            if (code==tablequi[kl])
                c=tablequi[kl+1];
        }         
    }
}


extern ostream* flot_erreur;
extern char *VERSION_XIP;
void Trim(char* chaine);
extern GlobalParseur* parseur_courant;
void MettreEnMinuscule(unsigned char* iterateur);

static char* refspaces[256];
static char* refseparators[256];
static char* refcaractereconversion[256];

void blanc(int indent);

static void initreferences() {
	static char init=0;
	if (init==1)
		return;
	init=1;
	int i;

	for (i=0;i<256;i++) {
		refcaractereconversion[i]=NULL;
		refspaces[i]=NULL;
		refseparators[i]=NULL;
	}	

	refspaces[9]="\t";
	refspaces[13]="\r";
	refspaces[32]=" ";
	refspaces[160]=" ";

	refseparators[9]="\t";
	refseparators[13]="\r";
	refseparators[32]=" ";
	refseparators[160]=" ";

	refseparators[44]=",";
	refseparators[59]=";";
	refseparators[46]=".";
	refseparators[58]=":";
	refseparators[33]="!";
	refseparators[63]="?";
	refseparators[45]="-";
	refseparators[95]="_";
	refseparators[34]="\"";
	refseparators[39]="'";
	refseparators[96]="`";
	refseparators[40]="(";
	refseparators[41]=")";
	refseparators[91]="[";
	refseparators[93]="]";
	refseparators[123]="{";
	refseparators[125]="}";
	refseparators[94]="^";
	refseparators[61]="=";
	refseparators[47]="/";
	refseparators[92]="\\";
	refseparators[124]="|";
	refseparators[42]="*";
	refseparators[43]="+";
	refseparators[37]="%";
	refseparators[36]="$";
	refseparators[163]="£";
	refseparators[35]="#";
	refseparators[60]="<";
	refseparators[62]=">";
	refseparators[126]="~";
	refseparators[38]="&";
	refseparators[32]=" ";
	refseparators[171]="«";
	refseparators[187]="»";
	refseparators[176]="°";
	refseparators[10]="\n";


	refcaractereconversion[65]="aàáâãäå";
	refcaractereconversion[66]="b";
	refcaractereconversion[67]="cç";
	refcaractereconversion[68]="d";
	refcaractereconversion[69]="eèéêë";
	refcaractereconversion[70]="f";
	refcaractereconversion[71]="g";
	refcaractereconversion[72]="h";
	refcaractereconversion[73]="iìíîï";
	refcaractereconversion[74]="j";
	refcaractereconversion[75]="k";
	refcaractereconversion[76]="l";
	refcaractereconversion[77]="m";
	refcaractereconversion[78]="n";
	refcaractereconversion[79]="oòóôõö";
	refcaractereconversion[80]="p";
	refcaractereconversion[81]="q";
	refcaractereconversion[82]="r";
	refcaractereconversion[83]="s";
	refcaractereconversion[84]="t";
	refcaractereconversion[85]="uùúûü";
	refcaractereconversion[86]="v";
	refcaractereconversion[87]="w";
	refcaractereconversion[88]="x";
	refcaractereconversion[89]="y";
	refcaractereconversion[90]="z";
	refcaractereconversion[192]="à";
	refcaractereconversion[193]="á";
	refcaractereconversion[194]="â";
	refcaractereconversion[195]="ã";
	refcaractereconversion[196]="ä";
	refcaractereconversion[197]="å";
	refcaractereconversion[198]="æ";
	refcaractereconversion[199]="ç";
	refcaractereconversion[200]="è";
	refcaractereconversion[201]="é";
	refcaractereconversion[202]="ê";
	refcaractereconversion[203]="ë";
	refcaractereconversion[204]="ì";
	refcaractereconversion[205]="í";
	refcaractereconversion[206]="î";
	refcaractereconversion[207]="ï";
	refcaractereconversion[208]="ð";
	refcaractereconversion[209]="ñ";
	refcaractereconversion[210]="ò";
	refcaractereconversion[211]="ó";
	refcaractereconversion[212]="ô";
	refcaractereconversion[213]="õ";
	refcaractereconversion[214]="ö";
	refcaractereconversion[215]="÷";
	refcaractereconversion[216]="ø";
	refcaractereconversion[217]="ù";
	refcaractereconversion[218]="ú";
	refcaractereconversion[219]="û";
	refcaractereconversion[220]="ü";
	refcaractereconversion[221]="ý";
	refcaractereconversion[222]="þ";
}

#define cfinmot 1
#define cfinres 2
#define epsilon 4
#define cANY 5

char* returnstring(char* s,arc* a) {

	switch(a->action) {
	case AUTOMATONANY:
		return "";
	case AUTOMATONDIGIT:      
		return "0123456789";
	case AUTOMATONPUNCT:
		return "<>{}[](),;:.&|!/\\=~#@^?+-*$%\"'_¬£€`";
	case AUTOMATONLETTER:
		return "abcdefghijklmnopqrstuvwxyz";
	case AUTOMATONLETTERUPPER:
		return "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	case AUTOMATONLABEL:
		s[0]=a->label;
		s[1]=0;
		return s;
	}
	return NULL;
}


char arctrans::element(GlobalParseurTrans* parseur,word_offset* w,char c,string& traitsencours,vector<string>& terminaisons) {
	if (id!=-1) {
		if ((l+id)>terminaisons.size())
			return NON;

		if (terminaisons[l+id]=="&")
			return OUI;

		w->concatene(traitsencours);
		w->concatene("+");
		w->concatene(terminaisons[l+id]);
		traitsencours="";
		return OUI;
	}
	else {
		if (u==cANY && l==63) //63=?
			w->ajoute(c,1,1);
		else
			if (l>=32)
				w->ajoute((char)l,1,1);
	}
	return OUI;
}


arctrans* GlobalParseurTrans::unarc(int i,char choix) {
	if (choix==NON)
		return arcs[i];
	return arcsguessers[i];
}

char GlobalParseurTrans::testseparator(int cd) {
	uchar c=alphabet[cd];

	if (c==255)
		return OUI;
	char* sep=separators[cd];
	if (sep==NULL)
		return NON;
	if (c<128)
		return OUI;
	int l=strlen(sep);
	int i;
	if (l==1)
		return OUI;
	char cc[]={0,0,0,0,0,0,0,0,0,0};
	i=0;
	while (l!=0) {
		c=lecteurcourant->lit();
		cc[i++]=alphabet[c];
		l--;
	}
	if (!strcmp(cc,sep))
		return OUI;
	while (i!=0)
		lecteurcourant->metattente(cc[--i]);
	return NON;
}

char GlobalParseurTrans::normalise(char c, uchar* loop) {
	char* normalize=caractereconversion[(uchar)c];
	loop[0]=c;
	if (normalize==NULL) {
		loop[1]=0;
		return 1;
	}
	strcpy((char*)loop+1,normalize);
	return strlen((char*)loop);
}

char arctrans::test(unsigned char c,bool utf8) {
	if (c==u)
		return OUI;
	return NON;
}

char arctransarc::test(unsigned char c,bool utf8) {
	return a->test(c);
}

parcoursarc* GlobalParseurTrans::fournirparcoursarc(arctrans* a,char p) {
	parcoursarc* lv=NULL;

	do {
		if (TasDeparcoursarc.dernier>= TasDeparcoursarc.taille) {
			TasDeparcoursarc.ajuste(TasDeparcoursarc.taille*2);	
			TasDeparcoursarc.dernier++;
			lv=NULL;
			break;
		}
		TasDeparcoursarc.dernier++;
		lv=TasDeparcoursarc.fin();
	}
	while (lv!=NULL && lv->reutiliser==OUI);

	if (lv==NULL) {
		lv=new parcoursarc(a,p);
		TasDeparcoursarc.dernier--;
		lv->position=TasDeparcoursarc.dernier;
		TasDeparcoursarc.ajoute(lv);
		lv->reutiliser=OUI;
		return lv;
	}
	
	lv->init(a,p);
	lv->reutiliser=OUI;
	return lv;
}

void GlobalParseurTrans::nettoyerparcoursarc() {
	for (int i=0;i<TasDeparcoursarc.dernier;i++)
		TasDeparcoursarc[i]->raz();

	TasDeparcoursarc.dernier=0;
}

//------------------------------------------------------------------------------
//									LOOK UP
//------------------------------------------------------------------------------

char GlobalParseurTrans::lookup() {

	if (!lectures.dernier)
		return NON;
    if (MaxSentence!=-1 && num_phrase>MaxSentence)
        return OUI;

	//Nous disposons donc de notre structure de lecture...
    borneGauche=lectures[0]->gauche;
    borneDroite=lectures[0]->droite;
    borneGaucheCar=lectures[0]->gauchecar;
    borneDroiteCar=lectures[0]->droitecar;

    ostream* os=&cout;
    ListeVoc* voc=NULL;

    if (OS!=NULL)
        os=OS;

	string ch;
	char chmin[10];
    char testFinDePhrase=NON;
    char testBalise=NON;
    
    char retourchariotlocal=NON;    
    
    char ajouteFinLigne=NON;
    //chaque lecture correspond a un mot
    if (gAfficherResultats==OUI &&
        TestOptions(AFFICHAGE_OFFSET) && 
        appliqueGrammaire==NON && xipresult==NULL)
        *os<<"["<<borneGauche<<"-"<<borneDroite<<"]"<<Endl;


    for (int i=0;i<lectures.dernier;i++) {
        char* s= lectures[i]->mot;     

        //cas particulier, on rencontre juste un retour chariot
        if (s[0]==10 && s[2]==10)
            retourchariotlocal=OUI;

		//On suppose que nos lexiques internes sont en ISO-Latin
		//En UTF8, le resultat est aussi en UTF8, donc on a besoin d'une conversion
		//pour pouvoir interroger nos lexiques internes.

        uchar* pt=(uchar*)s;
        char chgt=NON;

        int nbcr=0;
        while ( *pt!=0) {            
            if (*pt<=31 && *pt!=9) {
                if (*pt==10)
                    nbcr++;
                *pt=' ';
                chgt=OUI;
            }
            ++pt;
        }
                
        char* ptt=NULL;
		if (retourchariotlocal==NON && retourchariot) {
			if (retourchariot<=9)
				sprintf(chmin,"+CR%d",retourchariot);
			else
				strcpy(chmin,"+CRxN");

			ch=chmin;
			ptt=strrchr(s,9);
            if (ptt!=NULL) {
				ch+=ptt+1;
				ptt[1]=0;
				lectures[i]->longueur=ptt-s+1;
				s=lectures[i]->concatene(STR(ch));
            }
			else
				s=lectures[i]->concatene(STR(ch));
		}            

        //Dans le cas d'un retour chariot detecte, on regarde si l'on a juste un retour chariot (associe a +?)
        if (retourchariotlocal==OUI) {
			ptt=strrchr(s,9);
			if (ptt!=NULL && ptt[1]=='+' && ptt[2]=='?') {
				retourchariot++;
                return NON;
			}
            //sinon, on desactive le retourchariot
            retourchariot=NON;
        }

		//Dans le cas d'un passage par le HMM, il faut garder la trace des offsets...
        if (wbf!=NULL) {            
			if (borneGauche==borneGaucheCar && borneDroite==borneDroiteCar)
				sprintf(chmin,"+%d+%d",borneGauche,borneDroite);
			else
				sprintf(chmin,"+%d+%d+%d+%d",borneGauche,borneDroite,borneGaucheCar,borneDroiteCar);
	        //On cherche les traits:
			ch=chmin;
            ptt=strrchr(s,9);
            if (ptt!=NULL) {
				ch+=ptt+1;
				ptt[1]=0;
				lectures[i]->longueur=ptt-s+1;
				s=lectures[i]->concatene(STR(ch));
            }
        }
        
                
        if (appliqueGrammaire==NON && xipresult==NULL) {
            ajouteFinLigne=OUI;
			num_mot++;
			nombre_mots++;
            if (gAfficherResultats==OUI)
                *os<<ConversionChaine(this,s)<<Endl;
        }
        else {
            if (wbf==NULL) {
                if (testBalise==NON && TestBalise(s)==OUI)
                    return NON;

                if (testBalise==NON)
                    testFinDePhrase=EndOfSentence(s,NON);
                
                testBalise=OUI;
                char* copieChaine;
                LexiconEntryParsing(s,&copieChaine);
                if (voc==NULL) {
                    voc=ConstruitForme(copieChaine,num_mot);
                    if (voc!=NULL) {
                        voc->borneGauche=borneGauche;
                        voc->borneDroite=borneDroite;
                        voc->borneGaucheCar=borneGaucheCar;
                        voc->borneDroiteCar=borneDroiteCar;
#ifdef XIPLIBXML
						voc->xmlnoeud=noeudxmltokencourrant;
#endif
                        num_mot++;
                        nombre_mots++;
                    }
                }
                else
                    FusionneForme(voc,copieChaine);
            }//wbf!=NULL
            else {                
                int tag,num_tokens;

                if (testBalise==NON) {
                    auto_vocabulaire* trad=RechercheBalise(s);
                    pileDeMots.ajoute(trad);
                    if (trad!=NULL)
                        return NON;
                    testBalise=OUI;
                }
                
                analyse_input_line (s, &num_tokens, &tag);                            
                store_to_word_buffer (wbf, s, tag, (! i));
            }
        }
    }
    
    if (wbf==NULL) {
        if (testFinDePhrase==OUI || EndOfSentence(NULL,UTILISE)) {
            Parse(OUI);
            Detruit();
            
            num_phrase++;
            
			if (renumerotation==OUI) {
				num_token=0;
                num_mot=0;
			}
            return OUI;
        }
    }
 
    if (ajouteFinLigne==OUI && gAfficherResultats==OUI)
        *os<<Endl;

    return OUI;
}

//------------------------------------------------------------------------------
//									TEST Left Context
//------------------------------------------------------------------------------

char GlobalParseurTrans::testLC(arctrans* a,lecteur& circ) {
	char retour=OUI;
	string letrait;
	
	int cr=circ.parcours;
	int carcourant=circ.prec();

	if (carcourant==-1) {
		circ.parcours=cr;
		if (a->fin==1)
			return OUI;
		return NON;
	}

	if (a->next!=NULL) {
		retour=NON;
		int t=a->next[0];
		int* n=a->next+1;
		for (int j=0;j<t;j++) {
			arctrans* an=unarc(n[j],NON);
			if (carcourant!=an->u)
				continue;
			if (testLC(an,circ)==OUI)
				retour=OUI;
		}
	}

	circ.parcours=cr;
	if (retour==NON && a->fin==1)
		return OUI;
	
	return retour;
}


#ifdef _DEBUG

void arctrans::affichearc(GlobalParseurTrans* parseur,int i) {
	for (int j=0;j<i;j++)
		cout<<" ";
	if (u<33) {
		if (id!=-1)
			cout<<"#"<<u<<":"<<"+"<<parseur->terminaisons[l+id];
		else
			cout<<"#"<<u<<":"<<(char)l;
	}
	else {
		if (id!=-1)
			cout<<(char)u<<":"<<"+"<<parseur->terminaisons[l+id];
		else
			cout<<(char)u<<":"<<(char)l;
	}
	cout<<"="<<pos<<endl;
}


//Affichage d'un bout de transducteur. Uniquement en mode debug
void arctrans::print(GlobalParseurTrans* parseur,int i,int max) {
	if (i==max)
		return;
	if (passage>=1)
		return;

	passage++;
	affichearc(parseur,i);
	if (next!=NULL) {
		int t=next[0];
		int* n=next+1;
		for (int j=0;j<t;j++) {
			arctrans* an=parseur->arcs[n[j]];
			if (an->pos==91239)
				an->print(parseur,i+1,20);
			else
				an->print(parseur,i+1,max);
		}
	}
	passage--;
}
#endif


char GlobalParseurTrans::parcoursdesarcs(arctrans* a,
										  word_offset* wo,
										  string base,
										  string& traitencours,
										  char choixarc) {
	char retour=OUI;
	word_offset* wb=wo;
	string letrait;
	if (a->next!=NULL) {
		retour=NON;
		int t=a->next[0];
		int* n=a->next+1;
		for (int j=0;j<t;j++) {
			letrait=traitencours;
			arctrans* an=unarc(n[j],choixarc);

			if (an->id!=-1 && an->u>epsilon)
				continue;

			if (j) {
				wb=fournirWordOffset();
				wb->concatene(base);
			}

			if (an->element(this,wb,32,letrait,terminaisons)==NON) {
				libereWord(wb);
				continue;
			}
			
			if (parcoursdesarcs(an,wb,wb->mot,letrait,choixarc)==OUI)
				retour=OUI;
			else
				libereWord(wb);
		}
	}

	if (retour==NON && a->fin==1)
		return OUI;
	
	return retour;
}



char GlobalParseurTrans::affichearc(VECTA<parcoursarc*>& liste,
									lecteur& circ,
									char* surface,
									word_offset* wo,
									VECTA<int>& ids,
									char choixarc) {
	int i,pos;

	arctrans* a=liste[0]->a;
	//Premier cas, une expression reguliere integree
	if (a->type()==1) {
		char rc;
		pos=a->surface((uchar*)surface);
		if (pos!=-1) {
			rc=surface[pos];
			surface[pos]=0;
		}
		for (i=0;i<liste.dernier;i++) {
			liste[i]->marque=OUI;
			a=liste[i]->a;
			if (a->id!=-1) {
				if (ids.cherche(a->id)==-1) {
					ids.ajoute(a->id);
					wo->concatene(surface);
					wo->concatene("\t");
					wo->concatene(terminaisons[a->id]);
					if (pos!=-1)
						surface[pos]=rc;
					return OUI;
				}
				break;
			}
		}
		if (pos!=-1)
			surface[pos]=rc;
		return NON;
	}

	char ajouttrait=NON;
	string traitencours="\t";
	pos=0;	
	for (i=0;i<liste.dernier;i++) {
		liste[i]->marque=OUI;
		a=liste[i]->a;
		if (liste[i]->lcOUrc==A_LC) {
			//Ici on test le LC
			//On remonte la chaine en arriere
			circ.init(circ.debut+i-1);
			char rett=testLC(liste[i]->lcrc,circ);		
		}
			

		//cas particulier ou l'on rencontre un epsilon en chemin...
		if (liste[i]->type_epsilon!=0) {
			if (liste[i]->type_epsilon==1)
				wo->ajoute(a->l,1,1);
			else {
				traitencours+="+";
				traitencours+=terminaisons[a->l+a->id];
			}
			continue;
		}

		a->element(this,wo,surface[i],traitencours,terminaisons);
	}

	return parcoursdesarcs(a,wo,wo->mot,traitencours,choixarc);
}

void construitchemin(parcoursarc* pa,VECTA<parcoursarc*>& liste) {
	if (pa==NULL)
		return;
	construitchemin(pa->pere,liste);
	liste.ajoute(pa);
}

char* GlobalParseurTrans::Space(int c) {
	if (c>256)
		return NULL;
	return spaces[c];
}

//------------------------------------------------------------------------------
//									APPLY
//------------------------------------------------------------------------------

void GlobalParseurTrans::Apply() {
#ifdef _DEBUG
	int compteurbouc=0;
#endif

	if (lecteurcourant==NULL)
		return;
	
	lecteurcourant->courant=this;

	retourchariot=NON;
	int i=0,j,c,cc,consomme,nbseps=0;
	VECTA<parcoursarc*> courants(20);
	VECTA<parcoursarc*> suivants(50);
	VECTA<parcoursarc*> chaines(50);
	VECTA<parcoursarc*> terminaux(20);
	VECTA<int> idnodes(10);

	int lataille=taille_mot+(taille_mot>>1);
	char* surface=new char[lataille];
	int* lescodes=new int[lataille];
	char initial=OUI;
	word_offset* wo;

	VECTA<parcoursarc*>* piles=new VECTA<parcoursarc*>[lataille];

	c=lecteurcourant->lit();	
	char boucle=1;
	while (boucle) {
		if (c==-1) {
			//Nous sommes en fin de fichier, petite magouille honteuse
			//pour forcer la lecture correcte du mot
			//On rajoute un espace
			c=32;
			surface[i]=c;
			lescodes[i]=c;
			surface[i+1]=0;
		}
		else {
			if (i<taille_mot) {
				surface[i]=alphabet[c];
				lescodes[i]=c;
				surface[i+1]=0;
				if (!Space(c) || i)
					cherchearcs(courants,c,i,suivants,chaines,initial,*lecteurcourant);
			}
		}
#ifdef _DEBUG
			compteurbouc++;
#endif
		//limite de lecture d'un mot
		if (testseparator(c)==OUI)
			nbseps++;
		if (surface[0]==10 && spaces[10]==NULL) {
			wo=fournirWordOffset();
			wo->gauche=lecteurcourant->Gauche();
			wo->droite=lecteurcourant->Droite();
			wo->gauchecar=lecteurcourant->GaucheCar();
			wo->droitecar=lecteurcourant->DroiteCar();

			wo->concatene("\n\t\n\t+?");
			if (lookup()==OUI) {
				if (wbf!=NULL) {
					exchange_vit_vectors (vtree);   /* prev <--> cur */
					clear_vit_vector_and_lists (vtree->cur, vtree);

					set_current_class_data (wbf, vtree);
					viterbi_next_word (TRUE);
				}    
			}
			nbseps=0;
			nettoieLectures();
			lecteurcourant->raz();
		}
		else {
			//dans le cas ou on ne trouve rien et qu'en plus ce n'est pas un separateur
			//on accumule
			initial=NON;
			int trouve=i+1;
			if (suivants.dernier==0) {
				if (!nbseps) {
					//S'il s'agit d'un caractere seul, il est evidemment impossible que rien de bon n'en sorte
					if (!lecteurcourant->end()) {
						c=lecteurcourant->lit();						
						i++;
						while (!lecteurcourant->end() && testseparator(c)==NON && i<taille_mot) {
							lescodes[i]=c;
							surface[i++]=alphabet[c];
							c=lecteurcourant->lit();							
						}
					}
					surface[i]=0;
					lecteurcourant->metattente(c);					
					
					terminaux.raz();
					if (deviner((uchar*)surface,courants,suivants,chaines,terminaux)) {
						idnodes.raz();
						for (j=0;j<terminaux.dernier;j++) {
							suivants.raz();		
							construitchemin(terminaux[j],suivants); 
							if (suivants[0]->premier==1) {
								consomme=lectures.dernier;
								wo=fournirWordOffset();
								wo->concatene(surface);
								wo->concatene("\t");
								if (affichearc(suivants,*lecteurcourant,surface,wo,idnodes,OUI)==NON)
									libereLectures(consomme);
							}
						}
					}
					if (lectures.dernier==0) {
						wo=fournirWordOffset();
						wo->concatene(surface);
						wo->concatene("\t");
						wo->concatene(surface);
						wo->concatene("\t");
						wo->concatene(GUESSED_TAG);
					}					

					wo->gauche=lecteurcourant->Gauche();
					wo->droite=lecteurcourant->Droite();
					wo->gauchecar=lecteurcourant->GaucheCar();
					wo->droitecar=lecteurcourant->DroiteCar();
				}
				else {	
					int idx=i-1;
					if (testseparator(c)==OUI)
						nbseps--;
					else {
						//dans ce cas, on ne peut pas juste retirer le dernier caractere: xxxc
						//Il y a forcement un separateur quelque part, sinon, on serait en mode au-dessus
						//Il faut revenir jusqu'au dernier separateur lu
						while (idx>=0) {
							if (testseparator(surface[idx])==OUI)
								break;
							idx--;
						}
					}
					
					for (;idx>=0;idx--) {
						uchar ci=surface[idx];
						if (Space(ci)!=NULL) {
							nbseps--;
							continue;
						}

						cc=surface[idx+1];
						surface[idx+1]=0;

						//On recule etape par etape
						//On nettoie les courants, pour les repositionner sur les etats precedents
						courants.affecte(piles[idx]);
						wo=NULL;
						terminaux.raz();
						if (cherchearcs(courants,-1,idx,terminaux,chaines,0,*lecteurcourant)==1) {
							idnodes.raz();
							//On recherche d'abord les ancetres
							for (j=0;j<terminaux.dernier;j++) {
								suivants.raz();		
								construitchemin(terminaux[j],suivants); 
								if (suivants[0]->premier==1) {
									consomme=lectures.dernier;
									wo=fournirWordOffset();
									wo->concatene(surface);
									wo->concatene("\t");
									if (affichearc(suivants,*lecteurcourant,surface,wo,idnodes,NON)==NON)
										libereLectures(consomme);
								}
							}
							if (lectures.dernier!=0) {
								trouve=idx+1;
								surface[trouve]=cc;
								nbseps=0;
								break;
							}
						}
						//Dans le cas, ou nous en sommes reduit a un seul separateur, on le traite tout seul
						if (!nbseps || strlen(surface)==1) {
							terminaux.raz();
							if (deviner((uchar*)surface,courants,suivants,chaines,terminaux)) {
								idnodes.raz();
								for (j=0;j<terminaux.dernier;j++) {
									suivants.raz();		
									construitchemin(terminaux[j],suivants); 
									if (suivants[0]->premier==1) {
										consomme=lectures.dernier;
										wo=fournirWordOffset();
										wo->concatene(surface);
										wo->concatene("\t");
										if (affichearc(suivants,*lecteurcourant,surface,wo,idnodes,OUI)==NON)
											libereLectures(consomme);
									}
								}
							}
							if (lectures.dernier==0) {
								wo=fournirWordOffset();
								wo->concatene(surface);
								wo->concatene("\t");
								wo->concatene(surface);
								wo->concatene("\t");
								wo->concatene(GUESSED_TAG);
							}
							trouve=idx+1;
							surface[trouve]=cc;
							break;
						}
						if (testseparator(lescodes[idx]))
							nbseps--;
						surface[idx+1]=cc;
					}
					//Nous avons donc des caracteres a conserver a partir de idx
					for (idx=i;idx>=trouve;idx--)
						lecteurcourant->metattente((uchar)surface[idx]);
					if (lectures.dernier) {
						lectures[0]->gauche=lecteurcourant->Gauche();
						lectures[0]->droite=lecteurcourant->Droite();
						lectures[0]->gauchecar=lecteurcourant->GaucheCar();
						lectures[0]->droitecar=lecteurcourant->DroiteCar();
					}
				}
				courants.raz();
				suivants.raz();
				chaines.raz();
				nettoyerparcoursarc();
				lecteurcourant->raz();
				i=0;
				initial=OUI;
				if (lookup()==OUI) {
					gBaliseDebut->raz();

					if (wbf!=NULL) {
						exchange_vit_vectors (vtree);   /* prev <--> cur */
						clear_vit_vector_and_lists (vtree->cur, vtree);

						set_current_class_data (wbf, vtree);
						viterbi_next_word (TRUE);
					}    
				}

				if (surface[0]!=13 || spaces[10]!=NULL)
					retourchariot=NON;
				nettoieLectures();
			}
			else {
				piles[i].affecte(suivants);
				courants.affecte(suivants);
				suivants.raz();
				i++;
			}
		}

		if (lecteurcourant->end()) {
			if (i==0)
				break;
			c=-1;
		}
		else {
			c=lecteurcourant->lit();			
			if (c==-1 && lecteurcourant->end()) {
				if (i==0)
					break;
			}
			else {
				if (Space(c)!=NULL) {
					//dans le cas d'un espace, on les consomme
					while (!lecteurcourant->end()) {
						cc=lecteurcourant->lit();
						if (Space(cc)==NULL) {
							lecteurcourant->metattente(cc);
							break;
						}
					}
				}
			}
		}
	}
	delete lecteurcourant;
	delete[] surface;
	delete[] piles;
	lecteurcourant=NULL;
}



int lecteurutf8::lituncar() {
	int c_courant=extrait();
	int c_second,c_troisieme;
	code=0;
	uchar c;

	if (c_courant>=0xC0 && c_courant<=0xCF) {
		//le chiffre que l'on calcule contient les 5 premiers bits de 0xC... soit 5+6 bits
		//et les 6 bits du caractere suivant
		c_second=extrait();
		code=((c_courant&0x1f)<<6) | (c_second&0x3f);
		//c'est un ASCII modeste
		c=code&0xFF;
		longueurs.affecte(code,2);
		courant->alphabet.affecte(code,c);
	}
	else {
		if (c_courant>=0xE0 && c_courant<=0xEF) { //trois octets 4+6+6 bits
			int triple=c_courant&0xF; //4 premiers bits
			c_second=extrait();
			code=c_second&0x3f; //6 premiers bits
			c_troisieme=extrait();
			int val=c_troisieme&0x3f; //6 premiers bits
			code=triple<<12 | (code<<6) | val;
			if (code==65279) {
				//Il s'agit du e dans l'o
				extrait();
				extrait();
				courant->alphabet.affecte(code,'o');
				longueurs.affecte(code,5);
			}
			else {
				//On met un espace a la place de ce caractere inconnu
				c=32;
				carac_equivalent(code,c);
				courant->alphabet.affecte(code,c);
				longueurs.affecte(code,3);
			}
		}
		else {
			c=c_courant;
			longueurs.affecte(c,1);
			code=c;
		}
	}

	longueurcar++;
	droitcar++;
	return code;
}

void GlobalParseurTrans::SentenceAnalysis(char renumerote,char arbre) {
	num_phrase=0;
	wbf=NULL; 
	baseOffset=0;
	baseOffsetCar=0;
	renumerotation=renumerote;

	if (TestOptions(EXECUTE_TOKENIZE)) {
		if (entree_est_chaine==-1)
			return;
		if (entree_est_chaine==OUI)
			AnalyseTokenChaine();
		else
			AnalyseTokenFichier();    
		entree_est_chaine=-1;
	}

	//Dans un mode d'analyse par token, on ne passe par Apply
	if (initialisation_analyse_token==OUI) {
		CloreAnalyse();
		return;
	}


	if (hmmFileName!=NULL && hmmFileName[0]!=0) {
		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = TRUE;

		wbf = alloc_word_buffer (MAX_WORDS, MAX_ALTBF, MAX_LINEBF);
		vtree = alloc_viterbi (TAG.num);
	}

	Apply();

	if (wbf!=NULL && wbf->num)   /* still words in buffer */
	{
		vtree->cur->flags.sent_end = TRUE;
		viterbi_next_word (FALSE);
	}

	FinAnalyse=OUI;
	while (phrase.dernier!=0) {       
		Parse(OUI);
		Detruit();
		num_phrase++;
	}
	FinAnalyse=NON;
	//HMM lu
	if (wbf!=NULL) {
		words_total = wbf->words_total;

		free_word_buffer (wbf);
		free_viterbi (vtree);

		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = FALSE;
	}
}


void GlobalParseurTrans::StringAssociation(unsigned char* chaine) {
	if (TestOptions(TEST_INPUT_UTF8))
		TestUtf8Chaine(chaine);

    donneesATraiter=chaine;
    entree_est_chaine=OUI;
    if (TestOptions(EXECUTE_TOKENIZE))
        return;

	if (TestOptions(ENTREE_EN_MINUSCULE))
		MettreEnMinuscule(donneesATraiter);

	if (TestOptions(ENTREE_UTF8))
		lecteurcourant=new lecteurutf8(chaine);
	else
		lecteurcourant=new lecteur(chaine);
}

FILE* GlobalParseurTrans::FileAssociation(FILE* fichier) {
	fichierDonnees=fichier;
	if (TestOptions(TEST_INPUT_UTF8))
		TestUtf8Fichier(fichier);
    
    entree_est_chaine=NON;
    if (TestOptions(EXECUTE_TOKENIZE))
        return fichierDonnees;

	if (TestOptions(ENTREE_UTF8))
		lecteurcourant=new lecteurfichierutf8(fichier,NULL);
	else
		lecteurcourant=new lecteurfichier(fichier,NULL);
	return fichierDonnees;
}

void GlobalParseurTrans::AddAToken(unsigned char* tok) {

	StartAnalysis();
}

void GlobalParseurTrans::StartAnalysis() {
	if (initialisation_analyse_token==OUI)
		return;

	wbf=NULL; 
	baseOffset=0;
	baseOffsetCar=0;

	if (hmmFileName!=NULL && hmmFileName[0]!=0) {
		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = TRUE;

		wbf = alloc_word_buffer (MAX_WORDS, MAX_ALTBF, MAX_LINEBF);
		vtree = alloc_viterbi (TAG.num);
	}
	initialisation_analyse_token=OUI;
}

void GlobalParseurTrans::CloseAnalysis() {
	if (initialisation_analyse_token==NON)
		return;

	if (wbf!=NULL && wbf->num)   /* still words in buffer */
	{
		vtree->cur->flags.sent_end = TRUE;
		viterbi_next_word (FALSE);
	}

	FinAnalyse=OUI;
	while (phrase.dernier!=0) {       
		Parse(OUI);
		Detruit();
		num_phrase++;
	}

	FinAnalyse=NON;
	//HMM lu
	if (wbf!=NULL) {
		words_total = wbf->words_total;

		free_word_buffer (wbf);
		free_viterbi (vtree);

		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = FALSE;
	}
	initialisation_analyse_token=NON;
}

void GlobalParseurTrans::AnalyseTokenFichier() {
	//Chaque ligne est un token
	static char buff[1000];
	while (!feof(fichierDonnees)){
		fgets(buff,999,fichierDonnees);
		Trim(buff);
		if (buff[0])
			AddAToken((uchar*)buff);
	}
}

void GlobalParseurTrans::AnalyseTokenChaine() {

	//Chaque ligne est un token
	char* debut=(char*)donneesATraiter;
	char* pt=strchr(debut,'\n');


	while (pt!=NULL) {
		char trouve=NON;
		*pt=0;                
		if (pt[-1]=='\r') {
			pt[-1]=0;   
			trouve=OUI;
		}
		if (debut[0])
			AddAToken((uchar*)debut);
		*pt='\n';
		if (trouve==OUI)
			pt[-1]='\r';
		debut=pt+1;
		pt=strchr(debut,'\n');
	}

	if (debut[0])
		AddAToken((uchar*)debut);
}


char GlobalParseurTrans::consommeepsilon(arctrans* a,
										 int index,
										 uchar* loop,
										 VECTA<parcoursarc*>& suivants,
										 VECTA<parcoursarc*>& chaines,
										 parcoursarc* pac,
										 char choixarc) {
	//dans ce cas, il nous faut boucler pour trouver le premier arc
	//compatible
	if (a->next==NULL)
		return 0;
	int tb=a->next[0];
	int* nb=a->next+1;
	char retour=0;
	arctrans* an;
	parcoursarc* pa;
	
	for (int i=0;i<tb;i++) {
		an=unarc(nb[i],choixarc);
		if (an->u==epsilon || an->u==cfinmot) {
			pa=fournirparcoursarc(a);
			if (consommeepsilon(an,nb[i],loop,suivants,chaines,pa,choixarc)==1) {			
				if (a->id==-1)
					pa->type_epsilon=1;
				else
					pa->type_epsilon=2;
				pa->idx=index;
				pac->ajoute(pa);							
				chaines.ajoute(pa);
				retour=1;
			}
			else
				pa->raz();
		}
		else {
			if (an->u==cANY || strchr((char*)loop,an->u)) {
				pa=fournirparcoursarc(a);
				if (a->id==-1)
					pa->type_epsilon=1;
				else
					pa->type_epsilon=2;
				pa->idx=index;
				pac->ajoute(pa);							
				chaines.ajoute(pa);
				parcoursarc* pab=fournirparcoursarc(an);
				pab->idx=nb[i];
				pa->ajoute(pab);
				chaines.ajoute(pab);
				suivants.ajoute(pab);
				retour=1;
			}
		}
	}
	return retour;
}

char GlobalParseurTrans::consommeepsilonmin(arctrans* a,uchar c,char choixarc) {
	//dans ce cas, il nous faut boucler pour trouver le premier arc
	//compatible
	int tb=a->next[0];
	int* nb=a->next+1;
	char retour=0;
	arctrans* an;
	
	for (int i=0;i<tb;i++) {
		an=unarc(nb[i],choixarc);
		if (an->u==epsilon) {
			if (consommeepsilonmin(an,c,choixarc)==1)
				retour=1;
		}
		else {
			if (an->u==cANY || c==an->u)
				retour=1;
		}
	}
	return retour;
}

//Methode principale pour chercher les arcs, les uns apres les autres...
char GlobalParseurTrans::cherchearcs(VECTA<parcoursarc*>& courants,
									 int c,
									 int i,
									 VECTA<parcoursarc*>& suivants,
									 VECTA<parcoursarc*>& chaines,
									 char initial,
									 lecteur& circ) {


	int t,j,k;
	int* n;
	arctrans* a;
	arctrans* an;
	parcoursarc* pac;
	int l=0;
	parcoursarc* pa;
	unsigned char loop[20];
	uchar nb=0;

	vector<arctrans*> varcs;
	VECTA<int>& ia=initiauxnorm[c];		
	nb=0;
	for (t=0;t<ia.dernier;t++) {
		a=arcsnorm[ia[t]];
		if (a->id==-1) {
			varcs.push_back(a);
			//Nous verifions ensuite que les LC et RC ne sont pas au bout
			loop[nb++]=a->l;		
		}
	}
	
	char nettoyage=NON;
	if (varcs.size()==0) {
		nb=normalise(c,loop);
		for (l=0;l<nb;l++) {
			arctrans* aref=new arctrans;
			aref->u=c;
			aref->l=loop[l];
			varcs.push_back(aref);
			nettoyage=OUI;
		}
	}

	char retour=0;
	if (initial==1) {
		for (l=0;l<varcs.size();l++) {
			VECTA<int>& ia=initiaux[varcs[l]->l];		
			for (t=0;t<ia.dernier;t++) {
				a=arcs[ia[t]];
				pac=fournirparcoursarc(a,1);
				suivants.ajoute(pac);	
				chaines.ajoute(pac);
				retour=1;
			}
		}
		return(retour);
	}

	int pos=0;


	//Dans le cas d'un separateur trouve
	if (c==-1) {
		for (j=0;j<courants.dernier;j++) {
			pac=courants[j];
			a=pac->a;
			if (a->next!=NULL) {
				t=a->next[0];
				n=a->next+1;
				for (k=0;k<t;k++) {
					an=arcs[n[k]];
					if (an->u<=epsilon) {
						pa=fournirparcoursarc(an);
						pac->ajoute(pa);
						suivants.ajoute(pa);
						retour=OUI;
					}
				}
			}
		}
		return(retour);	
	}

	VECTA<int> gardes(5);
	
	for (j=0;j<courants.dernier;j++) {
		pac=courants[j];
		a=pac->a;
		//Presence d'un contexte. Dans ce cas, il n'est pas necessaire d'explorer
		//les arcs qui suivent... Il ne s'agit pas non plus d'une erreur...
		if (a->next!=NULL) {
			t=a->next[0];
			n=a->next+1;
			for (int j=0;j<t;j++) {
				arctrans* arcourant=NULL;
				TYPETESTARC lcOUrc=A_RIEN;
				an=arcs[n[j]];
				if (an->type()==0 && gardes.cherche(n[j])!=-1)
					continue;
				char trouve=NON;
				if (an->type()==0) {
					if (an->u==epsilon || an->u==cfinmot) {
						if (consommeepsilon(an,n[j],loop,suivants,chaines,pac,NON)==OUI) {
							retour=OUI;
							gardes.ajoute(n[j]);
						}
					}
					else {
						if (an->u==cANY || strchr((char*)loop,an->u)) {
							trouve=OUI;
							if (nettoyage==NON) {								
								for (l==0;l<varcs.size();l++) {
									if ((arcourant=testNextLCRC(varcs[l],lcOUrc))!=NULL)
										break;
								}
							}
						}
						else
							if (varcs.size()==1 && an->u>c)
								break;
					}
				}
				else {
					for (l=0;l<varcs.size();l++) {
						if (an->test(varcs[l]->l,circ.utf8)) {
							trouve=OUI;
							break;
						}
					}
				}
				if (trouve==OUI) {
					//On rajoute ces liens au noeuds deja present	
					gardes.ajoute(n[j]);
					pa=fournirparcoursarc(an);
					pa->lcrc=arcourant;					
					pa->lcOUrc=lcOUrc;
					chaines.ajoute(pa);
					pa->idx=n[j];
					pac->ajoute(pa);						
					suivants.ajoute(pa);
					retour=1;
				}
			}
		}
	}
	if (nettoyage==OUI) {
		for (l=0;l<varcs.size();l++)
			delete varcs[l];
	}
	return(retour);
}

char GlobalParseurTrans::normaliser(int* mot,unsigned char* res,int i,int r,arctrans* a) {
	int t;
	int* n;
	arctrans* an;
	int l=0;

	uchar c=mot[i];
	char retour=NON;

	if (i==0) {
		VECTA<int>& ia=initiauxnorm[c];		
		for (t=0;t<ia.dernier;t++) {
			res[r]=(char)arcsnorm[ia[t]]->l;
			res[r+1]=0;
			normaliser(mot,res,i+1,strlen((char*)res),arcsnorm[ia[t]]);
		}
	}
	else  {
		int pos=0;
		VECTA<int> gardes(5);
		if (a->next!=NULL) {
			t=a->next[0];
			n=a->next+1;
			for (int j=0;j<t;j++) {
				an=arcsnorm[n[j]];
				if (an->type()==0 && gardes.cherche(n[j])!=-1)
					continue;
				char trouve=NON;
				if (an->type()==0) {
					if (an->u==epsilon) {
						if (consommeepsilonmin(an,c,OUI)==OUI)
							gardes.ajoute(n[j]);
					}
					else {
						if (an->u==cANY || c==an->u)
							trouve=OUI;
						else
							if (an->u>c)
								break;
					}
				}
				else {
					if (an->test(c,lecteurcourant->utf8))
						trouve=OUI;
				}
				if (trouve==OUI) {
					//On rajoute ces liens au noeuds deja present	
					res[r]=(char)an->l;
					res[r+1]=0;
					if (normaliser(mot,res,i+1,r+1,an)==OUI)
						break;
				}
			}
		}
		else
			if (a->u<epsilon)
				retour=OUI;
	}

	return retour;
}

char GlobalParseurTrans::deviner(unsigned char* mot,
								 VECTA<parcoursarc*>& courants,
								 VECTA<parcoursarc*>& suivants,
								 VECTA<parcoursarc*>& chaines,
								 VECTA<parcoursarc*>& finals) {
	//Cette procedure utilise les arcsguessers
	//Elle applique une analyse sur une chaine de caractere

	int t,j;
	int* n;
	arctrans* an;
	parcoursarc* pac;
	int l=0;
	parcoursarc* pa;

	unsigned char loop[20];

	uchar c=mot[0];
	loop[0]=c;
	loop[1]=0;
	uchar nb=1;//normalise(c,loop);

	courants.raz();

	for (l=0;l<nb;l++) {
		VECTA<int>& ia=initiauxguessers[loop[l]];		
		for (t=0;t<ia.dernier;t++) {
			pac=fournirparcoursarc(arcsguessers[ia[t]],1);
			courants.ajoute(pac);	
		}
	}

	if (courants.dernier==0)
		return NON;
	
	VECTA<int> gardes(5);
	int ln=strlen((char*)mot);
	for (size_t i=1;i<ln;i++) {
		int pos=0;
		c=mot[i];
		loop[0]=c;
		nb=1;
		//nb=normalise(c,loop);
		suivants.raz();	
		for (j=0;j<courants.dernier;j++) {
			pac=courants[j];
			arctrans* a=pac->a;
			if (a->next!=NULL) {
				t=a->next[0];
				n=a->next+1;
				for (int j=0;j<t;j++) {
					an=arcsguessers[n[j]];
					if (an->type()==0 && gardes.cherche(n[j])!=-1)
						continue;
					char trouve=NON;
					if (an->type()==0) {
						if (an->u==epsilon || an->u==cfinmot) {
							chaines.raz();
							if (consommeepsilon(an,n[j],loop,suivants,chaines,pac,OUI)==OUI)
								gardes.ajoute(n[j]);
						}
						else {
							if (an->u==cANY || strchr((char*)loop,an->u))
								trouve=OUI;
							else
								if (nb==1 && an->u>c)
									break;
						}
					}
					else {
						for (l=0;l<nb;l++) {
							if (an->test(loop[l],lecteurcourant->utf8)) {
								trouve=OUI;
								break;
							}
						}
					}
					if (trouve==OUI) {
						//On rajoute ces liens au noeuds deja present	
						gardes.ajoute(n[j]);
						pa=fournirparcoursarc(an);
						pa->idx=n[j];
						pac->ajoute(pa);						
						suivants.ajoute(pa);
					}
				}
			}
		}

		if (suivants.dernier==0)
			return NON;
		
		gardes.raz();
		courants.affecte(suivants);
	}

	char retour=NON;
	for (j=0;j<courants.dernier;j++) {
		pac=courants[j];
		arctrans* a=pac->a;
		if (a->next!=NULL) {
			t=a->next[0];
			n=a->next+1;
			for (int k=0;k<t;k++) {
				an=arcsguessers[n[k]];
				if (an->u<=epsilon) {
					pa=fournirparcoursarc(an);
					pac->ajoute(pa);
					finals.ajoute(pa);
					retour=OUI;
				}
			}
		}
	}
	return retour;
}

char GlobalParseurTrans::test(unsigned char* mot,int* lemme,VECTA<arctrans*>& courants,int& dernier) {
	//Cette procedure utilise les arcsguessers
	//Elle applique une analyse sur une chaine de caractere

	int t,j;
	int* n;
	arctrans* an;
	int l=0;

	unsigned char loop[20];	
	VECTA<arctrans*> suivants;



	uchar c=mot[0];
	uchar nb=normalise(c,loop);

	for (l=0;l<nb;l++) {
		VECTA<int>& ia=initiaux[loop[l]];		
		for (t=0;t<ia.dernier;t++)
			courants.ajoute(arcs[ia[t]]);
	}

	if (courants.dernier==0)
		return NON;
	
	int lgm=strlen((char*)mot);
	for (size_t i=1;i<lgm;i++) {
		
		if (!lemme[i]) {
			dernier=i;
			return NON;
		}

		c=mot[i];
		nb=normalise(c,loop);
		suivants.raz();	
		for (j=0;j<courants.dernier;j++) {
			arctrans* a=courants[j];
			if (a->next!=NULL) {
				t=a->next[0];
				n=a->next+1;
				for (int j=0;j<t;j++) {
					an=arcs[n[j]];
					char trouve=NON;
					if (an->type()==0) {
						if (an->u==epsilon || an->u==cANY)
							continue;
						else {
							if (strchr((char*)loop,an->u) && an->l==lemme[i])
								trouve=OUI;
							else
								if (nb==1 && an->u>c)
									break;
						}
					}
					else
						continue;

					if (trouve==OUI)
						suivants.ajoute(an);
				}
			}
		}

		if (suivants.dernier==0) {
			dernier=i;
			return NON;
		}
		
		courants.affecte(suivants);
	}

	for (j=0;j<courants.dernier;j++) {
		arctrans* a=courants[j];
		if (a->next!=NULL) {
			t=a->next[0];
			n=a->next+1;
			for (int k=0;k<t;k++) {
				an=arcs[n[k]];
				if (an->u<=epsilon)
					return OUI;
			}
		}
	}
	dernier=lgm-1;
	return NON;
}

//------------------------------------------------------------------------------
//									COMPILATION
//------------------------------------------------------------------------------


int GlobalParseurTrans::lirecode(ifstream& fichier,unsigned char& c,char type) {
	static char tampon[1000];
	int i=0;
	int mult=1;
	
	if (type==OUI) {
		if (c=='-')
			mult=-1;
		else
			tampon[i++]=c;
	}

	c=fichier.get();
	if (type==NON) {
		if (c=='-') {
			mult=-1;
			c=fichier.get();
		}
	}

	while (c>='0' && c<='9') {
		tampon[i++]=c;
		c=fichier.get();
	}
	tampon[i]=0;
	return atoi(tampon)*mult;
}


void GlobalParseurTrans::liretrans(vector<string>& lexs,VECTA<int>* inits, VECTA<arctrans*>& lesarcs) {
	char lepartage=NON;
	arctrans* a;
	int i;
	for (size_t n=0;n < lexs.size();n++) {
		*flot_erreur<<"Loading:"<<lexs[n].c_str()<<Endl;
		ifstream fichier(lexs[n].c_str(),modeOuverture);
		if (fichier.fail() ) {
			string s="Unknown file: ";
			s+=lexs[n];
			*flot_erreur<<s.c_str()<<Endl;
			erreur((char*)s.c_str());
		}
		//Tout d'abord on lit la taille des terminaisons
		char tampon[1000];
		int refterminaison=terminaisons.size();
		fichier.getline(tampon,1000);
		int thesize=atoi(tampon);
		while (!fichier.eof()) {
			fichier.getline(tampon,1000);
			Trim(tampon);
			if (tampon[0]==0)
				break;
			if (tampon[0]=='+' && tampon[1]!=0)
				ajouteterminaison(tampon+1,NON);
			else
				ajouteterminaison(tampon,NON);
		}
		unsigned char c;	
		i=0;
		int k;
		VECTA<int> nn;
		c=fichier.get();
		//On lit les etats initiaux
		while(!fichier.eof() && c!='e') {
			//On lit maintenant l'automate
			if (c=='i') {
				i=lirecode(fichier,c,NON);
				if (c=='a') {
					k=lirecode(fichier,c,NON);
					inits[i].ajoute(k+lesarcs.dernier);
				}
			}
		}
		thesize=lirecode(fichier,c,NON);
		int affichepoint=thesize/50;
		if (affichepoint==0)
			affichepoint=1;
		lesarcs.ajuste(thesize+lesarcs.dernier);
#ifdef _DEBUG
		int comptedonc=0;
#endif			
		while (!fichier.eof()) {
			if (!strchr("SUVuxtT",(char)c)) {
				string err="Error while reading the TRANS file:";
				erreur((char*)err.c_str());
			}
#ifdef _DEBUG
			comptedonc+=1;
#endif
			//Debut d'un arc
			a=new arctrans;
			char deja=NON;
			char finetat=0;
			char partage=NON;
			if (c=='S') {
				//Nous sommes en train de lire un partage d'ensemble, provenant d'une conversion
				//Ce qui suit reste "standard"
				partage=OUI;
				lepartage=OUI;
				c=fichier.get();
			}

			//ce qui suit est soit un code, soit un l
			switch(c) {
						case 'V':
							finetat=1;
						case 'U':
							c=fichier.get(); 
							a->u=c;
							a->fin=finetat;
							i=lirecode(fichier,c,NON);
							deja=OUI;
							a->l=i;
							if (terminaisons[i+refterminaison].size()>1)
								a->id=refterminaison;
							else {
								if (i==1)
									a->l=1;
								else
									a->l=(uchar)terminaisons[i+refterminaison][0];
							}
							break;
						case 'u':
							c=fichier.get();
							a->u=c;
							c=fichier.get();
							a->l=c;
							break;
						case 'x':
							c=fichier.get();
							a->u=c;
							a->l=c;
							break;
						case 't':
							c=fichier.get();
							a->u=cfinmot;
							a->id=refterminaison;
							a->l=c;
							break;
						case 'T':
							c=fichier.get();
							a->u=c;
							c=fichier.get();
							a->id=refterminaison;
							a->l=c;
							break;
			}
			if (deja==NON)
				c=fichier.get();
			nn.raz();
			int sgn;
			long val;
			int nblect;
			while (strchr("LMNOPlmnop",c)) {
				sgn=1;
				if (c<108) {
					nblect=c-76;
					sgn=-1;
				}
				else {
					nblect=c-108;
					sgn=1;
				}
				val=0;
				while (nblect>0) {
					c=fichier.get();
					val*=255;
					val+=c-1;							
					nblect--;
				}
				val*=sgn;
				nn.ajoute(val);
				c=fichier.get();
			}
			if (nn.dernier!=0) {
				if (partage==OUI) {
					//dans ce cas nos numeros proviennent
					//de l'arc dont l'index est dans nn
					a->copie=nn[0]+lesarcs.dernier;
				}
				else {
					a->next=new int[nn.dernier+1];
					a->next[0]=nn.dernier;
					for (i=0;i<nn.dernier;i++) {
						a->next[i+1]=nn[i]+lesarcs.dernier;
						if (a->next[i+1]>lesarcs.taille)
							cout<<a->next[i+1]<<Endl;
					}
				}
			}
#ifdef _DEBUG
			a->pos=lesarcs.dernier;
#endif
			lesarcs.ajoute(a);
			a=NULL;
			if (lesarcs.dernier%affichepoint==0)
				*flot_erreur<<".";
		}
		*flot_erreur<<Endl;
	}

	if (lepartage==OUI) {
		for (i=0;i<lesarcs.dernier;i++) {
			a=lesarcs[i];
			if (a->copie!=-1) {
				a->next=lesarcs[a->copie]->next;
			}
		}
	}
	*flot_erreur<<Endl;
}

void majnext(arctrans* acourant,long val) {
	if (acourant->next==NULL) {
		acourant->next=new int[2];
		acourant->next[0]=0;
	}
	else {
		int* nn=new int[acourant->next[0]+2];
		memcpy(nn,acourant->next,(1+acourant->next[0])*sizeof(int));
		delete[] acourant->next;
		acourant->next=nn;
	}
	acourant->next[0]++;
	acourant->next[acourant->next[0]]=val;
}


void GlobalParseurTrans::LitUneRg(ifstream& fichier,VECTA<automate*>& rg,char guesser) {
	char traits[1000];
	char mot[1000];
	char lemme[1000];
	char k;
	automate* a;
	automate* alemme=NULL;


	mot[0]=0;
	lemme[0]=0;
	k=lit(fichier,mot);
	if (mot[0]==0)
		return;

	if (k==':') {//lecture du lemme
		k=lit(fichier,lemme);
		alemme=new automate;
		if (alemme->parse((unsigned char*)lemme,NULL)==NULL)
			RenvoieErreurAutomate(alemme->erreur);
		alemme->aupluspres();
		rg.ajoute(alemme);
	}


	if (k!='=')
		erreur(CHAINES[123]);

	//On ne lit qu'une seule chaine
	k=lit(fichier,traits);

	int idt=ajouteterminaison(traits,OUI);
	if (chainecomplexe((unsigned char*)mot,0,ARCINITIAL)==OUI) {
		a=new automate;			
		if (a->parse((unsigned char*)mot,(void*)terminaisons[idt].c_str())==NULL)
			RenvoieErreurAutomate(a->erreur);
		a->aupluspres();
		if (alemme==NULL) {
			if (guesser==OUI)
				maj(a,NULL,arcsguessers,OUI);
			else
				maj(a,NULL,arcs,NON);
		}
		else {
			if (guesser==OUI)
				maj(a,alemme,arcsguessers,OUI);
			else
				maj(a,alemme,arcs,NON);
		}
		rg.ajoute(a);
	}
	else {//c'est un mot simple a integrer
		string serr="To add new words, please use the -intrans -outtrans command on the XIP command line";
		erreur((char*)serr.c_str());
	}
}

void GlobalParseurTrans::lecturecode(char** sep,char* tampon) {
	char* curseur=tampon;
	char* pt=strchr(curseur,' ');
	int i=0;

	char* sp;
	uchar l;

	while (pt!=NULL) {
		*pt=0;
		if (curseur[0]=='\\' && isdigit(curseur[1])) {
			//c'est un code
			l=atoi(curseur+1);
			sp=new char[2];
			sp[0]=alphabet[l];
			sp[1]=0;
			sep[l]=sp;
		}
		else {			
			sp=new char[strlen(curseur)+1];
			strcpy(sp,curseur);
			sep[(uchar)curseur[0]]=sp;
		}
		curseur=pt+1;
		pt=strchr(curseur,' ');
	}
	if (curseur[0]=='\\' && isdigit(curseur[1])) {
		//c'est un code
		l=atoi(curseur+1);
		sp=new char[2];
		sp[0]=l;
		sp[1]=0;
		sep[l]=sp;
	}
	else {			
		sp=new char[strlen(curseur)+1];
		strcpy(sp,curseur);
		sep[(uchar)curseur[0]]=sp;
	}
}

void GlobalParseurTrans::lecturespace(char* tampon) {
	for (int i=0;i<256;i++)
		spaces[i]=NULL;

	lecturecode(spaces,tampon);
	nettoiespace=1;
}

void GlobalParseurTrans::lecturesep(char* tampon) {
	int i;
	for (i=0;i<256;i++) {
		if (spaces[i]!=NULL)
			separators.affecte(i,DupliqueChaine(spaces[i]));
		else
			separators.affecte(i,NULL);
	}

	char* separs[256];
	for (i=0;i<256;i++)
		separs[i]=NULL;
	lecturecode(separs,tampon);
	for (i=0;i<256;i++) {
		if (separs[i]!=NULL)
			separators.affecte(i,separs[i]);
	}
	
	nettoiesep=1;
}

void GlobalParseurTrans::rechargementsarcs() {
	int i;

	//On nettoie tout d'abord
	for (i=0;i<arcs.dernier;i++)
		delete arcs[i];
	arcs.raz();
	for (i=0;i<arcsguessers.dernier;i++)
		delete arcsguessers[i];
	arcsguessers.raz();
	rgx.nettoie();
	rgxguess.nettoie();
	TasDeparcoursarc.nettoie();
	for (i=0;i<256;i++) {
		initiaux[i].raz();
		initiauxnorm[i].raz();
		initiauxguessers[i].raz();
	}
	if (nettoiespace==OUI) {
		for (i=0;i<256;i++) 
			if (spaces[i]!=NULL)
				delete[] spaces[i];
	}
	if (nettoiesep==OUI) {
		for (i=0;i<256;i++) 
			if (separators[i]!=NULL)
				delete[] separators[i];
	}
	if (nettoieconv==OUI) {
		for (i=0;i<256;i++) 
			if (caractereconversion[i]!=NULL)
				libere(caractereconversion[i]);
	}
	terminaisons.clear();
	lexiques.clear();
	normaliseurs.clear();
	guessers.clear();
	auvol.clear();
	compiler.clear();
	mterminaison.clear();
	remplacements.clear();
	fichiersvoc.clear();
	guessersvoc.clear();
	chargementlexiques();
	dynamiques.clear();
}

void GlobalParseurTrans::lectureconv(ifstream& entree,char* tampon) {
	char res[100];
	char k;
	for (int i=0;i<256;i++)
		caractereconversion[i]=NULL;

	while (!entree.eof()) {
		k=lit(entree,tampon);
		if (tampon[0]==0)
			break;
		if (k==':') {
			strcat(tampon,":");
			break;
		}
		if (k!='=')
			erreur(CHAINES[43]);
		k=lit(entree,res);			
		if (k!='.')
			erreur("Missing '.'");
		caractereconversion[(uchar)tampon[0]]=DupliqueChaine(res);
	}
	nettoieconv=1;
}

void GlobalParseurTrans::lectureremplace(ifstream& entree,char* tampon) {
	char res[100];
	char k;

	while (!entree.eof()) {
		k=lit(entree,tampon);
		if (tampon[0]==0)
			break;
		if (k==':') {
			strcat(tampon,":");
			break;
		}

		if (k!='=')
			erreur(CHAINES[43]);
		k=lit(entree,res);			
		if (k!='.')
			erreur("Missing '.'");
		int l=strlen(tampon);
		uchar ic=(uchar)tampon[0];
		if (Cremplacements[ic]<l)
			Cremplacements[ic]=l;
		remplacements[tampon]=res;
	}
}

void GlobalParseurTrans::chargementlexiques() {
	initreferences();
	size_t i;
	for (i=0;i<256;i++) {
		spaces[i]=refspaces[i];
		separators.affecte(i,refseparators[i]);
		caractereconversion[i]=refcaractereconversion[i];
		Cremplacements[i]=0;
	}
	nettoiespace=NON;
	nettoiesep=NON;
	nettoieconv=NON;

	ifstream entree(nom,modeOuverture);
	char tampon[1000];
	char nomFichier[1000];
	tampon[0]=0;
	NOM=nom;
    ostream* os=&cout;
    if (OS!=NULL)
        os=OS;
	if (entree.fail() ) {
		string s="Unknown file: ";
		s+=nom;
		erreur((char*)s.c_str());
	}

	char relire=OUI;
	char sep=NON;

	while (!entree.eof()) {
		if (relire==OUI) {
			entree.getline(tampon,1000);
			Trim(tampon);
			if (tampon[0]==0 || tampon[0]=='#')
				continue;
		}
		else
			relire=OUI;

		//lecture des lexiques
		if (!strcmp(tampon,"lexicons:")) {
			while (!entree.eof()) {
				entree.getline(tampon,1000);
				Trim(tampon);
				if (tampon[0]==0 || tampon[0]=='#')
					continue;
				if (tampon[strlen(tampon)-1]==':')
					break;
				NormalisationNomFichier(nomFichier,tampon);
				lexiques.push_back(nomFichier);				
			}
			relire=NON;
			continue;
		}

		if (!strcmp(tampon,"guessers:")) {
			while (!entree.eof()) {
				entree.getline(tampon,1000);
				Trim(tampon);
				if (tampon[0]==0 || tampon[0]=='#')
					continue;
				if (tampon[strlen(tampon)-1]==':')
					break;
				NormalisationNomFichier(nomFichier,tampon);
				guessers.push_back(nomFichier);				
			}
			relire=NON;
			continue;
		}

		if (!strcmp(tampon,"guesservoc:")) {
			while (!entree.eof()) {
				entree.getline(tampon,1000);
				Trim(tampon);
				if (tampon[0]==0 || tampon[0]=='#')
					continue;
				if (tampon[strlen(tampon)-1]==':')
					break;
				NormalisationNomFichier(nomFichier,tampon);
				guessersvoc.push_back(nomFichier);				
			}
			relire=NON;
			continue;
		}

		if (!strcmp(tampon,"onthefly:")) {
			while (!entree.eof()) {
				entree.getline(tampon,1000);
				Trim(tampon);
				if (tampon[0]==0 || tampon[0]=='#')
					continue;
				if (tampon[strlen(tampon)-1]==':')
					break;
				NormalisationNomFichier(nomFichier,tampon);
				auvol.push_back(nomFichier);				
			}
			relire=NON;
			continue;
		}

		if (!strcmp(tampon,"compile:")) {
			while (!entree.eof()) {
				entree.getline(tampon,1000);
				Trim(tampon);
				if (tampon[0]==0 || tampon[0]=='#')
					continue;
				if (tampon[strlen(tampon)-1]==':')
					break;
				NormalisationNomFichier(nomFichier,tampon);
				compiler.push_back(nomFichier);				
			}
			relire=NON;
			continue;
		}

		if (!strcmp(tampon,"normalization:")) {
			while (!entree.eof()) {
				entree.getline(tampon,1000);
				Trim(tampon);
				if (tampon[0]==0 || tampon[0]=='#')
					continue;
				if (tampon[strlen(tampon)-1]==':')
					break;
				NormalisationNomFichier(nomFichier,tampon);
				normaliseurs.push_back(nomFichier);
			}
			relire=NON;
			continue;
		}


		if (!strcmp(tampon,"vocabulary:")) {
			while (!entree.eof()) {
				entree.getline(tampon,1000);
				Trim(tampon);
				if (tampon[0]==0 || tampon[0]=='#')
					continue;
				if (tampon[strlen(tampon)-1]==':')
					break;
				NormalisationNomFichier(nomFichier,tampon);
				fichiersvoc.push_back(nomFichier);
			}
			relire=NON;
			continue;
		}

		if (!strcmp(tampon,"guesstag:")) {
			entree.getline(tampon,1000);
			Trim(tampon);
			strcpy(GUESSED_TAG,tampon); 
			continue;
		}


		if (!strcmp(tampon,"spaces:")) {
			if (sep==OUI)
				erreur("The separators should be declared after the spaces");
			if (nettoiespace==OUI)
				erreur("The spaces have already been declared");
			entree.getline(tampon,1000);
			Trim(tampon);
			lecturespace(tampon);
			continue;
		}

		if (!strcmp(tampon,"dynamic:")) {
			//chaque ligne comprend un alias, separe par un "=" suivi d'un chemin
			while (!entree.eof()) {
				entree.getline(tampon,1000);
				Trim(tampon);
				if (tampon[0]==0 || tampon[0]=='#')
					continue;
				if (tampon[strlen(tampon)-1]==':')
					break;
				char* pt=strchr(tampon,'=');
				if (pt==NULL)
					erreur("Missing '=' in a 'dynamic' definition"); 
				*pt=0;
				Trim(tampon);
				Trim(pt+1);
				NormalisationNomFichier(nomFichier,pt+1);
				ifstream dyf(nomFichier,modeOuverture);
				if (!dyf.fail() )
					auvol.push_back(nomFichier);
				dynamiques[tampon]=nomFichier;
			}
			relire=NON;
			continue;
		}

		if (!strcmp(tampon,"separators:")) {
			if (nettoiesep==OUI)
				erreur("The separators have already been declared");
			entree.getline(tampon,1000);
			Trim(tampon);
			lecturesep(tampon);
			sep=OUI;
			continue;
		}

		if (!strcmp(tampon,"replacements:")) {
			lectureremplace(entree,tampon);
			relire=NON;
			continue;
		}

		if (!strcmp(tampon,"equivalences:")) {
			if (nettoieconv==OUI)
				erreur("The conversions have already been declared");
			lectureconv(entree,tampon);
			relire=NON;
			continue;
		}
		string err="Error: Unknown tag: ";
		err+=tampon;
		erreur((char*)err.c_str());
	}
	
	//On compile les fichiers au vol
	for (i=0;i<auvol.size();i++) {
		string nomcomp=auvol[i]+".ar";
		*flot_erreur<<"Compiling: "<<auvol[i]<<" into "<<nomcomp<<Endl;
		compileautomaton(auvol[i],nomcomp,300000,NON);
		lexiques.push_back(nomcomp);
	}

	for (i=0;i<compiler.size();i++) {
		string nomcomp=compiler[i]+".ar";
		*flot_erreur<<"Compiling: "<<compiler[i]<<" into "<<nomcomp<<Endl;
		compileautomaton(compiler[i],nomcomp,300000,NON);
	}

	*flot_erreur<<"Lexicons"<<Endl;
	liretrans(lexiques,initiaux,arcs);
	
	*flot_erreur<<"Normalization"<<Endl;
	liretrans(normaliseurs,initiauxnorm,arcsnorm);

	*flot_erreur<<"Guessers"<<Endl;
	liretrans(guessers,initiauxguessers,arcsguessers);
	
	for (i=0;i<fichiersvoc.size();i++) {
		ifstream fv(fichiersvoc[i].c_str(),modeOuverture);
		if (fv.fail() ) {
			string s="Unknown file: ";
			s+=fichiersvoc[i];
			erreur((char*)s.c_str());
		}

		*flot_erreur<<"Loading Vocabulary:"<<fichiersvoc[i].c_str()<<Endl;
		while (!fv.eof()) 
			LitUneRg(fv,rgx,NON);
	}


	for (i=0;i<guessersvoc.size();i++) {
		ifstream fv(guessersvoc[i].c_str(),modeOuverture);
		if (fv.fail() ) {
			string s="Unknown file: ";
			s+=guessersvoc[i];
			erreur((char*)s.c_str());
		}

		*flot_erreur<<"Loading Vocabulary for Guesser:"<<guessersvoc[i].c_str()<<Endl;
		while (!fv.eof()) 
			LitUneRg(fv,rgxguess,OUI);
	}
	*flot_erreur<<Endl;
}

GlobalParseurTrans::GlobalParseurTrans(char* ftrans,char* hmm,char* grammar,char* path) : GlobalParseurHMM(hmm,grammar,path) {
	for (int i=0;i<256;i++)
		alphabet.affecte(i,i);
	nom=strdup(ftrans);
	chargementlexiques();
}


GlobalParseurTrans::~GlobalParseurTrans() {
	int i;
	for (i=0;i<arcs.dernier;i++)
		delete arcs[i];
	for (i=0;i<arcsnorm.dernier;i++)
		delete arcsnorm[i];
	for (i=0;i<arcsguessers.dernier;i++)
		delete arcsguessers[i];

	rgx.nettoie();
	rgxguess.nettoie();
	TasDeparcoursarc.nettoie();
	if (nettoiespace==OUI) {
		for (i=0;i<256;i++) 
			if (spaces[i]!=NULL)
				delete[] spaces[i];
	}
	if (nettoiesep==OUI) {
		for (i=0;i<256;i++) 
			if (separators[i]!=NULL)
				delete[] separators[i];
	}
	if (nettoieconv==OUI) {
		for (i=0;i<256;i++) 
			if (caractereconversion[i]!=NULL)
				libere(caractereconversion[i]);
	}

}

int GlobalParseurTrans::remplace(string surface,string& res) {
	char modif=NON;
	for (map<string, string>::iterator i=remplacements.begin();i!=remplacements.end();i++) {
		if (surface.rfind(i->first)!=-1) {
			res=i->second;
			return i->first.size();
		}
	}
	return 0;
}

int GlobalParseurTrans::ajouteterminaison(string symbole,char test) {
	if (test==OUI) {
		map<string, int>::iterator iter=mterminaison.find( symbole );
		if (iter!=mterminaison.end())
			return mterminaison[symbole];
	}

	if (symbole=="LC")
		LC.ajoute(terminaisons.size());
	if (symbole=="RC")
		RC.ajoute(terminaisons.size());

	mterminaison[symbole]=(int)terminaisons.size();
	terminaisons.push_back(symbole);
	return (terminaisons.size()-1);
}


void construitinitiaux(etat* e,VECTA<int>* initiaux,int ref) {

	char s[2];
	for (int i=0;i<e->arcs.dernier;i++) {
		if (e->arcs[i]->marque==100)
			continue;
		e->arcs[i]->marque=100;
		if (e->arcs[i]->action==AUTOMATONEPSILON)
			construitinitiaux(e->arcs[i]->e,initiaux,ref);
		else {
			unsigned char* str=(unsigned char*)returnstring(s,e->arcs[i]);
			int j;
			if (str==NULL)
				continue;
			if (str[0]==0) {
				for (j=0;j<256;j++)
					initiaux[j].ajoute(e->arcs[i]->e->id+ref);
			}
			else{
				int l=strlen((char*)str);
				for (j=0;j<l;j++)
					initiaux[str[j]].ajoute(e->arcs[i]->e->id+ref);
			}
		}
	}
}

//On cree une duplication de notre automate dans le GlobalParseurTrans
void GlobalParseurTrans::maj(automate* a,automate* alemme,VECTA<arctrans*>& arcscourant,char guesser) {
	int ref=arcscourant.dernier;
	int i,j;
	arctransarc* ar;
	VECTA<arc*> garde;
	etat* et;
	VECTA<arctrans*> finaux;
	arctrans* arfinal;
	
	for (i=-1;i<a->etats.dernier;i++) {
		if (i==0 && alemme!=NULL) {
			ar=new arctransarcbase;
			ar->set(alemme);
		}
		else
			ar=new arctransarc;

		i==-1?et=&a->e:et=a->etats[i];
		garde.raz();	

		//Creation d'un noeud terminal, que l'on connectera en fin d'analyse
		if (et->valeur!=NULL) {
			char* tampon=(char*)et->valeur;
			arfinal=new arctrans;
			arfinal->id=ajouteterminaison(tampon,OUI);
			finaux.ajoute(ar);
			finaux.ajoute(arfinal);
			//On se reserve une case d'ajout pour plus tard
			garde.ajoute(a->e.arcs[0]);
		}			
		//Dupliquation des arcs
		for (j=0;j<et->arcs.dernier;j++) {
			if (et->arcs[j]->manipulation==AUTOMATONDELETED ||
				et->arcs[j]->manipulation==AUTOMATONMARKEDFORDELETION) 
				continue;
			garde.ajoute(et->arcs[j]);
		}

		if (garde.dernier) {
			ar->next=new int[garde.dernier+1];
			ar->next[0]=garde.dernier;
			for (j=0;j<garde.dernier;j++)
				ar->next[j+1]=garde[j]->e->id+ref;
		}
		arcscourant.ajoute(ar);
	}

	for (i=0;i<finaux.dernier;i+=2) {
		ar=(arctransarc*)finaux[i];
		arfinal=finaux[i+1];
		//On le met en tete
		ar->next[1]=arcscourant.dernier;
		arcscourant.ajoute(arfinal);
	}

	char premier=OUI;
	for (i=-1;i<a->etats.dernier;i++) {
		i==-1?et=&a->e:et=a->etats[i];		

		if (et->arcs.dernier!=0) {
			for (j=0;j<et->arcs.dernier;j++)
				arcscourant[et->arcs[j]->e->id+ref]->add(et->arcs[j]);
		}

		premier=NON;
	}

	if (guesser==OUI)
		construitinitiaux(&a->e,initiauxguessers,ref);
	else
		construitinitiaux(&a->e,initiaux,ref);
}

int GlobalParseurTrans::liretraits(int* lemme,char* curseur,int plemme) {
	char* pt=strchr(curseur,'+');
	int id;
	//On rajoute un fin de chaine
	lemme[plemme++]=cfinres;

	while (pt!=NULL) {
		*pt=0;
		id=ajouteterminaison(curseur,OUI);
		lemme[plemme++]=(uchar)id;
		curseur=pt+1;
		pt=strchr(curseur,'+');
	}
	lemme[plemme]=0;
	return plemme;
}


char GlobalParseurTrans::ajoutemot(string& nom,string& surface, string& lem, string& traits) {

	//s'il est deja en memoire...
	VECTA<arctrans*> courants;
	int dernier=0;


	string fi="";
	if (nom!="") {
		map<string, string>::iterator iter=dynamiques.find(nom);
		if (iter!=dynamiques.end())
			fi=dynamiques[nom];
		else
			return 2;
	}


	string trs=traits;
	int lemme[1000];
	int i;
	size_t l;
	for (l=0;l<lem.size();l++)
		lemme[l]=lem[l];
	lemme[l]=0;

	traits+="+";		
	int lgsurface=surface.size();
	//On decoupe les traits selon le "+"
	int lglemme=liretraits(lemme,(char*)traits.c_str()+1,lem.size());

	if (test((uchar*)surface.c_str(),lemme,courants,dernier)==OUI)
		return OUI;


	arctrans* premier=NULL;
	for (i=0;i<courants.dernier;i++) {
		if (courants[i]->id==-1 && courants[i]->type()==0) {
			premier=courants[i];
			break;
		}
	}

	if (premier==NULL)
		dernier=0;	

	arctrans* a=NULL;
	int haut,bas;
	if (lgsurface>=lglemme)
		surface[lgsurface++]=cfinmot;
	int maxlg=lglemme;
	if (maxlg<lgsurface)
		maxlg=lgsurface;
	char code=NON;
	
	for (i=dernier;i<maxlg;i++) {
		if (i>=lgsurface)
			haut=cfinmot;
		else
			haut=surface[i];
		if (i>=lglemme)
			bas=cfinmot;
		else
			bas=lemme[i];
		a=new arctrans;
		a->u=haut;
		a->l=bas;
		if (code==OUI) {
			a->l=0;
			a->id=bas;
		}
		if (bas==cfinres)
			code=OUI;
#ifdef _DEBUG
		a->pos=arcs.dernier;
#endif
		char trie=NON;
		if (!i)
			initiaux[haut].ajoute(arcs.dernier);
		else {
			majnext(premier,arcs.dernier);
			trie=OUI;
		}

		arcs.ajoute(a);
		if (trie==OUI) {
			//On trie ensuite
			int sz=premier->next[0];
			for (int c=0;c<sz-1;c++) {
				arctrans* an=arcs[premier->next[c+1]];
				if (a->u<an->u) {
					for (int cc=premier->next[0];cc>c+1;cc--)
						premier->next[cc]=premier->next[cc-1];
					premier->next[c+1]=arcs.dernier-1;
					break;
				}
			}
		}
		premier=a;

	}
	if (nom=="")
		return OUI;

	//On le rajoute ensuite dans le fichier automatiquement, dont l'alias nous a ete fourni
	ofstream fichier(fi.c_str(),ios::app|ios::binary);
	fichier<<surface.c_str()<<Endl;
	fichier<<lem.c_str()<<Endl;
	fichier<<trs.c_str()<<Endl;
	return OUI;
}

char GlobalParseurTrans::testLCRC(arctrans* a) {
	if (a->id!=-1)
		if (LC.cherche(a->l+a->id)!=-1 || RC.cherche(a->l+a->id)!=-1)
			return OUI;
	return NON;
}

arctrans* GlobalParseurTrans::testNextLCRC(arctrans* an,TYPETESTARC& lcourc) {
	if (an->next==NULL)
		return NULL;
	int t=an->next[0];
	int* n=an->next+1;
	for (int k=0;k<t;k++) {
		arctrans* a=arcsnorm[n[k]];
		if (a->id!=-1) {
			if (LC.cherche(a->l+a->id)!=-1) {
				lcourc=A_LC;
				return a;
			}
			if (RC.cherche(a->l+a->id)!=-1) {
				lcourc=A_RC;
				return a;
			}
		}
	}
	return NULL;
}

#ifdef _DEBUG
void GlobalParseurTrans::reinit(VECTA<arctrans*>& larcs) {
	for (int i=0;i<larcs.dernier;i++) 
		larcs[i]->passage=0;
}
#endif
