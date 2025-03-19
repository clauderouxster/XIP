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
filename   : kif.h
Date       : 09/09/2010
Purpose    : Implementation of the KiF language
Programmer : Claude ROUX
Reviewer   :
*/


#ifndef i_kifcontainer
#define i_kifcontainer

#ifdef WIN32
#include <wchar.h>
#endif

#ifndef swprintf_s
#define swprintf_s(a,b,c,d) swprintf(a,b,c,d)
#endif

#define KIFCIRCULARDETECTION -25

//Some set definitions...
struct set_compare
{
	bool operator()(KifElement* e1, KifElement* e2) const
	{
		char tstcontainer = e1->aContainer() + e2->aContainer();

		if (e1->type == e2->type || tstcontainer == 2)
			return e1->less(e2)->Boolean();

		if (!tstcontainer && (e1->inCode() | e2->inCode()) <= ckifIntFloat)
			return e1->less(e2)->Boolean();

		return (e1->type < e2->type);
	}

};

#define kifset std::set<KifElement*, set_compare>
#define kifsetiterator std::set<KifElement*, set_compare>::iterator
#define kifsetriterator std::set<KifElement*, set_compare>::reverse_iterator

///-------------------------------

KifElement* Mapcompare(KifElement*a, KifElement*b);

class KifList : public KifEvaluate {
public:
	list<KifElement*> values;

	int Garbager() {
		return kifList;
	}

	KifList(KifCode* klc, KifElement* kp) : KifEvaluate(klc, kp, kifList) {}

	Exported KifElement* Newinstance() {
		return new KifList(kifcode, NULL);
	}

	void storeInLoop(KifElement** alls);

	bool Permute();
	void Shuffle();
	KifElement* Unique();

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i = s->Integer();
		if (i < 0 || i >= values.size())
			return kifFALSE;
		return kifTRUE;
	}

	void Merge(KifElement* ke);
	Exported KifElement* Newiterator(bool);
	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);

	bool listValue(list<KifElement*>::iterator& it, int itx) {
		KifContainerLock _lock(this);
		if (itx >= values.size())
			return false;
		it = values.begin();
		while (itx > 0) {
			it++;
			itx--;
		}
		return true;
	}

	Exported void Add(KifElement* e) {
		KifContainerLock _lock(this);
		values.push_back(e);
	}


	Exported void Insert(int idx, KifElement* ke);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		list<KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v += (*itx)->Sum();
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		list<KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v *= (*itx)->Product();
		return v;
	}

	Exported void PushLast(KifElement* ke);
	Exported void Push(KifElement* ke);
	Exported void PushFirst(KifElement* ke);
	void PopLast();
	void Pop(KifElement* ke);
	void PopFirst();


	KifElement* Last() {
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return kifNULL;
		return values.back();
	}

	KifElement* First() {
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return kifNULL;
		return values.front();
	}

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);
	KifElement* ValueOnIndex(size_t ikey) {
		KifContainerLock _lock(this);
		if (ikey < 0 || ikey >= values.size())
			return kifNOELEMENT;
		list<KifElement*>::iterator it = values.begin();
		int i;
		for (i = 0; i < ikey; i++) it++;
		return *it;
	}


	//Clear cleans the vector and free some of the inner instances
	//There is slight difference with the next function. In this case,
	//we do not want to interfere with an instance that would be a map or
	//a vector and free its content, while it should still be alive...
	//Clean is called recursively to clean all instances, then itself
	Exported void Setreference(int inc = 1);

	Exported void Setpopped(int v = 1, bool force = false) {
		KifContainerLock _lock(this);
		if (reference <= 0 || force) {
			Putpopped(v);
			list<KifElement*>::iterator itx;
			for (itx = values.begin(); itx != values.end(); itx++)
				(*itx)->Setpopped(v, force);
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		list<KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			(*itx)->Clean();
		values.clear();
		if (reference <= 0)
			KifElement::Resetreference();

	}


	Exported void Resetreference(int inc = 1);

	Exported void Clear();


	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);
	Exported long Integer() {
		KifContainerLock _lock(this);
		return values.size();
	}
	Exported double Float() {
		KifContainerLock _lock(this);
		return values.size();
	}

	Exported string String();
	Exported string JSonString();
	Exported string BaseString();

	Exported long Size() {
		KifContainerLock _lock(this);
		return values.size();
	}

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* same(KifElement* a);

};


class KifBaseVector;

class KifVector : public KifEvaluate {
public:
	vector<KifElement*> values;

	KifVector(KifCode* klc, KifElement* kp);

	Exported KifElement* Newinstance() {
		return new KifVector(kifcode, NULL);
	}

	bool Statique() {
		return true;
	}

	void addstringto(string s, int i);
	void addstringto(wchar_t s, int i);
	void addustringto(wstring s, int i);

	bool aVectorContainer() {
		return true;
	}

	KifElement* Unique();
	void storeInLoop(KifElement** alls);
	unsigned int EditDistance(KifElement*);

	void Reserve(int i) {
		KifContainerLock _lock(this);
		values.reserve(i);
	}

	virtual int Garbager() {
		return kifVector;
	}

	KifElement* Split(KifBaseVector* params, KifCallFunction*);
	KifElement* Match(KifBaseVector* params, KifDomain* dom, int, KifCallFunction*);
	KifElement* MatchPattern(KifElement* params, KifDomain* dom, int, KifCallFunction*);

	bool Permute();
	void Shuffle();

	KifElement* Index(KifElement* s) {
		long i = s->Integer();
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Newiterator(bool);
	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	Exported virtual KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);
	KifElement* Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, bool);
	KifElement* FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool);

	long getinteger(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return values[i]->Integer();
	}

	double getfloat(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return values[i]->Float();
	}

	string getstring(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return values[i]->String();
	}

	wstring getustring(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return values[i]->UString();
	}


	//Predicate management....
	bool unified(KifDomain* dom, KifElement* a);
	bool unifiedDeclaration(KifDomain* dom);
	virtual Exported KifElement* ExtractPredicateVariables(KifElement* contextualpattern, KifDomain* dom, KifElement* val, KifElement* e, bool root);
	Exported KifElement* EvaluePredicateVariables(KifElement* context, KifDomain* dom);
	KifElement* Getvalues(KifDomain* dom, bool duplicate);
	Exported bool Insertvalue(KifDomain* dom, KifElement* v, bin_hash<KifElement*>&);

	virtual Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);
	virtual Exported KifElement* Vector(KifElement*, KifVector* itself = NULL) {
		return this;
	}
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		for (int i = 0; i < values.size(); i++)
			v += values[i]->Sum();
		return v;
	}


	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		for (int i = 0; i < values.size(); i++)
			v *= values[i]->Product();
		return v;
	}

	bool aVector() {
		return true;
	}


	virtual void Resetreference(int inc = 1);

	virtual void Clear();
	void Merge(KifElement* ke);

	Exported virtual void Push(KifElement* ke);
	Exported void Insert(int idx, KifElement* ke);
	void Pop(KifElement* ke);

	KifElement* Last() {
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return kifNULL;
		return values.back();
	}

	KifElement* ValueOnIndex(size_t i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return kifNOELEMENT;
		return values[i];
	}

	virtual void Add(KifElement* e) {
		KifContainerLock _lock(this);
		e->Setreference();
		values.push_back(e);
	}


	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	//Clear cleans the vector and free some of the inner instances
	//There is slight difference with the next function. In this case,
	//we do not want to interfere with an instance that would be a map or
	//a vector and free its content, while it should still be alive...
	//Clean is called recursively to clean all instances, then itself
	Exported void Setreference(int inc = 1);

	Exported void Setpopped(int v = 1, bool force = false) {
		if (reference <= 0 || force) {
			KifContainerLock _lock(this);
			Putpopped(v);
			for (int itx = 0; itx < values.size(); itx++)
				values[itx]->Setpopped(v, force);
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		for (int i = 0; i < values.size(); i++)
			values[i]->Clean();
		values.clear();
		if (reference <= 0)
			KifElement::Resetreference();
	}



	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);
	Exported long Integer() {
		KifContainerLock _lock(this);
		return values.size();
	}
	Exported double Float() {
		KifContainerLock _lock(this);
		return values.size();
	}

	virtual Exported string String();
	Exported string JSonString();
	Exported string BaseString();

	Exported long Size() {
		KifContainerLock _lock(this);
		return values.size();
	}

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* same(KifElement* a);

	KifElement* apply(KifCallFunctionGeneral* kfunc, KifElement* c, KifDomain* dom, KifCallFunction* callfunc);
};


class KifInfinitiveVector : public KifEvaluate {
public:

	KifElement* seed;
	KifElement* step;
	int direction;
	bool clean;
	bool compute;

	KifInfinitiveVector(KifCode* klc, KifElement* kp, int d) : KifEvaluate(klc, kp, kifInfinitiveVector) {
		clean = false;
		seed = kifNULL;
		compute = false;
		if (d == 1)
			step = kifONE;
		else
			step = kifMINUSONE;
		direction = d;
	}

	~KifInfinitiveVector() {
		if (clean)
			step->Resetreference();
	}

	Exported KifElement* Newinstance() {
		return new KifVector(kifcode, NULL);
	}

	bool Statique() {
		return true;
	}


	bool aVectorContainer() {
		return true;
	}

	Exported KifElement* Newiterator(bool);

	Exported void Add(KifElement* s) {
		if (seed == kifNULL)
			seed = s;
		else {
			step = s->Atom();
			step->Setreference();
			clean = true;
		}
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*) {
		return this;
	}

	Exported string String();
	long Size() {
		return 0;
	}

	bool Infinite() {
		return true;
	}
};

class KifIteratorInfiniteveVector : public KifIterationContainer {
public:
	long itx;
	int inc;

	KifIteratorInfiniteveVector(KifCode* klc, KifElement* kp, KifInfinitiveVector* k) : KifIterationContainer(klc, kp, k) {}

	Exported KifElement* IteratorKey() {
		return kifcode->Provideinteger(itx);
	}

	long IteratorKeyInteger() {
		return itx;
	}

	double IteratorKeyFloat() {
		return itx;
	}

	Exported KifElement* IteratorValue() {
		return kifcode->Provideinteger(itx);
	}

	Exported void Next() {
		itx += inc;
	}

	Exported KifElement* End() {
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		return kifTRUE;
	}
};

class KifIteratorInfiniteveFloatVector : public KifIterationContainer {
public:
	double itx;
	double inc;

	KifIteratorInfiniteveFloatVector(KifCode* klc, KifElement* kp, KifInfinitiveVector* k) : KifIterationContainer(klc, kp, k) {}

	Exported KifElement* IteratorKey() {
		return kifcode->Provideinteger(itx);
	}

	long IteratorKeyInteger() {
		return itx;
	}

	double IteratorKeyFloat() {
		return itx;
	}

	Exported KifElement* IteratorValue() {
		return kifcode->Providefloat(itx);
	}

	Exported void Next() {
		itx += inc;
	}

	Exported KifElement* End() {
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		return kifTRUE;
	}
};

class KifIteratorList : public KifIterationContainer {
public:
	int itx;
	list<KifElement*>* values;
	list<KifElement*>::iterator it;

	KifIteratorList(KifCode* klc, KifElement* kp, KifList* k) : itx(0), KifIterationContainer(klc, kp, k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
		return kifcode->Provideinteger(itx);
	}

	long IteratorKeyInteger() {
		return itx;
	}

	double IteratorKeyFloat() {
		return itx;
	}

	KifElement* listValue() {
		it = values->begin();
		for (int i = 0; i < itx; i++)
			it++;
		return *it;
	}

	Exported KifElement* IteratorValue() {
		if (itx >= values->size() || itx < 0)
			return kifNOELEMENT;
		return listValue();
	}

	Exported void Next() {
		if (reverse == false)
			itx++;
		else
			itx--;
	}

	Exported KifElement* End() {
		if (itx >= values->size() || itx < 0)
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		if (values->size() == 0)
			return kifFALSE;
		if (reverse == false)
			itx = 0;
		else
			itx = values->size() - 1;
		return kifTRUE;
	}
};


class KifIteratorVector : public KifIterationContainer {
public:
	int itx;
	vector<KifElement*>* values;
	KifIteratorVector(KifCode* klc, KifElement* kp, KifVector* k) : itx(0), KifIterationContainer(klc, kp, k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
		return kifcode->Provideinteger(itx);
	}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse == false) {
			if (itx == values->size())
				return;
		}
		else
		if (itx < 0)
			return;
		(*values)[itx]->Resetreference(reference + 1);
		ke->Setreference(reference + 1);
		(*values)[itx] = ke;
	}

	long IteratorKeyInteger() {
		return itx;
	}

	double IteratorKeyFloat() {
		return itx;
	}

	Exported KifElement* IteratorValue() {
		if (itx >= values->size() || itx < 0)
			return kifNOELEMENT;
		return (*values)[itx];
	}

	Exported void Next() {
		if (reverse == false)
			itx++;
		else
			itx--;
	}

	Exported KifElement* End() {
		if (itx >= values->size() || itx < 0)
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		if (values->size() == 0)
			return kifFALSE;
		if (reverse == false)
			itx = 0;
		else
			itx = values->size() - 1;
		return kifTRUE;
	}
};

class KifBaseVector : public KifVector {
public:
	char merge;

	KifBaseVector(KifCode* klc, KifElement* kp) : KifVector(klc, kp) {
		type = kifBaseVector;
		merge = 0;
	}

	Exported KifElement*  Linkedvalue() { return this; }

	void Release() {}

	bool Basevalue() {
		return true;
	}

	int Garbager() {
		return 0;
	}

	void Add(KifElement* e) {
		KifContainerLock _lock(this);
		values.push_back(e);
	}

	Exported void Push(KifElement* ke) {
		KifContainerLock _lock(this);
		ke->Setreference();
		values.push_back(ke);
	}

	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);
	Exported KifElement* DuplicateHardCodedValue(KifDomain* dom);
	Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL) {
		return true;
	}

	void Clear() {}
	int Ingarbage() {
		return idgarbage;
	}

	Exported void Resetreference(int inc = 1) {
		KifContainerLock _lock(this);
		for (int i = 0; i < values.size(); i++) {
			values[i]->Resetpopped();
			values[i]->Resetreference(inc);
		}
		KifElement::Resetreference(inc);
	}

};

class KifBaseVectorMerge : public KifBaseVector {
public:

	KifBaseVectorMerge(KifCode* klc, KifElement* kp) : KifBaseVector(klc, kp) {
		type = kifBaseVectorMerge;
	}
	Exported string String();
	Exported KifElement* ExtractPredicateVariables(KifElement* contextualpattern, KifDomain* dom, KifElement* val, KifElement* e, bool root);
};


class KifVectorString : public KifEvaluate {
public:
	vector<string> values;

	KifVectorString(KifCode* klc, KifElement* kp) : KifEvaluate(klc, kp, kifVectorString) {}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	void addstringto(string s, int i);
	void addstringto(wchar_t s, int i);
	void addustringto(wstring s, int i);

	void storevalue(wstring v) {
		KifContainerLock _lock(this);
		string s;
		s_unicode_to_utf8(s, v);
		values.push_back(s);
	}


	void storevalue(wchar_t v) {
		KifContainerLock _lock(this);
		values.push_back(c_unicode_to_utf8(v));
	}


	void storevalue(string v) {
		KifContainerLock _lock(this);
		values.push_back(v);
	}

	void storevalue(char* v) {
		KifContainerLock _lock(this);
		values.push_back(v);
	}

	char inCode() {
		return ckifString;
	}

	KifElement* Split(KifBaseVector* params, KifCallFunction*);
	KifElement* Match(KifBaseVector* params, KifDomain* dom, int, KifCallFunction*);
	KifElement* MatchPattern(KifElement* params, KifDomain* dom, int, KifCallFunction*);
	unsigned int EditDistance(KifElement*);


	bool Permute();
	void Shuffle();

	KifElement* Unique();

	bool Variable() {
		return true;
	}

	bool aValueVector() {
		return true;
	}

	void Merge(KifElement* ke);
	void storeInLoop(KifElement** alls);
	bool aValueContainer() {
		return true;
	}

	bool aVectorContainer() {
		return true;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i = s->Integer();
		if (i < 0 || i >= values.size())
			return kifFALSE;
		return kifTRUE;
	}

	void Reserve(int i) {
		KifContainerLock _lock(this);
		values.reserve(i);
	}

	Exported KifElement* Inverse() {
		KifVectorString* kvect = new KifVectorString(kifcode, NULL);
		KifContainerLock _lock(this);
		for (int i = values.size() - 1; i >= 0; i--)
			kvect->values.push_back(values[i]);
		return kvect;
	}

	Exported void Insert(int idx, KifElement* ke) {
		KifContainerLock _lock(this);
		values.insert(values.begin() + idx, ke->String());
	}

	Exported KifType theType() {
		return kifVectorString;
	}

	Exported void Sort(bool direction);

	KifElement* Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, bool);
	KifElement* FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool);

	KifElement* ValueOnIndex(size_t ikey) {
		KifContainerLock _lock(this);
		if (ikey < 0 || ikey >= values.size())
			return kifNOELEMENT;
		return kifcode->Providestring(values[ikey]);
	}

	long getinteger(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return conversionintegerhexa(STR(values[i]));
	}

	double getfloat(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return convertfloat(STR(values[i]));
	}

	string getstring(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return values[i];
	}

	wstring getustring(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		wstring wx;
		s_utf8_to_unicode(wx, STR(values[i]));
		return wx;
	}

	KifElement* Last() {
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return kifNULL;
		return kifcode->Providestring(values.back());
	}


	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifVectorString(kifcode, NULL);
	}

	string String();
	string JSonString();
	Exported void Push(KifElement*);

	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);

	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);


	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	KifElement* apply(KifCallFunctionGeneral* kfunc, KifElement* c, KifDomain* dom, KifCallFunction* callfunc);
};

class KifIteratorVectorString : public KifIterationContainer {
public:
	vector<string>* values;
	int i;
	KifIteratorVectorString(KifCode* klc, KifElement* kp, KifVectorString* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse == false) {
			if (i == values->size())
				return;
		}
		else
		if (i < 0)
			return;
		(*values)[i] = ke->String();
	}

	KifElement* IteratorKey() {
		return kifcode->Provideinteger(i);
	}

	long IteratorKeyInteger() {
		return i;
	}

	double IteratorKeyFloat() {
		return i;
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (i == values->size())
				return kifNOELEMENT;
		}
		else
		if (i < 0)
			return kifNOELEMENT;
		return kifcode->Providestringraw((*values)[i]);
	}

	string IteratorValueString() {
		if (reverse == false) {
			if (i == values->size())
				return "";
		}
		else
		if (i < 0)
			return "";
		return (*values)[i];
	}

	wstring IteratorValueUString() {
		if (reverse == false) {
			if (i == values->size())
				return L"";
		}
		else
		if (i < 0)
			return L"";
		wstring w;
		s_utf8_to_unicode(w, STR((*values)[i]));
		return w;
	}

	void Next() {
		if (reverse == false)
			i++;
		else
			i--;
	}

	KifElement* End() {
		if (reverse == false) {
			if (i >= values->size())
				return kifTRUE;
		}
		else {
			if (i < 0)
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		i = values->size();
		if (i == 0)
			return kifFALSE;

		if (reverse == false)
			i = 0;
		else
			i--;

		return kifTRUE;
	}
};

class KifVectorUString : public KifEvaluate {
public:
	vector<wstring> values;

	KifVectorUString(KifCode* klc, KifElement* kp) : KifEvaluate(klc, kp, kifVectorUString) {}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	void addstringto(string s, int i);
	void addstringto(wchar_t s, int i);
	void addustringto(wstring s, int i);

	void storevalue(wstring v) {
		KifContainerLock _lock(this);
		values.push_back(v);
	}

	void storevalue(wchar_t v) {
		wchar_t w[] = { v, 0 };
		KifContainerLock _lock(this);
		values.push_back(w);
	}

	void storevalue(string v) {
		KifContainerLock _lock(this);
		wstring wx;
		s_utf8_to_unicode(wx, STR(v));
		values.push_back(wx);
	}

	void storevalue(char* v) {
		KifContainerLock _lock(this);
		wstring wx;
		s_utf8_to_unicode(wx, v);
		values.push_back(wx);
	}

	char inCode() {
		return ckifUString;
	}

	KifElement* Split(KifBaseVector* params, KifCallFunction*);
	KifElement* Match(KifBaseVector* params, KifDomain* dom, int, KifCallFunction*);
	KifElement* MatchPattern(KifElement* params, KifDomain* dom, int, KifCallFunction*);
	unsigned int EditDistance(KifElement*);


	bool Permute();
	void Shuffle();

	KifElement* Unique();

	bool Variable() {
		return true;
	}

	bool aValueVector() {
		return true;
	}

	void Merge(KifElement* ke);
	void storeInLoop(KifElement** alls);
	bool aValueContainer() {
		return true;
	}

	bool aVectorContainer() {
		return true;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i = s->Integer();
		if (i < 0 || i >= values.size())
			return kifFALSE;
		return kifTRUE;
	}

	void Reserve(int i) {
		KifContainerLock _lock(this);
		values.reserve(i);
	}

	Exported KifElement* Inverse() {
		KifVectorUString* kvect = new KifVectorUString(kifcode, NULL);
		KifContainerLock _lock(this);
		for (int i = values.size() - 1; i >= 0; i--)
			kvect->values.push_back(values[i]);
		return kvect;
	}

	Exported void Insert(int idx, KifElement* ke) {
		KifContainerLock _lock(this);
		values.insert(values.begin() + idx, ke->UString());
	}

	Exported KifType theType() {
		return kifVectorUString;
	}

	Exported void Sort(bool direction);

	KifElement* Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, bool);
	KifElement* FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool);

	KifElement* ValueOnIndex(size_t ikey) {
		KifContainerLock _lock(this);
		if (ikey < 0 || ikey >= values.size())
			return kifNOELEMENT;
		return kifcode->Provideustring(values[ikey]);
	}

	long getinteger(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return conversionintegerhexa(STR(values[i]));
	}

	double getfloat(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return convertfloat(STR(values[i]));
	}

	string getstring(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		string s;
		s_unicode_to_utf8(s, values[i]);
		return s;
	}

	wstring getustring(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return values[i];
	}

	KifElement* Last() {
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return kifNULL;
		return kifcode->Provideustring(values.back());
	}


	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifVectorUString(kifcode, NULL);
	}

	string String();
	wstring UString();
	string JSonString();
	Exported void Push(KifElement*);

	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);

	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);


	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	KifElement* apply(KifCallFunctionGeneral* kfunc, KifElement* c, KifDomain* dom, KifCallFunction* callfunc);
};

class KifIteratorVectorUString : public KifIterationContainer {
public:
	vector<wstring>* values;
	int i;
	KifIteratorVectorUString(KifCode* klc, KifElement* kp, KifVectorUString* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse == false) {
			if (i == values->size())
				return;
		}
		else
		if (i < 0)
			return;
		(*values)[i] = ke->UString();
	}

	KifElement* IteratorKey() {
		return kifcode->Provideinteger(i);
	}

	long IteratorKeyInteger() {
		return i;
	}

	double IteratorKeyFloat() {
		return i;
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (i == values->size())
				return kifNOELEMENT;
		}
		else
		if (i < 0)
			return kifNOELEMENT;
		return kifcode->Provideustring((*values)[i]);
	}

	string IteratorValueString() {
		if (reverse == false) {
			if (i == values->size())
				return "";
		}
		else
		if (i < 0)
			return "";
		string s;
		s_unicode_to_utf8(s, (*values)[i]);
		return s;
	}

	wstring IteratorValueUString() {
		if (reverse == false) {
			if (i == values->size())
				return L"";
		}
		else
		if (i < 0)
			return L"";
		return (*values)[i];
	}

	void Next() {
		if (reverse == false)
			i++;
		else
			i--;
	}

	KifElement* End() {
		if (reverse == false) {
			if (i >= values->size())
				return kifTRUE;
		}
		else {
			if (i < 0)
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		i = values->size();
		if (i == 0)
			return kifFALSE;

		if (reverse == false)
			i = 0;
		else
			i--;

		return kifTRUE;
	}
};

class KifVectorByte : public KifEvaluate {
public:
	vector<unsigned char> values;

	KifVectorByte(KifCode* klc, KifElement* kp) : KifEvaluate(klc, kp, kifVectorByte) {}

	bool Variable() {
		return true;
	}

	void Merge(KifElement* ke);
	unsigned int EditDistance(KifElement*);

	KifElement* Unique();

	bool aValueVector() {
		return true;
	}

	char inCode() {
		return ckifInteger;
	}

	KifElement* Split(KifBaseVector* params, KifCallFunction*);
	KifElement* Match(KifBaseVector* params, KifDomain* dom, int, KifCallFunction*);
	KifElement* MatchPattern(KifElement* params, KifDomain* dom, int, KifCallFunction*);

	void storevalue(long v) {
		KifContainerLock _lock(this);
		values.push_back((unsigned char)v);
	}


	bool Permute();
	void Shuffle();

	bool aValueContainer() {
		return true;
	}

	void storeInLoop(KifElement** alls);

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i = s->Integer();
		if (i < 0 || i >= values.size())
			return kifFALSE;
		return kifTRUE;
	}

	bool aVectorContainer() {
		return true;
	}

	void Reserve(int i) {
		KifContainerLock _lock(this);
		values.reserve(i);
	}

	Exported KifElement* Inverse() {
		KifVectorByte* kvect = new KifVectorByte(kifcode, NULL);
		KifContainerLock _lock(this);
		for (int i = values.size() - 1; i >= 0; i--)
			kvect->values.push_back(values[i]);
		return kvect;
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		for (int i = 0; i < values.size(); i++)
			v += values[i];
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return 0;
		double v = values[0];
		for (int i = 1; i < values.size(); i++)
			v *= values[i];
		return v;
	}

	Exported void Sort(bool direction);

	Exported void Insert(int idx, KifElement* ke) {
		values.insert(values.begin() + idx, ke->Integer());
	}

	Exported KifType theType() {
		return kifVectorByte;
	}

	KifElement* Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, bool);
	KifElement* FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool);

	KifElement* ValueOnIndex(size_t ikey) {
		KifContainerLock _lock(this);
		if (ikey < 0 || ikey >= values.size())
			return kifNOELEMENT;
		return new KifByte(kifcode, NULL, values[ikey]);
	}

	long getinteger(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return values[i];
	}

	double getfloat(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return values[i];
	}

	string getstring(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return "";
		return c_unicode_to_utf8(values[i]);
	}

	wstring getustring(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return L"";
		wchar_t ch[] = { values[i], 0 };
		return ch;
	}


	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifVectorByte(kifcode, NULL);
	}

	string String();
	Exported void Push(KifElement*);

	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);

	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);

	KifElement* Last() {
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return kifNULL;
		return new KifByte(kifcode, NULL, values.back());
	}

	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	KifElement* apply(KifCallFunctionGeneral* kfunc, KifElement* c, KifDomain* dom, KifCallFunction* callfunc);
};

class KifIteratorVectorByte : public KifIterationContainer {
public:
	vector<unsigned char>* values;
	int i;
	KifIteratorVectorByte(KifCode* klc, KifElement* kp, KifVectorByte* k) : KifIterationContainer(klc, kp, k) {}


	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse == false) {
			if (i == values->size())
				return;
		}
		else
		if (i < 0)
			return;
		(*values)[i] = ke->Byte();
	}

	KifElement* IteratorKey() {
		return kifcode->Provideinteger(i);
	}

	long IteratorKeyInteger() {
		return i;
	}

	double IteratorKeyFloat() {
		return i;
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (i == values->size())
				return kifNULL;
		}
		else
		if (i < 0)
			return kifNULL;
		return new KifByte(kifcode, NULL, (*values)[i]);
	}

	long IteratorValueInteger() {
		if (reverse == false) {
			if (i == values->size())
				return 0;
		}
		else
		if (i < 0)
			return 0;
		return (*values)[i];
	}

	double IteratorValueFloat() {
		if (reverse == false) {
			if (i == values->size())
				return 0;
		}
		else
		if (i < 0)
			return 0;
		return (*values)[i];
	}

	void Next() {
		if (reverse == false)
			i++;
		else
			i--;
	}

	KifElement* End() {
		if (reverse == false) {
			if (i >= values->size())
				return kifTRUE;
		}
		else {
			if (i < 0)
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		i = values->size();
		if (i == 0)
			return kifFALSE;

		if (reverse == false)
			i = 0;
		else
			i--;

		return kifTRUE;
	}
};

class KifVectorInteger : public KifEvaluate {
public:
	vector<long> values;

	KifVectorInteger(KifCode* klc, KifElement* kp) : KifEvaluate(klc, kp, kifVectorInteger) {}

	bool Variable() {
		return true;
	}

	void Merge(KifElement* ke);
	unsigned int EditDistance(KifElement*);

	KifElement* Unique();

	bool aValueVector() {
		return true;
	}

	char inCode() {
		return ckifInteger;
	}


	KifElement* Split(KifBaseVector* params, KifCallFunction*);
	KifElement* Match(KifBaseVector* params, KifDomain* dom, int, KifCallFunction*);
	KifElement* MatchPattern(KifElement* params, KifDomain* dom, int, KifCallFunction*);

	void storevalue(long v) {
		KifContainerLock _lock(this);
		values.push_back(v);
	}


	bool Permute();
	void Shuffle();

	bool aValueContainer() {
		return true;
	}

	void storeInLoop(KifElement** alls);

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i = s->Integer();
		if (i < 0 || i >= values.size())
			return kifFALSE;
		return kifTRUE;
	}

	bool aVectorContainer() {
		return true;
	}

	void Reserve(int i) {
		KifContainerLock _lock(this);
		values.reserve(i);
	}

	Exported KifElement* Inverse() {
		KifVectorInteger* kvect = new KifVectorInteger(kifcode, NULL);
		KifContainerLock _lock(this);
		for (int i = values.size() - 1; i >= 0; i--)
			kvect->values.push_back(values[i]);
		return kvect;
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		for (int i = 0; i < values.size(); i++)
			v += values[i];
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return 0;
		double v = values[0];
		for (int i = 1; i < values.size(); i++)
			v *= values[i];
		return v;
	}

	Exported void Sort(bool direction);

	Exported void Insert(int idx, KifElement* ke) {
		values.insert(values.begin() + idx, ke->Integer());
	}

	Exported KifType theType() {
		return kifVectorInteger;
	}

	KifElement* Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, bool);
	KifElement* FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool);

	KifElement* ValueOnIndex(size_t ikey) {
		KifContainerLock _lock(this);
		if (ikey < 0 || ikey >= values.size())
			return kifNOELEMENT;
		return kifcode->Provideinteger(values[ikey]);
	}

	long getinteger(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return values[i];
	}

	double getfloat(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return values[i];
	}

	string getstring(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return "";
		char ch[20];
		sprintf_s(ch, 20, "%ld", values[i]);
		return ch;
	}

	wstring getustring(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return L"";
		wchar_t wch[20];
		swprintf_s(wch, 20, L"%ld", values[i]);
		return wch;
	}

	KifElement* Last() {
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return kifNULL;
		return kifcode->Provideinteger(values.back());
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifVectorInteger(kifcode, NULL);
	}

	string String();
	Exported void Push(KifElement*);

	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);

	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);


	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	KifElement* apply(KifCallFunctionGeneral* kfunc, KifElement* c, KifDomain* dom, KifCallFunction* callfunc);
};

class KifIteratorVectorInteger : public KifIterationContainer {
public:
	vector<long>* values;
	int i;
	KifIteratorVectorInteger(KifCode* klc, KifElement* kp, KifVectorInteger* k) : KifIterationContainer(klc, kp, k) {}


	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse == false) {
			if (i == values->size())
				return;
		}
		else
		if (i < 0)
			return;
		(*values)[i] = ke->Integer();
	}

	KifElement* IteratorKey() {
		return kifcode->Provideinteger(i);
	}

	long IteratorKeyInteger() {
		return i;
	}

	double IteratorKeyFloat() {
		return i;
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (i == values->size())
				return kifNULL;
		}
		else
		if (i < 0)
			return kifNULL;
		return kifcode->Provideinteger((*values)[i]);
	}

	long IteratorValueInteger() {
		if (reverse == false) {
			if (i == values->size())
				return 0;
		}
		else
		if (i < 0)
			return 0;
		return (*values)[i];
	}

	double IteratorValueFloat() {
		if (reverse == false) {
			if (i == values->size())
				return 0;
		}
		else
		if (i < 0)
			return 0;
		return (*values)[i];
	}

	void Next() {
		if (reverse == false)
			i++;
		else
			i--;
	}

	KifElement* End() {
		if (reverse == false) {
			if (i >= values->size())
				return kifTRUE;
		}
		else {
			if (i < 0)
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		i = values->size();
		if (i == 0)
			return kifFALSE;

		if (reverse == false)
			i = 0;
		else
			i--;

		return kifTRUE;
	}
};

class KifVectorFloat : public KifEvaluate {
public:
	vector<double> values;

	KifVectorFloat(KifCode* klc, KifElement* kp) : KifEvaluate(klc, kp, kifVectorFloat) {}

	bool Variable() {
		return true;
	}

	bool aValueVector() {
		return true;
	}

	void storevalue(double v) {
		KifContainerLock _lock(this);
		values.push_back(v);
	}

	char inCode() {
		return ckifFloat;
	}

	KifElement* Split(KifBaseVector* params, KifCallFunction*);
	KifElement* Match(KifBaseVector* params, KifDomain* dom, int, KifCallFunction*);
	KifElement* MatchPattern(KifElement* params, KifDomain* dom, int, KifCallFunction*);
	unsigned int EditDistance(KifElement*);


	KifElement* Unique();

	bool Permute();
	void Shuffle();

	void Merge(KifElement* ke);

	void storeInLoop(KifElement** alls);
	bool aValueContainer() {
		return true;
	}

	bool aVectorContainer() {
		return true;
	}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i = s->Integer();
		if (i < 0 || i >= values.size())
			return kifFALSE;
		return kifTRUE;
	}

	void Reserve(int i) {
		KifContainerLock _lock(this);
		values.reserve(i);
	}

	Exported KifElement* Inverse() {
		KifContainerLock _lock(this);
		KifVectorFloat* kvect = new KifVectorFloat(kifcode, NULL);
		for (int i = values.size() - 1; i >= 0; i--)
			kvect->values.push_back(values[i]);
		return kvect;
	}


	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		for (int i = 0; i < values.size(); i++)
			v += values[i];
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return 0;
		double v = values[0];
		for (int i = 1; i < values.size(); i++)
			v *= values[i];
		return v;
	}

	Exported KifType theType() {
		return kifVectorFloat;
	}

	Exported void Sort(bool direction);

	Exported void Insert(int idx, KifElement* ke) {
		KifContainerLock _lock(this);
		values.insert(values.begin() + idx, ke->Float());
	}

	KifElement* Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, bool);
	KifElement* FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool);

	KifElement* ValueOnIndex(size_t ikey) {
		KifContainerLock _lock(this);
		if (ikey < 0 || ikey >= values.size())
			return kifNOELEMENT;
		return kifcode->Providefloat(values[ikey]);
	}

	long getinteger(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return values[i];
	}

	double getfloat(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return values[i];
	}

	string getstring(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return "";
		char ch[20];
		sprintf_s(ch, 20, "%g", values[i]);
		return ch;
	}

	wstring getustring(int i) {
		KifContainerLock _lock(this);
		if (i < 0 || i >= values.size())
			return L"";
		wchar_t wch[20];
		swprintf_s(wch, 20, L"%g", values[i]);
		return wch;
	}

	KifElement* Last() {
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return kifNULL;
		return kifcode->Providefloat(values.back());
	}


	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifVectorFloat(kifcode, NULL);
	}

	string String();
	Exported void Push(KifElement*);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);

	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);


	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	KifElement* apply(KifCallFunctionGeneral* kfunc, KifElement* c, KifDomain* dom, KifCallFunction* callfunc);
};

class KifIteratorVectorFloat : public KifIterationContainer {
public:
	vector<double>* values;
	int i;
	KifIteratorVectorFloat(KifCode* klc, KifElement* kp, KifVectorFloat* k) : KifIterationContainer(klc, kp, k) {}


	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse == false) {
			if (i == values->size())
				return;
		}
		else
		if (i < 0)
			return;
		(*values)[i] = ke->Float();
	}

	KifElement* IteratorKey() {
		return kifcode->Provideinteger(i);
	}

	long IteratorKeyInteger() {
		return i;
	}

	double IteratorKeyFloat() {
		return i;
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (i == values->size())
				return kifNULL;
		}
		else
		if (i < 0)
			return kifNULL;
		return kifcode->Providefloat((*values)[i]);
	}

	long IteratorValueInteger() {
		if (reverse == false) {
			if (i == values->size())
				return 0;
		}
		else
		if (i < 0)
			return 0;
		return (*values)[i];
	}

	double IteratorValueFloat() {
		if (reverse == false) {
			if (i == values->size())
				return 0;
		}
		else
		if (i < 0)
			return 0;
		return (*values)[i];
	}

	void Next() {
		if (reverse == false)
			i++;
		else
			i--;
	}

	KifElement* End() {
		if (reverse == false) {
			if (i >= values->size())
				return kifTRUE;
		}
		else {
			if (i < 0)
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		i = values->size();
		if (i == 0)
			return kifFALSE;

		if (reverse == false)
			i = 0;
		else
			i--;

		return kifTRUE;
	}
};


//------------------------------------------------------------------------------------------------------
class KifTree : public KifElement {
public:

	KifTree* parent;
	KifTree* child;
	KifTree* next;
	KifTree* previous;
	KifTree* last;
	KifElement* value;
	int hight;

	KifTree(KifCode* klc, KifElement* kp, KifElement* v, KifType kt = kifTree) : parent(NULL), child(NULL), next(NULL), previous(NULL), last(NULL), value(v), hight(0), KifElement(klc, kp, kt) {}

	Exported long Size();
	long Computesize(KifTree* t);

	Exported void Flatten(KifVector* kvect, KifTree* t);
	Exported void Mapping(KifMap* kmap, KifTree* t);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);
	void TreeFromContainer(KifElement* k);

	bool Basic() {
		return true;
	}

	Exported void Setpopped(int v = 1, bool force = false);

	Exported int Depth() {
		return hight;
	}

	Exported KifElement* Copy(KifDomain*, KifElement* ke = kifNULL) {
		return new KifTree(kifcode, NULL, kifNULL);
	}

	unsigned int EditDistance(KifElement*);

	KifElement* Returnvalue() {
		return this;
	}

	Exported bool Setvalue(KifElement* k, KifElement* index = NULL, KifElement* dom = NULL);

	Exported KifElement* TreeChild() {
		if (child == NULL)
			return kifNULL;
		return child;
	}

	Exported KifElement* TreeNext()  {
		if (next == NULL)
			return kifNULL;
		return next;
	}
	Exported KifElement* TreePrevious() {
		if (previous == NULL)
			return kifNULL;
		return previous;
	}

	Exported KifElement* TreeParent() {
		if (parent == NULL)
			return kifNULL;
		return parent;
	}

	Exported KifElement* TreeLast() {
		if (last == NULL)
			return kifNULL;
		return last;
	}

	Exported KifElement* TreeAddchild(KifTree* t);
	Exported KifElement* TreeAddnext(KifTree* t);
	Exported KifElement* TreeAddprevious(KifTree* t);
	Exported KifElement* TreeRemove();
	Exported KifElement* TreeExtract(KifTree* with);
	Exported void SetHight(int seed);
	Exported KifElement* TreeValue() {
		return value;
	}

	Exported void Setreference(int inc = 1);
	Exported void Resetreference(int inc = 1);

	Exported string String() {
		return value->String();
	}
	Exported long Integer() {
		return value->Integer();
	}
	Exported double Float() {
		return value->Float();
	}
	Exported bool Boolean() {
		return value->Boolean();
	}

	Exported KifElement* same(KifElement* a) {
		if (a == kifNULL)
			return kifFALSE;
		if (a == this)
			return kifTRUE;
		if (a->type != kifTree)
			return value->same(a);
		return kifFALSE;
	}

	Exported KifElement* different(KifElement* a) {
		if (a == kifNULL)
			return kifTRUE;
		if (a == this)
			return kifFALSE;
		if (a->type != kifTree)
			return value->different(a);
		return kifTRUE;
	}

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);
	Exported KifElement* Newiterator(bool rev);
};

class KifIteratorTree : public KifIteration {
public:

	KifTree* tree;
	vector<KifTree*> flat;
	int idx;

	KifIteratorTree(KifCode* klc, KifElement* kp, KifTree* t = NULL) : idx(0), tree(t), KifIteration(klc, kp) {}

	Exported KifElement* IteratorKey();
	long IteratorKeyInteger();
	double IteratorKeyFloat();

	Exported KifElement* IteratorValue();
	Exported void Next();
	Exported KifElement* End();
	Exported KifElement* Begin();
	Exported void Flatten(KifTree*);
};

class KifTable : public KifEvaluate {
public:
	LockedThread* lock;
	KifElement** values;
	int size;
	bool functionloop;

	KifTable(KifCode* klc, KifElement* kp, KifType ty = kifTable) : KifEvaluate(klc, kp, ty) {
		functionloop = false;
		lock = NULL;
		values = NULL;
		size = 0;
	}

	Exported KifElement* constructor(KifDomain*, KifElement*, int, KifCallFunction*);
	virtual bool initialconstructor() { return true; }

	bool Variable() {
		return true;
	}

	void storeInLoop(KifElement** alls);
	void build(int);

	virtual void Merge(KifElement* ke);
	virtual KifElement* Unique();

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;
		return Copy(NULL);
	}

	KifElement* Index(KifElement* s) {

		long i = s->Integer();
		if (i < 0 || i >= size)
			return kifFALSE;
		return kifTRUE;
	}

	bool aVectorContainer() {
		return true;
	}

	virtual KifElement* Inverse() {
		KifTable* kvect = new KifTable(kifcode, NULL);
		kvect->build(size);

		for (int i = size - 1; i >= 0; i--)
			kvect->values[size - i - 1] = values[i];
		return kvect;
	}

	Exported double Sum() {

		double v = 0;
		for (int i = 0; i < size; i++)
			v += values[i]->Float();
		return v;
	}

	Exported double Product() {

		if (size == 0)
			return 0;
		double v = 0;
		bool first = true;
		for (int i = 0; i < size; i++) {
			if (first) {
				v = values[i]->Float();
				first = false;
			}
			else
				v *= values[i]->Float();
		}
		return v;
	}

	virtual void Insert(int idx, KifElement* ke);

	virtual KifType theType() {
		return kifTable;
	}

	KifElement* ValueOnIndex(size_t ikey) {

		if (ikey < 0 || ikey >= size)
			return kifNOELEMENT;
		return values[ikey];
	}

	long getinteger(int i) {

		if (i < 0 || i >= size)
			return 0;
		return values[i]->Integer();
	}

	double getfloat(int i) {

		if (i < 0 || i >= size)
			return 0;
		return values[i]->Float();
	}

	string getstring(int i) {

		if (i < 0 || i >= size)
			return 0;
		return values[i]->String();
	}

	wstring getustring(int i) {
		if (i < 0 || i >= size)
			return 0;
		return values[i]->UString();
	}

	virtual KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	virtual KifElement* Newinstance() {
		KifTable* ktab = new KifTable(kifcode, NULL);
		ktab->build(size);
		return ktab;
	}

	virtual string String();
	virtual string JSonString();
	virtual void Push(KifElement*);
	virtual void Pop(KifElement* ke);

	KifElement* Newiterator(bool);

	long Size() {
		return  size;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	virtual bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);

	void Clear() {

		for (int i = 0; i < size; i++) {
			if (values[i] != kifNULL) {
				values[i]->popped = 0;
				values[i]->Resetreference(reference + 1);
				values[i] = kifNULL;
			}
		}
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {

		return  size;
	}
	double Float() {

		return  size;
	}

	Exported void Resetreference(int inc = 1);
	Exported void Setreference(int inc = 1);

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself);

	~KifTable() {
		if (lock != NULL)
			delete lock;

		if (values != NULL)
			delete[] values;
	}
};

class KifTuple : public KifTable {
public:

	KifTuple(KifCode* klc, KifElement* kp) : KifTable(klc, kp, kifTuple) {}
	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);
	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	bool initialconstructor() { return false; }

	KifElement* Newinstance() {
		KifTuple* ktab = new KifTuple(kifcode, NULL);
		ktab->build(size);
		return ktab;
	}

	string String();
	string JSonString();

	void Push(KifElement*) {}
	void Pop(KifElement* ke) {}
	void Insert(int idx, KifElement* ke) {}
	void Merge(KifElement* ke) {}
	KifElement* Unique() { return this; }

	KifElement* Inverse() {
		KifTuple* kvect = new KifTuple(kifcode, NULL);
		kvect->build(size);

		for (int i = size - 1; i >= 0; i--)
			kvect->values[size - i - 1] = values[i];
		return kvect;
	}

	KifType theType() {
		return kifTuple;
	}

};


class KifIteratorTable : public KifIterationContainer {
public:
	int itx;
	int size;
	KifElement** values;

	KifIteratorTable(KifCode* klc, KifElement* kp, KifTable* k) : size(0), itx(0), KifIterationContainer(klc, kp, k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
		return kifcode->Provideinteger(itx);
	}

	long IteratorKeyInteger() {
		return itx;
	}

	double IteratorKeyFloat() {
		return itx;
	}

	Exported KifElement* IteratorValue() {
		if (itx >= size || itx < 0)
			return kifNOELEMENT;
		return values[itx];
	}

	Exported void Next() {
		if (reverse == false)
			itx++;
		else
			itx--;
	}

	Exported KifElement* End() {
		if (itx >= size || itx < 0)
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		if (size == 0)
			return kifFALSE;
		if (reverse == false)
			itx = 0;
		else
			itx = size - 1;
		return kifTRUE;
	}
};

class KifMap : public KifEvaluateMap {
public:

	hmap<string, KifElement*> values;

	void Items(KifVector* kvect) {
		hmap<string, KifElement*>::iterator it;
		KifMap* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifMap(kifcode, NULL);
			kval->push(it->first, it->second);
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	Exported KifMap(KifCode* klc, KifElement* kp) : KifEvaluateMap(klc, kp, kifMap) {}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern);

	void storeInLoop(KifElement** alls);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL) {
		return this;
	}

	bool aMapContainer() {
		return true;
	}

	int Garbager() {
		return kifMap;
	}


	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		char buffer[20];
		sprintf_s(buffer, 20, "%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end())
			return values[buffer];
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		char buffer[20];
		sprintf_s(buffer, 20, "%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end())
			return values[buffer];
		return kifNOELEMENT;
	}

	KifElement* Index(KifElement* ke) {
		KifContainerLock _lock(this);
		string s = ke->String();
		if (values.find(s) == values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Newinstance() {
		return new KifMap(kifcode, NULL);
	}
	Exported string String();
	Exported string JSonString();
	Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		hmap<string, KifElement*>::iterator iter = values.find(n);
		if (iter == values.end())
			return kifNOELEMENT;
		return iter->second;
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);
	Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		hmap<string, KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v += itx->second->Sum();
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		hmap<string, KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v *= itx->second->Product();
		return v;
	}

	Exported void Setreference(int inc = 1);

	Exported void Setpopped(int v = 1, bool force = false) {
		if (reference <= 0 || force) {
			Putpopped(v);
			hmap<string, KifElement*>::iterator itx;
			for (itx = values.begin(); itx != values.end(); itx++) {
				if (itx->second != NULL)
					itx->second->Setpopped(v, force);
			}
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		hmap<string, KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++) {
			if (itx->second != NULL)
				itx->second->Clean();
		}
		values.clear();
		if (reference <= 0)
			KifElement::Resetreference();
	}

	Exported void Resetreference(int inc = 1);
	Exported void Clear();


	Exported long Size() {
		KifContainerLock _lock(this);
		return values.size();
	}

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);

	Exported long Integer() {
		KifContainerLock _lock(this);
		return values.size();
	}
	Exported double Float() {
		KifContainerLock _lock(this);
		return values.size();
	}

	//To insert a value into a map through the API 
	Exported void push(string key, KifElement* val);
	Exported void Push(string& key, KifElement* val);
	Exported void Push(char* key, KifElement* val);
	Exported void Push(KifElement* key, KifElement* val);


	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);

};

class KifBaseMap : public KifElement {
public:
	bool canbereleased;
	vector<KifElement*> keys;
	vector<KifElement*> values;
	char typetocreate;
	char merge;

	bool Basevalue() {
		return true;
	}

	bool aContainer() {
		return true;
	}

	bool aMapContainer() {
		return true;
	}

	Exported KifElement*  Linkedvalue() { return this; }

	void Release();
	Exported KifElement* Atom(bool forced = false);

	//0 is a key string. 1 is an integer string. 2 is a float string
	KifBaseMap(KifCode* klc, KifElement* kp, bool cpr) : canbereleased(cpr), KifElement(klc, kp, kifBaseMap) {
		typetocreate = 0;
		merge = 0;
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);
	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return Map(dom);
	}

	//Predicate management
	bool Insertvalue(KifDomain* dom, KifElement* v, bin_hash<KifElement*>& kept);
	bool unified(KifDomain* dom, KifElement* a);
	bool unifiedDeclaration(KifDomain* dom);
	Exported KifElement* ExtractPredicateVariables(KifElement* contextualpattern, KifDomain* dom, KifElement* val, KifElement* e, bool root);
	Exported KifElement* EvaluePredicateVariables(KifElement* context, KifDomain* dom);
	KifElement* Getvalues(KifDomain* dom, bool duplicate);

	Exported void Setreference(int inc = 1);
	Exported void Resetreference(int inc = 1);

	string String();
};

class KifIteratorMap : public KifIterationContainer {
public:
	hmap<string, KifElement*>* values;
	hmap<string, KifElement*>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<string, KifElement*>::iterator itr;
#else
	hmap<string, KifElement*>::reverse_iterator itr;
#endif

	KifIteratorMap(KifCode* klc, KifElement* kp, KifMap* k) : KifIterationContainer(klc, kp, k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
		if (reverse == false) {
			if (it == values->end())
				return kifNOELEMENT;
			return kifcode->Providestringraw(it->first);
		}
		if (itr == values->Rend())
			return kifNOELEMENT;
		return kifcode->Providestringraw(itr->first);
	}

	long IteratorKeyInteger() {
		if (reverse == false) {
			if (it == values->end())
				return -1;
			return conversionintegerhexa(STR(it->first));
		}

		if (itr == values->Rend())
			return -1;

		return conversionintegerhexa(STR(itr->first));
	}

	double IteratorKeyFloat() {
		if (reverse == false) {
			if (it == values->end())
				return -1;
			return convertfloat(STR(it->first));
		}

		if (itr == values->Rend())
			return -1;

		return convertfloat(STR(itr->first));
	}

	string IteratorKeyString() {
		if (reverse == false) {
			if (it == values->end())
				return "";
			return it->first;
		}
		if (itr == values->Rend())
			return "";
		return itr->first;
	}

	Exported KifElement* IteratorValue() {
		if (reverse == false) {
			if (it == values->end())
				return kifNOELEMENT;
			return it->second;
		}
		if (itr == values->Rend())
			return kifNOELEMENT;
		return itr->second;
	}

	Exported void Next() {
		if (reverse == false)
			it++;
		else
			itr++;
	}

	Exported KifElement* End() {
		if (reverse == false) {
			if (it == values->end())
				return kifTRUE;
			return kifFALSE;
		}
		if (itr == values->Rend())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		if (reverse == false) {
			it = values->begin();
			if (it == values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr = values->Rbegin();
		if (itr == values->Rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifTreeMap : public KifEvaluateMap {
public:

	map<string, KifElement*> values;

	void Items(KifVector* kvect) {
		map<string, KifElement*>::iterator it;
		KifTreeMap* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifTreeMap(kifcode, NULL);
			kval->push(it->first, it->second);
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	Exported KifTreeMap(KifCode* klc, KifElement* kp) : KifEvaluateMap(klc, kp, kifTreeMap) {}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL) {
		return this;
	}

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern);

	bool aMapContainer() {
		return true;
	}

	int Garbager() {
		return kifMap;
	}


	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		char buffer[20];
		sprintf_s(buffer, 20, "%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end())
			return values[buffer];
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		char buffer[20];
		sprintf_s(buffer, 20, "%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end())
			return values[buffer];
		return kifNOELEMENT;
	}

	KifElement* Index(KifElement* ke) {
		KifContainerLock _lock(this);
		string s = ke->String();
		if (values.find(s) == values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Newinstance() {
		return new KifTreeMap(kifcode, NULL);
	}
	Exported string String();
	Exported string JSonString();
	Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		map<string, KifElement*>::iterator iter = values.find(n);
		if (iter == values.end())
			return kifNOELEMENT;
		return iter->second;
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);
	Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		map<string, KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v += itx->second->Sum();
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		map<string, KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v *= itx->second->Product();
		return v;
	}

	Exported void Setreference(int inc = 1);

	Exported void Setpopped(int v = 1, bool force = false) {
		if (reference <= 0 || force) {
			Putpopped(v);
			map<string, KifElement*>::iterator itx;
			for (itx = values.begin(); itx != values.end(); itx++) {
				if (itx->second != NULL)
					itx->second->Setpopped(v, force);
			}
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		map<string, KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++) {
			if (itx->second != NULL)
				itx->second->Clean();
		}
		values.clear();
		if (reference <= 0)
			KifElement::Resetreference();
	}

	Exported void Resetreference(int inc = 1);
	Exported void Clear();


	Exported long Size() {
		KifContainerLock _lock(this);
		return values.size();
	}

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);

	Exported long Integer() {
		KifContainerLock _lock(this);
		return values.size();
	}
	Exported double Float() {
		KifContainerLock _lock(this);
		return values.size();
	}

	//To insert a value into a map through the API 
	Exported void push(string key, KifElement* val);
	Exported void Push(string& key, KifElement* val);
	Exported void Push(char* key, KifElement* val);
	Exported void Push(KifElement* key, KifElement* val);

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);

};

class KifIteratorTreeMap : public KifIterationContainer {
public:
	map<string, KifElement*>* values;
	map<string, KifElement*>::iterator it;
	map<string, KifElement*>::reverse_iterator itr;


	KifIteratorTreeMap(KifCode* klc, KifElement* kp, KifTreeMap* k) : KifIterationContainer(klc, kp, k) {}


	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);
	Exported KifElement* IteratorKey() {
		if (reverse == false) {
			if (it == values->end())
				return kifNOELEMENT;
			return kifcode->Providestringraw(it->first);
		}
		if (itr == values->rend())
			return kifNOELEMENT;
		return kifcode->Providestringraw(itr->first);
	}

	long IteratorKeyInteger() {
		if (reverse == false) {
			if (it == values->end())
				return -1;
			return conversionintegerhexa(STR(it->first));
		}

		if (itr == values->rend())
			return -1;

		return conversionintegerhexa(STR(itr->first));
	}

	double IteratorKeyFloat() {
		if (reverse == false) {
			if (it == values->end())
				return -1;
			return convertfloat(STR(it->first));
		}

		if (itr == values->rend())
			return -1;

		return convertfloat(STR(itr->first));
	}

	string IteratorKeyString() {
		if (reverse == false) {
			if (it == values->end())
				return "";
			return it->first;
		}
		if (itr == values->rend())
			return "";
		return itr->first;
	}

	Exported KifElement* IteratorValue() {
		if (reverse == false) {
			if (it == values->end())
				return kifNOELEMENT;
			return it->second;
		}
		if (itr == values->rend())
			return kifNOELEMENT;
		return itr->second;
	}

	Exported void Next() {
		if (reverse == false)
			it++;
		else
			itr++;
	}

	Exported KifElement* End() {
		if (reverse == false) {
			if (it == values->end())
				return kifTRUE;
			return kifFALSE;
		}
		if (itr == values->rend())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		if (reverse == false) {
			it = values->begin();
			if (it == values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr = values->rbegin();
		if (itr == values->rend())
			return kifFALSE;
		return kifTRUE;
	}
};

//----------------------------------------------------------------------------------------
class KifCycleVector : public KifEvaluate {
public:

	KifElement* base;
	KifElement* value;
	bool repeat;

	KifCycleVector(KifCode* klc, KifElement* kp, KifElement* v, bool r) : KifEvaluate(klc, kp, kifCycleVector) {
		base = v;
		value = kifNULL;
		repeat = r;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);
	Exported KifElement* Newiterator(bool);

	Exported KifElement* Newinstance();

	bool Infinite() {
		return true;
	}
};

class KifIteratorCycleVector : public KifIterationContainer {
public:
	KifIteration* iter;
	KifCycleVector* cycle;

	KifIteratorCycleVector(KifCode* klc, KifElement* kp, KifCycleVector* k) : KifIterationContainer(klc, kp, k) {
		cycle = k;
		iter = (KifIteration*)cycle->value->Newiterator(0);
	}

	~KifIteratorCycleVector() {
		iter->Release();
		cycle->value->Release();
		cycle->value = kifNULL;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
		return iter->IteratorKey();
	}

	long IteratorKeyInteger() {
		return iter->IteratorKeyInteger();
	}

	double IteratorKeyFloat() {
		return iter->IteratorKeyFloat();
	}

	Exported KifElement* IteratorValue() {
		return iter->IteratorValue();
	}

	Exported void Next() {
		iter->Next();
	}

	Exported KifElement* End() {
		if (iter->End() == kifTRUE)
			iter->Begin();
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		return iter->Begin();
	}
};

class KifIteratorCycleElement : public KifIterationContainer {
public:
	KifCycleVector* cycle;
	int itx;

	KifIteratorCycleElement(KifCode* klc, KifElement* kp, KifCycleVector* k) : KifIterationContainer(klc, kp, k) {
		cycle = k;
		itx = 0;
	}

	~KifIteratorCycleElement() {
		cycle->value->Release();
		cycle->value = kifNULL;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
		return kifcode->Provideinteger(itx);
	}

	long IteratorKeyInteger() {
		return itx;
	}

	double IteratorKeyFloat() {
		return itx;
	}

	Exported KifElement* IteratorValue() {
		return cycle->value;
	}

	Exported void Next() {
		itx++;
	}

	Exported KifElement* End() {
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		itx = 0;
		return kifTRUE;
	}
};

class KifReplicateVector : public KifEvaluate {
public:

	KifElement* base;
	KifElement* nbbase;
	KifElement* value;
	KifElement* nb;

	KifReplicateVector(KifCode* klc, KifElement* kp, KifElement* v) : KifEvaluate(klc, kp, kifReplicateVector) {
		base = v;
		nbbase = kifNULL;
		value = kifNULL;
		nb = kifNULL;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);
	Exported KifElement* Newiterator(bool);

	Exported KifElement* Newinstance();
	KifElement* Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, bool);

	void Add(KifElement* v) {
		nbbase = v;
	}

	bool Infinite() {
		return true;
	}

	void Clear() {
		value->Release();
		value = kifNULL;
		nb->Release();
		nb = kifNULL;
	}
};

class KifIteratorReplicateElement : public KifIterationContainer {
public:
	KifReplicateVector* replicate;
	int itx;
	int mx;

	KifIteratorReplicateElement(KifCode* klc, KifElement* kp, KifReplicateVector* k) : KifIterationContainer(klc, kp, k) {
		replicate = k;
		itx = 0;
	}

	~KifIteratorReplicateElement() {
		replicate->Clear();
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
		return kifcode->Provideinteger(itx);
	}

	long IteratorKeyInteger() {
		return itx;
	}

	double IteratorKeyFloat() {
		return itx;
	}

	Exported KifElement* IteratorValue() {
		return replicate->value;
	}

	Exported void Next() {
		itx++;
	}

	Exported KifElement* End() {
		if (itx >= mx)
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		itx = 0;
		return kifTRUE;
	}
};

class KifSet : public KifEvaluate {
public:
	kifset values;

	KifSet(KifCode* klc, KifElement* kp) : KifEvaluate(klc, kp, kifSet) {}

	Exported void Setreference(int inc = 1);
	Exported void Resetreference(int inc = 1);

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	bool Variable() {
		return true;
	}

	void Merge(KifElement* ke);
	void storeInLoop(KifElement** alls);

	Exported KifType theType() {
		return kifSetString;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifSet(kifcode, NULL);
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		if (values.find(s) == values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		kifsetiterator it;
		for (it = values.begin(); it != values.end(); it++)
			v += (*it)->Float();
		return v;
	}


	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		kifsetiterator it;
		for (it = values.begin(); it != values.end(); it++)
			v *= (*it)->Float();
		return v;
	}


	string String();
	string JSonString();
	void Push(KifElement* e);
	void Pop(KifElement* ke) {
		KifContainerLock _lock(this);
		values.erase(ke);
	}

	KifElement* Newiterator(bool);


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);

	void Clear();
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
};

class KifIteratorSet : public KifIterationContainer {
public:
	kifset* values;
	kifsetiterator it;
	kifsetriterator rit;

	KifIteratorSet(KifCode* klc, KifElement* kp, KifSet* k) : KifIterationContainer(klc, kp, k) {
		values = &k->values;
	}

	KifElement* IteratorKey() {
		if (reverse)
			return *rit;
		return *it;
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (it == values->end())
				return kifNOELEMENT;
			return *it;
		}

		if (rit == values->rend())
			return kifNOELEMENT;
		return *rit;
	}

	void Next() {
		if (reverse == false)
			it++;
		else
			rit++;
	}

	KifElement* End() {
		if (reverse == false) {
			if (it == values->end())
				return kifTRUE;
		}
		else {
			if (rit == values->rend())
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {

		if (reverse == false)
			it = values->begin();
		else
			rit = values->rbegin();

		return kifTRUE;
	}
};

class KifSetString : public KifEvaluate {
public:
	std::set<string> values;

	KifSetString(KifCode* klc, KifElement* kp) : KifEvaluate(klc, kp, kifSetString) {}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	void storevalue(string v) {
		KifContainerLock _lock(this);
		values.insert(v);
	}

	char inCode() {
		return ckifString;
	}

	void storevalue(char* v) {
		KifContainerLock _lock(this);
		values.insert(v);
	}

	bool Variable() {
		return true;
	}

	void Merge(KifElement* ke);
	void storeInLoop(KifElement** alls);

	Exported KifType theType() {
		return kifSetString;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifSetString(kifcode, NULL);
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		if (values.find(s->String()) == values.end())
			return kifFALSE;
		return kifTRUE;
	}


	string String();
	string JSonString();
	Exported void Push(KifElement* e) {
		values.insert(e->String());
	}

	void Pop(KifElement* ke) {
		values.erase(ke->String());
	}

	KifElement* Newiterator(bool);


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);


	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
};

class KifIteratorSetString : public KifIterationContainer {
public:
	std::set<string>* values;
	std::set<string>::iterator it;
	std::set<string>::reverse_iterator rit;

	KifIteratorSetString(KifCode* klc, KifElement* kp, KifSetString* k) : KifIterationContainer(klc, kp, k) {
		values = &k->values;
	}

	KifElement* IteratorKey() {
		if (reverse)
			return kifcode->Providestringraw(*rit);
		return kifcode->Providestringraw(*it);
	}

	string IteratorKeyString() {
		if (reverse == false) {
			if (it == values->end())
				return "";
			return *it;
		}

		if (rit == values->rend())
			return "";
		return *rit;
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (it == values->end())
				return kifNOELEMENT;
			return kifcode->Providestringraw(*it);
		}

		if (rit == values->rend())
			return kifNOELEMENT;
		return kifcode->Providestringraw(*rit);
	}

	string IteratorValueString() {
		if (reverse == false) {
			if (it == values->end())
				return "";
			return *it;
		}

		if (rit == values->rend())
			return "";
		return *rit;
	}

	void Next() {
		if (reverse == false)
			it++;
		else
			rit++;
	}

	KifElement* End() {
		if (reverse == false) {
			if (it == values->end())
				return kifTRUE;
		}
		else {
			if (rit == values->rend())
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {

		if (reverse == false)
			it = values->begin();
		else
			rit = values->rbegin();

		return kifTRUE;
	}
};

class KifSetInteger : public KifEvaluate {
public:
	std::set<long> values;

	KifSetInteger(KifCode* klc, KifElement* kp) : KifEvaluate(klc, kp, kifSetInteger) {}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	void storevalue(long v) {
		KifContainerLock _lock(this);
		values.insert(v);
	}

	void storevalue(double v) {
		KifContainerLock _lock(this);
		values.insert((long)v);
	}

	char inCode() {
		return ckifInteger;
	}

	bool Variable() {
		return true;
	}

	void Merge(KifElement* ke);
	void storeInLoop(KifElement** alls);

	Exported KifType theType() {
		return kifSetInteger;
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		std::set<long>::iterator it;
		for (it = values.begin(); it != values.end(); it++)
			v += *it;
		return v;
	}


	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		std::set<long>::iterator it;
		for (it = values.begin(); it != values.end(); it++)
			v *= *it;
		return v;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifSetInteger(kifcode, NULL);
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		if (values.find(s->Integer()) == values.end())
			return kifFALSE;
		return kifTRUE;
	}


	string String();
	string JSonString();
	Exported void Push(KifElement* e) {
		values.insert(e->Integer());
	}

	void Pop(KifElement* ke) {
		values.erase(ke->Integer());
	}

	KifElement* Newiterator(bool);


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);


	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
};

class KifIteratorSetInteger : public KifIterationContainer {
public:
	std::set<long>* values;
	std::set<long>::iterator it;
	std::set<long>::reverse_iterator rit;

	KifIteratorSetInteger(KifCode* klc, KifElement* kp, KifSetInteger* k) : KifIterationContainer(klc, kp, k) {
		values = &k->values;
	}

	KifElement* IteratorKey() {
		if (reverse)
			return kifcode->Provideinteger(*rit);
		return kifcode->Provideinteger(*it);
	}

	long IteratorKeyInteger() {
		if (reverse == false) {
			if (it == values->end())
				return -1;
			return *it;
		}

		if (rit == values->rend())
			return -1;
		return *rit;
	}

	double IteratorKeyFloat() {
		if (reverse == false) {
			if (it == values->end())
				return -1;
			return *it;
		}

		if (rit == values->rend())
			return -1;
		return *rit;
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (it == values->end())
				return kifNOELEMENT;
			return kifcode->Provideinteger(*it);
		}

		if (rit == values->rend())
			return kifNOELEMENT;
		return kifcode->Provideinteger(*rit);
	}

	long IteratorValueInteger() {
		if (reverse == false) {
			if (it == values->end())
				return -1;
			return *it;
		}

		if (rit == values->rend())
			return -1;
		return *rit;
	}

	double IteratorValueFloat() {
		if (reverse == false) {
			if (it == values->end())
				return -1;
			return *it;
		}

		if (rit == values->rend())
			return -1;
		return *rit;
	}

	void Next() {
		if (reverse == false)
			it++;
		else
			rit++;
	}

	KifElement* End() {
		if (reverse == false) {
			if (it == values->end())
				return kifTRUE;
		}
		else {
			if (rit == values->rend())
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {

		if (reverse == false)
			it = values->begin();
		else
			rit = values->rbegin();

		return kifTRUE;
	}
};

class KifSetFloat : public KifEvaluate {
public:
	std::set<double> values;

	KifSetFloat(KifCode* klc, KifElement* kp) : KifEvaluate(klc, kp, kifSetFloat) {}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	void storevalue(long v) {
		KifContainerLock _lock(this);
		values.insert((double)v);
	}

	void storevalue(double v) {
		KifContainerLock _lock(this);
		values.insert(v);
	}

	char inCode() {
		return ckifInteger;
	}

	bool Variable() {
		return true;
	}

	void Merge(KifElement* ke);
	void storeInLoop(KifElement** alls);

	Exported KifType theType() {
		return kifSetFloat;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifSetFloat(kifcode, NULL);
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		if (values.find(s->Integer()) == values.end())
			return kifFALSE;
		return kifTRUE;
	}


	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		std::set<double>::iterator it;
		for (it = values.begin(); it != values.end(); it++)
			v += *it;
		return v;
	}


	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		std::set<double>::iterator it;
		for (it = values.begin(); it != values.end(); it++)
			v *= *it;
		return v;
	}

	string String();
	string JSonString();
	Exported void Push(KifElement* e) {
		values.insert(e->Integer());
	}

	void Pop(KifElement* ke) {
		values.erase(ke->Integer());
	}

	KifElement* Newiterator(bool);


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);


	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
};

class KifIteratorSetFloat : public KifIterationContainer {
public:
	std::set<double>* values;
	std::set<double>::iterator it;
	std::set<double>::reverse_iterator rit;

	KifIteratorSetFloat(KifCode* klc, KifElement* kp, KifSetFloat* k) : KifIterationContainer(klc, kp, k) {
		values = &k->values;
	}

	KifElement* IteratorKey() {
		if (reverse)
			return kifcode->Providefloat(*rit);
		return kifcode->Providefloat(*it);
	}

	long IteratorKeyInteger() {
		if (reverse == false) {
			if (it == values->end())
				return -1;
			return *it;
		}

		if (rit == values->rend())
			return -1;
		return *rit;
	}

	double IteratorKeyFloat() {
		if (reverse == false) {
			if (it == values->end())
				return -1;
			return *it;
		}

		if (rit == values->rend())
			return -1;
		return *rit;
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (it == values->end())
				return kifNOELEMENT;
			return kifcode->Providefloat(*it);
		}

		if (rit == values->rend())
			return kifNOELEMENT;
		return kifcode->Providefloat(*rit);
	}

	long IteratorValueInteger() {
		if (reverse == false) {
			if (it == values->end())
				return -1;
			return *it;
		}

		if (rit == values->rend())
			return -1;
		return *rit;
	}

	double IteratorValueFloat() {
		if (reverse == false) {
			if (it == values->end())
				return -1;
			return *it;
		}

		if (rit == values->rend())
			return -1;
		return *rit;
	}

	void Next() {
		if (reverse == false)
			it++;
		else
			rit++;
	}

	KifElement* End() {
		if (reverse == false) {
			if (it == values->end())
				return kifTRUE;
		}
		else {
			if (rit == values->rend())
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {

		if (reverse == false)
			it = values->begin();
		else
			rit = values->rbegin();

		return kifTRUE;
	}
};

//---------------------------------------------------------------------------
class KifSetUString : public KifEvaluate {
public:
	std::set<wstring> values;

	KifSetUString(KifCode* klc, KifElement* kp) : KifEvaluate(klc, kp, kifSetUString) {}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	void storevalue(wstring v) {
		KifContainerLock _lock(this);
		values.insert(v);
	}
	
	void storevalue(string v) {
		wstring ws;
		s_utf8_to_unicode(ws, STR(v));
		KifContainerLock _lock(this);
		values.insert(ws);
	}

	char inCode() {
		return ckifUString;
	}

	void storevalue(char* v) {
		wstring ws;
		s_utf8_to_unicode(ws, v);
		KifContainerLock _lock(this);
		values.insert(ws);
	}

	bool Variable() {
		return true;
	}

	void Merge(KifElement* ke);
	void storeInLoop(KifElement** alls);

	Exported KifType theType() {
		return kifSetUString;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifSetUString(kifcode, NULL);
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		if (values.find(s->UString()) == values.end())
			return kifFALSE;
		return kifTRUE;
	}

	wstring UString();
	string String();
	string JSonString();
	Exported void Push(KifElement* e) {
		values.insert(e->UString());
	}

	void Pop(KifElement* ke) {
		values.erase(ke->UString());
	}

	KifElement* Newiterator(bool);


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);


	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
};

class KifIteratorSetUString : public KifIterationContainer {
public:
	std::set<wstring>* values;
	std::set<wstring>::iterator it;
	std::set<wstring>::reverse_iterator rit;

	KifIteratorSetUString(KifCode* klc, KifElement* kp, KifSetUString* k) : KifIterationContainer(klc, kp, k) {
		values = &k->values;
	}

	KifElement* IteratorKey() {
		if (reverse)
			return kifcode->Provideustring(*rit);
		return kifcode->Provideustring(*it);
	}

	wstring IteratorKeyUString() {
		if (reverse == false) {
			if (it == values->end())
				return L"";
			return *it;
		}

		if (rit == values->rend())
			return L"";
		return *rit;
	}

	string IteratorKeyString() {
		string s;
		wstring ws;
		if (reverse == false) {
			if (it == values->end())
				return "";
			ws = *it;
			s_unicode_to_utf8(s, ws);
			return s;
		}

		if (rit == values->rend())
			return "";

		ws = *rit;
		s_unicode_to_utf8(s, ws);
		return s;
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (it == values->end())
				return kifNOELEMENT;
			return kifcode->Provideustring(*it);
		}

		if (rit == values->rend())
			return kifNOELEMENT;
		return kifcode->Provideustring(*rit);
	}

	wstring IteratorValueUString() {
		if (reverse == false) {
			if (it == values->end())
				return L"";
			return *it;
		}

		if (rit == values->rend())
			return L"";
		return *rit;
	}

	string IteratorValueString() {
		wstring ws;
		string s;
		if (reverse == false) {
			if (it == values->end())
				return "";
			ws = *it;
			s_unicode_to_utf8(s, ws);
			return s;
		}

		if (rit == values->rend())
			return "";
		ws = *rit;
		s_unicode_to_utf8(s, ws);
		return s;
	}

	void Next() {
		if (reverse == false)
			it++;
		else
			rit++;
	}

	KifElement* End() {
		if (reverse == false) {
			if (it == values->end())
				return kifTRUE;
		}
		else {
			if (rit == values->rend())
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {

		if (reverse == false)
			it = values->begin();
		else
			rit = values->rbegin();

		return kifTRUE;
	}
};

#endif
