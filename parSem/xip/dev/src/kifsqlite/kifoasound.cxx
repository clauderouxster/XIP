/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the XIP library, 
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : KIFF
Version    : 10.00
filename   : kifsound.cxx
Date       : 09/09/2010
Purpose    : Sound library template for KiF on raspberry pi
Programmer : Claude ROUX
Reviewer   : 
*/

#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"

//YOU NEED TO DEFINE: SOUND on the compiling line

//We use libao and libmpg123 for this
//sudo apt-get install libao-dev
//sudo apt - get install libmpg123-devel

//Based on an example found on: http://hzqtc.github.io/2012/05/play-mp3-with-libmpg123-and-libao.html

#include <ao/ao.h>
#include <mpg123.h>

#define BITS 8

//Replace sound by your own definition
short KifNewId(string n);
string KifIdString(short n);

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType sound_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Kifsound;
//This typedef defines a type "soundMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifsound::*soundMethod)(KifElement*,KifDomain*,KifCallFunction*);


static void intialization() {
	static bool init = false;
	if (!init) {
		ao_initialize();
		mpg123_init();
		init = true;
	}
}

class Kifsound : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short,KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short,soundMethod> linkedmethods;
	static map<string,string> infomethods;

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
	//Your personal variables here...
	//ao variable
	mpg123_handle *mh;
	unsigned char *buffer;
	size_t buffer_size;
	size_t done;
	
	ao_sample_format format;
	int channels, encoding;
	long rate;

	int driver;
	int err;

	//KiF variable
    string path;
    bool loaded;
	bool stop;


//---------------------------------------------------------------------------------------------------------------------
	Kifsound(KifCode* kifcode,KifElement* base) : KifBasic(kifcode,base,sound_type) {
		intialization();
        loaded=false;
		stop = false;
		
		driver = ao_default_driver_id();
		mh = mpg123_new(NULL, &err);
		buffer_size = mpg123_outblock(mh);
		buffer = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));
	}

	~Kifsound() {
		free(buffer);		
		mpg123_close(mh);
		mpg123_delete(mh);

        if (function!=NULL) {
            kifGlobals->ResetGarbageElement(dom,idom);
            kifGlobals->ResetGarbageElement(function,ifunc);
        }
    }

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval,KifElement* kindex,KifElement* domain) {
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
	KifElement* Copy(KifDomain* kp,KifElement* doma=kifNULL) {
		return this;
	}

//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	Exported bool initialconstructor() {
		return true;
	}
	
	//If initialconstructor returns true, then you might want to benefit from these parameters
	Exported KifElement* constructor(KifDomain* domloc,KifElement* params) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize()==0)
			return this;

		path=params->Instruction(0)->Execute(kifNULL,domloc)->String(); 
		
		if (mpg123_open(mh, STR(path)) != MPG123_OK)
			return kifcode->Returnerror("SND(003): Erreur au chargement")

		if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK)
			return kifcode->Returnerror("SND(004): Format inconnu")

		/* set the output format and open the output device */
		format.bits = mpg123_encsize(encoding) * BITS;
		format.rate = rate;
		format.channels = channels;
		format.byte_format = AO_FMT_NATIVE;
		format.matrix = 0;
		
		loaded=true;
		return this;
	}

//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n)!=kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short,KifCallMethod>::iterator it;
		for (it=kifexportedmethods.begin();it!=kifexportedmethods.end();it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s=kstr->String();
		if (infomethods.find(s)==infomethods.end())
			return kifNULL;
		return kifcode->Providestringraw(infomethods[s]);
	}

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodLoad(KifElement* contextualpattern,KifDomain* domain,KifCallFunction* callfunc) {
		if (loaded)
			return kifcode->Returnerror("SND(001): Déjà chargé")

		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		path=callfunc->Evaluate(0,domain)->String();
		
		if (mpg123_open(mh, STR(path)) != MPG123_OK) 
			return kifcode->Returnerror("SND(003): Erreur au chargement")

		if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK)
			return kifcode->Returnerror("SND(004): Format inconnu")

		/* set the output format and open the output device */
		format.bits = mpg123_encsize(encoding) * BITS;
		format.rate = rate;
		format.channels = channels;
		format.byte_format = AO_FMT_NATIVE;
		format.matrix = 0;
		
        loaded=true;        
        return kifTRUE;
	}

	KifElement* MethodPlay(KifElement* contextualpattern,KifDomain* domain,KifCallFunction* callfunc) {
        if (!loaded)
            return callfunc->kifcode->Returnerror("SND(002): Veuillez charger votre fichier d'abord");

		stop = false;
		ao_device *dev = ao_open_live(driver, &format, NULL);

		/* decode and play */
		while (!stop && mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
			ao_play(dev, buffer, done);

		ao_close(dev);

        return kifTRUE;		 
	}

	KifElement* MethodStop(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc) {
		if (!loaded)
			return callfunc->kifcode->Returnerror("SND(002): Veuillez charger votre fichier d'abord");

		stop = true;
		return kifTRUE;
	}
//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name,KifElement* contextualpattern,KifDomain* domain,int,KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFCURLSTRINGS[2]
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        KiFArityError(name)
		return (this->*linkedmethods[name])(contextualpattern,domain,callfunc);
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
	long Size() {return 0;}

	//How to interprete your class according to the basic following types:
	string String() {return path;}
	long Integer() {return 0;}
	double Float() {return 0;}
	bool Boolean() {return true;}

};
//We need to declare once again our local definitions.
map<short,KifCallMethod> Kifsound::kifexportedmethods;
map<short,soundMethod>  Kifsound::linkedmethods;
map<string,string> Kifsound::infomethods;

//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifsoundElement(KifCode* kifcode,x_node* xn,KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable=kifcode->KifInitVariable(xn ,base);
	//which we associate with our object
	Kifsound* local=new Kifsound(kifcode,variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn,local,variable,base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(KifCode* kifcode,string name,soundMethod func,int arity,string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname=KifNewId(name);
	KifCallMethod kcm(idname,arity);
	Kifsound::kifexportedmethods[idname]=kcm;
    kifGlobals->RecordExternalFunction(idname,arity);
	Kifsound::linkedmethods[idname]=func;
	Kifsound::infomethods[name]=infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exported bool Init_kifsound(KifCode* kifcode,string version) {
		if (version!=KIFVERSION) {
			cerr<<"Version incompatible de kiff"<<endl;
			return false;
		}
		
		kifcode->KifInitLib();

		if (kifGlobals->TestExternalType("son")==true)
			return false;
		
		///The new type will be: sound. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifsoundElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifsound)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifsoundElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res=KifAddNewType("son",CreatekifsoundElement);
		if (res==kifVOID)
			return false;
		sound_type=res;
		return true;
	}

	Exported bool kifsound_KifInitialisationModule(KifCode* kifcode,string version) {
		if (Init_kifsound(kifcode,version)==false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization(kifcode,"charge",&Kifsound::MethodLoad,P_ONE,"charge(chaine chemin): Charge un fichier son.");
		MethodInitialization(kifcode,"joue",&Kifsound::MethodPlay,P_NONE,"joue(): joue le son");
		MethodInitialization(kifcode,"arrête", &Kifsound::MethodStop, P_NONE, "arrête(): Arrête le son courant");

		return true;
	}

	Exported bool kifsound_Terminate(KifCode* kifcode) {
		Kifsound::kifexportedmethods.clear();
		Kifsound::linkedmethods.clear();
		Kifsound::infomethods.clear();

		mpg123_exit();
		ao_shutdown();

		return true;
	}
}
