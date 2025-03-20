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

//Replace kifmytest by your own definition
short KifNewId(string n);
string KifIdString(short n);

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType kifmytest_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Kifkifmytest;
//This typedef defines a type "kifmytestMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifkifmytest::*kifmytestMethod)(KifElement*,KifDomain*,KifCallFunction*);


class KifIteratorkifmytest: public KifIteration {
public:
	Kifkifmytest* value;

	KifIteratorkifmytest(KifCode* klc,KifElement* kp) : KifIteration(klc,kp) {
		value=NULL;
	}


	//The next methods are the ones that should be derived to add iterator capacities to Kifkifmytest
	KifElement* IteratorKey();
	KifElement* IteratorValue();
	KifElement* Next();
	KifElement* End();
	KifElement* Begin();
};


class Kifkifmytest : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short,KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short,kifmytestMethod> linkedmethods;
	static map<string,string> infomethods;

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
	//Your personal variables here...
//---------------------------------------------------------------------------------------------------------------------
	Kifkifmytest(KifCode* kifcode,KifElement* base) : KifBasic(kifcode,base,kifmytest_type) {
		//Do not forget your variable initialisation
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
		return kifmytest_type;
	}

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		Kifkifmytest* kperso=new Kifkifmytest(KifCurrent(),NULL);
//Your initialisations here
		return kperso;
	}

	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//This description is facultative
	//iterator it=mydb;
	KifElement* Newiterator(bool rev) {
		KifIteratorkifmytest* iter=new KifIteratorkifmytest(kifcode,NULL);
		iter->reverse=rev;
		//Your initialisation here for your iterator object
		return iter;
	}
//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	Exported bool initialconstructor() {
		return false;
	}
	
	//If initialconstructor returns true, then you might want to benefit from these parameters
	Exported KifElement* constructor(KifDomain* dom,KifElement* params) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize()==0)
			return this;

		//Your first parameter is at position 0 etc...
		KifElement* kfirst=params->Instruction(0)->Execute(kifNULL,dom); //would match i as in the example above: mynewtype toto(i,j);
		KifElement* ksecond=params->Instruction(1)->Execute(kifNULL,dom); //would match j as in the example above: mynewtype toto(i,j);
		//etc...
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
	void Methods(KifVector* fcts) {
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
	KifElement* MethodSample(KifElement* contextualpattern,KifDomain* domain,KifCallFunction* callfunc) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		KifElement* first=callfunc->Evaluate(0,domain);
		KifElement* second=callfunc->Evaluate(1,domain);
		string s=first->String();
		long l=second->Integer();
		double d=second->Float();
		
		//you may return any value of course...
		return first;
	}
//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecuteMethod(short name,KifElement* contextualpattern,KifDomain* domain,KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
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

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
	long Size() {return 0;}

	//How to interprete your class according to the basic following types:
	string String() {return "";}
	long Integer() {return 0;}
	double Float() {return 0;}
	bool Boolean() {return true;}

	//You can overcome the following function behaviour
	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* contextualpattern,bool idx) {
		return kifNULL;
	}
	 //union of two sets
	 KifElement* orset(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		return kifNULL;
	}

	 KifElement* xorset(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		return kifNULL;
	}

	 //intersection of two sets
	 KifElement* andset(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		return kifNULL;
	}

	 //Beware that a might be the current element
	 //plus: a+b
	 KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		 return kifNULL;
	}

	 //minus: a-b
	 KifElement* minus(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		return kifNULL;
	}
	 
	 //Multiplication: a*b
	 KifElement* multiply(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		return kifNULL;
	}

	//Division: a/b
	 KifElement* divide(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		return kifNULL;
	}
	 //This is the division integer rest: a%b
	 KifElement* mod(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		return kifNULL;
	}
	 //This is a binary right shift: a>>b
	 KifElement* shiftright(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		return kifNULL;
	}
	 //This is a binary left shift: a<<b
	 KifElement* shiftleft(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		return kifNULL;
	}
	 //a to the power of b: a^b
	 KifElement* power(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
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
		if (a==this)
			return kifTRUE;
		return kifFALSE;
	}

	 //different: this!=a
	 KifElement* different(KifElement* a) {
		if (a!=this)
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
map<short,KifCallMethod> Kifkifmytest::kifexportedmethods;
map<short,kifmytestMethod>  Kifkifmytest::linkedmethods;
map<string,string> Kifkifmytest::infomethods;


//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific iterator implementation...
KifElement* KifIteratorkifmytest::IteratorKey() {
	return kifNULL;
}

KifElement* KifIteratorkifmytest::IteratorValue() {
	return kifNULL;
}
KifElement* KifIteratorkifmytest::Next() {
	//reverse denotes the direction of iteration (from beginning to end or reverse)
	return IteratorValue();
}
KifElement* KifIteratorkifmytest::End() {
//if end of iteration return kifTRUE, kifFALSE otherwise
	//return kifTRUE;
	return kifFALSE;
}
KifElement* KifIteratorkifmytest::Begin() {
	//Your initialisation
	//reverse denotes the direction of iteration (from beginning to end or reverse)
	return IteratorValue();
}
//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifkifmytestElement(KifCode* kifcode,x_node* xn,KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable=kifcode->KifInitVariable(xn ,base);
	//which we associate with our object
	Kifkifmytest* local=new Kifkifmytest(kifcode,variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn,local,variable,base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name,kifmytestMethod func,int arity,string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname=KifNewId(name);
	KifCallMethod kcm(idname,arity);
	Kifkifmytest::kifexportedmethods[idname]=kcm;
    kifGlobals->RecordExternalFunction(idname,arity);
	Kifkifmytest::linkedmethods[idname]=func;
	Kifkifmytest::infomethods[name]=infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exported bool Init_kifmytest(KifCode* kifcode,string version) {
		static bool loaded=false;
		if (version!=KIFVERSION) {
			cerr<<"Incompatible version of KiF"<<endl;
			return false;
		}
		kifcode->KifInitLib();

		if (loaded==false) 
			loaded=true;
		else {
			//Put anything that need to re-install in a kif session
			return false;
		}
		
		///The new type will be: kifmytest. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifkifmytestElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifkifmytest)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifkifmytestElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		kifmytest_type=KifAddNewType("kifmytest",CreatekifkifmytestElement);
		if (kifmytest_type==kifVOID)
			return false;
		return true;
	}

	Exported bool kifmytest_KifInitialisationModule(KifCode* kifcode,string version) {
		if (Init_kifmytest(kifcode,version)==false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization("sample",&Kifkifmytest::MethodSample,P_TWO,"sample(x,y): example of a method implementation");

		//If you want to create specific variables in the main frame		
		KifVariableDeclaration* var=kifcode->Createvariable("myvariable");
		//then we create its value
		Kifkifmytest* local=new Kifkifmytest(kifcode,var);
		return true;
	}
	Exported bool kifmytest_Terminate(KifCode* kifcode) {
		return true;
	}
}
