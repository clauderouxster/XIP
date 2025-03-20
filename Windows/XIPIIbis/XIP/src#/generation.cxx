/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.64
   filename   : generation.cxx
   Date       : 05/03/2009
   Purpose    : Generation
   Programmer : Claude ROUX
   Reviewer   : 
*/
#ifdef NTMCONNECTION

extern "C" {
#include "fst/context.h"
#include "fst/cfsm.h"
#include "applyloc.h"
}


#include "fst/modify.h"
#include "fst/bin-in.h"
#include "fst/pars-tbl.h"
#include "lktypes.h"
#include "ems.h"
#include "lookup.h"
    
#include "networks.h"

#include "tok-aux.h"

#include <iostream>

using namespace std;
#include "xipvecta.h"

#include "parsbin.h"
#include "ntmhmm.h"
#include "maintok.h"
#include "versiontok.h"
#include "generation.h"
#include "x_node.h"

char* TV(GlobalParseur* parseur,Vecteur* v);
char* TC(GlobalParseur* parseur,Traits* tr);

static map<string,string> lamajuscule;
static map<string,string> lamajusculeutf8;

void initlamajuscule() {
	static char init=NON;
	if (init==OUI)
		return;
	init=OUI;
	lamajuscule["a"]="A";
	lamajuscule["b"]="B";
	lamajuscule["c"]="C";
	lamajuscule["d"]="D";
	lamajuscule["e"]="E";
	lamajuscule["f"]="F";
	lamajuscule["g"]="G";
	lamajuscule["h"]="H";
	lamajuscule["i"]="I";
	lamajuscule["j"]="J";
	lamajuscule["k"]="K";
	lamajuscule["l"]="L";
	lamajuscule["m"]="M";
	lamajuscule["n"]="N";
	lamajuscule["o"]="O";
	lamajuscule["p"]="P";
	lamajuscule["q"]="Q";
	lamajuscule["r"]="R";
	lamajuscule["s"]="S";
	lamajuscule["t"]="T";
	lamajuscule["u"]="U";
	lamajuscule["v"]="V";
	lamajuscule["w"]="W";
	lamajuscule["x"]="X";
	lamajuscule["y"]="Y";
	lamajuscule["z"]="Z";
	lamajuscule["à"]="À";
	lamajuscule["á"]="Á";
	lamajuscule["â"]="Â";
	lamajuscule["ã"]="Ã";
	lamajuscule["ä"]="Ä";
	lamajuscule["å"]="Å";
	lamajuscule["ç"]="Ç";
	lamajuscule["è"]="È";
	lamajuscule["é"]="É";
	lamajuscule["ê"]="Ê";
	lamajuscule["ë"]="Ë";
	lamajuscule["ì"]="Ì";
	lamajuscule["í"]="Í";
	lamajuscule["î"]="Î";
	lamajuscule["ï"]="Ï";
	lamajuscule["ò"]="Ò";
	lamajuscule["ó"]="Ó";
	lamajuscule["ô"]="Ô";
	lamajuscule["õ"]="Õ";
	lamajuscule["ö"]="Ö";
	lamajuscule["ù"]="Ù";
	lamajuscule["ú"]="Ú";
	lamajuscule["û"]="Û";
	lamajuscule["ü"]="Ü";
	lamajuscule["æ"]="Æ";
	lamajuscule["ð"]="Ð";
	lamajuscule["ñ"]="Ñ";
	lamajusculeutf8["a"]="A";
	lamajusculeutf8["b"]="B";
	lamajusculeutf8["c"]="C";
	lamajusculeutf8["d"]="D";
	lamajusculeutf8["e"]="E";
	lamajusculeutf8["f"]="F";
	lamajusculeutf8["g"]="G";
	lamajusculeutf8["h"]="H";
	lamajusculeutf8["i"]="I";
	lamajusculeutf8["j"]="J";
	lamajusculeutf8["k"]="K";
	lamajusculeutf8["l"]="L";
	lamajusculeutf8["m"]="M";
	lamajusculeutf8["n"]="N";
	lamajusculeutf8["o"]="O";
	lamajusculeutf8["p"]="P";
	lamajusculeutf8["q"]="Q";
	lamajusculeutf8["r"]="R";
	lamajusculeutf8["s"]="S";
	lamajusculeutf8["t"]="T";
	lamajusculeutf8["u"]="U";
	lamajusculeutf8["v"]="V";
	lamajusculeutf8["w"]="W";
	lamajusculeutf8["x"]="X";
	lamajusculeutf8["y"]="Y";
	lamajusculeutf8["z"]="Z";
	lamajusculeutf8["Ã "]="Ã€";
	lamajusculeutf8["Ã¡"]="Ã";
	lamajusculeutf8["Ã¢"]="Ã‚";
	lamajusculeutf8["Ã£"]="Ãƒ";
	lamajusculeutf8["Ã¤"]="Ã„";
	lamajusculeutf8["Ã¥"]="Ã…";
	lamajusculeutf8["Ã§"]="Ã‡";
	lamajusculeutf8["Ã¨"]="Ãˆ";
	lamajusculeutf8["Ã©"]="Ã‰";
	lamajusculeutf8["Ãª"]="ÃŠ";
	lamajusculeutf8["Ã«"]="Ã‹";
	lamajusculeutf8["Ã¬"]="ÃŒ";
	lamajusculeutf8["Ã­"]="Ã";
	lamajusculeutf8["Ã®"]="ÃŽ";
	lamajusculeutf8["Ã¯"]="Ã";
	lamajusculeutf8["Ã²"]="Ã’";
	lamajusculeutf8["Ã³"]="Ã“";
	lamajusculeutf8["Ã´"]="Ã”";
	lamajusculeutf8["Ãµ"]="Ã•";
	lamajusculeutf8["Ã¶"]="Ã–";
	lamajusculeutf8["Ã¹"]="Ã™";
	lamajusculeutf8["Ãº"]="Ãš";
	lamajusculeutf8["Ã»"]="Ã›";
	lamajusculeutf8["Ã¼"]="Ãœ";
	lamajusculeutf8["Ã¦"]="Ã†";
	lamajusculeutf8["Ã°"]="Ã";
	lamajusculeutf8["Ã±"]="Ã‘";
}


void GlobalParseur::RenvoieMaj(string& s) {
	string pr;
	string prm;
	if (encodage_fst==UTF8_ENCODING) {
		pr=s.substr(0,2);
		prm=lamajusculeutf8[pr];
		if (prm=="") {
			pr=s.substr(0,1);
			prm=lamajusculeutf8[pr];
			s.replace(0,1,prm);
		}
		else
			s.replace(0,2,prm);
	}
	else {
		pr=s.substr(0,1);
		prm=lamajuscule[pr];
		if (prm!="")
			s.replace(0,1,prm);
	}
}

void genereGraphe::profondeur(int p,int n) {
	if (noeud!=NULL) {
		noeud->profondeur=p;
		noeud->numero_ordre=n;
	}
	for (int i=0;i<enfants.dernier;i++)
		enfants[i]->profondeur(i+1,i);
}

genereGraphe* genereGraphe::recupere(ResultatFonction* r) {
	genereGraphe* g=r->frere;
	if (g==NULL)
		g=new genereGraphe(r);
	else {
		//On garde la position de ce noeud avant son deplacement
		g->parentancien=g->parent;
		g->posancien=g->pos;
		//On le retire de son ancienne position
		if (g->pos!=-1 && g->parent!=NULL) {
			g->parent->enfants.retireElement(g->pos);
			for (int i=0;i<g->parent->enfants.dernier;i++)
				g->parent->enfants[i]->pos=i;
		}
	}
	g->parent=this;
	return g;
}

void genereGraphe::ajoute(genereGraphe* g,int p) {
	g->pos=p;
	enfants.insere(p,g);
	g->parent=this;
}

void genereGraphe::retire(int p) {
	parent->enfants.retireElement(p);
	if (parentancien==NULL)
		return;

	parentancien->enfants.insertion(posancien,this);
	pos=posancien;
	parentancien=NULL;
	posancien=-1;	
}


//Cette fonction retire un element de la structure genereGraphe
//Elle retourne NON, si l'element ne doit pas etre detruit
//OUI si il doit etre detruit
char genereGraphe::retire() {
	if (parent==NULL)
		return NON;
	
	parent->enfants.retireElement(pos);
	
	for (int i=0;i<parent->enfants.dernier;i++)
		parent->enfants[i]->pos=i;
	//On le replace a sa place
	if (parentancien==NULL)
		return OUI;
	
	parentancien->enfants.insertion(posancien,this);
	pos=posancien;
	parentancien=NULL;
	posancien=-1;
	return NON;
}

genereGraphe::genereGraphe(ResultatFonction* r) {
	maj=NON;
	tokenpos=-1;
	noeud=r;
	mot="";
	formecalcule=NON;
	if (r!=NULL)
		r->frere=this;
	pos=-1;
	parent=NULL;
	parentancien=NULL;
	posancien=-1;
}

 genereGraphe::~genereGraphe() {
	 if (noeud!=NULL)
		 noeud->frere=NULL;
	 enfants.raz();
 }

 void parcourspourdestruction(genereGraphe* g,VECTA<genereGraphe*>& gs) {
	int i;
	gs.ajoute(g);
	for (i=0;i<g->enfants.dernier;i++)
		parcourspourdestruction(g->enfants[i],gs);
 }


void genereGraphe::nettoie() {
	int i;
	VECTA<genereGraphe*> gs;
	for (i=0;i<enfants.dernier;i++)
		parcourspourdestruction(enfants[i],gs);
	for (i=0;i<gs.dernier;i++)
		delete gs[i];
	enfants.raz();
}

void genereGraphe::affiche(ostream& os) {
	if (noeud==NULL)
		os<<"GEN_TREE";
	else
		noeud->Affiche(os,3);
	if (enfants.dernier!=0) {
		os<<"{";
		for (int i=0;i<enfants.dernier;i++) {
			if (i)
				os<<",";
			enfants[i]->affiche(os);
		}
		os<<"}";
	}
}

char parentde(ResultatFonction* fille,ResultatFonction* parent) {
	genereGraphe* gfille=fille->frere;
	if (gfille==NULL || parent->frere==NULL)
		return NON;
	while (gfille!=NULL) {
		if (gfille==parent->frere)
			return OUI;
		gfille=gfille->parent;
	}
	return NON;
}

ResultatFonction* suivantde(ResultatFonction* courant,ResultatFonction* suivant,int increment) {
	genereGraphe* gcourant=courant->frere;
	if (gcourant==NULL || gcourant->parent==NULL)
		return NULL;

	genereGraphe* lesuivant=gcourant->parent->enfants[gcourant->pos+increment];	
	if (suivant==NULL) {
		if (lesuivant!=NULL)
			return lesuivant->noeud;
		return NULL;
	}

	if (suivant->frere==lesuivant)
		return suivant;
	return NULL;
}

ResultatFonction* perede(ResultatFonction* courant,ResultatFonction* suivant) {
	genereGraphe* gcourant=courant->frere;
	if (gcourant==NULL || gcourant->parent==NULL)
		return NULL;

	genereGraphe* lesuivant=gcourant->parent;	
	if (suivant==NULL) {
		if (lesuivant!=NULL)
			return lesuivant->noeud;
		return NULL;
	}

	if (suivant->frere==lesuivant)
		return suivant;
	return NULL;
}

ResultatFonction* filsde(ResultatFonction* courant,ResultatFonction* suivant,char dernier) {
	genereGraphe* gcourant=courant->frere;
	if (gcourant==NULL || gcourant->parent==NULL)
		return NULL;


	genereGraphe* lesuivant;
	if (dernier==OUI)
		lesuivant=gcourant->enfants.fin();		
	else
		lesuivant=gcourant->enfants[0];		
	if (suivant==NULL) {
		if (lesuivant!=NULL)
			return lesuivant->noeud;
		return NULL;
	}

	if (suivant->frere==lesuivant)
		return suivant;
	return NULL;
}


void GlobalParseur::parcoursrecursif(genereGraphe* g) {
	if (g->enfants.dernier==0) {
		g->tokenpos=lestokens.dernier;
		lestokens.ajoute(g);
	}
	else {
		for (int i=0;i<g->enfants.dernier;i++)
			parcoursrecursif(g->enfants[i]);
	}
}

void GlobalParseur::parcoursrecursifinit(genereGraphe* g,char& premier) {
	if (g->enfants.dernier==0) {
		g->tokenpos=lestokens.dernier;
		ConstruitMotGraphe(g,premier);
		lestokens.ajoute(g);
	}
	else {
		for (int i=0;i<g->enfants.dernier;i++)
			parcoursrecursifinit(g->enfants[i],premier);
	}
}

int GlobalParseur::ListeTokens(VECTA<automate*>& expressions) {	
	char premier=OUI;
	lestokens.raz();
	parcoursrecursifinit(&graphegeneration,premier);
	int i,j,nb=0;	
	
	listedetokens.nettoie();
	//Nous explorons alors, l'ensemble de nos expressions par rapport a "lestokens"
	for (j=0;j<lestokens.dernier;j++) {
		if (expressions[0]->recherche(lestokens[j]->mot)!=NULL) {
			char trouve=OUI;
			for (i=1;i<expressions.dernier;i++) {
				if ((j+i)>=lestokens.dernier) {
					trouve=NON;
					break;
				}
				if (expressions[i]->recherche(lestokens[j+i]->mot)==NULL) {
					trouve=NON;
					break;
				}
			}
			if (trouve==OUI) {
				listedetokens.ajoute(new VECTA<genereGraphe*>);
				for (i=0;i<expressions.dernier;i++)
					listedetokens.fin()->ajoute(lestokens[i+j]);
				nb++;
			}
		}
	}
	return nb;
}

ResultatFonction* tokensuivant(GlobalParseur* parseur,ResultatFonction* courant,
							   ResultatFonction* suivant,int pos) {
	genereGraphe* gcourant=courant->frere;
	if (gcourant==NULL || gcourant->parent==NULL)
		return NULL;

	parseur->lestokens.raz();
	parseur->parcoursrecursif(&parseur->graphegeneration);
	
	int idx;
	if (gcourant->tokenpos==-1) {
		//Nous ne sommes pas sur un token, il nous faut descendre pour
		//trouver le bon
		genereGraphe* next=gcourant->enfants.fin();
		while (next!=NULL && next->tokenpos==-1)
			next=next->enfants.fin();
		//On va chercher systematiquement le dernier a droite
		//Si on ne le trouve pas, c'est une erreur
		if (next==NULL)
			return NULL;
		idx=next->tokenpos+pos;
	}
	else
		idx=gcourant->tokenpos+pos;

	genereGraphe* lesuivant=parseur->lestokens[idx];

	if (suivant==NULL) {
		if (lesuivant!=NULL)
			return lesuivant->noeud;
		return NULL;
	}

	if (suivant->frere==lesuivant)
		return suivant;
	return NULL;
}

ResultatFonction* GlobalParseur::tokendufond(ResultatFonction* courant,ResultatFonction* suivant,char dernier) {
	genereGraphe* gcourant=courant->frere;
	if (gcourant==NULL || gcourant->parent==NULL)
		return NULL;

	lestokens.raz();
	parcoursrecursif(gcourant);
	
	genereGraphe* lesuivant;
	if (dernier==NON)
		lesuivant=lestokens[0];
	else
		lesuivant=lestokens.fin();

	if (suivant==NULL) {
		if (lesuivant!=NULL)
			return lesuivant->noeud;
		return NULL;
	}

	if (suivant->frere==lesuivant)
		return suivant;
	return NULL;
}

void GlobalParseur::ConstruitMotGraphe(genereGraphe* g,char& premier) {
	if (g->noeud==NULL) {
		g->formecalcule=OUI;
		return;
	}
	if (g->formecalcule==NON) {
		char lem=NON;
		if (g->noeud->Fx==Xcreationtoken) 
			lem=construitchainegenere(g,g->noeud->arg[0]->source,g->mot);
		else {
			if (g->noeud->Fx==Xsurfacetoken) {
				g->noeud->arg[0]->Surface(g->mot);
				lem=OUI;
			}
		}

		if (lem==NON && g->noeud->arg.dernier!=0)
			g->noeud->arg[0]->Lemme(g->mot);

		g->formecalcule=OUI;
	}
	g->maj=NON;
	if (premier==OUI) {
		premier=NON;
		g->maj=OUI;
	}
}

void GlobalParseur::parcourssousnoeud(genereGraphe* g,string& phrase,char& premier) {

	if (g->noeud!=NULL && 
		(g->noeud->Fx==Xcreationtoken || g->noeud->Fx==Xsurfacetoken || g->enfants.dernier==0)) {
		ConstruitMotGraphe(g,premier);				
		if (g->mot!="") {
			phrase+=g->mot;
			if (g->maj==OUI)
				RenvoieMaj(phrase);
			phrase+=" ";
		}
	}
	else {
		for (int i=0;i<g->enfants.dernier;i++)
			parcourssousnoeud(g->enfants[i],phrase,premier);
	}
}


void GlobalParseur::parcoursgeneration(string& s) {
	char premier=OUI;
	for (int i=0;i<graphegeneration.enfants.dernier;i++)
		parcourssousnoeud(graphegeneration.enfants[i],s,premier);

	ostream* os=&cout;
    if (OS!=NULL)
        os=OS;	
	graphegeneration.affiche(*os);
	*os<<Endl;
}

char GlobalParseur::initgenere(Liste* L,string& lestraits) {
	if (tokdown==NULL)
		return NON;

	if (L==NULL || L->voc==NULL)
		return NON;
	for (int i=0;i<LesTraitsdeGenerations.dernier;i++) {
		genere_traits* g=LesTraitsdeGenerations[i];
		if (g->X!=NULL && g->X!=L->X)
			continue;
		if (g->tr!=NULL && !ITR(this,g->tr,L->traitsbase,NONECRIT))
			continue;

		string wrd=L->voc->lemme[L->choisie];
		if (lestraits=="")
			wrd+=g->lestraits;
		else
			wrd+=lestraits;
		tokdown->down((char*)wrd.c_str());	
		if (lectures.dernier) {
			char* res=lectures[0]->mot+wrd.size()+1;
			if (res[0]!='?') {
				InstancieTraits(L->traitsbase,g->tr);
				L->SauveTraits();
				nettoieLectures();
				return OUI;
			}
		}
		nettoieLectures();
		if (lestraits!="")
			break;
	}
	return NON;
}

char GlobalParseur::lecturemontante(string& wrd,VECTA<string*>& reponse) {
	if (tokdown==NULL)
		return NON;
	tokdown->up((char*)wrd.c_str());	
	if (lectures.dernier) {
		int sz=wrd.size()+1;
		char* res=lectures[0]->mot+sz;
		if (res[0]!='?') {
			for (int i=0;i<lectures.dernier;i++) {
				string* s=new string(lectures[i]->mot+sz);
				reponse.ajoute(s);
			}
			nettoieLectures();
			return OUI;
		}
	}
	nettoieLectures();
	return NON;
}

char GlobalParseur::lecturedescendante(string& wrd,string& feat,string& reponse) {
	if (tokdown==NULL)
		return NON;
	string w=wrd+feat;
	tokdown->down((char*)w.c_str());	
	if (lectures.dernier) {
		char* res=lectures[0]->mot+wrd.size()+1;
		if (res[0]!='?') {
			reponse=res;
			nettoieLectures();
			return OUI;
		}
	}
	nettoieLectures();
	return NON;
}

char GlobalParseur::construitchainegenere(genereGraphe* graphe,Liste* L,string& s) {
	if (tokdown==NULL)
		return NON;
	if (L==NULL || L->voc==NULL)
		return NON;
	for (int i=0;i<LesTraitsdeGenerations.dernier;i++) {
		genere_traits* g=LesTraitsdeGenerations[i];
		if (g->X!=NULL && g->X!=L->X)
			continue;

		char trouve=NON;

		//dans le cas ou TOKEN porte des traits, on verifie avec ceux-la en priorite
		if (graphe->noeud!=NULL && !graphe->noeud->traits->vide()) {
			if (ITR(this,g->tr,graphe->noeud->traits,NONECRIT))
				trouve=OUI;
		}
		
		if (trouve==NON && 
			g->tr!=NULL && 
			!ITR(this,g->tr,L->traitsbase,NONECRIT))
			continue;

		string wrd=L->voc->lemme[L->choisie];
		wrd+=g->lestraits;
		tokdown->down((char*)wrd.c_str());	
		if (lectures.dernier) {
			char* res=lectures[0]->mot+wrd.size()+1;
			if (res[0]!='?') {
				s=res;
				InstancieTraits(L->traitsbase,g->tr);
				L->SauveTraits();
				nettoieLectures();
				return OUI;
			}
		}
		nettoieLectures();
	}
	return NON;
}

char GlobalParseur::comparecat(Cat* X,Cat* Y) {
	if (Y==XCatVide)
		return OUI;
	if (X==Y)
		return OUI;
	return NON;
}

//Test en cas de presence d'un argument sur le noeud
char GlobalParseur::TestArgNodeGauche(OrdreNoeuds* ord,ResultatFonction* r,VECTA<Noeud*>& args) {
	if (ord->argsgauche.dernier==0)
		return OUI;

	//Il faut que l'arite soit la meme
	if (ord->argsgauche.dernier!=r->nb_args)
		return NON;

	//Sinon, on parcourt nos arguments
	for (int i=0;i<ord->argsgauche.dernier;i++) {
		//qui doivent evidemment correspondre aux arguments de r
		int num_noeud=ord->argsgauche[i];
		//Si ce n'est pas le meme noeud deja rencontre, on part en erreur
		if (args[num_noeud]!=NULL && args[num_noeud]!=r->arg[i])
			return NON;
		//On conserve la trace de notre noeud
		args.affecte(num_noeud,r->arg[i]);
		//On teste alors les traits
		if (!ITR(this,ord->argtraitsgauche[i],r->arg[i]->lesTraits(),NONECRIT))
			return NON;
	}
	
	return OUI;
}

//Test en cas de presence d'un argument sur le noeud
char GlobalParseur::TestArgNodeDroit(OrdreNoeuds* ord,ResultatFonction* r,VECTA<Noeud*>& args) {
	if (ord->argsdroit.dernier==0)
		return OUI;

	//Il faut que l'arite soit la meme
	if (ord->argsdroit.dernier!=r->nb_args)
		return NON;

	//Sinon, on parcourt nos arguments
	for (int i=0;i<ord->argsdroit.dernier;i++) {
		//qui doivent evidemment correspondre aux arguments de r
		int num_noeud=ord->argsdroit[i];
		//Si ce n'est pas le meme noeud deja rencontre, on part en erreur
		if (args[num_noeud]!=NULL && args[num_noeud]!=r->arg[i])
			return NON;
		//On teste alors les traits
		if (!ITR(this,ord->argtraitsdroit[i],r->arg[i]->lesTraits(),NONECRIT))
			return NON;
	}
	
	return OUI;
}

char GlobalParseur::VerifieOrdre(VECTA<OrdreNoeuds*>& liste,VECTA<genereGraphe*>& noeuds) {
	//Nous parcourons toutes les regles...
	VECTA<Noeud*> args;
	for (int l=0;l<liste.dernier;l++) {
		//Puis nous parcourons tous les noeuds dans noeuds susceptibles de correspondre a Xgauche[trgauche]
		for (int n=0;n<noeuds.dernier;n++) {			
			args.raz();
			if (comparecat(noeuds[n]->noeud->Fx,liste[l]->Xgauche) &&
				ITR(this,liste[l]->Tgauche,noeuds[n]->noeud->traits,NONECRIT) &&
				TestArgNodeGauche(liste[l],noeuds[n]->noeud,args)) {
				//Puis on reparcourt la liste et on verifie qu'aucun element ne correspond a Xdroit qui ne soit avant
				for (int d=0;d<noeuds.dernier;d++) {
					if (d==n)
						continue;
					if (comparecat(noeuds[d]->noeud->Fx,liste[l]->Xdroit) &&
						ITR(this,liste[l]->Tdroit,noeuds[d]->noeud->traits,NONECRIT) &&
						TestArgNodeDroit(liste[l],noeuds[d]->noeud,args)) {
							if (n>d)
								return NON;
					}
				}
			}
		}
	}
	return OUI;
}

//On les detruit a l'envers. En effet, certains elements peuvent etre le fils d'autres elements
//au-dessus...
void GlobalParseur::DestructionNodes(VECTA<ResultatFonction*>& creees) {
	for (int i=creees.dernier-1;i>=0;i--)
		destructiondependance(creees[i]);
}


char GlobalParseur::TestSimpleOrdreNoeuds(int couche,VECTA<genereGraphe*>& creees) {
	if (creees.dernier==0)
		return OUI;

	if (VerifieOrdre(ListeOrdreNoeuds[couche],creees)==NON)
		return NON;
	if (VerifieOrdre(ListeOrdreNoeuds[0],creees)==NON)
		return NON;

	return OUI;
}

void GlobalParseur::recuperenom(int& iarg,x_node* x,TestFonctionDependance* ntf,VECTA<char>& variables_utilisees) {
	Categorie* c=CatVide;

	if (x->token=="namedep") {
		if (x->value=="?")
			ntf->vardep=1;
	}
	
	if (x->token=="name") {
		c=ChercheFonc((char*)x->value.c_str());
		ntf->Fx=&c->X;
	}
	
	if (x->token=="vardepnode") {
		if (x->nodes[0]->token=="pointeur") {
			ntf->pointeur=OUI;
			ntf->vardep=atoi((char*)x->nodes[1]->value.c_str())+1;
		}
		else
			ntf->vardep=atoi((char*)x->nodes[0]->value.c_str())+1;
		return;
	}
	
	if (x->token=="pos") {
		ntf->arg.affecte(iarg,-1);
		iarg++;
		return;
	}
	
	if (x->token=="var") {
		ntf->arg.affecte(iarg,atoi((char*)x->nodes[0]->value.c_str()));
		variables_utilisees.affecte(ntf->arg[iarg],OUI);
		iarg++;
		return;
	}

	if (x->token=="nodefeatures") {
		istringstream traits(x->value.substr(1,x->value.size()-1));
		Traits* tr=BatitTraits(traits,']');
		ntf->traits.affecte(iarg-1,FactoriseTraits(tr));
		return;
	}
		
	if (x->token=="features") {
		istringstream traits(x->value.substr(1,x->value.size()-1));
		Traits* tr=BatitTraits(traits,']');
		ntf->traitsFonction=FactoriseTraits(tr);
		return;
	}
	
	for (int i=0;i<x->nodes.size();i++)
		recuperenom(iarg,x->nodes[i],ntf,variables_utilisees);
}

//Parcours recursif d'un x_node
void GlobalParseur::parcoursxnode(TestFonctionDependance* tf,x_node* x,VECTA<char>& variables_utilisees) {

	int i;
	if (x->token=="subdependency") {
		for (i=0;i<x->nodes.size();i++)
			parcoursxnode(tf->fils.fin(),x->nodes[i],variables_utilisees);
		return;
	}

	if (x->token=="dependency") {
		TestFonctionDependance* ntf=new TestFonctionDependance(XCatVide);
		StockeTestFonction((TestFonction*)ntf);
		ntf->typecat=C_GENERATION;
		ntf->leType=TESTNOEUDGENERATION;
		tf->fils.ajoute(ntf);
		int iarg=0;
		recuperenom(iarg,x,ntf,variables_utilisees);
		return;
	}
	
	for (i=0;i<x->nodes.size();i++)
		parcoursxnode(tf,x->nodes[i],variables_utilisees);

}

void initfinalTestFonctionDependance(TestFonctionDependance* e,char negation) {
	if (e->fils.dernier!=0)
		initfinalTestFonctionDependance(e->fils.fin(),negation);
	else
		e->final=1+negation;		
}

TestFonction* GlobalParseur::construitarbregenere(TestFonction* tinit,x_node* x,VECTA<char>& variables_utilisees,char negation) {
	if (tinit==NULL) {
		string message="Missing root in a GENERATION expression";
		erreur(STR(message));
	}

	if (tinit->Type()!=TESTFONCTIONDEPENDANCE) {
		string message="Wrong type of node";
		erreur(STR(message));
	}

	TestFonctionDependance* tf=(TestFonctionDependance*)tinit;
	if (tf->Fx->type!=C_GENERATION) {
		string message="Wrong type of node";
		erreur(STR(message));
	}
	tf->negation=negation;
	//tf->leType=TESTRACINEGENERATION;
	tf->leType=TESTNOEUDGENERATION;
	//On parcourt notre structure x_node
	parcoursxnode(tf,x,variables_utilisees);
	initfinalTestFonctionDependance(tf,negation);
	return tf;
}

#endif
