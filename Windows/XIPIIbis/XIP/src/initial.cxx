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
filename   : initial.cxx
Date       : 10/01/1999
Purpose    : Initialisation of exported methods of GlobalParseur
Programmer : Claude ROUX
Reviewer   :
*/

char VERSION_XIP_BUILD[] = "13.04 \"build 18\"";

#ifdef WIN32

//#ifdef DEBUGLEAK
//#include <vld.h>
//#endif

#include <windows.h>
#undef NO_ERROR
#undef ERROR
#endif

#include "parsbin.h"

#ifdef XIPLIBXML
#include "varlibxml.h"
#endif

#include "graphe.h"
#include "ntmhmm.h"
#include "transduct.h"
#include "kif.h"
#include "kifversion.h"

#ifdef CRFTAGGER
extern "C" {
	void CleanTagger(void* tg);
}
#endif

extern char crypte_repertoire[1000];
extern char duplication_crypte;
void Recuperenom(char* nom, char* path);
void NePasNettoyer(bool);

void initlamajuscule();

void afficheversion() {
	cout << "XIP:" << VERSION_XIP_BUILD << "  KiF:" << KIFVERSION << endl;
}

kifmutex(protegexip, _protegexip);

void InitialisationMutex() {
	static bool init = false;
	if (init == false)
		KifCreateMutex(protegexip);
	init = true;
}

#ifdef NTMCONNECTION

//typedef struct lower_sigs_net {
//  ALPHABETptr low_sig;
//  NETptr net;
//} lower_sigma_net, *lower_sigma_netptr;



#include "lktypes.h"
#include "maintok.h"
#endif


#define MAX_NB_PATH 100
//ces deux variables permettent de garder une trace des variables d'environnement
char g_environ_var[MAX_NB_PATH + 1][1000];
VECTA <char*> g_environ_var_inconnues;
char g_environ_varname[MAX_NB_PATH + 1][100];
void Trim(char* chaine);
string& Trim(string& chaine);
string FieldName(int i);

int g_nbvarenv = 0;

string GRM_COURANT;
extern char AFFICHE_PHRASE_MULTIMOTS;
extern ostream* flot_erreur;
extern int ERREUR_LIGNES;
extern char ERREUR_FICHIER[1000];
extern long ERREUR_POSITION_DANS_FICHIER;
extern int MAXVIRTUELVALEURINITIALE;
extern vector<string> grmkifarguments;

void lance_erreur(char* message);

#if defined(WIN32)
char AFFICHAGEDOS = 0;
#endif
char afficheautomate = NON;

//extern char python_debug;
//extern int num_python_ligne;
//extern int num_fichier_python;

//Definition des seules variables globales autorisees dans Xip
VecteurDeParseurs gVecteurParseurs;
GlobalParseur* parseur_courant = NULL;
int index_parseur_courant = -1;
extern int kif_declencheur;

Exportation void ParseurRAZ() {
	gVecteurParseurs.parseurs.raz();
	gVecteurParseurs.dernier = 0;
	parseur_courant = NULL;
	index_parseur_courant = -1;
}

char AffectationVariable(Variable* v, double res, string val);
char RenvoieValeurVariable(Variable* v, string& val);
//char LoadGrmFiles(char* grmname,char*  ntm,char*  hmm,char*  grammar, char* genere,char* token,char* lookup,lookup_strategy& strategy,char* lookup_flags,char& conversion,VECTA<char*>& parameters,vector<string>&,vector<string>& blocking_rule_space,int&,char);
char LoadGrmFiles(Fichiergrm& fgrm, int& ITAILLEVECT, char premier);
char CheminComplet(char* chemin);

int ERREUR_SURVENUE;
ofstream* Trace = NULL;

#ifdef XIPPYTHON
PyObject* GlobalParseur::module_principal = NULL;
#endif

int teste_differentes_modifications[100];

Exportation void XipInsertionDifference(int test, int val) {
	if (test >= 100 || test<0)
		return;

	teste_differentes_modifications[test] = val;
}

Exportation void send_error(string message) {
	lance_erreur((char*)message.c_str());
}


void GetVarName(int i, string& s) {
	s = g_environ_var[i];
	int pos = s.find("=", 0);
	s = s.substr(pos + 1, s.length() - pos - 1);
}

#ifdef NTMCONNECTION
TokLOOK* charge_un_fst(GlobalParseur* p, FILE* stream, string sfilename, bool utf8);
void chargegeneration(GlobalParseur* gp, char* generefile) {
	if (generefile[0] != 0) {
		if (strstr(generefile, ".fst") != NULL) {
			FILE* stream = fopen(generefile, "rb");
			if (stream == NULL) {
				string message = "FST(001): Unknown file: ";
				message += generefile;
				send_error(message);
			}
			gp->tokdown = charge_un_fst(gp, stream, generefile, gp->UTF8());
		}
		else
		gp->tokdown = new TokLOOK(gp, generefile, "", 0, 0);
	}
}
#endif

GlobalParseur* ChoixParseur(Fichiergrm& fgrm, char xml, char* chemin) {

	GlobalParseur* gp = NULL;

#ifdef XIPLIBXML
	if (xml == OUI) {
		GlobalParseurXML* gxml = NULL;
		try {
			gxml = new GlobalParseurXML(fgrm.grammar, chemin);
			gxml->ValeurCoucheInitiale = fgrm.lacoucheinitiale;
			gxml->fichiersdata = fgrm.datafiles;
			if (fgrm.model[0])
				gxml->crfmodel = fgrm.model;
			if (fgrm.mkcls[0])
				gxml->crfmkcls = fgrm.mkcls;
			if (fgrm.brown[0])
				gxml->crfbrown = fgrm.brown;

			strcpy_s(gxml->nom_fichier_grm,1024, fgrm.grmname);
			return gxml;
		}
		catch (XIPError* lerreur) {
			if (gxml != NULL)
				delete gxml;
			throw lerreur;
		}
		return NULL;
	}
#endif


	if (fgrm.trans.size()) {
		GlobalParseurTrans* gtrans = NULL;
		try {
			gtrans = new GlobalParseurTrans(fgrm.trans, fgrm.hmm, fgrm.grammar, chemin);
			gtrans->ValeurCoucheInitiale = fgrm.lacoucheinitiale;
			gtrans->fichiersdata = fgrm.datafiles;
			if (fgrm.model[0])
				gtrans->crfmodel = fgrm.model;
			if (fgrm.mkcls[0])
				gtrans->crfmkcls = fgrm.mkcls;
			if (fgrm.brown[0])
				gtrans->crfbrown = fgrm.brown;
			if (fgrm.hmm[0] != 0) {
				gtrans->AFFICHE_ERREUR = OUI;
				gtrans->InitDisambText(fgrm.hmm);
				gtrans->AFFICHE_ERREUR = NON;
			}
			strcpy_s(gtrans->nom_fichier_grm,1024, fgrm.grmname);
			return gtrans;
		}
		catch (XIPError* lerreur) {
			if (gtrans != NULL)
				delete gtrans;
			throw lerreur;
		}
	}


#ifdef NTMCONNECTION
	if (fgrm.tokenize[0] != 0 && fgrm.lookup[0] != 0) {
		if (fgrm.strategy == NO_STRATEGY)
			lance_erreur("Error: Please select a strategy in the GRM file:\nstrategy:\nBREADTH|DEPTH\n");
		char type = 0;
		if (fgrm.strategy == BREADTH)
			type = 1;
		GlobalParseurTokenLookup* glob = NULL;
		try {
			glob = new GlobalParseurTokenLookup(fgrm.tokenize, fgrm.lookup, fgrm.hmm,
				fgrm.grammar, chemin, fgrm.lookup_flags, type);
			glob->ValeurCoucheInitiale = fgrm.lacoucheinitiale;
			glob->fichiersdata = fgrm.datafiles;
			glob->LoadTokenLookup();
			if (fgrm.model[0])
				glob->crfmodel = fgrm.model;
			if (fgrm.mkcls[0])
				glob->crfmkcls = fgrm.mkcls;
			if (fgrm.brown[0])
				glob->crfbrown = fgrm.brown;

			if (fgrm.hmm[0] != 0) {
				glob->AFFICHE_ERREUR = OUI;
				glob->InitDisambText(fgrm.hmm);
				glob->AFFICHE_ERREUR = NON;
			}
#ifdef NTMCONNECTION
			chargegeneration(glob, fgrm.genere);
#endif
			strcpy_s(glob->nom_fichier_grm,1024, fgrm.grmname);
			return glob;
		}
		catch (XIPError* lerreur) {
			if (glob != NULL)
				delete glob;
			throw lerreur;
		}
	}

	if (gp == NULL && fgrm.ntm[0] != 0) {
		GlobalParseurNTM* gntm = NULL;
		try {
			gntm = new GlobalParseurNTM(fgrm.ntm, fgrm.hmm, fgrm.grammar, chemin);
			gntm->ValeurCoucheInitiale = fgrm.lacoucheinitiale;
			gntm->fichiersdata = fgrm.datafiles;
			if (fgrm.model[0])
				gntm->crfmodel = fgrm.model;
			if (fgrm.mkcls[0])
				gntm->crfmkcls = fgrm.mkcls;
			if (fgrm.brown[0])
				gntm->crfbrown = fgrm.brown;

			gp = (GlobalParseur*)gntm;
			gntm->NTMLoadScript();
			if (fgrm.hmm[0] != 0) {
				gntm->AFFICHE_ERREUR = OUI;
				gntm->InitDisambText(fgrm.hmm);
				gntm->AFFICHE_ERREUR = NON;
			}
#ifdef NTMCONNECTION
			chargegeneration(gntm, fgrm.genere);
#endif
			strcpy_s(gntm->nom_fichier_grm,1024, fgrm.grmname);
			return gntm;
		}
		catch (XIPError* lerreur) {
			if (gntm != NULL)
				delete gntm;
			throw lerreur;
		}
	}
#endif

	if (fgrm.hmm[0] != 0) {
		GlobalParseurHMM* ghmm = NULL;
		try {
			ghmm = new GlobalParseurHMM(fgrm.hmm, fgrm.grammar, chemin);
			ghmm->ValeurCoucheInitiale = fgrm.lacoucheinitiale;
			ghmm->fichiersdata = fgrm.datafiles;
			ghmm->AFFICHE_ERREUR = OUI;
			ghmm->InitDisambText(fgrm.hmm);
			ghmm->AFFICHE_ERREUR = NON;
#ifdef NTMCONNECTION
			chargegeneration(ghmm, fgrm.genere);
#endif
			strcpy_s(ghmm->nom_fichier_grm,1024, fgrm.grmname);
			return ghmm;
		}
		catch (XIPError* lerreur) {
			if (ghmm != NULL)
				delete ghmm;
			throw lerreur;
		}
	}

	try {
		gp = new GlobalParseur(fgrm.grammar, chemin);
		gp->ValeurCoucheInitiale = fgrm.lacoucheinitiale;
		gp->fichiersdata = fgrm.datafiles;
		if (fgrm.model[0])
			gp->crfmodel = fgrm.model;
		if (fgrm.mkcls[0])
			gp->crfmkcls = fgrm.mkcls;
		if (fgrm.brown[0])
			gp->crfbrown = fgrm.brown;
#ifdef NTMCONNECTION
		chargegeneration(gp, fgrm.genere);
#endif
		strcpy_s(gp->nom_fichier_grm,1024, fgrm.grmname);
		return gp;
	}
	catch (XIPError* lerreur) {
		if (gp != NULL)
			delete gp;
		throw lerreur;
	}
	return NULL;
}



void GlobalParseur::ChoixUTF8(long conv) {
	ParametresAffichage |= conv;
	ParametresAffichageDefini = ParametresAffichage;
}

void GlobalParseur::renseigne(char* f) {
	if (fichier_en_cours != NULL)
		libere(fichier_en_cours);
	fichier_en_cours = DupliqueChaine(f);
}



void GlobalParseur::raz(char premier) {
	int i;

	ajoute_info_erreur[0] = 0;
	initlamajuscule();
	actif = NON;
	taille_phrase = -1;
	encourspourlangue = NON;
	reglecourante = NULL;
	premiergraphelibre = 0;
	compteur_iteration = 0;
	if (premier == OUI) {
#ifdef NTMCONNECTION
		tokdown = NULL;
#endif
		taille_mot = 200;
		traittaggerguessed = NULL;
		traithmmselection = NULL;
		traitcrfselection = NULL;
		iParseurCourant = -1;
		DESAMBIGUISATION = NON;
		DESAMBIGUISATIONDefini = DESAMBIGUISATION;
		TAGGING = NON;

		memoiresem.memoire.raz();

		fichieroptimise = NON;
		TRACETRAITS = NON;
		TRACEDEPENDANCE = NON;
		ARBREIND = NON;
		TRACE = NON;
		TRACENOEUD = NON;
		TRACEREGLE = NON;

		mathematique = NON;
		dependanceModifiee = NON;
		ClefDependances = 0;

		prolog = NON;
		CalculFonction = NON;
		CalculFonctionDefini = CalculFonction;
		repertoire[0] = 0;
		laLangue = ANGLAIS;
		cryptage = NON;
		encoursCryptage = NON;
	}

	derniere_erreur_survenue = -1;
	erreur_survenue = -1;
	hierarchies = NULL;
	relations = NULL;
	concepts = NULL;

	python_debug = NON;
	num_python_ligne = 0;
	num_fichier_python = 0;
	fullpythonlexical = NON;
	Numero_Ordre = -1;
	encodage_grammaire = LATIN_ENCODING;
	encodage_fst = LATIN_ENCODING;
	encodage_source = LATIN_ENCODING;
	encodage_courant = LATIN_ENCODING;
	commentaire_regle = "";
	DECOUPAGE = NULL;
	langue_de_la_phrase[0] = 0;
	version_grammaire[0] = 0;
	larbredestraits = NULL;
	debut_traits_dans_langue = -1;
	debut_lexique_dans_langue = -1;
	debut_regles_dans_langue = -1;

	langue_de_la_grammaire = NULL;
	TraitsDependances = NULL;
	CatEpsilon = NULL;
	nbInference = 0;
	nbSequence = 0;
	nbSimplifie = 0;
	nbMarqueur = 0;
	nbDI = 0;
	nbReconstruction = 0;
	nbTermes = 0;
	nbCartesien = 0;
	punctuations = 0;
	guesser = NULL;
	guesser_feature[0] = 0;
	Limite_Numero_Regle = (unsigned int)-1;
	Limite_Couche = (unsigned int)-1;
	fichier_en_cours = NULL;
	nom_procedure = NULL;
	ListeTagsInitialisation();
	parse_phrase = OUI;
	baseOffset = 0;
	baseOffsetToken = 0;
	presencetoken = NON;
	baseOffsetCar = 0;
	AppelExterne = NULL;
	donneesExternes = NULL;
	DeductionFinale = OUI;
	DeductionInitiale = OUI;
	AFFICHE_ERREUR = NON;
	spacename = NULL;
	currentspacename = NULL;

	UsageLimitee = NON;
	reglesmultimots = NON;

#ifdef XIPPYTHON
	num_module_python = 0;
	espace_local = NULL;
#endif


#ifdef XIPLIBXML
	basexmlreference = NULL;
	XmlResult = NULL;
	basexmlcourante = NULL;
	repertoireresultat = "";
	amorce_affichage = "";
	defautEvenement = XMLANALYSE;
	defautAction = XMLDEPENDENCY;
	CompareAvecDTD = SANSDESCRIPTION;
	if (premier)
		refdocument = NULL;
	else {
		if (refdocument != NULL)
			xmlFreeDoc(refdocument);
		refdocument = NULL;
	}
	xipdtd = NULL;
	XmlTagCat = NULL;
	XmlLunit = NULL;
	AppelExterneXml = NULL;
	donneesExternesXml = NULL;
#endif
	CompteurIncompatible = 2;
	LIGNESERREUR = -1;
	fenetre = -1;
	TraitsFiltreOntologie = NULL;
	ERREUR_SURVENUE = 0;
	DESTRUCTION = OUI;
	colonne = 80;
	appelFactorise = 0;
	tailleFactorise = 0;

	CategoriesCoupeSequence = 0;
	CategoriesCoupeSequenceInterieur = 0;
	strcpy_s(encodage,100, "UTF-8");


	ChunkerLicense = NON;
	DeductionLicense = NON;
	SemantiqueLicense = NON;
	ServeurLicense = NON;
	licenseTest = -1;
	reentrance = OUI;

	nbmotsmaximum = 150;

	iCategorie = 0;
	PremierCaractereBalise[0] = 0;

	numero_vocabulaire = 0;
	numero_procedure = 0;

	gAfficherResultats = OUI;
	gAfficherResultatsDefini = gAfficherResultats;

	LectureRegles = NON;
	TAG_NOUVEAU[0] = 0;
	tag_nouveau = NULL;
	GUESSED_TAG[0] = 0;

	X_SIZE = 1;
	Y_SIZE = 1;

	ParametresAffichage = AFFICHAGE_ARBRE_CHUNK | AFFICHAGE_ERREUR_EXECUTION;
	ParametresAffichageDefini = ParametresAffichage;
	enAttente[0] = 0;

	TasDeListes = NULL;

	Inconnu = NULL;
	MAJUSCULE = NULL;
	TOUTMAJUSCULE = NULL;
	MAJEURES = NULL;
	Defauts = NULL;
	ATTENTE = 0;
	MaxProfondeurAnalyse = 0;
	MinProfondeurAnalyse = 0;
	NbFonctionsExtraction = 1;
	Zonage = MoinsUn;
	FinAnalyse = NON;
	detruireDonnees = NON;
	donnees = NULL;
	laCoucheDeduction = 0;
	MaxSentence = -1;
	xipresult = NULL;
	commentaire[0] = 0;
	num_phrase = 0;
	nombre_mots = 0;
	num_mot = 0;
	num_token = 0;
	fichierDonnees = NULL;
	donneesATraiter = NULL;
	curseurDonnees = NULL;
	socketcourante = -1;
	SUITE = -1;
	OS = NULL;
	LectureParametres = NON;
	controle_max_regle = 0;
	max_controle_regle = 10;
	aller_label = NULL;
	demarre_regle = 0;
	demarre_couche = -1;
	laTrace = NON;
	bouclage = 0;
	commutateurs.raz();

	//Initialisation des categories presentes par couche
	//La couche 0 correspond a la couche recursive...
	for (i = 0; i<MaxCouches; i++) {
		//Cette variable enregistre la sequence des couches qui 
		//peut entremeler regles ID et regles de sequence...
		CouchesActives[i] = VIDE;
		CouchesDeduction[i] = VIDE;

		CategoriesDansCouche[i] = 0;
		CategoriesObligatoiresDansCouche[i] = 0;
		ArbVocComplexe[i] = NULL;
		ArbVoc[i] = NULL;
		ArbSurfaceComplexe[i] = NULL;
		ArbSurface[i] = NULL;
		motsmultiplescomplexes[MULTISURFACE][i] = NULL;
		motsmultiples[MULTISURFACE][i] = NULL;
		motsmultiplescomplexes[MULTILEMME][i] = NULL;
		motsmultiples[MULTILEMME][i] = NULL;

		int j;
		Semantiques[i][NON] = NULL;
		Semantiques[i][OUI] = NULL;

		//ArbReg[i]=NULL;
		ListeDesRegles[i] = NULL;
		for (j = 0; j< NbCats; j++) {
			TabFiltre[i][j] = 0;
			TabDroiteUn[i][j] = 0;
			TabRegle[i][j] = 0;
		}

		Termes[i] = NULL;
	}

	for (i = 0; i<HACHE; i++)
		Traductions[i] = NULL;

	int laCouche;

	for (i = 0; i< NbCats; i++) {
		TableauDesNoms[i] = NULL;
		for (laCouche = 0; laCouche<MaxCouches; laCouche++)
			TableDesSequences[laCouche][i] = NULL;
	}

	TableDesTraits = NULL;

	FILTRE = NULL;
	TOUSTRAITS = NULL;
	FILTREFONCTION = NULL;
	FILTRENOEUD = NULL;
	FILTREGENERATION = NULL;


	ArbTraits = NULL;
	Abreviation = NULL;
	Defauts = NULL;

	dernierNumeroNoeud = 0;

	LTraits = NULL;

	numero_regle_semantique = 0;

	LEMME = NULL;
	SURFACE = NULL;
	pretraitement = NON;
	iNombreDeValeurs = -1;
	indiceCreeValeur = -2;
	IndiceCreeTrait = 0;

	Nom_Sortie[0] = 0;
	Nom_Trace[0] = 0;


	iNombreDeRegles = 0;

	//Ajout des traits positions

	DebutSeq = NULL;
	FinSeq = NULL;
	DebutPhrase = NULL;
	FinPhrase = NULL;
	AmbiguiteErreur = NULL;

	contexteFoncVide = NULL;

	CatVide = NULL;

	fournirResFonc = NULL;


	BaliseTraits = NULL;
	gBaliseTraits = NULL;
	gBaliseDebut = NULL;
	gBaliseSansFin = NULL;

	TasDeListes = NULL;

	Sortie = NULL;
	//Trace = NULL;
	FinDePhrase.raz();
	Nom_traits.raz();
	Nom_divers.raz();
	Nom_scripts.raz();
	Nom_lexique.raz();
	Nom_regle.raz();
	Nom_fonction.raz();
	NoeudsFictifs.raz();

	codageXML.raz();
	TableAttributs.raz();
	caractereXML.raz();
	NoeudsFictifs.raz();
	TraitsNoeudsFictifs.raz();
	fichiersSemantiquesNoms.raz();
	fichiersSemantiques.raz();
}

/*-------------------------------------------------------------------------*/
GlobalParseur::GlobalParseur(char* nom, char* chemin) : phrase(100), memoiresem(MAXVIRTUELVALEURINITIALE)  {
	tablelatin = 0;
	editdistanceflags = 0;
	editdistancescore = 0;
	editdistancemax = 0;

	ValeurCoucheInitiale = 0;
	interceptexml = NULL;
	crftagger = NULL;
	Xproposecategorie = NULL;
	OS = NULL;
	//kifcode=NULL;
	kifloader = NULL;
	kifdebug = false;
	NOM = NULL;
	AppelExterne = NULL;
	GrmPathname = GRM_COURANT;
	MAXVIRTUEL = MAXVIRTUELVALEURINITIALE;
	appliqueGrammaire = NON;
	appliqueGrammaireDefini = appliqueGrammaire;
	strcpy_s(nom_fichier,1024, nom);
	raz(OUI);
	Preparation(chemin);
};

/*-------------------------------------------------------------------------*/
GlobalParseur::~GlobalParseur() {
	if (detruireDonnees == OUI)
		delete donnees;

	index_parseur_courant = iParseurCourant;
	Nettoyage();
	
	int i;
	for (i = 0; i<enregListeTags.taille; i = i + 1) {
		delete[] enregListeTags[i];
		enregListeTags.vecteur[i] = NULL;
	}

	for (i = 0; i<fparametres.dernier; i++)
		libere(fparametres[i]);

#ifdef CRFTAGGER
	CleanTagger(crftagger);
#endif

	if (gVecteurParseurs.parseurs.vecteur[iParseurCourant] == this)
		gVecteurParseurs.parseurs.vecteur[iParseurCourant] = NULL;
}
/*-------------------------------------------------------------------------*/
void GlobalParseur::InitialisationParametres(char* parametres) {
	//sprintf(parametres,"%d:%d:%d:%d:%d:%d|", reduce,affFonction,
	//        tagger,fonction_synt,
	//        tree,renumerote);

	ParametresAffichage = parametres[0];
	ParametresAffichageDefini = ParametresAffichage;
	AfficheFonction = parametres[1];
	Desambiguisation(parametres[2]);
	CalculFonction = parametres[3];
	CalculFonctionDefini = CalculFonction;

}
/*--------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
int GlobalParseur::InitialisationParseur(typeLangue langue,
	char* stringfile,
	char* chemin,
	int numberValues,
	char latrace,
	TVecteur affFonc,
	char outputXML,
	char tagger,
	char tagging,
	char math,
	char crypte,
	char aff_err,
	char afficherResultats,
	TVecteur ordreDependances,
	TVecteur offset,
	TVecteur red,
	TVecteur tree,
	char fonc_syn,
	VECTA<char*>& fileparam) {

	ERREUR_FICHIER[0] = 0;
	ERREUR_LIGNES = -1;
	ERREUR_POSITION_DANS_FICHIER = -1;

	iParseurCourant = gVecteurParseurs.ajoute(this);

	laLangue = langue;
	mathematique = math;

	if (langue == PERSONNEL)
		strcpy_s(commentaire,1024, stringfile);

	laTrace = latrace;

	Desambiguisation(tagger);
	cryptage = crypte;
	AFFICHE_ERREUR = aff_err;

	for (int i = 0; i<fileparam.dernier; i++)
		fparametres.ajoute(fileparam[i]);

	fileparam.raz();

	iTailleVect = numberValues;
	XML = outputXML;

	ParametresAffichage = red | offset | ordreDependances | tree | AFFICHAGE_ERREUR_EXECUTION;
	ParametresAffichageDefini = ParametresAffichage;

	Initialisation(chemin);

	Tagging(tagging);
	gAfficherResultats = afficherResultats;
	gAfficherResultatsDefini = gAfficherResultats;


	CalculFonction = fonc_syn;
	CalculFonctionDefini = CalculFonction;

	AfficheFonction = affFonc;

	*flot_erreur << endl;
	if (!ERREUR_SURVENUE)
		return iParseurCourant;

	XipFreeGrammar(iParseurCourant);
	return ERREUR_SURVENUE;
}
/*-------------------------------------------------------------------------*/
void GlobalParseur::InitialisationParseurMin(TVecteur affFonc,
	char outputXML,
	char tagger,
	char tagging,
	char math,
	char aff_err,
	char afficherResultats,
	TVecteur ordreDependances,
	TVecteur offset,
	TVecteur red,
	TVecteur tree,
	char fonc_syn) {


	mathematique = math;


	Desambiguisation(tagger);
	AFFICHE_ERREUR = aff_err;

	XML = outputXML;

	ParametresAffichage = red | offset | ordreDependances | tree | AFFICHAGE_ERREUR_EXECUTION;
	ParametresAffichageDefini = ParametresAffichage;

	Tagging(tagging);
	gAfficherResultats = afficherResultats;
	gAfficherResultatsDefini = gAfficherResultats;


	CalculFonction = fonc_syn;
	CalculFonctionDefini = CalculFonction;

	AfficheFonction = affFonc;
}


void GlobalParseur::InitialisationAvecParseur(int ipar) {
	GlobalParseur* parseur = gVecteurParseurs[ipar];
	if (parseur == NULL)
		return;

	parseur->mathematique = mathematique;

	parseur->DESAMBIGUISATION = DESAMBIGUISATION;
	parseur->DESAMBIGUISATIONDefini = DESAMBIGUISATIONDefini;

	parseur->AFFICHE_ERREUR = AFFICHE_ERREUR;

	parseur->XML = XML;

	parseur->ParametresAffichage = ParametresAffichage;
	parseur->ParametresAffichageDefini = ParametresAffichageDefini;
	parseur->TAGGING = TAGGING;
	parseur->gAfficherResultats = gAfficherResultats;
	parseur->gAfficherResultatsDefini = gAfficherResultatsDefini;


	parseur->CalculFonction = CalculFonction;
	parseur->CalculFonctionDefini = CalculFonctionDefini;

	parseur->AfficheFonction = AfficheFonction;
	parseur->AppelExterne = AppelExterne;
	parseur->OS = OS;
}

/*-------------------------------------------------------------------------*/


Exportation arbretrait* XipGetFeatureTree(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NULL;
	return leParseur->larbredestraits;

}

Exportation char XipMessageLanguage(unsigned int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NON;

	if (leParseur->laLangue == ANGLAIS)
		return 1;
	return 2;
}

Exportation baseregle* XipCurrentRule(unsigned int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NON;

	return leParseur->reglecourante;
}

Exportation int XipTheCurrentRule(unsigned int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NULL;
	if (leParseur->reglecourante != NULL)
		return leParseur->reglecourante->identificateur;
	return -1;
}

Exportation char XipSetTaggerMode(unsigned int ipar, char tagger) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NON;

	leParseur->Desambiguisation(tagger);
	return OUI;
}

Exportation char XipGetTaggerMode(unsigned int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	return leParseur->DESAMBIGUISATION;
}

Exportation unsigned char* XipGetCurrentInternalStringBuffer(unsigned int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NON;
	return leParseur->donneesATraiter;
}


Exportation char XipIsGrammarCrypted(unsigned int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return 2;
	if (leParseur->cryptage == NON)
		return NON;
	return OUI;
}

Exportation void XipGrammarPathnames(int ipar,
	string& grammar,
	vector<string>& features,
	long& debutf,
	vector<string>& lexicons,
	long& debutl,
	vector<string>& rules,
	long& debutr,
	vector<string>& parameters,
	vector<string>& scripts,
	vector<string>& divers) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	int i;
	size_t ii;
	grammar = leParseur->Nom_Fichier;

	for (i = 0; i<leParseur->Nom_traits.dernier; i++)
		features.push_back(leParseur->Nom_traits[i]);

	if (leParseur->Type() == PARSEURTRANS) {
		GlobalParseurTrans* gtrans = (GlobalParseurTrans*)leParseur;
		for (ii = 0; ii<gtrans->filenames.size(); ii++)
			divers.push_back(gtrans->filenames[ii]);
	}

	for (i = 0; i<leParseur->Nom_lexique.dernier; i++)
		lexicons.push_back(leParseur->Nom_lexique[i]);


	for (i = 0; i<leParseur->Nom_regle.dernier; i++)
		rules.push_back(leParseur->Nom_regle[i]);

	for (i = 0; i<leParseur->Nom_divers.dernier; i++)
		divers.push_back(leParseur->Nom_divers[i]);

	for (i = 0; i<leParseur->Nom_scripts.dernier; i++)
		scripts.push_back(leParseur->Nom_scripts[i]);

	for (i = 0; i<leParseur->fparametres.dernier; i++)
		parameters.push_back(leParseur->fparametres[i]);

	debutf = leParseur->debut_traits_dans_langue;
	debutl = leParseur->debut_lexique_dans_langue;
	debutr = leParseur->debut_regles_dans_langue;

}


Exportation void XipGrammarPathnamesFull(int ipar,
	string& grmname,
	string& grammar,
	vector<string>& features,
	long& debutf,
	vector<string>& lexicons,
	long& debutl,
	vector<string>& rules,
	long& debutr,
	vector<string>& parameters,
	vector<string>& scripts,
	vector<string>& divers) {

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	grmname = leParseur->nom_fichier_grm;
	XipGrammarPathnames(ipar, grammar, features, debutf, lexicons, debutl, rules, debutr, parameters, scripts, divers);
}


Exportation void XipGrammarInformation(int ipar,
	vector<string>& license,
	string& clef,
	string& version,
	string& language,
	string& grammarlanguage,
	string& thelocale,
	string& charset,
	char& hmm,
	char& reentrance,
	int& max_words,
	int& max_size_word,
	char& encodage_lexique,
	int& featurenumber,
	string& tracepathname,
	string& indentpathname) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	if (leParseur->ChunkerLicense == OUI)
		license.push_back("chunker");

	if (leParseur->DeductionLicense == OUI)
		license.push_back("inference");
	if (leParseur->DeductionLicense == 2)
		license.push_back("extraction");
	if (leParseur->SemantiqueLicense == OUI)
		license.push_back("semantic");
	if (leParseur->ServeurLicense == OUI)
		license.push_back("server");
	if (leParseur->UsageLimitee == 1) {
		string str = "limited:";
		char ch[100];
		sprintf_s(ch,100, "%d", leParseur->max_controle_regle);
		str += ch;
		license.push_back(str);
	}
	if (leParseur->UsageLimitee == 2)
		license.push_back("noparameter");
	license.push_back(leParseur->DateLimite);
	clef = leParseur->CodeClef;
	tracepathname = leParseur->Nom_Trace;
	indentpathname = leParseur->Nom_Sortie;
	featurenumber = leParseur->iTailleVect;
	reentrance = leParseur->reentrance;
	thelocale = leParseur->LaLocale;
	max_words = leParseur->nbmotsmaximum;
	max_size_word = leParseur->taille_mot;
	hmm = leParseur->pretraitement;
	charset = leParseur->encodage;
	version = leParseur->version_grammaire;
	if (leParseur->langue_de_la_grammaire != NULL)
		grammarlanguage = leParseur->langue_de_la_grammaire;
	if (leParseur->laLangue == ANGLAIS)
		language = "english";
	else
		language = "french";

}

/*-------------------------------------------------------------------------*/
Exportation GlobalParseur* XipSelectGrammar(unsigned int iParseur) {
	index_parseur_courant = iParseur;
	return gVecteurParseurs[iParseur];
}

Exportation GlobalParseur* XipGetParser(unsigned int iParseur) {
	GlobalParseur* p = gVecteurParseurs[iParseur];
	if (p != NULL) {
		index_parseur_courant = iParseur;
		parseur_courant = p;
	}
	return p;
}

Exportation int XipGetNbParsers() {
	return gVecteurParseurs.dernier;
}

extern GlobalParseur* BaseParseur;
Exportation void XipBaseParseur(XipFunction f, bool fonction, void* data) {
	if (fonction) {
		if (BaseParseur == NULL)
			BaseParseur = new GlobalParseur("", "");
		BaseParseur->AppelExterne = f;
		BaseParseur->donneesExternes = data;
		BaseParseur->CalculFonction = fonction;
		BaseParseur->CalculFonctionDefini = fonction;
		BaseParseur->gAfficherResultats = OUI;
		BaseParseur->gAfficherResultatsDefini = BaseParseur->ParametresAffichage;

		BaseParseur->ParametresAffichage = AFFICHE_FONCTION | AFFICHAGE_DEP_NOM | AFFICHAGE_ERREUR_EXECUTION | AFFICHAGE_NUMERO_DEPENDANCE | AFFICHAGE_SURFACE | AFFICHAGE_ARBRE_CHUNK;
		BaseParseur->ParametresAffichageDefini = BaseParseur->ParametresAffichage;

		BaseParseur->Tagging(OUI);
	}
	else {
		delete BaseParseur;
		BaseParseur = NULL;
	}
}

Exportation string XipCurrentSentence(int i) {
	GlobalParseur* p = gVecteurParseurs[i];
	if (p == NULL)
		return "";
	return p->PhraseCourante();
}

Exportation void XipInitKifParseur(int i) {
	GlobalParseur* p = gVecteurParseurs[i];
	if (p == NULL)
		return;

	p->CalculFonction = 1;
	p->CalculFonctionDefini = 1;
	p->gAfficherResultats = OUI;
	p->gAfficherResultatsDefini = p->ParametresAffichage;

	p->ParametresAffichage = AFFICHE_FONCTION | AFFICHAGE_DEP_NOM | AFFICHAGE_ERREUR_EXECUTION | AFFICHAGE_NUMERO_DEPENDANCE | AFFICHAGE_SURFACE | AFFICHAGE_ARBRE_CHUNK;
	p->ParametresAffichageDefini = p->ParametresAffichage;

	p->Tagging(OUI);
}

/*-------------------------------------------------------------------------*/
Exportation void XipFreeGrammar(unsigned int i) {
	if (gVecteurParseurs.parseurs.vecteur == NULL || i < 0 || i >= gVecteurParseurs.parseurs.dernier || gVecteurParseurs.parseurs.vecteur[i] == NULL)
		return;

	delete gVecteurParseurs.parseurs.vecteur[i];
	gVecteurParseurs.parseurs.vecteur[i] = NULL;

	for (int j = gVecteurParseurs.parseurs.dernier; j >= 0; j--) {
		if (gVecteurParseurs.parseurs.vecteur[j] != NULL) {
			gVecteurParseurs.parseurs.dernier = j + 1;
			gVecteurParseurs.dernier = j + 1;
			return;
		}
	}
	ParseurRAZ();
}

#ifdef NTMCONNECTION
void reset_initialisation_fst();
#endif

Exportation void XipFreeAllGrammars() {
	KifThreadLock _lock(protegexip);
	for (int i = -1; i<gVecteurParseurs.dernier; i++)
		gVecteurParseurs.detruit(i);
	ParseurRAZ();
#ifdef NTMCONNECTION
	reset_initialisation_fst();
#endif
}

Exportation int XipExistGrammar(int iParseur) {
	if (gVecteurParseurs[iParseur] == NULL)
		return NON;
	return OUI;
}

Exportation char* XipIndentFilePathname(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NULL;

	return leParseur->Nom_Sortie;
}


Exportation void XipSetDependencyExtraction(int ipar, char ext) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	leParseur->CalculFonction = ext;
	leParseur->CalculFonctionDefini = leParseur->CalculFonction;
}



int ChargementGrammaire(Fichiergrm& fgrm, int xml, char crypte, int numberValues, KifCode* kifcode = NULL) {
	KifThreadLock _lock(protegexip);

	char path[1000];
	char extract = 0;

	char p_chemin = 0;
	char p_fichier = 0;
	char pretraitement = NON;
	char tagger = NON;
	char tagging = OUI;
	char aff_err = NON;

	typeLangue langue = ANGLAIS;
	int maxSentence = -1;
	char fonction_synt = OUI;
	char theTrace = 0;

	TVecteur aff_fonc = AFFICHE_FONCTION;
	TVecteur offset = 0;
	TVecteur reduce = AFFICHAGE_SURFACE | AFFICHAGE_REDUIT;

	char inputXML = NON;
	char outputXML = NON;

	char stringfile[1000];
	char texte[1000];
	char renumerote = OUI;
	TVecteur tree = OUI; // TODO not used

	Recuperenom(fgrm.grammar, path);

	stringfile[0] = 0;
	texte[0] = 0;

	//the language argument is processed
	int arret = 0;

	GlobalParseur* leParseur = ChoixParseur(fgrm, xml, path);
	for (size_t i = 0; i<fgrm.g_definition_var.size(); i++)
		leParseur->definition_var.push_back(fgrm.g_definition_var[i]);

	leParseur->kifloader = kifcode;
	int ipar = leParseur->InitialisationParseur(langue,
		stringfile,
		path,
		numberValues,
		theTrace,
		aff_fonc,
		outputXML,
		tagger,
		tagging,
		NON,
		crypte,
		aff_err,
		OUI,
		AFFICHAGE_DEP_NOM,
		offset,
		reduce,
		NON,
		fonction_synt,
		fgrm.parameters);

	return ipar;
}

Exportation int XipLoadGrammarBase(char* grmfile,
	char* generefile,
	int xml,
	char* ntmfile,
	char* hmmfile,
	char* tokenfile,
	char* lookupfile,
	char* lookup_flags,
	lookup_strategy strategy) {

	char crypte = NON;
	Fichiergrm fgrm;
	strcpy_s(fgrm.grmname,1024, grmfile);
	strcpy(fgrm.ntm, ntmfile);
	strcpy(fgrm.hmm, hmmfile);
	strcpy_s(fgrm.genere,1024, generefile);
	strcpy_s(fgrm.tokenize,1024, tokenfile);
	strcpy(fgrm.lookup, lookupfile);
	strcpy_s(fgrm.lookup_flags,1024, lookup_flags);
	fgrm.strategy = strategy;

	return ChargementGrammaire(fgrm, xml, crypte, -1);

}

Exportation int XipGrammarLoadingBase(char* grmfile,
	char* generefile,
	int xml,
	char* ntmfile,
	char* hmmfile,
	char* tokenfile,
	char* lookupfile,
	char* lookup_flags,
	lookup_strategy strategy) {
	char crypte = OUI;
	Fichiergrm fgrm;
	strcpy_s(fgrm.grmname,1024, grmfile);
	strcpy(fgrm.ntm, ntmfile);
	strcpy(fgrm.hmm, hmmfile);
	strcpy_s(fgrm.genere,1024, generefile);
	strcpy_s(fgrm.tokenize,1024, tokenfile);
	strcpy(fgrm.lookup, lookupfile);
	strcpy_s(fgrm.lookup_flags,1024, lookup_flags);
	fgrm.strategy = strategy;

	return ChargementGrammaire(fgrm, xml, crypte, -1);
}


int GlobalParseur::LicenseTest() {
	return licenseTest;
}


Exportation int XipLicense(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	return leParseur->LicenseTest();
}


Exportation int XipParseXipResult(XipResult* xipbase, XipResult* xipres, int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;
	leParseur->xipresult = (XipResult*)xipres;
	leParseur->AnalyseXipResult(xipbase);
	return 0;
}

Exportation int XipParseXipLeaves(XipUnit* xipnode, XipResult* xipres, int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;
	leParseur->xipresult = (XipResult*)xipres;
	leParseur->AnalyseXipLeaves(xipnode);
	return 0;
}

Exportation int XipParseXipVECTA(VECTA<XipNode*>& xipnode, XipResult* xipres, int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;
	leParseur->xipresult = (XipResult*)xipres;
	leParseur->AnalyseXipVECTA(xipnode);
	return 0;
}

Exportation int XipParseXipNode(XipNode* xipnode, XipResult* xipres, int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;
	leParseur->xipresult = (XipResult*)xipres;
	leParseur->AnalyseXipNode(xipnode);
	return 0;
}

//************************************************************************************************
//************************************************************************************************
//************************************************************************************************
#ifdef XIPSTLAPI
Exportation int XipGenerateFromFile(string nomfichier, int ipar, ostringstream* os) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;

	//Petit bug dans le cas d'une DLL STL windows... Pour une raison curieuse, 
	//la DLL perd ses "facettes"... Il faut donc lui adjoindre une comprehension
	//de ces facettes sous la forme d'un objet "locale"
#if (defined(WIN32) && defined(_WINDLL))
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif


	ostream* os_ancien = leParseur->OS;

	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;

	int xml_ancien = leParseur->XML;
	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = NULL;
	leParseur->appliqueGrammaire = 1;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;
	leParseur->XML = 0;

	ifstream fs(nomfichier.c_str(), modeOuverture);
	if (fs.fail())
		return 0;

	leParseur->FileStreamAssociation(&fs);
	int res = leParseur->ChargerDependances(nomfichier);
	leParseur->CleanStream();

	leParseur->XML = xml_ancien;
	leParseur->xipresult = ancien_xipresult;
	leParseur->OS = os_ancien;
	return res;
}

Exportation string XipLastGeneratedString(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return "";
	return leParseur->laststrgen;
}

Exportation int XipGenerateFromStringXipResult(string texte, XipResult* xip, int ipar, ostringstream* os) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;

	//Petit bug dans le cas d'une DLL STL windows... Pour une raison curieuse, 
	//la DLL perd ses "facettes"... Il faut donc lui adjoindre une comprehension
	//de ces facettes sous la forme d'un objet "locale"
#if (defined(WIN32) && defined(_WINDLL))
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif


	ostream* os_ancien = leParseur->OS;

	leParseur->creationtoken->X.cacher = NON;
	leParseur->surfacetoken->X.cacher = NON;

	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;

	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = xip;
	leParseur->appliqueGrammaire = 1;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;

	leParseur->StringStreamAssociation((uchar*)texte.c_str());
	int res = leParseur->ChargerDependances("STRING");
	leParseur->CleanStream();
	leParseur->xipresult = ancien_xipresult;
	leParseur->OS = os_ancien;

	leParseur->creationtoken->X.cacher = OUI;
	leParseur->surfacetoken->X.cacher = OUI;

	return res;
}

Exportation int XipGenerateFromString(string texte, int ipar, ostringstream* os) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;

	//Petit bug dans le cas d'une DLL STL windows... Pour une raison curieuse, 
	//la DLL perd ses "facettes"... Il faut donc lui adjoindre une comprehension
	//de ces facettes sous la forme d'un objet "locale"
#if (defined(WIN32) && defined(_WINDLL))
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif


	ostream* os_ancien = leParseur->OS;

	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;

	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = NULL;
	leParseur->appliqueGrammaire = 1;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;

	uchar* prev = leParseur->donneesATraiter;
	leParseur->donneesATraiter = USTR(texte);
	leParseur->StringStreamAssociation((uchar*)texte.c_str());
	int res = leParseur->ChargerDependances("STRING");
	leParseur->CleanStream();
	leParseur->donneesATraiter = prev;
	leParseur->xipresult = ancien_xipresult;
	leParseur->OS = os_ancien;
	return res;
}

Exportation int XipParseFileToFile(string nomfichier,
	int ipar,
	ofstream* os,
	char xml,
	char applique_grammaire) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;

	//Petit bug dans le cas d'une DLL STL windows... Pour une raison curieuse, 
	//la DLL perd ses "facettes"... Il faut donc lui adjoindre une comprehension
	//de ces facettes sous la forme d'un objet "locale"
#if (defined(WIN32) && defined(_WINDLL))
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif

	ostream* os_ancien = leParseur->OS;
	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;

	int xml_ancien = leParseur->XML;
	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = NULL;
	leParseur->appliqueGrammaire = applique_grammaire;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;
	leParseur->XML = xml;



#ifdef XIPLIBXML
	if (leParseur->Type() == PARSEURXML) {
		GlobalParseurXML* xmlparse = (GlobalParseurXML*)leParseur;
		xmlparse->AnalyseXipXMLFilename((char*)nomfichier.c_str());
		leParseur->OS = os_ancien;
		leParseur->XML = xml_ancien;
		leParseur->xipresult = ancien_xipresult;
		return 0;
	}
#endif

	FILE* document = fopen(nomfichier.c_str(), "rb");
	leParseur->renseigne((char*)nomfichier.c_str());
	document = leParseur->FileAssociation(document);
	leParseur->Analyse(OUI, OUI);
	leParseur->XML = xml_ancien;
	leParseur->xipresult = ancien_xipresult;
	leParseur->OS = os_ancien;
	if (document != NULL)
		fclose(document);
	return 0;
}

Exportation int XipParseFileToOS(string nomfichier,
	int ipar,
	ostream* os,
	char xml,
	char applique_grammaire) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;

	//Petit bug dans le cas d'une DLL STL windows... Pour une raison curieuse, 
	//la DLL perd ses "facettes"... Il faut donc lui adjoindre une comprehension
	//de ces facettes sous la forme d'un objet "locale"
#if (defined(WIN32) && defined(_WINDLL))
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif

	ostream* os_ancien = leParseur->OS;
	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;

	int xml_ancien = leParseur->XML;
	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = NULL;
	leParseur->appliqueGrammaire = applique_grammaire;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;
	leParseur->XML = xml;



#ifdef XIPLIBXML
	if (leParseur->Type() == PARSEURXML) {
		GlobalParseurXML* xmlparse = (GlobalParseurXML*)leParseur;
		xmlparse->AnalyseXipXMLFilename((char*)nomfichier.c_str());
		leParseur->OS = os_ancien;
		leParseur->XML = xml_ancien;
		leParseur->xipresult = ancien_xipresult;
		return 0;
	}
#endif

	FILE* document = fopen(nomfichier.c_str(), "rb");
	leParseur->renseigne((char*)nomfichier.c_str());
	document = leParseur->FileAssociation(document);
	leParseur->Analyse(OUI, OUI);
	leParseur->XML = xml_ancien;
	leParseur->xipresult = ancien_xipresult;
	leParseur->OS = os_ancien;
	if (document != NULL)
		fclose(document);
	return 0;
}

Exportation void XipSetDisplayMultiWordString(char v) {
	AFFICHE_PHRASE_MULTIMOTS = v;
}
//-----------------------------------------------------------------------------------------------------------
Exportation int XipParseFileOS(string nomfichier, int ipar, ostringstream* os, char xml, char applique_grammaire) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;

	//Petit bug dans le cas d'une DLL STL windows... Pour une raison curieuse, 
	//la DLL perd ses "facettes"... Il faut donc lui adjoindre une comprehension
	//de ces facettes sous la forme d'un objet "locale"
#if (defined(WIN32) && defined(_WINDLL))
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif


	ostream* os_ancien = leParseur->OS;

	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;

	int xml_ancien = leParseur->XML;
	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = NULL;
	leParseur->appliqueGrammaire = applique_grammaire;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;
	leParseur->XML = xml;



#ifdef XIPLIBXML
	if (leParseur->Type() == PARSEURXML) {
		GlobalParseurXML* xmlparse = (GlobalParseurXML*)leParseur;
		xmlparse->AnalyseXipXMLFilename((char*)nomfichier.c_str());
		leParseur->OS = os_ancien;
		leParseur->XML = xml_ancien;
		leParseur->xipresult = ancien_xipresult;
		return 0;
	}
#endif

	FILE* document = fopen(nomfichier.c_str(), "rb");
	leParseur->renseigne((char*)nomfichier.c_str());
	document = leParseur->FileAssociation(document);
	leParseur->Analyse(OUI, OUI);
	leParseur->XML = xml_ancien;
	leParseur->xipresult = ancien_xipresult;
	leParseur->OS = os_ancien;
	if (document != NULL)
		fclose(document);
	return 0;
}

Exportation int XipParseStringOS(string texte, int ipar, ostringstream* os, char xml, char applique_grammaire) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

#if (defined(WIN32) && defined(_WINDLL)) 
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif

	index_parseur_courant = ipar; parseur_courant = leParseur;
	ostream* os_ancien = leParseur->OS;

	leParseur->DeductionInitiale = OUI;
	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;

	int xml_ancien = leParseur->XML;
	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = NULL;
	leParseur->appliqueGrammaire = applique_grammaire;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;
	leParseur->XML = xml;

#ifdef XIPLIBXML
	if (leParseur->Type() == PARSEURXML) {
		GlobalParseurXML* xmlparse = (GlobalParseurXML*)leParseur;
		xmlparse->AnalyseXipXMLString((uchar*)texte.c_str());
		leParseur->XML = xml_ancien;
		leParseur->xipresult = ancien_xipresult;
		leParseur->OS = os_ancien;
		return 0;
	}
#endif

	leParseur->StringAssociation((uchar*)texte.c_str());
	leParseur->Analyse(OUI, OUI);
	leParseur->XML = xml_ancien;
	leParseur->xipresult = ancien_xipresult;
	leParseur->OS = os_ancien;
	return 0;
}

Exportation int XipParseStringStructure(string texte, int ipar, ostringstream* os, char xml, char applique_grammaire) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

#if (defined(WIN32) && defined(_WINDLL)) 
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif

	index_parseur_courant = ipar; parseur_courant = leParseur;
	ostream* os_ancien = leParseur->OS;

	leParseur->DeductionInitiale = OUI;
	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;

	int xml_ancien = leParseur->XML;
	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = NULL;
	leParseur->appliqueGrammaire = applique_grammaire;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;
	leParseur->XML = xml;

#ifdef XIPLIBXML
	if (leParseur->Type() == PARSEURXML) {
		GlobalParseurXML* xmlparse = (GlobalParseurXML*)leParseur;
		xmlparse->AnalyseXipXMLString((uchar*)texte.c_str());
		leParseur->XML = xml_ancien;
		leParseur->xipresult = ancien_xipresult;
		leParseur->OS = os_ancien;
		return 0;
	}
#endif

	//A mettre implantation ici
	leParseur->Analyse(OUI, OUI);
	leParseur->XML = xml_ancien;
	leParseur->xipresult = ancien_xipresult;
	leParseur->OS = os_ancien;
	return 0;
}

//-----------------------------------------------------------------------------------------------------------
Exportation int XipParseStringAndOS(string texte, int ipar,
	XipResult* xip,
	ostringstream* os,
	char xml,
	char applique_grammaire) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

#if (defined(WIN32) && defined(_WINDLL))
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif

	index_parseur_courant = ipar; parseur_courant = leParseur;
	ostream* os_ancien = leParseur->OS;

	leParseur->DeductionInitiale = OUI;
	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;

	int xml_ancien = leParseur->XML;
	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = xip;
	leParseur->appliqueGrammaire = applique_grammaire;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;
	leParseur->XML = xml;

#ifdef XIPLIBXML
	if (leParseur->Type() == PARSEURXML) {
		GlobalParseurXML* xmlparse = (GlobalParseurXML*)leParseur;
		xmlparse->AnalyseXipXMLString((uchar*)texte.c_str());
		leParseur->XML = xml_ancien;
		leParseur->xipresult = ancien_xipresult;
		leParseur->OS = os_ancien;
		return 0;
	}
#endif

	leParseur->StringAssociation((uchar*)texte.c_str());
	leParseur->Analyse(OUI, OUI);
	leParseur->XML = xml_ancien;
	leParseur->xipresult = ancien_xipresult;
	leParseur->OS = os_ancien;
	return 0;
}

#endif
//************************************************************************************************


void AfficheStatitistiques(GlobalParseur* leParseur) {
	int total = 0;
	cout << "Quelques statistiques:" << endl;
	total = leParseur->TasDeListes->taille;
	cout << "TasDeListe:" << leParseur->TasDeListes->taille << endl;
	total += leParseur->phrase.taille;
	cout << "phrase:" << leParseur->phrase.taille << endl;
	int i, lataille = 0;
	for (i = 0; i <= MaxCouches; i++)
		lataille += leParseur->glisteDesPhrases[i].taille;
	total += lataille;
	cout << "glisteDesPhrases:" << lataille << endl;
	total += leParseur->INTERFONCTIONS.taille;
	cout << "INTERFONCTIONS:" << leParseur->INTERFONCTIONS.taille << endl;
	total += leParseur->PREMIERFONCTIONS.taille;
	cout << "PREMIERFONCTIONS:" << leParseur->PREMIERFONCTIONS.taille << endl;
	total += leParseur->FONCTIONSINTERMEDIAIRES.taille;
	cout << "FONCTIONSINTERMEDIAIRES:" << leParseur->FONCTIONSINTERMEDIAIRES.taille << endl;
	lataille = 0;
	for (i = 0; i<NbCats; i++)
		lataille += leParseur->IndexParCat[i].taille;
	total += lataille;
	cout << "IndexParCat:" << lataille << endl;
	total += leParseur->NoeudsLexicaux.taille;
	cout << "NoeudsLexicaux:" << leParseur->NoeudsLexicaux.taille << endl;
	total += leParseur->NoeudsDependance.taille;
	cout << "NoeudsDependance:" << leParseur->NoeudsDependance.taille << endl;
	total += leParseur->memoiresem.memoire.taille;
	cout << "memoiresem.memoire:" << leParseur->memoiresem.memoire.taille << endl;
	cout << "TOTAL=" << total << endl << endl;
}

Exportation int XipParseFile(char* nomfichier, int ipar, XipResult* xip, char applique_grammaire) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	char applique = applique_grammaire;
	index_parseur_courant = ipar; parseur_courant = leParseur;
	if (applique)
		applique_grammaire = 1;

	leParseur->DeductionInitiale = OUI;
	leParseur->appliqueGrammaire = applique_grammaire;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = (XipResult*)xip;

#ifdef XIPLIBXML
	if (leParseur->Type() == PARSEURXML) {
		GlobalParseurXML* xmlparse = (GlobalParseurXML*)leParseur;
		xmlparse->AnalyseXipXMLFilename(nomfichier);
		if (applique == 2)
			AfficheStatitistiques(leParseur);
		leParseur->xipresult = ancien_xipresult;
		return 0;
	}
#endif

	FILE* document = fopen(nomfichier, "rb");
	leParseur->renseigne(nomfichier);
	document = leParseur->FileAssociation(document);
	leParseur->Analyse(OUI, OUI);
	if (applique == 2)
		AfficheStatitistiques(leParseur);
	if (document != NULL)
		fclose(document);
	leParseur->xipresult = ancien_xipresult;
	return 0;
}

Exportation int XipParseString(unsigned char* texte, int ipar, XipResult* xip, char applique_grammaire) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;
	leParseur->appliqueGrammaire = applique_grammaire;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = xip;

#ifdef XIPLIBXML
	if (leParseur->Type() == PARSEURXML) {
		GlobalParseurXML* xmlparse = (GlobalParseurXML*)leParseur;
		xmlparse->AnalyseXipXMLString(texte);
		leParseur->xipresult = ancien_xipresult;
		return 0;
	}
#endif

	leParseur->StringAssociation(texte);
	leParseur->Analyse(OUI, OUI);
	leParseur->xipresult = ancien_xipresult;
	return 0;
}

Exportation int XipParameterFile(int ipar, char* filename) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	leParseur->fparametres.ajoute(DupliqueChaine(filename));
	leParseur->LireParametres(filename);
	leParseur->MAJNumeroRegles();
	return 0;
}

#ifdef XIPLIBXML
Exportation int XipLoadXMLDataBase(int ipar, char* filename, char* alias) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	leParseur->ChargerFichierXML(filename, alias);
	return 0;
}
//************************************************************************************************
#ifdef XIPSTLAPI
Exportation int XipParseFileXMLOS(string nomfichier,
	int ipar,
	ostringstream* os,
	int profondeur,
	char xml,
	char applique_grammaire) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;
	//Petit bug dans le cas d'une DLL STL windows... Pour une raison curieuse, 
	//la DLL perd ses "facettes"... Il faut donc lui adjoindre une comprehension
	//de ces facettes sous la forme d'un objet "locale"
#if (defined(WIN32) && defined(_WINDLL))
	if (os != NULL){
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif

	if (os != NULL) {

		leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
		leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;
	}

	ostream* os_ancien = leParseur->OS;


	int xml_ancien = leParseur->XML;
	leParseur->XML = xml;

	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = NULL;

	leParseur->appliqueGrammaire = applique_grammaire;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;

	leParseur->AnalyseFichierXML((char*)nomfichier.c_str(), profondeur, NON, OUI);
	leParseur->xipresult = ancien_xipresult;
	leParseur->XML = xml_ancien;
	leParseur->OS = os_ancien;
	return 0;
}

Exportation int XipParseFileXMLToFile(string nomfichier,
	int ipar,
	ofstream* os,
	int profondeur,
	char xml,
	char applique_grammaire) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;
	//Petit bug dans le cas d'une DLL STL windows... Pour une raison curieuse, 
	//la DLL perd ses "facettes"... Il faut donc lui adjoindre une comprehension
	//de ces facettes sous la forme d'un objet "locale"
#if (defined(WIN32) && defined(_WINDLL))
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif

	if (os != NULL) {

		leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
		leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;
	}

	ostream* os_ancien = leParseur->OS;


	int xml_ancien = leParseur->XML;
	leParseur->XML = xml;

	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = NULL;

	leParseur->appliqueGrammaire = applique_grammaire;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;

	leParseur->AnalyseFichierXML((char*)nomfichier.c_str(), profondeur, NON, OUI);
	leParseur->xipresult = ancien_xipresult;
	leParseur->XML = xml_ancien;
	leParseur->OS = os_ancien;
	return 0;
}

//************************************************************************************************
Exportation int XipParseStringXMLOS(string texte,
	int ipar,
	ostringstream* os,
	int profondeur,
	char xml,
	char applique_grammaire) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;
	//Petit bug dans le cas d'une DLL STL windows... Pour une raison curieuse, 
	//la DLL perd ses "facettes"... Il faut donc lui adjoindre une comprehension
	//de ces facettes sous la forme d'un objet "locale"
#if (defined(WIN32) && defined(_WINDLL))
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif

#ifdef XIPSTLAPI
	ostream* os_ancien = leParseur->OS;
#else
	ostrstream* os_ancien = leParseur->OS;
#endif    

	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;

	int xml_ancien = leParseur->XML;
	leParseur->XML = xml;
	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = NULL;

	leParseur->appliqueGrammaire = applique_grammaire;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;

	leParseur->AnalyseChaineXML((char*)texte.c_str(), profondeur, NON, OUI);
	leParseur->xipresult = ancien_xipresult;
	leParseur->XML = xml_ancien;
	leParseur->OS = os_ancien;
	return 0;
}
//************************************************************************************************
Exportation int XipParseStringTextToXMLOS(string texte,
	int ipar,
	ostringstream* os,
	char* rootmarkup,
	char* encoding,
	char xml_output,
	char applique_grammaire) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	//Petit bug dans le cas d'une DLL STL windows... Pour une raison curieuse, 
	//la DLL perd ses "facettes"... Il faut donc lui adjoindre une comprehension
	//de ces facettes sous la forme d'un objet "locale"
#if (defined(WIN32) && defined(_WINDLL))
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif

#ifdef XIPSTLAPI
	ostream* os_ancien = leParseur->OS;
#else
	ostrstream* os_ancien = leParseur->OS;
#endif    

	leParseur->DeductionInitiale = OUI;
	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;

	int xml_ancien = leParseur->XML;
	leParseur->XML = xml_output;
	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = NULL;

	leParseur->appliqueGrammaire = applique_grammaire;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;

	leParseur->AnalyseChaineTexteVersXML(texte, rootmarkup, encoding, OUI, OUI);
	leParseur->xipresult = ancien_xipresult;
	leParseur->XML = xml_ancien;
	leParseur->OS = os_ancien;
	return 0;
}

//************************************************************************************************
Exportation int XipParseFileTextToXMLOS(string filename,
	int ipar,
	ostringstream* os,
	char* rootmarkup,
	char* encoding,
	char xml_output,
	char applique_grammaire) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	//Petit bug dans le cas d'une DLL STL windows... Pour une raison curieuse, 
	//la DLL perd ses "facettes"... Il faut donc lui adjoindre une comprehension
	//de ces facettes sous la forme d'un objet "locale"
#if (defined(WIN32) && defined(_WINDLL))
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif

#ifdef XIPSTLAPI
	ostream* os_ancien = leParseur->OS;
#else
	ostrstream* os_ancien = leParseur->OS;
#endif    

	leParseur->DeductionInitiale = OUI;
	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;

	int xml_ancien = leParseur->XML;
	leParseur->XML = xml_output;
	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = NULL;

	leParseur->appliqueGrammaire = applique_grammaire;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = os;

	leParseur->GenereFichierXML((char*)filename.c_str(), rootmarkup, encoding, OUI, OUI);
	leParseur->xipresult = ancien_xipresult;
	leParseur->XML = xml_ancien;
	leParseur->OS = os_ancien;
	return 0;
}
#endif
//************************************************************************************************
Exportation char XipCleanXMLAlias(int ipar, string alias) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	//On detruit la structure en memoire correspondant a arboxml
	for (int i = 0; i<leParseur->BaseDonneesXML.dernier; i++) {
		if (alias == (char*)leParseur->BaseDonneesXML[i]->pointentree) {
			delete leParseur->BaseDonneesXML.retireElement(i);
			return 0;
		}
	}

	return 1;
}

Exportation int XipParseXMLString(int ipar, char* texte, int profondeur, XipResult* xip) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->xipresult = xip;

	//Tout d'abord on charge la DTD
	//leParseur->AnalyseXDTD(xdtd);

	//Puis on lance l'analyse sur le texte
	leParseur->DeductionInitiale = OUI;
	leParseur->AnalyseChaineXML(texte, profondeur, NON, OUI);
	return 0;
}

Exportation int XipParseXMLFile(int ipar, char* fichier, int profondeur, XipResult* xip) {
	InitialisationMutex();
	KifThreadLock _lock(protegexip);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	index_parseur_courant = ipar; parseur_courant = leParseur;
	leParseur->DeductionInitiale = OUI;

	ostream* os_ancien = leParseur->OS;


	int xml_ancien = leParseur->XML;
	leParseur->XML = NON;

	XipResult* ancien_xipresult = leParseur->xipresult;
	leParseur->xipresult = xip;

	leParseur->appliqueGrammaire = OUI;
	leParseur->appliqueGrammaireDefini = leParseur->appliqueGrammaire;
	leParseur->OS = NULL;
	leParseur->AnalyseFichierXML(fichier, profondeur, NON, OUI);

	leParseur->xipresult = ancien_xipresult;
	leParseur->XML = xml_ancien;
	leParseur->OS = os_ancien;

	return 0;
}

Exportation char XipGetCurrentXMLNodeData(int ipar, string& tagname, vector<string>& properties) {

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL || leParseur->basexmlcourante == NULL || leParseur->basexmlcourante->XmlNodeCourant == NULL)
		return NON;

	xmlNodePtr node = leParseur->basexmlcourante->XmlNodeCourant;

	tagname = (char*)node->name;
	xmlAttrPtr propriete;
	if (node->properties != NULL) {
		propriete = node->properties;
		while (propriete != NULL) {
			char* index = (char*)propriete->name;
			char* valeur = (char*)propriete->children->content;
			properties.push_back(index);
			properties.push_back(valeur);
			propriete = propriete->next;
		}
	}
	return OUI;
}

Exportation xmlNodePtr XipXMLCurrentNode(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NULL;

	return leParseur->basexmlcourante->XmlNodeCourant;
}

Exportation char XipSetResultDirectory(int ipar, char* directory) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	leParseur->repertoireresultat = directory;
	return 0;
}

#endif 


Exportation int XipReloadGrammar(int ipar, char reloadparameter, char* grammar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	//KifCode* kcode=leParseur->kifcode;
	//leParseur->kifcode=NULL;
#ifdef NTMCONNECTION
	NePasNettoyer(true);
#endif
	index_parseur_courant = ipar;
	leParseur->Nettoyage();
#ifdef NTMCONNECTION
	NePasNettoyer(false);
#endif
	char chemingrm = NON;
	//	leParseur->kifcode=kcode;

	if (grammar != NULL && grammar[0] != 0)
		chemingrm = OUI;

	if (reloadparameter == 0) {
		for (int i = 0; i<leParseur->fparametres.dernier; i++)
			libere(leParseur->fparametres[i]);
		leParseur->fparametres.raz();
	}

	vector<string> blockingrule;

	if (chemingrm == OUI) {
		Fichiergrm fgrm;
		strcpy_s(fgrm.grmname,1024, grammar);
		int numberValues = -1;
		leParseur->definition_var.clear();
		int x;
		for (x = 0; x<leParseur->fparametres.size(); x++)
			fgrm.parameters.push_back(leParseur->fparametres[x]);
		for (x = 0; x<leParseur->definition_var.size(); x++)
			fgrm.g_definition_var.push_back(leParseur->definition_var[x]);
		LoadGrmFiles(fgrm, numberValues, OUI);

		strcpy_s(leParseur->Nom_Fichier,1024, fgrm.grammar);
		Recuperenom(fgrm.grammar, leParseur->repertoire);
		strcpy_s(leParseur->nom_fichier,1024, fgrm.grammar);
	}
	else
		strcpy_s(leParseur->nom_fichier,1024, leParseur->Nom_Fichier + strlen(leParseur->repertoire));

	if (leParseur->Type() == PARSEURTRANS) {
		GlobalParseurTrans* gtrans = (GlobalParseurTrans*)leParseur;
		gtrans->reloading();
	}

	bool maitre = false;
	//if (kcode!=NULL) {
	//	maitre=kcode->master;
	//	kcode->master=true;
	//}
	leParseur->Initialisation(leParseur->repertoire);
	//if (kcode!=NULL)
	//	kcode->master=maitre;

	for (size_t ide = 0; ide<blockingrule.size(); ide++)
		leParseur->desactiveEspaceRegle((char*)blockingrule[ide].c_str());

	return ipar;
}


Exportation int XipSetPythonDebug(int ipar, char reloadparameter, char* grammar, char python, int module, int ligne) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;
	leParseur->python_debug = NON;
	if (python)
		leParseur->python_debug = OUI;

	if (leParseur->python_debug == OUI) {
		leParseur->num_python_ligne = ligne;
		leParseur->num_fichier_python = module;
		if (python == 2)
			return 0;
	}
	return XipReloadGrammar(ipar, reloadparameter, grammar);
}

Exportation int XipSetIndentFilePathname(int ipar, string pathname) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	strcpy_s(leParseur->Nom_Sortie,1024, pathname.c_str());
	return 0;
}


Exportation int XipSetWorkingFilesPath(int ipar, string path) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	if (path[path.size() - 1] == SEP)
		path = path.substr(0, path.size() - 1);

	//On recupere le nom du fichier de sortie
	char nom[1000];
	char* last_slash = strrchr(leParseur->Nom_Sortie, SEP);

	if (last_slash == NULL)
		sprintf_s(nom,1000, "%c%s", SEP, leParseur->Nom_Sortie);
	else
		strcpy_s(nom,1000, last_slash);

	strcpy_s(leParseur->Nom_Sortie,1024, path.c_str());
	strcat_s(leParseur->Nom_Sortie,1024, nom);
	last_slash = strrchr(leParseur->Nom_Trace, SEP);

	if (last_slash == NULL)
		sprintf_s(nom,1000, "%c%s", SEP, leParseur->Nom_Trace);
	else
		strcpy_s(nom,1000, last_slash);

	strcpy_s(leParseur->Nom_Trace,1024, path.c_str());
	strcat_s(leParseur->Nom_Trace,1024, nom);

	if (Trace != NULL) {
		delete Trace;
		Trace = new ofstream(leParseur->Nom_Trace);
	}

	if (leParseur->Sortie != NULL) {
		if (leParseur->Sortie != NULL)
			delete leParseur->Sortie;
		leParseur->Sortie = new ofstream(leParseur->Nom_Sortie);
	}

	return 0;
}


Exportation int XipIndentFile(int ipar, char ouverture) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;
	if (ouverture == OUI) {
		if (leParseur->Sortie != NULL)
			delete leParseur->Sortie;
		leParseur->ARBREIND = OUI;
		leParseur->Sortie = new ofstream(leParseur->Nom_Sortie);
	}
	else {
		leParseur->ARBREIND = NON;
		if (leParseur->Sortie != NULL)
			delete leParseur->Sortie;
		leParseur->Sortie = NULL;
	}

	return 0;
}

Exportation void XipSetDisplayMode(int ipar, XIPLONG mode, int col) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	leParseur->ParametresAffichage = mode;

	leParseur->gAfficherResultats = OUI;
	leParseur->mathematique = NON;

	if (leParseur->TestOptions(AFFICHAGE_MATHEMATIQUE))
		leParseur->mathematique = OUI;

	if (leParseur->TestOptions(AFFICHAGE_NUL))
		leParseur->gAfficherResultats = NON;

	//if (leParseur->TestOptions(AFFICHAGE_NUMERO_DEPENDANCE))
	//    leParseur->AfficheFonction|=AFFICHE_NUMERO;
	//else
	//    leParseur->AfficheFonction&=~AFFICHE_NUMERO;

	leParseur->colonne = col;

	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;
}

Exportation char XipTestFlagDisplayMode(int ipar, XIPLONG mode) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	if ((leParseur->ParametresAffichage&mode) == mode)
		return 1;
	return 0;
}

Exportation TVecteur XipGetDisplayMode(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	return leParseur->ParametresAffichage;
}


Exportation void XipAddFlagDisplayMode(int ipar, XIPLONG mode, int col) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	leParseur->ParametresAffichage |= mode;

	leParseur->gAfficherResultats = OUI;
	leParseur->mathematique = NON;

	if (leParseur->TestOptions(AFFICHAGE_MATHEMATIQUE))
		leParseur->mathematique = OUI;

	if (leParseur->TestOptions(AFFICHAGE_NUL))
		leParseur->gAfficherResultats = NON;

	leParseur->colonne = col;

	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;
}

Exportation void XipRemoveFlagDisplayMode(int ipar, XIPLONG mode) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	leParseur->ParametresAffichage &= ~mode;

	leParseur->gAfficherResultats = OUI;
	leParseur->mathematique = NON;

	if (leParseur->TestOptions(AFFICHAGE_MATHEMATIQUE))
		leParseur->mathematique = OUI;

	if (leParseur->TestOptions(AFFICHAGE_NUL))
		leParseur->gAfficherResultats = NON;


	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;
}

//Cette fonction definit un point d'arret en fonction du nombre de regles deja execute.
Exportation void XipSetRuleRankNumberLimit(int ipar, int order) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	//On desactive le fonctionnement en numero de regle
	leParseur->Limite_Numero_Regle = -1;
	leParseur->Limite_Couche = -1;

	leParseur->Numero_Ordre = order;
}

//Ces fonctions ne sont pas officiellement exportees... Elles sont utilisees dans le cadre du debug
Exportation void XipSetRuleLimit(int ipar, int num_rule) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	//On ne peut avoir les deux mecanismes en meme temps...
	leParseur->Numero_Ordre = -1;
	//la valeur -1 sert a desactiver le tout
	if (leParseur->VlabelVecteur[num_rule] == NULL ||
		leParseur->VlabelVecteur[num_rule]->dr == NULL) {
		leParseur->Limite_Numero_Regle = -1;
		leParseur->Limite_Couche = -1;
	}
	else {
		num_rule++;
		leParseur->Limite_Couche = 0;
		leParseur->Limite_Numero_Regle = num_rule;
		int j = num_rule;
		while (j >= 0 && leParseur->VlabelVecteurIndex[j]->dr == NULL) j--;
		if (leParseur->VlabelVecteurIndex[j]->dr != NULL)
			leParseur->Limite_Couche = leParseur->VlabelVecteurIndex[j]->dr->couche;
	}
}

Exportation char XipSetLabelRuleLimit(int ipar, string label) {
	if (label.length() == 0)
		return OUI;

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NON;

	char chiffre = OUI;
	for (size_t i = 0; i<label.size(); i++) {
		uchar c = (uchar)label[i];
		if (c<'0' || c>'9') {
			chiffre = NON;
			break;
		}
	}

	//Sinon, il s'agit d'un label et on essaie de trouver le numero de regle equivalent
	Label* l = leParseur->rechercheLabel((char*)label.c_str());
	//ce label ne correspond a rien, on le saute sauf s'il n'est compose que de chiffre
	if (l == NULL) {
		if (chiffre == OUI) {
			int num = atoi(label.c_str());
			XipSetRuleLimit(ipar, num);
			return OUI;
		}
		return NON;
	}

	XipSetRuleLimit(ipar, l->index);
	return OUI;
}

Exportation char XipSetWeight(int ipar, int id, float v) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return 0;
	return leParseur->proba_metpoids(id, v);
}

Exportation char XipSetThreshold(int ipar, int id, float v){
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return 0;
	return leParseur->proba_metseuil(id, v);
}

Exportation char XipSetValue(int ipar, int id, float v){
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return 0;
	return leParseur->proba_metvaleur(id, v);
}

Exportation float XipGetWeight(int ipar, int id) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return 0;
	return leParseur->proba_poids(id);
}

Exportation float XipGetThreshold(int ipar, int id) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return 0;
	return leParseur->proba_seuil(id);
}

Exportation float XipGetValue(int ipar, int id) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return 0;
	return leParseur->proba_valeur(id);
}

Exportation int XipRuleCounter(int ipar, int id) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return 0;
	if (leParseur->VBases[id] == NULL)
		return (-1);
	return leParseur->VBases[id]->compteur;
}

Exportation int XipRuleType(int ipar, int id) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return 0;
	if (leParseur->VBases[id] == NULL)
		return (-1);
	return leParseur->VBases[id]->label->type;
}

Exportation int XipRuleLayer(int ipar, int id) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;
	if (leParseur->VBases[id] == NULL)
		return -1;
	return leParseur->VBases[id]->couche;
}

Exportation string XipRuleTypeString(int ipar, int id) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return "";
	if (leParseur->VBases[id] == NULL)
		return "";
	return FieldName(leParseur->VBases[id]->label->type);
}

Exportation int XipNbRules(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return 0;
	return leParseur->VBases.dernier;
}

Exportation encodingType XipGetGrammarCharset(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return LATIN_ENCODING;

	return leParseur->encodage_grammaire;
}
Exportation encodingType XipGetSourceCharset(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return LATIN_ENCODING;

	return leParseur->encodage_source;
}

Exportation encodingType XipGetFSTCharset(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return LATIN_ENCODING;

	return leParseur->encodage_fst;
}

Exportation void XipSetGrammarCharset(int ipar, encodingType ty) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	leParseur->encodage_grammaire = ty;
}

Exportation void XipSetFSTCharset(int ipar, encodingType ty) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	leParseur->encodage_fst = ty;
}


Exportation void XipSetSourceCharset(int ipar, encodingType ty) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	leParseur->encodage_source = ty;
}

Exportation VECTA<Label*>* XipReturnInfoRuleIndex(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NULL;

	return &leParseur->VlabelVecteurIndex;
}

Exportation VECTA<Label*>* XipReturnInfoRuleNumero(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NULL;

	return &leParseur->VlabelVecteur;
}


Exportation void XipReturnRules(int ipar, vector<int>& rulenumbers) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	//VECTA<char> enregistres;
	for (int i = 0; i<leParseur->regles_appliquees.dernier; i++) {
		int v = leParseur->regles_appliquees[i];
		//if (enregistres[v]==1)
		//	continue;
		//enregistres.affecte(v,1);
		rulenumbers.push_back(v);
	}
}

extern "C" {
	//Dans ce cas on compare avec nos variables en memoire et non exterieur
	void NormalisationNomFichier(char* nomFichier, char* tampon) {
		//Tout d'abord on ramene tous les chemins a de l'UNIX
		int l = strlen(tampon);
		//On normalise les chemins
		for (int i = 0; i<l; i++) {
			if (tampon[i] == ANTISEP)
				tampon[i] = SEP;
		}

		//s'il y a une variable d'environnement
		char* vari = strchr(tampon, '$');
		nomFichier[0] = 0;
		while (vari) {
			char* reper = NULL;
			char* pt = strchr(vari + 1, SEP);

			if (pt != NULL)
				*pt = 0;

			int lg = strlen(vari + 1);
			for (int j = 0; j<g_nbvarenv; j++) {
				if (!strcmp(g_environ_varname[j], vari + 1)) {
					reper = g_environ_var[j] + lg + 1;
					break;
				}
			}

			if (reper == NULL)
				reper = getenv(vari + 1);

			//On recopie la partie qui precede la variable
			int lvar = vari - tampon;
			int lnom = strlen(nomFichier);
			memcpy(nomFichier + lnom, tampon, lvar);
			nomFichier[lvar + lnom] = 0;

			if (reper != NULL) {
				strcat(nomFichier, reper);
			}
			else
				g_environ_var_inconnues.ajoute(DupliqueChaine(vari));

			if (pt != NULL) {
				*pt = SEP;
				static char inter[1000];
				strcpy(inter, pt);
				strcpy(tampon, inter);
			}
			else
				tampon[0] = 0;
			vari = strchr(tampon, '$');
		}

		strcat(nomFichier, tampon);

	}
}

Exportation void CutPathFromName(string ptnm, string& path, string& name) {
	int pslash = ptnm.rfind(SEP);

	if (pslash == -1) {
		path = DEB;
		name = ptnm;
	}
	else {
		path = ptnm.substr(0, pslash);
		name = ptnm.substr(pslash + 1, ptnm.size() - pslash);
	}
}

Exportation void ReadPathValue(ifstream& grm) {
	if (g_nbvarenv >= MAX_NB_PATH) {
		sprintf(g_environ_var[g_nbvarenv], "Too many environment variable defined (Max is %d)", MAX_NB_PATH);
		lance_erreur(strdup(g_environ_var[g_nbvarenv]));
	}

	GrmTrim(grm, (uchar*)g_environ_var[g_nbvarenv], 999, 2);
	if (g_environ_var[g_nbvarenv][0] != 0) {
		if (strlen(g_environ_var[g_nbvarenv]) >= 999) {
			strcpy(g_environ_var[g_nbvarenv], "Environment variable too long");
			lance_erreur(strdup(g_environ_var[g_nbvarenv]));
		}

#ifdef WIN32
		_putenv(g_environ_var[g_nbvarenv]);
#else
		putenv(g_environ_var[g_nbvarenv]);
#endif 

		int ik = 0;
		while (g_environ_var[g_nbvarenv][ik] != ' ' && g_environ_var[g_nbvarenv][ik] != '=') ik++;
		memcpy(g_environ_varname[g_nbvarenv], g_environ_var[g_nbvarenv], ik);
		g_environ_varname[g_nbvarenv][ik] = 0;
		g_nbvarenv++;
	}
}

Exportation void XipGetUnknownVariables(VECTA<char*>& unknowns) {
	unknowns.ajoute(g_environ_var_inconnues);
}

Exportation void ResetEnvironmentVariables(string grmname) {
	int ik;

	for (ik = 0; ik<g_environ_var_inconnues.dernier; ik++) {
		if (g_environ_var_inconnues[ik] != NULL)
			libere(g_environ_var_inconnues[ik]);
	}
	g_environ_var_inconnues.raz();
	for (ik = 0; ik<MAX_NB_PATH; ik++) {
		g_environ_var[ik][0] = 0;
		g_environ_varname[ik][0] = 0;
	}
	g_nbvarenv = 0;
	//Initialisation de GRM_PATHNAME
	char* last_slash = strrchr((char*)grmname.c_str(), SEP);
	char grmpath[1000];
	grmpath[0] = '.';
	grmpath[1] = 0;

	if (last_slash != NULL) {
		*last_slash = 0;
		strcpy_s(grmpath,1000, grmname.c_str());
		*last_slash = SEP;
	}

	sprintf(g_environ_var[g_nbvarenv], "GRM_PATHNAME=%s", grmpath);
	strcpy(g_environ_varname[g_nbvarenv], "GRM_PATHNAME");
#ifdef WIN32
	_putenv(g_environ_var[g_nbvarenv]);
#else
	putenv(g_environ_var[g_nbvarenv]);
#endif 
	g_nbvarenv++;
	GRM_COURANT = grmname;
}

void XipSetEnvVariable(string var, string val) {
	string command = var;
#ifdef WIN32
	command += "=";
	command += val;
	_putenv(STR(command));
#else
	setenv(command.c_str(), val.c_str(), 1);
#endif 			
}

string XipGetEnvVariable(string var) {
	char* rep = getenv(var.c_str());
	return rep;
}


/*-------------------------------------------------------------------------*/
void NormaliseNomFichier(char* nomFichier, char* tampon, char* repertoire) {
	//Tout d'abord on ramene tous les chemins a de l'UNIX
	int l = strlen(tampon);
	//On normalise les chemins
	for (int i = 0; i<l; i++) {
		if (tampon[i] == ANTISEP)
			tampon[i] = SEP;
	}

	//s'il y a une variable d'environnement
	char* vari = strchr(tampon, '$');
	nomFichier[0] = 0;
	while (vari) {
		char* reper = NULL;
		char* pt = strchr(vari + 1, SEP);
		if (pt != NULL)
			*pt = 0;

		int lg = strlen(vari + 1);
		for (int j = 0; j<g_nbvarenv; j++) {
			if (!strcmp(g_environ_varname[j], vari + 1)) {
				reper = g_environ_var[j] + lg + 1;
				break;
			}
		}

		if (reper == NULL)
			reper = getenv(vari + 1);

		//On recopie la partie qui precede la variable
		int lvar = vari - tampon;
		int lnom = strlen(nomFichier);
		memcpy(nomFichier + lnom, tampon, lvar);
		nomFichier[lvar + lnom] = 0;

		if (reper != NULL) {
			//Dans le cas ou la variable est en tete                
			//et ou il ne s'agit pas d'un chemin complet
			//      ANK Jan 2007
			//      In the current version the following two
			//      lines cause an error when the variable
			//      contains a relative path name.  Claude thinks
			//      these lines are a leftover from an earlier
			//      form of grammar management.
			// if (!CheminComplet(reper) && tampon[0]=='$')
			//    strcat(nomFichier,repertoire);      

			strcat(nomFichier, reper);
		}

		if (pt != NULL) {
			*pt = SEP;
			static char inter[1000];
			strcpy(inter, pt);
			strcpy(tampon, inter);
		}
		else
			tampon[0] = 0;

		vari = strchr(tampon, '$');
	}

	if (nomFichier[0] == 0) {
		if (!CheminComplet(tampon))
			strcpy(nomFichier, repertoire);
		else
			nomFichier[0] = 0;
	}
}
/*-------------------------------------------------------------------------*/

Exportation bool XipSetKifCode(int ipar, KifCode* kifcode) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return false;
	leParseur->kifloader = kifcode;
	return true;
}


void GrmRead(ifstream& grm, char* buf, int max) {
	int i = 0;
	char c = 0;
	grm.get(c);
	while (lespace(c) && !grm.eof()) grm.get(c);

	while (i<max && c>32 && !grm.eof()) {
		buf[i++] = c;
		grm.get(c);
	}
	buf[i] = 0;
}

void GrmReadLine(ifstream& grm, char* buf, int max) {
	int i = 0;
	char c = 0;
	grm.get(c);
	while (lespace(c) && !grm.eof()) grm.get(c);

	while (i<max && !grm.eof() && c != '\n' && c != '\r') {
		buf[i++] = c;
		grm.get(c);
	}
	buf[i] = 0;
}

Exportation void GrmTrim(ifstream& grm, uchar* buf, int max, char norm) {
	string chemin;
	char boucle = 1;
	buf[0] = 0;

	getline(grm, chemin);
	chemin = Trim(chemin);

	while (!grm.eof()) {
		if (chemin.size() != 0 && chemin[0] != '#')
			break;
		getline(grm, chemin);
		chemin = Trim(chemin);
	}

	if (norm != NON) {
		if (strchr(STR(chemin), '=') != NULL && chemin[0] == '$')
			lance_erreur("Wrong path definition");
		char chemloc[1000];
		strcpy_s(chemloc, 1000, STR(chemin));
		NormalisationNomFichier((char*)buf, chemloc);
	}
	else
		strcpy((char*)buf, STR(chemin));
}


char LoadGrmFiles(Fichiergrm& fgrm, int& ITAILLEVECT, char premier) {

	char bufread[1000];
	grmkifarguments.clear();
	char crypte = NON;
	Trim(fgrm.grmname);
	ifstream grm(fgrm.grmname, modeOuverture);


	if (grm.fail()) {
		sprintf_s(bufread,1000, "Cannot open: %s", fgrm.grmname);
		lance_erreur(strdup(bufread));
	}

	int ik;
	if (premier == OUI) {
		g_nbvarenv = 1;
		for (ik = 0; ik<g_environ_var_inconnues.dernier; ik++) {
			if (g_environ_var_inconnues[ik] != NULL)
				libere(g_environ_var_inconnues[ik]);
		}
		g_environ_var_inconnues.raz();
		for (ik = 0; ik<MAX_NB_PATH; ik++) {
			g_environ_var[ik][0] = 0;
			g_environ_varname[ik][0] = 0;
		}
	}

	GRM_COURANT = fgrm.grmname;
	int numberValues = ITAILLEVECT;
	//Initialisation de GRM_PATHNAME
	char* last_slash = strrchr(fgrm.grmname, SEP);
	char grmkeep[1024];
	char grmpath[1000];
	grmpath[0] = '.';
	grmpath[1] = 0;

	if (last_slash != NULL) {
		*last_slash = 0;
		strcpy_s(grmpath,1000, fgrm.grmname);
		*last_slash = SEP;
	}

	sprintf(g_environ_var[0], "GRM_PATHNAME=%s", grmpath);
	strcpy(g_environ_varname[0], "GRM_PATHNAME");
#ifdef WIN32
	_putenv(g_environ_var[0]);
#else
	putenv(g_environ_var[0]);
#endif 


	//----------------------------------------------------------------
	while (!grm.eof()) {
		GrmTrim(grm, (uchar*)bufread, 999, NON);

		if (!*bufread)
			break;

		if (!strcmp(bufread, "conversion:")) {
			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (!strcmp(bufread, "utf82ascii"))
				fgrm.conversion = OUI;
			continue;
		}

		//First line is grammar
		//Second line is ntm
		//thrid line is hmm
		//following lines are parameter files            
		if (!strcmp(bufread, "path:")) {
			if (g_nbvarenv >= MAX_NB_PATH) {
				sprintf(g_environ_var[g_nbvarenv], "Too many environment variable defined (Max is %d)", MAX_NB_PATH);
				lance_erreur(strdup(g_environ_var[g_nbvarenv]));
			}

			GrmTrim(grm, (uchar*)g_environ_var[g_nbvarenv], 999, 2);
			if (g_environ_var[g_nbvarenv][0] != 0) {
				if (strlen(g_environ_var[g_nbvarenv]) >= 999) {
					strcpy(g_environ_var[g_nbvarenv], "Environment variable too long");
					lance_erreur(strdup(g_environ_var[g_nbvarenv]));
				}

#ifdef WIN32
				_putenv(g_environ_var[g_nbvarenv]);
#else
				putenv(g_environ_var[g_nbvarenv]);
#endif 

				ik = 0;
				while (g_environ_var[g_nbvarenv][ik] != ' ' && g_environ_var[g_nbvarenv][ik] != '=') ik++;
				memcpy(g_environ_varname[g_nbvarenv], g_environ_var[g_nbvarenv], ik);
				g_environ_varname[g_nbvarenv][ik] = 0;
				g_nbvarenv++;
			}
			continue;
		}

		if (!strcmp(bufread, "indexingbuffersize:")) {
			grm.getline(bufread, 999);
			Trim(bufread);
			MAXVIRTUELVALEURINITIALE = atoi(bufread);
			continue;
		}

		if (!strcmp(bufread, "data:")) {
			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0)
				fgrm.datafiles.push_back(bufread);

			continue;
		}

		if (!strcmp(bufread, "number:")) {
			grm.getline(bufread, 999);
			Trim(bufread);
			ITAILLEVECT = atoi(bufread);
			continue;
		}

		if (!strcmp(bufread, "layer:")) {
			grm.getline(bufread, 999);
			Trim(bufread);
			fgrm.lacoucheinitiale = atoi(bufread);
			continue;
		}
		//lecture de variable de definition
		if (!strcmp(bufread, "variable:")) {
			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Variable name is too long");
					lance_erreur(strdup(bufread));
				}
				fgrm.g_definition_var.push_back(bufread);
			}
			continue;
		}

		//lecture de la strategy de lookup
		if (!strcmp(bufread, "strategy:")) {
			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Variable name is too long");
					lance_erreur(strdup(bufread));
				}
				if (!strcmp(bufread, "DEPTH"))
					fgrm.strategy = DEPTH;
				else
				if (!strcmp(bufread, "BREADTH"))
					fgrm.strategy = BREADTH;
				else {
					strcpy_s(bufread,1000, "Unknown value for STRATEGY: should be DEPTH or BREADTH");
					lance_erreur(strdup(bufread));
				}
			}
			continue;
		}

		if (!strcmp(bufread, "flags:")) {

			if (fgrm.lookup_flags[0]) {
				strcpy_s(bufread,1000, "A FLAG string has already been provided");
				lance_erreur(strdup(bufread));
			}

			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Flag string too long");
					lance_erreur(strdup(bufread));
				}
				strcpy_s(fgrm.lookup_flags,1024, bufread);
			}
			continue;
		}

		if (!strcmp(bufread, "blockingrulespace:")) {
			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Blocking Rule Space string too long");
					lance_erreur(strdup(bufread));
				}
				fgrm.blockingrule.push_back(bufread);
			}
			continue;
		}

		if (!strcmp(bufread, "kifarg:")) {
			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "KIFARG too long");
					lance_erreur(strdup(bufread));
				}
				grmkifarguments.push_back(bufread);
			}
			continue;
		}


		if (!strcmp(bufread, "grm:")) {
			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Pathname for GRM too long");
					lance_erreur(strdup(bufread));
				}
				strcpy_s(grmkeep,1024, fgrm.grmname);
				strcpy_s(fgrm.grmname,1024, bufread);
				crypte = LoadGrmFiles(fgrm, numberValues, NON);
				strcpy_s(fgrm.grmname,1024, grmkeep);
			}
			continue;
		}

		if (!strcmp(bufread, "lookup:")) {
			if (fgrm.lookup[0]) {
				strcpy_s(bufread,1000, "A LOOKUP file has already been provided");
				lance_erreur(strdup(bufread));
			}

			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Pathname for LOOKUP too long");
					lance_erreur(strdup(bufread));
				}
				strcpy(fgrm.lookup, bufread);
			}
			continue;
		}

		if (!strcmp(bufread, "tokenizer:")) {
			if (fgrm.tokenize[0]) {
				strcpy_s(bufread,1000, "A TOKENIZER file has already been provided");
				lance_erreur(strdup(bufread));
			}

			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Pathname for TOKENIZER too long");
					lance_erreur(strdup(bufread));
				}
				strcpy_s(fgrm.tokenize,1024, bufread);
			}
			continue;
		}

		if (!strcmp(bufread, "generation:")) {
			if (fgrm.genere[0]) {
				strcpy_s(bufread,1000, "A GENERATION file has already been provided");
				lance_erreur(strdup(bufread));
			}

			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Pathname for GENERATION too long");
					lance_erreur(strdup(bufread));
				}
				strcpy_s(fgrm.genere,1024, bufread);
			}
			continue;
		}

		if (!strcmp(bufread, "ntm:")) {
			if (fgrm.ntm[0]) {
				strcpy_s(bufread,1000, "A NTM file has already been provided");
				lance_erreur(strdup(bufread));
			}

			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Pathname for NTM too long");
					lance_erreur(strdup(bufread));
				}
				strcpy(fgrm.ntm, bufread);
			}
			continue;
		}

		if (!strcmp(bufread, "trans:")) {
			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Pathname for TRANS too long");
					lance_erreur(strdup(bufread));
				}
				fgrm.trans.push_back(bufread);
			}
			continue;
		}

		if (!strcmp(bufread, "hmm:")) {
			if (fgrm.hmm[0]) {
				strcpy_s(bufread,1000, "A HMM file has already been provided");
				lance_erreur(strdup(bufread));
			}

			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Pathname for HMM too long");
					lance_erreur(strdup(bufread));
				}
				strcpy(fgrm.hmm, bufread);
			}
			continue;
		}

		if (!strcmp(bufread, "crfmodel:")) {
			if (fgrm.model[0]) {
				strcpy_s(bufread,1000, "A CRF model file has already been provided");
				lance_erreur(strdup(bufread));
			}

			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Pathname for MODEL too long");
					lance_erreur(strdup(bufread));
				}
				strcpy_s(fgrm.model,1024, bufread);
			}
			continue;
		}

		if (!strcmp(bufread, "crfmkcls:")) {
			if (fgrm.mkcls[0]) {
				strcpy_s(bufread,1000, "A MKCLS file has already been provided");
				lance_erreur(strdup(bufread));
			}

			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Pathname for MKCLS too long");
					lance_erreur(strdup(bufread));
				}
				strcpy_s(fgrm.mkcls,1024, bufread);
			}
			continue;
		}

		if (!strcmp(bufread, "crfbrown:")) {
			if (fgrm.brown[0]) {
				strcpy_s(bufread,1000, "A BROWN file has already been provided");
				lance_erreur(strdup(bufread));
			}

			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Pathname for BROWN too long");
					lance_erreur(strdup(bufread));
				}
				strcpy_s(fgrm.brown,1024, bufread);
			}
			continue;
		}

		if (!strcmp(bufread, "grammar:")) {
			if (fgrm.grammar[0]) {
				strcpy_s(bufread,1000, "A GRAMMAR file has already been provided");
				lance_erreur(strdup(bufread));
			}
			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Pathname for GRAMMAR too long");
					lance_erreur(strdup(bufread));
				}
				strcpy(fgrm.grammar, bufread);
			}
			continue;
		}
		if (!strcmp(bufread, "gram:")) {
			if (fgrm.grammar[0]) {
				strcpy_s(bufread,1000, "A GRAMMAR file has already been provided");
				lance_erreur(strdup(bufread));
			}
			crypte = OUI;
			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Pathname for GRAM too long");
					lance_erreur(strdup(bufread));
				}
				strcpy(fgrm.grammar, bufread);
			}
			continue;
		}
		if (!strcmp(bufread, "parameter:") ||
			!strcmp(bufread, "addendum:")) {
			GrmTrim(grm, (uchar*)bufread, 999, OUI);
			if (bufread[0] != 0) {
				if (strlen(bufread)>999) {
					strcpy_s(bufread,1000, "Pathname for PARAMETER too long");
					lance_erreur(strdup(bufread));
				}
				fgrm.parameters.push_back(DupliqueChaine(bufread));
			}
			continue;
		}

		char err[100];
		sprintf_s(err,100, "Unknown GRAMMAR file field:\"%s\"", bufread);
		lance_erreur(err);
	}
	return crypte;
}

//Cette methode est utilisee pour enregistrer un objet GlobalParseur provenant d'une creation externe
Exportation int XipAddExternalGlobalParseur(GlobalParseur* gb) {
	gb->iParseurCourant = gVecteurParseurs.ajoute(gb);
	return gb->iParseurCourant;
}

#ifndef XIPEXTENALDERIVATION
Exportation void Whoami(char* question)
{
	sprintf(question, "XIP: Xerox Incremental Parser %s (1999-2016) KiF: %s", VERSION_XIP_BUILD,KIFVERSION);
}


Exportation void Whoami(string& question)
{
	char quest[1024];
	Whoami(quest);
	question = quest;
}

int XipGrmFileXX(char* grmname, int loadall, int xml, bool crypteduplique, KifCode* kifcode = NULL) {

	Fichiergrm fgrm;
	int numberValues = -1;

	strcpy_s(fgrm.grmname,1024, grmname);

	char crypte = LoadGrmFiles(fgrm, numberValues, OUI);
	if (crypteduplique)
		crypte = UTILISE;


	//dans le cas ou l'on ne charge que la grammaire...
	if (loadall == NON)
		fgrm.RAZ();

	int ipar = ChargementGrammaire(fgrm, xml, crypte, numberValues, kifcode);

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	for (size_t ide = 0; ide<fgrm.blockingrule.size(); ide++)
		leParseur->desactiveEspaceRegle((char*)fgrm.blockingrule[ide].c_str());

	if (fgrm.conversion == OUI)
		leParseur->ChoixUTF8(AFFICHAGE_UTF8);

	return ipar;
}


Exportation int XipGrmFileX(char* grmname, int loadall, int xml, KifCode* kifcode = NULL) {
	bool crypteduplique = false;
	return XipGrmFileXX(grmname, loadall, xml, crypteduplique, kifcode);
}

Exportation int XipEncryptGrammar(const char* grmname, const char* destdir, int loadall, int xml) {
	char *grmnamecopy = strdup(grmname);
	duplication_crypte = OUI;
	if (strlen(destdir) > 999) {
		lance_erreur("Error: destination directory path too long.\n");
	}
	strcpy(crypte_repertoire, destdir);
	if (crypte_repertoire[strlen(crypte_repertoire) - 1] == SEP)
		crypte_repertoire[strlen(crypte_repertoire)] = 0;
	bool crypteduplique = true;
	int result = XipGrmFileXX(grmnamecopy, loadall, xml, crypteduplique);
	free(grmnamecopy);
	return result;
}

Exportation int XipGrmFile(string grmname, char loadall, int xml) {
	return XipGrmFileX((char*)grmname.c_str(), loadall, xml);
}

Exportation int XipKifGrmFile(string grmname, KifCode* kifcode, char loadall, int xml) {
	if (kifcode != NULL)
		kifcode->master = true;
	int i = XipGrmFileX((char*)grmname.c_str(), loadall, xml, kifcode);
	XipInitKifParseur(i);
	return i;
}

#endif

Exportation char XipGetLabels(int ipar, string& surface, string& lemme) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	surface = leParseur->SURFACE;
	lemme = leParseur->LEMME;
	return 0;
}


//Manipulation des fonctions de CallBack

Exportation void XipSetErrorStream(ostringstream* os) {
	if (os == NULL)
		flot_erreur = &cerr;
	else
		flot_erreur = os;
}

Exportation char XipSetCallBack(int ipar, XipFunction f, void* data) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	leParseur->AppelExterne = f;
	leParseur->donneesExternes = data;
	return 0;
}

Exportation XipResult* XipCreateCurrentXipResult(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NULL;
	if (leParseur->xipresult != NULL)
		return leParseur->xipresult;

	leParseur->xipresult = new XipResult;
	return leParseur->xipresult;
}


Exportation XipResult* XipCleanCurrentXipResult(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NULL;


	//if (leParseur->xipresult!=NULL)
	//    delete leParseur->xipresult;

	//leParseur->xipresult=new XipResult;    

	if (leParseur->xipresult != NULL) {
		size_t i;
		for (i = 0; i<leParseur->xipresult->sentences.size(); i++)
			delete leParseur->xipresult->sentences[i];
		leParseur->xipresult->sentences.clear();
	}

	return leParseur->xipresult;

}

Exportation XipResult* XipCleanCurrentFullXipResult(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NULL;


	//if (leParseur->xipresult!=NULL)
	//    delete leParseur->xipresult;

	//leParseur->xipresult=new XipResult;    

	if (leParseur->xipresult != NULL) {
		size_t i;
		for (i = 0; i<leParseur->xipresult->sentences.size(); i++)
			delete leParseur->xipresult->sentences[i];
		leParseur->xipresult->sentences.clear();
		leParseur->Detruit();
	}

	return leParseur->xipresult;

}



Exportation XipResult* XipGetCurrentXipResult(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NULL;
	return leParseur->xipresult;
}

Exportation char XipDeleteCurrentXipResult(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	if (leParseur->xipresult == NULL)
		return 0;

	delete leParseur->xipresult;
	leParseur->xipresult = NULL;
	return 0;
}

Exportation char XipGetVariableValue(int ipar, string variable, string& val) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	etat* e = leParseur->VariablesNoms.recherche((uchar*)variable.c_str());
	//Relecture de variables
	if (e == NULL || e->valeur == NULL)
		return NON;

	return RenvoieValeurVariable((Variable*)e->valeur, val);
}

Exportation char XipGetDictionaryVariable(int ipar, string variable, vector<string>& dict) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	etat* e = leParseur->VariablesNoms.recherche((uchar*)variable.c_str());
	//Relecture de variables
	if (e == NULL || e->valeur == NULL)
		return NON;
	Variable* v = (Variable*)e->valeur;
	if (v->sorte != VDICTIONNAIRE)
		return -2;

	VariableDictionnaire* vdict = (VariableDictionnaire*)e->valeur;
	for (map<string, string>::iterator iter = vdict->dico.begin(); iter != vdict->dico.end(); iter++) {
		dict.push_back((*iter).first);
		dict.push_back((*iter).second);
	}

	return OUI;
}

Exportation char XipSetDictionaryVariable(int ipar, string variable, vector<string>& dict) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL || (dict.size() % 2) != 0)
		return -1;

	etat* e = leParseur->VariablesNoms.recherche((uchar*)variable.c_str());
	//Relecture de variables
	if (e == NULL || e->valeur == NULL)
		return NON;

	Variable* v = (Variable*)e->valeur;
	if (v->sorte != VDICTIONNAIRE)
		return -2;

	VariableDictionnaire* vdict = (VariableDictionnaire*)v;
	for (size_t i = 0; i<dict.size(); i += 2)
		vdict->AffecteValeurStringIndexChaine(new string(dict[i + 1]), dict[i]);
	return OUI;
}


Exportation char XipSetVariable(int ipar, unsigned char* variable, float val) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	etat* e = leParseur->VariablesNoms.recherche(variable);
	//Relecture de variables
	if (e == NULL || e->valeur == NULL)
		return 0;

	return AffectationVariable((Variable*)e->valeur, val, "");
}

Exportation char XipSetStringVariable(int ipar, unsigned char* variable, unsigned char* value) {

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	etat* e = leParseur->VariablesNoms.recherche(variable);
	//Relecture de variables
	if (e == NULL || e->valeur == NULL)
		return 0;

	return AffectationVariable((Variable*)e->valeur, -1, (char*)value);
}

Exportation char XipAddStringVariable(int ipar, unsigned char* string_vector, unsigned char* value) {

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	etat* e = leParseur->VariablesNoms.recherche(string_vector);
	//Relecture de variables
	if (e == NULL || e->valeur == NULL || ((Variable*)(e->valeur))->sorte != VVECTEURCHAINE)
		return 0;

	return AffectationVariable((Variable*)e->valeur, -10, (char*)value);
}

Exportation char XipClearVariable(int ipar, unsigned char* string_vector) {

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	etat* e = leParseur->VariablesNoms.recherche(string_vector);
	//Relecture de variables
	if (e == NULL || e->valeur == NULL || ((Variable*)(e->valeur))->sorte != VVECTEURCHAINE)
		return 0;

	return AffectationVariable((Variable*)e->valeur, -100, "");
}


Exportation char XipSetIntVariable(int ipar, unsigned char* variable, float value) {

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	etat* e = leParseur->VariablesNoms.recherche(variable);
	//Relecture de variables
	if (e == NULL || e->valeur == NULL)
		return 0;

	return AffectationVariable((Variable*)e->valeur, value, "a");
}

Exportation char XipAddIntVariable(int ipar, unsigned char* string_vector, float value) {

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	etat* e = leParseur->VariablesNoms.recherche(string_vector);
	//Relecture de variables
	if (e == NULL || e->valeur == NULL || ((Variable*)(e->valeur))->sorte != VVECTEURINT)
		return 0;

	return AffectationVariable((Variable*)e->valeur, value, "i");
}

Exportation char XipClearIntVariable(int ipar, unsigned char* int_vector) {

	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	etat* e = leParseur->VariablesNoms.recherche(int_vector);
	//Relecture de variables
	if (e == NULL || e->valeur == NULL || ((Variable*)(e->valeur))->sorte != VVECTEURINT)
		return 0;

	return AffectationVariable((Variable*)e->valeur, 0, "c");
}

//----------------------------------------------------------------------
#ifdef XIPSTLAPI
//Version pour les exportations en mode STL


/*
The 3 procedures: GrmRead, GrmTrim and GrmFile read and parse a grm file where filenames are
organized in section. The grm file is not a real part of XIP, but it simplifies the loading and
maintenance of XIP grammars. Those procedures are compatible with xipwin.exe loading of grm files.
*/

Exportation int XipGrammarLoading(string grammaire, string generefile, int xml, string ntmfile,
	string hmmfile, string token, string lookup,
	string lookup_flags,
	lookup_strategy strategy) {

	return XipGrammarLoadingBase((char*)grammaire.c_str(), (char*)generefile.c_str(), xml,
		(char*)ntmfile.c_str(), (char*)hmmfile.c_str(),
		(char*)token.c_str(), (char*)lookup.c_str(), (char*)lookup_flags.c_str(), strategy);
}


Exportation int XipLoadGrammar(string grammaire, string generefile, int xml, string ntmfile,
	string hmmfile, string token, string lookup,
	string lookup_flags,
	lookup_strategy strategy) {

	return XipLoadGrammarBase((char*)grammaire.c_str(), (char*)generefile.c_str(), xml,
		(char*)ntmfile.c_str(), (char*)hmmfile.c_str(),
		(char*)token.c_str(), (char*)lookup.c_str(), (char*)lookup_flags.c_str(), strategy);
}

Exportation int XipParseFile(string texte, int ipar, XipResult* xip, char applique_grammaire) {
	return XipParseFile((char*)texte.c_str(), ipar, xip, applique_grammaire);
}

Exportation int XipParseString(string texte, int ipar, XipResult* xip, char applique_grammaire) {
	return XipParseString((unsigned char*)texte.c_str(), ipar, xip, applique_grammaire);
}


Exportation int XipParameterFile(int ipar, string filename) {
	return XipParameterFile(ipar, (char*)filename.c_str());
}

Exportation char XipSetVariable(int ipar, string variable, float value) {
	return XipSetVariable(ipar, (uchar*)variable.c_str(), value);
}

Exportation char XipSetStringVariable(int ipar, string variable, string value) {
	return XipSetStringVariable(ipar, (uchar*)variable.c_str(), (uchar*)value.c_str());
}

Exportation char XipAddStringVariable(int ipar, string string_vector, string value) {
	return XipAddStringVariable(ipar, (uchar*)string_vector.c_str(), (uchar*)value.c_str());
}

Exportation char XipSetIntVariable(int ipar, string variable, float value) {
	return XipSetIntVariable(ipar, (uchar*)variable.c_str(), value);
}

Exportation char XipAddIntVariable(int ipar, string string_vector, float value) {
	return XipAddIntVariable(ipar, (uchar*)string_vector.c_str(), value);
}

Exportation char XipClearVariable(int ipar, string string_vector) {
	return XipClearVariable(ipar, (uchar*)string_vector.c_str());
}

Exportation char XipClearIntVariable(int ipar, string string_vector) {
	return XipClearIntVariable(ipar, (uchar*)string_vector.c_str());
}

Exportation void XipDisplayActivatedRules(int ipar, ostringstream* os, int index, char display) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL || os == NULL)
		return;

#if (defined(WIN32) && defined(_WINDLL))
	if (os != NULL) {
		locale loc_dll;
		os->imbue(loc_dll);
	}
#endif

	leParseur->FormatRuleText(*os, index, display);
}

Exportation void XipGetListVariables(int ipar,
	vector<string>& names,
	vector<typeVariable>& types) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	for (int i = 0; i<leParseur->Variables.dernier; i++) {
		names.push_back(leParseur->Variables[i]->nom);
		types.push_back(leParseur->Variables[i]->type);
	}
}

Exportation void XipGetListDependencies(int ipar, vector<string>& list) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	for (int i = 0; i<leParseur->Fonctions.dernier; i++) {
		list.push_back(leParseur->Fonctions[i]->X.nom);
		if (leParseur->Fonctions[i]->fichier != NULL)
			list.push_back(leParseur->Fonctions[i]->fichier);
		else
			list.push_back("nofile");
	}
}

Exportation void XipGetListCategories(int ipar, vector<string>& list) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	for (int i = 0; i<leParseur->Categories.dernier; i++) {
		list.push_back(leParseur->Categories[i]->X.nom);
		if (leParseur->Categories[i]->fichier != NULL)
			list.push_back(leParseur->Categories[i]->fichier);
		else
			list.push_back("nofile");
	}
}

#ifdef XIPLIBXML

Exportation char XipSetCallBackXML(int ipar, XipFunction f, void* data) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;

	leParseur->AppelExterneXml = f;
	leParseur->donneesExternesXml = data;
	return 0;
}

Exportation int XipLoadXMLDataBase(int ipar, string texte, string alias) {
	return XipLoadXMLDataBase(ipar, (char*)texte.c_str(), (char*)alias.c_str());
}

Exportation int XipParseXMLFile(int ipar, string texte, int profondeur, XipResult* xip) {
	return XipParseXMLFile(ipar, (char*)texte.c_str(), profondeur, xip);
}

Exportation int XipParseXMLString(int ipar, string texte, int profondeur, XipResult* xip) {
	return XipParseXMLString(ipar, (char*)texte.c_str(), profondeur, xip);
}

Exportation void XipAliasList(int ipar, vector<string>& alias) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	for (int i = 0; i<leParseur->ListeAlias.dernier; i++)
		alias.push_back(Majuscule(leParseur->ListeAlias[i]));
}


#endif
Exportation rulespace* XipReturnRuleSpaceRoot(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NULL;
	return leParseur->spacename;
}

Exportation char XipAddBlockingRuleSpace(int ipar, string name) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NON;
	return leParseur->desactiveEspaceRegle(STR(name));
}

Exportation char XipRemoveBlockingRuleSpace(int ipar, string name, char full) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NON;
	return leParseur->reactiveEspaceRegle(STR(name), full);
}

Exportation rulespace* XipReturnRuleSpace(int ipar, string name) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return NULL;
	return leParseur->regleEspaceRegle(STR(name));
}

Exportation int XipGetMaxSentence(int ipar) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -100;
	return leParseur->MaxSentence;
}

Exportation void XipSetMaxSentence(int ipar, int value) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;
	leParseur->MaxSentence = value;
}


Exportation void XipLayerRenumbering(int ipar, int idrule) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	leParseur->RenumeroteCouche(idrule, 1);
}
Exportation void XipGetGraphs(int ipar, vector<XipGraph*>& graphes) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;

	for (int i = 0; i<leParseur->listegraphes.dernier; i++) {
		if (leParseur->listegraphes[i] != NULL && leParseur->listegraphes[i]->instanciee == NON) {
			XipGraph* xg = leParseur->listegraphes[i]->ConstruitResultat();
			graphes.AJOUT(xg);
		}
	}
}

Exportation void StoreProbabilisticModel(int ipar, string filename) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return;
	leParseur->sauvegardeprobabilites(filename);
}

Exportation char LoadProbabilisticModel(int ipar, string filename) {
	GlobalParseur* leParseur = gVecteurParseurs[ipar];
	if (leParseur == NULL)
		return -1;
	return leParseur->chargeprobabilites(filename);
}

#endif

#ifdef WORDVB

char ConstruitSurface(XipNode* n, string& surface, int& droite) {
	if (n->Type() == XIPLEAF) {
		XipLeaf* f = (XipLeaf*)n;
		if (surface != "") {
			for (int i = droite; i != n->leftchar; i++)
				surface += " ";
		}
		surface += f->surface;
		return 1;
	}
	else {
		VXipNode::iterator da;
		int deb = 1;
		for (da = n->daughters.begin(); da != n->daughters.end(); da++) {
			if (ConstruitSurface(*da, surface, droite) == 1)
				break;
			droite = (*da)->rightchar;
		}
	}
	return 0;
}

extern "C" {
	Exportation void _stdcall visualloadgrm(LPSTR g, int l, int x) {
		if (gVecteurParseurs[0] == NULL)
			XipGrmFileX(g, l, x);
	}

	Exportation void _stdcall analysechaine(LPSTR ch, LPSTR resultat, int max) {
		ostringstream os;
		XipParseStringOS(ch, 0, &os);
		int mx = os.str().size();
		if (mx >= max - 1)
			mx = max - 1;
		for (int i = 0; i<mx; i++)
			resultat[i] = os.str()[i];
	}

	Exportation int _stdcall analysetableau(LPSTR ch, SAFEARRAY **tableau, int max) {
		BSTR *chaine;
		HRESULT ret;
		int nb = 0;
		if ((ret = SafeArrayAccessData(*tableau, (void **)&chaine)) == S_OK) {
			int maxelements = (*tableau)->rgsabound->cElements;
			XipResult xip;
			XipParseString(ch, 0, &xip, 1);
			for (int i = 0; i<xip.sentences.size(); i++) {
				for (int j = 0; j<xip.sentences[i]->dependencies.size(); j++) {
					XipDependency* dep = xip.sentences[i]->dependencies[j];
					string res = dep->name + " ; ";
					for (int k = 0; k<dep->parameters.size(); k++) {
						string surface;
						int droite = 0;
						ConstruitSurface(dep->parameters[k], surface, droite);
						if (k)
							res += " ; ";
						res += surface;
					}
					int mx = res.size();
					if (mx >= max)
						mx = max;

					LPTSTR ch = (LPTSTR)chaine[nb];
					for (int u = 0; u<mx; u++)
						ch[u] = res[u];

					nb++;
					if (nb >= maxelements) {
						SafeArrayUnaccessData(*tableau);
						return nb;
					}
				}
			}
		}
		SafeArrayUnaccessData(*tableau);
		return nb;
	}

	void __stdcall test(SAFEARRAY **tableau) {

		BSTR *chaine;
		HRESULT ret;
		unsigned long i;

		if ((ret = SafeArrayAccessData(*tableau, (void **)&chaine)) == S_OK)
		{
			for (i = 0; i < (*tableau)->rgsabound->cElements; i++) {
				CharUpper((LPTSTR)chaine[i]);
			}
			SafeArrayUnaccessData(*tableau);
		}
	}

	Exportation void _stdcall affiche(LPSTR ch) {
		ofstream debug("C:\\XIP\\Test\\Word\\debug.txt");
		debug << ch << endl;
		debug.flush();
		ch[0] = 'A';
		ch[1] = 0;
	}

	void _stdcall unload() {
		XipFreeGrammar(0);
	}
}
#endif



