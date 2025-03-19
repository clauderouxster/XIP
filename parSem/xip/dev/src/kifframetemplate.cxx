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
#include "x_node.h"



//Replace FrameTemplate by your own definition
short KifNewId(string n);
string KifIdString(short n);

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType FrameTemplate_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class KifFrameTemplate;
//This typedef defines a type "FrameTemplateMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (KifFrameTemplate::*FrameTemplateMethod)(KifElement*,KifDomain*,KifCallFunction*);


class KifIteratorFrameTemplate: public KifIteration {
public:
	KifFrameTemplate* value;

	KifIteratorFrameTemplate(KifCode* klc,KifElement* kp) : KifIteration(klc,kp) {
		value=NULL;
	}


	//The next methods are the ones that should be derived to add iterator capacities to KifFrameTemplate
	KifElement* IteratorKey();
	KifElement* IteratorValue();
	KifElement* Next();
	KifElement* End();
	KifElement* Begin();
};


class KifFrameTemplate : public KifFrame {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short,KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short,FrameTemplateMethod> linkedmethods;
	static map<string,string> infomethods;

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
	//Your personal variables here...
	//these two variables store their id string, which matches the variable names
	short idi;
	short ids;
//---------------------------------------------------------------------------------------------------------------------
	KifFrameTemplate(KifCode* kcode,KifElement* base,short name) : KifFrame(kcode,base,name) {
		//Mandatory to update the new methods exposed by this frame
		map<short,KifCallMethod>::iterator it;
		for (it=kifexportedmethods.begin();it!=kifexportedmethods.end();it++)
			declarations[it->first]=&it->second;

		//We create our own fields. For the sake of our example an integer (i) and a string (s).
		idi=Newfield("i",new KifInteger(kcode,NULL,0));		
		ids=Newfield("s",new KifString(kcode,NULL,"ici"));
	}


	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval,KifElement* kindex,KifElement* domain) {
		return true;
	}

	KifElement* Information(KifElement* kstr) {
		string s=kstr->String();
		if (infomethods.find(s)==infomethods.end())
			return kifNULL;
		return new KifString(kifcode,NULL,infomethods[s]);
	}

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//This description is facultative
	//iterator it=mydb;
	KifElement* Newiterator(bool rev) {
		KifIteratorFrameTemplate* iter=new KifIteratorFrameTemplate(kifcode,NULL);
		iter->reverse=rev;
		//Your initialisation here for your iterator object
		return iter;
	}

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodSample(KifElement* contextualpattern,KifDomain* domain,KifCallFunction* callfunc) {
		//In our example, we have only two parameters
		if (callfunc->Size()!=2)
			//we raise an error otherwise, that could be catched in a try{}catch section in your KiF program
			kifcode->Returnerror("KIF(800): Wrong number of parameters");

		//0 is the first parameter and so on...
		KifElement* first=callfunc->Evaluate(0,domain);
		KifElement* second=callfunc->Evaluate(1,domain);
		
		//Then we search for our own local variables
		KifElement* vi=Getfield(idi,domain);
		KifElement* vs=Getfield(ids,domain);
		vi->plus(vi,first,false);
		vs=vs->plus(vs,second,false);
		return kifTRUE;
	}

	KifElement* MethodString(KifElement* contextualpattern,KifDomain* domain,KifCallFunction* callfunc) {
		//No parameters
		if (callfunc->Size()!=0)
			//we raise an error otherwise, that could be catched in a try{}catch section in your KiF program
			kifcode->Returnerror("KIF(800): Wrong number of parameters");

		KifElement* vs=Getfield(ids,domain);
		return vs;
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


//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific implementation...
//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

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
map<short,KifCallMethod> KifFrameTemplate::kifexportedmethods;
map<short,FrameTemplateMethod>  KifFrameTemplate::linkedmethods;
map<string,string>  KifFrameTemplate::infomethods;

static KifFrameTemplate* localDefinition=NULL;

//---------------------------------------------------------------------------------------------------------------------
//This SECTION is for your specific iterator implementation...
KifElement* KifIteratorFrameTemplate::IteratorKey() {
	return kifNULL;
}

KifElement* KifIteratorFrameTemplate::IteratorValue() {
	return kifNULL;
}
KifElement* KifIteratorFrameTemplate::Next() {
	//reverse denotes the direction of iteration (from beginning to end or reverse)
	return IteratorValue();
}
KifElement* KifIteratorFrameTemplate::End() {
//if end of iteration return kifTRUE, kifFALSE otherwise
	//return kifTRUE;
	return kifFALSE;
}
KifElement* KifIteratorFrameTemplate::Begin() {
	//Your initialisation
	//reverse denotes the direction of iteration (from beginning to end or reverse)
	return IteratorValue();
}
//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifFrameTemplateElement(KifCode* kifcode,x_node* xn,KifElement* base) {
	string name=xn->nodes[0]->value;
	//Mandatory call to initialise our variable
	KifElement* variable=kifcode->KifInitVariable(xn ,base);
	//which we associate with our object declaration
	if (localDefinition==NULL)
		localDefinition=new KifFrameTemplate(kifcode,NULL,KifNewId(name));

	variable->Add(localDefinition);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn,variable,variable,base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name,FrameTemplateMethod func,int arity,string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname=KifNewId(name);
	KifCallMethod kcm(idname,arity);
	KifFrameTemplate::kifexportedmethods[idname]=kcm;
    kifGlobals->RecordExternalFunction(idname,arity);
	KifFrameTemplate::linkedmethods[idname]=func;
	KifFrameTemplate::infomethods[name]=infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exported bool Init_FrameTemplate(KifCode* kifcode,string version) {
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

		///The new type will be: FrameTemplate. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifFrameTemplateElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: KifFrameTemplate)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifFrameTemplateElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		FrameTemplate_type=KifAddNewType("FrameTemplate",CreatekifFrameTemplateElement);
		if (FrameTemplate_type==kifVOID)
			return false;
		return true;
	}

	Exported bool FrameTemplate_KifInitialisationModule(KifCode* kifcode,string version) {
		if (Init_FrameTemplate(kifcode,version)==false)
			return false;
		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization("sample",&KifFrameTemplate::MethodSample,P_TWO,"sample(x,y): implementation example");
		//No String() method or Integer() in this case. You need to provide it yourself
		MethodInitialization("string",&KifFrameTemplate::MethodString,P_NONE,"string(): String implementation");
		return true;
	}
	Exported bool FrameTemplate_Terminate(KifCode* kifcode) {
		return true;
	}
}

