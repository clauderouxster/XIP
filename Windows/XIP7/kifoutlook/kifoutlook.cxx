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

//Replace kifoutlook by your own definition
short KifNewId(string n);
string KifIdString(short n);

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType kifoutlook_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Kifkifoutlook;
//This typedef defines a type "kifoutlookMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifkifoutlook::*kifoutlookMethod)(KifElement*, KifDomain*, KifCallFunction*, int idthread);

#ifdef WIN32
#define Exporting __declspec(dllexport)
#import "C:\\Program Files (x86)\\Common Files\\microsoft shared\\OFFICE15\\mso.dll"
#import "C:\\Program Files (x86)\\Microsoft Office\\Office15\\MSOUTL.OLB"  rename_namespace("Outlook") rename("GetOrganizer", "GetOrganizerAddressEntry")  
using namespace Outlook;
#endif

//---------------------------------------------------------------------------------------------------------------------

void Time_tToSystemTime(time_t dosTime, DATE* d)
{
	tm *t = localtime(&dosTime);
	SYSTEMTIME s = { 1900 + t->tm_year, 1 + t->tm_mon, t->tm_wday, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, 0 };
	SystemTimeToVariantTime(&s, d);
}





class Kifkifoutlook : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, kifoutlookMethod> linkedmethods;
	static hmap<short, bool> methodassigned;	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	_ApplicationPtr spApp;
	bool tobereleased;

	//---------------------------------------------------------------------------------------------------------------------
	Kifkifoutlook(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, kifoutlook_type) {
		//Do not forget your variable initialisation
		CoInitialize(NULL);
		spApp = _ApplicationPtr(L"Outlook.Application");
		tobereleased = false;
	}

	~Kifkifoutlook() {
		if (tobereleased == true)
			CoUninitialize();
	}

	bool addEntrytoOutlook(string subject, string body, string location, time_t tstart, time_t tend, bool display) {


		wstring wsubject;
		wstring wbody;
		wstring wlocation;
		s_utf8_to_unicode(wsubject, STR(subject));
		s_utf8_to_unicode(wbody, STR(body));
		s_utf8_to_unicode(wlocation, STR(location));
		WCHAR* sb = (WCHAR*)wsubject.c_str();
		WCHAR* bd = (WCHAR*)wbody.c_str();
		WCHAR* lc = (WCHAR*)wlocation.c_str();

		DATE start;
		DATE end;

		Time_tToSystemTime(tstart, &start);
		Time_tToSystemTime(tend, &end);

		_AppointmentItemPtr rdv = spApp->CreateItem(olAppointmentItem);

		rdv->PutSubject((BSTR)sb);
		rdv->PutBody((BSTR)bd);
		rdv->PutLocation((BSTR)lc);
		rdv->PutStart(start);
		rdv->PutEnd(end);
		rdv->Save();
		if (display)
			rdv->Display(TRUE);

		tobereleased = true;
		return true;
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
		return kifoutlook_type;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		Kifkifoutlook* kperso = new Kifkifoutlook(KifCurrent(), NULL);
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
	KifElement* MethodRendezvous(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		KifElement* kstart = callfunc->Evaluate(3, domain);
		if (kstart->type != kifDate)
			return kifcode->Returnerror("OLK(001): Expecting date objects");

		KifElement* kend = callfunc->Evaluate(4, domain);
		if (kend->type != kifDate)
			return kifcode->Returnerror("OLK(001): Expecting date objects");

		string subject = callfunc->Evaluate(0, domain)->String();
		string body = callfunc->Evaluate(1, domain)->String();
		string location = callfunc->Evaluate(2, domain)->String();

		bool disp = false;
		if (callfunc->Size() == 6)
			disp = callfunc->Evaluate(5, domain)->Boolean();

		addEntrytoOutlook(subject, body, location, ((KifDate*)kstart)->value, ((KifDate*)kend)->value, disp);
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
map<short, KifCallMethod> Kifkifoutlook::kifexportedmethods;
map<short, kifoutlookMethod>  Kifkifoutlook::linkedmethods;
map<string, string> Kifkifoutlook::infomethods;
hmap<short, bool> Kifkifoutlook::methodassigned;


//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifkifoutlookElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifkifoutlook* local = new Kifkifoutlook(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name, kifoutlookMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifkifoutlook::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifkifoutlook::linkedmethods[idname] = func;
	Kifkifoutlook::methodassigned[idname] = true;
	Kifkifoutlook::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kifoutlook(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}
		kifcode->KifInitLib();

		///The new type will be: kifoutlook. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifkifoutlookElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifkifoutlook)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifkifoutlookElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("kifoutlook", CreatekifkifoutlookElement);
		if (res == kifVOID)
			return false;

		kifoutlook_type = res;
		return true;
	}

	Exporting bool kifoutlook_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifoutlook(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization("rendezvous", &Kifkifoutlook::MethodRendezvous, P_FIVE | P_SIX, "rendezvous(string subject,string body, string location, date start,date end,bool display): add an appointement in Outlook. display is optional.");

		kifGlobals->kifAllFunctions[kifoutlook_type] = Kifkifoutlook::methodassigned;
		kifGlobals->kifAllInformation[kifoutlook_type] = Kifkifoutlook::infomethods;
		//If you want to create specific variables in the main frame		
		KifVariableDeclaration* var = kifcode->Createvariable("outlook");
		//then we create its value
		Kifkifoutlook* local = new Kifkifoutlook(kifcode, var);
		return true;
	}
	Exporting bool kifoutlook_Terminate(KifCode* kifcode) {
		Kifkifoutlook::kifexportedmethods.clear();
		Kifkifoutlook::linkedmethods.clear();
		Kifkifoutlook::infomethods.clear();
		Kifkifoutlook::methodassigned.clear();
		return true;
	}
}




