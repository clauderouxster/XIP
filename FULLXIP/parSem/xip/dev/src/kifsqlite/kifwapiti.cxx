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
filename   : kiftemplate.cxx
Date       : 09/09/2010
Purpose    : A KiF library template
Programmer : Claude ROUX
Reviewer   :
*/

//Use KIF_DLL only in Windows, if you link to the KiF DLL
//#define KIF_DLL
#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"

#ifdef WIN32
#define Exporting __declspec(dllexport)
#endif

extern "C" {
#include "options.h"
	char* CRFError();
	void DoTrain(opt_t* opt);
	void DoLabel(opt_t* opt);
	void WAPITIInit();
	char* WAPITILastError();
}

//Replace Wapiti by your own definition
short KifNewId(string n);
string KifIdString(short n);

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType Wapiti_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class KifWapiti;
//This typedef defines a type "WapitiMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (KifWapiti::*WapitiMethod)(KifElement*, KifDomain*, int, KifCallFunction*);

extern "C" {
#ifdef CRFTAGGER
	void* CreateTagger(char* crfmodel, char* crfbrown, char* crfmkcls);
	int TaggerError(void* tg);
	int TaggingTokens(void* tg, char** wordSequence, char** predictedTag, int nb);
	void CleanTags(char** tags, int lp);
	void CleanTagger(void* tg);
#endif

	void* CreatePureTagger(opt_t* o);
	void CleanPureTagger(void* pt);
	bool LabelPureTagger(void* tp, char** words, char** results, int sz);
	bool InitPureTagger(void* tp);
}

extern "C" {
	static void initopt(opt_t& opt) {
		opt.mode = -1;
		opt.input = NULL;
		opt.output = NULL;
		opt.type = "crf";
		opt.maxent = false;
		opt.algo = "l-bfgs";
		opt.pattern = NULL;
		opt.model = NULL;
		opt.devel = NULL;
		opt.rstate = NULL;
		opt.sstate = NULL;
		opt.compact = false;
		opt.sparse = false;
		opt.nthread = 1;
		opt.jobsize = 64;
		opt.maxiter = 0;
		opt.rho1 = 0.5;
		opt.rho2 = 0.0001;
		opt.objwin = 5;
		opt.stopwin = 5;
		opt.stopeps = 0.02;
		opt.lbfgs.clip = false;
		opt.lbfgs.histsz = 5;
		opt.lbfgs.maxls = 40;
		opt.sgdl1.eta0 = 0.8;
		opt.sgdl1.alpha = 0.85;
		opt.bcd.kappa = 1.5;
		opt.rprop.stpmin = 1e-8;
		opt.rprop.stpmax = 50.0;
		opt.rprop.stpinc = 1.2;
		opt.rprop.stpdec = 0.5;
		opt.rprop.cutoff = false;
		opt.label = false;
		opt.check = false;
		opt.outsc = false;
		opt.lblpost = false;
		opt.nbest = 1;
		opt.force = false;
		opt.prec = 5;
		opt.all = false;
	}
}

class KifWapiti : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, WapitiMethod> linkedmethods;
	static hmap<short, bool> methodassigned;	static map<string, string> infomethods;


	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	string crfmodel;
#ifdef CRFTAGGER
	string brown;
	string mkcls;
#endif
	void* crftagger;
	char loadingmode;
	opt_t opt;
	char* args[100];

	//---------------------------------------------------------------------------------------------------------------------
	KifWapiti(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, Wapiti_type) {
		//Do not forget your variable initialisation
		crftagger = NULL;
		loadingmode = 0;
		initopt(opt);
		for (int i = 0; i < 100; i++)
			args[i] = (char*)malloc(1024);
		WAPITIInit();
	}

	~KifWapiti() {
		for (int i = 0; i < 100; i++)
			free(args[i]);
#ifdef CRFTAGGER
		if (crftagger != NULL) {
			if (loadingmode == 3)
				CleanTagger(crftagger);
			else
			if (loadingmode == 2)
				CleanPureTagger(crftagger);
		}
#else
		if (loadingmode == 2)
			CleanPureTagger(crftagger);
#endif
	}


	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		return true;
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return Wapiti_type;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifWapiti* kperso = new KifWapiti(KifCurrent(), NULL);
		//Your initialisations here
		return kperso;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	bool initialconstructor() {
		return false;
	}

	//If initialconstructor returns true, then you might want to benefit from these parameters
	KifElement* constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize() == 0)
			return this;

		//Your first parameter is at position 0 etc...
		KifElement* kfirst = params->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc); //would match i as in the example above: mynewtype toto(i,j);
		KifElement* ksecond = params->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc); //would match j as in the example above: mynewtype toto(i,j);
		//etc...
		return this;
	}

	//-----------------------------------------------------------------------------------------------------------------------
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
	void Methods(KifVector* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->Push(kifcode->Providestringraw(KifIdString(it->first)));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestringraw(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodLoadModel(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (loadingmode)
			return kifcode->Returnerror("WPT(004): Model already loaded");
		string model = callfunc->Evaluatethread(0, domain, _thread)->String();
		for (int i = 0; i < 100; i++)
			args[i][0] = 0;

		strcpy(args[0], "wapiti");
		strcpy(args[1], "label");
		strcpy(args[2], "-m");
		strcpy(args[3], STR(model));
		initopt(opt);
		opt_parse(4, args, &opt);
		char* erreur = CRFError();
		if (erreur[0] != 0) {
			model = "WPT(099): ";
			model += erreur;
			return kifcode->Returnerror(model);
		}

		loadingmode = 1;
		return kifTRUE;
	}

	KifElement* MethodOptions(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (loadingmode)
			return kifcode->Returnerror("WPT(004): Model already loaded");

		KifElement* koptions = callfunc->Evaluatethread(0, domain, _thread);
		if (!koptions->aVectorContainer())
			return kifcode->Returnerror("WPT(014): Expecting a vector container");

		int i;
		for (i = 0; i < 100; i++)
			args[i][0] = 0;

		KifIteration* itr = (KifIteration*)koptions->Newiterator(false);
		string s;
		i = 0;
		strcpy(args[i++], "wapiti");
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			s = itr->IteratorValueString();
			strcpy(args[i++], STR(s));
		}
		itr->Release();
		initopt(opt);
		opt_parse(i, args, &opt);
		char* erreur = CRFError();
		if (erreur[0] != 0) {
			string model = "WPT(099): ";
			model += erreur;
			return kifcode->Returnerror(model);
		}
		loadingmode = 1;
		return kifTRUE;
	}

	KifElement* MethodTraining(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (!loadingmode || opt.mode != 0)
			return kifcode->Returnerror("WPT(021): No options provided");
		DoTrain(&opt);
		return kifTRUE;
	}

	KifElement* MethodLasterror(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		string s = WAPITILastError();
		return kifcode->Providestring(s);
	}

	KifElement* MethodLabelling(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (!loadingmode || opt.mode == 0)
			return kifcode->Returnerror("WPT(022): No options provided");

		WAPITIInit();

		KifElement* kwords = callfunc->Evaluatethread(0, domain, _thread);
		if (!kwords->aVectorContainer())
			return kifcode->Returnerror("WPT(015): Expecting a vector container");

		int sz = kwords->Size();
		if (sz == 0)
			return kifFALSE;

		if (crftagger == NULL) {
			crftagger = CreatePureTagger(&opt);
			loadingmode = 2;
			if (!InitPureTagger(crftagger))
				return kifFALSE;
		}

		char** wordSequence = new char*[sz + 1];
		char** predictedTag = new char*[sz + 1];
		vector<string> wrds;
		int i;
		for (i = 0; i < sz; i++) {
			wrds.push_back(kwords->ValueOnIndex(i)->String());
			wordSequence[i] = strdup(STR(wrds[i]));
			predictedTag[i] = NULL;
		}

		if (!LabelPureTagger(crftagger, wordSequence, predictedTag, sz))
			return kifFALSE;

		kwords = Selectvectorstring(callfunc, contextualpattern);
		string v;
		for (i = 0; i < sz; i++)
			kwords->storevalue(predictedTag[i]);

		delete[] wordSequence;
		delete[] predictedTag;
		return kwords;
	}

#ifdef CRFTAGGER
	KifElement* MethodLoadXIP(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		if (crftagger != NULL)
			return kifcode->Returnerror("WPT(004): Model already loaded");

		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		crfmodel = callfunc->Evaluatethread(0, domain, _thread)->String();
		brown = callfunc->Evaluatethread(1, domain, _thread)->String();
		mkcls = callfunc->Evaluatethread(2, domain, _thread)->String();

		crftagger = CreateTagger(STR(crfmodel), STR(brown), STR(mkcls));
		loadingmode = 3;
		int errtagger = TaggerError(crftagger);
		switch (errtagger) {
		case 1:
			crftagger = NULL;
			return kifcode->Returnerror("WPT(001): Error while reading BROWN file");
		case 2:
			crftagger = NULL;
			return kifcode->Returnerror("WPT(002): Error while reading MKCLS file");
		case 3:
			crftagger = NULL;
			return kifcode->Returnerror("WPT(003): Error while reading MODEL file");
		}
		//you may return any value of course...
		return kifTRUE;
	}

	KifElement* MethodAnalysis(KifElement* contextualpattern, KifDomain* domain, int _thread, KifCallFunction* callfunc) {
		KifElement* kwords = callfunc->Evaluatethread(0, domain, _thread);
		if (!kwords->aVectorContainer())
			return kifcode->Returnerror("WPT(005): Expecting a vector of strings as input");

		int sz = kwords->Size();
		if (sz == 0)
			return kifFALSE;
		char** wordSequence = new char*[sz + 1];
		char** predictedTag = new char*[sz + 1];
		vector<string> wrds;
		int i;
		for (i = 0; i < sz; i++) {
			wrds.push_back(kwords->ValueOnIndex(i)->String());
			wordSequence[i] = STR(wrds[i]);
		}
		int nbelements = TaggingTokens(crftagger, wordSequence, predictedTag, sz);
		kwords = Selectvectorstring(callfunc, contextualpattern);
		string v;
		for (i = 0; i < nbelements; i++)
			kwords->storevalue(predictedTag[i]);

		CleanTags(predictedTag, nbelements);
		delete[] wordSequence;
		delete[] predictedTag;
		return kwords;
	}
#endif

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		KiFArityError(name)
			return (this->*linkedmethods[name])(contextualpattern, domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return ""; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

	//You can overcome the following function behaviour
	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		return kifNULL;
	}
	//union of two sets
	KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return kifNULL;
	}

	KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return kifNULL;
	}

	//intersection of two sets
	KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return kifNULL;
	}

	//Beware that a might be the current element
	//plus: a+b
	KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return kifNULL;
	}

	//minus: a-b
	KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return kifNULL;
	}

	//Multiplication: a*b
	KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return kifNULL;
	}

	//Division: a/b
	KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return kifNULL;
	}
	//This is the division integer rest: a%b
	KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return kifNULL;
	}
	//This is a binary right shift: a>>b
	KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return kifNULL;
	}
	//This is a binary left shift: a<<b
	KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return kifNULL;
	}
	//a to the power of b: a^b
	KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return kifNULL;
	}

	//Comparison functions that might also be modified
	//inferior: this<a
	KifElement* less(KifElement* a) {
		return kifFALSE;
	}
	//superior: this>a
	KifElement* more(KifElement* a) {
		return kifFALSE;
	}

	//equality: this==a
	KifElement* same(KifElement* a) {
		if (a == this)
			return kifTRUE;
		return kifFALSE;
	}

	//different: this!=a
	KifElement* different(KifElement* a) {
		if (a != this)
			return kifTRUE;
		return kifFALSE;
	}

	//inferior or equal: this<=a
	KifElement* lessequal(KifElement* a) {
		return kifFALSE;
	}

	//superior or equal: this>=a
	KifElement* moreequal(KifElement* a) {
		return kifFALSE;
	}

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> KifWapiti::kifexportedmethods;
map<short, WapitiMethod>  KifWapiti::linkedmethods;
map<string, string> KifWapiti::infomethods;
hmap<short, bool> KifWapiti::methodassigned;



//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifWapitiElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	KifWapiti* local = new KifWapiti(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name, WapitiMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	KifWapiti::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	KifWapiti::linkedmethods[idname] = func;
	KifWapiti::methodassigned[idname] = true;
	KifWapiti::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kifwapiti(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}
		kifcode->KifInitLib();
		///The new type will be: Wapiti. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifWapitiElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: KifWapiti)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifWapitiElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.		
		KifType res = KifAddNewType("wapiti", CreatekifWapitiElement);
		if (res == kifVOID)
			return false;
		Wapiti_type = res;
		return true;
	}

	Exporting bool kifwapiti_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifwapiti(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
#ifdef CRFTAGGER
		MethodInitialization("loadxip", &KifWapiti::MethodLoadXIP, P_ONE | P_THREE, "loadxip(string crfmodel, string brown,string mkcls): Loading the CRF files for tweet tagging.");
		MethodInitialization("analyze", &KifWapiti::MethodAnalysis, P_ONE, "analyze(svector words): Analyze a sequence of words and returns the predicted tags.");
#endif
		MethodInitialization("loadmodel", &KifWapiti::MethodLoadModel, P_ONE, "loadmodel(string crfmodel): Loading a CRF model.");
		MethodInitialization("options", &KifWapiti::MethodOptions, P_ONE, "options(map options): Setting options.");
		MethodInitialization("train", &KifWapiti::MethodTraining, P_NONE, "train(): Launch training.");
		MethodInitialization("label", &KifWapiti::MethodLabelling, P_ONE, "label(vector words): Launch labelling.");
		MethodInitialization("lasterror", &KifWapiti::MethodLasterror, P_NONE, "lasterror(): Return the last error.");

		kifGlobals->kifAllFunctions[Wapiti_type] = KifWapiti::methodassigned;
		kifGlobals->kifAllInformation[Wapiti_type] = KifWapiti::infomethods;
		return true;
	}

	Exporting bool kifwapiti_Terminate(KifCode* kifcode) {
		KifWapiti::kifexportedmethods.clear();
		KifWapiti::linkedmethods.clear();
		KifWapiti::methodassigned.clear();
		KifWapiti::infomethods.clear();
		return true;
	}
}




