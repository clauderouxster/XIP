/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the KiF library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : KiF
Version    : 1.00
filename   : kifwordnet.cxx
Date       : 20/06/2013
Purpose    : Implementation of the KiF language: Wornet library
Programmer : Claude ROUX
Reviewer   :
*/

//Use KIF_DLL only in Windows, if you link to the KiF DLL
#ifdef WIN32
#include "Windows.h"
#endif

#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"

#include "wn.h"

short KifNewId(string n);
string KifIdString(short n);

#define STR(x) (char*)x.c_str()
void CreationRepertoire(char* noms, int pos);
//Replace Wordnet by your own definition

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType Wordnet_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class KifWordnet;
//This typedef defines a type "WordnetMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (KifWordnet::*WordnetMethod)(KifElement*, KifDomain*, KifCallFunction*, int);

static bool init = false;

class KifWordnet : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, WordnetMethod> linkedmethods;
	static map<string, string> infomethods;


	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	hmap<string, int> wordnetpos;
	hmap<int, string> poswordnet;
	hmap<string, int> wordnetsenses;
	hmap<string, int> wordnettypes;
	hmap<int, string> categories;

	//---------------------------------------------------------------------------------------------------------------------
	KifWordnet(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, Wordnet_type) {
		//Do not forget your variable initialisation
		wordnetpos["ALL_POS"] = 0;
		wordnetpos["NOUN"] = 1;
		wordnetpos["VERB"] = 2;
		wordnetpos["ADJ"] = 3;
		wordnetpos["ADV"] = 4;
		wordnetpos["SATELLITE"] = 5;
		wordnetpos["ADJSAT"] = 5;

		poswordnet[1] = "NOUN";
		poswordnet[2] = "VERB";
		poswordnet[3] = "ADJ";
		poswordnet[4] = "ADV";
		poswordnet[5] = "ADJSAT";

		/* Pointer type and search type counts */
		/* Pointers */

		wordnettypes["ANTPTR"] = ANTPTR;	/* ! */
		wordnettypes["HYPERPTR"] = HYPERPTR;	/* @ */
		wordnettypes["HYPOPTR"] = HYPOPTR;	/* ~ */
		wordnettypes["ENTAILPTR"] = ENTAILPTR;	/* * */
		wordnettypes["SIMPTR"] = SIMPTR;	/* & */

		wordnettypes["ISMEMBERPTR"] = ISMEMBERPTR;	/* #m */
		wordnettypes["ISSTUFFPTR"] = ISSTUFFPTR;	/* #s */
		wordnettypes["ISPARTPTR"] = ISPARTPTR;	/* #p */

		wordnettypes["HASMEMBERPTR"] = HASMEMBERPTR;	/* %m */
		wordnettypes["HASSTUFFPTR"] = HASSTUFFPTR;	/* %s */
		wordnettypes["HASPARTPTR"] = HASPARTPTR;	/* %p */

		wordnettypes["CAUSETO"] = CAUSETO;	/* > */
		wordnettypes["PPLPTR"] = PPLPTR;	/* < */
		wordnettypes["SEEALSOPTR"] = SEEALSOPTR;	/* ^ */
		wordnettypes["PERTPTR"] = PERTPTR;	/* \ */
		wordnettypes["ATTRIBUTE"] = ATTRIBUTE;	/* = */
		wordnettypes["VERBGROUP"] = VERBGROUP;	/* $ */
		wordnettypes["DERIVATION"] = DERIVATION;	/* + */
		wordnettypes["CLASSIFICATION"] = CLASSIFICATION;	/* ; */
		wordnettypes["CLASS"] = CLASS;	/* - */


		/* Misc searches */

		wordnetsenses["ALLSENSES"] = ALLSENSES;	/* pass to findtheinfo() if want all senses */
		wordnetsenses["SYNS"] = SYNS;
		wordnetsenses["FRAMES"] = FRAMES;
		wordnetsenses["COORDS"] = COORDS;
		wordnetsenses["RELATIVES"] = RELATIVES;
		wordnetsenses["HMERONYM"] = HMERONYM;
		wordnetsenses["HHOLONYM"] = HHOLONYM;
		wordnetsenses["WNGREP"] = WNGREP;
		wordnetsenses["OVERVIEW"] = OVERVIEW;

		wordnetsenses["CLASSIF_START"] = CLASSIF_START;
		wordnetsenses["CLASSIF_USAGE"] = CLASSIF_USAGE;    /* ;u */
		wordnetsenses["CLASSIF_REGIONAL"] = CLASSIF_REGIONAL;    /* ;r */
		wordnetsenses["CLASS_START"] = CLASS_START;
		wordnetsenses["CLASS_USAGE"] = CLASS_USAGE;      /* -u */
		wordnetsenses["CLASS_REGIONAL"] = CLASS_REGIONAL;      /* -r */

		wordnetsenses["INSTANCE"] = INSTANCE;
		wordnetsenses["INSTANCES"] = INSTANCES;

		categories[NOUN] = "n";
		categories[ADJ] = "a";
		categories[SATELLITE] = "a";
		categories[ADJSAT] = "a";
		categories[VERB] = "v";
		categories[ADV] = "r";
		if (!init) {
			if (wninit())
				kifcode->Returnerror("WRN(001): Wordnet could not be started");
			init = true;
		}
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return Wordnet_type;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		return true;
	}

	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifWordnet* kperso = new KifWordnet(kifcode, kp);
		return kperso;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}


	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	void Setstructure(KifMap* kcurrent, SynsetPtr sw, int def) {
		KifMap* knew;
		KifVectorString* vs;
		KifVectorString* vp;
		KifVectorInteger* vo;
		int i;

		kcurrent->Push("pos", kifcode->Providestringraw(sw->pos));
		vs = new KifVectorString(kifcode, NULL);
		if ((def & 1) == 1)
			vp = new KifVectorString(kifcode, NULL);
		vo = new KifVectorInteger(kifcode, NULL);

		for (i = 0; i<sw->wcount; i++)
			vs->values.push_back(sw->words[i]);

		for (i = 0; i<sw->ptrcount; i++) {
			if ((def & 1) == 1)
				vp->values.push_back(categories[sw->ppos[i]]);
			vo->values.push_back(sw->ptroff[i]);
		}

		kcurrent->Push("words", vs);
		if ((def & 1) == 1)
			kcurrent->Push("ppos", vp);
		kcurrent->Push("synsets", vo);

		kcurrent->Push("synset", kifcode->Provideinteger(sw->hereiam));
		kcurrent->Push("whichword", kifcode->Provideinteger(sw->whichword));

		if ((def & 2) == 2)
			kcurrent->Push("definition", kifcode->Providestringraw(sw->defn));

		if (sw->headword != NULL) {
			kcurrent->Push("headword", kifcode->Providestringraw(sw->headword));
			kcurrent->Push("headsense", kifcode->Provideinteger(sw->headsense));
		}
		if (sw->nextss != NULL) {
			knew = new KifMap(kifcode, NULL);
			kcurrent->Push("nextsynset", knew);
			Setstructure(knew, sw->nextss, def);
		}

		if (sw->nextform != NULL) {
			knew = new KifMap(kifcode, NULL);
			kcurrent->Push("nextform", knew);
			Setstructure(knew, sw->nextform, def);
		}

		if (sw->ptrlist != NULL) {
			knew = new KifMap(kifcode, NULL);
			kcurrent->Push("list", knew);
			Setstructure(knew, sw->ptrlist, def);
		}
	}


	KifElement* MethodFindinfos(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		//The first parameter is the token itself
		string lasttoken = callfunc->Evaluate(0, domain)->String();
		string pos = callfunc->Evaluate(1, domain)->String();

		if (wordnetpos.find(pos) == wordnetpos.end()) {
			lasttoken = "WRN(002): Unkwown POS:" + pos;
			return kifcode->Returnerror(lasttoken);
		}

		string mode = callfunc->Evaluate(2, domain)->String();
		if (wordnettypes.find(mode) == wordnettypes.end()) {
			lasttoken = "WRN(003): Unkwown TYPE:" + mode;
			return kifcode->Returnerror(lasttoken);
		}

		string sense = callfunc->Evaluate(3, domain)->String();
		if (wordnetsenses.find(sense) == wordnetsenses.end()) {
			lasttoken = "WRN(004): Unkwown SENSE:" + sense;
			return kifcode->Returnerror(lasttoken);
		}

		int def = callfunc->Evaluate(4, domain)->Integer();
		KifMap* kres = Selectmap(callfunc, contextualpattern);

		if (pos == "ALL_POS") {
			KifMap* kloc = NULL;
			for (int ipos = 1; ipos <= 5; ipos++) {
				SynsetPtr s_w;
				{
					ThreadLock _lock(kifTemporary);
					s_w = findtheinfo_ds(STR(lasttoken), ipos, wordnettypes[mode], wordnetsenses[sense]);
				}

				if (s_w != NULL) {
					kloc = new KifMap(kifcode, NULL);
					kres->Push(poswordnet[ipos], kloc);
					Setstructure(kloc, s_w, def);
					free_syns(s_w);
				}
			}
			return kres;
		}

		SynsetPtr s_w;
		{
			ThreadLock _lock(kifTemporary);
			s_w = findtheinfo_ds(STR(lasttoken), wordnetpos[pos], wordnettypes[mode], wordnetsenses[sense]);
		}

		if (s_w != NULL) {
			Setstructure(kres, s_w, def);
			free_syns(s_w);
		}

		//you may return any value of course...
		return kres;
	}

	KifElement* MethodFindsynset(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		string pos = callfunc->Evaluate(1, domain)->String();
		if (wordnetpos.find(pos) == wordnetpos.end()) {
			pos = "WRN(002): Unkwown POS:" + pos;
			return kifcode->Returnerror(pos);
		}

		long offset = callfunc->Evaluate(0, domain)->Integer();
		int def = callfunc->Evaluate(2, domain)->Integer();

		char* wrd = NULL;
		string lasttoken;
		if (callfunc->Size() == 4) {
			lasttoken = callfunc->Evaluate(3, domain)->String();
			wrd = STR(lasttoken);
		}

		KifMap* kres = Selectmap(callfunc, contextualpattern);
		if (pos == "ALL_POS") {
			KifMap* kloc = NULL;
			for (int ipos = 1; ipos <= 5; ipos++) {
				SynsetPtr s_w;
				{
					ThreadLock _lock(kifTemporary);
					s_w = read_synset(ipos, offset, wrd);
				}				
				if (s_w != NULL) {
					kloc = new KifMap(kifcode, NULL);
					Setstructure(kloc, s_w, def);
					kres->Push(poswordnet[ipos], kloc);
					free_syns(s_w);
				}
			}
			return kres;
		}



		SynsetPtr s_w;
		{
			ThreadLock _lock(kifTemporary);
			s_w = read_synset(wordnetpos[pos], offset, wrd);
		}
		if (s_w != NULL) {
			Setstructure(kres, s_w, def);
			free_syns(s_w);
		}
	

		return kres;
	}


	long Frequence(string& word, int pos) {
		int familiar = 0;
		IndexPtr idx = getindex(STR(word), pos);
		if (idx != NULL) {
			int cnt = idx->sense_cnt;
			if (cnt == 0) familiar = 0;
			if (cnt == 1) familiar = 1;
			if (cnt == 2) familiar = 2;
			if (cnt >= 3 && cnt <= 4) familiar = 3;
			if (cnt >= 5 && cnt <= 8) familiar = 4;
			if (cnt >= 9 && cnt <= 16) familiar = 5;
			if (cnt >= 17 && cnt <= 32) familiar = 6;
			if (cnt > 32) familiar = 7;
			free_index(idx);
		}
		return familiar;
	}

	KifElement* MethodFindfrequence(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		string pos = callfunc->Evaluate(1, domain)->String();
		if (wordnetpos.find(pos) == wordnetpos.end()) {
			pos = "WRN(002): Unkwown POS:" + pos;
			return kifcode->Returnerror(pos);
		}

		string word = callfunc->Evaluate(0, domain)->String();

		if (pos == "ALL_POS") {
			KifMap* kres = Selectmap(callfunc, contextualpattern);

			for (int ipos = 1; ipos <= 5; ipos++)
				kres->Push(poswordnet[ipos], kifcode->Provideinteger(Frequence(word, ipos)));

			return kres;
		}

		return kifcode->Provideinteger(Frequence(word, wordnetpos[pos]));
	}


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		KiFArityError(name)
			return (this->*linkedmethods[name])(contextualpattern, domain, callfunc, idthread);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	string String() {
		return "WordNet 3.0";
	}

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		string sa = a->String();
		string sb = b->String();
		sa += sb;
		return kifcode->Providestring(sa);
	}

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> KifWordnet::kifexportedmethods;
map<short, WordnetMethod>  KifWordnet::linkedmethods;
map<string, string> KifWordnet::infomethods;

static KifWordnet* local;

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifWordnetElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	local = new KifWordnet(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name, WordnetMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	KifWordnet::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	KifWordnet::linkedmethods[idname] = func;
	KifWordnet::infomethods[name] = infos;
}

static bool Createstatickifwordnet(KifCode* kifcode) {
	string varname("kifwordnet");
	short idname = KifNewId(varname);
	if (kifcode->gGlobals->kconstants->declarations.find(idname) == kifcode->gGlobals->kconstants->declarations.end()) {
		//First we create a variable
		KifVariableDeclaration* var = kifcode->Createstaticvariable(varname);
		//then we create its value
		local = new KifWordnet(kifcode, var);
		return true;
	}
	return false;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exported bool Init_kifwordnet(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}

		kifcode->KifInitLib();
		///The new type will be: Wordnet. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifWordnetElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: KifWordnet)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifWordnetElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("wordnet", CreatekifWordnetElement);
		if (!Createstatickifwordnet(kifcode))
			return false;

		Wordnet_type = res;
		return true;
	}

	Exported bool kifwordnet_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifwordnet(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization("findinfo", &KifWordnet::MethodFindinfos, P_FIVE, "findinfo(string word,string pos,string mode,string senses,int definitions): Retrieve all senses of a word according to the different flags. 0 is no definition and no list of POS. 1 adds the  list of POS. 2 adds the definitions. 3 all.");
		MethodInitialization("findsynset", &KifWordnet::MethodFindsynset, P_THREE | P_FOUR, "findsynset(int synset,string pos,int definition,string word): Retrieve all senses associated with a synset. 'word' is optionnal. 0 is no definition and no list of POS. 1 adds the  list of POS. 2 adds the definitions. 3 all.");
		MethodInitialization("frequency", &KifWordnet::MethodFindfrequence, P_TWO, "frequency(string word,string pos): Return the 'frequency' of a word as a value between 0..7, 0 being the less frequent");
		Createstatickifwordnet(kifcode);
		return true;
	}

	Exported bool kifwordnet_Terminate(KifCode* kifcode) {
		KifWordnet::kifexportedmethods.clear();
		KifWordnet::linkedmethods.clear();
		KifWordnet::infomethods.clear();
		return true;
	}
}
