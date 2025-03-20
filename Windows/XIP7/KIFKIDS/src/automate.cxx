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
filename   : automate.cxx
Date       : 12/08/2003
Purpose    : Automaton implementation to handle lexicons
Programmer : Claude ROUX
Reviewer   :
*/

#ifdef KIF_RAW
#include "kif.h"
#define UTF8 kifGlobals->kifCodeVector[0]->utf8
ostream* flot_erreur = &cerr;
#else
#include "parsbin.h"
#include "conversion.h"
#define UTF8 parseur_courant->TestOptions(ENTREE_UTF8)
extern GlobalParseur* parseur_courant;
extern ostream* flot_erreur;
#endif

#ifndef WIN32
#define sprintf_s snprintf
#define strcpy_s(a,b,c) strncpy(a,c,b)
#define strcat_s(a,b,c) strncat(a,c,b)
#endif

//static VECTA<int> pilearcs;    
//static VECTA<int> pileposition;    
//static VECTA<etat*> pileetats;
//static char* automate_buffer=NULL;
//static int tailledebuf=0;

etat::etat() {
	status = AUTOMATONSTART;
	id = 0;
	valeur = NULL;
	marque = 0;
	nombre = 0;
}

etat::etat(automate* a, char s) {
	nombre = 0;
	marque = 0;
	valeur = NULL;
	status = s;
	id = a->dernier++;
	a->lesetats.ajoute(this, a->lesetats.taille + a->lesetats.taille / 2);
}

char testAlpha(int l) {
	if (isalpha(l))
		return AUTOMATONLETTER;

	return AUTOMATONANY;
}

char testAlphaUTF8(unsigned char* m, int& i) {
	if (c_is_alpha(m, i))
		return AUTOMATONLETTER;

	return AUTOMATONANY;
}


char analyseCaractere(unsigned char c) {

	if (c <= 32 || strchr("<>{}[](),;:.&|!/\\=~#@^?+-*$%\"'_¬£€`", c))
		return AUTOMATONPUNCT;

	if (c >= '0' && c <= '9')
		return AUTOMATONDIGIT;

	int l = c;
	if (isupper(l))
		return AUTOMATONLETTERUPPER;

	if (islower(l))
		return AUTOMATONLETTERLOWER;

	return AUTOMATONANY;
}

char analyseCaractereUTF8(unsigned char* m, int& i) {

	char res = c_is_alpha(m, i);

	if (res == 2)
		return AUTOMATONLETTERUPPER;

	if (res == 1)
		return AUTOMATONLETTERLOWER;

	return AUTOMATONANY;
}

arcautomate TestCharId(unsigned char id) {
	if (id >= '1' && id <= '9')
		return AUTOMATONVARIABLE;

	switch (id) {
	case '*':
		return AUTOMATONSTAR;
	case '+':
		return AUTOMATONPLUS;
	case 'a':
		return AUTOMATONLETTER;
	case 'c':
		return AUTOMATONLETTERLOWER;
	case 'C':
		return AUTOMATONLETTERUPPER;
	case 'd':
		return AUTOMATONDIGIT;
	case 'x':
		return AUTOMATONHEXA;
	case 'X':
		return AUTOMATONHEXAUPPER;
	case 'p':
		return AUTOMATONPUNCT;
	}
	return AUTOMATONEPSILON;
}

void etat::insertionarc(arc* a, char aupluslong) {
	if (aupluslong == NON)
		ajoutearc(a);
	else {
		//les arcs sont tries selon des criteres differents
		//On cherche a les inserer selon leur bouclage...
		arcs.ajoute(a);
	}
}

void etat::ajoutearc(arc* a) {
	//nous trions les arcs    
	if (a->action == AUTOMATONVARIABLE) {
		arcs.insere(0, a);
		return;
	}

	int i;
	for (i = 0; i<arcs.dernier; i++) {
		if (a == arcs[i])
			return;
		if (a->label>arcs[i]->label)
			break;
	}
	arcs.insere(i, a);
}


void ajouteautomate(automate* principal, automate* nouveau) {
	//Puis on rajoute notre automate dans l'automate courant
	//Tout d'abord, les etats
	int i;
	for (i = 0; i<nouveau->lesetats.dernier; i++) {
		if (nouveau->lesetats[i] != NULL) {
			principal->lesetats.ajoute(nouveau->lesetats[i]);
			nouveau->lesetats[i]->id = principal->lesetats.dernier;
		}
	}

	for (i = 0; i<nouveau->lesarcs.dernier; i++) {
		if (nouveau->lesarcs[i] != NULL)
			principal->lesarcs.ajoute(nouveau->lesarcs[i]);
	}

	//On vide ce tableau, pour eviter des destructions intempestives
	nouveau->lesetats.raz();
	//Ensuite, on effectue notre branchement...
	//Tous les arcs initiaux sont recopies comme arcs initiaux
	for (i = 0; i<nouveau->e.arcs.dernier; i++)
		principal->e.ajoutearc(nouveau->e.arcs[i]);
	//remise a zero pour la meme raison que precedemment
	nouveau->e.arcs.raz();
	nouveau->lesarcs.raz();
	delete nouveau;

}


arc::arc(automate* a, arcautomate ac, char n, unsigned char l) {
	manipulation = AUTOMATONNONE;
	label = l;
	e = NULL;
	negation = n;
	action = ac;
	marque = 0;
	id = a->lesarcs.dernier;
	a->lesarcs.ajoute(this, a->lesarcs.taille + a->lesarcs.taille / 2);
}

arc::arc(automate* a, arc* c) {
	manipulation = c->manipulation;
	label = c->label;
	e = c->e;
	negation = c->negation;
	action = c->action;
	marque = c->marque;
	id = a->lesarcs.dernier;
	a->lesarcs.ajoute(this, a->lesarcs.taille + a->lesarcs.taille / 2);
}




void nettoieduplicats(automate* principal) {
	if (principal == NULL)
		return;
	int i, j;

	VECTA<arc*> adetruire;
	VECTA<etat*> edetruire;
	principal->demarque();

	etat* ec;
	for (i = -1; i<principal->lesetats.dernier; i++) {
		if (i == -1)
			ec = &principal->e;
		else
			ec = principal->lesetats[i];
		if (ec == NULL)
			continue;
		for (j = 0; j<ec->arcs.dernier; j++)
			ec->arcs[j]->marque++;
	}



	//Pour les arcs dupliques sur plusieurs etats, on prefere dans
	//un premier temps les dupliquer...
	for (i = -1; i<principal->lesetats.dernier; i++) {
		if (i == -1)
			ec = &principal->e;
		else
			ec = principal->lesetats[i];
		for (j = 0; j<ec->arcs.dernier; j++) {
			if (ec->arcs[j]->marque>1) {
				arc* na = new arc(principal,ec->arcs[j]);
				if (adetruire.cherche(ec->arcs[j]) == -1)
					adetruire.ajoute(ec->arcs[j]);
				ec->arcs.affecte(j, na);
			}
		}
	}

	for (i = 0; i<adetruire.dernier; i++)
		delete adetruire[i];

	adetruire.raz();
	principal->demarque();
	for (i = 0; i<principal->lesetats.dernier; i++) {
		if (principal->lesetats[i] != NULL)
			principal->lesetats[i]->id = i + 1;
	}

	VECTA<int> remplacements(principal->lesetats.dernier + 1);
	principal->e.dedoublonne(adetruire, remplacements);


	principal->demarque();

	for (i = 0; i<adetruire.dernier; i++) {
		principal->lesetats.affecte(adetruire[i]->e->id - 1, NULL);
		if (adetruire[i]->e->marque == 0)
			edetruire.ajoute(adetruire[i]->e);
		adetruire[i]->e->marque = -1;
		adetruire[i]->marque = 10;
		delete adetruire[i];
	}

	for (i = -1; i<principal->lesetats.dernier; i++) {
		if (i == -1)
			ec = &principal->e;
		else
			ec = principal->lesetats[i];
		if (ec == NULL)
			continue;
		for (j = 0; j<ec->arcs.dernier; j++) {
			etat* ex = ec->arcs[j]->e;
			if (ex->marque == -1)
				ec->arcs[j]->e = principal->lesetats[remplacements[ex->id] - 1];
		}
	}

	for (i = 0; i<edetruire.dernier; i++)
		delete edetruire[i];
}

char chainecomplexe(unsigned char* chaine, int position, typeautoparse state) {

	//Chaine entierement analysee
	if (chaine[position] == 0)
		return NON;
	int lettresuivante;

	switch (state) {
		//etat initial
	case ARCINITIAL:
		lettresuivante = position;
		switch (chaine[position]) {
		case '~':
			return chainecomplexe(chaine, position + 1, ARCINITIAL);
		case '\\':
			lettresuivante = position + 1;
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
		return chainecomplexe(chaine, lettresuivante, ARCSIMPLE);
		//creation d'un simple arc
	case ARCSIMPLE:
		//si cet arc est suivi d'un symbole de Kleene: cas particulier
		//il faut aussi ne pas oublier de sauter la * ou le + d'ou la valeur d'increment a 2
		if (chaine[position + 1] == '*' || chaine[position + 1] == '+')
			return OUI;

		//on rajoute les arcs a la suite
		return chainecomplexe(chaine, position + 1, ARCINITIAL);
	}
	//On analyse un caractere special et on cree un arc au besoin 
	return NON;
}

char typeautomate(unsigned char* chaine, int position) {

	//Chaine entierement analysee
	if (chaine[position] == 0)
		return NON;

	int lettresuivante;

	lettresuivante = position + 1;
	switch (chaine[position]) {
	case '\\':
		if (chaine[position + 1] == 0)
			return NON;
		lettresuivante = position + 2;
		break;
	case '%':
		if (chaine[position + 1] == 0)
			return NON;
		if (chaine[position + 1] >= '1' && chaine[position + 1] <= '9')
			return AUTOMATEPILE;
		lettresuivante = position + 2;
		break;
	default:
		lettresuivante = position + 1;
	}

	return typeautomate(chaine, lettresuivante);
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

	if (a->erreur != AERR_NO_ERROR)
		return NULL;

	arcautomate act = AUTOMATONLABEL;
	unsigned char lab;
	etat* et;
	etat* etatf;
	arc* ar = NULL;
	arcautomate test;
	//dans ce cas le caractere est simplement saute
	lab = chaine[position];
	act = AUTOMATONLABEL;
	int increment = 1;
	typeautoparse etatsuivant;
	int lettresuivante;
	int val;

	//Chaine entierement analysee
	if (chaine[position] == 0) {
		status |= AUTOMATONEND;
		if (valeur == NULL)
			valeur = value;
		return this;
	}
	char ajouteetatsortant = NON;
	char fermeture = ']';
	char erreur_fermeture = AERR_MISSING_BRACKET;
	switch (state) {
		//etat initial
	case ARCINITIAL:
		lettresuivante = position;
		etatsuivant = ARCSIMPLE;
		switch (chaine[position]) {
		case '~':
			return parse(a, chaine, position + 1, finchaine, fin, value, debut, courant, etatfinal, ARCINITIAL, 1 - negation);
		case '\\':
			lettresuivante = position + 1;
			break;
		case '?':
			etatsuivant = ARCANY;
			lettresuivante = position;
			break;
		case '#':
			etatsuivant = ARCCODE;
			lettresuivante = position;
			break;
		case '%':
			etatsuivant = ARCCOMPLEXE;
			lettresuivante = position + 1;
			break;
		case ',':
			etatsuivant = ARCVIRGULE;
			break;
		case ']':
		case ')':
			etatsuivant = ARCFINCROPAR;
			break;
		case '[':
			etatsuivant = ARCCROCHET;
			lettresuivante = position + 1;
			break;
		case '(':
			etatsuivant = ARCPARENTHESE;
			lettresuivante = position + 1;
			break;
		}
		return parse(a, chaine, lettresuivante, finchaine, fin, value, debut, courant, etatfinal, etatsuivant, negation);
		//creation d'un simple arc
	case ARCANY:
		lab = AUTOMATONANY;
		act = AUTOMATONANY;
		ar = trouvearc(lab, act, negation, 0);
		if (ar == NULL) {
			ar = new arc(a, act, negation, lab);
			et = new etat(a, AUTOMATONNODE);
			insertionarc(ar, a->aupluslong);
			ar->e = et;
		}
		else
			et = ar->e;

		//si cet arc est suivi d'un symbole de Kleene: cas particulier
		//il faut aussi ne pas oublier de sauter la * ou le + d'ou la valeur d'increment a 2
		if (chaine[position + 1] == '*' || chaine[position + 1] == '+') {
			et->parse(a, chaine, position + 1, finchaine, fin, value, this, ar, etatfinal, ARCBOUCLE, negation);
			increment = 2;
		}

		//on rajoute les arcs a la suite
		return et->parse(a, chaine, position + increment, finchaine, fin, value, debut, courant, etatfinal, ARCINITIAL, 0);
	case ARCCODE:
		val = 0;
		lettresuivante = 0;
		while (chaine[position + 1] >= '0' && chaine[position + 1] <= '9' && lettresuivante<3) {
			val = val * 10 + chaine[++position] - 48;
			lettresuivante++;
		}
		if (val>255) {
			a->initErreur(position, AERR_WRONG_CODE);
			return NULL;
		}
		lab = val;
		if (val == 0)
			lab = '#';
	case ARCSIMPLE:
		ar = trouvearc(lab, act, negation, 0);
		if (ar == NULL) {
			ar = new arc(a, act, negation, lab);
			et = new etat(a, AUTOMATONNODE);
			insertionarc(ar, a->aupluslong);
			ar->e = et;
		}
		else
			et = ar->e;

		//si cet arc est suivi d'un symbole de Kleene: cas particulier
		//il faut aussi ne pas oublier de sauter la * ou le + d'ou la valeur d'increment a 2
		if (chaine[position + 1] == '*' || chaine[position + 1] == '+') {
			et->parse(a, chaine, position + 1, finchaine, fin, value, this, ar, etatfinal, ARCBOUCLE, negation);
			increment = 2;
		}

		//on rajoute les arcs a la suite
		return et->parse(a, chaine, position + increment, finchaine, fin, value, debut, courant, etatfinal, ARCINITIAL, 0);
		//On analyse un caractere special et on cree un arc au besoin 
	case ARCCOMPLEXE:
		test = TestCharId(chaine[position]);
		switch (test) {
		case AUTOMATONVARIABLE:
			lab = chaine[position] - 48;
			act = AUTOMATONVARIABLE;
			if (!a->construitvar(lab, position))
				return NULL;
			break;
		case AUTOMATONPLUS:
		case AUTOMATONSTAR:
			lab = AUTOMATONANY;
			act = AUTOMATONANY;
			break;
		case AUTOMATONLETTER:
		case AUTOMATONLETTERLOWER:
		case AUTOMATONLETTERUPPER:
		case AUTOMATONDIGIT:
		case AUTOMATONHEXA:
		case AUTOMATONHEXAUPPER:
		case AUTOMATONPUNCT:
			lab = test;
			act = test;
			break;
		default://cas d'echappement du caractere
			lab = chaine[position];
			act = AUTOMATONLABEL;
		}

		if (ar == NULL) {
			ar = new arc(a, act, negation, lab);
			et = new etat(a, AUTOMATONNODE);
			insertionarc(ar, a->aupluslong);
			ar->e = et;
		}
		else
			et = ar->e;

		if (test == AUTOMATONPLUS || test == AUTOMATONSTAR)
			et->parse(a, chaine, position, finchaine, fin, value, this, ar, etatfinal, ARCBOUCLE, negation);

		//si cet arc est suivi d'un symbole de Kleene: cas particulier
		//il faut aussi ne pas oublier de sauter la * ou le + d'ou la valeur d'increment a 2
		if (chaine[position + 1] == '*' || chaine[position + 1] == '+') {
			et->parse(a, chaine, position + 1, finchaine, fin, value, this, ar, etatfinal, ARCBOUCLE, negation);
			increment = 2;
		}

		//on rajoute les arcs a la suite
		return et->parse(a, chaine, position + increment, finchaine, fin, value, debut, courant, etatfinal, ARCINITIAL, 0);
		//On construit une boucle * ou +
	case ARCBOUCLE:
		//On construit la boucle vers cet etat final
		ar = new arc(a, courant->action, negation, courant->label);
		ar->e = this;
		arcs.ajoute(ar);

		if (chaine[position] == '*') {
			//L'arc qui permet l'etat vide
			ar = new arc(a,AUTOMATONEPSILON, 0, AUTOMATONEPSILON);
			debut->arcs.ajoute(ar);
			//Qui pointe vers notre etat final courant
			ar->e = this;
		}
		break;
		//Traitement du crochet
	case ARCPARENTHESE:
		ajouteetatsortant = OUI;
		fermeture = ')';
		erreur_fermeture = AERR_MISSING_PARENTHESIS;
	case ARCCROCHET:
		if (negation == 1) {
			a->initErreur(position - 1, AERR_WRONG_NEGATION_POSITION);
			return NULL;
		}
		//on construit des automates individuels
		et = new etat(a, AUTOMATONNODE);
		ar = new arc(a,AUTOMATONEPSILON, 0, AUTOMATONEPSILON);
		ar->e = et;
		arcs.ajoute(ar);
		etatf = new etat(a, AUTOMATONNODE);
		et->parse(a, chaine, position, finchaine, fermeture, value, this, NULL, etatf, ARCINITIAL, 0);
		if (chaine[finchaine] != fermeture) {
			a->initErreur(finchaine, AERR_MISSING_BRACKET);
			return NULL;
		}
		finchaine++;
		//s'il y a une boucle: cas particulier que l'on traite ici
		if (chaine[finchaine] == '*' || chaine[finchaine] == '+') {
			//tout d'abord on reboucle
			ar = new arc(a,AUTOMATONEPSILON, 0, AUTOMATONEPSILON);
			ar->e = this;
			etatf->arcs.ajoute(ar);
			//dans le cas de * il faut aussi sortir tout de suite
			if (chaine[finchaine] == '*') {
				ar = new arc(a,AUTOMATONEPSILON, 0, AUTOMATONEPSILON);
				ar->e = etatf;
				arcs.ajoute(ar);
				ajouteetatsortant = NON;
			}
			finchaine++;
		}

		if (ajouteetatsortant == OUI) {
			ar = new arc(a,AUTOMATONEPSILON, 0, AUTOMATONEPSILON);
			ar->e = etatf;
			arcs.ajoute(ar);
		}
		return etatf->parse(a, chaine, finchaine, finchaine, fin, value, debut, courant, etatfinal, ARCINITIAL, 0);
		//cas particulier du traitement de la ','
	case ARCVIRGULE:
		//on construit des automatates individuels
		//On finit d'abord d'accrocher le dernier automate construit avec l'etat final propose
		if (fin == 0) {
			a->initErreur(position, AERR_WRONG_USE_OF_COMMA);
			return NULL;
		}
		ar = new arc(a,AUTOMATONEPSILON, 0, AUTOMATONEPSILON);
		ar->e = etatfinal;
		arcs.ajoute(ar);

		//Puis on cree un nouvel automate
		et = new etat(a, AUTOMATONNODE);
		ar = new arc(a,AUTOMATONEPSILON, 0, AUTOMATONEPSILON);
		ar->e = et;
		debut->arcs.ajoute(ar);

		return et->parse(a, chaine, position + 1, finchaine, fin, value, debut, NULL, etatfinal, ARCINITIAL, 0);
		//Une fin de parenthese ou de crochet
	case ARCFINCROPAR:
		//On racroche notre automate, a l'etat final fourni
		if (fin != chaine[position]) {
			//si on attendait un crochet ou une parenthese et qu'on a l'inverse
			//c'est une erreur
			if (fin == ')')
				a->initErreur(position, AERR_MISMATCHED_PARENTHESIS);
			else
				a->initErreur(position, AERR_MISMATCHED_BRACKET);
			return NULL;
		}

		ar = new arc(a,AUTOMATONEPSILON, 0, AUTOMATONEPSILON);
		ar->e = etatfinal;
		arcs.ajoute(ar);
		//On enregistre la position de ce crochet ou de cette parenthese fermante
		//pour pouvoir remonter cette position dans l'analyse de la chaine
		finchaine = position;
		break;
	}
	return NULL;
}

void automate::initErreur(int l, autoerreur err){
	if (erreur == AERR_NO_ERROR) {
		erreur = err;
		lieuerreur = l;
	}
}


void automatepile::testordre(char ordre) {
	if (ordre == OUI)
		variables[0] = 0;
	else
		variables[0] = 100;
}

etat* automate::parse(unsigned char* mot, void* valeur, char ordre) {
	int finchaine = 0;
	erreur = AERR_NO_ERROR;
	lieuerreur = 0;
	testordre(ordre);
	return e.parse(this, mot, 0, finchaine, 0, valeur, NULL, NULL, NULL, ARCINITIAL, 0);
}


etat* etat::parsebrute(automate* a,
	unsigned char* chaine,
	int position,
	void* value,
	typeautoparse state) {

	if (a->erreur != AERR_NO_ERROR)
		return NULL;

	arcautomate act = AUTOMATONLABEL;
	etat* et;
	arc* ar = NULL;
	//dans ce cas le caractere est simplement saute
	unsigned char lab = chaine[position];

	//Chaine entierement analysee
	if (lab == 0) {
		status |= AUTOMATONEND;
		if (valeur == NULL)
			valeur = value;
		return this;
	}

	switch (state) {
		//etat initial
	case ARCINITIAL:
		return parsebrute(a, chaine, position, value, ARCSIMPLE);
		//creation d'un simple arc
	case ARCSIMPLE:
		ar = trouvearc(lab, act, NON, 0);
		if (ar == NULL) {
			ar = new arc(a, act, NON, lab);
			et = new etat(a, AUTOMATONNODE);
			ajoutearc(ar);
			ar->e = et;
		}
		else
			et = ar->e;
		//on rajoute les arcs a la suite
		return et->parsebrute(a, chaine, position + 1, value, ARCINITIAL);

	}
	return NULL;
}

//Dans ce cas, on stocke la chaine sans l'analyser. Il s'agit d'un simple mot et non
//d'une expression reguliere.
etat* automate::parsebrute(unsigned char* mot, void* valeur) {
	erreur = AERR_NO_ERROR;
	lieuerreur = 0;
	testordre(OUI);
	return e.parsebrute(this, mot, 0, valeur, ARCINITIAL);
}

etat* etat::parsebrutelin(automate* a, unsigned char* chaine, void* value) {

	if (a->erreur != AERR_NO_ERROR)
		return NULL;

	arcautomate act = AUTOMATONLABEL;
	arc* ar = NULL;
	//dans ce cas le caractere est simplement saute
	etat* courant = this;
	etat* et;
	unsigned char lab = chaine[0];
	int position = 1;

	while (lab != 0) {
		ar = courant->trouvearc(lab, act, NON, 0);
		if (ar == NULL) {
			ar = new arc(a,act, NON, lab);
			et = new etat(a, AUTOMATONNODE);
			courant->ajoutearc(ar);
			ar->e = et;
			courant = et;
		}
		else
			courant = ar->e;

		lab = chaine[position++];
	}

	courant->status |= AUTOMATONEND;
	if (courant->valeur == NULL)
		courant->valeur = value;
	return courant;
}

etat* automate::parsebrutelin(unsigned char* mot, void* valeur) {
	erreur = AERR_NO_ERROR;
	lieuerreur = 0;
	testordre(OUI);
	return e.parsebrutelin(this, mot, valeur);
}


void arc::ArcEpsilon(VECTA<etatarc*>& eas) {
	if (marque == 1)
		return;
	marque = 1;
	e->ArcEpsilon(eas);
}

void etat::ArcEpsilon(VECTA<etatarc*>& eas) {
	if (marque == 1)
		return;
	marque = 1;
	for (int i = 0; i<arcs.dernier; i++) {
		if (arcs[i]->action == AUTOMATONEPSILON) {
			etatarc* ea = new etatarc(this, arcs[i]);
			eas.ajoute(ea);
		}
		arcs[i]->ArcEpsilon(eas);
	}
}


char automate::retireEpsilon(VECTA<arc*>& adetruire) {
	//On retire les epsilons du dernier automate cree
	//Tout d'abord on recherche tous les arcs epsilons
	int i, j, k, l;

	VECTA<etatarc*> eas;

	arc* ar;
	e.ArcEpsilon(eas);
	demarque();
	etat* courant;

	if (eas.dernier == 0)
		return NON;

	char retour = NON;
	for (i = 0; i<eas.dernier; i++) {
		//on recupere tous les arcs semblables:
		k = i + 1;
		courant = eas[i]->ar->e;


		while (k<eas.dernier && eas[i]->ar->e == eas[k]->ar->e) k++;

		//Nous avons dans etatarbre toutes les transitions necessaires
		for (j = k - 1; j >= i; j--) {
			etat* ajout = eas[j]->e;

			//si ajout est un etat final ainsi que courant, on ne fait rien
			//if (ajout->end() && courant->end())
			//    continue;

			if (courant == ajout && eas[i]->ar->action == AUTOMATONEPSILON)
				continue;

			//On remplace les arcs epsilon par les arcs suivants            
			//La position de l'arc epsilon que l'on remplace
			//Tous les arcs seront places apres

			for (l = 0; l<courant->arcs.dernier; l++) {
				courant->arcs[l]->marque = -1;
				arc* trouve = ajout->trouvearc(courant->arcs[l]);
				if (trouve == NULL)
					ajout->insertionarc(courant->arcs[l], aupluslong);
			}

			if (!ajout->end()) {
				//on recupere notre transition finale                        
				ajout->status |= courant->status;
				ajout->valeur = courant->valeur;
			}
		}

		//On retire les transitions epsilons
		for (j = i; j<k; j++) {
			for (l = eas[j]->e->arcs.dernier - 1; l >= 0; l--)
			if (eas[j]->e->arcs[l] == eas[j]->ar) {
				ar = eas[j]->e->arcs.retireElement(l);
				if (ar->marque != -1) {
					//if (ar->action!=AUTOMATONMARKEDFORDELETION)
					//	adetruire.ajoute(ar);
					retour = OUI;
					ar->manipulation = AUTOMATONMARKEDFORDELETION;
				}
			}
		}

		i = k - 1;
	}

	eas.nettoie();
	demarque();
	retirefordeleted(adetruire);
	return retour;
}

char arc::test(unsigned char c) {
	if (!c)
		return 0;

	switch (action) {
	case AUTOMATONANY:
		return 1 - negation;
	case AUTOMATONHEXA:
		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
			return 1 - negation;
		return negation;
	case AUTOMATONHEXAUPPER:
		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
			return 1 - negation;
		return negation;
	case AUTOMATONDIGIT:
		if (analyseCaractere(c) == AUTOMATONDIGIT)
			return 1 - negation;
		return negation;
	case AUTOMATONPUNCT:
		if (analyseCaractere(c) == AUTOMATONPUNCT)
			return 1 - negation;
		return negation;
	case AUTOMATONLETTER:
		if (testAlpha(c) != AUTOMATONLETTER)
			return 1 - negation;
		return negation;
	case AUTOMATONLETTERLOWER:
		if (analyseCaractere(c) == AUTOMATONLETTERLOWER)
			return 1 - negation;
		return negation;
	case AUTOMATONLETTERUPPER:
		if (analyseCaractere(c) == AUTOMATONLETTERUPPER)
			return 1 - negation;
		return negation;
	case AUTOMATONLABEL:
		if (label == c)
			return 1 - negation;
		return negation;
	}

	return negation;
}

char arc::atest(unsigned char* m, int& i) {
	uchar c = m[i];
	if (!c)
		return 0;


	switch (action) {
	case AUTOMATONANY:
		return 1 - negation;
	case AUTOMATONHEXA:
		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
			return 1 - negation;
		return negation;
	case AUTOMATONHEXAUPPER:
		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
			return 1 - negation;
		return negation;
	case AUTOMATONDIGIT:
		if (analyseCaractere(c) == AUTOMATONDIGIT)
			return 1 - negation;
		return negation;
	case AUTOMATONPUNCT:
		if (analyseCaractere(c) == AUTOMATONPUNCT)
			return 1 - negation;
		return negation;
	case AUTOMATONLETTER:
		if (testAlphaUTF8(m, i) == AUTOMATONLETTER)
			return 1 - negation;
		return negation;
	case AUTOMATONLETTERLOWER:
		if (analyseCaractereUTF8(m, i) == AUTOMATONLETTERLOWER)
			return 1 - negation;
		return negation;
	case AUTOMATONLETTERUPPER:
		if (analyseCaractereUTF8(m, i) == AUTOMATONLETTERUPPER)
			return 1 - negation;
		return negation;
	case AUTOMATONLABEL:
		if (label == c)
			return 1 - negation;
		return negation;
	}

	return negation;
}

//----------------------------------------------------------------------------------------
//On extrait autant de sous-chaines possible que l'on stocke dans un vecteur
int automate::rechercheavecextractiondansvecteur(unsigned char* m, VECTA<string*>& vect) {
	int suivant = 0;
	int pos = 0;

	while (suivant != -1) {
		string* s = new string;
		suivant = rechercheavecextraction(m + pos, *s);
		if (suivant != -1)
			vect.ajoute(s);
		else {
			delete s;
			break;
		}
		suivant = maxpos;
		pos += suivant;
	}
	return vect.dernier;
}

int automate::rechercheavecextractiondansvecteur(unsigned char* m, vector<string>& vect) {
	int suivant = 0;
	int pos = 0;

	string s;
	while (suivant != -1) {
		suivant = rechercheavecextraction(m + pos, s);
		if (suivant != -1)
			vect.push_back(s);
		else
			break;
		suivant = maxpos;
		pos += suivant;
	}

	return vect.size();
}

int automate::rechercheindexes(unsigned char* m, vector<int>& vect) {
	int suivant = 0;
	int pos = 0;

	vect.clear();
	int taille;
	while (suivant != -1) {
		suivant = rechercheposition(m + pos, taille);
		if (suivant != -1) {
			vect.push_back(pos + suivant);
			vect.push_back(taille);
		}
		else
			break;
		suivant = maxpos;
		pos += suivant;
	}
	return vect.size();
}

int automate::rechercheindexes(unsigned char* m, vector<long>& vect) {
	int suivant = 0;
	int pos = 0;

	vect.clear();
	int taille;
	while (suivant != -1) {
		suivant = rechercheposition(m + pos, taille);
		if (suivant != -1) {
			vect.push_back(pos + suivant);
			vect.push_back(pos + suivant + taille);
		}
		else
			break;
		suivant = maxpos;
		pos += suivant;
	}
	return vect.size();
}

int automate::rechercheindexes(unsigned char* m, vector<double>& vect) {
	int suivant = 0;
	int pos = 0;

	vect.clear();
	int taille;
	while (suivant != -1) {
		suivant = rechercheposition(m + pos, taille);
		if (suivant != -1) {
			vect.push_back(pos + suivant);
			vect.push_back(pos + suivant + taille);
		}
		else
			break;
		suivant = maxpos;
		pos += suivant;
	}
	return vect.size();
}


int automate::rechercheposition(unsigned char* m, int& taille) {
	int l = strlen((char*)m);
	initvar();

	for (int i = 0; i<l; i++) {
		maxpos = -1;
		if (e.recherchepartielle(m, this, i)) {
			taille = maxpos - i;
			return i;
		}
	}
	return -1;
}

int automate::rechercheavecextraction(unsigned char* m, string& res) {
	int l = strlen((char*)m);
	initvar();

	if (automate_buffer == NULL) {
		automate_buffer = new char[l + 1];
		tailledebuf = l + 1;
	}
	else {
		if (tailledebuf <= l) {
			delete[] automate_buffer;
			tailledebuf = l + 1;
			automate_buffer = new char[tailledebuf];
		}
	}


	for (int i = 0; i<l; i++) {
		maxpos = -1;
		if (e.recherchepartielle(m, this, i)) {
			memcpy(automate_buffer, m + i, maxpos - i);
			automate_buffer[maxpos - i] = 0;
			res = automate_buffer;
			return i;
		}
	}
	return -1;
}



int automate::rechercheavecextractionparlafin(unsigned char* m, string& res) {
	int l = strlen((char*)m) - 1;
	initvar();

	if (automate_buffer == NULL) {
		automate_buffer = new char[l + 1];
		tailledebuf = l + 1;
	}
	else {
		if (tailledebuf <= l) {
			delete[] automate_buffer;
			tailledebuf = l + 1;
			automate_buffer = new char[tailledebuf];
		}
	}

	for (int i = l; i >= 0; i--) {
		maxpos = -1;
		if (e.recherchepartielle(m + i, this, 0)) {
			memcpy(automate_buffer, m + i, maxpos);
			automate_buffer[maxpos] = 0;
			res = automate_buffer;
			return i;
		}
	}
	return -1;
}

int automate::recherchejusqualafin(unsigned char* m, string& res) {
	int l = strlen((char*)m) - 1;
	initvar();

	if (automate_buffer == NULL) {
		automate_buffer = new char[l + 1];
		tailledebuf = l + 1;
	}
	else {
		if (tailledebuf <= l) {
			delete[] automate_buffer;
			tailledebuf = l + 1;
			automate_buffer = new char[tailledebuf];
		}
	}

	for (int i = l; i >= 0; i--) {
		maxpos = -1;
		if (recherche(m + i)) {
			memcpy(automate_buffer, m + i, l - i + 1);
			automate_buffer[l - i + 1] = 0;
			res = automate_buffer;
			return l;
		}
	}
	return -1;
}

//---------------------------------------------------------------
//Recherche de type longest match
etat* automate::recherchemax(unsigned char* mot) {
	//L'ancre permet de determiner la position du premier caractere de la chaine
	//a recopier dans la variable
	maxpos = -1;
	initvar();

	return e.recherchemax(mot, this, 0);
}


etat* arc::recherchemax(unsigned char* mot, automate* ref, int i) {
	if (e->end()) {
		if (ref->maxpos<i)
			ref->maxpos = i;
		if (mot[i] == 0)
			return e;
	}
	return e->recherchemax(mot, ref, i);
}


etat* etat::recherchemax(unsigned char* mot, automate* ref, int i) {
	etat* et = NULL;

	for (int ia = 0; ia<arcs.dernier; ia++) {
		//cas speciaux
		switch (arcs[ia]->action) {
		case AUTOMATONVARIABLE:
			//Le label correspond au numero de la variable
			if (ref != NULL)
				ref->majvar(arcs[ia], i);
			et = arcs[ia]->recherchemax(mot, ref, i);
			break;
		case AUTOMATONEPSILON:
			et = arcs[ia]->recherchemax(mot, ref, i);
			break;
		default:
			if (arcs[ia]->atest(mot, i))
				et = arcs[ia]->recherchemax(mot, ref, i + 1);
		}
		if (et != NULL)
			return et;
	}
	return NULL;
}
//---------------------------------------------------------------

etat* automate::recherchepartielle(unsigned char* m) {
	//L'ancre permet de determiner la position du premier caractere de la chaine
	//a recopier dans la variable
	maxpos = -1;
	initvar();

	//return e.recherchepartielle(m,this,0);
	etat* res = NULL;
	maxpos = e.rechercherecursive(m, this, 0, &res);
	return res;
}

int arc::rechercherecursive(unsigned char* mot, automate* ref, int i, etat** resetat) {
	switch (action) {
	case AUTOMATONVARIABLE:
		//Le label correspond au numero de la variable
		if (ref != NULL)
			ref->majvar(this, i);
	case AUTOMATONEPSILON:
		return e->rechercherecursive(mot, ref, i, resetat);
	default:
		if (atest(mot, i))
			return e->rechercherecursive(mot, ref, i + 1, resetat);
	}
	return -1;

}

int etat::rechercherecursive(unsigned char* mot, automate* ref, int i, etat** resetat) {
	int retour = -1;
	for (int a = 0; a<arcs.dernier; a++) {
		etat* res = NULL;
		int pos = arcs[a]->rechercherecursive(mot, ref, i, &res);
		if (pos>retour && res != NULL) {
			*resetat = res;
			retour = pos;
		}
	}

	if (retour == -1 && end()) {
		*resetat = this;
		return i;
	}
	return retour;
}

etat* etat::recherchepartielle(unsigned char* mot, automate* ref, int i) {

	ref->pilearcs.raz();
	ref->pileposition.raz();
	ref->pileetats.raz();
	char erreur = OUI;
	etat* courant = this;
	arc* a;
	int ia = 0;
	while (courant != NULL) {
		while (ia<courant->arcs.dernier) {
			//cas speciaux
			erreur = OUI;
			a = courant->arcs[ia];
			switch (a->action) {
			case AUTOMATONVARIABLE:
				//Le label correspond au numero de la variable
				if (ref != NULL)
					ref->majvar(a, i);
			case AUTOMATONEPSILON:
				//on passe directement a l'etat suivant
				ref->pilearcs.ajoute(ia);
				ref->pileposition.ajoute(i);
				ref->pileetats.ajoute(courant);
				courant = a->e;
				ia = 0;
				erreur = NON;
				break;
			default:
				if (a->atest(mot, i)) {
					ref->pilearcs.ajoute(ia);
					ref->pileposition.ajoute(i);
					ref->pileetats.ajoute(courant);
					courant = a->e;
					i++;
					ia = 0;
					erreur = NON;
				}
				else
					ia++;
			}

			if (courant->end() && erreur == NON) {
				int retrec = -1;
				//traitement des arcs optionels finaux...
				if (courant->arcs.dernier != 0) {
					etat* resetat;
					retrec = courant->rechercherecursive(mot, ref, i, &resetat);
					if (retrec != -1) {
						if (ref->maxpos<retrec)
							ref->maxpos = retrec;
					}
				}
				if (ref->maxpos<i)
					ref->maxpos = i;
				return courant;
			}
		}

		courant = ref->pileetats.retire();
		i = ref->pileposition.retire();
		ia = ref->pilearcs.retire() + 1;
	}
	return NULL;
}

VECTA<int>* lapiledesargs = NULL;
//----------------------------------------------------------------------------------------
etat* etat::recherchelin(unsigned char* mot, automate* ref, int i) {

	if (lapiledesargs == NULL)
		lapiledesargs = &ref->pilearcs;

	ref->pilearcs.raz();
	ref->pileposition.raz();
	ref->pileetats.raz();
	char erreur = OUI;
	etat* courant = this;
	arc* a;
	int ia = 0;
	while (courant != NULL) {
		while (ia<courant->arcs.dernier) {
			//cas speciaux
			erreur = OUI;
			a = courant->arcs[ia];
			switch (a->action) {
			case AUTOMATONVARIABLE:
				//Le label correspond au numero de la variable
				if (ref != NULL)
					ref->majvar(a, i);
			case AUTOMATONEPSILON:
				//on passe directement a l'etat suivant
				//S'il y a d'autres elements possibles en attente
				//On empile notre position
				if (ia<courant->arcs.dernier - 1) {
					ref->pilearcs.ajoute(ia);
					ref->pileposition.ajoute(i);
					ref->pileetats.ajoute(courant);
				}
				courant = a->e;
				ia = 0;
				erreur = NON;
				break;
			default:
				if (a->atest(mot, i)) {
					if (ia<courant->arcs.dernier - 1) {
						ref->pilearcs.ajoute(ia);
						ref->pileposition.ajoute(i);
						ref->pileetats.ajoute(courant);
					}
					courant = a->e;
					i++;
					ia = 0;
					erreur = NON;
				}
				else
					ia++;
			}

			if (!mot[i] && courant->end() && erreur == NON) {
				ref->maxpos = i;
				return courant;
			}
		}

		courant = ref->pileetats.retire();
		i = ref->pileposition.retire();
		ia = ref->pilearcs.retire() + 1;
	}
	return NULL;
}

etat* arc::recherche(unsigned char* mot, automate* ref, int i, int max) {
	if (mot[i] == 0 && e->end())
		return e;
	return e->recherche(mot, ref, i, max);
}

etat* etat::recherche(unsigned char* mot, automate* ref, int i, int max) {
	if (i + nombre > max)
		return NULL;

	etat* et = NULL;
	for (int ia = 0; ia<arcs.dernier; ia++) {
		//cas speciaux
		switch (arcs[ia]->action) {
		case AUTOMATONIF:
			if (arcs[ia]->recherche(mot, ref, i, max))
				return NULL;
			break;
		case AUTOMATONELSE:
		case AUTOMATONVARIABLE:
			//Le label correspond au numero de la variable
			if (ref != NULL)
				ref->majvar(arcs[ia], i);
			//on passe directement a l'etat suivant
			et = arcs[ia]->recherche(mot, ref, i, max);
			break;
		case AUTOMATONEPSILON:
			et = arcs[ia]->recherche(mot, ref, i, max);
			break;
		default:
			if (arcs[ia]->atest(mot, i))
				et = arcs[ia]->recherche(mot, ref, i + 1, max);
		}
		if (et != NULL)
			return et;
	}
	return NULL;
}

char arc::retirebrute(unsigned char* mot, automate* ref, int i) {
	if (mot[i] == 0 && e->end()) {
		//de deux choses l'une, cet arc est le dernier
		//si cet arc est le dernier
		if (e->arcs.dernier == 0) {
			ref->lesetats.affecte(e->id - 1, NULL);
			delete e;
			return 2;
		}
		else {
			e->status &= ~AUTOMATONEND; //sinon, on le marque comme n'etant plus un etat final
			e->valeur = NULL;
			return 1;
		}
	}

	char sts = e->retirebrute(mot, ref, i);
	if (sts == 2) {
		//attention, si les arcs sont vides pour l'etat suivant
		//il faut quand meme verifier qu'il n'est pas un etat final
		if (e->arcs.dernier == 0 && !e->end()) {
			ref->lesetats.affecte(e->id - 1, NULL);
			delete e;
			return 2;
		}
	}
	else {
		if (sts == 1) {
			e->status &= ~AUTOMATONEND; //sinon, on le marque comme n'etant plus un etat final
			e->valeur = NULL;
			return 1;
		}
	}
	return 0;
}

char etat::retirebrute(unsigned char* mot, automate* ref, int i) {

	char sts;
	for (int ia = 0; ia<arcs.dernier; ia++) {
		if (arcs[ia]->atest(mot, i)) {
			sts = arcs[ia]->retirebrute(mot, ref, i + 1);
			if (sts == 2) {
				status |= AUTOMATONEND;
				//on doit retirer cet arc
				arc* a = arcs.retireElement(ia);
				ref->lesarcs.affecte(a->id, NULL);
				delete a;
				return 2;
			}
			else
			if (sts == 1)
				return 1;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void arc::recherchetous(unsigned char* mot, VECTA<void*>& res, int i, int max) {
	if (mot[i] == 0 && e->end()) {
		for (int j = 0; j<res.dernier; j++)
		if (res[j] == e->valeur)
			return;
		res.ajoute(e->valeur);
	}
	e->recherchetous(mot, res, i, max);
}

void etat::recherchetous(unsigned char* mot, VECTA<void*>& res, int i, int max) {
	if (i + nombre > max)
		return;

	for (int ia = 0; ia<arcs.dernier; ia++) {
		//cas speciaux
		switch (arcs[ia]->action) {
		case AUTOMATONIF:
			if (arcs[ia]->recherche(mot, NULL, i, max))
				return;
			break;
		case AUTOMATONELSE:
		case AUTOMATONEPSILON:
			arcs[ia]->recherchetous(mot, res, i, max);
			break;
		default:
			if (arcs[ia]->atest(mot, i))
				arcs[ia]->recherchetous(mot, res, i + 1, max);
		}
	}
}

void etat::recherchetouslin(automate* ref, unsigned char* mot, VECTA<void*>& res, int i) {

	ref->pilearcs.raz();
	ref->pileposition.raz();
	ref->pileetats.raz();

	char erreur = NON;
	etat* courant = this;
	arc* a;
	int ia = 0;
	while (courant != NULL) {
		while (ia<courant->arcs.dernier) {
			//cas speciaux
			erreur = OUI;
			a = courant->arcs[ia];
			switch (a->action) {
			case AUTOMATONEPSILON:
				//on passe directement a l'etat suivant
				//S'il y a d'autres elements possibles en attente
				//On empile notre position
				if (ia<courant->arcs.dernier - 1) {
					ref->pilearcs.ajoute(ia);
					ref->pileposition.ajoute(i);
					ref->pileetats.ajoute(courant);
				}
				courant = a->e;
				ia = 0;
				erreur = NON;
				break;
			default:
				if (a->atest(mot, i)) {
					if (ia<courant->arcs.dernier - 1) {
						ref->pilearcs.ajoute(ia);
						ref->pileposition.ajoute(i);
						ref->pileetats.ajoute(courant);
					}
					courant = a->e;
					i++;
					ia = 0;
					erreur = NON;
				}
				else
					ia++;
			}
			if (erreur == NON && courant->end() && mot[i] == 0) {
				char trouve = NON;
				for (int j = 0; j<res.dernier; j++) {
					if (res[j] == courant->valeur) {
						trouve = OUI;
						break;
					}

				}
				if (trouve == NON)
					res.ajoute(courant->valeur);
			}
		}

		courant = ref->pileetats.retire();
		i = ref->pileposition.retire();
		ia = ref->pilearcs.retire() + 1;
	}
}


//----------------------------------------------------------------------------------------
void arc::compose(arc* a, VECTA<void*>& res) {
	if (a->e->end() && e->end()) {
		for (int i = 0; i<res.dernier; i++)
		if (res[i] == e->valeur)
			return;
		res.ajoute(e->valeur);
	}
	e->compose(a->e, res);
}

void etat::compose(etat* e, VECTA<void*>& res) {
	int i;
	for (int ia = 0; ia<arcs.dernier; ia++) {
		//cas speciaux
		if (arcs[ia]->action == AUTOMATONEPSILON)
			arcs[ia]->e->compose(e, res);
		else {
			for (i = 0; i<e->arcs.dernier; i++) {
				if (arcs[ia]->test(e->arcs[i]->label))
					arcs[ia]->compose(e->arcs[i], res);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
automate::automate(unsigned char* m, void* ind) {
	aupluslong = NON;
	automate_buffer = NULL;
	tailledebuf = 0;

	dernier = 1;
	parse(m, ind);
}

automate::automate() {
	aupluslong = NON;
	automate_buffer = NULL;
	tailledebuf = 0;
	dernier = 1;
	erreur = AERR_NO_ERROR;
}


void automate::duplique(automate* a) {
	etat* et;
	int i, j;

	e.status = a->e.status;
	e.valeur = a->e.valeur;
	//Pour tous les etats, on effectue une duplication
	for (i = 0; i<a->lesetats.dernier; i++) {
		if (a->lesetats[i] != NULL) {
			et = new etat(this, a->lesetats[i]->status);
			et->valeur = a->lesetats[i]->valeur;
		}
	}

	//Pour chaque etat nouveau on duplique les arcs
	for (i = -1; i<a->lesetats.dernier; i++) {
		//On prend donc un etat a dupliquer
		if (i == -1)
			et = &a->e;
		else
			et = a->lesetats[i];
		//Pour tous les arcs de cet etat
		for (j = 0; j<et->arcs.dernier; j++) {
			//On cree un arc equivalent
			arc* ar = new arc(this, et->arcs[j]->action, et->arcs[j]->negation, et->arcs[j]->label);
			//qui pointe vers l'etat suivant
			if (et->arcs[j]->e->id>0)
				ar->e = lesetats[et->arcs[j]->e->id - 1];
			else
				ar->e = &e;
			if (i == -1)
				e.arcs.ajoute(ar);
			else
				lesetats[i]->arcs.ajoute(ar);
		}
	}
}

int automate::compare(automate* a) {

	static char m1[1000];
	static char m2[1000];

	chaine(m1);
	a->chaine(m2);
	return strcmp(m1, m2);
}

void arc::chaine(char* m) {
	unsigned char x[] = { label, 0 };

	if (strlen(m) >= 950)
		return;

	switch (action) {
	case AUTOMATONIF:
		strcat_s(m, 1000, "IF");
		break;
	case AUTOMATONELSE:
		strcat_s(m, 1000, "LS");
		break;
	case AUTOMATONEPSILON:
		strcat_s(m, 1000, "ES");
		break;
	case AUTOMATONANY:
		strcat_s(m, 1000, "%?");
		break;
	case AUTOMATONHEXA:
		strcat_s(m, 1000, "hx");
		break;
	case AUTOMATONHEXAUPPER:
		strcat_s(m, 1000, "HX");
		break;
	case AUTOMATONDIGIT:
		strcat_s(m, 1000, "DG");
		break;
	case AUTOMATONPUNCT:
		strcat_s(m, 1000, "PT");
		break;
	case AUTOMATONLETTER:
		strcat_s(m, 1000, "Le");
		break;
	case AUTOMATONLETTERLOWER:
		strcat_s(m, 1000, "le");
		break;
	case AUTOMATONLETTERUPPER:
		strcat_s(m, 1000, "LE");
		break;
	case AUTOMATONLABEL:
		strcat_s(m, 1000, (char*)x);
	}
	e->chaine(m);
}


void etat::chaine(char* m) {
	if (strlen(m) >= 950)
		return;

	strcat_s(m, 1000, "<");
	if (start())
		strcat_s(m, 1000, "s");
	if (end())
		strcat_s(m, 1000, "f");
	strcat_s(m, 1000, ">");
	for (int i = 0; i<arcs.dernier; i++) {
		arcs[i]->chaine(m);
	}
}

void automate::chaine(char* m) {
	m[0] = 0;
	for (int i = 0; i<e.arcs.dernier; i++) {
		e.arcs[i]->chaine(m);
	}
}

void automate::mot(char* m, int mx) {
	int i = 0;
	etat* et = &e;
	while (!et->end() && i<mx - 1) {
		if (et->arcs[0]->action != AUTOMATONVARIABLE)
			m[i++] = et->arcs[0]->label;
		et = et->arcs[0]->e;
	}
	m[i] = 0;
}

void automate::mot(string& m) {
	etat* et = &e;
	while (!et->end()) {
		if (et->arcs[0]->action != AUTOMATONVARIABLE)
			m += et->arcs[0]->label;
		et = et->arcs[0]->e;
	}
}

void automatepile::mot(char* m, int mx) {
	int i = 0;
	etat* et = &e;
	while (!et->end()) {
		//dans le cas d'une variable, on l'integre dans la chaine
		if (et->arcs[0]->action == AUTOMATONVARIABLE) {
			strcpy_s(m + i, mx - i, (char*)chaines[et->arcs[0]->label].c_str());
			i = strlen(m);
		}
		else
			m[i++] = et->arcs[0]->label;
		et = et->arcs[0]->e;
	}
	m[i] = 0;
}

void automatepile::mot(string& m) {
	etat* et = &e;
	while (!et->end()) {
		//dans le cas d'une variable, on l'integre dans la chaine
		if (et->arcs[0]->action == AUTOMATONVARIABLE)
			m += (char*)chaines[et->arcs[0]->label].c_str();
		else
			m += et->arcs[0]->label;
		et = et->arcs[0]->e;
	}
}

arc* etat::trouvearc(unsigned char l, arcautomate a, char neg, char boucle) {
	for (int i = 0; i<arcs.dernier; i++) {
		if (arcs[i]->label == l && arcs[i]->negation == neg && arcs[i]->action == a)
		if (!boucle || arcs[i]->e->id <= id)
			return arcs[i];
	}
	return NULL;
}

arc* etat::trouvearc(arc* a) {
	for (int i = 0; i<arcs.dernier; i++) {
		if (arcs[i]->label == a->label &&
			arcs[i]->negation == a->negation &&
			arcs[i]->action == a->action &&
			arcs[i]->e == a->e)
			return arcs[i];
	}
	return 0;
}

void automate::demarque() {
	int j;
	e.marque = 0;
	for (j = 0; j<e.arcs.dernier; j++)
		e.arcs[j]->marque = 0;

	int i;
	for (i = 0; i<lesetats.dernier; i++) {
		if (lesetats[i] != NULL)
			lesetats[i]->marque = 0;
	}

	for (i = 0; i<lesarcs.dernier; i++) {
		if (lesarcs[i] != NULL)
			lesarcs[i]->marque = 0;
	}
}

void etat::print(int profondeur, ostream* os) {
	if (marque == 1)
		return;
	marque = 1;
	char ch[20];
	char chh[20];
	ch[0] = 0;
	if (start())
		strcat_s(ch, 20, "s");
	if (node())
		strcat_s(ch, 20, "n");
	if (end())
		strcat_s(ch, 20, "e");
	sprintf_s(chh, 20, "%03d", id);
	strcat_s(ch, 20, chh);
	//sprintf(chh,"=%d",nombre);
	//strcat_s(ch,20,chh);
	if (end()) {
		sprintf_s(chh, 20, "[%d]", valeur);
		strcat_s(ch, 20, chh);
	}
	*os << ch;
	profondeur += strlen(ch);
	for (int i = 0; i<arcs.dernier; i++) {
		arcs[i]->print(id, profondeur, os);
		if (i<arcs.dernier - 1) {
			for (int k = 0; k<profondeur; k++)
				*os << " ";
		}
	}

	if (profondeur == 0 || end())
	if (!arcs.dernier || !arcs.fin()->e->end())
		*os << endl;

}

void arc::print(int id, int profondeur, ostream* os) {

	int taille = 1;
	char ch[20];
	unsigned char x[] = { label, 0 };

	strcpy_s(ch, 20, "-(");
	if (negation == 1) {
		strcat_s(ch, 20, "~");
		taille++;
	}
	switch (action) {
	case AUTOMATONIF:
		strcat_s(ch, 20, "IF");
		break;
	case AUTOMATONELSE:
		strcat_s(ch, 20, "LS");
		break;
	case AUTOMATONEPSILON:
		strcat_s(ch, 20, "ES");
		break;
	case AUTOMATONANY:
		strcat_s(ch, 20, "%?");
		break;
	case AUTOMATONHEXA:
		strcat_s(ch, 20, "hx");
		break;
	case AUTOMATONHEXAUPPER:
		strcat_s(ch, 1000, "HX");
		break;
	case AUTOMATONDIGIT:
		strcat_s(ch, 20, "DG");
		break;
	case AUTOMATONPUNCT:
		strcat_s(ch, 20, "PT");
		break;
	case AUTOMATONLETTER:
		strcat_s(ch, 20, "Le");
		break;
	case AUTOMATONLETTERLOWER:
		strcat_s(ch, 20, "le");
		break;
	case AUTOMATONLETTERUPPER:
		strcat_s(ch, 20, "LE");
		break;
	case AUTOMATONVARIABLE:
		strcat_s(ch, 20, "%");
		x[0] += 48;
	case AUTOMATONLABEL:
		strcat_s(ch, 20, (char*)x);
	}
	strcat_s(ch, 20, ")-");
	*os << ch;
	if (e->marque == 0)
		e->print(profondeur + strlen(ch), os);
	else {
		*os << ">";
		if (e->start())
			*os << ch << "s";
		if (e->node())
			*os << "n";
		if (e->end())
			*os << "e";
		sprintf_s(ch, 20, "%03d", e->id);
		*os << ch << endl;
	}
}


void automate::print(ostream* os) {
	*os << Endl;
	demarque();
	e.print(0, os);
	demarque();
	*os << endl << Endl;
}

char automate::retirebrute(unsigned char* m) {
	//L'ancre permet de determiner la position du premier caractere de la chaine
	//a recopier dans la variable    
	return e.retirebrute(m, this, 0);
}


etat* automate::recherche(unsigned char* m) {
	//L'ancre permet de determiner la position du premier caractere de la chaine
	//a recopier dans la variable
	initvar();
	maxpos = -1;
	return e.recherchelin(m, this, 0);
}

etat* automate::recherche(string& m) {
	//L'ancre permet de determiner la position du premier caractere de la chaine
	//a recopier dans la variable
	return recherche((unsigned char*)m.c_str());
}



etat* automate::recherche(char* m) {
	initvar();
	maxpos = -1;
	return e.recherchelin((unsigned char*)m, this, 0);
}

void automate::recherchetouslabels(unsigned char* m, VECTA<void*>& res) {
	initvar();
	e.recherchetouslin(this, m, res, 0);
}

void automate::recherchetouslabels(char* m, VECTA<void*>& res) {
	initvar();
	e.recherchetouslin(this, (unsigned char*)m, res, 0);
}

void automate::compose(automate& a, VECTA<void*>& res) { e.compose(&(a.e), res); }


void etat::aupluspres() {
	if (marque == 1)
		return;
	marque = 1;
	arcs.aupluspres();

	for (int i = 0; i<arcs.dernier; i++)
		arcs[i]->aupluspres(id);
}

void arc::aupluspres(int id) {
	if (marque == 1)
		return;
	marque = 1;
	e->aupluspres();
}

void automate::retirefordeleted(VECTA<arc*>& adetruire) {
	etat* courant;
	arc* ar;
	for (int i = -1; i<lesetats.dernier; i++) {
		if (i == -1)
			courant = &e;
		else
			courant = lesetats[i];
		for (int j = courant->arcs.dernier - 1; j >= 0; j--) {
			ar = courant->arcs[j];
			if (ar->manipulation == AUTOMATONMARKEDFORDELETION || ar->manipulation == AUTOMATONDELETED) {
				if (ar->manipulation == AUTOMATONMARKEDFORDELETION) {
					adetruire.ajoute(ar);
					ar->manipulation = AUTOMATONDELETED;
				}
				courant->arcs.retireElement(j);
			}
		}
	}
}

void automate::aupluspres() {
	VECTA<arc*> adetruire;
	demarque();
	while (retireEpsilon(adetruire) == OUI)
		demarque();
	//retireEpsilon(adetruire);
	//demarque();
	//fusion(adetruire);	
	//demarque();
	e.CalculChemin(0);
	for (int i = 0; i < adetruire.dernier; i++) {
		lesarcs.affecte(adetruire[i]->id, NULL);
		delete adetruire[i];
	}
	vectorisation();
}

automate::~automate() {
	lesetats.nettoie();
	lesarcs.nettoie();
}

void automate::nettoie() {
	lesarcs.nettoie();
	lesetats.nettoie();
	e.arcs.raz();
	dernier = 1;
}

void automatepile::extraitvariables(unsigned char* ch, string souschaines[10]) {
	int prem = 0;
	char mot[TailleChaine];
	int i;
	for (i = 1; i<10; i++)
		souschaines[i] = "";
	if (variables[0] != -1)
		prem = variables[0];
	for (i = 1; i <= maxvar; i++) {
		if (variables[i] != -1) {
			int taille = variables[i] - prem;
			if (taille>TailleChaine)
				taille = TailleChaine - 1;
			memcpy(mot, ch + prem, taille);
			mot[taille] = 0;
			souschaines[i] = mot;
			prem = variables[i];
		}
	}
}

void automatepile::extraitvariables(unsigned char* ch, automate* a) {
	if (a->Type() != AUTOMATEPILE)
		return;
	automatepile* b = (automatepile*)a;
	int prem = 0;
	char mot[TailleChaine];
	int i;
	b->raz();
	for (i = 1; i <= maxvar; i++) {
		if (variables[i] != -1) {
			int taille = variables[i] - prem;
			if (taille>TailleChaine)
				taille = TailleChaine - 1;
			memcpy(mot, ch + prem, taille);
			mot[taille] = 0;
			b->chaines[i] = (char*)mot;
			prem = variables[i];
		}
	}
}

void automatepile::extraction(unsigned char* ch) {
	int prem = 0;
	char mot[TailleChaine];
	int i;
	for (i = 1; i<10; i++)
		chaines[i] = "";
	for (i = 1; i <= maxvar; i++) {
		if (variables[i] != -1) {
			int taille = variables[i] - prem;
			if (taille>TailleChaine)
				taille = TailleChaine - 1;
			memcpy(mot, ch + prem, taille);
			mot[taille] = 0;
			chaines[i] = (char*)mot;
			prem = variables[i];
		}
	}
}


void automatepile::affecte(string souschaines[10]) {
	for (size_t i = 1; i<10; i++) {
		if (souschaines[i].size() != 0)
			chaines[i] = souschaines[i];
	}
}


void automatepile::initvar() {
	maxvar = 0;
	for (int i = 0; i<10; i++)
		variables[i] = -1;
}

void automatepile::majvar(arc* a, int i) {
	variables[a->label] = i;
	if (maxvar<a->label)
		maxvar = a->label;
}

char automatepile::construitvar(unsigned char lab, int position) {
	if (variables[0] != 100) {
		if (lab <= variables[0]) {
			initErreur(position, AERR_WRONG_VARIABLE_ORDER);
			return NON;
		}
		variables[0] = lab;
	}
	return OUI;
}

void automatepile::raz() {
	for (size_t i = 0; i<10; i++) {
		chaines[i] = "";
	}
}


int arc::CalculChemin(int max) {
	if (marque == 1)
		return -1;
	marque = 1;

	int ret = e->CalculChemin(max);

	if (ret == -1)
		return -1;

	marque = -1;

	if (action == AUTOMATONEPSILON)
		return ret;

	return ret + 1;
}

int etat::CalculChemin(int max) {
	if (marque == 1)
		return -1;
	marque = 1;

	char debut = OUI;
	int minarc = 0;
	int i;
	char debutcle = OUI;
	for (i = 0; i<arcs.dernier; i++) {

		if (arcs[i]->marque == 1)
			continue;

		int nb = arcs[i]->CalculChemin(0);

		if (nb == -1)
			continue;

		if (debut == OUI || nb<minarc) {
			minarc = nb;
			debut = NON;
			debutcle = NON;
		}
	}


	if (!end())
		nombre = minarc + max;
	else
		nombre = max;

	marque = -1;
	return nombre;
}



void arc::dedoublonne() {
	if (marque == 1)
		return;
	marque = 1;
}


//if (arcs[i]->marque==1)
//	continue;
//if (arcsaretirer.cherche(i)!=-1)
//	continue;
//if (arcs[i]->e->marque)
//	continue;
//for (j=i+1;j<arcs.dernier;j++) {
//	if (arcs[j]->e==arcs[i]->e)
//		continue;
//	if (arcs[j]->e->marque)
//		continue;

//	//Il peut y en avoir plus d'un
//	if (arcs[j]->label==arcs[i]->label && arcs[j]->action==arcs[j]->action) {
//		if (arcs[j]->e->end() && arcs[i]->e->end() && arcs[i]->e->valeur!=arcs[j]->e->valeur)
//			continue;
//		
//		if (arcsaretirer.cherche(j)==-1) {
//			arcsaretirer.ajoute(j);
//			arcspointant.ajoute(arcs[i]);
//		}
//	}
//	else
//		break;
//      }

void etat::dedoublonne(VECTA<arc*>& det, VECTA<int>& remplacements) {
	if (marque)
		return;
	marque = 1;
	int i = 0;
	int j;

	//Tout d'abord on retire les arcs dupliques
	VECTA<arc*> pointants;
	VECTA<int> lesarcs;

	for (i = 0; i<arcs.dernier; i++) {
		if (arcs[i]->marque >= 2)
			continue;
		if (arcs[i]->e->marque == 10)
			continue;

		arcs[i]->marque = 1;
		for (j = i + 1; j<arcs.dernier; j++) {
			if (arcs[j]->marque)
				continue;
			if (arcs[j]->e == arcs[i]->e)
				continue;

			//Il peut y en avoir plus d'un
			//Les arcs finaux sont soigneusement evites
			if (arcs[j]->e->end() && arcs[i]->e->end())
				continue;

			if (arcs[j]->label == arcs[i]->label && arcs[j]->action == arcs[j]->action) {
				arcs[j]->marque = 2;
				pointants.ajoute(arcs[i]);
				lesarcs.ajoute(j);
			}
		}
	}

	for (i = lesarcs.dernier - 1; i >= 0; i--) {
		arc* a = arcs.retireElement(lesarcs[i]);
		arc* p = pointants[i];
		if (a->marque == 2 && a->e->marque == 0) {
			//On garde la trace de notre noeud
			if (a->e->end()) {
				p->e->status |= a->e->status;
				p->e->valeur = a->e->valeur;
			}
			for (j = 0; j<a->e->arcs.dernier; j++)
				p->e->ajoutearc(a->e->arcs[j]);
			remplacements.affecte(a->e->id, p->e->id);
			a->e->arcs.raz();
			a->e->marque = 10;
			det.ajoute(a);
		}
		a->marque = 3;
	}

	for (i = arcs.dernier - 1; i >= 0; i--) {
		if (arcs[i]->marque == 2) {
			if (det.cherche(arcs[i]) == -1)
				det.ajoute(arcs.retireElement(i));
		}
	}

	for (i = 0; i<arcs.dernier; i++)
		arcs[i]->e->dedoublonne(det, remplacements);
}


void etat::marqueduplicat() {
	if (marque == 1)
		return;
	marque = 1;
	int i;

	//Tout d'abord on retire les arcs dupliques
	for (i = 0; i<arcs.dernier; i++) {
		arcs[i]->marque++;
		arcs[i]->e->marqueduplicat();
	}
}

void arc::fusion(VECTA<arc*>& adetruire) {
	if (marque == 1)
		return;
	marque = 1;
	e->fusion(adetruire);
}


void etat::fusion(VECTA<arc*>& adetruire) {
	if (marque == 1)
		return;
	marque = 1;

	int i = 0;
	int j;
	VECTA<int> arcsaretirer;

	while (i<arcs.dernier) {
		j = i + 1;
		//Nous savons que les noeuds sont deja ordonnes en fonction de leur label
		while (j<arcs.dernier &&
			arcs[j]->label == arcs[i]->label &&
			arcs[j]->action == arcs[i]->action)
			j++;
		//
		//si nous avons au moins deux noeuds semblables
		etat* et = arcs[i]->e;
		int larc;
		for (int k = i + 1; k<j; k++) {
			//Nous prenons tous les arcs pointes par k et nous les repartissons sur le noeud 
			//pointe par i
			etat* etbis = arcs[k]->e;
			if (etbis != NULL) {
				if (arcs[k]->manipulation != AUTOMATONDELETED)
					arcs[k]->manipulation = AUTOMATONMARKEDFORDELETION;

				for (larc = 0; larc<etbis->arcs.dernier; larc++) {
					//d'abord on verifie que cet arc n'a pas deja ete copie
					if (et->arcs.cherche(etbis->arcs[larc]) == -1)
						et->ajoutearc(etbis->arcs[larc]);
				}
				//cet arc k devra par la suite etre retire
				arcsaretirer.ajoute(k);
				et->status |= etbis->status;
			}
		}
		//Puis, comme il y peut y avoir des arcs partages, on les reinitialise
		for (larc = 0; larc<et->arcs.dernier; larc++) {
			//d'abord on verifie que cet arc n'a pas deja ete copie
			et->arcs[larc]->manipulation = AUTOMATONNONE;
		}
		i = j;
	}

	//On retire nos arcs
	for (i = arcsaretirer.dernier - 1; i >= 0; i--) {
		arc* a = arcs.retireElement(arcsaretirer[i]);
		if (a->manipulation == AUTOMATONMARKEDFORDELETION) {
			adetruire.ajoute(a);
			a->manipulation = AUTOMATONDELETED;
		}
	}

	for (i = 0; i<arcs.dernier; i++)
		arcs[i]->fusion(adetruire);
}


void automate::fusion(VECTA<arc*>& adetruire) {
	demarque();
	e.fusion(adetruire);
	demarque();
	retirefordeleted(adetruire);
}


int retournecode(arc* a) {
	switch (a->action) {
	case AUTOMATONANY:
		return 257;
	case AUTOMATONDIGIT:
		return 258;
	case AUTOMATONPUNCT:
		return 259;
	case AUTOMATONHEXA:
		return 260;
	case AUTOMATONHEXAUPPER:
		return 261;
	default:
		return a->label;
	}
}


void automatevecteur::vectorisation() {
	int i;
	for (i = 0; i<260; i++)
		vectorise[i] = NULL;

	for (i = 0; i<e.arcs.dernier; i++) {
		int idx = retournecode(e.arcs[i]);
		vectorise[idx] = e.arcs[i];
	}
}

void automatevecteur::recherchetouslabels(unsigned char* m, VECTA<void*>& res) {

	arcautomate lb = (arcautomate)analyseCaractere(m[0]);
	int idx;
	switch (lb) {
	case AUTOMATONDIGIT:
		idx = 258;
		break;
	case AUTOMATONPUNCT:
		idx = 259;
		break;
	case AUTOMATONHEXA:
		idx = 260;
		break;
	case AUTOMATONHEXAUPPER:
		idx = 261;
		break;
	default:
		idx = m[0];
	}

	char init = 0;
	if (vectorise[idx] != NULL) {
		initvar();
		init = 1;
		if (m[1] == 0) {
			if (vectorise[idx]->e->end())
				res.ajoute(vectorise[idx]->e->valeur);
		}
		else
			vectorise[idx]->e->recherchetouslin(this, m, res, 1);
	}
	//Dans le cas d'un super label, on peut quand meme tester la valeur directe du caractere.
	if (idx >= 258) {
		if (!init)
			initvar();
		init = 1;
		idx = m[0];
		if (vectorise[idx] != NULL) {
			if (m[1] == 0) {
				if (vectorise[idx]->e->end())
					res.ajoute(vectorise[idx]->e->valeur);
			}
			else
				vectorise[idx]->e->recherchetouslin(this, m, res, 1);
		}
	}
	//Cas ANY
	if (vectorise[257] != NULL) {
		if (!init)
			initvar();
		if (m[1] == 0) {
			if (vectorise[idx]->e->end())
				res.ajoute(vectorise[idx]->e->valeur);
		}
		else
			vectorise[idx]->e->recherchetouslin(this, m, res, 1);
	}
}

void automatevecteur::recherchetouslabels(char* m, VECTA<void*>& res) {
	recherchetouslabels((unsigned char*)m, res);
}

string replacestringwithautomaton(string &str, string& reg, string& rep) {
	string res;

	automate* a;
	string* var = NULL;
	char typeauto = typeautomate(USTR(reg), 0);
	if (typeauto == AUTOMATEPILE) {
		a = new automatepile(USTR(reg), (void*)1);
		var = new string[10];
	}
	else
		a = new automate(USTR(reg), (void*)1);

	size_t found = 0;
	uchar* ucstr = USTR(str);


	while (found != -1 && ucstr[0] != 0) {
		string sub;
		found = a->rechercheavecextraction(ucstr, sub);
		if (found != -1) {
			string repstr = rep;
			if (var != NULL) {
				((automatepile*)a)->variables[0] = found;
				a->extraitvariables(ucstr, var);
				for (int i = 1; i<10; i++) {
					if (var[i] != "") {
						string rs = "%";
						rs += (char)(48 + i);
						repstr = s_replacestring(repstr, rs, var[i]);
					}
				}
			}
			uchar c = ucstr[found];
			ucstr[found] = 0;
			res += (char*)ucstr;
			res += repstr;
			ucstr[found] = c;
			ucstr = ucstr + found + sub.size();
		}
	}
	if (ucstr[0] != 0)
		res += (char*)ucstr;
	if (var != NULL)
		delete[] var;
	delete a;
	return res;
}
//----------------------------------------------------------------------------------
//------------ Kif_Automaton Section -----------------------------------------------
//----------------------------------------------------------------------------------

#define action_first 1
#define action_change 2
#define action_delete 4
#define action_insert 8
#define action_switch 16
#define action_nocase 32

const short action_suball = action_change | action_insert;

#define isaction(a) (flags & a) == a
#define isnotaction(a) (flags & a) == 0

//----------------------------------------------------------------------------------

bool compileautomaton(string intrans, string outtrans, int latintable, bool norm) {
	Kif_Automaton a;
	if (!valid_latin_table(latintable))
		return false;

	a.encoding_table = latintable;
	a.normalize = norm;
	return a.compile(intrans, outtrans);
}

bool compileAutomaton(Kif_Automaton& a, string intrans, string outtrans, int latintable, bool norm) {
	if (!valid_latin_table(latintable))
		return false;

	a.encoding_table = latintable;
	a.normalize = norm;
	return a.compile(intrans, outtrans);
}

Kif_Fst::Kif_Fst(Kif_Automaton& a) {
	id = a.garbage.size();
	a.garbage.push_back(this);
	status = 0;
}

Kif_Fst::Kif_Fst(wstring& w, wstring& lf, int posw, int posl, Kif_Automaton& a) {
	id = a.garbage.size();
	a.garbage.push_back(this);
	status = 0;
	add(w, lf, posw, posl, a);
}

void Kif_Fst::loadarcs(ifstream& dump, hmap<int, long>& allarcs, Kif_Automaton& a) {
	char buff[2];
	dump.read(buff, 1);
	status = buff[0];

	if (status & xflast)
		return;

	dump.read(buff, 1);
	arcs.nb = buff[0];

	arcs.redimension(arcs.nb + 1);

	octet4 v4;
	octet2 v2;

	unsigned short vx = 1 << 14;
	unsigned short vxx = 1 << 15;
	long val, vbuff;
	for (int i = 0; i < arcs.nb; i++) {
		dump.read(v2.b, 2);
		if ((v2.v & vxx) == vxx) {
			v2.v &= ~vxx;
			arcs.indexes[i] = allarcs[v2.v];
			dump.read(v4.b, 4);
			arcs.table[i] = a.garbage[v4.v];
		}
		else {
			if ((v2.v & vx) == vx) {
				v2.v &= ~vx;
				arcs.indexes[i] = allarcs[v2.v];
				dump.read(buff, 1);
				dump.read(v2.b, 2);
				val = v2.v;
				val <<= 8;
				vbuff = (uchar)buff[0];
				val |= vbuff;
				arcs.table[i] = a.garbage[val];
			}
			else {
				arcs.indexes[i] = allarcs[v2.v];
				dump.read(v2.b, 2);
				arcs.table[i] = a.garbage[v2.v];
			}
		}
	}
}

bool Kif_Fst::load(string name, Kif_Automaton& a) {
	ifstream dump(STR(name), modeOuverture);
	if (dump.fail())
		return false;
	octet2 v;

	//we read the signature+the table encoding... 24,9,16=XIP
	char buff[2048];
	dump.read(buff, 5);
	if (buff[0] != 24 || buff[1] != 9 || buff[2] != 16)
		return false;

	a.encoding_table = buff[3];
	a.normalize = buff[4];

	//the size
	dump.read(v.b, 2);
	int sz = v.v;
	int i;
	for (i = 1; i <= sz; i++) {
		dump.read(v.b, 2); //the string size
		dump.read(buff, v.v);
		buff[v.v] = 0;
		a.ialphabet[i] = buff;
		a.alphabet[buff] = i;
	}
	a.ialphabet[0] = "";

	a.fillencoding(true);
	octet4 vv;
	dump.read(vv.b, 4);
	//We read the number of elements...
	Kif_Fst* trans;

	int nbarcs = vv.v;
	for (i = 1; i < nbarcs; i++)
		trans = new Kif_Fst(a);

	//Nb arc labels
	dump.read(v.b, 2);
	hmap<int, long> allarcs;
	for (i = 0; i < v.v; i++) {
		dump.read(vv.b, 4);
		allarcs[i] = vv.v;
	}

	for (i = 0; i < nbarcs; i++) {
		if ((i % 5000) == 0)
			*flot_erreur << "<";
		a.garbage[i]->loadarcs(dump, allarcs, a);
	}

	*flot_erreur << endl;
	return true;
}

void Kif_Fst::savearc(ofstream& dump, hmap<long, int>& allarcs) {
	//we check first if all indexes are identities
	char buff[] = { 0, 0, 0 };

	if (arcs.nb == 0) {
		buff[0] = status | xflast;
		dump.write(buff, 1);
		return;
	}

	octet2 v2;
	octet4 v4;

	buff[0] = status; buff[1] = arcs.nb;
	dump.write(buff, 2);
	unsigned long idx;
	buff[0] = 0; buff[1] = 0;
	for (int i = 0; i < arcs.nb; i++) {
		idx = arcs.table[i]->id;
		v2.v = allarcs[arcs.indexes[i]];
		if ((idx & 0xFFFF) == idx) {
			dump.write(v2.b, 2);
			v2.v = idx;
			dump.write(v2.b, 2);
		}
		else {
			if ((idx & 0xFFFFFF) == idx) {
				buff[0] = idx & 0xFF;
				v2.v |= (1 << 14);
				dump.write(v2.b, 2);
				dump.write(buff, 1);
				v2.v = idx >> 8;
				dump.write(v2.b, 2);
			}
			else {
				v2.v |= (1 << 15);
				dump.write(v2.b, 2);
				v4.v = idx;
				dump.write(v4.b, 4);
			}
		}
	}
}

//we suppose our automaton without any loops... A pure lexicon
bool Kif_Fst::compile(string name, Kif_Automaton& a) {
	map<unsigned short, string> sorted;

	hash_bin<unsigned short, string>::iterator it;
	for (it = a.ialphabet.begin(); it != a.ialphabet.end(); it++) {
		if (it->first != 0)
			sorted[it->first] = it->second;
	}

	ofstream dump(STR(name), ios::binary);
	map<unsigned short, string>::iterator its;

	//We write the signature at the beginning of the file
	char buff[5] = { 24, 9, 16, 1, 0 }; //XIP

	if (a.encoding_table != 1)
		buff[3] = a.encoding_table;
	buff[4] = a.normalize;

	dump.write(buff, 5);

	octet2 v;
	v.v = sorted.size();
	dump.write(v.b, 2);

	for (its = sorted.begin(); its != sorted.end(); its++) {
		v.v = its->second.size();
		dump.write(v.b, 2);
		dump.write(its->second.c_str(), its->second.size());
	}

	//traverse(dump);
	//We reindex to skip the NULL...
	int idx = 0;
	int i;
	for (i = 0; i < a.garbage.size(); i++) {
		if (a.garbage[i] != NULL)
			a.garbage[i]->id = idx++;
	}

	octet4 vv;
	vv.v = idx;
	dump.write(vv.b, 4);

	int total = 0;
	hmap<long, int> allarcs;
	vector<long> varcs;
	long e;
	for (i = 0; i < a.garbage.size(); i++)  {
		if (a.garbage[i] == NULL)
			continue;

		for (idx = 0; idx < a.garbage[i]->arcs.nb; idx++) {
			e = a.garbage[i]->arcs.indexes[idx];
			if (allarcs.find(e) == allarcs.end())  {
				allarcs[e] = varcs.size();
				varcs.push_back(e);
			}
			total++;
		}
	}

	v.v = varcs.size();
	dump.write(v.b, 2);
	for (i = 0; i < varcs.size(); i++) {
		vv.v = varcs[i];
		dump.write(vv.b, 4);
	}

	for (i = 0; i < a.garbage.size(); i++) {
		if (a.garbage[i] != NULL)
			a.garbage[i]->savearc(dump, allarcs);
	}

	return true;

}
//-----------------------------------------------------------
//We add a pair of characters to the automaton...
//We conflate the character code of the lower and upper sides on one single 32 bits value...
void Kif_Fst::add(wstring& w, wstring& lf, int posw, int posl, Kif_Automaton& a) {
	unsigned long cw;

	if (posw < w.size()) {
		cw = w[posw++];
		cw = a.index(cw);
		//we merge the bits of the two characters as a key
		if (posl < lf.size()) {
			unsigned long  clf = lf[posl++];
			//the last character is the feature structure, which we do not want to reinterpret
			if (posl < lf.size())
				clf = a.index(clf);
			//the upper side is stored on the upper bits
			cw |= clf << 16;
		}
		else
			status |= xfepsilonupper;

		if (!arcs.find(cw))
			arcs[cw] = new Kif_Fst(w, lf, posw, posl, a);
		else
			arcs[cw]->add(w, lf, posw, posl, a);
		return;
	}


	status |= xfarcend;
	if (posl < lf.size()) {
		cw = lf[posl++];
		//the last character is the feature structure, which we do not want to reinterpret
		if (posl < lf.size())
			cw = a.index(cw);
		cw <<= 16;
		if (!arcs.find(cw)) {
			if (a.features.find(cw) != a.features.end())
				arcs[cw] = a.features[cw];
			else
				arcs[cw] = new Kif_Fst(w, lf, posw, posl, a);
		}
		else
			arcs[cw]->add(w, lf, posw, posl, a);
		status |= xfepsilonlower;
	}
}

static bool novel(Kif_Fst* xf, vector<Kif_Fst*>& v) {
	for (int i = 0; i < v.size(); i++) {
		if (v[i] == xf)
			return false;
	}
	v.push_back(xf);
	return true;
}

static bool comparenodes(Kif_Fst* x1, Kif_Fst* x2) {
	//All arcs should be in both elements
	if (x1 == NULL || x2 == NULL)
		return false;

	if (x1 == x2)
		return true;

	if (x1->status != x2->status)
		return false;

	if (x1->arcs.nb != x2->arcs.nb)
		return false;

	int i;
	for (i = 0; i < x1->arcs.nb; i++) {
		if (x2->arcs.find(x1->arcs.indexes[i]) == false)
			return false;
	}

	for (i = 0; i < x1->arcs.nb; i++) {
		if (!comparenodes(x1->arcs.table[i], x2->arcs[x1->arcs.indexes[i]]))
			return false;
	}

	return true;
}


static void fstsize(int& idx, Kif_Fst* xf) {
	if (xf == NULL)
		return;

	idx += xf->arcs.nb;
	for (int i = 0; i < xf->arcs.nb; i++)
		fstsize(idx, xf->arcs.table[i]);
}

static void simplify(hmap<int, vector<Kif_Fst*> >& pile, hmap<int, int>& toberemoved, bool);

//In order to remove redundant arcs, we shuffle arcs according either to their first next arc, or the second next one or their size
//The idea here is to create very small vectors of arcs, with a certain level of similarity (either according to their size or to their next arcs)
//to reduce unnecessary complexity in comparing arcs together.
static bool shuffle(vector<Kif_Fst*>& v, hmap<int, int>& toberemoved) {
	Kif_Fst* xf;
	int idx;
	vector<Kif_Fst*> ref;
	hmap<int, vector<Kif_Fst*> > subpile1;
	hmap<int, vector<Kif_Fst*> > subpile2;
	hmap<int, vector<Kif_Fst*> > subpile3;
	hmap<int, vector<Kif_Fst*> > subpile4;
	for (int i = 0; i < v.size(); i++) {
		xf = v[i];

		idx = 0;
		//we index on the number of arcs stemming from xf
		fstsize(idx, xf);
		if (subpile3.find(idx) == subpile3.end())
			subpile3[idx] = ref;
		subpile3[idx].push_back(xf);

		//we index on the next arc from the next arc
		if (xf->arcs.nb) {
			if (xf->arcs.table[0]->arcs.nb)
				idx = xf->arcs.table[0]->arcs.indexes[0];
			else
				idx = xf->arcs.indexes[0];
		}
		if (subpile4.find(idx) == subpile4.end())
			subpile4[idx] = ref;
		subpile4[idx].push_back(xf);

		//we index on the first next arc
		if (xf->arcs.nb >= 1)
			idx = xf->arcs.indexes[0];
		else
			idx = 0;

		if (subpile1.find(idx) == subpile1.end())
			subpile1[idx] = ref;
		subpile1[idx].push_back(xf);


		//we index on the second next arc
		if (xf->arcs.nb > 1)
			idx = xf->arcs.indexes[1];
		else
			idx = 0;

		if (subpile2.find(idx) == subpile2.end())
			subpile2[idx] = ref;
		subpile2[idx].push_back(xf);
	}

	//we then detect the largest number of vectors
	//since this value is usually correlated with smaller vectors inside...
	idx = max(subpile1.size(), subpile3.size());
	idx = max(idx, (int)subpile4.size());
	idx = max(idx, (int)subpile2.size());

	if (idx == 1)
		return false;

	if (idx == subpile1.size())
		simplify(subpile1, toberemoved, false);
	else {
		if (idx == subpile2.size())
			simplify(subpile2, toberemoved, false);
		else {
			if (idx == subpile3.size())
				simplify(subpile3, toberemoved, false);
			else
				simplify(subpile4, toberemoved, false);
		}
	}
	return true;
}

//simplify compares lists of arcs gathered according to their size or the ids of the next arcs.
static void simplify(hmap<int, vector<Kif_Fst*> >& pile, hmap<int, int>& toberemoved, bool top) {
	int i, n;
	Kif_Fst* xe;
	Kif_Fst* xf;
	vector<int> ref;

	hmap<int, vector<Kif_Fst*> >::iterator it;
	for (it = pile.begin(); it != pile.end(); it++) {
		if (top)
			*flot_erreur << ".";
		vector<Kif_Fst*>& v = it->second;
		if (v.size() > 1) {
			//if the size of this vector is too large
			//we shuffle its elements into smaller vectors.
			if (v.size() > 1000) {
				if (shuffle(v, toberemoved))
					continue;
			}

			for (i = 0; i < v.size(); i++) {
				xf = v[i];

				if (toberemoved.find(xf->id) != toberemoved.end())
					continue;

				for (n = i + 1; n < v.size(); n++) {
					xe = v[n];
					if (comparenodes(xf, xe))
						toberemoved[xe->id] = xf->id;
				}
			}
		}
	}
}

bool Kif_Fst::loadtext(string name, Kif_Automaton& a) {
	ifstream f(name.c_str(), ios::in | ios::binary);
	if (f.fail())
		return false;

	agnostring w;
	agnostring l;
	wstring wstr;
	wstring lstr;

	string feats;
	int compte = 0;
	int fpos = 0;

	while (!f.eof()) {
		getline(f, w);
		getline(f, l);
		w = w.trim();
		l = l.trim();
		if (w.size() && l.size()) {
			fpos = l.rfind("\t", l.size());
			if (fpos != string::npos) {
				feats = l.substr(fpos, l.size() - fpos);
				l = l.substr(0, fpos);
				fpos = a.index(feats);
				a.addfeature(fpos << 16);

				wstr = w.latintounicode(a.encoding_table);
				lstr = l.latintounicode(a.encoding_table);

				lstr += fpos;
				add(wstr, lstr, 0, 0, a);
				compte++;
				if ((compte % 10000) == 0)
					*flot_erreur << "<";
			}
		}
	}

	return factorize(a);
}


bool Kif_Fst::addmap(hmap<string, string>& lexicon, Kif_Automaton& a) {
	hmap<string, string>::iterator it;

	agnostring w;
	agnostring l;
	wstring wstr;
	wstring lstr;

	string feats;
	int compte = 0;
	int fpos = 0;

	for (it = lexicon.begin(); it != lexicon.end();it++) {
		w = it->first;
		l = it->second;
		if (w.size() && l.size()) {
			fpos = l.rfind("\t", l.size());
			if (fpos != string::npos) {
				feats = l.substr(fpos, l.size() - fpos);
				l = l.substr(0, fpos);
				fpos = a.index(feats);
				a.addfeature(fpos << 16);

				wstr = w.latintounicode(a.encoding_table);
				lstr = l.latintounicode(a.encoding_table);

				lstr += fpos;
				add(wstr, lstr, 0, 0, a);
				compte++;
				if ((compte % 10000) == 0)
					*flot_erreur << "<";
			}
		}
	}

	return factorize(a);
}


bool Kif_Fst::factorize(Kif_Automaton& a) {
	*flot_erreur << endl << a.garbage.size() << " arcs" << endl;
	int i, n;
	Kif_Fst* xf;
	hmap<int, vector<Kif_Fst*> > pile;
	vector<Kif_Fst*> ref;
	for (i = 1; i < a.garbage.size(); i++) {
		xf = a.garbage[i];
		if (xf == NULL)
			continue;
		if (pile.find(xf->arcs.nb) == pile.end())
			pile[xf->arcs.nb] = ref;
		pile[xf->arcs.nb].push_back(xf);
	}

	hmap<int, int> toberemoved;
	//For 0 size arcs, we do not need the full analysis
	if (pile.find(0) != pile.end()) {
		vector<Kif_Fst*>& v = pile[0];
		xf = v[0];
		for (i = 1; i < v.size(); i++)
			toberemoved[v[i]->id] = xf->id;

		pile[0].clear();
	}

	*flot_erreur << endl;
	simplify(pile, toberemoved, true);
	*flot_erreur << endl;

	hmap<int, int>::iterator et;
	for (et = toberemoved.begin(); et != toberemoved.end(); et++) {
		//First we mark the heads...
		a.garbage[et->second]->status |= xfmark;
	}

	a.start.status |= xfmark;

	Kif_Fst* xe;
	//We have identified with simplify the arcs, which are similar. For each of these arcs, we have kept one as
	//the replacement. This procedure replaces each of these arcs with their replacement.
	//The arcs that we want to keep are marked
	for (i = 0; i < a.garbage.size(); i++) {
		xf = a.garbage[i];
		if (xf == NULL)
			continue;

		if (toberemoved.find(xf->id) != toberemoved.end())
			continue;

		for (n = 0; n < xf->arcs.nb; n++) {
			xe = xf->arcs.table[n];
			if (toberemoved.find(xe->id) != toberemoved.end())
				xf->arcs.table[n] = a.garbage[toberemoved[xe->id]];
			else
				xe->status |= xfmark;
		}
	}

	//we then delete the unmarked arcs
	n = 0;
	for (i = 0; i < a.garbage.size(); i++) {
		xf = a.garbage[i];
		if (xf == NULL)
			continue;

		if (xf->status & xfmark) {
			xf->status &= ~xfmark;
			n++;
		}
		else {
			delete xf;
			a.garbage[i] = NULL;
		}
	}

	*flot_erreur << n << " factorized arcs remain" << endl;

	return true;
}

//-----------------------------------------------------------
bool Kif_Fst::parse(Kif_Automaton& a, vector<wstring>& vs, int& i, vector<unsigned long>& indexes) {
	if (i == vs.size())
		return true;

	int j;
	int subpos = -1;
	unsigned long cw;

	Kif_Fst* sub = NULL;
	if (vs[i] == L"(") {
		status |= xfarcend;
		subpos = a.garbage.size();
		sub = new Kif_Fst(a);
		if (!sub->parse(a, vs, ++i, indexes))
			return false;

		if (i == vs.size() || vs[i++] != L")")
			return false;

		for (j = 0; j < sub->arcs.nb; j++) {
			cw = sub->arcs.indexes[j];
			arcs.add(cw, sub->arcs.table[j]);
		}

		if (i == vs.size()) {
			delete sub;
			a.garbage[subpos] = NULL;
			return true;
		}
	}

	if (vs[i] == L")")
		return true;

	Kif_Fst* loop;
	Kif_Fst* element;

	if (vs[i] == L"!") {
		i++;
		if (i == vs.size())
			return false;
		j = vs[i][0] - 49;
		cw = indexes[j];
		cw <<= 16;
		loop = new Kif_Fst(a);
		loop->status |= xfarcend;
		status |= xfepsilonlower | xfarcend;
		arcs.add(cw, loop);
		return parse(a, vs, ++i, indexes);
	}

	if (vs[i][0] == '$') {
		j = 1;
		if (vs[i].size() == 1) {
			i++;
			if (i == vs.size())
				return false;
			j = 0;
		}

		loop = this;
		Kif_Fst* first = NULL;
		unsigned long cwfirst = 0;
		for (; j < vs[i].size(); j++) {
			cw = a.index(vs[i][j]);
			cw |= cw << 16;
			element = new Kif_Fst(a);
			if (first == NULL) {
				first = element;
				cwfirst = cw;
			}
			loop->arcs.add(cw, element);
			loop = element;
		}


		if (sub != NULL && first != NULL) {
			for (int e = 0; e < sub->arcs.nb; e++) {
				element = arcs[sub->arcs.indexes[e]];
				element->arcs[cwfirst] = first;
			}
			delete sub;
			a.garbage[subpos] = NULL;
		}

		return loop->parse(a, vs, ++i, indexes);
	}

	if (vs[i][0] == '%') {
		//the character should be next...
		if (vs[i].size() == 1) {
			i++;
			if (i == vs.size())
				return false;

			cw = a.index(vs[i][0]);
		}
		else
			cw = a.index(vs[i][1]);

		cw |= cw << 16;
		loop = new Kif_Fst(a);
		if (sub != NULL) {
			for (j = 0; j < sub->arcs.nb; j++) {
				element = arcs[sub->arcs.indexes[j]];
				element->arcs.add(cw, loop);
			}
			delete sub;
			a.garbage[subpos] = NULL;
		}

		arcs.add(cw, loop);
		return loop->parse(a, vs, ++i, indexes);
	}

	if (vs[i] == L"{") {
		i++;
		if (i == vs.size())
			return false;
		vector<Kif_Fst*> vl;
		vector<int> gloop;
		while (vs[i] != L"}") {
			j = 0;
			vector<wstring> vsub;
			vsub.push_back(L"$" + vs[i]);
			gloop.push_back(a.garbage.size());
			loop = new Kif_Fst(a);
			loop->parse(a, vsub, j, indexes);
			vl.push_back(loop);
			i++;
			if (i == vs.size())
				return false;
		}

		bool reloop = false;
		if ((i + 1) < vs.size() && vs[i + 1] == L"+") {
			i++;
			reloop = true;
		}

		if (vl.size()) {
			loop = vl[0];
			//we are looking for the last arc at the end of this structure			
			while (loop->arcs.size() != 0)
				loop = loop->arcs.table[0];
			//we then parse from the end of this state
			if (!loop->parse(a, vs, ++i, indexes))
				return false;

			if (reloop)
				//we need to add a loop to go back to this currrent arc...
				loop->arcs.add(0, this);

			if (loop->arcs.nb) {
				cw = loop->arcs.indexes[0];
				for (j = 1; j < vl.size(); j++) {
					element = vl[j];
					while (element->arcs.size() != 0)
						element = element->arcs.table[0];
					element->status |= loop->status;
					element->arcs.add(cw, loop->arcs[cw]);
					if (reloop)
						element->arcs.add(0, this);
				}
			}
			for (j = 0; j < vl.size(); j++) {
				loop = vl[j];
				if (loop->arcs.nb) {
					cw = loop->arcs.indexes[0];
					arcs.add(cw, loop->arcs.table[0]);
					if (sub != NULL) {
						for (int e = 0; e < sub->arcs.nb; e++) {
							element = arcs[sub->arcs.indexes[e]];
							element->arcs.add(cw, loop);
						}
					}
				}
				delete loop;
				a.garbage[gloop[j]] = NULL;
			}
		}

		if (sub != NULL) {
			delete sub;
			a.garbage[subpos] = NULL;
		}
		return true;
	}

	if (vs[i] == L"[") {
		i++;
		if (i == vs.size())
			return false;
		vector<unsigned long> vl;
		while (i<vs.size() && vs[i] != L"]") {
			if (vs[i].size() == 3 && vs[i][1] == '-') {
				wchar_t c = vs[i][0];
				wchar_t f = vs[i][2];
				if (c > f)
					return false;
				while (c <= f) {
					cw = a.index(c);
					cw |= cw << 16;
					vl.push_back(cw);
					c++;
				}
				i++;
			}
			else {
				if (vs[i].size() != 1)
					return false;

				if ((i + 1) < vs.size() && vs[i + 1] == L"-") {
					if ((i + 2) >= vs.size() || vs[i + 2].size() != 1)
						return false;

					wchar_t c = vs[i][0];
					wchar_t f = vs[i + 2][0];
					if (c > f)
						return false;
					while (c <= f) {
						cw = a.index(c);
						cw |= cw << 16;
						vl.push_back(cw);
						c++;
					}
					i += 3;
				}
				else {
					cw = a.index(vs[i][0]);
					cw |= cw << 16;
					vl.push_back(cw);
					i++;
				}
			}
		}
	
		if (i == vs.size())
			return false;

		loop = new Kif_Fst(a);

		if ((i + 1) < vs.size() && vs[i + 1] == L"+") {
			i++;
			for (j = 0; j < vl.size(); j++)
				loop->arcs.add(vl[j], loop);
		}

		for (j = 0; j < vl.size(); j++) {
			cw = vl[j];
			arcs.add(cw, loop);

			if (sub != NULL) {
				for (int e = 0; e < sub->arcs.nb; e++) {
					element = arcs[sub->arcs.indexes[e]];
					element->arcs.add(cw, loop);
				}
			}
		}

		if (sub != NULL) {
			delete sub;
			a.garbage[subpos] = NULL;
		}
		return loop->parse(a, vs, ++i, indexes);
	}
	return false;
}

bool Kif_Fst::parse(Kif_Automaton& a, agnostring& expression, vector<unsigned long>& indexes) {
	vector<wstring> vs;
	expression.tokenize(vs, false, false, true);
	int i = 0;
	int ig = a.garbage.size();
	Kif_Fst* xf = new Kif_Fst(a);
	if (!xf->parse(a, vs, i, indexes))
		return false;

	arcs.resize(arcs.sz + xf->arcs.nb + 1);
	status |= xf->status;

	for (int i = 0; i < xf->arcs.nb; i++)
		arcs.add(xf->arcs.indexes[i], xf->arcs.table[i]);

	factorize(a);
	return true;
}

void Kif_Fst::regulars(Kif_Automaton& a) {
	//Our features
	vector<unsigned long> indexes;

	string e = "\t+Dig+Card";
	indexes.push_back(a.index(e));
	e = "\t+Dig+Dec";
	indexes.push_back(a.index(e));
	e = "\t+Exp+Dec";
	indexes.push_back(a.index(e));
	e = "\t+Dig+Ord";
	indexes.push_back(a.index(e));
	e = "\t+Dig+Rom";
	indexes.push_back(a.index(e));

	agnostring expression("([- +])[0 1 2 3 4 5 6 7 8 9]+!1(%.[0 1 2 3 4 5 6 7 8 9]+!2([e E]([- +])[0 1 2 3 4 5 6 7 8 9]+!3))");
	parse(a, expression, indexes);

	expression = "[D M C L X V I]+!5";
	parse(a, expression, indexes);

	expression = "{1st 2nd 3rd}!4";
	parse(a, expression, indexes);

	expression = "[0 1 2 3 4 5 6 7 8 9]([0 1 2 3 4 5 6 7 8 9]+)$th!4";
	parse(a, expression, indexes);
}

//----------------------------------TOKENIZE AND MORPHO-ANALYSYS IN ONE SINGE STEP----------------
bool Kif_Automaton::process(charRead& w, vector<string>& readings, bool parseur) {
	if (!finalize) {
		fillencoding(true);
		start.shuffle();
	}

	string s;
	w.init(w.w);
	w.encoding_table = encoding_table;

	if (start.vprocess(w, s, readings, *this)) {
		if (readings.size() > 1) {
			for (int i = readings.size() - 1; i >= 0; i--) {
				if (w.bends[i] < w.bend)
					readings.erase(readings.begin() + i);
			}
		}

		w.setpos(w.bend, w.cend);
		if (w.addoffsets) {
			agnostring surface;
			surface = w.extract();
			if (surface.trim() == "")
				return false;
			agnostring feat;
			int ps;
			string off = w.offsets();
			if (!readings.size()) {
				readings.push_back(surface);
				surface += "\t" + off;
				readings.push_back(surface);
			}
			else {
				surface = surface.latintoutf8(encoding_table);
				for (int i = 0; i < readings.size(); i++) {
					feat = readings[i];
					ps = feat.find("\t", 0);
					if (ps == string::npos) {
						feat = feat.latintoutf8(encoding_table);
						feat += "\t" + off;
						readings[i] = feat;
						continue;
					}

					if (feat.find("\t", ps + 1) != string::npos)
						feat = feat.replace("\t", "");
					feat.insert(off, ps + 1);
					readings[i] = feat.latintoutf8(encoding_table);
				}
				readings.insert(readings.begin(), surface);
			}
		}
		return true;
	}

	//If we are calling this process from GlobalParseurTrans, then we might have other lexicons to check against
	if (parseur)
		return false;

	//Otherwise this is a call from a kif program

	//No value found....
	w.setpos(w.bbegin, w.cbegin);
	w.eset(w.bbegin, w.cbegin);

	BULONG cr = w.nextcode();
	if (c_is_punctuation(cr)) {
		w.eset(w.w);
		s = w.extract();
		s += "\t";
		if (w.addoffsets) {
			string off = w.offsets();
			s += off;
		}
		s += "+PUNCT";
	}
	else {
		if (w.end())
			return false;

		while (!w.end() && !c_is_space(cr) && !c_is_punctuation(cr)) {
			w.eset(w.w);
			cr = w.nextcode();
		}
		if (!w.end())
			w.setpos(w.bend, w.cend);

		s = w.extract();
		if (s == "")
			return false;

		s += "\t";
		if (w.addoffsets) {
			string off = w.offsets();
			s += off;
		}
		s += "+Unknown+NOUN";
	}
	readings.push_back(s);
	return true;
}

bool Kif_Fst::process(charRead& w, string& s, vector<string>& res, bool punct, Kif_Automaton& a) {
	BULONG cr;
	unsigned long cw, u;
	bool endtoken = false;
	bool found = false;
	int bpos, cpos, ubpos, ucpos;
	int i;

	string v;

	if (w.end())
		endtoken = true;
	else {
		ubpos = w.bytepos();
		ucpos = w.charpos();
		cr = w.nextcode();
		bpos = w.bytepos();
		cpos = w.charpos();

		cw = a.code(cr);

		if (c_is_space(cr) || c_is_punctuation(cr) || punct) {
			//we need then to check if we can carry on with arcs...
			if (s != "") {
				if (!cw && c_is_punctuation(cr)) {
					//In this case, we try to convert it to a knowledgable one
					cr = c_unicode_to_latin(cr);
					if (cr != 0)
						cw = a.code(cr);
				}

				if (cw) {
					i = -1;
					while (arcs.checkup(cw, i)) {
						if (!(arcs.indexes[i] & 0xFFFF))
							w.setpos(ubpos, ucpos);

						u = arcs.code(i);
						if (u) {
							v = s + a.ialphabet[u];
							if (arcs.found(i)->process(w, v, res, punct, a))
								found = true;
						}
						else {
							if (arcs.found(i)->process(w, s, res, punct, a))
								found = true;
						}
						w.setpos(bpos, cpos);
					}

					if (found)
						return true;
				}

				//In this case, the character does not belong to the string
				//We try to analyze it...
				w.setpos(ubpos, ucpos);
				endtoken = true;
			}
			else {
				if (c_is_punctuation(cr))
					punct = true;
			}
		}
	}

	if (endtoken) {
		if (status&xfarcend) {
			if (!finals(s, res, a))
				return false;
			w.eset(w.w);
			return true;
		}
		return false;
	}

	if (!arcs.nb)
		return false;

	i = -1;
	if (cw) {
		while (arcs.checkup(cw, i)) {
			if (!(arcs.indexes[i] & 0xFFFF))
				w.setpos(ubpos, ucpos);

			u = arcs.code(i);
			if (u) {
				v = s + a.ialphabet[u];
				if (arcs.found(i)->process(w, v, res, punct, a))
					found = true;
			}
			else {
				if (arcs.found(i)->process(w, s, res, punct, a))
					found = true;
			}
			w.setpos(bpos, cpos);
		}

		if (a.normalize && c_is_upper(cr)) {
			unsigned long cwl = a.code(c_to_lower(cr));
			i = -1;
			while (arcs.checkup(cwl, i)) {
				//the espsilon arcs have already been covered above...
				if (!(arcs.indexes[i] & 0xFFFF))
					continue;

				u = arcs.code(i);
				if (u) {
					v = s + a.ialphabet[u];
					if (arcs.found(i)->process(w, v, res, punct, a))
						found = true;
				}
				else {
					if (arcs.found(i)->process(w, s, res, punct, a))
						found = true;
				}
				w.setpos(bpos, cpos);
			}
		}
	}
	else {
		if (punct) {
			cr = c_unicode_to_latin(cr);
			cw = a.code(cr);
			if (cw)
				return found;

			i = -1;
			while (arcs.checkup(cw, i)) {
				if (!(arcs.indexes[i] & 0xFFFF))
					w.setpos(ubpos, ucpos);

				u = arcs.code(i);
				if (u) {
					v = s + a.ialphabet[u];
					if (arcs.found(i)->process(w, v, res, punct, a))
						found = true;
				}
				else {
					if (arcs.found(i)->process(w, s, res, punct, a))
						found = true;
				}
				w.setpos(bpos, cpos);
			}
		}
	}

	return found;
}

bool Kif_Fst_Vectorized::vprocess(charRead& w, string& s, vector<string>& res, Kif_Automaton& a) {
	BULONG cr;
	unsigned long cw, u;
	bool endtoken = false;
	bool found = false, punct = false;
	int ubpos, ucpos, bpos, cpos;

	if (w.end())
		endtoken = true;
	else {
		ubpos = w.bytepos();
		ucpos = w.charpos();
		cr = w.nextcode();
		bpos = w.bytepos();
		cpos = w.charpos();

		if (c_is_space(cr)) {
			w.eset(bpos, cpos);
			return true;
		}

		cw = a.code(cr);

		if (c_is_punctuation(cr))
			punct = true;
	}

	if (endtoken) {
		if (status&xfarcend) {
			if (!finals(s, res, a))
				return false;
			w.eset(w.w);
			return true;
		}
		return false;
	}


	string v;

	int i, ia;
	if (cw) {
		//then we are in the vectorized zone...
		//Then we use the code as an entry point to the arcs...
		if (a.start.arcsv.check(cw)) {
			for (i = 0; i < a.start.arcsv[cw].size(); i++) {
				ia = a.start.arcsv[cw][i];
				u = arcs.code(ia);
				if (u) {
					v = s + a.ialphabet[u];
					if (arcs.found(ia)->process(w, v, res, punct, a))
						found = true;
				}
				else {
					if (arcs.found(ia)->process(w, s, res, punct, a))
						found = true;
				}
				w.setpos(bpos, cpos);
			}
		}

		if (a.normalize && c_is_upper(cr)) {
			unsigned long cwl = a.code(c_to_lower(cr));
			if (a.start.arcsv.check(cwl)) {
				for (i = 0; i < a.start.arcsv[cwl].size(); i++) {
					ia = a.start.arcsv[cwl][i];
					u = arcs.code(ia);
					if (u) {
						v = s + a.ialphabet[u];
						if (arcs.found(ia)->process(w, v, res, punct, a))
							found = true;
					}
					else {
						if (arcs.found(ia)->process(w, s, res, punct, a))
							found = true;
					}
					w.setpos(bpos, cpos);
				}
			}
		}
	}
	else {
		if (punct) {
			cr = c_unicode_to_latin(cr);
			cw = a.code(cr);
			if (cw) {
				if (a.start.arcsv.check(cw)) {
					for (i = 0; i < a.start.arcsv[cw].size(); i++) {
						ia = a.start.arcsv[cw][i];
						u = arcs.code(ia);
						if (u) {
							v = s + a.ialphabet[u];
							if (arcs.found(ia)->process(w, v, res, punct, a))
								found = true;
						}
						else {
							if (arcs.found(ia)->process(w, s, res, punct, a))
								found = true;
						}
						w.setpos(bpos, cpos);
					}
				}
			}
		}
	}

	if (a.start.arcsv.check(0)) {
		w.setpos(ubpos, ucpos);
		for (i = 0; i < a.start.arcsv[0].size(); i++) {
			ia = a.start.arcsv[0][i];
			u = arcs.code(ia);
			if (u) {
				v = s + a.ialphabet[u];
				if (arcs.found(ia)->process(w, v, res, punct, a))
					found = true;
			}
			else {
				if (arcs.found(ia)->process(w, s, res, punct, a))
					found = true;
			}
			w.setpos(ubpos, ucpos);
		}
	}

	return found;
}


bool Kif_Fst::finals(string& s, vector<string>& res, Kif_Automaton& a) {
	bool found = false;
	if (status&xfarcend) {
		if (status&xfepsilonlower) {
			string v;
			int i = -1;
			while (arcs.checkup(0, i)) {
				v = s + a.ialphabet[arcs.code(i)];
				if (arcs.found(i)->finals(v, res, a))
					found = true;
			}
		}
		else {
			res.push_back(s);
			return true;
		}
	}
	return found;
}
//----------------------------------------------------------------------------------------
bool Kif_Fst::up(wstring& w, int pos, string& s, vector<string>& res, Kif_Automaton& a, int threshold, short flags) {
	string v;

	if (w.size() == pos) {
		if (status&xfarcend) {
			if (status&xfepsilonlower) {
				string v;
				for (int i = 0; i < arcs.nb; i++) {
					//It should not be a transducer arc....
					if (arcs.indexes[i] & 0xFFFF)
						continue;
					v = s + a.ialphabet[arcs.code(i)];
					arcs.found(i)->up(w, pos, v, res, a, threshold, flags);
				}
			}
			else {
				int ps = s.find("\t", 0);
				if (ps != string::npos) {
					v = s;
					v = s_replacestrings(v, "\t", "");
					v.insert(ps, "\t");
				}
				res.push_back(v);
			}
			return true;
		}
		return false;
	}

	int current;
	unsigned long cr = w[pos];
	unsigned long cw = a.code(cr);

	unsigned long u;

	bool found = false;
	int i = -1;
	while (arcs.checkup(cw, i)) {
		if (!(arcs.indexes[i] & 0xFFFF))
			current = pos;
		else
			current = pos + 1;

		u = arcs.code(i);
		if (u) {
			v = s + a.ialphabet[u];
			if (arcs.found(i)->up(w, current, v, res, a, threshold, flags))
				found = true;
		}
		else {
			if (arcs.found(i)->up(w, current, s, res, a, threshold, flags))
				found = true;
		}
	}


	if (flags && threshold) {
		//if we have both flags and threshold
		//we have four cases: action_change, action_delete, action_insert, action_switch
		if (pos == 0 && isnotaction(action_first))
			return found;

		if (isaction(action_delete)) {
			if (up(w, pos + 1, s, res, a, threshold - 1, flags))
				found = true;
		}

		if (isaction(action_switch) && (pos + 1) < w.size()) {
			//we switch the two characters...
			wchar_t c = w[pos];
			w[pos] = w[pos + 1];
			w[pos + 1] = c;

			if (up(w, pos, s, res, a, threshold - 1, flags))
				found = true;

			w[pos + 1] = w[pos];
			w[pos] = c;
		}

		if (isaction(action_nocase) && c_is_upper(cr)) {
			w[pos] = c_to_lower(cr);
			if (up(w, pos, s, res, a, threshold - 1, flags))
				found = true;
			w[pos] = cr;
		}

		if (flags & action_suball) {
			//in that case, we do not try our current character, we accept any others
			for (i = 0; i < arcs.nb; i++) {
				u = arcs.indexes[i] & 0xFFFF;
				if (!u)
					continue;

				if (isaction(action_insert)) {
					u = arcs.code(i);
					if (u) {
						v = s + a.ialphabet[u];
						if (arcs.found(i)->up(w, pos, v, res, a, threshold - 1, flags))
							found = true;
					}
				}

				if (isaction(action_change)) {
					if (u == cw)
						continue;

					u = arcs.code(i);
					if (u) {
						v = s + a.ialphabet[u];
						if (arcs.found(i)->up(w, pos + 1, v, res, a, threshold - 1, flags))
							found = true;
					}
				}
			}
		}
	}

	return found;
}

bool Kif_Fst::down(vector<unsigned short>& w, int pos, string& s, vector<string>& res, Kif_Automaton& a) {
	if (pos == w.size()) {
		if (status & xfarcend) {
			res.push_back(s);
			return true;
		}
		return false;
	}

	bool found = false;
	unsigned long car, code;
	int nxt;
	string v;
	for (int i = 0; i < arcs.nb; i++) {
		code = arcs.code(i);
		car = arcs.character(i);
		if (code) {
			if (code == w[pos]) {
				nxt = pos + 1;
				if ((status & xfarcend) && (status&xfepsilonlower) && !car) {
					if (arcs.found(i)->down(w, nxt, s, res, a)) {
						found = true;
						continue;
					}
				}
			}
			else
				continue;
		}
		else
			nxt = pos;

		if (car) {
			v = s + a.ialphabet[car];
			if (arcs.found(i)->down(w, nxt, v, res, a))
				found = true;
		}
		else {
			if (arcs.found(i)->down(w, nxt, s, res, a))
				found = true;
		}
	}

	return found;
}

bool Kif_Automaton::down(wstring& w, vector<string>& res) {
	if (!finalize)
		fillencoding(true);

	//first we detect the features...
	int ps = w.find(L"\t", 0);
	if (ps == wstring::npos)
		return false;
	//we need a tab to cut at the feature level...
	vector<unsigned short> v;
	int i;
	for (i = 0; i < ps; i++)
		v.push_back(encoding[w[i]]);

	int fpos;
	for (i = 0; i < sortedmultis.size(); i++) {
		fpos = w.find(sortedmultis[i], ps);
		if (fpos != wstring::npos) {
			w = s_replacestring(w, sortedmultis[i], L"&");
			w[fpos] = multis[sortedmultis[i]];
		}
	}

	for (i = ps + 1; i < w.size(); i++)
		v.push_back((unsigned short)w[i]);

	res.clear();
	string s;
	return start.down(v, 0, s, res, *this);
}


bool CompareStrings(wstring s1, wstring s2) {
	if (s1.size() > s2.size())
		return(true);
	return(false);
}

void Kif_Automaton::fillencoding(bool add) {
	finalize = true;
	hmap<string, unsigned short>::iterator it;
	for (it = alphabet.begin(); it != alphabet.end(); it++) {
		agnostring s(it->first);
		if (s.sizec() == 1) {
			ushort p = s.nextcode();
			encoding[p] = it->second;
			decoding[it->second] = p;
		}
		else {
			if (add) {
				wstring u = s.latintounicode(encoding_table);
				if (it->first.find("\t") == string::npos) {
					multis[u] = it->second;
					sortedmultis.push_back(u);
					ialphabet[it->second] = "\t" + it->first;
				}
			}
		}
	}
	sort(sortedmultis.begin(), sortedmultis.end(), CompareStrings);
}
