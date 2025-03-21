// kifdll.cpp : Defines the exported functions for the DLL application.
//

#include "kif.h"

static KifType perso_type;
static map<string,KifCallMethod*> kifpersomethods;

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

	//What happens when a KifPerso element is evaluated
	KifElement* Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple=false) {
		return new KifString(KifCurrent(),NULL,s);
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
		return NULL;
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

	//ExecuteMethod must be implemented in order to execute our methods. This method is different from Execute, which is called for
	//a single object without any methods attached to it.
	KifElement* ExecuteMethod(string name,KifElement* pattern,KifDomain* domain,KifCallFunction* callfunc) {
		if (name=="set")
			return MethodSet(pattern,domain,callfunc);

		if (name=="size")
			return MethodSize(pattern,domain,callfunc);
		
		return kifNULL;
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


//Browsing function to evaluate the new type
KifElement* ANewType(KifCode* kifcode,x_node* xn,KifElement* base) {
	KifElement* variable=kifcode->KifInitVariable(xn ,base);
	KifPerso* local=new KifPerso(kifcode,variable);
	return kifcode->KifTerminateVariable(xn,local,variable,base);
}

extern "C" {
	Exported void KifInitialisationModule(KifCode* kifcode) {
		kifcode->KifInitLib();
		perso_type=kifcode->KifAddNewType("testage",ANewType);
		//We need to create specific objects in order to have our right functions called
		kifpersomethods["set"]=new KifCallMethod(kifcode,"set");
		kifpersomethods["size"]=new KifCallMethod(kifcode,"size");
	}
}
