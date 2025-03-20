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
   filename   : kifautomaton.h
   Date       : 09/09/2010
   Purpose    : Implementation of the KiF language
   Programmer : Claude ROUX
   Reviewer   : 
*/


#ifndef i_kifautomaton
#define i_kifautomaton

#ifdef KIF_HASH_MAP
#ifdef WIN32
#define automap unordered_map
#else
#define automap map
#endif
#else
#define automap map
#endif

class kifstatres {
public:
	KifMapFloat* kvectres;
	KifMap* kactions;
	KifVector* splitwords;
	vector<hmap<int,string> >* actions;
	vector<string> wrds;

	kifstatres() {
		kvectres=NULL;
		splitwords=NULL;
		actions=NULL;
		kactions=NULL;
	}

	kifstatres(KifMapFloat* k,KifVector* v,vector<hmap<int,string> >* a,KifMap* ka) {
		kvectres=k;
		splitwords=v;
		actions=a;
		kactions=ka;
	}

	void updateactions(string& k);
};

class kifstate {
public:
	char status;
	automap<BULONG,kifstate> arcs;

	kifstate() {
		status=0;
	}
	void addword(agnostring& w,int i,KifAutomaton& a);
	void addworddouble(agnostring& w,agnostring& lemma,int i,int j,KifAutomaton& a);
	void dump(string upper,string lower,ofstream& dumping,KifAutomaton& a);
	void store(ofstream& dumping,map<BULONG,short>& alphabet);
	void getalphabet(map<BULONG,short>& alphabet);
	bool loadcompact(FILE* f,map<short,BULONG>& alphabet,short& v,unsigned char& st);
	void clear() {
		arcs.clear();
	}
	bool search(agnostring& e,int pos);
	void search(agnostring& e,string base,int pos,KifAutomaton& a,vector<string>& v,int nb);
	void getfeature(KifAutomaton& a,string& lemma,string& surface,kifstatres& rs,double score,short flags);
	void getfeature(KifAutomaton& a,string base,KifMap* kvectres,KifVector* score);
	bool evaluate(KifDomain* d,KifAutomaton& a,KifCallFunctionLocal& kfunc,vector<hmap<string,string> >& actions,KifElement* o,agnostring& e,agnostring& bb,int pos,double score,KifMap* kvectres);
	bool editdistance(KifDomain* d,KifAutomaton& a,agnostring& e,agnostring& lemma,agnostring& surface,int pos,double score,int threashold,short flags,kifstatres& rs);
};

class KifAutomaton : public KifElement {
public:
	KifThreadLock _locker;
	kifstate start;
	KifElement* function;
	string pathname;
	int nbwords;
	KifElement* object;
	BULONG codefeat;
	hmap<int,string> ifeatures;
	hmap<string,int> features;
	hmap<long,hmap<long,double> > paths;
	bool usepaths;
	bool spotfinalstate;
	short threadowner;

	KifAutomaton(KifCode* kcode,KifElement* base) : KifElement(kcode,base,kifAutomaton), _locker(NULL,false) {
		spotfinalstate=false;
		usepaths=false;
		codefeat=0;
		function=NULL;
		object=kifNULL;
		nbwords=0;
		if (base!=NULL)
			function=base->Robfunction(this);
	}

	bool testfunction(KifElement* kfunc) {
		if (kfunc != NULL && kfunc->isFunction())  {
			if (kfunc->Size() != 3 && kfunc->Size() != 7)
				return false;
		}
		return true;
	}

	bool initialconstructor() {
		return true;
	}
	
	void dump(string& path);
	Exported KifElement* Store(KifElement*,KifDomain*);
	KifElement* loadcompact(string path,string& featchar);

	//If initialconstructor returns true, then you might want to benefit from these parameters
	KifElement* constructor(KifDomain* domloc,KifElement* params,int idthread,KifCallFunction*);

	bool load(string& path,string& featchar);
	bool loaddouble(string& path,string& featchar);
	void addword(agnostring& w);
	void addworddouble(agnostring& w,agnostring& lf);

	void addWord(agnostring& w);
	void addWorddouble(agnostring& w,agnostring& lf);

	KifElement* evaluate(KifElement* contextualpattern,KifDomain* dom,KifElement* e,KifElement* o);
	KifElement* editdistance(KifElement* contextualpattern,KifDomain* dom,KifElement* e,KifElement* flags,KifElement* threashold);

	void setmodel(KifElement* f) {
		function=f;
	}

	KifElement* setpath(KifMap* m);
	KifElement* setcodefeat(string& c);

	bool Variable() {
		return true;
	}

	bool Basic() {
		return true;
	}

	bool Statique() {
		return true;
	}

	Exported KifElement* Atom(bool forced=false) {
		return this;
	}

	Exported KifType theType() {
		return kifAutomaton;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		KifAutomaton* kf=new KifAutomaton(kifcode,NULL);
		kf->function=function;
		return kf;
	}

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifAutomaton(kifcode,NULL);
	}

	string String() {
		return pathname;
	}

	long Size() {		
		return  nbwords;
	}

	KifElement* Execute(KifElement* pattern,KifDomain* dom,KifElement* idx);
	bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL,int idthread=0,KifCallFunction* callfunc=NULL);

	void Clear() {
		codefeat=0;
		start.clear();
	}


	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);

	long Integer() {
		return  nbwords;
	}

	double Float() {
		return  nbwords;
	}

	~KifAutomaton();
};

#endif

