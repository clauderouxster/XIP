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
filename   : kifmp3.cxx
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

#ifdef MACSOUND
typedef unsigned int SystemSoundID;
typedef void(*AudioServicesSystemSoundCompletionProc)(SystemSoundID  mySSID, void* myself);

bool createSoundId(const char* thepath, SystemSoundID* _soundId);
void cleanSoundId(SystemSoundID*);
void playSoundId(SystemSoundID*);
void addSoundFunction(SystemSoundID* soundid, AudioServicesSystemSoundCompletionProc c, void* data);
void removeSoundFunction(SystemSoundID* soundid);
void completionCallback(SystemSoundID  mySSID, void* myself);
#endif

#ifdef WIN32
#include "Mp3.h"
#endif


//Replace sound by your own definition
short KifNewId(string n);
string KifIdString(short n);

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType sound_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class kifmp3;
//This typedef defines a type "soundMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (kifmp3::*soundMethod)(KifElement*, KifDomain*, KifCallFunction*, int);


class kifmp3 : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, soundMethod> linkedmethods;
	static hmap<short, bool> methodassigned;	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
#ifdef MACSOUND
	SystemSoundID sound;
#endif
#ifdef WIN32
	Mp3 sound;
#endif

	string path;
	bool loaded;
	bool created;

	KifElement* function;
	KifDomain* dom;
	int idom;
	int ifunc;


	//---------------------------------------------------------------------------------------------------------------------
	kifmp3(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, sound_type) {
		function = base->Robfunction(this);
		ifunc = -1;
		if (function != NULL) {
			function->Setreference();
			ifunc = function->Ingarbage();
		}
		loaded = false;
		created = false;
		dom = NULL;
		idom = -1;
	}

	~kifmp3() {
#ifdef MACSOUND
		if (created) {
			if (function != NULL)
				removeSoundFunction(&sound);
			cleanSoundId(&sound);
		}
#endif
#ifdef WIN32
		sound.Cleanup();
#endif
		if (function != NULL) {
			kifGlobals->ResetGarbageElement(dom, idom);
			kifGlobals->ResetGarbageElement(function, ifunc);
		}
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
		return sound_type;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* doma = kifNULL) {
		kifmp3* kperso = new kifmp3(KifCurrent(), this);
		kperso->sound = sound;
		kperso->loaded = loaded;

		kperso->function = function;
		kperso->ifunc = ifunc;
		kperso->dom = dom;
		kperso->idom = idom;
		return kperso;
	}

	//-----------------------------------------------------------------------------------------------------------------------
#ifdef MACSOUND
	//this function is called to test the kind of function we expect from the library with the "with" operator...
	bool testfunction(KifElement* func) {
		if (func->type == kifFunction) {
			KifFunction* kfunction = (KifFunction*)func;
			if (kfunction->arguments.size() != 1)
				return false;
			if (kfunction->arguments[0]->VariableValue()->inType() != sound_type)
				return false;
			return true;
		}
		return false;
	}
#endif

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	Exported bool initialconstructor() {
		return true;
	}

	//If initialconstructor returns true, then you might want to benefit from these parameters
	Exported KifElement* constructor(KifDomain* domloc, KifElement* params, int idthread, KifCallFunction* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize() == 0)
			return this;

		KifElement* kpath = params->Instruction(0)->Exec(kifNULL, domloc, kifNULL, idthread, callfunc); //would match i as in the example above: mynewtype toto(i,j);
		path = kpath->String();
		kpath->Release();

#ifdef MACSOUND
		if (createSoundId(path.c_str(), &sound) == false)
			return domloc->kifcode->Returnerror("SND(001): Unknown file");
		if (function != NULL)
			addSoundFunction(&sound, completionCallback, (void*)this);
#endif
#ifdef WIN32
		wstring base;
		s_utf8_to_unicode(base, (char*)path.c_str());
		sound.Load((WCHAR*)base.c_str());
#endif
		dom = domloc;
		idom = dom->Ingarbage();
		dom->Setreference();

		loaded = true;
		created = true;
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
			fcts->values.push_back(KifIdString(it->first));
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
	KifElement* MethodLoad(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
#ifdef MACSOUND
		if (created) {
			if (function != NULL)
				removeSoundFunction(&sound);
			cleanSoundId(&sound);
		}
#endif
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		path = callfunc->Evaluatethread(0, domain, idthread)->String();
#ifdef MACSOUND
		if (createSoundId(path.c_str(), &sound) == false)
			return callfunc->kifcode->Returnerror("SND(001): Unknown file");
		if (function != NULL)
			addSoundFunction(&sound, completionCallback, (void*)this);
#endif

#ifdef WIN32
		wstring base;
		s_utf8_to_unicode(base, (char*)path.c_str());
		if (sound.Load((WCHAR*)base.c_str()) == false)
			return callfunc->kifcode->Returnerror("SND(005): Error during loading");
#endif
		loaded = true;
		created = true;
		return kifTRUE;
	}

	KifElement* MethodPlay(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!loaded)
			return callfunc->kifcode->Returnerror("SND(003): Please load a file first");
#ifdef MACSOUND
		playSoundId(&sound);
#endif
#ifdef WIN32
		sound.Play();
#endif
		return kifTRUE;
	}

	KifElement* MethodStop(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!loaded)
			return callfunc->kifcode->Returnerror("SND(003): Please load a file first");
#ifdef MACSOUND
		if (created) {
			if (function != NULL)
				removeSoundFunction(&sound);
			cleanSoundId(&sound);
			if (createSoundId(path.c_str(), &sound) == false)
				return callfunc->kifcode->Returnerror("SND(001): Unknown file");
			if (function != NULL)
				addSoundFunction(&sound, completionCallback, (void*)this);
		}
#endif
#ifdef WIN32
		sound.Stop();
#endif
		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFCURLSTRINGS[2]
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

	//If you need to return a specific size for your object (the KIFCURLSTRINGS[3] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return path; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> kifmp3::kifexportedmethods;
map<short, soundMethod>  kifmp3::linkedmethods;
map<string, string> kifmp3::infomethods;
hmap<short, bool> kifmp3::methodassigned;

//-----------------------------------------------------------------------------------------------------------------------
#ifdef MACSOUND
void completionCallback(SystemSoundID  mySSID, void* data) {
	kifmp3* ksnd = (kifmp3*)data;
	KifFunction* func = ksnd->function->Functioncall(ksnd->dom);
	KifCallFunctionLocal kfunc(func->kifcode, func->Name(), func);
	ksnd->Setreference();
	kfunc.parameters.push_back(ksnd);
	KifElement* ret = kfunc.Execute(kifNULL, ksnd->dom, kifNULL);
	ksnd->Resetreference();
	ret->Release();
}
#endif
//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* Createkifmp3Element(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	kifmp3* local = new kifmp3(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(KifCode* kifcode, string name, soundMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	kifmp3::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	kifmp3::linkedmethods[idname] = func;
	kifmp3::methodassigned[idname] = true;
	kifmp3::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exported bool Init_kifmp3(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}

		kifcode->KifInitLib();

		if (kifGlobals->TestExternalType("mp3") == true)
			return false;

		///The new type will be: sound. You will be able to declare any variable with that type, once this library has been loaded...
		//Createkifmp3Element will be invoked by the KiF compiler to create the right sort of object for this type (in our example: kifmp3)
		//Whenever the keyword testage will be found by KiF it will then call Createkifmp3Element, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("mp3", Createkifmp3Element);
		if (res == kifVOID)
			return false;
		sound_type = res;
		return true;
	}

	Exported bool kifmp3_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifmp3(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization(kifcode, "load", &kifmp3::MethodLoad, P_ONE, "load(string pathname): Load the sound pathname.");
		MethodInitialization(kifcode, "play", &kifmp3::MethodPlay, P_NONE, "play(): play the sound");
		MethodInitialization(kifcode, "stop", &kifmp3::MethodStop, P_NONE, "stop(): stop the sound");

		kifGlobals->kifAllFunctions[sound_type] = kifmp3::methodassigned;
		kifGlobals->kifAllInformation[sound_type] = kifmp3::infomethods;
		return true;
	}

	Exported bool kifmp3_Terminate(KifCode* kifcode) {
		kifmp3::kifexportedmethods.clear();
		kifmp3::linkedmethods.clear();
		kifmp3::methodassigned.clear();
		kifmp3::infomethods.clear();
		return true;
	}
}




