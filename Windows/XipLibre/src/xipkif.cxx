/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the XIP library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : XIP
Version    : 10.00
filename   : xipkif.cxx
Date       : 09/09/2010
Purpose    : Specific implementation of the KiF language to account for XIP specific objects
Programmer : Claude ROUX
Reviewer   :
*/



#include "parsbin.h"
#include "ntmhmm.h"

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


#include "kif.h"
#include "kifversion.h"
#include "transduct.h"

//------------------------------------------
#include "xipkif.h"
//------------------------------------------
#include "conversion.h"
#include "graphe.h"
#include "kifcontainer.h"
#include "kifmaps.h"
#include "kiftreemaps.h"
#include "kifxml.h"
#include "kifrulegen.h"
#include "fractalhmap.h"
#include "kifpmaps.h"

#ifndef WIN32
#define strcat_s(x,y,z) strcat(x,z)
#endif

extern ostream* flot_erreur;
static char xiptagger = OUI;
extern char affiche_regle_appliquee;
extern char statistique_interne;
Exportation void XipGetListCategories(int, vector<string>&);
Exportation void XipGetListDependencies(int, vector<string>&);
Exportation int XipParseFileToOS(string nomfichier, int ipar, ostream* os, char xml, char applique_grammaire);
void kif_erreur(string message, int ligne);
bool XipSetKifCode(int ipar, KifCode* kifcode);

#ifndef WIN32
#define sprintf_s snprintf
#define strcpy_s(a,b,c) strncpy(a,c,b)
#endif

extern "C" {
	Exported KifType xmldoc_Type();
	bool xmldoc_KifInitialisationModule(KifCode* kifcode, string version);
	bool Init_xmldoc(KifCode* kifcode, string version);
	Exported bool kifsparse_KifInitialisationModule(KifCode* kifcode, string version);
	bool kifsys_KifInitialisationModule(KifCode* localkifcode, string version);
	bool kifsocket_KifInitialisationModule(KifCode* localkifcode, string version);
	Exported void SetXipExecution(bool);

}

KifElement* KifSetFeatures(KifCode* kifcode, KifElement* kf, string& att, string& vval);
Exported bool KifRecordLibraries(string name, KifExternalModule);
extern KifGlobalThread* kifGlobals;
extern GlobalParseur* BaseParseur;
extern GlobalParseur* parseur_courant;
Exported KifType KifNewType();

typedef KifElement* (KifCode::*KifParseElement)(x_node*, KifElement*);
void ParseurKifInit(KifCode* kifcodeptr);

Exportation void XipSetRuleLimit(int ipar, int num_rule);
Exportation void XipDisplayActivatedRules(int ipar, ostringstream* os, int index, char display);
Exportation int XipNbRules(int ipar);
Exportation void XipReturnRules(int ipar, vector<int>& rulenumbers);
string& Trim(string& chaine);
string KifIdString(short);


extern int kif_declencheur;

static  hmap<short, bool> kifParserFunctions;
static  hmap<short, bool> kifNodeFunctions;
static  hmap<short, bool> kifGraphFunctions;
static  hmap<short, bool> kifDependencyFunctions;
static  hmap<short, bool> kifGenerationFunctions;

static  map<string, string> kifInformationNode;
static  map<string, string> kifInformationDependency;
static  map<string, string> kifInformationGeneration;
static  map<string, string> kifInformationGraph;
static  map<string, string> kifInformationParser;

static  hmap<string, KifParseElement> kifFunctions;
static  hmap<string, KifParseElement> kifTypes;

inline GlobalParseur* Parseur(KifCode* k) {
	if (kifGlobals->data == NULL) {
		kifGlobals->data = (void*)new GlobalParseur("", "");
		kifGlobals->cleandata = true;
	}
	return (GlobalParseur*)kifGlobals->data;
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
	vector<string>& divers);


//--------------------------------------------------------------------------------------

void KiFSetdebug(bool d);


#ifdef _DEBUG
extern KifCode* currentkifcode;
#endif


bool TestkifFunctions(short n, hmap<short, bool>& kiffunc);
Exported int XipParseStringAndOS(string texte, int ipar, XipResult* xip, ostringstream* os, char xml, char applique_grammaire);
int KifLoadPath(KifCode* loader, string& path, vector<string>& arguments, bool debugkif);
extern VecteurDeParseurs gVecteurParseurs;
string replacestringwithautomaton(string &str, string& reg, string& rep);
void KifInitFunctions(KifCode*, KifFrame* kstart);
Exportation GlobalParseur* XipGetParser(unsigned int iParseur);

extern const char* kifErrorStrings[];

Exportation int XipKifGrmFile(string grmname, KifCode*, char loadall = 1, int xml = 0);
char parentde(ResultatFonction* fille, ResultatFonction* parent);
ResultatFonction* suivantde(ResultatFonction* courant, ResultatFonction* suivant, int increment);
ResultatFonction* perede(ResultatFonction* courant, ResultatFonction* suivant);
ResultatFonction* filsde(ResultatFonction* courant, ResultatFonction* suivant, char dernier);
ResultatFonction* tokensuivant(GlobalParseur*, ResultatFonction* courant, ResultatFonction* suivant, int pos);
short KifNewId(string n);

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------


Exported KifElement* KifXipGeneration::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifInformationGeneration.find(s) == kifInformationGeneration.end())
		return kifNULL;
	return kifcode->Providestringraw(kifInformationGeneration[s]);
}

KifElement* KifXipGeneration::Declaration(short n) {
	if (TestkifFunctions(n, kifGenerationFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifXipGeneration::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGenerationFunctions.begin(); it != kifGenerationFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}


bool KifXipGeneration::Setvalue(KifElement* a, KifElement* index, KifElement* dom) {
	if (a == this)
		return true;
	if (a == kifNULL || a == kifNOELEMENT) {
		value = NULL;
		return true;
	}

	if (a->type != kifXipGeneration) {
		if (value == NULL)
			return true;
		if (index != NULL && !index->isConst()) {
			string att = index->String();
			string val = a->String();
			val = s_to_lower(val);
			KifSetFeatures(kifcode, this, att, val);
		}
		return true;
	}

	KifXipGeneration* kx = (KifXipGeneration*)a;
	value = kx->value;
	return true;
}

KifElement* KifParseVarGeneration(KifCode* kifcode, x_node* xn, KifElement* base) {
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	KifXipGeneration* local = new KifXipGeneration(kifcode, variable, NULL);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

GlobalParseur* KifXipNode::Leparseur() {
	if (value == NULL || value->X == NULL)
		return NULL;
	return value->X->courant;
}

GlobalParseur* KifXipDependency::Leparseur() {
	if (value == NULL || value->Fx == NULL)
		return NULL;
	return value->Fx->courant;
}

vector<KifXipNode*> KifXipNode::nodes;
vector<KifXipDependency*> KifXipDependency::dependencies;

//Specific GARBAGE: we clean all KifElement elements pointing to a XIP temporary information
void kif_garbage_xip() {
	ThreadLock _lock(kifGarbage);
	size_t it;
	KifElement* ke;
	for (it = 0; it < KifXipNode::nodes.size(); it++) {
		ke = KifXipNode::nodes[it];
		if (ke != NULL)
			ke->CleanPointer();
	}
	for (it = 0; it < KifXipDependency::dependencies.size(); it++) {
		ke = KifXipDependency::dependencies[it];
		if (ke != NULL)
			ke->CleanPointer();
	}
	KifXipNode::nodes.clear();
	KifXipDependency::dependencies.clear();
}


void KifRecreatePredefined(KifCode* kcode, string name, int arity, Predefined func) {
	short idname = KifNewId(name);
	if (kifGlobals->kifBaseFunctions.find(idname) != kifGlobals->kifBaseFunctions.end()) {
		KifPredefined* pred = kifGlobals->kifBaseFunctions[idname];
		kifGlobals->kifBaseFunctions.erase(idname);
		delete pred;
	}

	kifGlobals->kifBaseFunctions[idname] = new KifPredefined(NULL, arity, func, idname);
	kifGlobals->kifBaseFunctions[idname]->kifcode = kcode;
}

static void MapFeatureSet(GlobalParseur* parseur, Vecteur* v, KifElement* map) {
	for (register int i = v->TailleVect - 1; i >= 0; i--) {
		STVecteur val = (STVecteur)v->vecteur[i];
		if (val != 0) {
			//On itere a l'envers
			for (register int j = gBitVect - 1; j >= 0; j--) {
				//Plus de bit a examiner on s'arrete
				if (val == 0)
					break;
				//le nombre est negatif, le jieme bit etait a un
				if (val < 0) {
					//nous calculons sa position absolue
					int pos = (i*gBitVect + j) * 2;
					map->storekey(parseur->TableDesTraits[pos], parseur->TableDesTraits[pos + 1]);
				}
				//nous decalons tous les bits d'un cran vers la gauche, pour les
				//amener les uns derriere les autres en position de bit de signe.
				val <<= 1;
			}
		}
	}
}

static void MapFeatureDependency(GlobalParseur* parseur, ResultatFonction* f, KifElement* map) {
	MapFeatureSet(parseur, f->traits->valeur, map);
}

static string KifSurface(Noeud* n) {
	string surface;
	n->Surface(surface);
	return surface;
}

static void MapSurface(GlobalParseur* parseur, Noeud* n, KifElement* map) {
	map->storekey(parseur->SURFACE, KifSurface(n));
}

static string KifLemma(Noeud* n) {
	string lemma;
	n->Lemme(lemma);
	return lemma;
}

static void MapLemma(GlobalParseur* parseur, Noeud* n, KifElement* map) {
	map->storekey(parseur->LEMME, KifLemma(n));
}

static void MapFeatureNode(GlobalParseur* parseur, Noeud* n, KifElement* map) {
	if (n->source == NULL)
		return;
	Vecteur* v = n->source->traitsactif->valeur;
	MapFeatureSet(parseur, v, map);
	MapSurface(parseur, n, map);
	MapLemma(parseur, n, map);
}


KifElement* ParseurPreParseCategories(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter is the parser object
	KifElement* kparser = callfunc->Evaluatethread(0, dom, idthread);
	if (kparser->type != kifParser)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifParser* kpars = (KifParser*)kparser;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifVector* vect = Selectvector(callfunc, contextualpattern);
	vector<string> names;
	XipGetListCategories(kpars->id, names);
	KifBaseString localstring(NULL, NULL);
	localstring.kifcode = callfunc->kifcode;
	for (int i = 0; i < names.size(); i++) {
		localstring.value = names[i];
		vect->Push(&localstring);
	}
	return vect;
}


KifElement* ParseurPreParseDependencies(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter is the parser object
	KifElement* kparser = callfunc->Evaluatethread(0, dom, idthread);
	if (kparser->type != kifParser)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifParser* kpars = (KifParser*)kparser;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifVector* vect = Selectvector(callfunc, contextualpattern);
	vector<string> names;
	XipGetListDependencies(kpars->id, names);
	KifBaseString localstring(NULL, NULL);
	localstring.kifcode = callfunc->kifcode;
	for (int i = 0; i < names.size(); i++) {
		localstring.value = names[i];
		vect->Push(&localstring);
	}
	return vect;
}

KifElement* ParseurPreParseFiles(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter is the parser object
	KifElement* kparser = callfunc->Evaluatethread(0, dom, idthread);
	if (kparser->type != kifParser)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifParser* kpars = (KifParser*)kparser;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	string grammar;
	vector<string> features;
	long debutf;
	vector<string> lexicons;
	long debutl;
	vector<string> rules;
	long debutr;
	vector<string> parameters;
	vector<string> scripts;
	vector<string> divers;
	XipGrammarPathnames(kpars->id, grammar, features, debutf, lexicons, debutl, rules, debutr, parameters, scripts, divers);
	KifElement* kmap = Selectamap(callfunc, contextualpattern);
	KifBaseString localstring(NULL, NULL, "");
	localstring.kifcode = callfunc->kifcode;
	int i;
	KifVector* kvect = new KifVector(callfunc->kifcode, NULL);
	for (i = 0; i < features.size(); i++) {
		localstring.value = features[i];
		kvect->Push(&localstring);
	}
	kmap->Push("features", kvect);
	kvect = new KifVector(callfunc->kifcode, NULL);
	for (i = 0; i < lexicons.size(); i++) {
		localstring.value = lexicons[i];
		kvect->Push(&localstring);
	}
	kmap->Push("lexicons", kvect);
	kvect = new KifVector(callfunc->kifcode, NULL);
	for (i = 0; i < rules.size(); i++) {
		localstring.value = rules[i];
		kvect->Push(&localstring);
	}
	kmap->Push("rules", kvect);
	kvect = new KifVector(callfunc->kifcode, NULL);
	for (i = 0; i < parameters.size(); i++) {
		localstring.value = parameters[i];
		kvect->Push(&localstring);
	}
	kmap->Push("addendum", kvect);
	kvect = new KifVector(callfunc->kifcode, NULL);
	for (i = 0; i < scripts.size(); i++) {
		localstring.value = scripts[i];
		kvect->Push(&localstring);
	}
	kmap->Push("scripts", kvect);
	kvect = new KifVector(callfunc->kifcode, NULL);
	for (i = 0; i < divers.size(); i++) {
		localstring.value = divers[i];
		kvect->Push(&localstring);
	}
	kmap->Push("miscellaneous", kvect);
	localstring.value = grammar;
	kmap->Push("grammar", &localstring);
	return kmap;
}


KifElement* ParseurPreParseReload(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter is the parser object
	KifElement* kparser = callfunc->Evaluatethread(0, dom, idthread);
	if (kparser->type != kifParser)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifParser* kpars = (KifParser*)kparser;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	try {
		XipReloadGrammar(kpars->id, 1);
	}
	catch (XIPError* message) {
		string error;
		error = "XIP(333): line: ";
		char ch[100];
		sprintf_s(ch, 10, "%d", message->line);
		error += ch;
		error += " in " + message->filename;
		error += " --> ";
		error += message->message;
		error += " in RELOAD";
		delete message;
		return callfunc->kifcode->Returnerror(error);
	}


	return kifNULL;
}
//--------------------------------------------------------------------------------
static void KifAddFlagDisplayMode(GlobalParseur* leParseur, XIPLONG mode) {
	leParseur->ParametresAffichage |= mode;

	if (leParseur->TestOptions(AFFICHAGE_MATHEMATIQUE))
		leParseur->mathematique = OUI;

	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;
}
//--------------------------------------------------------------------------------
static void KifRemoveFlagDisplayMode(GlobalParseur* leParseur, XIPLONG mode) {
	leParseur->ParametresAffichage &= ~mode;

	if (leParseur->TestOptions(AFFICHAGE_MATHEMATIQUE))
		leParseur->mathematique = OUI;

	leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
	leParseur->gAfficherResultatsDefini = leParseur->gAfficherResultats;
}
//--------------------------------------------------------------------------------
KifElement* ParseurPreParseSetOption(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter is the parser object
	KifElement* kparser = callfunc->Evaluatethread(0, dom, idthread);
	if (kparser->type != kifParser)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifParser* kpars = (KifParser*)kparser;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);

	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");
	kpars->toinitialize = false;
	XipSetDisplayMode(kpars->id, 0, 80);
	for (int i = 1; i < callfunc->Size(); i++) {
		KifElement* ke = callfunc->Evaluatethread(i, dom, idthread);
		int option = ke->Integer();
		switch (option) {
		case -40:
			xiptagger = OUI;
			break;
		case -30:
			statistique_interne = OUI;
			affiche_regle_appliquee = OUI;
			break;
		case -20:
			leParseur->gAfficherResultats = 0;
			leParseur->gAfficherResultatsDefini = 0;
			break;
		case -10:
			XipSetDependencyExtraction(kpars->id, 1);
			break;
		default:
			XIPLONG v = Un << option;
			KifAddFlagDisplayMode(leParseur, v);
		}
	}
	return kifTRUE;
}

KifElement* ParseurPreParseAddOption(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter is the parser object
	KifElement* kparser = callfunc->Evaluatethread(0, dom, idthread);
	if (kparser->type != kifParser)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifParser* kpars = (KifParser*)kparser;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);

	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	kpars->toinitialize = false;
	for (int i = 1; i < callfunc->Size(); i++) {
		KifElement* ke = callfunc->Evaluatethread(i, dom, idthread);
		int option = ke->Integer();
		switch (option) {
		case -40:
			xiptagger = OUI;
			break;
		case -30:
			statistique_interne = OUI;
			affiche_regle_appliquee = OUI;
			break;
		case -20:
			leParseur->gAfficherResultats = 0;
			leParseur->gAfficherResultatsDefini = 0;
			break;
		case -10:
			XipSetDependencyExtraction(kpars->id, 1);
			break;
		default:
			XIPLONG v = Un << option;
			KifAddFlagDisplayMode(leParseur, v);
		}
	}
	return kifTRUE;
}

KifElement* ParseurPreParseRemoveOption(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter is the parser object
	KifElement* kparser = callfunc->Evaluatethread(0, dom, idthread);
	if (kparser->type != kifParser)
		return callfunc->kifcode->Returnerror(kifErrorStrings[216]);
	KifParser* kpars = (KifParser*)kparser;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[216]);

	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	kpars->toinitialize = false;
	for (int i = 1; i < callfunc->Size(); i++) {
		KifElement* ke = callfunc->Evaluatethread(i, dom, idthread);
		int option = ke->Integer();
		switch (option) {
		case -40:
			xiptagger = NON;
			break;
		case -30:
			affiche_regle_appliquee = NON;
			break;
		case -20:
			leParseur->gAfficherResultats = 1;
			leParseur->gAfficherResultatsDefini = 1;
			break;
		case -10:
			XipSetDependencyExtraction(kpars->id, 0);
			break;
		default:
			XIPLONG v = Un << option;
			KifRemoveFlagDisplayMode(leParseur, v);
		}
	}
	return kifTRUE;
}

KifElement* ParseurPreSetrulelimit(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter is the parser object
	KifElement* kparser = callfunc->Evaluatethread(0, dom, idthread);
	if (kparser->type != kifParser)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifParser* kpars = (KifParser*)kparser;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	//Next parameter is the rule limit index
	int limit = callfunc->Evaluatethread(1, dom, idthread)->Integer();
	XipSetRuleLimit(kpars->id, limit);
	return kifTRUE;
}

KifElement* ParseurPreAppliedRules(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter is the parser object
	KifElement* kparser = callfunc->Evaluatethread(0, dom, idthread);
	if (kparser->type != kifParser)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifParser* kpars = (KifParser*)kparser;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	//Next parameter is the rule limit index
	vector<int> v;
	XipReturnRules(kpars->id, v);
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	for (int i = 0; i < v.size(); i++)
		kvect->storevalue((long)v[i]);
	return kvect;
}

KifElement* ParseurPreNbRules(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter is the parser object
	KifElement* kparser = callfunc->Evaluatethread(0, dom, idthread);
	if (kparser->type != kifParser)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifParser* kpars = (KifParser*)kparser;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	//Next parameter is the rule limit index
	return callfunc->kifcode->Provideinteger(XipNbRules(kpars->id));
}


KifElement* ParseurPreGetRuleBody(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter is the parser object
	KifElement* kparser = callfunc->Evaluatethread(0, dom, idthread);
	if (kparser->type != kifParser)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifParser* kpars = (KifParser*)kparser;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	//Next parameter is the rule limit index
	int rule = callfunc->Evaluatethread(1, dom, idthread)->Integer();
	ostringstream os;
	ostream* oldos = kifGlobals->os;
	XipDisplayActivatedRules(kpars->id, &os, rule, 5);
	kifGlobals->os = oldos;
	return callfunc->kifcode->Providestringraw(os.str());
}


KifElement* ParseurPreAddendum(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	ThreadLock _lock(kifParser);
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);

	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);
	KifElement* kfilename = callfunc->Evaluatethread(1, dom, idthread);
	string fname = kfilename->String();
	if (callfunc->Size() == 2) {
		try {
			XipParameterFile(kpars->id, STR(fname));
		}
		catch (XIPError* message) {
			string error("XIP(867): Erreur in ADDENDUM: ");
			error += message->message;
			delete message;
			return callfunc->kifcode->Returnerror(error);
		}
	}
	else {
		GlobalParseur* leParseur = XipGetParser(kpars->id);
		if (leParseur == NULL)
			return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

		bool add = callfunc->Evaluatethread(2, dom, idthread)->Boolean();
		try {
			leParseur->LireParametresEnChaine(fname, add);
		}
		catch (XIPError* message) {
			string error("XIP(867): Erreur in ADDENDUM: ");
			error += message->message;
			delete message;
			return callfunc->kifcode->Returnerror(error);
		}
	}
	return kifTRUE;
}

KifElement* ParseurPreGetSentence(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	ThreadLock _lock(kifParser);
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);

	KifXipNode* knode = (KifXipNode*)kfirst;
	if (knode->value == NULL)
		return callfunc->kifcode->Returnerror(kifErrorStrings[98]);

	GlobalParseur* leParseur = knode->Leparseur();
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	if (leParseur->donneesATraiter == NULL)
		return kifNULL;
	string buff = (char*)leParseur->donneesATraiter;
	buff = buff.substr(knode->value->gauche, knode->value->droite - knode->value->gauche);
	return callfunc->kifcode->Providestring(buff);
}

KifElement* ParseurPreInternalBuffer(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	ThreadLock _lock(kifParser);
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);

	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);
	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	if (leParseur->donneesATraiter == NULL)
		return kifNULL;
	return callfunc->kifcode->Providestringraw((char*)leParseur->donneesATraiter);
}

KifElement* ParseurPreSentenceNumber(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	ThreadLock _lock(kifParser);
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);

	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);
	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	return callfunc->kifcode->Provideinteger(leParseur->num_phrase);
}

KifElement* ParseurGetGrammar(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	ThreadLock _lock(kifParser);
	KifParser* kpars = (KifParser*)callfunc->Evaluatethread(0, dom, idthread);
	if (callfunc->Size() == 2)
		return kpars->Setgrammar(callfunc->Evaluatethread(1, dom, idthread));

	if (parseur_courant == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	kpars->id = parseur_courant->iParseurCourant;
	return kifTRUE;
}


KifElement* ParseurPreLemmaName(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);

	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);
	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	return callfunc->kifcode->Providestringraw(leParseur->LEMME);
}

KifElement* ParseurPreSurfaceName(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);

	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);
	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	return callfunc->kifcode->Providestringraw(leParseur->SURFACE);
}

KifElement* ParseurPreCatchXml(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	ThreadLock _lock(kifParser);
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);

	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);

	KifElement* fonc = callfunc->Evaluatethread(1, dom, idthread);
	KifFunction* kfonc = fonc->Functioncall(dom);
	if (kfonc == NULL)
		return callfunc->kifcode->Returnerror("XIP(173): Expecting a function");
	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	leParseur->interceptexml = kfonc;
	return kifTRUE;
}

KifElement* ParseurPreParse(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	ThreadLock _lock(kifParser);
	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);
	KifElement* ks = callfunc->Evaluatethread(1, dom, idthread);
	ostringstream os;
	ostream* oldos = kifGlobals->os;

	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	if (leParseur->kifloader != NULL)
		leParseur->kifloader->Setloader(callfunc->kifcode);

	leParseur->vkif_echange->kif = kifNULL;
	if (callfunc->Size() == 3) {
		KifElement* kinst = callfunc->parameters[2];
		if (kinst->type != kifInstance)
			callfunc->kifcode->Returnerror("XIP(877): Expecting a variable");
		leParseur->vkif_echange->kif = ((KifInstance*)kinst)->Thevariable(dom, kifGlobals->GetThreadid(), NULL);
	}

	//In the case of an external CALL within XIP as a library, we use a XipResult as well
	if (kpars->toinitialize) {
		if (BaseParseur != NULL)
			BaseParseur->InitialisationAvecParseur(kpars->id);
	}

	try {
		if (leParseur->AppelExterne != NULL) {
			XipResult* xip = XipCreateCurrentXipResult(kpars->id);
			XipParseStringAndOS(ks->String(), kpars->id, xip, &os, XML_NONE, 1);
		}
		else
			XipParseStringOS(ks->String(), kpars->id, &os);
	}
	catch (XIPError* message) {
		kifGlobals->os = oldos;
		string error;
		error = "(XIP(333): line: ";
		char l[100];
		sprintf(l, "%d", message->line);
		error += l;
		error += " in ";
		error += message->filename;
		error += "  --> ";
		error += message->message;
		error += " in GENERATEFROMSTRING";
		delete message;
		return callfunc->kifcode->Returnerror(error);
	}

	kifGlobals->os = oldos;

	ks = callfunc->kifcode->Providestringraw(os.str());
	if (contextualpattern == kifNULL)
		*kifGlobals->os << STR(os.str());
	return ks;
}

KifElement* ParseurPreDistances(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	ThreadLock _lock(kifParser);
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);

	GlobalParseur* parseur = XipGetParser(kpars->id);
	if (parseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	KifVector* kvect = Selectvector(callfunc, contextualpattern);

	if (parseur->TestOptions(AFFICHAGE_CALCUL_DISTANCE) == OUI) {
		hmap<int, hmap<int, int> >::iterator it;
		hmap<int, int>::iterator itsub;
		KifIntegerTreeMap* kdistances;
		KifTreeMapIntegerInteger* ksub;
		KifVector* kvectdist;
		for (int i = 0; i < parseur->distances.size(); i++) {
			kvectdist = new KifVector(callfunc->kifcode, NULL);
			kvect->Push(kvectdist);

			//----------------Distance 
			kdistances = new KifIntegerTreeMap(callfunc->kifcode, NULL);
			kvectdist->Push(kdistances);
			for (it = parseur->distances[i].begin(); it != parseur->distances[i].end(); it++) {
				ksub = new KifTreeMapIntegerInteger(callfunc->kifcode, NULL);
				kdistances->push(it->first, ksub);
				for (itsub = it->second.begin(); itsub != it->second.end(); itsub++)
					ksub->values[itsub->first] = itsub->second;
			}

			//---------------Tree Distance
			kdistances = new KifIntegerTreeMap(callfunc->kifcode, NULL);
			kvectdist->Push(kdistances);
			for (it = parseur->distancesarbre[i].begin(); it != parseur->distancesarbre[i].end(); it++) {
				ksub = new KifTreeMapIntegerInteger(callfunc->kifcode, NULL);
				kdistances->push(it->first, ksub);
				for (itsub = it->second.begin(); itsub != it->second.end(); itsub++)
					ksub->values[itsub->first] = itsub->second;
			}
		}
	}
	return kvect;
}

KifElement* ParseurPreDistance(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	ThreadLock _lock(kifParser);
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);

	GlobalParseur* parseur = XipGetParser(kpars->id);
	if (parseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	KifVector* kvect = Selectvector(callfunc, contextualpattern);

	KifTreeMapIntegerInteger* kdist;
	KifTreeMapIntegerInteger* kadist;

	KifIntegerTreeMap* kdistances = new KifIntegerTreeMap(callfunc->kifcode, NULL);
	KifIntegerTreeMap* kadistances = new KifIntegerTreeMap(callfunc->kifcode, NULL);
	kvect->Push(kdistances);
	kvect->Push(kadistances);
	int idn, idnn, i, j;
	for (i = 0; i < parseur->NoeudsLexicaux.dernier; i++) {
		idn = parseur->NoeudsLexicaux[i]->num_noeud;
		kdist = new KifTreeMapIntegerInteger(callfunc->kifcode, NULL);
		kadist = new KifTreeMapIntegerInteger(callfunc->kifcode, NULL);

		kdistances->push(idn, kdist);
		kadistances->push(idn, kadist);

		for (j = 0; j < parseur->NoeudsLexicaux.dernier; j++) {
			idnn = parseur->NoeudsLexicaux[j]->num_noeud;
			if (i == j) {
				kdist->values[idnn] = 0;
				kadist->values[idnn] = 0;
			}
			else {
				kdist->values[idnn] = parseur->NoeudsLexicaux[i]->distance(parseur->NoeudsLexicaux[j]);
				kadist->values[idnn] = parseur->NoeudsLexicaux[i]->distancearbre(parseur->NoeudsLexicaux[j]);
			}
		}
	}
	return kvect;
}

KifElement* ParseurPreLookup(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	if (ke->type == kifParser) {

		ThreadLock _lock(kifParser);

		KifParser* kpars = (KifParser*)ke;
		if (kpars->id == -1)
			return callfunc->kifcode->Returnerror(kifErrorStrings[181]);
		KifElement* ks = callfunc->Evaluatethread(1, dom, idthread);
		ostringstream os;
		ostream* oldos = kifGlobals->os;

		GlobalParseur* leParseur = XipGetParser(kpars->id);
		if (leParseur == NULL)
			return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

		if (leParseur->kifloader != NULL)
			leParseur->kifloader->Setloader(callfunc->kifcode);

		leParseur->vkif_echange->kif = kifNULL;
		bool feats = false;
		unsigned long long vaff = leParseur->AfficheFonction;
		if (callfunc->Size() == 3) {
			feats = callfunc->Evaluatethread(2, dom, idthread)->Boolean();
			if (feats)
				leParseur->AfficheFonction |= AFFICHE_TRAITS;
		}

		//In the case of an external CALL within XIP as a library, we use a XipResult as well
		if (kpars->toinitialize) {
			if (BaseParseur != NULL)
				BaseParseur->InitialisationAvecParseur(kpars->id);
		}
		char analyse = 0;
		bool returnjson = false;
		if (xiptagger == OUI) {
			leParseur->ParametresAffichage |= AFFICHAGE_COMPLET | AFFICHAGE_ENTREE;
			leParseur->ParametresAffichageDefini = leParseur->ParametresAffichage;
			analyse = 1;
			if (leParseur->TestOptions(AFFICHAGE_JSON)) {
				leParseur->vkif_echange->kif = Selectvector(callfunc, contextualpattern);
				returnjson = true;
			}
		}

		char vtag = leParseur->DESAMBIGUISATION;
		char vtagd = leParseur->DESAMBIGUISATIONDefini;
		char vtg = leParseur->TAGGING;

		leParseur->Desambiguisation(xiptagger);
		leParseur->Tagging(xiptagger);


		if (leParseur->AppelExterne != NULL) {
			XipResult* xip = XipCreateCurrentXipResult(kpars->id);
			XipParseStringAndOS(ks->String(), kpars->id, xip, &os, XML_NONE, analyse);
		}
		else
			XipParseStringOS(ks->String(), kpars->id, &os, XML_NONE, analyse);

		kifGlobals->os = oldos;
		leParseur->DESAMBIGUISATION = vtag;
		leParseur->DESAMBIGUISATIONDefini = vtagd;
		leParseur->TAGGING = vtg;
		leParseur->AfficheFonction = vaff;

		if (returnjson) {
			ks = leParseur->vkif_echange->kif;
			leParseur->vkif_echange->kif = kifNULL;
			return ks;
		}

		if (contextualpattern != kifNULL) {
			ks = callfunc->kifcode->Providestringraw(os.str());
			return ks;
		}
		else {
			*kifGlobals->os << STR(os.str());
			return kifTRUE;
		}
	}

	return PreLookup(contextualpattern, dom, callfunc, idthread);
}

KifElement* ParseurPreGenerateFromString(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	ThreadLock _lock(kifParser);
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);

	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);
	KifElement* ks = callfunc->Evaluatethread(1, dom, idthread);
	ostringstream os;
	ostream* oldos = kifGlobals->os;
	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	leParseur->vkif_echange->kif = kifNULL;
	if (callfunc->Size() == 3) {
		KifElement* kinst = callfunc->parameters[2];
		if (kinst->type != kifInstance)
			callfunc->kifcode->Returnerror("XIP(877): Expecting a variable");
		leParseur->vkif_echange->kif = ((KifInstance*)kinst)->Thevariable(dom, kifGlobals->GetThreadid(), NULL);
	}
	if (leParseur->kifloader != NULL)
		leParseur->kifloader->Setloader(callfunc->kifcode);
	//In the case of an external CALL within XIP as a library, we use a XipResult as well
	if (kpars->toinitialize) {
		if (BaseParseur != NULL)
			BaseParseur->InitialisationAvecParseur(kpars->id);
	}

	try {
		XipGenerateFromString(ks->String(), kpars->id, &os);
	}
	catch (XIPError* message) {
		kifGlobals->os = oldos;
		string error;
		error = "(XIP(333): line: ";
		char l[100];
		sprintf(l, "%d", message->line);
		error += l;
		error += " in ";
		error += message->filename;
		error += "  --> ";
		error += message->message;
		error += " in GENERATEFROMSTRING";
		delete message;
		return callfunc->kifcode->Returnerror(error);
	}


	kifGlobals->os = oldos;

	if (contextualpattern != kifNULL) {
		ks = callfunc->kifcode->Providestringraw(os.str());
		return ks;
	}
	else {
		*kifGlobals->os << STR(os.str());
		return kifTRUE;
	}
}

KifElement* ParseurPreGenerateFromFile(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	ThreadLock _lock(kifParser);
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);

	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);
	KifElement* ks = callfunc->Evaluatethread(1, dom, idthread);
	ostringstream os;
	ostream* oldos = kifGlobals->os;

	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	if (leParseur->kifloader != NULL)
		leParseur->kifloader->Setloader(callfunc->kifcode);
	leParseur->vkif_echange->kif = kifNULL;
	if (callfunc->Size() == 3) {
		KifElement* kinst = callfunc->parameters[2];
		if (kinst->type != kifInstance)
			callfunc->kifcode->Returnerror("XIP(877): Expecting a variable");
		leParseur->vkif_echange->kif = ((KifInstance*)kinst)->Thevariable(dom, kifGlobals->GetThreadid(), NULL);
	}
	//In the case of an external CALL within XIP as a library, we use a XipResult as well
	if (kpars->toinitialize) {
		if (BaseParseur != NULL)
			BaseParseur->InitialisationAvecParseur(kpars->id);
	}


	XipGenerateFromFile(ks->String(), kpars->id, &os);
	kifGlobals->os = oldos;

	if (contextualpattern != kifNULL) {
		ks = callfunc->kifcode->Providestringraw(os.str());
		return ks;
	}
	else {
		*kifGlobals->os << STR(os.str());
		return kifTRUE;
	}
}


bool KifCompareNoeuds(KifElement* e1, KifElement* e2) {
	Noeud* n1 = ((KifXipNode*)e1)->value;
	Noeud* n2 = ((KifXipNode*)e2)->value;
	if (n1->droite <= n2->gauche)
		return true;

	if (n1->gauche <= n2->gauche && n1->droite >= n2->droite && n1->hauteur < n2->hauteur)
		return true;

	return false;
}


KifElement* ParseurPreGenerateRule(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifRuleGeneration rg;
	string rule;

	KifElement* kparser = callfunc->Evaluatethread(0, dom, idthread);
	if (kparser->type != kifParser)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifParser* kpars = (KifParser*)kparser;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");


	string depname = callfunc->Evaluatethread(1, dom, idthread)->String();
	KifVector* nfocus = (KifVector*)callfunc->Evaluatethread(2, dom, idthread);
	KifVector* nodes = (KifVector*)callfunc->Evaluatethread(3, dom, idthread);
	KifVector* fnodes = (KifVector*)callfunc->Evaluatethread(4, dom, idthread);
	KifVector* deps = (KifVector*)callfunc->Evaluatethread(5, dom, idthread);
	KifVector* dnodes = (KifVector*)callfunc->Evaluatethread(6, dom, idthread);

	char ruletype = DEPENDENCYRULE;
	bool addquestionmark = false;
	if (callfunc->Size() >= 8) {
		addquestionmark = callfunc->Evaluatethread(7, dom, idthread)->Boolean();
		if (callfunc->Size() == 9)
			ruletype = (char)callfunc->Evaluatethread(8, dom, idthread)->Integer();
	}

	VECTA<KifSelectedNode*> vnodes;
	VECTA<KifSelectedDependency*> vdeps;
	VECTA<int> focus;
	map<int, int> featureorders;

	int i, fi;
	Noeud* n;
	Noeud* boundary = NULL;
	vector<string> feats;

	//On garde les focus dans l'ordre ou ils sont declares
	for (i = 0; i < nodes->values.size(); i++) {
		if (nodes->values[i]->type != kifXipNode)
			return callfunc->kifcode->Returnerror("XIP(043): Only node are accepted in the 2 parameter");
		if (fnodes->values.size() <= i || !fnodes->values[i]->aVectorContainer())
			return callfunc->kifcode->Returnerror("XIP(044): Only svector are accepted in the 3 parameter");
		n = ((KifXipNode*)nodes->values[i])->value;
		if (n == NULL)
			return callfunc->kifcode->Returnerror("XIP(047): Unknown node");
		featureorders[n->num_noeud] = i;
	}

	for (i = 0; i < nfocus->values.size(); i++) {
		n = ((KifXipNode*)nfocus->values[i])->value;
		focus.push_back(n->num_noeud);
	}

	//On trie les noeuds du plus a gauche au plus a droite...
	sort(nodes->values.begin(), nodes->values.end(), KifCompareNoeuds);

	for (i = 0; i < nodes->values.size(); i++) {
		n = ((KifXipNode*)nodes->values[i])->value;
		fi = featureorders[n->num_noeud];
		if (fnodes->values[fi]->type == kifVectorString)
			feats = ((KifVectorString*)fnodes->values[fi])->values;
		else {
			feats.clear();
			if (fnodes->values[fi]->Size() != 0) {
				for (int j = 0; j<fnodes->values[fi]->Size(); j++)
					feats.push_back(fnodes->values[fi]->ValueOnIndex(j)->String());
			}
		}

		if (boundary == NULL)
			boundary = n;
		else
		if (n->num_noeud>boundary->num_noeud)
			boundary = n;
		vnodes.push_back(new KifSelectedNode(n, n->num_noeud, feats));
	}

	if (boundary != NULL) {
		int mxnd = leParseur->dernierNumeroNoeud;
		rg.numvar.ajuste(mxnd + 1);
		for (i = 0; i <= mxnd; i++)
			rg.numvar.ajoute(-1);
	}

	rg.addquestionmark = addquestionmark;
	ResultatFonction* d;
	for (i = 0; i < deps->values.size(); i++) {
		if (deps->values[i]->type != kifXipDependency) {
			vdeps.clear();
			vnodes.clear();
			return callfunc->kifcode->Returnerror("XIP(045): Only dependency are accepted in the 4 parameter");
		}

		if (dnodes->values.size() <= i || !dnodes->values[i]->aVectorContainer()) {
			vdeps.clear();
			vnodes.clear();
			return callfunc->kifcode->Returnerror("XIP(044): Only svector are accepted in the 3 parameter");
		}

		if (dnodes->values[i]->type == kifVectorString)
			feats = ((KifVectorString*)dnodes->values[i])->values;
		else {
			feats.clear();
			if (dnodes->values[i]->Size() != 0) {
				for (int j = 0; j < dnodes->values[i]->Size(); j++)
					feats.push_back(dnodes->values[i]->ValueOnIndex(j)->String());
			}
		}

		d = ((KifXipDependency*)deps->values[i])->value;
		vdeps.push_back(new KifSelectedDependency(d, d->index, feats));
	}



	rg.kifrulegenRule(focus, rule, vnodes, vdeps, boundary, depname, focus.size(), ruletype);

	vnodes.clear();
	vdeps.clear();
	return callfunc->kifcode->Providestring(rule);
}


KifElement* ParseurPreParseFile(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	ThreadLock _lock(kifParser);
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);

	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);

	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	if (leParseur->kifloader != NULL)
		leParseur->kifloader->Setloader(callfunc->kifcode);

	if (kpars->toinitialize) {
		if (BaseParseur != NULL)
			BaseParseur->InitialisationAvecParseur(kpars->id);
	}

	ostream* of = NULL;
	KifElement* kof = NULL;
	string fname;
	KifElement* ks = callfunc->Evaluatethread(1, dom, idthread);
	KifElement* kinst = callfunc->parameters[2];
	if (kinst->type != kifInstance)
		callfunc->kifcode->Returnerror("XIP(877): Expecting a variable");
	leParseur->vkif_echange->kif = ((KifInstance*)kinst)->Thevariable(dom, kifGlobals->GetThreadid(), NULL);

	if (callfunc->Size() == 4) {
		kof = callfunc->Evaluatethread(3, dom, idthread);
		if (kof->type == kifFile)
			of = new ofstream(STR(kof->String()));
		else
		if (kof->type != kifString)
			kof = NULL;
	}

	if (of == NULL)
		of = new ostringstream;

	if (leParseur->AppelExterne != NULL) {
		ostream* os = leParseur->OS;
		leParseur->OS = of;
		XipResult* xip = XipCreateCurrentXipResult(kpars->id);
		int v = XipParseFile(ks->String(), kpars->id, xip, 1);
		leParseur->OS = os;
		if (kof != NULL) {
			if (kof->type == kifString)
				((KifString*)kof)->value = ((ostringstream*)of)->str();
			delete of;
			return callfunc->kifcode->Provideinteger(v);
		}
		kof = callfunc->kifcode->Providestringraw(((ostringstream*)of)->str());
		delete of;
		return kof;
	}

	int res = XipParseFileToOS(ks->String(), kpars->id, of, XML_NONE, 1);
	if (kof->type == kifString)
		((KifString*)kof)->value = ((ostringstream*)of)->str();
	delete of;
	if (res == 0)
		return kifTRUE;
	return kifFALSE;
}

KifElement* ParseurPreMatch(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifGraph* kg = (KifGraph*)callfunc->Evaluatethread(0, dom, idthread);
	KifElement* ks = callfunc->Evaluatethread(1, dom, idthread);
	return kg->Extract(ks);
}



KifElement* ParseurPreCreateDependency(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);

	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	if (leParseur->kifloader != NULL)
		leParseur->kifloader->Setloader(callfunc->kifcode);

	//First parameter is the name of our dependency
	KifElement* kname = callfunc->Evaluatethread(1, dom, idthread);
	Categorie* cfoncx;
	string name = kname->String();
	cfoncx = leParseur->RetrouveFonc(STR(name));
	if (cfoncx == NULL) {
		string mess = kifErrorStrings[51] + name;
		return callfunc->kifcode->Returnerror(mess);
	}

	//Second parameter is a feature map
	KifElement* kfeat = callfunc->Evaluatethread(2, dom, idthread);
	if (!kfeat->aMapContainer())
		return callfunc->kifcode->Returnerror(kifErrorStrings[52]);
	//Third parameter is a vector of nodes
	KifElement* knodes = callfunc->Evaluatethread(3, dom, idthread);
	if (knodes->type != kifVector)
		return callfunc->kifcode->Returnerror(kifErrorStrings[53]);
	KifVector* knodevect = (KifVector*)knodes;
	vector<KifElement*>::iterator itx;
	VECTA<Noeud*> lnoeuds;
	for (itx = knodevect->values.begin(); itx != knodevect->values.end(); itx++) {
		if ((*itx)->type != kifXipNode)
			return callfunc->kifcode->Returnerror(kifErrorStrings[54]);
		KifXipNode* node = (KifXipNode*)(*itx);
		if (node->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[55]);
		lnoeuds.ajoute(node->value);
	}
	Traits* trbase = new Traits(leParseur);
	KifIteration* itr = (KifIteration*)kfeat->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		string att = s_to_lower(itr->IteratorKeyString());
		string val = s_to_lower(itr->IteratorValueString());
		if (leParseur->XipFeatureTraits(trbase, STR(att), STR(val)) == 0) {
			itr->Release();
			delete trbase;
			string mess = kifErrorStrings[56] + att + ":" + val + " in CREATEDEPENDENCY";
			return callfunc->kifcode->Returnerror(mess);
		}
	}
	itr->Release();

	ResultatFonction* rf = leParseur->CreeNouvelleDependance(cfoncx, trbase, lnoeuds);
	if (rf != NULL) {
		KifXipDependency* kdep = new KifXipDependency(callfunc->kifcode, NULL, rf);
		return kdep;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[57]);
}

KifElement* KifSetFeatures(KifCode* kifcode, KifElement* kf, string& att, string& vval) {
	GlobalParseur* parseur = ((KifXipElement*)kf)->Leparseur();
	if (parseur == NULL) {
		if (kf->type == kifXipNode)
			return kifcode->Returnerror(kifErrorStrings[70]);
		return kifcode->Returnerror(kifErrorStrings[71]);
	}

	Traits trbase(parseur);

	if (att == parseur->LEMME || att == parseur->SURFACE) {
		if (kf->type == kifXipNode) {
			char* inter;
			KifXipNode* knode = (KifXipNode*)kf;
			if (knode->value != NULL) {
				if (att == parseur->LEMME) {
					int posmot = knode->value->source->choisie;
					inter = knode->value->source->voc->lemme[posmot];
					libere(inter);
					inter = (char*)DupliqueChaine(USTR(vval));
					knode->value->source->voc->lemme.affecte(posmot, inter);
					knode->value->fils->lemme = inter;
				}
				else {
					inter = knode->value->source->voc->surface;
					libere(inter);
					inter = (char*)DupliqueChaine(STR(vval));
					knode->value->source->voc->surface = inter;
					knode->value->fils->surface = inter;
				}
			}
			return kifTRUE;
		}
		return kifFALSE;
	}

	if (parseur->XipFeatureTraits(&trbase, STR(att), STR(vval)) == 0) {
		string mess = kifErrorStrings[60] + att + ":" + vval + " in SETFEATURE";
		return kifcode->Returnerror(mess);
	}
	if (kf->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kf;
		if (knode->value != NULL) {
			knode->value->source->traitsactif->ou(trbase);
			knode->value->source->traitsbase->ou(trbase);
		}
	}
	else {
		KifXipDependency* kdep = (KifXipDependency*)kf;
		if (kdep->value != NULL)
			kdep->value->traits->ou(trbase);
	}
	return kifTRUE;
}


KifElement* ParseurPreSetFeature(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->type != kifXipNode && kf->type != kifXipDependency && kf->type != kifXipGeneration)
		return callfunc->kifcode->Returnerror(kifErrorStrings[59]);

	string att;
	string vval;

	GlobalParseur* parseur = ((KifXipElement*)kf)->Leparseur();
	if (parseur == NULL) {
		if (kf->type == kifXipNode)
			return callfunc->kifcode->Returnerror(kifErrorStrings[70]);
		return callfunc->kifcode->Returnerror(kifErrorStrings[71]);
	}

	if (callfunc->Size() == 3) {
		KifElement* katt = callfunc->Evaluatethread(1, dom, idthread);
		KifElement* kval = callfunc->Evaluatethread(2, dom, idthread);
		att = s_to_lower(katt->String());
		vval = kval->String();
		if (att != parseur->LEMME && att != parseur->SURFACE)
			vval = s_to_lower(vval);
		return KifSetFeatures(callfunc->kifcode, kf, att, vval);
	}

	KifElement* kmap = callfunc->Evaluatethread(1, dom, idthread);
	if (!kmap->aMapContainer())
		return callfunc->kifcode->Returnerror(kifErrorStrings[69]);

	KifIteration* itr = (KifIteration*)kmap->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		att = s_to_lower(itr->IteratorKeyString());
		vval = itr->IteratorValueString();
		if (att != parseur->LEMME && att != parseur->SURFACE)
			vval = s_to_lower(vval);
		if (KifSetFeatures(callfunc->kifcode, kf, att, vval) != kifTRUE) {
			itr->Release();
			return kifFALSE;
		}
	}
	itr->Release();
	return  kifTRUE;
}


KifElement* ParseurPreRemoveFeature(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->type != kifXipNode && kf->type != kifXipDependency && kf->type != kifXipGeneration)
		return callfunc->kifcode->Returnerror(kifErrorStrings[62]);

	KifElement* katt = callfunc->Evaluatethread(1, dom, idthread);
	string att = s_to_lower(katt->String());
	GlobalParseur* parseur = ((KifXipElement*)kf)->Leparseur();
	if (parseur == NULL) {
		if (kf->type == kifXipNode)
			return callfunc->kifcode->Returnerror(kifErrorStrings[70]);
		return callfunc->kifcode->Returnerror(kifErrorStrings[71]);
	}

	if (att == parseur->LEMME || att == parseur->SURFACE)
		return kifFALSE;

	trait* vattribut = parseur->ArbTraits->recherche(STR(att));

	if (!vattribut) {
		string mess = kifErrorStrings[63] + att + " in REMOVEFEATURE";
		return callfunc->kifcode->Returnerror(mess);
	}

	if (kf->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kf;
		if (knode->value != NULL) {
			knode->value->source->traitsactif->valeur->etnon(vattribut->valeur);
			knode->value->source->traitsbase->valeur->etnon(vattribut->valeur);
			knode->value->source->traitsactif->filtre->etnon(vattribut->valeur);
			knode->value->source->traitsbase->filtre->etnon(vattribut->valeur);
		}
	}
	else {
		KifXipDependency* kdep = (KifXipDependency*)kf;
		if (kdep->value != NULL) {
			kdep->value->traits->valeur->etnon(vattribut->valeur);
			kdep->value->traits->filtre->etnon(vattribut->valeur);
		}
	}
	return  kifTRUE;
}


KifElement* ParseurPreFeature(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->type != kifXipNode && kf->type != kifXipDependency && kf->type != kifXipGeneration)
		return callfunc->kifcode->Returnerror(kifErrorStrings[65]);

	KifElement* katt = callfunc->Evaluatethread(1, dom, idthread);
	string att = s_to_lower(katt->String());
	string vval;
	if (callfunc->Size() == 3) {
		KifElement* kval = callfunc->Evaluatethread(2, dom, idthread);
		vval = s_to_lower(kval->String());
	}
	GlobalParseur* parseur = ((KifXipElement*)kf)->Leparseur();
	if (parseur == NULL) {
		if (kf->type == kifXipNode)
			return callfunc->kifcode->Returnerror(kifErrorStrings[70]);
		return callfunc->kifcode->Returnerror(kifErrorStrings[71]);
	}

	if (att == parseur->LEMME || att == parseur->SURFACE)
		return kifFALSE;

	trait* vattribut = parseur->ArbTraits->recherche(STR(att));
	string mess;
	if (!vattribut) {
		mess = kifErrorStrings[66] + att + " in FEATURE";
		return callfunc->kifcode->Returnerror(mess);
	}

	Traits* tr = NULL;
	if (kf->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kf;
		if (knode->value != NULL)
			tr = knode->value->source->traitsactif;
	}
	else {
		KifXipDependency* kdep = (KifXipDependency*)kf;
		if (kdep->value != NULL)
			tr = kdep->value->traits;
	}

	if (vval == "") {
		if (tr == NULL)
			return kifNULL;

		Vecteur v(parseur->iTailleVect);
		v.ou(vattribut->valeur);
		v.et(tr->valeur);
		char chaine[100];
		for (int i = 0; i < v.TailleVect; i++) {
			if (v.vecteur[i] != 0) {
				v.Valeur(parseur, chaine, i);
				return callfunc->kifcode->Providestringraw(chaine);
			}
		}
		return kifNULL;
	}

	trait* vvaleur = parseur->RetrouveValeurTrait(vattribut, STR(vval), NON);
	if (vvaleur == NULL) {
		mess = kifErrorStrings[67] + att + ":" + vval + " in FEATURE";
		return callfunc->kifcode->Returnerror(mess);
	}

	if (tr->valeur->contient(vvaleur->valeur) == 1)
		return kifTRUE;

	return  kifFALSE;
}

KifElement* ParseurPreGetRuleNumber(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type != kifXipNode && kfirst->type != kifXipDependency && kfirst->type != kifXipGeneration)
		return callfunc->kifcode->Returnerror(kifErrorStrings[59]);

	if (kfirst->type == kifXipNode) {
		KifXipNode* kxn = (KifXipNode*)kfirst;
		if (kxn->value == NULL)
			return kifMINUSONE;
		return kxn->kifcode->Provideinteger(kxn->value->numero);
	}

	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	KifXipDependency* kxd = (KifXipDependency*)kfirst;
	if (kxd->value == NULL)
		return kvect;
	for (int i = 0; i < kxd->value->numeros.size(); i++)
		kvect->storevalue((long)kxd->value->numeros[i]);
	return kvect;
}

KifElement* ParseurPreGetData(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kmap;
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type != kifXipNode && kfirst->type != kifXipDependency && kfirst->type != kifXipGeneration)
		return callfunc->kifcode->Returnerror(kifErrorStrings[59]);

	GlobalParseur* parseur = ((KifXipElement*)kfirst)->Leparseur();
	if (parseur == NULL) {
		if (kfirst->type == kifXipNode)
			return callfunc->kifcode->Returnerror(kifErrorStrings[70]);
		return callfunc->kifcode->Returnerror(kifErrorStrings[71]);
	}

	if (callfunc->Size() == 1) {
		kmap = Selectmapstring(callfunc, contextualpattern);
		if (kfirst->type == kifXipNode) {
			KifXipNode* knode = (KifXipNode*)kfirst;
			if (knode->value == NULL)
				return callfunc->kifcode->Returnerror(kifErrorStrings[70]);
			MapFeatureNode(parseur, knode->value, kmap);
			return kmap;
		}

		KifXipDependency* knode = (KifXipDependency*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[71]);
		MapFeatureDependency(parseur, knode->value, kmap);
		return kmap;
	}

	kmap = callfunc->Evaluatethread(1, dom, idthread);
	if (!kmap->aMapContainer())
		return callfunc->kifcode->Returnerror(kifErrorStrings[69]);

	KifIteration* itr = (KifIteration*)kmap->Newiterator(false);
	string att;
	string vval;
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		att = s_to_lower(itr->IteratorKeyString());
		vval = s_to_lower(itr->IteratorValueString());
		if (KifSetFeatures(callfunc->kifcode, kfirst, att, vval) != kifTRUE) {
			itr->Release();
			return kifFALSE;
		}
	}
	itr->Release();
	return kifTRUE;
}

KifElement* ParseurPreReadings(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* knode = callfunc->Evaluatethread(0, dom, idthread);

	if (knode->type != kifXipNode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[54]);

	Noeud* n = ((KifXipNode*)knode)->value;

	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	if (n == NULL)
		return kvect;
	ListeVoc* voc = n->source->voc;
	if (voc == NULL)
		return kvect;
	KifMapStringString* smap;
	for (int i = 0; i < voc->X.size(); i++) {
		smap = new KifMapStringString(callfunc->kifcode, NULL);
		smap->values["%POS"] = voc->X[i]->nom;
		if (voc->tag[i] != NULL)
			smap->values["%TAG"] = voc->tag[i];
		smap->values[n->X->courant->LEMME] = voc->lemme[i];
		smap->values[n->X->courant->SURFACE] = voc->surface;
		MapFeatureSet(n->X->courant, voc->tr[i]->valeur, smap);
		kvect->Push(smap);
	}
	return kvect;
}

KifElement* ParseurPreProperties(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);

	if (kfirst->type != kifXML)
		return callfunc->kifcode->Returnerror(kifErrorStrings[72]);

	ThreadLock _lock(kfirst->type);
	xmlNodePtr ndp = (xmlNodePtr)kfirst->Node();
	if (ndp == NULL)
		return kifNULL;

	KifElement* map = Selectmapstring(callfunc, contextualpattern);
	xmlAttrPtr propriete;
	string index;
	string valeur;
	if (ndp->properties != NULL) {
		propriete = ndp->properties;
		while (propriete != NULL) {
			index = (char*)propriete->name;
			valeur = (char*)propriete->children->content;
			map->storekey(index, valeur);
			propriete = propriete->next;
		}
	}
	return map;
}

KifElement* ParseurPreGetName(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	string valeur;
	if (kfirst->type == kifXML) {
		ThreadLock _lock(kfirst->type);
		xmlNodePtr ndp = (xmlNodePtr)kfirst->Node();
		if (ndp == NULL)
			return kifNULL;
		valeur = (char*)ndp->name;
		return callfunc->kifcode->Providestring(valeur);
	}

	if (kfirst->type == kifGraph) {
		KifGraph* kg = (KifGraph*)kfirst;
		if (kg->graph == NULL)
			return kifNULL;
		return callfunc->kifcode->Providestring(kg->graph->nom);
	}

	if (kfirst->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[73]);

		if (contextualpattern->type == kifVector) {
			ListeVoc* voc = knode->value->source->voc;
			KifVector* vect = Selectvector(callfunc, contextualpattern);
			KifBaseString localstring(NULL, NULL);
			localstring.kifcode = callfunc->kifcode;

			if (voc != NULL) {
				for (int i = 0; i < voc->X.dernier; i++) {
					localstring.value = voc->X[i]->nom;
					vect->Push(&localstring);
				}
			}
			else {
				localstring.value = knode->value->X->nom;
				vect->Push(&localstring);
			}
			return vect;
		}
		if (knode->value != NULL && knode->value->X != NULL)
			valeur = knode->value->X->nom;
		return callfunc->kifcode->Providestring(valeur);
	}

	if (kfirst->type == kifXipDependency || kfirst->type == kifXipGeneration) {
		KifXipDependency* knode = (KifXipDependency*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[74]);
		valeur = knode->value->Fx->nom;
		return callfunc->kifcode->Providestring(valeur);
	}
	return PreName(contextualpattern, dom, callfunc, idthread);
}


KifElement* ParseurPreDependencyStack(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kvect;
	bool ret = true;
	if (callfunc->Size() == 2) {
		ret = false;
		kvect = callfunc->Evaluatethread(1, dom, idthread);
		if (kvect->type != kifVector)
			return callfunc->kifcode->Returnerror(kifErrorStrings[77]);
	}
	else
		kvect = Selectvectorstring(callfunc, contextualpattern);

	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipDependency || kfirst->type == kifXipGeneration) {
		KifXipDependency* kdep = (KifXipDependency*)kfirst;
		if (kdep->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[78]);
		kvect->Clear();
		if (kvect->type == kifVectorString) {
			for (int i = 0; i < kdep->value->stack.dernier; i++)
				kvect->storevalue(kdep->value->stack[i]);
		}
		else {
			KifBaseString localstring(NULL, NULL);
			localstring.kifcode = callfunc->kifcode;

			for (int i = 0; i < kdep->value->stack.dernier; i++) {
				localstring.value = kdep->value->stack[i];
				kvect->Push(&localstring);
			}
		}
		if (!ret)
			return kifTRUE;
		return kvect;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[79]);
}

KifElement* ParseurPreDependencyParameters(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);

	KifElement* kvect = Selectvector(callfunc, contextualpattern);
	KifXipDependency* kdep = (KifXipDependency*)kfirst;
	if (kdep->value == NULL)
		return callfunc->kifcode->Returnerror(kifErrorStrings[82]);
	KifElement* knode;
	for (int i = 0; i < kdep->value->arg.dernier; i++) {
		knode = new KifXipNode(callfunc->kifcode, NULL, kdep->value->arg[i]);
		kvect->Push(knode);
	}
	return kvect;
}

//-------------------------------------------------------------------------------------------------------------------------------------------

KifElement* KifGraph::Copy(KifDomain* kp, KifElement* dom) {
	return new KifGraph(kifcode, kp, graph);
}


string KifGraph::String() {
	if (graph == NULL)
		return "";
	return graph->nom;
}

void KifGraph::Clean() {
	reference--;
	if (local == true && graph != NULL)
		delete graph;
	graph = NULL;
}

void KifGraph::Clear() {
	if (local == true && graph != NULL)
		delete graph;
	graph = NULL;
}

KifElement* KifGraph::same(KifElement* a) {
	if (graph == NULL || a->type != kifGraph)
		return kifFALSE;
	KifGraph* g = (KifGraph*)a;
	if (g->graph == NULL)
		return kifFALSE;

	if (graph->compare(g->graph) == 1)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifGraph::different(KifElement* a) {
	if (same(a) == kifTRUE)
		return kifFALSE;
	return kifTRUE;
}


bool KifGraph::Setvalue(KifElement* a, KifElement* index, KifElement* dom) {
	if (a->type != kifGraph)
		return true;
	KifGraph* g = (KifGraph*)a;
	graph = g->graph;
	return true;
}

KifElement* KifGraph::Project(KifElement* a) {
	if (graph == NULL || a->type != kifGraph)
		return kifFALSE;
	KifGraph* g = (KifGraph*)a;
	if (g->graph == NULL)
		return kifFALSE;
	if (graph->projection(g->graph) == OUI)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifGraph::Extract(KifElement* a) {
	if (graph == NULL || a->type != kifGraph)
		return kifFALSE;
	KifGraph* g = (KifGraph*)a;
	if (g->graph == NULL)
		return kifFALSE;
	VECTA<graphe*> gvect;
	graph->extraire(g->graph, gvect);
	KifVector* kvect = new KifVector(kifcode, NULL);
	for (int i = 0; i < gvect.dernier; i++) {
		g = new KifGraph(kifcode, NULL, gvect[i], true);
		kvect->Push(g);
	}
	return kvect;
}

KifElement* KifGraph::Remove(KifElement* a) {
	if (graph == NULL || a->type != kifGraph)
		return kifFALSE;
	KifGraph* g = (KifGraph*)a;
	if (g->graph == NULL)
		return kifFALSE;
	if (graph->detruitgraphe(g->graph) == OUI)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifGraph::Replace(KifElement* pat, KifElement* replacement) {
	if (graph == NULL || pat->type != kifGraph || replacement->type != kifGraph)
		return kifFALSE;
	KifGraph* gpat = (KifGraph*)pat;
	if (gpat->graph == NULL)
		return kifFALSE;
	KifGraph* grep = (KifGraph*)replacement;
	if (grep->graph == NULL)
		return kifFALSE;

	if (graph->remplacement(gpat->graph, grep->graph) == OUI)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifGraph::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	if (a->type != kifGraph || b->type != kifGraph)
		return kifFALSE;
	KifGraph* ga = (KifGraph*)a;
	KifGraph* gb = (KifGraph*)b;
	if (ga->graph == NULL || gb->graph == NULL)
		return kifFALSE;
	if (gb->graph->projection(ga->graph) == OUI)
		return kifTRUE;
	return kifFALSE;
}

//===================================================================



string KifXipNode::String() {
	if (value != NULL)
		return value->X->nom;
	return "";
}

string KifXipNode::JSonString() {
	if (value == NULL)
		return "";

	KifPrimeMap features(NULL, NULL);
	features.storekey("#ID", (long)value->num_noeud);
	features.storekey("#POS", value->X->nom);
	features.storekey("#LEFT", value->gauche);
	features.storekey("#RIGHT", value->droite);
	features.storekey("#LEFTCHAR", value->gauchecar);
	features.storekey("#RIGHTCHAR", value->droitecar);
	MapFeatureNode(value->X->courant, value, &features);
	return features.JSonString();
}

long KifXipNode::Integer() {
	if (value != NULL)
		return value->num_noeud;
	return -1;
}

double KifXipNode::Float() {
	if (value != NULL)
		return value->num_noeud;
	return -1;
}

string KifXipDependency::String() {
	if (value != NULL)
		return value->Fx->nom;
	return "";
}

string KifXipDependency::JSonString() {
	if (value == NULL)
		return "";

	KifPrimeMap features(NULL, NULL);
	features.storekey("#NAME", value->Fx->nom);
	MapFeatureDependency(value->Fx->courant, value, &features);

	Noeud* n;
	int i;

	KifPrimeMap* nodefeatures;
	KifVector kvect(NULL, NULL);
	features.Push("#ARGS", &kvect);

	for (i = 0; i < value->arg.dernier; i++) {
		n = value->arg[i];
		nodefeatures = new KifPrimeMap(NULL, NULL);

		nodefeatures->storekey("#ID", (long)n->num_noeud);
		nodefeatures->storekey("#POS", n->X->nom);
		nodefeatures->storekey("#LEFT", n->gauche);
		nodefeatures->storekey("#RIGHT", n->droite);
		nodefeatures->storekey("#LEFTCHAR", n->gauchecar);
		nodefeatures->storekey("#RIGHTCHAR", n->droitecar);
		MapFeatureNode(n->X->courant, n, nodefeatures);
		kvect.Push(nodefeatures);
	}


	string s = features.JSonString();
	for (i = 0; i < value->arg.dernier; i++)
		delete kvect.values[i];
	return s;
}

long KifXipDependency::Integer() {
	if (value != NULL)
		return value->index;
	return -1;
}

double KifXipDependency::Float() {
	if (value != NULL)
		return value->index;
	return -1;
}


Exported void KifMapPtr::Push(string& key, KifElement* val) {
	ThreadLock _lock(type);
	(*values)[key] = val->String();
}

Exported void KifMapPtr::Push(char* key, KifElement* val) {
	ThreadLock _lock(type);
	(*values)[key] = val->String();
}

string KifMapPtr::String() {
	string res;
	if (values == NULL)
		return "";
	map<string, string>::iterator it;
	res = "{";
	bool beg = true;
	string sx;
	for (it = values->begin(); it != values->end(); it++) {
		if (beg == false)
			res += ",";
		beg = false;
		sx = it->first;
		stringing(res, sx);
		res += ":";
		sx = it->second;
		stringing(res, sx);
	}
	res += "}";
	return res;
}

bool KifVectorNumberPtr::Boolean() {
	if (values == NULL || values->size() == 0)
		return false;
	return true;
}

bool KifVectorStringPtr::Boolean() {
	if (values == NULL || values->size() == 0)
		return false;
	return true;
}

string KifVectorNumberPtr::String() {
	string res;
	if (values == NULL)
		return res;
	int i;
	res = "[";
	char ch[20];
	bool beg = true;
	for (i = 0; i < values->size(); i++) {
		if (beg == false)
			res += ",";
		beg = false;
		double v = (*values)[i];
		sprintf_s(ch, 20, "%g", v);
		res += ch;
	}
	res += "]";
	return res;
}

string KifVectorStringPtr::String() {
	string res;
	if (values == NULL)
		return res;
	int i;
	res = "[";
	bool beg = true;
	for (i = 0; i < values->size(); i++) {
		string* s = (*values)[i];
		if (beg == false)
			res += ",";
		beg = false;
		if (s == NULL || *s == "")
			continue;
		stringing(res, *s);
	}
	res += "]";
	return res;
}


//===================================================================

KifElement* KifMapPtr::Newiterator(bool rev) {
	KifIteratorMapPtr* iter = new KifIteratorMapPtr(kifcode, NULL);
	iter->reverse = rev;
	iter->values = values;
	return iter;
}

KifElement* KifVectorStringPtr::Newiterator(bool rev) {
	KifIteratorVectorStringPtr* iter = new KifIteratorVectorStringPtr(kifcode, NULL);
	iter->reverse = rev;
	iter->values = values;
	return iter;
}

KifElement* KifVectorNumberPtr::Newiterator(bool rev) {
	KifIteratorVectorNumber* iter = new KifIteratorVectorNumber(kifcode, NULL);
	iter->reverse = rev;
	iter->values = values;
	return iter;
}


void KifVectorNumberPtr::Pop(KifElement* ke) {
	if (values != NULL) {
		int i = ke->Integer();
		if (i == -1)
			i = values->size() - 1;
		if (i >= values->size())
			return;

		values->retireElement(i);
	}
}

void KifVectorStringPtr::Pop(KifElement* ke) {
	if (values != NULL) {
		int i = ke->Integer();
		if (i == -1)
			i = values->size() - 1;
		if (i >= values->size())
			return;
		string* s = values->retireElement(i);
		if (s != NULL)
			delete s;
	}
}

void KifMapPtr::Pop(KifElement* ke) {
	if (values == NULL || values->size() == 0)
		return;

	string s = ke->String();
	map<string, string>::iterator itx = values->find(s);

	if (itx == values->end())
		return;

	values->erase(itx);
}




KifElement* KifVectorNumberPtr::Last() {
	if (values != NULL && values->size() != 0) {
		int i = values->size() - 1;
		double v = (*values)[i];
		return kifcode->Providefloat(v);
	}
	return kifNULL;
}

KifElement* KifVectorStringPtr::Last() {
	if (values != NULL && values->size() != 0) {
		int i = values->size() - 1;
		string* s = (*values)[i];
		if (s != NULL)
			return kifcode->Providestringraw(*s);
	}
	return kifNULL;
}

//--------------------------------------------------------------------------------------------------
//Rebuilding a tree out of a map
bool Addfeature(GlobalParseur* parseur, Liste* l, KifMap* m, bool terminal, string& error) {
	error = "";
	Noeud* n = &l->N;
	hmap<string, KifElement*>::iterator it;

	string att;
	string vval;

	for (it = m->values.begin(); it != m->values.end(); it++) {
		if (it->first[0] == '#')
			continue;
		att = it->first;

		if (att == parseur->LEMME || att == parseur->SURFACE)
			continue;

		vval = it->second->String();
		if (parseur->XipFeatureTraits(l->traitsactif, STR(att), STR(vval)) == 0) {
			error = vval;
			return false;
		}

		l->traitsbase->ou(l->traitsactif);
	}
	return true;
}

//we rebuild a tree out of a map, whose structure is: attribute:value, and a special node #CHILD and #POS, for the part of speech
Liste* TraverseTreeNoeud(GlobalParseur* parseur, KifMap* arbre, bool top, hmap<int, Liste*>& indexes) {
	if (arbre->values.find("#POS") == arbre->values.end()) {
		arbre->kifcode->Returnerror("XIP(500): Missing POS in the structure");
		return NULL;
	}
	string pos = arbre->values["#POS"]->String();
	int id = -100;
	if (arbre->values.find("#ID") != arbre->values.end())
		id = arbre->values["#ID"]->Integer();

	Categorie* c = parseur->ChercheCat(STR(pos));
	if (c == NULL) {
		string message = "XIP(501): Unknown POS in the structure:";
		message += pos;
		arbre->kifcode->Returnerror(message);
		return NULL;
	}

	bool terminal = false;
	if (arbre->values.find("#CHILD") == arbre->values.end())
		terminal = true;
	KifElement* e;
	//Eventually we have enough information for our structure
	Liste* root;

	Liste* fils;
	Liste* previous = NULL;
	int i = 0;
	if (!terminal) {
		root = parseur->TasDeListes->fournir(parseur, &c->X, NULL);
		if (id != -100)
			indexes[id] = root;
		if (top)
			parseur->glisteDesPhrases[0].push_back(root);
		if (!Addfeature(parseur, root, arbre, false, pos)) {
			string message = "XIP(503): Unknown feature: ";
			message += pos;
			arbre->kifcode->Returnerror(message);
			return NULL;
		}
		e = arbre->values["#CHILD"];
		if (e->type != kifVector) {
			arbre->kifcode->Returnerror("XIP(502): Wrong tree structure");
			return NULL;
		}
		KifVector* noeuds = (KifVector*)e;
		for (i = 0; i < noeuds->values.size(); i++) {
			e = noeuds->values[i];
			if (e->type != kifMap) {
				arbre->kifcode->Returnerror("XIP(502): Wrong tree structure");
				return NULL;
			}
			fils = TraverseTreeNoeud(parseur, (KifMap*)e, false, indexes);
			if (fils == NULL)
				return NULL;
			if (!i)
				root->N.fils = &fils->N;
			else
				previous->N.frere = &fils->N;
			fils->N.pere = &root->N;
			previous = fils;
			if (fils->N.fils == NULL)
				root->voc = fils->voc;
		}
		root->N.dernier = &fils->N;
		return root;
	}

	vocabulaire A;
	A.X = &c->X;
	if (arbre->values.find(parseur->LEMME) != arbre->values.end()) {
		pos = arbre->values[parseur->LEMME]->String();
		strcpy(A.lemme, STR(pos));
	}
	char majus = NON;
	if (arbre->values.find(parseur->SURFACE) != arbre->values.end()) {
		pos = arbre->values[parseur->SURFACE]->String();
		if (c_is_alpha(USTR(pos), i) == 2)
			majus = OUI;
		strcpy(A.surface, STR(pos));
		strcpy(A.surfacebrute, STR(pos));
	}

	root = parseur->TasDeListes->fournir(parseur, &c->X, NULL);
	if (id != -100)
		indexes[id] = root;

	if (!Addfeature(parseur, root, arbre, false, pos)) {
		string message = "XIP(503): Unknown feature: ";
		message += pos;
		arbre->kifcode->Returnerror(message);
		return NULL;
	}

	Traits* ts = root->traitsactif;
	A.initiaux = new Traits(ts);
	ListeVoc* voc = parseur->fournirListeVoc(A, majus);
	voc->tr.affecte(0, new Traits(ts));
	voc->nbX = 1;
	root->voc = voc;
	root->N.lemme = DupliqueChaine(A.lemme);
	root->N.surface = DupliqueChaine(A.surface);
	return root;
}

bool CreateDependance(GlobalParseur* parseur, KifVector* kvectdep, hmap<int, Liste*> indexes) {
	Categorie* cfoncx;
	KifElement* knodes;
	KifElement* kdep;
	for (int i = 0; i < kvectdep->values.size(); i++) {
		kdep = kvectdep->values[i];
		if (kdep->aMapContainer()) {
			kvectdep->kifcode->Returnerror(kifErrorStrings[52]);
			return false;
		}
		//TWO MAIN values: #NODES, #NAME, all the rest is features
		knodes = kdep->ValueOnKey("#NAME");
		if (knodes == kifNOELEMENT) {
			kvectdep->kifcode->Returnerror("XIP(506): Missing #NAME");
			return false;
		}
		string name = knodes->String();

		cfoncx = parseur->RetrouveFonc(STR(name));
		if (cfoncx == NULL) {
			string mess = kifErrorStrings[51] + name;
			return kvectdep->kifcode->Returnerror(mess);
		}

		knodes = kdep->ValueOnKey("#NODE");
		if (knodes == kifNOELEMENT) {
			kvectdep->kifcode->Returnerror("XIP(509): Missing #NODES");
			return false;
		}
		if (!knodes->aVectorContainer()) {
			kvectdep->kifcode->Returnerror("XIP(505): Expecting a vector of integers as #NODES");
			return false;
		}

		Traits* trbase = new Traits(parseur);
		KifIteration* itr = (KifIteration*)kvectdep->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			string att = s_to_lower(itr->IteratorKeyString());
			if (att == "#NAME" || att == "#NODES")
				continue;

			string val = s_to_lower(itr->IteratorValueString());
			if (parseur->XipFeatureTraits(trbase, STR(att), STR(val)) == 0) {
				itr->Release();
				delete trbase;
				string mess = kifErrorStrings[56] + att + ":" + val + " in CREATEDEPENDENCY";
				kvectdep->kifcode->Returnerror(mess);
				return false;
			}
		}
		itr->Release();
		itr = (KifIteration*)knodes->Newiterator(false);
		int nd;
		VECTA<Noeud*> thenodes;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			nd = itr->IteratorValueInteger();
			if (indexes.find(nd) == indexes.end()) {
				itr->Release();
				return kvectdep->kifcode->Returnerror(kifErrorStrings[54]);
			}
			thenodes.push_back(&indexes[nd]->N);
		}

		parseur->CreeNouvelleDependance(cfoncx, trbase, thenodes);
	}
	return true;
}


bool GlobalParseur::AnalyseFromKif(KifElement* ke, KifElement* kdep) {

	//On relit chaque phrase independemment
	//Tout d'abord l'arbre
	LListe& Phrase = glisteDesPhrases[0]; //les couches vont de 0 a MaxCouches...
	Phrase.dernier = 0;
	int i;
	hmap<int, Liste*> indexes;

	TasDeListes->Raz();
	Liste* l;
	if (ke->type == kifMap) {
		l = TraverseTreeNoeud(this, (KifMap*)ke, true, indexes);
		if (l == NULL)
			return false;
	}
	else {
		KifVector* vect = (KifVector*)ke;
		for (int i = 0; i < vect->values.size(); i++) {
			l = TraverseTreeNoeud(this, (KifMap*)ke, true, indexes);
			if (l == NULL)
				return false;
		}
	}

	MajINTERFONCTIONS();

	TVecteur laClef = 0;
	int nbn = 1;
	for (i = Phrase.dernier - 1; i >= 0; i--) {
		Liste* L = Phrase[i];
		laClef |= L->index;
		L->N.clef_suivant = laClef;
		L->N.nombre = nbn++;
	}

	if (kdep != NULL) {
		if (!CreateDependance(this, (KifVector*)kdep, indexes))
			return false;
	}

	//Application d'un jeu de regles special pour simplifier 
	//les categories multiples pour un mot donne
	if (TAGGING == OUI || DESAMBIGUISATION == OUI)
		AppliqueSimplification(Phrase, -1);

	if (Phrase.size() > 0)
		ActiveCouche(Phrase, 0);
	ExecuteExtraction();

	NettoiePhrase();
	NettoieVecteursTravail();
	return true;
}

KifElement* ParseurPreParseStructure(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	ThreadLock _lock(kifParser);
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);

	KifElement* kstruct = callfunc->Evaluatethread(1, dom, idthread);
	if (kstruct->type != kifMap && kstruct->type != kifVector)
		return callfunc->kifcode->Returnerror("XIP(508): Expect a container as input");


	KifParser* kpars = (KifParser*)ke;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[181]);
	KifElement* ks = callfunc->Evaluatethread(1, dom, idthread);
	ostringstream os;
	ostream* oldos = kifGlobals->os;

	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	if (leParseur->kifloader != NULL)
		leParseur->kifloader->Setloader(callfunc->kifcode);

	leParseur->vkif_echange->kif = kifNULL;
	if (callfunc->Size() == 3) {
		KifElement* kinst = callfunc->parameters[2];
		if (kinst->type != kifInstance)
			callfunc->kifcode->Returnerror("XIP(877): Expecting a variable");
		leParseur->vkif_echange->kif = ((KifInstance*)kinst)->Thevariable(dom, kifGlobals->GetThreadid(), NULL);
	}

	//In the case of an external CALL within XIP as a library, we use a XipResult as well
	if (kpars->toinitialize) {
		if (BaseParseur != NULL)
			BaseParseur->InitialisationAvecParseur(kpars->id);
	}

	if (callfunc->Size() == 3) {//we have dependencies: a vector of maps with: #NAME,features,#NODES:vector<int>
		ke = callfunc->Evaluatethread(2, dom, idthread);
		if (ke->type != kifVector)
			return callfunc->kifcode->Returnerror("XIP(509): Expect a vector as input");
	}
	else
		ke = NULL;

	bool ret = leParseur->AnalyseFromKif(kstruct, ke);
	if (!ret)
		return kifFALSE;

	kifGlobals->os = oldos;

	if (contextualpattern != kifNULL)
		return callfunc->kifcode->Providestringraw(os.str());
	else {
		*kifGlobals->os << STR(os.str());
		return kifTRUE;
	}
}


void KifXipNode::Constructionarbre(Noeud* n, KifVector* v) {
	KifPrimeMap* nodestructure = new KifPrimeMap(kifcode, NULL);

	//we populate it...
	nodestructure->storekey("#ID", (long)n->num_noeud);
	nodestructure->storekey("#POS", n->X->nom);
	nodestructure->storekey("#LEFT", n->gauche);
	nodestructure->storekey("#RIGHT", n->droite);
	nodestructure->storekey("#LEFTCHAR", n->gauchecar);
	nodestructure->storekey("#RIGHTCHAR", n->droitecar);
	MapFeatureNode(n->X->courant, n, nodestructure);

	v->Push(nodestructure);
	if (n->fils != NULL) {
		KifVector* subvect = new KifVector(kifcode, NULL);
		nodestructure->Push("#CHILDREN", subvect);
		Constructionarbre(n->fils, subvect);
	}
	if (n->frere != NULL)
		Constructionarbre(n->frere, v);
}

//We traverse the whole Noeud value and we create a vector at each level 
KifElement* KifXipNode::Vector(KifElement* dom, KifVector* v) {
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = new KifVector(kifcode, NULL);
	else
		kvect->Clear();

	if (value == NULL)
		return kvect;

	//we create our node structure, with all its information
	KifPrimeMap* nodestructure = new KifPrimeMap(kifcode, NULL);
	//we populate it...
	nodestructure->storekey("#ID", (long)value->num_noeud);
	nodestructure->storekey("#POS", value->X->nom);
	nodestructure->storekey("#LEFT", value->gauche);
	nodestructure->storekey("#RIGHT", value->droite);
	nodestructure->storekey("#LEFTCHAR", value->gauchecar);
	nodestructure->storekey("#RIGHTCHAR", value->droitecar);
	MapFeatureNode(value->X->courant, value, nodestructure);

	kvect->Push(nodestructure);
	//then we explore the subnodes...
	if (value->fils != NULL) {
		KifVector* subvect = new KifVector(kifcode, NULL);
		nodestructure->Push("#CHILDREN", subvect);
		Constructionarbre(value->fils, subvect);
	}
	return kvect;
}

KifElement* KifXipNode::Map(KifElement* dom, KifMap* m) {
	KifElement* nodestructure = m;
	if (nodestructure == NULL)
		nodestructure = new KifMap(kifcode, NULL);
	else
		nodestructure->Clear();

	if (value == NULL)
		return nodestructure;

	//we populate it...
	nodestructure->storekey("#ID", (long)value->num_noeud);
	nodestructure->storekey("#POS", value->X->nom);
	nodestructure->storekey("#LEFT", value->gauche);
	nodestructure->storekey("#RIGHT", value->droite);
	nodestructure->storekey("#LEFTCHAR", value->gauchecar);
	nodestructure->storekey("#RIGHTCHAR", value->droitecar);
	MapFeatureNode(value->X->courant, value, nodestructure);

	//then we explore the subnodes...
	if (value->fils != NULL) {
		KifVector* subvect = new KifVector(kifcode, NULL);
		nodestructure->Push("#CHILDREN", subvect);
		Constructionarbre(value->fils, subvect);
	}

	return nodestructure;
}


KifElement* KifXipDependency::Vector(KifElement* dom, KifVector* v) {
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = new KifVector(kifcode, NULL);
	else
		kvect->Clear();

	if (value == NULL)
		return kvect;

	//we create our node structure, with all its information
	KifPrimeMap* nodestructure = new KifPrimeMap(kifcode, NULL);
	//we populate it...
	nodestructure->storekey("#NAME", value->Fx->nom);
	MapFeatureDependency(value->Fx->courant, value, nodestructure);
	kvect->Push(nodestructure);
	KifXipNode nd(NULL, NULL, NULL);
	nd.kifcode = kifcode;

	KifVector* params = new KifVector(kifcode, NULL);
	nodestructure->Push("#ARGS", params);
	KifElement* m;
	for (int i = 0; i < value->arg.dernier; i++) {
		nd.value = value->arg[i];
		m = nd.Map(dom, NULL);
		params->Push(m);
	}
	return kvect;
}

KifElement* KifXipDependency::Map(KifElement* dom, KifMap* m) {
	KifElement* nodestructure = m;
	if (nodestructure == NULL)
		nodestructure = new KifMap(kifcode, NULL);
	else
		nodestructure->Clear();

	if (value == NULL)
		return nodestructure;

	//we populate it...
	nodestructure->storekey("#NAME", value->Fx->nom);
	MapFeatureDependency(value->Fx->courant, value, nodestructure);
	KifXipNode nd(NULL, NULL, NULL);
	nd.kifcode = kifcode;
	KifVector* params = new KifVector(kifcode, NULL);
	nodestructure->Push("#ARGS", params);
	KifElement* mnd;
	for (int i = 0; i < value->arg.dernier; i++) {
		nd.value = value->arg[i];
		mnd = nd.Map(dom, NULL);
		params->Push(mnd);
	}

	return nodestructure;
}

//--------------------------------------------------------------------------------------------------

bool KifXipNode::Setvalue(KifElement* a, KifElement* index, KifElement* dom) {
	if (a == this)
		return true;
	if (a == kifNULL || a == kifNOELEMENT) {
		value = NULL;
		return true;
	}
	if (a->type != kifXipNode) {
		if (value == NULL)
			return true;
		if (index != NULL && !index->isConst()) {
			GlobalParseur* parseur = Leparseur();
			string att = index->String();
			string val = a->String();
			if (att != parseur->LEMME && att != parseur->SURFACE)
				val = s_to_lower(val);
			KifSetFeatures(kifcode, this, att, val);
		}
		return true;
	}

	KifXipNode* kx = (KifXipNode*)a;
	value = kx->value;
	return true;
}

bool KifXipDependency::Setvalue(KifElement* a, KifElement* index, KifElement* dom) {
	if (a == this)
		return true;
	if (a == kifNULL || a == kifNOELEMENT) {
		value = NULL;
		return true;
	}

	if (a->type != kifXipDependency) {
		if (value == NULL)
			return true;
		if (index != NULL && !index->isConst()) {
			string att = index->String();
			string val = a->String();
			val = s_to_lower(val);
			KifSetFeatures(kifcode, this, att, val);
		}
		return true;
	}

	KifXipDependency* kx = (KifXipDependency*)a;
	value = kx->value;
	return true;
}

bool KifVectorNumberPtr::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	ThreadLock _lock(type);
	if (ke == this)
		return true;

	if (idx == NULL || idx == kifNULL) {
		if (ke->aVector()) {
			//We copy all values from ke to this
			Clear();
			for (int it = 0; it < ke->Size(); it++)
				values->push_back(ke->ValueOnIndex(it)->Float());
		}
		else
			values->push_back(ke->Float());
		return true;
	}

	int ikey = idx->Integer();
	if (ikey >= values->size()) {
		for (int nb = values->size(); nb < ikey; nb++)
			values->push_back(0);
		values->push_back(ke->Float());
	}
	else {
		values->affecte(ikey, ke->Float());
	}
	return true;
}

bool KifVectorStringPtr::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	ThreadLock _lock(type);
	if (ke == this)
		return true;

	vector<KifElement*>::iterator it;
	if (idx == NULL || idx == kifNULL) {
		if (ke->aVector()) {
			//We copy all values from ke to this
			Clear();
			for (int it = 0; it < ke->Size(); it++)
				values->push_back(new string(ke->ValueOnIndex(it)->String()));
		}
		else
			values->push_back(new string(ke->String()));
		return true;
	}

	int ikey = idx->Integer();
	if (ikey >= values->size()) {
		for (int nb = values->size(); nb < ikey; nb++)
			values->push_back(new string(""));
		values->push_back(new string(ke->String()));
	}
	else {
		if ((*values)[ikey] != NULL)
			delete (*values)[ikey];
		values->affecte(ikey, new string(ke->String()));
	}
	return true;
}


bool KifMapPtr::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	ThreadLock _lock(type);
	if (ke == this)
		return true;

	if (idx == NULL || idx == kifNULL) {
		if (ke->type != kifMap)
			return true;
		KifMap* kvect = (KifMap*)ke;
		hmap<string, KifElement*>::iterator it;
		//We copy all values from ke to this
		Clear();
		for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
			KifElement* x = it->second;
			string n = it->first;
			(*values)[n] = x->String();
		}
	}
	else {
		string key = idx->String();
		(*values)[key] = ke->String();
	}
	return true;
}


KifElement* KifGraph::Declaration(short n) {
	if (TestkifFunctions(n, kifGraphFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifParser::Declaration(short n) {
	if (TestkifFunctions(n, kifParserFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	if (id != -1) {
		GlobalParseur* parseur = XipSelectGrammar(id);
		if (parseur->kifloader != NULL)
			return parseur->kifloader->kstart->Declaration(n);
	}
	return NULL;
}

KifElement* KifVectorNumberPtr::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifVectorFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifVectorStringPtr::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifVectorFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}


KifElement* KifMapPtr::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}


KifElement* KifXipNode::Declaration(short n) {
	if (TestkifFunctions(n, kifNodeFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}


KifElement* KifXipDependency::Declaration(short n) {
	if (TestkifFunctions(n, kifDependencyFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

Exported KifElement* KifXipNode::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifInformationNode.find(s) == kifInformationNode.end())
		return kifNULL;
	return kifcode->Providestringraw(kifInformationNode[s]);
}

Exported KifElement* KifXipDependency::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifInformationDependency.find(s) == kifInformationDependency.end())
		return kifNULL;
	return kifcode->Providestringraw(kifInformationDependency[s]);
}

Exported KifElement* KifGraph::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifInformationGraph.find(s) == kifInformationGraph.end())
		return kifNULL;
	return kifcode->Providestringraw(kifInformationGraph[s]);
}

Exported KifElement* KifParser::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifInformationParser.find(s) == kifInformationParser.end())
		return kifNULL;
	return kifcode->Providestringraw(kifInformationParser[s]);
}
//-------------------------------------------------------------------------------
void KifVectorNumberPtr::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifVectorFunctions.begin(); it != kifGlobals->kifVectorFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

void KifVectorStringPtr::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifVectorFunctions.begin(); it != kifGlobals->kifVectorFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

void KifMapPtr::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}



void KifXipNode::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifNodeFunctions.begin(); it != kifNodeFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}




void KifGraph::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGraphFunctions.begin(); it != kifGraphFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

void KifXipDependency::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifDependencyFunctions.begin(); it != kifDependencyFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

void KifParser::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifParserFunctions.begin(); it != kifParserFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

//------------------------------------------------------------------------------
void KifVectorNumberPtr::Merge(KifElement* ke) {
	//Three cases:
	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values->push_back(itr->IteratorValue()->Float());
	itr->Release();
}

void KifVectorStringPtr::Merge(KifElement* ke) {
	//Three cases:
	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	string* n;
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())  {
		n = new string(itr->IteratorValue()->String());
		values->push_back(n);
	}
	itr->Release();
}

void KifMapPtr::Merge(KifElement* ke) {
	//Three cases:
	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		string n = itr->IteratorKeyString();
		if (values->find(n) == values->end())
			(*values)[n] = itr->IteratorValue()->String();
	}
	itr->Release();
}
//------------------------------------------------------------------------------
KifElement* KifVectorNumberPtr::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	if (b != this)
		return kifFALSE;

	int ix = 0;
	KifElement* ke;
	double v = a->Float();
	for (ix = 0; ix < values->size(); ix++) {
		if (values->vecteur[ix] == NULL)
			continue;
		if (values->vecteur[ix] == v) {
			if (idx == true) {
				ke = kifcode->Provideinteger(ix);
				return ke;
			}
			return kifTRUE;
		}
	}
	if (idx == true)
		return kifMINUSONE;
	return kifFALSE;
}

KifElement* KifVectorStringPtr::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	if (b != this)
		return kifFALSE;

	int ix = 0;
	string v = a->String();
	for (ix = 0; ix < values->size(); ix++) {
		if (values->vecteur[ix] == NULL)
			continue;
		if (*values->vecteur[ix] == v) {
			if (idx == true) {
				KifElement* ke = kifcode->Provideinteger(ix);
				return ke;
			}
			return kifTRUE;
		}
	}
	if (idx == true)
		return kifMINUSONE;
	return kifFALSE;
}


KifElement* KifMapPtr::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	if (b != this)
		return kifFALSE;

	map<string, string>::iterator itx;
	string v = a->String();
	for (itx = values->begin(); itx != values->end(); itx++) {
		if (itx->second == v) {
			if (idx == true)
				return kifcode->Providestringraw(itx->first);
			else
				return kifTRUE;
		}
	}
	if (idx == true)
		return kifNULL;
	return kifFALSE;
}


//===================================================================


KifElement* KifXipNode::less(KifElement* a) {
	if (a == kifNULL || a == kifNOELEMENT || value == NULL)
		return kifFALSE;
	if (a->type != kifXipNode)
		return kifFALSE;
	KifXipNode* kx = (KifXipNode*)a;
	if (kx->value == NULL)
		return kifFALSE;
	if (value->num_droite < kx->value->num_gauche)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifXipNode::more(KifElement* a) {
	if (a == kifNULL || a == kifNOELEMENT || value == NULL)
		return kifFALSE;
	if (a->type != kifXipNode)
		return kifFALSE;
	KifXipNode* kx = (KifXipNode*)a;
	if (kx->value == NULL)
		return kifFALSE;
	if (value->num_droite > kx->value->num_gauche)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifXipNode::lessequal(KifElement* a) {
	if (a == kifNULL || a == kifNOELEMENT || value == NULL)
		return kifFALSE;
	if (a->type != kifXipNode)
		return kifFALSE;
	KifXipNode* kx = (KifXipNode*)a;
	if (kx->value == NULL)
		return kifFALSE;
	if (value->num_droite <= kx->value->num_gauche)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifXipNode::moreequal(KifElement* a) {
	if (a == kifNULL || a == kifNOELEMENT || value == NULL)
		return kifFALSE;
	if (a->type != kifXipNode)
		return kifFALSE;
	KifXipNode* kx = (KifXipNode*)a;
	if (kx->value == NULL)
		return kifFALSE;
	if (value->num_droite >= kx->value->num_gauche)
		return kifTRUE;
	return kifFALSE;
}


KifElement* KifGraph::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction* callfunc) {
	if (ke != NULL && ke->type == kifIndex) {
		string message = "XIP(083): No index for 'graph'";
		return kifcode->Returnerror(message);
	}
	return this;
}

KifElement* KifXipNode::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	if (idx != NULL && !idx->isConst()) {
		if (value == NULL)
			return kifNOELEMENT;
		GlobalParseur* parseur = Leparseur();
		if (parseur == NULL)
			return kifcode->Returnerror(kifErrorStrings[70]);
		string att = idx->String();

		att = s_to_lower(att);

		if (att == parseur->LEMME)
			return kifcode->Providestringraw(KifLemma(value));

		if (att == parseur->SURFACE)
			return kifcode->Providestringraw(KifSurface(value));

		trait* vattribut = parseur->ArbTraits->recherche(STR(att));
		if (!vattribut) {
			string mess = kifErrorStrings[66] + att + " in FEATURE";
			return kifcode->Returnerror(mess);
		}

		Traits* tr = NULL;
		tr = value->source->traitsactif;
		if (tr == NULL)
			return kifNOELEMENT;

		Vecteur v(parseur->iTailleVect);
		v.ou(vattribut->valeur);
		v.et(tr->valeur);
		char chaine[256];
		for (int i = 0; i < v.TailleVect; i++) {
			if (v.vecteur[i] != 0) {
				v.Valeur(parseur, chaine, i);
				return kifcode->Providestringraw(chaine);
			}
		}
		return kifNOELEMENT;
	}
	return this;
}

KifElement* KifXipDependency::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	if (idx != NULL && !idx->isConst()) {
		if (value == NULL)
			return kifNOELEMENT;
		GlobalParseur* parseur = Leparseur();
		if (parseur == NULL)
			return kifcode->Returnerror(kifErrorStrings[70]);
		string att = idx->String();

		att = s_to_lower(att);

		trait* vattribut = parseur->ArbTraits->recherche(STR(att));
		if (!vattribut) {
			string mess = kifErrorStrings[66] + att + " in FEATURE";
			return kifcode->Returnerror(mess);
		}

		Traits* tr = NULL;
		tr = value->traits;

		if (tr == NULL)
			return kifNOELEMENT;

		Vecteur v(parseur->iTailleVect);
		v.ou(vattribut->valeur);
		v.et(tr->valeur);
		char chaine[100];
		for (int i = 0; i < v.TailleVect; i++) {
			if (v.vecteur[i] != 0) {
				v.Valeur(parseur, chaine, i);
				return kifcode->Providestringraw(chaine);
			}
		}
		return kifNOELEMENT;
	}
	return this;
}


KifElement* KifMapPtr::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	if (idx == NULL || idx == kifNULL) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->type == kifVector) {
			kv = Selectvector(this, contextualpattern);
			map<string, string>::iterator it;
			for (it = values->begin(); it != values->end(); it++) {
				KifElement* x = kifcode->Providestringraw(it->first);
				kv->Push(x);
			}
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat) {
			kv = kifcode->Provideinteger(values->size());
			return kv;
		}
		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;
		if (kind->interval == true)
			return kifNULL;
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNULL;
	string skey = key->String();
	KifElement* kval = new KifString(kifcode, this, (*values)[skey]);
	if (kval == NULL)
		return kifNULL;
	return kval;
}

KifElement* KifVectorNumberPtr::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	if (idx == NULL || idx == kifNULL) {

		//In this case, we copy the elements from the vector to the map, using the position as index
		if (contextualpattern->type == kifMap) {
			KifMap* map = Selectmap(this, contextualpattern);
			for (size_t i = 0; i < values->size(); i++) {
				sprintf_s(kifGlobals->buffer, 20, "%d", i);
				KifElement* ke = kifcode->Providefloat((*values)[i]);
				ke->Setreference();
				map->values[kifGlobals->buffer] = ke;
			}
			return map;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat) {
			KifElement* kv = kifcode->Provideinteger(values->size());
			return kv;
		}
		return this;
	}

	KifElement* key;
	KifElement* keyright = NULL;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (kind->interval == true)
			keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNULL;

	int ikey = key->Integer();
	if (ikey < 0)
		ikey = values->size() + ikey;

	if (ikey < 0 || ikey >= values->size())
		return kifNULL;

	if (keyright == NULL)
		return kifcode->Providefloat((*values)[ikey]);

	int iright = keyright->Integer();
	if (iright < 0 || keyright == kifNULL)
		iright = values->size() + iright;
	else
	if (iright >= values->size())
		iright = values->size();
	//In this case, we must create a new vector
	KifVector* kvect = Selectvector(this, contextualpattern);
	KifBaseFloat kfloat(NULL, NULL);
	kfloat.kifcode = kifcode;
	for (int i = ikey; i < iright; i++) {
		kfloat.value = (*values)[i];
		kvect->Push(&kfloat);
	}
	return kvect;
}

KifElement* KifVectorStringPtr::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	if (idx == NULL || idx == kifNULL) {

		//In this case, we copy the elements from the vector to the map, using the position as index
		if (contextualpattern->type == kifMap) {
			KifMap* map = Selectmap(this, contextualpattern);
			for (size_t i = 0; i < values->size(); i++) {
				sprintf_s(kifGlobals->buffer, 20, "%d", i);
				KifElement* ke = new KifString(kifcode, this, *(*values)[i]);
				ke->Setreference();
				map->values[kifGlobals->buffer] = ke;
			}
			return map;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat) {
			KifElement* kv = kifcode->Provideinteger(values->size());
			return kv;
		}
		return this;
	}

	KifElement* key;
	KifElement* keyright = NULL;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (kind->interval == true)
			keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNULL;

	int ikey = key->Integer();
	if (ikey < 0)
		ikey = values->size() + ikey;

	if (ikey < 0 || ikey >= values->size())
		return kifNULL;

	if (keyright == NULL) {
		if ((*values)[ikey] == NULL)
			return kifNULL;
		return kifcode->Providestringraw(*(*values)[ikey]);
	}

	int iright = keyright->Integer();
	if (iright < 0 || keyright == kifNULL)
		iright = values->size() + iright;
	else
	if (iright >= values->size())
		iright = values->size();
	//In this case, we must create a new vector
	KifVector* kvect = Selectvector(this, contextualpattern);
	KifBaseString localstring(NULL, NULL);
	localstring.kifcode = kifcode;
	for (int i = ikey; i < iright; i++) {
		if ((*values)[ikey] != NULL) {
			localstring.value = *(*values)[i];
			kvect->Push(&localstring);
		}
	}
	return kvect;
}

KifElement* KifParser::Setgrammar(KifElement* k) {
	int i;
	if (k->type == kifXipNode) {
		KifXipNode* kxipnode = (KifXipNode*)k;
		if (kxipnode->value == NULL)
			return kifFALSE;
		i = kxipnode->value->X->courant->iParseurCourant;
	}
	else {
		if (k->type == kifXipDependency) {
			KifXipDependency* kxipdep = (KifXipDependency*)k;
			if (kxipdep->value == NULL)
				return kifFALSE;
			i = kxipdep->value->Fx->courant->iParseurCourant;
		}
		else
			i = k->Integer();
	}

	if (i == id || i < 0)
		return kifFALSE;

	id = i;
	name = k->String();
	todelete = false;
	if (!XipSetKifCode(id, kifcode))
		return kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	return kifTRUE;
}


KifParser::~KifParser() {
	if (id != -1 && todelete)
		XipFreeGrammar(id);
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

static bool CompileKifCode(KifCode* localkifcode, string& code, string& msgerreur) {
	if (code == "") {
		msgerreur = "Code Empty";
		return false;
	}

	msgerreur = "";
	int firstinstruction = 0;

	int id = kifGlobals->GetThreadid();

	int igarbage = kifGlobals->garbage.dernier;
	int icursor = kifGlobals->cursor;

	kifGlobals->THR[id]->threadstacks.clear();

	localkifcode->kstart->adding = true;
	localkifcode->addtolocal = true;
	kifGlobals->PushStack(localkifcode->kstart);
	code = conversion_latin_to_utf8(code);

	kifGlobals->executionbreak = 0;
	kifGlobals->bnfkif->baseline = 1;
	localkifcode->Evaluate(localkifcode->kstart, code, false);
	kifGlobals->PopStack();
	localkifcode->addtolocal = false;

	//then from igarbage up to the end, we reference our elements
	if (Errorid(id) == true) {
		msgerreur = Errorstring();
		kifGlobals->THR[id]->cleanerror();
		kifGlobals->garbage.dernier = igarbage;
		kifGlobals->cursor = icursor;
		return false;
	}

	for (int i = 0; i < localkifcode->locals.size(); i++) {
		int j = localkifcode->locals[i];
		if (kifGlobals->garbage[j] != NULL) {
			kifGlobals->garbage[j]->Setpopped();
			kifGlobals->garbage[j]->Setreference();
		}
	}

	localkifcode->kstart->Run(firstinstruction);
	if (Errorid(id) == true) {
		msgerreur = Errorstring();
		kifGlobals->THR[id]->cleanerror();
		kifGlobals->garbage.dernier = igarbage;
		kifGlobals->cursor = icursor;
		return false;
	}

	return true;

}

Exportation int KifExecuteCodeProgram(string code, string arguments) {

	//On initialise une valeur par defaut du declencheur 
	//de garbage, pour que celui-ci puisse s'effectuer
	//a chaque fin d'execution
	if (BaseParseur == NULL) {
		if (parseur_courant != NULL)
			BaseParseur = parseur_courant;
		else {
			BaseParseur = new GlobalParseur("", "");
			BaseParseur->InitialisationParseurMin(1, 0, 0, 1, 0, 0, 1, 32768, 0, 2147483682, 0, 1);
			gVecteurParseurs.affecte(-1, BaseParseur);
		}
	}

	KifCode* kifcode;

	kifcode = new KifCodeParseur(NULL, kif_declencheur, NULL, BaseParseur->OS);

	if (arguments.size() != 0) {
		vector<string> args;
		v_split(arguments, " ", args);
		kifcode->ClearArguments();
		kifcode->AddArguments(args);
	}

	kifGlobals->threadmode = true;
	string msgerreur;

	if (!CompileKifCode(kifcode, code, msgerreur)) {
		kif_erreur(msgerreur, kifGlobals->debuginfocurrent->currentline);
		return -1;
	}

	return kifcode->idkifmain;
}

Exportation int KifExecuteProgram(string fname, string arguments, int map) {

	//On initialise une valeur par defaut du declencheur 
	//de garbage, pour que celui-ci puisse s'effectuer
	//a chaque fin d'execution
	if (BaseParseur == NULL) {
		if (parseur_courant != NULL)
			BaseParseur = parseur_courant;
		else {
			BaseParseur = new GlobalParseur("", "");
			BaseParseur->InitialisationParseurMin(1, 0, 0, 1, 0, 0, 1, 32768, 0, 2147483682, 0, 1);
			gVecteurParseurs.affecte(-1, BaseParseur);
		}
	}

	KifCode* kifcode;

	if (BaseParseur->kifloader == NULL) {
		//BaseParseur->iParseurCourant=gVecteurParseurs.dernier;
		BaseParseur->kifloader = new KifCodeParseur(NULL, kif_declencheur, (void*)BaseParseur, BaseParseur->OS);
		kifcode = BaseParseur->kifloader;
	}
	else {
		if (map == 1)
			kifcode = BaseParseur->kifloader;
		else
			kifcode = new KifCodeParseur(NULL, kif_declencheur, NULL, BaseParseur->OS);
	}

	if (arguments.size() != 0) {
		vector<string> args;
		v_split(arguments, " ", args);
		kifcode->ClearArguments();
		kifcode->AddArguments(args);
	}

	kifGlobals->threadmode = true;

	fname = Getfilename(fname);
	ifstream* fkif = new ifstream(STR(fname), modeOuverture);
	if (fkif->fail()) {
		string value = "Unknown file:";
		value += fname;
		kif_erreur(value, -1);
		return -1;
	}

	try {
		kifGlobals->bnfkif->baseline = 1;
		kifcode->Loadfile(fkif, fname, 0);
	}
	catch (KifRaiseError* m) {
		kif_erreur(m->message, m->left);
		delete m;
		return -1;
	}

	kifcode->Run();
	if (Error() == true) {
		kif_erreur(Errorstring(), -1);
		return -1;
	}

	return kifcode->idkifmain;
}

Exportation int KifExecuteCodeProgramParseur(int ipar, string code, string arguments) {

	GlobalParseur* parseur = XipGetParser(ipar);
	if (parseur == NULL) {
		string value = "Unknown parser handler";
		kif_erreur(value, -1);
		return -1;
	}

	if (parseur->kifloader == NULL) {
		parseur->kifloader = new KifCodeParseur(NULL, kif_declencheur, (void*)parseur, parseur->OS);
		if (arguments.size() != 0) {
			vector<string> args;
			v_split(arguments, " ", args);
			parseur->kifloader->AddArguments(args);
		}
	}

	KifCode* kifcode = parseur->kifloader;
	string msgerreur;

	if (!CompileKifCode(kifcode, code, msgerreur)) {
		kif_erreur(msgerreur, kifGlobals->debuginfocurrent->currentline);
		return -1;
	}

	return 0;
}

Exportation int KifExecuteProgramParseur(int ipar, string fname, string arguments) {

	GlobalParseur* parseur = XipGetParser(ipar);
	if (parseur == NULL) {
		string value = "Unknown parser handler";
		kif_erreur(value, -1);
		return -1;
	}

	if (parseur->kifloader == NULL) {
		parseur->kifloader = new KifCodeParseur(NULL, kif_declencheur, (void*)parseur, parseur->OS);
		if (arguments.size() != 0) {
			vector<string> args;
			v_split(arguments, " ", args);
			parseur->kifloader->AddArguments(args);
		}
	}

	KifCode* kifcode = parseur->kifloader;
	fname = Getfilename(fname);
	ifstream* fkif = new ifstream(STR(fname), modeOuverture);
	if (fkif->fail()) {
		string value = "Unknown file:";
		value += fname;
		kif_erreur(value, -1);
		return -1;
	}

	try {
		kifGlobals->bnfkif->baseline = 1;
		kifcode->Loadfile(fkif, fname, 0);
	}
	catch (KifRaiseError* m) {
		kif_erreur(m->message, m->left);
		delete m;
		return -1;
	}

	kifcode->Run();
	if (Error() == true) {
		kif_erreur(Errorstring(), -1);
		return -1;
	}

	return 0;
}

Exported int XipKifLoad(vector<string>& paths, vector<string>& arguments, bool debugkif) {

	//On initialise une valeur par defaut du declencheur 
	//de garbage, pour que celui-ci puisse s'effectuer
	//a chaque fin d'execution
	if (BaseParseur == NULL) {
		BaseParseur = new GlobalParseur("", "");
		BaseParseur->InitialisationParseurMin(1, 0, 0, 1, 0, 0, 1, 32768, 0, 2147483682, 0, 1);
		gVecteurParseurs.affecte(-1, BaseParseur);
	}

	if (BaseParseur->kifloader == NULL) {
		//BaseParseur->iParseurCourant=gVecteurParseurs.dernier;
		BaseParseur->kifloader = new KifCodeParseur(NULL, kif_declencheur, (void*)BaseParseur, BaseParseur->OS);
		BaseParseur->kifloader->AddArguments(arguments);
	}

	KiFSetdebug(debugkif);

	KifCode* kifcode = BaseParseur->kifloader;
	for (int i = 0; i < paths.size(); i++) {
		string fname = Getfilename(paths[i]);
		ifstream* fkif = new ifstream(STR(fname), modeOuverture);
		try {
			kifGlobals->bnfkif->baseline = 1;
			*flot_erreur << "Loading kif file: " << STR(fname) << endl;
			kifcode->Loadfile(fkif, fname, 0);
		}
		catch (KifRaiseError* m) {
			*flot_erreur << STR(m->message) << " at " << m->left << endl;
			delete m;
			return -1;
		}
	}
	kifcode->Run();
	if (Error() == true) {
		*flot_erreur << STR(Errorstring()) << endl;
		return -1;
	}
	return BaseParseur->iParseurCourant;
}


Exported string XipKifExecute(string name, vector<string>& parameters, bool& erreur) {
	string valeur;
	erreur = false;
	if (BaseParseur == NULL || BaseParseur->kifloader == NULL) {
		valeur = "KIF(888): Error no code loaded";
		kif_erreur(valeur, -1);
		erreur = true;
		return valeur;
	}

	KifCode* kifcode = BaseParseur->kifloader;


	KifElement* proc_ke = kifcode->Findraw(name);
	if (proc_ke == NULL || proc_ke->type != kifFunction) {
		erreur = true;
		valeur = "KIF(889): Unknown function=";
		valeur += name;
		kif_erreur(valeur, -1);
		return valeur;
	}

	vector<KifElement*> params;
	int i;
	for (i = 0; i < parameters.size(); i++) {
		params.push_back(kifcode->Providestring(parameters[i]));
		params.back()->Setreference();
	}

	KifFunction* proc = (KifFunction*)proc_ke;
	KifElement* resultat = kifcode->Execute(proc, params);

	for (i = 0; i < parameters.size(); i++)
		params[i]->Resetreference();

	if (Error()) {
		erreur = true;
		valeur = Errorstring();
		kif_erreur(valeur, -1);
		return valeur;
	}

	valeur = resultat->String();
	resultat->Release();
	return valeur;
}

Exported string XipKifExecuteIndex(int kifhandler, string name, vector<string>& parameters, bool& erreur) {
	string valeur;
	erreur = false;

	KifCode* kifcode = kifGlobals->kifCodeVector[kifhandler];
	if (kifcode == NULL) {
		valeur = "KIF(888): Error no code loaded";
		kif_erreur(valeur, -1);
		erreur = true;
		return valeur;
	}

	KifElement* proc_ke = kifcode->Findraw(name);
	if (proc_ke == NULL || proc_ke->type != kifFunction) {
		erreur = true;
		valeur = "KIF(889): Unknown function=";
		valeur += name;
		kif_erreur(valeur, -1);
		return valeur;
	}

	vector<KifElement*> params;
	int i;
	for (i = 0; i < parameters.size(); i++) {
		params.push_back(kifcode->Providestring(parameters[i]));
		params.back()->Setreference();
	}

	KifFunction* proc = (KifFunction*)proc_ke;
	KifElement* resultat = kifcode->Execute(proc, params);

	for (i = 0; i < parameters.size(); i++)
		params[i]->Resetreference();

	if (Error()) {
		erreur = true;
		valeur = Errorstring();
		kif_erreur(valeur, -1);
		return valeur;
	}

	valeur = resultat->String();
	resultat->Release();
	return valeur;
}

KifElement* XipKifExecuteIndexObject(KifCode* kifcode, string name, vector<KifElement*>& params, bool& erreur) {
	string valeur;
	erreur = false;

	KifElement* proc_ke = kifcode->Findraw(name);
	if (proc_ke == NULL || proc_ke->type != kifFunction) {
		erreur = true;
		valeur = "KIF(889): Unknown function=";
		valeur += name;
		return kifcode->Returnerror(valeur);
	}

	int i;
	for (i = 0; i < params.size(); i++)
		params.back()->Setreference();

	KifFunction* proc = (KifFunction*)proc_ke;
	KifElement* resultat = kifcode->Execute(proc, params);

	for (i = 0; i < params.size(); i++)
		params[i]->Resetreference();

	if (Error())
		erreur = true;

	return resultat;
}

KifElement* XipKifExecuteObject(string name, vector<KifElement*>& parameters, bool& erreur) {
	string valeur;
	erreur = false;
	if (BaseParseur == NULL || BaseParseur->kifloader == NULL) {
		valeur = "KIF(888): Error no code loaded";
		kif_erreur(valeur, -1);
		erreur = true;
		return BaseParseur->kifloader->Providestring(valeur);
	}

	KifCode* kifcode = BaseParseur->kifloader;


	KifElement* proc_ke = kifcode->Findraw(name);
	if (proc_ke == NULL || proc_ke->type != kifFunction) {
		erreur = true;
		valeur = "KIF(889): Unknown function=";
		valeur += name;
		kif_erreur(valeur, -1);
		return BaseParseur->kifloader->Providestring(valeur);
	}

	int i;
	for (i = 0; i < parameters.size(); i++)
		parameters.back()->Setreference();

	KifFunction* proc = (KifFunction*)proc_ke;
	KifElement* resultat = kifcode->Execute(proc, parameters);

	for (i = 0; i < parameters.size(); i++)
		parameters[i]->Resetreference();

	if (Error()) {
		erreur = true;
		valeur = Errorstring();
		kif_erreur(valeur, -1);
		return BaseParseur->kifloader->Providestring(valeur);
	}
	return resultat;
}

Exported string XipKifExecuteParseur(int ipar, string name, vector<string>& parameters, bool& erreur) {
	string valeur;
	erreur = false;
	GlobalParseur* parseur = XipGetParser(ipar);
	if (parseur == NULL || parseur->kifloader == NULL) {
		valeur = "KIF(888): Error no code loaded";
		kif_erreur(valeur, -1);
		erreur = true;
		return valeur;
	}

	KifCode* kifcode = parseur->kifloader;


	KifElement* proc_ke = kifcode->Findraw(name);
	if (proc_ke == NULL || proc_ke->type != kifFunction) {
		erreur = true;
		valeur = "KIF(889): Unknown function=";
		valeur += name;
		kif_erreur(valeur, -1);
		return valeur;
	}

	vector<KifElement*> params;
	int i;
	for (i = 0; i < parameters.size(); i++) {
		params.push_back(kifcode->Providestring(parameters[i]));
		params.back()->Setreference();
	}

	KifFunction* proc = (KifFunction*)proc_ke;
	KifElement* resultat = kifcode->Execute(proc, params);

	for (i = 0; i < parameters.size(); i++)
		params[i]->Resetreference();

	if (Error()) {
		erreur = true;
		valeur = Errorstring();
		kif_erreur(valeur, -1);
		return valeur;
	}

	valeur = resultat->String();
	resultat->Release();
	return valeur;
}

KifElement* ParseurPreXipNumber(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return kifMINUSONE;
		return callfunc->kifcode->Provideinteger(knode->value->num_noeud);
	}
	return kifMINUSONE;
}


KifElement* ParseurPreOffset(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[98]);
		if (callfunc->Size() == 3) {
			KifElement* kleft = callfunc->Evaluatethread(1, dom, idthread);
			KifElement* kright = callfunc->Evaluatethread(2, dom, idthread);
			kleft->Setvalue(callfunc->kifcode->Provideinteger(knode->value->gauche));
			kright->Setvalue(callfunc->kifcode->Provideinteger(knode->value->droite));
			return kifTRUE;
		}

		KifElement* kvect = Selectavector(callfunc, contextualpattern);
		kvect->Push(callfunc->kifcode->Provideinteger(knode->value->gauche));
		kvect->Push(callfunc->kifcode->Provideinteger(knode->value->droite));
		return kvect;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[99]);
}

KifElement* ParseurPreOffsetChar(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[101]);
		if (callfunc->Size() == 3) {
			KifElement* kleft = callfunc->Evaluatethread(1, dom, idthread);
			KifElement* kright = callfunc->Evaluatethread(2, dom, idthread);
			kleft->Setvalue(callfunc->kifcode->Provideinteger(knode->value->gauchecar));
			kright->Setvalue(callfunc->kifcode->Provideinteger(knode->value->droitecar));
			return kifTRUE;
		}

		KifElement* kvect = Selectavector(callfunc, contextualpattern);
		kvect->Push(callfunc->kifcode->Provideinteger(knode->value->gauchecar));
		kvect->Push(callfunc->kifcode->Provideinteger(knode->value->droitecar));
		return kvect;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[102]);
}

KifElement* ParseurPreTokenOffset(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[104]);
		if (callfunc->Size() == 3) {
			KifElement* kleft = callfunc->Evaluatethread(1, dom, idthread);
			KifElement* kright = callfunc->Evaluatethread(2, dom, idthread);
			kleft->Setvalue(callfunc->kifcode->Provideinteger(knode->value->num_gauche));
			kright->Setvalue(callfunc->kifcode->Provideinteger(knode->value->num_droite));
			return kifTRUE;
		}

		KifElement* kvect = Selectavector(callfunc, contextualpattern);
		kvect->Push(callfunc->kifcode->Provideinteger(knode->value->num_gauche));
		kvect->Push(callfunc->kifcode->Provideinteger(knode->value->num_droite));
		return kvect;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[105]);
}


KifElement* ParseurPreLeftOffset(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[107]);
		return callfunc->kifcode->Provideinteger(knode->value->gauche);
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[108]);
}

KifElement* ParseurPreRightOffset(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[110]);
		return callfunc->kifcode->Provideinteger(knode->value->droite);
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[111]);
}

KifElement* ParseurPreLeftOffsetChar(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[113]);
		return callfunc->kifcode->Provideinteger(knode->value->gauchecar);
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[114]);
}

KifElement* ParseurPreRightOffsetChar(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[116]);
		return callfunc->kifcode->Provideinteger(knode->value->droitecar);
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[117]);
}

KifElement* ParseurPreLeftTokenOffset(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[119]);
		return callfunc->kifcode->Provideinteger(knode->value->num_gauche);
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[120]);
}

KifElement* ParseurPreRightTokenOffset(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {
		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[122]);
		return callfunc->kifcode->Provideinteger(knode->value->num_droite);
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[123]);
}

KifElement* ParseurPreParseFeatures(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is the parser object
	KifElement* kparser = callfunc->Evaluatethread(0, dom, idthread);
	if (kparser->type != kifParser)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);
	KifParser* kpars = (KifParser*)kparser;
	if (kpars->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[215]);

	GlobalParseur* leParseur = XipGetParser(kpars->id);
	if (leParseur == NULL)
		return callfunc->kifcode->Returnerror("XIP(555): Unknown parser. Either ID does not match an existing parser or parser is not loaded.");

	string val;
	//We get the first element, it should be a map...
	KifElement* kvectinit = callfunc->Evaluatethread(1, dom, idthread);
	if (!kvectinit->aVectorContainer()) {
		val = "XIP(856): First parameter should be a VECTOR";
		return callfunc->kifcode->Returnerror(val);
	}

	KifElement* kmapres = callfunc->Evaluatethread(2, dom, idthread);
	if (!kmapres->aMapContainer()) {
		val = "XIP(856): Second parameter should be a MAP";
		return callfunc->kifcode->Returnerror(val);
	}

	kmapres->Clear();
	vector<string> feats;
	KifIteration* itr = (KifIteration*)kvectinit->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		feats.push_back(s_to_lower(itr->IteratorValueString()));
	itr->Release();

	Traits* tlocal = new Traits(leParseur);
	leParseur->FeatureAnalysis(feats, val, tlocal);
	MapFeatureSet(leParseur, tlocal->valeur, kmapres);
	delete tlocal;
	return callfunc->kifcode->Providestring(val);
}

KifElement* ParseurPreNodeDistance(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type != kifXipNode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[124]);

	KifXipNode* knode = (KifXipNode*)kfirst;
	if (knode->value == NULL)
		return callfunc->kifcode->Returnerror(kifErrorStrings[124]);

	kfirst = callfunc->Evaluatethread(1, dom, idthread);
	if (kfirst->type != kifXipNode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[124]);

	KifXipNode* knodecomp = (KifXipNode*)kfirst;
	if (knodecomp->value == NULL)
		return callfunc->kifcode->Returnerror(kifErrorStrings[124]);

	return callfunc->kifcode->Provideinteger(knode->value->distance(knodecomp->value));
}

KifElement* ParseurPreNodeTreeDistance(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type != kifXipNode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[124]);

	KifXipNode* knode = (KifXipNode*)kfirst;
	if (knode->value == NULL)
		return callfunc->kifcode->Returnerror(kifErrorStrings[124]);

	kfirst = callfunc->Evaluatethread(1, dom, idthread);
	if (kfirst->type != kifXipNode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[124]);

	KifXipNode* knodecomp = (KifXipNode*)kfirst;
	if (knodecomp->value == NULL)
		return callfunc->kifcode->Returnerror(kifErrorStrings[124]);

	return callfunc->kifcode->Provideinteger(knode->value->distancearbre(knodecomp->value));
}


KifElement* ParseurPreLemma(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type != kifXipNode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[124]);

	KifXipNode* knode = (KifXipNode*)kfirst;
	if (knode->value == NULL)
		return callfunc->kifcode->Returnerror(kifErrorStrings[124]);

	if (callfunc->Size() == 1) {
		KifBaseString localstring(NULL, NULL);
		localstring.kifcode = callfunc->kifcode;
		if (contextualpattern->aVectorContainer()) {
			ListeVoc* voc = knode->value->source->voc;
			KifElement* kvect = Selectavector(callfunc, contextualpattern);
			if (voc != NULL) {
				for (int i = 0; i < voc->lemme.dernier; i++)
					kvect->storevalue(voc->lemme[i]);
			}
			else
				kvect->storevalue(KifLemma(knode->value));
			return kvect;
		}
		return callfunc->kifcode->Providestringraw(KifLemma(knode->value));
	}
	char* inter;
	if (callfunc->Size() == 2) {

		KifElement* kstr = callfunc->Evaluatethread(1, dom, idthread);
		if (kstr->type == kifBoolean) {
			//we defined how the value should be returned
			string retval = KifLemma(knode->value);
			if (kstr->Boolean()) {
				wstring wretval;
				s_utf8_to_unicode(wretval, STR(retval));
				return callfunc->kifcode->Provideustring(wretval);
			}
			return callfunc->kifcode->Providestringraw(retval);
		}

		if (knode->value->source == NULL || knode->value->source->voc == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[124]);

		string vval = kstr->String();
		int posmot = knode->value->source->choisie;
		inter = knode->value->source->voc->lemme[posmot];
		libere(inter);
		inter = (char*)DupliqueChaine(USTR(vval));
		knode->value->source->voc->lemme.affecte(posmot, inter);
		knode->value->fils->lemme = inter;
		return kifTRUE;
	}

	//Then we will add a new reading to the structure...
	//then, we have three parameters...
	GlobalParseur* leParseur = knode->value->X->courant;
	string lemma = callfunc->Evaluatethread(1, dom, idthread)->String();
	string pos = callfunc->Evaluatethread(2, dom, idthread)->String();

	Categorie* C = leParseur->ChercheCat(STR(pos));
	if (C == NULL) {
		string mess = "XIP(801): Unknown POS in LEMMA=";
		mess += pos;
		return callfunc->kifcode->Returnerror(mess);
	}

	KifElement* ksmap = callfunc->Evaluatethread(3, dom, idthread);
	if (!ksmap->aMapContainer()) {
		lemma = "XIP(233): Expecting a MAP in a LEMMA function";
		return callfunc->kifcode->Returnerror(lemma);
	}

	Traits trbase(leParseur);
	KifIteration* itr = (KifIteration*)ksmap->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		string att = s_to_lower(itr->IteratorKeyString());
		string val = s_to_lower(itr->IteratorValueString());
		if (leParseur->XipFeatureTraits(&trbase, STR(att), STR(val)) == 0) {
			itr->Release();
			string mess = kifErrorStrings[56] + att + ":" + val + " in LEMMA";
			return callfunc->kifcode->Returnerror(mess);
		}
	}
	itr->Release();

	//First we clean the lemma structure...
	leParseur->ListeTagsRAZ(STR(lemma), &inter);
	//Second we add our information within the structure...
	//First the surface brute form
	leParseur->ListeTagsAjouter(knode->value->source->voc->surfacebrute);
	//Second the surface form
	leParseur->ListeTagsAjouter(knode->value->source->voc->surface);
	//Third the lemma form
	leParseur->ListeTagsAjouter(inter);
	leParseur->Xproposecategorie = C;
	leParseur->FusionneForme(knode->value->source->voc, inter);
	leParseur->Xproposecategorie = NULL;
	knode->value->source->voc->tr.vecteur[knode->value->source->voc->tr.dernier - 1]->ou(&trbase);

	//inter=(char*)DupliqueChaine(USTR(lemma));
	//knode->value->source->voc->lemme.ajoute(inter);
	//knode->value->source->voc->X.ajoute(X);
	//knode->value->source->voc->tr.ajoute(trbase);
	//if (knode->value->source->voc->tag.size()>0) {
	//	inter=(char*)DupliqueChaine(knode->value->source->voc->tag[0]);
	//	knode->value->source->voc->tag.ajoute(inter);
	//}
	//knode->value->source->voc->garder.ajoute(knode->value->source->voc->garder[0]);
	//knode->value->source->voc->nbX++;
	return kifTRUE;
}

KifElement* ParseurPreNodeInfos(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type != kifXipNode)
		return callfunc->kifcode->Returnerror("XIP(885): Unknwon node");

	KifXipNode* knode = (KifXipNode*)kfirst;
	if (knode->value == NULL)
		return callfunc->kifcode->Returnerror("XIP(885): Unknwon node");

	if (callfunc->Size() == 1)
		return callfunc->kifcode->Providestring(knode->value->infos);

	KifElement* kstr = callfunc->Evaluatethread(1, dom, idthread);
	knode->value->infos = kstr->String();
	return kifTRUE;
}

KifElement* ParseurPreSurface(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type != kifXipNode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[127]);

	KifXipNode* knode = (KifXipNode*)kfirst;
	if (knode->value == NULL)
		return callfunc->kifcode->Returnerror(kifErrorStrings[127]);

	if (callfunc->Size() == 1)
		return callfunc->kifcode->Providestringraw(KifSurface(knode->value));

	KifElement* kstr = callfunc->Evaluatethread(1, dom, idthread);
	if (kstr->type == kifBoolean) {
		//we defined how the value should be returned
		string retval = KifSurface(knode->value);
		if (kstr->Boolean()) {
			wstring wretval;
			s_utf8_to_unicode(wretval, STR(retval));
			return callfunc->kifcode->Provideustring(wretval);
		}
		return callfunc->kifcode->Providestringraw(retval);
	}

	if (knode->value->source == NULL || knode->value->source->voc == NULL)
		return callfunc->kifcode->Returnerror(kifErrorStrings[127]);

	string vval = kstr->String();
	char* inter;
	inter = knode->value->source->voc->surface;
	libere(inter);
	inter = (char*)DupliqueChaine(STR(vval));
	knode->value->source->voc->surface = inter;
	knode->value->fils->surface = inter;
	return kifTRUE;
}

KifElement* ParseurPreAnalyzeLexiconEntry(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* ksmap = Selectmapstring(callfunc, contextualpattern);

	return kifTRUE;
}


KifElement* ParseurPreNext(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter should be a node
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {

		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[138]);

		Noeud* n = knode->value->frere;
		if (n == NULL) {
			if (knode->value->pere == NULL) {
				//C'est un noeud au-dessus de la melee...
				GlobalParseur* parseur = knode->Leparseur();
				if (parseur->phraseCourante == NULL)
					return kifNULL;

				LListe& ph = *parseur->phraseCourante;
				if (knode->value->source == NULL)
					return kifNULL;

				int i = knode->value->source->iphrase + 1;
				if (ph[i] == NULL)
					return kifNULL;
				n = &ph[i]->N;
			}
			else
				return kifNULL;
		}

		if (contextualpattern == kfirst)
			knode->value = n;
		else
			knode = new KifXipNode(callfunc->kifcode, dom, n);
		return knode;
	}

	if (kfirst->type == kifXipGeneration) {
		KifXipGeneration* kgen = (KifXipGeneration*)kfirst;
		if (kgen->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[139]);
		ResultatFonction* rf = NULL;
		rf = suivantde(kgen->value, rf, 1);
		if (contextualpattern == kfirst)
			kgen->value = rf;
		else
			kgen = new KifXipGeneration(callfunc->kifcode, dom, rf);
		return kgen;
	}

	if (kfirst->type == kifXML) {

		xmlNodePtr n = (xmlNodePtr)kfirst->Node();
		if (n == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[140]);
		n = n->next;
		if (n == NULL)
			return kifNULL;
		return new Kifxml(callfunc->kifcode, dom, n);
	}

	if (kfirst->type == kifIterator) {
		kfirst->Next();
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[141]);
}


KifElement* ParseurPreSister(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter should be a node
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);

	KifXipNode* knode = (KifXipNode*)kfirst;
	if (knode->value == NULL)
		return callfunc->kifcode->Returnerror(kifErrorStrings[138]);

	Noeud* n = knode->value->frere;
	if (n == NULL)
		return kifNULL;

	if (contextualpattern == kfirst)
		knode->value = n;
	else
		knode = new KifXipNode(callfunc->kifcode, dom, n);

	return knode;
}

KifElement* ParseurPrePrevious(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter should be a node
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {

		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[142]);

		Noeud* n = knode->value->precedent;
		if (n == NULL)
			return kifNULL;

		if (contextualpattern == kfirst)
			knode->value = n;
		else
			knode = new KifXipNode(callfunc->kifcode, dom, n);
		return knode;
	}

	if (kfirst->type == kifXipGeneration) {
		KifXipGeneration* kgen = (KifXipGeneration*)kfirst;
		if (kgen->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[143]);
		ResultatFonction* rf = NULL;
		rf = suivantde(kgen->value, rf, -1);
		if (contextualpattern == kfirst)
			kgen->value = rf;
		else
			kgen = new KifXipGeneration(callfunc->kifcode, dom, rf);
		return kgen;
	}

	if (kfirst->type == kifXML) {

		xmlNodePtr n = (xmlNodePtr)kfirst->Node();
		if (n == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[144]);
		n = n->prev;
		if (n == NULL)
			return kifNULL;
		return new Kifxml(callfunc->kifcode, dom, n);
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[145]);
}

KifElement* ParseurPreParent(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter should be a node
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {

		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[146]);

		Noeud* n = knode->value->pere;
		if (n == NULL)
			return kifNULL;

		if (contextualpattern == kfirst)
			knode->value = n;
		else
			knode = new KifXipNode(callfunc->kifcode, dom, n);
		return knode;
	}
	if (kfirst->type == kifXipGeneration) {
		KifXipGeneration* kgen = (KifXipGeneration*)kfirst;
		if (kgen->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[147]);
		ResultatFonction* rf = NULL;
		rf = perede(kgen->value, rf);
		if (contextualpattern == kfirst)
			kgen->value = rf;
		else
			kgen = new KifXipGeneration(callfunc->kifcode, dom, rf);
		return kgen;
	}

	if (kfirst->type == kifXML) {

		xmlNodePtr n = (xmlNodePtr)kfirst->Node();
		if (n == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[148]);
		n = n->parent;
		if (n == NULL)
			return kifNULL;
		return new Kifxml(callfunc->kifcode, dom, n);
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[149]);
}

KifElement* ParseurPreChild(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter should be a node
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifXipNode) {

		KifXipNode* knode = (KifXipNode*)kfirst;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[150]);

		Noeud* n = knode->value->fils;
		if (n == NULL)
			return kifNULL;

		if (contextualpattern == kfirst)
			knode->value = n;
		else
			knode = new KifXipNode(callfunc->kifcode, dom, n);
		return knode;
	}
	if (kfirst->type == kifXipGeneration) {
		KifXipGeneration* kgen = (KifXipGeneration*)kfirst;
		if (kgen->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[151]);
		ResultatFonction* rf = NULL;
		rf = filsde(kgen->value, rf, NON);
		if (contextualpattern == kfirst)
			kgen->value = rf;
		else
			kgen = new KifXipGeneration(callfunc->kifcode, dom, rf);
		return kgen;
	}

	if (kfirst->type == kifXML) {

		xmlNodePtr n = (xmlNodePtr)kfirst->Node();
		if (n == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[152]);
		n = n->children;
		if (n == NULL)
			return kifNULL;
		return new Kifxml(callfunc->kifcode, dom, n);
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[153]);
}

KifElement* ParseurPreReplace(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//Replace an element with another
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kreg = callfunc->Evaluatethread(1, dom, idthread);
	KifElement* krep = callfunc->Evaluatethread(2, dom, idthread);

	if (kstr->type == kifGraph) {
		KifGraph* kg = (KifGraph*)kstr;
		return kg->Replace(kreg, krep);
	}

	string reg = kreg->String();
	string str = kstr->String();

	if (reg == "")
		return callfunc->kifcode->Providestring(str);

	string rep = krep->String();
	str = s_replacestring(str, reg, rep);

	return callfunc->kifcode->Providestring(str);
}


Exported KifElement* KifParser::constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
	if (params->InstructionSize() == 0)
		return this;
	KifElement* kinit = params->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	name = Getfilename(kinit->String());
	kinit->Release();

	if (params->InstructionSize() == 2) {
		kinit = params->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		statistique_interne = kinit->Boolean();
		kinit->Release();
	}


	try {
		id = XipKifGrmFile(name, dom->kifcode);
		if (toinitialize && BaseParseur != NULL)
			BaseParseur->InitialisationAvecParseur(id);
	}
	catch (XIPError* message) {
		string error;
		error = "XIP(333): line: ";
		char ch[100];
		sprintf_s(ch, 10, "%d", message->line);
		error += ch;
		error += " in " + message->filename;
		error += " --> ";
		error += message->message;
		delete message;
		return kifcode->Returnerror(error);
	}
	return kifTRUE;
}

KifElement* ParseurPreLoad(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* filename;


	if (ke->type == kifParser) {
		if (callfunc->Size() != 2 && callfunc->Size() != 3)
			return callfunc->kifcode->Returnerror(kifErrorStrings[129]);
		KifParser* kpars = (KifParser*)ke;

		if (kpars->id != -1)
			return callfunc->kifcode->Returnerror(kifErrorStrings[179]);
		filename = callfunc->Evaluatethread(1, dom, idthread);
		string flname = Getfilename(filename->String());
		if (callfunc->Size() == 3)
			statistique_interne = callfunc->Evaluatethread(2, dom, idthread)->Boolean();

		try {
			kpars->id = XipKifGrmFile(flname, dom->kifcode);
			kpars->name = flname;
			if (kpars->toinitialize) {
				if (BaseParseur != NULL)
					BaseParseur->InitialisationAvecParseur(kpars->id);
			}
		}
		catch (XIPError* message) {
			string error;
			error = "XIP(333): line: ";
			char ch[100];
			sprintf_s(ch, 10, "%d", message->line);
			error += ch;
			error += " in " + message->filename;
			error += " --> ";
			error += message->message;
			delete message;
			return callfunc->kifcode->Returnerror(error);
		}
		return kifTRUE;
	}

	return PreLoad(contextualpattern, dom, callfunc, idthread);
}

//==============================================================================================================================

KifElement* ParseurPrePush(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	if (callfunc->Size() < 2)
		return callfunc->kifcode->Returnerror(kifErrorStrings[38]);
	//First parameter should be a vector
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);

	if (kf->type == kifXipDependency || kf->type == kifXipGeneration) {
		KifXipDependency* knode = (KifXipDependency*)kf;
		if (knode->value == NULL)
			return kifFALSE;
		KifElement* res;
		char* s;
		for (size_t i = 1; i < callfunc->Size(); i++) {
			res = callfunc->Evaluatethread(i, dom, idthread);
			string sres = res->String();
			s = DupliqueChaine(STR(sres));
			knode->value->stack.ajoute(s);
		}
		return kifTRUE;
	}

	//second parameter is anything
	KifElement* res;
	for (size_t i = 1; i < callfunc->Size(); i++) {
		res = callfunc->Evaluatethread(i, dom, idthread);
		if (res == kifNOELEMENT)
			continue;
		kf->Push(res);
	}
	return kifTRUE;
}


KifElement* ParseurPrePop(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	if (callfunc->Size()<1 || callfunc->Size()>3)
		return callfunc->kifcode->Returnerror(kifErrorStrings[40]);
	//First parameter should be a vector
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);

	if (kf->type == kifGraph) {
		KifGraph* kg = (KifGraph*)kf;
		if (kg->graph == NULL)
			return kifFALSE;
		KifElement* ks = callfunc->Evaluatethread(1, dom, idthread);
		return kg->Remove(ks);
	}

	//second parameter is an integer (the number of pop)
	KifElement* nb = NULL;
	int nbpop = 1;
	if (callfunc->Size() == 3)
		nbpop = callfunc->Evaluatethread(2, dom, idthread)->Integer();
	if (callfunc->Size() >= 2)
		nb = callfunc->Evaluatethread(1, dom, idthread);
	else
		nb = kifMINUSONE;

	if (kf->aContainer() || kf->type == kifString || kf->type == kifStringPtr) {
		while (nbpop > 0) {
			kf->Pop(nb);
			nbpop--;
		}
		return kifTRUE;
	}

	int ix = nb->Integer();
	if (kf->type == kifXipDependency || kf->type == kifXipGeneration) {

		KifXipDependency* knode = (KifXipDependency*)kf;
		if (knode->value == NULL)
			return kifFALSE;

		char* s;
		if (ix == -1)
			s = knode->value->stack.retire();
		else
			s = knode->value->stack.retireElement(ix);

		if (s != NULL) {
			string str = s;
			delete s;
			return callfunc->kifcode->Providestring(str);
		}
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[41]);
}

KifElement* ParseurPreLast(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	if (callfunc->Size() != 1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[42]);
	//First parameter should be a vector
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	//second parameter is an integer (the number of pop)

	if (kf->aContainer() || kf->type == kifString || kf->type == kifStringPtr)
		return kf->Last();

	if (kf->type == kifXipNode) {

		KifXipNode* knode = (KifXipNode*)kf;
		if (knode->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[43]);

		Noeud* n = knode->value->fils;
		if (n == NULL || n->dernier == NULL)
			return kifNULL;
		n = n->dernier;
		if (contextualpattern == kf)
			knode->value = n;
		else
			knode = new KifXipNode(callfunc->kifcode, dom, n);
		return knode;
	}

	if (kf->type == kifXipGeneration) {
		KifXipGeneration* kgen = (KifXipGeneration*)kf;
		if (kgen->value == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[44]);
		ResultatFonction* rf = NULL;
		rf = filsde(kgen->value, rf, OUI);
		if (contextualpattern == kf)
			kgen->value = rf;
		else
			kgen = new KifXipGeneration(callfunc->kifcode, dom, rf);
		return kgen;
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[45]);
}

KifElement* ParseurPreSet(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	if (callfunc->Size() == 0)
		return callfunc->kifcode->Returnerror(kifErrorStrings[88]);

	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);

	if (kfirst->type == kifGraph) {
		if (callfunc->Size() != 2)
			return callfunc->kifcode->Returnerror(kifErrorStrings[88]);
		KifElement* kname = callfunc->Evaluatethread(1, dom, idthread);
		KifGraph* kg = (KifGraph*)kfirst;
		if (kg->graph != NULL)
			kg->graph->nom = kname->String();
		return kifTRUE;
	}

	if (kfirst->type == kifIterator) {
		if (callfunc->Size() != 2)
			return callfunc->kifcode->Returnerror(kifErrorStrings[88]);
		KifElement* value = callfunc->Evaluatethread(1, dom, idthread);
		KifElement* var = kfirst->IteratorValue();
		var->Setvalue(value);
		return kifTRUE;
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[92]);
}


KifElement* KifParseVarParser(KifCode* kifcode, x_node* xn, KifElement* base) {
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	KifParser* local = new KifParser(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}


KifElement* KifParseVarNode(KifCode* kifcode, x_node* xn, KifElement* base) {
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	KifXipNode* local = new KifXipNode(kifcode, variable, NULL);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

KifElement* KifParseVarGraph(KifCode* kifcode, x_node* xn, KifElement* base) {
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	KifGraph* local = new KifGraph(kifcode, variable, NULL);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

KifElement* KifParseVarDependency(KifCode* kifcode, x_node* xn, KifElement* base) {
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	KifXipDependency* local = new KifXipDependency(kifcode, variable, NULL);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//--------------------------------------------------------------------------
KifElement* KifFloatPtr::Declaration(short n) {
	KifBaseFloat f(NULL, NULL);
	f.kifcode = kifcode;
	return f.Declaration(n);
}
Exported KifElement* KifFloatPtr::Information(KifElement* kstr) {
	KifBaseFloat f(NULL, NULL);
	f.kifcode = kifcode;
	return f.Information(kstr);
}

void KifFloatPtr::Methods(KifVectorString* fcts) {
	KifBaseFloat f(NULL, NULL);
	return f.Methods(fcts);
}

string KifFloatPtr::String() {
	//ostringstream is;
	//is<<value;
	sprintf_s(kifGlobals->buffer, 100, "%g", *valueptr);
	return kifGlobals->buffer;
}

Exported KifFraction* KifFloatPtr::Fraction() {
	KifBaseFloat f(NULL, NULL, *valueptr);
	f.kifcode = kifcode;
	return f.Fraction();
}

Exported KifElement* KifFloatPtr::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	double ib = b->Float();
	if (ib == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	if (autoself) {
		*valueptr /= ib;
		return this;
	}
	double i = a->Float() / ib;
	KifElement* kv = kifcode->Providefloat(i);
	return kv;
}

Exported KifElement* KifFloatPtr::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself) {
		*valueptr = pow(*valueptr, b->Float());
		return this;
	}
	double i = pow(a->Float(), b->Float());
	return kifcode->Providefloat(i);
}

Exported bool KifFloatPtr::Setvalue(KifElement* ke, KifElement* index, KifElement* dom) {
	*valueptr = ke->Float();
	return true;
}


KifElement* KifStringPtr::Declaration(short n) {
	KifBaseString f(NULL, NULL);
	f.kifcode = kifcode;
	return f.Declaration(n);
}
Exported KifElement* KifStringPtr::Information(KifElement* kstr) {
	KifBaseString f(NULL, NULL);
	f.kifcode = kifcode;
	return f.Information(kstr);
}

void KifStringPtr::Methods(KifVectorString* fcts) {
	KifBaseString f(NULL, NULL);
	return f.Methods(fcts);
}

long KifStringPtr::Size() {
	return size_c(*valueptr);
}

long KifStringPtr::Integer() {
	if (*valueptr == "")
		return 0;
	long i = atoi(valueptr->c_str());
	return i;
}
double KifStringPtr::Float() {
	if (*valueptr == "")
		return 0;
	double i = atof(valueptr->c_str());
	return i;
}

KifElement* KifStringPtr::Newiterator(bool rev) {
	KifIteratorString* iter = new KifIteratorString(kifcode, NULL);
	iter->reverse = rev;
	iter->value = *valueptr;
	return iter;
}

bool KifStringPtr::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
#ifdef PROFILING
	profilingcall("String::Setvalue");
#endif
	if (ke == this)
		return true;
	bool ret = true;
	if (idx == NULL || idx == kifNULL)
		*valueptr = ke->String();
	else {
		int ikey = idx->Integer();
		string s = ke->String();
		ret = c_char_index_assign(*valueptr, s, ikey);
	}
	return ret;
}

KifElement* KifStringPtr::Map(KifElement* dom, KifMap* m) {
	ThreadLock _lock(type);
	string smap = *valueptr;
	kifGlobals->PushStack((KifDomain*)dom);
	KifElement* ke = kifcode->EvaluateMap(dom, smap);
	kifGlobals->PopStack();
	if (ke->type != kifMap)
		return kifNULL;
	threadblocinit;
	ke = ke->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
	return ke;
}


KifElement* KifStringPtr::Vector(KifElement* dom, KifVector* v) {
	ThreadLock _lock(type);
	string svect = *valueptr;
	kifGlobals->PushStack((KifDomain*)dom);
	KifElement* ke = kifcode->EvaluateVector(dom, svect);
	kifGlobals->PopStack();
	if (ke->type != kifVector)
		return kifNULL;
	threadblocinit;
	ke = ke->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
	return ke;
}

KifElement* KifStringPtr::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifBaseString f(NULL, NULL, *valueptr);
	f.kifcode = kifcode;
	return f.in(a, b, kfunc, contextualpattern, idx);
}

KifElement* KifStringPtr::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifBaseString f(NULL, NULL, *valueptr);
	f.kifcode = kifcode;
	return f.Exec(contextualpattern, dom, idx, idthread, callfunc);
}

//-------------------------------------------------------------------------

Exported void KifCodeParseur::CreeVariables() {
	KifVariableDeclaration* var;
	//then we create its value
	KifBaseInteger* local;

	var = Createstaticvariable("XIP_TAGGER"); local = new KifBaseInteger(this, var, -40);
	var = Createstaticvariable("XIP_PRINT_RULES"); local = new KifBaseInteger(this, var, -30);
	var = Createstaticvariable("XIP_NO_DISPLAY"); local = new KifBaseInteger(this, var, -20);
	var = Createstaticvariable("XIP_ENABLE_DEPENDENCY"); local = new KifBaseInteger(this, var, -10);
	var = Createstaticvariable("XIP_LEMMA"); local = new KifBaseInteger(this, var, 0);
	var = Createstaticvariable("XIP_SURFACE"); local = new KifBaseInteger(this, var, 1);
	var = Createstaticvariable("XIP_MARKUP"); local = new KifBaseInteger(this, var, 2);
	var = Createstaticvariable("XIP_ENTREE"); local = new KifBaseInteger(this, var, 3);
	var = Createstaticvariable("XIP_CATEGORY"); local = new KifBaseInteger(this, var, 4);
	var = Createstaticvariable("XIP_REDUCED"); local = new KifBaseInteger(this, var, 5);
	var = Createstaticvariable("XIP_FULL"); local = new KifBaseInteger(this, var, 6);
	var = Createstaticvariable("XIP_OFFSET"); local = new KifBaseInteger(this, var, 7);
	var = Createstaticvariable("XIP_WORDNUM"); local = new KifBaseInteger(this, var, 8);
	var = Createstaticvariable("XIP_SENTENCE"); local = new KifBaseInteger(this, var, 9);
	var = Createstaticvariable("XIP_NONE"); local = new KifBaseInteger(this, var, 14);
	var = Createstaticvariable("XIP_DEPENDENCY_BY_NAME"); local = new KifBaseInteger(this, var, 15);
	var = Createstaticvariable("XIP_DEPENDENCY_BY_NODE"); local = new KifBaseInteger(this, var, 16);
	var = Createstaticvariable("XIP_DEPENDENCY_BY_CREATION"); local = new KifBaseInteger(this, var, 17);
	var = Createstaticvariable("XIP_TREE"); local = new KifBaseInteger(this, var, 18);
	var = Createstaticvariable("XIP_TREE_PHRASE"); local = new KifBaseInteger(this, var, 19);
	var = Createstaticvariable("XIP_TREE_COLUMN"); local = new KifBaseInteger(this, var, 20);
	var = Createstaticvariable("XIP_MERGE_XML_SUBTREE"); local = new KifBaseInteger(this, var, 21);
	var = Createstaticvariable("XIP_CONVERSION_UTF8"); local = new KifBaseInteger(this, var, 22);
	var = Createstaticvariable("XIP_EXECUTION_ERROR"); local = new KifBaseInteger(this, var, 23);
	var = Createstaticvariable("XIP_MATHEMATICS"); local = new KifBaseInteger(this, var, 24);
	var = Createstaticvariable("XIP_DEPENDENCY_NUMBER"); local = new KifBaseInteger(this, var, 25);
	var = Createstaticvariable("XIP_UTF8_INPUT"); local = new KifBaseInteger(this, var, 26);
	var = Createstaticvariable("XIP_EXECUTE_TOKEN"); local = new KifBaseInteger(this, var, 27);
	var = Createstaticvariable("XIP_SENTENCE_NUMBER"); local = new KifBaseInteger(this, var, 28);
	var = Createstaticvariable("XIP_LANGUAGE_GUESSER"); local = new KifBaseInteger(this, var, 29);
	var = Createstaticvariable("XIP_NOT_USED"); local = new KifBaseInteger(this, var, 30);
	var = Createstaticvariable("XIP_CHUNK_TREE"); local = new KifBaseInteger(this, var, 31);
	var = Createstaticvariable("XIP_DEPENDENCY_FEATURE_VALUE"); local = new KifBaseInteger(this, var, 32);
	var = Createstaticvariable("XIP_NO_TAG_NORMALISATION"); local = new KifBaseInteger(this, var, 33);
	var = Createstaticvariable("XIP_LOWER_INPUT"); local = new KifBaseInteger(this, var, 34);
	var = Createstaticvariable("XIP_CHECK_INPUT_UTF8"); local = new KifBaseInteger(this, var, 35);
	var = Createstaticvariable("XIP_GENERATION_CATEGORY"); local = new KifBaseInteger(this, var, 36);
	var = Createstaticvariable("XIP_GENERATION"); local = new KifBaseInteger(this, var, 37);
	var = Createstaticvariable("XIP_RANDOM_ANALYSIS"); local = new KifBaseInteger(this, var, 38);
	var = Createstaticvariable("XIP_PASSAGE_XML_FORMAT"); local = new KifBaseInteger(this, var, 39);
	var = Createstaticvariable("XIP_JSON"); local = new KifBaseInteger(this, var, 40);
	var = Createstaticvariable("XIP_JSON_API"); local = new KifBaseInteger(this, var, 41);
	var = Createstaticvariable("XIP_COMPUTE_DISTANCE"); local = new KifBaseInteger(this, var, 42);
	var = Createstaticvariable("XIP_CLOSING_POS"); local = new KifBaseInteger(this, var, 43);
	var = Createstaticvariable("XIP_DISPLAY_TREEBANK"); local = new KifBaseInteger(this, var, 44);
}

void ParseurKifInit(KifCode* kifcodeptr) {
	//We need to be carreful to avoid recreating stuff...
	if (kifGlobals->kifStringType.find(kifParser) != kifGlobals->kifStringType.end())
		return;

	KifSetMainThread();

	kifGlobals->KifAssociateType("parser", KifParseVarParser, kifParser);
	kifGlobals->KifAssociateType("node", KifParseVarNode, kifXipNode);
	kifGlobals->KifAssociateType("graph", KifParseVarGraph, kifGraph);
	kifGlobals->KifAssociateType("dependency", KifParseVarDependency, kifXipDependency);

	kifGlobals->kifStringType[kifMapPtr] = "sstreemap";
	kifGlobals->equivalences[kifMapPtr] = kifMap;

	kifGlobals->kifStringType[kifVectorPtr] = "svector";
	kifGlobals->equivalences[kifVectorPtr] = kifVector;

	kifGlobals->kifStringType[kifFloatVectorPtr] = "fvector";
	kifGlobals->equivalences[kifFloatVectorPtr] = kifVector;

	kifGlobals->kifStringType[kifStringPtr] = "string";
	kifGlobals->equivalences[kifStringPtr] = kifString;

	KifRecreatePredefined(kifcodeptr, "nodeinfos", P_ONE | P_TWO, &ParseurPreNodeInfos);
	KifRecreatePredefined(kifcodeptr, "surface", P_ONE | P_TWO, &ParseurPreSurface);
	KifRecreatePredefined(kifcodeptr, "lemma", P_ONE | P_TWO | P_FOUR, &ParseurPreLemma);
	KifRecreatePredefined(kifcodeptr, "nodedistance", P_TWO, &ParseurPreNodeDistance);
	KifRecreatePredefined(kifcodeptr, "nodetreedistance", P_TWO, &ParseurPreNodeTreeDistance);
	KifRecreatePredefined(kifcodeptr, "sentencenumber", P_ONE, &ParseurPreSentenceNumber);
	KifRecreatePredefined(kifcodeptr, "categories", P_ONE, &ParseurPreParseCategories);
	KifRecreatePredefined(kifcodeptr, "dependencies", P_ONE, &ParseurPreParseDependencies);
	KifRecreatePredefined(kifcodeptr, "reload", P_ONE, &ParseurPreParseReload);
	KifRecreatePredefined(kifcodeptr, "grammarfiles", P_ONE, &ParseurPreParseFiles);
	KifRecreatePredefined(kifcodeptr, "setoption", P_ATLEASTTWO, &ParseurPreParseSetOption);
	KifRecreatePredefined(kifcodeptr, "addoption", P_ATLEASTTWO, &ParseurPreParseAddOption);
	KifRecreatePredefined(kifcodeptr, "upto", P_TWO, &ParseurPreSetrulelimit);
	KifRecreatePredefined(kifcodeptr, "nbrules", P_ONE, &ParseurPreNbRules);
	KifRecreatePredefined(kifcodeptr, "applied", P_ONE, &ParseurPreAppliedRules);
	KifRecreatePredefined(kifcodeptr, "getrulebody", P_TWO, &ParseurPreGetRuleBody);
	KifRecreatePredefined(kifcodeptr, "removeoption", P_ATLEASTTWO, &ParseurPreParseRemoveOption);
	KifRecreatePredefined(kifcodeptr, "getgrammar", P_ONE | P_TWO, &ParseurGetGrammar);
	KifRecreatePredefined(kifcodeptr, "parse", P_THREE | P_TWO, &ParseurPreParse);
	KifRecreatePredefined(kifcodeptr, "distances", P_ONE, &ParseurPreDistances);
	KifRecreatePredefined(kifcodeptr, "distance", P_ONE, &ParseurPreDistance);
	KifRecreatePredefined(kifcodeptr, "setcatchxml", P_TWO, &ParseurPreCatchXml);
	KifRecreatePredefined(kifcodeptr, "lemmafeature", P_ONE, &ParseurPreLemmaName);
	KifRecreatePredefined(kifcodeptr, "surfacefeature", P_ONE, &ParseurPreSurfaceName);
	KifRecreatePredefined(kifcodeptr, "parsefeatures", P_THREE, &ParseurPreParseFeatures);
	KifRecreatePredefined(kifcodeptr, "parsestructure", P_TWO | P_THREE, &ParseurPreParseStructure);
	KifRecreatePredefined(kifcodeptr, "lookup", P_FOUR | P_THREE | P_TWO, &ParseurPreLookup);
	KifRecreatePredefined(kifcodeptr, "buffer", P_ONE, &ParseurPreInternalBuffer);
	KifRecreatePredefined(kifcodeptr, "sentence", P_ONE, &ParseurPreGetSentence);
	KifRecreatePredefined(kifcodeptr, "addendum", P_THREE | P_TWO, &ParseurPreAddendum);
	KifRecreatePredefined(kifcodeptr, "parsefile", P_FOUR | P_THREE, &ParseurPreParseFile);
	KifRecreatePredefined(kifcodeptr, "generaterule", P_SEVEN | P_EIGHT | P_NINE, &ParseurPreGenerateRule);
	KifRecreatePredefined(kifcodeptr, "generatefromfile", P_THREE | P_TWO, &ParseurPreGenerateFromFile);
	KifRecreatePredefined(kifcodeptr, "generatefromstring", P_THREE | P_TWO, &ParseurPreGenerateFromString);
	KifRecreatePredefined(kifcodeptr, "match", P_TWO, &ParseurPreMatch);
	KifRecreatePredefined(kifcodeptr, "createdependency", P_FOUR, &ParseurPreCreateDependency);
	KifRecreatePredefined(kifcodeptr, "setfeature", P_TWO | P_THREE, &ParseurPreSetFeature);
	KifRecreatePredefined(kifcodeptr, "feature", P_THREE | P_TWO, &ParseurPreFeature);
	KifRecreatePredefined(kifcodeptr, "removefeature", P_TWO, &ParseurPreRemoveFeature);

	//Functions for: iterator,node,xml

	KifRecreatePredefined(kifcodeptr, "previous", P_ONE, &ParseurPrePrevious);


	//Functions for: node,xml
	KifRecreatePredefined(kifcodeptr, "parent", P_ONE, &ParseurPreParent);
	KifRecreatePredefined(kifcodeptr, "child", P_ONE, &ParseurPreChild);
	KifRecreatePredefined(kifcodeptr, "next", P_ONE, &ParseurPreNext);
	KifRecreatePredefined(kifcodeptr, "sister", P_ONE, &ParseurPreSister);
	KifRecreatePredefined(kifcodeptr, "last", P_ONE, &ParseurPreLast);

	KifRecreatePredefined(kifcodeptr, "load", P_ONE | P_TWO | P_THREE | P_FIVE, &ParseurPreLoad);

	//Function for: node
	KifRecreatePredefined(kifcodeptr, "number", P_ONE, &ParseurPreXipNumber);
	KifRecreatePredefined(kifcodeptr, "offset", P_ONE | P_THREE, &ParseurPreOffset);
	KifRecreatePredefined(kifcodeptr, "offsetchar", P_ONE | P_THREE, &ParseurPreOffsetChar);
	KifRecreatePredefined(kifcodeptr, "tokenoffset", P_ONE | P_THREE, &ParseurPreTokenOffset);
	KifRecreatePredefined(kifcodeptr, "leftoffset", P_ONE, &ParseurPreLeftOffset);
	KifRecreatePredefined(kifcodeptr, "leftoffsetchar", P_ONE, &ParseurPreLeftOffsetChar);
	KifRecreatePredefined(kifcodeptr, "lefttokenoffset", P_ONE, &ParseurPreLeftTokenOffset);
	KifRecreatePredefined(kifcodeptr, "rightoffset", P_ONE, &ParseurPreRightOffset);
	KifRecreatePredefined(kifcodeptr, "rightoffsetchar", P_ONE, &ParseurPreRightOffsetChar);
	KifRecreatePredefined(kifcodeptr, "righttokenoffset", P_ONE, &ParseurPreRightTokenOffset);

	//Function for: node,xml,dependency
	KifRecreatePredefined(kifcodeptr, "data", P_ONE | P_TWO, &ParseurPreGetData);
	KifRecreatePredefined(kifcodeptr, "rulenumber", P_ONE, &ParseurPreGetRuleNumber);
	KifRecreatePredefined(kifcodeptr, "features", P_ONE | P_TWO, &ParseurPreGetData);
	KifRecreatePredefined(kifcodeptr, "readings", P_ONE, &ParseurPreReadings);
	KifRecreatePredefined(kifcodeptr, "properties", P_ONE, &ParseurPreProperties);
	KifRecreatePredefined(kifcodeptr, "name", P_ONE, &ParseurPreGetName);
	KifRecreatePredefined(kifcodeptr, "pos", P_ONE, &ParseurPreGetName);
	KifRecreatePredefined(kifcodeptr, "parameters", P_ONE, &ParseurPreDependencyParameters);
	KifRecreatePredefined(kifcodeptr, "stack", P_ONE | P_TWO, &ParseurPreDependencyStack);

	KifRecreatePredefined(kifcodeptr, "push", P_ATLEASTTWO, &ParseurPrePush);
	KifRecreatePredefined(kifcodeptr, "pop", P_ONE | P_TWO | P_THREE, &ParseurPrePop);
	KifRecreatePredefined(kifcodeptr, "set", P_TWO | P_FOUR | P_FIVE, &ParseurPreSet);
	KifRecreatePredefined(kifcodeptr, "replace", P_THREE, &ParseurPreReplace);

	InitMapBooleanString(kifParserFunctions, "load");
	InitMapBooleanString(kifParserFunctions, "methods", false);
	InitMapBooleanString(kifParserFunctions, "infos", false);
	InitMapBooleanString(kifParserFunctions, "isa", false);
	InitMapBooleanString(kifParserFunctions, "type", false);
	InitMapBooleanString(kifParserFunctions, "reload");
	InitMapBooleanString(kifParserFunctions, "grammarfiles");
	InitMapBooleanString(kifParserFunctions, "categories");
	InitMapBooleanString(kifParserFunctions, "dependencies");
	InitMapBooleanString(kifParserFunctions, "setoption");
	InitMapBooleanString(kifParserFunctions, "addoption");
	InitMapBooleanString(kifParserFunctions, "removeoption");
	InitMapBooleanString(kifParserFunctions, "upto");
	InitMapBooleanString(kifParserFunctions, "nbrules");
	InitMapBooleanString(kifParserFunctions, "applied");
	InitMapBooleanString(kifParserFunctions, "getrulebody");
	InitMapBooleanString(kifParserFunctions, "parse");
	InitMapBooleanString(kifParserFunctions, "build");
	InitMapBooleanString(kifParserFunctions, "distance");
	InitMapBooleanString(kifParserFunctions, "distances");
	InitMapBooleanString(kifParserFunctions, "setcatchxml");
	InitMapBooleanString(kifParserFunctions, "lemmafeature");
	InitMapBooleanString(kifParserFunctions, "surfacefeature");
	InitMapBooleanString(kifParserFunctions, "parsefeatures");
	InitMapBooleanString(kifParserFunctions, "getgrammar");
	InitMapBooleanString(kifParserFunctions, "sentencenumber");
	InitMapBooleanString(kifParserFunctions, "parsestructure");
	InitMapBooleanString(kifParserFunctions, "lookup");
	InitMapBooleanString(kifParserFunctions, "buffer");
	InitMapBooleanString(kifParserFunctions, "createdependency");
	InitMapBooleanString(kifParserFunctions, "addendum");
	InitMapBooleanString(kifParserFunctions, "parsefile");
	InitMapBooleanString(kifParserFunctions, "generaterule");
	InitMapBooleanString(kifParserFunctions, "generatefromstring");
	InitMapBooleanString(kifParserFunctions, "generatefromfile");
	InitMapBooleanString(kifParserFunctions, "texttoxml");
	InitMapBooleanString(kifParserFunctions, "parsexml");
	InitMapBooleanString(kifParserFunctions, "parsexmlstring");
	InitMapBooleanString(kifParserFunctions, "name");

	InitMapBooleanString(kifGraphFunctions, "isa", false);
	InitMapBooleanString(kifGraphFunctions, "set");
	InitMapBooleanString(kifGraphFunctions, "name");
	InitMapBooleanString(kifGraphFunctions, "type", false);
	InitMapBooleanString(kifGraphFunctions, "methods", false);
	InitMapBooleanString(kifGraphFunctions, "infos", false);
	InitMapBooleanString(kifGraphFunctions, "replace");
	InitMapBooleanString(kifGraphFunctions, "project");
	InitMapBooleanString(kifGraphFunctions, "pop");
	InitMapBooleanString(kifGraphFunctions, "match");

	InitMapBooleanString(kifNodeFunctions, "next");
	InitMapBooleanString(kifNodeFunctions, "sister");
	InitMapBooleanString(kifNodeFunctions, "last");
	InitMapBooleanString(kifNodeFunctions, "previous");
	InitMapBooleanString(kifNodeFunctions, "parent");
	InitMapBooleanString(kifNodeFunctions, "data");
	InitMapBooleanString(kifNodeFunctions, "features");
	InitMapBooleanString(kifNodeFunctions, "rulenumber");
	InitMapBooleanString(kifNodeFunctions, "readings");
	InitMapBooleanString(kifNodeFunctions, "name");
	InitMapBooleanString(kifNodeFunctions, "pos");
	InitMapBooleanString(kifNodeFunctions, "json");
	InitMapBooleanString(kifNodeFunctions, "vector");
	InitMapBooleanString(kifNodeFunctions, "map");
	InitMapBooleanString(kifNodeFunctions, "child");
	InitMapBooleanString(kifNodeFunctions, "xmlnode");
	InitMapBooleanString(kifNodeFunctions, "surface");
	InitMapBooleanString(kifNodeFunctions, "nodeinfos");
	InitMapBooleanString(kifNodeFunctions, "lemma");
	InitMapBooleanString(kifNodeFunctions, "nodedistance");
	InitMapBooleanString(kifNodeFunctions, "nodetreedistance");
	InitMapBooleanString(kifNodeFunctions, "setfeature");
	InitMapBooleanString(kifNodeFunctions, "feature");
	InitMapBooleanString(kifNodeFunctions, "sentence");
	InitMapBooleanString(kifNodeFunctions, "removefeature");
	InitMapBooleanString(kifNodeFunctions, "isa", false);
	InitMapBooleanString(kifNodeFunctions, "methods", false);
	InitMapBooleanString(kifNodeFunctions, "infos", false);
	InitMapBooleanString(kifNodeFunctions, "type", false);
	InitMapBooleanString(kifNodeFunctions, "number");
	InitMapBooleanString(kifNodeFunctions, "offset");
	InitMapBooleanString(kifNodeFunctions, "offsetchar");
	InitMapBooleanString(kifNodeFunctions, "tokenoffset");
	InitMapBooleanString(kifNodeFunctions, "leftoffset");
	InitMapBooleanString(kifNodeFunctions, "leftoffsetchar");
	InitMapBooleanString(kifNodeFunctions, "lefttokenoffset");
	InitMapBooleanString(kifNodeFunctions, "rightoffset");
	InitMapBooleanString(kifNodeFunctions, "rightoffsetchar");
	InitMapBooleanString(kifNodeFunctions, "righttokenoffset");
	InitMapBooleanString(kifNodeFunctions, "string");
	InitMapBooleanString(kifNodeFunctions, "int");
	InitMapBooleanString(kifNodeFunctions, "float");
	InitMapBooleanString(kifNodeFunctions, "boolean");

	InitMapBooleanString(kifDependencyFunctions, "string");
	InitMapBooleanString(kifDependencyFunctions, "json");
	InitMapBooleanString(kifDependencyFunctions, "int");
	InitMapBooleanString(kifDependencyFunctions, "float");
	InitMapBooleanString(kifDependencyFunctions, "boolean");
	InitMapBooleanString(kifDependencyFunctions, "data");
	InitMapBooleanString(kifDependencyFunctions, "rulenumber");
	InitMapBooleanString(kifDependencyFunctions, "features");
	InitMapBooleanString(kifDependencyFunctions, "name");
	InitMapBooleanString(kifDependencyFunctions, "setfeature");
	InitMapBooleanString(kifDependencyFunctions, "feature");
	InitMapBooleanString(kifDependencyFunctions, "removefeature");
	InitMapBooleanString(kifDependencyFunctions, "isa", false);
	InitMapBooleanString(kifDependencyFunctions, "methods", false);
	InitMapBooleanString(kifDependencyFunctions, "infos", false);
	InitMapBooleanString(kifDependencyFunctions, "type", false);
	InitMapBooleanString(kifDependencyFunctions, "stack");
	InitMapBooleanString(kifDependencyFunctions, "push");
	InitMapBooleanString(kifDependencyFunctions, "pop");
	InitMapBooleanString(kifDependencyFunctions, "last");
	InitMapBooleanString(kifDependencyFunctions, "parameters");

	InitMapBooleanString(kifGenerationFunctions, "string");
	InitMapBooleanString(kifGenerationFunctions, "rulenumber");
	InitMapBooleanString(kifGenerationFunctions, "int");
	InitMapBooleanString(kifGenerationFunctions, "float");
	InitMapBooleanString(kifGenerationFunctions, "boolean");
	InitMapBooleanString(kifGenerationFunctions, "data");
	InitMapBooleanString(kifGenerationFunctions, "features");
	InitMapBooleanString(kifGenerationFunctions, "name");
	InitMapBooleanString(kifGenerationFunctions, "setfeature");
	InitMapBooleanString(kifGenerationFunctions, "feature");
	InitMapBooleanString(kifGenerationFunctions, "removefeature");
	InitMapBooleanString(kifGenerationFunctions, "isa", false);
	InitMapBooleanString(kifGenerationFunctions, "methods", false);
	InitMapBooleanString(kifGenerationFunctions, "infos", false);
	InitMapBooleanString(kifGenerationFunctions, "type", false);
	InitMapBooleanString(kifGenerationFunctions, "stack");
	InitMapBooleanString(kifGenerationFunctions, "push");
	InitMapBooleanString(kifGenerationFunctions, "pop");
	InitMapBooleanString(kifGenerationFunctions, "last");
	InitMapBooleanString(kifGenerationFunctions, "parameters");
	InitMapBooleanString(kifGenerationFunctions, "next");
	InitMapBooleanString(kifGenerationFunctions, "previous");
	InitMapBooleanString(kifGenerationFunctions, "parent");
	InitMapBooleanString(kifGenerationFunctions, "last");
	InitMapBooleanString(kifGenerationFunctions, "child");


	kifGlobals->kifAllFunctions[kifParser] = kifParserFunctions;
	kifGlobals->kifAllFunctions[kifGraph] = kifGraphFunctions;
	kifGlobals->kifAllFunctions[kifXipNode] = kifNodeFunctions;
	kifGlobals->kifAllFunctions[kifXipDependency] = kifDependencyFunctions;
	kifGlobals->kifAllFunctions[kifXipGeneration] = kifGenerationFunctions;


	kifInformationNode["name"] = "name(): return the part of speech";
	kifInformationNode["vector"] = "vector(): return the node tree as a vector";
	kifInformationNode["map"] = "map(): return the node tree as a map";
	kifInformationNode["pos"] = "pos(): return the part of speech. It should be noted, that if the recipient variable is a vector and the element is ambiguous, then a vector of pos can be returned (the same applied to name above).";
	kifInformationNode["data"] = "data(map feats): Features are stored in map as attribute/value";
	kifInformationNode["rulenumber"] = "rulenumber: Return the number of the rule that was used to create that node";
	kifInformationNode["setfeature"] = "setfeature(string att, string val): feature assignment to the node";
	kifInformationNode["removefeature"] = "removefeature(string att): remove the feature from node whose attribute is att";
	kifInformationNode["sentence"] = "sentence(): return the sentence spanned by a node.";
	kifInformationNode["feature"] = "feature(string att): return the value of an attribute\nfeature(string att,string val): test the value of an attribute";
	kifInformationNode["features"] = "features(): return a smap of all features.\nfeatures(smap f): set the features from a smap";
	kifInformationNode["readings"] = "readings(): return a smap of all possible readings for a given word";
	kifInformationNode["next"] = "next(): return the next node after current node. Can also return the next node if the structure is flat from the sentence.";
	kifInformationNode["sister"] = "sister(): return the next node after current node";
	kifInformationNode["previous"] = "previous(): return the previous node after current node";
	kifInformationNode["child"] = "child(): return the first child node under current node";
	kifInformationNode["last"] = "last(): return the last child node under current node";
	kifInformationNode["parent"] = "parent(): return the parent node above current node";
	kifInformationNode["surface"] = "surface(): return the surface string";
	kifInformationNode["nodeinfos"] = "nodeinfos(): return the node infos value.\tnodeinfos(string v): set the node infos value.";
	kifInformationNode["lemma"] = "lemma(): return the lemma string. It should be noted, that if the recipient variable is a vector and the element is ambiguous, then a vector of lemmas can be returned  as for pos above.";
	kifInformationNode["number"] = "number(): return the ID of current node";
	kifInformationNode["offset"] = "offset(int left,int right): left and right receive the offset";
	kifInformationNode["offsetchar"] = "offsetchar(int left,int right): left and right receive the character offset";
	kifInformationNode["tokenoffset"] = "tokenoffset(int left,int right): left and right receive the  token offset";
	kifInformationNode["leftoffset"] = "leftoffset(): return the left offset";
	kifInformationNode["rightoffset"] = "rightoffset(): return the right offset";
	kifInformationNode["leftoffsetchar"] = "leftoffsetchar(): return the left character offset";
	kifInformationNode["rightoffsetchar"] = "rightoffsetchar(): return the right character offset";
	kifInformationNode["righttokenoffset"] = "righttokenoffset(): return the right token offset";
	kifInformationNode["xmlnode"] = "xmlnode(): return the XML node associated with this node (TOKENIZE mode)";
	kifInformationDependency["name"] = "name(): return the dependency name";
	kifInformationDependency["rulenumber"] = "rulenumber: Return the numbers of the rule that were used to create that dependency";
	kifInformationDependency["data"] = "data(map feats): Features are stored in map as attribute/value";
	kifInformationDependency["setfeature"] = "setfeature(string att, string val): feature assignment to the dependency";
	kifInformationDependency["feature"] = "feature(string att): return the value of an attribute\nfeature(string att,string val): test the value of an attribute";
	kifInformationDependency["features"] = "features(): return a smap of all features.\nfeatures(smap f): set the features from a smap";
	kifInformationDependency["parameters"] = "parameters(vector v): return a vector of node variable";
	kifInformationDependency["stack"] = "stack(vector v): return the dependency stack in a vector of strings";
	kifInformationDependency["push"] = "push(string s): push s on the dependency’s stack";
	kifInformationDependency["pop"] = "pop(int i): remove the ith element from the dependency’s stack";
	kifInformationDependency["pop"] = "pop(): remove the last element on top of the dependency’s stack";

	kifInformationGeneration["rulenumber"] = "rulenumber: Return the numbers of the rule that were used to create that dependency";
	kifInformationGeneration["next"] = "next(): return the next Generation Node";
	kifInformationGeneration["previous"] = "previous(): return the previous Generation Node";
	kifInformationGeneration["child"] = "child(): return the first Generation Node  child";
	kifInformationGeneration["parent"] = "parent(): return the parent Generation Node";
	kifInformationGeneration["last"] = "last(): return the last child";

	kifInformationGraph["set"] = "set(string name): give the graph a name";
	kifInformationGraph["name"] = "name(): return the name of the graph";
	kifInformationGraph["project"] = "project(g): g should be a graph. We project g on the current graph.";
	kifInformationGraph["pop"] = "pop(remove): remove should be a graph. We remove from the current graph, the sub-graphs that match remove.";
	kifInformationGraph["replace"] = "replace(pattern,replacement): pattern and replacement should be two graphs. We replace the sub-graph that matches pattern with a new replacement graph.";
	kifInformationGraph["match"] = "match(extractor): extractor should be a graph. We return a vector of all sub-graphs from the current graph that match extractor.";

	kifInformationParser["load"] = "load(string grmpathname): Load a XIP grammar.";
	kifInformationParser["reload"] = "reload(): Reload a XIP grammar.";
	kifInformationParser["grammarfiles"] = "map m=grammarfiles(): Return all files for a loaded grammar.";
	kifInformationParser["categories"] = "categories(): Return a vector of all category names.";
	kifInformationParser["dependencies"] = "dependencies(): Return a vector of dependency names.";
	kifInformationParser["lemmafeature"] = "lemmafeature(): Return the lemma feature name in a grammar.";
	kifInformationParser["surfacefeature"] = "surfacefeature(): Return the surface feature name in a grammar.";
	kifInformationParser["distance"] = "distance(): Return a vector of distances between nodes, for the current analysis";
	kifInformationParser["distances"] = "distances(): Return a vector of distances between nodes, for all analyses";
	kifInformationParser["parse"] = "parse(string sentence,kif_exchange_data): parse a sentence using the grammar. If no variable is provided to get the analysis, then the result is displayed on the current output.";
	kifInformationParser["build"] = "build(string input,string output,bool norm): Build a xip fst file out of a text file containing on the first line surface form, then on next line lemma+features.";
	kifInformationParser["setcatchxml"] = "setcatchxml(function): Set the catch function, which will be used in an XML analysis to catch the 'string', which will be parsed beforehand.";
	kifInformationParser["parsefeatures"] = "parsefeatures(svector featinits,svector featres): Return the translation of the features from featinits in featres. This function also returns a POS, if found in featinits";
	kifInformationParser["getgrammar"] = "getgrammar(int handle): Get the current parser object to point to an existing grammar.";
	kifInformationParser["sentencenumber"] = "sentencenumber(): return the current number of the sentence being processed.";
	kifInformationParser["parsestructure"] = "parsestructure(map|vector tree): parse a predefined tree using the grammar. The tree can also be a vector. The structure should respect the output of map or of vector on a 'node'";
	kifInformationParser["lookup"] = "lookup(string sentence,bool displayfeatures): Lookup of a sentence using the grammar FSTs. ";
	kifInformationParser["buffer"] = "buffer(): Return the internal string buffer.";
	kifInformationParser["generaterule"] = "generaterule(string dependencyname,vector focus,vector nodes,vector nodefeatures,vector dependencies,vector dependencyfeatures,bool adquationmark,int ruletype): generate a rule based on a vector of nodes and a vector of dependencies. If addquestionmark is true, then in case of gaps, the system inserts ?+";
	kifInformationParser["parsefile"] = "parsefile(string input,kif_exchange_data,string output): parse the file input and store the results in output.\r\rparsefile(string input,kif_exchange_data): parse the file input within a XIP library call. A callback function must be provided in that case to catch the XipResult objects.";
	kifInformationParser["parsexml"] = "parsexml(string input,kif_exchange_data,string output,int depth): parse the XML file input and store the results in file output. Depth is the depth at which the analysis should take place.\r\rparsexml(string input,kif_exchange_data, int depth): parse the XML file input A callback function must be provided in that case to catch the XipResult objects.";
	kifInformationParser["parsexmlstring"] = "parsexmlstring(string input,kif_exchange_data,int depth): parse the XML string input and store the results in file output. Depth is the depth at which the analysis should take place.";
	kifInformationParser["generatefromstring"] = "generatefromstring(string input,kif_exchange_data): Generate a sentence out of a dependency string";
	kifInformationParser["generatefromfile"] = "generatefromfile(string pathname,kif_exchange_data): Generate a sentence out of a dependency file";
	kifInformationParser["texttoxml"] = "texttoxml(string file,kif_exchange_data, string tag, string encoding): transform a text file into an XML file, using 'tag' as the new xml root and 'encoding', which can take either 'latin' or 'utf8' as a value";
	kifInformationParser["addendum"] = "addendum(string filename): Enrich a grammar with an external parameter file.";
	kifInformationParser["setbreakpoint"] = "setbreakpoint(int breakpoint): Set a breakpoint in a grammar.";
	kifInformationParser["name"] = "name(): Return the pathname of the grm file";
	kifInformationParser["setoption"] = "setoption(int nb1,int nb2,etc.): Set the options of ids: nb1,nb2 etc. See below for a list of these options.";
	kifInformationParser["addoption"] = "addoption(int nb1,int nb2,etc.): Add the options of ids: nb1,nb2 etc. See below for a list of these options.";
	kifInformationParser["removeoption"] = "removeoption(int nb1,int nb2,etc.): Remove the options of ids: nb1,nb2, nb3 etc.";

	kifGlobals->kifAllInformation[kifParser] = kifInformationParser;
	kifGlobals->kifAllInformation[kifGraph] = kifInformationGraph;
	kifGlobals->kifAllInformation[kifXipNode] = kifInformationNode;
	kifGlobals->kifAllInformation[kifXipDependency] = kifInformationDependency;
	kifGlobals->kifAllInformation[kifXipGeneration] = kifInformationGeneration;

	KifRecordMainLibraries("xmldoc_KifInitialisationModule", xmldoc_KifInitialisationModule);
	KifRecordMainLibraries("kifsparse_KifInitialisationModule", kifsparse_KifInitialisationModule);
	KifRecordMainLibraries("kifsys_KifInitialisationModule", kifsys_KifInitialisationModule);
	KifRecordMainLibraries("kifsocket_KifInitialisationModule", kifsocket_KifInitialisationModule);

	xmldoc_KifInitialisationModule(kifcodeptr, KIFVERSION);
	kifsparse_KifInitialisationModule(kifcodeptr, KIFVERSION);
	kifsys_KifInitialisationModule(kifcodeptr, KIFVERSION);
	kifsocket_KifInitialisationModule(kifcodeptr, KIFVERSION);

	kifGlobals->KifAssociateType("generation", KifParseVarGeneration, kifXipGeneration);


}

//--------------------------------------------------------------------------------------
//On modifie KifCode avec l'objectif suivant, que les fonctions definies dans le code KiF appelant ait priorite sur celles definies dans la grammaire...
//De cette facon, on permet au programme appelant d'avoir priorite sur le reste
//La definition de KifCodeParseur est dans variable.h
Exported KifElement* KifCodeParseur::Get(short n, KifDomain* dom) {
	KifElement* ret = NULL;
	if (loader != NULL)
		ret = loader->kstart->Declaration(n);
	if (ret == NULL)
		return KifCode::Get(n, dom);
	return ret;
}

Exported KifElement* KifCodeParseur::Getbasic(short n, KifElement* dom) {
	KifElement* ret = NULL;
	if (loader != NULL)
		ret = loader->kstart->Declaration(n);
	if (ret == NULL)
		return KifCode::Getbasic(n, dom);
	return ret;
}


KifCodeParseur::KifCodeParseur(KifCode* l, int trig, void* d, ostream* o) : KifCode(trig, d) {
	loader = l;
	ParseurKifInit(this);
	CreeVariables();
}

KifCodeParseur::~KifCodeParseur() {
	if (kifGlobals->cleandata && kifGlobals->data != NULL) {
		delete (GlobalParseur*)kifGlobals->data;
		kifGlobals->cleandata = false;
		kifGlobals->data = NULL;
	}
}

Exported KifCode* KifCodeParseur::Newinstance() {
	KifInitLib();
	return new KifCodeParseur(this, kifGlobals->trigger, kifGlobals->data, kifGlobals->os);
}


Exported KifCode* Newkifcodeparseur(int t) {
	return new KifCodeParseur(NULL, t, NULL, NULL);
}

void KifCodeParseur::Terminate() {
	SetXipExecution(true);

	kifGlobals->Releaseallloquet();
	ThreadLock _lock(kifPrepareError);
	ThreadLock _lockxThreadLock(kifError);
	kifGlobals->executionbreak = 1;
}



