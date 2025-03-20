/* --- CONTENTS ---
Project    : KIFF
Version    : 1.93
filename   : crfsuite.cxx
Date       : 2015/09/04
Purpose    : Automatically Generated
Programmer : Le GRAC
*/

#include <os.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "option.h"
#include <crfsuite.h>
#include "kifversion.h"
#include "kif.h"
#include "kifcontainer.h"

#ifdef WIN32
#define Exporting __declspec(dllexport)
#endif

#define    APPLICATION_S    "CRFSuite"

extern "C" {
	int crfsuite_call(int argc, char *argv[], tagger_option_t* opt);
	int crfsuite_tagging(char* filename, char* outputfilename, tagger_option_t* opt);
	void crf_suite_closing_model(tagger_option_t* opt);
	void tagger_option_init(tagger_option_t* opt);
	void tagger_option_finish(tagger_option_t* opt);
	int crfsuite_tagging_string(char* astream, int sz, tagger_option_t* opt);
}

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType crfsuite_type;
short KifNewId(string n);
string KifIdString(short n);

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Kifcrfsuite;
//This typedef defines a type "crfsuiteMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifcrfsuite::*crfsuiteMethod)(KifElement*, KifDomain*, KifCallFunction*, int idthread);

class Kifcrfsuite : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, crfsuiteMethod> linkedmethods;
	static hmap<short, bool> methodassigned;	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	char* args[100];
	KifVector* kvectoutput;
	tagger_option_t opt;
	string separator;
	string endoftagging;

	//---------------------------------------------------------------------------------------------------------------------
	Kifcrfsuite(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, crfsuite_type) {
		//Do not forget your variable initialisation
		separator = "#";
		endoftagging = "$";
		kvectoutput = NULL;
		tagger_option_init(&opt);
		opt.caller = (void*)this;
		for (int i = 0; i < 100; i++)
			args[i] = (char*)malloc(1024);
	}

	~Kifcrfsuite() {
		crf_suite_closing_model(&opt);
		tagger_option_finish(&opt);
		for (int i = 0; i < 100; i++)
			free(args[i]);
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
		return crfsuite_type;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		Kifcrfsuite* kperso = new Kifcrfsuite(kifcode, NULL);
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

		//Do not forget to release them, if they come from a vector of string for instance... They might have been temporarily created...
		kfirst->Release();
		ksecond->Release();
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
	void Methods(KifVectorString* fcts) {
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
	KifElement* MethodLearn(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		KifElement* first = callfunc->Evaluatethread(0, domain, idthread);
		if (first->aVectorContainer() == false)
			return kifcode->Returnerror("CST(001): Expecting a vector for options as input");


		for (int i = 0; i < 100; i++)
			args[i][0] = 0;

		int argc = 2;
		strcpy_s(args[0], 100, "crfsuite");
		strcpy_s(args[1], 100, "learn");
		string val;
		for (int i = 0; i < first->Size(); i++) {
			val = first->getstring(i);
			strcpy_s(args[i + 2], 100, STR(val));
			++argc;
		}

		crfsuite_call(argc, args, &opt);
		//you may return any value of course...
		return kifTRUE;
	}

	KifElement* MethodLoad(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		KifElement* first = callfunc->Evaluatethread(0, domain, idthread);
		if (first->aVectorContainer() == false)
			return kifcode->Returnerror("CST(001): Expecting a vector for options as input");


		for (int i = 0; i < 100; i++)
			args[i][0] = 0;

		int argc = 2;
		strcpy_s(args[0], 100, "crfsuite");
		strcpy_s(args[1], 100, "tag");
		string val;
		for (int i = 0; i < first->Size(); i++) {
			val = first->getstring(i);
			strcpy_s(args[i + 2], 100, STR(val));
			++argc;
		}

		if (!crfsuite_call(argc, args, &opt))
			return kifTRUE;

		//you may return any value of course...
		return kifcode->Returnerror("CST(005): Error while processing model");
	}

	KifElement* MethodTag(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (opt.themodel == NULL)
			return kifcode->Returnerror("CST(004): Missing model");

		opt.store_in_struct = 1;
		if (callfunc->Size() >= 1) {
			string name = callfunc->Evaluatethread(0, domain, idthread)->String();
			strcpy_s(args[0], 1024, STR(name));
			if (callfunc->Size() == 2) {
				opt.store_in_struct = 0;
				name = callfunc->Evaluatethread(1, domain, idthread)->String();
				strcpy_s(args[1], 1024, STR(name));
				if (!crfsuite_tagging(args[0], args[1], &opt))
					return kifTRUE;
				return kifcode->Returnerror("CST(006): Error while processing tagging file");
			}
			kvectoutput = new KifVector(kifcode, NULL);
			crfsuite_tagging(args[0], NULL, &opt);
		}
		else {
			kvectoutput = new KifVector(kifcode, NULL);
			crfsuite_tagging(NULL, NULL, &opt);
		}
		return kvectoutput;
	}

	KifElement* MethodSTag(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (opt.themodel == NULL)
			return kifcode->Returnerror("CST(004): Missing model");

		opt.store_in_struct = 1;
		kvectoutput = new KifVector(kifcode, NULL);
		KifElement* kinput = callfunc->Evaluatethread(0, domain, idthread);
		if (kinput->aVectorContainer() == false)
			return kifcode->Returnerror("CST(003): Expecting a vector input");
		string content;
		for (int i = 0; i < kinput->Size(); i++) {
			content += kinput->getstring(i);
			content += "\n";
		}
		crfsuite_tagging_string(STR(content), content.size(), &opt);
		return kvectoutput;
	}

	KifElement* MethodClose(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		crf_suite_closing_model(&opt);
		opt.themodel = NULL;
		return kifTRUE;
	}

	KifElement* MethodSeps(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		separator = callfunc->Evaluatethread(0, domain, idthread)->String();
		endoftagging = callfunc->Evaluatethread(1, domain, idthread)->String();
		return kifTRUE;
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
map<short, KifCallMethod> Kifcrfsuite::kifexportedmethods;
map<short, crfsuiteMethod>  Kifcrfsuite::linkedmethods;
map<string, string> Kifcrfsuite::infomethods;
hmap<short, bool> Kifcrfsuite::methodassigned;

//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifcrfsuiteElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifcrfsuite* local = new Kifcrfsuite(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name, crfsuiteMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifcrfsuite::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifcrfsuite::linkedmethods[idname] = func;
	Kifcrfsuite::methodassigned[idname] = true;
	Kifcrfsuite::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	void kif_storing_double(void* call, const char* label, double v);
	void kif_storing_label(void* call, const char* label);
	void kif_storing_label_sep(void* call, const char* label);
	void kif_storing_next(void* call);
	void kif_storing_next_final(void* call);
}

void kif_storing_double(void* call, const char* label, double v) {
	Kifcrfsuite* kcrf = (Kifcrfsuite*)call;
	KifString* kstr = kcrf->kifcode->Providestringraw(label);
	KifFloat* kfloat = kcrf->kifcode->Providefloat(v);
	kcrf->kvectoutput->Push(kstr);
	kcrf->kvectoutput->Push(kfloat);
}

void kif_storing_label_sep(void* call, const char* label) {
	Kifcrfsuite* kcrf = (Kifcrfsuite*)call;
	KifString* kstr = kcrf->kifcode->Providestringraw(label);
	kstr->value += "\t";
	kcrf->kvectoutput->Push(kstr);
}

void kif_storing_label(void* call, const char* label) {
	Kifcrfsuite* kcrf = (Kifcrfsuite*)call;
	KifString* kstr = kcrf->kifcode->Providestringraw(label);
	kcrf->kvectoutput->Push(kstr);
}

void kif_storing_next(void* call) {
	Kifcrfsuite* kcrf = (Kifcrfsuite*)call;
	KifString* kstr = kcrf->kifcode->Providestring(kcrf->separator);
	kcrf->kvectoutput->Push(kstr);
}

void kif_storing_next_final(void* call) {
	Kifcrfsuite* kcrf = (Kifcrfsuite*)call;
	KifString* kstr = kcrf->kifcode->Providestring(kcrf->endoftagging);
	kcrf->kvectoutput->Push(kstr);
}

extern "C" {
	Exporting bool Init_kifcrfsuite(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}
		kifcode->KifInitLib();

		///The new type will be: crfsuite. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifcrfsuiteElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifcrfsuite)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifcrfsuiteElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("crfsuite", CreatekifcrfsuiteElement);
		if (res == kifVOID)
			return false;

		crfsuite_type = res;
		return true;
	}

	Exporting bool kifcrfsuite_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifcrfsuite(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization("learn", &Kifcrfsuite::MethodLearn, P_ONE, "learn(svector args): Launching learning");
		MethodInitialization("tagfile", &Kifcrfsuite::MethodTag, P_NONE | P_ONE | P_TWO, "tagfile(string filename,string outputfile): tagging a file.");
		MethodInitialization("tag", &Kifcrfsuite::MethodSTag, P_ONE, "tag(svector tokens): tagging a vector of tokens.");
		MethodInitialization("load", &Kifcrfsuite::MethodLoad, P_ONE, "load(svector args): loading a model.");
		MethodInitialization("close", &Kifcrfsuite::MethodClose, P_NONE, "close(): closing a model.");
		MethodInitialization("separators", &Kifcrfsuite::MethodSeps, P_TWO, "separators(string separator,string endoftagging): setting how the fields will be separated.");

		kifGlobals->kifAllFunctions[crfsuite_type] = Kifcrfsuite::methodassigned;
		kifGlobals->kifAllInformation[crfsuite_type] = Kifcrfsuite::infomethods;
		return true;
	}

	Exporting bool kifcrfsuite_Terminate(KifCode* kifcode) {
		Kifcrfsuite::kifexportedmethods.clear();
		Kifcrfsuite::linkedmethods.clear();
		Kifcrfsuite::infomethods.clear();
		Kifcrfsuite::methodassigned.clear();
		return true;
	}
}






