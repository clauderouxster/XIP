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

//Replace KifTree by your own definition
short KifNewId(string n);
string KifIdString(short n);

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType KifTree_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class KifKifTree;
//This typedef defines a type "KifTreeMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (KifKifTree::*KifTreeMethod)(KifElement*,KifDomain*,KifCallFunction*);


class KifIteratorKifTree: public KifIteration {
public:
	KifKifTree* value;

	KifIteratorKifTree(KifCode* klc,KifElement* kp) : KifIteration(klc,kp) {
		value=NULL;
	}


	//The next methods are the ones that should be derived to add iterator capacities to KifKifTree
	KifElement* IteratorKey();
	KifElement* IteratorValue();
	KifElement* Next();
	KifElement* End();
	KifElement* Begin();
};


class KifKifTree : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short,KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short,KifTreeMethod> linkedmethods;
	static map<string,string> infomethods;

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
	//Your personal variables here...
//---------------------------------------------------------------------------------------------------------------------
	KifKifTree(KifCode* kifcode,KifElement* base) : KifBasic(kifcode,base,KifTree_type) {
		//Do not forget your variable initialisation
	}


	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval,KifElement* kindex,KifElement* domain,int idthread,KifCallFunction* callfunc) {
		return true;
	}

    //Very Important, you should always implement this function in order to distinguish this object from other objects
    //It can return whatever value you want
    int Yourowntype() {
		return KifTree_type;
	}

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		KifKifTree* kperso=new KifKifTree(KifCurrent(),NULL);
//Your initialisations here
		return kperso;
	}

	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//This description is facultative
	//iterator it=mydb;
	KifElement* Newiterator(bool rev) {
		KifIteratorKifTree* iter=new KifIteratorKifTree(kifcode,NULL);
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
		KifElement* kfirst=params->Instruction(0)->Exec(kifNULL,dom,idthread,callfunc); //would match i as in the example above: mynewtype toto(i,j,idthread,callfunc);
		KifElement* ksecond=params->Instruction(1)->Exec(kifNULL,dom,idthread,callfunc); //would match j as in the example above: mynewtype toto(i,j,idthread,callfunc);
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
		//you may return any value of course...
		return first;
	}
//---------------------------------------------------------------------------------------------------------------------

	//ExecMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name,KifElement* contextualpattern,KifDomain* domain,KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
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
	 KifElement* orset(KifElement* a,KifElement* b) {
		return kifNULL;
	}

	 //intersection of two sets
	 KifElement* andset(KifElement* a,KifElement* b) {
		return kifNULL;
	}

	 //Beware that a might be the current element
	 //plus: a+b
	 KifElement* plus(KifElement* a,KifElement* b) {
		 return kifNULL;
	}

	 //minus: a-b
	 KifElement* minus(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	 
	 //Multiplication: a*b
	 KifElement* multiply(KifElement* a,KifElement* b) {
		return kifNULL;
	}

	//Division: a/b
	 KifElement* divide(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	 //This is the division integer rest: a%b
	 KifElement* mod(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	 //This is a binary right shift: a>>b
	 KifElement* shiftright(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	 //This is a binary left shift: a<<b
	 KifElement* shiftleft(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	 //a to the power of b: a^b
	 KifElement* power(KifElement* a,KifElement* b) {
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
map<short,KifCallMethod> KifKifTree::kifexportedmethods;
map<short,KifTreeMethod>  KifKifTree::linkedmethods;
map<string,string> KifKifTree::infomethods;


//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific iterator implementation...
KifElement* KifIteratorKifTree::IteratorKey() {
	return kifNULL;
}

KifElement* KifIteratorKifTree::IteratorValue() {
	return kifNULL;
}
KifElement* KifIteratorKifTree::Next() {
	//reverse denotes the direction of iteration (from beginning to end or reverse)
	return IteratorValue();
}
KifElement* KifIteratorKifTree::End() {
//if end of iteration return kifTRUE, kifFALSE otherwise
	//return kifTRUE;
	return kifFALSE;
}
KifElement* KifIteratorKifTree::Begin() {
	//Your initialisation
	//reverse denotes the direction of iteration (from beginning to end or reverse)
	return IteratorValue();
}
//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifKifTreeElement(KifCode* kifcode,x_node* xn,KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable=kifcode->KifInitVariable(xn ,base);
	//which we associate with our object
	KifKifTree* local=new KifKifTree(kifcode,variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn,local,variable,base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(KifCode* kifcode,string name,KifTreeMethod func,int arity,string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname=KifNewId(name);
	KifCallMethod kcm(idname,arity);
	KifKifTree::kifexportedmethods[idname]=kcm;
    kifcode->kifExternalFunctions[idname]=&KifKifTree::kifexportedmethods[idname];
	KifKifTree::linkedmethods[idname]=func;
	KifKifTree::infomethods[name]=infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exported bool Init_KifTree(KifCode* kifcode,string version) {
		if (version!=KIFVERSION) {
			cerr<<"Incompatible version of KiF"<<endl;
			return false;
		}
		kifcode->KifInitLib();
		///The new type will be: KifTree. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifKifTreeElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: KifKifTree)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifKifTreeElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifTree_type=kifcode->KifAddNewType("KifTree",CreatekifKifTreeElement);
		if (KifTree_type==kifVOID)
			return false;
		return true;
	}

	Exported bool KifTree_KifInitialisationModule(KifCode* kifcode,string version) {
		if (Init_KifTree(kifcode,version)==false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization(kifcode,"sample",&KifKifTree::MethodSample,P_TWO,"sample(x,y): example of a method implementation");

		//If you want to create specific variables in the main frame		
		KifVariableDeclaration* var=kifcode->Createvariable("myvariable");
		//then we create its value
		KifKifTree* local=new KifKifTree(kifcode,var);
		return true;
	}
	Exported bool KifTree_Terminate(KifCode* kifcode) {
		return true;
	}
}

