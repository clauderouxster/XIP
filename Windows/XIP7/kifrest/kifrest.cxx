/* --- CONTENTS ---
Project    : KIFF
Version    : 1.93
filename   : kifrest.cxx
Date       : 2015/07/23
Purpose    : Automatically Generated
Programmer : Le GRAC
*/

#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"
#include "kiftreemaps.h"
#include "restservice.h"

//Replace kifrest by your own definition
short KifNewId(string n);
string KifIdString(short n);

#ifdef WIN32
#define Exporting __declspec(dllexport)
#endif

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType kifrest_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Kifkifrest;
//This typedef defines a type "kifrestMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifkifrest::*kifrestMethod)(KifElement*,KifDomain*,KifCallFunction*,int idthread);

static map<string, bool> actions;

static void initialize() {
	static bool init = false;
	if (!init) {
		init = true;
		actions["jpg"] = true;
		actions["image"] = true;
		actions["text"] = true;
		actions["PUT"] = true;
		actions["DEL"] = true;
	}
}

#ifdef C11
class KifRestLock {
public:
	std::lock_guard<std::recursive_mutex>* g;

	KifRestLock(KifThreadLock* d) {
		g = NULL;
		if (kifGlobals->Test() == true)
			g = new std::lock_guard<std::recursive_mutex>(*d->lock);

	}

	~KifRestLock() {
		if (g != NULL)
			delete g;
	}
};
#else
class KifRestLock {
public:
	bool unlock;
	KifThreadLock* _locker;

	KifRestLock(KifThreadLock* l) : _locker(l) {
		unlock = false;
		if (kifGlobals->Test() == true) {
			unlock = true;
			_locker->Locking();
		}
	}

	~KifRestLock() {
		if (unlock)
			_locker->Unlocking();
	}
};

#endif

class Kifkifrest : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short,KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short,kifrestMethod> linkedmethods;
	static map<string,string> infomethods;

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
	//Your personal variables here...
	Callrest* myweb;
	bool owned;
	hmap<string, KifFunction*> post_handlers;
	KifElement* function;
	KifDomain* dom;
	KifThreadLock _locker;

//---------------------------------------------------------------------------------------------------------------------
	Kifkifrest(KifCode* kifcode,KifElement* base) : KifBasic(kifcode,base,kifrest_type), _locker(NULL,false) {
		//Do not forget your variable initialisation
		function = NULL;
		if (base!=NULL)
			function = base->Robfunction(this);
		myweb = NULL;
	}

	~Kifkifrest() {
		if (myweb!=NULL)
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
		return kifrest_type;
	}

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		Kifkifrest* kperso=new Kifkifrest(kifcode,NULL);
		kperso->function = function;
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

	KifElement* MethodLaunch(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (myweb==NULL)
			myweb = new Callrest(this);
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		string thehost = callfunc->Evaluatethread(0, domain, idthread)->String();
		int  theport = callfunc->Evaluatethread(1, domain, idthread)->Integer();
		if (!myweb->launch(thehost, theport))
			return kifcode->Returnerror(myweb->buff);
		//you may return any value of course...
		return kifTRUE;
	}

	KifElement* MethodConnect(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (myweb == NULL)
			myweb = new Callrest(this);
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		string thehost = callfunc->Evaluatethread(0, domain, idthread)->String();
		int  theport = callfunc->Evaluatethread(1, domain, idthread)->Integer();
		if (!myweb->connect(thehost, theport))
			return kifcode->Returnerror(myweb->buff);
		//you may return any value of course...
		return kifTRUE;
	}

	KifElement* MethodWait(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (myweb == NULL)
			return kifcode->Returnerror("RST(001): Please launch a server first...");
		int client = myweb->wait();
		if (!client)
			return kifcode->Returnerror(myweb->buff);
		//you may return any value of course...
		return kifcode->Provideinteger(client);
	}

	KifElement* MethodClose(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (myweb == NULL)
			return kifcode->Returnerror("RST(001): Please launch a server first...");

		if (!myweb->close())
			return kifcode->Returnerror(myweb->buff);
		//you may return any value of course...
		return kifTRUE;
	}

	KifElement* MethodReceive(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (myweb == NULL)
			return kifcode->Returnerror("RST(002): Please launch or connect to a server first...");

		int client = -1;
		KifRestLock _l(&_locker);
		if (myweb->iserver()) {
			if (callfunc->Size() == 1)
				client = callfunc->Evaluatethread(0, domain, idthread)->Integer();
			else
				return kifcode->Returnerror("RST(202): Please supply the client socket...");
		}
		dom = domain;		
		map<string,string> tokens;
		if (!myweb->get(tokens,client))
			return kifcode->Returnerror(myweb->buff);
		//you may return any value of course...
		KifTreeMapStringString* treemap;
		if (contextualpattern->type == kifTreeMapStringString)
			treemap = (KifTreeMapStringString*)contextualpattern;
		else
			treemap = new KifTreeMapStringString(kifcode, NULL);
		treemap->values = tokens;
		return treemap;
	}

	KifElement* MethodSend(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (myweb == NULL)
			return kifcode->Returnerror("RST(002): Please launch or connect to a server first...");

		string data;
		KifRestLock _l(&_locker);

		if (callfunc->Size() == 0) {
			if (myweb->iserver())
				return kifcode->Returnerror("RST(202): Please supply the client socket...");
			if (!myweb->put(data, "HTML", true, -1))
				return kifcode->Returnerror(myweb->buff);
			return kifTRUE;
		}

		data = callfunc->Evaluatethread(0, domain, idthread)->String();
		int client = -1;
		if (myweb->iserver()) {
			if (callfunc->Size() == 2)
				client = callfunc->Evaluatethread(1, domain, idthread)->Integer();
			else
				return kifcode->Returnerror("RST(202): Please supply the client socket...");
		}

		if (!myweb->put(data, "HTML", false, client))
			return kifcode->Returnerror(myweb->buff);
		return kifTRUE;
	}

	KifElement* MethodRegister(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		initialize();
		string key = callfunc->Evaluatethread(0, domain, idthread)->String();
		if (actions.find(key) == actions.end())
			return kifcode->Returnerror("RST(080): Unknown key for handlers");

		string name = callfunc->Evaluatethread(1, domain, idthread)->String();
		if (kifGlobals->kifStringId.find(name) == kifGlobals->kifStringId.end())
			return kifcode->Returnerror("RST(091): Unknown function");
		int idname = kifGlobals->kifStringId[name];
		if (kifcode->kstart->declarations.find(idname) == kifcode->kstart->declarations.end())
			return kifcode->Returnerror("RST(091): Unknown function");
		KifElement* kfunc = kifcode->kstart->declarations[idname];
		if (kfunc->isFunction() == false)
			return kifcode->Returnerror("RST(091): Unknown function");
		KifFunction* kfunction = (KifFunction*)kfunc;
		if (kfunction->arguments.size()!=3)
			return kifcode->Returnerror("RST(092): Wrong number of arguments in function: it should be 3");

		post_handlers[key] = kfunction;

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
	string String() { return myweb->hostport(); }
	long Integer() { return myweb->port; }
	double Float() { return myweb->port; }
	bool Boolean() { if (myweb->port == -1) return false; else return true; }
};

//We need to declare once again our local definitions.
map<short,KifCallMethod> Kifkifrest::kifexportedmethods;
map<short,kifrestMethod>  Kifkifrest::linkedmethods;
map<string,string> Kifkifrest::infomethods;

//-----------------------------------------------------------------------------------------------------------------------
int Callrest::callget(map<string, string>& dico) {
	Kifkifrest* krest = (Kifkifrest*)kifrest;
	if (krest->function == NULL)
		return 0;

	KifTreeMapStringString* ktree = new KifTreeMapStringString(krest->kifcode, NULL);
	ktree->values = dico;
	KifFunction* func = (KifFunction*)krest->function;
	KifCallFunctionGeneral kfunc(NULL, NULL, func->Name(), func);
	kfunc.kifcode = func->kifcode;
	ktree->Setreference();
	kfunc.parameters.push_back(ktree);
	threadblocinit;
	KifElement* ret = kfunc.Exec(kifNULL, krest->dom, kifNULL, idthread, callfunc);
	ktree->Resetreference();
	int res = ret->Integer();
	ret->Release();
	return res;
}

int Callrest::callpost(string key,char* buff,map<string,string>& dico) {
	Kifkifrest* krest = (Kifkifrest*)kifrest;
	if (krest->post_handlers.find(key) == krest->post_handlers.end())
		return 0;

	string text;
	if (buff != NULL)
		text = buff;
	KifTreeMapStringString* ktree = new KifTreeMapStringString(krest->kifcode, NULL);
	ktree->values = dico;
	ktree->Setreference();
	KifString* val = krest->kifcode->Providestring(text);
	val->Setreference();
	KifString* kkey= krest->kifcode->Providestring(key);
	kkey->Setreference();
	KifFunction* func = krest->post_handlers[key];
	KifCallFunctionGeneral kfunc(NULL, NULL, func->Name(), func);
	kfunc.kifcode = func->kifcode;
	kfunc.parameters.push_back(kkey);
	kfunc.parameters.push_back(val);
	kfunc.parameters.push_back(ktree);
	threadblocinit;
	KifElement* ret = kfunc.Exec(kifNULL, krest->dom, kifNULL,idthread,callfunc);
	kkey->Resetreference(); 
	val->Resetreference();
	ktree->Resetreference();
	int res = ret->Integer();
	ret->Release();
	return res;
}

//-----------------------------------------------------------------------------------------------------------------------
//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifkifrestElement(KifCode* kifcode,x_node* xn,KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable=kifcode->KifInitVariable(xn ,base);
	//which we associate with our object
	Kifkifrest* local=new Kifkifrest(kifcode,variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn,local,variable,base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name,kifrestMethod func,int arity,string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname=KifNewId(name);
	KifCallMethod kcm(idname,arity);
	Kifkifrest::kifexportedmethods[idname]=kcm;
    kifGlobals->RecordExternalFunction(idname,arity);
	Kifkifrest::linkedmethods[idname]=func;
	Kifkifrest::infomethods[name]=infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kifrest(KifCode* kifcode,string version) {
		if (version!=KIFVERSION) {
			cerr<<"Incompatible version of KiF"<<endl;
			return false;
		}
		kifcode->KifInitLib();
		
		///The new type will be: kifrest. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifkifrestElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifkifrest)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifkifrestElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res=KifAddNewType("REST",CreatekifkifrestElement);
		if (res==kifVOID)
			return false;

		kifrest_type=res;
		return true;
	}

	Exporting bool kifrest_KifInitialisationModule(KifCode* kifcode,string version) {
		if (Init_kifrest(kifcode,version)==false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization("launch", &Kifkifrest::MethodLaunch, P_TWO, "launch(string host,int port): Launch the Web Service.");
		MethodInitialization("connect", &Kifkifrest::MethodConnect, P_TWO, "connect(string host,int port): Connect to a Web Service.");
		MethodInitialization("wait", &Kifkifrest::MethodWait, P_NONE, "wait(): Wait for connection.");
		MethodInitialization("receive", &Kifkifrest::MethodReceive, P_NONE | P_ONE, "receive(int client): Pick a string from data flow.");
		MethodInitialization("send", &Kifkifrest::MethodSend, P_NONE|P_ONE|P_TWO, "send(string data,int client): Put string into data flow.");
		MethodInitialization("close", &Kifkifrest::MethodClose, P_NONE, "close(): close REST socket.");
		//MethodInitialization("register", &Kifkifrest::MethodRegister, P_TWO, "register(string  action,string name): register functions with an action.");
		return true;
	}

	Exporting bool kifrest_Terminate(KifCode* kifcode) {
		return true;
	}
}
