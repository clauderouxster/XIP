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

#include "automate.h"
class kifstatres {
public:
	KifMapStringFloat* kvectres;
	KifMap* kactions;
	KifVector* splitwords;
	vector<hmap<int, string> >* actions;
	vector<string> wrds;

	kifstatres() {
		kvectres = NULL;
		splitwords = NULL;
		actions = NULL;
		kactions = NULL;
	}

	kifstatres(KifMapStringFloat* k, KifVector* v, vector<hmap<int, string> >* a, KifMap* ka) {
		kvectres = k;
		splitwords = v;
		actions = a;
		kactions = ka;
	}

	void updateactions(string& k);
};

class kifstate {
public:
	char status;
	automap<BULONG, kifstate> arcs;

	kifstate() {
		status = 0;
	}
	void addword(agnostring& w, int i, KifAutomaton& a);
	void addworddouble(agnostring& w, agnostring& lemma, int i, int j, KifAutomaton& a);
	void dump(string upper, string lower, ofstream& dumping, KifAutomaton& a);
	void store(ofstream& dumping, map<BULONG, short>& alphabet);
	void getalphabet(map<BULONG, short>& alphabet);
	bool loadcompact(FILE* f, map<short, BULONG>& alphabet, short& v, unsigned char& st);
	void clear() {
		arcs.clear();
	}
	bool search(agnostring& e, int pos);
	void search(agnostring& e, string base, int pos, KifAutomaton& a, vector<string>& v, int nb);
	void getfeature(KifAutomaton& a, string& lemma, string& surface, kifstatres& rs, double score, short flags);
	void getfeature(KifAutomaton& a, string base, KifMap* kvectres, KifVector* score);
	bool evaluate(KifDomain* d, KifAutomaton& a, KifCallFunctionGeneral& kfunc, vector<hmap<string, string> >& actions, KifElement* o, agnostring& e, agnostring& bb, int pos, double score, KifMap* kvectres);
	bool editdistance(KifDomain* d, KifAutomaton& a, agnostring& e, agnostring& lemma, agnostring& surface, int pos, double score, int threashold, short flags, kifstatres& rs);
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
	hmap<int, string> ifeatures;
	hmap<string, int> features;
	hmap<long, hmap<long, double> > paths;
	bool usepaths;
	bool spotfinalstate;
	short threadowner;

	KifAutomaton(KifCode* kcode, KifElement* base) : KifElement(kcode, base, kifAutomaton), _locker(NULL, false) {
		spotfinalstate = false;
		usepaths = false;
		codefeat = 0;
		function = NULL;
		object = kifNULL;
		nbwords = 0;
		if (base != NULL)
			function = base->Robfunction(this);
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
	Exported KifElement* Store(KifElement*, KifDomain*);
	KifElement* loadcompact(string path, string& featchar);

	//If initialconstructor returns true, then you might want to benefit from these parameters
	KifElement* constructor(KifDomain* domloc, KifElement* params, int idthread, KifCallFunction*);

	bool load(string& path, string& featchar);
	bool loaddouble(string& path, string& featchar);
	void addword(agnostring& w);
	void addworddouble(agnostring& w, agnostring& lf);

	void addWord(agnostring& w);
	void addWorddouble(agnostring& w, agnostring& lf);

	KifElement* evaluate(KifElement* contextualpattern, KifDomain* dom, KifElement* e, KifElement* o);
	KifElement* editdistance(KifElement* contextualpattern, KifDomain* dom, KifElement* e, KifElement* flags, KifElement* threashold);

	void setmodel(KifElement* f) {
		function = f;
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

	Exported KifElement* Atom(bool forced = false) {
		return this;
	}

	Exported KifType theType() {
		return kifAutomaton;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifAutomaton* kf = new KifAutomaton(kifcode, NULL);
		kf->function = function;
		return kf;
	}

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifAutomaton(kifcode, NULL);
	}

	string String() {
		return pathname;
	}

	long Size() {
		return  nbwords;
	}

	KifElement* Exec(KifElement* pattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc);
	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);

	void Clear() {
		codefeat = 0;
		start.clear();
	}


	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		return  nbwords;
	}

	double Float() {
		return  nbwords;
	}

	~KifAutomaton();
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

class gramstate;
class KifGrammar;
class KifGrammarBaseAutomaton;
class KifGrammarAutomaton;

class gramarc {
public:
	gramstate* state;
	gramstate* from;
	wstring wlabel;
	short idrule;
	unsigned char status;

	gramarc();

	virtual char test(wstring& c);
	virtual int test(wstring& l, wstring& r, int i);
	char test(wchar_t c);
};

class autobasekifvector;
class autobasekif {
public:
	static vector<autobasekif*> garbage;
	static void cleanall();

	int gint;

	autobasekif(bool g = true) {
		gint = -1;
		if (g) {
			gint = autobasekif::garbage.size();
			autobasekif::garbage.push_back(this);
		}
	}

	~autobasekif() {
		if (gint != -1) {
			if (gint == autobasekif::garbage.size() - 1)
				autobasekif::garbage.pop_back();
			else
				autobasekif::garbage[gint] = NULL;
		}
	}

	virtual void Merge(autobasekifvector*) {}
	virtual bool istring() {
		return false;
	}
	virtual void Clear() {}
	virtual void Push(autobasekif* v) {}
	virtual void storevalue(wstring w) {}
	virtual wstring value() { return L""; }
	virtual void Pop(int v) {}
	virtual int Size() { return 0; }
	virtual void addvalue(wstring w) {}
};

class autobasekifstring : public autobasekif {
public:
	wstring w;
	bool istring() {
		return true;
	}
	autobasekifstring(bool g = true) : autobasekif(g) {}
	autobasekifstring(wstring& x, bool g = true) : w(x), autobasekif(g) {}

	void Clear() {
		w = L"";
	}

	void storevalue(wstring x) {
		w += x;
	}
	wstring value() { return w; }
	int Size() { return w.size(); }

	void addvalue(wstring x) {
		w += x;
	}
};

class autobasekifvector : public autobasekif {
public:
	vector<autobasekif*> values;

	autobasekifvector(bool g = true) : autobasekif(g) {}

	void storevalue(wstring w) {
		values.push_back(new autobasekifstring(w));
	}

	void addvalue(wstring w) {
		if (values.size() == 0)
			values.push_back(new autobasekifstring(w));
		else
			values.back()->addvalue(w);
	}

	void Pop(int v) {
		autobasekif* a;
		if (v == -1) {
			a = values.back();
			values.pop_back();
		}
		else {
			a = values[v];
			values.erase(values.begin() + v);
		}
		delete a;
	}

	void Merge(autobasekifvector* s) {
		for (int i = 0; i < s->values.size(); i++)
			values.push_back(s->values[i]);
	}

	void Clear() {
		values.clear();
	}

	void Push(autobasekif* v) {
		values.push_back(v);
	}

	int Size() { return values.size(); }
};


class gramstate {
public:
	unsigned char status;
	vector<gramarc*> arcs;

	gramstate() {
		status = 0;
	}

	char comparemap(int idthread, wstring& lkey, KifGrammarBaseAutomaton* gram, KifDomain* dom, vector<wstring>& labels, int& i, KifEvaluateMap* m, bool);
	char compare(int idthread, KifGrammarBaseAutomaton* gram, KifDomain* dom, vector<wstring>& labels, int& i, KifElement* v, bool);

	char compare(KifGrammarBaseAutomaton* gram, vector<wstring>& labels, int& i, autobasekif* v);

	char comparemap(int idthread, wstring& lkey, KifGrammarBaseAutomaton* gram, KifDomain* dom, wstring& labels, int& i, KifEvaluateMap* m, bool);
	char compare(int idthread, KifGrammarBaseAutomaton* gram, KifDomain* dom, wstring& labels, int& i, KifElement* v, bool);
};

class KifGrammarBaseAutomaton {
public:
	KifCode* kifcode;
	KifElement* function;

	vector<gramstate*> states;
	vector<gramarc*> arcs;
	vector<gramstate*> rules;

	KifGrammarBaseAutomaton(KifCode* k) {
		kifcode = k;
		function = NULL;
	}

	~KifGrammarBaseAutomaton() {
		int i;

		for (i = 0; i < states.size(); i++)
			delete states[i];

		for (i = 0; i < arcs.size(); i++)
			delete arcs[i];
	}
	bool callfunction(int idthread, KifDomain* dom, string&, KifElement*, int pos);
	bool callfunction(int idthread, KifDomain* dom, wstring&, KifElement*, int pos);
	bool callfunction(KifElement* func, int idthread, KifDomain* dom, KifElement*, int pos);

};

class subgrammar : public gramarc {
public:

	gramstate substate;
	KifGrammarBaseAutomaton gram;

	subgrammar(KifGrammarBaseAutomaton* g) : gram(g->kifcode) {}

	char test(wstring& c);
	int test(wstring& l, wstring& r, int i);
};


class KifGrammarAutomaton : public KifGrammarBaseAutomaton {
public:

	hmap<wstring, int> dictionary;
	hmap<int, wstring> rdictionary;

	KifGrammarAutomaton(KifCode* k) : KifGrammarBaseAutomaton(k) {}

	KifElement* Apply(int, KifElement* res, KifDomain* dom, wstring& labels, bool asstring);
	KifElement* Apply(int, KifElement* res, KifDomain* dom, vector<wstring>& labels, bool asstring);
	bool Apply(vector<wstring>& labels, autobasekifvector& res);
};


class KifGrammar : public KifElement {
public:

	bool clean;
	KifGrammarAutomaton* automaton;
	KifElement* function;

	KifGrammar(KifCode* kcode, KifElement* base) : KifElement(kcode, base, kifGrammar) {
		function = NULL;
		if (base != NULL)
			function = base->Robfunction(this);
		clean = false;
		automaton = NULL;
	}

	~KifGrammar() {
		if (clean)
			delete automaton;
	}


	bool testfunction(KifElement* kfunc) {
		if (kfunc != NULL && kfunc->isFunction())  {
			if (kfunc->Size() != 3)
				return false;
		}
		return true;
	}

	Exported bool initialconstructor() { return true; }
	Exported KifElement* constructor(KifDomain*, KifElement*, int, KifCallFunction*);

	KifElement* Loader(string& v);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifGrammar(kifcode, NULL);
	}


	bool Variable() {
		return true;
	}

	bool Basic() {
		return true;
	}

	bool Statique() {
		return true;
	}

	Exported KifElement* Atom(bool forced = false) {
		return this;
	}

	Exported KifType theType() {
		return kifGrammar;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifGrammar* kf = new KifGrammar(kifcode, NULL);
		kf->automaton = automaton;
		kf->function = function;
		return kf;
	}

	KifElement* Apply(int, KifElement* v, KifDomain* dom, vector<wstring>& labels, bool asstring);
	bool Apply(vector<wstring>& labels, autobasekifvector&);
	KifElement* Apply(int, KifElement* v, KifDomain* dom, wstring& labels, bool asstring);
	KifElement* Exec(KifElement* pattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
		return this;
	}

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);
};

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

class KifTransducer : public KifElement {
public:

	string name;
	Kif_Automaton* automaton;

	KifTransducer(KifCode* klc, KifElement* kp) : KifElement(klc, kp, kifTransducer) {
		automaton = NULL;
	}

	~KifTransducer() {
		if (automaton != NULL)
			delete automaton;
	}

	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return new KifTransducer(kifcode, NULL);
	}

	bool Basic() {
		return true;
	}

	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement* kstr);

	KifElement* Declaration(short name);

	string String() {
		return name;
	}

	bool Boolean() {
		if (automaton == NULL)
			return false;
		return true;
	}

	Exported KifElement* constructor(KifDomain*, KifElement*, int, KifCallFunction*);

	Exported bool initialconstructor() {
		return true;
	}

	void Clean() {
		if (automaton != NULL)
			delete automaton;
		automaton = NULL;
	}

	void Clear() {
		if (automaton != NULL)
			delete automaton;
		automaton = NULL;
	}
};

#endif

