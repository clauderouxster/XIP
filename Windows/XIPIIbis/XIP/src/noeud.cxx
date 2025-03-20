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
   Module     : noeud.cxx
   Date       :
   Purpose    : Manipulation de noeuds de l'arbre des chunks
   Programmer : Claude ROUX
   Reviewer   : 
*/

char affichage_supplementaire=0;

#include "parsbin.h"
void strcatchr(char* tmp,char c);
char* TV(GlobalParseur* parseur,Vecteur* v);
char* TC(GlobalParseur* parseur,Traits* tr);
unsigned char* minuscule(unsigned char* c);
//extern int fullpythonlexical;

inline void RemplaceCar(char* ligne,char type,char remplacement) {

    char* pt= strchr(ligne,type);

    while (pt!=NULL) {
        *pt=remplacement;
        pt= strchr(pt+1,type);
    }
}

#define parseurutf8(x) if (parseur->TestOptions(AFFICHAGE_UTF8)) os<<parseur->conversionAsciiVersUTF8(x); else os<<x;
void AffectationTraitsOntologiques(Traits* tr,Traits* RegTraits);
/*-------------------------------------------------------------------------*/
Noeud::Noeud() {
    hauteur=-1;
    etat=-1;
    numero=-1;
    num_mot=-1;
    marquage=-1;
    modifie=0;
	sousnoeud=NON;
    nombre=0;
    dernier=this;precedent=NULL;lemme=NULL;
    frere=NULL;fils=NULL;surface=NULL;
    clef=0;
	gauche=0;
	droite=0;
	gauchecar=0;
	droitecar=0;
	source=NULL;
    X=NULL;
    clef_suivant=0;
    pere=NULL;
    soeur=NULL;
    num_noeud=-1;
    fictif=NON;
    num_gauche=0;
    num_droite=0;
}

Noeud::Noeud(Cat* x,Liste* L) {
    hauteur=-1;
    if (x!=NULL)
        surface=x->nom;
    else
        surface=NULL;

    clef=0;
    X=x;
    nombre=0;
    marquage=-1;
    numero=-1;
    modifie=0;
	sousnoeud=NON;
    soeur=NULL;
    lemme=NULL;
    dernier=this;
    precedent=NULL;
    frere=NULL;
    fils=NULL;
    num_mot=0;
    num_noeud=-1;
    gauche=0;
    droite=0;
	gauchecar=0;
	droitecar=0;
    num_gauche=0;
    num_droite=0;
    source=L;
    clef_suivant=0;
    pere=NULL;
    fictif=NON;
}

 Noeud::~Noeud() {
 }

Traits* Noeud::lesTraits() {
    if (source==NULL)
        return NULL;
    return source->traitsactif;
}

Traits* Noeud::lesTraitsAffiche() {
    if (source==NULL)
        return NULL;
    return source->traitsbase;
}


void Noeud::copie(Noeud* n) {
    dernier=this;
    precedent=NULL;
    frere=NULL;
    fils=NULL;
    marquage=-1;
    numero=-1;
    modifie=0;
	sousnoeud=NON;
    pere=NULL;
    clef_suivant=0;


    hauteur=n->hauteur;
    clef=n->clef;
    X=n->X;
    lemme=n->lemme;
    surface=n->surface;

    num_mot=n->num_mot;
    num_noeud=n->num_noeud;
    gauchecar=n->gauchecar;
    droitecar=n->droitecar;
    gauche=n->gauche;
    droite=n->droite;
    num_gauche=n->num_gauche;
    num_droite=n->num_droite;
}
    
void Noeud::annulereconstruction() {
    dernier=this;
    marquage=-1;    
    modifie=OUI;    
    pere=NULL;
    hauteur=-1;
    if (source!=NULL) {
		source->instanciateur=NULL;
        RetireDebut(source->traitsactif);
        RetireFin(source->traitsactif);
        RetireDebut(source->traitsbase);
        RetireFin(source->traitsbase);
    }   
}

void Noeud::annule() {
    dernier=this;
    frere=NULL;
    precedent=NULL;
    marquage=-1;    
    modifie=0;
	sousnoeud=NON;
	if (source!=NULL)
		source->instanciateur=NULL;
    pere=NULL;
}

void Noeud::retire() {
    dernier->marquage=-1;
    dernier->modifie=0;    
	dernier->sousnoeud=NON;
    dernier=dernier->precedent;
    (dernier->frere)->annule();
    dernier->frere=NULL;
	if (source!=NULL)
		source->instanciateur=NULL;
}

void Noeud::RetTete() {
    if (frere) {
        frere->dernier=dernier;
        frere->precedent=NULL;
    }
    annule();
}

void Noeud::initialise(Cat* x,Liste* L) {
    if (x!=NULL)
        surface=x->nom;
    else
        surface=NULL;

    clef=0;
    X=x;
	infos="";
    hauteur=-1;
    marquage=-1;
    modifie=0;
	sousnoeud=NON;
    numero=-1;
    soeur=NULL;
    lemme=NULL;
    dernier=this;
    precedent=NULL;
    frere=NULL;
    fils=NULL;
    num_mot=0;
    num_noeud=-1;
    gauche=0;
    droite=0;
	num_gauche=0;
    num_droite=0;
    gauchecar=0;
    droitecar=0;
    source=L;
    clef_suivant=0;
    pere=NULL;
	fictif=NON;
 }

//On compte la hauteur de l'arbre commun...
int Noeud::distancearbre(Noeud* n) {
	int hthis = 0;
	Noeud* nbase = this;
	Noeud* nn;
	while (nbase != NULL) {
		nn = n;
		int hn = 0;
		while (nn != NULL) {
			if (nn == nbase)
				return (hthis + hn);
			nn = nn->pere;
			++hn;
		}
		++hthis;
		nbase = nbase->pere;
	}

	return hthis;
}


void Noeud::insertion(Noeud* n) {
    dernier->frere=n;
    n->frere=NULL;
    n->precedent=dernier;
    dernier=n;
    n->pere=pere;
}

void Noeud::inseresous(Noeud* n) {
    if (fils==NULL) {
        fils=n;
        n->pere=this;
        n->dernier=n;
    }
    else
        fils->insertion(n);
}

void Noeud::insere(Noeud* n) {
    dernier->frere=n;
    n->frere=NULL;
    n->precedent=dernier;
    dernier=n;
}

//Cette methode n'est utilise que dans le cadre de l'affichage des fonctions syntaxiques...
//voir avlfonc.cxx pour une utilisation de cette methode...
//Elle peut aussi bien afficher un syntagme complet qu'un simple mot...
int Noeud::AfficheMot(GlobalParseur* parseur,ostream& os) {
    char fermante=NON;
    Noeud* n;
    Traits* Valeur=lesTraitsAffiche();

    if (affichage_supplementaire==1 && fils!=NULL)
        os<<ConversionChaine(parseur,surface)<<">";

    if (fils ==NULL) {

        char chaine[2];
        chaine[0]=0;

        parseur->Chaine_traits[0]=0;
        if (pere!=NULL) {
            //On affiche les traits du noeud superieur, 
            //puisque les traits sur un noeud
            //lexicaux ne sont jamais modifies...
            Traits* trpere=pere->lesTraitsAffiche();
			if (trpere!=NULL) {
				(trpere->valeur)->ConcateneValeurs(parseur,
					parseur->Chaine_traits,
					parseur->FILTRENOEUD);
				if (trpere->onto!=NULL) {
					for (int ix=0;ix<trpere->onto->dernier;ix++) {
						trpere->onto->cell(ix)->ConcateneValeurs(parseur,parseur->hierarchies->lahierarchie,parseur->TraitsFiltreOntologie,parseur->Chaine_traits,"_");                    
					}
				}
			}
        }
        if (!parseur->TestOptions(AFFICHAGE_ENTREE)) {
            //Affichage du lemme seulement
            if (parseur->TestOptions(AFFICHAGE_REDUIT | AFFICHAGE_LEMME)) {
                if (lemme!=NULL)
                    os<<ConversionChaine(parseur,lemme);
                else
                    os<<ConversionChaine(parseur,surface);
            }
            else
                os<<ConversionChaine(parseur,surface);

            if (parseur->TestAfficheFonction(AFFICHE_NUM_PHRASE)) {
                if (lemme!=NULL)
                    os<<"^"<<ConversionChaine(parseur,lemme);
                
				if (parseur->TestOptions(AFFICHAGE_OFFSET)) {
					if (gauche!=gauchecar || droite!=droitecar)
						os<<":"<<"["<<gauche<<"-"<<droite<<"|"<<gauchecar<<"-"<<droitecar<<"]";
					else
						os<<":"<<"["<<gauche<<"-"<<droite<<"]";
				}
				else
					if (parseur->TestOptions(AFFICHAGE_NUM_MOT))
						os<<":"<<num_mot;            
            }
            else
                if (parseur->TestOptions(AFFICHAGE_COMPLET)) {
                    if (lemme !=NULL)
                        os<<"^"<<ConversionChaine(parseur,lemme);
                    os<<ConversionChaine(parseur,parseur->Chaine_traits)<<":";
					if (parseur->TestOptions(AFFICHAGE_OFFSET)) {
						if (gauche!=gauchecar || droite!=droitecar)
							os<<":"<<"["<<gauche<<"-"<<droite<<"|"<<gauchecar<<"-"<<droitecar<<"]";
						else
							os<<":"<<"["<<gauche<<"-"<<droite<<"]";
					}
                    else
						if (parseur->TestOptions(AFFICHAGE_NUM_MOT))
							os<<num_mot;            
                }
                else {
                    os<<ConversionChaine(parseur,parseur->Chaine_traits);    
					if (parseur->TestOptions(AFFICHAGE_OFFSET)) {
						if (gauche!=gauchecar || droite!=droitecar)
							os<<":"<<"["<<gauche<<"-"<<droite<<"|"<<gauchecar<<"-"<<droitecar<<"]";
						else
							os<<":"<<"["<<gauche<<"-"<<droite<<"]";
					}
					else
						if (parseur->TestOptions(AFFICHAGE_NUM_MOT))
							os<<":"<<num_mot;
                }
        }
        else {
            if (pere!=NULL && pere->source!=NULL) {
                Liste* ancien=pere->source;
                if (ancien->voc!=NULL &&
                    ancien->voc->tag[ancien->choisie]) { 
                    //ancien->voc->tag[ancien->choisie]!=NULL) {
                    strcpy_s(parseur->Chaine_traits,3000,
                           ancien->voc->tag[ancien->choisie]);
                    RemplaceCar(parseur->Chaine_traits,'\n',0);
                    RemplaceCar(parseur->Chaine_traits,'\t','^');
                    os<<ConversionChaine(parseur,parseur->Chaine_traits);
                }
				else {
					os<<ConversionChaine(parseur,surface)<<"^";
					os<<ConversionChaine(parseur,surface)<<"^+Fictive";
				}
            }
            else {
                os<<ConversionChaine(parseur,surface);
				os<<ConversionChaine(parseur,parseur->Chaine_traits);
                if (lemme!=NULL)
                    os<<"^"<<ConversionChaine(parseur,lemme);
                if (chaine[0])
                    os<<"^"<<ConversionChaine(parseur,chaine);
            } 
            
			if (parseur->TestOptions(AFFICHAGE_OFFSET)) {
				if (gauche!=gauchecar || droite!=droitecar)
					os<<":"<<"["<<gauche<<"-"<<droite<<"|"<<gauchecar<<"-"<<droitecar<<"]";
				else
					os<<":"<<"["<<gauche<<"-"<<droite<<"]";
			}
            else
				if (parseur->TestOptions(AFFICHAGE_NUM_MOT))
					os<<":"<<num_mot;            
        }
        
        return OUI;
    }
    

    n=fils;
    while (n!=NULL) {
        n->AfficheMot(parseur,os);
        n=n->frere;
        if (n) {
            if (parseur->TestOptions(AFFICHAGE_COMPLET))
                os<<",";
            else
                os<<" ";
        }
    }

    if (fermante)
        os<<"}"<<Endl;

    return  OUI;
}

inline char testmaxchaine(unsigned char* ch,char* chh,size_t max) {
    size_t l=strlen((char*)ch)+strlen(chh)+1;
    if (l<max)
        return OUI;
    return NON;
}

void ajoutechainepourautomate(string& chaine,char* ajoute) {
    switch(ajoute[0]) {
    case '~':
    case '\\':                 
    case '?':
    case '%':
    case ',':
    case ']':
    case ')':
    case '[':
    case '(':
        chaine+="\\";
    }
    chaine+=ajoute;
}


void Noeud::AutomateLemme(string& chaine) {
    if (source->voc!=NULL) {
        if (chaine.size()!=0)
            chaine+=" ";     

        if (source->voc->nbX==1) 
               ajoutechainepourautomate(chaine,source->voc->lemme[0]);    
        else {
			chaine+="[";
			for (int i=0;i<source->voc->nbX;i++) {
				if (i)
					chaine+=",";
				ajoutechainepourautomate(chaine,source->voc->lemme[i]);    
			}
			chaine+="]";
        }            
        return;
    }

	Noeud* n=fils;
	while (n!=NULL) {
		char ajoutebl=OUI;
		n->AutomateLemme(chaine);
		n=n->frere;
	}
}

void ajouteunique(vector<string>& vs,string& chaine) {
	for (int i=0;i<vs.size();i++)
		if (vs[i]==chaine)
			return;
	vs.push_back(chaine);
}

void Noeud::ListeLemme(vector<string>& vs) {
	string chaine;
	int j;
    if (source->voc!=NULL) {
		if (source->voc->nbX==1) {
			   string chaine;
               ajoutechainepourautomate(chaine,source->voc->lemme[0]);    
			   //On repartit alors cette nouvelle chaine sur tous les elements
			   if (vs.size()==0)
				   vs.push_back(chaine);
			   else{
				   for (j=0;j<vs.size();j++) {
					   vs[j]+=" ";
					   vs[j]+=chaine;
				   }
			   }
		}
        else {			
			//On duplique d'abord les chaines existantes:
			int max=vs.size();
			if (max==0) {
				max=1;
				for (int i=0;i<source->voc->nbX;i++)
					vs.push_back("");
			}
			else {
				for (j=0;j<max;j++) 
					vs[j]+=" ";
				for (int i=0;i<source->voc->nbX-1;i++) {
					for (j=0;j<max;j++) 
						vs.push_back(vs[j]);
				}
			}
			for (int i=0;i<source->voc->nbX;i++) {
				string chaine;
				ajoutechainepourautomate(chaine,source->voc->lemme[i]);    
				for (j=0;j<max;j++)
					vs[j+i*max]+=chaine;
			}
        }            
        return;
    }

	Noeud* n=fils;
	while (n!=NULL) {
		char ajoutebl=OUI;
		n->ListeLemme(vs);
		n=n->frere;
	}
}



void Noeud::Lemme(uchar* chaine,size_t max) {
	if (fils==NULL) {
		if (lemme!=NULL && testmaxchaine(chaine,lemme,max)) {
			if (chaine[0]!=0)
				strcat((char*)chaine," ");
			strcat((char*)chaine,lemme);            
		}
		return;
	}

	Noeud* n=fils;
	while (n!=NULL) {
		n->Lemme(chaine,max);        
		n=n->frere;
	}
}

void Noeud::Lemme(string& chaine) {
	if (fils==NULL) {
		if (lemme!=NULL) {
			if (chaine.size()!=0)
				chaine+=" ";
			chaine+=lemme;
		}
		return;
	}

	Noeud* n=fils;
	while (n!=NULL) {
		n->Lemme(chaine);        
		n=n->frere;
	}
}

void Noeud::Surface(string& chaine,char ajouteblanc,long g,long d) {
	if (fils==NULL) {
		if (surface!=NULL) {
			if (chaine.size()!=0 && ajouteblanc==OUI)
				chaine+=" ";
			chaine+=surface;
		}
		return;
	}

	Noeud* n=fils;
	while (n!=NULL) {
		char ajoutebl=OUI;
		g=n->gauche;

		if (d==-1)
			d=n->gauche;
		else			
			if (d==g)
				ajoutebl=NON;
		n->Surface(chaine,ajoutebl,g,d);        
		d=n->droite;
		n=n->frere;
	}
}

void Noeud::Surface(uchar* chaine,size_t max,char ajouteblanc,long g,long d) {
    if (fils==NULL) {
        if (testmaxchaine(chaine,surface,max)) {
            if (chaine[0]!=0 && ajouteblanc==OUI)
                strcat((char*)chaine," ");
            strcat((char*)chaine,surface);        
        }
        return;
    }

    Noeud* n=fils;
	while (n!=NULL) {
		char ajoutebl=OUI;
		g=n->gauche;

		if (d==-1)
			d=n->gauche;
		else			
			if (d==g)
				ajoutebl=NON;
        n->Surface(chaine,max,ajouteblanc,g,g);
		d=n->droite;
        n=n->frere;
    }
}

//Cette methode n'est utilise que dans le cadre de l'affichage des fonctions syntaxiques...
//voir avlfonc.cxx pour une utilisation de cette methode...
//Elle peut aussi bien affiche un syntagme complet qu'un simple mot...
void Noeud::AfficheMotPF(GlobalParseur* parseur) {
    char fermante=NON;
    Noeud* n;


    if (fils ==NULL)
        cout<<surface<<":"<<num_mot;
    else { 
        cout<<X->nom;
        if (pere==NULL)
            cout<<":X";
        else
            cout<<":"<<pere->X->nom;
        cout<<"{";
        n=fils;        
        while (n!=NULL) {
            n->AfficheMotPF(parseur);
            n=n->frere;
            if (n)
                cout<<",";                
        }
        cout<<"}";
    }
}


void Noeud::SauveResultat(GlobalParseur* parseur,ostream& os) {

    //Dans le cas d'un noeud cree par une dependance, on ne l'affiche pas
    if (droite==-10)
        return;

    Noeud* n;
    char ch[100];
    char fermante=NON;
    Traits* Valeur=lesTraitsAffiche();
	int i;

    ch[0]=0;

	if (fils !=NULL) {
        if (parseur->TestOptions(AFFICHAGE_CATEGORIE) || (fils->fils !=NULL || fils->lemme==NULL)) {
            if (source==NULL)
                os<<ConversionChaine(parseur,surface);
            else
                if (source->voc==NULL)
                    os<<ConversionChaine(parseur,surface);
                else {
                    ch[0]=0;
                    TVecteur code=0;
                    char ajout=NON;
                    for (i=0;i<source->voc->X.dernier;i++) {
                        if ((code&source->voc->X[i]->Val)!=0)
                            continue;
                        code|=source->voc->X[i]->Val;
                        if (ajout)
                            strcatchr(ch,';');
                        ajout=OUI;
                        strcat_s(ch,100,source->voc->X[i]->nom);
                    }
                    os<<ConversionChaine(parseur,ch);
                }
                    
            if (fils->fils!=NULL)
                if (parseur->TestOptions(AFFICHAGE_OFFSET)) {
					if (gauche!=gauchecar || droite!=droitecar)
						os<<":"<<"("<<gauche<<"-"<<droite<<"|"<<gauchecar<<"-"<<droitecar<<")";
					else
	                    os<<"("<<gauche<<":"<<droite<<")";
				}
                else
					if (parseur->TestOptions(AFFICHAGE_NUM_MOT))
						os<<"("<<num_gauche<<":"<<num_droite<<")";

            parseur->Chaine_traits[0]=0;
			if (Valeur!=NULL) {
                (Valeur->valeur)->ConcateneValeurs(parseur,
                                                   parseur->Chaine_traits,
                                                   parseur->FILTRENOEUD);            
				if (Valeur->onto!=NULL) {
					for (i=0;i<Valeur->onto->dernier;i++) {
						Valeur->onto->cell(i)->ConcateneValeurs(parseur,parseur->hierarchies->lahierarchie,parseur->TraitsFiltreOntologie,parseur->Chaine_traits,"_");                    
					}
				}
			}
            os<<ConversionChaine(parseur,parseur->Chaine_traits)<<"{";
            fermante=OUI;
        }
    }

	if (fils != NULL && fils->fils==NULL) {    
		parseur->Chaine_traits[0]=0;		
		if (Valeur!=NULL) {
			(Valeur->valeur)->ConcateneValeurs(parseur,
				parseur->Chaine_traits,
				parseur->FILTRENOEUD);
			if (Valeur->onto!=NULL) {
				for (i=0;i<Valeur->onto->dernier;i++) {
					Valeur->onto->cell(i)->ConcateneValeurs(parseur,parseur->hierarchies->lahierarchie,parseur->TraitsFiltreOntologie,parseur->Chaine_traits,"_");                    
				}
			}
		}

		ch[0]=0;
		if (parseur->TestOptions(AFFICHAGE_OFFSET)) {
			if (fils->gauche!=fils->gauchecar || fils->droite!=fils->droitecar)
				sprintf_s(ch,100,"[%d-%d|%d-%d]",fils->gauche,fils->droite,fils->gauchecar,fils->droitecar);
			else
				sprintf_s(ch,100,"[%d-%d]",fils->gauche,fils->droite);
		}
        else
			if (parseur->TestOptions(AFFICHAGE_NUM_MOT))
				sprintf_s(ch,100,"%d",fils->num_mot);

        if (!parseur->TestOptions(AFFICHAGE_ENTREE)) {
            if (parseur->TestOptions(AFFICHAGE_REDUIT|AFFICHAGE_LEMME)) {
                if (fils->lemme!=NULL)
                    os<<ConversionChaine(parseur,fils->lemme);
                else
                    os<<ConversionChaine(parseur,fils->surface);
            }
            else
                os<<ConversionChaine(parseur,fils->surface);
            
            
            if (parseur->TestOptions(AFFICHAGE_COMPLET)) {
                if (fils->lemme!=NULL)
                    os<<"^"<<ConversionChaine(parseur,fils->lemme);
                os<<ConversionChaine(parseur,parseur->Chaine_traits)<<":"<<ch;
            }
            else {
                os<<ConversionChaine(parseur,parseur->Chaine_traits);
                if (parseur->TestOptions(AFFICHAGE_OFFSET) ||
					parseur->TestOptions(AFFICHAGE_NUM_MOT))
                    os<<":"<<ch;
            }
        }
        else {
            if (source!=NULL && source->voc!=NULL && source->voc->tag[source->choisie]) {
                strcpy_s(parseur->Chaine_traits,3000,(source->voc)->tag[source->choisie]);   
                RemplaceCar(parseur->Chaine_traits,'\n',0);
                RemplaceCar(parseur->Chaine_traits,'\t','^');
                os<<ConversionChaine(parseur,parseur->Chaine_traits);
            }
            else {
                os<<ConversionChaine(parseur,fils->surface);
				os<<ConversionChaine(parseur,parseur->Chaine_traits);
                if (fils->lemme!=NULL)
                    os<<"^"<<ConversionChaine(parseur,fils->lemme);
            }
            
            os<<":"<<ch;
        }
         
        //if (fermante)
        //    os<<"}";        
		if (fils->lemme==NULL && fils->droite!=-10) {
			if (parseur->TestOptions(AFFICHAGE_COMPLET)) 
				os<<",";
			else
				os<<" ";
		}
    }


    n=fils;
    while (n!=NULL) {
        if (n->fils!=NULL) {
            n->SauveResultat(parseur,os);
            n=n->frere;
            if (n && n->droite!=-10) {
                if (parseur->TestOptions(AFFICHAGE_COMPLET)) 
                    os<<",";
                else
                    os<<" ";
            }
        }
        else
            n=n->frere; 
    }
    
	if (fermante) {
		if (parseur->TestOptions(AFFICHAGE_CAT_FERMANTE))
			os << "}" << surface;
		else
			os << "}";
	}
}

#ifndef XIPLIBXML
void GlobalParseur::RemplaceChaine(ostream& buffer,char* chaine) {
    int pos=0;

    while (chaine[pos]!=0) {
        char modif=NON;
        for (int i=0;i<caractereXML.dernier;i++) {
            if (!memcmp(chaine+pos,caractereXML[i],strlen(caractereXML[i]))) {
                buffer<<codageXML[i];
                pos+=strlen(caractereXML[i]);
                modif=OUI;
                break;
            }
        }
        if (modif==NON) {
            buffer<<chaine[pos];
            pos++;
        }
    }
}
#endif

#ifdef XIPLIBXML
 /** Start MCR added code for passage XML output **/
void GlobalParseur::RemplaceChaine(ostream& buffer, char* chaine) {
	xmlChar* champ = xmlEncodeEntitiesReentrant(refdocument, (uchar*)chaine);
	buffer << champ;
	xmlFree(champ);
}

void Noeud::ConstruitNoeudXML(GlobalParseur* parseur, xmlNodePtr parent) {
	if (parseur->TestOptions(AFFICHAGE_PASSAGE_XML_FORMAT) == OUI) {
		// first output all tokens
		ConstruitNoeudXMLPassageTokens(parseur, parent);
		// then process groups
		int numGroupe = 0;
		ConstruitNoeudXMLPassageGroupes(parseur, parent, numGroupe);
	} else {
		ConstruitNoeudXMLStandard(parseur, parent);
	}
}
void Noeud::ConstruitNoeudXMLPassageTokens(GlobalParseur* parseur,xmlNodePtr noeudParent){

	// we are not interested in fictive nodes
	if (this->fictif == OUI) {
		return;
	}

	if (this->fils == NULL) {
        xmlNodePtr node=parseur->CreationNoeudXML(noeudParent, "T", surface);
	    char ch[100];
		sprintf_s(ch,100,"E%dT%d",parseur->num_phrase, this->num_mot);
		this->idPassage = new char[strlen(ch)];
		strcpy(this->idPassage, ch);
		parseur->AjouteAttributValeur(node, "id", ch);
		sprintf_s(ch,100,"%d",gauche);
		parseur->AjouteAttributValeur(node,"start",ch);
		sprintf_s(ch,100,"%d",droite);
		parseur->AjouteAttributValeur(node,"end",ch);
	} else {
		Noeud* n = this->fils;
		while (n != NULL) {
			n->ConstruitNoeudXMLPassageTokens(parseur, noeudParent);
			n = n->frere;
		}
	}
}

void Noeud::ConstruitNoeudXMLPassageGroupes(GlobalParseur* parseur,xmlNodePtr noeudParent, int& numGroupe){
    Noeud* n;
    char ch[100];
    xmlNodePtr node;

	// we are not interested in fictive nodes, nor in leaves
	if (this->fictif == OUI || this->fils == NULL) {
		return;
	}
    ch[0]=0;
	if (this->hauteur == 0) { // we are not interested in the root node
		node = noeudParent;
	} else {
		if (this->source->voc != NULL) { // noeud terminal
			node = parseur->CreationNoeudXML(noeudParent, "W", NULL);
			sprintf_s(ch,100,"E%dW%d",parseur->num_phrase, this->num_mot);
			parseur->AjouteAttributValeur(node, "id", ch);
			this->idPassage = new char[strlen(ch)];
			strcpy(this->idPassage, ch);
			Noeud* noeudsToken = this->fils;
			strcpy_s(ch,100, "");
			while (noeudsToken != NULL) {
				strcat_s(ch,100, noeudsToken->idPassage);
				noeudsToken = noeudsToken->frere;
				if (noeudsToken != NULL) {
					strcat_s(ch,100, " ");
				}
			}
			parseur->AjouteAttributValeur(node, "tokens", ch);
			parseur->AjouteAttributValeur(node, "pos", this->X->nom);
			parseur->AjouteAttributValeur(node, "lemma", this->lemme);
			parseur->AjouteAttributValeur(node, "form", this->surface);
		} else {
			node = parseur->CreationNoeudXML(noeudParent, "G",NULL);
			parseur->AjouteAttributValeur(node, "type", this->X->nom);
			sprintf_s(ch,100,"E%dG%d",parseur->num_phrase, numGroupe++);
			parseur->AjouteAttributValeur(node, "id", ch);
			this->idPassage = new char[strlen(ch)];
			strcpy(this->idPassage, ch);
		}
		/*
		// output features
		// features we are interested in are held by the terminal source
		Vecteur* traits = this->fils->source->traitsbase->valeur;
		// we copy it in order to apply filters
		Vecteur v(traits);
		// apply filters
		v.et(parseur->FILTRE);

		// to output the key/value pairs we need to process 
		// the multi-vecteur structure holding the bit map representation 
		// of the features.
		string* outputTraits = new string("");
		for (int i=traits->TailleVect-1;i>=0; i--) {
			STVecteur filtre = (STVecteur) v.vecteur[i];
			if (filtre != 0) {
				for (int j=gBitVect-1; j>=0; j--) {
	                if (filtre < 0) {
						int pos= (i*gBitVect + j)*2;
						if (strcmp(parseur->TableDesTraits[pos+1],"+") == 0) {
							string val(parseur->TableDesTraits[pos]);
							*outputTraits +=  val + " ";
						}
					}
					filtre<<=1;
				}
			}
		}
		if (outputTraits->compare("") !=0) {
			char* val = new char[outputTraits->length()+1];
			strcpy(val, outputTraits->c_str());
			parseur->AjouteAttributValeur(node, "mstag", val);
			delete[] val;
		}
		delete outputTraits;
		*/
	}
	n=this->fils;
	while (n!=NULL) {
		n->ConstruitNoeudXMLPassageGroupes(parseur, node, numGroupe);
		n=n->frere;
	}
}

void Noeud::ConstruitNoeudXMLStandard(GlobalParseur* parseur,xmlNodePtr parent) {
    Noeud* n;
    char ch[100];
    char fermante=NON;
    Traits* Valeur=lesTraitsAffiche();
    xmlNodePtr node;

    ch[0]=0;
    if (fils!=NULL) {
        node=parseur->CreationNoeudXML(parent,"NODE",NULL);
        sprintf_s(ch,100,"%d",num_noeud);
        parseur->AjouteAttributValeur(node,"num",ch);
        parseur->AjouteAttributValeur(node,"tag",surface);
        sprintf_s(ch,100,"%d",gauche);
        parseur->AjouteAttributValeur(node,"start",ch);
        sprintf_s(ch,100,"%d",droite);
        parseur->AjouteAttributValeur(node,"end",ch);
    }
    else {
        node=parseur->CreationNoeudXML(parent,"TOKEN",surface);
        parseur->AjouteAttributValeur(node,"pos",((pere->source)->X)->nom);
        sprintf_s(ch,100,"%d",gauche);
        parseur->AjouteAttributValeur(node,"start",ch);
        sprintf_s(ch,100,"%d",droite);
        parseur->AjouteAttributValeur(node,"end",ch);
    }
    
    if (parseur->DESAMBIGUISATION==OUI) {
        if (fils==NULL && pere!=NULL)
            pere->source->traitsbase->ConstruitXML(parseur,node,parseur->FILTRE);
    }
    else
        if (fils!=NULL && Valeur!=NULL)
            Valeur->ConstruitXML(parseur,node,parseur->FILTRE);

    if (parseur->mathematique) {
        for (int i=0;i<source->variables.dernier;i++) {
            //On met a jour les variables avec les noeuds auxquels elles sont liees
            if (source->variables[i]!=NULL && source->variables[i]->utilisee)
                source->variables[i]->CreationXMLNoeud(parseur,node,this);
        }
    }
    
    //On penetre dans le coeur de l'entree
    if (fils ==NULL) {
        Liste* L=pere->source;
        ListeVoc* voc=L->voc;
        int choix=L->choisie;
        xmlNodePtr lecture=parseur->CreationNoeudXML(node,"READING",NULL);
        parseur->AjouteAttributValeur(lecture,"lemma",lemme);
        parseur->AjouteAttributValeur(lecture,"pos",(L->X)->nom);
		if (voc->tr[choix]!=NULL)
			voc->tr[choix]->ConstruitXML(parseur,lecture,parseur->FILTRE);

        for (int i=0;i<voc->nbX;i++) {
            if (i!=choix) {
                lecture=parseur->CreationNoeudXML(node,"READING",NULL);
                parseur->AjouteAttributValeur(lecture,"lemma",lemme);
                parseur->AjouteAttributValeur(lecture,"pos",(L->X)->nom);
                if (voc->tr[i]!=NULL)
                    voc->tr[i]->ConstruitXML(parseur,lecture,parseur->FILTRE);
            }
        }
    }
          

    n=fils;
    while (n!=NULL) {
        n->ConstruitNoeudXMLStandard(parseur,node);
        n=n->frere;
    }
}
/** End MCR added code for passage XML output **/
#endif

void Noeud::SauveResultatXML(GlobalParseur* parseur,ostream& os) {
    Noeud* n;
    char ch[100];
    char fermante=NON;
    Traits* Valeur=lesTraitsAffiche();

    ch[0]=0;
    if (fils!=NULL) {
        os<<"<NODE num=\""<<num_noeud<<"\" tag=\"";
        parseur->RemplaceChaine(os,(char*)surface);
        os<<"\" start=\""<<gauche<<"\" end=\""<<droite<<"\">"<<Endl;
    }
    else {
        os<<"<TOKEN pos=\""<<((pere->source)->X)->nom;
        os<<"\" start=\""<<gauche<<"\" end=\""<<droite<<"\">"<<Endl;
        parseur->RemplaceChaine(os,(char*)surface);
        os<<Endl;
    }
    
    
    if ( (fils!=NULL || parseur->DESAMBIGUISATION==OUI) 
         && Valeur!=NULL)
        Valeur->AfficheValeursXML(parseur,os,parseur->FILTRE);

    if (parseur->mathematique) {
        for (int i=0;i<source->variables.dernier;i++) {
            //On met a jour les variables avec les noeuds auxquels elles sont liees
            if (source->variables[i]!=NULL && source->variables[i]->utilisee)
                source->variables[i]->afficheNoeudXML(os,this);
        }
    }
    
    //On penetre dans le coeur de l'entree
    if (fils ==NULL) {
        Liste* L=pere->source;
        ListeVoc* voc=L->voc;
        int choix=L->choisie;

        os<<"<READING lemma=\"";
        os<<ConversionChaine(parseur,lemme);

		os<<"\" pos=\""<< (L->X)->nom<<"\">"<<Endl;
		if (voc->tr[choix]!=NULL)
			voc->tr[choix]->AfficheValeursXML(parseur,os,parseur->FILTRE);
		os<<"</READING>"<<Endl;

        for (int i=0;i<voc->nbX;i++) {
            if (i!=choix) {
                os<<"<READING lemma=\"";
                os<<ConversionChaine(parseur,voc->lemme[i]);
                os<<"\" pos=\""<< (voc->X[i])->nom<<"\">"<<Endl;

                if (voc->tr[i]!=NULL)
                    voc->tr[i]->AfficheValeursXML(parseur,os,parseur->FILTRE);
                os<<"</READING>"<<Endl;
            }
        }
    }
          

    n=fils;
    while (n!=NULL) {
        n->SauveResultatXML(parseur,os);
        n=n->frere;
    }

    if (fils!=NULL)
        os<<"</NODE>"<<Endl;
    else
        os<<"</TOKEN>"<<Endl;
}

   
int Noeud::TraitsEnChaine(GlobalParseur* parseur,char* chaine_traits) {
    Traits* Valeur=lesTraitsAffiche();
    if (Valeur) {
        chaine_traits[0]=0;
        Valeur->TraitsEnChaine(parseur,chaine_traits);
        if (fils != NULL && fils->lemme!=NULL) {
            int lon=strlen(chaine_traits)-1;
            if (lon >= 1) {
                sprintf(chaine_traits+lon,",%s:%s]",parseur->LEMME,fils->lemme);
                //chaine_traits[lon]=',';
                //strcat(chaine_traits,parseur->LEMME);
                //strcat(chaine_traits,":");
                //strcat(chaine_traits,(char*)fils->lemme);
                //strcat(chaine_traits,"]");
            }
        }
        return 1;
    }
    return 0;
}

int Noeud::TraitsEnChaineString(GlobalParseur* parseur,string& chaine_traits) {
    Traits* Valeur=lesTraitsAffiche();
    if (Valeur) {
        Valeur->TraitsEnChaineString(parseur,chaine_traits);
        return 1;
    }
    return 0;
}

void Noeud::ConstruitUnNoeud(GlobalParseur* pars, 
                             string& rpos,
                             string& rsurface,
                             string& rlemme,
                             string& rtraits,
							 string& rinitialfeatures,
                             int& rpere,
                             int& rfils,
                             int& rdernier,
                             int& rsuivant,
                             int& rprecedent) {


    string buff;
    Surface(buff);
    rsurface=ConversionChaine(pars,STR(buff));

	rpos=X->nom;	

	if (pars->fullpythonlexical==OUI && source->voc!=NULL && source->voc->nbX>1) {
		rlemme=ConversionChaine(pars,source->voc->lemme[source->choisie]);
		if (source->voc->tag[source->choisie]!=NULL)
			rinitialfeatures=ConversionChaine(pars,source->voc->tag[source->choisie]);
		rpos=ConversionChaine(pars,source->voc->X[source->choisie]->nom);
		source->voc->tr[source->choisie]->TraitsEnChaineString(pars,rtraits);		
	}
	else {
		buff="";		
		if (pars->fullpythonlexical==OUI && source->voc!=NULL && source->voc->tag[source->choisie]!=NULL)
			rinitialfeatures=ConversionChaine(pars,source->voc->tag[source->choisie]);
		Lemme(buff);
		rlemme=ConversionChaine(pars,STR(buff));
		source->traitsbase->TraitsEnChaineString(pars,rtraits);
	}
    
	rtraits=ConversionChaine(pars,(char*)rtraits.c_str());
    rpere=-1;
    if (pere!=NULL)
        rpere=pere->num_noeud;
    rfils=-1;
    if (fils!=NULL)
        rfils=fils->num_noeud;
    rdernier=-1;
    if (dernier!=NULL)
        rdernier=dernier->num_noeud;
    rsuivant=-1;
    if (frere!=NULL)
        rsuivant=frere->num_noeud;
    rprecedent=-1;
    if (precedent!=NULL)
        rprecedent=precedent->num_noeud;
}



void Noeud::affiches(GlobalParseur* parseur,int x) {
    Noeud* n;
    int i;
    ostream& sor=*(parseur->Sortie);
    Traits* trcourant;

    sor << Endl;    
    int k=x;

    if (lemme==NULL && source != NULL && source->voc!=NULL && (source->voc)->nbX > 1) {
        for (i=0;i<(source->voc)->nbX;i++) {
            for (int j=0;j<k;j++) sor << " ";
            if (!i) {
                sor<<"{";
                k++;
            }
            sor << ((source->voc)->X[i])->nom<< "<" << (source->voc)->lemme[i] << ">";
            sor << "("<<gauche<<"-"<<droite<<")";
            if ((source->voc)->tr[i]!=NULL) {
                (source->voc)->tr[i]->TraitsEnChaine(parseur,parseur->Chaine_traits);
                sor<<minuscule((uchar*)parseur->Chaine_traits);
            }
            else
                sor<<"[]";
            if (i<(source->voc)->nbX-1)
                sor<<Endl;
            else
                sor<<"}";
        }
    }
    else {
        if (lemme!=NULL)
            k++;
        for (i=0;i<k;i++) sor << " ";
        sor << surface << "("<<gauche<<"-"<<droite<<")";
        if (source!=NULL) {
            if (source->couvert==OUI)
                sor<<"+";
            else
                sor<<"-";
            if (lemme!=NULL)
                trcourant=pere->source->traitsbase;                
            else 
                trcourant=source->traitsbase;
            trcourant->TraitsEnChaine(parseur,parseur->Chaine_traits);
			minuscule((uchar*)parseur->Chaine_traits);
			if (trcourant->onto!=NULL) {
				for (i=0;i<trcourant->onto->dernier;i++) {
					strcatchr(parseur->Chaine_traits,',');
					trcourant->onto->cell(i)->ConcateneValeurs(parseur,parseur->hierarchies->lahierarchie,parseur->TraitsFiltreOntologie,parseur->Chaine_traits,",");                    
				}
			}
            sor<<parseur->Chaine_traits;
        }
    }
    
    n=fils;
    while (n!=NULL) {
        n->affiches(parseur,x+1);
        n=n->frere;
    }
}

  

/*-------------------------------------------------------------------------*/
void Liste::affichet(GlobalParseur* parseur,char* Message) {
    Vecteur* v;

    trace << Message << X->nom << ": [";
    v=new Vecteur(traitsbase->valeur);
    if (traitsbase->negation) v->ou(traitsbase->negation);
    v->et(parseur->FILTRE);
    v->affichev(parseur,traitsbase->libre,traitsbase->negation);
    delete v;
    trace << "]\n";
}

/*-------------------------------------------------------------------------*/


Liste::Liste(Cat* x) {
    X=x;
    
	blocfiltrage=NULL;
    position=-1;
    fin=NON;debut=NON;
    N.initialise(X,this);

    traitsactif=NULL;
    traitsbase=NULL;
    instanciateur=NULL;
    aSelectionner=NULL;
    couvert=NON;
    voc=NULL;
    choisie=0;
    index=0;
	iphrase=-1;
}

Liste::~Liste() {

	if (blocfiltrage!=NULL)
		delete blocfiltrage;
    if (traitsactif) 
        delete traitsactif;
    if (traitsbase)
        delete traitsbase;
}

void Liste::initialise(Cat* x,Traits* tr) {
	iphrase=-1;
    debut=NON;
    fin=NON;
    X=x;
    voc=NULL;
    couvert=NON;
    index=x->Val;
    choisie=0;
    variables.raz();
	if (blocfiltrage!=NULL)
		blocfiltrage->raz();
    instanciateur=NULL;
    aSelectionner=NULL;
    traitsactif->razp();
    traitsbase->razp();

    N.initialise(X,this);
    
    if (tr)
        traitsbase->ou(tr);
}

 
void Liste::SauveTraits() {
    traitsactif->recopie(traitsbase);
}

void Liste::RetrouveTraits() {
    traitsbase->recopie(traitsactif);
}

void Liste::StabiliseTraits(char libre) {
    traitsbase->recopie(traitsactif);
    if (libre==NON)
        if (traitsbase->libre!=NULL)
            (traitsbase->libre)->raz();
}


/*-------------------------Application des PL-règles sur traitsbase-----------------------*/

char GlobalParseur::AppPlCouche(int laCouche,LListe& Base,size_t ipremier,size_t idernier) {
    Liste* l;
    Liste* ld;
    Liste* lf=Base[idernier];    
    

    while (ipremier!=idernier) {
        ld=Base[ipremier];
        VECTA<RegleTraits*>& pl=PListes[laCouche];
        for (int i=0;i<pl.dernier && ld->traitsbase;i++) {        
            if (pl[i]->Test(this,ld->traitsbase,NULL,DROIT)) {
                size_t il=ipremier;
                while (il!=idernier) {
                    l=Base[++il];
                    if (l->traitsbase && pl[i]->Test(this,l->traitsbase,NULL,GAUCHE)) {
                        if (TRACETRAITS) {
                            Espace(IEspace);
                            trace<<CHAINES[155];
                            Espace(IEspace);
                            pl[i]->affichePL(this);
                            Espace(IEspace);
                            trace<<" -> "<<(l->X)->nom<< " <=> "<<(lf->X)->nom<<Endl;
                        }
                        return NON;
                    }
                }
            }            
        }
        ipremier++;
    }
    return OUI;
}

char GlobalParseur::AppPl(int laCouche,LListe& Base,size_t ipremier,size_t idernier) {
    if (AppPlCouche(laCouche,Base,ipremier,idernier)) {
        if (laCouche)
            return AppPlCouche(0,Base,ipremier,idernier);
        return OUI;
    }
    return NON;
}

    
/*--------Le Principe des Traits de Tête (Montée des traits libres)--------*/
/* «l» reçoit l'ensemble des traits libres de la liste «liste»...
 Il faut se souvenir donc que pour chaque structure de traits, on sauvegarde un vecteur 
appelé libre... Ce vecteur contient toutes les valeurs possibles pour un attribut, sous une forme 
binaire s'entend...
*/

char Test(int i,TVecteur vecteur) {
    TVecteur v;
    
    v=Un << i;
    return (v==(vecteur & v));
}
   
void affiche(GlobalParseur* parseur,short i,TVecteur v) {
    int k,j,debut;
  
    trace << "[";debut=1;
    for (k=0;k<gBitVect;k++) 
        if (Test(k,v)) {
            if (debut) debut=0;else trace << ",";
            j=(i*gBitVect+k)*2;
            trace << parseur->TableDesTraits[j] << ":" << parseur->TableDesTraits[j+1];
        }
    trace << "]";
}


/*-------------------------------------------------------------------------*/
 /*
 Les regles de montee des traits libres sont les suivantes:
	a) si sur le noeud superieur un trait de meme type est deja present, sa valeur
	doit etre la meme.
	b) Il ne faut pas qu'une negation empeche cette valeur pour ce trait
	c) En cas d'opposition entre la valeur du trait montant et celle deja valuee sur le noeud
	superieure, on verifie qu'il n'existe pas une regle de defaut qui resoudrait l'antagonisme
	pour ces valeurs (exemple: la resolution du genre lors d'une conjonction de SN)
	d)Un trait libre le reste apres remontee sur un noeud superieur, a moins que ce noeud n'est
	une regle de type: [trait:?] pour bloquer toute future remontee de ce trait.
 */

//Version pour les noeuds...
/* 
La signification de suivant est la suivante:
   suivant=false: dans le cas d'une extraction de fonction, la remontee des traits se fait pour chacun des
   arguments de la fonction
   suivant=true: tous les noeuds sont pris en compte dans le processus
*/
void affecteontologielibre(Traits* sommet,Traits* traits,ontologie* libre) {
	if (libre!=NULL && libre->element.dernier!=0 && traits->onto!=NULL) {
		VECTA<ontologie*>& ronto = *traits->onto;
		if (ronto[ONTOLOGIE_COMPARAISON]!=NULL &&
			ronto[ONTOLOGIE_COMPARAISON]->present(libre)) {
			if (sommet->onto==NULL)
				sommet->onto=new VECTA<ontologie*>;
			VECTA<ontologie*>& loconto=*sommet->onto;
			if (loconto[ONTOLOGIE_LIBRE]==NULL)
				loconto.affecte(ONTOLOGIE_LIBRE,new ontologie(libre));
			else
				loconto[ONTOLOGIE_LIBRE]->ou(libre);

			if (loconto[ONTOLOGIE_COMPARAISON]==NULL)
				loconto.affecte(ONTOLOGIE_COMPARAISON,new ontologie(ronto[ONTOLOGIE_COMPARAISON]));                   
			else {
				//dans le cas d'une affectation de valeur,
				//il est important aussi d'eviter de generer les 
				//hyponymes ou hyperonymes
				//est-ce que on a @animal@mammifere@chien et est-ce que on veut rajouter
				//@animal@mammifere                    
				//est-ce que on a @animal@mammifere et est-ce que on veut rajouter
				//@animal@mammifere@chien
				if (ronto[ONTOLOGIE_COMPARAISON]->present(loconto[ONTOLOGIE_COMPARAISON]))                        
					loconto[ONTOLOGIE_COMPARAISON]->ou(ronto[ONTOLOGIE_COMPARAISON]);                    
			}			
		}
	}
}

char GlobalParseur::PTT(Traits& sommet,Traits* traits) {   
    TVecteur Vl;
    TVecteur Vld;
    TVecteur libre;
    short i;

	affecteontologielibre(&sommet,traits,traits->Onto(ONTOLOGIE_LIBRE));

    if (traits && traits->libre) {
        for (i=0;i<iTailleVect;i++) {
            libre=(traits->libre)->vecteur[i] & (traits->filtre)->vecteur[i];
			if (!libre)
				continue;
            Vld= (traits->valeur)->vecteur[i] & libre;
            Vl=(sommet.valeur)->vecteur[i] & libre;
            Vl|=Vld;
            if (sommet.negation) Vl&=~(sommet.negation)->vecteur[i];
            if (Vld && Vld!=Vl) {
                libre=Vld^Vl;
                if (TRACETRAITS)
                    if (libre==(libre & FILTRE->vecteur[i])) {
                        Espace(IEspace);
                        trace << CHAINES[84];
                        affiche(this,i,libre);
                        trace <<Endl;
                    }
                    return NON;
            }
            (sommet.valeur)->vecteur[i]|=Vld;
            (sommet.filtre)->vecteur[i]|= libre;
        }
        
        if (!sommet.libre) 
            sommet.libre=new Vecteur(traits->libre);
        else 
            (sommet.libre)->ou(traits->libre);
        
        if (sommet.bloquage) (sommet.libre)->etnon(sommet.bloquage);
    }    
    return OUI;
}	


char GlobalParseur::PTT(Traits* temporaire,Noeud* premier,Noeud* dernier) {
    TVecteur Vl;
    TVecteur Vld;
    TVecteur libre;
    Traits* tr=temporaire;
    char boucle;
    short i;
    Liste* ld;
    
    if (premier==NULL)
        return NON;

    if (tr==NULL)
        return NON;
	

    boucle=OUI;
    while (boucle) {

        ld=premier->source;
        if (premier==dernier)
            boucle=NON;

		affecteontologielibre(tr,ld->traitsactif,ld->traitsactif->Onto(ONTOLOGIE_LIBRE));

        if ((ld->traitsactif)->libre) {
            for (i=0;i<iTailleVect;i++) {
                libre=((ld->traitsactif)->libre)->vecteur[i] & ((ld->traitsactif)->filtre)->vecteur[i];
				if (!libre)
					continue;
                Vld= ((ld->traitsactif)->valeur)->vecteur[i] & libre;
                Vl=(tr->valeur)->vecteur[i] & libre;
                Vl|=Vld;
                if (tr->negation) Vl&=~(tr->negation)->vecteur[i];
                if (Vld && Vld!=Vl) {
                    libre=Vld^Vl;
                    if (TRACETRAITS)
                        if (libre==(libre & FILTRE->vecteur[i])) {
                            Espace(IEspace);
                            trace << CHAINES[84];
                            affiche(this,i,libre);
                            trace <<Endl;
                        }
                        return NON;
                }
                (tr->valeur)->vecteur[i]|=Vld;
                (tr->filtre)->vecteur[i]|= libre;
            }
	  
            if (!tr->libre) 
                tr->libre=new Vecteur((ld->traitsactif)->libre);
            else 
                (tr->libre)->ou((ld->traitsactif)->libre);

            if (tr->bloquage) (tr->libre)->etnon(tr->bloquage);
        }	

        premier=premier->frere;
    }

    return OUI;
}


//Version pour les listes
char GlobalParseur::PTT(Liste* l,LListe& Base,size_t ipremier,size_t idernier) {
    TVecteur Vl;
    TVecteur Vld;
    TVecteur libre;
    Traits* tr=l->traitsbase;
    char boucle;
    short i;
    size_t iListe=ipremier;
    Liste* ld;
    
  
  
    boucle=OUI;    
    while (boucle) {

        ld=Base[iListe];
        if (iListe==idernier)
            boucle=NON;

		affecteontologielibre(tr,ld->traitsactif,ld->traitsactif->Onto(ONTOLOGIE_LIBRE));

		if ((ld->traitsactif)->libre) {
            for (i=0;i<iTailleVect;i++) {
                libre=((ld->traitsactif)->libre)->vecteur[i] & ((ld->traitsactif)->filtre)->vecteur[i];
				if (!libre)
					continue;
                Vld= ((ld->traitsactif)->valeur)->vecteur[i] & libre;
                Vl=(tr->valeur)->vecteur[i] & libre;
                Vl|=Vld;
                if (tr->negation) Vl&=~(tr->negation)->vecteur[i];
                if (Vld && Vld!=Vl) {
                    libre=Vld^Vl;
					if (TRACETRAITS) {
						if (libre == (libre & FILTRE->vecteur[i])) {
							Espace(IEspace);
							trace << CHAINES[84];
							affiche(this, i, libre);
							trace << Endl;
						}
					}
					//Dans ce cas, nous sommes en train de faire remonter des traits sur le noeud TOP
					//par conséquent, une erreur ne doit pas être considérée comme définitive.
					if (l != LINTERVICTOIRE)
						return NON;
                }
				else {
					(tr->valeur)->vecteur[i] |= Vld;
					(tr->filtre)->vecteur[i] |= libre;
				}
            }
            
            if (!tr->libre)
                tr->libre=new Vecteur((ld->traitsactif)->libre);
            else 
                (tr->libre)->ou((ld->traitsactif)->libre);
     
            //(tr->filtre)->ou((ld->traits)->libre & (ld->traits)->filtre);
            if (tr->bloquage) (tr->libre)->etnon(tr->bloquage);
        }	
        iListe++;
    }
  
    return OUI;
}

/*-------------------------Application des V-règles sur traitsbase-----------------------*/
/*
 S'il s'agit d'une RCT, type=RCT=0 ou type=PostRCT=1.
 Dans ce cas-là, on n'enregistre pas la partie droite de la Vrègle. On ne fait que tester
 pour vérifier la cohérence des traits testés avec la partie droite de la VRègle.
 En revanche s'il s'agit d'une STD, là on n'enregistre les traits de la partie droite.
*/

char GlobalParseur::AppVregle(Traits* traits,Cat* cat,short type) {
    VECTA<RegleTraits*>& vregle=Vregle[type];
    
    for (int i=0;i<vregle.dernier;i++) {
        if (vregle[i]->Test(this,traits,cat,GAUCHE)) {
            if (type<=PostRCT) {
                if (!vregle[i]->Test(this,traits,NULL,DROIT)) {
                    if (TRACETRAITS) {
                        Espace(IEspace);
                        trace << CHAINES[144];
                        vregle[i]->affiche(this);
                        Espace(IEspace);
                        trace << CHAINES[145];(traits->valeur)->affichev(this,traits->libre,NULL);
                        trace << "]\n";				
                    }
                    return NON;
                }
            }
            else                
                ITR(this,vregle[i]->affecte,traits,OUI);            
        }        
    }
    return OUI;
}

/*-------------------------------------------------------------------------*/
void GlobalParseur::InstancieContexte(LListe& Base,int ipremier,int idernier,char tout) {
    //On initialise la liste des traits uniquement en final...
    //Si la regle a pu completement passer.
    for (int ia=ipremier;ia<=idernier;ia++) {
        Liste* Ld=Base[ia];
        //puis on initialise avec les nouvelles valeurs
        if (Ld->instanciateur != NULL) {
            InstancieTraits(Ld->traitsactif,Ld->instanciateur);
            if (tout==OUI)
                InstancieTraits(Ld->traitsbase,Ld->instanciateur);
        }
        Ld->instanciateur=NULL;
    }
}

/*-------------------------------------------------------------------------*/
//Ce test est utilise pour tester des traits ttr soit sur les traits globaux, soit
//a selectionner une entree.
inline char TestChaineTraits(GlobalParseur* parseur,char testLecture,Traits* ttr,Liste* Ld) {

    if (testLecture==NON || Ld->voc==NULL)
        return ITR(parseur,ttr,Ld->traitsbase,ECRIT,&Ld->N);
    
    //On teste quelle lecture doit etre selectionnee en tete
    ListeVoc* voc=Ld->voc;
    int choix=Ld->choisie;
    
    for (int vok=0;vok<voc->nbX;vok++) {
        Ld->choixCategorie(parseur,vok);
        if (ITR(parseur,ttr,voc->tr[vok],ECRIT))            
            return OUI;
    }            
    Ld->choixCategorie(parseur,choix);

    return NON;
}

/* Cette procédure permet d'initialiser chacun des éléments de la liste allant de ld à lf avec 
les traits issus d'une règle... Puis l'élément pointé par l (le nouveau père de l'arbre qui se
construit) reçoit les traits libres issus de [ld..lf]...*/

char GlobalParseur::InitTraits(Liste* ltete,LListe& Base,size_t ipremier,size_t idernier,
                               ChaineTraits* r_traits, ChaineTraits* tete_traits,
                               int nbTraits,VECTA<Noeud*>* vect_noeuds) {
    ChaineTraits* tr;
    Liste* Ld;
    Liste* ldernier=NULL;
    Liste* lpremier=NULL;
    short nb;
    short boucle;
    Vecteur* v;
  
    size_t iListe=ipremier;
  
    //On initialise chacun des noeuds [ld..lf] avec les traits issus de la regle choisie...
    if (r_traits) {
        nb=nbTraits;
        r_traits->initialise();
        boucle=OUI;
        while (boucle) {

            Ld=Base[iListe];
            
            if (iListe==idernier)
                boucle=NON;

            tr=r_traits;
            while (tr && tr->val() != Ld->X->val)
                tr=tr->suivant; //On recherche la premiere categorie correspondante

            if (TRACETRAITS) {
                Espace(IEspace);
                Ld->affichet(this,CHAINES[161]);
            }

            while (tr) { //on boucle sur l'ensemble des traits d'une regle donnee...                

                Traits* ttr=tr->traits;

                if (TRACETRAITS) { //Juste une trace...
                    Espace(IEspace);
                    trace<<CHAINES[163];
                    trace<<(tr->X)->nom<<"[";
                    v=new Vecteur(ttr->valeur);
                    if (ttr->negation) 
                        v->ou(ttr->negation);
                    v->et(FILTRE);
                    v->affichev(this,ttr->libre,ttr->negation);
                    delete v;
                    trace<<"]"<<Endl;
                }
                //si ce trait n'a jamais ete utilise (ou que l'on boucle dessus)
                if (!tr->dejaLu || tr->etoile) {
                    //Ld->SauveTraits(); //on teste sa coherence avec les traits du noeud Ld...
                    Ld->instanciateur=NULL;
                    //s'il s'agit du debut de la sequence, on ajoute le trait debut...
                    if (Ld->debut) {   
                        MetDebut(Ld->traitsbase);
                        lpremier=Ld;
                    }
                    //s'il s'agit de la fin de la sequence, on ajoute le trait fin...
                    if (Ld->fin) {  
                        MetFin(Ld->traitsbase);
                        ldernier=Ld;
                    }


                    if (TestChaineTraits(this,tr->testLecture,ttr,Ld)) {
                        if (!tr->dejaLu) //nb contient le nombre total d'element dans la regle...
                            nb--; 
                        //On verifie que tous les elements ont ete utilise, 
                        //en verifiant qu'il vaut 0 a la fin
                        //Dans le cas d'une boucle, on ne le decremente qu'une fois...
                        //On sauvegarde l'ancienne structure de traits
                        Ld->instanciateur=ttr;
                        tr->dejaLu=OUI; 
                        //s'il y a une variable associee
                        if (tr->arg!=-1 && vect_noeuds!=NULL)
                            vect_noeuds->affecte(tr->arg,&Ld->N);
                        break;
                    }
                    else {
                           //s'il s'agit du debut de la sequence, on retire le trait debut...
                        if (Ld->debut) {
                            RetireDebut(Ld->traitsbase);
                        }
                        //s'il s'agit de la fin de la sequence, on retire le trait fin...
                        if (Ld->fin) {
                            RetireFin(Ld->traitsbase);
                        }
                    }
                    if (TRACETRAITS) {
                        Espace(IEspace);
                        trace<<CHAINES[162]<<Endl;
                    }
                }

                tr=tr->autre;
            }

            if (!tr) {
                if (lpremier!=NULL) {
                    RetireDebut(lpremier->traitsbase);
                }
                if (ldernier!=NULL) {
                    RetireFin(ldernier->traitsbase);
                }
                return NON; //intermediaire et on revient...
            }

            iListe++; //pour chacun des noeuds en attente dans la liste...
        }

        if (nb) {
            if (lpremier!=NULL) {
                RetireDebut(lpremier->traitsbase);
            }
            if (ldernier!=NULL) {
                RetireFin(ldernier->traitsbase);
            }
            return NON;                
        }
    }
    else {
        //si la remontee des traits n'a pas lieu, on marque quand meme 
        //les noeuds avec les traits first et last
        MetDebut(Base[ipremier]->traitsbase);
        lpremier=Base[ipremier];
		MetFin(Base[idernier]->traitsbase);
		ldernier=Base[idernier];
    }

    //On initialise la liste des traits uniquement en final...
    //Si la regle a pu completement passer.
    for (size_t ia=ipremier;ia<=idernier;ia++) {
        Ld=Base[ia];
        Ld->SauveTraits();

		if (Ld==lpremier) {  //s'il s'agit du debut de la sequence, on retire le trait debut...
            RetireDebut(Ld->traitsbase);
		}
        
		if (Ld==ldernier) {   //s'il s'agit de la fin de la sequence, on retire le trait fin...
            RetireFin(Ld->traitsbase);
		}

        //puis on initialise avec les nouvelles valeurs
        if (Ld->instanciateur != NULL)
            InstancieTraits(Ld->traitsactif,Ld->instanciateur);
		Ld->instanciateur=NULL;
    }
    

    //Les traits de tete de la regle sont transmis au futur noeud pere...
    if (ltete->traitsbase!=NULL)
        (ltete->traitsbase)->recopie(tete_traits->traits);
    else
        ltete->traitsbase=new Traits(tete_traits->traits);


    //On applique les Vregles sur ce nouveau noeud pere...
    if (!AppVregle(ltete->traitsbase,ltete->X,RCT)) {
        return NON;
    }
    AppVregle(ltete->traitsbase,ltete->X,STD);


 /* Remontée des traits libres vers la tête...*/
    if (!PTT(ltete,Base,ipremier,idernier)) {
        return NON;
    }
    
	ltete->SauveTraits();
	return OUI;
}
  

/*-------------------------------------------------------------------------*/





