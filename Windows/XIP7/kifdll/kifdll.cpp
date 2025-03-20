/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 10.00
   filename   : kifdll.cpp
   Date       : 09/09/2010
   Purpose    : An example of a KiF custom library
   Programmer : Claude ROUX
   Reviewer   : 
*/

// kifdll.cpp : Defines the exported functions for the DLL application.
//
#define KIF_DLL
#include "kif.h"

//Declarations
static KifType perso_type;
static map<string,KifCallMethod*> kifpersomethods;

//We create a map between our methods, which have been declared in our class below (see MethodSet and MethodSize)
//with their names.
class KifPerso;
//This typedef defines a type "MethodPerso", which defines the parameters that we expect for our new KiF methods
typedef KifElement* (KifPerso::*MethodPerso)(KifElement*,KifDomain*,KifCallFunction*);
//We store the information in our persomethods map
static map<string,MethodPerso> persomethods;


class KifPerso : public KifBasic {
public:
	string s;

	KifPerso(KifCode* kifcode,KifElement* base) : KifBasic(kifcode,base,perso_type) {
		s="";
	}
	
	//Duplication of an element
	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		KifPerso* kperso=new KifPerso(KifCurrent(),NULL);
		kperso->s=s;
		return kperso;
	}


	//How to set the inner value
	bool Setvalue(KifElement* kval,KifElement* kindex,KifElement* domain) {
		s=kval->String();
		//always return true
		return true;
	}


	//Declaration
	//All our methods must have been declared in our kifpersomethod variable (which might take any name you want)
	KifElement* Declaration(string& n) {
		if (kifpersomethods.find(n)!=kifpersomethods.end())
			return kifpersomethods[n];
		//It might be a predefined function: list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void List(KifVector* fcts) {
		map<string,KifCallMethod*>::iterator it;
		for (it=kifpersomethods.begin();it!=kifpersomethods.end();it++)
			fcts->Push(new KifString(KifCurrent(),NULL,it->first));
	}
	
	KifElement* MethodSet(KifElement* pattern,KifDomain* domain,KifCallFunction* callfunc) {
		//callfunc contains the list of our parameters. 
		if (callfunc->parameters.size()!=1)
			return kifNULL;
		KifElement* kelement=callfunc->parameters[0]->Execute(kifNULL,domain,kifNULL,false);
		s=kelement->String();
		return kifTRUE;
	}

	KifElement* MethodSize(KifElement* pattern,KifDomain* domain,KifCallFunction* callfunc) {
		//callfunc contains the list of our parameters. 
		if (callfunc->parameters.size()!=0)
			return kifNULL;
		int i=s.size();
		return new KifInteger(KifCurrent(),NULL,i);
	}

	//ExecuteMethod must be implemented in order to execute our new KiF methods. 
	KifElement* ExecuteMethod(string name,KifElement* pattern,KifDomain* domain,KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*persomethods[name])(pattern,domain,callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
		s="";
	}
	
	void Clear() {
		//Your reset code here
		s="";
	}

	//How to interprete your class according to the basic following types:
	string String() {
		return s;
	}
	long Integer() {return 0;}
	double Float() {return 0;}
	bool Boolean() {
		if (s=="")
			return false;
		return true;
	}

	//You can overcome the following function behaviour
	 KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx) {
		return kifNULL;
	}
	 //union of two sets
	 KifElement* orset(KifElement* a,KifElement* b) {
		return kifNULL;
	}

	 //intersection of two sets
	 KifElement* andset(KifElement* a,KifElement* b) {
		return kifNULL;
	}

	 //Beware that a might equal the current element
	 KifElement* plus(KifElement* a,KifElement* b) {
		 return kifNULL;
	}

	 KifElement* minus(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	 KifElement* multiply(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	 KifElement* divide(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	 KifElement* mod(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	 KifElement* shiftright(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	 KifElement* shiftleft(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	 KifElement* power(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	
	 //Comparison functions that might also be modified
	 KifElement* less(KifElement* a) {
		return kifFALSE;
	}
	 KifElement* more(KifElement* a) {
		return kifFALSE;
	}
	 KifElement* same(KifElement* a) {
		if (a==this)
			return kifTRUE;
		return kifFALSE;
	}
	 KifElement* different(KifElement* a) {
		if (a!=this)
			return kifTRUE;
		return kifFALSE;
	}
	 KifElement* lessequal(KifElement* a) {
		return kifFALSE;
	}
	 KifElement* moreequal(KifElement* a) {
		return kifFALSE;
	}

};


//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreateKifPersoElement(KifCode* kifcode,x_node* xn,KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable=kifcode->KifInitVariable(xn ,base);
	//which we associate with our object
	KifPerso* local=new KifPerso(kifcode,variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn,local,variable,base);
}

//EnrichPerso will add the right references to "name", which is always a new method associated to the object we are creating
static void EnrichPerso(string name,KifCode* kifcode,MethodPerso func) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	//It also tells kifcode about the new method names available.
	kifpersomethods[name]=new KifCallMethod(kifcode,name);
	persomethods[name]=func;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exported bool KifInitialisationModule(KifCode* kifcode) {
		kifcode->KifInitLib();
		///The new type will be: testage. You will be able to declare any variable with that type, once this library has been loaded...
		//CreateKifPersoElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: KifPerso)
		//Whenever the keyword testage will be found by KiF it will then call CreateKifPersoElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		perso_type=kifcode->KifAddNewType("testage",CreateKifPersoElement);
		if (perso_type==kifVOID)
			return false;
		//We need to create specific objects in order to have our right class method called
		//For instance in this example: set is the method name and KifPerso::MethodSet the method that should be invoked when set is called.
		EnrichPerso("set",kifcode,&KifPerso::MethodSet);
		EnrichPerso("size",kifcode,&KifPerso::MethodSize);
		return true;
	}
}
