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
filename   : traitement.cxx
Date       : 10/01/1999
Purpose    : Main treatments and routing according to rules. Final result generation
Programmer : Claude ROUX
Reviewer   :
*/


#if defined(WIN32) && !defined(_WINDLL)
#include <windows.h>
#endif

#include "parsbin.h"
#include <map>
#include <algorithm>
#include "kif.h"
#include "conversion.h"

#ifdef CRFTAGGER
extern "C" {
	int TaggingTokens(void* tg, char** wordSequence, char** predictedTag, int nb);
	void CleanTags(char** tags, int lp);
}
#endif

extern string choix_ambigue;
void kif_garbage_xip();

#if defined(WIN32) && !defined(_WINDLL)
extern char AFFICHAGEDOS;
char* ConversionChaine(GlobalParseur* parseur, char* chaine) {
	if (AFFICHAGEDOS == 0) {
		if (parseur != NULL && parseur->TestOptions(AFFICHAGE_UTF8))
			return (char*)parseur->conversionAsciiVersUTF8((uchar*)chaine);
		return chaine;
	}
	static char buffer[1000];
	int l = strlen(chaine);
	if (l >= 1000)
		l = 999;
	CharToOemBuff(chaine, buffer, l);
	buffer[l] = 0;
	return buffer;
}
#else
char* ConversionChaine(GlobalParseur* parseur, char* chaine) {
	if (parseur != NULL && parseur->TestOptions(AFFICHAGE_UTF8))
		return (char*)parseur->conversionAsciiVersUTF8((uchar*)chaine);
	return chaine;
}
#endif

#include "affiche.h"
//extern "C" {
//    #include "guess_lib.h"  /* for RXRC Language Guesser API   */
//}
void Trim(char* chaine);
extern char affiche_regle_appliquee;
extern VecteurDeParseurs gVecteurParseurs;
extern GlobalParseur* parseur_courant;
char calcultemps;
time_t tempsbasephrase;
int nb_dependances = 0;
char* TV(GlobalParseur* parseur, Vecteur* v);
char* TC(GlobalParseur* parseur, Traits* tr);
extern ostream* flot_erreur;
char modeOutC = NON;

/*-------------------------------------------------------------------------*/

int CompteConstruitForme = 0;

char TestChiffre(unsigned char* forme) {
	int i = 0;
	if (forme == NULL)
		return NON;
	while (forme[i]) {
		if (forme[i]<'0' || forme[i]>'9')
			return NON;
		i++;
	}
	return OUI;
}



uchar* minuscule(unsigned char* c) {

	for (int i = 0; c[i] != 0; i++) {
		if (isupper(c[i]))
			c[i] = tolower(c[i]);
	}
	return c;
}


char GlobalParseur::TestBalise(char* entree) {

	char* attribut = entree + 1;
	char* fin;

	char ajout = NON;

	char premcar = entree[0];
	premcar = tolower(premcar);

	if (strchr(PremierCaractereBalise, premcar)) {
		fin = strchr(attribut, '\t');
		if (fin == NULL)
			return NON;

		*fin = 0;
		string mot = entree;
		minuscule(USTR(mot));
		*fin = '\t';
		etat* a = LesBalises.recherche(STR(mot));
		if (a != NULL) {
			auto_vocabulaire* trad = (auto_vocabulaire*)a->valeur;
			if (trad != NULL) {
				AffecteBalise(trad);
				return OUI;
			}
		}
	}

	return NON;
}


string rulespace::chaine(string& chemin) {
	if (pere == NULL)
		chemin = nom;
	else {
		pere->chaine(chemin);
		chemin += ".";
		chemin += nom;
	}
	return chemin;
}

char GlobalParseur::testEspaceRegle(baseregle* b) {
	if (b->testprobabilite(TestOptions(ANALYSE_ALEATOIRE)) == NON)
		return NON;

	rulespace* r = b->label->space;
	if (r != NULL)
		return r->actif;
	return OUI;
}

char GlobalParseur::proba_metpoids(int id, float v) {
	baseregle* br = VBases[id];
	if (br == NULL)
		return NON;
	br->poids = v;
	return OUI;
}

char GlobalParseur::proba_metseuil(int id, float v){
	baseregle* br = VBases[id];
	if (br == NULL)
		return NON;
	br->seuil = v;
	return OUI;
}
char GlobalParseur::proba_metvaleur(int id, float v) {
	baseregle* br = VBases[id];
	if (br == NULL)
		return NON;
	br->valeur = v;
	return OUI;
}

float GlobalParseur::proba_poids(int id) {
	baseregle* br = VBases[id];
	if (br == NULL)
		return 0;
	return br->poids;
}

float GlobalParseur::proba_seuil(int id) {
	baseregle* br = VBases[id];
	if (br == NULL)
		return 0;
	return br->seuil;
}

float GlobalParseur::proba_valeur(int id) {
	baseregle* br = VBases[id];
	if (br == NULL)
		return 0;
	return br->valeur;
}

char baseregle::activationmin(GlobalParseur* parseur) {
	if (identificateur >= parseur->Limite_Numero_Regle)
		return NON;

	return OUI;
}

baseregle::baseregle() {
	poids = 1;
	seuil = 0;
	valeur = 1;
	numero = -1;
	identificateur = -1;
	couche = -1;
	label = NULL;
	compteur = 0;
}

char baseregle::activation(GlobalParseur* parseur) {
	if (identificateur != -1 && identificateur >= parseur->Limite_Numero_Regle || parseur->NumeroOrdre() == OUI)
		return NON;
	return OUI;
}

char baseregle::calculaleatoire() {
	valeur = rand() % 100;
	valeur *= poids;
	return OUI;
}

char baseregle::testprobabilite(char aleatoire) {
	if (valeur<seuil)
		return NON;
	return OUI;
}


void GlobalParseur::activation(rulespace* r, char type) {
	if (r == NULL)
		return;
	r->actif = type;
	for (int i = 0; i<r->fils.dernier; i++)
		activation(r->fils[i], type);
}


void GlobalParseur::CategorySplitProcessing(char* start) {
	if (enregListeTags.dernier<2)
		return;

	if (DECOUPAGE != NULL && !strcmp(DECOUPAGE, start)) {
		nb_lectures++;
		if (readingposition == -1)
			readingposition = enregListeTags.dernier;
	}
}

void GlobalParseur::ListeTagsRAZ(char* reading_string, char** duplicat) {
	enregListeTags.dernier = 0;
	readingposition = -1;
	nb_lectures = 0;
	*duplicat = DupliqueChaine(reading_string);
}

void GlobalParseur::HMMInsertionProcessing() {
	if (readingposition != -1) {
		//We insert all +HMM feature strings to the end of the string, in order to keep them
		//alive in case of a sub-tokenisation
		int j = enregListeTags.dernier - 1;
		while (!memcmp(enregListeTags[j], "hmm", 3)) {
			char* v = new char[taille_mot];
			strcpy(v, enregListeTags[j]);
			enregListeTags.insertion(readingposition, v);
			enregListeTags.dernier--;
		}
	}
}

void GlobalParseur::activationpere(rulespace* r) {
	if (r == NULL)
		return;
	r->actif = OUI;
	activationpere(r->pere);
}

char GlobalParseur::desactiveEspaceRegle(char* nom) {
	rulespace* r = regleEspaceRegle(nom);
	if (r == NULL)
		return NON;

	//on descend dans la hierarchie et on desactive toutes les regles		
	activation(r, NON);
	//la racine ne peut pas etre desactivee
	spacename->actif = OUI;
	return OUI;
}

char GlobalParseur::reactiveEspaceRegle(char* nom, char full) {
	rulespace* r = regleEspaceRegle(nom);
	if (r == NULL)
		return NON;
	activation(r, OUI);
	if (full == OUI)
		activationpere(r->pere);
	return OUI;
}

rulespace* GlobalParseur::regleEspaceRegle(char* sname) {
	if (spacename == NULL)
		return NULL;
	char* s = sname;
	rulespace* rst = spacename;
	char* p = strchr(s, '.');
	if (p != NULL)
		*p = 0;
	if (rst->nom != s)
		return NULL;
	if (p == NULL)
		return rst;

	s = p + 1;
	while (s != NULL) {
		p = strchr(s, '.');
		if (p != NULL)
			*p = 0;
		char trouve = NON;
		for (int i = 0; i<rst->fils.dernier; i++) {
			if (rst->fils[i]->nom == s) {
				trouve = OUI;
				rst = rst->fils[i];
				break;
			}
		}
		if (trouve == NON)
			return NULL;
		if (p == NULL)
			return rst;
		s = p + 1;
	}
	return NULL;
}

auto_vocabulaire* GlobalParseur::RechercheBalise(char* entree) {

	char* attribut = entree + 1;
	char* fin;


	char ajout = NON;

	char premcar = entree[0];
	premcar = tolower((uchar)premcar);

	if (strchr(PremierCaractereBalise, premcar)) {
		fin = strchr(attribut, '\t');
		if (fin == NULL)
			return NON;

		*fin = 0;
		string mot = entree;
		minuscule(USTR(mot));
		*fin = '\t';
		etat* a = LesBalises.recherche(STR(mot));
		if (a != NULL) {
			auto_vocabulaire* trad = (auto_vocabulaire*)a->valeur;
			//Balise de demarrage
			if (trad != NULL)
				return trad;

		}
	}

	return NULL;
}

void GlobalParseur::AffecteBalise(auto_vocabulaire* trad) {
	int i;
	ListeVoc* lv;

	switch (trad->ecraser) {
	case BALISEDEBUT:
		gBaliseTraits->ou(trad->tr);
		if (trad->initiaux != NULL)
			gBaliseDebut->ou(trad->initiaux);
		break;
	case BALISEFIN:
		gBaliseTraits->etnon(trad->tr);
		gBaliseDebut->raz();
		//Le dernier mot est initialisee avec ce trait
		if (trad->initiaux != NULL) {
			lv = phrase.fin();
			if (lv != NULL)
			for (i = 0; i<lv->tr.dernier; i++) {
				if (lv->tr[i] != NULL)
					lv->tr[i]->ou(trad->initiaux);
			}
		}
		break;
	case BALISECOMMUTATEUR:
		gBaliseTraits->xou(trad->tr);
		if (trad->initiaux != NULL)
			gBaliseDebut->ou(trad->initiaux);
		trad->ecraser = BALISECOMMUTATEURFERMEE;
		break;
	case BALISECOMMUTATEURFERMEE:
		gBaliseDebut->raz();
		gBaliseTraits->xou(trad->tr);
		trad->ecraser = BALISECOMMUTATEUR;
		//Le dernier mot est initialisee avec ce trait
		if (trad->initiaux != NULL) {
			lv = phrase.fin();
			if (lv != NULL)
			for (i = 0; i<lv->tr.dernier; i++) {
				if (lv->tr[i] != NULL)
					lv->tr[i]->ou(trad->initiaux);
			}
		}
		break;
	case BALISEDEBUTSF:
		gBaliseSansFin->ou(trad->tr);
		if (trad->initiaux != NULL)
			gBaliseDebut->ou(trad->initiaux);
		break;
	case BALISEFINSF:
		gBaliseSansFin->etnon(trad->tr);
		gBaliseDebut->razp();
		//Le dernier mot est initialisee avec ce trait
		if (trad->initiaux != NULL) {
			lv = phrase.fin();
			if (lv != NULL)
			for (i = 0; i<lv->tr.dernier; i++) {
				if (lv->tr[i] != NULL)
					lv->tr[i]->ou(trad->initiaux);
			}
		}
		break;
	case BALISECOMMUTATEURSF:
		gBaliseSansFin->xou(trad->tr);
		if (trad->initiaux != NULL)
			gBaliseDebut->ou(trad->initiaux);
		trad->ecraser = BALISECOMMUTATEURFERMEESF;
		break;
	case BALISECOMMUTATEURFERMEESF:
		gBaliseDebut->razp();
		gBaliseSansFin->xou(trad->tr);
		trad->ecraser = BALISECOMMUTATEURSF;
		//Le dernier mot est initialisee avec ce trait
		if (trad->initiaux != NULL) {
			lv = phrase.fin();
			if (lv != NULL)
			for (i = 0; i<lv->tr.dernier; i++) {
				if (lv->tr[i] != NULL)
					lv->tr[i]->ou(trad->initiaux);
			}
		}
	}
}

char GlobalParseur::TestLexique(auto_vocabulaire* trad, char ajoute_lecture, int dernierelement) {
	char typeRetour = OUI;

	while (trad != NULL) {
		while (trad != NULL) {
			//si on n'a pas besoin de tester les traits, 
			//on sort en cas de meme categorie                        
			if (trad->X == &CatVide->X ||
				trad->X == vlex.X) {
				if ((ajoute_lecture == NON || lemmesDejaLus.cherche(trad) == -1) &&
					(trad->traits_mot == NULL ||
					ITR(this, trad->traits_mot, vlex.initiaux, NONECRIT)))
					break;
			}

			//Plusieurs mots peuvent partager le meme lemme...
			trad = trad->Autre();
		}

		if (trad != NULL) {
			if (trad->gauche != NULL) {
				static VECTA<ListeVoc*> pile;
				static VECTA<Traits*> piletraits;
				pile.raz();
				piletraits.raz();
				char solution = NON;
				int coupe = dernierelement;
				EtatArbre* arb = trad->gauche->regle;
				//if ( (arb->arcs[0]->X->Val & FusionX) == 0)
				//    continue;
				initIteration();
				arb->Decoupe(this, NULL, solution, phrase, coupe, -1, pile, piletraits, coupe, NON, NULL);
				if (solution == NON) {
					trad = trad->Autre();
					continue;
				}
			}
			if (trad->ecraser != NOUVEL && trad->ecraser != UNEFOISNOUVEAU) {
				//si ce n'est pas les memes categories, 
				//on nettoie les traits en question
				if (trad->ecraser == ECRASER || trad->ecraser == ECRASERTOUT) {
					if (vlex.initiaux != NULL)
						vlex.initiaux->raz();

					//autre categorie
					vlex.X = trad->XNouveau;
					//Dans le cas ou on est dans une situation de remplacement total
					//le type de retour passe a 2...
					if (trad->ecraser == ECRASERTOUT)
						typeRetour = REMPLACEMENT;
				}

				//on ajoute les nouveaux traits...
				if (trad->tr != NULL) {
					InstancieTraits(vlex.initiaux, trad->tr);
					if ((trad->tr)->lemme != NULL &&
						(trad->tr)->maniplemme == ECRASE)
						(trad->tr)->lemme->mot(vlex.lemme, 2048);

					if ((trad->tr)->surface != NULL &&
						(trad->tr)->manipsurface == ECRASE)
						(trad->tr)->surface->mot(vlex.surface, 2048);
				}
			}
			else {
				if (ajoute_lecture == OUI) {
					nouvellesLectures.ajoute(trad);
					if (trad->ecraser == UNEFOISNOUVEAU)
						lemmesDejaLus.ajoute(trad);
				}
			}
			//On passe a l'interpretation suivante
			trad = trad->Autre();
		}
	}
	return typeRetour;
}



void RemplaceLaChaine(char* buffer, char* chaine, const char c, const char* remplace) {
	char* ch = chaine;

	char* pt = strchr(ch, c);
	buffer[0] = 0;

	while (pt != NULL) {
		int ll = pt - ch;
		memcpy(buffer, ch, ll);
		strcpy(buffer + ll, remplace);
		buffer += strlen(remplace) + ll;
		ch = pt + 1;
		pt = strchr(ch, c);
	}
	strcat(buffer, ch);
}

void GlobalParseur::RechercheVocabulaire(VECTA<void*>& a, int laCouche, char* lemme, char* surface) {
	auto_vocabulaire* auv = NULL;

	if (ArbSurface[laCouche] != NULL)
		ArbSurface[laCouche]->recherchetouslabels(surface, a);

	if (ArbSurfaceComplexe[laCouche] != NULL) {
		ArbSurfaceComplexe[laCouche]->recherchetouslabels(surface, a);
		auv = (auto_vocabulaire*)a.fin();
		if (auv != NULL) {
			if (auv->tr->lemme != NULL)
				ArbSurfaceComplexe[laCouche]->extraitvariables((uchar*)surface, auv->tr->lemme);
			if (auv->tr->surface != NULL)
				ArbSurfaceComplexe[laCouche]->extraitvariables((uchar*)surface, auv->tr->surface);
		}
	}

	if (ArbVoc[laCouche] != NULL)
		ArbVoc[laCouche]->recherchetouslabels(lemme, a);

	if (ArbVocComplexe[laCouche] != NULL) {
		ArbVocComplexe[laCouche]->recherchetouslabels(lemme, a);
		auv = (auto_vocabulaire*)a.fin();
		if (auv != NULL) {
			if (auv->tr->lemme != NULL)
				ArbVocComplexe[laCouche]->extraitvariables((uchar*)lemme, auv->tr->lemme);
			if (auv->tr->surface != NULL)
				ArbVocComplexe[laCouche]->extraitvariables((uchar*)lemme, auv->tr->surface);
		}
	}
}

void GlobalParseur::ExtraitVocabulaire(VECTA<void*>& a, int laCouche, vocabulaire& v) {}


char GlobalParseur::TraitementVocabulaire(char ajoute_lecture, int laCouche, int dernierelement) {
	char typeRetour = OUI;

	VECTA<void*> a;
	auto_vocabulaire* auv = NULL;

	//ExtraitVocabulaire(a,laCouche,vlex);
	RechercheVocabulaire(a, laCouche, vlex.lemme, vlex.surface);

	for (int ia = 0; ia<a.dernier; ia++) {
		auv = (auto_vocabulaire*)a[ia];

		if (TestLexique(auv, ajoute_lecture, dernierelement) == REMPLACEMENT)
			typeRetour = REMPLACEMENT;
	}


	return typeRetour;
}

bool GlobalParseur::FeatureAnalysis(vector<string>& features, string& catog, Traits* tlocal) {

	avl_vocabulaire* trad = NULL;

	//chaque trait est separe du suivant par un '_'
	//La chaine a gauche correspond a une categorie 
	//sinon c'est une erreur...

	int i;
	Cat* Xx = NULL;

	string err;
	for (i = 0; i<features.size(); i++) {
		//on recherche d'abord la categorie
		size_t indexMot = CalculClefHachageTout(USTR(features[i]));
		trad = NULL;
		if (Traductions[indexMot] != NULL)
			trad = Traductions[indexMot]->recherchecasse(STR(features[i]));

		//categorie a sauter
		if (trad == NULL || trad->tr == NULL)
			continue;

		if (Xx == NULL)
			Xx = trad->X;

		tlocal->ou(trad->tr);
	}

	//Il peut y avoir aussi des creations de traits bases sur un test
	for (i = 0; i<definirtraits.dernier; i++) {
		if (ITR(this, definirtraits[i]->traitstest, tlocal, NONECRIT)) {
			tlocal->ou(definirtraits[i]->traitsaffecter);
		}
	}

	if (Xx != NULL)
		catog = Xx->nom;

	return true;
}

Cat* GlobalParseur::AnalyseTrait(string& feat, Traits* tlocal) {

	avl_vocabulaire* trad = NULL;

	//chaque trait est separe du suivant par un '_'
	//La chaine a gauche correspond a une categorie 
	//sinon c'est une erreur...

	int i;
	Cat* Xx = NULL;
	int ln = feat.size();
	unsigned char* feature = new unsigned char[ln + 1];
	for (i = 0; i<ln; i++)
		feature[i] = feat[i];
	feature[i] = 0;

	if (!TestOptions(NON_NORMALISATION_TAG))
		MetEnMinuscule(feature);

	//on recherche d'abord la categorie
	size_t indexMot = CalculClefHachageTout(feature);
	trad = NULL;
	if (Traductions[indexMot] != NULL)
		trad = Traductions[indexMot]->recherchecasse((char*)feature);

	delete[] feature;

	//categorie a sauter
	if (trad == NULL || trad->tr == NULL)
		return NULL;

	Xx = trad->X;

	tlocal->ou(trad->tr);

	//Il peut y avoir aussi des creations de traits bases sur un test
	for (i = 0; i<definirtraits.dernier; i++) {
		if (ITR(this, definirtraits[i]->traitstest, tlocal, NONECRIT)) {
			tlocal->ou(definirtraits[i]->traitsaffecter);
		}
	}

	return Xx;
}


char GlobalParseur::AnalyseForme(VECTA<char*>& enreg,
	char* catog,
	VECTA<Cat*>& catX,
	char rechTrad,
	char& majus,
	int dernierelement) {

	avl_vocabulaire* trad = NULL;

	majus = NON;

	//Le second au lemme
	if (prolog == OUI) {
		RemplaceLaChaine(vlex.surface, enreg[1], '\'', "''");
		RemplaceLaChaine(vlex.lemme, enreg[2], '\'', "''");
	}
	else {
		strcpy(vlex.surfacebrute, enreg[0]);
		strcpy(vlex.surface, enreg[1]);
		strcpy(vlex.lemme, enreg[2]);
	}

	trad = NULL;

	//Le premier champ correspond a la forme de surface...
	int i = 0;
	if (c_is_alpha((uchar*)enreg[1], i) == 2) {
		majus = OUI;
		if (s_is_upper(enreg[1]) == true)
			majus = 2;
	}


	//chaque trait est separe du suivant par un '_'
	//La chaine a gauche correspond a une categorie 
	//sinon c'est une erreur...

	Traits* tlocal = NULL;
	Cat* Xx = NULL;

	int debuttags = 3;
	//Si les offsets sont fournis dans la liste en entree
	if (TestChiffre((unsigned char*)enreg[3]) && TestChiffre((unsigned char*)enreg[4])) {
		vlex.gauche = atoi(enreg[3]);
		vlex.droit = atoi(enreg[4]);
		debuttags = 5;
		if (TestChiffre((unsigned char*)enreg[5]) && TestChiffre((unsigned char*)enreg[6])) {
			vlex.gauchecar = atoi(enreg[5]);
			vlex.droitcar = atoi(enreg[6]);
			debuttags = 7;
		}
		else {
			vlex.gauchecar = vlex.gauche;
			vlex.droitcar = vlex.droit;
		}
	}

	if (Xproposecategorie != NULL) {
		Xx = &Xproposecategorie->X;
		tlocal = new Traits(Xproposecategorie->traits);
	}

	string err;
	for (i = debuttags; i<enreg.dernier; i++) {
		//on recherche d'abord la categorie
		size_t indexMot = CalculClefHachageTout((uchar*)enreg[i]);
		trad = NULL;
		if (Traductions[indexMot] != NULL)
			trad = Traductions[indexMot]->recherchecasse(enreg[i]);

		//categorie a sauter
		if (trad != NULL && trad->tr == NULL) {
			if (tlocal != NULL)
				delete tlocal;
			return NON;
		}

		if (trad != NULL && trad->X != NULL)
		if (Xx == NULL) {
			Xx = trad->X;
			strcpy(catog, enreg[i]);
		}
		else
		if (Xx != trad->X)
			catX.ajoute(trad->X);

		if (trad != NULL && trad->tr != NULL) {
			if (tlocal == NULL) {
				tlocal = new Traits(trad->tr);
				tlocal->oup(gBaliseTraits);
				tlocal->oup(gBaliseSansFin);
				tlocal->oup(gBaliseDebut);
			}
			else
				tlocal->ou(trad->tr);
		}
		else {
			NOM = NULL;
			if (AFFICHE_ERREUR == OUI) {
				err = CHAINES[173];
				err += enreg[i];
				insere_erreur(STR(err));
				*flot_erreur << STR(err) << Endl;
			}
		}
	}

	if (Xx == NULL) {
		//Dans ce cas, on regarde dans la liste des definitions de categorie
		if (tlocal != NULL) {
			for (i = 0; i<definircategories.dernier; i++) {
				if (ITR(this, definircategories[i]->traitstest, tlocal, NONECRIT)) {
					Xx = definircategories[i]->X;
					tlocal->ou(definircategories[i]->traitsaffecter);
					break;
				}
			}
		}

		//Si malgre tout la categorie est encore NULL
		if (Xx == NULL) {
			trad = tag_nouveau;
			if (AFFICHE_ERREUR == OUI) {
				err = CHAINES[173];
				err += enreg[1];
				insere_erreur(STR(err));
				*flot_erreur << STR(err) << Endl;
			}
			if (trad == NULL || trad->X == NULL) {
				if (AFFICHE_ERREUR == OUI) {
					err = "Unknown TAG category: ";
					err += TAG_NOUVEAU;
					erreur(STR(err));
				}
			}
			else {
				if (tlocal == NULL) {
					tlocal = new Traits(trad->tr);
					tlocal->oup(gBaliseTraits);
					tlocal->oup(gBaliseSansFin);
					tlocal->oup(gBaliseDebut);
				}
				else
					tlocal->ou(trad->tr);
				Xx = trad->X;
			}
		}
	}

	if (tlocal != NULL) {
		//Il peut y avoir aussi des creations de traits bases sur un test
		for (i = 0; i<definirtraits.dernier; i++) {
			if (ITR(this, definirtraits[i]->traitstest, tlocal, NONECRIT)) {
				tlocal->ou(definirtraits[i]->traitsaffecter);
			}
		}
	}

	vlex.init(Xx, tlocal);
	//On recopie les autres acceptations pour la categorie du mot...
	//dans le cas ou il y aurait plusieurs analyses differentes...
	//Exemple: le mot ferme est nom, adj, adv et verbe...



	//On verifie si ce mot n'est pas un mot du lexique sur lequel certains traitements
	//doivent s'appliquer

	if (rechTrad == OUI)
		return TraitementVocabulaire(OUI, 0, dernierelement);

	return OUI;
}

void GlobalParseur::AjouteMajuscule(char majus) {
	if (majus) { //s'il y a une majuscule dans le mot 
		if (vlex.initiaux == NULL)
			vlex.initiaux = new Traits(this);
		if (MAJUSCULE != NULL)
			vlex.initiaux->ou(MAJUSCULE);
		if (majus == 2 && TOUTMAJUSCULE != NULL) //si tout le mot est en majuscule
			vlex.initiaux->ou(TOUTMAJUSCULE);
	}
}

ListeVoc* GlobalParseur::EclateVoc(char* entree, int numMot, ListeVoc* lv) {
	//Sinon, il faut generer plusieurs mots differents
	//On recopie d'abord dans un vecteur les infos...
	VECTA<char*> enreg;
	ListeVoc* slv = NULL;
	int j = 2;
	int posphrase = phrase.dernier;
	char hmmselect = NON;
	if (!strcmp(enregListeTags.fin(), "hmmselection"))
		hmmselect = OUI;
	char* souschaine = NULL;

	for (int i = 0; i <= nb_lectures; i++) {
		enreg.raz();
		//On rajoute systematiquement la forme brute
		enreg.ajoute(enregListeTags[0]);
		//on rajoute systematiquement la forme de surface
		if (!i)
			enreg.ajoute(enregListeTags[1]);
		else
			enreg.ajoute(enregListeTags[j]);
		for (; j<enregListeTags.dernier && strcmp(enregListeTags[j], DECOUPAGE); j++)
			enreg.ajoute(enregListeTags[j]);
		//On rajoute le hmmselection 
		if (hmmselect == OUI && j<enregListeTags.dernier)
			enreg.ajoute(enregListeTags.fin());

		if (entree != NULL)
			souschaine = DupliqueChaine(entree);

		if (lv == NULL)
			lv = ConstruitUneForme(enreg, souschaine, numMot, OUI);
		else {
			if (i) {
				ListeVoc* lvv = ConstruitUneForme(enreg, souschaine, numMot + i, NON);
				if (slv == NULL) {
					lv->suivants.affecte(lv->nbX - 1, lvv);
					slv = lvv;
				}
				else {
					slv->suivant = lvv;
					slv = lvv;
				}
			}
			else
				FusionneUneForme(enreg, lv, entree);
		}
		//Nous sautons alors systematiquement l'element, ce qui suit sera le lemme
		j++;
	}

	if (lv != NULL) {
		//Puis nous rajoutons les traits...
		lv->tr[lv->nbX - 1]->ou(tokenstart);
		lv->tr[lv->nbX - 1]->ou(untoken);
		slv = lv->suivants[lv->nbX - 1];
		if (slv != NULL) {
			while (slv->suivant != NULL) {
				slv->tr[0]->ou(tokeninter);
				slv->tr[0]->ou(untoken);
				slv = slv->suivant;
			}
			slv->tr[0]->ou(tokenend);
			slv->tr[0]->ou(untoken);
		}
	}
	else
		lv = ConstruitUneForme(enregListeTags, entree, numMot, OUI);
	return lv;
}

ListeVoc* GlobalParseur::ConstruitForme(char* entree, int numMot) {
	//cette methode permet de verifier s'il n'y a pas plus d'un mot enregistre dans la sequence
	//de traits et categories...
	//S'il n'y a pas de decoupage, le fonctionnement est transparent
	if (nb_lectures == 0)
		return ConstruitUneForme(enregListeTags, entree, numMot, OUI);

	return EclateVoc(entree, numMot, NULL);
}

void GlobalParseur::FusionneForme(ListeVoc* lv, char* entree) {
	//cette methode permet de verifier s'il n'y a pas plus d'un mot enregistre dans la sequence
	//de traits et categories...
	//S'il n'y a pas de decoupage, le fonctionnement est transparent
	if (nb_lectures == 0) {
		FusionneUneForme(enregListeTags, lv, entree);
		return;
	}

	EclateVoc(entree, num_mot, lv);
}


//Cette methode est utilisee pour analyser une entree et renvoyer un ListeVoc
//Cette analyse consiste a regarder les traits et categories obtenus apres decoupage
//de l'information renvoyee par les transducteurs.
ListeVoc* GlobalParseur::ConstruitUneForme(VECTA<char*>& enreg, char* entree, int numMot, char ajoute) {
	VECTA<Cat*> catX;
	char catog[100];
	char majus = NON;

	vlex.raz();
	nouvellesLectures.raz();
	lemmesDejaLus.raz();


	char typeLemme = AnalyseForme(enreg, catog, catX, OUI, majus, phrase.dernier - 1);
	if (!vlex.lemme[1] && strchr(",.?!\"{}[]()'`#~|\\/", vlex.lemme[0]))
		punctuations++;

	if (typeLemme == NON) {
		if (entree != NULL)
			libere(entree);
		return NULL;
	}

	if (vlex.lemme == NULL || vlex.X == NULL) {
		if (entree != NULL)
			libere(entree);
		vlex.detruit();
		return NULL;
	}

	if (catX.dernier >= 1) {
		if (AFFICHE_ERREUR == OUI) {
			char buffer[1000];
			strcpy(buffer, CHAINES[64]);
			strcat(buffer, vlex.X->nom);
			strcat(buffer, "(");
			strcat(buffer, catog);
			strcat(buffer, ")");
			strcat(buffer, "\n");
			insere_erreur(buffer);

			*flot_erreur << CHAINES[64] << vlex.X->nom << "(" << catog << ")" << Endl;

			*flot_erreur << vlex.X->nom << " ";
			for (int j = 0; j<catX.dernier; j++)
				*flot_erreur << catX[j]->nom << " ";
			*flot_erreur << Endl << Endl;
		}
	}

	AjouteMajuscule(majus);
	FusionX = 0;
	ListeVoc* lv = fournirListeVoc(vlex);
	FusionX |= vlex.X->Val;
	if (ListeCoupeSequence.dernier != 0 || TestOptions(TESTER_LANGUE) || encourspourlangue == OUI)
		lv->global = new Traits(vlex.initiaux);

	lv->num_mot = numMot;

	lv->tag.affecte(0, entree);

	//Lorsque typeLemme==REMPLACEMENT, cela signifie que l'on conserve qu'une seule interpretation pour un mot
	//celle du lexique... Les autres seront detruites sans etre analysee...
	lv->passage = typeLemme;
	if (ajoute == OUI)
		phrase.ajoute(lv);
	int i = 1;

	for (int ivoc = 0; ivoc<nouvellesLectures.dernier; ivoc++) {
		auto_vocabulaire* lexeme = nouvellesLectures[ivoc];
		//autre categorie
		vlex.X = lexeme->XNouveau;
		//on ajoute les nouveaux traits...
		if (lexeme->tr != NULL) {
			vlex.initiaux = new Traits(lexeme->tr);
			if ((lexeme->tr)->lemme != NULL && (lexeme->tr)->maniplemme == ECRASE)
				(lexeme->tr)->lemme->mot(vlex.lemme, 2048);
			if ((lexeme->tr)->surface != NULL &&
				(lexeme->tr)->manipsurface == ECRASE)
				(lexeme->tr)->surface->mot(vlex.surface, 2048);
		}
		else {
			Categorie* cc = Categories[vlex.X->index];
			vlex.initiaux = new Traits(this);
			(vlex.initiaux)->ou(cc);
		}

		FusionX |= vlex.X->Val;
		if (ListeCoupeSequence.dernier != 0)
			(lv->global)->ou(vlex.initiaux);

		if (entree != NULL)
			lv->tag.affecte(i, DupliqueChaine(entree));
		else
			lv->tag.affecte(i, NULL);

		AjouteMajuscule(majus);
		lv->tr.affecte(i, vlex.initiaux);
		lv->lemme.affecte(i, DupliqueChaine(vlex.lemme));
		lv->X.affecte(i, vlex.X);
		lv->nbX++;
		i++;
	}

	return lv;
}


void GlobalParseur::FusionneUneForme(VECTA<char*>& enreg, ListeVoc* lv, char* entree) {
	VECTA<Cat*> catX;
	char catog[100];
	char majus = NON;

	vlex.raz();
	nouvellesLectures.raz();

	//Dans le cas ou l'on ecrase completement une entree
	//on revient sans l'analyser...
	if (lv->passage == REMPLACEMENT) {
		if (entree != NULL)
			libere(entree);
		return;
	}

	if (AnalyseForme(enreg, catog, catX, OUI, majus, phrase.dernier - 2) == NON) {
		vlex.detruit();
		if (entree != NULL)
			libere(entree);
		return;
	}

	if (vlex.lemme == NULL || vlex.X == NULL) {
		if (entree != NULL)
			libere(entree);
		vlex.detruit();
		return;
	}

	if (catX.dernier >= 1) {
		if (AFFICHE_ERREUR == OUI) {
			char buffer[1000];
			strcpy(buffer, CHAINES[64]);
			strcat(buffer, vlex.X->nom);
			strcat(buffer, "(");
			strcat(buffer, catog);
			strcat(buffer, ")");
			strcat(buffer, "\n");
			if (entree != NULL) {
				strcat(buffer, entree);
				strcat(buffer, "\n");
			}
			insere_erreur(buffer);

			*flot_erreur << CHAINES[64] << vlex.X->nom << "(" << catog << ")" << Endl;
			if (entree != NULL)
				*flot_erreur << entree << Endl;
			*flot_erreur << vlex.X->nom << " ";
			for (int j = 0; j<catX.dernier; j++)
				*flot_erreur << catX[j]->nom << " ";
			*flot_erreur << Endl << Endl;
		}
	}

	int i = lv->nbX;
	AjouteMajuscule(majus);
	lv->tag.affecte(i, entree);
	if (ListeCoupeSequence.dernier != 0)
		(lv->global)->ou(vlex.initiaux);

	lv->tr.affecte(i, vlex.initiaux);
	lv->lemme.affecte(i, DupliqueChaine(vlex.lemme));
	lv->X.affecte(i, vlex.X);
	lv->nbX++;

	FusionX |= vlex.X->Val;

	for (int ivoc = 0; ivoc<nouvellesLectures.dernier; ivoc++) {
		auto_vocabulaire* lexeme = nouvellesLectures[ivoc];

		i++; //on rajoute une entree nouvelle
		//autre categorie
		vlex.X = lexeme->XNouveau;
		//on ajoute les nouveaux traits...
		if (lexeme->tr != NULL) {
			vlex.initiaux = new Traits(lexeme->tr);
			if ((lexeme->tr)->lemme != NULL && (lexeme->tr)->maniplemme == ECRASE)
				(lexeme->tr)->lemme->mot(vlex.lemme, 2048);
			if ((lexeme->tr)->surface != NULL &&
				(lexeme->tr)->manipsurface == ECRASE)
				(lexeme->tr)->surface->mot(vlex.surface, 2048);
		}
		else {
			Categorie* cc = Categories[vlex.X->index];
			vlex.initiaux = new Traits(this);
			(vlex.initiaux)->ou(cc);
		}

		FusionX |= vlex.X->Val;
		if (ListeCoupeSequence.dernier != 0)
			(lv->global)->ou(vlex.initiaux);

		if (entree != NULL)
			lv->tag.affecte(i, DupliqueChaine(entree));
		else
			lv->tag.affecte(i, NULL);

		AjouteMajuscule(majus);
		lv->tr.affecte(i, vlex.initiaux);
		lv->lemme.affecte(i, DupliqueChaine(vlex.lemme));
		lv->X.affecte(i, vlex.X);
		lv->nbX++;
	}
}


void GlobalParseur::ListeTagsAjouter(char* entree) {
	if (enregListeTags.dernier >= enregListeTags.taille)
		enregListeTags.ajuste(enregListeTags.taille + 10);

	enregListeTags.dernier++;
	char* v = enregListeTags.fin();
	if (v == NULL) {
		v = new char[taille_mot + 1];
		enregListeTags.dernier--;
		enregListeTags.ajoute(v);
	}
	if (strlen(entree) >= taille_mot)
		entree[taille_mot] = 0;
	strcpy(v, entree);
}

int CompteTab(char* ligne) {
	int v = 0;
	char* pt = ligne;

	while ((pt = strchr(pt + 1, '\t')) != NULL) v++;
	return v;
}

void GlobalParseur::ResetMarkuptags() {
	gBaliseDebut->raz();
}


void GlobalParseur::lireLigne(char* ligne, unsigned int taille) {
	if (fichierDonnees != NULL)
		fgets(ligne, taille, fichierDonnees);
	else {
		if (curseurDonnees == NULL) {
			ligne[0] = 0;
			return;
		}
		char* pt = strchr((char*)curseurDonnees, '\n');
		if (pt != NULL) {
			char c = pt[1];
			pt[1] = 0;
			if (strlen((char*)curseurDonnees) < taille)
				strcpy(ligne, (char*)curseurDonnees);
			else {
				memcpy(ligne, (char*)curseurDonnees, taille - 1);
				ligne[taille - 1] = 0;
			}

			pt[1] = c;
			curseurDonnees = (unsigned char*)(pt + 1);
			int i = 0;
			while (curseurDonnees[i] != 0 &&
				(curseurDonnees[i] == '\t' || curseurDonnees[i] == ' ')) i++;
			if (curseurDonnees[i] == 0)
				curseurDonnees = NULL;
		}
		else {
			strcpy(ligne, (char*)curseurDonnees);
			strcat(ligne, "\n");
			curseurDonnees = NULL;
		}
	}
}

int GlobalParseur::finLecture(void) {
	if (fichierDonnees != NULL)
		return feof(fichierDonnees);

	if (curseurDonnees == NULL)
		return OUI;
	return NON;
}

void GlobalParseur::LitDoubleTab(char* debut,
	char* ligne,
	int taille,
	char premier) {

	if (taille <= 1)
		erreur("Unknown Entry Format");

	lireLigne(ligne, taille);

#ifdef WIN32
	//On se debarrasse du
	char* suivant = ligne;
	char* pt;

	while ((pt = strchr(suivant, 13))) {
		strcpy(pt, pt + 1);
		suivant = pt;
	}
#endif

	if ((premier == OUI && ligne[0] == '\n') || finLecture())
		return;

	int nbtab = CompteTab(ligne);
	if (nbtab<2) {
		int tailleligne = strlen(ligne);
		ligne[tailleligne - 1] = ' ';
		LitDoubleTab(debut, ligne + tailleligne, taille - tailleligne, NON);
	}
	else
	if (nbtab>2) {
		nbtab -= 2;
		char* pt = ligne;
		for (int i = 0; i<nbtab; i++) {
			pt = strchr(pt, '\t');
			*pt = ' ';
			pt++;
		}
	}
}

void GlobalParseur::SetLowerCase(unsigned char* iterateur) {
	if (enregListeTags.dernier == 3 && !TestOptions(NON_NORMALISATION_TAG)) {
		MetEnMinuscule((unsigned char*)iterateur);
	}
}

//Cette fonction permet de reconstruire la chaine en eleminant les doublons parmi les traits
void GlobalParseur::LexiconEntryParsing(char* chaine, char** duplicat) {
	char* pt;
	ListeTagsRAZ(chaine, duplicat);

	char* iterateur = strrchr(chaine, '\t'); //debut de la chaine

	//Les deux premieres positions correspondent a la surface et au lemme...
	//la premiere chaine est la surface et la deuxieme le lemme...
	char* ptsurface = strchr(chaine, '\t');
	*ptsurface = 0;
	//les cas suivants peuvent se poser:
	//la source est en UTF8 et l'encodage de la grammaire est en UTF8: on ne fait rien
	//la source est en ISO et l'encodage de la grammaire est en ISO: on ne fait rien
	//la source est en UTF8 et l'encodage de la grammaire est en ISO: on convertit vers ISO
	//la source est en ISO et l'encodage de la grammaire est en UTF8: on convertit vers UTF8
	ListeTagsAjouter(chaine);
	if (tablelatin) {
		chainetraduction = conversion_latin_table_to_utf8(tablelatin, (uchar*)chaine);
		ListeTagsAjouter(STR(chainetraduction));
	}
	else {
		if (encodage_source == encodage_grammaire)
			ListeTagsAjouter(chaine);
		else
		if (encodage_source == UTF8_ENCODING)
			ListeTagsAjouter((char*)conversionUTF8VersAscii((uchar*)chaine));
		else
			ListeTagsAjouter((char*)conversionAsciiVersUTF8((uchar*)chaine));
	}
	pt = strchr(ptsurface + 1, '\t');
	*pt = 0;
	if (tablelatin) {
		chainetraduction = conversion_latin_table_to_utf8(tablelatin, (uchar*)chaine);
		ListeTagsAjouter(STR(chainetraduction));
	}
	else {
		if (encodage_source == encodage_grammaire)
			ListeTagsAjouter(ptsurface + 1);
		else
		if (encodage_source == UTF8_ENCODING)
			ListeTagsAjouter((char*)conversionUTF8VersAscii((uchar*)(ptsurface + 1)));
		else
			ListeTagsAjouter((char*)conversionAsciiVersUTF8((uchar*)(ptsurface + 1)));
	}

	pt = iterateur;
	iterateur += 2; //premier "+"
	pt = strchr(iterateur, '+');
	if (!TestOptions(NON_NORMALISATION_TAG))
		MetEnMinuscule((unsigned char*)iterateur);

	while (pt != NULL) {
		*pt = 0;
		//dans le cas ou l'on reconnait une chaine de decoupage
		//on en garde la trace en decomptant le nombre de bornes trouvees.
		CategorySplitProcessing(iterateur);
		ListeTagsAjouter(iterateur);
		iterateur = pt + 1;
		pt = strchr(iterateur, '+');
	}
	ListeTagsAjouter(iterateur);

	//Nous ajoutons la chaine brute en queue
	HMMInsertionProcessing();
}

char GlobalParseur::EndOfSentence(char* ligne, char type) {
	int i;
	//On verifie si l'on atteint le maximum de mots
	if (phrase.dernier >= nbmotsmaximum && nbmotsmaximum != -1) {
		if (TRACE)
			trace << Endl << CHAINES[279] << Endl << Endl;
		if (AFFICHE_ERREUR)
			*flot_erreur << CHAINES[279] << Endl;
		return OUI;
	}

	ListeVoc* voc = phrase.fin();

	fenetre = -1;
	//dans le cas ou l'on decoupe avec des regles, on lit des fenetres de lecture de maxmots
	if (ListeCoupeSequence.dernier>0) {
		//On attend d'avoir nbmaxmotsmaximum-1 pour commencer a appliquer nos regles
		if (FinAnalyse == NON && phrase.dernier<nbmotsmaximum - 1)
			return NON;
	}
	else {//Methode par la chaine de caractere
		if (type == OUI || type == NON) {
			for (i = 0; i<FinDePhrase.dernier; i++) {
				if (strstr(ligne, FinDePhrase[i])) {
					return OUI;
				}
			}
			for (i = 0; i<ExpressionsFinDePhrase.dernier; i++) {
				if (ExpressionsFinDePhrase[i]->recherche((uchar*)ligne) != NULL) {
					return OUI;
				}
			}
		}
		return NON;
	}

	//On teste avec les regles
	if (type == UTILISE || type == OUI) {
		//if ((CategoriesCoupeSequence & FusionX) == 0) {
		//    return NON;
		//}
		//On initialise le premier mot avec le trait start
		voc = phrase[0];
		if (voc != NULL && voc->global != NULL)
			(voc->global)->ou(DebutPhrase);

		for (int ireg = 0; ireg<ListeCoupeSequence.dernier; ireg++) {
			VECTA<ListeVoc*> vect_voc;
			VECTA<ListeVoc*> pile;
			VECTA<Traits*> piletraits;
			char solution = NON;
			int coupe = phrase.dernier - 1;
			EtatArbre* arb = ListeCoupeSequence[ireg]->regle;
			//if ( (arb->arcs[0]->X->Val & FusionX) == 0)
			//    continue;
			initIteration();
			if (ListeCoupeSequence[ireg] != NULL)
				ListeCoupeSequence[ireg]->deja = coupe;
			arb->Decoupe(this,
				ListeCoupeSequence[ireg],
				solution,
				phrase, 0,
				1,
				pile,
				piletraits,
				coupe,
				OUI,
				&vect_voc);

			if (solution == OUI) {
				//VECTA<Noeud*> vect_noeuds;
				//for (int i=0;i<vect_voc.dernier;i++) {
				//	if (vect_voc[i]!=NULL)
				//		vect_noeuds.ajoute(ProduitListe(vect_voc[i],vect_voc[i]->idx));
				//}
				//if (ListeCoupeSequence[ireg]->TestExpressionBooleen(this,vect_noeuds)) {
				//On reduit la fenetre d'analyse aux elements a "coupe"
				fenetre = phrase.dernier;
				phrase.dernier = coupe + 1;
				//tout le reste est conserve dans reste_phrase et sera reutilise plus tard
				//for (int ic=phrase.dernier-1;ic>coupe;ic--)
				//	reste_phrase.affecte(i--,phrase.retire());
				return OUI;
			}
		}
		//Si aucune regle n'a ete trouvee, la phrase fait maxmot...
		return OUI;
	}

	return NON;
}

void GlobalParseur::NettoiePhrase() {
	int i;
	//phrase.raz();
	phrasefictifs.raz();
	punctuations = 0;

	if (fenetre != -1) {
		//On repere alors dans TasDeListeVoc, les elements de phrase_res
		//pour les retirer. De cette facon, on pourra nettoyer cette liste d'un seul coup
		//Ce sont tous les elements compris jusqu'a phrase.dernier dont on doit se debarasser
		//On marque les elements a detruire		
		for (i = 0; i<phrase.dernier; i++)
			phrase[i]->raz();

		//On effectue alors un decalage de tous les elements non encore pris en compte vers la gauche
		//pour eliminer ceux qui ont deja ete utilises
//#if defined(ARCH64) && defined(UNIX)
		for (i = phrase.dernier; i<fenetre; i++) {
			phrase.vecteur[i - phrase.dernier] = phrase.vecteur[i];
			phrase.vecteur[i] = NULL;
		}
//#else
//		memcpy(phrase.vecteur, phrase.vecteur + phrase.dernier, (fenetre - phrase.dernier)*sizeof(void*));
//#endif
		//Le dernier element est donc desormais fenetre-phrase.dernier
		phrase.dernier = fenetre - phrase.dernier;
		fenetre = -1;
	}
	else {//sinon, on nettoie tout
		nettoyerListeVoc();
		phrase.raz();
	}
}

void ConstruitListePrefixee(Noeud* n, VECTA<Noeud*>& liste) {
	if (n == NULL || n->fils == NULL)
		return;

	Noeud* c = n->fils;

	while (c != NULL) {
		ConstruitListePrefixee(c, liste);
		c = c->frere;
	}

	liste.ajoute(n);
}

string& GlobalParseur::PhraseCourante() {
	phrasecourante = phrase[0]->surfacebrute;
	int iph;
	for (iph = 1; iph<phrase.dernier; iph++) {
		if (phrase[iph - 1]->borneDroite != phrase[iph]->borneGauche)
			phrasecourante += " ";
		phrasecourante += phrase[iph]->surfacebrute;
	}
	return phrasecourante;
}

void GlobalParseur::TraiteXipResult() {
#ifdef XMLINPUT
	if (xipresult == NULL || parse_phrase == NON)
		return;

	int i;
	for (i = 0; i<Variables.dernier; i++) {
		//et on les rajoute au noeud INTERVICTOIRE
		if (Variables[i]->sorte == VGLOBALE ||
			Variables[i]->sorte == VGLOBALEREINIT ||
			Variables[i]->sorte == VLEMMEINDEX ||
			Variables[i]->sorte == VLEMMECHAINEINDEX ||
			Variables[i]->sorte == VCHAINE)
			LINTERVICTOIRE->variables.affecte(Variables[i]->position, Variables[i]);
	}
	ConstruitResultat(xipresult);

	//Appel d'une fonction externe definie par l'utilisateur
	//la fonction est de type: void nom(XipResult* xip,void* donnees);
	if (AppelExterne != NULL)
		(*AppelExterne)(iParseurCourant, xipresult, donneesExternes);
#endif
}

void GlobalParseur::AffichageResultat() {
	int i, j;
	ostream* os = &cout;
	if (OS != NULL)
		os = OS;

	//Affichage de la phrase initiale
	if (XML != OUTPUTLIBXML && XML != OUI && TestOptions(AFFICHAGE_PHRASE)) {
		if (TestOptions(AFFICHAGE_NUMPHRASE))
			*os << num_phrase << "> ";

		if (NoeudsLexicaux.dernier) {
			if (NoeudsLexicaux[0]->source->voc != NULL)
				*os << ConversionChaine(this, NoeudsLexicaux[0]->source->voc->surface);

			for (i = 1; i<NoeudsLexicaux.dernier; i++) {
				if (NoeudsLexicaux[i]->fictif == OUI)
					continue;
				if (NoeudsLexicaux[i]->source->voc != NULL) {
					if (NoeudsLexicaux[i - 1]->source->voc != NULL) {
						if (NoeudsLexicaux[i - 1]->source->voc->borneDroite !=
							NoeudsLexicaux[i]->source->voc->borneGauche)
							*os << " ";
					}
					*os << ConversionChaine(this, NoeudsLexicaux[i]->source->voc->surface);
				}
			}
			*os << Endl;
		}
	}

	if (DESAMBIGUISATION == OUI || mathematique == 2 || parse_phrase == NON)
		return;


	//     XipResult truc;
	//     ConstruitResultat(&truc);
	//     VECTA<XipFeature*>& fe= truc.sentences[0]->root->daughters[0]->daughters[0]->features;
	//     for (int ife=0;ife<fe.dernier;ife++)
	//         *flot_erreur<<fe[ife]->attribute<<":"<<fe[ife]->value<<endl;


#ifdef XIPLIBXML
	if (XML == OUTPUTLIBXML) {
		if (gAfficherResultats == OUI) {
			//On crée un LUNIT, qui nous servira de parent
			XmlLunit = CreationNoeudXML(XmlResult, "LUNIT", NULL);
			if (langue_de_la_grammaire != NULL)
				AjouteAttributValeur(XmlLunit, "language", langue_de_la_grammaire);
			else
				AjouteAttributValeur(XmlLunit, "language", "Unknown");

			//On affiche l'arbre avant les dependances..
			if (TestOptions(AFFICHAGE_ARBRE_CHUNK) && INTERVICTOIRE != NULL)
				/** Start MCR added code for passage XML output **/
				INTERVICTOIRE->ConstruitNoeudXML(this, XmlLunit);
			if (CalculFonction) {
				int numFonction = 0;
				for (i = 1; i<INTERFONCTIONS.dernier; i++) {
					if (INTERFONCTIONS[i] != NULL) {
						for (int j = 0; j<INTERFONCTIONS[i]->dernier; j++)
						if ((*INTERFONCTIONS[i])[j] != NULL)
							(*INTERFONCTIONS[i])[j]->ConstruitXML(numFonction);
						/** End MCR added code for passage XML output **/
					}
				}
			}
		}
		return;
	}
	if (XML == OUI) {
		if (gAfficherResultats == OUI) {
			char buffer[100];
			xmlChar* buf = NULL;
			int longueur = 0;

			/** Start MCR added code for passage XML output **/
			if (!TestOptions(AFFICHAGE_PASSAGE_XML_FORMAT)) {
				if (langue_de_la_grammaire != NULL)
					sprintf(buffer, "<LUNIT language=\"%s\"/>", langue_de_la_grammaire);
				else
					strcpy(buffer, "<LUNIT language=\"Unknown\"/>");
			}
			else {

				sprintf(buffer, "<Sentence id=\"E%d\"/>", this->num_phrase);
			}
			/** End MCR added code for passage XML output **/

			xmlDocPtr doc = xmlSAXParseMemory(NULL, buffer, strlen(buffer), 0);
			XmlLunit = doc->children;

			//On affiche l'arbre avant les dependances..
			if (TestOptions(AFFICHAGE_ARBRE_CHUNK) && INTERVICTOIRE != NULL)
				/** Start MCR added code for passage XML output **/
				INTERVICTOIRE->ConstruitNoeudXML(this, XmlLunit);
			if (CalculFonction) {
				int numFonction = 0;
				for (i = 1; i<INTERFONCTIONS.dernier; i++) {
					if (INTERFONCTIONS[i] != NULL) {
						for (int j = 0; j<INTERFONCTIONS[i]->dernier; j++)
						if ((*INTERFONCTIONS[i])[j] != NULL)
							(*INTERFONCTIONS[i])[j]->ConstruitXML(numFonction);
						/** End MCR added code for passage XML output **/
					}
				}
			}

			if (TestOptions(AFFICHAGE_PHRASE) && phrase.dernier != 0)
				CreationNoeudXML(XmlLunit, "SENTENCE", STR(PhraseCourante()));

			xmlDocDumpMemoryEnc(doc, &buf, &longueur, (const char*)refdocument->encoding);
			char* pt = strstr((char*)buf, "<?");
			if (pt != NULL) {
				pt = strstr(pt, "?>");
				if (pt != NULL)
					pt = strchr(pt, '<');
				*os << pt;
			}
			else
				*os << buf;
			xmlFree(buf);
			xmlFreeDoc(doc);
		}
		return;
	}

#else
	if (XML == OUI) {
		if (gAfficherResultats == OUI) {
			*os << "<LUNIT language=";
			if (langue_de_la_grammaire != NULL)
				*os << "\"" << langue_de_la_grammaire << "\">";
			else
				*os << "\"Unknown\">";

			//On affiche l'arbre avant les dependances..
			if (TestOptions(AFFICHAGE_ARBRE_CHUNK) && INTERVICTOIRE != NULL)
				INTERVICTOIRE->SauveResultatXML(this, *os);
			if (CalculFonction) {
				for (i = 1; i<INTERFONCTIONS.dernier; i++) {
					if (INTERFONCTIONS[i] != NULL) {
						for (int j = 0; j<INTERFONCTIONS[i]->dernier; j++)
						if ((*INTERFONCTIONS[i])[j] != NULL)
							(*INTERFONCTIONS[i])[j]->Affiche(*os);
					}
				}
			}
			*os << "</LUNIT>" << Endl;
		}
		return;
	}
#endif    

	if (CalculFonction && gAfficherResultats == OUI) {
		//l'ordre est donne par la definition des fonctions
		TVecteur test_ordre = ParametresAffichage & AFFICHAGE_ORDRE_DEPENDANCE;
		VECTA<Noeud*> liste;
		switch (test_ordre) {
		case AFFICHAGE_DEP_NOEUD:
			ConstruitListePrefixee(INTERVICTOIRE, liste);
			for (i = 0; i<liste.dernier; i++) {
				ExtractionFonction* listeResFonc = PREMIERFONCTIONS[liste[i]->num_noeud];
				if (listeResFonc != NULL)
				for (j = 0; j<listeResFonc->dernier; j++)
				if ((*listeResFonc)[j] != NULL)
					(*listeResFonc)[j]->Affiche(*os);
			}
			break;
		case AFFICHAGE_DEP_CREATION:
			if (INTERFONCTIONS[0] != NULL) {
				for (j = 0; j<INTERFONCTIONS[0]->dernier; j++)
				if ((*INTERFONCTIONS[0])[j] != NULL)
					(*INTERFONCTIONS[0])[j]->Affiche(*os);
			}
			break;
		case AFFICHAGE_DEP_NOM:
		default:
			for (i = 1; i<INTERFONCTIONS.dernier; i++) {
				if (INTERFONCTIONS[i] != NULL) {
					for (j = 0; j<INTERFONCTIONS[i]->dernier; j++)
					if ((*INTERFONCTIONS[i])[j] != NULL)
						(*INTERFONCTIONS[i])[j]->Affiche(*os);
				}
			}
			break;
		}
	}

	if (TestOptions(AFFICHAGE_ARBRE_CHUNK) && INTERVICTOIRE != NULL) {

		if (gAfficherResultats == OUI && TestAfficheFonction(AFFICHE_FONCTION)) {
			//if (langue_de_la_grammaire!=NULL)
			//	*os<<"["<<langue_de_la_grammaire<<"]";
			*os << num_phrase << ">";
			INTERVICTOIRE->SauveResultat(this, *os);
			//Affichage des variables:
			VariableIndexeeNoeud* vn;
			VariableIndexeeChaine* vic;
			VariableIndexeeChaineDependance* vicd;
			VariableIndexeeDependance* vid;
			int ino;
			if (mathematique) {
				*os << Endl;
				for (i = 0; i<Variables.dernier; i++) {
					if (Variables[i]->procedure == NON && Variables[i]->utilisee == OUI) {
						switch (Variables[i]->sorte) {
						case VNOEUD:
							vn = (VariableIndexeeNoeud*)Variables[i];
							for (ino = 0; ino<vn->Taille(); ino++) {
								*os << vn->nom << "(";
								vn->indexes[ino]->AfficheMot(this, *os);
								*os << ") : " << vn->valeurs[ino] << Endl;
							}
							break;
						case VCHAINENOEUD:
							vic = (VariableIndexeeChaine*)Variables[i];

							for (ino = 0; ino<vic->Taille(); ino++) {
								*os << vic->nom << "(";
								vic->indexes[ino]->AfficheMot(this, *os);
								*os << ") : " << (char*)vic->valeurs[ino]->c_str() << Endl;
							}
							break;
						case VDEPENDANCE:
							vid = (VariableIndexeeDependance*)Variables[i];
							for (ino = 0; ino<vid->valeurs.dernier; ino++) {
								*os << vic->nom << "(";
								*os << vid->indexes[ino]->Fx->nom;
								*os << ") : " << vid->valeurs[ino] << Endl;
							}
							break;
						case VCHAINEDEPENDANCE:
							vicd = (VariableIndexeeChaineDependance*)Variables[i];
							for (ino = 0; ino<vicd->valeurs.dernier; ino++) {
								*os << vicd->nom << "(";
								*os << vicd->indexes[ino]->Fx->nom;
								*os << ") : " << (char*)vicd->valeurs[ino]->c_str() << Endl;
							}
							break;
						case VGLOBALEREINIT:
							Variables[i]->affiche(*os);
						}
					}
				}
			}
			*os << Endl << Endl;
		}
	}
}


void GlobalParseur::AfficheVariablesGlobales() {
	if (xipresult != NULL)
		return;

	if (mathematique == NON)
		return;

	ostream* os = &cout;
	if (OS != NULL)
		os = OS;

#ifdef XIPLIBXML
	if (XML == OUTPUTLIBXML) {
		if (XmlLunit != NULL) {
			for (int i = 0; i<Variables.dernier; i++) {
				if (Variables[i]->procedure == NON && Variables[i]->utilisee == OUI) {
					if (Variables[i]->reinit == NON)
						Variables[i]->CreationXML(this, XmlLunit);
				}
			}
		}
		return;
	}
#endif    

	if (XML == OUI) {
		*os << "<VARIABLES>" << endl;
		for (int i = 0; i<Variables.dernier; i++) {
			if (Variables[i]->procedure == NON && Variables[i]->utilisee == OUI) {
				if (Variables[i]->reinit == NON)
					Variables[i]->afficheXML(*os);
			}
		}
		*os << "</VARIABLES>" << endl;
		return;
	}
	if (gAfficherResultats == OUI) {
		*os << Endl << "Variables:" << Endl;
		for (int i = 0; i<Variables.dernier; i++) {
			if (Variables[i]->procedure == NON && Variables[i]->utilisee == OUI) {
				if (Variables[i]->reinit == NON)
					Variables[i]->affiche(*os);
			}
		}
	}
}


void GlobalParseur::Analyse(char renumerote, char arbre) {
	ostream* os = &cout;

	bool deletexipresult = false;

	distances.clear();
	distancesarbre.clear();

	int i;
	if (TestOptions(ANALYSE_ALEATOIRE) == OUI) {
		for (i = 0; i<VBases.dernier; i++)
			VBases[i]->calculaleatoire();
	}

	if (OS != NULL)
		os = OS;

	if (XML == OUI) {
		if (mathematique == OUI)
			*os << "<XIPRESULT math=\"1\">" << Endl;
		else
			*os << "<XIPRESULT math=\"0\">" << Endl;
	}

	if (TestOptions(AFFICHAGE_JSON)) {
		if (xipresult == NULL) {
			xipresult = new XipResult();
			deletexipresult = true;
		}
	}

	SentenceAnalysis(renumerote, arbre);
	AfficheVariablesGlobales();
	if (TestOptions(TESTER_LANGUE)) {
		for (i = 0; i<gVecteurParseurs.dernier; i++) {
			GlobalParseur* leParseur = gVecteurParseurs[i];
			leParseur->DeduitFonction(-1, DEDUCTIONFINALE, NON);
		}
	}
	else
		DeduitFonction(-1, DEDUCTIONFINALE, NON);

	if (TestOptions(AFFICHAGE_JSON)) {
		ConstruitResultat(xipresult);
		string rjson = xipresult->JSon();
		*os << rjson;
		if (deletexipresult) {
			delete xipresult;
			xipresult = NULL;
		}
	}

	if (XML == OUI)
		*os << "</XIPRESULT>" << Endl;
	gBaliseSansFin->raz();
	gBaliseDebut->raz();
	gBaliseTraits->raz();
	razpile();
}

//Analyse d'une sequence de mots a partir d'un fichier...
void GlobalParseur::SentenceAnalysis(char renumerote, char arbre) {
	char ligne[1000];
	char chaine[1000];
	int len;
	char* dupchaine;
	int bascule = 1;
	baseOffset = 0;

	num_phrase = 0;

	while (!finLecture()) {
		ligne[0] = 0;

		if (renumerote == OUI) {
			num_token = 0;
			num_mot = 0;
		}
		else
			num_mot++;

		nombre_mots++;
		chaine[0] = 0;
		ListeVoc* voc = NULL;

		LitDoubleTab(ligne, ligne, 999, OUI);

		len = strlen(ligne) - 1;
		if (len >= 0)
			ligne[len] = 0;

		char testFinDePhrase = NON;
		char testBalise = NON;

		while (!finLecture()) {
			if (ligne[0]) {
				testBalise = TestBalise(ligne);
				if (testBalise == NON) {
					testFinDePhrase = EndOfSentence(ligne, NON);
					LexiconEntryParsing(ligne, &dupchaine);
					if (voc == NULL) {
						voc = ConstruitForme(dupchaine, num_mot);
						if (voc != NULL) {
#ifdef XIPLIBXML
							voc->xmlnoeud = noeudxmltokencourrant;
#endif
							num_mot++;
							nombre_mots++;
						}
					}
					else
						FusionneForme(voc, dupchaine);
				}
			}
			else {
				voc = NULL;
				if (testBalise == NON)
					gBaliseDebut->raz();

				if (testFinDePhrase == OUI || EndOfSentence(ligne, UTILISE))
					break;
			}

			LitDoubleTab(ligne, ligne, 999, OUI);

			len = strlen(ligne) - 1;
			if (len >= 0)
				ligne[len] = 0;
		}

		FinAnalyse = OUI;
		while (phrase.dernier != 0) {
			Parse(OUI);
			Detruit();
			num_phrase++;
		}
		FinAnalyse = NON;
	}

	FinAnalyse = OUI;
	while (phrase.dernier != 0) {
		Parse(OUI);
		Detruit();
	}
	FinAnalyse = NON;
}

/*-------------------------------------------------------------------------*/
void GlobalParseur::Detruit() {
	int ino;

	//Dans le cas d'un test sur une langue
	//On ne detruit rien...
	if (encourspourlangue == OUI) {
		taille_phrase = phrase.dernier;
		phrase.dernier = 0;
		return;
	}

	//On teste une derniere fois les regles en queue
	DeduitFonction(-1, DEDUCTIONQUEUE, NON);
	taille_phrase = -1;
	NettoiePhrase();
	NettoieVecteursTravail();

	for (ino = 0; ino<NoeudsDependance.dernier; ino++) {
		NoeudsDependance[ino]->N.frere = NULL;
		NoeudsDependance[ino]->N.droite = -100;
		NoeudsDependance[ino]->N.num_droite = -100;
		NoeudsDependance[ino]->N.num_noeud = -1;
	}

	NoeudsDependance.raz();
	string buffbis;
	static VECTA<Noeud*> noeuds;

	for (ino = 0; ino<Variables.dernier; ino++) {
		if (Variables[ino]->reinit == OUI)
			Variables[ino]->raz();
	}


	//On reinitialise les commutateurs
	for (ino = 0; ino<commutateurs.dernier; ino++) {
		auto_vocabulaire* trad = commutateurs[ino];
		if (trad->ecraser == BALISECOMMUTATEURFERMEE) {
			gBaliseDebut->raz();
			gBaliseTraits->xou(trad->tr);
		}
		trad->ecraser = BALISECOMMUTATEUR;
	}
	dernierNumeroNoeud = 0;
	demarre_regle = 0;
	demarre_couche = -1;
	aller_label = NULL;
	kif_garbage_xip();
}



void GlobalParseur::NettoieVecteursTravail() {
	int i;

	if (INTERFONCTIONS[0] != NULL)
		nb_dependances += INTERFONCTIONS[0]->dernier;

	CompteurIncompatible = 2;
	filtreGlobalDependance.raz();
	dependanceModifiee = NON;
	//Nous utilisons les variables ci-dessus pour construire un vecteur binaire des dependances deja construites
	//en fonction de leur type... Si le NombreDependances d'un type de dependances atteint zero, on le retire du
	//vecteur binaire ClefDependance pour ne garder qu'un codage de celles presentes. De cette facon, on rapidement
	//savoir si l'ensemble des dependances d'un test existe avant meme d'effectuer le calcul de ce test.
	//Voir la methode TestClef dans avlfonc.cxx
	ClefDependances = 0;
	TraitsDependances->raz();
	//for (int inbdep=0;inbdep<=NbFonctionsExtraction;inbdep++)
	//  NombreDependances[inbdep]=0;

	NombreDependances.raz();
	TasDeListes->RecupereDernier(this);
	int j, dernier;
	ResultatFonction* rf;
	for (i = 1; i<INTERFONCTIONS.dernier; i++) {
		dernier = 0;
		if (INTERFONCTIONS[i] != NULL) {
			for (j = 0; j<INTERFONCTIONS[i]->dernier; j++) {
				rf = (*INTERFONCTIONS[i])[j];
				if (rf == NULL || rf->data != OUI)
					break;
				ClefDependances |= rf->Fx->Val;
				dernier++;
			}
			INTERFONCTIONS[i]->dernier = dernier;
		}
	}

	//Il ne faut pas nettoyer dans les structures les dependances de type BD
	for (i = 0; i<PREMIERFONCTIONS.dernier; i++) {
		if (PREMIERFONCTIONS[i] != NULL) {
			dernier = 0;
			for (j = 0; j<PREMIERFONCTIONS[i]->dernier; j++) {
				rf = (*PREMIERFONCTIONS[i])[j];
				if (rf == NULL || rf->data != OUI)
					break;
				dernier++;
			}
			PREMIERFONCTIONS[i]->dernier = dernier;
		}
	}

	for (i = 0; i<SECONDFONCTIONS.dernier; i++) {
		if (SECONDFONCTIONS[i] != NULL) {
			dernier = 0;
			for (j = 0; j<SECONDFONCTIONS[i]->dernier; j++) {
				rf = (*SECONDFONCTIONS[i])[j];
				if (rf == NULL || rf->data != OUI)
					break;
				dernier++;
			}
			SECONDFONCTIONS[i]->dernier = dernier;
		}
	}

	if (INTERFONCTIONS[0] != NULL) {
		dernier = 0;
		for (j = 0; j<INTERFONCTIONS[0]->dernier; j++) {
			rf = (*INTERFONCTIONS[0])[j];
			if (rf == NULL || rf->data != OUI)
				break;
			dernier++;
		}

		for (; j<INTERFONCTIONS[0]->dernier; j++) {
			rf = (*INTERFONCTIONS[0])[j];
			if (rf != NULL)
				RendreResFonc(rf);
		}

		INTERFONCTIONS[0]->dernier = dernier;
	}

	INTERVICTOIRE = NULL;
	LINTERVICTOIRE = NULL;
}

//Nous allons utiliser un test de langue, tres different de celui prealablement implante
//Nous allons effectuer une analyse de la phrase avec chaque tokenizer. La langue pour laquelle
//nous avons le plus lecture est la bonne...
//De plus, nous allons effectuer cette analyse pour toutes les autres grammaires a l'exception
//du parseur courant
float TestAmbiguite(VECTA<ListeVoc*>& phrase, trait* guesser) {
	float compte = 0;
	for (int i = 0; i<phrase.dernier; i++) {
		if (phrase[i]->global != NULL && phrase[i]->global->valeur->contient(guesser->valeur))
			compte++;
	}
	float perc = (compte / phrase.dernier) * 100;
	return perc;
}

int GlobalParseur::TestLangue(string& laphrase) {
	choix_ambigue = "";
	if (encourspourlangue == OUI)
		return iParseurCourant;
	char ch[10];

	taille_phrase = phrase.dernier;
	if (guesser == NULL) {
		sprintf(ch, "%d", iParseurCourant);
		string err = CHAINES[379];
		err += " Grammar:";
		err += ch;
		erreur((char*)err.c_str());
	}

	int choix = iParseurCourant;
	int ipc = index_parseur_courant;
	float base = TestAmbiguite(phrase, guesser);
	if (base == 0)
		return iParseurCourant;
	encourspourlangue = OUI;
	int i;
	GlobalParseur* leParseur;
	char mode = DESAMBIGUISATION;
	for (i = 0; i<gVecteurParseurs.dernier; i++) {
		if (i == iParseurCourant)
			continue;

		leParseur = gVecteurParseurs[i];

		if (leParseur->guesser == NULL) {
			sprintf(ch, "%d", i);
			string err = CHAINES[379];
			err += " Grammar:";
			err += ch;
			erreur((char*)err.c_str());
		}

		index_parseur_courant = i;

		//leParseur->DESAMBIGUISATION=OUI;
		//string chaine=laphrase;
		leParseur->ParametresAffichage &= ~TESTER_LANGUE;
		leParseur->StringAssociation((uchar*)laphrase.c_str());
		leParseur->encourspourlangue = OUI;
		leParseur->taille_phrase = -1;
		leParseur->SentenceAnalysis(NON, NON);

		if (leParseur->ambiguite<base) {
			base = leParseur->ambiguite;
			choix = i;
		}
		else
		if (leParseur->ambiguite == 0) {
			choix_ambigue += "_";
			choix_ambigue += leParseur->langue_de_la_grammaire;
		}
	}

	//On nettoie les contextes en attente non pertinent
	for (i = 0; i<gVecteurParseurs.dernier; i++) {
		leParseur = gVecteurParseurs[i];
		if (leParseur->taille_phrase != -1)
			leParseur->phrase.dernier = leParseur->taille_phrase;
		leParseur->encourspourlangue = NON;
		if (i != choix)
			leParseur->Detruit();

		leParseur->ParametresAffichage |= TESTER_LANGUE;
		//leParseur->DESAMBIGUISATION=mode;
	}

	index_parseur_courant = ipc;

	encourspourlangue = NON;
	return choix;
}


char GlobalParseur::TesterUneLangue() {

	if (TestOptions(TESTER_LANGUE)) {
		string unephrase = phrase[0]->surfacebrute;
		int iph;
		for (iph = 1; iph<phrase.dernier; iph++) {
			if (phrase[iph - 1]->borneDroite != phrase[iph]->borneGauche)
				unephrase += " ";
			unephrase += phrase[iph]->surfacebrute;
		}
		//l'initialisation ne se fait qu'une fois
		//        if (initialiser_guesser==NON) {
		//            init_guesser();
		//            initialiser_guesser=OUI;
		//        }
		//        guess_language((uchar*)(unephrase.c_str()), langue_de_la_phrase, guesser_full, trig_shortword);
		//        char* pt=strchr(langue_de_la_phrase,'_');
		//        if (pt!=NULL)
		//            *pt=0;

		int i_num_mot = 0;
		long i_baseOffset = 0;
		long i_baseOffsetCar = 0;

		if (phrase.dernier != 0) {
			i_num_mot = phrase[0]->num_mot;
			i_baseOffset = phrase[0]->borneGauche;
			i_baseOffsetCar = phrase[0]->borneGaucheCar;
		}

		int choix = TestLangue(unephrase);

		//si la langue ne correspond pas a celle de la grammaire, c'est une erreur
		//on n'analyse pas cette langue...
		//Sauf, si une grammaire est dispo en memoire pour la traiter
		if (choix != iParseurCourant) {
			//si nous trouvons une grammaire pour cette langue
			//Alors nous l'utilisons pour analyser cette phrase
			int ipc = index_parseur_courant;
			GlobalParseur* parseur_ancien = parseur_courant;
			GlobalParseur* leParseur = gVecteurParseurs[choix];
			if (i_baseOffset != 0) {
				for (iph = 0; iph<leParseur->phrase.dernier; iph++) {
					leParseur->phrase[iph]->borneGauche += i_baseOffset;
					leParseur->phrase[iph]->borneGaucheCar += i_baseOffsetCar;
					leParseur->phrase[iph]->borneDroite += i_baseOffset;
					leParseur->phrase[iph]->borneDroiteCar += i_baseOffsetCar;
				}
			}
			index_parseur_courant = choix;
			ostream* os_ancien = leParseur->OS;
			parseur_courant = leParseur;
			leParseur->num_phrase = num_phrase;
			leParseur->num_mot = i_num_mot;
			leParseur->baseOffset = i_baseOffset;
			leParseur->baseOffsetCar = i_baseOffsetCar;
			char* ancien_fichier_en_cours = leParseur->fichier_en_cours;
			leParseur->fichier_en_cours = fichier_en_cours;
			TVecteur paramaff = leParseur->ParametresAffichage;
			TVecteur paramaffdef = leParseur->ParametresAffichageDefini;
			char gaff = leParseur->gAfficherResultats;
			char gaffdef = leParseur->gAfficherResultatsDefini;


			leParseur->ParametresAffichage = ParametresAffichage;
			//On retire evidemment le mode TESTER_LANGUE
			leParseur->ParametresAffichage &= ~TESTER_LANGUE;
			leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;

			leParseur->gAfficherResultats = gAfficherResultats;
			leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;

			int xml_ancien = leParseur->XML;
			XipResult* ancien_xipresult = leParseur->xipresult;

			leParseur->xipresult = xipresult;
			leParseur->appliqueGrammaire = appliqueGrammaire;
			leParseur->appliqueGrammaireDefini = appliqueGrammaire;
			leParseur->OS = OS;
			leParseur->XML = XML;

			if (leParseur->AppelExterne == NULL)
				leParseur->AppelExterne = AppelExterne;

			//On cree une entree temporaire pour cette chaine
			//string chaine=unephrase;
			leParseur->StringAssociation((uchar*)unephrase.c_str());

			char lafin = leParseur->FinAnalyse;
			leParseur->FinAnalyse = NON;
			leParseur->Parse(OUI);
			leParseur->Detruit();
			leParseur->FinAnalyse = lafin;

			leParseur->ParametresAffichage = paramaff;
			leParseur->ParametresAffichageDefini = paramaffdef;
			leParseur->gAfficherResultats = gaff;
			leParseur->gAfficherResultatsDefini = gaffdef;

			leParseur->fichier_en_cours = ancien_fichier_en_cours;
			leParseur->XML = xml_ancien;
			leParseur->xipresult = ancien_xipresult;
			leParseur->OS = os_ancien;
			parseur_courant = parseur_ancien;
			index_parseur_courant = ipc;
			return NON;
		}
	}
	return OUI;
}

/*-------------------------------------------------------------------------*/
void GlobalParseur::Parse(char sauve_resultat, char construit) {

	//Dans ce cas, aucune analyse, on s'arrete ici...
	if (encourspourlangue == OUI) {
		ambiguite = TestAmbiguite(phrase, guesser);
		return;
	}

#ifdef CRFTAGGER 
	if (crftagger != NULL) {
		char** wordSequence = new char*[phrase.size() + 1];
		char** predictedTag = new char*[phrase.size() + 1];
		int iph;
		for (iph = 0; iph<phrase.size(); iph++)
			wordSequence[iph] = phrase[iph]->surface;

		int nbelements = TaggingTokens(crftagger, wordSequence, predictedTag, phrase.size());

		ListeVoc* voc;
		Traits* tlocal;
		string tag;
		for (iph = 0; iph<nbelements; iph++) {
			tlocal = new Traits(this);
			tag = predictedTag[iph];
			Cat* X = AnalyseTrait(tag, tlocal);
			if (X != NULL) {
				//Dans ce cas, il faut trouver la bonne categorie
				voc = phrase[iph];
				bool trouve = false;
				for (int ivoc = 0; ivoc<voc->nbX; ivoc++) {
					if (voc->X[ivoc] == X && ITR(this, tlocal, voc->tr[ivoc], NONECRIT)) {
						trouve = true;
						voc->tr[ivoc]->ou(traitcrfselection);
					}
				}
				//Si cela ne correspond a rien de connu... On rajoute tout le stock
				if (!trouve) {
					voc->nbX++;
					voc->X.ajoute(X);
					voc->lemme.ajoute(DupliqueChaine(voc->lemme.fin()));
					voc->garder.ajoute(voc->garder.fin());
					voc->tag.ajoute(DupliqueChaine(voc->tag.fin()));
					tlocal->ou(traitcrfselection);
					tlocal->ou(traittaggerguessed);
					voc->tr.ajoute(tlocal);
				}
				else
					delete tlocal;
			}
		}

		CleanTags(predictedTag, nbelements);
		delete[] wordSequence;
		delete[] predictedTag;
	}
#endif

	Ajusteoffset();

	if (FinAnalyse == OUI && ListeCoupeSequence.dernier)
		EndOfSentence(NULL, UTILISE);

	//dans le cas ou notre test sur les langues echoue
	//on n'analyse pas la phrase
	if (TesterUneLangue() == NON)
		return;

	index_parseur_courant = iParseurCourant;
	INTERVICTOIRE = NULL;
	regles_appliquees.raz();
	gBaliseDebut->raz();
	DeductionFinale = OUI;
	//Dans le cas de l'analyse d'un fichier XML, on ne remet pas a jour les traits
	if (XML != OUTPUTLIBXML)
		gBaliseTraits->raz();

	memoiresem.max++;
	demarre_regle = 0;
	demarre_couche = -1;
	aller_label = NULL;

	if (MaxSentence != -1 && num_phrase>MaxSentence)
		return;

	//Il s'agit d'une execution qui n'a lieu qu'une fois en debut d'analyse pour la premiere phrase
	if (DeductionInitiale == OUI) {
		DeduitFonction(-1, DEDUCTIONINITIALE, NON);
		DeductionInitiale = NON;
	}

	//Tout d'abord on reconstruit notre phrase

	if (phrase.dernier > 0) {

		//dans le cas ou l'on doit tester la langue
		if (appliqueGrammaire != NON)
			Execute(sauve_resultat, construit);
		else
		if (xipresult != NULL) {
			ConsListePhraseAvecDesamb(construit);
			TraiteXipResult();
			Detruit();
		}
	}

	if (TestOptions(AFFICHAGE_CALCUL_DISTANCE) == OUI) {
		hmap<int, hmap<int, int> > dists;
		hmap<int, hmap<int, int> > adists;
		int idn, idnn, i, j;
		for (i = 0; i < NoeudsLexicaux.dernier; i++) {
			idn = NoeudsLexicaux[i]->num_noeud;
			for (j = 0; j < NoeudsLexicaux.dernier; j++) {
				idnn = NoeudsLexicaux[j]->num_noeud;
				if (i == j) {
					dists[idn][idnn] = 0;
					adists[idn][idnn] = 0;
				}
				else {
					dists[idn][idnn] = NoeudsLexicaux[i]->distance(NoeudsLexicaux[j]);
					adists[idn][idnn] = NoeudsLexicaux[i]->distancearbre(NoeudsLexicaux[j]);
				}
			}
		}
		distances.push_back(dists);
		distancesarbre.push_back(adists);
	}

	//Nous calculons le temps entre deux phrases
	//Le temps initial nous est fourni avant l'analyse de la première phrase
	//De cette façon nous pouvons tenir compte de toutes les couches successives
	//telle que NTM
	if (calcultemps == 2) {
		if ((num_phrase % 100) == 0)
			*flot_erreur << "Sentence:" << num_phrase << endl;
		static time_t tempsintermediaire;
		time(&tempsintermediaire);
		double duree = difftime(tempsintermediaire, tempsbasephrase);
		if (duree >= 10) {
			cout << CHAINES[243] << ":" << duree << "s" << endl;
			*flot_erreur << CHAINES[243] << ":" << duree << "s" << endl;
		}
		//On réinitialise le temps de base pour la phrase suivante...
		time(&tempsbasephrase);
	}

	if (Sortie != NULL)
		Sortie->flush();

	if (Trace != NULL)
		Trace->flush();

	//Apres une analyse on remet toujours les variables a leur etat anterieur
	CalculFonction = CalculFonctionDefini;
	DESAMBIGUISATION = DESAMBIGUISATIONDefini;
	appliqueGrammaire = appliqueGrammaireDefini;
	ParametresAffichage = ParametresAffichageDefini;
	gAfficherResultats = gAfficherResultatsDefini;
	parse_phrase = OUI;

	if (affiche_regle_appliquee != NON)
		AfficheLesStats();
}

/*-------------------------------------------------------------------------*/
void GlobalParseur::Execute(char sauve_resultat, char construit) {

	double delai = 0;

	if (phrase.dernier == 0)
		return;


	float maxX = 5, maxY = 5;
	int i;

	ostream* os = &cout;

	if (OS != NULL)
		os = OS;

	if (phrase.dernier) {

		CompteSolution = num_phrase;
		INTERVICTOIRE = NULL;

		ConsListePhraseAvecDesamb(construit);
	}

	ExecuteExtraction();


	int numerote = 0;

	if (DESAMBIGUISATION == OUI) {
		ListeVoc* v;
		char* pt;
		int iphrase = 0;
		int iph;


#ifdef XIPLIBXML
		//Output XML
		if (XML == OUTPUTLIBXML && gAfficherResultats == OUI) {
			XmlLunit = CreationNoeudXML(XmlResult, "LUNIT", NULL);
			if (langue_de_la_grammaire != NULL)
				AjouteAttributValeur(XmlLunit, "language", langue_de_la_grammaire);
			else
				AjouteAttributValeur(XmlLunit, "language", "Unknown");

			while (INTERVICTOIRE != NULL) {
				(INTERVICTOIRE->fils)->num_noeud = numerote++;
				/** Start MCR added code for passage XML output **/
				(INTERVICTOIRE->fils)->ConstruitNoeudXML(this, XmlLunit);
				INTERVICTOIRE = INTERVICTOIRE->frere;
			}
			// output relations
			if (CalculFonction) {
				int numFonction = 0;
				for (i = 1; i<INTERFONCTIONS.dernier; i++) {
					if (INTERFONCTIONS[i] != NULL) {
						for (int j = 0; j<INTERFONCTIONS[i]->dernier; j++) {
							if ((*INTERFONCTIONS[i])[j] != NULL) {
								(*INTERFONCTIONS[i])[j]->ConstruitXML(numFonction);
							}
						}
					}
				}
			}
			/** End MCR added code for passage XML output **/
			return;
		}

#endif
		//Output XML
		if (XML == OUI && gAfficherResultats == OUI) {
			while (INTERVICTOIRE != NULL) {
				(INTERVICTOIRE->fils)->num_noeud = numerote++;
				(INTERVICTOIRE->fils)->SauveResultatXML(this, *os);
				INTERVICTOIRE = INTERVICTOIRE->frere;
			}
			return;
		}

		//Autre Output
		if (gAfficherResultats == OUI &&
			(TestOptions(AFFICHAGE_COMPLET) || TestOptions(AFFICHAGE_REDUIT))) {
			for (iph = 0; iph<phrase.dernier; iph++) {
				v = phrase[iph];
				if (TestAfficheFonction(AFFICHE_TRAITS)) {
					//Affichage des traits complets...    
					Chaine_traits[0] = 0;
					Liste* L = v->L;
					if (L != NULL) {
						(L->traitsactif)->TraitsEnChaine(this, Chaine_traits);
						*os << "==> " << ConversionChaine(this, Chaine_traits) << Endl;
					}
				}

				for (i = 0; i<v->nbX; i++) {
					if (TestAfficheFonction(AFFICHE_TRAITS)) {
						*os << (v->X[i])->nom;
						if (v->tr[i] != NULL) {
							Chaine_traits[0] = 0;
							(v->tr[i])->TraitsEnChaine(this, Chaine_traits);
							*os << ConversionChaine(this, Chaine_traits);
						}
						*os << ": ";
					}

					if (TestOptions(AFFICHAGE_REDUIT)) {
						pt = strchr(v->tag[i], '\t');
						if (pt != NULL)
							*pt = 0;
						*os << ConversionChaine(this, v->tag[i]) << Endl;
						if (pt != NULL)
							*pt = '\t';
					}
					else
					if (TestOptions(AFFICHAGE_ENTREE)) {
						*os << ConversionChaine(this, v->tag[i]);
						if (!strrchr(v->tag[i], '\n'))
							*os << Endl;
					}
					else {
						pt = strchr(v->tag[i], '\t');
						if (pt != NULL) {
							pt++;
							char* ppt = strchr(pt, '\t');
							if (ppt != NULL)
								*ppt = ' ';
							*os << ConversionChaine(this, pt) << Endl;
							*ppt = '\t';
						}
					}
				}
				*os << Endl;
			}
		}
	}
}

void GlobalParseur::AffichageArbreModeTexte(ostream* os, Noeud* fr) {
	NCoord* nco;
	float maxX = 5, maxY = 5;


	//On coupe en morceau
	if (TestOptions(ARBRE_SYNTAGME))
		fr = INTERVICTOIRE->fils;
	Noeud lepere;

	int compteur = 1;
	string chc;
	lepere.surface = STR(chc);

	while (fr != NULL) {
		maxX = 5; maxY = 5;
		Noeud* svf = fr->frere;
		fr->frere = NULL;

		if (TestOptions(ARBRE_SYNTAGME)) {
			chc = INTERVICTOIRE->surface;
			char ch[20];
			sprintf(ch, "(%d)", compteur);
			chc += ch;
			lepere.surface = STR(chc);
			lepere.fils = fr;
			nco = new NCoord(&lepere, &maxX, &maxY, 1);
		}
		else
			nco = new NCoord(fr, &maxX, &maxY, 1);

		InitEcran();
		nco->Dessine(this);
		if (TestOptions(ARBRE_COLONNE))
			AfficheCoupe(*os, (long)maxX, (long)maxY);
		else
			AfficheEcran(*os, (long)maxX, (long)maxY);
		delete nco;
		fr->frere = svf;
		fr = svf;
		compteur++;
	}
}

void GlobalParseur::ExecuteExtraction() {

	double delai = 0;

	ostream* os = &cout;

	if (OS != NULL)
		os = OS;


	if (INTERVICTOIRE != NULL) {

		for (int ii = 0; ii<NbCats; ii++)
			IndexParCat[ii].dernier = 0;

		//Construction de IndexParCat;
		NoeudsLexicaux.raz();
		INTERVICTOIRE->ConstruitIndex(this, NULL, OUI, 0);
		if (aller_label != label_stop && aller_label != label_terminal) {
			demarre_regle = 0;
			demarre_couche = -1;
			aller_label = NULL;

			if (TRACE || TRACEDEPENDANCE)
				trace << Endl << Endl << "EXTRACTION FONCTIONS" << Endl << Endl;

			for (int laCouche = 0; laCouche <= laCoucheDeduction; laCouche++) {
				switch (CouchesDeduction[laCouche]) {
				case REGLES_DEDUCTION:
					if (DeductionLicense == OUI)
						DeduitFonction(laCouche, DEDUCTIONCOURANTE, NON);
					break;
				case REGLES_SEMANTIQUE:
					if (SemantiqueLicense == OUI && CalculFonction != NON)
						AppliqueSemantiques(laCouche, OUI);
					break;
				case REGLES_GENERATION:
				case REGLES_EXTRACTION:
					if (DeductionLicense == OUI) {
						DeduitFonction(laCouche, DEDUCTIONCOURANTE, NON);
						if (laCouche == 0)
							DeduitFonction(laCouche, DEDUCTIONGENERATION, NON);
					}
				}
			}
		}

		if (INTERVICTOIRE && INTERVICTOIRE->fils) {
			Noeud* fin = INTERVICTOIRE->fils->dernier;

			//Des noeuds ont pu etre cree pendant le calcul des dependances
			for (int ino = 0; ino<NoeudsDependance.dernier; ino++) {
				//on les rajoute sous la racine...
				fin->frere = &NoeudsDependance[ino]->N;
				fin = fin->frere;
			}
		}

		TVecteur AffichageArbre = ParametresAffichage & AFFICHAGE_ARBRE;
		if (AffichageArbre)
				AffichageArbreModeTexte(os, INTERVICTOIRE);

		if (ARBREIND) {
			sortie << endl << endl;
			INTERVICTOIRE->affiches(this, 0);
		}
	}

	if (xipresult != NULL)    {
		TraiteXipResult();
	}
	else {
		AffichageResultat();
		os->flush();
		if (modeOutC == OUI) {
			printf(((ostringstream*)os)->str().c_str());
			fflush(stdout);
			delete OS;
			OS = new ostringstream;
		}
	}
}

Noeud* GlobalParseur::ProduitListe(ListeVoc* alter) {
	if (alter == NULL)
		return NULL;
	int iph = alter->idx;
	if (alter->L != NULL) {
		alter->L->N.annule();
		return &alter->L->N;
	}
	Liste* L = TasDeListes->fournir(this, alter->X[0], NULL);
	L->voc = alter;
	alter->L = L;

	L->N.gauche = alter->borneGauche;
	L->N.droite = alter->borneDroite;
	L->N.gauchecar = alter->borneGaucheCar;
	L->N.droitecar = alter->borneDroiteCar;
	L->N.num_gauche = iph;
	L->N.num_droite = iph;

	Liste* Lfils = TasDeListes->fournir(this, alter->X[0], NULL);

	Noeud* noeud = &Lfils->N;

	noeud->surface = (char*)alter->surface;
	noeud->lemme = (char*)alter->lemme[0];

	noeud->num_mot = alter->num_mot;
	noeud->clef = (alter->X[0])->Val;

	noeud->gauche = alter->borneGauche;
	noeud->droite = alter->borneDroite;
	noeud->gauchecar = alter->borneGaucheCar;
	noeud->droitecar = alter->borneDroiteCar;
	noeud->num_gauche = noeud->num_mot;
	noeud->num_droite = noeud->num_mot;

	L->N.fils = noeud;
	noeud->pere = &L->N;
	L->traitsbase->ou(TraitNoeudTerminal);
	L->traitsactif->ou(TraitNoeudTerminal);
	return &L->N;
}

char GlobalParseur::ConsListePhraseAvecDesamb(char construit) {
	Noeud* noeud;
	Liste* L = NULL;
	int uneclef;

	LListe& Phrase = glisteDesPhrases[0]; //les couches vont de 0 a MaxCouches...
	if (construit == NON) {
		Phrase.dernier = 0;
		TasDeListes->Raz();
		NoeudsTemporaires.raz();

		ListeVoc* alter;

		//Cette structure n'est utilise que dans le cas de la desamb
		//On y stocke l'ensemble des traits fusionnes
		Traits tousTraits(this);
		int it;
		VECTA<ListeVoc*> listealter;


		for (int iph = 0; iph<phrase.dernier; iph++) {
			alter = phrase[iph];
			if (presencetoken == NON)
				alter->num_mot = iph;
			char premierelement = OUI;
			ListeVoc* lv;
			int e;
			listealter.raz();
			listealter.ajoute(alter);

			if (alter->suivants.dernier) {
				//Pour avoir toujours le meme ordre des lectures...
				for (e = 0; e<alter->suivants.dernier; e++) {
					if (alter->suivants[e] != NULL) {
						uneclef = 0;
						lv = alter->suivants[e];
						it = 1;
						while (lv != NULL) {
							for (int ivh = 0; ivh<lv->X.dernier; ivh++)
								uneclef += lv->X[ivh]->val;
							lv->num_mot = iph + it;
							lv->passage = premierelement;
							listealter.ajoute(lv);
							lv = lv->suivant;
							it++;
						}
					}
					premierelement = NON;
				}
			}

			premierelement = OUI;
			for (e = 0; e<listealter.dernier; e++) {
				alter = listealter[e];
				//seul le premier element peut contribuer aux traits globaux...
				Vecteur filtreur(iTailleVect);
				char modif = NON;
				for (it = 0; it< alter->nbX; it++) {

					if (iph == 0)
						(alter->tr[it])->ou(DebutPhrase);

					if (iph + 1 == phrase.dernier)
						alter->tr[it]->ou(FinPhrase);

					//Dans le cas ou il s'agit d'un parse normal
					//on teste les V-regles.
					AppVregle(alter->tr[it], alter->X[it], RCT);
					AppVregle(alter->tr[it], alter->X[it], STD);


					//On fusionne l'ensemble des traits...
					if (FusionneTraits(&tousTraits, alter->tr[it], filtreur))
						modif = OUI;

					if (alter->tr[it]->permanent != NULL) {
						if (tousTraits.permanent == NULL)
							tousTraits.permanent = new Vecteur(alter->tr[it]->permanent);
						else
							tousTraits.permanent->ou(alter->tr[it]->permanent);
					}

					//tousTraits.ou(alter->tr[it]);
				}

				if (modif == OUI)
					NettoieTraitsIncoherent(&tousTraits, filtreur);

				if (tousTraits.permanent != NULL) {
					tousTraits.valeur->ou(tousTraits.permanent);
					tousTraits.filtre->ou(tousTraits.permanent);
				}

				L = TasDeListes->fournir(this, alter->X[0], &tousTraits);

				tousTraits.razp();


				L->voc = alter;
				alter->L = L;
				L->index = alter->CalculIndex();

				L->SauveTraits();//On recopie traitsbase dans traitsactif 


				L->N.gauche = alter->borneGauche;
				L->N.droite = alter->borneDroite;
				L->N.gauchecar = alter->borneGaucheCar;
				L->N.droitecar = alter->borneDroiteCar;
				L->N.num_gauche = alter->num_mot;
				L->N.num_droite = alter->num_mot;


				//seul le premier element est officiellement integre dans
				//la phrase...
				if (premierelement == OUI || alter->passage == OUI)
					Phrase.ajoute(L);

				alter->passage = NON;

				Liste* Lfils = TasDeListes->fournir(this, alter->X[0], NULL);

				noeud = &Lfils->N;

				noeud->surface = (char*)alter->surface;
				noeud->lemme = (char*)alter->lemme[0];

				noeud->num_mot = alter->num_mot;
				noeud->clef = (alter->X[0])->Val;

				noeud->gauche = alter->borneGauche;
				noeud->droite = alter->borneDroite;
				noeud->gauchecar = alter->borneGaucheCar;
				noeud->droitecar = alter->borneDroiteCar;
				noeud->num_gauche = noeud->num_mot;
				noeud->num_droite = noeud->num_mot;

				L->N.fils = noeud;
				noeud->pere = &L->N;
				L->traitsbase->ou(TraitNoeudTerminal);
				L->traitsactif->ou(TraitNoeudTerminal);

				premierelement = NON;
			}
		}
	}

	LINTERVICTOIRE = TasDeListes->fournir(this, &Categories[1]->X, NULL);

	if (appliqueGrammaire == NON) {
		MAJInterVictoire(Phrase);
		return OUI;
	}


	int i;

	TVecteur laClef = 0;
	int nbn = 1;
	for (i = Phrase.dernier - 1; i >= 0; i--) {
		L = Phrase[i];
		laClef |= L->index;
		L->N.clef_suivant = laClef;
		L->N.nombre = nbn++;
		if (L->voc->nbX == 1) {
			L->traitsactif->filtre->vecteur[0] |= 16;
			L->traitsactif->valeur->vecteur[0] |= 16;
			L->traitsbase->filtre->vecteur[0] |= 16;
			L->traitsbase->valeur->vecteur[0] |= 16;
		}
	}


	//Application d'un jeu de regles special pour simplifier 
	//les categories multiples pour un mot donne
	if (TAGGING == OUI || DESAMBIGUISATION == OUI)
		AppliqueSimplification(Phrase, -1);

	if (Phrase.dernier > 0)
		ActiveCouche(Phrase, 0);

	if (DESAMBIGUISATION == OUI && XML != NON) {
		INTERVICTOIRE = &Phrase[0]->N;
		noeud = INTERVICTOIRE;
		for (i = 1; i<Phrase.dernier; i++) {
			noeud->frere = &Phrase[i]->N;
			noeud = noeud->frere;
		}
	}

	return OUI;
}


void GlobalParseur::ParseFinal() {
	FinAnalyse = OUI;
	while (phrase.dernier != 0) {
		Parse(OUI);
		Detruit();
		num_phrase++;
	}

	FinAnalyse = NON;
}

void extrait_nom_fichier(char* nom, string& reponse) {

	char* last_slash = strrchr(nom, SEP);

	if (last_slash != NULL) {
		*last_slash = 0;
		reponse = last_slash + 1;
		*last_slash = SEP;
	}
	else
		reponse = nom;
}

void trimage(string s, string& res) {
	for (int k = 0; k<s.size(); k++) {
		if (s[k]>32)
			res += s[k];
	}
}

void comparegrammaire(GlobalParseur* g1, GlobalParseur* g2, char* repertoire, char* grm1, char* grm2) {
	map<string, char> noms;
	vector<string> communs;
	string nom;
	int i;
	for (i = 0; i<g1->Nom_regle.dernier; i++) {
		extrait_nom_fichier(g1->Nom_regle[i], nom);
		noms[nom] = 1;
	}
	//On detecte les noms communs entre les deux grammaires
	for (i = 0; i<g2->Nom_regle.dernier; i++) {
		extrait_nom_fichier(g2->Nom_regle[i], nom);
		map<string, char>::iterator iter = noms.find(nom);
		if (iter != noms.end())
			communs.push_back(nom);
	}

	for (int c = 0; c<communs.size(); c++) {
		vector<string> vstat1;
		vector<string> vstat2;
		vector<string> vdisp1;
		vector<string> vdisp2;
		map<string, char> map1;
		map<string, char> map2;
		int ind;

		for (i = 0; i<g1->statistiques.dernier; i++) {
			extrait_nom_fichier(g1->statistiques[i]->regle->label->dr->nom_fichier, nom);
			ind = g1->statistiques[i]->regle->identificateur;
			if (nom == communs[c]) {
				ostringstream os;
				g1->FormatRuleText(os, ind, 0);
				string s = "";
				trimage(os.str(), s);
				vstat1.push_back(s);
				map1[s] = i;
				ostringstream osbis;
				g1->FormatRuleText(osbis, ind, 1);
				vdisp1.push_back(osbis.str());
			}
		}
		for (i = 0; i<g2->statistiques.dernier; i++) {
			extrait_nom_fichier(g2->statistiques[i]->regle->label->dr->nom_fichier, nom);
			ind = g2->statistiques[i]->regle->identificateur;
			if (nom == communs[c]) {
				ostringstream os;
				g2->FormatRuleText(os, ind, 0);
				string s = "";
				trimage(os.str(), s);
				vstat2.push_back(s);
				map2[s] = i;
				ostringstream osbis;
				g2->FormatRuleText(osbis, ind, 1);
				vdisp2.push_back(osbis.str());
			}
		}
		int posdansg2 = 0;
		nom = repertoire;
		nom += communs[c];
		nom += ".txt";
		ofstream fichier(nom.c_str());
		char deja = NON;
		int maxstat = vstat1.size();
		if (maxstat<vstat2.size())
			maxstat = vstat2.size();

		for (i = 0; i<maxstat; i++) {
			//Il y a une difference...
			//Est-ce que cette regle appartient a la deuxieme grammaire?
			if (i<vstat1.size()) {
				map<string, char>::iterator iter = map2.find(vstat1[i]);
				if (iter == map2.end()) {
					//il s'agit donc d'une regle de plus dans g1			
					fichier << "//-------------------------------------" << endl;
					fichier << "//From Grammar:" << grm1 << endl;
					fichier << vdisp1[i] << endl << endl;
				}
			}
			//Il y a une difference...
			//Est-ce que cette regle appartient a la deuxieme grammaire?
			if (i<vstat2.size()) {
				map<string, char>::iterator iter = map1.find(vstat2[i]);
				if (iter == map1.end()) {
					//il s'agit donc d'une regle de plus dans g2
					fichier << "//-------------------------------------" << endl;
					fichier << "//From Grammar:" << grm2 << endl;
					fichier << vdisp2[i] << endl << endl;
				}
			}
		}
		fichier.close();
	}
}

void GlobalParseur::raz() {
	nettoieLectures();
	wo.raz();
}

word_offset* GlobalParseur::fournirWordOffset() {
	word_offset* w;
	int idx = lectures.dernier;
	if (idx >= lectures.taille)
		lectures.ajuste(lectures.taille + 10);
	
	w = lectures.vecteur[idx];
	lectures.dernier++;
	if (w == NULL) {
		w = new word_offset;
		w->pos = idx;
		lectures.vecteur[idx] = w;
		return w;
	}

	w->raz();
	return w;
}

void GlobalParseur::nettoieLectures() {
	for (int i = 0; i < lectures.taille; i++) {
		if (lectures.vecteur[i] != NULL)
			lectures.vecteur[i]->raz();
	}
	lectures.dernier = 0;
}

void GlobalParseur::libereLectures(int t) {
	for (int i = t; i<lectures.dernier; i++)
		lectures[i]->raz();
	lectures.dernier = t;
}

void GlobalParseur::ListeTagsInitialisation() {
	enregListeTags.ajuste(200);
	enregListeTags.dernier = 0;
	char* entree;
	for (int ilt = 0; ilt<enregListeTags.taille; ilt++) {
		entree = enregListeTags.vecteur[ilt];
		if (entree == NULL) {
			entree = new char[taille_mot + 1];
			enregListeTags.affecte(ilt, entree);
		}
		entree[0] = 0;
	}
}
