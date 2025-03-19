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
#include <cstdlib>
#include <iostream>
#include "mongo/client/dbclient.h" // for the driver
#include "mongo/bson/bsonobj.h"

#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"

#ifdef WIN32
#define Exporting __declspec(dllexport)
#endif

using mongo::BSONElement;
using mongo::BSONObj;
using mongo::BSONObjBuilder;
using mongo::DBClientCursor;
using namespace std;

//Replace kifmongo by your own definition
short KifNewId(string n);
string KifIdString(short n);

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType kifmongo_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Kifkifmongo;
//This typedef defines a type "kifmongoMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifkifmongo::*kifmongoMethod)(KifElement*, KifDomain*, KifCallFunction*, int idthread);

class KifIteratorkifmongo : public KifIteration {
public:
	Kifkifmongo* value;

	KifIteratorkifmongo(KifCode* klc, KifElement* kp) : KifIteration(klc, kp) {
		value = NULL;
	}


	//The next methods are the ones that should be derived to add iterator capacities to Kifkifmongo
	KifElement* IteratorKey();
	KifElement* IteratorValue();
	void Next();
	KifElement* End();
	KifElement* Begin();
};


class Kifkifmongo : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, kifmongoMethod> linkedmethods;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	mongo::DBClientConnection clientmongo;
	string dbToUse;
	bool connected;
	//---------------------------------------------------------------------------------------------------------------------
	Kifkifmongo(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, kifmongo_type) {
		//Do not forget your variable initialisation
		connected = false;
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
		return kifmongo_type;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		Kifkifmongo* kperso = new Kifkifmongo(KifCurrent(), NULL);
		//Your initialisations here
		return kperso;
	}

	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//This description is facultative
	//iterator it=mydb;
	KifElement* Newiterator(bool rev) {
		KifIteratorkifmongo* iter = new KifIteratorkifmongo(kifcode, NULL);
		iter->reverse = rev;
		//Your initialisation here for your iterator object
		return iter;
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
	KifElement* MethodConnect(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (connected)
			return kifcode->Returnerror("MGB(001): Already connected.");

		string server = callfunc->Evaluatethread(0, domain, idthread)->String();
		string db;
		try {
			clientmongo.connect(server, db);
			connected = true;
		}
		catch (const mongo::DBException &e) {
			db = e.what();
		}

		return kifcode->Providestring(db);
	}

	KifElement* MethodLogout(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (connected == false)
			return kifFALSE;

		string server = callfunc->Evaluatethread(0, domain, idthread)->String();
		BSONObj info;
		string db;

		try {
			clientmongo.logout(server, info);
			connected = false;
			db = info.jsonString();
		}
		catch (const mongo::DBException &e) {
			db = e.what();
		}

		return kifcode->Providestring(db);
	}

	KifElement* MethodUse(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!connected)
			return kifcode->Returnerror("MGB(002): Connect to a server first");
		dbToUse = callfunc->Evaluatethread(0, domain, idthread)->String();
		return kifTRUE;
	}

	KifElement* MethodGeoidfile(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!connected)
			return kifcode->Returnerror("MGB(002): Connect to a server first");

		if (dbToUse == "")
			return kifcode->Returnerror("MGB(012): Select a database with 'use'");

		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		string prefix = callfunc->Evaluatethread(0, domain, idthread)->String();
		string objectid = callfunc->Evaluatethread(1, domain, idthread)->String();
		string query;

		try {
			mongo::GridFS fichier(clientmongo, dbToUse, prefix);
			mongo::GridFile kf = fichier.findFile(BSON("_id" << mongo::OID(objectid)));
			stringstream os;
			kf.write(os);
			query = os.str();
		}
		catch (const mongo::DBException &e) {
			query = "MGB(014):";
			query += e.what();
			return kifcode->Returnerror(query);

		}

		return kifcode->Providestring(query);
	}

	KifElement* MethodGetfile(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!connected)
			return kifcode->Returnerror("MGB(002): Connect to a server first");

		if (dbToUse == "")
			return kifcode->Returnerror("MGB(012): Select a database with 'use'");

		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		string prefix = callfunc->Evaluatethread(0, domain, idthread)->String();
		string filename = callfunc->Evaluatethread(1, domain, idthread)->String();
		string query;

		try {
			mongo::GridFS fichier(clientmongo, dbToUse, prefix);
			mongo::GridFile kf = fichier.findFile(filename);
			stringstream os;
			kf.write(os);
			query = os.str();
		}
		catch (const mongo::DBException &e) {
			query = "MGB(014):";
			query += e.what();
			return kifcode->Returnerror(query);

		}

		return kifcode->Providestring(query);
	}

	KifElement* MethodFind(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!connected)
			return kifcode->Returnerror("MGB(002): Connect to a server first");

		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		KifElement* kserver = callfunc->Evaluatethread(0, domain, idthread);
		KifElement* kjson = callfunc->Evaluatethread(1, domain, idthread);

		string nspace = kserver->String();
		if (nspace.find(".") == string::npos && dbToUse != "")
			nspace = dbToUse + "." + nspace;

		string query = kjson->String();
		string subquery;
		size_t fquery = query.find("},{");
		if (fquery != string::npos) {
			subquery = query.substr(fquery + 2, query.size() - fquery - 2);
			query = query.substr(0, fquery + 1);
		}
		try {
			auto_ptr<DBClientCursor> cursor;
			BSONObj bquery = mongo::fromjson(query);
			if (subquery != "") {
				BSONObj subbquery = mongo::fromjson(subquery);
				cursor = clientmongo.query(nspace, bquery, 0, 0, &subbquery);
			}
			else
				cursor = clientmongo.query(nspace, bquery);

			BSONObj p;

			if (contextualpattern->type == kifVectorString) {
				KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
				while (cursor->more()) {
					p = cursor->next();
					query = p.jsonString();
					((KifVectorString*)kvect)->values.push_back(query);
				}

				//you may return any value of course...
				return kvect;
			}

			if (contextualpattern->aVectorContainer()) {
				KifElement* kvect = Selectvector(callfunc, contextualpattern);
				while (cursor->more()) {
					p = cursor->next();
					query = p.jsonString();
					kjson = kifcode->EvaluateJSON(domain, query);
					kvect->Push(kjson);
				}

				//you may return any value of course...
				return kvect;
			}
			query = "[";
			bool first = true;
			string local;
			while (cursor->more()) {
				if (!first)
					query += ",";
				p = cursor->next();
				local = p.jsonString();
				if (local.size()) {
					query += local;
					first = false;
				}
			}
			query += "]";
			return kifcode->Providestring(query);
		}

		catch (const mongo::DBException &e) {
			query = "MGB(004):";
			query += e.what();
			return kifcode->Returnerror(query);

		}
		return kifNULL;
	}

	KifElement* MethodInsert(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!connected)
			return kifcode->Returnerror("MGB(002): Connect to a server first");

		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		KifElement* kserver = callfunc->Evaluatethread(0, domain, idthread);
		KifElement* kjson = callfunc->Evaluatethread(1, domain, idthread);

		string nspace = kserver->String();
		if (nspace.find(".") == string::npos && dbToUse != "")
			nspace = dbToUse + "." + nspace;

		string query = kjson->String();
		try {
			BSONObj bquery = mongo::fromjson(query);
			clientmongo.insert(nspace, bquery);
			nspace = clientmongo.getLastError();
			return kifcode->Providestring(nspace);
		}
		catch (const mongo::DBException &e) {
			query = "MGB(006):";
			query += e.what();
			return kifcode->Returnerror(query);

		}
		return kifFALSE;
	}

	KifElement* MethodUpdate(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!connected)
			return kifcode->Returnerror("MGB(002): Connect to a server first");

		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		KifElement* kserver = callfunc->Evaluatethread(0, domain, idthread);
		KifElement* kjson = callfunc->Evaluatethread(1, domain, idthread);

		bool upone = callfunc->Evaluatethread(2, domain, idthread)->Boolean();
		bool uptwo = callfunc->Evaluatethread(3, domain, idthread)->Boolean();

		string nspace = kserver->String();
		if (nspace.find(".") == string::npos && dbToUse != "")
			nspace = dbToUse + "." + nspace;

		string query = kjson->String();
		try {
			BSONObj bson;
			BSONObj bquery = mongo::fromjson(query);
			clientmongo.update(nspace, bquery, bson, upone, uptwo);
			nspace = clientmongo.getLastError();
			return kifcode->Providestring(nspace);
		}
		catch (const mongo::DBException &e) {
			query = "MGB(006):";
			query += e.what();
			return kifcode->Returnerror(query);

		}
		return kifFALSE;
	}

	KifElement* MethodRemove(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!connected)
			return kifcode->Returnerror("MGB(002): Connect to a server first");

		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		KifElement* kserver = callfunc->Evaluatethread(0, domain, idthread);
		KifElement* kjson = callfunc->Evaluatethread(1, domain, idthread);

		string nspace = kserver->String();
		if (nspace.find(".") == string::npos && dbToUse != "")
			nspace = dbToUse + "." + nspace;

		string query = kjson->String();
		try {
			BSONObj bquery = mongo::fromjson(query);
			clientmongo.remove(nspace, bquery);
			nspace = clientmongo.getLastError();
			return kifcode->Providestring(nspace);
		}
		catch (const mongo::DBException &e) {
			query = "MGB(009):";
			query += e.what();
			return kifcode->Returnerror(query);

		}
		return kifFALSE;
	}

	KifElement* MethodCount(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!connected)
			return kifcode->Returnerror("MGB(002): Connect to a server first");

		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		KifElement* kserver = callfunc->Evaluatethread(0, domain, idthread);

		string nspace = kserver->String();
		try {
			int v = clientmongo.count(nspace);
			//you may return any value of course...
			return kifcode->Provideinteger(v);
		}
		catch (const mongo::DBException &e) {
			string query = "MGB(005):";
			query += e.what();
			return kifcode->Returnerror(query);

		}
		return kifZERO;
	}

	KifElement* MethodRun(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!connected)
			return kifcode->Returnerror("MGB(002): Connect to a server first");

		string nspace;
		string query;
		if (callfunc->Size() == 1) {
			if (dbToUse == "")
				return kifcode->Returnerror("MGB(007): Select a database");
			nspace = dbToUse;
			query = callfunc->Evaluatethread(0, domain, idthread)->String();
		}
		else {
			nspace = callfunc->Evaluatethread(0, domain, idthread)->String();
			query = callfunc->Evaluatethread(1, domain, idthread)->String();
		}

		try {
			BSONObj cmdResult;
			BSONObj bquery = mongo::fromjson(query);
			bool ok = clientmongo.runCommand(nspace, bquery, cmdResult);
			query = cmdResult.jsonString();
			//you may return any value of course...
			return kifcode->Providestring(query);
		}
		catch (const mongo::DBException &e) {
			string query = "MGB(008):";
			query += e.what();
			return kifcode->Returnerror(query);

		}
		return kifNULL;
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
map<short, KifCallMethod> Kifkifmongo::kifexportedmethods;
map<short, kifmongoMethod>  Kifkifmongo::linkedmethods;
map<string, string> Kifkifmongo::infomethods;


//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific iterator implementation...
KifElement* KifIteratorkifmongo::IteratorKey() {
	return kifNULL;
}

KifElement* KifIteratorkifmongo::IteratorValue() {
	return kifNULL;
}
void KifIteratorkifmongo::Next() {
	//reverse denotes the direction of iteration (from beginning to end or reverse)

}
KifElement* KifIteratorkifmongo::End() {
	//if end of iteration return kifTRUE, kifFALSE otherwise
	//return kifTRUE;
	return kifFALSE;
}
KifElement* KifIteratorkifmongo::Begin() {
	//Your initialisation
	//reverse denotes the direction of iteration (from beginning to end or reverse)
	return IteratorValue();
}
//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifkifmongoElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifkifmongo* local = new Kifkifmongo(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name, kifmongoMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifkifmongo::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifkifmongo::linkedmethods[idname] = func;
	Kifkifmongo::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kifmongo(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}

		kifcode->KifInitLib();

		///The new type will be: kifmongo. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifkifmongoElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifkifmongo)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifkifmongoElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("mongodb", CreatekifkifmongoElement);
		if (res == kifVOID)
			return false;

		kifmongo_type = res;
		return true;
	}

	Exporting bool kifmongo_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifmongo(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization("connect", &Kifkifmongo::MethodConnect, P_ONE, "connect(string server): Connecting to a server");
		MethodInitialization("logout", &Kifkifmongo::MethodLogout, P_ONE, "logout(string server): Logging out from a server");
		MethodInitialization("find", &Kifkifmongo::MethodFind, P_TWO, "find(string ns,string query): Finding results");
		MethodInitialization("insert", &Kifkifmongo::MethodInsert, P_TWO, "insert(string ns,string query): Insert a new value");
		MethodInitialization("update", &Kifkifmongo::MethodUpdate, P_FOUR, "update(string ns,string query,bool,bool): Update");
		MethodInitialization("remove", &Kifkifmongo::MethodRemove, P_TWO, "remove(string ns,string query): Remove values");
		MethodInitialization("run", &Kifkifmongo::MethodRun, P_ONE, "run(string query): Execute a mongo command");
		MethodInitialization("count", &Kifkifmongo::MethodCount, P_ONE, "count(string ns): Count");
		MethodInitialization("usedb", &Kifkifmongo::MethodUse, P_ONE, "usedb(string db): Set the database space");
		MethodInitialization("getoidfile", &Kifkifmongo::MethodGeoidfile, P_TWO, "getoidfile(string prefix,string oid): Get a file throught its Object ID");
		MethodInitialization("getfile", &Kifkifmongo::MethodGetfile, P_TWO, "getofile(string prefix,string name): Get a file throught its name");

		return true;
	}
	Exporting bool kifmongo_Terminate(KifCode* kifcode) {
		return true;
	}
}
