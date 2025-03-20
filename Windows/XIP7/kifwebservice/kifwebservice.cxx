/* --- CONTENTS ---
Project    : KIFF
Version    : 1.93
filename   : kifwebservice.cxx
Date       : 2015/07/23
Purpose    : Automatically Generated
Programmer : Le GRAC
*/

#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"
#include "kiftreemaps.h"
#include "webservice.h"

//Replace kifwebservice by your own definition
short KifNewId(string n);
string KifIdString(short n);

#ifdef WIN32
#define Exporting __declspec(dllexport)
#endif

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType kifwebservice_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Kifkifwebservice;
//This typedef defines a type "kifwebserviceMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifkifwebservice::*kifwebserviceMethod)(KifElement*,KifDomain*,KifCallFunction*,int idthread);


class Kifkifwebservice : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short,KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short,kifwebserviceMethod> linkedmethods;
	static map<string,string> infomethods;

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
	//Your personal variables here...
	Callwebservice* myweb;
	hmap<string, KifFunction*> charts;
	map<string, string> descriptors;

//---------------------------------------------------------------------------------------------------------------------
	Kifkifwebservice(KifCode* kifcode,KifElement* base) : KifBasic(kifcode,base,kifwebservice_type) {
		//Do not forget your variable initialisation
		myweb = NULL;
	}

	~Kifkifwebservice() {
		delete myweb;
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
		return kifwebservice_type;
	}

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		Kifkifwebservice* kperso=new Kifkifwebservice(kifcode,NULL);
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
	KifElement* constructor(KifDomain* dom,KifElement* params,int idthread,KifCallFunction* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize()==0)
			return this;

		//Your first parameter is at position 0 etc...
		KifElement* kfirst=params->Instruction(0)->Exec(kifNULL,dom,kifNULL,idthread,callfunc); //would match i as in the example above: mynewtype toto(i,j);
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
			fcts->Push(kifcode->Providestringraw(KifIdString(it->first)));
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

	KifElement* MethodRegistered(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (myweb == NULL)
			myweb = new Callwebservice(this, false);

		string loc = callfunc->Evaluatethread(0, domain, idthread)->String();
		string res;
		int e = myweb->getmethods(loc,res);
		return kifcode->Providestring(res);
	}

	KifElement* MethodCall(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (myweb == NULL)
			myweb = new Callwebservice(this, false);

		string loc = callfunc->Evaluatethread(0, domain, idthread)->String();
		string name = callfunc->Evaluatethread(1, domain, idthread)->String();

		vector<string> params;
		for (int i = 2; i < callfunc->Size(); i++) {
			string p = callfunc->Evaluatethread(i, domain, idthread)->String();
			params.push_back(p);
		}

		string res;
		int e = myweb->calling(loc, name, params, res);
		return kifcode->Providestring(res);
	}

	KifElement* MethodServe(KifElement* contextualpattern,KifDomain* domain,KifCallFunction* callfunc,int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (myweb == NULL)
			myweb = new Callwebservice(this, true);
		string thehost = callfunc->Evaluatethread(0, domain, idthread)->String();
		int  theport = callfunc->Evaluatethread(1, domain, idthread)->Integer();
		int i=myweb->run(thehost,theport);
		//you may return any value of course...
		if (!i)
			return kifTRUE;
		return kifcode->Returnerror(myweb->error());
	}

	KifElement* MethodWSDL(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		string loc = callfunc->Evaluatethread(0, domain, idthread)->String();
		int prt = callfunc->Evaluatethread(1, domain, idthread)->Integer();
		loc = myweb->Generatewsdl(loc,prt);
		return kifcode->Providestring(loc);
	}

	KifElement* MethodRegister(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		string name = callfunc->Evaluatethread(0, domain, idthread)->String();
		string description = callfunc->Evaluatethread(1, domain, idthread)->String();
		if (kifGlobals->kifStringId.find(name) == kifGlobals->kifStringId.end())
			return kifcode->Returnerror("WSD(001): Unknown function");
		int idname = kifGlobals->kifStringId[name];
		if (kifcode->kstart->declarations.find(idname) == kifcode->kstart->declarations.end())
			return kifcode->Returnerror("WSD(001): Unknown function");
		KifElement* kfunc = kifcode->kstart->declarations[idname];
		if (kfunc->isFunction()==false)
			return kifcode->Returnerror("WSD(001): Unknown function");
		KifFunction* kfunction = (KifFunction*)kfunc;

		if (kfunction->arguments.size()>5)
			return kifcode->Returnerror("WSD(002): Wrong number of arguments in function: it should be between 0 and 5");

		charts[name] = kfunction;
		descriptors[name] = description;
		return kifTRUE;
	}
//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name,KifElement* contextualpattern,KifDomain* domain,int idthread,KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        KiFArityError(name)

		return (this->*linkedmethods[name])(contextualpattern,domain,callfunc,idthread);
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
	long Size() {return 0;}

	//How to interprete your class according to the basic following types:
	string String() {return myweb->hostport();}
	long Integer() { return myweb->port; }
	double Float() { return myweb->port; }
	bool Boolean() { if (myweb->port == -1) return false; else return true; }
};

//We need to declare once again our local definitions.
map<short,KifCallMethod> Kifkifwebservice::kifexportedmethods;
map<short,kifwebserviceMethod>  Kifkifwebservice::linkedmethods;
map<string,string> Kifkifwebservice::infomethods;

//-----------------------------------------------------------------------------------------------------------------------
int Callwebservice::execute0(string name,string& ret) {
	Kifkifwebservice* ksrv = (Kifkifwebservice*)kifservice;
	if (ksrv->charts.find(name) == ksrv->charts.end()) {
		ret = "WSD(001): Unknown function";
		return 1;
	}

	threadblocinit;
	int idname = kifGlobals->kifStringId[name];

	KifCallFunctionGeneral kfunc(NULL, NULL, idname, ksrv->charts[name]);
	kfunc.kifcode = ksrv->kifcode;
	KifElement* res = kfunc.Exec(kifNULL, ksrv->kifcode->kstart, kifNULL, idthread, callfunc);
	
	ret = res->String();

	res->Release();
	return 1;
}
//-----------------------------------------------------------------------------------------------------------------------
int Callwebservice::execute1(string name, string p1, string& ret) {
	Kifkifwebservice* ksrv = (Kifkifwebservice*)kifservice;
	if (ksrv->charts.find(name) == ksrv->charts.end()) {
		ret = "WSD(001): Unknown function";
		return 1;
	}

	threadblocinit;
	int idname = kifGlobals->kifStringId[name];
	KifString* kp1 = ksrv->kifcode->Providestring(p1);
	kp1->Setreference();

	KifCallFunctionGeneral kfunc(NULL, NULL, idname, ksrv->charts[name]);
	kfunc.kifcode = ksrv->kifcode;
	kfunc.parameters.push_back(kp1);
	KifElement* res = kfunc.Exec(kifNULL, ksrv->kifcode->kstart, kifNULL, idthread, callfunc);

	ret = res->String();

	kp1->Resetreference();
	res->Release();
	return 1;
}
//-----------------------------------------------------------------------------------------------------------------------
int Callwebservice::execute2(string name, string p1, string p2, string& ret) {
	Kifkifwebservice* ksrv = (Kifkifwebservice*)kifservice;
	if (ksrv->charts.find(name) == ksrv->charts.end()) {
		ret = "WSD(001): Unknown function";
		return 1;
	}

	threadblocinit;
	int idname = kifGlobals->kifStringId[name];
	KifString* kp1 = ksrv->kifcode->Providestring(p1);
	KifString* kp2 = ksrv->kifcode->Providestring(p2);
	kp1->Setreference();
	kp2->Setreference();

	KifCallFunctionGeneral kfunc(NULL, NULL, idname, ksrv->charts[name]);
	kfunc.kifcode = ksrv->kifcode; 
	kfunc.parameters.push_back(kp1);
	kfunc.parameters.push_back(kp2);
	KifElement* res = kfunc.Exec(kifNULL, ksrv->kifcode->kstart, kifNULL, idthread, callfunc);

	ret = res->String();

	kp1->Resetreference();
	kp2->Resetreference();
	res->Release();
	return 1;
}
//-----------------------------------------------------------------------------------------------------------------------
int Callwebservice::execute3(string name, string p1, string p2, string p3, string& ret) {
	Kifkifwebservice* ksrv = (Kifkifwebservice*)kifservice;
	if (ksrv->charts.find(name) == ksrv->charts.end()) {
		ret = "WSD(001): Unknown function";
		return 1;
	}

	threadblocinit;
	int idname = kifGlobals->kifStringId[name];
	KifString* kp1 = ksrv->kifcode->Providestring(p1);
	KifString* kp2 = ksrv->kifcode->Providestring(p2);
	KifString* kp3 = ksrv->kifcode->Providestring(p3);
	kp1->Setreference();
	kp2->Setreference();
	kp3->Setreference();

	KifCallFunctionGeneral kfunc(NULL, NULL, idname, ksrv->charts[name]);
	kfunc.kifcode = ksrv->kifcode;
	kfunc.parameters.push_back(kp1);
	kfunc.parameters.push_back(kp2);
	kfunc.parameters.push_back(kp3);
	KifElement* res = kfunc.Exec(kifNULL, ksrv->kifcode->kstart, kifNULL, idthread, callfunc);

	ret = res->String();

	kp1->Resetreference();
	kp2->Resetreference();
	kp3->Resetreference();
	res->Release();
	return 1;
}
//-----------------------------------------------------------------------------------------------------------------------
int Callwebservice::execute4(string name, string p1, string p2, string p3, string p4, string& ret) {
	Kifkifwebservice* ksrv = (Kifkifwebservice*)kifservice;
	if (ksrv->charts.find(name) == ksrv->charts.end()) {
		ret = "WSD(001): Unknown function";
		return 1;
	}

	threadblocinit;
	int idname = kifGlobals->kifStringId[name];
	KifString* kp1 = ksrv->kifcode->Providestring(p1);
	KifString* kp2 = ksrv->kifcode->Providestring(p2);
	KifString* kp3 = ksrv->kifcode->Providestring(p3);
	KifString* kp4 = ksrv->kifcode->Providestring(p4);
	kp1->Setreference();
	kp2->Setreference();
	kp3->Setreference();
	kp4->Setreference();

	KifCallFunctionGeneral kfunc(NULL, NULL, idname, ksrv->charts[name]);
	kfunc.kifcode = ksrv->kifcode;
	kfunc.parameters.push_back(kp1);
	kfunc.parameters.push_back(kp2);
	kfunc.parameters.push_back(kp3);
	kfunc.parameters.push_back(kp4);
	KifElement* res = kfunc.Exec(kifNULL, ksrv->kifcode->kstart, kifNULL, idthread, callfunc);

	ret = res->String();

	kp1->Resetreference();
	kp2->Resetreference();
	kp3->Resetreference();
	kp4->Resetreference();
	res->Release();
	return 1;
}
//-----------------------------------------------------------------------------------------------------------------------
int Callwebservice::execute5(string name, string p1, string p2, string p3, string p4, string p5, string& ret) {
	Kifkifwebservice* ksrv = (Kifkifwebservice*)kifservice;
	if (ksrv->charts.find(name) == ksrv->charts.end()) {
		ret = "WSD(001): Unknown function";
		return 1;
	}

	threadblocinit;
	int idname = kifGlobals->kifStringId[name];
	KifString* kp1 = ksrv->kifcode->Providestring(p1);
	KifString* kp2 = ksrv->kifcode->Providestring(p2);
	KifString* kp3 = ksrv->kifcode->Providestring(p3);
	KifString* kp4 = ksrv->kifcode->Providestring(p4);
	KifString* kp5 = ksrv->kifcode->Providestring(p5);
	kp1->Setreference();
	kp2->Setreference();
	kp3->Setreference();
	kp4->Setreference();
	kp5->Setreference();

	KifCallFunctionGeneral kfunc(NULL, NULL, idname, ksrv->charts[name]);
	kfunc.kifcode = ksrv->kifcode;
	kfunc.parameters.push_back(kp1);
	kfunc.parameters.push_back(kp2);
	kfunc.parameters.push_back(kp3);
	kfunc.parameters.push_back(kp4);
	kfunc.parameters.push_back(kp5);
	KifElement* res = kfunc.Exec(kifNULL, ksrv->kifcode->kstart, kifNULL, idthread, callfunc);

	ret = res->String();

	kp1->Resetreference();
	kp2->Resetreference();
	kp3->Resetreference();
	kp4->Resetreference();
	kp5->Resetreference();

	res->Release();
	return 1;
}
//-----------------------------------------------------------------------------------------------------------------------

int Callwebservice::methods(string& ret) {
	KifTreeMapStringString* kt = new KifTreeMapStringString(NULL, NULL);
	Kifkifwebservice* ksrv = (Kifkifwebservice*)kifservice;
	kt->values = ksrv->descriptors;
	ret = kt->JSonString();
	return 1;
}
//-----------------------------------------------------------------------------------------------------------------------
//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifkifwebserviceElement(KifCode* kifcode,x_node* xn,KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable=kifcode->KifInitVariable(xn ,base);
	//which we associate with our object
	Kifkifwebservice* local=new Kifkifwebservice(kifcode,variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn,local,variable,base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name,kifwebserviceMethod func,int arity,string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname=KifNewId(name);
	KifCallMethod kcm(idname,arity);
	Kifkifwebservice::kifexportedmethods[idname]=kcm;
    kifGlobals->RecordExternalFunction(idname,arity);
	Kifkifwebservice::linkedmethods[idname]=func;
	Kifkifwebservice::infomethods[name]=infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kifwebservice(KifCode* kifcode,string version) {
		if (version!=KIFVERSION) {
			cerr<<"Incompatible version of KiF"<<endl;
			return false;
		}
		kifcode->KifInitLib();
		
		///The new type will be: kifwebservice. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifkifwebserviceElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifkifwebservice)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifkifwebserviceElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res=KifAddNewType("webservice",CreatekifkifwebserviceElement);
		if (res==kifVOID)
			return false;

		kifwebservice_type=res;
		return true;
	}

	Exporting bool kifwebservice_KifInitialisationModule(KifCode* kifcode,string version) {
		if (Init_kifwebservice(kifcode,version)==false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization("registered", &Kifkifwebservice::MethodRegistered, P_ONE, "registered(string endpoint): Return the list of registered methods.");
		MethodInitialization("calling", &Kifkifwebservice::MethodCall, P_TWO | P_THREE | P_FOUR | P_FIVE | P_SIX | P_SEVEN, "calling(string endpoint,string name, ...): Call the Web Service with 0 to five more parameters.");
		MethodInitialization("launch", &Kifkifwebservice::MethodServe, P_TWO, "launch(string host,int port): Launch the Web Service.");
		MethodInitialization("wsdl", &Kifkifwebservice::MethodWSDL, P_TWO, "wsdl(string  location,int port): return the wsdl description.");
		MethodInitialization("register", &Kifkifwebservice::MethodRegister, P_TWO, "register(string  name,string description): register functions with their description.");
		return true;
	}

	Exporting bool kifwebservice_Terminate(KifCode* kifcode) {
		return true;
	}
}
